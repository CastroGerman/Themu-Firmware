/**THEMU Config File.
 * 
 * Defines marked as 'Cores' will enable important functionalities
 * in the firmware and will determine its behavior.
 * 
 * Defines marked as 'Logs' will enable debug related information.
 * ENABLE_THEMU_LOGS has to be defined in order to enable the rest 
 * of the logs defines. Likewise ENABLE_THEMU_LOGS can be commented
 * to disable all logs.
 */

//Core
#define ENABLE_THEMU_BLE
#define ENABLE_THEMU_IMU
#define ENABLE_THEMU_ADC

//Logs
#define ENABLE_THEMU_LOGS
#ifdef ENABLE_THEMU_LOGS
    //#define ENABLE_THEMU_BLE_LOGS
    //#define ENABLE_THEMU_ADC_LOGS
    //#define ENABLE_THEMU_IMU_LOGS
#endif

//Test
//#define ENABLE_VECTOR_ROTATION
//#define ENABLE_LIVE_PLOT