.global integer_division

integer_division:
    mov r3, r0          @ Cargar el dividendo en r3
    mov r4, r1          @ Cargar el divisor en r4
    mov r0, #0          @ Inicializar el cociente en r0

division_loop:
    cmp r3, r4          @ Comparar el dividendo y el divisor
    blt division_done   @ Si el dividendo es menor que el divisor, terminar
    sub r3, r3, r4      @ Restar el divisor del dividendo
    add r0, r0, #1      @ Incrementar el cociente
    b division_loop     @ Volver al inicio del bucle

division_done:
    bx lr               @ Regresar
