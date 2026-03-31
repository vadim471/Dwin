#pragma once

#include "Config.hpp"
#include "Endpoint.hpp"

#include <cstring>
#include <memory>
#include <tuple>

namespace itp {

	ref class Tunnel;

	ref class TunnelEndpoint:
		public itp::Endpoint {
		typedef TunnelEndpoint self_type;
	public:
		typedef std::unique_ptr<self_type> uptr;
	public:
		TunnelEndpoint(void):
			data_(gcnew array<itp_byte_t>(ITP_TUNNEL_BUFFER_SIZE)),
			length_(0)
		{
			//itp_debug_print(ITP_ERRC_TRACE, "TunnelEndpoint> Created");
		}
		TunnelEndpoint(const TunnelEndpoint%) :
			data_(gcnew array<itp_byte_t>(ITP_TUNNEL_BUFFER_SIZE)),
			length_(0)
		{
			//itp_debug_print(ITP_ERRC_TRACE, "TunnelEndpoint> Copied");
		}
	public:
		virtual itp_size_t Read(array<Byte>^% data, itp_size_t size) override {
			if (this->length_ < 1) return this->length_;
			itp_size_t length = this->length_;
			if (size >= this->length_) {
				length = this->length_;
				Array::Copy(this->data_, data, length);
				this->length_ = 0;
			} else {
				length = size;
				Array::Copy(this->data_, data, length);
				Array::Copy(this->data_, length, this->data_, 0, this->length_ - length);
				this->length_ -= length;
			}
			return length;
		}
		virtual itp_size_t Write(array<Byte>^% data, itp_size_t length) override {
			if (length < 1) return length;
			itp_size_t max = ITP_TUNNEL_BUFFER_SIZE - (this->remote_->length_);
			if (length > max) length = max;
			Array::Copy(data, 0, this->remote_->data_, this->remote_->length_, length);
			this->remote_->length_ += length;
			return length;
		}
		property TunnelEndpoint^ Remote {
			TunnelEndpoint^ get() {
				return this->remote_;
			}
			void set(TunnelEndpoint^ endpoint) {
				this->remote_ = endpoint;
			}
		}
	private:
		array<itp_byte_t>^ data_;
		itp_size_t length_;
		TunnelEndpoint^ remote_;
	};

	public ref class Tunnel {
	public:
		static Tuple<Endpoint^, Endpoint^>^ Join(void) {
			TunnelEndpoint^ ep0 = gcnew TunnelEndpoint();
			TunnelEndpoint^ ep1 = gcnew TunnelEndpoint();
			ep0->Remote = ep1;
			ep1->Remote = ep0;
			return gcnew Tuple<Endpoint^, Endpoint^>(ep0, ep1);
		}
	};

}
