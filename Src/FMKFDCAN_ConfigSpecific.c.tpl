/*********************************************************************
 * @file        FMKFDCAN_ConfigSpecific.c
 * @brief       FDCAN ECU-specific configuration services.
 * @note        Keeps FMK_FDCAN independent from STM32 family preconditions.
 *
 * @author      xxxxxx
 * @date        jj/mm/yyyy
 * @version     1.0
 */

// ********************************************************************
// *                      Includes
// ********************************************************************
#include "./FMKFDCAN_ConfigSpecific.h"
#include "FMK_HAL/FMK_CPU/Src/FMK_CPU.h"

#ifdef APPSYS_MODULE_FMKCAN_ENABLE

// ********************************************************************
// *                      Variables
// ********************************************************************
#if defined(FMKCPU_STM32_ECU_FAMILY_H7)
static t_bool g_FmkFdcan_H7ClockCfgDone_b = (t_bool)False;
static t_eFMKFDCAN_ClockKernelDiv g_FmkFdcan_H7ClockDivider_e = FMKFDCAN_CLOCK_KERNEL_DIV_NB;
#endif

// ********************************************************************
// *                      Local Defines
// ********************************************************************
#define FMKFDCAN_TIMING_SYNC_SEG                 ((t_uint8)1U)
#define FMKFDCAN_TIMING_SAMPLE_POINT_LOW_PERMIL  ((t_uint16)800U)
#define FMKFDCAN_TIMING_SAMPLE_POINT_HIGH_PERMIL ((t_uint16)750U)
#define FMKFDCAN_TIMING_MAX_PRESCALER            ((t_uint16)512U)
#define FMKFDCAN_TIMING_MIN_TOTAL_TQ             ((t_uint16)4U)
#define FMKFDCAN_TIMING_MAX_TOTAL_TQ             ((t_uint16)385U)
#define FMKFDCAN_TIMING_MAX_TIME_SEG1            ((t_uint16)255U)
#define FMKFDCAN_TIMING_MAX_TIME_SEG2            ((t_uint16)127U)

//********************************************************************************
//                      Local functions - Prototypes
//********************************************************************************
static t_eReturnCode s_FMKFDCAN_GetTargetBaudrateHz(t_eFMKFDCAN_Baudrate f_baudrate_e,
                                                    t_uint32 *f_targetBaudrateHz_pu32);

static t_uint16 s_FMKFDCAN_GetTargetSamplePointPermil(t_eFMKFDCAN_Baudrate f_baudrate_e);

static t_uint32 s_FMKFDCAN_GetAbsDiffU32(t_uint32 f_value1_u32, t_uint32 f_value2_u32);

static t_uint16 s_FMKFDCAN_RoundDivU32(t_uint32 f_numerator_u32, t_uint32 f_denominator_u32);

//********************************************************************************
//                      Public functions - Implementation
//********************************************************************************
/*********************************
 * FMKFDCAN_Set_BspNodeSpecificInit
 *********************************/
