#include <utils/Log.h>
#include <fcntl.h>
#include <math.h>

#include "camera_custom_nvram.h"
#include "camera_custom_sensor.h"
#include "image_sensor.h"
#include "kd_imgsensor_define.h"
#include "camera_AE_PLineTable_imx179raw.h"
#include "camera_info_imx179raw.h"
#include "camera_custom_AEPlinetable.h"
#include "camera_custom_tsf_tbl.h"
const NVRAM_CAMERA_ISP_PARAM_STRUCT CAMERA_ISP_DEFAULT_VALUE =
{{
    //Version
    Version: NVRAM_CAMERA_PARA_FILE_VERSION,
    //SensorId
    SensorId: SENSOR_ID,
    ISPComm:{
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    },
    ISPPca:{
        #include INCLUDE_FILENAME_ISP_PCA_PARAM
    },
    ISPRegs:{
        #include INCLUDE_FILENAME_ISP_REGS_PARAM
        },
    ISPMfbMixer:{{
        {//00: MFB mixer for ISO 100
            0x00000000, 0x00000000
        },
        {//01: MFB mixer for ISO 200
            0x00000000, 0x00000000
        },
        {//02: MFB mixer for ISO 400
            0x00000000, 0x00000000
        },
        {//03: MFB mixer for ISO 800
            0x00000000, 0x00000000
        },
        {//04: MFB mixer for ISO 1600
            0x00000000, 0x00000000
        },
        {//05: MFB mixer for ISO 2400
            0x00000000, 0x00000000
        },
        {//06: MFB mixer for ISO 3200
            0x00000000, 0x00000000
        }
    }},
    ISPCcmPoly22:{
        81500,    // i4R_AVG
        18775,    // i4R_STD
        116875,    // i4B_AVG
        32050,    // i4B_STD
        {  // i4P00[9]
            4995000, -2330000, -105000, -807500, 3715000, -350000, 187500, -2265000, 4637500
        },
        {  // i4P10[9]
            1690880, -1686702, -4178, -145634, 88497, 58144, -149717, 128881, 20836
        },
        {  // i4P01[9]
            1234433, -1258492, 24058, -247551, -64635, 317510, -82586, -506035, 588621
        },
        {  // i4P20[9]
            0, 0, 0, 0, 0, 0, 0, 0, 0
        },
        {  // i4P11[9]
            0, 0, 0, 0, 0, 0, 0, 0, 0
        },
        {  // i4P02[9]
            0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    }
}};

const NVRAM_CAMERA_3A_STRUCT CAMERA_3A_NVRAM_DEFAULT_VALUE =
{
    NVRAM_CAMERA_3A_FILE_VERSION, // u4Version
    SENSOR_ID, // SensorId

    // AE NVRAM
    {
        // rDevicesInfo
        {
            1160,    // u4MinGain, 1024 base = 1x
            8192,    // u4MaxGain, 16x
            50,    // u4MiniISOGain, ISOxx  
            128,    // u4GainStepUnit, 1x/8 
            14,    // u4PreExpUnit 
            30,    // u4PreMaxFrameRate
            14,    // u4VideoExpUnit  
            30,    // u4VideoMaxFrameRate 
            1024,    // u4Video2PreRatio, 1024 base = 1x 
            14,    // u4CapExpUnit 
            23,    // u4CapMaxFrameRate
            1024,    // u4Cap2PreRatio, 1024 base = 1x
            20,    // u4LensFno, Fno = 2.8
            350    // u4FocusLength_100x
        },
        // rHistConfig
        {
            2,    // u4HistHighThres
            37,    // u4HistLowThres//40
            2,    // u4MostBrightRatio
            1,    // u4MostDarkRatio
            163,    // u4CentralHighBound//160
            20,    // u4CentralLowBound
            {240, 230, 220, 210, 200},    // u4OverExpThres[AE_CCT_STRENGTH_NUM] 
            {86, 108, 128, 148, 170},    // u4HistStretchThres[AE_CCT_STRENGTH_NUM] 
            {18, 22, 26, 30, 36}    // u4BlackLightThres[AE_CCT_STRENGTH_NUM] //34
        },
        // rCCTConfig
        {
            TRUE,    // bEnableBlackLight
            TRUE,    // bEnableHistStretch
            FALSE,    // bEnableAntiOverExposure
            TRUE,    // bEnableTimeLPF
            FALSE,    // bEnableCaptureThres
            FALSE,    // bEnableVideoThres
            FALSE,    // bEnableStrobeThres
            47,    // u4AETarget//47
            47,    // u4StrobeAETarget//47
            50,    // u4InitIndex
            4,    // u4BackLightWeight
            32,    // u4HistStretchWeight
            4,    // u4AntiOverExpWeight
            3,    // u4BlackLightStrengthIndex//2
            2,    // u4HistStretchStrengthIndex
            2,    // u4AntiOverExpStrengthIndex
            2,    // u4TimeLPFStrengthIndex
            {1, 3, 5, 7, 8},    // u4LPFConvergeTable[AE_CCT_STRENGTH_NUM] 
            90,    // u4InDoorEV = 9.0, 10 base 
            -10,    // i4BVOffset delta BV = value/10 
            4,    // u4PreviewFlareOffset
            4,    // u4CaptureFlareOffset
            5,    // u4CaptureFlareThres
            4,    // u4VideoFlareOffset
            5,    // u4VideoFlareThres
            2,    // u4StrobeFlareOffset
            2,    // u4StrobeFlareThres
            8,    // u4PrvMaxFlareThres
            0,    // u4PrvMinFlareThres
            8,    // u4VideoMaxFlareThres
            0,    // u4VideoMinFlareThres
            18,    // u4FlatnessThres    // 10 base for flatness condition.
            75    // u4FlatnessStrength
        }
    },
    // AWB NVRAM
    {
        // AWB calibration data
        {
            // rUnitGain (unit gain: 1.0 = 512)
            {
                0,    // i4R
                0,    // i4G
                0    // i4B
            },
            // rGoldenGain (golden sample gain: 1.0 = 512)
            {
                0,    // i4R
                0,    // i4G
                0    // i4B
            },
            // rTuningUnitGain (Tuning sample unit gain: 1.0 = 512)
            {
                0,    // i4R
                0,    // i4G
                0    // i4B
            },
            // rD65Gain (D65 WB gain: 1.0 = 512)
            {
                1018,    // i4R
                512,    // i4G
                748    // i4B
            }
        },
        // Original XY coordinate of AWB light source
        {
           // Strobe
            {
                14,    // i4X
                -483    // i4Y
            },
            // Horizon
            {
                -510,    // i4X
                -477    // i4Y
            },
            // A
            {
                -344,    // i4X
                -467    // i4Y
            },
            // TL84
            {
                -186,    // i4X
                -454    // i4Y
            },
            // CWF
            {
                -123,    // i4X
                -535    // i4Y
            },
            // DNP
            {
                -77,    // i4X
                -446    // i4Y
            },
            // D65
            {
                114,    // i4X
                -394    // i4Y
            },
            // DF
            {
                -77,    // i4X
                -446    // i4Y
            }
        },
        // Rotated XY coordinate of AWB light source
        {
            // Strobe
            {
                -62,    // i4X
                -480    // i4Y
            },
            // Horizon
            {
                -579,    // i4X
                -392    // i4Y
            },
            // A
            {
                -413,    // i4X
                -408    // i4Y
            },
            // TL84
            {
                -255,    // i4X
                -420    // i4Y
            },
            // CWF
            {
                -205,    // i4X
                -510    // i4Y
            },
            // DNP
            {
                -146,    // i4X
                -429    // i4Y
            },
            // D65
            {
                51,    // i4X
                -407    // i4Y
            },
            // DF
            {
                -146,    // i4X
                -429    // i4Y
            }
        },
        // AWB gain of AWB light source
        {
            // Strobe 
            {
                1004,    // i4R
                512,    // i4G
                966    // i4B
            },
            // Horizon 
            {
                512,    // i4R
                535,    // i4G
                2037    // i4B
            },
            // A 
            {
                605,    // i4R
                512,    // i4G
                1536    // i4B
            },
            // TL84 
            {
                736,    // i4R
                512,    // i4G
                1217    // i4B
            },
            // CWF 
            {
                894,    // i4R
                512,    // i4G
                1248    // i4B
            },
            // DNP 
            {
                844,    // i4R
                512,    // i4G
                1039    // i4B
            },
            // D65 
            {
                1018,    // i4R
                512,    // i4G
                748    // i4B
            },
            // DF 
            {
                844,    // i4R
                512,    // i4G
                1039    // i4B
            }
        },
        // Rotation matrix parameter
        {
            9,    // i4RotationAngle
            253,    // i4Cos
            40    // i4Sin
        },
        // Daylight locus parameter
        {
            -173,    // i4SlopeNumerator
            128    // i4SlopeDenominator
        },
        // AWB light area
        {
            // Strobe:FIXME
            {
            0,    // i4RightBound
            0,    // i4LeftBound
            0,    // i4UpperBound
            0    // i4LowerBound
            },
            // Tungsten
            {
            -305,    // i4RightBound
            -955,    // i4LeftBound
            -350,    // i4UpperBound
            -450    // i4LowerBound
            },
            // Warm fluorescent
            {
            -305,    // i4RightBound
            -955,    // i4LeftBound
            -450,    // i4UpperBound
            -570    // i4LowerBound
            },
            // Fluorescent
            {
            -196,    // i4RightBound
            -305,    // i4LeftBound
            -338,    // i4UpperBound
            -465    // i4LowerBound
            },
            // CWF
            {
            -196,    // i4RightBound
            -305,    // i4LeftBound
            -465,    // i4UpperBound
            -560    // i4LowerBound
            },
            // Daylight
            {
            76,    // i4RightBound
            -196,    // i4LeftBound
            -327,    // i4UpperBound
            -487    // i4LowerBound
            },
            // Shade
            {
            436,    // i4RightBound
            76,    // i4LeftBound
            -327,    // i4UpperBound
            -487    // i4LowerBound
            },
            // Daylight Fluorescent
            {
            76,    // i4RightBound
            -196,    // i4LeftBound
            -487,    // i4UpperBound
            -580    // i4LowerBound
            }
        },
        // PWB light area
        {
            // Reference area
            {
            436,    // i4RightBound
            -955,    // i4LeftBound
            0,    // i4UpperBound
            -580    // i4LowerBound
            },
            // Daylight
            {
            101,    // i4RightBound
            -196,    // i4LeftBound
            -327,    // i4UpperBound
            -487    // i4LowerBound
            },
            // Cloudy daylight
            {
            281,    // i4RightBound
            76,    // i4LeftBound
            -327,    // i4UpperBound
            -487    // i4LowerBound
            },
            // Shade
            {
            381,    // i4RightBound
            76,    // i4LeftBound
            -327,    // i4UpperBound
            -487    // i4LowerBound
            },
            // Twilight
            {
            -196,    // i4RightBound
            -356,    // i4LeftBound
            -327,    // i4UpperBound
            -487    // i4LowerBound
            },
            // Fluorescent
            {
            101,    // i4RightBound
            -355,    // i4LeftBound
            -357,    // i4UpperBound
            -560    // i4LowerBound
            },
            // Warm fluorescent
            {
            -313,    // i4RightBound
            -513,    // i4LeftBound
            -357,    // i4UpperBound
            -560    // i4LowerBound
            },
            // Incandescent
            {
            -313,    // i4RightBound
            -513,    // i4LeftBound
            -327,    // i4UpperBound
            -487    // i4LowerBound
            },
            // Gray World
            {
            5000,    // i4RightBound
            -5000,    // i4LeftBound
            5000,    // i4UpperBound
            -5000    // i4LowerBound
            }
        },
        // PWB default gain	
        {
            // Daylight
            {
            911,    // i4R
            512,    // i4G
            871    // i4B
            },
            // Cloudy daylight
            {
            1175,    // i4R
            512,    // i4G
            614    // i4B
            },
            // Shade
            {
            1243,    // i4R
            512,    // i4G
            568    // i4B
            },
            // Twilight
            {
            705,    // i4R
            512,    // i4G
            1241    // i4B
            },
            // Fluorescent
            {
            902,    // i4R
            512,    // i4G
            1044    // i4B
            },
            // Warm fluorescent
            {
            654,    // i4R
            512,    // i4G
            1625    // i4B
            },
            // Incandescent
            {
            604,    // i4R
            512,    // i4G
            1534    // i4B
            },
            // Gray World
            {
            512,    // i4R
            512,    // i4G
            512    // i4B
            }
        },
        // AWB preference color	
        {
            // Tungsten
            {
            3,    // i4SliderValue
            7235    // i4OffsetThr
            },
            // Warm fluorescent	
            {
            2,    // i4SliderValue
            5864    // i4OffsetThr
            },
            // Shade
            {
            0,    // i4SliderValue//50
            344    // i4OffsetThr
            },
            // Daylight WB gain
            {
            816,    // i4R
            512,    // i4G
            1015    // i4B
            },
            // Preference gain: strobe
            {
            512,    // i4R
            512,    // i4G
            512    // i4B
            },
            // Preference gain: tungsten
            {
            512,    // i4R
            512,    // i4G
            512    // i4B
            },
            // Preference gain: warm fluorescent
            {
            493,    // i4R
            512,    // i4G
            512    // i4B
            },
            // Preference gain: fluorescent
            {
            512,    // i4R
            512,    // i4G
            512    // i4B
            },
            // Preference gain: CWF
            {
            512,    // i4R
            512,    // i4G
            512    // i4B
            },
            // Preference gain: daylight
            {
            512,    // i4R
            512,    // i4G
            512    // i4B
            },
            // Preference gain: shade
            {
            512,    // i4R
            512,    // i4G
            512    // i4B
            },
            // Preference gain: daylight fluorescent
            {
            512,    // i4R
            512,    // i4G
            512    // i4B
            }
        },
        {// CCT estimation
            {// CCT
                2300,    // i4CCT[0]
                2850,    // i4CCT[1]
                4100,    // i4CCT[2]
                5100,    // i4CCT[3]
                6500    // i4CCT[4]
            },
            {// Rotated X coordinate
                -630,    // i4RotatedXCoordinate[0]
                -464,    // i4RotatedXCoordinate[1]
                -306,    // i4RotatedXCoordinate[2]
                -197,    // i4RotatedXCoordinate[3]
                0    // i4RotatedXCoordinate[4]
            }
        }
    },
    {0}
};

#include INCLUDE_FILENAME_ISP_LSC_PARAM
//};  //  namespace

const CAMERA_TSF_TBL_STRUCT CAMERA_TSF_DEFAULT_VALUE =
{
    #include INCLUDE_FILENAME_TSF_PARA
    #include INCLUDE_FILENAME_TSF_DATA
};


typedef NSFeature::RAWSensorInfo<SENSOR_ID> SensorInfoSingleton_T;


namespace NSFeature {
template <>
UINT32
SensorInfoSingleton_T::
impGetDefaultData(CAMERA_DATA_TYPE_ENUM const CameraDataType, VOID*const pDataBuf, UINT32 const size) const
{
    UINT32 dataSize[CAMERA_DATA_TYPE_NUM] = {sizeof(NVRAM_CAMERA_ISP_PARAM_STRUCT),
                                             sizeof(NVRAM_CAMERA_3A_STRUCT),
                                             sizeof(NVRAM_CAMERA_SHADING_STRUCT),
                                             sizeof(NVRAM_LENS_PARA_STRUCT),
                                             sizeof(AE_PLINETABLE_T),
                                             0,
                                             sizeof(CAMERA_TSF_TBL_STRUCT)};

    if (CameraDataType > CAMERA_DATA_TSF_TABLE || NULL == pDataBuf || (size < dataSize[CameraDataType]))
    {
        return 1;
    }

    switch(CameraDataType)
    {
        case CAMERA_NVRAM_DATA_ISP:
            memcpy(pDataBuf,&CAMERA_ISP_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_ISP_PARAM_STRUCT));
            break;
        case CAMERA_NVRAM_DATA_3A:
            memcpy(pDataBuf,&CAMERA_3A_NVRAM_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_3A_STRUCT));
            break;
        case CAMERA_NVRAM_DATA_SHADING:
            memcpy(pDataBuf,&CAMERA_SHADING_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_SHADING_STRUCT));
            break;
        case CAMERA_DATA_AE_PLINETABLE:
            memcpy(pDataBuf,&g_PlineTableMapping,sizeof(AE_PLINETABLE_T));
            break;
        case CAMERA_DATA_TSF_TABLE:
            memcpy(pDataBuf,&CAMERA_TSF_DEFAULT_VALUE,sizeof(CAMERA_TSF_TBL_STRUCT));
            break;
        default:
            break;
    }
    return 0;
}}; // NSFeature


