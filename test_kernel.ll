; ModuleID = 'working_exampels\13_os_dev\05_kernel.sad'
source_filename = "working_exampels\\13_os_dev\\05_kernel.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [41 x i8] c"========================================\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [47 x i8] c"    \D9\86\D9\88\D8\A7\D8\A9 \D9\86\D8\B8\D8\A7\D9\85 \D8\AA\D8\B4\D8\BA\D9\8A\D9\84 \D8\B5 \E2\80\94 Sad OS\00", align 1
@str.const.5 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.7 = private unnamed_addr constant [47 x i8] c"[\D9\86\D9\88\D8\A7\D8\A9] 1: \D8\AA\D8\B9\D8\B7\D9\8A\D9\84 \D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA...\00", align 1
@str.const.9 = private unnamed_addr constant [37 x i8] c"[\D9\86\D9\88\D8\A7\D8\A9] 2: \D9\85\D8\B3\D8\AD \D8\A7\D9\84\D8\B4\D8\A7\D8\B4\D8\A9...\00", align 1
@str.const.11 = private unnamed_addr constant [43 x i8] c"[\D9\86\D9\88\D8\A7\D8\A9] 3: \D8\B1\D8\B3\D8\A7\D9\84\D8\A9 \D8\A7\D9\84\D8\AA\D8\B1\D8\AD\D9\8A\D8\A8...\00", align 1
@str.const.13 = private unnamed_addr constant [32 x i8] c"[\D9\86\D9\88\D8\A7\D8\A9] 4: \D8\A5\D8\B9\D8\AF\D8\A7\D8\AF PIC...\00", align 1
@str.const.15 = private unnamed_addr constant [22 x i8] c"  \D8\AA\D9\85 \D8\A5\D8\B9\D8\AF\D8\A7\D8\AF PIC\00", align 1
@str.const.17 = private unnamed_addr constant [47 x i8] c"[\D9\86\D9\88\D8\A7\D8\A9] 5: \D8\AA\D9\81\D8\B9\D9\8A\D9\84 \D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA...\00", align 1
@str.const.19 = private unnamed_addr constant [39 x i8] c"[\D9\86\D9\88\D8\A7\D8\A9] 6: \D8\AD\D9\84\D9\82\D8\A9 \D8\A7\D9\84\D9\86\D9\88\D8\A7\D8\A9...\00", align 1
@str.const.21 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.23 = private unnamed_addr constant [35 x i8] c"[\D9\86\D9\88\D8\A7\D8\A9] \D8\AF\D9\88\D8\B1\D8\A9 1 \E2\80\94 \D8\AE\D9\85\D9\88\D9\84\00", align 1
@str.const.25 = private unnamed_addr constant [30 x i8] c"  \D9\84\D9\88\D8\AD\D8\A9 \D8\A7\D9\84\D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD: \00", align 1
@int.fmt = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@fmt.d = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1
@str.const.26 = private unnamed_addr constant [35 x i8] c"[\D9\86\D9\88\D8\A7\D8\A9] \D8\AF\D9\88\D8\B1\D8\A9 2 \E2\80\94 \D8\AE\D9\85\D9\88\D9\84\00", align 1
@str.const.31 = private unnamed_addr constant [35 x i8] c"[\D9\86\D9\88\D8\A7\D8\A9] \D8\AF\D9\88\D8\B1\D8\A9 3 \E2\80\94 \D8\AE\D9\85\D9\88\D9\84\00", align 1
@str.const.36 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.40 = private unnamed_addr constant [47 x i8] c"    \D8\A7\D9\86\D8\AA\D9\87\D8\AA \D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9 Sad OS \D8\A8\D9\86\D8\AC\D8\A7\D8\AD!\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  call void asm sideeffect "cli", "~{memory}"()
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.9)
  br label %vga.loop

vga.loop:                                         ; preds = %vga.loop, %entry
  %i = phi i64 [ 0, %entry ], [ %8, %vga.loop ]
  %6 = mul i64 %i, 2
  %7 = add i64 753664, %6
  %vga.p = inttoptr i64 %7 to ptr
  store volatile i16 32, ptr %vga.p, align 2
  %8 = add i64 %i, 1
  %9 = icmp uge i64 %8, 2000
  br i1 %9, label %vga.done, label %vga.loop

