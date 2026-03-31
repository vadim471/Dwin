#include <stdio.h>
#include <itp/memory.h>
#include <itp/list.h>
#include <itp/frame.h>

void itp_free_user_handler(void* handler) {}

int main(int argc, char** argv) {
    itp_frame_list_t list;
    itp_init_frame_list(&list, 3);
    for (uint8_t i = 1; i <= 4; i++) {
        itp_frame_tp frame = itp_create_frame(i);
        itp_frame_item_tp item = itp_malloc(sizeof(itp_frame_item_t), "main:frame_item");
        if (!item) {
            itp_free_frame(frame);
            printf("Failed to create item\r\n");
            continue;
        }
        itp_init_frame_item(item, frame);
        if (!itp_push_frame_item(&list, item)) {
            printf("Added frame %d\r\n", i);
        } else {
            itp_free(item);
            itp_free_frame(frame);
        }
    }
    for (uint8_t i = 0; i < 2; i++) {
        itp_frame_item_tp last = list.first;
        while (last) {
            if (last->frame->command == 2) {
                printf("Erasing frame %d\r\n", last->frame->command);
                last = itp_erase_frame_item(&list, last->frame);
            } else {
                printf("Skip frame %d\r\n", last->frame->command);
                last = last->next;
            }
        }
    }
    itp_clear_frame_list(&list);
    return 0;
}
