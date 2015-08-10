section .text
  global _start

_start:
  mov [ebp - 4380 + (ecx * 4)], eax
	mov eax, [ebp - 300 + (edx * 4)]
  mov [ebp - 300 + (ecx * 4)], eax
  mov ecx, eax
  mov edx, eax
  sub esp, 10000
  add esp, 10000
  mov [esp-300], eax
  mov eax, [esp-300]
