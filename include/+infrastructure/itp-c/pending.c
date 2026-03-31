#include <stdio.h>
#include <itp/list.h>
#include <itp/node.h>
#include <itp/memory.h>

void itp_free_user_handler(void* handler) {}

int main(int argc, char** argv) {
    itp_pending_list_t list;
    itp_init_pending_list(&list);
    for (uint8_t i = 1; i <= 4; i++) {
        itp_node_tp node = itp_malloc(sizeof(itp_node_t), "main:node");
        itp_init_node(node, NULL);
        node->address = i;
        if (!itp_push_pending_node(&list, node)) {
            printf("Added node %d\r\n", i);
        } else itp_free(node);
    }
    for (uint8_t i = 0; i < 2; i++) {
        itp_pending_node_tp last = list.first;
        while (last) {
            if (last->node->address == 2) {
                printf("Erasing node %d\r\n", last->node->address);
                itp_node_tp node = last->node;
                last = itp_erase_pending_node(&list, node);
                itp_free_node(node);
            } else {
                printf("Skip node %d\r\n", last->node->address);
                last = last->next;
            }
        }
    }
    itp_clear_pending_list(&list);
    return 0;
}
