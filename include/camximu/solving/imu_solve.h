#ifndef IMU_SOLVE_H
#define IMU_SOLVE_H

#include <Eigen/Dense>
#include <sensor_msgs/msg/imu.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <vector>

namespace IMU 
{
    class IMU_Solver {
        public:
            IMU_Solver();
            ~IMU_Solver();

        private:

    };
}

#endif //IMU_SOLVE_H

