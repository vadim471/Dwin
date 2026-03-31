#include <modbus/repeat.hpp>

namespace modbus {

mbs_repeat_status_t repeat_check(
    void* object,
    mbs_time_t time,
    mbs_error_code_t* error
) {
    if (!object) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "modbus::repeat_check> User object is null");
        if (error) *error = MBS_ERRC_NULL_POINTER;
        return MBS_RS_ERROR;
    }
    auto result = static_cast<repeat*>(object)->check(time);
    if (error) *error = std::get<1>(result);
    return std::get<0>(result);
}

mbs_error_code_t repeat_reset(
    void* object,
    mbs_time_t time
) {
    if (!object) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "modbus::repeat_reset> User object is null");
        return MBS_ERRC_NULL_POINTER;
    }
    return static_cast<repeat*>(object)->reset(time);
}

repeat::uptr repeat::once(void) {
    repeat_once* object = (repeat_once*)mbs_malloc(sizeof(repeat_once), "modbus::repeat::once:repeat_once");
    if (!object) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "modbus::repeat::once> Memory allocation failed");
        return nullptr;
    }
    new (object) repeat_once;
    return uptr(object);
}

repeat::uptr repeat::cyclic(mbs_time_t timeout) {
    repeat_cyclic* object = (repeat_cyclic*)mbs_malloc(sizeof(repeat_cyclic), "modbus::repeat::cyclic:repeat_cyclic");
    if (!object) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "modbus::repeat::cyclic> Memory allocation failed");
        return nullptr;
    }
    new (object) repeat_cyclic(timeout);
    return uptr(object);
}

}
