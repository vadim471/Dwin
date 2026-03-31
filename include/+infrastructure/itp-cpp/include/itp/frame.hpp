#ifndef ITP_FRAME_HPP_INCLUDED
#define ITP_FRAME_HPP_INCLUDED

#include "config.hpp"

extern "C" {

#include <itp/debug.h>
#include <itp/frame.h>
#include <itp/memory.h>

}

#include <vector>
#include <memory>
#include <tuple>

namespace itp {

/**
 * \brief Класс, управляющий структурой \c itp_frame.
 *
 * Позволяет создавать и заполнять кадры для дальнейшей их передачи по сети.
 */
class frame {

private: // Private types
    typedef frame self_type;

public: // Public types
    typedef std::unique_ptr<self_type> uptr;
    typedef std::shared_ptr<self_type> ptr;
    typedef std::weak_ptr<self_type> wptr;

public: // Construction
    frame(uint16_t command):
        c_object_(NULL) {
        this->c_object_ = itp_create_frame(command);
    }
    frame(itp_frame_tp frame):
        c_object_(frame) {}
    explicit frame(const self_type& frame):
        c_object_(NULL) {
        if (frame.c_object_) {
            this->c_object_ = itp_copy_frame(frame.c_object_);
        }
    }
    virtual ~frame(void) {
        if (this->c_object_) {
            itp_free_frame(this->c_object_);
        }
    }

public: // Public methods
    itp_error_code_t prepare(itp_size_t size) {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::prepare> C object is null");
            return ITP_ERRC_NULL_POINTER;
        }
        return itp_prepare_frame(this->c_object_, size);
    }
    itp_error_code_t expand(itp_size_t size) {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::expand> C object is null");
            return ITP_ERRC_NULL_POINTER;
        }
        return itp_expand_frame(this->c_object_, size);
    }
    void prepare_to_read(void) {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::prepare_to_read> C object is null");
            return;
        }
        itp_frame_prepare_to_read(this->c_object_);
    }
    void prepare_to_write(void) {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::prepare_to_write> C object is null");
            return;
        }
        itp_frame_prepare_to_write(this->c_object_);
    }
    void dispose(void) {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::dispose> C object is null");
            return;
        }
        itp_dispose_frame(this->c_object_);
    }
    bool has_more(void) const {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::has_more> C object is null");
            return false;
        }
        return (itp_frame_has_more(this->c_object_) == 0) ? false : true;
    }
    itp_size_t remain(void) const {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::remain> C object is null");
            return 0;
        }
        return itp_frame_remain(this->c_object_);
    }
    void dump(void) {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::dump> C object is null");
            return;
        }
        itp_dump_frame(this->c_object_);
    }

