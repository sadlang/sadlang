; ModuleID = 'ps2_نحلة'
source_filename = "ps2_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@COM1 = constant i64 1016
@PS2_DATA = constant i64 96
@PS2_STATUS = constant i64 100
@PS2_COMMAND = constant i64 100
@PS2_STS_OUTPUT = constant i64 1
@PS2_STS_INPUT = constant i64 2
@PS2_STS_SYSTEM = constant i64 4
@PS2_STS_CMD = constant i64 8
@PS2_STS_TIMEOUT = constant i64 64
@PS2_STS_PARITY = constant i64 128
@PS2_CMD_READ_CONFIG = constant i64 32
@PS2_CMD_WRITE_CONFIG = constant i64 96
@PS2_CMD_DISABLE_PORT2 = constant i64 167
@PS2_CMD_ENABLE_PORT2 = constant i64 168
@PS2_CMD_TEST_PORT2 = constant i64 169
@PS2_CMD_SELF_TEST = constant i64 170
@PS2_CMD_TEST_PORT1 = constant i64 171
@PS2_CMD_DISABLE_PORT1 = constant i64 173
@PS2_CMD_ENABLE_PORT1 = constant i64 174
@PS2_CMD_WRITE_PORT2 = constant i64 212
@PS2_CFG_INT1 = constant i64 1
@PS2_CFG_INT2 = constant i64 2
@PS2_CFG_SYSTEM = constant i64 4
@PS2_CFG_CLOCK1 = constant i64 16
@PS2_CFG_CLOCK2 = constant i64 32
@PS2_CFG_TRANS = constant i64 64
@PS2_DEV_RESET = constant i64 255
@PS2_DEV_ENABLE_SCAN = constant i64 244
@PS2_DEV_DISABLE_SCAN = constant i64 245
@PS2_DEV_IDENTIFY = constant i64 242
@PS2_DEV_SET_RATE = constant i64 243
@PS2_DEV_SET_SCANSET = constant i64 240
@PS2_DEV_SET_LEDS = constant i64 237
@PS2_RESP_ACK = constant i64 250
@PS2_RESP_RESEND = constant i64 254
@PS2_RESP_ERROR = constant i64 252
@PS2_RESP_SELF_OK = constant i64 170
@PS2_RESP_ECHO = constant i64 238
@SC2_RELEASE = constant i64 240
@SC2_EXTENDED = constant i64 224
@SC2_EXTENDED2 = constant i64 225
@KEY_BUF_SIZE = constant i64 128
@MOUSE_BUF_SIZE = constant i64 64
@"ps2_\D9\87\D9\8A\D8\A6_\D8\AA\D9\85" = global i64 0
@ps2_port1_exists = global i64 0
@ps2_port2_exists = global i64 0
@ps2_dual_channel = global i64 0
@kbd_extended = global i64 0
@kbd_release = global i64 0
@kbd_shift_left = global i64 0
@kbd_shift_right = global i64 0
@kbd_ctrl_left = global i64 0
@kbd_ctrl_right = global i64 0
@kbd_alt_left = global i64 0
@kbd_alt_right = global i64 0
@kbd_capslock = global i64 0
@kbd_numlock = global i64 0
@kbd_scrolllock = global i64 0
@kbd_leds = global i64 0
@kbd_buf_scancode_0 = global i64 0
@kbd_buf_scancode_1 = global i64 0
@kbd_buf_scancode_2 = global i64 0
@kbd_buf_scancode_3 = global i64 0
@kbd_buf_scancode_4 = global i64 0
@kbd_buf_scancode_5 = global i64 0
@kbd_buf_scancode_6 = global i64 0
@kbd_buf_scancode_7 = global i64 0
@kbd_buf_scancode_8 = global i64 0
@kbd_buf_scancode_9 = global i64 0
@kbd_buf_scancode_10 = global i64 0
@kbd_buf_scancode_11 = global i64 0
@kbd_buf_scancode_12 = global i64 0
@kbd_buf_scancode_13 = global i64 0
@kbd_buf_scancode_14 = global i64 0
@kbd_buf_scancode_15 = global i64 0
@kbd_buf_scancode_16 = global i64 0
@kbd_buf_scancode_17 = global i64 0
@kbd_buf_scancode_18 = global i64 0
@kbd_buf_scancode_19 = global i64 0
@kbd_buf_scancode_20 = global i64 0
@kbd_buf_scancode_21 = global i64 0
@kbd_buf_scancode_22 = global i64 0
@kbd_buf_scancode_23 = global i64 0
@kbd_buf_scancode_24 = global i64 0
@kbd_buf_scancode_25 = global i64 0
@kbd_buf_scancode_26 = global i64 0
@kbd_buf_scancode_27 = global i64 0
@kbd_buf_scancode_28 = global i64 0
@kbd_buf_scancode_29 = global i64 0
@kbd_buf_scancode_30 = global i64 0
@kbd_buf_scancode_31 = global i64 0
@kbd_buf_ascii_0 = global i64 0
@kbd_buf_ascii_1 = global i64 0
@kbd_buf_ascii_2 = global i64 0
@kbd_buf_ascii_3 = global i64 0
@kbd_buf_ascii_4 = global i64 0
@kbd_buf_ascii_5 = global i64 0
@kbd_buf_ascii_6 = global i64 0
@kbd_buf_ascii_7 = global i64 0
@kbd_buf_ascii_8 = global i64 0
@kbd_buf_ascii_9 = global i64 0
@kbd_buf_ascii_10 = global i64 0
@kbd_buf_ascii_11 = global i64 0
@kbd_buf_ascii_12 = global i64 0
@kbd_buf_ascii_13 = global i64 0
@kbd_buf_ascii_14 = global i64 0
@kbd_buf_ascii_15 = global i64 0
@kbd_buf_ascii_16 = global i64 0
@kbd_buf_ascii_17 = global i64 0
@kbd_buf_ascii_18 = global i64 0
@kbd_buf_ascii_19 = global i64 0
@kbd_buf_ascii_20 = global i64 0
@kbd_buf_ascii_21 = global i64 0
@kbd_buf_ascii_22 = global i64 0
@kbd_buf_ascii_23 = global i64 0
@kbd_buf_ascii_24 = global i64 0
@kbd_buf_ascii_25 = global i64 0
@kbd_buf_ascii_26 = global i64 0
@kbd_buf_ascii_27 = global i64 0
@kbd_buf_ascii_28 = global i64 0
@kbd_buf_ascii_29 = global i64 0
@kbd_buf_ascii_30 = global i64 0
@kbd_buf_ascii_31 = global i64 0
@kbd_buf_head = global i64 0
@kbd_buf_tail = global i64 0
@kbd_buf_count = global i64 0
@mouse_x = global i64 320
@mouse_y = global i64 240
@mouse_buttons = global i64 0
@mouse_byte_idx = global i64 0
@mouse_byte0 = global i64 0
@mouse_byte1 = global i64 0
@mouse_byte2 = global i64 0
@mouse_has_wheel = global i64 0
@mouse_byte3 = global i64 0
@mouse_wheel = global i64 0
@mouse_phase = global i64 0
@mouse_max_x = global i64 639
@mouse_max_y = global i64 479
@ps2_kbd_irq_count = global i64 0
@ps2_mouse_irq_count = global i64 0
@ps2_errors = global i64 0
@sc2_ascii_0 = global i64 0
@sc2_ascii_1 = global i64 0
@sc2_ascii_2 = global i64 0
@sc2_ascii_3 = global i64 0
@sc2_ascii_4 = global i64 0
@sc2_ascii_5 = global i64 0
@sc2_ascii_6 = global i64 0
@sc2_ascii_7 = global i64 0
@sc2_ascii_8 = global i64 0
@sc2_ascii_9 = global i64 0
@sc2_ascii_10 = global i64 0
@sc2_ascii_11 = global i64 0
@sc2_ascii_12 = global i64 0
@sc2_ascii_13 = global i64 9
@sc2_ascii_14 = global i64 96
@sc2_ascii_15 = global i64 0
@sc2_ascii_16 = global i64 0
@sc2_ascii_17 = global i64 0
@sc2_ascii_18 = global i64 0
@sc2_ascii_19 = global i64 0
@sc2_ascii_20 = global i64 0
@sc2_ascii_21 = global i64 113
@sc2_ascii_22 = global i64 49
@sc2_ascii_23 = global i64 0
@sc2_ascii_24 = global i64 0
@sc2_ascii_25 = global i64 0
@sc2_ascii_26 = global i64 122
@sc2_ascii_27 = global i64 115
@sc2_ascii_28 = global i64 97
@sc2_ascii_29 = global i64 119
@sc2_ascii_30 = global i64 50
@sc2_ascii_31 = global i64 0
@sc2_ascii_32 = global i64 0
@sc2_ascii_33 = global i64 99
@sc2_ascii_34 = global i64 120
@sc2_ascii_35 = global i64 100
@sc2_ascii_36 = global i64 101
@sc2_ascii_37 = global i64 52
@sc2_ascii_38 = global i64 51
@sc2_ascii_39 = global i64 0
@sc2_ascii_40 = global i64 0
@sc2_ascii_41 = global i64 32
@sc2_ascii_42 = global i64 118
@sc2_ascii_43 = global i64 102
@sc2_ascii_44 = global i64 116
@sc2_ascii_45 = global i64 114
@sc2_ascii_46 = global i64 53
@sc2_ascii_47 = global i64 0
@sc2_ascii_48 = global i64 0
@sc2_ascii_49 = global i64 110
@sc2_ascii_50 = global i64 98
@sc2_ascii_51 = global i64 104
@sc2_ascii_52 = global i64 103
@sc2_ascii_53 = global i64 121
@sc2_ascii_54 = global i64 54
@sc2_ascii_55 = global i64 0
@sc2_ascii_56 = global i64 0
@sc2_ascii_57 = global i64 0
@sc2_ascii_58 = global i64 109
@sc2_ascii_59 = global i64 106
@sc2_ascii_60 = global i64 117
@sc2_ascii_61 = global i64 55
@sc2_ascii_62 = global i64 56
@sc2_ascii_63 = global i64 0
@sc2_ascii_64 = global i64 0
@sc2_ascii_65 = global i64 44
@sc2_ascii_66 = global i64 107
@sc2_ascii_67 = global i64 105
@sc2_ascii_68 = global i64 111
@sc2_ascii_69 = global i64 48
@sc2_ascii_70 = global i64 57
@sc2_ascii_71 = global i64 0
@sc2_ascii_72 = global i64 0
@sc2_ascii_73 = global i64 46
@sc2_ascii_74 = global i64 47
@sc2_ascii_75 = global i64 108
@sc2_ascii_76 = global i64 59
@sc2_ascii_77 = global i64 112
@sc2_ascii_78 = global i64 45
@sc2_ascii_79 = global i64 0
@sc2_ascii_80 = global i64 0
@sc2_ascii_81 = global i64 0
@sc2_ascii_82 = global i64 39
@sc2_ascii_83 = global i64 0
@sc2_ascii_84 = global i64 91
@sc2_ascii_85 = global i64 61
@sc2_ascii_86 = global i64 0
@sc2_ascii_87 = global i64 0
@sc2_ascii_88 = global i64 0
@sc2_ascii_89 = global i64 0
@sc2_ascii_90 = global i64 13
@sc2_ascii_91 = global i64 93
@sc2_ascii_92 = global i64 0
@sc2_ascii_93 = global i64 92
@sc2_ascii_94 = global i64 0
@sc2_ascii_95 = global i64 0
@sc2_ascii_96 = global i64 0
@sc2_ascii_97 = global i64 0
@sc2_ascii_98 = global i64 0
@sc2_ascii_99 = global i64 0
@sc2_ascii_100 = global i64 0
@sc2_ascii_101 = global i64 0
@sc2_ascii_102 = global i64 8
@sc2_ascii_103 = global i64 0
@sc2_ascii_104 = global i64 0
@sc2_ascii_105 = global i64 0
@sc2_ascii_106 = global i64 0
@sc2_ascii_107 = global i64 0
@sc2_ascii_108 = global i64 0
@sc2_ascii_109 = global i64 0
@sc2_ascii_110 = global i64 0
@sc2_ascii_111 = global i64 0
@sc2_ascii_112 = global i64 0
@sc2_ascii_113 = global i64 0
@sc2_ascii_114 = global i64 0
@sc2_ascii_115 = global i64 0
@sc2_ascii_116 = global i64 0
@sc2_ascii_117 = global i64 0
@sc2_ascii_118 = global i64 27
@sc2_ascii_119 = global i64 0
@sc2_ascii_120 = global i64 0
@sc2_ascii_121 = global i64 0
@sc2_ascii_122 = global i64 0
@sc2_ascii_123 = global i64 0
@sc2_ascii_124 = global i64 0
@sc2_ascii_125 = global i64 0
@sc2_ascii_126 = global i64 0
@sc2_ascii_127 = global i64 0
@sc2_shift_14 = global i64 126
@sc2_shift_21 = global i64 81
@sc2_shift_22 = global i64 33
@sc2_shift_26 = global i64 90
@sc2_shift_27 = global i64 83
@sc2_shift_28 = global i64 65
@sc2_shift_29 = global i64 87
@sc2_shift_30 = global i64 64
@sc2_shift_33 = global i64 67
@sc2_shift_34 = global i64 88
@sc2_shift_35 = global i64 68
@sc2_shift_36 = global i64 69
@sc2_shift_37 = global i64 36
@sc2_shift_38 = global i64 35
@sc2_shift_42 = global i64 86
@sc2_shift_43 = global i64 70
@sc2_shift_44 = global i64 84
@sc2_shift_45 = global i64 82
@sc2_shift_46 = global i64 37
@sc2_shift_49 = global i64 78
@sc2_shift_50 = global i64 66
@sc2_shift_51 = global i64 72
@sc2_shift_52 = global i64 71
@sc2_shift_53 = global i64 89
@sc2_shift_54 = global i64 94
@sc2_shift_58 = global i64 77
@sc2_shift_59 = global i64 74
@sc2_shift_60 = global i64 85
@sc2_shift_61 = global i64 38
@sc2_shift_62 = global i64 42
@sc2_shift_65 = global i64 60
@sc2_shift_66 = global i64 75
@sc2_shift_67 = global i64 73
@sc2_shift_68 = global i64 79
@sc2_shift_69 = global i64 41
@sc2_shift_70 = global i64 40
@sc2_shift_73 = global i64 62
@sc2_shift_74 = global i64 63
@sc2_shift_75 = global i64 76
@sc2_shift_76 = global i64 58
@sc2_shift_77 = global i64 80
@sc2_shift_78 = global i64 95
@sc2_shift_82 = global i64 34
@sc2_shift_84 = global i64 123
@sc2_shift_85 = global i64 43
@sc2_shift_91 = global i64 125
@sc2_shift_93 = global i64 124
@str.const = private unnamed_addr constant [34 x i8] c"[PS/2] IntelliMouse wheel enabled\00", align 1
@str.const.1 = private unnamed_addr constant [29 x i8] c"[PS/2] No wheel support (id=\00", align 1
@str.const.2 = private unnamed_addr constant [2 x i8] c")\00", align 1
@str.const.3 = private unnamed_addr constant [38 x i8] c"\E2\95\90\E2\95\90\E2\95\90 [PS/2] \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.4 = private unnamed_addr constant [10 x i8] c"  Port1: \00", align 1
@str.const.5 = private unnamed_addr constant [3 x i8] c"OK\00", align 1
@str.const.6 = private unnamed_addr constant [4 x i8] c"N/A\00", align 1
@str.const.7 = private unnamed_addr constant [10 x i8] c"  Port2: \00", align 1
@str.const.10 = private unnamed_addr constant [13 x i8] c"  Kbd IRQs: \00", align 1
@str.const.11 = private unnamed_addr constant [14 x i8] c" Mouse IRQs: \00", align 1
@str.const.12 = private unnamed_addr constant [12 x i8] c"  Key buf: \00", align 1
@str.const.13 = private unnamed_addr constant [9 x i8] c"  Shift:\00", align 1
@str.const.14 = private unnamed_addr constant [7 x i8] c" Ctrl:\00", align 1
@str.const.15 = private unnamed_addr constant [6 x i8] c" Alt:\00", align 1
@str.const.16 = private unnamed_addr constant [8 x i8] c"  Caps:\00", align 1
@str.const.17 = private unnamed_addr constant [6 x i8] c" Num:\00", align 1
@str.const.18 = private unnamed_addr constant [9 x i8] c" Scroll:\00", align 1
@str.const.19 = private unnamed_addr constant [11 x i8] c"  Mouse: (\00", align 1
@str.const.20 = private unnamed_addr constant [2 x i8] c",\00", align 1
@str.const.21 = private unnamed_addr constant [7 x i8] c") btn=\00", align 1
@str.const.22 = private unnamed_addr constant [6 x i8] c" whl=\00", align 1
@str.const.23 = private unnamed_addr constant [106 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.24 = private unnamed_addr constant [37 x i8] c"[PS/2] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D9\85\D8\AA\D8\AD\D9\83\D9\85 PS/2...\00", align 1
@str.const.25 = private unnamed_addr constant [23 x i8] c"[PS/2] Config byte: 0x\00", align 1
@str.const.26 = private unnamed_addr constant [24 x i8] c"[PS/2] Self-test passed\00", align 1
@str.const.27 = private unnamed_addr constant [28 x i8] c"[PS/2] Self-test FAILED: 0x\00", align 1
@str.const.28 = private unnamed_addr constant [26 x i8] c"[PS/2] Port 1 test passed\00", align 1
@str.const.29 = private unnamed_addr constant [28 x i8] c"[PS/2] Port 1 test failed: \00", align 1
@str.const.30 = private unnamed_addr constant [26 x i8] c"[PS/2] Port 2 test passed\00", align 1
@str.const.31 = private unnamed_addr constant [28 x i8] c"[PS/2] Port 2 test failed: \00", align 1
@str.const.32 = private unnamed_addr constant [25 x i8] c"[PS/2] Keyboard reset OK\00", align 1
@str.const.33 = private unnamed_addr constant [16 x i8] c"[PS/2] \D8\AC\D8\A7\D9\87\D8\B2\00", align 1

declare void @sad_ll_port_inb(i64)

declare void @sad_ll_port_outb(i64, i64)

declare void @sad_ll_mem_read8(i64)

declare void @sad_ll_mem_write8(i64, i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_kmalloc(i64)

declare void @sad_ll_memset(i64, i64, i64)

define i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
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

define i64 @"ps2_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
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

define i64 @ps2_serial_hex8(i64 %val) {
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

define i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"() {
entry:
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 13)
  %"%COM1.load1" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load1", i64 10)
  ret i64 0
}

define i64 @ps2_wait_input() {
entry:
  %"%sts" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  store i64 10000, ptr %"%timeout", align 8
  br label %while_cond_30

merge_34:                                         ; preds = %while_body_31
  %"%timeout.load1" = load i64, ptr %"%timeout", align 8
  %subtmp = sub i64 %"%timeout.load1", 1
  store i64 %subtmp, ptr %"%timeout", align 8
  br label %while_cond_30

then_33:                                          ; preds = %while_body_31
  ret i64 0

while_body_31:                                    ; preds = %while_cond_30
  %"%PS2_STATUS.load" = load i64, ptr @PS2_STATUS, align 8
  call void @sad_ll_port_inb(i64 %"%PS2_STATUS.load")
  store i64 0, ptr %"%sts", align 8
  %"%sts.load" = load i64, ptr %"%sts", align 8
  %shrtmp = lshr i64 %"%sts.load", 1
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_33, label %merge_34

while_cond_30:                                    ; preds = %merge_34, %entry
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp, label %while_body_31, label %while_exit_32

while_exit_32:                                    ; preds = %while_cond_30
  ret i64 -1
}

define i64 @ps2_wait_output() {
entry:
  %"%sts" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  store i64 10000, ptr %"%timeout", align 8
  br label %while_cond_35

merge_39:                                         ; preds = %while_body_36
  %"%timeout.load1" = load i64, ptr %"%timeout", align 8
  %subtmp = sub i64 %"%timeout.load1", 1
  store i64 %subtmp, ptr %"%timeout", align 8
  br label %while_cond_35

then_38:                                          ; preds = %while_body_36
  ret i64 0

while_body_36:                                    ; preds = %while_cond_35
  %"%PS2_STATUS.load" = load i64, ptr @PS2_STATUS, align 8
  call void @sad_ll_port_inb(i64 %"%PS2_STATUS.load")
  store i64 0, ptr %"%sts", align 8
  %"%sts.load" = load i64, ptr %"%sts", align 8
  %andtmp = and i64 %"%sts.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_38, label %merge_39

while_cond_35:                                    ; preds = %merge_39, %entry
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp, label %while_body_36, label %while_exit_37

while_exit_37:                                    ; preds = %while_cond_35
  ret i64 -1
}

define i64 @ps2_send_controller_cmd(i64 %cmd) {
entry:
  %cmd1 = alloca i64, align 8
  store i64 %cmd, ptr %cmd1, align 8
  %calltmp = call i64 @ps2_wait_input()
  %"%PS2_COMMAND.load" = load i64, ptr @PS2_COMMAND, align 8
  %"%cmd.load" = load i64, ptr %cmd1, align 8
  call void @sad_ll_port_outb(i64 %"%PS2_COMMAND.load", i64 %"%cmd.load")
  ret i64 0
}

define i64 @ps2_send_controller_data(i64 %data) {
entry:
  %data1 = alloca i64, align 8
  store i64 %data, ptr %data1, align 8
  %calltmp = call i64 @ps2_wait_input()
  %"%PS2_DATA.load" = load i64, ptr @PS2_DATA, align 8
  %"%data.load" = load i64, ptr %data1, align 8
  call void @sad_ll_port_outb(i64 %"%PS2_DATA.load", i64 %"%data.load")
  ret i64 0
}

define i64 @ps2_read_data() {
entry:
  %"%rc" = alloca i64, align 8
  %calltmp = call i64 @ps2_wait_output()
  store i64 %calltmp, ptr %"%rc", align 8
  %"%rc.load" = load i64, ptr %"%rc", align 8
  %cmplttmp = icmp slt i64 %"%rc.load", 0
  br i1 %cmplttmp, label %then_40, label %merge_41

merge_41:                                         ; preds = %entry
  %"%PS2_DATA.load" = load i64, ptr @PS2_DATA, align 8
  call void @sad_ll_port_inb(i64 %"%PS2_DATA.load")
  ret i64 0

then_40:                                          ; preds = %entry
  ret i64 -1
}

define i64 @ps2_send_device1_cmd(i64 %cmd) {
entry:
  %"%resp" = alloca i64, align 8
  %cmd1 = alloca i64, align 8
  store i64 %cmd, ptr %cmd1, align 8
  %calltmp = call i64 @ps2_wait_input()
  %"%PS2_DATA.load" = load i64, ptr @PS2_DATA, align 8
  %"%cmd.load" = load i64, ptr %cmd1, align 8
  call void @sad_ll_port_outb(i64 %"%PS2_DATA.load", i64 %"%cmd.load")
  %calltmp2 = call i64 @ps2_read_data()
  store i64 %calltmp2, ptr %"%resp", align 8
  %"%resp.load" = load i64, ptr %"%resp", align 8
  %"%PS2_RESP_ACK.load" = load i64, ptr @PS2_RESP_ACK, align 8
  %cmpeqtmp = icmp eq i64 %"%resp.load", %"%PS2_RESP_ACK.load"
  br i1 %cmpeqtmp, label %then_42, label %else_44

else_44:                                          ; preds = %entry
  %"%resp.load3" = load i64, ptr %"%resp", align 8
  %"%PS2_RESP_RESEND.load" = load i64, ptr @PS2_RESP_RESEND, align 8
  %cmpeqtmp4 = icmp eq i64 %"%resp.load3", %"%PS2_RESP_RESEND.load"
  br i1 %cmpeqtmp4, label %then_45, label %merge_46

merge_43:                                         ; preds = %merge_46
  ret i64 -1

merge_46:                                         ; preds = %else_44
  br label %merge_43

then_42:                                          ; preds = %entry
  ret i64 0

then_45:                                          ; preds = %else_44
  ret i64 -2
}

define i64 @ps2_send_device2_cmd(i64 %cmd) {
entry:
  %"%resp" = alloca i64, align 8
  %cmd1 = alloca i64, align 8
  store i64 %cmd, ptr %cmd1, align 8
  %"%PS2_CMD_WRITE_PORT2.load" = load i64, ptr @PS2_CMD_WRITE_PORT2, align 8
  %calltmp = call i64 @ps2_send_controller_cmd(i64 %"%PS2_CMD_WRITE_PORT2.load")
  %calltmp2 = call i64 @ps2_wait_input()
  %"%PS2_DATA.load" = load i64, ptr @PS2_DATA, align 8
  %"%cmd.load" = load i64, ptr %cmd1, align 8
  call void @sad_ll_port_outb(i64 %"%PS2_DATA.load", i64 %"%cmd.load")
  %calltmp3 = call i64 @ps2_read_data()
  store i64 %calltmp3, ptr %"%resp", align 8
  %"%resp.load" = load i64, ptr %"%resp", align 8
  %"%PS2_RESP_ACK.load" = load i64, ptr @PS2_RESP_ACK, align 8
  %cmpeqtmp = icmp eq i64 %"%resp.load", %"%PS2_RESP_ACK.load"
  br i1 %cmpeqtmp, label %then_47, label %merge_48

merge_48:                                         ; preds = %entry
  ret i64 -1

then_47:                                          ; preds = %entry
  ret i64 0
}

define i64 @ps2_flush() {
entry:
  %"%sts" = alloca i64, align 8
  %"%count" = alloca i64, align 8
  store i64 0, ptr %"%count", align 8
  br label %while_cond_49

merge_53:                                         ; preds = %then_52, %while_body_50
  %"%PS2_DATA.load" = load i64, ptr @PS2_DATA, align 8
  call void @sad_ll_port_inb(i64 %"%PS2_DATA.load")
  %"%count.load1" = load i64, ptr %"%count", align 8
  %addtmp = add i64 %"%count.load1", 1
  store i64 %addtmp, ptr %"%count", align 8
  br label %while_cond_49

then_52:                                          ; preds = %while_body_50
  br label %merge_53

while_body_50:                                    ; preds = %while_cond_49
  %"%PS2_STATUS.load" = load i64, ptr @PS2_STATUS, align 8
  call void @sad_ll_port_inb(i64 %"%PS2_STATUS.load")
  store i64 0, ptr %"%sts", align 8
  %"%sts.load" = load i64, ptr %"%sts", align 8
  %andtmp = and i64 %"%sts.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_52, label %merge_53

while_cond_49:                                    ; preds = %merge_53, %entry
  %"%count.load" = load i64, ptr %"%count", align 8
  %cmplttmp = icmp slt i64 %"%count.load", 64
  br i1 %cmplttmp, label %while_body_50, label %while_exit_51

while_exit_51:                                    ; preds = %while_cond_49
  %loadtmp = load i64, ptr %"%count", align 8
  ret i64 %loadtmp
}

define i64 @ps2_sc2_to_ascii(i64 %scancode) {
entry:
  %scancode1 = alloca i64, align 8
  store i64 %scancode, ptr %scancode1, align 8
  %"%scancode.load" = load i64, ptr %scancode1, align 8
  %cmpgttmp = icmp sgt i64 %"%scancode.load", 127
  br i1 %cmpgttmp, label %then_54, label %merge_55

merge_101:                                        ; preds = %merge_99
  %"%scancode.load69" = load i64, ptr %scancode1, align 8
  %cmpeqtmp70 = icmp eq i64 %"%scancode.load69", 23
  br i1 %cmpeqtmp70, label %then_102, label %merge_103

merge_103:                                        ; preds = %merge_101
  %"%scancode.load72" = load i64, ptr %scancode1, align 8
  %cmpeqtmp73 = icmp eq i64 %"%scancode.load72", 24
  br i1 %cmpeqtmp73, label %then_104, label %merge_105

merge_105:                                        ; preds = %merge_103
  %"%scancode.load75" = load i64, ptr %scancode1, align 8
  %cmpeqtmp76 = icmp eq i64 %"%scancode.load75", 25
  br i1 %cmpeqtmp76, label %then_106, label %merge_107

merge_107:                                        ; preds = %merge_105
  %"%scancode.load78" = load i64, ptr %scancode1, align 8
  %cmpeqtmp79 = icmp eq i64 %"%scancode.load78", 26
  br i1 %cmpeqtmp79, label %then_108, label %merge_109

merge_109:                                        ; preds = %merge_107
  %"%scancode.load81" = load i64, ptr %scancode1, align 8
  %cmpeqtmp82 = icmp eq i64 %"%scancode.load81", 27
  br i1 %cmpeqtmp82, label %then_110, label %merge_111

merge_111:                                        ; preds = %merge_109
  %"%scancode.load84" = load i64, ptr %scancode1, align 8
  %cmpeqtmp85 = icmp eq i64 %"%scancode.load84", 28
  br i1 %cmpeqtmp85, label %then_112, label %merge_113

merge_113:                                        ; preds = %merge_111
  %"%scancode.load87" = load i64, ptr %scancode1, align 8
  %cmpeqtmp88 = icmp eq i64 %"%scancode.load87", 29
  br i1 %cmpeqtmp88, label %then_114, label %merge_115

merge_115:                                        ; preds = %merge_113
  %"%scancode.load90" = load i64, ptr %scancode1, align 8
  %cmpeqtmp91 = icmp eq i64 %"%scancode.load90", 30
  br i1 %cmpeqtmp91, label %then_116, label %merge_117

merge_117:                                        ; preds = %merge_115
  %"%scancode.load93" = load i64, ptr %scancode1, align 8
  %cmpeqtmp94 = icmp eq i64 %"%scancode.load93", 31
  br i1 %cmpeqtmp94, label %then_118, label %merge_119

merge_119:                                        ; preds = %merge_117
  %"%scancode.load96" = load i64, ptr %scancode1, align 8
  %cmpeqtmp97 = icmp eq i64 %"%scancode.load96", 32
  br i1 %cmpeqtmp97, label %then_120, label %merge_121

merge_121:                                        ; preds = %merge_119
  %"%scancode.load99" = load i64, ptr %scancode1, align 8
  %cmpeqtmp100 = icmp eq i64 %"%scancode.load99", 33
  br i1 %cmpeqtmp100, label %then_122, label %merge_123

merge_123:                                        ; preds = %merge_121
  %"%scancode.load102" = load i64, ptr %scancode1, align 8
  %cmpeqtmp103 = icmp eq i64 %"%scancode.load102", 34
  br i1 %cmpeqtmp103, label %then_124, label %merge_125

merge_125:                                        ; preds = %merge_123
  %"%scancode.load105" = load i64, ptr %scancode1, align 8
  %cmpeqtmp106 = icmp eq i64 %"%scancode.load105", 35
  br i1 %cmpeqtmp106, label %then_126, label %merge_127

merge_127:                                        ; preds = %merge_125
  %"%scancode.load108" = load i64, ptr %scancode1, align 8
  %cmpeqtmp109 = icmp eq i64 %"%scancode.load108", 36
  br i1 %cmpeqtmp109, label %then_128, label %merge_129

merge_129:                                        ; preds = %merge_127
  %"%scancode.load111" = load i64, ptr %scancode1, align 8
  %cmpeqtmp112 = icmp eq i64 %"%scancode.load111", 37
  br i1 %cmpeqtmp112, label %then_130, label %merge_131

merge_131:                                        ; preds = %merge_129
  %"%scancode.load114" = load i64, ptr %scancode1, align 8
  %cmpeqtmp115 = icmp eq i64 %"%scancode.load114", 38
  br i1 %cmpeqtmp115, label %then_132, label %merge_133

merge_133:                                        ; preds = %merge_131
  %"%scancode.load117" = load i64, ptr %scancode1, align 8
  %cmpeqtmp118 = icmp eq i64 %"%scancode.load117", 39
  br i1 %cmpeqtmp118, label %then_134, label %merge_135

merge_135:                                        ; preds = %merge_133
  %"%scancode.load120" = load i64, ptr %scancode1, align 8
  %cmpeqtmp121 = icmp eq i64 %"%scancode.load120", 40
  br i1 %cmpeqtmp121, label %then_136, label %merge_137

merge_137:                                        ; preds = %merge_135
  %"%scancode.load123" = load i64, ptr %scancode1, align 8
  %cmpeqtmp124 = icmp eq i64 %"%scancode.load123", 41
  br i1 %cmpeqtmp124, label %then_138, label %merge_139

merge_139:                                        ; preds = %merge_137
  %"%scancode.load126" = load i64, ptr %scancode1, align 8
  %cmpeqtmp127 = icmp eq i64 %"%scancode.load126", 42
  br i1 %cmpeqtmp127, label %then_140, label %merge_141

merge_141:                                        ; preds = %merge_139
  %"%scancode.load129" = load i64, ptr %scancode1, align 8
  %cmpeqtmp130 = icmp eq i64 %"%scancode.load129", 43
  br i1 %cmpeqtmp130, label %then_142, label %merge_143

merge_143:                                        ; preds = %merge_141
  %"%scancode.load132" = load i64, ptr %scancode1, align 8
  %cmpeqtmp133 = icmp eq i64 %"%scancode.load132", 44
  br i1 %cmpeqtmp133, label %then_144, label %merge_145

merge_145:                                        ; preds = %merge_143
  %"%scancode.load135" = load i64, ptr %scancode1, align 8
  %cmpeqtmp136 = icmp eq i64 %"%scancode.load135", 45
  br i1 %cmpeqtmp136, label %then_146, label %merge_147

merge_147:                                        ; preds = %merge_145
  %"%scancode.load138" = load i64, ptr %scancode1, align 8
  %cmpeqtmp139 = icmp eq i64 %"%scancode.load138", 46
  br i1 %cmpeqtmp139, label %then_148, label %merge_149

merge_149:                                        ; preds = %merge_147
  %"%scancode.load141" = load i64, ptr %scancode1, align 8
  %cmpeqtmp142 = icmp eq i64 %"%scancode.load141", 47
  br i1 %cmpeqtmp142, label %then_150, label %merge_151

merge_151:                                        ; preds = %merge_149
  %"%scancode.load144" = load i64, ptr %scancode1, align 8
  %cmpeqtmp145 = icmp eq i64 %"%scancode.load144", 48
  br i1 %cmpeqtmp145, label %then_152, label %merge_153

merge_153:                                        ; preds = %merge_151
  %"%scancode.load147" = load i64, ptr %scancode1, align 8
  %cmpeqtmp148 = icmp eq i64 %"%scancode.load147", 49
  br i1 %cmpeqtmp148, label %then_154, label %merge_155

merge_155:                                        ; preds = %merge_153
  %"%scancode.load150" = load i64, ptr %scancode1, align 8
  %cmpeqtmp151 = icmp eq i64 %"%scancode.load150", 50
  br i1 %cmpeqtmp151, label %then_156, label %merge_157

merge_157:                                        ; preds = %merge_155
  %"%scancode.load153" = load i64, ptr %scancode1, align 8
  %cmpeqtmp154 = icmp eq i64 %"%scancode.load153", 51
  br i1 %cmpeqtmp154, label %then_158, label %merge_159

merge_159:                                        ; preds = %merge_157
  %"%scancode.load156" = load i64, ptr %scancode1, align 8
  %cmpeqtmp157 = icmp eq i64 %"%scancode.load156", 52
  br i1 %cmpeqtmp157, label %then_160, label %merge_161

merge_161:                                        ; preds = %merge_159
  %"%scancode.load159" = load i64, ptr %scancode1, align 8
  %cmpeqtmp160 = icmp eq i64 %"%scancode.load159", 53
  br i1 %cmpeqtmp160, label %then_162, label %merge_163

merge_163:                                        ; preds = %merge_161
  %"%scancode.load162" = load i64, ptr %scancode1, align 8
  %cmpeqtmp163 = icmp eq i64 %"%scancode.load162", 54
  br i1 %cmpeqtmp163, label %then_164, label %merge_165

merge_165:                                        ; preds = %merge_163
  %"%scancode.load165" = load i64, ptr %scancode1, align 8
  %cmpeqtmp166 = icmp eq i64 %"%scancode.load165", 55
  br i1 %cmpeqtmp166, label %then_166, label %merge_167

merge_167:                                        ; preds = %merge_165
  %"%scancode.load168" = load i64, ptr %scancode1, align 8
  %cmpeqtmp169 = icmp eq i64 %"%scancode.load168", 56
  br i1 %cmpeqtmp169, label %then_168, label %merge_169

merge_169:                                        ; preds = %merge_167
  %"%scancode.load171" = load i64, ptr %scancode1, align 8
  %cmpeqtmp172 = icmp eq i64 %"%scancode.load171", 57
  br i1 %cmpeqtmp172, label %then_170, label %merge_171

merge_171:                                        ; preds = %merge_169
  %"%scancode.load174" = load i64, ptr %scancode1, align 8
  %cmpeqtmp175 = icmp eq i64 %"%scancode.load174", 58
  br i1 %cmpeqtmp175, label %then_172, label %merge_173

merge_173:                                        ; preds = %merge_171
  %"%scancode.load177" = load i64, ptr %scancode1, align 8
  %cmpeqtmp178 = icmp eq i64 %"%scancode.load177", 59
  br i1 %cmpeqtmp178, label %then_174, label %merge_175

merge_175:                                        ; preds = %merge_173
  %"%scancode.load180" = load i64, ptr %scancode1, align 8
  %cmpeqtmp181 = icmp eq i64 %"%scancode.load180", 60
  br i1 %cmpeqtmp181, label %then_176, label %merge_177

merge_177:                                        ; preds = %merge_175
  %"%scancode.load183" = load i64, ptr %scancode1, align 8
  %cmpeqtmp184 = icmp eq i64 %"%scancode.load183", 61
  br i1 %cmpeqtmp184, label %then_178, label %merge_179

merge_179:                                        ; preds = %merge_177
  %"%scancode.load186" = load i64, ptr %scancode1, align 8
  %cmpeqtmp187 = icmp eq i64 %"%scancode.load186", 62
  br i1 %cmpeqtmp187, label %then_180, label %merge_181

merge_181:                                        ; preds = %merge_179
  %"%scancode.load189" = load i64, ptr %scancode1, align 8
  %cmpeqtmp190 = icmp eq i64 %"%scancode.load189", 63
  br i1 %cmpeqtmp190, label %then_182, label %merge_183

merge_183:                                        ; preds = %merge_181
  %"%scancode.load192" = load i64, ptr %scancode1, align 8
  %cmpeqtmp193 = icmp eq i64 %"%scancode.load192", 64
  br i1 %cmpeqtmp193, label %then_184, label %merge_185

merge_185:                                        ; preds = %merge_183
  %"%scancode.load195" = load i64, ptr %scancode1, align 8
  %cmpeqtmp196 = icmp eq i64 %"%scancode.load195", 65
  br i1 %cmpeqtmp196, label %then_186, label %merge_187

merge_187:                                        ; preds = %merge_185
  %"%scancode.load198" = load i64, ptr %scancode1, align 8
  %cmpeqtmp199 = icmp eq i64 %"%scancode.load198", 66
  br i1 %cmpeqtmp199, label %then_188, label %merge_189

merge_189:                                        ; preds = %merge_187
  %"%scancode.load201" = load i64, ptr %scancode1, align 8
  %cmpeqtmp202 = icmp eq i64 %"%scancode.load201", 67
  br i1 %cmpeqtmp202, label %then_190, label %merge_191

merge_191:                                        ; preds = %merge_189
  %"%scancode.load204" = load i64, ptr %scancode1, align 8
  %cmpeqtmp205 = icmp eq i64 %"%scancode.load204", 68
  br i1 %cmpeqtmp205, label %then_192, label %merge_193

merge_193:                                        ; preds = %merge_191
  %"%scancode.load207" = load i64, ptr %scancode1, align 8
  %cmpeqtmp208 = icmp eq i64 %"%scancode.load207", 69
  br i1 %cmpeqtmp208, label %then_194, label %merge_195

merge_195:                                        ; preds = %merge_193
  %"%scancode.load210" = load i64, ptr %scancode1, align 8
  %cmpeqtmp211 = icmp eq i64 %"%scancode.load210", 70
  br i1 %cmpeqtmp211, label %then_196, label %merge_197

merge_197:                                        ; preds = %merge_195
  %"%scancode.load213" = load i64, ptr %scancode1, align 8
  %cmpeqtmp214 = icmp eq i64 %"%scancode.load213", 71
  br i1 %cmpeqtmp214, label %then_198, label %merge_199

merge_199:                                        ; preds = %merge_197
  %"%scancode.load216" = load i64, ptr %scancode1, align 8
  %cmpeqtmp217 = icmp eq i64 %"%scancode.load216", 72
  br i1 %cmpeqtmp217, label %then_200, label %merge_201

merge_201:                                        ; preds = %merge_199
  %"%scancode.load219" = load i64, ptr %scancode1, align 8
  %cmpeqtmp220 = icmp eq i64 %"%scancode.load219", 73
  br i1 %cmpeqtmp220, label %then_202, label %merge_203

merge_203:                                        ; preds = %merge_201
  %"%scancode.load222" = load i64, ptr %scancode1, align 8
  %cmpeqtmp223 = icmp eq i64 %"%scancode.load222", 74
  br i1 %cmpeqtmp223, label %then_204, label %merge_205

merge_205:                                        ; preds = %merge_203
  %"%scancode.load225" = load i64, ptr %scancode1, align 8
  %cmpeqtmp226 = icmp eq i64 %"%scancode.load225", 75
  br i1 %cmpeqtmp226, label %then_206, label %merge_207

merge_207:                                        ; preds = %merge_205
  %"%scancode.load228" = load i64, ptr %scancode1, align 8
  %cmpeqtmp229 = icmp eq i64 %"%scancode.load228", 76
  br i1 %cmpeqtmp229, label %then_208, label %merge_209

merge_209:                                        ; preds = %merge_207
  %"%scancode.load231" = load i64, ptr %scancode1, align 8
  %cmpeqtmp232 = icmp eq i64 %"%scancode.load231", 77
  br i1 %cmpeqtmp232, label %then_210, label %merge_211

merge_211:                                        ; preds = %merge_209
  %"%scancode.load234" = load i64, ptr %scancode1, align 8
  %cmpeqtmp235 = icmp eq i64 %"%scancode.load234", 78
  br i1 %cmpeqtmp235, label %then_212, label %merge_213

merge_213:                                        ; preds = %merge_211
  %"%scancode.load237" = load i64, ptr %scancode1, align 8
  %cmpeqtmp238 = icmp eq i64 %"%scancode.load237", 79
  br i1 %cmpeqtmp238, label %then_214, label %merge_215

merge_215:                                        ; preds = %merge_213
  %"%scancode.load240" = load i64, ptr %scancode1, align 8
  %cmpeqtmp241 = icmp eq i64 %"%scancode.load240", 80
  br i1 %cmpeqtmp241, label %then_216, label %merge_217

merge_217:                                        ; preds = %merge_215
  %"%scancode.load243" = load i64, ptr %scancode1, align 8
  %cmpeqtmp244 = icmp eq i64 %"%scancode.load243", 81
  br i1 %cmpeqtmp244, label %then_218, label %merge_219

merge_219:                                        ; preds = %merge_217
  %"%scancode.load246" = load i64, ptr %scancode1, align 8
  %cmpeqtmp247 = icmp eq i64 %"%scancode.load246", 82
  br i1 %cmpeqtmp247, label %then_220, label %merge_221

merge_221:                                        ; preds = %merge_219
  %"%scancode.load249" = load i64, ptr %scancode1, align 8
  %cmpeqtmp250 = icmp eq i64 %"%scancode.load249", 83
  br i1 %cmpeqtmp250, label %then_222, label %merge_223

merge_223:                                        ; preds = %merge_221
  %"%scancode.load252" = load i64, ptr %scancode1, align 8
  %cmpeqtmp253 = icmp eq i64 %"%scancode.load252", 84
  br i1 %cmpeqtmp253, label %then_224, label %merge_225

merge_225:                                        ; preds = %merge_223
  %"%scancode.load255" = load i64, ptr %scancode1, align 8
  %cmpeqtmp256 = icmp eq i64 %"%scancode.load255", 85
  br i1 %cmpeqtmp256, label %then_226, label %merge_227

merge_227:                                        ; preds = %merge_225
  %"%scancode.load258" = load i64, ptr %scancode1, align 8
  %cmpeqtmp259 = icmp eq i64 %"%scancode.load258", 86
  br i1 %cmpeqtmp259, label %then_228, label %merge_229

merge_229:                                        ; preds = %merge_227
  %"%scancode.load261" = load i64, ptr %scancode1, align 8
  %cmpeqtmp262 = icmp eq i64 %"%scancode.load261", 87
  br i1 %cmpeqtmp262, label %then_230, label %merge_231

merge_231:                                        ; preds = %merge_229
  %"%scancode.load264" = load i64, ptr %scancode1, align 8
  %cmpeqtmp265 = icmp eq i64 %"%scancode.load264", 88
  br i1 %cmpeqtmp265, label %then_232, label %merge_233

merge_233:                                        ; preds = %merge_231
  %"%scancode.load267" = load i64, ptr %scancode1, align 8
  %cmpeqtmp268 = icmp eq i64 %"%scancode.load267", 89
  br i1 %cmpeqtmp268, label %then_234, label %merge_235

merge_235:                                        ; preds = %merge_233
  %"%scancode.load270" = load i64, ptr %scancode1, align 8
  %cmpeqtmp271 = icmp eq i64 %"%scancode.load270", 90
  br i1 %cmpeqtmp271, label %then_236, label %merge_237

merge_237:                                        ; preds = %merge_235
  %"%scancode.load273" = load i64, ptr %scancode1, align 8
  %cmpeqtmp274 = icmp eq i64 %"%scancode.load273", 91
  br i1 %cmpeqtmp274, label %then_238, label %merge_239

merge_239:                                        ; preds = %merge_237
  %"%scancode.load276" = load i64, ptr %scancode1, align 8
  %cmpeqtmp277 = icmp eq i64 %"%scancode.load276", 92
  br i1 %cmpeqtmp277, label %then_240, label %merge_241

merge_241:                                        ; preds = %merge_239
  %"%scancode.load279" = load i64, ptr %scancode1, align 8
  %cmpeqtmp280 = icmp eq i64 %"%scancode.load279", 93
  br i1 %cmpeqtmp280, label %then_242, label %merge_243

merge_243:                                        ; preds = %merge_241
  %"%scancode.load282" = load i64, ptr %scancode1, align 8
  %cmpeqtmp283 = icmp eq i64 %"%scancode.load282", 94
  br i1 %cmpeqtmp283, label %then_244, label %merge_245

merge_245:                                        ; preds = %merge_243
  %"%scancode.load285" = load i64, ptr %scancode1, align 8
  %cmpeqtmp286 = icmp eq i64 %"%scancode.load285", 95
  br i1 %cmpeqtmp286, label %then_246, label %merge_247

merge_247:                                        ; preds = %merge_245
  %"%scancode.load288" = load i64, ptr %scancode1, align 8
  %cmpeqtmp289 = icmp eq i64 %"%scancode.load288", 96
  br i1 %cmpeqtmp289, label %then_248, label %merge_249

merge_249:                                        ; preds = %merge_247
  %"%scancode.load291" = load i64, ptr %scancode1, align 8
  %cmpeqtmp292 = icmp eq i64 %"%scancode.load291", 97
  br i1 %cmpeqtmp292, label %then_250, label %merge_251

merge_251:                                        ; preds = %merge_249
  %"%scancode.load294" = load i64, ptr %scancode1, align 8
  %cmpeqtmp295 = icmp eq i64 %"%scancode.load294", 98
  br i1 %cmpeqtmp295, label %then_252, label %merge_253

merge_253:                                        ; preds = %merge_251
  %"%scancode.load297" = load i64, ptr %scancode1, align 8
  %cmpeqtmp298 = icmp eq i64 %"%scancode.load297", 99
  br i1 %cmpeqtmp298, label %then_254, label %merge_255

merge_255:                                        ; preds = %merge_253
  %"%scancode.load300" = load i64, ptr %scancode1, align 8
  %cmpeqtmp301 = icmp eq i64 %"%scancode.load300", 100
  br i1 %cmpeqtmp301, label %then_256, label %merge_257

merge_257:                                        ; preds = %merge_255
  %"%scancode.load303" = load i64, ptr %scancode1, align 8
  %cmpeqtmp304 = icmp eq i64 %"%scancode.load303", 101
  br i1 %cmpeqtmp304, label %then_258, label %merge_259

merge_259:                                        ; preds = %merge_257
  %"%scancode.load306" = load i64, ptr %scancode1, align 8
  %cmpeqtmp307 = icmp eq i64 %"%scancode.load306", 102
  br i1 %cmpeqtmp307, label %then_260, label %merge_261

merge_261:                                        ; preds = %merge_259
  %"%scancode.load309" = load i64, ptr %scancode1, align 8
  %cmpeqtmp310 = icmp eq i64 %"%scancode.load309", 103
  br i1 %cmpeqtmp310, label %then_262, label %merge_263

merge_263:                                        ; preds = %merge_261
  %"%scancode.load312" = load i64, ptr %scancode1, align 8
  %cmpeqtmp313 = icmp eq i64 %"%scancode.load312", 104
  br i1 %cmpeqtmp313, label %then_264, label %merge_265

merge_265:                                        ; preds = %merge_263
  %"%scancode.load315" = load i64, ptr %scancode1, align 8
  %cmpeqtmp316 = icmp eq i64 %"%scancode.load315", 105
  br i1 %cmpeqtmp316, label %then_266, label %merge_267

merge_267:                                        ; preds = %merge_265
  %"%scancode.load318" = load i64, ptr %scancode1, align 8
  %cmpeqtmp319 = icmp eq i64 %"%scancode.load318", 106
  br i1 %cmpeqtmp319, label %then_268, label %merge_269

merge_269:                                        ; preds = %merge_267
  %"%scancode.load321" = load i64, ptr %scancode1, align 8
  %cmpeqtmp322 = icmp eq i64 %"%scancode.load321", 107
  br i1 %cmpeqtmp322, label %then_270, label %merge_271

merge_271:                                        ; preds = %merge_269
  %"%scancode.load324" = load i64, ptr %scancode1, align 8
  %cmpeqtmp325 = icmp eq i64 %"%scancode.load324", 108
  br i1 %cmpeqtmp325, label %then_272, label %merge_273

merge_273:                                        ; preds = %merge_271
  %"%scancode.load327" = load i64, ptr %scancode1, align 8
  %cmpeqtmp328 = icmp eq i64 %"%scancode.load327", 109
  br i1 %cmpeqtmp328, label %then_274, label %merge_275

merge_275:                                        ; preds = %merge_273
  %"%scancode.load330" = load i64, ptr %scancode1, align 8
  %cmpeqtmp331 = icmp eq i64 %"%scancode.load330", 110
  br i1 %cmpeqtmp331, label %then_276, label %merge_277

merge_277:                                        ; preds = %merge_275
  %"%scancode.load333" = load i64, ptr %scancode1, align 8
  %cmpeqtmp334 = icmp eq i64 %"%scancode.load333", 111
  br i1 %cmpeqtmp334, label %then_278, label %merge_279

merge_279:                                        ; preds = %merge_277
  %"%scancode.load336" = load i64, ptr %scancode1, align 8
  %cmpeqtmp337 = icmp eq i64 %"%scancode.load336", 112
  br i1 %cmpeqtmp337, label %then_280, label %merge_281

merge_281:                                        ; preds = %merge_279
  %"%scancode.load339" = load i64, ptr %scancode1, align 8
  %cmpeqtmp340 = icmp eq i64 %"%scancode.load339", 113
  br i1 %cmpeqtmp340, label %then_282, label %merge_283

merge_283:                                        ; preds = %merge_281
  %"%scancode.load342" = load i64, ptr %scancode1, align 8
  %cmpeqtmp343 = icmp eq i64 %"%scancode.load342", 114
  br i1 %cmpeqtmp343, label %then_284, label %merge_285

merge_285:                                        ; preds = %merge_283
  %"%scancode.load345" = load i64, ptr %scancode1, align 8
  %cmpeqtmp346 = icmp eq i64 %"%scancode.load345", 115
  br i1 %cmpeqtmp346, label %then_286, label %merge_287

merge_287:                                        ; preds = %merge_285
  %"%scancode.load348" = load i64, ptr %scancode1, align 8
  %cmpeqtmp349 = icmp eq i64 %"%scancode.load348", 116
  br i1 %cmpeqtmp349, label %then_288, label %merge_289

merge_289:                                        ; preds = %merge_287
  %"%scancode.load351" = load i64, ptr %scancode1, align 8
  %cmpeqtmp352 = icmp eq i64 %"%scancode.load351", 117
  br i1 %cmpeqtmp352, label %then_290, label %merge_291

merge_291:                                        ; preds = %merge_289
  %"%scancode.load354" = load i64, ptr %scancode1, align 8
  %cmpeqtmp355 = icmp eq i64 %"%scancode.load354", 118
  br i1 %cmpeqtmp355, label %then_292, label %merge_293

merge_293:                                        ; preds = %merge_291
  %"%scancode.load357" = load i64, ptr %scancode1, align 8
  %cmpeqtmp358 = icmp eq i64 %"%scancode.load357", 119
  br i1 %cmpeqtmp358, label %then_294, label %merge_295

merge_295:                                        ; preds = %merge_293
  %"%scancode.load360" = load i64, ptr %scancode1, align 8
  %cmpeqtmp361 = icmp eq i64 %"%scancode.load360", 120
  br i1 %cmpeqtmp361, label %then_296, label %merge_297

merge_297:                                        ; preds = %merge_295
  %"%scancode.load363" = load i64, ptr %scancode1, align 8
  %cmpeqtmp364 = icmp eq i64 %"%scancode.load363", 121
  br i1 %cmpeqtmp364, label %then_298, label %merge_299

merge_299:                                        ; preds = %merge_297
  %"%scancode.load366" = load i64, ptr %scancode1, align 8
  %cmpeqtmp367 = icmp eq i64 %"%scancode.load366", 122
  br i1 %cmpeqtmp367, label %then_300, label %merge_301

merge_301:                                        ; preds = %merge_299
  %"%scancode.load369" = load i64, ptr %scancode1, align 8
  %cmpeqtmp370 = icmp eq i64 %"%scancode.load369", 123
  br i1 %cmpeqtmp370, label %then_302, label %merge_303

merge_303:                                        ; preds = %merge_301
  %"%scancode.load372" = load i64, ptr %scancode1, align 8
  %cmpeqtmp373 = icmp eq i64 %"%scancode.load372", 124
  br i1 %cmpeqtmp373, label %then_304, label %merge_305

merge_305:                                        ; preds = %merge_303
  %"%scancode.load375" = load i64, ptr %scancode1, align 8
  %cmpeqtmp376 = icmp eq i64 %"%scancode.load375", 125
  br i1 %cmpeqtmp376, label %then_306, label %merge_307

merge_307:                                        ; preds = %merge_305
  %"%scancode.load378" = load i64, ptr %scancode1, align 8
  %cmpeqtmp379 = icmp eq i64 %"%scancode.load378", 126
  br i1 %cmpeqtmp379, label %then_308, label %merge_309

merge_309:                                        ; preds = %merge_307
  %"%scancode.load381" = load i64, ptr %scancode1, align 8
  %cmpeqtmp382 = icmp eq i64 %"%scancode.load381", 127
  br i1 %cmpeqtmp382, label %then_310, label %merge_311

merge_311:                                        ; preds = %merge_309
  ret i64 0

merge_55:                                         ; preds = %entry
  %"%scancode.load2" = load i64, ptr %scancode1, align 8
  %cmpeqtmp = icmp eq i64 %"%scancode.load2", 0
  br i1 %cmpeqtmp, label %then_56, label %merge_57

merge_57:                                         ; preds = %merge_55
  %"%scancode.load3" = load i64, ptr %scancode1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%scancode.load3", 1
  br i1 %cmpeqtmp4, label %then_58, label %merge_59

merge_59:                                         ; preds = %merge_57
  %"%scancode.load6" = load i64, ptr %scancode1, align 8
  %cmpeqtmp7 = icmp eq i64 %"%scancode.load6", 2
  br i1 %cmpeqtmp7, label %then_60, label %merge_61

merge_61:                                         ; preds = %merge_59
  %"%scancode.load9" = load i64, ptr %scancode1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%scancode.load9", 3
  br i1 %cmpeqtmp10, label %then_62, label %merge_63

merge_63:                                         ; preds = %merge_61
  %"%scancode.load12" = load i64, ptr %scancode1, align 8
  %cmpeqtmp13 = icmp eq i64 %"%scancode.load12", 4
  br i1 %cmpeqtmp13, label %then_64, label %merge_65

merge_65:                                         ; preds = %merge_63
  %"%scancode.load15" = load i64, ptr %scancode1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%scancode.load15", 5
  br i1 %cmpeqtmp16, label %then_66, label %merge_67

merge_67:                                         ; preds = %merge_65
  %"%scancode.load18" = load i64, ptr %scancode1, align 8
  %cmpeqtmp19 = icmp eq i64 %"%scancode.load18", 6
  br i1 %cmpeqtmp19, label %then_68, label %merge_69

merge_69:                                         ; preds = %merge_67
  %"%scancode.load21" = load i64, ptr %scancode1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%scancode.load21", 7
  br i1 %cmpeqtmp22, label %then_70, label %merge_71

merge_71:                                         ; preds = %merge_69
  %"%scancode.load24" = load i64, ptr %scancode1, align 8
  %cmpeqtmp25 = icmp eq i64 %"%scancode.load24", 8
  br i1 %cmpeqtmp25, label %then_72, label %merge_73

merge_73:                                         ; preds = %merge_71
  %"%scancode.load27" = load i64, ptr %scancode1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%scancode.load27", 9
  br i1 %cmpeqtmp28, label %then_74, label %merge_75

merge_75:                                         ; preds = %merge_73
  %"%scancode.load30" = load i64, ptr %scancode1, align 8
  %cmpeqtmp31 = icmp eq i64 %"%scancode.load30", 10
  br i1 %cmpeqtmp31, label %then_76, label %merge_77

merge_77:                                         ; preds = %merge_75
  %"%scancode.load33" = load i64, ptr %scancode1, align 8
  %cmpeqtmp34 = icmp eq i64 %"%scancode.load33", 11
  br i1 %cmpeqtmp34, label %then_78, label %merge_79

merge_79:                                         ; preds = %merge_77
  %"%scancode.load36" = load i64, ptr %scancode1, align 8
  %cmpeqtmp37 = icmp eq i64 %"%scancode.load36", 12
  br i1 %cmpeqtmp37, label %then_80, label %merge_81

merge_81:                                         ; preds = %merge_79
  %"%scancode.load39" = load i64, ptr %scancode1, align 8
  %cmpeqtmp40 = icmp eq i64 %"%scancode.load39", 13
  br i1 %cmpeqtmp40, label %then_82, label %merge_83

merge_83:                                         ; preds = %merge_81
  %"%scancode.load42" = load i64, ptr %scancode1, align 8
  %cmpeqtmp43 = icmp eq i64 %"%scancode.load42", 14
  br i1 %cmpeqtmp43, label %then_84, label %merge_85

merge_85:                                         ; preds = %merge_83
  %"%scancode.load45" = load i64, ptr %scancode1, align 8
  %cmpeqtmp46 = icmp eq i64 %"%scancode.load45", 15
  br i1 %cmpeqtmp46, label %then_86, label %merge_87

merge_87:                                         ; preds = %merge_85
  %"%scancode.load48" = load i64, ptr %scancode1, align 8
  %cmpeqtmp49 = icmp eq i64 %"%scancode.load48", 16
  br i1 %cmpeqtmp49, label %then_88, label %merge_89

merge_89:                                         ; preds = %merge_87
  %"%scancode.load51" = load i64, ptr %scancode1, align 8
  %cmpeqtmp52 = icmp eq i64 %"%scancode.load51", 17
  br i1 %cmpeqtmp52, label %then_90, label %merge_91

merge_91:                                         ; preds = %merge_89
  %"%scancode.load54" = load i64, ptr %scancode1, align 8
  %cmpeqtmp55 = icmp eq i64 %"%scancode.load54", 18
  br i1 %cmpeqtmp55, label %then_92, label %merge_93

merge_93:                                         ; preds = %merge_91
  %"%scancode.load57" = load i64, ptr %scancode1, align 8
  %cmpeqtmp58 = icmp eq i64 %"%scancode.load57", 19
  br i1 %cmpeqtmp58, label %then_94, label %merge_95

merge_95:                                         ; preds = %merge_93
  %"%scancode.load60" = load i64, ptr %scancode1, align 8
  %cmpeqtmp61 = icmp eq i64 %"%scancode.load60", 20
  br i1 %cmpeqtmp61, label %then_96, label %merge_97

merge_97:                                         ; preds = %merge_95
  %"%scancode.load63" = load i64, ptr %scancode1, align 8
  %cmpeqtmp64 = icmp eq i64 %"%scancode.load63", 21
  br i1 %cmpeqtmp64, label %then_98, label %merge_99

merge_99:                                         ; preds = %merge_97
  %"%scancode.load66" = load i64, ptr %scancode1, align 8
  %cmpeqtmp67 = icmp eq i64 %"%scancode.load66", 22
  br i1 %cmpeqtmp67, label %then_100, label %merge_101

then_100:                                         ; preds = %merge_99
  %loadtmp68 = load i64, ptr @sc2_ascii_22, align 8
  ret i64 %loadtmp68

then_102:                                         ; preds = %merge_101
  %loadtmp71 = load i64, ptr @sc2_ascii_23, align 8
  ret i64 %loadtmp71

then_104:                                         ; preds = %merge_103
  %loadtmp74 = load i64, ptr @sc2_ascii_24, align 8
  ret i64 %loadtmp74

then_106:                                         ; preds = %merge_105
  %loadtmp77 = load i64, ptr @sc2_ascii_25, align 8
  ret i64 %loadtmp77

then_108:                                         ; preds = %merge_107
  %loadtmp80 = load i64, ptr @sc2_ascii_26, align 8
  ret i64 %loadtmp80

then_110:                                         ; preds = %merge_109
  %loadtmp83 = load i64, ptr @sc2_ascii_27, align 8
  ret i64 %loadtmp83

then_112:                                         ; preds = %merge_111
  %loadtmp86 = load i64, ptr @sc2_ascii_28, align 8
  ret i64 %loadtmp86

then_114:                                         ; preds = %merge_113
  %loadtmp89 = load i64, ptr @sc2_ascii_29, align 8
  ret i64 %loadtmp89

then_116:                                         ; preds = %merge_115
  %loadtmp92 = load i64, ptr @sc2_ascii_30, align 8
  ret i64 %loadtmp92

then_118:                                         ; preds = %merge_117
  %loadtmp95 = load i64, ptr @sc2_ascii_31, align 8
  ret i64 %loadtmp95

then_120:                                         ; preds = %merge_119
  %loadtmp98 = load i64, ptr @sc2_ascii_32, align 8
  ret i64 %loadtmp98

then_122:                                         ; preds = %merge_121
  %loadtmp101 = load i64, ptr @sc2_ascii_33, align 8
  ret i64 %loadtmp101

then_124:                                         ; preds = %merge_123
  %loadtmp104 = load i64, ptr @sc2_ascii_34, align 8
  ret i64 %loadtmp104

then_126:                                         ; preds = %merge_125
  %loadtmp107 = load i64, ptr @sc2_ascii_35, align 8
  ret i64 %loadtmp107

then_128:                                         ; preds = %merge_127
  %loadtmp110 = load i64, ptr @sc2_ascii_36, align 8
  ret i64 %loadtmp110

then_130:                                         ; preds = %merge_129
  %loadtmp113 = load i64, ptr @sc2_ascii_37, align 8
  ret i64 %loadtmp113

then_132:                                         ; preds = %merge_131
  %loadtmp116 = load i64, ptr @sc2_ascii_38, align 8
  ret i64 %loadtmp116

then_134:                                         ; preds = %merge_133
  %loadtmp119 = load i64, ptr @sc2_ascii_39, align 8
  ret i64 %loadtmp119

then_136:                                         ; preds = %merge_135
  %loadtmp122 = load i64, ptr @sc2_ascii_40, align 8
  ret i64 %loadtmp122

then_138:                                         ; preds = %merge_137
  %loadtmp125 = load i64, ptr @sc2_ascii_41, align 8
  ret i64 %loadtmp125

then_140:                                         ; preds = %merge_139
  %loadtmp128 = load i64, ptr @sc2_ascii_42, align 8
  ret i64 %loadtmp128

then_142:                                         ; preds = %merge_141
  %loadtmp131 = load i64, ptr @sc2_ascii_43, align 8
  ret i64 %loadtmp131

then_144:                                         ; preds = %merge_143
  %loadtmp134 = load i64, ptr @sc2_ascii_44, align 8
  ret i64 %loadtmp134

then_146:                                         ; preds = %merge_145
  %loadtmp137 = load i64, ptr @sc2_ascii_45, align 8
  ret i64 %loadtmp137

then_148:                                         ; preds = %merge_147
  %loadtmp140 = load i64, ptr @sc2_ascii_46, align 8
  ret i64 %loadtmp140

then_150:                                         ; preds = %merge_149
  %loadtmp143 = load i64, ptr @sc2_ascii_47, align 8
  ret i64 %loadtmp143

then_152:                                         ; preds = %merge_151
  %loadtmp146 = load i64, ptr @sc2_ascii_48, align 8
  ret i64 %loadtmp146

then_154:                                         ; preds = %merge_153
  %loadtmp149 = load i64, ptr @sc2_ascii_49, align 8
  ret i64 %loadtmp149

then_156:                                         ; preds = %merge_155
  %loadtmp152 = load i64, ptr @sc2_ascii_50, align 8
  ret i64 %loadtmp152

then_158:                                         ; preds = %merge_157
  %loadtmp155 = load i64, ptr @sc2_ascii_51, align 8
  ret i64 %loadtmp155

then_160:                                         ; preds = %merge_159
  %loadtmp158 = load i64, ptr @sc2_ascii_52, align 8
  ret i64 %loadtmp158

then_162:                                         ; preds = %merge_161
  %loadtmp161 = load i64, ptr @sc2_ascii_53, align 8
  ret i64 %loadtmp161

then_164:                                         ; preds = %merge_163
  %loadtmp164 = load i64, ptr @sc2_ascii_54, align 8
  ret i64 %loadtmp164

then_166:                                         ; preds = %merge_165
  %loadtmp167 = load i64, ptr @sc2_ascii_55, align 8
  ret i64 %loadtmp167

then_168:                                         ; preds = %merge_167
  %loadtmp170 = load i64, ptr @sc2_ascii_56, align 8
  ret i64 %loadtmp170

then_170:                                         ; preds = %merge_169
  %loadtmp173 = load i64, ptr @sc2_ascii_57, align 8
  ret i64 %loadtmp173

then_172:                                         ; preds = %merge_171
  %loadtmp176 = load i64, ptr @sc2_ascii_58, align 8
  ret i64 %loadtmp176

then_174:                                         ; preds = %merge_173
  %loadtmp179 = load i64, ptr @sc2_ascii_59, align 8
  ret i64 %loadtmp179

then_176:                                         ; preds = %merge_175
  %loadtmp182 = load i64, ptr @sc2_ascii_60, align 8
  ret i64 %loadtmp182

then_178:                                         ; preds = %merge_177
  %loadtmp185 = load i64, ptr @sc2_ascii_61, align 8
  ret i64 %loadtmp185

then_180:                                         ; preds = %merge_179
  %loadtmp188 = load i64, ptr @sc2_ascii_62, align 8
  ret i64 %loadtmp188

then_182:                                         ; preds = %merge_181
  %loadtmp191 = load i64, ptr @sc2_ascii_63, align 8
  ret i64 %loadtmp191

then_184:                                         ; preds = %merge_183
  %loadtmp194 = load i64, ptr @sc2_ascii_64, align 8
  ret i64 %loadtmp194

then_186:                                         ; preds = %merge_185
  %loadtmp197 = load i64, ptr @sc2_ascii_65, align 8
  ret i64 %loadtmp197

then_188:                                         ; preds = %merge_187
  %loadtmp200 = load i64, ptr @sc2_ascii_66, align 8
  ret i64 %loadtmp200

then_190:                                         ; preds = %merge_189
  %loadtmp203 = load i64, ptr @sc2_ascii_67, align 8
  ret i64 %loadtmp203

then_192:                                         ; preds = %merge_191
  %loadtmp206 = load i64, ptr @sc2_ascii_68, align 8
  ret i64 %loadtmp206

then_194:                                         ; preds = %merge_193
  %loadtmp209 = load i64, ptr @sc2_ascii_69, align 8
  ret i64 %loadtmp209

then_196:                                         ; preds = %merge_195
  %loadtmp212 = load i64, ptr @sc2_ascii_70, align 8
  ret i64 %loadtmp212

then_198:                                         ; preds = %merge_197
  %loadtmp215 = load i64, ptr @sc2_ascii_71, align 8
  ret i64 %loadtmp215

then_200:                                         ; preds = %merge_199
  %loadtmp218 = load i64, ptr @sc2_ascii_72, align 8
  ret i64 %loadtmp218

then_202:                                         ; preds = %merge_201
  %loadtmp221 = load i64, ptr @sc2_ascii_73, align 8
  ret i64 %loadtmp221

then_204:                                         ; preds = %merge_203
  %loadtmp224 = load i64, ptr @sc2_ascii_74, align 8
  ret i64 %loadtmp224

then_206:                                         ; preds = %merge_205
  %loadtmp227 = load i64, ptr @sc2_ascii_75, align 8
  ret i64 %loadtmp227

then_208:                                         ; preds = %merge_207
  %loadtmp230 = load i64, ptr @sc2_ascii_76, align 8
  ret i64 %loadtmp230

then_210:                                         ; preds = %merge_209
  %loadtmp233 = load i64, ptr @sc2_ascii_77, align 8
  ret i64 %loadtmp233

then_212:                                         ; preds = %merge_211
  %loadtmp236 = load i64, ptr @sc2_ascii_78, align 8
  ret i64 %loadtmp236

then_214:                                         ; preds = %merge_213
  %loadtmp239 = load i64, ptr @sc2_ascii_79, align 8
  ret i64 %loadtmp239

then_216:                                         ; preds = %merge_215
  %loadtmp242 = load i64, ptr @sc2_ascii_80, align 8
  ret i64 %loadtmp242

then_218:                                         ; preds = %merge_217
  %loadtmp245 = load i64, ptr @sc2_ascii_81, align 8
  ret i64 %loadtmp245

then_220:                                         ; preds = %merge_219
  %loadtmp248 = load i64, ptr @sc2_ascii_82, align 8
  ret i64 %loadtmp248

then_222:                                         ; preds = %merge_221
  %loadtmp251 = load i64, ptr @sc2_ascii_83, align 8
  ret i64 %loadtmp251

then_224:                                         ; preds = %merge_223
  %loadtmp254 = load i64, ptr @sc2_ascii_84, align 8
  ret i64 %loadtmp254

then_226:                                         ; preds = %merge_225
  %loadtmp257 = load i64, ptr @sc2_ascii_85, align 8
  ret i64 %loadtmp257

then_228:                                         ; preds = %merge_227
  %loadtmp260 = load i64, ptr @sc2_ascii_86, align 8
  ret i64 %loadtmp260

then_230:                                         ; preds = %merge_229
  %loadtmp263 = load i64, ptr @sc2_ascii_87, align 8
  ret i64 %loadtmp263

then_232:                                         ; preds = %merge_231
  %loadtmp266 = load i64, ptr @sc2_ascii_88, align 8
  ret i64 %loadtmp266

then_234:                                         ; preds = %merge_233
  %loadtmp269 = load i64, ptr @sc2_ascii_89, align 8
  ret i64 %loadtmp269

then_236:                                         ; preds = %merge_235
  %loadtmp272 = load i64, ptr @sc2_ascii_90, align 8
  ret i64 %loadtmp272

then_238:                                         ; preds = %merge_237
  %loadtmp275 = load i64, ptr @sc2_ascii_91, align 8
  ret i64 %loadtmp275

then_240:                                         ; preds = %merge_239
  %loadtmp278 = load i64, ptr @sc2_ascii_92, align 8
  ret i64 %loadtmp278

then_242:                                         ; preds = %merge_241
  %loadtmp281 = load i64, ptr @sc2_ascii_93, align 8
  ret i64 %loadtmp281

then_244:                                         ; preds = %merge_243
  %loadtmp284 = load i64, ptr @sc2_ascii_94, align 8
  ret i64 %loadtmp284

then_246:                                         ; preds = %merge_245
  %loadtmp287 = load i64, ptr @sc2_ascii_95, align 8
  ret i64 %loadtmp287

then_248:                                         ; preds = %merge_247
  %loadtmp290 = load i64, ptr @sc2_ascii_96, align 8
  ret i64 %loadtmp290

then_250:                                         ; preds = %merge_249
  %loadtmp293 = load i64, ptr @sc2_ascii_97, align 8
  ret i64 %loadtmp293

then_252:                                         ; preds = %merge_251
  %loadtmp296 = load i64, ptr @sc2_ascii_98, align 8
  ret i64 %loadtmp296

then_254:                                         ; preds = %merge_253
  %loadtmp299 = load i64, ptr @sc2_ascii_99, align 8
  ret i64 %loadtmp299

then_256:                                         ; preds = %merge_255
  %loadtmp302 = load i64, ptr @sc2_ascii_100, align 8
  ret i64 %loadtmp302

then_258:                                         ; preds = %merge_257
  %loadtmp305 = load i64, ptr @sc2_ascii_101, align 8
  ret i64 %loadtmp305

then_260:                                         ; preds = %merge_259
  %loadtmp308 = load i64, ptr @sc2_ascii_102, align 8
  ret i64 %loadtmp308

then_262:                                         ; preds = %merge_261
  %loadtmp311 = load i64, ptr @sc2_ascii_103, align 8
  ret i64 %loadtmp311

then_264:                                         ; preds = %merge_263
  %loadtmp314 = load i64, ptr @sc2_ascii_104, align 8
  ret i64 %loadtmp314

then_266:                                         ; preds = %merge_265
  %loadtmp317 = load i64, ptr @sc2_ascii_105, align 8
  ret i64 %loadtmp317

then_268:                                         ; preds = %merge_267
  %loadtmp320 = load i64, ptr @sc2_ascii_106, align 8
  ret i64 %loadtmp320

then_270:                                         ; preds = %merge_269
  %loadtmp323 = load i64, ptr @sc2_ascii_107, align 8
  ret i64 %loadtmp323

then_272:                                         ; preds = %merge_271
  %loadtmp326 = load i64, ptr @sc2_ascii_108, align 8
  ret i64 %loadtmp326

then_274:                                         ; preds = %merge_273
  %loadtmp329 = load i64, ptr @sc2_ascii_109, align 8
  ret i64 %loadtmp329

then_276:                                         ; preds = %merge_275
  %loadtmp332 = load i64, ptr @sc2_ascii_110, align 8
  ret i64 %loadtmp332

then_278:                                         ; preds = %merge_277
  %loadtmp335 = load i64, ptr @sc2_ascii_111, align 8
  ret i64 %loadtmp335

then_280:                                         ; preds = %merge_279
  %loadtmp338 = load i64, ptr @sc2_ascii_112, align 8
  ret i64 %loadtmp338

then_282:                                         ; preds = %merge_281
  %loadtmp341 = load i64, ptr @sc2_ascii_113, align 8
  ret i64 %loadtmp341

then_284:                                         ; preds = %merge_283
  %loadtmp344 = load i64, ptr @sc2_ascii_114, align 8
  ret i64 %loadtmp344

then_286:                                         ; preds = %merge_285
  %loadtmp347 = load i64, ptr @sc2_ascii_115, align 8
  ret i64 %loadtmp347

then_288:                                         ; preds = %merge_287
  %loadtmp350 = load i64, ptr @sc2_ascii_116, align 8
  ret i64 %loadtmp350

then_290:                                         ; preds = %merge_289
  %loadtmp353 = load i64, ptr @sc2_ascii_117, align 8
  ret i64 %loadtmp353

then_292:                                         ; preds = %merge_291
  %loadtmp356 = load i64, ptr @sc2_ascii_118, align 8
  ret i64 %loadtmp356

then_294:                                         ; preds = %merge_293
  %loadtmp359 = load i64, ptr @sc2_ascii_119, align 8
  ret i64 %loadtmp359

then_296:                                         ; preds = %merge_295
  %loadtmp362 = load i64, ptr @sc2_ascii_120, align 8
  ret i64 %loadtmp362

then_298:                                         ; preds = %merge_297
  %loadtmp365 = load i64, ptr @sc2_ascii_121, align 8
  ret i64 %loadtmp365

then_300:                                         ; preds = %merge_299
  %loadtmp368 = load i64, ptr @sc2_ascii_122, align 8
  ret i64 %loadtmp368

then_302:                                         ; preds = %merge_301
  %loadtmp371 = load i64, ptr @sc2_ascii_123, align 8
  ret i64 %loadtmp371

then_304:                                         ; preds = %merge_303
  %loadtmp374 = load i64, ptr @sc2_ascii_124, align 8
  ret i64 %loadtmp374

then_306:                                         ; preds = %merge_305
  %loadtmp377 = load i64, ptr @sc2_ascii_125, align 8
  ret i64 %loadtmp377

then_308:                                         ; preds = %merge_307
  %loadtmp380 = load i64, ptr @sc2_ascii_126, align 8
  ret i64 %loadtmp380

then_310:                                         ; preds = %merge_309
  %loadtmp383 = load i64, ptr @sc2_ascii_127, align 8
  ret i64 %loadtmp383

then_54:                                          ; preds = %entry
  ret i64 0

then_56:                                          ; preds = %merge_55
  %loadtmp = load i64, ptr @sc2_ascii_0, align 8
  ret i64 %loadtmp

then_58:                                          ; preds = %merge_57
  %loadtmp5 = load i64, ptr @sc2_ascii_1, align 8
  ret i64 %loadtmp5

then_60:                                          ; preds = %merge_59
  %loadtmp8 = load i64, ptr @sc2_ascii_2, align 8
  ret i64 %loadtmp8

then_62:                                          ; preds = %merge_61
  %loadtmp11 = load i64, ptr @sc2_ascii_3, align 8
  ret i64 %loadtmp11

then_64:                                          ; preds = %merge_63
  %loadtmp14 = load i64, ptr @sc2_ascii_4, align 8
  ret i64 %loadtmp14

then_66:                                          ; preds = %merge_65
  %loadtmp17 = load i64, ptr @sc2_ascii_5, align 8
  ret i64 %loadtmp17

then_68:                                          ; preds = %merge_67
  %loadtmp20 = load i64, ptr @sc2_ascii_6, align 8
  ret i64 %loadtmp20

then_70:                                          ; preds = %merge_69
  %loadtmp23 = load i64, ptr @sc2_ascii_7, align 8
  ret i64 %loadtmp23

then_72:                                          ; preds = %merge_71
  %loadtmp26 = load i64, ptr @sc2_ascii_8, align 8
  ret i64 %loadtmp26

then_74:                                          ; preds = %merge_73
  %loadtmp29 = load i64, ptr @sc2_ascii_9, align 8
  ret i64 %loadtmp29

then_76:                                          ; preds = %merge_75
  %loadtmp32 = load i64, ptr @sc2_ascii_10, align 8
  ret i64 %loadtmp32

then_78:                                          ; preds = %merge_77
  %loadtmp35 = load i64, ptr @sc2_ascii_11, align 8
  ret i64 %loadtmp35

then_80:                                          ; preds = %merge_79
  %loadtmp38 = load i64, ptr @sc2_ascii_12, align 8
  ret i64 %loadtmp38

then_82:                                          ; preds = %merge_81
  %loadtmp41 = load i64, ptr @sc2_ascii_13, align 8
  ret i64 %loadtmp41

then_84:                                          ; preds = %merge_83
  %loadtmp44 = load i64, ptr @sc2_ascii_14, align 8
  ret i64 %loadtmp44

then_86:                                          ; preds = %merge_85
  %loadtmp47 = load i64, ptr @sc2_ascii_15, align 8
  ret i64 %loadtmp47

then_88:                                          ; preds = %merge_87
  %loadtmp50 = load i64, ptr @sc2_ascii_16, align 8
  ret i64 %loadtmp50

then_90:                                          ; preds = %merge_89
  %loadtmp53 = load i64, ptr @sc2_ascii_17, align 8
  ret i64 %loadtmp53

then_92:                                          ; preds = %merge_91
  %loadtmp56 = load i64, ptr @sc2_ascii_18, align 8
  ret i64 %loadtmp56

then_94:                                          ; preds = %merge_93
  %loadtmp59 = load i64, ptr @sc2_ascii_19, align 8
  ret i64 %loadtmp59

then_96:                                          ; preds = %merge_95
  %loadtmp62 = load i64, ptr @sc2_ascii_20, align 8
  ret i64 %loadtmp62

then_98:                                          ; preds = %merge_97
  %loadtmp65 = load i64, ptr @sc2_ascii_21, align 8
  ret i64 %loadtmp65
}

define i64 @ps2_sc2_to_ascii_shift(i64 %scancode) {
entry:
  %scancode1 = alloca i64, align 8
  store i64 %scancode, ptr %scancode1, align 8
  %"%scancode.load" = load i64, ptr %scancode1, align 8
  %cmpeqtmp = icmp eq i64 %"%scancode.load", 14
  br i1 %cmpeqtmp, label %then_312, label %merge_313

merge_313:                                        ; preds = %entry
  %"%scancode.load2" = load i64, ptr %scancode1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%scancode.load2", 21
  br i1 %cmpeqtmp3, label %then_314, label %merge_315

merge_315:                                        ; preds = %merge_313
  %"%scancode.load5" = load i64, ptr %scancode1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%scancode.load5", 22
  br i1 %cmpeqtmp6, label %then_316, label %merge_317

merge_317:                                        ; preds = %merge_315
  %"%scancode.load8" = load i64, ptr %scancode1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%scancode.load8", 26
  br i1 %cmpeqtmp9, label %then_318, label %merge_319

merge_319:                                        ; preds = %merge_317
  %"%scancode.load11" = load i64, ptr %scancode1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%scancode.load11", 27
  br i1 %cmpeqtmp12, label %then_320, label %merge_321

merge_321:                                        ; preds = %merge_319
  %"%scancode.load14" = load i64, ptr %scancode1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%scancode.load14", 28
  br i1 %cmpeqtmp15, label %then_322, label %merge_323

merge_323:                                        ; preds = %merge_321
  %"%scancode.load17" = load i64, ptr %scancode1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%scancode.load17", 29
  br i1 %cmpeqtmp18, label %then_324, label %merge_325

merge_325:                                        ; preds = %merge_323
  %"%scancode.load20" = load i64, ptr %scancode1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%scancode.load20", 30
  br i1 %cmpeqtmp21, label %then_326, label %merge_327

merge_327:                                        ; preds = %merge_325
  %"%scancode.load23" = load i64, ptr %scancode1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%scancode.load23", 33
  br i1 %cmpeqtmp24, label %then_328, label %merge_329

merge_329:                                        ; preds = %merge_327
  %"%scancode.load26" = load i64, ptr %scancode1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%scancode.load26", 34
  br i1 %cmpeqtmp27, label %then_330, label %merge_331

merge_331:                                        ; preds = %merge_329
  %"%scancode.load29" = load i64, ptr %scancode1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%scancode.load29", 35
  br i1 %cmpeqtmp30, label %then_332, label %merge_333

merge_333:                                        ; preds = %merge_331
  %"%scancode.load32" = load i64, ptr %scancode1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%scancode.load32", 36
  br i1 %cmpeqtmp33, label %then_334, label %merge_335

merge_335:                                        ; preds = %merge_333
  %"%scancode.load35" = load i64, ptr %scancode1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%scancode.load35", 37
  br i1 %cmpeqtmp36, label %then_336, label %merge_337

merge_337:                                        ; preds = %merge_335
  %"%scancode.load38" = load i64, ptr %scancode1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%scancode.load38", 38
  br i1 %cmpeqtmp39, label %then_338, label %merge_339

merge_339:                                        ; preds = %merge_337
  %"%scancode.load41" = load i64, ptr %scancode1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%scancode.load41", 42
  br i1 %cmpeqtmp42, label %then_340, label %merge_341

merge_341:                                        ; preds = %merge_339
  %"%scancode.load44" = load i64, ptr %scancode1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%scancode.load44", 43
  br i1 %cmpeqtmp45, label %then_342, label %merge_343

merge_343:                                        ; preds = %merge_341
  %"%scancode.load47" = load i64, ptr %scancode1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%scancode.load47", 44
  br i1 %cmpeqtmp48, label %then_344, label %merge_345

merge_345:                                        ; preds = %merge_343
  %"%scancode.load50" = load i64, ptr %scancode1, align 8
  %cmpeqtmp51 = icmp eq i64 %"%scancode.load50", 45
  br i1 %cmpeqtmp51, label %then_346, label %merge_347

merge_347:                                        ; preds = %merge_345
  %"%scancode.load53" = load i64, ptr %scancode1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%scancode.load53", 46
  br i1 %cmpeqtmp54, label %then_348, label %merge_349

merge_349:                                        ; preds = %merge_347
  %"%scancode.load56" = load i64, ptr %scancode1, align 8
  %cmpeqtmp57 = icmp eq i64 %"%scancode.load56", 49
  br i1 %cmpeqtmp57, label %then_350, label %merge_351

merge_351:                                        ; preds = %merge_349
  %"%scancode.load59" = load i64, ptr %scancode1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%scancode.load59", 50
  br i1 %cmpeqtmp60, label %then_352, label %merge_353

merge_353:                                        ; preds = %merge_351
  %"%scancode.load62" = load i64, ptr %scancode1, align 8
  %cmpeqtmp63 = icmp eq i64 %"%scancode.load62", 51
  br i1 %cmpeqtmp63, label %then_354, label %merge_355

merge_355:                                        ; preds = %merge_353
  %"%scancode.load65" = load i64, ptr %scancode1, align 8
  %cmpeqtmp66 = icmp eq i64 %"%scancode.load65", 52
  br i1 %cmpeqtmp66, label %then_356, label %merge_357

merge_357:                                        ; preds = %merge_355
  %"%scancode.load68" = load i64, ptr %scancode1, align 8
  %cmpeqtmp69 = icmp eq i64 %"%scancode.load68", 53
  br i1 %cmpeqtmp69, label %then_358, label %merge_359

merge_359:                                        ; preds = %merge_357
  %"%scancode.load71" = load i64, ptr %scancode1, align 8
  %cmpeqtmp72 = icmp eq i64 %"%scancode.load71", 54
  br i1 %cmpeqtmp72, label %then_360, label %merge_361

merge_361:                                        ; preds = %merge_359
  %"%scancode.load74" = load i64, ptr %scancode1, align 8
  %cmpeqtmp75 = icmp eq i64 %"%scancode.load74", 58
  br i1 %cmpeqtmp75, label %then_362, label %merge_363

merge_363:                                        ; preds = %merge_361
  %"%scancode.load77" = load i64, ptr %scancode1, align 8
  %cmpeqtmp78 = icmp eq i64 %"%scancode.load77", 59
  br i1 %cmpeqtmp78, label %then_364, label %merge_365

merge_365:                                        ; preds = %merge_363
  %"%scancode.load80" = load i64, ptr %scancode1, align 8
  %cmpeqtmp81 = icmp eq i64 %"%scancode.load80", 60
  br i1 %cmpeqtmp81, label %then_366, label %merge_367

merge_367:                                        ; preds = %merge_365
  %"%scancode.load83" = load i64, ptr %scancode1, align 8
  %cmpeqtmp84 = icmp eq i64 %"%scancode.load83", 61
  br i1 %cmpeqtmp84, label %then_368, label %merge_369

merge_369:                                        ; preds = %merge_367
  %"%scancode.load86" = load i64, ptr %scancode1, align 8
  %cmpeqtmp87 = icmp eq i64 %"%scancode.load86", 62
  br i1 %cmpeqtmp87, label %then_370, label %merge_371

merge_371:                                        ; preds = %merge_369
  %"%scancode.load89" = load i64, ptr %scancode1, align 8
  %cmpeqtmp90 = icmp eq i64 %"%scancode.load89", 65
  br i1 %cmpeqtmp90, label %then_372, label %merge_373

merge_373:                                        ; preds = %merge_371
  %"%scancode.load92" = load i64, ptr %scancode1, align 8
  %cmpeqtmp93 = icmp eq i64 %"%scancode.load92", 66
  br i1 %cmpeqtmp93, label %then_374, label %merge_375

merge_375:                                        ; preds = %merge_373
  %"%scancode.load95" = load i64, ptr %scancode1, align 8
  %cmpeqtmp96 = icmp eq i64 %"%scancode.load95", 67
  br i1 %cmpeqtmp96, label %then_376, label %merge_377

merge_377:                                        ; preds = %merge_375
  %"%scancode.load98" = load i64, ptr %scancode1, align 8
  %cmpeqtmp99 = icmp eq i64 %"%scancode.load98", 68
  br i1 %cmpeqtmp99, label %then_378, label %merge_379

merge_379:                                        ; preds = %merge_377
  %"%scancode.load101" = load i64, ptr %scancode1, align 8
  %cmpeqtmp102 = icmp eq i64 %"%scancode.load101", 69
  br i1 %cmpeqtmp102, label %then_380, label %merge_381

merge_381:                                        ; preds = %merge_379
  %"%scancode.load104" = load i64, ptr %scancode1, align 8
  %cmpeqtmp105 = icmp eq i64 %"%scancode.load104", 70
  br i1 %cmpeqtmp105, label %then_382, label %merge_383

merge_383:                                        ; preds = %merge_381
  %"%scancode.load107" = load i64, ptr %scancode1, align 8
  %cmpeqtmp108 = icmp eq i64 %"%scancode.load107", 73
  br i1 %cmpeqtmp108, label %then_384, label %merge_385

merge_385:                                        ; preds = %merge_383
  %"%scancode.load110" = load i64, ptr %scancode1, align 8
  %cmpeqtmp111 = icmp eq i64 %"%scancode.load110", 74
  br i1 %cmpeqtmp111, label %then_386, label %merge_387

merge_387:                                        ; preds = %merge_385
  %"%scancode.load113" = load i64, ptr %scancode1, align 8
  %cmpeqtmp114 = icmp eq i64 %"%scancode.load113", 75
  br i1 %cmpeqtmp114, label %then_388, label %merge_389

merge_389:                                        ; preds = %merge_387
  %"%scancode.load116" = load i64, ptr %scancode1, align 8
  %cmpeqtmp117 = icmp eq i64 %"%scancode.load116", 76
  br i1 %cmpeqtmp117, label %then_390, label %merge_391

merge_391:                                        ; preds = %merge_389
  %"%scancode.load119" = load i64, ptr %scancode1, align 8
  %cmpeqtmp120 = icmp eq i64 %"%scancode.load119", 77
  br i1 %cmpeqtmp120, label %then_392, label %merge_393

merge_393:                                        ; preds = %merge_391
  %"%scancode.load122" = load i64, ptr %scancode1, align 8
  %cmpeqtmp123 = icmp eq i64 %"%scancode.load122", 78
  br i1 %cmpeqtmp123, label %then_394, label %merge_395

merge_395:                                        ; preds = %merge_393
  %"%scancode.load125" = load i64, ptr %scancode1, align 8
  %cmpeqtmp126 = icmp eq i64 %"%scancode.load125", 82
  br i1 %cmpeqtmp126, label %then_396, label %merge_397

merge_397:                                        ; preds = %merge_395
  %"%scancode.load128" = load i64, ptr %scancode1, align 8
  %cmpeqtmp129 = icmp eq i64 %"%scancode.load128", 84
  br i1 %cmpeqtmp129, label %then_398, label %merge_399

merge_399:                                        ; preds = %merge_397
  %"%scancode.load131" = load i64, ptr %scancode1, align 8
  %cmpeqtmp132 = icmp eq i64 %"%scancode.load131", 85
  br i1 %cmpeqtmp132, label %then_400, label %merge_401

merge_401:                                        ; preds = %merge_399
  %"%scancode.load134" = load i64, ptr %scancode1, align 8
  %cmpeqtmp135 = icmp eq i64 %"%scancode.load134", 91
  br i1 %cmpeqtmp135, label %then_402, label %merge_403

merge_403:                                        ; preds = %merge_401
  %"%scancode.load137" = load i64, ptr %scancode1, align 8
  %cmpeqtmp138 = icmp eq i64 %"%scancode.load137", 93
  br i1 %cmpeqtmp138, label %then_404, label %merge_405

merge_405:                                        ; preds = %merge_403
  %"%scancode.load140" = load i64, ptr %scancode1, align 8
  %calltmp = call i64 @ps2_sc2_to_ascii(i64 %"%scancode.load140")
  ret i64 %calltmp

then_312:                                         ; preds = %entry
  %loadtmp = load i64, ptr @sc2_shift_14, align 8
  ret i64 %loadtmp

then_314:                                         ; preds = %merge_313
  %loadtmp4 = load i64, ptr @sc2_shift_21, align 8
  ret i64 %loadtmp4

then_316:                                         ; preds = %merge_315
  %loadtmp7 = load i64, ptr @sc2_shift_22, align 8
  ret i64 %loadtmp7

then_318:                                         ; preds = %merge_317
  %loadtmp10 = load i64, ptr @sc2_shift_26, align 8
  ret i64 %loadtmp10

then_320:                                         ; preds = %merge_319
  %loadtmp13 = load i64, ptr @sc2_shift_27, align 8
  ret i64 %loadtmp13

then_322:                                         ; preds = %merge_321
  %loadtmp16 = load i64, ptr @sc2_shift_28, align 8
  ret i64 %loadtmp16

then_324:                                         ; preds = %merge_323
  %loadtmp19 = load i64, ptr @sc2_shift_29, align 8
  ret i64 %loadtmp19

then_326:                                         ; preds = %merge_325
  %loadtmp22 = load i64, ptr @sc2_shift_30, align 8
  ret i64 %loadtmp22

then_328:                                         ; preds = %merge_327
  %loadtmp25 = load i64, ptr @sc2_shift_33, align 8
  ret i64 %loadtmp25

then_330:                                         ; preds = %merge_329
  %loadtmp28 = load i64, ptr @sc2_shift_34, align 8
  ret i64 %loadtmp28

then_332:                                         ; preds = %merge_331
  %loadtmp31 = load i64, ptr @sc2_shift_35, align 8
  ret i64 %loadtmp31

then_334:                                         ; preds = %merge_333
  %loadtmp34 = load i64, ptr @sc2_shift_36, align 8
  ret i64 %loadtmp34

then_336:                                         ; preds = %merge_335
  %loadtmp37 = load i64, ptr @sc2_shift_37, align 8
  ret i64 %loadtmp37

then_338:                                         ; preds = %merge_337
  %loadtmp40 = load i64, ptr @sc2_shift_38, align 8
  ret i64 %loadtmp40

then_340:                                         ; preds = %merge_339
  %loadtmp43 = load i64, ptr @sc2_shift_42, align 8
  ret i64 %loadtmp43

then_342:                                         ; preds = %merge_341
  %loadtmp46 = load i64, ptr @sc2_shift_43, align 8
  ret i64 %loadtmp46

then_344:                                         ; preds = %merge_343
  %loadtmp49 = load i64, ptr @sc2_shift_44, align 8
  ret i64 %loadtmp49

then_346:                                         ; preds = %merge_345
  %loadtmp52 = load i64, ptr @sc2_shift_45, align 8
  ret i64 %loadtmp52

then_348:                                         ; preds = %merge_347
  %loadtmp55 = load i64, ptr @sc2_shift_46, align 8
  ret i64 %loadtmp55

then_350:                                         ; preds = %merge_349
  %loadtmp58 = load i64, ptr @sc2_shift_49, align 8
  ret i64 %loadtmp58

then_352:                                         ; preds = %merge_351
  %loadtmp61 = load i64, ptr @sc2_shift_50, align 8
  ret i64 %loadtmp61

then_354:                                         ; preds = %merge_353
  %loadtmp64 = load i64, ptr @sc2_shift_51, align 8
  ret i64 %loadtmp64

then_356:                                         ; preds = %merge_355
  %loadtmp67 = load i64, ptr @sc2_shift_52, align 8
  ret i64 %loadtmp67

then_358:                                         ; preds = %merge_357
  %loadtmp70 = load i64, ptr @sc2_shift_53, align 8
  ret i64 %loadtmp70

then_360:                                         ; preds = %merge_359
  %loadtmp73 = load i64, ptr @sc2_shift_54, align 8
  ret i64 %loadtmp73

then_362:                                         ; preds = %merge_361
  %loadtmp76 = load i64, ptr @sc2_shift_58, align 8
  ret i64 %loadtmp76

then_364:                                         ; preds = %merge_363
  %loadtmp79 = load i64, ptr @sc2_shift_59, align 8
  ret i64 %loadtmp79

then_366:                                         ; preds = %merge_365
  %loadtmp82 = load i64, ptr @sc2_shift_60, align 8
  ret i64 %loadtmp82

then_368:                                         ; preds = %merge_367
  %loadtmp85 = load i64, ptr @sc2_shift_61, align 8
  ret i64 %loadtmp85

then_370:                                         ; preds = %merge_369
  %loadtmp88 = load i64, ptr @sc2_shift_62, align 8
  ret i64 %loadtmp88

then_372:                                         ; preds = %merge_371
  %loadtmp91 = load i64, ptr @sc2_shift_65, align 8
  ret i64 %loadtmp91

then_374:                                         ; preds = %merge_373
  %loadtmp94 = load i64, ptr @sc2_shift_66, align 8
  ret i64 %loadtmp94

then_376:                                         ; preds = %merge_375
  %loadtmp97 = load i64, ptr @sc2_shift_67, align 8
  ret i64 %loadtmp97

then_378:                                         ; preds = %merge_377
  %loadtmp100 = load i64, ptr @sc2_shift_68, align 8
  ret i64 %loadtmp100

then_380:                                         ; preds = %merge_379
  %loadtmp103 = load i64, ptr @sc2_shift_69, align 8
  ret i64 %loadtmp103

then_382:                                         ; preds = %merge_381
  %loadtmp106 = load i64, ptr @sc2_shift_70, align 8
  ret i64 %loadtmp106

then_384:                                         ; preds = %merge_383
  %loadtmp109 = load i64, ptr @sc2_shift_73, align 8
  ret i64 %loadtmp109

then_386:                                         ; preds = %merge_385
  %loadtmp112 = load i64, ptr @sc2_shift_74, align 8
  ret i64 %loadtmp112

then_388:                                         ; preds = %merge_387
  %loadtmp115 = load i64, ptr @sc2_shift_75, align 8
  ret i64 %loadtmp115

then_390:                                         ; preds = %merge_389
  %loadtmp118 = load i64, ptr @sc2_shift_76, align 8
  ret i64 %loadtmp118

then_392:                                         ; preds = %merge_391
  %loadtmp121 = load i64, ptr @sc2_shift_77, align 8
  ret i64 %loadtmp121

then_394:                                         ; preds = %merge_393
  %loadtmp124 = load i64, ptr @sc2_shift_78, align 8
  ret i64 %loadtmp124

then_396:                                         ; preds = %merge_395
  %loadtmp127 = load i64, ptr @sc2_shift_82, align 8
  ret i64 %loadtmp127

then_398:                                         ; preds = %merge_397
  %loadtmp130 = load i64, ptr @sc2_shift_84, align 8
  ret i64 %loadtmp130

then_400:                                         ; preds = %merge_399
  %loadtmp133 = load i64, ptr @sc2_shift_85, align 8
  ret i64 %loadtmp133

then_402:                                         ; preds = %merge_401
  %loadtmp136 = load i64, ptr @sc2_shift_91, align 8
  ret i64 %loadtmp136

then_404:                                         ; preds = %merge_403
  %loadtmp139 = load i64, ptr @sc2_shift_93, align 8
  ret i64 %loadtmp139
}

define i64 @kbd_buf_set_sc(i64 %idx, i64 %val) {
entry:
  %"%kbd_buf_scancode_31" = alloca i64, align 8
  %"%kbd_buf_scancode_30" = alloca i64, align 8
  %"%kbd_buf_scancode_29" = alloca i64, align 8
  %"%kbd_buf_scancode_28" = alloca i64, align 8
  %"%kbd_buf_scancode_27" = alloca i64, align 8
  %"%kbd_buf_scancode_26" = alloca i64, align 8
  %"%kbd_buf_scancode_25" = alloca i64, align 8
  %"%kbd_buf_scancode_24" = alloca i64, align 8
  %"%kbd_buf_scancode_23" = alloca i64, align 8
  %"%kbd_buf_scancode_22" = alloca i64, align 8
  %"%kbd_buf_scancode_21" = alloca i64, align 8
  %"%kbd_buf_scancode_20" = alloca i64, align 8
  %"%kbd_buf_scancode_19" = alloca i64, align 8
  %"%kbd_buf_scancode_18" = alloca i64, align 8
  %"%kbd_buf_scancode_17" = alloca i64, align 8
  %"%kbd_buf_scancode_16" = alloca i64, align 8
  %"%kbd_buf_scancode_15" = alloca i64, align 8
  %"%kbd_buf_scancode_14" = alloca i64, align 8
  %"%kbd_buf_scancode_13" = alloca i64, align 8
  %"%kbd_buf_scancode_12" = alloca i64, align 8
  %"%kbd_buf_scancode_11" = alloca i64, align 8
  %"%kbd_buf_scancode_10" = alloca i64, align 8
  %"%kbd_buf_scancode_9" = alloca i64, align 8
  %"%kbd_buf_scancode_8" = alloca i64, align 8
  %"%kbd_buf_scancode_7" = alloca i64, align 8
  %"%kbd_buf_scancode_6" = alloca i64, align 8
  %"%kbd_buf_scancode_5" = alloca i64, align 8
  %"%kbd_buf_scancode_4" = alloca i64, align 8
  %"%kbd_buf_scancode_3" = alloca i64, align 8
  %"%kbd_buf_scancode_2" = alloca i64, align 8
  %"%kbd_buf_scancode_1" = alloca i64, align 8
  %"%kbd_buf_scancode_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_406, label %merge_407

merge_407:                                        ; preds = %then_406, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_408, label %merge_409

merge_409:                                        ; preds = %then_408, %merge_407
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_410, label %merge_411

merge_411:                                        ; preds = %then_410, %merge_409
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_412, label %merge_413

merge_413:                                        ; preds = %then_412, %merge_411
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_414, label %merge_415

merge_415:                                        ; preds = %then_414, %merge_413
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_416, label %merge_417

merge_417:                                        ; preds = %then_416, %merge_415
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_418, label %merge_419

merge_419:                                        ; preds = %then_418, %merge_417
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_420, label %merge_421

merge_421:                                        ; preds = %then_420, %merge_419
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_422, label %merge_423

merge_423:                                        ; preds = %then_422, %merge_421
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_424, label %merge_425

merge_425:                                        ; preds = %then_424, %merge_423
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_426, label %merge_427

merge_427:                                        ; preds = %then_426, %merge_425
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_428, label %merge_429

merge_429:                                        ; preds = %then_428, %merge_427
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_430, label %merge_431

merge_431:                                        ; preds = %then_430, %merge_429
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_432, label %merge_433

merge_433:                                        ; preds = %then_432, %merge_431
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_434, label %merge_435

merge_435:                                        ; preds = %then_434, %merge_433
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_436, label %merge_437

merge_437:                                        ; preds = %then_436, %merge_435
  %"%idx.load33" = load i64, ptr %idx1, align 8
  %cmpeqtmp34 = icmp eq i64 %"%idx.load33", 16
  br i1 %cmpeqtmp34, label %then_438, label %merge_439

merge_439:                                        ; preds = %then_438, %merge_437
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 17
  br i1 %cmpeqtmp36, label %then_440, label %merge_441

merge_441:                                        ; preds = %then_440, %merge_439
  %"%idx.load37" = load i64, ptr %idx1, align 8
  %cmpeqtmp38 = icmp eq i64 %"%idx.load37", 18
  br i1 %cmpeqtmp38, label %then_442, label %merge_443

merge_443:                                        ; preds = %then_442, %merge_441
  %"%idx.load39" = load i64, ptr %idx1, align 8
  %cmpeqtmp40 = icmp eq i64 %"%idx.load39", 19
  br i1 %cmpeqtmp40, label %then_444, label %merge_445

merge_445:                                        ; preds = %then_444, %merge_443
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 20
  br i1 %cmpeqtmp42, label %then_446, label %merge_447

merge_447:                                        ; preds = %then_446, %merge_445
  %"%idx.load43" = load i64, ptr %idx1, align 8
  %cmpeqtmp44 = icmp eq i64 %"%idx.load43", 21
  br i1 %cmpeqtmp44, label %then_448, label %merge_449

merge_449:                                        ; preds = %then_448, %merge_447
  %"%idx.load45" = load i64, ptr %idx1, align 8
  %cmpeqtmp46 = icmp eq i64 %"%idx.load45", 22
  br i1 %cmpeqtmp46, label %then_450, label %merge_451

merge_451:                                        ; preds = %then_450, %merge_449
  %"%idx.load47" = load i64, ptr %idx1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%idx.load47", 23
  br i1 %cmpeqtmp48, label %then_452, label %merge_453

merge_453:                                        ; preds = %then_452, %merge_451
  %"%idx.load49" = load i64, ptr %idx1, align 8
  %cmpeqtmp50 = icmp eq i64 %"%idx.load49", 24
  br i1 %cmpeqtmp50, label %then_454, label %merge_455

merge_455:                                        ; preds = %then_454, %merge_453
  %"%idx.load51" = load i64, ptr %idx1, align 8
  %cmpeqtmp52 = icmp eq i64 %"%idx.load51", 25
  br i1 %cmpeqtmp52, label %then_456, label %merge_457

merge_457:                                        ; preds = %then_456, %merge_455
  %"%idx.load53" = load i64, ptr %idx1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%idx.load53", 26
  br i1 %cmpeqtmp54, label %then_458, label %merge_459

merge_459:                                        ; preds = %then_458, %merge_457
  %"%idx.load55" = load i64, ptr %idx1, align 8
  %cmpeqtmp56 = icmp eq i64 %"%idx.load55", 27
  br i1 %cmpeqtmp56, label %then_460, label %merge_461

merge_461:                                        ; preds = %then_460, %merge_459
  %"%idx.load57" = load i64, ptr %idx1, align 8
  %cmpeqtmp58 = icmp eq i64 %"%idx.load57", 28
  br i1 %cmpeqtmp58, label %then_462, label %merge_463

merge_463:                                        ; preds = %then_462, %merge_461
  %"%idx.load59" = load i64, ptr %idx1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%idx.load59", 29
  br i1 %cmpeqtmp60, label %then_464, label %merge_465

merge_465:                                        ; preds = %then_464, %merge_463
  %"%idx.load61" = load i64, ptr %idx1, align 8
  %cmpeqtmp62 = icmp eq i64 %"%idx.load61", 30
  br i1 %cmpeqtmp62, label %then_466, label %merge_467

merge_467:                                        ; preds = %then_466, %merge_465
  %"%idx.load63" = load i64, ptr %idx1, align 8
  %cmpeqtmp64 = icmp eq i64 %"%idx.load63", 31
  br i1 %cmpeqtmp64, label %then_468, label %merge_469

merge_469:                                        ; preds = %then_468, %merge_467
  ret i64 0

then_406:                                         ; preds = %entry
  %"%1097.load" = load i64, ptr %val2, align 8
  store i64 %"%1097.load", ptr %"%kbd_buf_scancode_0", align 8
  br label %merge_407

then_408:                                         ; preds = %merge_407
  %"%1101.load" = load i64, ptr %val2, align 8
  store i64 %"%1101.load", ptr %"%kbd_buf_scancode_1", align 8
  br label %merge_409

then_410:                                         ; preds = %merge_409
  %"%1105.load" = load i64, ptr %val2, align 8
  store i64 %"%1105.load", ptr %"%kbd_buf_scancode_2", align 8
  br label %merge_411

then_412:                                         ; preds = %merge_411
  %"%1109.load" = load i64, ptr %val2, align 8
  store i64 %"%1109.load", ptr %"%kbd_buf_scancode_3", align 8
  br label %merge_413

then_414:                                         ; preds = %merge_413
  %"%1113.load" = load i64, ptr %val2, align 8
  store i64 %"%1113.load", ptr %"%kbd_buf_scancode_4", align 8
  br label %merge_415

then_416:                                         ; preds = %merge_415
  %"%1117.load" = load i64, ptr %val2, align 8
  store i64 %"%1117.load", ptr %"%kbd_buf_scancode_5", align 8
  br label %merge_417

then_418:                                         ; preds = %merge_417
  %"%1121.load" = load i64, ptr %val2, align 8
  store i64 %"%1121.load", ptr %"%kbd_buf_scancode_6", align 8
  br label %merge_419

then_420:                                         ; preds = %merge_419
  %"%1125.load" = load i64, ptr %val2, align 8
  store i64 %"%1125.load", ptr %"%kbd_buf_scancode_7", align 8
  br label %merge_421

then_422:                                         ; preds = %merge_421
  %"%1129.load" = load i64, ptr %val2, align 8
  store i64 %"%1129.load", ptr %"%kbd_buf_scancode_8", align 8
  br label %merge_423

then_424:                                         ; preds = %merge_423
  %"%1133.load" = load i64, ptr %val2, align 8
  store i64 %"%1133.load", ptr %"%kbd_buf_scancode_9", align 8
  br label %merge_425

then_426:                                         ; preds = %merge_425
  %"%1137.load" = load i64, ptr %val2, align 8
  store i64 %"%1137.load", ptr %"%kbd_buf_scancode_10", align 8
  br label %merge_427

then_428:                                         ; preds = %merge_427
  %"%1141.load" = load i64, ptr %val2, align 8
  store i64 %"%1141.load", ptr %"%kbd_buf_scancode_11", align 8
  br label %merge_429

then_430:                                         ; preds = %merge_429
  %"%1145.load" = load i64, ptr %val2, align 8
  store i64 %"%1145.load", ptr %"%kbd_buf_scancode_12", align 8
  br label %merge_431

then_432:                                         ; preds = %merge_431
  %"%1149.load" = load i64, ptr %val2, align 8
  store i64 %"%1149.load", ptr %"%kbd_buf_scancode_13", align 8
  br label %merge_433

then_434:                                         ; preds = %merge_433
  %"%1153.load" = load i64, ptr %val2, align 8
  store i64 %"%1153.load", ptr %"%kbd_buf_scancode_14", align 8
  br label %merge_435

then_436:                                         ; preds = %merge_435
  %"%1157.load" = load i64, ptr %val2, align 8
  store i64 %"%1157.load", ptr %"%kbd_buf_scancode_15", align 8
  br label %merge_437

then_438:                                         ; preds = %merge_437
  %"%1161.load" = load i64, ptr %val2, align 8
  store i64 %"%1161.load", ptr %"%kbd_buf_scancode_16", align 8
  br label %merge_439

then_440:                                         ; preds = %merge_439
  %"%1165.load" = load i64, ptr %val2, align 8
  store i64 %"%1165.load", ptr %"%kbd_buf_scancode_17", align 8
  br label %merge_441

then_442:                                         ; preds = %merge_441
  %"%1169.load" = load i64, ptr %val2, align 8
  store i64 %"%1169.load", ptr %"%kbd_buf_scancode_18", align 8
  br label %merge_443

then_444:                                         ; preds = %merge_443
  %"%1173.load" = load i64, ptr %val2, align 8
  store i64 %"%1173.load", ptr %"%kbd_buf_scancode_19", align 8
  br label %merge_445

then_446:                                         ; preds = %merge_445
  %"%1177.load" = load i64, ptr %val2, align 8
  store i64 %"%1177.load", ptr %"%kbd_buf_scancode_20", align 8
  br label %merge_447

then_448:                                         ; preds = %merge_447
  %"%1181.load" = load i64, ptr %val2, align 8
  store i64 %"%1181.load", ptr %"%kbd_buf_scancode_21", align 8
  br label %merge_449

then_450:                                         ; preds = %merge_449
  %"%1185.load" = load i64, ptr %val2, align 8
  store i64 %"%1185.load", ptr %"%kbd_buf_scancode_22", align 8
  br label %merge_451

then_452:                                         ; preds = %merge_451
  %"%1189.load" = load i64, ptr %val2, align 8
  store i64 %"%1189.load", ptr %"%kbd_buf_scancode_23", align 8
  br label %merge_453

then_454:                                         ; preds = %merge_453
  %"%1193.load" = load i64, ptr %val2, align 8
  store i64 %"%1193.load", ptr %"%kbd_buf_scancode_24", align 8
  br label %merge_455

then_456:                                         ; preds = %merge_455
  %"%1197.load" = load i64, ptr %val2, align 8
  store i64 %"%1197.load", ptr %"%kbd_buf_scancode_25", align 8
  br label %merge_457

then_458:                                         ; preds = %merge_457
  %"%1201.load" = load i64, ptr %val2, align 8
  store i64 %"%1201.load", ptr %"%kbd_buf_scancode_26", align 8
  br label %merge_459

then_460:                                         ; preds = %merge_459
  %"%1205.load" = load i64, ptr %val2, align 8
  store i64 %"%1205.load", ptr %"%kbd_buf_scancode_27", align 8
  br label %merge_461

then_462:                                         ; preds = %merge_461
  %"%1209.load" = load i64, ptr %val2, align 8
  store i64 %"%1209.load", ptr %"%kbd_buf_scancode_28", align 8
  br label %merge_463

then_464:                                         ; preds = %merge_463
  %"%1213.load" = load i64, ptr %val2, align 8
  store i64 %"%1213.load", ptr %"%kbd_buf_scancode_29", align 8
  br label %merge_465

then_466:                                         ; preds = %merge_465
  %"%1217.load" = load i64, ptr %val2, align 8
  store i64 %"%1217.load", ptr %"%kbd_buf_scancode_30", align 8
  br label %merge_467

then_468:                                         ; preds = %merge_467
  %"%1221.load" = load i64, ptr %val2, align 8
  store i64 %"%1221.load", ptr %"%kbd_buf_scancode_31", align 8
  br label %merge_469
}

define i64 @kbd_buf_set_ascii(i64 %idx, i64 %val) {
entry:
  %"%kbd_buf_ascii_31" = alloca i64, align 8
  %"%kbd_buf_ascii_30" = alloca i64, align 8
  %"%kbd_buf_ascii_29" = alloca i64, align 8
  %"%kbd_buf_ascii_28" = alloca i64, align 8
  %"%kbd_buf_ascii_27" = alloca i64, align 8
  %"%kbd_buf_ascii_26" = alloca i64, align 8
  %"%kbd_buf_ascii_25" = alloca i64, align 8
  %"%kbd_buf_ascii_24" = alloca i64, align 8
  %"%kbd_buf_ascii_23" = alloca i64, align 8
  %"%kbd_buf_ascii_22" = alloca i64, align 8
  %"%kbd_buf_ascii_21" = alloca i64, align 8
  %"%kbd_buf_ascii_20" = alloca i64, align 8
  %"%kbd_buf_ascii_19" = alloca i64, align 8
  %"%kbd_buf_ascii_18" = alloca i64, align 8
  %"%kbd_buf_ascii_17" = alloca i64, align 8
  %"%kbd_buf_ascii_16" = alloca i64, align 8
  %"%kbd_buf_ascii_15" = alloca i64, align 8
  %"%kbd_buf_ascii_14" = alloca i64, align 8
  %"%kbd_buf_ascii_13" = alloca i64, align 8
  %"%kbd_buf_ascii_12" = alloca i64, align 8
  %"%kbd_buf_ascii_11" = alloca i64, align 8
  %"%kbd_buf_ascii_10" = alloca i64, align 8
  %"%kbd_buf_ascii_9" = alloca i64, align 8
  %"%kbd_buf_ascii_8" = alloca i64, align 8
  %"%kbd_buf_ascii_7" = alloca i64, align 8
  %"%kbd_buf_ascii_6" = alloca i64, align 8
  %"%kbd_buf_ascii_5" = alloca i64, align 8
  %"%kbd_buf_ascii_4" = alloca i64, align 8
  %"%kbd_buf_ascii_3" = alloca i64, align 8
  %"%kbd_buf_ascii_2" = alloca i64, align 8
  %"%kbd_buf_ascii_1" = alloca i64, align 8
  %"%kbd_buf_ascii_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_470, label %merge_471

merge_471:                                        ; preds = %then_470, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_472, label %merge_473

merge_473:                                        ; preds = %then_472, %merge_471
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_474, label %merge_475

merge_475:                                        ; preds = %then_474, %merge_473
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_476, label %merge_477

merge_477:                                        ; preds = %then_476, %merge_475
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_478, label %merge_479

merge_479:                                        ; preds = %then_478, %merge_477
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_480, label %merge_481

merge_481:                                        ; preds = %then_480, %merge_479
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_482, label %merge_483

merge_483:                                        ; preds = %then_482, %merge_481
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_484, label %merge_485

merge_485:                                        ; preds = %then_484, %merge_483
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_486, label %merge_487

merge_487:                                        ; preds = %then_486, %merge_485
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_488, label %merge_489

merge_489:                                        ; preds = %then_488, %merge_487
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_490, label %merge_491

merge_491:                                        ; preds = %then_490, %merge_489
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_492, label %merge_493

merge_493:                                        ; preds = %then_492, %merge_491
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_494, label %merge_495

merge_495:                                        ; preds = %then_494, %merge_493
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_496, label %merge_497

merge_497:                                        ; preds = %then_496, %merge_495
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_498, label %merge_499

merge_499:                                        ; preds = %then_498, %merge_497
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_500, label %merge_501

merge_501:                                        ; preds = %then_500, %merge_499
  %"%idx.load33" = load i64, ptr %idx1, align 8
  %cmpeqtmp34 = icmp eq i64 %"%idx.load33", 16
  br i1 %cmpeqtmp34, label %then_502, label %merge_503

merge_503:                                        ; preds = %then_502, %merge_501
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 17
  br i1 %cmpeqtmp36, label %then_504, label %merge_505

merge_505:                                        ; preds = %then_504, %merge_503
  %"%idx.load37" = load i64, ptr %idx1, align 8
  %cmpeqtmp38 = icmp eq i64 %"%idx.load37", 18
  br i1 %cmpeqtmp38, label %then_506, label %merge_507

merge_507:                                        ; preds = %then_506, %merge_505
  %"%idx.load39" = load i64, ptr %idx1, align 8
  %cmpeqtmp40 = icmp eq i64 %"%idx.load39", 19
  br i1 %cmpeqtmp40, label %then_508, label %merge_509

merge_509:                                        ; preds = %then_508, %merge_507
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 20
  br i1 %cmpeqtmp42, label %then_510, label %merge_511

merge_511:                                        ; preds = %then_510, %merge_509
  %"%idx.load43" = load i64, ptr %idx1, align 8
  %cmpeqtmp44 = icmp eq i64 %"%idx.load43", 21
  br i1 %cmpeqtmp44, label %then_512, label %merge_513

merge_513:                                        ; preds = %then_512, %merge_511
  %"%idx.load45" = load i64, ptr %idx1, align 8
  %cmpeqtmp46 = icmp eq i64 %"%idx.load45", 22
  br i1 %cmpeqtmp46, label %then_514, label %merge_515

merge_515:                                        ; preds = %then_514, %merge_513
  %"%idx.load47" = load i64, ptr %idx1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%idx.load47", 23
  br i1 %cmpeqtmp48, label %then_516, label %merge_517

merge_517:                                        ; preds = %then_516, %merge_515
  %"%idx.load49" = load i64, ptr %idx1, align 8
  %cmpeqtmp50 = icmp eq i64 %"%idx.load49", 24
  br i1 %cmpeqtmp50, label %then_518, label %merge_519

merge_519:                                        ; preds = %then_518, %merge_517
  %"%idx.load51" = load i64, ptr %idx1, align 8
  %cmpeqtmp52 = icmp eq i64 %"%idx.load51", 25
  br i1 %cmpeqtmp52, label %then_520, label %merge_521

merge_521:                                        ; preds = %then_520, %merge_519
  %"%idx.load53" = load i64, ptr %idx1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%idx.load53", 26
  br i1 %cmpeqtmp54, label %then_522, label %merge_523

merge_523:                                        ; preds = %then_522, %merge_521
  %"%idx.load55" = load i64, ptr %idx1, align 8
  %cmpeqtmp56 = icmp eq i64 %"%idx.load55", 27
  br i1 %cmpeqtmp56, label %then_524, label %merge_525

merge_525:                                        ; preds = %then_524, %merge_523
  %"%idx.load57" = load i64, ptr %idx1, align 8
  %cmpeqtmp58 = icmp eq i64 %"%idx.load57", 28
  br i1 %cmpeqtmp58, label %then_526, label %merge_527

merge_527:                                        ; preds = %then_526, %merge_525
  %"%idx.load59" = load i64, ptr %idx1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%idx.load59", 29
  br i1 %cmpeqtmp60, label %then_528, label %merge_529

merge_529:                                        ; preds = %then_528, %merge_527
  %"%idx.load61" = load i64, ptr %idx1, align 8
  %cmpeqtmp62 = icmp eq i64 %"%idx.load61", 30
  br i1 %cmpeqtmp62, label %then_530, label %merge_531

merge_531:                                        ; preds = %then_530, %merge_529
  %"%idx.load63" = load i64, ptr %idx1, align 8
  %cmpeqtmp64 = icmp eq i64 %"%idx.load63", 31
  br i1 %cmpeqtmp64, label %then_532, label %merge_533

merge_533:                                        ; preds = %then_532, %merge_531
  ret i64 0

then_470:                                         ; preds = %entry
  %"%1226.load" = load i64, ptr %val2, align 8
  store i64 %"%1226.load", ptr %"%kbd_buf_ascii_0", align 8
  br label %merge_471

then_472:                                         ; preds = %merge_471
  %"%1230.load" = load i64, ptr %val2, align 8
  store i64 %"%1230.load", ptr %"%kbd_buf_ascii_1", align 8
  br label %merge_473

then_474:                                         ; preds = %merge_473
  %"%1234.load" = load i64, ptr %val2, align 8
  store i64 %"%1234.load", ptr %"%kbd_buf_ascii_2", align 8
  br label %merge_475

then_476:                                         ; preds = %merge_475
  %"%1238.load" = load i64, ptr %val2, align 8
  store i64 %"%1238.load", ptr %"%kbd_buf_ascii_3", align 8
  br label %merge_477

then_478:                                         ; preds = %merge_477
  %"%1242.load" = load i64, ptr %val2, align 8
  store i64 %"%1242.load", ptr %"%kbd_buf_ascii_4", align 8
  br label %merge_479

then_480:                                         ; preds = %merge_479
  %"%1246.load" = load i64, ptr %val2, align 8
  store i64 %"%1246.load", ptr %"%kbd_buf_ascii_5", align 8
  br label %merge_481

then_482:                                         ; preds = %merge_481
  %"%1250.load" = load i64, ptr %val2, align 8
  store i64 %"%1250.load", ptr %"%kbd_buf_ascii_6", align 8
  br label %merge_483

then_484:                                         ; preds = %merge_483
  %"%1254.load" = load i64, ptr %val2, align 8
  store i64 %"%1254.load", ptr %"%kbd_buf_ascii_7", align 8
  br label %merge_485

then_486:                                         ; preds = %merge_485
  %"%1258.load" = load i64, ptr %val2, align 8
  store i64 %"%1258.load", ptr %"%kbd_buf_ascii_8", align 8
  br label %merge_487

then_488:                                         ; preds = %merge_487
  %"%1262.load" = load i64, ptr %val2, align 8
  store i64 %"%1262.load", ptr %"%kbd_buf_ascii_9", align 8
  br label %merge_489

then_490:                                         ; preds = %merge_489
  %"%1266.load" = load i64, ptr %val2, align 8
  store i64 %"%1266.load", ptr %"%kbd_buf_ascii_10", align 8
  br label %merge_491

then_492:                                         ; preds = %merge_491
  %"%1270.load" = load i64, ptr %val2, align 8
  store i64 %"%1270.load", ptr %"%kbd_buf_ascii_11", align 8
  br label %merge_493

then_494:                                         ; preds = %merge_493
  %"%1274.load" = load i64, ptr %val2, align 8
  store i64 %"%1274.load", ptr %"%kbd_buf_ascii_12", align 8
  br label %merge_495

then_496:                                         ; preds = %merge_495
  %"%1278.load" = load i64, ptr %val2, align 8
  store i64 %"%1278.load", ptr %"%kbd_buf_ascii_13", align 8
  br label %merge_497

then_498:                                         ; preds = %merge_497
  %"%1282.load" = load i64, ptr %val2, align 8
  store i64 %"%1282.load", ptr %"%kbd_buf_ascii_14", align 8
  br label %merge_499

then_500:                                         ; preds = %merge_499
  %"%1286.load" = load i64, ptr %val2, align 8
  store i64 %"%1286.load", ptr %"%kbd_buf_ascii_15", align 8
  br label %merge_501

then_502:                                         ; preds = %merge_501
  %"%1290.load" = load i64, ptr %val2, align 8
  store i64 %"%1290.load", ptr %"%kbd_buf_ascii_16", align 8
  br label %merge_503

then_504:                                         ; preds = %merge_503
  %"%1294.load" = load i64, ptr %val2, align 8
  store i64 %"%1294.load", ptr %"%kbd_buf_ascii_17", align 8
  br label %merge_505

then_506:                                         ; preds = %merge_505
  %"%1298.load" = load i64, ptr %val2, align 8
  store i64 %"%1298.load", ptr %"%kbd_buf_ascii_18", align 8
  br label %merge_507

then_508:                                         ; preds = %merge_507
  %"%1302.load" = load i64, ptr %val2, align 8
  store i64 %"%1302.load", ptr %"%kbd_buf_ascii_19", align 8
  br label %merge_509

then_510:                                         ; preds = %merge_509
  %"%1306.load" = load i64, ptr %val2, align 8
  store i64 %"%1306.load", ptr %"%kbd_buf_ascii_20", align 8
  br label %merge_511

then_512:                                         ; preds = %merge_511
  %"%1310.load" = load i64, ptr %val2, align 8
  store i64 %"%1310.load", ptr %"%kbd_buf_ascii_21", align 8
  br label %merge_513

then_514:                                         ; preds = %merge_513
  %"%1314.load" = load i64, ptr %val2, align 8
  store i64 %"%1314.load", ptr %"%kbd_buf_ascii_22", align 8
  br label %merge_515

then_516:                                         ; preds = %merge_515
  %"%1318.load" = load i64, ptr %val2, align 8
  store i64 %"%1318.load", ptr %"%kbd_buf_ascii_23", align 8
  br label %merge_517

then_518:                                         ; preds = %merge_517
  %"%1322.load" = load i64, ptr %val2, align 8
  store i64 %"%1322.load", ptr %"%kbd_buf_ascii_24", align 8
  br label %merge_519

then_520:                                         ; preds = %merge_519
  %"%1326.load" = load i64, ptr %val2, align 8
  store i64 %"%1326.load", ptr %"%kbd_buf_ascii_25", align 8
  br label %merge_521

then_522:                                         ; preds = %merge_521
  %"%1330.load" = load i64, ptr %val2, align 8
  store i64 %"%1330.load", ptr %"%kbd_buf_ascii_26", align 8
  br label %merge_523

then_524:                                         ; preds = %merge_523
  %"%1334.load" = load i64, ptr %val2, align 8
  store i64 %"%1334.load", ptr %"%kbd_buf_ascii_27", align 8
  br label %merge_525

then_526:                                         ; preds = %merge_525
  %"%1338.load" = load i64, ptr %val2, align 8
  store i64 %"%1338.load", ptr %"%kbd_buf_ascii_28", align 8
  br label %merge_527

then_528:                                         ; preds = %merge_527
  %"%1342.load" = load i64, ptr %val2, align 8
  store i64 %"%1342.load", ptr %"%kbd_buf_ascii_29", align 8
  br label %merge_529

then_530:                                         ; preds = %merge_529
  %"%1346.load" = load i64, ptr %val2, align 8
  store i64 %"%1346.load", ptr %"%kbd_buf_ascii_30", align 8
  br label %merge_531

then_532:                                         ; preds = %merge_531
  %"%1350.load" = load i64, ptr %val2, align 8
  store i64 %"%1350.load", ptr %"%kbd_buf_ascii_31", align 8
  br label %merge_533
}

define i64 @kbd_buf_get_sc(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_534, label %merge_535

merge_535:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_536, label %merge_537

merge_537:                                        ; preds = %merge_535
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_538, label %merge_539

merge_539:                                        ; preds = %merge_537
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_540, label %merge_541

merge_541:                                        ; preds = %merge_539
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_542, label %merge_543

merge_543:                                        ; preds = %merge_541
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_544, label %merge_545

merge_545:                                        ; preds = %merge_543
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_546, label %merge_547

merge_547:                                        ; preds = %merge_545
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_548, label %merge_549

merge_549:                                        ; preds = %merge_547
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 8
  br i1 %cmpeqtmp24, label %then_550, label %merge_551

merge_551:                                        ; preds = %merge_549
  %"%idx.load26" = load i64, ptr %idx1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%idx.load26", 9
  br i1 %cmpeqtmp27, label %then_552, label %merge_553

merge_553:                                        ; preds = %merge_551
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 10
  br i1 %cmpeqtmp30, label %then_554, label %merge_555

merge_555:                                        ; preds = %merge_553
  %"%idx.load32" = load i64, ptr %idx1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%idx.load32", 11
  br i1 %cmpeqtmp33, label %then_556, label %merge_557

merge_557:                                        ; preds = %merge_555
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 12
  br i1 %cmpeqtmp36, label %then_558, label %merge_559

merge_559:                                        ; preds = %merge_557
  %"%idx.load38" = load i64, ptr %idx1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%idx.load38", 13
  br i1 %cmpeqtmp39, label %then_560, label %merge_561

merge_561:                                        ; preds = %merge_559
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 14
  br i1 %cmpeqtmp42, label %then_562, label %merge_563

merge_563:                                        ; preds = %merge_561
  %"%idx.load44" = load i64, ptr %idx1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%idx.load44", 15
  br i1 %cmpeqtmp45, label %then_564, label %merge_565

merge_565:                                        ; preds = %merge_563
  %"%idx.load47" = load i64, ptr %idx1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%idx.load47", 16
  br i1 %cmpeqtmp48, label %then_566, label %merge_567

merge_567:                                        ; preds = %merge_565
  %"%idx.load50" = load i64, ptr %idx1, align 8
  %cmpeqtmp51 = icmp eq i64 %"%idx.load50", 17
  br i1 %cmpeqtmp51, label %then_568, label %merge_569

merge_569:                                        ; preds = %merge_567
  %"%idx.load53" = load i64, ptr %idx1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%idx.load53", 18
  br i1 %cmpeqtmp54, label %then_570, label %merge_571

merge_571:                                        ; preds = %merge_569
  %"%idx.load56" = load i64, ptr %idx1, align 8
  %cmpeqtmp57 = icmp eq i64 %"%idx.load56", 19
  br i1 %cmpeqtmp57, label %then_572, label %merge_573

merge_573:                                        ; preds = %merge_571
  %"%idx.load59" = load i64, ptr %idx1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%idx.load59", 20
  br i1 %cmpeqtmp60, label %then_574, label %merge_575

merge_575:                                        ; preds = %merge_573
  %"%idx.load62" = load i64, ptr %idx1, align 8
  %cmpeqtmp63 = icmp eq i64 %"%idx.load62", 21
  br i1 %cmpeqtmp63, label %then_576, label %merge_577

merge_577:                                        ; preds = %merge_575
  %"%idx.load65" = load i64, ptr %idx1, align 8
  %cmpeqtmp66 = icmp eq i64 %"%idx.load65", 22
  br i1 %cmpeqtmp66, label %then_578, label %merge_579

merge_579:                                        ; preds = %merge_577
  %"%idx.load68" = load i64, ptr %idx1, align 8
  %cmpeqtmp69 = icmp eq i64 %"%idx.load68", 23
  br i1 %cmpeqtmp69, label %then_580, label %merge_581

merge_581:                                        ; preds = %merge_579
  %"%idx.load71" = load i64, ptr %idx1, align 8
  %cmpeqtmp72 = icmp eq i64 %"%idx.load71", 24
  br i1 %cmpeqtmp72, label %then_582, label %merge_583

merge_583:                                        ; preds = %merge_581
  %"%idx.load74" = load i64, ptr %idx1, align 8
  %cmpeqtmp75 = icmp eq i64 %"%idx.load74", 25
  br i1 %cmpeqtmp75, label %then_584, label %merge_585

merge_585:                                        ; preds = %merge_583
  %"%idx.load77" = load i64, ptr %idx1, align 8
  %cmpeqtmp78 = icmp eq i64 %"%idx.load77", 26
  br i1 %cmpeqtmp78, label %then_586, label %merge_587

merge_587:                                        ; preds = %merge_585
  %"%idx.load80" = load i64, ptr %idx1, align 8
  %cmpeqtmp81 = icmp eq i64 %"%idx.load80", 27
  br i1 %cmpeqtmp81, label %then_588, label %merge_589

merge_589:                                        ; preds = %merge_587
  %"%idx.load83" = load i64, ptr %idx1, align 8
  %cmpeqtmp84 = icmp eq i64 %"%idx.load83", 28
  br i1 %cmpeqtmp84, label %then_590, label %merge_591

merge_591:                                        ; preds = %merge_589
  %"%idx.load86" = load i64, ptr %idx1, align 8
  %cmpeqtmp87 = icmp eq i64 %"%idx.load86", 29
  br i1 %cmpeqtmp87, label %then_592, label %merge_593

merge_593:                                        ; preds = %merge_591
  %"%idx.load89" = load i64, ptr %idx1, align 8
  %cmpeqtmp90 = icmp eq i64 %"%idx.load89", 30
  br i1 %cmpeqtmp90, label %then_594, label %merge_595

merge_595:                                        ; preds = %merge_593
  %"%idx.load92" = load i64, ptr %idx1, align 8
  %cmpeqtmp93 = icmp eq i64 %"%idx.load92", 31
  br i1 %cmpeqtmp93, label %then_596, label %merge_597

merge_597:                                        ; preds = %merge_595
  ret i64 0

then_534:                                         ; preds = %entry
  %loadtmp = load i64, ptr @kbd_buf_scancode_0, align 8
  ret i64 %loadtmp

then_536:                                         ; preds = %merge_535
  %loadtmp4 = load i64, ptr @kbd_buf_scancode_1, align 8
  ret i64 %loadtmp4

then_538:                                         ; preds = %merge_537
  %loadtmp7 = load i64, ptr @kbd_buf_scancode_2, align 8
  ret i64 %loadtmp7

then_540:                                         ; preds = %merge_539
  %loadtmp10 = load i64, ptr @kbd_buf_scancode_3, align 8
  ret i64 %loadtmp10

then_542:                                         ; preds = %merge_541
  %loadtmp13 = load i64, ptr @kbd_buf_scancode_4, align 8
  ret i64 %loadtmp13

then_544:                                         ; preds = %merge_543
  %loadtmp16 = load i64, ptr @kbd_buf_scancode_5, align 8
  ret i64 %loadtmp16

then_546:                                         ; preds = %merge_545
  %loadtmp19 = load i64, ptr @kbd_buf_scancode_6, align 8
  ret i64 %loadtmp19

then_548:                                         ; preds = %merge_547
  %loadtmp22 = load i64, ptr @kbd_buf_scancode_7, align 8
  ret i64 %loadtmp22

then_550:                                         ; preds = %merge_549
  %loadtmp25 = load i64, ptr @kbd_buf_scancode_8, align 8
  ret i64 %loadtmp25

then_552:                                         ; preds = %merge_551
  %loadtmp28 = load i64, ptr @kbd_buf_scancode_9, align 8
  ret i64 %loadtmp28

then_554:                                         ; preds = %merge_553
  %loadtmp31 = load i64, ptr @kbd_buf_scancode_10, align 8
  ret i64 %loadtmp31

then_556:                                         ; preds = %merge_555
  %loadtmp34 = load i64, ptr @kbd_buf_scancode_11, align 8
  ret i64 %loadtmp34

then_558:                                         ; preds = %merge_557
  %loadtmp37 = load i64, ptr @kbd_buf_scancode_12, align 8
  ret i64 %loadtmp37

then_560:                                         ; preds = %merge_559
  %loadtmp40 = load i64, ptr @kbd_buf_scancode_13, align 8
  ret i64 %loadtmp40

then_562:                                         ; preds = %merge_561
  %loadtmp43 = load i64, ptr @kbd_buf_scancode_14, align 8
  ret i64 %loadtmp43

then_564:                                         ; preds = %merge_563
  %loadtmp46 = load i64, ptr @kbd_buf_scancode_15, align 8
  ret i64 %loadtmp46

then_566:                                         ; preds = %merge_565
  %loadtmp49 = load i64, ptr @kbd_buf_scancode_16, align 8
  ret i64 %loadtmp49

then_568:                                         ; preds = %merge_567
  %loadtmp52 = load i64, ptr @kbd_buf_scancode_17, align 8
  ret i64 %loadtmp52

then_570:                                         ; preds = %merge_569
  %loadtmp55 = load i64, ptr @kbd_buf_scancode_18, align 8
  ret i64 %loadtmp55

then_572:                                         ; preds = %merge_571
  %loadtmp58 = load i64, ptr @kbd_buf_scancode_19, align 8
  ret i64 %loadtmp58

then_574:                                         ; preds = %merge_573
  %loadtmp61 = load i64, ptr @kbd_buf_scancode_20, align 8
  ret i64 %loadtmp61

then_576:                                         ; preds = %merge_575
  %loadtmp64 = load i64, ptr @kbd_buf_scancode_21, align 8
  ret i64 %loadtmp64

then_578:                                         ; preds = %merge_577
  %loadtmp67 = load i64, ptr @kbd_buf_scancode_22, align 8
  ret i64 %loadtmp67

then_580:                                         ; preds = %merge_579
  %loadtmp70 = load i64, ptr @kbd_buf_scancode_23, align 8
  ret i64 %loadtmp70

then_582:                                         ; preds = %merge_581
  %loadtmp73 = load i64, ptr @kbd_buf_scancode_24, align 8
  ret i64 %loadtmp73

then_584:                                         ; preds = %merge_583
  %loadtmp76 = load i64, ptr @kbd_buf_scancode_25, align 8
  ret i64 %loadtmp76

then_586:                                         ; preds = %merge_585
  %loadtmp79 = load i64, ptr @kbd_buf_scancode_26, align 8
  ret i64 %loadtmp79

then_588:                                         ; preds = %merge_587
  %loadtmp82 = load i64, ptr @kbd_buf_scancode_27, align 8
  ret i64 %loadtmp82

then_590:                                         ; preds = %merge_589
  %loadtmp85 = load i64, ptr @kbd_buf_scancode_28, align 8
  ret i64 %loadtmp85

then_592:                                         ; preds = %merge_591
  %loadtmp88 = load i64, ptr @kbd_buf_scancode_29, align 8
  ret i64 %loadtmp88

then_594:                                         ; preds = %merge_593
  %loadtmp91 = load i64, ptr @kbd_buf_scancode_30, align 8
  ret i64 %loadtmp91

then_596:                                         ; preds = %merge_595
  %loadtmp94 = load i64, ptr @kbd_buf_scancode_31, align 8
  ret i64 %loadtmp94
}

define i64 @kbd_buf_get_ascii(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_598, label %merge_599

merge_599:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_600, label %merge_601

merge_601:                                        ; preds = %merge_599
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_602, label %merge_603

merge_603:                                        ; preds = %merge_601
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_604, label %merge_605

merge_605:                                        ; preds = %merge_603
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_606, label %merge_607

merge_607:                                        ; preds = %merge_605
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_608, label %merge_609

merge_609:                                        ; preds = %merge_607
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_610, label %merge_611

merge_611:                                        ; preds = %merge_609
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_612, label %merge_613

merge_613:                                        ; preds = %merge_611
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 8
  br i1 %cmpeqtmp24, label %then_614, label %merge_615

merge_615:                                        ; preds = %merge_613
  %"%idx.load26" = load i64, ptr %idx1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%idx.load26", 9
  br i1 %cmpeqtmp27, label %then_616, label %merge_617

merge_617:                                        ; preds = %merge_615
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 10
  br i1 %cmpeqtmp30, label %then_618, label %merge_619

merge_619:                                        ; preds = %merge_617
  %"%idx.load32" = load i64, ptr %idx1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%idx.load32", 11
  br i1 %cmpeqtmp33, label %then_620, label %merge_621

merge_621:                                        ; preds = %merge_619
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 12
  br i1 %cmpeqtmp36, label %then_622, label %merge_623

merge_623:                                        ; preds = %merge_621
  %"%idx.load38" = load i64, ptr %idx1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%idx.load38", 13
  br i1 %cmpeqtmp39, label %then_624, label %merge_625

merge_625:                                        ; preds = %merge_623
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 14
  br i1 %cmpeqtmp42, label %then_626, label %merge_627

merge_627:                                        ; preds = %merge_625
  %"%idx.load44" = load i64, ptr %idx1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%idx.load44", 15
  br i1 %cmpeqtmp45, label %then_628, label %merge_629

merge_629:                                        ; preds = %merge_627
  %"%idx.load47" = load i64, ptr %idx1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%idx.load47", 16
  br i1 %cmpeqtmp48, label %then_630, label %merge_631

merge_631:                                        ; preds = %merge_629
  %"%idx.load50" = load i64, ptr %idx1, align 8
  %cmpeqtmp51 = icmp eq i64 %"%idx.load50", 17
  br i1 %cmpeqtmp51, label %then_632, label %merge_633

merge_633:                                        ; preds = %merge_631
  %"%idx.load53" = load i64, ptr %idx1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%idx.load53", 18
  br i1 %cmpeqtmp54, label %then_634, label %merge_635

merge_635:                                        ; preds = %merge_633
  %"%idx.load56" = load i64, ptr %idx1, align 8
  %cmpeqtmp57 = icmp eq i64 %"%idx.load56", 19
  br i1 %cmpeqtmp57, label %then_636, label %merge_637

merge_637:                                        ; preds = %merge_635
  %"%idx.load59" = load i64, ptr %idx1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%idx.load59", 20
  br i1 %cmpeqtmp60, label %then_638, label %merge_639

merge_639:                                        ; preds = %merge_637
  %"%idx.load62" = load i64, ptr %idx1, align 8
  %cmpeqtmp63 = icmp eq i64 %"%idx.load62", 21
  br i1 %cmpeqtmp63, label %then_640, label %merge_641

merge_641:                                        ; preds = %merge_639
  %"%idx.load65" = load i64, ptr %idx1, align 8
  %cmpeqtmp66 = icmp eq i64 %"%idx.load65", 22
  br i1 %cmpeqtmp66, label %then_642, label %merge_643

merge_643:                                        ; preds = %merge_641
  %"%idx.load68" = load i64, ptr %idx1, align 8
  %cmpeqtmp69 = icmp eq i64 %"%idx.load68", 23
  br i1 %cmpeqtmp69, label %then_644, label %merge_645

merge_645:                                        ; preds = %merge_643
  %"%idx.load71" = load i64, ptr %idx1, align 8
  %cmpeqtmp72 = icmp eq i64 %"%idx.load71", 24
  br i1 %cmpeqtmp72, label %then_646, label %merge_647

merge_647:                                        ; preds = %merge_645
  %"%idx.load74" = load i64, ptr %idx1, align 8
  %cmpeqtmp75 = icmp eq i64 %"%idx.load74", 25
  br i1 %cmpeqtmp75, label %then_648, label %merge_649

merge_649:                                        ; preds = %merge_647
  %"%idx.load77" = load i64, ptr %idx1, align 8
  %cmpeqtmp78 = icmp eq i64 %"%idx.load77", 26
  br i1 %cmpeqtmp78, label %then_650, label %merge_651

merge_651:                                        ; preds = %merge_649
  %"%idx.load80" = load i64, ptr %idx1, align 8
  %cmpeqtmp81 = icmp eq i64 %"%idx.load80", 27
  br i1 %cmpeqtmp81, label %then_652, label %merge_653

merge_653:                                        ; preds = %merge_651
  %"%idx.load83" = load i64, ptr %idx1, align 8
  %cmpeqtmp84 = icmp eq i64 %"%idx.load83", 28
  br i1 %cmpeqtmp84, label %then_654, label %merge_655

merge_655:                                        ; preds = %merge_653
  %"%idx.load86" = load i64, ptr %idx1, align 8
  %cmpeqtmp87 = icmp eq i64 %"%idx.load86", 29
  br i1 %cmpeqtmp87, label %then_656, label %merge_657

merge_657:                                        ; preds = %merge_655
  %"%idx.load89" = load i64, ptr %idx1, align 8
  %cmpeqtmp90 = icmp eq i64 %"%idx.load89", 30
  br i1 %cmpeqtmp90, label %then_658, label %merge_659

merge_659:                                        ; preds = %merge_657
  %"%idx.load92" = load i64, ptr %idx1, align 8
  %cmpeqtmp93 = icmp eq i64 %"%idx.load92", 31
  br i1 %cmpeqtmp93, label %then_660, label %merge_661

merge_661:                                        ; preds = %merge_659
  ret i64 0

then_598:                                         ; preds = %entry
  %loadtmp = load i64, ptr @kbd_buf_ascii_0, align 8
  ret i64 %loadtmp

then_600:                                         ; preds = %merge_599
  %loadtmp4 = load i64, ptr @kbd_buf_ascii_1, align 8
  ret i64 %loadtmp4

then_602:                                         ; preds = %merge_601
  %loadtmp7 = load i64, ptr @kbd_buf_ascii_2, align 8
  ret i64 %loadtmp7

then_604:                                         ; preds = %merge_603
  %loadtmp10 = load i64, ptr @kbd_buf_ascii_3, align 8
  ret i64 %loadtmp10

then_606:                                         ; preds = %merge_605
  %loadtmp13 = load i64, ptr @kbd_buf_ascii_4, align 8
  ret i64 %loadtmp13

then_608:                                         ; preds = %merge_607
  %loadtmp16 = load i64, ptr @kbd_buf_ascii_5, align 8
  ret i64 %loadtmp16

then_610:                                         ; preds = %merge_609
  %loadtmp19 = load i64, ptr @kbd_buf_ascii_6, align 8
  ret i64 %loadtmp19

then_612:                                         ; preds = %merge_611
  %loadtmp22 = load i64, ptr @kbd_buf_ascii_7, align 8
  ret i64 %loadtmp22

then_614:                                         ; preds = %merge_613
  %loadtmp25 = load i64, ptr @kbd_buf_ascii_8, align 8
  ret i64 %loadtmp25

then_616:                                         ; preds = %merge_615
  %loadtmp28 = load i64, ptr @kbd_buf_ascii_9, align 8
  ret i64 %loadtmp28

then_618:                                         ; preds = %merge_617
  %loadtmp31 = load i64, ptr @kbd_buf_ascii_10, align 8
  ret i64 %loadtmp31

then_620:                                         ; preds = %merge_619
  %loadtmp34 = load i64, ptr @kbd_buf_ascii_11, align 8
  ret i64 %loadtmp34

then_622:                                         ; preds = %merge_621
  %loadtmp37 = load i64, ptr @kbd_buf_ascii_12, align 8
  ret i64 %loadtmp37

then_624:                                         ; preds = %merge_623
  %loadtmp40 = load i64, ptr @kbd_buf_ascii_13, align 8
  ret i64 %loadtmp40

then_626:                                         ; preds = %merge_625
  %loadtmp43 = load i64, ptr @kbd_buf_ascii_14, align 8
  ret i64 %loadtmp43

then_628:                                         ; preds = %merge_627
  %loadtmp46 = load i64, ptr @kbd_buf_ascii_15, align 8
  ret i64 %loadtmp46

then_630:                                         ; preds = %merge_629
  %loadtmp49 = load i64, ptr @kbd_buf_ascii_16, align 8
  ret i64 %loadtmp49

then_632:                                         ; preds = %merge_631
  %loadtmp52 = load i64, ptr @kbd_buf_ascii_17, align 8
  ret i64 %loadtmp52

then_634:                                         ; preds = %merge_633
  %loadtmp55 = load i64, ptr @kbd_buf_ascii_18, align 8
  ret i64 %loadtmp55

then_636:                                         ; preds = %merge_635
  %loadtmp58 = load i64, ptr @kbd_buf_ascii_19, align 8
  ret i64 %loadtmp58

then_638:                                         ; preds = %merge_637
  %loadtmp61 = load i64, ptr @kbd_buf_ascii_20, align 8
  ret i64 %loadtmp61

then_640:                                         ; preds = %merge_639
  %loadtmp64 = load i64, ptr @kbd_buf_ascii_21, align 8
  ret i64 %loadtmp64

then_642:                                         ; preds = %merge_641
  %loadtmp67 = load i64, ptr @kbd_buf_ascii_22, align 8
  ret i64 %loadtmp67

then_644:                                         ; preds = %merge_643
  %loadtmp70 = load i64, ptr @kbd_buf_ascii_23, align 8
  ret i64 %loadtmp70

then_646:                                         ; preds = %merge_645
  %loadtmp73 = load i64, ptr @kbd_buf_ascii_24, align 8
  ret i64 %loadtmp73

then_648:                                         ; preds = %merge_647
  %loadtmp76 = load i64, ptr @kbd_buf_ascii_25, align 8
  ret i64 %loadtmp76

then_650:                                         ; preds = %merge_649
  %loadtmp79 = load i64, ptr @kbd_buf_ascii_26, align 8
  ret i64 %loadtmp79

then_652:                                         ; preds = %merge_651
  %loadtmp82 = load i64, ptr @kbd_buf_ascii_27, align 8
  ret i64 %loadtmp82

then_654:                                         ; preds = %merge_653
  %loadtmp85 = load i64, ptr @kbd_buf_ascii_28, align 8
  ret i64 %loadtmp85

then_656:                                         ; preds = %merge_655
  %loadtmp88 = load i64, ptr @kbd_buf_ascii_29, align 8
  ret i64 %loadtmp88

then_658:                                         ; preds = %merge_657
  %loadtmp91 = load i64, ptr @kbd_buf_ascii_30, align 8
  ret i64 %loadtmp91

then_660:                                         ; preds = %merge_659
  %loadtmp94 = load i64, ptr @kbd_buf_ascii_31, align 8
  ret i64 %loadtmp94
}

define i64 @ps2_kbd_push(i64 %scancode, i64 %ascii) {
entry:
  %"%kbd_buf_count" = alloca i64, align 8
  %"%kbd_buf_head" = alloca i64, align 8
  %ascii2 = alloca i64, align 8
  store i64 %ascii, ptr %ascii2, align 8
  %scancode1 = alloca i64, align 8
  store i64 %scancode, ptr %scancode1, align 8
  %"%kbd_buf_count.load" = load i64, ptr @kbd_buf_count, align 8
  %cmpgetmp = icmp sge i64 %"%kbd_buf_count.load", 32
  br i1 %cmpgetmp, label %then_662, label %merge_663

merge_663:                                        ; preds = %entry
  %"%kbd_buf_head.load" = load i64, ptr @kbd_buf_head, align 8
  %"%scancode.load" = load i64, ptr %scancode1, align 8
  %calltmp = call i64 @kbd_buf_set_sc(i64 %"%kbd_buf_head.load", i64 %"%scancode.load")
  %"%kbd_buf_head.load3" = load i64, ptr @kbd_buf_head, align 8
  %"%ascii.load" = load i64, ptr %ascii2, align 8
  %calltmp4 = call i64 @kbd_buf_set_ascii(i64 %"%kbd_buf_head.load3", i64 %"%ascii.load")
  %"%kbd_buf_head.load5" = load i64, ptr @kbd_buf_head, align 8
  %addtmp = add i64 %"%kbd_buf_head.load5", 1
  %andtmp = and i64 %addtmp, 31
  store i64 %andtmp, ptr %"%kbd_buf_head", align 8
  %"%kbd_buf_count.load6" = load i64, ptr @kbd_buf_count, align 8
  %addtmp7 = add i64 %"%kbd_buf_count.load6", 1
  store i64 %addtmp7, ptr %"%kbd_buf_count", align 8
  ret i64 0

then_662:                                         ; preds = %entry
  ret i64 -1
}

define i64 @ps2_kbd_pop_ascii() {
entry:
  %"%kbd_buf_count" = alloca i64, align 8
  %"%kbd_buf_tail" = alloca i64, align 8
  %"%ascii" = alloca i64, align 8
  %"%kbd_buf_count.load" = load i64, ptr @kbd_buf_count, align 8
  %cmpeqtmp = icmp eq i64 %"%kbd_buf_count.load", 0
  br i1 %cmpeqtmp, label %then_664, label %merge_665

merge_665:                                        ; preds = %entry
  %"%kbd_buf_tail.load" = load i64, ptr @kbd_buf_tail, align 8
  %calltmp = call i64 @kbd_buf_get_ascii(i64 %"%kbd_buf_tail.load")
  store i64 %calltmp, ptr %"%ascii", align 8
  %"%kbd_buf_tail.load1" = load i64, ptr @kbd_buf_tail, align 8
  %addtmp = add i64 %"%kbd_buf_tail.load1", 1
  %andtmp = and i64 %addtmp, 31
  store i64 %andtmp, ptr %"%kbd_buf_tail", align 8
  %"%kbd_buf_count.load2" = load i64, ptr @kbd_buf_count, align 8
  %subtmp = sub i64 %"%kbd_buf_count.load2", 1
  store i64 %subtmp, ptr %"%kbd_buf_count", align 8
  %loadtmp = load i64, ptr %"%ascii", align 8
  ret i64 %loadtmp

then_664:                                         ; preds = %entry
  ret i64 -1
}

define i64 @ps2_kbd_pop_scancode() {
entry:
  %"%kbd_buf_count" = alloca i64, align 8
  %"%kbd_buf_tail" = alloca i64, align 8
  %"%sc" = alloca i64, align 8
  %"%kbd_buf_count.load" = load i64, ptr @kbd_buf_count, align 8
  %cmpeqtmp = icmp eq i64 %"%kbd_buf_count.load", 0
  br i1 %cmpeqtmp, label %then_666, label %merge_667

merge_667:                                        ; preds = %entry
  %"%kbd_buf_tail.load" = load i64, ptr @kbd_buf_tail, align 8
  %calltmp = call i64 @kbd_buf_get_sc(i64 %"%kbd_buf_tail.load")
  store i64 %calltmp, ptr %"%sc", align 8
  %"%kbd_buf_tail.load1" = load i64, ptr @kbd_buf_tail, align 8
  %addtmp = add i64 %"%kbd_buf_tail.load1", 1
  %andtmp = and i64 %addtmp, 31
  store i64 %andtmp, ptr %"%kbd_buf_tail", align 8
  %"%kbd_buf_count.load2" = load i64, ptr @kbd_buf_count, align 8
  %subtmp = sub i64 %"%kbd_buf_count.load2", 1
  store i64 %subtmp, ptr %"%kbd_buf_count", align 8
  %loadtmp = load i64, ptr %"%sc", align 8
  ret i64 %loadtmp

then_666:                                         ; preds = %entry
  ret i64 -1
}

define i64 @ps2_kbd_has_key() {
entry:
  %loadtmp = load i64, ptr @kbd_buf_count, align 8
  ret i64 %loadtmp
}

define i64 @ps2_kbd_handle_byte(i64 %byte) {
entry:
  %"%ascii" = alloca i64, align 8
  %"%shift" = alloca i64, align 8
  %"%kbd_scrolllock" = alloca i64, align 8
  %"%kbd_numlock" = alloca i64, align 8
  %"%kbd_capslock" = alloca i64, align 8
  %"%kbd_alt_right" = alloca i64, align 8
  %"%kbd_alt_left" = alloca i64, align 8
  %"%kbd_ctrl_right" = alloca i64, align 8
  %"%kbd_ctrl_left" = alloca i64, align 8
  %"%kbd_shift_right" = alloca i64, align 8
  %"%kbd_shift_left" = alloca i64, align 8
  %"%is_extended" = alloca i64, align 8
  %"%is_release" = alloca i64, align 8
  %"%scancode" = alloca i64, align 8
  %"%kbd_release" = alloca i64, align 8
  %"%kbd_extended" = alloca i64, align 8
  %"%ps2_kbd_irq_count" = alloca i64, align 8
  %byte1 = alloca i64, align 8
  store i64 %byte, ptr %byte1, align 8
  %"%ps2_kbd_irq_count.load" = load i64, ptr @ps2_kbd_irq_count, align 8
  %addtmp = add i64 %"%ps2_kbd_irq_count.load", 1
  store i64 %addtmp, ptr %"%ps2_kbd_irq_count", align 8
  %"%byte.load" = load i64, ptr %byte1, align 8
  %"%SC2_EXTENDED.load" = load i64, ptr @SC2_EXTENDED, align 8
  %cmpeqtmp = icmp eq i64 %"%byte.load", %"%SC2_EXTENDED.load"
  br i1 %cmpeqtmp, label %then_668, label %merge_669

else_676:                                         ; preds = %then_672
  store i64 1, ptr %"%kbd_shift_left", align 8
  br label %merge_675

else_681:                                         ; preds = %then_677
  store i64 1, ptr %"%kbd_shift_right", align 8
  br label %merge_680

else_686:                                         ; preds = %then_682
  %"%is_release.load17" = load i64, ptr %"%is_release", align 8
  %cmpeqtmp18 = icmp eq i64 %"%is_release.load17", 1
  br i1 %cmpeqtmp18, label %then_690, label %else_692

else_689:                                         ; preds = %then_684
  store i64 1, ptr %"%kbd_ctrl_left", align 8
  br label %merge_688

else_692:                                         ; preds = %else_686
  store i64 1, ptr %"%kbd_ctrl_right", align 8
  br label %merge_691

else_697:                                         ; preds = %then_693
  %"%is_release.load25" = load i64, ptr %"%is_release", align 8
  %cmpeqtmp26 = icmp eq i64 %"%is_release.load25", 1
  br i1 %cmpeqtmp26, label %then_701, label %else_703

else_700:                                         ; preds = %then_695
  store i64 1, ptr %"%kbd_alt_left", align 8
  br label %merge_699

else_703:                                         ; preds = %else_697
  store i64 1, ptr %"%kbd_alt_right", align 8
  br label %merge_702

else_722:                                         ; preds = %merge_719
  %"%scancode.load48" = load i64, ptr %"%scancode", align 8
  %calltmp49 = call i64 @ps2_sc2_to_ascii(i64 %"%scancode.load48")
  store i64 %calltmp49, ptr %"%ascii", align 8
  br label %merge_721

else_727:                                         ; preds = %then_723
  %"%ascii.load55" = load i64, ptr %"%ascii", align 8
  %cmpgetmp56 = icmp sge i64 %"%ascii.load55", 65
  br i1 %cmpgetmp56, label %then_730, label %merge_731

merge_669:                                        ; preds = %entry
  %"%byte.load2" = load i64, ptr %byte1, align 8
  %"%SC2_RELEASE.load" = load i64, ptr @SC2_RELEASE, align 8
  %cmpeqtmp3 = icmp eq i64 %"%byte.load2", %"%SC2_RELEASE.load"
  br i1 %cmpeqtmp3, label %then_670, label %merge_671

merge_671:                                        ; preds = %merge_669
  %"%1737.load" = load i64, ptr %byte1, align 8
  store i64 %"%1737.load", ptr %"%scancode", align 8
  %"%1739.load" = load i64, ptr %"%kbd_release", align 8
  store i64 %"%1739.load", ptr %"%is_release", align 8
  %"%1741.load" = load i64, ptr %"%kbd_extended", align 8
  store i64 %"%1741.load", ptr %"%is_extended", align 8
  store i64 0, ptr %"%kbd_release", align 8
  store i64 0, ptr %"%kbd_extended", align 8
  %"%scancode.load" = load i64, ptr %"%scancode", align 8
  %cmpeqtmp4 = icmp eq i64 %"%scancode.load", 18
  br i1 %cmpeqtmp4, label %then_672, label %merge_673

merge_673:                                        ; preds = %merge_671
  %"%scancode.load6" = load i64, ptr %"%scancode", align 8
  %cmpeqtmp7 = icmp eq i64 %"%scancode.load6", 89
  br i1 %cmpeqtmp7, label %then_677, label %merge_678

merge_675:                                        ; preds = %else_676, %then_674
  ret i64 0

merge_678:                                        ; preds = %merge_673
  %"%scancode.load10" = load i64, ptr %"%scancode", align 8
  %cmpeqtmp11 = icmp eq i64 %"%scancode.load10", 20
  br i1 %cmpeqtmp11, label %then_682, label %merge_683

merge_680:                                        ; preds = %else_681, %then_679
  ret i64 0

merge_683:                                        ; preds = %merge_678
  %"%scancode.load13" = load i64, ptr %"%scancode", align 8
  %cmpeqtmp14 = icmp eq i64 %"%scancode.load13", 17
  br i1 %cmpeqtmp14, label %then_693, label %merge_694

merge_685:                                        ; preds = %merge_691, %merge_688
  ret i64 0

merge_688:                                        ; preds = %else_689, %then_687
  br label %merge_685

merge_691:                                        ; preds = %else_692, %then_690
  br label %merge_685

merge_694:                                        ; preds = %merge_683
  %"%scancode.load21" = load i64, ptr %"%scancode", align 8
  %cmpeqtmp22 = icmp eq i64 %"%scancode.load21", 88
  br i1 %cmpeqtmp22, label %then_704, label %merge_705

merge_696:                                        ; preds = %merge_702, %merge_699
  ret i64 0

merge_699:                                        ; preds = %else_700, %then_698
  br label %merge_696

merge_702:                                        ; preds = %else_703, %then_701
  br label %merge_696

merge_705:                                        ; preds = %merge_694
  %"%scancode.load29" = load i64, ptr %"%scancode", align 8
  %cmpeqtmp30 = icmp eq i64 %"%scancode.load29", 119
  br i1 %cmpeqtmp30, label %then_708, label %merge_709

merge_707:                                        ; preds = %then_706, %then_704
  ret i64 0

merge_709:                                        ; preds = %merge_705
  %"%scancode.load33" = load i64, ptr %"%scancode", align 8
  %cmpeqtmp34 = icmp eq i64 %"%scancode.load33", 126
  br i1 %cmpeqtmp34, label %then_712, label %merge_713

merge_711:                                        ; preds = %then_710, %then_708
  ret i64 0

merge_713:                                        ; preds = %merge_709
  %"%is_release.load39" = load i64, ptr %"%is_release", align 8
  %cmpeqtmp40 = icmp eq i64 %"%is_release.load39", 1
  br i1 %cmpeqtmp40, label %then_716, label %merge_717

merge_715:                                        ; preds = %then_714, %then_712
  ret i64 0

merge_717:                                        ; preds = %merge_713
  %"%kbd_shift_left.load" = load i64, ptr %"%kbd_shift_left", align 8
  %"%kbd_shift_right.load" = load i64, ptr %"%kbd_shift_right", align 8
  %addtmp43 = add i64 %"%kbd_shift_left.load", %"%kbd_shift_right.load"
  store i64 %addtmp43, ptr %"%shift", align 8
  %"%shift.load" = load i64, ptr %"%shift", align 8
  %cmpgttmp = icmp sgt i64 %"%shift.load", 1
  br i1 %cmpgttmp, label %then_718, label %merge_719

merge_719:                                        ; preds = %then_718, %merge_717
  store i64 0, ptr %"%ascii", align 8
  %"%shift.load44" = load i64, ptr %"%shift", align 8
  %cmpeqtmp45 = icmp eq i64 %"%shift.load44", 1
  br i1 %cmpeqtmp45, label %then_720, label %else_722

merge_721:                                        ; preds = %else_722, %then_720
  %"%kbd_capslock.load50" = load i64, ptr %"%kbd_capslock", align 8
  %cmpeqtmp51 = icmp eq i64 %"%kbd_capslock.load50", 1
  br i1 %cmpeqtmp51, label %then_723, label %merge_724

merge_724:                                        ; preds = %merge_726, %merge_721
  %"%ascii.load52" = load i64, ptr %"%ascii", align 8
  %cmpgttmp53 = icmp sgt i64 %"%ascii.load52", 0
  br i1 %cmpgttmp53, label %then_734, label %merge_735

merge_726:                                        ; preds = %merge_731, %merge_729
  br label %merge_724

merge_729:                                        ; preds = %then_728, %then_725
  br label %merge_726

merge_731:                                        ; preds = %merge_733, %else_727
  br label %merge_726

merge_733:                                        ; preds = %then_732, %then_730
  br label %merge_731

merge_735:                                        ; preds = %then_734, %merge_724
  %loadtmp = load i64, ptr %"%ascii", align 8
  ret i64 %loadtmp

then_668:                                         ; preds = %entry
  store i64 1, ptr %"%kbd_extended", align 8
  ret i64 0

then_670:                                         ; preds = %merge_669
  store i64 1, ptr %"%kbd_release", align 8
  ret i64 0

then_672:                                         ; preds = %merge_671
  %"%is_release.load" = load i64, ptr %"%is_release", align 8
  %cmpeqtmp5 = icmp eq i64 %"%is_release.load", 1
  br i1 %cmpeqtmp5, label %then_674, label %else_676

then_674:                                         ; preds = %then_672
  store i64 0, ptr %"%kbd_shift_left", align 8
  br label %merge_675

then_677:                                         ; preds = %merge_673
  %"%is_release.load8" = load i64, ptr %"%is_release", align 8
  %cmpeqtmp9 = icmp eq i64 %"%is_release.load8", 1
  br i1 %cmpeqtmp9, label %then_679, label %else_681

then_679:                                         ; preds = %then_677
  store i64 0, ptr %"%kbd_shift_right", align 8
  br label %merge_680

then_682:                                         ; preds = %merge_678
  %"%is_extended.load" = load i64, ptr %"%is_extended", align 8
  %cmpeqtmp12 = icmp eq i64 %"%is_extended.load", 0
  br i1 %cmpeqtmp12, label %then_684, label %else_686

then_684:                                         ; preds = %then_682
  %"%is_release.load15" = load i64, ptr %"%is_release", align 8
  %cmpeqtmp16 = icmp eq i64 %"%is_release.load15", 1
  br i1 %cmpeqtmp16, label %then_687, label %else_689

then_687:                                         ; preds = %then_684
  store i64 0, ptr %"%kbd_ctrl_left", align 8
  br label %merge_688

then_690:                                         ; preds = %else_686
  store i64 0, ptr %"%kbd_ctrl_right", align 8
  br label %merge_691

then_693:                                         ; preds = %merge_683
  %"%is_extended.load19" = load i64, ptr %"%is_extended", align 8
  %cmpeqtmp20 = icmp eq i64 %"%is_extended.load19", 0
  br i1 %cmpeqtmp20, label %then_695, label %else_697

then_695:                                         ; preds = %then_693
  %"%is_release.load23" = load i64, ptr %"%is_release", align 8
  %cmpeqtmp24 = icmp eq i64 %"%is_release.load23", 1
  br i1 %cmpeqtmp24, label %then_698, label %else_700

then_698:                                         ; preds = %then_695
  store i64 0, ptr %"%kbd_alt_left", align 8
  br label %merge_699

then_701:                                         ; preds = %else_697
  store i64 0, ptr %"%kbd_alt_right", align 8
  br label %merge_702

then_704:                                         ; preds = %merge_694
  %"%is_release.load27" = load i64, ptr %"%is_release", align 8
  %cmpeqtmp28 = icmp eq i64 %"%is_release.load27", 0
  br i1 %cmpeqtmp28, label %then_706, label %merge_707

then_706:                                         ; preds = %then_704
  %"%kbd_capslock.load" = load i64, ptr @kbd_capslock, align 8
  %subtmp = sub i64 1, %"%kbd_capslock.load"
  store i64 %subtmp, ptr %"%kbd_capslock", align 8
  %calltmp = call i64 @ps2_update_leds()
  br label %merge_707

then_708:                                         ; preds = %merge_705
  %"%is_release.load31" = load i64, ptr %"%is_release", align 8
  %cmpeqtmp32 = icmp eq i64 %"%is_release.load31", 0
  br i1 %cmpeqtmp32, label %then_710, label %merge_711

then_710:                                         ; preds = %then_708
  %"%kbd_numlock.load" = load i64, ptr @kbd_numlock, align 8
  %subtmp35 = sub i64 1, %"%kbd_numlock.load"
  store i64 %subtmp35, ptr %"%kbd_numlock", align 8
  %calltmp36 = call i64 @ps2_update_leds()
  br label %merge_711

then_712:                                         ; preds = %merge_709
  %"%is_release.load37" = load i64, ptr %"%is_release", align 8
  %cmpeqtmp38 = icmp eq i64 %"%is_release.load37", 0
  br i1 %cmpeqtmp38, label %then_714, label %merge_715

then_714:                                         ; preds = %then_712
  %"%kbd_scrolllock.load" = load i64, ptr @kbd_scrolllock, align 8
  %subtmp41 = sub i64 1, %"%kbd_scrolllock.load"
  store i64 %subtmp41, ptr %"%kbd_scrolllock", align 8
  %calltmp42 = call i64 @ps2_update_leds()
  br label %merge_715

then_716:                                         ; preds = %merge_713
  ret i64 0

then_718:                                         ; preds = %merge_717
  store i64 1, ptr %"%shift", align 8
  br label %merge_719

then_720:                                         ; preds = %merge_719
  %"%scancode.load46" = load i64, ptr %"%scancode", align 8
  %calltmp47 = call i64 @ps2_sc2_to_ascii_shift(i64 %"%scancode.load46")
  store i64 %calltmp47, ptr %"%ascii", align 8
  br label %merge_721

then_723:                                         ; preds = %merge_721
  %"%ascii.load" = load i64, ptr %"%ascii", align 8
  %cmpgetmp = icmp sge i64 %"%ascii.load", 97
  br i1 %cmpgetmp, label %then_725, label %else_727

then_725:                                         ; preds = %then_723
  %"%ascii.load54" = load i64, ptr %"%ascii", align 8
  %cmpletmp = icmp sle i64 %"%ascii.load54", 122
  br i1 %cmpletmp, label %then_728, label %merge_729

then_728:                                         ; preds = %then_725
  %"%ascii.load57" = load i64, ptr %"%ascii", align 8
  %subtmp58 = sub i64 %"%ascii.load57", 32
  store i64 %subtmp58, ptr %"%ascii", align 8
  br label %merge_729

then_730:                                         ; preds = %else_727
  %"%ascii.load59" = load i64, ptr %"%ascii", align 8
  %cmpletmp60 = icmp sle i64 %"%ascii.load59", 90
  br i1 %cmpletmp60, label %then_732, label %merge_733

then_732:                                         ; preds = %then_730
  %"%ascii.load61" = load i64, ptr %"%ascii", align 8
  %addtmp62 = add i64 %"%ascii.load61", 32
  store i64 %addtmp62, ptr %"%ascii", align 8
  br label %merge_733

then_734:                                         ; preds = %merge_724
  %"%scancode.load63" = load i64, ptr %"%scancode", align 8
  %"%ascii.load64" = load i64, ptr %"%ascii", align 8
  %calltmp65 = call i64 @ps2_kbd_push(i64 %"%scancode.load63", i64 %"%ascii.load64")
  br label %merge_735
}

define i64 @ps2_update_leds() {
entry:
  %"%kbd_leds" = alloca i64, align 8
  %"%led_val" = alloca i64, align 8
  %"%kbd_numlock.load" = load i64, ptr @kbd_numlock, align 8
  %shltmp = shl i64 %"%kbd_numlock.load", 1
  %"%kbd_scrolllock.load" = load i64, ptr @kbd_scrolllock, align 8
  %addtmp = add i64 %"%kbd_scrolllock.load", %shltmp
  %"%kbd_capslock.load" = load i64, ptr @kbd_capslock, align 8
  %shltmp1 = shl i64 %"%kbd_capslock.load", 2
  %addtmp2 = add i64 %addtmp, %shltmp1
  store i64 %addtmp2, ptr %"%led_val", align 8
  %"%PS2_DEV_SET_LEDS.load" = load i64, ptr @PS2_DEV_SET_LEDS, align 8
  %calltmp = call i64 @ps2_send_device1_cmd(i64 %"%PS2_DEV_SET_LEDS.load")
  %"%led_val.load" = load i64, ptr %"%led_val", align 8
  %calltmp3 = call i64 @ps2_send_device1_cmd(i64 %"%led_val.load")
  %"%1855.load" = load i64, ptr %"%led_val", align 8
  store i64 %"%1855.load", ptr %"%kbd_leds", align 8
  ret i64 0
}

define i64 @ps2_mouse_handle_byte(i64 %byte) {
entry:
  %"%mouse_byte3" = alloca i64, align 8
  %"%mouse_byte2" = alloca i64, align 8
  %"%mouse_byte1" = alloca i64, align 8
  %"%mouse_byte_idx" = alloca i64, align 8
  %"%mouse_byte0" = alloca i64, align 8
  %"%ps2_mouse_irq_count" = alloca i64, align 8
  %byte1 = alloca i64, align 8
  store i64 %byte, ptr %byte1, align 8
  %"%ps2_mouse_irq_count.load" = load i64, ptr @ps2_mouse_irq_count, align 8
  %addtmp = add i64 %"%ps2_mouse_irq_count.load", 1
  store i64 %addtmp, ptr %"%ps2_mouse_irq_count", align 8
  %"%mouse_byte_idx.load" = load i64, ptr @mouse_byte_idx, align 8
  %cmpeqtmp = icmp eq i64 %"%mouse_byte_idx.load", 0
  br i1 %cmpeqtmp, label %then_736, label %merge_737

merge_737:                                        ; preds = %entry
  %"%mouse_byte_idx.load3" = load i64, ptr @mouse_byte_idx, align 8
  %cmpeqtmp4 = icmp eq i64 %"%mouse_byte_idx.load3", 1
  br i1 %cmpeqtmp4, label %then_740, label %merge_741

merge_739:                                        ; preds = %then_736
  %"%1869.load" = load i64, ptr %byte1, align 8
  store i64 %"%1869.load", ptr %"%mouse_byte0", align 8
  store i64 1, ptr %"%mouse_byte_idx", align 8
  ret i64 0

merge_741:                                        ; preds = %merge_737
  %"%mouse_byte_idx.load5" = load i64, ptr %"%mouse_byte_idx", align 8
  %cmpeqtmp6 = icmp eq i64 %"%mouse_byte_idx.load5", 2
  br i1 %cmpeqtmp6, label %then_742, label %merge_743

merge_743:                                        ; preds = %merge_741
  %"%mouse_byte_idx.load8" = load i64, ptr %"%mouse_byte_idx", align 8
  %cmpeqtmp9 = icmp eq i64 %"%mouse_byte_idx.load8", 3
  br i1 %cmpeqtmp9, label %then_746, label %merge_747

merge_745:                                        ; preds = %then_742
  %calltmp = call i64 @ps2_mouse_process_packet()
  store i64 0, ptr %"%mouse_byte_idx", align 8
  ret i64 0

merge_747:                                        ; preds = %merge_743
  store i64 0, ptr %"%mouse_byte_idx", align 8
  ret i64 0

then_736:                                         ; preds = %entry
  %"%byte.load" = load i64, ptr %byte1, align 8
  %shrtmp = lshr i64 %"%byte.load", 3
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp2 = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp2, label %then_738, label %merge_739

then_738:                                         ; preds = %then_736
  ret i64 0

then_740:                                         ; preds = %merge_737
  %"%1875.load" = load i64, ptr %byte1, align 8
  store i64 %"%1875.load", ptr %"%mouse_byte1", align 8
  store i64 2, ptr %"%mouse_byte_idx", align 8
  ret i64 0

then_742:                                         ; preds = %merge_741
  %"%1881.load" = load i64, ptr %byte1, align 8
  store i64 %"%1881.load", ptr %"%mouse_byte2", align 8
  %"%mouse_has_wheel.load" = load i64, ptr @mouse_has_wheel, align 8
  %cmpeqtmp7 = icmp eq i64 %"%mouse_has_wheel.load", 1
  br i1 %cmpeqtmp7, label %then_744, label %merge_745

then_744:                                         ; preds = %then_742
  store i64 3, ptr %"%mouse_byte_idx", align 8
  ret i64 0

then_746:                                         ; preds = %merge_743
  %"%1892.load" = load i64, ptr %byte1, align 8
  store i64 %"%1892.load", ptr %"%mouse_byte3", align 8
  %calltmp10 = call i64 @ps2_mouse_process_packet()
  store i64 0, ptr %"%mouse_byte_idx", align 8
  ret i64 0
}

define i64 @ps2_mouse_process_packet() {
entry:
  %"%mouse_wheel" = alloca i64, align 8
  %"%w" = alloca i64, align 8
  %"%mouse_y" = alloca i64, align 8
  %"%mouse_x" = alloca i64, align 8
  %"%mouse_buttons" = alloca i64, align 8
  %"%dy" = alloca i64, align 8
  %"%dx" = alloca i64, align 8
  %"%b0" = alloca i64, align 8
  %"%1899.load" = load i64, ptr @mouse_byte0, align 8
  store i64 %"%1899.load", ptr %"%b0", align 8
  %"%1901.load" = load i64, ptr @mouse_byte1, align 8
  store i64 %"%1901.load", ptr %"%dx", align 8
  %"%1903.load" = load i64, ptr @mouse_byte2, align 8
  store i64 %"%1903.load", ptr %"%dy", align 8
  %"%b0.load" = load i64, ptr %"%b0", align 8
  %andtmp = and i64 %"%b0.load", 7
  store i64 %andtmp, ptr %"%mouse_buttons", align 8
  %"%b0.load1" = load i64, ptr %"%b0", align 8
  %shrtmp = lshr i64 %"%b0.load1", 6
  %andtmp2 = and i64 %shrtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp2, 1
  br i1 %cmpeqtmp, label %then_748, label %merge_749

merge_749:                                        ; preds = %entry
  %"%b0.load3" = load i64, ptr %"%b0", align 8
  %shrtmp4 = lshr i64 %"%b0.load3", 7
  %andtmp5 = and i64 %shrtmp4, 1
  %cmpeqtmp6 = icmp eq i64 %andtmp5, 1
  br i1 %cmpeqtmp6, label %then_750, label %merge_751

merge_751:                                        ; preds = %merge_749
  %"%b0.load7" = load i64, ptr %"%b0", align 8
  %shrtmp8 = lshr i64 %"%b0.load7", 4
  %andtmp9 = and i64 %shrtmp8, 1
  %cmpeqtmp10 = icmp eq i64 %andtmp9, 1
  br i1 %cmpeqtmp10, label %then_752, label %merge_753

merge_753:                                        ; preds = %then_752, %merge_751
  %"%b0.load11" = load i64, ptr %"%b0", align 8
  %shrtmp12 = lshr i64 %"%b0.load11", 5
  %andtmp13 = and i64 %shrtmp12, 1
  %cmpeqtmp14 = icmp eq i64 %andtmp13, 1
  br i1 %cmpeqtmp14, label %then_754, label %merge_755

merge_755:                                        ; preds = %then_754, %merge_753
  %"%dy.load16" = load i64, ptr %"%dy", align 8
  %subtmp17 = sub i64 0, %"%dy.load16"
  store i64 %subtmp17, ptr %"%dy", align 8
  %"%mouse_x.load" = load i64, ptr @mouse_x, align 8
  %"%dx.load18" = load i64, ptr %"%dx", align 8
  %addtmp = add i64 %"%mouse_x.load", %"%dx.load18"
  store i64 %addtmp, ptr %"%mouse_x", align 8
  %"%mouse_y.load" = load i64, ptr @mouse_y, align 8
  %"%dy.load19" = load i64, ptr %"%dy", align 8
  %addtmp20 = add i64 %"%mouse_y.load", %"%dy.load19"
  store i64 %addtmp20, ptr %"%mouse_y", align 8
  %"%mouse_x.load21" = load i64, ptr %"%mouse_x", align 8
  %cmplttmp = icmp slt i64 %"%mouse_x.load21", 0
  br i1 %cmplttmp, label %then_756, label %merge_757

merge_757:                                        ; preds = %then_756, %merge_755
  %"%mouse_y.load22" = load i64, ptr %"%mouse_y", align 8
  %cmplttmp23 = icmp slt i64 %"%mouse_y.load22", 0
  br i1 %cmplttmp23, label %then_758, label %merge_759

merge_759:                                        ; preds = %then_758, %merge_757
  %"%mouse_x.load24" = load i64, ptr %"%mouse_x", align 8
  %"%mouse_max_x.load" = load i64, ptr @mouse_max_x, align 8
  %cmpgttmp = icmp sgt i64 %"%mouse_x.load24", %"%mouse_max_x.load"
  br i1 %cmpgttmp, label %then_760, label %merge_761

merge_761:                                        ; preds = %then_760, %merge_759
  %"%mouse_y.load25" = load i64, ptr %"%mouse_y", align 8
  %"%mouse_max_y.load" = load i64, ptr @mouse_max_y, align 8
  %cmpgttmp26 = icmp sgt i64 %"%mouse_y.load25", %"%mouse_max_y.load"
  br i1 %cmpgttmp26, label %then_762, label %merge_763

merge_763:                                        ; preds = %then_762, %merge_761
  %"%mouse_has_wheel.load" = load i64, ptr @mouse_has_wheel, align 8
  %cmpeqtmp27 = icmp eq i64 %"%mouse_has_wheel.load", 1
  br i1 %cmpeqtmp27, label %then_764, label %merge_765

merge_765:                                        ; preds = %merge_767, %merge_763
  ret i64 0

merge_767:                                        ; preds = %then_766, %then_764
  %"%1961.load" = load i64, ptr %"%w", align 8
  store i64 %"%1961.load", ptr %"%mouse_wheel", align 8
  br label %merge_765

then_748:                                         ; preds = %entry
  ret i64 0

then_750:                                         ; preds = %merge_749
  ret i64 0

then_752:                                         ; preds = %merge_751
  %"%dx.load" = load i64, ptr %"%dx", align 8
  %subtmp = sub i64 %"%dx.load", 256
  store i64 %subtmp, ptr %"%dx", align 8
  br label %merge_753

then_754:                                         ; preds = %merge_753
  %"%dy.load" = load i64, ptr %"%dy", align 8
  %subtmp15 = sub i64 %"%dy.load", 256
  store i64 %subtmp15, ptr %"%dy", align 8
  br label %merge_755

then_756:                                         ; preds = %merge_755
  store i64 0, ptr %"%mouse_x", align 8
  br label %merge_757

then_758:                                         ; preds = %merge_757
  store i64 0, ptr %"%mouse_y", align 8
  br label %merge_759

then_760:                                         ; preds = %merge_759
  %"%1948.load" = load i64, ptr @mouse_max_x, align 8
  store i64 %"%1948.load", ptr %"%mouse_x", align 8
  br label %merge_761

then_762:                                         ; preds = %merge_761
  %"%1951.load" = load i64, ptr @mouse_max_y, align 8
  store i64 %"%1951.load", ptr %"%mouse_y", align 8
  br label %merge_763

then_764:                                         ; preds = %merge_763
  %"%1955.load" = load i64, ptr @mouse_byte3, align 8
  store i64 %"%1955.load", ptr %"%w", align 8
  %"%w.load" = load i64, ptr %"%w", align 8
  %cmpgttmp28 = icmp sgt i64 %"%w.load", 127
  br i1 %cmpgttmp28, label %then_766, label %merge_767

then_766:                                         ; preds = %then_764
  %"%w.load29" = load i64, ptr %"%w", align 8
  %subtmp30 = sub i64 %"%w.load29", 256
  store i64 %subtmp30, ptr %"%w", align 8
  br label %merge_767
}

define i64 @ps2_mouse_get_x() {
entry:
  %loadtmp = load i64, ptr @mouse_x, align 8
  ret i64 %loadtmp
}

define i64 @ps2_mouse_get_y() {
entry:
  %loadtmp = load i64, ptr @mouse_y, align 8
  ret i64 %loadtmp
}

define i64 @ps2_mouse_get_buttons() {
entry:
  %loadtmp = load i64, ptr @mouse_buttons, align 8
  ret i64 %loadtmp
}

define i64 @ps2_mouse_left() {
entry:
  %"%mouse_buttons.load" = load i64, ptr @mouse_buttons, align 8
  %andtmp = and i64 %"%mouse_buttons.load", 1
  ret i64 %andtmp
}

define i64 @ps2_mouse_right() {
entry:
  %"%mouse_buttons.load" = load i64, ptr @mouse_buttons, align 8
  %shrtmp = lshr i64 %"%mouse_buttons.load", 1
  %andtmp = and i64 %shrtmp, 1
  ret i64 %andtmp
}

define i64 @ps2_mouse_middle() {
entry:
  %"%mouse_buttons.load" = load i64, ptr @mouse_buttons, align 8
  %shrtmp = lshr i64 %"%mouse_buttons.load", 2
  %andtmp = and i64 %shrtmp, 1
  ret i64 %andtmp
}

define i64 @ps2_mouse_get_wheel() {
entry:
  %loadtmp = load i64, ptr @mouse_wheel, align 8
  ret i64 %loadtmp
}

define i64 @ps2_poll() {
entry:
  %"%data" = alloca i64, align 8
  %"%sts" = alloca i64, align 8
  %"%PS2_STATUS.load" = load i64, ptr @PS2_STATUS, align 8
  call void @sad_ll_port_inb(i64 %"%PS2_STATUS.load")
  store i64 0, ptr %"%sts", align 8
  %"%sts.load" = load i64, ptr %"%sts", align 8
  %andtmp = and i64 %"%sts.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_768, label %merge_769

else_772:                                         ; preds = %merge_769
  %"%data.load4" = load i64, ptr %"%data", align 8
  %calltmp5 = call i64 @ps2_kbd_handle_byte(i64 %"%data.load4")
  br label %merge_771

merge_769:                                        ; preds = %entry
  %"%PS2_DATA.load" = load i64, ptr @PS2_DATA, align 8
  call void @sad_ll_port_inb(i64 %"%PS2_DATA.load")
  store i64 0, ptr %"%data", align 8
  %"%sts.load1" = load i64, ptr %"%sts", align 8
  %shrtmp = lshr i64 %"%sts.load1", 5
  %andtmp2 = and i64 %shrtmp, 1
  %cmpeqtmp3 = icmp eq i64 %andtmp2, 1
  br i1 %cmpeqtmp3, label %then_770, label %else_772

merge_771:                                        ; preds = %else_772, %then_770
  ret i64 1

then_768:                                         ; preds = %entry
  ret i64 0

then_770:                                         ; preds = %merge_769
  %"%data.load" = load i64, ptr %"%data", align 8
  %calltmp = call i64 @ps2_mouse_handle_byte(i64 %"%data.load")
  br label %merge_771
}

define i64 @ps2_mouse_enable_wheel() {
entry:
  %"%mouse_has_wheel" = alloca i64, align 8
  %"%id" = alloca i64, align 8
  %"%PS2_DEV_SET_RATE.load" = load i64, ptr @PS2_DEV_SET_RATE, align 8
  %calltmp = call i64 @ps2_send_device2_cmd(i64 %"%PS2_DEV_SET_RATE.load")
  %calltmp1 = call i64 @ps2_send_device2_cmd(i64 200)
  %"%PS2_DEV_SET_RATE.load2" = load i64, ptr @PS2_DEV_SET_RATE, align 8
  %calltmp3 = call i64 @ps2_send_device2_cmd(i64 %"%PS2_DEV_SET_RATE.load2")
  %calltmp4 = call i64 @ps2_send_device2_cmd(i64 100)
  %"%PS2_DEV_SET_RATE.load5" = load i64, ptr @PS2_DEV_SET_RATE, align 8
  %calltmp6 = call i64 @ps2_send_device2_cmd(i64 %"%PS2_DEV_SET_RATE.load5")
  %calltmp7 = call i64 @ps2_send_device2_cmd(i64 80)
  %"%PS2_DEV_IDENTIFY.load" = load i64, ptr @PS2_DEV_IDENTIFY, align 8
  %calltmp8 = call i64 @ps2_send_device2_cmd(i64 %"%PS2_DEV_IDENTIFY.load")
  %calltmp9 = call i64 @ps2_read_data()
  store i64 %calltmp9, ptr %"%id", align 8
  %"%id.load" = load i64, ptr %"%id", align 8
  %cmpeqtmp = icmp eq i64 %"%id.load", 3
  br i1 %cmpeqtmp, label %then_773, label %else_775

else_775:                                         ; preds = %entry
  %calltmp12 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %"%2030.load" = load i64, ptr %"%id", align 8
  %calltmp13 = call i64 @"ps2_serial_\D8\B1\D9\82\D9\85"(i64 %"%2030.load")
  %calltmp14 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %calltmp15 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_774

merge_774:                                        ; preds = %else_775, %then_773
  %loadtmp = load i64, ptr %"%mouse_has_wheel", align 8
  ret i64 %loadtmp

then_773:                                         ; preds = %entry
  store i64 1, ptr %"%mouse_has_wheel", align 8
  %calltmp10 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %calltmp11 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_774
}

define i64 @"ps2_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %calltmp = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %calltmp1 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%ps2_port1_exists.load" = load i64, ptr @ps2_port1_exists, align 8
  %cmpeqtmp = icmp eq i64 %"%ps2_port1_exists.load", 1
  br i1 %cmpeqtmp, label %then_776, label %else_778

else_778:                                         ; preds = %entry
  %calltmp4 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  br label %merge_777

else_781:                                         ; preds = %merge_777
  %calltmp8 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  br label %merge_780

merge_777:                                        ; preds = %else_778, %then_776
  %calltmp5 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %"%ps2_port2_exists.load" = load i64, ptr @ps2_port2_exists, align 8
  %cmpeqtmp6 = icmp eq i64 %"%ps2_port2_exists.load", 1
  br i1 %cmpeqtmp6, label %then_779, label %else_781

merge_780:                                        ; preds = %else_781, %then_779
  %calltmp9 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp10 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %"%2061.load" = load i64, ptr @ps2_kbd_irq_count, align 8
  %calltmp11 = call i64 @"ps2_serial_\D8\B1\D9\82\D9\85"(i64 %"%2061.load")
  %calltmp12 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %"%2066.load" = load i64, ptr @ps2_mouse_irq_count, align 8
  %calltmp13 = call i64 @"ps2_serial_\D8\B1\D9\82\D9\85"(i64 %"%2066.load")
  %calltmp14 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp15 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %"%2072.load" = load i64, ptr @kbd_buf_count, align 8
  %calltmp16 = call i64 @"ps2_serial_\D8\B1\D9\82\D9\85"(i64 %"%2072.load")
  %calltmp17 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %"%kbd_shift_left.load" = load i64, ptr @kbd_shift_left, align 8
  %"%kbd_shift_right.load" = load i64, ptr @kbd_shift_right, align 8
  %addtmp = add i64 %"%kbd_shift_left.load", %"%kbd_shift_right.load"
  %calltmp18 = call i64 @"ps2_serial_\D8\B1\D9\82\D9\85"(i64 %addtmp)
  %calltmp19 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  %"%kbd_ctrl_left.load" = load i64, ptr @kbd_ctrl_left, align 8
  %"%kbd_ctrl_right.load" = load i64, ptr @kbd_ctrl_right, align 8
  %addtmp20 = add i64 %"%kbd_ctrl_left.load", %"%kbd_ctrl_right.load"
  %calltmp21 = call i64 @"ps2_serial_\D8\B1\D9\82\D9\85"(i64 %addtmp20)
  %calltmp22 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%kbd_alt_left.load" = load i64, ptr @kbd_alt_left, align 8
  %"%kbd_alt_right.load" = load i64, ptr @kbd_alt_right, align 8
  %addtmp23 = add i64 %"%kbd_alt_left.load", %"%kbd_alt_right.load"
  %calltmp24 = call i64 @"ps2_serial_\D8\B1\D9\82\D9\85"(i64 %addtmp23)
  %calltmp25 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp26 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  %"%2090.load" = load i64, ptr @kbd_capslock, align 8
  %calltmp27 = call i64 @"ps2_serial_\D8\B1\D9\82\D9\85"(i64 %"%2090.load")
  %calltmp28 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %"%2095.load" = load i64, ptr @kbd_numlock, align 8
  %calltmp29 = call i64 @"ps2_serial_\D8\B1\D9\82\D9\85"(i64 %"%2095.load")
  %calltmp30 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %"%2100.load" = load i64, ptr @kbd_scrolllock, align 8
  %calltmp31 = call i64 @"ps2_serial_\D8\B1\D9\82\D9\85"(i64 %"%2100.load")
  %calltmp32 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp33 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  %"%2106.load" = load i64, ptr @mouse_x, align 8
  %calltmp34 = call i64 @"ps2_serial_\D8\B1\D9\82\D9\85"(i64 %"%2106.load")
  %calltmp35 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  %"%2111.load" = load i64, ptr @mouse_y, align 8
  %calltmp36 = call i64 @"ps2_serial_\D8\B1\D9\82\D9\85"(i64 %"%2111.load")
  %calltmp37 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  %"%2116.load" = load i64, ptr @mouse_buttons, align 8
  %calltmp38 = call i64 @"ps2_serial_\D8\B1\D9\82\D9\85"(i64 %"%2116.load")
  %"%mouse_has_wheel.load" = load i64, ptr @mouse_has_wheel, align 8
  %cmpeqtmp39 = icmp eq i64 %"%mouse_has_wheel.load", 1
  br i1 %cmpeqtmp39, label %then_782, label %merge_783

merge_783:                                        ; preds = %then_782, %merge_780
  %calltmp42 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp43 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  %calltmp44 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_776:                                         ; preds = %entry
  %calltmp3 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  br label %merge_777

then_779:                                         ; preds = %merge_777
  %calltmp7 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  br label %merge_780

then_782:                                         ; preds = %merge_780
  %calltmp40 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  %"%2123.load" = load i64, ptr @mouse_wheel, align 8
  %calltmp41 = call i64 @"ps2_serial_\D8\B1\D9\82\D9\85"(i64 %"%2123.load")
  br label %merge_783
}

