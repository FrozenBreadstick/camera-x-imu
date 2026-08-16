#include <camximu/simulation/ui.h>
#include <rclcpp/rclcpp.hpp>
#include <QApplication>
#include <thread>

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    QApplication app(argc, argv);

    auto ui_node = std::make_shared<UI::UI>();
    ui_node->show();

    std::thread ros_thread([ui_node]() {
        rclcpp::spin(ui_node);
    });

    int exec_code = app.exec();

    rclcpp::shutdown();
    ros_thread.join();

    return exec_code;
}