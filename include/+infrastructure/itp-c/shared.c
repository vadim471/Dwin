#include <itp/callback.h>

itp_free_user_handler_fp handler;

void itp_set_free_user_handler(itp_free_user_handler_fp f) {
    handler = f;
}

void itp_free_user_handler(
    void* ptr
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {
    if (handler) {
        handler(
            ptr
            #ifdef ITP_EXTEND_CALLBACKS
            , eparam
            #endif // ITP_EXTEND_CALLBACKS
        );
    }
}
