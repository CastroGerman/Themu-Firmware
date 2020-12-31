#ifndef QUATERNIONMATH_H_
#define QUATERNIONMATH_H_

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

#endif /* QUATERNIONMATH_H_ */