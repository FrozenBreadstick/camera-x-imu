#ifndef MAPPING_H
#define MAPPING_H

#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/path.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <chrono>


namespace MAPPING {
    class Mapper : public rclcpp::Node {
        public:
            Mapper();
            ~Mapper();

        private:

            rclcpp::TimerBase::SharedPtr tracer_timer_;
            void tracer_callback_();
            rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr tracer_pub_;

            nav_msgs::msg::Path path_;
            geometry_msgs::msg::TransformStamped prev_ee_;
            std::unique_ptr<tf2_ros::Buffer> tf_buffer_;
            std::shared_ptr<tf2_ros::TransformListener> tf_listener_;

    };
}

#endif