#pragma once

#include "Callback.hpp"
#include "Storage.hpp"

extern "C" {

#include <itp/debug.h>

}

#include <boost/preprocessor/repetition/repeat.hpp>

BOOST_PP_REPEAT(10, DEFINE_FUNCTION_POOL, itp::itp_on_error_fe);
BOOST_PP_REPEAT(20, DEFINE_FUNCTION_POOL, itp::itp_on_signal_fe);
BOOST_PP_REPEAT(10, DEFINE_FUNCTION_POOL, itp::itp_on_connect_fe);
BOOST_PP_REPEAT(10, DEFINE_FUNCTION_POOL, itp::itp_on_connect_endpoint_fe);
BOOST_PP_REPEAT(10, DEFINE_FUNCTION_POOL, itp::itp_on_create_fe);
BOOST_PP_REPEAT(60, DEFINE_FUNCTION_POOL, itp::itp_on_result_fe);
BOOST_PP_REPEAT(40, DEFINE_FUNCTION_POOL, itp::itp_on_request_fe);
BOOST_PP_REPEAT(40, DEFINE_FUNCTION_POOL, itp::itp_on_response_fe);

extern "C" {

	void itp_free_user_handler(void* handler, size_t eparam) {
		using namespace itp;
		if (eparam) {
			//itp_debug_print(ITP_ERRC_TRACE, "itp_free_user_handler> Free address 0x%x", eparam);
			IStorage^ managed = IStorage::Pool::get((void*)eparam);
			if (managed) {
				managed->Free(handler, eparam);
			} else {
				itp_debug_print(ITP_ERRC_INTERNAL, "itp_free_user_handler> Failed to get object 0x%x", eparam);
			}
		}
	}

}
