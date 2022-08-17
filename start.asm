section .text
global  _start
extern main
_start:
    xor ebp, ebp ; mark end of stack frames
    
    mov edi, [esp] ; argc
    lea esi, [esp+4] ; argv
    lea edx, [esp+4+4*edi+4] ; envp_str
    push edx ; envp
    push esi ; argv
    push edi ; argc
    xor eax, eax


    call main ; call C start function(int argc, char* argv[], char* envp_str)
    
    ; exit with return value of main
    mov ebx, eax ; main returns in eax
    mov eax, 1 ; sys_exit
    int 0x80
.loop:
    hlt ; if we are here something went wrong badly
    jmp .loop