#include <stdio.h>
#include <itp/debug.h>
#include <itp/memory.h>
#include <itp/queue.h>
#include <itp/package.h>
#include <itp/symbol.h>

void itp_free_user_handler(void* handler) {}

int main(int argc, char** argv) {
    itp_package_queue_t queue;
    itp_init_package_queue(&queue, 3);
    for (int i = 0; i < 5; ++i) {
        itp_package_tp package = itp_malloc(sizeof(itp_package_t), "main:package");
        itp_init_package(package);
        package->status = (i > 0) ? ITP_SYM_REQ : ITP_SYM_CTR;
        if (!itp_push_package(&queue, package)) {
            itp_debug_print(ITP_ERRC_TRACE, "Pushed");
        } else itp_free_package(package);
    }
    itp_clear_package_queue(&queue);
    return 0;
}
