#include <camximu/simulation/mapping.h>

using namespace std::chrono_literals;

namespace MAPPING {
    Mapper::Mapper() : Node("mapper") {
        tracer_pub_ = this->create_publisher<nav_msgs::msg::Path>("/camximu/ee_tracer", 10);
        tracer_timer_ = this->create_wall_timer(100ms, std::bind(&Mapper::tracer_callback_, this));

        path_.header.frame_id = "world";
        prev_ee_ = *new geometry_msgs::msg::TransformStamped();

        tf_buffer_ = std::make_unique<tf2_ros::Buffer>(this->get_clock());
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
    }

    Mapper::~Mapper() {

    }

    void Mapper::tracer_callback_() {
        //Get TF Tree EE Position
        //Add to Path Array
        //Publish Path Array
        geometry_msgs::msg::TransformStamped current_ee_;

        try
        {
        current_ee_ = tf_buffer_->lookupTransform("world", "camera_link", tf2::TimePointZero); //Using camera link as EE
        }
        catch (const tf2::TransformException & ex)
        {
        RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 3000, "Could not get transform: %s", ex.what());
        return;
        }

        geometry_msgs::msg::Vector3 prev = prev_ee_.transform.translation;
        geometry_msgs::msg::Vector3 cur = current_ee_.transform.translation;
        double thresh = 0.02;

        if(
            abs(prev.x - cur.x) < thresh &&
            abs(prev.y - cur.y) < thresh &&
            abs(prev.z - cur.z) < thresh
        ) {
            RCLCPP_WARN_SKIPFIRST_THROTTLE(this->get_logger(), *this->get_clock(), 5000, "EE Movement Not Significant Enough.");
            return;
        }

        geometry_msgs::msg::PoseStamped pose;
        pose.header.frame_id = "world";
        pose.header.stamp = this->get_clock()->now();
        pose.pose.position.x = current_ee_.transform.translation.x;
        pose.pose.position.y = current_ee_.transform.translation.y;
        pose.pose.position.z = current_ee_.transform.translation.z;
        pose.pose.orientation = current_ee_.transform.rotation;

        path_.poses.push_back(pose);
        path_.header.stamp = this->get_clock()->now();
        tracer_pub_->publish(path_);
        prev_ee_ = current_ee_;
    }
}