#include "Root.hpp"
#include "Types.hpp"
#include "Storage.hpp"

extern "C" {

#include <itp/root.h>
#include <itp/callback.h>
#include <itp/command.h>
#include <itp/memory.h>

}

#include <initp/system/macro.hpp>

#include <msclr/lock.h>

namespace itp {

	using namespace System;
	using namespace System::Collections::Generic;

		// Handler calls

	static void call_on_error(size_t object, uint8_t address, itp_error_code_t error) {

		IStorage^ managed = IStorage::Pool::get((void*)object);

		if (!managed) {
			itp_debug_print(ITP_ERRC_NULL_POINTER, "call_on_error> No managed object found");
			return;
		}

		OnErrorTuple^ params = managed->As<Storage<OnErrorTuple>>()->Object;
		Root^ root = params->Item1;
		OnError^ on_error = params->Item2;

		if (!on_error) {
			itp_debug_print(ITP_ERRC_NULL_POINTER, "call_on_error> No on_error callback");
			return;
		}

		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(root->Mutex);
		#endif // ITP_ENABLE_THREAD_SAFETY

		return on_error(root, address, static_cast<ErrorCode>(error));
	}

	static void call_on_signal(size_t object) {

		IStorage^ managed = IStorage::Pool::get((void*)object);

		if (!managed) {
			itp_debug_print(ITP_ERRC_NULL_POINTER, "call_on_signal> No managed object found");
			return;
		}

		OnSignalTuple^ params = managed->As<Storage<OnSignalTuple>>()->Object;
		Root^ root = params->Item1;
		OnSignal^ on_signal = params->Item2;

		if (!on_signal) {
			itp_debug_print(ITP_ERRC_NULL_POINTER, "call_on_signal> No on_signal callback");
			return;
		}

		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(root->Mutex);
		#endif // ITP_ENABLE_THREAD_SAFETY

		return on_signal(root);
	}

	static itp_endpoint_tp call_on_create(size_t object, const char* path, uint16_t* error) {

		IStorage^ managed = IStorage::Pool::get((void*)object);

		if (!managed) {
			itp_debug_print(ITP_ERRC_NULL_POINTER, "call_on_create> No managed object found");
			return nullptr;
		}

		OnCreateTuple^ params = managed->As<Storage<OnCreateTuple>>()->Object;
		Root^ root = params->Item1;
		OnCreate^ on_create = params->Item2;

		if (!on_create) {
			itp_debug_print(ITP_ERRC_EP_NOT_CONFIGURED, "call_on_create> No on_create callback");
			if (error) *error = ITP_ERR_NOT_CONFIGURED;
			return nullptr;
		}

		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(root->Mutex);
		#endif // ITP_ENABLE_THREAD_SAFETY

		uint16_t result = ITP_ERR_NONE;
		String^ p = Marshal::PtrToStringAnsi(IntPtr((char*)path));
		Endpoint^ endpoint = on_create(p, result);
		p = nullptr;
		if (!result) {
			if (!endpoint) {
				if (error) *error = ITP_ERR_NULL_POINTER;
				return nullptr;
			} else {
				if (error) *error = result;
				itp_endpoint_tp ep = endpoint->CObject;
				root->push_endpoint(endpoint, nullptr);
				return ep;
			}
		} else {
			if (error) *error = result;
			return nullptr;
		}
	}

	static void call_on_connect(size_t object, uint8_t address, uint16_t error, itp_endpoint_tp endpoint) {

		IStorage^ managed = IStorage::Pool::get((void*)object);

		if (!managed) {
			itp_debug_print(ITP_ERRC_NULL_POINTER, "call_on_connect> No managed object found");
			return;
		}

		OnConnectTuple^ params = managed->As<Storage<OnConnectTuple>>()->Object;
		Root^ root = params->Item1;
		OnConnect^ on_connect = params->Item2;

		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(root->Mutex);
		#endif // ITP_ENABLE_THREAD_SAFETY

		if (error && endpoint) {
			root->erase_endpoint(endpoint, nullptr);
		}
		if (!on_connect) {
			itp_debug_print(ITP_ERRC_NULL_POINTER, "call_on_connect> No on_connect callback");
			return;
		}
		return on_connect(root, address, error);
	}

