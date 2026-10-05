#include "stream.h"

#include "mem.h"
#include "stdarg.h"

isz read(void *dev, void *buffer, usz size)
{
    const Stream *stream = (Stream *)dev;
    return stream->read(dev, buffer, size);
}

isz write(void *dev, const void *buffer, usz size)
{
    const Stream *stream = (Stream *)dev;
    return stream->write(dev, buffer, size);
}

isz get(void *dev, u8 *c)
{
    return read(dev, c, 1);
}

isz put(void *dev, u8 c)
{
    return write(dev, &c, 1);
}

isz gets(void *dev, u8 *str)
{
    isz n = 0;
    while (get(dev, &str[n]) && str[n]) n++;
    str[n++] = 0;
    return n;
}

isz puts(void *dev, const u8 *str)
{
    return write(dev, str, str_len(str));
}

isz printu(void *dev, u32 val, u8 width)
{
    isz len = 0;
    u32 reverse = 0;
    while (val > 0)
    {
        reverse = reverse * 10 + val % 10;
        val /= 10;
        len++;
    }
    for (int i = len; i < width; i++)
        if (put(dev, '0') < 0) return -1;
    for (isz i = 0; i < len; i++)
    {
        if (put(dev, '0' + reverse % 10) < 0) return len;
        reverse /= 10;
    }
    return len;
}

isz printi(void *dev, i32 val, u8 width)
{
    if (val < 0)
    {
        if (put(dev, '-') < 0) return -1;
        return printu(dev, -val, width) + 1;
    }
    return printu(dev, val, width);
}

isz printh(void *dev, u32 val, u8 width)
{
    isz len = 0;
    u32 reverse = 0;
    while (val > 0)
    {
        reverse = reverse * 16 + val % 16;
        val /= 16;
        len++;
    }
    for (int i = len; i < width; i++)
        if (put(dev, '0') < 0) return -1;
    for (isz i = 0; i < len; i++)
    {
        u8 c = reverse % 16;
        reverse /= 16;
        if (put(dev, (c < 10 ? '0' : 'A' - 10) + c) < 0) return -1;
    }
    return len;
}

isz printf(void *dev, const u8 *format, ...)
{
    isz wrote = 0;

    va_list args;
    va_start(args, format);

    while (*format)
    {
        if (*format == '%')
        {
            if (!++format) goto printf_end;

            if (*format == '%')
            {
                if (put(dev, '%') < 0) goto printf_end;
                wrote++;
            }
            else
            {
                i32 width = 0;
                if (*format == '*')
                {
                    width = va_arg(args, i32);
                    if (!*++format) goto printf_end;
                }
                else if (*format >= '0' && *format <= '9')
                {
                    while (*format >= '0' && *format <= '9')
                    {
                        width = width * 10 + *format - '0';
                        if (!*++format) goto printf_end;
                    }
                }
                else width = -1;

                if (*format == 's')
                {
                    u8 *str = va_arg(args, u8 *);
                    isz result = write(dev, str, width < 0 ? (isz)str_len(str) : width);
                    if (result < 0) goto printf_end;
                    wrote += result;
                }
                else
                {
                    if (width < 0) width = 1;
                    usz val = va_arg(args, u32);
                    isz result;
                    switch (*format)
                    {
                    case 'u':
                        result = printu(dev, val, width);
                        break;
                    case 'i':
                        result = printi(dev, *(i32 *)&val, width);
                        break;
                    case 'x':
                        result = printh(dev, val, width);
                        break;
                    default:
                        goto printf_end;
                    }
                    if (result < 0) goto printf_end;
                    wrote += result;
                }
            }
        }
        else
        {
            if (put(dev, *format) < 0) goto printf_end;
            wrote++;
        }
        format++;
    }

printf_end:
    va_end(args);
    return wrote;
}
