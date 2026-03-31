#ifdef ITP_USE_STACK_TRACE

#include <itp/stack_trace.h>
#include <backtrace.h>

static struct backtrace_state* state = NULL;

static void error_callback(void* data, const char* msg, int errnum) {
	fprintf(stderr, "ERROR: %s (%d)\r\n", msg, errnum);
}

void itp_init_stack_trace(const char* executable) {
    state = backtrace_create_state(executable, 0, error_callback, NULL);
}

void itp_print_stack_trace(FILE* stream) {
    fprintf(stream, "ITP stack trace ------->\r\n");
    backtrace_print(state, 0, stream);
    fprintf(stream, "<-----------------------\r\n");
}

#endif // ITP_USE_STACK_TRACE
