/*
 * spi_16.c
 *
 * Created: 28/02/2018 12:18:50
 *  Author: Alberto
 */ 

#include "drv_spi.h"
//#include <conf_spi.h>

//#include "asf.h"
#include <spi.h>
#include <board.h>
//#include <stdint.h>

//! [setup]
//! [slave_select_pin]
#define SLAVE_SELECT_PIN CONF_MASTER_SS_PIN
//! [dev_inst]
struct spi_module spi_master_instance;
//! [dev_inst]
//! [slave_dev_inst]
struct spi_slave_inst drv;
//! [slave_dev_inst]
//! [setup]



/*! @brief Configures SPI as Master Device, initializes DRV83XX as SPI slave
*/
void configure_spi_master_drv(void)
{
	//! [config]
	struct spi_config config_spi_master;
	//! [config]
	//! [slave_config]
	struct spi_slave_inst_config slave_dev_config;
	//! [slave_config]
	/* Configure and initialize software device instance of peripheral slave */
	//! [slave_conf_defaults]
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	//! [slave_conf_defaults]
	//! [ss_pin]
	slave_dev_config.ss_pin = SLAVE_SELECT_PIN;
	//! [ss_pin]
	//! [slave_init]
	spi_attach_slave(&drv, &slave_dev_config);
	//! [slave_init]
	/* Configure, initialize and enable SERCOM SPI module */
	//! [conf_defaults]
	spi_get_config_defaults(&config_spi_master);
	//! [conf_defaults]
	//! [mux_setting]
	config_spi_master.mux_setting = CONF_MASTER_MUX_SETTING;
	//! [mux_setting]
	config_spi_master.generator_source = GCLK_GENERATOR_1;
	
	config_spi_master.pinmux_pad0 = CONF_MASTER_PINMUX_PAD0;
	config_spi_master.pinmux_pad1 = CONF_MASTER_PINMUX_PAD1;
	config_spi_master.pinmux_pad2 = CONF_MASTER_PINMUX_PAD2;
	config_spi_master.pinmux_pad3 = CONF_MASTER_PINMUX_PAD3;
	//! CPOL (clock polarity) = 0 and CPHA (clock phase) = 1
	config_spi_master.transfer_mode    = SPI_TRANSFER_MODE_1;

	//! [init]
	spi_init(&spi_master_instance, CONF_MASTER_SPI_MODULE, &config_spi_master);
	//! [init]

	//! [enable]
	spi_enable(&spi_master_instance);
	//! [enable]

}

/*! @brief Enables SPI Slave DRV83XX
*/
void spi_enable_slave_drv(void)
{
	spi_select_slave(&spi_master_instance, &drv, true);
}

/*! @brief Disables SPI Slave DRV83XX
*/
void spi_disable_slave_drv(void)
{
	spi_select_slave(&spi_master_instance, &drv, false);
}


/*! @brief SPI 16 bit transceiver
*/
uint16_t spi_transceiver_16 (uint16_t data)
{
	uint16_t read_byte_str[1]={0};
	uint8_t read_byte_1 = 0;
	uint8_t read_byte_0 = 0;
	uint8_t write_byte_1 = 0;
	uint8_t write_byte_0 = 0;
	write_byte_1 = data>>8;
	write_byte_0 = (uint8_t) data;
	
	spi_enable_slave_drv();
	spi_transceive_wait(&spi_master_instance, write_byte_1, read_byte_str);
	read_byte_1 = (uint8_t) read_byte_str[0];
	spi_transceive_wait(&spi_master_instance, write_byte_0, read_byte_str);
	read_byte_0 = (uint8_t) read_byte_str[0];
	spi_disable_slave_drv();
	
	return (read_byte_1<<8)|read_byte_0;

}
