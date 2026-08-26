cmake_minimum_required(VERSION 3.8)
project(camximu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

cmake_policy(SET CMP0071 NEW) #SHHHH Make QT be quiet about its MOC warnings
find_package(ament_cmake REQUIRED)

#general dependencies
find_package(geometry_msgs REQUIRED)
find_package(control_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(rclcpp REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

#ui dependencies
find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON) #For QT to process macros like Q_OBJECT

# custom messages
find_package(rosidl_default_generators REQUIRED)
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/StrStr.msg"

  # DEPENDENCIES geometry_msgs
)
rosidl_get_typesupport_target(custom_messages ${PROJECT_NAME} "rosidl_typesupport_cpp")

#Solver Libraries
add_library(imu_solve SHARED
  src/solving/imu_solve.cpp
)
target_include_directories(imu_solve PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(imu_solve PUBLIC
  sensor_msgs
  geometry_msgs
)
target_link_libraries(imu_solve PUBLIC 
  Eigen3::Eigen
)

add_library(camera_solve SHARED
  src/solving/camera_solve.cpp
)
target_include_directories(camera_solve PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(camera_solve PUBLIC
  sensor_msgs
  geometry_msgs
)
target_link_libraries(camera_solve PUBLIC 
  ${OpenCV_LIBS}
)

#UI Executable
add_executable(ui
  src/simulation/ui.cpp
  src/simulation/ui_main.cpp
  include/camximu/simulation/ui.h # MOC for QT skips files that are no explicitly listed, only need for UI executables
  include/camximu/template/aruco_template.h
)
target_include_directories(ui PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> # Can just be "include" if no QT in executable
  ${OpenCV_INCLUDE_DIRS}
)
ament_target_dependencies(ui PUBLIC
  rclcpp
  cv_bridge
)
target_link_libraries(ui PUBLIC
  Qt6::Widgets
  ${custom_messages}
  ${OpenCV_LIBRARIES}
)

#install executables to lib
install(
  TARGETS
    ui
  DESTINATION lib/${PROJECT_NAME}
)

#install data files to share
install(
  DIRECTORY launch models worlds urdf config
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_dependencies(rosidl_default_runtime)

ament_package()
