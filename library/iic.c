/*
Copyright (c) 2023 Eindhoven University of Technology

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "iic.h"
#include "arm_shared_memory_system.h"
#include "log.h"
#include <platform.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <xiic_l.h>

#include <unistd.h>

#define IIC_TIMEOUT 5
typedef enum {
  IIC_IDLE = 0,
  IIC_ADDRESS = 1,
  IIC_READ = 2,
  IIC_WRITE = 3

} IICState;

typedef struct IICHandle {
  arm_shared mem_handle;
  volatile uint32_t *ptr;

  // Register interface for slave mode.
  uint32_t *register_map;
  uint32_t register_map_length;

  uint8_t saddr;
  uint32_t selected_register;
  uint32_t new_val;
  uint32_t recv_cnt;
  IICState state;
  int addressed;
} IICHandle;

static IICHandle iic_handles[NUM_IICS] = {
    {.ptr = NULL,

     .saddr = 0,
     .register_map = NULL,
     .register_map_length = 0,
     .selected_register = 0,
     .state = IIC_IDLE,
     .addressed = 0},
};

#define IIC_STOP 0x00
#define IIC_REPEATED_START 0x01

#define IIC_DGIER_OFFSET 0x1C    /**< Global Interrupt Enable Register */
#define IIC_IISR_OFFSET 0x20     /**< Interrupt Status Register */
#define IIC_IIER_OFFSET 0x28     /**< Interrupt Enable Register */
#define IIC_RESETR_OFFSET 0x40   /**< Reset Register */
#define IIC_CR_REG_OFFSET 0x100  /**< Control Register */
#define IIC_SR_REG_OFFSET 0x104  /**< Status Register */
#define IIC_DTR_REG_OFFSET 0x108 /**< Data Tx Register */
#define IIC_DRR_REG_OFFSET 0x10C /**< Data Rx Register */
#define IIC_ADR_REG_OFFSET 0x110 /**< Address Register */
#define IIC_TFO_REG_OFFSET 0x114 /**< Tx FIFO Occupancy */
#define IIC_RFO_REG_OFFSET 0x118 /**< Rx FIFO Occupancy */
#define IIC_TBA_REG_OFFSET 0x11C /**< 10 Bit Address reg */
#define IIC_RFD_REG_OFFSET 0x120 /**< Rx FIFO Depth reg */
#define IIC_GPO_REG_OFFSET 0x124 /**< Output Register */

#define IIC_CR_ENABLE_DEVICE_MASK 0x00000001  /**< Device enable = 1 */
#define IIC_CR_TX_FIFO_RESET_MASK 0x00000002  /**< Transmit FIFO reset=1 */
#define IIC_CR_MSMS_MASK 0x00000004           /**< Master starts Txing=1 */
#define IIC_CR_DIR_IS_TX_MASK 0x00000008      /**< Dir of Tx. Txing=1 */
#define IIC_CR_NO_ACK_MASK 0x00000010         /**< Tx Ack. NO ack = 1 */
#define IIC_CR_REPEATED_START_MASK 0x00000020 /**< Repeated start = 1 */
#define IIC_CR_GENERAL_CALL_MASK 0x00000040   /**< Gen Call enabled = 1 */

#define IIC_INTR_ARB_LOST_MASK 0x00000001 /**< 1 = Arbitration lost */
#define IIC_INTR_TX_ERROR_MASK 0x00000002 /**< 1 = Tx error/msg complete */
#define IIC_INTR_TX_EMPTY_MASK 0x00000004 /**< 1 = Tx FIFO/reg empty */
#define IIC_INTR_RX_FULL_MASK 0x00000008  /**< 1 = Rx FIFO/reg=OCY level */
#define IIC_INTR_BNB_MASK 0x00000010      /**< 1 = Bus not busy */
#define IIC_INTR_AAS_MASK 0x00000020      /**< 1 = When addr as slave */
#define IIC_INTR_NAAS_MASK 0x00000040     /**< 1 = Not addr as slave */
#define IIC_INTR_TX_HALF_MASK 0x00000080  /**< 1 = Tx FIFO half empty */
#define IIC_SR_BUS_BUSY_MASK 0x00000004   /**< 1 = Bus is busy */
#define IIC_SR_RX_FIFO_EMPTY 0x00000040
#define IIC_REG_SOFT_RESET (0x40)
#define IIC_SR_MSTR_RDING_SLAVE_MASK 0x00000008

