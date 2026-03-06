; ModuleID = 'usb_hid_نحلة'
source_filename = "usb_hid_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@COM1 = constant i64 1016
@KBD_BUF_SIZE = constant i64 64
@MOUSE_BUF_SIZE = constant i64 32
@KBD_REPORT_SIZE = constant i64 8
@MOUSE_REPORT_SIZE = constant i64 4
@MOD_LCTRL = constant i64 1
@MOD_LSHIFT = constant i64 2
@MOD_LALT = constant i64 4
@MOD_LMETA = constant i64 8
@MOD_RCTRL = constant i64 16
@MOD_RSHIFT = constant i64 32
@MOD_RALT = constant i64 64
@MOD_RMETA = constant i64 128
@MOUSE_BTN_LEFT = constant i64 1
@MOUSE_BTN_RIGHT = constant i64 2
@MOUSE_BTN_MIDDLE = constant i64 4
@sc_normal_0 = global i64 0
@sc_normal_1 = global i64 0
@sc_normal_2 = global i64 0
@sc_normal_3 = global i64 0
@sc_normal_4 = global i64 97
@sc_normal_5 = global i64 98
@sc_normal_6 = global i64 99
@sc_normal_7 = global i64 100
@sc_normal_8 = global i64 101
@sc_normal_9 = global i64 102
@sc_normal_10 = global i64 103
@sc_normal_11 = global i64 104
@sc_normal_12 = global i64 105
@sc_normal_13 = global i64 106
@sc_normal_14 = global i64 107
@sc_normal_15 = global i64 108
@sc_normal_16 = global i64 109
@sc_normal_17 = global i64 110
@sc_normal_18 = global i64 111
@sc_normal_19 = global i64 112
@sc_normal_20 = global i64 113
@sc_normal_21 = global i64 114
@sc_normal_22 = global i64 115
@sc_normal_23 = global i64 116
@sc_normal_24 = global i64 117
@sc_normal_25 = global i64 118
@sc_normal_26 = global i64 119
@sc_normal_27 = global i64 120
@sc_normal_28 = global i64 121
@sc_normal_29 = global i64 122
@sc_normal_30 = global i64 49
@sc_normal_31 = global i64 50
@sc_normal_32 = global i64 51
@sc_normal_33 = global i64 52
@sc_normal_34 = global i64 53
@sc_normal_35 = global i64 54
@sc_normal_36 = global i64 55
@sc_normal_37 = global i64 56
@sc_normal_38 = global i64 57
@sc_normal_39 = global i64 48
@sc_normal_40 = global i64 10
@sc_normal_41 = global i64 27
@sc_normal_42 = global i64 8
@sc_normal_43 = global i64 9
@sc_normal_44 = global i64 32
@sc_normal_45 = global i64 45
@sc_normal_46 = global i64 61
@sc_normal_47 = global i64 91
@sc_normal_48 = global i64 93
@sc_normal_49 = global i64 92
@sc_normal_50 = global i64 35
@sc_normal_51 = global i64 59
@sc_normal_52 = global i64 39
@sc_normal_53 = global i64 96
@sc_normal_54 = global i64 44
@sc_normal_55 = global i64 46
@sc_normal_56 = global i64 47
@sc_normal_57 = global i64 0
@sc_normal_58 = global i64 0
@sc_normal_59 = global i64 0
@sc_normal_60 = global i64 0
@sc_normal_61 = global i64 0
@sc_normal_62 = global i64 0
@sc_normal_63 = global i64 0
@sc_shift_0 = global i64 0
@sc_shift_1 = global i64 0
@sc_shift_2 = global i64 0
@sc_shift_3 = global i64 0
@sc_shift_4 = global i64 65
@sc_shift_5 = global i64 66
@sc_shift_6 = global i64 67
@sc_shift_7 = global i64 68
@sc_shift_8 = global i64 69
@sc_shift_9 = global i64 70
@sc_shift_10 = global i64 71
@sc_shift_11 = global i64 72
@sc_shift_12 = global i64 73
@sc_shift_13 = global i64 74
@sc_shift_14 = global i64 75
@sc_shift_15 = global i64 76
@sc_shift_16 = global i64 77
@sc_shift_17 = global i64 78
@sc_shift_18 = global i64 79
@sc_shift_19 = global i64 80
@sc_shift_20 = global i64 81
@sc_shift_21 = global i64 82
@sc_shift_22 = global i64 83
@sc_shift_23 = global i64 84
@sc_shift_24 = global i64 85
@sc_shift_25 = global i64 86
@sc_shift_26 = global i64 87
@sc_shift_27 = global i64 88
@sc_shift_28 = global i64 89
@sc_shift_29 = global i64 90
@sc_shift_30 = global i64 33
@sc_shift_31 = global i64 64
@sc_shift_32 = global i64 35
@sc_shift_33 = global i64 36
@sc_shift_34 = global i64 37
@sc_shift_35 = global i64 94
@sc_shift_36 = global i64 38
@sc_shift_37 = global i64 42
@sc_shift_38 = global i64 40
@sc_shift_39 = global i64 41
@sc_shift_40 = global i64 10
@sc_shift_41 = global i64 27
@sc_shift_42 = global i64 8
@sc_shift_43 = global i64 9
@sc_shift_44 = global i64 32
@sc_shift_45 = global i64 95
@sc_shift_46 = global i64 43
@sc_shift_47 = global i64 123
@sc_shift_48 = global i64 125
@sc_shift_49 = global i64 124
@sc_shift_50 = global i64 126
@sc_shift_51 = global i64 58
@sc_shift_52 = global i64 34
@sc_shift_53 = global i64 126
@sc_shift_54 = global i64 60
@sc_shift_55 = global i64 62
@sc_shift_56 = global i64 63
@sc_shift_57 = global i64 0
@sc_shift_58 = global i64 0
@sc_shift_59 = global i64 0
@sc_shift_60 = global i64 0
@sc_shift_61 = global i64 0
@sc_shift_62 = global i64 0
@sc_shift_63 = global i64 0
@kbd_dev_idx = global i64 0
@kbd_hc_idx = global i64 0
@kbd_dev_addr = global i64 0
@kbd_endpoint = global i64 1
@kbd_report_buf = global i64 0
@kbd_prev_report = global i64 0
@kbd_active = global i64 0
@kbd_event_buf = global i64 0
@kbd_event_head = global i64 0
@kbd_event_tail = global i64 0
@kbd_modifiers = global i64 0
@kbd_capslock = global i64 0
@kbd_numlock = global i64 0
@kbd_scrolllock = global i64 0
@kbd_pressed_0 = global i64 0
@kbd_pressed_1 = global i64 0
@kbd_pressed_2 = global i64 0
@kbd_pressed_3 = global i64 0
@kbd_pressed_4 = global i64 0
@kbd_pressed_5 = global i64 0
@mouse_dev_idx = global i64 0
@mouse_hc_idx = global i64 0
@mouse_dev_addr = global i64 0
@mouse_endpoint = global i64 1
@mouse_report_buf = global i64 0
@mouse_active = global i64 0
@mouse_x = global i64 0
@mouse_y = global i64 0
@mouse_buttons = global i64 0
@mouse_dx = global i64 0
@mouse_dy = global i64 0
@mouse_scroll = global i64 0
@mouse_event_buf = global i64 0
@mouse_event_head = global i64 0
@mouse_event_tail = global i64 0
@mouse_max_x = global i64 1024
@mouse_max_y = global i64 768
@hid_kbd_events = global i64 0
@hid_mouse_events = global i64 0
@hid_poll_count = global i64 0
@hid_errors = global i64 0
@str.const = private unnamed_addr constant [37 x i8] c"\E2\95\90\E2\95\90\E2\95\90 [HID] \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.1 = private unnamed_addr constant [26 x i8] c"  \D9\84\D9\88\D8\AD\D8\A9 \D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD: \00", align 1
@str.const.2 = private unnamed_addr constant [15 x i8] c"\D9\86\D8\B4\D8\B7\D8\A9 addr=\00", align 1
@str.const.3 = private unnamed_addr constant [18 x i8] c"\D8\BA\D9\8A\D8\B1 \D9\85\D8\AA\D8\B5\D9\84\D8\A9\00", align 1
@str.const.4 = private unnamed_addr constant [13 x i8] c"  \D9\85\D8\A7\D9\88\D8\B3: \00", align 1
@str.const.5 = private unnamed_addr constant [13 x i8] c"\D9\86\D8\B4\D8\B7 addr=\00", align 1
@str.const.6 = private unnamed_addr constant [16 x i8] c"\D8\BA\D9\8A\D8\B1 \D9\85\D8\AA\D8\B5\D9\84\00", align 1
@str.const.7 = private unnamed_addr constant [37 x i8] c"  \D8\A3\D8\AD\D8\AF\D8\A7\D8\AB \D9\84\D9\88\D8\AD\D8\A9 \D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD: \00", align 1
@str.const.8 = private unnamed_addr constant [24 x i8] c"  \D8\A3\D8\AD\D8\AF\D8\A7\D8\AB \D9\85\D8\A7\D9\88\D8\B3: \00", align 1
@str.const.9 = private unnamed_addr constant [23 x i8] c"  \D8\A7\D8\B3\D8\AA\D9\82\D8\B5\D8\A7\D8\A1\D8\A7\D8\AA: \00", align 1
@str.const.10 = private unnamed_addr constant [15 x i8] c"  \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1: \00", align 1
@str.const.11 = private unnamed_addr constant [27 x i8] c"  \D9\85\D9\88\D9\82\D8\B9 \D8\A7\D9\84\D9\85\D8\A7\D9\88\D8\B3: (\00", align 1
@str.const.12 = private unnamed_addr constant [2 x i8] c")\00", align 1
@str.const.13 = private unnamed_addr constant [106 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.14 = private unnamed_addr constant [39 x i8] c"[HID] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D8\AA\D8\B9\D8\B1\D9\8A\D9\81\D8\A7\D8\AA HID...\00", align 1
@str.const.15 = private unnamed_addr constant [30 x i8] c"[HID] Keyboard found at dev #\00", align 1
@str.const.16 = private unnamed_addr constant [24 x i8] c"[HID] No keyboard found\00", align 1
@str.const.17 = private unnamed_addr constant [27 x i8] c"[HID] Mouse found at dev #\00", align 1
@str.const.18 = private unnamed_addr constant [21 x i8] c"[HID] No mouse found\00", align 1
@str.const.19 = private unnamed_addr constant [15 x i8] c"[HID] \D8\AC\D8\A7\D9\87\D8\B2\00", align 1

declare void @sad_ll_mem_read8(i64)

declare void @sad_ll_mem_write8(i64, i64)

declare void @sad_ll_mem_read32(i64)

declare void @sad_ll_mem_write32(i64, i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_kmalloc(i64)

declare void @sad_ll_memset(i64, i64, i64)

declare void @sad_ll_timer_get_ticks()

declare void @usb_get_class(i64)

declare void @usb_get_proto(i64)

declare void @usb_find_keyboard()

declare void @usb_find_mouse()

declare void @uhci_interrupt_transfer(i64, i64, i64, i64, i64)

declare void @uhci_set_idle(i64, i64, i64, i64)

declare void @uhci_set_protocol(i64, i64, i64, i64)

define i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"%ch" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_0

merge_4:                                          ; preds = %while_body_1
  %"%COM1.load" = load i64, ptr @COM1, align 8
  %"%ch.load3" = load i64, ptr %"%ch", align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 %"%ch.load3")
  %"%i.load4" = load i64, ptr %"%i", align 8
  %addtmp5 = add i64 %"%i.load4", 1
  store i64 %addtmp5, ptr %"%i", align 8
  br label %while_cond_0

then_3:                                           ; preds = %while_body_1
  ret i64 0

while_body_1:                                     ; preds = %while_cond_0
  %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load" = load i64, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  %"%i.load2" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load", %"%i.load2"
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%ch", align 8
  %"%ch.load" = load i64, ptr %"%ch", align 8
  %cmpeqtmp = icmp eq i64 %"%ch.load", 0
  br i1 %cmpeqtmp, label %then_3, label %merge_4

while_cond_0:                                     ; preds = %merge_4, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %cmplttmp = icmp slt i64 %"%i.load", 80
  br i1 %cmplttmp, label %while_body_1, label %while_exit_2

while_exit_2:                                     ; preds = %while_cond_0
  ret i64 0
}

define i64 @"hid_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
entry:
  %"%d2" = alloca i64, align 8
  %"%d3" = alloca i64, align 8
  %"%d4" = alloca i64, align 8
  %"%d5" = alloca i64, align 8
  %"%val" = alloca i64, align 8
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  %"%17.load" = load i64, ptr %"\D9\861", align 8
  store i64 %"%17.load", ptr %"%val", align 8
  %"%val.load" = load i64, ptr %"%val", align 8
  %cmplttmp = icmp slt i64 %"%val.load", 0
  br i1 %cmplttmp, label %then_5, label %merge_6

else_11:                                          ; preds = %merge_8
  %"%\D9\86.load" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp19 = icmp sge i64 %"%\D9\86.load", 10000
  br i1 %cmpgetmp19, label %then_12, label %merge_13

else_16:                                          ; preds = %merge_10
  %"%\D9\86.load31" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp32 = icmp sge i64 %"%\D9\86.load31", 1000
  br i1 %cmpgetmp32, label %then_17, label %merge_18

else_21:                                          ; preds = %merge_15
  %"%\D9\86.load44" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp45 = icmp sge i64 %"%\D9\86.load44", 100
  br i1 %cmpgetmp45, label %then_22, label %merge_23

merge_10:                                         ; preds = %merge_13, %then_9
  %"%val.load20" = load i64, ptr %"%val", align 8
  %cmpgetmp21 = icmp sge i64 %"%val.load20", 100
  br i1 %cmpgetmp21, label %then_14, label %else_16

merge_13:                                         ; preds = %then_12, %else_11
  br label %merge_10

merge_15:                                         ; preds = %merge_18, %then_14
  %"%val.load33" = load i64, ptr %"%val", align 8
  %cmpgetmp34 = icmp sge i64 %"%val.load33", 10
  br i1 %cmpgetmp34, label %then_19, label %else_21

merge_18:                                         ; preds = %then_17, %else_16
  br label %merge_15

merge_20:                                         ; preds = %merge_23, %then_19
  %"%val.load46" = load i64, ptr %"%val", align 8
  %addtmp47 = add i64 48, %"%val.load46"
  %"%COM1.load48" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load48", i64 %addtmp47)
  ret i64 0

merge_23:                                         ; preds = %then_22, %else_21
  br label %merge_20

merge_6:                                          ; preds = %then_5, %entry
  %"%val.load3" = load i64, ptr %"%val", align 8
  %cmpgetmp = icmp sge i64 %"%val.load3", 10000
  br i1 %cmpgetmp, label %then_7, label %merge_8

merge_8:                                          ; preds = %then_7, %merge_6
  %"%val.load9" = load i64, ptr %"%val", align 8
  %cmpgetmp10 = icmp sge i64 %"%val.load9", 1000
  br i1 %cmpgetmp10, label %then_9, label %else_11

then_12:                                          ; preds = %else_11
  %"%COM1.load22" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load22", i64 48)
  br label %merge_13

then_14:                                          ; preds = %merge_10
  %"%val.load23" = load i64, ptr %"%val", align 8
  %divtmp24 = sdiv i64 %"%val.load23", 100
  store i64 %divtmp24, ptr %"%d3", align 8
  %"%d3.load" = load i64, ptr %"%d3", align 8
  %addtmp25 = add i64 48, %"%d3.load"
  %"%COM1.load26" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load26", i64 %addtmp25)
  %"%d3.load27" = load i64, ptr %"%d3", align 8
  %multmp28 = mul i64 %"%d3.load27", 100
  %"%val.load29" = load i64, ptr %"%val", align 8
  %subtmp30 = sub i64 %"%val.load29", %multmp28
  store i64 %subtmp30, ptr %"%val", align 8
  br label %merge_15

then_17:                                          ; preds = %else_16
  %"%COM1.load35" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load35", i64 48)
  br label %merge_18

then_19:                                          ; preds = %merge_15
  %"%val.load36" = load i64, ptr %"%val", align 8
  %divtmp37 = sdiv i64 %"%val.load36", 10
  store i64 %divtmp37, ptr %"%d2", align 8
  %"%d2.load" = load i64, ptr %"%d2", align 8
  %addtmp38 = add i64 48, %"%d2.load"
  %"%COM1.load39" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load39", i64 %addtmp38)
  %"%d2.load40" = load i64, ptr %"%d2", align 8
  %multmp41 = mul i64 %"%d2.load40", 10
  %"%val.load42" = load i64, ptr %"%val", align 8
  %subtmp43 = sub i64 %"%val.load42", %multmp41
  store i64 %subtmp43, ptr %"%val", align 8
  br label %merge_20

then_22:                                          ; preds = %else_21
  %"%COM1.load49" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load49", i64 48)
  br label %merge_23

then_5:                                           ; preds = %entry
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 45)
  %"%val.load2" = load i64, ptr %"%val", align 8
  %subtmp = sub i64 0, %"%val.load2"
  store i64 %subtmp, ptr %"%val", align 8
  br label %merge_6

then_7:                                           ; preds = %merge_6
  %"%val.load4" = load i64, ptr %"%val", align 8
  %divtmp = sdiv i64 %"%val.load4", 10000
  store i64 %divtmp, ptr %"%d5", align 8
  %"%d5.load" = load i64, ptr %"%d5", align 8
  %addtmp = add i64 48, %"%d5.load"
  %"%COM1.load5" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load5", i64 %addtmp)
  %"%d5.load6" = load i64, ptr %"%d5", align 8
  %multmp = mul i64 %"%d5.load6", 10000
  %"%val.load7" = load i64, ptr %"%val", align 8
  %subtmp8 = sub i64 %"%val.load7", %multmp
  store i64 %subtmp8, ptr %"%val", align 8
  br label %merge_8

then_9:                                           ; preds = %merge_8
  %"%val.load11" = load i64, ptr %"%val", align 8
  %divtmp12 = sdiv i64 %"%val.load11", 1000
  store i64 %divtmp12, ptr %"%d4", align 8
  %"%d4.load" = load i64, ptr %"%d4", align 8
  %addtmp13 = add i64 48, %"%d4.load"
  %"%COM1.load14" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load14", i64 %addtmp13)
  %"%d4.load15" = load i64, ptr %"%d4", align 8
  %multmp16 = mul i64 %"%d4.load15", 1000
  %"%val.load17" = load i64, ptr %"%val", align 8
  %subtmp18 = sub i64 %"%val.load17", %multmp16
  store i64 %subtmp18, ptr %"%val", align 8
  br label %merge_10
}

