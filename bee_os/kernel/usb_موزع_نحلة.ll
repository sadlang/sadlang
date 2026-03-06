; ModuleID = 'usb_موزع_نحلة'
source_filename = "usb_\D9\85\D9\88\D8\B2\D8\B9_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@COM1 = constant i64 1016
@MAX_HUBS = constant i64 8
@MAX_PORTS_PER_HUB = constant i64 8
@HUB_REQ_TYPE_GET = constant i64 160
@HUB_REQ_TYPE_SET = constant i64 32
@PORT_REQ_TYPE_GET = constant i64 163
@PORT_REQ_TYPE_SET = constant i64 35
@PORT_REQ_TYPE_CLR = constant i64 35
@GET_STATUS = constant i64 0
@CLEAR_FEATURE = constant i64 1
@SET_FEATURE = constant i64 3
@GET_DESCRIPTOR = constant i64 6
@C_HUB_LOCAL_POWER = constant i64 0
@C_HUB_OVER_CURRENT = constant i64 1
@PORT_CONNECTION = constant i64 0
@PORT_ENABLE = constant i64 1
@PORT_SUSPEND = constant i64 2
@PORT_OVER_CURRENT = constant i64 3
@PORT_RESET = constant i64 4
@PORT_POWER = constant i64 8
@PORT_LOW_SPEED = constant i64 9
@C_PORT_CONNECTION = constant i64 16
@C_PORT_ENABLE = constant i64 17
@C_PORT_SUSPEND = constant i64 18
@C_PORT_OVER_CURRENT = constant i64 19
@C_PORT_RESET = constant i64 20
@PORT_STATUS_CONNECTION = constant i64 1
@PORT_STATUS_ENABLED = constant i64 2
@PORT_STATUS_SUSPENDED = constant i64 4
@PORT_STATUS_OVERCURRENT = constant i64 8
@PORT_STATUS_RESET = constant i64 16
@PORT_STATUS_POWERED = constant i64 256
@PORT_STATUS_LOW_SPEED = constant i64 512
@PORT_STATUS_HIGH_SPEED = constant i64 1024
@PORT_CHANGE_CONNECTION = constant i64 1
@PORT_CHANGE_ENABLE = constant i64 2
@PORT_CHANGE_SUSPEND = constant i64 4
@PORT_CHANGE_OVERCURRENT = constant i64 8
@PORT_CHANGE_RESET = constant i64 16
@HUB_DESC_TYPE = constant i64 41
@hub_active_0 = global i64 0
@hub_active_1 = global i64 0
@hub_active_2 = global i64 0
@hub_active_3 = global i64 0
@hub_active_4 = global i64 0
@hub_active_5 = global i64 0
@hub_active_6 = global i64 0
@hub_active_7 = global i64 0
@hub_hc_0 = global i64 0
@hub_hc_1 = global i64 0
@hub_hc_2 = global i64 0
@hub_hc_3 = global i64 0
@hub_hc_4 = global i64 0
@hub_hc_5 = global i64 0
@hub_hc_6 = global i64 0
@hub_hc_7 = global i64 0
@hub_addr_0 = global i64 0
@hub_addr_1 = global i64 0
@hub_addr_2 = global i64 0
@hub_addr_3 = global i64 0
@hub_addr_4 = global i64 0
@hub_addr_5 = global i64 0
@hub_addr_6 = global i64 0
@hub_addr_7 = global i64 0
@hub_num_ports_0 = global i64 0
@hub_num_ports_1 = global i64 0
@hub_num_ports_2 = global i64 0
@hub_num_ports_3 = global i64 0
@hub_num_ports_4 = global i64 0
@hub_num_ports_5 = global i64 0
@hub_num_ports_6 = global i64 0
@hub_num_ports_7 = global i64 0
@hub_pwr_delay_0 = global i64 0
@hub_pwr_delay_1 = global i64 0
@hub_pwr_delay_2 = global i64 0
@hub_pwr_delay_3 = global i64 0
@hub_pwr_delay_4 = global i64 0
@hub_pwr_delay_5 = global i64 0
@hub_pwr_delay_6 = global i64 0
@hub_pwr_delay_7 = global i64 0
@hub_sc_ep_0 = global i64 1
@hub_sc_ep_1 = global i64 1
@hub_sc_ep_2 = global i64 1
@hub_sc_ep_3 = global i64 1
@hub_sc_ep_4 = global i64 1
@hub_sc_ep_5 = global i64 1
@hub_sc_ep_6 = global i64 1
@hub_sc_ep_7 = global i64 1
@port_dev_00 = global i64 0
@port_dev_01 = global i64 0
@port_dev_02 = global i64 0
@port_dev_03 = global i64 0
@port_dev_04 = global i64 0
@port_dev_05 = global i64 0
@port_dev_06 = global i64 0
@port_dev_07 = global i64 0
@port_dev_10 = global i64 0
@port_dev_11 = global i64 0
@port_dev_12 = global i64 0
@port_dev_13 = global i64 0
@port_dev_14 = global i64 0
@port_dev_15 = global i64 0
@port_dev_16 = global i64 0
@port_dev_17 = global i64 0
@port_dev_20 = global i64 0
@port_dev_21 = global i64 0
@port_dev_22 = global i64 0
@port_dev_23 = global i64 0
@port_dev_24 = global i64 0
@port_dev_25 = global i64 0
@port_dev_26 = global i64 0
@port_dev_27 = global i64 0
@port_dev_30 = global i64 0
@port_dev_31 = global i64 0
@port_dev_32 = global i64 0
@port_dev_33 = global i64 0
@port_dev_34 = global i64 0
@port_dev_35 = global i64 0
@port_dev_36 = global i64 0
@port_dev_37 = global i64 0
@hub_count = global i64 0
@hub_setup_buf = global i64 0
@hub_data_buf = global i64 0
@hub_attach_count = global i64 0
@hub_detach_count = global i64 0
@hub_poll_count = global i64 0
@hub_errors = global i64 0
@str.const = private unnamed_addr constant [12 x i8] c"[HUB] Hub #\00", align 1
@str.const.1 = private unnamed_addr constant [8 x i8] c" ports=\00", align 1
@str.const.2 = private unnamed_addr constant [12 x i8] c" pwr_delay=\00", align 1
@str.const.3 = private unnamed_addr constant [3 x i8] c"ms\00", align 1
@str.const.4 = private unnamed_addr constant [19 x i8] c"[HUB] Powering on \00", align 1
@str.const.5 = private unnamed_addr constant [16 x i8] c" ports on hub #\00", align 1
@str.const.6 = private unnamed_addr constant [27 x i8] c"[HUB] All ports powered on\00", align 1
@str.const.7 = private unnamed_addr constant [22 x i8] c"[HUB] Resetting port \00", align 1
@str.const.8 = private unnamed_addr constant [10 x i8] c" on hub #\00", align 1
@str.const.9 = private unnamed_addr constant [25 x i8] c"[HUB] Port reset timeout\00", align 1
@str.const.10 = private unnamed_addr constant [12 x i8] c"[HUB] Port \00", align 1
@str.const.11 = private unnamed_addr constant [16 x i8] c" reset complete\00", align 1
@str.const.12 = private unnamed_addr constant [28 x i8] c"[HUB] Device attached hub #\00", align 1
@str.const.13 = private unnamed_addr constant [7 x i8] c" port \00", align 1
@str.const.14 = private unnamed_addr constant [26 x i8] c"[HUB] Registered device #\00", align 1
@str.const.15 = private unnamed_addr constant [8 x i8] c" speed=\00", align 1
@str.const.16 = private unnamed_addr constant [32 x i8] c"[HUB] Failed to register device\00", align 1
@str.const.17 = private unnamed_addr constant [28 x i8] c"[HUB] Device detached hub #\00", align 1
@str.const.19 = private unnamed_addr constant [27 x i8] c"[HUB] Overcurrent on port \00", align 1
@str.const.20 = private unnamed_addr constant [23 x i8] c"[HUB] Max hubs reached\00", align 1
@str.const.21 = private unnamed_addr constant [23 x i8] c"[HUB] Registered hub #\00", align 1
@str.const.22 = private unnamed_addr constant [7 x i8] c" addr=\00", align 1
@str.const.23 = private unnamed_addr constant [54 x i8] c"\E2\95\90\E2\95\90\E2\95\90 [HUB] \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D9\85\D9\88\D8\B2\D8\B9\D8\A7\D8\AA \E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.24 = private unnamed_addr constant [28 x i8] c"  \D9\85\D9\88\D8\B2\D8\B9\D8\A7\D8\AA \D9\85\D8\B3\D8\AC\D9\84\D8\A9: \00", align 1
@str.const.25 = private unnamed_addr constant [26 x i8] c"  \D8\A3\D8\AC\D9\87\D8\B2\D8\A9 \D9\85\D8\AA\D8\B5\D9\84\D8\A9: \00", align 1
@str.const.26 = private unnamed_addr constant [17 x i8] c"  \D9\85\D9\81\D8\B5\D9\88\D9\84\D8\A9: \00", align 1
@str.const.27 = private unnamed_addr constant [23 x i8] c"  \D8\A7\D8\B3\D8\AA\D9\82\D8\B5\D8\A7\D8\A1\D8\A7\D8\AA: \00", align 1
@str.const.28 = private unnamed_addr constant [15 x i8] c"  \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1: \00", align 1
@str.const.29 = private unnamed_addr constant [106 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.30 = private unnamed_addr constant [8 x i8] c"  Hub #\00", align 1
@str.const.33 = private unnamed_addr constant [10 x i8] c"    Port \00", align 1
@str.const.34 = private unnamed_addr constant [11 x i8] c": device #\00", align 1
@str.const.35 = private unnamed_addr constant [52 x i8] c"[HUB] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D8\AA\D8\B9\D8\B1\D9\8A\D9\81\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D9\88\D8\B2\D8\B9\D8\A7\D8\AA...\00", align 1
@str.const.36 = private unnamed_addr constant [15 x i8] c"[HUB] \D8\AC\D8\A7\D9\87\D8\B2\00", align 1

declare void @sad_ll_mem_read8(i64)

declare void @sad_ll_mem_write8(i64, i64)

declare void @sad_ll_mem_read32(i64)

declare void @sad_ll_mem_write32(i64, i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_kmalloc(i64)

declare void @sad_ll_memset(i64, i64, i64)

declare void @sad_ll_memcpy(i64, i64, i64)

declare void @sad_ll_timer_get_ticks()

declare void @uhci_control_transfer(i64, i64, i64, i64, i64)

declare void @uhci_interrupt_transfer(i64, i64, i64, i64, i64)

declare void @usb_build_setup(i64, i64, i64, i64, i64, i64)

declare void @usb_register_device(i64, i64, i64, i64, i64, i64, i64)

define i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
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

define i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
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

define i64 @hub_serial_hex8(i64 %val) {
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

define i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"() {
entry:
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 13)
  %"%COM1.load1" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load1", i64 10)
  ret i64 0
}

define i64 @hub_get_active(i64 %idx) {
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
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_38, label %merge_39

merge_39:                                         ; preds = %merge_37
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_40, label %merge_41

merge_41:                                         ; preds = %merge_39
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_42, label %merge_43

merge_43:                                         ; preds = %merge_41
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_44, label %merge_45

merge_45:                                         ; preds = %merge_43
  ret i64 0

then_30:                                          ; preds = %entry
  %loadtmp = load i64, ptr @hub_active_0, align 8
  ret i64 %loadtmp

then_32:                                          ; preds = %merge_31
  %loadtmp4 = load i64, ptr @hub_active_1, align 8
  ret i64 %loadtmp4

then_34:                                          ; preds = %merge_33
  %loadtmp7 = load i64, ptr @hub_active_2, align 8
  ret i64 %loadtmp7

then_36:                                          ; preds = %merge_35
  %loadtmp10 = load i64, ptr @hub_active_3, align 8
  ret i64 %loadtmp10

then_38:                                          ; preds = %merge_37
  %loadtmp13 = load i64, ptr @hub_active_4, align 8
  ret i64 %loadtmp13

then_40:                                          ; preds = %merge_39
  %loadtmp16 = load i64, ptr @hub_active_5, align 8
  ret i64 %loadtmp16

then_42:                                          ; preds = %merge_41
  %loadtmp19 = load i64, ptr @hub_active_6, align 8
  ret i64 %loadtmp19

then_44:                                          ; preds = %merge_43
  %loadtmp22 = load i64, ptr @hub_active_7, align 8
  ret i64 %loadtmp22
}

define i64 @hub_set_active(i64 %idx, i64 %val) {
entry:
  %"%hub_active_7" = alloca i64, align 8
  %"%hub_active_6" = alloca i64, align 8
  %"%hub_active_5" = alloca i64, align 8
  %"%hub_active_4" = alloca i64, align 8
  %"%hub_active_3" = alloca i64, align 8
  %"%hub_active_2" = alloca i64, align 8
  %"%hub_active_1" = alloca i64, align 8
  %"%hub_active_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_46, label %merge_47

merge_47:                                         ; preds = %then_46, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_48, label %merge_49

merge_49:                                         ; preds = %then_48, %merge_47
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_50, label %merge_51

merge_51:                                         ; preds = %then_50, %merge_49
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_52, label %merge_53

merge_53:                                         ; preds = %then_52, %merge_51
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_54, label %merge_55

merge_55:                                         ; preds = %then_54, %merge_53
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_56, label %merge_57

merge_57:                                         ; preds = %then_56, %merge_55
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_58, label %merge_59

merge_59:                                         ; preds = %then_58, %merge_57
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_60, label %merge_61

merge_61:                                         ; preds = %then_60, %merge_59
  ret i64 0

then_46:                                          ; preds = %entry
  %"%150.load" = load i64, ptr %val2, align 8
  store i64 %"%150.load", ptr %"%hub_active_0", align 8
  br label %merge_47

then_48:                                          ; preds = %merge_47
  %"%154.load" = load i64, ptr %val2, align 8
  store i64 %"%154.load", ptr %"%hub_active_1", align 8
  br label %merge_49

then_50:                                          ; preds = %merge_49
  %"%158.load" = load i64, ptr %val2, align 8
  store i64 %"%158.load", ptr %"%hub_active_2", align 8
  br label %merge_51

then_52:                                          ; preds = %merge_51
  %"%162.load" = load i64, ptr %val2, align 8
  store i64 %"%162.load", ptr %"%hub_active_3", align 8
  br label %merge_53

then_54:                                          ; preds = %merge_53
  %"%166.load" = load i64, ptr %val2, align 8
  store i64 %"%166.load", ptr %"%hub_active_4", align 8
  br label %merge_55

then_56:                                          ; preds = %merge_55
  %"%170.load" = load i64, ptr %val2, align 8
  store i64 %"%170.load", ptr %"%hub_active_5", align 8
  br label %merge_57

then_58:                                          ; preds = %merge_57
  %"%174.load" = load i64, ptr %val2, align 8
  store i64 %"%174.load", ptr %"%hub_active_6", align 8
  br label %merge_59

then_60:                                          ; preds = %merge_59
  %"%178.load" = load i64, ptr %val2, align 8
  store i64 %"%178.load", ptr %"%hub_active_7", align 8
  br label %merge_61
}

define i64 @hub_get_hc(i64 %idx) {
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
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_70, label %merge_71

merge_71:                                         ; preds = %merge_69
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_72, label %merge_73

merge_73:                                         ; preds = %merge_71
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_74, label %merge_75

merge_75:                                         ; preds = %merge_73
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_76, label %merge_77

merge_77:                                         ; preds = %merge_75
  ret i64 0

then_62:                                          ; preds = %entry
  %loadtmp = load i64, ptr @hub_hc_0, align 8
  ret i64 %loadtmp

then_64:                                          ; preds = %merge_63
  %loadtmp4 = load i64, ptr @hub_hc_1, align 8
  ret i64 %loadtmp4

then_66:                                          ; preds = %merge_65
  %loadtmp7 = load i64, ptr @hub_hc_2, align 8
  ret i64 %loadtmp7

then_68:                                          ; preds = %merge_67
  %loadtmp10 = load i64, ptr @hub_hc_3, align 8
  ret i64 %loadtmp10

then_70:                                          ; preds = %merge_69
  %loadtmp13 = load i64, ptr @hub_hc_4, align 8
  ret i64 %loadtmp13

then_72:                                          ; preds = %merge_71
  %loadtmp16 = load i64, ptr @hub_hc_5, align 8
  ret i64 %loadtmp16

then_74:                                          ; preds = %merge_73
  %loadtmp19 = load i64, ptr @hub_hc_6, align 8
  ret i64 %loadtmp19

then_76:                                          ; preds = %merge_75
  %loadtmp22 = load i64, ptr @hub_hc_7, align 8
  ret i64 %loadtmp22
}

define i64 @hub_set_hc(i64 %idx, i64 %val) {
entry:
  %"%hub_hc_7" = alloca i64, align 8
  %"%hub_hc_6" = alloca i64, align 8
  %"%hub_hc_5" = alloca i64, align 8
  %"%hub_hc_4" = alloca i64, align 8
  %"%hub_hc_3" = alloca i64, align 8
  %"%hub_hc_2" = alloca i64, align 8
  %"%hub_hc_1" = alloca i64, align 8
  %"%hub_hc_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_78, label %merge_79

merge_79:                                         ; preds = %then_78, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_80, label %merge_81

merge_81:                                         ; preds = %then_80, %merge_79
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_82, label %merge_83

merge_83:                                         ; preds = %then_82, %merge_81
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_84, label %merge_85

merge_85:                                         ; preds = %then_84, %merge_83
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_86, label %merge_87

merge_87:                                         ; preds = %then_86, %merge_85
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_88, label %merge_89

merge_89:                                         ; preds = %then_88, %merge_87
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_90, label %merge_91

merge_91:                                         ; preds = %then_90, %merge_89
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_92, label %merge_93

merge_93:                                         ; preds = %then_92, %merge_91
  ret i64 0

then_78:                                          ; preds = %entry
  %"%224.load" = load i64, ptr %val2, align 8
  store i64 %"%224.load", ptr %"%hub_hc_0", align 8
  br label %merge_79

then_80:                                          ; preds = %merge_79
  %"%228.load" = load i64, ptr %val2, align 8
  store i64 %"%228.load", ptr %"%hub_hc_1", align 8
  br label %merge_81

then_82:                                          ; preds = %merge_81
  %"%232.load" = load i64, ptr %val2, align 8
  store i64 %"%232.load", ptr %"%hub_hc_2", align 8
  br label %merge_83

then_84:                                          ; preds = %merge_83
  %"%236.load" = load i64, ptr %val2, align 8
  store i64 %"%236.load", ptr %"%hub_hc_3", align 8
  br label %merge_85

then_86:                                          ; preds = %merge_85
  %"%240.load" = load i64, ptr %val2, align 8
  store i64 %"%240.load", ptr %"%hub_hc_4", align 8
  br label %merge_87

then_88:                                          ; preds = %merge_87
  %"%244.load" = load i64, ptr %val2, align 8
  store i64 %"%244.load", ptr %"%hub_hc_5", align 8
  br label %merge_89

then_90:                                          ; preds = %merge_89
  %"%248.load" = load i64, ptr %val2, align 8
  store i64 %"%248.load", ptr %"%hub_hc_6", align 8
  br label %merge_91

then_92:                                          ; preds = %merge_91
  %"%252.load" = load i64, ptr %val2, align 8
  store i64 %"%252.load", ptr %"%hub_hc_7", align 8
  br label %merge_93
}

define i64 @hub_get_addr(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_94, label %merge_95

merge_101:                                        ; preds = %merge_99
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_102, label %merge_103

merge_103:                                        ; preds = %merge_101
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_104, label %merge_105

merge_105:                                        ; preds = %merge_103
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_106, label %merge_107

merge_107:                                        ; preds = %merge_105
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_108, label %merge_109

merge_109:                                        ; preds = %merge_107
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
  %loadtmp10 = load i64, ptr @hub_addr_3, align 8
  ret i64 %loadtmp10

then_102:                                         ; preds = %merge_101
  %loadtmp13 = load i64, ptr @hub_addr_4, align 8
  ret i64 %loadtmp13

then_104:                                         ; preds = %merge_103
  %loadtmp16 = load i64, ptr @hub_addr_5, align 8
  ret i64 %loadtmp16

then_106:                                         ; preds = %merge_105
  %loadtmp19 = load i64, ptr @hub_addr_6, align 8
  ret i64 %loadtmp19

then_108:                                         ; preds = %merge_107
  %loadtmp22 = load i64, ptr @hub_addr_7, align 8
  ret i64 %loadtmp22

then_94:                                          ; preds = %entry
  %loadtmp = load i64, ptr @hub_addr_0, align 8
  ret i64 %loadtmp

then_96:                                          ; preds = %merge_95
  %loadtmp4 = load i64, ptr @hub_addr_1, align 8
  ret i64 %loadtmp4

then_98:                                          ; preds = %merge_97
  %loadtmp7 = load i64, ptr @hub_addr_2, align 8
  ret i64 %loadtmp7
}

define i64 @hub_set_addr(i64 %idx, i64 %val) {
entry:
  %"%hub_addr_7" = alloca i64, align 8
  %"%hub_addr_6" = alloca i64, align 8
  %"%hub_addr_5" = alloca i64, align 8
  %"%hub_addr_4" = alloca i64, align 8
  %"%hub_addr_3" = alloca i64, align 8
  %"%hub_addr_2" = alloca i64, align 8
  %"%hub_addr_1" = alloca i64, align 8
  %"%hub_addr_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_110, label %merge_111

merge_111:                                        ; preds = %then_110, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_112, label %merge_113

merge_113:                                        ; preds = %then_112, %merge_111
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_114, label %merge_115

merge_115:                                        ; preds = %then_114, %merge_113
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_116, label %merge_117

merge_117:                                        ; preds = %then_116, %merge_115
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_118, label %merge_119

merge_119:                                        ; preds = %then_118, %merge_117
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_120, label %merge_121

merge_121:                                        ; preds = %then_120, %merge_119
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_122, label %merge_123

merge_123:                                        ; preds = %then_122, %merge_121
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_124, label %merge_125

merge_125:                                        ; preds = %then_124, %merge_123
  ret i64 0

then_110:                                         ; preds = %entry
  %"%298.load" = load i64, ptr %val2, align 8
  store i64 %"%298.load", ptr %"%hub_addr_0", align 8
  br label %merge_111

then_112:                                         ; preds = %merge_111
  %"%302.load" = load i64, ptr %val2, align 8
  store i64 %"%302.load", ptr %"%hub_addr_1", align 8
  br label %merge_113

then_114:                                         ; preds = %merge_113
  %"%306.load" = load i64, ptr %val2, align 8
  store i64 %"%306.load", ptr %"%hub_addr_2", align 8
  br label %merge_115

then_116:                                         ; preds = %merge_115
  %"%310.load" = load i64, ptr %val2, align 8
  store i64 %"%310.load", ptr %"%hub_addr_3", align 8
  br label %merge_117

then_118:                                         ; preds = %merge_117
  %"%314.load" = load i64, ptr %val2, align 8
  store i64 %"%314.load", ptr %"%hub_addr_4", align 8
  br label %merge_119

then_120:                                         ; preds = %merge_119
  %"%318.load" = load i64, ptr %val2, align 8
  store i64 %"%318.load", ptr %"%hub_addr_5", align 8
  br label %merge_121

then_122:                                         ; preds = %merge_121
  %"%322.load" = load i64, ptr %val2, align 8
  store i64 %"%322.load", ptr %"%hub_addr_6", align 8
  br label %merge_123

then_124:                                         ; preds = %merge_123
  %"%326.load" = load i64, ptr %val2, align 8
  store i64 %"%326.load", ptr %"%hub_addr_7", align 8
  br label %merge_125
}

define i64 @hub_get_num_ports(i64 %idx) {
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
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_134, label %merge_135

merge_135:                                        ; preds = %merge_133
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_136, label %merge_137

merge_137:                                        ; preds = %merge_135
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_138, label %merge_139

merge_139:                                        ; preds = %merge_137
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_140, label %merge_141

merge_141:                                        ; preds = %merge_139
  ret i64 0

then_126:                                         ; preds = %entry
  %loadtmp = load i64, ptr @hub_num_ports_0, align 8
  ret i64 %loadtmp

then_128:                                         ; preds = %merge_127
  %loadtmp4 = load i64, ptr @hub_num_ports_1, align 8
  ret i64 %loadtmp4

then_130:                                         ; preds = %merge_129
  %loadtmp7 = load i64, ptr @hub_num_ports_2, align 8
  ret i64 %loadtmp7

then_132:                                         ; preds = %merge_131
  %loadtmp10 = load i64, ptr @hub_num_ports_3, align 8
  ret i64 %loadtmp10

then_134:                                         ; preds = %merge_133
  %loadtmp13 = load i64, ptr @hub_num_ports_4, align 8
  ret i64 %loadtmp13

then_136:                                         ; preds = %merge_135
  %loadtmp16 = load i64, ptr @hub_num_ports_5, align 8
  ret i64 %loadtmp16

then_138:                                         ; preds = %merge_137
  %loadtmp19 = load i64, ptr @hub_num_ports_6, align 8
  ret i64 %loadtmp19

then_140:                                         ; preds = %merge_139
  %loadtmp22 = load i64, ptr @hub_num_ports_7, align 8
  ret i64 %loadtmp22
}

define i64 @hub_set_num_ports(i64 %idx, i64 %val) {
entry:
  %"%hub_num_ports_7" = alloca i64, align 8
  %"%hub_num_ports_6" = alloca i64, align 8
  %"%hub_num_ports_5" = alloca i64, align 8
  %"%hub_num_ports_4" = alloca i64, align 8
  %"%hub_num_ports_3" = alloca i64, align 8
  %"%hub_num_ports_2" = alloca i64, align 8
  %"%hub_num_ports_1" = alloca i64, align 8
  %"%hub_num_ports_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_142, label %merge_143

merge_143:                                        ; preds = %then_142, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_144, label %merge_145

merge_145:                                        ; preds = %then_144, %merge_143
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_146, label %merge_147

merge_147:                                        ; preds = %then_146, %merge_145
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_148, label %merge_149

merge_149:                                        ; preds = %then_148, %merge_147
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_150, label %merge_151

merge_151:                                        ; preds = %then_150, %merge_149
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_152, label %merge_153

merge_153:                                        ; preds = %then_152, %merge_151
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_154, label %merge_155

merge_155:                                        ; preds = %then_154, %merge_153
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_156, label %merge_157

merge_157:                                        ; preds = %then_156, %merge_155
  ret i64 0

then_142:                                         ; preds = %entry
  %"%372.load" = load i64, ptr %val2, align 8
  store i64 %"%372.load", ptr %"%hub_num_ports_0", align 8
  br label %merge_143

then_144:                                         ; preds = %merge_143
  %"%376.load" = load i64, ptr %val2, align 8
  store i64 %"%376.load", ptr %"%hub_num_ports_1", align 8
  br label %merge_145

then_146:                                         ; preds = %merge_145
  %"%380.load" = load i64, ptr %val2, align 8
  store i64 %"%380.load", ptr %"%hub_num_ports_2", align 8
  br label %merge_147

then_148:                                         ; preds = %merge_147
  %"%384.load" = load i64, ptr %val2, align 8
  store i64 %"%384.load", ptr %"%hub_num_ports_3", align 8
  br label %merge_149

then_150:                                         ; preds = %merge_149
  %"%388.load" = load i64, ptr %val2, align 8
  store i64 %"%388.load", ptr %"%hub_num_ports_4", align 8
  br label %merge_151

then_152:                                         ; preds = %merge_151
  %"%392.load" = load i64, ptr %val2, align 8
  store i64 %"%392.load", ptr %"%hub_num_ports_5", align 8
  br label %merge_153

then_154:                                         ; preds = %merge_153
  %"%396.load" = load i64, ptr %val2, align 8
  store i64 %"%396.load", ptr %"%hub_num_ports_6", align 8
  br label %merge_155

then_156:                                         ; preds = %merge_155
  %"%400.load" = load i64, ptr %val2, align 8
  store i64 %"%400.load", ptr %"%hub_num_ports_7", align 8
  br label %merge_157
}

define i64 @hub_get_pwr_delay(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_158, label %merge_159

merge_159:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_160, label %merge_161

merge_161:                                        ; preds = %merge_159
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_162, label %merge_163

merge_163:                                        ; preds = %merge_161
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_164, label %merge_165

merge_165:                                        ; preds = %merge_163
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_166, label %merge_167

merge_167:                                        ; preds = %merge_165
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_168, label %merge_169

merge_169:                                        ; preds = %merge_167
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_170, label %merge_171

merge_171:                                        ; preds = %merge_169
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_172, label %merge_173

merge_173:                                        ; preds = %merge_171
  ret i64 0

then_158:                                         ; preds = %entry
  %loadtmp = load i64, ptr @hub_pwr_delay_0, align 8
  ret i64 %loadtmp

then_160:                                         ; preds = %merge_159
  %loadtmp4 = load i64, ptr @hub_pwr_delay_1, align 8
  ret i64 %loadtmp4

then_162:                                         ; preds = %merge_161
  %loadtmp7 = load i64, ptr @hub_pwr_delay_2, align 8
  ret i64 %loadtmp7

then_164:                                         ; preds = %merge_163
  %loadtmp10 = load i64, ptr @hub_pwr_delay_3, align 8
  ret i64 %loadtmp10

then_166:                                         ; preds = %merge_165
  %loadtmp13 = load i64, ptr @hub_pwr_delay_4, align 8
  ret i64 %loadtmp13

then_168:                                         ; preds = %merge_167
  %loadtmp16 = load i64, ptr @hub_pwr_delay_5, align 8
  ret i64 %loadtmp16

then_170:                                         ; preds = %merge_169
  %loadtmp19 = load i64, ptr @hub_pwr_delay_6, align 8
  ret i64 %loadtmp19

then_172:                                         ; preds = %merge_171
  %loadtmp22 = load i64, ptr @hub_pwr_delay_7, align 8
  ret i64 %loadtmp22
}

define i64 @hub_set_pwr_delay(i64 %idx, i64 %val) {
entry:
  %"%hub_pwr_delay_7" = alloca i64, align 8
  %"%hub_pwr_delay_6" = alloca i64, align 8
  %"%hub_pwr_delay_5" = alloca i64, align 8
  %"%hub_pwr_delay_4" = alloca i64, align 8
  %"%hub_pwr_delay_3" = alloca i64, align 8
  %"%hub_pwr_delay_2" = alloca i64, align 8
  %"%hub_pwr_delay_1" = alloca i64, align 8
  %"%hub_pwr_delay_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_174, label %merge_175

merge_175:                                        ; preds = %then_174, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_176, label %merge_177

merge_177:                                        ; preds = %then_176, %merge_175
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_178, label %merge_179

merge_179:                                        ; preds = %then_178, %merge_177
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_180, label %merge_181

merge_181:                                        ; preds = %then_180, %merge_179
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_182, label %merge_183

merge_183:                                        ; preds = %then_182, %merge_181
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_184, label %merge_185

merge_185:                                        ; preds = %then_184, %merge_183
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_186, label %merge_187

merge_187:                                        ; preds = %then_186, %merge_185
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_188, label %merge_189

merge_189:                                        ; preds = %then_188, %merge_187
  ret i64 0

then_174:                                         ; preds = %entry
  %"%446.load" = load i64, ptr %val2, align 8
  store i64 %"%446.load", ptr %"%hub_pwr_delay_0", align 8
  br label %merge_175

then_176:                                         ; preds = %merge_175
  %"%450.load" = load i64, ptr %val2, align 8
  store i64 %"%450.load", ptr %"%hub_pwr_delay_1", align 8
  br label %merge_177

then_178:                                         ; preds = %merge_177
  %"%454.load" = load i64, ptr %val2, align 8
  store i64 %"%454.load", ptr %"%hub_pwr_delay_2", align 8
  br label %merge_179

then_180:                                         ; preds = %merge_179
  %"%458.load" = load i64, ptr %val2, align 8
  store i64 %"%458.load", ptr %"%hub_pwr_delay_3", align 8
  br label %merge_181

then_182:                                         ; preds = %merge_181
  %"%462.load" = load i64, ptr %val2, align 8
  store i64 %"%462.load", ptr %"%hub_pwr_delay_4", align 8
  br label %merge_183

then_184:                                         ; preds = %merge_183
  %"%466.load" = load i64, ptr %val2, align 8
  store i64 %"%466.load", ptr %"%hub_pwr_delay_5", align 8
  br label %merge_185

then_186:                                         ; preds = %merge_185
  %"%470.load" = load i64, ptr %val2, align 8
  store i64 %"%470.load", ptr %"%hub_pwr_delay_6", align 8
  br label %merge_187

then_188:                                         ; preds = %merge_187
  %"%474.load" = load i64, ptr %val2, align 8
  store i64 %"%474.load", ptr %"%hub_pwr_delay_7", align 8
  br label %merge_189
}

define i64 @hub_get_port_dev(i64 %hub_idx, i64 %port) {
entry:
  %port2 = alloca i64, align 8
  store i64 %port, ptr %port2, align 8
  %hub_idx1 = alloca i64, align 8
  store i64 %hub_idx, ptr %hub_idx1, align 8
  %"%hub_idx.load" = load i64, ptr %hub_idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%hub_idx.load", 0
  br i1 %cmpeqtmp, label %then_190, label %merge_191

merge_191:                                        ; preds = %merge_207, %entry
  %"%hub_idx.load4" = load i64, ptr %hub_idx1, align 8
  %cmpeqtmp5 = icmp eq i64 %"%hub_idx.load4", 1
  br i1 %cmpeqtmp5, label %then_208, label %merge_209

merge_193:                                        ; preds = %then_190
  %"%port.load6" = load i64, ptr %port2, align 8
  %cmpeqtmp7 = icmp eq i64 %"%port.load6", 1
  br i1 %cmpeqtmp7, label %then_194, label %merge_195

merge_195:                                        ; preds = %merge_193
  %"%port.load9" = load i64, ptr %port2, align 8
  %cmpeqtmp10 = icmp eq i64 %"%port.load9", 2
  br i1 %cmpeqtmp10, label %then_196, label %merge_197

merge_197:                                        ; preds = %merge_195
  %"%port.load12" = load i64, ptr %port2, align 8
  %cmpeqtmp13 = icmp eq i64 %"%port.load12", 3
  br i1 %cmpeqtmp13, label %then_198, label %merge_199

merge_199:                                        ; preds = %merge_197
  %"%port.load15" = load i64, ptr %port2, align 8
  %cmpeqtmp16 = icmp eq i64 %"%port.load15", 4
  br i1 %cmpeqtmp16, label %then_200, label %merge_201

merge_201:                                        ; preds = %merge_199
  %"%port.load18" = load i64, ptr %port2, align 8
  %cmpeqtmp19 = icmp eq i64 %"%port.load18", 5
  br i1 %cmpeqtmp19, label %then_202, label %merge_203

merge_203:                                        ; preds = %merge_201
  %"%port.load21" = load i64, ptr %port2, align 8
  %cmpeqtmp22 = icmp eq i64 %"%port.load21", 6
  br i1 %cmpeqtmp22, label %then_204, label %merge_205

merge_205:                                        ; preds = %merge_203
  %"%port.load24" = load i64, ptr %port2, align 8
  %cmpeqtmp25 = icmp eq i64 %"%port.load24", 7
  br i1 %cmpeqtmp25, label %then_206, label %merge_207

merge_207:                                        ; preds = %merge_205
  br label %merge_191

merge_209:                                        ; preds = %merge_225, %merge_191
  %"%hub_idx.load29" = load i64, ptr %hub_idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%hub_idx.load29", 2
  br i1 %cmpeqtmp30, label %then_226, label %merge_227

merge_211:                                        ; preds = %then_208
  %"%port.load32" = load i64, ptr %port2, align 8
  %cmpeqtmp33 = icmp eq i64 %"%port.load32", 1
  br i1 %cmpeqtmp33, label %then_212, label %merge_213

merge_213:                                        ; preds = %merge_211
  %"%port.load35" = load i64, ptr %port2, align 8
  %cmpeqtmp36 = icmp eq i64 %"%port.load35", 2
  br i1 %cmpeqtmp36, label %then_214, label %merge_215

merge_215:                                        ; preds = %merge_213
  %"%port.load38" = load i64, ptr %port2, align 8
  %cmpeqtmp39 = icmp eq i64 %"%port.load38", 3
  br i1 %cmpeqtmp39, label %then_216, label %merge_217

merge_217:                                        ; preds = %merge_215
  %"%port.load41" = load i64, ptr %port2, align 8
  %cmpeqtmp42 = icmp eq i64 %"%port.load41", 4
  br i1 %cmpeqtmp42, label %then_218, label %merge_219

merge_219:                                        ; preds = %merge_217
  %"%port.load44" = load i64, ptr %port2, align 8
  %cmpeqtmp45 = icmp eq i64 %"%port.load44", 5
  br i1 %cmpeqtmp45, label %then_220, label %merge_221

merge_221:                                        ; preds = %merge_219
  %"%port.load47" = load i64, ptr %port2, align 8
  %cmpeqtmp48 = icmp eq i64 %"%port.load47", 6
  br i1 %cmpeqtmp48, label %then_222, label %merge_223

merge_223:                                        ; preds = %merge_221
  %"%port.load50" = load i64, ptr %port2, align 8
  %cmpeqtmp51 = icmp eq i64 %"%port.load50", 7
  br i1 %cmpeqtmp51, label %then_224, label %merge_225

merge_225:                                        ; preds = %merge_223
  br label %merge_209

merge_227:                                        ; preds = %merge_243, %merge_209
  %"%hub_idx.load55" = load i64, ptr %hub_idx1, align 8
  %cmpeqtmp56 = icmp eq i64 %"%hub_idx.load55", 3
  br i1 %cmpeqtmp56, label %then_244, label %merge_245

merge_229:                                        ; preds = %then_226
  %"%port.load58" = load i64, ptr %port2, align 8
  %cmpeqtmp59 = icmp eq i64 %"%port.load58", 1
  br i1 %cmpeqtmp59, label %then_230, label %merge_231

merge_231:                                        ; preds = %merge_229
  %"%port.load61" = load i64, ptr %port2, align 8
  %cmpeqtmp62 = icmp eq i64 %"%port.load61", 2
  br i1 %cmpeqtmp62, label %then_232, label %merge_233

merge_233:                                        ; preds = %merge_231
  %"%port.load64" = load i64, ptr %port2, align 8
  %cmpeqtmp65 = icmp eq i64 %"%port.load64", 3
  br i1 %cmpeqtmp65, label %then_234, label %merge_235

merge_235:                                        ; preds = %merge_233
  %"%port.load67" = load i64, ptr %port2, align 8
  %cmpeqtmp68 = icmp eq i64 %"%port.load67", 4
  br i1 %cmpeqtmp68, label %then_236, label %merge_237

merge_237:                                        ; preds = %merge_235
  %"%port.load70" = load i64, ptr %port2, align 8
  %cmpeqtmp71 = icmp eq i64 %"%port.load70", 5
  br i1 %cmpeqtmp71, label %then_238, label %merge_239

merge_239:                                        ; preds = %merge_237
  %"%port.load73" = load i64, ptr %port2, align 8
  %cmpeqtmp74 = icmp eq i64 %"%port.load73", 6
  br i1 %cmpeqtmp74, label %then_240, label %merge_241

merge_241:                                        ; preds = %merge_239
  %"%port.load76" = load i64, ptr %port2, align 8
  %cmpeqtmp77 = icmp eq i64 %"%port.load76", 7
  br i1 %cmpeqtmp77, label %then_242, label %merge_243

merge_243:                                        ; preds = %merge_241
  br label %merge_227

merge_245:                                        ; preds = %merge_261, %merge_227
  ret i64 0

merge_247:                                        ; preds = %then_244
  %"%port.load82" = load i64, ptr %port2, align 8
  %cmpeqtmp83 = icmp eq i64 %"%port.load82", 1
  br i1 %cmpeqtmp83, label %then_248, label %merge_249

merge_249:                                        ; preds = %merge_247
  %"%port.load85" = load i64, ptr %port2, align 8
  %cmpeqtmp86 = icmp eq i64 %"%port.load85", 2
  br i1 %cmpeqtmp86, label %then_250, label %merge_251

merge_251:                                        ; preds = %merge_249
  %"%port.load88" = load i64, ptr %port2, align 8
  %cmpeqtmp89 = icmp eq i64 %"%port.load88", 3
  br i1 %cmpeqtmp89, label %then_252, label %merge_253

merge_253:                                        ; preds = %merge_251
  %"%port.load91" = load i64, ptr %port2, align 8
  %cmpeqtmp92 = icmp eq i64 %"%port.load91", 4
  br i1 %cmpeqtmp92, label %then_254, label %merge_255

merge_255:                                        ; preds = %merge_253
  %"%port.load94" = load i64, ptr %port2, align 8
  %cmpeqtmp95 = icmp eq i64 %"%port.load94", 5
  br i1 %cmpeqtmp95, label %then_256, label %merge_257

merge_257:                                        ; preds = %merge_255
  %"%port.load97" = load i64, ptr %port2, align 8
  %cmpeqtmp98 = icmp eq i64 %"%port.load97", 6
  br i1 %cmpeqtmp98, label %then_258, label %merge_259

merge_259:                                        ; preds = %merge_257
  %"%port.load100" = load i64, ptr %port2, align 8
  %cmpeqtmp101 = icmp eq i64 %"%port.load100", 7
  br i1 %cmpeqtmp101, label %then_260, label %merge_261

merge_261:                                        ; preds = %merge_259
  br label %merge_245

then_190:                                         ; preds = %entry
  %"%port.load" = load i64, ptr %port2, align 8
  %cmpeqtmp3 = icmp eq i64 %"%port.load", 0
  br i1 %cmpeqtmp3, label %then_192, label %merge_193

then_192:                                         ; preds = %then_190
  %loadtmp = load i64, ptr @port_dev_00, align 8
  ret i64 %loadtmp

then_194:                                         ; preds = %merge_193
  %loadtmp8 = load i64, ptr @port_dev_01, align 8
  ret i64 %loadtmp8

then_196:                                         ; preds = %merge_195
  %loadtmp11 = load i64, ptr @port_dev_02, align 8
  ret i64 %loadtmp11

then_198:                                         ; preds = %merge_197
  %loadtmp14 = load i64, ptr @port_dev_03, align 8
  ret i64 %loadtmp14

then_200:                                         ; preds = %merge_199
  %loadtmp17 = load i64, ptr @port_dev_04, align 8
  ret i64 %loadtmp17

then_202:                                         ; preds = %merge_201
  %loadtmp20 = load i64, ptr @port_dev_05, align 8
  ret i64 %loadtmp20

then_204:                                         ; preds = %merge_203
  %loadtmp23 = load i64, ptr @port_dev_06, align 8
  ret i64 %loadtmp23

then_206:                                         ; preds = %merge_205
  %loadtmp26 = load i64, ptr @port_dev_07, align 8
  ret i64 %loadtmp26

then_208:                                         ; preds = %merge_191
  %"%port.load27" = load i64, ptr %port2, align 8
  %cmpeqtmp28 = icmp eq i64 %"%port.load27", 0
  br i1 %cmpeqtmp28, label %then_210, label %merge_211

then_210:                                         ; preds = %then_208
  %loadtmp31 = load i64, ptr @port_dev_10, align 8
  ret i64 %loadtmp31

then_212:                                         ; preds = %merge_211
  %loadtmp34 = load i64, ptr @port_dev_11, align 8
  ret i64 %loadtmp34

then_214:                                         ; preds = %merge_213
  %loadtmp37 = load i64, ptr @port_dev_12, align 8
  ret i64 %loadtmp37

then_216:                                         ; preds = %merge_215
  %loadtmp40 = load i64, ptr @port_dev_13, align 8
  ret i64 %loadtmp40

then_218:                                         ; preds = %merge_217
  %loadtmp43 = load i64, ptr @port_dev_14, align 8
  ret i64 %loadtmp43

then_220:                                         ; preds = %merge_219
  %loadtmp46 = load i64, ptr @port_dev_15, align 8
  ret i64 %loadtmp46

then_222:                                         ; preds = %merge_221
  %loadtmp49 = load i64, ptr @port_dev_16, align 8
  ret i64 %loadtmp49

then_224:                                         ; preds = %merge_223
  %loadtmp52 = load i64, ptr @port_dev_17, align 8
  ret i64 %loadtmp52

then_226:                                         ; preds = %merge_209
  %"%port.load53" = load i64, ptr %port2, align 8
  %cmpeqtmp54 = icmp eq i64 %"%port.load53", 0
  br i1 %cmpeqtmp54, label %then_228, label %merge_229

then_228:                                         ; preds = %then_226
  %loadtmp57 = load i64, ptr @port_dev_20, align 8
  ret i64 %loadtmp57

then_230:                                         ; preds = %merge_229
  %loadtmp60 = load i64, ptr @port_dev_21, align 8
  ret i64 %loadtmp60

then_232:                                         ; preds = %merge_231
  %loadtmp63 = load i64, ptr @port_dev_22, align 8
  ret i64 %loadtmp63

then_234:                                         ; preds = %merge_233
  %loadtmp66 = load i64, ptr @port_dev_23, align 8
  ret i64 %loadtmp66

then_236:                                         ; preds = %merge_235
  %loadtmp69 = load i64, ptr @port_dev_24, align 8
  ret i64 %loadtmp69

then_238:                                         ; preds = %merge_237
  %loadtmp72 = load i64, ptr @port_dev_25, align 8
  ret i64 %loadtmp72

then_240:                                         ; preds = %merge_239
  %loadtmp75 = load i64, ptr @port_dev_26, align 8
  ret i64 %loadtmp75

then_242:                                         ; preds = %merge_241
  %loadtmp78 = load i64, ptr @port_dev_27, align 8
  ret i64 %loadtmp78

then_244:                                         ; preds = %merge_227
  %"%port.load79" = load i64, ptr %port2, align 8
  %cmpeqtmp80 = icmp eq i64 %"%port.load79", 0
  br i1 %cmpeqtmp80, label %then_246, label %merge_247

then_246:                                         ; preds = %then_244
  %loadtmp81 = load i64, ptr @port_dev_30, align 8
  ret i64 %loadtmp81

then_248:                                         ; preds = %merge_247
  %loadtmp84 = load i64, ptr @port_dev_31, align 8
  ret i64 %loadtmp84

then_250:                                         ; preds = %merge_249
  %loadtmp87 = load i64, ptr @port_dev_32, align 8
  ret i64 %loadtmp87

then_252:                                         ; preds = %merge_251
  %loadtmp90 = load i64, ptr @port_dev_33, align 8
  ret i64 %loadtmp90

then_254:                                         ; preds = %merge_253
  %loadtmp93 = load i64, ptr @port_dev_34, align 8
  ret i64 %loadtmp93

then_256:                                         ; preds = %merge_255
  %loadtmp96 = load i64, ptr @port_dev_35, align 8
  ret i64 %loadtmp96

then_258:                                         ; preds = %merge_257
  %loadtmp99 = load i64, ptr @port_dev_36, align 8
  ret i64 %loadtmp99

then_260:                                         ; preds = %merge_259
  %loadtmp102 = load i64, ptr @port_dev_37, align 8
  ret i64 %loadtmp102
}

define i64 @hub_set_port_dev(i64 %hub_idx, i64 %port, i64 %val) {
entry:
  %"%port_dev_37" = alloca i64, align 8
  %"%port_dev_36" = alloca i64, align 8
  %"%port_dev_35" = alloca i64, align 8
  %"%port_dev_34" = alloca i64, align 8
  %"%port_dev_33" = alloca i64, align 8
  %"%port_dev_32" = alloca i64, align 8
  %"%port_dev_31" = alloca i64, align 8
  %"%port_dev_30" = alloca i64, align 8
  %"%port_dev_27" = alloca i64, align 8
  %"%port_dev_26" = alloca i64, align 8
  %"%port_dev_25" = alloca i64, align 8
  %"%port_dev_24" = alloca i64, align 8
  %"%port_dev_23" = alloca i64, align 8
  %"%port_dev_22" = alloca i64, align 8
  %"%port_dev_21" = alloca i64, align 8
  %"%port_dev_20" = alloca i64, align 8
  %"%port_dev_17" = alloca i64, align 8
  %"%port_dev_16" = alloca i64, align 8
  %"%port_dev_15" = alloca i64, align 8
  %"%port_dev_14" = alloca i64, align 8
  %"%port_dev_13" = alloca i64, align 8
  %"%port_dev_12" = alloca i64, align 8
  %"%port_dev_11" = alloca i64, align 8
  %"%port_dev_10" = alloca i64, align 8
  %"%port_dev_07" = alloca i64, align 8
  %"%port_dev_06" = alloca i64, align 8
  %"%port_dev_05" = alloca i64, align 8
  %"%port_dev_04" = alloca i64, align 8
  %"%port_dev_03" = alloca i64, align 8
  %"%port_dev_02" = alloca i64, align 8
  %"%port_dev_01" = alloca i64, align 8
  %"%port_dev_00" = alloca i64, align 8
  %val3 = alloca i64, align 8
  store i64 %val, ptr %val3, align 8
  %port2 = alloca i64, align 8
  store i64 %port, ptr %port2, align 8
  %hub_idx1 = alloca i64, align 8
  store i64 %hub_idx, ptr %hub_idx1, align 8
  %"%hub_idx.load" = load i64, ptr %hub_idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%hub_idx.load", 0
  br i1 %cmpeqtmp, label %then_262, label %merge_263

merge_263:                                        ; preds = %merge_279, %entry
  %"%hub_idx.load5" = load i64, ptr %hub_idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%hub_idx.load5", 1
  br i1 %cmpeqtmp6, label %then_280, label %merge_281

merge_265:                                        ; preds = %then_264, %then_262
  %"%port.load7" = load i64, ptr %port2, align 8
  %cmpeqtmp8 = icmp eq i64 %"%port.load7", 1
  br i1 %cmpeqtmp8, label %then_266, label %merge_267

merge_267:                                        ; preds = %then_266, %merge_265
  %"%port.load9" = load i64, ptr %port2, align 8
  %cmpeqtmp10 = icmp eq i64 %"%port.load9", 2
  br i1 %cmpeqtmp10, label %then_268, label %merge_269

merge_269:                                        ; preds = %then_268, %merge_267
  %"%port.load11" = load i64, ptr %port2, align 8
  %cmpeqtmp12 = icmp eq i64 %"%port.load11", 3
  br i1 %cmpeqtmp12, label %then_270, label %merge_271

merge_271:                                        ; preds = %then_270, %merge_269
  %"%port.load13" = load i64, ptr %port2, align 8
  %cmpeqtmp14 = icmp eq i64 %"%port.load13", 4
  br i1 %cmpeqtmp14, label %then_272, label %merge_273

merge_273:                                        ; preds = %then_272, %merge_271
  %"%port.load15" = load i64, ptr %port2, align 8
  %cmpeqtmp16 = icmp eq i64 %"%port.load15", 5
  br i1 %cmpeqtmp16, label %then_274, label %merge_275

merge_275:                                        ; preds = %then_274, %merge_273
  %"%port.load17" = load i64, ptr %port2, align 8
  %cmpeqtmp18 = icmp eq i64 %"%port.load17", 6
  br i1 %cmpeqtmp18, label %then_276, label %merge_277

merge_277:                                        ; preds = %then_276, %merge_275
  %"%port.load19" = load i64, ptr %port2, align 8
  %cmpeqtmp20 = icmp eq i64 %"%port.load19", 7
  br i1 %cmpeqtmp20, label %then_278, label %merge_279

merge_279:                                        ; preds = %then_278, %merge_277
  br label %merge_263

merge_281:                                        ; preds = %merge_297, %merge_263
  %"%hub_idx.load23" = load i64, ptr %hub_idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%hub_idx.load23", 2
  br i1 %cmpeqtmp24, label %then_298, label %merge_299

merge_283:                                        ; preds = %then_282, %then_280
  %"%port.load25" = load i64, ptr %port2, align 8
  %cmpeqtmp26 = icmp eq i64 %"%port.load25", 1
  br i1 %cmpeqtmp26, label %then_284, label %merge_285

merge_285:                                        ; preds = %then_284, %merge_283
  %"%port.load27" = load i64, ptr %port2, align 8
  %cmpeqtmp28 = icmp eq i64 %"%port.load27", 2
  br i1 %cmpeqtmp28, label %then_286, label %merge_287

merge_287:                                        ; preds = %then_286, %merge_285
  %"%port.load29" = load i64, ptr %port2, align 8
  %cmpeqtmp30 = icmp eq i64 %"%port.load29", 3
  br i1 %cmpeqtmp30, label %then_288, label %merge_289

merge_289:                                        ; preds = %then_288, %merge_287
  %"%port.load31" = load i64, ptr %port2, align 8
  %cmpeqtmp32 = icmp eq i64 %"%port.load31", 4
  br i1 %cmpeqtmp32, label %then_290, label %merge_291

merge_291:                                        ; preds = %then_290, %merge_289
  %"%port.load33" = load i64, ptr %port2, align 8
  %cmpeqtmp34 = icmp eq i64 %"%port.load33", 5
  br i1 %cmpeqtmp34, label %then_292, label %merge_293

merge_293:                                        ; preds = %then_292, %merge_291
  %"%port.load35" = load i64, ptr %port2, align 8
  %cmpeqtmp36 = icmp eq i64 %"%port.load35", 6
  br i1 %cmpeqtmp36, label %then_294, label %merge_295

merge_295:                                        ; preds = %then_294, %merge_293
  %"%port.load37" = load i64, ptr %port2, align 8
  %cmpeqtmp38 = icmp eq i64 %"%port.load37", 7
  br i1 %cmpeqtmp38, label %then_296, label %merge_297

merge_297:                                        ; preds = %then_296, %merge_295
  br label %merge_281

merge_299:                                        ; preds = %merge_315, %merge_281
  %"%hub_idx.load41" = load i64, ptr %hub_idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%hub_idx.load41", 3
  br i1 %cmpeqtmp42, label %then_316, label %merge_317

merge_301:                                        ; preds = %then_300, %then_298
  %"%port.load43" = load i64, ptr %port2, align 8
  %cmpeqtmp44 = icmp eq i64 %"%port.load43", 1
  br i1 %cmpeqtmp44, label %then_302, label %merge_303

merge_303:                                        ; preds = %then_302, %merge_301
  %"%port.load45" = load i64, ptr %port2, align 8
  %cmpeqtmp46 = icmp eq i64 %"%port.load45", 2
  br i1 %cmpeqtmp46, label %then_304, label %merge_305

merge_305:                                        ; preds = %then_304, %merge_303
  %"%port.load47" = load i64, ptr %port2, align 8
  %cmpeqtmp48 = icmp eq i64 %"%port.load47", 3
  br i1 %cmpeqtmp48, label %then_306, label %merge_307

merge_307:                                        ; preds = %then_306, %merge_305
  %"%port.load49" = load i64, ptr %port2, align 8
  %cmpeqtmp50 = icmp eq i64 %"%port.load49", 4
  br i1 %cmpeqtmp50, label %then_308, label %merge_309

merge_309:                                        ; preds = %then_308, %merge_307
  %"%port.load51" = load i64, ptr %port2, align 8
  %cmpeqtmp52 = icmp eq i64 %"%port.load51", 5
  br i1 %cmpeqtmp52, label %then_310, label %merge_311

merge_311:                                        ; preds = %then_310, %merge_309
  %"%port.load53" = load i64, ptr %port2, align 8
  %cmpeqtmp54 = icmp eq i64 %"%port.load53", 6
  br i1 %cmpeqtmp54, label %then_312, label %merge_313

merge_313:                                        ; preds = %then_312, %merge_311
  %"%port.load55" = load i64, ptr %port2, align 8
  %cmpeqtmp56 = icmp eq i64 %"%port.load55", 7
  br i1 %cmpeqtmp56, label %then_314, label %merge_315

merge_315:                                        ; preds = %then_314, %merge_313
  br label %merge_299

merge_317:                                        ; preds = %merge_333, %merge_299
  ret i64 0

merge_319:                                        ; preds = %then_318, %then_316
  %"%port.load59" = load i64, ptr %port2, align 8
  %cmpeqtmp60 = icmp eq i64 %"%port.load59", 1
  br i1 %cmpeqtmp60, label %then_320, label %merge_321

merge_321:                                        ; preds = %then_320, %merge_319
  %"%port.load61" = load i64, ptr %port2, align 8
  %cmpeqtmp62 = icmp eq i64 %"%port.load61", 2
  br i1 %cmpeqtmp62, label %then_322, label %merge_323

merge_323:                                        ; preds = %then_322, %merge_321
  %"%port.load63" = load i64, ptr %port2, align 8
  %cmpeqtmp64 = icmp eq i64 %"%port.load63", 3
  br i1 %cmpeqtmp64, label %then_324, label %merge_325

merge_325:                                        ; preds = %then_324, %merge_323
  %"%port.load65" = load i64, ptr %port2, align 8
  %cmpeqtmp66 = icmp eq i64 %"%port.load65", 4
  br i1 %cmpeqtmp66, label %then_326, label %merge_327

merge_327:                                        ; preds = %then_326, %merge_325
  %"%port.load67" = load i64, ptr %port2, align 8
  %cmpeqtmp68 = icmp eq i64 %"%port.load67", 5
  br i1 %cmpeqtmp68, label %then_328, label %merge_329

merge_329:                                        ; preds = %then_328, %merge_327
  %"%port.load69" = load i64, ptr %port2, align 8
  %cmpeqtmp70 = icmp eq i64 %"%port.load69", 6
  br i1 %cmpeqtmp70, label %then_330, label %merge_331

merge_331:                                        ; preds = %then_330, %merge_329
  %"%port.load71" = load i64, ptr %port2, align 8
  %cmpeqtmp72 = icmp eq i64 %"%port.load71", 7
  br i1 %cmpeqtmp72, label %then_332, label %merge_333

merge_333:                                        ; preds = %then_332, %merge_331
  br label %merge_317

then_262:                                         ; preds = %entry
  %"%port.load" = load i64, ptr %port2, align 8
  %cmpeqtmp4 = icmp eq i64 %"%port.load", 0
  br i1 %cmpeqtmp4, label %then_264, label %merge_265

then_264:                                         ; preds = %then_262
  %"%650.load" = load i64, ptr %val3, align 8
  store i64 %"%650.load", ptr %"%port_dev_00", align 8
  br label %merge_265

then_266:                                         ; preds = %merge_265
  %"%654.load" = load i64, ptr %val3, align 8
  store i64 %"%654.load", ptr %"%port_dev_01", align 8
  br label %merge_267

then_268:                                         ; preds = %merge_267
  %"%658.load" = load i64, ptr %val3, align 8
  store i64 %"%658.load", ptr %"%port_dev_02", align 8
  br label %merge_269

then_270:                                         ; preds = %merge_269
  %"%662.load" = load i64, ptr %val3, align 8
  store i64 %"%662.load", ptr %"%port_dev_03", align 8
  br label %merge_271

then_272:                                         ; preds = %merge_271
  %"%666.load" = load i64, ptr %val3, align 8
  store i64 %"%666.load", ptr %"%port_dev_04", align 8
  br label %merge_273

then_274:                                         ; preds = %merge_273
  %"%670.load" = load i64, ptr %val3, align 8
  store i64 %"%670.load", ptr %"%port_dev_05", align 8
  br label %merge_275

then_276:                                         ; preds = %merge_275
  %"%674.load" = load i64, ptr %val3, align 8
  store i64 %"%674.load", ptr %"%port_dev_06", align 8
  br label %merge_277

then_278:                                         ; preds = %merge_277
  %"%678.load" = load i64, ptr %val3, align 8
  store i64 %"%678.load", ptr %"%port_dev_07", align 8
  br label %merge_279

then_280:                                         ; preds = %merge_263
  %"%port.load21" = load i64, ptr %port2, align 8
  %cmpeqtmp22 = icmp eq i64 %"%port.load21", 0
  br i1 %cmpeqtmp22, label %then_282, label %merge_283

then_282:                                         ; preds = %then_280
  %"%684.load" = load i64, ptr %val3, align 8
  store i64 %"%684.load", ptr %"%port_dev_10", align 8
  br label %merge_283

then_284:                                         ; preds = %merge_283
  %"%688.load" = load i64, ptr %val3, align 8
  store i64 %"%688.load", ptr %"%port_dev_11", align 8
  br label %merge_285

then_286:                                         ; preds = %merge_285
  %"%692.load" = load i64, ptr %val3, align 8
  store i64 %"%692.load", ptr %"%port_dev_12", align 8
  br label %merge_287

then_288:                                         ; preds = %merge_287
  %"%696.load" = load i64, ptr %val3, align 8
  store i64 %"%696.load", ptr %"%port_dev_13", align 8
  br label %merge_289

then_290:                                         ; preds = %merge_289
  %"%700.load" = load i64, ptr %val3, align 8
  store i64 %"%700.load", ptr %"%port_dev_14", align 8
  br label %merge_291

then_292:                                         ; preds = %merge_291
  %"%704.load" = load i64, ptr %val3, align 8
  store i64 %"%704.load", ptr %"%port_dev_15", align 8
  br label %merge_293

then_294:                                         ; preds = %merge_293
  %"%708.load" = load i64, ptr %val3, align 8
  store i64 %"%708.load", ptr %"%port_dev_16", align 8
  br label %merge_295

then_296:                                         ; preds = %merge_295
  %"%712.load" = load i64, ptr %val3, align 8
  store i64 %"%712.load", ptr %"%port_dev_17", align 8
  br label %merge_297

then_298:                                         ; preds = %merge_281
  %"%port.load39" = load i64, ptr %port2, align 8
  %cmpeqtmp40 = icmp eq i64 %"%port.load39", 0
  br i1 %cmpeqtmp40, label %then_300, label %merge_301

then_300:                                         ; preds = %then_298
  %"%718.load" = load i64, ptr %val3, align 8
  store i64 %"%718.load", ptr %"%port_dev_20", align 8
  br label %merge_301

then_302:                                         ; preds = %merge_301
  %"%722.load" = load i64, ptr %val3, align 8
  store i64 %"%722.load", ptr %"%port_dev_21", align 8
  br label %merge_303

then_304:                                         ; preds = %merge_303
  %"%726.load" = load i64, ptr %val3, align 8
  store i64 %"%726.load", ptr %"%port_dev_22", align 8
  br label %merge_305

then_306:                                         ; preds = %merge_305
  %"%730.load" = load i64, ptr %val3, align 8
  store i64 %"%730.load", ptr %"%port_dev_23", align 8
  br label %merge_307

then_308:                                         ; preds = %merge_307
  %"%734.load" = load i64, ptr %val3, align 8
  store i64 %"%734.load", ptr %"%port_dev_24", align 8
  br label %merge_309

then_310:                                         ; preds = %merge_309
  %"%738.load" = load i64, ptr %val3, align 8
  store i64 %"%738.load", ptr %"%port_dev_25", align 8
  br label %merge_311

then_312:                                         ; preds = %merge_311
  %"%742.load" = load i64, ptr %val3, align 8
  store i64 %"%742.load", ptr %"%port_dev_26", align 8
  br label %merge_313

then_314:                                         ; preds = %merge_313
  %"%746.load" = load i64, ptr %val3, align 8
  store i64 %"%746.load", ptr %"%port_dev_27", align 8
  br label %merge_315

then_316:                                         ; preds = %merge_299
  %"%port.load57" = load i64, ptr %port2, align 8
  %cmpeqtmp58 = icmp eq i64 %"%port.load57", 0
  br i1 %cmpeqtmp58, label %then_318, label %merge_319

then_318:                                         ; preds = %then_316
  %"%752.load" = load i64, ptr %val3, align 8
  store i64 %"%752.load", ptr %"%port_dev_30", align 8
  br label %merge_319

then_320:                                         ; preds = %merge_319
  %"%756.load" = load i64, ptr %val3, align 8
  store i64 %"%756.load", ptr %"%port_dev_31", align 8
  br label %merge_321

then_322:                                         ; preds = %merge_321
  %"%760.load" = load i64, ptr %val3, align 8
  store i64 %"%760.load", ptr %"%port_dev_32", align 8
  br label %merge_323

then_324:                                         ; preds = %merge_323
  %"%764.load" = load i64, ptr %val3, align 8
  store i64 %"%764.load", ptr %"%port_dev_33", align 8
  br label %merge_325

then_326:                                         ; preds = %merge_325
  %"%768.load" = load i64, ptr %val3, align 8
  store i64 %"%768.load", ptr %"%port_dev_34", align 8
  br label %merge_327

then_328:                                         ; preds = %merge_327
  %"%772.load" = load i64, ptr %val3, align 8
  store i64 %"%772.load", ptr %"%port_dev_35", align 8
  br label %merge_329

then_330:                                         ; preds = %merge_329
  %"%776.load" = load i64, ptr %val3, align 8
  store i64 %"%776.load", ptr %"%port_dev_36", align 8
  br label %merge_331

then_332:                                         ; preds = %merge_331
  %"%780.load" = load i64, ptr %val3, align 8
  store i64 %"%780.load", ptr %"%port_dev_37", align 8
  br label %merge_333
}

define i64 @hub_get_descriptor(i64 %hub_idx) {
entry:
  %"%hub_errors" = alloca i64, align 8
  %"%pwr_delay" = alloca i64, align 8
  %"%num_ports" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%wValue" = alloca i64, align 8
  %"%addr" = alloca i64, align 8
  %"%hc" = alloca i64, align 8
  %hub_idx1 = alloca i64, align 8
  store i64 %hub_idx, ptr %hub_idx1, align 8
  %"%hub_idx.load" = load i64, ptr %hub_idx1, align 8
  %calltmp = call i64 @hub_get_active(i64 %"%hub_idx.load")
  %cmpeqtmp = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp, label %then_334, label %merge_335

else_338:                                         ; preds = %merge_335
  %"%hub_errors.load" = load i64, ptr @hub_errors, align 8
  %addtmp11 = add i64 %"%hub_errors.load", 1
  store i64 %addtmp11, ptr %"%hub_errors", align 8
  br label %merge_337

merge_335:                                        ; preds = %entry
  %"%hub_idx.load2" = load i64, ptr %hub_idx1, align 8
  %calltmp3 = call i64 @hub_get_hc(i64 %"%hub_idx.load2")
  store i64 %calltmp3, ptr %"%hc", align 8
  %"%hub_idx.load4" = load i64, ptr %hub_idx1, align 8
  %calltmp5 = call i64 @hub_get_addr(i64 %"%hub_idx.load4")
  store i64 %calltmp5, ptr %"%addr", align 8
  %"%HUB_DESC_TYPE.load" = load i64, ptr @HUB_DESC_TYPE, align 8
  %shltmp = shl i64 %"%HUB_DESC_TYPE.load", 8
  store i64 %shltmp, ptr %"%wValue", align 8
  %"%hub_setup_buf.load" = load i64, ptr @hub_setup_buf, align 8
  %"%HUB_REQ_TYPE_GET.load" = load i64, ptr @HUB_REQ_TYPE_GET, align 8
  %"%GET_DESCRIPTOR.load" = load i64, ptr @GET_DESCRIPTOR, align 8
  %"%wValue.load" = load i64, ptr %"%wValue", align 8
  call void @usb_build_setup(i64 %"%hub_setup_buf.load", i64 %"%HUB_REQ_TYPE_GET.load", i64 %"%GET_DESCRIPTOR.load", i64 %"%wValue.load", i64 0, i64 8)
  %"%hub_data_buf.load" = load i64, ptr @hub_data_buf, align 8
  call void @sad_ll_memset(i64 %"%hub_data_buf.load", i64 0, i64 16)
  %"%hc.load" = load i64, ptr %"%hc", align 8
  %"%addr.load" = load i64, ptr %"%addr", align 8
  %"%hub_setup_buf.load6" = load i64, ptr @hub_setup_buf, align 8
  %"%hub_data_buf.load7" = load i64, ptr @hub_data_buf, align 8
  call void @uhci_control_transfer(i64 %"%hc.load", i64 %"%addr.load", i64 %"%hub_setup_buf.load6", i64 %"%hub_data_buf.load7", i64 8)
  store i64 0, ptr %"%result", align 8
  %"%result.load" = load i64, ptr %"%result", align 8
  %cmpgetmp = icmp sge i64 %"%result.load", 0
  br i1 %cmpgetmp, label %then_336, label %else_338

merge_337:                                        ; preds = %merge_340, %else_338
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

merge_340:                                        ; preds = %then_339, %then_336
  %"%hub_idx.load13" = load i64, ptr %hub_idx1, align 8
  %"%num_ports.load14" = load i64, ptr %"%num_ports", align 8
  %calltmp15 = call i64 @hub_set_num_ports(i64 %"%hub_idx.load13", i64 %"%num_ports.load14")
  %"%hub_idx.load16" = load i64, ptr %hub_idx1, align 8
  %"%pwr_delay.load" = load i64, ptr %"%pwr_delay", align 8
  %calltmp17 = call i64 @hub_set_pwr_delay(i64 %"%hub_idx.load16", i64 %"%pwr_delay.load")
  %calltmp18 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %"%825.load" = load i64, ptr %hub_idx1, align 8
  %calltmp19 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%825.load")
  %calltmp20 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %"%830.load" = load i64, ptr %"%num_ports", align 8
  %calltmp21 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%830.load")
  %calltmp22 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %"%pwr_delay.load23" = load i64, ptr %"%pwr_delay", align 8
  %shltmp24 = shl i64 %"%pwr_delay.load23", 1
  %calltmp25 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %shltmp24)
  %calltmp26 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %calltmp27 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_337

then_334:                                         ; preds = %entry
  ret i64 -1

then_336:                                         ; preds = %merge_335
  %"%hub_data_buf.load8" = load i64, ptr @hub_data_buf, align 8
  %addtmp = add i64 %"%hub_data_buf.load8", 2
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%num_ports", align 8
  %"%hub_data_buf.load9" = load i64, ptr @hub_data_buf, align 8
  %addtmp10 = add i64 %"%hub_data_buf.load9", 5
  call void @sad_ll_mem_read8(i64 %addtmp10)
  store i64 0, ptr %"%pwr_delay", align 8
  %"%num_ports.load" = load i64, ptr %"%num_ports", align 8
  %"%MAX_PORTS_PER_HUB.load" = load i64, ptr @MAX_PORTS_PER_HUB, align 8
  %cmpgttmp = icmp sgt i64 %"%num_ports.load", %"%MAX_PORTS_PER_HUB.load"
  br i1 %cmpgttmp, label %then_339, label %merge_340

then_339:                                         ; preds = %then_336
  %"%MAX_PORTS_PER_HUB.load12" = load i64, ptr @MAX_PORTS_PER_HUB, align 8
  store i64 %"%MAX_PORTS_PER_HUB.load12", ptr %"%num_ports", align 8
  br label %merge_340
}

