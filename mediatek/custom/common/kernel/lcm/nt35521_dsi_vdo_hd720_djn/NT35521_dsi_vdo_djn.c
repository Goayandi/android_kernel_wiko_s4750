/*<------------Edited by xiaoyan.yu   ---- 20140305---------->*/
/*<IC:NT35521>*/
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
	LCM_PRINT("[DJN_LCM_NT35521-HALFRAM-DSI] %s (line:%d) :" fmt "\r\n", __func__, __LINE__, ## arg)
#else
#define LCM_DBG(fmt, arg...) do {} while (0)
#endif

#define LCM_DSI_CMD_MODE									0

#define TINNO_INT
static unsigned int lcm_esd_test = 0;      ///only for ESD test
#define TINNO_INT_ESD        1

#define OTP_ENABLE
#ifdef OTP_ENABLE
static unsigned char g_otp_enabled = 0;
#endif/*OTP_ENABLE*/
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
{0xF0, 5, {0x55,0xAA,0x52,0x08,0x00}},
{0xFF, 4, {0xAA,0x55,0xA5,0x80}},
{0x6F, 2, {0x11,0x00}},
{0xF7, 2, {0x20,0x00}},
{0x6F, 1, {0x01}},
{0xB1, 1, {0x21}},
{0xB6, 1, {0x02}},
{0xB8, 4, {0x01,0x02,0x04,0x02}},
{0xBB, 2, {0x11,0x11}},
{0xBC, 2, {0x00,0x00}},
{0xBD, 5 ,{0x01,0xA0,0x10,0x08,0x01}},
{0xC8, 1, {0x80}},
{0xF0, 5, {0x55,0xAA,0x52,0x08,0x01}},
{0xB0, 2, {0x09,0x09}},
{0xB1, 2, {0x09,0x09}},
{0xCA, 1, {0x00}},
{0xC0, 1, {0x0C}},
{0xBE, 1, {0xC2}},
{0xB3, 2, {0x35,0x35}},
{0xB4, 2, {0x25,0x25}},
{0xB9, 2, {0x34,0x34}},
{0xBA, 2, {0x24,0x24}},
{0xBC, 2, {0x90,0x00}},
{0xBD, 2, {0x94,0x00}},
{0xF0, 5, {0x55,0xAA,0x52,0x08,0x02}},
{0xEE, 1, {0x01}},
{0xB0, 16, {0x00,0x00,0x00,0x11,0x00,0x2C,0x00,0x44,0x00,0x55,0x00,0x77,0x00,0x97,0x00,0xC7}},
{0xB1, 16, {0x00,0xEE,0x01,0x2B,0x01,0x5C,0x01,0xA9,0x01,0xE8,0x01,0xE9,0x02,0x25,0x02,0x66}},
{0xB2, 16, {0x02,0x8E,0x02,0xC7,0x02,0xEA,0x03,0x1A,0x03,0x36,0x03,0x5D,0x03,0x72,0x03,0x8D}},
{0xB3, 4,  {0x03,0xB1,0x03,0xFF}},
{0xF0, 5, {0x55, 0xAA, 0x52, 0x08, 0x03}},
{0xB0, 2, {0x22, 0x00}},
{0xB1, 2, {0x22, 0x00}},
{0xB2, 5, {0x05, 0x00, 0xB0, 0x00, 0x00}},
{0xB3, 5, {0x05, 0x00, 0xB0, 0x00, 0x00}},
{0xB4, 5, {0x05, 0x00, 0xB0, 0x00, 0x00}},
{0xB5, 5, {0x05, 0x00, 0xB0, 0x00, 0x00}},
{0xBA, 5, {0x53, 0x00, 0xB0, 0x00, 0x00}},
{0xBB, 5, {0x53, 0x00, 0xB0, 0x00, 0x00}},
{0xBC, 5, {0x53, 0x00, 0xB0, 0x00, 0x00}},
{0xBD, 5, {0x53, 0x00, 0xB0, 0x00, 0x00}},
{0xC0, 4, {0x00, 0x60, 0x00, 0x00}},
{0xC1, 4, {0x00, 0x00, 0x60, 0x00}},
{0xC2, 4, {0x00, 0x00, 0x34, 0x00}},
{0xC3, 4, {0x00, 0x00, 0x34, 0x00}},
{0xC4, 1, {0x60}},
{0xC5, 1, {0xC0}},
{0xC6, 1, {0x00}},
{0xC7, 1, {0x00}},
//CMD2 page5 enable
{0xF0, 5, {0x55, 0xAA, 0x52, 0x08, 0x05}},
{0xB0, 2, {0x17, 0x06}},
{0xB1, 2, {0x17, 0x06}},
{0xB2, 2, {0x17, 0x06}},
{0xB3, 2, {0x17, 0x06}},
{0xB4, 2, {0x17, 0x06}},
{0xB5, 2, {0x17, 0x06}},
{0xB6, 2, {0x17, 0x06}},
{0xB7, 2, {0x17, 0x06}},
{0xB8, 1, {0x00}},
{0xB9, 2, {0x00, 0x03}},
{0xBA, 2, {0x00, 0x00}},
{0xBB, 2, {0x02, 0x03}},
{0xBC, 2, {0x02, 0x03}},
{0xBD, 5, {0x03, 0x03, 0x00, 0x03, 0x03}},
//0430 NVT update
{0xC0, 1, {0x0B}},
{0xC1, 1, {0x09}},
{0xC2, 1, {0xA6}},
{0xC3, 1, {0x05}}, 
{0xC4, 1, {0x00}},
{0xC5, 1, {0x02}},
{0xC6, 1, {0x22}},
{0xC7, 1, {0x03}},
{0xC8, 2, {0x07, 0x20}},
{0xC9, 2, {0x03, 0x20}},
{0xCA, 2, {0x01, 0x60}},
{0xCB, 2, {0x01, 0x60}},
{0xCC, 3, {0x00, 0x00, 0x02}},
{0xCD, 3, {0x00, 0x00, 0x02}},
{0xCE, 3, {0x00, 0x00, 0x02}},
{0xCF, 3, {0x00, 0x00, 0x02}},
{0xD1, 5, {0x00, 0x05, 0x01, 0x07, 0x10}},
{0xD2, 5, {0x10, 0x05, 0x05, 0x03, 0x10}},
{0xD3, 5, {0x20, 0x00, 0x43, 0x07, 0x10}},
{0xD4, 5, {0x30, 0x00, 0x43, 0x07, 0x10}},
{0xD0, 7, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},   
{0xD5, 11, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
{0xD6, 11, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
{0xD7, 11, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
{0xD8, 5, {0x00, 0x00, 0x00, 0x00, 0x00}},	
{0xE5, 1, {0x06}},
{0xE6, 1, {0x06}},
{0xE7, 1, {0x00}},
{0xE8, 1, {0x06}},
{0xE9, 1, {0x06}},
{0xEA, 1, {0x06}},
{0xEB, 1, {0x00}},
{0xEC, 1, {0x00}},
{0xED, 1, {0x30}},
//CMD2 page6 enable
{0xF0,5, {0x55, 0xAA, 0x52, 0x08, 0x06}},
{0xB0, 2, {0x34, 0x34}},
{0xB1, 2, {0x34, 0x34}},
{0xB2, 2, {0x2D, 0x2E}},
{0xB3, 2, {0x34, 0x34}},
{0xB4, 2, {0x29, 0x2A}},
{0xB5, 2, {0x12, 0x10}},
{0xB6, 2, {0x18, 0x16}},
{0xB7, 2, {0x00, 0x02}},
{0xB8, 2, {0x34, 0x31}},
{0xB9, 2, {0x31, 0x31}},
{0xBA, 2, {0x31, 0x31}},
{0xBB, 2, {0x31, 0x34}},
{0xBC, 2, {0x03, 0x01}}, 
{0xBD, 2, {0x17, 0x19}},
{0xBE, 2, {0x11, 0x13}},
{0xBF, 2, {0x2A, 0x29}},
{0xC0, 2, {0x34, 0x34}},
{0xC1, 2, {0x2E, 0x2D}},
{0xC2, 2, {0x34, 0x34}},
{0xC3, 2, {0x34, 0x34}},
{0xC4, 2, {0x34, 0x34}},
{0xC5, 2, {0x34, 0x34}},
{0xC6, 2, {0x2E, 0x2D}},
{0xC7, 2, {0x34, 0x34}},
{0xC8, 2, {0x29, 0x2A}},
{0xC9, 2, {0x17, 0x19}},
{0xCA, 2, {0x11, 0x13}},
{0xCB, 2, {0x03, 0x01}},
{0xCC, 2, {0x34, 0x31}},
{0xCD, 2, {0x31, 0x31}},
{0xCE, 2, {0x31, 0x31}},
{0xCF, 2, {0x31, 0x34}},
{0xD0, 2, {0x00, 0x02}},
{0xD1, 2, {0x12, 0x10}},
{0xD2, 2, {0x18, 0x16}},
{0xD3, 2, {0x2A, 0x29}},
{0xD4, 2, {0x34, 0x34}},
{0xD5, 2, {0x2D, 0x2E}},
{0xD6, 2, {0x34, 0x34}},
{0xD7, 2, {0x34, 0x34}},
{0xE6, 2, {0x31, 0x31}},
{0xD8, 5, {0x00, 0x00, 0x00, 0x00, 0x00}},
{0xD9, 5, {0x00, 0x00, 0x00, 0x00, 0x00}},
{0xE5, 2, {0x00, 0x00}},
{0xE7, 1, {0x00}},
//TE ON 	 
{0x35,1,{0x00}},
//{0x44,2,{0x00,0x40}},
{0x11,0, {}} ,
{REGFLAG_DELAY, 120, {}},
{0x29,0 ,{}},
{REGFLAG_END_OF_TABLE ,50, {}},
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
              #ifdef OTP_ENABLE
              if(g_otp_enabled && cmd == 0xBE &&  table[i].count == 1 &&  table[i].para_list[0] == 0xC2)
              {
                  LCM_DBG("Otp enabled, skip oxBE register setting"); 
                  continue;
              }
              #endif/*OTP_ENABLE*/	
			   
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
	unsigned char fResult; 
	//static int err_count = 0; 
	unsigned char buffer_1[12] ={0x00}; 
	unsigned int array[16]; 
	int i; 
		
	//--------------------------------- 
	// Set Maximum Return Size 
	//--------------------------------- 
	array[0] = 0x00083700; 
	dsi_set_cmdq(array, 1, 1); 
	
	//--------------------------------- 
	// Read [9Ch, 00h, ECC] + Error Report(4 Bytes) 
	//--------------------------------- 
	read_reg_v2(0x0A, &buffer_1, 7); 
	
	//for(i = 0; i < 7; i++) 
	//	LCM_DEBUG("line:%d,buffer_1[%d]:0x%x \n",__LINE__,i,buffer_1[i]); 
	
	if(buffer_1[0] != 0x9C) 
	{ 
		LCM_DBG("buffer_1[0]=%d buffer_1[3]=%d buffer_1[4]=%d\n",buffer_1[0],buffer_1[3],buffer_1[4]); 	
		//lcm_suspend();
		return 1; 
	} 
	else 
	{ 
		return 0; 
	} 
} 


static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
      memset(params, 0, sizeof(LCM_PARAMS));
      
      params->type	 = LCM_TYPE_DSI;
      
      params->width  = FRAME_WIDTH;
      params->height = FRAME_HEIGHT;
      
      // enable tearing-free 
      params->dbi.te_mode				  = LCM_DBI_TE_MODE_VSYNC_ONLY;//LCM_DBI_TE_MODE_DISABLED;//LCM_DBI_TE_MODE_VSYNC_ONLY; 
      params->dbi.te_edge_polarity		  = LCM_POLARITY_RISING;//LCM_POLARITY_FALLING;//LCM_POLARITY_RISING   TINNO_INT test
      
      params->dsi.mode   = SYNC_PULSE_VDO_MODE;
      
      // DSI
      /* Command mode setting */
      params->dsi.LANE_NUM				= LCM_FOUR_LANE;
      //The following defined the fomat for data coming from LCD engine.
      params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
      params->dsi.data_format.trans_seq	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
      params->dsi.data_format.padding 	= LCM_DSI_PADDING_ON_LSB;
      params->dsi.data_format.format		= LCM_DSI_FORMAT_RGB888;
      
      // Highly depends on LCD driver capability.
      //video mode timing
      
      params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
      
      /*params->dsi.vertical_sync_active				  = 4;
      params->dsi.vertical_backporch			  = 40;
      params->dsi.vertical_frontporch			  = 40;
      params->dsi.vertical_active_line				  = FRAME_HEIGHT;
      
      params->dsi.horizontal_sync_active		  = 4;
      params->dsi.horizontal_backporch				  = 82;
      params->dsi.horizontal_frontporch 			  = 82;
      params->dsi.horizontal_active_pixel		  = FRAME_WIDTH;
      
      //improve clk quality
      params->dsi.PLL_CLOCK = 240; //this value must be in MTK suggested table*/

	  
      params->dsi.word_count=720*3;
      params->dsi.vertical_sync_active				  = 4;
      params->dsi.vertical_backporch			  = 40;
      params->dsi.vertical_frontporch			  = 40;
      params->dsi.vertical_active_line				  = FRAME_HEIGHT;
	  
      params->dsi.horizontal_sync_active		  = 4;
      params->dsi.horizontal_backporch				  = 82;
      params->dsi.horizontal_frontporch 			  = 82;
      params->dsi.horizontal_active_pixel		  = FRAME_WIDTH;
	  
           // Bit rate calculation
           //1 Every lane speed
           //params->dsi.pll_div1=0; 	  // div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps	1:273Mbps
           //params->dsi.pll_div2=0; 	  // div2=0,1,2,3;div1_real=1,2,4,4   
           //params->dsi.fbk_div =0x12;	  // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	  
           
      params->dsi.PLL_CLOCK = 200;   // 265
           
           //params->dsi.ssc_range=2;
           //params->dsi.ssc_disable=0;
           
      params->dsi.noncont_clock = TRUE;  
      params->dsi.noncont_clock_period = 2;
  
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
	MDELAY(5);
	SET_RESET_PIN(0);
	MDELAY(2);
	SET_RESET_PIN(1);
	MDELAY(120);
}

static void lcm_init(void)
{
    LCM_DBG();

    lcm_reset();
	
     #ifdef OTP_ENABLE
     if(!g_otp_enabled) 
     { 
         unsigned char buffer_1[12] ={0x00}; 
         unsigned int array[10]; 
         int i; 
         
         array[0]=0x00063902;
         array[1]=0x52aa55f0;
         array[2]=0x00000108;
         dsi_set_cmdq(array, 3, 1);
         MDELAY(10);
         
         memset(buffer_1,0,sizeof(buffer_1));
         array[0] = 0x00063700;
         dsi_set_cmdq(array, 1, 1);
         read_reg_v2(0xEF, buffer_1,4);
         
         if(buffer_1[1] != 0x00)
         g_otp_enabled = 1;
         
         LCM_DBG("buffer[0]=%x buffer[1]=%x ,buffer[2]=%x buffer[3]=%x,g_otp_enabled =%d\n",buffer_1[0],buffer_1[1],buffer_1[2],buffer_1[3],g_otp_enabled);    
     }
     #endif/*OTP_ENABLE*/ 

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
       // printk("lcm abnormal, reset it!\n");  

        lcm_init();
    }	
	LCM_DBG("Otp enabled?,g_otp_enabled = %d",g_otp_enabled); 

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
	unsigned int id = 0;
	unsigned char buffer[2];
	unsigned int array[16];

	int Channel=0;
	int data[4];
	int adcVol=0;
	char  id0=0,id1=0;

	//char  id0=0,id1=0;
	int res=IMM_GetOneChannelValue(Channel,data,0);
	adcVol=data[0]*1000+data[1]*10;
	
#if 1
	   SET_RESET_PIN(1);
	   MDELAY(5);
	   SET_RESET_PIN(0);
	   MDELAY(5);
	   SET_RESET_PIN(1);
	   MDELAY(10);
	   
			
             array[0]=0x00063902;
             array[1]=0x52aa55f0;
             array[2]=0x00000108;
             dsi_set_cmdq(array, 3, 1);
             MDELAY(10);
             
             
             array[0] = 0x00083700;
             dsi_set_cmdq(array, 1, 1);
             read_reg_v2(0xc5, buffer,2);
             id0 = buffer[0];//0X55
             id1 = buffer[1];//0X21
#endif
#ifdef BUILD_LK
             printf("LINE=%d %s, res=%d,adcVol = %d ,id0 = 0x%08x id1 = 0x%08x\n", __LINE__,__func__,res,adcVol, id0,id1); 
//printf("LINE=%d %s,id0 = 0x%08x id1 = 0x%08x\n",__LINE__,__func__, id0,id1);  
#else
             printk("LINE=%d %s,res=%d,adcVol = %d,id0 = 0x%08x id1 = 0x%08x\n",__LINE__,__func__, res,adcVol, id0,id1);	
// printk("LINE=%d %s,id3 = 0x%08x id4 = 0x%08x\n",__LINE__,__func__, id3,id4);	 
#endif
            {
               #ifdef OTP_ENABLE
               array[0] = 0x00063700;
               dsi_set_cmdq(array, 1, 1);
               read_reg_v2(0xEF, buffer,4);
               
               if(buffer[1] != 0x00)
               g_otp_enabled = 1;
               
              // LCM_DBG("buffer[0]=%x buffer[1]=%x ,buffer[2]=%x buffer[3]=%x,g_otp_enabled =%d\n",buffer[0],buffer[1],buffer[2],buffer[3],g_otp_enabled);	
               #endif/*OTP_ENABLE*/	
            }
   //lcm_esd_check();
   if(adcVol >= 800 && adcVol <= 1000 )//&& id0 == 0x55 && id1 == 0x10)   //10
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
//LCM_DRIVER TCL_5025_lcm_drv = 
LCM_DRIVER nt35521_dsi_vdo_hd720_djn_lcm_drv = 
{
	.name		= "DJN_LCM_NT35521",
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
/*<------------End by xiaoyan.yu ---- 20140305---------->*/

