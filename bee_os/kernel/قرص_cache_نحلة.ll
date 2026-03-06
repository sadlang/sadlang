; ModuleID = 'قرص_cache_نحلة'
source_filename = "\D9\82\D8\B1\D8\B5_cache_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%SadArray = type { i64, i64, ptr }

declare void @sad_ll_mem_read64(i64)

declare void @sad_ll_mem_write64(i64, i64)

declare void @sad_ll_mem_read32(i64)

declare void @sad_ll_mem_write32(i64, i64)

declare void @sad_ll_memset(i64, i64, i64)

declare void @sad_ll_memcpy(i64, i64, i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_interrupt_disable()

declare void @sad_ll_interrupt_enable()

declare void @sad_ll_page_alloc()

declare void @sad_ll_page_free(i64)

declare void @sad_ll_pages_alloc(i64)

declare void @sad_ll_pages_free(i64, i64)

declare void @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\83\D8\AA\D9\84\D8\A9"(i64, i64, i64, i64)

declare void @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\83\D8\AA\D9\84\D8\A9"(i64, i64, i64, i64)

define void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmplttmp = icmp slt i64 %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load", 0
  br i1 %cmplttmp, label %then_0, label %merge_1

merge_1:                                          ; preds = %entry
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load3" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load3", 10
  br i1 %cmpgetmp, label %then_2, label %merge_3

merge_3:                                          ; preds = %then_2, %merge_1
  ret void

then_0:                                           ; preds = %entry
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load2" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %subtmp = sub i64 0, %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load2"
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %subtmp)
  ret void

then_2:                                           ; preds = %merge_1
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load4" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %divtmp = sdiv i64 %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load4", 10
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %divtmp)
  br label %merge_3
}

define i64 @"\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D9\83\D8\A7\D8\B4"() {
entry:
  %"%\D9\83" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  br i1 false, label %then_4, label %merge_5

merge_5:                                          ; preds = %entry
  call void @sad_ll_pages_alloc(i64 8)
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load", 0
  br i1 %cmpeqtmp, label %then_6, label %merge_7

merge_7:                                          ; preds = %merge_5
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load1" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  call void @sad_ll_memset(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load1", i64 0, i64 32768)
  store i64 0, ptr %"%\D9\83", align 8
  br label %while_cond_8

then_4:                                           ; preds = %entry
  ret i64 0

then_6:                                           ; preds = %merge_5
  ret i64 -1

while_body_9:                                     ; preds = %while_cond_8
  %"%\D9\83.load2" = load i64, ptr %"%\D9\83", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\83.load2"
  store i64 0, ptr %arr.elem, align 8
  %"%\D9\83.load3" = load i64, ptr %"%\D9\83", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D9\83.load3"
  store i64 0, ptr %arr.elem5, align 8
  %"%\D9\83.load6" = load i64, ptr %"%\D9\83", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D9\83.load6"
  store i64 0, ptr %arr.elem8, align 8
  %"%\D9\83.load9" = load i64, ptr %"%\D9\83", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D9\83.load9"
  store i64 0, ptr %arr.elem11, align 8
  %"%\D9\83.load12" = load i64, ptr %"%\D9\83", align 8
  %shltmp = shl i64 %"%\D9\83.load12", 9
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load13" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load13", %shltmp
  %"%\D9\83.load14" = load i64, ptr %"%\D9\83", align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 %"%\D9\83.load14"
  store i64 %addtmp, ptr %arr.elem16, align 8
  %"%\D9\83.load17" = load i64, ptr %"%\D9\83", align 8
  %addtmp18 = add i64 %"%\D9\83.load17", 1
  store i64 %addtmp18, ptr %"%\D9\83", align 8
  br label %while_cond_8

while_cond_8:                                     ; preds = %while_body_9, %merge_7
  %"%\D9\83.load" = load i64, ptr %"%\D9\83", align 8
  %cmplttmp = icmp slt i64 %"%\D9\83.load", 64
  br i1 %cmplttmp, label %while_body_9, label %while_exit_10

while_exit_10:                                    ; preds = %while_cond_8
  ret i64 0
}

define i64 @"cache_\D8\A7\D8\A8\D8\AD\D8\AB"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9") {
entry:
  %"%\D9\83" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  store i64 0, ptr %"%\D9\83", align 8
  br label %while_cond_11

merge_15:                                         ; preds = %merge_17, %while_body_12
  %"%\D9\83.load8" = load i64, ptr %"%\D9\83", align 8
  %addtmp = add i64 %"%\D9\83.load8", 1
  store i64 %addtmp, ptr %"%\D9\83", align 8
  br label %while_cond_11

merge_17:                                         ; preds = %merge_19, %then_14
  br label %merge_15

merge_19:                                         ; preds = %then_16
  br label %merge_17

then_14:                                          ; preds = %while_body_12
  %"%\D9\83.load4" = load i64, ptr %"%\D9\83", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D9\83.load4"
  %arr.get7 = load i64, ptr %arr.elem6, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmpeqtmp = icmp eq i64 %arr.get7, %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load"
  br i1 %cmpeqtmp, label %then_16, label %merge_17

then_16:                                          ; preds = %then_14
  %"%\D9\83.load9" = load i64, ptr %"%\D9\83", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D9\83.load9"
  %arr.get12 = load i64, ptr %arr.elem11, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %cmpeqtmp13 = icmp eq i64 %arr.get12, %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load"
  br i1 %cmpeqtmp13, label %then_18, label %merge_19

then_18:                                          ; preds = %then_16
  %"%\D9\83.load14" = load i64, ptr %"%\D9\83", align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 %"%\D9\83.load14"
  store i64 0, ptr %arr.elem16, align 8
  %loadtmp = load i64, ptr %"%\D9\83", align 8
  ret i64 %loadtmp

while_body_12:                                    ; preds = %while_cond_11
  %"%\D9\83.load3" = load i64, ptr %"%\D9\83", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\83.load3"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp, label %then_14, label %merge_15

while_cond_11:                                    ; preds = %merge_15, %entry
  %"%\D9\83.load" = load i64, ptr %"%\D9\83", align 8
  %cmplttmp = icmp slt i64 %"%\D9\83.load", 64
  br i1 %cmplttmp, label %while_body_12, label %while_exit_13

while_exit_13:                                    ; preds = %while_cond_11
  ret i64 -1
}

define i64 @"cache_\D8\B3\D8\B7\D8\B1_\D9\81\D8\A7\D8\B1\D8\BA"() {
entry:
  %"%\D9\83" = alloca i64, align 8
  store i64 0, ptr %"%\D9\83", align 8
  br label %while_cond_20

merge_24:                                         ; preds = %while_body_21
  %"%\D9\83.load2" = load i64, ptr %"%\D9\83", align 8
  %addtmp = add i64 %"%\D9\83.load2", 1
  store i64 %addtmp, ptr %"%\D9\83", align 8
  br label %while_cond_20

then_23:                                          ; preds = %while_body_21
  %loadtmp = load i64, ptr %"%\D9\83", align 8
  ret i64 %loadtmp

while_body_21:                                    ; preds = %while_cond_20
  %"%\D9\83.load1" = load i64, ptr %"%\D9\83", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\83.load1"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_23, label %merge_24

while_cond_20:                                    ; preds = %merge_24, %entry
  %"%\D9\83.load" = load i64, ptr %"%\D9\83", align 8
  %cmplttmp = icmp slt i64 %"%\D9\83.load", 64
  br i1 %cmplttmp, label %while_body_21, label %while_exit_22

while_exit_22:                                    ; preds = %while_cond_20
  ret i64 -1
}

define i64 @"cache_\D8\B3\D8\B7\D8\B1_LRU"() {
entry:
  %"%\D9\83" = alloca i64, align 8
  %"%\D8\A3\D9\82\D9\84_\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"%\D8\A3\D9\82\D9\84_\D8\A7\D8\B3\D8\AA\D8\AE\D8\AF\D8\A7\D9\85" = alloca i64, align 8
  %"%\D9\81\D8\A7\D8\B1\D8\BA" = alloca i64, align 8
  %calltmp = call i64 @"cache_\D8\B3\D8\B7\D8\B1_\D9\81\D8\A7\D8\B1\D8\BA"()
  store i64 %calltmp, ptr %"%\D9\81\D8\A7\D8\B1\D8\BA", align 8
  %"%\D9\81\D8\A7\D8\B1\D8\BA.load" = load i64, ptr %"%\D9\81\D8\A7\D8\B1\D8\BA", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D8\A7\D8\B1\D8\BA.load", 0
  br i1 %cmpgetmp, label %then_25, label %merge_26

merge_26:                                         ; preds = %entry
  store i64 1, ptr %"%\D8\A3\D9\82\D9\84_\D8\A7\D8\B3\D8\AA\D8\AE\D8\AF\D8\A7\D9\85", align 8
  store i64 0, ptr %"%\D8\A3\D9\82\D9\84_\D9\81\D9\87\D8\B1\D8\B3", align 8
  store i64 0, ptr %"%\D9\83", align 8
  br label %while_cond_27

merge_31:                                         ; preds = %merge_33, %while_body_28
  %"%\D9\83.load8" = load i64, ptr %"%\D9\83", align 8
  %addtmp = add i64 %"%\D9\83.load8", 1
  store i64 %addtmp, ptr %"%\D9\83", align 8
  br label %while_cond_27

merge_33:                                         ; preds = %then_32, %then_30
  br label %merge_31

then_25:                                          ; preds = %entry
  %loadtmp = load i64, ptr %"%\D9\81\D8\A7\D8\B1\D8\BA", align 8
  ret i64 %loadtmp

then_30:                                          ; preds = %while_body_28
  %"%\D9\83.load3" = load i64, ptr %"%\D9\83", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D9\83.load3"
  %arr.get6 = load i64, ptr %arr.elem5, align 8
  %"%\D8\A3\D9\82\D9\84_\D8\A7\D8\B3\D8\AA\D8\AE\D8\AF\D8\A7\D9\85.load" = load i64, ptr %"%\D8\A3\D9\82\D9\84_\D8\A7\D8\B3\D8\AA\D8\AE\D8\AF\D8\A7\D9\85", align 8
  %cmplttmp7 = icmp slt i64 %arr.get6, %"%\D8\A3\D9\82\D9\84_\D8\A7\D8\B3\D8\AA\D8\AE\D8\AF\D8\A7\D9\85.load"
  br i1 %cmplttmp7, label %then_32, label %merge_33

then_32:                                          ; preds = %then_30
  %"%\D9\83.load9" = load i64, ptr %"%\D9\83", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D9\83.load9"
  %arr.get12 = load i64, ptr %arr.elem11, align 8
  store i64 %arr.get12, ptr %"%\D8\A3\D9\82\D9\84_\D8\A7\D8\B3\D8\AA\D8\AE\D8\AF\D8\A7\D9\85", align 8
  %"%\D9\83.load13" = load i64, ptr %"%\D9\83", align 8
  store i64 %"%\D9\83.load13", ptr %"%\D8\A3\D9\82\D9\84_\D9\81\D9\87\D8\B1\D8\B3", align 8
  br label %merge_33

while_body_28:                                    ; preds = %while_cond_27
  %"%\D9\83.load1" = load i64, ptr %"%\D9\83", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\83.load1"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp, label %then_30, label %merge_31

while_cond_27:                                    ; preds = %merge_31, %merge_26
  %"%\D9\83.load" = load i64, ptr %"%\D9\83", align 8
  %cmplttmp = icmp slt i64 %"%\D9\83.load", 64
  br i1 %cmplttmp, label %while_body_28, label %while_exit_29

while_exit_29:                                    ; preds = %while_cond_27
  %loadtmp2 = load i64, ptr %"%\D8\A3\D9\82\D9\84_\D9\81\D9\87\D8\B1\D8\B3", align 8
  ret i64 %loadtmp2
}

define i64 @"cache_\D8\A3\D8\AE\D9\84_\D8\B3\D8\B7\D8\B1"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B11" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B11", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B11", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load", 0
  br i1 %cmplttmp, label %then_34, label %merge_35

merge_35:                                         ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load2" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B11", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load2", 64
  br i1 %cmpgetmp, label %then_36, label %merge_37

merge_37:                                         ; preds = %merge_35
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load3" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B11", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load3"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_38, label %merge_39

merge_39:                                         ; preds = %merge_37
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load4" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B11", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load4"
  %arr.get7 = load i64, ptr %arr.elem6, align 8
  %cmpeqtmp8 = icmp eq i64 %arr.get7, 0
  br i1 %cmpeqtmp8, label %then_40, label %merge_41

merge_41:                                         ; preds = %merge_39
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load9" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B11", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load9"
  %arr.get12 = load i64, ptr %arr.elem11, align 8
  %cmpeqtmp13 = icmp eq i64 %arr.get12, 0
  br i1 %cmpeqtmp13, label %then_42, label %merge_43

merge_43:                                         ; preds = %merge_45, %merge_41
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load27" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B11", align 8
  %arr.data28 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem29 = getelementptr i64, ptr %arr.data28, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load27"
  store i64 0, ptr %arr.elem29, align 8
  ret i64 0

merge_45:                                         ; preds = %then_42
  br label %merge_43

then_34:                                          ; preds = %entry
  ret i64 -1

then_36:                                          ; preds = %merge_35
  ret i64 -1

then_38:                                          ; preds = %merge_37
  ret i64 0

then_40:                                          ; preds = %merge_39
  ret i64 -1

then_42:                                          ; preds = %merge_41
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load14" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B11", align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load14"
  %arr.get17 = load i64, ptr %arr.elem16, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load18" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B11", align 8
  %arr.data19 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load18"
  %arr.get21 = load i64, ptr %arr.elem20, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load22" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B11", align 8
  %arr.data23 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem24 = getelementptr i64, ptr %arr.data23, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\B7\D8\B1.load22"
  %arr.get25 = load i64, ptr %arr.elem24, align 8
  call void @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\83\D8\AA\D9\84\D8\A9"(i64 %arr.get17, i64 %arr.get21, i64 1, i64 %arr.get25)
  store i64 0, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpeqtmp26 = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpeqtmp26, label %then_44, label %merge_45

then_44:                                          ; preds = %then_42
  ret i64 -1
}

define i64 @"cache_\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D8\A5\D8\AE\D9\84\D8\A7\D8\A1" = alloca i64, align 8
  %"%\D8\B3\D8\B7\D8\B1" = alloca i64, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A93" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A93", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  br i1 true, label %then_46, label %merge_47

merge_47:                                         ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %calltmp = call i64 @"cache_\D8\A7\D8\A8\D8\AD\D8\AB"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load5")
  store i64 %calltmp, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 0
  br i1 %cmpgetmp, label %then_50, label %merge_51

merge_49:                                         ; preds = %then_46
  ret i64 -1

merge_51:                                         ; preds = %merge_47
  %calltmp6 = call i64 @"cache_\D8\B3\D8\B7\D8\B1_LRU"()
  store i64 %calltmp6, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %"%\D8\B3\D8\B7\D8\B1.load" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B3\D8\B7\D8\B1.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp, label %then_52, label %merge_53

merge_53:                                         ; preds = %merge_55, %merge_51
  %"%\D8\B3\D8\B7\D8\B1.load10" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 %"%\D8\B3\D8\B7\D8\B1.load10"
  %arr.get13 = load i64, ptr %arr.elem12, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load14" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load15" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  call void @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load14", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load15", i64 1, i64 %arr.get13)
  store i64 0, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_58, label %merge_59

merge_55:                                         ; preds = %then_52
  br label %merge_53

merge_57:                                         ; preds = %then_54
  ret i64 -1

merge_59:                                         ; preds = %merge_53
  %"%\D8\B3\D8\B7\D8\B1.load20" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %arr.data21 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 %"%\D8\B3\D8\B7\D8\B1.load20"
  store i64 0, ptr %arr.elem22, align 8
  %"%\D8\B3\D8\B7\D8\B1.load23" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load24" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data25 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem26 = getelementptr i64, ptr %arr.data25, i64 %"%\D8\B3\D8\B7\D8\B1.load23"
  store i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load24", ptr %arr.elem26, align 8
  %"%\D8\B3\D8\B7\D8\B1.load27" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load28" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %arr.data29 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem30 = getelementptr i64, ptr %arr.data29, i64 %"%\D8\B3\D8\B7\D8\B1.load27"
  store i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load28", ptr %arr.elem30, align 8
  %"%\D8\B3\D8\B7\D8\B1.load31" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %arr.data32 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem33 = getelementptr i64, ptr %arr.data32, i64 %"%\D8\B3\D8\B7\D8\B1.load31"
  store i64 0, ptr %arr.elem33, align 8
  ret i64 0

then_46:                                          ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A93", align 8
  call void @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load", i64 1, i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load")
  store i64 0, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpgttmp, label %then_48, label %merge_49

then_48:                                          ; preds = %then_46
  ret i64 0

then_50:                                          ; preds = %merge_47
  ret i64 0

then_52:                                          ; preds = %merge_51
  %"%\D8\B3\D8\B7\D8\B1.load7" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %calltmp8 = call i64 @"cache_\D8\A3\D8\AE\D9\84_\D8\B3\D8\B7\D8\B1"(i64 %"%\D8\B3\D8\B7\D8\B1.load7")
  store i64 %calltmp8, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D8\A5\D8\AE\D9\84\D8\A7\D8\A1", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D8\A5\D8\AE\D9\84\D8\A7\D8\A1.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D8\A5\D8\AE\D9\84\D8\A7\D8\A1", align 8
  %cmpnetmp9 = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D8\A5\D8\AE\D9\84\D8\A7\D8\A1.load", 0
  br i1 %cmpnetmp9, label %then_54, label %merge_55

then_54:                                          ; preds = %then_52
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load16" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load17" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load18" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A93", align 8
  call void @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load16", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load17", i64 1, i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load18")
  store i64 0, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9", align 8
  %cmpgttmp19 = icmp sgt i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9.load", 0
  br i1 %cmpgttmp19, label %then_56, label %merge_57

then_56:                                          ; preds = %then_54
  ret i64 0

then_58:                                          ; preds = %merge_53
  ret i64 -1
}

define i64 @"cache_\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A94" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A94", align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  store i64 0, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  store i64 0, ptr %"%\D9\82", align 8
  br label %while_cond_60

merge_64:                                         ; preds = %while_body_61
  %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load" = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  %addtmp9 = add i64 %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load", 1
  store i64 %addtmp9, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  %"%\D9\82.load10" = load i64, ptr %"%\D9\82", align 8
  %addtmp11 = add i64 %"%\D9\82.load10", 1
  store i64 %addtmp11, ptr %"%\D9\82", align 8
  br label %while_cond_60

then_63:                                          ; preds = %while_body_61
  %loadtmp8 = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  ret i64 %loadtmp8

while_body_61:                                    ; preds = %while_cond_60
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"%\D9\82.load5" = load i64, ptr %"%\D9\82", align 8
  %addtmp = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load", %"%\D9\82.load5"
  %"%\D9\82.load6" = load i64, ptr %"%\D9\82", align 8
  %multmp = mul i64 %"%\D9\82.load6", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A94", align 8
  %addtmp7 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load", %multmp
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp = call i64 @"cache_\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", i64 %addtmp, i64 %addtmp7)
  store i64 %calltmp, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_63, label %merge_64

while_cond_60:                                    ; preds = %merge_64, %entry
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82.load", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load"
  br i1 %cmplttmp, label %while_body_61, label %while_exit_62

while_exit_62:                                    ; preds = %while_cond_60
  %loadtmp = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  ret i64 %loadtmp
}

define i64 @"cache_\D8\A7\D9\83\D8\AA\D8\A8_\D9\82\D8\B7\D8\A7\D8\B9"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1") {
entry:
  %"%\D8\B3\D8\B7\D8\B1" = alloca i64, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B13" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B13", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  br i1 true, label %then_65, label %merge_66

merge_66:                                         ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %calltmp = call i64 @"cache_\D8\A7\D8\A8\D8\AD\D8\AB"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load5")
  store i64 %calltmp, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 0
  br i1 %cmpgetmp, label %then_69, label %merge_70

merge_68:                                         ; preds = %then_65
  ret i64 -1

merge_70:                                         ; preds = %merge_66
  %calltmp7 = call i64 @"cache_\D8\B3\D8\B7\D8\B1_LRU"()
  store i64 %calltmp7, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %"%\D8\B3\D8\B7\D8\B1.load" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %arr.data8 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 %"%\D8\B3\D8\B7\D8\B1.load"
  %arr.get = load i64, ptr %arr.elem9, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp, label %then_71, label %merge_72