define i64 @hub_get_port_status(i64 %hub_idx, i64 %port) {
entry:
  %"%hub_errors" = alloca i64, align 8
  %"%port_change" = alloca i64, align 8
  %"%port_status" = alloca i64, align 8
  %"%change_lo" = alloca i64, align 8
  %"%status_hi" = alloca i64, align 8
  %"%status_lo" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%addr" = alloca i64, align 8
  %"%hc" = alloca i64, align 8
  %port2 = alloca i64, align 8
  store i64 %port, ptr %port2, align 8
  %hub_idx1 = alloca i64, align 8
  store i64 %hub_idx, ptr %hub_idx1, align 8
  %"%hub_idx.load" = load i64, ptr %hub_idx1, align 8
  %calltmp = call i64 @hub_get_active(i64 %"%hub_idx.load")
  %cmpeqtmp = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp, label %then_341, label %merge_342

merge_342:                                        ; preds = %entry
  %"%hub_idx.load3" = load i64, ptr %hub_idx1, align 8
  %calltmp4 = call i64 @hub_get_hc(i64 %"%hub_idx.load3")
  store i64 %calltmp4, ptr %"%hc", align 8
  %"%hub_idx.load5" = load i64, ptr %hub_idx1, align 8
  %calltmp6 = call i64 @hub_get_addr(i64 %"%hub_idx.load5")
  store i64 %calltmp6, ptr %"%addr", align 8
  %"%hub_setup_buf.load" = load i64, ptr @hub_setup_buf, align 8
  %"%PORT_REQ_TYPE_GET.load" = load i64, ptr @PORT_REQ_TYPE_GET, align 8
  %"%GET_STATUS.load" = load i64, ptr @GET_STATUS, align 8
  %"%port.load" = load i64, ptr %port2, align 8
  call void @usb_build_setup(i64 %"%hub_setup_buf.load", i64 %"%PORT_REQ_TYPE_GET.load", i64 %"%GET_STATUS.load", i64 0, i64 %"%port.load", i64 4)
  %"%hub_data_buf.load" = load i64, ptr @hub_data_buf, align 8
  call void @sad_ll_memset(i64 %"%hub_data_buf.load", i64 0, i64 4)
  %"%hc.load" = load i64, ptr %"%hc", align 8
  %"%addr.load" = load i64, ptr %"%addr", align 8
  %"%hub_setup_buf.load7" = load i64, ptr @hub_setup_buf, align 8
  %"%hub_data_buf.load8" = load i64, ptr @hub_data_buf, align 8
  call void @uhci_control_transfer(i64 %"%hc.load", i64 %"%addr.load", i64 %"%hub_setup_buf.load7", i64 %"%hub_data_buf.load8", i64 4)
  store i64 0, ptr %"%result", align 8
  %"%result.load" = load i64, ptr %"%result", align 8
  %cmpgetmp = icmp sge i64 %"%result.load", 0
  br i1 %cmpgetmp, label %then_343, label %merge_344

merge_344:                                        ; preds = %merge_342
  %"%hub_errors.load" = load i64, ptr @hub_errors, align 8
  %addtmp16 = add i64 %"%hub_errors.load", 1
  store i64 %addtmp16, ptr %"%hub_errors", align 8
  ret i64 -1

then_341:                                         ; preds = %entry
  ret i64 -1

then_343:                                         ; preds = %merge_342
  %"%hub_data_buf.load9" = load i64, ptr @hub_data_buf, align 8
  call void @sad_ll_mem_read8(i64 %"%hub_data_buf.load9")
  store i64 0, ptr %"%status_lo", align 8
  %"%hub_data_buf.load10" = load i64, ptr @hub_data_buf, align 8
  %addtmp = add i64 %"%hub_data_buf.load10", 1
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%status_hi", align 8
  %"%hub_data_buf.load11" = load i64, ptr @hub_data_buf, align 8
  %addtmp12 = add i64 %"%hub_data_buf.load11", 2
  call void @sad_ll_mem_read8(i64 %addtmp12)
  store i64 0, ptr %"%change_lo", align 8
  %"%status_hi.load" = load i64, ptr %"%status_hi", align 8
  %shltmp = shl i64 %"%status_hi.load", 8
  %"%status_lo.load" = load i64, ptr %"%status_lo", align 8
  %addtmp13 = add i64 %"%status_lo.load", %shltmp
  store i64 %addtmp13, ptr %"%port_status", align 8
  %"%887.load" = load i64, ptr %"%change_lo", align 8
  store i64 %"%887.load", ptr %"%port_change", align 8
  %"%port_change.load" = load i64, ptr %"%port_change", align 8
  %shltmp14 = shl i64 %"%port_change.load", 16
  %"%port_status.load" = load i64, ptr %"%port_status", align 8
  %addtmp15 = add i64 %"%port_status.load", %shltmp14
  ret i64 %addtmp15
}

