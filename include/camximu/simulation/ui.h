#ifndef UI_H
#define UI_H

#include <rclcpp/rclcpp.hpp>
#include <fstream>
#include <sstream>
#include <array>
#include <stdexcept>
#include <filesystem>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <std_srvs/srv/empty.hpp>

#include <camximu/template/aruco_template.h>

namespace UI
{
    class UI : public QWidget, public rclcpp::Node {
        Q_OBJECT
        public:
            UI();
            ~UI();

        private:
            void start_simulation();
            void make_aruco();
            std::string find_replace(std::string text, std::string find, std::string replace);
            bool write_file(std::string file_path, std::string file_contents);
            void camera_solver_send_();

            rclcpp::Client<std_srvs::srv::Empty>::SharedPtr camera_solver_client_;

            QLabel* camera_solver_label_;
            QPushButton* camera_solver_button_;

            QLabel* test_label_;
            QPushButton* test_button_;

            QLabel* aruco_maker_label_;
            QPushButton* aruco_maker_button_;
            QLineEdit* aruco_maker_namer_;
    };
}

#endif // UI_H