define i64 @"ps2_\D9\87\D9\8A\D8\A6"() {
entry:
  %"%ps2_\D9\87\D9\8A\D8\A6_\D8\AA\D9\85" = alloca i64, align 8
  %"%resp2" = alloca i64, align 8
  %"%resp1" = alloca i64, align 8
  %"%ps2_port2_exists" = alloca i64, align 8
  %"%port2_result" = alloca i64, align 8
  %"%ps2_port1_exists" = alloca i64, align 8
  %"%port1_result" = alloca i64, align 8
  %"%ps2_dual_channel" = alloca i64, align 8
  %"%config2" = alloca i64, align 8
  %"%ps2_errors" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%config" = alloca i64, align 8
  %calltmp = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %calltmp1 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @ps2_flush()
  %"%PS2_CMD_DISABLE_PORT1.load" = load i64, ptr @PS2_CMD_DISABLE_PORT1, align 8
  %calltmp3 = call i64 @ps2_send_controller_cmd(i64 %"%PS2_CMD_DISABLE_PORT1.load")
  %"%PS2_CMD_DISABLE_PORT2.load" = load i64, ptr @PS2_CMD_DISABLE_PORT2, align 8
  %calltmp4 = call i64 @ps2_send_controller_cmd(i64 %"%PS2_CMD_DISABLE_PORT2.load")
  %calltmp5 = call i64 @ps2_flush()
  %"%PS2_CMD_READ_CONFIG.load" = load i64, ptr @PS2_CMD_READ_CONFIG, align 8
  %calltmp6 = call i64 @ps2_send_controller_cmd(i64 %"%PS2_CMD_READ_CONFIG.load")
  %calltmp7 = call i64 @ps2_read_data()
  store i64 %calltmp7, ptr %"%config", align 8
  %calltmp8 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  %"%2144.load" = load i64, ptr %"%config", align 8
  %calltmp9 = call i64 @ps2_serial_hex8(i64 %"%2144.load")
  %calltmp10 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  %"%config.load" = load i64, ptr %"%config", align 8
  %andtmp = and i64 %"%config.load", 1
  %"%config.load11" = load i64, ptr %"%config", align 8
  %subtmp = sub i64 %"%config.load11", %andtmp
  store i64 %subtmp, ptr %"%config", align 8
  %"%config.load12" = load i64, ptr %"%config", align 8
  %shrtmp = lshr i64 %"%config.load12", 1
  %andtmp13 = and i64 %shrtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp13, 1
  br i1 %cmpeqtmp, label %then_784, label %merge_785

else_790:                                         ; preds = %merge_787
  %calltmp30 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  %"%2180.load" = load i64, ptr %"%result", align 8
  %calltmp31 = call i64 @ps2_serial_hex8(i64 %"%2180.load")
  %calltmp32 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  %"%ps2_errors.load" = load i64, ptr @ps2_errors, align 8
  %addtmp = add i64 %"%ps2_errors.load", 1
  store i64 %addtmp, ptr %"%ps2_errors", align 8
  br label %merge_789

else_795:                                         ; preds = %merge_792
  %calltmp51 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  %"%2213.load" = load i64, ptr %"%port1_result", align 8
  %calltmp52 = call i64 @"ps2_serial_\D8\B1\D9\82\D9\85"(i64 %"%2213.load")
  %calltmp53 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_794

else_800:                                         ; preds = %then_796
  %calltmp61 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  %"%2231.load" = load i64, ptr %"%port2_result", align 8
  %calltmp62 = call i64 @"ps2_serial_\D8\B1\D9\82\D9\85"(i64 %"%2231.load")
  %calltmp63 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_799

merge_785:                                        ; preds = %then_784, %entry
  %"%config.load16" = load i64, ptr %"%config", align 8
  %shrtmp17 = lshr i64 %"%config.load16", 6
  %andtmp18 = and i64 %shrtmp17, 1
  %cmpeqtmp19 = icmp eq i64 %andtmp18, 1
  br i1 %cmpeqtmp19, label %then_786, label %merge_787

merge_787:                                        ; preds = %then_786, %merge_785
  %"%PS2_CMD_WRITE_CONFIG.load" = load i64, ptr @PS2_CMD_WRITE_CONFIG, align 8
  %calltmp22 = call i64 @ps2_send_controller_cmd(i64 %"%PS2_CMD_WRITE_CONFIG.load")
  %"%config.load23" = load i64, ptr %"%config", align 8
  %calltmp24 = call i64 @ps2_send_controller_data(i64 %"%config.load23")
  %"%PS2_CMD_SELF_TEST.load" = load i64, ptr @PS2_CMD_SELF_TEST, align 8
  %calltmp25 = call i64 @ps2_send_controller_cmd(i64 %"%PS2_CMD_SELF_TEST.load")
  %calltmp26 = call i64 @ps2_read_data()
  store i64 %calltmp26, ptr %"%result", align 8
  %"%result.load" = load i64, ptr %"%result", align 8
  %cmpeqtmp27 = icmp eq i64 %"%result.load", 85
  br i1 %cmpeqtmp27, label %then_788, label %else_790

merge_789:                                        ; preds = %else_790, %then_788
  %"%PS2_CMD_WRITE_CONFIG.load33" = load i64, ptr @PS2_CMD_WRITE_CONFIG, align 8
  %calltmp34 = call i64 @ps2_send_controller_cmd(i64 %"%PS2_CMD_WRITE_CONFIG.load33")
  %"%config.load35" = load i64, ptr %"%config", align 8
  %calltmp36 = call i64 @ps2_send_controller_data(i64 %"%config.load35")
  %"%PS2_CMD_ENABLE_PORT2.load" = load i64, ptr @PS2_CMD_ENABLE_PORT2, align 8
  %calltmp37 = call i64 @ps2_send_controller_cmd(i64 %"%PS2_CMD_ENABLE_PORT2.load")
  %"%PS2_CMD_READ_CONFIG.load38" = load i64, ptr @PS2_CMD_READ_CONFIG, align 8
  %calltmp39 = call i64 @ps2_send_controller_cmd(i64 %"%PS2_CMD_READ_CONFIG.load38")
  %calltmp40 = call i64 @ps2_read_data()
  store i64 %calltmp40, ptr %"%config2", align 8
  %"%config2.load" = load i64, ptr %"%config2", align 8
  %shrtmp41 = lshr i64 %"%config2.load", 5
  %andtmp42 = and i64 %shrtmp41, 1
  %cmpeqtmp43 = icmp eq i64 %andtmp42, 0
  br i1 %cmpeqtmp43, label %then_791, label %merge_792

merge_792:                                        ; preds = %then_791, %merge_789
  %"%PS2_CMD_TEST_PORT1.load" = load i64, ptr @PS2_CMD_TEST_PORT1, align 8
  %calltmp46 = call i64 @ps2_send_controller_cmd(i64 %"%PS2_CMD_TEST_PORT1.load")
  %calltmp47 = call i64 @ps2_read_data()
  store i64 %calltmp47, ptr %"%port1_result", align 8
  %"%port1_result.load" = load i64, ptr %"%port1_result", align 8
  %cmpeqtmp48 = icmp eq i64 %"%port1_result.load", 0
  br i1 %cmpeqtmp48, label %then_793, label %else_795

merge_794:                                        ; preds = %else_795, %then_793
  %"%ps2_dual_channel.load" = load i64, ptr %"%ps2_dual_channel", align 8
  %cmpeqtmp54 = icmp eq i64 %"%ps2_dual_channel.load", 1
  br i1 %cmpeqtmp54, label %then_796, label %merge_797

merge_797:                                        ; preds = %merge_799, %merge_794
  %"%ps2_port1_exists.load" = load i64, ptr %"%ps2_port1_exists", align 8
  %cmpeqtmp58 = icmp eq i64 %"%ps2_port1_exists.load", 1
  br i1 %cmpeqtmp58, label %then_801, label %merge_802

merge_799:                                        ; preds = %else_800, %then_798
  br label %merge_797

merge_802:                                        ; preds = %then_801, %merge_797
  %"%ps2_port2_exists.load" = load i64, ptr %"%ps2_port2_exists", align 8
  %cmpeqtmp67 = icmp eq i64 %"%ps2_port2_exists.load", 1
  br i1 %cmpeqtmp67, label %then_803, label %merge_804

merge_804:                                        ; preds = %then_803, %merge_802
  %"%PS2_CMD_WRITE_CONFIG.load72" = load i64, ptr @PS2_CMD_WRITE_CONFIG, align 8
  %calltmp73 = call i64 @ps2_send_controller_cmd(i64 %"%PS2_CMD_WRITE_CONFIG.load72")
  %"%config.load74" = load i64, ptr %"%config", align 8
  %calltmp75 = call i64 @ps2_send_controller_data(i64 %"%config.load74")
  %"%ps2_port1_exists.load76" = load i64, ptr %"%ps2_port1_exists", align 8
  %cmpeqtmp77 = icmp eq i64 %"%ps2_port1_exists.load76", 1
  br i1 %cmpeqtmp77, label %then_805, label %merge_806

merge_806:                                        ; preds = %merge_808, %merge_804
  %"%ps2_port2_exists.load81" = load i64, ptr %"%ps2_port2_exists", align 8
  %cmpeqtmp82 = icmp eq i64 %"%ps2_port2_exists.load81", 1
  br i1 %cmpeqtmp82, label %then_809, label %merge_810

merge_808:                                        ; preds = %then_807, %then_805
  %calltmp85 = call i64 @ps2_flush()
  %"%PS2_DEV_ENABLE_SCAN.load" = load i64, ptr @PS2_DEV_ENABLE_SCAN, align 8
  %calltmp86 = call i64 @ps2_send_device1_cmd(i64 %"%PS2_DEV_ENABLE_SCAN.load")
  br label %merge_806

merge_810:                                        ; preds = %then_809, %merge_806
  store i64 1, ptr %"%ps2_\D9\87\D9\8A\D8\A6_\D8\AA\D9\85", align 8
  %calltmp96 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  %calltmp97 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_784:                                         ; preds = %entry
  %"%config.load14" = load i64, ptr %"%config", align 8
  %subtmp15 = sub i64 %"%config.load14", 2
  store i64 %subtmp15, ptr %"%config", align 8
  br label %merge_785

then_786:                                         ; preds = %merge_785
  %"%config.load20" = load i64, ptr %"%config", align 8
  %subtmp21 = sub i64 %"%config.load20", 64
  store i64 %subtmp21, ptr %"%config", align 8
  br label %merge_787

then_788:                                         ; preds = %merge_787
  %calltmp28 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  %calltmp29 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_789

then_791:                                         ; preds = %merge_789
  store i64 1, ptr %"%ps2_dual_channel", align 8
  %"%PS2_CMD_DISABLE_PORT2.load44" = load i64, ptr @PS2_CMD_DISABLE_PORT2, align 8
  %calltmp45 = call i64 @ps2_send_controller_cmd(i64 %"%PS2_CMD_DISABLE_PORT2.load44")
  br label %merge_792

then_793:                                         ; preds = %merge_792
  store i64 1, ptr %"%ps2_port1_exists", align 8
  %calltmp49 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  %calltmp50 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_794

then_796:                                         ; preds = %merge_794
  %"%PS2_CMD_TEST_PORT2.load" = load i64, ptr @PS2_CMD_TEST_PORT2, align 8
  %calltmp55 = call i64 @ps2_send_controller_cmd(i64 %"%PS2_CMD_TEST_PORT2.load")
  %calltmp56 = call i64 @ps2_read_data()
  store i64 %calltmp56, ptr %"%port2_result", align 8
  %"%port2_result.load" = load i64, ptr %"%port2_result", align 8
  %cmpeqtmp57 = icmp eq i64 %"%port2_result.load", 0
  br i1 %cmpeqtmp57, label %then_798, label %else_800

then_798:                                         ; preds = %then_796
  store i64 1, ptr %"%ps2_port2_exists", align 8
  %calltmp59 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.30 to i64))
  %calltmp60 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_799

