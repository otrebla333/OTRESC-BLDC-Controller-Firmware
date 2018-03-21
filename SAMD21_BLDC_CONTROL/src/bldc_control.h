/*
 * bldc_control.h
 *
 * Created: 20/03/2018 12:29:46
 *  Author: Alberto
 */ 


#ifndef BLDC_CONTROL_H_
#define BLDC_CONTROL_H_

#include "tlv_i2c.h"
#include "tlv_functions.h"

#include "drv_spi.h"
#include "drv_functions.h"

#include "drv_tcc.h"

#include "tlv_tc.h"

void bldc_init(void);

void bldc_enable(void);

void bldc_disable(void);

void bldc_set_zero(void);


#endif /* BLDC_CONTROL_H_ */