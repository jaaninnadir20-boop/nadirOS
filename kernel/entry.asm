; nadirOS kernel entry point (x86_64, Limine boot protocol).
; Limine enters here in long mode with paging enabled and interrupts disabled.

global kmain
extern kernel_main

section .text.entry
kmain:
    cli
    cld
    mov rsp, stack_top
    xor ebp, ebp            ; terminate frame-pointer chains for backtraces
    call kernel_main

.hang:
    cli
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

section .note.GNU-stack noalloc noexec nowrite progbits
