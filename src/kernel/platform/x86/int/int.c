#include "int.h"

#include "../../../panic.h"
#include "../../../tty.h"

typedef struct __attribute__((packed))
{
    u16 addr_low;
    u16 segment;
    u8 _;
    u8 flags;
    u16 addr_high;
} GateDescriptor;

typedef enum
{
    GATE_TYPE_TASK = 0x5,
    GATE_TYPE_INT16 = 0x6,
    GATE_TYPE_TRAP16 = 0x7,
    GATE_TYPE_INT32 = 0xE,
    GATE_TYPE_TRAP32 = 0xF,

    GATE_DPL0 = 0 << 5,
    GATE_DPL1 = 1 << 5,
    GATE_DPL2 = 2 << 5,
    GATE_DPL3 = 3 << 5,

    GATE_PRESENT = 0x80,
} GateFlags;

static GateDescriptor idt[256];

struct __attribute__((packed))
{
    u16 size;
    usz addr;
} idt_descriptor = {.size = sizeof(idt) - 1, .addr = (usz)idt};

IntHandlerFn isr_handlers[256];

extern void _isr0(RegState *state);
extern void _isr1(RegState *state);
extern void _isr8(RegState *state);
extern void _isr9(RegState *state);
extern void int_lidt();

#define ISR_PRIMARY_HANDLERS_START (usz)(&_isr0)
#define ISR_PRIMARY_HANDLER_SIZE (usz)((u8 *)&_isr1 - (u8 *)&_isr0)
#define ISR_PRIMARY_HANDLER_SIZE2 (usz)((u8 *)&_isr9 - (u8 *)&_isr8)

static const char *exception_names[] = {"Divide by zero error",
                                        "Debug",
                                        "Non-maskable Interrupt",
                                        "Breakpoint",
                                        "Overflow",
                                        "Bound Range Exceeded",
                                        "Invalid Opcode",
                                        "Device Not Available",
                                        "Double Fault",
                                        "Coprocessor Segment Overrun",
                                        "Invalid TSS",
                                        "Segment Not Present",
                                        "Stack-Segment Fault",
                                        "General Protection Fault",
                                        "Page Fault",
                                        "",
                                        "x87 Floating-Point Exception",
                                        "Alignment Check",
                                        "Machine Check",
                                        "SIMD Floating-Point Exception",
                                        "Virtualization Exception",
                                        "Control Protection Exception ",
                                        "",
                                        "",
                                        "",
                                        "",
                                        "",
                                        "",
                                        "Hypervisor Injection Exception",
                                        "VMM Communication Exception",
                                        "Security Exception",
                                        ""};

static void default_handler(RegState *state)
{
    tty.color = tty_color(BLACK, YELLOW);
    printf(&tty, "\x1EwUnhandled interrupt #%u.\n", state->int_num);
}

static void exception_handler(RegState *state)
{
    printf(&tty,
           TTY_ERR "Exception #%u with error code 0x%8x: %s!\n"
                   "eax=0x%8x ebx=0x%8x ecx=0x%8x edx=0x%8x\n"
                   "ebp=0x%8x esp=0x%8x esi=0x%8x edi=0x%8x\n"
                   "cs=0x%4x ds=0x%4x es=0x%4x fs=0x%4x gs=0x%4x ss=0x%4x\n"
                   "eip=0x%8x eflags=0x%8x, new_esp=0x%8x\n\n",
           state->int_num, state->err, exception_names[state->int_num], state->eax, state->ebx, state->ecx, state->edx,
           state->ebp, state->esp, state->esi, state->edi, state->cs, state->ds, state->es, state->fs, state->gs,
           state->ss, state->eip, state->eflags, state->_);
    panic("Halting.\n");
}

void int_init()
{
    for (u16 i = 0; i < 256; i++)
    {
        usz addr = ISR_PRIMARY_HANDLERS_START + i * ISR_PRIMARY_HANDLER_SIZE;
        idt[i] = (GateDescriptor){
            .addr_low = addr & 0xFFFF,
            .segment = 0x08, // XXX: hardoding is bad!
            ._ = 0,
            .flags = GATE_TYPE_INT32 | GATE_DPL0 | GATE_PRESENT, // XXX: bad, bad and bad.
            .addr_high = addr >> 16,
        };
    }

    for (u16 i = 0; i < 32; i++) int_register(i, &exception_handler);
    for (u16 i = 32; i < 256; i++) int_register(i, &default_handler);

    __asm__ volatile("lidt idt_descriptor");
}

void int_register(u8 num, IntHandlerFn handler)
{
    isr_handlers[num] = handler;
}
