section .text
	global _start

_start:
	mov [ecx + 256], eax
	mov eax, [eax + 4]
	mov ebx, [eax+4]
	push ebx
	mov [ebp - 4], eax
	add eax, 1
	mov ebx, [ecx + 4]
