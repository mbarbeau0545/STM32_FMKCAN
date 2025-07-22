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
    #define FMKFDCAN_NODE_MODE (FDCAN_MODE_INTERNAL_LOOPBACK)
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
    #define FMKFDCAN_RX_SOFT_BUFF_SIZE ((t_uint8)20)

    /**
     * @brief Buffer size for transmission software queue.
     */
    #define FMKFDCAN_TX_SOFT_BUFF_SIZE ((t_uint8)20)

    /**
     * @brief Maximum number of RX event registrations.
     */
    #define FMKFDCAN_RX_NUM_REGISTRATION_EVNT ((t_uint8)30)

    // #define FMKFDCAN_RX_NUM_REGISTRATION_POLL ((t_uint8)20) /**< Uncomment for polling-based registrations. */

    /**
     * @brief Maximum number of items to send from TX queue per interrupt.
     */
    #define FMKFDCAN_MAX_TX_ITEM_SEND_PER_IT ((t_uint8)5)

    /**
     * @brief Maximum number of queue items processed per interrupt.
     */
    #define FMKFDCAN_MAX_ITEM_ON_QUEUE_PER_IT ((t_uint8)5)

    /**
     * @brief Buffer size in bytes for a Data Frame FDCAN (64 bytes).
     */
    #define FMKFDCAN_64_BYTES ((t_uint8)64)

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
     * @brief Configuration structure FDCAN Node.
     */
    typedef struct 
    {
        FDCAN_GlobalTypeDef * Instance;
        t_eFMKCPU_ClockPort c_Clock_e;
        t_eFMKCPU_IRQNType c_IrqnLine1_e;
        t_eFMKCPU_IRQNType c_IrqnLine2_e;
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

    /* CAUTION : Automatic generated code section for Variable: End */
    const t_sFMKFDCAN_NodeCfg c_FmkFdcan_NodeCfg_as[FMKFDCAN_NODE_NB] = {
        [FMKFDCAN_NODE_1] = {
            .Instance = FDCAN1,
            .c_Clock_e = FMKCPU_RCC_CLK_FDCAN,
            .c_IrqnLine1_e = FMKCPU_NVIC_FDCAN1_IT0_IRQN,
            .c_IrqnLine2_e = FMKCPU_NVIC_FDCAN1_IT1_IRQN,
        },
        [FMKFDCAN_NODE_2] = {
            .Instance = FDCAN2,
            .c_Clock_e = FMKCPU_RCC_CLK_FDCAN,
            .c_IrqnLine1_e = FMKCPU_NVIC_FDCAN2_IT0_IRQN,
            .c_IrqnLine2_e = FMKCPU_NVIC_FDCAN2_IT1_IRQN,
        },
        [FMKFDCAN_NODE_3] = {
            .Instance = FDCAN3,
            .c_Clock_e = FMKCPU_RCC_CLK_FDCAN,
            .c_IrqnLine1_e = FMKCPU_NVIC_FDCAN3_IT0_IRQN,
            .c_IrqnLine2_e = FMKCPU_NVIC_FDCAN3_IT1_IRQN,
        },
    };

    const t_eFMKFDCAN_NodeCfgList c_FmkCan_NodeCfg_ae[FMKFDCAN_NODE_NB] = {
        FMKFDCAN_NODE_CFG_1,  // FMKFDCAN_NODE_1
        FMKFDCAN_NODE_CFG_1, // FMKFDCAN_NODE_2
        FMKFDCAN_NODE_CFG_1, // FMKFDCAN_NODE_3
    };
    

    const t_bool c_FmkCan_IsNodeActive[FMKFDCAN_NODE_NB] = {
        (t_bool)False,  // FMKFDCAN_NODE_1
        (t_bool)False, // FMKFDCAN_NODE_2
        (t_bool)False, // FMKFDCAN_NODE_3
    };

    const t_uint32 c_FmkCan_NodeItLine[FMKFDCAN_NODE_NB] = {
        FDCAN_INTERRUPT_LINE0,        // FMKFDCAN_NODE_1
        FDCAN_INTERRUPT_LINE1,        // FMKFDCAN_NODE_2
        FDCAN_INTERRUPT_LINE1,        // FMKFDCAN_NODE_3
    };

    const t_sFMKFDCAN_DrvNodeCfg c_FmkCan_BspNodeCfgList_as[FMKFDCAN_NODE_CFG_NB] = 
    {// clockDivider_e                      ProtocolUse_e                       FrameBaudrate_e                 DataBaudrate_e                    QueueType_e                       FifoMode_e
        {FMKFDCAN_CLOCK_KERNEL_DIV1,        FMKFDCAN_PROTOCOL_CAN2_0B,          FMKFDCAN_FRAME_BAUDRATE_250K,   FMKFDCAN_FRAME_BAUDRATE_250K,    FMKFDCAN_HWQUEUE_TYPE_FIFO,      FMKFDCAN_FIFO_OPEMODE_BLOCKING}, // FMKFDCAN_NODE_CFG_1
        {FMKFDCAN_CLOCK_KERNEL_DIV1,        FMKFDCAN_PROTOCOL_CAN2_0B,          FMKFDCAN_FRAME_BAUDRATE_1M,     FMKFDCAN_FRAME_BAUDRATE_1M,      FMKFDCAN_HWQUEUE_TYPE_FIFO,      FMKFDCAN_FIFO_OPEMODE_BLOCKING}, // FMKFDCAN_NODE_CFG_2
        {FMKFDCAN_CLOCK_KERNEL_DIV1,        FMKFDCAN_PROTOCOL_FDCAN_NO_BRS,     FMKFDCAN_FRAME_BAUDRATE_1M,     FMKFDCAN_FRAME_BAUDRATE_1M,      FMKFDCAN_HWQUEUE_TYPE_FIFO,      FMKFDCAN_FIFO_OPEMODE_BLOCKING}, // FMKFDCAN_NODE_CFG_3
        {FMKFDCAN_CLOCK_KERNEL_DIV1,        FMKFDCAN_PROTOCOL_FDCAN_BRS,        FMKFDCAN_FRAME_BAUDRATE_1M,     FMKFDCAN_FRAME_BAUDRATE_4M,      FMKFDCAN_HWQUEUE_TYPE_FIFO,      FMKFDCAN_FIFO_OPEMODE_BLOCKING}, // FMKFDCAN_NODE_CFG_4
    };
    /* /!\/!\/!\ This configration has been calculated for CLOCK FDCAN equals 120MHz but works for every divider /!\/!\/!\*/
    /*  Formule :
    *                                  Fclock (FMKFDCAN_SRC_CLOCK)
    *  Baudrate =     -----------------------------------------
    *                   Prescaler * (SyncSeg + TimSeg1 + TimSeg2)
    */                  
    /*< Configuration to found Init baudrate value*/ 
    const t_sFMKFDCAN_BaudrateCfg c_FmkCan_BspBaudrateCfg_as[FMKFDCAN_FRAME_BAUDRATE_NB] = {
    //       prescaler        syncSeg     timeSeg1     timeSeg2         
            {(t_uint16)150, (t_uint8)1, (t_uint8)15, (t_uint8)4},    // FMKFDCAN_FRAME_BAUDRATE_40K
            {(t_uint16)48,  (t_uint8)1, (t_uint8)15, (t_uint8)4},    // FMKFDCAN_FRAME_BAUDRATE_125K
            {(t_uint16)24,  (t_uint8)1, (t_uint8)15, (t_uint8)4},    // FMKFDCAN_FRAME_BAUDRATE_250K 
            {(t_uint16)12,  (t_uint8)1, (t_uint8)15, (t_uint8)4},    // FMKFDCAN_FRAME_BAUDRATE_500K 
            {(t_uint16)6,   (t_uint8)1, (t_uint8)15, (t_uint8)4},    // FMKFDCAN_FRAME_BAUDRATE_1M 
            {(t_uint16)4,   (t_uint8)1, (t_uint8)10, (t_uint8)4},    // FMKFDCAN_FRAME_BAUDRATE_2M
            {(t_uint16)2,   (t_uint8)1, (t_uint8)10, (t_uint8)4},    // FMKFDCAN_FRAME_BAUDRATE_4M
            {(t_uint16)1,   (t_uint8)1, (t_uint8)10, (t_uint8)4},    // FMKFDCAN_FRAME_BAUDRATE_8M
    };

    //********************************************************************************
    //                      Public functions - Prototyupes
    //********************************************************************************
    /**********************************
    * FDCAN1_IT0_IRQHandler
    **********************************/
    void FDCAN1_IT0_IRQHandler(void)
    {
        HAL_FDCAN_IRQHandler(FMKFDCAN_PRIVATE_GetHandleTypeDef(FMKFDCAN_NODE_1));
    }
    /**********************************
    * FDCAN1_IT1_IRQHandler
    **********************************/
    void FDCAN1_IT1_IRQHandler(void)
    {
        HAL_FDCAN_IRQHandler(FMKFDCAN_PRIVATE_GetHandleTypeDef(FMKFDCAN_NODE_1));
    }
    /**********************************
    * FDCAN2_IT0_IRQHandler
    **********************************/
    void FDCAN2_IT0_IRQHandler(void)
    {
        HAL_FDCAN_IRQHandler(FMKFDCAN_PRIVATE_GetHandleTypeDef(FMKFDCAN_NODE_2));
    }
    /**********************************
    * FDCAN2_IT1_IRQHandler
    **********************************/
    void FDCAN2_IT1_IRQHandler(void)
    {
        HAL_FDCAN_IRQHandler(FMKFDCAN_PRIVATE_GetHandleTypeDef(FMKFDCAN_NODE_2));
    }
    /**********************************
    * FDCAN3_IT0_IRQHandler
    **********************************/
    void FDCAN3_IT0_IRQHandler(void)
    {
        HAL_FDCAN_IRQHandler(FMKFDCAN_PRIVATE_GetHandleTypeDef(FMKFDCAN_NODE_3));
    }
    /**********************************
    * FDCAN3_IT1_IRQHandler
    **********************************/
    void FDCAN3_IT1_IRQHandler(void)
    {
        HAL_FDCAN_IRQHandler(FMKFDCAN_PRIVATE_GetHandleTypeDef(FMKFDCAN_NODE_3));
    }
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
