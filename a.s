section .text
            global _start
_start:
  mov ecx, 10
  mov eax, 1
  jmp label

label:
  add eax, 1
  loop label
