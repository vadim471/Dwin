#pragma once

#include "Config.hpp"
#include "Endpoint.hpp"

extern "C" {

#include <initp/platform/serial_port.h>

}

#include <memory>

namespace itp {

public ref class SerialPortNative:
    public Endpoint {
private: // Private types
    typedef SerialPortNative self_type;
public: // Public types
    typedef std::unique_ptr<self_type> uptr;
public: // Construction
	SerialPortNative(void):
	    Endpoint(),
	    c_object_(nullptr),
	    opened_(false)
    {
	    this->c_object_ = new sys_serial_port_t();
	    if (this->c_object_)
	    {
		    sys_init_serial_port(this->c_object_);
	    }
    }

	virtual ~SerialPortNative(void) {
		this->Close();
		delete this->c_object_;
	}
public: // Public methods
	virtual bool Open(String^ path, uint32_t baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity) {
		if (!this->c_object_) return false;
		if (this->opened_) return true;
		IntPtr s = Marshal::StringToHGlobalAnsi(path);
		try {
			if (sys_open_serial_port(this->c_object_, static_cast<char*>(s.ToPointer()), baudrate, databits, stopbits, parity)) {
				this->opened_ = true;
			}
		} finally {
			Marshal::FreeHGlobal(s);
		}
		return this->opened_;
	}
	virtual void Close(void) {
		if (this->opened_) {
			sys_close_serial_port(this->c_object_);
			this->opened_ = false;
		}
	}
public: // Endpoint implementation
	virtual itp_size_t Read(array<Byte>^% data, itp_size_t size) override {
		if (size < 1) return 0;
		char* unmanaged = (char*)itp_malloc(size * sizeof(char), "itp::SerialPort:data");
		if (!unmanaged) return 0;
		itp_size_t result = (itp_size_t)sys_read_serial_port(this->c_object_, unmanaged, size);
		if (result > 0) {
			Array::Resize(data, result);
			for (itp_size_t i = 0; i < result; ++i) {
				data[i] = (Byte)unmanaged[i];
			}
		}
		itp_free(unmanaged);
		return result;
	}
	virtual itp_size_t Write(array<Byte>^% data, itp_size_t length) override {
		if (length < 1) return 0;
		char* unmanaged = (char*)itp_malloc(length * sizeof(char), "itp::SerialPort:data");
		if (!unmanaged) return 0;
		for (itp_size_t i = 0; i < length; ++i)
			unmanaged[i] = data[i];
		itp_size_t result = (itp_size_t)sys_write_serial_port(this->c_object_, unmanaged, length);
		itp_free(unmanaged);
		return result;
	}
public:
	property sys_serial_port_tp CObject {
		sys_serial_port_tp get() {
			return this->c_object_;
		}
	}
	property bool Opened {
		bool get() {
			return this->opened_;
		}
	}
private:
	sys_serial_port_tp c_object_;
	bool opened_;
};

}