	static void call_on_connect_endpoint(size_t object, uint8_t address, uint16_t error, itp_endpoint_tp endpoint) {

		IStorage^ managed = IStorage::Pool::get((void*)object);

		if (!managed) {
			itp_debug_print(ITP_ERRC_NULL_POINTER, "call_on_connect_endpoint> No managed object found");
			return;
		}

		OnConnectEndpointTuple^ params = managed->As<Storage<OnConnectEndpointTuple>>()->Object;
		Root^ root = params->Item1;
		OnConnectEndpoint^ on_connect = params->Item2;

		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(root->Mutex);
		#endif // ITP_ENABLE_THREAD_SAFETY

		if (!on_connect) {
			itp_debug_print(ITP_ERRC_NULL_POINTER, "call_on_connect_endpoint> No on_connect callback");
			return;
		}
		return on_connect(root, address, error, nullptr);
	}

	static void call_on_result(size_t object, uint16_t error) {

		IStorage^ managed = IStorage::Pool::get((void*)object);

		if (!managed) {
			itp_debug_print(ITP_ERRC_NULL_POINTER, "call_on_result> No managed object found");
			return;
		}

		OnResultTuple^ params = managed->As<Storage<OnResultTuple>>()->Object;
		Root^ root = params->Item1;
		OnResult^ on_result = params->Item2;

		if (!on_result) {
			itp_debug_print(ITP_ERRC_NULL_POINTER, "call_on_result> No on_result callback");
			return;
		}

		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(root->Mutex);
		#endif // ITP_ENABLE_THREAD_SAFETY

		return on_result(root, error);
	}

	static uint16_t call_on_request(size_t object, itp_frame_tp frame) {

		IStorage^ managed = IStorage::Pool::get((void*)object);

		if (!managed) {
			itp_debug_print(ITP_ERRC_NULL_POINTER, "call_on_request> No managed object found");
			return ITP_ERR_NONE;
		}

		OnRequestTuple^ params = managed->As<Storage<OnRequestTuple>>()->Object;
		Root^ root = params->Item1;
		OnRequest^ on_request = params->Item2;

		if (!on_request) {
			itp_debug_print(ITP_ERRC_NULL_POINTER, "call_on_request> No on_request callback");
			return ITP_ERR_NONE;
		}

		Frame^ f = gcnew Frame(frame);

		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(root->Mutex);
		#endif // ITP_ENABLE_THREAD_SAFETY

		uint16_t result = on_request(root, f);
		f->CObject = nullptr;
		return result;
	}

	static void call_on_response(size_t object, uint16_t error, itp_frame_tp frame) {

		IStorage^ managed = IStorage::Pool::get((void*)object);

		if (!managed) {
			itp_debug_print(ITP_ERRC_NULL_POINTER, "call_on_response> No managed object found");
			return;
		}

		OnResponseTuple^ params = managed->As<Storage<OnResponseTuple>>()->Object;
		Root^ root = params->Item1;
		OnResponse^ on_response = params->Item2;

		if (!on_response) {
			itp_debug_print(ITP_ERRC_NULL_POINTER, "call_on_response> No on_response callback");
			return;
		}

		Frame^ f = gcnew Frame(frame);

		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(root->Mutex);
		#endif // ITP_ENABLE_THREAD_SAFETY

		on_response(root, error, f);
		f->CObject = nullptr;
	}

		// Construction

	Root::Root():
		c_object_(nullptr)
		#ifdef ITP_ENABLE_THREAD_SAFETY
		, mutex_(gcnew Object())
		#endif // ITP_ENABLE_THREAD_SAFETY
	{
		this->c_object_ = (itp_root_tp)itp_malloc(sizeof(itp_root_t), "itp::root:itp_root");
		if (this->c_object_) {
			itp_init_root(this->c_object_, NULL, 0);
		}
	}

