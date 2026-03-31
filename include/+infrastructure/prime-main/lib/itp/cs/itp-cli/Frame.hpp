#pragma once

#include "Config.hpp"
#include "ErrorCode.hpp"

extern "C" {

#include <itp/debug.h>
#include <itp/frame.h>
#include <itp/memory.h>

}

using namespace System::Text;

namespace itp {

	public ref class Frame {

	private: // Private types
		typedef Frame self_type;

	public: // Construction
		Frame(uint16_t command): c_object_(NULL) {
			this->c_object_ = itp_create_frame(command);
		}
		Frame(itp_frame_tp frame): c_object_(frame) {}
		explicit Frame(const self_type% frame) :
			c_object_(NULL) {
			if (frame.c_object_) {
				this->c_object_ = itp_copy_frame(frame.c_object_);
			}
		}
		virtual ~Frame(void) {
			if (this->c_object_) {
				itp_free_frame(this->c_object_);
			}
		}
		!Frame(void) {
			if (this->c_object_) {
				itp_free_frame(this->c_object_);
			}
		}

	public: // Public methods
		ErrorCode Prepare(itp_size_t size) {
			if (!this->c_object_) {
				itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::prepare> C object is null");
				return ErrorCode::NULL_POINTER;
			}
			return static_cast<ErrorCode>(itp_prepare_frame(this->c_object_, size));
		}
		void PrepareToRead(void) {
			if (!this->c_object_) {
				itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::prepare_to_read> C object is null");
				return;
			}
			itp_frame_prepare_to_read(this->c_object_);
		}
		void PrepareToWrite(void) {
			if (!this->c_object_) {
				itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::prepare_to_write> C object is null");
				return;
			}
			itp_frame_prepare_to_write(this->c_object_);
		}
		void DisposeFrame(void) {
			if (!this->c_object_) {
				itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::dispose> C object is null");
				return;
			}
			itp_dispose_frame(this->c_object_);
		}
		bool HasMore(void) {
			if (!this->c_object_) {
				itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::has_more> C object is null");
				return false;
			}
			return (itp_frame_has_more(this->c_object_) == 0) ? false : true;
		}
		itp_size_t Remain(void) {
			if (!this->c_object_) {
				itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::remain> C object is null");
				return 0;
			}
			return itp_frame_remain(this->c_object_);
		}
		void Dump(void) {
			if (!this->c_object_) {
				itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::dump> C object is null");
				return;
			}
			itp_dump_frame(this->c_object_);
		}

	public: // Read methods
		template<typename T>
		ErrorCode read_value([Out] T% value) {
			if (!this->c_object_) {
				itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::read_value> C object is null");
				return ErrorCode::NULL_POINTER;
			}
			T unmanaged = (T)0;
			itp_error_code_t error = itp_frame_read_value(this->c_object_, &unmanaged, sizeof(T));
			if (!error) value = unmanaged;
			return static_cast<ErrorCode>(error);
		}
		ErrorCode ReadUInt8([Out] uint8_t% value) {
			return read_value<uint8_t>(value);
		}
		ErrorCode ReadUInt16([Out] uint16_t% value) {
			return read_value<uint16_t>(value);
		}
		ErrorCode ReadUInt32([Out] uint32_t% value) {
			return read_value<uint32_t>(value);
		}
		ErrorCode ReadUInt64([Out] uint64_t% value) {
			return read_value<uint64_t>(value);
		}
		ErrorCode ReadInt8([Out] int8_t% value) {
			return read_value<int8_t>(value);
		}
		ErrorCode ReadInt16([Out] int16_t% value) {
			return read_value<int16_t>(value);
		}
		ErrorCode ReadInt32([Out] int32_t% value) {
			return read_value<int32_t>(value);
		}
		ErrorCode ReadInt64([Out] int64_t% value) {
			return read_value<int64_t>(value);
		}
		ErrorCode ReadFloat([Out] float% value) {
			return read_value<float>(value);
		}
		ErrorCode ReadDouble([Out] double% value) {
			return read_value<double>(value);
		}
		ErrorCode ReadString([Out] String^% value) {
			if (!this->c_object_) {
				itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::read_string> C object is null");
				return ErrorCode::NULL_POINTER;
			}
			char* data(NULL);
			itp_error_code_t error = itp_frame_read_string(this->c_object_, &data);
			if (error == ITP_ERRC_NONE && data != nullptr) {
				value = StringFromNativeUtf8(IntPtr(data));
				itp_free(data);
			}
			return static_cast<ErrorCode>(error);
		}
		ErrorCode ReadWideString([Out] String^% value) {
			if (!this->c_object_) {
				itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::read_wide_string> C object is null");
				return ErrorCode::NULL_POINTER;
			}
			char* data(NULL);
			itp_error_code_t error = itp_frame_read_wide_string(this->c_object_, &data);
			if (error == ITP_ERRC_NONE && data != nullptr) {
				value = StringFromNativeUtf8(IntPtr(data));
				itp_free(data);
			}
			return static_cast<ErrorCode>(error);
		}
		template<typename T>
		ErrorCode read_array(array<T>^% data) {
			if (!this->c_object_) {
				itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::read_array> C object is null");
				return ErrorCode::NULL_POINTER;
			}
			T* unmanaged(NULL);
			uint16_t length = 0;
			itp_error_code_t error = itp_frame_read_array(this->c_object_, (void**)&unmanaged, &length, sizeof(T));
			if (!error) {
				if (!unmanaged) {
					if (length > 0) {
						itp_debug_print(ITP_ERRC_INTERNAL, "itp::frame::read_array> No error, but array is null");
						return ErrorCode::INTERNAL;
					} else
					{
						Array::Resize(data, 0);
					}
				} else {
					if (length > 0) {
						Array::Resize(data, length);
						for (uint16_t i = 0; i < length; ++i) {
							data[i] = unmanaged[i];
						}
					} else {
						itp_debug_print(ITP_ERRC_WARNING, "itp::frame::read_array> No error, but array length is null");
					}
					itp_free(unmanaged);
				}
			}
			return static_cast<ErrorCode>(error);
		}
		ErrorCode ReadArrayUInt8(array<uint8_t>^% data) {
			return read_array<uint8_t>(data);
		}
		ErrorCode ReadArrayUInt16(array<uint16_t>^% data) {
			return read_array<uint16_t>(data);
		}
		ErrorCode ReadArrayUInt32(array<uint32_t>^% data) {
			return read_array<uint32_t>(data);
		}
		ErrorCode ReadArrayUInt64(array<uint64_t>^% data) {
			return read_array<uint64_t>(data);
		}
		ErrorCode ReadArrayInt8(array<int8_t>^% data) {
			return read_array<int8_t>(data);
		}
		ErrorCode ReadArrayInt16(array<int16_t>^% data) {
			return read_array<int16_t>(data);
		}
		ErrorCode ReadArrayInt32(array<int32_t>^% data) {
			return read_array<int32_t>(data);
		}
		ErrorCode ReadArrayInt64(array<int64_t>^% data) {
			return read_array<int64_t>(data);
		}
		ErrorCode ReadArrayFloat(array<float>^% data) {
			return read_array<float>(data);
		}
		ErrorCode ReadArrayDouble(array<double>^% data) {
			return read_array<double>(data);
		}

	public: // Write methods
		template<typename T>
		ErrorCode write_value(const T% value) {
			if (!this->c_object_) {
				itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::write_value> C object is null");
				return ErrorCode::NULL_POINTER;
			}
			T unmanaged = value;
			return static_cast<ErrorCode>(itp_frame_write_value(this->c_object_, &unmanaged, sizeof(T)));
		}
		ErrorCode WriteUInt8(uint8_t value) {
			return write_value<uint8_t>(value);
		}
		ErrorCode WriteUInt16(uint16_t value) {
			return write_value<uint16_t>(value);
		}
		ErrorCode WriteUInt32(uint32_t value) {
			return write_value<uint32_t>(value);
		}
		ErrorCode WriteUInt64(uint64_t value) {
			return write_value<uint64_t>(value);
		}
		ErrorCode WriteInt8(int8_t value) {
			return write_value<int8_t>(value);
		}
		ErrorCode WriteInt16(int16_t value) {
			return write_value<int16_t>(value);
		}
		ErrorCode WriteInt32(int32_t value) {
			return write_value<int32_t>(value);
		}
		ErrorCode WriteInt64(int64_t value) {
			return write_value<int64_t>(value);
		}
		ErrorCode WriteFloat(float value) {
			return write_value<float>(value);
		}
		ErrorCode WriteDouble(double value) {
			return write_value<double>(value);
		}
		ErrorCode WriteString(String^ value) {
			if (!this->c_object_) {
				itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::write_string> C object is null");
				return ErrorCode::NULL_POINTER;
			}
			if (value->Length > 0xFF) {
				itp_debug_print(ITP_ERRC_OVERFLOW, "itp::frame::write_string> String length must be less than 256");
				return itp::ErrorCode::OVERFLOWED;
			}
			IntPtr s = NativeUtf8FromString(value);
			try {
				return static_cast<ErrorCode>(itp_frame_write_string(this->c_object_, (char*)s.ToPointer()));
			} finally {
				Marshal::FreeHGlobal(s);
			}
		}
		ErrorCode WriteWideString(String^ value) {
			if (!this->c_object_) {
				itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::write_wide_string> C object is null");
				return ErrorCode::NULL_POINTER;
			}
			if (value->Length > 0xFFFD) {
				itp_debug_print(ITP_ERRC_OVERFLOW, "itp::frame::write_wide_string> String length must be less than 0xFFFD");
				return itp::ErrorCode::OVERFLOWED;
			}
			IntPtr s = NativeUtf8FromString(value);
			try {
				return static_cast<ErrorCode>(itp_frame_write_wide_string(this->c_object_, (char*)s.ToPointer()));
			}
			finally {
				Marshal::FreeHGlobal(s);
			}
		}
		template<typename T>
		ErrorCode write_array(array<T>^ data) {
			if (!this->c_object_) {
				itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::write_array> C object is null");
				return ErrorCode::NULL_POINTER;
			}
			size_t length = data->Length;
			if (length > 0) {
				size_t size = length * sizeof(T);
				if (size > 0xFFFD) {
					itp_debug_print(ITP_ERRC_OVERFLOW, "itp::frame::write_array> Total array size must be less than 0xFFFD");
					return itp::ErrorCode::OVERFLOWED;
				}
				T* unmanaged = (T*)itp_malloc(size, "itp::frame::write_array:data");
				if (!unmanaged) return ErrorCode::MALLOC_FAILED;
				for (uint16_t i = 0; i < length; ++i)
					unmanaged[i] = data[i];
				itp_error_code_t errc = itp_frame_write_array(this->c_object_, (const void*)unmanaged, (uint16_t)length, (uint16_t)sizeof(T));
				itp_free(unmanaged);
				return static_cast<ErrorCode>(errc);
			} else {
				return static_cast<ErrorCode>(itp_frame_write_array(this->c_object_, NULL, 0, (uint16_t)sizeof(T)));
			}
		}
		ErrorCode WriteArrayUInt8(array<uint8_t>^ data) {
			return write_array<uint8_t>(data);
		}
		ErrorCode WriteArrayUInt16(array<uint16_t>^ data) {
			return write_array<uint16_t>(data);
		}
		ErrorCode WriteArrayUInt32(array<uint32_t>^ data) {
			return write_array<uint32_t>(data);
		}
		ErrorCode WriteArrayUInt64(array<uint64_t>^ data) {
			return write_array<uint64_t>(data);
		}
		ErrorCode WriteArrayInt8(array<int8_t>^ data) {
			return write_array<int8_t>(data);
		}
		ErrorCode WriteArrayInt16(array<int16_t>^ data) {
			return write_array<int16_t>(data);
		}
		ErrorCode WriteArrayInt32(array<int32_t>^ data) {
			return write_array<int32_t>(data);
		}
		ErrorCode WriteArrayInt64(array<int64_t>^ data) {
			return write_array<int64_t>(data);
		}
		ErrorCode WriteArrayFloat(array<float>^ data) {
			return write_array<float>(data);
		}
		ErrorCode WriteArrayDouble(array<double>^ data) {
			return write_array<double>(data);
		}

	public: // Public properties
		property itp_frame_tp CObject {
			itp_frame_tp get() {
				return this->c_object_;
			}
			void set(itp_frame_tp object) {
				this->c_object_ = object;
			}
		}
		property uint8_t From {
			uint8_t get() {
				if (!this->c_object_) {
					itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::from::get> C object is null");
					return 0;
				}
				return this->c_object_->from;
			}
			void set(uint8_t value) {
				if (!this->c_object_) {
					itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::from::set> C object is null");
					return;
				}
				this->c_object_->from = value;
			}
		}
		property uint8_t To {
			uint8_t get(void) {
				if (!this->c_object_) {
					itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::to::get> C object is null");
					return 0;
				}
				return this->c_object_->to;
			}
			void set(uint8_t value) {
				if (!this->c_object_) {
					itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::to::set> C object is null");
					return;
				}
				this->c_object_->to = value;
			}
		}
		property uint16_t Command {
			uint16_t get(void) {
				if (!this->c_object_) {
					itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::command::get> C object is null");
					return 0;
				}
				return this->c_object_->command;
			}
			void set(uint16_t value) {
				if (!this->c_object_) {
					itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::command::set> C object is null");
					return;
				}
				this->c_object_->command = value;
			}
		}
		property uint16_t Order {
			uint16_t get(void) {
				if (!this->c_object_) {
					itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::order::get> C object is null");
					return 0;
				}
				return this->c_object_->order;
			}
			void set(uint16_t value) {
				if (!this->c_object_) {
					itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::order::set> C object is null");
					return;
				}
				this->c_object_->order = value;
			}
		}
		property uint16_t Error {
			uint16_t get(void) {
				if (!this->c_object_) {
					itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::error::get> C object is null");
					return 0;
				}
				return this->c_object_->error;
			}
			void set(uint16_t value) {
				if (!this->c_object_) {
					itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::error::set> C object is null");
					return;
				}
				this->c_object_->error = value;
			}
		}
		property itp_size_t DataSize {
			itp_size_t get(void) {
				if (!this->c_object_) {
					itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::data_size> C object is null");
					return 0;
				}
				return this->c_object_->length;
			}
		}

	private: // Private fields
		itp_frame_tp c_object_;

		static IntPtr NativeUtf8FromString(String^ managedString)
	    {
            const auto len = Encoding::UTF8->GetByteCount(managedString);
            const auto buffer = gcnew array<byte>(len + 1);
			Encoding::UTF8->GetBytes(managedString, 0, managedString->Length, buffer, 0);
            auto nativeUtf8 = Marshal::AllocHGlobal(buffer->Length);
			Marshal::Copy(buffer, 0, nativeUtf8, buffer->Length);

			return nativeUtf8;
		}

		static String^ StringFromNativeUtf8(IntPtr nativeUtf8)
	    {
            auto len = 0;
			while (Marshal::ReadByte(nativeUtf8, len) != 0) 
			{
				++len;
			}

            const auto buffer = gcnew array<byte>(len);
			Marshal::Copy(nativeUtf8, buffer, 0, buffer->Length);
			return Encoding::UTF8->GetString(buffer);
		}
	};

}
