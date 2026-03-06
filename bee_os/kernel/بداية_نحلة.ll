; ModuleID = 'بداية_نحلة'
source_filename = "\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%SadArray = type { i64, i64, ptr }

@str.const = private unnamed_addr constant [53 x i8] c"[BeeOS] \D9\85\D8\B1\D8\AD\D9\84\D8\A9 1: Serial \D9\85\D9\87\D9\8A\D8\A3 (115200 baud)\0A\00", align 1
@str.const.1 = private unnamed_addr constant [41 x i8] c"[BeeOS] \D9\85\D8\B1\D8\AD\D9\84\D8\A9 2: \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 GDT...\0A\00", align 1
@str.const.2 = private unnamed_addr constant [40 x i8] c"[BeeOS] \D9\85\D8\B1\D8\AD\D9\84\D8\A9 2: \E2\9C\93 GDT \D8\AC\D8\A7\D9\87\D8\B2\0A\00", align 1
@str.const.3 = private unnamed_addr constant [65 x i8] c"[BeeOS] \D9\85\D8\B1\D8\AD\D9\84\D8\A9 3: \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA...\0A\00", align 1
@str.const.4 = private unnamed_addr constant [46 x i8] c"[BeeOS] \D9\85\D8\B1\D8\AD\D9\84\D8\A9 3: \E2\9C\93 IDT + PIC \D8\AC\D8\A7\D9\87\D8\B2\0A\00", align 1
@str.const.5 = private unnamed_addr constant [48 x i8] c"[BeeOS] \D9\85\D8\B1\D8\AD\D9\84\D8\A9 4: \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D8\A7\D9\84\D9\85\D8\A4\D9\82\D8\AA (\00", align 1
@str.const.6 = private unnamed_addr constant [9 x i8] c" Hz)...\0A\00", align 1
@str.const.7 = private unnamed_addr constant [46 x i8] c"[BeeOS] \D9\85\D8\B1\D8\AD\D9\84\D8\A9 4: \E2\9C\93 PIT Timer \D8\AC\D8\A7\D9\87\D8\B2\0A\00", align 1
@str.const.8 = private unnamed_addr constant [48 x i8] c"[BeeOS] \D9\85\D8\B1\D8\AD\D9\84\D8\A9 5: \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D8\A7\D9\84\D8\B4\D8\A7\D8\B4\D8\A9 (\00", align 1
@str.const.9 = private unnamed_addr constant [2 x i8] c"x\00", align 1
@str.const.10 = private unnamed_addr constant [6 x i8] c")...\0A\00", align 1
@str.const.11 = private unnamed_addr constant [51 x i8] c"[BeeOS] \D9\85\D8\B1\D8\AD\D9\84\D8\A9 5: \E2\9C\93 \D8\A7\D9\84\D8\B4\D8\A7\D8\B4\D8\A9 \D8\AC\D8\A7\D9\87\D8\B2\D8\A9\0A\00", align 1
@str.const.12 = private unnamed_addr constant [57 x i8] c"[BeeOS] \D9\85\D8\B1\D8\AD\D9\84\D8\A9 6: \D8\B9\D8\B1\D8\B6 \D8\B4\D8\A7\D8\B4\D8\A9 \D8\A7\D9\84\D8\AA\D8\B1\D8\AD\D9\8A\D8\A8...\0A\00", align 1
@str.const.13 = private unnamed_addr constant [64 x i8] c"[BeeOS] \D9\85\D8\B1\D8\AD\D9\84\D8\A9 6: \E2\9C\93 \D8\B4\D8\A7\D8\B4\D8\A9 \D8\A7\D9\84\D8\AA\D8\B1\D8\AD\D9\8A\D8\A8 \D9\85\D8\B9\D8\B1\D9\88\D8\B6\D8\A9\0A\00", align 1
@str.const.14 = private unnamed_addr constant [56 x i8] c"[BeeOS] \D9\85\D8\B1\D8\AD\D9\84\D8\A9 7: \D8\AA\D9\81\D8\B9\D9\8A\D9\84 \D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA...\0A\00", align 1
@str.const.15 = private unnamed_addr constant [67 x i8] c"[BeeOS] \D9\85\D8\B1\D8\AD\D9\84\D8\A9 7: \E2\9C\93 \D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA \D9\85\D9\8F\D9\81\D8\B9\D9\91\D9\84\D8\A9 (sti)\0A\00", align 1
@str.const.16 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.17 = private unnamed_addr constant [170 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\0A\00", align 1
@str.const.18 = private unnamed_addr constant [99 x i8] c"\E2\95\91        \D8\A8\D9\90\D8\B3\D9\92\D9\85\D9\90 \D8\A7\D9\84\D9\84\D9\87\D9\90 \D8\A7\D9\84\D8\B1\D9\8E\D9\91\D8\AD\D9\92\D9\85\D9\B0\D9\86\D9\90 \D8\A7\D9\84\D8\B1\D9\8E\D9\91\D8\AD\D9\90\D9\8A\D9\85\D9\90                \E2\95\91\0A\00", align 1
@str.const.19 = private unnamed_addr constant [62 x i8] c"\E2\95\91                                                      \E2\95\91\0A\00", align 1
@str.const.20 = private unnamed_addr constant [83 x i8] c"\E2\95\91     \F0\9F\90\9D  \D9\86\D9\90\D8\B8\D9\8E\D8\A7\D9\85\D9\8F \D8\A7\D9\84\D9\86\D9\8E\D9\91\D8\AD\D9\92\D9\84\D9\8E\D8\A9 \E2\80\94 BeeOS                   \E2\95\91\0A\00", align 1
@str.const.21 = private unnamed_addr constant [69 x i8] c"\E2\95\91     \D8\A7\D9\84\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1: 0.1.1 Alpha                             \E2\95\91\0A\00", align 1
@str.const.23 = private unnamed_addr constant [101 x i8] c"\E2\95\91     \D9\88\D9\8E\D8\A3\D9\8E\D9\88\D9\92\D8\AD\D9\8E\D9\89 \D8\B1\D9\8E\D8\A8\D9\8F\D9\91\D9\83\D9\8E \D8\A5\D9\90\D9\84\D9\8E\D9\89 \D8\A7\D9\84\D9\86\D9\8E\D9\91\D8\AD\D9\92\D9\84\D9\90 \E2\80\94 \D9\A1\D9\A6:\D9\A6\D9\A8           \E2\95\91\0A\00", align 1
@str.const.24 = private unnamed_addr constant [170 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\0A\00", align 1
@str.const.27 = private unnamed_addr constant [164 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\0A\00", align 1
@str.const.28 = private unnamed_addr constant [65 x i8] c"  \E2\9C\93 BeeOS v0.1.1 \D8\AC\D8\A7\D9\87\D8\B2 \E2\80\94 7 \D9\85\D8\B1\D8\A7\D8\AD\D9\84 \D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9! \F0\9F\90\9D\0A\00", align 1
@str.const.30 = private unnamed_addr constant [34 x i8] c"  \D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86\D8\A7\D8\AA \D8\A7\D9\84\D9\86\D8\B4\D8\B7\D8\A9:\0A\00", align 1
@str.const.31 = private unnamed_addr constant [46 x i8] c"    \E2\80\A2 Serial/UART   \E2\80\94 COM1 @ 115200 baud\0A\00", align 1
@str.const.32 = private unnamed_addr constant [45 x i8] c"    \E2\80\A2 GDT           \E2\80\94 64-bit flat model\0A\00", align 1
@str.const.33 = private unnamed_addr constant [53 x i8] c"    \E2\80\A2 IDT + PIC     \E2\80\94 256 entries, IRQ remapped\0A\00", align 1
@str.const.34 = private unnamed_addr constant [27 x i8] c"    \E2\80\A2 PIT Timer     \E2\80\94 \00", align 1
@str.const.35 = private unnamed_addr constant [5 x i8] c" Hz\0A\00", align 1
@str.const.36 = private unnamed_addr constant [27 x i8] c"    \E2\80\A2 Framebuffer   \E2\80\94 \00", align 1
@str.const.42 = private unnamed_addr constant [54 x i8] c"[BeeOS] \D8\AF\D8\AE\D9\88\D9\84 \D8\AD\D9\84\D9\82\D8\A9 \D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84 (HLT loop)...\0A\00", align 1

declare void @sad_ll_fb_init(i64, i64, i64)

declare void @sad_ll_fb_set_pixel(i64, i64, i64)

declare void @sad_ll_fb_fill_rect(i64, i64, i64, i64, i64)

declare void @sad_ll_fb_clear(i64)

declare void @sad_ll_serial_init(i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_gdt_init()

declare void @sad_ll_gdt_load()

declare void @sad_ll_idt_init()

declare void @sad_ll_idt_load()

declare void @sad_ll_pic_remap(i64, i64)

declare void @sad_ll_timer_init(i64)

declare void @sad_ll_interrupt_enable()

declare void @sad_ll_interrupt_disable()

define void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_0

while_body_1:                                     ; preds = %while_cond_0
  %"%\D8\B7.load2" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load2", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_0

while_cond_0:                                     ; preds = %while_body_1, %entry
  %arr.ptr.int = load i64, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 0
  %arr.len = load i64, ptr %arr.len.gep, align 8
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", %arr.len
  br i1 %cmplttmp, label %while_body_1, label %while_exit_2

while_exit_2:                                     ; preds = %while_cond_0
  ret void
}

define void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85" = alloca i64, align 8
  %"\D9\82\D9\8A\D9\85\D8\A91" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_3, label %merge_4

merge_4:                                          ; preds = %entry
  %"%\D9\82\D9\8A\D9\85\D8\A9.load2" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load2", 0
  br i1 %cmplttmp, label %then_5, label %merge_6

merge_6:                                          ; preds = %then_5, %merge_4
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 20, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 20, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 20))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep3 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data4 = load ptr, ptr %arr.data.gep3, align 8
  %arr.elem = getelementptr i64, ptr %arr.data4, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data.gep5 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data6 = load ptr, ptr %arr.data.gep5, align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 1
  store i64 0, ptr %arr.elem7, align 8
  %arr.data.gep8 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data9 = load ptr, ptr %arr.data.gep8, align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 2
  store i64 0, ptr %arr.elem10, align 8
  %arr.data.gep11 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data12 = load ptr, ptr %arr.data.gep11, align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 3
  store i64 0, ptr %arr.elem13, align 8
  %arr.data.gep14 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data15 = load ptr, ptr %arr.data.gep14, align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 4
  store i64 0, ptr %arr.elem16, align 8
  %arr.data.gep17 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data18 = load ptr, ptr %arr.data.gep17, align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 5
  store i64 0, ptr %arr.elem19, align 8
  %arr.data.gep20 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data21 = load ptr, ptr %arr.data.gep20, align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 6
  store i64 0, ptr %arr.elem22, align 8
  %arr.data.gep23 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data24 = load ptr, ptr %arr.data.gep23, align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 7
  store i64 0, ptr %arr.elem25, align 8
  %arr.data.gep26 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data27 = load ptr, ptr %arr.data.gep26, align 8
  %arr.elem28 = getelementptr i64, ptr %arr.data27, i64 8
  store i64 0, ptr %arr.elem28, align 8
  %arr.data.gep29 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data30 = load ptr, ptr %arr.data.gep29, align 8
  %arr.elem31 = getelementptr i64, ptr %arr.data30, i64 9
  store i64 0, ptr %arr.elem31, align 8
  %arr.data.gep32 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data33 = load ptr, ptr %arr.data.gep32, align 8
  %arr.elem34 = getelementptr i64, ptr %arr.data33, i64 10
  store i64 0, ptr %arr.elem34, align 8
  %arr.data.gep35 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data36 = load ptr, ptr %arr.data.gep35, align 8
  %arr.elem37 = getelementptr i64, ptr %arr.data36, i64 11
  store i64 0, ptr %arr.elem37, align 8
  %arr.data.gep38 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data39 = load ptr, ptr %arr.data.gep38, align 8
  %arr.elem40 = getelementptr i64, ptr %arr.data39, i64 12
  store i64 0, ptr %arr.elem40, align 8
  %arr.data.gep41 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data42 = load ptr, ptr %arr.data.gep41, align 8
  %arr.elem43 = getelementptr i64, ptr %arr.data42, i64 13
  store i64 0, ptr %arr.elem43, align 8
  %arr.data.gep44 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data45 = load ptr, ptr %arr.data.gep44, align 8
  %arr.elem46 = getelementptr i64, ptr %arr.data45, i64 14
  store i64 0, ptr %arr.elem46, align 8
  %arr.data.gep47 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data48 = load ptr, ptr %arr.data.gep47, align 8
  %arr.elem49 = getelementptr i64, ptr %arr.data48, i64 15
  store i64 0, ptr %arr.elem49, align 8
  %arr.data.gep50 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data51 = load ptr, ptr %arr.data.gep50, align 8
  %arr.elem52 = getelementptr i64, ptr %arr.data51, i64 16
  store i64 0, ptr %arr.elem52, align 8
  %arr.data.gep53 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data54 = load ptr, ptr %arr.data.gep53, align 8
  %arr.elem55 = getelementptr i64, ptr %arr.data54, i64 17
  store i64 0, ptr %arr.elem55, align 8
  %arr.data.gep56 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data57 = load ptr, ptr %arr.data.gep56, align 8
  %arr.elem58 = getelementptr i64, ptr %arr.data57, i64 18
  store i64 0, ptr %arr.elem58, align 8
  %arr.data.gep59 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data60 = load ptr, ptr %arr.data.gep59, align 8
  %arr.elem61 = getelementptr i64, ptr %arr.data60, i64 19
  store i64 0, ptr %arr.elem61, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  br label %while_cond_7

then_3:                                           ; preds = %entry
  ret void

then_5:                                           ; preds = %merge_4
  br label %merge_6

while_body_11:                                    ; preds = %while_cond_10
  %"%\D8\B7.load70" = load i64, ptr %"%\D8\B7", align 8
  %subtmp71 = sub i64 %"%\D8\B7.load70", 1
  store i64 %subtmp71, ptr %"%\D8\B7", align 8
  br label %while_cond_10

while_body_8:                                     ; preds = %while_cond_7
  %"%\D9\82\D9\8A\D9\85\D8\A9.load63" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %modtmp = srem i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load63", 10
  %addtmp = add i64 %modtmp, 48
  %"%\D8\B9\D8\AF\D8\AF.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %arr.ptr.int = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.data.gep64 = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 2
  %arr.data65 = load ptr, ptr %arr.data.gep64, align 8
  %arr.elem66 = getelementptr i64, ptr %arr.data65, i64 %"%\D8\B9\D8\AF\D8\AF.load"
  store i64 %addtmp, ptr %arr.elem66, align 8
  %"%\D8\B9\D8\AF\D8\AF.load67" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %addtmp68 = add i64 %"%\D8\B9\D8\AF\D8\AF.load67", 1
  store i64 %addtmp68, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  br label %while_cond_7

while_cond_10:                                    ; preds = %while_body_11, %while_exit_9
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B7.load", 0
  br i1 %cmpgetmp, label %while_body_11, label %while_exit_12

while_cond_7:                                     ; preds = %while_body_8, %merge_6
  %"%\D9\82\D9\8A\D9\85\D8\A9.load62" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load62", 0
  br i1 %cmpgttmp, label %while_body_8, label %while_exit_9

while_exit_12:                                    ; preds = %while_cond_10
  ret void

while_exit_9:                                     ; preds = %while_cond_7
  %"%\D8\B9\D8\AF\D8\AF.load69" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %subtmp = sub i64 %"%\D8\B9\D8\AF\D8\AF.load69", 1
  store i64 %subtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_10
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\A5\D8\B7\D8\A7\D8\B1"(i64 %"\D8\B3", i64 %"\D8\B5", i64 %"\D8\B9\D8\B1\D8\B6", i64 %"\D8\A7\D8\B1\D8\AA\D9\81\D8\A7\D8\B9", i64 %"\D9\84\D9\88\D9\86") {
entry:
  %"\D9\84\D9\88\D9\865" = alloca i64, align 8
  store i64 %"\D9\84\D9\88\D9\86", ptr %"\D9\84\D9\88\D9\865", align 8
  %"\D8\A7\D8\B1\D8\AA\D9\81\D8\A7\D8\B94" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B1\D8\AA\D9\81\D8\A7\D8\B9", ptr %"\D8\A7\D8\B1\D8\AA\D9\81\D8\A7\D8\B94", align 8
  %"\D8\B9\D8\B1\D8\B63" = alloca i64, align 8
  store i64 %"\D8\B9\D8\B1\D8\B6", ptr %"\D8\B9\D8\B1\D8\B63", align 8
  %"\D8\B52" = alloca i64, align 8
  store i64 %"\D8\B5", ptr %"\D8\B52", align 8
  %"\D8\B31" = alloca i64, align 8
  store i64 %"\D8\B3", ptr %"\D8\B31", align 8
  ret void
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\B4\D8\B9\D8\A7\D8\B1_\D9\86\D8\AD\D9\84\D8\A9"(i64 %"\D9\85\D8\B1\D9\83\D8\B2_\D8\B3", i64 %"\D9\85\D8\B1\D9\83\D8\B2_\D8\B5") {
entry:
  %"\D9\85\D8\B1\D9\83\D8\B2_\D8\B52" = alloca i64, align 8
  store i64 %"\D9\85\D8\B1\D9\83\D8\B2_\D8\B5", ptr %"\D9\85\D8\B1\D9\83\D8\B2_\D8\B52", align 8
  %"\D9\85\D8\B1\D9\83\D8\B2_\D8\B31" = alloca i64, align 8
  store i64 %"\D9\85\D8\B1\D9\83\D8\B2_\D8\B3", ptr %"\D9\85\D8\B1\D9\83\D8\B2_\D8\B31", align 8
  ret void
}