	Root::~Root() {
		if (this->c_object_) {
			itp_free_root(this->c_object_);
		}
	}

	Root::!Root() {
		if (this->c_object_) {
			itp_free_root(this->c_object_);
		}
	}

		// Public methods

	void Root::Poll(itp_time_t time) {
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		itp_poll_root(this->c_object_, time);
	}

	void Root::DisposeRoot(void) {
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		itp_dispose_root(this->c_object_);
		this->children_.Clear();
		this->listeners_.Clear();
		this->parent_ = nullptr;
	}

	void Root::Reinitialize(void) {
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		itp_init_root(this->c_object_, NULL, 0);
	}

	ErrorCode Root::RequestConnect(itp_time_t time, OnResult^ on_result) {
		using namespace std::placeholders;
		itp_error_code_t error;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		if (on_result) {
			void* managed = IStorage::Pool::push(gcnew Storage<OnResultTuple>(this, on_result));
			auto f = PoolOnResult::bind(std::bind(&call_on_result, _1, _3));
			error = itp_request_connect(this->c_object_, time, f, (size_t)managed);
			if (error) {
				PoolOnResult::free(f);
				IStorage::Pool::erase(managed);
			}
		} else {
			error = itp_request_connect(this->c_object_, time, NULL, 0);
		}
		return static_cast<ErrorCode>(error);
	}

	ErrorCode Root::ConnectLocalNode(itp_time_t time, Endpoint^ endpoint, uint8_t address, OnConnect^ on_connect) {
		using namespace std::placeholders;
		if (!endpoint) {
			itp_debug_print(ITP_ERRC_INTERNAL, "itp::root::connect_local_node> Endpoint is null");
			return ErrorCode::INTERNAL;
		}
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		itp_endpoint_tp ep = endpoint->CObject;
		this->children_.Add(endpoint);
		itp_error_code_t error;
		if (on_connect) {
			void* managed = IStorage::Pool::push(gcnew Storage<OnConnectTuple>(this, on_connect));
			auto f = PoolOnConnect::bind(std::bind(&call_on_connect, _1, _3, _4, ep));
			error = itp_connect_local_node(this->c_object_, time, ep, address, f, (size_t)managed);
			if (error) {
				PoolOnConnect::free(f);
				IStorage::Pool::erase(managed);
			}
		} else {
			error = itp_connect_local_node(this->c_object_, time, ep, address, NULL, 0);
		}
		return static_cast<ErrorCode>(error);
	}

	ErrorCode Root::ConnectRemoteNode(itp_time_t time, uint8_t host, uint8_t address, String^ path, OnConnect^ on_connect) {
		using namespace std::placeholders;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		IntPtr str = Marshal::StringToHGlobalAnsi(path);
		try {
			itp_error_code_t error;
			if (on_connect) {
				void* managed = IStorage::Pool::push(gcnew Storage<OnConnectTuple>(this, on_connect));
				auto f = PoolOnConnect::bind(std::bind(&call_on_connect, _1, _3, _4, (itp_endpoint_tp)0));
				error = itp_connect_remote_node(this->c_object_, time, host, address, (char*)str.ToPointer(), f, (size_t)managed);
				if (error) {
					PoolOnConnect::free(f);
					IStorage::Pool::erase(managed);
				}
			} else {
				error = itp_connect_remote_node(this->c_object_, time, host, address, (char*)str.ToPointer(), NULL, 0);
			}
			return static_cast<ErrorCode>(error);
		} finally {
			Marshal::FreeHGlobal(str);
		}
	}

