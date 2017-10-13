.intel_syntax noprefix

#  
# This file implements context functionality for UNIX x64 systems that support
# the System V ABI
#

    .set context_esp, 0
    .set context_ebp, context_esp + 4
    .set context_ebx, context_ebp + 4
    .set context_num_saved_variables, context_ebx + 4

.section .text
    .extern malloc
    .extern free
  
#=======================================================================================
#
# coroutine_t coroutine_new(coroutine_entry_point point, size_t stack_size)
# - [esp+4]: point
# - [esp+8]: stack_size
# - eax: new coroutine_t*
#
#=======================================================================================

    .global coroutine_new
coroutine_new:
    # allocate memory for the context data plus the stack
    mov ecx, [esp+8]
    add ecx, context_num_saved_variables  # allocate everything in a single chunk
    push ecx
    call malloc
    pop ecx
    
    # If allocation fails, return early
    test eax, eax
    jz early_return
    
    # Initialize the stack for a stub entry point
    lea ecx, [eax + ecx-12]
    
    mov edx, [esp+4]
    mov [ecx+8], edx                       # Bottom of stack: return pointer
    mov [ecx+4], eax                       # Middle of stack: context_t parameter
    lea edx, entry_point                   # 
    mov [ecx], edx                         # Top of stack: entry_point stub
    
    mov [eax + context_esp], ecx           # Move the stack pointer to its place
    
early_return:
    ret

#=======================================================================================
# This implements the stub entry point for proper funcion entering
#=======================================================================================
entry_point:
    # assemble the function's parameters
    # [esp] must be the coroutine_t
    # [esp+4] must be the void* param
    xchg [esp], eax
    push eax
    call [esp+8]
    
    # retrieve the parameters again
    mov ecx, [esp]
    
    # invalidate the own method's stack - this is safe
    mov esp, 0
    
    # yield a last time
    jmp cryield

#=======================================================================================
#
# void* coroutine_resume(coroutine_t coro, void* param);
# void* coroutine_yield(coroutine_t coro, void* param);
# - [esp+4]: coro
# - [esp+8]: param
# - eax: param returned from the entry point
#
#=======================================================================================
    .global coroutine_resume
coroutine_resume:
    .global coroutine_yield
coroutine_yield:
    cmp dword ptr [esp+4], 0 # Careful with nullptr's
    jz return_nullptr
    
    # If the coroutine is ended, return
    mov ecx, [esp+4]
    cmp dword ptr [ecx + context_esp], 0
    jz return_nullptr
    
    # Set the return value as the param passed
    mov eax, [esp+8]
    
cryield:
    # Exchange callee-saved registers
    xchg [ecx + context_esp], esp
    xchg [ecx + context_ebp], ebp
    xchg [ecx + context_ebx], ebx
    ret
    
return_nullptr:
    mov eax, 0
    ret

#=======================================================================================
#
# int is_coroutine_ended(coroutine_t coro)
# - [esp+4]: coro
# - eax: 1 if coroutine is ended, 0 if it is not
#
#=======================================================================================
    .global is_coroutine_ended
is_coroutine_ended:
    cmp dword ptr [esp+4], 0 # Careful with nullptr's
    jz coroutine_is_ended
    
    mov ecx, [esp+4]
    cmp dword ptr [ecx + context_esp], 0
    jz coroutine_is_ended
    mov eax, 0
    ret

coroutine_is_ended:
    mov eax, 1
    ret
    
#=======================================================================================
#
# void coroutine_destroy(coroutine_t coro)
# - edi: coro
#
#=======================================================================================
    .global coroutine_destroy
coroutine_destroy:
    cmp dword ptr [esp+4], 0 # Careful with nullptr's
    jz dont_delete_unended_coroutine

    mov ecx, [esp+4]
    cmp dword ptr [ecx + context_esp], 0
    jnz dont_delete_unended_coroutine
    
    # Free the coroutine
    push ecx
    call free
    pop ecx
dont_delete_unended_coroutine:
    ret
    
