/*
 * DRV8305.h
 *
 * Created: 19/02/2018 19:05:54
 *  Author: Alberto
 */ 


#ifndef DRV_FUNCTIONS_H_
#define DRV_FUNCTIONS_H_

//#include <avr/io.h>

//#include <asf.h>
#include <port.h>
#include <board.h>

/*! @brief Configures DRV PWM_MODE
*/
void drv_config_pwm_mode(uint8_t mode);

/*! @brief Initializes DRV SPI and pins
*/
void drv_init(void);

/*! @brief Pulls up the ENGATE pin, enables DRV 
*/
void drv_enable(void);

/*! @brief Pulls down ENGATE pin
*/
void drv_disable(void);

/*! @brief Puts DRV into sleep mode. ATTENTION! this resets all the SPI registers
*/
void drv_sleep(void);

/*! @brief Wakes up DRV from sleep mode. ATTENTION! SPI Registers will be erased
*/
void drv_wake(void);

/*! @brief Detects DRV fault
	@return 0 No faults
			1 Fault detected
*/
uint8_t drv_fault(void);

/*! @brief Sets bits in the selected register, NO SENDING to DRV via SPI , only local
*/
void drv_config_set(uint16_t register_address, uint16_t bit_set);

/*! @brief Clears bits in the selected register, NO SENDING to DRV via SPI , only local
*/
void drv_config_clear(uint16_t register_address, uint16_t bit_clear);

/*! @brief Writes the selected register to the DRV via SPI from local register
*/
void drv_config_write_one(uint16_t register_address);

/*! @brief Reads the selected register from the DRV via SPI, stores it in local register
*/
void drv_config_read_one(uint16_t register_address);

/*! @brief Writes all the register to the DRV via SPI from the local registers
*/
void drv_config_write_all(void);

/*! @brief Reads all thed register from the DRV via SPI, stores it in local registers
*/
void drv_config_read_all(void);



#define DRV_N_REGISTERS				0xC

#define DRV_ENGATE					PIN_PA20
#define DRV_nFAULT					PIN_PA21
#define DRV_PWRGD					PIN_PB14
#define DRV_WAKE					PIN_PB15

//#define PWMHA			IOPORT_CREATE_PIN(PORTD,PORTD5)
//#define PWMLA			IOPORT_CREATE_PIN(PORTD,PORTD4)
//#define PWMHB			IOPORT_CREATE_PIN(PORTD,PORTD3)
//#define PWMLB			IOPORT_CREATE_PIN(PORTD,PORTD2)
//#define PWMHC			IOPORT_CREATE_PIN(PORTD,PORTD1)
//#define PWMLC			IOPORT_CREATE_PIN(PORTD,PORTD0)

#define DRV_PWM_MODE_6INPUT		0
#define DRV_PWM_MODE_3INPUT		1
#define DRV_PWM_MODE_1INPUT		2

//Warning and Watchdog Reset Registers (Address = 0x1)

#define DRV_1_ADDRESS				0x0800
#define DRV_1_FAULT					0x0400
#define DRV_1_TEMP_FLAG4			0x0100
#define DRV_1_PVDD_UVFL				0x0080
#define DRV_1_PVDD_OVFL				0x0040
#define DRV_1_VDS_STATUS			0x0020
#define DRV_1_VCHP_UVFL				0x0010
#define DRV_1_TEMP_FLAG1			0x0008
#define DRV_1_TEMP_FLAG2			0x0004
#define DRV_1_TEMP_FLAG3			0x0002
#define DRV_1_OTW					0x0001


//OV/VDS Faults Registers (Address = 0x2)

#define DRV_2_ADDRESS				0x1000
#define DRV_2_VDS_HA				0x0400
#define DRV_2_VDS_LA				0x0200
#define DRV_2_VDS_HB				0x0100
#define DRV_2_VDS_LB				0x0080
#define DRV_2_VDS_HC				0x0040
#define DRV_2_VDS_LC				0x0020
#define DRV_2_SNS_C_OCP				0x0004
#define DRV_2_SNS_B_OCP				0x0002
#define DRV_2_SNS_A_OCP				0x0001


//IC Faults RegisterS  (Address = 0x3)

#define DRV_3_ADDRESS				0x1800
#define DRV_3_PVDD_UVLO2			0x0400
#define DRV_3_WD_FAULT				0x0200
#define DRV_3_OTSD					0x0100
#define DRV_3_VREG_UV				0x0040
#define DRV_3_AVDD_UVLO				0x0020
#define DRV_3_VCP_LSD_UVLO2			0x0010
#define DRV_3_VCPH_UVLO2			0x0004
#define DRV_3_VCHP_OVLO				0x0002
#define DRV_3_VCHP_OVLO_ABS			0x0001


//Gate Driver VGS Faults Register Description  (Address = 0x4)

