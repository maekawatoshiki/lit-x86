section .text
  global _start

_start:
	mov [ebp-0x0c], eax
  push ebx
  push esi
  push edi
  pop ebx
  pop esi
  pop edi
