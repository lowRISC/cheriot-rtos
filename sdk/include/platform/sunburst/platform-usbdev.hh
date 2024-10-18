#pragma once
#include <cdefs.h>
#include <stdint.h>

/**
 * OpenTitan USB Device
 *
 * This peripheral's source and documentation can be found at:
 * https://github.com/lowRISC/opentitan/tree/ab878b5d3578939a04db72d4ed966a56a869b2ed/hw/ip/usbdev
 *
 * Rendered register documentation is served at:
 * https://opentitan.org/book/hw/ip/uart/doc/registers.html
 */
class OpenTitanUsbdev {
 public:
  /// USBDEV supports a maximum packet length of 64 bytes.
  static constexpr uint8_t MaxPacketLen = 64U;
  /// USBDEV provides 32 buffers.
  static constexpr uint8_t NumBuffers = 32U;
  /// USBDEV supports up to 12 endpoints, in each direction.
  static constexpr uint8_t MaxEndpoints = 12U;

	/**
	 * The offset from the start of the USB Device MMIO region at which
	 * packet buffer memory begins.
	 */
	static constexpr uint32_t BufferStartAddress = 0x800u;

	/**
	 * Register definitions for the OpenTitan USB device. Each register is 4
	 * bytes in length, defined sequentially with no gaps in memory.
	 *
	 * Documentation source:
	 * https://opentitan.org/book/hw/ip/uart/doc/registers.html
	 */
	uint32_t interruptState;
	uint32_t interruptEnable;
	uint32_t interruptTest;
	uint32_t alertTest;
	uint32_t usbControl;
	uint32_t endpointOutEnable;
	uint32_t endpointInEnable;
	uint32_t usbStatus;
	uint32_t availableOutBuffer;
	uint32_t availableSetupBuffer;
	uint32_t receiveBuffer;
	/* Register to enable receive SETUP transactions */
	uint32_t receiveEnableSetup;
	/* Register to enable receive OUT transactions */
	uint32_t receiveEnableOut;
	/* Register to set NAK (Not/Negated Acknowledge) after OUT transactions */
	uint32_t setNotAcknowledgeOut;
	/* Register showing ACK receival to indicate a successful IN send */
	uint32_t inSent;
	/* Registers for controlling the stalling of OUT and IN endpoints */
	uint32_t outStall;
	uint32_t inStall;
	/**
	 * IN transaction configuration registers. There is one register per
	 * endpoint for the USB device.
	 */
	uint32_t configIn[MaxEndpoints];
	/**
	 * Registers for configuring which endpoints should be treated as
	 * isochronous endpoints. This means that if the corresponding bit is set,
	 * then that no handshake packet will be sent for an OUT/IN transaction on
	 * that endpoint.
	 */
	uint32_t outIsochronous;
	uint32_t inIsochronous;
	/* Registers for configuring if endpoints data toggle on transactions */
	uint32_t outDataToggle;
	uint32_t inDataToggle;

	private:
	/**
	 * Registers to sense/drive the USB PHY pins. That is, these registers can
	 * be used to respectively read out the state of the USB device inputs and
	 * outputs, or to control the inputs and outputs from software. These
	 * registers are kept private as they are intended to be used for debugging
	 * purposes or during chip testing, and not in actual software.
	 */
	[[maybe_unused]] uint32_t phyPinsSense;
	[[maybe_unused]] uint32_t phyPinsDrive;

	public:
	/* Config register for the USB PHY pins. */
	uint32_t phyConfig;

