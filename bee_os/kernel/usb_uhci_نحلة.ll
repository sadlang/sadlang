; ModuleID = 'usb_uhci_نحلة'
source_filename = "usb_uhci_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@COM1 = constant i64 1016
@UHCI_USBCMD = constant i64 0
@UHCI_USBSTS = constant i64 2
@UHCI_USBINTR = constant i64 4
@UHCI_FRNUM = constant i64 6
@UHCI_FRBASEADD = constant i64 8
@UHCI_SOFMOD = constant i64 12
@UHCI_PORTSC1 = constant i64 16
@UHCI_PORTSC2 = constant i64 18
@UHCI_CMD_RS = constant i64 1
@UHCI_CMD_HCRESET = constant i64 2
@UHCI_CMD_GRESET = constant i64 4
@UHCI_CMD_EGSM = constant i64 8
@UHCI_CMD_FGR = constant i64 16
@UHCI_CMD_SWDBG = constant i64 32
@UHCI_CMD_CF = constant i64 64
@UHCI_CMD_MAXP = constant i64 128
@UHCI_STS_USBINT = constant i64 1
@UHCI_STS_ERROR = constant i64 2
@UHCI_STS_RD = constant i64 4
@UHCI_STS_HSE = constant i64 8
@UHCI_STS_HCPE = constant i64 16
@UHCI_STS_HCH = constant i64 32
@UHCI_PORT_CONNECT = constant i64 1
@UHCI_PORT_CSC = constant i64 2
@UHCI_PORT_ENABLE = constant i64 4
@UHCI_PORT_ENC = constant i64 8
@UHCI_PORT_LS_LO = constant i64 16
@UHCI_PORT_LS_HI = constant i64 32
@UHCI_PORT_RD = constant i64 64
@UHCI_PORT_LSDA = constant i64 256
@UHCI_PORT_RESET = constant i64 512
@UHCI_PORT_SUSP = constant i64 4096
@UHCI_TD_ACTIVE = constant i64 8388608
@UHCI_TD_STALLED = constant i64 4194304
@UHCI_TD_DBUF_ERR = constant i64 2097152
@UHCI_TD_BABBLE = constant i64 1048576
@UHCI_TD_NAK = constant i64 524288
@UHCI_TD_CRC_ERR = constant i64 262144
@UHCI_TD_BITSTUFF = constant i64 131072
@UHCI_TD_IOC = constant i64 16777216
@UHCI_TD_IOS = constant i64 33554432
@UHCI_TD_LS = constant i64 67108864
@UHCI_TD_SPD = constant i64 536870912
@UHCI_PID_SETUP = constant i64 45
@UHCI_PID_IN = constant i64 105
@UHCI_PID_OUT = constant i64 225
@UHCI_FRAME_COUNT = constant i64 1024
@uhci_count = global i64 0
@uhci_base_0 = global i64 0
@uhci_base_1 = global i64 0
@uhci_base_2 = global i64 0
@uhci_base_3 = global i64 0
@uhci_framelist_0 = global i64 0
@uhci_framelist_1 = global i64 0
@uhci_framelist_2 = global i64 0
@uhci_framelist_3 = global i64 0
@uhci_qh_pool_0 = global i64 0
@uhci_qh_pool_1 = global i64 0
@uhci_qh_pool_2 = global i64 0
@uhci_qh_pool_3 = global i64 0
@uhci_td_pool_0 = global i64 0
@uhci_td_pool_1 = global i64 0
@uhci_td_pool_2 = global i64 0
@uhci_td_pool_3 = global i64 0
@uhci_data_buf_0 = global i64 0
@uhci_data_buf_1 = global i64 0
@uhci_data_buf_2 = global i64 0
@uhci_data_buf_3 = global i64 0
@uhci_setup_buf_0 = global i64 0
@uhci_setup_buf_1 = global i64 0
@uhci_setup_buf_2 = global i64 0
@uhci_setup_buf_3 = global i64 0
@uhci_status_0 = global i64 0
@uhci_status_1 = global i64 0
@uhci_status_2 = global i64 0
@uhci_status_3 = global i64 0
@uhci_port1_connected_0 = global i64 0
@uhci_port2_connected_0 = global i64 0
@uhci_port1_connected_1 = global i64 0
@uhci_port2_connected_1 = global i64 0
@uhci_port1_connected_2 = global i64 0
@uhci_port2_connected_2 = global i64 0
@uhci_port1_connected_3 = global i64 0
@uhci_port2_connected_3 = global i64 0
@uhci_port1_speed_0 = global i64 1
@uhci_port2_speed_0 = global i64 1
@uhci_port1_speed_1 = global i64 1
@uhci_port2_speed_1 = global i64 1
@uhci_port1_speed_2 = global i64 1
@uhci_port2_speed_2 = global i64 1
@uhci_port1_speed_3 = global i64 1
@uhci_port2_speed_3 = global i64 1
@uhci_td_used_0 = global i64 0
@uhci_td_used_1 = global i64 0
@uhci_td_used_2 = global i64 0
@uhci_td_used_3 = global i64 0
@uhci_total_transfers = global i64 0
@uhci_transfer_errors = global i64 0
@uhci_port_resets = global i64 0
@uhci_interrupts = global i64 0
@str.const = private unnamed_addr constant [22 x i8] c"[UHCI] Resetting HC #\00", align 1
@str.const.1 = private unnamed_addr constant [22 x i8] c"[UHCI] Reset timeout!\00", align 1
@str.const.2 = private unnamed_addr constant [22 x i8] c"[UHCI] Reset complete\00", align 1
@str.const.3 = private unnamed_addr constant [22 x i8] c"[UHCI] Port reset HC#\00", align 1
@str.const.4 = private unnamed_addr constant [7 x i8] c" port \00", align 1
@str.const.5 = private unnamed_addr constant [29 x i8] c"[UHCI] Port status: connect=\00", align 1
@str.const.6 = private unnamed_addr constant [10 x i8] c" enabled=\00", align 1
@str.const.7 = private unnamed_addr constant [11 x i8] c" lowspeed=\00", align 1
@str.const.8 = private unnamed_addr constant [25 x i8] c"[UHCI] Transfer timeout!\00", align 1
@str.const.9 = private unnamed_addr constant [22 x i8] c"[UHCI] SETUP TD error\00", align 1
@str.const.10 = private unnamed_addr constant [23 x i8] c"[UHCI] STATUS TD error\00", align 1
@str.const.11 = private unnamed_addr constant [21 x i8] c"[UHCI] DATA TD error\00", align 1
@str.const.12 = private unnamed_addr constant [33 x i8] c"[UHCI] Enumerating device on HC#\00", align 1
@str.const.14 = private unnamed_addr constant [40 x i8] c"[UHCI] Failed to get initial descriptor\00", align 1
@str.const.15 = private unnamed_addr constant [23 x i8] c"[UHCI] maxPacketSize0=\00", align 1
@str.const.16 = private unnamed_addr constant [26 x i8] c"[UHCI] SET_ADDRESS failed\00", align 1
@str.const.17 = private unnamed_addr constant [28 x i8] c"[UHCI] Device addressed as \00", align 1
@str.const.18 = private unnamed_addr constant [37 x i8] c"[UHCI] SET_CONFIG failed (non-fatal)\00", align 1
@str.const.19 = private unnamed_addr constant [35 x i8] c"[UHCI] Device enumeration complete\00", align 1
@str.const.20 = private unnamed_addr constant [29 x i8] c"[UHCI] Scanning ports on HC#\00", align 1
@str.const.21 = private unnamed_addr constant [32 x i8] c"[UHCI] Port 1: device connected\00", align 1
@str.const.22 = private unnamed_addr constant [25 x i8] c"[UHCI] Port 1: no device\00", align 1
@str.const.23 = private unnamed_addr constant [32 x i8] c"[UHCI] Port 2: device connected\00", align 1
@str.const.24 = private unnamed_addr constant [25 x i8] c"[UHCI] Port 2: no device\00", align 1
@str.const.25 = private unnamed_addr constant [38 x i8] c"\E2\95\90\E2\95\90\E2\95\90 [UHCI] \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.26 = private unnamed_addr constant [24 x i8] c"  \D9\85\D8\AA\D8\AD\D9\83\D9\85\D8\A7\D8\AA UHCI: \00", align 1
@str.const.27 = private unnamed_addr constant [28 x i8] c"  \D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D9\86\D9\82\D9\84: \00", align 1
@str.const.28 = private unnamed_addr constant [15 x i8] c"  \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1: \00", align 1
@str.const.29 = private unnamed_addr constant [37 x i8] c"  \D8\A5\D8\B9\D8\A7\D8\AF\D8\A9 \D8\AA\D8\B9\D9\8A\D9\8A\D9\86 \D9\85\D9\86\D8\A7\D9\81\D8\B0: \00", align 1
@str.const.30 = private unnamed_addr constant [6 x i8] c"  HC#\00", align 1
@str.const.31 = private unnamed_addr constant [9 x i8] c" base=0x\00", align 1
@str.const.32 = private unnamed_addr constant [9 x i8] c" status=\00", align 1
@str.const.33 = private unnamed_addr constant [106 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.34 = private unnamed_addr constant [27 x i8] c"[UHCI] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 UHCI HC#\00", align 1
@str.const.36 = private unnamed_addr constant [32 x i8] c"[UHCI] Max controllers reached!\00", align 1
@str.const.37 = private unnamed_addr constant [34 x i8] c"[UHCI] HC initialized and running\00", align 1
@str.const.38 = private unnamed_addr constant [34 x i8] c"[UHCI] Port 1 change detected HC#\00", align 1
@str.const.39 = private unnamed_addr constant [34 x i8] c"[UHCI] Port 2 change detected HC#\00", align 1
@str.const.40 = private unnamed_addr constant [27 x i8] c"[UHCI] Error interrupt HC#\00", align 1
@str.const.41 = private unnamed_addr constant [29 x i8] c"[UHCI] Host System Error HC#\00", align 1

declare void @sad_ll_port_inb(i64)

declare void @sad_ll_port_outb(i64, i64)

declare void @sad_ll_port_inw(i64)

declare void @sad_ll_port_outw(i64, i64)

declare void @sad_ll_port_inl(i64)

declare void @sad_ll_port_outl(i64, i64)

declare void @sad_ll_mem_read8(i64)

declare void @sad_ll_mem_write8(i64, i64)

declare void @sad_ll_mem_read32(i64)

declare void @sad_ll_mem_write32(i64, i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_kmalloc(i64)

declare void @sad_ll_kfree(i64)

declare void @sad_ll_memset(i64, i64, i64)

declare void @sad_ll_memcpy(i64, i64, i64)

declare void @sad_ll_timer_get_ticks()

declare void @usb_get_hc_base(i64)

declare void @usb_register_device(i64, i64, i64)

declare void @usb_build_setup(i64, i64, i64, i64, i64, i64)

declare void @usb_parse_device_descriptor(i64, i64)

declare void @usb_parse_config_descriptor(i64, i64, i64)

declare void @usb_classify_device(i64)

define i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
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

define i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
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

define i64 @uhci_serial_hex8(i64 %val) {
entry:
  %"%lo" = alloca i64, align 8
  %"%hi" = alloca i64, align 8
  %"%v" = alloca i64, align 8
  %val1 = alloca i64, align 8
  store i64 %val, ptr %val1, align 8
  %"%81.load" = load i64, ptr %val1, align 8
  store i64 %"%81.load", ptr %"%v", align 8
  %"%v.load" = load i64, ptr %"%v", align 8
  %shrtmp = lshr i64 %"%v.load", 4
  store i64 %shrtmp, ptr %"%hi", align 8
  %"%v.load2" = load i64, ptr %"%v", align 8
  %andtmp = and i64 %"%v.load2", 15
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

define i64 @uhci_serial_hex16(i64 %val) {
entry:
  %val1 = alloca i64, align 8
  store i64 %val, ptr %val1, align 8
  %"%val.load" = load i64, ptr %val1, align 8
  %shrtmp = lshr i64 %"%val.load", 8
  %calltmp = call i64 @uhci_serial_hex8(i64 %shrtmp)
  %"%val.load2" = load i64, ptr %val1, align 8
  %andtmp = and i64 %"%val.load2", 255
  %calltmp3 = call i64 @uhci_serial_hex8(i64 %andtmp)
  ret i64 0
}

define i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"() {
entry:
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 13)
  %"%COM1.load1" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load1", i64 10)
  ret i64 0
}

define i64 @uhci_get_base(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_30, label %merge_31

merge_31:                                         ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_32, label %merge_33

merge_33:                                         ; preds = %merge_31
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_34, label %merge_35

merge_35:                                         ; preds = %merge_33
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_36, label %merge_37

merge_37:                                         ; preds = %merge_35
  ret i64 0

then_30:                                          ; preds = %entry
  %loadtmp = load i64, ptr @uhci_base_0, align 8
  ret i64 %loadtmp

then_32:                                          ; preds = %merge_31
  %loadtmp4 = load i64, ptr @uhci_base_1, align 8
  ret i64 %loadtmp4

then_34:                                          ; preds = %merge_33
  %loadtmp7 = load i64, ptr @uhci_base_2, align 8
  ret i64 %loadtmp7

then_36:                                          ; preds = %merge_35
  %loadtmp10 = load i64, ptr @uhci_base_3, align 8
  ret i64 %loadtmp10
}

define i64 @uhci_set_base(i64 %idx, i64 %val) {
entry:
  %"%uhci_base_3" = alloca i64, align 8
  %"%uhci_base_2" = alloca i64, align 8
  %"%uhci_base_1" = alloca i64, align 8
  %"%uhci_base_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_38, label %merge_39

merge_39:                                         ; preds = %then_38, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_40, label %merge_41

merge_41:                                         ; preds = %then_40, %merge_39
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_42, label %merge_43

merge_43:                                         ; preds = %then_42, %merge_41
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_44, label %merge_45

merge_45:                                         ; preds = %then_44, %merge_43
  ret i64 0

then_38:                                          ; preds = %entry
  %"%139.load" = load i64, ptr %val2, align 8
  store i64 %"%139.load", ptr %"%uhci_base_0", align 8
  br label %merge_39

then_40:                                          ; preds = %merge_39
  %"%143.load" = load i64, ptr %val2, align 8
  store i64 %"%143.load", ptr %"%uhci_base_1", align 8
  br label %merge_41

then_42:                                          ; preds = %merge_41
  %"%147.load" = load i64, ptr %val2, align 8
  store i64 %"%147.load", ptr %"%uhci_base_2", align 8
  br label %merge_43

then_44:                                          ; preds = %merge_43
  %"%151.load" = load i64, ptr %val2, align 8
  store i64 %"%151.load", ptr %"%uhci_base_3", align 8
  br label %merge_45
}

define i64 @uhci_get_framelist(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_46, label %merge_47

merge_47:                                         ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_48, label %merge_49

merge_49:                                         ; preds = %merge_47
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_50, label %merge_51

merge_51:                                         ; preds = %merge_49
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_52, label %merge_53

merge_53:                                         ; preds = %merge_51
  ret i64 0

then_46:                                          ; preds = %entry
  %loadtmp = load i64, ptr @uhci_framelist_0, align 8
  ret i64 %loadtmp

then_48:                                          ; preds = %merge_47
  %loadtmp4 = load i64, ptr @uhci_framelist_1, align 8
  ret i64 %loadtmp4

then_50:                                          ; preds = %merge_49
  %loadtmp7 = load i64, ptr @uhci_framelist_2, align 8
  ret i64 %loadtmp7

then_52:                                          ; preds = %merge_51
  %loadtmp10 = load i64, ptr @uhci_framelist_3, align 8
  ret i64 %loadtmp10
}

define i64 @uhci_set_framelist(i64 %idx, i64 %val) {
entry:
  %"%uhci_framelist_3" = alloca i64, align 8
  %"%uhci_framelist_2" = alloca i64, align 8
  %"%uhci_framelist_1" = alloca i64, align 8
  %"%uhci_framelist_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_54, label %merge_55

merge_55:                                         ; preds = %then_54, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_56, label %merge_57

merge_57:                                         ; preds = %then_56, %merge_55
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_58, label %merge_59

merge_59:                                         ; preds = %then_58, %merge_57
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_60, label %merge_61

merge_61:                                         ; preds = %then_60, %merge_59
  ret i64 0

then_54:                                          ; preds = %entry
  %"%177.load" = load i64, ptr %val2, align 8
  store i64 %"%177.load", ptr %"%uhci_framelist_0", align 8
  br label %merge_55

then_56:                                          ; preds = %merge_55
  %"%181.load" = load i64, ptr %val2, align 8
  store i64 %"%181.load", ptr %"%uhci_framelist_1", align 8
  br label %merge_57

then_58:                                          ; preds = %merge_57
  %"%185.load" = load i64, ptr %val2, align 8
  store i64 %"%185.load", ptr %"%uhci_framelist_2", align 8
  br label %merge_59

then_60:                                          ; preds = %merge_59
  %"%189.load" = load i64, ptr %val2, align 8
  store i64 %"%189.load", ptr %"%uhci_framelist_3", align 8
  br label %merge_61
}

define i64 @uhci_get_qh_pool(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_62, label %merge_63

merge_63:                                         ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_64, label %merge_65

merge_65:                                         ; preds = %merge_63
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_66, label %merge_67

merge_67:                                         ; preds = %merge_65
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_68, label %merge_69

merge_69:                                         ; preds = %merge_67
  ret i64 0

then_62:                                          ; preds = %entry
  %loadtmp = load i64, ptr @uhci_qh_pool_0, align 8
  ret i64 %loadtmp

then_64:                                          ; preds = %merge_63
  %loadtmp4 = load i64, ptr @uhci_qh_pool_1, align 8
  ret i64 %loadtmp4

then_66:                                          ; preds = %merge_65
  %loadtmp7 = load i64, ptr @uhci_qh_pool_2, align 8
  ret i64 %loadtmp7

then_68:                                          ; preds = %merge_67
  %loadtmp10 = load i64, ptr @uhci_qh_pool_3, align 8
  ret i64 %loadtmp10
}

define i64 @uhci_set_qh_pool(i64 %idx, i64 %val) {
entry:
  %"%uhci_qh_pool_3" = alloca i64, align 8
  %"%uhci_qh_pool_2" = alloca i64, align 8
  %"%uhci_qh_pool_1" = alloca i64, align 8
  %"%uhci_qh_pool_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_70, label %merge_71

merge_71:                                         ; preds = %then_70, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_72, label %merge_73

merge_73:                                         ; preds = %then_72, %merge_71
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_74, label %merge_75

merge_75:                                         ; preds = %then_74, %merge_73
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_76, label %merge_77

merge_77:                                         ; preds = %then_76, %merge_75
  ret i64 0

then_70:                                          ; preds = %entry
  %"%215.load" = load i64, ptr %val2, align 8
  store i64 %"%215.load", ptr %"%uhci_qh_pool_0", align 8
  br label %merge_71

then_72:                                          ; preds = %merge_71
  %"%219.load" = load i64, ptr %val2, align 8
  store i64 %"%219.load", ptr %"%uhci_qh_pool_1", align 8
  br label %merge_73

then_74:                                          ; preds = %merge_73
  %"%223.load" = load i64, ptr %val2, align 8
  store i64 %"%223.load", ptr %"%uhci_qh_pool_2", align 8
  br label %merge_75

then_76:                                          ; preds = %merge_75
  %"%227.load" = load i64, ptr %val2, align 8
  store i64 %"%227.load", ptr %"%uhci_qh_pool_3", align 8
  br label %merge_77
}

define i64 @uhci_get_td_pool(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_78, label %merge_79

merge_79:                                         ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_80, label %merge_81

merge_81:                                         ; preds = %merge_79
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_82, label %merge_83

merge_83:                                         ; preds = %merge_81
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_84, label %merge_85

merge_85:                                         ; preds = %merge_83
  ret i64 0

then_78:                                          ; preds = %entry
  %loadtmp = load i64, ptr @uhci_td_pool_0, align 8
  ret i64 %loadtmp

then_80:                                          ; preds = %merge_79
  %loadtmp4 = load i64, ptr @uhci_td_pool_1, align 8
  ret i64 %loadtmp4

then_82:                                          ; preds = %merge_81
  %loadtmp7 = load i64, ptr @uhci_td_pool_2, align 8
  ret i64 %loadtmp7

then_84:                                          ; preds = %merge_83
  %loadtmp10 = load i64, ptr @uhci_td_pool_3, align 8
  ret i64 %loadtmp10
}

define i64 @uhci_set_td_pool(i64 %idx, i64 %val) {
entry:
  %"%uhci_td_pool_3" = alloca i64, align 8
  %"%uhci_td_pool_2" = alloca i64, align 8
  %"%uhci_td_pool_1" = alloca i64, align 8
  %"%uhci_td_pool_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_86, label %merge_87

merge_87:                                         ; preds = %then_86, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_88, label %merge_89

merge_89:                                         ; preds = %then_88, %merge_87
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_90, label %merge_91

merge_91:                                         ; preds = %then_90, %merge_89
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_92, label %merge_93

merge_93:                                         ; preds = %then_92, %merge_91
  ret i64 0

then_86:                                          ; preds = %entry
  %"%253.load" = load i64, ptr %val2, align 8
  store i64 %"%253.load", ptr %"%uhci_td_pool_0", align 8
  br label %merge_87

then_88:                                          ; preds = %merge_87
  %"%257.load" = load i64, ptr %val2, align 8
  store i64 %"%257.load", ptr %"%uhci_td_pool_1", align 8
  br label %merge_89

then_90:                                          ; preds = %merge_89
  %"%261.load" = load i64, ptr %val2, align 8
  store i64 %"%261.load", ptr %"%uhci_td_pool_2", align 8
  br label %merge_91

then_92:                                          ; preds = %merge_91
  %"%265.load" = load i64, ptr %val2, align 8
  store i64 %"%265.load", ptr %"%uhci_td_pool_3", align 8
  br label %merge_93
}

define i64 @uhci_get_data_buf(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_94, label %merge_95

merge_101:                                        ; preds = %merge_99
  ret i64 0

merge_95:                                         ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_96, label %merge_97

merge_97:                                         ; preds = %merge_95
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_98, label %merge_99

merge_99:                                         ; preds = %merge_97
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_100, label %merge_101

then_100:                                         ; preds = %merge_99
  %loadtmp10 = load i64, ptr @uhci_data_buf_3, align 8
  ret i64 %loadtmp10

then_94:                                          ; preds = %entry
  %loadtmp = load i64, ptr @uhci_data_buf_0, align 8
  ret i64 %loadtmp

then_96:                                          ; preds = %merge_95
  %loadtmp4 = load i64, ptr @uhci_data_buf_1, align 8
  ret i64 %loadtmp4

then_98:                                          ; preds = %merge_97
  %loadtmp7 = load i64, ptr @uhci_data_buf_2, align 8
  ret i64 %loadtmp7
}

define i64 @uhci_set_data_buf(i64 %idx, i64 %val) {
entry:
  %"%uhci_data_buf_3" = alloca i64, align 8
  %"%uhci_data_buf_2" = alloca i64, align 8
  %"%uhci_data_buf_1" = alloca i64, align 8
  %"%uhci_data_buf_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_102, label %merge_103

merge_103:                                        ; preds = %then_102, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_104, label %merge_105

merge_105:                                        ; preds = %then_104, %merge_103
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_106, label %merge_107

merge_107:                                        ; preds = %then_106, %merge_105
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_108, label %merge_109

merge_109:                                        ; preds = %then_108, %merge_107
  ret i64 0

then_102:                                         ; preds = %entry
  %"%291.load" = load i64, ptr %val2, align 8
  store i64 %"%291.load", ptr %"%uhci_data_buf_0", align 8
  br label %merge_103

then_104:                                         ; preds = %merge_103
  %"%295.load" = load i64, ptr %val2, align 8
  store i64 %"%295.load", ptr %"%uhci_data_buf_1", align 8
  br label %merge_105

then_106:                                         ; preds = %merge_105
  %"%299.load" = load i64, ptr %val2, align 8
  store i64 %"%299.load", ptr %"%uhci_data_buf_2", align 8
  br label %merge_107

then_108:                                         ; preds = %merge_107
  %"%303.load" = load i64, ptr %val2, align 8
  store i64 %"%303.load", ptr %"%uhci_data_buf_3", align 8
  br label %merge_109
}

define i64 @uhci_get_setup_buf(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_110, label %merge_111

merge_111:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_112, label %merge_113

merge_113:                                        ; preds = %merge_111
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_114, label %merge_115

merge_115:                                        ; preds = %merge_113
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_116, label %merge_117

merge_117:                                        ; preds = %merge_115
  ret i64 0

then_110:                                         ; preds = %entry
  %loadtmp = load i64, ptr @uhci_setup_buf_0, align 8
  ret i64 %loadtmp

then_112:                                         ; preds = %merge_111
  %loadtmp4 = load i64, ptr @uhci_setup_buf_1, align 8
  ret i64 %loadtmp4

then_114:                                         ; preds = %merge_113
  %loadtmp7 = load i64, ptr @uhci_setup_buf_2, align 8
  ret i64 %loadtmp7

then_116:                                         ; preds = %merge_115
  %loadtmp10 = load i64, ptr @uhci_setup_buf_3, align 8
  ret i64 %loadtmp10
}

define i64 @uhci_set_setup_buf(i64 %idx, i64 %val) {
entry:
  %"%uhci_setup_buf_3" = alloca i64, align 8
  %"%uhci_setup_buf_2" = alloca i64, align 8
  %"%uhci_setup_buf_1" = alloca i64, align 8
  %"%uhci_setup_buf_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_118, label %merge_119

merge_119:                                        ; preds = %then_118, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_120, label %merge_121

merge_121:                                        ; preds = %then_120, %merge_119
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_122, label %merge_123

merge_123:                                        ; preds = %then_122, %merge_121
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_124, label %merge_125

merge_125:                                        ; preds = %then_124, %merge_123
  ret i64 0

then_118:                                         ; preds = %entry
  %"%329.load" = load i64, ptr %val2, align 8
  store i64 %"%329.load", ptr %"%uhci_setup_buf_0", align 8
  br label %merge_119

then_120:                                         ; preds = %merge_119
  %"%333.load" = load i64, ptr %val2, align 8
  store i64 %"%333.load", ptr %"%uhci_setup_buf_1", align 8
  br label %merge_121

then_122:                                         ; preds = %merge_121
  %"%337.load" = load i64, ptr %val2, align 8
  store i64 %"%337.load", ptr %"%uhci_setup_buf_2", align 8
  br label %merge_123

then_124:                                         ; preds = %merge_123
  %"%341.load" = load i64, ptr %val2, align 8
  store i64 %"%341.load", ptr %"%uhci_setup_buf_3", align 8
  br label %merge_125
}

define i64 @uhci_get_status(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_126, label %merge_127

merge_127:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_128, label %merge_129

merge_129:                                        ; preds = %merge_127
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_130, label %merge_131

merge_131:                                        ; preds = %merge_129
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_132, label %merge_133

merge_133:                                        ; preds = %merge_131
  ret i64 0

then_126:                                         ; preds = %entry
  %loadtmp = load i64, ptr @uhci_status_0, align 8
  ret i64 %loadtmp

then_128:                                         ; preds = %merge_127
  %loadtmp4 = load i64, ptr @uhci_status_1, align 8
  ret i64 %loadtmp4

then_130:                                         ; preds = %merge_129
  %loadtmp7 = load i64, ptr @uhci_status_2, align 8
  ret i64 %loadtmp7

then_132:                                         ; preds = %merge_131
  %loadtmp10 = load i64, ptr @uhci_status_3, align 8
  ret i64 %loadtmp10
}

define i64 @uhci_set_status(i64 %idx, i64 %val) {
entry:
  %"%uhci_status_3" = alloca i64, align 8
  %"%uhci_status_2" = alloca i64, align 8
  %"%uhci_status_1" = alloca i64, align 8
  %"%uhci_status_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_134, label %merge_135

merge_135:                                        ; preds = %then_134, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_136, label %merge_137

merge_137:                                        ; preds = %then_136, %merge_135
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_138, label %merge_139

merge_139:                                        ; preds = %then_138, %merge_137
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_140, label %merge_141

merge_141:                                        ; preds = %then_140, %merge_139
  ret i64 0

then_134:                                         ; preds = %entry
  %"%367.load" = load i64, ptr %val2, align 8
  store i64 %"%367.load", ptr %"%uhci_status_0", align 8
  br label %merge_135

then_136:                                         ; preds = %merge_135
  %"%371.load" = load i64, ptr %val2, align 8
  store i64 %"%371.load", ptr %"%uhci_status_1", align 8
  br label %merge_137

then_138:                                         ; preds = %merge_137
  %"%375.load" = load i64, ptr %val2, align 8
  store i64 %"%375.load", ptr %"%uhci_status_2", align 8
  br label %merge_139

then_140:                                         ; preds = %merge_139
  %"%379.load" = load i64, ptr %val2, align 8
  store i64 %"%379.load", ptr %"%uhci_status_3", align 8
  br label %merge_141
}

define i64 @uhci_read16(i64 %hc_idx, i64 %reg) {
entry:
  %"%base" = alloca i64, align 8
  %reg2 = alloca i64, align 8
  store i64 %reg, ptr %reg2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @uhci_get_base(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%base", align 8
  %"%base.load" = load i64, ptr %"%base", align 8
  %"%reg.load" = load i64, ptr %reg2, align 8
  %addtmp = add i64 %"%base.load", %"%reg.load"
  call void @sad_ll_port_inw(i64 %addtmp)
  ret i64 0
}

define i64 @uhci_write16(i64 %hc_idx, i64 %reg, i64 %val) {
entry:
  %"%base" = alloca i64, align 8
  %val3 = alloca i64, align 8
  store i64 %val, ptr %val3, align 8
  %reg2 = alloca i64, align 8
  store i64 %reg, ptr %reg2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @uhci_get_base(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%base", align 8
  %"%base.load" = load i64, ptr %"%base", align 8
  %"%reg.load" = load i64, ptr %reg2, align 8
  %addtmp = add i64 %"%base.load", %"%reg.load"
  %"%394.load" = load i64, ptr %val3, align 8
  call void @sad_ll_port_outw(i64 %addtmp, i64 %"%394.load")
  ret i64 0
}

define i64 @uhci_read32(i64 %hc_idx, i64 %reg) {
entry:
  %"%base" = alloca i64, align 8
  %reg2 = alloca i64, align 8
  store i64 %reg, ptr %reg2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @uhci_get_base(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%base", align 8
  %"%base.load" = load i64, ptr %"%base", align 8
  %"%reg.load" = load i64, ptr %reg2, align 8
  %addtmp = add i64 %"%base.load", %"%reg.load"
  call void @sad_ll_port_inl(i64 %addtmp)
  ret i64 0
}

define i64 @uhci_write32(i64 %hc_idx, i64 %reg, i64 %val) {
entry:
  %"%base" = alloca i64, align 8
  %val3 = alloca i64, align 8
  store i64 %val, ptr %val3, align 8
  %reg2 = alloca i64, align 8
  store i64 %reg, ptr %reg2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @uhci_get_base(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%base", align 8
  %"%base.load" = load i64, ptr %"%base", align 8
  %"%reg.load" = load i64, ptr %reg2, align 8
  %addtmp = add i64 %"%base.load", %"%reg.load"
  %"%410.load" = load i64, ptr %val3, align 8
  call void @sad_ll_port_outl(i64 %addtmp, i64 %"%410.load")
  ret i64 0
}

define i64 @uhci_read8(i64 %hc_idx, i64 %reg) {
entry:
  %"%base" = alloca i64, align 8
  %reg2 = alloca i64, align 8
  store i64 %reg, ptr %reg2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @uhci_get_base(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%base", align 8
  %"%base.load" = load i64, ptr %"%base", align 8
  %"%reg.load" = load i64, ptr %reg2, align 8
  %addtmp = add i64 %"%base.load", %"%reg.load"
  call void @sad_ll_port_inb(i64 %addtmp)
  ret i64 0
}

define i64 @uhci_reset(i64 %hc_idx) {
entry:
  %"%d2" = alloca i64, align 8
  %"%w2" = alloca i64, align 8
  %"%cmd" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  %"%dummy" = alloca i64, align 8
  %"%wait_start" = alloca i64, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %calltmp = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %"%424.load" = load i64, ptr %hc_idx1, align 8
  %calltmp2 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%424.load")
  %calltmp3 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_USBCMD.load" = load i64, ptr @UHCI_USBCMD, align 8
  %"%UHCI_CMD_GRESET.load" = load i64, ptr @UHCI_CMD_GRESET, align 8
  %calltmp4 = call i64 @uhci_write16(i64 %"%hc_idx.load", i64 %"%UHCI_USBCMD.load", i64 %"%UHCI_CMD_GRESET.load")
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%wait_start", align 8
  br label %while_cond_142

merge_149:                                        ; preds = %while_body_146
  %"%timeout.load18" = load i64, ptr %"%timeout", align 8
  %subtmp19 = sub i64 %"%timeout.load18", 1
  store i64 %subtmp19, ptr %"%timeout", align 8
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%w2", align 8
  br label %while_cond_150

then_148:                                         ; preds = %while_body_146
  %calltmp16 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %calltmp17 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1

while_body_143:                                   ; preds = %while_cond_142
  store i64 0, ptr %"%dummy", align 8
  br label %while_cond_142

while_body_146:                                   ; preds = %while_cond_145
  %"%hc_idx.load11" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_USBCMD.load12" = load i64, ptr @UHCI_USBCMD, align 8
  %calltmp13 = call i64 @uhci_read16(i64 %"%hc_idx.load11", i64 %"%UHCI_USBCMD.load12")
  store i64 %calltmp13, ptr %"%cmd", align 8
  %"%cmd.load" = load i64, ptr %"%cmd", align 8
  %andtmp = and i64 %"%cmd.load", 3
  %shrtmp = lshr i64 %andtmp, 1
  %cmpeqtmp = icmp eq i64 %shrtmp, 0
  br i1 %cmpeqtmp, label %then_148, label %merge_149

while_body_151:                                   ; preds = %while_cond_150
  store i64 0, ptr %"%d2", align 8
  br label %while_cond_150

while_cond_142:                                   ; preds = %while_body_143, %entry
  call void @sad_ll_timer_get_ticks()
  %"%wait_start.load" = load i64, ptr %"%wait_start", align 8
  %subtmp = sub i64 0, %"%wait_start.load"
  %cmplttmp = icmp slt i64 %subtmp, 5000
  br i1 %cmplttmp, label %while_body_143, label %while_exit_144

while_cond_145:                                   ; preds = %while_exit_152, %while_exit_144
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp, label %while_body_146, label %while_exit_147

while_cond_150:                                   ; preds = %while_body_151, %merge_149
  call void @sad_ll_timer_get_ticks()
  %"%w2.load" = load i64, ptr %"%w2", align 8
  %subtmp20 = sub i64 0, %"%w2.load"
  %cmplttmp21 = icmp slt i64 %subtmp20, 100
  br i1 %cmplttmp21, label %while_body_151, label %while_exit_152

while_exit_144:                                   ; preds = %while_cond_142
  %"%hc_idx.load5" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_USBCMD.load6" = load i64, ptr @UHCI_USBCMD, align 8
  %calltmp7 = call i64 @uhci_write16(i64 %"%hc_idx.load5", i64 %"%UHCI_USBCMD.load6", i64 0)
  %"%hc_idx.load8" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_USBCMD.load9" = load i64, ptr @UHCI_USBCMD, align 8
  %"%UHCI_CMD_HCRESET.load" = load i64, ptr @UHCI_CMD_HCRESET, align 8
  %calltmp10 = call i64 @uhci_write16(i64 %"%hc_idx.load8", i64 %"%UHCI_USBCMD.load9", i64 %"%UHCI_CMD_HCRESET.load")
  store i64 100, ptr %"%timeout", align 8
  br label %while_cond_145

while_exit_147:                                   ; preds = %while_cond_145
  %calltmp14 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %calltmp15 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

while_exit_152:                                   ; preds = %while_cond_150
  br label %while_cond_145
}

define i64 @uhci_td_set_link(i64 %td_addr, i64 %next_td, i64 %terminate, i64 %is_qh) {
entry:
  %"%val" = alloca i64, align 8
  %is_qh4 = alloca i64, align 8
  store i64 %is_qh, ptr %is_qh4, align 8
  %terminate3 = alloca i64, align 8
  store i64 %terminate, ptr %terminate3, align 8
  %next_td2 = alloca i64, align 8
  store i64 %next_td, ptr %next_td2, align 8
  %td_addr1 = alloca i64, align 8
  store i64 %td_addr, ptr %td_addr1, align 8
  %"%474.load" = load i64, ptr %next_td2, align 8
  store i64 %"%474.load", ptr %"%val", align 8
  %"%terminate.load" = load i64, ptr %terminate3, align 8
  %cmpeqtmp = icmp eq i64 %"%terminate.load", 1
  br i1 %cmpeqtmp, label %then_153, label %merge_154

merge_154:                                        ; preds = %then_153, %entry
  %"%is_qh.load" = load i64, ptr %is_qh4, align 8
  %cmpeqtmp5 = icmp eq i64 %"%is_qh.load", 1
  br i1 %cmpeqtmp5, label %then_155, label %merge_156

merge_156:                                        ; preds = %then_155, %merge_154
  %"%td_addr.load" = load i64, ptr %td_addr1, align 8
  %"%val.load6" = load i64, ptr %"%val", align 8
  call void @sad_ll_mem_write32(i64 %"%td_addr.load", i64 %"%val.load6")
  ret i64 0

then_153:                                         ; preds = %entry
  store i64 1, ptr %"%val", align 8
  br label %merge_154

then_155:                                         ; preds = %merge_154
  %"%val.load" = load i64, ptr %"%val", align 8
  %addtmp = add i64 %"%val.load", 2
  store i64 %addtmp, ptr %"%val", align 8
  br label %merge_156
}

define i64 @uhci_td_set_status(i64 %td_addr, i64 %active, i64 %ioc, i64 %ls_dev, i64 %cerr_count) {
entry:
  %"%val" = alloca i64, align 8
  %cerr_count5 = alloca i64, align 8
  store i64 %cerr_count, ptr %cerr_count5, align 8
  %ls_dev4 = alloca i64, align 8
  store i64 %ls_dev, ptr %ls_dev4, align 8
  %ioc3 = alloca i64, align 8
  store i64 %ioc, ptr %ioc3, align 8
  %active2 = alloca i64, align 8
  store i64 %active, ptr %active2, align 8
  %td_addr1 = alloca i64, align 8
  store i64 %td_addr, ptr %td_addr1, align 8
  store i64 0, ptr %"%val", align 8
  %"%active.load" = load i64, ptr %active2, align 8
  %cmpeqtmp = icmp eq i64 %"%active.load", 1
  br i1 %cmpeqtmp, label %then_157, label %merge_158

merge_158:                                        ; preds = %then_157, %entry
  %"%ioc.load" = load i64, ptr %ioc3, align 8
  %cmpeqtmp6 = icmp eq i64 %"%ioc.load", 1
  br i1 %cmpeqtmp6, label %then_159, label %merge_160

merge_160:                                        ; preds = %then_159, %merge_158
  %"%ls_dev.load" = load i64, ptr %ls_dev4, align 8
  %cmpeqtmp9 = icmp eq i64 %"%ls_dev.load", 1
  br i1 %cmpeqtmp9, label %then_161, label %merge_162

merge_162:                                        ; preds = %then_161, %merge_160
  %"%cerr_count.load" = load i64, ptr %cerr_count5, align 8
  %shltmp = shl i64 %"%cerr_count.load", 27
  %"%val.load12" = load i64, ptr %"%val", align 8
  %addtmp13 = add i64 %"%val.load12", %shltmp
  store i64 %addtmp13, ptr %"%val", align 8
  %"%td_addr.load" = load i64, ptr %td_addr1, align 8
  %addtmp14 = add i64 %"%td_addr.load", 4
  %"%val.load15" = load i64, ptr %"%val", align 8
  call void @sad_ll_mem_write32(i64 %addtmp14, i64 %"%val.load15")
  ret i64 0

then_157:                                         ; preds = %entry
  %"%val.load" = load i64, ptr %"%val", align 8
  %"%UHCI_TD_ACTIVE.load" = load i64, ptr @UHCI_TD_ACTIVE, align 8
  %addtmp = add i64 %"%val.load", %"%UHCI_TD_ACTIVE.load"
  store i64 %addtmp, ptr %"%val", align 8
  br label %merge_158

then_159:                                         ; preds = %merge_158
  %"%val.load7" = load i64, ptr %"%val", align 8
  %"%UHCI_TD_IOC.load" = load i64, ptr @UHCI_TD_IOC, align 8
  %addtmp8 = add i64 %"%val.load7", %"%UHCI_TD_IOC.load"
  store i64 %addtmp8, ptr %"%val", align 8
  br label %merge_160

then_161:                                         ; preds = %merge_160
  %"%val.load10" = load i64, ptr %"%val", align 8
  %"%UHCI_TD_LS.load" = load i64, ptr @UHCI_TD_LS, align 8
  %addtmp11 = add i64 %"%val.load10", %"%UHCI_TD_LS.load"
  store i64 %addtmp11, ptr %"%val", align 8
  br label %merge_162
}

define i64 @uhci_td_set_token(i64 %td_addr, i64 %pid, i64 %dev_addr, i64 %endpoint, i64 %toggle, i64 %max_len) {
entry:
  %"%ml" = alloca i64, align 8
  %"%val" = alloca i64, align 8
  %max_len6 = alloca i64, align 8
  store i64 %max_len, ptr %max_len6, align 8
  %toggle5 = alloca i64, align 8
  store i64 %toggle, ptr %toggle5, align 8
  %endpoint4 = alloca i64, align 8
  store i64 %endpoint, ptr %endpoint4, align 8
  %dev_addr3 = alloca i64, align 8
  store i64 %dev_addr, ptr %dev_addr3, align 8
  %pid2 = alloca i64, align 8
  store i64 %pid, ptr %pid2, align 8
  %td_addr1 = alloca i64, align 8
  store i64 %td_addr, ptr %td_addr1, align 8
  %"%504.load" = load i64, ptr %pid2, align 8
  store i64 %"%504.load", ptr %"%val", align 8
  %"%dev_addr.load" = load i64, ptr %dev_addr3, align 8
  %shltmp = shl i64 %"%dev_addr.load", 8
  %"%val.load" = load i64, ptr %"%val", align 8
  %addtmp = add i64 %"%val.load", %shltmp
  store i64 %addtmp, ptr %"%val", align 8
  %"%endpoint.load" = load i64, ptr %endpoint4, align 8
  %shltmp7 = shl i64 %"%endpoint.load", 15
  %"%val.load8" = load i64, ptr %"%val", align 8
  %addtmp9 = add i64 %"%val.load8", %shltmp7
  store i64 %addtmp9, ptr %"%val", align 8
  %"%toggle.load" = load i64, ptr %toggle5, align 8
  %cmpeqtmp = icmp eq i64 %"%toggle.load", 1
  br i1 %cmpeqtmp, label %then_163, label %merge_164

else_167:                                         ; preds = %merge_164
  store i64 2047, ptr %"%ml", align 8
  br label %merge_166

merge_164:                                        ; preds = %then_163, %entry
  store i64 0, ptr %"%ml", align 8
  %"%max_len.load" = load i64, ptr %max_len6, align 8
  %cmpgttmp = icmp sgt i64 %"%max_len.load", 0
  br i1 %cmpgttmp, label %then_165, label %else_167

merge_166:                                        ; preds = %else_167, %then_165
  %"%ml.load" = load i64, ptr %"%ml", align 8
  %shltmp13 = shl i64 %"%ml.load", 21
  %"%val.load14" = load i64, ptr %"%val", align 8
  %addtmp15 = add i64 %"%val.load14", %shltmp13
  store i64 %addtmp15, ptr %"%val", align 8
  %"%td_addr.load" = load i64, ptr %td_addr1, align 8
  %addtmp16 = add i64 %"%td_addr.load", 8
  %"%val.load17" = load i64, ptr %"%val", align 8
  call void @sad_ll_mem_write32(i64 %addtmp16, i64 %"%val.load17")
  ret i64 0

then_163:                                         ; preds = %entry
  %"%val.load10" = load i64, ptr %"%val", align 8
  %addtmp11 = add i64 %"%val.load10", 524288
  store i64 %addtmp11, ptr %"%val", align 8
  br label %merge_164

then_165:                                         ; preds = %merge_164
  %"%max_len.load12" = load i64, ptr %max_len6, align 8
  %subtmp = sub i64 %"%max_len.load12", 1
  store i64 %subtmp, ptr %"%ml", align 8
  br label %merge_166
}

define i64 @uhci_td_set_buffer(i64 %td_addr, i64 %buf_phys) {
entry:
  %buf_phys2 = alloca i64, align 8
  store i64 %buf_phys, ptr %buf_phys2, align 8
  %td_addr1 = alloca i64, align 8
  store i64 %td_addr, ptr %td_addr1, align 8
  %"%td_addr.load" = load i64, ptr %td_addr1, align 8
  %addtmp = add i64 %"%td_addr.load", 12
  %"%533.load" = load i64, ptr %buf_phys2, align 8
  call void @sad_ll_mem_write32(i64 %addtmp, i64 %"%533.load")
  ret i64 0
}

define i64 @uhci_td_get_status(i64 %td_addr) {
entry:
  %td_addr1 = alloca i64, align 8
  store i64 %td_addr, ptr %td_addr1, align 8
  %"%td_addr.load" = load i64, ptr %td_addr1, align 8
  %addtmp = add i64 %"%td_addr.load", 4
  call void @sad_ll_mem_read32(i64 %addtmp)
  ret i64 0
}

define i64 @uhci_td_is_active(i64 %td_addr) {
entry:
  %"%bit23" = alloca i64, align 8
  %"%st" = alloca i64, align 8
  %td_addr1 = alloca i64, align 8
  store i64 %td_addr, ptr %td_addr1, align 8
  %"%td_addr.load" = load i64, ptr %td_addr1, align 8
  %calltmp = call i64 @uhci_td_get_status(i64 %"%td_addr.load")
  store i64 %calltmp, ptr %"%st", align 8
  %"%st.load" = load i64, ptr %"%st", align 8
  %"%UHCI_TD_ACTIVE.load" = load i64, ptr @UHCI_TD_ACTIVE, align 8
  %divtmp = sdiv i64 %"%st.load", %"%UHCI_TD_ACTIVE.load"
  %andtmp = and i64 %divtmp, 1
  store i64 %andtmp, ptr %"%bit23", align 8
  %loadtmp = load i64, ptr %"%bit23", align 8
  ret i64 %loadtmp
}

define i64 @uhci_td_has_error(i64 %td_addr) {
entry:
  %"%bitstuff" = alloca i64, align 8
  %"%crc" = alloca i64, align 8
  %"%babble" = alloca i64, align 8
  %"%dbuf" = alloca i64, align 8
  %"%stalled" = alloca i64, align 8
  %"%st" = alloca i64, align 8
  %td_addr1 = alloca i64, align 8
  store i64 %td_addr, ptr %td_addr1, align 8
  %"%td_addr.load" = load i64, ptr %td_addr1, align 8
  %calltmp = call i64 @uhci_td_get_status(i64 %"%td_addr.load")
  store i64 %calltmp, ptr %"%st", align 8
  %"%st.load" = load i64, ptr %"%st", align 8
  %"%UHCI_TD_STALLED.load" = load i64, ptr @UHCI_TD_STALLED, align 8
  %divtmp = sdiv i64 %"%st.load", %"%UHCI_TD_STALLED.load"
  %andtmp = and i64 %divtmp, 1
  store i64 %andtmp, ptr %"%stalled", align 8
  %"%st.load2" = load i64, ptr %"%st", align 8
  %"%UHCI_TD_DBUF_ERR.load" = load i64, ptr @UHCI_TD_DBUF_ERR, align 8
  %divtmp3 = sdiv i64 %"%st.load2", %"%UHCI_TD_DBUF_ERR.load"
  %andtmp4 = and i64 %divtmp3, 1
  store i64 %andtmp4, ptr %"%dbuf", align 8
  %"%st.load5" = load i64, ptr %"%st", align 8
  %"%UHCI_TD_BABBLE.load" = load i64, ptr @UHCI_TD_BABBLE, align 8
  %divtmp6 = sdiv i64 %"%st.load5", %"%UHCI_TD_BABBLE.load"
  %andtmp7 = and i64 %divtmp6, 1
  store i64 %andtmp7, ptr %"%babble", align 8
  %"%st.load8" = load i64, ptr %"%st", align 8
  %"%UHCI_TD_CRC_ERR.load" = load i64, ptr @UHCI_TD_CRC_ERR, align 8
  %divtmp9 = sdiv i64 %"%st.load8", %"%UHCI_TD_CRC_ERR.load"
  %andtmp10 = and i64 %divtmp9, 1
  store i64 %andtmp10, ptr %"%crc", align 8
  %"%st.load11" = load i64, ptr %"%st", align 8
  %"%UHCI_TD_BITSTUFF.load" = load i64, ptr @UHCI_TD_BITSTUFF, align 8
  %divtmp12 = sdiv i64 %"%st.load11", %"%UHCI_TD_BITSTUFF.load"
  %andtmp13 = and i64 %divtmp12, 1
  store i64 %andtmp13, ptr %"%bitstuff", align 8
  %"%stalled.load" = load i64, ptr %"%stalled", align 8
  %"%dbuf.load" = load i64, ptr %"%dbuf", align 8
  %addtmp = add i64 %"%stalled.load", %"%dbuf.load"
  %"%babble.load" = load i64, ptr %"%babble", align 8
  %addtmp14 = add i64 %addtmp, %"%babble.load"
  %"%crc.load" = load i64, ptr %"%crc", align 8
  %addtmp15 = add i64 %addtmp14, %"%crc.load"
  %"%bitstuff.load" = load i64, ptr %"%bitstuff", align 8
  %addtmp16 = add i64 %addtmp15, %"%bitstuff.load"
  %cmpgttmp = icmp sgt i64 %addtmp16, 0
  br i1 %cmpgttmp, label %then_168, label %merge_169

merge_169:                                        ; preds = %entry
  ret i64 0

then_168:                                         ; preds = %entry
  ret i64 1
}

define i64 @uhci_td_actual_len(i64 %td_addr) {
entry:
  %"%actlen" = alloca i64, align 8
  %"%st" = alloca i64, align 8
  %td_addr1 = alloca i64, align 8
  store i64 %td_addr, ptr %td_addr1, align 8
  %"%td_addr.load" = load i64, ptr %td_addr1, align 8
  %calltmp = call i64 @uhci_td_get_status(i64 %"%td_addr.load")
  store i64 %calltmp, ptr %"%st", align 8
  %"%st.load" = load i64, ptr %"%st", align 8
  %andtmp = and i64 %"%st.load", 2047
  store i64 %andtmp, ptr %"%actlen", align 8
  %"%actlen.load" = load i64, ptr %"%actlen", align 8
  %cmpeqtmp = icmp eq i64 %"%actlen.load", 2047
  br i1 %cmpeqtmp, label %then_170, label %merge_171

merge_171:                                        ; preds = %entry
  %"%actlen.load2" = load i64, ptr %"%actlen", align 8
  %addtmp = add i64 %"%actlen.load2", 1
  ret i64 %addtmp

then_170:                                         ; preds = %entry
  ret i64 0
}

define i64 @uhci_qh_set_head_link(i64 %qh_addr, i64 %next_qh, i64 %terminate) {
entry:
  %"%val" = alloca i64, align 8
  %terminate3 = alloca i64, align 8
  store i64 %terminate, ptr %terminate3, align 8
  %next_qh2 = alloca i64, align 8
  store i64 %next_qh, ptr %next_qh2, align 8
  %qh_addr1 = alloca i64, align 8
  store i64 %qh_addr, ptr %qh_addr1, align 8
  %"%589.load" = load i64, ptr %next_qh2, align 8
  store i64 %"%589.load", ptr %"%val", align 8
  %"%terminate.load" = load i64, ptr %terminate3, align 8
  %cmpeqtmp = icmp eq i64 %"%terminate.load", 1
  br i1 %cmpeqtmp, label %then_172, label %else_174

else_174:                                         ; preds = %entry
  %"%val.load" = load i64, ptr %"%val", align 8
  %addtmp = add i64 %"%val.load", 2
  store i64 %addtmp, ptr %"%val", align 8
  br label %merge_173

merge_173:                                        ; preds = %else_174, %then_172
  %"%qh_addr.load" = load i64, ptr %qh_addr1, align 8
  %"%val.load4" = load i64, ptr %"%val", align 8
  call void @sad_ll_mem_write32(i64 %"%qh_addr.load", i64 %"%val.load4")
  ret i64 0

then_172:                                         ; preds = %entry
  store i64 1, ptr %"%val", align 8
  br label %merge_173
}

define i64 @uhci_qh_set_element(i64 %qh_addr, i64 %first_td, i64 %terminate) {
entry:
  %"%val" = alloca i64, align 8
  %terminate3 = alloca i64, align 8
  store i64 %terminate, ptr %terminate3, align 8
  %first_td2 = alloca i64, align 8
  store i64 %first_td, ptr %first_td2, align 8
  %qh_addr1 = alloca i64, align 8
  store i64 %qh_addr, ptr %qh_addr1, align 8
  %"%598.load" = load i64, ptr %first_td2, align 8
  store i64 %"%598.load", ptr %"%val", align 8
  %"%terminate.load" = load i64, ptr %terminate3, align 8
  %cmpeqtmp = icmp eq i64 %"%terminate.load", 1
  br i1 %cmpeqtmp, label %then_175, label %merge_176

merge_176:                                        ; preds = %then_175, %entry
  %"%qh_addr.load" = load i64, ptr %qh_addr1, align 8
  %addtmp = add i64 %"%qh_addr.load", 4
  %"%val.load" = load i64, ptr %"%val", align 8
  call void @sad_ll_mem_write32(i64 %addtmp, i64 %"%val.load")
  ret i64 0

then_175:                                         ; preds = %entry
  store i64 1, ptr %"%val", align 8
  br label %merge_176
}

define i64 @uhci_alloc_td(i64 %hc_idx) {
entry:
  %"%uhci_td_used_3" = alloca i64, align 8
  %"%uhci_td_used_2" = alloca i64, align 8
  %"%uhci_td_used_1" = alloca i64, align 8
  %"%uhci_td_used_0" = alloca i64, align 8
  %"%td_addr" = alloca i64, align 8
  %"%pool" = alloca i64, align 8
  %"%used" = alloca i64, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  store i64 0, ptr %"%used", align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%hc_idx.load", 0
  br i1 %cmpeqtmp, label %then_177, label %merge_178

merge_178:                                        ; preds = %then_177, %entry
  %"%hc_idx.load2" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%hc_idx.load2", 1
  br i1 %cmpeqtmp3, label %then_179, label %merge_180

merge_180:                                        ; preds = %then_179, %merge_178
  %"%hc_idx.load4" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp5 = icmp eq i64 %"%hc_idx.load4", 2
  br i1 %cmpeqtmp5, label %then_181, label %merge_182

merge_182:                                        ; preds = %then_181, %merge_180
  %"%hc_idx.load6" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp7 = icmp eq i64 %"%hc_idx.load6", 3
  br i1 %cmpeqtmp7, label %then_183, label %merge_184

merge_184:                                        ; preds = %then_183, %merge_182
  %"%used.load" = load i64, ptr %"%used", align 8
  %cmpgetmp = icmp sge i64 %"%used.load", 32
  br i1 %cmpgetmp, label %then_185, label %merge_186

merge_186:                                        ; preds = %merge_184
  %"%hc_idx.load8" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @uhci_get_td_pool(i64 %"%hc_idx.load8")
  store i64 %calltmp, ptr %"%pool", align 8
  %"%used.load9" = load i64, ptr %"%used", align 8
  %shltmp = shl i64 %"%used.load9", 5
  %"%pool.load" = load i64, ptr %"%pool", align 8
  %addtmp = add i64 %"%pool.load", %shltmp
  store i64 %addtmp, ptr %"%td_addr", align 8
  %"%td_addr.load" = load i64, ptr %"%td_addr", align 8
  call void @sad_ll_mem_write32(i64 %"%td_addr.load", i64 1)
  %"%td_addr.load10" = load i64, ptr %"%td_addr", align 8
  %addtmp11 = add i64 %"%td_addr.load10", 4
  call void @sad_ll_mem_write32(i64 %addtmp11, i64 0)
  %"%td_addr.load12" = load i64, ptr %"%td_addr", align 8
  %addtmp13 = add i64 %"%td_addr.load12", 8
  call void @sad_ll_mem_write32(i64 %addtmp13, i64 0)
  %"%td_addr.load14" = load i64, ptr %"%td_addr", align 8
  %addtmp15 = add i64 %"%td_addr.load14", 12
  call void @sad_ll_mem_write32(i64 %addtmp15, i64 0)
  %"%hc_idx.load16" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp17 = icmp eq i64 %"%hc_idx.load16", 0
  br i1 %cmpeqtmp17, label %then_187, label %merge_188

merge_188:                                        ; preds = %then_187, %merge_186
  %"%hc_idx.load20" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%hc_idx.load20", 1
  br i1 %cmpeqtmp21, label %then_189, label %merge_190

merge_190:                                        ; preds = %then_189, %merge_188
  %"%hc_idx.load24" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp25 = icmp eq i64 %"%hc_idx.load24", 2
  br i1 %cmpeqtmp25, label %then_191, label %merge_192

merge_192:                                        ; preds = %then_191, %merge_190
  %"%hc_idx.load28" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp29 = icmp eq i64 %"%hc_idx.load28", 3
  br i1 %cmpeqtmp29, label %then_193, label %merge_194

merge_194:                                        ; preds = %then_193, %merge_192
  %loadtmp = load i64, ptr %"%td_addr", align 8
  ret i64 %loadtmp

then_177:                                         ; preds = %entry
  %"%612.load" = load i64, ptr @uhci_td_used_0, align 8
  store i64 %"%612.load", ptr %"%used", align 8
  br label %merge_178

then_179:                                         ; preds = %merge_178
  %"%616.load" = load i64, ptr @uhci_td_used_1, align 8
  store i64 %"%616.load", ptr %"%used", align 8
  br label %merge_180

then_181:                                         ; preds = %merge_180
  %"%620.load" = load i64, ptr @uhci_td_used_2, align 8
  store i64 %"%620.load", ptr %"%used", align 8
  br label %merge_182

then_183:                                         ; preds = %merge_182
  %"%624.load" = load i64, ptr @uhci_td_used_3, align 8
  store i64 %"%624.load", ptr %"%used", align 8
  br label %merge_184

then_185:                                         ; preds = %merge_184
  ret i64 0

then_187:                                         ; preds = %merge_186
  %"%used.load18" = load i64, ptr %"%used", align 8
  %addtmp19 = add i64 %"%used.load18", 1
  store i64 %addtmp19, ptr %"%uhci_td_used_0", align 8
  br label %merge_188

then_189:                                         ; preds = %merge_188
  %"%used.load22" = load i64, ptr %"%used", align 8
  %addtmp23 = add i64 %"%used.load22", 1
  store i64 %addtmp23, ptr %"%uhci_td_used_1", align 8
  br label %merge_190

then_191:                                         ; preds = %merge_190
  %"%used.load26" = load i64, ptr %"%used", align 8
  %addtmp27 = add i64 %"%used.load26", 1
  store i64 %addtmp27, ptr %"%uhci_td_used_2", align 8
  br label %merge_192

then_193:                                         ; preds = %merge_192
  %"%used.load30" = load i64, ptr %"%used", align 8
  %addtmp31 = add i64 %"%used.load30", 1
  store i64 %addtmp31, ptr %"%uhci_td_used_3", align 8
  br label %merge_194
}

define i64 @uhci_reset_td_pool(i64 %hc_idx) {
entry:
  %"%uhci_td_used_3" = alloca i64, align 8
  %"%uhci_td_used_2" = alloca i64, align 8
  %"%uhci_td_used_1" = alloca i64, align 8
  %"%uhci_td_used_0" = alloca i64, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%hc_idx.load", 0
  br i1 %cmpeqtmp, label %then_195, label %merge_196

merge_196:                                        ; preds = %then_195, %entry
  %"%hc_idx.load2" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%hc_idx.load2", 1
  br i1 %cmpeqtmp3, label %then_197, label %merge_198

merge_198:                                        ; preds = %then_197, %merge_196
  %"%hc_idx.load4" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp5 = icmp eq i64 %"%hc_idx.load4", 2
  br i1 %cmpeqtmp5, label %then_199, label %merge_200

merge_200:                                        ; preds = %then_199, %merge_198
  %"%hc_idx.load6" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp7 = icmp eq i64 %"%hc_idx.load6", 3
  br i1 %cmpeqtmp7, label %then_201, label %merge_202

merge_202:                                        ; preds = %then_201, %merge_200
  ret i64 0

then_195:                                         ; preds = %entry
  store i64 0, ptr %"%uhci_td_used_0", align 8
  br label %merge_196

then_197:                                         ; preds = %merge_196
  store i64 0, ptr %"%uhci_td_used_1", align 8
  br label %merge_198

then_199:                                         ; preds = %merge_198
  store i64 0, ptr %"%uhci_td_used_2", align 8
  br label %merge_200

then_201:                                         ; preds = %merge_200
  store i64 0, ptr %"%uhci_td_used_3", align 8
  br label %merge_202
}

define i64 @uhci_port_reset(i64 %hc_idx, i64 %port_num) {
entry:
  %"%uhci_port2_speed_3" = alloca i64, align 8
  %"%uhci_port2_speed_2" = alloca i64, align 8
  %"%uhci_port2_speed_1" = alloca i64, align 8
  %"%uhci_port2_speed_0" = alloca i64, align 8
  %"%uhci_port2_connected_3" = alloca i64, align 8
  %"%uhci_port2_connected_2" = alloca i64, align 8
  %"%uhci_port2_connected_1" = alloca i64, align 8
  %"%uhci_port2_connected_0" = alloca i64, align 8
  %"%uhci_port1_speed_3" = alloca i64, align 8
  %"%uhci_port1_speed_2" = alloca i64, align 8
  %"%uhci_port1_speed_1" = alloca i64, align 8
  %"%uhci_port1_speed_0" = alloca i64, align 8
  %"%uhci_port1_connected_3" = alloca i64, align 8
  %"%uhci_port1_connected_2" = alloca i64, align 8
  %"%uhci_port1_connected_1" = alloca i64, align 8
  %"%uhci_port1_connected_0" = alloca i64, align 8
  %"%speed" = alloca i64, align 8
  %"%low_speed" = alloca i64, align 8
  %"%enabled" = alloca i64, align 8
  %"%connected" = alloca i64, align 8
  %"%d3" = alloca i64, align 8
  %"%wait3" = alloca i64, align 8
  %"%d2" = alloca i64, align 8
  %"%wait2" = alloca i64, align 8
  %"%new_sc" = alloca i64, align 8
  %"%d" = alloca i64, align 8
  %"%wait" = alloca i64, align 8
  %"%sc" = alloca i64, align 8
  %"%uhci_port_resets" = alloca i64, align 8
  %"%port_reg" = alloca i64, align 8
  %port_num2 = alloca i64, align 8
  store i64 %port_num, ptr %port_num2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %calltmp = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %"%683.load" = load i64, ptr %hc_idx1, align 8
  %calltmp3 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%683.load")
  %calltmp4 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%688.load" = load i64, ptr %port_num2, align 8
  %calltmp5 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%688.load")
  %calltmp6 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%692.load" = load i64, ptr @UHCI_PORTSC1, align 8
  store i64 %"%692.load", ptr %"%port_reg", align 8
  %"%port_num.load" = load i64, ptr %port_num2, align 8
  %cmpeqtmp = icmp eq i64 %"%port_num.load", 1
  br i1 %cmpeqtmp, label %then_203, label %merge_204

else_220:                                         ; preds = %merge_217
  %"%hc_idx.load54" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp55 = icmp eq i64 %"%hc_idx.load54", 0
  br i1 %cmpeqtmp55, label %then_237, label %merge_238

merge_204:                                        ; preds = %then_203, %entry
  %"%uhci_port_resets.load" = load i64, ptr @uhci_port_resets, align 8
  %addtmp = add i64 %"%uhci_port_resets.load", 1
  store i64 %addtmp, ptr %"%uhci_port_resets", align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %"%port_reg.load" = load i64, ptr %"%port_reg", align 8
  %calltmp7 = call i64 @uhci_read16(i64 %"%hc_idx.load", i64 %"%port_reg.load")
  store i64 %calltmp7, ptr %"%sc", align 8
  %"%sc.load" = load i64, ptr %"%sc", align 8
  %"%UHCI_PORT_RESET.load" = load i64, ptr @UHCI_PORT_RESET, align 8
  %addtmp8 = add i64 %"%sc.load", %"%UHCI_PORT_RESET.load"
  %"%hc_idx.load9" = load i64, ptr %hc_idx1, align 8
  %"%port_reg.load10" = load i64, ptr %"%port_reg", align 8
  %calltmp11 = call i64 @uhci_write16(i64 %"%hc_idx.load9", i64 %"%port_reg.load10", i64 %addtmp8)
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%wait", align 8
  br label %while_cond_205

merge_215:                                        ; preds = %merge_219, %while_exit_213
  %loadtmp = load i64, ptr %"%connected", align 8
  ret i64 %loadtmp

merge_217:                                        ; preds = %then_216, %then_214
  %"%port_num.load50" = load i64, ptr %port_num2, align 8
  %cmpeqtmp51 = icmp eq i64 %"%port_num.load50", 0
  br i1 %cmpeqtmp51, label %then_218, label %else_220

merge_219:                                        ; preds = %merge_252, %merge_236
  br label %merge_215

merge_222:                                        ; preds = %then_221, %then_218
  %"%hc_idx.load56" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp57 = icmp eq i64 %"%hc_idx.load56", 1
  br i1 %cmpeqtmp57, label %then_223, label %merge_224

merge_224:                                        ; preds = %then_223, %merge_222
  %"%hc_idx.load58" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp59 = icmp eq i64 %"%hc_idx.load58", 2
  br i1 %cmpeqtmp59, label %then_225, label %merge_226

merge_226:                                        ; preds = %then_225, %merge_224
  %"%hc_idx.load60" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp61 = icmp eq i64 %"%hc_idx.load60", 3
  br i1 %cmpeqtmp61, label %then_227, label %merge_228

merge_228:                                        ; preds = %then_227, %merge_226
  %"%hc_idx.load62" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp63 = icmp eq i64 %"%hc_idx.load62", 0
  br i1 %cmpeqtmp63, label %then_229, label %merge_230

merge_230:                                        ; preds = %then_229, %merge_228
  %"%hc_idx.load64" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp65 = icmp eq i64 %"%hc_idx.load64", 1
  br i1 %cmpeqtmp65, label %then_231, label %merge_232

merge_232:                                        ; preds = %then_231, %merge_230
  %"%hc_idx.load66" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp67 = icmp eq i64 %"%hc_idx.load66", 2
  br i1 %cmpeqtmp67, label %then_233, label %merge_234

merge_234:                                        ; preds = %then_233, %merge_232
  %"%hc_idx.load68" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp69 = icmp eq i64 %"%hc_idx.load68", 3
  br i1 %cmpeqtmp69, label %then_235, label %merge_236

merge_236:                                        ; preds = %then_235, %merge_234
  br label %merge_219

merge_238:                                        ; preds = %then_237, %else_220
  %"%hc_idx.load70" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp71 = icmp eq i64 %"%hc_idx.load70", 1
  br i1 %cmpeqtmp71, label %then_239, label %merge_240

merge_240:                                        ; preds = %then_239, %merge_238
  %"%hc_idx.load72" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp73 = icmp eq i64 %"%hc_idx.load72", 2
  br i1 %cmpeqtmp73, label %then_241, label %merge_242

merge_242:                                        ; preds = %then_241, %merge_240
  %"%hc_idx.load74" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp75 = icmp eq i64 %"%hc_idx.load74", 3
  br i1 %cmpeqtmp75, label %then_243, label %merge_244

merge_244:                                        ; preds = %then_243, %merge_242
  %"%hc_idx.load76" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp77 = icmp eq i64 %"%hc_idx.load76", 0
  br i1 %cmpeqtmp77, label %then_245, label %merge_246

merge_246:                                        ; preds = %then_245, %merge_244
  %"%hc_idx.load78" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp79 = icmp eq i64 %"%hc_idx.load78", 1
  br i1 %cmpeqtmp79, label %then_247, label %merge_248

merge_248:                                        ; preds = %then_247, %merge_246
  %"%hc_idx.load80" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp81 = icmp eq i64 %"%hc_idx.load80", 2
  br i1 %cmpeqtmp81, label %then_249, label %merge_250

merge_250:                                        ; preds = %then_249, %merge_248
  %"%hc_idx.load82" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp83 = icmp eq i64 %"%hc_idx.load82", 3
  br i1 %cmpeqtmp83, label %then_251, label %merge_252

merge_252:                                        ; preds = %then_251, %merge_250
  br label %merge_219

then_203:                                         ; preds = %entry
  %"%696.load" = load i64, ptr @UHCI_PORTSC2, align 8
  store i64 %"%696.load", ptr %"%port_reg", align 8
  br label %merge_204

then_214:                                         ; preds = %while_exit_213
  store i64 1, ptr %"%speed", align 8
  %"%low_speed.load" = load i64, ptr %"%low_speed", align 8
  %cmpeqtmp49 = icmp eq i64 %"%low_speed.load", 1
  br i1 %cmpeqtmp49, label %then_216, label %merge_217

then_216:                                         ; preds = %then_214
  store i64 0, ptr %"%speed", align 8
  br label %merge_217

then_218:                                         ; preds = %merge_217
  %"%hc_idx.load52" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp53 = icmp eq i64 %"%hc_idx.load52", 0
  br i1 %cmpeqtmp53, label %then_221, label %merge_222

then_221:                                         ; preds = %then_218
  store i64 1, ptr %"%uhci_port1_connected_0", align 8
  br label %merge_222

then_223:                                         ; preds = %merge_222
  store i64 1, ptr %"%uhci_port1_connected_1", align 8
  br label %merge_224

then_225:                                         ; preds = %merge_224
  store i64 1, ptr %"%uhci_port1_connected_2", align 8
  br label %merge_226

then_227:                                         ; preds = %merge_226
  store i64 1, ptr %"%uhci_port1_connected_3", align 8
  br label %merge_228

then_229:                                         ; preds = %merge_228
  %"%793.load" = load i64, ptr %"%speed", align 8
  store i64 %"%793.load", ptr %"%uhci_port1_speed_0", align 8
  br label %merge_230

then_231:                                         ; preds = %merge_230
  %"%797.load" = load i64, ptr %"%speed", align 8
  store i64 %"%797.load", ptr %"%uhci_port1_speed_1", align 8
  br label %merge_232

then_233:                                         ; preds = %merge_232
  %"%801.load" = load i64, ptr %"%speed", align 8
  store i64 %"%801.load", ptr %"%uhci_port1_speed_2", align 8
  br label %merge_234

then_235:                                         ; preds = %merge_234
  %"%805.load" = load i64, ptr %"%speed", align 8
  store i64 %"%805.load", ptr %"%uhci_port1_speed_3", align 8
  br label %merge_236

then_237:                                         ; preds = %else_220
  store i64 1, ptr %"%uhci_port2_connected_0", align 8
  br label %merge_238

then_239:                                         ; preds = %merge_238
  store i64 1, ptr %"%uhci_port2_connected_1", align 8
  br label %merge_240

then_241:                                         ; preds = %merge_240
  store i64 1, ptr %"%uhci_port2_connected_2", align 8
  br label %merge_242

then_243:                                         ; preds = %merge_242
  store i64 1, ptr %"%uhci_port2_connected_3", align 8
  br label %merge_244

then_245:                                         ; preds = %merge_244
  %"%821.load" = load i64, ptr %"%speed", align 8
  store i64 %"%821.load", ptr %"%uhci_port2_speed_0", align 8
  br label %merge_246

then_247:                                         ; preds = %merge_246
  %"%825.load" = load i64, ptr %"%speed", align 8
  store i64 %"%825.load", ptr %"%uhci_port2_speed_1", align 8
  br label %merge_248

then_249:                                         ; preds = %merge_248
  %"%829.load" = load i64, ptr %"%speed", align 8
  store i64 %"%829.load", ptr %"%uhci_port2_speed_2", align 8
  br label %merge_250

then_251:                                         ; preds = %merge_250
  %"%833.load" = load i64, ptr %"%speed", align 8
  store i64 %"%833.load", ptr %"%uhci_port2_speed_3", align 8
  br label %merge_252

while_body_206:                                   ; preds = %while_cond_205
  store i64 0, ptr %"%d", align 8
  br label %while_cond_205

while_body_209:                                   ; preds = %while_cond_208
  store i64 0, ptr %"%d2", align 8
  br label %while_cond_208

while_body_212:                                   ; preds = %while_cond_211
  store i64 0, ptr %"%d3", align 8
  br label %while_cond_211

while_cond_205:                                   ; preds = %while_body_206, %merge_204
  call void @sad_ll_timer_get_ticks()
  %"%wait.load" = load i64, ptr %"%wait", align 8
  %subtmp = sub i64 0, %"%wait.load"
  %cmplttmp = icmp slt i64 %subtmp, 5000
  br i1 %cmplttmp, label %while_body_206, label %while_exit_207

while_cond_208:                                   ; preds = %while_body_209, %while_exit_207
  call void @sad_ll_timer_get_ticks()
  %"%wait2.load" = load i64, ptr %"%wait2", align 8
  %subtmp20 = sub i64 0, %"%wait2.load"
  %cmplttmp21 = icmp slt i64 %subtmp20, 1000
  br i1 %cmplttmp21, label %while_body_209, label %while_exit_210

while_cond_211:                                   ; preds = %while_body_212, %while_exit_210
  call void @sad_ll_timer_get_ticks()
  %"%wait3.load" = load i64, ptr %"%wait3", align 8
  %subtmp30 = sub i64 0, %"%wait3.load"
  %cmplttmp31 = icmp slt i64 %subtmp30, 500
  br i1 %cmplttmp31, label %while_body_212, label %while_exit_213

while_exit_207:                                   ; preds = %while_cond_205
  %"%hc_idx.load12" = load i64, ptr %hc_idx1, align 8
  %"%port_reg.load13" = load i64, ptr %"%port_reg", align 8
  %calltmp14 = call i64 @uhci_read16(i64 %"%hc_idx.load12", i64 %"%port_reg.load13")
  store i64 %calltmp14, ptr %"%sc", align 8
  %"%sc.load15" = load i64, ptr %"%sc", align 8
  %"%UHCI_PORT_RESET.load16" = load i64, ptr @UHCI_PORT_RESET, align 8
  %modtmp = srem i64 %"%sc.load15", %"%UHCI_PORT_RESET.load16"
  store i64 %modtmp, ptr %"%new_sc", align 8
  %"%hc_idx.load17" = load i64, ptr %hc_idx1, align 8
  %"%port_reg.load18" = load i64, ptr %"%port_reg", align 8
  %"%new_sc.load" = load i64, ptr %"%new_sc", align 8
  %calltmp19 = call i64 @uhci_write16(i64 %"%hc_idx.load17", i64 %"%port_reg.load18", i64 %"%new_sc.load")
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%wait2", align 8
  br label %while_cond_208

while_exit_210:                                   ; preds = %while_cond_208
  %"%hc_idx.load22" = load i64, ptr %hc_idx1, align 8
  %"%port_reg.load23" = load i64, ptr %"%port_reg", align 8
  %calltmp24 = call i64 @uhci_read16(i64 %"%hc_idx.load22", i64 %"%port_reg.load23")
  store i64 %calltmp24, ptr %"%sc", align 8
  %"%UHCI_PORT_ENABLE.load" = load i64, ptr @UHCI_PORT_ENABLE, align 8
  %"%UHCI_PORT_CSC.load" = load i64, ptr @UHCI_PORT_CSC, align 8
  %addtmp25 = add i64 %"%UHCI_PORT_ENABLE.load", %"%UHCI_PORT_CSC.load"
  %"%UHCI_PORT_ENC.load" = load i64, ptr @UHCI_PORT_ENC, align 8
  %addtmp26 = add i64 %addtmp25, %"%UHCI_PORT_ENC.load"
  %"%hc_idx.load27" = load i64, ptr %hc_idx1, align 8
  %"%port_reg.load28" = load i64, ptr %"%port_reg", align 8
  %calltmp29 = call i64 @uhci_write16(i64 %"%hc_idx.load27", i64 %"%port_reg.load28", i64 %addtmp26)
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%wait3", align 8
  br label %while_cond_211

while_exit_213:                                   ; preds = %while_cond_211
  %"%hc_idx.load32" = load i64, ptr %hc_idx1, align 8
  %"%port_reg.load33" = load i64, ptr %"%port_reg", align 8
  %calltmp34 = call i64 @uhci_read16(i64 %"%hc_idx.load32", i64 %"%port_reg.load33")
  store i64 %calltmp34, ptr %"%sc", align 8
  %"%sc.load35" = load i64, ptr %"%sc", align 8
  %andtmp = and i64 %"%sc.load35", 1
  store i64 %andtmp, ptr %"%connected", align 8
  %"%sc.load36" = load i64, ptr %"%sc", align 8
  %shrtmp = lshr i64 %"%sc.load36", 2
  %andtmp37 = and i64 %shrtmp, 1
  store i64 %andtmp37, ptr %"%enabled", align 8
  %"%sc.load38" = load i64, ptr %"%sc", align 8
  %shrtmp39 = lshr i64 %"%sc.load38", 8
  %andtmp40 = and i64 %shrtmp39, 1
  store i64 %andtmp40, ptr %"%low_speed", align 8
  %calltmp41 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %"%755.load" = load i64, ptr %"%connected", align 8
  %calltmp42 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%755.load")
  %calltmp43 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  %"%760.load" = load i64, ptr %"%enabled", align 8
  %calltmp44 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%760.load")
  %calltmp45 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %"%765.load" = load i64, ptr %"%low_speed", align 8
  %calltmp46 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%765.load")
  %calltmp47 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%connected.load" = load i64, ptr %"%connected", align 8
  %cmpeqtmp48 = icmp eq i64 %"%connected.load", 1
  br i1 %cmpeqtmp48, label %then_214, label %merge_215
}

define i64 @uhci_control_transfer(i64 %hc_idx, i64 %dev_addr, i64 %setup_buf, i64 %data_buf, i64 %data_len, i64 %is_read) {
entry:
  %"%check_count" = alloca i64, align 8
  %"%check_td" = alloca i64, align 8
  %"%total_actual" = alloca i64, align 8
  %"%active" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  %"%framelist" = alloca i64, align 8
  %"%status_pid" = alloca i64, align 8
  %"%pkt_len" = alloca i64, align 8
  %"%td_status" = alloca i64, align 8
  %"%td_data" = alloca i64, align 8
  %"%pid" = alloca i64, align 8
  %"%buf_offset" = alloca i64, align 8
  %"%remaining" = alloca i64, align 8
  %"%toggle" = alloca i64, align 8
  %"%td_data_last" = alloca i64, align 8
  %"%td_data_first" = alloca i64, align 8
  %"%td_setup" = alloca i64, align 8
  %"%uhci_transfer_errors" = alloca i64, align 8
  %"%qh" = alloca i64, align 8
  %"%ls_dev" = alloca i64, align 8
  %"%uhci_total_transfers" = alloca i64, align 8
  %is_read6 = alloca i64, align 8
  store i64 %is_read, ptr %is_read6, align 8
  %data_len5 = alloca i64, align 8
  store i64 %data_len, ptr %data_len5, align 8
  %data_buf4 = alloca i64, align 8
  store i64 %data_buf, ptr %data_buf4, align 8
  %setup_buf3 = alloca i64, align 8
  store i64 %setup_buf, ptr %setup_buf3, align 8
  %dev_addr2 = alloca i64, align 8
  store i64 %dev_addr, ptr %dev_addr2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%uhci_total_transfers.load" = load i64, ptr @uhci_total_transfers, align 8
  %addtmp = add i64 %"%uhci_total_transfers.load", 1
  store i64 %addtmp, ptr %"%uhci_total_transfers", align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @uhci_reset_td_pool(i64 %"%hc_idx.load")
  store i64 0, ptr %"%ls_dev", align 8
  %"%hc_idx.load7" = load i64, ptr %hc_idx1, align 8
  %calltmp8 = call i64 @uhci_get_qh_pool(i64 %"%hc_idx.load7")
  store i64 %calltmp8, ptr %"%qh", align 8
  %"%qh.load" = load i64, ptr %"%qh", align 8
  %cmpeqtmp = icmp eq i64 %"%qh.load", 0
  br i1 %cmpeqtmp, label %then_253, label %merge_254

else_276:                                         ; preds = %merge_273
  %"%td_setup.load74" = load i64, ptr %"%td_setup", align 8
  %"%td_status.load75" = load i64, ptr %"%td_status", align 8
  %calltmp76 = call i64 @uhci_td_set_link(i64 %"%td_setup.load74", i64 %"%td_status.load75", i64 0, i64 0)
  br label %merge_275

else_295:                                         ; preds = %merge_292
  %"%check_td.load137" = load i64, ptr %"%check_td", align 8
  %addtmp138 = add i64 %"%check_td.load137", 32
  store i64 %addtmp138, ptr %"%check_td", align 8
  %"%check_count.load139" = load i64, ptr %"%check_count", align 8
  %addtmp140 = add i64 %"%check_count.load139", 1
  store i64 %addtmp140, ptr %"%check_count", align 8
  br label %merge_294

merge_254:                                        ; preds = %entry
  %"%hc_idx.load10" = load i64, ptr %hc_idx1, align 8
  %calltmp11 = call i64 @uhci_alloc_td(i64 %"%hc_idx.load10")
  store i64 %calltmp11, ptr %"%td_setup", align 8
  %"%td_setup.load" = load i64, ptr %"%td_setup", align 8
  %cmpeqtmp12 = icmp eq i64 %"%td_setup.load", 0
  br i1 %cmpeqtmp12, label %then_255, label %merge_256

merge_256:                                        ; preds = %merge_254
  store i64 0, ptr %"%td_data_first", align 8
  store i64 0, ptr %"%td_data_last", align 8
  store i64 1, ptr %"%toggle", align 8
  %"%875.load" = load i64, ptr %data_len5, align 8
  store i64 %"%875.load", ptr %"%remaining", align 8
  store i64 0, ptr %"%buf_offset", align 8
  %"%880.load" = load i64, ptr @UHCI_PID_IN, align 8
  store i64 %"%880.load", ptr %"%pid", align 8
  %"%is_read.load" = load i64, ptr %is_read6, align 8
  %cmpeqtmp15 = icmp eq i64 %"%is_read.load", 0
  br i1 %cmpeqtmp15, label %then_257, label %merge_258

merge_258:                                        ; preds = %then_257, %merge_256
  br label %while_cond_259

merge_263:                                        ; preds = %while_body_260
  %"%td_data_first.load" = load i64, ptr %"%td_data_first", align 8
  %cmpeqtmp24 = icmp eq i64 %"%td_data_first.load", 0
  br i1 %cmpeqtmp24, label %then_264, label %merge_265

merge_265:                                        ; preds = %then_264, %merge_263
  %"%td_data_last.load" = load i64, ptr %"%td_data_last", align 8
  %cmpnetmp = icmp ne i64 %"%td_data_last.load", 0
  br i1 %cmpnetmp, label %then_266, label %merge_267

merge_267:                                        ; preds = %then_266, %merge_265
  %"%908.load" = load i64, ptr %"%remaining", align 8
  store i64 %"%908.load", ptr %"%pkt_len", align 8
  %"%pkt_len.load" = load i64, ptr %"%pkt_len", align 8
  %cmpgttmp28 = icmp sgt i64 %"%pkt_len.load", 8
  br i1 %cmpgttmp28, label %then_268, label %merge_269

merge_269:                                        ; preds = %then_268, %merge_267
  %"%td_data.load29" = load i64, ptr %"%td_data", align 8
  %"%ls_dev.load" = load i64, ptr %"%ls_dev", align 8
  %calltmp30 = call i64 @uhci_td_set_status(i64 %"%td_data.load29", i64 1, i64 0, i64 %"%ls_dev.load", i64 3)
  %"%td_data.load31" = load i64, ptr %"%td_data", align 8
  %"%pid.load" = load i64, ptr %"%pid", align 8
  %"%dev_addr.load" = load i64, ptr %dev_addr2, align 8
  %"%toggle.load" = load i64, ptr %"%toggle", align 8
  %"%pkt_len.load32" = load i64, ptr %"%pkt_len", align 8
  %calltmp33 = call i64 @uhci_td_set_token(i64 %"%td_data.load31", i64 %"%pid.load", i64 %"%dev_addr.load", i64 0, i64 %"%toggle.load", i64 %"%pkt_len.load32")
  %"%data_buf.load" = load i64, ptr %data_buf4, align 8
  %"%buf_offset.load" = load i64, ptr %"%buf_offset", align 8
  %addtmp34 = add i64 %"%data_buf.load", %"%buf_offset.load"
  %"%td_data.load35" = load i64, ptr %"%td_data", align 8
  %calltmp36 = call i64 @uhci_td_set_buffer(i64 %"%td_data.load35", i64 %addtmp34)
  %"%921.load" = load i64, ptr %"%td_data", align 8
  store i64 %"%921.load", ptr %"%td_data_last", align 8
  %"%buf_offset.load37" = load i64, ptr %"%buf_offset", align 8
  %"%pkt_len.load38" = load i64, ptr %"%pkt_len", align 8
  %addtmp39 = add i64 %"%buf_offset.load37", %"%pkt_len.load38"
  store i64 %addtmp39, ptr %"%buf_offset", align 8
  %"%remaining.load40" = load i64, ptr %"%remaining", align 8
  %"%pkt_len.load41" = load i64, ptr %"%pkt_len", align 8
  %subtmp = sub i64 %"%remaining.load40", %"%pkt_len.load41"
  store i64 %subtmp, ptr %"%remaining", align 8
  %"%toggle.load42" = load i64, ptr %"%toggle", align 8
  %subtmp43 = sub i64 1, %"%toggle.load42"
  store i64 %subtmp43, ptr %"%toggle", align 8
  br label %while_cond_259

merge_271:                                        ; preds = %while_exit_261
  %"%938.load" = load i64, ptr @UHCI_PID_OUT, align 8
  store i64 %"%938.load", ptr %"%status_pid", align 8
  %"%is_read.load46" = load i64, ptr %is_read6, align 8
  %cmpeqtmp47 = icmp eq i64 %"%is_read.load46", 0
  br i1 %cmpeqtmp47, label %then_272, label %merge_273

merge_273:                                        ; preds = %then_272, %merge_271
  %"%td_status.load48" = load i64, ptr %"%td_status", align 8
  %"%ls_dev.load49" = load i64, ptr %"%ls_dev", align 8
  %calltmp50 = call i64 @uhci_td_set_status(i64 %"%td_status.load48", i64 1, i64 1, i64 %"%ls_dev.load49", i64 3)
  %"%td_status.load51" = load i64, ptr %"%td_status", align 8
  %"%status_pid.load" = load i64, ptr %"%status_pid", align 8
  %"%dev_addr.load52" = load i64, ptr %dev_addr2, align 8
  %calltmp53 = call i64 @uhci_td_set_token(i64 %"%td_status.load51", i64 %"%status_pid.load", i64 %"%dev_addr.load52", i64 0, i64 1, i64 0)
  %"%td_status.load54" = load i64, ptr %"%td_status", align 8
  %calltmp55 = call i64 @uhci_td_set_buffer(i64 %"%td_status.load54", i64 0)
  %"%td_status.load56" = load i64, ptr %"%td_status", align 8
  %calltmp57 = call i64 @uhci_td_set_link(i64 %"%td_status.load56", i64 0, i64 1, i64 0)
  %"%td_setup.load58" = load i64, ptr %"%td_setup", align 8
  %"%ls_dev.load59" = load i64, ptr %"%ls_dev", align 8
  %calltmp60 = call i64 @uhci_td_set_status(i64 %"%td_setup.load58", i64 1, i64 0, i64 %"%ls_dev.load59", i64 3)
  %"%td_setup.load61" = load i64, ptr %"%td_setup", align 8
  %"%UHCI_PID_SETUP.load" = load i64, ptr @UHCI_PID_SETUP, align 8
  %"%dev_addr.load62" = load i64, ptr %dev_addr2, align 8
  %calltmp63 = call i64 @uhci_td_set_token(i64 %"%td_setup.load61", i64 %"%UHCI_PID_SETUP.load", i64 %"%dev_addr.load62", i64 0, i64 0, i64 8)
  %"%td_setup.load64" = load i64, ptr %"%td_setup", align 8
  %"%setup_buf.load" = load i64, ptr %setup_buf3, align 8
  %calltmp65 = call i64 @uhci_td_set_buffer(i64 %"%td_setup.load64", i64 %"%setup_buf.load")
  %"%td_data_first.load66" = load i64, ptr %"%td_data_first", align 8
  %cmpnetmp67 = icmp ne i64 %"%td_data_first.load66", 0
  br i1 %cmpnetmp67, label %then_274, label %else_276

merge_275:                                        ; preds = %else_276, %then_274
  %"%qh.load77" = load i64, ptr %"%qh", align 8
  %calltmp78 = call i64 @uhci_qh_set_head_link(i64 %"%qh.load77", i64 0, i64 1)
  %"%qh.load79" = load i64, ptr %"%qh", align 8
  %"%td_setup.load80" = load i64, ptr %"%td_setup", align 8
  %calltmp81 = call i64 @uhci_qh_set_element(i64 %"%qh.load79", i64 %"%td_setup.load80", i64 0)
  %"%hc_idx.load82" = load i64, ptr %hc_idx1, align 8
  %calltmp83 = call i64 @uhci_get_framelist(i64 %"%hc_idx.load82")
  store i64 %calltmp83, ptr %"%framelist", align 8
  %"%qh.load84" = load i64, ptr %"%qh", align 8
  %addtmp85 = add i64 %"%qh.load84", 2
  %"%framelist.load" = load i64, ptr %"%framelist", align 8
  call void @sad_ll_mem_write32(i64 %"%framelist.load", i64 %addtmp85)
  %"%hc_idx.load86" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_FRNUM.load" = load i64, ptr @UHCI_FRNUM, align 8
  %calltmp87 = call i64 @uhci_write16(i64 %"%hc_idx.load86", i64 %"%UHCI_FRNUM.load", i64 0)
  %"%UHCI_CMD_RS.load" = load i64, ptr @UHCI_CMD_RS, align 8
  %"%UHCI_CMD_MAXP.load" = load i64, ptr @UHCI_CMD_MAXP, align 8
  %addtmp88 = add i64 %"%UHCI_CMD_RS.load", %"%UHCI_CMD_MAXP.load"
  %"%hc_idx.load89" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_USBCMD.load" = load i64, ptr @UHCI_USBCMD, align 8
  %calltmp90 = call i64 @uhci_write16(i64 %"%hc_idx.load89", i64 %"%UHCI_USBCMD.load", i64 %addtmp88)
  store i64 10000, ptr %"%timeout", align 8
  br label %while_cond_277

merge_281:                                        ; preds = %while_body_278
  %"%timeout.load109" = load i64, ptr %"%timeout", align 8
  %subtmp110 = sub i64 %"%timeout.load109", 1
  store i64 %subtmp110, ptr %"%timeout", align 8
  br label %while_cond_277

merge_283:                                        ; preds = %then_280
  %"%td_status.load115" = load i64, ptr %"%td_status", align 8
  %calltmp116 = call i64 @uhci_td_has_error(i64 %"%td_status.load115")
  %cmpeqtmp117 = icmp eq i64 %calltmp116, 1
  br i1 %cmpeqtmp117, label %then_284, label %merge_285

merge_285:                                        ; preds = %merge_283
  store i64 0, ptr %"%total_actual", align 8
  %"%td_data_first.load122" = load i64, ptr %"%td_data_first", align 8
  %cmpnetmp123 = icmp ne i64 %"%td_data_first.load122", 0
  br i1 %cmpnetmp123, label %then_286, label %merge_287

merge_287:                                        ; preds = %while_exit_290, %merge_285
  %"%framelist.load124" = load i64, ptr %"%framelist", align 8
  call void @sad_ll_mem_write32(i64 %"%framelist.load124", i64 1)
  %loadtmp = load i64, ptr %"%total_actual", align 8
  ret i64 %loadtmp

merge_292:                                        ; preds = %while_body_289
  %"%check_td.load131" = load i64, ptr %"%check_td", align 8
  %calltmp132 = call i64 @uhci_td_actual_len(i64 %"%check_td.load131")
  %"%total_actual.load" = load i64, ptr %"%total_actual", align 8
  %addtmp133 = add i64 %"%total_actual.load", %calltmp132
  store i64 %addtmp133, ptr %"%total_actual", align 8
  %"%check_td.load134" = load i64, ptr %"%check_td", align 8
  %"%td_data_last.load135" = load i64, ptr %"%td_data_last", align 8
  %cmpeqtmp136 = icmp eq i64 %"%check_td.load134", %"%td_data_last.load135"
  br i1 %cmpeqtmp136, label %then_293, label %else_295

merge_294:                                        ; preds = %else_295, %then_293
  br label %while_cond_288

then_253:                                         ; preds = %entry
  %"%uhci_transfer_errors.load" = load i64, ptr @uhci_transfer_errors, align 8
  %addtmp9 = add i64 %"%uhci_transfer_errors.load", 1
  store i64 %addtmp9, ptr %"%uhci_transfer_errors", align 8
  ret i64 -1

then_255:                                         ; preds = %merge_254
  %"%uhci_transfer_errors.load13" = load i64, ptr %"%uhci_transfer_errors", align 8
  %addtmp14 = add i64 %"%uhci_transfer_errors.load13", 1
  store i64 %addtmp14, ptr %"%uhci_transfer_errors", align 8
  ret i64 -1

then_257:                                         ; preds = %merge_256
  %"%884.load" = load i64, ptr @UHCI_PID_OUT, align 8
  store i64 %"%884.load", ptr %"%pid", align 8
  br label %merge_258

then_262:                                         ; preds = %while_body_260
  %"%uhci_transfer_errors.load22" = load i64, ptr %"%uhci_transfer_errors", align 8
  %addtmp23 = add i64 %"%uhci_transfer_errors.load22", 1
  store i64 %addtmp23, ptr %"%uhci_transfer_errors", align 8
  ret i64 -1

then_264:                                         ; preds = %merge_263
  %"%901.load" = load i64, ptr %"%td_data", align 8
  store i64 %"%901.load", ptr %"%td_data_first", align 8
  br label %merge_265

then_266:                                         ; preds = %merge_265
  %"%td_data_last.load25" = load i64, ptr %"%td_data_last", align 8
  %"%td_data.load26" = load i64, ptr %"%td_data", align 8
  %calltmp27 = call i64 @uhci_td_set_link(i64 %"%td_data_last.load25", i64 %"%td_data.load26", i64 0, i64 0)
  br label %merge_267

then_268:                                         ; preds = %merge_267
  store i64 8, ptr %"%pkt_len", align 8
  br label %merge_269

then_270:                                         ; preds = %while_exit_261
  %"%uhci_transfer_errors.load44" = load i64, ptr %"%uhci_transfer_errors", align 8
  %addtmp45 = add i64 %"%uhci_transfer_errors.load44", 1
  store i64 %addtmp45, ptr %"%uhci_transfer_errors", align 8
  ret i64 -1

then_272:                                         ; preds = %merge_271
  %"%942.load" = load i64, ptr @UHCI_PID_IN, align 8
  store i64 %"%942.load", ptr %"%status_pid", align 8
  br label %merge_273

then_274:                                         ; preds = %merge_273
  %"%td_setup.load68" = load i64, ptr %"%td_setup", align 8
  %"%td_data_first.load69" = load i64, ptr %"%td_data_first", align 8
  %calltmp70 = call i64 @uhci_td_set_link(i64 %"%td_setup.load68", i64 %"%td_data_first.load69", i64 0, i64 0)
  %"%td_data_last.load71" = load i64, ptr %"%td_data_last", align 8
  %"%td_status.load72" = load i64, ptr %"%td_status", align 8
  %calltmp73 = call i64 @uhci_td_set_link(i64 %"%td_data_last.load71", i64 %"%td_status.load72", i64 0, i64 0)
  br label %merge_275

then_280:                                         ; preds = %while_body_278
  %"%hc_idx.load103" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_USBCMD.load104" = load i64, ptr @UHCI_USBCMD, align 8
  %calltmp105 = call i64 @uhci_write16(i64 %"%hc_idx.load103", i64 %"%UHCI_USBCMD.load104", i64 0)
  %"%td_setup.load106" = load i64, ptr %"%td_setup", align 8
  %calltmp107 = call i64 @uhci_td_has_error(i64 %"%td_setup.load106")
  %cmpeqtmp108 = icmp eq i64 %calltmp107, 1
  br i1 %cmpeqtmp108, label %then_282, label %merge_283

then_282:                                         ; preds = %then_280
  %calltmp111 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  %calltmp112 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%uhci_transfer_errors.load113" = load i64, ptr %"%uhci_transfer_errors", align 8
  %addtmp114 = add i64 %"%uhci_transfer_errors.load113", 1
  store i64 %addtmp114, ptr %"%uhci_transfer_errors", align 8
  ret i64 -2

then_284:                                         ; preds = %merge_283
  %calltmp118 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %calltmp119 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%uhci_transfer_errors.load120" = load i64, ptr %"%uhci_transfer_errors", align 8
  %addtmp121 = add i64 %"%uhci_transfer_errors.load120", 1
  store i64 %addtmp121, ptr %"%uhci_transfer_errors", align 8
  ret i64 -3

then_286:                                         ; preds = %merge_285
  %"%1034.load" = load i64, ptr %"%td_data_first", align 8
  store i64 %"%1034.load", ptr %"%check_td", align 8
  store i64 0, ptr %"%check_count", align 8
  br label %while_cond_288

then_291:                                         ; preds = %while_body_289
  %calltmp127 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %calltmp128 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%uhci_transfer_errors.load129" = load i64, ptr %"%uhci_transfer_errors", align 8
  %addtmp130 = add i64 %"%uhci_transfer_errors.load129", 1
  store i64 %addtmp130, ptr %"%uhci_transfer_errors", align 8
  ret i64 -4

then_293:                                         ; preds = %merge_292
  store i64 16, ptr %"%check_count", align 8
  br label %merge_294

while_body_260:                                   ; preds = %while_cond_259
  %"%hc_idx.load16" = load i64, ptr %hc_idx1, align 8
  %calltmp17 = call i64 @uhci_alloc_td(i64 %"%hc_idx.load16")
  store i64 %calltmp17, ptr %"%td_data", align 8
  %"%td_data.load" = load i64, ptr %"%td_data", align 8
  %cmpeqtmp18 = icmp eq i64 %"%td_data.load", 0
  br i1 %cmpeqtmp18, label %then_262, label %merge_263

while_body_278:                                   ; preds = %while_cond_277
  %"%td_status.load92" = load i64, ptr %"%td_status", align 8
  %calltmp93 = call i64 @uhci_td_is_active(i64 %"%td_status.load92")
  store i64 %calltmp93, ptr %"%active", align 8
  %"%active.load" = load i64, ptr %"%active", align 8
  %cmpeqtmp94 = icmp eq i64 %"%active.load", 0
  br i1 %cmpeqtmp94, label %then_280, label %merge_281

while_body_289:                                   ; preds = %while_cond_288
  %"%check_td.load" = load i64, ptr %"%check_td", align 8
  %calltmp125 = call i64 @uhci_td_has_error(i64 %"%check_td.load")
  %cmpeqtmp126 = icmp eq i64 %calltmp125, 1
  br i1 %cmpeqtmp126, label %then_291, label %merge_292

while_cond_259:                                   ; preds = %merge_269, %merge_258
  %"%remaining.load" = load i64, ptr %"%remaining", align 8
  %cmpgttmp = icmp sgt i64 %"%remaining.load", 0
  br i1 %cmpgttmp, label %while_body_260, label %while_exit_261

while_cond_277:                                   ; preds = %merge_281, %merge_275
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp91 = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp91, label %while_body_278, label %while_exit_279

while_cond_288:                                   ; preds = %merge_294, %then_286
  %"%check_count.load" = load i64, ptr %"%check_count", align 8
  %cmplttmp = icmp slt i64 %"%check_count.load", 16
  br i1 %cmplttmp, label %while_body_289, label %while_exit_290

while_exit_261:                                   ; preds = %while_cond_259
  %"%hc_idx.load19" = load i64, ptr %hc_idx1, align 8
  %calltmp20 = call i64 @uhci_alloc_td(i64 %"%hc_idx.load19")
  store i64 %calltmp20, ptr %"%td_status", align 8
  %"%td_status.load" = load i64, ptr %"%td_status", align 8
  %cmpeqtmp21 = icmp eq i64 %"%td_status.load", 0
  br i1 %cmpeqtmp21, label %then_270, label %merge_271

while_exit_279:                                   ; preds = %while_cond_277
  %"%hc_idx.load95" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_USBCMD.load96" = load i64, ptr @UHCI_USBCMD, align 8
  %calltmp97 = call i64 @uhci_write16(i64 %"%hc_idx.load95", i64 %"%UHCI_USBCMD.load96", i64 0)
  %"%framelist.load98" = load i64, ptr %"%framelist", align 8
  call void @sad_ll_mem_write32(i64 %"%framelist.load98", i64 1)
  %calltmp99 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %calltmp100 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%uhci_transfer_errors.load101" = load i64, ptr %"%uhci_transfer_errors", align 8
  %addtmp102 = add i64 %"%uhci_transfer_errors.load101", 1
  store i64 %addtmp102, ptr %"%uhci_transfer_errors", align 8
  ret i64 -5

while_exit_290:                                   ; preds = %while_cond_288
  br label %merge_287
}

define i64 @uhci_get_device_descriptor(i64 %hc_idx, i64 %dev_addr, i64 %buf, i64 %buf_len) {
entry:
  %"%setup" = alloca i64, align 8
  %buf_len4 = alloca i64, align 8
  store i64 %buf_len, ptr %buf_len4, align 8
  %buf3 = alloca i64, align 8
  store i64 %buf, ptr %buf3, align 8
  %dev_addr2 = alloca i64, align 8
  store i64 %dev_addr, ptr %dev_addr2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @uhci_get_setup_buf(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%setup", align 8
  %"%setup.load" = load i64, ptr %"%setup", align 8
  %"%buf_len.load" = load i64, ptr %buf_len4, align 8
  call void @usb_build_setup(i64 %"%setup.load", i64 128, i64 6, i64 256, i64 0, i64 %"%buf_len.load")
  %"%hc_idx.load5" = load i64, ptr %hc_idx1, align 8
  %"%dev_addr.load" = load i64, ptr %dev_addr2, align 8
  %"%setup.load6" = load i64, ptr %"%setup", align 8
  %"%buf.load" = load i64, ptr %buf3, align 8
  %"%buf_len.load7" = load i64, ptr %buf_len4, align 8
  %calltmp8 = call i64 @uhci_control_transfer(i64 %"%hc_idx.load5", i64 %"%dev_addr.load", i64 %"%setup.load6", i64 %"%buf.load", i64 %"%buf_len.load7", i64 1)
  ret i64 %calltmp8
}

define i64 @uhci_get_config_descriptor(i64 %hc_idx, i64 %dev_addr, i64 %config_idx, i64 %buf, i64 %buf_len) {
entry:
  %"%w_value" = alloca i64, align 8
  %"%setup" = alloca i64, align 8
  %buf_len5 = alloca i64, align 8
  store i64 %buf_len, ptr %buf_len5, align 8
  %buf4 = alloca i64, align 8
  store i64 %buf, ptr %buf4, align 8
  %config_idx3 = alloca i64, align 8
  store i64 %config_idx, ptr %config_idx3, align 8
  %dev_addr2 = alloca i64, align 8
  store i64 %dev_addr, ptr %dev_addr2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @uhci_get_setup_buf(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%setup", align 8
  %"%config_idx.load" = load i64, ptr %config_idx3, align 8
  %addtmp = add i64 512, %"%config_idx.load"
  store i64 %addtmp, ptr %"%w_value", align 8
  %"%setup.load" = load i64, ptr %"%setup", align 8
  %"%w_value.load" = load i64, ptr %"%w_value", align 8
  %"%buf_len.load" = load i64, ptr %buf_len5, align 8
  call void @usb_build_setup(i64 %"%setup.load", i64 128, i64 6, i64 %"%w_value.load", i64 0, i64 %"%buf_len.load")
  %"%hc_idx.load6" = load i64, ptr %hc_idx1, align 8
  %"%dev_addr.load" = load i64, ptr %dev_addr2, align 8
  %"%setup.load7" = load i64, ptr %"%setup", align 8
  %"%buf.load" = load i64, ptr %buf4, align 8
  %"%buf_len.load8" = load i64, ptr %buf_len5, align 8
  %calltmp9 = call i64 @uhci_control_transfer(i64 %"%hc_idx.load6", i64 %"%dev_addr.load", i64 %"%setup.load7", i64 %"%buf.load", i64 %"%buf_len.load8", i64 1)
  ret i64 %calltmp9
}

define i64 @uhci_set_address(i64 %hc_idx, i64 %old_addr, i64 %new_addr) {
entry:
  %"%setup" = alloca i64, align 8
  %new_addr3 = alloca i64, align 8
  store i64 %new_addr, ptr %new_addr3, align 8
  %old_addr2 = alloca i64, align 8
  store i64 %old_addr, ptr %old_addr2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @uhci_get_setup_buf(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%setup", align 8
  %"%setup.load" = load i64, ptr %"%setup", align 8
  %"%new_addr.load" = load i64, ptr %new_addr3, align 8
  call void @usb_build_setup(i64 %"%setup.load", i64 0, i64 5, i64 %"%new_addr.load", i64 0, i64 0)
  %"%hc_idx.load4" = load i64, ptr %hc_idx1, align 8
  %"%old_addr.load" = load i64, ptr %old_addr2, align 8
  %"%setup.load5" = load i64, ptr %"%setup", align 8
  %calltmp6 = call i64 @uhci_control_transfer(i64 %"%hc_idx.load4", i64 %"%old_addr.load", i64 %"%setup.load5", i64 0, i64 0, i64 0)
  ret i64 %calltmp6
}

define i64 @uhci_set_configuration(i64 %hc_idx, i64 %dev_addr, i64 %config_value) {
entry:
  %"%setup" = alloca i64, align 8
  %config_value3 = alloca i64, align 8
  store i64 %config_value, ptr %config_value3, align 8
  %dev_addr2 = alloca i64, align 8
  store i64 %dev_addr, ptr %dev_addr2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @uhci_get_setup_buf(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%setup", align 8
  %"%setup.load" = load i64, ptr %"%setup", align 8
  %"%config_value.load" = load i64, ptr %config_value3, align 8
  call void @usb_build_setup(i64 %"%setup.load", i64 0, i64 9, i64 %"%config_value.load", i64 0, i64 0)
  %"%hc_idx.load4" = load i64, ptr %hc_idx1, align 8
  %"%dev_addr.load" = load i64, ptr %dev_addr2, align 8
  %"%setup.load5" = load i64, ptr %"%setup", align 8
  %calltmp6 = call i64 @uhci_control_transfer(i64 %"%hc_idx.load4", i64 %"%dev_addr.load", i64 %"%setup.load5", i64 0, i64 0, i64 0)
  ret i64 %calltmp6
}

define i64 @uhci_get_string_descriptor(i64 %hc_idx, i64 %dev_addr, i64 %str_idx, i64 %lang_id, i64 %buf, i64 %buf_len) {
entry:
  %"%w_value" = alloca i64, align 8
  %"%setup" = alloca i64, align 8
  %buf_len6 = alloca i64, align 8
  store i64 %buf_len, ptr %buf_len6, align 8
  %buf5 = alloca i64, align 8
  store i64 %buf, ptr %buf5, align 8
  %lang_id4 = alloca i64, align 8
  store i64 %lang_id, ptr %lang_id4, align 8
  %str_idx3 = alloca i64, align 8
  store i64 %str_idx, ptr %str_idx3, align 8
  %dev_addr2 = alloca i64, align 8
  store i64 %dev_addr, ptr %dev_addr2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @uhci_get_setup_buf(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%setup", align 8
  %"%str_idx.load" = load i64, ptr %str_idx3, align 8
  %addtmp = add i64 768, %"%str_idx.load"
  store i64 %addtmp, ptr %"%w_value", align 8
  %"%setup.load" = load i64, ptr %"%setup", align 8
  %"%w_value.load" = load i64, ptr %"%w_value", align 8
  %"%lang_id.load" = load i64, ptr %lang_id4, align 8
  %"%buf_len.load" = load i64, ptr %buf_len6, align 8
  call void @usb_build_setup(i64 %"%setup.load", i64 128, i64 6, i64 %"%w_value.load", i64 %"%lang_id.load", i64 %"%buf_len.load")
  %"%hc_idx.load7" = load i64, ptr %hc_idx1, align 8
  %"%dev_addr.load" = load i64, ptr %dev_addr2, align 8
  %"%setup.load8" = load i64, ptr %"%setup", align 8
  %"%buf.load" = load i64, ptr %buf5, align 8
  %"%buf_len.load9" = load i64, ptr %buf_len6, align 8
  %calltmp10 = call i64 @uhci_control_transfer(i64 %"%hc_idx.load7", i64 %"%dev_addr.load", i64 %"%setup.load8", i64 %"%buf.load", i64 %"%buf_len.load9", i64 1)
  ret i64 %calltmp10
}

define i64 @uhci_clear_feature(i64 %hc_idx, i64 %dev_addr, i64 %feature, i64 %recip) {
entry:
  %"%setup" = alloca i64, align 8
  %recip4 = alloca i64, align 8
  store i64 %recip, ptr %recip4, align 8
  %feature3 = alloca i64, align 8
  store i64 %feature, ptr %feature3, align 8
  %dev_addr2 = alloca i64, align 8
  store i64 %dev_addr, ptr %dev_addr2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @uhci_get_setup_buf(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%setup", align 8
  %"%setup.load" = load i64, ptr %"%setup", align 8
  %"%recip.load" = load i64, ptr %recip4, align 8
  %"%feature.load" = load i64, ptr %feature3, align 8
  call void @usb_build_setup(i64 %"%setup.load", i64 %"%recip.load", i64 1, i64 %"%feature.load", i64 0, i64 0)
  %"%hc_idx.load5" = load i64, ptr %hc_idx1, align 8
  %"%dev_addr.load" = load i64, ptr %dev_addr2, align 8
  %"%setup.load6" = load i64, ptr %"%setup", align 8
  %calltmp7 = call i64 @uhci_control_transfer(i64 %"%hc_idx.load5", i64 %"%dev_addr.load", i64 %"%setup.load6", i64 0, i64 0, i64 0)
  ret i64 %calltmp7
}

define i64 @uhci_set_idle(i64 %hc_idx, i64 %dev_addr, i64 %iface, i64 %duration) {
entry:
  %"%w_value" = alloca i64, align 8
  %"%setup" = alloca i64, align 8
  %duration4 = alloca i64, align 8
  store i64 %duration, ptr %duration4, align 8
  %iface3 = alloca i64, align 8
  store i64 %iface, ptr %iface3, align 8
  %dev_addr2 = alloca i64, align 8
  store i64 %dev_addr, ptr %dev_addr2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @uhci_get_setup_buf(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%setup", align 8
  %"%duration.load" = load i64, ptr %duration4, align 8
  %shltmp = shl i64 %"%duration.load", 8
  store i64 %shltmp, ptr %"%w_value", align 8
  %"%setup.load" = load i64, ptr %"%setup", align 8
  %"%w_value.load" = load i64, ptr %"%w_value", align 8
  %"%iface.load" = load i64, ptr %iface3, align 8
  call void @usb_build_setup(i64 %"%setup.load", i64 33, i64 10, i64 %"%w_value.load", i64 %"%iface.load", i64 0)
  %"%hc_idx.load5" = load i64, ptr %hc_idx1, align 8
  %"%dev_addr.load" = load i64, ptr %dev_addr2, align 8
  %"%setup.load6" = load i64, ptr %"%setup", align 8
  %calltmp7 = call i64 @uhci_control_transfer(i64 %"%hc_idx.load5", i64 %"%dev_addr.load", i64 %"%setup.load6", i64 0, i64 0, i64 0)
  ret i64 %calltmp7
}

define i64 @uhci_set_protocol(i64 %hc_idx, i64 %dev_addr, i64 %iface, i64 %protocol) {
entry:
  %"%setup" = alloca i64, align 8
  %protocol4 = alloca i64, align 8
  store i64 %protocol, ptr %protocol4, align 8
  %iface3 = alloca i64, align 8
  store i64 %iface, ptr %iface3, align 8
  %dev_addr2 = alloca i64, align 8
  store i64 %dev_addr, ptr %dev_addr2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @uhci_get_setup_buf(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%setup", align 8
  %"%setup.load" = load i64, ptr %"%setup", align 8
  %"%protocol.load" = load i64, ptr %protocol4, align 8
  %"%iface.load" = load i64, ptr %iface3, align 8
  call void @usb_build_setup(i64 %"%setup.load", i64 33, i64 11, i64 %"%protocol.load", i64 %"%iface.load", i64 0)
  %"%hc_idx.load5" = load i64, ptr %hc_idx1, align 8
  %"%dev_addr.load" = load i64, ptr %dev_addr2, align 8
  %"%setup.load6" = load i64, ptr %"%setup", align 8
  %calltmp7 = call i64 @uhci_control_transfer(i64 %"%hc_idx.load5", i64 %"%dev_addr.load", i64 %"%setup.load6", i64 0, i64 0, i64 0)
  ret i64 %calltmp7
}

define i64 @uhci_bulk_transfer(i64 %hc_idx, i64 %dev_addr, i64 %endpoint, i64 %buf, i64 %buf_len, i64 %is_in) {
entry:
  %"%cc" = alloca i64, align 8
  %"%ct" = alloca i64, align 8
  %"%total" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  %"%framelist" = alloca i64, align 8
  %"%st" = alloca i64, align 8
  %"%pkt" = alloca i64, align 8
  %"%td" = alloca i64, align 8
  %"%max_pkt" = alloca i64, align 8
  %"%offset" = alloca i64, align 8
  %"%remaining" = alloca i64, align 8
  %"%toggle" = alloca i64, align 8
  %"%last_td" = alloca i64, align 8
  %"%first_td" = alloca i64, align 8
  %"%pid" = alloca i64, align 8
  %"%uhci_transfer_errors" = alloca i64, align 8
  %"%qh" = alloca i64, align 8
  %"%uhci_total_transfers" = alloca i64, align 8
  %is_in6 = alloca i64, align 8
  store i64 %is_in, ptr %is_in6, align 8
  %buf_len5 = alloca i64, align 8
  store i64 %buf_len, ptr %buf_len5, align 8
  %buf4 = alloca i64, align 8
  store i64 %buf, ptr %buf4, align 8
  %endpoint3 = alloca i64, align 8
  store i64 %endpoint, ptr %endpoint3, align 8
  %dev_addr2 = alloca i64, align 8
  store i64 %dev_addr, ptr %dev_addr2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%uhci_total_transfers.load" = load i64, ptr @uhci_total_transfers, align 8
  %addtmp = add i64 %"%uhci_total_transfers.load", 1
  store i64 %addtmp, ptr %"%uhci_total_transfers", align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @uhci_reset_td_pool(i64 %"%hc_idx.load")
  %"%hc_idx.load7" = load i64, ptr %hc_idx1, align 8
  %calltmp8 = call i64 @uhci_get_qh_pool(i64 %"%hc_idx.load7")
  store i64 %calltmp8, ptr %"%qh", align 8
  %"%qh.load" = load i64, ptr %"%qh", align 8
  %cmpeqtmp = icmp eq i64 %"%qh.load", 0
  br i1 %cmpeqtmp, label %then_296, label %merge_297

else_327:                                         ; preds = %while_body_323
  %"%ct.load87" = load i64, ptr %"%ct", align 8
  %addtmp88 = add i64 %"%ct.load87", 32
  store i64 %addtmp88, ptr %"%ct", align 8
  %"%cc.load89" = load i64, ptr %"%cc", align 8
  %addtmp90 = add i64 %"%cc.load89", 1
  store i64 %addtmp90, ptr %"%cc", align 8
  br label %merge_326

merge_297:                                        ; preds = %entry
  %"%1209.load" = load i64, ptr @UHCI_PID_OUT, align 8
  store i64 %"%1209.load", ptr %"%pid", align 8
  %"%is_in.load" = load i64, ptr %is_in6, align 8
  %cmpeqtmp10 = icmp eq i64 %"%is_in.load", 1
  br i1 %cmpeqtmp10, label %then_298, label %merge_299

merge_299:                                        ; preds = %then_298, %merge_297
  store i64 0, ptr %"%first_td", align 8
  store i64 0, ptr %"%last_td", align 8
  store i64 0, ptr %"%toggle", align 8
  %"%1224.load" = load i64, ptr %buf_len5, align 8
  store i64 %"%1224.load", ptr %"%remaining", align 8
  store i64 0, ptr %"%offset", align 8
  store i64 8, ptr %"%max_pkt", align 8
  br label %while_cond_300

merge_304:                                        ; preds = %while_body_301
  %"%first_td.load" = load i64, ptr %"%first_td", align 8
  %cmpeqtmp16 = icmp eq i64 %"%first_td.load", 0
  br i1 %cmpeqtmp16, label %then_305, label %merge_306

merge_306:                                        ; preds = %then_305, %merge_304
  %"%last_td.load17" = load i64, ptr %"%last_td", align 8
  %cmpnetmp18 = icmp ne i64 %"%last_td.load17", 0
  br i1 %cmpnetmp18, label %then_307, label %merge_308

merge_308:                                        ; preds = %then_307, %merge_306
  %"%1254.load" = load i64, ptr %"%remaining", align 8
  store i64 %"%1254.load", ptr %"%pkt", align 8
  %"%pkt.load" = load i64, ptr %"%pkt", align 8
  %"%max_pkt.load" = load i64, ptr %"%max_pkt", align 8
  %cmpgttmp22 = icmp sgt i64 %"%pkt.load", %"%max_pkt.load"
  br i1 %cmpgttmp22, label %then_309, label %merge_310

merge_310:                                        ; preds = %then_309, %merge_308
  %"%td.load23" = load i64, ptr %"%td", align 8
  %calltmp24 = call i64 @uhci_td_set_status(i64 %"%td.load23", i64 1, i64 0, i64 0, i64 3)
  %"%td.load25" = load i64, ptr %"%td", align 8
  %"%pid.load" = load i64, ptr %"%pid", align 8
  %"%dev_addr.load" = load i64, ptr %dev_addr2, align 8
  %"%endpoint.load" = load i64, ptr %endpoint3, align 8
  %"%toggle.load" = load i64, ptr %"%toggle", align 8
  %"%pkt.load26" = load i64, ptr %"%pkt", align 8
  %calltmp27 = call i64 @uhci_td_set_token(i64 %"%td.load25", i64 %"%pid.load", i64 %"%dev_addr.load", i64 %"%endpoint.load", i64 %"%toggle.load", i64 %"%pkt.load26")
  %"%buf.load" = load i64, ptr %buf4, align 8
  %"%offset.load" = load i64, ptr %"%offset", align 8
  %addtmp28 = add i64 %"%buf.load", %"%offset.load"
  %"%td.load29" = load i64, ptr %"%td", align 8
  %calltmp30 = call i64 @uhci_td_set_buffer(i64 %"%td.load29", i64 %addtmp28)
  %"%1267.load" = load i64, ptr %"%td", align 8
  store i64 %"%1267.load", ptr %"%last_td", align 8
  %"%offset.load31" = load i64, ptr %"%offset", align 8
  %"%pkt.load32" = load i64, ptr %"%pkt", align 8
  %addtmp33 = add i64 %"%offset.load31", %"%pkt.load32"
  store i64 %addtmp33, ptr %"%offset", align 8
  %"%remaining.load34" = load i64, ptr %"%remaining", align 8
  %"%pkt.load35" = load i64, ptr %"%pkt", align 8
  %subtmp = sub i64 %"%remaining.load34", %"%pkt.load35"
  store i64 %subtmp, ptr %"%remaining", align 8
  %"%toggle.load36" = load i64, ptr %"%toggle", align 8
  %subtmp37 = sub i64 1, %"%toggle.load36"
  store i64 %subtmp37, ptr %"%toggle", align 8
  br label %while_cond_300

merge_312:                                        ; preds = %then_311, %while_exit_302
  %"%qh.load45" = load i64, ptr %"%qh", align 8
  %calltmp46 = call i64 @uhci_qh_set_head_link(i64 %"%qh.load45", i64 0, i64 1)
  %"%qh.load47" = load i64, ptr %"%qh", align 8
  %"%first_td.load48" = load i64, ptr %"%first_td", align 8
  %calltmp49 = call i64 @uhci_qh_set_element(i64 %"%qh.load47", i64 %"%first_td.load48", i64 0)
  %"%hc_idx.load50" = load i64, ptr %hc_idx1, align 8
  %calltmp51 = call i64 @uhci_get_framelist(i64 %"%hc_idx.load50")
  store i64 %calltmp51, ptr %"%framelist", align 8
  %"%qh.load52" = load i64, ptr %"%qh", align 8
  %addtmp53 = add i64 %"%qh.load52", 2
  %"%framelist.load" = load i64, ptr %"%framelist", align 8
  call void @sad_ll_mem_write32(i64 %"%framelist.load", i64 %addtmp53)
  %"%hc_idx.load54" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_FRNUM.load" = load i64, ptr @UHCI_FRNUM, align 8
  %calltmp55 = call i64 @uhci_write16(i64 %"%hc_idx.load54", i64 %"%UHCI_FRNUM.load", i64 0)
  %"%UHCI_CMD_RS.load" = load i64, ptr @UHCI_CMD_RS, align 8
  %"%UHCI_CMD_MAXP.load" = load i64, ptr @UHCI_CMD_MAXP, align 8
  %addtmp56 = add i64 %"%UHCI_CMD_RS.load", %"%UHCI_CMD_MAXP.load"
  %"%hc_idx.load57" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_USBCMD.load" = load i64, ptr @UHCI_USBCMD, align 8
  %calltmp58 = call i64 @uhci_write16(i64 %"%hc_idx.load57", i64 %"%UHCI_USBCMD.load", i64 %addtmp56)
  store i64 50000, ptr %"%timeout", align 8
  br label %while_cond_313

merge_317:                                        ; preds = %merge_319, %while_body_314
  %"%timeout.load71" = load i64, ptr %"%timeout", align 8
  %subtmp72 = sub i64 %"%timeout.load71", 1
  store i64 %subtmp72, ptr %"%timeout", align 8
  br label %while_cond_313

merge_319:                                        ; preds = %then_316
  br label %merge_317

merge_321:                                        ; preds = %then_318
  store i64 0, ptr %"%total", align 8
  %"%1327.load" = load i64, ptr %"%first_td", align 8
  store i64 %"%1327.load", ptr %"%ct", align 8
  store i64 0, ptr %"%cc", align 8
  br label %while_cond_322

merge_326:                                        ; preds = %else_327, %then_325
  br label %while_cond_322

then_296:                                         ; preds = %entry
  %"%uhci_transfer_errors.load" = load i64, ptr @uhci_transfer_errors, align 8
  %addtmp9 = add i64 %"%uhci_transfer_errors.load", 1
  store i64 %addtmp9, ptr %"%uhci_transfer_errors", align 8
  ret i64 -1

then_298:                                         ; preds = %merge_297
  %"%1213.load" = load i64, ptr @UHCI_PID_IN, align 8
  store i64 %"%1213.load", ptr %"%pid", align 8
  br label %merge_299

then_303:                                         ; preds = %while_body_301
  %"%uhci_transfer_errors.load14" = load i64, ptr %"%uhci_transfer_errors", align 8
  %addtmp15 = add i64 %"%uhci_transfer_errors.load14", 1
  store i64 %addtmp15, ptr %"%uhci_transfer_errors", align 8
  ret i64 -1

then_305:                                         ; preds = %merge_304
  %"%1247.load" = load i64, ptr %"%td", align 8
  store i64 %"%1247.load", ptr %"%first_td", align 8
  br label %merge_306

then_307:                                         ; preds = %merge_306
  %"%last_td.load19" = load i64, ptr %"%last_td", align 8
  %"%td.load20" = load i64, ptr %"%td", align 8
  %calltmp21 = call i64 @uhci_td_set_link(i64 %"%last_td.load19", i64 %"%td.load20", i64 0, i64 0)
  br label %merge_308

then_309:                                         ; preds = %merge_308
  %"%1257.load" = load i64, ptr %"%max_pkt", align 8
  store i64 %"%1257.load", ptr %"%pkt", align 8
  br label %merge_310

then_311:                                         ; preds = %while_exit_302
  %"%last_td.load38" = load i64, ptr %"%last_td", align 8
  %calltmp39 = call i64 @uhci_td_get_status(i64 %"%last_td.load38")
  store i64 %calltmp39, ptr %"%st", align 8
  %"%last_td.load40" = load i64, ptr %"%last_td", align 8
  %addtmp41 = add i64 %"%last_td.load40", 4
  %"%st.load" = load i64, ptr %"%st", align 8
  %"%UHCI_TD_IOC.load" = load i64, ptr @UHCI_TD_IOC, align 8
  %addtmp42 = add i64 %"%st.load", %"%UHCI_TD_IOC.load"
  call void @sad_ll_mem_write32(i64 %addtmp41, i64 %addtmp42)
  %"%last_td.load43" = load i64, ptr %"%last_td", align 8
  %calltmp44 = call i64 @uhci_td_set_link(i64 %"%last_td.load43", i64 0, i64 1, i64 0)
  br label %merge_312

then_316:                                         ; preds = %while_body_314
  %"%last_td.load68" = load i64, ptr %"%last_td", align 8
  %calltmp69 = call i64 @uhci_td_is_active(i64 %"%last_td.load68")
  %cmpeqtmp70 = icmp eq i64 %calltmp69, 0
  br i1 %cmpeqtmp70, label %then_318, label %merge_319

then_318:                                         ; preds = %then_316
  %"%hc_idx.load73" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_USBCMD.load74" = load i64, ptr @UHCI_USBCMD, align 8
  %calltmp75 = call i64 @uhci_write16(i64 %"%hc_idx.load73", i64 %"%UHCI_USBCMD.load74", i64 0)
  %"%framelist.load76" = load i64, ptr %"%framelist", align 8
  call void @sad_ll_mem_write32(i64 %"%framelist.load76", i64 1)
  %"%last_td.load77" = load i64, ptr %"%last_td", align 8
  %calltmp78 = call i64 @uhci_td_has_error(i64 %"%last_td.load77")
  %cmpeqtmp79 = icmp eq i64 %calltmp78, 1
  br i1 %cmpeqtmp79, label %then_320, label %merge_321

then_320:                                         ; preds = %then_318
  %"%uhci_transfer_errors.load80" = load i64, ptr %"%uhci_transfer_errors", align 8
  %addtmp81 = add i64 %"%uhci_transfer_errors.load80", 1
  store i64 %addtmp81, ptr %"%uhci_transfer_errors", align 8
  ret i64 -2

then_325:                                         ; preds = %while_body_323
  store i64 64, ptr %"%cc", align 8
  br label %merge_326

while_body_301:                                   ; preds = %while_cond_300
  %"%hc_idx.load11" = load i64, ptr %hc_idx1, align 8
  %calltmp12 = call i64 @uhci_alloc_td(i64 %"%hc_idx.load11")
  store i64 %calltmp12, ptr %"%td", align 8
  %"%td.load" = load i64, ptr %"%td", align 8
  %cmpeqtmp13 = icmp eq i64 %"%td.load", 0
  br i1 %cmpeqtmp13, label %then_303, label %merge_304

while_body_314:                                   ; preds = %while_cond_313
  %"%last_td.load60" = load i64, ptr %"%last_td", align 8
  %cmpnetmp61 = icmp ne i64 %"%last_td.load60", 0
  br i1 %cmpnetmp61, label %then_316, label %merge_317

while_body_323:                                   ; preds = %while_cond_322
  %"%ct.load" = load i64, ptr %"%ct", align 8
  %calltmp82 = call i64 @uhci_td_actual_len(i64 %"%ct.load")
  %"%total.load" = load i64, ptr %"%total", align 8
  %addtmp83 = add i64 %"%total.load", %calltmp82
  store i64 %addtmp83, ptr %"%total", align 8
  %"%ct.load84" = load i64, ptr %"%ct", align 8
  %"%last_td.load85" = load i64, ptr %"%last_td", align 8
  %cmpeqtmp86 = icmp eq i64 %"%ct.load84", %"%last_td.load85"
  br i1 %cmpeqtmp86, label %then_325, label %else_327

while_cond_300:                                   ; preds = %merge_310, %merge_299
  %"%remaining.load" = load i64, ptr %"%remaining", align 8
  %cmpgttmp = icmp sgt i64 %"%remaining.load", 0
  br i1 %cmpgttmp, label %while_body_301, label %while_exit_302

while_cond_313:                                   ; preds = %merge_317, %merge_312
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp59 = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp59, label %while_body_314, label %while_exit_315

while_cond_322:                                   ; preds = %merge_326, %merge_321
  %"%cc.load" = load i64, ptr %"%cc", align 8
  %cmplttmp = icmp slt i64 %"%cc.load", 64
  br i1 %cmplttmp, label %while_body_323, label %while_exit_324

while_exit_302:                                   ; preds = %while_cond_300
  %"%last_td.load" = load i64, ptr %"%last_td", align 8
  %cmpnetmp = icmp ne i64 %"%last_td.load", 0
  br i1 %cmpnetmp, label %then_311, label %merge_312

while_exit_315:                                   ; preds = %while_cond_313
  %"%hc_idx.load62" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_USBCMD.load63" = load i64, ptr @UHCI_USBCMD, align 8
  %calltmp64 = call i64 @uhci_write16(i64 %"%hc_idx.load62", i64 %"%UHCI_USBCMD.load63", i64 0)
  %"%framelist.load65" = load i64, ptr %"%framelist", align 8
  call void @sad_ll_mem_write32(i64 %"%framelist.load65", i64 1)
  %"%uhci_transfer_errors.load66" = load i64, ptr %"%uhci_transfer_errors", align 8
  %addtmp67 = add i64 %"%uhci_transfer_errors.load66", 1
  store i64 %addtmp67, ptr %"%uhci_transfer_errors", align 8
  ret i64 -5

while_exit_324:                                   ; preds = %while_cond_322
  %loadtmp = load i64, ptr %"%total", align 8
  ret i64 %loadtmp
}

define i64 @uhci_interrupt_transfer(i64 %hc_idx, i64 %dev_addr, i64 %endpoint, i64 %buf, i64 %buf_len) {
entry:
  %"%nak" = alloca i64, align 8
  %"%st" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  %"%framelist" = alloca i64, align 8
  %"%td" = alloca i64, align 8
  %"%uhci_transfer_errors" = alloca i64, align 8
  %"%qh" = alloca i64, align 8
  %"%uhci_total_transfers" = alloca i64, align 8
  %buf_len5 = alloca i64, align 8
  store i64 %buf_len, ptr %buf_len5, align 8
  %buf4 = alloca i64, align 8
  store i64 %buf, ptr %buf4, align 8
  %endpoint3 = alloca i64, align 8
  store i64 %endpoint, ptr %endpoint3, align 8
  %dev_addr2 = alloca i64, align 8
  store i64 %dev_addr, ptr %dev_addr2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%uhci_total_transfers.load" = load i64, ptr @uhci_total_transfers, align 8
  %addtmp = add i64 %"%uhci_total_transfers.load", 1
  store i64 %addtmp, ptr %"%uhci_total_transfers", align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @uhci_reset_td_pool(i64 %"%hc_idx.load")
  %"%hc_idx.load6" = load i64, ptr %hc_idx1, align 8
  %calltmp7 = call i64 @uhci_get_qh_pool(i64 %"%hc_idx.load6")
  store i64 %calltmp7, ptr %"%qh", align 8
  %"%qh.load" = load i64, ptr %"%qh", align 8
  %cmpeqtmp = icmp eq i64 %"%qh.load", 0
  br i1 %cmpeqtmp, label %then_328, label %merge_329

merge_329:                                        ; preds = %entry
  %"%hc_idx.load9" = load i64, ptr %hc_idx1, align 8
  %calltmp10 = call i64 @uhci_alloc_td(i64 %"%hc_idx.load9")
  store i64 %calltmp10, ptr %"%td", align 8
  %"%td.load" = load i64, ptr %"%td", align 8
  %cmpeqtmp11 = icmp eq i64 %"%td.load", 0
  br i1 %cmpeqtmp11, label %then_330, label %merge_331

merge_331:                                        ; preds = %merge_329
  %"%td.load14" = load i64, ptr %"%td", align 8
  %calltmp15 = call i64 @uhci_td_set_status(i64 %"%td.load14", i64 1, i64 1, i64 0, i64 3)
  %"%td.load16" = load i64, ptr %"%td", align 8
  %"%UHCI_PID_IN.load" = load i64, ptr @UHCI_PID_IN, align 8
  %"%dev_addr.load" = load i64, ptr %dev_addr2, align 8
  %"%endpoint.load" = load i64, ptr %endpoint3, align 8
  %"%buf_len.load" = load i64, ptr %buf_len5, align 8
  %calltmp17 = call i64 @uhci_td_set_token(i64 %"%td.load16", i64 %"%UHCI_PID_IN.load", i64 %"%dev_addr.load", i64 %"%endpoint.load", i64 0, i64 %"%buf_len.load")
  %"%td.load18" = load i64, ptr %"%td", align 8
  %"%buf.load" = load i64, ptr %buf4, align 8
  %calltmp19 = call i64 @uhci_td_set_buffer(i64 %"%td.load18", i64 %"%buf.load")
  %"%td.load20" = load i64, ptr %"%td", align 8
  %calltmp21 = call i64 @uhci_td_set_link(i64 %"%td.load20", i64 0, i64 1, i64 0)
  %"%qh.load22" = load i64, ptr %"%qh", align 8
  %calltmp23 = call i64 @uhci_qh_set_head_link(i64 %"%qh.load22", i64 0, i64 1)
  %"%qh.load24" = load i64, ptr %"%qh", align 8
  %"%td.load25" = load i64, ptr %"%td", align 8
  %calltmp26 = call i64 @uhci_qh_set_element(i64 %"%qh.load24", i64 %"%td.load25", i64 0)
  %"%hc_idx.load27" = load i64, ptr %hc_idx1, align 8
  %calltmp28 = call i64 @uhci_get_framelist(i64 %"%hc_idx.load27")
  store i64 %calltmp28, ptr %"%framelist", align 8
  %"%qh.load29" = load i64, ptr %"%qh", align 8
  %addtmp30 = add i64 %"%qh.load29", 2
  %"%framelist.load" = load i64, ptr %"%framelist", align 8
  call void @sad_ll_mem_write32(i64 %"%framelist.load", i64 %addtmp30)
  %"%hc_idx.load31" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_FRNUM.load" = load i64, ptr @UHCI_FRNUM, align 8
  %calltmp32 = call i64 @uhci_write16(i64 %"%hc_idx.load31", i64 %"%UHCI_FRNUM.load", i64 0)
  %"%UHCI_CMD_RS.load" = load i64, ptr @UHCI_CMD_RS, align 8
  %"%UHCI_CMD_MAXP.load" = load i64, ptr @UHCI_CMD_MAXP, align 8
  %addtmp33 = add i64 %"%UHCI_CMD_RS.load", %"%UHCI_CMD_MAXP.load"
  %"%hc_idx.load34" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_USBCMD.load" = load i64, ptr @UHCI_USBCMD, align 8
  %calltmp35 = call i64 @uhci_write16(i64 %"%hc_idx.load34", i64 %"%UHCI_USBCMD.load", i64 %addtmp33)
  store i64 5000, ptr %"%timeout", align 8
  br label %while_cond_332

merge_336:                                        ; preds = %while_body_333
  %"%timeout.load50" = load i64, ptr %"%timeout", align 8
  %subtmp = sub i64 %"%timeout.load50", 1
  store i64 %subtmp, ptr %"%timeout", align 8
  br label %while_cond_332

merge_338:                                        ; preds = %then_335
  %"%td.load54" = load i64, ptr %"%td", align 8
  %calltmp55 = call i64 @uhci_td_actual_len(i64 %"%td.load54")
  ret i64 %calltmp55

merge_340:                                        ; preds = %then_337
  %"%uhci_transfer_errors.load56" = load i64, ptr %"%uhci_transfer_errors", align 8
  %addtmp57 = add i64 %"%uhci_transfer_errors.load56", 1
  store i64 %addtmp57, ptr %"%uhci_transfer_errors", align 8
  ret i64 -2

then_328:                                         ; preds = %entry
  %"%uhci_transfer_errors.load" = load i64, ptr @uhci_transfer_errors, align 8
  %addtmp8 = add i64 %"%uhci_transfer_errors.load", 1
  store i64 %addtmp8, ptr %"%uhci_transfer_errors", align 8
  ret i64 -1

then_330:                                         ; preds = %merge_329
  %"%uhci_transfer_errors.load12" = load i64, ptr %"%uhci_transfer_errors", align 8
  %addtmp13 = add i64 %"%uhci_transfer_errors.load12", 1
  store i64 %addtmp13, ptr %"%uhci_transfer_errors", align 8
  ret i64 -1

then_335:                                         ; preds = %while_body_333
  %"%hc_idx.load43" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_USBCMD.load44" = load i64, ptr @UHCI_USBCMD, align 8
  %calltmp45 = call i64 @uhci_write16(i64 %"%hc_idx.load43", i64 %"%UHCI_USBCMD.load44", i64 0)
  %"%framelist.load46" = load i64, ptr %"%framelist", align 8
  call void @sad_ll_mem_write32(i64 %"%framelist.load46", i64 1)
  %"%td.load47" = load i64, ptr %"%td", align 8
  %calltmp48 = call i64 @uhci_td_has_error(i64 %"%td.load47")
  %cmpeqtmp49 = icmp eq i64 %calltmp48, 1
  br i1 %cmpeqtmp49, label %then_337, label %merge_338

then_337:                                         ; preds = %then_335
  %"%td.load51" = load i64, ptr %"%td", align 8
  %calltmp52 = call i64 @uhci_td_get_status(i64 %"%td.load51")
  store i64 %calltmp52, ptr %"%st", align 8
  %"%st.load" = load i64, ptr %"%st", align 8
  %"%UHCI_TD_NAK.load" = load i64, ptr @UHCI_TD_NAK, align 8
  %divtmp = sdiv i64 %"%st.load", %"%UHCI_TD_NAK.load"
  %andtmp = and i64 %divtmp, 1
  store i64 %andtmp, ptr %"%nak", align 8
  %"%nak.load" = load i64, ptr %"%nak", align 8
  %cmpeqtmp53 = icmp eq i64 %"%nak.load", 1
  br i1 %cmpeqtmp53, label %then_339, label %merge_340

then_339:                                         ; preds = %then_337
  ret i64 0

while_body_333:                                   ; preds = %while_cond_332
  %"%td.load36" = load i64, ptr %"%td", align 8
  %calltmp37 = call i64 @uhci_td_is_active(i64 %"%td.load36")
  %cmpeqtmp38 = icmp eq i64 %calltmp37, 0
  br i1 %cmpeqtmp38, label %then_335, label %merge_336

while_cond_332:                                   ; preds = %merge_336, %merge_331
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp, label %while_body_333, label %while_exit_334

while_exit_334:                                   ; preds = %while_cond_332
  %"%hc_idx.load39" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_USBCMD.load40" = load i64, ptr @UHCI_USBCMD, align 8
  %calltmp41 = call i64 @uhci_write16(i64 %"%hc_idx.load39", i64 %"%UHCI_USBCMD.load40", i64 0)
  %"%framelist.load42" = load i64, ptr %"%framelist", align 8
  call void @sad_ll_mem_write32(i64 %"%framelist.load42", i64 1)
  ret i64 0
}

define i64 @uhci_enumerate_device(i64 %hc_idx, i64 %port_num) {
entry:
  %"%total_len" = alloca i64, align 8
  %"%total_hi" = alloca i64, align 8
  %"%total_lo" = alloca i64, align 8
  %"%d" = alloca i64, align 8
  %"%wait" = alloca i64, align 8
  %"%new_addr" = alloca i64, align 8
  %"%maxpkt" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%data_buf" = alloca i64, align 8
  %"%dev_idx" = alloca i64, align 8
  %"%speed" = alloca i64, align 8
  %port_num2 = alloca i64, align 8
  store i64 %port_num, ptr %port_num2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %calltmp = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %"%1450.load" = load i64, ptr %hc_idx1, align 8
  %calltmp3 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1450.load")
  %calltmp4 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%1455.load" = load i64, ptr %port_num2, align 8
  %calltmp5 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1455.load")
  %calltmp6 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 1, ptr %"%speed", align 8
  %"%port_num.load" = load i64, ptr %port_num2, align 8
  %cmpeqtmp = icmp eq i64 %"%port_num.load", 0
  br i1 %cmpeqtmp, label %then_341, label %else_343

else_343:                                         ; preds = %entry
  %"%hc_idx.load8" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%hc_idx.load8", 0
  br i1 %cmpeqtmp9, label %then_352, label %merge_353

merge_342:                                        ; preds = %merge_359, %merge_351
  %"%hc_idx.load10" = load i64, ptr %hc_idx1, align 8
  %"%port_num.load11" = load i64, ptr %port_num2, align 8
  %"%speed.load" = load i64, ptr %"%speed", align 8
  call void @usb_register_device(i64 %"%hc_idx.load10", i64 %"%port_num.load11", i64 %"%speed.load")
  store i64 0, ptr %"%dev_idx", align 8
  %"%dev_idx.load" = load i64, ptr %"%dev_idx", align 8
  %cmplttmp = icmp slt i64 %"%dev_idx.load", 0
  br i1 %cmplttmp, label %then_360, label %merge_361

merge_345:                                        ; preds = %then_344, %then_341
  %"%hc_idx.load12" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp13 = icmp eq i64 %"%hc_idx.load12", 1
  br i1 %cmpeqtmp13, label %then_346, label %merge_347

merge_347:                                        ; preds = %then_346, %merge_345
  %"%hc_idx.load14" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%hc_idx.load14", 2
  br i1 %cmpeqtmp15, label %then_348, label %merge_349

merge_349:                                        ; preds = %then_348, %merge_347
  %"%hc_idx.load16" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp17 = icmp eq i64 %"%hc_idx.load16", 3
  br i1 %cmpeqtmp17, label %then_350, label %merge_351

merge_351:                                        ; preds = %then_350, %merge_349
  br label %merge_342

merge_353:                                        ; preds = %then_352, %else_343
  %"%hc_idx.load18" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp19 = icmp eq i64 %"%hc_idx.load18", 1
  br i1 %cmpeqtmp19, label %then_354, label %merge_355

merge_355:                                        ; preds = %then_354, %merge_353
  %"%hc_idx.load20" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%hc_idx.load20", 2
  br i1 %cmpeqtmp21, label %then_356, label %merge_357

merge_357:                                        ; preds = %then_356, %merge_355
  %"%hc_idx.load22" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp23 = icmp eq i64 %"%hc_idx.load22", 3
  br i1 %cmpeqtmp23, label %then_358, label %merge_359

merge_359:                                        ; preds = %then_358, %merge_357
  br label %merge_342

merge_361:                                        ; preds = %merge_342
  %"%hc_idx.load24" = load i64, ptr %hc_idx1, align 8
  %calltmp25 = call i64 @uhci_get_data_buf(i64 %"%hc_idx.load24")
  store i64 %calltmp25, ptr %"%data_buf", align 8
  %"%data_buf.load" = load i64, ptr %"%data_buf", align 8
  call void @sad_ll_memset(i64 %"%data_buf.load", i64 0, i64 64)
  %"%hc_idx.load26" = load i64, ptr %hc_idx1, align 8
  %"%data_buf.load27" = load i64, ptr %"%data_buf", align 8
  %calltmp28 = call i64 @uhci_get_device_descriptor(i64 %"%hc_idx.load26", i64 0, i64 %"%data_buf.load27", i64 8)
  store i64 %calltmp28, ptr %"%result", align 8
  %"%result.load" = load i64, ptr %"%result", align 8
  %cmplttmp29 = icmp slt i64 %"%result.load", 0
  br i1 %cmplttmp29, label %then_362, label %merge_363

merge_363:                                        ; preds = %merge_361
  %"%data_buf.load32" = load i64, ptr %"%data_buf", align 8
  %addtmp = add i64 %"%data_buf.load32", 7
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%maxpkt", align 8
  %"%maxpkt.load" = load i64, ptr %"%maxpkt", align 8
  %cmpgttmp = icmp sgt i64 %"%maxpkt.load", 0
  br i1 %cmpgttmp, label %then_364, label %merge_365

merge_365:                                        ; preds = %then_364, %merge_363
  %"%dev_idx.load36" = load i64, ptr %"%dev_idx", align 8
  %addtmp37 = add i64 %"%dev_idx.load36", 1
  store i64 %addtmp37, ptr %"%new_addr", align 8
  %"%hc_idx.load38" = load i64, ptr %hc_idx1, align 8
  %"%new_addr.load" = load i64, ptr %"%new_addr", align 8
  %calltmp39 = call i64 @uhci_set_address(i64 %"%hc_idx.load38", i64 0, i64 %"%new_addr.load")
  store i64 %calltmp39, ptr %"%result", align 8
  %"%result.load40" = load i64, ptr %"%result", align 8
  %cmplttmp41 = icmp slt i64 %"%result.load40", 0
  br i1 %cmplttmp41, label %then_366, label %merge_367

merge_367:                                        ; preds = %merge_365
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%wait", align 8
  br label %while_cond_368

merge_372:                                        ; preds = %then_371, %while_exit_370
  %"%data_buf.load56" = load i64, ptr %"%data_buf", align 8
  call void @sad_ll_memset(i64 %"%data_buf.load56", i64 0, i64 256)
  %"%hc_idx.load57" = load i64, ptr %hc_idx1, align 8
  %"%new_addr.load58" = load i64, ptr %"%new_addr", align 8
  %"%data_buf.load59" = load i64, ptr %"%data_buf", align 8
  %calltmp60 = call i64 @uhci_get_config_descriptor(i64 %"%hc_idx.load57", i64 %"%new_addr.load58", i64 0, i64 %"%data_buf.load59", i64 9)
  store i64 %calltmp60, ptr %"%result", align 8
  %"%result.load61" = load i64, ptr %"%result", align 8
  %cmpgetmp62 = icmp sge i64 %"%result.load61", 4
  br i1 %cmpgetmp62, label %then_373, label %merge_374

merge_374:                                        ; preds = %merge_376, %merge_372
  %"%hc_idx.load69" = load i64, ptr %hc_idx1, align 8
  %"%new_addr.load70" = load i64, ptr %"%new_addr", align 8
  %calltmp71 = call i64 @uhci_set_configuration(i64 %"%hc_idx.load69", i64 %"%new_addr.load70", i64 1)
  store i64 %calltmp71, ptr %"%result", align 8
  %"%result.load72" = load i64, ptr %"%result", align 8
  %cmplttmp73 = icmp slt i64 %"%result.load72", 0
  br i1 %cmplttmp73, label %then_381, label %merge_382

merge_376:                                        ; preds = %merge_380, %then_373
  br label %merge_374

merge_378:                                        ; preds = %then_377, %then_375
  %"%data_buf.load76" = load i64, ptr %"%data_buf", align 8
  call void @sad_ll_memset(i64 %"%data_buf.load76", i64 0, i64 256)
  %"%hc_idx.load77" = load i64, ptr %hc_idx1, align 8
  %"%new_addr.load78" = load i64, ptr %"%new_addr", align 8
  %"%data_buf.load79" = load i64, ptr %"%data_buf", align 8
  %"%total_len.load80" = load i64, ptr %"%total_len", align 8
  %calltmp81 = call i64 @uhci_get_config_descriptor(i64 %"%hc_idx.load77", i64 %"%new_addr.load78", i64 0, i64 %"%data_buf.load79", i64 %"%total_len.load80")
  store i64 %calltmp81, ptr %"%result", align 8
  %"%result.load82" = load i64, ptr %"%result", align 8
  %cmpgttmp83 = icmp sgt i64 %"%result.load82", 0
  br i1 %cmpgttmp83, label %then_379, label %merge_380

merge_380:                                        ; preds = %then_379, %merge_378
  br label %merge_376

merge_382:                                        ; preds = %then_381, %merge_374
  %"%dev_idx.load89" = load i64, ptr %"%dev_idx", align 8
  call void @usb_classify_device(i64 %"%dev_idx.load89")
  %calltmp90 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  %calltmp91 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr %"%dev_idx", align 8
  ret i64 %loadtmp

then_341:                                         ; preds = %entry
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp7 = icmp eq i64 %"%hc_idx.load", 0
  br i1 %cmpeqtmp7, label %then_344, label %merge_345

then_344:                                         ; preds = %then_341
  %"%1466.load" = load i64, ptr @uhci_port1_speed_0, align 8
  store i64 %"%1466.load", ptr %"%speed", align 8
  br label %merge_345

then_346:                                         ; preds = %merge_345
  %"%1470.load" = load i64, ptr @uhci_port1_speed_1, align 8
  store i64 %"%1470.load", ptr %"%speed", align 8
  br label %merge_347

then_348:                                         ; preds = %merge_347
  %"%1474.load" = load i64, ptr @uhci_port1_speed_2, align 8
  store i64 %"%1474.load", ptr %"%speed", align 8
  br label %merge_349

then_350:                                         ; preds = %merge_349
  %"%1478.load" = load i64, ptr @uhci_port1_speed_3, align 8
  store i64 %"%1478.load", ptr %"%speed", align 8
  br label %merge_351

then_352:                                         ; preds = %else_343
  %"%1482.load" = load i64, ptr @uhci_port2_speed_0, align 8
  store i64 %"%1482.load", ptr %"%speed", align 8
  br label %merge_353

then_354:                                         ; preds = %merge_353
  %"%1486.load" = load i64, ptr @uhci_port2_speed_1, align 8
  store i64 %"%1486.load", ptr %"%speed", align 8
  br label %merge_355

then_356:                                         ; preds = %merge_355
  %"%1490.load" = load i64, ptr @uhci_port2_speed_2, align 8
  store i64 %"%1490.load", ptr %"%speed", align 8
  br label %merge_357

then_358:                                         ; preds = %merge_357
  %"%1494.load" = load i64, ptr @uhci_port2_speed_3, align 8
  store i64 %"%1494.load", ptr %"%speed", align 8
  br label %merge_359

then_360:                                         ; preds = %merge_342
  ret i64 -1

then_362:                                         ; preds = %merge_361
  %calltmp30 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  %calltmp31 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -2

then_364:                                         ; preds = %merge_363
  %calltmp33 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%1534.load" = load i64, ptr %"%maxpkt", align 8
  %calltmp34 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1534.load")
  %calltmp35 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_365

then_366:                                         ; preds = %merge_365
  %calltmp42 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  %calltmp43 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -3

then_371:                                         ; preds = %while_exit_370
  %"%data_buf.load54" = load i64, ptr %"%data_buf", align 8
  %"%dev_idx.load55" = load i64, ptr %"%dev_idx", align 8
  call void @usb_parse_device_descriptor(i64 %"%data_buf.load54", i64 %"%dev_idx.load55")
  br label %merge_372

then_373:                                         ; preds = %merge_372
  %"%data_buf.load63" = load i64, ptr %"%data_buf", align 8
  %addtmp64 = add i64 %"%data_buf.load63", 2
  call void @sad_ll_mem_read8(i64 %addtmp64)
  store i64 0, ptr %"%total_lo", align 8
  %"%data_buf.load65" = load i64, ptr %"%data_buf", align 8
  %addtmp66 = add i64 %"%data_buf.load65", 3
  call void @sad_ll_mem_read8(i64 %addtmp66)
  store i64 0, ptr %"%total_hi", align 8
  %"%total_hi.load" = load i64, ptr %"%total_hi", align 8
  %shltmp = shl i64 %"%total_hi.load", 8
  %"%total_lo.load" = load i64, ptr %"%total_lo", align 8
  %addtmp67 = add i64 %shltmp, %"%total_lo.load"
  store i64 %addtmp67, ptr %"%total_len", align 8
  %"%total_len.load" = load i64, ptr %"%total_len", align 8
  %cmpgttmp68 = icmp sgt i64 %"%total_len.load", 0
  br i1 %cmpgttmp68, label %then_375, label %merge_376

then_375:                                         ; preds = %then_373
  %"%total_len.load74" = load i64, ptr %"%total_len", align 8
  %cmpgttmp75 = icmp sgt i64 %"%total_len.load74", 256
  br i1 %cmpgttmp75, label %then_377, label %merge_378

then_377:                                         ; preds = %then_375
  store i64 256, ptr %"%total_len", align 8
  br label %merge_378

then_379:                                         ; preds = %merge_378
  %"%data_buf.load84" = load i64, ptr %"%data_buf", align 8
  %"%total_len.load85" = load i64, ptr %"%total_len", align 8
  %"%dev_idx.load86" = load i64, ptr %"%dev_idx", align 8
  call void @usb_parse_config_descriptor(i64 %"%data_buf.load84", i64 %"%total_len.load85", i64 %"%dev_idx.load86")
  br label %merge_380

then_381:                                         ; preds = %merge_374
  %calltmp87 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %calltmp88 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_382

while_body_369:                                   ; preds = %while_cond_368
  store i64 0, ptr %"%d", align 8
  br label %while_cond_368

while_cond_368:                                   ; preds = %while_body_369, %merge_367
  call void @sad_ll_timer_get_ticks()
  %"%wait.load" = load i64, ptr %"%wait", align 8
  %subtmp = sub i64 0, %"%wait.load"
  %cmplttmp44 = icmp slt i64 %subtmp, 200
  br i1 %cmplttmp44, label %while_body_369, label %while_exit_370

while_exit_370:                                   ; preds = %while_cond_368
  %calltmp45 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %"%1563.load" = load i64, ptr %"%new_addr", align 8
  %calltmp46 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1563.load")
  %calltmp47 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%data_buf.load48" = load i64, ptr %"%data_buf", align 8
  call void @sad_ll_memset(i64 %"%data_buf.load48", i64 0, i64 64)
  %"%hc_idx.load49" = load i64, ptr %hc_idx1, align 8
  %"%new_addr.load50" = load i64, ptr %"%new_addr", align 8
  %"%data_buf.load51" = load i64, ptr %"%data_buf", align 8
  %calltmp52 = call i64 @uhci_get_device_descriptor(i64 %"%hc_idx.load49", i64 %"%new_addr.load50", i64 %"%data_buf.load51", i64 18)
  store i64 %calltmp52, ptr %"%result", align 8
  %"%result.load53" = load i64, ptr %"%result", align 8
  %cmpgetmp = icmp sge i64 %"%result.load53", 8
  br i1 %cmpgetmp, label %then_371, label %merge_372
}

define i64 @uhci_scan_ports(i64 %hc_idx) {
entry:
  %"%csc2" = alloca i64, align 8
  %"%connected2" = alloca i64, align 8
  %"%sc2" = alloca i64, align 8
  %"%csc1" = alloca i64, align 8
  %"%connected1" = alloca i64, align 8
  %"%sc1" = alloca i64, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %calltmp = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  %"%1633.load" = load i64, ptr %hc_idx1, align 8
  %calltmp2 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1633.load")
  %calltmp3 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_PORTSC1.load" = load i64, ptr @UHCI_PORTSC1, align 8
  %calltmp4 = call i64 @uhci_read16(i64 %"%hc_idx.load", i64 %"%UHCI_PORTSC1.load")
  store i64 %calltmp4, ptr %"%sc1", align 8
  %"%sc1.load" = load i64, ptr %"%sc1", align 8
  %andtmp = and i64 %"%sc1.load", 1
  store i64 %andtmp, ptr %"%connected1", align 8
  %"%sc1.load5" = load i64, ptr %"%sc1", align 8
  %shrtmp = lshr i64 %"%sc1.load5", 1
  %andtmp6 = and i64 %shrtmp, 1
  store i64 %andtmp6, ptr %"%csc1", align 8
  %"%connected1.load" = load i64, ptr %"%connected1", align 8
  %cmpeqtmp = icmp eq i64 %"%connected1.load", 1
  br i1 %cmpeqtmp, label %then_383, label %else_385

else_385:                                         ; preds = %entry
  %calltmp13 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  %calltmp14 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_384

else_390:                                         ; preds = %merge_387
  %calltmp32 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %calltmp33 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_389

merge_384:                                        ; preds = %else_385, %then_383
  %"%csc1.load" = load i64, ptr %"%csc1", align 8
  %cmpeqtmp15 = icmp eq i64 %"%csc1.load", 1
  br i1 %cmpeqtmp15, label %then_386, label %merge_387

merge_387:                                        ; preds = %then_386, %merge_384
  %"%hc_idx.load19" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_PORTSC2.load" = load i64, ptr @UHCI_PORTSC2, align 8
  %calltmp20 = call i64 @uhci_read16(i64 %"%hc_idx.load19", i64 %"%UHCI_PORTSC2.load")
  store i64 %calltmp20, ptr %"%sc2", align 8
  %"%sc2.load" = load i64, ptr %"%sc2", align 8
  %andtmp21 = and i64 %"%sc2.load", 1
  store i64 %andtmp21, ptr %"%connected2", align 8
  %"%sc2.load22" = load i64, ptr %"%sc2", align 8
  %shrtmp23 = lshr i64 %"%sc2.load22", 1
  %andtmp24 = and i64 %shrtmp23, 1
  store i64 %andtmp24, ptr %"%csc2", align 8
  %"%connected2.load" = load i64, ptr %"%connected2", align 8
  %cmpeqtmp25 = icmp eq i64 %"%connected2.load", 1
  br i1 %cmpeqtmp25, label %then_388, label %else_390

merge_389:                                        ; preds = %else_390, %then_388
  %"%csc2.load" = load i64, ptr %"%csc2", align 8
  %cmpeqtmp34 = icmp eq i64 %"%csc2.load", 1
  br i1 %cmpeqtmp34, label %then_391, label %merge_392

merge_392:                                        ; preds = %then_391, %merge_389
  %"%connected1.load39" = load i64, ptr %"%connected1", align 8
  %"%connected2.load40" = load i64, ptr %"%connected2", align 8
  %addtmp = add i64 %"%connected1.load39", %"%connected2.load40"
  ret i64 %addtmp

then_383:                                         ; preds = %entry
  %calltmp7 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  %calltmp8 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hc_idx.load9" = load i64, ptr %hc_idx1, align 8
  %calltmp10 = call i64 @uhci_port_reset(i64 %"%hc_idx.load9", i64 0)
  %"%hc_idx.load11" = load i64, ptr %hc_idx1, align 8
  %calltmp12 = call i64 @uhci_enumerate_device(i64 %"%hc_idx.load11", i64 0)
  br label %merge_384

then_386:                                         ; preds = %merge_384
  %"%hc_idx.load16" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_PORTSC1.load17" = load i64, ptr @UHCI_PORTSC1, align 8
  %"%UHCI_PORT_CSC.load" = load i64, ptr @UHCI_PORT_CSC, align 8
  %calltmp18 = call i64 @uhci_write16(i64 %"%hc_idx.load16", i64 %"%UHCI_PORTSC1.load17", i64 %"%UHCI_PORT_CSC.load")
  br label %merge_387

then_388:                                         ; preds = %merge_387
  %calltmp26 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  %calltmp27 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hc_idx.load28" = load i64, ptr %hc_idx1, align 8
  %calltmp29 = call i64 @uhci_port_reset(i64 %"%hc_idx.load28", i64 1)
  %"%hc_idx.load30" = load i64, ptr %hc_idx1, align 8
  %calltmp31 = call i64 @uhci_enumerate_device(i64 %"%hc_idx.load30", i64 1)
  br label %merge_389

then_391:                                         ; preds = %merge_389
  %"%hc_idx.load35" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_PORTSC2.load36" = load i64, ptr @UHCI_PORTSC2, align 8
  %"%UHCI_PORT_CSC.load37" = load i64, ptr @UHCI_PORT_CSC, align 8
  %calltmp38 = call i64 @uhci_write16(i64 %"%hc_idx.load35", i64 %"%UHCI_PORTSC2.load36", i64 %"%UHCI_PORT_CSC.load37")
  br label %merge_392
}

define i64 @"uhci_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %"%st" = alloca i64, align 8
  %"%base" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %calltmp = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  %calltmp1 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  %"%1692.load" = load i64, ptr @uhci_count, align 8
  %calltmp3 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1692.load")
  %calltmp4 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp5 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  %"%1698.load" = load i64, ptr @uhci_total_transfers, align 8
  %calltmp6 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1698.load")
  %calltmp7 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp8 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  %"%1704.load" = load i64, ptr @uhci_transfer_errors, align 8
  %calltmp9 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1704.load")
  %calltmp10 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp11 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  %"%1710.load" = load i64, ptr @uhci_port_resets, align 8
  %calltmp12 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1710.load")
  %calltmp13 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%i", align 8
  br label %while_cond_393

while_body_394:                                   ; preds = %while_cond_393
  %"%i.load14" = load i64, ptr %"%i", align 8
  %calltmp15 = call i64 @uhci_get_base(i64 %"%i.load14")
  store i64 %calltmp15, ptr %"%base", align 8
  %"%i.load16" = load i64, ptr %"%i", align 8
  %calltmp17 = call i64 @uhci_get_status(i64 %"%i.load16")
  store i64 %calltmp17, ptr %"%st", align 8
  %calltmp18 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.30 to i64))
  %"%1726.load" = load i64, ptr %"%i", align 8
  %calltmp19 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1726.load")
  %calltmp20 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  %"%1731.load" = load i64, ptr %"%base", align 8
  %calltmp21 = call i64 @uhci_serial_hex16(i64 %"%1731.load")
  %calltmp22 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.32 to i64))
  %"%1736.load" = load i64, ptr %"%st", align 8
  %calltmp23 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1736.load")
  %calltmp24 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%i.load25" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load25", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_393

while_cond_393:                                   ; preds = %while_body_394, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%uhci_count.load" = load i64, ptr @uhci_count, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%uhci_count.load"
  br i1 %cmplttmp, label %while_body_394, label %while_exit_395

while_exit_395:                                   ; preds = %while_cond_393
  %calltmp26 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  %calltmp27 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @"uhci_\D9\87\D9\8A\D8\A6"(i64 %hc_idx, i64 %io_base) {
entry:
  %"%uhci_count" = alloca i64, align 8
  %"%base" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %"%sbuf" = alloca i64, align 8
  %"%dbuf" = alloca i64, align 8
  %"%tdp" = alloca i64, align 8
  %"%qhp" = alloca i64, align 8
  %"%fl" = alloca i64, align 8
  %io_base2 = alloca i64, align 8
  store i64 %io_base, ptr %io_base2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %calltmp = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.34 to i64))
  %"%1748.load" = load i64, ptr %hc_idx1, align 8
  %calltmp3 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1748.load")
  %calltmp4 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  %"%1753.load" = load i64, ptr %io_base2, align 8
  %calltmp5 = call i64 @uhci_serial_hex16(i64 %"%1753.load")
  %calltmp6 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%uhci_count.load" = load i64, ptr @uhci_count, align 8
  %cmpgetmp = icmp sge i64 %"%uhci_count.load", 4
  br i1 %cmpgetmp, label %then_396, label %merge_397

merge_397:                                        ; preds = %entry
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %"%io_base.load" = load i64, ptr %io_base2, align 8
  %calltmp9 = call i64 @uhci_set_base(i64 %"%hc_idx.load", i64 %"%io_base.load")
  call void @sad_ll_kmalloc(i64 4096)
  store i64 0, ptr %"%fl", align 8
  %"%hc_idx.load10" = load i64, ptr %hc_idx1, align 8
  %"%fl.load" = load i64, ptr %"%fl", align 8
  %calltmp11 = call i64 @uhci_set_framelist(i64 %"%hc_idx.load10", i64 %"%fl.load")
  call void @sad_ll_kmalloc(i64 512)
  store i64 0, ptr %"%qhp", align 8
  %"%hc_idx.load12" = load i64, ptr %hc_idx1, align 8
  %"%qhp.load" = load i64, ptr %"%qhp", align 8
  %calltmp13 = call i64 @uhci_set_qh_pool(i64 %"%hc_idx.load12", i64 %"%qhp.load")
  call void @sad_ll_kmalloc(i64 1024)
  store i64 0, ptr %"%tdp", align 8
  %"%hc_idx.load14" = load i64, ptr %hc_idx1, align 8
  %"%tdp.load" = load i64, ptr %"%tdp", align 8
  %calltmp15 = call i64 @uhci_set_td_pool(i64 %"%hc_idx.load14", i64 %"%tdp.load")
  call void @sad_ll_kmalloc(i64 512)
  store i64 0, ptr %"%dbuf", align 8
  %"%hc_idx.load16" = load i64, ptr %hc_idx1, align 8
  %"%dbuf.load" = load i64, ptr %"%dbuf", align 8
  %calltmp17 = call i64 @uhci_set_data_buf(i64 %"%hc_idx.load16", i64 %"%dbuf.load")
  call void @sad_ll_kmalloc(i64 32)
  store i64 0, ptr %"%sbuf", align 8
  %"%hc_idx.load18" = load i64, ptr %hc_idx1, align 8
  %"%sbuf.load" = load i64, ptr %"%sbuf", align 8
  %calltmp19 = call i64 @uhci_set_setup_buf(i64 %"%hc_idx.load18", i64 %"%sbuf.load")
  %"%hc_idx.load20" = load i64, ptr %hc_idx1, align 8
  %calltmp21 = call i64 @uhci_reset(i64 %"%hc_idx.load20")
  store i64 0, ptr %"%i", align 8
  br label %while_cond_398

then_396:                                         ; preds = %entry
  %calltmp7 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.36 to i64))
  %calltmp8 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

while_body_399:                                   ; preds = %while_cond_398
  %"%i.load22" = load i64, ptr %"%i", align 8
  %shltmp = shl i64 %"%i.load22", 2
  %"%fl.load23" = load i64, ptr %"%fl", align 8
  %addtmp = add i64 %"%fl.load23", %shltmp
  call void @sad_ll_mem_write32(i64 %addtmp, i64 1)
  %"%i.load24" = load i64, ptr %"%i", align 8
  %addtmp25 = add i64 %"%i.load24", 1
  store i64 %addtmp25, ptr %"%i", align 8
  br label %while_cond_398

while_cond_398:                                   ; preds = %while_body_399, %merge_397
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%UHCI_FRAME_COUNT.load" = load i64, ptr @UHCI_FRAME_COUNT, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%UHCI_FRAME_COUNT.load"
  br i1 %cmplttmp, label %while_body_399, label %while_exit_400

while_exit_400:                                   ; preds = %while_cond_398
  %"%hc_idx.load26" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_FRBASEADD.load" = load i64, ptr @UHCI_FRBASEADD, align 8
  %"%fl.load27" = load i64, ptr %"%fl", align 8
  %calltmp28 = call i64 @uhci_write32(i64 %"%hc_idx.load26", i64 %"%UHCI_FRBASEADD.load", i64 %"%fl.load27")
  %"%hc_idx.load29" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_FRNUM.load" = load i64, ptr @UHCI_FRNUM, align 8
  %calltmp30 = call i64 @uhci_write16(i64 %"%hc_idx.load29", i64 %"%UHCI_FRNUM.load", i64 0)
  %"%hc_idx.load31" = load i64, ptr %hc_idx1, align 8
  %calltmp32 = call i64 @uhci_get_base(i64 %"%hc_idx.load31")
  store i64 %calltmp32, ptr %"%base", align 8
  %"%base.load" = load i64, ptr %"%base", align 8
  %"%UHCI_SOFMOD.load" = load i64, ptr @UHCI_SOFMOD, align 8
  %addtmp33 = add i64 %"%base.load", %"%UHCI_SOFMOD.load"
  call void @sad_ll_port_outb(i64 %addtmp33, i64 64)
  %"%hc_idx.load34" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_USBSTS.load" = load i64, ptr @UHCI_USBSTS, align 8
  %calltmp35 = call i64 @uhci_write16(i64 %"%hc_idx.load34", i64 %"%UHCI_USBSTS.load", i64 63)
  %"%hc_idx.load36" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_USBINTR.load" = load i64, ptr @UHCI_USBINTR, align 8
  %calltmp37 = call i64 @uhci_write16(i64 %"%hc_idx.load36", i64 %"%UHCI_USBINTR.load", i64 15)
  %"%UHCI_CMD_RS.load" = load i64, ptr @UHCI_CMD_RS, align 8
  %"%UHCI_CMD_CF.load" = load i64, ptr @UHCI_CMD_CF, align 8
  %addtmp38 = add i64 %"%UHCI_CMD_RS.load", %"%UHCI_CMD_CF.load"
  %"%UHCI_CMD_MAXP.load" = load i64, ptr @UHCI_CMD_MAXP, align 8
  %addtmp39 = add i64 %addtmp38, %"%UHCI_CMD_MAXP.load"
  %"%hc_idx.load40" = load i64, ptr %hc_idx1, align 8
  %"%UHCI_USBCMD.load" = load i64, ptr @UHCI_USBCMD, align 8
  %calltmp41 = call i64 @uhci_write16(i64 %"%hc_idx.load40", i64 %"%UHCI_USBCMD.load", i64 %addtmp39)
  %"%hc_idx.load42" = load i64, ptr %hc_idx1, align 8
  %calltmp43 = call i64 @uhci_set_status(i64 %"%hc_idx.load42", i64 2)
  %"%uhci_count.load44" = load i64, ptr @uhci_count, align 8
  %addtmp45 = add i64 %"%uhci_count.load44", 1
  store i64 %addtmp45, ptr %"%uhci_count", align 8
  %calltmp46 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.37 to i64))
  %calltmp47 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hc_idx.load48" = load i64, ptr %hc_idx1, align 8
  %calltmp49 = call i64 @uhci_scan_ports(i64 %"%hc_idx.load48")
  ret i64 1
}

define i64 @"uhci_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  %"%uhci_interrupts" = alloca i64, align 8
  %"%sts" = alloca i64, align 8
  %"%csc2" = alloca i64, align 8
  %"%sc2" = alloca i64, align 8
  %"%csc1" = alloca i64, align 8
  %"%sc1" = alloca i64, align 8
  %"%base" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_401

merge_405:                                        ; preds = %then_404, %while_body_402
  %"%i.load10" = load i64, ptr %"%i", align 8
  %"%UHCI_PORTSC2.load" = load i64, ptr @UHCI_PORTSC2, align 8
  %calltmp11 = call i64 @uhci_read16(i64 %"%i.load10", i64 %"%UHCI_PORTSC2.load")
  store i64 %calltmp11, ptr %"%sc2", align 8
  %"%sc2.load" = load i64, ptr %"%sc2", align 8
  %shrtmp12 = lshr i64 %"%sc2.load", 1
  %andtmp13 = and i64 %shrtmp12, 1
  store i64 %andtmp13, ptr %"%csc2", align 8
  %"%csc2.load" = load i64, ptr %"%csc2", align 8
  %cmpeqtmp14 = icmp eq i64 %"%csc2.load", 1
  br i1 %cmpeqtmp14, label %then_406, label %merge_407

merge_407:                                        ; preds = %then_406, %merge_405
  %"%i.load22" = load i64, ptr %"%i", align 8
  %"%UHCI_USBSTS.load" = load i64, ptr @UHCI_USBSTS, align 8
  %calltmp23 = call i64 @uhci_read16(i64 %"%i.load22", i64 %"%UHCI_USBSTS.load")
  store i64 %calltmp23, ptr %"%sts", align 8
  %"%sts.load" = load i64, ptr %"%sts", align 8
  %shrtmp24 = lshr i64 %"%sts.load", 1
  %andtmp25 = and i64 %shrtmp24, 1
  %cmpeqtmp26 = icmp eq i64 %andtmp25, 1
  br i1 %cmpeqtmp26, label %then_408, label %merge_409

merge_409:                                        ; preds = %then_408, %merge_407
  %"%sts.load33" = load i64, ptr %"%sts", align 8
  %"%UHCI_STS_HSE.load" = load i64, ptr @UHCI_STS_HSE, align 8
  %divtmp = sdiv i64 %"%sts.load33", %"%UHCI_STS_HSE.load"
  %andtmp34 = and i64 %divtmp, 1
  %cmpeqtmp35 = icmp eq i64 %andtmp34, 1
  br i1 %cmpeqtmp35, label %then_410, label %merge_411

merge_411:                                        ; preds = %then_410, %merge_409
  %"%i.load43" = load i64, ptr %"%i", align 8
  %addtmp44 = add i64 %"%i.load43", 1
  store i64 %addtmp44, ptr %"%i", align 8
  br label %while_cond_401

then_404:                                         ; preds = %while_body_402
  %calltmp4 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.38 to i64))
  %"%1844.load" = load i64, ptr %"%i", align 8
  %calltmp5 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1844.load")
  %calltmp6 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%i.load7" = load i64, ptr %"%i", align 8
  %"%UHCI_PORTSC1.load8" = load i64, ptr @UHCI_PORTSC1, align 8
  %"%UHCI_PORT_CSC.load" = load i64, ptr @UHCI_PORT_CSC, align 8
  %calltmp9 = call i64 @uhci_write16(i64 %"%i.load7", i64 %"%UHCI_PORTSC1.load8", i64 %"%UHCI_PORT_CSC.load")
  br label %merge_405

then_406:                                         ; preds = %merge_405
  %calltmp15 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.39 to i64))
  %"%1860.load" = load i64, ptr %"%i", align 8
  %calltmp16 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1860.load")
  %calltmp17 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%i.load18" = load i64, ptr %"%i", align 8
  %"%UHCI_PORTSC2.load19" = load i64, ptr @UHCI_PORTSC2, align 8
  %"%UHCI_PORT_CSC.load20" = load i64, ptr @UHCI_PORT_CSC, align 8
  %calltmp21 = call i64 @uhci_write16(i64 %"%i.load18", i64 %"%UHCI_PORTSC2.load19", i64 %"%UHCI_PORT_CSC.load20")
  br label %merge_407

