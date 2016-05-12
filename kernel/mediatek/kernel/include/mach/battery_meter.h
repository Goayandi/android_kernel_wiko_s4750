#ifndef _BATTERY_METER_H
#define _BATTERY_METER_H

#include <mach/mt_typedefs.h>
#include "cust_battery_meter.h"
// ============================================================
// define
// ============================================================

// ============================================================
// ENUM
// ============================================================

// ============================================================
// structure
// ============================================================

// ============================================================
// typedef
// ============================================================
typedef struct{
    INT32 BatteryTemp;
    INT32 TemperatureR;
}BATT_TEMPERATURE;

// ============================================================
// External Variables
// ============================================================

// ============================================================
// External function
// ============================================================
extern kal_int32 battery_meter_get_battery_voltage(void);
extern kal_int32 battery_meter_get_charging_current(void);
extern kal_int32 battery_meter_get_battery_current(void);
extern kal_bool  battery_meter_get_battery_current_sign(void);
extern kal_int32 battery_meter_get_car(void);
extern kal_int32 battery_meter_get_battery_temperature(void);
extern kal_int32 battery_meter_get_charger_voltage(void);
extern kal_int32 battery_meter_get_battery_percentage(void);
extern kal_int32 battery_meter_initial(void);
extern kal_int32 battery_meter_reset(void);
extern kal_int32 battery_meter_sync(kal_int32 bat_i_sense_offset);                      

extern kal_int32 battery_meter_get_battery_zcv(void);
extern kal_int32 battery_meter_get_battery_nPercent_zcv(void);    // 15% zcv,  15% can be customized
extern kal_int32 battery_meter_get_battery_nPercent_UI_SOC(void); // tracking point

extern kal_int32 battery_meter_get_tempR(kal_int32 dwVolt);
extern kal_int32 battery_meter_get_tempV(void);
extern kal_int32 battery_meter_get_VSense(void);    //isense voltage

#if defined (TINNO_BATTERY_EXT_METER_SUPPORT)
// ============================================================
// ENUM
// ============================================================
typedef enum
{
	BATTERY_METER_EXT_CMD_INIT,
	BATTERY_METER_EXT_CMD_DUMP_REGISTER,	
	BATTERY_METER_EXT_CMD_GET_STATUS,	
	BATTERY_METER_EXT_CMD_GET_TEMPERATURE,
	BATTERY_METER_EXT_CMD_GET_VOLTAGE,
	BATTERY_METER_EXT_CMD_GET_CURRENT,
	BATTERY_METER_EXT_CMD_GET_SOC,
	BATTERY_METER_EXT_CMD_NUMBER,
} BATTERY_METER_EXT_CMD;

// ============================================================
// typedef
// ============================================================
typedef kal_int32 (*BATTERY_METER_EXT_CONTROL)(BATTERY_METER_EXT_CMD cmd, void *data);

// ============================================================
// External function
// ============================================================
extern kal_int32 g_terminal_current_base;
extern kal_int32 bat_meter_ext_interface(BATTERY_METER_EXT_CMD cmd, void *data);

extern BATTERY_METER_EXT_CONTROL battery_meter_ext_control;
#define IS_BM_EXT    battery_meter_ext_control(BATTERY_METER_EXT_CMD_GET_STATUS, NULL)
extern kal_int32 battery_meter_get_battery_voltage_by_adc(void);
extern kal_int32 battery_meter_get_battery_vc(void);
extern kal_int32 battery_meter_get_0percent_voltage(void);
#endif  /* TINNO_BATTERY_EXT_METER_SUPPORT */

#endif    //#ifndef _BATTERY_METER_H

