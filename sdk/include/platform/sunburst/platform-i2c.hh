#pragma once
#include <cdefs.h>
#include <stdint.h>

static constexpr uint32_t I2cDevClockHz = CPU_TIMER_HZ;

/**
 * Specification times (Table 10) in nanoseconds for each bus mode.
 */
static const uint16_t SpcThigh[]  = {4000u, 600u, 260u};
static const uint16_t SpcTlow[]   = {4700u, 1300u, 150u};
static const uint16_t SpcThdSta[] = {4000u, 600u, 260u};
static const uint16_t SpcTsuSta[] = {4700u, 600u, 260u};
static const uint16_t SpcThdDat[] = {5000u, 1u, 1u};
static const uint16_t SpcTsuDat[] = {250u, 100u, 50u};
static const uint16_t SpcTBuf[]   = {4700u, 1300u, 500u};
static const uint16_t SpcTsuSto[] = {4000u, 600u, 260u};

/**
 * Performs a 32-bit integer unsigned division, rounding up. The bottom
 * 16 bits of the result are then returned.
 *
 * As usual, a divisor of 0 is still Undefined Behavior.
 */
static constexpr uint16_t round_up_divide(const uint32_t A, const uint32_t B)
{
	const uint32_t Result = ((A - 1) / B) + 1;
	return static_cast<uint16_t>(Result);
}

enum class OpenTitanI2cIntr
{
	// host mode interrupt: asserted whilst the FMT FIFO level is below the low
	// threshold. This is a level status interrupt.
	FmtThreshold,
	// host mode interrupt: asserted whilst the RX FIFO level is above the high
	// threshold. This is a level status interrupt.
	RxThreshold,
	// target mode interrupt: asserted whilst the ACQ FIFO level is above the
	// high threshold. This is a level status interrupt.
	AcqThreshold,
	// host mode interrupt: raised if the RX FIFO has overflowed.
	RxOverflow,
	// host mode interrupt: raised if there is no ACK in response to an address
	// or data
	Nak,
	// host mode interrupt: raised if the SCL line drops early (not supported
	// without clock synchronization).
	SclInterference,
	// host mode interrupt: raised if the SDA line goes low when host is trying
	// to assert high
	SdaInterference,
	// host mode interrupt: raised if target stretches the clock beyond the
	// allowed timeout period
	StretchTimeout,
	// host mode interrupt: raised if the target does not assert a constant
	// value of SDA during transmission.
	SdaUnstable,
	// host and target mode interrupt. In host mode, raised if the host issues a
	// repeated START or terminates the transaction by issuing STOP. In target
	// mode, raised if the external host issues a STOP or repeated START.
	CmdComplete,
	// target mode interrupt: raised if the target is stretching clocks for a
	// read command. This is a level status interrupt.
	TxStretch,
	// target mode interrupt: asserted whilst the TX FIFO level is below the low
	// threshold. This is a level status interrupt.
	TxThreshold,
	// target mode interrupt: raised if the target is stretching clocks due to
	// full ACQ FIFO or zero count in TARGET_ACK_CTRL.NBYTES (if enabled). This
	// is a level status interrupt.
	AcqFull,
	// target mode interrupt: raised if STOP is received without a preceding
	// NACK during an external host read.
	UnexpStop,
	// target mode interrupt: raised if the host stops sending the clock during
	// an ongoing transaction.
	HostTimeout,
};

static constexpr uint32_t interrupt_bit(const OpenTitanI2cIntr Interrupt)
{
	return 1 << static_cast<uint32_t>(Interrupt);
};

/**
 * A Simple Driver for the OpenTitan's I2C.
 *
 * Documentation source can be found at:
 * https://github.com/lowRISC/opentitan/tree/9ddf276c64e2974ed8e528e8b2feb00b977861de/hw/ip/i2c
 */

