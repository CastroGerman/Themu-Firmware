#include "MPU6050.h"
#include "myI2C.h"
#include "MadgwickAHRS.h"
#include "myBLE.h"
#include "myTasks.h"
#include "QuaternionLib.h"
#include <string.h>
#include "configs.h"

quaternion_t *quaternion;

void InitMPU6050 (void)
{
    /*Master Write or Read?
    *
    * After sending a slave’s address, see step 3 on pictures above, 
    * the master either writes to or reads from the slave. 
    * The information what the master will actually do is hidden in the least 
    * significant bit of the slave’s address.
    * 
    * Therefore the command link instructing the slave that the master will write
    * the data contains the address like (ESP_SLAVE_ADDR << 1) | I2C_MASTER_WRITE
    */
	i2c_cmd_handle_t cmd;		//command link

	//Power Management config. Command link assembly.
	cmd = i2c_cmd_link_create();
	ESP_ERROR_CHECK(i2c_master_start(cmd)); // Loading Start Bit
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (MPU6050_ADDRESS << 1) | I2C_MASTER_WRITE, 1)); //Loading Slave Address
	i2c_master_write_byte(cmd, MPU6050_PWR_MGMT_1, NACK); // Data
	i2c_master_write_byte(cmd, 0, NACK); 
	ESP_ERROR_CHECK(i2c_master_stop(cmd)); // Loading Stop Bit
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS); //Sending assembled command and blocking 1s till finish
	i2c_cmd_link_delete(cmd); //Freeing cmd resources taken

	//Setting gyroscope full scale range according to FS_SEL. See MPU6050.h
	cmd = i2c_cmd_link_create();
	ESP_ERROR_CHECK(i2c_master_start(cmd)); 
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (MPU6050_ADDRESS << 1) | I2C_MASTER_WRITE, 1)); 
	i2c_master_write_byte(cmd, MPU6050_GYRO_CONFIG, NACK); 
	i2c_master_write_byte(cmd, FS_SEL<<3 , NACK); 
	ESP_ERROR_CHECK(i2c_master_stop(cmd)); 
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS); 
	i2c_cmd_link_delete(cmd); 

	//Setting accelerometer full scale range according to AFS_SEL. See MPU6050.h
	cmd = i2c_cmd_link_create();
	ESP_ERROR_CHECK(i2c_master_start(cmd)); 
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (MPU6050_ADDRESS << 1) | I2C_MASTER_WRITE, 1)); 
	i2c_master_write_byte(cmd, MPU6050_ACCEL_CONFIG, NACK); 
	i2c_master_write_byte(cmd, AFS_SEL<<3 , NACK); 
	ESP_ERROR_CHECK(i2c_master_stop(cmd)); 
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS); 
	i2c_cmd_link_delete(cmd); 
}

void readMPU6050Regs(uint8_t *_mpuRegs)
{
    i2c_cmd_handle_t cmd;	
    cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (MPU6050_ADDRESS << 1) | I2C_MASTER_WRITE, 1));
    //Tell the MPU6050 to position the internal register pointer to register MPU6050_ACCEL_XOUT_H.
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, MPU6050_ACCEL_XOUT_H, NACK));
    ESP_ERROR_CHECK(i2c_master_stop(cmd));
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS));
    i2c_cmd_link_delete(cmd);
    cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (MPU6050_ADDRESS << 1) | I2C_MASTER_READ, 1));
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, &_mpuRegs[accelX_H], ACK));//MPU6050_ACCEL_XOUT_H
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, &_mpuRegs[accelX_L], ACK));//MPU6050_ACCEL_XOUT_L
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, &_mpuRegs[accelY_H], ACK));//MPU6050_ACCEL_YOUT_H
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, &_mpuRegs[accelY_L], ACK));//MPU6050_ACCEL_YOUT_L
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, &_mpuRegs[accelZ_H], ACK));//MPU6050_ACCEL_ZOUT_H
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, &_mpuRegs[accelZ_L], ACK));//MPU6050_ACCEL_ZOUT_L
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, &_mpuRegs[temp_H], ACK));//MPU6050_TEMP_OUT_H
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, &_mpuRegs[temp_L], ACK));//MPU6050_TEMP_OUT_L
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, &_mpuRegs[gyroX_H], ACK));//MPU6050_GYRO_XOUT_H
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, &_mpuRegs[gyroX_L], ACK));//MPU6050_GYRO_XOUT_L
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, &_mpuRegs[gyroY_H], ACK));//MPU6050_GYRO_YOUT_H
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, &_mpuRegs[gyroY_L], ACK));//MPU6050_GYRO_YOUT_L
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, &_mpuRegs[gyroZ_H], ACK));//MPU6050_GYRO_ZOUT_H
    ESP_ERROR_CHECK(i2c_master_read_byte(cmd, &_mpuRegs[gyroZ_L], NACK));//MPU6050_GYRO_ZOUT_L 
    //Send ACK(0) n-1 times and end with NAK(1) on the last sending.
    //i2c_master_read(cmd, data, sizeof(data), 1);
    ESP_ERROR_CHECK(i2c_master_stop(cmd));
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS));
    i2c_cmd_link_delete(cmd);
}