t_eReturnCode FMKFDCAN_Set_BspNodeSpecificInit(FDCAN_HandleTypeDef *f_bspHandle_ps,
                                               t_uint32 f_bspClkDivider_u32)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_bspHandle_ps == (FDCAN_HandleTypeDef *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    else
    {
#if defined(FMKCPU_STM32_ECU_FAMILY_G4)
        f_bspHandle_ps->Init.ClockDivider = f_bspClkDivider_u32;
#elif defined(FMKCPU_STM32_ECU_FAMILY_H7)
        t_uint32 messageRamOffset_u32 = 0U;
        t_uint32 nodeRamSize_u32 = 0U;

        f_bspHandle_ps->Init.RxFifo0ElmtsNbr = (t_uint32)64U;
        f_bspHandle_ps->Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
        f_bspHandle_ps->Init.RxFifo1ElmtsNbr = (t_uint32)0U;
        f_bspHandle_ps->Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
        f_bspHandle_ps->Init.RxBuffersNbr = (t_uint32)0U;
        f_bspHandle_ps->Init.RxBufferSize = FDCAN_DATA_BYTES_8;
        f_bspHandle_ps->Init.TxEventsNbr = (t_uint32)32U;
        f_bspHandle_ps->Init.TxBuffersNbr = (t_uint32)0U;
        f_bspHandle_ps->Init.TxFifoQueueElmtsNbr = (t_uint32)32U;
        f_bspHandle_ps->Init.TxElmtSize = FDCAN_DATA_BYTES_8;

        nodeRamSize_u32 = f_bspHandle_ps->Init.StdFiltersNbr
                        + (f_bspHandle_ps->Init.ExtFiltersNbr * 2U)
                        + (f_bspHandle_ps->Init.RxFifo0ElmtsNbr * f_bspHandle_ps->Init.RxFifo0ElmtSize)
                        + (f_bspHandle_ps->Init.RxFifo1ElmtsNbr * f_bspHandle_ps->Init.RxFifo1ElmtSize)
                        + (f_bspHandle_ps->Init.RxBuffersNbr * f_bspHandle_ps->Init.RxBufferSize)
                        + (f_bspHandle_ps->Init.TxEventsNbr * 2U)
                        + (f_bspHandle_ps->Init.TxBuffersNbr * f_bspHandle_ps->Init.TxElmtSize)
                        + (f_bspHandle_ps->Init.TxFifoQueueElmtsNbr * f_bspHandle_ps->Init.TxElmtSize);

        if(f_bspHandle_ps->Instance == FDCAN2)
        {
            messageRamOffset_u32 = nodeRamSize_u32;
        }
        f_bspHandle_ps->Init.MessageRAMOffset = messageRamOffset_u32;
#else
        (void)f_bspClkDivider_u32;
#endif
    }

    return Ret_e;
}

/*********************************
 * FMKFDCAN_Set_KernelClockCfg
 *********************************/
t_eReturnCode FMKFDCAN_Set_KernelClockCfg(t_bool f_isAfterBspInit_b,
                                          FDCAN_HandleTypeDef *f_bspHandle_ps,
                                          t_eFMKFDCAN_ClockKernelDiv f_clockDivider_e,
                                          t_uint32 f_bspClkDivider_u32,
                                          t_bool f_isNode1Configured_b,
                                          FDCAN_HandleTypeDef *f_node1BspHandle_ps)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_bspHandle_ps == (FDCAN_HandleTypeDef *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    else
    {
#if defined(FMKCPU_STM32_ECU_FAMILY_G4)
        if(f_isAfterBspInit_b == (t_bool)False)
        {
            f_bspHandle_ps->Init.ClockDivider = f_bspClkDivider_u32;
        }
        (void)f_clockDivider_e;
        (void)f_isNode1Configured_b;
        (void)f_node1BspHandle_ps;
#elif defined(FMKCPU_STM32_ECU_FAMILY_H7)
        if(f_isAfterBspInit_b == (t_bool)True)
        {
            FDCAN_HandleTypeDef *ccuHandle_ps = f_bspHandle_ps;
            FDCAN_ClkCalUnitTypeDef clkCalCfg_s = {0};
            HAL_StatusTypeDef bspRet_e = HAL_OK;

            if((g_FmkFdcan_H7ClockCfgDone_b == (t_bool)True)
            && (g_FmkFdcan_H7ClockDivider_e != f_clockDivider_e))
            {
                Ret_e = RC_ERROR_NOT_SUPPORTED;
            }
            if((Ret_e == RC_OK)
            && (f_bspHandle_ps->Instance != FDCAN1)
            && (f_isNode1Configured_b == (t_bool)True)
            && (f_node1BspHandle_ps != (FDCAN_HandleTypeDef *)NULL))
            {
                ccuHandle_ps = f_node1BspHandle_ps;
            }
            if((Ret_e == RC_OK)
            && (f_bspHandle_ps->Instance != FDCAN1)
            && (f_isNode1Configured_b == (t_bool)False))
            {
                if(f_clockDivider_e != FMKFDCAN_CLOCK_KERNEL_DIV1)
                {
                    Ret_e = RC_ERROR_NOT_SUPPORTED;
                }
                else
                {
                    g_FmkFdcan_H7ClockCfgDone_b = (t_bool)True;
                    g_FmkFdcan_H7ClockDivider_e = f_clockDivider_e;
                }
            }
            if((Ret_e == RC_OK) && (g_FmkFdcan_H7ClockCfgDone_b == (t_bool)False))
            {
                clkCalCfg_s.ClockCalibration = FDCAN_CLOCK_CALIBRATION_DISABLE;
                clkCalCfg_s.ClockDivider = f_bspClkDivider_u32;
                bspRet_e = HAL_FDCAN_ConfigClockCalibration(ccuHandle_ps, &clkCalCfg_s);
                if(bspRet_e != HAL_OK)
                {
                    Ret_e = RC_ERROR_WRONG_RESULT;
                }
                else
                {
                    g_FmkFdcan_H7ClockCfgDone_b = (t_bool)True;
                    g_FmkFdcan_H7ClockDivider_e = f_clockDivider_e;
                }
            }
        }
#else
        (void)f_isAfterBspInit_b;
        (void)f_clockDivider_e;
        (void)f_bspClkDivider_u32;
        (void)f_isNode1Configured_b;
        (void)f_node1BspHandle_ps;
#endif
    }

    return Ret_e;
}

