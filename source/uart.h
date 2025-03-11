// // Mostly lifted from https://git.codelinaro.org/linaro/qcomlt/u-boot/-/blob/caleb/rbx-integration/drivers/serial/serial_msm_geni.c
#ifndef _UART_H_
#define _UART_H_

#include "utils.h"

#define UART_REG_ADDR ((intptr_t)(0x998000))

int uart_init(int baudrate);

/* Common SE registers */
#define GENI_INIT_CFG_REVISION      (0x0)
#define GENI_S_INIT_CFG_REVISION    (0x4)
#define SE_GENI_GENERAL_CFG             (0x10)
#define GENI_FORCE_DEFAULT_REG      (0x20)
#define GENI_OUTPUT_CTRL        (0x24)
#define GENI_CGC_CTRL           (0x28)
#define SE_GENI_STATUS          (0x40)
#define GENI_SER_M_CLK_CFG      (0x48)
#define GENI_SER_S_CLK_CFG      (0x4C)
#define GENI_CLK_CTRL_RO        (0x60)
#define GENI_IF_FIFO_DISABLE_RO     (0x64)
#define GENI_FW_REVISION_RO     (0x68)
#define GENI_FW_S_REVISION_RO       (0x6C)
#define SE_GENI_CLK_SEL         (0x7C)
#define SE_GENI_CFG_SEQ_START       (0x84)
#define SE_DMA_IF_EN            (0x004)
#define SE_GENI_CFG_REG         (0x200)
#define SE_UART_LOOPBACK_CFG            (0x22C)
#define SE_GENI_CFG_REG80       (0x240)
#define SE_UART_IO_MACRO_CTRL           (0x240)
#define SE_UART_IO3_VAL                 (0x248)
#define SE_GENI_BYTE_GRAN       (0x254)
#define SE_GENI_DMA_MODE_EN     (0x258)
#define SE_UART_TX_TRANS_CFG            (0x25C)
#define SE_GENI_TX_PACKING_CFG0     (0x260)
#define SE_GENI_TX_PACKING_CFG1     (0x264)
#define SE_UART_TX_WORD_LEN             (0x268)
#define SE_UART_TX_STOP_BIT_LEN         (0x26C)
#define SE_UART_TX_TRANS_LEN            (0x270)
#define SE_UART_RX_TRANS_CFG            (0x280)
#define SE_GENI_RX_PACKING_CFG0     (0x284)
#define SE_GENI_RX_PACKING_CFG1     (0x288)
#define SE_UART_RX_WORD_LEN             (0x28C)
#define SE_UART_RX_STALE_CNT            (0x294)
#define SE_UART_TX_PARITY_CFG           (0x2A4)
#define SE_UART_RX_PARITY_CFG           (0x2A8)
#define SE_UART_MANUAL_RFR              (0x2AC)
#define SE_GENI_M_CMD0          (0x600)
#define SE_GENI_M_CMD_CTRL_REG      (0x604)
#define SE_GENI_M_IRQ_STATUS        (0x610)
#define SE_GENI_M_IRQ_EN        (0x614)
#define M_IRQ_ENABLE                    (0x614)
#define SE_GENI_M_IRQ_CLEAR     (0x618)
#define M_CMD_ERR_STATUS                (0x624)
#define M_FW_ERR_STATUS                 (0x628)
#define SE_GENI_S_CMD0          (0x630)
#define SE_GENI_S_CMD_CTRL_REG      (0x634)
#define SE_GENI_S_IRQ_STATUS        (0x640)
#define SE_GENI_S_IRQ_EN        (0x644)
#define SE_GENI_S_IRQ_CLEAR     (0x648)
#define SE_GENI_TX_FIFOn        (0x700)
#define SE_GENI_RX_FIFOn        (0x780)
#define SE_GENI_TX_FIFO_STATUS      (0x800)
#define SE_GENI_RX_FIFO_STATUS      (0x804)
#define SE_GENI_TX_WATERMARK_REG    (0x80C)
#define SE_GENI_RX_WATERMARK_REG    (0x810)
#define SE_GENI_RX_RFR_WATERMARK_REG    (0x814)
#define SE_GENI_IOS         (0x908)
#define SE_GENI_M_GP_LENGTH     (0x910)
#define SE_GENI_S_GP_LENGTH     (0x914)
#define SE_GSI_EVENT_EN         (0xE18)
#define SE_IRQ_EN           (0xE1C)
#define SE_HW_PARAM_0           (0xE24)
#define SE_HW_PARAM_1           (0xE28)
#define SE_HW_PARAM_2           (0xE2C)
#define SE_DMA_GENERAL_CFG      (0xE30)
#define SE_DMA_DEBUG_REG0       (0xE40)
#define SE_GENI_CLK_CTRL                (0x2000)
#define SE_FIFO_IF_DISABLE              (0x2008)
#define SLAVE_MODE_EN           (BIT(3))
#define START_TRIGGER           (BIT(0))
#define QUPV3_HW_VER            (0x4)

