# SmartGlove

- [Stable version](https://github.com/CastroGerman/SmartGlove)

Embedded software development workspace for ESP32

# Test Quaternion
This test consists of getting a quaternion representation from [MPU6050](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf) measurements. This is possible thanks to Madgwick's filter.

## Why do we need this?
One of the main practical uses of quaternions is in how they describe 3D-rotation. 
Since there is no solution for gimbal lock problem in the 3D space, we have to move on to the 4D space and help ourselves with stereographic projections.

### Theory:
 - [WTF is a quaternion?](https://en.wikipedia.org/wiki/Quaternion)
 - [Roller coaster to the fourth dimension.](https://youtu.be/d4EgbgTm0Bg?t=1)
 - [Understanding 3D rotations using quaternions.](https://www.youtube.com/watch?v=zjMuIxRvygQ)
 - [Madgwick Algorithm](https://www.x-io.co.uk/res/doc/madgwick_internal_report.pdf)

### Explorable videos:
- [Stereographic projection in 2D, 3D and 4D.](https://eater.net/quaternions/)

## Running this test
Running this test will print in terminal a unit quaternion (a quaternion of norm one) at 5 Hz in the format shown below:
    
    Q = Q0*1 + Q1*i + Q2*j + Q3*k



