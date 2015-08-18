section .text
  global _start

_start:

mov eax, [ebp +8]
mov [esp-4], eax
