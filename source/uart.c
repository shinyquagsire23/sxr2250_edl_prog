// Mostly lifted from https://git.codelinaro.org/linaro/qcomlt/u-boot/-/blob/caleb/rbx-integration/drivers/serial/serial_msm_geni.c
#include "uart.h"

#include <string.h>

#define USEC_PER_SEC            1000000UL

static u32 uart_baud = 115200;

static inline u32 geni_se_get_tx_fifo_depth(long base)
{
    u32 tx_fifo_depth;

    tx_fifo_depth = ((readl(base + SE_HW_PARAM_0) & TX_FIFO_DEPTH_MSK) >>
             TX_FIFO_DEPTH_SHFT);
    return tx_fifo_depth;
}

static inline u32 geni_se_get_tx_fifo_width(long base)
{
    u32 tx_fifo_width;

    tx_fifo_width = ((readl(base + SE_HW_PARAM_0) & TX_FIFO_WIDTH_MSK) >>
             TX_FIFO_WIDTH_SHFT);
    return tx_fifo_width;
}


static bool qcom_geni_serial_poll_bit(int offset, int field, bool set)
{
    u32 reg;
    unsigned int tx_fifo_depth;
    unsigned int tx_fifo_width;
    unsigned int fifo_bits;
    unsigned long timeout_us = 10000;

    tx_fifo_depth = geni_se_get_tx_fifo_depth(UART_REG_ADDR);
    tx_fifo_width = geni_se_get_tx_fifo_width(UART_REG_ADDR);
    fifo_bits = tx_fifo_depth * tx_fifo_width;
    /*
     * Total polling iterations based on FIFO worth of bytes to be
     * sent at current baud. Add a little fluff to the wait.
     */
    timeout_us = ((fifo_bits * USEC_PER_SEC) / uart_baud) + 500;

    timeout_us = ((timeout_us / 10)+10) * 10;
    while (timeout_us) {
        reg = readl(UART_REG_ADDR + offset);
        if ((bool)(reg & field) == set)
            return true;
        //udelay(10);
        //timeout_us -= 10;
    }
    return false;
}

static void qcom_geni_serial_setup_tx(u64 base, u32 xmit_size)
{
    u32 m_cmd;

    writel(xmit_size, base + SE_UART_TX_TRANS_LEN);
    m_cmd = UART_START_TX << M_OPCODE_SHIFT;
    writel(m_cmd, base + SE_GENI_M_CMD0);
}

static inline void qcom_geni_serial_poll_tx_done()
{
    int done = 0;
    u32 irq_clear = M_CMD_DONE_EN;

    done = qcom_geni_serial_poll_bit(SE_GENI_M_IRQ_STATUS,
                     M_CMD_DONE_EN, true);
    if (!done) {
        writel(M_GENI_CMD_ABORT, UART_REG_ADDR + SE_GENI_M_CMD_CTRL_REG);
        irq_clear |= M_CMD_ABORT_EN;
        qcom_geni_serial_poll_bit(SE_GENI_M_IRQ_STATUS, M_CMD_ABORT_EN, true);
    }
    writel(irq_clear, UART_REG_ADDR + SE_GENI_M_IRQ_CLEAR);
}





void uart_putc(char ch) {
    writel(DEF_TX_WM, UART_REG_ADDR + SE_GENI_TX_WATERMARK_REG);
    qcom_geni_serial_setup_tx(UART_REG_ADDR, 1);

    qcom_geni_serial_poll_bit(SE_GENI_M_IRQ_STATUS, M_TX_FIFO_WATERMARK_EN, true);

    writel(ch, UART_REG_ADDR + SE_GENI_TX_FIFOn);
    writel(M_TX_FIFO_WATERMARK_EN, UART_REG_ADDR + SE_GENI_M_IRQ_CLEAR);

    qcom_geni_serial_poll_tx_done();
}

void uart_puts(const char* s) {
    while(*s) {
        uart_putc(*s);
        if (*s == '\n') {
            uart_putc('\r');
        }
        s++;
    }
}

int uart_init(int baudrate) {
    uart_baud = baudrate;
    return 0;
}