then_408:                                         ; preds = %merge_407
  %calltmp27 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.40 to i64))
  %"%1876.load" = load i64, ptr %"%i", align 8
  %calltmp28 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1876.load")
  %calltmp29 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%uhci_interrupts.load" = load i64, ptr @uhci_interrupts, align 8
  %addtmp = add i64 %"%uhci_interrupts.load", 1
  store i64 %addtmp, ptr %"%uhci_interrupts", align 8
  %"%i.load30" = load i64, ptr %"%i", align 8
  %"%UHCI_USBSTS.load31" = load i64, ptr @UHCI_USBSTS, align 8
  %"%UHCI_STS_ERROR.load" = load i64, ptr @UHCI_STS_ERROR, align 8
  %calltmp32 = call i64 @uhci_write16(i64 %"%i.load30", i64 %"%UHCI_USBSTS.load31", i64 %"%UHCI_STS_ERROR.load")
  br label %merge_409

then_410:                                         ; preds = %merge_409
  %calltmp36 = call i64 @"uhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.41 to i64))
  %"%1890.load" = load i64, ptr %"%i", align 8
  %calltmp37 = call i64 @"uhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1890.load")
  %calltmp38 = call i64 @"uhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%i.load39" = load i64, ptr %"%i", align 8
  %"%UHCI_USBSTS.load40" = load i64, ptr @UHCI_USBSTS, align 8
  %"%UHCI_STS_HSE.load41" = load i64, ptr @UHCI_STS_HSE, align 8
  %calltmp42 = call i64 @uhci_write16(i64 %"%i.load39", i64 %"%UHCI_USBSTS.load40", i64 %"%UHCI_STS_HSE.load41")
  br label %merge_411

