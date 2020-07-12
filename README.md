# SmartGlove

- [Stable version](https://github.com/CastroGerman/SmartGlove)

Embedded software development workspace for ESP32

# Test Euler Angles
This test consists of getting Euler angles (roll, pitch and yaw) from [MPU6050](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf). 

Since the MPU6050 got a 3-axis accelerometer and a 3-axis gyroscope, its possible to implement a complementary filter to get roll and pitch, except yaw, that is got only from GYRO_ZOUT.
Working with Euler angles delivers in a problem known as [`Gimbal Lock`](https://www.youtube.com/watch?v=zc8b2Jo7mno&) which cannot be avoided.


## Running this test
Running this test will print in terminal the values in degree for roll, pitch and yaw at 5 Hz.



