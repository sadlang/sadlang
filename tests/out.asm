	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"test_all_outputs.sad"
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
	subq	$72, %rsp
	.seh_stackalloc 72
	.seh_endprologue
	leaq	.L__unnamed_1(%rip), %rcx
	leaq	.L__unnamed_2(%rip), %rdx
	callq	printf
	leaq	.L__unnamed_3(%rip), %rdx
	movq	%rdx, 64(%rsp)
	leaq	.L__unnamed_4(%rip), %rcx
	callq	printf
	movq	$100, 48(%rsp)
	movq	$50, 40(%rsp)
	leaq	.L__unnamed_5(%rip), %rcx
	movl	$150, %edx
	callq	printf
	movq	48(%rsp), %rdx
	subq	40(%rsp), %rdx
	leaq	.L__unnamed_6(%rip), %rcx
	callq	printf
	movq	48(%rsp), %rdx
	imulq	40(%rsp), %rdx
	leaq	.L__unnamed_7(%rip), %rcx
	callq	printf
	movabsq	$4614253070214989087, %rax
	movq	%rax, 56(%rsp)
	leaq	.L__unnamed_8(%rip), %rcx
	movq	__real@40091eb851eb851f(%rip), %xmm1
	movq	%xmm1, %rdx
	callq	printf
	leaq	.L__unnamed_9(%rip), %rcx
	leaq	.L__unnamed_10(%rip), %rdx
	callq	printf
	xorl	%eax, %eax
	addq	$72, %rsp
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
	.asciz	"=== \330\247\330\256\330\252\330\250\330\247\330\261 \330\264\330\247\331\205\331\204 \331\204\331\204\331\205\330\252\330\261\330\254\331\205 ==="

.L__unnamed_3:
	.asciz	"\331\205\330\261\330\255\330\250\330\247 \331\212\330\247 \330\271\330\247\331\204\331\205"

.L__unnamed_4:
	.asciz	"%s"

.L__unnamed_5:
	.asciz	"%lld"

.L__unnamed_6:
	.asciz	"%lld"

.L__unnamed_7:
	.asciz	"%lld"

.L__unnamed_8:
	.asciz	"%f"

.L__unnamed_9:
	.asciz	"%s"

.L__unnamed_10:
	.asciz	"=== \330\247\331\204\331\206\331\207\330\247\331\212\330\251 ==="

	.globl	_fltused
