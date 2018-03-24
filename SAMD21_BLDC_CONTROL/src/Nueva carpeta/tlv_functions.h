/*
 * tlv_functions.h
 *
 * Created: 18/02/2018 20:23:01
 *  Author: Alberto
 */ 


#ifndef TLV_FUNCTIONS_H_
#define TLV_FUNCTIONS_H_


#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <arm_math.h>

#include "tlv_i2c.h"
#include "tlv_tc.h"

#define TLV_POWER_PIN				PIN_PB23
#define TLV_I2C_SERCOM				SERCOM5
#define TLV_I2C_SDA_PIN				PIN_PB16	
#define TLV_I2C_SDA_PINMUX			PINMUX_PB16C_SERCOM5_PAD0
#define TLV_I2C_SCL_PIN				PIN_PB17	
#define TLV_I2C_SCL_PINMUX			PINMUX_PB17C_SERCOM5_PAD1



/*! @brief Initializes TLV sensor

    Sets configuration and performs a reset of the device
*/
void tlv_init(void);

/*! @brief Resets the TLV Magnetic Sensor

    At start or after ADC hang up is sometimes convenient to reset the IC to ensure that it's working.
*/
uint8_t tlv_reset(void);

/*! @brief Powers down the TLV sensor
*/
void tlv_disable(void);

/*! @brief Starts reading the TLV sensor data

	Starts the reading process job.
*/	
void tlv_read (void);


uint8_t tlv_check_data_sanity(void);

/*! @brief Calculates the angle, speed and rev_count
*/
void tlv_calculate_angle(void);

/*! @brief Configures the Magnetic sensor registers

	@param		intscl Interrupt Pad Enabled
	@param		fast Fast mode
	@param		low Low Power Mode
	@param		temp Temperature 
	@param		lp_period Low Power Period
	@param		array Initial data read to write with the selected configuration
*/
uint8_t tlv_write (uint8_t intscl, uint8_t fast, uint8_t low, uint8_t temp, uint8_t lp_period, uint8_t array[]);

/*! @brief Aligns electric angle with position sensor
*/
void tlv_angle_zero(void);

/*! @brief Returns the angle of the shaft
*/
float32_t tlv_angle(void);

/*! @brief Returns the speed of the shaft
*/
float32_t tlv_speed(void);

/*! @brief Returns the direction of the shaft

	CW = -1 ; CCW = 1 ; stop = 0
*/
int8_t tlv_direction(void);

/*! @brief Returns the rev_count of the shaft
*/
int16_t tlv_rev_count(void);

void tlv_wait_revs(int16_t rev_wait);

/*! @brief Faster Atan Approximation

Source: https://www.dsprelated.com/showarticle/1052.php
Atan approximation graph: https://graphsketch.com/?eqn1_color=1&eqn1_eqn=atan(x)&eqn2_color=2&eqn2_eqn=&eqn3_color=3&eqn3_eqn=(pi%2F4%20%2B%200.273(1%20-%20mod(x)))*x&eqn4_color=4&eqn4_eqn=&eqn5_color=5&eqn5_eqn=&eqn6_color=6&eqn6_eqn=&x_min=-2.5&x_max=2.5&y_min=-1.57&y_max=1.57&x_tick=.1&y_tick=.1&x_label_freq=5&y_label_freq=5&do_grid=0&do_grid=1&bold_labeled_lines=0&bold_labeled_lines=1&line_width=4&image_w=850&image_h=525

*/
float32_t ApproxAtan(float32_t z);

/*! @brief Atan2 Approximation
*/
float32_t ApproxAtan2(float32_t y, float32_t x);



#endif /* TLV_FUNCTIONS_H_ */