#define DRV_4_ADDRESS				0x2000
#define DRV_4_VGS_HA				0x0400
#define DRV_4_VGS_LA				0x0200
#define DRV_4_VGS_HB				0x0100
#define DRV_4_VGS_LB				0x0080
#define DRV_4_VGS_HC				0x0040
#define DRV_4_VGS_LC				0x0020

// HS Gate Driver Control Registers (Address = 0x5)

#define DRV_5_ADDRESS				0x2800
#define DRV_5_TDRIVEN1				0x0200
#define DRV_5_TDRIVEN0				0x0100
#define DRV_5_IDRIVEN_HS3			0x0080
#define DRV_5_IDRIVEN_HS2			0x0040
#define DRV_5_IDRIVEN_HS1			0x0020
#define DRV_5_IDRIVEN_HS0			0x0010
#define DRV_5_IDRIVEP_HS3			0x0008
#define DRV_5_IDRIVEP_HS2			0x0004
#define DRV_5_IDRIVEP_HS1			0x0002
#define DRV_5_IDRIVEP_HS0			0x0001


// LS Gate Driver Control Registers (Address = 0x6)

#define DRV_6_ADDRESS				0x3000
#define DRV_6_TDRIVEP1				0x0200
#define DRV_6_TDRIVEP0				0x0100
#define DRV_6_IDRIVEN_LS3			0x0080
#define DRV_6_IDRIVEN_LS2			0x0040
#define DRV_6_IDRIVEN_LS1			0x0020
#define DRV_6_IDRIVEN_LS0			0x0010
#define DRV_6_IDRIVEP_LS3			0x0008
#define DRV_6_IDRIVEP_LS2			0x0004
#define DRV_6_IDRIVEP_LS1			0x0002
#define DRV_6_IDRIVEP_LS0			0x0001

// Gate Driver Control Registers (Address = 0x7)

#define DRV_7_ADDRESS				0x3800
#define DRV_7_COMM_OPTION			0x0200
#define DRV_7_PWM_MODE1				0x0100
#define DRV_7_PWM_MODE0				0x0080
#define DRV_7_DEAD_TIME2			0x0040
#define DRV_7_DEAD_TIME1			0x0020
#define DRV_7_DEAD_TIME0			0x0010
#define DRV_7_TBLANK1				0x0008
#define DRV_7_TBLANK0				0x0004
#define DRV_7_TVDS1					0x0002
#define DRV_7_TVDS0					0x0001

// Gate Driver Control Registers (Address = 0x9)

#define DRV_9_ADDRESS				0x4800
#define DRV_9_FLIP_OTSD				0x0400
#define DRV_9_DIS_PVDD_UVLO2		0x0200
#define DRV_9_DIS_GDRV_FAULT		0x0100
#define DRV_9_EN_SNS_CLAMP			0x0080
#define DRV_9_WD_DLY1				0x0040
#define DRV_9_WD_DLY0				0x0020
#define DRV_9_DIS_SNS_OCP			0x0010
#define DRV_9_WD_EN					0x0008
#define DRV_9_SLEEP					0x0004
#define DRV_9_CLR_FLTS				0x0002
#define DRV_9_SET_VCPH_UV			0x0001

// Shunt Amplifier Control Register (Address = 0xA)

#define DRV_A_ADDRESS				0x5000
#define DRV_A_DC_CAL_CH3			0x0400
#define DRV_A_DC_CAL_CH2			0x0200
#define DRV_A_DC_CAL_CH1			0x0100
#define DRV_A_CS_BLANK1				0x0080
#define DRV_A_CS_BLANK0				0x0040
#define DRV_A_GAIN_CS31 			0x0020
#define DRV_A_GAIN_CS30 			0x0010
#define DRV_A_GAIN_CS21				0x0008
#define DRV_A_GAIN_CS20 			0x0004
#define DRV_A_GAIN_CS11 			0x0002
#define DRV_A_GAIN_CS10 			0x0001

// Voltage Regulator Control Register (Address = 0xB)

#define DRV_B_ADDRESS				0x5000
#define DRV_B_VREF_SCALE1 			0x0200
#define DRV_B_VREF_SCALE0 			0x0100
#define DRV_B_SLEEP_DLY1 			0x0010
#define DRV_B_SLEEP_DLY0			0x0008
#define DRV_B_DIS_VREG_PWRGD 		0x0004
#define DRV_B_VREG_UV_LEVEL1 		0x0002
#define DRV_B_VREG_UV_LEVEL0 		0x0001

// VDS Sense Control Register (Address = 0xC)

#define DRV_C_ADDRESS				0x5000
#define DRV_C_VDS_LEVEL4 			0x0080
#define DRV_C_VDS_LEVEL3			0x0040
#define DRV_C_VDS_LEVEL2 			0x0020
#define DRV_C_VDS_LEVEL1 			0x0010
#define DRV_C_VDS_LEVEL0			0x0008
#define DRV_C_VDS_MODE2				0x0004
#define DRV_C_VDS_MODE1 			0x0002
#define DRV_C_VDS_MODE0 			0x0001




#endif /* DRV_FUNCTIONS_H_ */