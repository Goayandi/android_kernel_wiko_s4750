/*<------------Edited by xiaoyan.yu   ---- 20140712---------->*/
/*<IC:HX8394D>*/
#ifdef BUILD_LK
  #include <platform/mt_typedefs.h>
  #include <platform/mt_gpio.h>
  #define LCM_PRINT printf
#else
  #include <linux/string.h>
  #include <linux/kernel.h>
  #include <mach/mt_gpio.h>
  
#include <mach/mt_pm_ldo.h>
  #define LCM_PRINT printk
#endif
#include "lcm_drv.h"

#if 1
#define LCM_DBG(fmt, arg...) \
	LCM_PRINT("[hx8394d_hd720_dsi_vdo_tianma] %s (line:%d) :" fmt "\r\n", __func__, __LINE__, ## arg)
#else
#define LCM_DBG(fmt, arg...) do {} while (0)
#endif

#define LCM_DSI_CMD_MODE									0
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)
#define FALSE (0)
#define TRUE (1)


// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)   	\
	mt_set_gpio_mode(GPIO_LCM_RST,GPIO_MODE_00);	\
	mt_set_gpio_dir(GPIO_LCM_RST,GPIO_DIR_OUT);		\
	if(v)											\
		mt_set_gpio_out(GPIO_LCM_RST,GPIO_OUT_ONE);	\
	else											\
		mt_set_gpio_out(GPIO_LCM_RST,GPIO_OUT_ZERO);

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd,buffer,buffer_size)					lcm_util.dsi_dcs_read_lcm_reg_v2(cmd,buffer,buffer_size)


 struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[128];
};

static struct LCM_setting_table lcm_initialization_setting[] = {
{0xB9, 3, {0xFF,0x83,0x94}},
{REGFLAG_DELAY, 5, {}},
{0xBA, 2, {0x33,0x83}},
{REGFLAG_DELAY, 5, {}},
{0xB1, 15, {0x6C,0x10,0x11,0x17,0x04,0x11,0xF1,
            0x80,0xE8,0x95,0x23,0x80,0xC0,0xD2,0x18}},
{REGFLAG_DELAY, 5, {}},
{0xB2, 11, {0x00,0x64,0x0E,0x0D,0x22,0x23,0x08,
            0x08,0x1C,0x4D,0x00}},
{REGFLAG_DELAY, 5, {}},
{0xB4, 12, {0x00,0xFF,0x03,0x5A,0x03,0x5A,0x03,
            0x5A,0x03,0x64,0x03,0x64}},
{REGFLAG_DELAY, 5, {}},
{0xBF, 3, {0x41,0x0E,0x01}},
{REGFLAG_DELAY, 5, {}},
{0xD3, 37, {0x00,0x0F,0x00,0x00,0x00,0x10,0x00,
            0x32,0x10,0x0D,0x00,0x00,0x32,0x10,0x00,
	    0x00,0x00,0x32,0x10,0x00,0x00,0x00,0x36,
            0x03,0x11,0x09,0x37,0x00,0x00,0x37,0x00,
	    0x00,0x00,0x00,0x0A,0x00,0x01}},
{REGFLAG_DELAY, 5, {}},
{0xD5, 44, {0x02,0x03,0x00,0x01,0x06,0x07,0x04,
            0x05,0x20,0x21,0x22,0x23,0x18,0x18,0x18,
	    0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
            0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
	    0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x24,
	    0x25,0x18,0x18,0x19,0x19}},
{REGFLAG_DELAY, 5, {}},
{0xD6, 44, {0x05,0x04,0x07,0x06,0x01,0x00,0x03,
            0x02,0x23,0x22,0x21,0x20,0x18,0x18,0x18,
	    0x18,0x18,0x18,0x58,0x58,0x18,0x18,0x18,
            0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
	    0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x25,
	    0x24,0x19,0x19,0x18,0x18}},
{REGFLAG_DELAY, 5, {}},
{0xE0, 42, {0x00,0x0D,0x15,0x38,0x3B,0x3F,0x28,
            0x49,0x06,0x0A,0x0D,0x18,0x0F,0x13,0x16,
	    0x13,0x14,0x08,0x13,0x16,0x1B,0x00,0x0D,
            0x15,0x38,0x3B,0x3F,0x28,0x49,0x06,0x0A,
	    0x0D,0x18,0x0F,0x13,0x16,0x13,0x14,0x08,
	    0x13,0x16,0x1B}},
{REGFLAG_DELAY, 5, {}},
{0xCC, 1, {0x09}},
{0xC7, 4, {0x00,0x00,0x00,0xC0}},
{0xB6, 2, {0x7A,0x7A}},
{0xC0, 2, {0x30,0x14}},
{REGFLAG_DELAY, 5, {}},
{0xBC, 1, {0x07}},
{0x11,0, {}} ,
{REGFLAG_DELAY, 200, {}},
{0x29,0 ,{}},
{REGFLAG_DELAY, 10, {}},
{0x35,1,{0x00}},
{REGFLAG_DELAY, 10, {}},
/*{0x51,1,{0xff}},
{0x53,1,{0x24}},
{0x55,1,{0x00}},*/
{REGFLAG_END_OF_TABLE ,00, {}},
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
        {0x28, 0, {}},
        {REGFLAG_DELAY, 50, {}},
        {0x10, 0, {}},
        {REGFLAG_DELAY, 120, {}},
};


