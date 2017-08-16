/*
 * 
 * Driver MPU6050 for nRF51xxx
 * Author: Kachel
 * Data: Mar-26-2017
 */

#include "mpu6050.h"
#include "nrf_delay.h"

static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(0);
static float acc_x, acc_y, acc_z;
//static float gyro_x, gyro_y, gyro_z;
static int16_t raw_acc_x, raw_acc_y, raw_acc_z;
//static int16_t raw_gyro_x, raw_gyro_y, raw_gyro_z;
bool twi_init(uint32_t scl_pin, uint32_t sda_pin)
{
    ret_code_t err_code;
    const nrf_drv_twi_config_t twi_config = {
       /*
       .scl                = scl_pin,
       .sda                = sda_pin,
        */
        .scl                = scl_pin,
        .sda                = sda_pin,
        .frequency          = NRF_TWI_FREQ_100K,
        .interrupt_priority = 1
    };
    
    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);
    
    nrf_drv_twi_enable(&m_twi);
		if (err_code == NRF_SUCCESS)
			return true;
		else 
			return false;
}

bool twi_readByte(uint8_t slaveAddr, uint8_t regAddr, uint8_t *data)
{
    uint8_t len = 1;
    nrf_drv_twi_tx(&m_twi, slaveAddr, &regAddr, 1, true);
    ret_code_t  err = nrf_drv_twi_rx(&m_twi, slaveAddr, data, len);
    if (err != 0)   printf("twi_readByte = %d\n\r",err);
    nrf_delay_ms(1000);
    APP_ERROR_CHECK(err);
    //return err == NRF_SUCCESS : true ? false;
		if (err == NRF_SUCCESS)
			return true;
		else 
			return false;
}

bool twi_readBytes(uint8_t slaveAddr, uint8_t regAddr, uint8_t len, uint8_t *data)
{
    nrf_drv_twi_tx(&m_twi, slaveAddr, &regAddr, 1, true);
    ret_code_t  err = nrf_drv_twi_rx(&m_twi, slaveAddr, data, len);
    //return err == NRF_SUCCESS : true ? false;
		if (err == NRF_SUCCESS)
			return true;
		else 
			return false;
}

uint8_t twi_readBit(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitNum)
{
    uint8_t raw;
    uint8_t result;
    twi_readByte(slaveAddr, regAddr, &raw);
    result = (raw >> bitNum) & 0x01;
    return  result;
}

uint8_t twi_readBits(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitStart, uint8_t lenght)
{
    uint8_t raw;
    uint8_t result;
    uint8_t bitsMark;

    bitsMark = ((1 << lenght) - 1) <<  (bitStart - lenght + 1);
    
    twi_readByte(slaveAddr, regAddr, &raw);
    result = (raw && bitsMark) >> bitStart;
    return  result;
}

bool twi_writeByte(uint8_t slaveAddr, uint8_t regAddr, uint8_t data)
{
    uint8_t buff[2];
    ret_code_t  err;
    
    buff[0] = regAddr;
    buff[1] = data;
    nrf_drv_twi_tx(&m_twi, slaveAddr, buff, 2, false);
    //return err == NRF_SUCCESS : true ? false;
		if (err == NRF_SUCCESS)
			return true;
		else 
			return false;
}

bool twi_writeBytes(uint8_t slaveAddr, uint8_t regAddr, uint8_t *data, uint8_t lenght)
{
    ret_code_t  err;
    err = nrf_drv_twi_tx(&m_twi, slaveAddr, data, lenght, false);
    //return err == NRF_SUCCESS : true ? false;
		if (err == NRF_SUCCESS)
			return true;
		else 
			return false;
}

bool twi_writeBit(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitNum, uint8_t value)
{
    uint8_t raw;
    twi_readByte(slaveAddr, regAddr, &raw);
    if (value == 1){
        raw = raw | (1 << bitNum);
    } else if (value == 0) {
        raw &= ~(1 << bitNum);
    }
    else
        return false;
    return twi_writeByte(slaveAddr, regAddr, raw);
}

