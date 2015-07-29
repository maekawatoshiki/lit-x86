section .text
            global _start
_start:
  mov ecx, 10
  mov eax, 1
  cmp eax, 0
  jne label
  jmp l2
label:
  mov eax, [esp-4]
  cmp eax, 0
  mov eax, edx
  jmp label
l2:
  mov eax, 2