	/* Interrupt definitions for OpenTitan's USB Device. */
	enum class UsbdevInterrupt : uint32_t
	{
		/* Interrupt asserted whilst the receive FIFO (buffer) is not empty. */
		PacketReceived = 1u << 0,
		/**
		 * Interrupt asserted when a packet was sent as part of an IN
		 * transaction, but not cleared from the `inSent` register.
		 */
		PacketSent = 1u << 1,
		/**
		 * Interrupt raised when VBUS (power supply) is lost, i.e. the link to
		 * the USB host controller has been disconnected.
		 */
		Disconnected = 1u << 2,
		/**
		 * Interrupt raised when the link is active, but a Start of Frame (SOF)
		 * packet has not been received within a given timeout threshold, which
		 * is set to 4.096 milliseconds.
		 */
		HostLost = 1u << 3,
		/**
		 * Interrupt raised when a Bus Reset condition is indicated on the link
		 * by the link being held in an SE0 state (Single Ended Zero, both lines
		 * being pulled low) for longer than 3 microseconds.
		 */
		LinkReset = 1u << 4,
		/**
		 * Interrupt raised when the link has entered the suspend state, due to
		 * being idle for more than 3 milliseconds.
		 */
		LinkSuspend = 1u << 5,
		/* Interrupt raised on link transition from suspended to non-idle. */
		LinkResume = 1u << 6,
		/* Interrupt asserted whilst the Available OUT buffer is empty.
		 */
		AvailableOutEmpty = 1u << 7,
		/* Interrupt asserted whilst the Receive buffer is full. */
		ReceiveFull = 1u << 8,
		/**
		 * Interrupt raised when the Available OUT buffer or the Available SETUP
		 * buffer overflows.
		 */
		AvailableBufferOverflow = 1u << 9,
		/* Interrupt raised when an error occurs during an IN transaction. */
		LinkInError = 1u << 10,
		/**
		 * Interrupt raised when a CRC (cyclic redundancy check) error occurs on
		 * a received packet; i.e. there was an error in transmission.
		 */
		RedundancyCheckError = 1u << 11,
		/* Interrupt raised when an invalid Packet Identifier is received. */
		PacketIdentifierError = 1u << 12,
		/* Interrupt raised when a bit stuffing violation is detected. */
		BitstuffingError = 1u << 13,
		/**
		 * Interrupt raised when the USB frame number is updated with a valid
		 * SOF (Start of Frame) packet.
		 */
		FrameUpdated = 1u << 14,
		/* Interrupt raised when VBUS (power supply) is detected. */
		Powered = 1u << 15,
		/* Interrupt raised when an error occurs during an OUT transaction. */
		LinkOutError = 1u << 16,
		/* Interrupt asserted whilst the Available SETUP buffer is empty. */
		AvailableSetupEmpty = 1u << 17,
	};

	/**
	 * Definitions of fields (and their locations) for the USB Control register
	 * (offset 0x10).
	 *
	 * https://opentitan.org/book/hw/ip/usbdev/doc/registers.html#usbctrl
	 */
	enum class UsbControlField : uint32_t
	{
		Enable           = (1u << 0),
		ResumeLinkActive = (1u << 1),
		/* Bits 2-15 are currently unused and should not be interacted with. */
		DeviceAddress = (0x7Fu << 16),
		/* Bits 23-31 are currently unused and should not be interacted with. */
	};

	/**
	 * Definitions of fields (and their locations) for the USB Status register
	 * (offset 0x1c).
	 *
	 * https://opentitan.org/book/hw/ip/usbdev/doc/registers.html#usbstat
	 */
	enum class UsbStatusField : uint32_t
	{
		Frame               = (0x7FFu << 0),
		HostLost            = (1u << 11),
		LinkState           = (0x7u << 12),
		Sense               = (1u << 15),
		AvailableOutDepth   = (0xFu << 16),
		AvailableSetupDepth = (0x7u << 20),
		AvailableOutFull    = (1u << 23),
		ReceiveDepth        = (0xFu << 24),
		/* Bits 28-29 are currently unused and should not be interacted with. */
		AvailableSetupFull = (1u << 30),
		ReceiveEmpty       = (1u << 31),
	};

	/**
	 * Definitions of fields (and their locations) for the Receive FIFO
	 * buffer register (offset 0x28).
	 *
	 * https://opentitan.org/book/hw/ip/usbdev/doc/registers.html#rxfifo
	 */
	enum class ReceiveBufferField : uint32_t
	{
		BufferId = (0x1Fu << 0),
		/* Bits 5-7 are currently unused and should not be interacted with. */
		Size = (0x7Fu << 8),
		/* Bits 15-18 are currently unused and should not be interacted with. */
		Setup      = (1u << 19),
		EndpointId = (0xFu << 20),
		/* Bits 24-31 are currently unused and should not be interacted with. */
	};

