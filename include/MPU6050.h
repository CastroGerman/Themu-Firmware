#ifndef MPU6050_H_
#define MPU6050_H_

#include <stdint.h>

#define MPU6050_ADDRESS 0x68 // I2C address of MPU6050

/*
 * The following registers contain the primary data we are interested in
 * 0x3B MPU6050_ACCEL_XOUT_H
 * 0x3C MPU6050_ACCEL_XOUT_L
 * 0x3D MPU6050_ACCEL_YOUT_H
 * 0x3E MPU6050_ACCEL_YOUT_L
 * 0x3F MPU6050_ACCEL_ZOUT_H
 * 0x40 MPU6050_ACCEL_ZOUT_L
 * 0x41 MPU6050_TEMP_OUT_H
 * 0x42 MPU6050_TEMP_OUT_L
 * 0x43 MPU6050_GYRO_XOUT_H
 * 0x44 MPU6050_GYRO_XOUT_L
 * 0x45 MPU6050_GYRO_YOUT_H
 * 0x46 MPU6050_GYRO_YOUT_L
 * 0x47 MPU6050_GYRO_ZOUT_H
 * 0x48 MPU6050_GYRO_ZOUT_L
 */
#define MPU6050_ACCEL_XOUT_H    0x3B
#define MPU6050_GYRO_XOUT_H     0x43
#define MPU6050_PWR_MGMT_1      0x6B
#define MPU6050_GYRO_CONFIG     0x1B
#define MPU6050_ACCEL_CONFIG    0x1C

/*
*   AFS_SEL     Full Scale Range        LSB Sensitivity
*   0               +/- 2 g                 16384 LSB/g
*   1               +/- 4 g                 8192 LSB/g
*   2               +/- 8 g                 4096 LSB/g
*   3               +/- 16 g                2048 LSB/g
*/
#define AFS_SEL         0x0

/*
*   FS_SEL     Full Scale Range        LSB Sensitivity
*   0           +/- 250 º/s             131 LSB/(º/s)
*   1           +/- 500 º/s             65.5 LSB/(º/s)
*   2           +/- 1000 º/s            32.8 LSB/(º/s)
*   3           +/- 2000 º/s            16.4 LSB/(º/s)
*/
#define FS_SEL          0x3

#define TEMP_SCALE      (1/340.00f)
#define ACCEL_SCALE     (1/(16384.0f/(1<<(AFS_SEL))))
#define GYRO_SCALE      (1/(131.072f/(1<<(FS_SEL))))
#define DEG_TO_RAD      (3.14159265f/180)
#define RAD_TO_DEG      (1/DEG_TO_RAD)

#define MPU6050_8BITS_REGS      14
#define MPU6050_16BITS_REGS     (MPU6050_8BITS_REGS/2)
#define CAL_ITERATIONS          64

typedef enum
{
    accelX_H = 0,
    accelX_L,
    accelY_H,
    accelY_L,
    accelZ_H,
    accelZ_L,
    temp_H,
    temp_L,
    gyroX_H,
    gyroX_L,
    gyroY_H,
    gyroY_L,
    gyroZ_H,
    gyroZ_L 
}MPU6050_8BitsRegsIdx_t;

typedef enum 
{
    accelX = 0,
    accelY,
    accelZ,
    temp,
    gyroX,
    gyroY,
    gyroZ
}MPU6050_16BitsRegsIdx_t;

extern volatile double processedValues[MPU6050_16BITS_REGS];

void InitMPU6050 (void);
void readMPU6050Regs(uint8_t *_mpuRegs);
void getMPUValuesFromRegs(double *_values, uint8_t *_mpuRegs);
void processMPUValues(volatile double *_processed, double *_values, double *_offsets);
void takeOutGForceFromAccel(double *_MPU6050Values);
void getMPU6050Offset(double *_offsetValues);
void printValues(double *_values);
void tMPU6050 (void *pv);

#endif /* MPU6050_H_ */