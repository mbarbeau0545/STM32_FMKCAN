/*********************************************************************
 * @file        FMKCDA_ConfigPrivate.h
 * @brief       Template_BriefDescription.
 * @note        TemplateDetailsDescription.\n
 *
 * @author      xxxxxx
 * @date        jj/mm/yyyy
 * @version     1.0
 */
  
#ifndef FMKFDCAN_CONFIGPRIVATE_H_INCLUDED
#define FMKFDCAN_CONFIGPRIVATE_H_INCLUDED





    // ********************************************************************
    // *                      Includes
    // ********************************************************************
    #include "./FMKFDCAN_ConfigPublic.h"
    #include "./FMKCPU_ConfigPublic.h"
    #include "FMK_HAL/FMK_CAN/Src/FMK_FDCAN.h"
    // ********************************************************************
    // *                      Defines
    // ********************************************************************
    /**
     * @brief Operating mode for FDCAN nodes.
     */
    #define FMKFDCAN_NODE_MODE (FDCAN_MODE_NORMAL)
    /**
     * @brief Source clock for FDCAN kernel (in MHz).
     */
    #define FMKFDCAN_SRC_CLOCK ((t_uint8)FMKCPU_PLLQ_CLOCK_OSC_MHZ)

    /**
     * @brief Buffer size for reception without callback queue.
     */
    #define FMKFDCAN_RX_NO_CB_BUFF_SIZE ((t_uint8)15)

    /**
     * @brief Buffer size for reception software queue.
     */
    #define FMKFDCAN_RX_BUFFER_DATA_SIZE       (FMKFDCAN_DLC_8)

    /**
     * @brief Buffer size for transmission software queue.
     */
    #define FMKFDCAN_TX_BUFFER_DATA_SIZE       (FMKFDCAN_DLC_8)

    /**
     * @brief Maximum number of RX event registrations.
     */
    #define FMKFDCAN_RX_NUM_REGISTRATION_EVNT ((t_uint8)30)

    // #define FMKFDCAN_RX_NUM_REGISTRATION_POLL ((t_uint8)20) /**< Uncomment for polling-based registrations. */

    /**
     * @brief Maximum number of items to send from TX queue per interrupt.
     */
    #define FMKFDCAN_MAX_TX_ITEM_SEND_PER_IT ((t_uint8)8)

    /**
     * @brief Maximum number of queue items processed per interrupt.
     */
    #define FMKFDCAN_MAX_ITEM_ON_QUEUE_PER_IT ((t_uint8)5)

    /**
     * @brief Buffer size in bytes for a Data Frame FDCAN (64 bytes).
     */
    #define FMKFDCAN_64_BYTES ((t_uint8)64)

#if defined (FMKCPU_STM32_ECU_FAMILY_G4)
    ///@brief Rx,Tx Bsp Buffer size, @note once we integrate the HAL_LIB inside project change this define in fdcan.c for G4
    #define FMKFDCAN_SRAMCAN_RF0_NBR                ((t_uint32)6U)         /* RX FIFO 0 Elements Number             */
    #define FMKFDCAN_SRAMCAN_RF1_NBR                ((t_uint32)0U)         /* RX FIFO 1 Elements Number             */
    #define FMKFDCAN_SRAMCAN_TEF_NBR                ((t_uint32)3U)         /* TX Event FIFO Elements Number         */
    #define FMKFDCAN_SRAMCAN_TFQ_NBR                ((t_uint32)3U)         /* TX FIFO/Queue Elements Number         */

    #define FMKFDCA_SRAMCAN_FLS_SIZE                ((t_uint32)1U  * 4U)         /* Filter Standard Element Size in bytes */
    #define FMKFDCA_SRAMCAN_FLE_SIZE                ((t_uint32)2U  * 4U)         /* Filter Extended Element Size in bytes */
    #define FMKFDCA_SRAMCAN_RF0_SIZE                ((t_uint32)18U * 4U)         /* RX FIFO 0 Elements Size in bytes      */
    #define FMKFDCA_SRAMCAN_RF1_SIZE                ((t_uint32)18U * 4U)         /* RX FIFO 1 Elements Size in bytes      */
    #define FMKFDCA_SRAMCAN_TEF_SIZE                ((t_uint32)2U  * 4U)         /* TX Event FIFO Elements Size in bytes  */
    #define FMKFDCA_SRAMCAN_TFQ_SIZE                ((t_uint32)18U * 4U)         /* TX FIFO/Queue Elements Size in bytes  */