define i64 @hid_serial_hex8(i64 %val) {
entry:
  %"%lo" = alloca i64, align 8
  %"%hi" = alloca i64, align 8
  %val1 = alloca i64, align 8
  store i64 %val, ptr %val1, align 8
  %"%val.load" = load i64, ptr %val1, align 8
  %shrtmp = lshr i64 %"%val.load", 4
  store i64 %shrtmp, ptr %"%hi", align 8
  %"%val.load2" = load i64, ptr %val1, align 8
  %andtmp = and i64 %"%val.load2", 15
  store i64 %andtmp, ptr %"%lo", align 8
  %"%hi.load" = load i64, ptr %"%hi", align 8
  %cmplttmp = icmp slt i64 %"%hi.load", 10
  br i1 %cmplttmp, label %then_24, label %else_26

else_26:                                          ; preds = %entry
  %"%hi.load4" = load i64, ptr %"%hi", align 8
  %addtmp5 = add i64 55, %"%hi.load4"
  %"%COM1.load6" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load6", i64 %addtmp5)
  br label %merge_25

else_29:                                          ; preds = %merge_25
  %"%lo.load11" = load i64, ptr %"%lo", align 8
  %addtmp12 = add i64 55, %"%lo.load11"
  %"%COM1.load13" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load13", i64 %addtmp12)
  br label %merge_28

merge_25:                                         ; preds = %else_26, %then_24
  %"%lo.load" = load i64, ptr %"%lo", align 8
  %cmplttmp7 = icmp slt i64 %"%lo.load", 10
  br i1 %cmplttmp7, label %then_27, label %else_29

merge_28:                                         ; preds = %else_29, %then_27
  ret i64 0

then_24:                                          ; preds = %entry
  %"%hi.load3" = load i64, ptr %"%hi", align 8
  %addtmp = add i64 48, %"%hi.load3"
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 %addtmp)
  br label %merge_25

then_27:                                          ; preds = %merge_25
  %"%lo.load8" = load i64, ptr %"%lo", align 8
  %addtmp9 = add i64 48, %"%lo.load8"
  %"%COM1.load10" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load10", i64 %addtmp9)
  br label %merge_28
}

define i64 @"hid_serial_\D8\B3\D8\B7\D8\B1"() {
entry:
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 13)
  %"%COM1.load1" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load1", i64 10)
  ret i64 0
}

define i64 @hid_get_normal_key(i64 %sc) {
entry:
  %sc1 = alloca i64, align 8
  store i64 %sc, ptr %sc1, align 8
  %"%sc.load" = load i64, ptr %sc1, align 8
  %cmpgetmp = icmp sge i64 %"%sc.load", 64
  br i1 %cmpgetmp, label %then_30, label %merge_31

merge_101:                                        ; preds = %merge_99
  %"%sc.load105" = load i64, ptr %sc1, align 8
  %cmpeqtmp106 = icmp eq i64 %"%sc.load105", 35
  br i1 %cmpeqtmp106, label %then_102, label %merge_103

merge_103:                                        ; preds = %merge_101
  %"%sc.load108" = load i64, ptr %sc1, align 8
  %cmpeqtmp109 = icmp eq i64 %"%sc.load108", 36
  br i1 %cmpeqtmp109, label %then_104, label %merge_105

merge_105:                                        ; preds = %merge_103
  %"%sc.load111" = load i64, ptr %sc1, align 8
  %cmpeqtmp112 = icmp eq i64 %"%sc.load111", 37
  br i1 %cmpeqtmp112, label %then_106, label %merge_107

merge_107:                                        ; preds = %merge_105
  %"%sc.load114" = load i64, ptr %sc1, align 8
  %cmpeqtmp115 = icmp eq i64 %"%sc.load114", 38
  br i1 %cmpeqtmp115, label %then_108, label %merge_109

merge_109:                                        ; preds = %merge_107
  %"%sc.load117" = load i64, ptr %sc1, align 8
  %cmpeqtmp118 = icmp eq i64 %"%sc.load117", 39
  br i1 %cmpeqtmp118, label %then_110, label %merge_111

merge_111:                                        ; preds = %merge_109
  %"%sc.load120" = load i64, ptr %sc1, align 8
  %cmpeqtmp121 = icmp eq i64 %"%sc.load120", 40
  br i1 %cmpeqtmp121, label %then_112, label %merge_113

merge_113:                                        ; preds = %merge_111
  %"%sc.load123" = load i64, ptr %sc1, align 8
  %cmpeqtmp124 = icmp eq i64 %"%sc.load123", 41
  br i1 %cmpeqtmp124, label %then_114, label %merge_115

merge_115:                                        ; preds = %merge_113
  %"%sc.load126" = load i64, ptr %sc1, align 8
  %cmpeqtmp127 = icmp eq i64 %"%sc.load126", 42
  br i1 %cmpeqtmp127, label %then_116, label %merge_117

merge_117:                                        ; preds = %merge_115
  %"%sc.load129" = load i64, ptr %sc1, align 8
  %cmpeqtmp130 = icmp eq i64 %"%sc.load129", 43
  br i1 %cmpeqtmp130, label %then_118, label %merge_119

merge_119:                                        ; preds = %merge_117
  %"%sc.load132" = load i64, ptr %sc1, align 8
  %cmpeqtmp133 = icmp eq i64 %"%sc.load132", 44
  br i1 %cmpeqtmp133, label %then_120, label %merge_121

merge_121:                                        ; preds = %merge_119
  %"%sc.load135" = load i64, ptr %sc1, align 8
  %cmpeqtmp136 = icmp eq i64 %"%sc.load135", 45
  br i1 %cmpeqtmp136, label %then_122, label %merge_123

merge_123:                                        ; preds = %merge_121
  %"%sc.load138" = load i64, ptr %sc1, align 8
  %cmpeqtmp139 = icmp eq i64 %"%sc.load138", 46
  br i1 %cmpeqtmp139, label %then_124, label %merge_125

merge_125:                                        ; preds = %merge_123
  %"%sc.load141" = load i64, ptr %sc1, align 8
  %cmpeqtmp142 = icmp eq i64 %"%sc.load141", 47
  br i1 %cmpeqtmp142, label %then_126, label %merge_127

merge_127:                                        ; preds = %merge_125
  %"%sc.load144" = load i64, ptr %sc1, align 8
  %cmpeqtmp145 = icmp eq i64 %"%sc.load144", 48
  br i1 %cmpeqtmp145, label %then_128, label %merge_129

merge_129:                                        ; preds = %merge_127
  %"%sc.load147" = load i64, ptr %sc1, align 8
  %cmpeqtmp148 = icmp eq i64 %"%sc.load147", 49
  br i1 %cmpeqtmp148, label %then_130, label %merge_131

merge_131:                                        ; preds = %merge_129
  %"%sc.load150" = load i64, ptr %sc1, align 8
  %cmpeqtmp151 = icmp eq i64 %"%sc.load150", 50
  br i1 %cmpeqtmp151, label %then_132, label %merge_133

merge_133:                                        ; preds = %merge_131
  %"%sc.load153" = load i64, ptr %sc1, align 8
  %cmpeqtmp154 = icmp eq i64 %"%sc.load153", 51
  br i1 %cmpeqtmp154, label %then_134, label %merge_135

merge_135:                                        ; preds = %merge_133
  %"%sc.load156" = load i64, ptr %sc1, align 8
  %cmpeqtmp157 = icmp eq i64 %"%sc.load156", 52
  br i1 %cmpeqtmp157, label %then_136, label %merge_137

merge_137:                                        ; preds = %merge_135
  %"%sc.load159" = load i64, ptr %sc1, align 8
  %cmpeqtmp160 = icmp eq i64 %"%sc.load159", 53
  br i1 %cmpeqtmp160, label %then_138, label %merge_139

merge_139:                                        ; preds = %merge_137
  %"%sc.load162" = load i64, ptr %sc1, align 8
  %cmpeqtmp163 = icmp eq i64 %"%sc.load162", 54
  br i1 %cmpeqtmp163, label %then_140, label %merge_141

merge_141:                                        ; preds = %merge_139
  %"%sc.load165" = load i64, ptr %sc1, align 8
  %cmpeqtmp166 = icmp eq i64 %"%sc.load165", 55
  br i1 %cmpeqtmp166, label %then_142, label %merge_143

merge_143:                                        ; preds = %merge_141
  %"%sc.load168" = load i64, ptr %sc1, align 8
  %cmpeqtmp169 = icmp eq i64 %"%sc.load168", 56
  br i1 %cmpeqtmp169, label %then_144, label %merge_145

merge_145:                                        ; preds = %merge_143
  %"%sc.load171" = load i64, ptr %sc1, align 8
  %cmpeqtmp172 = icmp eq i64 %"%sc.load171", 57
  br i1 %cmpeqtmp172, label %then_146, label %merge_147

merge_147:                                        ; preds = %merge_145
  %"%sc.load174" = load i64, ptr %sc1, align 8
  %cmpeqtmp175 = icmp eq i64 %"%sc.load174", 58
  br i1 %cmpeqtmp175, label %then_148, label %merge_149

merge_149:                                        ; preds = %merge_147
  %"%sc.load177" = load i64, ptr %sc1, align 8
  %cmpeqtmp178 = icmp eq i64 %"%sc.load177", 59
  br i1 %cmpeqtmp178, label %then_150, label %merge_151

merge_151:                                        ; preds = %merge_149
  %"%sc.load180" = load i64, ptr %sc1, align 8
  %cmpeqtmp181 = icmp eq i64 %"%sc.load180", 60
  br i1 %cmpeqtmp181, label %then_152, label %merge_153

merge_153:                                        ; preds = %merge_151
  %"%sc.load183" = load i64, ptr %sc1, align 8
  %cmpeqtmp184 = icmp eq i64 %"%sc.load183", 61
  br i1 %cmpeqtmp184, label %then_154, label %merge_155

merge_155:                                        ; preds = %merge_153
  %"%sc.load186" = load i64, ptr %sc1, align 8
  %cmpeqtmp187 = icmp eq i64 %"%sc.load186", 62
  br i1 %cmpeqtmp187, label %then_156, label %merge_157

merge_157:                                        ; preds = %merge_155
  %"%sc.load189" = load i64, ptr %sc1, align 8
  %cmpeqtmp190 = icmp eq i64 %"%sc.load189", 63
  br i1 %cmpeqtmp190, label %then_158, label %merge_159

merge_159:                                        ; preds = %merge_157
  ret i64 0

merge_31:                                         ; preds = %entry
  %"%sc.load2" = load i64, ptr %sc1, align 8
  %cmpeqtmp = icmp eq i64 %"%sc.load2", 0
  br i1 %cmpeqtmp, label %then_32, label %merge_33

merge_33:                                         ; preds = %merge_31
  %"%sc.load3" = load i64, ptr %sc1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%sc.load3", 1
  br i1 %cmpeqtmp4, label %then_34, label %merge_35

merge_35:                                         ; preds = %merge_33
  %"%sc.load6" = load i64, ptr %sc1, align 8
  %cmpeqtmp7 = icmp eq i64 %"%sc.load6", 2
  br i1 %cmpeqtmp7, label %then_36, label %merge_37

merge_37:                                         ; preds = %merge_35
  %"%sc.load9" = load i64, ptr %sc1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%sc.load9", 3
  br i1 %cmpeqtmp10, label %then_38, label %merge_39

merge_39:                                         ; preds = %merge_37
  %"%sc.load12" = load i64, ptr %sc1, align 8
  %cmpeqtmp13 = icmp eq i64 %"%sc.load12", 4
  br i1 %cmpeqtmp13, label %then_40, label %merge_41

merge_41:                                         ; preds = %merge_39
  %"%sc.load15" = load i64, ptr %sc1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%sc.load15", 5
  br i1 %cmpeqtmp16, label %then_42, label %merge_43

merge_43:                                         ; preds = %merge_41
  %"%sc.load18" = load i64, ptr %sc1, align 8
  %cmpeqtmp19 = icmp eq i64 %"%sc.load18", 6
  br i1 %cmpeqtmp19, label %then_44, label %merge_45

merge_45:                                         ; preds = %merge_43
  %"%sc.load21" = load i64, ptr %sc1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%sc.load21", 7
  br i1 %cmpeqtmp22, label %then_46, label %merge_47

merge_47:                                         ; preds = %merge_45
  %"%sc.load24" = load i64, ptr %sc1, align 8
  %cmpeqtmp25 = icmp eq i64 %"%sc.load24", 8
  br i1 %cmpeqtmp25, label %then_48, label %merge_49

merge_49:                                         ; preds = %merge_47
  %"%sc.load27" = load i64, ptr %sc1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%sc.load27", 9
  br i1 %cmpeqtmp28, label %then_50, label %merge_51

merge_51:                                         ; preds = %merge_49
  %"%sc.load30" = load i64, ptr %sc1, align 8
  %cmpeqtmp31 = icmp eq i64 %"%sc.load30", 10
  br i1 %cmpeqtmp31, label %then_52, label %merge_53

merge_53:                                         ; preds = %merge_51
  %"%sc.load33" = load i64, ptr %sc1, align 8
  %cmpeqtmp34 = icmp eq i64 %"%sc.load33", 11
  br i1 %cmpeqtmp34, label %then_54, label %merge_55

merge_55:                                         ; preds = %merge_53
  %"%sc.load36" = load i64, ptr %sc1, align 8
  %cmpeqtmp37 = icmp eq i64 %"%sc.load36", 12
  br i1 %cmpeqtmp37, label %then_56, label %merge_57

merge_57:                                         ; preds = %merge_55
  %"%sc.load39" = load i64, ptr %sc1, align 8
  %cmpeqtmp40 = icmp eq i64 %"%sc.load39", 13
  br i1 %cmpeqtmp40, label %then_58, label %merge_59

merge_59:                                         ; preds = %merge_57
  %"%sc.load42" = load i64, ptr %sc1, align 8
  %cmpeqtmp43 = icmp eq i64 %"%sc.load42", 14
  br i1 %cmpeqtmp43, label %then_60, label %merge_61

merge_61:                                         ; preds = %merge_59
  %"%sc.load45" = load i64, ptr %sc1, align 8
  %cmpeqtmp46 = icmp eq i64 %"%sc.load45", 15
  br i1 %cmpeqtmp46, label %then_62, label %merge_63

merge_63:                                         ; preds = %merge_61
  %"%sc.load48" = load i64, ptr %sc1, align 8
  %cmpeqtmp49 = icmp eq i64 %"%sc.load48", 16
  br i1 %cmpeqtmp49, label %then_64, label %merge_65

merge_65:                                         ; preds = %merge_63
  %"%sc.load51" = load i64, ptr %sc1, align 8
  %cmpeqtmp52 = icmp eq i64 %"%sc.load51", 17
  br i1 %cmpeqtmp52, label %then_66, label %merge_67

merge_67:                                         ; preds = %merge_65
  %"%sc.load54" = load i64, ptr %sc1, align 8
  %cmpeqtmp55 = icmp eq i64 %"%sc.load54", 18
  br i1 %cmpeqtmp55, label %then_68, label %merge_69

merge_69:                                         ; preds = %merge_67
  %"%sc.load57" = load i64, ptr %sc1, align 8
  %cmpeqtmp58 = icmp eq i64 %"%sc.load57", 19
  br i1 %cmpeqtmp58, label %then_70, label %merge_71

merge_71:                                         ; preds = %merge_69
  %"%sc.load60" = load i64, ptr %sc1, align 8
  %cmpeqtmp61 = icmp eq i64 %"%sc.load60", 20
  br i1 %cmpeqtmp61, label %then_72, label %merge_73

merge_73:                                         ; preds = %merge_71
  %"%sc.load63" = load i64, ptr %sc1, align 8
  %cmpeqtmp64 = icmp eq i64 %"%sc.load63", 21
  br i1 %cmpeqtmp64, label %then_74, label %merge_75

merge_75:                                         ; preds = %merge_73
  %"%sc.load66" = load i64, ptr %sc1, align 8
  %cmpeqtmp67 = icmp eq i64 %"%sc.load66", 22
  br i1 %cmpeqtmp67, label %then_76, label %merge_77

merge_77:                                         ; preds = %merge_75
  %"%sc.load69" = load i64, ptr %sc1, align 8
  %cmpeqtmp70 = icmp eq i64 %"%sc.load69", 23
  br i1 %cmpeqtmp70, label %then_78, label %merge_79

merge_79:                                         ; preds = %merge_77
  %"%sc.load72" = load i64, ptr %sc1, align 8
  %cmpeqtmp73 = icmp eq i64 %"%sc.load72", 24
  br i1 %cmpeqtmp73, label %then_80, label %merge_81

merge_81:                                         ; preds = %merge_79
  %"%sc.load75" = load i64, ptr %sc1, align 8
  %cmpeqtmp76 = icmp eq i64 %"%sc.load75", 25
  br i1 %cmpeqtmp76, label %then_82, label %merge_83

merge_83:                                         ; preds = %merge_81
  %"%sc.load78" = load i64, ptr %sc1, align 8
  %cmpeqtmp79 = icmp eq i64 %"%sc.load78", 26
  br i1 %cmpeqtmp79, label %then_84, label %merge_85

merge_85:                                         ; preds = %merge_83
  %"%sc.load81" = load i64, ptr %sc1, align 8
  %cmpeqtmp82 = icmp eq i64 %"%sc.load81", 27
  br i1 %cmpeqtmp82, label %then_86, label %merge_87

merge_87:                                         ; preds = %merge_85
  %"%sc.load84" = load i64, ptr %sc1, align 8
  %cmpeqtmp85 = icmp eq i64 %"%sc.load84", 28
  br i1 %cmpeqtmp85, label %then_88, label %merge_89

merge_89:                                         ; preds = %merge_87
  %"%sc.load87" = load i64, ptr %sc1, align 8
  %cmpeqtmp88 = icmp eq i64 %"%sc.load87", 29
  br i1 %cmpeqtmp88, label %then_90, label %merge_91

merge_91:                                         ; preds = %merge_89
  %"%sc.load90" = load i64, ptr %sc1, align 8
  %cmpeqtmp91 = icmp eq i64 %"%sc.load90", 30
  br i1 %cmpeqtmp91, label %then_92, label %merge_93

merge_93:                                         ; preds = %merge_91
  %"%sc.load93" = load i64, ptr %sc1, align 8
  %cmpeqtmp94 = icmp eq i64 %"%sc.load93", 31
  br i1 %cmpeqtmp94, label %then_94, label %merge_95

merge_95:                                         ; preds = %merge_93
  %"%sc.load96" = load i64, ptr %sc1, align 8
  %cmpeqtmp97 = icmp eq i64 %"%sc.load96", 32
  br i1 %cmpeqtmp97, label %then_96, label %merge_97

merge_97:                                         ; preds = %merge_95
  %"%sc.load99" = load i64, ptr %sc1, align 8
  %cmpeqtmp100 = icmp eq i64 %"%sc.load99", 33
  br i1 %cmpeqtmp100, label %then_98, label %merge_99

merge_99:                                         ; preds = %merge_97
  %"%sc.load102" = load i64, ptr %sc1, align 8
  %cmpeqtmp103 = icmp eq i64 %"%sc.load102", 34
  br i1 %cmpeqtmp103, label %then_100, label %merge_101

then_100:                                         ; preds = %merge_99
  %loadtmp104 = load i64, ptr @sc_normal_34, align 8
  ret i64 %loadtmp104

then_102:                                         ; preds = %merge_101
  %loadtmp107 = load i64, ptr @sc_normal_35, align 8
  ret i64 %loadtmp107

then_104:                                         ; preds = %merge_103
  %loadtmp110 = load i64, ptr @sc_normal_36, align 8
  ret i64 %loadtmp110

then_106:                                         ; preds = %merge_105
  %loadtmp113 = load i64, ptr @sc_normal_37, align 8
  ret i64 %loadtmp113

then_108:                                         ; preds = %merge_107
  %loadtmp116 = load i64, ptr @sc_normal_38, align 8
  ret i64 %loadtmp116

then_110:                                         ; preds = %merge_109
  %loadtmp119 = load i64, ptr @sc_normal_39, align 8
  ret i64 %loadtmp119

then_112:                                         ; preds = %merge_111
  %loadtmp122 = load i64, ptr @sc_normal_40, align 8
  ret i64 %loadtmp122

then_114:                                         ; preds = %merge_113
  %loadtmp125 = load i64, ptr @sc_normal_41, align 8
  ret i64 %loadtmp125

then_116:                                         ; preds = %merge_115
  %loadtmp128 = load i64, ptr @sc_normal_42, align 8
  ret i64 %loadtmp128

then_118:                                         ; preds = %merge_117
  %loadtmp131 = load i64, ptr @sc_normal_43, align 8
  ret i64 %loadtmp131

then_120:                                         ; preds = %merge_119
  %loadtmp134 = load i64, ptr @sc_normal_44, align 8
  ret i64 %loadtmp134

then_122:                                         ; preds = %merge_121
  %loadtmp137 = load i64, ptr @sc_normal_45, align 8
  ret i64 %loadtmp137

then_124:                                         ; preds = %merge_123
  %loadtmp140 = load i64, ptr @sc_normal_46, align 8
  ret i64 %loadtmp140

then_126:                                         ; preds = %merge_125
  %loadtmp143 = load i64, ptr @sc_normal_47, align 8
  ret i64 %loadtmp143

then_128:                                         ; preds = %merge_127
  %loadtmp146 = load i64, ptr @sc_normal_48, align 8
  ret i64 %loadtmp146

then_130:                                         ; preds = %merge_129
  %loadtmp149 = load i64, ptr @sc_normal_49, align 8
  ret i64 %loadtmp149

then_132:                                         ; preds = %merge_131
  %loadtmp152 = load i64, ptr @sc_normal_50, align 8
  ret i64 %loadtmp152

then_134:                                         ; preds = %merge_133
  %loadtmp155 = load i64, ptr @sc_normal_51, align 8
  ret i64 %loadtmp155

then_136:                                         ; preds = %merge_135
  %loadtmp158 = load i64, ptr @sc_normal_52, align 8
  ret i64 %loadtmp158

then_138:                                         ; preds = %merge_137
  %loadtmp161 = load i64, ptr @sc_normal_53, align 8
  ret i64 %loadtmp161

then_140:                                         ; preds = %merge_139
  %loadtmp164 = load i64, ptr @sc_normal_54, align 8
  ret i64 %loadtmp164

then_142:                                         ; preds = %merge_141
  %loadtmp167 = load i64, ptr @sc_normal_55, align 8
  ret i64 %loadtmp167

then_144:                                         ; preds = %merge_143
  %loadtmp170 = load i64, ptr @sc_normal_56, align 8
  ret i64 %loadtmp170

then_146:                                         ; preds = %merge_145
  %loadtmp173 = load i64, ptr @sc_normal_57, align 8
  ret i64 %loadtmp173

then_148:                                         ; preds = %merge_147
  %loadtmp176 = load i64, ptr @sc_normal_58, align 8
  ret i64 %loadtmp176

then_150:                                         ; preds = %merge_149
  %loadtmp179 = load i64, ptr @sc_normal_59, align 8
  ret i64 %loadtmp179

then_152:                                         ; preds = %merge_151
  %loadtmp182 = load i64, ptr @sc_normal_60, align 8
  ret i64 %loadtmp182

then_154:                                         ; preds = %merge_153
  %loadtmp185 = load i64, ptr @sc_normal_61, align 8
  ret i64 %loadtmp185

then_156:                                         ; preds = %merge_155
  %loadtmp188 = load i64, ptr @sc_normal_62, align 8
  ret i64 %loadtmp188

then_158:                                         ; preds = %merge_157
  %loadtmp191 = load i64, ptr @sc_normal_63, align 8
  ret i64 %loadtmp191

then_30:                                          ; preds = %entry
  ret i64 0

then_32:                                          ; preds = %merge_31
  %loadtmp = load i64, ptr @sc_normal_0, align 8
  ret i64 %loadtmp

then_34:                                          ; preds = %merge_33
  %loadtmp5 = load i64, ptr @sc_normal_1, align 8
  ret i64 %loadtmp5

then_36:                                          ; preds = %merge_35
  %loadtmp8 = load i64, ptr @sc_normal_2, align 8
  ret i64 %loadtmp8

then_38:                                          ; preds = %merge_37
  %loadtmp11 = load i64, ptr @sc_normal_3, align 8
  ret i64 %loadtmp11

then_40:                                          ; preds = %merge_39
  %loadtmp14 = load i64, ptr @sc_normal_4, align 8
  ret i64 %loadtmp14

then_42:                                          ; preds = %merge_41
  %loadtmp17 = load i64, ptr @sc_normal_5, align 8
  ret i64 %loadtmp17

then_44:                                          ; preds = %merge_43
  %loadtmp20 = load i64, ptr @sc_normal_6, align 8
  ret i64 %loadtmp20

then_46:                                          ; preds = %merge_45
  %loadtmp23 = load i64, ptr @sc_normal_7, align 8
  ret i64 %loadtmp23

then_48:                                          ; preds = %merge_47
  %loadtmp26 = load i64, ptr @sc_normal_8, align 8
  ret i64 %loadtmp26

then_50:                                          ; preds = %merge_49
  %loadtmp29 = load i64, ptr @sc_normal_9, align 8
  ret i64 %loadtmp29

then_52:                                          ; preds = %merge_51
  %loadtmp32 = load i64, ptr @sc_normal_10, align 8
  ret i64 %loadtmp32

then_54:                                          ; preds = %merge_53
  %loadtmp35 = load i64, ptr @sc_normal_11, align 8
  ret i64 %loadtmp35

then_56:                                          ; preds = %merge_55
  %loadtmp38 = load i64, ptr @sc_normal_12, align 8
  ret i64 %loadtmp38

then_58:                                          ; preds = %merge_57
  %loadtmp41 = load i64, ptr @sc_normal_13, align 8
  ret i64 %loadtmp41

then_60:                                          ; preds = %merge_59
  %loadtmp44 = load i64, ptr @sc_normal_14, align 8
  ret i64 %loadtmp44

then_62:                                          ; preds = %merge_61
  %loadtmp47 = load i64, ptr @sc_normal_15, align 8
  ret i64 %loadtmp47

then_64:                                          ; preds = %merge_63
  %loadtmp50 = load i64, ptr @sc_normal_16, align 8
  ret i64 %loadtmp50

then_66:                                          ; preds = %merge_65
  %loadtmp53 = load i64, ptr @sc_normal_17, align 8
  ret i64 %loadtmp53

then_68:                                          ; preds = %merge_67
  %loadtmp56 = load i64, ptr @sc_normal_18, align 8
  ret i64 %loadtmp56

then_70:                                          ; preds = %merge_69
  %loadtmp59 = load i64, ptr @sc_normal_19, align 8
  ret i64 %loadtmp59

then_72:                                          ; preds = %merge_71
  %loadtmp62 = load i64, ptr @sc_normal_20, align 8
  ret i64 %loadtmp62

then_74:                                          ; preds = %merge_73
  %loadtmp65 = load i64, ptr @sc_normal_21, align 8
  ret i64 %loadtmp65

then_76:                                          ; preds = %merge_75
  %loadtmp68 = load i64, ptr @sc_normal_22, align 8
  ret i64 %loadtmp68

then_78:                                          ; preds = %merge_77
  %loadtmp71 = load i64, ptr @sc_normal_23, align 8
  ret i64 %loadtmp71

then_80:                                          ; preds = %merge_79
  %loadtmp74 = load i64, ptr @sc_normal_24, align 8
  ret i64 %loadtmp74

then_82:                                          ; preds = %merge_81
  %loadtmp77 = load i64, ptr @sc_normal_25, align 8
  ret i64 %loadtmp77

then_84:                                          ; preds = %merge_83
  %loadtmp80 = load i64, ptr @sc_normal_26, align 8
  ret i64 %loadtmp80

then_86:                                          ; preds = %merge_85
  %loadtmp83 = load i64, ptr @sc_normal_27, align 8
  ret i64 %loadtmp83

then_88:                                          ; preds = %merge_87
  %loadtmp86 = load i64, ptr @sc_normal_28, align 8
  ret i64 %loadtmp86

then_90:                                          ; preds = %merge_89
  %loadtmp89 = load i64, ptr @sc_normal_29, align 8
  ret i64 %loadtmp89

then_92:                                          ; preds = %merge_91
  %loadtmp92 = load i64, ptr @sc_normal_30, align 8
  ret i64 %loadtmp92

then_94:                                          ; preds = %merge_93
  %loadtmp95 = load i64, ptr @sc_normal_31, align 8
  ret i64 %loadtmp95

then_96:                                          ; preds = %merge_95
  %loadtmp98 = load i64, ptr @sc_normal_32, align 8
  ret i64 %loadtmp98

then_98:                                          ; preds = %merge_97
  %loadtmp101 = load i64, ptr @sc_normal_33, align 8
  ret i64 %loadtmp101
}

