/*
 * DRV8305.c
 *
 * Created: 19/02/2018 19:04:30
 *  Author: Alberto`
 */ 

#include "drv_functions.h"
#include "drv_spi.h"

uint16_t drv_config[DRV_N_REGISTERS+1];

void drv_config_pwm_mode(uint8_t mode)
{
	switch (mode)
	{
		case 0:
			drv_config_clear(DRV_7_ADDRESS, DRV_7_PWM_MODE1 | DRV_7_PWM_MODE0);
			break;
		case 1:
			drv_config_clear(DRV_7_ADDRESS, DRV_7_PWM_MODE1);
			drv_config_set(DRV_7_ADDRESS, DRV_7_PWM_MODE0);
			break;
		case 2:
			drv_config_clear(DRV_7_ADDRESS, DRV_7_PWM_MODE0);
			drv_config_set(DRV_7_ADDRESS, DRV_7_PWM_MODE1);
			break;
		default:
			drv_config_clear(DRV_7_ADDRESS, DRV_7_PWM_MODE1 | DRV_7_PWM_MODE0);
			break;
	}
	drv_config_write_one(DRV_7_ADDRESS);
}

void drv_init(void)
{
	
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	/* Configure ENGATE as outputs, turn it off */
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(DRV_ENGATE, &pin_conf);
	port_pin_set_config(DRV_WAKE, &pin_conf);
	port_pin_set_output_level(DRV_ENGATE, false);
	port_pin_set_output_level(DRV_WAKE, false);
	
	/* Configure nFAULT, PWRGD as input*/
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_DOWN;
	port_pin_set_config(DRV_nFAULT, &pin_conf);
	port_pin_set_config(DRV_PWRGD, &pin_conf);
	
	/* Configure SPI for drv pins */
	configure_spi_master_drv();
	
	/* Read configuration registers */
	drv_config_read_all();
}

void drv_enable(void)
{
	port_pin_set_output_level(DRV_ENGATE, true);
}

void drv_disable(void)
{
	port_pin_set_output_level(DRV_ENGATE, false);
}

void drv_sleep(void)
{
	drv_config_set(DRV_9_ADDRESS, DRV_9_SLEEP);
	drv_config_write_one(DRV_9_ADDRESS);

}

void drv_wake(void)
{
	port_pin_set_output_level(DRV_WAKE, true);
	port_pin_set_output_level(DRV_WAKE, false);
}


uint8_t drv_fault(void)
{
	return port_pin_get_input_level(DRV_nFAULT);

}

void drv_config_set(uint16_t register_address, uint16_t bit_set)
{
	drv_config[register_address >> 11 ] = (drv_config[register_address >> 11 ] | bit_set);
}

void drv_config_clear(uint16_t register_address, uint16_t bit_clear)
{
	drv_config[register_address >> 11 ] = (drv_config[register_address >> 11 ] & ~bit_clear);
}

void drv_config_write_one(uint16_t register_address)
{
	spi_transceiver_16((drv_config[register_address >> 11 ] | register_address) & 0x7FFF);
}

void drv_config_read_one(uint16_t register_address)
{
	drv_config[register_address >> 11] = spi_transceiver_16(register_address | 0x8000 );
}

void drv_config_write_all(void)
{	
	for (uint8_t i = 1; i <= DRV_N_REGISTERS ; i++)
	{
		spi_transceiver_16((drv_config[i] | (i << 11)) & 0x7FFF);		
	}
	
}

void drv_config_read_all(void)
{
	for (uint8_t i = 1; i <= DRV_N_REGISTERS ; i++)
	{
		drv_config[i] = spi_transceiver_16(((i) << 11) | 0x8000 );
	}
}

