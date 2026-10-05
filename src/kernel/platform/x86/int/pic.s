.text

.equ pic1_cmd, 0x0020
.equ pic1_dat, 0x0021
.equ pic2_cmd, 0x00A0
.equ pic2_dat, 0x00A1

.equ offset1, 0x20
.equ offset2, 0x28

.equ cascade_irq, 2

.equ eoi, 0x20

.macro iowait
    outb %al, $0x80
.endm

.global pic_init
pic_init:
    pushw %ax

    // Start initialization
    movb $0x11, %al
    outb %al, $pic1_cmd
    iowait
    outb %al, $pic2_cmd
    iowait

    // Remap IRQs
    movb $offset1, %al
    outb %al, $pic1_dat
    iowait
    movb $offset2, %al
    outb %al, $pic2_dat
    iowait

    // Configure cascading
    movb $(1 << cascade_irq), %al
    outb %al, $pic1_dat
    iowait
    movb $cascade_irq, %al
    outb %al, $pic2_dat
    iowait

    // 8086 mode
    movb $0x01, %al
    outb %al, $pic1_dat
    iowait
    outb %al, $pic2_dat
    iowait

    // Unmask
    movb $0x00, %al
    outb %al, $pic1_dat
    outb %al, $pic2_dat

    popw %ax
    ret

.global pic_send_eoi
pic_send_eoi:
    pushw %ax
    movb $eoi, %al

    cmp $8, 4(%esp)
    jl .no_slave
    outb %al, $pic2_cmd
    .no_slave:
    outb %al, $pic1_cmd

    popw %ax
    ret

