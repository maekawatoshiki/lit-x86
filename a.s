section .text
	global _start

_start:
	mov [ecx + 256], eax
	mov eax, [eax + 4]
	mov ebx, [eax+4]
	push ebx
	mov [ebp - 4], eax
	add eax, 1
	lea eax, [ebp - 4]
	mov [esp + 4], eax
	add eax, 15
	sub eax, 15
	sub eax, 1000
	mov eax, 1000
	mov eax, 16