define void @"\D9\85\D8\B1\D8\AD\D9\84\D8\A9_1_serial"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  ret void
}

define void @"\D9\85\D8\B1\D8\AD\D9\84\D8\A9_2_gdt"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @sad_ll_gdt_init()
  call void @sad_ll_gdt_load()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  ret void
}

define void @"\D9\85\D8\B1\D8\AD\D9\84\D8\A9_3_\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  call void @sad_ll_interrupt_disable()
  call void @sad_ll_idt_init()
  call void @sad_ll_pic_remap(i64 32, i64 40)
  call void @sad_ll_idt_load()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  ret void
}

define void @"\D9\85\D8\B1\D8\AD\D9\84\D8\A9_4_\D9\85\D8\A4\D9\82\D8\AA"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  ret void
}

define void @"\D9\85\D8\B1\D8\AD\D9\84\D8\A9_5_\D8\B4\D8\A7\D8\B4\D8\A9"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  ret void
}

define void @"\D9\85\D8\B1\D8\AD\D9\84\D8\A9_6_\D8\AA\D8\B1\D8\AD\D9\8A\D8\A8"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\B4\D8\B9\D8\A7\D8\B1_\D9\86\D8\AD\D9\84\D8\A9"(i64 0, i64 0)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  ret void
}

define void @"\D9\85\D8\B1\D8\AD\D9\84\D8\A9_7_\D8\AA\D9\81\D8\B9\D9\8A\D9\84_\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  call void @sad_ll_interrupt_enable()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  ret void
}

