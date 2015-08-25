	.arch armv4t
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 18, 4
	.file	"lit.c"
	.local	jitCode
	.comm	jitCode,4,4
	.global	jitCount
	.bss
	.align	2
	.type	jitCount, %object
	.size	jitCount, 4
jitCount:
	.space	4
	.global	token
	.align	2
	.type	token, %object
	.size	token, 147420
token:
	.space	147420
	.global	tkpos
	.align	2
	.type	tkpos, %object
	.size	tkpos, 4
tkpos:
	.space	4
	.global	tksize
	.align	2
	.type	tksize, %object
	.size	tksize, 4
tksize:
	.space	4
	.global	varNames
	.align	2
	.type	varNames, %object
	.size	varNames, 1165860
varNames:
	.space	1165860
	.global	varSize
	.align	2
	.type	varSize, %object
	.size	varSize, 1020
varSize:
	.space	1020
	.global	varCounter
	.align	2
	.type	varCounter, %object
	.size	varCounter, 4
varCounter:
	.space	4
	.global	nowFunc
	.align	2
	.type	nowFunc, %object
	.size	nowFunc, 4
nowFunc:
	.space	4
	.global	breaks
	.align	2
	.type	breaks, %object
	.size	breaks, 1020
breaks:
	.space	1020
	.global	brkCount
	.align	2
	.type	brkCount, %object
	.size	brkCount, 4
brkCount:
	.space	4
	.global	blocksCount
	.align	2
	.type	blocksCount, %object
	.size	blocksCount, 4
blocksCount:
	.space	4
	.global	functions
	.align	2
	.type	functions, %object
	.size	functions, 66300
functions:
	.space	66300
	.global	funcCount
	.align	2
	.type	funcCount, %object
	.size	funcCount, 4
funcCount:
	.space	4
	.global	strings
	.align	2
	.type	strings, %object
	.size	strings, 65025
strings:
	.space	65025
	.comm	stringsPos,4,4
	.global	stringsCount
	.align	2
	.type	stringsCount, %object
	.size	stringsCount, 4
stringsCount:
	.space	4
	.text
	.align	2
	.type	init, %function
init:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	ldr	r3, .L2
	mov	r2, #0
	str	r2, [r3, #0]
	ldr	r3, .L2
	ldr	r2, [r3, #0]
	ldr	r3, .L2+4
	str	r2, [r3, #0]
	ldr	r3, .L2+8
	ldr	r3, [r3, #0]
	mov	r2, r3
	ldr	r3, .L2+12
	mov	r0, r2
	mov	r1, #0
	mov	r2, r3
	bl	memset
	ldr	r3, .L2+16
	mov	r2, r3
	ldr	r3, .L2+20
	mov	r0, r2
	mov	r1, #0
	mov	r2, r3
	bl	memset
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L3:
	.align	2
.L2:
	.word	jitCount
	.word	tkpos
	.word	jitCode
	.word	65535
	.word	token
	.word	147420
	.size	init, .-init
	.section	.rodata
	.align	2
.LC0:
	.ascii	"\"\000"
	.align	2
.LC1:
	.ascii	"error: %d: expected expression '\"'\000"
	.align	2
.LC2:
	.ascii	";\000"
	.align	2
.LC3:
	.ascii	"tk> %s\012\000"
	.text
	.align	2
	.type	lex, %function
lex:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #24
	str	r0, [fp, #-24]
	ldr	r0, [fp, #-24]
	bl	strlen
	mov	r3, r0
	str	r3, [fp, #-16]
	mov	r3, #1
	str	r3, [fp, #-12]
	mov	r3, #0
	str	r3, [fp, #-8]
	b	.L5
.L24:
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-8]
	ldr	r1, [fp, #-24]
	add	r3, r1, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r2, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #2048
	cmp	r3, #0
	beq	.L6
	b	.L7
.L8:
	ldr	r3, .L27
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L27+4
	add	r2, r3, r2
	ldr	r1, [fp, #-24]
	ldr	r3, [fp, #-8]
	add	r3, r1, r3
	mov	r0, r2
	mov	r1, r3
	mov	r2, #1
	bl	strncat
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L7:
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-8]
	ldr	r1, [fp, #-24]
	add	r3, r1, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r2, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #2048
	cmp	r3, #0
	bne	.L8
	ldr	r3, .L27
	ldr	r2, [r3, #0]
	ldr	r0, .L27+4
	mov	r1, #32
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	add	r3, r3, r1
	ldr	r2, [fp, #-12]
	str	r2, [r3, #0]
	ldr	r3, [fp, #-8]
	sub	r3, r3, #1
	str	r3, [fp, #-8]
	ldr	r3, .L27
	ldr	r3, [r3, #0]
	add	r2, r3, #1
	ldr	r3, .L27
	str	r2, [r3, #0]
	b	.L9
.L6:
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-8]
	ldr	r1, [fp, #-24]
	add	r3, r1, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r2, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #1024
	cmp	r3, #0
	beq	.L10
	b	.L11
.L12:
	ldr	r3, .L27
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L27+4
	add	r2, r3, r2
	ldr	r1, [fp, #-24]
	ldr	r3, [fp, #-8]
	add	r3, r1, r3
	mov	r0, r2
	mov	r1, r3
	mov	r2, #1
	bl	strncat
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L11:
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-8]
	ldr	r1, [fp, #-24]
	add	r3, r1, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r2, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #1024
	cmp	r3, #0
	bne	.L12
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-8]
	ldr	r1, [fp, #-24]
	add	r3, r1, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r2, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #2048
	cmp	r3, #0
	bne	.L12
	ldr	r3, [fp, #-8]
	ldr	r2, [fp, #-24]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #95
	beq	.L12
	ldr	r3, .L27
	ldr	r2, [r3, #0]
	ldr	r0, .L27+4
	mov	r1, #32
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	add	r3, r3, r1
	ldr	r2, [fp, #-12]
	str	r2, [r3, #0]
	ldr	r3, [fp, #-8]
	sub	r3, r3, #1
	str	r3, [fp, #-8]
	ldr	r3, .L27
	ldr	r3, [r3, #0]
	add	r2, r3, #1
	ldr	r3, .L27
	str	r2, [r3, #0]
	b	.L9
.L10:
	ldr	r3, [fp, #-8]
	ldr	r2, [fp, #-24]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #32
	beq	.L9
	ldr	r3, [fp, #-8]
	ldr	r2, [fp, #-24]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #9
	beq	.L9
	ldr	r3, [fp, #-8]
	ldr	r2, [fp, #-24]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #35
	bne	.L13
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
	b	.L14
.L15:
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L14:
	ldr	r3, [fp, #-8]
	ldr	r2, [fp, #-24]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #10
	bne	.L15
	ldr	r3, [fp, #-12]
	add	r3, r3, #1
	str	r3, [fp, #-12]
	b	.L9
.L13:
	ldr	r3, [fp, #-8]
	ldr	r2, [fp, #-24]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #34
	bne	.L16
	ldr	r3, .L27
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L27+4
	add	r2, r3, r2
	ldr	r3, .L27+8
	mov	r1, r2
	mov	r2, r3
	mov	r3, #2
	mov	r0, r1
	mov	r1, r2
	mov	r2, r3
	bl	memcpy
	ldr	r3, .L27
	ldr	r2, [r3, #0]
	ldr	r0, .L27+4
	mov	r1, #32
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	add	r3, r3, r1
	ldr	r1, [fp, #-12]
	str	r1, [r3, #0]
	add	r2, r2, #1
	ldr	r3, .L27
	str	r2, [r3, #0]
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
	b	.L17
.L19:
	ldr	r3, .L27
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L27+4
	add	r2, r3, r2
	ldr	r1, [fp, #-24]
	ldr	r3, [fp, #-8]
	add	r3, r1, r3
	mov	r0, r2
	mov	r1, r3
	mov	r2, #1
	bl	strncat
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L17:
	ldr	r3, [fp, #-8]
	ldr	r2, [fp, #-24]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #34
	beq	.L18
	ldr	r3, [fp, #-8]
	ldr	r2, [fp, #-24]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	bne	.L19
.L18:
	ldr	r3, .L27
	ldr	r2, [r3, #0]
	ldr	r0, .L27+4
	mov	r1, #32
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	add	r3, r3, r1
	ldr	r2, [fp, #-12]
	str	r2, [r3, #0]
	ldr	r3, [fp, #-8]
	ldr	r2, [fp, #-24]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	bne	.L20
	ldr	r3, .L27
	ldr	r2, [r3, #0]
	ldr	r0, .L27+4
	mov	r1, #32
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	add	r3, r3, r1
	ldr	r3, [r3, #0]
	ldr	r0, .L27+12
	mov	r1, r3
	bl	error
.L20:
	ldr	r3, .L27
	ldr	r3, [r3, #0]
	add	r2, r3, #1
	ldr	r3, .L27
	str	r2, [r3, #0]
	b	.L9
.L16:
	ldr	r3, [fp, #-8]
	ldr	r2, [fp, #-24]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #10
	bne	.L21
	ldr	r3, .L27
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L27+4
	add	r2, r3, r2
	ldr	r3, .L27+16
	mov	r1, r2
	mov	r2, r3
	mov	r3, #2
	mov	r0, r1
	mov	r1, r2
	mov	r2, r3
	bl	memcpy
	ldr	r3, [fp, #-12]
	add	r3, r3, #1
	str	r3, [fp, #-12]
	b	.L22
.L21:
	ldr	r3, .L27
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L27+4
	add	r2, r3, r2
	ldr	r1, [fp, #-24]
	ldr	r3, [fp, #-8]
	add	r3, r1, r3
	mov	r0, r2
	mov	r1, r3
	mov	r2, #1
	bl	strncat
.L22:
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	ldr	r2, [fp, #-24]
	add	r3, r2, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #61
	bne	.L23
	ldr	r3, .L27
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L27+4
	add	r2, r3, r2
	ldr	r1, [fp, #-24]
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
	ldr	r3, [fp, #-8]
	add	r3, r1, r3
	mov	r0, r2
	mov	r1, r3
	mov	r2, #1
	bl	strncat
.L23:
	ldr	r3, .L27
	ldr	r2, [r3, #0]
	ldr	r0, .L27+4
	mov	r1, #32
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	add	r3, r3, r1
	ldr	r2, [fp, #-12]
	str	r2, [r3, #0]
	ldr	r3, .L27
	ldr	r3, [r3, #0]
	add	r2, r3, #1
	ldr	r3, .L27
	str	r2, [r3, #0]
.L9:
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L5:
	ldr	r2, [fp, #-8]
	ldr	r3, [fp, #-16]
	cmp	r2, r3
	blt	.L24
	ldr	r3, .L27
	ldr	r2, [r3, #0]
	ldr	r0, .L27+4
	mov	r1, #32
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	add	r3, r3, r1
	ldr	r2, [fp, #-12]
	str	r2, [r3, #0]
	mov	r3, #0
	str	r3, [fp, #-8]
	b	.L25
.L26:
	ldr	r1, .L27+20
	ldr	r2, [fp, #-8]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L27+4
	add	r3, r3, r2
	mov	r0, r1
	mov	r1, r3
	bl	printf
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L25:
	ldr	r3, .L27
	ldr	r3, [r3, #0]
	ldr	r2, [fp, #-8]
	cmp	r2, r3
	blt	.L26
	ldr	r3, .L27
	ldr	r2, [r3, #0]
	ldr	r3, .L27+24
	str	r2, [r3, #0]
	mov	r3, #0
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L28:
	.align	2
.L27:
	.word	tkpos
	.word	token
	.word	.LC0
	.word	.LC1
	.word	.LC2
	.word	.LC3
	.word	tksize
	.size	lex, .-lex
	.section	.rodata
	.align	2
.LC4:
	.ascii	"!\000"
	.align	2
.LC5:
	.ascii	"[\000"
	.align	2
.LC6:
	.ascii	"error: %d: invalid expression\000"
	.align	2
.LC7:
	.ascii	"]\000"
	.align	2
.LC8:
	.ascii	"puts\000"
	.align	2
.LC9:
	.ascii	"output\000"
	.align	2
.LC10:
	.ascii	"push eax\000"
	.align	2
.LC11:
	.ascii	"pop eax\000"
	.align	2
.LC12:
	.ascii	",\000"
	.align	2
.LC13:
	.ascii	"def\000"
	.align	2
.LC14:
	.ascii	"(\000"
	.align	2
.LC15:
	.ascii	")\000"
	.align	2
.LC16:
	.ascii	"main\000"
	.align	2
.LC17:
	.ascii	"push ebp\000"
	.align	2
.LC18:
	.ascii	"mov ebp esp\000"
	.align	2
.LC19:
	.ascii	"%s() has %d byte\012\000"
	.align	2
.LC20:
	.ascii	"return\000"
	.align	2
.LC21:
	.ascii	"for\000"
	.align	2
.LC22:
	.ascii	"while\000"
	.align	2
.LC23:
	.ascii	"if\000"
	.align	2
.LC24:
	.ascii	"else\000"
	.align	2
.LC25:
	.ascii	"elsif\000"
	.align	2
.LC26:
	.ascii	"break\000"
	.align	2
.LC27:
	.ascii	"end\000"
	.align	2
.LC28:
	.ascii	"error: expected 'end' before %d line\000"
	.text
	.align	2
	.type	eval, %function
eval:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 568
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #568
	str	r0, [fp, #-568]
	str	r1, [fp, #-572]
	mov	r3, #0
	str	r3, [fp, #-20]
	b	.L30
.L61:
	bl	isassign
	mov	r3, r0
	cmp	r3, #0
	beq	.L31
	bl	assignment
	b	.L30
.L31:
	ldr	r0, .L63
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L32
	ldr	r3, .L63+4
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r1, .L63+8
	add	r3, r3, r1
	str	r3, [fp, #-24]
	add	r2, r2, #1
	ldr	r3, .L63+4
	str	r2, [r3, #0]
	ldr	r0, .L63+12
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	bne	.L33
	ldr	r3, .L63+4
	ldr	r2, [r3, #0]
	ldr	r0, .L63+8
	mov	r1, #32
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	add	r3, r3, r1
	ldr	r3, [r3, #0]
	ldr	r0, .L63+16
	mov	r1, r3
	bl	error
.L33:
	ldr	r3, .L63+4
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r1, .L63+8
	add	r3, r3, r1
	add	r1, r2, #1
	ldr	r2, .L63+4
	str	r1, [r2, #0]
	mov	r0, r3
	bl	atoi
	str	r0, [fp, #-28]
	ldr	r0, .L63+20
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	bne	.L34
	ldr	r3, .L63+4
	ldr	r2, [r3, #0]
	ldr	r0, .L63+8
	mov	r1, #32
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	add	r3, r3, r1
	ldr	r3, [r3, #0]
	ldr	r0, .L63+16
	mov	r1, r3
	bl	error
.L34:
	ldr	r0, [fp, #-24]
	ldr	r1, [fp, #-28]
	bl	getNumOfVar
	b	.L30
.L32:
	ldr	r0, .L63+24
	bl	skip
	str	r0, [fp, #-20]
	ldr	r3, [fp, #-20]
	cmp	r3, #0
	bne	.L35
	ldr	r0, .L63+28
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L36
.L35:
	mov	r3, #0
	str	r3, [fp, #-8]
	ldr	r0, .L63+32
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L37
	mov	r0, #184
	bl	genCode
	bl	getString
	mov	r0, #0
	bl	genCodeInt32
	mov	r3, #1
	str	r3, [fp, #-8]
	b	.L38
.L37:
	bl	relExpr
.L38:
	ldr	r0, .L63+36
	bl	genas
	ldr	r3, [fp, #-8]
	cmp	r3, #0
	beq	.L39
	mov	r0, #255
	bl	genCode
	mov	r0, #86
	bl	genCode
	mov	r0, #4
	bl	genCode
	b	.L40
.L39:
	mov	r0, #255
	bl	genCode
	mov	r0, #22
	bl	genCode
.L40:
	ldr	r0, .L63+40
	bl	genas
	ldr	r0, .L63+44
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	bne	.L35
	ldr	r3, [fp, #-20]
	cmp	r3, #0
	beq	.L30
	mov	r0, #255
	bl	genCode
	mov	r0, #86
	bl	genCode
	mov	r0, #8
	bl	genCode
	b	.L30
.L36:
	ldr	r0, .L63+48
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L42
	mov	r3, #0
	str	r3, [fp, #-12]
	ldr	r3, .L63+4
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r1, .L63+8
	add	r3, r3, r1
	str	r3, [fp, #-32]
	add	r2, r2, #1
	ldr	r3, .L63+4
	str	r2, [r3, #0]
	ldr	r3, .L63+52
	ldr	r3, [r3, #0]
	add	r2, r3, #1
	ldr	r3, .L63+52
	str	r2, [r3, #0]
	ldr	r0, .L63+56
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L43
.L44:
	ldr	r3, .L63+4
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r1, .L63+8
	add	r3, r3, r1
	add	r1, r2, #1
	ldr	r2, .L63+4
	str	r1, [r2, #0]
	mov	r0, r3
	mov	r1, #0
	bl	getNumOfVar
	ldr	r3, [fp, #-12]
	add	r3, r3, #1
	str	r3, [fp, #-12]
	ldr	r0, .L63+44
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	bne	.L44
	ldr	r0, .L63+60
	bl	skip
.L43:
	ldr	r3, .L63+64
	ldr	r3, [r3, #0]
	ldr	r0, [fp, #-32]
	mov	r1, r3
	bl	getFunction
	ldr	r0, .L63+68
	ldr	r1, [fp, #-32]
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L45
	ldr	r0, .L63+72
	bl	genas
	ldr	r0, .L63+76
	bl	genas
	mov	r0, #129
	bl	genCode
	mov	r0, #236
	bl	genCode
	ldr	r3, .L63+64
	ldr	r3, [r3, #0]
	str	r3, [fp, #-36]
	mov	r0, #0
	bl	genCodeInt32
	mov	r0, #139
	bl	genCode
	mov	r0, #117
	bl	genCode
	mov	r0, #12
	bl	genCode
	mov	r0, #0
	mov	r1, #0
	bl	eval
	mov	r0, #129
	bl	genCode
	mov	r0, #196
	bl	genCode
	ldr	r3, .L63+52
	ldr	r2, [r3, #0]
	ldr	r3, .L63+80
	ldr	r3, [r3, r2, asl #2]
	add	r3, r3, #6
	mov	r3, r3, asl #2
	mov	r0, r3
	bl	genCodeInt32
	mov	r0, #201
	bl	genCode
	mov	r0, #195
	bl	genCode
	ldr	r3, .L63+52
	ldr	r2, [r3, #0]
	ldr	r3, .L63+80
	ldr	r3, [r3, r2, asl #2]
	add	r3, r3, #6
	mov	r3, r3, asl #2
	mov	r0, r3
	ldr	r1, [fp, #-36]
	bl	genCodeInt32Insert
	b	.L46
.L45:
	ldr	r0, .L63+72
	bl	genas
	ldr	r0, .L63+76
	bl	genas
	mov	r0, #129
	bl	genCode
	mov	r0, #236
	bl	genCode
	ldr	r3, .L63+64
	ldr	r3, [r3, #0]
	str	r3, [fp, #-36]
	mov	r0, #0
	bl	genCodeInt32
	mov	r3, #0
	str	r3, [fp, #-16]
	b	.L47
.L48:
	mov	r0, #139
	bl	genCode
	mov	r0, #69
	bl	genCode
	ldr	r2, [fp, #-12]
	ldr	r3, [fp, #-16]
	rsb	r3, r3, r2
	add	r3, r3, #1
	and	r3, r3, #255
	mov	r3, r3, asl #2
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	mov	r0, #137
	bl	genCode
	mov	r0, #68
	bl	genCode
	mov	r0, #36
	bl	genCode
	ldr	r3, .L63+64
	ldr	r2, [r3, #0]
	ldr	r1, [fp, #-16]
	ldr	r3, .L63+84
	mov	r1, r1, asl #2
	sub	r0, fp, #4
	add	r1, r0, r1
	add	r3, r1, r3
	str	r2, [r3, #0]
	mov	r0, #0
	bl	genCode
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
.L47:
	ldr	r2, [fp, #-16]
	ldr	r3, [fp, #-12]
	cmp	r2, r3
	blt	.L48
	mov	r0, #0
	mov	r1, #0
	bl	eval
	mov	r0, #129
	bl	genCode
	mov	r0, #196
	bl	genCode
	ldr	r3, .L63+52
	ldr	r2, [r3, #0]
	ldr	r3, .L63+80
	ldr	r3, [r3, r2, asl #2]
	add	r3, r3, #6
	mov	r3, r3, asl #2
	mov	r0, r3
	bl	genCodeInt32
	mov	r0, #201
	bl	genCode
	mov	r0, #195
	bl	genCode
	ldr	r3, .L63+52
	ldr	r2, [r3, #0]
	ldr	r3, .L63+80
	ldr	r3, [r3, r2, asl #2]
	add	r3, r3, #6
	mov	r3, r3, asl #2
	mov	r0, r3
	ldr	r1, [fp, #-36]
	bl	genCodeInt32Insert
	mov	r3, #0
	str	r3, [fp, #-16]
	b	.L49
.L50:
	ldr	r3, .L63+88
	ldr	r2, [r3, #0]
	ldr	r1, [fp, #-16]
	ldr	r3, .L63+84
	mov	r1, r1, asl #2
	sub	r0, fp, #4
	add	r1, r0, r1
	add	r3, r1, r3
	ldr	r3, [r3, #0]
	add	r3, r2, r3
	ldr	r2, [fp, #-16]
	mvn	r2, r2
	add	r1, r2, #64
	ldr	r2, .L63+52
	ldr	r0, [r2, #0]
	ldr	r2, .L63+80
	ldr	r2, [r2, r0, asl #2]
	add	r2, r2, #5
	add	r2, r1, r2
	and	r2, r2, #255
	mov	r2, r2, asl #2
	and	r2, r2, #255
	strb	r2, [r3, #0]
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
.L49:
	ldr	r2, [fp, #-16]
	ldr	r3, [fp, #-12]
	cmp	r2, r3
	blt	.L50
.L46:
	ldr	r2, .L63+92
	ldr	r3, .L63+52
	ldr	r1, [r3, #0]
	ldr	r3, .L63+80
	ldr	r3, [r3, r1, asl #2]
	mov	r3, r3, asl #2
	mov	r0, r2
	ldr	r1, [fp, #-32]
	mov	r2, r3
	bl	printf
	b	.L30
.L42:
	ldr	r0, .L63+96
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L51
	bl	relExpr
	mov	r3, #1
	b	.L52
.L51:
	ldr	r0, .L63+100
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L53
	ldr	r3, .L63+104
	ldr	r3, [r3, #0]
	add	r2, r3, #1
	ldr	r3, .L63+104
	str	r2, [r3, #0]
	bl	assignment
	ldr	r0, .L63+44
	bl	skip
	bl	whileStmt
	b	.L30
.L53:
	ldr	r0, .L63+108
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L54
	ldr	r3, .L63+104
	ldr	r3, [r3, #0]
	add	r2, r3, #1
	ldr	r3, .L63+104
	str	r2, [r3, #0]
	bl	whileStmt
	b	.L30
.L54:
	ldr	r0, .L63+112
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L55
	ldr	r3, .L63+104
	ldr	r3, [r3, #0]
	add	r2, r3, #1
	ldr	r3, .L63+104
	str	r2, [r3, #0]
	bl	ifStmt
	b	.L30
.L55:
	ldr	r0, .L63+116
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L56
	mov	r0, #233
	bl	genCode
	ldr	r3, .L63+64
	ldr	r3, [r3, #0]
	str	r3, [fp, #-40]
	mov	r0, #0
	bl	genCodeInt32
	ldr	r3, .L63+64
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-568]
	rsb	r3, r3, r2
	sub	r3, r3, #4
	mov	r0, r3
	ldr	r1, [fp, #-568]
	bl	genCodeInt32Insert
	ldr	r0, [fp, #-40]
	mov	r1, #0
	bl	eval
	mov	r3, #1
	b	.L52
.L56:
	ldr	r0, .L63+120
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L57
	mov	r0, #233
	bl	genCode
	ldr	r3, .L63+64
	ldr	r3, [r3, #0]
	str	r3, [fp, #-44]
	mov	r0, #0
	bl	genCodeInt32
	ldr	r3, .L63+64
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-568]
	rsb	r3, r3, r2
	sub	r3, r3, #4
	mov	r0, r3
	ldr	r1, [fp, #-568]
	bl	genCodeInt32Insert
	bl	relExpr
	str	r0, [fp, #-48]
	ldr	r3, [fp, #-48]
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	mov	r0, #5
	bl	genCode
	mov	r0, #233
	bl	genCode
	ldr	r3, .L63+64
	ldr	r3, [r3, #0]
	str	r3, [fp, #-52]
	mov	r0, #0
	bl	genCodeInt32
	ldr	r0, [fp, #-52]
	mov	r1, #0
	bl	eval
	ldr	r3, .L63+64
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-44]
	rsb	r3, r3, r2
	sub	r3, r3, #4
	mov	r0, r3
	ldr	r1, [fp, #-44]
	bl	genCodeInt32Insert
	mov	r3, #1
	b	.L52
.L57:
	ldr	r0, .L63+124
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L58
	mov	r0, #233
	bl	genCode
	ldr	r3, .L63+128
	ldr	r2, [r3, #0]
	ldr	r3, .L63+64
	ldr	r1, [r3, #0]
	ldr	r3, .L63+132
	str	r1, [r3, r2, asl #2]
	mov	r0, #0
	bl	genCodeInt32
	ldr	r3, .L63+128
	ldr	r3, [r3, #0]
	add	r2, r3, #1
	ldr	r3, .L63+128
	str	r2, [r3, #0]
	b	.L30
.L58:
	ldr	r0, .L63+136
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L59
	ldr	r3, .L63+104
	ldr	r3, [r3, #0]
	sub	r2, r3, #1
	ldr	r3, .L63+104
	str	r2, [r3, #0]
	ldr	r3, [fp, #-572]
	cmp	r3, #0
	bne	.L60
	ldr	r3, .L63+64
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-568]
	rsb	r3, r3, r2
	sub	r3, r3, #4
	mov	r0, r3
	ldr	r1, [fp, #-568]
	bl	genCodeInt32Insert
.L60:
	mov	r3, #1
	b	.L52
.L59:
	ldr	r0, .L63+140
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	bne	.L30
	bl	relExpr
.L30:
	ldr	r3, .L63+4
	ldr	r2, [r3, #0]
	ldr	r3, .L63+144
	ldr	r3, [r3, #0]
	cmp	r2, r3
	blt	.L61
	ldr	r3, .L63+104
	ldr	r3, [r3, #0]
	cmp	r3, #0
	beq	.L62
	ldr	r3, .L63+4
	ldr	r2, [r3, #0]
	ldr	r0, .L63+8
	mov	r1, #32
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	add	r3, r3, r1
	ldr	r3, [r3, #0]
	ldr	r0, .L63+148
	mov	r1, r3
	bl	error
.L62:
	mov	r3, #0
.L52:
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L64:
	.align	2
.L63:
	.word	.LC4
	.word	tkpos
	.word	token
	.word	.LC5
	.word	.LC6
	.word	.LC7
	.word	.LC8
	.word	.LC9
	.word	.LC0
	.word	.LC10
	.word	.LC11
	.word	.LC12
	.word	.LC13
	.word	nowFunc
	.word	.LC14
	.word	.LC15
	.word	jitCount
	.word	.LC16
	.word	.LC17
	.word	.LC18
	.word	varSize
	.word	-560
	.word	jitCode
	.word	.LC19
	.word	.LC20
	.word	.LC21
	.word	blocksCount
	.word	.LC22
	.word	.LC23
	.word	.LC24
	.word	.LC25
	.word	.LC26
	.word	brkCount
	.word	breaks
	.word	.LC27
	.word	.LC2
	.word	tksize
	.word	.LC28
	.size	eval, .-eval
	.section	.rodata
	.align	2
.LC29:
	.ascii	"main() addr> %u\012\000"
	.align	2
.LC30:
	.ascii	"%d\012\000"
	.align	2
.LC31:
	.ascii	"memsz: %d\012\000"
	.text
	.align	2
	.type	parser, %function
parser:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #16
	ldr	r3, .L70
	mov	r2, #0
	str	r2, [r3, #0]
	ldr	r3, .L70
	ldr	r2, [r3, #0]
	ldr	r3, .L70+4
	str	r2, [r3, #0]
	ldr	r3, .L70+8
	ldr	r3, [r3, #0]
	mov	r2, r3
	ldr	r3, .L70+12
	mov	r0, r2
	mov	r1, #0
	mov	r2, r3
	bl	memset
	mov	r0, #255
	mov	r1, #4
	bl	calloc
	mov	r3, r0
	mov	r2, r3
	ldr	r3, .L70+16
	str	r2, [r3, #0]
	mov	r0, #233
	bl	genCode
	ldr	r3, .L70
	ldr	r3, [r3, #0]
	str	r3, [fp, #-12]
	mov	r0, #0
	bl	genCodeInt32
	mov	r0, #0
	mov	r1, #0
	bl	eval
	ldr	r0, .L70+20
	mov	r1, #0
	bl	getFunction
	str	r0, [fp, #-16]
	ldr	r3, .L70+24
	mov	r0, r3
	ldr	r1, [fp, #-16]
	bl	printf
	ldr	r3, [fp, #-16]
	sub	r3, r3, #5
	mov	r0, r3
	ldr	r1, [fp, #-12]
	bl	genCodeInt32Insert
	ldr	r3, .L70+16
	ldr	r3, [r3, #0]
	sub	r2, r3, #4
	ldr	r3, .L70+16
	str	r2, [r3, #0]
	b	.L66
.L69:
	ldr	r3, .L70
	ldr	r3, [r3, #0]
	mov	r2, r3
	ldr	r3, .L70+16
	ldr	r3, [r3, #0]
	ldr	r3, [r3, #0]
	mov	r0, r2
	mov	r1, r3
	bl	genCodeInt32Insert
	ldr	r3, .L70+28
	ldr	r3, [r3, #0]
	sub	r2, r3, #1
	ldr	r3, .L70+28
	str	r2, [r3, #0]
	ldr	r3, .L70+28
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #8
	rsb	r3, r2, r3
	ldr	r2, .L70+32
	add	r3, r3, r2
	mov	r0, r3
	bl	replaceEscape
	mov	r3, #0
	str	r3, [fp, #-8]
	b	.L67
.L68:
	ldr	r3, .L70+28
	ldr	r2, [r3, #0]
	ldr	r1, .L70+32
	mov	r3, r2
	mov	r3, r3, asl #8
	rsb	r3, r2, r3
	ldr	r2, [fp, #-8]
	add	r3, r3, r2
	add	r3, r1, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r0, r3
	bl	genCode
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L67:
	ldr	r3, .L70+28
	ldr	r2, [r3, #0]
	ldr	r1, .L70+32
	mov	r3, r2
	mov	r3, r3, asl #8
	rsb	r3, r2, r3
	ldr	r2, [fp, #-8]
	add	r3, r3, r2
	add	r3, r1, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	bne	.L68
	mov	r0, #0
	bl	genCode
	ldr	r2, .L70+36
	ldr	r3, .L70+16
	ldr	r3, [r3, #0]
	mov	r0, r2
	mov	r1, r3
	bl	printf
	ldr	r3, .L70+16
	ldr	r3, [r3, #0]
	sub	r2, r3, #4
	ldr	r3, .L70+16
	str	r2, [r3, #0]
.L66:
	ldr	r3, .L70+28
	ldr	r3, [r3, #0]
	cmp	r3, #0
	bne	.L69
	ldr	r2, .L70+40
	ldr	r3, .L70+44
	ldr	r1, [r3, #0]
	ldr	r3, .L70+48
	ldr	r3, [r3, r1, asl #2]
	mov	r0, r2
	mov	r1, r3
	bl	printf
	mov	r3, #0
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L71:
	.align	2
.L70:
	.word	jitCount
	.word	tkpos
	.word	jitCode
	.word	4095
	.word	stringsPos
	.word	.LC16
	.word	.LC29
	.word	stringsCount
	.word	strings
	.word	.LC30
	.word	.LC31
	.word	nowFunc
	.word	varSize
	.size	parser, .-parser
	.align	2
	.type	getString, %function
getString:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	ldr	r3, .L73
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #8
	rsb	r3, r2, r3
	ldr	r2, .L73+4
	add	r1, r3, r2
	ldr	r3, .L73+8
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r0, .L73+12
	add	r3, r3, r0
	add	r0, r2, #1
	ldr	r2, .L73+8
	str	r0, [r2, #0]
	mov	r0, r1
	mov	r1, r3
	bl	strcpy
	ldr	r3, .L73+16
	ldr	r3, [r3, #0]
	ldr	r2, .L73+20
	ldr	r2, [r2, #0]
	str	r2, [r3, #0]
	add	r2, r3, #4
	ldr	r3, .L73+16
	str	r2, [r3, #0]
	ldr	r3, .L73
	ldr	r3, [r3, #0]
	mov	r2, r3
	add	r1, r3, #1
	ldr	r3, .L73
	str	r1, [r3, #0]
	mov	r3, r2
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L74:
	.align	2
.L73:
	.word	stringsCount
	.word	strings
	.word	tkpos
	.word	token
	.word	stringsPos
	.word	jitCount
	.size	getString, .-getString
	.align	2
	.type	getNumOfVar, %function
getNumOfVar:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {r4, fp, lr}
	add	fp, sp, #8
	sub	sp, sp, #20
	str	r0, [fp, #-24]
	str	r1, [fp, #-28]
	mov	r3, #0
	str	r3, [fp, #-16]
	b	.L76
.L79:
	ldr	r3, .L80
	ldr	r1, [r3, #0]
	ldr	r2, [fp, #-16]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	mov	r2, r1
	mov	r2, r2, asl #3
	add	r2, r2, r1
	mov	r1, r2, asl #7
	rsb	r1, r2, r1
	mov	r2, r1, asl #2
	mov	r1, r2
	add	r2, r3, r1
	ldr	r3, .L80+4
	add	r3, r2, r3
	ldr	r0, [fp, #-24]
	mov	r1, r3
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L77
	ldr	r3, .L80
	ldr	r1, [r3, #0]
	ldr	ip, .L80+4
	ldr	r2, [fp, #-16]
	mov	r0, #32
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	mov	r2, r1
	mov	r2, r2, asl #3
	add	r2, r2, r1
	mov	r1, r2, asl #7
	rsb	r1, r2, r1
	mov	r2, r1, asl #2
	mov	r1, r2
	add	r3, r3, r1
	add	r3, ip, r3
	add	r3, r3, r0
	ldr	r3, [r3, #0]
	b	.L78
.L77:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
.L76:
	ldr	r3, .L80+8
	ldr	r3, [r3, #0]
	ldr	r2, [fp, #-16]
	cmp	r2, r3
	blt	.L79
	ldr	r3, .L80
	ldr	r1, [r3, #0]
	ldr	r3, .L80+8
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	mov	r2, r1
	mov	r2, r2, asl #3
	add	r2, r2, r1
	mov	r1, r2, asl #7
	rsb	r1, r2, r1
	mov	r2, r1, asl #2
	mov	r1, r2
	add	r2, r3, r1
	ldr	r3, .L80+4
	add	r2, r2, r3
	ldr	r3, [fp, #-24]
	mov	r0, r2
	mov	r1, r3
	bl	strcpy
	ldr	r3, .L80
	ldr	r3, [r3, #0]
	ldr	r2, .L80
	ldr	r1, [r2, #0]
	ldr	r2, .L80+12
	ldr	r1, [r2, r1, asl #2]
	ldr	r2, [fp, #-28]
	add	r2, r2, #1
	add	r1, r1, r2
	ldr	r2, .L80+12
	str	r1, [r2, r3, asl #2]
	ldr	r2, .L80+12
	ldr	r3, [r2, r3, asl #2]
	add	r3, r3, #1
	str	r3, [fp, #-20]
	ldr	r3, .L80
	ldr	r1, [r3, #0]
	ldr	r3, .L80+8
	ldr	r0, [r3, #0]
	ldr	r4, .L80+4
	mov	ip, #32
	mov	r3, r0
	mov	r3, r3, asl #3
	add	r3, r3, r0
	mov	r3, r3, asl #2
	mov	r2, r1
	mov	r2, r2, asl #3
	add	r2, r2, r1
	mov	r1, r2, asl #7
	rsb	r1, r2, r1
	mov	r2, r1, asl #2
	mov	r1, r2
	add	r3, r3, r1
	add	r3, r4, r3
	add	r3, r3, ip
	ldr	r2, [fp, #-20]
	str	r2, [r3, #0]
	add	r2, r0, #1
	ldr	r3, .L80+8
	str	r2, [r3, #0]
	ldr	r3, [fp, #-20]
.L78:
	mov	r0, r3
	sub	sp, fp, #8
	ldmfd	sp!, {r4, fp, lr}
	bx	lr
.L81:
	.align	2
.L80:
	.word	nowFunc
	.word	varNames
	.word	varCounter
	.word	varSize
	.size	getNumOfVar, .-getNumOfVar
	.align	2
	.type	getFunction, %function
getFunction:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #16
	str	r0, [fp, #-16]
	str	r1, [fp, #-20]
	mov	r3, #0
	str	r3, [fp, #-8]
	b	.L83
.L86:
	ldr	r2, [fp, #-8]
	mov	r3, r2
	mov	r3, r3, asl #6
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L87
	add	r3, r3, r2
	add	r3, r3, #4
	mov	r0, r3
	ldr	r1, [fp, #-16]
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L84
	ldr	r1, .L87
	ldr	r2, [fp, #-8]
	mov	r3, r2
	mov	r3, r3, asl #6
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r1, r3
	ldr	r3, [r3, #0]
	b	.L85
.L84:
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L83:
	ldr	r3, .L87+4
	ldr	r3, [r3, #0]
	ldr	r2, [fp, #-8]
	cmp	r2, r3
	blt	.L86
	ldr	r3, .L87+4
	ldr	r2, [r3, #0]
	ldr	r1, .L87
	mov	r3, r2
	mov	r3, r3, asl #6
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r1, r3
	ldr	r2, [fp, #-20]
	str	r2, [r3, #0]
	ldr	r3, .L87+4
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #6
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L87
	add	r3, r3, r2
	add	r2, r3, #4
	ldr	r3, [fp, #-16]
	mov	r0, r2
	mov	r1, r3
	bl	strcpy
	ldr	r3, .L87+4
	ldr	r2, [r3, #0]
	ldr	r1, .L87
	mov	r3, r2
	mov	r3, r3, asl #6
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r1, r3
	ldr	r3, [r3, #0]
	add	r1, r2, #1
	ldr	r2, .L87+4
	str	r1, [r2, #0]
.L85:
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L88:
	.align	2
.L87:
	.word	functions
	.word	funcCount
	.size	getFunction, .-getFunction
	.align	2
	.type	genCode, %function
genCode:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #12
	mov	r3, r0
	strb	r3, [fp, #-5]
	ldr	r3, .L90
	ldr	r1, [r3, #0]
	ldr	r3, .L90+4
	ldr	r3, [r3, #0]
	mov	r2, r3
	add	r2, r1, r2
	ldrb	r1, [fp, #-5]
	strb	r1, [r2, #0]
	add	r2, r3, #1
	ldr	r3, .L90+4
	str	r2, [r3, #0]
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
.L91:
	.align	2
.L90:
	.word	jitCode
	.word	jitCount
	.size	genCode, .-genCode
	.align	2
	.type	genCodeInt32, %function
genCodeInt32:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	str	r0, [fp, #-8]
	ldr	r3, [fp, #-8]
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	ldr	r3, [fp, #-8]
	mov	r3, r3, asl #16
	mov	r3, r3, lsr #24
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	ldr	r3, [fp, #-8]
	mov	r3, r3, asl #8
	mov	r3, r3, lsr #24
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	ldr	r3, [fp, #-8]
	mov	r3, r3, lsr #24
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
	.size	genCodeInt32, .-genCodeInt32
	.align	2
	.type	genCodeInt32Insert, %function
genCodeInt32Insert:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #12
	str	r0, [fp, #-8]
	str	r1, [fp, #-12]
	ldr	r3, .L94
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-12]
	add	r3, r2, r3
	ldr	r2, [fp, #-8]
	and	r2, r2, #255
	strb	r2, [r3, #0]
	ldr	r3, .L94
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-12]
	add	r3, r3, #1
	add	r3, r2, r3
	ldr	r2, [fp, #-8]
	mov	r2, r2, asl #16
	mov	r2, r2, lsr #24
	and	r2, r2, #255
	strb	r2, [r3, #0]
	ldr	r3, .L94
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-12]
	add	r3, r3, #2
	add	r3, r2, r3
	ldr	r2, [fp, #-8]
	mov	r2, r2, asl #8
	mov	r2, r2, lsr #24
	and	r2, r2, #255
	strb	r2, [r3, #0]
	ldr	r3, .L94
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-12]
	add	r3, r3, #3
	add	r3, r2, r3
	ldr	r2, [fp, #-8]
	mov	r2, r2, lsr #24
	and	r2, r2, #255
	strb	r2, [r3, #0]
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
.L95:
	.align	2
.L94:
	.word	jitCode
	.size	genCodeInt32Insert, .-genCodeInt32Insert
	.section	.rodata
	.align	2
.LC32:
	.ascii	"eax\000"
	.align	2
.LC33:
	.ascii	"ebx\000"
	.align	2
.LC34:
	.ascii	"ecx\000"
	.align	2
.LC35:
	.ascii	"edx\000"
	.align	2
.LC36:
	.ascii	"esp\000"
	.align	2
.LC37:
	.ascii	"edi\000"
	.align	2
.LC38:
	.ascii	"ebp\000"
	.align	2
.LC39:
	.ascii	"esi\000"
	.text
	.align	2
	.type	regBit, %function
regBit:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	str	r0, [fp, #-8]
	ldr	r0, .L106
	ldr	r1, [fp, #-8]
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L97
	mov	r3, #0
	b	.L98
.L97:
	ldr	r0, .L106+4
	ldr	r1, [fp, #-8]
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L99
	mov	r3, #3
	b	.L98
.L99:
	ldr	r0, .L106+8
	ldr	r1, [fp, #-8]
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L100
	mov	r3, #1
	b	.L98
.L100:
	ldr	r0, .L106+12
	ldr	r1, [fp, #-8]
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L101
	mov	r3, #2
	b	.L98
.L101:
	ldr	r0, .L106+16
	ldr	r1, [fp, #-8]
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L102
	mov	r3, #4
	b	.L98
.L102:
	ldr	r0, .L106+20
	ldr	r1, [fp, #-8]
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L103
	mov	r3, #7
	b	.L98
.L103:
	ldr	r0, .L106+24
	ldr	r1, [fp, #-8]
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L104
	mov	r3, #5
	b	.L98
.L104:
	ldr	r0, .L106+28
	ldr	r1, [fp, #-8]
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L105
	mov	r3, #6
	b	.L98
.L105:
	mvn	r3, #0
.L98:
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L107:
	.align	2
.L106:
	.word	.LC32
	.word	.LC33
	.word	.LC34
	.word	.LC35
	.word	.LC36
	.word	.LC37
	.word	.LC38
	.word	.LC39
	.size	regBit, .-regBit
	.align	2
	.type	mk_modrm, %function
mk_modrm:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #16
	str	r0, [fp, #-16]
	str	r1, [fp, #-20]
	ldr	r0, [fp, #-16]
	bl	regBit
	mov	r3, r0
	mov	r3, r3, asl #3
	str	r3, [fp, #-8]
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-20]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r2, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #1024
	cmp	r3, #0
	beq	.L109
	ldr	r3, [fp, #-8]
	add	r3, r3, #192
	str	r3, [fp, #-8]
	ldr	r0, [fp, #-20]
	bl	regBit
	mov	r3, r0
	ldr	r2, [fp, #-8]
	add	r3, r2, r3
	str	r3, [fp, #-8]
	b	.L110
.L109:
	bl	__ctype_b_loc
.L110:
	ldr	r3, [fp, #-8]
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
	.size	mk_modrm, .-mk_modrm
	.section	.rodata
	.align	2
.LC40:
	.ascii	"src> %s\012\000"
	.align	2
.LC41:
	.ascii	"as> %s<\012\000"
	.align	2
.LC42:
	.ascii	"mov\000"
	.align	2
.LC43:
	.ascii	"add\000"
	.align	2
.LC44:
	.ascii	"sub\000"
	.align	2
.LC45:
	.ascii	"mul\000"
	.align	2
.LC46:
	.ascii	"div\000"
	.align	2
.LC47:
	.ascii	"push\000"
	.align	2
.LC48:
	.ascii	"pop\000"
	.text
	.align	2
	.type	genas, %function
genas:
	@ Function supports interworking.
	@ args = 4, pretend = 16, frame = 88
	@ frame_needed = 1, uses_anonymous_args = 1
	stmfd	sp!, {r0, r1, r2, r3}
	stmfd	sp!, {r4, fp, lr}
	add	fp, sp, #8
	sub	sp, sp, #92
	ldr	r0, [fp, #4]
	bl	strlen
	mov	r3, r0
	add	r3, r3, #255
	mov	r0, #1
	mov	r1, r3
	bl	calloc
	mov	r3, r0
	str	r3, [fp, #-16]
	sub	r2, fp, #96
	mov	r3, #64
	mov	r0, r2
	mov	r1, #0
	mov	r2, r3
	bl	memset
	mov	r3, #0
	str	r3, [fp, #-24]
	add	r3, fp, #8
	str	r3, [fp, #-32]
	ldr	r3, [fp, #4]
	ldr	r0, [fp, #-16]
	mov	r1, r3
	ldr	r2, [fp, #-32]
	bl	vsprintf
	ldr	r0, [fp, #-16]
	bl	strlen
	mov	r3, r0
	add	r3, r3, #1
	str	r3, [fp, #-28]
	ldr	r3, .L131
	mov	r0, r3
	ldr	r1, [fp, #-16]
	bl	printf
	b	.L112
.L118:
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-16]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r2, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #1024
	cmp	r3, #0
	bne	.L130
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-16]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r2, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #2048
	cmp	r3, #0
	beq	.L114
	b	.L130
.L117:
	ldr	r3, [fp, #-24]
	mov	r3, r3, asl #4
	sub	r2, fp, #96
	add	r2, r2, r3
	ldr	r3, [fp, #-16]
	mov	r0, r2
	mov	r1, r3
	mov	r2, #1
	bl	strncat
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
	b	.L115
.L130:
	mov	r0, r0	@ nop
.L115:
	ldr	r3, [fp, #-16]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #32
	beq	.L116
	ldr	r3, [fp, #-16]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	bne	.L117
.L116:
	ldr	r3, [fp, #-16]
	sub	r3, r3, #1
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-24]
	add	r3, r3, #1
	str	r3, [fp, #-24]
.L114:
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
.L112:
	ldr	r3, [fp, #-16]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	bne	.L118
	mov	r3, #0
	str	r3, [fp, #-24]
	ldr	r3, [fp, #-24]
	str	r3, [fp, #-20]
	b	.L119
.L120:
	ldr	r2, .L131+4
	ldr	r3, [fp, #-20]
	mov	r3, r3, asl #4
	sub	r1, fp, #96
	add	r3, r1, r3
	mov	r0, r2
	mov	r1, r3
	bl	printf
	ldr	r3, [fp, #-20]
	add	r3, r3, #1
	str	r3, [fp, #-20]
.L119:
	ldr	r3, [fp, #-20]
	cmp	r3, #3
	ble	.L120
	sub	r3, fp, #96
	mov	r0, r3
	ldr	r1, .L131+8
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L121
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r2, [r3, #0]
	sub	r3, fp, #96
	add	r3, r3, #32
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r2, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #1024
	cmp	r3, #0
	beq	.L122
	mov	r0, #137
	bl	genCode
	sub	r3, fp, #96
	add	r3, r3, #32
	mov	r0, r3
	bl	regBit
	mov	r3, r0
	add	r3, r3, #24
	and	r3, r3, #255
	mov	r3, r3, asl #3
	and	r4, r3, #255
	sub	r3, fp, #96
	add	r3, r3, #16
	mov	r0, r3
	bl	regBit
	mov	r3, r0
	and	r3, r3, #255
	add	r3, r4, r3
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	b	.L123
.L122:
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r2, [r3, #0]
	sub	r3, fp, #96
	add	r3, r3, #32
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r2, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #2048
	cmp	r3, #0
	beq	.L123
	sub	r3, fp, #96
	add	r3, r3, #16
	mov	r0, r3
	bl	regBit
	mov	r3, r0
	and	r3, r3, #255
	sub	r3, r3, #72
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	sub	r3, fp, #96
	add	r3, r3, #32
	mov	r0, r3
	bl	atoi
	mov	r3, r0
	mov	r0, r3
	bl	genCodeInt32
	b	.L123
.L121:
	sub	r3, fp, #96
	mov	r0, r3
	ldr	r1, .L131+12
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L124
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r2, [r3, #0]
	sub	r3, fp, #96
	add	r3, r3, #32
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r2, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #1024
	cmp	r3, #0
	beq	.L125
	mov	r0, #3
	bl	genCode
	sub	r3, fp, #96
	add	r2, r3, #16
	sub	r3, fp, #96
	add	r3, r3, #32
	mov	r0, r2
	mov	r1, r3
	bl	mk_modrm
	mov	r3, r0
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	b	.L123
.L125:
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r2, [r3, #0]
	sub	r3, fp, #96
	add	r3, r3, #32
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r2, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #2048
	cmp	r3, #0
	beq	.L123
	mov	r0, #129
	bl	genCode
	sub	r3, fp, #96
	add	r3, r3, #16
	mov	r0, r3
	bl	regBit
	mov	r3, r0
	and	r3, r3, #255
	sub	r3, r3, #64
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	sub	r3, fp, #96
	add	r3, r3, #32
	mov	r0, r3
	bl	atoi
	mov	r3, r0
	mov	r0, r3
	bl	genCodeInt32
	b	.L123
.L124:
	sub	r3, fp, #96
	mov	r0, r3
	ldr	r1, .L131+16
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L126
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r2, [r3, #0]
	sub	r3, fp, #96
	add	r3, r3, #32
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r2, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #1024
	cmp	r3, #0
	beq	.L123
	mov	r0, #43
	bl	genCode
	sub	r3, fp, #96
	add	r2, r3, #16
	sub	r3, fp, #96
	add	r3, r3, #32
	mov	r0, r2
	mov	r1, r3
	bl	mk_modrm
	mov	r3, r0
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	b	.L123
.L126:
	sub	r3, fp, #96
	mov	r0, r3
	ldr	r1, .L131+20
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L127
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r2, [r3, #0]
	sub	r3, fp, #96
	add	r3, r3, #16
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r2, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #1024
	cmp	r3, #0
	beq	.L123
	mov	r0, #247
	bl	genCode
	sub	r3, fp, #96
	add	r3, r3, #16
	mov	r0, r3
	bl	regBit
	mov	r3, r0
	and	r3, r3, #255
	sub	r3, r3, #32
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	b	.L123
.L127:
	sub	r3, fp, #96
	mov	r0, r3
	ldr	r1, .L131+24
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L128
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r2, [r3, #0]
	sub	r3, fp, #96
	add	r3, r3, #16
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r2, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #1024
	cmp	r3, #0
	beq	.L123
	mov	r0, #247
	bl	genCode
	sub	r3, fp, #96
	add	r3, r3, #16
	mov	r0, r3
	bl	regBit
	mov	r3, r0
	and	r3, r3, #255
	sub	r3, r3, #16
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	b	.L123
.L128:
	sub	r3, fp, #96
	mov	r0, r3
	ldr	r1, .L131+28
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L129
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r2, [r3, #0]
	sub	r3, fp, #96
	add	r3, r3, #16
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r2, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #1024
	cmp	r3, #0
	beq	.L123
	sub	r3, fp, #96
	add	r3, r3, #16
	mov	r0, r3
	bl	regBit
	mov	r3, r0
	and	r3, r3, #255
	add	r3, r3, #80
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	b	.L123
.L129:
	sub	r3, fp, #96
	mov	r0, r3
	ldr	r1, .L131+32
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L123
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r2, [r3, #0]
	sub	r3, fp, #96
	add	r3, r3, #16
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r2, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #1024
	cmp	r3, #0
	beq	.L123
	sub	r3, fp, #96
	add	r3, r3, #16
	mov	r0, r3
	bl	regBit
	mov	r3, r0
	and	r3, r3, #255
	add	r3, r3, #88
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
.L123:
	mov	r3, #0
	mov	r0, r3
	sub	sp, fp, #8
	ldmfd	sp!, {r4, fp, lr}
	add	sp, sp, #16
	bx	lr
.L132:
	.align	2
.L131:
	.word	.LC40
	.word	.LC41
	.word	.LC42
	.word	.LC43
	.word	.LC44
	.word	.LC45
	.word	.LC46
	.word	.LC47
	.word	.LC48
	.size	genas, .-genas
	.align	2
	.type	skip, %function
skip:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	str	r0, [fp, #-8]
	ldr	r3, .L136
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L136+4
	add	r3, r3, r2
	ldr	r0, [fp, #-8]
	mov	r1, r3
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L134
	ldr	r3, .L136
	ldr	r3, [r3, #0]
	add	r2, r3, #1
	ldr	r3, .L136
	str	r2, [r3, #0]
	mov	r3, #1
	b	.L135
.L134:
	mov	r3, #0
.L135:
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L137:
	.align	2
.L136:
	.word	tkpos
	.word	token
	.size	skip, .-skip
	.section	.rodata
	.align	2
.LC49:
	.ascii	"\000"
	.text
	.align	2
	.type	error, %function
error:
	@ Function supports interworking.
	@ args = 4, pretend = 16, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 1
	stmfd	sp!, {r0, r1, r2, r3}
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	add	r3, fp, #8
	str	r3, [fp, #-8]
	ldr	r3, [fp, #4]
	mov	r0, r3
	ldr	r1, [fp, #-8]
	bl	vprintf
	ldr	r0, .L139
	bl	puts
	mov	r0, #0
	bl	exit
.L140:
	.align	2
.L139:
	.word	.LC49
	.size	error, .-error
	.section	.rodata
	.align	2
.LC50:
	.ascii	"<\000"
	.align	2
.LC51:
	.ascii	">\000"
	.align	2
.LC52:
	.ascii	"!=\000"
	.align	2
.LC53:
	.ascii	"==\000"
	.align	2
.LC54:
	.ascii	"<=\000"
	.align	2
.LC55:
	.ascii	">=\000"
	.align	2
.LC56:
	.ascii	"mov ebx eax\000"
	.text
	.align	2
	.type	relExpr, %function
relExpr:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #24
	mov	r3, #0
	str	r3, [fp, #-24]
	mov	r3, #0
	str	r3, [fp, #-8]
	mov	r3, #0
	str	r3, [fp, #-12]
	mov	r3, #0
	str	r3, [fp, #-16]
	mov	r3, #0
	str	r3, [fp, #-20]
	bl	addSubExpr
	ldr	r0, .L155
	bl	skip
	str	r0, [fp, #-24]
	ldr	r3, [fp, #-24]
	cmp	r3, #0
	bne	.L142
	ldr	r0, .L155+4
	bl	skip
	str	r0, [fp, #-8]
	ldr	r3, [fp, #-8]
	cmp	r3, #0
	bne	.L142
	ldr	r0, .L155+8
	bl	skip
	str	r0, [fp, #-12]
	ldr	r3, [fp, #-12]
	cmp	r3, #0
	bne	.L142
	ldr	r0, .L155+12
	bl	skip
	str	r0, [fp, #-16]
	ldr	r3, [fp, #-16]
	cmp	r3, #0
	bne	.L142
	ldr	r0, .L155+16
	bl	skip
	str	r0, [fp, #-20]
	ldr	r3, [fp, #-20]
	cmp	r3, #0
	bne	.L142
	ldr	r0, .L155+20
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L143
.L142:
	ldr	r0, .L155+24
	bl	genas
	bl	addSubExpr
	ldr	r0, .L155+28
	bl	genas
	ldr	r0, .L155+32
	bl	genas
	mov	r0, #57
	bl	genCode
	mov	r0, #216
	bl	genCode
	ldr	r3, [fp, #-24]
	cmp	r3, #0
	bne	.L144
	ldr	r3, [fp, #-8]
	cmp	r3, #0
	bne	.L145
	ldr	r3, [fp, #-12]
	cmp	r3, #0
	bne	.L146
	ldr	r3, [fp, #-16]
	cmp	r3, #0
	bne	.L147
	ldr	r3, [fp, #-20]
	cmp	r3, #0
	beq	.L148
	mov	r3, #126
	b	.L149
.L148:
	mov	r3, #125
.L149:
	b	.L150
.L147:
	mov	r3, #116
.L150:
	b	.L151
.L146:
	mov	r3, #117
.L151:
	b	.L152
.L145:
	mov	r3, #127
.L152:
	b	.L153
.L144:
	mov	r3, #124
.L153:
	b	.L154
.L143:
	mov	r3, #0
.L154:
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L156:
	.align	2
.L155:
	.word	.LC50
	.word	.LC51
	.word	.LC52
	.word	.LC53
	.word	.LC54
	.word	.LC55
	.word	.LC10
	.word	.LC56
	.word	.LC11
	.size	relExpr, .-relExpr
	.section	.rodata
	.align	2
.LC57:
	.ascii	"add eax ebx\000"
	.align	2
.LC58:
	.ascii	"sub eax ebx\000"
	.align	2
.LC59:
	.ascii	"+\000"
	.align	2
.LC60:
	.ascii	"-\000"
	.text
	.align	2
	.type	addSubExpr, %function
addSubExpr:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	bl	mulDivExpr
	b	.L158
.L160:
	ldr	r0, .L161
	bl	genas
	bl	mulDivExpr
	ldr	r0, .L161+4
	bl	genas
	ldr	r0, .L161+8
	bl	genas
	ldr	r3, [fp, #-8]
	cmp	r3, #0
	beq	.L159
	ldr	r0, .L161+12
	bl	genas
	b	.L158
.L159:
	ldr	r0, .L161+16
	bl	genas
.L158:
	ldr	r0, .L161+20
	bl	skip
	str	r0, [fp, #-8]
	ldr	r3, [fp, #-8]
	cmp	r3, #0
	bne	.L160
	ldr	r0, .L161+24
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	bne	.L160
	mov	r3, #0
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L162:
	.align	2
.L161:
	.word	.LC10
	.word	.LC56
	.word	.LC11
	.word	.LC57
	.word	.LC58
	.word	.LC59
	.word	.LC60
	.size	addSubExpr, .-addSubExpr
	.section	.rodata
	.align	2
.LC61:
	.ascii	"mul ebx\000"
	.align	2
.LC62:
	.ascii	"mov edx 0\000"
	.align	2
.LC63:
	.ascii	"div ebx\000"
	.align	2
.LC64:
	.ascii	"mov eax edx\000"
	.align	2
.LC65:
	.ascii	"*\000"
	.align	2
.LC66:
	.ascii	"/\000"
	.align	2
.LC67:
	.ascii	"%\000"
	.text
	.align	2
	.type	mulDivExpr, %function
mulDivExpr:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	bl	primExpr
	b	.L164
.L167:
	ldr	r0, .L168
	bl	genas
	bl	primExpr
	ldr	r0, .L168+4
	bl	genas
	ldr	r0, .L168+8
	bl	genas
	ldr	r3, [fp, #-12]
	cmp	r3, #0
	beq	.L165
	ldr	r0, .L168+12
	bl	genas
	b	.L164
.L165:
	ldr	r3, [fp, #-8]
	cmp	r3, #0
	beq	.L166
	ldr	r0, .L168+16
	bl	genas
	ldr	r0, .L168+20
	bl	genas
	b	.L164
.L166:
	ldr	r0, .L168+16
	bl	genas
	ldr	r0, .L168+20
	bl	genas
	ldr	r0, .L168+24
	bl	genas
.L164:
	ldr	r0, .L168+28
	bl	skip
	str	r0, [fp, #-12]
	ldr	r3, [fp, #-12]
	cmp	r3, #0
	bne	.L167
	ldr	r0, .L168+32
	bl	skip
	str	r0, [fp, #-8]
	ldr	r3, [fp, #-8]
	cmp	r3, #0
	bne	.L167
	ldr	r0, .L168+36
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	bne	.L167
	mov	r3, #0
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L169:
	.align	2
.L168:
	.word	.LC10
	.word	.LC56
	.word	.LC11
	.word	.LC61
	.word	.LC62
	.word	.LC63
	.word	.LC64
	.word	.LC65
	.word	.LC66
	.word	.LC67
	.size	mulDivExpr, .-mulDivExpr
	.section	.rodata
	.align	2
.LC68:
	.ascii	"mov eax %s\000"
	.align	2
.LC69:
	.ascii	"mov edx eax\000"
	.align	2
.LC70:
	.ascii	"error: %d: expected expression ']'\000"
	.align	2
.LC71:
	.ascii	"addr: %d\012\000"
	.align	2
.LC72:
	.ascii	"add esp 4\000"
	.align	2
.LC73:
	.ascii	"error: %d: expected expression ')'\000"
	.align	2
.LC74:
	.ascii	"size: OK: %d\012\000"
	.align	2
.LC75:
	.ascii	"prim>%s\012\000"
	.text
	.align	2
	.type	primExpr, %function
primExpr:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #16
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r1, [r3, #0]
	ldr	r3, .L182
	ldr	r2, [r3, #0]
	ldr	r0, .L182+4
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r1, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #2048
	cmp	r3, #0
	beq	.L171
	ldr	r3, .L182
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r1, .L182+4
	add	r3, r3, r1
	add	r1, r2, #1
	ldr	r2, .L182
	str	r1, [r2, #0]
	ldr	r0, .L182+8
	mov	r1, r3
	bl	genas
	b	.L172
.L171:
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r1, [r3, #0]
	ldr	r3, .L182
	ldr	r2, [r3, #0]
	ldr	r0, .L182+4
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r1, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #1024
	cmp	r3, #0
	beq	.L173
	ldr	r3, .L182
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L182+4
	add	r3, r3, r2
	mov	r0, r3
	mov	r1, #0
	bl	getNumOfVar
	str	r0, [fp, #-8]
	ldr	r3, .L182
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L182+4
	add	r3, r3, r2
	str	r3, [fp, #-12]
	ldr	r3, .L182
	ldr	r3, [r3, #0]
	add	r2, r3, #1
	ldr	r3, .L182
	str	r2, [r3, #0]
	ldr	r0, .L182+12
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L174
	bl	relExpr
	ldr	r0, .L182+16
	bl	genas
	mov	r0, #139
	bl	genCode
	mov	r0, #68
	bl	genCode
	mov	r0, #149
	bl	genCode
	ldr	r3, [fp, #-8]
	and	r3, r3, #255
	mov	r2, r3
	mov	r2, r2, asl #6
	rsb	r3, r3, r2
	mov	r3, r3, asl #2
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	ldr	r0, .L182+20
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	bne	.L172
	ldr	r3, .L182
	ldr	r2, [r3, #0]
	ldr	r0, .L182+4
	mov	r1, #32
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	add	r3, r3, r1
	ldr	r3, [r3, #0]
	ldr	r0, .L182+24
	mov	r1, r3
	bl	error
	b	.L172
.L174:
	ldr	r0, .L182+28
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L175
	ldr	r0, [fp, #-12]
	mov	r1, #0
	bl	getFunction
	str	r0, [fp, #-16]
	ldr	r3, .L182+32
	mov	r0, r3
	ldr	r1, [fp, #-16]
	bl	printf
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r1, [r3, #0]
	ldr	r3, .L182
	ldr	r2, [r3, #0]
	ldr	r0, .L182+4
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r1, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #1024
	cmp	r3, #0
	bne	.L176
	bl	__ctype_b_loc
	mov	r3, r0
	ldr	r1, [r3, #0]
	ldr	r3, .L182
	ldr	r2, [r3, #0]
	ldr	r0, .L182+4
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	mov	r3, r3, asl #1
	add	r3, r1, r3
	ldrh	r3, [r3, #0]
	and	r3, r3, #2048
	cmp	r3, #0
	bne	.L176
	ldr	r3, .L182
	ldr	r2, [r3, #0]
	ldr	r1, .L182+4
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r1, r3
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #40
	bne	.L177
.L176:
	bl	relExpr
	ldr	r0, .L182+36
	bl	genas
	ldr	r0, .L182+40
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	bne	.L176
.L177:
	mov	r0, #232
	bl	genCode
	ldr	r3, .L182+44
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-16]
	rsb	r3, r3, r2
	mvn	r3, r3
	sub	r3, r3, #3
	mov	r0, r3
	bl	genCodeInt32
	ldr	r0, .L182+48
	bl	genas
	ldr	r0, .L182+52
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	bne	.L172
	ldr	r3, .L182
	ldr	r2, [r3, #0]
	ldr	r0, .L182+4
	mov	r1, #32
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	add	r3, r3, r1
	ldr	r3, [r3, #0]
	ldr	r0, .L182+56
	mov	r1, r3
	bl	error
	b	.L172
.L175:
	mov	r0, #139
	bl	genCode
	mov	r0, #69
	bl	genCode
	ldr	r3, [fp, #-8]
	and	r3, r3, #255
	mov	r2, r3
	mov	r2, r2, asl #6
	rsb	r3, r3, r2
	mov	r3, r3, asl #2
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	ldr	r2, .L182+60
	ldr	r3, .L182+44
	ldr	r3, [r3, #0]
	mov	r0, r2
	mov	r1, r3
	bl	printf
	b	.L172
.L173:
	ldr	r0, .L182+20
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L178
	ldr	r3, .L182
	ldr	r2, [r3, #0]
	ldr	r0, .L182+4
	mov	r1, #32
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	add	r3, r3, r1
	ldr	r3, [r3, #0]
	ldr	r0, .L182+64
	mov	r1, r3
	bl	error
	b	.L172
.L178:
	ldr	r0, .L182+28
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L179
	bl	isassign
	mov	r3, r0
	cmp	r3, #0
	beq	.L180
	bl	assignment
	b	.L181
.L180:
	bl	relExpr
.L181:
	ldr	r0, .L182+52
	bl	skip
	b	.L172
.L179:
	ldr	r0, .L182+68
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	bne	.L172
	ldr	r1, .L182+72
	ldr	r3, .L182
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L182+4
	add	r3, r3, r2
	mov	r0, r1
	mov	r1, r3
	bl	printf
	ldr	r3, .L182
	ldr	r2, [r3, #0]
	ldr	r0, .L182+4
	mov	r1, #32
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	add	r3, r3, r1
	ldr	r3, [r3, #0]
	ldr	r0, .L182+64
	mov	r1, r3
	bl	error
.L172:
	mov	r3, #0
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L183:
	.align	2
.L182:
	.word	tkpos
	.word	token
	.word	.LC68
	.word	.LC5
	.word	.LC69
	.word	.LC7
	.word	.LC70
	.word	.LC14
	.word	.LC71
	.word	.LC10
	.word	.LC12
	.word	jitCount
	.word	.LC72
	.word	.LC15
	.word	.LC73
	.word	.LC74
	.word	.LC6
	.word	.LC2
	.word	.LC75
	.size	primExpr, .-primExpr
	.align	2
	.type	ifStmt, %function
ifStmt:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	bl	relExpr
	str	r0, [fp, #-8]
	ldr	r3, [fp, #-8]
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	mov	r0, #5
	bl	genCode
	mov	r0, #233
	bl	genCode
	ldr	r3, .L185
	ldr	r3, [r3, #0]
	str	r3, [fp, #-12]
	mov	r0, #0
	bl	genCodeInt32
	ldr	r0, [fp, #-12]
	mov	r1, #0
	bl	eval
	mov	r3, r0
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L186:
	.align	2
.L185:
	.word	jitCount
	.size	ifStmt, .-ifStmt
	.align	2
	.type	whileStmt, %function
whileStmt:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #16
	ldr	r3, .L191
	ldr	r3, [r3, #0]
	str	r3, [fp, #-8]
	bl	relExpr
	str	r0, [fp, #-12]
	ldr	r3, [fp, #-12]
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	mov	r0, #5
	bl	genCode
	mov	r0, #233
	bl	genCode
	ldr	r3, .L191
	ldr	r3, [r3, #0]
	str	r3, [fp, #-16]
	mov	r0, #0
	bl	genCodeInt32
	mov	r0, #0
	mov	r1, #1
	bl	eval
	mov	r3, r0
	cmp	r3, #0
	beq	.L188
	ldr	r3, .L191
	ldr	r3, [r3, #0]
	mvn	r2, r3
	ldr	r3, [fp, #-8]
	add	r3, r2, r3
	sub	r3, r3, #4
	str	r3, [fp, #-20]
	mov	r0, #233
	bl	genCode
	ldr	r0, [fp, #-20]
	bl	genCodeInt32
	ldr	r3, .L191
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-16]
	rsb	r3, r3, r2
	sub	r3, r3, #4
	mov	r0, r3
	ldr	r1, [fp, #-16]
	bl	genCodeInt32Insert
	ldr	r3, .L191+4
	ldr	r3, [r3, #0]
	sub	r2, r3, #1
	ldr	r3, .L191+4
	str	r2, [r3, #0]
	b	.L189
.L190:
	ldr	r3, .L191
	ldr	r2, [r3, #0]
	ldr	r3, .L191+4
	ldr	r1, [r3, #0]
	ldr	r3, .L191+8
	ldr	r3, [r3, r1, asl #2]
	rsb	r3, r3, r2
	sub	r3, r3, #4
	mov	r2, r3
	ldr	r3, .L191+4
	ldr	r1, [r3, #0]
	ldr	r3, .L191+8
	ldr	r3, [r3, r1, asl #2]
	mov	r0, r2
	mov	r1, r3
	bl	genCodeInt32Insert
	ldr	r3, .L191+4
	ldr	r3, [r3, #0]
	sub	r2, r3, #1
	ldr	r3, .L191+4
	str	r2, [r3, #0]
.L189:
	ldr	r3, .L191+4
	ldr	r3, [r3, #0]
	cmp	r3, #0
	bge	.L190
	ldr	r3, .L191+4
	mov	r2, #0
	str	r2, [r3, #0]
.L188:
	mov	r3, #0
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L192:
	.align	2
.L191:
	.word	jitCount
	.word	brkCount
	.word	breaks
	.size	whileStmt, .-whileStmt
	.section	.rodata
	.align	2
.LC76:
	.ascii	"=\000"
	.align	2
.LC77:
	.ascii	">%s\012\000"
	.text
	.align	2
	.type	isassign, %function
isassign:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	ldr	r3, .L200
	ldr	r3, [r3, #0]
	add	r2, r3, #1
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L200+4
	add	r3, r3, r2
	mov	r0, r3
	ldr	r1, .L200+8
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L194
	mov	r3, #1
	b	.L195
.L194:
	ldr	r3, .L200
	ldr	r3, [r3, #0]
	add	r2, r3, #1
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L200+4
	add	r3, r3, r2
	mov	r0, r3
	ldr	r1, .L200+12
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L196
	ldr	r3, .L200
	ldr	r3, [r3, #0]
	add	r3, r3, #1
	str	r3, [fp, #-8]
	b	.L197
.L199:
	ldr	r2, [fp, #-8]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L200+4
	add	r3, r3, r2
	mov	r0, r3
	ldr	r1, .L200+16
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L198
	ldr	r3, .L200
	ldr	r2, [r3, #0]
	ldr	r0, .L200+4
	mov	r1, #32
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	add	r3, r3, r1
	ldr	r3, [r3, #0]
	ldr	r0, .L200+20
	mov	r1, r3
	bl	error
.L198:
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L197:
	ldr	r2, [fp, #-8]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L200+4
	add	r3, r3, r2
	mov	r0, r3
	ldr	r1, .L200+24
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L199
	ldr	r1, .L200+28
	ldr	r2, [fp, #-8]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L200+4
	add	r3, r3, r2
	mov	r0, r1
	mov	r1, r3
	bl	printf
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
	ldr	r2, [fp, #-8]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r2, .L200+4
	add	r3, r3, r2
	mov	r0, r3
	ldr	r1, .L200+8
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L196
	mov	r3, #1
	b	.L195
.L196:
	mov	r3, #0
.L195:
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L201:
	.align	2
.L200:
	.word	tkpos
	.word	token
	.word	.LC76
	.word	.LC5
	.word	.LC2
	.word	.LC6
	.word	.LC7
	.word	.LC77
	.size	isassign, .-isassign
	.section	.rodata
	.align	2
.LC78:
	.ascii	"mov ecx eax\000"
	.align	2
.LC79:
	.ascii	"error: %d: invalid assignment\000"
	.text
	.align	2
	.type	assignment, %function
assignment:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	ldr	r3, .L207
	ldr	r2, [r3, #0]
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	ldr	r1, .L207+4
	add	r3, r3, r1
	add	r1, r2, #1
	ldr	r2, .L207
	str	r1, [r2, #0]
	mov	r0, r3
	mov	r1, #0
	bl	getNumOfVar
	str	r0, [fp, #-8]
	ldr	r0, .L207+8
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L203
	bl	relExpr
	ldr	r0, .L207+12
	bl	genas
	ldr	r0, .L207+16
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	beq	.L204
	ldr	r0, .L207+20
	bl	skip
	mov	r3, r0
	cmp	r3, #0
	bne	.L205
.L204:
	ldr	r3, .L207
	ldr	r2, [r3, #0]
	ldr	r0, .L207+4
	mov	r1, #32
	mov	r3, r2
	mov	r3, r3, asl #3
	add	r3, r3, r2
	mov	r3, r3, asl #2
	add	r3, r0, r3
	add	r3, r3, r1
	ldr	r3, [r3, #0]
	ldr	r0, .L207+24
	mov	r1, r3
	bl	error
.L205:
	bl	relExpr
	mov	r0, #137
	bl	genCode
	mov	r0, #68
	bl	genCode
	mov	r0, #141
	bl	genCode
	ldr	r3, [fp, #-8]
	and	r3, r3, #255
	mov	r2, r3
	mov	r2, r2, asl #6
	rsb	r3, r3, r2
	mov	r3, r3, asl #2
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	b	.L206
.L203:
	ldr	r0, .L207+20
	bl	skip
	bl	relExpr
	mov	r0, #137
	bl	genCode
	mov	r0, #69
	bl	genCode
	ldr	r3, [fp, #-8]
	and	r3, r3, #255
	mov	r2, r3
	mov	r2, r2, asl #6
	rsb	r3, r3, r2
	mov	r3, r3, asl #2
	and	r3, r3, #255
	mov	r0, r3
	bl	genCode
	ldr	r2, .L207+28
	ldr	r3, .L207
	ldr	r3, [r3, #0]
	mov	r0, r2
	mov	r1, r3
	bl	printf
.L206:
	mov	r3, #0
	mov	r0, r3
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L208:
	.align	2
.L207:
	.word	tkpos
	.word	token
	.word	.LC5
	.word	.LC78
	.word	.LC7
	.word	.LC76
	.word	.LC79
	.word	.LC30
	.size	assignment, .-assignment
	.section	.rodata
	.align	2
.LC80:
	.ascii	"\\a\000"
	.align	2
.LC81:
	.ascii	"\007\000"
	.space	1
	.align	2
.LC82:
	.ascii	"\\r\000"
	.align	2
.LC83:
	.ascii	"\015\000"
	.space	1
	.align	2
.LC84:
	.ascii	"\\f\000"
	.align	2
.LC85:
	.ascii	"\014\000"
	.space	1
	.align	2
.LC86:
	.ascii	"\\n\000"
	.align	2
.LC87:
	.ascii	"\012\000"
	.space	1
	.align	2
.LC88:
	.ascii	"\\t\000"
	.align	2
.LC89:
	.ascii	"\011\000"
	.space	1
	.align	2
.LC90:
	.ascii	"\\b\000"
	.align	2
.LC91:
	.ascii	"\010\000"
	.space	1
	.text
	.align	2
	.type	replaceEscape, %function
replaceEscape:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 56
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, fp, lr}
	add	fp, sp, #12
	sub	sp, sp, #56
	str	r0, [fp, #-64]
	sub	r3, fp, #56
	mov	r2, #0
	str	r2, [r3, #0]
	add	r3, r3, #4
	mov	r2, #0
	str	r2, [r3, #0]
	add	r3, r3, #4
	mov	r2, #0
	str	r2, [r3, #0]
	add	r3, r3, #4
	mov	r2, #0
	str	r2, [r3, #0]
	add	r3, r3, #4
	mov	r2, #0
	str	r2, [r3, #0]
	add	r3, r3, #4
	mov	r2, #0
	str	r2, [r3, #0]
	add	r3, r3, #4
	mov	r2, #0
	str	r2, [r3, #0]
	add	r3, r3, #4
	mov	r2, #0
	str	r2, [r3, #0]
	add	r3, r3, #4
	mov	r2, #0
	str	r2, [r3, #0]
	add	r3, r3, #4
	ldr	r2, .L215
	sub	r3, fp, #56
	ldr	r2, [r2, #0]
	strh	r2, [r3, #0]	@ movhi
	add	r3, r3, #2
	mov	r2, r2, lsr #16
	strb	r2, [r3, #0]
	ldr	r3, .L215+4
	sub	r1, fp, #53
	mov	r2, r3
	mov	r3, #2
	mov	r0, r1
	mov	r1, r2
	mov	r2, r3
	bl	memcpy
	mov	r3, #0
	strb	r3, [fp, #-51]
	ldr	r3, .L215+8
	sub	r1, fp, #50
	mov	r2, r3
	mov	r3, #3
	mov	r0, r1
	mov	r1, r2
	mov	r2, r3
	bl	memcpy
	ldr	r3, .L215+12
	sub	r1, fp, #47
	mov	r2, r3
	mov	r3, #2
	mov	r0, r1
	mov	r1, r2
	mov	r2, r3
	bl	memcpy
	mov	r3, #0
	strb	r3, [fp, #-45]
	ldr	r2, .L215+16
	sub	r3, fp, #44
	ldr	r2, [r2, #0]
	strh	r2, [r3, #0]	@ movhi
	add	r3, r3, #2
	mov	r2, r2, lsr #16
	strb	r2, [r3, #0]
	ldr	r3, .L215+20
	sub	r1, fp, #41
	mov	r2, r3
	mov	r3, #2
	mov	r0, r1
	mov	r1, r2
	mov	r2, r3
	bl	memcpy
	mov	r3, #0
	strb	r3, [fp, #-39]
	ldr	r3, .L215+24
	sub	r1, fp, #38
	mov	r2, r3
	mov	r3, #3
	mov	r0, r1
	mov	r1, r2
	mov	r2, r3
	bl	memcpy
	ldr	r3, .L215+28
	sub	r1, fp, #35
	mov	r2, r3
	mov	r3, #2
	mov	r0, r1
	mov	r1, r2
	mov	r2, r3
	bl	memcpy
	mov	r3, #0
	strb	r3, [fp, #-33]
	ldr	r2, .L215+32
	sub	r3, fp, #32
	ldr	r2, [r2, #0]
	strh	r2, [r3, #0]	@ movhi
	add	r3, r3, #2
	mov	r2, r2, lsr #16
	strb	r2, [r3, #0]
	ldr	r3, .L215+36
	sub	r1, fp, #29
	mov	r2, r3
	mov	r3, #2
	mov	r0, r1
	mov	r1, r2
	mov	r2, r3
	bl	memcpy
	mov	r3, #0
	strb	r3, [fp, #-27]
	ldr	r3, .L215+40
	sub	r1, fp, #26
	mov	r2, r3
	mov	r3, #3
	mov	r0, r1
	mov	r1, r2
	mov	r2, r3
	bl	memcpy
	ldr	r3, .L215+44
	sub	r1, fp, #23
	mov	r2, r3
	mov	r3, #2
	mov	r0, r1
	mov	r1, r2
	mov	r2, r3
	bl	memcpy
	mov	r3, #0
	strb	r3, [fp, #-21]
	mov	r3, #0
	str	r3, [fp, #-16]
	b	.L210
.L212:
	ldr	r3, [fp, #-16]
	add	r2, r3, #1
	mvn	r1, #43
	mov	r3, r2
	mov	r3, r3, asl #1
	add	r3, r3, r2
	sub	r2, fp, #12
	add	r3, r2, r3
	add	r3, r3, r1
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, [fp, #-20]
	strb	r2, [r3, #0]
	ldr	r3, [fp, #-20]
	add	r5, r3, #1
	ldr	r3, [fp, #-20]
	add	r4, r3, #2
	ldr	r0, [fp, #-64]
	bl	strlen
	mov	r3, r0
	sub	r3, r3, #1
	mov	r0, r5
	mov	r1, r4
	mov	r2, r3
	bl	memmove
	b	.L211
.L214:
	mov	r0, r0	@ nop
.L211:
	ldr	r2, [fp, #-16]
	mov	r3, r2
	mov	r3, r3, asl #1
	add	r3, r3, r2
	sub	r2, fp, #56
	add	r3, r2, r3
	ldr	r0, [fp, #-64]
	mov	r1, r3
	bl	strstr
	str	r0, [fp, #-20]
	ldr	r3, [fp, #-20]
	cmp	r3, #0
	bne	.L212
	ldr	r3, [fp, #-16]
	add	r3, r3, #2
	str	r3, [fp, #-16]
.L210:
	ldr	r3, [fp, #-16]
	cmp	r3, #11
	ble	.L214
	ldr	r3, [fp, #-64]
	mov	r0, r3
	sub	sp, fp, #12
	ldmfd	sp!, {r4, r5, fp, lr}
	bx	lr
.L216:
	.align	2
.L215:
	.word	.LC80
	.word	.LC81
	.word	.LC82
	.word	.LC83
	.word	.LC84
	.word	.LC85
	.word	.LC86
	.word	.LC87
	.word	.LC88
	.word	.LC89
	.word	.LC90
	.word	.LC91
	.size	replaceEscape, .-replaceEscape
	.section	.rodata
	.align	2
.LC92:
	.ascii	"%d\000"
	.text
	.align	2
	.type	putNumber, %function
putNumber:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	str	r0, [fp, #-8]
	ldr	r3, .L218
	mov	r0, r3
	ldr	r1, [fp, #-8]
	bl	printf
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L219:
	.align	2
.L218:
	.word	.LC92
	.size	putNumber, .-putNumber
	.section	.rodata
	.align	2
.LC93:
	.ascii	"%s\000"
	.text
	.align	2
	.type	putString, %function
putString:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	str	r0, [fp, #-8]
	ldr	r2, .L221
	ldr	r3, .L221+4
	ldr	r1, [r3, #0]
	ldr	r3, [fp, #-8]
	add	r3, r1, r3
	mov	r0, r2
	mov	r1, r3
	bl	printf
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
.L222:
	.align	2
.L221:
	.word	.LC93
	.word	jitCode
	.size	putString, .-putString
	.align	2
	.type	putLN, %function
putLN:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	mov	r0, #10
	bl	putchar
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
	.size	putLN, .-putLN
	.global	funcTable
	.data
	.align	2
	.type	funcTable, %object
	.size	funcTable, 12
funcTable:
	.word	putNumber
	.word	putString
	.word	putLN
	.section	.rodata
	.align	2
.LC94:
	.ascii	"size: %dbyte, %.2lf%%\012\000"
	.global	__aeabi_i2d
	.global	__aeabi_ddiv
	.global	__aeabi_dmul
	.text
	.align	2
	.global	run
	.type	run, %function
run:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 16384
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, r6, fp, lr}
	add	fp, sp, #16
	sub	sp, sp, #16384
	sub	sp, sp, #4
	ldr	r3, .L225
	sub	r1, fp, #20
	add	r2, r1, r3
	ldr	r3, .L225+4
	mov	r0, r2
	mov	r1, #0
	mov	r2, r3
	bl	memset
	ldr	r6, .L225+8
	ldr	r3, .L225+12
	ldr	r5, [r3, #0]
	ldr	r3, .L225+12
	ldr	r3, [r3, #0]
	mov	r0, r3
	bl	__aeabi_i2d
	mov	r3, r0
	mov	r4, r1
	mov	r0, r3
	mov	r1, r4
	mov	r2, #0
	mvn	r3, #-1090519040
	sub	r3, r3, #1048576
	sub	r3, r3, #31
	bl	__aeabi_ddiv
	mov	r3, r0
	mov	r4, r1
	mov	r0, r3
	mov	r1, r4
	mov	r2, #0
	mov	r3, #1073741824
	add	r3, r3, #5832704
	bl	__aeabi_dmul
	mov	r3, r0
	mov	r4, r1
	mov	r0, r6
	mov	r1, r5
	mov	r2, r3
	mov	r3, r4
	bl	printf
	ldr	r3, .L225+16
	ldr	r3, [r3, #0]
	mov	r2, r3
	sub	r3, fp, #16320
	sub	r3, r3, #20
	sub	r3, r3, #60
	mov	r0, r3
	ldr	r1, .L225+20
	mov	lr, pc
	bx	r2
	mov	r3, r0
	mov	r0, r3
	sub	sp, fp, #16
	ldmfd	sp!, {r4, r5, r6, fp, lr}
	bx	lr
.L226:
	.align	2
.L225:
	.word	-16380
	.word	16380
	.word	.LC94
	.word	jitCount
	.word	jitCode
	.word	funcTable
	.size	run, .-run
	.section	.rodata
	.align	2
.LC95:
	.ascii	"page_size = %ld\012\000"
	.align	2
.LC96:
	.ascii	"posix_memalign\000"
	.align	2
.LC97:
	.ascii	"mprotect\000"
	.align	2
.LC98:
	.ascii	">> \000"
	.align	2
.LC99:
	.ascii	"run\012\000"
	.align	2
.LC100:
	.ascii	"rb\000"
	.align	2
.LC101:
	.ascii	"file not found\000"
	.align	2
.LC102:
	.ascii	"time: %lfsec\012\000"
	.text
	.align	2
	.global	main
	.type	main, %function
main:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 65824
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, fp, lr}
	add	fp, sp, #12
	sub	sp, sp, #65536
	sub	sp, sp, #288
	ldr	r3, .L235
	sub	r2, fp, #12
	str	r0, [r2, r3]
	ldr	r3, .L235+4
	sub	r2, fp, #12
	str	r1, [r2, r3]
	mov	r3, #65536
	str	r3, [fp, #-16]
	ldr	r3, .L235+8
	mov	r0, r3
	ldr	r1, [fp, #-16]
	bl	printf
	ldr	r2, [fp, #-16]
	ldr	r3, [fp, #-16]
	ldr	r0, .L235+12
	mov	r1, r2
	mov	r2, r3
	bl	posix_memalign
	mov	r3, r0
	cmp	r3, #0
	beq	.L228
	ldr	r0, .L235+16
	bl	perror
.L228:
	ldr	r3, .L235+12
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-16]
	mov	r0, r2
	mov	r1, r3
	mov	r2, #7
	bl	mprotect
	mov	r3, r0
	cmp	r3, #0
	beq	.L229
	ldr	r0, .L235+20
	bl	perror
.L229:
	bl	init
	ldr	r3, .L235
	sub	r1, fp, #12
	ldr	r3, [r1, r3]
	cmp	r3, #1
	bgt	.L230
	ldr	r3, .L235+24
	mov	r2, #0
	sub	r1, fp, #12
	str	r2, [r1, r3]
	mov	r2, #4
	sub	r1, fp, #12
	add	r3, r1, r3
	add	r3, r3, r2
	mov	r2, r3
	ldr	r3, .L235+28
	mov	r0, r2
	mov	r1, #0
	mov	r2, r3
	bl	memset
	mov	r3, #0
	str	r3, [fp, #-292]
	sub	r2, fp, #288
	mov	r3, #251
	mov	r0, r2
	mov	r1, #0
	mov	r2, r3
	bl	memset
	b	.L231
.L232:
	ldr	r3, .L235+32
	mov	r0, r3
	bl	printf
	sub	r2, fp, #292
	sub	r3, fp, #65536
	sub	r3, r3, #12
	sub	r3, r3, #280
	mov	r0, r3
	mov	r1, r2
	bl	strcat
	sub	r3, fp, #292
	mov	r2, r3
	mov	r3, #255
	mov	r0, r2
	mov	r1, #0
	mov	r2, r3
	bl	memset
	ldr	r3, .L235+36
	ldr	r3, [r3, #0]
	sub	r2, fp, #292
	mov	r0, r2
	mov	r1, #255
	mov	r2, r3
	bl	fgets
.L231:
	sub	r3, fp, #292
	mov	r0, r3
	ldr	r1, .L235+40
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L232
	sub	r3, fp, #65536
	sub	r3, r3, #12
	sub	r3, r3, #280
	mov	r0, r3
	bl	lex
	b	.L233
.L230:
	ldr	r3, .L235+4
	sub	r2, fp, #12
	ldr	r3, [r2, r3]
	add	r3, r3, #4
	ldr	r3, [r3, #0]
	mov	r2, r3
	ldr	r3, .L235+44
	mov	r0, r2
	mov	r1, r3
	bl	fopen
	str	r0, [fp, #-20]
	mov	r3, #0
	str	r3, [fp, #-24]
	ldr	r3, [fp, #-20]
	cmp	r3, #0
	bne	.L234
	ldr	r0, .L235+48
	bl	perror
	mov	r0, #0
	bl	exit
.L234:
	ldr	r0, [fp, #-20]
	mov	r1, #0
	mov	r2, #2
	bl	fseek
	ldr	r0, [fp, #-20]
	bl	ftell
	str	r0, [fp, #-24]
	ldr	r0, [fp, #-20]
	mov	r1, #0
	mov	r2, #0
	bl	fseek
	ldr	r3, [fp, #-24]
	add	r3, r3, #2
	mov	r0, #1
	mov	r1, r3
	bl	calloc
	mov	r3, r0
	str	r3, [fp, #-28]
	ldr	r2, [fp, #-28]
	ldr	r3, [fp, #-24]
	mov	r0, r2
	mov	r1, #1
	mov	r2, r3
	ldr	r3, [fp, #-20]
	bl	fread
	ldr	r0, [fp, #-28]
	bl	lex
.L233:
	bl	parser
	bl	clock
	str	r0, [fp, #-32]
	bl	run
	bl	clock
	str	r0, [fp, #-36]
	ldr	r5, .L235+52
	ldr	r2, [fp, #-36]
	ldr	r3, [fp, #-32]
	rsb	r3, r3, r2
	mov	r0, r3
	bl	__aeabi_i2d
	mov	r3, r0
	mov	r4, r1
	mov	r0, r3
	mov	r1, r4
	mov	r2, #0
	mov	r3, #1090519040
	add	r3, r3, #3047424
	add	r3, r3, #1152
	bl	__aeabi_ddiv
	mov	r3, r0
	mov	r4, r1
	mov	r0, r5
	mov	r2, r3
	mov	r3, r4
	bl	printf
	ldr	r3, .L235+12
	ldr	r3, [r3, #0]
	mov	r0, r3
	bl	free
	mov	r0, r3
	sub	sp, fp, #12
	ldmfd	sp!, {r4, r5, fp, lr}
	bx	lr
.L236:
	.align	2
.L235:
	.word	-65820
	.word	-65824
	.word	.LC95
	.word	jitCode
	.word	.LC96
	.word	.LC97
	.word	-65816
	.word	65531
	.word	.LC98
	.word	stdin
	.word	.LC99
	.word	.LC100
	.word	.LC101
	.word	.LC102
	.size	main, .-main
	.ident	"GCC: (Debian 4.6.3-14) 4.6.3"
	.section	.note.GNU-stack,"",%progbits
