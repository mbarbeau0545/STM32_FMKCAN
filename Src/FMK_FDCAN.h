/**
 * @file        FMK_MAC.h
 * @brief       Memomry access control for DMA & user access\n.
 * @note        TemplateDetailsDescription.\n
 *
 * @author      xxxxxx
 * @date        jj/mm/yyyy
 * @version     1.0
 */
  
#ifndef FMKFDCAN_H_INCLUDED
#define FMKFDCAN_H_INCLUDED





    // ********************************************************************
    // *                      Includes
    // ********************************************************************
    #include "TypeCommon.h"
    #include "APP_CFG/ConfigFiles/APPSYS_ConfigPublic.h"
    #ifdef APPSYS_MODULE_FMKCAN_ENABLE
    #include "FMK_CFG/FMKCFG_ConfigFiles/FMKCPU_ConfigPublic.h"
    #include "FMK_CFG/FMKCFG_ConfigFiles/FMKFDCAN_ConfigPublic.h"
    // ********************************************************************
    // *                      Defines
    // ********************************************************************

    // ********************************************************************
    // *                      Types
    // ********************************************************************

    //-----------------------------ENUM TYPES-----------------------------//
    /**
 * @brief Enumeration for FDCAN data length codes (DLC).
 */
typedef enum
{
    FMKFDCAN_DLC_0  = 0x00,       /**< Data length of 0 bytes. */
    FMKFDCAN_DLC_1  = 0x01,       /**< Data length of 1 byte. */
    FMKFDCAN_DLC_2  = 0x02,       /**< Data length of 2 bytes. */
    FMKFDCAN_DLC_3  = 0x03,       /**< Data length of 3 bytes. */
    FMKFDCAN_DLC_4  = 0x04,       /**< Data length of 4 bytes. */
    FMKFDCAN_DLC_5  = 0x05,       /**< Data length of 5 bytes. */
    FMKFDCAN_DLC_6  = 0x06,       /**< Data length of 6 bytes. */
    FMKFDCAN_DLC_7  = 0x07,       /**< Data length of 7 bytes. */
    FMKFDCAN_DLC_8  = 0x08,       /**< Data length of 8 bytes. */
    FMKFDCAN_DLC_16 = 0x10,       /**< Data length of 16 bytes. */
    FMKFDCAN_DLC_20 = 0x14,       /**< Data length of 20 bytes. */
    FMKFDCAN_DLC_24 = 0x18,       /**< Data length of 24 bytes. */
    FMKFDCAN_DLC_32 = 0x20,       /**< Data length of 32 bytes. */
    FMKFDCAN_DLC_48 = 0x30,       /**< Data length of 48 bytes. */
    FMKFDCAN_DLC_64 = 0x40,       /**< Data length of 64 bytes. */

    FMKFDCAN_DLC_NB        /**< Total number of DLC options. */
} t_eFMKFDCAN_DataLength;

/**
 * @brief Enumeration for FDCAN node statuses.
 */
typedef enum 
{
    FMKFDCAN_NODE_STATE_OK             = 0x00,     /**< Node is in a healthy state. */
    FMKFDCAN_NODE_STATE_ERR_PARAM_INIT = 0x01,     /**< An error occured in the intialization*/
    FMKFDCAN_NODE_STATE_ERR_FIFO       = 0x02,     /**< Node is queuing messages. */
    FMKFDCAN_NODE_STATE_ERR_TIMEOUT    = 0x04,     /**< Timeout occured  on the Node */
    FMKFDCAN_NODE_STATE_ERR_PROTOCOL   = 0x08,     /**< Protocol error detected in the node. */
    FMKFDCAN_NODE_STATE_ERR_MEM        = 0x10,     /**< Memory error detected in the node. */
    FMKFDCAN_NODE_STATE_ERR_WDG        = 0x20,     /**<  WhatchDog error detected*/

    FMKFDCAN_NODE_STATE_NB                         /**< Total number of node statuses. */
} t_eFMKFDCAN_NodeStatus;

/**
 * @brief Enumeration for FDCAN message directions.
 */
typedef enum 
{
    FMKFDCAN_NODE_DIRECTION_RX = 0X00, /**< Message direction is receive. */
    FMKFDCAN_NODE_DIRECTION_TX,        /**< Message direction is transmit. */

    FMKFDCAN_NODE_DIRECTION_NB         /**< Total number of message directions. */
} t_eFMKFDCAN_MsgDirection;

/**
 * @brief Structure for identifying FDCAN items.
 */
