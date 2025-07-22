/**
 * @file        FMK_MAC.c
 * @brief       Framework Memory Access Control module.
 * @note        TemplateDetailsDescription.\n
 *
 * @author      mba
 * @date        15/09/2024
 * @version     1.0
 */






// ********************************************************************
// *                      Includes
// ********************************************************************
#include "./FMK_FDCAN.h"
#ifdef APPSYS_MODULE_FMKCAN_ENABLE

#include "FMK_HAL/FMK_CPU/Src/FMK_CPU.h"
#include "FMK_CFG/FMKCFG_ConfigFiles/FMKFDCAN_ConfigPrivate.h"
#include "FMK_HAL/FMK_IO/Src/FMK_IO.h"
#include "APP_CTRL/APP_SYS/Src/APP_SYS.h"

#include "Library/Queue/Src/LIBQueue.h"
#include "Library/SafeMem/SafeMem.h"
#include "stm32g4xx_hal.h"

// ********************************************************************
// *                      Defines
// ********************************************************************

// ********************************************************************
// *                      Types
// ********************************************************************
/* CAUTION : Automatic generated code section for Enum: Start */

/* CAUTION : Automatic generated code section for Enum: End */


/**
 * @brief Enumeration for the list of FDCAN BSP callback events.
 */
typedef enum __t_eFMKFDCAN_BspCallbackList
{
    FMKFDCAN_BSP_TX_CB_EVENT = 0x00,       /**< General Tx callback event. */
    FMKFDCAN_BSP_TX_CB_BUFFER_COMPLETE,    /**< Tx buffer transmission completed. */
    FMKFDCAN_BSP_TX_CB_BUFFER_ABORT,       /**< Tx buffer transmission aborted. */
    FMKFDCAN_BSP_TX_CB_FIFO_EMPTY,         /**< Tx FIFO is empty. */

    FMKFDCAN_BSP_RX_CB_FIFO_0,             /**< Rx FIFO 0 event. */
    FMKFDCAN_BSP_RX_CB_FIFO_1,             /**< Rx FIFO 1 event. */

    FMKFDCAN_BSP_CB_PROTOCOL_ERR,          /**< Protocol error detected. */

    FMKFDCAN_BSP_CB_NB                     /**< Total number of callbacks (for bounds checking). */
} t_eFMKFDCAN_BspCallbackList;

/**
 * @brief Bsp Callback Status
 */
typedef enum __t_eFMKFDCAN_BspStatusCb
{
    FMKFDCAN_CALLBACK_STATUS_ACTIVATE = 0x00,       /**< The Callback needs activated by software */
    FMKFDCAN_CALLBACK_STATUS_DEACTIVATE,            /**< The Callback needs deactivated by software */

    FMKFDCAN_CALLBACK_STATUS_NB,
} t_eFMKFDCAN_BspStatusCb;
//-----------------------------ENUM TYPES-----------------------------//

/* CAUTION : Automatic generated code section for Structure: Start */

/* CAUTION : Automatic generated code section for Structure: End */
//-----------------------------STRUCT TYPES---------------------------//
/**
 * @brief Soft flags structure to manage various FDCAN software flags.
 */
typedef struct __t_sFMKFDCAN_FlagSoft
{
    t_bool TxQueuePending_b;       /**< Indicates if there are pending frames in the Tx queue. */
    t_bool RxQueuePending_b;       /**< Indicates if there are pending frames in the Rx queue. */
    t_bool TxFramePrioPending_b;   /**< Indicates if a prioritized Tx frame is pending. */
    t_bool ErrorDetected_b;        /**< Indicates if an error has been detected. */
} t_sFMKFDCAN_FlagSoft;

/**
 * @brief Node information structure for FDCAN configuration and status.
 */
typedef struct __t_sFMKFDCAN_NodeInfo
{
    FDCAN_HandleTypeDef bspNode_s;           /**< BSP handle for FDCAN peripheral. */
    t_eFMKCPU_ClockPort c_Clock_e;     /**< Clock port associated with the FDCAN node. */
    t_eFMKCPU_IRQNType c_IrqnLine1_e;  /**< IRQ line 1 associated with the FDCAN peripheral. */
    t_eFMKCPU_IRQNType c_IrqnLine2_e;  /**< IRQ line 2 associated with the FDCAN peripheral. */
    t_sFMKFDCAN_FlagSoft Flag_s;             /**< Software flags related to the FDCAN node. */
    t_bool isNodeConfigured_b;               /**< Indicates if the FDCAN node has been configured. */
    t_bool isNodeActive_b;                   /**< Indicates if the FDCAN node is active and operational. */
    t_eFMKFDCAN_NodeStatus nodeHealth_e;      /**< Node status */
} t_sFMKFDCAN_NodeInfo;

/**
 * @brief Rx item buffer structure for receiving FDCAN frames.
 */
typedef struct __t_sFMKFDCAN_RxItemBuffer
{
    FDCAN_RxHeaderTypeDef bspRxItem_s; /**< FDCAN BSP Rx header structure for frame details. */
    t_uint8 data_ua8[FMKFDCAN_DLC_8];  /**< Data buffer for the received frame. */
} t_sFMKFDCAN_RxItemBuffer;

/**
 * @brief Tx item buffer structure for transmitting FDCAN frames.
 */
typedef struct 
{
    FDCAN_TxHeaderTypeDef bspTxItem_s; /**< FDCAN BSP Tx header structure for frame details. */
    t_uint8 data_ua8[FMKFDCAN_DLC_8];  /**< Data buffer for the frame to transmit. */
} t_sFMKFDCAN_TxItemBuffer;

/**
 * @brief User register structure for managing FDCAN items and callbacks.
 */
typedef struct
{
    t_sFMKFDCAN_ItemIdentifier itemId_s; /**< Identifier structure for the FDCAN item. */
    t_uint32 maskId_u32;                 /**< Mask identifier for the FDCAN item. */
    t_cbFMKFDCAN_RcvItem *rcvItem_cb;    /**< Callback function to handle received items. */
} t_sFMKFDCAN_UserItemRegister;



/* CAUTION : Automatic generated code section : Start */

/* CAUTION : Automatic generated code section : End */
//-----------------------------TYPEDEF TYPES---------------------------//

// ********************************************************************
// *                      Prototypes
// ********************************************************************
	
// ********************************************************************
// *                      Variables
// ********************************************************************
static t_eCyclicModState g_FmkCan_ModState_e = STATE_CYCLIC_CFG;
/* Store information about node*/
t_sFMKFDCAN_NodeInfo g_NodeInfo_as[FMKFDCAN_NODE_NB];
/*------------------------Tx FIFO MANAGEMENT-------------------------------*/
/**< Transmission software Buffer*/
t_sFMKFDCAN_TxItemBuffer g_TxSoftBuffer_as[FMKFDCAN_NODE_NB][FMKFDCAN_TX_SOFT_BUFF_SIZE];
/**< Transmission Queue management*/
t_sLIBQUEUE_QueueCore g_TxSoftQueue_as[FMKFDCAN_NODE_NB];
/*------------------------WITH CALLBACK (EVENT) MANAGMENT-------------------------------*/
t_sFMKFDCAN_RxItemBuffer g_RxBufferEvnt_as[FMKFDCAN_NODE_NB][FMKFDCAN_RX_SOFT_BUFF_SIZE];
/**< Reception Queue management*/
t_sLIBQUEUE_QueueCore g_RxSoftQueue_as[FMKFDCAN_NODE_NB];
/**< User Item Registration Managment */
t_sFMKFDCAN_UserItemRegister g_UserRegisterEvnt_as[FMKFDCAN_NODE_NB][FMKFDCAN_RX_NUM_REGISTRATION_EVNT];
/**< Save the number of register already received */
t_uint8 g_CtrUserRegisterEvnt_ua8[FMKFDCAN_NODE_NB];


/**< Hardware Interrupt Management */
t_eFMKFDCAN_BspStatusCb g_BspCbMngmt_ae[FMKFDCAN_NODE_NB][FMKFDCAN_BSP_CB_NB];
//********************************************************************************
//                      Local functions - Prototypes
//*********************************************************************************
/**
 *	@brief      Init the CAN driver used thanks to c_FmkCan_IsNodeActive
 *
 */
static t_eReturnCode s_FMKFDCAN_ConfigurationState(void);
/**
 *	@brief      Activate Interrupt line 
 *
 */
static t_eReturnCode s_FMKFDCAN_PreOperational(void);
/**
 *	@brief      Perform Cyclic Operation
 *
 */
static t_eReturnCode s_FMKFDCAN_Operational(void);
/**
*	@brief      Configure The Initiation of the Hardware
*	@note       Set the pin, Set the bsp Init Strucuture, 
*               Set the hardware clock, set the Interruption Enabling
*               Call FDCAN_Init and configure Fifo Mode.\n
*
 *	@param[in]  f_Node_e     : The Node where the RxMsg is Pending -> enum value from @ref t_eFMKFDCAN_NodeList
 *	@param[in]  f_NodeCfg_s  : software init structure @ref t_sFMKFDCAN_DrvNodeCfg
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
*
*/
static t_eReturnCode s_FMKFDCAN_InitDriver(t_eFMKFDCAN_NodeList f_Node_e, t_sFMKFDCAN_DrvNodeCfg f_NodeCfg_s);

/**
 *	@brief      Set the initiation of the Node.\n
 *	@note       We extract from software confgiuration the bsp configuration
 *              and copy it into bsp init structure. Then we call HAL_FDCAN.\n
 *
 *	@param[in]  f_bspInit_ps : bsp init structrue @ref FDCAN_HandleTypeDef
 *	@param[in]  f_NodeCfg_s  : software init structure @ref t_sFMKFDCAN_DrvNodeCfg
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 *
 */
static t_eReturnCode s_FMKFDCAN_SetBspNodeInit(FDCAN_HandleTypeDef *f_bspInit_ps, t_sFMKFDCAN_DrvNodeCfg f_NodeCfg_s);
/**
 *	@brief      Function called when Bsp Tx Interruption occured.\n
 *	@note       In this function, We firstly see if the flag TxItemPending is raised.\n
 *              If it's the case, that means there is messages waiting to be sent in the 
 *              software buffer, we send FMKFDCAN_MAX_TX_ITEM_SEND_PER_IT messages per Interruption
 *              or at least the amount of TxItem in the Queue.\n
 *              If the Queu Size equals 0, reset the flag and Reset The Interruption for this function
 *              because it's no longer required as there isn't message to send.\n
 *
 *	@param[in] f_bspInfo_ps : a pointor to bsp Node, @ref FDCAN_HandleTypeDef
 *	@param[in] f_EvntCbInfo_u32 : information and the bsp interruption
 *	@param[in] f_bspRxCallback_e : information about the callback that raise the interruption
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 *
 */
static void s_FMKFDCAN_BspTxEventCb(FDCAN_HandleTypeDef *f_bspInfo_ps, 
                                              t_uint32 f_EvntCbInfo_u32, 
                                              t_eFMKFDCAN_BspCallbackList f_bspTxCallback_e);
/**
 *	@brief      Function called When Bsp Rx Interruption occured.\n
 *	@note       In this function, We firstly trying to know which node and which Bsp Rx Fifo
 *              Causes the intetrruption with f_bspInfo_ps and f_EvntCbInfo_u32.\n
 *              Then if f_EvntCbInfo_u32 says that a message has been received we treat wether 
 *              we call user if the Identifier is register with the callback or we store it into the dedicate 
 *              Software Buffer.\n 
 *              If the flag is Fifo Full we rapidly put FMKFDCAN_MAX_ITEM_ON_QUEUE_PER_IT messages into a Software Buffer 
 *              and raise flag so that message will be treated in cyclic.\n
 *              
 *	@param[in] f_bspInfo_ps : a pointor to bsp Node, @ref FDCAN_HandleTypeDef
 *	@param[in] f_EvntCbInfo_u32 : information and the bsp interruption
 *	@param[in] f_bspRxCallback_e : information about the callback that raise the interruption
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 *
 */
static void s_FMKFDCAN_BspRxEventCb(FDCAN_HandleTypeDef *f_bspInfo_ps, 
                                              t_uint32 f_EvntCbInfo_u32, 
                                              t_eFMKFDCAN_BspCallbackList f_bspRxCallback_e);
/**
 *	@brief      Receive Error From Bsp, Save them, and raise a flag 'Error Pending'
 *
 *	@param[in] f_bspInfo_ps : a pointor to bsp Node, @ref FDCAN_HandleTypeDef
 *	@param[in] f_EvntCbInfo_u32 : information and the bsp interruption
 *
 *
 */
static void s_FMKFDCAN_BspErrorEventCb(FDCAN_HandleTypeDef *f_bspInfo_ps, 
                                              t_uint32 f_EvntCbInfo_u32);
/**
 *	@brief      Get a RX Message strcuture from bsp fifo
 *
 *	@param[in] f_Node_e        : The Node where the RxMsg is Pending -> enum value from @ref t_eFMKFDCAN_NodeList
 *	@param[in] f_RxFifo_e      : The Fifo where the RxMsg is Pending
 *	@param[in] f_bspRxItem_ps  : A pointor To receive RxMsg
 *	@param[in] f_bspData_pu8   : A pointor to data
 *
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 *  @retval RC_ERROR_WRONG_RESULT             @ref RC_ERROR_WRONG_RESULT
 *
 */
static t_eReturnCode s_FMKFDCAN_RetrieveRxItem(t_eFMKFDCAN_NodeList f_Node_e,
                                                  t_eFMKFDCAN_HwRxFifoList f_RxFifo_e,
                                                  FDCAN_RxHeaderTypeDef * f_bspRxItem_ps,
                                                  t_uint8 * f_bspData_pu8);