merge_72:                                         ; preds = %then_71, %merge_70
  %"%\D8\B3\D8\B7\D8\B1.load12" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 %"%\D8\B3\D8\B7\D8\B1.load12"
  store i64 0, ptr %arr.elem14, align 8
  %"%\D8\B3\D8\B7\D8\B1.load15" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load16" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data17 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem18 = getelementptr i64, ptr %arr.data17, i64 %"%\D8\B3\D8\B7\D8\B1.load15"
  store i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load16", ptr %arr.elem18, align 8
  %"%\D8\B3\D8\B7\D8\B1.load19" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load20" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %arr.data21 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 %"%\D8\B3\D8\B7\D8\B1.load19"
  store i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load20", ptr %arr.elem22, align 8
  %"%\D8\B3\D8\B7\D8\B1.load23" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %arr.data24 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 %"%\D8\B3\D8\B7\D8\B1.load23"
  store i64 0, ptr %arr.elem25, align 8
  ret i64 0

then_65:                                          ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B13", align 8
  call void @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load", i64 1, i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load")
  store i64 0, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpgttmp, label %then_67, label %merge_68

then_67:                                          ; preds = %then_65
  ret i64 0

then_69:                                          ; preds = %merge_66
  %"%\D9\81\D9\87\D8\B1\D8\B3.load6" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load6"
  store i64 0, ptr %arr.elem, align 8
  ret i64 0

then_71:                                          ; preds = %merge_70
  %"%\D8\B3\D8\B7\D8\B1.load10" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %calltmp11 = call i64 @"cache_\D8\A3\D8\AE\D9\84_\D8\B3\D8\B7\D8\B1"(i64 %"%\D8\B3\D8\B7\D8\B1.load10")
  br label %merge_72
}

define i64 @"cache_\D8\A7\D9\83\D8\AA\D8\A8_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B14" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B14", align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  store i64 0, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  store i64 0, ptr %"%\D9\82", align 8
  br label %while_cond_73

merge_77:                                         ; preds = %while_body_74
  %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load" = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  %addtmp9 = add i64 %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load", 1
  store i64 %addtmp9, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  %"%\D9\82.load10" = load i64, ptr %"%\D9\82", align 8
  %addtmp11 = add i64 %"%\D9\82.load10", 1
  store i64 %addtmp11, ptr %"%\D9\82", align 8
  br label %while_cond_73

then_76:                                          ; preds = %while_body_74
  %loadtmp8 = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  ret i64 %loadtmp8

while_body_74:                                    ; preds = %while_cond_73
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"%\D9\82.load5" = load i64, ptr %"%\D9\82", align 8
  %addtmp = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load", %"%\D9\82.load5"
  %"%\D9\82.load6" = load i64, ptr %"%\D9\82", align 8
  %multmp = mul i64 %"%\D9\82.load6", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B14", align 8
  %addtmp7 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load", %multmp
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp = call i64 @"cache_\D8\A7\D9\83\D8\AA\D8\A8_\D9\82\D8\B7\D8\A7\D8\B9"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", i64 %addtmp, i64 %addtmp7)
  store i64 %calltmp, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_76, label %merge_77

while_cond_73:                                    ; preds = %merge_77, %entry
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82.load", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load"
  br i1 %cmplttmp, label %while_body_74, label %while_exit_75

while_exit_75:                                    ; preds = %while_cond_73
  %loadtmp = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  ret i64 %loadtmp
}

define i64 @"cache_\D8\A7\D9\83\D8\AA\D8\A8_\D9\82\D8\B7\D8\A7\D8\B9_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1") {
entry:
  %"%\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B13" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B13", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B13", align 8
  call void @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load", i64 1, i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load")
  store i64 0, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_78, label %merge_79

merge_79:                                         ; preds = %entry
  br i1 false, label %then_80, label %merge_81

merge_81:                                         ; preds = %merge_83, %merge_79
  ret i64 0

merge_83:                                         ; preds = %then_82, %then_80
  br label %merge_81

then_78:                                          ; preds = %entry
  ret i64 -1

then_80:                                          ; preds = %merge_79
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %calltmp = call i64 @"cache_\D8\A7\D8\A8\D8\AD\D8\AB"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load5")
  store i64 %calltmp, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 0
  br i1 %cmpgetmp, label %then_82, label %merge_83

then_82:                                          ; preds = %then_80
  %"%\D9\81\D9\87\D8\B1\D8\B3.load6" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load6"
  store i64 0, ptr %arr.elem, align 8
  br label %merge_83
}

define i64 @"cache_\D9\81\D8\B1\D8\BA"() {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\83" = alloca i64, align 8
  %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8" = alloca i64, align 8
  br i1 true, label %then_84, label %merge_85

merge_85:                                         ; preds = %entry
  call void @sad_ll_interrupt_disable()
  store i64 0, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  store i64 0, ptr %"%\D9\83", align 8
  br label %while_cond_86

merge_90:                                         ; preds = %merge_92, %while_body_87
  %"%\D9\83.load14" = load i64, ptr %"%\D9\83", align 8
  %addtmp = add i64 %"%\D9\83.load14", 1
  store i64 %addtmp, ptr %"%\D9\83", align 8
  br label %while_cond_86

merge_92:                                         ; preds = %then_91, %then_89
  br label %merge_90

then_84:                                          ; preds = %entry
  ret i64 0

then_89:                                          ; preds = %while_body_87
  %"%\D9\83.load2" = load i64, ptr %"%\D9\83", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D9\83.load2"
  %arr.get5 = load i64, ptr %arr.elem4, align 8
  %"%\D9\83.load6" = load i64, ptr %"%\D9\83", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D9\83.load6"
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  %"%\D9\83.load10" = load i64, ptr %"%\D9\83", align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 %"%\D9\83.load10"
  %arr.get13 = load i64, ptr %arr.elem12, align 8
  call void @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\83\D8\AA\D9\84\D8\A9"(i64 %arr.get5, i64 %arr.get9, i64 1, i64 %arr.get13)
  store i64 0, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpgttmp, label %then_91, label %merge_92

then_91:                                          ; preds = %then_89
  %"%\D9\83.load15" = load i64, ptr %"%\D9\83", align 8
  %arr.data16 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 %"%\D9\83.load15"
  store i64 0, ptr %arr.elem17, align 8
  %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load" = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  %addtmp18 = add i64 %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load", 1
  store i64 %addtmp18, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  br label %merge_92

while_body_87:                                    ; preds = %while_cond_86
  %"%\D9\83.load1" = load i64, ptr %"%\D9\83", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\83.load1"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_89, label %merge_90

while_cond_86:                                    ; preds = %merge_90, %merge_85
  %"%\D9\83.load" = load i64, ptr %"%\D9\83", align 8
  %cmplttmp = icmp slt i64 %"%\D9\83.load", 64
  br i1 %cmplttmp, label %while_body_87, label %while_exit_88

while_exit_88:                                    ; preds = %while_cond_86
  call void @sad_ll_interrupt_enable()
  %loadtmp = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  ret i64 %loadtmp
}

define i64 @"cache_\D9\81\D8\B1\D8\BA_\D9\82\D8\B1\D8\B5"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\83" = alloca i64, align 8
  %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8" = alloca i64, align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  br i1 true, label %then_93, label %merge_94

merge_101:                                        ; preds = %merge_103, %then_98
  br label %merge_99

merge_103:                                        ; preds = %then_102, %then_100
  br label %merge_101

merge_94:                                         ; preds = %entry
  store i64 0, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  store i64 0, ptr %"%\D9\83", align 8
  br label %while_cond_95

merge_99:                                         ; preds = %merge_101, %while_body_96
  %"%\D9\83.load8" = load i64, ptr %"%\D9\83", align 8
  %addtmp = add i64 %"%\D9\83.load8", 1
  store i64 %addtmp, ptr %"%\D9\83", align 8
  br label %while_cond_95

then_100:                                         ; preds = %then_98
  %"%\D9\83.load9" = load i64, ptr %"%\D9\83", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D9\83.load9"
  %arr.get12 = load i64, ptr %arr.elem11, align 8
  %"%\D9\83.load13" = load i64, ptr %"%\D9\83", align 8
  %arr.data14 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 %"%\D9\83.load13"
  %arr.get16 = load i64, ptr %arr.elem15, align 8
  %"%\D9\83.load17" = load i64, ptr %"%\D9\83", align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 %"%\D9\83.load17"
  %arr.get20 = load i64, ptr %arr.elem19, align 8
  call void @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\83\D8\AA\D9\84\D8\A9"(i64 %arr.get12, i64 %arr.get16, i64 1, i64 %arr.get20)
  store i64 0, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpgttmp, label %then_102, label %merge_103

then_102:                                         ; preds = %then_100
  %"%\D9\83.load21" = load i64, ptr %"%\D9\83", align 8
  %arr.data22 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 %"%\D9\83.load21"
  store i64 0, ptr %arr.elem23, align 8
  %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load" = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  %addtmp24 = add i64 %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load", 1
  store i64 %addtmp24, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  br label %merge_103

then_93:                                          ; preds = %entry
  ret i64 0

then_98:                                          ; preds = %while_body_96
  %"%\D9\83.load3" = load i64, ptr %"%\D9\83", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D9\83.load3"
  %arr.get6 = load i64, ptr %arr.elem5, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmpeqtmp7 = icmp eq i64 %arr.get6, %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load"
  br i1 %cmpeqtmp7, label %then_100, label %merge_101

while_body_96:                                    ; preds = %while_cond_95
  %"%\D9\83.load2" = load i64, ptr %"%\D9\83", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\83.load2"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_98, label %merge_99

while_cond_95:                                    ; preds = %merge_99, %merge_94
  %"%\D9\83.load" = load i64, ptr %"%\D9\83", align 8
  %cmplttmp = icmp slt i64 %"%\D9\83.load", 64
  br i1 %cmplttmp, label %while_body_96, label %while_exit_97

while_exit_97:                                    ; preds = %while_cond_95
  %loadtmp = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  ret i64 %loadtmp
}

define void @"cache_\D8\A7\D9\85\D8\B3\D8\AD"() {
entry:
  %"%\D9\83" = alloca i64, align 8
  %calltmp = call i64 @"cache_\D9\81\D8\B1\D8\BA"()
  store i64 0, ptr %"%\D9\83", align 8
  br label %while_cond_104

while_body_105:                                   ; preds = %while_cond_104
  %"%\D9\83.load1" = load i64, ptr %"%\D9\83", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\83.load1"
  store i64 0, ptr %arr.elem, align 8
  %"%\D9\83.load2" = load i64, ptr %"%\D9\83", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D9\83.load2"
  store i64 0, ptr %arr.elem4, align 8
  %"%\D9\83.load5" = load i64, ptr %"%\D9\83", align 8
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 %"%\D9\83.load5"
  store i64 0, ptr %arr.elem7, align 8
  %"%\D9\83.load8" = load i64, ptr %"%\D9\83", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D9\83.load8"
  store i64 0, ptr %arr.elem10, align 8
  %"%\D9\83.load11" = load i64, ptr %"%\D9\83", align 8
  %addtmp = add i64 %"%\D9\83.load11", 1
  store i64 %addtmp, ptr %"%\D9\83", align 8
  br label %while_cond_104

while_cond_104:                                   ; preds = %while_body_105, %entry
  %"%\D9\83.load" = load i64, ptr %"%\D9\83", align 8
  %cmplttmp = icmp slt i64 %"%\D9\83.load", 64
  br i1 %cmplttmp, label %while_body_105, label %while_exit_106

while_exit_106:                                   ; preds = %while_cond_104
  ret void
}

define i64 @"cache_\D8\A7\D9\82\D9\81\D9\84_\D9\82\D8\B7\D8\A7\D8\B9"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9") {
entry:
  %"%\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  br i1 true, label %then_107, label %merge_108

merge_108:                                        ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %calltmp = call i64 @"cache_\D8\A7\D8\A8\D8\AD\D8\AB"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load")
  store i64 %calltmp, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 0
  br i1 %cmpgetmp, label %then_109, label %merge_110

merge_110:                                        ; preds = %merge_108
  ret i64 -1

then_107:                                         ; preds = %entry
  ret i64 -1

then_109:                                         ; preds = %merge_108
  %"%\D9\81\D9\87\D8\B1\D8\B3.load3" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load3"
  store i64 0, ptr %arr.elem, align 8
  ret i64 0
}

define i64 @"cache_\D8\A7\D9\81\D8\AA\D8\AD_\D9\82\D8\B7\D8\A7\D8\B9"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9") {
entry:
  %"%\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  br i1 true, label %then_111, label %merge_112

merge_112:                                        ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %calltmp = call i64 @"cache_\D8\A7\D8\A8\D8\AD\D8\AB"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load")
  store i64 %calltmp, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 0
  br i1 %cmpgetmp, label %then_113, label %merge_114

merge_114:                                        ; preds = %merge_112
  ret i64 -1

merge_116:                                        ; preds = %then_115, %then_113
  ret i64 0

then_111:                                         ; preds = %entry
  ret i64 -1

then_113:                                         ; preds = %merge_112
  %"%\D9\81\D9\87\D8\B1\D8\B3.load3" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load3"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_115, label %merge_116

then_115:                                         ; preds = %then_113
  %"%\D9\81\D9\87\D8\B1\D8\B3.load4" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load4"
  store i64 0, ptr %arr.elem6, align 8
  br label %merge_116
}

define i64 @"\D8\A3\D8\B6\D9\81_\D8\B7\D9\84\D8\A8_IO"(i64 %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D8\B7\D9\84\D8\A8", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_lba", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9") {
entry:
  %"%\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A95" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A95", align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA4" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA4", align 8
  %"\D8\B1\D9\82\D9\85_lba3" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_lba", ptr %"\D8\B1\D9\82\D9\85_lba3", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B1\D8\B52" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B1\D8\B52", align 8
  %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D8\B7\D9\84\D8\A81" = alloca i64, align 8
  store i64 %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D8\B7\D9\84\D8\A8", ptr %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D8\B7\D9\84\D8\A81", align 8
  br i1 false, label %then_117, label %merge_118

merge_118:                                        ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D8\B7\D9\84\D8\A8.load" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D8\B7\D9\84\D8\A81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load"
  store i64 %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D8\B7\D9\84\D8\A8.load", ptr %arr.elem, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load6" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B1\D8\B52", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load6"
  store i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", ptr %arr.elem8, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load9" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D8\B1\D9\82\D9\85_lba.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_lba3", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load9"
  store i64 %"%\D8\B1\D9\82\D9\85_lba.load", ptr %arr.elem11, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load12" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA4", align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load12"
  store i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load", ptr %arr.elem14, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load15" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A95", align 8
  %arr.data16 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load15"
  store i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load", ptr %arr.elem17, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load18" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data19 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load18"
  store i64 0, ptr %arr.elem20, align 8
  %loadtmp = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  ret i64 %loadtmp

then_117:                                         ; preds = %entry
  ret i64 -1
}

define i64 @"\D8\B9\D8\A7\D9\84\D8\AC_\D8\B7\D9\84\D8\A8_IO"() {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  br i1 true, label %then_119, label %merge_120

else_125:                                         ; preds = %merge_122
  %"%\D9\81\D9\87\D8\B1\D8\B3.load21" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data22 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load21"
  %arr.get24 = load i64, ptr %arr.elem23, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load25" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data26 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem27 = getelementptr i64, ptr %arr.data26, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load25"
  %arr.get28 = load i64, ptr %arr.elem27, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load29" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data30 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem31 = getelementptr i64, ptr %arr.data30, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load29"
  %arr.get32 = load i64, ptr %arr.elem31, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load33" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data34 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem35 = getelementptr i64, ptr %arr.data34, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load33"
  %arr.get36 = load i64, ptr %arr.elem35, align 8
  %calltmp37 = call i64 @"cache_\D8\A7\D9\83\D8\AA\D8\A8_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA"(i64 %arr.get24, i64 %arr.get28, i64 %arr.get32, i64 %arr.get36)
  store i64 %calltmp37, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  br label %merge_124

else_128:                                         ; preds = %merge_124
  %"%\D9\81\D9\87\D8\B1\D8\B3.load45" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data46 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem47 = getelementptr i64, ptr %arr.data46, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load45"
  store i64 0, ptr %arr.elem47, align 8
  br label %merge_127

merge_120:                                        ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp, label %then_121, label %merge_122

merge_122:                                        ; preds = %merge_120
  store i64 0, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load1" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data2 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem3 = getelementptr i64, ptr %arr.data2, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load1"
  %arr.get4 = load i64, ptr %arr.elem3, align 8
  %cmpeqtmp = icmp eq i64 %arr.get4, 0
  br i1 %cmpeqtmp, label %then_123, label %else_125

merge_124:                                        ; preds = %else_125, %then_123
  %"%\D9\81\D9\87\D8\B1\D8\B3.load38" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data39 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem40 = getelementptr i64, ptr %arr.data39, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load38"
  %arr.get41 = load i64, ptr %arr.elem40, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", %arr.get41
  br i1 %cmpgetmp, label %then_126, label %else_128

merge_127:                                        ; preds = %else_128, %then_126
  ret i64 1

then_119:                                         ; preds = %entry
  ret i64 0

then_121:                                         ; preds = %merge_120
  ret i64 1

then_123:                                         ; preds = %merge_122
  %"%\D9\81\D9\87\D8\B1\D8\B3.load5" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load5"
  %arr.get8 = load i64, ptr %arr.elem7, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load9" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load9"
  %arr.get12 = load i64, ptr %arr.elem11, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load13" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data14 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load13"
  %arr.get16 = load i64, ptr %arr.elem15, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load17" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load17"
  %arr.get20 = load i64, ptr %arr.elem19, align 8
  %calltmp = call i64 @"cache_\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA"(i64 %arr.get8, i64 %arr.get12, i64 %arr.get16, i64 %arr.get20)
  store i64 %calltmp, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  br label %merge_124

then_126:                                         ; preds = %merge_124
  %"%\D9\81\D9\87\D8\B1\D8\B3.load42" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data43 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem44 = getelementptr i64, ptr %arr.data43, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load42"
  store i64 0, ptr %arr.elem44, align 8
  br label %merge_127
}

define void @"\D8\B9\D8\A7\D9\84\D8\AC_\D9\83\D9\84_\D8\A7\D9\84\D8\B7\D9\84\D8\A8\D8\A7\D8\AA"() {
entry:
  br label %while_cond_129

while_body_130:                                   ; preds = %while_cond_129
  %calltmp = call i64 @"\D8\B9\D8\A7\D9\84\D8\AC_\D8\B7\D9\84\D8\A8_IO"()
  br label %while_cond_129

while_cond_129:                                   ; preds = %while_body_130, %entry
  br i1 false, label %while_body_130, label %while_exit_131

while_exit_131:                                   ; preds = %while_cond_129
  ret void
}

define i64 @"cache_\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D8\B9_prefetch"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_pf" = alloca i64, align 8
  %"%\D8\B3\D8\B7\D8\B1" = alloca i64, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\85\D9\88\D8\AC\D9\88\D8\AF" = alloca i64, align 8
  %"%lba_\D8\A7\D9\84\D9\82\D8\A7\D8\AF\D9\85" = alloca i64, align 8
  %"%\D9\82" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A93" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A93", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A93", align 8
  %calltmp = call i64 @"cache_\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load")
  store i64 %calltmp, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_132, label %merge_133

else_136:                                         ; preds = %merge_133
  br label %merge_135

else_139:                                         ; preds = %then_134
  br label %merge_138

else_151:                                         ; preds = %then_147
  %"%\D8\B3\D8\B7\D8\B1.load22" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %arr.data23 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem24 = getelementptr i64, ptr %arr.data23, i64 %"%\D8\B3\D8\B7\D8\B1.load22"
  %arr.get25 = load i64, ptr %arr.elem24, align 8
  %cmpnetmp26 = icmp ne i64 %arr.get25, 0
  br i1 %cmpnetmp26, label %then_152, label %merge_153

merge_133:                                        ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4", 0
  br i1 %cmpeqtmp, label %then_134, label %else_136

merge_135:                                        ; preds = %merge_138, %else_136
  br i1 false, label %then_140, label %merge_141

merge_138:                                        ; preds = %else_139, %then_137
  br label %merge_135

merge_141:                                        ; preds = %while_exit_144, %merge_135
  ret i64 0

merge_146:                                        ; preds = %merge_148, %while_body_143
  %"%\D9\82.load13" = load i64, ptr %"%\D9\82", align 8
  %addtmp14 = add i64 %"%\D9\82.load13", 1
  store i64 %addtmp14, ptr %"%\D9\82", align 8
  br label %while_cond_142

merge_148:                                        ; preds = %merge_155, %then_145
  br label %merge_146

merge_150:                                        ; preds = %merge_153, %then_149
  %"%\D8\B3\D8\B7\D8\B1.load27" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %arr.data28 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem29 = getelementptr i64, ptr %arr.data28, i64 %"%\D8\B3\D8\B7\D8\B1.load27"
  %arr.get30 = load i64, ptr %arr.elem29, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load31" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%lba_\D8\A7\D9\84\D9\82\D8\A7\D8\AF\D9\85.load32" = load i64, ptr %"%lba_\D8\A7\D9\84\D9\82\D8\A7\D8\AF\D9\85", align 8
  call void @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load31", i64 %"%lba_\D8\A7\D9\84\D9\82\D8\A7\D8\AF\D9\85.load32", i64 1, i64 %arr.get30)
  store i64 0, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_pf", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_pf.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_pf", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_pf.load", 0
  br i1 %cmpgttmp, label %then_154, label %merge_155

