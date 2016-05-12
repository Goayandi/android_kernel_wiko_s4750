/*<------------Edited by xiaoyan.yu   ---- DATE20140321---------->*/
/*<IC:TXD HX8394>*/

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#include <platform/mt_pmic.h> 
#elif defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
#include <mach/mt_pm_ldo.h>

extern void Tinno_set_HS_read();
extern void Tinno_restore_HS_read();
extern void DSI_clk_HS_mode(bool enter);
extern void DSI_clk_ULP_mode(bool enter);
#include <mach/mt_gpio.h>
#endif

#ifdef BUILD_LK
#define LCM_PRINT printf
#else
#if defined(BUILD_UBOOT)
#define LCM_PRINT printf
#else
#define LCM_PRINT printk
#endif
#endif

#define LCM_DBG(fmt, arg...) \
	LCM_PRINT ("[LCM-HX8394A-DSI-VDO] %s (line:%d) :" fmt "\r\n", __func__, __LINE__, ## arg)


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (720)
#define FRAME_HEIGHT (1280)

#define LCM_ID_HX8394 (0x94)

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

static unsigned int lcm_esd_test = FALSE;      ///only for ESD test

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

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)   

#define   LCM_DSI_CMD_MODE							0

 struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[128];
};

static struct LCM_setting_table lcm_initialization_setting[] = {
{0xB9 ,    3, {0xFF,0x83,0x94} },
{0xBA , 16, {0x13,0x82,0x00,0x16,0xC5,0x00,0x10,0xFF,0x0F,0x24,
                         0x03,0x21,0x24,0x25,0x20,0x08} },
{0xB1 , 15, {0x01,0x00,0x07,0x8A,0x01,0x11,0x11,0x35,0x3C,0x37,
                         0x37,0x47,0x12,0x01,0xE6} },
{0xB2 ,   6, {0x00,0xC8,0x08,0x04,0x00,0x22} },
{0xB4 , 22, {0x80,0x08,0x32,0x10,0x00,0x32,0x15,0x08,0x32,0x10,
             0x08,0x33,0x04,0x43,0x04,0x37,0x04,0x43,0x06,0x61,
            0x61,0x06} },
{0xB6 , 1,   {0xf9} },
{0xBF , 4,   {0x00,0x02,0x10,0x04} },
{0xC7 , 4,   {0x00,0x10,0x00,0x10} },
{0xCC , 1,   {0x09} },
{0xD5 , 54, {0x00,0x00,0x00,0x00,0x0A,0x00,0x01,0x00,0xCC,0x00,
                         0x00,0x00,0x88,0x88,0x88,0x88,0x99,0x88,0x88,0x88,
                         0xAA,0xBB,0x23,0x01,0x67,0x45,0x01,0x23,0x88,0x88,
                         0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x99,0x88,0x88,
                         0xBB,0xAA,0x54,0x76,0x10,0x32,0x32,0x10,0x88,0x88,
                         0x88,0x88,0x3C,0x01} },
{0xE0 , 34, {0x00,0x01,0x07,0x2B,0x33,0x3F,0x17,0x3A,0x0C,0x0D,
                         0x0D,0x11,0x13,0x11,0x13,0x11,0x17,0x00,0x01,0x07,
                         0x2B,0x33,0x3F,0x17,0x3A,0x0C,0x0D,0x0D,0x11,0x13,
                         0x11,0x13,0x11,0x17} },
{0x35,  1,  {0x00}},
{0x44,  2,  {0x00,0x40}},
{0x11,  0,  {}} ,
{REGFLAG_DELAY, 120, {}},
//{0x36 , 1, {0x02} },
//{REGFLAG_DELAY, 120, {}},
{0x29,0 ,{}},
{REGFLAG_DELAY, 20, {}},
{REGFLAG_END_OF_TABLE ,00, {}},
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
                dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
        }
    }
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type   = LCM_TYPE_DSI;
	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;
	params->dsi.mode					= SYNC_PULSE_VDO_MODE;	//BURST_VDO_MODE;  // SYNC_PULSE_VDO_MODE
	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM				= LCM_FOUR_LANE;//LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding 	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format		= LCM_DSI_FORMAT_RGB888;

	params->dsi.packet_size=256;
	
	// Video mode setting
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	params->dsi.word_count=720*3;

          params->dsi.vertical_sync_active				= 2;	//2;
          params->dsi.vertical_backporch					= 10;	//14;
          params->dsi.vertical_frontporch 				= 8;	//16;
          params->dsi.vertical_active_line				= FRAME_HEIGHT; 
          
          params->dsi.horizontal_sync_active				= 30;	//2;
          params->dsi.horizontal_backporch				= 150;	//60;	//42;
          params->dsi.horizontal_frontporch				= 150;	//60;	//44;
          params->dsi.horizontal_active_pixel 			= FRAME_WIDTH;
          
          
          
          params->dsi.PLL_CLOCK = 250;//240;	// 265
          
          //params->dsi.ssc_range=2;
          //params->dsi.ssc_disable=0;
          
          params->dsi.noncont_clock = TRUE;	
          params->dsi.noncont_clock_period = 2;
}

