#include "pedometer.h"
#include "nrf.h"
#include "nrf_drv_timer.h"
#include "bsp.h"
#include "app_error.h"
#include <math.h>


enum Axis{
    X_AXIS = 0,
    Y_AXIS = 1,
    Z_AXIS = 2,
}_model_axis;


int _model_val;
int32_t _curr_val;   
int _model_ratio;
int _last_val;

const nrf_drv_timer_t TIMER_MPU = NRF_DRV_TIMER_INSTANCE(0);
uint32_t time_ms = 100; //Time(in miliseconds) between consecutive compare events.
uint32_t time_ticks;
uint32_t err_code = NRF_SUCCESS;
uint8_t stat_Calc = 0;
uint32_t pedometer_stepCalc(void);
void pedometer_updateModelAxis(void);
void pedometer_getValue(void);
void pedometer_getValidValue(void);

/**
 * @brief Handler for timer events.
 */
void timer_mpu_event_handler(nrf_timer_event_t event_type, void* p_context)
{
    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
            stat_Calc = 0;
            break;

        default:
            //Do nothing.
            break;
    }
}

void pedometer_init()
{

    mpu6050_init();

	stepCount = 0;
	pedometer_updateModelAxis();
	_curr_val = 0;

    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG(0);
    err_code = nrf_drv_timer_init(&TIMER_MPU, &timer_cfg, timer_mpu_event_handler);
    APP_ERROR_CHECK(err_code);
    time_ticks = nrf_drv_timer_ms_to_ticks(&TIMER_MPU, time_ms);
    nrf_drv_timer_extended_compare(
         &TIMER_MPU, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
}

void pedometer_updateModelAxis(void)
{
    int16_t x,y,z;
    int32_t sum[MAX_AXIS] = {0};
    float a,b,c;

    for(int i = 0; i < SAMPLING_MODEL_NUMBER; i++){
		mpu6050_readAccelerometer(&x, &y, &z);
        sum[X_AXIS] += abs(x);
        sum[Y_AXIS] += abs(y);
        sum[Z_AXIS] += abs(z);
    }
    sum[X_AXIS] /= SAMPLING_MODEL_NUMBER;
    sum[Y_AXIS] /= SAMPLING_MODEL_NUMBER;
    sum[Z_AXIS] /= SAMPLING_MODEL_NUMBER;
    _model_axis = sum[X_AXIS] >= sum[Y_AXIS]?X_AXIS:Y_AXIS;
    _model_axis = sum[_model_axis] >= sum[Z_AXIS]?_model_axis:Z_AXIS;
    _model_val = sum[_model_axis];
    
    _model_ratio = (_model_val+MODEL_STANDARD_VALUE/2)/MODEL_STANDARD_VALUE;
}

void pedometer_getValue(void)
{
    int16_t tmp_val[MAX_AXIS];
    _curr_val = 0;
    for(int i = 0; i < MAX_WINDOW; i++){
        mpu6050_readAccelerometer(&tmp_val[X_AXIS], &tmp_val[Y_AXIS], &tmp_val[Z_AXIS]);
        _curr_val += abs(tmp_val[_model_axis]);
    }
    _curr_val /= MAX_WINDOW;
    
    _curr_val = (_curr_val + _model_ratio/2)/_model_ratio;
}
void pedometer_getValidValue(void)
{
    uint32_t i = 0;
	do{
        i++;
        if (i%100 == 0)
        {
            //printf("getValidValue:_curr_val = %d ,_model_val = %d ,_last_val = %d  \n\r", _curr_val,_model_val,_last_val);

        }
        pedometer_getValue(); 
    }while((abs(_curr_val-_model_val) <= 2) || (_curr_val == _last_val));

    _last_val = _curr_val;
}

uint32_t pedometer_stepCalc(void)
{
    stat_Calc =1;
	nrf_drv_timer_enable(&TIMER_MPU);
    pedometer_getValidValue();
    
    if(_curr_val >= 23){
        while(1){
            pedometer_getValidValue();  
            if(_curr_val <= 17){
                if(stat_Calc == 0) {
                    stepCount++;
                    nrf_drv_timer_enable(&TIMER_MPU);
                }
                break;
            }
        }
    }
    return stepCount;
}