#elif defined(FMKCPU_STM32_ECU_FAMILY_H7)
    ///@brief Rx,Tx Bsp Buffer size, 

    #define FMKCPU_SRAMCAN_FLS_NBR                  ((t_uint32)28U)        /* Max. Filter List Standard Number      */
    #define FMKCPU_SRAMCAN_FLE_NBR                  ((t_uint32)8U)         /* Max. Filter List Extended Number      */
    #define FMKFDCAN_SRAMCAN_RF0_NBR                ((t_uint32)64U)         /* RX FIFO 0 Elements Number */
    #define FMKFDCAN_SRAMCAN_RF1_NBR                ((t_uint32)0U)         /* RX FIFO 1 Elements Number */
    #define FMKFDCAN_SRAMCAN_RB0_NBR                ((t_uint32)0U)         /* RX Buffer 0 Elements Number */
    #define FMKFDCAN_SRAMCAN_TEF_NBR                ((t_uint32)32U)         /* TX Event FIFO Elements Number */
    #define FMKFDCAN_SRAMCAN_TFQ_NBR                ((t_uint32)32U)         /* TX FIFO/Queue Elements Number */

    #define FMKFDCA_SRAMCAN_FLS_SIZE                ((t_uint32)1U  * 4U)         /* Filter Standard Element Size in bytes */
    #define FMKFDCA_SRAMCAN_FLE_SIZE                ((t_uint32)2U  * 4U)         /* Filter Extended Element Size in bytes */
    #define FMKFDCA_SRAMCAN_RF0_SIZE                ((t_uint32)18U * 4U)         /* RX FIFO 0 Elements Size in bytes */
    #define FMKFDCA_SRAMCAN_RF1_SIZE                ((t_uint32)18U * 4U)         /* RX FIFO 1 Elements Size in bytes */
    #define FMKFDCA_SRAMCAN_RB0_SIZE                ((t_uint32)18U * 4U)         /* RX buffer 0 Elements Size in bytes */
    #define FMKFDCA_SRAMCAN_TEF_SIZE                ((t_uint32)2U  * 4U)         /* TX Event FIFO Elements Size in bytes */
    #define FMKFDCA_SRAMCAN_TFQ_SIZE                ((t_uint32)18U * 4U)         /* TX FIFO/Queue Elements Size in bytes */
#endif //
///@brief Define for Interrupt mask set they are define in G4 but not in H7
#ifndef FDCAN_IT_LIST_RX_FIFO0
#define FDCAN_IT_LIST_RX_FIFO0         (FDCAN_IT_RX_FIFO0_MESSAGE_LOST | \
                                        FDCAN_IT_RX_FIFO0_FULL         | \
                                        FDCAN_IT_RX_FIFO0_NEW_MESSAGE)       /*!< RX FIFO 0 Interrupts List          */
#endif // FDCAN_IT_LIST_RX_FIFO0
#ifndef FDCAN_IT_LIST_RX_FIFO1
#define FDCAN_IT_LIST_RX_FIFO1         (FDCAN_IT_RX_FIFO1_MESSAGE_LOST | \
                                        FDCAN_IT_RX_FIFO1_FULL         | \
                                        FDCAN_IT_RX_FIFO1_NEW_MESSAGE)       /*!< RX FIFO 1 Interrupts List          */
#endif // FDCAN_IT_LIST_RX_FIFO1
#define FDCAN_IT_LIST_SMSG             (FDCAN_IT_TX_ABORT_COMPLETE | \
                                        FDCAN_IT_TX_COMPLETE | \
                                        FDCAN_IT_RX_HIGH_PRIORITY_MSG)       /*!< Status Message Interrupts List     */
#ifndef FDCAN_IT_LIST_TX_FIFO_ERROR
#define FDCAN_IT_LIST_TX_FIFO_ERROR    (FDCAN_IT_TX_EVT_FIFO_ELT_LOST | \
                                        FDCAN_IT_TX_EVT_FIFO_FULL | \
                                        FDCAN_IT_TX_EVT_FIFO_NEW_DATA | \
                                        FDCAN_IT_TX_FIFO_EMPTY)              /*!< TX FIFO Error Interrupts List      */