vga.done:                                         ; preds = %vga.loop
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.11)
  store volatile i16 83, ptr inttoptr (i64 754044 to ptr), align 2
  store volatile i16 65, ptr inttoptr (i64 754046 to ptr), align 2
  store volatile i16 68, ptr inttoptr (i64 754048 to ptr), align 2
  store volatile i16 32, ptr inttoptr (i64 754050 to ptr), align 2
  store volatile i16 79, ptr inttoptr (i64 754052 to ptr), align 2
  store volatile i16 83, ptr inttoptr (i64 754054 to ptr), align 2
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.13)
  call void asm sideeffect "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}"(i8 0, i16 0)
  call void asm sideeffect "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}"(i8 0, i16 0)
  call void asm sideeffect "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}"(i8 0, i16 0)
  call void asm sideeffect "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}"(i8 0, i16 0)
  call void asm sideeffect "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}"(i8 0, i16 0)
  call void asm sideeffect "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}"(i8 0, i16 0)
  call void asm sideeffect "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}"(i8 0, i16 0)
  call void asm sideeffect "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}"(i8 0, i16 0)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.15)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.17)
  call void asm sideeffect "sti", "~{memory}"()
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.19)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.21)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.23)
  %port.read = call i8 asm sideeffect "inb $1, $0", "={ax},{dx},~{dirflag},~{fpsr},~{flags}"(i16 0)
  %port.read.ext = zext i8 %port.read to i64
  %alloca = alloca i64, align 8
  store i64 %port.read.ext, ptr %alloca, align 8
  %"%\D9\88\D8\B6\D8\B91.load" = load i64, ptr %alloca, align 8
  %strbuf = alloca i8, i64 32, align 1
  %17 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf, i64 32, ptr @int.fmt, i64 %"%\D9\88\D8\B6\D8\B91.load")
  %len1 = call i64 @strlen(ptr @str.const.25)
  %len2 = call i64 @strlen(ptr %strbuf)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %18 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.25, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %19 = call ptr @memcpy(ptr %dest2, ptr %strbuf, i64 %copylen2)
  %"%57.load" = load i8, ptr %concat.buf, align 1
  %20 = sext i8 %"%57.load" to i64
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %20)
  call void asm sideeffect "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}"(i8 0, i16 0)
  call void asm sideeffect "hlt", ""()
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.26)
  %port.read1 = call i8 asm sideeffect "inb $1, $0", "={ax},{dx},~{dirflag},~{fpsr},~{flags}"(i16 0)
  %port.read.ext2 = zext i8 %port.read1 to i64
  %alloca3 = alloca i64, align 8
  store i64 %port.read.ext2, ptr %alloca3, align 8
  %"%\D9\88\D8\B6\D8\B92.load" = load i64, ptr %alloca3, align 8
  %strbuf4 = alloca i8, i64 32, align 1
  %23 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf4, i64 32, ptr @int.fmt, i64 %"%\D9\88\D8\B6\D8\B92.load")
  %len15 = call i64 @strlen(ptr @str.const.25)
  %len26 = call i64 @strlen(ptr %strbuf4)
  %totallen7 = add i64 %len15, %len26
  %bufsize8 = add i64 %totallen7, 1
  %concat.buf9 = alloca i8, i64 %bufsize8, align 1
  %24 = call ptr @memcpy(ptr %concat.buf9, ptr @str.const.25, i64 %len15)
  %dest210 = getelementptr i8, ptr %concat.buf9, i64 %len15
  %copylen211 = add i64 %len26, 1
  %25 = call ptr @memcpy(ptr %dest210, ptr %strbuf4, i64 %copylen211)
  %"%64.load" = load i8, ptr %concat.buf9, align 1
  %26 = sext i8 %"%64.load" to i64
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %26)
  call void asm sideeffect "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}"(i8 0, i16 0)
  call void asm sideeffect "hlt", ""()
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.31)
  %port.read12 = call i8 asm sideeffect "inb $1, $0", "={ax},{dx},~{dirflag},~{fpsr},~{flags}"(i16 0)
  %port.read.ext13 = zext i8 %port.read12 to i64
  %alloca14 = alloca i64, align 8
  store i64 %port.read.ext13, ptr %alloca14, align 8
  %"%\D9\88\D8\B6\D8\B93.load" = load i64, ptr %alloca14, align 8
  %strbuf15 = alloca i8, i64 32, align 1
  %29 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf15, i64 32, ptr @int.fmt, i64 %"%\D9\88\D8\B6\D8\B93.load")
  %len116 = call i64 @strlen(ptr @str.const.25)
  %len217 = call i64 @strlen(ptr %strbuf15)
  %totallen18 = add i64 %len116, %len217
  %bufsize19 = add i64 %totallen18, 1
  %concat.buf20 = alloca i8, i64 %bufsize19, align 1
  %30 = call ptr @memcpy(ptr %concat.buf20, ptr @str.const.25, i64 %len116)
  %dest221 = getelementptr i8, ptr %concat.buf20, i64 %len116
  %copylen222 = add i64 %len217, 1
  %31 = call ptr @memcpy(ptr %dest221, ptr %strbuf15, i64 %copylen222)
  %"%71.load" = load i8, ptr %concat.buf20, align 1
  %32 = sext i8 %"%71.load" to i64
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %32)
  call void asm sideeffect "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}"(i8 0, i16 0)
  call void asm sideeffect "hlt", ""()
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.36)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.40)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  ret i64 0
}

declare i32 @printf(ptr, ...)

declare i32 @snprintf(ptr, i64, ptr, ...)

declare i64 @strlen(ptr)

declare ptr @memcpy(ptr, ptr, i64)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