public: // Read methods
    template<typename T>
    std::tuple<T, itp_error_code_t> read_value(void) {
        T value = (T)0;
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::read_value> C object is null");
            return std::make_tuple(value, ITP_ERRC_NULL_POINTER);
        }
        itp_error_code_t error = itp_frame_read_value(this->c_object_, &value, sizeof(T));
        return std::make_tuple(value, error);
    }
    template<typename T>
    itp_error_code_t read_value(T& value) {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::read_value> C object is null");
            return ITP_ERRC_NULL_POINTER;
        }
        return itp_frame_read_value(this->c_object_, &value, sizeof(T));
    }
    std::tuple<std::string, itp_error_code_t> read_string(void) {
        std::string value;
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::read_string> C object is null");
            return std::make_tuple(value, ITP_ERRC_NULL_POINTER);
        }
        char* data(NULL);
        itp_error_code_t error = itp_frame_read_string(this->c_object_, &data);
        if (!error && data) {
            value = std::string(data);
            itp_free(data);
        }
        return std::make_tuple(value, error);
    }
    std::tuple<std::string, itp_error_code_t> read_wide_string(void) {
        std::string value;
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::read_wide_string> C object is null");
            return std::make_tuple(value, ITP_ERRC_NULL_POINTER);
        }
        char* data(NULL);
        itp_error_code_t error = itp_frame_read_wide_string(this->c_object_, &data);
        if (!error && data) {
            value = std::string(data);
            itp_free(data);
        }
        return std::make_tuple(value, error);
    }
    template<typename T>
    std::tuple<std::vector<T>, itp_error_code_t> read_array(void)
    {
        std::vector<T> vector;
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::read_array> C object is null");
            return std::make_tuple(vector, ITP_ERRC_NULL_POINTER);
        }
        T* array(NULL);
        uint16_t length;
        itp_error_code_t error = itp_frame_read_array(this->c_object_, (void**)&array, &length, sizeof(T));
        if (!error) {
            if (array) {
                if (length > 0) {
                    vector.insert(vector.begin(), array, array + length);
                } else {
                    itp_debug_print(ITP_ERRC_WARNING, "itp::frame::read_array> No error, but array length is 0");
                }
                itp_free(array);
            }
        }
        return std::make_tuple(vector, error);
    }
    template<typename T>
    itp_error_code_t read_array(std::vector<T>& vector)
    {
        if (!vector.empty())
            vector.clear();
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::read_array> C object is null");
            return ITP_ERRC_NULL_POINTER;
        }
        T* array(NULL);
        uint16_t length;
        itp_error_code_t error = itp_frame_read_array(this->c_object_, (void**)&array, &length, sizeof(T));
        if (!error) {
            if (array) {
                if (length > 0) {
                    vector.insert(vector.begin(), array, array + length);
                } else {
                    itp_debug_print(ITP_ERRC_WARNING, "itp::frame::read_array> No error, but array length is null");
                }
                itp_free(array);
            }
        }
        return error;
    }
    template<typename T>
    std::tuple<std::vector<T>, itp_error_code_t> read_wide_array(void) {
        std::vector<T> vector;
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::read_wide_array> C object is null");
            return std::make_tuple(vector, ITP_ERRC_NULL_POINTER);
        }
        T* array(NULL);
        uint16_t length;
        itp_error_code_t error = itp_frame_read_wide_array(this->c_object_, (void**)&array, &length, sizeof(T));
        if (!error) {
            if (array) {
                if (length > 0) {
                    vector.insert(vector.begin(), array, array + length);
                } else {
                    itp_debug_print(ITP_ERRC_WARNING, "itp::frame::read_wide_array> No error, but array length is 0");
                }
                itp_free(array);
            }
        }
        return std::make_tuple(vector, error);
    }
    template<typename T>
    itp_error_code_t read_wide_array(std::vector<T>& vector) {
        if (!vector.empty())
            vector.clear();
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::read_wide_array> C object is null");
            return ITP_ERRC_NULL_POINTER;
        }
        T* array(NULL);
        uint16_t length;
        itp_error_code_t error = itp_frame_read_wide_array(this->c_object_, (void**)&array, &length, sizeof(T));
        if (!error) {
            if (array) {
                if (length > 0) {
                    vector.insert(vector.begin(), array, array + length);
                } else {
                    itp_debug_print(ITP_ERRC_WARNING, "itp::frame::read_wide_array> No error, but array length is null");
                }
                itp_free(array);
            }
        }
        return error;
    }

