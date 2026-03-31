#pragma once

#include "Config.hpp"

extern "C" {

#include <itp/endpoint.h>
#include <itp/memory.h>
#include <itp/debug.h>

}

#include <initp/tools/managed_pool.hpp>

namespace itp {

	itp_size_t endpoint_read(void*, itp_byte_t*, itp_size_t);
	itp_size_t endpoint_write(void*, const itp_byte_t*, itp_size_t);

	using namespace System;
	using namespace System::Runtime::InteropServices;

	/**
	 * \brief  ласс, позвол€ющий определить интерфейс приЄма/передачи данных.
	 *
	 * ƒл€ нормального функционировани€ достаточно определить методы \c read/write.
	 */
	public ref class Endpoint abstract {
	public:
		typedef initp::tools::managed_pool<Endpoint> Pool;
	public:
		Endpoint(void): c_object_(nullptr)
		{
			this->c_object_ = new itp_endpoint_t();
			this->c_object_->read = &endpoint_read;
			this->c_object_->write = &endpoint_write;
			this->c_object_->object = Pool::push(this);
			this->c_object_->use_checksum = 0;
		}
		virtual ~Endpoint(void) {
			Pool::erase(this->c_object_->object);
			delete this->c_object_;
		}
	public:
		virtual itp_size_t Read(array<Byte>^% data, itp_size_t size) = 0;
		virtual itp_size_t Write(array<Byte>^% data, itp_size_t length) = 0;
	public:
		property itp_endpoint_tp CObject {
			itp_endpoint_tp get() {
				return this->c_object_;
			}
		}
		property bool UseChecksum {
			bool get() {
				return (this->c_object_->use_checksum > 0) ? true : false;
			}
			void set(bool value) {
				this->c_object_->use_checksum = value ? 1 : 0;
			}
		}
	protected:
		itp_endpoint_tp c_object_;
	};

	inline itp_size_t endpoint_read(void* object, itp_byte_t* data, itp_size_t length) {
		if (!object) {
			itp_debug_print(ITP_ERRC_INTERNAL, "itp::endpoint_read> User object is null");
			return 0;
		}
		array<Byte>^ arr = gcnew array<Byte>(length);
		if (Endpoint^ endpoint = Endpoint::Pool::get(object)) {
			itp_size_t result = endpoint->Read(arr, length);
			Marshal::Copy(arr, 0, IntPtr((void*)data), result);
			return result;
		} else return 0;
	}

	inline itp_size_t endpoint_write(void* object, const itp_byte_t* data, itp_size_t length) {
		if (!object) {
			itp_debug_print(ITP_ERRC_INTERNAL, "itp::endpoint_write> User object is null");
			return 0;
		}
		array<Byte>^ arr = gcnew array<Byte>(length);
		Marshal::Copy(IntPtr((void*)data), arr, 0, length);
		if (Endpoint^ endpoint = Endpoint::Pool::get(object)) {
			return endpoint->Write(arr, length);
		} else return 0;
	}

	/**
	 * \brief  ласс удалЄнного интерфейса приЄма/передачи данных.
	 */
	public ref class RemoteEndpoint :
		public itp::Endpoint {
		typedef RemoteEndpoint self_type;
	public:
		RemoteEndpoint(itp_endpoint_tp ep) :
			itp::Endpoint() {
			this->c_object_->object = ep->object;
			this->c_object_->use_checksum = ep->use_checksum;
			this->c_object_->read = ep->read;
			this->c_object_->write = ep->write;
			itp_free(ep);
		}
	public:
		virtual itp_size_t Read(array<Byte>^% data, itp_size_t size) override {
			if (size < 1) return 0;
			itp_byte_t* buffer = (itp_byte_t*)itp_malloc(sizeof(itp_byte_t) * size, "RemoteEndpoint::Read:buffer");
			if (data) {
				itp_size_t result = this->c_object_->read(this->c_object_->object, buffer, size);
				if (result > 0) {
					Marshal::Copy(IntPtr((void*)buffer), data, 0, result);
				}
				itp_free(buffer);
				return result;
			}
			return 0;
		}
		virtual itp_size_t Write(array<Byte>^% data, itp_size_t length) override {
			if (length < 1) return 0;
			itp_byte_t* buffer = (itp_byte_t*)itp_malloc(sizeof(itp_byte_t) * length, "RemoteEndpoint::Write:buffer");
			if (data) {
				Marshal::Copy(data, 0, IntPtr((void*)buffer), length);
				itp_size_t result = this->c_object_->write(this->c_object_->object, buffer, length);
				itp_free(buffer);
				return result;
			}
			return 0;
		}
	};

}