/* GENI_OUTPUT_CTRL fields */
#define DEFAULT_IO_OUTPUT_CTRL_MSK  (GENMASK(6, 0))
#define GENI_IO_MUX_0_EN        BIT(0)
#define GENI_IO_MUX_1_EN        BIT(1)

/* GENI_CFG_REG80 fields */
#define IO1_SEL_TX          BIT(2)
#define IO2_DATA_IN_SEL_PAD2        (GENMASK(11, 10))
#define IO3_DATA_IN_SEL_PAD2        BIT(15)
#define OTHER_IO_OE         BIT(12)
#define IO2_DATA_IN_SEL     BIT(11)
#define RX_DATA_IN_SEL          BIT(8)
#define IO_MACRO_IO3_SEL        (GENMASK(7, 6))
#define IO_MACRO_IO2_SEL        BIT(5)
#define IO_MACRO_IO0_SEL        BIT(0)

/* GENI_FORCE_DEFAULT_REG fields */
#define FORCE_DEFAULT   (BIT(0))

/* GENI_CGC_CTRL fields */
#define CFG_AHB_CLK_CGC_ON      (BIT(0))
#define CFG_AHB_WR_ACLK_CGC_ON      (BIT(1))
#define DATA_AHB_CLK_CGC_ON     (BIT(2))
#define SCLK_CGC_ON         (BIT(3))
#define TX_CLK_CGC_ON           (BIT(4))
#define RX_CLK_CGC_ON           (BIT(5))
#define EXT_CLK_CGC_ON          (BIT(6))
#define PROG_RAM_HCLK_OFF       (BIT(8))
#define PROG_RAM_SCLK_OFF       (BIT(9))
#define DEFAULT_CGC_EN          (GENMASK(6, 0))

/* GENI_STATUS fields */
#define M_GENI_CMD_ACTIVE       (BIT(0))
#define S_GENI_CMD_ACTIVE       (BIT(12))

/* GENI_SER_M_CLK_CFG/GENI_SER_S_CLK_CFG */
#define SER_CLK_EN          (BIT(0))
#define CLK_DIV_MSK         (GENMASK(15, 4))
#define CLK_DIV_SHFT            (4)

/* CLK_CTRL_RO fields */

/* FIFO_IF_DISABLE_RO fields */
#define FIFO_IF_DISABLE         (BIT(0))

/* FW_REVISION_RO fields */
#define FW_REV_PROTOCOL_MSK (GENMASK(15, 8))
#define FW_REV_PROTOCOL_SHFT    (8)
#define FW_REV_VERSION_MSK  (GENMASK(7, 0))

/* GENI_CLK_SEL fields */
#define CLK_SEL_MSK     (GENMASK(2, 0))

/* SE_GENI_DMA_MODE_EN */
#define GENI_DMA_MODE_EN    (BIT(0))

/* GENI_M_CMD0 fields */
#define M_OPCODE_MSK        (GENMASK(31, 27))
#define M_OPCODE_SHFT       (27)
#define M_PARAMS_MSK        (GENMASK(26, 0))

