// Include necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rtos_api_lib.h"
#include <signal.h>

// Define constants
#define PI 3.14159265

// Define struct for storing sensor data
typedef struct {
    double ax, ay, az; // Accelerometer data
    double gx, gy, gz; // Gyroscope data
    double mx, my, mz; // Magnetometer data
} SensorData;

// Define struct for storing Kalman filter variables
typedef struct {
    double qAngle, qGyro, rAngle; // Tuning variables
    double angle, bias; // State variables
    double rate; // Unbiased rate
    double P[2][2]; // Error covariance matrix
} Kalman;

// Function to initialize Kalman filter
void initKalman(Kalman* filter) {
    filter->qAngle = 0.001;
    filter->qGyro = 0.003;
    filter->rAngle = 0.03;
    filter->angle = 0;
    filter->bias = 0;
    filter->rate = 0;
    filter->P[0][0] = 0;
    filter->P[0][1] = 0;
    filter->P[1][0] = 0;
    filter->P[1][1] = 0;
}

// Function to update Kalman filter
double updateKalman(Kalman* filter, double newAngle, double newRate, double dt) {
    // Predict
    filter->rate = newRate - filter->bias;
    filter->angle += dt * filter->rate;
    filter->P[0][0] += dt * (dt*filter->P[1][1] - filter->P[0][1] - filter->P[1][0] + filter->qAngle);
    filter->P[0][1] -= dt * filter->P[1][1];
    filter->P[1][0] -= dt * filter->P[1][1];
    filter->P[1][1] += dt * filter->qGyro;

    // Update
    double y = newAngle - filter->angle;
    double S = filter->P[0][0] + filter->rAngle;
    double K[2];
    K[0] = filter->P[0][0] / S;
    K[1] = filter->P[1][0] / S;
    filter->angle += K[0] * y;
    filter->bias += K[1] * y;
    filter->P[0][0] -= K[0] * filter->P[0][0];
    filter->P[0][1] -= K[0] * filter->P[0][1];
    filter->P[1][0] -= K[1] * filter->P[0][0];
    filter->P[1][1] -= K[1] * filter->P[0][1];

    return filter->angle;
}

// Function to convert sensor data to Euler angles
void getEulerAngles(SensorData* data, double* roll, double* pitch, double* yaw) {
    // Convert accelerometer data to roll and pitch
    *roll = atan2(data->ay, data->az) * 180 / PI;
    *pitch = atan2(-data->ax, sqrt(data->ay*data->ay + data->az*data->az)) * 180 / PI;

    // Convert magnetometer data to yaw
    //double mx = data->mx * cos(*pitch) + data->my * sin(*roll) * sin(*pitch) + data->mz * cos(*roll) * sin(*pitch);
    //double my = data->my * cos(*roll) - data->mz * sin(*roll);
    *yaw = 0;//atan2(-my, mx) * 180 / PI;

    // Ensure yaw is between 0 and 360 degrees
    if (*yaw < 0) {
        *yaw += 360;
    }
}

#if 0
// Function to read sensor data and return Euler angles
void getSensorData(double* roll, double* pitch, double* yaw) {
    // Read sensor data
    SensorData data;
    // ${INSERT_HERE}

    // Initialize Kalman filter
    Kalman filter;
    initKalman(&filter);

    // Loop to read sensor data and update Euler angles
    while (1) {
        // ${INSERT_HERE}

        // Get time difference since last loop iteration
        double dt = (double)(clock() - prevTime) / CLOCKS_PER_SEC;
        prevTime = clock();

        // Get unbiased rate from gyroscope data
        double gyroRateX = data.gx - filter.bias;
        double gyroRateY = data.gy - filter.bias;
        double gyroRateZ = data.gz - filter.bias;

        // Get Euler angles using Kalman filter
        double newRoll, newPitch, newYaw;
        getEulerAngles(&data, &newRoll, &newPitch, &newYaw);
        *roll = updateKalman(&filter, newRoll, gyroRateX, dt);
        *pitch = updateKalman(&filter, newPitch, gyroRateY, dt);
        *yaw = updateKalman(&filter, newYaw, gyroRateZ, dt);
    }
}
#endif

static float accel_convert(unsigned int value)
{
    int val = value;

    if (value > 0x7fff) {
        val = -(0xffff - value);
    }
    return val * 9.8 / (0x8000 / 16);
}

