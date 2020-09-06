#ifndef QUATERNIONLIB_H_
#define QUATERNIONLIB_H_

#include <stdint.h>

/*Quaternion value formatted as:
* value[0]......value[4]......value[8]......value[12]......value[15]
* q0 (LSB)......q1 (LSB)......q2 (LSB)......q3 (LSB)........q3 (MSB)
*/
#define QUATERNION_SIZE_BYTES   16

typedef struct quaternion
{
	uint8_t value[QUATERNION_SIZE_BYTES];
}quaternion_t;

typedef struct qHamiltonForm
{
    double q0,q1,q2,q3;
}qHamiltonForm_t;

typedef struct qPolarForm
{
    double ang,i,j,k;
}qPolarForm_t;

typedef struct quaternionForm
{
    qHamiltonForm_t hamiltonForm;
    qPolarForm_t polarForm;
}quaternionForm_t;

typedef struct vector
{
    double i,j,k;
}vector_t;


vector_t rotateVector(vector_t vector, qHamiltonForm_t hamiltonForm);
qPolarForm_t hamilton2polar (qHamiltonForm_t hamiltonForm);
qHamiltonForm_t polar2hamilton (qPolarForm_t polarForm);
quaternion_t *vQuaternionCreate(void);
void vQuaternionSave(quaternion_t *_quaternion, float _q0, float _q1, float _q2, float _q3);
void vQuaternionPrint(quaternion_t *_quaternion);

#endif
