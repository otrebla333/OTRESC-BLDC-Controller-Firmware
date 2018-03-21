/*
 * drv_tcc.c
 *
 * Created: 06/03/2018 13:14:38
 *  Author: Alberto
 */ 


#include "drv_tcc.h"


#define TCC0_PERIOD		0x007F
#define TCC2_PERIOD		0x0020

#define PI_x2			6.28319f
#define PI_1			3.14156f
#define PI_2			1.57079f
#define PI_3_5			0.8976f
#define PI_7			0.4488f
#define PI_14			0.2244f

//! [module_inst]
struct tcc_module tcc0_instance;
struct tcc_module tcc2_instance;
//! [module_inst]
int8_t drv_direction = -1;

volatile float angle_desired_global = 0.0f;
float32_t angle_rad_elec_global = 0.0f;
volatile uint8_t duty_cycle_global = 0x00;
volatile uint8_t duty_cycle_global_max = 0x77;
volatile float32_t speed_rad_global = 0x0000;
uint8_t drv_is_ready = 0;

void drv_div_test(void);


volatile static float32_t t_step = 0.0f;
//! [callback_funcs]
void drv_tcc2_callback(
	struct tcc_module *const module_inst)
{
		
	if(drv_is_ready)
	{
		drv_set_position_rad(0.0f);
		drv_elec_sinusoidal_waveform(angle_rad_elec_global, duty_cycle_global);
	}
	else
	{
		drv_set_speed_rad(speed_rad_global);
		drv_elec_sinusoidal_waveform(angle_rad_elec_global,duty_cycle_global);
	}

}

uint8_t drv_div7_angle(float32_t angle_to_divide)
{
	static float32_t angle_to_divide_ant = 0;
	uint8_t div_result;
	static uint8_t div_result_ant;
	
	if(angle_to_divide != angle_to_divide_ant){
		div_result = (int)((angle_to_divide + PI_1) / PI_3_5);
		angle_to_divide_ant = angle_to_divide;
		div_result_ant = div_result;
		return div_result;
	}
	return div_result_ant;
}

float32_t rad_to_rad_elec(float32_t angle_rad)
{
	float32_t angle_rad_elec = 0;
	angle_rad_elec = ((angle_rad + PI_1) - (PI_3_5 * drv_div7_angle(angle_rad))) * 7;
	return angle_rad_elec;
	
}



	volatile int16_t duty_cycle_PID = 0;
	float32_t error_rad = 0;
	volatile static float32_t error_rad_ant = 0.0f;
	volatile static float32_t integral_rad = 0.0f;
	volatile float32_t derivative_rad = 0.0f;
	volatile float32_t next_angle_elec = 0.0f;
void drv_set_position_rad(float32_t desired_position)
{
	const float32_t Kp = 100;
	const float32_t Kd =  10000;
	const float32_t Ki = 0.15;
	
	const float32_t max_speed_step = 0.2f;
	const float32_t integral_rad_max = 400;
	const int16_t duty_cycle_min = 0x30;

	float32_t actual_position = tlv_angle();
	error_rad = (desired_position - actual_position);
	integral_rad = integral_rad + error_rad;
	if (integral_rad > integral_rad_max) integral_rad = integral_rad_max;
	else if (integral_rad < - integral_rad_max)  integral_rad = - integral_rad_max;
	
	derivative_rad = error_rad - error_rad_ant;
	error_rad_ant = error_rad;
	//Limits the max_step
	if(error_rad > 0) 
	{
		drv_direction = 1;
		if (error_rad > max_speed_step) desired_position = actual_position + max_speed_step;
		else desired_position = actual_position + error_rad;
	}
	else if (error_rad < 0) 
	{
		drv_direction = -1;
		if (error_rad < (max_speed_step * (-1))) desired_position = actual_position - max_speed_step;
		else desired_position = actual_position + error_rad;
	}
	
	
	//duty_cycle_PID = (int16_t)((error_rad * Kp));
	duty_cycle_PID = (int16_t)(((error_rad * Kp) + (Kd * derivative_rad)));
	//duty_cycle_PID = ((Kp * error_rad) + (Ki * integral_rad) + (Kd * derivative_rad));
	
	if (duty_cycle_PID < 0) duty_cycle_PID =  ~duty_cycle_PID;
	duty_cycle_PID = duty_cycle_PID + duty_cycle_min;
	//else duty_cycle_PID = duty_cycle_PID + duty_cycle_min;
	//if (duty_cycle_PID < duty_cycle_min) duty_cycle_PID = duty_cycle_min ;
	if (duty_cycle_PID > duty_cycle_global_max ) duty_cycle_PID = duty_cycle_global_max;
	duty_cycle_global = (uint8_t) duty_cycle_PID;
	
	next_angle_elec = rad_to_rad_elec(desired_position) - PI_1;
	
	angle_rad_elec_global = next_angle_elec;
	
}