	ErrorCode Root::ConnectRemoteEndpoint(itp_time_t time, uint8_t address, String^ path, OnConnectEndpoint^ on_connect) {
		using namespace std::placeholders;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		IntPtr str = Marshal::StringToHGlobalAnsi(path);
		try {
			itp_error_code_t error;
			if (on_connect) {
				void* managed = IStorage::Pool::push(gcnew Storage<OnConnectEndpointTuple>(this, on_connect));
				auto f = PoolOnConnectEndpoint::bind(std::bind(&call_on_connect_endpoint, _1, _3, _4, _5));
				error = itp_connect_remote_endpoint(this->c_object_, time, address, (char*)str.ToPointer(), f, (size_t)managed);
				if (error) {
					PoolOnConnectEndpoint::free(f);
					IStorage::Pool::erase(managed);
				}
			} else {
				error = itp_connect_remote_endpoint(this->c_object_, time, address, (char*)str.ToPointer(), NULL, 0);
			}
			return static_cast<ErrorCode>(error);
		} finally {
			Marshal::FreeHGlobal(str);
		}
	}

	ErrorCode Root::GetFreeAddress(uint8_t% address) {
		itp_error_code_t error = ITP_ERRC_NONE;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		address = itp_get_free_address(this->c_object_, &error);
		return static_cast<ErrorCode>(error);
	}

	void Root::SetOnCreate(OnCreate^ on_create) {
		using namespace std::placeholders;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
        if (this->c_object_->on_create) {
            PoolOnCreate::free(this->c_object_->on_create);
			this->c_object_->on_create = NULL;
        }
        if (this->c_object_->on_create_eparam) {
            IStorage::Pool::erase((void*)this->c_object_->on_create_eparam);
			this->c_object_->on_create_eparam = 0;
        }
		if (on_create) {
			void* managed = IStorage::Pool::push(gcnew Storage<OnCreateTuple>(this, on_create));
			this->c_object_->on_create = PoolOnCreate::bind(&call_on_create);
			this->c_object_->on_create_eparam = (size_t)managed;
		}
	}

	void Root::SetOnConnect(OnConnect^ on_connect) {
		using namespace std::placeholders;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
        if (this->c_object_->on_connect) {
            PoolOnConnect::free(this->c_object_->on_connect);
			this->c_object_->on_connect = NULL;
        }
        if (this->c_object_->on_connect_eparam) {
            IStorage::Pool::erase((void*)this->c_object_->on_connect_eparam);
			this->c_object_->on_connect_eparam = 0;
        }
		if (on_connect) {
			void* managed = IStorage::Pool::push(gcnew Storage<OnConnectTuple>(this, on_connect));
			this->c_object_->on_connect = PoolOnConnect::bind(std::bind(&call_on_connect, _1, _3, _4, (itp_endpoint_tp)0));
			this->c_object_->on_connect_eparam = (size_t)managed;
		}
	}

	void Root::SetOnReset(OnSignal^ on_reset) {
		using namespace std::placeholders;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
        if (this->c_object_->on_reset) {
            PoolOnSignal::free(this->c_object_->on_reset);
			this->c_object_->on_reset = NULL;
        }
        if (this->c_object_->on_reset_eparam) {
            IStorage::Pool::erase((void*)this->c_object_->on_reset_eparam);
            this->c_object_->on_reset_eparam = 0;
        }
		if (on_reset) {
			void* managed = IStorage::Pool::push(gcnew Storage<OnSignalTuple>(this, on_reset));
			this->c_object_->on_reset = PoolOnSignal::bind(std::bind(&call_on_signal, _1));
			this->c_object_->on_reset_eparam = (size_t)managed;
		}
	}

	void Root::SetOnUpdateFirmware(OnSignal^ on_update_firmware) {
		throw gcnew NotImplementedException("Root::SetOnUpdateFirmware() doesn't implemented yet!");

		//TODO: fix "on_update_firmware" bindings
		/*using namespace std::placeholders;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
        if (this->c_object_->on_update_firmware) {
            PoolOnSignal::free(this->c_object_->on_update_firmware);
            this->c_object_->on_update_firmware = NULL;
        }
        if (this->c_object_->on_update_firmware_eparam) {
            IStorage::Pool::erase((void*)this->c_object_->on_update_firmware_eparam);
            this->c_object_->on_update_firmware_eparam = 0;
        }
		if (on_update_firmware) {
			void* managed = IStorage::Pool::push(gcnew Storage<OnSignalTuple>(this, on_update_firmware));
			this->c_object_->on_update_firmware = PoolOnSignal::bind(std::bind(&call_on_signal, _1));
			this->c_object_->on_update_firmware_eparam = (size_t)managed;
		}*/
	}