struct OpenTitanI2c
{
	// Interrupt State Register
	uint32_t intrState;
	// Interrupt Enable Register
	uint32_t intrEnable;
	// Interrupt Test Register
	uint32_t intrTest;
	// Alert Test Register
	uint32_t alertTest;
	// I2C Control Register
	uint32_t ctrl;
	// I2C Live Status Register for Host and Target modes
	uint32_t status;
	// I2C Read Data
	uint32_t rData;
	// I2C Host Format Data
	uint32_t fData;
	// I2C FIFO control register
	uint32_t fifoCtrl;
	// Host mode FIFO configuration
	uint32_t hostFifoConfig;
	// Target mode FIFO configuration
	uint32_t targetFifoConfig;
	// Host mode FIFO status register
	uint32_t hostFifoStatus;
	// Target mode FIFO status register
	uint32_t targetFifoStatus;
	// I2C Override Control Register
	uint32_t ovrd;
	// Oversampled RX values
	uint32_t val;
	// Detailed I2C Timings (directly corresponding to table 10 in the I2C
	// Specification).
	uint32_t timing0;
	// Detailed I2C Timings (directly corresponding to table 10 in the I2C
	// Specification).
	uint32_t timing1;
	// Detailed I2C Timings (directly corresponding to table 10 in the I2C
	// Specification).
	uint32_t timing2;
	// Detailed I2C Timings (directly corresponding to table 10, in the I2C
	// Specification).
	uint32_t timing3;
	// Detailed I2C Timings (directly corresponding to table 10, in the I2C
	// Specification).
	uint32_t timing4;
	// I2C clock stretching timeout control.
	uint32_t timeoutCtrl;
	// I2C target address and mask pairs
	uint32_t targetId;
	// I2C target acquired data
	uint32_t acqdata;
	// I2C target transmit data
	uint32_t txdata;
	// I2C host clock generation timeout value (in units of input clock
	// frequency).
	uint32_t hostTimeoutCtrl;
	// I2C target internal stretching timeout control.
	uint32_t targetTimeoutCtrl;
	// Number of times the I2C target has NACK'ed a new transaction since the
	// last read of this register.
	uint32_t targetNackCount;
	// Timeout in Host-Mode for an unhandled NACK before hardware automatically
	// ends the transaction.
	uint32_t targetAckCtrl;

	// Status Register Values
	enum : uint32_t
	{
		// Host mode FMT FIFO is full
		StatusFmtFull = 1 << 0,
		// Host mode RX FIFO is full
		StatusRxFull = 1 << 1,
		// Host mode FMT FIFO is empty
		StatusFmtEmpty = 1 << 2,
		// Host functionality is idle. No Host transaction is in progress
		StatusHostIdle = 1 << 3,
		// Target functionality is idle. No Target transaction is in progress
		StatusTargetIdle = 1 << 4,
		// Host mode RX FIFO is empty
		StatusRxEmpty = 1 << 5,
		// Target mode TX FIFO is full
		StatusTxFull = 1 << 6,
		// Target mode receive FIFO is full
		StatusAcqFull = 1 << 7,
		// Target mode TX FIFO is empty
		StatusTxEmpty = 1 << 8,
		// Target mode receive FIFO is empty
		StatusAcqEmpty = 1 << 9,
		// Target mode stretching at (N)ACK phase due to zero count in
		// TARGET_ACK_CTRL.NBYTES
		StatusAckCtrlStretch = 1 << 10,
	};

	enum : uint32_t
	{
		// Issue a START condition before transmitting BYTE.
		FDataStart = 1 << 8,
		// Issue a STOP condition after this operation
		FDataStop = 1 << 9,
		// Read BYTE bytes from I2C. (256 if BYTE==0)
		FDataReadB = 1 << 10,
		// Do not NACK the last byte read, let the read
		// operation continue
		FDataRCont = 1 << 11,
		// Do not signal an exception if the current byte is not ACK’d
		FDataNAkOk = 1 << 12,
	};

	enum : uint32_t
	{
		// RX fifo reset. Write 1 to the register resets RX_
		FifoCtrlRxRst = 1 << 0,
		// FMT fifo reset. Write 1 to the register resets FMT_FIFO. Read returns
		// 0
		FifoCtrlFmtRst = 1 << 1,
		// ACQ FIFO reset. Write 1 to the register resets it. Read returns 0
		FifoCtrlAcqRst = 1 << 7,
		// TX FIFO reset. Write 1 to the register resets it. Read returns 0
		FifoCtrlTxRst = 1 << 8,
	};

	void reset_fifos() volatile
	{
		fifoCtrl =
		  (FifoCtrlRxRst | FifoCtrlFmtRst | FifoCtrlAcqRst | FifoCtrlTxRst);
	}

	void set_host_mode() volatile
	{
		ctrl = 1;
	}

