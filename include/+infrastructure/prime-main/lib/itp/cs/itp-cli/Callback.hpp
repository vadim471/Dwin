#pragma once

#include "Config.hpp"
#include "Types.hpp"
#include "ErrorCode.hpp"

extern "C" {

#include <itp/callback.h>

}

#include <initp/generic/function_pool.hpp>

#include <type_traits>

namespace itp {

	// Extended callback types
	typedef std::remove_pointer<itp_on_error_fp>::type itp_on_error_fe;
	typedef std::remove_pointer<itp_on_signal_fp>::type itp_on_signal_fe;
	typedef std::remove_pointer<itp_on_connect_fp>::type itp_on_connect_fe;
	typedef std::remove_pointer<itp_on_connect_endpoint_fp>::type itp_on_connect_endpoint_fe;
	typedef std::remove_pointer<itp_on_create_fp>::type itp_on_create_fe;
	typedef std::remove_pointer<itp_on_result_fp>::type itp_on_result_fe;
	typedef std::remove_pointer<itp_on_request_fp>::type itp_on_request_fe;
	typedef std::remove_pointer<itp_on_response_fp>::type itp_on_response_fe;

	// Function pool types
	typedef initp::tools::function_pool<10, itp_on_error_fe> PoolOnError;
	typedef initp::tools::function_pool<20, itp_on_signal_fe> PoolOnSignal;
	typedef initp::tools::function_pool<10, itp_on_connect_fe> PoolOnConnect;
	typedef initp::tools::function_pool<10, itp_on_connect_endpoint_fe> PoolOnConnectEndpoint;
	typedef initp::tools::function_pool<10, itp_on_create_fe> PoolOnCreate;
	typedef initp::tools::function_pool<60, itp_on_result_fe> PoolOnResult;
	typedef initp::tools::function_pool<40, itp_on_request_fe> PoolOnRequest;
	typedef initp::tools::function_pool<40, itp_on_response_fe> PoolOnResponse;

	// Delegates
	public delegate void OnError(Root^ root, uint8_t address, ErrorCode errorCode);
	public delegate void OnSignal(Root^ root);
	public delegate void OnConnect(Root^ root, uint8_t address, uint16_t error);
	public delegate void OnConnectEndpoint(Root^ root, uint8_t address, uint16_t error, RemoteEndpoint^ endpoint);
	public delegate Endpoint^ OnCreate(System::String^ path, uint16_t% error);
	public delegate void OnResult(Root^ root, uint16_t error);
	public delegate uint16_t OnRequest(Root^ root, Frame^ frame);
	public delegate void OnResponse(Root^ root, uint16_t error, Frame^ frame);

	// Managed types
	typedef System::Tuple<Root^, OnError^> OnErrorTuple;
	typedef System::Tuple<Root^, OnSignal^> OnSignalTuple;
	typedef System::Tuple<Root^, OnConnect^> OnConnectTuple;
	typedef System::Tuple<Root^, OnConnectEndpoint^> OnConnectEndpointTuple;
	typedef System::Tuple<Root^, OnCreate^> OnCreateTuple;
	typedef System::Tuple<Root^, OnResult^> OnResultTuple;
	typedef System::Tuple<Root^, OnRequest^> OnRequestTuple;
	typedef System::Tuple<Root^, OnResponse^> OnResponseTuple;

}
