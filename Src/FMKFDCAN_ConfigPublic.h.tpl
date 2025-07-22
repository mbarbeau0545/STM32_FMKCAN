/*********************************************************************
 * @file        FMKCDA_ConfigPrivate.h
 * @brief       Template_BriefDescription.
 * @note        TemplateDetailsDescription.\n
 *
 * @author      xxxxxx
 * @date        jj/mm/yyyy
 * @version     1.0
 */


#ifndef FMKFDCAN_CONFIGPUBLIC_H_INCLUDED
#define FMKFDCAN_CONFIGPUBLIC_H_INCLUDED





    // ********************************************************************
    // *                      Includes
    // ********************************************************************

    // ********************************************************************
    // *                      Defines
    // ********************************************************************

    // ********************************************************************
    // *                      Types
    // ********************************************************************
    /**
     * @brief List of FDCAN nodes.
     */
    typedef enum
    {
        FMKFDCAN_NODE_1 = 0x00, /**< Node 1 identifier. */
        FMKFDCAN_NODE_2,        /**< Node 2 identifier. */
        FMKFDCAN_NODE_3,        /**< Node 3 identifier. */

        FMKFDCAN_NODE_NB,       /**< Total number of nodes. */
    } t_eFMKFDCAN_NodeList;

    /**
     * @brief Operating modes for an FDCAN node.
     */
    typedef enum
    {
        FMKFDCAN_NODE_OPEMODE_NORMAL = 0x00,        /**< Normal operating mode. */
        FMKFDCAN_NODE_OPEMODE_RESTRICTED,           /**< Restricted operating mode. */
        FMKFDCAN_NODE_OPEMODE_BUS_MONITRING,        /**< Bus monitoring mode. */
        FMKFDCAN_NODE_OPEMODE_INTERNAL_LB,          /**< Internal loopback mode. */
        FMKFDCAN_NODE_OPEMODE_EXTERNAL_LB,          /**< External loopback mode. */

        FMKFDCAN_NODE_OPEMODE_NB,                   /**< Total number of operating modes. */
    } t_eFMKFDCAN_NodeOpeMode;

    /**
     * @brief Configuration list for FDCAN nodes.
     */
    typedef enum
    {
        FMKFDCAN_NODE_CFG_1 = 0x00, /**< Node configuration 1. */
        FMKFDCAN_NODE_CFG_2,        /**< Node configuration 2. */
        FMKFDCAN_NODE_CFG_3,        /**< Node configuration 3. */
        FMKFDCAN_NODE_CFG_4,        /**< Node configuration 4. */

        FMKFDCAN_NODE_CFG_NB,       /**< Total number of node configurations. */
    } t_eFMKFDCAN_NodeCfgList;

    /**
     * @brief FDCAN interrupt lines.
     */
    typedef enum
    {
        FMKFDCAN_INTERUPT_LINE_1 = 0x00, /**< Interrupt line 1. */
        FMKFDCAN_INTERUPT_LINE_2,        /**< Interrupt line 2. */

        FMKFDCAN_INTERUPT_LINE_NB,       /**< Total number of interrupt lines. */
    } t_eFMKFDCAN_InterruptLine;

    /**
     * @brief Baud rates supported by FDCAN frames.
     */
    typedef enum
    {
        FMKFDCAN_FRAME_BAUDRATE_40K = 0x00, /**< Baud rate 40 Kbps. */
        FMKFDCAN_FRAME_BAUDRATE_125K,       /**< Baud rate 125 Kbps. */
        FMKFDCAN_FRAME_BAUDRATE_250K,       /**< Baud rate 250 Kbps. */
        FMKFDCAN_FRAME_BAUDRATE_500K,       /**< Baud rate 500 Kbps. */
        FMKFDCAN_FRAME_BAUDRATE_1M,         /**< Baud rate 1 Mbps. */
        FMKFDCAN_FRAME_BAUDRATE_2M,         /**< Baud rate 2 Mbps. */
        FMKFDCAN_FRAME_BAUDRATE_4M,         /**< Baud rate 4 Mbps. */
        FMKFDCAN_FRAME_BAUDRATE_8M,         /**< Baud rate 8 Mbps. */

        FMKFDCAN_FRAME_BAUDRATE_NB,         /**< Total number of supported baud rates. */
    } t_eFMKFDCAN_Baudrate;

    /**
     * @brief Clock kernel divider configurations for FDCAN.
     */
    typedef enum
    {
        FMKFDCAN_CLOCK_KERNEL_DIV1  = 0x01, /**< FDCAN clock divided by 1. */
        FMKFDCAN_CLOCK_KERNEL_DIV2  = 0x02, /**< FDCAN clock divided by 2. */
        FMKFDCAN_CLOCK_KERNEL_DIV4  = 0x04, /**< FDCAN clock divided by 4. */
        FMKFDCAN_CLOCK_KERNEL_DIV6  = 0x06, /**< FDCAN clock divided by 6. */
        FMKFDCAN_CLOCK_KERNEL_DIV8  = 0x08, /**< FDCAN clock divided by 8. */
        FMKFDCAN_CLOCK_KERNEL_DIV10 = 0x0A, /**< FDCAN clock divided by 10. */
        FMKFDCAN_CLOCK_KERNEL_DIV12 = 0x0C, /**< FDCAN clock divided by 12. */
        FMKFDCAN_CLOCK_KERNEL_DIV14 = 0x0E, /**< FDCAN clock divided by 14. */
        FMKFDCAN_CLOCK_KERNEL_DIV16 = 0x10, /**< FDCAN clock divided by 16. */
        FMKFDCAN_CLOCK_KERNEL_DIV18 = 0x12, /**< FDCAN clock divided by 18. */
        FMKFDCAN_CLOCK_KERNEL_DIV20 = 0x14, /**< FDCAN clock divided by 20. */
        FMKFDCAN_CLOCK_KERNEL_DIV22 = 0x16, /**< FDCAN clock divided by 22. */
        FMKFDCAN_CLOCK_KERNEL_DIV24 = 0x18, /**< FDCAN clock divided by 24. */
        FMKFDCAN_CLOCK_KERNEL_DIV26 = 0x1A, /**< FDCAN clock divided by 26. */
        FMKFDCAN_CLOCK_KERNEL_DIV28 = 0x1C, /**< FDCAN clock divided by 28. */
        FMKFDCAN_CLOCK_KERNEL_DIV30 = 0x1E, /**< FDCAN clock divided by 30. */

        FMKFDCAN_CLOCK_KERNEL_DIV_NB = 0x1F /**< Total number of clock divider options. */
    } t_eFMKFDCAN_ClockKernelDiv;

    /**
     * @brief Hardware queue types for FDCAN.
     */
    typedef enum
    {
        FMKFDCAN_HWQUEUE_TYPE_NORMAL = 0x00, /**< Hardware queue operates as a normal queue. */
        FMKFDCAN_HWQUEUE_TYPE_FIFO,          /**< Hardware queue operates as a FIFO. */

        FMKFDCAN_HWQUEUE_TYPE_NB,            /**< Total number of hardware queue types. */
    } t_eFMKFDCAN_HwTxQueueType;

    /**
     * @brief FIFO operation modes for FDCAN.
     */
    typedef enum
    {
        FMKFDCAN_FIFO_OPEMODE_OVERWRITE = 0x00, /**< FIFO overwrite mode. */
        FMKFDCAN_FIFO_OPEMODE_BLOCKING,         /**< FIFO blocking mode. */

        FMKFDCAN_FIFO_OPEMODE_NB,               /**< Total number of FIFO operation modes. */
    } t_eFMKFDCAN_FifoOpeMode;

    /**
     * @brief Frame formats supported by FDCAN.
     */
    typedef enum
    {
        FMKFDCAN_FRAME_FORMAT_CLASSIC = 0x00, /**< Classic CAN frame format. */
        FMKFDCAN_FRAME_FORMAT_FDCAN,          /**< FDCAN frame format. */

        FMKFDCAN_FRAME_FORMAT_NB,             /**< Total number of frame formats. */
    } t_eFMKFDCAN_FrameFormat;

    /**
     * @brief Bit rate switching status for FDCAN frames.
     */
    typedef enum
    {
        FMKFDCAN_BITRATE_SWITCH_ON = 0x00, /**< Bit rate switching enabled. */
        FMKFDCAN_BITRATE_SWITCH_OFF,       /**< Bit rate switching disabled. */

        FMKFDCAN_BITRATE_SWITCH_NB,        /**< Total number of bit rate switch statuses. */
    } t_eFMKFDCAN_BitRateSwitchStatus;

    /**
     * @brief Protocol types supported by FDCAN.
     */
    typedef enum
    {
        FMKFDCAN_PROTOCOL_CAN2_0B = 0x00,    /**< CAN 2.0B protocol. */
        FMKFDCAN_PROTOCOL_FDCAN_NO_BRS,      /**< FDCAN protocol without bit rate switching. */
        FMKFDCAN_PROTOCOL_FDCAN_BRS,         /**< FDCAN protocol with bit rate switching. */

        FMKFDCAN_PROTOCOL_FDCAN_NB,          /**< Total number of supported protocols. */
    } t_eFMKFDCAN_ProtocolType;

    /**
     * @brief Identifier types for FDCAN frames.
     */
    typedef enum
    {
        FMKFDCAN_IDTYPE_STANDARD = 0x00, /**< Standard identifier (11-bit). */
        FMKFDCAN_IDTYPE_EXTENDED,        /**< Extended identifier (29-bit). */

        FMKFDCAN_IDTYPE_NB,              /**< Total number of identifier types. */
    } t_eFMKFDCAN_IdentifierType;

    /**
     * @brief Frame purposes for FDCAN.
     */
    typedef enum
    {
        FMKFDCAN_FRAME_PURPOSE_REMOTE = 0x00, /**< Remote frame request. */
        FMKFDCAN_FRAME_PURPOSE_DATA,          /**< Data frame. */

        FMKFDCAN_FRAME_PURPOSE_NB,            /**< Total number of frame purposes. */
    } t_eFMKFDCAN_FramePurpose;

    //-----------------------------ENUM TYPES-----------------------------//
    
    /* CAUTION : Automatic generated code section for Enum: Start */

    /* CAUTION : Automatic generated code section for Enum: End */

    //-----------------------------TYPEDEF TYPES---------------------------//
    
    //-----------------------------STRUCT TYPES---------------------------//
    /* CAUTION : Automatic generated code section for Structure: Start */

    /* CAUTION : Automatic generated code section for Structure: End */
    /**
    * @brief Configuration structure for the FDCAN driver node.
    */
    typedef struct
    {
        t_eFMKFDCAN_ClockKernelDiv clockDivider_e; /**< Clock divider configuration for FDCAN kernel clock. */
        t_eFMKFDCAN_ProtocolType ProtocolUse_e;   /**< Protocol type to be used (CAN 2.0B, FDCAN with or without BRS). */
        t_eFMKFDCAN_Baudrate FrameBaudrate_e;     /**< Baud rate configuration for arbitration phase. */
        t_eFMKFDCAN_Baudrate DataBaudrate_e;      /**< Baud rate configuration for data phase. */
        t_eFMKFDCAN_HwTxQueueType QueueType_e;    /**< Hardware transmission queue type (normal or FIFO). */
        t_eFMKFDCAN_FifoOpeMode FifoMode_e;       /**< FIFO operation mode (overwrite or blocking). */
    } t_sFMKFDCAN_DrvNodeCfg;
    // ********************************************************************
    // *                      Prototypes
    // ********************************************************************
        
    // ********************************************************************
    // *                      Variables
    // ********************************************************************
    /* CAUTION : Automatic generated code section for Variable: Start */

    /* CAUTION : Automatic generated code section for Variable: End */
    //********************************************************************************
    //                      Public functions - Prototyupes
    //********************************************************************************

#endif // FMKCDA_CONFIGPRIVATE_H_INCLUDED           
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
