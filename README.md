# camera-x-imu

1. Clone Repository
```bash
git clone https://github.com/FrozenBreadstick/camera-x-imu
```

2. Symlink into ROS2 Workspace SRC
```bash
ln -s ~/git/{REPO_DIRECTORY}
```

3. Colcon build into ros2 workspace:
```bash
colcon build --symlink-install --packages-select camximu
```

4. launch simulation:
```bash
ros2 launch camximu sim.launch.py
```

5. Move robot from another terminal:
```bash
ros2 topic pub /position_controller/commands std_msgs/msg/Float64MultiArray "{data: [1.0,0.0,0.0]}"
```

A good position to observe aruco solves from:
```bash
ros2 topic pub /position_controller/commands std_msgs/msg/Float64MultiArray "{data: [0.15,-0.4,0.6]}"
```

You can get the solved pose in another terminal by running:
```bash
ros2 topic echo /camximu/aruco_pose
```
Before you click the SOLVE button in the UI

You can get the real pose of the camera by running:
```bash
ros2 run tf2_ros tf2_echo world camera_link
```

This can be used to compare the two poses