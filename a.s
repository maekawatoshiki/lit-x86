section .text
            global _start
_start:
        cmp eax, ebx
        mov eax, [ebp-10]

        jmp label
        jmp _start
        jmp a
label:
        add eax, ebx

aas:
  mov eax, ebx
a:
  mov eax, ebx