define i64 @hub_set_port_feature(i64 %hub_idx, i64 %port, i64 %feature) {
entry:
  %"%result" = alloca i64, align 8
  %"%addr" = alloca i64, align 8
  %"%hc" = alloca i64, align 8
  %feature3 = alloca i64, align 8
  store i64 %feature, ptr %feature3, align 8
  %port2 = alloca i64, align 8
  store i64 %port, ptr %port2, align 8
  %hub_idx1 = alloca i64, align 8
  store i64 %hub_idx, ptr %hub_idx1, align 8
  %"%hub_idx.load" = load i64, ptr %hub_idx1, align 8
  %calltmp = call i64 @hub_get_active(i64 %"%hub_idx.load")
  %cmpeqtmp = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp, label %then_345, label %merge_346

merge_346:                                        ; preds = %entry
  %"%hub_idx.load4" = load i64, ptr %hub_idx1, align 8
  %calltmp5 = call i64 @hub_get_hc(i64 %"%hub_idx.load4")
  store i64 %calltmp5, ptr %"%hc", align 8
  %"%hub_idx.load6" = load i64, ptr %hub_idx1, align 8
  %calltmp7 = call i64 @hub_get_addr(i64 %"%hub_idx.load6")
  store i64 %calltmp7, ptr %"%addr", align 8
  %"%hub_setup_buf.load" = load i64, ptr @hub_setup_buf, align 8
  %"%PORT_REQ_TYPE_SET.load" = load i64, ptr @PORT_REQ_TYPE_SET, align 8
  %"%SET_FEATURE.load" = load i64, ptr @SET_FEATURE, align 8
  %"%feature.load" = load i64, ptr %feature3, align 8
  %"%port.load" = load i64, ptr %port2, align 8
  call void @usb_build_setup(i64 %"%hub_setup_buf.load", i64 %"%PORT_REQ_TYPE_SET.load", i64 %"%SET_FEATURE.load", i64 %"%feature.load", i64 %"%port.load", i64 0)
  %"%hc.load" = load i64, ptr %"%hc", align 8
  %"%addr.load" = load i64, ptr %"%addr", align 8
  %"%hub_setup_buf.load8" = load i64, ptr @hub_setup_buf, align 8
  call void @uhci_control_transfer(i64 %"%hc.load", i64 %"%addr.load", i64 %"%hub_setup_buf.load8", i64 0, i64 0)
  store i64 0, ptr %"%result", align 8
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

then_345:                                         ; preds = %entry
  ret i64 -1
}

