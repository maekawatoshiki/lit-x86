section .text
  global _start

_start:
  sub ebp, 1000
	mov [esp+4], eax