while_body_402:                                   ; preds = %while_cond_401
  %"%i.load1" = load i64, ptr %"%i", align 8
  %calltmp = call i64 @uhci_get_base(i64 %"%i.load1")
  store i64 %calltmp, ptr %"%base", align 8
  %"%i.load2" = load i64, ptr %"%i", align 8
  %"%UHCI_PORTSC1.load" = load i64, ptr @UHCI_PORTSC1, align 8
  %calltmp3 = call i64 @uhci_read16(i64 %"%i.load2", i64 %"%UHCI_PORTSC1.load")
  store i64 %calltmp3, ptr %"%sc1", align 8
  %"%sc1.load" = load i64, ptr %"%sc1", align 8
  %shrtmp = lshr i64 %"%sc1.load", 1
  %andtmp = and i64 %shrtmp, 1
  store i64 %andtmp, ptr %"%csc1", align 8
  %"%csc1.load" = load i64, ptr %"%csc1", align 8
  %cmpeqtmp = icmp eq i64 %"%csc1.load", 1
  br i1 %cmpeqtmp, label %then_404, label %merge_405

while_cond_401:                                   ; preds = %merge_411, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%uhci_count.load" = load i64, ptr @uhci_count, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%uhci_count.load"
  br i1 %cmplttmp, label %while_body_402, label %while_exit_403

