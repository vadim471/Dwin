#pragma once

#include "ErrorCode.hpp"

#include <cstdint>

namespace itp {

	public ref class Tools {
	public:
		static System::String^ GetErrorDescription(uint16_t error);
		static uint16_t CastErrorCode(ErrorCode errc);
	};

	public ref class Error {
	public:
		static const uint16_t NONE = 0x0000;
		static const uint16_t INTERNAL = 0x0001;
		static const uint16_t NULL_POINTER = 0x0002;
		static const uint16_t CRC_FAILED = 0x0003;
		static const uint16_t TRANSMIT_FAILED = 0x0004;
		static const uint16_t TIMEOUT = 0x0005;
		static const uint16_t WRONG_STATUS = 0x0006;
		static const uint16_t WRONG_ADDRESS = 0x0007;
		static const uint16_t WRONG_COMMAND = 0x0008;
		static const uint16_t WRONG_SEQUENCE = 0x0009;
		static const uint16_t IGNORED = 0x000A;
		static const uint16_t WRONG_PATH = 0x000B;
		static const uint16_t BAD_DATA = 0x000C;
		static const uint16_t NOT_CONFIGURED = 0x000D;
		static const uint16_t UNKNOWN = 0x000E;
		static const uint16_t MALLOC_FAILED = 0x000F;
		static const uint16_t NOT_IMPLEMENTED = 0x0010;

	};

}
