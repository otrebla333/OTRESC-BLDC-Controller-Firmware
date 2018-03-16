/*
 * tlv_i2c.c
 *
 * Created: 27/02/2018 12:06:31
 *  Author: Alberto
 */ 

#include "tlv_i2c.h"

// From module: SERCOM I2C - Master Mode I2C (Callback APIs)
//#include <i2c_common.h>
//#include <i2c_master_interrupt.h>


//! [packet_data]
#define DATA_LENGTH 10


static uint8_t wr_buffer[DATA_LENGTH] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};

static uint8_t wr_buffer_reversed[DATA_LENGTH] = {
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
};

static uint8_t rd_buffer[DATA_LENGTH];
//! [packet_data]

//! [address]
//#define SLAVE_ADDRESS 0x1F
void tlv_i2c_read_callback(struct i2c_master_module *const module);
void tlv_i2c_write_callback(struct i2c_master_module *const module);
//! [packet_glob]
struct i2c_master_packet wr_packet;
struct i2c_master_packet wr_packet_reset;
struct i2c_master_packet rd_packet;


/* Init software module instance. */

struct i2c_master_module i2c_master_instance;



//! [initialize_i2c]
void tlv_i2c_configure(void)
{

	
	/* Initialize config structure and software module */
	//! [init_conf]
	struct i2c_master_config config_i2c_master;
	i2c_master_get_config_defaults(&config_i2c_master);
	//! [init_conf]
	
	/* Change baudrate and generator_source*/
	//! [conf_change]
	config_i2c_master.baud_rate = I2C_MASTER_BAUD_RATE_400KHZ;
	config_i2c_master.generator_source = GCLK_GENERATOR_1;
	config_i2c_master.pinmux_pad0    = TLV_I2C_SDA_PINMUX;
	config_i2c_master.pinmux_pad1    = TLV_I2C_SCL_PINMUX;
	//! [conf_change]

	/* Initialize and enable device with config */
	//! [init_module]
	while(i2c_master_init(&i2c_master_instance, TLV_I2C_SERCOM, &config_i2c_master)     \
			!= STATUS_OK);
	//while(i2c_master_init(&i2c_master_instance, CONF_I2C_MASTER_MODULE, &config_i2c_master)     
	//!= STATUS_OK);
	//! [init_module]
	
	//! [enable_module]
	i2c_master_enable(&i2c_master_instance);
	//! [enable_module]
	
}
//! [initialize_i2c]
volatile static uint8_t read_callback_count = 0xF0;
void tlv_i2c_read_callback(struct i2c_master_module *const module)
{
	port_pin_toggle_output_level(PIN_PB30);
	read_callback_count = read_callback_count + 1;
	i2c_master_read_packet_job(&i2c_master_instance, &rd_packet);

}

volatile static uint8_t write_callback_count = 0xF0;
void tlv_i2c_write_callback(struct i2c_master_module *const module)
{
	port_pin_toggle_output_level(PIN_PB30);
	write_callback_count = write_callback_count + 1;
	for (int i = 0; i<60000 ; i++){
		for (int i = 0; i<2 ; i++);
	}
	i2c_master_read_packet_job(&i2c_master_instance, &rd_packet);
}


void tlv_i2c_configure_callbacks(void)
{
	///* Register callback function. */
	//! [callback_reg]
	i2c_master_register_callback(&i2c_master_instance, tlv_i2c_read_callback,
	(enum i2c_master_callback) (I2C_MASTER_CALLBACK_READ_COMPLETE));
	//! [callback_reg]
		//! [callback_reg]
	//i2c_master_register_callback(&i2c_master_instance, tlv_i2c_write_callback,
			//I2C_MASTER_CALLBACK_WRITE_COMPLETE);
	//! [callback_reg]
	//! [callback_en]
	i2c_master_enable_callback(&i2c_master_instance,
	(enum i2c_master_callback) (I2C_MASTER_CALLBACK_READ_COMPLETE));
	//
	//i2c_master_enable_callback(&i2c_master_instance,
			//I2C_MASTER_CALLBACK_WRITE_COMPLETE);
	//! [callback_en]
}


//! [disable_i2c]
void tlv_i2c_disable(void)
{
	i2c_master_disable(&i2c_master_instance);
}
//! [disable_i2c]


/*! @brief Reads the i2c bus

    @param		address Address of the i2c device
	@param		data_i2c Bytes to read from the bus
	@param		array[] Array to store the read values
*/
bool tlv_i2c_read(uint16_t address, uint16_t data_size, uint8_t array[])
{
	
	rd_packet.address = address;
	rd_packet.data_length = data_size;
	rd_packet.data = array;
	
	//if((i2c_master_read_packet_wait(&i2c_master_instance, &rd_packet)) != STATUS_OK) return 1;
	i2c_master_read_packet_job(&i2c_master_instance, &rd_packet);
	return 0;
}


/*! @brief Reads the i2c bus

    @param		address Address of the i2c device
	@param		data_i2c Bytes to write to the bus
	@param		array[] Array with the values to write
*/
bool tlv_i2c_write(uint16_t address, uint16_t data_size, uint8_t array[])
{
	
	wr_packet.address = address;
	wr_packet.data_length = data_size;
	wr_packet.data = array;
	if((i2c_master_write_packet_wait(&i2c_master_instance, &wr_packet))!= STATUS_OK) return 1;

	return 0;
	
}