define i64 @hid_get_shift_key(i64 %sc) {
entry:
  %sc1 = alloca i64, align 8
  store i64 %sc, ptr %sc1, align 8
  %"%sc.load" = load i64, ptr %sc1, align 8
  %cmpgetmp = icmp sge i64 %"%sc.load", 64
  br i1 %cmpgetmp, label %then_160, label %merge_161

merge_161:                                        ; preds = %entry
  %"%sc.load2" = load i64, ptr %sc1, align 8
  %cmpeqtmp = icmp eq i64 %"%sc.load2", 0
  br i1 %cmpeqtmp, label %then_162, label %merge_163

merge_163:                                        ; preds = %merge_161
  %"%sc.load3" = load i64, ptr %sc1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%sc.load3", 1
  br i1 %cmpeqtmp4, label %then_164, label %merge_165

merge_165:                                        ; preds = %merge_163
  %"%sc.load6" = load i64, ptr %sc1, align 8
  %cmpeqtmp7 = icmp eq i64 %"%sc.load6", 2
  br i1 %cmpeqtmp7, label %then_166, label %merge_167

merge_167:                                        ; preds = %merge_165
  %"%sc.load9" = load i64, ptr %sc1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%sc.load9", 3
  br i1 %cmpeqtmp10, label %then_168, label %merge_169

merge_169:                                        ; preds = %merge_167
  %"%sc.load12" = load i64, ptr %sc1, align 8
  %cmpeqtmp13 = icmp eq i64 %"%sc.load12", 4
  br i1 %cmpeqtmp13, label %then_170, label %merge_171

merge_171:                                        ; preds = %merge_169
  %"%sc.load15" = load i64, ptr %sc1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%sc.load15", 5
  br i1 %cmpeqtmp16, label %then_172, label %merge_173

merge_173:                                        ; preds = %merge_171
  %"%sc.load18" = load i64, ptr %sc1, align 8
  %cmpeqtmp19 = icmp eq i64 %"%sc.load18", 6
  br i1 %cmpeqtmp19, label %then_174, label %merge_175

merge_175:                                        ; preds = %merge_173
  %"%sc.load21" = load i64, ptr %sc1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%sc.load21", 7
  br i1 %cmpeqtmp22, label %then_176, label %merge_177

merge_177:                                        ; preds = %merge_175
  %"%sc.load24" = load i64, ptr %sc1, align 8
  %cmpeqtmp25 = icmp eq i64 %"%sc.load24", 8
  br i1 %cmpeqtmp25, label %then_178, label %merge_179

merge_179:                                        ; preds = %merge_177
  %"%sc.load27" = load i64, ptr %sc1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%sc.load27", 9
  br i1 %cmpeqtmp28, label %then_180, label %merge_181

merge_181:                                        ; preds = %merge_179
  %"%sc.load30" = load i64, ptr %sc1, align 8
  %cmpeqtmp31 = icmp eq i64 %"%sc.load30", 10
  br i1 %cmpeqtmp31, label %then_182, label %merge_183

merge_183:                                        ; preds = %merge_181
  %"%sc.load33" = load i64, ptr %sc1, align 8
  %cmpeqtmp34 = icmp eq i64 %"%sc.load33", 11
  br i1 %cmpeqtmp34, label %then_184, label %merge_185

merge_185:                                        ; preds = %merge_183
  %"%sc.load36" = load i64, ptr %sc1, align 8
  %cmpeqtmp37 = icmp eq i64 %"%sc.load36", 12
  br i1 %cmpeqtmp37, label %then_186, label %merge_187

merge_187:                                        ; preds = %merge_185
  %"%sc.load39" = load i64, ptr %sc1, align 8
  %cmpeqtmp40 = icmp eq i64 %"%sc.load39", 13
  br i1 %cmpeqtmp40, label %then_188, label %merge_189

merge_189:                                        ; preds = %merge_187
  %"%sc.load42" = load i64, ptr %sc1, align 8
  %cmpeqtmp43 = icmp eq i64 %"%sc.load42", 14
  br i1 %cmpeqtmp43, label %then_190, label %merge_191

merge_191:                                        ; preds = %merge_189
  %"%sc.load45" = load i64, ptr %sc1, align 8
  %cmpeqtmp46 = icmp eq i64 %"%sc.load45", 15
  br i1 %cmpeqtmp46, label %then_192, label %merge_193

merge_193:                                        ; preds = %merge_191
  %"%sc.load48" = load i64, ptr %sc1, align 8
  %cmpeqtmp49 = icmp eq i64 %"%sc.load48", 16
  br i1 %cmpeqtmp49, label %then_194, label %merge_195

merge_195:                                        ; preds = %merge_193
  %"%sc.load51" = load i64, ptr %sc1, align 8
  %cmpeqtmp52 = icmp eq i64 %"%sc.load51", 17
  br i1 %cmpeqtmp52, label %then_196, label %merge_197

merge_197:                                        ; preds = %merge_195
  %"%sc.load54" = load i64, ptr %sc1, align 8
  %cmpeqtmp55 = icmp eq i64 %"%sc.load54", 18
  br i1 %cmpeqtmp55, label %then_198, label %merge_199

merge_199:                                        ; preds = %merge_197
  %"%sc.load57" = load i64, ptr %sc1, align 8
  %cmpeqtmp58 = icmp eq i64 %"%sc.load57", 19
  br i1 %cmpeqtmp58, label %then_200, label %merge_201

merge_201:                                        ; preds = %merge_199
  %"%sc.load60" = load i64, ptr %sc1, align 8
  %cmpeqtmp61 = icmp eq i64 %"%sc.load60", 20
  br i1 %cmpeqtmp61, label %then_202, label %merge_203

merge_203:                                        ; preds = %merge_201
  %"%sc.load63" = load i64, ptr %sc1, align 8
  %cmpeqtmp64 = icmp eq i64 %"%sc.load63", 21
  br i1 %cmpeqtmp64, label %then_204, label %merge_205

merge_205:                                        ; preds = %merge_203
  %"%sc.load66" = load i64, ptr %sc1, align 8
  %cmpeqtmp67 = icmp eq i64 %"%sc.load66", 22
  br i1 %cmpeqtmp67, label %then_206, label %merge_207

merge_207:                                        ; preds = %merge_205
  %"%sc.load69" = load i64, ptr %sc1, align 8
  %cmpeqtmp70 = icmp eq i64 %"%sc.load69", 23
  br i1 %cmpeqtmp70, label %then_208, label %merge_209

merge_209:                                        ; preds = %merge_207
  %"%sc.load72" = load i64, ptr %sc1, align 8
  %cmpeqtmp73 = icmp eq i64 %"%sc.load72", 24
  br i1 %cmpeqtmp73, label %then_210, label %merge_211

merge_211:                                        ; preds = %merge_209
  %"%sc.load75" = load i64, ptr %sc1, align 8
  %cmpeqtmp76 = icmp eq i64 %"%sc.load75", 25
  br i1 %cmpeqtmp76, label %then_212, label %merge_213

merge_213:                                        ; preds = %merge_211
  %"%sc.load78" = load i64, ptr %sc1, align 8
  %cmpeqtmp79 = icmp eq i64 %"%sc.load78", 26
  br i1 %cmpeqtmp79, label %then_214, label %merge_215

merge_215:                                        ; preds = %merge_213
  %"%sc.load81" = load i64, ptr %sc1, align 8
  %cmpeqtmp82 = icmp eq i64 %"%sc.load81", 27
  br i1 %cmpeqtmp82, label %then_216, label %merge_217

merge_217:                                        ; preds = %merge_215
  %"%sc.load84" = load i64, ptr %sc1, align 8
  %cmpeqtmp85 = icmp eq i64 %"%sc.load84", 28
  br i1 %cmpeqtmp85, label %then_218, label %merge_219

merge_219:                                        ; preds = %merge_217
  %"%sc.load87" = load i64, ptr %sc1, align 8
  %cmpeqtmp88 = icmp eq i64 %"%sc.load87", 29
  br i1 %cmpeqtmp88, label %then_220, label %merge_221

merge_221:                                        ; preds = %merge_219
  %"%sc.load90" = load i64, ptr %sc1, align 8
  %cmpeqtmp91 = icmp eq i64 %"%sc.load90", 30
  br i1 %cmpeqtmp91, label %then_222, label %merge_223

merge_223:                                        ; preds = %merge_221
  %"%sc.load93" = load i64, ptr %sc1, align 8
  %cmpeqtmp94 = icmp eq i64 %"%sc.load93", 31
  br i1 %cmpeqtmp94, label %then_224, label %merge_225

merge_225:                                        ; preds = %merge_223
  %"%sc.load96" = load i64, ptr %sc1, align 8
  %cmpeqtmp97 = icmp eq i64 %"%sc.load96", 32
  br i1 %cmpeqtmp97, label %then_226, label %merge_227

merge_227:                                        ; preds = %merge_225
  %"%sc.load99" = load i64, ptr %sc1, align 8
  %cmpeqtmp100 = icmp eq i64 %"%sc.load99", 33
  br i1 %cmpeqtmp100, label %then_228, label %merge_229

merge_229:                                        ; preds = %merge_227
  %"%sc.load102" = load i64, ptr %sc1, align 8
  %cmpeqtmp103 = icmp eq i64 %"%sc.load102", 34
  br i1 %cmpeqtmp103, label %then_230, label %merge_231

merge_231:                                        ; preds = %merge_229
  %"%sc.load105" = load i64, ptr %sc1, align 8
  %cmpeqtmp106 = icmp eq i64 %"%sc.load105", 35
  br i1 %cmpeqtmp106, label %then_232, label %merge_233

merge_233:                                        ; preds = %merge_231
  %"%sc.load108" = load i64, ptr %sc1, align 8
  %cmpeqtmp109 = icmp eq i64 %"%sc.load108", 36
  br i1 %cmpeqtmp109, label %then_234, label %merge_235

merge_235:                                        ; preds = %merge_233
  %"%sc.load111" = load i64, ptr %sc1, align 8
  %cmpeqtmp112 = icmp eq i64 %"%sc.load111", 37
  br i1 %cmpeqtmp112, label %then_236, label %merge_237

merge_237:                                        ; preds = %merge_235
  %"%sc.load114" = load i64, ptr %sc1, align 8
  %cmpeqtmp115 = icmp eq i64 %"%sc.load114", 38
  br i1 %cmpeqtmp115, label %then_238, label %merge_239

merge_239:                                        ; preds = %merge_237
  %"%sc.load117" = load i64, ptr %sc1, align 8
  %cmpeqtmp118 = icmp eq i64 %"%sc.load117", 39
  br i1 %cmpeqtmp118, label %then_240, label %merge_241

merge_241:                                        ; preds = %merge_239
  %"%sc.load120" = load i64, ptr %sc1, align 8
  %cmpeqtmp121 = icmp eq i64 %"%sc.load120", 40
  br i1 %cmpeqtmp121, label %then_242, label %merge_243

merge_243:                                        ; preds = %merge_241
  %"%sc.load123" = load i64, ptr %sc1, align 8
  %cmpeqtmp124 = icmp eq i64 %"%sc.load123", 41
  br i1 %cmpeqtmp124, label %then_244, label %merge_245

merge_245:                                        ; preds = %merge_243
  %"%sc.load126" = load i64, ptr %sc1, align 8
  %cmpeqtmp127 = icmp eq i64 %"%sc.load126", 42
  br i1 %cmpeqtmp127, label %then_246, label %merge_247

merge_247:                                        ; preds = %merge_245
  %"%sc.load129" = load i64, ptr %sc1, align 8
  %cmpeqtmp130 = icmp eq i64 %"%sc.load129", 43
  br i1 %cmpeqtmp130, label %then_248, label %merge_249

merge_249:                                        ; preds = %merge_247
  %"%sc.load132" = load i64, ptr %sc1, align 8
  %cmpeqtmp133 = icmp eq i64 %"%sc.load132", 44
  br i1 %cmpeqtmp133, label %then_250, label %merge_251

merge_251:                                        ; preds = %merge_249
  %"%sc.load135" = load i64, ptr %sc1, align 8
  %cmpeqtmp136 = icmp eq i64 %"%sc.load135", 45
  br i1 %cmpeqtmp136, label %then_252, label %merge_253

merge_253:                                        ; preds = %merge_251
  %"%sc.load138" = load i64, ptr %sc1, align 8
  %cmpeqtmp139 = icmp eq i64 %"%sc.load138", 46
  br i1 %cmpeqtmp139, label %then_254, label %merge_255

merge_255:                                        ; preds = %merge_253
  %"%sc.load141" = load i64, ptr %sc1, align 8
  %cmpeqtmp142 = icmp eq i64 %"%sc.load141", 47
  br i1 %cmpeqtmp142, label %then_256, label %merge_257

merge_257:                                        ; preds = %merge_255
  %"%sc.load144" = load i64, ptr %sc1, align 8
  %cmpeqtmp145 = icmp eq i64 %"%sc.load144", 48
  br i1 %cmpeqtmp145, label %then_258, label %merge_259

merge_259:                                        ; preds = %merge_257
  %"%sc.load147" = load i64, ptr %sc1, align 8
  %cmpeqtmp148 = icmp eq i64 %"%sc.load147", 49
  br i1 %cmpeqtmp148, label %then_260, label %merge_261

merge_261:                                        ; preds = %merge_259
  %"%sc.load150" = load i64, ptr %sc1, align 8
  %cmpeqtmp151 = icmp eq i64 %"%sc.load150", 50
  br i1 %cmpeqtmp151, label %then_262, label %merge_263

merge_263:                                        ; preds = %merge_261
  %"%sc.load153" = load i64, ptr %sc1, align 8
  %cmpeqtmp154 = icmp eq i64 %"%sc.load153", 51
  br i1 %cmpeqtmp154, label %then_264, label %merge_265

merge_265:                                        ; preds = %merge_263
  %"%sc.load156" = load i64, ptr %sc1, align 8
  %cmpeqtmp157 = icmp eq i64 %"%sc.load156", 52
  br i1 %cmpeqtmp157, label %then_266, label %merge_267

merge_267:                                        ; preds = %merge_265
  %"%sc.load159" = load i64, ptr %sc1, align 8
  %cmpeqtmp160 = icmp eq i64 %"%sc.load159", 53
  br i1 %cmpeqtmp160, label %then_268, label %merge_269

merge_269:                                        ; preds = %merge_267
  %"%sc.load162" = load i64, ptr %sc1, align 8
  %cmpeqtmp163 = icmp eq i64 %"%sc.load162", 54
  br i1 %cmpeqtmp163, label %then_270, label %merge_271

merge_271:                                        ; preds = %merge_269
  %"%sc.load165" = load i64, ptr %sc1, align 8
  %cmpeqtmp166 = icmp eq i64 %"%sc.load165", 55
  br i1 %cmpeqtmp166, label %then_272, label %merge_273

merge_273:                                        ; preds = %merge_271
  %"%sc.load168" = load i64, ptr %sc1, align 8
  %cmpeqtmp169 = icmp eq i64 %"%sc.load168", 56
  br i1 %cmpeqtmp169, label %then_274, label %merge_275

merge_275:                                        ; preds = %merge_273
  %"%sc.load171" = load i64, ptr %sc1, align 8
  %cmpeqtmp172 = icmp eq i64 %"%sc.load171", 57
  br i1 %cmpeqtmp172, label %then_276, label %merge_277

merge_277:                                        ; preds = %merge_275
  %"%sc.load174" = load i64, ptr %sc1, align 8
  %cmpeqtmp175 = icmp eq i64 %"%sc.load174", 58
  br i1 %cmpeqtmp175, label %then_278, label %merge_279

merge_279:                                        ; preds = %merge_277
  %"%sc.load177" = load i64, ptr %sc1, align 8
  %cmpeqtmp178 = icmp eq i64 %"%sc.load177", 59
  br i1 %cmpeqtmp178, label %then_280, label %merge_281

merge_281:                                        ; preds = %merge_279
  %"%sc.load180" = load i64, ptr %sc1, align 8
  %cmpeqtmp181 = icmp eq i64 %"%sc.load180", 60
  br i1 %cmpeqtmp181, label %then_282, label %merge_283

merge_283:                                        ; preds = %merge_281
  %"%sc.load183" = load i64, ptr %sc1, align 8
  %cmpeqtmp184 = icmp eq i64 %"%sc.load183", 61
  br i1 %cmpeqtmp184, label %then_284, label %merge_285

merge_285:                                        ; preds = %merge_283
  %"%sc.load186" = load i64, ptr %sc1, align 8
  %cmpeqtmp187 = icmp eq i64 %"%sc.load186", 62
  br i1 %cmpeqtmp187, label %then_286, label %merge_287

merge_287:                                        ; preds = %merge_285
  %"%sc.load189" = load i64, ptr %sc1, align 8
  %cmpeqtmp190 = icmp eq i64 %"%sc.load189", 63
  br i1 %cmpeqtmp190, label %then_288, label %merge_289

merge_289:                                        ; preds = %merge_287
  ret i64 0

then_160:                                         ; preds = %entry
  ret i64 0

then_162:                                         ; preds = %merge_161
  %loadtmp = load i64, ptr @sc_shift_0, align 8
  ret i64 %loadtmp

then_164:                                         ; preds = %merge_163
  %loadtmp5 = load i64, ptr @sc_shift_1, align 8
  ret i64 %loadtmp5

then_166:                                         ; preds = %merge_165
  %loadtmp8 = load i64, ptr @sc_shift_2, align 8
  ret i64 %loadtmp8

then_168:                                         ; preds = %merge_167
  %loadtmp11 = load i64, ptr @sc_shift_3, align 8
  ret i64 %loadtmp11

then_170:                                         ; preds = %merge_169
  %loadtmp14 = load i64, ptr @sc_shift_4, align 8
  ret i64 %loadtmp14

then_172:                                         ; preds = %merge_171
  %loadtmp17 = load i64, ptr @sc_shift_5, align 8
  ret i64 %loadtmp17

then_174:                                         ; preds = %merge_173
  %loadtmp20 = load i64, ptr @sc_shift_6, align 8
  ret i64 %loadtmp20

then_176:                                         ; preds = %merge_175
  %loadtmp23 = load i64, ptr @sc_shift_7, align 8
  ret i64 %loadtmp23

then_178:                                         ; preds = %merge_177
  %loadtmp26 = load i64, ptr @sc_shift_8, align 8
  ret i64 %loadtmp26

then_180:                                         ; preds = %merge_179
  %loadtmp29 = load i64, ptr @sc_shift_9, align 8
  ret i64 %loadtmp29

then_182:                                         ; preds = %merge_181
  %loadtmp32 = load i64, ptr @sc_shift_10, align 8
  ret i64 %loadtmp32

then_184:                                         ; preds = %merge_183
  %loadtmp35 = load i64, ptr @sc_shift_11, align 8
  ret i64 %loadtmp35

then_186:                                         ; preds = %merge_185
  %loadtmp38 = load i64, ptr @sc_shift_12, align 8
  ret i64 %loadtmp38

then_188:                                         ; preds = %merge_187
  %loadtmp41 = load i64, ptr @sc_shift_13, align 8
  ret i64 %loadtmp41

then_190:                                         ; preds = %merge_189
  %loadtmp44 = load i64, ptr @sc_shift_14, align 8
  ret i64 %loadtmp44

then_192:                                         ; preds = %merge_191
  %loadtmp47 = load i64, ptr @sc_shift_15, align 8
  ret i64 %loadtmp47

then_194:                                         ; preds = %merge_193
  %loadtmp50 = load i64, ptr @sc_shift_16, align 8
  ret i64 %loadtmp50

then_196:                                         ; preds = %merge_195
  %loadtmp53 = load i64, ptr @sc_shift_17, align 8
  ret i64 %loadtmp53

then_198:                                         ; preds = %merge_197
  %loadtmp56 = load i64, ptr @sc_shift_18, align 8
  ret i64 %loadtmp56

then_200:                                         ; preds = %merge_199
  %loadtmp59 = load i64, ptr @sc_shift_19, align 8
  ret i64 %loadtmp59

then_202:                                         ; preds = %merge_201
  %loadtmp62 = load i64, ptr @sc_shift_20, align 8
  ret i64 %loadtmp62

then_204:                                         ; preds = %merge_203
  %loadtmp65 = load i64, ptr @sc_shift_21, align 8
  ret i64 %loadtmp65

then_206:                                         ; preds = %merge_205
  %loadtmp68 = load i64, ptr @sc_shift_22, align 8
  ret i64 %loadtmp68

then_208:                                         ; preds = %merge_207
  %loadtmp71 = load i64, ptr @sc_shift_23, align 8
  ret i64 %loadtmp71

then_210:                                         ; preds = %merge_209
  %loadtmp74 = load i64, ptr @sc_shift_24, align 8
  ret i64 %loadtmp74

then_212:                                         ; preds = %merge_211
  %loadtmp77 = load i64, ptr @sc_shift_25, align 8
  ret i64 %loadtmp77

then_214:                                         ; preds = %merge_213
  %loadtmp80 = load i64, ptr @sc_shift_26, align 8
  ret i64 %loadtmp80

then_216:                                         ; preds = %merge_215
  %loadtmp83 = load i64, ptr @sc_shift_27, align 8
  ret i64 %loadtmp83

then_218:                                         ; preds = %merge_217
  %loadtmp86 = load i64, ptr @sc_shift_28, align 8
  ret i64 %loadtmp86

then_220:                                         ; preds = %merge_219
  %loadtmp89 = load i64, ptr @sc_shift_29, align 8
  ret i64 %loadtmp89

then_222:                                         ; preds = %merge_221
  %loadtmp92 = load i64, ptr @sc_shift_30, align 8
  ret i64 %loadtmp92

then_224:                                         ; preds = %merge_223
  %loadtmp95 = load i64, ptr @sc_shift_31, align 8
  ret i64 %loadtmp95

then_226:                                         ; preds = %merge_225
  %loadtmp98 = load i64, ptr @sc_shift_32, align 8
  ret i64 %loadtmp98

then_228:                                         ; preds = %merge_227
  %loadtmp101 = load i64, ptr @sc_shift_33, align 8
  ret i64 %loadtmp101

then_230:                                         ; preds = %merge_229
  %loadtmp104 = load i64, ptr @sc_shift_34, align 8
  ret i64 %loadtmp104

then_232:                                         ; preds = %merge_231
  %loadtmp107 = load i64, ptr @sc_shift_35, align 8
  ret i64 %loadtmp107

then_234:                                         ; preds = %merge_233
  %loadtmp110 = load i64, ptr @sc_shift_36, align 8
  ret i64 %loadtmp110

then_236:                                         ; preds = %merge_235
  %loadtmp113 = load i64, ptr @sc_shift_37, align 8
  ret i64 %loadtmp113

then_238:                                         ; preds = %merge_237
  %loadtmp116 = load i64, ptr @sc_shift_38, align 8
  ret i64 %loadtmp116

then_240:                                         ; preds = %merge_239
  %loadtmp119 = load i64, ptr @sc_shift_39, align 8
  ret i64 %loadtmp119

then_242:                                         ; preds = %merge_241
  %loadtmp122 = load i64, ptr @sc_shift_40, align 8
  ret i64 %loadtmp122

then_244:                                         ; preds = %merge_243
  %loadtmp125 = load i64, ptr @sc_shift_41, align 8
  ret i64 %loadtmp125

then_246:                                         ; preds = %merge_245
  %loadtmp128 = load i64, ptr @sc_shift_42, align 8
  ret i64 %loadtmp128

then_248:                                         ; preds = %merge_247
  %loadtmp131 = load i64, ptr @sc_shift_43, align 8
  ret i64 %loadtmp131

then_250:                                         ; preds = %merge_249
  %loadtmp134 = load i64, ptr @sc_shift_44, align 8
  ret i64 %loadtmp134

then_252:                                         ; preds = %merge_251
  %loadtmp137 = load i64, ptr @sc_shift_45, align 8
  ret i64 %loadtmp137

then_254:                                         ; preds = %merge_253
  %loadtmp140 = load i64, ptr @sc_shift_46, align 8
  ret i64 %loadtmp140

then_256:                                         ; preds = %merge_255
  %loadtmp143 = load i64, ptr @sc_shift_47, align 8
  ret i64 %loadtmp143

then_258:                                         ; preds = %merge_257
  %loadtmp146 = load i64, ptr @sc_shift_48, align 8
  ret i64 %loadtmp146

then_260:                                         ; preds = %merge_259
  %loadtmp149 = load i64, ptr @sc_shift_49, align 8
  ret i64 %loadtmp149

then_262:                                         ; preds = %merge_261
  %loadtmp152 = load i64, ptr @sc_shift_50, align 8
  ret i64 %loadtmp152

then_264:                                         ; preds = %merge_263
  %loadtmp155 = load i64, ptr @sc_shift_51, align 8
  ret i64 %loadtmp155

then_266:                                         ; preds = %merge_265
  %loadtmp158 = load i64, ptr @sc_shift_52, align 8
  ret i64 %loadtmp158

then_268:                                         ; preds = %merge_267
  %loadtmp161 = load i64, ptr @sc_shift_53, align 8
  ret i64 %loadtmp161

then_270:                                         ; preds = %merge_269
  %loadtmp164 = load i64, ptr @sc_shift_54, align 8
  ret i64 %loadtmp164

then_272:                                         ; preds = %merge_271
  %loadtmp167 = load i64, ptr @sc_shift_55, align 8
  ret i64 %loadtmp167

then_274:                                         ; preds = %merge_273
  %loadtmp170 = load i64, ptr @sc_shift_56, align 8
  ret i64 %loadtmp170

then_276:                                         ; preds = %merge_275
  %loadtmp173 = load i64, ptr @sc_shift_57, align 8
  ret i64 %loadtmp173

then_278:                                         ; preds = %merge_277
  %loadtmp176 = load i64, ptr @sc_shift_58, align 8
  ret i64 %loadtmp176

then_280:                                         ; preds = %merge_279
  %loadtmp179 = load i64, ptr @sc_shift_59, align 8
  ret i64 %loadtmp179

then_282:                                         ; preds = %merge_281
  %loadtmp182 = load i64, ptr @sc_shift_60, align 8
  ret i64 %loadtmp182

then_284:                                         ; preds = %merge_283
  %loadtmp185 = load i64, ptr @sc_shift_61, align 8
  ret i64 %loadtmp185

then_286:                                         ; preds = %merge_285
  %loadtmp188 = load i64, ptr @sc_shift_62, align 8
  ret i64 %loadtmp188

then_288:                                         ; preds = %merge_287
  %loadtmp191 = load i64, ptr @sc_shift_63, align 8
  ret i64 %loadtmp191
}

