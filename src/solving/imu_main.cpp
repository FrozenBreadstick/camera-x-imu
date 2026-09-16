#include <camximu/solving/imu_solve.h>
#include <rclcpp/rclcpp.hpp>
#include <thread>

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto imu_node = std::make_shared<IMU::Solver>();
    rclcpp::spin(imu_node);

    return 0;
}