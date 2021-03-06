/**********************************************************************
 *              Interrupt Service Routines (ISRs)
 *
 *
 *  This file is part of Aquila OS and is released under the terms of
 *  GNU GPLv3 - See LICENSE.
 *
 *  Copyright (C) 2016 Mohamed Anwar <mohamed_anwar@opmbx.org>
 */

#include <core/system.h>
#include <core/arch.h>

#include <core/panic.h>
#include <core/string.h>
#include <cpu/cpu.h>
#include <sys/sched.h>
#include <mm/mm.h>

extern void __x86_isr0 (void);
extern void __x86_isr1 (void);
extern void __x86_isr2 (void);
extern void __x86_isr3 (void);
extern void __x86_isr4 (void);
extern void __x86_isr5 (void);
extern void __x86_isr6 (void);
extern void __x86_isr7 (void);
extern void __x86_isr8 (void);
extern void __x86_isr9 (void);
extern void __x86_isr10(void);
extern void __x86_isr11(void);
extern void __x86_isr12(void);
extern void __x86_isr13(void);
extern void __x86_isr14(void);
extern void __x86_isr15(void);
extern void __x86_isr16(void);
extern void __x86_isr17(void);
extern void __x86_isr18(void);
extern void __x86_isr19(void);
extern void __x86_isr20(void);
extern void __x86_isr21(void);
extern void __x86_isr22(void);
extern void __x86_isr23(void);
extern void __x86_isr24(void);
extern void __x86_isr25(void);
extern void __x86_isr26(void);
extern void __x86_isr27(void);
extern void __x86_isr28(void);
extern void __x86_isr29(void);
extern void __x86_isr30(void);
extern void __x86_isr31(void);
extern void __x86_isr128(void);

/* Refer to 
 * - Intel 64 and IA-32 Architectures Software Developer’s Manual
 * - Volume 3: System Programming Guide
 * - Table 6-1. Protected-Mode Exceptions and Interrupts
 */

static const char *int_msg[32] = {
    /* 0x00 */ "#DE: Divide Error",
    /* 0x01 */ "#DB: Debug Exception",
    /* 0x02 */ "NMI Interrupt",
    /* 0x03 */ "#BP: Breakpoint",
    /* 0x04 */ "#OF: Overflow",
    /* 0x05 */ "#BR: BOUND Range Exceeded",
    /* 0x06 */ "#UD: Invalid Opcode (Undefined Opcode)",
    /* 0x07 */ "#NM: Device Not Available (No Math Coprocessor)",
    /* 0x08 */ "#DF: Double Fault",
    /* 0x09 */ "Coprocessor Segment Overrun (reserved)",
    /* 0x0a */ "#TS: Invalid TSS",
    /* 0x0b */ "#NP: Segment Not Present",
    /* 0x0C */ "#SS: Stack-Segment Fault",
    /* 0x0D */ "#GP: General Protection",
    /* 0x0E */ "#PF: Page Fault",
    /* 0x0F */ "Reserved",
    /* 0x10 */ "#MF: x87 FPU Floating-Point Error (Math Fault)",
    /* 0x11 */ "#AC: Alignment Check",
    /* 0x12 */ "#MC: Machine Check",
    /* 0x13 */ "#XM: SIMD Floating-Point Exception",
    /* 0x14 */ "#VE: Virtualization Exception",
    /* 0x15 */ "Reserved",
    /* 0x16 */ "Reserved",
    /* 0x17 */ "Reserved",
    /* 0x18 */ "Reserved",
    /* 0x19 */ "Reserved",
    /* 0x1A */ "Reserved",
    /* 0x1B */ "Reserved",
    /* 0x1C */ "Reserved",
    /* 0x1D */ "Reserved",
    /* 0x1E */ "Reserved",
    /* 0x1F */ "Reserved"
};

