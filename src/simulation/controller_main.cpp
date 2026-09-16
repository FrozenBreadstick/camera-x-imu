#include <camximu/simulation/dynamic_controller.h>
#include <rclcpp/rclcpp.hpp>
#include <thread>

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto controller_node = std::make_shared<CONTROLLER::DynamicController>();
    rclcpp::spin(controller_node);

    return 0;
}