void drv_set_angle_rad(float32_t angle_desired)
{
	angle_desired_global = angle_desired;
}


void drv_set_angle_rad_elec(float32_t angle_desired)
{
	angle_rad_elec_global = angle_desired;
}

void drv_set_speed_rad(float32_t set_speed_rad)
{
	speed_rad_global = set_speed_rad;
	
	static int i = 0;
	static float32_t next_angle_elec = 0;
	const float32_t speed_tick = ((speed_rad_global / 7324)*7);
	if (i >600){
		i = 0;
		t_step = t_step + 0.001;
		if (t_step > speed_tick) t_step = speed_tick;
	}
	else i++;
	
	if(speed_rad_global != 0.0f) {
		next_angle_elec = next_angle_elec + t_step;
		if (next_angle_elec > PI_1) next_angle_elec = next_angle_elec - PI_x2;
		else if (next_angle_elec < (-PI_1)) next_angle_elec = next_angle_elec + PI_x2;
		angle_rad_elec_global = next_angle_elec;
	}

	
	

}

void drv_set_duty_cycle(uint8_t duty_cycle)
{
		duty_cycle_global = duty_cycle;
}

void drv_elec_sinusoidal_waveform(float32_t angle_elec_rad_desired, uint8_t duty_cycle)
{
	
	volatile q15_t sin_angle_A_Q15 = 0.0f;
	volatile float32_t sin_angle_A_Q15f = 0.0f;

	volatile q15_t sin_angle_B_Q15 = 0.0f;
	volatile float32_t sin_angle_B_Q15f = 0.0f;

	volatile q15_t sin_angle_C_Q15 = 0.0f;
	volatile float32_t sin_angle_C_Q15f = 0.0f;

	q15_t angle_q15_desired = (q15_t)(((angle_elec_rad_desired + PI_1) / (2 * PI_1))*32768);
	const q15_t q15_120degrees = (q15_t)0x2AAB ; //120 degrees in Q15 from 0 to 1

	sin_angle_A_Q15 = arm_sin_q15(angle_q15_desired);
	sin_angle_A_Q15f = (float32_t)-sin_angle_A_Q15 / 32768;

	if((angle_q15_desired + q15_120degrees) >=0x7FFF) angle_q15_desired = angle_q15_desired - ( 2 * q15_120degrees);
	else angle_q15_desired = angle_q15_desired + q15_120degrees;
	sin_angle_B_Q15 = arm_sin_q15(angle_q15_desired);
	sin_angle_B_Q15f = (float32_t)-sin_angle_B_Q15 / 32768;
	
	if((angle_q15_desired + q15_120degrees) >=0x7FFF) angle_q15_desired = angle_q15_desired - ( 2 * q15_120degrees);
	else angle_q15_desired = angle_q15_desired + q15_120degrees;
	sin_angle_C_Q15 = arm_sin_q15(angle_q15_desired);
	sin_angle_C_Q15f = (float32_t)-sin_angle_C_Q15 / 32768;


	
	if(sin_angle_A_Q15f > 0.0f){
		tcc_set_pattern(&tcc0_instance, CONF_PWM0_OUTPUT_0, TCC_OUTPUT_PATTERN_DISABLE);
		tcc_set_pattern(&tcc0_instance, CONF_PWM0_OUTPUT_4, TCC_OUTPUT_PATTERN_0);
		tcc_set_compare_value(&tcc0_instance, CONF_PWM0_CHANNEL_0,(uint8_t) (TCC0_PERIOD-(duty_cycle * (sin_angle_A_Q15f))));
	}
	else
	{
		tcc_set_pattern(&tcc0_instance, CONF_PWM0_OUTPUT_4, TCC_OUTPUT_PATTERN_DISABLE);
		tcc_set_pattern(&tcc0_instance, CONF_PWM0_OUTPUT_0, TCC_OUTPUT_PATTERN_0);
		tcc_set_compare_value(&tcc0_instance, CONF_PWM0_CHANNEL_0,(uint8_t) (TCC0_PERIOD-(duty_cycle * (-sin_angle_A_Q15f))));
	}

	if(sin_angle_B_Q15f > 0.0f){
		tcc_set_pattern(&tcc0_instance, CONF_PWM0_OUTPUT_1, TCC_OUTPUT_PATTERN_DISABLE);
		tcc_set_pattern(&tcc0_instance, CONF_PWM0_OUTPUT_5, TCC_OUTPUT_PATTERN_0);
		tcc_set_compare_value(&tcc0_instance, CONF_PWM0_CHANNEL_1,(uint8_t) (TCC0_PERIOD-(duty_cycle * sin_angle_B_Q15f)));
	}
	else
	{
		tcc_set_pattern(&tcc0_instance, CONF_PWM0_OUTPUT_5, TCC_OUTPUT_PATTERN_DISABLE);
		tcc_set_pattern(&tcc0_instance, CONF_PWM0_OUTPUT_1, TCC_OUTPUT_PATTERN_0);
		tcc_set_compare_value(&tcc0_instance, CONF_PWM0_CHANNEL_1,(uint8_t) (TCC0_PERIOD-(duty_cycle * (-sin_angle_B_Q15f))));
	}
	
	if(sin_angle_C_Q15f > 0.0f){
		tcc_set_pattern(&tcc0_instance, CONF_PWM0_OUTPUT_2, TCC_OUTPUT_PATTERN_DISABLE);
		tcc_set_pattern(&tcc0_instance, CONF_PWM0_OUTPUT_6, TCC_OUTPUT_PATTERN_0);
		tcc_set_compare_value(&tcc0_instance, CONF_PWM0_CHANNEL_2,(uint8_t) (TCC0_PERIOD-(duty_cycle * sin_angle_C_Q15f)));
	}
	else
	{
		tcc_set_pattern(&tcc0_instance, CONF_PWM0_OUTPUT_6, TCC_OUTPUT_PATTERN_DISABLE);
		tcc_set_pattern(&tcc0_instance, CONF_PWM0_OUTPUT_2, TCC_OUTPUT_PATTERN_0);
		tcc_set_compare_value(&tcc0_instance, CONF_PWM0_CHANNEL_2,(uint8_t) (TCC0_PERIOD-(duty_cycle * (-sin_angle_C_Q15f))));
	}
}


