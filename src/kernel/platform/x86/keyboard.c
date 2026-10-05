#include "../../keyboard.h"

#include "int/irq.h"
#include "io.h"
#include "ps2.h"

static enum {
    NORMAL,
    READ_EXTENDED_1,
    READ_EXTENDED_2,
} state;

__attribute__((nonstring)) static const u8 lower_chars[SCANCODE_COUNT] =
    "\000\0001234567890-=\b\tqwertyuiop[]\n\000asdfghjkl;'`\000\\zxcvbnm,./\000"
    "*\000 \000\0\0\0\0\0\0\0\0\0\0\000\000789-456+1230.\0\0\0\0\0";
__attribute__((nonstring)) static const u8 upper_chars[SCANCODE_COUNT] =
    "\000\000!@#$%^&*()_+\b\tQWERTYUIOP{}\n\000ASDFGHJKL:\"~\000|ZXCVBNM<>?\000"
    "*\000 \000\0\0\0\0\0\0\0\0\0\0\000\000789-456+1230.\0\0\0\0\0";

static bool key_presses[SCANCODE_COUNT];
static bool caps_lock, num_lock, scroll_lock;

KeyCodeHandlerFn key_code_handler;
KeyCharHandlerFn key_char_handler;

static void keyboard_handler()
{
    switch (state)
    {
    case NORMAL:
    {
        u8 event = io_inb(PS2_PORT_DATA);
        if (event == 0xE0) state = READ_EXTENDED_1;
        else if (event == 0xE1) state = READ_EXTENDED_2;
        else if (event < SCANCODE_COUNT)
        {
            u8 key = event;
            if (!key_presses[key])
            {
                switch (key)
                {
                case KEY_CAPS_LOCK:
                    caps_lock = !caps_lock;
                    break;
                case KEY_NUM_LOCK:
                    num_lock = !num_lock;
                    break;
                case KEY_SCROLL_LOCK:
                    scroll_lock = !scroll_lock;
                    break;
                }

                key_presses[key] = true;
                key_code_handler(key, true);
            }

            bool upper = key_presses[KEY_LSHIFT] || key_presses[KEY_RSHIFT] || caps_lock;
            u8 key_char = upper ? upper_chars[key] : lower_chars[key];
            if (key_char) key_char_handler(key_char);
        }
        else if (event > 0x80 && event < SCANCODE_COUNT + 0x80)
        {
            u8 key = event - 0x80;
            if (key_presses[key])
            {
                key_presses[key] = false;
                key_code_handler(key, false);
            }
        }
        break;
    }
    case READ_EXTENDED_1:
        io_inb(PS2_PORT_DATA); // ignore special keys
        state = NORMAL;
        break;
    case READ_EXTENDED_2:
        io_inw(PS2_PORT_DATA); // ignore special keys
        state = NORMAL;
        break;
    }
}

void keyboard_init()
{
    irq_register(1, &keyboard_handler);
    io_outb(PS2_PORT_DATA, 0xEE);
}

bool keyboard_is_down(KeyCode key)
{
    return key_presses[key];
}