/* GENI_M_CMD_CTRL_REG */
#define M_GENI_CMD_CANCEL   BIT(2)
#define M_GENI_CMD_ABORT    BIT(1)
#define M_GENI_DISABLE      BIT(0)

/* GENI_S_CMD0 fields */
#define S_OPCODE_MSK        (GENMASK(31, 27))
#define S_OPCODE_SHFT       (27)
#define S_PARAMS_MSK        (GENMASK(26, 0))

/* GENI_S_CMD_CTRL_REG */
#define S_GENI_CMD_CANCEL   (BIT(2))
#define S_GENI_CMD_ABORT    (BIT(1))
#define S_GENI_DISABLE      (BIT(0))

/* GENI_M_IRQ_EN fields */
#define M_CMD_DONE_EN       (BIT(0))
#define M_CMD_OVERRUN_EN    (BIT(1))
#define M_ILLEGAL_CMD_EN    (BIT(2))
#define M_CMD_FAILURE_EN    (BIT(3))
#define M_CMD_CANCEL_EN     (BIT(4))
#define M_CMD_ABORT_EN      (BIT(5))
#define M_TIMESTAMP_EN      (BIT(6))
#define M_RX_IRQ_EN     (BIT(7))
#define M_GP_SYNC_IRQ_0_EN  (BIT(8))
#define M_GP_IRQ_0_EN       (BIT(9))
#define M_GP_IRQ_1_EN       (BIT(10))
#define M_GP_IRQ_2_EN       (BIT(11))
#define M_GP_IRQ_3_EN       (BIT(12))
#define M_GP_IRQ_4_EN       (BIT(13))
#define M_GP_IRQ_5_EN       (BIT(14))
#define M_IO_DATA_DEASSERT_EN   (BIT(22))
#define M_IO_DATA_ASSERT_EN (BIT(23))
#define M_RX_FIFO_RD_ERR_EN (BIT(24))
#define M_RX_FIFO_WR_ERR_EN (BIT(25))
#define M_RX_FIFO_WATERMARK_EN  (BIT(26))
#define M_RX_FIFO_LAST_EN   (BIT(27))
#define M_TX_FIFO_RD_ERR_EN (BIT(28))
#define M_TX_FIFO_WR_ERR_EN (BIT(29))
#define M_TX_FIFO_WATERMARK_EN  (BIT(30))
#define M_SEC_IRQ_EN        (BIT(31))
#define M_COMMON_GENI_M_IRQ_EN  (GENMASK(6, 1) | \
                M_IO_DATA_DEASSERT_EN | \
                M_IO_DATA_ASSERT_EN | M_RX_FIFO_RD_ERR_EN | \
                M_RX_FIFO_WR_ERR_EN | M_TX_FIFO_RD_ERR_EN | \
                M_TX_FIFO_WR_ERR_EN)

/* GENI_S_IRQ_EN fields */
#define S_CMD_DONE_EN       (BIT(0))
#define S_CMD_OVERRUN_EN    (BIT(1))
#define S_ILLEGAL_CMD_EN    (BIT(2))
#define S_CMD_FAILURE_EN    (BIT(3))
#define S_CMD_CANCEL_EN     (BIT(4))
#define S_CMD_ABORT_EN      (BIT(5))
#define S_GP_SYNC_IRQ_0_EN  (BIT(8))
#define S_GP_IRQ_0_EN       (BIT(9))
#define S_GP_IRQ_1_EN       (BIT(10))
#define S_GP_IRQ_2_EN       (BIT(11))
#define S_GP_IRQ_3_EN       (BIT(12))
#define S_GP_IRQ_4_EN       (BIT(13))
#define S_GP_IRQ_5_EN       (BIT(14))
#define S_IO_DATA_DEASSERT_EN   (BIT(22))
#define S_IO_DATA_ASSERT_EN (BIT(23))
#define S_RX_FIFO_RD_ERR_EN (BIT(24))
#define S_RX_FIFO_WR_ERR_EN (BIT(25))
#define S_RX_FIFO_WATERMARK_EN  (BIT(26))
#define S_RX_FIFO_LAST_EN   (BIT(27))
#define S_COMMON_GENI_S_IRQ_EN  (GENMASK(5, 1) | GENMASK(13, 9) | \
                 S_RX_FIFO_RD_ERR_EN | S_RX_FIFO_WR_ERR_EN)