then_801:                                         ; preds = %merge_797
  %"%PS2_CMD_ENABLE_PORT1.load" = load i64, ptr @PS2_CMD_ENABLE_PORT1, align 8
  %calltmp64 = call i64 @ps2_send_controller_cmd(i64 %"%PS2_CMD_ENABLE_PORT1.load")
  %"%config.load65" = load i64, ptr %"%config", align 8
  %"%PS2_CFG_INT1.load" = load i64, ptr @PS2_CFG_INT1, align 8
  %addtmp66 = add i64 %"%config.load65", %"%PS2_CFG_INT1.load"
  store i64 %addtmp66, ptr %"%config", align 8
  br label %merge_802

then_803:                                         ; preds = %merge_802
  %"%PS2_CMD_ENABLE_PORT2.load68" = load i64, ptr @PS2_CMD_ENABLE_PORT2, align 8
  %calltmp69 = call i64 @ps2_send_controller_cmd(i64 %"%PS2_CMD_ENABLE_PORT2.load68")
  %"%config.load70" = load i64, ptr %"%config", align 8
  %"%PS2_CFG_INT2.load" = load i64, ptr @PS2_CFG_INT2, align 8
  %addtmp71 = add i64 %"%config.load70", %"%PS2_CFG_INT2.load"
  store i64 %addtmp71, ptr %"%config", align 8
  br label %merge_804

