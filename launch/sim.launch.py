import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution, Command, FindExecutable
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():

    ld = LaunchDescription()

    robot_launch_arg = DeclareLaunchArgument(
            'robot',
            default_value='simple_one_dof',
            description='Which robot to load (without file extension)',
        )
    ld.add_action(robot_launch_arg)

    world_launch_arg = DeclareLaunchArgument(
            'world',
            default_value='simple',
            description='Which world to load (without file extension)',
        )
    ld.add_action(world_launch_arg)

    share_dir = get_package_share_directory("camximu")

    urdf_xacro = PathJoinSubstitution([share_dir, 'urdf', [LaunchConfiguration('robot'), '.urdf.xacro']])
    sdf_file = PathJoinSubstitution([share_dir, 'worlds', [LaunchConfiguration('world'), '.sdf']])
    rviz_conf = PathJoinSubstitution([share_dir, 'config', 'rviz_config.rviz'])

    robot_desc = Command([
        FindExecutable(name='xacro'), ' ', urdf_xacro
    ])

    gazebo_launch = IncludeLaunchDescription(
        PathJoinSubstitution([FindPackageShare('ros_ign_gazebo'),'launch', 'ign_gazebo.launch.py']),
        launch_arguments={'ign_args': [sdf_file, ' -r', ' -s']}.items()
    )
    ld.add_action(gazebo_launch)

    state_pub = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{
            'robot_description': robot_desc,
            'use_sim_time': True
        }]
    )
    ld.add_action(state_pub)

    gazebo_spawn = Node(
        package='ros_gz_sim',
        executable='create',
        arguments=[
            '-topic', 'robot_description',
            '-name', 'robot',
            '-z', '0.1'
        ],
        output='screen'
    )
    ld.add_action(gazebo_spawn)

    bridge = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        arguments=[
            '/clock@rosgraph_msgs/msg/Clock[ignition.msgs.Clock',
            '/imu@sensor_msgs/msg/Imu[ignition.msgs.IMU',
            '/camera/image_raw@sensor_msgs/msg/Image[ignition.msgs.Image',
            '/camera/camera_info@sensor_msgs/msg/CameraInfo[ignition.msgs.CameraInfo',
        ],
        output='screen'
    )
    ld.add_action(bridge)

    load_joint_state_broadcaster = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_state_broadcaster"],
        output="screen",
    )
    ld.add_action(load_joint_state_broadcaster)

    load_position_controller = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["position_controller"],
        output="screen",
    )
    ld.add_action(load_position_controller)

    rviz = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
        arguments=['-d', rviz_conf],
        parameters=[{'use_sim_time': True}]
    )
    ld.add_action(rviz)

    return ld

# ros2 topic pub /position_controller/commands std_msgs/msg/Float64MultiArray "{data: [1.0]}"