/*
 * tlv_functions.c
 *
 * Created: 18/02/2018 20:19:58
 *  Author: Alberto
 */ 


#include "tlv_functions.h"

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#include "tlv_i2c.h"



/*! @brief Macro to set bit value
*/
#define _BV(bit) \
(1 << (bit))

/*! @brief Macro to clear bit value
*/
#define _BC(bit) \
(0 << (bit))



#define PIx2 6.28318531f
#define PI 3.14159265f
#define PI_2 1.57079633f

#define RESET_ADDRESS 0x00
//#define TLV_ADDRESS 0x1F		//addr0
#define TLV_ADDRESS 0x5E		//addr1
#define TLV_READ_ADDRESS 0xBD
#define TLV_WRITE_ADDRESS 0xBC
#define RX_SIZE 10

unsigned char RX_init[RX_SIZE];
unsigned char RX_init_2[RX_SIZE];
unsigned char RX[RX_SIZE];


void tlv_init(void)
{
	tlv_i2c_configure();
	
	tlv_reset();
	
	for(int i=0;i<60000;i++);
}

/*! @brief Resets the TLV Magnetic Sensor

    At start or after ADC hang up is sometimes convenient to reset the IC to ensure that it's working.
*/
bool tlv_reset(void)
{
	//configure_tlv_i2c();
	unsigned char rx_init[RX_SIZE];
	
	tlv_i2c_write(RESET_ADDRESS, 1, 0);
	
	tlv_i2c_disable();
	
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	/* Resets TLV power, holds high SDA line to set ADDR = 1 */
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(TLV_POWER_PIN, &pin_conf);
	port_pin_set_output_level(TLV_POWER_PIN, false);
	port_pin_set_config(TLV_I2C_SDA_PIN, &pin_conf);
	port_pin_set_config(TLV_I2C_SCL_PIN, &pin_conf);
	port_pin_set_output_level(TLV_I2C_SDA_PIN, true);
	port_pin_set_output_level(TLV_I2C_SCL_PIN, false);
	for(int i=0;i<16000;i++);
	port_pin_set_output_level(TLV_POWER_PIN, true);
	for(int i=0;i<16000;i++);
	
	tlv_i2c_configure();
	
	for(unsigned int i = 0; i < 600; i++);
	if((tlv_i2c_read(TLV_ADDRESS,RX_SIZE,rx_init))!=STATUS_OK)return 1;

	
	for(int i=0;i<10;i++){
		RX_init[i]=rx_init[i];
	}
	//
	for(unsigned int i = 0; i < 600; i++);
	
	if((tlv_write(0, 1, 1, 0, 1, rx_init))!= STATUS_OK)return 1;
	
	return 0;
}

/*! @brief Power down the TLV sensor
*/
void tlv_disable(void)
{
	tlv_i2c_disable();
	
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	/* Shuts down TLV sensor power */
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(TLV_POWER_PIN, &pin_conf);
	port_pin_set_output_level(TLV_POWER_PIN, false);
	

}

/*! @brief Configures the Magnetic sensor registers

	@param		intscl Interrupt Pad Enabled
	@param		fast Fast mode
	@param		low Low Power Mode
	@param		temp Temperature 
	@param		lp_period Low Power Period
	@param		array Initial data read to write with the selected configuration
*/
bool tlv_write (bool intscl, bool fast, bool low, bool temp, bool lp_period,unsigned char array[])
{
	unsigned char TX[4];
	uint8_t TX2[10]={0,1,2,3,4,5,6,7,8,9};
	TX[0]=0x00;
	TX[1]=( (array[7]&0b01111000) | (intscl<<2) | (fast<<1) | low);
	TX[2]=array[8];
	TX[3]=( (array[9]&0x00011111) | (!temp<<7) | (lp_period<<6));
	
	if((tlv_i2c_write(TLV_ADDRESS, 4, TX))!= STATUS_OK)return 1;
	return 0;
}


float angle_rad=0;
float speed_rad_2=0;
signed int dir=0;
signed int rev_count=0;
	
