#include "ps2.h"

#include "io.h"

#define TEST_SUCCESS 0x55

#define STATUS_OUTPUT_FULL 0x01
#define STATUS_INPUT_FULL 0x02

#define CONFIG_DEV1_INTERRUPT 0x01
#define CONFIG_DEV2_INTERRUPT 0x02
#define CONFIG_DEV1_CLOCK 0x10
#define CONFIG_DEV2_CLOCK 0x20
#define CONFIG_DEV1_TRANSLATE 0x40

u8 ps2_init()
{
    io_outb(PS2_PORT_CMD, PS2_CMD_DISABLE_DEV1);
    io_outb(PS2_PORT_CMD, PS2_CMD_DISABLE_DEV2);

    io_outb(PS2_PORT_CMD, PS2_CMD_TEST_CONTROLLER);
    if (io_inb(PS2_PORT_DATA) != TEST_SUCCESS) return PS2_ERROR;

    io_outb(PS2_PORT_CMD, PS2_CMD_GET_CONFIG);
    u8 config = io_inb(PS2_PORT_DATA);
    u8 features = 0;

    io_outb(PS2_PORT_CMD, PS2_CMD_TEST_DEV1);
    if (io_inb(PS2_PORT_DATA) == 0)
    {
        config |= CONFIG_DEV1_INTERRUPT | CONFIG_DEV1_CLOCK;
        io_outb(PS2_PORT_CMD, PS2_CMD_SET_CONFIG);
        io_outb(PS2_PORT_CMD, config);

        io_outb(PS2_PORT_CMD, PS2_CMD_ENABLE_DEV1);
        for (u16 i = 0; io_inb(PS2_PORT_CMD) & STATUS_INPUT_FULL && i < 65535; i++);
        io_outb(PS2_PORT_DATA, PS2_CMD_DEV_RESET);
        features |= PS2_KEYBOARD;
    }

    /*io_outb(PS2_PORT_CMD, PS2_CMD_TEST_DEV2);
    if (io_inb(PS2_PORT_DATA) == 0)
    {
        config |= CONFIG_DEV2_INTERRUPT | CONFIG_DEV2_CLOCK;
        io_outb(PS2_PORT_CMD, PS2_CMD_SET_CONFIG);
        io_outb(PS2_PORT_CMD, config);

        io_outb(PS2_PORT_CMD, PS2_CMD_ENABLE_DEV2);
        io_outb(PS2_PORT_CMD, PS2_CMD_SELECT_DEV2);
        for (u16 i = 0; io_inb(PS2_PORT_CMD) & STATUS_INPUT_FULL && i < 65535; i++);
        io_outb(PS2_PORT_DATA, PS2_CMD_DEV_RESET);
        features |= PS2_MOUSE;
    }*/

    return features;
}

void ps2_system_reset()
{
    while (io_inb(PS2_PORT_CMD) & STATUS_INPUT_FULL);
    io_outb(PS2_PORT_CMD, PS2_CMD_SYSTEM_RESET);
}
