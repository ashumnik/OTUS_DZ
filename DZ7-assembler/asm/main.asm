;;; односвязный список

    bits 64 									;;; 64 битная машина
    extern malloc, puts, printf, fflush, abort	;;; определены в другом месте
    global main									;;; противоволожность extern, должен быть тут определен

    section   .data								;;; КОНСТАНТЫ
empty_str: db 0x0
int_format: db "%ld ", 0x0
data: dq 4, 8, 15, 16, 23, 42
data_length: equ ($-data) / 8					;;; $ - текущий адрес, $-data - длина строки в битах

    section   .text								;;; ФАКТИЧЕСКИЙ КОД
;;; print_int proc
print_int:
    mov rsi, rdi								;;; mov x, y: из y записать в x
    mov rdi, int_format							;;; mov x, y: из y записать в x
    xor rax, rax								;;; xor используется для очистки регистра
    call printf									;;; call вызывает функцию

    xor rdi, rdi								;;; xor используется для очистки регистра
    call fflush									;;; call вызывает функцию

    ret

;;; p proc
p:
    mov rax, rdi								;;; mov x, y: из y записать в x
    and rax, 1									;;; and логическое и
    ret

;;; add_element proc
add_element:
    push rbp									;;; помещение операнда в стек
    push rbx									;;; помещение операнда в стек

    mov rbp, rdi								;;; mov x, y: из y записать в x
    mov rbx, rsi								;;; mov x, y: из y записать в x

    mov rdi, 16									;;; mov x, y: из y записать в x
    call malloc									;;; call вызывает функцию
    test rax, rax								;;; по сути if наверно типа после malloc
    jz abort

    mov [rax], rbp								;;; mov x, y: из y записать в x
    mov [rax + 8], rbx							;;; mov x, y: из y записать в x

    pop rbx										;;; извлечение операнда из стека
    pop rbp										;;; извлечение операнда из стека

    ret

;;; m proc
m:
    test rdi, rdi
    jz outm

    push rbp									;;; помещение операнда в стек
    push rbx									;;; помещение операнда в стек

    mov rbx, rdi								;;; mov x, y: из y записать в x
    mov rbp, rsi								;;; mov x, y: из y записать в x

    mov rdi, [rdi]								;;; mov x, y: из y записать в x
    call rsi									;;; call вызывает функцию

    mov rdi, [rbx + 8]							;;; mov x, y: из y записать в x
    mov rsi, rbp								;;; mov x, y: из y записать в x
    call m										;;; call вызывает функцию

    pop rbx										;;; извлечение операнда из стека
    pop rbp										;;; извлечение операнда из стека

outm:
    ret

;;; f proc
f:
    mov rax, rsi								;;; mov x, y: из y записать в x

    test rdi, rdi
    jz outf

    push rbx									;;; помещение операнда в стек
    push r12									;;; помещение операнда в стек
    push r13									;;; помещение операнда в стек

    mov rbx, rdi								;;; mov x, y: из y записать в x
    mov r12, rsi								;;; mov x, y: из y записать в x
    mov r13, rdx								;;; mov x, y: из y записать в x

    mov rdi, [rdi]								;;; mov x, y: из y записать в x
    call rdx									;;; call вызывает функцию
    test rax, rax
    jz z

    mov rdi, [rbx]								;;; mov x, y: из y записать в x
    mov rsi, r12								;;; mov x, y: из y записать в x
    call add_element							;;; call вызывает функцию
    mov rsi, rax								;;; mov x, y: из y записать в x
    jmp ff

z:
    mov rsi, r12								;;; mov x, y: из y записать в x	

ff:
    mov rdi, [rbx + 8]							;;; mov x, y: из y записать в x
    mov rdx, r13								;;; mov x, y: из y записать в x
    call f										;;; call вызывает функцию

    pop r13										;;; извлечение операнда из стека
    pop r12										;;; извлечение операнда из стека
    pop rbx										;;; извлечение операнда из стека

outf:
    ret

;;; main proc
main:
    push rbx									;;; помещение операнда в стек

    xor rax, rax								;;; xor используется для очистки регистра
    mov rbx, data_length						;;; mov x, y: из y записать в x
adding_loop:
    mov rdi, [data - 8 + rbx * 8]				;;; mov x, y: из y записать в x
    mov rsi, rax								;;; mov x, y: из y записать в x
    call add_element							;;; call вызывает функцию
    dec rbx										;;; вычесть 1
    jnz adding_loop								;;; грубо говоря завершение цикла

    mov rbx, rax								;;; mov x, y: из y записать в x

    mov rdi, rax								;;; mov x, y: из y записать в x
    mov rsi, print_int							;;; mov x, y: из y записать в x
    call m										;;; call вызывает функцию

    mov rdi, empty_str							;;; mov x, y: из y записать в x
    call puts									;;; call вызывает функцию

    mov rdx, p									;;; mov x, y: из y записать в x
    xor rsi, rsi								;;; xor используется для очистки регистра
    mov rdi, rbx								;;; mov x, y: из y записать в x
    call f										;;; call вызывает функцию

    mov rdi, rax								;;; mov x, y: из y записать в x
    mov rsi, print_int							;;; mov x, y: из y записать в x
    call m										;;; call вызывает функцию

    mov rdi, empty_str							;;; mov x, y: из y записать в x
    call puts									;;; call вызывает функцию

    pop rbx										;;; извлечение операнда из стека

    xor rax, rax								;;; xor используется для очистки регистра
    ret