define void @"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\A7\D9\84\D9\86\D9\88\D8\A7\D8\A9"() {
entry:
  call void @"\D9\85\D8\B1\D8\AD\D9\84\D8\A9_1_serial"()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  call void @"\D9\85\D8\B1\D8\AD\D9\84\D8\A9_2_gdt"()
  call void @"\D9\85\D8\B1\D8\AD\D9\84\D8\A9_3_\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA"()
  call void @"\D9\85\D8\B1\D8\AD\D9\84\D8\A9_4_\D9\85\D8\A4\D9\82\D8\AA"()
  call void @"\D9\85\D8\B1\D8\AD\D9\84\D8\A9_5_\D8\B4\D8\A7\D8\B4\D8\A9"()
  call void @"\D9\85\D8\B1\D8\AD\D9\84\D8\A9_6_\D8\AA\D8\B1\D8\AD\D9\8A\D8\A8"()
  call void @"\D9\85\D8\B1\D8\AD\D9\84\D8\A9_7_\D8\AA\D9\81\D8\B9\D9\8A\D9\84_\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA"()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.30 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.32 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.34 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.35 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.36 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.42 to i64))
  br label %while_cond_13

while_body_14:                                    ; preds = %while_cond_13
  br label %while_cond_13

while_cond_13:                                    ; preds = %while_body_14, %entry
  br i1 true, label %while_body_14, label %while_exit_15