typedef struct 
{
    t_uint32 Identifier_u32;                    /**< Identifier of the FDCAN item (standard or extended). */
    t_eFMKFDCAN_IdentifierType IdType_e;        /**< Type of identifier (e.g., standard or extended). */
    t_eFMKFDCAN_FramePurpose FramePurpose_e;    /**< Purpose of the frame (e.g., data, remote). */
} t_sFMKFDCAN_ItemIdentifier;

/**
 * @brief Structure for representing FDCAN node messages.
 */
typedef struct 
{
    t_eFMKFDCAN_DataLength Dlc_e;                /**< Data length code (DLC) for the message. */
    t_eFMKFDCAN_MsgDirection Direction_e;        /**< Direction of the message (Rx or Tx). */
    t_uint8 * data_pu8;                          /**< Pointer to the message data buffer. */
} t_sFMKFDCAN_NodeMessage;

/**
 * @brief Structure for managing FDCAN Rx item events.
 */
typedef struct
{
    t_sFMKFDCAN_ItemIdentifier ItemId_s; /**< Identifier of the received item. */
    t_sFMKFDCAN_NodeMessage CanMsg_s;    /**< Message details associated with the event. */
    t_uint32 timeStamp_32;               /**< Timestamp of the received event. */
} t_sFMKFDCAN_RxItemEvent;
    //-----------------------------TYPEDEF TYPES---------------------------//
    /**
    *
    *	@brief      Callback function to be called when a Can Message is received.\n
    *	@note       If a can message is received and it matches Identifer & Mask which has been
    *               Register in FMKFDCAN_ConfigureRxItemEvent software will call you with this function
    *               is used in blocking mode t_sFMKFDCAN_RxItemEventCfg @ref t_sFMKFDCAN_RxItemEventCfg
    *
    *   @param[in] f_Node_e       : CAN  Node -> enum value from @ref t_eFMKFDCAN_NodeList
    *	@param[in] f_RxItem_s     : strcuture that contains Identifer, Trame_Purpose etc @ref t_sFMKFDCAN_RxItemEvent
    *	@param[in] f_NodeStatus_e : Node health when the Msg has benn received
    *
    */
    typedef t_eReturnCode t_cbFMKFDCAN_RcvItem(t_eFMKFDCAN_NodeList f_Node_e,
                                                t_sFMKFDCAN_RxItemEvent f_RxItem_s, 
                                                t_eFMKFDCAN_NodeStatus f_NodeStatus_e);
                                              /**
    *
    *	@brief      Callback Function called when a Message has been succressfully send

    *
    *	@param[in] f_NodeStatus_e : CAN  Node -> enum value from @ref t_eFMKFDCAN_NodeList
    *	@param[in] f_IsMsgTransmit : Wether or not the TxItem has been send (True) or abort (False)
    */
    typedef t_eReturnCode t_cbFMKFDCAN_TxItemStatus(t_eFMKFDCAN_NodeStatus f_NodeStatus_e, t_bool f_IsMsgTransmit);
    //-----------------------------STRUCT TYPES---------------------------//
    

    
    /**
     * @brief Configuration structure for FDCAN Rx item events.
     */
    typedef struct 
    {
        t_sFMKFDCAN_ItemIdentifier ItemId_s; /**< Identifier for the Rx item, including ID type and purpose. */
        t_uint32 maskId_u32;                 /**< Mask applied to the identifier for filtering purposes. */
        t_eFMKFDCAN_DataLength Dlc_e;        /**< Data length code (DLC) for the expected Rx message. */
        t_cbFMKFDCAN_RcvItem *callback_cb;   /**< Pointer to the callback function triggered on receiving this item. */
    } t_sFMKFDCAN_RxItemEventCfg;

    /**
     * @brief Configuration structure for FDCAN Tx items.
     */
    typedef struct
    {
        t_sFMKFDCAN_ItemIdentifier ItemId_s;   /**< Identifier for the Tx item, including ID type and purpose. */
        t_sFMKFDCAN_NodeMessage CanMsg_s;      /**< Message structure containing data and direction for transmission. */
        t_eFMKFDCAN_FrameFormat frameFormat_e; /**< Format of the CAN frame (standard or extended). */
        t_eFMKFDCAN_BitRateSwitchStatus BitRate_e; /**< Bit rate switching status (enabled or disabled). */
    } t_sFMKFDCAN_TxItemCfg;
    /* CAUTION : Automatic generated code section for Structure: Start */

    /* CAUTION : Automatic generated code section for Structure: End */

    // ********************************************************************
    // *                      Prototypes
    // ********************************************************************
        
    // ********************************************************************
    // *                      Variables
    // ********************************************************************



    //********************************************************************************
    //                      Public functions - Prototypes
    //********************************************************************************
    /**
    *
    *	@brief      Perform all Init action for this module.\n
    *	@note       Set to default value all globals structure that
    *                store information for each signals.\n
    *
    */
    t_eReturnCode FMKFDCAN_Init(void);
    /**
    *
    *	@brief      Perform all Cyclic action for this module.\n
    *	@note       
    *
    */
    t_eReturnCode FMKFDCAN_Cyclic(void);
    /**
    *
    *	@brief Function to know the module state.\n 
    *
    *	@param[in]  f_State_pe : store the value, value from @ref t_eCyclicModState
    *
    *   @retval RC_OK                             @ref RC_OK
    *   @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NUL
    */
    t_eReturnCode FMKFDCAN_GetState(t_eCyclicModState *f_State_pe);
    /**
    *
    *	@brief Function to update the module state.\n
    *
    *	@param[in]  f_State_e : the new value, value from @ref t_eCyclicModState
    *
    *   @retval RC_OK                             @ref RC_OK
    */
    t_eReturnCode FMKFDCAN_SetState(t_eCyclicModState f_State_e);
    /**
    *	@brief      Configure a RxItem to be received.\n
    *	@note       This Function is used for registration to received a message CAN.\n
    *               If a Can message is received on the node f_Node_e, and the identifier 
    *               matches the identifer & mask from registration, software will call you with 
    *               t_cbFMKFDCAN_RcvItem.\n
    *               The registration has a limitation, if the Registration buffer is pass,*
    *               RC_ERROR_LIMIT_REACHED is return.\n
    *
    *
    *	@param[in] f_NodeStatus_e : CAN  Node -> enum value from @ref t_eFMKFDCAN_NodeList
    *	@param[in] f_RxItemCfg_s  : Structure for registration @ref t_sFMKFDCAN_RxItemEventCfg
    *	 
    *  @retval RC_OK                             @ref RC_OK
    *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
    *  @retval RC_ERROR_NOT_CONFIGURED           @ref RC_ERROR_NOT_CONFIGURED
    *  @retval RC_ERROR_LIMIT_REACHED            @ref RC_ERROR_LIMIT_REACHED
    *
    */
    t_eReturnCode FMKFDCAN_ConfigureRxItemEvent(t_eFMKFDCAN_NodeList f_Node_e, t_sFMKFDCAN_RxItemEventCfg f_RxItemCfg_s);
    /**
    *	@brief      Send a TxITem into a Node CAN
    *	@note       This Function is used To send a message CAn threw a node with
    *               f_TxItemCfg_s information.\n
    *               If the Hardware TxFiFo is full and cannot accept this TxItem, the software will
    *               put the TxItem into software Buffer and try later on ONLY IF DLC (data_len) IS <= 8.\n
    *
    *	@param[in] f_NodeStatus_e : CAN  Node -> enum value from @ref t_eFMKFDCAN_NodeList
    *	@param[in] f_TxItemCfg_s  : Structure for TxItem @ref t_sFMKFDCAN_TxItemCfg
    *	 
    *  @retval RC_OK                             @ref RC_OK
    *  @retval RC_WARNING_BUSY                   @ref RC_WARNING_BUSY
    *  @retval RC_WARNING_WRONG_RESULT           @ref RC_WARNING_WRONG_RESULT
    *
    */
    t_eReturnCode FMKFDCAN_SendTxItem(t_eFMKFDCAN_NodeList f_Node_e, t_sFMKFDCAN_TxItemCfg f_TxItemCfg_s);
    /**
    *	@brief      Not Implemented Yet
    *	@note
    *
    *
    *	@param[in] 
    *	@param[in]
    *	 
    *
    *
    */
    t_eReturnCode FMKFDCAN_GetRxItem(t_eFMKFDCAN_NodeList f_Node_e, t_sFMKFDCAN_RxItemEvent *f_RxItem_ps);

    /**
    *
    *	@brief      DO NOT USE, Function to get the Handle Type def for IRQN_Handler Function
    *
    *	@param[in]  f_timer_e                : enum value for the timer, value from @ref t_eFMKTIM_Timer
    *
    *
    */
    FDCAN_HandleTypeDef* FMKFDCAN_PRIVATE_GetHandleTypeDef(t_eFMKFDCAN_NodeList f_Node_e);
    #endif // APPSYS_MODULE_FMKCAN_ENABLE
    
#endif // FMKFDCAN_H_INCLUDED           
//************************************************************************************
// End of File
//************************************************************************************

/**
 *	@brief
 *	@note   
 *
 *
 *	@param[in] 
 *	@param[in]
 *	 
 *
 *
 */
