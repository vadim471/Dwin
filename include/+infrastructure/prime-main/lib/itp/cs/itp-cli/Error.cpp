#include "Error.hpp"
#include "Command.hpp"
#include "clix.hpp"

extern "C" {

#include <itp/error.h>

}

#include <cstdio>

namespace itp {

	System::String^ Tools::GetErrorDescription(uint16_t error) {
		const char* result = itp_get_error_description(error);
		if (!result) return gcnew System::String("");
		return clix::marshalString<clix::Encoding::E_UTF8>(result);
	}

	uint16_t Tools::CastErrorCode(ErrorCode errc) {
		return itp_cast_error_code(static_cast<itp_error_code_t>(errc));
	}

}