define i64 @hub_clear_port_feature(i64 %hub_idx, i64 %port, i64 %feature) {
entry:
  %"%result" = alloca i64, align 8
  %"%addr" = alloca i64, align 8
  %"%hc" = alloca i64, align 8
  %feature3 = alloca i64, align 8
  store i64 %feature, ptr %feature3, align 8
  %port2 = alloca i64, align 8
  store i64 %port, ptr %port2, align 8
  %hub_idx1 = alloca i64, align 8
  store i64 %hub_idx, ptr %hub_idx1, align 8
  %"%hub_idx.load" = load i64, ptr %hub_idx1, align 8
  %calltmp = call i64 @hub_get_active(i64 %"%hub_idx.load")
  %cmpeqtmp = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp, label %then_347, label %merge_348

merge_348:                                        ; preds = %entry
  %"%hub_idx.load4" = load i64, ptr %hub_idx1, align 8
  %calltmp5 = call i64 @hub_get_hc(i64 %"%hub_idx.load4")
  store i64 %calltmp5, ptr %"%hc", align 8
  %"%hub_idx.load6" = load i64, ptr %hub_idx1, align 8
  %calltmp7 = call i64 @hub_get_addr(i64 %"%hub_idx.load6")
  store i64 %calltmp7, ptr %"%addr", align 8
  %"%hub_setup_buf.load" = load i64, ptr @hub_setup_buf, align 8
  %"%PORT_REQ_TYPE_CLR.load" = load i64, ptr @PORT_REQ_TYPE_CLR, align 8
  %"%CLEAR_FEATURE.load" = load i64, ptr @CLEAR_FEATURE, align 8
  %"%feature.load" = load i64, ptr %feature3, align 8
  %"%port.load" = load i64, ptr %port2, align 8
  call void @usb_build_setup(i64 %"%hub_setup_buf.load", i64 %"%PORT_REQ_TYPE_CLR.load", i64 %"%CLEAR_FEATURE.load", i64 %"%feature.load", i64 %"%port.load", i64 0)
  %"%hc.load" = load i64, ptr %"%hc", align 8
  %"%addr.load" = load i64, ptr %"%addr", align 8
  %"%hub_setup_buf.load8" = load i64, ptr @hub_setup_buf, align 8
  call void @uhci_control_transfer(i64 %"%hc.load", i64 %"%addr.load", i64 %"%hub_setup_buf.load8", i64 0, i64 0)
  store i64 0, ptr %"%result", align 8
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

then_347:                                         ; preds = %entry
  ret i64 -1
}