void __x86_isr(struct x86_regs *regs)
{
    extern uint32_t int_num;
    extern uint32_t err_num;

    //printk("__x86_isr %d\n",int_num);
    //x86_dump_registers(regs);

    if (int_num == 0xE && cur_thread) {// && regs->cs == X86_CS) {   /* Page fault from user-space */

        x86_thread_t *arch = cur_thread->arch;
        //arch->regs = regs;

        if (regs->eip == 0x0FFF) {  /* Signal return */
            //printk("Returned from signal [regs=%p]\n", regs);

            /* Fix kstack and regs pointers*/
            arch->regs = (struct x86_regs *) arch->kstack;
            arch->kstack += sizeof(struct x86_regs); 
            x86_kernel_stack_set(arch->kstack);

            extern void return_from_signal(uintptr_t) __attribute__((noreturn));
            return_from_signal((uintptr_t) arch->regs);
        }

        //pmman.handle_page_fault(read_cr2());
        arch_mm_page_fault(read_cr2());
        return;
    }

    if (int_num == 0x07) {  /* FPU Trap */
        trap_fpu();
        return;
    }
    
    if (int_num == 0x80) {  /* syscall */
        x86_thread_t *arch = cur_thread->arch;
        arch->regs = regs;
        arch_syscall(regs);
        return;
    }


    if (int_num < 32) {
        const char *msg = int_msg[int_num];
        printk("Recieved interrupt %d [err=%d]: %s\n", int_num, err_num, msg);

        if (int_num == 0x0E) { /* Page Fault */
            printk("CR2 = %p\n", read_cr2());
        }
        x86_dump_registers(regs);
        panic("Kernel Exception");
    } else {
        printk("Unhandled interrupt %d\n", int_num);
        panic("Kernel Exception");
    }
}

void x86_isr_setup()
{   
    x86_idt_gate_set(0x00, (uint32_t) __x86_isr0);
    x86_idt_gate_set(0x01, (uint32_t) __x86_isr1);
    x86_idt_gate_set(0x02, (uint32_t) __x86_isr2);
    x86_idt_gate_set(0x03, (uint32_t) __x86_isr3);
    x86_idt_gate_set(0x04, (uint32_t) __x86_isr4);
    x86_idt_gate_set(0x05, (uint32_t) __x86_isr5);
    x86_idt_gate_set(0x06, (uint32_t) __x86_isr6);
    x86_idt_gate_set(0x07, (uint32_t) __x86_isr7);
    x86_idt_gate_set(0x08, (uint32_t) __x86_isr8);
    x86_idt_gate_set(0x09, (uint32_t) __x86_isr9);
    x86_idt_gate_set(0x0A, (uint32_t) __x86_isr10);
    x86_idt_gate_set(0x0B, (uint32_t) __x86_isr11);
    x86_idt_gate_set(0x0C, (uint32_t) __x86_isr12);
    x86_idt_gate_set(0x0D, (uint32_t) __x86_isr13);
    x86_idt_gate_set(0x0E, (uint32_t) __x86_isr14);
    x86_idt_gate_set(0x0F, (uint32_t) __x86_isr15);
    x86_idt_gate_set(0x10, (uint32_t) __x86_isr16);
    x86_idt_gate_set(0x11, (uint32_t) __x86_isr17);
    x86_idt_gate_set(0x12, (uint32_t) __x86_isr18);
    x86_idt_gate_set(0x13, (uint32_t) __x86_isr19);
    x86_idt_gate_set(0x14, (uint32_t) __x86_isr20);
    x86_idt_gate_set(0x15, (uint32_t) __x86_isr21);
    x86_idt_gate_set(0x16, (uint32_t) __x86_isr22);
    x86_idt_gate_set(0x17, (uint32_t) __x86_isr23);
    x86_idt_gate_set(0x18, (uint32_t) __x86_isr24);
    x86_idt_gate_set(0x19, (uint32_t) __x86_isr25);
    x86_idt_gate_set(0x1A, (uint32_t) __x86_isr26);
    x86_idt_gate_set(0x1B, (uint32_t) __x86_isr27);
    x86_idt_gate_set(0x1C, (uint32_t) __x86_isr28);
    x86_idt_gate_set(0x1D, (uint32_t) __x86_isr29);
    x86_idt_gate_set(0x1E, (uint32_t) __x86_isr30);
    x86_idt_gate_set(0x1F, (uint32_t) __x86_isr31);
    x86_idt_gate_user_set(0x80, (uint32_t) __x86_isr128);
}
