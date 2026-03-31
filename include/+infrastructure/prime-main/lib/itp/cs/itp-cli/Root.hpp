#pragma once

#include "Config.hpp"
#include "DebugLevel.hpp"
#include "Callback.hpp"
#include "Endpoint.hpp"
#include "SerialPortNative.hpp"
#include "Frame.hpp"

#include <functional>

namespace itp
{

	public ref class Root
	{

	private: // Private types
		typedef Root self_type;

	public: // Construction
		Root();
		~Root();
		!Root();

	public: // Public methods
		void Poll(itp_time_t time);
		void DisposeRoot(void);
		void Reinitialize(void);
		ErrorCode RequestConnect(itp_time_t time, OnResult^ on_result);
		ErrorCode ConnectLocalNode(itp_time_t time, Endpoint^ endpoint, uint8_t address, OnConnect^ on_connect);
		ErrorCode ConnectRemoteNode(itp_time_t time, uint8_t host, uint8_t address, String^ path, OnConnect^ on_connect);
		ErrorCode ConnectRemoteEndpoint(itp_time_t time, uint8_t address, String^ path, OnConnectEndpoint^ on_connect);
		ErrorCode GetFreeAddress(uint8_t% address);
		void SetOnCreate(OnCreate^ on_create);
		void SetOnConnect(OnConnect^ on_connect);
		void SetOnReset(OnSignal^ on_reset);
		void SetOnUpdateFirmware(OnSignal^ on_update_firmware);
		ErrorCode SetParent(Endpoint^ endpoint, OnError^ on_error);
		ErrorCode SetConnectionListener(Endpoint^ endpoint);

	public: // Command layer
		ErrorCode PushEvent(Frame^ frame);
		ErrorCode PushNotification(Frame^ frame, uint8_t address, OnResult^ on_result);
		ErrorCode PushRequest(Frame^ frame, uint8_t address, OnResponse^ on_response);
		ErrorCode PushResponse(Frame^ request, Frame^ response, OnResult^ on_result);
		ErrorCode PushResponseFor(uint8_t address, uint16_t order, Frame^ response, OnResult^ on_result);
		ErrorCode RegisterHandler(uint8_t address, uint16_t command, OnRequest^ on_request);
		ErrorCode RegisterHandler(uint16_t command, OnRequest^ on_request);
		void UnregisterHandler(uint8_t address, uint16_t command);
		void UnregisterHandler(uint16_t command);
		void UnregisterAllHandlers(void);
		ErrorCode ListenRemote(uint8_t address, OnResult^ on_result);
		ErrorCode ListenRemote(uint8_t address, System::Collections::Generic::List<uint16_t>^ commands, OnResult^ on_result);
		ErrorCode TraceRoute(uint8_t address, OnResponse^ on_response);
		ErrorCode GetChildrenAddress(uint8_t address, OnResponse^ on_response);
		ErrorCode SignalReset(uint8_t address);
		ErrorCode SignalUpdateFirmware(uint8_t address);

	public: // Friend-only methods
		void push_endpoint(Endpoint^, std::function<void(void)>);
		void erase_endpoint(itp_endpoint_tp, std::function<void(void)>);

	public: // Public properties
		property uint8_t Address {
			uint8_t get(void);
			void set(uint8_t);
		}
		property uint8_t RootAddress {
			uint8_t get(void);
		}
		#ifdef ITP_ENABLE_THREAD_SAFETY
		property Object^ Mutex {
			Object^ get(void) {
				return this->mutex_;
			}
		}
		#endif // ITP_ENABLE_THREAD_SAFETY

	private: // Private fields
		itp_root_tp c_object_;
		Endpoint^ parent_;
		System::Collections::Generic::List<Endpoint^> children_;
		System::Collections::Generic::List<Endpoint^> listeners_;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		Object^ mutex_;
		#endif // ITP_ENABLE_THREAD_SAFETY
	};
}