/*  GENI_/TX/RX/RX_RFR/_WATERMARK_REG fields */
#define WATERMARK_MSK       (GENMASK(5, 0))

/* GENI_TX_FIFO_STATUS fields */
#define TX_FIFO_WC      (GENMASK(27, 0))

/*  GENI_RX_FIFO_STATUS fields */
#define RX_LAST         (BIT(31))
#define RX_LAST_BYTE_VALID_MSK  (GENMASK(30, 28))
#define RX_LAST_BYTE_VALID_SHFT (28)
#define RX_FIFO_WC_MSK      (GENMASK(24, 0))

/* SE_GSI_EVENT_EN fields */
#define DMA_RX_EVENT_EN     (BIT(0))
#define DMA_TX_EVENT_EN     (BIT(1))
#define GENI_M_EVENT_EN     (BIT(2))
#define GENI_S_EVENT_EN     (BIT(3))

/* SE_GENI_IOS fields */
#define IO2_DATA_IN     (BIT(1))
#define RX_DATA_IN      (BIT(0))

/* SE_IRQ_EN fields */
#define DMA_RX_IRQ_EN       (BIT(0))
#define DMA_TX_IRQ_EN       (BIT(1))
#define GENI_M_IRQ_EN       (BIT(2))
#define GENI_S_IRQ_EN       (BIT(3))

/* SE_HW_PARAM_0 fields */
#define TX_FIFO_WIDTH_MSK   (GENMASK(29, 24))
#define TX_FIFO_WIDTH_SHFT  (24)
#define TX_FIFO_DEPTH_MSK   (GENMASK(21, 16))
#define TX_FIFO_DEPTH_SHFT  (16)
#define GEN_I3C_IBI_CTRL    (BIT(7))

/* SE_HW_PARAM_1 fields */
#define RX_FIFO_WIDTH_MSK   (GENMASK(29, 24))
#define RX_FIFO_WIDTH_SHFT  (24)
#define RX_FIFO_DEPTH_MSK   (GENMASK(21, 16))
#define RX_FIFO_DEPTH_SHFT  (16)

/* SE_HW_PARAM_2 fields */
#define GEN_HW_FSM_I2C      (BIT(15))

/* SE_DMA_GENERAL_CFG */
#define DMA_RX_CLK_CGC_ON   (BIT(0))
#define DMA_TX_CLK_CGC_ON   (BIT(1))
#define DMA_AHB_SLV_CFG_ON  (BIT(2))
#define AHB_SEC_SLV_CLK_CGC_ON  (BIT(3))
#define DUMMY_RX_NON_BUFFERABLE (BIT(4))
#define RX_DMA_ZERO_PADDING_EN  (BIT(5))
#define RX_DMA_IRQ_DELAY_MSK    (GENMASK(8, 6))
#define RX_DMA_IRQ_DELAY_SHFT   (6)

#define SE_DMA_TX_PTR_L     (0xC30)
#define SE_DMA_TX_PTR_H     (0xC34)
#define SE_DMA_TX_ATTR      (0xC38)
#define SE_DMA_TX_LEN       (0xC3C)
#define SE_DMA_TX_IRQ_STAT  (0xC40)
#define SE_DMA_TX_IRQ_CLR   (0xC44)
#define SE_DMA_TX_IRQ_EN    (0xC48)
#define SE_DMA_TX_IRQ_EN_SET    (0xC4C)
#define SE_DMA_TX_IRQ_EN_CLR    (0xC50)
#define SE_DMA_TX_LEN_IN    (0xC54)
#define SE_DMA_TX_FSM_RST   (0xC58)
#define SE_DMA_TX_MAX_BURST (0xC5C)