/**
 *	@brief      Copy a Software TxItem into a Bsp TxItem.\n
 *
 *	@param[in] f_bspTxItem_ps   : a pointer to bsp RxITem to copy Data Strucutre -> @ref FDCAN_TxHeaderTypeDef
 *	@param[in] f_TxItemCfg_s    : a pointer to software TxITem  -> @ref t_sFMKFDCAN_TxItemCfg
 *
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 */
static t_eReturnCode s_FMKFDCAN_CopyBspTxItem(t_sFMKFDCAN_TxItemCfg *f_TxItemCfg_s, FDCAN_TxHeaderTypeDef *f_bspTxItem_ps);
/**
 *	@brief      Copy a Software RxItem into a Bsp RxItem.\n
 *
 *	@param[in] f_bspRxItem_ps : a pointer to bsp TxITem to copy Data Strucutre -> @ref FDCAN_RxHeaderTypeDef
 *	@param[in] f_RxItem_ps    : a pointer to oftware TxITem  -> @ref t_sFMKFDCAN_RxItemEvent
 *	@param[in] f_bspData_pu8  : a pointor ta data
 *	 
 *
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 */
static t_eReturnCode s_FMKFDCAN_CopyBspRxItem(FDCAN_RxHeaderTypeDef *f_bspRxItem_ps,
                                              t_sFMKFDCAN_RxItemEvent * f_RxItem_ps,
                                              t_uint8 * f_bspData_pu8);
/**
 *	@brief      Get the user Register Index from Software Fifo Buffer.\n
 *	@note       This function purpose is to find the RxItem store into software buffer
 *              using f_Identifier_u32 and f_NodeRegister_pas.\n
 *              At the Registration we copy Identifier and Mask into Software Buffer Registration.\n
 *              In order to return f_idxUserRegister_pu8, We compare every Buffer_item & mask with
 *              f_Identifier_u32 and the mask.\n
 *              If the Identifier is not found in Software Buffer f_idxUserRegister_pu8 = 0
 *
 *	@param[in] f_Identifier_u32 : The identifier to find into Registration
 *	@param[in] f_NodeRegister_pas : Registration Software Buffer -> @ref t_sFMKFDCAN_UserItemRegister
 *	@param[in] f_RegistrationCtr_u8 : Number of registration which as been made in f_NodeRegister_pas
 *	@param[in] f_idxUserRegister_pu8 : Storage for Idx Register.\n
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_LIMIT_REACHED            @ref RC_ERROR_LIMIT_REACHED
 *
 */
static t_eReturnCode s_FMKFDCAN_GetUserRegisterIndex(t_uint32 f_Identifier_u32,
                                                     t_sFMKFDCAN_UserItemRegister * f_NodeRegister_pas,
                                                     t_uint8 f_RegistrationCtr_u8,
                                                     t_uint8 * f_idxUserRegister_pu8);

/**
 *	@brief      Configure the Global Filter for All Node And FIFO
 *  @note       At this point, all Id are acdepted there are not filters.
 *              To not let the filter floating we set them to accept every id
 *
 *
 *	@param[in] f_Node_e : CAN  Node -> enum value from @ref t_eFMKFDCAN_NodeList
 *	@param[in] f_bspCb_e : bsp calback -> enum value from @ref t_eFMKFDCAN_BspCallbackList
 *	@param[in] f_status_e : status bsp calback -> enum value from @ref t_eFMKFDCAN_BspStatusCb
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_WRONG_RESULT             @ref RC_ERROR_WRONG_RESULT
 */
static t_eReturnCode s_FMKFDCAN_SetNodeFilters(void);
/**
 *	@brief      Set the Hardware Bsp Callback Status -> Active or Inactive.\n
 *	@note       The Hal_FDCAN Library allows us to dynamically set the callback status 
 *              of interrutpion.\n
 *              This function update the status using f_status_e of a callback f_bspCb_e
 *              of a dedicate Node.\n 
 *
 *
 *	@param[in] f_Node_e : CAN  Node -> enum value from @ref t_eFMKFDCAN_NodeList
 *	@param[in] f_bspCb_e : bsp calback -> enum value from @ref t_eFMKFDCAN_BspCallbackList
 *	@param[in] f_status_e : status bsp calback -> enum value from @ref t_eFMKFDCAN_BspStatusCb
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_WRONG_RESULT             @ref RC_ERROR_WRONG_RESULT
 */
static t_eReturnCode s_FMKFDCAN_SetHwBspCallbackStatus(t_eFMKFDCAN_NodeList f_Node_e ,
                                                        t_eFMKFDCAN_BspCallbackList f_bspCb_e,
                                                        t_eFMKFDCAN_BspStatusCb f_status_e);
/**
 *	@brief      Set the Node FIFO Operation Mode for Hardware Rx Fifo.\n
 *	@note       Each Node has x Rx FIFO, which can configured as Overwrite or Blocking.\n
 *              When the fifo is full and a message is receive, in the first case, 
 *              the hardware erase an element and write the new one. In the other case,
 *              new message is not receive
 *              Here we decided to applied f_fifoMode_e at every Fifos Node.\n
 *              
 *	@param[in] f_Node_e : CAN  Node -> enum value from @ref t_eFMKFDCAN_NodeList
 *	@param[in] f_fifoMode_e : Operation Fifo Mode, enum value from @ref t_eFMKFDCAN_FifoOpeMode
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_WRONG_RESULT             @ref RC_ERROR_WRONG_RESULT
 */
static t_eReturnCode s_FMKFDCAN_SetHwFifoOpeMode(t_eFMKFDCAN_NodeList f_Node_e ,t_eFMKFDCAN_FifoOpeMode f_fifoMode_e);
/**
 *	@brief          Get the Bsp Callback Identifier from Software enum.\n
 *
 *	@param[in]      f_Callback_e : enum value for the callback, value from @ref t_eFMKFDCAN_BspCallbackList.\n
 *	@param[out]     f_bspCallback_u32 : Storage for Bsp Callback Identifier.\n
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 *
 */
static t_eReturnCode s_FMKFDCAN_GetBspCallbackId(t_eFMKFDCAN_BspCallbackList f_Callback_e, t_uint32 *f_bspCallback_u32);
/**
 *	@brief          Get the Bsp FIFO Identifier from Software enum.\n
 *
 *	@param[in]      f_FiRxfoId_e : enum value for FIFO, value from @ref t_eFMKFDCAN_HwRxFifoList.\n
 *	@param[out]     f_bspRxFifoId_u32 : Storage for Bsp FIFO Identifier.\n
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 *
 */
static t_eReturnCode s_FMKFDCAN_GetBspRxFifoId(t_eFMKFDCAN_HwRxFifoList f_FiRxfoId_e, t_uint32 *f_bspRxFifoId_u32);
/**
 *	@brief          Get the Bsp Buffer Identifier from Software enum.\n
 *
 *	@param[in]      f_TxFifoId_e : enum value for FIFO, value from @ref t_eFMKFDCAN_HwRxFifoList.\n
 *	@param[out]     f_bspTxFifoId_u32 : Storage for Bsp FIFO Identifier.\n
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 *
 */
static t_eReturnCode s_FMKFDCAN_GetBspTxFifoId(t_eFMKFDCAN_HwTxFifoList f_TxFifoId_e, t_uint32 *f_bspTxFifoId_u32);
/**
 *	@brief          Get the Bsp Frame Purpose from Software enum.\n
 *
 *	@param[in]      f_frameType_e : enum value for frame purpose, value from @ref t_eFMKFDCAN_FramePurpose.\n
 *	@param[out]     f_bspFramePurpose_pu32 : Storage for Bsp Frame Purpose.\n
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 *
 */
static t_eReturnCode s_FMKFDCAN_GetBspFramePurpose(t_eFMKFDCAN_FramePurpose f_frameType_e, t_uint32 *f_bspFramePurpose_pu32);
/**
 *	@brief          Get the Bsp Queue Type from Software enum.\n
 *
 *	@param[in]      f_queueType_e : enum value for queue type, value from @ref t_eFMKFDCAN_HwTxQueueType.\n
 *	@param[out]     f_bspQueueType_pu32 : Storage for Bsp Queue Type.\n
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 *
 */
static t_eReturnCode s_FMKFDCAN_GetBspQueueType(t_eFMKFDCAN_HwTxQueueType f_queueType_e, t_uint32 *f_bspQueueType_pu32);
/**
 *	@brief          Get the Bsp FIFO Operation Mode from Software enum.\n
 *
 *	@param[in]      f_fifoOpeMode : enum value for FIFO operation mode, value from @ref t_eFMKFDCAN_FifoOpeMode.\n
 *	@param[out]     f_bspfifoOpeMode_e : Storage for Bsp FIFO Operation Mode.\n
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 *
 */
static t_eReturnCode s_FMKFDCAN_GetBspFifoOpeMode(t_eFMKFDCAN_FifoOpeMode f_fifoOpeMode , t_uint32 *f_bspfifoOpeMode_e);
/**
 *	@brief          Get the Bsp Identifier Type from Software enum.\n
 *
 *	@param[in]      f_IdType : enum value for identifier type, value from @ref t_eFMKFDCAN_IdentifierType.\n
 *	@param[out]     f_bspIdType_pu32 : Storage for Bsp Identifier Type.\n
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 *
 */
static t_eReturnCode s_FMKFDCAN_GetBspIdType(t_eFMKFDCAN_IdentifierType f_IdType, t_uint32 *f_bspIdType_pu32);
/**
 *	@brief          Get the Bsp Bit Rate Status from Software enum.\n
 *
 *	@param[in]      f_BitRate_e : enum value for bit rate status, value from @ref t_eFMKFDCAN_BitRateSwitchStatus.\n
 *	@param[out]     f_BspBitRate_pu32 : Storage for Bsp Bit Rate Status.\n
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 *
 */
static t_eReturnCode s_FMKFDCAN_GetBspBitRateStatus(t_eFMKFDCAN_BitRateSwitchStatus f_BitRate_e, t_uint32 *f_BspBitRate_pu32);
/**
 *	@brief          Get the Bsp Data Length Code (DLC) from Software enum.\n
 *
 *	@param[in]      f_Dlc_e : enum value for data length code, value from @ref t_eFMKFDCAN_DataLength.\n
 *	@param[out]     f_BspDlc_pu32 : Storage for Bsp Data Length Code.\n
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 *
 */
static t_eReturnCode s_FMKFDCAN_GetBspDlc(t_eFMKFDCAN_DataLength f_Dlc_e, t_uint32 *f_BspDlc_pu32);
/**
 *	@brief          Get the Software Data Length Code (DLC) from Bsp value.\n
 *
 *	@param[in]      f_BspDlc_u32 : Bsp Data Length Code.\n
 *	@param[out]     f_Dlc_pe : Storage for Software Data Length Code, value from @ref t_eFMKFDCAN_DataLength.\n
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 *
 */
static t_eReturnCode s_FMKFDCAN_GetDlcFromBsp(t_uint32 f_BspDlc_u32, t_eFMKFDCAN_DataLength *f_Dlc_pe);
/**
 *	@brief          Get the Bsp Frame Format from Software enum.\n
 *
 *	@param[in]      f_formatType_e : enum value for frame format, value from @ref t_eFMKFDCAN_FrameFormat.\n
 *	@param[out]     f_BspFormatType_pu32 : Storage for Bsp Frame Format.\n
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 *
 */
static t_eReturnCode s_FMKFDCAN_GetBspFDFormat(t_eFMKFDCAN_FrameFormat f_formatType_e, t_uint32 *f_BspFormatType_pu32);
/**
 *	@brief          Get the Bsp Clock Kernel Divider from Software enum.\n
 *
 *	@param[in]      f_ClkDiv_e : enum value for clock kernel divider, value from @ref t_eFMKFDCAN_ClockKernelDiv.\n
 *	@param[out]     bspClkDiv_pu32 : Storage for Bsp Clock Kernel Divider.\n
 *	 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
 *  @retval RC_ERROR_NOT_SUPPORTED            @ref RC_ERROR_NOT_SUPPORTED
 *
 */