define i64 @hub_power_on_ports(i64 %hub_idx) {
entry:
  %"%start" = alloca i64, align 8
  %"%wait_ticks" = alloca i64, align 8
  %"%p" = alloca i64, align 8
  %"%pwr_delay" = alloca i64, align 8
  %"%num_ports" = alloca i64, align 8
  %hub_idx1 = alloca i64, align 8
  store i64 %hub_idx, ptr %hub_idx1, align 8
  %"%hub_idx.load" = load i64, ptr %hub_idx1, align 8
  %calltmp = call i64 @hub_get_active(i64 %"%hub_idx.load")
  %cmpeqtmp = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp, label %then_349, label %merge_350

merge_350:                                        ; preds = %entry
  %"%hub_idx.load2" = load i64, ptr %hub_idx1, align 8
  %calltmp3 = call i64 @hub_get_num_ports(i64 %"%hub_idx.load2")
  store i64 %calltmp3, ptr %"%num_ports", align 8
  %"%hub_idx.load4" = load i64, ptr %hub_idx1, align 8
  %calltmp5 = call i64 @hub_get_pwr_delay(i64 %"%hub_idx.load4")
  store i64 %calltmp5, ptr %"%pwr_delay", align 8
  %calltmp6 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%960.load" = load i64, ptr %"%num_ports", align 8
  %calltmp7 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%960.load")
  %calltmp8 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %"%965.load" = load i64, ptr %hub_idx1, align 8
  %calltmp9 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%965.load")
  %calltmp10 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 1, ptr %"%p", align 8
  br label %while_cond_351

then_349:                                         ; preds = %entry
  ret i64 -1

while_body_352:                                   ; preds = %while_cond_351
  %"%hub_idx.load11" = load i64, ptr %hub_idx1, align 8
  %"%p.load12" = load i64, ptr %"%p", align 8
  %"%PORT_POWER.load" = load i64, ptr @PORT_POWER, align 8
  %calltmp13 = call i64 @hub_set_port_feature(i64 %"%hub_idx.load11", i64 %"%p.load12", i64 %"%PORT_POWER.load")
  %"%p.load14" = load i64, ptr %"%p", align 8
  %addtmp = add i64 %"%p.load14", 1
  store i64 %addtmp, ptr %"%p", align 8
  br label %while_cond_351

while_body_355:                                   ; preds = %while_cond_354
  br label %while_cond_354

while_cond_351:                                   ; preds = %while_body_352, %merge_350
  %"%p.load" = load i64, ptr %"%p", align 8
  %"%num_ports.load" = load i64, ptr %"%num_ports", align 8
  %cmpletmp = icmp sle i64 %"%p.load", %"%num_ports.load"
  br i1 %cmpletmp, label %while_body_352, label %while_exit_353

while_cond_354:                                   ; preds = %while_body_355, %while_exit_353
  call void @sad_ll_timer_get_ticks()
  %"%start.load" = load i64, ptr %"%start", align 8
  %subtmp = sub i64 0, %"%start.load"
  %"%wait_ticks.load" = load i64, ptr %"%wait_ticks", align 8
  %cmplttmp = icmp slt i64 %subtmp, %"%wait_ticks.load"
  br i1 %cmplttmp, label %while_body_355, label %while_exit_356

while_exit_353:                                   ; preds = %while_cond_351
  %"%pwr_delay.load" = load i64, ptr %"%pwr_delay", align 8
  %shltmp = shl i64 %"%pwr_delay.load", 1
  %addtmp15 = add i64 %shltmp, 10
  store i64 %addtmp15, ptr %"%wait_ticks", align 8
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%start", align 8
  br label %while_cond_354

while_exit_356:                                   ; preds = %while_cond_354
  %calltmp16 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  %calltmp17 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @hub_port_reset(i64 %hub_idx, i64 %port) {
entry:
  %"%change" = alloca i64, align 8
  %"%hub_errors" = alloca i64, align 8
  %"%status_change" = alloca i64, align 8
  %"%reset_done" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  %port2 = alloca i64, align 8
  store i64 %port, ptr %port2, align 8
  %hub_idx1 = alloca i64, align 8
  store i64 %hub_idx, ptr %hub_idx1, align 8
  %calltmp = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %"%992.load" = load i64, ptr %port2, align 8
  %calltmp3 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%992.load")
  %calltmp4 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %"%997.load" = load i64, ptr %hub_idx1, align 8
  %calltmp5 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%997.load")
  %calltmp6 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hub_idx.load" = load i64, ptr %hub_idx1, align 8
  %"%port.load" = load i64, ptr %port2, align 8
  %"%PORT_RESET.load" = load i64, ptr @PORT_RESET, align 8
  %calltmp7 = call i64 @hub_set_port_feature(i64 %"%hub_idx.load", i64 %"%port.load", i64 %"%PORT_RESET.load")
  store i64 100, ptr %"%timeout", align 8
  store i64 0, ptr %"%reset_done", align 8
  br label %while_cond_357

merge_361:                                        ; preds = %while_body_358
  %"%status_change.load11" = load i64, ptr %"%status_change", align 8
  %shrtmp = lshr i64 %"%status_change.load11", 16
  store i64 %shrtmp, ptr %"%change", align 8
  %"%change.load" = load i64, ptr %"%change", align 8
  %"%PORT_CHANGE_RESET.load" = load i64, ptr @PORT_CHANGE_RESET, align 8
  %divtmp = sdiv i64 %"%change.load", %"%PORT_CHANGE_RESET.load"
  %andtmp = and i64 %divtmp, 1
  %cmpeqtmp12 = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp12, label %then_362, label %merge_363

merge_363:                                        ; preds = %then_362, %merge_361
  %"%timeout.load13" = load i64, ptr %"%timeout", align 8
  %subtmp = sub i64 %"%timeout.load13", 1
  store i64 %subtmp, ptr %"%timeout", align 8
  br label %while_cond_357

merge_365:                                        ; preds = %while_exit_359
  %"%hub_idx.load16" = load i64, ptr %hub_idx1, align 8
  %"%port.load17" = load i64, ptr %port2, align 8
  %"%C_PORT_RESET.load" = load i64, ptr @C_PORT_RESET, align 8
  %calltmp18 = call i64 @hub_clear_port_feature(i64 %"%hub_idx.load16", i64 %"%port.load17", i64 %"%C_PORT_RESET.load")
  %calltmp19 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %"%1043.load" = load i64, ptr %port2, align 8
  %calltmp20 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%1043.load")
  %calltmp21 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %calltmp22 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_360:                                         ; preds = %while_body_358
  %"%hub_errors.load" = load i64, ptr @hub_errors, align 8
  %addtmp = add i64 %"%hub_errors.load", 1
  store i64 %addtmp, ptr %"%hub_errors", align 8
  ret i64 -1

then_362:                                         ; preds = %merge_361
  store i64 1, ptr %"%reset_done", align 8
  br label %merge_363

then_364:                                         ; preds = %while_exit_359
  %calltmp14 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  %calltmp15 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -2

while_body_358:                                   ; preds = %while_cond_357
  %"%hub_idx.load8" = load i64, ptr %hub_idx1, align 8
  %"%port.load9" = load i64, ptr %port2, align 8
  %calltmp10 = call i64 @hub_get_port_status(i64 %"%hub_idx.load8", i64 %"%port.load9")
  store i64 %calltmp10, ptr %"%status_change", align 8
  %"%status_change.load" = load i64, ptr %"%status_change", align 8
  %cmplttmp = icmp slt i64 %"%status_change.load", 0
  br i1 %cmplttmp, label %then_360, label %merge_361

while_cond_357:                                   ; preds = %merge_363, %entry
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp, label %while_body_358, label %while_exit_359

while_exit_359:                                   ; preds = %while_cond_357
  %"%reset_done.load" = load i64, ptr %"%reset_done", align 8
  %cmpeqtmp = icmp eq i64 %"%reset_done.load", 0
  br i1 %cmpeqtmp, label %then_364, label %merge_365
}

define i64 @hub_handle_attach(i64 %hub_idx, i64 %port) {
entry:
  %"%hub_errors" = alloca i64, align 8
  %"%hub_attach_count" = alloca i64, align 8
  %"%new_dev" = alloca i64, align 8
  %"%hc" = alloca i64, align 8
  %"%speed" = alloca i64, align 8
  %"%port_status" = alloca i64, align 8
  %"%status" = alloca i64, align 8
  %"%reset_result" = alloca i64, align 8
  %port2 = alloca i64, align 8
  store i64 %port, ptr %port2, align 8
  %hub_idx1 = alloca i64, align 8
  store i64 %hub_idx, ptr %hub_idx1, align 8
  %calltmp = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %"%1052.load" = load i64, ptr %hub_idx1, align 8
  %calltmp3 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%1052.load")
  %calltmp4 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %"%1057.load" = load i64, ptr %port2, align 8
  %calltmp5 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%1057.load")
  %calltmp6 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hub_idx.load" = load i64, ptr %hub_idx1, align 8
  %"%port.load" = load i64, ptr %port2, align 8
  %calltmp7 = call i64 @hub_port_reset(i64 %"%hub_idx.load", i64 %"%port.load")
  store i64 %calltmp7, ptr %"%reset_result", align 8
  %"%reset_result.load" = load i64, ptr %"%reset_result", align 8
  %cmplttmp = icmp slt i64 %"%reset_result.load", 0
  br i1 %cmplttmp, label %then_366, label %merge_367

else_376:                                         ; preds = %merge_373
  %calltmp31 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  %calltmp32 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hub_errors.load" = load i64, ptr @hub_errors, align 8
  %addtmp33 = add i64 %"%hub_errors.load", 1
  store i64 %addtmp33, ptr %"%hub_errors", align 8
  br label %merge_375

merge_367:                                        ; preds = %entry
  %"%hub_idx.load8" = load i64, ptr %hub_idx1, align 8
  %"%port.load9" = load i64, ptr %port2, align 8
  %calltmp10 = call i64 @hub_get_port_status(i64 %"%hub_idx.load8", i64 %"%port.load9")
  store i64 %calltmp10, ptr %"%status", align 8
  %"%status.load" = load i64, ptr %"%status", align 8
  %cmplttmp11 = icmp slt i64 %"%status.load", 0
  br i1 %cmplttmp11, label %then_368, label %merge_369

merge_369:                                        ; preds = %merge_367
  %"%status.load12" = load i64, ptr %"%status", align 8
  %andtmp = and i64 %"%status.load12", 65535
  store i64 %andtmp, ptr %"%port_status", align 8
  store i64 1, ptr %"%speed", align 8
  %"%port_status.load" = load i64, ptr %"%port_status", align 8
  %"%PORT_STATUS_LOW_SPEED.load" = load i64, ptr @PORT_STATUS_LOW_SPEED, align 8
  %divtmp = sdiv i64 %"%port_status.load", %"%PORT_STATUS_LOW_SPEED.load"
  %andtmp13 = and i64 %divtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp13, 1
  br i1 %cmpeqtmp, label %then_370, label %merge_371

merge_371:                                        ; preds = %then_370, %merge_369
  %"%port_status.load14" = load i64, ptr %"%port_status", align 8
  %"%PORT_STATUS_HIGH_SPEED.load" = load i64, ptr @PORT_STATUS_HIGH_SPEED, align 8
  %divtmp15 = sdiv i64 %"%port_status.load14", %"%PORT_STATUS_HIGH_SPEED.load"
  %andtmp16 = and i64 %divtmp15, 1
  %cmpeqtmp17 = icmp eq i64 %andtmp16, 1
  br i1 %cmpeqtmp17, label %then_372, label %merge_373

merge_373:                                        ; preds = %then_372, %merge_371
  %"%hub_idx.load18" = load i64, ptr %hub_idx1, align 8
  %calltmp19 = call i64 @hub_get_hc(i64 %"%hub_idx.load18")
  store i64 %calltmp19, ptr %"%hc", align 8
  %"%hc.load" = load i64, ptr %"%hc", align 8
  %"%port.load20" = load i64, ptr %port2, align 8
  %"%speed.load" = load i64, ptr %"%speed", align 8
  call void @usb_register_device(i64 %"%hc.load", i64 %"%port.load20", i64 %"%speed.load", i64 0, i64 0, i64 0, i64 0)
  store i64 0, ptr %"%new_dev", align 8
  %"%new_dev.load" = load i64, ptr %"%new_dev", align 8
  %cmpgetmp = icmp sge i64 %"%new_dev.load", 0
  br i1 %cmpgetmp, label %then_374, label %else_376

merge_375:                                        ; preds = %else_376, %then_374
  %"%hub_idx.load34" = load i64, ptr %hub_idx1, align 8
  %"%port.load35" = load i64, ptr %port2, align 8
  %"%C_PORT_CONNECTION.load" = load i64, ptr @C_PORT_CONNECTION, align 8
  %calltmp36 = call i64 @hub_clear_port_feature(i64 %"%hub_idx.load34", i64 %"%port.load35", i64 %"%C_PORT_CONNECTION.load")
  %loadtmp = load i64, ptr %"%new_dev", align 8
  ret i64 %loadtmp

then_366:                                         ; preds = %entry
  ret i64 -1

then_368:                                         ; preds = %merge_367
  ret i64 -2

then_370:                                         ; preds = %merge_369
  store i64 0, ptr %"%speed", align 8
  br label %merge_371

then_372:                                         ; preds = %merge_371
  store i64 2, ptr %"%speed", align 8
  br label %merge_373

then_374:                                         ; preds = %merge_373
  %"%port.load21" = load i64, ptr %port2, align 8
  %subtmp = sub i64 %"%port.load21", 1
  %"%new_dev.load22" = load i64, ptr %"%new_dev", align 8
  %addtmp = add i64 %"%new_dev.load22", 1
  %"%hub_idx.load23" = load i64, ptr %hub_idx1, align 8
  %calltmp24 = call i64 @hub_set_port_dev(i64 %"%hub_idx.load23", i64 %subtmp, i64 %addtmp)
  %"%hub_attach_count.load" = load i64, ptr @hub_attach_count, align 8
  %addtmp25 = add i64 %"%hub_attach_count.load", 1
  store i64 %addtmp25, ptr %"%hub_attach_count", align 8
  %calltmp26 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  %"%1115.load" = load i64, ptr %"%new_dev", align 8
  %calltmp27 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%1115.load")
  %calltmp28 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%1120.load" = load i64, ptr %"%speed", align 8
  %calltmp29 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%1120.load")
  %calltmp30 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_375
}

define i64 @hub_handle_detach(i64 %hub_idx, i64 %port) {
entry:
  %"%hub_detach_count" = alloca i64, align 8
  %port2 = alloca i64, align 8
  store i64 %port, ptr %port2, align 8
  %hub_idx1 = alloca i64, align 8
  store i64 %hub_idx, ptr %hub_idx1, align 8
  %calltmp = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %"%1135.load" = load i64, ptr %hub_idx1, align 8
  %calltmp3 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%1135.load")
  %calltmp4 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %"%1140.load" = load i64, ptr %port2, align 8
  %calltmp5 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%1140.load")
  %calltmp6 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  %"%port.load" = load i64, ptr %port2, align 8
  %subtmp = sub i64 %"%port.load", 1
  %"%hub_idx.load" = load i64, ptr %hub_idx1, align 8
  %calltmp7 = call i64 @hub_set_port_dev(i64 %"%hub_idx.load", i64 %subtmp, i64 0)
  %"%hub_detach_count.load" = load i64, ptr @hub_detach_count, align 8
  %addtmp = add i64 %"%hub_detach_count.load", 1
  store i64 %addtmp, ptr %"%hub_detach_count", align 8
  %"%hub_idx.load8" = load i64, ptr %hub_idx1, align 8
  %"%port.load9" = load i64, ptr %port2, align 8
  %"%C_PORT_CONNECTION.load" = load i64, ptr @C_PORT_CONNECTION, align 8
  %calltmp10 = call i64 @hub_clear_port_feature(i64 %"%hub_idx.load8", i64 %"%port.load9", i64 %"%C_PORT_CONNECTION.load")
  %"%hub_idx.load11" = load i64, ptr %hub_idx1, align 8
  %"%port.load12" = load i64, ptr %port2, align 8
  %"%C_PORT_ENABLE.load" = load i64, ptr @C_PORT_ENABLE, align 8
  %calltmp13 = call i64 @hub_clear_port_feature(i64 %"%hub_idx.load11", i64 %"%port.load12", i64 %"%C_PORT_ENABLE.load")
  ret i64 0
}

define i64 @hub_poll(i64 %hub_idx) {
entry:
  %"%hub_errors" = alloca i64, align 8
  %"%port_change" = alloca i64, align 8
  %"%port_status" = alloca i64, align 8
  %"%status_change" = alloca i64, align 8
  %"%p" = alloca i64, align 8
  %"%changes" = alloca i64, align 8
  %"%num_ports" = alloca i64, align 8
  %"%hub_poll_count" = alloca i64, align 8
  %hub_idx1 = alloca i64, align 8
  store i64 %hub_idx, ptr %hub_idx1, align 8
  %"%hub_idx.load" = load i64, ptr %hub_idx1, align 8
  %calltmp = call i64 @hub_get_active(i64 %"%hub_idx.load")
  %cmpeqtmp = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp, label %then_377, label %merge_378

else_388:                                         ; preds = %then_384
  %"%hub_idx.load23" = load i64, ptr %hub_idx1, align 8
  %"%p.load24" = load i64, ptr %"%p", align 8
  %calltmp25 = call i64 @hub_handle_detach(i64 %"%hub_idx.load23", i64 %"%p.load24")
  br label %merge_387

merge_378:                                        ; preds = %entry
  %"%hub_poll_count.load" = load i64, ptr @hub_poll_count, align 8
  %addtmp = add i64 %"%hub_poll_count.load", 1
  store i64 %addtmp, ptr %"%hub_poll_count", align 8
  %"%hub_idx.load2" = load i64, ptr %hub_idx1, align 8
  %calltmp3 = call i64 @hub_get_num_ports(i64 %"%hub_idx.load2")
  store i64 %calltmp3, ptr %"%num_ports", align 8
  store i64 0, ptr %"%changes", align 8
  store i64 1, ptr %"%p", align 8
  br label %while_cond_379

merge_383:                                        ; preds = %merge_390, %while_body_380
  %"%p.load11" = load i64, ptr %"%p", align 8
  %addtmp12 = add i64 %"%p.load11", 1
  store i64 %addtmp12, ptr %"%p", align 8
  br label %while_cond_379

merge_385:                                        ; preds = %merge_387, %then_382
  %"%port_change.load16" = load i64, ptr %"%port_change", align 8
  %"%PORT_CHANGE_OVERCURRENT.load" = load i64, ptr @PORT_CHANGE_OVERCURRENT, align 8
  %divtmp17 = sdiv i64 %"%port_change.load16", %"%PORT_CHANGE_OVERCURRENT.load"
  %andtmp18 = and i64 %divtmp17, 1
  %cmpeqtmp19 = icmp eq i64 %andtmp18, 1
  br i1 %cmpeqtmp19, label %then_389, label %merge_390

merge_387:                                        ; preds = %else_388, %then_386
  %"%changes.load" = load i64, ptr %"%changes", align 8
  %addtmp26 = add i64 %"%changes.load", 1
  store i64 %addtmp26, ptr %"%changes", align 8
  br label %merge_385

merge_390:                                        ; preds = %then_389, %merge_385
  br label %merge_383

then_377:                                         ; preds = %entry
  ret i64 0

then_382:                                         ; preds = %while_body_380
  %"%status_change.load7" = load i64, ptr %"%status_change", align 8
  %andtmp = and i64 %"%status_change.load7", 65535
  store i64 %andtmp, ptr %"%port_status", align 8
  %"%status_change.load8" = load i64, ptr %"%status_change", align 8
  %shrtmp = lshr i64 %"%status_change.load8", 16
  store i64 %shrtmp, ptr %"%port_change", align 8
  %"%port_change.load" = load i64, ptr %"%port_change", align 8
  %"%PORT_CHANGE_CONNECTION.load" = load i64, ptr @PORT_CHANGE_CONNECTION, align 8
  %divtmp = sdiv i64 %"%port_change.load", %"%PORT_CHANGE_CONNECTION.load"
  %andtmp9 = and i64 %divtmp, 1
  %cmpeqtmp10 = icmp eq i64 %andtmp9, 1
  br i1 %cmpeqtmp10, label %then_384, label %merge_385

then_384:                                         ; preds = %then_382
  %"%port_status.load" = load i64, ptr %"%port_status", align 8
  %"%PORT_STATUS_CONNECTION.load" = load i64, ptr @PORT_STATUS_CONNECTION, align 8
  %divtmp13 = sdiv i64 %"%port_status.load", %"%PORT_STATUS_CONNECTION.load"
  %andtmp14 = and i64 %divtmp13, 1
  %cmpeqtmp15 = icmp eq i64 %andtmp14, 1
  br i1 %cmpeqtmp15, label %then_386, label %else_388

then_386:                                         ; preds = %then_384
  %"%hub_idx.load20" = load i64, ptr %hub_idx1, align 8
  %"%p.load21" = load i64, ptr %"%p", align 8
  %calltmp22 = call i64 @hub_handle_attach(i64 %"%hub_idx.load20", i64 %"%p.load21")
  br label %merge_387

then_389:                                         ; preds = %merge_385
  %calltmp27 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  %"%1199.load" = load i64, ptr %"%p", align 8
  %calltmp28 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%1199.load")
  %calltmp29 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hub_idx.load30" = load i64, ptr %hub_idx1, align 8
  %"%p.load31" = load i64, ptr %"%p", align 8
  %"%C_PORT_OVER_CURRENT.load" = load i64, ptr @C_PORT_OVER_CURRENT, align 8
  %calltmp32 = call i64 @hub_clear_port_feature(i64 %"%hub_idx.load30", i64 %"%p.load31", i64 %"%C_PORT_OVER_CURRENT.load")
  %"%hub_errors.load" = load i64, ptr @hub_errors, align 8
  %addtmp33 = add i64 %"%hub_errors.load", 1
  store i64 %addtmp33, ptr %"%hub_errors", align 8
  br label %merge_390

while_body_380:                                   ; preds = %while_cond_379
  %"%hub_idx.load4" = load i64, ptr %hub_idx1, align 8
  %"%p.load5" = load i64, ptr %"%p", align 8
  %calltmp6 = call i64 @hub_get_port_status(i64 %"%hub_idx.load4", i64 %"%p.load5")
  store i64 %calltmp6, ptr %"%status_change", align 8
  %"%status_change.load" = load i64, ptr %"%status_change", align 8
  %cmpgetmp = icmp sge i64 %"%status_change.load", 0
  br i1 %cmpgetmp, label %then_382, label %merge_383

while_cond_379:                                   ; preds = %merge_383, %merge_378
  %"%p.load" = load i64, ptr %"%p", align 8
  %"%num_ports.load" = load i64, ptr %"%num_ports", align 8
  %cmpletmp = icmp sle i64 %"%p.load", %"%num_ports.load"
  br i1 %cmpletmp, label %while_body_380, label %while_exit_381

while_exit_381:                                   ; preds = %while_cond_379
  %loadtmp = load i64, ptr %"%changes", align 8
  ret i64 %loadtmp
}

define i64 @hub_register(i64 %hc_idx, i64 %dev_addr) {
entry:
  %"%hub_count" = alloca i64, align 8
  %"%idx" = alloca i64, align 8
  %dev_addr2 = alloca i64, align 8
  store i64 %dev_addr, ptr %dev_addr2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hub_count.load" = load i64, ptr @hub_count, align 8
  %"%MAX_HUBS.load" = load i64, ptr @MAX_HUBS, align 8
  %cmpgetmp = icmp sge i64 %"%hub_count.load", %"%MAX_HUBS.load"
  br i1 %cmpgetmp, label %then_391, label %merge_392

merge_392:                                        ; preds = %entry
  %"%1219.load" = load i64, ptr @hub_count, align 8
  store i64 %"%1219.load", ptr %"%idx", align 8
  %"%idx.load" = load i64, ptr %"%idx", align 8
  %calltmp4 = call i64 @hub_set_active(i64 %"%idx.load", i64 1)
  %"%idx.load5" = load i64, ptr %"%idx", align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp6 = call i64 @hub_set_hc(i64 %"%idx.load5", i64 %"%hc_idx.load")
  %"%idx.load7" = load i64, ptr %"%idx", align 8
  %"%dev_addr.load" = load i64, ptr %dev_addr2, align 8
  %calltmp8 = call i64 @hub_set_addr(i64 %"%idx.load7", i64 %"%dev_addr.load")
  %calltmp9 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  %"%1227.load" = load i64, ptr %"%idx", align 8
  %calltmp10 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%1227.load")
  %calltmp11 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  %"%1232.load" = load i64, ptr %dev_addr2, align 8
  %calltmp12 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%1232.load")
  %calltmp13 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  %"%idx.load14" = load i64, ptr %"%idx", align 8
  %calltmp15 = call i64 @hub_get_descriptor(i64 %"%idx.load14")
  %"%idx.load16" = load i64, ptr %"%idx", align 8
  %calltmp17 = call i64 @hub_power_on_ports(i64 %"%idx.load16")
  %"%idx.load18" = load i64, ptr %"%idx", align 8
  %calltmp19 = call i64 @hub_poll(i64 %"%idx.load18")
  %"%hub_count.load20" = load i64, ptr @hub_count, align 8
  %addtmp = add i64 %"%hub_count.load20", 1
  store i64 %addtmp, ptr %"%hub_count", align 8
  %loadtmp = load i64, ptr %"%idx", align 8
  ret i64 %loadtmp

then_391:                                         ; preds = %entry
  %calltmp = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  %calltmp3 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -1
}

define i64 @"hub_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %"%dev" = alloca i64, align 8
  %"%pp" = alloca i64, align 8
  %"%np" = alloca i64, align 8
  %"%h" = alloca i64, align 8
  %calltmp = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  %calltmp1 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %"%1249.load" = load i64, ptr @hub_count, align 8
  %calltmp3 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%1249.load")
  %calltmp4 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%h", align 8
  br label %while_cond_393

merge_397:                                        ; preds = %while_exit_400, %while_body_394
  %"%h.load32" = load i64, ptr %"%h", align 8
  %addtmp = add i64 %"%h.load32", 1
  store i64 %addtmp, ptr %"%h", align 8
  br label %while_cond_393

merge_402:                                        ; preds = %then_401, %while_body_399
  %"%pp.load44" = load i64, ptr %"%pp", align 8
  %addtmp45 = add i64 %"%pp.load44", 1
  store i64 %addtmp45, ptr %"%pp", align 8
  br label %while_cond_398

then_396:                                         ; preds = %while_body_394
  %calltmp19 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.30 to i64))
  %"%1262.load" = load i64, ptr %"%h", align 8
  %calltmp20 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%1262.load")
  %calltmp21 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  %"%h.load22" = load i64, ptr %"%h", align 8
  %calltmp23 = call i64 @hub_get_addr(i64 %"%h.load22")
  %calltmp24 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp23)
  %calltmp25 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %"%h.load26" = load i64, ptr %"%h", align 8
  %calltmp27 = call i64 @hub_get_num_ports(i64 %"%h.load26")
  %calltmp28 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp27)
  %calltmp29 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  %"%h.load30" = load i64, ptr %"%h", align 8
  %calltmp31 = call i64 @hub_get_num_ports(i64 %"%h.load30")
  store i64 %calltmp31, ptr %"%np", align 8
  store i64 0, ptr %"%pp", align 8
  br label %while_cond_398