define i64 @hid_scancode_to_ascii(i64 %sc, i64 %modifiers) {
entry:
  %"%shifted" = alloca i64, align 8
  %modifiers2 = alloca i64, align 8
  store i64 %modifiers, ptr %modifiers2, align 8
  %sc1 = alloca i64, align 8
  store i64 %sc, ptr %sc1, align 8
  %"%sc.load" = load i64, ptr %sc1, align 8
  %cmpeqtmp = icmp eq i64 %"%sc.load", 0
  br i1 %cmpeqtmp, label %then_290, label %merge_291

else_306:                                         ; preds = %merge_299
  %"%sc.load16" = load i64, ptr %sc1, align 8
  %calltmp17 = call i64 @hid_get_normal_key(i64 %"%sc.load16")
  ret i64 %calltmp17

merge_291:                                        ; preds = %entry
  %"%sc.load3" = load i64, ptr %sc1, align 8
  %cmpgetmp = icmp sge i64 %"%sc.load3", 64
  br i1 %cmpgetmp, label %then_292, label %merge_293

merge_293:                                        ; preds = %merge_291
  store i64 0, ptr %"%shifted", align 8
  %"%modifiers.load" = load i64, ptr %modifiers2, align 8
  %"%MOD_LSHIFT.load" = load i64, ptr @MOD_LSHIFT, align 8
  %divtmp = sdiv i64 %"%modifiers.load", %"%MOD_LSHIFT.load"
  %andtmp = and i64 %divtmp, 1
  %cmpeqtmp4 = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp4, label %then_294, label %merge_295

merge_295:                                        ; preds = %then_294, %merge_293
  %"%modifiers.load5" = load i64, ptr %modifiers2, align 8
  %"%MOD_RSHIFT.load" = load i64, ptr @MOD_RSHIFT, align 8
  %divtmp6 = sdiv i64 %"%modifiers.load5", %"%MOD_RSHIFT.load"
  %andtmp7 = and i64 %divtmp6, 1
  %cmpeqtmp8 = icmp eq i64 %andtmp7, 1
  br i1 %cmpeqtmp8, label %then_296, label %merge_297

merge_297:                                        ; preds = %then_296, %merge_295
  %"%kbd_capslock.load" = load i64, ptr @kbd_capslock, align 8
  %cmpeqtmp9 = icmp eq i64 %"%kbd_capslock.load", 1
  br i1 %cmpeqtmp9, label %then_298, label %merge_299

merge_299:                                        ; preds = %merge_301, %merge_297
  %"%shifted.load" = load i64, ptr %"%shifted", align 8
  %cmpeqtmp12 = icmp eq i64 %"%shifted.load", 1
  br i1 %cmpeqtmp12, label %then_304, label %else_306

merge_301:                                        ; preds = %merge_303, %then_298
  br label %merge_299

merge_303:                                        ; preds = %then_302, %then_300
  br label %merge_301

then_290:                                         ; preds = %entry
  ret i64 0

then_292:                                         ; preds = %merge_291
  ret i64 0

then_294:                                         ; preds = %merge_293
  store i64 1, ptr %"%shifted", align 8
  br label %merge_295

then_296:                                         ; preds = %merge_295
  store i64 1, ptr %"%shifted", align 8
  br label %merge_297

then_298:                                         ; preds = %merge_297
  %"%sc.load10" = load i64, ptr %sc1, align 8
  %cmpgetmp11 = icmp sge i64 %"%sc.load10", 4
  br i1 %cmpgetmp11, label %then_300, label %merge_301

then_300:                                         ; preds = %then_298
  %"%sc.load13" = load i64, ptr %sc1, align 8
  %cmpletmp = icmp sle i64 %"%sc.load13", 29
  br i1 %cmpletmp, label %then_302, label %merge_303

then_302:                                         ; preds = %then_300
  %"%shifted.load14" = load i64, ptr %"%shifted", align 8
  %subtmp = sub i64 1, %"%shifted.load14"
  store i64 %subtmp, ptr %"%shifted", align 8
  br label %merge_303

then_304:                                         ; preds = %merge_299
  %"%sc.load15" = load i64, ptr %sc1, align 8
  %calltmp = call i64 @hid_get_shift_key(i64 %"%sc.load15")
  ret i64 %calltmp
}