static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 0, {}},
	{REGFLAG_DELAY, 120, {}},
	{0x29, 0, {}},
          {REGFLAG_DELAY, 50, {}},
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for(i = 0; i < count; i++)
    {
        unsigned cmd;
        cmd = table[i].cmd;

        switch (cmd) {
            case REGFLAG_DELAY :
                if(table[i].count <= 10)
                    MDELAY(table[i].count);
                else
                    MDELAY(table[i].count);
                break;

            case REGFLAG_END_OF_TABLE :
                break;

            default:
           {			   
               dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
           }
        }
    }
}

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------
static unsigned int lcm_esd_check(void)
{
        unsigned int ret=FALSE;
        #ifndef BUILD_LK	
        char  buffer[6];
        int   array[4];
        char esd1,esd2,esd3,esd4;
        
        
        array[0] = 0x00200500;
        dsi_set_cmdq(array, 1, 1);
        
        array[0] = 0x00033700;
        dsi_set_cmdq(array, 1, 1);
        
        read_reg_v2(0x09, buffer, 3);
        esd1=buffer[0];	
        esd2=buffer[1];
        esd3=buffer[2];
        
        if(esd1==0x80 && esd2==0x73)
        {
            ret=FALSE;
        }
        else
        {	
	LCM_DBG("ESD check esd1 = 0x%x, esd2 = 0x%x,esd3 = 0x%x", esd1, esd2, esd3);
           ret=TRUE;
        }
        #endif/*BUILD_LK*/
        return ret;
} 


static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	 memset(params, 0, sizeof(LCM_PARAMS));
	 
	 params->type	= LCM_TYPE_DSI;
	 params->width	= FRAME_WIDTH;
	 params->height = FRAME_HEIGHT;
	 params->dsi.mode						= SYNC_PULSE_VDO_MODE;	 //BURST_VDO_MODE;	// SYNC_PULSE_VDO_MODE
	 // DSI
	 /* Command mode setting */
	 params->dsi.LANE_NUM				= LCM_FOUR_LANE;//LCM_FOUR_LANE;
	 //The following defined the fomat for data coming from LCD engine.
	 params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	 params->dsi.data_format.trans_seq	 = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	 params->dsi.data_format.padding	 = LCM_DSI_PADDING_ON_LSB;
	 params->dsi.data_format.format 	 = LCM_DSI_FORMAT_RGB888;
	 
	 // Highly depends on LCD driver capability.
	 // Not support in MT6573
	 params->dsi.packet_size=256;
	 
	 // Video mode setting
	 params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage
	 params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	 params->dsi.word_count=720*3;
	 
           params->dsi.vertical_sync_active				= 2;	//2;
           params->dsi.vertical_backporch					= 40;	//14;
           params->dsi.vertical_frontporch					= 40;	//16;
           params->dsi.vertical_active_line				= FRAME_HEIGHT; 
           
           params->dsi.horizontal_sync_active	= 20;
           params->dsi.horizontal_backporch	= 80;
           params->dsi.horizontal_frontporch	= 60;
           params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
           
           params->dsi.PLL_CLOCK = 240;
           params->dsi.force_dcs_packet = 1;
}