public: // Write methods
    template<typename T>
    itp_error_code_t write_value(const T& value) {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::write_value> C object is null");
            return ITP_ERRC_NULL_POINTER;
        }
        return itp_frame_write_value(this->c_object_, &value, sizeof(T));
    }
    itp_error_code_t write_string(const std::string& value) {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::write_string> C object is null");
            return ITP_ERRC_NULL_POINTER;
        }
        if (value.size() > 0xFF) {
            itp_debug_print(ITP_ERRC_OVERFLOW, "itp::frame::write_string> String length must be less than 256");
            return ITP_ERRC_OVERFLOW;
        }
        return itp_frame_write_string(this->c_object_, value.c_str());
    }
    itp_error_code_t write_wide_string(const std::string& value) {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::write_wide_string> C object is null");
            return ITP_ERRC_NULL_POINTER;
        }
        if (value.size() > 0xFFFF) {
            itp_debug_print(ITP_ERRC_OVERFLOW, "itp::frame::write_wide_string> String length must be less than 65535");
            return ITP_ERRC_OVERFLOW;
        }
        return itp_frame_write_wide_string(this->c_object_, value.c_str());
    }
    template<typename T>
    itp_error_code_t write_array(const std::vector<T>& vector) 
    {
        if (!this->c_object_) 
        {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::write_array> C object is null");
            return ITP_ERRC_NULL_POINTER;
        }
        size_t length = vector.size();
        if (length > 0) 
        {
            size_t size = length * sizeof(T);
            if (size > 0xFFFF) 
            {
                itp_debug_print(ITP_ERRC_OVERFLOW, "itp::frame::write_array> Total array size must be less than 65535");
                return ITP_ERRC_OVERFLOW;
            }
            return itp_frame_write_array(this->c_object_, (const void*)&vector[0], (uint16_t)length, (uint16_t)sizeof(T));
        } 
        else 
        {
            return itp_frame_write_array(this->c_object_, NULL, 0, (uint16_t)sizeof(T));
        }
    }
    template<typename T>
    itp_error_code_t write_array(const T* array, uint16_t length) 
    {
        if (!this->c_object_) 
        {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::write_array> C object is null");
            return ITP_ERRC_NULL_POINTER;
        }
        if (length > 0) 
        {
            size_t size = size_t(length) * sizeof(T);
            if (size > 0xFFFF) 
            {
                itp_debug_print(ITP_ERRC_OVERFLOW, "itp::frame::write_array> Total array size must be less than 65535");
                return ITP_ERRC_OVERFLOW;
            }
            return itp_frame_write_array(this->c_object_, (const void*)array, length, (uint16_t)sizeof(T));
        } 
        else 
        {
            return itp_frame_write_array(this->c_object_, NULL, 0, (uint16_t)sizeof(T));
        }
    }
    template<typename T>
    itp_error_code_t write_wide_array(const std::vector<T>& vector) {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::write_wide_array> C object is null");
            return ITP_ERRC_NULL_POINTER;
        }
        size_t length = vector.size();
        if (length > 0) {
            size_t size = length * sizeof(T);
            if (size > 0xFFFF) {
                itp_debug_print(ITP_ERRC_OVERFLOW, "itp::frame::write_wide_array> Total array size must be less than 65535");
                return ITP_ERRC_OVERFLOW;
            }
            return itp_frame_write_wide_array(this->c_object_, (const void*)&vector[0], (uint16_t)length, (uint8_t)sizeof(T));
        } else {
            return itp_frame_write_wide_array(this->c_object_, NULL, 0, (uint8_t)sizeof(T));
        }
    }
    template<typename T>
    itp_error_code_t write_wide_array(const T* array, uint16_t length) {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::write_wide_array> C object is null");
            return ITP_ERRC_NULL_POINTER;
        }
        if (length > 0) {
            size_t size = size_t(length) * sizeof(T);
            if (size > 0xFFFF) {
                itp_debug_print(ITP_ERRC_OVERFLOW, "itp::frame::write_wide_array> Total array size must be less than 65535");
                return ITP_ERRC_OVERFLOW;
            }
            return itp_frame_write_wide_array(this->c_object_, (const void*)array, length, (uint8_t)sizeof(T));
        } else {
            return itp_frame_write_wide_array(this->c_object_, NULL, 0, (uint8_t)sizeof(T));
        }
    }

public: // Public properties
    itp_frame_tp c_object(void) {
        return this->c_object_;
    }
    void c_object(itp_frame_tp value) {
        this->c_object_ = value;
    }
    uint8_t from(void) const {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::from::get> C object is null");
            return 0;
        }
        return this->c_object_->from;
    }
    void from(uint8_t value) {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::from::set> C object is null");
            return;
        }
        this->c_object_->from = value;
    }
    uint8_t to(void) const {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::to::get> C object is null");
            return 0;
        }
        return this->c_object_->to;
    }
    void to(uint8_t value) {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::to::set> C object is null");
            return;
        }
        this->c_object_->to = value;
    }
    uint16_t command(void) const {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::command::get> C object is null");
            return 0;
        }
        return this->c_object_->command;
    }
    void command(uint16_t value) {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::command::set> C object is null");
            return;
        }
        this->c_object_->command = value;
    }
    uint16_t order(void) const {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::order::get> C object is null");
            return 0;
        }
        return this->c_object_->order;
    }
    void order(uint16_t value) {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::order::set> C object is null");
            return;
        }
        this->c_object_->order = value;
    }
    uint16_t error(void) const {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::error::get> C object is null");
            return 0;
        }
        return this->c_object_->error;
    }
    void error(uint16_t value) {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::error::set> C object is null");
            return;
        }
        this->c_object_->error = value;
    }
    itp_size_t data_size(void) const {
        if (!this->c_object_) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::frame::data_size> C object is null");
            return 0;
        }
        return this->c_object_->length;
    }

private: // Private fields
    itp_frame_tp c_object_;
};

}

#endif // ITP_FRAME_HPP_INCLUDED