then_401:                                         ; preds = %while_body_399
  %calltmp37 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  %"%pp.load38" = load i64, ptr %"%pp", align 8
  %addtmp39 = add i64 %"%pp.load38", 1
  %calltmp40 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %addtmp39)
  %calltmp41 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.34 to i64))
  %"%1297.load" = load i64, ptr %"%dev", align 8
  %calltmp42 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%1297.load")
  %calltmp43 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_402

while_body_394:                                   ; preds = %while_cond_393
  %"%h.load5" = load i64, ptr %"%h", align 8
  %calltmp6 = call i64 @hub_get_active(i64 %"%h.load5")
  %cmpeqtmp = icmp eq i64 %calltmp6, 1
  br i1 %cmpeqtmp, label %then_396, label %merge_397

while_body_399:                                   ; preds = %while_cond_398
  %"%h.load34" = load i64, ptr %"%h", align 8
  %"%pp.load35" = load i64, ptr %"%pp", align 8
  %calltmp36 = call i64 @hub_get_port_dev(i64 %"%h.load34", i64 %"%pp.load35")
  store i64 %calltmp36, ptr %"%dev", align 8
  %"%dev.load" = load i64, ptr %"%dev", align 8
  %cmpgttmp = icmp sgt i64 %"%dev.load", 0
  br i1 %cmpgttmp, label %then_401, label %merge_402

