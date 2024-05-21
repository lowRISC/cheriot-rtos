#pragma once
#include <cdefs.h>
#include <stdint.h>

/**
 * A Simple Driver for the Sonata's GPIO.
 *
 * Documentation source can be found at:
 * https://github.com/lowRISC/sonata-system/blob/1a59633d2515d4fe186a07d53e49ff95c18d9bbf/doc/ip/spi.md
 *
 * Rendered documentation is served from:
 * https://lowrisc.org/sonata-system/doc/ip/spi.html
 */
struct SonataSpi
{
	uint32_t intrState;
	uint32_t intrEnable;
	uint32_t intrTest;
	uint32_t cfg;
	uint32_t control;
	uint32_t status;
	uint32_t start;
	uint32_t rxFifo;
	uint32_t txFifo;

    void init(bool cpol, bool cpha, bool msb_first, uint16_t half_clk_period) volatile
    {
      cfg = (cpol      ? 1 << 31 : 0) |
            (cpha      ? 1 << 30 : 0) |
            (msb_first ? 1 << 29 : 0) |
            half_clk_period;
    }

    void wait_idle() volatile
    {
      // Wait whilst IDLE field in STATUS is low
      while((status & 0x40000) == 0);
    }

    void tx(const uint8_t* data, uint32_t len) volatile {
      wait_idle();
      // Set TX_ENABLE
      control = 0x4;
      start = len;

      uint32_t tx_avail = 0;
      for (int i = 0;i < len; ++i) {
        if (tx_avail == 0) {
          while (tx_avail < 64) {
            // Read number of bytes in TX FIFO to calculate space available for more bytes
            tx_avail = 64 - (status & 0xff);
          }
        }

        txFifo = data[i];
        tx_avail--;
      }
    }

    void rx(uint8_t* data, uint32_t len) volatile {
      wait_idle();
      // Set RX_ENABLE
      control = 0x8;
      start = len;

      for (int i = 0;i < len; ++i) {
        // Wait for at least one byte to be available in the RX FIFO
        while (((status >> 8) & 0xff) == 0);
        data[i] = static_cast<uint8_t>(rxFifo);
      }
    }
};