merge_153:                                        ; preds = %then_152, %else_151
  br label %merge_150

merge_155:                                        ; preds = %then_154, %merge_150
  br label %merge_148

then_132:                                         ; preds = %entry
  %loadtmp = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  ret i64 %loadtmp

then_134:                                         ; preds = %merge_133
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %cmpeqtmp6 = icmp eq i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load5", 1
  br i1 %cmpeqtmp6, label %then_137, label %else_139

then_137:                                         ; preds = %then_134
  br label %merge_138

then_140:                                         ; preds = %merge_135
  store i64 1, ptr %"%\D9\82", align 8
  br label %while_cond_142

then_145:                                         ; preds = %while_body_143
  %calltmp11 = call i64 @"cache_\D8\B3\D8\B7\D8\B1_LRU"()
  store i64 %calltmp11, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %"%\D8\B3\D8\B7\D8\B1.load" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B3\D8\B7\D8\B1.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp12 = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp12, label %then_147, label %merge_148

then_147:                                         ; preds = %then_145
  %"%\D8\B3\D8\B7\D8\B1.load15" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %arr.data16 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 %"%\D8\B3\D8\B7\D8\B1.load15"
  %arr.get18 = load i64, ptr %arr.elem17, align 8
  %cmpeqtmp19 = icmp eq i64 %arr.get18, 0
  br i1 %cmpeqtmp19, label %then_149, label %else_151

then_149:                                         ; preds = %then_147
  %"%\D8\B3\D8\B7\D8\B1.load20" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %calltmp21 = call i64 @"cache_\D8\A3\D8\AE\D9\84_\D8\B3\D8\B7\D8\B1"(i64 %"%\D8\B3\D8\B7\D8\B1.load20")
  br label %merge_150

then_152:                                         ; preds = %else_151
  %"%\D8\B3\D8\B7\D8\B1.load33" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %arr.data34 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem35 = getelementptr i64, ptr %arr.data34, i64 %"%\D8\B3\D8\B7\D8\B1.load33"
  store i64 0, ptr %arr.elem35, align 8
  br label %merge_153

then_154:                                         ; preds = %merge_150
  %"%\D8\B3\D8\B7\D8\B1.load36" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %arr.data37 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem38 = getelementptr i64, ptr %arr.data37, i64 %"%\D8\B3\D8\B7\D8\B1.load36"
  store i64 0, ptr %arr.elem38, align 8
  %"%\D8\B3\D8\B7\D8\B1.load39" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load40" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data41 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem42 = getelementptr i64, ptr %arr.data41, i64 %"%\D8\B3\D8\B7\D8\B1.load39"
  store i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load40", ptr %arr.elem42, align 8
  %"%\D8\B3\D8\B7\D8\B1.load43" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %"%lba_\D8\A7\D9\84\D9\82\D8\A7\D8\AF\D9\85.load44" = load i64, ptr %"%lba_\D8\A7\D9\84\D9\82\D8\A7\D8\AF\D9\85", align 8
  %arr.data45 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem46 = getelementptr i64, ptr %arr.data45, i64 %"%\D8\B3\D8\B7\D8\B1.load43"
  store i64 %"%lba_\D8\A7\D9\84\D9\82\D8\A7\D8\AF\D9\85.load44", ptr %arr.elem46, align 8
  %"%\D8\B3\D8\B7\D8\B1.load47" = load i64, ptr %"%\D8\B3\D8\B7\D8\B1", align 8
  %arr.data48 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem49 = getelementptr i64, ptr %arr.data48, i64 %"%\D8\B3\D8\B7\D8\B1.load47"
  store i64 0, ptr %arr.elem49, align 8
  br label %merge_155

while_body_143:                                   ; preds = %while_cond_142
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load7" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"%\D9\82.load8" = load i64, ptr %"%\D9\82", align 8
  %addtmp = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load7", %"%\D9\82.load8"
  store i64 %addtmp, ptr %"%lba_\D8\A7\D9\84\D9\82\D8\A7\D8\AF\D9\85", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load9" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%lba_\D8\A7\D9\84\D9\82\D8\A7\D8\AF\D9\85.load" = load i64, ptr %"%lba_\D8\A7\D9\84\D9\82\D8\A7\D8\AF\D9\85", align 8
  %calltmp10 = call i64 @"cache_\D8\A7\D8\A8\D8\AD\D8\AB"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load9", i64 %"%lba_\D8\A7\D9\84\D9\82\D8\A7\D8\AF\D9\85.load")
  store i64 %calltmp10, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\85\D9\88\D8\AC\D9\88\D8\AF", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\85\D9\88\D8\AC\D9\88\D8\AF.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\85\D9\88\D8\AC\D9\88\D8\AF", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\85\D9\88\D8\AC\D9\88\D8\AF.load", 0
  br i1 %cmplttmp, label %then_145, label %merge_146

while_cond_142:                                   ; preds = %merge_146, %then_140
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  %cmpletmp = icmp sle i64 %"%\D9\82.load", 0
  br i1 %cmpletmp, label %while_body_143, label %while_exit_144

while_exit_144:                                   ; preds = %while_cond_142
  br label %merge_141
}

define i64 @"blk_\D8\A7\D9\82\D8\B1\D8\A3"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9") {
entry:
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A93" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A93", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A93", align 8
  %calltmp = call i64 @"cache_\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D8\B9_prefetch"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load")
  ret i64 %calltmp
}

define i64 @"blk_\D8\A7\D9\83\D8\AA\D8\A8"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1") {
entry:
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B13" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B13", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B13", align 8
  %calltmp = call i64 @"cache_\D8\A7\D9\83\D8\AA\D8\A8_\D9\82\D8\B7\D8\A7\D8\B9"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load")
  ret i64 %calltmp
}

define i64 @"blk_\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D8\AA\D8\B9\D8\AF\D8\AF"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9") {
entry:
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A94" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A94", align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A94", align 8
  %calltmp = call i64 @"cache_\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load", i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load")
  ret i64 %calltmp
}

define i64 @"blk_\D8\A7\D9\83\D8\AA\D8\A8_\D9\85\D8\AA\D8\B9\D8\AF\D8\AF"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1") {
entry:
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B14" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B14", align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B14", align 8
  %calltmp = call i64 @"cache_\D8\A7\D9\83\D8\AA\D8\A8_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load", i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load")
  ret i64 %calltmp
}

define i64 @"blk_\D9\81\D8\B1\D8\BA"() {
entry:
  %calltmp = call i64 @"cache_\D9\81\D8\B1\D8\BA"()
  ret i64 %calltmp
}

define void @"\D8\A7\D8\B7\D8\A8\D8\B9_\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA_\D8\A7\D9\84\D9\83\D8\A7\D8\B4"() {
entry:
  %"%\D9\83" = alloca i64, align 8
  %"%\D9\85\D9\82\D9\81\D9\84\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\B9\D8\AF\D9\84\D8\A9_\D8\B9" = alloca i64, align 8
  %"%\D9\85\D8\B4\D8\BA\D9\88\D9\84\D8\A9" = alloca i64, align 8
  %"%\D9\86\D8\B3\D8\A8\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA" = alloca i64, align 8
  store i64 0, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA", align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load", 0
  br i1 %cmpgttmp, label %then_156, label %merge_157

merge_157:                                        ; preds = %then_156, %entry
  store i64 0, ptr %"%\D9\85\D8\B4\D8\BA\D9\88\D9\84\D8\A9", align 8
  store i64 0, ptr %"%\D9\85\D8\B9\D8\AF\D9\84\D8\A9_\D8\B9", align 8
  store i64 0, ptr %"%\D9\85\D9\82\D9\81\D9\84\D8\A9", align 8
  store i64 0, ptr %"%\D9\83", align 8
  br label %while_cond_158

merge_162:                                        ; preds = %then_161, %while_body_159
  %"%\D9\83.load4" = load i64, ptr %"%\D9\83", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D9\83.load4"
  %arr.get7 = load i64, ptr %arr.elem6, align 8
  %cmpeqtmp = icmp eq i64 %arr.get7, 0
  br i1 %cmpeqtmp, label %then_163, label %merge_164

merge_164:                                        ; preds = %then_163, %merge_162
  %"%\D9\83.load10" = load i64, ptr %"%\D9\83", align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 %"%\D9\83.load10"
  %arr.get13 = load i64, ptr %arr.elem12, align 8
  %cmpeqtmp14 = icmp eq i64 %arr.get13, 0
  br i1 %cmpeqtmp14, label %then_165, label %merge_166

merge_166:                                        ; preds = %then_165, %merge_164
  %"%\D9\83.load17" = load i64, ptr %"%\D9\83", align 8
  %addtmp18 = add i64 %"%\D9\83.load17", 1
  store i64 %addtmp18, ptr %"%\D9\83", align 8
  br label %while_cond_158

then_156:                                         ; preds = %entry
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load1" = load i64, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA", align 8
  %divtmp = sdiv i64 0, %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load1"
  store i64 %divtmp, ptr %"%\D9\86\D8\B3\D8\A8\D8\A9", align 8
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load" = load i64, ptr %"%\D9\86\D8\B3\D8\A8\D8\A9", align 8
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load")
  br label %merge_157

then_161:                                         ; preds = %while_body_159
  %"%\D9\85\D8\B4\D8\BA\D9\88\D9\84\D8\A9.load3" = load i64, ptr %"%\D9\85\D8\B4\D8\BA\D9\88\D9\84\D8\A9", align 8
  %addtmp = add i64 %"%\D9\85\D8\B4\D8\BA\D9\88\D9\84\D8\A9.load3", 1
  store i64 %addtmp, ptr %"%\D9\85\D8\B4\D8\BA\D9\88\D9\84\D8\A9", align 8
  br label %merge_162

then_163:                                         ; preds = %merge_162
  %"%\D9\85\D8\B9\D8\AF\D9\84\D8\A9_\D8\B9.load8" = load i64, ptr %"%\D9\85\D8\B9\D8\AF\D9\84\D8\A9_\D8\B9", align 8
  %addtmp9 = add i64 %"%\D9\85\D8\B9\D8\AF\D9\84\D8\A9_\D8\B9.load8", 1
  store i64 %addtmp9, ptr %"%\D9\85\D8\B9\D8\AF\D9\84\D8\A9_\D8\B9", align 8
  br label %merge_164

then_165:                                         ; preds = %merge_164
  %"%\D9\85\D9\82\D9\81\D9\84\D8\A9.load15" = load i64, ptr %"%\D9\85\D9\82\D9\81\D9\84\D8\A9", align 8
  %addtmp16 = add i64 %"%\D9\85\D9\82\D9\81\D9\84\D8\A9.load15", 1
  store i64 %addtmp16, ptr %"%\D9\85\D9\82\D9\81\D9\84\D8\A9", align 8
  br label %merge_166

while_body_159:                                   ; preds = %while_cond_158
  %"%\D9\83.load2" = load i64, ptr %"%\D9\83", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\83.load2"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp, label %then_161, label %merge_162

while_cond_158:                                   ; preds = %merge_166, %merge_157
  %"%\D9\83.load" = load i64, ptr %"%\D9\83", align 8
  %cmplttmp = icmp slt i64 %"%\D9\83.load", 64
  br i1 %cmplttmp, label %while_body_159, label %while_exit_160

while_exit_160:                                   ; preds = %while_cond_158
  %"%\D9\85\D8\B4\D8\BA\D9\88\D9\84\D8\A9.load" = load i64, ptr %"%\D9\85\D8\B4\D8\BA\D9\88\D9\84\D8\A9", align 8
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\85\D8\B4\D8\BA\D9\88\D9\84\D8\A9.load")
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 64)
  %"%\D9\85\D8\B9\D8\AF\D9\84\D8\A9_\D8\B9.load" = load i64, ptr %"%\D9\85\D8\B9\D8\AF\D9\84\D8\A9_\D8\B9", align 8
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\85\D8\B9\D8\AF\D9\84\D8\A9_\D8\B9.load")
  %"%\D9\85\D9\82\D9\81\D9\84\D8\A9.load" = load i64, ptr %"%\D9\85\D9\82\D9\81\D9\84\D8\A9", align 8
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\85\D9\82\D9\81\D9\84\D8\A9.load")
  ret void
}

define void @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\A7\D9\84\D9\83\D8\A7\D8\B4"() {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A95" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A94" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A93" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A92" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91" = alloca i64, align 8
  %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC" = alloca i64, align 8
  store i64 0, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  %calltmp = call i64 @"\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D9\83\D8\A7\D8\B4"()
  store i64 %calltmp, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91.load", 0
  br i1 %cmpeqtmp, label %then_167, label %else_169

else_169:                                         ; preds = %entry
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_168

else_172:                                         ; preds = %merge_168
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_171

else_175:                                         ; preds = %merge_171
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_174

else_178:                                         ; preds = %merge_174
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_177

else_181:                                         ; preds = %merge_177
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_180

merge_168:                                        ; preds = %else_169, %then_167
  %calltmp1 = call i64 @"cache_\D8\A7\D8\A8\D8\AD\D8\AB"(i64 0, i64 0)
  store i64 %calltmp1, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A92", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A92.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A92", align 8
  %cmpeqtmp2 = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A92.load", -1
  br i1 %cmpeqtmp2, label %then_170, label %else_172

merge_171:                                        ; preds = %else_172, %then_170
  %calltmp5 = call i64 @"cache_\D8\B3\D8\B7\D8\B1_\D9\81\D8\A7\D8\B1\D8\BA"()
  store i64 %calltmp5, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A93", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A93.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A93", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A93.load", 0
  br i1 %cmpgetmp, label %then_173, label %else_175

merge_174:                                        ; preds = %else_175, %then_173
  %calltmp8 = call i64 @"\D8\A3\D8\B6\D9\81_\D8\B7\D9\84\D8\A8_IO"(i64 0, i64 0, i64 0, i64 1, i64 0)
  store i64 %calltmp8, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A94", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A94.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A94", align 8
  %cmpgetmp9 = icmp sge i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A94.load", 0
  br i1 %cmpgetmp9, label %then_176, label %else_178

merge_177:                                        ; preds = %else_178, %then_176
  call void @"cache_\D8\A7\D9\85\D8\B3\D8\AD"()
  %calltmp12 = call i64 @"cache_\D8\B3\D8\B7\D8\B1_\D9\81\D8\A7\D8\B1\D8\BA"()
  store i64 %calltmp12, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A95", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A95.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A95", align 8
  %cmpeqtmp13 = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A95.load", 0
  br i1 %cmpeqtmp13, label %then_179, label %else_181

merge_180:                                        ; preds = %else_181, %then_179
  %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load16" = load i64, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load16")
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 5)
  call void @"\D8\A7\D8\B7\D8\A8\D8\B9_\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA_\D8\A7\D9\84\D9\83\D8\A7\D8\B4"()
  ret void

then_167:                                         ; preds = %entry
  %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load" = load i64, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  %addtmp = add i64 %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load", 1
  store i64 %addtmp, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  br label %merge_168

then_170:                                         ; preds = %merge_168
  %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load3" = load i64, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  %addtmp4 = add i64 %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load3", 1
  store i64 %addtmp4, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  br label %merge_171

then_173:                                         ; preds = %merge_171
  %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load6" = load i64, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  %addtmp7 = add i64 %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load6", 1
  store i64 %addtmp7, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  br label %merge_174

then_176:                                         ; preds = %merge_174
  %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load10" = load i64, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  %addtmp11 = add i64 %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load10", 1
  store i64 %addtmp11, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  br label %merge_177

then_179:                                         ; preds = %merge_177
  %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load14" = load i64, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  %addtmp15 = add i64 %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load14", 1
  store i64 %addtmp15, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  call void @"cache_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  br label %merge_180
}

