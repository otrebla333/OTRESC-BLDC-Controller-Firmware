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

#include <tcc.h>
#include <tcc_callback.h>

#include <conf_tcc.h>

#include <tlv_functions.h>

void drv_tcc0_configure(void);

void drv_tcc2_configure(void);

void drv_tcc2_callback(struct tcc_module *const module_inst);

void drv_tcc2_configure_callbacks(void);

void drv_set_position_rad(float desired_position);

void drv_set_duty_cycle(uint8_t duty_cycle);

void tcc2_sinusoidal_waveform(float32_t angle_rad_desired, uint8_t duty_cycle);



#endif /* DRV_TCC_H_ */