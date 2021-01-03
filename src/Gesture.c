#include "Gesture.h"
#include "myTasks.h"
#include "MadgwickAHRS.h"
#include "configs.h"

//gesture_t *gesture;

gesture_t *createGesture(void)
{
    gesture_t *_gesture = pvPortMalloc(sizeof(gesture_t));
    return _gesture;
}

void updateGesture(gesture_t *_gesture, double *_imu, float _q0, float _q1, float _q2, float _q3)
{
    _gesture->imu[accelX] = _imu[accelX];
    _gesture->imu[accelY] = _imu[accelY];
    _gesture->imu[accelZ] = _imu[accelZ];
    _gesture->imu[gyroX] = _imu[gyroX];
    _gesture->imu[gyroY] = _imu[gyroY];
    _gesture->imu[gyroZ] = _imu[gyroZ];
    _gesture->flex[THUMB] = getFingerFlexChannel(THUMB_FLEX_CHANNEL);
    _gesture->flex[INDEX] = getFingerFlexChannel(INDEX_FLEX_CHANNEL);
    _gesture->flex[MIDDLE] = getFingerFlexChannel(MIDDLE_FLEX_CHANNEL);
    _gesture->flex[RING] = getFingerFlexChannel(RING_FLEX_CHANNEL);
    _gesture->flex[LITTLE] = getFingerFlexChannel(LITTLE_FLEX_CHANNEL);
    _gesture->quaternion.hamiltonForm.q0 = _q0;
    _gesture->quaternion.hamiltonForm.q1 = _q1;
    _gesture->quaternion.hamiltonForm.q2 = _q2;
    _gesture->quaternion.hamiltonForm.q3 = _q3;          
}

void tGestures (void *pv)
{
    uint32_t notifycount = 0;
    gesture_t *gesture = createGesture();
    while (1)
    {
        notifycount = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if(notifycount == 1)
        {
            MadgwickAHRSupdateIMU(processedValues[gyroX],processedValues[gyroY],processedValues[gyroZ],
                                processedValues[accelX],processedValues[accelY],processedValues[accelZ]);
            updateGesture(gesture, (double *)processedValues, q0, q1, q2, q3);     
    
            printf("%f\t%f\n",gesture->imu[accelX],processedValues[accelX]);
    
            #ifdef ENABLE_VECTOR_ROTATION
            quaternionForm_t myQuat;
            myQuat.hamiltonForm.q0 = q0;
            myQuat.hamiltonForm.q1 = q1;
            myQuat.hamiltonForm.q2 = q2;
            myQuat.hamiltonForm.q3 = q3;
            //myQuat.polarForm = hamilton2polar(myQuat.hamiltonForm);
            vector_t p, prot;
            p.i=1;
            p.j=0;
            p.k=0;
            prot = rotateVector(p,myQuat.hamiltonForm);
            printf("Vector Rotation Info:\npi=%f\tpj=%f\tpk=%f\tQ0=%f\tQ1=%f\tQ2=%f\tQ3=%f\n",prot.i,prot.j,prot.k,q0,q1,q2,q3);
            #endif
        }
        else
        {
            printf("TIMEOUT waiting notification on tGestures\n");
        }
    }
}