#define SE_DMA_RX_PTR_L     (0xD30)
#define SE_DMA_RX_PTR_H     (0xD34)
#define SE_DMA_RX_ATTR      (0xD38)
#define SE_DMA_RX_LEN       (0xD3C)
#define SE_DMA_RX_IRQ_STAT  (0xD40)
#define SE_DMA_RX_IRQ_CLR   (0xD44)
#define SE_DMA_RX_IRQ_EN    (0xD48)
#define SE_DMA_RX_IRQ_EN_SET    (0xD4C)
#define SE_DMA_RX_IRQ_EN_CLR    (0xD50)
#define SE_DMA_RX_LEN_IN    (0xD54)
#define SE_DMA_RX_FSM_RST   (0xD58)
#define SE_DMA_RX_MAX_BURST (0xD5C)
#define SE_DMA_RX_FLUSH     (0xD60)

/* SE_DMA_TX_IRQ_STAT Register fields */
#define TX_DMA_DONE     (BIT(0))
#define TX_EOT          (BIT(1))
#define TX_SBE          (BIT(2))
#define TX_RESET_DONE       (BIT(3))
#define TX_FLUSH_DONE       (BIT(4))
#define TX_GENI_GP_IRQ      (GENMASK(12, 5))
#define TX_GENI_CANCEL_IRQ  (BIT(14))
#define TX_GENI_CMD_FAILURE (BIT(15))
#define DMA_TX_ERROR_STATUS (TX_SBE | TX_GENI_CANCEL_IRQ | TX_GENI_CMD_FAILURE)

/* SE_DMA_RX_IRQ_STAT Register fields */
#define RX_DMA_DONE     (BIT(0))
#define RX_EOT          (BIT(1))
#define RX_SBE          (BIT(2))
#define RX_RESET_DONE       (BIT(3))
#define RX_FLUSH_DONE       (BIT(4))
#define RX_GENI_GP_IRQ      (GENMASK(12, 5))
#define RX_GENI_CANCEL_IRQ  (BIT(14))
#define RX_GENI_CMD_FAILURE (BIT(15))
#define DMA_RX_ERROR_STATUS (RX_SBE | RX_GENI_CANCEL_IRQ | RX_GENI_CMD_FAILURE)

/* DMA DEBUG Register fields */
#define DMA_TX_ACTIVE       (BIT(0))
#define DMA_RX_ACTIVE       (BIT(1))
#define DMA_TX_STATE        (GENMASK(7, 4))
#define DMA_RX_STATE        (GENMASK(11, 8))

#define DEFAULT_BUS_WIDTH   (4)

/* GSI TRE fields */
/* Packing fields */
#define GSI_TX_PACK_EN          (BIT(0))
#define GSI_RX_PACK_EN          (BIT(1))
#define GSI_PRESERVE_PACK       (BIT(2))

/* UART specific GENI registers */
#define SE_UART_LOOPBACK_CFG        (0x22C)
#define SE_GENI_CFG_REG80       (0x240)
#define SE_UART_TX_TRANS_CFG        (0x25C)
#define SE_UART_TX_WORD_LEN     (0x268)
#define SE_UART_TX_STOP_BIT_LEN     (0x26C)
#define SE_UART_TX_TRANS_LEN        (0x270)
#define SE_UART_RX_TRANS_CFG        (0x280)
#define SE_UART_RX_WORD_LEN     (0x28C)
#define SE_UART_RX_STALE_CNT        (0x294)
#define SE_UART_TX_PARITY_CFG       (0x2A4)
#define SE_UART_RX_PARITY_CFG       (0x2A8)
#define SE_UART_MANUAL_RFR      (0x2AC)

/* SE_UART_LOOPBACK_CFG */
#define NO_LOOPBACK     (0)
#define TX_RX_LOOPBACK      (0x1)
#define CTS_RFR_LOOPBACK    (0x2)
#define CTSRFR_TXRX_LOOPBACK    (0x3)

/* SE_UART_TRANS_CFG */
#define UART_TX_PAR_EN      (BIT(0))
#define UART_CTS_MASK       (BIT(1))