static float gyro_convert(unsigned int value)
{
    int val = value;

    if (value > 0x7fff) {
        val = -(0xffff - value);
    }
    return val * 2000 * 1.0 / 0x7fff;
}

static int running = 1;

static void signal_handler(int signal)
{
    if (running == 1) {
        running = 0;
        return;
    }
    printf("Application exit by signal:%d\n", signal);
    exit(1);
}

static void capture_all_signal(void)
{
    int i = 0;
    for(i = 0; i < 32; i ++) {
        if ( (i == SIGPIPE) || (i == SIGCHLD) || (i == SIGALRM) || (i == SIGPROF)) {
            signal(i, SIG_IGN);
        } else {
            signal(i, signal_handler);
        }
    }
}

int main(int argc, char **argv)
{
    mmapInfo_s map;
    stream_share_info_s share_info;
    imu_share_frame_data_s *fifo;
    int frame_num = 0;
    SensorData data;
    double roll, pitch, yaw, ax, ay, az, gx, gy, gz;
    Kalman filter;
    unsigned char first = 1;
	double pre_time = 0.0;

    capture_all_signal();

    initKalman(&filter);
    rtos_api_lib_get_imu_fifo_share_info(&share_info);
    fifo = (imu_share_frame_data_s *)rtos_api_lib_convert_memory2linux(share_info, &map);
    //in your app, you can loop call rtos_api_lib_get_imu_fifo_data
    //0-3: seconds
    //4-7: useconds
    //8-11: sensor time
    //12-13: accel x [high low]
    //14-15: accel y [high low]
    //16-17: accel z [high low]
    //18-19: gyro x [high low]
    //20-21: gyro y [high low]
    //22-23: gyro z [high low]
    //24-25: temp [high low]
    while (running) {
        rtos_api_lib_get_imu_fifo_data(200, &frame_num);
        //printf("frame_num: %d\n", frame_num);
        if (frame_num > 0) {
            int i = 0;
            for (i = 0; i < frame_num; i++) {
                unsigned int seconds = 0, useconds = 0;
                seconds = (fifo[i].data[3] << 24) | (fifo[i].data[2] << 16) | (fifo[i].data[2] << 8) | (fifo[i].data[0]);
                useconds = (fifo[i].data[7] << 24) | (fifo[i].data[6] << 16) | (fifo[i].data[5] << 8) | (fifo[i].data[4]);
                if (first) {
                    first = 0;
                    pre_time = seconds + useconds / 1000000;
                    continue;
                }
                ax = accel_convert((fifo[i].data[12] << 8) | fifo[i].data[13]);
                ay = accel_convert((fifo[i].data[14] << 8) | fifo[i].data[15]);
                az = accel_convert((fifo[i].data[16] << 8) | fifo[i].data[17]);
                gx = gyro_convert((fifo[i].data[18] << 8) | fifo[i].data[19]);
                gy = gyro_convert((fifo[i].data[20] << 8) | fifo[i].data[21]);
                gz = gyro_convert((fifo[i].data[22] << 8) | fifo[i].data[23]);

                data.ax = -az;
                data.ay = ax;
                data.az = ay;
                data.gx = -gz;
                data.gy = gx;
                data.gz = gy;

                double gyroRateX = data.gx - filter.bias;
                double gyroRateY = data.gy - filter.bias;
                //double gyroRateZ = data.gz - filter.bias;

                double cur_time = seconds + useconds / 1000000;
                double dt = cur_time - pre_time;

                getEulerAngles(&data, &roll, &pitch, &yaw);
                roll = updateKalman(&filter, roll, gyroRateX, dt);
                pitch = updateKalman(&filter, pitch, gyroRateY, dt);
                printf("%d.%06d accel: %.2f %.2f %.2f gyro:%.2f %.2f %.2f, roll: %.2f, pitch: %.2f, yaw: %.2f\n", seconds, useconds,
                                    data.ax, data.ay, data.az,
                                    data.gx, data.gy, data.gz,
                                    roll, pitch, yaw);
            }
        }
        usleep(1000);
    }
    //when your app exit, you need call rtos_api_lib_munmap
    rtos_api_lib_munmap(&map);
    printf("[%s]Application exit\n", argv[0]);

    return 0;
}
