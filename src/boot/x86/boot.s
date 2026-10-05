.section .boot

.equ kernel_size, 0x800
.equ block_size, 0x40
.equ block_count, kernel_size / block_size
.equ sector_size, 512

.code16
.global _start
_start:
    // Disable interrupts
    cli

    // Setup stack
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw $0x7C00, %sp

    // Print loading string
    movb $0x0E, %ah
    movw $msg, %si
    .msg_loop:
        lodsb
        int $0x10
        testb %al, %al
        jnz .msg_loop

    // Load kernel from disk
    movw $disk_packet, %si
    movw $block_count, %cx
    .disk_loop:
        movb $0x42, %ah
        int $0x13
        jc .disk_end
        addw $(block_size * sector_size), disk_packet_offset
        adcw $0, disk_packet_segment
        addw $block_size, disk_packet_sector
        loop .disk_loop
    .disk_end:

    // Detect memory regions (list will be stored at 0x7C00)
    movw $0x7E08, %di
    movw $1, %si
    xorl %ebx, %ebx
    movl $0x534D4150, %edx
    .mem_loop:
        movl $0xE820, %eax
        movl $24, %ecx
        int $0x15
        jc .mem_end
        cmpl $0, 8(%di)
        jz .no_length
            incw %si
            addw $24, %di
        .no_length:
            testl %ebx, %ebx
            jnz .mem_loop
    .mem_end:
        movl $0x00000000, 0(%di)
        movl $0x00100000, 8(%di)
        movl $0x00000002, 16(%di)
        movw %si, 0x7E00
    
    // Enable A20 line
    inb $0x92, %al
    orb $2, %al
    outb %al, $0x92

    // Enable protected mode
    lgdt gdt_descriptor

    movl %cr0, %eax
    orb $1, %al
    movl %eax, %cr0

    // Yay, 32 bits! :)
    ljmp $(gdt_code - gdt_start), $protected_mode

    .error:
        hlt

.code32
protected_mode:
    // Setting up data segments
    movw $(gdt_data - gdt_start), %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    // Hello, C!
    call 0x10000

msg: .asciz "Loading..."

disk_packet:
    .word 0x0010
    .word block_size
    disk_packet_offset: .word 0x0000
    disk_packet_segment: .word 0x1000
    disk_packet_sector: .quad 1

.align 16
gdt_descriptor:
    .word gdt_end - gdt_start - 1
    .long gdt_start

.align 16
gdt_start:
gdt_null:
    .quad 0
gdt_code:
    .word 0xFFFF
    .word 0x0000
    .byte 0x00
    .byte 0b10011011
    .byte 0b11001111
    .byte 0x00
gdt_data:
    .word 0xFFFF
    .word 0x0000
    .byte 0x00
    .byte 0b10010011
    .byte 0b11001111
    .byte 0x00
gdt_end:

.fill 510 - (. - _start), 1, 0
.word 0xAA55

