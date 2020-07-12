#include "MPU6050.h"
#include "myI2C.h"
#include "myTimers.h"
#include <math.h>

#include "MadgwickAHRS.h"


xTaskHandle thMPU6050 = NULL;

void InitMPU6050 (void)
{
    /*
    * Master Write or Read?
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

	// Power Management config. Command link assembly.
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


void offsetCalibration (double *accel_x_offset, double *accel_y_offset, double *accel_z_offset, 
double *temp_offset, double *gyro_x_offset,double *gyro_y_offset,double *gyro_z_offset)
{
    uint8_t data[14], cal_iterations;
	i2c_cmd_handle_t cmd;	

    for (cal_iterations = 0; cal_iterations < CAL_ITERATIONS; cal_iterations++)
    {
        cmd = i2c_cmd_link_create();
        ESP_ERROR_CHECK(i2c_master_start(cmd));
        ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (MPU6050_ADDRESS << 1) | I2C_MASTER_WRITE, 1));
        ESP_ERROR_CHECK(i2c_master_write_byte(cmd, MPU6050_ACCEL_XOUT_H, NACK));
        ESP_ERROR_CHECK(i2c_master_stop(cmd));
        ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS));
        i2c_cmd_link_delete(cmd);
        cmd = i2c_cmd_link_create();
        ESP_ERROR_CHECK(i2c_master_start(cmd));
        ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (MPU6050_ADDRESS << 1) | I2C_MASTER_READ, 1));
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data,   ACK));//MPU6050_ACCEL_XOUT_H
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+1, ACK));//MPU6050_ACCEL_XOUT_L
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+2, ACK));//MPU6050_ACCEL_YOUT_H
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+3, ACK));//MPU6050_ACCEL_YOUT_L
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+4, ACK));//MPU6050_ACCEL_ZOUT_H
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+5, ACK));//MPU6050_ACCEL_ZOUT_L
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+6, ACK));//MPU6050_TEMP_OUT_H
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+7, ACK));//MPU6050_TEMP_OUT_L
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+8, ACK));//MPU6050_GYRO_XOUT_H
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+9, ACK));//MPU6050_GYRO_XOUT_L
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+10,ACK));//MPU6050_GYRO_YOUT_H
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+11,ACK));//MPU6050_GYRO_YOUT_L
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+12,ACK));//MPU6050_GYRO_ZOUT_H
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+13,NACK));//MPU6050_GYRO_ZOUT_L 
        ESP_ERROR_CHECK(i2c_master_stop(cmd));
        ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS));
        i2c_cmd_link_delete(cmd);

        *accel_x_offset += ((data[0] << 8) | data[1]);
		*accel_y_offset += ((data[2] << 8) | data[3]);
		*accel_z_offset += ((data[4] << 8) | data[5]);
		*temp_offset    += ((data[6] << 8) | data[7]);
        *gyro_x_offset  += ((data[8] << 8) | data[9]);
        *gyro_y_offset  += ((data[10] << 8) | data[11]);
        *gyro_z_offset  += ((data[12] << 8) | data[13]);
    }  
    *accel_x_offset /= CAL_ITERATIONS;
    *accel_y_offset /= CAL_ITERATIONS;
    *accel_z_offset /= CAL_ITERATIONS;
    *temp_offset    /= CAL_ITERATIONS;
    *gyro_x_offset  /= CAL_ITERATIONS;
    *gyro_y_offset  /= CAL_ITERATIONS;
    *gyro_z_offset  /= CAL_ITERATIONS;
    // Taking off the G force.
    // This step requires to have 1 axis pointing up. See a better solution.
    if (*accel_x_offset > 16384.0f){*accel_x_offset -= 16384.0f;}
    if (*accel_y_offset > 16384.0f){*accel_y_offset -= 16384.0f;}
    if (*accel_z_offset > 16384.0f){*accel_z_offset -= 16384.0f;}
}

double getAccelXAngle (double faccel_x, double faccel_y, double faccel_z)
{
    double accel_ang_x = atan(faccel_x / sqrt(pow(faccel_y, 2) + pow(faccel_z, 2)))*RAD_TO_DEG;
    return accel_ang_x;
}
double getAccelYAngle (double faccel_x, double faccel_y, double faccel_z)
{
    double accel_ang_y = atan(faccel_y / sqrt(pow(faccel_x, 2) + pow(faccel_z, 2)))*RAD_TO_DEG;
    return accel_ang_y;
}
double getAccelZAngle (double faccel_x, double faccel_y, double faccel_z)
{
    double accel_ang_z = atan(faccel_z / sqrt(pow(faccel_x, 2) + pow(faccel_y, 2)))*RAD_TO_DEG;
    return accel_ang_z;
}


void displayAngles (double faccel_x, double faccel_y, double faccel_z)
{
    printf("AXAng: %f \tAYAng: %f \tAZAng: %f\n",
    getAccelXAngle (faccel_x, faccel_y, faccel_z),
    getAccelYAngle (faccel_x, faccel_y, faccel_z),
    getAccelZAngle (faccel_x, faccel_y, faccel_z));
}


/*
*   Task to show if register values were processed correctly
*/
void tMPU6050 (void *pv)
{
    uint32_t notifycount = 0;

  	uint8_t data[14];
    // Maybe changing for arrays and make it less explicit?
	short accel_x, accel_y, accel_z, temp, gyro_x, gyro_y, gyro_z; //Raw Register Values
    double accel_x_offset, accel_y_offset, accel_z_offset, temp_offset, gyro_x_offset, gyro_y_offset, gyro_z_offset;
    double faccel_x, faccel_y, faccel_z, ftemp, fgyro_x, fgyro_y, fgyro_z; //Processed Register Values

    static double gyro_x_ang,gyro_y_ang,gyro_z_ang;
    static double roll,pitch,yaw;


    offsetCalibration(&accel_x_offset,&accel_y_offset,&accel_z_offset,
    &temp_offset,&gyro_x_offset,&gyro_y_offset,&gyro_z_offset);

    i2c_cmd_handle_t cmd;		//command link

	while(1) {
        notifycount = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if(notifycount == 1)
        {
            // Tell the MPU6050 to position the internal register pointer to register MPU6050_ACCEL_XOUT_H.
            cmd = i2c_cmd_link_create();
            ESP_ERROR_CHECK(i2c_master_start(cmd));
            ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (MPU6050_ADDRESS << 1) | I2C_MASTER_WRITE, 1));
            ESP_ERROR_CHECK(i2c_master_write_byte(cmd, MPU6050_ACCEL_XOUT_H, NACK));
            ESP_ERROR_CHECK(i2c_master_stop(cmd));
            ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS));
            i2c_cmd_link_delete(cmd);

            cmd = i2c_cmd_link_create();
            ESP_ERROR_CHECK(i2c_master_start(cmd));
            ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (MPU6050_ADDRESS << 1) | I2C_MASTER_READ, 1));

            ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data,   ACK));//MPU6050_ACCEL_XOUT_H
            ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+1, ACK));//MPU6050_ACCEL_XOUT_L
            ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+2, ACK));//MPU6050_ACCEL_YOUT_H
            ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+3, ACK));//MPU6050_ACCEL_YOUT_L
            ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+4, ACK));//MPU6050_ACCEL_ZOUT_H
            ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+5, ACK));//MPU6050_ACCEL_ZOUT_L

            ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+6, ACK));//MPU6050_TEMP_OUT_H
            ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+7, ACK));//MPU6050_TEMP_OUT_L

            ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+8, ACK));//MPU6050_GYRO_XOUT_H
            ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+9, ACK));//MPU6050_GYRO_XOUT_L
            ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+10,ACK));//MPU6050_GYRO_YOUT_H
            ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+11,ACK));//MPU6050_GYRO_YOUT_L
            ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+12,ACK));//MPU6050_GYRO_ZOUT_H
            ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+13,NACK));//MPU6050_GYRO_ZOUT_L 
            //Send ACK(0) n-1 times and end with NAK(1) on the last sending.
            //i2c_master_read(cmd, data, sizeof(data), 1);
            ESP_ERROR_CHECK(i2c_master_stop(cmd));
            ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS));
            i2c_cmd_link_delete(cmd);

            accel_x = (data[0] << 8) | data[1];
            accel_y = (data[2] << 8) | data[3];
            accel_z = (data[4] << 8) | data[5];
            temp 	= (data[6] << 8) | data[7];
            gyro_x 	= (data[8] << 8) | data[9];
            gyro_y 	= (data[10] << 8) | data[11];
            gyro_z 	= (data[12] << 8) | data[13];

            // Read gyro values and convert to Radians per second
            fgyro_x  =  (gyro_x - (short)gyro_x_offset) * DEG_TO_RAD * GYRO_SCALE;
            fgyro_y  =  (gyro_y - (short)gyro_y_offset) * DEG_TO_RAD * GYRO_SCALE;
            fgyro_z  =  (gyro_z - (short)gyro_z_offset) * DEG_TO_RAD * GYRO_SCALE;
            // As datasheet says:
            // Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53
            // As I say:
            ftemp = (temp - (short)temp_offset) * TEMP_SCALE;
            // Read accel values and convert to G force.
            faccel_x = (accel_x - (short)accel_x_offset) * ACCEL_SCALE;
            faccel_y = (accel_y - (short)accel_y_offset) * ACCEL_SCALE;
            faccel_z = (accel_z - (short)accel_z_offset) * ACCEL_SCALE;
            
           // printf("faccel_x: %f \tfaccel_y: %f \tfaccel_z: %f \tftemp: %f \tfgryo_x: %f \tfgryo_y: %f \tfgryo_z: %f\n",
            //faccel_x, faccel_y, faccel_z, ftemp, fgyro_x, fgyro_y, fgyro_z);
            
            MadgwickAHRSupdateIMU(fgyro_x,fgyro_y,fgyro_z,faccel_x,faccel_y,faccel_z);

            //Euler Angles

            /*// Error that would tend to drift: zero crossing
            gyro_x_ang = fgyro_x*RAD_TO_DEG*G0_TIMER0_INTERVAL_SEC;
            gyro_y_ang = fgyro_y*RAD_TO_DEG*G0_TIMER0_INTERVAL_SEC;
            gyro_z_ang = fgyro_z*RAD_TO_DEG*G0_TIMER0_INTERVAL_SEC;
        
            //printf("GXAng: %f \tGYAng: %f \tGZAng: %f \n",gyro_x_ang,gyro_y_ang,gyro_z_ang);
            //displayAngles(faccel_x, faccel_y, faccel_z);
            roll += 0.96*(gyro_x_ang) + 0.04*(getAccelYAngle (faccel_x, faccel_y, faccel_z) - roll);
            pitch += 0.96*(gyro_y_ang) - 0.04*(getAccelXAngle (faccel_x, faccel_y, faccel_z) + pitch);
            yaw += gyro_z_ang ;

            printf("roll: %f \t pitch: %f \t yaw: %f \n",roll,pitch,yaw);
            */
        }
        else
        {
            printf("TIMEOUT esperando notificacion en tMPU6050\n");
        }
        
	}
}
