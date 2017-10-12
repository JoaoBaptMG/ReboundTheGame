.intel_syntax noprefix

#  
# This file implements context functionality for UNIX x64 systems that support
# the System V ABI
#
 
    .set context_rip, 0
    .set context_rsp, context_rip + 8
    .set context_rbp, context_rsp + 8
    .set context_rbx, context_rbp + 8
    .set context_r12, context_rbx + 8
    .set context_r13, context_r12 + 8
    .set context_r14, context_r13 + 8
    .set context_r15, context_r14 + 8
    .set context_stack_size, context_r15 + 8
    .set context_num_saved_variables, context_stack_size + 8

.section .text
    .extern malloc
    .extern free
    
    #
    # coroutine_t coroutine_new(coroutine_entry_point point, size_t stack_size)
    # - rdi: point
    # - rsi: stack_size
    # - rax: new coroutine_t*
    #
    .global coroutine_new
coroutine_new:
    # allocate memory for the context data plus the stack
    push rdi
    push rsi
    lea rdi, [rsi + context_num_saved_variables+8]  # allocate everything in a single chunk
    call malloc
    pop rsi
    pop rdi
    
    # If allocation fails, return early
    test rax, rax
    jz early_return
    
    mov [rax + context_rip], rdi             # move the entry_point into place
    mov [rax + context_stack_size], rsi      # move the stack_size into its place 
    mov qword ptr [rax + context_rsp], 0     # set the rsp to 0 (for not initialized)
    
early_return:
    ret
    
    #
    # void* coroutine_resume(coroutine_t coro, void* param)#
    # - rdi: coro
    # - rsi: param
    # - rax: param returned from the entry point
    #
    .global coroutine_resume
coroutine_resume:
    test rdi, rdi # Careful with nullptr's
    jz return_nullptr

    cmp qword ptr [rdi + context_rip], 0
    jz dont_resume_ended_coroutine
    
    # If the coroutine is initialized, just yield to it
    cmp qword ptr [rdi + context_rsp], 0
    jnz coroutine_yield

    # move the saved IP to context
    mov r8, [rdi + context_rip]
    mov r9, [rsp-4]
    mov [rdi + context_rip], r9

    # move callee-preserved registers to context
    mov [rdi + context_rbp], rbp
    mov [rdi + context_rbx], rbx
    mov [rdi + context_r12], r12
    mov [rdi + context_r13], r13
    mov [rdi + context_r14], r14
    mov [rdi + context_r15], r15

    # CALL the function as if on the new stack, so it can be used
    # align the stack to 16 bytes
    # a small remark: rsi stays the same (the void* param passed)
    mov [rdi + context_rsp], rsp
    mov rsp, [rdi + context_stack_size]
    lea rsp, [rsp + rdi + context_num_saved_variables]
    and rsp, ~16
    add rsp, 8
    
    # call the coroutine entry point
    push rdi
    call r8
    pop rdi
    
    # unexchange the stack
    mov rsp, [rdi + context_rsp]
    
    # update the return pointer
    mov r8, [rdi + context_rip]
    mov [rsp-4], r8
    
    # end the coroutine
    mov qword ptr [rdi + context_rip], 0
    
    # return the same value as the last coroutine return
    ret

return_nullptr:
dont_resume_ended_coroutine:
    mov rax, 0
    ret
    
    #
    # void* coroutine_yield(coroutine_t coro, void* param)
    # - rdi: coro
    # - rsi: param
    # - rax: param passed to last context_yield
    #
    .global coroutine_yield
coroutine_yield:
    test rdi, rdi # Careful with nullptr's
    jz return_nullptr

    # Here, we are going to save the last saved instruction to memory
    mov r8, [rdi + context_rip]
    mov r9, [rsp-4]
    
    mov [rsp-4], r8 
    mov [rdi + context_rip], r9
    
    # Exchange callee-saved registers
    xchg [rdi + context_rsp], rsp
    xchg [rdi + context_rbp], rbp
    xchg [rdi + context_rbx], rbx
    xchg [rdi + context_r12], r12
    xchg [rdi + context_r13], r13
    xchg [rdi + context_r14], r14
    xchg [rdi + context_r15], r15
    
    # Set the return value as the param passed
    mov rax, rsi
    ret
    
    #
    # int is_coroutine_ended(coroutine_t coro)
    # - rdi: coro
    # - rax: 1 if coroutine is ended, 0 if it is not
    #
    .global is_coroutine_ended
is_coroutine_ended:
    test rax, rax
    jz coroutine_is_ended

    cmp qword ptr [rdi + context_rip], 0
    jz coroutine_is_ended
    mov rax, 0
    ret

coroutine_is_ended:
    mov rax, 1
    ret
    
    #
    # void coroutine_destroy(coroutine_t coro)
    # - rdi: coro
    #
    .global coroutine_destroy
coroutine_destroy:
    test rdi, rdi
    jz dont_delete_unended_coroutine

    cmp qword ptr [rdi + context_rip], 0
    jnz dont_delete_unended_coroutine
    call free
dont_delete_unended_coroutine:
    ret