while_cond_393:                                   ; preds = %merge_397, %entry
  %"%h.load" = load i64, ptr %"%h", align 8
  %"%hub_count.load" = load i64, ptr @hub_count, align 8
  %cmplttmp = icmp slt i64 %"%h.load", %"%hub_count.load"
  br i1 %cmplttmp, label %while_body_394, label %while_exit_395

while_cond_398:                                   ; preds = %merge_402, %then_396
  %"%pp.load" = load i64, ptr %"%pp", align 8
  %"%np.load" = load i64, ptr %"%np", align 8
  %cmplttmp33 = icmp slt i64 %"%pp.load", %"%np.load"
  br i1 %cmplttmp33, label %while_body_399, label %while_exit_400

while_exit_395:                                   ; preds = %while_cond_393
  %calltmp7 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  %"%1307.load" = load i64, ptr @hub_attach_count, align 8
  %calltmp8 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%1307.load")
  %calltmp9 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  %"%1312.load" = load i64, ptr @hub_detach_count, align 8
  %calltmp10 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%1312.load")
  %calltmp11 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp12 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  %"%1318.load" = load i64, ptr @hub_poll_count, align 8
  %calltmp13 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%1318.load")
  %calltmp14 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  %"%1323.load" = load i64, ptr @hub_errors, align 8
  %calltmp15 = call i64 @"hub_serial_\D8\B1\D9\82\D9\85"(i64 %"%1323.load")
  %calltmp16 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp17 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  %calltmp18 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

while_exit_400:                                   ; preds = %while_cond_398
  br label %merge_397
}

define i64 @"hub_\D9\87\D9\8A\D8\A6"() {
entry:
  %"%hub_errors" = alloca i64, align 8
  %"%hub_poll_count" = alloca i64, align 8
  %"%hub_detach_count" = alloca i64, align 8
  %"%hub_attach_count" = alloca i64, align 8
  %"%hub_count" = alloca i64, align 8
  %"%hub_data_buf" = alloca i64, align 8
  %"%hub_setup_buf" = alloca i64, align 8
  %calltmp = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.35 to i64))
  %calltmp1 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  call void @sad_ll_kmalloc(i64 16)
  store i64 0, ptr %"%hub_setup_buf", align 8
  call void @sad_ll_kmalloc(i64 64)
  store i64 0, ptr %"%hub_data_buf", align 8
  %"%hub_setup_buf.load" = load i64, ptr %"%hub_setup_buf", align 8
  call void @sad_ll_memset(i64 %"%hub_setup_buf.load", i64 0, i64 16)
  %"%hub_data_buf.load" = load i64, ptr %"%hub_data_buf", align 8
  call void @sad_ll_memset(i64 %"%hub_data_buf.load", i64 0, i64 64)
  store i64 0, ptr %"%hub_count", align 8
  store i64 0, ptr %"%hub_attach_count", align 8
  store i64 0, ptr %"%hub_detach_count", align 8
  store i64 0, ptr %"%hub_poll_count", align 8
  store i64 0, ptr %"%hub_errors", align 8
  %calltmp2 = call i64 @"hub_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.36 to i64))
  %calltmp3 = call i64 @"hub_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @"hub_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  %"%h" = alloca i64, align 8
  store i64 0, ptr %"%h", align 8
  br label %while_cond_403

while_body_404:                                   ; preds = %while_cond_403
  %"%h.load1" = load i64, ptr %"%h", align 8
  %calltmp = call i64 @hub_poll(i64 %"%h.load1")
  %"%h.load2" = load i64, ptr %"%h", align 8
  %addtmp = add i64 %"%h.load2", 1
  store i64 %addtmp, ptr %"%h", align 8
  br label %while_cond_403

while_cond_403:                                   ; preds = %while_body_404, %entry
  %"%h.load" = load i64, ptr %"%h", align 8
  %"%hub_count.load" = load i64, ptr @hub_count, align 8
  %cmplttmp = icmp slt i64 %"%h.load", %"%hub_count.load"
  br i1 %cmplttmp, label %while_body_404, label %while_exit_405

