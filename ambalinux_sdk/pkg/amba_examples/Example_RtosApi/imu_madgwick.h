#ifndef _IMU_MADGWICK_H_
#define _IMU_MADGWICK_H_

void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy,
                              float gz, float mx, float my, float mz,
                              float deltat);
void MahonyQuaternionUpdate(float ax, float ay, float az, float gx, float gy,
                            float gz, float mx, float my, float mz,
                            float deltat);
const float * getQ();

#endif // _IMU_MADGWICK_H_
