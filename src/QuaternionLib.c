#include "QuaternionLib.h"
#include <math.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

qPolarForm_t hamilton2polar (qHamiltonForm_t hamiltonForm)
{
    qPolarForm_t polarForm;
    polarForm.ang = acos(hamiltonForm.q0);
    polarForm.i = hamiltonForm.q1/sin(polarForm.ang);
    polarForm.j = hamiltonForm.q2/sin(polarForm.ang);
    polarForm.k = hamiltonForm.q3/sin(polarForm.ang);
    return polarForm;
}

qHamiltonForm_t polar2hamilton (qPolarForm_t polarForm)
{
    qHamiltonForm_t hamiltonForm;
    hamiltonForm.q0 = cos(polarForm.ang);
    hamiltonForm.q1 = polarForm.i*sin(polarForm.ang);
    hamiltonForm.q2 = polarForm.j*sin(polarForm.ang);
    hamiltonForm.q3 = polarForm.k*sin(polarForm.ang);
    return hamiltonForm;
}

vector_t rotateVector(vector_t vector, qHamiltonForm_t hamiltonForm)
{
    vector_t rotated;
    double r11 = hamiltonForm.q0*hamiltonForm.q0+hamiltonForm.q1*hamiltonForm.q1-hamiltonForm.q2*hamiltonForm.q2-hamiltonForm.q3*hamiltonForm.q3;
    double r12 = 2*(hamiltonForm.q1*hamiltonForm.q2-hamiltonForm.q3*hamiltonForm.q0);
    double r13 = 2*(hamiltonForm.q1*hamiltonForm.q3+hamiltonForm.q2*hamiltonForm.q0);
    double r21 = 2*(hamiltonForm.q1*hamiltonForm.q2+hamiltonForm.q3*hamiltonForm.q0);
    double r22 = hamiltonForm.q0*hamiltonForm.q0-hamiltonForm.q1*hamiltonForm.q1+hamiltonForm.q2*hamiltonForm.q2-hamiltonForm.q3*hamiltonForm.q3;
    double r23 = 2*(hamiltonForm.q2*hamiltonForm.q3-hamiltonForm.q1*hamiltonForm.q0);
    double r31 = 2*(hamiltonForm.q1*hamiltonForm.q3-hamiltonForm.q2*hamiltonForm.q0);
    double r32 = 2*(hamiltonForm.q2*hamiltonForm.q3+hamiltonForm.q1*hamiltonForm.q0);
    double r33 = hamiltonForm.q0*hamiltonForm.q0-hamiltonForm.q1*hamiltonForm.q1-hamiltonForm.q2*hamiltonForm.q2+hamiltonForm.q3*hamiltonForm.q3;

    rotated.i = r11*vector.i + r12*vector.j + r13*vector.k;
    rotated.j = r21*vector.i + r22*vector.j + r23*vector.k;
    rotated.k = r31*vector.i + r32*vector.j + r33*vector.k;
    return rotated;
}

quaternion_t *createQuaternion(void)
{
    quaternion_t *_quaternion = pvPortMalloc(sizeof(quaternion_t));
    return _quaternion;
}

//Madgwick to quaternion type
void saveQuaternion(quaternion_t *_quaternion, float _q0, float _q1, float _q2, float _q3)
{
    memcpy(_quaternion->value, (const void *) &_q0, sizeof(_q0));
    memcpy(_quaternion->value+4, (const void *) &_q1, sizeof(_q1));
    memcpy(_quaternion->value+8, (const void *) &_q2, sizeof(_q2));
    memcpy(_quaternion->value+12, (const void *) &_q3, sizeof(_q3));
}

void printQuaternion(quaternion_t *_quaternion)
{
    uint32_t _q0 = (_quaternion->value[3]<<24)|(_quaternion->value[2]<<16)|(_quaternion->value[1]<<8)|(_quaternion->value[0]);
    uint32_t _q1 = (_quaternion->value[7]<<24)|(_quaternion->value[6]<<16)|(_quaternion->value[5]<<8)|(_quaternion->value[4]);
    uint32_t _q2 = (_quaternion->value[11]<<24)|(_quaternion->value[10]<<16)|(_quaternion->value[9]<<8)|(_quaternion->value[8]);
    uint32_t _q3 = (_quaternion->value[15]<<24)|(_quaternion->value[14]<<16)|(_quaternion->value[13]<<8)|(_quaternion->value[12]);
    
    float _q0f,_q1f,_q2f,_q3f;  
    memcpy(&_q0f, &_q0, sizeof(_q0f));
    memcpy(&_q1f, &_q1, sizeof(_q1f));
    memcpy(&_q2f, &_q2, sizeof(_q2f));
    memcpy(&_q3f, &_q3, sizeof(_q3f));

    printf("THEMU Quaternion Reading:\n_Q0=%f\t_Q1=%f\t_Q2=%f\t_Q3=%f\n",_q0f,_q1f,_q2f,_q3f);
}