#endif // FDCAN_IT_LIST_SMSG
#define FDCAN_IT_LIST_MISC             (FDCAN_IT_TIMEOUT_OCCURRED | \
                                        FDCAN_IT_RAM_ACCESS_FAILURE | \
                                        FDCAN_IT_TIMESTAMP_WRAPAROUND)       /*!< Misc. Interrupts List              */
#ifndef FDCAN_IT_LIST_BIT_LINE_ERROR
#define FDCAN_IT_LIST_BIT_LINE_ERROR   (FDCAN_IT_ERROR_PASSIVE | \
                                        FDCAN_IT_ERROR_LOGGING_OVERFLOW)     /*!< Bit and Line Error Interrupts List */
#endif // FDCAN_IT_LIST_BIT_LINE_ERROR
#ifndef FDCAN_IT_LIST_PROTOCOL_ERROR
#define FDCAN_IT_LIST_PROTOCOL_ERROR   (FDCAN_IT_RESERVED_ADDRESS_ACCESS | \
                                        FDCAN_IT_DATA_PROTOCOL_ERROR | \
                                        FDCAN_IT_ARB_PROTOCOL_ERROR | \
                                        FDCAN_IT_RAM_WATCHDOG | \
                                        FDCAN_IT_BUS_OFF | \
                                        FDCAN_IT_ERROR_WARNING) 
