#ifndef CAMERA_SOLVE_H
#define CAMERA_SOLVE_H

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <sensor_msgs/image_encodings.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <cv_bridge/cv_bridge.h>
#include <tf2/LinearMath/Matrix3x3.hpp>
#include <tf2/LinearMath/Quaternion.hpp>
#include <tf2/LinearMath/Transform.hpp>
#include <chrono>
#include <optional>
#include <std_srvs/srv/empty.hpp>

namespace CAMERA 
{
    class Solver : public rclcpp::Node {
        public:
            Solver();
            ~Solver();

            std::optional<geometry_msgs::msg::PoseStamped> ARUCO_solve(sensor_msgs::msg::Image data);

        private:

            rclcpp::Subscription<sensor_msgs::msg::CameraInfo>::SharedPtr camera_info_sub_;
            void camera_info_callback(const sensor_msgs::msg::CameraInfo::SharedPtr msg);

            rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_raw_sub_;
            void image_callback(const sensor_msgs::msg::Image::SharedPtr msg);

            rclcpp::TimerBase::SharedPtr solver_timer_;
            void solver_callback_();
            rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr pose_pub_;
            void solver_service_callback_(const std::shared_ptr<std_srvs::srv::Empty::Request> request, std::shared_ptr<std_srvs::srv::Empty::Response> response);
            rclcpp::Service<std_srvs::srv::Empty>::SharedPtr solver_service_;

            sensor_msgs::msg::Image latest_;
            std::mutex image_mutex_;
            std::mutex calibration_mutex_;
            cv::Mat camera_matrix;
            cv::Mat dist_coeffs;
            bool calibration_received = false;
            float plane_w;
            float plane_h;

            cv::Ptr<cv::aruco::GridBoard> board;
            cv::Ptr<cv::aruco::Dictionary> dictionary;
    };
}

#endif //CAMERA_SOLVE_H