	/**
	 * Definitions of fields (and their locations) for a Config In register
	 * (where there is one such register for each endpoint). These are
	 * the registers with offsets 0x44 up to (and not including) 0x74.
	 *
	 * https://opentitan.org/book/hw/ip/usbdev/doc/registers.html#configin
	 */
	enum class ConfigInField : uint32_t
	{
		BufferId = (0x1Fu << 0),
		/* Bits 5-7 are currently unused and should not be interacted with. */
		Size = (0x7Fu << 8),
		/* Bits 15-28 are currently unused and should not be interacted with. */
		Sending = (1u << 29),
		Pending = (1u << 30),
		Ready   = (1u << 31),
	};

	/**
	 * Definitions of fields (and their locations) for the PHY Config
	 * Register (offset 0x8c).
	 *
	 * https://opentitan.org/book/hw/ip/usbdev/doc/registers.html#phy_config
	 */
	enum class PhyConfigField : uint32_t
	{
		UseDifferentialReceiver = (1u << 0),
		/* Other PHY Configuration fields are omitted. */
	};

  /**
   * Ensure that the Available OUT Buffer and Available SETUP Buffers are kept supplied with
   * buffers for packet reception. `buf_avail` specifies a bitmap of the buffers that are not
   * currently committed and the return value is the updated bitmap.
   */
  [[nodiscard]] uint64_t supply_buffers(uint64_t buf_avail) volatile {
		constexpr uint32_t SetupFullBit = static_cast<uint32_t>(UsbStatusField::AvailableSetupFull);
		constexpr uint32_t OutFullBit = static_cast<uint32_t>(UsbStatusField::AvailableOutFull);

    for (uint8_t buf_num = 0U; buf_num < NumBuffers; buf_num++) {
      if (buf_avail & (1U << buf_num)) {
        if (usbStatus & SetupFullBit) {
          if (usbStatus & OutFullBit) {
            break;
          }
          availableOutBuffer = buf_num;
        } else {
          availableSetupBuffer = buf_num;
        }
        buf_avail &= ~(1U << buf_num);
      }
    }
    return buf_avail;
  }

	/// Enable the given interrupt(s).
	void interrupt_enable(UsbdevInterrupt interrupt) volatile
	{
		interruptEnable = interruptEnable | static_cast<uint32_t>(interrupt);
	}

	/// Disable the given interrupt(s).
	void interrupt_disable(UsbdevInterrupt interrupt) volatile
	{
		interruptEnable = interruptEnable & ~static_cast<uint32_t>(interrupt);
	}

  /**
   * Initialise the USB device, ensuring that packet buffers are available for reception and that
   * the PHY has been configured. Note that at this endpoints have not been configured and the
   * device has not been connected to the USB.
   */
  [[nodiscard]] int init(uint64_t &buf_avail) volatile {
    buf_avail = supply_buffers(((uint64_t)1U << NumBuffers) - 1U);
		phyConfig = static_cast<uint32_t>(PhyConfigField::UseDifferentialReceiver);
    return 0;
  }

  /**
   * Set up the configuration of an OUT endpoint.
   */
  [[nodiscard]] int configure_out_endpoint(uint8_t ep, bool enabled, bool setup, bool iso) volatile {
    if (ep < MaxEndpoints) {
      const uint32_t epMask = 1u << ep;
      endpointOutEnable     = (endpointOutEnable & ~epMask) | (enabled ? epMask : 0u);
      receiveEnableSetup    = (receiveEnableSetup & ~epMask) | (setup ? epMask : 0U);
      receiveEnableOut      = (receiveEnableOut & ~epMask) | (enabled ? epMask : 0u);
      outIsochronous        = (outIsochronous & ~epMask) | (iso ? epMask : 0u);
      return 0;
    }
    return -1;
  }