while_exit_403:                                   ; preds = %while_cond_401
  ret i64 0
}

define void @__sad_main() {
entry:
  %"%uhci_interrupts" = alloca i64, align 8
  %"%uhci_port_resets" = alloca i64, align 8
  %"%uhci_transfer_errors" = alloca i64, align 8
  %"%uhci_total_transfers" = alloca i64, align 8
  %"%uhci_td_used_3" = alloca i64, align 8
  %"%uhci_td_used_2" = alloca i64, align 8
  %"%uhci_td_used_1" = alloca i64, align 8
  %"%uhci_td_used_0" = alloca i64, align 8
  %"%uhci_port2_speed_3" = alloca i64, align 8
  %"%uhci_port1_speed_3" = alloca i64, align 8
  %"%uhci_port2_speed_2" = alloca i64, align 8
  %"%uhci_port1_speed_2" = alloca i64, align 8
  %"%uhci_port2_speed_1" = alloca i64, align 8
  %"%uhci_port1_speed_1" = alloca i64, align 8
  %"%uhci_port2_speed_0" = alloca i64, align 8
  %"%uhci_port1_speed_0" = alloca i64, align 8
  %"%uhci_port2_connected_3" = alloca i64, align 8
  %"%uhci_port1_connected_3" = alloca i64, align 8
  %"%uhci_port2_connected_2" = alloca i64, align 8
  %"%uhci_port1_connected_2" = alloca i64, align 8
  %"%uhci_port2_connected_1" = alloca i64, align 8
  %"%uhci_port1_connected_1" = alloca i64, align 8
  %"%uhci_port2_connected_0" = alloca i64, align 8
  %"%uhci_port1_connected_0" = alloca i64, align 8
  %"%uhci_status_3" = alloca i64, align 8
  %"%uhci_status_2" = alloca i64, align 8
  %"%uhci_status_1" = alloca i64, align 8
  %"%uhci_status_0" = alloca i64, align 8
  %"%uhci_setup_buf_3" = alloca i64, align 8
  %"%uhci_setup_buf_2" = alloca i64, align 8
  %"%uhci_setup_buf_1" = alloca i64, align 8
  %"%uhci_setup_buf_0" = alloca i64, align 8
  %"%uhci_data_buf_3" = alloca i64, align 8
  %"%uhci_data_buf_2" = alloca i64, align 8
  %"%uhci_data_buf_1" = alloca i64, align 8
  %"%uhci_data_buf_0" = alloca i64, align 8
  %"%uhci_td_pool_3" = alloca i64, align 8
  %"%uhci_td_pool_2" = alloca i64, align 8
  %"%uhci_td_pool_1" = alloca i64, align 8
  %"%uhci_td_pool_0" = alloca i64, align 8
  %"%uhci_qh_pool_3" = alloca i64, align 8
  %"%uhci_qh_pool_2" = alloca i64, align 8
  %"%uhci_qh_pool_1" = alloca i64, align 8
  %"%uhci_qh_pool_0" = alloca i64, align 8
  %"%uhci_framelist_3" = alloca i64, align 8
  %"%uhci_framelist_2" = alloca i64, align 8
  %"%uhci_framelist_1" = alloca i64, align 8
  %"%uhci_framelist_0" = alloca i64, align 8
  %"%uhci_base_3" = alloca i64, align 8
  %"%uhci_base_2" = alloca i64, align 8
  %"%uhci_base_1" = alloca i64, align 8
  %"%uhci_base_0" = alloca i64, align 8
  %"%uhci_count" = alloca i64, align 8
  %"%UHCI_FRAME_COUNT" = alloca i64, align 8
  %"%UHCI_PID_OUT" = alloca i64, align 8
  %"%UHCI_PID_IN" = alloca i64, align 8
  %"%UHCI_PID_SETUP" = alloca i64, align 8
  %"%UHCI_TD_SPD" = alloca i64, align 8
  %"%UHCI_TD_LS" = alloca i64, align 8
  %"%UHCI_TD_IOS" = alloca i64, align 8
  %"%UHCI_TD_IOC" = alloca i64, align 8
  %"%UHCI_TD_BITSTUFF" = alloca i64, align 8
  %"%UHCI_TD_CRC_ERR" = alloca i64, align 8
  %"%UHCI_TD_NAK" = alloca i64, align 8
  %"%UHCI_TD_BABBLE" = alloca i64, align 8
  %"%UHCI_TD_DBUF_ERR" = alloca i64, align 8
  %"%UHCI_TD_STALLED" = alloca i64, align 8
  %"%UHCI_TD_ACTIVE" = alloca i64, align 8
  %"%UHCI_PORT_SUSP" = alloca i64, align 8
  %"%UHCI_PORT_RESET" = alloca i64, align 8
  %"%UHCI_PORT_LSDA" = alloca i64, align 8
  %"%UHCI_PORT_RD" = alloca i64, align 8
  %"%UHCI_PORT_LS_HI" = alloca i64, align 8
  %"%UHCI_PORT_LS_LO" = alloca i64, align 8
  %"%UHCI_PORT_ENC" = alloca i64, align 8
  %"%UHCI_PORT_ENABLE" = alloca i64, align 8
  %"%UHCI_PORT_CSC" = alloca i64, align 8
  %"%UHCI_PORT_CONNECT" = alloca i64, align 8
  %"%UHCI_STS_HCH" = alloca i64, align 8
  %"%UHCI_STS_HCPE" = alloca i64, align 8
  %"%UHCI_STS_HSE" = alloca i64, align 8
  %"%UHCI_STS_RD" = alloca i64, align 8
  %"%UHCI_STS_ERROR" = alloca i64, align 8
  %"%UHCI_STS_USBINT" = alloca i64, align 8
  %"%UHCI_CMD_MAXP" = alloca i64, align 8
  %"%UHCI_CMD_CF" = alloca i64, align 8
  %"%UHCI_CMD_SWDBG" = alloca i64, align 8
  %"%UHCI_CMD_FGR" = alloca i64, align 8
  %"%UHCI_CMD_EGSM" = alloca i64, align 8
  %"%UHCI_CMD_GRESET" = alloca i64, align 8
  %"%UHCI_CMD_HCRESET" = alloca i64, align 8
  %"%UHCI_CMD_RS" = alloca i64, align 8
  %"%UHCI_PORTSC2" = alloca i64, align 8
  %"%UHCI_PORTSC1" = alloca i64, align 8
  %"%UHCI_SOFMOD" = alloca i64, align 8
  %"%UHCI_FRBASEADD" = alloca i64, align 8
  %"%UHCI_FRNUM" = alloca i64, align 8
  %"%UHCI_USBINTR" = alloca i64, align 8
  %"%UHCI_USBSTS" = alloca i64, align 8
  %"%UHCI_USBCMD" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 0, ptr %"%UHCI_USBCMD", align 8
  store i64 2, ptr %"%UHCI_USBSTS", align 8
  store i64 4, ptr %"%UHCI_USBINTR", align 8
  store i64 6, ptr %"%UHCI_FRNUM", align 8
  store i64 8, ptr %"%UHCI_FRBASEADD", align 8
  store i64 12, ptr %"%UHCI_SOFMOD", align 8
  store i64 16, ptr %"%UHCI_PORTSC1", align 8
  store i64 18, ptr %"%UHCI_PORTSC2", align 8
  store i64 1, ptr %"%UHCI_CMD_RS", align 8
  store i64 2, ptr %"%UHCI_CMD_HCRESET", align 8
  store i64 4, ptr %"%UHCI_CMD_GRESET", align 8
  store i64 8, ptr %"%UHCI_CMD_EGSM", align 8
  store i64 16, ptr %"%UHCI_CMD_FGR", align 8
  store i64 32, ptr %"%UHCI_CMD_SWDBG", align 8
  store i64 64, ptr %"%UHCI_CMD_CF", align 8
  store i64 128, ptr %"%UHCI_CMD_MAXP", align 8
  store i64 1, ptr %"%UHCI_STS_USBINT", align 8
  store i64 2, ptr %"%UHCI_STS_ERROR", align 8
  store i64 4, ptr %"%UHCI_STS_RD", align 8
  store i64 8, ptr %"%UHCI_STS_HSE", align 8
  store i64 16, ptr %"%UHCI_STS_HCPE", align 8
  store i64 32, ptr %"%UHCI_STS_HCH", align 8
  store i64 1, ptr %"%UHCI_PORT_CONNECT", align 8
  store i64 2, ptr %"%UHCI_PORT_CSC", align 8
  store i64 4, ptr %"%UHCI_PORT_ENABLE", align 8
  store i64 8, ptr %"%UHCI_PORT_ENC", align 8
  store i64 16, ptr %"%UHCI_PORT_LS_LO", align 8
  store i64 32, ptr %"%UHCI_PORT_LS_HI", align 8
  store i64 64, ptr %"%UHCI_PORT_RD", align 8
  store i64 256, ptr %"%UHCI_PORT_LSDA", align 8
  store i64 512, ptr %"%UHCI_PORT_RESET", align 8
  store i64 4096, ptr %"%UHCI_PORT_SUSP", align 8
  store i64 8388608, ptr %"%UHCI_TD_ACTIVE", align 8
  store i64 4194304, ptr %"%UHCI_TD_STALLED", align 8
  store i64 2097152, ptr %"%UHCI_TD_DBUF_ERR", align 8
  store i64 1048576, ptr %"%UHCI_TD_BABBLE", align 8
  store i64 524288, ptr %"%UHCI_TD_NAK", align 8
  store i64 262144, ptr %"%UHCI_TD_CRC_ERR", align 8
  store i64 131072, ptr %"%UHCI_TD_BITSTUFF", align 8
  store i64 16777216, ptr %"%UHCI_TD_IOC", align 8
  store i64 33554432, ptr %"%UHCI_TD_IOS", align 8
  store i64 67108864, ptr %"%UHCI_TD_LS", align 8
  store i64 536870912, ptr %"%UHCI_TD_SPD", align 8
  store i64 45, ptr %"%UHCI_PID_SETUP", align 8
  store i64 105, ptr %"%UHCI_PID_IN", align 8
  store i64 225, ptr %"%UHCI_PID_OUT", align 8
  store i64 1024, ptr %"%UHCI_FRAME_COUNT", align 8
  store i64 0, ptr %"%uhci_count", align 8
  store i64 0, ptr %"%uhci_base_0", align 8
  store i64 0, ptr %"%uhci_base_1", align 8
  store i64 0, ptr %"%uhci_base_2", align 8
  store i64 0, ptr %"%uhci_base_3", align 8
  store i64 0, ptr %"%uhci_framelist_0", align 8
  store i64 0, ptr %"%uhci_framelist_1", align 8
  store i64 0, ptr %"%uhci_framelist_2", align 8
  store i64 0, ptr %"%uhci_framelist_3", align 8
  store i64 0, ptr %"%uhci_qh_pool_0", align 8
  store i64 0, ptr %"%uhci_qh_pool_1", align 8
  store i64 0, ptr %"%uhci_qh_pool_2", align 8
  store i64 0, ptr %"%uhci_qh_pool_3", align 8
  store i64 0, ptr %"%uhci_td_pool_0", align 8
  store i64 0, ptr %"%uhci_td_pool_1", align 8
  store i64 0, ptr %"%uhci_td_pool_2", align 8
  store i64 0, ptr %"%uhci_td_pool_3", align 8
  store i64 0, ptr %"%uhci_data_buf_0", align 8
  store i64 0, ptr %"%uhci_data_buf_1", align 8
  store i64 0, ptr %"%uhci_data_buf_2", align 8
  store i64 0, ptr %"%uhci_data_buf_3", align 8
  store i64 0, ptr %"%uhci_setup_buf_0", align 8
  store i64 0, ptr %"%uhci_setup_buf_1", align 8
  store i64 0, ptr %"%uhci_setup_buf_2", align 8
  store i64 0, ptr %"%uhci_setup_buf_3", align 8
  store i64 0, ptr %"%uhci_status_0", align 8
  store i64 0, ptr %"%uhci_status_1", align 8
  store i64 0, ptr %"%uhci_status_2", align 8
  store i64 0, ptr %"%uhci_status_3", align 8
  store i64 0, ptr %"%uhci_port1_connected_0", align 8
  store i64 0, ptr %"%uhci_port2_connected_0", align 8
  store i64 0, ptr %"%uhci_port1_connected_1", align 8
  store i64 0, ptr %"%uhci_port2_connected_1", align 8
  store i64 0, ptr %"%uhci_port1_connected_2", align 8
  store i64 0, ptr %"%uhci_port2_connected_2", align 8
  store i64 0, ptr %"%uhci_port1_connected_3", align 8
  store i64 0, ptr %"%uhci_port2_connected_3", align 8
  store i64 1, ptr %"%uhci_port1_speed_0", align 8
  store i64 1, ptr %"%uhci_port2_speed_0", align 8
  store i64 1, ptr %"%uhci_port1_speed_1", align 8
  store i64 1, ptr %"%uhci_port2_speed_1", align 8
  store i64 1, ptr %"%uhci_port1_speed_2", align 8
  store i64 1, ptr %"%uhci_port2_speed_2", align 8
  store i64 1, ptr %"%uhci_port1_speed_3", align 8
  store i64 1, ptr %"%uhci_port2_speed_3", align 8
  store i64 0, ptr %"%uhci_td_used_0", align 8
  store i64 0, ptr %"%uhci_td_used_1", align 8
  store i64 0, ptr %"%uhci_td_used_2", align 8
  store i64 0, ptr %"%uhci_td_used_3", align 8
  store i64 0, ptr %"%uhci_total_transfers", align 8
  store i64 0, ptr %"%uhci_transfer_errors", align 8
  store i64 0, ptr %"%uhci_port_resets", align 8
  store i64 0, ptr %"%uhci_interrupts", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
