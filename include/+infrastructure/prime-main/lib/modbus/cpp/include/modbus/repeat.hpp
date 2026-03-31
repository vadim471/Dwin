#ifndef MODBUS_REPEAT_HPP_INCLUDED
#define MODBUS_REPEAT_HPP_INCLUDED

#include "config.hpp"

extern "C" {

#include <modbus/debug.h>
#include <modbus/repeat.h>
#include <modbus/memory.h>

}

#include <memory>
#include <tuple>

namespace modbus {

/**
 * \brief Интерфейс автоповтора запросов.
 *
 * После передачи объекта C-библиотеке, указатель будет освобождён,
 * а память в последствии будет освобождена через \c mbs_free,
 * поэтому объект не должен иметь сложного деструктора.
 */
class repeat {
    typedef repeat self_type;
public:
    typedef std::unique_ptr<self_type> uptr;
protected:
    repeat(void) = default;
    repeat(const repeat&) = delete;
public:
    virtual ~repeat(void) = default;
public:
    static uptr once(void);
    static uptr cyclic(mbs_time_t);
public:
    virtual std::tuple<mbs_repeat_status_t, mbs_error_code_t> check(mbs_time_t time) = 0;
    virtual mbs_error_code_t reset(mbs_time_t time) = 0;
};

mbs_repeat_status_t repeat_check(
    void* object,
    mbs_time_t time,
    mbs_error_code_t* error
);

mbs_error_code_t repeat_reset(
    void* object,
    mbs_time_t time
);

class repeat_once:
    public repeat {
public:
    repeat_once(void):
        repeat() {
        mbs_repeat_once_init(&this->c_object_);
    }
public:
    virtual std::tuple<mbs_repeat_status_t, mbs_error_code_t> check(mbs_time_t time) {
        mbs_error_code_t errc = MBS_ERRC_NONE;
        mbs_repeat_status_t status = mbs_repeat_once_check(&this->c_object_, time, &errc);
        return std::make_tuple(status, errc);
    }
    virtual mbs_error_code_t reset(mbs_time_t time) {
        return mbs_repeat_once_reset(&this->c_object_, time);
    }
private:
    mbs_repeat_once_t c_object_;
};

class repeat_cyclic:
    public repeat {
public:
    repeat_cyclic(mbs_time_t timeout):
        repeat() {
        mbs_repeat_cyclic_init(&this->c_object_, timeout);
    }
public:
    virtual std::tuple<mbs_repeat_status_t, mbs_error_code_t> check(mbs_time_t time) {
        mbs_error_code_t errc = MBS_ERRC_NONE;
        mbs_repeat_status_t status = mbs_repeat_cyclic_check(&this->c_object_, time, &errc);
        return std::make_tuple(status, errc);
    }
    virtual mbs_error_code_t reset(mbs_time_t time) {
        return mbs_repeat_cyclic_reset(&this->c_object_, time);
    }
private:
    mbs_repeat_cyclic_t c_object_;
};

}

#endif // MODBUS_REPEAT_HPP_INCLUDED
