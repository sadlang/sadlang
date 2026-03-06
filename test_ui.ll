; ModuleID = 'test_ui'
source_filename = "test_ui"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82" = global i64 0
@"\D8\A7\D9\84\D9\87\D9\8A\D9\83\D9\84" = global i64 0
@"\D8\A7\D9\84\D8\B4\D8\B1\D9\8A\D8\B7" = global i64 0
@"\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89" = global i64 0
@"\D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\86" = global i64 0
@"\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9" = global i64 0
@"\D9\86\D8\B5_\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9" = global i64 0
@"\D8\B5\D9\81_\D8\A7\D9\84\D8\A3\D8\B2\D8\B1\D8\A7\D8\B1" = global i64 0
@"\D8\B2\D8\B11" = global i64 0
@"\D8\B2\D8\B12" = global i64 0
@"\D8\A7\D9\84\D9\81\D8\A7\D8\B5\D9\84" = global i64 0
@"\D9\85\D8\B3\D8\A7\D8\AD\D8\A9" = global i64 0
@str.const = private unnamed_addr constant [24 x i8] c"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82\D9\8A \D8\A7\D9\84\D8\A3\D9\88\D9\84\00", align 1
@str.const.1 = private unnamed_addr constant [29 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7\D9\8B \D8\A8\D8\A7\D9\84\D8\B9\D8\A7\D9\84\D9\85!\00", align 1
@str.const.2 = private unnamed_addr constant [32 x i8] c"\D9\87\D8\B0\D8\A7 \D9\86\D8\B5 \D8\AF\D8\A7\D8\AE\D9\84 \D8\A8\D8\B7\D8\A7\D9\82\D8\A9\00", align 1
@str.const.3 = private unnamed_addr constant [16 x i8] c"\D8\A7\D8\B6\D8\BA\D8\B7 \D9\87\D9\86\D8\A7\00", align 1
@str.const.4 = private unnamed_addr constant [11 x i8] c"\D8\A5\D9\84\D8\BA\D8\A7\D8\A1\00", align 1
@str.const.5 = private unnamed_addr constant [54 x i8] c"\D8\AA\D9\85 \D8\A8\D9\86\D8\A7\D8\A1 \D9\88\D8\A7\D8\AC\D9\87\D8\A9 \D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD!\00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@str.const.6 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

define void @__sad_main() {
entry:
  %"%\D9\85\D8\B3\D8\A7\D8\AD\D8\A9" = alloca ptr, align 8
  %"%\D8\A7\D9\84\D9\81\D8\A7\D8\B5\D9\84" = alloca ptr, align 8
  %"%\D8\B2\D8\B12" = alloca ptr, align 8
  %"%\D8\B2\D8\B11" = alloca ptr, align 8
  %"%\D8\B5\D9\81_\D8\A7\D9\84\D8\A3\D8\B2\D8\B1\D8\A7\D8\B1" = alloca ptr, align 8
  %"%\D9\86\D8\B5_\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9" = alloca ptr, align 8
  %"%\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9" = alloca ptr, align 8
  %"%\D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca ptr, align 8
  %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89" = alloca ptr, align 8
  %"%\D8\A7\D9\84\D8\B4\D8\B1\D9\8A\D8\B7" = alloca ptr, align 8
  %"%\D8\A7\D9\84\D9\87\D9\8A\D9\83\D9\84" = alloca ptr, align 8
  %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82" = alloca ptr, align 8
  %sad_app_create_result = call ptr @sad_app_create()
  store ptr %sad_app_create_result, ptr %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82", align 8
  %sad_scaffold_result = call ptr @sad_scaffold()
  store ptr %sad_scaffold_result, ptr %"%\D8\A7\D9\84\D9\87\D9\8A\D9\83\D9\84", align 8
  %sad_app_bar_result = call ptr @sad_app_bar(ptr @str.const)
  store ptr %sad_app_bar_result, ptr %"%\D8\A7\D9\84\D8\B4\D8\B1\D9\8A\D8\B7", align 8
  %"%\D8\A7\D9\84\D9\87\D9\8A\D9\83\D9\84.load" = load ptr, ptr %"%\D8\A7\D9\84\D9\87\D9\8A\D9\83\D9\84", align 8
  %"%\D8\A7\D9\84\D8\B4\D8\B1\D9\8A\D8\B7.load" = load ptr, ptr %"%\D8\A7\D9\84\D8\B4\D8\B1\D9\8A\D8\B7", align 8
  call void @sad_add_child(ptr %"%\D8\A7\D9\84\D9\87\D9\8A\D9\83\D9\84.load", ptr %"%\D8\A7\D9\84\D8\B4\D8\B1\D9\8A\D8\B7.load")
  %sad_column_result = call ptr @sad_column()
  store ptr %sad_column_result, ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89", align 8
  %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load" = load ptr, ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89", align 8
  call void @sad_set_spacing(ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load", float 1.600000e+01)
  %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load1" = load ptr, ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89", align 8
  call void @sad_set_padding(ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load1", float 2.000000e+01, float 1.600000e+01, float 2.000000e+01, float 1.600000e+01)
  %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load2" = load ptr, ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89", align 8
  call void @sad_set_alignment(ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load2", i32 1, i32 1)
  %sad_text_result = call ptr @sad_text(ptr @str.const.1)
  store ptr %sad_text_result, ptr %"%\D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %"%\D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load ptr, ptr %"%\D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  call void @sad_set_background(ptr %"%\D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", float 0.000000e+00, float 0.000000e+00, float 0.000000e+00, float 0.000000e+00)
  %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load3" = load ptr, ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89", align 8
  %"%\D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\86.load4" = load ptr, ptr %"%\D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  call void @sad_add_child(ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load3", ptr %"%\D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\86.load4")
  %sad_card_result = call ptr @sad_card()
  store ptr %sad_card_result, ptr %"%\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9", align 8
  %"%\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9.load" = load ptr, ptr %"%\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9", align 8
  call void @sad_set_padding(ptr %"%\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9.load", float 1.600000e+01, float 1.600000e+01, float 1.600000e+01, float 1.600000e+01)
  %sad_text_result5 = call ptr @sad_text(ptr @str.const.2)
  store ptr %sad_text_result5, ptr %"%\D9\86\D8\B5_\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9", align 8
  %"%\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9.load6" = load ptr, ptr %"%\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9", align 8
  %"%\D9\86\D8\B5_\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9.load" = load ptr, ptr %"%\D9\86\D8\B5_\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9", align 8
  call void @sad_add_child(ptr %"%\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9.load6", ptr %"%\D9\86\D8\B5_\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9.load")
  %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load7" = load ptr, ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89", align 8
  %"%\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9.load8" = load ptr, ptr %"%\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9", align 8
  call void @sad_add_child(ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load7", ptr %"%\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D9\82\D8\A9.load8")
  %sad_row_result = call ptr @sad_row()
  store ptr %sad_row_result, ptr %"%\D8\B5\D9\81_\D8\A7\D9\84\D8\A3\D8\B2\D8\B1\D8\A7\D8\B1", align 8
  %"%\D8\B5\D9\81_\D8\A7\D9\84\D8\A3\D8\B2\D8\B1\D8\A7\D8\B1.load" = load ptr, ptr %"%\D8\B5\D9\81_\D8\A7\D9\84\D8\A3\D8\B2\D8\B1\D8\A7\D8\B1", align 8
  call void @sad_set_spacing(ptr %"%\D8\B5\D9\81_\D8\A7\D9\84\D8\A3\D8\B2\D8\B1\D8\A7\D8\B1.load", float 8.000000e+00)
  %sad_button_result = call ptr @sad_button(ptr @str.const.3, ptr null, ptr null)
  store ptr %sad_button_result, ptr %"%\D8\B2\D8\B11", align 8
  %sad_button_result9 = call ptr @sad_button(ptr @str.const.4, ptr null, ptr null)
  store ptr %sad_button_result9, ptr %"%\D8\B2\D8\B12", align 8
  %"%\D8\B5\D9\81_\D8\A7\D9\84\D8\A3\D8\B2\D8\B1\D8\A7\D8\B1.load10" = load ptr, ptr %"%\D8\B5\D9\81_\D8\A7\D9\84\D8\A3\D8\B2\D8\B1\D8\A7\D8\B1", align 8
  %"%\D8\B2\D8\B11.load" = load ptr, ptr %"%\D8\B2\D8\B11", align 8
  call void @sad_add_child(ptr %"%\D8\B5\D9\81_\D8\A7\D9\84\D8\A3\D8\B2\D8\B1\D8\A7\D8\B1.load10", ptr %"%\D8\B2\D8\B11.load")
  %"%\D8\B5\D9\81_\D8\A7\D9\84\D8\A3\D8\B2\D8\B1\D8\A7\D8\B1.load11" = load ptr, ptr %"%\D8\B5\D9\81_\D8\A7\D9\84\D8\A3\D8\B2\D8\B1\D8\A7\D8\B1", align 8
  %"%\D8\B2\D8\B12.load" = load ptr, ptr %"%\D8\B2\D8\B12", align 8
  call void @sad_add_child(ptr %"%\D8\B5\D9\81_\D8\A7\D9\84\D8\A3\D8\B2\D8\B1\D8\A7\D8\B1.load11", ptr %"%\D8\B2\D8\B12.load")
  %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load12" = load ptr, ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89", align 8
  %"%\D8\B5\D9\81_\D8\A7\D9\84\D8\A3\D8\B2\D8\B1\D8\A7\D8\B1.load13" = load ptr, ptr %"%\D8\B5\D9\81_\D8\A7\D9\84\D8\A3\D8\B2\D8\B1\D8\A7\D8\B1", align 8
  call void @sad_add_child(ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load12", ptr %"%\D8\B5\D9\81_\D8\A7\D9\84\D8\A3\D8\B2\D8\B1\D8\A7\D8\B1.load13")
  %sad_divider_result = call ptr @sad_divider()
  store ptr %sad_divider_result, ptr %"%\D8\A7\D9\84\D9\81\D8\A7\D8\B5\D9\84", align 8
  %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load14" = load ptr, ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89", align 8
  %"%\D8\A7\D9\84\D9\81\D8\A7\D8\B5\D9\84.load" = load ptr, ptr %"%\D8\A7\D9\84\D9\81\D8\A7\D8\B5\D9\84", align 8
  call void @sad_add_child(ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load14", ptr %"%\D8\A7\D9\84\D9\81\D8\A7\D8\B5\D9\84.load")
  %sad_spacer_result = call ptr @sad_spacer()
  store ptr %sad_spacer_result, ptr %"%\D9\85\D8\B3\D8\A7\D8\AD\D8\A9", align 8
  %"%\D9\85\D8\B3\D8\A7\D8\AD\D8\A9.load" = load ptr, ptr %"%\D9\85\D8\B3\D8\A7\D8\AD\D8\A9", align 8
  call void @sad_set_flex(ptr %"%\D9\85\D8\B3\D8\A7\D8\AD\D8\A9.load", float 1.000000e+00)
  %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load15" = load ptr, ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89", align 8
  %"%\D9\85\D8\B3\D8\A7\D8\AD\D8\A9.load16" = load ptr, ptr %"%\D9\85\D8\B3\D8\A7\D8\AD\D8\A9", align 8
  call void @sad_add_child(ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load15", ptr %"%\D9\85\D8\B3\D8\A7\D8\AD\D8\A9.load16")
  %"%\D8\A7\D9\84\D9\87\D9\8A\D9\83\D9\84.load17" = load ptr, ptr %"%\D8\A7\D9\84\D9\87\D9\8A\D9\83\D9\84", align 8
  %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load18" = load ptr, ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89", align 8
  call void @sad_add_child(ptr %"%\D8\A7\D9\84\D9\87\D9\8A\D9\83\D9\84.load17", ptr %"%\D8\A7\D9\84\D9\85\D8\AD\D8\AA\D9\88\D9\89.load18")
  %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82.load" = load ptr, ptr %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82", align 8
  %"%\D8\A7\D9\84\D9\87\D9\8A\D9\83\D9\84.load19" = load ptr, ptr %"%\D8\A7\D9\84\D9\87\D9\8A\D9\83\D9\84", align 8
  call void @sad_app_set_root(ptr %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82.load", ptr %"%\D8\A7\D9\84\D9\87\D9\8A\D9\83\D9\84.load19")
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  ret void
}

; Function Attrs: nounwind
declare ptr @sad_app_create() #0

; Function Attrs: nounwind
declare ptr @sad_scaffold() #0

; Function Attrs: nounwind
declare ptr @sad_app_bar(ptr) #0

; Function Attrs: nounwind
declare void @sad_add_child(ptr, ptr) #0

; Function Attrs: nounwind
declare ptr @sad_column() #0

; Function Attrs: nounwind
declare void @sad_set_spacing(ptr, float) #0

; Function Attrs: nounwind
declare void @sad_set_padding(ptr, float, float, float, float) #0

; Function Attrs: nounwind
declare void @sad_set_alignment(ptr, i32, i32) #0

; Function Attrs: nounwind
declare ptr @sad_text(ptr) #0

; Function Attrs: nounwind
declare void @sad_set_background(ptr, float, float, float, float) #0

; Function Attrs: nounwind
declare ptr @sad_card() #0

; Function Attrs: nounwind
declare ptr @sad_row() #0

; Function Attrs: nounwind
declare ptr @sad_button(ptr, ptr, ptr) #0

; Function Attrs: nounwind
declare ptr @sad_divider() #0

; Function Attrs: nounwind
declare ptr @sad_spacer() #0

; Function Attrs: nounwind
declare void @sad_set_flex(ptr, float) #0

; Function Attrs: nounwind
declare void @sad_app_set_root(ptr, ptr) #0

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}

attributes #0 = { nounwind }