static void lcm_reset(void)
{
    //************* Reset LCD Driver ****************// 

	
#if defined( BUILD_LK ) ||defined(BUILD_UBOOT)
			upmu_set_vio28_en(1);	
			upmu_set_rg_vio18_en(1);
#else
			if(TRUE != hwPowerOn(MT6322_POWER_LDO_VIO28, VOL_2800,"LCM")
			&& TRUE != hwPowerOn(MT6322_POWER_LDO_VIO18, VOL_1800,"LCM"))
			{
             #if defined( BUILD_LK ) ||defined(BUILD_UBOOT)
				 printf("%s, Fail to enable digital power\n");
             #else
				 printk("%s, Fail to enable digital power\n", __func__);
             #endif
			}
 #endif    
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(50);
}

static void lcm_init(void)
{
    LCM_DBG();

    lcm_reset();

    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
    //push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
   // push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}
extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);

static void lcm_suspend(void)
{

    push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
	
}

static void lcm_resume(void)
{
    push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);

    if(lcm_esd_check())
    {
         //printk("lcm abnormal, reset it!\n");  

        lcm_init();
    }	
}

static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
  //  LCM_DBG("UPDATE-UPDATE-UPDATE");
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

#if 0
	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;
	dsi_set_cmdq(&data_array, 7, 0);
#else
	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x00290508;//HW bug, so need send one HS packet
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);
#endif
//LCM_DBG("tcp_test:lcm_update\n");
}



#if !defined(BUILD_UBOOT) && !defined(BUILD_LK)

static int get_initialization_settings(unsigned char table[])
{
	memcpy(table, lcm_initialization_setting, sizeof(lcm_initialization_setting));
	return sizeof(lcm_initialization_setting);
}

static int set_initialization_settings(const unsigned char table[], const int count)
{
	if ( count > LCM_INIT_TABLE_SIZE_MAX ){
		return -EIO;
	}
	memset(lcm_initialization_setting, REGFLAG_END_OF_TABLE, sizeof(lcm_initialization_setting));
	memcpy(lcm_initialization_setting, table, sizeof(lcm_initialization_setting));
		  
	//lcm_init();

	return count;
}
static unsigned int lcm_esd_recover(void)
{
       printk("%s, LINE = %d\n", __func__,__LINE__);  
       
       lcm_init();
       lcm_resume();
       return TRUE;
}
#endif

// ---------------------------------------------------------------------------
//  Get LCM ID Information
// ---------------------------------------------------------------------------

static unsigned int lcm_compare_id()
{
	unsigned char buffer[3];
	unsigned int array[16];

	int Channel=0;
	int data[4];
	int adcVol=0;

	//char  id0=0,id1=0;
	int res=IMM_GetOneChannelValue(Channel,data,0);
	adcVol=data[0]*1000+data[1]*10;
	
#if 1
	   SET_RESET_PIN(1);
	   MDELAY(1);
	   SET_RESET_PIN(0);
	   MDELAY(50);
	   SET_RESET_PIN(1);
	   MDELAY(50);
	   			
#endif
#ifdef BUILD_LK
             printf("LINE=%d %s, res=%d,adcVol = %d\n", __LINE__,__func__,res,adcVol); 
//printf("LINE=%d %s,id0 = 0x%08x id1 = 0x%08x\n",__LINE__,__func__, id0,id1);  
#else
             printk("LINE=%d %s,res=%d,adcVol = %d\n",__LINE__,__func__, res,adcVol);	
// printk("LINE=%d %s,id3 = 0x%08x id4 = 0x%08x\n",__LINE__,__func__, id3,id4);	 
#endif
            
  // lcm_esd_check();
   if(adcVol > 300 && adcVol < 500 )//&& id0 == 0x55 && id1 == 0x10)   //10
   {
	return 1;
   }
   else
   {
	return 0;
   }
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER hx8394d_hd720_dsi_vdo_tianma_lcm_drv = 
{
	.name		= "hx8394d_hd720_dsi_vdo_tianma",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id		= lcm_compare_id,
#if (LCM_DSI_CMD_MODE)
	.update         = lcm_update,
#endif
#if !defined(BUILD_UBOOT) && !defined(BUILD_LK)
	.esd_check   = lcm_esd_check,
          .esd_recover   = lcm_esd_recover,
	.get_initialization_settings = get_initialization_settings,
	.set_initialization_settings = set_initialization_settings,
#endif
};
/*<------------End by xiaoyan.yu ---- 20140712---------->*/

