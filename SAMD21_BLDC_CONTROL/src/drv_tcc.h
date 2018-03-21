/*
 * drv_tcc.h
 *
 * Created: 06/03/2018 13:14:49
 *  Author: Alberto
 */ 


#ifndef DRV_TCC_H_
#define DRV_TCC_H_

#include <asf.h>

#include <arm_math.h>
#include <math.h>

#include <tcc.h>
#include <tcc_callback.h>

#include <conf_tcc.h>

#include <tlv_functions.h>

void drv_tcc0_configure(void);

void drv_tcc2_configure(void);

void drv_tcc2_callback(struct tcc_module *const module_inst);

void drv_tcc2_configure_callbacks(void);

void drv_set_position_rad(float desired_position);

void drv_elec_sinusoidal_waveform(float32_t angle_rad_desired, uint8_t duty_cycle);

void drv_set_speed_rad(float32_t set_speed_rad);

void drv_set_angle_rad(float32_t angle_desired);

void drv_set_angle_rad_elec(float32_t angle_desired);

void drv_set_duty_cycle(uint8_t duty_cycle);

uint8_t drv_div7_angle(float32_t angle_to_divide);

void drv_set_ready(void);


#endif /* DRV_TCC_H_ */