#pragma once

#include "Types.hpp"
#include "Callback.hpp"

extern "C" {

#include <itp/debug.h>

}

#include <initp/tools/managed_pool.hpp>

namespace itp {

	ref class IStorage {
	public:
		typedef initp::tools::managed_pool<IStorage> Pool;
	public:
		enum class StorageType {
			NONE = 0,
			ON_ERROR,
			ON_SIGNAL,
			ON_CREATE,
			ON_CONNECT,
			ON_CONNECT_ENDPOINT,
			ON_RESULT,
			ON_REQUEST,
			ON_RESPONSE
		};
	public:
		IStorage(StorageType type) :
			type_(type) {}
	public:
		template<typename T>
		T^ As(void) {
			return (T^)this;
		}
	public:
		void Free(void* handler, size_t eparam) {
			Pool::erase((void*)eparam);
			if (handler) {
				switch (this->type_) {
					case StorageType::ON_ERROR:
						PoolOnError::free((itp_on_error_fp)handler);
						break;
					case StorageType::ON_SIGNAL:
						PoolOnSignal::free((itp_on_signal_fp)handler);
						break;
					case StorageType::ON_CREATE:
						PoolOnCreate::free((itp_on_create_fp)handler);
						break;
					case StorageType::ON_CONNECT:
						PoolOnConnect::free((itp_on_connect_fp)handler);
						break;
					case StorageType::ON_CONNECT_ENDPOINT:
						PoolOnConnectEndpoint::free((itp_on_connect_endpoint_fp)handler);
						break;
					case StorageType::ON_RESULT:
						PoolOnResult::free((itp_on_result_fp)handler);
						break;
					case StorageType::ON_REQUEST:
						PoolOnRequest::free((itp_on_request_fp)handler);
						break;
					case StorageType::ON_RESPONSE:
						PoolOnResponse::free((itp_on_response_fp)handler);
						break;
					default:
						itp_debug_print(ITP_ERRC_INTERNAL, "IStorage::Free> Unknown type");
						break;
				}
			}
		}
	public:
		property StorageType Type {
			StorageType get(void) {
				return this->type_;
			}
			void set(StorageType value) {
				this->type_ = value;
			}
		}
	private:
		StorageType type_;
	};

	template<typename T>
	ref class Storage :
		public IStorage {
	public:
		Storage(StorageType type, T^ object) :
			IStorage(type), object_(object) {}
	public:
		property T^ Object {
			T^ get(void) {
				return this->object_;
			}
		}
	private:
		T^ object_;
	};

	template<>
	ref class Storage<OnErrorTuple> :
		public IStorage {
	public:
		Storage(Root^ root, OnError^ handler) :
			IStorage(StorageType::ON_ERROR),
			object_(gcnew OnErrorTuple(root, handler)) {}
	public:
		property OnErrorTuple^ Object {
			OnErrorTuple^ get(void) {
				return this->object_;
			}
		}
	private:
		OnErrorTuple^ object_;
	};

	template<>
	ref class Storage<OnSignalTuple> :
		public IStorage {
	public:
		Storage(Root^ root, OnSignal^ handler) :
			IStorage(StorageType::ON_SIGNAL),
			object_(gcnew OnSignalTuple(root, handler)) {}
	public:
		property OnSignalTuple^ Object {
			OnSignalTuple^ get(void) {
				return this->object_;
			}
		}
	private:
		OnSignalTuple^ object_;
	};

	template<>
	ref class Storage<OnCreateTuple> :
		public IStorage {
	public:
		Storage(Root^ root, OnCreate^ handler) :
			IStorage(StorageType::ON_CREATE),
			object_(gcnew OnCreateTuple(root, handler)) {}
	public:
		property OnCreateTuple^ Object {
			OnCreateTuple^ get(void) {
				return this->object_;
			}
		}
	private:
		OnCreateTuple^ object_;
	};

	template<>
	ref class Storage<OnConnectTuple> :
		public IStorage {
	public:
		Storage(Root^ root, OnConnect^ handler) :
			IStorage(StorageType::ON_CONNECT),
			object_(gcnew OnConnectTuple(root, handler)) {}
	public:
		property OnConnectTuple^ Object {
			OnConnectTuple^ get(void) {
				return this->object_;
			}
		}
	private:
		OnConnectTuple^ object_;
	};

	template<>
	ref class Storage<OnConnectEndpointTuple> :
		public IStorage {
	public:
		Storage(Root^ root, OnConnectEndpoint^ handler) :
			IStorage(StorageType::ON_CONNECT_ENDPOINT),
			object_(gcnew OnConnectEndpointTuple(root, handler)) {}
	public:
		property OnConnectEndpointTuple^ Object {
			OnConnectEndpointTuple^ get(void) {
				return this->object_;
			}
		}
	private:
		OnConnectEndpointTuple^ object_;
	};

	template<>
	ref class Storage<OnResultTuple> :
		public IStorage {
	public:
		Storage(Root^ root, OnResult^ handler) :
			IStorage(StorageType::ON_RESULT),
			object_(gcnew OnResultTuple(root, handler)) {}
	public:
		property OnResultTuple^ Object {
			OnResultTuple^ get(void) {
				return this->object_;
			}
		}
	private:
		OnResultTuple^ object_;
	};

	template<>
	ref class Storage<OnRequestTuple> :
		public IStorage {
	public:
		Storage(Root^ root, OnRequest^ handler) :
			IStorage(StorageType::ON_REQUEST),
			object_(gcnew OnRequestTuple(root, handler)) {}
	public:
		property OnRequestTuple^ Object {
			OnRequestTuple^ get(void) {
				return this->object_;
			}
		}
	private:
		OnRequestTuple^ object_;
	};

	template<>
	ref class Storage<OnResponseTuple> :
		public IStorage {
	public:
		Storage(Root^ root, OnResponse^ handler) :
			IStorage(StorageType::ON_RESPONSE),
			object_(gcnew OnResponseTuple(root, handler)) {}
	public:
		property OnResponseTuple^ Object {
			OnResponseTuple^ get(void) {
				return this->object_;
			}
		}
	private:
		OnResponseTuple^ object_;
	};

}