#endif // FDCAN_IT_LIST_PROTOCOL_ERROR
    // ********************************************************************
    // *                      Types
    // ********************************************************************
    //-----------------------------ENUM TYPES-----------------------------//
    // Flag Automatic Generate Code
    /**
     * @brief List of hardware RX FIFOs.
     */
    typedef enum 
    {
        FMKFDCAN_HW_RX_FIFO_0 = 0x00, /**< RX FIFO 0 for receiving messages. */
        FMKFDCAN_HW_RX_FIFO_1,        /**< RX FIFO 1 for receiving messages. */

        FMKFDCAN_HW_RX_FIFO_NB,       /**< Number of RX FIFOs available. */
    } t_eFMKFDCAN_HwRxFifoList;

    /**
     * @brief List of hardware TX FIFOs.
     */
    typedef enum 
    {
        FMKFDCAN_HW_TX_BUFFER_1 = 0x00,    /**< TX Buffer 1 for transmitting messages. */
        FMKFDCAN_HW_TX_BUFFER_2,           /**< TX Buffer 2 for transmitting messages. */ 
        FMKFDCAN_HW_TX_BUFFER_3,           /**< TX Buffer 3  for transmitting messages. */ 

        FMKFDCAN_HW_TX_BUFFER_NB,       /**< Number of TX FIFOs available. */
    } t_eFMKFDCAN_HwTxFifoList;
    /* CAUTION : Automatic generated code section for Enum: Start */

    /* CAUTION : Automatic generated code section for Enum: End */

    //-----------------------------TYPEDEF TYPES---------------------------//
    
    //-----------------------------STRUCT TYPES---------------------------//
    /**
     * @brief Configuration structure for baud rate parameters.
     */
    typedef struct 
    {
        t_uint16 prescaler_u16; /**< Prescaler value for the FDCAN clock. */
        t_uint8 syncSeg_u8;     /**< Synchronization segment value. */
        t_uint8 timeSeg1_u8;    /**< Time segment 1 value. */
        t_uint8 timeSeg2_u8;    /**< Time segment 2 value. */
    } t_sFMKFDCAN_BaudrateCfg;

        /**
     * @brief Rx item buffer structure for receiving FDCAN frames.
     */
    typedef struct __t_sFMKFDCAN_RxItemBuffer
    {
        FDCAN_RxHeaderTypeDef bspRxItem_s; /**< FDCAN BSP Rx header structure for frame details. */
        t_uint8 data_ua8[FMKFDCAN_RX_BUFFER_DATA_SIZE];  /**< Data buffer for the received frame. */
    } t_sFMKFDCAN_RxItemBuffer;

    /**
     * @brief Tx item buffer structure for transmitting FDCAN frames.
     */
    typedef struct 
    {
        FDCAN_TxHeaderTypeDef bspTxItem_s; /**< FDCAN BSP Tx header structure for frame details. */
        t_uint8 data_ua8[FMKFDCAN_TX_BUFFER_DATA_SIZE];  /**< Data buffer for the frame to transmit. */
    } t_sFMKFDCAN_TxItemBuffer;

    /**
     * @brief Configuration structure FDCAN Node.
     */
    typedef struct 
    {
        FDCAN_GlobalTypeDef * Instance;
        t_eFMKCPU_ClockPort c_Clock_e;
        t_eFMKCPU_IRQNType c_IrqnLine1_e;
        t_eFMKCPU_IRQNType c_IrqnLine2_e;
        t_sFMKFDCAN_RxItemBuffer * rxBufferStartAddress_pas;
        t_uint16 rxBufferSize_u16;
        t_sFMKFDCAN_TxItemBuffer * txBufferStartAddress_pas;
        t_uint16 txBufferSize_u16;
    } t_sFMKFDCAN_NodeCfg;
    /* CAUTION : Automatic generated code section for Structure: Start */

    /* CAUTION : Automatic generated code section for Structure: End */
    
    // ********************************************************************
    // *                      Prototypes
    // ********************************************************************
        
    // ********************************************************************
    // *                      Variables
    // ********************************************************************
    /* CAUTION : Automatic generated code section for Variable: Start */
    //--------- Tx, Rx Buffer for Can Node 1 ---------//
    t_sFMKFDCAN_RxItemBuffer g_Node1_RxBuffer_as[40];
    t_sFMKFDCAN_TxItemBuffer g_Node1_TxBuffer_as[60];
    //--------- Tx, Rx Buffer for Can Node 2 ---------//
    t_sFMKFDCAN_RxItemBuffer g_Node2_RxBuffer_as[0];
    t_sFMKFDCAN_TxItemBuffer g_Node2_TxBuffer_as[0];
    //--------- Tx, Rx Buffer for Can Node 3 ---------//
    t_sFMKFDCAN_RxItemBuffer g_Node3_RxBuffer_as[0];
    t_sFMKFDCAN_TxItemBuffer g_Node3_TxBuffer_as[0];


    ///@brief Node configuraiton
    const t_sFMKFDCAN_NodeCfg c_FmkFdcan_NodeCfg_as[FMKFDCAN_NODE_NB] = {
        [FMKFDCAN_NODE_1] = {
            .Instance = FDCAN1,
            .c_Clock_e = FMKCPU_RCC_CLK_FDCAN,
            .c_IrqnLine1_e = FMKCPU_NVIC_FDCAN1_IT0_IRQN,
            .c_IrqnLine2_e = FMKCPU_NVIC_FDCAN1_IT1_IRQN,
            .rxBufferStartAddress_pas = (t_sFMKFDCAN_RxItemBuffer *)(&g_Node1_RxBuffer_as[0]),
            .rxBufferSize_u16 = (t_uint16)40,
            .txBufferStartAddress_pas = (t_sFMKFDCAN_TxItemBuffer *)(&g_Node1_TxBuffer_as[0]),
            .txBufferSize_u16 = (t_uint16)60,
        },
        [FMKFDCAN_NODE_2] = {
            .Instance = FDCAN2,
            .c_Clock_e = FMKCPU_RCC_CLK_FDCAN,
            .c_IrqnLine1_e = FMKCPU_NVIC_FDCAN2_IT0_IRQN,
            .c_IrqnLine2_e = FMKCPU_NVIC_FDCAN2_IT1_IRQN,
            .rxBufferStartAddress_pas = (t_sFMKFDCAN_RxItemBuffer *)(&g_Node2_RxBuffer_as[0]),
            .rxBufferSize_u16 = (t_uint16)0,
            .txBufferStartAddress_pas = (t_sFMKFDCAN_TxItemBuffer *)(&g_Node2_TxBuffer_as[0]),
            .txBufferSize_u16 = (t_uint16)0,
        },
        [FMKFDCAN_NODE_3] = {
            .Instance = FDCAN3,
            .c_Clock_e = FMKCPU_RCC_CLK_FDCAN,
            .c_IrqnLine1_e = FMKCPU_NVIC_FDCAN3_IT0_IRQN,
            .c_IrqnLine2_e = FMKCPU_NVIC_FDCAN3_IT1_IRQN,
            .rxBufferStartAddress_pas = (t_sFMKFDCAN_RxItemBuffer *)(&g_Node3_RxBuffer_as[0]),
            .rxBufferSize_u16 = (t_uint16)0,
            .txBufferStartAddress_pas = (t_sFMKFDCAN_TxItemBuffer *)(&g_Node3_TxBuffer_as[0]),
            .txBufferSize_u16 = (t_uint16)0,
        },
    };

    /* CAUTION : Automatic generated code section for Variable: End */

    const t_eFMKFDCAN_NodeCfgList c_FmkCan_NodeCfg_ae[FMKFDCAN_NODE_NB] = {
        FMKFDCAN_NODE_CFG_1,  // FMKFDCAN_NODE_1
        FMKFDCAN_NODE_CFG_1, // FMKFDCAN_NODE_2
#if defined(FMKCPU_STM32_ECU_FAMILY_G4)
        FMKFDCAN_NODE_CFG_1, // FMKFDCAN_NODE_3
#endif
    };
    

    const t_bool c_FmkCan_IsNodeActive[FMKFDCAN_NODE_NB] = {
        (t_bool)True,  // FMKFDCAN_NODE_1
        FALSE, // FMKFDCAN_NODE_2
#if defined(FMKCPU_STM32_ECU_FAMILY_G4)
        FALSE, // FMKFDCAN_NODE_3
#endif
    };


    const t_sFMKFDCAN_DrvNodeCfg c_FmkCan_BspNodeCfgList_as[FMKFDCAN_NODE_CFG_NB] = 
    {// clockDivider_e                      ProtocolUse_e                       FrameBaudrate_e                 DataBaudrate_e                    QueueType_e                       FifoMode_e
        {FMKFDCAN_CLOCK_KERNEL_DIV1,        FMKFDCAN_PROTOCOL_CAN2_0B,          FMKFDCAN_FRAME_BAUDRATE_250K,   FMKFDCAN_FRAME_BAUDRATE_250K,    FMKFDCAN_HWQUEUE_TYPE_FIFO,      FMKFDCAN_FIFO_OPEMODE_BLOCKING}, // FMKFDCAN_NODE_CFG_1
        {FMKFDCAN_CLOCK_KERNEL_DIV1,        FMKFDCAN_PROTOCOL_CAN2_0B,          FMKFDCAN_FRAME_BAUDRATE_1M,     FMKFDCAN_FRAME_BAUDRATE_1M,      FMKFDCAN_HWQUEUE_TYPE_FIFO,      FMKFDCAN_FIFO_OPEMODE_BLOCKING}, // FMKFDCAN_NODE_CFG_2
        {FMKFDCAN_CLOCK_KERNEL_DIV1,        FMKFDCAN_PROTOCOL_CAN2_0B,     FMKFDCAN_FRAME_BAUDRATE_1M,     FMKFDCAN_FRAME_BAUDRATE_1M,      FMKFDCAN_HWQUEUE_TYPE_FIFO,      FMKFDCAN_FIFO_OPEMODE_BLOCKING}, // FMKFDCAN_NODE_CFG_3
        {FMKFDCAN_CLOCK_KERNEL_DIV1,        FMKFDCAN_PROTOCOL_CAN2_0B,        FMKFDCAN_FRAME_BAUDRATE_1M,     FMKFDCAN_FRAME_BAUDRATE_4M,      FMKFDCAN_HWQUEUE_TYPE_FIFO,      FMKFDCAN_FIFO_OPEMODE_BLOCKING}, // FMKFDCAN_NODE_CFG_4
    };
    /* /!\/!\/!\ This configration has been calculated for CLOCK FDCAN equals 128MHz but works for every divider /!\/!\/!\*/
    /*  Formule :
    *                                  Fclock (FMKFDCAN_SRC_CLOCK)
    *  Baudrate =     -----------------------------------------
    *                   Prescaler * (SyncSeg + TimSeg1 + TimSeg2)
    */                  
    /*< Configuration to found Init baudrate value*/ 
    const t_sFMKFDCAN_BaudrateCfg c_FmkCan_BspBaudrateCfg_as[FMKFDCAN_FRAME_BAUDRATE_NB] = {
    //       prescaler        syncSeg     timeSeg1     timeSeg2         
            {(t_uint16)200, (t_uint8)1, (t_uint8)12, (t_uint8)3},    // FMKFDCAN_FRAME_BAUDRATE_40K
            {(t_uint16)64,  (t_uint8)1, (t_uint8)12, (t_uint8)3},    // FMKFDCAN_FRAME_BAUDRATE_125K
            {(t_uint16)32,  (t_uint8)1, (t_uint8)12, (t_uint8)3},    // FMKFDCAN_FRAME_BAUDRATE_250K 
            {(t_uint16)16,  (t_uint8)1, (t_uint8)12, (t_uint8)3},    // FMKFDCAN_FRAME_BAUDRATE_500K 
            {(t_uint16)8,   (t_uint8)1, (t_uint8)12, (t_uint8)3},    // FMKFDCAN_FRAME_BAUDRATE_1M 
            {(t_uint16)4,   (t_uint8)1, (t_uint8)12, (t_uint8)3},    // FMKFDCAN_FRAME_BAUDRATE_2M
            {(t_uint16)2,   (t_uint8)1, (t_uint8)12, (t_uint8)3},    // FMKFDCAN_FRAME_BAUDRATE_4M
            {(t_uint16)1,   (t_uint8)1, (t_uint8)12, (t_uint8)3},    // FMKFDCAN_FRAME_BAUDRATE_8M
    };

    //********************************************************************************
    //                      Public functions - Prototyupes
    //********************************************************************************
    /* CAUTION : Automatic generated code section for FDCAN IRQN Handler : Start */
    /**********************************
    * FDCAN2_IT0_IRQHandler
    **********************************/
    void FDCAN2_IT0_IRQHandler(void)
    {
    HAL_FDCAN_IRQHandler(FMKFDCAN_PRIVATE_GetHandleTypeDef(FMKFDCAN_NODE_2));
    return;
    }

    /**********************************
    * FDCAN2_IT1_IRQHandler
    **********************************/
    void FDCAN2_IT1_IRQHandler(void)
    {
    HAL_FDCAN_IRQHandler(FMKFDCAN_PRIVATE_GetHandleTypeDef(FMKFDCAN_NODE_2));
    return;
    }

    /**********************************
    * FDCAN3_IT0_IRQHandler
    **********************************/
    void FDCAN3_IT0_IRQHandler(void)
    {
    HAL_FDCAN_IRQHandler(FMKFDCAN_PRIVATE_GetHandleTypeDef(FMKFDCAN_NODE_3));
    return;
    }

    /**********************************
    * FDCAN3_IT1_IRQHandler
    **********************************/
    void FDCAN3_IT1_IRQHandler(void)
    {
    HAL_FDCAN_IRQHandler(FMKFDCAN_PRIVATE_GetHandleTypeDef(FMKFDCAN_NODE_3));
    return;
    }

    /**********************************
    * FDCAN1_IT0_IRQHandler
    **********************************/
    void FDCAN1_IT0_IRQHandler(void)
    {
    HAL_FDCAN_IRQHandler(FMKFDCAN_PRIVATE_GetHandleTypeDef(FMKFDCAN_NODE_1));
    return;
    }

    /**********************************
    * FDCAN1_IT1_IRQHandler
    **********************************/
    void FDCAN1_IT1_IRQHandler(void)
    {
    HAL_FDCAN_IRQHandler(FMKFDCAN_PRIVATE_GetHandleTypeDef(FMKFDCAN_NODE_1));
    return;
    }

    /* CAUTION : Automatic generated code section for FDCAN IRQN Handler : End */

#endif // FMKFDCAN_CONFIGPRIVATE_H_INCLUDED           
//************************************************************************************
// End of File
//************************************************************************************

/**
 *
 *	@brief
 *	@note   
 *
 *
 *	@params[in] 
 *	@params[out]
 *	 
 *
 *
 */