static t_eReturnCode s_FMKFDCAN_GetClockKernelDivider(t_eFMKFDCAN_ClockKernelDiv f_ClkDiv_e, t_uint32 * bspClkDiv_pu32);
//********************************************************************************
//                      Public functions - Implementation
//********************************************************************************
/*******************
* FMKFDCAN_Init
******************/
t_eReturnCode FMKFDCAN_Init(void)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint8 idxBspCallback_u8;
    t_uint8 idxNode_u8 = (t_uint8)0;
    t_uint8 LLI_u8 = (t_uint8)0;
    t_uint8 LLI2_u8 = (t_uint8)0;

    t_sLIBQUEUE_QueueCfg TxBufferCfg_s = {
        .bufferSize_u8 = FMKFDCAN_TX_SOFT_BUFF_SIZE,
        .elementSize_u8 = sizeof(t_sFMKFDCAN_TxItemBuffer),
        .enableOverwrite_b = False,
    };
    t_sLIBQUEUE_QueueCfg RxBufferCfg_s = {
        .bufferSize_u8 = FMKFDCAN_RX_SOFT_BUFF_SIZE,
        .elementSize_u8 = sizeof(t_sFMKFDCAN_RxItemBuffer),
        .enableOverwrite_b = False,
    };

    //-----------------For Every Node in FDCAN----------------//
    for (idxNode_u8 = (t_uint8)0 ; (idxNode_u8 < FMKFDCAN_NODE_NB) && (Ret_e == RC_OK) ; idxNode_u8++)
    {   
        // init bufferHead_p
        RxBufferCfg_s.bufferHead_pv = (void *)NULL;
        // init Node Global Inforamtion
        g_NodeInfo_as[idxNode_u8].Flag_s.ErrorDetected_b      = (t_bool)False;
        g_NodeInfo_as[idxNode_u8].Flag_s.RxQueuePending_b     = (t_bool)False;
        g_NodeInfo_as[idxNode_u8].Flag_s.TxQueuePending_b     = (t_bool)False;
        g_NodeInfo_as[idxNode_u8].Flag_s.TxFramePrioPending_b = (t_bool)False;
        g_NodeInfo_as[idxNode_u8].isNodeActive_b              = (t_bool)False;
        g_NodeInfo_as[idxNode_u8].isNodeConfigured_b          = (t_bool)False;
        g_NodeInfo_as[idxNode_u8].nodeHealth_e                = FMKFDCAN_NODE_STATE_OK;
        g_NodeInfo_as[idxNode_u8].c_Clock_e                   = c_FmkFdcan_NodeCfg_as[idxNode_u8].c_Clock_e;
        g_NodeInfo_as[idxNode_u8].c_IrqnLine1_e               = c_FmkFdcan_NodeCfg_as[idxNode_u8].c_Clock_e;
        g_NodeInfo_as[idxNode_u8].c_IrqnLine2_e               = c_FmkFdcan_NodeCfg_as[idxNode_u8].c_Clock_e;
        g_NodeInfo_as[idxNode_u8].bspNode_s.Instance          = c_FmkFdcan_NodeCfg_as[idxNode_u8].Instance;


        
        // Init countor register 
        g_CtrUserRegisterEvnt_ua8[idxNode_u8] = (t_uint8)0;
        //-------------------Init Bsp Callback To Deactivate----------------//
        for(idxBspCallback_u8 = (t_uint8)0 ; idxBspCallback_u8 < FMKFDCAN_BSP_CB_NB ; idxBspCallback_u8++)
        {
            g_BspCbMngmt_ae[idxNode_u8][idxBspCallback_u8] = FMKFDCAN_CALLBACK_STATUS_DEACTIVATE;
        }
        //-------------------Init Tx, Rx Software Queue ----------------//
        for(LLI_u8 = (t_uint8)0 ; LLI_u8 < FMKFDCAN_RX_SOFT_BUFF_SIZE ; LLI_u8++)
        {// Rx buffer
            g_RxBufferEvnt_as[idxNode_u8][LLI_u8].bspRxItem_s.BitRateSwitch = (t_uint32)0;
            g_RxBufferEvnt_as[idxNode_u8][LLI_u8].bspRxItem_s.DataLength = (t_uint32)0;
            g_RxBufferEvnt_as[idxNode_u8][LLI_u8].bspRxItem_s.ErrorStateIndicator = (t_uint32)0;
            g_RxBufferEvnt_as[idxNode_u8][LLI_u8].bspRxItem_s.FDFormat = (t_uint32)0;
            g_RxBufferEvnt_as[idxNode_u8][LLI_u8].bspRxItem_s.Identifier = (t_uint32)0;
            g_RxBufferEvnt_as[idxNode_u8][LLI_u8].bspRxItem_s.IdType = (t_uint32)0;
            g_RxBufferEvnt_as[idxNode_u8][LLI_u8].bspRxItem_s.IsFilterMatchingFrame = (t_uint32)0;
            g_RxBufferEvnt_as[idxNode_u8][LLI_u8].bspRxItem_s.RxFrameType = (t_uint32)0;
            g_RxBufferEvnt_as[idxNode_u8][LLI_u8].bspRxItem_s.RxTimestamp = (t_uint32)0;
            // init buffer data
            for (LLI2_u8 = (t_uint8)0; LLI2_u8 < FMKFDCAN_DLC_8 ; LLI2_u8++)
            {
                g_RxBufferEvnt_as[idxNode_u8][LLI_u8].data_ua8[LLI2_u8] = (t_uint8)0;
            }
        }

        for(LLI_u8 = (t_uint8)0 ; LLI_u8 < FMKFDCAN_TX_SOFT_BUFF_SIZE ; LLI_u8++)
        {// TxBuffer
            g_TxSoftBuffer_as[idxNode_u8][LLI_u8].bspTxItem_s.BitRateSwitch = (t_uint32)0;
            g_TxSoftBuffer_as[idxNode_u8][LLI_u8].bspTxItem_s.DataLength = (t_uint32)0;
            g_TxSoftBuffer_as[idxNode_u8][LLI_u8].bspTxItem_s.ErrorStateIndicator = (t_uint32)0;
            g_TxSoftBuffer_as[idxNode_u8][LLI_u8].bspTxItem_s.FDFormat = (t_uint32)0;
            g_TxSoftBuffer_as[idxNode_u8][LLI_u8].bspTxItem_s.Identifier = (t_uint32)0;
            g_TxSoftBuffer_as[idxNode_u8][LLI_u8].bspTxItem_s.IdType = (t_uint32)0;
            g_TxSoftBuffer_as[idxNode_u8][LLI_u8].bspTxItem_s.MessageMarker = (t_uint32)0;
            g_TxSoftBuffer_as[idxNode_u8][LLI_u8].bspTxItem_s.TxEventFifoControl = (t_uint32)0;
            g_TxSoftBuffer_as[idxNode_u8][LLI_u8].bspTxItem_s.TxFrameType = (t_uint32)0;
            // init buffer data
            for (LLI2_u8 = (t_uint8)0; LLI2_u8 < FMKFDCAN_DLC_8 ; LLI2_u8++)
            {
                g_TxSoftBuffer_as[idxNode_u8][LLI_u8].data_ua8[LLI2_u8] = (t_uint8)0;
            }
        }
        //-------------------Initialize RxItem Registration ----------------//
        for(LLI_u8 = (t_uint8)0 ; LLI_u8 < (t_uint8)FMKFDCAN_RX_NUM_REGISTRATION_EVNT ; LLI_u8++)
        {
            g_UserRegisterEvnt_as[idxNode_u8][LLI_u8].itemId_s.Identifier_u32 = (t_uint32)0;
            g_UserRegisterEvnt_as[idxNode_u8][LLI_u8].itemId_s.FramePurpose_e = FMKFDCAN_FRAME_PURPOSE_NB;
            g_UserRegisterEvnt_as[idxNode_u8][LLI_u8].itemId_s.IdType_e = FMKFDCAN_IDTYPE_NB;
            g_UserRegisterEvnt_as[idxNode_u8][LLI_u8].maskId_u32 = (t_uint32)0;
            g_UserRegisterEvnt_as[idxNode_u8][LLI_u8].rcvItem_cb = (t_cbFMKFDCAN_RcvItem *)NULL_FUNCTION;
        }
        //-------------------Configure the Rx, Tx Queue----------------//
        RxBufferCfg_s.bufferHead_pv = g_RxBufferEvnt_as[idxNode_u8];
        // Configure Rx Buffer Queue
        Ret_e = LIBQUEUE_Create(&g_RxSoftQueue_as[idxNode_u8], RxBufferCfg_s);
        // Configure Tx Buffer Queue
        if(Ret_e == RC_OK)
        {
            TxBufferCfg_s.bufferHead_pv = g_TxSoftBuffer_as[idxNode_u8];
            Ret_e = LIBQUEUE_Create(&g_TxSoftQueue_as[idxNode_u8], TxBufferCfg_s);
        }
    }
    
    return Ret_e;
}

/***********************
* FMKFDCAN_Cyclic
************************/
t_eReturnCode FMKFDCAN_Cyclic(void)
{
    t_eReturnCode Ret_e = RC_OK;

    switch (g_FmkCan_ModState_e)
    {
        case STATE_CYCLIC_CFG:
        {
            Ret_e = s_FMKFDCAN_ConfigurationState();;
            if(Ret_e == RC_OK)
            {
                g_FmkCan_ModState_e = STATE_CYCLIC_WAITING;
            }
            break;
        }
        case STATE_CYCLIC_PREOPE:
        {
            Ret_e = s_FMKFDCAN_PreOperational();
            if(Ret_e  == RC_OK)
            {
                g_FmkCan_ModState_e = STATE_CYCLIC_OPE;
            }
            break;
        }
        case STATE_CYCLIC_WAITING:
        {
            // nothing to do just wait AppSys Signal
            break;
        }
        case STATE_CYCLIC_OPE:
        {
            Ret_e = s_FMKFDCAN_Operational();
            if(Ret_e < RC_OK)
            {
                g_FmkCan_ModState_e = STATE_CYCLIC_ERROR;
            }
            break;
        }
        
        case STATE_CYCLIC_ERROR:
        {
            break;
        }
        case STATE_CYCLIC_BUSY:
        default:
            Ret_e = RC_OK;
            break;
    }
    return Ret_e;
}

/*********************************
 * FMKCDA_GetState
 *********************************/
t_eReturnCode FMKFDCAN_GetState(t_eCyclicModState *f_State_pe)
{
    t_eReturnCode Ret_e = RC_OK;
    
    if(f_State_pe == (t_eCyclicModState *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        *f_State_pe = g_FmkCan_ModState_e;
    }

    return Ret_e;
}

/*********************************
 * FMKCDA_SetState
 *********************************/
t_eReturnCode FMKFDCAN_SetState(t_eCyclicModState f_State_e)
{

    g_FmkCan_ModState_e = f_State_e;
    return RC_OK;
}

/*********************************
* FMKFDCAN_ConfigureRxItemEvent
*********************************/
t_eReturnCode FMKFDCAN_ConfigureRxItemEvent(t_eFMKFDCAN_NodeList f_Node_e, t_sFMKFDCAN_RxItemEventCfg f_RxItemCfg_s)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint8 idxCounter_u8 = (t_uint8)0;

    if((f_Node_e >= FMKFDCAN_NODE_NB)
    || (f_RxItemCfg_s.callback_cb == (t_cbFMKFDCAN_RcvItem *)NULL_FUNCTION))
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(g_NodeInfo_as[f_Node_e].isNodeConfigured_b == (t_bool)False)
    {
        Ret_e = RC_ERROR_INSTANCE_NOT_INITIALIZED;
    }
    if(g_CtrUserRegisterEvnt_ua8[f_Node_e] >= FMKFDCAN_RX_NUM_REGISTRATION_EVNT)
    {
        Ret_e = RC_ERROR_LIMIT_REACHED;
    }
    if(Ret_e == RC_OK)
    {
        //-------------------Copy Information into Register Node Variable-------------------//
        idxCounter_u8 = g_CtrUserRegisterEvnt_ua8[f_Node_e];
        g_UserRegisterEvnt_as[f_Node_e][idxCounter_u8].itemId_s.Identifier_u32 = f_RxItemCfg_s.ItemId_s.Identifier_u32;
        g_UserRegisterEvnt_as[f_Node_e][idxCounter_u8].itemId_s.FramePurpose_e = f_RxItemCfg_s.ItemId_s.FramePurpose_e;
        g_UserRegisterEvnt_as[f_Node_e][idxCounter_u8].itemId_s.IdType_e = f_RxItemCfg_s.ItemId_s.IdType_e;
        g_UserRegisterEvnt_as[f_Node_e][idxCounter_u8].maskId_u32 = f_RxItemCfg_s.maskId_u32;
        g_UserRegisterEvnt_as[f_Node_e][idxCounter_u8].rcvItem_cb = f_RxItemCfg_s.callback_cb;

        //-------------------update countor Register-------------------//
        g_CtrUserRegisterEvnt_ua8[f_Node_e] += (t_uint8)1;
    }

    return Ret_e;
}

