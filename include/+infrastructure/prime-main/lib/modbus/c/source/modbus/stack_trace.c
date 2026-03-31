#ifdef MODBUS_USE_STACK_TRACE

#include <modbus/stack_trace.h>
#include <backtrace.h>

static struct backtrace_state* state = NULL;

static void error_callback(void* data, const char* msg, int errnum) {
	fprintf(stderr, "ERROR: %s (%d)\r\n", msg, errnum);
}

void mbs_init_stack_trace(const char* executable) {
    state = backtrace_create_state(executable, 0, error_callback, NULL);
}

void mbs_print_stack_trace(FILE* stream) {
    fprintf(stream, "- MODBUS stack trace -->\r\n");
    backtrace_print(state, 0, stream);
    fprintf(stream, "<-----------------------\r\n");
}

#endif // MODBUS_USE_STACK_TRACE
