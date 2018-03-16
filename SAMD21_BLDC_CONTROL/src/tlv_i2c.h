/*
 * tlv_i2c.h
 *
 * Created: 27/02/2018 12:06:43
 *  Author: Alberto
 */ 


#ifndef TLV_I2C_H_
#define TLV_I2C_H_

#include <stdbool.h>
#include <stdint.h>
#include <port.h>

#define TLV_POWER_PIN				PIN_PB23
#define TLV_I2C_SERCOM				SERCOM5
#define TLV_I2C_SDA_PIN				PIN_PB16	
#define TLV_I2C_SDA_PINMUX			PINMUX_PB16C_SERCOM5_PAD0
#define TLV_I2C_SCL_PIN				PIN_PB17	
#define TLV_I2C_SCL_PINMUX			PINMUX_PB17C_SERCOM5_PAD1

// From module: SERCOM I2C - Master Mode I2C (Callback APIs)
#include <i2c_master.h>
#include <i2c_master_interrupt.h>


void tlv_i2c_configure_callbacks(void);



/*! @brief Configure the I2C bus pins, baudrate, GLCK.. for the TLV sensor
*/
void tlv_i2c_configure(void);

void tlv_i2c_disable(void);


/*! @brief Reads the i2c bus

    @param		address Address of the i2c device
	@param		data_i2c Bytes to read from the bus
	@param		array[] Array to store the read values
*/
bool tlv_i2c_read(uint16_t address, uint16_t data_size, uint8_t array[]);


/*! @brief Writes to the i2c bus

    @param		address Address of the i2c device
	@param		data_i2c Bytes to write to the bus
	@param		array[] Array with the values to write
*/
bool tlv_i2c_write(uint16_t address, uint16_t data_size, uint8_t array[]);


#endif /* TLV_I2C_H_ */