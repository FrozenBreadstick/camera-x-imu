#include <camximu/simulation/mapping.h>
#include <rclcpp/rclcpp.hpp>
#include <thread>

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto mapper_node = std::make_shared<MAPPING::Mapper>();
    rclcpp::spin(mapper_node);

    return 0;
}