  /**
   * Set up the configuration of an IN endpoint.
   */
  [[nodiscard]] int configure_in_endpoint(uint8_t ep, bool enabled, bool iso) volatile {
    if (ep < MaxEndpoints) {
      const uint32_t epMask = 1u << ep;
      endpointInEnable      = (endpointInEnable & ~epMask) | (enabled ? epMask : 0u);
      inIsochronous         = (inIsochronous & ~epMask) | (iso ? epMask : 0u);
      return 0;
    }
    return -1;
  }

  /**
   * Set the STALL state of the specified endpoint pair (IN and OUT).
   */
  [[nodiscard]] int set_ep_stalling(uint8_t ep, bool stalling) volatile {
    if (ep < MaxEndpoints) {
      const uint32_t epMask = 1u << ep;
      outStall              = (outStall & ~epMask) | (stalling ? epMask : 0U);
      inStall               = (inStall & ~epMask) | (stalling ? epMask : 0U);
      return 0;
    }
    return -1;
  }

  /**
   * Connect the device to the USB, indicating its presence to the USB host controller.
   * Endpoints must already have been configured at this point because traffic may be received
   * imminently.
   */
  [[nodiscard]] int connect() volatile {
		usbControl = usbControl | static_cast<uint32_t>(UsbControlField::Enable);
    return 0;
  }

  /**
   * Disconnect the device from the USB.
   */
  [[nodiscard]] int disconnect() volatile {
		usbControl = usbControl & ~static_cast<uint32_t>(UsbControlField::Enable);
    return 0;
  }

  /**
   * Indicate whether the USB device is connected (pullup enabled).
   */
	[[nodiscard]] bool connected() volatile
	{
		return (usbControl & static_cast<uint32_t>(UsbControlField::Enable));
	}

  /**
   * Set the device address on the USB; this address will have been supplied by the USB host
   * controller in the standard `SET_ADDRESS` Control Transfer.
   */
  [[nodiscard]] int set_device_address(uint8_t address) volatile {
    if (address < 0x80) {
		  constexpr uint32_t Mask = static_cast<uint32_t>(UsbControlField::DeviceAddress);
		  usbControl = (usbControl & ~Mask) | (address << 16);
      return 0;
    }
    return -1;
  }

  /**
   * Check for and return the endpoint number and buffer number of a recently-collected IN data
   * packet. The caller is responsible for reusing or releasing the buffer.
   */
  [[nodiscard]] int packet_collected(uint8_t &ep, uint8_t &buf_num) volatile {
		constexpr uint32_t BufferIdMask = static_cast<uint32_t>(ConfigInField::BufferId);
    uint32_t sent = inSent;
    // Clear first packet sent indication.
    for (ep = 0U; ep < MaxEndpoints; ep++) {
      uint32_t epMask = 1U << ep;
      if (sent & epMask) {
        // Clear the `in_sent` bit for this specific endpoint.
        inSent = epMask;
        // Indicate which buffer has been released.
        buf_num = (configIn[ep] & BufferIdMask) >> 0;
        return 0;
      }
    }
    return -1;
  }

  /**
   * Present a packet on the specified IN endpoint for collection by the USB host controller.
   */
  [[nodiscard]] int send_packet(uint8_t buf_num, uint8_t ep, const uint32_t *data, uint8_t size) volatile {
    // Transmission of Zero Length Packets is common over the USB.
    if (size) {
      usbdev_transfer((uint32_t *)buffers(buf_num), data, size, true);
    }
		constexpr uint32_t ReadyBit = static_cast<uint32_t>(ConfigInField::Ready);
    configIn[ep] = (buf_num << 0) | (size << 8);
    configIn[ep] = configIn[ep] | ReadyBit;
    return 0;
  }

