/*
 * tlv_tc.h
 *
 * Created: 07/03/2018 14:25:32
 *  Author: Alberto
 */ 


#ifndef TLV_TC_H_
#define TLV_TC_H_

#include <asf.h>
// From module: TC - Timer Counter (Callback APIs)
#include <tc.h>
#include <tc_interrupt.h>

#include <config_tc.h>

#include <tlv_functions.h>

void tlv_tc4_configure(void);
void tlv_tc4_configure_callbacks(void);
void tlv_tc_callback_to_change_duty_cycle(
	struct tc_module *const module_inst);
void tlv_tc4_disable_callbacks(void);


#endif /* TLV_TC_H_ */