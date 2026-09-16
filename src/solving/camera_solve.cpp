#include <camximu/solving/camera_solve.h>

using namespace std::chrono_literals;

namespace CAMERA
{
    Solver::Solver() : Node("CameraSolver") {
        camera_info_sub_ = this->create_subscription<sensor_msgs::msg::CameraInfo>("/camera/camera_info", 10, std::bind(&Solver::camera_info_callback, this, std::placeholders::_1));
        image_raw_sub_ = this->create_subscription<sensor_msgs::msg::Image>("/camera/image_raw", 10, std::bind(&Solver::image_callback, this, std::placeholders::_1));
        pose_pub_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("/camximu/aruco_pose", 10);
        solver_timer_ = this->create_wall_timer(100ms, std::bind(&Solver::solver_callback_, this));
        solver_timer_->cancel(); //Temporary
        solver_service_ = this->create_service<std_srvs::srv::Empty>("/camximu/trigger_solve", std::bind(&Solver::solver_service_callback_, this, std::placeholders::_1, std::placeholders::_2));

        // [TODO] MAKE DYNAMIC AT SOME STAGE, PREDEFINED FOR NOW
        // Board size params
        int num_width = 6;
        int num_height = 4;
        float marker_length = 0.04f;
        float marker_spacing = 0.01f;
        plane_w = (num_width * marker_length) + ((num_width - 1) * marker_spacing);
        plane_h = (num_height * marker_length) + ((num_height - 1) * marker_spacing);
        dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
        board = cv::aruco::GridBoard::create(num_width, num_height, marker_length, marker_spacing, dictionary);
    }

    Solver::~Solver() {
        
    }

    std::optional<geometry_msgs::msg::PoseStamped> Solver::ARUCO_solve(sensor_msgs::msg::Image data) {

        //[TODO] MAKE DYNAMIC, HARDCODED TO MATCH simple.sdf WORLD FILE
        tf2::Quaternion board_rotation;
        board_rotation.setRPY(1.57079632679, 0.0, -1.57079632679);
        tf2::Vector3 local_offset(-plane_w/2, -plane_h/2, 0.0); //corner relative to model-origin/center
        tf2::Transform world_T_modelorigin(board_rotation, tf2::Vector3(2.0, 0.0, 0.2)); //from SDF
        tf2::Transform world_T_board = world_T_modelorigin * tf2::Transform(tf2::Quaternion::getIdentity(), local_offset);

        geometry_msgs::msg::PoseStamped pose;

        std::lock_guard<std::mutex> lock(calibration_mutex_);
        if (!calibration_received) {
            RCLCPP_WARN(this->get_logger(), "No camera calibration yet, skipping");
            return std::nullopt;
        }

        //Convert ROS Image to OpenCV Mat
        cv_bridge::CvImagePtr cv_ptr;
        try {
            cv_ptr = cv_bridge::toCvCopy(data, sensor_msgs::image_encodings::BGR8);
        } catch (cv_bridge::Exception &e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
            return std::nullopt;
        }
        cv::Mat image = cv_ptr->image;
        if (image.empty()) {
            RCLCPP_ERROR(this->get_logger(), "Image is empty");
            return std::nullopt;
        }

        //For marker detection
        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners, rejected;
        cv::aruco::detectMarkers(image, dictionary, corners, ids);

        if (ids.empty()) {
            RCLCPP_ERROR(this->get_logger(), "Ids are empty");
            return std::nullopt;  //nothing detected this frame
        }

        // Board pose from whichever markers were found
        cv::Vec3d rvec, tvec;
        int num_used = cv::aruco::estimatePoseBoard(
            corners, ids, board, camera_matrix, dist_coeffs, rvec, tvec);

        if (num_used <= 0) {
            RCLCPP_ERROR(this->get_logger(), "Board pose estimate is none");
            return std::nullopt;  //no markers matched the board layout
        }

        cv::Mat rot_mat;
        cv::Rodrigues(rvec, rot_mat);
        tf2::Matrix3x3 tf_rot(
            rot_mat.at<double>(0,0), rot_mat.at<double>(0,1), rot_mat.at<double>(0,2),
            rot_mat.at<double>(1,0), rot_mat.at<double>(1,1), rot_mat.at<double>(1,2),
            rot_mat.at<double>(2,0), rot_mat.at<double>(2,1), rot_mat.at<double>(2,2));
        tf2::Transform camera_T_board(tf_rot, tf2::Vector3(tvec[0], tvec[1], tvec[2]));

        // world_T_camera = world_T_board * board_T_camera
        tf2::Transform world_T_camera = world_T_board * camera_T_board.inverse();

        tf2::Vector3 t = world_T_camera.getOrigin();
        tf2::Quaternion q = world_T_camera.getRotation();

        pose.pose.position.x = t.x();
        pose.pose.position.y = t.y();
        pose.pose.position.z = t.z();
        pose.pose.orientation.x = q.x();
        pose.pose.orientation.y = q.y();
        pose.pose.orientation.z = q.z();
        pose.pose.orientation.w = q.w();

        pose.header.frame_id = "world";
        pose.header.stamp = this->now();

        return pose;
    }
    
    void Solver::camera_info_callback(const sensor_msgs::msg::CameraInfo::SharedPtr msg) {
        std::lock_guard<std::mutex> lock(calibration_mutex_);
        if(calibration_received) return;
        camera_matrix = cv::Mat(3, 3, CV_64F, const_cast<double*>(msg->k.data())).clone();
        dist_coeffs = cv::Mat(msg->d).clone();
        calibration_received = true;
    }

    void Solver::image_callback(const sensor_msgs::msg::Image::SharedPtr msg) {
        std::lock_guard<std::mutex> lock(image_mutex_);
        latest_ = *msg;
    }

    void Solver::solver_callback_() {
        sensor_msgs::msg::Image latest;
        {
            std::lock_guard<std::mutex> lock(image_mutex_);
            latest = latest_;
        }
        RCLCPP_INFO(this->get_logger(), "Beginning Solve");
        std::optional<geometry_msgs::msg::PoseStamped> pose = ARUCO_solve(latest);
        RCLCPP_INFO(this->get_logger(), "Publishing if value");
        if(pose.has_value()) pose_pub_->publish(pose.value());
    }

    void Solver::solver_service_callback_(const std::shared_ptr<std_srvs::srv::Empty::Request>, std::shared_ptr<std_srvs::srv::Empty::Response>)
    {
        RCLCPP_INFO(this->get_logger(), "Camera Solve Triggered");
        solver_callback_();
    }
}