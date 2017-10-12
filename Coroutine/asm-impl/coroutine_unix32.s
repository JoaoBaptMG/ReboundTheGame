.intel_syntax noprefix

#  
# This file implements context functionality for UNIX i386 systems that support
# the System V ABI
#
 
    .set context_entry_point, 0
    .set context_esp, context_entry_point + 4
    .set context_ebp, context_esp + 4
    .set context_ebx, context_ebp + 4
    .set context_stack_size, context_ebx + 4
    .set context_num_saved_variables, context_stack_size + 4

.section .text
    .extern malloc
    .extern free
    
    #
    # coroutine_t* coroutine_new(coroutine_entry_point point, size_t stack_size)
    # - [esp+4]: point
    # - [esp+8]: stack_size
    # - eax: new coroutine_t*
    #
    .global coroutine_new
coroutine_new:
    # allocate memory for the context data plus the stack
    mov ecx, [esp+8]
    add ecx, context_num_saved_variables
    push ecx
    call malloc
    add esp, 4
    
    # If allocation fails, return early
    test eax, eax
    jz early_return
    
    mov ecx, [esp+4]
    mov [eax + context_entry_point], ecx             # move the entry_point into place
    
    mov ecx, [esp+8]
    mov [eax + context_stack_size], ecx      # move the stack_size into its place 
    mov dword ptr [eax + context_esp], 0     # set the rsp to 0 (for not initialized)
    
early_return:
    ret
    
    #
    # void* coroutine_resume(coroutine_t coro, void* param)
    # - [esp+4]: coro
    # - [esp+8]: param
    # - eax: param returned from the entry point
    #
    .global coroutine_resume
coroutine_resume:
    cmp dword ptr [esp+4], 0 # Careful with nullptr's
    jz return_nullptr
    mov ecx, [esp+4]

    cmp dword ptr [ecx + context_entry_point], 0
    jz dont_resume_ended_coroutine
    
    # If the coroutine is initialized, just yield to it
    cmp dword ptr [ecx + context_esp], 0
    jnz coroutine_yield

    # move callee-preserved registers to context
    mov [ecx + context_ebp], ebp
    mov [ecx + context_ebx], ebx

    # move the parameter to edx
    mov edx, [esp+8]

    # CALL the function as if on the new stack, so it can be used
    # a small remark: rsi stays the same (the void* param passed)
    mov [ecx + context_esp], esp
    mov esp, [ecx + context_stack_size]
    lea esp, [esp + ecx + context_num_saved_variables]
    
    # call the coroutine entry point
    push edx
    push ecx
    call [ecx + context_entry_point]
    pop ecx
    add esp, 4
    
    # unexchange the stack
    mov esp, [ecx + context_esp]
    
    # end the coroutine
    mov qword ptr [ecx + context_entry_point], 0
    
    # return the same value as the last coroutine return
    ret
    
return_nullptr:
dont_resume_ended_coroutine:
    mov eax, 0
    ret
    
    #
    # void* coroutine_yield(coroutine_t coro, void* param)
    # - [esp+4]: coro
    # - [esp+8]: param
    # - [eax]: param passed to last context_yield
    #
    .global coroutine_yield
coroutine_yield:
    cmp dword ptr [esp+4], 0 # Careful with nullptr's
    jz return_nullptr
    
    mov ecx, [esp+4]
    mov eax, [esp+8]
    
    # Exchange callee-saved registers
    xchg [ecx + context_esp], esp
    xchg [ecx + context_ebp], ebp
    xchg [ecx + context_ebx], ebx

    ret
    
    #
    # int is_coroutine_ended(coroutine_t coro)
    # - [esp+4]: coro
    # - eax: 1 if coroutine is ended, 0 if it is not
    #
    .global is_coroutine_ended
is_coroutine_ended:
    cmp dword ptr [esp+4], 0 # Careful with nullptr's
    jz coroutine_is_ended
    mov ecx, [esp+4]

    cmp qword ptr [ecx + context_entry_point], 0
    jz coroutine_is_ended
    mov eax, 0
    ret

coroutine_is_ended:
    mov eax, 1
    ret
    
    #
    # void coroutine_destroy(coroutine_t coro)
    # - [esp+4]: coro
    #
    .global coroutine_destroy
coroutine_destroy:
    cmp dword ptr [esp+4], 0 # Careful with nullptr's
    jz dont_delete_unended_coroutine

    mov ecx, [esp+4]
    cmp qword ptr [ecx + context_entry_point], 0
    jnz dont_delete_unended_coroutine
    
    push ecx
    call free
    pop ecx
dont_delete_unended_coroutine:
    ret

