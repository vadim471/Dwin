#pragma once

#include <cstdint>

namespace itp {

	public ref class Command {
	public:
		static const uint16_t SET_ADDRESS = 0x0001;
		static const uint16_t CREATE_NODE = 0x0002;
		static const uint16_t GET_CHILDREN = 0x0003;
		static const uint16_t TRACE_ROUTE = 0x0004;
		static const uint16_t RESET = 0x0005;
		static const uint16_t UPDATE_FIRMWARE = 0x0006;
		static const uint16_t LISTEN_REMOTE = 0x0007;
		static const uint16_t TRANSMIT_DATA = 0x0008;
		static const uint16_t REQUEST_DATA = 0x0009;
		static const uint16_t REQUEST_CONNECT = 0x000A;
		static const uint16_t REQUEST_DISCONNECT = 0x000B;
		static const uint16_t DEBUG_MESSAGE = 0x00A0;
	};

}
