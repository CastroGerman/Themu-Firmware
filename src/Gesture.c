#include "Gesture.h"
#include "myTasks.h"
#include "MadgwickAHRS.h"
#include "configs.h"
#include "myGPIO.h"

gesture_t *gesture;
uint8_t gesturesPayload;

int isPointingUp(vector_t _vector)
{
    if(_vector.k > GST_POINTING_UP_EVT_TGR)
    {
        return 1;
    }
    return 0;
}

int isPointingDown(vector_t _vector)
{
    if(_vector.k < GST_POINTING_DOWN_EVT_TGR)
    {
        return 1;
    }
    return 0;
}

int isPointingFront(vector_t _vector)
{
    if(_vector.i > GST_POINTING_FRONT_EVT_TGR)
    {
        return 1;
    }
    return 0;
}

int isPointingBack(vector_t _vector)
{
    if(_vector.i < GST_POINTING_BACK_EVT_TGR)
    {
        return 1;
    }
    return 0;
}

int isPointingLeft(vector_t _vector)
{
    if(_vector.j > GST_POINTING_LEFT_EVT_TGR)
    {
        return 1;
    }
    return 0;
}

int isPointingRight(vector_t _vector)
{
    if(_vector.j < GST_POINTING_RIGHT_EVT_TGR)
    {
        return 1;
    }
    return 0;
}

int isPunching(gesture_t *_gesture)
{
    if(_gesture->imu[accelX]<GST_FIST_BUMP_EVT_TGR)
    {
        return 1;
    }
    return 0;
}

int isSlidingRight(gesture_t *_gesture)
{
    if(_gesture->imu[accelY]>GST_SLIDE_RIGHT_EVT_TGR)
    {
        return 1;
    }
    return 0;
}


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

void analyzeGestures(gesture_t *_gesture, uint8_t *_gesturesPayload)
{
    vector_t vector = {VECTOR_REF};
    #ifdef ENABLE_THEMU_ORIENTATION_LOGS
    printf("Reference Orientation:\t(%f,%f,%f)\n", vector.i, vector.j, vector.k);
    #endif
    vector = rotateVector(vector, _gesture->quaternion.hamiltonForm);
    #ifdef ENABLE_THEMU_ORIENTATION_LOGS
    printf("Rotated Orientation:\t(%f,%f,%f)\n", vector.i, vector.j, vector.k);
    #endif

    setBitInByte(_gesturesPayload, GST_POINTING_UP_PLOAD_BIT, isPointingUp(vector));
    setBitInByte(_gesturesPayload, GST_POINTING_DOWN_PLOAD_BIT, isPointingDown(vector));
    setBitInByte(_gesturesPayload, GST_POINTING_FRONT_PLOAD_BIT, isPointingFront(vector));
    setBitInByte(_gesturesPayload, GST_POINTING_BACK_PLOAD_BIT, isPointingBack(vector));
    setBitInByte(_gesturesPayload, GST_POINTING_LEFT_PLOAD_BIT, isPointingLeft(vector));
    setBitInByte(_gesturesPayload, GST_POINTING_RIGHT_PLOAD_BIT, isPointingRight(vector));
}

void tGestures (void *pv)
{
    uint32_t notifycount = 0;
    gesture = createGesture();
    while (1)
    {
        notifycount = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if(notifycount == 1)
        {
            MadgwickAHRSupdateIMU(processedValues[gyroX],processedValues[gyroY],processedValues[gyroZ],
                                processedValues[accelX],processedValues[accelY],processedValues[accelZ]);
            updateGesture(gesture, (double *)processedValues, q0, q1, q2, q3);
            if (isPunching(gesture))
            {
                toggleBitInByte(&gesturesPayload, GST_FIST_BUMP_PLOAD_BIT);
            }
            if (isSlidingRight(gesture))
            {
                toggleBitInByte(&gesturesPayload, GST_SLIDE_RIGHT_PLOAD_BIT);
            }  
        }
        else
        {
            printf("TIMEOUT waiting notification on tGestures\n");
        }
    }
}
