#include <initp/tools/crc.h>
#include <initp/tools/format.h>

#include <stdio.h>

int main(int argc, char** argv) {
    char data[] = { 0x31, 0x53 };
    uint16_t crc = sys_eval_crc16(0, data, 2);
    char buffer[0xFF];
    tools_array_to_string((uint8_t*)(&crc), 2, buffer, 0xFF);
    printf("target: 0x55AD; eval: %s", buffer);
    return 0;
}