define i64 @hid_kbd_push_key(i64 %ascii_code) {
entry:
  %"%hid_kbd_events" = alloca i64, align 8
  %"%kbd_event_tail" = alloca i64, align 8
  %"%next_tail" = alloca i64, align 8
  %ascii_code1 = alloca i64, align 8
  store i64 %ascii_code, ptr %ascii_code1, align 8
  %"%ascii_code.load" = load i64, ptr %ascii_code1, align 8
  %cmpeqtmp = icmp eq i64 %"%ascii_code.load", 0
  br i1 %cmpeqtmp, label %then_307, label %merge_308

merge_308:                                        ; preds = %entry
  %"%kbd_event_tail.load" = load i64, ptr @kbd_event_tail, align 8
  %addtmp = add i64 %"%kbd_event_tail.load", 1
  %"%KBD_BUF_SIZE.load" = load i64, ptr @KBD_BUF_SIZE, align 8
  %modtmp = srem i64 %addtmp, %"%KBD_BUF_SIZE.load"
  store i64 %modtmp, ptr %"%next_tail", align 8
  %"%next_tail.load" = load i64, ptr %"%next_tail", align 8
  %"%kbd_event_head.load" = load i64, ptr @kbd_event_head, align 8
  %cmpeqtmp2 = icmp eq i64 %"%next_tail.load", %"%kbd_event_head.load"
  br i1 %cmpeqtmp2, label %then_309, label %merge_310

merge_310:                                        ; preds = %merge_308
  %"%kbd_event_buf.load" = load i64, ptr @kbd_event_buf, align 8
  %"%kbd_event_tail.load3" = load i64, ptr @kbd_event_tail, align 8
  %addtmp4 = add i64 %"%kbd_event_buf.load", %"%kbd_event_tail.load3"
  %"%803.load" = load i64, ptr %ascii_code1, align 8
  call void @sad_ll_mem_write8(i64 %addtmp4, i64 %"%803.load")
  %"%806.load" = load i64, ptr %"%next_tail", align 8
  store i64 %"%806.load", ptr %"%kbd_event_tail", align 8
  %"%hid_kbd_events.load" = load i64, ptr @hid_kbd_events, align 8
  %addtmp5 = add i64 %"%hid_kbd_events.load", 1
  store i64 %addtmp5, ptr %"%hid_kbd_events", align 8
  ret i64 1

then_307:                                         ; preds = %entry
  ret i64 0

then_309:                                         ; preds = %merge_308
  ret i64 0
}

define i64 @hid_kbd_pop_key() {
entry:
  %"%kbd_event_head" = alloca i64, align 8
  %"%ch" = alloca i64, align 8
  %"%kbd_event_head.load" = load i64, ptr @kbd_event_head, align 8
  %"%kbd_event_tail.load" = load i64, ptr @kbd_event_tail, align 8
  %cmpeqtmp = icmp eq i64 %"%kbd_event_head.load", %"%kbd_event_tail.load"
  br i1 %cmpeqtmp, label %then_311, label %merge_312

merge_312:                                        ; preds = %entry
  %"%kbd_event_buf.load" = load i64, ptr @kbd_event_buf, align 8
  %"%kbd_event_head.load1" = load i64, ptr @kbd_event_head, align 8
  %addtmp = add i64 %"%kbd_event_buf.load", %"%kbd_event_head.load1"
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%ch", align 8
  %"%kbd_event_head.load2" = load i64, ptr @kbd_event_head, align 8
  %addtmp3 = add i64 %"%kbd_event_head.load2", 1
  %"%KBD_BUF_SIZE.load" = load i64, ptr @KBD_BUF_SIZE, align 8
  %modtmp = srem i64 %addtmp3, %"%KBD_BUF_SIZE.load"
  store i64 %modtmp, ptr %"%kbd_event_head", align 8
  %loadtmp = load i64, ptr %"%ch", align 8
  ret i64 %loadtmp

then_311:                                         ; preds = %entry
  ret i64 0
}

define i64 @hid_kbd_has_key() {
entry:
  %"%kbd_event_head.load" = load i64, ptr @kbd_event_head, align 8
  %"%kbd_event_tail.load" = load i64, ptr @kbd_event_tail, align 8
  %cmpnetmp = icmp ne i64 %"%kbd_event_head.load", %"%kbd_event_tail.load"
  br i1 %cmpnetmp, label %then_313, label %merge_314

merge_314:                                        ; preds = %entry
  ret i64 0

then_313:                                         ; preds = %entry
  ret i64 1
}

define i64 @hid_kbd_get_modifiers() {
entry:
  %loadtmp = load i64, ptr @kbd_modifiers, align 8
  ret i64 %loadtmp
}

define i64 @hid_process_kbd_report(i64 %buf) {
entry:
  %"%ascii" = alloca i64, align 8
  %"%kbd_capslock" = alloca i64, align 8
  %"%prev_5" = alloca i64, align 8
  %"%prev_4" = alloca i64, align 8
  %"%prev_3" = alloca i64, align 8
  %"%prev_2" = alloca i64, align 8
  %"%prev_1" = alloca i64, align 8
  %"%prev_0" = alloca i64, align 8
  %"%is_new" = alloca i64, align 8
  %"%kbd_pressed_5" = alloca i64, align 8
  %"%kbd_pressed_4" = alloca i64, align 8
  %"%kbd_pressed_3" = alloca i64, align 8
  %"%kbd_pressed_2" = alloca i64, align 8
  %"%kbd_pressed_1" = alloca i64, align 8
  %"%kbd_pressed_0" = alloca i64, align 8
  %"%scancode" = alloca i64, align 8
  %"%k" = alloca i64, align 8
  %"%kbd_modifiers" = alloca i64, align 8
  %"%mods" = alloca i64, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  call void @sad_ll_mem_read8(i64 %"%buf.load")
  store i64 0, ptr %"%mods", align 8
  %"%832.load" = load i64, ptr %"%mods", align 8
  store i64 %"%832.load", ptr %"%kbd_modifiers", align 8
  store i64 2, ptr %"%k", align 8
  br label %while_cond_315

else_338:                                         ; preds = %then_334
  %"%scancode.load47" = load i64, ptr %"%scancode", align 8
  %"%mods.load" = load i64, ptr %"%mods", align 8
  %calltmp = call i64 @hid_scancode_to_ascii(i64 %"%scancode.load47", i64 %"%mods.load")
  store i64 %calltmp, ptr %"%ascii", align 8
  %"%ascii.load" = load i64, ptr %"%ascii", align 8
  %cmpnetmp48 = icmp ne i64 %"%ascii.load", 0
  br i1 %cmpnetmp48, label %then_339, label %merge_340

merge_319:                                        ; preds = %merge_321, %while_body_316
  %"%k.load19" = load i64, ptr %"%k", align 8
  %addtmp20 = add i64 %"%k.load19", 1
  store i64 %addtmp20, ptr %"%k", align 8
  br label %while_cond_315

merge_321:                                        ; preds = %merge_335, %then_318
  br label %merge_319

merge_323:                                        ; preds = %then_322, %then_320
  %"%scancode.load34" = load i64, ptr %"%scancode", align 8
  %"%prev_1.load" = load i64, ptr %"%prev_1", align 8
  %cmpeqtmp35 = icmp eq i64 %"%scancode.load34", %"%prev_1.load"
  br i1 %cmpeqtmp35, label %then_324, label %merge_325

merge_325:                                        ; preds = %then_324, %merge_323
  %"%scancode.load36" = load i64, ptr %"%scancode", align 8
  %"%prev_2.load" = load i64, ptr %"%prev_2", align 8
  %cmpeqtmp37 = icmp eq i64 %"%scancode.load36", %"%prev_2.load"
  br i1 %cmpeqtmp37, label %then_326, label %merge_327

merge_327:                                        ; preds = %then_326, %merge_325
  %"%scancode.load38" = load i64, ptr %"%scancode", align 8
  %"%prev_3.load" = load i64, ptr %"%prev_3", align 8
  %cmpeqtmp39 = icmp eq i64 %"%scancode.load38", %"%prev_3.load"
  br i1 %cmpeqtmp39, label %then_328, label %merge_329

merge_329:                                        ; preds = %then_328, %merge_327
  %"%scancode.load40" = load i64, ptr %"%scancode", align 8
  %"%prev_4.load" = load i64, ptr %"%prev_4", align 8
  %cmpeqtmp41 = icmp eq i64 %"%scancode.load40", %"%prev_4.load"
  br i1 %cmpeqtmp41, label %then_330, label %merge_331

merge_331:                                        ; preds = %then_330, %merge_329
  %"%scancode.load42" = load i64, ptr %"%scancode", align 8
  %"%prev_5.load" = load i64, ptr %"%prev_5", align 8
  %cmpeqtmp43 = icmp eq i64 %"%scancode.load42", %"%prev_5.load"
  br i1 %cmpeqtmp43, label %then_332, label %merge_333

merge_333:                                        ; preds = %then_332, %merge_331
  %"%is_new.load" = load i64, ptr %"%is_new", align 8
  %cmpeqtmp44 = icmp eq i64 %"%is_new.load", 1
  br i1 %cmpeqtmp44, label %then_334, label %merge_335

merge_335:                                        ; preds = %merge_337, %merge_333
  br label %merge_321

merge_337:                                        ; preds = %merge_340, %then_336
  br label %merge_335

merge_340:                                        ; preds = %then_339, %else_338
  br label %merge_337

then_318:                                         ; preds = %while_body_316
  %"%scancode.load17" = load i64, ptr %"%scancode", align 8
  %cmpnetmp18 = icmp ne i64 %"%scancode.load17", 1
  br i1 %cmpnetmp18, label %then_320, label %merge_321

then_320:                                         ; preds = %then_318
  store i64 1, ptr %"%is_new", align 8
  %"%kbd_prev_report.load21" = load i64, ptr @kbd_prev_report, align 8
  %addtmp22 = add i64 %"%kbd_prev_report.load21", 2
  call void @sad_ll_mem_read8(i64 %addtmp22)
  store i64 0, ptr %"%prev_0", align 8
  %"%kbd_prev_report.load23" = load i64, ptr @kbd_prev_report, align 8
  %addtmp24 = add i64 %"%kbd_prev_report.load23", 3
  call void @sad_ll_mem_read8(i64 %addtmp24)
  store i64 0, ptr %"%prev_1", align 8
  %"%kbd_prev_report.load25" = load i64, ptr @kbd_prev_report, align 8
  %addtmp26 = add i64 %"%kbd_prev_report.load25", 4
  call void @sad_ll_mem_read8(i64 %addtmp26)
  store i64 0, ptr %"%prev_2", align 8
  %"%kbd_prev_report.load27" = load i64, ptr @kbd_prev_report, align 8
  %addtmp28 = add i64 %"%kbd_prev_report.load27", 5
  call void @sad_ll_mem_read8(i64 %addtmp28)
  store i64 0, ptr %"%prev_3", align 8
  %"%kbd_prev_report.load29" = load i64, ptr @kbd_prev_report, align 8
  %addtmp30 = add i64 %"%kbd_prev_report.load29", 6
  call void @sad_ll_mem_read8(i64 %addtmp30)
  store i64 0, ptr %"%prev_4", align 8
  %"%kbd_prev_report.load31" = load i64, ptr @kbd_prev_report, align 8
  %addtmp32 = add i64 %"%kbd_prev_report.load31", 7
  call void @sad_ll_mem_read8(i64 %addtmp32)
  store i64 0, ptr %"%prev_5", align 8
  %"%scancode.load33" = load i64, ptr %"%scancode", align 8
  %"%prev_0.load" = load i64, ptr %"%prev_0", align 8
  %cmpeqtmp = icmp eq i64 %"%scancode.load33", %"%prev_0.load"
  br i1 %cmpeqtmp, label %then_322, label %merge_323

then_322:                                         ; preds = %then_320
  store i64 0, ptr %"%is_new", align 8
  br label %merge_323

then_324:                                         ; preds = %merge_323
  store i64 0, ptr %"%is_new", align 8
  br label %merge_325

then_326:                                         ; preds = %merge_325
  store i64 0, ptr %"%is_new", align 8
  br label %merge_327

then_328:                                         ; preds = %merge_327
  store i64 0, ptr %"%is_new", align 8
  br label %merge_329

then_330:                                         ; preds = %merge_329
  store i64 0, ptr %"%is_new", align 8
  br label %merge_331

then_332:                                         ; preds = %merge_331
  store i64 0, ptr %"%is_new", align 8
  br label %merge_333

then_334:                                         ; preds = %merge_333
  %"%scancode.load45" = load i64, ptr %"%scancode", align 8
  %cmpeqtmp46 = icmp eq i64 %"%scancode.load45", 57
  br i1 %cmpeqtmp46, label %then_336, label %else_338

then_336:                                         ; preds = %then_334
  %"%kbd_capslock.load" = load i64, ptr @kbd_capslock, align 8
  %subtmp = sub i64 1, %"%kbd_capslock.load"
  store i64 %subtmp, ptr %"%kbd_capslock", align 8
  br label %merge_337

then_339:                                         ; preds = %else_338
  %"%ascii.load49" = load i64, ptr %"%ascii", align 8
  %calltmp50 = call i64 @hid_kbd_push_key(i64 %"%ascii.load49")
  br label %merge_340

while_body_316:                                   ; preds = %while_cond_315
  %"%buf.load2" = load i64, ptr %buf1, align 8
  %"%k.load3" = load i64, ptr %"%k", align 8
  %addtmp = add i64 %"%buf.load2", %"%k.load3"
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%scancode", align 8
  %"%scancode.load" = load i64, ptr %"%scancode", align 8
  %cmpnetmp = icmp ne i64 %"%scancode.load", 0
  br i1 %cmpnetmp, label %then_318, label %merge_319

while_cond_315:                                   ; preds = %merge_319, %entry
  %"%k.load" = load i64, ptr %"%k", align 8
  %cmplttmp = icmp slt i64 %"%k.load", 8
  br i1 %cmplttmp, label %while_body_316, label %while_exit_317

while_exit_317:                                   ; preds = %while_cond_315
  %"%kbd_prev_report.load" = load i64, ptr @kbd_prev_report, align 8
  %"%buf.load4" = load i64, ptr %buf1, align 8
  call void @sad_ll_memcpy(i64 %"%kbd_prev_report.load", i64 %"%buf.load4", i64 8)
  %"%buf.load5" = load i64, ptr %buf1, align 8
  %addtmp6 = add i64 %"%buf.load5", 2
  call void @sad_ll_mem_read8(i64 %addtmp6)
  store i64 0, ptr %"%kbd_pressed_0", align 8
  %"%buf.load7" = load i64, ptr %buf1, align 8
  %addtmp8 = add i64 %"%buf.load7", 3
  call void @sad_ll_mem_read8(i64 %addtmp8)
  store i64 0, ptr %"%kbd_pressed_1", align 8
  %"%buf.load9" = load i64, ptr %buf1, align 8
  %addtmp10 = add i64 %"%buf.load9", 4
  call void @sad_ll_mem_read8(i64 %addtmp10)
  store i64 0, ptr %"%kbd_pressed_2", align 8
  %"%buf.load11" = load i64, ptr %buf1, align 8
  %addtmp12 = add i64 %"%buf.load11", 5
  call void @sad_ll_mem_read8(i64 %addtmp12)
  store i64 0, ptr %"%kbd_pressed_3", align 8
  %"%buf.load13" = load i64, ptr %buf1, align 8
  %addtmp14 = add i64 %"%buf.load13", 6
  call void @sad_ll_mem_read8(i64 %addtmp14)
  store i64 0, ptr %"%kbd_pressed_4", align 8
  %"%buf.load15" = load i64, ptr %buf1, align 8
  %addtmp16 = add i64 %"%buf.load15", 7
  call void @sad_ll_mem_read8(i64 %addtmp16)
  store i64 0, ptr %"%kbd_pressed_5", align 8
  ret i64 1
}