	/**
	 * Set the I2C timing parameters appropriately for the given bit rate.
	 * Distilled from:
	 * https://opentitan.org/book/hw/ip/i2c/doc/programmers_guide.html
	 */
	void set_speed(const uint32_t SpeedKhz) volatile
	{
		// We must round up the system clock frequency to lengthen intervals.
		constexpr uint32_t SysclkKhz = (I2cDevClockHz + 999) / 1000;
		// We want to underestimate the clock period, to lengthen the timings.
		uint32_t clkPeriod = (1000 * 1000) / SysclkKhz;

		// Decide which bus mode this represents
		uint32_t mode = (SpeedKhz > 100u) + (SpeedKhz > 400u);

		// Calculation of timing parameters
		uint16_t tHigh =
		  round_up_divide(SpcThigh[mode], clkPeriod);              // Spec. min.
		uint16_t tLow = round_up_divide(SpcTlow[mode], clkPeriod); // Spec. min.
		uint16_t tFall =
		  round_up_divide(20 * 3 / 5, clkPeriod); // Spec. min. 3.3V
		uint16_t tRise = round_up_divide(120, clkPeriod);
		// Setup and Hold times for Start
		uint16_t thdSta = round_up_divide(SpcThdSta[mode], clkPeriod);
		uint16_t tsuSta = round_up_divide(SpcTsuSta[mode], clkPeriod);
		// Setup and Hold times for Data
		uint16_t thdDat = round_up_divide(SpcThdDat[mode], clkPeriod);
		uint16_t tsuDat = round_up_divide(SpcTsuDat[mode], clkPeriod);
		uint16_t tBuf   = round_up_divide(SpcTBuf[mode], clkPeriod);
		uint16_t tsuSto = round_up_divide(SpcTsuSto[mode], clkPeriod);

		// Prevent counters underflowing
		if (tLow < thdDat + 1u)
			tLow = thdDat + 1u;
		if (tBuf < tsuSta + 1u)
			tBuf = tsuSta + 1u;

		timing0 = (tLow << 16) | tHigh;
		timing1 = (tFall << 16) | tRise;
		timing2 = (thdSta << 16) | tsuSta;
		timing3 = (thdDat << 16) | tsuDat;
		timing4 = (tBuf << 16) | tsuSto;
	}

	void blocking_write(const uint32_t Fmt) volatile
	{
		while (0 != (StatusFmtFull & status)) {}
		fData = Fmt;
	}

	auto fmt_empty() volatile -> bool
	{
		return 0 != (StatusFmtEmpty & status);
	}

	void write(const uint8_t  Addr7,
	           const uint8_t  data[],
	           const uint32_t NumBytes,
	           const bool     SkipStop) volatile
	{
		if (NumBytes == 0)
			return;
		blocking_write(FDataStart | (Addr7 << 1) | 0u);
		for (uint32_t i = 0; i < NumBytes - 1; ++i)
		{
			blocking_write(data[i]);
		}
		blocking_write((SkipStop ? 0u : FDataStop) | data[NumBytes - 1]);
	}

	[[nodiscard]] bool
	read(const uint8_t Addr7, uint8_t buf[], const uint32_t NumBytes) volatile
	{
		for (uint32_t idx = 0; idx < NumBytes; idx += UINT8_MAX)
		{
			blocking_write(FDataStart | (Addr7 << 1) | 1u);
			while (!fmt_empty()) {}
			if (interrupt_asserted(OpenTitanI2cIntr::Nak))
			{
				clear_interrupt(OpenTitanI2cIntr::Nak);
				return false;
			}
			uint32_t bytesRemaining = NumBytes - idx;
			bool     lastChunk      = UINT8_MAX >= bytesRemaining;
			uint8_t  chunkSize =
              lastChunk ? static_cast<uint8_t>(bytesRemaining) : UINT8_MAX;

			blocking_write((lastChunk ? FDataStop : 0) | FDataReadB |
			               chunkSize);
			while (!fmt_empty()) {}

			for (uint32_t chunkIdx = 0; chunkIdx < chunkSize; ++chunkIdx)
			{
				buf[idx + chunkIdx] = rData;
			}
		}
		return true;
	}

	[[nodiscard]] bool
	interrupt_asserted(const OpenTitanI2cIntr Interrupt) volatile
	{
		return 0 != (intrState & interrupt_bit(Interrupt));
	}

	void clear_interrupt(const OpenTitanI2cIntr Interrupt) volatile
	{
		intrState = intrState & ~interrupt_bit(Interrupt);
	}

	void enable_interrupt(const OpenTitanI2cIntr Interrupt) volatile
	{
		intrEnable = intrEnable | interrupt_bit(Interrupt);
	}

	void disable_interrupt(const OpenTitanI2cIntr Interrupt) volatile
	{
		intrEnable = intrEnable & ~interrupt_bit(Interrupt);
	}

	void set_host_thresholds(const uint16_t FmtThresh,
	                         const uint16_t RxThresh) volatile
	{
		hostFifoConfig = (FmtThresh & 0xfff) << 16 | (RxThresh & 0xfff);
	}
};