then_805:                                         ; preds = %merge_804
  %"%PS2_DEV_RESET.load" = load i64, ptr @PS2_DEV_RESET, align 8
  %calltmp78 = call i64 @ps2_send_device1_cmd(i64 %"%PS2_DEV_RESET.load")
  %calltmp79 = call i64 @ps2_read_data()
  store i64 %calltmp79, ptr %"%resp1", align 8
  %"%resp1.load" = load i64, ptr %"%resp1", align 8
  %"%PS2_RESP_SELF_OK.load" = load i64, ptr @PS2_RESP_SELF_OK, align 8
  %cmpeqtmp80 = icmp eq i64 %"%resp1.load", %"%PS2_RESP_SELF_OK.load"
  br i1 %cmpeqtmp80, label %then_807, label %merge_808

then_807:                                         ; preds = %then_805
  %calltmp83 = call i64 @"ps2_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.32 to i64))
  %calltmp84 = call i64 @"ps2_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_808

then_809:                                         ; preds = %merge_806
  %"%PS2_DEV_RESET.load87" = load i64, ptr @PS2_DEV_RESET, align 8
  %calltmp88 = call i64 @ps2_send_device2_cmd(i64 %"%PS2_DEV_RESET.load87")
  %calltmp89 = call i64 @ps2_read_data()
  store i64 %calltmp89, ptr %"%resp2", align 8
  %calltmp90 = call i64 @ps2_flush()
  %calltmp91 = call i64 @ps2_mouse_enable_wheel()
  %"%PS2_DEV_ENABLE_SCAN.load92" = load i64, ptr @PS2_DEV_ENABLE_SCAN, align 8
  %calltmp93 = call i64 @ps2_send_device2_cmd(i64 %"%PS2_DEV_ENABLE_SCAN.load92")
  %"%PS2_DEV_SET_RATE.load" = load i64, ptr @PS2_DEV_SET_RATE, align 8
  %calltmp94 = call i64 @ps2_send_device2_cmd(i64 %"%PS2_DEV_SET_RATE.load")
  %calltmp95 = call i64 @ps2_send_device2_cmd(i64 100)
  br label %merge_810
}