declare void @sad_ll_memcpy(i64, i64, i64)

define i64 @hid_mouse_push_event(i64 %buttons, i64 %dx, i64 %dy, i64 %scroll) {
entry:
  %"%hid_mouse_events" = alloca i64, align 8
  %"%mouse_event_tail" = alloca i64, align 8
  %"%next_tail" = alloca i64, align 8
  %scroll4 = alloca i64, align 8
  store i64 %scroll, ptr %scroll4, align 8
  %dy3 = alloca i64, align 8
  store i64 %dy, ptr %dy3, align 8
  %dx2 = alloca i64, align 8
  store i64 %dx, ptr %dx2, align 8
  %buttons1 = alloca i64, align 8
  store i64 %buttons, ptr %buttons1, align 8
  %"%mouse_event_tail.load" = load i64, ptr @mouse_event_tail, align 8
  %addtmp = add i64 %"%mouse_event_tail.load", 4
  %"%MOUSE_BUF_SIZE.load" = load i64, ptr @MOUSE_BUF_SIZE, align 8
  %shltmp = shl i64 %"%MOUSE_BUF_SIZE.load", 2
  %modtmp = srem i64 %addtmp, %shltmp
  store i64 %modtmp, ptr %"%next_tail", align 8
  %"%next_tail.load" = load i64, ptr %"%next_tail", align 8
  %"%mouse_event_head.load" = load i64, ptr @mouse_event_head, align 8
  %cmpeqtmp = icmp eq i64 %"%next_tail.load", %"%mouse_event_head.load"
  br i1 %cmpeqtmp, label %then_341, label %merge_342

merge_342:                                        ; preds = %entry
  %"%mouse_event_buf.load" = load i64, ptr @mouse_event_buf, align 8
  %"%mouse_event_tail.load5" = load i64, ptr @mouse_event_tail, align 8
  %addtmp6 = add i64 %"%mouse_event_buf.load", %"%mouse_event_tail.load5"
  %"%947.load" = load i64, ptr %buttons1, align 8
  call void @sad_ll_mem_write8(i64 %addtmp6, i64 %"%947.load")
  %addtmp7 = add i64 %addtmp6, 1
  %"%dx.load" = load i64, ptr %dx2, align 8
  %addtmp8 = add i64 %"%dx.load", 128
  call void @sad_ll_mem_write8(i64 %addtmp7, i64 %addtmp8)
  %addtmp9 = add i64 %addtmp6, 2
  %"%dy.load" = load i64, ptr %dy3, align 8
  %addtmp10 = add i64 %"%dy.load", 128
  call void @sad_ll_mem_write8(i64 %addtmp9, i64 %addtmp10)
  %addtmp11 = add i64 %addtmp6, 3
  %"%scroll.load" = load i64, ptr %scroll4, align 8
  %addtmp12 = add i64 %"%scroll.load", 128
  call void @sad_ll_mem_write8(i64 %addtmp11, i64 %addtmp12)
  %"%968.load" = load i64, ptr %"%next_tail", align 8
  store i64 %"%968.load", ptr %"%mouse_event_tail", align 8
  %"%hid_mouse_events.load" = load i64, ptr @hid_mouse_events, align 8
  %addtmp13 = add i64 %"%hid_mouse_events.load", 1
  store i64 %addtmp13, ptr %"%hid_mouse_events", align 8
  ret i64 1

then_341:                                         ; preds = %entry
  ret i64 0
}

define i64 @hid_mouse_pop_event() {
entry:
  %"%mouse_event_head" = alloca i64, align 8
  %"%buttons" = alloca i64, align 8
  %"%mouse_event_head.load" = load i64, ptr @mouse_event_head, align 8
  %"%mouse_event_tail.load" = load i64, ptr @mouse_event_tail, align 8
  %cmpeqtmp = icmp eq i64 %"%mouse_event_head.load", %"%mouse_event_tail.load"
  br i1 %cmpeqtmp, label %then_343, label %merge_344

merge_344:                                        ; preds = %entry
  %"%mouse_event_buf.load" = load i64, ptr @mouse_event_buf, align 8
  %"%mouse_event_head.load1" = load i64, ptr @mouse_event_head, align 8
  %addtmp = add i64 %"%mouse_event_buf.load", %"%mouse_event_head.load1"
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%buttons", align 8
  %"%mouse_event_head.load2" = load i64, ptr @mouse_event_head, align 8
  %addtmp3 = add i64 %"%mouse_event_head.load2", 4
  %"%MOUSE_BUF_SIZE.load" = load i64, ptr @MOUSE_BUF_SIZE, align 8
  %shltmp = shl i64 %"%MOUSE_BUF_SIZE.load", 2
  %modtmp = srem i64 %addtmp3, %shltmp
  store i64 %modtmp, ptr %"%mouse_event_head", align 8
  %loadtmp = load i64, ptr %"%buttons", align 8
  ret i64 %loadtmp

then_343:                                         ; preds = %entry
  ret i64 0
}

define i64 @hid_process_mouse_report(i64 %buf, i64 %len) {
entry:
  %"%mouse_y" = alloca i64, align 8
  %"%mouse_x" = alloca i64, align 8
  %"%mouse_scroll" = alloca i64, align 8
  %"%mouse_dy" = alloca i64, align 8
  %"%mouse_dx" = alloca i64, align 8
  %"%mouse_buttons" = alloca i64, align 8
  %"%scroll" = alloca i64, align 8
  %"%dy" = alloca i64, align 8
  %"%dx" = alloca i64, align 8
  %"%raw_scroll" = alloca i64, align 8
  %"%raw_dy" = alloca i64, align 8
  %"%raw_dx" = alloca i64, align 8
  %"%buttons" = alloca i64, align 8
  %len2 = alloca i64, align 8
  store i64 %len, ptr %len2, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  call void @sad_ll_mem_read8(i64 %"%buf.load")
  store i64 0, ptr %"%buttons", align 8
  %"%buf.load3" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load3", 1
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%raw_dx", align 8
  %"%buf.load4" = load i64, ptr %buf1, align 8
  %addtmp5 = add i64 %"%buf.load4", 2
  call void @sad_ll_mem_read8(i64 %addtmp5)
  store i64 0, ptr %"%raw_dy", align 8
  store i64 0, ptr %"%raw_scroll", align 8
  %"%len.load" = load i64, ptr %len2, align 8
  %cmpgetmp = icmp sge i64 %"%len.load", 4
  br i1 %cmpgetmp, label %then_345, label %merge_346

else_363:                                         ; preds = %merge_360
  %"%dy.load30" = load i64, ptr %"%dy", align 8
  %cmpnetmp31 = icmp ne i64 %"%dy.load30", 0
  br i1 %cmpnetmp31, label %then_364, label %else_366

else_366:                                         ; preds = %else_363
  %"%buttons.load37" = load i64, ptr %"%buttons", align 8
  %cmpnetmp38 = icmp ne i64 %"%buttons.load37", 0
  br i1 %cmpnetmp38, label %then_367, label %merge_368

merge_346:                                        ; preds = %then_345, %entry
  %"%1010.load" = load i64, ptr %"%raw_dx", align 8
  store i64 %"%1010.load", ptr %"%dx", align 8
  %"%dx.load" = load i64, ptr %"%dx", align 8
  %cmpgttmp = icmp sgt i64 %"%dx.load", 127
  br i1 %cmpgttmp, label %then_347, label %merge_348

merge_348:                                        ; preds = %then_347, %merge_346
  %"%1016.load" = load i64, ptr %"%raw_dy", align 8
  store i64 %"%1016.load", ptr %"%dy", align 8
  %"%dy.load" = load i64, ptr %"%dy", align 8
  %cmpgttmp9 = icmp sgt i64 %"%dy.load", 127
  br i1 %cmpgttmp9, label %then_349, label %merge_350

merge_350:                                        ; preds = %then_349, %merge_348
  %"%1022.load" = load i64, ptr %"%raw_scroll", align 8
  store i64 %"%1022.load", ptr %"%scroll", align 8
  %"%scroll.load" = load i64, ptr %"%scroll", align 8
  %cmpgttmp12 = icmp sgt i64 %"%scroll.load", 127
  br i1 %cmpgttmp12, label %then_351, label %merge_352

merge_352:                                        ; preds = %then_351, %merge_350
  %"%1028.load" = load i64, ptr %"%buttons", align 8
  store i64 %"%1028.load", ptr %"%mouse_buttons", align 8
  %"%1030.load" = load i64, ptr %"%dx", align 8
  store i64 %"%1030.load", ptr %"%mouse_dx", align 8
  %"%1032.load" = load i64, ptr %"%dy", align 8
  store i64 %"%1032.load", ptr %"%mouse_dy", align 8
  %"%1034.load" = load i64, ptr %"%scroll", align 8
  store i64 %"%1034.load", ptr %"%mouse_scroll", align 8
  %"%mouse_x.load" = load i64, ptr @mouse_x, align 8
  %"%dx.load15" = load i64, ptr %"%dx", align 8
  %addtmp16 = add i64 %"%mouse_x.load", %"%dx.load15"
  store i64 %addtmp16, ptr %"%mouse_x", align 8
  %"%mouse_y.load" = load i64, ptr @mouse_y, align 8
  %"%dy.load17" = load i64, ptr %"%dy", align 8
  %addtmp18 = add i64 %"%mouse_y.load", %"%dy.load17"
  store i64 %addtmp18, ptr %"%mouse_y", align 8
  %"%mouse_x.load19" = load i64, ptr %"%mouse_x", align 8
  %cmplttmp = icmp slt i64 %"%mouse_x.load19", 0
  br i1 %cmplttmp, label %then_353, label %merge_354

merge_354:                                        ; preds = %then_353, %merge_352
  %"%mouse_y.load20" = load i64, ptr %"%mouse_y", align 8
  %cmplttmp21 = icmp slt i64 %"%mouse_y.load20", 0
  br i1 %cmplttmp21, label %then_355, label %merge_356

merge_356:                                        ; preds = %then_355, %merge_354
  %"%mouse_x.load22" = load i64, ptr %"%mouse_x", align 8
  %"%mouse_max_x.load" = load i64, ptr @mouse_max_x, align 8
  %cmpgttmp23 = icmp sgt i64 %"%mouse_x.load22", %"%mouse_max_x.load"
  br i1 %cmpgttmp23, label %then_357, label %merge_358

merge_358:                                        ; preds = %then_357, %merge_356
  %"%mouse_y.load24" = load i64, ptr %"%mouse_y", align 8
  %"%mouse_max_y.load" = load i64, ptr @mouse_max_y, align 8
  %cmpgttmp25 = icmp sgt i64 %"%mouse_y.load24", %"%mouse_max_y.load"
  br i1 %cmpgttmp25, label %then_359, label %merge_360

merge_360:                                        ; preds = %then_359, %merge_358
  %"%dx.load26" = load i64, ptr %"%dx", align 8
  %cmpnetmp = icmp ne i64 %"%dx.load26", 0
  br i1 %cmpnetmp, label %then_361, label %else_363

merge_362:                                        ; preds = %merge_365, %then_361
  ret i64 1

merge_365:                                        ; preds = %merge_368, %then_364
  br label %merge_362

merge_368:                                        ; preds = %then_367, %else_366
  br label %merge_365

then_345:                                         ; preds = %entry
  %"%buf.load6" = load i64, ptr %buf1, align 8
  %addtmp7 = add i64 %"%buf.load6", 3
  call void @sad_ll_mem_read8(i64 %addtmp7)
  store i64 0, ptr %"%raw_scroll", align 8
  br label %merge_346

then_347:                                         ; preds = %merge_346
  %"%dx.load8" = load i64, ptr %"%dx", align 8
  %subtmp = sub i64 %"%dx.load8", 256
  store i64 %subtmp, ptr %"%dx", align 8
  br label %merge_348

then_349:                                         ; preds = %merge_348
  %"%dy.load10" = load i64, ptr %"%dy", align 8
  %subtmp11 = sub i64 %"%dy.load10", 256
  store i64 %subtmp11, ptr %"%dy", align 8
  br label %merge_350

then_351:                                         ; preds = %merge_350
  %"%scroll.load13" = load i64, ptr %"%scroll", align 8
  %subtmp14 = sub i64 %"%scroll.load13", 256
  store i64 %subtmp14, ptr %"%scroll", align 8
  br label %merge_352

then_353:                                         ; preds = %merge_352
  store i64 0, ptr %"%mouse_x", align 8
  br label %merge_354

then_355:                                         ; preds = %merge_354
  store i64 0, ptr %"%mouse_y", align 8
  br label %merge_356

then_357:                                         ; preds = %merge_356
  %"%1045.load" = load i64, ptr @mouse_max_x, align 8
  store i64 %"%1045.load", ptr %"%mouse_x", align 8
  br label %merge_358

then_359:                                         ; preds = %merge_358
  %"%1048.load" = load i64, ptr @mouse_max_y, align 8
  store i64 %"%1048.load", ptr %"%mouse_y", align 8
  br label %merge_360

then_361:                                         ; preds = %merge_360
  %"%buttons.load" = load i64, ptr %"%buttons", align 8
  %"%dx.load27" = load i64, ptr %"%dx", align 8
  %"%dy.load28" = load i64, ptr %"%dy", align 8
  %"%scroll.load29" = load i64, ptr %"%scroll", align 8
  %calltmp = call i64 @hid_mouse_push_event(i64 %"%buttons.load", i64 %"%dx.load27", i64 %"%dy.load28", i64 %"%scroll.load29")
  br label %merge_362

then_364:                                         ; preds = %else_363
  %"%buttons.load32" = load i64, ptr %"%buttons", align 8
  %"%dx.load33" = load i64, ptr %"%dx", align 8
  %"%dy.load34" = load i64, ptr %"%dy", align 8
  %"%scroll.load35" = load i64, ptr %"%scroll", align 8
  %calltmp36 = call i64 @hid_mouse_push_event(i64 %"%buttons.load32", i64 %"%dx.load33", i64 %"%dy.load34", i64 %"%scroll.load35")
  br label %merge_365

then_367:                                         ; preds = %else_366
  %"%buttons.load39" = load i64, ptr %"%buttons", align 8
  %"%scroll.load40" = load i64, ptr %"%scroll", align 8
  %calltmp41 = call i64 @hid_mouse_push_event(i64 %"%buttons.load39", i64 0, i64 0, i64 %"%scroll.load40")
  br label %merge_368
}

define i64 @hid_mouse_get_x() {
entry:
  %loadtmp = load i64, ptr @mouse_x, align 8
  ret i64 %loadtmp
}

define i64 @hid_mouse_get_y() {
entry:
  %loadtmp = load i64, ptr @mouse_y, align 8
  ret i64 %loadtmp
}

define i64 @hid_mouse_get_buttons() {
entry:
  %loadtmp = load i64, ptr @mouse_buttons, align 8
  ret i64 %loadtmp
}

define i64 @hid_mouse_left_pressed() {
entry:
  %"%mouse_buttons.load" = load i64, ptr @mouse_buttons, align 8
  %andtmp = and i64 %"%mouse_buttons.load", 1
  ret i64 %andtmp
}

define i64 @hid_mouse_right_pressed() {
entry:
  %"%mouse_buttons.load" = load i64, ptr @mouse_buttons, align 8
  %shrtmp = lshr i64 %"%mouse_buttons.load", 1
  %andtmp = and i64 %shrtmp, 1
  ret i64 %andtmp
}

define i64 @hid_mouse_middle_pressed() {
entry:
  %"%mouse_buttons.load" = load i64, ptr @mouse_buttons, align 8
  %shrtmp = lshr i64 %"%mouse_buttons.load", 2
  %andtmp = and i64 %shrtmp, 1
  ret i64 %andtmp
}

define i64 @hid_mouse_set_bounds(i64 %max_x, i64 %max_y) {
entry:
  %"%mouse_max_y" = alloca i64, align 8
  %"%mouse_max_x" = alloca i64, align 8
  %max_y2 = alloca i64, align 8
  store i64 %max_y, ptr %max_y2, align 8
  %max_x1 = alloca i64, align 8
  store i64 %max_x, ptr %max_x1, align 8
  %"%1084.load" = load i64, ptr %max_x1, align 8
  store i64 %"%1084.load", ptr %"%mouse_max_x", align 8
  %"%1086.load" = load i64, ptr %max_y2, align 8
  store i64 %"%1086.load", ptr %"%mouse_max_y", align 8
  ret i64 0
}