	ErrorCode Root::SetParent(Endpoint^ endpoint, OnError^ on_error) {
		using namespace std::placeholders;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		itp_error_code_t error = ITP_ERRC_NONE;
		if (endpoint) {
			this->parent_ = endpoint;
			if (on_error) {
				void* managed = IStorage::Pool::push(gcnew Storage<OnErrorTuple>(this, on_error));
				auto f = PoolOnError::bind(std::bind(&call_on_error, _1, _3, _4));
				error = itp_set_parent(this->c_object_, this->parent_->CObject, f, (size_t)managed);
				if (error) {
					PoolOnError::free(f);
					IStorage::Pool::erase(managed);
				}
			} else {
				error = itp_set_parent(this->c_object_, this->parent_->CObject, NULL, 0);
			}
		} else {
			if (this->parent_) {
				this->parent_ = nullptr;
				error = itp_set_parent(this->c_object_, NULL, NULL, 0);
			}
		}
		return static_cast<ErrorCode>(error);
	}

	ErrorCode Root::SetConnectionListener(Endpoint^ endpoint) {
		using namespace std::placeholders;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		if (endpoint) {
			for each(Endpoint^ it in this->listeners_) {
				if (it->CObject == endpoint->CObject) {
					return ErrorCode::NONE;
				}
			}
			itp_error_code_t errc = itp_set_connection_listener(this->c_object_, endpoint->CObject);
			if (!errc) this->listeners_.Add(endpoint);
			return static_cast<ErrorCode>(errc);
		}
		return ErrorCode::NONE;
	}

		// Command layer

	ErrorCode Root::PushEvent(Frame^ frame) {
		using namespace std::placeholders;
		itp_error_code_t error;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		error = itp_push_event(this->c_object_, frame->CObject);
		if (error) itp_free_frame(frame->CObject);
		frame->CObject = nullptr;
		return static_cast<ErrorCode>(error);
	}

	ErrorCode Root::PushNotification(Frame^ frame, uint8_t address, OnResult^ on_result) {
		using namespace std::placeholders;
		itp_error_code_t error;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		if (on_result) {
			void* managed = IStorage::Pool::push(gcnew Storage<OnResultTuple>(this, on_result));
			auto f = PoolOnResult::bind(std::bind(&call_on_result, _1, _3));
			error = itp_push_notification(this->c_object_, frame->CObject, address, f, (size_t)managed);
			if (error) {
				PoolOnResult::free(f);
				IStorage::Pool::erase(managed);
			}
		} else {
			error = itp_push_notification(this->c_object_, frame->CObject, address, NULL, 0);
		}
		if (error) itp_free_frame(frame->CObject);
		frame->CObject = nullptr;
		return static_cast<ErrorCode>(error);
	}

	ErrorCode Root::PushRequest(Frame^ frame, uint8_t address, OnResponse^ on_response) {
		using namespace std::placeholders;
		itp_error_code_t error;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		if (on_response) {
			void* managed = IStorage::Pool::push(gcnew Storage<OnResponseTuple>(this, on_response));
			auto f = PoolOnResponse::bind(std::bind(&call_on_response, _1, _3, _4));
			error = itp_push_request(this->c_object_, frame->CObject, address, f, (size_t)managed);
			if (error) {
				PoolOnResponse::free(f);
				IStorage::Pool::erase(managed);
			}
		} else {
			error = itp_push_request(this->c_object_, frame->CObject, address, NULL, 0);
		}
		if (error) itp_free_frame(frame->CObject);
		frame->CObject = nullptr;
		return static_cast<ErrorCode>(error);
	}