define i64 @"ps2_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  %"%rc" = alloca i64, align 8
  %"%max_iter" = alloca i64, align 8
  %"%processed" = alloca i64, align 8
  store i64 0, ptr %"%processed", align 8
  store i64 32, ptr %"%max_iter", align 8
  br label %while_cond_811

merge_815:                                        ; preds = %then_814, %while_body_812
  %"%processed.load" = load i64, ptr %"%processed", align 8
  %addtmp = add i64 %"%processed.load", 1
  store i64 %addtmp, ptr %"%processed", align 8
  %"%max_iter.load1" = load i64, ptr %"%max_iter", align 8
  %subtmp = sub i64 %"%max_iter.load1", 1
  store i64 %subtmp, ptr %"%max_iter", align 8
  br label %while_cond_811

then_814:                                         ; preds = %while_body_812
  br label %merge_815

while_body_812:                                   ; preds = %while_cond_811
  %calltmp = call i64 @ps2_poll()
  store i64 %calltmp, ptr %"%rc", align 8
  %"%rc.load" = load i64, ptr %"%rc", align 8
  %cmpeqtmp = icmp eq i64 %"%rc.load", 0
  br i1 %cmpeqtmp, label %then_814, label %merge_815

while_cond_811:                                   ; preds = %merge_815, %entry
  %"%max_iter.load" = load i64, ptr %"%max_iter", align 8
  %cmpgttmp = icmp sgt i64 %"%max_iter.load", 0
  br i1 %cmpgttmp, label %while_body_812, label %while_exit_813