  /**
   * Test for and collect the next received packet.
   */
  [[nodiscard]] int recv_packet(uint8_t &ep, uint8_t &buf_num, uint16_t &size, bool &is_setup,
                                uint32_t *data) volatile {
    if (usbStatus & static_cast<uint32_t>(UsbStatusField::ReceiveDepth)) {
      uint32_t rx = receiveBuffer;  // FIFO, single word read required.

      typedef ReceiveBufferField Reg;
      ep       = (rx & static_cast<uint32_t>(Reg::EndpointId)) >> 20;
      size     = (rx & static_cast<uint32_t>(Reg::Size)) >> 8;
      is_setup = (rx & static_cast<uint32_t>(Reg::Setup)) != 0U;
      buf_num  = rx & static_cast<uint32_t>(Reg::BufferId);
      // Reception of Zero Length Packets occurs in the Status Stage of IN Control Transfers.
      if (size) {
        usbdev_transfer(data, (uint32_t *)buffers(buf_num), size, false);
      }
      return 0;
    }
    return -1;
  }

	private:
	/**
	 * Return a pointer to the given offset within the USB device register
	 * space; this is used to access the packet buffer memory.
	 *
	 * @param bufferId The buffer number to access the packet buffer memory for
	 *
	 * @returns A volatile pointer to the buffer's memory.
	 */
	volatile uint32_t *buffers(uint8_t bufferId) volatile
	{
		const uint32_t Offset = BufferStartAddress + bufferId * MaxPacketLen;
		const uintptr_t Address = reinterpret_cast<uintptr_t>(this) + Offset;
		return reinterpret_cast<uint32_t *>(Address);
	}

	/**
	 * Perform a transfer to or from packet buffer memory. This function is
	 * hand-optimised to perform a faster, unrolled, word-based data transfer
	 * for efficiency.
	 *
	 * @param destination A pointer to transfer the source data to.
	 * @param source A pointer to the data to be transferred.
	 * @param size The size of the data pointed to by `source`.
	 * @param toDevice True if the transfer is to the device (e.g. when sending
	 * a packet), and False if not (e.g. when receiving a packet).
	 */
	static void usbdev_transfer(volatile uint32_t       *destination,
	                            const volatile uint32_t *source,
	                            uint8_t                  size,
	                            bool                     toDevice)
	{
		// Unroll word transfer. Each word transfer is 4 bytes, so we must round
		// to the closest multiple of (4 * words) when unrolling.
		constexpr uint8_t  UnrollFactor = 4u;
		constexpr uint32_t UnrollMask   = 0xF;

		// Round down to the previous multiple for unrolling
		const uint32_t  UnrollSize = (size & ~UnrollMask);
		const uint32_t *sourceEnd  = reinterpret_cast<uint32_t *>(
          reinterpret_cast<uintptr_t>(source) + UnrollSize);

		// Unrolled to mitigate loop overheads.
		// Ensure the unrolling here matches `UnrollFactor`.
		while (source < sourceEnd)
		{
			destination[0] = source[0];
			destination[1] = source[1];
			destination[2] = source[2];
			destination[3] = source[3];
			destination += UnrollFactor;
			source += UnrollFactor;
		}

		// Copy the remaining whole words.
		for (size &= UnrollMask; size >= UnrollFactor; size -= UnrollFactor)
			*destination++ = *source++;
		if (size == 0)
			return;

		// Copy trailing tail bytes, as USBDEV only supports 32-bit accesses.
		if (toDevice)
		{
			// Collect final bytes into a word.
			const volatile uint8_t *trailingBytes =
			  reinterpret_cast<const volatile uint8_t *>(source);
			uint32_t partialWord = trailingBytes[0];
			if (size > 1)
				partialWord |= trailingBytes[1] << 8;
			if (size > 2)
				partialWord |= trailingBytes[2] << 16;
			// Write the final word to the device.
			*destination = partialWord;
		}
		else
		{
			volatile uint8_t *destinationBytes =
			  reinterpret_cast<volatile uint8_t *>(destination);
			// Collect the final word from the device.
			const uint32_t TrailingBytes = *source;
			// Unpack it into final bytes.
			destinationBytes[0] = static_cast<uint8_t>(TrailingBytes);
			if (size > 1)
				destinationBytes[1] = static_cast<uint8_t>(TrailingBytes >> 8);
			if (size > 2)
				destinationBytes[2] = static_cast<uint8_t>(TrailingBytes >> 16);
		}
	}
};
