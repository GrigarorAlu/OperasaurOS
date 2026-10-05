#ifndef PS2_H
#define PS2_H

#include <types.h>

#define PS2_PORT_CMD 0x64
#define PS2_PORT_DATA 0x60

#define PS2_CMD_GET_CONFIG 0x20
#define PS2_CMD_SET_CONFIG 0x60
#define PS2_CMD_TEST_CONTROLLER 0xAA
#define PS2_CMD_TEST_DEV1 0xAB
#define PS2_CMD_TEST_DEV2 0xA9
#define PS2_CMD_DISABLE_DEV1 0xAD
#define PS2_CMD_DISABLE_DEV2 0xA7
#define PS2_CMD_ENABLE_DEV1 0xAE
#define PS2_CMD_ENABLE_DEV2 0xA8
#define PS2_CMD_SELECT_DEV2 0xD4
#define PS2_CMD_SYSTEM_RESET 0xFE
#define PS2_CMD_DEV_RESET 0xFF

typedef enum
{
    PS2_ERROR,
    PS2_KEYBOARD,
    PS2_MOUSE,
    PS2_KEYBOARD_MOUSE,
} PS2InitStatus;

u8 ps2_init();

void ps2_system_reset();

#endif
