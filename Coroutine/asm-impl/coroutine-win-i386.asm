;  
; This file implements context functionality for Windows x32 using the cdecl calling convention
;

    context_esp equ 0
    context_ebp equ context_esp + 4
    context_ebx equ context_ebp + 4
    context_num_saved_variables equ context_ebx + 4

	.model flat
	.386

	.code

	extern _malloc : PROC
	extern _free : PROC

;=======================================================================================
;
; coroutine_t coroutine_new(coroutine_entry_point point, size_t stack_size)
; - [esp+4]: point
; - [esp+8]: stack_size
; - eax: new coroutine_t*
;
;=======================================================================================
    public _coroutine_new
_coroutine_new:
    ; allocate memory for the context data plus the stack
    mov ecx, [esp+8]
    add ecx, context_num_saved_variables  ; allocate everything in a single chunk
    push ecx
    call _malloc
    pop ecx
    
    ; If allocation fails, return early
    test eax, eax
    jz early_return
    
    ; Initialize the stack for a stub entry point
    lea ecx, [eax + ecx-12]
    
    mov edx, [esp+4]
    mov [ecx+8], edx                      ; Bottom of stack: return pointer
    mov [ecx+4], eax                       ; Middle of stack: context_t parameter
    lea edx, entry_point                   ; 
    mov [ecx], edx                         ; Top of stack: entry_point stub
    
    mov [eax + context_esp], ecx           ; Move the stack pointer to its place
    
early_return:
    ret

;=======================================================================================
; This implements the stub entry point for proper funcion entering
;=======================================================================================
entry_point:
    ; assemble the function's parameters
    ; [esp] must be the coroutine_t
    ; [esp+4] must be the void* param
    xchg [esp], eax
    push eax
    call dword ptr [esp+8]
    
    ; retrieve the parameters again
    mov ecx, [esp]
    
    ; invalidate the own method's stack - this is safe
    mov esp, 0

    ; yield a last time
    jmp cryield

;=======================================================================================
;
; void* coroutine_resume(coroutine_t coro, void* param);
; void* coroutine_yield(coroutine_t coro, void* param);
; - [esp+4]: coro
; - [esp+8]: param
; - eax: param returned from the entry point
;
;=======================================================================================
    public _coroutine_resume
_coroutine_resume:
    public _coroutine_yield
_coroutine_yield:
    cmp dword ptr [esp+4], 0 ; Careful with nullptr's
    jz return_nullptr
    
    ; If the coroutine is ended, return
    mov ecx, [esp+4]
    cmp dword ptr [ecx + context_esp], 0
    jz return_nullptr
    
    ; Set the return value as the param passed
    mov eax, [esp+8]
    
cryield:
    ; Exchange callee-saved registers
    xchg [ecx + context_esp], esp
    xchg [ecx + context_ebp], ebp
    xchg [ecx + context_ebx], ebx
    ret
    
return_nullptr:
    mov eax, 0
    ret

;=======================================================================================
;
; int is_coroutine_ended(coroutine_t coro)
; - [esp+4]: coro
; - eax: 1 if coroutine is ended, 0 if it is not
;
;=======================================================================================
    public _is_coroutine_ended
_is_coroutine_ended:
    cmp dword ptr [esp+4], 0 ; Careful with nullptr's
    jz coroutine_is_ended
    
    mov ecx, [esp+4]
    cmp dword ptr [ecx + context_esp], 0
    jz coroutine_is_ended
    mov eax, 0
    ret

coroutine_is_ended:
    mov eax, 1
    ret
    
;=======================================================================================
;
; void coroutine_destroy(coroutine_t coro)
; - edi: coro
;
;=======================================================================================
    public _coroutine_destroy
_coroutine_destroy:
    cmp dword ptr [esp+4], 0 ; Careful with nullptr's
    jz dont_delete_unended_coroutine

    mov ecx, [esp+4]
    cmp dword ptr [ecx + context_esp], 0
    jnz dont_delete_unended_coroutine
    
    ; Free the coroutine
    push ecx
    call _free
    pop ecx
dont_delete_unended_coroutine:
    ret

END