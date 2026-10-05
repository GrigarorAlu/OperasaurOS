#include "../../tty.h"

static TtyChar *tty_buffer = (void *)0xB8000;
static usz tty_pos;

static u16 tty_width = 80;
static u16 tty_height = 25;

static isz tty_read(void *self, void *buffer, usz size)
{
    return 0;
}

static isz tty_write(void *self, const void *buffer, usz size)
{
    Tty *tty = self;

    for (usz i = 0; i < size; i++)
    {
        // TODO: it shouldn't be based on platform!
        u8 c = ((u8 *)buffer)[i];
        static u8 state = 0;

        switch (state)
        {
        case 0:
            switch (c)
            {
            case '\b':
                tty_buffer[--tty_pos] = (TtyChar){.glyph = ' ', .color = tty->color};
                break;
            case '\t':
                do
                {
                    tty_buffer[tty_pos++] = (TtyChar){.glyph = ' ', .color = tty->color};
                } while (tty_pos % 4 > 0);
                break;
            case '\r':
                tty_pos -= tty_pos % tty_width;
                break;
            case '\n':
                tty_pos += tty_width;
                tty_pos -= tty_pos % tty_width;
                break;
            case '\033':
                state = 1;
                break;
            default:
                tty_buffer[tty_pos++] = (TtyChar){.glyph = c, .color = tty->color};
            }
            break;
        case 1:
            state = 0;
            switch (c)
            {
            case 'd':
                tty->color = TTY_COLOR_DEBUG;
                break;
            case 'i':
                tty->color = TTY_COLOR_INFO;
                break;
            case 'w':
                tty->color = TTY_COLOR_WARN;
                break;
            case 'e':
                tty->color = TTY_COLOR_ERR;
                break;
            case 'c':
                tty->color = TTY_COLOR_CRIT;
                break;
            default:
                state = 2;
            }
            break;
        case 2:
            state = 0;
            tty->color = c;
            break;
        }
    }
    return size;
}

Tty tty = {.stream = {.read = &tty_read, .write = &tty_write}, .color = TTY_COLOR_INFO};

void tty_init()
{
    for (u16 i = 0; i < tty_width * tty_height; i++) tty_buffer[i] = (TtyChar){0, 0};
}
