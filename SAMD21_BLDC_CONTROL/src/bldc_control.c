/*
 * bldc_control.c
 *
 * Created: 20/03/2018 12:29:20
 *  Author: Alberto
 */ 

#include "bldc_control.h"

void bldc_init(void)
{
	tlv_init();
	
	drv_tcc0_configure();
	drv_tcc2_configure();
	drv_tcc2_configure_callbacks();

	drv_init();
	
	bldc_set_zero();
	
	bldc_disable();
}

void bldc_enable(void)
{
	drv_enable();
}

void bldc_disable(void)
{
	drv_disable();
}

void bldc_set_zero(void)
{
	
	int16_t rev_count_ant = 0;
	
	rev_count_ant = tlv_rev_count();
	
	drv_elec_sinusoidal_waveform(0.0f, 0);
	drv_enable();
	drv_set_duty_cycle(0x4F);
	drv_set_speed_rad(10.0f);
	for (int i = 0; i<60000 ; i++)
	{
		for (int i = 0; i<1000 ; i++);
		if((tlv_rev_count()-rev_count_ant) > 1) break;
	}
	
	for (int i = 0; i<60000 ; i++)
	{
		for (int i = 0; i<1000 ; i++);
		if((tlv_angle() < 0.1f) && (tlv_angle() > -0.2f)) break;
	}
	drv_set_speed_rad(0.0);
	drv_set_duty_cycle(0x5F);
	drv_set_angle_rad_elec(0.0f);
	tlv_angle_zero();
	drv_disable();
	drv_set_ready();
	
}