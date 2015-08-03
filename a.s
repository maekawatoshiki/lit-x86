section .text
            global _start
_start:
  mov ecx, esp
  mov esp, ecx
  cmp eax, ecx
  mov eax, 10
  push esp
  pop esp
  mov edx, eax
  mov eax, [ebp - %eax]