static unsigned int lcm_compare_id(void);
static void lcm_init(void)
{
	LCM_DBG();
	//lcm_compare_id();

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

	MDELAY(5);
	
	SET_RESET_PIN(1);
	MDELAY(5);
	SET_RESET_PIN(0);
	MDELAY(10);
	
	SET_RESET_PIN(1);
	MDELAY(120);	  
	
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

}




static void lcm_suspend(void)
{
	unsigned int data_array[16];

	data_array[0] = 0x00100500; // Sleep In
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(120);
	
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);    
}


static void lcm_resume(void)
{
	SET_RESET_PIN(1);
	MDELAY(5);
	SET_RESET_PIN(0);
	MDELAY(10);
	
	SET_RESET_PIN(1);
	MDELAY(120);	  

	//init_lcm_registers();  // test esd del 
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

}

         
#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
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

	/*data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(&data_array, 3, 1);
	
	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(&data_array, 3, 1);

	data_array[0]= 0x00290508; //HW bug, so need send one HS packet
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0]= 0x002c3909;
	dsi_set_cmdq(&data_array, 1, 0);
*/
}
#endif

static unsigned int lcm_esd_check(void)
{
        unsigned int ret=FALSE;
        #ifndef BUILD_LK	
        char  buffer[6];
        int   array[4];
        char esd1,esd2,esd3,esd4;
        if(lcm_esd_test)
        {
             lcm_esd_test = FALSE;
             return TRUE;
        }
        
        //Tinno_set_HS_read();
        
        array[0] = 0x00200500;
        dsi_set_cmdq(array, 1, 1);
        
        #if 1
        array[0] = 0x00033700;
        dsi_set_cmdq(array, 1, 1);
        
        read_reg_v2(0x09, buffer, 3);
        esd1=buffer[0];	
        esd2=buffer[1];
        esd3=buffer[2];
        //esd4=buffer[3];
        //LCM_DBG("esd1 = 0x%x, esd2 = 0x%x,esd3 = 0x%x", esd1, esd2, esd3);
        
        //Tinno_restore_HS_read();
        
        if(esd1==0x80 && esd2==0x73)
        {
            ret=FALSE;
        }
        else
        {	
	LCM_DBG("ESD check esd1 = 0x%x, esd2 = 0x%x,esd3 = 0x%x", esd1, esd2, esd3);
           ret=TRUE;
        }
        #endif
        
        
        #endif
        return ret;
}

extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);
static unsigned int lcm_compare_id(void)
{
	unsigned int id0,id1,id=0;
	unsigned char buffer[2];
	unsigned int array[16];  

   	 LCM_DBG();
	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(1);
	SET_RESET_PIN(1);
	MDELAY(20); 

	array[0] = 0x00043902;                          
	array[1] = 0x9483ffb9;                 
	dsi_set_cmdq(array, 2, 1);

	  
	MDELAY(20);
	memset(buffer, 0, sizeof(buffer));
	array[0] = 0x00023700;// return byte number
	dsi_set_cmdq(&array, 1, 1);
	//MDELAY(5);

	read_reg_v2(0xf4, buffer, 1);
	id=buffer[0];

	LCM_DBG("id0 = 0x%08x\n", id0);
	LCM_DBG("id1 = 0x%08x\n" , id1);
	LCM_DBG("id  = 0x%08x\n", id);
	
	#if 1
         {
                 unsigned int id = 0;
                 unsigned char buffer[2];
                 unsigned int array[16];
                 
                 int Channel=0;
                 int data[4];
                 int adcVol=0;
                 //char  id0=0,id1=0;
                 int res=IMM_GetOneChannelValue(Channel,data,0);
                 adcVol=data[0]*1000+data[1]*10;
                 
                 #ifdef BUILD_LK
                 printf("LINE=%d %s, res=%d,adcVol = %d \n", __LINE__,__func__,res,adcVol); 
                 //printf("LINE=%d %s,id0 = 0x%08x id1 = 0x%08x\n",__LINE__,__func__, id0,id1);  
                 #else
                 printk("LINE=%d %s, res=%d,adcVol = %d \n", __LINE__,__func__,res,adcVol); 
                 #endif
                 
                 if(adcVol > 200 && adcVol < 500 )
                 {
                      return 1;
                 }
                 else
                 {
                      return 0;
                 }
         }
	#endif
}

static unsigned int lcm_esd_recover(void)
{
          LCM_DBG();	
          
          DSI_clk_ULP_mode(1);
          MDELAY(10);
          DSI_clk_ULP_mode(0);
          MDELAY(10);
          DSI_clk_HS_mode(1);
          MDELAY(10);
          //* */
          SET_RESET_PIN(1);
          SET_RESET_PIN(0);
          MDELAY(10);
          SET_RESET_PIN(1);
          MDELAY(120);
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
          return TRUE;
}

LCM_DRIVER hx8394a_dsi_vdo_hd720_txd_lcm_drv = 
{
          .name	         = "hx8394_dsi_vdo_txd",
	.set_util_funcs  = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init                = lcm_init,
	.suspend         = lcm_suspend,
	.resume          = lcm_resume,
	.compare_id    = lcm_compare_id,
	.esd_check = lcm_esd_check,
	.esd_recover = lcm_esd_recover,
    #if (LCM_DSI_CMD_MODE)
          .update         = lcm_update,
    #endif
    };
/*<------------End by xiaoyan.yu ---- DATE20140321---------->*/
