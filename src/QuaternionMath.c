#include "QuaternionMath.h"
#include <math.h>


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