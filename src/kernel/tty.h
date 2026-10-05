#ifndef TTY_H
#define TTY_H

#include <stream.h>
#include <types.h>

typedef struct
{
    u8 glyph;
    u8 color;
} TtyChar;

typedef struct
{
    const Stream stream;
    u8 color;
} Tty;

typedef enum
{
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHT_GRAY,
    GRAY,
    LIGHT_BLUE,
    LIGHT_GREEN,
    LIGHT_CYAN,
    LIGHT_RED,
    LIGHT_MAGENTA,
    YELLOW,
    WHITE,
} TtyColor;

extern Tty tty;

void tty_init();

#define tty_color(bg, fg) ((bg) << 4 | (fg))

#define TTY_COLOR_DEBUG tty_color(BLACK, GRAY)
#define TTY_COLOR_INFO tty_color(BLACK, WHITE)
#define TTY_COLOR_WARN tty_color(BLACK, YELLOW)
#define TTY_COLOR_ERR tty_color(BLACK, LIGHT_RED)
#define TTY_COLOR_CRIT tty_color(RED, WHITE)

#define TTY_DEBUG "\033d"
#define TTY_INFO "\033i"
#define TTY_WARN "\033w"
#define TTY_ERR "\033e"
#define TTY_CRIT "\033c"
#define TTY_CUSTOM(color) "\033 " color

#endif