while_exit_405:                                   ; preds = %while_cond_403
  ret i64 0
}

define void @__sad_main() {
entry:
  %"%hub_errors" = alloca i64, align 8
  %"%hub_poll_count" = alloca i64, align 8
  %"%hub_detach_count" = alloca i64, align 8
  %"%hub_attach_count" = alloca i64, align 8
  %"%hub_data_buf" = alloca i64, align 8
  %"%hub_setup_buf" = alloca i64, align 8
  %"%hub_count" = alloca i64, align 8
  %"%port_dev_37" = alloca i64, align 8
  %"%port_dev_36" = alloca i64, align 8
  %"%port_dev_35" = alloca i64, align 8
  %"%port_dev_34" = alloca i64, align 8
  %"%port_dev_33" = alloca i64, align 8
  %"%port_dev_32" = alloca i64, align 8
  %"%port_dev_31" = alloca i64, align 8
  %"%port_dev_30" = alloca i64, align 8
  %"%port_dev_27" = alloca i64, align 8
  %"%port_dev_26" = alloca i64, align 8
  %"%port_dev_25" = alloca i64, align 8
  %"%port_dev_24" = alloca i64, align 8
  %"%port_dev_23" = alloca i64, align 8
  %"%port_dev_22" = alloca i64, align 8
  %"%port_dev_21" = alloca i64, align 8
  %"%port_dev_20" = alloca i64, align 8
  %"%port_dev_17" = alloca i64, align 8
  %"%port_dev_16" = alloca i64, align 8
  %"%port_dev_15" = alloca i64, align 8
  %"%port_dev_14" = alloca i64, align 8
  %"%port_dev_13" = alloca i64, align 8
  %"%port_dev_12" = alloca i64, align 8
  %"%port_dev_11" = alloca i64, align 8
  %"%port_dev_10" = alloca i64, align 8
  %"%port_dev_07" = alloca i64, align 8
  %"%port_dev_06" = alloca i64, align 8
  %"%port_dev_05" = alloca i64, align 8
  %"%port_dev_04" = alloca i64, align 8
  %"%port_dev_03" = alloca i64, align 8
  %"%port_dev_02" = alloca i64, align 8
  %"%port_dev_01" = alloca i64, align 8
  %"%port_dev_00" = alloca i64, align 8
  %"%hub_sc_ep_7" = alloca i64, align 8
  %"%hub_sc_ep_6" = alloca i64, align 8
  %"%hub_sc_ep_5" = alloca i64, align 8
  %"%hub_sc_ep_4" = alloca i64, align 8
  %"%hub_sc_ep_3" = alloca i64, align 8
  %"%hub_sc_ep_2" = alloca i64, align 8
  %"%hub_sc_ep_1" = alloca i64, align 8
  %"%hub_sc_ep_0" = alloca i64, align 8
  %"%hub_pwr_delay_7" = alloca i64, align 8
  %"%hub_pwr_delay_6" = alloca i64, align 8
  %"%hub_pwr_delay_5" = alloca i64, align 8
  %"%hub_pwr_delay_4" = alloca i64, align 8
  %"%hub_pwr_delay_3" = alloca i64, align 8
  %"%hub_pwr_delay_2" = alloca i64, align 8
  %"%hub_pwr_delay_1" = alloca i64, align 8
  %"%hub_pwr_delay_0" = alloca i64, align 8
  %"%hub_num_ports_7" = alloca i64, align 8
  %"%hub_num_ports_6" = alloca i64, align 8
  %"%hub_num_ports_5" = alloca i64, align 8
  %"%hub_num_ports_4" = alloca i64, align 8
  %"%hub_num_ports_3" = alloca i64, align 8
  %"%hub_num_ports_2" = alloca i64, align 8
  %"%hub_num_ports_1" = alloca i64, align 8
  %"%hub_num_ports_0" = alloca i64, align 8
  %"%hub_addr_7" = alloca i64, align 8
  %"%hub_addr_6" = alloca i64, align 8
  %"%hub_addr_5" = alloca i64, align 8
  %"%hub_addr_4" = alloca i64, align 8
  %"%hub_addr_3" = alloca i64, align 8
  %"%hub_addr_2" = alloca i64, align 8
  %"%hub_addr_1" = alloca i64, align 8
  %"%hub_addr_0" = alloca i64, align 8
  %"%hub_hc_7" = alloca i64, align 8
  %"%hub_hc_6" = alloca i64, align 8
  %"%hub_hc_5" = alloca i64, align 8
  %"%hub_hc_4" = alloca i64, align 8
  %"%hub_hc_3" = alloca i64, align 8
  %"%hub_hc_2" = alloca i64, align 8
  %"%hub_hc_1" = alloca i64, align 8
  %"%hub_hc_0" = alloca i64, align 8
  %"%hub_active_7" = alloca i64, align 8
  %"%hub_active_6" = alloca i64, align 8
  %"%hub_active_5" = alloca i64, align 8
  %"%hub_active_4" = alloca i64, align 8
  %"%hub_active_3" = alloca i64, align 8
  %"%hub_active_2" = alloca i64, align 8
  %"%hub_active_1" = alloca i64, align 8
  %"%hub_active_0" = alloca i64, align 8
  %"%HUB_DESC_TYPE" = alloca i64, align 8
  %"%PORT_CHANGE_RESET" = alloca i64, align 8
  %"%PORT_CHANGE_OVERCURRENT" = alloca i64, align 8
  %"%PORT_CHANGE_SUSPEND" = alloca i64, align 8
  %"%PORT_CHANGE_ENABLE" = alloca i64, align 8
  %"%PORT_CHANGE_CONNECTION" = alloca i64, align 8
  %"%PORT_STATUS_HIGH_SPEED" = alloca i64, align 8
  %"%PORT_STATUS_LOW_SPEED" = alloca i64, align 8
  %"%PORT_STATUS_POWERED" = alloca i64, align 8
  %"%PORT_STATUS_RESET" = alloca i64, align 8
  %"%PORT_STATUS_OVERCURRENT" = alloca i64, align 8
  %"%PORT_STATUS_SUSPENDED" = alloca i64, align 8
  %"%PORT_STATUS_ENABLED" = alloca i64, align 8
  %"%PORT_STATUS_CONNECTION" = alloca i64, align 8
  %"%C_PORT_RESET" = alloca i64, align 8
  %"%C_PORT_OVER_CURRENT" = alloca i64, align 8
  %"%C_PORT_SUSPEND" = alloca i64, align 8
  %"%C_PORT_ENABLE" = alloca i64, align 8
  %"%C_PORT_CONNECTION" = alloca i64, align 8
  %"%PORT_LOW_SPEED" = alloca i64, align 8
  %"%PORT_POWER" = alloca i64, align 8
  %"%PORT_RESET" = alloca i64, align 8
  %"%PORT_OVER_CURRENT" = alloca i64, align 8
  %"%PORT_SUSPEND" = alloca i64, align 8
  %"%PORT_ENABLE" = alloca i64, align 8
  %"%PORT_CONNECTION" = alloca i64, align 8
  %"%C_HUB_OVER_CURRENT" = alloca i64, align 8
  %"%C_HUB_LOCAL_POWER" = alloca i64, align 8
  %"%GET_DESCRIPTOR" = alloca i64, align 8
  %"%SET_FEATURE" = alloca i64, align 8
  %"%CLEAR_FEATURE" = alloca i64, align 8
  %"%GET_STATUS" = alloca i64, align 8
  %"%PORT_REQ_TYPE_CLR" = alloca i64, align 8
  %"%PORT_REQ_TYPE_SET" = alloca i64, align 8
  %"%PORT_REQ_TYPE_GET" = alloca i64, align 8
  %"%HUB_REQ_TYPE_SET" = alloca i64, align 8
  %"%HUB_REQ_TYPE_GET" = alloca i64, align 8
  %"%MAX_PORTS_PER_HUB" = alloca i64, align 8
  %"%MAX_HUBS" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 8, ptr %"%MAX_HUBS", align 8
  store i64 8, ptr %"%MAX_PORTS_PER_HUB", align 8
  store i64 160, ptr %"%HUB_REQ_TYPE_GET", align 8
  store i64 32, ptr %"%HUB_REQ_TYPE_SET", align 8
  store i64 163, ptr %"%PORT_REQ_TYPE_GET", align 8
  store i64 35, ptr %"%PORT_REQ_TYPE_SET", align 8
  store i64 35, ptr %"%PORT_REQ_TYPE_CLR", align 8
  store i64 0, ptr %"%GET_STATUS", align 8
  store i64 1, ptr %"%CLEAR_FEATURE", align 8
  store i64 3, ptr %"%SET_FEATURE", align 8
  store i64 6, ptr %"%GET_DESCRIPTOR", align 8
  store i64 0, ptr %"%C_HUB_LOCAL_POWER", align 8
  store i64 1, ptr %"%C_HUB_OVER_CURRENT", align 8
  store i64 0, ptr %"%PORT_CONNECTION", align 8
  store i64 1, ptr %"%PORT_ENABLE", align 8
  store i64 2, ptr %"%PORT_SUSPEND", align 8
  store i64 3, ptr %"%PORT_OVER_CURRENT", align 8
  store i64 4, ptr %"%PORT_RESET", align 8
  store i64 8, ptr %"%PORT_POWER", align 8
  store i64 9, ptr %"%PORT_LOW_SPEED", align 8
  store i64 16, ptr %"%C_PORT_CONNECTION", align 8
  store i64 17, ptr %"%C_PORT_ENABLE", align 8
  store i64 18, ptr %"%C_PORT_SUSPEND", align 8
  store i64 19, ptr %"%C_PORT_OVER_CURRENT", align 8
  store i64 20, ptr %"%C_PORT_RESET", align 8
  store i64 1, ptr %"%PORT_STATUS_CONNECTION", align 8
  store i64 2, ptr %"%PORT_STATUS_ENABLED", align 8
  store i64 4, ptr %"%PORT_STATUS_SUSPENDED", align 8
  store i64 8, ptr %"%PORT_STATUS_OVERCURRENT", align 8
  store i64 16, ptr %"%PORT_STATUS_RESET", align 8
  store i64 256, ptr %"%PORT_STATUS_POWERED", align 8
  store i64 512, ptr %"%PORT_STATUS_LOW_SPEED", align 8
  store i64 1024, ptr %"%PORT_STATUS_HIGH_SPEED", align 8
  store i64 1, ptr %"%PORT_CHANGE_CONNECTION", align 8
  store i64 2, ptr %"%PORT_CHANGE_ENABLE", align 8
  store i64 4, ptr %"%PORT_CHANGE_SUSPEND", align 8
  store i64 8, ptr %"%PORT_CHANGE_OVERCURRENT", align 8
  store i64 16, ptr %"%PORT_CHANGE_RESET", align 8
  store i64 41, ptr %"%HUB_DESC_TYPE", align 8
  store i64 0, ptr %"%hub_active_0", align 8
  store i64 0, ptr %"%hub_active_1", align 8
  store i64 0, ptr %"%hub_active_2", align 8
  store i64 0, ptr %"%hub_active_3", align 8
  store i64 0, ptr %"%hub_active_4", align 8
  store i64 0, ptr %"%hub_active_5", align 8
  store i64 0, ptr %"%hub_active_6", align 8
  store i64 0, ptr %"%hub_active_7", align 8
  store i64 0, ptr %"%hub_hc_0", align 8
  store i64 0, ptr %"%hub_hc_1", align 8
  store i64 0, ptr %"%hub_hc_2", align 8
  store i64 0, ptr %"%hub_hc_3", align 8
  store i64 0, ptr %"%hub_hc_4", align 8
  store i64 0, ptr %"%hub_hc_5", align 8
  store i64 0, ptr %"%hub_hc_6", align 8
  store i64 0, ptr %"%hub_hc_7", align 8
  store i64 0, ptr %"%hub_addr_0", align 8
  store i64 0, ptr %"%hub_addr_1", align 8
  store i64 0, ptr %"%hub_addr_2", align 8
  store i64 0, ptr %"%hub_addr_3", align 8
  store i64 0, ptr %"%hub_addr_4", align 8
  store i64 0, ptr %"%hub_addr_5", align 8
  store i64 0, ptr %"%hub_addr_6", align 8
  store i64 0, ptr %"%hub_addr_7", align 8
  store i64 0, ptr %"%hub_num_ports_0", align 8
  store i64 0, ptr %"%hub_num_ports_1", align 8
  store i64 0, ptr %"%hub_num_ports_2", align 8
  store i64 0, ptr %"%hub_num_ports_3", align 8
  store i64 0, ptr %"%hub_num_ports_4", align 8
  store i64 0, ptr %"%hub_num_ports_5", align 8
  store i64 0, ptr %"%hub_num_ports_6", align 8
  store i64 0, ptr %"%hub_num_ports_7", align 8
  store i64 0, ptr %"%hub_pwr_delay_0", align 8
  store i64 0, ptr %"%hub_pwr_delay_1", align 8
  store i64 0, ptr %"%hub_pwr_delay_2", align 8
  store i64 0, ptr %"%hub_pwr_delay_3", align 8
  store i64 0, ptr %"%hub_pwr_delay_4", align 8
  store i64 0, ptr %"%hub_pwr_delay_5", align 8
  store i64 0, ptr %"%hub_pwr_delay_6", align 8
  store i64 0, ptr %"%hub_pwr_delay_7", align 8
  store i64 1, ptr %"%hub_sc_ep_0", align 8
  store i64 1, ptr %"%hub_sc_ep_1", align 8
  store i64 1, ptr %"%hub_sc_ep_2", align 8
  store i64 1, ptr %"%hub_sc_ep_3", align 8
  store i64 1, ptr %"%hub_sc_ep_4", align 8
  store i64 1, ptr %"%hub_sc_ep_5", align 8
  store i64 1, ptr %"%hub_sc_ep_6", align 8
  store i64 1, ptr %"%hub_sc_ep_7", align 8
  store i64 0, ptr %"%port_dev_00", align 8
  store i64 0, ptr %"%port_dev_01", align 8
  store i64 0, ptr %"%port_dev_02", align 8
  store i64 0, ptr %"%port_dev_03", align 8
  store i64 0, ptr %"%port_dev_04", align 8
  store i64 0, ptr %"%port_dev_05", align 8
  store i64 0, ptr %"%port_dev_06", align 8
  store i64 0, ptr %"%port_dev_07", align 8
  store i64 0, ptr %"%port_dev_10", align 8
  store i64 0, ptr %"%port_dev_11", align 8
  store i64 0, ptr %"%port_dev_12", align 8
  store i64 0, ptr %"%port_dev_13", align 8
  store i64 0, ptr %"%port_dev_14", align 8
  store i64 0, ptr %"%port_dev_15", align 8
  store i64 0, ptr %"%port_dev_16", align 8
  store i64 0, ptr %"%port_dev_17", align 8
  store i64 0, ptr %"%port_dev_20", align 8
  store i64 0, ptr %"%port_dev_21", align 8
  store i64 0, ptr %"%port_dev_22", align 8
  store i64 0, ptr %"%port_dev_23", align 8
  store i64 0, ptr %"%port_dev_24", align 8
  store i64 0, ptr %"%port_dev_25", align 8
  store i64 0, ptr %"%port_dev_26", align 8
  store i64 0, ptr %"%port_dev_27", align 8
  store i64 0, ptr %"%port_dev_30", align 8
  store i64 0, ptr %"%port_dev_31", align 8
  store i64 0, ptr %"%port_dev_32", align 8
  store i64 0, ptr %"%port_dev_33", align 8
  store i64 0, ptr %"%port_dev_34", align 8
  store i64 0, ptr %"%port_dev_35", align 8
  store i64 0, ptr %"%port_dev_36", align 8
  store i64 0, ptr %"%port_dev_37", align 8
  store i64 0, ptr %"%hub_count", align 8
  store i64 0, ptr %"%hub_setup_buf", align 8
  store i64 0, ptr %"%hub_data_buf", align 8
  store i64 0, ptr %"%hub_attach_count", align 8
  store i64 0, ptr %"%hub_detach_count", align 8
  store i64 0, ptr %"%hub_poll_count", align 8
  store i64 0, ptr %"%hub_errors", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