define i64 @hid_poll_keyboard() {
entry:
  %"%hid_errors" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%hid_poll_count" = alloca i64, align 8
  %"%kbd_active.load" = load i64, ptr @kbd_active, align 8
  %cmpeqtmp = icmp eq i64 %"%kbd_active.load", 0
  br i1 %cmpeqtmp, label %then_369, label %merge_370

else_373:                                         ; preds = %merge_370
  %"%result.load2" = load i64, ptr %"%result", align 8
  %cmplttmp = icmp slt i64 %"%result.load2", 0
  br i1 %cmplttmp, label %then_374, label %merge_375

merge_370:                                        ; preds = %entry
  %"%hid_poll_count.load" = load i64, ptr @hid_poll_count, align 8
  %addtmp = add i64 %"%hid_poll_count.load", 1
  store i64 %addtmp, ptr %"%hid_poll_count", align 8
  %"%kbd_hc_idx.load" = load i64, ptr @kbd_hc_idx, align 8
  %"%kbd_dev_addr.load" = load i64, ptr @kbd_dev_addr, align 8
  %"%kbd_endpoint.load" = load i64, ptr @kbd_endpoint, align 8
  %"%kbd_report_buf.load" = load i64, ptr @kbd_report_buf, align 8
  %"%KBD_REPORT_SIZE.load" = load i64, ptr @KBD_REPORT_SIZE, align 8
  call void @uhci_interrupt_transfer(i64 %"%kbd_hc_idx.load", i64 %"%kbd_dev_addr.load", i64 %"%kbd_endpoint.load", i64 %"%kbd_report_buf.load", i64 %"%KBD_REPORT_SIZE.load")
  store i64 0, ptr %"%result", align 8
  %"%result.load" = load i64, ptr %"%result", align 8
  %cmpgttmp = icmp sgt i64 %"%result.load", 0
  br i1 %cmpgttmp, label %then_371, label %else_373

merge_372:                                        ; preds = %merge_375
  ret i64 0

merge_375:                                        ; preds = %then_374, %else_373
  br label %merge_372

then_369:                                         ; preds = %entry
  ret i64 0

then_371:                                         ; preds = %merge_370
  %"%kbd_report_buf.load1" = load i64, ptr @kbd_report_buf, align 8
  %calltmp = call i64 @hid_process_kbd_report(i64 %"%kbd_report_buf.load1")
  ret i64 1

then_374:                                         ; preds = %else_373
  %"%hid_errors.load" = load i64, ptr @hid_errors, align 8
  %addtmp3 = add i64 %"%hid_errors.load", 1
  store i64 %addtmp3, ptr %"%hid_errors", align 8
  br label %merge_375
}

define i64 @hid_poll_mouse() {
entry:
  %"%hid_errors" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%hid_poll_count" = alloca i64, align 8
  %"%mouse_active.load" = load i64, ptr @mouse_active, align 8
  %cmpeqtmp = icmp eq i64 %"%mouse_active.load", 0
  br i1 %cmpeqtmp, label %then_376, label %merge_377

else_380:                                         ; preds = %merge_377
  %"%result.load3" = load i64, ptr %"%result", align 8
  %cmplttmp = icmp slt i64 %"%result.load3", 0
  br i1 %cmplttmp, label %then_381, label %merge_382

merge_377:                                        ; preds = %entry
  %"%hid_poll_count.load" = load i64, ptr @hid_poll_count, align 8
  %addtmp = add i64 %"%hid_poll_count.load", 1
  store i64 %addtmp, ptr %"%hid_poll_count", align 8
  %"%mouse_hc_idx.load" = load i64, ptr @mouse_hc_idx, align 8
  %"%mouse_dev_addr.load" = load i64, ptr @mouse_dev_addr, align 8
  %"%mouse_endpoint.load" = load i64, ptr @mouse_endpoint, align 8
  %"%mouse_report_buf.load" = load i64, ptr @mouse_report_buf, align 8
  %"%MOUSE_REPORT_SIZE.load" = load i64, ptr @MOUSE_REPORT_SIZE, align 8
  call void @uhci_interrupt_transfer(i64 %"%mouse_hc_idx.load", i64 %"%mouse_dev_addr.load", i64 %"%mouse_endpoint.load", i64 %"%mouse_report_buf.load", i64 %"%MOUSE_REPORT_SIZE.load")
  store i64 0, ptr %"%result", align 8
  %"%result.load" = load i64, ptr %"%result", align 8
  %cmpgttmp = icmp sgt i64 %"%result.load", 0
  br i1 %cmpgttmp, label %then_378, label %else_380

merge_379:                                        ; preds = %merge_382
  ret i64 0

merge_382:                                        ; preds = %then_381, %else_380
  br label %merge_379

then_376:                                         ; preds = %entry
  ret i64 0

then_378:                                         ; preds = %merge_377
  %"%mouse_report_buf.load1" = load i64, ptr @mouse_report_buf, align 8
  %"%result.load2" = load i64, ptr %"%result", align 8
  %calltmp = call i64 @hid_process_mouse_report(i64 %"%mouse_report_buf.load1", i64 %"%result.load2")
  ret i64 1

then_381:                                         ; preds = %else_380
  %"%hid_errors.load" = load i64, ptr @hid_errors, align 8
  %addtmp4 = add i64 %"%hid_errors.load", 1
  store i64 %addtmp4, ptr %"%hid_errors", align 8
  br label %merge_382
}

define i64 @"hid_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %calltmp = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %calltmp1 = call i64 @"hid_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %"%kbd_active.load" = load i64, ptr @kbd_active, align 8
  %cmpeqtmp = icmp eq i64 %"%kbd_active.load", 1
  br i1 %cmpeqtmp, label %then_383, label %else_385

else_385:                                         ; preds = %entry
  %calltmp5 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  br label %merge_384

else_388:                                         ; preds = %merge_384
  %calltmp11 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  br label %merge_387

merge_384:                                        ; preds = %else_385, %then_383
  %calltmp6 = call i64 @"hid_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp7 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%mouse_active.load" = load i64, ptr @mouse_active, align 8
  %cmpeqtmp8 = icmp eq i64 %"%mouse_active.load", 1
  br i1 %cmpeqtmp8, label %then_386, label %else_388

merge_387:                                        ; preds = %else_388, %then_386
  %calltmp12 = call i64 @"hid_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp13 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %"%1152.load" = load i64, ptr @hid_kbd_events, align 8
  %calltmp14 = call i64 @"hid_serial_\D8\B1\D9\82\D9\85"(i64 %"%1152.load")
  %calltmp15 = call i64 @"hid_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp16 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %"%1158.load" = load i64, ptr @hid_mouse_events, align 8
  %calltmp17 = call i64 @"hid_serial_\D8\B1\D9\82\D9\85"(i64 %"%1158.load")
  %calltmp18 = call i64 @"hid_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp19 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  %"%1164.load" = load i64, ptr @hid_poll_count, align 8
  %calltmp20 = call i64 @"hid_serial_\D8\B1\D9\82\D9\85"(i64 %"%1164.load")
  %calltmp21 = call i64 @"hid_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp22 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %"%1170.load" = load i64, ptr @hid_errors, align 8
  %calltmp23 = call i64 @"hid_serial_\D8\B1\D9\82\D9\85"(i64 %"%1170.load")
  %calltmp24 = call i64 @"hid_serial_\D8\B3\D8\B7\D8\B1"()
  %"%mouse_active.load25" = load i64, ptr @mouse_active, align 8
  %cmpeqtmp26 = icmp eq i64 %"%mouse_active.load25", 1
  br i1 %cmpeqtmp26, label %then_389, label %merge_390

merge_390:                                        ; preds = %then_389, %merge_387
  %calltmp32 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %calltmp33 = call i64 @"hid_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_383:                                         ; preds = %entry
  %calltmp3 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %"%1132.load" = load i64, ptr @kbd_dev_addr, align 8
  %calltmp4 = call i64 @"hid_serial_\D8\B1\D9\82\D9\85"(i64 %"%1132.load")
  br label %merge_384

then_386:                                         ; preds = %merge_384
  %calltmp9 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %"%1144.load" = load i64, ptr @mouse_dev_addr, align 8
  %calltmp10 = call i64 @"hid_serial_\D8\B1\D9\82\D9\85"(i64 %"%1144.load")
  br label %merge_387

then_389:                                         ; preds = %merge_387
  %calltmp27 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %"%1178.load" = load i64, ptr @mouse_x, align 8
  %calltmp28 = call i64 @"hid_serial_\D8\B1\D9\82\D9\85"(i64 %"%1178.load")
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 44)
  %"%1183.load" = load i64, ptr @mouse_y, align 8
  %calltmp29 = call i64 @"hid_serial_\D8\B1\D9\82\D9\85"(i64 %"%1183.load")
  %calltmp30 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %calltmp31 = call i64 @"hid_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_390
}

define i64 @"hid_\D9\87\D9\8A\D8\A6"() {
entry:
  %"%mouse_dev_addr" = alloca i64, align 8
  %"%mouse_active" = alloca i64, align 8
  %"%mouse_dev_idx" = alloca i64, align 8
  %"%ms_idx" = alloca i64, align 8
  %"%kbd_dev_addr" = alloca i64, align 8
  %"%kbd_active" = alloca i64, align 8
  %"%kbd_dev_idx" = alloca i64, align 8
  %"%kb_idx" = alloca i64, align 8
  %"%mouse_y" = alloca i64, align 8
  %"%mouse_x" = alloca i64, align 8
  %"%kbd_capslock" = alloca i64, align 8
  %"%kbd_modifiers" = alloca i64, align 8
  %"%mouse_event_tail" = alloca i64, align 8
  %"%mouse_event_head" = alloca i64, align 8
  %"%kbd_event_tail" = alloca i64, align 8
  %"%kbd_event_head" = alloca i64, align 8
  %"%mouse_event_buf" = alloca i64, align 8
  %"%mouse_report_buf" = alloca i64, align 8
  %"%kbd_event_buf" = alloca i64, align 8
  %"%kbd_prev_report" = alloca i64, align 8
  %"%kbd_report_buf" = alloca i64, align 8
  %calltmp = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  %calltmp1 = call i64 @"hid_serial_\D8\B3\D8\B7\D8\B1"()
  call void @sad_ll_kmalloc(i64 16)
  store i64 0, ptr %"%kbd_report_buf", align 8
  call void @sad_ll_kmalloc(i64 16)
  store i64 0, ptr %"%kbd_prev_report", align 8
  %"%KBD_BUF_SIZE.load" = load i64, ptr @KBD_BUF_SIZE, align 8
  call void @sad_ll_kmalloc(i64 %"%KBD_BUF_SIZE.load")
  store i64 0, ptr %"%kbd_event_buf", align 8
  call void @sad_ll_kmalloc(i64 16)
  store i64 0, ptr %"%mouse_report_buf", align 8
  %"%MOUSE_BUF_SIZE.load" = load i64, ptr @MOUSE_BUF_SIZE, align 8
  %shltmp = shl i64 %"%MOUSE_BUF_SIZE.load", 2
  call void @sad_ll_kmalloc(i64 %shltmp)
  store i64 0, ptr %"%mouse_event_buf", align 8
  %"%kbd_report_buf.load" = load i64, ptr %"%kbd_report_buf", align 8
  call void @sad_ll_memset(i64 %"%kbd_report_buf.load", i64 0, i64 16)
  %"%kbd_prev_report.load" = load i64, ptr %"%kbd_prev_report", align 8
  call void @sad_ll_memset(i64 %"%kbd_prev_report.load", i64 0, i64 16)
  %"%kbd_event_buf.load" = load i64, ptr %"%kbd_event_buf", align 8
  %"%KBD_BUF_SIZE.load2" = load i64, ptr @KBD_BUF_SIZE, align 8
  call void @sad_ll_memset(i64 %"%kbd_event_buf.load", i64 0, i64 %"%KBD_BUF_SIZE.load2")
  %"%mouse_report_buf.load" = load i64, ptr %"%mouse_report_buf", align 8
  call void @sad_ll_memset(i64 %"%mouse_report_buf.load", i64 0, i64 16)
  %"%mouse_event_buf.load" = load i64, ptr %"%mouse_event_buf", align 8
  call void @sad_ll_memset(i64 %"%mouse_event_buf.load", i64 0, i64 %shltmp)
  store i64 0, ptr %"%kbd_event_head", align 8
  store i64 0, ptr %"%kbd_event_tail", align 8
  store i64 0, ptr %"%mouse_event_head", align 8
  store i64 0, ptr %"%mouse_event_tail", align 8
  store i64 0, ptr %"%kbd_modifiers", align 8
  store i64 0, ptr %"%kbd_capslock", align 8
  %"%mouse_max_x.load" = load i64, ptr @mouse_max_x, align 8
  %shrtmp = lshr i64 %"%mouse_max_x.load", 1
  store i64 %shrtmp, ptr %"%mouse_x", align 8
  %"%mouse_max_y.load" = load i64, ptr @mouse_max_y, align 8
  %shrtmp3 = lshr i64 %"%mouse_max_y.load", 1
  store i64 %shrtmp3, ptr %"%mouse_y", align 8
  call void @usb_find_keyboard()
  store i64 0, ptr %"%kb_idx", align 8
  %"%kb_idx.load" = load i64, ptr %"%kb_idx", align 8
  %cmpgetmp = icmp sge i64 %"%kb_idx.load", 0
  br i1 %cmpgetmp, label %then_391, label %else_393

else_393:                                         ; preds = %entry
  %calltmp9 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  %calltmp10 = call i64 @"hid_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_392

else_396:                                         ; preds = %merge_392
  %calltmp18 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %calltmp19 = call i64 @"hid_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_395

merge_392:                                        ; preds = %else_393, %then_391
  call void @usb_find_mouse()
  store i64 0, ptr %"%ms_idx", align 8
  %"%ms_idx.load" = load i64, ptr %"%ms_idx", align 8
  %cmpgetmp11 = icmp sge i64 %"%ms_idx.load", 0
  br i1 %cmpgetmp11, label %then_394, label %else_396

merge_395:                                        ; preds = %else_396, %then_394
  %calltmp20 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  %calltmp21 = call i64 @"hid_serial_\D8\B3\D8\B7\D8\B1"()
  %"%kbd_active.load" = load i64, ptr %"%kbd_active", align 8
  %"%mouse_active.load" = load i64, ptr %"%mouse_active", align 8
  %addtmp22 = add i64 %"%kbd_active.load", %"%mouse_active.load"
  ret i64 %addtmp22

then_391:                                         ; preds = %entry
  %"%1246.load" = load i64, ptr %"%kb_idx", align 8
  store i64 %"%1246.load", ptr %"%kbd_dev_idx", align 8
  store i64 1, ptr %"%kbd_active", align 8
  %"%kb_idx.load4" = load i64, ptr %"%kb_idx", align 8
  %addtmp = add i64 %"%kb_idx.load4", 1
  store i64 %addtmp, ptr %"%kbd_dev_addr", align 8
  %calltmp5 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%1246.load6" = load i64, ptr %"%kb_idx", align 8
  %calltmp7 = call i64 @"hid_serial_\D8\B1\D9\82\D9\85"(i64 %"%1246.load6")
  %calltmp8 = call i64 @"hid_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_392

then_394:                                         ; preds = %merge_392
  %"%1265.load" = load i64, ptr %"%ms_idx", align 8
  store i64 %"%1265.load", ptr %"%mouse_dev_idx", align 8
  store i64 1, ptr %"%mouse_active", align 8
  %"%ms_idx.load12" = load i64, ptr %"%ms_idx", align 8
  %addtmp13 = add i64 %"%ms_idx.load12", 1
  store i64 %addtmp13, ptr %"%mouse_dev_addr", align 8
  %calltmp14 = call i64 @"hid_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %"%1265.load15" = load i64, ptr %"%ms_idx", align 8
  %calltmp16 = call i64 @"hid_serial_\D8\B1\D9\82\D9\85"(i64 %"%1265.load15")
  %calltmp17 = call i64 @"hid_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_395
}

define i64 @"hid_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  %calltmp = call i64 @hid_poll_keyboard()
  %calltmp1 = call i64 @hid_poll_mouse()
  ret i64 0
}

