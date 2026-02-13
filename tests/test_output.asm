	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"test_compiler_output.sad"
	.def	"رئيسية";
	.scl	2;
	.type	32;
	.endef
	.globl	__real@40091eb851eb851f
	.section	.rdata,"dr",discard,__real@40091eb851eb851f
	.p2align	3, 0x0
__real@40091eb851eb851f:
	.quad	0x40091eb851eb851f
	.text
	.globl	"رئيسية"
	.p2align	4, 0x90
"رئيسية":
.seh_proc "رئيسية"
	subq	$56, %rsp
	.seh_stackalloc 56
	.seh_endprologue
	leaq	.L__unnamed_1(%rip), %rcx
	leaq	.L__unnamed_2(%rip), %rdx
	callq	printf
	leaq	.L__unnamed_3(%rip), %rcx
	movl	$42, %edx
	callq	printf
	leaq	.L__unnamed_4(%rip), %rcx
	movq	__real@40091eb851eb851f(%rip), %xmm1
	movq	%xmm1, %rdx
	callq	printf
	movq	$10, 48(%rsp)
	movq	$20, 40(%rsp)
	leaq	.L__unnamed_5(%rip), %rcx
	movl	$30, %edx
	callq	printf
	xorl	%eax, %eax
	addq	$56, %rsp
	retq
	.seh_endproc

	.def	main;
	.scl	2;
	.type	32;
	.endef
	.globl	main
	.p2align	4, 0x90
main:
.seh_proc main
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	callq	"رئيسية"
	nop
	addq	$40, %rsp
	retq
	.seh_endproc

	.section	.rdata,"dr"
.L__unnamed_1:
	.asciz	"%s"

.L__unnamed_2:
	.asciz	"\331\205\330\261\330\255\330\250\330\247 \331\205\331\206 \330\247\331\204\331\205\330\252\330\261\330\254\331\205!"

.L__unnamed_3:
	.asciz	"%lld"

.L__unnamed_4:
	.asciz	"%f"

.L__unnamed_5:
	.asciz	"%lld"

	.globl	_fltused