/* SE_UART_TX_WORD_LEN */
#define TX_WORD_LEN_MSK     (GENMASK(9, 0))

/* SE_UART_TX_STOP_BIT_LEN */
#define TX_STOP_BIT_LEN_MSK (GENMASK(23, 0))
#define TX_STOP_BIT_LEN_1   (0)
#define TX_STOP_BIT_LEN_1_5 (1)
#define TX_STOP_BIT_LEN_2   (2)

/* SE_UART_TX_TRANS_LEN */
#define TX_TRANS_LEN_MSK    (GENMASK(23, 0))

/* SE_UART_RX_TRANS_CFG */
#define UART_RX_INS_STATUS_BIT  (BIT(2))
#define UART_RX_PAR_EN      (BIT(3))

/* SE_UART_RX_WORD_LEN */
#define RX_WORD_LEN_MASK    (GENMASK(9, 0))

/* SE_UART_RX_STALE_CNT */
#define RX_STALE_CNT        (GENMASK(23, 0))

/* SE_UART_TX_PARITY_CFG/RX_PARITY_CFG */
#define PAR_CALC_EN     (BIT(0))
#define PAR_MODE_MSK        (GENMASK(2, 1))
#define PAR_MODE_SHFT       (1)
#define PAR_EVEN        (0x00)
#define PAR_ODD         (0x01)
#define PAR_SPACE       (0x02)
#define PAR_MARK        (0x03)

/* SE_UART_MANUAL_RFR register fields */
#define UART_MANUAL_RFR_EN  (BIT(31))
#define UART_RFR_NOT_READY  (BIT(1))
#define UART_RFR_READY      (BIT(0))

/* UART M_CMD OP codes */
#define UART_START_TX       (0x1)
#define UART_START_BREAK    (0x4)
#define UART_STOP_BREAK     (0x5)

/* UART S_CMD OP codes */
#define UART_START_READ         (0x1)
#define UART_PARAM          (0x1)
/* When set character with framing error is not written in RX fifo */
#define UART_PARAM_SKIP_FRAME_ERR_CHAR  (BIT(5))
/* When set break character is not written in RX fifo */
#define UART_PARAM_SKIP_BREAK_CHAR  (BIT(6))
#define UART_PARAM_RFR_OPEN     (BIT(7))

/* UART DMA Rx GP_IRQ_BITS */
#define UART_DMA_RX_PARITY_ERR  BIT(5)
#define UART_DMA_RX_FRAMING_ERR BIT(6)
#define UART_DMA_RX_ERRS    (GENMASK(6, 5))
#define UART_DMA_RX_BREAK   (GENMASK(8, 7))

#define UART_OVERSAMPLING   (32)
#define STALE_TIMEOUT       (16)
#define STALE_COUNT     (DEFAULT_BITS_PER_CHAR * STALE_TIMEOUT)
#define SEC_TO_USEC     (1000000)
#define SYSTEM_DELAY        (500) /* 500 usec */
#define STALE_DELAY_MAX     (10000) /* 10 msec */
#define DEFAULT_BITS_PER_CHAR   (10)
#define GENI_UART_NR_PORTS  (6)
#define GENI_UART_CONS_PORTS    (1)
#define DEF_FIFO_DEPTH_WORDS    (16)
#define DEF_TX_WM       (2)
#define DEF_FIFO_WIDTH_BITS (32)

#define WAKEBYTE_TIMEOUT_MSEC   (2000) /* 2 Seconds */
#define WAIT_XFER_MAX_ITER  (2)
#define WAIT_XFER_MAX_TIMEOUT_US    (150)
#define WAIT_XFER_MIN_TIMEOUT_US    (100)
#define IPC_LOG_PWR_PAGES   (10)
#define IPC_LOG_MISC_PAGES  (30)
#define IPC_LOG_TX_RX_PAGES (30)
#define DATA_BYTES_PER_LINE (32)

#define M_OPCODE_SHIFT  27
#define S_OPCODE_SHIFT  27


#endif // _UART_H_