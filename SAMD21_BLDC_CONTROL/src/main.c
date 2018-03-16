/**
 * \file
 *
 * \brief SAM LED Toggle Example
 *
 * Copyright (C) 2012-2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

/**
 * \mainpage SAM LED Toggle Example
 * See \ref appdoc_main "here" for project documentation.
 * \copydetails appdoc_preface
 *
 *
 * \page appdoc_preface Overview
 * This application demonstrates a simple example to toggle the board LED.
 */

/**
 * \page appdoc_main SAM LED Toggle Example
 *
 * Overview:
 * - \ref appdoc_sam0_led_toggle_app_intro
 * - \ref appdoc_sam0_led_toggle_app_usage
 * - \ref appdoc_sam0_led_toggle_app_compinfo
 * - \ref appdoc_sam0_led_toggle_app_contactinfo
 *
 * \section appdoc_sam0_led_toggle_app_intro Introduction
 * This application demonstrates a simple example to toggle the board LED.
 *
 * This application has been tested on following boards:
 * - SAM D20/D21/R21/D11/L21/L22/R30 Xplained Pro
 * - SAM D10 Xplained Mini
 * - SAMR21ZLL-EK
 *
 * \section appdoc_sam0_led_toggle_app_usage Usage
 * The application uses system timer to generate periodic interrupts, once the
 * interrupt occurs, LED0 will toggle.
 *
 * \section appdoc_sam0_led_toggle_app_compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for ARM.
 * Other compilers may or may not work.
 *
 * \section appdoc_sam0_led_toggle_app_contactinfo Contact Information
 * For further information, visit
 * <a href="http://www.atmel.com">http://www.atmel.com</a>.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>

#include "tlv_i2c.h"
#include "tlv_functions.h"

#include "drv_spi.h"
#include "drv_functions.h"

#include "drv_tcc.h"

#include "tlv_tc.h"


/** Handler for the device SysTick module, called when the SysTick counter
 *  reaches the set period.
 *
 *  \note As this is a raw device interrupt, the function name is significant
 *        and must not be altered to ensure it is hooked into the device's
 *        vector table.
 */
void SysTick_Handler(void)
{
	port_pin_toggle_output_level(LED_0_PIN);
}


/** Configure LED0, turn it off*/
static void config_led(void)
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_0_PIN, &pin_conf);
	port_pin_set_output_level(LED_0_PIN, LED_0_INACTIVE);
}

//_sercom_interrupt_handlers[5]= &SERCOM5_Handler_main;
//
//static void SERCOM5_Handler_main(const uint8_t instance)
//{
	//port_pin_toggle_output_level(LED_0_PIN);
//}


uint8_t test =0;
int main(void)
{
	
	
	
	system_init();
	

	tlv_init();
	tlv_i2c_configure_callbacks();

	system_interrupt_enable_global();
	/*Configure system tick to generate periodic interrupts */
	//SysTick_Config(12000000);

	config_led();
	
	drv_tcc0_configure();
	drv_tcc2_configure();
	drv_tcc2_configure_callbacks();
	
	

	//drv_init();
	
	//tlv_tc4_configure();
	//tlv_tc4_configure_callbacks();
	
	while (true) {
		
			/* Infinite loop */
			if(!port_pin_get_input_level(BUTTON_0_PIN)) {
				//! [select_slave]
				//drv_config_pwm_mode(DRV_PWM_MODE_1INPUT);
				//drv_enable();
				tlv_read();
				
				port_pin_set_output_level(LED_0_PIN,false);
				for (int i = 0; i<20000 ; i++);
				//! [light_up]
			}
			else
			{
				port_pin_set_output_level(LED_0_PIN,true);
				//port_pin_set_output_level(LED_0_PIN, drv_fault());
				//port_pin_set_output_level(LED_0_PIN, LED0_ACTIVE);
				//drv_disable();
				//tlv_read();
	
				
				//port_pin_set_output_level(LED_0_PIN, !test);
				//read_tlv();

			}
			
	}
}
