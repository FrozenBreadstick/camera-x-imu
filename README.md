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
ros2 topic pub /position_controller/commands std_msgs/msg/Float64MultiArray "{data: [1.0]}"
```