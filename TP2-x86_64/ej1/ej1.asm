; /** defines bool y puntero **/
%define NULL 0
%define TRUE 1
%define FALSE 0

section .data

section .text

global string_proc_list_create_asm
global string_proc_node_create_asm
global string_proc_list_add_node_asm
global string_proc_list_concat_asm

; FUNCIONES auxiliares que pueden llegar a necesitar:
extern malloc
extern free
extern str_concat


string_proc_list_create_asm:
    ; Crear una nueva lista vacía
    ; Sin parámetros
    ; Devuelve: puntero a la nueva lista (RAX)

    push rbp
    mov rbp, rsp

    ; Llamar a malloc para sizeof(string_proc_list) = 16 bytes
    mov rdi, 16
    call malloc

    ; check_malloc
    test rax, rax
    jnz .malloc_success

    ; Manejar error (devolver NULL)
    mov rax, NULL
    jmp .end

.malloc_success:
    mov qword [rax], NULL     ; list->first = NULL
    mov qword [rax+8], NULL   ; list->last = NULL

.end:
    pop rbp
    ret


string_proc_node_create_asm:
    ; Crear un nuevo nodo
    ; Parámetros:
    ;   RDI = type (uint8_t)
    ;   RSI = hash (char*)
    ; Devuelve: puntero al nuevo nodo (RAX)

    push rbp
    mov rbp, rsp

    ; Guardar parámetros
    push rdi    ; tipo
    push rsi    ; hash

    ; Llamar a malloc para sizeof(string_proc_node) = 32 bytes
    mov rdi, 32
    call malloc

    ; check_malloc
    test rax, rax
    jnz .malloc_success

    ; Manejar error (devolver NULL)
    mov rax, NULL
    jmp .end

.malloc_success:
    ; Restaurar parámetros
    pop rsi     ; hash
    pop rdi     ; tipo

    mov qword [rax], NULL     ; node->next = NULL
    mov qword [rax+8], NULL   ; node->previous = NULL
    mov byte [rax+16], dil    ; node->type = tipo
    mov qword [rax+24], rsi   ; node->hash = hash

.end:
    pop rbp
    ret


string_proc_list_add_node_asm:
    ; Agregar un nodo a la lista
    ; Parámetros:
    ;   RDI = list (string_proc_list*)
    ;   RSI = type (uint8_t)
    ;   RDX = hash (char*)
    ; Devuelve: void

    push rbp
    mov rbp, rsp

    ; if (list == NULL)
    test rdi, rdi
    jz .end    ; return;

    ; Guardar parámetros
    push rdi    ; lista
    push rsi    ; tipo
    push rdx    ; hash


    ; string_proc_node* node = string_proc_node_create(type, hash);
    mov rdi, rsi    ; type
    mov rsi, rdx    ; hash
    call string_proc_node_create_asm

    ; check_malloc
    test rax, rax
    jz .end

    ; RAX ahora contiene el nuevo nodo
    mov rdx, rax    ; rdx = nuevo_nodo

    ; Restaurar el puntero de la lista
    pop rax         ; hash (descartar)
    pop rax         ; tipo (descartar)
    pop rdi         ; lista

    ; if(list->first == NULL)
    mov r8, [rdi]   ; r8 = list->first
    test r8, r8
    jnz .list_not_empty

    ; La lista está vacía
    mov [rdi], rdx      ; list->first = node
    mov [rdi+8], rdx    ; list->last = node
    jmp .end

.list_not_empty:
    mov r9, [rdi+8]     ; r9 = list->last

    mov [rdx+8], r9     ; node->previous = list->last

    mov [r9], rdx       ; list->last->next = node

    mov [rdi+8], rdx    ; list->last = node

.end:
    pop rbp
    ret


string_proc_list_concat_asm:
    ; Genera un nuevo hash concatenando el pasado por parámetro con todos los hashes
    ; de los nodos de la lista cuyos tipos coinciden con el pasado por parámetro
    ; Parámetros:
    ;   RDI = list (string_proc_list*)
    ;   RSI = type (uint8_t)
    ;   RDX = hash (char*)
    ; Devuelve: char* (nueva cadena concatenada)

    push rbp
    mov rbp, rsp
    sub rsp, 48         ; Reservar espacio en la pila para variables locales

    ; if (list == NULL || hash == NULL) { return NULL; }
    test rdi, rdi
    jz .fail
    test rdx, rdx
    jz .fail

    ; Guardar parámetros
    mov [rbp-8], rdi    ; lista
    mov [rbp-16], rsi   ; tipo
    mov [rbp-24], rdx   ; hash

    ; (char*)malloc(1)
    mov rdi, 1
    call malloc

    ; check_malloc
    test rax, rax
    jnz .malloc_success

    ; Manejar error (devolver NULL)
    jmp .fail

.malloc_success:
    mov byte [rax], 0   ; new_hash[0] = '\0'
    mov [rbp-32], rax   ; new_hash

    mov rdi, [rbp-8]    ; lista
    mov rdi, [rdi]      ; current_node = list->first
    mov [rbp-40], rdi   ; current_node

.loop:
    mov rdi, [rbp-40]   ; current_node
    test rdi, rdi
    jz .after_loop      ; while(current_node != NULL)

    ; if(current_node->type == type)
    movzx rax, byte [rdi+16]  ; rax = current_node->type
    cmp rax, [rbp-16]   ; if(current_node->type == type)
    jne .next_node

    mov rdi, [rbp-32]   ; new_hash
    mov rsi, [rbp-40]   ; current_node
    mov rsi, [rsi+24]   ; current_node->hash

    ; if (current_node->hash == NULL) ; Si un hash es NULL, retorno NULL
    test rsi, rsi
    jz .free_newhash

    ; char* temp = str_concat(new_hash, current_node->hash);
    call str_concat

    ; free(new_hash)
    .free_newhash:
        mov rdi, [rbp-32]
        push rax            ; Guardar resultado de str_concat
        call free
        pop rax             ; Restaurar resultado

    mov rsi, [rbp-40]   ; current_node
    mov rsi, [rsi+24]   ; current_node->hash
    test rsi, rsi
    jz .fail            ; Si el hash dio NULL, manejar error

    ; new_hash = temp;
    mov [rbp-32], rax

.next_node:
    mov rdi, [rbp-40]   ; current_node
    mov rdi, [rdi]      ; current_node->next
    mov [rbp-40], rdi   ; current_node = current_node->next
    jmp .loop

.after_loop:
    ; char* result = str_concat(hash, new_hash);
    mov rdi, [rbp-24]   ; hash
    mov rsi, [rbp-32]   ; new_hash
    call str_concat
    mov [rbp-48], rax   ; result

    ; free(new_hash);
    mov rdi, [rbp-32]
    call free

    ; return result;
    mov rax, [rbp-48]
    jmp .end

.fail:
    ; Manejar error (devolver NULL)
    MOV rax, NULL

.end:
    add rsp, 48
    pop rbp
    ret
