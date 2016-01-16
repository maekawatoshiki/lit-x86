section .text
  global _start

_start:
	mov ecx, [ebp-8]
	mov eax, [ebp+4]
	mov eax, [edx + ecx]
	mov [ecx + edx*4], eax
	mov eax, [edx + ecx * 4]
	mov [ebp -4], eax
	imul eax
	idiv eax
	shl eax, 2
	shr eax, 2
  mov [ecx+edx], eax
  mov [esp], eax
  mov [esp+ 4], eax
  movzx eax, byte [edx + ecx]
  dec eax
  inc eax
	mov eax, [eax * 4+ ecx]
	sete cl
	setl cl
	setg cl
	setne cl
	and cl, 1
	movzx eax, cl
	setle cl
	setge cl
	je 5
	cmp eax, 0
	and eax, ebx
	or eax, ebx
	movsd xmm0, xmm1
	mov eax, 1000
	mov [esp-0], ebx
	mov [esp-4], eax
