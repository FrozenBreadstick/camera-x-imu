#include <camximu/simulation/ui.h>

namespace UI
{
    UI::UI() : QWidget(), Node("UI") {
        RCLCPP_INFO(this->get_logger(), "UI Started");

        model_swapper_ = std::make_shared<rclcpp::SyncParametersClient>(this, "robot_state_publisher");

        auto* layout = new QVBoxLayout(this);
        _label = new QLabel("Swap URDF", this);
        _button = new QPushButton("Click Here", this);

        layout->addWidget(_label);
        layout->addWidget(_button);
        connect(_button, &QPushButton::clicked, this, &UI::start_simulation);
    }

    UI::~UI() {

    }

    void UI::start_simulation() {
        QMessageBox::information(this, "Info", "Simulation Yay!");
    }
}