void drv_set_ready(void)
{
	drv_is_ready = 1;
}
//! [callback_funcs]

//! [setup]
void drv_tcc0_configure(void)
{
	//! [setup_config]
	struct tcc_config config_tcc0;
	//! [setup_config]
	//! [setup_config_defaults]
	tcc_get_config_defaults(&config_tcc0, CONF_PWM0_MODULE);
	//! [setup_config_defaults]

	//! [setup_change_config]
	config_tcc0.counter.clock_source = GCLK_GENERATOR_1;
	config_tcc0.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV1;
	config_tcc0.counter.period = TCC0_PERIOD;
	config_tcc0.compare.wave_generation = TCC_WAVE_GENERATION_DOUBLE_SLOPE_BOTTOM;
	config_tcc0.compare.match[CONF_PWM0_CHANNEL_0] = TCC0_PERIOD/2;
	config_tcc0.compare.match[CONF_PWM0_CHANNEL_1] = TCC0_PERIOD/2;
	config_tcc0.compare.match[CONF_PWM0_CHANNEL_2] = TCC0_PERIOD/2;
	config_tcc0.compare.match[CONF_PWM0_CHANNEL_3] = TCC0_PERIOD/2;
	//! [setup_change_config]

	//! [setup_change_config_pwm]
	config_tcc0.pins.enable_wave_out_pin[CONF_PWM0_OUTPUT_0] = true;
	config_tcc0.pins.enable_wave_out_pin[CONF_PWM0_OUTPUT_1] = true;
	config_tcc0.pins.enable_wave_out_pin[CONF_PWM0_OUTPUT_2] = true;
	config_tcc0.pins.enable_wave_out_pin[CONF_PWM0_OUTPUT_3] = true;
	config_tcc0.pins.enable_wave_out_pin[CONF_PWM0_OUTPUT_4] = true;
	config_tcc0.pins.enable_wave_out_pin[CONF_PWM0_OUTPUT_5] = true;
	config_tcc0.pins.enable_wave_out_pin[CONF_PWM0_OUTPUT_6] = true;
	config_tcc0.pins.enable_wave_out_pin[CONF_PWM0_OUTPUT_7] = false;
	config_tcc0.pins.wave_out_pin[CONF_PWM0_OUTPUT_0]        = CONF_PWM0_OUT_PIN_0;
	config_tcc0.pins.wave_out_pin_mux[CONF_PWM0_OUTPUT_0]    = CONF_PWM0_OUT_MUX_0;
	config_tcc0.pins.wave_out_pin[CONF_PWM0_OUTPUT_1]        = CONF_PWM0_OUT_PIN_1;
	config_tcc0.pins.wave_out_pin_mux[CONF_PWM0_OUTPUT_1]    = CONF_PWM0_OUT_MUX_1;
	config_tcc0.pins.wave_out_pin[CONF_PWM0_OUTPUT_2]        = CONF_PWM0_OUT_PIN_2;
	config_tcc0.pins.wave_out_pin_mux[CONF_PWM0_OUTPUT_2]    = CONF_PWM0_OUT_MUX_2;
	config_tcc0.pins.wave_out_pin[CONF_PWM0_OUTPUT_3]        = CONF_PWM0_OUT_PIN_3;
	config_tcc0.pins.wave_out_pin_mux[CONF_PWM0_OUTPUT_3]    = CONF_PWM0_OUT_MUX_3;
	config_tcc0.pins.wave_out_pin[CONF_PWM0_OUTPUT_4]        = CONF_PWM0_OUT_PIN_4;
	config_tcc0.pins.wave_out_pin_mux[CONF_PWM0_OUTPUT_4]    = CONF_PWM0_OUT_MUX_4;
	config_tcc0.pins.wave_out_pin[CONF_PWM0_OUTPUT_5]        = CONF_PWM0_OUT_PIN_5;
	config_tcc0.pins.wave_out_pin_mux[CONF_PWM0_OUTPUT_5]    = CONF_PWM0_OUT_MUX_5;
	config_tcc0.pins.wave_out_pin[CONF_PWM0_OUTPUT_6]        = CONF_PWM0_OUT_PIN_6;
	config_tcc0.pins.wave_out_pin_mux[CONF_PWM0_OUTPUT_6]    = CONF_PWM0_OUT_MUX_6;
	config_tcc0.pins.wave_out_pin[CONF_PWM0_OUTPUT_7]        = CONF_PWM0_OUT_PIN_7;
	config_tcc0.pins.wave_out_pin_mux[CONF_PWM0_OUTPUT_7]    = CONF_PWM0_OUT_MUX_7;

	//! [setup_change_config_pwm]

	//Disabling double buffering to be able to use multiple tcc_set_pattern functions
	config_tcc0.double_buffering_enabled = false;

	//! [setup_set_config]
	tcc_init(&tcc0_instance, CONF_PWM0_MODULE, &config_tcc0);
	//! [setup_set_config]

	//! [setup_enable]
	tcc_enable(&tcc0_instance);
	//! [setup_enable]
}
//! [setup]

