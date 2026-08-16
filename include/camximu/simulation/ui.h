#ifndef UI_SAMPLE_H
#define UI_SAMPLE_H

#include <rclcpp/rclcpp.hpp>
#include <rcl_interfaces/srv/set_parameters.hpp>
#include <fstream>
#include <sstream>
#include <array>
#include <stdexcept>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QMessageBox>

namespace UI
{
    class UI : public QWidget, public rclcpp::Node {
        Q_OBJECT
        public:
            UI();
            ~UI();

        private:
            void start_simulation();

            rclcpp::SyncParametersClient::SharedPtr model_swapper_;

            QLabel* _label;
            QPushButton* _button;
    };
}

#endif // UI_SAMPLE_H