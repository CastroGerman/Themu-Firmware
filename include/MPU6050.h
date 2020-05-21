#ifndef MPU6050_H_
#define MPU6050_H_


#define CAL_ITERATIONS  64
#define AFS_SEL         0x0
#define FS_SEL          0x3

#define TEMP_SCALE      (1/340.00f)
#define ACCEL_SCALE     (1/(16384.0f/(1<<(AFS_SEL))))
#define GYRO_SCALE      (1/(131.072f/(1<<(FS_SEL))))
#define DEG_TO_RAD      (3.14159265f/180)

#define ACCEL_X_OFF (800.0f/ACCEL_SCALE/16384.0f)
#define ACCEL_Y_OFF (100.0f/ACCEL_SCALE/16384.0f)
#define ACCEL_Z_OFF (3400.0f/ACCEL_SCALE/16384.0f)


#endif /* MPU6050_H_ */