void drv_tcc2_configure_callbacks(void)
{
	//! [setup_register_callback]
	tcc_register_callback(
	&tcc2_instance,
	drv_tcc2_callback,
	(enum tcc_callback)(CONF_PWM2_CHANNEL_0));
	//! [setup_register_callback]

	//! [setup_enable_callback]
	tcc_enable_callback(&tcc2_instance,
	(enum tcc_callback)(CONF_PWM2_CHANNEL_0));
	//! [setup_enable_callback]
}

//! [setup]
void drv_tcc2_configure(void)
{
	//! [setup_config]
	struct tcc_config config_tcc2;
	//! [setup_config]
	//! [setup_config_defaults]
	tcc_get_config_defaults(&config_tcc2, CONF_PWM2_MODULE);
	//! [setup_config_defaults]

	//! [setup_change_config]
	config_tcc2.counter.clock_source = GCLK_GENERATOR_1;
	config_tcc2.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV16;
	config_tcc2.counter.period = TCC2_PERIOD;
	config_tcc2.compare.wave_generation = TCC_WAVE_GENERATION_DOUBLE_SLOPE_BOTTOM;
	config_tcc2.compare.match[CONF_PWM2_CHANNEL_0] = TCC2_PERIOD/2;
	//! [setup_change_config]

	//! [setup_change_config_pwm]
	config_tcc2.pins.enable_wave_out_pin[CONF_PWM2_OUTPUT_0] = true;
	config_tcc2.pins.wave_out_pin[CONF_PWM2_OUTPUT_0]        = CONF_PWM2_OUT_PIN_0;
	config_tcc2.pins.wave_out_pin_mux[CONF_PWM2_OUTPUT_0]    = CONF_PWM2_OUT_MUX_0;
	//! [setup_change_config_pwm]

	//! [setup_set_config]
	tcc_init(&tcc2_instance, CONF_PWM2_MODULE, &config_tcc2);
	//! [setup_set_config]

	//! [setup_enable]
	tcc_enable(&tcc2_instance);
	//! [setup_enable]
}
//! [setup]