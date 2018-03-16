/*
 * spi_16.h
 *
 * Created: 28/02/2018 12:19:09
 *  Author: Alberto
 */ 


#ifndef DRV_SPI_H_
#define DRV_SPI_H_

#include <stdint.h>

/*! @brief Configures SPI as Master Device, initializes DRV83XX as SPI slave
*/
void configure_spi_master_drv(void);

/*! @brief Enables SPI Slave DRV83XX
*/
void spi_enable_slave_drv(void);

/*! @brief Disables SPI Slave DRV83XX
*/
void spi_disable_slave_drv(void);

/*! @brief SPI 16 bit transceiver
*/
uint16_t spi_transceiver_16 (uint16_t data);


#endif /* SPI_16_H_ */