while_exit_15:                                    ; preds = %while_cond_13
  ret void
}

define void @__sad_main() {
entry:
  %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\A4\D9\82\D8\AA" = alloca i64, align 8
  %"%\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1_\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD" = alloca i64, align 8
  %"%\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1_\D9\81\D8\B1\D8\B9\D9\8A" = alloca i64, align 8
  %"%\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D8\B1\D9\85\D8\A7\D8\AF\D9\8A" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D8\A3\D8\B2\D8\B1\D9\82" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D8\A3\D8\AD\D9\85\D8\B1" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D8\B0\D9\87\D8\A8\D9\8A" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D8\A3\D8\AE\D8\B6\D8\B1" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D8\A3\D8\A8\D9\8A\D8\B6" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D8\A3\D8\B3\D9\88\D8\AF" = alloca i64, align 8
  %"%\D8\B4\D8\A7\D8\B4\D8\A9_\D8\B9\D9\85\D9\82" = alloca i64, align 8
  %"%\D8\B4\D8\A7\D8\B4\D8\A9_\D8\A7\D8\B1\D8\AA\D9\81\D8\A7\D8\B9" = alloca i64, align 8
  %"%\D8\B4\D8\A7\D8\B4\D8\A9_\D8\B9\D8\B1\D8\B6" = alloca i64, align 8
  store i64 1024, ptr %"%\D8\B4\D8\A7\D8\B4\D8\A9_\D8\B9\D8\B1\D8\B6", align 8
  store i64 768, ptr %"%\D8\B4\D8\A7\D8\B4\D8\A9_\D8\A7\D8\B1\D8\AA\D9\81\D8\A7\D8\B9", align 8
  store i64 32, ptr %"%\D8\B4\D8\A7\D8\B4\D8\A9_\D8\B9\D9\85\D9\82", align 8
  store i64 0, ptr %"%\D9\84\D9\88\D9\86_\D8\A3\D8\B3\D9\88\D8\AF", align 8
  store i64 16777215, ptr %"%\D9\84\D9\88\D9\86_\D8\A3\D8\A8\D9\8A\D8\B6", align 8
  store i64 43520, ptr %"%\D9\84\D9\88\D9\86_\D8\A3\D8\AE\D8\B6\D8\B1", align 8
  store i64 16766720, ptr %"%\D9\84\D9\88\D9\86_\D8\B0\D9\87\D8\A8\D9\8A", align 8
  store i64 16711680, ptr %"%\D9\84\D9\88\D9\86_\D8\A3\D8\AD\D9\85\D8\B1", align 8
  store i64 17578, ptr %"%\D9\84\D9\88\D9\86_\D8\A3\D8\B2\D8\B1\D9\82", align 8
  store i64 3355443, ptr %"%\D9\84\D9\88\D9\86_\D8\B1\D9\85\D8\A7\D8\AF\D9\8A", align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 0, ptr %"%\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A", align 8
  store i64 1, ptr %"%\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1_\D9\81\D8\B1\D8\B9\D9\8A", align 8
  store i64 1, ptr %"%\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1_\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD", align 8
  store i64 100, ptr %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\A4\D9\82\D8\AA", align 8
  call void @"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\A7\D9\84\D9\86\D9\88\D8\A7\D8\A9"()
  ret void
}

declare ptr @malloc(i64)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
