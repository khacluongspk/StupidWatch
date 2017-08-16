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



int main(){
	uint16_t step_status,step_count;
	uint16_t a,b,c;
	float x,y,z;
	uart_init();
	printf("SmartWatch\n\r");
	pedometer_init();
	
	printf("Oled SSD1306 init ...");
	ssd1306_init();
	printf("done\n\r");
	ssd1306_clear();

	step_status =1;
	step_count = 0;
	while(1){
#if 1
		a++;
		UpdateUI(step_status,step_count);
		step_count = pedometer_stepCalc();
		if (a%100 )	step_status = step_status == 1 ?  0:1;
		
#else
		nrf_delay_ms(1000);
		mpu6050_readAccelerometer(&a, &b, &c);
		mpu6050_convertAcc(&x, &y, &z);
		printf("MPU6050 Raw data: x = %5.2f, y = %5.2f, z = %5.2f\n\r",x, y, z);
#endif
	}
}
