#include <camximu/simulation/ui.h>

namespace UI
{
    UI::UI() : QWidget(), Node("ui") {
        RCLCPP_INFO(this->get_logger(), "UI Started");

        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint); //Always on top

        auto* layout = new QVBoxLayout(this);

        // Test Label + Button ___
        test_label_ = new QLabel("Test", this);
        test_button_ = new QPushButton("Click Here", this);
        layout->addWidget(test_label_);
        layout->addWidget(test_button_);
        connect(test_button_, &QPushButton::clicked, this, &UI::start_simulation);
        // Test Label + Button ^^^

        // Aruco Label + Button ___
        aruco_maker_label_ = new QLabel("Create & Save Aurco Board", this);
        aruco_maker_button_ = new QPushButton("Make Aruco", this);
        aruco_maker_namer_ = new QLineEdit(this);
        layout->addWidget(aruco_maker_label_);
        layout->addWidget(aruco_maker_namer_);
        layout->addWidget(aruco_maker_button_);
        connect(aruco_maker_button_, &QPushButton::clicked, this, &UI::make_aruco);
        aruco_maker_namer_->setPlaceholderText("Enter Aruco Name...");
        // Aruco Label + Button ^^^
    }

    UI::~UI() {

    }

    void UI::start_simulation() {
        QMessageBox::information(this, "Info :)", "!#! WOW !#!");
    }

    void UI::make_aruco() {
        // Pick file path
        QString file_path = QFileDialog::getExistingDirectory(this, "Select Save Folder", QDir::homePath(), QFileDialog::ShowDirsOnly);        
        if (file_path.isEmpty()) return;
        RCLCPP_INFO(this->get_logger(), "Folder Picked");

        // Board size params
        int num_width = 6;
        int num_height = 4;
        float marker_length = 0.04f;
        float marker_spacing = 0.01f;
        float plane_w = (num_width * marker_length) + ((num_width - 1) * marker_spacing);
        float plane_h = (num_height * marker_length) + ((num_height - 1) * marker_spacing);
        int image_w = std::round(plane_w * 10000);
        int image_h = std::round(plane_h * 10000);

        // Make aruco board
        cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
        cv::Ptr<cv::aruco::GridBoard> board = cv::aruco::GridBoard::create(num_width, num_height, marker_length, marker_spacing, dictionary);
        cv::Mat boardImage;
        board->draw(cv::Size(image_w, image_h), boardImage);
        RCLCPP_INFO(this->get_logger(), "Aruco Made");

        // File path using textbox name
        std::string aruco_name = aruco_maker_namer_->text().toStdString();
        std::string file_path_2 = file_path.toStdString() + "/aruco_" + aruco_name + "/";
        std::string file_name = "aruco_" + aruco_name + ".png";
        std::filesystem::create_directory(file_path_2);
        cv::imwrite(file_path_2 + file_name, boardImage);
        RCLCPP_INFO(this->get_logger(), "Aruco Saved");

        // Edit templates
        float half_w = plane_w / 2.0f;
        float neg_half_w = -half_w;
        float half_h = plane_h / 2.0f;
        float neg_half_h = -half_h;

        std::string model_dae = find_replace(Templates::ARUCO_MODEL_DAE, "{HALF_W}", std::to_string(half_w));
        model_dae = find_replace(model_dae, "{NEG_HALF_W}", std::to_string(neg_half_w));
        model_dae = find_replace(model_dae, "{HALF_H}", std::to_string(half_h));
        model_dae = find_replace(model_dae, "{NEG_HALF_H}", std::to_string(neg_half_h));
        model_dae = find_replace(model_dae, "{IMAGE_PATH}", file_name);
        
        std::string model_sdf = find_replace(Templates::ARUCO_MODEL_SDF, "{X}", aruco_name);
        std::string model_config = find_replace(Templates::ARUCO_MODEL_CONFIG, "{X}", aruco_name);
        RCLCPP_INFO(this->get_logger(), "Dae, Sdf, Config Made");
        
        write_file(file_path_2 + "model.dae", model_dae);
        write_file(file_path_2 + "model.sdf", model_sdf);
        write_file(file_path_2 + "model.config", model_config);
        RCLCPP_INFO(this->get_logger(), "Aruco Saved");
    }

    std::string UI::find_replace(std::string text, std::string find, std::string replace) {
        size_t pos = text.find(find);
        while (pos != std::string::npos) {
            text.replace(pos, find.size(), replace);
            pos = text.find(find, pos + replace.size());
        }
        return text;
    }

    bool UI::write_file(std::string file_path, std::string file_contents) {
        std::ofstream file(file_path);
        if (!file.is_open()) {
            QMessageBox::information(this, "IO Error", QString::fromStdString("Couldn't open " + file_path));
            return false;
        }
        file << file_contents;
        file.close();
        return true;
    }
}