define void @__sad_main() {
entry:
  %"%PREFETCH_AHEAD" = alloca i64, align 8
  %"%prefetch_\D8\B9\D8\AF\D8\A7\D8\AF_\D8\AA\D8\B3\D9\84\D8\B3\D9\84\D9\8A" = alloca i64, align 8
  %"%prefetch_\D8\A2\D8\AE\D8\B1_lba" = alloca i64, align 8
  %"%prefetch_\D8\A2\D8\AE\D8\B1_\D9\82\D8\B1\D8\B5" = alloca i64, align 8
  %"%\D8\B7\D8\A7\D8\A8\D9\88\D8\B1_\D8\B9\D8\AF\D8\AF" = alloca i64, align 8
  %"%\D8\B7\D8\A7\D8\A8\D9\88\D8\B1_\D8\B0\D9\8A\D9\84" = alloca i64, align 8
  %"%\D8\B7\D8\A7\D8\A8\D9\88\D8\B1_\D8\B1\D8\A3\D8\B3" = alloca i64, align 8
  %"%IOREQ_ERROR" = alloca i64, align 8
  %"%IOREQ_DONE" = alloca i64, align 8
  %"%IOREQ_PENDING" = alloca i64, align 8
  %"%IOREQ_EMPTY" = alloca i64, align 8
  %"%\D8\B7\D9\84\D8\A8_\D8\AD\D8\A7\D9\84\D8\A9_q" = alloca i64, align 8
  %"%\D8\B7\D9\84\D8\A8_\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca i64, align 8
  %"%\D8\B7\D9\84\D8\A8_\D8\B9\D8\AF\D8\AF" = alloca i64, align 8
  %"%\D8\B7\D9\84\D8\A8_lba" = alloca i64, align 8
  %"%\D8\B7\D9\84\D8\A8_\D9\82\D8\B1\D8\B5" = alloca i64, align 8
  %"%\D8\B7\D9\84\D8\A8_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%cache_\D9\85\D9\87\D9\8A\D8\A3" = alloca i64, align 8
  %"%cache_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9" = alloca i64, align 8
  %"%cache_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9" = alloca i64, align 8
  %"%cache_\D8\A5\D8\AE\D9\84\D8\A7\D8\A1\D8\A7\D8\AA" = alloca i64, align 8
  %"%cache_\D8\AA\D9\81\D8\B1\D9\8A\D8\BA\D8\A7\D8\AA" = alloca i64, align 8
  %"%cache_\D9\83\D8\AA\D8\A7\D8\A8\D8\A7\D8\AA_\D9\85\D8\A4\D8\AC\D9\84\D8\A9" = alloca i64, align 8
  %"%cache_\D8\A5\D8\AE\D9\81\D8\A7\D9\82\D8\A7\D8\AA" = alloca i64, align 8
  %"%cache_\D8\A5\D8\B5\D8\A7\D8\A8\D8\A7\D8\AA" = alloca i64, align 8
  %"%cache_\D8\B9\D8\AF\D8\A7\D8\AF_LRU" = alloca i64, align 8
  %"%cache_\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca i64, align 8
  %"%cache_\D8\A7\D8\B3\D8\AA\D8\AE\D8\AF\D8\A7\D9\85" = alloca i64, align 8
  %"%cache_lba" = alloca i64, align 8
  %"%cache_\D9\82\D8\B1\D8\B5" = alloca i64, align 8
  %"%cache_\D8\AD\D8\A7\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\B7\D8\A7\D8\A8\D9\88\D8\B1" = alloca i64, align 8
  %"%CACHE_LOCKED" = alloca i64, align 8
  %"%CACHE_DIRTY" = alloca i64, align 8
  %"%CACHE_CLEAN" = alloca i64, align 8
  %"%CACHE_EMPTY" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\B7\D8\B1\D9\82_\D8\A7\D9\84\D8\AA\D8\AF\D8\A7\D8\B9\D9\8A" = alloca i64, align 8
  %"%\D8\B3\D8\B9\D8\A9_\D8\A7\D9\84\D9\83\D8\A7\D8\B4" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9_CACHE" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 512, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9_CACHE", align 8
  store i64 256, ptr %"%\D8\B3\D8\B9\D8\A9_\D8\A7\D9\84\D9\83\D8\A7\D8\B4", align 8
  store i64 4, ptr %"%\D8\B7\D8\B1\D9\82_\D8\A7\D9\84\D8\AA\D8\AF\D8\A7\D8\B9\D9\8A", align 8
  store i64 64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%CACHE_EMPTY", align 8
  store i64 1, ptr %"%CACHE_CLEAN", align 8
  store i64 2, ptr %"%CACHE_DIRTY", align 8
  store i64 3, ptr %"%CACHE_LOCKED", align 8
  store i64 64, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\B7\D8\A7\D8\A8\D9\88\D8\B1", align 8
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 64, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep1 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data2 = load ptr, ptr %arr.data.gep1, align 8
  %arr.elem = getelementptr i64, ptr %arr.data2, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data.gep3 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data4 = load ptr, ptr %arr.data.gep3, align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 1
  store i64 0, ptr %arr.elem5, align 8
  %arr.data.gep6 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data7 = load ptr, ptr %arr.data.gep6, align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 2
  store i64 0, ptr %arr.elem8, align 8
  %arr.data.gep9 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data10 = load ptr, ptr %arr.data.gep9, align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 3
  store i64 0, ptr %arr.elem11, align 8
  %arr.data.gep12 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data13 = load ptr, ptr %arr.data.gep12, align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 4
  store i64 0, ptr %arr.elem14, align 8
  %arr.data.gep15 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data16 = load ptr, ptr %arr.data.gep15, align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 5
  store i64 0, ptr %arr.elem17, align 8
  %arr.data.gep18 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data19 = load ptr, ptr %arr.data.gep18, align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 6
  store i64 0, ptr %arr.elem20, align 8
  %arr.data.gep21 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data22 = load ptr, ptr %arr.data.gep21, align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 7
  store i64 0, ptr %arr.elem23, align 8
  %arr.data.gep24 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data25 = load ptr, ptr %arr.data.gep24, align 8
  %arr.elem26 = getelementptr i64, ptr %arr.data25, i64 8
  store i64 0, ptr %arr.elem26, align 8
  %arr.data.gep27 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data28 = load ptr, ptr %arr.data.gep27, align 8
  %arr.elem29 = getelementptr i64, ptr %arr.data28, i64 9
  store i64 0, ptr %arr.elem29, align 8
  %arr.data.gep30 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data31 = load ptr, ptr %arr.data.gep30, align 8
  %arr.elem32 = getelementptr i64, ptr %arr.data31, i64 10
  store i64 0, ptr %arr.elem32, align 8
  %arr.data.gep33 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data34 = load ptr, ptr %arr.data.gep33, align 8
  %arr.elem35 = getelementptr i64, ptr %arr.data34, i64 11
  store i64 0, ptr %arr.elem35, align 8
  %arr.data.gep36 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data37 = load ptr, ptr %arr.data.gep36, align 8
  %arr.elem38 = getelementptr i64, ptr %arr.data37, i64 12
  store i64 0, ptr %arr.elem38, align 8
  %arr.data.gep39 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data40 = load ptr, ptr %arr.data.gep39, align 8
  %arr.elem41 = getelementptr i64, ptr %arr.data40, i64 13
  store i64 0, ptr %arr.elem41, align 8
  %arr.data.gep42 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data43 = load ptr, ptr %arr.data.gep42, align 8
  %arr.elem44 = getelementptr i64, ptr %arr.data43, i64 14
  store i64 0, ptr %arr.elem44, align 8
  %arr.data.gep45 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data46 = load ptr, ptr %arr.data.gep45, align 8
  %arr.elem47 = getelementptr i64, ptr %arr.data46, i64 15
  store i64 0, ptr %arr.elem47, align 8
  %arr.data.gep48 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data49 = load ptr, ptr %arr.data.gep48, align 8
  %arr.elem50 = getelementptr i64, ptr %arr.data49, i64 16
  store i64 0, ptr %arr.elem50, align 8
  %arr.data.gep51 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data52 = load ptr, ptr %arr.data.gep51, align 8
  %arr.elem53 = getelementptr i64, ptr %arr.data52, i64 17
  store i64 0, ptr %arr.elem53, align 8
  %arr.data.gep54 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data55 = load ptr, ptr %arr.data.gep54, align 8
  %arr.elem56 = getelementptr i64, ptr %arr.data55, i64 18
  store i64 0, ptr %arr.elem56, align 8
  %arr.data.gep57 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data58 = load ptr, ptr %arr.data.gep57, align 8
  %arr.elem59 = getelementptr i64, ptr %arr.data58, i64 19
  store i64 0, ptr %arr.elem59, align 8
  %arr.data.gep60 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data61 = load ptr, ptr %arr.data.gep60, align 8
  %arr.elem62 = getelementptr i64, ptr %arr.data61, i64 20
  store i64 0, ptr %arr.elem62, align 8
  %arr.data.gep63 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data64 = load ptr, ptr %arr.data.gep63, align 8
  %arr.elem65 = getelementptr i64, ptr %arr.data64, i64 21
  store i64 0, ptr %arr.elem65, align 8
  %arr.data.gep66 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data67 = load ptr, ptr %arr.data.gep66, align 8
  %arr.elem68 = getelementptr i64, ptr %arr.data67, i64 22
  store i64 0, ptr %arr.elem68, align 8
  %arr.data.gep69 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data70 = load ptr, ptr %arr.data.gep69, align 8
  %arr.elem71 = getelementptr i64, ptr %arr.data70, i64 23
  store i64 0, ptr %arr.elem71, align 8
  %arr.data.gep72 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data73 = load ptr, ptr %arr.data.gep72, align 8
  %arr.elem74 = getelementptr i64, ptr %arr.data73, i64 24
  store i64 0, ptr %arr.elem74, align 8
  %arr.data.gep75 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data76 = load ptr, ptr %arr.data.gep75, align 8
  %arr.elem77 = getelementptr i64, ptr %arr.data76, i64 25
  store i64 0, ptr %arr.elem77, align 8
  %arr.data.gep78 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data79 = load ptr, ptr %arr.data.gep78, align 8
  %arr.elem80 = getelementptr i64, ptr %arr.data79, i64 26
  store i64 0, ptr %arr.elem80, align 8
  %arr.data.gep81 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data82 = load ptr, ptr %arr.data.gep81, align 8
  %arr.elem83 = getelementptr i64, ptr %arr.data82, i64 27
  store i64 0, ptr %arr.elem83, align 8
  %arr.data.gep84 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data85 = load ptr, ptr %arr.data.gep84, align 8
  %arr.elem86 = getelementptr i64, ptr %arr.data85, i64 28
  store i64 0, ptr %arr.elem86, align 8
  %arr.data.gep87 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data88 = load ptr, ptr %arr.data.gep87, align 8
  %arr.elem89 = getelementptr i64, ptr %arr.data88, i64 29
  store i64 0, ptr %arr.elem89, align 8
  %arr.data.gep90 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data91 = load ptr, ptr %arr.data.gep90, align 8
  %arr.elem92 = getelementptr i64, ptr %arr.data91, i64 30
  store i64 0, ptr %arr.elem92, align 8
  %arr.data.gep93 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data94 = load ptr, ptr %arr.data.gep93, align 8
  %arr.elem95 = getelementptr i64, ptr %arr.data94, i64 31
  store i64 0, ptr %arr.elem95, align 8
  %arr.data.gep96 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data97 = load ptr, ptr %arr.data.gep96, align 8
  %arr.elem98 = getelementptr i64, ptr %arr.data97, i64 32
  store i64 0, ptr %arr.elem98, align 8
  %arr.data.gep99 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data100 = load ptr, ptr %arr.data.gep99, align 8
  %arr.elem101 = getelementptr i64, ptr %arr.data100, i64 33
  store i64 0, ptr %arr.elem101, align 8
  %arr.data.gep102 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data103 = load ptr, ptr %arr.data.gep102, align 8
  %arr.elem104 = getelementptr i64, ptr %arr.data103, i64 34
  store i64 0, ptr %arr.elem104, align 8
  %arr.data.gep105 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data106 = load ptr, ptr %arr.data.gep105, align 8
  %arr.elem107 = getelementptr i64, ptr %arr.data106, i64 35
  store i64 0, ptr %arr.elem107, align 8
  %arr.data.gep108 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data109 = load ptr, ptr %arr.data.gep108, align 8
  %arr.elem110 = getelementptr i64, ptr %arr.data109, i64 36
  store i64 0, ptr %arr.elem110, align 8
  %arr.data.gep111 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data112 = load ptr, ptr %arr.data.gep111, align 8
  %arr.elem113 = getelementptr i64, ptr %arr.data112, i64 37
  store i64 0, ptr %arr.elem113, align 8
  %arr.data.gep114 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data115 = load ptr, ptr %arr.data.gep114, align 8
  %arr.elem116 = getelementptr i64, ptr %arr.data115, i64 38
  store i64 0, ptr %arr.elem116, align 8
  %arr.data.gep117 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data118 = load ptr, ptr %arr.data.gep117, align 8
  %arr.elem119 = getelementptr i64, ptr %arr.data118, i64 39
  store i64 0, ptr %arr.elem119, align 8
  %arr.data.gep120 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data121 = load ptr, ptr %arr.data.gep120, align 8
  %arr.elem122 = getelementptr i64, ptr %arr.data121, i64 40
  store i64 0, ptr %arr.elem122, align 8
  %arr.data.gep123 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data124 = load ptr, ptr %arr.data.gep123, align 8
  %arr.elem125 = getelementptr i64, ptr %arr.data124, i64 41
  store i64 0, ptr %arr.elem125, align 8
  %arr.data.gep126 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data127 = load ptr, ptr %arr.data.gep126, align 8
  %arr.elem128 = getelementptr i64, ptr %arr.data127, i64 42
  store i64 0, ptr %arr.elem128, align 8
  %arr.data.gep129 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data130 = load ptr, ptr %arr.data.gep129, align 8
  %arr.elem131 = getelementptr i64, ptr %arr.data130, i64 43
  store i64 0, ptr %arr.elem131, align 8
  %arr.data.gep132 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data133 = load ptr, ptr %arr.data.gep132, align 8
  %arr.elem134 = getelementptr i64, ptr %arr.data133, i64 44
  store i64 0, ptr %arr.elem134, align 8
  %arr.data.gep135 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data136 = load ptr, ptr %arr.data.gep135, align 8
  %arr.elem137 = getelementptr i64, ptr %arr.data136, i64 45
  store i64 0, ptr %arr.elem137, align 8
  %arr.data.gep138 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data139 = load ptr, ptr %arr.data.gep138, align 8
  %arr.elem140 = getelementptr i64, ptr %arr.data139, i64 46
  store i64 0, ptr %arr.elem140, align 8
  %arr.data.gep141 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data142 = load ptr, ptr %arr.data.gep141, align 8
  %arr.elem143 = getelementptr i64, ptr %arr.data142, i64 47
  store i64 0, ptr %arr.elem143, align 8
  %arr.data.gep144 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data145 = load ptr, ptr %arr.data.gep144, align 8
  %arr.elem146 = getelementptr i64, ptr %arr.data145, i64 48
  store i64 0, ptr %arr.elem146, align 8
  %arr.data.gep147 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data148 = load ptr, ptr %arr.data.gep147, align 8
  %arr.elem149 = getelementptr i64, ptr %arr.data148, i64 49
  store i64 0, ptr %arr.elem149, align 8
  %arr.data.gep150 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data151 = load ptr, ptr %arr.data.gep150, align 8
  %arr.elem152 = getelementptr i64, ptr %arr.data151, i64 50
  store i64 0, ptr %arr.elem152, align 8
  %arr.data.gep153 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data154 = load ptr, ptr %arr.data.gep153, align 8
  %arr.elem155 = getelementptr i64, ptr %arr.data154, i64 51
  store i64 0, ptr %arr.elem155, align 8
  %arr.data.gep156 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data157 = load ptr, ptr %arr.data.gep156, align 8
  %arr.elem158 = getelementptr i64, ptr %arr.data157, i64 52
  store i64 0, ptr %arr.elem158, align 8
  %arr.data.gep159 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data160 = load ptr, ptr %arr.data.gep159, align 8
  %arr.elem161 = getelementptr i64, ptr %arr.data160, i64 53
  store i64 0, ptr %arr.elem161, align 8
  %arr.data.gep162 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data163 = load ptr, ptr %arr.data.gep162, align 8
  %arr.elem164 = getelementptr i64, ptr %arr.data163, i64 54
  store i64 0, ptr %arr.elem164, align 8
  %arr.data.gep165 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data166 = load ptr, ptr %arr.data.gep165, align 8
  %arr.elem167 = getelementptr i64, ptr %arr.data166, i64 55
  store i64 0, ptr %arr.elem167, align 8
  %arr.data.gep168 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data169 = load ptr, ptr %arr.data.gep168, align 8
  %arr.elem170 = getelementptr i64, ptr %arr.data169, i64 56
  store i64 0, ptr %arr.elem170, align 8
  %arr.data.gep171 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data172 = load ptr, ptr %arr.data.gep171, align 8
  %arr.elem173 = getelementptr i64, ptr %arr.data172, i64 57
  store i64 0, ptr %arr.elem173, align 8
  %arr.data.gep174 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data175 = load ptr, ptr %arr.data.gep174, align 8
  %arr.elem176 = getelementptr i64, ptr %arr.data175, i64 58
  store i64 0, ptr %arr.elem176, align 8
  %arr.data.gep177 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data178 = load ptr, ptr %arr.data.gep177, align 8
  %arr.elem179 = getelementptr i64, ptr %arr.data178, i64 59
  store i64 0, ptr %arr.elem179, align 8
  %arr.data.gep180 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data181 = load ptr, ptr %arr.data.gep180, align 8
  %arr.elem182 = getelementptr i64, ptr %arr.data181, i64 60
  store i64 0, ptr %arr.elem182, align 8
  %arr.data.gep183 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data184 = load ptr, ptr %arr.data.gep183, align 8
  %arr.elem185 = getelementptr i64, ptr %arr.data184, i64 61
  store i64 0, ptr %arr.elem185, align 8
  %arr.data.gep186 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data187 = load ptr, ptr %arr.data.gep186, align 8
  %arr.elem188 = getelementptr i64, ptr %arr.data187, i64 62
  store i64 0, ptr %arr.elem188, align 8
  %arr.data.gep189 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data190 = load ptr, ptr %arr.data.gep189, align 8
  %arr.elem191 = getelementptr i64, ptr %arr.data190, i64 63
  store i64 0, ptr %arr.elem191, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%cache_\D8\AD\D8\A7\D9\84\D8\A9", align 8
  %arr_new192 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep193 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 0
  store i64 64, ptr %arr.len.gep193, align 8
  %arr.cap.gep194 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep194, align 8
  %arr.data195 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep196 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  store ptr %arr.data195, ptr %arr.data.gep196, align 8
  %arr.data.gep197 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data198 = load ptr, ptr %arr.data.gep197, align 8
  %arr.elem199 = getelementptr i64, ptr %arr.data198, i64 0
  store i64 0, ptr %arr.elem199, align 8
  %arr.data.gep200 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data201 = load ptr, ptr %arr.data.gep200, align 8
  %arr.elem202 = getelementptr i64, ptr %arr.data201, i64 1
  store i64 0, ptr %arr.elem202, align 8
  %arr.data.gep203 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data204 = load ptr, ptr %arr.data.gep203, align 8
  %arr.elem205 = getelementptr i64, ptr %arr.data204, i64 2
  store i64 0, ptr %arr.elem205, align 8
  %arr.data.gep206 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data207 = load ptr, ptr %arr.data.gep206, align 8
  %arr.elem208 = getelementptr i64, ptr %arr.data207, i64 3
  store i64 0, ptr %arr.elem208, align 8
  %arr.data.gep209 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data210 = load ptr, ptr %arr.data.gep209, align 8
  %arr.elem211 = getelementptr i64, ptr %arr.data210, i64 4
  store i64 0, ptr %arr.elem211, align 8
  %arr.data.gep212 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data213 = load ptr, ptr %arr.data.gep212, align 8
  %arr.elem214 = getelementptr i64, ptr %arr.data213, i64 5
  store i64 0, ptr %arr.elem214, align 8
  %arr.data.gep215 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data216 = load ptr, ptr %arr.data.gep215, align 8
  %arr.elem217 = getelementptr i64, ptr %arr.data216, i64 6
  store i64 0, ptr %arr.elem217, align 8
  %arr.data.gep218 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data219 = load ptr, ptr %arr.data.gep218, align 8
  %arr.elem220 = getelementptr i64, ptr %arr.data219, i64 7
  store i64 0, ptr %arr.elem220, align 8
  %arr.data.gep221 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data222 = load ptr, ptr %arr.data.gep221, align 8
  %arr.elem223 = getelementptr i64, ptr %arr.data222, i64 8
  store i64 0, ptr %arr.elem223, align 8
  %arr.data.gep224 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data225 = load ptr, ptr %arr.data.gep224, align 8
  %arr.elem226 = getelementptr i64, ptr %arr.data225, i64 9
  store i64 0, ptr %arr.elem226, align 8
  %arr.data.gep227 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data228 = load ptr, ptr %arr.data.gep227, align 8
  %arr.elem229 = getelementptr i64, ptr %arr.data228, i64 10
  store i64 0, ptr %arr.elem229, align 8
  %arr.data.gep230 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data231 = load ptr, ptr %arr.data.gep230, align 8
  %arr.elem232 = getelementptr i64, ptr %arr.data231, i64 11
  store i64 0, ptr %arr.elem232, align 8
  %arr.data.gep233 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data234 = load ptr, ptr %arr.data.gep233, align 8
  %arr.elem235 = getelementptr i64, ptr %arr.data234, i64 12
  store i64 0, ptr %arr.elem235, align 8
  %arr.data.gep236 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data237 = load ptr, ptr %arr.data.gep236, align 8
  %arr.elem238 = getelementptr i64, ptr %arr.data237, i64 13
  store i64 0, ptr %arr.elem238, align 8
  %arr.data.gep239 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data240 = load ptr, ptr %arr.data.gep239, align 8
  %arr.elem241 = getelementptr i64, ptr %arr.data240, i64 14
  store i64 0, ptr %arr.elem241, align 8
  %arr.data.gep242 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data243 = load ptr, ptr %arr.data.gep242, align 8
  %arr.elem244 = getelementptr i64, ptr %arr.data243, i64 15
  store i64 0, ptr %arr.elem244, align 8
  %arr.data.gep245 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data246 = load ptr, ptr %arr.data.gep245, align 8
  %arr.elem247 = getelementptr i64, ptr %arr.data246, i64 16
  store i64 0, ptr %arr.elem247, align 8
  %arr.data.gep248 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data249 = load ptr, ptr %arr.data.gep248, align 8
  %arr.elem250 = getelementptr i64, ptr %arr.data249, i64 17
  store i64 0, ptr %arr.elem250, align 8
  %arr.data.gep251 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data252 = load ptr, ptr %arr.data.gep251, align 8
  %arr.elem253 = getelementptr i64, ptr %arr.data252, i64 18
  store i64 0, ptr %arr.elem253, align 8
  %arr.data.gep254 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data255 = load ptr, ptr %arr.data.gep254, align 8
  %arr.elem256 = getelementptr i64, ptr %arr.data255, i64 19
  store i64 0, ptr %arr.elem256, align 8
  %arr.data.gep257 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data258 = load ptr, ptr %arr.data.gep257, align 8
  %arr.elem259 = getelementptr i64, ptr %arr.data258, i64 20
  store i64 0, ptr %arr.elem259, align 8
  %arr.data.gep260 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data261 = load ptr, ptr %arr.data.gep260, align 8
  %arr.elem262 = getelementptr i64, ptr %arr.data261, i64 21
  store i64 0, ptr %arr.elem262, align 8
  %arr.data.gep263 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data264 = load ptr, ptr %arr.data.gep263, align 8
  %arr.elem265 = getelementptr i64, ptr %arr.data264, i64 22
  store i64 0, ptr %arr.elem265, align 8
  %arr.data.gep266 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data267 = load ptr, ptr %arr.data.gep266, align 8
  %arr.elem268 = getelementptr i64, ptr %arr.data267, i64 23
  store i64 0, ptr %arr.elem268, align 8
  %arr.data.gep269 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data270 = load ptr, ptr %arr.data.gep269, align 8
  %arr.elem271 = getelementptr i64, ptr %arr.data270, i64 24
  store i64 0, ptr %arr.elem271, align 8
  %arr.data.gep272 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data273 = load ptr, ptr %arr.data.gep272, align 8
  %arr.elem274 = getelementptr i64, ptr %arr.data273, i64 25
  store i64 0, ptr %arr.elem274, align 8
  %arr.data.gep275 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data276 = load ptr, ptr %arr.data.gep275, align 8
  %arr.elem277 = getelementptr i64, ptr %arr.data276, i64 26
  store i64 0, ptr %arr.elem277, align 8
  %arr.data.gep278 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data279 = load ptr, ptr %arr.data.gep278, align 8
  %arr.elem280 = getelementptr i64, ptr %arr.data279, i64 27
  store i64 0, ptr %arr.elem280, align 8
  %arr.data.gep281 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data282 = load ptr, ptr %arr.data.gep281, align 8
  %arr.elem283 = getelementptr i64, ptr %arr.data282, i64 28
  store i64 0, ptr %arr.elem283, align 8
  %arr.data.gep284 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data285 = load ptr, ptr %arr.data.gep284, align 8
  %arr.elem286 = getelementptr i64, ptr %arr.data285, i64 29
  store i64 0, ptr %arr.elem286, align 8
  %arr.data.gep287 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data288 = load ptr, ptr %arr.data.gep287, align 8
  %arr.elem289 = getelementptr i64, ptr %arr.data288, i64 30
  store i64 0, ptr %arr.elem289, align 8
  %arr.data.gep290 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data291 = load ptr, ptr %arr.data.gep290, align 8
  %arr.elem292 = getelementptr i64, ptr %arr.data291, i64 31
  store i64 0, ptr %arr.elem292, align 8
  %arr.data.gep293 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data294 = load ptr, ptr %arr.data.gep293, align 8
  %arr.elem295 = getelementptr i64, ptr %arr.data294, i64 32
  store i64 0, ptr %arr.elem295, align 8
  %arr.data.gep296 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data297 = load ptr, ptr %arr.data.gep296, align 8
  %arr.elem298 = getelementptr i64, ptr %arr.data297, i64 33
  store i64 0, ptr %arr.elem298, align 8
  %arr.data.gep299 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data300 = load ptr, ptr %arr.data.gep299, align 8
  %arr.elem301 = getelementptr i64, ptr %arr.data300, i64 34
  store i64 0, ptr %arr.elem301, align 8
  %arr.data.gep302 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data303 = load ptr, ptr %arr.data.gep302, align 8
  %arr.elem304 = getelementptr i64, ptr %arr.data303, i64 35
  store i64 0, ptr %arr.elem304, align 8
  %arr.data.gep305 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data306 = load ptr, ptr %arr.data.gep305, align 8
  %arr.elem307 = getelementptr i64, ptr %arr.data306, i64 36
  store i64 0, ptr %arr.elem307, align 8
  %arr.data.gep308 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data309 = load ptr, ptr %arr.data.gep308, align 8
  %arr.elem310 = getelementptr i64, ptr %arr.data309, i64 37
  store i64 0, ptr %arr.elem310, align 8
  %arr.data.gep311 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data312 = load ptr, ptr %arr.data.gep311, align 8
  %arr.elem313 = getelementptr i64, ptr %arr.data312, i64 38
  store i64 0, ptr %arr.elem313, align 8
  %arr.data.gep314 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data315 = load ptr, ptr %arr.data.gep314, align 8
  %arr.elem316 = getelementptr i64, ptr %arr.data315, i64 39
  store i64 0, ptr %arr.elem316, align 8
  %arr.data.gep317 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data318 = load ptr, ptr %arr.data.gep317, align 8
  %arr.elem319 = getelementptr i64, ptr %arr.data318, i64 40
  store i64 0, ptr %arr.elem319, align 8
  %arr.data.gep320 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data321 = load ptr, ptr %arr.data.gep320, align 8
  %arr.elem322 = getelementptr i64, ptr %arr.data321, i64 41
  store i64 0, ptr %arr.elem322, align 8
  %arr.data.gep323 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data324 = load ptr, ptr %arr.data.gep323, align 8
  %arr.elem325 = getelementptr i64, ptr %arr.data324, i64 42
  store i64 0, ptr %arr.elem325, align 8
  %arr.data.gep326 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data327 = load ptr, ptr %arr.data.gep326, align 8
  %arr.elem328 = getelementptr i64, ptr %arr.data327, i64 43
  store i64 0, ptr %arr.elem328, align 8
  %arr.data.gep329 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data330 = load ptr, ptr %arr.data.gep329, align 8
  %arr.elem331 = getelementptr i64, ptr %arr.data330, i64 44
  store i64 0, ptr %arr.elem331, align 8
  %arr.data.gep332 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data333 = load ptr, ptr %arr.data.gep332, align 8
  %arr.elem334 = getelementptr i64, ptr %arr.data333, i64 45
  store i64 0, ptr %arr.elem334, align 8
  %arr.data.gep335 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data336 = load ptr, ptr %arr.data.gep335, align 8
  %arr.elem337 = getelementptr i64, ptr %arr.data336, i64 46
  store i64 0, ptr %arr.elem337, align 8
  %arr.data.gep338 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data339 = load ptr, ptr %arr.data.gep338, align 8
  %arr.elem340 = getelementptr i64, ptr %arr.data339, i64 47
  store i64 0, ptr %arr.elem340, align 8
  %arr.data.gep341 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data342 = load ptr, ptr %arr.data.gep341, align 8
  %arr.elem343 = getelementptr i64, ptr %arr.data342, i64 48
  store i64 0, ptr %arr.elem343, align 8
  %arr.data.gep344 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data345 = load ptr, ptr %arr.data.gep344, align 8
  %arr.elem346 = getelementptr i64, ptr %arr.data345, i64 49
  store i64 0, ptr %arr.elem346, align 8
  %arr.data.gep347 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data348 = load ptr, ptr %arr.data.gep347, align 8
  %arr.elem349 = getelementptr i64, ptr %arr.data348, i64 50
  store i64 0, ptr %arr.elem349, align 8
  %arr.data.gep350 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data351 = load ptr, ptr %arr.data.gep350, align 8
  %arr.elem352 = getelementptr i64, ptr %arr.data351, i64 51
  store i64 0, ptr %arr.elem352, align 8
  %arr.data.gep353 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data354 = load ptr, ptr %arr.data.gep353, align 8
  %arr.elem355 = getelementptr i64, ptr %arr.data354, i64 52
  store i64 0, ptr %arr.elem355, align 8
  %arr.data.gep356 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data357 = load ptr, ptr %arr.data.gep356, align 8
  %arr.elem358 = getelementptr i64, ptr %arr.data357, i64 53
  store i64 0, ptr %arr.elem358, align 8
  %arr.data.gep359 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data360 = load ptr, ptr %arr.data.gep359, align 8
  %arr.elem361 = getelementptr i64, ptr %arr.data360, i64 54
  store i64 0, ptr %arr.elem361, align 8
  %arr.data.gep362 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data363 = load ptr, ptr %arr.data.gep362, align 8
  %arr.elem364 = getelementptr i64, ptr %arr.data363, i64 55
  store i64 0, ptr %arr.elem364, align 8
  %arr.data.gep365 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data366 = load ptr, ptr %arr.data.gep365, align 8
  %arr.elem367 = getelementptr i64, ptr %arr.data366, i64 56
  store i64 0, ptr %arr.elem367, align 8
  %arr.data.gep368 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data369 = load ptr, ptr %arr.data.gep368, align 8
  %arr.elem370 = getelementptr i64, ptr %arr.data369, i64 57
  store i64 0, ptr %arr.elem370, align 8
  %arr.data.gep371 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data372 = load ptr, ptr %arr.data.gep371, align 8
  %arr.elem373 = getelementptr i64, ptr %arr.data372, i64 58
  store i64 0, ptr %arr.elem373, align 8
  %arr.data.gep374 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data375 = load ptr, ptr %arr.data.gep374, align 8
  %arr.elem376 = getelementptr i64, ptr %arr.data375, i64 59
  store i64 0, ptr %arr.elem376, align 8
  %arr.data.gep377 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data378 = load ptr, ptr %arr.data.gep377, align 8
  %arr.elem379 = getelementptr i64, ptr %arr.data378, i64 60
  store i64 0, ptr %arr.elem379, align 8
  %arr.data.gep380 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data381 = load ptr, ptr %arr.data.gep380, align 8
  %arr.elem382 = getelementptr i64, ptr %arr.data381, i64 61
  store i64 0, ptr %arr.elem382, align 8
  %arr.data.gep383 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data384 = load ptr, ptr %arr.data.gep383, align 8
  %arr.elem385 = getelementptr i64, ptr %arr.data384, i64 62
  store i64 0, ptr %arr.elem385, align 8
  %arr.data.gep386 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data387 = load ptr, ptr %arr.data.gep386, align 8
  %arr.elem388 = getelementptr i64, ptr %arr.data387, i64 63
  store i64 0, ptr %arr.elem388, align 8
  %obj.ptrtoint389 = ptrtoint ptr %arr_new192 to i64
  store i64 %obj.ptrtoint389, ptr %"%cache_\D9\82\D8\B1\D8\B5", align 8
  %arr_new390 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep391 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 0
  store i64 64, ptr %arr.len.gep391, align 8
  %arr.cap.gep392 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep392, align 8
  %arr.data393 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep394 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  store ptr %arr.data393, ptr %arr.data.gep394, align 8
  %arr.data.gep395 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data396 = load ptr, ptr %arr.data.gep395, align 8
  %arr.elem397 = getelementptr i64, ptr %arr.data396, i64 0
  store i64 0, ptr %arr.elem397, align 8
  %arr.data.gep398 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data399 = load ptr, ptr %arr.data.gep398, align 8
  %arr.elem400 = getelementptr i64, ptr %arr.data399, i64 1
  store i64 0, ptr %arr.elem400, align 8
  %arr.data.gep401 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data402 = load ptr, ptr %arr.data.gep401, align 8
  %arr.elem403 = getelementptr i64, ptr %arr.data402, i64 2
  store i64 0, ptr %arr.elem403, align 8
  %arr.data.gep404 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data405 = load ptr, ptr %arr.data.gep404, align 8
  %arr.elem406 = getelementptr i64, ptr %arr.data405, i64 3
  store i64 0, ptr %arr.elem406, align 8
  %arr.data.gep407 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data408 = load ptr, ptr %arr.data.gep407, align 8
  %arr.elem409 = getelementptr i64, ptr %arr.data408, i64 4
  store i64 0, ptr %arr.elem409, align 8
  %arr.data.gep410 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data411 = load ptr, ptr %arr.data.gep410, align 8
  %arr.elem412 = getelementptr i64, ptr %arr.data411, i64 5
  store i64 0, ptr %arr.elem412, align 8
  %arr.data.gep413 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data414 = load ptr, ptr %arr.data.gep413, align 8
  %arr.elem415 = getelementptr i64, ptr %arr.data414, i64 6
  store i64 0, ptr %arr.elem415, align 8
  %arr.data.gep416 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data417 = load ptr, ptr %arr.data.gep416, align 8
  %arr.elem418 = getelementptr i64, ptr %arr.data417, i64 7
  store i64 0, ptr %arr.elem418, align 8
  %arr.data.gep419 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data420 = load ptr, ptr %arr.data.gep419, align 8
  %arr.elem421 = getelementptr i64, ptr %arr.data420, i64 8
  store i64 0, ptr %arr.elem421, align 8
  %arr.data.gep422 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data423 = load ptr, ptr %arr.data.gep422, align 8
  %arr.elem424 = getelementptr i64, ptr %arr.data423, i64 9
  store i64 0, ptr %arr.elem424, align 8
  %arr.data.gep425 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data426 = load ptr, ptr %arr.data.gep425, align 8
  %arr.elem427 = getelementptr i64, ptr %arr.data426, i64 10
  store i64 0, ptr %arr.elem427, align 8
  %arr.data.gep428 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data429 = load ptr, ptr %arr.data.gep428, align 8
  %arr.elem430 = getelementptr i64, ptr %arr.data429, i64 11
  store i64 0, ptr %arr.elem430, align 8
  %arr.data.gep431 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data432 = load ptr, ptr %arr.data.gep431, align 8
  %arr.elem433 = getelementptr i64, ptr %arr.data432, i64 12
  store i64 0, ptr %arr.elem433, align 8
  %arr.data.gep434 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data435 = load ptr, ptr %arr.data.gep434, align 8
  %arr.elem436 = getelementptr i64, ptr %arr.data435, i64 13
  store i64 0, ptr %arr.elem436, align 8
  %arr.data.gep437 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data438 = load ptr, ptr %arr.data.gep437, align 8
  %arr.elem439 = getelementptr i64, ptr %arr.data438, i64 14
  store i64 0, ptr %arr.elem439, align 8
  %arr.data.gep440 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data441 = load ptr, ptr %arr.data.gep440, align 8
  %arr.elem442 = getelementptr i64, ptr %arr.data441, i64 15
  store i64 0, ptr %arr.elem442, align 8
  %arr.data.gep443 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data444 = load ptr, ptr %arr.data.gep443, align 8
  %arr.elem445 = getelementptr i64, ptr %arr.data444, i64 16
  store i64 0, ptr %arr.elem445, align 8
  %arr.data.gep446 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data447 = load ptr, ptr %arr.data.gep446, align 8
  %arr.elem448 = getelementptr i64, ptr %arr.data447, i64 17
  store i64 0, ptr %arr.elem448, align 8
  %arr.data.gep449 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data450 = load ptr, ptr %arr.data.gep449, align 8
  %arr.elem451 = getelementptr i64, ptr %arr.data450, i64 18
  store i64 0, ptr %arr.elem451, align 8
  %arr.data.gep452 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data453 = load ptr, ptr %arr.data.gep452, align 8
  %arr.elem454 = getelementptr i64, ptr %arr.data453, i64 19
  store i64 0, ptr %arr.elem454, align 8
  %arr.data.gep455 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data456 = load ptr, ptr %arr.data.gep455, align 8
  %arr.elem457 = getelementptr i64, ptr %arr.data456, i64 20
  store i64 0, ptr %arr.elem457, align 8
  %arr.data.gep458 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data459 = load ptr, ptr %arr.data.gep458, align 8
  %arr.elem460 = getelementptr i64, ptr %arr.data459, i64 21
  store i64 0, ptr %arr.elem460, align 8
  %arr.data.gep461 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data462 = load ptr, ptr %arr.data.gep461, align 8
  %arr.elem463 = getelementptr i64, ptr %arr.data462, i64 22
  store i64 0, ptr %arr.elem463, align 8
  %arr.data.gep464 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data465 = load ptr, ptr %arr.data.gep464, align 8
  %arr.elem466 = getelementptr i64, ptr %arr.data465, i64 23
  store i64 0, ptr %arr.elem466, align 8
  %arr.data.gep467 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data468 = load ptr, ptr %arr.data.gep467, align 8
  %arr.elem469 = getelementptr i64, ptr %arr.data468, i64 24
  store i64 0, ptr %arr.elem469, align 8
  %arr.data.gep470 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data471 = load ptr, ptr %arr.data.gep470, align 8
  %arr.elem472 = getelementptr i64, ptr %arr.data471, i64 25
  store i64 0, ptr %arr.elem472, align 8
  %arr.data.gep473 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data474 = load ptr, ptr %arr.data.gep473, align 8
  %arr.elem475 = getelementptr i64, ptr %arr.data474, i64 26
  store i64 0, ptr %arr.elem475, align 8
  %arr.data.gep476 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data477 = load ptr, ptr %arr.data.gep476, align 8
  %arr.elem478 = getelementptr i64, ptr %arr.data477, i64 27
  store i64 0, ptr %arr.elem478, align 8
  %arr.data.gep479 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data480 = load ptr, ptr %arr.data.gep479, align 8
  %arr.elem481 = getelementptr i64, ptr %arr.data480, i64 28
  store i64 0, ptr %arr.elem481, align 8
  %arr.data.gep482 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data483 = load ptr, ptr %arr.data.gep482, align 8
  %arr.elem484 = getelementptr i64, ptr %arr.data483, i64 29
  store i64 0, ptr %arr.elem484, align 8
  %arr.data.gep485 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data486 = load ptr, ptr %arr.data.gep485, align 8
  %arr.elem487 = getelementptr i64, ptr %arr.data486, i64 30
  store i64 0, ptr %arr.elem487, align 8
  %arr.data.gep488 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data489 = load ptr, ptr %arr.data.gep488, align 8
  %arr.elem490 = getelementptr i64, ptr %arr.data489, i64 31
  store i64 0, ptr %arr.elem490, align 8
  %arr.data.gep491 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data492 = load ptr, ptr %arr.data.gep491, align 8
  %arr.elem493 = getelementptr i64, ptr %arr.data492, i64 32
  store i64 0, ptr %arr.elem493, align 8
  %arr.data.gep494 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data495 = load ptr, ptr %arr.data.gep494, align 8
  %arr.elem496 = getelementptr i64, ptr %arr.data495, i64 33
  store i64 0, ptr %arr.elem496, align 8
  %arr.data.gep497 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data498 = load ptr, ptr %arr.data.gep497, align 8
  %arr.elem499 = getelementptr i64, ptr %arr.data498, i64 34
  store i64 0, ptr %arr.elem499, align 8
  %arr.data.gep500 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data501 = load ptr, ptr %arr.data.gep500, align 8
  %arr.elem502 = getelementptr i64, ptr %arr.data501, i64 35
  store i64 0, ptr %arr.elem502, align 8
  %arr.data.gep503 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data504 = load ptr, ptr %arr.data.gep503, align 8
  %arr.elem505 = getelementptr i64, ptr %arr.data504, i64 36
  store i64 0, ptr %arr.elem505, align 8
  %arr.data.gep506 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data507 = load ptr, ptr %arr.data.gep506, align 8
  %arr.elem508 = getelementptr i64, ptr %arr.data507, i64 37
  store i64 0, ptr %arr.elem508, align 8
  %arr.data.gep509 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data510 = load ptr, ptr %arr.data.gep509, align 8
  %arr.elem511 = getelementptr i64, ptr %arr.data510, i64 38
  store i64 0, ptr %arr.elem511, align 8
  %arr.data.gep512 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data513 = load ptr, ptr %arr.data.gep512, align 8
  %arr.elem514 = getelementptr i64, ptr %arr.data513, i64 39
  store i64 0, ptr %arr.elem514, align 8
  %arr.data.gep515 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data516 = load ptr, ptr %arr.data.gep515, align 8
  %arr.elem517 = getelementptr i64, ptr %arr.data516, i64 40
  store i64 0, ptr %arr.elem517, align 8
  %arr.data.gep518 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data519 = load ptr, ptr %arr.data.gep518, align 8
  %arr.elem520 = getelementptr i64, ptr %arr.data519, i64 41
  store i64 0, ptr %arr.elem520, align 8
  %arr.data.gep521 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data522 = load ptr, ptr %arr.data.gep521, align 8
  %arr.elem523 = getelementptr i64, ptr %arr.data522, i64 42
  store i64 0, ptr %arr.elem523, align 8
  %arr.data.gep524 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data525 = load ptr, ptr %arr.data.gep524, align 8
  %arr.elem526 = getelementptr i64, ptr %arr.data525, i64 43
  store i64 0, ptr %arr.elem526, align 8
  %arr.data.gep527 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data528 = load ptr, ptr %arr.data.gep527, align 8
  %arr.elem529 = getelementptr i64, ptr %arr.data528, i64 44
  store i64 0, ptr %arr.elem529, align 8
  %arr.data.gep530 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data531 = load ptr, ptr %arr.data.gep530, align 8
  %arr.elem532 = getelementptr i64, ptr %arr.data531, i64 45
  store i64 0, ptr %arr.elem532, align 8
  %arr.data.gep533 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data534 = load ptr, ptr %arr.data.gep533, align 8
  %arr.elem535 = getelementptr i64, ptr %arr.data534, i64 46
  store i64 0, ptr %arr.elem535, align 8
  %arr.data.gep536 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data537 = load ptr, ptr %arr.data.gep536, align 8
  %arr.elem538 = getelementptr i64, ptr %arr.data537, i64 47
  store i64 0, ptr %arr.elem538, align 8
  %arr.data.gep539 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data540 = load ptr, ptr %arr.data.gep539, align 8
  %arr.elem541 = getelementptr i64, ptr %arr.data540, i64 48
  store i64 0, ptr %arr.elem541, align 8
  %arr.data.gep542 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data543 = load ptr, ptr %arr.data.gep542, align 8
  %arr.elem544 = getelementptr i64, ptr %arr.data543, i64 49
  store i64 0, ptr %arr.elem544, align 8
  %arr.data.gep545 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data546 = load ptr, ptr %arr.data.gep545, align 8
  %arr.elem547 = getelementptr i64, ptr %arr.data546, i64 50
  store i64 0, ptr %arr.elem547, align 8
  %arr.data.gep548 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data549 = load ptr, ptr %arr.data.gep548, align 8
  %arr.elem550 = getelementptr i64, ptr %arr.data549, i64 51
  store i64 0, ptr %arr.elem550, align 8
  %arr.data.gep551 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data552 = load ptr, ptr %arr.data.gep551, align 8
  %arr.elem553 = getelementptr i64, ptr %arr.data552, i64 52
  store i64 0, ptr %arr.elem553, align 8
  %arr.data.gep554 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data555 = load ptr, ptr %arr.data.gep554, align 8
  %arr.elem556 = getelementptr i64, ptr %arr.data555, i64 53
  store i64 0, ptr %arr.elem556, align 8
  %arr.data.gep557 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data558 = load ptr, ptr %arr.data.gep557, align 8
  %arr.elem559 = getelementptr i64, ptr %arr.data558, i64 54
  store i64 0, ptr %arr.elem559, align 8
  %arr.data.gep560 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data561 = load ptr, ptr %arr.data.gep560, align 8
  %arr.elem562 = getelementptr i64, ptr %arr.data561, i64 55
  store i64 0, ptr %arr.elem562, align 8
  %arr.data.gep563 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data564 = load ptr, ptr %arr.data.gep563, align 8
  %arr.elem565 = getelementptr i64, ptr %arr.data564, i64 56
  store i64 0, ptr %arr.elem565, align 8
  %arr.data.gep566 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data567 = load ptr, ptr %arr.data.gep566, align 8
  %arr.elem568 = getelementptr i64, ptr %arr.data567, i64 57
  store i64 0, ptr %arr.elem568, align 8
  %arr.data.gep569 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data570 = load ptr, ptr %arr.data.gep569, align 8
  %arr.elem571 = getelementptr i64, ptr %arr.data570, i64 58
  store i64 0, ptr %arr.elem571, align 8
  %arr.data.gep572 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data573 = load ptr, ptr %arr.data.gep572, align 8
  %arr.elem574 = getelementptr i64, ptr %arr.data573, i64 59
  store i64 0, ptr %arr.elem574, align 8
  %arr.data.gep575 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data576 = load ptr, ptr %arr.data.gep575, align 8
  %arr.elem577 = getelementptr i64, ptr %arr.data576, i64 60
  store i64 0, ptr %arr.elem577, align 8
  %arr.data.gep578 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data579 = load ptr, ptr %arr.data.gep578, align 8
  %arr.elem580 = getelementptr i64, ptr %arr.data579, i64 61
  store i64 0, ptr %arr.elem580, align 8
  %arr.data.gep581 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data582 = load ptr, ptr %arr.data.gep581, align 8
  %arr.elem583 = getelementptr i64, ptr %arr.data582, i64 62
  store i64 0, ptr %arr.elem583, align 8
  %arr.data.gep584 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data585 = load ptr, ptr %arr.data.gep584, align 8
  %arr.elem586 = getelementptr i64, ptr %arr.data585, i64 63
  store i64 0, ptr %arr.elem586, align 8
  %obj.ptrtoint587 = ptrtoint ptr %arr_new390 to i64
  store i64 %obj.ptrtoint587, ptr %"%cache_lba", align 8
  %arr_new588 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep589 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 0
  store i64 64, ptr %arr.len.gep589, align 8
  %arr.cap.gep590 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep590, align 8
  %arr.data591 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep592 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  store ptr %arr.data591, ptr %arr.data.gep592, align 8
  %arr.data.gep593 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data594 = load ptr, ptr %arr.data.gep593, align 8
  %arr.elem595 = getelementptr i64, ptr %arr.data594, i64 0
  store i64 0, ptr %arr.elem595, align 8
  %arr.data.gep596 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data597 = load ptr, ptr %arr.data.gep596, align 8
  %arr.elem598 = getelementptr i64, ptr %arr.data597, i64 1
  store i64 0, ptr %arr.elem598, align 8
  %arr.data.gep599 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data600 = load ptr, ptr %arr.data.gep599, align 8
  %arr.elem601 = getelementptr i64, ptr %arr.data600, i64 2
  store i64 0, ptr %arr.elem601, align 8
  %arr.data.gep602 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data603 = load ptr, ptr %arr.data.gep602, align 8
  %arr.elem604 = getelementptr i64, ptr %arr.data603, i64 3
  store i64 0, ptr %arr.elem604, align 8
  %arr.data.gep605 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data606 = load ptr, ptr %arr.data.gep605, align 8
  %arr.elem607 = getelementptr i64, ptr %arr.data606, i64 4
  store i64 0, ptr %arr.elem607, align 8
  %arr.data.gep608 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data609 = load ptr, ptr %arr.data.gep608, align 8
  %arr.elem610 = getelementptr i64, ptr %arr.data609, i64 5
  store i64 0, ptr %arr.elem610, align 8
  %arr.data.gep611 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data612 = load ptr, ptr %arr.data.gep611, align 8
  %arr.elem613 = getelementptr i64, ptr %arr.data612, i64 6
  store i64 0, ptr %arr.elem613, align 8
  %arr.data.gep614 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data615 = load ptr, ptr %arr.data.gep614, align 8
  %arr.elem616 = getelementptr i64, ptr %arr.data615, i64 7
  store i64 0, ptr %arr.elem616, align 8
  %arr.data.gep617 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data618 = load ptr, ptr %arr.data.gep617, align 8
  %arr.elem619 = getelementptr i64, ptr %arr.data618, i64 8
  store i64 0, ptr %arr.elem619, align 8
  %arr.data.gep620 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data621 = load ptr, ptr %arr.data.gep620, align 8
  %arr.elem622 = getelementptr i64, ptr %arr.data621, i64 9
  store i64 0, ptr %arr.elem622, align 8
  %arr.data.gep623 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data624 = load ptr, ptr %arr.data.gep623, align 8
  %arr.elem625 = getelementptr i64, ptr %arr.data624, i64 10
  store i64 0, ptr %arr.elem625, align 8
  %arr.data.gep626 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data627 = load ptr, ptr %arr.data.gep626, align 8
  %arr.elem628 = getelementptr i64, ptr %arr.data627, i64 11
  store i64 0, ptr %arr.elem628, align 8
  %arr.data.gep629 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data630 = load ptr, ptr %arr.data.gep629, align 8
  %arr.elem631 = getelementptr i64, ptr %arr.data630, i64 12
  store i64 0, ptr %arr.elem631, align 8
  %arr.data.gep632 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data633 = load ptr, ptr %arr.data.gep632, align 8
  %arr.elem634 = getelementptr i64, ptr %arr.data633, i64 13
  store i64 0, ptr %arr.elem634, align 8
  %arr.data.gep635 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data636 = load ptr, ptr %arr.data.gep635, align 8
  %arr.elem637 = getelementptr i64, ptr %arr.data636, i64 14
  store i64 0, ptr %arr.elem637, align 8
  %arr.data.gep638 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data639 = load ptr, ptr %arr.data.gep638, align 8
  %arr.elem640 = getelementptr i64, ptr %arr.data639, i64 15
  store i64 0, ptr %arr.elem640, align 8
  %arr.data.gep641 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data642 = load ptr, ptr %arr.data.gep641, align 8
  %arr.elem643 = getelementptr i64, ptr %arr.data642, i64 16
  store i64 0, ptr %arr.elem643, align 8
  %arr.data.gep644 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data645 = load ptr, ptr %arr.data.gep644, align 8
  %arr.elem646 = getelementptr i64, ptr %arr.data645, i64 17
  store i64 0, ptr %arr.elem646, align 8
  %arr.data.gep647 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data648 = load ptr, ptr %arr.data.gep647, align 8
  %arr.elem649 = getelementptr i64, ptr %arr.data648, i64 18
  store i64 0, ptr %arr.elem649, align 8
  %arr.data.gep650 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data651 = load ptr, ptr %arr.data.gep650, align 8
  %arr.elem652 = getelementptr i64, ptr %arr.data651, i64 19
  store i64 0, ptr %arr.elem652, align 8
  %arr.data.gep653 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data654 = load ptr, ptr %arr.data.gep653, align 8
  %arr.elem655 = getelementptr i64, ptr %arr.data654, i64 20
  store i64 0, ptr %arr.elem655, align 8
  %arr.data.gep656 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data657 = load ptr, ptr %arr.data.gep656, align 8
  %arr.elem658 = getelementptr i64, ptr %arr.data657, i64 21
  store i64 0, ptr %arr.elem658, align 8
  %arr.data.gep659 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data660 = load ptr, ptr %arr.data.gep659, align 8
  %arr.elem661 = getelementptr i64, ptr %arr.data660, i64 22
  store i64 0, ptr %arr.elem661, align 8
  %arr.data.gep662 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data663 = load ptr, ptr %arr.data.gep662, align 8
  %arr.elem664 = getelementptr i64, ptr %arr.data663, i64 23
  store i64 0, ptr %arr.elem664, align 8
  %arr.data.gep665 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data666 = load ptr, ptr %arr.data.gep665, align 8
  %arr.elem667 = getelementptr i64, ptr %arr.data666, i64 24
  store i64 0, ptr %arr.elem667, align 8
  %arr.data.gep668 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data669 = load ptr, ptr %arr.data.gep668, align 8
  %arr.elem670 = getelementptr i64, ptr %arr.data669, i64 25
  store i64 0, ptr %arr.elem670, align 8
  %arr.data.gep671 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data672 = load ptr, ptr %arr.data.gep671, align 8
  %arr.elem673 = getelementptr i64, ptr %arr.data672, i64 26
  store i64 0, ptr %arr.elem673, align 8
  %arr.data.gep674 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data675 = load ptr, ptr %arr.data.gep674, align 8
  %arr.elem676 = getelementptr i64, ptr %arr.data675, i64 27
  store i64 0, ptr %arr.elem676, align 8
  %arr.data.gep677 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data678 = load ptr, ptr %arr.data.gep677, align 8
  %arr.elem679 = getelementptr i64, ptr %arr.data678, i64 28
  store i64 0, ptr %arr.elem679, align 8
  %arr.data.gep680 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data681 = load ptr, ptr %arr.data.gep680, align 8
  %arr.elem682 = getelementptr i64, ptr %arr.data681, i64 29
  store i64 0, ptr %arr.elem682, align 8
  %arr.data.gep683 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data684 = load ptr, ptr %arr.data.gep683, align 8
  %arr.elem685 = getelementptr i64, ptr %arr.data684, i64 30
  store i64 0, ptr %arr.elem685, align 8
  %arr.data.gep686 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data687 = load ptr, ptr %arr.data.gep686, align 8
  %arr.elem688 = getelementptr i64, ptr %arr.data687, i64 31
  store i64 0, ptr %arr.elem688, align 8
  %arr.data.gep689 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data690 = load ptr, ptr %arr.data.gep689, align 8
  %arr.elem691 = getelementptr i64, ptr %arr.data690, i64 32
  store i64 0, ptr %arr.elem691, align 8
  %arr.data.gep692 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data693 = load ptr, ptr %arr.data.gep692, align 8
  %arr.elem694 = getelementptr i64, ptr %arr.data693, i64 33
  store i64 0, ptr %arr.elem694, align 8
  %arr.data.gep695 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data696 = load ptr, ptr %arr.data.gep695, align 8
  %arr.elem697 = getelementptr i64, ptr %arr.data696, i64 34
  store i64 0, ptr %arr.elem697, align 8
  %arr.data.gep698 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data699 = load ptr, ptr %arr.data.gep698, align 8
  %arr.elem700 = getelementptr i64, ptr %arr.data699, i64 35
  store i64 0, ptr %arr.elem700, align 8
  %arr.data.gep701 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data702 = load ptr, ptr %arr.data.gep701, align 8
  %arr.elem703 = getelementptr i64, ptr %arr.data702, i64 36
  store i64 0, ptr %arr.elem703, align 8
  %arr.data.gep704 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data705 = load ptr, ptr %arr.data.gep704, align 8
  %arr.elem706 = getelementptr i64, ptr %arr.data705, i64 37
  store i64 0, ptr %arr.elem706, align 8
  %arr.data.gep707 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data708 = load ptr, ptr %arr.data.gep707, align 8
  %arr.elem709 = getelementptr i64, ptr %arr.data708, i64 38
  store i64 0, ptr %arr.elem709, align 8
  %arr.data.gep710 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data711 = load ptr, ptr %arr.data.gep710, align 8
  %arr.elem712 = getelementptr i64, ptr %arr.data711, i64 39
  store i64 0, ptr %arr.elem712, align 8
  %arr.data.gep713 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data714 = load ptr, ptr %arr.data.gep713, align 8
  %arr.elem715 = getelementptr i64, ptr %arr.data714, i64 40
  store i64 0, ptr %arr.elem715, align 8
  %arr.data.gep716 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data717 = load ptr, ptr %arr.data.gep716, align 8
  %arr.elem718 = getelementptr i64, ptr %arr.data717, i64 41
  store i64 0, ptr %arr.elem718, align 8
  %arr.data.gep719 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data720 = load ptr, ptr %arr.data.gep719, align 8
  %arr.elem721 = getelementptr i64, ptr %arr.data720, i64 42
  store i64 0, ptr %arr.elem721, align 8
  %arr.data.gep722 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data723 = load ptr, ptr %arr.data.gep722, align 8
  %arr.elem724 = getelementptr i64, ptr %arr.data723, i64 43
  store i64 0, ptr %arr.elem724, align 8
  %arr.data.gep725 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data726 = load ptr, ptr %arr.data.gep725, align 8
  %arr.elem727 = getelementptr i64, ptr %arr.data726, i64 44
  store i64 0, ptr %arr.elem727, align 8
  %arr.data.gep728 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data729 = load ptr, ptr %arr.data.gep728, align 8
  %arr.elem730 = getelementptr i64, ptr %arr.data729, i64 45
  store i64 0, ptr %arr.elem730, align 8
  %arr.data.gep731 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data732 = load ptr, ptr %arr.data.gep731, align 8
  %arr.elem733 = getelementptr i64, ptr %arr.data732, i64 46
  store i64 0, ptr %arr.elem733, align 8
  %arr.data.gep734 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data735 = load ptr, ptr %arr.data.gep734, align 8
  %arr.elem736 = getelementptr i64, ptr %arr.data735, i64 47
  store i64 0, ptr %arr.elem736, align 8
  %arr.data.gep737 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data738 = load ptr, ptr %arr.data.gep737, align 8
  %arr.elem739 = getelementptr i64, ptr %arr.data738, i64 48
  store i64 0, ptr %arr.elem739, align 8
  %arr.data.gep740 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data741 = load ptr, ptr %arr.data.gep740, align 8
  %arr.elem742 = getelementptr i64, ptr %arr.data741, i64 49
  store i64 0, ptr %arr.elem742, align 8
  %arr.data.gep743 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data744 = load ptr, ptr %arr.data.gep743, align 8
  %arr.elem745 = getelementptr i64, ptr %arr.data744, i64 50
  store i64 0, ptr %arr.elem745, align 8
  %arr.data.gep746 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data747 = load ptr, ptr %arr.data.gep746, align 8
  %arr.elem748 = getelementptr i64, ptr %arr.data747, i64 51
  store i64 0, ptr %arr.elem748, align 8
  %arr.data.gep749 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data750 = load ptr, ptr %arr.data.gep749, align 8
  %arr.elem751 = getelementptr i64, ptr %arr.data750, i64 52
  store i64 0, ptr %arr.elem751, align 8
  %arr.data.gep752 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data753 = load ptr, ptr %arr.data.gep752, align 8
  %arr.elem754 = getelementptr i64, ptr %arr.data753, i64 53
  store i64 0, ptr %arr.elem754, align 8
  %arr.data.gep755 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data756 = load ptr, ptr %arr.data.gep755, align 8
  %arr.elem757 = getelementptr i64, ptr %arr.data756, i64 54
  store i64 0, ptr %arr.elem757, align 8
  %arr.data.gep758 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data759 = load ptr, ptr %arr.data.gep758, align 8
  %arr.elem760 = getelementptr i64, ptr %arr.data759, i64 55
  store i64 0, ptr %arr.elem760, align 8
  %arr.data.gep761 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data762 = load ptr, ptr %arr.data.gep761, align 8
  %arr.elem763 = getelementptr i64, ptr %arr.data762, i64 56
  store i64 0, ptr %arr.elem763, align 8
  %arr.data.gep764 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data765 = load ptr, ptr %arr.data.gep764, align 8
  %arr.elem766 = getelementptr i64, ptr %arr.data765, i64 57
  store i64 0, ptr %arr.elem766, align 8
  %arr.data.gep767 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data768 = load ptr, ptr %arr.data.gep767, align 8
  %arr.elem769 = getelementptr i64, ptr %arr.data768, i64 58
  store i64 0, ptr %arr.elem769, align 8
  %arr.data.gep770 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data771 = load ptr, ptr %arr.data.gep770, align 8
  %arr.elem772 = getelementptr i64, ptr %arr.data771, i64 59
  store i64 0, ptr %arr.elem772, align 8
  %arr.data.gep773 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data774 = load ptr, ptr %arr.data.gep773, align 8
  %arr.elem775 = getelementptr i64, ptr %arr.data774, i64 60
  store i64 0, ptr %arr.elem775, align 8
  %arr.data.gep776 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data777 = load ptr, ptr %arr.data.gep776, align 8
  %arr.elem778 = getelementptr i64, ptr %arr.data777, i64 61
  store i64 0, ptr %arr.elem778, align 8
  %arr.data.gep779 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data780 = load ptr, ptr %arr.data.gep779, align 8
  %arr.elem781 = getelementptr i64, ptr %arr.data780, i64 62
  store i64 0, ptr %arr.elem781, align 8
  %arr.data.gep782 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data783 = load ptr, ptr %arr.data.gep782, align 8
  %arr.elem784 = getelementptr i64, ptr %arr.data783, i64 63
  store i64 0, ptr %arr.elem784, align 8
  %obj.ptrtoint785 = ptrtoint ptr %arr_new588 to i64
  store i64 %obj.ptrtoint785, ptr %"%cache_\D8\A7\D8\B3\D8\AA\D8\AE\D8\AF\D8\A7\D9\85", align 8
  %arr_new786 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep787 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 0
  store i64 64, ptr %arr.len.gep787, align 8
  %arr.cap.gep788 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep788, align 8
  %arr.data789 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep790 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  store ptr %arr.data789, ptr %arr.data.gep790, align 8
  %arr.data.gep791 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data792 = load ptr, ptr %arr.data.gep791, align 8
  %arr.elem793 = getelementptr i64, ptr %arr.data792, i64 0
  store i64 0, ptr %arr.elem793, align 8
  %arr.data.gep794 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data795 = load ptr, ptr %arr.data.gep794, align 8
  %arr.elem796 = getelementptr i64, ptr %arr.data795, i64 1
  store i64 0, ptr %arr.elem796, align 8
  %arr.data.gep797 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data798 = load ptr, ptr %arr.data.gep797, align 8
  %arr.elem799 = getelementptr i64, ptr %arr.data798, i64 2
  store i64 0, ptr %arr.elem799, align 8
  %arr.data.gep800 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data801 = load ptr, ptr %arr.data.gep800, align 8
  %arr.elem802 = getelementptr i64, ptr %arr.data801, i64 3
  store i64 0, ptr %arr.elem802, align 8
  %arr.data.gep803 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data804 = load ptr, ptr %arr.data.gep803, align 8
  %arr.elem805 = getelementptr i64, ptr %arr.data804, i64 4
  store i64 0, ptr %arr.elem805, align 8
  %arr.data.gep806 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data807 = load ptr, ptr %arr.data.gep806, align 8
  %arr.elem808 = getelementptr i64, ptr %arr.data807, i64 5
  store i64 0, ptr %arr.elem808, align 8
  %arr.data.gep809 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data810 = load ptr, ptr %arr.data.gep809, align 8
  %arr.elem811 = getelementptr i64, ptr %arr.data810, i64 6
  store i64 0, ptr %arr.elem811, align 8
  %arr.data.gep812 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data813 = load ptr, ptr %arr.data.gep812, align 8
  %arr.elem814 = getelementptr i64, ptr %arr.data813, i64 7
  store i64 0, ptr %arr.elem814, align 8
  %arr.data.gep815 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data816 = load ptr, ptr %arr.data.gep815, align 8
  %arr.elem817 = getelementptr i64, ptr %arr.data816, i64 8
  store i64 0, ptr %arr.elem817, align 8
  %arr.data.gep818 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data819 = load ptr, ptr %arr.data.gep818, align 8
  %arr.elem820 = getelementptr i64, ptr %arr.data819, i64 9
  store i64 0, ptr %arr.elem820, align 8
  %arr.data.gep821 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data822 = load ptr, ptr %arr.data.gep821, align 8
  %arr.elem823 = getelementptr i64, ptr %arr.data822, i64 10
  store i64 0, ptr %arr.elem823, align 8
  %arr.data.gep824 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data825 = load ptr, ptr %arr.data.gep824, align 8
  %arr.elem826 = getelementptr i64, ptr %arr.data825, i64 11
  store i64 0, ptr %arr.elem826, align 8
  %arr.data.gep827 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data828 = load ptr, ptr %arr.data.gep827, align 8
  %arr.elem829 = getelementptr i64, ptr %arr.data828, i64 12
  store i64 0, ptr %arr.elem829, align 8
  %arr.data.gep830 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data831 = load ptr, ptr %arr.data.gep830, align 8
  %arr.elem832 = getelementptr i64, ptr %arr.data831, i64 13
  store i64 0, ptr %arr.elem832, align 8
  %arr.data.gep833 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data834 = load ptr, ptr %arr.data.gep833, align 8
  %arr.elem835 = getelementptr i64, ptr %arr.data834, i64 14
  store i64 0, ptr %arr.elem835, align 8
  %arr.data.gep836 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data837 = load ptr, ptr %arr.data.gep836, align 8
  %arr.elem838 = getelementptr i64, ptr %arr.data837, i64 15
  store i64 0, ptr %arr.elem838, align 8
  %arr.data.gep839 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data840 = load ptr, ptr %arr.data.gep839, align 8
  %arr.elem841 = getelementptr i64, ptr %arr.data840, i64 16
  store i64 0, ptr %arr.elem841, align 8
  %arr.data.gep842 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data843 = load ptr, ptr %arr.data.gep842, align 8
  %arr.elem844 = getelementptr i64, ptr %arr.data843, i64 17
  store i64 0, ptr %arr.elem844, align 8
  %arr.data.gep845 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data846 = load ptr, ptr %arr.data.gep845, align 8
  %arr.elem847 = getelementptr i64, ptr %arr.data846, i64 18
  store i64 0, ptr %arr.elem847, align 8
  %arr.data.gep848 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data849 = load ptr, ptr %arr.data.gep848, align 8
  %arr.elem850 = getelementptr i64, ptr %arr.data849, i64 19
  store i64 0, ptr %arr.elem850, align 8
  %arr.data.gep851 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data852 = load ptr, ptr %arr.data.gep851, align 8
  %arr.elem853 = getelementptr i64, ptr %arr.data852, i64 20
  store i64 0, ptr %arr.elem853, align 8
  %arr.data.gep854 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data855 = load ptr, ptr %arr.data.gep854, align 8
  %arr.elem856 = getelementptr i64, ptr %arr.data855, i64 21
  store i64 0, ptr %arr.elem856, align 8
  %arr.data.gep857 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data858 = load ptr, ptr %arr.data.gep857, align 8
  %arr.elem859 = getelementptr i64, ptr %arr.data858, i64 22
  store i64 0, ptr %arr.elem859, align 8
  %arr.data.gep860 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data861 = load ptr, ptr %arr.data.gep860, align 8
  %arr.elem862 = getelementptr i64, ptr %arr.data861, i64 23
  store i64 0, ptr %arr.elem862, align 8
  %arr.data.gep863 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data864 = load ptr, ptr %arr.data.gep863, align 8
  %arr.elem865 = getelementptr i64, ptr %arr.data864, i64 24
  store i64 0, ptr %arr.elem865, align 8
  %arr.data.gep866 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data867 = load ptr, ptr %arr.data.gep866, align 8
  %arr.elem868 = getelementptr i64, ptr %arr.data867, i64 25
  store i64 0, ptr %arr.elem868, align 8
  %arr.data.gep869 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data870 = load ptr, ptr %arr.data.gep869, align 8
  %arr.elem871 = getelementptr i64, ptr %arr.data870, i64 26
  store i64 0, ptr %arr.elem871, align 8
  %arr.data.gep872 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data873 = load ptr, ptr %arr.data.gep872, align 8
  %arr.elem874 = getelementptr i64, ptr %arr.data873, i64 27
  store i64 0, ptr %arr.elem874, align 8
  %arr.data.gep875 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data876 = load ptr, ptr %arr.data.gep875, align 8
  %arr.elem877 = getelementptr i64, ptr %arr.data876, i64 28
  store i64 0, ptr %arr.elem877, align 8
  %arr.data.gep878 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data879 = load ptr, ptr %arr.data.gep878, align 8
  %arr.elem880 = getelementptr i64, ptr %arr.data879, i64 29
  store i64 0, ptr %arr.elem880, align 8
  %arr.data.gep881 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data882 = load ptr, ptr %arr.data.gep881, align 8
  %arr.elem883 = getelementptr i64, ptr %arr.data882, i64 30
  store i64 0, ptr %arr.elem883, align 8
  %arr.data.gep884 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data885 = load ptr, ptr %arr.data.gep884, align 8
  %arr.elem886 = getelementptr i64, ptr %arr.data885, i64 31
  store i64 0, ptr %arr.elem886, align 8
  %arr.data.gep887 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data888 = load ptr, ptr %arr.data.gep887, align 8
  %arr.elem889 = getelementptr i64, ptr %arr.data888, i64 32
  store i64 0, ptr %arr.elem889, align 8
  %arr.data.gep890 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data891 = load ptr, ptr %arr.data.gep890, align 8
  %arr.elem892 = getelementptr i64, ptr %arr.data891, i64 33
  store i64 0, ptr %arr.elem892, align 8
  %arr.data.gep893 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data894 = load ptr, ptr %arr.data.gep893, align 8
  %arr.elem895 = getelementptr i64, ptr %arr.data894, i64 34
  store i64 0, ptr %arr.elem895, align 8
  %arr.data.gep896 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data897 = load ptr, ptr %arr.data.gep896, align 8
  %arr.elem898 = getelementptr i64, ptr %arr.data897, i64 35
  store i64 0, ptr %arr.elem898, align 8
  %arr.data.gep899 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data900 = load ptr, ptr %arr.data.gep899, align 8
  %arr.elem901 = getelementptr i64, ptr %arr.data900, i64 36
  store i64 0, ptr %arr.elem901, align 8
  %arr.data.gep902 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data903 = load ptr, ptr %arr.data.gep902, align 8
  %arr.elem904 = getelementptr i64, ptr %arr.data903, i64 37
  store i64 0, ptr %arr.elem904, align 8
  %arr.data.gep905 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data906 = load ptr, ptr %arr.data.gep905, align 8
  %arr.elem907 = getelementptr i64, ptr %arr.data906, i64 38
  store i64 0, ptr %arr.elem907, align 8
  %arr.data.gep908 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data909 = load ptr, ptr %arr.data.gep908, align 8
  %arr.elem910 = getelementptr i64, ptr %arr.data909, i64 39
  store i64 0, ptr %arr.elem910, align 8
  %arr.data.gep911 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data912 = load ptr, ptr %arr.data.gep911, align 8
  %arr.elem913 = getelementptr i64, ptr %arr.data912, i64 40
  store i64 0, ptr %arr.elem913, align 8
  %arr.data.gep914 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data915 = load ptr, ptr %arr.data.gep914, align 8
  %arr.elem916 = getelementptr i64, ptr %arr.data915, i64 41
  store i64 0, ptr %arr.elem916, align 8
  %arr.data.gep917 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data918 = load ptr, ptr %arr.data.gep917, align 8
  %arr.elem919 = getelementptr i64, ptr %arr.data918, i64 42
  store i64 0, ptr %arr.elem919, align 8
  %arr.data.gep920 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data921 = load ptr, ptr %arr.data.gep920, align 8
  %arr.elem922 = getelementptr i64, ptr %arr.data921, i64 43
  store i64 0, ptr %arr.elem922, align 8
  %arr.data.gep923 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data924 = load ptr, ptr %arr.data.gep923, align 8
  %arr.elem925 = getelementptr i64, ptr %arr.data924, i64 44
  store i64 0, ptr %arr.elem925, align 8
  %arr.data.gep926 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data927 = load ptr, ptr %arr.data.gep926, align 8
  %arr.elem928 = getelementptr i64, ptr %arr.data927, i64 45
  store i64 0, ptr %arr.elem928, align 8
  %arr.data.gep929 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data930 = load ptr, ptr %arr.data.gep929, align 8
  %arr.elem931 = getelementptr i64, ptr %arr.data930, i64 46
  store i64 0, ptr %arr.elem931, align 8
  %arr.data.gep932 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data933 = load ptr, ptr %arr.data.gep932, align 8
  %arr.elem934 = getelementptr i64, ptr %arr.data933, i64 47
  store i64 0, ptr %arr.elem934, align 8
  %arr.data.gep935 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data936 = load ptr, ptr %arr.data.gep935, align 8
  %arr.elem937 = getelementptr i64, ptr %arr.data936, i64 48
  store i64 0, ptr %arr.elem937, align 8
  %arr.data.gep938 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data939 = load ptr, ptr %arr.data.gep938, align 8
  %arr.elem940 = getelementptr i64, ptr %arr.data939, i64 49
  store i64 0, ptr %arr.elem940, align 8
  %arr.data.gep941 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data942 = load ptr, ptr %arr.data.gep941, align 8
  %arr.elem943 = getelementptr i64, ptr %arr.data942, i64 50
  store i64 0, ptr %arr.elem943, align 8
  %arr.data.gep944 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data945 = load ptr, ptr %arr.data.gep944, align 8
  %arr.elem946 = getelementptr i64, ptr %arr.data945, i64 51
  store i64 0, ptr %arr.elem946, align 8
  %arr.data.gep947 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data948 = load ptr, ptr %arr.data.gep947, align 8
  %arr.elem949 = getelementptr i64, ptr %arr.data948, i64 52
  store i64 0, ptr %arr.elem949, align 8
  %arr.data.gep950 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data951 = load ptr, ptr %arr.data.gep950, align 8
  %arr.elem952 = getelementptr i64, ptr %arr.data951, i64 53
  store i64 0, ptr %arr.elem952, align 8
  %arr.data.gep953 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data954 = load ptr, ptr %arr.data.gep953, align 8
  %arr.elem955 = getelementptr i64, ptr %arr.data954, i64 54
  store i64 0, ptr %arr.elem955, align 8
  %arr.data.gep956 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data957 = load ptr, ptr %arr.data.gep956, align 8
  %arr.elem958 = getelementptr i64, ptr %arr.data957, i64 55
  store i64 0, ptr %arr.elem958, align 8
  %arr.data.gep959 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data960 = load ptr, ptr %arr.data.gep959, align 8
  %arr.elem961 = getelementptr i64, ptr %arr.data960, i64 56
  store i64 0, ptr %arr.elem961, align 8
  %arr.data.gep962 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data963 = load ptr, ptr %arr.data.gep962, align 8
  %arr.elem964 = getelementptr i64, ptr %arr.data963, i64 57
  store i64 0, ptr %arr.elem964, align 8
  %arr.data.gep965 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data966 = load ptr, ptr %arr.data.gep965, align 8
  %arr.elem967 = getelementptr i64, ptr %arr.data966, i64 58
  store i64 0, ptr %arr.elem967, align 8
  %arr.data.gep968 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data969 = load ptr, ptr %arr.data.gep968, align 8
  %arr.elem970 = getelementptr i64, ptr %arr.data969, i64 59
  store i64 0, ptr %arr.elem970, align 8
  %arr.data.gep971 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data972 = load ptr, ptr %arr.data.gep971, align 8
  %arr.elem973 = getelementptr i64, ptr %arr.data972, i64 60
  store i64 0, ptr %arr.elem973, align 8
  %arr.data.gep974 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data975 = load ptr, ptr %arr.data.gep974, align 8
  %arr.elem976 = getelementptr i64, ptr %arr.data975, i64 61
  store i64 0, ptr %arr.elem976, align 8
  %arr.data.gep977 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data978 = load ptr, ptr %arr.data.gep977, align 8
  %arr.elem979 = getelementptr i64, ptr %arr.data978, i64 62
  store i64 0, ptr %arr.elem979, align 8
  %arr.data.gep980 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data981 = load ptr, ptr %arr.data.gep980, align 8
  %arr.elem982 = getelementptr i64, ptr %arr.data981, i64 63
  store i64 0, ptr %arr.elem982, align 8
  %obj.ptrtoint983 = ptrtoint ptr %arr_new786 to i64
  store i64 %obj.ptrtoint983, ptr %"%cache_\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  store i64 0, ptr %"%cache_\D8\B9\D8\AF\D8\A7\D8\AF_LRU", align 8
  store i64 0, ptr %"%cache_\D8\A5\D8\B5\D8\A7\D8\A8\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%cache_\D8\A5\D8\AE\D9\81\D8\A7\D9\82\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%cache_\D9\83\D8\AA\D8\A7\D8\A8\D8\A7\D8\AA_\D9\85\D8\A4\D8\AC\D9\84\D8\A9", align 8
  store i64 0, ptr %"%cache_\D8\AA\D9\81\D8\B1\D9\8A\D8\BA\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%cache_\D8\A5\D8\AE\D9\84\D8\A7\D8\A1\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%cache_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  store i64 0, ptr %"%cache_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  store i64 0, ptr %"%cache_\D9\85\D9\87\D9\8A\D8\A3", align 8
  %arr_new984 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep985 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 0
  store i64 16, ptr %arr.len.gep985, align 8
  %arr.cap.gep986 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep986, align 8
  %arr.data987 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep988 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  store ptr %arr.data987, ptr %arr.data.gep988, align 8
  %arr.data.gep989 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data990 = load ptr, ptr %arr.data.gep989, align 8
  %arr.elem991 = getelementptr i64, ptr %arr.data990, i64 0
  store i64 0, ptr %arr.elem991, align 8
  %arr.data.gep992 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data993 = load ptr, ptr %arr.data.gep992, align 8
  %arr.elem994 = getelementptr i64, ptr %arr.data993, i64 1
  store i64 0, ptr %arr.elem994, align 8
  %arr.data.gep995 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data996 = load ptr, ptr %arr.data.gep995, align 8
  %arr.elem997 = getelementptr i64, ptr %arr.data996, i64 2
  store i64 0, ptr %arr.elem997, align 8
  %arr.data.gep998 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data999 = load ptr, ptr %arr.data.gep998, align 8
  %arr.elem1000 = getelementptr i64, ptr %arr.data999, i64 3
  store i64 0, ptr %arr.elem1000, align 8
  %arr.data.gep1001 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1002 = load ptr, ptr %arr.data.gep1001, align 8
  %arr.elem1003 = getelementptr i64, ptr %arr.data1002, i64 4
  store i64 0, ptr %arr.elem1003, align 8
  %arr.data.gep1004 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1005 = load ptr, ptr %arr.data.gep1004, align 8
  %arr.elem1006 = getelementptr i64, ptr %arr.data1005, i64 5
  store i64 0, ptr %arr.elem1006, align 8
  %arr.data.gep1007 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1008 = load ptr, ptr %arr.data.gep1007, align 8
  %arr.elem1009 = getelementptr i64, ptr %arr.data1008, i64 6
  store i64 0, ptr %arr.elem1009, align 8
  %arr.data.gep1010 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1011 = load ptr, ptr %arr.data.gep1010, align 8
  %arr.elem1012 = getelementptr i64, ptr %arr.data1011, i64 7
  store i64 0, ptr %arr.elem1012, align 8
  %arr.data.gep1013 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1014 = load ptr, ptr %arr.data.gep1013, align 8
  %arr.elem1015 = getelementptr i64, ptr %arr.data1014, i64 8
  store i64 0, ptr %arr.elem1015, align 8
  %arr.data.gep1016 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1017 = load ptr, ptr %arr.data.gep1016, align 8
  %arr.elem1018 = getelementptr i64, ptr %arr.data1017, i64 9
  store i64 0, ptr %arr.elem1018, align 8
  %arr.data.gep1019 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1020 = load ptr, ptr %arr.data.gep1019, align 8
  %arr.elem1021 = getelementptr i64, ptr %arr.data1020, i64 10
  store i64 0, ptr %arr.elem1021, align 8
  %arr.data.gep1022 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1023 = load ptr, ptr %arr.data.gep1022, align 8
  %arr.elem1024 = getelementptr i64, ptr %arr.data1023, i64 11
  store i64 0, ptr %arr.elem1024, align 8
  %arr.data.gep1025 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1026 = load ptr, ptr %arr.data.gep1025, align 8
  %arr.elem1027 = getelementptr i64, ptr %arr.data1026, i64 12
  store i64 0, ptr %arr.elem1027, align 8
  %arr.data.gep1028 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1029 = load ptr, ptr %arr.data.gep1028, align 8
  %arr.elem1030 = getelementptr i64, ptr %arr.data1029, i64 13
  store i64 0, ptr %arr.elem1030, align 8
  %arr.data.gep1031 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1032 = load ptr, ptr %arr.data.gep1031, align 8
  %arr.elem1033 = getelementptr i64, ptr %arr.data1032, i64 14
  store i64 0, ptr %arr.elem1033, align 8
  %arr.data.gep1034 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1035 = load ptr, ptr %arr.data.gep1034, align 8
  %arr.elem1036 = getelementptr i64, ptr %arr.data1035, i64 15
  store i64 0, ptr %arr.elem1036, align 8
  %obj.ptrtoint1037 = ptrtoint ptr %arr_new984 to i64
  store i64 %obj.ptrtoint1037, ptr %"%\D8\B7\D9\84\D8\A8_\D9\86\D9\88\D8\B9", align 8
  %arr_new1038 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep1039 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 0
  store i64 16, ptr %arr.len.gep1039, align 8
  %arr.cap.gep1040 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep1040, align 8
  %arr.data1041 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep1042 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 2
  store ptr %arr.data1041, ptr %arr.data.gep1042, align 8
  %arr.data.gep1043 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 2
  %arr.data1044 = load ptr, ptr %arr.data.gep1043, align 8
  %arr.elem1045 = getelementptr i64, ptr %arr.data1044, i64 0
  store i64 0, ptr %arr.elem1045, align 8
  %arr.data.gep1046 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 2
  %arr.data1047 = load ptr, ptr %arr.data.gep1046, align 8
  %arr.elem1048 = getelementptr i64, ptr %arr.data1047, i64 1
  store i64 0, ptr %arr.elem1048, align 8
  %arr.data.gep1049 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 2
  %arr.data1050 = load ptr, ptr %arr.data.gep1049, align 8
  %arr.elem1051 = getelementptr i64, ptr %arr.data1050, i64 2
  store i64 0, ptr %arr.elem1051, align 8
  %arr.data.gep1052 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 2
  %arr.data1053 = load ptr, ptr %arr.data.gep1052, align 8
  %arr.elem1054 = getelementptr i64, ptr %arr.data1053, i64 3
  store i64 0, ptr %arr.elem1054, align 8
  %arr.data.gep1055 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 2
  %arr.data1056 = load ptr, ptr %arr.data.gep1055, align 8
  %arr.elem1057 = getelementptr i64, ptr %arr.data1056, i64 4
  store i64 0, ptr %arr.elem1057, align 8
  %arr.data.gep1058 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 2
  %arr.data1059 = load ptr, ptr %arr.data.gep1058, align 8
  %arr.elem1060 = getelementptr i64, ptr %arr.data1059, i64 5
  store i64 0, ptr %arr.elem1060, align 8
  %arr.data.gep1061 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 2
  %arr.data1062 = load ptr, ptr %arr.data.gep1061, align 8
  %arr.elem1063 = getelementptr i64, ptr %arr.data1062, i64 6
  store i64 0, ptr %arr.elem1063, align 8
  %arr.data.gep1064 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 2
  %arr.data1065 = load ptr, ptr %arr.data.gep1064, align 8
  %arr.elem1066 = getelementptr i64, ptr %arr.data1065, i64 7
  store i64 0, ptr %arr.elem1066, align 8
  %arr.data.gep1067 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 2
  %arr.data1068 = load ptr, ptr %arr.data.gep1067, align 8
  %arr.elem1069 = getelementptr i64, ptr %arr.data1068, i64 8
  store i64 0, ptr %arr.elem1069, align 8
  %arr.data.gep1070 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 2
  %arr.data1071 = load ptr, ptr %arr.data.gep1070, align 8
  %arr.elem1072 = getelementptr i64, ptr %arr.data1071, i64 9
  store i64 0, ptr %arr.elem1072, align 8
  %arr.data.gep1073 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 2
  %arr.data1074 = load ptr, ptr %arr.data.gep1073, align 8
  %arr.elem1075 = getelementptr i64, ptr %arr.data1074, i64 10
  store i64 0, ptr %arr.elem1075, align 8
  %arr.data.gep1076 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 2
  %arr.data1077 = load ptr, ptr %arr.data.gep1076, align 8
  %arr.elem1078 = getelementptr i64, ptr %arr.data1077, i64 11
  store i64 0, ptr %arr.elem1078, align 8
  %arr.data.gep1079 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 2
  %arr.data1080 = load ptr, ptr %arr.data.gep1079, align 8
  %arr.elem1081 = getelementptr i64, ptr %arr.data1080, i64 12
  store i64 0, ptr %arr.elem1081, align 8
  %arr.data.gep1082 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 2
  %arr.data1083 = load ptr, ptr %arr.data.gep1082, align 8
  %arr.elem1084 = getelementptr i64, ptr %arr.data1083, i64 13
  store i64 0, ptr %arr.elem1084, align 8
  %arr.data.gep1085 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 2
  %arr.data1086 = load ptr, ptr %arr.data.gep1085, align 8
  %arr.elem1087 = getelementptr i64, ptr %arr.data1086, i64 14
  store i64 0, ptr %arr.elem1087, align 8
  %arr.data.gep1088 = getelementptr inbounds %SadArray, ptr %arr_new1038, i32 0, i32 2
  %arr.data1089 = load ptr, ptr %arr.data.gep1088, align 8
  %arr.elem1090 = getelementptr i64, ptr %arr.data1089, i64 15
  store i64 0, ptr %arr.elem1090, align 8
  %obj.ptrtoint1091 = ptrtoint ptr %arr_new1038 to i64
  store i64 %obj.ptrtoint1091, ptr %"%\D8\B7\D9\84\D8\A8_\D9\82\D8\B1\D8\B5", align 8
  %arr_new1092 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep1093 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 0
  store i64 16, ptr %arr.len.gep1093, align 8
  %arr.cap.gep1094 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep1094, align 8
  %arr.data1095 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep1096 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 2
  store ptr %arr.data1095, ptr %arr.data.gep1096, align 8
  %arr.data.gep1097 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 2
  %arr.data1098 = load ptr, ptr %arr.data.gep1097, align 8
  %arr.elem1099 = getelementptr i64, ptr %arr.data1098, i64 0
  store i64 0, ptr %arr.elem1099, align 8
  %arr.data.gep1100 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 2
  %arr.data1101 = load ptr, ptr %arr.data.gep1100, align 8
  %arr.elem1102 = getelementptr i64, ptr %arr.data1101, i64 1
  store i64 0, ptr %arr.elem1102, align 8
  %arr.data.gep1103 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 2
  %arr.data1104 = load ptr, ptr %arr.data.gep1103, align 8
  %arr.elem1105 = getelementptr i64, ptr %arr.data1104, i64 2
  store i64 0, ptr %arr.elem1105, align 8
  %arr.data.gep1106 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 2
  %arr.data1107 = load ptr, ptr %arr.data.gep1106, align 8
  %arr.elem1108 = getelementptr i64, ptr %arr.data1107, i64 3
  store i64 0, ptr %arr.elem1108, align 8
  %arr.data.gep1109 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 2
  %arr.data1110 = load ptr, ptr %arr.data.gep1109, align 8
  %arr.elem1111 = getelementptr i64, ptr %arr.data1110, i64 4
  store i64 0, ptr %arr.elem1111, align 8
  %arr.data.gep1112 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 2
  %arr.data1113 = load ptr, ptr %arr.data.gep1112, align 8
  %arr.elem1114 = getelementptr i64, ptr %arr.data1113, i64 5
  store i64 0, ptr %arr.elem1114, align 8
  %arr.data.gep1115 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 2
  %arr.data1116 = load ptr, ptr %arr.data.gep1115, align 8
  %arr.elem1117 = getelementptr i64, ptr %arr.data1116, i64 6
  store i64 0, ptr %arr.elem1117, align 8
  %arr.data.gep1118 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 2
  %arr.data1119 = load ptr, ptr %arr.data.gep1118, align 8
  %arr.elem1120 = getelementptr i64, ptr %arr.data1119, i64 7
  store i64 0, ptr %arr.elem1120, align 8
  %arr.data.gep1121 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 2
  %arr.data1122 = load ptr, ptr %arr.data.gep1121, align 8
  %arr.elem1123 = getelementptr i64, ptr %arr.data1122, i64 8
  store i64 0, ptr %arr.elem1123, align 8
  %arr.data.gep1124 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 2
  %arr.data1125 = load ptr, ptr %arr.data.gep1124, align 8
  %arr.elem1126 = getelementptr i64, ptr %arr.data1125, i64 9
  store i64 0, ptr %arr.elem1126, align 8
  %arr.data.gep1127 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 2
  %arr.data1128 = load ptr, ptr %arr.data.gep1127, align 8
  %arr.elem1129 = getelementptr i64, ptr %arr.data1128, i64 10
  store i64 0, ptr %arr.elem1129, align 8
  %arr.data.gep1130 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 2
  %arr.data1131 = load ptr, ptr %arr.data.gep1130, align 8
  %arr.elem1132 = getelementptr i64, ptr %arr.data1131, i64 11
  store i64 0, ptr %arr.elem1132, align 8
  %arr.data.gep1133 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 2
  %arr.data1134 = load ptr, ptr %arr.data.gep1133, align 8
  %arr.elem1135 = getelementptr i64, ptr %arr.data1134, i64 12
  store i64 0, ptr %arr.elem1135, align 8
  %arr.data.gep1136 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 2
  %arr.data1137 = load ptr, ptr %arr.data.gep1136, align 8
  %arr.elem1138 = getelementptr i64, ptr %arr.data1137, i64 13
  store i64 0, ptr %arr.elem1138, align 8
  %arr.data.gep1139 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 2
  %arr.data1140 = load ptr, ptr %arr.data.gep1139, align 8
  %arr.elem1141 = getelementptr i64, ptr %arr.data1140, i64 14
  store i64 0, ptr %arr.elem1141, align 8
  %arr.data.gep1142 = getelementptr inbounds %SadArray, ptr %arr_new1092, i32 0, i32 2
  %arr.data1143 = load ptr, ptr %arr.data.gep1142, align 8
  %arr.elem1144 = getelementptr i64, ptr %arr.data1143, i64 15
  store i64 0, ptr %arr.elem1144, align 8
  %obj.ptrtoint1145 = ptrtoint ptr %arr_new1092 to i64
  store i64 %obj.ptrtoint1145, ptr %"%\D8\B7\D9\84\D8\A8_lba", align 8
  %arr_new1146 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep1147 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 0
  store i64 16, ptr %arr.len.gep1147, align 8
  %arr.cap.gep1148 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep1148, align 8
  %arr.data1149 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep1150 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 2
  store ptr %arr.data1149, ptr %arr.data.gep1150, align 8
  %arr.data.gep1151 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 2
  %arr.data1152 = load ptr, ptr %arr.data.gep1151, align 8
  %arr.elem1153 = getelementptr i64, ptr %arr.data1152, i64 0
  store i64 0, ptr %arr.elem1153, align 8
  %arr.data.gep1154 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 2
  %arr.data1155 = load ptr, ptr %arr.data.gep1154, align 8
  %arr.elem1156 = getelementptr i64, ptr %arr.data1155, i64 1
  store i64 0, ptr %arr.elem1156, align 8
  %arr.data.gep1157 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 2
  %arr.data1158 = load ptr, ptr %arr.data.gep1157, align 8
  %arr.elem1159 = getelementptr i64, ptr %arr.data1158, i64 2
  store i64 0, ptr %arr.elem1159, align 8
  %arr.data.gep1160 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 2
  %arr.data1161 = load ptr, ptr %arr.data.gep1160, align 8
  %arr.elem1162 = getelementptr i64, ptr %arr.data1161, i64 3
  store i64 0, ptr %arr.elem1162, align 8
  %arr.data.gep1163 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 2
  %arr.data1164 = load ptr, ptr %arr.data.gep1163, align 8
  %arr.elem1165 = getelementptr i64, ptr %arr.data1164, i64 4
  store i64 0, ptr %arr.elem1165, align 8
  %arr.data.gep1166 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 2
  %arr.data1167 = load ptr, ptr %arr.data.gep1166, align 8
  %arr.elem1168 = getelementptr i64, ptr %arr.data1167, i64 5
  store i64 0, ptr %arr.elem1168, align 8
  %arr.data.gep1169 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 2
  %arr.data1170 = load ptr, ptr %arr.data.gep1169, align 8
  %arr.elem1171 = getelementptr i64, ptr %arr.data1170, i64 6
  store i64 0, ptr %arr.elem1171, align 8
  %arr.data.gep1172 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 2
  %arr.data1173 = load ptr, ptr %arr.data.gep1172, align 8
  %arr.elem1174 = getelementptr i64, ptr %arr.data1173, i64 7
  store i64 0, ptr %arr.elem1174, align 8
  %arr.data.gep1175 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 2
  %arr.data1176 = load ptr, ptr %arr.data.gep1175, align 8
  %arr.elem1177 = getelementptr i64, ptr %arr.data1176, i64 8
  store i64 0, ptr %arr.elem1177, align 8
  %arr.data.gep1178 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 2
  %arr.data1179 = load ptr, ptr %arr.data.gep1178, align 8
  %arr.elem1180 = getelementptr i64, ptr %arr.data1179, i64 9
  store i64 0, ptr %arr.elem1180, align 8
  %arr.data.gep1181 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 2
  %arr.data1182 = load ptr, ptr %arr.data.gep1181, align 8
  %arr.elem1183 = getelementptr i64, ptr %arr.data1182, i64 10
  store i64 0, ptr %arr.elem1183, align 8
  %arr.data.gep1184 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 2
  %arr.data1185 = load ptr, ptr %arr.data.gep1184, align 8
  %arr.elem1186 = getelementptr i64, ptr %arr.data1185, i64 11
  store i64 0, ptr %arr.elem1186, align 8
  %arr.data.gep1187 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 2
  %arr.data1188 = load ptr, ptr %arr.data.gep1187, align 8
  %arr.elem1189 = getelementptr i64, ptr %arr.data1188, i64 12
  store i64 0, ptr %arr.elem1189, align 8
  %arr.data.gep1190 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 2
  %arr.data1191 = load ptr, ptr %arr.data.gep1190, align 8
  %arr.elem1192 = getelementptr i64, ptr %arr.data1191, i64 13
  store i64 0, ptr %arr.elem1192, align 8
  %arr.data.gep1193 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 2
  %arr.data1194 = load ptr, ptr %arr.data.gep1193, align 8
  %arr.elem1195 = getelementptr i64, ptr %arr.data1194, i64 14
  store i64 0, ptr %arr.elem1195, align 8
  %arr.data.gep1196 = getelementptr inbounds %SadArray, ptr %arr_new1146, i32 0, i32 2
  %arr.data1197 = load ptr, ptr %arr.data.gep1196, align 8
  %arr.elem1198 = getelementptr i64, ptr %arr.data1197, i64 15
  store i64 0, ptr %arr.elem1198, align 8
  %obj.ptrtoint1199 = ptrtoint ptr %arr_new1146 to i64
  store i64 %obj.ptrtoint1199, ptr %"%\D8\B7\D9\84\D8\A8_\D8\B9\D8\AF\D8\AF", align 8
  %arr_new1200 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep1201 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 0
  store i64 16, ptr %arr.len.gep1201, align 8
  %arr.cap.gep1202 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep1202, align 8
  %arr.data1203 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep1204 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 2
  store ptr %arr.data1203, ptr %arr.data.gep1204, align 8
  %arr.data.gep1205 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 2
  %arr.data1206 = load ptr, ptr %arr.data.gep1205, align 8
  %arr.elem1207 = getelementptr i64, ptr %arr.data1206, i64 0
  store i64 0, ptr %arr.elem1207, align 8
  %arr.data.gep1208 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 2
  %arr.data1209 = load ptr, ptr %arr.data.gep1208, align 8
  %arr.elem1210 = getelementptr i64, ptr %arr.data1209, i64 1
  store i64 0, ptr %arr.elem1210, align 8
  %arr.data.gep1211 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 2
  %arr.data1212 = load ptr, ptr %arr.data.gep1211, align 8
  %arr.elem1213 = getelementptr i64, ptr %arr.data1212, i64 2
  store i64 0, ptr %arr.elem1213, align 8
  %arr.data.gep1214 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 2
  %arr.data1215 = load ptr, ptr %arr.data.gep1214, align 8
  %arr.elem1216 = getelementptr i64, ptr %arr.data1215, i64 3
  store i64 0, ptr %arr.elem1216, align 8
  %arr.data.gep1217 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 2
  %arr.data1218 = load ptr, ptr %arr.data.gep1217, align 8
  %arr.elem1219 = getelementptr i64, ptr %arr.data1218, i64 4
  store i64 0, ptr %arr.elem1219, align 8
  %arr.data.gep1220 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 2
  %arr.data1221 = load ptr, ptr %arr.data.gep1220, align 8
  %arr.elem1222 = getelementptr i64, ptr %arr.data1221, i64 5
  store i64 0, ptr %arr.elem1222, align 8
  %arr.data.gep1223 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 2
  %arr.data1224 = load ptr, ptr %arr.data.gep1223, align 8
  %arr.elem1225 = getelementptr i64, ptr %arr.data1224, i64 6
  store i64 0, ptr %arr.elem1225, align 8
  %arr.data.gep1226 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 2
  %arr.data1227 = load ptr, ptr %arr.data.gep1226, align 8
  %arr.elem1228 = getelementptr i64, ptr %arr.data1227, i64 7
  store i64 0, ptr %arr.elem1228, align 8
  %arr.data.gep1229 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 2
  %arr.data1230 = load ptr, ptr %arr.data.gep1229, align 8
  %arr.elem1231 = getelementptr i64, ptr %arr.data1230, i64 8
  store i64 0, ptr %arr.elem1231, align 8
  %arr.data.gep1232 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 2
  %arr.data1233 = load ptr, ptr %arr.data.gep1232, align 8
  %arr.elem1234 = getelementptr i64, ptr %arr.data1233, i64 9
  store i64 0, ptr %arr.elem1234, align 8
  %arr.data.gep1235 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 2
  %arr.data1236 = load ptr, ptr %arr.data.gep1235, align 8
  %arr.elem1237 = getelementptr i64, ptr %arr.data1236, i64 10
  store i64 0, ptr %arr.elem1237, align 8
  %arr.data.gep1238 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 2
  %arr.data1239 = load ptr, ptr %arr.data.gep1238, align 8
  %arr.elem1240 = getelementptr i64, ptr %arr.data1239, i64 11
  store i64 0, ptr %arr.elem1240, align 8
  %arr.data.gep1241 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 2
  %arr.data1242 = load ptr, ptr %arr.data.gep1241, align 8
  %arr.elem1243 = getelementptr i64, ptr %arr.data1242, i64 12
  store i64 0, ptr %arr.elem1243, align 8
  %arr.data.gep1244 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 2
  %arr.data1245 = load ptr, ptr %arr.data.gep1244, align 8
  %arr.elem1246 = getelementptr i64, ptr %arr.data1245, i64 13
  store i64 0, ptr %arr.elem1246, align 8
  %arr.data.gep1247 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 2
  %arr.data1248 = load ptr, ptr %arr.data.gep1247, align 8
  %arr.elem1249 = getelementptr i64, ptr %arr.data1248, i64 14
  store i64 0, ptr %arr.elem1249, align 8
  %arr.data.gep1250 = getelementptr inbounds %SadArray, ptr %arr_new1200, i32 0, i32 2
  %arr.data1251 = load ptr, ptr %arr.data.gep1250, align 8
  %arr.elem1252 = getelementptr i64, ptr %arr.data1251, i64 15
  store i64 0, ptr %arr.elem1252, align 8
  %obj.ptrtoint1253 = ptrtoint ptr %arr_new1200 to i64
  store i64 %obj.ptrtoint1253, ptr %"%\D8\B7\D9\84\D8\A8_\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %arr_new1254 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep1255 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 0
  store i64 16, ptr %arr.len.gep1255, align 8
  %arr.cap.gep1256 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep1256, align 8
  %arr.data1257 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep1258 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 2
  store ptr %arr.data1257, ptr %arr.data.gep1258, align 8
  %arr.data.gep1259 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 2
  %arr.data1260 = load ptr, ptr %arr.data.gep1259, align 8
  %arr.elem1261 = getelementptr i64, ptr %arr.data1260, i64 0
  store i64 0, ptr %arr.elem1261, align 8
  %arr.data.gep1262 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 2
  %arr.data1263 = load ptr, ptr %arr.data.gep1262, align 8
  %arr.elem1264 = getelementptr i64, ptr %arr.data1263, i64 1
  store i64 0, ptr %arr.elem1264, align 8
  %arr.data.gep1265 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 2
  %arr.data1266 = load ptr, ptr %arr.data.gep1265, align 8
  %arr.elem1267 = getelementptr i64, ptr %arr.data1266, i64 2
  store i64 0, ptr %arr.elem1267, align 8
  %arr.data.gep1268 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 2
  %arr.data1269 = load ptr, ptr %arr.data.gep1268, align 8
  %arr.elem1270 = getelementptr i64, ptr %arr.data1269, i64 3
  store i64 0, ptr %arr.elem1270, align 8
  %arr.data.gep1271 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 2
  %arr.data1272 = load ptr, ptr %arr.data.gep1271, align 8
  %arr.elem1273 = getelementptr i64, ptr %arr.data1272, i64 4
  store i64 0, ptr %arr.elem1273, align 8
  %arr.data.gep1274 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 2
  %arr.data1275 = load ptr, ptr %arr.data.gep1274, align 8
  %arr.elem1276 = getelementptr i64, ptr %arr.data1275, i64 5
  store i64 0, ptr %arr.elem1276, align 8
  %arr.data.gep1277 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 2
  %arr.data1278 = load ptr, ptr %arr.data.gep1277, align 8
  %arr.elem1279 = getelementptr i64, ptr %arr.data1278, i64 6
  store i64 0, ptr %arr.elem1279, align 8
  %arr.data.gep1280 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 2
  %arr.data1281 = load ptr, ptr %arr.data.gep1280, align 8
  %arr.elem1282 = getelementptr i64, ptr %arr.data1281, i64 7
  store i64 0, ptr %arr.elem1282, align 8
  %arr.data.gep1283 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 2
  %arr.data1284 = load ptr, ptr %arr.data.gep1283, align 8
  %arr.elem1285 = getelementptr i64, ptr %arr.data1284, i64 8
  store i64 0, ptr %arr.elem1285, align 8
  %arr.data.gep1286 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 2
  %arr.data1287 = load ptr, ptr %arr.data.gep1286, align 8
  %arr.elem1288 = getelementptr i64, ptr %arr.data1287, i64 9
  store i64 0, ptr %arr.elem1288, align 8
  %arr.data.gep1289 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 2
  %arr.data1290 = load ptr, ptr %arr.data.gep1289, align 8
  %arr.elem1291 = getelementptr i64, ptr %arr.data1290, i64 10
  store i64 0, ptr %arr.elem1291, align 8
  %arr.data.gep1292 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 2
  %arr.data1293 = load ptr, ptr %arr.data.gep1292, align 8
  %arr.elem1294 = getelementptr i64, ptr %arr.data1293, i64 11
  store i64 0, ptr %arr.elem1294, align 8
  %arr.data.gep1295 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 2
  %arr.data1296 = load ptr, ptr %arr.data.gep1295, align 8
  %arr.elem1297 = getelementptr i64, ptr %arr.data1296, i64 12
  store i64 0, ptr %arr.elem1297, align 8
  %arr.data.gep1298 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 2
  %arr.data1299 = load ptr, ptr %arr.data.gep1298, align 8
  %arr.elem1300 = getelementptr i64, ptr %arr.data1299, i64 13
  store i64 0, ptr %arr.elem1300, align 8
  %arr.data.gep1301 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 2
  %arr.data1302 = load ptr, ptr %arr.data.gep1301, align 8
  %arr.elem1303 = getelementptr i64, ptr %arr.data1302, i64 14
  store i64 0, ptr %arr.elem1303, align 8
  %arr.data.gep1304 = getelementptr inbounds %SadArray, ptr %arr_new1254, i32 0, i32 2
  %arr.data1305 = load ptr, ptr %arr.data.gep1304, align 8
  %arr.elem1306 = getelementptr i64, ptr %arr.data1305, i64 15
  store i64 0, ptr %arr.elem1306, align 8
  %obj.ptrtoint1307 = ptrtoint ptr %arr_new1254 to i64
  store i64 %obj.ptrtoint1307, ptr %"%\D8\B7\D9\84\D8\A8_\D8\AD\D8\A7\D9\84\D8\A9_q", align 8
  store i64 0, ptr %"%IOREQ_EMPTY", align 8
  store i64 1, ptr %"%IOREQ_PENDING", align 8
  store i64 2, ptr %"%IOREQ_DONE", align 8
  store i64 3, ptr %"%IOREQ_ERROR", align 8
  store i64 0, ptr %"%\D8\B7\D8\A7\D8\A8\D9\88\D8\B1_\D8\B1\D8\A3\D8\B3", align 8
  store i64 0, ptr %"%\D8\B7\D8\A7\D8\A8\D9\88\D8\B1_\D8\B0\D9\8A\D9\84", align 8
  store i64 0, ptr %"%\D8\B7\D8\A7\D8\A8\D9\88\D8\B1_\D8\B9\D8\AF\D8\AF", align 8
  store i64 -1, ptr %"%prefetch_\D8\A2\D8\AE\D8\B1_\D9\82\D8\B1\D8\B5", align 8
  store i64 -1, ptr %"%prefetch_\D8\A2\D8\AE\D8\B1_lba", align 8
  store i64 0, ptr %"%prefetch_\D8\B9\D8\AF\D8\A7\D8\AF_\D8\AA\D8\B3\D9\84\D8\B3\D9\84\D9\8A", align 8
  store i64 4, ptr %"%PREFETCH_AHEAD", align 8
  ret void
}

declare ptr @malloc(i64)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
