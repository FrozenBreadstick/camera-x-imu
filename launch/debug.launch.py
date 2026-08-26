from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():

    ld = LaunchDescription()

    ui = Node(
            package='camximu',
            executable='ui',
            output='screen',
        )
    ld.add_action(ui)
    
    return ld

