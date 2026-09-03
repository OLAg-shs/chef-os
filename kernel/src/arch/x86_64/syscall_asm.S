section .data
global g_kernel_syscall_stack
g_kernel_syscall_stack: dq 0
g_user_saved_rsp:       dq 0

section .text
extern syscall_dispatch
global syscall_entry_stub
global user_payload_code
global user_payload_code_end

syscall_entry_stub:
    ; CPU sets: RCX = user RIP, R11 = user RFLAGS, CS = 0x08, SS = 0x10
    ; Save user RSP and switch to kernel syscall stack
    mov [rel g_user_saved_rsp], rsp
    mov rsp, [rel g_kernel_syscall_stack]

    ; Push preserved registers
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15
    push rcx             ; user RIP
    push r11             ; user RFLAGS
    push qword [rel g_user_saved_rsp] ; user RSP

    ; Arguments:
    ; System V Syscall: RAX=num, RDI=arg1, RSI=arg2, RDX=arg3, R10=arg4, R8=arg5, R9=arg6
    ; Function call ABI: RDI=num, RSI=arg1, RDX=arg2, RCX=arg3, R8=arg4, R9=arg5, stack=arg6
    mov rcx, rdx         ; arg3 (RDX -> RCX)
    mov rdx, rsi         ; arg2 (RSI -> RDX)
    mov rsi, rdi         ; arg1 (RDI -> RSI)
    mov rdi, rax         ; syscall num (RAX -> RDI)
    ; arg4 is in R10, move to R8
    mov rax, r8          ; save arg5
    mov r8, r10          ; arg4 (R10 -> R8)
    ; arg5 is in RAX, move to R9
    push r9              ; arg6 onto stack for call
    mov r9, rax          ; arg5 -> R9

    call syscall_dispatch
    add rsp, 8           ; clean up arg6 from stack

    ; RAX contains return value
    ; Restore user stack and registers
    pop r11              ; dummy pop user RSP
    pop r11              ; restore user RFLAGS
    pop rcx              ; restore user RIP
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp

    mov rsp, [rel g_user_saved_rsp]
    o64 sysret

user_payload_code:
    ; SYS_WRITE (#4): RDI=1, RSI=str, RDX=len
    mov rax, 4
    mov rdi, 1
    lea rsi, [rel .user_str]
    mov rdx, .user_str_len
    syscall

.user_loop:
    mov rax, 8           ; SYS_YIELD (#8)
    syscall
    pause
    jmp .user_loop

.user_str:
    db 10, "[USERLAND] Hello from Chef OS User Mode (Ring 3)!", 10
    db "[USERLAND] Fast Syscall (SYS_WRITE #4) Executed Successfully.", 10, 10
.user_str_len equ $ - .user_str

user_payload_code_end:
