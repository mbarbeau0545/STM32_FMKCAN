/*********************************************************************
 * @file        FMKFDCAN_ConfigSpecific.h
 * @brief       FDCAN ECU-specific configuration services.
 * @note        Keeps FMK_FDCAN independent from STM32 family preconditions.
 *
 * @author      xxxxxx
 * @date        jj/mm/yyyy
 * @version     1.0
 */

#ifndef FMKFDCAN_CONFIGSPECIFIC_H_INCLUDED
#define FMKFDCAN_CONFIGSPECIFIC_H_INCLUDED

// ********************************************************************
// *                      Includes
// ********************************************************************
#include "APP_CFG/ConfigFiles/APPSYS_ConfigPublic.h"

#ifdef APPSYS_MODULE_FMKCAN_ENABLE
#include "FMK_HAL/FMK_CAN/Src/FMK_FDCAN.h"

// ********************************************************************
// *                      Defines
// ********************************************************************
#if defined(FMKCPU_STM32_ECU_FAMILY_G4)
    #define FMKFDCAN_SRAMCAN_RF0_NBR ((t_uint32)6U)
    #define FMKFDCAN_SRAMCAN_RF1_NBR ((t_uint32)0U)
    #define FMKFDCAN_SRAMCAN_TEF_NBR ((t_uint32)3U)
    #define FMKFDCAN_SRAMCAN_TFQ_NBR ((t_uint32)3U)
    #define FMKFDCA_SRAMCAN_FLS_SIZE ((t_uint32)1U * 4U)
    #define FMKFDCA_SRAMCAN_FLE_SIZE ((t_uint32)2U * 4U)
    #define FMKFDCA_SRAMCAN_RF0_SIZE ((t_uint32)18U * 4U)
    #define FMKFDCA_SRAMCAN_RF1_SIZE ((t_uint32)18U * 4U)
    #define FMKFDCA_SRAMCAN_TEF_SIZE ((t_uint32)2U * 4U)
    #define FMKFDCA_SRAMCAN_TFQ_SIZE ((t_uint32)18U * 4U)
#elif defined(FMKCPU_STM32_ECU_FAMILY_H7)
    #define FMKCPU_SRAMCAN_FLS_NBR    ((t_uint32)28U)
    #define FMKCPU_SRAMCAN_FLE_NBR    ((t_uint32)8U)
    #define FMKFDCAN_SRAMCAN_RF0_NBR  ((t_uint32)64U)
    #define FMKFDCAN_SRAMCAN_RF1_NBR  ((t_uint32)0U)
    #define FMKFDCAN_SRAMCAN_RB0_NBR  ((t_uint32)0U)
    #define FMKFDCAN_SRAMCAN_TEF_NBR  ((t_uint32)32U)
    #define FMKFDCAN_SRAMCAN_TFQ_NBR  ((t_uint32)32U)
    #define FMKFDCA_SRAMCAN_FLS_SIZE  ((t_uint32)1U * 4U)
    #define FMKFDCA_SRAMCAN_FLE_SIZE  ((t_uint32)2U * 4U)
    #define FMKFDCA_SRAMCAN_RF0_SIZE  ((t_uint32)18U * 4U)
    #define FMKFDCA_SRAMCAN_RF1_SIZE  ((t_uint32)18U * 4U)
    #define FMKFDCA_SRAMCAN_RB0_SIZE  ((t_uint32)18U * 4U)
    #define FMKFDCA_SRAMCAN_TEF_SIZE  ((t_uint32)2U * 4U)
    #define FMKFDCA_SRAMCAN_TFQ_SIZE  ((t_uint32)18U * 4U)
#else
    #error "FDCAN SRAM configuration is not available for this ECU family"
#endif

// ********************************************************************
// *                      Types
// ********************************************************************
typedef struct
{
    t_uint16 prescaler_u16; /**< Prescaler value for the FDCAN clock. */
    t_uint8 syncSeg_u8;     /**< Synchronization segment value. */
    t_uint8 timeSeg1_u8;    /**< Time segment 1 value. */
    t_uint8 timeSeg2_u8;    /**< Time segment 2 value. */
} t_sFMKFDCAN_BaudrateCfg;

// ********************************************************************
// *                      Public functions - Prototypes
// ********************************************************************
t_eReturnCode FMKFDCAN_Set_BspNodeSpecificInit(FDCAN_HandleTypeDef *f_bspHandle_ps,
                                               t_uint32 f_bspClkDivider_u32);

t_eReturnCode FMKFDCAN_Set_KernelClockCfg(t_bool f_isAfterBspInit_b,
                                          FDCAN_HandleTypeDef *f_bspHandle_ps,
                                          t_eFMKFDCAN_ClockKernelDiv f_clockDivider_e,
                                          t_uint32 f_bspClkDivider_u32,
                                          t_bool f_isNode1Configured_b,
                                          FDCAN_HandleTypeDef *f_node1BspHandle_ps);

t_eReturnCode FMKFDCAN_GetKernelClockHz(t_eFMKFDCAN_ClockKernelDiv f_clockDivider_e,
                                        t_uint32 *f_kernelClockHz_pu32);

t_eReturnCode FMKFDCAN_ComputeBitTiming(t_uint32 f_kernelClockHz_u32,
                                        t_eFMKFDCAN_Baudrate f_baudrate_e,
                                        t_sFMKFDCAN_BaudrateCfg *f_baudrateCfg_ps);

#endif // APPSYS_MODULE_FMKCAN_ENABLE
#endif // FMKFDCAN_CONFIGSPECIFIC_H_INCLUDED
//************************************************************************************
// End of File
//************************************************************************************