/*********************************
* FMKFDCAN_SendTxItem
*********************************/
t_eReturnCode FMKFDCAN_SendTxItem(t_eFMKFDCAN_NodeList f_Node_e, t_sFMKFDCAN_TxItemCfg f_TxItemCfg_s)
{
    t_eReturnCode Ret_e = RC_OK;
    HAL_StatusTypeDef bspRet_e = HAL_OK;
    t_sFMKFDCAN_TxItemBuffer SoTxitem_s;
    t_uint8 idxData_u8;
    t_uint32 fifoLevel_u32 = 0;


    if(f_Node_e >= FMKFDCAN_NODE_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(g_FmkCan_ModState_e != STATE_CYCLIC_OPE)
    {
        Ret_e = RC_WARNING_BUSY;
    }
    if(Ret_e == RC_OK)
    {
        //------------------- Copy Bsp TxItem in BspTxItem-------------------//
        Ret_e = s_FMKFDCAN_CopyBspTxItem(&f_TxItemCfg_s, &SoTxitem_s.bspTxItem_s);
        if(Ret_e == RC_OK)
        {
            fifoLevel_u32 = HAL_FDCAN_GetTxFifoFreeLevel(&g_NodeInfo_as[f_Node_e].bspNode_s);

            // if the hardware buffer is not full, add directly the message 
            // else put it in the software buffer only if there <= DLC_8 Bytes
            // and activate IT notification to send msg from HAL Callback
            if(fifoLevel_u32 > (t_uint32)0)
            {
                bspRet_e = HAL_FDCAN_AddMessageToTxFifoQ(&g_NodeInfo_as[f_Node_e].bspNode_s,
                                                        &SoTxitem_s.bspTxItem_s,
                                                        f_TxItemCfg_s.CanMsg_s.data_pu8);
                if(bspRet_e != HAL_OK)
                {
                    Ret_e = RC_WARNING_WRONG_RESULT;
                }
            }
            //-----if hardware fifo Tx full, put in in software fifo if dlc <= 8-----//
            else if (f_TxItemCfg_s.CanMsg_s.Dlc_e <= FMKFDCAN_DLC_8)
            {
                //----------copy structure into SoTxitem_s----------//
                //-----Copy data Into Software Buffer Structre-----//
                for(idxData_u8 = (t_uint8)0 ; idxData_u8 < FMKFDCAN_DLC_8 ; idxData_u8++)
                {
                    SoTxitem_s.data_ua8[idxData_u8] = f_TxItemCfg_s.CanMsg_s.data_pu8[idxData_u8];
                }
                //----------Write Into Software Queue----------//
                Ret_e = LIBQUEUE_WriteElement(&g_TxSoftQueue_as[f_Node_e], &SoTxitem_s);
                //----------update flag Tx Item to send/----------//
                if(Ret_e == RC_OK)
                {
                    g_NodeInfo_as[f_Node_e].Flag_s.TxQueuePending_b = True;
                    //----------Activate Notification /----------//
                    Ret_e = s_FMKFDCAN_SetHwBspCallbackStatus(f_Node_e, 
                                                            FMKFDCAN_BSP_TX_CB_BUFFER_COMPLETE,
                                                            FMKFDCAN_CALLBACK_STATUS_ACTIVATE);
                }
            }
            else 
            {
                Ret_e = RC_WARNING_BUSY;
            }
        }
        
    }
    return Ret_e;
}

/*********************************
* FMKFDCAN_GetRxItem
*********************************/
t_eReturnCode FMKFDCAN_GetRxItem(t_eFMKFDCAN_NodeList f_Node_e, t_sFMKFDCAN_RxItemEvent *f_RxItem_ps)
{
    t_eReturnCode Ret_e = RC_OK;
    //t_uint8 LLI_u8;
    //t_sFMKFDCAN_RxItemBuffer *itemBuff_ps;
    t_uint8 fifoLevel_u8 = (t_uint8)0;
    t_uint8 idxData_u8 = (t_uint8)0;
    t_eFMKFDCAN_DataLength dataLenght_e;
    t_uint32 FIFO_Id_u32 = (t_uint32)0;
    FDCAN_RxHeaderTypeDef bspRxITem_s;
    t_uint8 data_pu8[8];
    HAL_StatusTypeDef bspRet_e = HAL_OK;

    if(f_RxItem_ps == (t_sFMKFDCAN_RxItemEvent * )NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(g_FmkCan_ModState_e != STATE_CYCLIC_OPE)
    {
        Ret_e = RC_WARNING_BUSY;
    }
    if((g_NodeInfo_as[f_Node_e].isNodeConfigured_b == (t_bool)False)
    || g_NodeInfo_as[f_Node_e].isNodeActive_b == (t_bool)False)
    {
        Ret_e = RC_ERROR_WRONG_STATE;
    }
    if(Ret_e == RC_OK)
    {
        #warning('FMKFDCAN_GetRxItem No Callback Not Implemented')
        fifoLevel_u8 = HAL_FDCAN_GetRxFifoFillLevel(&g_NodeInfo_as[f_Node_e].bspNode_s, FDCAN_RX_FIFO0);
        FIFO_Id_u32 = FDCAN_RX_FIFO0;
        if(fifoLevel_u8 == 0)
        {
            fifoLevel_u8 = HAL_FDCAN_GetRxFifoFillLevel(&g_NodeInfo_as[f_Node_e].bspNode_s, FDCAN_RX_FIFO1);
            FIFO_Id_u32 = FDCAN_RX_FIFO1;
        }
        if(fifoLevel_u8 != 0)
        {
            bspRet_e = HAL_FDCAN_GetRxMessage(&g_NodeInfo_as[f_Node_e].bspNode_s, 
                                    FIFO_Id_u32, 
                                    &bspRxITem_s,
                                    data_pu8);

            if(bspRet_e == HAL_OK)
            {
                //---------- Get the number of data ----------//
                Ret_e = s_FMKFDCAN_GetDlcFromBsp(bspRxITem_s.DataLength, &dataLenght_e);
            
                if(Ret_e == RC_OK)
                {
                    for(idxData_u8 = (t_uint8)0; idxData_u8 < (t_uint8)dataLenght_e ; idxData_u8++)
                    {
                        f_RxItem_ps->CanMsg_s.data_pu8[idxData_u8] = data_pu8[idxData_u8];
                    }
                    f_RxItem_ps->CanMsg_s.Direction_e = FMKFDCAN_NODE_DIRECTION_RX;
                    f_RxItem_ps->CanMsg_s.Dlc_e = dataLenght_e;
                    f_RxItem_ps->ItemId_s.Identifier_u32 = bspRxITem_s.Identifier;

                    if (bspRxITem_s.IdType == FDCAN_STANDARD_ID)
                    {
                        f_RxItem_ps->ItemId_s.IdType_e = FMKFDCAN_IDTYPE_STANDARD;
                    }
                    else 
                    {
                        f_RxItem_ps->ItemId_s.IdType_e = FMKFDCAN_IDTYPE_EXTENDED;
                    }
                    
                }
            }
            else 
            {
                Ret_e = RC_WARNING_WRONG_RESULT;
            }
        }
        if(Ret_e != RC_OK)
        {
            //---------- Get Default Data values ----------//

            f_RxItem_ps->CanMsg_s.data_pu8 = (t_uint8 *)NULL;
            f_RxItem_ps->CanMsg_s.Direction_e = FMKFDCAN_NODE_DIRECTION_RX;
            f_RxItem_ps->CanMsg_s.Dlc_e = 0;
            f_RxItem_ps->ItemId_s.Identifier_u32 = 0;
        }
    }
    return Ret_e;

}

/***********************
* FMKFDCAN_PRIVATE_GetHandleTypeDef
************************/
FDCAN_HandleTypeDef * FMKFDCAN_PRIVATE_GetHandleTypeDef(t_eFMKFDCAN_NodeList f_Node_e)
{
    if(g_NodeInfo_as[f_Node_e].isNodeConfigured_b == (t_bool)False)
    {
        ASSERT((t_uint16)0);
    }
    return (FDCAN_HandleTypeDef *)(&g_NodeInfo_as[f_Node_e].bspNode_s);
}
//********************************************************************************
//                      Local functions - Implementation
//********************************************************************************
/***********************
* s_FMKFDCAN_InitDriver
************************/
static t_eReturnCode s_FMKFDCAN_InitDriver(t_eFMKFDCAN_NodeList f_Node_e, t_sFMKFDCAN_DrvNodeCfg f_NodeCfg_s)
{
    t_eReturnCode Ret_e = RC_OK;
    HAL_StatusTypeDef bspRet_e = HAL_OK;

    if(f_Node_e >= FMKFDCAN_NODE_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(Ret_e == RC_OK)
    {
        //----------Configure Pin Init----------//
        if(Ret_e == RC_OK)
        {
            Ret_e = FMKIO_Set_ComCanCfg((t_eFMKIO_ComSigCan)f_Node_e);
        }
        //----------Copy Bsp Init from Config/----------//
        if(Ret_e == RC_OK)
        {
            Ret_e = s_FMKFDCAN_SetBspNodeInit(&g_NodeInfo_as[f_Node_e].bspNode_s, f_NodeCfg_s);
        }
        //----------Set hardware clock RCC----------//
        if(Ret_e == RC_OK)
        {
            Ret_e = FMKCPU_Set_HwClock(g_NodeInfo_as[f_Node_e].c_Clock_e, FMKCPU_CLOCKPORT_OPE_ENABLE);
        }
        //----------set enable the Interruption----------//
        if(Ret_e == RC_OK)
        {
            Ret_e = FMKCPU_Set_NVICState(g_NodeInfo_as[f_Node_e].c_IrqnLine1_e, FMKCPU_NVIC_OPE_ENABLE);
            if(Ret_e == RC_OK)
            {
                Ret_e = FMKCPU_Set_NVICState(g_NodeInfo_as[f_Node_e].c_IrqnLine2_e, FMKCPU_NVIC_OPE_ENABLE);
            }
        }
        //----------Call Bsp Init for FDCAN----------//
        if(Ret_e == RC_OK)
        {
            bspRet_e = HAL_FDCAN_Init(&g_NodeInfo_as[f_Node_e].bspNode_s);
        }
        //----------Set Hardware FIFO mode for FDCAN----------//
        if(bspRet_e == HAL_OK)
        {
            Ret_e = s_FMKFDCAN_SetHwFifoOpeMode(f_Node_e, f_NodeCfg_s.FifoMode_e);
        }
    }
    if(bspRet_e != HAL_OK)
    {
        Ret_e = RC_ERROR_WRONG_RESULT;
    }
    else 
    {
        g_NodeInfo_as[f_Node_e].isNodeConfigured_b = (t_bool)True;
    }
    
    return Ret_e;
}

/*********************************
* s_FMKFDCAN_ConfigurationState
*********************************/
static t_eReturnCode s_FMKFDCAN_ConfigurationState(void)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint8 idxNode_u8 = (t_uint8)0;
    t_sFMKFDCAN_DrvNodeCfg nodeCfg_s;
    t_uint8 idxBspNodeCfg_u8;
    for (idxNode_u8 = (t_uint8)0 ; (idxNode_u8 < FMKFDCAN_NODE_NB) && (Ret_e == RC_OK) ; idxNode_u8++)
    { 
        //-----------------Init driver if Node is used ----------------//
        if(c_FmkCan_IsNodeActive[idxNode_u8] == (t_bool)True)
        {
            idxBspNodeCfg_u8 = c_FmkCan_NodeCfg_ae[idxNode_u8];

            //-----------------Copy FdCan Configuration ----------------//
            nodeCfg_s.clockDivider_e = c_FmkCan_BspNodeCfgList_as[idxBspNodeCfg_u8].clockDivider_e;
            nodeCfg_s.DataBaudrate_e = c_FmkCan_BspNodeCfgList_as[idxBspNodeCfg_u8].DataBaudrate_e;
            nodeCfg_s.FifoMode_e = c_FmkCan_BspNodeCfgList_as[idxBspNodeCfg_u8].FifoMode_e;
            nodeCfg_s.FrameBaudrate_e = c_FmkCan_BspNodeCfgList_as[idxBspNodeCfg_u8].FrameBaudrate_e;
            nodeCfg_s.ProtocolUse_e = c_FmkCan_BspNodeCfgList_as[idxBspNodeCfg_u8].ProtocolUse_e;
            nodeCfg_s.QueueType_e = c_FmkCan_BspNodeCfgList_as[idxBspNodeCfg_u8].QueueType_e;
            
            //-----------------Call Init Driver Managment----------------//
            Ret_e = s_FMKFDCAN_InitDriver((t_eFMKFDCAN_NodeList)idxNode_u8, nodeCfg_s);
        }
    }

    return Ret_e;
}
/*********************************
* s_FMKFDCAN_PreOperational
*********************************/
static t_eReturnCode s_FMKFDCAN_PreOperational(void)
{
    t_eReturnCode Ret_e = RC_OK;
    HAL_StatusTypeDef bspRet_e = HAL_OK;
    t_uint8 idxNode_u8;
    //----------Activate Notification that are always ON----------//
    for(idxNode_u8 = (t_uint8)0; idxNode_u8 < FMKFDCAN_NODE_NB ; idxNode_u8++)
    {
        if(g_NodeInfo_as[idxNode_u8].isNodeConfigured_b ==(t_bool)True)
        {
                  
            //---------- CallBack for every FIFO_0 Event----------//
            Ret_e = s_FMKFDCAN_SetHwBspCallbackStatus(idxNode_u8,
                                                    FMKFDCAN_BSP_RX_CB_FIFO_0,
                                                    FMKFDCAN_CALLBACK_STATUS_ACTIVATE);
            if(Ret_e == RC_OK)
            {
                //---------- CallBack for every FIFO_1 Event----------//
                Ret_e = s_FMKFDCAN_SetHwBspCallbackStatus(idxNode_u8,
                                                        FMKFDCAN_BSP_RX_CB_FIFO_1,
                                                        FMKFDCAN_CALLBACK_STATUS_ACTIVATE);

            }
            if(Ret_e == RC_OK)
            {
                //---------- CallBack for every Tx Send A Msg Event----------//
                Ret_e = s_FMKFDCAN_SetHwBspCallbackStatus(idxNode_u8,
                                                        FMKFDCAN_BSP_TX_CB_BUFFER_COMPLETE,
                                                        FMKFDCAN_CALLBACK_STATUS_ACTIVATE);
            }
            if(Ret_e == RC_OK)
            {
                //---------- CallBack for Protocol Error----------//
                Ret_e = s_FMKFDCAN_SetHwBspCallbackStatus(idxNode_u8,
                                                        FMKFDCAN_BSP_CB_PROTOCOL_ERR,
                                                        FMKFDCAN_CALLBACK_STATUS_ACTIVATE);
            }
            if(Ret_e == RC_OK)
            {
                Ret_e = s_FMKFDCAN_SetNodeFilters();
            }
            if(Ret_e == RC_OK)
            {
                bspRet_e = HAL_FDCAN_Start(&g_NodeInfo_as[idxNode_u8].bspNode_s);
            }
            if(bspRet_e != HAL_OK)
            {
                Ret_e = RC_ERROR_WRONG_RESULT;
            }
            if(Ret_e == RC_OK)
            {
                //---------- Update Flag----------//
                g_NodeInfo_as[idxNode_u8].isNodeActive_b = (t_bool)True;
            }
        }
    }
    
    return Ret_e;
}

/*********************************
* s_FMKFDCAN_Operational
*********************************/
static t_eReturnCode s_FMKFDCAN_Operational(void)
{
    t_eReturnCode Ret_e =  RC_OK;
    t_uint8 idxNode_u8;
    t_sFMKFDCAN_NodeInfo *nodeInfo_ps;

    for(idxNode_u8 = (t_uint8)0 ; idxNode_u8 < FMKFDCAN_NODE_NB ; idxNode_u8++)
    {
        if(g_NodeInfo_as[idxNode_u8].isNodeConfigured_b == (t_bool)True)
        {
            nodeInfo_ps = &g_NodeInfo_as[idxNode_u8];
            // check flags for this Node
            if(nodeInfo_ps->Flag_s.ErrorDetected_b == (t_bool)True)
            {
                // PerformDiagnosticEvent
            }
            if(nodeInfo_ps->Flag_s.RxQueuePending_b == (t_bool)True)
            {
                // Manage Calling some Callback until Qeuue is off or MAX COUNTOR attien
            }
            if(nodeInfo_ps->Flag_s.TxQueuePending_b == (t_bool)True)
            {
                // see if flag are already been reach 
            }
        }
    }

    return Ret_e;
}
/*****************************
* s_FMKFDCAN_BspTxEventCb
******************************/
static void s_FMKFDCAN_BspTxEventCb(FDCAN_HandleTypeDef *f_bspInfo_ps, 
                                              t_uint32 f_EvntCbInfo_u32, 
                                              t_eFMKFDCAN_BspCallbackList f_bspTxCallback_e)
{
    t_eReturnCode Ret_e = RC_OK;
    HAL_StatusTypeDef bspRet_e = HAL_OK;
    t_uint8 TxQueueSize_u8;
    t_uint8 idxNode_u8;
    t_sFMKFDCAN_TxItemBuffer SoftTxItem_s;
    t_uint8 msgProcessed_u8 = 0;

    if(f_bspInfo_ps == (FDCAN_HandleTypeDef *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        for(idxNode_u8 = (t_uint8)0 ; idxNode_u8 < FMKFDCAN_NODE_NB ; idxNode_u8++)
        {
            if(&g_NodeInfo_as[idxNode_u8].bspNode_s == f_bspInfo_ps)
            {
                break;
            }
        }
        if(idxNode_u8 != FMKFDCAN_NODE_NB)
        {
            switch(f_bspTxCallback_e)
            {
                case FMKFDCAN_BSP_TX_CB_EVENT:
                {
                    break;
                }
                case FMKFDCAN_BSP_TX_CB_BUFFER_COMPLETE:
                case FMKFDCAN_BSP_TX_CB_FIFO_EMPTY:
                {
                    //---------Check if there are messages pending in the queue---------//
                    if (g_NodeInfo_as[idxNode_u8].Flag_s.TxQueuePending_b == (t_bool)True)
                    {
                        LIBQUEUE_GetActualSize(&g_TxSoftQueue_as[idxNode_u8], &TxQueueSize_u8);

                        //---------Process messages from the queue, respecting the maximum limit per interrupt---------//
                        
                        while ((TxQueueSize_u8 > 0) 
                            && (msgProcessed_u8 < (t_uint8)FMKFDCAN_MAX_TX_ITEM_SEND_PER_IT))
                        {
                            //---------Read an element from the queue---------//
                            Ret_e = LIBQUEUE_ReadElement(&g_TxSoftQueue_as[idxNode_u8], &SoftTxItem_s);
                            if (Ret_e != RC_OK)
                            {
                                //---------Exit loop on read failure---------//
                                break;
                            }

                            //---------Attempt to send the message to the hardware buffer---------//
                            bspRet_e = HAL_FDCAN_AddMessageToTxFifoQ(&g_NodeInfo_as[idxNode_u8].bspNode_s,
                                                                    &SoftTxItem_s.bspTxItem_s,
                                                                    SoftTxItem_s.data_ua8);
                            if (bspRet_e != HAL_OK)
                            {
                                Ret_e = RC_ERROR_WRONG_RESULT;
                                break;
                            }

                            //---------Increment processed message count---------//
                            msgProcessed_u8++;

                            //---------Update the queue size---------//
                            LIBQUEUE_GetActualSize(&g_TxSoftQueue_as[idxNode_u8], &TxQueueSize_u8);
                        }

                        //---------If the queue is empty, deactivate the callback and update the flag---------//
                        if (TxQueueSize_u8 == 0)
                        {
                            Ret_e = s_FMKFDCAN_SetHwBspCallbackStatus(idxNode_u8, 
                                                                    FMKFDCAN_BSP_TX_CB_BUFFER_COMPLETE,
                                                                    FMKFDCAN_CALLBACK_STATUS_DEACTIVATE);
                                                                    
                            g_NodeInfo_as[idxNode_u8].Flag_s.TxQueuePending_b = (t_bool)False;
                        }
                    }
                        
                        
                        break;
                }
                case FMKFDCAN_BSP_TX_CB_BUFFER_ABORT:
                    // Error Management
                    break;
                //---------Not Deal With This Function---------//
                case FMKFDCAN_BSP_RX_CB_FIFO_0:
                case FMKFDCAN_BSP_RX_CB_FIFO_1:
                case FMKFDCAN_BSP_CB_PROTOCOL_ERR:
                default:
                    break;
            }
        }
    }
}

/*****************************
* s_FMKFDCAN_BspRxEventCb
******************************/
static void s_FMKFDCAN_BspRxEventCb(FDCAN_HandleTypeDef *f_bspInfo_ps, 
                                    t_uint32 f_EvntCbInfo_u32, 
                                    t_eFMKFDCAN_BspCallbackList f_bspRxCallback_e)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint8 idxNode_u8;
    t_uint8 LLI_u8;
    t_uint8 bspData_ua8[FMKFDCAN_64_BYTES];
    t_uint8 idxUserRegister_u8 = 0;
    /**< Structure that contains Bsp Information */
    t_sFMKFDCAN_RxItemBuffer RxItemBuffer_s;
    t_eFMKFDCAN_HwRxFifoList RxFifo_e = FMKFDCAN_HW_RX_FIFO_NB;
    t_sFMKFDCAN_RxItemEvent RxItemEvent_s;

    // First we try to found the Node that causes the interruption with f_bspInfo_ps
    // Then we try to found which FIFO xauses the interruption with f_bspRxCallback_e
    // Then if the flag MSG_RCV causes IT -> deal with it by calling callback or saved msg into software Rx Queue
    // If the flag FIFO_FULL causes IT -> Put X msg into Queue and Raise a flag to deal with them later
    if(f_bspInfo_ps == (FDCAN_HandleTypeDef *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        for(idxNode_u8 = (t_uint8)0 ; idxNode_u8 < FMKFDCAN_NODE_NB ; idxNode_u8++)
        {
            if(&g_NodeInfo_as[idxNode_u8].bspNode_s == f_bspInfo_ps)
            {
                break;
            }
        }
        if(idxNode_u8 != FMKFDCAN_NODE_NB)
        {
            switch(f_bspRxCallback_e)
            {
                case FMKFDCAN_BSP_RX_CB_FIFO_0:
                    RxFifo_e = FMKFDCAN_HW_RX_FIFO_0;
                   
                    break;
                case FMKFDCAN_BSP_RX_CB_FIFO_1:
                {   
                    RxFifo_e = FMKFDCAN_HW_RX_FIFO_1;
                    break;
                }
                //---------Not Deal With This Function---------//
                case FMKFDCAN_BSP_CB_PROTOCOL_ERR:
                case FMKFDCAN_BSP_TX_CB_EVENT:
                case FMKFDCAN_BSP_TX_CB_BUFFER_COMPLETE:
                case FMKFDCAN_BSP_TX_CB_BUFFER_ABORT:
                case FMKFDCAN_BSP_TX_CB_FIFO_EMPTY:
                case FMKFDCAN_BSP_CB_NB:
                default: 
                    break;
            }
            if(RxFifo_e != FMKFDCAN_HW_RX_FIFO_NB)
            {
                switch(f_EvntCbInfo_u32)
                {
                    case FDCAN_IT_RX_FIFO0_FULL:
                    case FDCAN_IT_RX_FIFO1_FULL:
                    {
                        for(LLI_u8 = (t_uint8)0; (LLI_u8 < FMKFDCAN_MAX_ITEM_ON_QUEUE_PER_IT) 
                            && (Ret_e == RC_OK) ; LLI_u8++)
                        {
                            //---------Get a msg from Hardware Buffer---------//
                            Ret_e = s_FMKFDCAN_RetrieveRxItem(idxNode_u8, 
                                                            RxFifo_e,
                                                            &RxItemBuffer_s.bspRxItem_s,
                                                            bspData_ua8);
                            //---------Copy into buffer only if DataLenght <= 8---------//
                            if((Ret_e == RC_OK )
                            &&( RxItemBuffer_s.bspRxItem_s.DataLength <= FDCAN_DLC_BYTES_8))
                            {
                                //--------- Copy Into Data Item Software Buffer---------//
                                Ret_e = SafeMem_memcpy(RxItemBuffer_s.data_ua8, bspData_ua8, (t_uint16)FMKFDCAN_DLC_8);
                                if(Ret_e == RC_OK)
                                {
                                    Ret_e = LIBQUEUE_WriteElement(&g_RxSoftQueue_as[idxNode_u8], &RxItemBuffer_s);
                                }
                            }
                        }
                        //---------Flag RxFrame Pending ---------//
                        g_NodeInfo_as[idxNode_u8].Flag_s.RxQueuePending_b = (t_bool)True;
                        break;
                    }
                    
                    case FDCAN_IT_RX_FIFO0_NEW_MESSAGE:
                    case FDCAN_IT_RX_FIFO1_NEW_MESSAGE:
                    {
                        Ret_e = s_FMKFDCAN_RetrieveRxItem(idxNode_u8, 
                                                            RxFifo_e,
                                                            &RxItemBuffer_s.bspRxItem_s,
                                                            RxItemBuffer_s.data_ua8);
                        if(Ret_e == RC_OK)
                        {
                            Ret_e = s_FMKFDCAN_CopyBspRxItem(&RxItemBuffer_s.bspRxItem_s, 
                                                            &RxItemEvent_s,
                                                            RxItemBuffer_s.data_ua8);
                        }
                        if(Ret_e == RC_OK)
                        {
                            //---------Find in Registration the Registerindex of the ItemId---------//
                            Ret_e = s_FMKFDCAN_GetUserRegisterIndex(RxItemBuffer_s.bspRxItem_s.Identifier,
                                                                    g_UserRegisterEvnt_as[idxNode_u8],
                                                                    g_CtrUserRegisterEvnt_ua8[idxNode_u8],
                                                                    &idxUserRegister_u8);
                        }
                        if(Ret_e == RC_OK)
                        {
                            //--------- Call Callback User with RxItem---------//
                            g_UserRegisterEvnt_as[idxNode_u8][idxUserRegister_u8].rcvItem_cb(idxNode_u8, RxItemEvent_s, 0);
                            #warning('No diagnostic on NodeStatus')
                        }
                        break;
                    }
                    case FDCAN_IT_RX_FIFO0_MESSAGE_LOST:
                    case FDCAN_IT_RX_FIFO1_MESSAGE_LOST:
                    {
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }
}

/*****************************
* s_FMKFDCAN_RetrieveRxItem
******************************/
static t_eReturnCode s_FMKFDCAN_RetrieveRxItem(t_eFMKFDCAN_NodeList f_Node_e,
                                                  t_eFMKFDCAN_HwRxFifoList f_RxFifo_e,
                                                  FDCAN_RxHeaderTypeDef * f_bspRxItem_ps,
                                                  t_uint8 * f_bspData_pu8)
{
    t_eReturnCode Ret_e = RC_OK;
    HAL_StatusTypeDef bspRet_e = HAL_OK;
    t_uint32 bspRxFifo_u32;

    Ret_e = s_FMKFDCAN_GetBspRxFifoId(f_RxFifo_e, &bspRxFifo_u32);
    if(Ret_e == RC_OK)
    {
        //--------- Get Rx Msg From Hardware Fifo---------//
        bspRet_e = HAL_FDCAN_GetRxMessage(&g_NodeInfo_as[f_Node_e].bspNode_s, 
                                bspRxFifo_u32,
                                f_bspRxItem_ps,
                                f_bspData_pu8);

        if(bspRet_e != HAL_OK)
        {
            Ret_e = RC_ERROR_WRONG_RESULT;
        }
    }
    return Ret_e;
}
/*****************************
* s_FMKFDCAN_SetBspNodeInit
******************************/
static t_eReturnCode s_FMKFDCAN_SetBspNodeInit(FDCAN_HandleTypeDef *f_bspInit_ps, t_sFMKFDCAN_DrvNodeCfg f_NodeCfg_s)
{
    t_eReturnCode Ret_e = RC_OK;
    t_eFMKFDCAN_Baudrate dataBaudrate_e;
    t_eFMKFDCAN_Baudrate frameBaudrate_e;
    t_uint32 bspTxQueueType_u32 = 0;
    t_uint32 bspClkDivider_u32 = 0;
    t_uint32 nominalPrescaler_u32 = 0;
    t_uint32 dataPrescaler_u32 = 0;
    if(f_bspInit_ps == (FDCAN_HandleTypeDef *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        //-------------------copy information -------------------//
        dataBaudrate_e = f_NodeCfg_s.DataBaudrate_e;
        frameBaudrate_e = f_NodeCfg_s.FrameBaudrate_e;
        //-------------------Get the Type-------------------//
        Ret_e = s_FMKFDCAN_GetBspQueueType(f_NodeCfg_s.QueueType_e, &bspTxQueueType_u32);
        if(Ret_e == RC_OK)
        {
            Ret_e = s_FMKFDCAN_GetClockKernelDivider(f_NodeCfg_s.clockDivider_e, &bspClkDivider_u32);
        }
        if(Ret_e == RC_OK)
        {
            //-------------------Global configuration Init -------------------//
            f_bspInit_ps->Init.ClockDivider = (t_uint32)bspClkDivider_u32;
            f_bspInit_ps->Init.Mode = FMKFDCAN_NODE_MODE;
            f_bspInit_ps->Init.AutoRetransmission = ENABLE;
            f_bspInit_ps->Init.TransmitPause = DISABLE;
            f_bspInit_ps->Init.ProtocolException = ENABLE;
            //--------------One per FIFO to allowed every ID----------//
            f_bspInit_ps->Init.StdFiltersNbr = (t_uint32)2; 
            f_bspInit_ps->Init.ExtFiltersNbr = (t_uint32)0;

            f_bspInit_ps->Init.TxFifoQueueMode = bspTxQueueType_u32;

            //-------------------Init for nominal baudrate-------------------//
            /* Information 
            *   Here in c_FmkCan_BspBaudrateCfg_as, the configuration has been made 
            *   for Clock Cfg (120Mhz), if user wants to divided this clock, in order to 
            *   obtain the right baudrate, we have to multiply the the value of the enum.\n
            *   In consequence no matter the clock divider value, the baudrate will always be 
            *   good 
            *   
            *   For Instance if the divider clock is Divided by FDCAN_CLOCK_DIV10, we multiply 
            *   the Nominal and Data precaler by FMKFDCAN_CLOCK_KERNEL_DIV10 (10)
            * 
            *   For record 
            *                                         Fclock (FMKFDCAN_SRC_CLOCK) (/ x)
            *  Baudrate =                        -----------------------------------------
            *                                   (Prescaler (* x)) * (SyncSeg + TimSeg1 + TimSeg2)
            */
            nominalPrescaler_u32 = (t_uint32)(c_FmkCan_BspBaudrateCfg_as[frameBaudrate_e].prescaler_u16 *
                                        f_NodeCfg_s.clockDivider_e);

            //-------------------copy information -------------------//            
            f_bspInit_ps->Init.NominalPrescaler = nominalPrescaler_u32;
            f_bspInit_ps->Init.NominalSyncJumpWidth = c_FmkCan_BspBaudrateCfg_as[frameBaudrate_e].syncSeg_u8;
            f_bspInit_ps->Init.NominalTimeSeg1 = c_FmkCan_BspBaudrateCfg_as[frameBaudrate_e].timeSeg1_u8;
            f_bspInit_ps->Init.NominalTimeSeg2 = c_FmkCan_BspBaudrateCfg_as[frameBaudrate_e].timeSeg2_u8;

            
            //-------------------depending on complete configuration-------------------// 
            switch(f_NodeCfg_s.ProtocolUse_e)
            {
                case FMKFDCAN_PROTOCOL_CAN2_0B:
                    f_bspInit_ps->Init.FrameFormat = FDCAN_FRAME_CLASSIC;
                    f_bspInit_ps->Init.DataPrescaler     = (t_uint32)0;
                    f_bspInit_ps->Init.DataSyncJumpWidth = (t_uint32)0;
                    f_bspInit_ps->Init.DataTimeSeg1      = (t_uint32)0;
                    f_bspInit_ps->Init.DataTimeSeg2      = (t_uint32)0;
                    break;

                case FMKFDCAN_PROTOCOL_FDCAN_BRS:
                    f_bspInit_ps->Init.FrameFormat = FDCAN_FRAME_FD_BRS;
                    //-------------------Init for Data Baudrate-------------//
                    dataPrescaler_u32 =  (t_uint32)(c_FmkCan_BspBaudrateCfg_as[dataBaudrate_e].prescaler_u16 * 
                                            f_NodeCfg_s.clockDivider_e);
                    f_bspInit_ps->Init.DataPrescaler = dataPrescaler_u32;
                    f_bspInit_ps->Init.DataSyncJumpWidth = c_FmkCan_BspBaudrateCfg_as[dataBaudrate_e].syncSeg_u8;
                    f_bspInit_ps->Init.DataTimeSeg1 = c_FmkCan_BspBaudrateCfg_as[dataBaudrate_e].timeSeg1_u8;
                    f_bspInit_ps->Init.DataTimeSeg2 = c_FmkCan_BspBaudrateCfg_as[dataBaudrate_e].timeSeg2_u8;
                    break;

                case  FMKFDCAN_PROTOCOL_FDCAN_NO_BRS:
                    f_bspInit_ps->Init.FrameFormat = FDCAN_FRAME_FD_NO_BRS;
                    f_bspInit_ps->Init.DataPrescaler     = (t_uint32)0;
                    f_bspInit_ps->Init.DataSyncJumpWidth = (t_uint32)0;
                    f_bspInit_ps->Init.DataTimeSeg1      = (t_uint32)0;
                    f_bspInit_ps->Init.DataTimeSeg2      = (t_uint32)0;
                    break;

                default:
                    Ret_e = RC_ERROR_NOT_SUPPORTED;
            }
        }
    }
    return Ret_e;
}

/*********************************
* s_FMKFDCAN_CopyBspRxItem
*********************************/
static t_eReturnCode s_FMKFDCAN_CopyBspRxItem(FDCAN_RxHeaderTypeDef *f_bspRxItem_ps,
                                              t_sFMKFDCAN_RxItemEvent * f_RxItem_ps,
                                              t_uint8 * f_bspData_pu8)
{
    t_eReturnCode Ret_e = RC_OK;
    t_eFMKFDCAN_DataLength Dlc_e;

    if(f_RxItem_ps == (t_sFMKFDCAN_RxItemEvent *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        
        //-------------------assemble the User Message-------------------//
        f_RxItem_ps->ItemId_s.Identifier_u32 = f_bspRxItem_ps->Identifier;
        f_RxItem_ps->timeStamp_32 = f_bspRxItem_ps->RxTimestamp;
        //------------------- If the message is Remote, there isn't data-------------------//
        if(f_bspRxItem_ps->RxFrameType == FDCAN_REMOTE_FRAME)
        {
            f_RxItem_ps->ItemId_s.FramePurpose_e = FMKFDCAN_FRAME_PURPOSE_REMOTE;
            f_RxItem_ps->CanMsg_s.data_pu8 = NULL;
            f_RxItem_ps->CanMsg_s.Dlc_e = FMKFDCAN_DLC_0;
            f_RxItem_ps->CanMsg_s.Direction_e = FMKFDCAN_NODE_DIRECTION_RX;
        }
        else
        {//-------------------else get Bspdlc -------------------//
            Ret_e = s_FMKFDCAN_GetDlcFromBsp(f_bspRxItem_ps->DataLength, &Dlc_e);
            if(Ret_e == RC_OK)
            {
                f_RxItem_ps->ItemId_s.FramePurpose_e = FMKFDCAN_FRAME_PURPOSE_DATA;
                f_RxItem_ps->CanMsg_s.data_pu8 = f_bspData_pu8;
                f_RxItem_ps->CanMsg_s.Dlc_e = Dlc_e;
                f_RxItem_ps->CanMsg_s.Direction_e = FMKFDCAN_NODE_DIRECTION_RX;
            }  
        }
        //------------------- Configure Identifier Type -> Extended or Standard-------------------//
        if(f_bspRxItem_ps->IdType == FDCAN_EXTENDED_ID)
        {
            f_RxItem_ps->ItemId_s.IdType_e = FMKFDCAN_IDTYPE_EXTENDED;
        }
        else 
        {
            f_RxItem_ps->ItemId_s.IdType_e = FMKFDCAN_IDTYPE_STANDARD;
        }
    }

    return Ret_e;
}

/*********************************
* s_FMKFDCAN_BspErrorEventCb
*********************************/
static void s_FMKFDCAN_BspErrorEventCb(FDCAN_HandleTypeDef *f_bspInfo_ps, 
                                              t_uint32 f_EvntCbInfo_u32)

{
    t_eReturnCode Ret_e = RC_OK;
    t_uint8 idxNode_u8 = 0;

    if(f_bspInfo_ps == (FDCAN_HandleTypeDef *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        for(idxNode_u8 = (t_uint8)0 ; idxNode_u8 < FMKFDCAN_NODE_NB ; idxNode_u8++)
        {
            if(&g_NodeInfo_as[idxNode_u8].bspNode_s == f_bspInfo_ps)
            {
                break;
            }
        }
        if(idxNode_u8 != FMKFDCAN_NODE_NB)
        {
            // update flag 
            g_NodeInfo_as[idxNode_u8].Flag_s.ErrorDetected_b = (t_bool)True;
            switch(f_EvntCbInfo_u32)
            {
                case HAL_FDCAN_ERROR_NONE:
                    Ret_e = RC_OK;
                    break;
                case HAL_FDCAN_ERROR_NOT_STARTED:
                case HAL_FDCAN_ERROR_NOT_INITIALIZED:
                case HAL_FDCAN_ERROR_NOT_READY:
                case HAL_FDCAN_ERROR_PARAM:
                case HAL_FDCAN_ERROR_NOT_SUPPORTED:
                    g_NodeInfo_as[idxNode_u8].nodeHealth_e |= FMKFDCAN_NODE_STATE_ERR_PARAM_INIT;
                    break;
                case HAL_FDCAN_ERROR_TIMEOUT:
                    g_NodeInfo_as[idxNode_u8].nodeHealth_e |= FMKFDCAN_NODE_STATE_ERR_TIMEOUT;
                    break;
                case HAL_FDCAN_ERROR_FIFO_EMPTY:
                case HAL_FDCAN_ERROR_FIFO_FULL:
                    g_NodeInfo_as[idxNode_u8].nodeHealth_e |= FMKFDCAN_NODE_STATE_ERR_FIFO;
                    break;
                case HAL_FDCAN_ERROR_PROTOCOL_ARBT:
                case HAL_FDCAN_ERROR_PROTOCOL_DATA:
                    g_NodeInfo_as[idxNode_u8].nodeHealth_e |= FMKFDCAN_NODE_STATE_ERR_PROTOCOL;
                    break;
                case HAL_FDCAN_ERROR_LOG_OVERFLOW:
                case HAL_FDCAN_ERROR_RAM_ACCESS:
                case HAL_FDCAN_ERROR_PENDING:
                case HAL_FDCAN_ERROR_RESERVED_AREA:
                    g_NodeInfo_as[idxNode_u8].nodeHealth_e |= FMKFDCAN_NODE_STATE_ERR_MEM;
                    break;
                case HAL_FDCAN_ERROR_RAM_WDG:
                    g_NodeInfo_as[idxNode_u8].nodeHealth_e |= FMKFDCAN_NODE_STATE_ERR_WDG;
                    break;
                default:
                    Ret_e = RC_OK;
            }   
        }
    }

}
/*********************************
* s_FMKFDCAN_CopyBspTxItem
*********************************/
static t_eReturnCode s_FMKFDCAN_CopyBspTxItem(t_sFMKFDCAN_TxItemCfg *f_TxItemCfg_s, FDCAN_TxHeaderTypeDef *f_bspTxItem_ps)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint32 bspFramePurpose_u32;
    t_uint32 bspDlc_u32;
    t_uint32 bspFDFormat_u32;
    t_uint32 bspIdType_u32;
    t_uint32 bspBitRateStatus_u32;

    if (f_bspTxItem_ps == (FDCAN_TxHeaderTypeDef *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {   
        //-------------------Get all Bsp Variable from enum-------------------//
        Ret_e = s_FMKFDCAN_GetBspDlc(f_TxItemCfg_s->CanMsg_s.Dlc_e, &bspDlc_u32);
        if(Ret_e == RC_OK)
        {
            Ret_e = s_FMKFDCAN_GetBspFDFormat(f_TxItemCfg_s->frameFormat_e, &bspFDFormat_u32);
        }
        if(Ret_e == RC_OK)
        {
            Ret_e = s_FMKFDCAN_GetBspIdType(f_TxItemCfg_s->ItemId_s.IdType_e, &bspIdType_u32);
        }
        if(Ret_e == RC_OK)
        {
            Ret_e = s_FMKFDCAN_GetBspFramePurpose(f_TxItemCfg_s->ItemId_s.FramePurpose_e, &bspFramePurpose_u32);
        }
        if(Ret_e == RC_OK)
        {
            Ret_e = s_FMKFDCAN_GetBspBitRateStatus(f_TxItemCfg_s->BitRate_e, &bspBitRateStatus_u32);
        }
        if(Ret_e == RC_OK)
        {
            //------------------- Copy bsp Tx Item structure-------------------//
            f_bspTxItem_ps->Identifier          = (t_uint32)f_TxItemCfg_s->ItemId_s.Identifier_u32;
            f_bspTxItem_ps->IdType              = (t_uint32)bspIdType_u32;
            f_bspTxItem_ps->BitRateSwitch       = (t_uint32)bspBitRateStatus_u32;
            f_bspTxItem_ps->DataLength          = (t_uint32)bspDlc_u32;
            f_bspTxItem_ps->FDFormat            = (t_uint32)bspFDFormat_u32;
            f_bspTxItem_ps->TxEventFifoControl  = (t_uint32)FDCAN_NO_TX_EVENTS;
            f_bspTxItem_ps->TxFrameType         = (t_uint32)bspFramePurpose_u32;
            f_bspTxItem_ps->MessageMarker       = (t_uint32)0;
            f_bspTxItem_ps->ErrorStateIndicator = (t_uint32)0;
        }
    }

    return Ret_e;
}

/*********************************
* s_FMKFDCAN_GetUserRegisterIndex
*********************************/
static t_eReturnCode s_FMKFDCAN_GetUserRegisterIndex(t_uint32 f_Identifier_u32,
                                                     t_sFMKFDCAN_UserItemRegister * f_NodeRegister_pas,
                                                     t_uint8 f_RegistrationCtr_u8,
                                                     t_uint8 * f_idxUserRegister_pu8)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint8 idxUserRgstr_u8;

    if(f_NodeRegister_pas == (t_sFMKFDCAN_UserItemRegister *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        //-------------------Find User Information-------------------//
        // Loop until the number of registration done 
        // We enter in the condition if the mask applied to f_Identifier_u32
        // is equals to an identifier register while appliyng the same mask
        // For instance 
        //
        // f_Identifier_u32     mask_u32
        // 0b11010100       & 0b11110000 = 0b11010000
        // 0b11010011       & 0b11110000 = 0b11010000 
        // Works -> We enter in condition
        for(idxUserRgstr_u8 = (t_uint8)0; idxUserRgstr_u8 < f_RegistrationCtr_u8 ; idxUserRgstr_u8++)
        {
            if ((f_Identifier_u32 & f_NodeRegister_pas[idxUserRgstr_u8].maskId_u32) == 
                (f_NodeRegister_pas[idxUserRgstr_u8].itemId_s.Identifier_u32 & f_NodeRegister_pas[idxUserRgstr_u8].maskId_u32)
                )
            {
                break;
            }
        }
        if(idxUserRgstr_u8 == f_RegistrationCtr_u8)
        {
            Ret_e = RC_ERROR_LIMIT_REACHED;
            *f_idxUserRegister_pu8 = (t_uint8)0;
        }
        else 
        {
            *f_idxUserRegister_pu8 = idxUserRgstr_u8;
        }
    }

    return Ret_e;
}

/************************************
* s_FMKFDCAN_SetHwBspCallbackStatus
***********************************/
static t_eReturnCode s_FMKFDCAN_SetNodeFilters(void)
{
    t_eReturnCode Ret_e = RC_OK;
    FDCAN_FilterTypeDef bspFilter_s;
    t_uint8 idxNode_u8;
    HAL_StatusTypeDef bspRet_e = HAL_OK;

    #warning('Filter are for Standard ID, please change following line if Node is use in Extended Mode, Think to also changed bspInit FilerNb')
    bspFilter_s.IdType = FDCAN_STANDARD_ID;
    bspFilter_s.FilterIndex = 0;                 // Index du filtre
    bspFilter_s.FilterType = FDCAN_FILTER_RANGE;  // Type de filtre
    bspFilter_s.FilterID1 = 0x000;               // ID accepté
    bspFilter_s.FilterID2 = 0x7FF;               // Masque (toutes les trames)

    for( idxNode_u8 = (t_uint8)0 ; (idxNode_u8 < FMKFDCAN_NODE_NB) && (Ret_e == RC_OK) ; idxNode_u8++)
    {
        if(g_NodeInfo_as[idxNode_u8].isNodeConfigured_b == (t_bool)True)
        {
            bspFilter_s.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
            bspRet_e = HAL_FDCAN_ConfigFilter(&g_NodeInfo_as[idxNode_u8].bspNode_s, &bspFilter_s);
            if(bspRet_e == HAL_OK)
            {
                bspFilter_s.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
                bspRet_e = HAL_FDCAN_ConfigFilter(&g_NodeInfo_as[idxNode_u8].bspNode_s, &bspFilter_s);
            }
        }
        if(bspRet_e != HAL_OK)
        {
            Ret_e = RC_ERROR_WRONG_RESULT;
        }
    }   

    return Ret_e;
}
/************************************
* s_FMKFDCAN_SetHwBspCallbackStatus
***********************************/
static t_eReturnCode s_FMKFDCAN_SetHwBspCallbackStatus(t_eFMKFDCAN_NodeList f_Node_e ,
                                                        t_eFMKFDCAN_BspCallbackList f_bspCb_e,
                                                        t_eFMKFDCAN_BspStatusCb f_status_e)
{
    t_eReturnCode Ret_e = RC_OK;
    HAL_StatusTypeDef bspRet_e = HAL_OK;
    t_uint32 bspCbId_u32;
    t_uint32 bspFifo_u32;
    t_uint8 idxTxFifo_u8;

    if((f_bspCb_e >= FMKFDCAN_BSP_CB_NB)
    || (f_status_e >= FMKFDCAN_CALLBACK_STATUS_NB))
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(Ret_e == RC_OK)
    {
        Ret_e = s_FMKFDCAN_GetBspCallbackId(f_bspCb_e, &bspCbId_u32);
    }
    if(Ret_e == RC_OK)
    {
        if (g_BspCbMngmt_ae[f_Node_e][f_bspCb_e] != f_status_e)
        {
            
            switch(f_status_e)
            {
                case FMKFDCAN_CALLBACK_STATUS_ACTIVATE:
                {
                    // according datasheet, if IT_Tx is in the loop, 
                    // specify the TxFifo 
                    // Here we activate interruption on all Fifo
                    if((bspCbId_u32 == FDCAN_IT_TX_COMPLETE)
                    || bspCbId_u32 == FDCAN_IT_TX_ABORT_COMPLETE)
                    {
                        for(idxTxFifo_u8 = (t_uint8)0 ; idxTxFifo_u8 < FMKFDCAN_HW_TX_BUFFER_NB ; idxTxFifo_u8++)
                        {
                            Ret_e = s_FMKFDCAN_GetBspTxFifoId(idxTxFifo_u8, &bspFifo_u32);
                            if(Ret_e == RC_OK)
                            {
                                bspRet_e = HAL_FDCAN_ActivateNotification(&g_NodeInfo_as[f_Node_e].bspNode_s,
                                                                        (t_uint32)bspCbId_u32,
                                                                        (t_uint32)bspFifo_u32);
                            }
                        }  
                    }
                    else
                    {
                        bspRet_e = HAL_FDCAN_ActivateNotification(&g_NodeInfo_as[f_Node_e].bspNode_s,
                                                                        (t_uint32)bspCbId_u32,
                                                                        (t_uint32)NULL);
                    }
                    break;
                }
                case FMKFDCAN_CALLBACK_STATUS_DEACTIVATE:
                {
                    bspRet_e = HAL_FDCAN_DeactivateNotification(&g_NodeInfo_as[f_Node_e].bspNode_s,
                                                                bspCbId_u32);
                    break;
                }
                case FMKFDCAN_CALLBACK_STATUS_NB:
                default:
                    Ret_e = RC_ERROR_NOT_SUPPORTED;
                    break;
            }
            if(bspRet_e != HAL_OK)
            {
                Ret_e = RC_ERROR_WRONG_RESULT;
            }
            if(Ret_e == RC_OK)
            {
                //------------------Update Flag for this Bsp Callback------------------//
                g_BspCbMngmt_ae[f_Node_e][f_bspCb_e] = f_status_e;
            }
        }
    }

    return Ret_e;
}

/*****************************
* s_FMKFDCAN_SetHwFifoOpeMode
******************************/
static t_eReturnCode s_FMKFDCAN_SetHwFifoOpeMode(t_eFMKFDCAN_NodeList f_Node_e ,t_eFMKFDCAN_FifoOpeMode f_fifoMode_e)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint8 idxFifo_u8 = 0;
    t_uint32 bspFifo_u32;
    t_uint32 bspFifoOpeMode_u32;
    HAL_StatusTypeDef bspRet_e = HAL_OK;

    if(f_fifoMode_e >= FMKFDCAN_FIFO_OPEMODE_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(Ret_e == RC_OK)
    {
        //---------We loop for every Hardware Can FIfo and set the same Fifo Operation Mode---------//
        for (idxFifo_u8 = (t_uint8)0 ; (idxFifo_u8 < FMKFDCAN_HW_RX_FIFO_NB) && (Ret_e == RC_OK); idxFifo_u8++)    
        {
            Ret_e = s_FMKFDCAN_GetBspRxFifoId(idxFifo_u8, &bspFifo_u32);
            if(Ret_e == RC_OK)
            {
                Ret_e = s_FMKFDCAN_GetBspFifoOpeMode(f_fifoMode_e, &bspFifoOpeMode_u32);
                
            }
            if(Ret_e == RC_OK)
            {
                //---------Call Bsp Function to configure FIFO/QUEUE mode---------//
                bspRet_e = HAL_FDCAN_ConfigRxFifoOverwrite(&g_NodeInfo_as[f_Node_e].bspNode_s, 
                                                            bspFifo_u32, 
                                                            bspFifoOpeMode_u32);
            }
            if(bspRet_e != HAL_OK)
            {
                Ret_e = RC_ERROR_WRONG_RESULT;
            }
        }
    }
    return Ret_e;
}

/*****************************
* s_FMKFDCAN_GetBspFramePurpose
******************************/
static t_eReturnCode s_FMKFDCAN_GetBspFramePurpose(t_eFMKFDCAN_FramePurpose f_frameType_e, t_uint32 *f_bspFramePurpose_pu32)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_frameType_e >= FMKFDCAN_FRAME_PURPOSE_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(f_bspFramePurpose_pu32 == (t_uint32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        switch(f_frameType_e)
        {
            case FMKFDCAN_FRAME_PURPOSE_DATA:
                *f_bspFramePurpose_pu32 = FDCAN_DATA_FRAME;
                break;

            case FMKFDCAN_FRAME_PURPOSE_REMOTE:
                *f_bspFramePurpose_pu32 = FDCAN_REMOTE_FRAME;
                break;

            case FMKFDCAN_FRAME_PURPOSE_NB:
            default:
                Ret_e = RC_ERROR_NOT_SUPPORTED;
        }
    }
    
    return Ret_e;
}
/*****************************
* s_FMKFDCAN_GetBspIdType
******************************/
static t_eReturnCode s_FMKFDCAN_GetBspIdType(t_eFMKFDCAN_IdentifierType f_IdType, t_uint32 *f_bspIdType_pu32)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_IdType >= FMKFDCAN_IDTYPE_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(f_bspIdType_pu32 == (t_uint32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        switch(f_IdType)
        {
            case FMKFDCAN_IDTYPE_STANDARD:
                *f_bspIdType_pu32 = FDCAN_STANDARD_ID;
                break;

            case FMKFDCAN_IDTYPE_EXTENDED:
                *f_bspIdType_pu32 = FDCAN_EXTENDED_ID;
                break;

            case FMKFDCAN_FRAME_PURPOSE_NB:
            default:
                Ret_e = RC_ERROR_NOT_SUPPORTED;
        }
    }
    
    return Ret_e;
}

/********************************
* s_FMKFDCAN_GetBspBitRateStatus
********************************/
static t_eReturnCode s_FMKFDCAN_GetBspBitRateStatus(t_eFMKFDCAN_BitRateSwitchStatus f_BitRate_e, t_uint32 *f_BspBitRate_pu32)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_BitRate_e >= FMKFDCAN_BITRATE_SWITCH_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(f_BspBitRate_pu32 == (t_uint32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        switch(f_BitRate_e)
        {
            case FMKFDCAN_BITRATE_SWITCH_ON:
                *f_BspBitRate_pu32 = FDCAN_BRS_ON;
                break;

            case FMKFDCAN_BITRATE_SWITCH_OFF:
                *f_BspBitRate_pu32 = FDCAN_BRS_OFF;
                break;

            case FMKFDCAN_FRAME_PURPOSE_NB:
            default:
                Ret_e = RC_ERROR_NOT_SUPPORTED;
        }
    }
    
    return Ret_e;
}

/*****************************
* s_FMKFDCAN_GetBspDlc
******************************/
static t_eReturnCode s_FMKFDCAN_GetBspDlc(t_eFMKFDCAN_DataLength f_Dlc_e, t_uint32 *f_BspDlc_pu32)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_Dlc_e >= FMKFDCAN_DLC_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(f_BspDlc_pu32 == (t_uint32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        switch (f_Dlc_e)
        {
            case FMKFDCAN_DLC_0:
                *f_BspDlc_pu32 = FDCAN_DLC_BYTES_0;
                break;
            case FMKFDCAN_DLC_1:
                *f_BspDlc_pu32 = FDCAN_DLC_BYTES_1;
                break;
            case FMKFDCAN_DLC_2:
                *f_BspDlc_pu32 = FDCAN_DLC_BYTES_2;
                break;
            case FMKFDCAN_DLC_3:
                *f_BspDlc_pu32 = FDCAN_DLC_BYTES_3;
                break;
            case FMKFDCAN_DLC_4:
                *f_BspDlc_pu32 = FDCAN_DLC_BYTES_4;
                break;
            case FMKFDCAN_DLC_5:
                *f_BspDlc_pu32 = FDCAN_DLC_BYTES_5;
                break;
            case FMKFDCAN_DLC_6:
                *f_BspDlc_pu32 = FDCAN_DLC_BYTES_6;
                break;
            case FMKFDCAN_DLC_7:
                *f_BspDlc_pu32 = FDCAN_DLC_BYTES_7;
                break;
            case FMKFDCAN_DLC_8:
                *f_BspDlc_pu32 = FDCAN_DLC_BYTES_8;
                break;
            case FMKFDCAN_DLC_16:
                *f_BspDlc_pu32 = FDCAN_DLC_BYTES_16;
                break;
            case FMKFDCAN_DLC_20:
                *f_BspDlc_pu32 = FDCAN_DLC_BYTES_20;
                break;
            case FMKFDCAN_DLC_24:
                *f_BspDlc_pu32 = FDCAN_DLC_BYTES_24;
                break;
            case FMKFDCAN_DLC_32:
                *f_BspDlc_pu32 = FDCAN_DLC_BYTES_32;
                break;
            case FMKFDCAN_DLC_48:
                *f_BspDlc_pu32 = FDCAN_DLC_BYTES_48;
                break;
            case FMKFDCAN_DLC_64:
                *f_BspDlc_pu32 = FDCAN_DLC_BYTES_64;
                break;

            case FMKFDCAN_DLC_NB:
            default:
                Ret_e = RC_ERROR_NOT_SUPPORTED;
                break;
        }
    }
    
    return Ret_e;
}

/*****************************
* s_FMKFDCAN_GetDlcFromBsp
******************************/
static t_eReturnCode s_FMKFDCAN_GetDlcFromBsp(t_uint32 f_BspDlc_u32, t_eFMKFDCAN_DataLength *f_Dlc_pe)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_Dlc_pe == (t_eFMKFDCAN_DataLength *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    else
    {
        switch (f_BspDlc_u32)
        {
            case FDCAN_DLC_BYTES_0:
                *f_Dlc_pe = FMKFDCAN_DLC_0;
                break;
            case FDCAN_DLC_BYTES_1:
                *f_Dlc_pe = FMKFDCAN_DLC_1;
                break;
            case FDCAN_DLC_BYTES_2:
                *f_Dlc_pe = FMKFDCAN_DLC_2;
                break;
            case FDCAN_DLC_BYTES_3:
                *f_Dlc_pe = FMKFDCAN_DLC_3;
                break;
            case FDCAN_DLC_BYTES_4:
                *f_Dlc_pe = FMKFDCAN_DLC_4;
                break;
            case FDCAN_DLC_BYTES_5:
                *f_Dlc_pe = FMKFDCAN_DLC_5;
                break;
            case FDCAN_DLC_BYTES_6:
                *f_Dlc_pe = FMKFDCAN_DLC_6;
                break;
            case FDCAN_DLC_BYTES_7:
                *f_Dlc_pe = FMKFDCAN_DLC_7;
                break;
            case FDCAN_DLC_BYTES_8:
                *f_Dlc_pe = FMKFDCAN_DLC_8;
                break;
            case FDCAN_DLC_BYTES_16:
                *f_Dlc_pe = FMKFDCAN_DLC_16;
                break;
            case FDCAN_DLC_BYTES_20:
                *f_Dlc_pe = FMKFDCAN_DLC_20;
                break;
            case FDCAN_DLC_BYTES_24:
                *f_Dlc_pe = FMKFDCAN_DLC_24;
                break;
            case FDCAN_DLC_BYTES_32:
                *f_Dlc_pe = FMKFDCAN_DLC_32;
                break;
            case FDCAN_DLC_BYTES_48:
                *f_Dlc_pe = FMKFDCAN_DLC_48;
                break;
            case FDCAN_DLC_BYTES_64:
                *f_Dlc_pe = FMKFDCAN_DLC_64;
                break;
            default:
                Ret_e = RC_ERROR_NOT_SUPPORTED;
                break;
        }
    }
    
    return Ret_e;
}

/*****************************
* s_FMKFDCAN_GetBspCallbackId
******************************/
static t_eReturnCode s_FMKFDCAN_GetBspCallbackId(t_eFMKFDCAN_BspCallbackList f_Callback_e, t_uint32 *f_bspCallback_u32)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_Callback_e >= FMKFDCAN_BSP_CB_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(f_bspCallback_u32 == (t_uint32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        switch(f_Callback_e)
        {
            case FMKFDCAN_BSP_TX_CB_EVENT: 
                *f_bspCallback_u32 =FDCAN_IT_LIST_TX_FIFO_ERROR;
                break;
            case FMKFDCAN_BSP_TX_CB_BUFFER_COMPLETE:
                *f_bspCallback_u32 =FDCAN_IT_TX_COMPLETE;
                break;
            case FMKFDCAN_BSP_TX_CB_BUFFER_ABORT:
                *f_bspCallback_u32 =FDCAN_IT_TX_ABORT_COMPLETE;
                break;
            case FMKFDCAN_BSP_TX_CB_FIFO_EMPTY:
                *f_bspCallback_u32 =FDCAN_IT_TX_FIFO_EMPTY;
                break;
            case FMKFDCAN_BSP_RX_CB_FIFO_0:
                *f_bspCallback_u32 =FDCAN_IT_LIST_RX_FIFO0;
                break;
            case FMKFDCAN_BSP_RX_CB_FIFO_1:
                *f_bspCallback_u32 =FDCAN_IT_LIST_RX_FIFO1;
                break;
            case FMKFDCAN_BSP_CB_PROTOCOL_ERR:
                *f_bspCallback_u32 =FDCAN_IT_LIST_PROTOCOL_ERROR;
                break;
            case FMKFDCAN_BSP_CB_NB:
            default:
                Ret_e = RC_ERROR_NOT_SUPPORTED;
        }
    }
    
    return Ret_e;
}

/*****************************
* s_FMKFDCAN_GetBspRxFifoId
********************************/
static t_eReturnCode s_FMKFDCAN_GetBspRxFifoId(t_eFMKFDCAN_HwRxFifoList f_RxFifoId_e, t_uint32 *f_bspRxFifoId_u32)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_RxFifoId_e >= FMKFDCAN_HW_RX_FIFO_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(f_bspRxFifoId_u32 == (t_uint32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        switch(f_RxFifoId_e)
        {
            case FMKFDCAN_HW_RX_FIFO_0: 
                *f_bspRxFifoId_u32 =FDCAN_RX_FIFO0;
                break;

            case FMKFDCAN_HW_RX_FIFO_1:
                *f_bspRxFifoId_u32 =FDCAN_RX_FIFO1;
                break;

            case FMKFDCAN_HW_RX_FIFO_NB:
            default:
                Ret_e = RC_ERROR_NOT_SUPPORTED;
        }
    }
    
    return Ret_e;
}
/*****************************
* s_FMKFDCAN_GetBspRxFifoId
********************************/
static t_eReturnCode s_FMKFDCAN_GetBspTxFifoId(t_eFMKFDCAN_HwTxFifoList f_TxFifoId_e, t_uint32 *f_bspTxFifoId_u32)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_TxFifoId_e >= FMKFDCAN_HW_TX_BUFFER_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(f_bspTxFifoId_u32 == (t_uint32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        switch(f_TxFifoId_e)
        {
            case FMKFDCAN_HW_TX_BUFFER_1: 
                *f_bspTxFifoId_u32 =FDCAN_TX_BUFFER0;
                break;

            case FMKFDCAN_HW_TX_BUFFER_2:
                *f_bspTxFifoId_u32 =FDCAN_TX_BUFFER1;
                break;

            case FMKFDCAN_HW_TX_BUFFER_3:
                *f_bspTxFifoId_u32 =FDCAN_TX_BUFFER2;
                break;
            case FMKFDCAN_HW_TX_BUFFER_NB:
            default:
                Ret_e = RC_ERROR_NOT_SUPPORTED;
        }
    }
    
    return Ret_e;
}

/*****************************
* s_FMKFDCAN_GetBspFDFormat
******************************/
static t_eReturnCode s_FMKFDCAN_GetBspFDFormat(t_eFMKFDCAN_FrameFormat f_formatType_e, t_uint32 *f_BspFormatType_pu32)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_formatType_e >= FMKFDCAN_FRAME_FORMAT_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(f_BspFormatType_pu32 == (t_uint32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        switch(f_formatType_e)
        {
            case FMKFDCAN_FRAME_FORMAT_CLASSIC:
                *f_BspFormatType_pu32 = FDCAN_CLASSIC_CAN;
                break;

            case FMKFDCAN_FRAME_FORMAT_FDCAN:
                *f_BspFormatType_pu32 = FDCAN_FD_CAN;
                break;

            case FMKFDCAN_FRAME_PURPOSE_NB:
            default:
                Ret_e = RC_ERROR_NOT_SUPPORTED;
        }
    }
    
    return Ret_e;
}
/*****************************
* s_FMKFDCAN_GetBspQueueType
******************************/
static t_eReturnCode s_FMKFDCAN_GetBspQueueType(t_eFMKFDCAN_HwTxQueueType f_queueType_e, t_uint32 *f_bspQueueType_pu32)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_queueType_e >= FMKFDCAN_HWQUEUE_TYPE_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(f_bspQueueType_pu32 == (t_uint32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        switch(f_queueType_e)
        {
            case FMKFDCAN_HWQUEUE_TYPE_NORMAL:
                *f_bspQueueType_pu32 = FDCAN_TX_QUEUE_OPERATION;
                break;

            case FMKFDCAN_HWQUEUE_TYPE_FIFO:
                *f_bspQueueType_pu32 = FDCAN_TX_FIFO_OPERATION;
                break;
            
            case FMKFDCAN_HWQUEUE_TYPE_NB:
            default:
                Ret_e = RC_ERROR_NOT_SUPPORTED;
        }
    }
    
    return Ret_e;
}

/*****************************
* s_FMKFDCAN_GetBspQueueType
*****************************/
static t_eReturnCode s_FMKFDCAN_GetBspFifoOpeMode(t_eFMKFDCAN_FifoOpeMode f_fifoOpeMode , t_uint32 *f_bspfifoOpeMode_e)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_fifoOpeMode >= FMKFDCAN_FIFO_OPEMODE_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(f_bspfifoOpeMode_e == (t_uint32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        switch(f_fifoOpeMode)
        {
            case FMKFDCAN_FIFO_OPEMODE_OVERWRITE:
                *f_bspfifoOpeMode_e = FDCAN_RX_FIFO_OVERWRITE;
                break;

            case FMKFDCAN_FIFO_OPEMODE_BLOCKING:
                *f_bspfifoOpeMode_e = FDCAN_RX_FIFO_BLOCKING;
                break;
            
            case FMKFDCAN_FIFO_OPEMODE_NB:
            default:
                Ret_e = RC_ERROR_NOT_SUPPORTED;
        }
    }
    
    return Ret_e;
}

static t_eReturnCode s_FMKFDCAN_GetClockKernelDivider(t_eFMKFDCAN_ClockKernelDiv f_ClkDiv_e, t_uint32 * bspClkDiv_pu32)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_ClkDiv_e >= FMKFDCAN_CLOCK_KERNEL_DIV_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(bspClkDiv_pu32 == (t_uint32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        switch(f_ClkDiv_e)
        {
            case FMKFDCAN_CLOCK_KERNEL_DIV1:
                *bspClkDiv_pu32 = FDCAN_CLOCK_DIV1;
                break;
            case FMKFDCAN_CLOCK_KERNEL_DIV2:
                *bspClkDiv_pu32 = FDCAN_CLOCK_DIV2;
                break;
            case FMKFDCAN_CLOCK_KERNEL_DIV4:
                *bspClkDiv_pu32 = FDCAN_CLOCK_DIV4;
                break;
            case FMKFDCAN_CLOCK_KERNEL_DIV6:
                *bspClkDiv_pu32 = FDCAN_CLOCK_DIV6;
                break;
            case FMKFDCAN_CLOCK_KERNEL_DIV8:
                *bspClkDiv_pu32 = FDCAN_CLOCK_DIV8;
                break;
            case FMKFDCAN_CLOCK_KERNEL_DIV10:
                *bspClkDiv_pu32 = FDCAN_CLOCK_DIV10;
                break;
            case FMKFDCAN_CLOCK_KERNEL_DIV12:
                *bspClkDiv_pu32 = FDCAN_CLOCK_DIV12;
                break;
            case FMKFDCAN_CLOCK_KERNEL_DIV14:
                *bspClkDiv_pu32 = FDCAN_CLOCK_DIV14;
                break;
            case FMKFDCAN_CLOCK_KERNEL_DIV16:
                *bspClkDiv_pu32 = FDCAN_CLOCK_DIV16;
                break;
            case FMKFDCAN_CLOCK_KERNEL_DIV18:
                *bspClkDiv_pu32 = FDCAN_CLOCK_DIV18;
                break;
            case FMKFDCAN_CLOCK_KERNEL_DIV20:
                *bspClkDiv_pu32 = FDCAN_CLOCK_DIV20;
                break;
            case FMKFDCAN_CLOCK_KERNEL_DIV22:
                *bspClkDiv_pu32 = FDCAN_CLOCK_DIV22;
                break;
            case FMKFDCAN_CLOCK_KERNEL_DIV24:
                *bspClkDiv_pu32 = FDCAN_CLOCK_DIV24;
                break;
            case FMKFDCAN_CLOCK_KERNEL_DIV26:
                *bspClkDiv_pu32 = FDCAN_CLOCK_DIV26;
                break;
            case FMKFDCAN_CLOCK_KERNEL_DIV28:
                *bspClkDiv_pu32 = FDCAN_CLOCK_DIV28;
                break;
            case FMKFDCAN_CLOCK_KERNEL_DIV30:
                *bspClkDiv_pu32 = FDCAN_CLOCK_DIV30;
                break;
            case FMKFDCAN_CLOCK_KERNEL_DIV_NB:
            default:
                Ret_e = RC_ERROR_NOT_SUPPORTED;
                break;
        }

    }
    return Ret_e;
}

//********************************************************************************
//                      Hardware Callback Management
//********************************************************************************

/**********************************
* HAL_FDCAN_TxEventFifoCallback
**********************************/
void HAL_FDCAN_TxEventFifoCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t TxEventFifoITs)
{
    s_FMKFDCAN_BspTxEventCb(hfdcan, TxEventFifoITs, FMKFDCAN_BSP_TX_CB_EVENT);
}
/**********************************
* HAL_FDCAN_TxFifoEmptyCallback
**********************************/
void HAL_FDCAN_TxFifoEmptyCallback(FDCAN_HandleTypeDef *hfdcan)
{                           // not take in charge
    s_FMKFDCAN_BspTxEventCb(hfdcan, 0xFF, FMKFDCAN_BSP_TX_CB_FIFO_EMPTY);
}
/**********************************
* HAL_FDCAN_TxBufferCompleteCallback
**********************************/
void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
    s_FMKFDCAN_BspTxEventCb(hfdcan, BufferIndexes, FMKFDCAN_BSP_TX_CB_BUFFER_COMPLETE);
}
/**********************************
* HAL_FDCAN_TxBufferAbortCallback
**********************************/
void HAL_FDCAN_TxBufferAbortCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
    s_FMKFDCAN_BspTxEventCb(hfdcan, BufferIndexes, FMKFDCAN_BSP_TX_CB_BUFFER_ABORT);
}

/**********************************
* HAL_FDCAN_RxFifo1Callback
**********************************/
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
    s_FMKFDCAN_BspRxEventCb(hfdcan, RxFifo1ITs, FMKFDCAN_BSP_RX_CB_FIFO_0);
}
/**********************************
* HAL_FDCAN_RxFifo0Callback
**********************************/
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    s_FMKFDCAN_BspRxEventCb(hfdcan, RxFifo0ITs, FMKFDCAN_BSP_RX_CB_FIFO_0);
}
/**********************************
* HAL_FDCAN_RxFifo0Callback
**********************************/
void HAL_FDCAN_ErrorCallback(FDCAN_HandleTypeDef *hfdcan)
{
    s_FMKFDCAN_BspErrorEventCb(hfdcan, hfdcan->ErrorCode);
}
/**********************************
* HAL_FDCAN_RxFifo0Callback
**********************************/
void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
    s_FMKFDCAN_BspErrorEventCb(hfdcan, ErrorStatusITs);
}

#endif // APPSYS_MODULE_FMKCAN_ENABLE
//************************************************************************************
// End of File
//************************************************************************************

/**
 *
 *	@brief
 *	@note   
 *
 *
 *	@param[in] 
 *	@param[out]
 *	 
 *
 *
 */


