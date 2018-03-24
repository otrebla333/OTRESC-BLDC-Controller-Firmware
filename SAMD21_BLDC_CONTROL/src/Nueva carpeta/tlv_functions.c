/*
 * tlv_functions.c
 *
 * Created: 18/02/2018 20:19:58
 *  Author: Alberto
 */ 


#include "tlv_functions.h"

/*! @brief Macro to set bit value
*/
#define _BV(bit) \
(1 << (bit))

/*! @brief Macro to clear bit value
*/
#define _BC(bit) \
(0 << (bit))


#	define		PI_x2				6.28318531f
#	define		PI_1				3.14159265f
#	define		PI_2				1.57079633f

#	define		RESET_ADDRESS		0x00
#	define		TLV_ADDRESS			0x5E		//addr1
#	define		RX_SIZE				10

uint8_t rx_read[RX_SIZE];

float32_t angle_rad=0;
float32_t angle_rad_zero=0;
float32_t speed_rad_2=0;
uint8_t dir=0;
int16_t rev_count=0;



/*! @brief Initializes TLV sensor

    Sets configuration and performs a reset of the device
*/
void tlv_init(void)
{

	tlv_i2c_configure();
	
	tlv_reset();
	
	tlv_read();

	tlv_tc4_configure();
	
	tlv_tc4_configure_callbacks();
	
}

/*! @brief Resets the TLV Magnetic Sensor

    At start or after ADC hang up is sometimes convenient to reset the IC to ensure that it's working.
*/
uint8_t tlv_reset(void)
{
	//configure_tlv_i2c();
	uint8_t rx_init[RX_SIZE];
	
	
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
	for(uint16_t i=0;i<16000;i++);
	port_pin_set_output_level(TLV_POWER_PIN, true);
	for(uint16_t i=0;i<16000;i++);
	
	tlv_i2c_configure();
	
	for(uint16_t i = 0; i < 600; i++);
	
	
	//if((tlv_i2c_read(TLV_ADDRESS,RX_SIZE,rx_init))!=STATUS_OK)return 1;
	tlv_i2c_read(TLV_ADDRESS,RX_SIZE,rx_init);
	
	for(uint16_t i = 0; i < 60000; i++){
		for(uint16_t i = 0; i < 100; i++);
	}
	//
	for(uint16_t i = 0; i < 600; i++);
	
	if((tlv_write(0, 1, 1, 0, 1, rx_init))!= STATUS_OK)return 1;
	
		
	for(int i=0;i<60000;i++);
	
	//tlv_tc4_configure_callbacks();
	
	return 0;
}

/*! @brief Powers down the TLV sensor
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
uint8_t tlv_write (uint8_t intscl, uint8_t fast, uint8_t low, uint8_t temp, uint8_t lp_period,uint8_t array[])
{
	uint8_t TX[4];
	uint8_t TX2[10]={0,1,2,3,4,5,6,7,8,9};
	TX[0]=0x00;
	TX[1]=( (array[7]&0b01111000) | (intscl<<2) | (fast<<1) | low);
	TX[2]=array[8];
	TX[3]=( (array[9]&0x00011111) | (!temp<<7) | (lp_period<<6));
	
	if((tlv_i2c_write(TLV_ADDRESS, 4, TX))!= STATUS_OK)return 1;
	return 0;
}


/*! @brief Starts reading the TLV sensor data

	Starts the reading process job.
*/
void tlv_read (void)
{
	if( tlv_check_data_sanity() ) tlv_reset();
 	tlv_i2c_read(TLV_ADDRESS,RX_SIZE,rx_read);
}

/*! @brief Checks if the readout contains valid data
*/
uint8_t tlv_check_data_sanity(void)
{
	if ((rx_read[0] == 0) &&
		(rx_read[1] == 0) &&
		(rx_read[2] == 0))
		{			
			return 1;
		}
	return 0;
}