	ErrorCode Root::PushResponse(Frame^ request, Frame^ response, OnResult^ on_result) {
		using namespace std::placeholders;
		itp_error_code_t error;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		if (on_result) {
			void* managed = IStorage::Pool::push(gcnew Storage<OnResultTuple>(this, on_result));
			auto f = PoolOnResult::bind(std::bind(&call_on_result, _1, _3));
			error = itp_push_response(this->c_object_, request->CObject, response->CObject, f, (size_t)managed);
			if (error) {
				PoolOnResult::free(f);
				IStorage::Pool::erase(managed);
			}
		} else {
			error = itp_push_response(this->c_object_, request->CObject, response->CObject, NULL, 0);
		}
		if (error) itp_free_frame(response->CObject);
		response->CObject = nullptr;
		return static_cast<ErrorCode>(error);
	}

	ErrorCode Root::PushResponseFor(uint8_t address, uint16_t order, Frame^ response, OnResult^ on_result) {
		using namespace std::placeholders;
		itp_error_code_t error;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		if (on_result) {
			void* managed = IStorage::Pool::push(gcnew Storage<OnResultTuple>(this, on_result));
			auto f = PoolOnResult::bind(std::bind(&call_on_result, _1, _3));
			error = itp_push_response_for(this->c_object_, address, order, response->CObject, f, (size_t)managed);
			if (error) {
				PoolOnResult::free(f);
				IStorage::Pool::erase(managed);
			}
		} else {
			error = itp_push_response_for(this->c_object_, address, order, response->CObject, NULL, 0);
		}
		if (error) itp_free_frame(response->CObject);
		response->CObject = nullptr;
		return static_cast<ErrorCode>(error);
	}

	ErrorCode Root::RegisterHandler(uint8_t address, uint16_t command, OnRequest^ on_request) {
		using namespace std::placeholders;
		if (!on_request) {
			itp_debug_print(ITP_ERRC_INTERNAL, "itp::root::register_handler> Handler is null");
			return ErrorCode::NULL_POINTER;
		}
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		void* managed = IStorage::Pool::push(gcnew Storage<OnRequestTuple>(this, on_request));
		auto f = PoolOnRequest::bind(std::bind(&call_on_request, _1, _3));
		itp_error_code_t error = itp_register_handler(this->c_object_, address, command, f, (size_t)managed);
		if (error) {
			PoolOnRequest::free(f);
			IStorage::Pool::erase(managed);
		}
		return static_cast<ErrorCode>(error);
	}

	ErrorCode Root::RegisterHandler(uint16_t command, OnRequest^ on_request) {
		return this->RegisterHandler(0, command, on_request);
	}

	void Root::UnregisterHandler(uint8_t address, uint16_t command) {
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		return itp_unregister_handler(this->c_object_, address, command);
	}

	void Root::UnregisterHandler(uint16_t command) {
		return this->UnregisterHandler(0, command);
	}

	void Root::UnregisterAllHandlers(void) {
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		itp_unregister_all_handlers(this->c_object_);
	}

	ErrorCode Root::ListenRemote(uint8_t address, OnResult^ on_result) {
		using namespace std::placeholders;
		itp_error_code_t error;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		if (on_result) {
			void* managed = IStorage::Pool::push(gcnew Storage<OnResultTuple>(this, on_result));
			auto f = PoolOnResult::bind(std::bind(&call_on_result, _1, _3));
			error = itp_listen_remote(this->c_object_, address, NULL, 0, f, (size_t)managed);
			if (error) {
				PoolOnResult::free(f);
				IStorage::Pool::erase(managed);
			}
		}
		else {
			error = itp_listen_remote(this->c_object_, address, NULL, 0, NULL, 0);
		}
		return static_cast<ErrorCode>(error);
	}

