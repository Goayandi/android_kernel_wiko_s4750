/*****************************************************************************
 *
 * Filename:
 * ---------
 *    battery_meter_bq27541.c
 *
 * Project:
 * --------
 *   ALPS_Software
 *
 * Description:
 * ------------
 *   This file implements the interface between BMT and ADC scheduler.
 *
 * Author:
 * -------
 *  Jake Liang
 *
 ****************************************************************************/
#include <mach/battery_meter.h>
#include "bq27541.h"
#include <mach/upmu_common.h>
#include <mach/mt_gpio.h>
#include <cust_gpio_usage.h>
#include <mach/upmu_hw.h>
#include <linux/xlog.h>
#include <linux/delay.h>
#include <mach/mt_sleep.h>
#include <mach/mt_boot.h>
#include <mach/system.h>
#include <cust_charging.h>

 // ============================================================ //
 //define
 // ============================================================ //
#define STATUS_OK	0
#define STATUS_UNSUPPORTED	-1
#define GETARRAYNUM(array) (sizeof(array)/sizeof(array[0]))


 // ============================================================ //
 //global variable
 // ============================================================ //


 // ============================================================ //
 // function prototype
 // ============================================================ //
 
 
 // ============================================================ //
 //extern variable
 // ============================================================ //
 
 // ============================================================ //
 //extern function
 // ============================================================ //
 extern void mt_power_off(void);
 

 // ============================================================ //

 static kal_int32 bat_meter_hw_init(void *data)
 {
 	kal_int32 status = STATUS_OK;

 	return status;
 }


 static kal_int32 bat_meter_dump_register(void *data)
 {
 	kal_int32 status = STATUS_OK;

	bq27541_parameter_dump();
   	
	return status;
 }	


 static kal_int32 bat_meter_get_status(void *data)
 {	
	return (kal_int32)bq27541_check_state();
 }


 static kal_int32 bat_meter_get_soc(void *data)
 {	
	return (kal_int32)bq27541_read_soc();
 }

 static kal_int32 bat_meter_get_temp(void *data)
 {	
	return (kal_int32)bq27541_get_temp();
 }

 static kal_int32 bat_meter_get_batvol(void *data)
 {	
	return (kal_int32)bq27541_get_batvol();
 }
 
 static kal_int32 bat_meter_get_current(void *data)
 {	
	return (kal_int32)bq27541_get_current();
 }

 static kal_int32 (* const battery_meter_ext_func[BATTERY_METER_EXT_CMD_NUMBER])(void *data)=
 {
    bat_meter_hw_init
    ,bat_meter_dump_register  	
    ,bat_meter_get_status 
    ,bat_meter_get_temp
    ,bat_meter_get_batvol
    ,bat_meter_get_current
    ,bat_meter_get_soc
 };

 
 /*
 * FUNCTION
 *		bat_meter_interface
 *
 * DESCRIPTION															 
 *		 This function is called to set the battery meter hw
 *
 * CALLS  
 *
 * PARAMETERS
 *		None
 *	 
 * RETURNS
 *		
 *
 * GLOBALS AFFECTED
 *	   None
 */
 kal_int32 bat_meter_ext_interface(BATTERY_METER_EXT_CMD cmd, void *data)
 {
	 kal_int32 status;
	 if(cmd < BATTERY_METER_EXT_CMD_NUMBER)
		 status = battery_meter_ext_func[cmd](data);
	 else
		 return STATUS_UNSUPPORTED;
 
	 return status;
 }


