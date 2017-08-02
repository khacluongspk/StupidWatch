#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "fstorage.h"
#include "fds.h"
#include "peer_manager.h"

#include "bsp_btn_ble.h"
#include "nrf_gpio.h"
#include "ble_hci.h"
#include "ble_advdata.h"        
#include "ble_advertising.h"
#include "ble_conn_state.h"

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_delay.h"
#include "nrf_drv_twi.h"
#include "uart_module.h"
#include "mpu6050.h"
#include "ssd1306.h"

float x = 0;
float y = 0;
float z = 0;

int16_t a,b,c;

int main(){
	uart_init();
	printf("SmartWatch\n\r");
	mpu6050_init();
	
	printf("Oled SSD1306 init ...");
	ssd1306_init();
	printf(" done\n\r");
	ssd1306_clear();
	ssd1306_setXY(0,0);
	ssd1306_putChar('A');
	ssd1306_setXY(1,0);
	ssd1306_putString("Kachel");
	ssd1306_setXY(2,0);
	ssd1306_putNumber(123456789);
	ssd1306_setXY(3,0);
	ssd1306_putNumber(1111);
	ssd1306_putNumber(-1111);
	while(1){
		/*
		nrf_delay_ms(10000);
		mpu6050_readAccelerometer(&a, &b, &c);
		mpu6050_convertAcc(&x, &y, &z);
		printf("MPU6050 Raw data: x = %5.2f, y = %5.2f, z = %5.2f\n\r",x, y, z);
		ssd1306_show();
		nrf_delay_ms(50);
		ssd1306_clear();
		nrf_delay_ms(50);
		*/
	}
}