/*! @brief Calculates the angle, speed and rev_count
*/
void tlv_calculate_angle(void)
{
	#define READ_PERIOD	0.001024f	
	//unsigned int Temperature=0;
	int16_t Bx=0;
	int16_t By=0;
	int16_t Bz=0;
	uint8_t FRM=0;
	//uint8_t CH=0;
	//bool PD=0;
	

	static uint8_t FRM_old=3;
	static uint16_t i=0;
	static uint16_t j=0;


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


	static int16_t Bx_max=0;
	static int16_t Bx_min=0;
	static int16_t By_max=0;
	static int16_t By_min=0;
	static int16_t Bx_ant=0;
	static int16_t By_ant=0;
	
	//ojo, reduciendo resolucion
	Bx = Bx >> 4;
	By = By >> 4;
	
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
	
	float32_t x_rad=0;
	float32_t y_rad=0;
	//Convert X Y measurement to rad
	x_rad=((float32_t)Bx/(Bx_max-((Bx_max+Bx_min)/2)));
	if (x_rad>1.0f)x_rad=1.0f;
	else if (x_rad<(-1.0f))x_rad=(-1.0f);
	
	y_rad=((float32_t)By/(By_max-((By_max+By_min)/2)));
	if (y_rad>1.0f)y_rad=1.0f;
	else if (y_rad<(-1.0f))y_rad=(-1.0f);

	//Calculates shaft angle
	angle_rad=(ApproxAtan2(y_rad,x_rad) - angle_rad_zero);

	if (angle_rad > PI_1) angle_rad = angle_rad - PI_x2;
 	else if (angle_rad < (-PI_1)) angle_rad = angle_rad + PI_x2;

	static int16_t rev_count_ant=0;
	static int16_t rev_count_ant_rev=0;

	float32_t speed_rad_rev=0;

	static float32_t angle_rad_ant=0;
	float32_t dif_rad=0;
	static float32_t dif_rad_sum=0;
	float32_t speed_rad=0;
	
	//Calculates actual speed, and averages speed every 100 measurements.
	dif_rad=angle_rad+((rev_count-rev_count_ant)*PI_x2)-angle_rad_ant;
	dif_rad_sum=dif_rad+dif_rad_sum;
	rev_count_ant=rev_count;
	angle_rad_ant=angle_rad;
	
	if(j==99){
		j=0;
		if (((dif_rad_sum)>(-0.01f))&&((dif_rad_sum)<0.01f))
		{
			dir=0;
			speed_rad=0;
			speed_rad_2=0;
		}
		else speed_rad_2=((dif_rad_sum)/(READ_PERIOD*100));
		dif_rad_sum=0;
	}
	else j++;
	
	return 0;
}


void tlv_angle_zero(void)
{
	for (int i = 0; i<60000 ; i++)
	{
		for (int i = 0; i<20 ; i++);
	}
	angle_rad_zero = angle_rad;
}

/*! @brief Returns the angle of the shaft
*/	
float32_t tlv_angle(void){
	return angle_rad;
}


/*! @brief Returns the speed of the shaft
*/
float32_t tlv_speed(void){
	return speed_rad_2;
}


/*! @brief Returns the direction of the shaft

	CW = -1 ; CCW = 1 ; stop = 0
*/
int8_t tlv_direction(void){
	return dir;
}


/*! @brief Returns the rev_count of the shaft
*/
int16_t tlv_rev_count(void){
	return rev_count;
}


void tlv_wait_revs(int16_t rev_wait)
{
	int16_t rev_count_ant = 0;
	
	rev_count_ant = tlv_rev_count();
	
	for (int j = 0; j<60000 ; j++)
	{
		for (int j = 0; j<1000 ; j++);
		if((tlv_rev_count()-rev_count_ant) > rev_wait) break;
	}
}

/*
Source: https://www.dsprelated.com/showarticle/1052.php
Atan approximation graph: https://graphsketch.com/?eqn1_color=1&eqn1_eqn=atan(x)&eqn2_color=2&eqn2_eqn=&eqn3_color=3&eqn3_eqn=(pi%2F4%20%2B%200.273(1%20-%20mod(x)))*x&eqn4_color=4&eqn4_eqn=&eqn5_color=5&eqn5_eqn=&eqn6_color=6&eqn6_eqn=&x_min=-2.5&x_max=2.5&y_min=-1.57&y_max=1.57&x_tick=.1&y_tick=.1&x_label_freq=5&y_label_freq=5&do_grid=0&do_grid=1&bold_labeled_lines=0&bold_labeled_lines=1&line_width=4&image_w=850&image_h=525
*/
float32_t ApproxAtan(float32_t z)
{
	const float32_t n1 = 0.97239411f;
	const float32_t n2 = -0.19194795f;
	return (n1 + n2 * z * z) * z;
}


float32_t ApproxAtan2(float32_t y, float32_t x)
{
	if (x != 0.0f)
	{
		if (fabsf(x) > fabsf(y))
		{
			const float32_t z = y / x;
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
			const float32_t z = x / y;
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