[bits 16]         ; Указываем NASM, что код 16-битный
[org 0x7C00]      ; BIOS загружает сектор по этому адресу, отсчитываем метки от него

; Устанавливаем данные для стека загрузчика
pre_set:
    xor ax, ax ; обнуляем AX
    mov ds, ax ; запись указателя сегмента данных
    mov ss, ax ; запись указателя сегмента стека
    mov sp, pre_set+1024+512 ;установка указателя стека

main:
    ; Вывод строки с фамилией и группой
    mov si, student_data
    call string_to_out

    ; Получаем n [0..9], то есть 1 значное число
    xor ah, ah ; 0x00
    int 0x16
    sub al, '0' ; получаем численное значение n

    ; Печатаем само введённое число n
    movzx eax, al         ; обнуляем старшие байты EAX
    push eax              ; Сохраняем n в стек, так как print_number его испортит
    call print_number

    mov si, format_str
    call string_to_out
    
    pop eax
    call factorial
    call print_number

    mov si, msg_before_reboot
    call string_to_out

    mov cl, '5'
    call countdown_loop   ; Функция перезапускает пк поэтому в ней нет ret

string_to_out:
    mov ah, 0x0E ; Переводим в режим вывода
.loop:
    lodsb
    cmp al, 0
    je .end
    int 0x10
    jmp .loop
.end:
    ret 

factorial:
    movzx ecx, al
    mov eax, 1
.loop:
    jecxz .done           ; Jump if EXC Zero
    mul ecx               ; EDX:EAX = EAX * ECX
    loop .loop            ; Сам декрементирует ECX
.done:
    ret

print_number:
    ; Вход: EAX = число
    ; Сохраняем регистры
    push eax
    push ecx
    push edx

    xor ecx, ecx        ; ECX будет счетчиком сохраненных цифр
    mov ebx, 10         ; Делитель

.push_loop:
    xor edx, edx        ; Готовим EDX:EAX к делению
    div ebx             ; EAX = частное, EDX = остаток (цифра)

    push edx            ; Кладем цифру (0..9) в стек
    inc ecx             ; Увеличиваем счетчик цифр

    test eax, eax       ; Осталось ли еще число?
    jnz .push_loop      ; Если EAX != 0, продолжаем делить

.pop_loop:
    pop edx             ; Достаем цифру из стека (в обратном порядке!)
    
    mov al, dl
    add al, '0'         ; Переводим в ASCII
    mov ah, 0x0E        
    int 0x10

    loop .pop_loop      ; Уменьшает ECX на 1. Если ECX != 0, переход на .pop_loop

    pop edx
    pop ecx
    pop eax
    ret

countdown_loop:
    ; Выводим текущую цифру секунды
    mov ah, 0x0E
    mov al, cl
    int 0x10

    ; Выводим символ возврата каретки '\r', чтобы следующая цифра затирала старую
    mov al, 0x0D
    int 0x10

    ; Для создания задершки используется прерывание 0x15 и функция AH = 0x86 (sleep)
    ; Аргументы функции пара регистров CX:DX
    ; 1 000 000 -> 0xF4240
    push cx             ; Сохраняем счетчик секунд в стек
    mov ah, 0x86
    mov cx, 0x000F      ; Старшее слово от 1 000 000
    mov dx, 0x4240      ; Младшее слово от 1 000 000
    int 0x15
    pop cx              ; Восстанавливаем счетчик секунд из стека

    ; Уменьшаем символ в CL (например, с '5' до '4')
    dec cl
    cmp cl, '0'
    jne countdown_loop

    ; 4. Выводим финальный текст перед перезагрузкой
    mov si, msg_reboot
    call string_to_out

    ; Короткая пауза в полсекунды, чтобы пользователь успел увидеть надпись "Rebooting..."
    ; 500 000 -> 0x7A120
    mov ah, 0x86
    mov cx, 0x0007
    mov dx, 0xA120
    int 0x15

    int 0x19

student_data db "Savely Burkovskiy", 0x0D, 0x0A, "8V42", 0x0D, 0x0A, 0
format_str db "! = ", 0
msg_before_reboot db 0x0D, 0x0A, "Seconds before restarting...", 0x0D, 0x0A, 0  ; 0x0D, 0x0A — это перевод строки (\r\n)
msg_reboot db "Rebooting...", 0x0D, 0x0A, 0

; Заполнение оставшегося пространства до 510 байт нулями
times 510 - ($ - $$) db 0
; Магическая сигнатура загрузочного сектора
dw 0xAA55
