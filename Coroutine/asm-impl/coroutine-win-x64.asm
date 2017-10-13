;  
; This file implements context functionality for the Windos x64 calling convention
;

    context_rsp equ 0
    context_rbp equ context_rsp + 8
	context_rdi equ context_rbp + 8
	context_rsi equ context_rdi + 8
    context_r12 equ context_rsi + 8
    context_r13 equ context_r12 + 8
    context_r14 equ context_r13 + 8
    context_r15 equ context_r14 + 8
	context_xmm6 equ context_r15 + 8
	context_xmm7 equ context_xmm6 + 16
	context_xmm8 equ context_xmm7 + 16
	context_xmm9 equ context_xmm8 + 16
	context_xmm10 equ context_xmm9 + 16
	context_xmm11 equ context_xmm10 + 16
	context_xmm12 equ context_xmm11 + 16
	context_xmm13 equ context_xmm12 + 16
	context_xmm14 equ context_xmm13 + 16
	context_xmm15 equ context_xmm14 + 16
	context_rbx equ context_xmm15 + 16
    context_num_saved_variables equ context_rbx + 8

.code
    extern malloc : PROC
    extern free : PROC
  
;=======================================================================================
;
; coroutine_t coroutine_new(coroutine_entry_point point, size_t stack_size)
; - rcx: point
; - rdx: stack_size
; - rax: new coroutine_t*
;
;=======================================================================================

    public coroutine_new
coroutine_new:
    ; allocate memory for the context data plus the stack
    mov [rsp+8], rcx
	mov [rsp+16], rdx
	sub rsp, 32
    lea rcx, [rdx + context_num_saved_variables]  ; allocate everything in a single chunk
    call malloc

    ; If allocation fails, return early
    test rax, rax
    jz early_return
    
    ; Initialize the stack for a stub entry point
	mov rcx, [rsp+40]
	mov rdx, [rsp+48]
    lea rdx, [rax + rdx + context_num_saved_variables-24]
    
    mov [rdx+16], rcx                      ; Bottom of stack: return pointer
    mov [rdx+8], rax                       ; Middle of stack: context_t parameter
    lea rcx, entry_point                   ; 
    mov [rdx], rcx                         ; Top of stack: entry_point stub
    
    mov [rax + context_rsp], rdx           ; Move the stack pointer to its place
    
early_return:
	add rsp, 32
    ret

;=======================================================================================
; This implements the stub entry point for proper funcion entering
;=======================================================================================
entry_point:
    ; assemble the function's parameters
    mov rcx, [rsp]
    mov rdx, rax
	sub rsp, 32
    call qword ptr [rsp+40]
    add rsp, 32

    ; retrieve the parameters again
    mov rcx, [rsp]
    
    ; invalidate the own method's stack - this is safe
    mov rsp, 0
    
    ; yield a last time
    jmp cryield

;=======================================================================================
;
; void* coroutine_resume(coroutine_t coro, void* param);
; void* coroutine_yield(coroutine_t coro, void* param);
; - rdi: coro
; - rsi: param
; - rax: param returned from the entry point
;
;=======================================================================================
    public coroutine_resume
coroutine_resume:
    public coroutine_yield
coroutine_yield:
	mov [rsp+8], rcx
	mov [rsp+16], rcx

    test rcx, rcx ; Careful with nullptr's
    jz return_nullptr
    
    ; If the coroutine is ended, return
    cmp qword ptr [rcx + context_rsp], 0
    jz return_nullptr
    
    ; Set the return value as the param passed
    mov rax, rdx
    
cryield:
    ; Exchange callee-saved registers
    xchg [rcx + context_rsp], rsp
    xchg [rcx + context_rbp], rbp
	xchg [rcx + context_rdi], rdi
	xchg [rcx + context_rsi], rsi
    xchg [rcx + context_r12], r12
    xchg [rcx + context_r13], r13
    xchg [rcx + context_r14], r14
    xchg [rcx + context_r15], r15
    xchg [rcx + context_rbx], rbx

	; Exchange XMM registers (helper macro)
exchange_xmm macro xmmn : REQ, context_xmmn : REQ
	movdqa xmm0, xmmn
	movdqa xmmn, [rcx + context_xmmn]
	movdqa [rcx + context_xmmn], xmm0
endm

	; Exchange XMM registers
	exchange_xmm xmm6, context_xmm6
	exchange_xmm xmm7, context_xmm7
	exchange_xmm xmm8, context_xmm8
	exchange_xmm xmm9, context_xmm9
	exchange_xmm xmm10, context_xmm10
	exchange_xmm xmm11, context_xmm11
	exchange_xmm xmm12, context_xmm12
	exchange_xmm xmm13, context_xmm13
	exchange_xmm xmm14, context_xmm14
	exchange_xmm xmm15, context_xmm15
    ret
    
return_nullptr:
    mov rax, 0
    ret

;=======================================================================================
;
; int is_coroutine_ended(coroutine_t coro)
; - rdi: coro
; - rax: 1 if coroutine is ended, 0 if it is not
;
;=======================================================================================
    public is_coroutine_ended
is_coroutine_ended:
    test rcx, rcx
    jz coroutine_is_ended

    cmp qword ptr [rcx + context_rsp], 0
    jz coroutine_is_ended
    mov rax, 0
    ret

coroutine_is_ended:
    mov rax, 1
    ret
    
;=======================================================================================
;
; void coroutine_destroy(coroutine_t coro)
; - rdi: coro
;
;=======================================================================================
    public coroutine_destroy
coroutine_destroy:
	sub rsp, 32
    test rcx, rcx
    jz dont_delete_unended_coroutine

    cmp qword ptr [rcx + context_rsp], 0
    jnz dont_delete_unended_coroutine
    call free
dont_delete_unended_coroutine:
	add rsp, 32
    ret

END
