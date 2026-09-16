#include <camximu/solving/camera_solve.h>
#include <rclcpp/rclcpp.hpp>
#include <thread>

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto camera_node = std::make_shared<CAMERA::Solver>();
    rclcpp::spin(camera_node);

    return 0;
}