bool twi_writeBits(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitStart, uint8_t lenght, uint8_t value)
{
    uint8_t raw;
    uint8_t bitsMask;
	  ret_code_t  err;
    twi_readByte(slaveAddr, regAddr, &raw);
    bitsMask =  ((0x01 << bitStart) -1) << (bitStart - lenght + 1);
    raw &= bitsMask;
    raw |= (value << bitStart);
    twi_writeByte(slaveAddr, regAddr, raw);
		if (err == NRF_SUCCESS)
			return true;
		else 
			return false;
}

bool mpu6050_init()
{
    uint8_t raw = 0;
    bool error;
		ret_code_t err_code;
		const nrf_drv_twi_config_t twi_config = {
#if 0 
			.scl                = 0,
			.sda                = 1,
#else
            .scl                = 5,
            .sda                = 6,
#endif
			.frequency          = NRF_TWI_FREQ_100K,
			.interrupt_priority = 1
		};

		err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
		APP_ERROR_CHECK(err_code);
		
		nrf_drv_twi_enable(&m_twi);
		APP_ERROR_CHECK(err_code);
		
    error = twi_readByte(MPU6050_ADDRESS, MPU6050_RA_WHO_AM_I, &raw);
		if	(error != 1) printf("error = %d, MPU6050_RA_WHO_AM_I = %x\n\r", error, raw);
    /* set Clock Source -- Reg 0x6B -- 107: Bits[2-0] = 0x01 PLL with X axis gyroscope reference. */
    if (!twi_writeBits(MPU6050_ADDRESS, MPU6050_RA_PWR_MGMT_1, 0x00, 3, 0x01)){
        printf("MPU6050_RA_PWR_MGMT_1 false\n\r");
        return false;
    }

    /*set Gyroscope range -- Reg 0x1B -- 27: Bits[4-3] = 0x00 Full Scale  range = 250 o/s */
    if (!twi_writeBits(MPU6050_ADDRESS, MPU6050_RA_GYRO_CONFIG, 0x03, 2, 0x00)){
        printf("MPU6050_RA_GYRO_CONFIG false\n\r");
        return false;
    }   
    
    /*set Accelerometer range -- Reg 0x1C -- 28: Bits[4-3] = 0x00  Full Scale Range = 2g. */   
    if (!twi_writeBits(MPU6050_ADDRESS, MPU6050_RA_ACCEL_CONFIG, 0x03, 2, 0x00)){
        printf("MPU6050_RA_ACCEL_CONFIG false\n\r");
        return false;
    }
	return true;
}

void mpu6050_readAccelerometer(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t buff[6];
    twi_readBytes(MPU6050_ADDRESS,MPU6050_RA_ACCEL_XOUT_H, 6, buff);
    raw_acc_x = (int16_t)((buff[0] << 8) | buff[1]);
    raw_acc_y = (int16_t)((buff[2] << 8) | buff[3]);
    raw_acc_z = (int16_t)((buff[4] << 8) | buff[5]);
    *x = raw_acc_x;
    *y = raw_acc_y;
    *z = raw_acc_z;
}

void mpu6050_convertAcc(float *x, float *y, float *z){
    acc_x = (float)raw_acc_x * 9.8 /16384;
    acc_y = (float)raw_acc_y * 9.8 /16384;
    acc_z = (float)raw_acc_z * 9.8 /16384;
    *x = acc_x;
    *y = acc_y;
    *z = acc_z;

}

void mpu6050_readXYZ(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t buff[6];
    twi_readBytes(MPU6050_ADDRESS,MPU6050_RA_ACCEL_XOUT_H, 6, buff);
    raw_acc_x = (int16_t)((buff[0] << 8) | buff[1]);
    raw_acc_y = (int16_t)((buff[2] << 8) | buff[3]);
    raw_acc_z = (int16_t)((buff[4] << 8) | buff[5]);
    *x = raw_acc_x  /1638;
    *y = raw_acc_y  /1638;
    *z = raw_acc_z  /1638;
}