/*! @brief Reads TLV sensor data

	Reads TLV in Master Mode and calculates the angle, speed, direction and rev_count of the spinning shaft
*/	
bool tlv_read (void)
{
	
	
	unsigned char rx_read[RX_SIZE];
	
	//unsigned int Temperature=0;
	signed int Bx=0;
	signed int By=0;
	signed int Bz=0;
	unsigned char FRM=0;
	//unsigned char CH=0;
	//bool PD=0;
	

	static unsigned char FRM_old=3;
	static int i=0;
	static int j=0;

	if((tlv_i2c_read(TLV_ADDRESS,RX_SIZE,rx_read)) != STATUS_OK)return 1;
	//if (rx_read==1)return 1;

	//Temperature=(((((RX[3]&0xF0)<<4)|RX[6])-320)*1.1);
	Bx = (rx_read[0]<<4)|(rx_read[4]>>4);
	Bx = (Bx&0x07FF)-2048*((Bx&0x0800)>>11);
	By = (rx_read[1]<<4)|(rx_read[4]&0x0F);
	By = (By&0x07FF)-2048*((By&0x0800)>>11);
	Bz = (rx_read[2]<<4)|(rx_read[5]&0x0F);
	Bz = (Bz&0x07FF)-2048*((Bz&0x0800)>>11);
	
	FRM = (char)(rx_read[3]&0x0C)>>2;
	//CH = (char)(RX[3]&0x03);
	//PD = (bool)(RX[5]&0x10)>>4;
	if(FRM==FRM_old) return 1;
	else FRM_old=FRM;


	static signed int Bx_max=0;
	static signed int Bx_min=0;
	static signed int By_max=0;
	static signed int By_min=0;
	static signed int Bx_ant=0;
	static signed int By_ant=0;
	
	//Save min & max for reference
	if ((Bx<Bx_min)&&(Bx>(-2000))) Bx_min=Bx;
	else if ((Bx>Bx_max)&&(Bx<2000))Bx_max=Bx;
	if ((By<By_min)&&(By>(-2000))) By_min=By;
	else if ((By>By_max)&&(By<2000))By_max=By;
	
	//Check for sign change in Y measurement, that means 1 turn completed
	if(By==0)By=By_ant;			//Dirty fix for solving problems when By=0
	if((abs(By)+abs(By_ant))>abs(By+By_ant)){
		if(Bx<0){
			if(By<0) {
				dir=1;
				rev_count++;
			}
			else if(By>0) {
				dir=-1;
				rev_count--;
			}
		}
		else{
			if(By<0)dir=-1;
			else dir=1;		
		}
	}
	
	Bx_ant=Bx;
	By_ant=By;
	
	float x_rad=0;
	float y_rad=0;
	//Convert X Y measurement to rad
	x_rad=((float)Bx/(Bx_max-((Bx_max+Bx_min)/2)));
	if (x_rad>1.0f)x_rad=1.0f;
	else if (x_rad<(-1.0f))x_rad=(-1.0f);
	
	y_rad=((float)By/(By_max-((By_max+By_min)/2)));
	if (y_rad>1.0f)y_rad=1.0f;
	else if (y_rad<(-1.0f))y_rad=(-1.0f);

	//Calculates shaft angle
	angle_rad=ApproxAtan2(y_rad,x_rad);

	
	
	
	static signed int rev_count_ant=0;
	static signed int rev_count_ant_rev=0;

	float speed_rad_rev=0;
	
	//Counts revolutions in a second
	if(i==1243){
		i=0;
		speed_rad_rev=(rev_count-rev_count_ant_rev)*PIx2;
		rev_count_ant_rev=rev_count;
	}
	else i++;

	static float angle_rad_ant=0;
	float dif_rad=0;
	static float dif_rad_sum=0;
	float speed_rad=0;
	
	//Calculates actual speed, and averages speed every 100 measurements.
	dif_rad=angle_rad+((rev_count-rev_count_ant)*PIx2)-angle_rad_ant;
	dif_rad_sum=dif_rad+dif_rad_sum;
	rev_count_ant=rev_count;
	angle_rad_ant=angle_rad;
	speed_rad=((dif_rad)/0.000804f);
	
	if(j==99){
		j=0;
		if (((dif_rad_sum)>(-0.01f))&&((dif_rad_sum)<0.01f)) 
		{
		dir=0;
		speed_rad=0;
		speed_rad_2=0;
		}
		else speed_rad_2=((dif_rad_sum)/0.2050f);	
		dif_rad_sum=0;
	}
	else j++;
	
	return 0;
}

/*! @brief Returns the angle of the shaft
*/	
float tlv_angle(void){
	return angle_rad;
}

/*! @brief Returns the speed of the shaft
*/
float tlv_speed(void){
	return speed_rad_2;
}

/*! @brief Returns the direction of the shaft

	CW = -1 ; CCW = 1 ; stop = 0
*/
signed int tlv_direction(void){
	return dir;
}

/*! @brief Returns the rev_count of the shaft
*/
signed int tlv_rev_count(void){
	return rev_count;
}

/*
Source: https://www.dsprelated.com/showarticle/1052.php
Atan approximation graph: https://graphsketch.com/?eqn1_color=1&eqn1_eqn=atan(x)&eqn2_color=2&eqn2_eqn=&eqn3_color=3&eqn3_eqn=(pi%2F4%20%2B%200.273(1%20-%20mod(x)))*x&eqn4_color=4&eqn4_eqn=&eqn5_color=5&eqn5_eqn=&eqn6_color=6&eqn6_eqn=&x_min=-2.5&x_max=2.5&y_min=-1.57&y_max=1.57&x_tick=.1&y_tick=.1&x_label_freq=5&y_label_freq=5&do_grid=0&do_grid=1&bold_labeled_lines=0&bold_labeled_lines=1&line_width=4&image_w=850&image_h=525
*/
float ApproxAtan(float z)
{
	const float n1 = 0.97239411f;
	const float n2 = -0.19194795f;
	return (n1 + n2 * z * z) * z;
}


float ApproxAtan2(float y, float x)
{
	if (x != 0.0f)
	{
		if (fabsf(x) > fabsf(y))
		{
			const float z = y / x;
			if (x > 0.0)
			{
				// atan2(y,x) = atan(y/x) if x > 0
				return ApproxAtan(z);
			}
			else if (y >= 0.0)
			{
				// atan2(y,x) = atan(y/x) + PI if x < 0, y >= 0
				return ApproxAtan(z) + PI;
			}
			else
			{
				// atan2(y,x) = atan(y/x) - PI if x < 0, y < 0
				return ApproxAtan(z) - PI;
			}
		}
		else // Use property atan(y/x) = PI/2 - atan(x/y) if |y/x| > 1.
		{
			const float z = x / y;
			if (y > 0.0)
			{
				// atan2(y,x) = PI/2 - atan(x/y) if |y/x| > 1, y > 0
				return -ApproxAtan(z) + PI_2;
			}
			else
			{
				// atan2(y,x) = -PI/2 - atan(x/y) if |y/x| > 1, y < 0
				return -ApproxAtan(z) - PI_2;
			}
		}
	}
	else
	{
		if (y > 0.0f) // x = 0, y > 0
		{
			return PI_2;
		}
		else if (y < 0.0f) // x = 0, y < 0
		{
			return -PI_2;
		}
	}
	return 0.0f; // x,y = 0. Could return NaN instead.
}