while_exit_813:                                   ; preds = %while_cond_811
  %loadtmp = load i64, ptr %"%processed", align 8
  ret i64 %loadtmp
}

define void @__sad_main() {
entry:
  %"%sc2_shift_93" = alloca i64, align 8
  %"%sc2_shift_91" = alloca i64, align 8
  %"%sc2_shift_85" = alloca i64, align 8
  %"%sc2_shift_84" = alloca i64, align 8
  %"%sc2_shift_82" = alloca i64, align 8
  %"%sc2_shift_78" = alloca i64, align 8
  %"%sc2_shift_77" = alloca i64, align 8
  %"%sc2_shift_76" = alloca i64, align 8
  %"%sc2_shift_75" = alloca i64, align 8
  %"%sc2_shift_74" = alloca i64, align 8
  %"%sc2_shift_73" = alloca i64, align 8
  %"%sc2_shift_70" = alloca i64, align 8
  %"%sc2_shift_69" = alloca i64, align 8
  %"%sc2_shift_68" = alloca i64, align 8
  %"%sc2_shift_67" = alloca i64, align 8
  %"%sc2_shift_66" = alloca i64, align 8
  %"%sc2_shift_65" = alloca i64, align 8
  %"%sc2_shift_62" = alloca i64, align 8
  %"%sc2_shift_61" = alloca i64, align 8
  %"%sc2_shift_60" = alloca i64, align 8
  %"%sc2_shift_59" = alloca i64, align 8
  %"%sc2_shift_58" = alloca i64, align 8
  %"%sc2_shift_54" = alloca i64, align 8
  %"%sc2_shift_53" = alloca i64, align 8
  %"%sc2_shift_52" = alloca i64, align 8
  %"%sc2_shift_51" = alloca i64, align 8
  %"%sc2_shift_50" = alloca i64, align 8
  %"%sc2_shift_49" = alloca i64, align 8
  %"%sc2_shift_46" = alloca i64, align 8
  %"%sc2_shift_45" = alloca i64, align 8
  %"%sc2_shift_44" = alloca i64, align 8
  %"%sc2_shift_43" = alloca i64, align 8
  %"%sc2_shift_42" = alloca i64, align 8
  %"%sc2_shift_38" = alloca i64, align 8
  %"%sc2_shift_37" = alloca i64, align 8
  %"%sc2_shift_36" = alloca i64, align 8
  %"%sc2_shift_35" = alloca i64, align 8
  %"%sc2_shift_34" = alloca i64, align 8
  %"%sc2_shift_33" = alloca i64, align 8
  %"%sc2_shift_30" = alloca i64, align 8
  %"%sc2_shift_29" = alloca i64, align 8
  %"%sc2_shift_28" = alloca i64, align 8
  %"%sc2_shift_27" = alloca i64, align 8
  %"%sc2_shift_26" = alloca i64, align 8
  %"%sc2_shift_22" = alloca i64, align 8
  %"%sc2_shift_21" = alloca i64, align 8
  %"%sc2_shift_14" = alloca i64, align 8
  %"%sc2_ascii_127" = alloca i64, align 8
  %"%sc2_ascii_126" = alloca i64, align 8
  %"%sc2_ascii_125" = alloca i64, align 8
  %"%sc2_ascii_124" = alloca i64, align 8
  %"%sc2_ascii_123" = alloca i64, align 8
  %"%sc2_ascii_122" = alloca i64, align 8
  %"%sc2_ascii_121" = alloca i64, align 8
  %"%sc2_ascii_120" = alloca i64, align 8
  %"%sc2_ascii_119" = alloca i64, align 8
  %"%sc2_ascii_118" = alloca i64, align 8
  %"%sc2_ascii_117" = alloca i64, align 8
  %"%sc2_ascii_116" = alloca i64, align 8
  %"%sc2_ascii_115" = alloca i64, align 8
  %"%sc2_ascii_114" = alloca i64, align 8
  %"%sc2_ascii_113" = alloca i64, align 8
  %"%sc2_ascii_112" = alloca i64, align 8
  %"%sc2_ascii_111" = alloca i64, align 8
  %"%sc2_ascii_110" = alloca i64, align 8
  %"%sc2_ascii_109" = alloca i64, align 8
  %"%sc2_ascii_108" = alloca i64, align 8
  %"%sc2_ascii_107" = alloca i64, align 8
  %"%sc2_ascii_106" = alloca i64, align 8
  %"%sc2_ascii_105" = alloca i64, align 8
  %"%sc2_ascii_104" = alloca i64, align 8
  %"%sc2_ascii_103" = alloca i64, align 8
  %"%sc2_ascii_102" = alloca i64, align 8
  %"%sc2_ascii_101" = alloca i64, align 8
  %"%sc2_ascii_100" = alloca i64, align 8
  %"%sc2_ascii_99" = alloca i64, align 8
  %"%sc2_ascii_98" = alloca i64, align 8
  %"%sc2_ascii_97" = alloca i64, align 8
  %"%sc2_ascii_96" = alloca i64, align 8
  %"%sc2_ascii_95" = alloca i64, align 8
  %"%sc2_ascii_94" = alloca i64, align 8
  %"%sc2_ascii_93" = alloca i64, align 8
  %"%sc2_ascii_92" = alloca i64, align 8
  %"%sc2_ascii_91" = alloca i64, align 8
  %"%sc2_ascii_90" = alloca i64, align 8
  %"%sc2_ascii_89" = alloca i64, align 8
  %"%sc2_ascii_88" = alloca i64, align 8
  %"%sc2_ascii_87" = alloca i64, align 8
  %"%sc2_ascii_86" = alloca i64, align 8
  %"%sc2_ascii_85" = alloca i64, align 8
  %"%sc2_ascii_84" = alloca i64, align 8
  %"%sc2_ascii_83" = alloca i64, align 8
  %"%sc2_ascii_82" = alloca i64, align 8
  %"%sc2_ascii_81" = alloca i64, align 8
  %"%sc2_ascii_80" = alloca i64, align 8
  %"%sc2_ascii_79" = alloca i64, align 8
  %"%sc2_ascii_78" = alloca i64, align 8
  %"%sc2_ascii_77" = alloca i64, align 8
  %"%sc2_ascii_76" = alloca i64, align 8
  %"%sc2_ascii_75" = alloca i64, align 8
  %"%sc2_ascii_74" = alloca i64, align 8
  %"%sc2_ascii_73" = alloca i64, align 8
  %"%sc2_ascii_72" = alloca i64, align 8
  %"%sc2_ascii_71" = alloca i64, align 8
  %"%sc2_ascii_70" = alloca i64, align 8
  %"%sc2_ascii_69" = alloca i64, align 8
  %"%sc2_ascii_68" = alloca i64, align 8
  %"%sc2_ascii_67" = alloca i64, align 8
  %"%sc2_ascii_66" = alloca i64, align 8
  %"%sc2_ascii_65" = alloca i64, align 8
  %"%sc2_ascii_64" = alloca i64, align 8
  %"%sc2_ascii_63" = alloca i64, align 8
  %"%sc2_ascii_62" = alloca i64, align 8
  %"%sc2_ascii_61" = alloca i64, align 8
  %"%sc2_ascii_60" = alloca i64, align 8
  %"%sc2_ascii_59" = alloca i64, align 8
  %"%sc2_ascii_58" = alloca i64, align 8
  %"%sc2_ascii_57" = alloca i64, align 8
  %"%sc2_ascii_56" = alloca i64, align 8
  %"%sc2_ascii_55" = alloca i64, align 8
  %"%sc2_ascii_54" = alloca i64, align 8
  %"%sc2_ascii_53" = alloca i64, align 8
  %"%sc2_ascii_52" = alloca i64, align 8
  %"%sc2_ascii_51" = alloca i64, align 8
  %"%sc2_ascii_50" = alloca i64, align 8
  %"%sc2_ascii_49" = alloca i64, align 8
  %"%sc2_ascii_48" = alloca i64, align 8
  %"%sc2_ascii_47" = alloca i64, align 8
  %"%sc2_ascii_46" = alloca i64, align 8
  %"%sc2_ascii_45" = alloca i64, align 8
  %"%sc2_ascii_44" = alloca i64, align 8
  %"%sc2_ascii_43" = alloca i64, align 8
  %"%sc2_ascii_42" = alloca i64, align 8
  %"%sc2_ascii_41" = alloca i64, align 8
  %"%sc2_ascii_40" = alloca i64, align 8
  %"%sc2_ascii_39" = alloca i64, align 8
  %"%sc2_ascii_38" = alloca i64, align 8
  %"%sc2_ascii_37" = alloca i64, align 8
  %"%sc2_ascii_36" = alloca i64, align 8
  %"%sc2_ascii_35" = alloca i64, align 8
  %"%sc2_ascii_34" = alloca i64, align 8
  %"%sc2_ascii_33" = alloca i64, align 8
  %"%sc2_ascii_32" = alloca i64, align 8
  %"%sc2_ascii_31" = alloca i64, align 8
  %"%sc2_ascii_30" = alloca i64, align 8
  %"%sc2_ascii_29" = alloca i64, align 8
  %"%sc2_ascii_28" = alloca i64, align 8
  %"%sc2_ascii_27" = alloca i64, align 8
  %"%sc2_ascii_26" = alloca i64, align 8
  %"%sc2_ascii_25" = alloca i64, align 8
  %"%sc2_ascii_24" = alloca i64, align 8
  %"%sc2_ascii_23" = alloca i64, align 8
  %"%sc2_ascii_22" = alloca i64, align 8
  %"%sc2_ascii_21" = alloca i64, align 8
  %"%sc2_ascii_20" = alloca i64, align 8
  %"%sc2_ascii_19" = alloca i64, align 8
  %"%sc2_ascii_18" = alloca i64, align 8
  %"%sc2_ascii_17" = alloca i64, align 8
  %"%sc2_ascii_16" = alloca i64, align 8
  %"%sc2_ascii_15" = alloca i64, align 8
  %"%sc2_ascii_14" = alloca i64, align 8
  %"%sc2_ascii_13" = alloca i64, align 8
  %"%sc2_ascii_12" = alloca i64, align 8
  %"%sc2_ascii_11" = alloca i64, align 8
  %"%sc2_ascii_10" = alloca i64, align 8
  %"%sc2_ascii_9" = alloca i64, align 8
  %"%sc2_ascii_8" = alloca i64, align 8
  %"%sc2_ascii_7" = alloca i64, align 8
  %"%sc2_ascii_6" = alloca i64, align 8
  %"%sc2_ascii_5" = alloca i64, align 8
  %"%sc2_ascii_4" = alloca i64, align 8
  %"%sc2_ascii_3" = alloca i64, align 8
  %"%sc2_ascii_2" = alloca i64, align 8
  %"%sc2_ascii_1" = alloca i64, align 8
  %"%sc2_ascii_0" = alloca i64, align 8
  %"%ps2_errors" = alloca i64, align 8
  %"%ps2_mouse_irq_count" = alloca i64, align 8
  %"%ps2_kbd_irq_count" = alloca i64, align 8
  %"%mouse_max_y" = alloca i64, align 8
  %"%mouse_max_x" = alloca i64, align 8
  %"%mouse_phase" = alloca i64, align 8
  %"%mouse_wheel" = alloca i64, align 8
  %"%mouse_byte3" = alloca i64, align 8
  %"%mouse_has_wheel" = alloca i64, align 8
  %"%mouse_byte2" = alloca i64, align 8
  %"%mouse_byte1" = alloca i64, align 8
  %"%mouse_byte0" = alloca i64, align 8
  %"%mouse_byte_idx" = alloca i64, align 8
  %"%mouse_buttons" = alloca i64, align 8
  %"%mouse_y" = alloca i64, align 8
  %"%mouse_x" = alloca i64, align 8
  %"%kbd_buf_count" = alloca i64, align 8
  %"%kbd_buf_tail" = alloca i64, align 8
  %"%kbd_buf_head" = alloca i64, align 8
  %"%kbd_buf_ascii_31" = alloca i64, align 8
  %"%kbd_buf_ascii_30" = alloca i64, align 8
  %"%kbd_buf_ascii_29" = alloca i64, align 8
  %"%kbd_buf_ascii_28" = alloca i64, align 8
  %"%kbd_buf_ascii_27" = alloca i64, align 8
  %"%kbd_buf_ascii_26" = alloca i64, align 8
  %"%kbd_buf_ascii_25" = alloca i64, align 8
  %"%kbd_buf_ascii_24" = alloca i64, align 8
  %"%kbd_buf_ascii_23" = alloca i64, align 8
  %"%kbd_buf_ascii_22" = alloca i64, align 8
  %"%kbd_buf_ascii_21" = alloca i64, align 8
  %"%kbd_buf_ascii_20" = alloca i64, align 8
  %"%kbd_buf_ascii_19" = alloca i64, align 8
  %"%kbd_buf_ascii_18" = alloca i64, align 8
  %"%kbd_buf_ascii_17" = alloca i64, align 8
  %"%kbd_buf_ascii_16" = alloca i64, align 8
  %"%kbd_buf_ascii_15" = alloca i64, align 8
  %"%kbd_buf_ascii_14" = alloca i64, align 8
  %"%kbd_buf_ascii_13" = alloca i64, align 8
  %"%kbd_buf_ascii_12" = alloca i64, align 8
  %"%kbd_buf_ascii_11" = alloca i64, align 8
  %"%kbd_buf_ascii_10" = alloca i64, align 8
  %"%kbd_buf_ascii_9" = alloca i64, align 8
  %"%kbd_buf_ascii_8" = alloca i64, align 8
  %"%kbd_buf_ascii_7" = alloca i64, align 8
  %"%kbd_buf_ascii_6" = alloca i64, align 8
  %"%kbd_buf_ascii_5" = alloca i64, align 8
  %"%kbd_buf_ascii_4" = alloca i64, align 8
  %"%kbd_buf_ascii_3" = alloca i64, align 8
  %"%kbd_buf_ascii_2" = alloca i64, align 8
  %"%kbd_buf_ascii_1" = alloca i64, align 8
  %"%kbd_buf_ascii_0" = alloca i64, align 8
  %"%kbd_buf_scancode_31" = alloca i64, align 8
  %"%kbd_buf_scancode_30" = alloca i64, align 8
  %"%kbd_buf_scancode_29" = alloca i64, align 8
  %"%kbd_buf_scancode_28" = alloca i64, align 8
  %"%kbd_buf_scancode_27" = alloca i64, align 8
  %"%kbd_buf_scancode_26" = alloca i64, align 8
  %"%kbd_buf_scancode_25" = alloca i64, align 8
  %"%kbd_buf_scancode_24" = alloca i64, align 8
  %"%kbd_buf_scancode_23" = alloca i64, align 8
  %"%kbd_buf_scancode_22" = alloca i64, align 8
  %"%kbd_buf_scancode_21" = alloca i64, align 8
  %"%kbd_buf_scancode_20" = alloca i64, align 8
  %"%kbd_buf_scancode_19" = alloca i64, align 8
  %"%kbd_buf_scancode_18" = alloca i64, align 8
  %"%kbd_buf_scancode_17" = alloca i64, align 8
  %"%kbd_buf_scancode_16" = alloca i64, align 8
  %"%kbd_buf_scancode_15" = alloca i64, align 8
  %"%kbd_buf_scancode_14" = alloca i64, align 8
  %"%kbd_buf_scancode_13" = alloca i64, align 8
  %"%kbd_buf_scancode_12" = alloca i64, align 8
  %"%kbd_buf_scancode_11" = alloca i64, align 8
  %"%kbd_buf_scancode_10" = alloca i64, align 8
  %"%kbd_buf_scancode_9" = alloca i64, align 8
  %"%kbd_buf_scancode_8" = alloca i64, align 8
  %"%kbd_buf_scancode_7" = alloca i64, align 8
  %"%kbd_buf_scancode_6" = alloca i64, align 8
  %"%kbd_buf_scancode_5" = alloca i64, align 8
  %"%kbd_buf_scancode_4" = alloca i64, align 8
  %"%kbd_buf_scancode_3" = alloca i64, align 8
  %"%kbd_buf_scancode_2" = alloca i64, align 8
  %"%kbd_buf_scancode_1" = alloca i64, align 8
  %"%kbd_buf_scancode_0" = alloca i64, align 8
  %"%kbd_leds" = alloca i64, align 8
  %"%kbd_scrolllock" = alloca i64, align 8
  %"%kbd_numlock" = alloca i64, align 8
  %"%kbd_capslock" = alloca i64, align 8
  %"%kbd_alt_right" = alloca i64, align 8
  %"%kbd_alt_left" = alloca i64, align 8
  %"%kbd_ctrl_right" = alloca i64, align 8
  %"%kbd_ctrl_left" = alloca i64, align 8
  %"%kbd_shift_right" = alloca i64, align 8
  %"%kbd_shift_left" = alloca i64, align 8
  %"%kbd_release" = alloca i64, align 8
  %"%kbd_extended" = alloca i64, align 8
  %"%ps2_dual_channel" = alloca i64, align 8
  %"%ps2_port2_exists" = alloca i64, align 8
  %"%ps2_port1_exists" = alloca i64, align 8
  %"%ps2_\D9\87\D9\8A\D8\A6_\D8\AA\D9\85" = alloca i64, align 8
  %"%MOUSE_BUF_SIZE" = alloca i64, align 8
  %"%KEY_BUF_SIZE" = alloca i64, align 8
  %"%SC2_EXTENDED2" = alloca i64, align 8
  %"%SC2_EXTENDED" = alloca i64, align 8
  %"%SC2_RELEASE" = alloca i64, align 8
  %"%PS2_RESP_ECHO" = alloca i64, align 8
  %"%PS2_RESP_SELF_OK" = alloca i64, align 8
  %"%PS2_RESP_ERROR" = alloca i64, align 8
  %"%PS2_RESP_RESEND" = alloca i64, align 8
  %"%PS2_RESP_ACK" = alloca i64, align 8
  %"%PS2_DEV_SET_LEDS" = alloca i64, align 8
  %"%PS2_DEV_SET_SCANSET" = alloca i64, align 8
  %"%PS2_DEV_SET_RATE" = alloca i64, align 8
  %"%PS2_DEV_IDENTIFY" = alloca i64, align 8
  %"%PS2_DEV_DISABLE_SCAN" = alloca i64, align 8
  %"%PS2_DEV_ENABLE_SCAN" = alloca i64, align 8
  %"%PS2_DEV_RESET" = alloca i64, align 8
  %"%PS2_CFG_TRANS" = alloca i64, align 8
  %"%PS2_CFG_CLOCK2" = alloca i64, align 8
  %"%PS2_CFG_CLOCK1" = alloca i64, align 8
  %"%PS2_CFG_SYSTEM" = alloca i64, align 8
  %"%PS2_CFG_INT2" = alloca i64, align 8
  %"%PS2_CFG_INT1" = alloca i64, align 8
  %"%PS2_CMD_WRITE_PORT2" = alloca i64, align 8
  %"%PS2_CMD_ENABLE_PORT1" = alloca i64, align 8
  %"%PS2_CMD_DISABLE_PORT1" = alloca i64, align 8
  %"%PS2_CMD_TEST_PORT1" = alloca i64, align 8
  %"%PS2_CMD_SELF_TEST" = alloca i64, align 8
  %"%PS2_CMD_TEST_PORT2" = alloca i64, align 8
  %"%PS2_CMD_ENABLE_PORT2" = alloca i64, align 8
  %"%PS2_CMD_DISABLE_PORT2" = alloca i64, align 8
  %"%PS2_CMD_WRITE_CONFIG" = alloca i64, align 8
  %"%PS2_CMD_READ_CONFIG" = alloca i64, align 8
  %"%PS2_STS_PARITY" = alloca i64, align 8
  %"%PS2_STS_TIMEOUT" = alloca i64, align 8
  %"%PS2_STS_CMD" = alloca i64, align 8
  %"%PS2_STS_SYSTEM" = alloca i64, align 8
  %"%PS2_STS_INPUT" = alloca i64, align 8
  %"%PS2_STS_OUTPUT" = alloca i64, align 8
  %"%PS2_COMMAND" = alloca i64, align 8
  %"%PS2_STATUS" = alloca i64, align 8
  %"%PS2_DATA" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 96, ptr %"%PS2_DATA", align 8
  store i64 100, ptr %"%PS2_STATUS", align 8
  store i64 100, ptr %"%PS2_COMMAND", align 8
  store i64 1, ptr %"%PS2_STS_OUTPUT", align 8
  store i64 2, ptr %"%PS2_STS_INPUT", align 8
  store i64 4, ptr %"%PS2_STS_SYSTEM", align 8
  store i64 8, ptr %"%PS2_STS_CMD", align 8
  store i64 64, ptr %"%PS2_STS_TIMEOUT", align 8
  store i64 128, ptr %"%PS2_STS_PARITY", align 8
  store i64 32, ptr %"%PS2_CMD_READ_CONFIG", align 8
  store i64 96, ptr %"%PS2_CMD_WRITE_CONFIG", align 8
  store i64 167, ptr %"%PS2_CMD_DISABLE_PORT2", align 8
  store i64 168, ptr %"%PS2_CMD_ENABLE_PORT2", align 8
  store i64 169, ptr %"%PS2_CMD_TEST_PORT2", align 8
  store i64 170, ptr %"%PS2_CMD_SELF_TEST", align 8
  store i64 171, ptr %"%PS2_CMD_TEST_PORT1", align 8
  store i64 173, ptr %"%PS2_CMD_DISABLE_PORT1", align 8
  store i64 174, ptr %"%PS2_CMD_ENABLE_PORT1", align 8
  store i64 212, ptr %"%PS2_CMD_WRITE_PORT2", align 8
  store i64 1, ptr %"%PS2_CFG_INT1", align 8
  store i64 2, ptr %"%PS2_CFG_INT2", align 8
  store i64 4, ptr %"%PS2_CFG_SYSTEM", align 8
  store i64 16, ptr %"%PS2_CFG_CLOCK1", align 8
  store i64 32, ptr %"%PS2_CFG_CLOCK2", align 8
  store i64 64, ptr %"%PS2_CFG_TRANS", align 8
  store i64 255, ptr %"%PS2_DEV_RESET", align 8
  store i64 244, ptr %"%PS2_DEV_ENABLE_SCAN", align 8
  store i64 245, ptr %"%PS2_DEV_DISABLE_SCAN", align 8
  store i64 242, ptr %"%PS2_DEV_IDENTIFY", align 8
  store i64 243, ptr %"%PS2_DEV_SET_RATE", align 8
  store i64 240, ptr %"%PS2_DEV_SET_SCANSET", align 8
  store i64 237, ptr %"%PS2_DEV_SET_LEDS", align 8
  store i64 250, ptr %"%PS2_RESP_ACK", align 8
  store i64 254, ptr %"%PS2_RESP_RESEND", align 8
  store i64 252, ptr %"%PS2_RESP_ERROR", align 8
  store i64 170, ptr %"%PS2_RESP_SELF_OK", align 8
  store i64 238, ptr %"%PS2_RESP_ECHO", align 8
  store i64 240, ptr %"%SC2_RELEASE", align 8
  store i64 224, ptr %"%SC2_EXTENDED", align 8
  store i64 225, ptr %"%SC2_EXTENDED2", align 8
  store i64 128, ptr %"%KEY_BUF_SIZE", align 8
  store i64 64, ptr %"%MOUSE_BUF_SIZE", align 8
  store i64 0, ptr %"%ps2_\D9\87\D9\8A\D8\A6_\D8\AA\D9\85", align 8
  store i64 0, ptr %"%ps2_port1_exists", align 8
  store i64 0, ptr %"%ps2_port2_exists", align 8
  store i64 0, ptr %"%ps2_dual_channel", align 8
  store i64 0, ptr %"%kbd_extended", align 8
  store i64 0, ptr %"%kbd_release", align 8
  store i64 0, ptr %"%kbd_shift_left", align 8
  store i64 0, ptr %"%kbd_shift_right", align 8
  store i64 0, ptr %"%kbd_ctrl_left", align 8
  store i64 0, ptr %"%kbd_ctrl_right", align 8
  store i64 0, ptr %"%kbd_alt_left", align 8
  store i64 0, ptr %"%kbd_alt_right", align 8
  store i64 0, ptr %"%kbd_capslock", align 8
  store i64 0, ptr %"%kbd_numlock", align 8
  store i64 0, ptr %"%kbd_scrolllock", align 8
  store i64 0, ptr %"%kbd_leds", align 8
  store i64 0, ptr %"%kbd_buf_scancode_0", align 8
  store i64 0, ptr %"%kbd_buf_scancode_1", align 8
  store i64 0, ptr %"%kbd_buf_scancode_2", align 8
  store i64 0, ptr %"%kbd_buf_scancode_3", align 8
  store i64 0, ptr %"%kbd_buf_scancode_4", align 8
  store i64 0, ptr %"%kbd_buf_scancode_5", align 8
  store i64 0, ptr %"%kbd_buf_scancode_6", align 8
  store i64 0, ptr %"%kbd_buf_scancode_7", align 8
  store i64 0, ptr %"%kbd_buf_scancode_8", align 8
  store i64 0, ptr %"%kbd_buf_scancode_9", align 8
  store i64 0, ptr %"%kbd_buf_scancode_10", align 8
  store i64 0, ptr %"%kbd_buf_scancode_11", align 8
  store i64 0, ptr %"%kbd_buf_scancode_12", align 8
  store i64 0, ptr %"%kbd_buf_scancode_13", align 8
  store i64 0, ptr %"%kbd_buf_scancode_14", align 8
  store i64 0, ptr %"%kbd_buf_scancode_15", align 8
  store i64 0, ptr %"%kbd_buf_scancode_16", align 8
  store i64 0, ptr %"%kbd_buf_scancode_17", align 8
  store i64 0, ptr %"%kbd_buf_scancode_18", align 8
  store i64 0, ptr %"%kbd_buf_scancode_19", align 8
  store i64 0, ptr %"%kbd_buf_scancode_20", align 8
  store i64 0, ptr %"%kbd_buf_scancode_21", align 8
  store i64 0, ptr %"%kbd_buf_scancode_22", align 8
  store i64 0, ptr %"%kbd_buf_scancode_23", align 8
  store i64 0, ptr %"%kbd_buf_scancode_24", align 8
  store i64 0, ptr %"%kbd_buf_scancode_25", align 8
  store i64 0, ptr %"%kbd_buf_scancode_26", align 8
  store i64 0, ptr %"%kbd_buf_scancode_27", align 8
  store i64 0, ptr %"%kbd_buf_scancode_28", align 8
  store i64 0, ptr %"%kbd_buf_scancode_29", align 8
  store i64 0, ptr %"%kbd_buf_scancode_30", align 8
  store i64 0, ptr %"%kbd_buf_scancode_31", align 8
  store i64 0, ptr %"%kbd_buf_ascii_0", align 8
  store i64 0, ptr %"%kbd_buf_ascii_1", align 8
  store i64 0, ptr %"%kbd_buf_ascii_2", align 8
  store i64 0, ptr %"%kbd_buf_ascii_3", align 8
  store i64 0, ptr %"%kbd_buf_ascii_4", align 8
  store i64 0, ptr %"%kbd_buf_ascii_5", align 8
  store i64 0, ptr %"%kbd_buf_ascii_6", align 8
  store i64 0, ptr %"%kbd_buf_ascii_7", align 8
  store i64 0, ptr %"%kbd_buf_ascii_8", align 8
  store i64 0, ptr %"%kbd_buf_ascii_9", align 8
  store i64 0, ptr %"%kbd_buf_ascii_10", align 8
  store i64 0, ptr %"%kbd_buf_ascii_11", align 8
  store i64 0, ptr %"%kbd_buf_ascii_12", align 8
  store i64 0, ptr %"%kbd_buf_ascii_13", align 8
  store i64 0, ptr %"%kbd_buf_ascii_14", align 8
  store i64 0, ptr %"%kbd_buf_ascii_15", align 8
  store i64 0, ptr %"%kbd_buf_ascii_16", align 8
  store i64 0, ptr %"%kbd_buf_ascii_17", align 8
  store i64 0, ptr %"%kbd_buf_ascii_18", align 8
  store i64 0, ptr %"%kbd_buf_ascii_19", align 8
  store i64 0, ptr %"%kbd_buf_ascii_20", align 8
  store i64 0, ptr %"%kbd_buf_ascii_21", align 8
  store i64 0, ptr %"%kbd_buf_ascii_22", align 8
  store i64 0, ptr %"%kbd_buf_ascii_23", align 8
  store i64 0, ptr %"%kbd_buf_ascii_24", align 8
  store i64 0, ptr %"%kbd_buf_ascii_25", align 8
  store i64 0, ptr %"%kbd_buf_ascii_26", align 8
  store i64 0, ptr %"%kbd_buf_ascii_27", align 8
  store i64 0, ptr %"%kbd_buf_ascii_28", align 8
  store i64 0, ptr %"%kbd_buf_ascii_29", align 8
  store i64 0, ptr %"%kbd_buf_ascii_30", align 8
  store i64 0, ptr %"%kbd_buf_ascii_31", align 8
  store i64 0, ptr %"%kbd_buf_head", align 8
  store i64 0, ptr %"%kbd_buf_tail", align 8
  store i64 0, ptr %"%kbd_buf_count", align 8
  store i64 320, ptr %"%mouse_x", align 8
  store i64 240, ptr %"%mouse_y", align 8
  store i64 0, ptr %"%mouse_buttons", align 8
  store i64 0, ptr %"%mouse_byte_idx", align 8
  store i64 0, ptr %"%mouse_byte0", align 8
  store i64 0, ptr %"%mouse_byte1", align 8
  store i64 0, ptr %"%mouse_byte2", align 8
  store i64 0, ptr %"%mouse_has_wheel", align 8
  store i64 0, ptr %"%mouse_byte3", align 8
  store i64 0, ptr %"%mouse_wheel", align 8
  store i64 0, ptr %"%mouse_phase", align 8
  store i64 639, ptr %"%mouse_max_x", align 8
  store i64 479, ptr %"%mouse_max_y", align 8
  store i64 0, ptr %"%ps2_kbd_irq_count", align 8
  store i64 0, ptr %"%ps2_mouse_irq_count", align 8
  store i64 0, ptr %"%ps2_errors", align 8
  store i64 0, ptr %"%sc2_ascii_0", align 8
  store i64 0, ptr %"%sc2_ascii_1", align 8
  store i64 0, ptr %"%sc2_ascii_2", align 8
  store i64 0, ptr %"%sc2_ascii_3", align 8
  store i64 0, ptr %"%sc2_ascii_4", align 8
  store i64 0, ptr %"%sc2_ascii_5", align 8
  store i64 0, ptr %"%sc2_ascii_6", align 8
  store i64 0, ptr %"%sc2_ascii_7", align 8
  store i64 0, ptr %"%sc2_ascii_8", align 8
  store i64 0, ptr %"%sc2_ascii_9", align 8
  store i64 0, ptr %"%sc2_ascii_10", align 8
  store i64 0, ptr %"%sc2_ascii_11", align 8
  store i64 0, ptr %"%sc2_ascii_12", align 8
  store i64 9, ptr %"%sc2_ascii_13", align 8
  store i64 96, ptr %"%sc2_ascii_14", align 8
  store i64 0, ptr %"%sc2_ascii_15", align 8
  store i64 0, ptr %"%sc2_ascii_16", align 8
  store i64 0, ptr %"%sc2_ascii_17", align 8
  store i64 0, ptr %"%sc2_ascii_18", align 8
  store i64 0, ptr %"%sc2_ascii_19", align 8
  store i64 0, ptr %"%sc2_ascii_20", align 8
  store i64 113, ptr %"%sc2_ascii_21", align 8
  store i64 49, ptr %"%sc2_ascii_22", align 8
  store i64 0, ptr %"%sc2_ascii_23", align 8
  store i64 0, ptr %"%sc2_ascii_24", align 8
  store i64 0, ptr %"%sc2_ascii_25", align 8
  store i64 122, ptr %"%sc2_ascii_26", align 8
  store i64 115, ptr %"%sc2_ascii_27", align 8
  store i64 97, ptr %"%sc2_ascii_28", align 8
  store i64 119, ptr %"%sc2_ascii_29", align 8
  store i64 50, ptr %"%sc2_ascii_30", align 8
  store i64 0, ptr %"%sc2_ascii_31", align 8
  store i64 0, ptr %"%sc2_ascii_32", align 8
  store i64 99, ptr %"%sc2_ascii_33", align 8
  store i64 120, ptr %"%sc2_ascii_34", align 8
  store i64 100, ptr %"%sc2_ascii_35", align 8
  store i64 101, ptr %"%sc2_ascii_36", align 8
  store i64 52, ptr %"%sc2_ascii_37", align 8
  store i64 51, ptr %"%sc2_ascii_38", align 8
  store i64 0, ptr %"%sc2_ascii_39", align 8
  store i64 0, ptr %"%sc2_ascii_40", align 8
  store i64 32, ptr %"%sc2_ascii_41", align 8
  store i64 118, ptr %"%sc2_ascii_42", align 8
  store i64 102, ptr %"%sc2_ascii_43", align 8
  store i64 116, ptr %"%sc2_ascii_44", align 8
  store i64 114, ptr %"%sc2_ascii_45", align 8
  store i64 53, ptr %"%sc2_ascii_46", align 8
  store i64 0, ptr %"%sc2_ascii_47", align 8
  store i64 0, ptr %"%sc2_ascii_48", align 8
  store i64 110, ptr %"%sc2_ascii_49", align 8
  store i64 98, ptr %"%sc2_ascii_50", align 8
  store i64 104, ptr %"%sc2_ascii_51", align 8
  store i64 103, ptr %"%sc2_ascii_52", align 8
  store i64 121, ptr %"%sc2_ascii_53", align 8
  store i64 54, ptr %"%sc2_ascii_54", align 8
  store i64 0, ptr %"%sc2_ascii_55", align 8
  store i64 0, ptr %"%sc2_ascii_56", align 8
  store i64 0, ptr %"%sc2_ascii_57", align 8
  store i64 109, ptr %"%sc2_ascii_58", align 8
  store i64 106, ptr %"%sc2_ascii_59", align 8
  store i64 117, ptr %"%sc2_ascii_60", align 8
  store i64 55, ptr %"%sc2_ascii_61", align 8
  store i64 56, ptr %"%sc2_ascii_62", align 8
  store i64 0, ptr %"%sc2_ascii_63", align 8
  store i64 0, ptr %"%sc2_ascii_64", align 8
  store i64 44, ptr %"%sc2_ascii_65", align 8
  store i64 107, ptr %"%sc2_ascii_66", align 8
  store i64 105, ptr %"%sc2_ascii_67", align 8
  store i64 111, ptr %"%sc2_ascii_68", align 8
  store i64 48, ptr %"%sc2_ascii_69", align 8
  store i64 57, ptr %"%sc2_ascii_70", align 8
  store i64 0, ptr %"%sc2_ascii_71", align 8
  store i64 0, ptr %"%sc2_ascii_72", align 8
  store i64 46, ptr %"%sc2_ascii_73", align 8
  store i64 47, ptr %"%sc2_ascii_74", align 8
  store i64 108, ptr %"%sc2_ascii_75", align 8
  store i64 59, ptr %"%sc2_ascii_76", align 8
  store i64 112, ptr %"%sc2_ascii_77", align 8
  store i64 45, ptr %"%sc2_ascii_78", align 8
  store i64 0, ptr %"%sc2_ascii_79", align 8
  store i64 0, ptr %"%sc2_ascii_80", align 8
  store i64 0, ptr %"%sc2_ascii_81", align 8
  store i64 39, ptr %"%sc2_ascii_82", align 8
  store i64 0, ptr %"%sc2_ascii_83", align 8
  store i64 91, ptr %"%sc2_ascii_84", align 8
  store i64 61, ptr %"%sc2_ascii_85", align 8
  store i64 0, ptr %"%sc2_ascii_86", align 8
  store i64 0, ptr %"%sc2_ascii_87", align 8
  store i64 0, ptr %"%sc2_ascii_88", align 8
  store i64 0, ptr %"%sc2_ascii_89", align 8
  store i64 13, ptr %"%sc2_ascii_90", align 8
  store i64 93, ptr %"%sc2_ascii_91", align 8
  store i64 0, ptr %"%sc2_ascii_92", align 8
  store i64 92, ptr %"%sc2_ascii_93", align 8
  store i64 0, ptr %"%sc2_ascii_94", align 8
  store i64 0, ptr %"%sc2_ascii_95", align 8
  store i64 0, ptr %"%sc2_ascii_96", align 8
  store i64 0, ptr %"%sc2_ascii_97", align 8
  store i64 0, ptr %"%sc2_ascii_98", align 8
  store i64 0, ptr %"%sc2_ascii_99", align 8
  store i64 0, ptr %"%sc2_ascii_100", align 8
  store i64 0, ptr %"%sc2_ascii_101", align 8
  store i64 8, ptr %"%sc2_ascii_102", align 8
  store i64 0, ptr %"%sc2_ascii_103", align 8
  store i64 0, ptr %"%sc2_ascii_104", align 8
  store i64 0, ptr %"%sc2_ascii_105", align 8
  store i64 0, ptr %"%sc2_ascii_106", align 8
  store i64 0, ptr %"%sc2_ascii_107", align 8
  store i64 0, ptr %"%sc2_ascii_108", align 8
  store i64 0, ptr %"%sc2_ascii_109", align 8
  store i64 0, ptr %"%sc2_ascii_110", align 8
  store i64 0, ptr %"%sc2_ascii_111", align 8
  store i64 0, ptr %"%sc2_ascii_112", align 8
  store i64 0, ptr %"%sc2_ascii_113", align 8
  store i64 0, ptr %"%sc2_ascii_114", align 8
  store i64 0, ptr %"%sc2_ascii_115", align 8
  store i64 0, ptr %"%sc2_ascii_116", align 8
  store i64 0, ptr %"%sc2_ascii_117", align 8
  store i64 27, ptr %"%sc2_ascii_118", align 8
  store i64 0, ptr %"%sc2_ascii_119", align 8
  store i64 0, ptr %"%sc2_ascii_120", align 8
  store i64 0, ptr %"%sc2_ascii_121", align 8
  store i64 0, ptr %"%sc2_ascii_122", align 8
  store i64 0, ptr %"%sc2_ascii_123", align 8
  store i64 0, ptr %"%sc2_ascii_124", align 8
  store i64 0, ptr %"%sc2_ascii_125", align 8
  store i64 0, ptr %"%sc2_ascii_126", align 8
  store i64 0, ptr %"%sc2_ascii_127", align 8
  store i64 126, ptr %"%sc2_shift_14", align 8
  store i64 81, ptr %"%sc2_shift_21", align 8
  store i64 33, ptr %"%sc2_shift_22", align 8
  store i64 90, ptr %"%sc2_shift_26", align 8
  store i64 83, ptr %"%sc2_shift_27", align 8
  store i64 65, ptr %"%sc2_shift_28", align 8
  store i64 87, ptr %"%sc2_shift_29", align 8
  store i64 64, ptr %"%sc2_shift_30", align 8
  store i64 67, ptr %"%sc2_shift_33", align 8
  store i64 88, ptr %"%sc2_shift_34", align 8
  store i64 68, ptr %"%sc2_shift_35", align 8
  store i64 69, ptr %"%sc2_shift_36", align 8
  store i64 36, ptr %"%sc2_shift_37", align 8
  store i64 35, ptr %"%sc2_shift_38", align 8
  store i64 86, ptr %"%sc2_shift_42", align 8
  store i64 70, ptr %"%sc2_shift_43", align 8
  store i64 84, ptr %"%sc2_shift_44", align 8
  store i64 82, ptr %"%sc2_shift_45", align 8
  store i64 37, ptr %"%sc2_shift_46", align 8
  store i64 78, ptr %"%sc2_shift_49", align 8
  store i64 66, ptr %"%sc2_shift_50", align 8
  store i64 72, ptr %"%sc2_shift_51", align 8
  store i64 71, ptr %"%sc2_shift_52", align 8
  store i64 89, ptr %"%sc2_shift_53", align 8
  store i64 94, ptr %"%sc2_shift_54", align 8
  store i64 77, ptr %"%sc2_shift_58", align 8
  store i64 74, ptr %"%sc2_shift_59", align 8
  store i64 85, ptr %"%sc2_shift_60", align 8
  store i64 38, ptr %"%sc2_shift_61", align 8
  store i64 42, ptr %"%sc2_shift_62", align 8
  store i64 60, ptr %"%sc2_shift_65", align 8
  store i64 75, ptr %"%sc2_shift_66", align 8
  store i64 73, ptr %"%sc2_shift_67", align 8
  store i64 79, ptr %"%sc2_shift_68", align 8
  store i64 41, ptr %"%sc2_shift_69", align 8
  store i64 40, ptr %"%sc2_shift_70", align 8
  store i64 62, ptr %"%sc2_shift_73", align 8
  store i64 63, ptr %"%sc2_shift_74", align 8
  store i64 76, ptr %"%sc2_shift_75", align 8
  store i64 58, ptr %"%sc2_shift_76", align 8
  store i64 80, ptr %"%sc2_shift_77", align 8
  store i64 95, ptr %"%sc2_shift_78", align 8
  store i64 34, ptr %"%sc2_shift_82", align 8
  store i64 123, ptr %"%sc2_shift_84", align 8
  store i64 43, ptr %"%sc2_shift_85", align 8
  store i64 125, ptr %"%sc2_shift_91", align 8
  store i64 124, ptr %"%sc2_shift_93", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