void iic_init(const iic_index_t iic) {
  if (!(iic >= IIC0 && iic < NUM_IICS)) {
    pynq_error("invalid IIC %d, must be 0..%d\n", iic, NUM_IICS);
  }
  if (iic == IIC0) {
    iic_handles[iic].ptr =
        arm_shared_init(&((iic_handles[iic].mem_handle)), axi_iic_0, 4096);
  } else if (iic == IIC1) {
    iic_handles[iic].ptr =
        arm_shared_init(&((iic_handles[iic].mem_handle)), axi_iic_1, 4096);
  }
  // Reset
  (iic_handles[iic].ptr[IIC_REG_SOFT_RESET / 4]) = 0xA;
  usleep(1000);
}

void iic_destroy(const iic_index_t iic) {
  if (!(iic >= IIC0 && iic < NUM_IICS)) {
    pynq_error("invalid IIC %d, must be 0..%d-1\n", iic, NUM_IICS);
  }
  if (iic_handles[iic].ptr == NULL) {
    pynq_error("IIC%d has not been initialized.\n", iic);
  }
  arm_shared_close(&((iic_handles[iic].mem_handle)));
  iic_handles[iic].ptr = NULL;
}

bool iic_set_slave_mode(const iic_index_t iic, const uint8_t addr,
                        uint32_t *register_map, const uint32_t rm_length) {
  if (!(iic >= IIC0 && iic < NUM_IICS)) {
    pynq_error("invalid IIC %d, must be 0..%d-1\n", iic, NUM_IICS);
  }
  if (iic_handles[iic].ptr == NULL) {
    pynq_error("IIC%d has not been initialized.\n", iic);
  }
  (iic_handles[iic].saddr) = addr;
  (iic_handles[iic].ptr[IIC_ADR_REG_OFFSET / 4]) = addr << 1;
  uint32_t ctr_reg = (iic_handles[iic].ptr[IIC_CR_REG_OFFSET / 4]);
  // Clear the master bit.
  ctr_reg &= ~(IIC_CR_MSMS_MASK);
  // Enable IIC
  ctr_reg |= IIC_CR_ENABLE_DEVICE_MASK;

  (iic_handles[iic].ptr[IIC_CR_REG_OFFSET / 4]) = ctr_reg;
  (iic_handles[iic].ptr[IIC_RFD_REG_OFFSET / 4]) = 0x0;

  iic_handles[iic].register_map = register_map;
  iic_handles[iic].register_map_length = rm_length;

  return true;
}

static inline void iic_clear_isr_mask(const iic_index_t iic, uint32_t mask) {

  (iic_handles[iic].ptr[IIC_IISR_OFFSET / 4]) =
      (iic_handles[iic].ptr[IIC_IISR_OFFSET / 4]) & mask;
}

static void iic_flush_tx_fifo(const iic_index_t iic) {
  IICHandle *handle = &(iic_handles[iic]);
  uint32_t reg = handle->ptr[IIC_CR_REG_OFFSET / 4];
  handle->ptr[IIC_CR_REG_OFFSET / 4] = reg | IIC_CR_TX_FIFO_RESET_MASK;
  handle->ptr[IIC_CR_REG_OFFSET / 4] = reg;
}

static void iic_tx_error_handler(const iic_index_t iic) {
  IICHandle *handle = &(iic_handles[iic]);
  iic_flush_tx_fifo(iic);
  iic_clear_isr_mask(iic, IIC_INTR_RX_FULL_MASK | IIC_INTR_TX_HALF_MASK |
                              IIC_INTR_TX_ERROR_MASK | IIC_INTR_TX_EMPTY_MASK);

  uint32_t reg = handle->ptr[IIC_CR_REG_OFFSET / 4];
  handle->ptr[IIC_CR_REG_OFFSET / 4] = reg & ~IIC_CR_MSMS_MASK;
}
static void iic_slave_master_write(const iic_index_t iic, const uint32_t c) {
  IICHandle *handle = &(iic_handles[iic]);
  uint32_t v = (c << (handle->recv_cnt) * 8);
  handle->new_val |= v;
  handle->recv_cnt++;
  // If we have one full word, write it back to register.
  if (handle->recv_cnt == 4) {
    handle->register_map[handle->selected_register %
                         handle->register_map_length] = handle->new_val;
    // go to idle mode.
    handle->state = IIC_IDLE;
  }
}