/*********************************
 * FMKFDCAN_GetKernelClockHz
 *********************************/
t_eReturnCode FMKFDCAN_GetKernelClockHz(t_eFMKFDCAN_ClockKernelDiv f_clockDivider_e,
                                        t_uint32 *f_kernelClockHz_pu32)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint16 fdcanClockMHz_u16 = 0U;

    if(f_clockDivider_e >= FMKFDCAN_CLOCK_KERNEL_DIV_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(f_kernelClockHz_pu32 == (t_uint32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        Ret_e = FMKCPU_GetRccClockValue(FMKCPU_RCC_CLK_FDCAN, &fdcanClockMHz_u16);
    }
    if(Ret_e == RC_OK)
    {
        *f_kernelClockHz_pu32 = (((t_uint32)fdcanClockMHz_u16) * 1000000UL)
                              / ((t_uint32)f_clockDivider_e);
    }

    return Ret_e;
}

/*********************************
 * FMKFDCAN_ComputeBitTiming
 *********************************/
t_eReturnCode FMKFDCAN_ComputeBitTiming(t_uint32 f_kernelClockHz_u32,
                                        t_eFMKFDCAN_Baudrate f_baudrate_e,
                                        t_sFMKFDCAN_BaudrateCfg *f_baudrateCfg_ps)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint32 targetBaudrateHz_u32 = 0U;
    t_uint16 targetSamplePointPermil_u16 = 0U;
    t_uint32 bestBaudrateErr_u32 = 0xFFFFFFFFUL;
    t_uint32 bestSamplePointErr_u32 = 0xFFFFFFFFUL;
    t_uint16 totalTq_u16 = 0U;
    t_bool isCfgFound_b = (t_bool)False;

    if(f_kernelClockHz_u32 == 0U)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(f_baudrateCfg_ps == (t_sFMKFDCAN_BaudrateCfg *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        Ret_e = s_FMKFDCAN_GetTargetBaudrateHz(f_baudrate_e, &targetBaudrateHz_u32);
    }
    if(Ret_e == RC_OK)
    {
        targetSamplePointPermil_u16 = s_FMKFDCAN_GetTargetSamplePointPermil(f_baudrate_e);
    }
    if(Ret_e == RC_OK)
    {
        for(totalTq_u16 = FMKFDCAN_TIMING_MIN_TOTAL_TQ;
            totalTq_u16 <= FMKFDCAN_TIMING_MAX_TOTAL_TQ;
            totalTq_u16++)
        {
            t_uint32 denominator_u32 = targetBaudrateHz_u32 * (t_uint32)totalTq_u16;
            t_uint16 prescaler_u16 = 0U;
            t_uint16 timeSeg1_u16 = 0U;
            t_uint16 timeSeg2_u16 = 0U;
            t_uint32 actualBaudrateHz_u32 = 0U;
            t_uint32 samplePointPermil_u32 = 0U;
            t_uint32 baudrateErr_u32 = 0U;
            t_uint32 samplePointErr_u32 = 0U;

            if(denominator_u32 == 0U)
            {
                continue;
            }

            prescaler_u16 = s_FMKFDCAN_RoundDivU32(f_kernelClockHz_u32, denominator_u32);
            if((prescaler_u16 == 0U) || (prescaler_u16 > FMKFDCAN_TIMING_MAX_PRESCALER))
            {
                continue;
            }

            actualBaudrateHz_u32 = f_kernelClockHz_u32
                                 / ((t_uint32)prescaler_u16 * (t_uint32)totalTq_u16);
            if(actualBaudrateHz_u32 == 0U)
            {
                continue;
            }

            timeSeg1_u16 = (t_uint16)(((t_uint32)targetSamplePointPermil_u16
                                     * (t_uint32)totalTq_u16
                                     + 500U) / 1000U);
            if(timeSeg1_u16 == 0U)
            {
                continue;
            }
            timeSeg1_u16--;

            if((timeSeg1_u16 < 1U)
            || (timeSeg1_u16 > FMKFDCAN_TIMING_MAX_TIME_SEG1)
            || (timeSeg1_u16 >= totalTq_u16))
            {
                continue;
            }

            timeSeg2_u16 = (t_uint16)((t_uint32)totalTq_u16
                                    - 1UL
                                    - (t_uint32)timeSeg1_u16);

            if((timeSeg2_u16 < 1U) || (timeSeg2_u16 > FMKFDCAN_TIMING_MAX_TIME_SEG2))
            {
                continue;
            }

            samplePointPermil_u32 = (((t_uint32)FMKFDCAN_TIMING_SYNC_SEG + (t_uint32)timeSeg1_u16)
                                    * 1000UL) / (t_uint32)totalTq_u16;
            baudrateErr_u32 = s_FMKFDCAN_GetAbsDiffU32(actualBaudrateHz_u32, targetBaudrateHz_u32);
            samplePointErr_u32 = s_FMKFDCAN_GetAbsDiffU32(samplePointPermil_u32,
                                                          (t_uint32)targetSamplePointPermil_u16);

            if((isCfgFound_b == (t_bool)False)
            || (baudrateErr_u32 < bestBaudrateErr_u32)
            || ((baudrateErr_u32 == bestBaudrateErr_u32)
             && (samplePointErr_u32 < bestSamplePointErr_u32)))
            {
                isCfgFound_b = (t_bool)True;
                bestBaudrateErr_u32 = baudrateErr_u32;
                bestSamplePointErr_u32 = samplePointErr_u32;
                f_baudrateCfg_ps->prescaler_u16 = prescaler_u16;
                f_baudrateCfg_ps->syncSeg_u8 = FMKFDCAN_TIMING_SYNC_SEG;
                f_baudrateCfg_ps->timeSeg1_u8 = (t_uint8)timeSeg1_u16;
                f_baudrateCfg_ps->timeSeg2_u8 = (t_uint8)timeSeg2_u16;
            }
        }
    }
    if((Ret_e == RC_OK) && (isCfgFound_b == (t_bool)False))
    {
        Ret_e = RC_ERROR_NOT_SUPPORTED;
    }

    return Ret_e;
}

//********************************************************************************
//                      Local functions - Implementation
//********************************************************************************
/*********************************
 * s_FMKFDCAN_GetTargetBaudrateHz
 *********************************/
static t_eReturnCode s_FMKFDCAN_GetTargetBaudrateHz(t_eFMKFDCAN_Baudrate f_baudrate_e,
                                                    t_uint32 *f_targetBaudrateHz_pu32)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_targetBaudrateHz_pu32 == (t_uint32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    else
    {
        switch(f_baudrate_e)
        {
            case FMKFDCAN_FRAME_BAUDRATE_40K:
                *f_targetBaudrateHz_pu32 = 40000UL;
                break;
            case FMKFDCAN_FRAME_BAUDRATE_125K:
                *f_targetBaudrateHz_pu32 = 125000UL;
                break;
            case FMKFDCAN_FRAME_BAUDRATE_250K:
                *f_targetBaudrateHz_pu32 = 250000UL;
                break;
            case FMKFDCAN_FRAME_BAUDRATE_500K:
                *f_targetBaudrateHz_pu32 = 500000UL;
                break;
            case FMKFDCAN_FRAME_BAUDRATE_1M:
                *f_targetBaudrateHz_pu32 = 1000000UL;
                break;
            case FMKFDCAN_FRAME_BAUDRATE_2M:
                *f_targetBaudrateHz_pu32 = 2000000UL;
                break;
            case FMKFDCAN_FRAME_BAUDRATE_4M:
                *f_targetBaudrateHz_pu32 = 4000000UL;
                break;
            case FMKFDCAN_FRAME_BAUDRATE_8M:
                *f_targetBaudrateHz_pu32 = 8000000UL;
                break;
            case FMKFDCAN_FRAME_BAUDRATE_NB:
            default:
                Ret_e = RC_ERROR_NOT_SUPPORTED;
                break;
        }
    }

    return Ret_e;
}

/*********************************
 * s_FMKFDCAN_GetTargetSamplePointPermil
 *********************************/
static t_uint16 s_FMKFDCAN_GetTargetSamplePointPermil(t_eFMKFDCAN_Baudrate f_baudrate_e)
{
    t_uint16 targetSamplePointPermil_u16 = FMKFDCAN_TIMING_SAMPLE_POINT_LOW_PERMIL;

    if(f_baudrate_e >= FMKFDCAN_FRAME_BAUDRATE_2M)
    {
        targetSamplePointPermil_u16 = FMKFDCAN_TIMING_SAMPLE_POINT_HIGH_PERMIL;
    }

    return targetSamplePointPermil_u16;
}

/*********************************
 * s_FMKFDCAN_GetAbsDiffU32
 *********************************/
static t_uint32 s_FMKFDCAN_GetAbsDiffU32(t_uint32 f_value1_u32, t_uint32 f_value2_u32)
{
    t_uint32 absDiff_u32 = 0U;

    if(f_value1_u32 >= f_value2_u32)
    {
        absDiff_u32 = f_value1_u32 - f_value2_u32;
    }
    else
    {
        absDiff_u32 = f_value2_u32 - f_value1_u32;
    }

    return absDiff_u32;
}

/*********************************
 * s_FMKFDCAN_RoundDivU32
 *********************************/
static t_uint16 s_FMKFDCAN_RoundDivU32(t_uint32 f_numerator_u32, t_uint32 f_denominator_u32)
{
    t_uint16 roundedValue_u16 = 0U;

    if(f_denominator_u32 != 0U)
    {
        roundedValue_u16 = (t_uint16)((f_numerator_u32 + (f_denominator_u32 / 2UL))
                                    / f_denominator_u32);
    }

    return roundedValue_u16;
}

#endif // APPSYS_MODULE_FMKCAN_ENABLE
//************************************************************************************
// End of File
//************************************************************************************
