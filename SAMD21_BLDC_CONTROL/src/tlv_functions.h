/*
 * tlv_functions.h
 *
 * Created: 18/02/2018 20:23:01
 *  Author: Alberto
 */ 


#ifndef TLV_FUNCTIONS_H_
#define TLV_FUNCTIONS_H_


#include <stdbool.h>


#define TLV_POWER_PIN				PIN_PB23
#define TLV_I2C_SERCOM				SERCOM5
#define TLV_I2C_SDA_PIN				PIN_PB16	
#define TLV_I2C_SDA_PINMUX			PINMUX_PB16C_SERCOM5_PAD0
#define TLV_I2C_SCL_PIN				PIN_PB17	
#define TLV_I2C_SCL_PINMUX			PINMUX_PB17C_SERCOM5_PAD1


/*! @brief Starts the TLV sensor
TLV config + TLV reset
*/
void tlv_init(void);

/*! @brief Resets the TLV Magnetic Sensor

    At start or after ADC hang up is sometimes convenient to reset the IC to ensure that it's working.
*/
bool tlv_reset(void);

/*! @brief Power down the TLV sensor
*/
void tlv_disable(void);

/*! @brief Reads TLV sensor data

	Reads TLV in Master Mode and calculates the angle, speed, direction and rev_count of the spinning shaft
*/	
bool tlv_read (void);

/*! @brief Configures the Magnetic sensor registers

	@param		intscl Interrupt Pad Enabled
	@param		fast Fast mode
	@param		low Low Power Mode
	@param		temp Temperature 
	@param		lp_period Low Power Period
	@param		array Initial data read to write with the selected configuration
*/
bool tlv_write (bool intscl, bool fast, bool low, bool temp, bool lp_period,unsigned char array[]);

/*! @brief Returns the angle of the shaft
*/
float tlv_angle(void);

/*! @brief Returns the speed of the shaft
*/
float tlv_speed(void);

/*! @brief Returns the direction of the shaft

	CW = -1 ; CCW = 1 ; stop = 0
*/
signed int tlv_direction(void);

/*! @brief Returns the rev_count of the shaft
*/
signed int tlv_rev_count(void);

/*! @brief Faster Atan Approximation
*/
float ApproxAtan(float z);

/*! @brief Atan2 Approximation
*/
float ApproxAtan2(float y, float x);



#endif /* TLV_FUNCTIONS_H_ */