static void iic_slave_master_read(const iic_index_t iic) {
  IICHandle *handle = &(iic_handles[iic]);
  if (handle->state == IIC_ADDRESS) {
    handle->state = IIC_WRITE;
  }
  if (handle->state == IIC_WRITE) {
    uint32_t r = (handle->register_map[handle->selected_register %
                                       handle->register_map_length]);
    uint8_t c = (r >> ((handle->recv_cnt) * 8)) & 0xFF;
    (iic_handles[iic].ptr[IIC_DTR_REG_OFFSET / 4]) = c;
    handle->recv_cnt++;
    if (handle->recv_cnt == 4) {
      // printf("1\n");
      handle->state = IIC_IDLE;
    }
    // modulo 4;
    handle->recv_cnt &= 0x03;
  }
};
static void iic_interrupt_handle(const iic_index_t iic) {
  time_t start = time(NULL);
  IICHandle *handle = &(iic_handles[iic]);
  int loop = 1;
  uint32_t sr_reg = (handle->ptr[IIC_SR_REG_OFFSET / 4]);
  do {
    time_t now = time(NULL);
    uint32_t nisr = (handle->ptr[IIC_IISR_OFFSET / 4]);
    uint32_t clear = 0;
    uint32_t isr = 0;
    isr = nisr;
    if (isr & IIC_INTR_ARB_LOST_MASK) {

      clear = IIC_INTR_ARB_LOST_MASK;
    } else if (isr & IIC_INTR_TX_ERROR_MASK) {
      iic_tx_error_handler(iic);
      handle->state = IIC_IDLE;
      clear = IIC_INTR_TX_ERROR_MASK;
    } else if (isr & IIC_INTR_RX_FULL_MASK) {
      // if there is data in outgoing fifo, flush this.
      uint8_t d = handle->ptr[IIC_DRR_REG_OFFSET / 4];

      uint32_t reg = handle->ptr[IIC_CR_REG_OFFSET / 4];
      reg &= ~IIC_CR_NO_ACK_MASK;
      handle->ptr[IIC_CR_REG_OFFSET / 4] = reg;
      switch (handle->state) {
      case IIC_IDLE:
        handle->recv_cnt = 0;
        handle->new_val = 0;
        handle->selected_register = d;
        handle->state = IIC_ADDRESS;
        break;
      case IIC_ADDRESS:
        handle->state = IIC_WRITE;
        // FALLTHROUGH
      case IIC_WRITE:
        iic_slave_master_write(iic, d);
        start = now;
        break;
      default:
        pynq_warning("unhandled");
        break;
      }

      clear = IIC_INTR_RX_FULL_MASK;
    } else if (handle->addressed && (isr & IIC_INTR_NAAS_MASK)) {
      handle->addressed = 0;

      clear = IIC_INTR_NAAS_MASK;
    } else if (!handle->addressed && (isr & IIC_INTR_AAS_MASK)) {
      handle->addressed = 1;
      clear = IIC_INTR_AAS_MASK;
    } else if (isr & IIC_INTR_BNB_MASK) {
      loop = 0;

      clear = IIC_INTR_BNB_MASK;
    } else if (isr & (IIC_INTR_TX_EMPTY_MASK | IIC_INTR_TX_HALF_MASK)) {

      if (handle->state == IIC_ADDRESS || handle->state == IIC_WRITE) {
        if (sr_reg & IIC_SR_MSTR_RDING_SLAVE_MASK) {
          iic_slave_master_read(iic);
          start = now;
        }
      }
      clear = isr & (IIC_INTR_TX_EMPTY_MASK | IIC_INTR_TX_HALF_MASK);
    }

    if ((now - start) > IIC_TIMEOUT) {
      pynq_warning("IIC timeout, resetting bus.");
      iic_reset(iic);
      iic_clear_isr_mask(iic, 0xFF);
      uint32_t ctr_reg = (handle->ptr[IIC_CR_REG_OFFSET / 4]);
      (iic_handles[iic].ptr[IIC_ADR_REG_OFFSET / 4]) = handle->saddr << 1;
      // Clear the master bit.
      ctr_reg &= ~(IIC_CR_MSMS_MASK);
      // Enable IIC
      ctr_reg |= IIC_CR_ENABLE_DEVICE_MASK;

      (handle->ptr[IIC_CR_REG_OFFSET / 4]) = ctr_reg;
      loop = 0;
    }
    //(iic_handles[iic].ptr[IIC_IISR_OFFSET / 4]) = nisr;
    iic_clear_isr_mask(iic, clear);
    sr_reg = (handle->ptr[IIC_SR_REG_OFFSET / 4]);
  } while (loop && (sr_reg & IIC_SR_BUS_BUSY_MASK));
  // iic_clear_isr_mask(iic, 0xFF);
}
void iic_slave_mode_handler(const iic_index_t iic) {

  if (!(iic >= IIC0 && iic < NUM_IICS)) {
    pynq_error("invalid IIC %d, must be 0..%d-1\n", iic, NUM_IICS);
  }
  if (iic_handles[iic].ptr == NULL) {
    pynq_error("IIC%d has not been initialized.\n", iic);
  }
  iic_interrupt_handle(iic);
  return;
}