void getMPUValuesFromRegs(double *_values, uint8_t *_mpuRegs)
{   
    _values[accelX] = ((_mpuRegs[accelX_H] << 8) | _mpuRegs[accelX_L]);
    _values[accelY] = ((_mpuRegs[accelY_H] << 8) | _mpuRegs[accelY_L]);
    _values[accelZ] = ((_mpuRegs[accelZ_H] << 8) | _mpuRegs[accelZ_L]);
    _values[temp] = ((_mpuRegs[temp_H] << 8) | _mpuRegs[temp_L]);
    _values[gyroX] = ((_mpuRegs[gyroX_H] << 8) | _mpuRegs[gyroX_L]);
    _values[gyroY] = ((_mpuRegs[gyroY_H] << 8) | _mpuRegs[gyroY_L]);
    _values[gyroZ] = ((_mpuRegs[gyroZ_H] << 8) | _mpuRegs[gyroZ_L]);
}

void processMPUValues(double *_processed, double *_values, double *_offsets)
{
    // Read gyro values and convert to Radians per second
    _processed[gyroX]  =  ((short)_values[gyroX] - (short)_offsets[gyroX]) * DEG_TO_RAD * GYRO_SCALE;
    _processed[gyroY]  =  ((short)_values[gyroY] - (short)_offsets[gyroY]) * DEG_TO_RAD * GYRO_SCALE;
    _processed[gyroZ]  =  ((short)_values[gyroZ] - (short)_offsets[gyroZ]) * DEG_TO_RAD * GYRO_SCALE;
    // As datasheet says:
    // Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53
    // As I say:
    _processed[temp] = ((short)_values[temp] - (short)_offsets[temp]) * TEMP_SCALE;
    // Read accel values and convert to G force.
    _processed[accelX] = ((short)_values[accelX] - (short)_offsets[accelX]) * ACCEL_SCALE;
    _processed[accelY] = ((short)_values[accelY] - (short)_offsets[accelY]) * ACCEL_SCALE;    
    _processed[accelZ] = ((short)_values[accelZ] - (short)_offsets[accelZ]) * ACCEL_SCALE;
}

void takeOutGForceFromAccel(double *_MPU6050Values)
{
    // This step requires to have 1 axis pointing up.
    if (_MPU6050Values[accelX] > (1/ACCEL_SCALE)){_MPU6050Values[accelX] -= (1/ACCEL_SCALE);}
    if (_MPU6050Values[accelY] > (1/ACCEL_SCALE)){_MPU6050Values[accelY] -= (1/ACCEL_SCALE);}
    if (_MPU6050Values[accelZ] > (1/ACCEL_SCALE)){_MPU6050Values[accelZ] -= (1/ACCEL_SCALE);}
}

void getMPU6050Offset(double *_offsetValues)
{   
    uint8_t mpuRegs[MPU6050_8BITS_REGS];
    double mpuValues[MPU6050_16BITS_REGS];
    for (int i = 0; i < CAL_ITERATIONS; i++)
    {
        readMPU6050Regs(mpuRegs);
        getMPUValuesFromRegs(mpuValues, mpuRegs);
        for(int j = 0; j < MPU6050_16BITS_REGS; j++)
        {
            _offsetValues[j] += mpuValues[j];
        }
    }  
    for(int i = 0; i < MPU6050_16BITS_REGS; i++)
    {
        _offsetValues[i] /= CAL_ITERATIONS;
    }
    takeOutGForceFromAccel(_offsetValues);
}

void printValues(double *_values)
{
    printf("Values:\naccelX: %f\t accelY: %f\t accelZ: %f\t temp: %f\t gryoX: %f\t gryoY: %f\t gryoZ: %f\n",
            _values[accelX],_values[accelY],_values[accelZ],_values[temp],_values[gyroX],_values[gyroY],_values[gyroZ]);
}

void tMPU6050 (void *pv)
{
    uint32_t notifycount = 0;
  	uint8_t mpuRegs[MPU6050_8BITS_REGS];
    double mpuValues[MPU6050_16BITS_REGS], processedValues[MPU6050_16BITS_REGS];
    double offsetValues[MPU6050_16BITS_REGS];
    quaternion = createQuaternion();

    getMPU6050Offset(offsetValues);

	while(1) {
        notifycount = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if(notifycount == 1)
        {
            readMPU6050Regs(mpuRegs);
            getMPUValuesFromRegs(mpuValues, mpuRegs);
            processMPUValues(processedValues, mpuValues, offsetValues);
            MadgwickAHRSupdateIMU(processedValues[gyroX],processedValues[gyroY],processedValues[gyroZ],
                                processedValues[accelX],processedValues[accelY],processedValues[accelZ]);
            saveQuaternion(quaternion,q0,q1,q2,q3);
            
            #ifdef ENABLE_THEMU_IMU_LOGS
            printf("MPU ");
            printValues(mpuValues);
            printf("Offset ");
            printValues(offsetValues);
            printf("Processed ");
            printValues(processedValues);
            printQuaternion(quaternion);
            printf("Madgwick Quaternion Reading:\nQ0=%f\tQ1=%f\tQ2=%f\tQ3=%f\n",q0,q1,q2,q3);
            #endif
            
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
            printf("TIMEOUT waiting notification on tMPU6050\n");
        }
	}
}