define void @__sad_main() {
entry:
  %"%hid_errors" = alloca i64, align 8
  %"%hid_poll_count" = alloca i64, align 8
  %"%hid_mouse_events" = alloca i64, align 8
  %"%hid_kbd_events" = alloca i64, align 8
  %"%mouse_max_y" = alloca i64, align 8
  %"%mouse_max_x" = alloca i64, align 8
  %"%mouse_event_tail" = alloca i64, align 8
  %"%mouse_event_head" = alloca i64, align 8
  %"%mouse_event_buf" = alloca i64, align 8
  %"%mouse_scroll" = alloca i64, align 8
  %"%mouse_dy" = alloca i64, align 8
  %"%mouse_dx" = alloca i64, align 8
  %"%mouse_buttons" = alloca i64, align 8
  %"%mouse_y" = alloca i64, align 8
  %"%mouse_x" = alloca i64, align 8
  %"%mouse_active" = alloca i64, align 8
  %"%mouse_report_buf" = alloca i64, align 8
  %"%mouse_endpoint" = alloca i64, align 8
  %"%mouse_dev_addr" = alloca i64, align 8
  %"%mouse_hc_idx" = alloca i64, align 8
  %"%mouse_dev_idx" = alloca i64, align 8
  %"%kbd_pressed_5" = alloca i64, align 8
  %"%kbd_pressed_4" = alloca i64, align 8
  %"%kbd_pressed_3" = alloca i64, align 8
  %"%kbd_pressed_2" = alloca i64, align 8
  %"%kbd_pressed_1" = alloca i64, align 8
  %"%kbd_pressed_0" = alloca i64, align 8
  %"%kbd_scrolllock" = alloca i64, align 8
  %"%kbd_numlock" = alloca i64, align 8
  %"%kbd_capslock" = alloca i64, align 8
  %"%kbd_modifiers" = alloca i64, align 8
  %"%kbd_event_tail" = alloca i64, align 8
  %"%kbd_event_head" = alloca i64, align 8
  %"%kbd_event_buf" = alloca i64, align 8
  %"%kbd_active" = alloca i64, align 8
  %"%kbd_prev_report" = alloca i64, align 8
  %"%kbd_report_buf" = alloca i64, align 8
  %"%kbd_endpoint" = alloca i64, align 8
  %"%kbd_dev_addr" = alloca i64, align 8
  %"%kbd_hc_idx" = alloca i64, align 8
  %"%kbd_dev_idx" = alloca i64, align 8
  %"%sc_shift_63" = alloca i64, align 8
  %"%sc_shift_62" = alloca i64, align 8
  %"%sc_shift_61" = alloca i64, align 8
  %"%sc_shift_60" = alloca i64, align 8
  %"%sc_shift_59" = alloca i64, align 8
  %"%sc_shift_58" = alloca i64, align 8
  %"%sc_shift_57" = alloca i64, align 8
  %"%sc_shift_56" = alloca i64, align 8
  %"%sc_shift_55" = alloca i64, align 8
  %"%sc_shift_54" = alloca i64, align 8
  %"%sc_shift_53" = alloca i64, align 8
  %"%sc_shift_52" = alloca i64, align 8
  %"%sc_shift_51" = alloca i64, align 8
  %"%sc_shift_50" = alloca i64, align 8
  %"%sc_shift_49" = alloca i64, align 8
  %"%sc_shift_48" = alloca i64, align 8
  %"%sc_shift_47" = alloca i64, align 8
  %"%sc_shift_46" = alloca i64, align 8
  %"%sc_shift_45" = alloca i64, align 8
  %"%sc_shift_44" = alloca i64, align 8
  %"%sc_shift_43" = alloca i64, align 8
  %"%sc_shift_42" = alloca i64, align 8
  %"%sc_shift_41" = alloca i64, align 8
  %"%sc_shift_40" = alloca i64, align 8
  %"%sc_shift_39" = alloca i64, align 8
  %"%sc_shift_38" = alloca i64, align 8
  %"%sc_shift_37" = alloca i64, align 8
  %"%sc_shift_36" = alloca i64, align 8
  %"%sc_shift_35" = alloca i64, align 8
  %"%sc_shift_34" = alloca i64, align 8
  %"%sc_shift_33" = alloca i64, align 8
  %"%sc_shift_32" = alloca i64, align 8
  %"%sc_shift_31" = alloca i64, align 8
  %"%sc_shift_30" = alloca i64, align 8
  %"%sc_shift_29" = alloca i64, align 8
  %"%sc_shift_28" = alloca i64, align 8
  %"%sc_shift_27" = alloca i64, align 8
  %"%sc_shift_26" = alloca i64, align 8
  %"%sc_shift_25" = alloca i64, align 8
  %"%sc_shift_24" = alloca i64, align 8
  %"%sc_shift_23" = alloca i64, align 8
  %"%sc_shift_22" = alloca i64, align 8
  %"%sc_shift_21" = alloca i64, align 8
  %"%sc_shift_20" = alloca i64, align 8
  %"%sc_shift_19" = alloca i64, align 8
  %"%sc_shift_18" = alloca i64, align 8
  %"%sc_shift_17" = alloca i64, align 8
  %"%sc_shift_16" = alloca i64, align 8
  %"%sc_shift_15" = alloca i64, align 8
  %"%sc_shift_14" = alloca i64, align 8
  %"%sc_shift_13" = alloca i64, align 8
  %"%sc_shift_12" = alloca i64, align 8
  %"%sc_shift_11" = alloca i64, align 8
  %"%sc_shift_10" = alloca i64, align 8
  %"%sc_shift_9" = alloca i64, align 8
  %"%sc_shift_8" = alloca i64, align 8
  %"%sc_shift_7" = alloca i64, align 8
  %"%sc_shift_6" = alloca i64, align 8
  %"%sc_shift_5" = alloca i64, align 8
  %"%sc_shift_4" = alloca i64, align 8
  %"%sc_shift_3" = alloca i64, align 8
  %"%sc_shift_2" = alloca i64, align 8
  %"%sc_shift_1" = alloca i64, align 8
  %"%sc_shift_0" = alloca i64, align 8
  %"%sc_normal_63" = alloca i64, align 8
  %"%sc_normal_62" = alloca i64, align 8
  %"%sc_normal_61" = alloca i64, align 8
  %"%sc_normal_60" = alloca i64, align 8
  %"%sc_normal_59" = alloca i64, align 8
  %"%sc_normal_58" = alloca i64, align 8
  %"%sc_normal_57" = alloca i64, align 8
  %"%sc_normal_56" = alloca i64, align 8
  %"%sc_normal_55" = alloca i64, align 8
  %"%sc_normal_54" = alloca i64, align 8
  %"%sc_normal_53" = alloca i64, align 8
  %"%sc_normal_52" = alloca i64, align 8
  %"%sc_normal_51" = alloca i64, align 8
  %"%sc_normal_50" = alloca i64, align 8
  %"%sc_normal_49" = alloca i64, align 8
  %"%sc_normal_48" = alloca i64, align 8
  %"%sc_normal_47" = alloca i64, align 8
  %"%sc_normal_46" = alloca i64, align 8
  %"%sc_normal_45" = alloca i64, align 8
  %"%sc_normal_44" = alloca i64, align 8
  %"%sc_normal_43" = alloca i64, align 8
  %"%sc_normal_42" = alloca i64, align 8
  %"%sc_normal_41" = alloca i64, align 8
  %"%sc_normal_40" = alloca i64, align 8
  %"%sc_normal_39" = alloca i64, align 8
  %"%sc_normal_38" = alloca i64, align 8
  %"%sc_normal_37" = alloca i64, align 8
  %"%sc_normal_36" = alloca i64, align 8
  %"%sc_normal_35" = alloca i64, align 8
  %"%sc_normal_34" = alloca i64, align 8
  %"%sc_normal_33" = alloca i64, align 8
  %"%sc_normal_32" = alloca i64, align 8
  %"%sc_normal_31" = alloca i64, align 8
  %"%sc_normal_30" = alloca i64, align 8
  %"%sc_normal_29" = alloca i64, align 8
  %"%sc_normal_28" = alloca i64, align 8
  %"%sc_normal_27" = alloca i64, align 8
  %"%sc_normal_26" = alloca i64, align 8
  %"%sc_normal_25" = alloca i64, align 8
  %"%sc_normal_24" = alloca i64, align 8
  %"%sc_normal_23" = alloca i64, align 8
  %"%sc_normal_22" = alloca i64, align 8
  %"%sc_normal_21" = alloca i64, align 8
  %"%sc_normal_20" = alloca i64, align 8
  %"%sc_normal_19" = alloca i64, align 8
  %"%sc_normal_18" = alloca i64, align 8
  %"%sc_normal_17" = alloca i64, align 8
  %"%sc_normal_16" = alloca i64, align 8
  %"%sc_normal_15" = alloca i64, align 8
  %"%sc_normal_14" = alloca i64, align 8
  %"%sc_normal_13" = alloca i64, align 8
  %"%sc_normal_12" = alloca i64, align 8
  %"%sc_normal_11" = alloca i64, align 8
  %"%sc_normal_10" = alloca i64, align 8
  %"%sc_normal_9" = alloca i64, align 8
  %"%sc_normal_8" = alloca i64, align 8
  %"%sc_normal_7" = alloca i64, align 8
  %"%sc_normal_6" = alloca i64, align 8
  %"%sc_normal_5" = alloca i64, align 8
  %"%sc_normal_4" = alloca i64, align 8
  %"%sc_normal_3" = alloca i64, align 8
  %"%sc_normal_2" = alloca i64, align 8
  %"%sc_normal_1" = alloca i64, align 8
  %"%sc_normal_0" = alloca i64, align 8
  %"%MOUSE_BTN_MIDDLE" = alloca i64, align 8
  %"%MOUSE_BTN_RIGHT" = alloca i64, align 8
  %"%MOUSE_BTN_LEFT" = alloca i64, align 8
  %"%MOD_RMETA" = alloca i64, align 8
  %"%MOD_RALT" = alloca i64, align 8
  %"%MOD_RSHIFT" = alloca i64, align 8
  %"%MOD_RCTRL" = alloca i64, align 8
  %"%MOD_LMETA" = alloca i64, align 8
  %"%MOD_LALT" = alloca i64, align 8
  %"%MOD_LSHIFT" = alloca i64, align 8
  %"%MOD_LCTRL" = alloca i64, align 8
  %"%MOUSE_REPORT_SIZE" = alloca i64, align 8
  %"%KBD_REPORT_SIZE" = alloca i64, align 8
  %"%MOUSE_BUF_SIZE" = alloca i64, align 8
  %"%KBD_BUF_SIZE" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 64, ptr %"%KBD_BUF_SIZE", align 8
  store i64 32, ptr %"%MOUSE_BUF_SIZE", align 8
  store i64 8, ptr %"%KBD_REPORT_SIZE", align 8
  store i64 4, ptr %"%MOUSE_REPORT_SIZE", align 8
  store i64 1, ptr %"%MOD_LCTRL", align 8
  store i64 2, ptr %"%MOD_LSHIFT", align 8
  store i64 4, ptr %"%MOD_LALT", align 8
  store i64 8, ptr %"%MOD_LMETA", align 8
  store i64 16, ptr %"%MOD_RCTRL", align 8
  store i64 32, ptr %"%MOD_RSHIFT", align 8
  store i64 64, ptr %"%MOD_RALT", align 8
  store i64 128, ptr %"%MOD_RMETA", align 8
  store i64 1, ptr %"%MOUSE_BTN_LEFT", align 8
  store i64 2, ptr %"%MOUSE_BTN_RIGHT", align 8
  store i64 4, ptr %"%MOUSE_BTN_MIDDLE", align 8
  store i64 0, ptr %"%sc_normal_0", align 8
  store i64 0, ptr %"%sc_normal_1", align 8
  store i64 0, ptr %"%sc_normal_2", align 8
  store i64 0, ptr %"%sc_normal_3", align 8
  store i64 97, ptr %"%sc_normal_4", align 8
  store i64 98, ptr %"%sc_normal_5", align 8
  store i64 99, ptr %"%sc_normal_6", align 8
  store i64 100, ptr %"%sc_normal_7", align 8
  store i64 101, ptr %"%sc_normal_8", align 8
  store i64 102, ptr %"%sc_normal_9", align 8
  store i64 103, ptr %"%sc_normal_10", align 8
  store i64 104, ptr %"%sc_normal_11", align 8
  store i64 105, ptr %"%sc_normal_12", align 8
  store i64 106, ptr %"%sc_normal_13", align 8
  store i64 107, ptr %"%sc_normal_14", align 8
  store i64 108, ptr %"%sc_normal_15", align 8
  store i64 109, ptr %"%sc_normal_16", align 8
  store i64 110, ptr %"%sc_normal_17", align 8
  store i64 111, ptr %"%sc_normal_18", align 8
  store i64 112, ptr %"%sc_normal_19", align 8
  store i64 113, ptr %"%sc_normal_20", align 8
  store i64 114, ptr %"%sc_normal_21", align 8
  store i64 115, ptr %"%sc_normal_22", align 8
  store i64 116, ptr %"%sc_normal_23", align 8
  store i64 117, ptr %"%sc_normal_24", align 8
  store i64 118, ptr %"%sc_normal_25", align 8
  store i64 119, ptr %"%sc_normal_26", align 8
  store i64 120, ptr %"%sc_normal_27", align 8
  store i64 121, ptr %"%sc_normal_28", align 8
  store i64 122, ptr %"%sc_normal_29", align 8
  store i64 49, ptr %"%sc_normal_30", align 8
  store i64 50, ptr %"%sc_normal_31", align 8
  store i64 51, ptr %"%sc_normal_32", align 8
  store i64 52, ptr %"%sc_normal_33", align 8
  store i64 53, ptr %"%sc_normal_34", align 8
  store i64 54, ptr %"%sc_normal_35", align 8
  store i64 55, ptr %"%sc_normal_36", align 8
  store i64 56, ptr %"%sc_normal_37", align 8
  store i64 57, ptr %"%sc_normal_38", align 8
  store i64 48, ptr %"%sc_normal_39", align 8
  store i64 10, ptr %"%sc_normal_40", align 8
  store i64 27, ptr %"%sc_normal_41", align 8
  store i64 8, ptr %"%sc_normal_42", align 8
  store i64 9, ptr %"%sc_normal_43", align 8
  store i64 32, ptr %"%sc_normal_44", align 8
  store i64 45, ptr %"%sc_normal_45", align 8
  store i64 61, ptr %"%sc_normal_46", align 8
  store i64 91, ptr %"%sc_normal_47", align 8
  store i64 93, ptr %"%sc_normal_48", align 8
  store i64 92, ptr %"%sc_normal_49", align 8
  store i64 35, ptr %"%sc_normal_50", align 8
  store i64 59, ptr %"%sc_normal_51", align 8
  store i64 39, ptr %"%sc_normal_52", align 8
  store i64 96, ptr %"%sc_normal_53", align 8
  store i64 44, ptr %"%sc_normal_54", align 8
  store i64 46, ptr %"%sc_normal_55", align 8
  store i64 47, ptr %"%sc_normal_56", align 8
  store i64 0, ptr %"%sc_normal_57", align 8
  store i64 0, ptr %"%sc_normal_58", align 8
  store i64 0, ptr %"%sc_normal_59", align 8
  store i64 0, ptr %"%sc_normal_60", align 8
  store i64 0, ptr %"%sc_normal_61", align 8
  store i64 0, ptr %"%sc_normal_62", align 8
  store i64 0, ptr %"%sc_normal_63", align 8
  store i64 0, ptr %"%sc_shift_0", align 8
  store i64 0, ptr %"%sc_shift_1", align 8
  store i64 0, ptr %"%sc_shift_2", align 8
  store i64 0, ptr %"%sc_shift_3", align 8
  store i64 65, ptr %"%sc_shift_4", align 8
  store i64 66, ptr %"%sc_shift_5", align 8
  store i64 67, ptr %"%sc_shift_6", align 8
  store i64 68, ptr %"%sc_shift_7", align 8
  store i64 69, ptr %"%sc_shift_8", align 8
  store i64 70, ptr %"%sc_shift_9", align 8
  store i64 71, ptr %"%sc_shift_10", align 8
  store i64 72, ptr %"%sc_shift_11", align 8
  store i64 73, ptr %"%sc_shift_12", align 8
  store i64 74, ptr %"%sc_shift_13", align 8
  store i64 75, ptr %"%sc_shift_14", align 8
  store i64 76, ptr %"%sc_shift_15", align 8
  store i64 77, ptr %"%sc_shift_16", align 8
  store i64 78, ptr %"%sc_shift_17", align 8
  store i64 79, ptr %"%sc_shift_18", align 8
  store i64 80, ptr %"%sc_shift_19", align 8
  store i64 81, ptr %"%sc_shift_20", align 8
  store i64 82, ptr %"%sc_shift_21", align 8
  store i64 83, ptr %"%sc_shift_22", align 8
  store i64 84, ptr %"%sc_shift_23", align 8
  store i64 85, ptr %"%sc_shift_24", align 8
  store i64 86, ptr %"%sc_shift_25", align 8
  store i64 87, ptr %"%sc_shift_26", align 8
  store i64 88, ptr %"%sc_shift_27", align 8
  store i64 89, ptr %"%sc_shift_28", align 8
  store i64 90, ptr %"%sc_shift_29", align 8
  store i64 33, ptr %"%sc_shift_30", align 8
  store i64 64, ptr %"%sc_shift_31", align 8
  store i64 35, ptr %"%sc_shift_32", align 8
  store i64 36, ptr %"%sc_shift_33", align 8
  store i64 37, ptr %"%sc_shift_34", align 8
  store i64 94, ptr %"%sc_shift_35", align 8
  store i64 38, ptr %"%sc_shift_36", align 8
  store i64 42, ptr %"%sc_shift_37", align 8
  store i64 40, ptr %"%sc_shift_38", align 8
  store i64 41, ptr %"%sc_shift_39", align 8
  store i64 10, ptr %"%sc_shift_40", align 8
  store i64 27, ptr %"%sc_shift_41", align 8
  store i64 8, ptr %"%sc_shift_42", align 8
  store i64 9, ptr %"%sc_shift_43", align 8
  store i64 32, ptr %"%sc_shift_44", align 8
  store i64 95, ptr %"%sc_shift_45", align 8
  store i64 43, ptr %"%sc_shift_46", align 8
  store i64 123, ptr %"%sc_shift_47", align 8
  store i64 125, ptr %"%sc_shift_48", align 8
  store i64 124, ptr %"%sc_shift_49", align 8
  store i64 126, ptr %"%sc_shift_50", align 8
  store i64 58, ptr %"%sc_shift_51", align 8
  store i64 34, ptr %"%sc_shift_52", align 8
  store i64 126, ptr %"%sc_shift_53", align 8
  store i64 60, ptr %"%sc_shift_54", align 8
  store i64 62, ptr %"%sc_shift_55", align 8
  store i64 63, ptr %"%sc_shift_56", align 8
  store i64 0, ptr %"%sc_shift_57", align 8
  store i64 0, ptr %"%sc_shift_58", align 8
  store i64 0, ptr %"%sc_shift_59", align 8
  store i64 0, ptr %"%sc_shift_60", align 8
  store i64 0, ptr %"%sc_shift_61", align 8
  store i64 0, ptr %"%sc_shift_62", align 8
  store i64 0, ptr %"%sc_shift_63", align 8
  store i64 -1, ptr %"%kbd_dev_idx", align 8
  store i64 0, ptr %"%kbd_hc_idx", align 8
  store i64 0, ptr %"%kbd_dev_addr", align 8
  store i64 1, ptr %"%kbd_endpoint", align 8
  store i64 0, ptr %"%kbd_report_buf", align 8
  store i64 0, ptr %"%kbd_prev_report", align 8
  store i64 0, ptr %"%kbd_active", align 8
  store i64 0, ptr %"%kbd_event_buf", align 8
  store i64 0, ptr %"%kbd_event_head", align 8
  store i64 0, ptr %"%kbd_event_tail", align 8
  store i64 0, ptr %"%kbd_modifiers", align 8
  store i64 0, ptr %"%kbd_capslock", align 8
  store i64 0, ptr %"%kbd_numlock", align 8
  store i64 0, ptr %"%kbd_scrolllock", align 8
  store i64 0, ptr %"%kbd_pressed_0", align 8
  store i64 0, ptr %"%kbd_pressed_1", align 8
  store i64 0, ptr %"%kbd_pressed_2", align 8
  store i64 0, ptr %"%kbd_pressed_3", align 8
  store i64 0, ptr %"%kbd_pressed_4", align 8
  store i64 0, ptr %"%kbd_pressed_5", align 8
  store i64 -1, ptr %"%mouse_dev_idx", align 8
  store i64 0, ptr %"%mouse_hc_idx", align 8
  store i64 0, ptr %"%mouse_dev_addr", align 8
  store i64 1, ptr %"%mouse_endpoint", align 8
  store i64 0, ptr %"%mouse_report_buf", align 8
  store i64 0, ptr %"%mouse_active", align 8
  store i64 0, ptr %"%mouse_x", align 8
  store i64 0, ptr %"%mouse_y", align 8
  store i64 0, ptr %"%mouse_buttons", align 8
  store i64 0, ptr %"%mouse_dx", align 8
  store i64 0, ptr %"%mouse_dy", align 8
  store i64 0, ptr %"%mouse_scroll", align 8
  store i64 0, ptr %"%mouse_event_buf", align 8
  store i64 0, ptr %"%mouse_event_head", align 8
  store i64 0, ptr %"%mouse_event_tail", align 8
  store i64 1024, ptr %"%mouse_max_x", align 8
  store i64 768, ptr %"%mouse_max_y", align 8
  store i64 0, ptr %"%hid_kbd_events", align 8
  store i64 0, ptr %"%hid_mouse_events", align 8
  store i64 0, ptr %"%hid_poll_count", align 8
  store i64 0, ptr %"%hid_errors", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