void iic_reset(const iic_index_t iic) {
  if (!(iic >= IIC0 && iic < NUM_IICS)) {
    pynq_error("invalid IIC %d, must be 0..%d-1\n", iic, NUM_IICS);
  }
  if (iic_handles[iic].ptr == NULL) {
    pynq_error("IIC%d has not been initialized.\n", iic);
  }
  iic_handles[iic].ptr[IIC_REG_SOFT_RESET / 4] = 0x0A;
  uint32_t reg = iic_handles[iic].ptr[IIC_CR_REG_OFFSET / 4];
  iic_handles[iic].ptr[IIC_CR_REG_OFFSET / 4] =
      reg & ~IIC_CR_REPEATED_START_MASK;
}

bool iic_read_register(const iic_index_t iic, const uint8_t addr,
                       const uint8_t reg, uint8_t *data, uint16_t data_length) {
  if (!(iic >= IIC0 && iic < NUM_IICS)) {
    pynq_error("invalid IIC %d, must be 0..%d-1\n", iic, NUM_IICS);
  }
  if (iic_handles[iic].ptr == NULL) {
    pynq_error("IIC%d has not been initialized.\n", iic);
  }
  if (XIic_Send((UINTPTR)iic_handles[iic].ptr, addr, (u8 *)&reg, 1,
                XIIC_REPEATED_START) != 1) {
    return 1;
  }
  uint8_t ByteCount = XIic_Recv((UINTPTR)iic_handles[iic].ptr, addr, data,
                                data_length, XIIC_STOP);
  return (ByteCount == data_length) ? 0 : 1;
}

bool iic_write_register(const iic_index_t iic, const uint8_t addr,
                        const uint8_t reg, uint8_t *data,
                        uint16_t data_length) {
  if (!(iic >= IIC0 && iic < NUM_IICS)) {
    pynq_error("invalid IIC %d, must be 0..%d-1\n", iic, NUM_IICS);
  }
  if (iic_handles[iic].ptr == NULL) {
    pynq_error("IIC%d has not been initialized.\n", iic);
  }
  uint8_t buffer[1 + data_length];
  buffer[0] = reg;
  memcpy(&(buffer[1]), data, data_length);
  uint8_t ByteCount = XIic_Send((UINTPTR)iic_handles[iic].ptr, addr,
                                &(buffer[0]), 1 + data_length, XIIC_STOP);
  return (ByteCount == (data_length + 1)) ? 0 : 1;
}