	ErrorCode Root::ListenRemote(uint8_t address, System::Collections::Generic::List<uint16_t>^ commands, OnResult^ on_result) {
		using namespace std::placeholders;
		itp_error_code_t error;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY

		uint8_t length = 0;
		uint16_t* data = NULL;
		if (commands && commands->Count > 0) {
			length = (commands->Count > 122) ? 122 : (uint8_t)commands->Count;
			data = (uint16_t*)itp_malloc(sizeof(uint16_t) * length, "itp::Root::ListenRemote:commands");
			if (!data) {
				itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp::Root::ListenRemote> Memory allocation failed");
				return static_cast<ErrorCode>(ITP_ERRC_MALLOC_FAILED);
			}
			for (uint8_t i = 0; i < length; ++i) {
				data[i] = commands[i];
			}
		}

		if (on_result) {
			void* managed = IStorage::Pool::push(gcnew Storage<OnResultTuple>(this, on_result));
			auto f = PoolOnResult::bind(std::bind(&call_on_result, _1, _3));
			error = itp_listen_remote(this->c_object_, address, data, length, f, (size_t)managed);
			if (error) {
				PoolOnResult::free(f);
				IStorage::Pool::erase(managed);
			}
		}
		else {
			error = itp_listen_remote(this->c_object_, address, data, length, NULL, 0);
		}
		return static_cast<ErrorCode>(error);
	}

	ErrorCode Root::TraceRoute(uint8_t address, OnResponse^ on_response) {
		using namespace std::placeholders;
		itp_error_code_t error;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		if (on_response) {
			void* managed = IStorage::Pool::push(gcnew Storage<OnResponseTuple>(this, on_response));
			auto f = PoolOnResponse::bind(std::bind(&call_on_response, _1, _3, _4));
			error = itp_trace_route(this->c_object_, address, f, (size_t)managed);
			if (error) {
				PoolOnResponse::free(f);
				IStorage::Pool::erase(managed);
			}
		} else {
			error = itp_trace_route(this->c_object_, address, NULL, 0);
		}
		return static_cast<ErrorCode>(error);
	}

	ErrorCode Root::GetChildrenAddress(uint8_t address, OnResponse^ on_response) {
		using namespace std::placeholders;
		itp_error_code_t error;
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		if (on_response) {
			void* managed = IStorage::Pool::push(gcnew Storage<OnResponseTuple>(this, on_response));
			auto f = PoolOnResponse::bind(std::bind(&call_on_response, _1, _3, _4));
			error = itp_get_children_address(this->c_object_, address, f, (size_t)managed);
			if (error) {
				PoolOnResponse::free(f);
				IStorage::Pool::erase(managed);
			}
		} else {
			error = itp_get_children_address(this->c_object_, address, NULL, 0);
		}
		return static_cast<ErrorCode>(error);
	}

	ErrorCode Root::SignalReset(uint8_t address) {
		#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		return static_cast<ErrorCode>(itp_signal_reset(this->c_object_, address));
	}

	ErrorCode Root::SignalUpdateFirmware(uint8_t address) {
		throw gcnew NotImplementedException("Root::SignalUpdateFirmware() doesn't implemented yet!");

		//TODO: fix "itp_signal_update_firmware" bindings
		/*#ifdef ITP_ENABLE_THREAD_SAFETY
		msclr::lock lock(this->mutex_);
		#endif // ITP_ENABLE_THREAD_SAFETY
		return static_cast<ErrorCode>(itp_signal_update_firmware(this->c_object_, address));*/
	}

		// Friend-only methods

	void Root::push_endpoint(Endpoint^ endpoint, std::function<void(void)>) {
		this->children_.Add(endpoint);
	}

	void Root::erase_endpoint(itp_endpoint_tp endpoint, std::function<void(void)>) {
		for each (Endpoint^ ep in this->children_) {
			if (ep->CObject == endpoint) {
				this->children_.Remove(ep);
				return;
			}
		}
	}

		// Public properties

	uint8_t Root::Address::get(void) {
		return this->c_object_->address;
	}

	void Root::Address::set(uint8_t value) {
		this->c_object_->address = value;
	}

	uint8_t Root::RootAddress::get(void) {
		return this->c_object_->root;
	}

}
