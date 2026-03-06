; ModuleID = 'usb_ehci_نحلة'
source_filename = "usb_ehci_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@COM1 = constant i64 1016
@EHCI_CAP_CAPLENGTH = constant i64 0
@EHCI_CAP_HCIVERSION = constant i64 2
@EHCI_CAP_HCSPARAMS = constant i64 4
@EHCI_CAP_HCCPARAMS = constant i64 8
@EHCI_OP_USBCMD = constant i64 0
@EHCI_OP_USBSTS = constant i64 4
@EHCI_OP_USBINTR = constant i64 8
@EHCI_OP_FRINDEX = constant i64 12
@EHCI_OP_CTRLDSSEG = constant i64 16
@EHCI_OP_PERIODICLIST = constant i64 20
@EHCI_OP_ASYNCLIST = constant i64 24
@EHCI_OP_CONFIGFLAG = constant i64 64
@EHCI_OP_PORTSC_BASE = constant i64 68
@EHCI_CMD_RS = constant i64 1
@EHCI_CMD_HCRESET = constant i64 2
@EHCI_CMD_FLS_1024 = constant i64 0
@EHCI_CMD_FLS_512 = constant i64 4
@EHCI_CMD_FLS_256 = constant i64 8
@EHCI_CMD_PSE = constant i64 16
@EHCI_CMD_ASE = constant i64 32
@EHCI_CMD_IAAD = constant i64 64
@EHCI_CMD_ITC_1 = constant i64 65536
@EHCI_STS_USBINT = constant i64 1
@EHCI_STS_USBERR = constant i64 2
@EHCI_STS_PCD = constant i64 4
@EHCI_STS_FLR = constant i64 8
@EHCI_STS_HSE = constant i64 16
@EHCI_STS_IAA = constant i64 32
@EHCI_STS_HCH = constant i64 4096
@EHCI_STS_RECL = constant i64 8192
@EHCI_STS_PSS = constant i64 16384
@EHCI_STS_ASS = constant i64 32768
@EHCI_PORT_CONNECT = constant i64 1
@EHCI_PORT_CSC = constant i64 2
@EHCI_PORT_ENABLE = constant i64 4
@EHCI_PORT_ENC = constant i64 8
@EHCI_PORT_OCA = constant i64 16
@EHCI_PORT_OCC = constant i64 32
@EHCI_PORT_FPR = constant i64 64
@EHCI_PORT_SUSPEND = constant i64 128
@EHCI_PORT_RESET = constant i64 256
@EHCI_PORT_LS_MASK = constant i64 3072
@EHCI_PORT_PP = constant i64 4096
@EHCI_PORT_OWNER = constant i64 8192
@EHCI_PORT_PIC_OFF = constant i64 0
@EHCI_PORT_PIC_AMBER = constant i64 16384
@EHCI_PORT_PIC_GREEN = constant i64 32768
@EHCI_QH_RL_SHIFT = constant i64 28
@EHCI_QH_C_BIT = constant i64 134217728
@EHCI_QH_MAX_PKT_SHIFT = constant i64 16
@EHCI_QH_H_BIT = constant i64 32768
@EHCI_QH_DTC = constant i64 16384
@EHCI_QH_EPS_FULL = constant i64 0
@EHCI_QH_EPS_LOW = constant i64 4096
@EHCI_QH_EPS_HIGH = constant i64 8192
@EHCI_QH_EP_SHIFT = constant i64 8
@EHCI_QH_INACT = constant i64 128
@EHCI_QH_ADDR_MASK = constant i64 127
@EHCI_QTD_ACTIVE = constant i64 128
@EHCI_QTD_HALTED = constant i64 64
@EHCI_QTD_DBUF_ERR = constant i64 32
@EHCI_QTD_BABBLE = constant i64 16
@EHCI_QTD_XACT_ERR = constant i64 8
@EHCI_QTD_MISS_MF = constant i64 4
@EHCI_QTD_SPLIT_XS = constant i64 2
@EHCI_QTD_PING = constant i64 1
@EHCI_QTD_PID_OUT = constant i64 0
@EHCI_QTD_PID_IN = constant i64 256
@EHCI_QTD_PID_SETUP = constant i64 512
@EHCI_QTD_TOGGLE = constant i64 2147483648
@ehci_count = global i64 0
@ehci_mmio_0 = global i64 0
@ehci_mmio_1 = global i64 0
@ehci_mmio_2 = global i64 0
@ehci_mmio_3 = global i64 0
@ehci_opbase_0 = global i64 0
@ehci_opbase_1 = global i64 0
@ehci_opbase_2 = global i64 0
@ehci_opbase_3 = global i64 0
@ehci_num_ports_0 = global i64 0
@ehci_num_ports_1 = global i64 0
@ehci_num_ports_2 = global i64 0
@ehci_num_ports_3 = global i64 0
@ehci_periodic_0 = global i64 0
@ehci_periodic_1 = global i64 0
@ehci_periodic_2 = global i64 0
@ehci_periodic_3 = global i64 0
@ehci_qh_pool_0 = global i64 0
@ehci_qh_pool_1 = global i64 0
@ehci_qh_pool_2 = global i64 0
@ehci_qh_pool_3 = global i64 0
@ehci_qtd_pool_0 = global i64 0
@ehci_qtd_pool_1 = global i64 0
@ehci_qtd_pool_2 = global i64 0
@ehci_qtd_pool_3 = global i64 0
@ehci_data_buf_0 = global i64 0
@ehci_data_buf_1 = global i64 0
@ehci_data_buf_2 = global i64 0
@ehci_data_buf_3 = global i64 0
@ehci_setup_buf_0 = global i64 0
@ehci_setup_buf_1 = global i64 0
@ehci_setup_buf_2 = global i64 0
@ehci_setup_buf_3 = global i64 0
@ehci_status_0 = global i64 0
@ehci_status_1 = global i64 0
@ehci_status_2 = global i64 0
@ehci_status_3 = global i64 0
@ehci_qh_used_0 = global i64 0
@ehci_qh_used_1 = global i64 0
@ehci_qh_used_2 = global i64 0
@ehci_qh_used_3 = global i64 0
@ehci_qtd_used_0 = global i64 0
@ehci_qtd_used_1 = global i64 0
@ehci_qtd_used_2 = global i64 0
@ehci_qtd_used_3 = global i64 0
@ehci_async_qh_0 = global i64 0
@ehci_async_qh_1 = global i64 0
@ehci_async_qh_2 = global i64 0
@ehci_async_qh_3 = global i64 0
@ehci_total_transfers = global i64 0
@ehci_transfer_errors = global i64 0
@ehci_port_resets = global i64 0
@ehci_companion_routes = global i64 0
@str.const = private unnamed_addr constant [22 x i8] c"[EHCI] Resetting HC #\00", align 1
@str.const.1 = private unnamed_addr constant [22 x i8] c"[EHCI] Reset timeout!\00", align 1
@str.const.2 = private unnamed_addr constant [22 x i8] c"[EHCI] Reset complete\00", align 1
@str.const.3 = private unnamed_addr constant [22 x i8] c"[EHCI] Port reset HC#\00", align 1
@str.const.4 = private unnamed_addr constant [7 x i8] c" port \00", align 1
@str.const.5 = private unnamed_addr constant [19 x i8] c"[EHCI] Port: conn=\00", align 1
@str.const.6 = private unnamed_addr constant [5 x i8] c" en=\00", align 1
@str.const.7 = private unnamed_addr constant [8 x i8] c" owner=\00", align 1
@str.const.8 = private unnamed_addr constant [5 x i8] c" LS=\00", align 1
@str.const.9 = private unnamed_addr constant [43 x i8] c"[EHCI] Non-HS device, routing to companion\00", align 1
@str.const.10 = private unnamed_addr constant [25 x i8] c"[EHCI] Transfer timeout!\00", align 1
@str.const.11 = private unnamed_addr constant [23 x i8] c"[EHCI] SETUP qTD error\00", align 1
@str.const.12 = private unnamed_addr constant [24 x i8] c"[EHCI] STATUS qTD error\00", align 1
@str.const.13 = private unnamed_addr constant [17 x i8] c"[EHCI] Scanning \00", align 1
@str.const.14 = private unnamed_addr constant [14 x i8] c" ports on HC#\00", align 1
@str.const.15 = private unnamed_addr constant [14 x i8] c"[EHCI] Found \00", align 1
@str.const.16 = private unnamed_addr constant [14 x i8] c" HS device(s)\00", align 1
@str.const.17 = private unnamed_addr constant [13 x i8] c"[EHCI] Port \00", align 1
@str.const.18 = private unnamed_addr constant [19 x i8] c": device connected\00", align 1
@str.const.20 = private unnamed_addr constant [12 x i8] c": no device\00", align 1
@str.const.21 = private unnamed_addr constant [38 x i8] c"\E2\95\90\E2\95\90\E2\95\90 [EHCI] \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.22 = private unnamed_addr constant [24 x i8] c"  \D9\85\D8\AA\D8\AD\D9\83\D9\85\D8\A7\D8\AA EHCI: \00", align 1
@str.const.23 = private unnamed_addr constant [28 x i8] c"  \D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D9\86\D9\82\D9\84: \00", align 1
@str.const.24 = private unnamed_addr constant [15 x i8] c"  \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1: \00", align 1
@str.const.25 = private unnamed_addr constant [37 x i8] c"  \D8\A5\D8\B9\D8\A7\D8\AF\D8\A9 \D8\AA\D8\B9\D9\8A\D9\8A\D9\86 \D9\85\D9\86\D8\A7\D9\81\D8\B0: \00", align 1
@str.const.26 = private unnamed_addr constant [29 x i8] c"  \D8\AA\D9\88\D8\AC\D9\8A\D9\87\D8\A7\D8\AA companion: \00", align 1
@str.const.27 = private unnamed_addr constant [106 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.28 = private unnamed_addr constant [27 x i8] c"[EHCI] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 EHCI HC#\00", align 1
@str.const.29 = private unnamed_addr constant [9 x i8] c" MMIO=0x\00", align 1
@str.const.30 = private unnamed_addr constant [32 x i8] c"[EHCI] Max controllers reached!\00", align 1
@str.const.31 = private unnamed_addr constant [18 x i8] c"[EHCI] CAPLENGTH=\00", align 1
@str.const.32 = private unnamed_addr constant [15 x i8] c"[EHCI] Ports: \00", align 1
@str.const.33 = private unnamed_addr constant [15 x i8] c"[EHCI] EECP=0x\00", align 1
@str.const.34 = private unnamed_addr constant [26 x i8] c"[EHCI] Controller running\00", align 1
@str.const.35 = private unnamed_addr constant [32 x i8] c"[EHCI] Controller still halted!\00", align 1
@str.const.36 = private unnamed_addr constant [31 x i8] c"[EHCI] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9\00", align 1
@str.const.37 = private unnamed_addr constant [23 x i8] c"[EHCI] Port change HC#\00", align 1
@str.const.38 = private unnamed_addr constant [29 x i8] c"[EHCI] Host System Error HC#\00", align 1

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

define i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
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

define i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
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

define i64 @ehci_serial_hex8(i64 %val) {
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

define i64 @ehci_serial_hex16(i64 %val) {
entry:
  %val1 = alloca i64, align 8
  store i64 %val, ptr %val1, align 8
  %"%val.load" = load i64, ptr %val1, align 8
  %shrtmp = lshr i64 %"%val.load", 8
  %calltmp = call i64 @ehci_serial_hex8(i64 %shrtmp)
  %"%val.load2" = load i64, ptr %val1, align 8
  %andtmp = and i64 %"%val.load2", 255
  %calltmp3 = call i64 @ehci_serial_hex8(i64 %andtmp)
  ret i64 0
}

define i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"() {
entry:
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 13)
  %"%COM1.load1" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load1", i64 10)
  ret i64 0
}

define i64 @ehci_get_mmio(i64 %idx) {
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
  %loadtmp = load i64, ptr @ehci_mmio_0, align 8
  ret i64 %loadtmp

then_32:                                          ; preds = %merge_31
  %loadtmp4 = load i64, ptr @ehci_mmio_1, align 8
  ret i64 %loadtmp4

then_34:                                          ; preds = %merge_33
  %loadtmp7 = load i64, ptr @ehci_mmio_2, align 8
  ret i64 %loadtmp7

then_36:                                          ; preds = %merge_35
  %loadtmp10 = load i64, ptr @ehci_mmio_3, align 8
  ret i64 %loadtmp10
}

define i64 @ehci_get_opbase(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_38, label %merge_39

merge_39:                                         ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_40, label %merge_41

merge_41:                                         ; preds = %merge_39
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_42, label %merge_43

merge_43:                                         ; preds = %merge_41
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_44, label %merge_45

merge_45:                                         ; preds = %merge_43
  ret i64 0

then_38:                                          ; preds = %entry
  %loadtmp = load i64, ptr @ehci_opbase_0, align 8
  ret i64 %loadtmp

then_40:                                          ; preds = %merge_39
  %loadtmp4 = load i64, ptr @ehci_opbase_1, align 8
  ret i64 %loadtmp4

then_42:                                          ; preds = %merge_41
  %loadtmp7 = load i64, ptr @ehci_opbase_2, align 8
  ret i64 %loadtmp7

then_44:                                          ; preds = %merge_43
  %loadtmp10 = load i64, ptr @ehci_opbase_3, align 8
  ret i64 %loadtmp10
}

define i64 @ehci_get_num_ports(i64 %idx) {
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
  %loadtmp = load i64, ptr @ehci_num_ports_0, align 8
  ret i64 %loadtmp

then_48:                                          ; preds = %merge_47
  %loadtmp4 = load i64, ptr @ehci_num_ports_1, align 8
  ret i64 %loadtmp4

then_50:                                          ; preds = %merge_49
  %loadtmp7 = load i64, ptr @ehci_num_ports_2, align 8
  ret i64 %loadtmp7

then_52:                                          ; preds = %merge_51
  %loadtmp10 = load i64, ptr @ehci_num_ports_3, align 8
  ret i64 %loadtmp10
}

define i64 @ehci_get_data_buf(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_54, label %merge_55

merge_55:                                         ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_56, label %merge_57

merge_57:                                         ; preds = %merge_55
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_58, label %merge_59

merge_59:                                         ; preds = %merge_57
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_60, label %merge_61

merge_61:                                         ; preds = %merge_59
  ret i64 0

then_54:                                          ; preds = %entry
  %loadtmp = load i64, ptr @ehci_data_buf_0, align 8
  ret i64 %loadtmp

then_56:                                          ; preds = %merge_55
  %loadtmp4 = load i64, ptr @ehci_data_buf_1, align 8
  ret i64 %loadtmp4

then_58:                                          ; preds = %merge_57
  %loadtmp7 = load i64, ptr @ehci_data_buf_2, align 8
  ret i64 %loadtmp7

then_60:                                          ; preds = %merge_59
  %loadtmp10 = load i64, ptr @ehci_data_buf_3, align 8
  ret i64 %loadtmp10
}

define i64 @ehci_get_setup_buf(i64 %idx) {
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
  %loadtmp = load i64, ptr @ehci_setup_buf_0, align 8
  ret i64 %loadtmp

then_64:                                          ; preds = %merge_63
  %loadtmp4 = load i64, ptr @ehci_setup_buf_1, align 8
  ret i64 %loadtmp4

then_66:                                          ; preds = %merge_65
  %loadtmp7 = load i64, ptr @ehci_setup_buf_2, align 8
  ret i64 %loadtmp7

then_68:                                          ; preds = %merge_67
  %loadtmp10 = load i64, ptr @ehci_setup_buf_3, align 8
  ret i64 %loadtmp10
}

define i64 @ehci_get_qh_pool(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_70, label %merge_71

merge_71:                                         ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_72, label %merge_73

merge_73:                                         ; preds = %merge_71
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_74, label %merge_75

merge_75:                                         ; preds = %merge_73
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_76, label %merge_77

merge_77:                                         ; preds = %merge_75
  ret i64 0

then_70:                                          ; preds = %entry
  %loadtmp = load i64, ptr @ehci_qh_pool_0, align 8
  ret i64 %loadtmp

then_72:                                          ; preds = %merge_71
  %loadtmp4 = load i64, ptr @ehci_qh_pool_1, align 8
  ret i64 %loadtmp4

then_74:                                          ; preds = %merge_73
  %loadtmp7 = load i64, ptr @ehci_qh_pool_2, align 8
  ret i64 %loadtmp7

then_76:                                          ; preds = %merge_75
  %loadtmp10 = load i64, ptr @ehci_qh_pool_3, align 8
  ret i64 %loadtmp10
}

define i64 @ehci_get_qtd_pool(i64 %idx) {
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
  %loadtmp = load i64, ptr @ehci_qtd_pool_0, align 8
  ret i64 %loadtmp

then_80:                                          ; preds = %merge_79
  %loadtmp4 = load i64, ptr @ehci_qtd_pool_1, align 8
  ret i64 %loadtmp4

then_82:                                          ; preds = %merge_81
  %loadtmp7 = load i64, ptr @ehci_qtd_pool_2, align 8
  ret i64 %loadtmp7

then_84:                                          ; preds = %merge_83
  %loadtmp10 = load i64, ptr @ehci_qtd_pool_3, align 8
  ret i64 %loadtmp10
}

define i64 @ehci_get_async_qh(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_86, label %merge_87

merge_87:                                         ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_88, label %merge_89

merge_89:                                         ; preds = %merge_87
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_90, label %merge_91

merge_91:                                         ; preds = %merge_89
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_92, label %merge_93

merge_93:                                         ; preds = %merge_91
  ret i64 0

then_86:                                          ; preds = %entry
  %loadtmp = load i64, ptr @ehci_async_qh_0, align 8
  ret i64 %loadtmp

then_88:                                          ; preds = %merge_87
  %loadtmp4 = load i64, ptr @ehci_async_qh_1, align 8
  ret i64 %loadtmp4

then_90:                                          ; preds = %merge_89
  %loadtmp7 = load i64, ptr @ehci_async_qh_2, align 8
  ret i64 %loadtmp7

then_92:                                          ; preds = %merge_91
  %loadtmp10 = load i64, ptr @ehci_async_qh_3, align 8
  ret i64 %loadtmp10
}

define i64 @ehci_read_cap(i64 %hc_idx, i64 %reg) {
entry:
  %"%mmio" = alloca i64, align 8
  %reg2 = alloca i64, align 8
  store i64 %reg, ptr %reg2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @ehci_get_mmio(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%mmio", align 8
  %"%mmio.load" = load i64, ptr %"%mmio", align 8
  %"%reg.load" = load i64, ptr %reg2, align 8
  %addtmp = add i64 %"%mmio.load", %"%reg.load"
  call void @sad_ll_mem_read32(i64 %addtmp)
  ret i64 0
}

define i64 @ehci_read_op(i64 %hc_idx, i64 %reg) {
entry:
  %"%opbase" = alloca i64, align 8
  %reg2 = alloca i64, align 8
  store i64 %reg, ptr %reg2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @ehci_get_opbase(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%opbase", align 8
  %"%opbase.load" = load i64, ptr %"%opbase", align 8
  %"%reg.load" = load i64, ptr %reg2, align 8
  %addtmp = add i64 %"%opbase.load", %"%reg.load"
  call void @sad_ll_mem_read32(i64 %addtmp)
  ret i64 0
}

define i64 @ehci_write_op(i64 %hc_idx, i64 %reg, i64 %val) {
entry:
  %"%opbase" = alloca i64, align 8
  %val3 = alloca i64, align 8
  store i64 %val, ptr %val3, align 8
  %reg2 = alloca i64, align 8
  store i64 %reg, ptr %reg2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @ehci_get_opbase(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%opbase", align 8
  %"%opbase.load" = load i64, ptr %"%opbase", align 8
  %"%reg.load" = load i64, ptr %reg2, align 8
  %addtmp = add i64 %"%opbase.load", %"%reg.load"
  %"%304.load" = load i64, ptr %val3, align 8
  call void @sad_ll_mem_write32(i64 %addtmp, i64 %"%304.load")
  ret i64 0
}

define i64 @ehci_read_port(i64 %hc_idx, i64 %port_num) {
entry:
  %"%opbase" = alloca i64, align 8
  %port_num2 = alloca i64, align 8
  store i64 %port_num, ptr %port_num2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @ehci_get_opbase(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%opbase", align 8
  %"%opbase.load" = load i64, ptr %"%opbase", align 8
  %"%EHCI_OP_PORTSC_BASE.load" = load i64, ptr @EHCI_OP_PORTSC_BASE, align 8
  %addtmp = add i64 %"%opbase.load", %"%EHCI_OP_PORTSC_BASE.load"
  %"%port_num.load" = load i64, ptr %port_num2, align 8
  %shltmp = shl i64 %"%port_num.load", 2
  %addtmp3 = add i64 %addtmp, %shltmp
  call void @sad_ll_mem_read32(i64 %addtmp3)
  ret i64 0
}

define i64 @ehci_write_port(i64 %hc_idx, i64 %port_num, i64 %val) {
entry:
  %"%opbase" = alloca i64, align 8
  %val3 = alloca i64, align 8
  store i64 %val, ptr %val3, align 8
  %port_num2 = alloca i64, align 8
  store i64 %port_num, ptr %port_num2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @ehci_get_opbase(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%opbase", align 8
  %"%opbase.load" = load i64, ptr %"%opbase", align 8
  %"%EHCI_OP_PORTSC_BASE.load" = load i64, ptr @EHCI_OP_PORTSC_BASE, align 8
  %addtmp = add i64 %"%opbase.load", %"%EHCI_OP_PORTSC_BASE.load"
  %"%port_num.load" = load i64, ptr %port_num2, align 8
  %shltmp = shl i64 %"%port_num.load", 2
  %addtmp4 = add i64 %addtmp, %shltmp
  %"%326.load" = load i64, ptr %val3, align 8
  call void @sad_ll_mem_write32(i64 %addtmp4, i64 %"%326.load")
  ret i64 0
}

define i64 @ehci_reset(i64 %hc_idx) {
entry:
  %"%d2" = alloca i64, align 8
  %"%w2" = alloca i64, align 8
  %"%d" = alloca i64, align 8
  %"%w" = alloca i64, align 8
  %"%halted" = alloca i64, align 8
  %"%sts" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  %"%new_cmd" = alloca i64, align 8
  %"%cmd" = alloca i64, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %calltmp = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %"%332.load" = load i64, ptr %hc_idx1, align 8
  %calltmp2 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%332.load")
  %calltmp3 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_USBCMD.load" = load i64, ptr @EHCI_OP_USBCMD, align 8
  %calltmp4 = call i64 @ehci_read_op(i64 %"%hc_idx.load", i64 %"%EHCI_OP_USBCMD.load")
  store i64 %calltmp4, ptr %"%cmd", align 8
  %"%cmd.load" = load i64, ptr %"%cmd", align 8
  %andtmp = and i64 %"%cmd.load", 1
  %"%cmd.load5" = load i64, ptr %"%cmd", align 8
  %subtmp = sub i64 %"%cmd.load5", %andtmp
  store i64 %subtmp, ptr %"%new_cmd", align 8
  %"%hc_idx.load6" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_USBCMD.load7" = load i64, ptr @EHCI_OP_USBCMD, align 8
  %"%new_cmd.load" = load i64, ptr %"%new_cmd", align 8
  %calltmp8 = call i64 @ehci_write_op(i64 %"%hc_idx.load6", i64 %"%EHCI_OP_USBCMD.load7", i64 %"%new_cmd.load")
  store i64 200, ptr %"%timeout", align 8
  br label %while_cond_94

else_99:                                          ; preds = %while_body_95
  %"%timeout.load15" = load i64, ptr %"%timeout", align 8
  %subtmp16 = sub i64 %"%timeout.load15", 1
  store i64 %subtmp16, ptr %"%timeout", align 8
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%w", align 8
  br label %while_cond_100

merge_107:                                        ; preds = %while_body_104
  %"%timeout.load30" = load i64, ptr %"%timeout", align 8
  %subtmp31 = sub i64 %"%timeout.load30", 1
  store i64 %subtmp31, ptr %"%timeout", align 8
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%w2", align 8
  br label %while_cond_108

merge_98:                                         ; preds = %while_exit_102, %then_97
  br label %while_cond_94

then_106:                                         ; preds = %while_body_104
  %calltmp28 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %calltmp29 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1

then_97:                                          ; preds = %while_body_95
  store i64 0, ptr %"%timeout", align 8
  br label %merge_98

while_body_101:                                   ; preds = %while_cond_100
  store i64 0, ptr %"%d", align 8
  br label %while_cond_100

while_body_104:                                   ; preds = %while_cond_103
  %"%hc_idx.load20" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_USBCMD.load21" = load i64, ptr @EHCI_OP_USBCMD, align 8
  %calltmp22 = call i64 @ehci_read_op(i64 %"%hc_idx.load20", i64 %"%EHCI_OP_USBCMD.load21")
  store i64 %calltmp22, ptr %"%cmd", align 8
  %"%cmd.load23" = load i64, ptr %"%cmd", align 8
  %shrtmp = lshr i64 %"%cmd.load23", 1
  %andtmp24 = and i64 %shrtmp, 1
  %cmpeqtmp25 = icmp eq i64 %andtmp24, 0
  br i1 %cmpeqtmp25, label %then_106, label %merge_107

while_body_109:                                   ; preds = %while_cond_108
  store i64 0, ptr %"%d2", align 8
  br label %while_cond_108

while_body_95:                                    ; preds = %while_cond_94
  %"%hc_idx.load9" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_USBSTS.load" = load i64, ptr @EHCI_OP_USBSTS, align 8
  %calltmp10 = call i64 @ehci_read_op(i64 %"%hc_idx.load9", i64 %"%EHCI_OP_USBSTS.load")
  store i64 %calltmp10, ptr %"%sts", align 8
  %"%sts.load" = load i64, ptr %"%sts", align 8
  %"%EHCI_STS_HCH.load" = load i64, ptr @EHCI_STS_HCH, align 8
  %divtmp = sdiv i64 %"%sts.load", %"%EHCI_STS_HCH.load"
  %andtmp11 = and i64 %divtmp, 1
  store i64 %andtmp11, ptr %"%halted", align 8
  %"%halted.load" = load i64, ptr %"%halted", align 8
  %cmpeqtmp = icmp eq i64 %"%halted.load", 1
  br i1 %cmpeqtmp, label %then_97, label %else_99

while_cond_100:                                   ; preds = %while_body_101, %else_99
  call void @sad_ll_timer_get_ticks()
  %"%w.load" = load i64, ptr %"%w", align 8
  %subtmp17 = sub i64 0, %"%w.load"
  %cmplttmp = icmp slt i64 %subtmp17, 100
  br i1 %cmplttmp, label %while_body_101, label %while_exit_102

while_cond_103:                                   ; preds = %while_exit_110, %while_exit_96
  %"%timeout.load18" = load i64, ptr %"%timeout", align 8
  %cmpgttmp19 = icmp sgt i64 %"%timeout.load18", 0
  br i1 %cmpgttmp19, label %while_body_104, label %while_exit_105

while_cond_108:                                   ; preds = %while_body_109, %merge_107
  call void @sad_ll_timer_get_ticks()
  %"%w2.load" = load i64, ptr %"%w2", align 8
  %subtmp32 = sub i64 0, %"%w2.load"
  %cmplttmp33 = icmp slt i64 %subtmp32, 100
  br i1 %cmplttmp33, label %while_body_109, label %while_exit_110

while_cond_94:                                    ; preds = %merge_98, %entry
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp, label %while_body_95, label %while_exit_96

while_exit_102:                                   ; preds = %while_cond_100
  br label %merge_98

while_exit_105:                                   ; preds = %while_cond_103
  %calltmp26 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %calltmp27 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

while_exit_110:                                   ; preds = %while_cond_108
  br label %while_cond_103

while_exit_96:                                    ; preds = %while_cond_94
  %"%hc_idx.load12" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_USBCMD.load13" = load i64, ptr @EHCI_OP_USBCMD, align 8
  %"%EHCI_CMD_HCRESET.load" = load i64, ptr @EHCI_CMD_HCRESET, align 8
  %calltmp14 = call i64 @ehci_write_op(i64 %"%hc_idx.load12", i64 %"%EHCI_OP_USBCMD.load13", i64 %"%EHCI_CMD_HCRESET.load")
  store i64 500, ptr %"%timeout", align 8
  br label %while_cond_103
}

define i64 @ehci_alloc_qtd(i64 %hc_idx) {
entry:
  %"%ehci_qtd_used_3" = alloca i64, align 8
  %"%ehci_qtd_used_2" = alloca i64, align 8
  %"%ehci_qtd_used_1" = alloca i64, align 8
  %"%ehci_qtd_used_0" = alloca i64, align 8
  %"%qtd" = alloca i64, align 8
  %"%pool" = alloca i64, align 8
  %"%used" = alloca i64, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  store i64 0, ptr %"%used", align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%hc_idx.load", 0
  br i1 %cmpeqtmp, label %then_111, label %merge_112

merge_112:                                        ; preds = %then_111, %entry
  %"%hc_idx.load2" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%hc_idx.load2", 1
  br i1 %cmpeqtmp3, label %then_113, label %merge_114

merge_114:                                        ; preds = %then_113, %merge_112
  %"%hc_idx.load4" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp5 = icmp eq i64 %"%hc_idx.load4", 2
  br i1 %cmpeqtmp5, label %then_115, label %merge_116

merge_116:                                        ; preds = %then_115, %merge_114
  %"%hc_idx.load6" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp7 = icmp eq i64 %"%hc_idx.load6", 3
  br i1 %cmpeqtmp7, label %then_117, label %merge_118

merge_118:                                        ; preds = %then_117, %merge_116
  %"%used.load" = load i64, ptr %"%used", align 8
  %cmpgetmp = icmp sge i64 %"%used.load", 32
  br i1 %cmpgetmp, label %then_119, label %merge_120

merge_120:                                        ; preds = %merge_118
  %"%hc_idx.load8" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @ehci_get_qtd_pool(i64 %"%hc_idx.load8")
  store i64 %calltmp, ptr %"%pool", align 8
  %"%used.load9" = load i64, ptr %"%used", align 8
  %shltmp = shl i64 %"%used.load9", 5
  %"%pool.load" = load i64, ptr %"%pool", align 8
  %addtmp = add i64 %"%pool.load", %shltmp
  store i64 %addtmp, ptr %"%qtd", align 8
  %"%qtd.load" = load i64, ptr %"%qtd", align 8
  call void @sad_ll_mem_write32(i64 %"%qtd.load", i64 1)
  %"%qtd.load10" = load i64, ptr %"%qtd", align 8
  %addtmp11 = add i64 %"%qtd.load10", 4
  call void @sad_ll_mem_write32(i64 %addtmp11, i64 1)
  %"%qtd.load12" = load i64, ptr %"%qtd", align 8
  %addtmp13 = add i64 %"%qtd.load12", 8
  call void @sad_ll_mem_write32(i64 %addtmp13, i64 0)
  %"%qtd.load14" = load i64, ptr %"%qtd", align 8
  %addtmp15 = add i64 %"%qtd.load14", 12
  call void @sad_ll_mem_write32(i64 %addtmp15, i64 0)
  %"%qtd.load16" = load i64, ptr %"%qtd", align 8
  %addtmp17 = add i64 %"%qtd.load16", 16
  call void @sad_ll_mem_write32(i64 %addtmp17, i64 0)
  %"%qtd.load18" = load i64, ptr %"%qtd", align 8
  %addtmp19 = add i64 %"%qtd.load18", 20
  call void @sad_ll_mem_write32(i64 %addtmp19, i64 0)
  %"%qtd.load20" = load i64, ptr %"%qtd", align 8
  %addtmp21 = add i64 %"%qtd.load20", 24
  call void @sad_ll_mem_write32(i64 %addtmp21, i64 0)
  %"%qtd.load22" = load i64, ptr %"%qtd", align 8
  %addtmp23 = add i64 %"%qtd.load22", 28
  call void @sad_ll_mem_write32(i64 %addtmp23, i64 0)
  %"%hc_idx.load24" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp25 = icmp eq i64 %"%hc_idx.load24", 0
  br i1 %cmpeqtmp25, label %then_121, label %merge_122

merge_122:                                        ; preds = %then_121, %merge_120
  %"%hc_idx.load28" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp29 = icmp eq i64 %"%hc_idx.load28", 1
  br i1 %cmpeqtmp29, label %then_123, label %merge_124

merge_124:                                        ; preds = %then_123, %merge_122
  %"%hc_idx.load32" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%hc_idx.load32", 2
  br i1 %cmpeqtmp33, label %then_125, label %merge_126

merge_126:                                        ; preds = %then_125, %merge_124
  %"%hc_idx.load36" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp37 = icmp eq i64 %"%hc_idx.load36", 3
  br i1 %cmpeqtmp37, label %then_127, label %merge_128

merge_128:                                        ; preds = %then_127, %merge_126
  %loadtmp = load i64, ptr %"%qtd", align 8
  ret i64 %loadtmp

then_111:                                         ; preds = %entry
  %"%405.load" = load i64, ptr @ehci_qtd_used_0, align 8
  store i64 %"%405.load", ptr %"%used", align 8
  br label %merge_112

then_113:                                         ; preds = %merge_112
  %"%409.load" = load i64, ptr @ehci_qtd_used_1, align 8
  store i64 %"%409.load", ptr %"%used", align 8
  br label %merge_114

then_115:                                         ; preds = %merge_114
  %"%413.load" = load i64, ptr @ehci_qtd_used_2, align 8
  store i64 %"%413.load", ptr %"%used", align 8
  br label %merge_116

then_117:                                         ; preds = %merge_116
  %"%417.load" = load i64, ptr @ehci_qtd_used_3, align 8
  store i64 %"%417.load", ptr %"%used", align 8
  br label %merge_118

then_119:                                         ; preds = %merge_118
  ret i64 0

then_121:                                         ; preds = %merge_120
  %"%used.load26" = load i64, ptr %"%used", align 8
  %addtmp27 = add i64 %"%used.load26", 1
  store i64 %addtmp27, ptr %"%ehci_qtd_used_0", align 8
  br label %merge_122

then_123:                                         ; preds = %merge_122
  %"%used.load30" = load i64, ptr %"%used", align 8
  %addtmp31 = add i64 %"%used.load30", 1
  store i64 %addtmp31, ptr %"%ehci_qtd_used_1", align 8
  br label %merge_124

then_125:                                         ; preds = %merge_124
  %"%used.load34" = load i64, ptr %"%used", align 8
  %addtmp35 = add i64 %"%used.load34", 1
  store i64 %addtmp35, ptr %"%ehci_qtd_used_2", align 8
  br label %merge_126

then_127:                                         ; preds = %merge_126
  %"%used.load38" = load i64, ptr %"%used", align 8
  %addtmp39 = add i64 %"%used.load38", 1
  store i64 %addtmp39, ptr %"%ehci_qtd_used_3", align 8
  br label %merge_128
}

define i64 @ehci_reset_qtd_pool(i64 %hc_idx) {
entry:
  %"%ehci_qtd_used_3" = alloca i64, align 8
  %"%ehci_qtd_used_2" = alloca i64, align 8
  %"%ehci_qtd_used_1" = alloca i64, align 8
  %"%ehci_qtd_used_0" = alloca i64, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%hc_idx.load", 0
  br i1 %cmpeqtmp, label %then_129, label %merge_130

merge_130:                                        ; preds = %then_129, %entry
  %"%hc_idx.load2" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%hc_idx.load2", 1
  br i1 %cmpeqtmp3, label %then_131, label %merge_132

merge_132:                                        ; preds = %then_131, %merge_130
  %"%hc_idx.load4" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp5 = icmp eq i64 %"%hc_idx.load4", 2
  br i1 %cmpeqtmp5, label %then_133, label %merge_134

merge_134:                                        ; preds = %then_133, %merge_132
  %"%hc_idx.load6" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp7 = icmp eq i64 %"%hc_idx.load6", 3
  br i1 %cmpeqtmp7, label %then_135, label %merge_136

merge_136:                                        ; preds = %then_135, %merge_134
  ret i64 0

then_129:                                         ; preds = %entry
  store i64 0, ptr %"%ehci_qtd_used_0", align 8
  br label %merge_130

then_131:                                         ; preds = %merge_130
  store i64 0, ptr %"%ehci_qtd_used_1", align 8
  br label %merge_132

then_133:                                         ; preds = %merge_132
  store i64 0, ptr %"%ehci_qtd_used_2", align 8
  br label %merge_134

then_135:                                         ; preds = %merge_134
  store i64 0, ptr %"%ehci_qtd_used_3", align 8
  br label %merge_136
}

define i64 @ehci_qtd_set_next(i64 %qtd, i64 %next_qtd, i64 %terminate) {
entry:
  %terminate3 = alloca i64, align 8
  store i64 %terminate, ptr %terminate3, align 8
  %next_qtd2 = alloca i64, align 8
  store i64 %next_qtd, ptr %next_qtd2, align 8
  %qtd1 = alloca i64, align 8
  store i64 %qtd, ptr %qtd1, align 8
  %"%terminate.load" = load i64, ptr %terminate3, align 8
  %cmpeqtmp = icmp eq i64 %"%terminate.load", 1
  br i1 %cmpeqtmp, label %then_137, label %else_139

else_139:                                         ; preds = %entry
  %"%qtd.load4" = load i64, ptr %qtd1, align 8
  %"%494.load" = load i64, ptr %next_qtd2, align 8
  call void @sad_ll_mem_write32(i64 %"%qtd.load4", i64 %"%494.load")
  br label %merge_138

merge_138:                                        ; preds = %else_139, %then_137
  ret i64 0

then_137:                                         ; preds = %entry
  %"%qtd.load" = load i64, ptr %qtd1, align 8
  call void @sad_ll_mem_write32(i64 %"%qtd.load", i64 1)
  br label %merge_138
}

define i64 @ehci_qtd_set_token(i64 %qtd, i64 %pid_code, i64 %total_bytes, i64 %toggle, i64 %cerr, i64 %ioc) {
entry:
  %"%token" = alloca i64, align 8
  %ioc6 = alloca i64, align 8
  store i64 %ioc, ptr %ioc6, align 8
  %cerr5 = alloca i64, align 8
  store i64 %cerr, ptr %cerr5, align 8
  %toggle4 = alloca i64, align 8
  store i64 %toggle, ptr %toggle4, align 8
  %total_bytes3 = alloca i64, align 8
  store i64 %total_bytes, ptr %total_bytes3, align 8
  %pid_code2 = alloca i64, align 8
  store i64 %pid_code, ptr %pid_code2, align 8
  %qtd1 = alloca i64, align 8
  store i64 %qtd, ptr %qtd1, align 8
  %"%EHCI_QTD_ACTIVE.load" = load i64, ptr @EHCI_QTD_ACTIVE, align 8
  store i64 %"%EHCI_QTD_ACTIVE.load", ptr %"%token", align 8
  %"%token.load" = load i64, ptr %"%token", align 8
  %"%pid_code.load" = load i64, ptr %pid_code2, align 8
  %addtmp = add i64 %"%token.load", %"%pid_code.load"
  store i64 %addtmp, ptr %"%token", align 8
  %"%cerr.load" = load i64, ptr %cerr5, align 8
  %shltmp = shl i64 %"%cerr.load", 10
  %"%token.load7" = load i64, ptr %"%token", align 8
  %addtmp8 = add i64 %"%token.load7", %shltmp
  store i64 %addtmp8, ptr %"%token", align 8
  %"%ioc.load" = load i64, ptr %ioc6, align 8
  %cmpeqtmp = icmp eq i64 %"%ioc.load", 1
  br i1 %cmpeqtmp, label %then_140, label %merge_141

merge_141:                                        ; preds = %then_140, %entry
  %"%total_bytes.load" = load i64, ptr %total_bytes3, align 8
  %shltmp11 = shl i64 %"%total_bytes.load", 16
  %"%token.load12" = load i64, ptr %"%token", align 8
  %addtmp13 = add i64 %"%token.load12", %shltmp11
  store i64 %addtmp13, ptr %"%token", align 8
  %"%toggle.load" = load i64, ptr %toggle4, align 8
  %cmpeqtmp14 = icmp eq i64 %"%toggle.load", 1
  br i1 %cmpeqtmp14, label %then_142, label %merge_143

merge_143:                                        ; preds = %then_142, %merge_141
  %"%qtd.load" = load i64, ptr %qtd1, align 8
  %addtmp17 = add i64 %"%qtd.load", 8
  %"%token.load18" = load i64, ptr %"%token", align 8
  call void @sad_ll_mem_write32(i64 %addtmp17, i64 %"%token.load18")
  ret i64 0

then_140:                                         ; preds = %entry
  %"%token.load9" = load i64, ptr %"%token", align 8
  %addtmp10 = add i64 %"%token.load9", 32768
  store i64 %addtmp10, ptr %"%token", align 8
  br label %merge_141

then_142:                                         ; preds = %merge_141
  %"%token.load15" = load i64, ptr %"%token", align 8
  %"%EHCI_QTD_TOGGLE.load" = load i64, ptr @EHCI_QTD_TOGGLE, align 8
  %addtmp16 = add i64 %"%token.load15", %"%EHCI_QTD_TOGGLE.load"
  store i64 %addtmp16, ptr %"%token", align 8
  br label %merge_143
}

define i64 @ehci_qtd_set_buffer(i64 %qtd, i64 %buf_phys) {
entry:
  %buf_phys2 = alloca i64, align 8
  store i64 %buf_phys, ptr %buf_phys2, align 8
  %qtd1 = alloca i64, align 8
  store i64 %qtd, ptr %qtd1, align 8
  %"%qtd.load" = load i64, ptr %qtd1, align 8
  %addtmp = add i64 %"%qtd.load", 12
  %"%518.load" = load i64, ptr %buf_phys2, align 8
  call void @sad_ll_mem_write32(i64 %addtmp, i64 %"%518.load")
  ret i64 0
}

define i64 @ehci_qtd_get_token(i64 %qtd) {
entry:
  %qtd1 = alloca i64, align 8
  store i64 %qtd, ptr %qtd1, align 8
  %"%qtd.load" = load i64, ptr %qtd1, align 8
  %addtmp = add i64 %"%qtd.load", 8
  call void @sad_ll_mem_read32(i64 %addtmp)
  ret i64 0
}

define i64 @ehci_qtd_is_active(i64 %qtd) {
entry:
  %"%active" = alloca i64, align 8
  %"%token" = alloca i64, align 8
  %qtd1 = alloca i64, align 8
  store i64 %qtd, ptr %qtd1, align 8
  %"%qtd.load" = load i64, ptr %qtd1, align 8
  %calltmp = call i64 @ehci_qtd_get_token(i64 %"%qtd.load")
  store i64 %calltmp, ptr %"%token", align 8
  %"%token.load" = load i64, ptr %"%token", align 8
  %"%EHCI_QTD_ACTIVE.load" = load i64, ptr @EHCI_QTD_ACTIVE, align 8
  %divtmp = sdiv i64 %"%token.load", %"%EHCI_QTD_ACTIVE.load"
  %andtmp = and i64 %divtmp, 1
  store i64 %andtmp, ptr %"%active", align 8
  %loadtmp = load i64, ptr %"%active", align 8
  ret i64 %loadtmp
}

define i64 @ehci_qtd_has_error(i64 %qtd) {
entry:
  %"%xact" = alloca i64, align 8
  %"%babble" = alloca i64, align 8
  %"%dbuf" = alloca i64, align 8
  %"%halted" = alloca i64, align 8
  %"%token" = alloca i64, align 8
  %qtd1 = alloca i64, align 8
  store i64 %qtd, ptr %qtd1, align 8
  %"%qtd.load" = load i64, ptr %qtd1, align 8
  %calltmp = call i64 @ehci_qtd_get_token(i64 %"%qtd.load")
  store i64 %calltmp, ptr %"%token", align 8
  %"%token.load" = load i64, ptr %"%token", align 8
  %"%EHCI_QTD_HALTED.load" = load i64, ptr @EHCI_QTD_HALTED, align 8
  %divtmp = sdiv i64 %"%token.load", %"%EHCI_QTD_HALTED.load"
  %andtmp = and i64 %divtmp, 1
  store i64 %andtmp, ptr %"%halted", align 8
  %"%token.load2" = load i64, ptr %"%token", align 8
  %"%EHCI_QTD_DBUF_ERR.load" = load i64, ptr @EHCI_QTD_DBUF_ERR, align 8
  %divtmp3 = sdiv i64 %"%token.load2", %"%EHCI_QTD_DBUF_ERR.load"
  %andtmp4 = and i64 %divtmp3, 1
  store i64 %andtmp4, ptr %"%dbuf", align 8
  %"%token.load5" = load i64, ptr %"%token", align 8
  %"%EHCI_QTD_BABBLE.load" = load i64, ptr @EHCI_QTD_BABBLE, align 8
  %divtmp6 = sdiv i64 %"%token.load5", %"%EHCI_QTD_BABBLE.load"
  %andtmp7 = and i64 %divtmp6, 1
  store i64 %andtmp7, ptr %"%babble", align 8
  %"%token.load8" = load i64, ptr %"%token", align 8
  %"%EHCI_QTD_XACT_ERR.load" = load i64, ptr @EHCI_QTD_XACT_ERR, align 8
  %divtmp9 = sdiv i64 %"%token.load8", %"%EHCI_QTD_XACT_ERR.load"
  %andtmp10 = and i64 %divtmp9, 1
  store i64 %andtmp10, ptr %"%xact", align 8
  %"%halted.load" = load i64, ptr %"%halted", align 8
  %"%dbuf.load" = load i64, ptr %"%dbuf", align 8
  %addtmp = add i64 %"%halted.load", %"%dbuf.load"
  %"%babble.load" = load i64, ptr %"%babble", align 8
  %addtmp11 = add i64 %addtmp, %"%babble.load"
  %"%xact.load" = load i64, ptr %"%xact", align 8
  %addtmp12 = add i64 %addtmp11, %"%xact.load"
  %cmpgttmp = icmp sgt i64 %addtmp12, 0
  br i1 %cmpgttmp, label %then_144, label %merge_145

merge_145:                                        ; preds = %entry
  ret i64 0

then_144:                                         ; preds = %entry
  ret i64 1
}

define i64 @ehci_qtd_actual_len(i64 %qtd) {
entry:
  %"%remaining" = alloca i64, align 8
  %"%token" = alloca i64, align 8
  %qtd1 = alloca i64, align 8
  store i64 %qtd, ptr %qtd1, align 8
  %"%qtd.load" = load i64, ptr %qtd1, align 8
  %calltmp = call i64 @ehci_qtd_get_token(i64 %"%qtd.load")
  store i64 %calltmp, ptr %"%token", align 8
  %"%token.load" = load i64, ptr %"%token", align 8
  %shrtmp = lshr i64 %"%token.load", 16
  %andtmp = and i64 %shrtmp, 32767
  store i64 %andtmp, ptr %"%remaining", align 8
  %loadtmp = load i64, ptr %"%remaining", align 8
  ret i64 %loadtmp
}

define i64 @ehci_alloc_qh(i64 %hc_idx) {
entry:
  %"%ehci_qh_used_3" = alloca i64, align 8
  %"%ehci_qh_used_2" = alloca i64, align 8
  %"%ehci_qh_used_1" = alloca i64, align 8
  %"%ehci_qh_used_0" = alloca i64, align 8
  %"%\D9\82\D9\87" = alloca i64, align 8
  %"%pool" = alloca i64, align 8
  %"%used" = alloca i64, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  store i64 0, ptr %"%used", align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%hc_idx.load", 0
  br i1 %cmpeqtmp, label %then_146, label %merge_147

merge_147:                                        ; preds = %then_146, %entry
  %"%hc_idx.load2" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%hc_idx.load2", 1
  br i1 %cmpeqtmp3, label %then_148, label %merge_149

merge_149:                                        ; preds = %then_148, %merge_147
  %"%hc_idx.load4" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp5 = icmp eq i64 %"%hc_idx.load4", 2
  br i1 %cmpeqtmp5, label %then_150, label %merge_151

merge_151:                                        ; preds = %then_150, %merge_149
  %"%hc_idx.load6" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp7 = icmp eq i64 %"%hc_idx.load6", 3
  br i1 %cmpeqtmp7, label %then_152, label %merge_153

merge_153:                                        ; preds = %then_152, %merge_151
  %"%used.load" = load i64, ptr %"%used", align 8
  %cmpgetmp = icmp sge i64 %"%used.load", 16
  br i1 %cmpgetmp, label %then_154, label %merge_155

merge_155:                                        ; preds = %merge_153
  %"%hc_idx.load8" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @ehci_get_qh_pool(i64 %"%hc_idx.load8")
  store i64 %calltmp, ptr %"%pool", align 8
  %"%used.load9" = load i64, ptr %"%used", align 8
  %shltmp = shl i64 %"%used.load9", 6
  %"%pool.load" = load i64, ptr %"%pool", align 8
  %addtmp = add i64 %"%pool.load", %shltmp
  store i64 %addtmp, ptr %"%\D9\82\D9\87", align 8
  %"%\D9\82\D9\87.load" = load i64, ptr %"%\D9\82\D9\87", align 8
  call void @sad_ll_memset(i64 %"%\D9\82\D9\87.load", i64 0, i64 64)
  %"%\D9\82\D9\87.load10" = load i64, ptr %"%\D9\82\D9\87", align 8
  call void @sad_ll_mem_write32(i64 %"%\D9\82\D9\87.load10", i64 1)
  %"%\D9\82\D9\87.load11" = load i64, ptr %"%\D9\82\D9\87", align 8
  %addtmp12 = add i64 %"%\D9\82\D9\87.load11", 16
  call void @sad_ll_mem_write32(i64 %addtmp12, i64 1)
  %"%hc_idx.load13" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%hc_idx.load13", 0
  br i1 %cmpeqtmp14, label %then_156, label %merge_157

merge_157:                                        ; preds = %then_156, %merge_155
  %"%hc_idx.load17" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%hc_idx.load17", 1
  br i1 %cmpeqtmp18, label %then_158, label %merge_159

merge_159:                                        ; preds = %then_158, %merge_157
  %"%hc_idx.load21" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%hc_idx.load21", 2
  br i1 %cmpeqtmp22, label %then_160, label %merge_161

merge_161:                                        ; preds = %then_160, %merge_159
  %"%hc_idx.load25" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%hc_idx.load25", 3
  br i1 %cmpeqtmp26, label %then_162, label %merge_163

merge_163:                                        ; preds = %then_162, %merge_161
  %loadtmp = load i64, ptr %"%\D9\82\D9\87", align 8
  ret i64 %loadtmp

then_146:                                         ; preds = %entry
  %"%574.load" = load i64, ptr @ehci_qh_used_0, align 8
  store i64 %"%574.load", ptr %"%used", align 8
  br label %merge_147

then_148:                                         ; preds = %merge_147
  %"%578.load" = load i64, ptr @ehci_qh_used_1, align 8
  store i64 %"%578.load", ptr %"%used", align 8
  br label %merge_149

then_150:                                         ; preds = %merge_149
  %"%582.load" = load i64, ptr @ehci_qh_used_2, align 8
  store i64 %"%582.load", ptr %"%used", align 8
  br label %merge_151

then_152:                                         ; preds = %merge_151
  %"%586.load" = load i64, ptr @ehci_qh_used_3, align 8
  store i64 %"%586.load", ptr %"%used", align 8
  br label %merge_153

then_154:                                         ; preds = %merge_153
  ret i64 0

then_156:                                         ; preds = %merge_155
  %"%used.load15" = load i64, ptr %"%used", align 8
  %addtmp16 = add i64 %"%used.load15", 1
  store i64 %addtmp16, ptr %"%ehci_qh_used_0", align 8
  br label %merge_157

then_158:                                         ; preds = %merge_157
  %"%used.load19" = load i64, ptr %"%used", align 8
  %addtmp20 = add i64 %"%used.load19", 1
  store i64 %addtmp20, ptr %"%ehci_qh_used_1", align 8
  br label %merge_159

then_160:                                         ; preds = %merge_159
  %"%used.load23" = load i64, ptr %"%used", align 8
  %addtmp24 = add i64 %"%used.load23", 1
  store i64 %addtmp24, ptr %"%ehci_qh_used_2", align 8
  br label %merge_161

then_162:                                         ; preds = %merge_161
  %"%used.load27" = load i64, ptr %"%used", align 8
  %addtmp28 = add i64 %"%used.load27", 1
  store i64 %addtmp28, ptr %"%ehci_qh_used_3", align 8
  br label %merge_163
}

define i64 @ehci_qh_setup(i64 %"\D9\82\D9\87", i64 %dev_addr, i64 %endpoint, i64 %max_pkt, i64 %speed, i64 %is_head) {
entry:
  %"%caps" = alloca i64, align 8
  %"%chars" = alloca i64, align 8
  %is_head6 = alloca i64, align 8
  store i64 %is_head, ptr %is_head6, align 8
  %speed5 = alloca i64, align 8
  store i64 %speed, ptr %speed5, align 8
  %max_pkt4 = alloca i64, align 8
  store i64 %max_pkt, ptr %max_pkt4, align 8
  %endpoint3 = alloca i64, align 8
  store i64 %endpoint, ptr %endpoint3, align 8
  %dev_addr2 = alloca i64, align 8
  store i64 %dev_addr, ptr %dev_addr2, align 8
  %"\D9\82\D9\871" = alloca i64, align 8
  store i64 %"\D9\82\D9\87", ptr %"\D9\82\D9\871", align 8
  %"%625.load" = load i64, ptr %dev_addr2, align 8
  store i64 %"%625.load", ptr %"%chars", align 8
  %"%endpoint.load" = load i64, ptr %endpoint3, align 8
  %shltmp = shl i64 %"%endpoint.load", 8
  %"%chars.load" = load i64, ptr %"%chars", align 8
  %addtmp = add i64 %"%chars.load", %shltmp
  store i64 %addtmp, ptr %"%chars", align 8
  %"%speed.load" = load i64, ptr %speed5, align 8
  %cmpeqtmp = icmp eq i64 %"%speed.load", 2
  br i1 %cmpeqtmp, label %then_164, label %else_166

else_166:                                         ; preds = %entry
  %"%speed.load9" = load i64, ptr %speed5, align 8
  %cmpeqtmp10 = icmp eq i64 %"%speed.load9", 0
  br i1 %cmpeqtmp10, label %then_167, label %else_169

else_169:                                         ; preds = %else_166
  %"%chars.load20" = load i64, ptr %"%chars", align 8
  %"%EHCI_QH_EPS_FULL.load" = load i64, ptr @EHCI_QH_EPS_FULL, align 8
  %addtmp21 = add i64 %"%chars.load20", %"%EHCI_QH_EPS_FULL.load"
  store i64 %addtmp21, ptr %"%chars", align 8
  br label %merge_168

merge_165:                                        ; preds = %merge_168, %then_164
  %"%chars.load11" = load i64, ptr %"%chars", align 8
  %"%EHCI_QH_DTC.load" = load i64, ptr @EHCI_QH_DTC, align 8
  %addtmp12 = add i64 %"%chars.load11", %"%EHCI_QH_DTC.load"
  store i64 %addtmp12, ptr %"%chars", align 8
  %"%max_pkt.load" = load i64, ptr %max_pkt4, align 8
  %shltmp13 = shl i64 %"%max_pkt.load", 16
  %"%chars.load14" = load i64, ptr %"%chars", align 8
  %addtmp15 = add i64 %"%chars.load14", %shltmp13
  store i64 %addtmp15, ptr %"%chars", align 8
  %"%endpoint.load16" = load i64, ptr %endpoint3, align 8
  %cmpeqtmp17 = icmp eq i64 %"%endpoint.load16", 0
  br i1 %cmpeqtmp17, label %then_170, label %merge_171

merge_168:                                        ; preds = %else_169, %then_167
  br label %merge_165

merge_171:                                        ; preds = %then_170, %merge_165
  %"%is_head.load" = load i64, ptr %is_head6, align 8
  %cmpeqtmp24 = icmp eq i64 %"%is_head.load", 1
  br i1 %cmpeqtmp24, label %then_172, label %merge_173

merge_173:                                        ; preds = %then_172, %merge_171
  %"%chars.load27" = load i64, ptr %"%chars", align 8
  %addtmp28 = add i64 %"%chars.load27", 4026531840
  store i64 %addtmp28, ptr %"%chars", align 8
  %"%\D9\82\D9\87.load" = load i64, ptr %"\D9\82\D9\871", align 8
  %addtmp29 = add i64 %"%\D9\82\D9\87.load", 4
  %"%chars.load30" = load i64, ptr %"%chars", align 8
  call void @sad_ll_mem_write32(i64 %addtmp29, i64 %"%chars.load30")
  store i64 1073741824, ptr %"%caps", align 8
  %"%speed.load31" = load i64, ptr %speed5, align 8
  %cmpnetmp = icmp ne i64 %"%speed.load31", 2
  br i1 %cmpnetmp, label %then_174, label %merge_175

merge_175:                                        ; preds = %then_174, %merge_173
  %"%\D9\82\D9\87.load35" = load i64, ptr %"\D9\82\D9\871", align 8
  %addtmp36 = add i64 %"%\D9\82\D9\87.load35", 8
  %"%caps.load37" = load i64, ptr %"%caps", align 8
  call void @sad_ll_mem_write32(i64 %addtmp36, i64 %"%caps.load37")
  ret i64 0

then_164:                                         ; preds = %entry
  %"%chars.load7" = load i64, ptr %"%chars", align 8
  %"%EHCI_QH_EPS_HIGH.load" = load i64, ptr @EHCI_QH_EPS_HIGH, align 8
  %addtmp8 = add i64 %"%chars.load7", %"%EHCI_QH_EPS_HIGH.load"
  store i64 %addtmp8, ptr %"%chars", align 8
  br label %merge_165

then_167:                                         ; preds = %else_166
  %"%chars.load18" = load i64, ptr %"%chars", align 8
  %"%EHCI_QH_EPS_LOW.load" = load i64, ptr @EHCI_QH_EPS_LOW, align 8
  %addtmp19 = add i64 %"%chars.load18", %"%EHCI_QH_EPS_LOW.load"
  store i64 %addtmp19, ptr %"%chars", align 8
  br label %merge_168

then_170:                                         ; preds = %merge_165
  %"%chars.load22" = load i64, ptr %"%chars", align 8
  %"%EHCI_QH_C_BIT.load" = load i64, ptr @EHCI_QH_C_BIT, align 8
  %addtmp23 = add i64 %"%chars.load22", %"%EHCI_QH_C_BIT.load"
  store i64 %addtmp23, ptr %"%chars", align 8
  br label %merge_171

then_172:                                         ; preds = %merge_171
  %"%chars.load25" = load i64, ptr %"%chars", align 8
  %"%EHCI_QH_H_BIT.load" = load i64, ptr @EHCI_QH_H_BIT, align 8
  %addtmp26 = add i64 %"%chars.load25", %"%EHCI_QH_H_BIT.load"
  store i64 %addtmp26, ptr %"%chars", align 8
  br label %merge_173

then_174:                                         ; preds = %merge_173
  %"%caps.load" = load i64, ptr %"%caps", align 8
  %addtmp32 = add i64 %"%caps.load", 1
  store i64 %addtmp32, ptr %"%caps", align 8
  %"%caps.load33" = load i64, ptr %"%caps", align 8
  %addtmp34 = add i64 %"%caps.load33", 7168
  store i64 %addtmp34, ptr %"%caps", align 8
  br label %merge_175
}

define i64 @ehci_qh_link_qtd(i64 %"\D9\82\D9\87", i64 %first_qtd) {
entry:
  %first_qtd2 = alloca i64, align 8
  store i64 %first_qtd, ptr %first_qtd2, align 8
  %"\D9\82\D9\871" = alloca i64, align 8
  store i64 %"\D9\82\D9\87", ptr %"\D9\82\D9\871", align 8
  %"%\D9\82\D9\87.load" = load i64, ptr %"\D9\82\D9\871", align 8
  %addtmp = add i64 %"%\D9\82\D9\87.load", 16
  %"%667.load" = load i64, ptr %first_qtd2, align 8
  call void @sad_ll_mem_write32(i64 %addtmp, i64 %"%667.load")
  %"%\D9\82\D9\87.load3" = load i64, ptr %"\D9\82\D9\871", align 8
  %addtmp4 = add i64 %"%\D9\82\D9\87.load3", 24
  call void @sad_ll_mem_write32(i64 %addtmp4, i64 0)
  ret i64 0
}

define i64 @ehci_port_reset(i64 %hc_idx, i64 %port_num) {
entry:
  %"%ehci_companion_routes" = alloca i64, align 8
  %"%d2" = alloca i64, align 8
  %"%w" = alloca i64, align 8
  %"%line_status" = alloca i64, align 8
  %"%owner" = alloca i64, align 8
  %"%enabled" = alloca i64, align 8
  %"%connected" = alloca i64, align 8
  %"%reset_bit" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  %"%d" = alloca i64, align 8
  %"%wait" = alloca i64, align 8
  %"%new_sc" = alloca i64, align 8
  %"%sc" = alloca i64, align 8
  %"%ehci_port_resets" = alloca i64, align 8
  %port_num2 = alloca i64, align 8
  store i64 %port_num, ptr %port_num2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %calltmp = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %"%677.load" = load i64, ptr %hc_idx1, align 8
  %calltmp3 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%677.load")
  %calltmp4 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%682.load" = load i64, ptr %port_num2, align 8
  %calltmp5 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%682.load")
  %calltmp6 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%ehci_port_resets.load" = load i64, ptr @ehci_port_resets, align 8
  %addtmp = add i64 %"%ehci_port_resets.load", 1
  store i64 %addtmp, ptr %"%ehci_port_resets", align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %"%port_num.load" = load i64, ptr %port_num2, align 8
  %calltmp7 = call i64 @ehci_read_port(i64 %"%hc_idx.load", i64 %"%port_num.load")
  store i64 %calltmp7, ptr %"%sc", align 8
  %"%EHCI_PORT_RESET.load" = load i64, ptr @EHCI_PORT_RESET, align 8
  %"%EHCI_PORT_PP.load" = load i64, ptr @EHCI_PORT_PP, align 8
  %addtmp8 = add i64 %"%EHCI_PORT_RESET.load", %"%EHCI_PORT_PP.load"
  store i64 %addtmp8, ptr %"%new_sc", align 8
  %"%hc_idx.load9" = load i64, ptr %hc_idx1, align 8
  %"%port_num.load10" = load i64, ptr %port_num2, align 8
  %"%new_sc.load" = load i64, ptr %"%new_sc", align 8
  %calltmp11 = call i64 @ehci_write_port(i64 %"%hc_idx.load9", i64 %"%port_num.load10", i64 %"%new_sc.load")
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%wait", align 8
  br label %while_cond_176

else_184:                                         ; preds = %while_body_180
  %"%timeout.load55" = load i64, ptr %"%timeout", align 8
  %subtmp56 = sub i64 %"%timeout.load55", 1
  store i64 %subtmp56, ptr %"%timeout", align 8
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%w", align 8
  br label %while_cond_185

merge_183:                                        ; preds = %while_exit_187, %then_182
  br label %while_cond_179

merge_189:                                        ; preds = %merge_191, %while_exit_181
  %"%sc.load60" = load i64, ptr %"%sc", align 8
  %"%EHCI_PORT_CSC.load" = load i64, ptr @EHCI_PORT_CSC, align 8
  %divtmp61 = sdiv i64 %"%sc.load60", %"%EHCI_PORT_CSC.load"
  %andtmp62 = and i64 %divtmp61, 1
  %cmpeqtmp63 = icmp eq i64 %andtmp62, 1
  br i1 %cmpeqtmp63, label %then_192, label %merge_193

merge_191:                                        ; preds = %then_188
  br label %merge_189

merge_193:                                        ; preds = %then_192, %merge_189
  %loadtmp = load i64, ptr %"%connected", align 8
  ret i64 %loadtmp

then_182:                                         ; preds = %while_body_180
  store i64 0, ptr %"%timeout", align 8
  br label %merge_183

then_188:                                         ; preds = %while_exit_181
  %"%enabled.load" = load i64, ptr %"%enabled", align 8
  %cmpeqtmp59 = icmp eq i64 %"%enabled.load", 0
  br i1 %cmpeqtmp59, label %then_190, label %merge_191

then_190:                                         ; preds = %then_188
  %calltmp64 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  %calltmp65 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%EHCI_PORT_OWNER.load66" = load i64, ptr @EHCI_PORT_OWNER, align 8
  %"%EHCI_PORT_PP.load67" = load i64, ptr @EHCI_PORT_PP, align 8
  %addtmp68 = add i64 %"%EHCI_PORT_OWNER.load66", %"%EHCI_PORT_PP.load67"
  %"%hc_idx.load69" = load i64, ptr %hc_idx1, align 8
  %"%port_num.load70" = load i64, ptr %port_num2, align 8
  %calltmp71 = call i64 @ehci_write_port(i64 %"%hc_idx.load69", i64 %"%port_num.load70", i64 %addtmp68)
  %"%ehci_companion_routes.load" = load i64, ptr @ehci_companion_routes, align 8
  %addtmp72 = add i64 %"%ehci_companion_routes.load", 1
  store i64 %addtmp72, ptr %"%ehci_companion_routes", align 8
  ret i64 0

then_192:                                         ; preds = %merge_189
  %"%EHCI_PORT_PP.load73" = load i64, ptr @EHCI_PORT_PP, align 8
  %"%EHCI_PORT_CSC.load74" = load i64, ptr @EHCI_PORT_CSC, align 8
  %addtmp75 = add i64 %"%EHCI_PORT_PP.load73", %"%EHCI_PORT_CSC.load74"
  %"%hc_idx.load76" = load i64, ptr %hc_idx1, align 8
  %"%port_num.load77" = load i64, ptr %port_num2, align 8
  %calltmp78 = call i64 @ehci_write_port(i64 %"%hc_idx.load76", i64 %"%port_num.load77", i64 %addtmp75)
  br label %merge_193

while_body_177:                                   ; preds = %while_cond_176
  store i64 0, ptr %"%d", align 8
  br label %while_cond_176

while_body_180:                                   ; preds = %while_cond_179
  %"%hc_idx.load25" = load i64, ptr %hc_idx1, align 8
  %"%port_num.load26" = load i64, ptr %port_num2, align 8
  %calltmp27 = call i64 @ehci_read_port(i64 %"%hc_idx.load25", i64 %"%port_num.load26")
  store i64 %calltmp27, ptr %"%sc", align 8
  %"%sc.load28" = load i64, ptr %"%sc", align 8
  %"%EHCI_PORT_RESET.load29" = load i64, ptr @EHCI_PORT_RESET, align 8
  %divtmp = sdiv i64 %"%sc.load28", %"%EHCI_PORT_RESET.load29"
  %andtmp30 = and i64 %divtmp, 1
  store i64 %andtmp30, ptr %"%reset_bit", align 8
  %"%reset_bit.load" = load i64, ptr %"%reset_bit", align 8
  %cmpeqtmp = icmp eq i64 %"%reset_bit.load", 0
  br i1 %cmpeqtmp, label %then_182, label %else_184

while_body_186:                                   ; preds = %while_cond_185
  store i64 0, ptr %"%d2", align 8
  br label %while_cond_185

while_cond_176:                                   ; preds = %while_body_177, %entry
  call void @sad_ll_timer_get_ticks()
  %"%wait.load" = load i64, ptr %"%wait", align 8
  %subtmp = sub i64 0, %"%wait.load"
  %cmplttmp = icmp slt i64 %subtmp, 5000
  br i1 %cmplttmp, label %while_body_177, label %while_exit_178

while_cond_179:                                   ; preds = %merge_183, %while_exit_178
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp, label %while_body_180, label %while_exit_181

while_cond_185:                                   ; preds = %while_body_186, %else_184
  call void @sad_ll_timer_get_ticks()
  %"%w.load" = load i64, ptr %"%w", align 8
  %subtmp57 = sub i64 0, %"%w.load"
  %cmplttmp58 = icmp slt i64 %subtmp57, 100
  br i1 %cmplttmp58, label %while_body_186, label %while_exit_187

while_exit_178:                                   ; preds = %while_cond_176
  %"%hc_idx.load12" = load i64, ptr %hc_idx1, align 8
  %"%port_num.load13" = load i64, ptr %port_num2, align 8
  %calltmp14 = call i64 @ehci_read_port(i64 %"%hc_idx.load12", i64 %"%port_num.load13")
  store i64 %calltmp14, ptr %"%sc", align 8
  %"%sc.load" = load i64, ptr %"%sc", align 8
  %andtmp = and i64 %"%sc.load", 511
  %"%sc.load15" = load i64, ptr %"%sc", align 8
  %subtmp16 = sub i64 %"%sc.load15", %andtmp
  store i64 %subtmp16, ptr %"%new_sc", align 8
  %"%new_sc.load17" = load i64, ptr %"%new_sc", align 8
  %"%EHCI_PORT_PP.load18" = load i64, ptr @EHCI_PORT_PP, align 8
  %addtmp19 = add i64 %"%new_sc.load17", %"%EHCI_PORT_PP.load18"
  %"%EHCI_PORT_ENABLE.load" = load i64, ptr @EHCI_PORT_ENABLE, align 8
  %addtmp20 = add i64 %addtmp19, %"%EHCI_PORT_ENABLE.load"
  store i64 %addtmp20, ptr %"%new_sc", align 8
  %"%hc_idx.load21" = load i64, ptr %hc_idx1, align 8
  %"%port_num.load22" = load i64, ptr %port_num2, align 8
  %"%EHCI_PORT_PP.load23" = load i64, ptr @EHCI_PORT_PP, align 8
  %calltmp24 = call i64 @ehci_write_port(i64 %"%hc_idx.load21", i64 %"%port_num.load22", i64 %"%EHCI_PORT_PP.load23")
  store i64 200, ptr %"%timeout", align 8
  br label %while_cond_179

while_exit_181:                                   ; preds = %while_cond_179
  %"%hc_idx.load31" = load i64, ptr %hc_idx1, align 8
  %"%port_num.load32" = load i64, ptr %port_num2, align 8
  %calltmp33 = call i64 @ehci_read_port(i64 %"%hc_idx.load31", i64 %"%port_num.load32")
  store i64 %calltmp33, ptr %"%sc", align 8
  %"%sc.load34" = load i64, ptr %"%sc", align 8
  %andtmp35 = and i64 %"%sc.load34", 1
  store i64 %andtmp35, ptr %"%connected", align 8
  %"%sc.load36" = load i64, ptr %"%sc", align 8
  %"%EHCI_PORT_ENABLE.load37" = load i64, ptr @EHCI_PORT_ENABLE, align 8
  %divtmp38 = sdiv i64 %"%sc.load36", %"%EHCI_PORT_ENABLE.load37"
  %andtmp39 = and i64 %divtmp38, 1
  store i64 %andtmp39, ptr %"%enabled", align 8
  %"%sc.load40" = load i64, ptr %"%sc", align 8
  %"%EHCI_PORT_OWNER.load" = load i64, ptr @EHCI_PORT_OWNER, align 8
  %divtmp41 = sdiv i64 %"%sc.load40", %"%EHCI_PORT_OWNER.load"
  %andtmp42 = and i64 %divtmp41, 1
  store i64 %andtmp42, ptr %"%owner", align 8
  %"%sc.load43" = load i64, ptr %"%sc", align 8
  %shrtmp = lshr i64 %"%sc.load43", 10
  %andtmp44 = and i64 %shrtmp, 3
  store i64 %andtmp44, ptr %"%line_status", align 8
  %calltmp45 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %"%751.load" = load i64, ptr %"%connected", align 8
  %calltmp46 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%751.load")
  %calltmp47 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  %"%756.load" = load i64, ptr %"%enabled", align 8
  %calltmp48 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%756.load")
  %calltmp49 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %"%761.load" = load i64, ptr %"%owner", align 8
  %calltmp50 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%761.load")
  %calltmp51 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %"%766.load" = load i64, ptr %"%line_status", align 8
  %calltmp52 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%766.load")
  %calltmp53 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%connected.load" = load i64, ptr %"%connected", align 8
  %cmpeqtmp54 = icmp eq i64 %"%connected.load", 1
  br i1 %cmpeqtmp54, label %then_188, label %merge_189

while_exit_187:                                   ; preds = %while_cond_185
  br label %merge_183
}

define i64 @ehci_control_transfer(i64 %hc_idx, i64 %dev_addr, i64 %setup_buf, i64 %data_buf, i64 %data_len, i64 %is_read) {
entry:
  %"%rem" = alloca i64, align 8
  %"%cc" = alloca i64, align 8
  %"%cq" = alloca i64, align 8
  %"%total" = alloca i64, align 8
  %"%new_cmd" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  %"%cmd" = alloca i64, align 8
  %"%async_qh" = alloca i64, align 8
  %"%status_pid" = alloca i64, align 8
  %"%pkt" = alloca i64, align 8
  %"%qtd_status" = alloca i64, align 8
  %"%qtd_data" = alloca i64, align 8
  %"%pid" = alloca i64, align 8
  %"%offset" = alloca i64, align 8
  %"%remaining" = alloca i64, align 8
  %"%toggle" = alloca i64, align 8
  %"%qtd_data_last" = alloca i64, align 8
  %"%qtd_data_first" = alloca i64, align 8
  %"%ehci_transfer_errors" = alloca i64, align 8
  %"%qtd_setup" = alloca i64, align 8
  %"%ehci_total_transfers" = alloca i64, align 8
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
  %"%ehci_total_transfers.load" = load i64, ptr @ehci_total_transfers, align 8
  %addtmp = add i64 %"%ehci_total_transfers.load", 1
  store i64 %addtmp, ptr %"%ehci_total_transfers", align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @ehci_reset_qtd_pool(i64 %"%hc_idx.load")
  %"%hc_idx.load7" = load i64, ptr %hc_idx1, align 8
  %calltmp8 = call i64 @ehci_alloc_qtd(i64 %"%hc_idx.load7")
  store i64 %calltmp8, ptr %"%qtd_setup", align 8
  %"%qtd_setup.load" = load i64, ptr %"%qtd_setup", align 8
  %cmpeqtmp = icmp eq i64 %"%qtd_setup.load", 0
  br i1 %cmpeqtmp, label %then_194, label %merge_195

else_215:                                         ; preds = %merge_212
  %"%qtd_setup.load57" = load i64, ptr %"%qtd_setup", align 8
  %"%qtd_status.load58" = load i64, ptr %"%qtd_status", align 8
  %calltmp59 = call i64 @ehci_qtd_set_next(i64 %"%qtd_setup.load57", i64 %"%qtd_status.load58", i64 0)
  br label %merge_214

else_236:                                         ; preds = %merge_233
  %"%cq.load134" = load i64, ptr %"%cq", align 8
  %addtmp135 = add i64 %"%cq.load134", 32
  store i64 %addtmp135, ptr %"%cq", align 8
  %"%cc.load136" = load i64, ptr %"%cc", align 8
  %addtmp137 = add i64 %"%cc.load136", 1
  store i64 %addtmp137, ptr %"%cc", align 8
  br label %merge_235

merge_195:                                        ; preds = %entry
  %"%qtd_setup.load10" = load i64, ptr %"%qtd_setup", align 8
  %"%EHCI_QTD_PID_SETUP.load" = load i64, ptr @EHCI_QTD_PID_SETUP, align 8
  %calltmp11 = call i64 @ehci_qtd_set_token(i64 %"%qtd_setup.load10", i64 %"%EHCI_QTD_PID_SETUP.load", i64 8, i64 0, i64 3, i64 0)
  %"%qtd_setup.load12" = load i64, ptr %"%qtd_setup", align 8
  %"%setup_buf.load" = load i64, ptr %setup_buf3, align 8
  %calltmp13 = call i64 @ehci_qtd_set_buffer(i64 %"%qtd_setup.load12", i64 %"%setup_buf.load")
  store i64 0, ptr %"%qtd_data_first", align 8
  store i64 0, ptr %"%qtd_data_last", align 8
  store i64 1, ptr %"%toggle", align 8
  %"%821.load" = load i64, ptr %data_len5, align 8
  store i64 %"%821.load", ptr %"%remaining", align 8
  store i64 0, ptr %"%offset", align 8
  %"%826.load" = load i64, ptr @EHCI_QTD_PID_IN, align 8
  store i64 %"%826.load", ptr %"%pid", align 8
  %"%is_read.load" = load i64, ptr %is_read6, align 8
  %cmpeqtmp14 = icmp eq i64 %"%is_read.load", 0
  br i1 %cmpeqtmp14, label %then_196, label %merge_197

merge_197:                                        ; preds = %then_196, %merge_195
  br label %while_cond_198

merge_202:                                        ; preds = %while_body_199
  %"%qtd_data_first.load" = load i64, ptr %"%qtd_data_first", align 8
  %cmpeqtmp23 = icmp eq i64 %"%qtd_data_first.load", 0
  br i1 %cmpeqtmp23, label %then_203, label %merge_204

merge_204:                                        ; preds = %then_203, %merge_202
  %"%qtd_data_last.load" = load i64, ptr %"%qtd_data_last", align 8
  %cmpnetmp = icmp ne i64 %"%qtd_data_last.load", 0
  br i1 %cmpnetmp, label %then_205, label %merge_206

merge_206:                                        ; preds = %then_205, %merge_204
  %"%853.load" = load i64, ptr %"%remaining", align 8
  store i64 %"%853.load", ptr %"%pkt", align 8
  %"%pkt.load" = load i64, ptr %"%pkt", align 8
  %cmpgttmp27 = icmp sgt i64 %"%pkt.load", 512
  br i1 %cmpgttmp27, label %then_207, label %merge_208

merge_208:                                        ; preds = %then_207, %merge_206
  %"%qtd_data.load28" = load i64, ptr %"%qtd_data", align 8
  %"%pid.load" = load i64, ptr %"%pid", align 8
  %"%pkt.load29" = load i64, ptr %"%pkt", align 8
  %"%toggle.load" = load i64, ptr %"%toggle", align 8
  %calltmp30 = call i64 @ehci_qtd_set_token(i64 %"%qtd_data.load28", i64 %"%pid.load", i64 %"%pkt.load29", i64 %"%toggle.load", i64 3, i64 0)
  %"%data_buf.load" = load i64, ptr %data_buf4, align 8
  %"%offset.load" = load i64, ptr %"%offset", align 8
  %addtmp31 = add i64 %"%data_buf.load", %"%offset.load"
  %"%qtd_data.load32" = load i64, ptr %"%qtd_data", align 8
  %calltmp33 = call i64 @ehci_qtd_set_buffer(i64 %"%qtd_data.load32", i64 %addtmp31)
  %"%863.load" = load i64, ptr %"%qtd_data", align 8
  store i64 %"%863.load", ptr %"%qtd_data_last", align 8
  %"%offset.load34" = load i64, ptr %"%offset", align 8
  %"%pkt.load35" = load i64, ptr %"%pkt", align 8
  %addtmp36 = add i64 %"%offset.load34", %"%pkt.load35"
  store i64 %addtmp36, ptr %"%offset", align 8
  %"%remaining.load37" = load i64, ptr %"%remaining", align 8
  %"%pkt.load38" = load i64, ptr %"%pkt", align 8
  %subtmp = sub i64 %"%remaining.load37", %"%pkt.load38"
  store i64 %subtmp, ptr %"%remaining", align 8
  %"%toggle.load39" = load i64, ptr %"%toggle", align 8
  %subtmp40 = sub i64 1, %"%toggle.load39"
  store i64 %subtmp40, ptr %"%toggle", align 8
  br label %while_cond_198

merge_210:                                        ; preds = %while_exit_200
  %"%880.load" = load i64, ptr @EHCI_QTD_PID_OUT, align 8
  store i64 %"%880.load", ptr %"%status_pid", align 8
  %"%is_read.load43" = load i64, ptr %is_read6, align 8
  %cmpeqtmp44 = icmp eq i64 %"%is_read.load43", 0
  br i1 %cmpeqtmp44, label %then_211, label %merge_212

merge_212:                                        ; preds = %then_211, %merge_210
  %"%qtd_status.load45" = load i64, ptr %"%qtd_status", align 8
  %"%status_pid.load" = load i64, ptr %"%status_pid", align 8
  %calltmp46 = call i64 @ehci_qtd_set_token(i64 %"%qtd_status.load45", i64 %"%status_pid.load", i64 0, i64 1, i64 3, i64 1)
  %"%qtd_status.load47" = load i64, ptr %"%qtd_status", align 8
  %calltmp48 = call i64 @ehci_qtd_set_next(i64 %"%qtd_status.load47", i64 0, i64 1)
  %"%qtd_data_first.load49" = load i64, ptr %"%qtd_data_first", align 8
  %cmpnetmp50 = icmp ne i64 %"%qtd_data_first.load49", 0
  br i1 %cmpnetmp50, label %then_213, label %else_215

merge_214:                                        ; preds = %else_215, %then_213
  %"%hc_idx.load60" = load i64, ptr %hc_idx1, align 8
  %calltmp61 = call i64 @ehci_get_async_qh(i64 %"%hc_idx.load60")
  store i64 %calltmp61, ptr %"%async_qh", align 8
  %"%async_qh.load" = load i64, ptr %"%async_qh", align 8
  %"%dev_addr.load" = load i64, ptr %dev_addr2, align 8
  %calltmp62 = call i64 @ehci_qh_setup(i64 %"%async_qh.load", i64 %"%dev_addr.load", i64 0, i64 64, i64 2, i64 1)
  %"%async_qh.load63" = load i64, ptr %"%async_qh", align 8
  %addtmp64 = add i64 %"%async_qh.load63", 2
  %"%async_qh.load65" = load i64, ptr %"%async_qh", align 8
  call void @sad_ll_mem_write32(i64 %"%async_qh.load65", i64 %addtmp64)
  %"%async_qh.load66" = load i64, ptr %"%async_qh", align 8
  %"%qtd_setup.load67" = load i64, ptr %"%qtd_setup", align 8
  %calltmp68 = call i64 @ehci_qh_link_qtd(i64 %"%async_qh.load66", i64 %"%qtd_setup.load67")
  %"%hc_idx.load69" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_ASYNCLIST.load" = load i64, ptr @EHCI_OP_ASYNCLIST, align 8
  %"%async_qh.load70" = load i64, ptr %"%async_qh", align 8
  %calltmp71 = call i64 @ehci_write_op(i64 %"%hc_idx.load69", i64 %"%EHCI_OP_ASYNCLIST.load", i64 %"%async_qh.load70")
  %"%hc_idx.load72" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_USBCMD.load" = load i64, ptr @EHCI_OP_USBCMD, align 8
  %calltmp73 = call i64 @ehci_read_op(i64 %"%hc_idx.load72", i64 %"%EHCI_OP_USBCMD.load")
  store i64 %calltmp73, ptr %"%cmd", align 8
  %"%cmd.load" = load i64, ptr %"%cmd", align 8
  %"%EHCI_CMD_ASE.load" = load i64, ptr @EHCI_CMD_ASE, align 8
  %addtmp74 = add i64 %"%cmd.load", %"%EHCI_CMD_ASE.load"
  %"%EHCI_CMD_RS.load" = load i64, ptr @EHCI_CMD_RS, align 8
  %addtmp75 = add i64 %addtmp74, %"%EHCI_CMD_RS.load"
  %"%hc_idx.load76" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_USBCMD.load77" = load i64, ptr @EHCI_OP_USBCMD, align 8
  %calltmp78 = call i64 @ehci_write_op(i64 %"%hc_idx.load76", i64 %"%EHCI_OP_USBCMD.load77", i64 %addtmp75)
  store i64 50000, ptr %"%timeout", align 8
  br label %while_cond_216

merge_220:                                        ; preds = %while_body_217
  %"%timeout.load102" = load i64, ptr %"%timeout", align 8
  %subtmp103 = sub i64 %"%timeout.load102", 1
  store i64 %subtmp103, ptr %"%timeout", align 8
  br label %while_cond_216

merge_222:                                        ; preds = %then_221, %then_219
  %"%hc_idx.load104" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_USBCMD.load105" = load i64, ptr @EHCI_OP_USBCMD, align 8
  %"%new_cmd.load106" = load i64, ptr %"%new_cmd", align 8
  %calltmp107 = call i64 @ehci_write_op(i64 %"%hc_idx.load104", i64 %"%EHCI_OP_USBCMD.load105", i64 %"%new_cmd.load106")
  %"%qtd_setup.load108" = load i64, ptr %"%qtd_setup", align 8
  %calltmp109 = call i64 @ehci_qtd_has_error(i64 %"%qtd_setup.load108")
  %cmpeqtmp110 = icmp eq i64 %calltmp109, 1
  br i1 %cmpeqtmp110, label %then_223, label %merge_224

merge_224:                                        ; preds = %merge_222
  %"%qtd_status.load115" = load i64, ptr %"%qtd_status", align 8
  %calltmp116 = call i64 @ehci_qtd_has_error(i64 %"%qtd_status.load115")
  %cmpeqtmp117 = icmp eq i64 %calltmp116, 1
  br i1 %cmpeqtmp117, label %then_225, label %merge_226

merge_226:                                        ; preds = %merge_224
  store i64 0, ptr %"%total", align 8
  %"%qtd_data_first.load122" = load i64, ptr %"%qtd_data_first", align 8
  %cmpnetmp123 = icmp ne i64 %"%qtd_data_first.load122", 0
  br i1 %cmpnetmp123, label %then_227, label %merge_228

merge_228:                                        ; preds = %while_exit_231, %merge_226
  %loadtmp = load i64, ptr %"%total", align 8
  ret i64 %loadtmp

merge_233:                                        ; preds = %while_body_230
  %"%cq.load129" = load i64, ptr %"%cq", align 8
  %calltmp130 = call i64 @ehci_qtd_actual_len(i64 %"%cq.load129")
  store i64 %calltmp130, ptr %"%rem", align 8
  %"%cq.load131" = load i64, ptr %"%cq", align 8
  %"%qtd_data_last.load132" = load i64, ptr %"%qtd_data_last", align 8
  %cmpeqtmp133 = icmp eq i64 %"%cq.load131", %"%qtd_data_last.load132"
  br i1 %cmpeqtmp133, label %then_234, label %else_236

merge_235:                                        ; preds = %else_236, %then_234
  br label %while_cond_229

then_194:                                         ; preds = %entry
  %"%ehci_transfer_errors.load" = load i64, ptr @ehci_transfer_errors, align 8
  %addtmp9 = add i64 %"%ehci_transfer_errors.load", 1
  store i64 %addtmp9, ptr %"%ehci_transfer_errors", align 8
  ret i64 -1

then_196:                                         ; preds = %merge_195
  %"%830.load" = load i64, ptr @EHCI_QTD_PID_OUT, align 8
  store i64 %"%830.load", ptr %"%pid", align 8
  br label %merge_197

then_201:                                         ; preds = %while_body_199
  %"%ehci_transfer_errors.load21" = load i64, ptr %"%ehci_transfer_errors", align 8
  %addtmp22 = add i64 %"%ehci_transfer_errors.load21", 1
  store i64 %addtmp22, ptr %"%ehci_transfer_errors", align 8
  ret i64 -1

then_203:                                         ; preds = %merge_202
  %"%847.load" = load i64, ptr %"%qtd_data", align 8
  store i64 %"%847.load", ptr %"%qtd_data_first", align 8
  br label %merge_204

then_205:                                         ; preds = %merge_204
  %"%qtd_data_last.load24" = load i64, ptr %"%qtd_data_last", align 8
  %"%qtd_data.load25" = load i64, ptr %"%qtd_data", align 8
  %calltmp26 = call i64 @ehci_qtd_set_next(i64 %"%qtd_data_last.load24", i64 %"%qtd_data.load25", i64 0)
  br label %merge_206

then_207:                                         ; preds = %merge_206
  store i64 512, ptr %"%pkt", align 8
  br label %merge_208

then_209:                                         ; preds = %while_exit_200
  %"%ehci_transfer_errors.load41" = load i64, ptr %"%ehci_transfer_errors", align 8
  %addtmp42 = add i64 %"%ehci_transfer_errors.load41", 1
  store i64 %addtmp42, ptr %"%ehci_transfer_errors", align 8
  ret i64 -1

then_211:                                         ; preds = %merge_210
  %"%884.load" = load i64, ptr @EHCI_QTD_PID_IN, align 8
  store i64 %"%884.load", ptr %"%status_pid", align 8
  br label %merge_212

then_213:                                         ; preds = %merge_212
  %"%qtd_setup.load51" = load i64, ptr %"%qtd_setup", align 8
  %"%qtd_data_first.load52" = load i64, ptr %"%qtd_data_first", align 8
  %calltmp53 = call i64 @ehci_qtd_set_next(i64 %"%qtd_setup.load51", i64 %"%qtd_data_first.load52", i64 0)
  %"%qtd_data_last.load54" = load i64, ptr %"%qtd_data_last", align 8
  %"%qtd_status.load55" = load i64, ptr %"%qtd_status", align 8
  %calltmp56 = call i64 @ehci_qtd_set_next(i64 %"%qtd_data_last.load54", i64 %"%qtd_status.load55", i64 0)
  br label %merge_214

then_219:                                         ; preds = %while_body_217
  %"%hc_idx.load96" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_USBCMD.load97" = load i64, ptr @EHCI_OP_USBCMD, align 8
  %calltmp98 = call i64 @ehci_read_op(i64 %"%hc_idx.load96", i64 %"%EHCI_OP_USBCMD.load97")
  store i64 %calltmp98, ptr %"%cmd", align 8
  %"%cmd.load99" = load i64, ptr %"%cmd", align 8
  %"%EHCI_CMD_ASE.load100" = load i64, ptr @EHCI_CMD_ASE, align 8
  %subtmp101 = sub i64 %"%cmd.load99", %"%EHCI_CMD_ASE.load100"
  store i64 %subtmp101, ptr %"%new_cmd", align 8
  %"%new_cmd.load" = load i64, ptr %"%new_cmd", align 8
  %cmplttmp = icmp slt i64 %"%new_cmd.load", 0
  br i1 %cmplttmp, label %then_221, label %merge_222

then_221:                                         ; preds = %then_219
  %"%935.load" = load i64, ptr %"%cmd", align 8
  store i64 %"%935.load", ptr %"%new_cmd", align 8
  br label %merge_222

then_223:                                         ; preds = %merge_222
  %calltmp111 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %calltmp112 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%ehci_transfer_errors.load113" = load i64, ptr %"%ehci_transfer_errors", align 8
  %addtmp114 = add i64 %"%ehci_transfer_errors.load113", 1
  store i64 %addtmp114, ptr %"%ehci_transfer_errors", align 8
  ret i64 -2

then_225:                                         ; preds = %merge_224
  %calltmp118 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %calltmp119 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%ehci_transfer_errors.load120" = load i64, ptr %"%ehci_transfer_errors", align 8
  %addtmp121 = add i64 %"%ehci_transfer_errors.load120", 1
  store i64 %addtmp121, ptr %"%ehci_transfer_errors", align 8
  ret i64 -3

then_227:                                         ; preds = %merge_226
  %"%967.load" = load i64, ptr %"%qtd_data_first", align 8
  store i64 %"%967.load", ptr %"%cq", align 8
  store i64 0, ptr %"%cc", align 8
  br label %while_cond_229

then_232:                                         ; preds = %while_body_230
  %"%ehci_transfer_errors.load127" = load i64, ptr %"%ehci_transfer_errors", align 8
  %addtmp128 = add i64 %"%ehci_transfer_errors.load127", 1
  store i64 %addtmp128, ptr %"%ehci_transfer_errors", align 8
  ret i64 -4

then_234:                                         ; preds = %merge_233
  store i64 32, ptr %"%cc", align 8
  br label %merge_235

while_body_199:                                   ; preds = %while_cond_198
  %"%hc_idx.load15" = load i64, ptr %hc_idx1, align 8
  %calltmp16 = call i64 @ehci_alloc_qtd(i64 %"%hc_idx.load15")
  store i64 %calltmp16, ptr %"%qtd_data", align 8
  %"%qtd_data.load" = load i64, ptr %"%qtd_data", align 8
  %cmpeqtmp17 = icmp eq i64 %"%qtd_data.load", 0
  br i1 %cmpeqtmp17, label %then_201, label %merge_202

while_body_217:                                   ; preds = %while_cond_216
  %"%qtd_status.load80" = load i64, ptr %"%qtd_status", align 8
  %calltmp81 = call i64 @ehci_qtd_is_active(i64 %"%qtd_status.load80")
  %cmpeqtmp82 = icmp eq i64 %calltmp81, 0
  br i1 %cmpeqtmp82, label %then_219, label %merge_220

while_body_230:                                   ; preds = %while_cond_229
  %"%cq.load" = load i64, ptr %"%cq", align 8
  %calltmp125 = call i64 @ehci_qtd_has_error(i64 %"%cq.load")
  %cmpeqtmp126 = icmp eq i64 %calltmp125, 1
  br i1 %cmpeqtmp126, label %then_232, label %merge_233

while_cond_198:                                   ; preds = %merge_208, %merge_197
  %"%remaining.load" = load i64, ptr %"%remaining", align 8
  %cmpgttmp = icmp sgt i64 %"%remaining.load", 0
  br i1 %cmpgttmp, label %while_body_199, label %while_exit_200

while_cond_216:                                   ; preds = %merge_220, %merge_214
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp79 = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp79, label %while_body_217, label %while_exit_218

while_cond_229:                                   ; preds = %merge_235, %then_227
  %"%cc.load" = load i64, ptr %"%cc", align 8
  %cmplttmp124 = icmp slt i64 %"%cc.load", 32
  br i1 %cmplttmp124, label %while_body_230, label %while_exit_231

while_exit_200:                                   ; preds = %while_cond_198
  %"%hc_idx.load18" = load i64, ptr %hc_idx1, align 8
  %calltmp19 = call i64 @ehci_alloc_qtd(i64 %"%hc_idx.load18")
  store i64 %calltmp19, ptr %"%qtd_status", align 8
  %"%qtd_status.load" = load i64, ptr %"%qtd_status", align 8
  %cmpeqtmp20 = icmp eq i64 %"%qtd_status.load", 0
  br i1 %cmpeqtmp20, label %then_209, label %merge_210

while_exit_218:                                   ; preds = %while_cond_216
  %"%hc_idx.load83" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_USBCMD.load84" = load i64, ptr @EHCI_OP_USBCMD, align 8
  %calltmp85 = call i64 @ehci_read_op(i64 %"%hc_idx.load83", i64 %"%EHCI_OP_USBCMD.load84")
  store i64 %calltmp85, ptr %"%cmd", align 8
  %"%cmd.load86" = load i64, ptr %"%cmd", align 8
  %"%EHCI_CMD_ASE.load87" = load i64, ptr @EHCI_CMD_ASE, align 8
  %subtmp88 = sub i64 %"%cmd.load86", %"%EHCI_CMD_ASE.load87"
  %"%hc_idx.load89" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_USBCMD.load90" = load i64, ptr @EHCI_OP_USBCMD, align 8
  %calltmp91 = call i64 @ehci_write_op(i64 %"%hc_idx.load89", i64 %"%EHCI_OP_USBCMD.load90", i64 %subtmp88)
  %calltmp92 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %calltmp93 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%ehci_transfer_errors.load94" = load i64, ptr %"%ehci_transfer_errors", align 8
  %addtmp95 = add i64 %"%ehci_transfer_errors.load94", 1
  store i64 %addtmp95, ptr %"%ehci_transfer_errors", align 8
  ret i64 -5

while_exit_231:                                   ; preds = %while_cond_229
  %"%990.load" = load i64, ptr %data_len5, align 8
  store i64 %"%990.load", ptr %"%total", align 8
  br label %merge_228
}

define i64 @ehci_get_device_descriptor(i64 %hc_idx, i64 %dev_addr, i64 %buf, i64 %buf_len) {
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
  %calltmp = call i64 @ehci_get_setup_buf(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%setup", align 8
  %"%setup.load" = load i64, ptr %"%setup", align 8
  %"%buf_len.load" = load i64, ptr %buf_len4, align 8
  call void @usb_build_setup(i64 %"%setup.load", i64 128, i64 6, i64 256, i64 0, i64 %"%buf_len.load")
  %"%hc_idx.load5" = load i64, ptr %hc_idx1, align 8
  %"%dev_addr.load" = load i64, ptr %dev_addr2, align 8
  %"%setup.load6" = load i64, ptr %"%setup", align 8
  %"%buf.load" = load i64, ptr %buf3, align 8
  %"%buf_len.load7" = load i64, ptr %buf_len4, align 8
  %calltmp8 = call i64 @ehci_control_transfer(i64 %"%hc_idx.load5", i64 %"%dev_addr.load", i64 %"%setup.load6", i64 %"%buf.load", i64 %"%buf_len.load7", i64 1)
  ret i64 %calltmp8
}

define i64 @ehci_get_config_descriptor(i64 %hc_idx, i64 %dev_addr, i64 %config_idx, i64 %buf, i64 %buf_len) {
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
  %calltmp = call i64 @ehci_get_setup_buf(i64 %"%hc_idx.load")
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
  %calltmp9 = call i64 @ehci_control_transfer(i64 %"%hc_idx.load6", i64 %"%dev_addr.load", i64 %"%setup.load7", i64 %"%buf.load", i64 %"%buf_len.load8", i64 1)
  ret i64 %calltmp9
}

define i64 @ehci_set_address(i64 %hc_idx, i64 %old_addr, i64 %new_addr) {
entry:
  %"%setup" = alloca i64, align 8
  %new_addr3 = alloca i64, align 8
  store i64 %new_addr, ptr %new_addr3, align 8
  %old_addr2 = alloca i64, align 8
  store i64 %old_addr, ptr %old_addr2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @ehci_get_setup_buf(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%setup", align 8
  %"%setup.load" = load i64, ptr %"%setup", align 8
  %"%new_addr.load" = load i64, ptr %new_addr3, align 8
  call void @usb_build_setup(i64 %"%setup.load", i64 0, i64 5, i64 %"%new_addr.load", i64 0, i64 0)
  %"%hc_idx.load4" = load i64, ptr %hc_idx1, align 8
  %"%old_addr.load" = load i64, ptr %old_addr2, align 8
  %"%setup.load5" = load i64, ptr %"%setup", align 8
  %calltmp6 = call i64 @ehci_control_transfer(i64 %"%hc_idx.load4", i64 %"%old_addr.load", i64 %"%setup.load5", i64 0, i64 0, i64 0)
  ret i64 %calltmp6
}

define i64 @ehci_set_configuration(i64 %hc_idx, i64 %dev_addr, i64 %config_value) {
entry:
  %"%setup" = alloca i64, align 8
  %config_value3 = alloca i64, align 8
  store i64 %config_value, ptr %config_value3, align 8
  %dev_addr2 = alloca i64, align 8
  store i64 %dev_addr, ptr %dev_addr2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @ehci_get_setup_buf(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%setup", align 8
  %"%setup.load" = load i64, ptr %"%setup", align 8
  %"%config_value.load" = load i64, ptr %config_value3, align 8
  call void @usb_build_setup(i64 %"%setup.load", i64 0, i64 9, i64 %"%config_value.load", i64 0, i64 0)
  %"%hc_idx.load4" = load i64, ptr %hc_idx1, align 8
  %"%dev_addr.load" = load i64, ptr %dev_addr2, align 8
  %"%setup.load5" = load i64, ptr %"%setup", align 8
  %calltmp6 = call i64 @ehci_control_transfer(i64 %"%hc_idx.load4", i64 %"%dev_addr.load", i64 %"%setup.load5", i64 0, i64 0, i64 0)
  ret i64 %calltmp6
}

define i64 @ehci_scan_ports(i64 %hc_idx) {
entry:
  %"%d" = alloca i64, align 8
  %"%wait" = alloca i64, align 8
  %"%new_addr" = alloca i64, align 8
  %"%res" = alloca i64, align 8
  %"%data_buf" = alloca i64, align 8
  %"%dev_idx" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%connected" = alloca i64, align 8
  %"%sc" = alloca i64, align 8
  %"%p" = alloca i64, align 8
  %"%devices_found" = alloca i64, align 8
  %"%num_ports" = alloca i64, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @ehci_get_num_ports(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%num_ports", align 8
  %calltmp2 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %"%1073.load" = load i64, ptr %"%num_ports", align 8
  %calltmp3 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1073.load")
  %calltmp4 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  %"%1078.load" = load i64, ptr %hc_idx1, align 8
  %calltmp5 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1078.load")
  %calltmp6 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%devices_found", align 8
  store i64 0, ptr %"%p", align 8
  br label %while_cond_237

else_242:                                         ; preds = %while_body_238
  %"%hc_idx.load13" = load i64, ptr %hc_idx1, align 8
  %"%p.load14" = load i64, ptr %"%p", align 8
  %calltmp15 = call i64 @ehci_read_port(i64 %"%hc_idx.load13", i64 %"%p.load14")
  store i64 %calltmp15, ptr %"%sc", align 8
  %"%sc.load" = load i64, ptr %"%sc", align 8
  %andtmp = and i64 %"%sc.load", 1
  store i64 %andtmp, ptr %"%connected", align 8
  %"%connected.load" = load i64, ptr %"%connected", align 8
  %cmpeqtmp = icmp eq i64 %"%connected.load", 1
  br i1 %cmpeqtmp, label %then_243, label %else_245

else_245:                                         ; preds = %else_242
  %calltmp23 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %"%1159.load" = load i64, ptr %"%p", align 8
  %calltmp24 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1159.load")
  %calltmp25 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  %calltmp26 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_244

merge_241:                                        ; preds = %merge_244, %then_240
  br label %while_cond_237

merge_244:                                        ; preds = %merge_247, %else_245
  %"%p.load27" = load i64, ptr %"%p", align 8
  %addtmp = add i64 %"%p.load27", 1
  store i64 %addtmp, ptr %"%p", align 8
  br label %merge_241

merge_247:                                        ; preds = %merge_249, %then_243
  br label %merge_244

merge_249:                                        ; preds = %merge_251, %then_246
  br label %merge_247

merge_251:                                        ; preds = %merge_256, %then_248
  br label %merge_249

merge_256:                                        ; preds = %then_255, %while_exit_254
  %"%hc_idx.load51" = load i64, ptr %hc_idx1, align 8
  %"%new_addr.load52" = load i64, ptr %"%new_addr", align 8
  %calltmp53 = call i64 @ehci_set_configuration(i64 %"%hc_idx.load51", i64 %"%new_addr.load52", i64 1)
  %"%dev_idx.load54" = load i64, ptr %"%dev_idx", align 8
  call void @usb_classify_device(i64 %"%dev_idx.load54")
  %"%devices_found.load" = load i64, ptr %"%devices_found", align 8
  %addtmp55 = add i64 %"%devices_found.load", 1
  store i64 %addtmp55, ptr %"%devices_found", align 8
  br label %merge_251

then_240:                                         ; preds = %while_body_238
  %"%num_ports.load12" = load i64, ptr %"%num_ports", align 8
  store i64 %"%num_ports.load12", ptr %"%p", align 8
  br label %merge_241

then_243:                                         ; preds = %else_242
  %calltmp16 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %"%1100.load" = load i64, ptr %"%p", align 8
  %calltmp17 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1100.load")
  %calltmp18 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %calltmp19 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hc_idx.load20" = load i64, ptr %hc_idx1, align 8
  %"%p.load21" = load i64, ptr %"%p", align 8
  %calltmp22 = call i64 @ehci_port_reset(i64 %"%hc_idx.load20", i64 %"%p.load21")
  store i64 %calltmp22, ptr %"%result", align 8
  %"%result.load" = load i64, ptr %"%result", align 8
  %cmpgttmp = icmp sgt i64 %"%result.load", 0
  br i1 %cmpgttmp, label %then_246, label %merge_247

then_246:                                         ; preds = %then_243
  %"%hc_idx.load28" = load i64, ptr %hc_idx1, align 8
  %"%p.load29" = load i64, ptr %"%p", align 8
  call void @usb_register_device(i64 %"%hc_idx.load28", i64 %"%p.load29", i64 2)
  store i64 0, ptr %"%dev_idx", align 8
  %"%dev_idx.load" = load i64, ptr %"%dev_idx", align 8
  %cmpgetmp30 = icmp sge i64 %"%dev_idx.load", 0
  br i1 %cmpgetmp30, label %then_248, label %merge_249

then_248:                                         ; preds = %then_246
  %"%hc_idx.load31" = load i64, ptr %hc_idx1, align 8
  %calltmp32 = call i64 @ehci_get_data_buf(i64 %"%hc_idx.load31")
  store i64 %calltmp32, ptr %"%data_buf", align 8
  %"%data_buf.load" = load i64, ptr %"%data_buf", align 8
  call void @sad_ll_memset(i64 %"%data_buf.load", i64 0, i64 64)
  %"%hc_idx.load33" = load i64, ptr %hc_idx1, align 8
  %"%data_buf.load34" = load i64, ptr %"%data_buf", align 8
  %calltmp35 = call i64 @ehci_get_device_descriptor(i64 %"%hc_idx.load33", i64 0, i64 %"%data_buf.load34", i64 8)
  store i64 %calltmp35, ptr %"%res", align 8
  %"%res.load" = load i64, ptr %"%res", align 8
  %cmpgetmp36 = icmp sge i64 %"%res.load", 0
  br i1 %cmpgetmp36, label %then_250, label %merge_251

then_250:                                         ; preds = %then_248
  %"%dev_idx.load37" = load i64, ptr %"%dev_idx", align 8
  %addtmp38 = add i64 %"%dev_idx.load37", 1
  store i64 %addtmp38, ptr %"%new_addr", align 8
  %"%hc_idx.load39" = load i64, ptr %hc_idx1, align 8
  %"%new_addr.load" = load i64, ptr %"%new_addr", align 8
  %calltmp40 = call i64 @ehci_set_address(i64 %"%hc_idx.load39", i64 0, i64 %"%new_addr.load")
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%wait", align 8
  br label %while_cond_252

then_255:                                         ; preds = %while_exit_254
  %"%data_buf.load49" = load i64, ptr %"%data_buf", align 8
  %"%dev_idx.load50" = load i64, ptr %"%dev_idx", align 8
  call void @usb_parse_device_descriptor(i64 %"%data_buf.load49", i64 %"%dev_idx.load50")
  br label %merge_256

while_body_238:                                   ; preds = %while_cond_237
  %"%p.load7" = load i64, ptr %"%p", align 8
  %cmpgetmp = icmp sge i64 %"%p.load7", 8
  br i1 %cmpgetmp, label %then_240, label %else_242

while_body_253:                                   ; preds = %while_cond_252
  store i64 0, ptr %"%d", align 8
  br label %while_cond_252

while_cond_237:                                   ; preds = %merge_241, %entry
  %"%p.load" = load i64, ptr %"%p", align 8
  %"%num_ports.load" = load i64, ptr %"%num_ports", align 8
  %cmplttmp = icmp slt i64 %"%p.load", %"%num_ports.load"
  br i1 %cmplttmp, label %while_body_238, label %while_exit_239

while_cond_252:                                   ; preds = %while_body_253, %then_250
  call void @sad_ll_timer_get_ticks()
  %"%wait.load" = load i64, ptr %"%wait", align 8
  %subtmp = sub i64 0, %"%wait.load"
  %cmplttmp41 = icmp slt i64 %subtmp, 200
  br i1 %cmplttmp41, label %while_body_253, label %while_exit_254

while_exit_239:                                   ; preds = %while_cond_237
  %calltmp8 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%1169.load" = load i64, ptr %"%devices_found", align 8
  %calltmp9 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1169.load")
  %calltmp10 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  %calltmp11 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr %"%devices_found", align 8
  ret i64 %loadtmp

while_exit_254:                                   ; preds = %while_cond_252
  %"%data_buf.load42" = load i64, ptr %"%data_buf", align 8
  call void @sad_ll_memset(i64 %"%data_buf.load42", i64 0, i64 64)
  %"%hc_idx.load43" = load i64, ptr %hc_idx1, align 8
  %"%new_addr.load44" = load i64, ptr %"%new_addr", align 8
  %"%data_buf.load45" = load i64, ptr %"%data_buf", align 8
  %calltmp46 = call i64 @ehci_get_device_descriptor(i64 %"%hc_idx.load43", i64 %"%new_addr.load44", i64 %"%data_buf.load45", i64 18)
  store i64 %calltmp46, ptr %"%res", align 8
  %"%res.load47" = load i64, ptr %"%res", align 8
  %cmpgetmp48 = icmp sge i64 %"%res.load47", 0
  br i1 %cmpgetmp48, label %then_255, label %merge_256
}

define i64 @"ehci_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %calltmp = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  %calltmp1 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  %"%1183.load" = load i64, ptr @ehci_count, align 8
  %calltmp3 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1183.load")
  %calltmp4 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp5 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  %"%1189.load" = load i64, ptr @ehci_total_transfers, align 8
  %calltmp6 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1189.load")
  %calltmp7 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp8 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %"%1195.load" = load i64, ptr @ehci_transfer_errors, align 8
  %calltmp9 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1195.load")
  %calltmp10 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp11 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  %"%1201.load" = load i64, ptr @ehci_port_resets, align 8
  %calltmp12 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1201.load")
  %calltmp13 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp14 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  %"%1207.load" = load i64, ptr @ehci_companion_routes, align 8
  %calltmp15 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1207.load")
  %calltmp16 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp17 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  %calltmp18 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @"ehci_\D9\87\D9\8A\D8\A6"(i64 %hc_idx, i64 %mmio_base) {
entry:
  %"%d2" = alloca i64, align 8
  %"%wait2" = alloca i64, align 8
  %"%ehci_status_3" = alloca i64, align 8
  %"%ehci_status_2" = alloca i64, align 8
  %"%ehci_status_1" = alloca i64, align 8
  %"%ehci_status_0" = alloca i64, align 8
  %"%p" = alloca i64, align 8
  %"%ehci_count" = alloca i64, align 8
  %"%halted" = alloca i64, align 8
  %"%sts" = alloca i64, align 8
  %"%d" = alloca i64, align 8
  %"%wait" = alloca i64, align 8
  %"%cmd" = alloca i64, align 8
  %"%ehci_async_qh_3" = alloca i64, align 8
  %"%ehci_async_qh_2" = alloca i64, align 8
  %"%ehci_async_qh_1" = alloca i64, align 8
  %"%ehci_async_qh_0" = alloca i64, align 8
  %"%aqh" = alloca i64, align 8
  %"%ehci_setup_buf_3" = alloca i64, align 8
  %"%ehci_setup_buf_2" = alloca i64, align 8
  %"%ehci_setup_buf_1" = alloca i64, align 8
  %"%ehci_setup_buf_0" = alloca i64, align 8
  %"%sbuf" = alloca i64, align 8
  %"%ehci_data_buf_3" = alloca i64, align 8
  %"%ehci_data_buf_2" = alloca i64, align 8
  %"%ehci_data_buf_1" = alloca i64, align 8
  %"%ehci_data_buf_0" = alloca i64, align 8
  %"%dbuf" = alloca i64, align 8
  %"%ehci_qtd_pool_3" = alloca i64, align 8
  %"%ehci_qtd_pool_2" = alloca i64, align 8
  %"%ehci_qtd_pool_1" = alloca i64, align 8
  %"%ehci_qtd_pool_0" = alloca i64, align 8
  %"%qtdp" = alloca i64, align 8
  %"%ehci_qh_pool_3" = alloca i64, align 8
  %"%ehci_qh_pool_2" = alloca i64, align 8
  %"%ehci_qh_pool_1" = alloca i64, align 8
  %"%ehci_qh_pool_0" = alloca i64, align 8
  %"%qhp" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %"%ehci_periodic_3" = alloca i64, align 8
  %"%ehci_periodic_2" = alloca i64, align 8
  %"%ehci_periodic_1" = alloca i64, align 8
  %"%ehci_periodic_0" = alloca i64, align 8
  %"%periodic" = alloca i64, align 8
  %"%eecp" = alloca i64, align 8
  %"%hccparams" = alloca i64, align 8
  %"%ehci_num_ports_3" = alloca i64, align 8
  %"%ehci_num_ports_2" = alloca i64, align 8
  %"%ehci_num_ports_1" = alloca i64, align 8
  %"%ehci_num_ports_0" = alloca i64, align 8
  %"%num_ports" = alloca i64, align 8
  %"%hcsparams" = alloca i64, align 8
  %"%ehci_opbase_3" = alloca i64, align 8
  %"%ehci_opbase_2" = alloca i64, align 8
  %"%ehci_opbase_1" = alloca i64, align 8
  %"%ehci_opbase_0" = alloca i64, align 8
  %"%opbase" = alloca i64, align 8
  %"%caplength" = alloca i64, align 8
  %"%ehci_mmio_3" = alloca i64, align 8
  %"%ehci_mmio_2" = alloca i64, align 8
  %"%ehci_mmio_1" = alloca i64, align 8
  %"%ehci_mmio_0" = alloca i64, align 8
  %mmio_base2 = alloca i64, align 8
  store i64 %mmio_base, ptr %mmio_base2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %calltmp = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  %"%1217.load" = load i64, ptr %hc_idx1, align 8
  %calltmp3 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1217.load")
  %calltmp4 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  %"%mmio_base.load" = load i64, ptr %mmio_base2, align 8
  %shrtmp = lshr i64 %"%mmio_base.load", 16
  %calltmp5 = call i64 @ehci_serial_hex16(i64 %shrtmp)
  %"%mmio_base.load6" = load i64, ptr %mmio_base2, align 8
  %andtmp = and i64 %"%mmio_base.load6", 65535
  %calltmp7 = call i64 @ehci_serial_hex16(i64 %andtmp)
  %calltmp8 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%ehci_count.load" = load i64, ptr @ehci_count, align 8
  %cmpgetmp = icmp sge i64 %"%ehci_count.load", 4
  br i1 %cmpgetmp, label %then_257, label %merge_258

else_339:                                         ; preds = %while_exit_336
  %calltmp135 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.35 to i64))
  %calltmp136 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hc_idx.load137" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp138 = icmp eq i64 %"%hc_idx.load137", 0
  br i1 %cmpeqtmp138, label %then_348, label %merge_349

merge_258:                                        ; preds = %entry
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%hc_idx.load", 0
  br i1 %cmpeqtmp, label %then_259, label %merge_260

merge_260:                                        ; preds = %then_259, %merge_258
  %"%hc_idx.load11" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%hc_idx.load11", 1
  br i1 %cmpeqtmp12, label %then_261, label %merge_262

merge_262:                                        ; preds = %then_261, %merge_260
  %"%hc_idx.load13" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%hc_idx.load13", 2
  br i1 %cmpeqtmp14, label %then_263, label %merge_264

merge_264:                                        ; preds = %then_263, %merge_262
  %"%hc_idx.load15" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%hc_idx.load15", 3
  br i1 %cmpeqtmp16, label %then_265, label %merge_266

merge_266:                                        ; preds = %then_265, %merge_264
  %"%mmio_base.load17" = load i64, ptr %mmio_base2, align 8
  call void @sad_ll_mem_read8(i64 %"%mmio_base.load17")
  store i64 0, ptr %"%caplength", align 8
  %"%mmio_base.load18" = load i64, ptr %mmio_base2, align 8
  %"%caplength.load" = load i64, ptr %"%caplength", align 8
  %addtmp = add i64 %"%mmio_base.load18", %"%caplength.load"
  store i64 %addtmp, ptr %"%opbase", align 8
  %"%hc_idx.load19" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%hc_idx.load19", 0
  br i1 %cmpeqtmp20, label %then_267, label %merge_268

merge_268:                                        ; preds = %then_267, %merge_266
  %"%hc_idx.load21" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%hc_idx.load21", 1
  br i1 %cmpeqtmp22, label %then_269, label %merge_270

merge_270:                                        ; preds = %then_269, %merge_268
  %"%hc_idx.load23" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%hc_idx.load23", 2
  br i1 %cmpeqtmp24, label %then_271, label %merge_272

merge_272:                                        ; preds = %then_271, %merge_270
  %"%hc_idx.load25" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%hc_idx.load25", 3
  br i1 %cmpeqtmp26, label %then_273, label %merge_274

merge_274:                                        ; preds = %then_273, %merge_272
  %calltmp27 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  %"%1273.load" = load i64, ptr %"%caplength", align 8
  %calltmp28 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1273.load")
  %calltmp29 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hc_idx.load30" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_CAP_HCSPARAMS.load" = load i64, ptr @EHCI_CAP_HCSPARAMS, align 8
  %calltmp31 = call i64 @ehci_read_cap(i64 %"%hc_idx.load30", i64 %"%EHCI_CAP_HCSPARAMS.load")
  store i64 %calltmp31, ptr %"%hcsparams", align 8
  %"%hcsparams.load" = load i64, ptr %"%hcsparams", align 8
  %andtmp32 = and i64 %"%hcsparams.load", 15
  store i64 %andtmp32, ptr %"%num_ports", align 8
  %"%hc_idx.load33" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp34 = icmp eq i64 %"%hc_idx.load33", 0
  br i1 %cmpeqtmp34, label %then_275, label %merge_276

merge_276:                                        ; preds = %then_275, %merge_274
  %"%hc_idx.load35" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%hc_idx.load35", 1
  br i1 %cmpeqtmp36, label %then_277, label %merge_278

merge_278:                                        ; preds = %then_277, %merge_276
  %"%hc_idx.load37" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp38 = icmp eq i64 %"%hc_idx.load37", 2
  br i1 %cmpeqtmp38, label %then_279, label %merge_280

merge_280:                                        ; preds = %then_279, %merge_278
  %"%hc_idx.load39" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp40 = icmp eq i64 %"%hc_idx.load39", 3
  br i1 %cmpeqtmp40, label %then_281, label %merge_282

merge_282:                                        ; preds = %then_281, %merge_280
  %calltmp41 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.32 to i64))
  %"%1300.load" = load i64, ptr %"%num_ports", align 8
  %calltmp42 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1300.load")
  %calltmp43 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hc_idx.load44" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_CAP_HCCPARAMS.load" = load i64, ptr @EHCI_CAP_HCCPARAMS, align 8
  %calltmp45 = call i64 @ehci_read_cap(i64 %"%hc_idx.load44", i64 %"%EHCI_CAP_HCCPARAMS.load")
  store i64 %calltmp45, ptr %"%hccparams", align 8
  %"%hccparams.load" = load i64, ptr %"%hccparams", align 8
  %shrtmp46 = lshr i64 %"%hccparams.load", 8
  %andtmp47 = and i64 %shrtmp46, 255
  store i64 %andtmp47, ptr %"%eecp", align 8
  %calltmp48 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  %"%1313.load" = load i64, ptr %"%eecp", align 8
  %calltmp49 = call i64 @ehci_serial_hex8(i64 %"%1313.load")
  %calltmp50 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hc_idx.load51" = load i64, ptr %hc_idx1, align 8
  %calltmp52 = call i64 @ehci_reset(i64 %"%hc_idx.load51")
  call void @sad_ll_kmalloc(i64 4096)
  store i64 0, ptr %"%periodic", align 8
  %"%hc_idx.load53" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%hc_idx.load53", 0
  br i1 %cmpeqtmp54, label %then_283, label %merge_284

merge_284:                                        ; preds = %then_283, %merge_282
  %"%hc_idx.load55" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp56 = icmp eq i64 %"%hc_idx.load55", 1
  br i1 %cmpeqtmp56, label %then_285, label %merge_286

merge_286:                                        ; preds = %then_285, %merge_284
  %"%hc_idx.load57" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp58 = icmp eq i64 %"%hc_idx.load57", 2
  br i1 %cmpeqtmp58, label %then_287, label %merge_288

merge_288:                                        ; preds = %then_287, %merge_286
  %"%hc_idx.load59" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%hc_idx.load59", 3
  br i1 %cmpeqtmp60, label %then_289, label %merge_290

merge_290:                                        ; preds = %then_289, %merge_288
  store i64 0, ptr %"%i", align 8
  br label %while_cond_291

merge_295:                                        ; preds = %then_294, %while_exit_293
  %"%hc_idx.load67" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp68 = icmp eq i64 %"%hc_idx.load67", 1
  br i1 %cmpeqtmp68, label %then_296, label %merge_297

merge_297:                                        ; preds = %then_296, %merge_295
  %"%hc_idx.load69" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp70 = icmp eq i64 %"%hc_idx.load69", 2
  br i1 %cmpeqtmp70, label %then_298, label %merge_299

merge_299:                                        ; preds = %then_298, %merge_297
  %"%hc_idx.load71" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp72 = icmp eq i64 %"%hc_idx.load71", 3
  br i1 %cmpeqtmp72, label %then_300, label %merge_301

merge_301:                                        ; preds = %then_300, %merge_299
  %"%qhp.load" = load i64, ptr %"%qhp", align 8
  call void @sad_ll_memset(i64 %"%qhp.load", i64 0, i64 1024)
  call void @sad_ll_kmalloc(i64 1024)
  store i64 0, ptr %"%qtdp", align 8
  %"%hc_idx.load73" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp74 = icmp eq i64 %"%hc_idx.load73", 0
  br i1 %cmpeqtmp74, label %then_302, label %merge_303

merge_303:                                        ; preds = %then_302, %merge_301
  %"%hc_idx.load75" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp76 = icmp eq i64 %"%hc_idx.load75", 1
  br i1 %cmpeqtmp76, label %then_304, label %merge_305

merge_305:                                        ; preds = %then_304, %merge_303
  %"%hc_idx.load77" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp78 = icmp eq i64 %"%hc_idx.load77", 2
  br i1 %cmpeqtmp78, label %then_306, label %merge_307

merge_307:                                        ; preds = %then_306, %merge_305
  %"%hc_idx.load79" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp80 = icmp eq i64 %"%hc_idx.load79", 3
  br i1 %cmpeqtmp80, label %then_308, label %merge_309

merge_309:                                        ; preds = %then_308, %merge_307
  %"%qtdp.load" = load i64, ptr %"%qtdp", align 8
  call void @sad_ll_memset(i64 %"%qtdp.load", i64 0, i64 1024)
  call void @sad_ll_kmalloc(i64 1024)
  store i64 0, ptr %"%dbuf", align 8
  %"%hc_idx.load81" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp82 = icmp eq i64 %"%hc_idx.load81", 0
  br i1 %cmpeqtmp82, label %then_310, label %merge_311

merge_311:                                        ; preds = %then_310, %merge_309
  %"%hc_idx.load83" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp84 = icmp eq i64 %"%hc_idx.load83", 1
  br i1 %cmpeqtmp84, label %then_312, label %merge_313

merge_313:                                        ; preds = %then_312, %merge_311
  %"%hc_idx.load85" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp86 = icmp eq i64 %"%hc_idx.load85", 2
  br i1 %cmpeqtmp86, label %then_314, label %merge_315

merge_315:                                        ; preds = %then_314, %merge_313
  %"%hc_idx.load87" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp88 = icmp eq i64 %"%hc_idx.load87", 3
  br i1 %cmpeqtmp88, label %then_316, label %merge_317

merge_317:                                        ; preds = %then_316, %merge_315
  call void @sad_ll_kmalloc(i64 32)
  store i64 0, ptr %"%sbuf", align 8
  %"%hc_idx.load89" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp90 = icmp eq i64 %"%hc_idx.load89", 0
  br i1 %cmpeqtmp90, label %then_318, label %merge_319

merge_319:                                        ; preds = %then_318, %merge_317
  %"%hc_idx.load91" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp92 = icmp eq i64 %"%hc_idx.load91", 1
  br i1 %cmpeqtmp92, label %then_320, label %merge_321

merge_321:                                        ; preds = %then_320, %merge_319
  %"%hc_idx.load93" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp94 = icmp eq i64 %"%hc_idx.load93", 2
  br i1 %cmpeqtmp94, label %then_322, label %merge_323

merge_323:                                        ; preds = %then_322, %merge_321
  %"%hc_idx.load95" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp96 = icmp eq i64 %"%hc_idx.load95", 3
  br i1 %cmpeqtmp96, label %then_324, label %merge_325

merge_325:                                        ; preds = %then_324, %merge_323
  %"%hc_idx.load97" = load i64, ptr %hc_idx1, align 8
  %calltmp98 = call i64 @ehci_alloc_qh(i64 %"%hc_idx.load97")
  store i64 %calltmp98, ptr %"%aqh", align 8
  %"%hc_idx.load99" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp100 = icmp eq i64 %"%hc_idx.load99", 0
  br i1 %cmpeqtmp100, label %then_326, label %merge_327

merge_327:                                        ; preds = %then_326, %merge_325
  %"%hc_idx.load101" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp102 = icmp eq i64 %"%hc_idx.load101", 1
  br i1 %cmpeqtmp102, label %then_328, label %merge_329

merge_329:                                        ; preds = %then_328, %merge_327
  %"%hc_idx.load103" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp104 = icmp eq i64 %"%hc_idx.load103", 2
  br i1 %cmpeqtmp104, label %then_330, label %merge_331

merge_331:                                        ; preds = %then_330, %merge_329
  %"%hc_idx.load105" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp106 = icmp eq i64 %"%hc_idx.load105", 3
  br i1 %cmpeqtmp106, label %then_332, label %merge_333

merge_333:                                        ; preds = %then_332, %merge_331
  %"%aqh.load" = load i64, ptr %"%aqh", align 8
  %addtmp107 = add i64 %"%aqh.load", 2
  %"%aqh.load108" = load i64, ptr %"%aqh", align 8
  call void @sad_ll_mem_write32(i64 %"%aqh.load108", i64 %addtmp107)
  %"%hc_idx.load109" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_USBINTR.load" = load i64, ptr @EHCI_OP_USBINTR, align 8
  %calltmp110 = call i64 @ehci_write_op(i64 %"%hc_idx.load109", i64 %"%EHCI_OP_USBINTR.load", i64 7)
  %"%hc_idx.load111" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_PERIODICLIST.load" = load i64, ptr @EHCI_OP_PERIODICLIST, align 8
  %"%periodic.load112" = load i64, ptr %"%periodic", align 8
  %calltmp113 = call i64 @ehci_write_op(i64 %"%hc_idx.load111", i64 %"%EHCI_OP_PERIODICLIST.load", i64 %"%periodic.load112")
  %"%hc_idx.load114" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_ASYNCLIST.load" = load i64, ptr @EHCI_OP_ASYNCLIST, align 8
  %"%aqh.load115" = load i64, ptr %"%aqh", align 8
  %calltmp116 = call i64 @ehci_write_op(i64 %"%hc_idx.load114", i64 %"%EHCI_OP_ASYNCLIST.load", i64 %"%aqh.load115")
  %"%hc_idx.load117" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_CTRLDSSEG.load" = load i64, ptr @EHCI_OP_CTRLDSSEG, align 8
  %calltmp118 = call i64 @ehci_write_op(i64 %"%hc_idx.load117", i64 %"%EHCI_OP_CTRLDSSEG.load", i64 0)
  %"%hc_idx.load119" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_FRINDEX.load" = load i64, ptr @EHCI_OP_FRINDEX, align 8
  %calltmp120 = call i64 @ehci_write_op(i64 %"%hc_idx.load119", i64 %"%EHCI_OP_FRINDEX.load", i64 0)
  %"%hc_idx.load121" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_CONFIGFLAG.load" = load i64, ptr @EHCI_OP_CONFIGFLAG, align 8
  %calltmp122 = call i64 @ehci_write_op(i64 %"%hc_idx.load121", i64 %"%EHCI_OP_CONFIGFLAG.load", i64 1)
  %"%EHCI_CMD_RS.load" = load i64, ptr @EHCI_CMD_RS, align 8
  %"%EHCI_CMD_ITC_1.load" = load i64, ptr @EHCI_CMD_ITC_1, align 8
  %addtmp123 = add i64 %"%EHCI_CMD_RS.load", %"%EHCI_CMD_ITC_1.load"
  store i64 %addtmp123, ptr %"%cmd", align 8
  %"%hc_idx.load124" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_USBCMD.load" = load i64, ptr @EHCI_OP_USBCMD, align 8
  %"%cmd.load" = load i64, ptr %"%cmd", align 8
  %calltmp125 = call i64 @ehci_write_op(i64 %"%hc_idx.load124", i64 %"%EHCI_OP_USBCMD.load", i64 %"%cmd.load")
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%wait", align 8
  br label %while_cond_334

merge_338:                                        ; preds = %merge_355, %merge_347
  %"%ehci_count.load139" = load i64, ptr @ehci_count, align 8
  %addtmp140 = add i64 %"%ehci_count.load139", 1
  store i64 %addtmp140, ptr %"%ehci_count", align 8
  store i64 0, ptr %"%p", align 8
  br label %while_cond_356

merge_341:                                        ; preds = %then_340, %then_337
  %"%hc_idx.load141" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp142 = icmp eq i64 %"%hc_idx.load141", 1
  br i1 %cmpeqtmp142, label %then_342, label %merge_343

merge_343:                                        ; preds = %then_342, %merge_341
  %"%hc_idx.load143" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp144 = icmp eq i64 %"%hc_idx.load143", 2
  br i1 %cmpeqtmp144, label %then_344, label %merge_345

merge_345:                                        ; preds = %then_344, %merge_343
  %"%hc_idx.load145" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp146 = icmp eq i64 %"%hc_idx.load145", 3
  br i1 %cmpeqtmp146, label %then_346, label %merge_347

merge_347:                                        ; preds = %then_346, %merge_345
  br label %merge_338

merge_349:                                        ; preds = %then_348, %else_339
  %"%hc_idx.load147" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp148 = icmp eq i64 %"%hc_idx.load147", 1
  br i1 %cmpeqtmp148, label %then_350, label %merge_351

merge_351:                                        ; preds = %then_350, %merge_349
  %"%hc_idx.load149" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp150 = icmp eq i64 %"%hc_idx.load149", 2
  br i1 %cmpeqtmp150, label %then_352, label %merge_353

merge_353:                                        ; preds = %then_352, %merge_351
  %"%hc_idx.load151" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp152 = icmp eq i64 %"%hc_idx.load151", 3
  br i1 %cmpeqtmp152, label %then_354, label %merge_355

merge_355:                                        ; preds = %then_354, %merge_353
  br label %merge_338

merge_360:                                        ; preds = %then_359, %while_body_357
  %"%p.load159" = load i64, ptr %"%p", align 8
  %addtmp160 = add i64 %"%p.load159", 1
  store i64 %addtmp160, ptr %"%p", align 8
  br label %while_cond_356

then_257:                                         ; preds = %entry
  %calltmp9 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.30 to i64))
  %calltmp10 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_259:                                         ; preds = %merge_258
  %"%1237.load" = load i64, ptr %mmio_base2, align 8
  store i64 %"%1237.load", ptr %"%ehci_mmio_0", align 8
  br label %merge_260

then_261:                                         ; preds = %merge_260
  %"%1241.load" = load i64, ptr %mmio_base2, align 8
  store i64 %"%1241.load", ptr %"%ehci_mmio_1", align 8
  br label %merge_262

then_263:                                         ; preds = %merge_262
  %"%1245.load" = load i64, ptr %mmio_base2, align 8
  store i64 %"%1245.load", ptr %"%ehci_mmio_2", align 8
  br label %merge_264

then_265:                                         ; preds = %merge_264
  %"%1249.load" = load i64, ptr %mmio_base2, align 8
  store i64 %"%1249.load", ptr %"%ehci_mmio_3", align 8
  br label %merge_266

then_267:                                         ; preds = %merge_266
  %"%1257.load" = load i64, ptr %"%opbase", align 8
  store i64 %"%1257.load", ptr %"%ehci_opbase_0", align 8
  br label %merge_268

then_269:                                         ; preds = %merge_268
  %"%1261.load" = load i64, ptr %"%opbase", align 8
  store i64 %"%1261.load", ptr %"%ehci_opbase_1", align 8
  br label %merge_270

then_271:                                         ; preds = %merge_270
  %"%1265.load" = load i64, ptr %"%opbase", align 8
  store i64 %"%1265.load", ptr %"%ehci_opbase_2", align 8
  br label %merge_272

then_273:                                         ; preds = %merge_272
  %"%1269.load" = load i64, ptr %"%opbase", align 8
  store i64 %"%1269.load", ptr %"%ehci_opbase_3", align 8
  br label %merge_274

then_275:                                         ; preds = %merge_274
  %"%1284.load" = load i64, ptr %"%num_ports", align 8
  store i64 %"%1284.load", ptr %"%ehci_num_ports_0", align 8
  br label %merge_276

then_277:                                         ; preds = %merge_276
  %"%1288.load" = load i64, ptr %"%num_ports", align 8
  store i64 %"%1288.load", ptr %"%ehci_num_ports_1", align 8
  br label %merge_278

then_279:                                         ; preds = %merge_278
  %"%1292.load" = load i64, ptr %"%num_ports", align 8
  store i64 %"%1292.load", ptr %"%ehci_num_ports_2", align 8
  br label %merge_280

then_281:                                         ; preds = %merge_280
  %"%1296.load" = load i64, ptr %"%num_ports", align 8
  store i64 %"%1296.load", ptr %"%ehci_num_ports_3", align 8
  br label %merge_282

then_283:                                         ; preds = %merge_282
  %"%1324.load" = load i64, ptr %"%periodic", align 8
  store i64 %"%1324.load", ptr %"%ehci_periodic_0", align 8
  br label %merge_284

then_285:                                         ; preds = %merge_284
  %"%1328.load" = load i64, ptr %"%periodic", align 8
  store i64 %"%1328.load", ptr %"%ehci_periodic_1", align 8
  br label %merge_286

then_287:                                         ; preds = %merge_286
  %"%1332.load" = load i64, ptr %"%periodic", align 8
  store i64 %"%1332.load", ptr %"%ehci_periodic_2", align 8
  br label %merge_288

then_289:                                         ; preds = %merge_288
  %"%1336.load" = load i64, ptr %"%periodic", align 8
  store i64 %"%1336.load", ptr %"%ehci_periodic_3", align 8
  br label %merge_290

then_294:                                         ; preds = %while_exit_293
  %"%1356.load" = load i64, ptr %"%qhp", align 8
  store i64 %"%1356.load", ptr %"%ehci_qh_pool_0", align 8
  br label %merge_295

then_296:                                         ; preds = %merge_295
  %"%1360.load" = load i64, ptr %"%qhp", align 8
  store i64 %"%1360.load", ptr %"%ehci_qh_pool_1", align 8
  br label %merge_297

then_298:                                         ; preds = %merge_297
  %"%1364.load" = load i64, ptr %"%qhp", align 8
  store i64 %"%1364.load", ptr %"%ehci_qh_pool_2", align 8
  br label %merge_299

then_300:                                         ; preds = %merge_299
  %"%1368.load" = load i64, ptr %"%qhp", align 8
  store i64 %"%1368.load", ptr %"%ehci_qh_pool_3", align 8
  br label %merge_301

then_302:                                         ; preds = %merge_301
  %"%1379.load" = load i64, ptr %"%qtdp", align 8
  store i64 %"%1379.load", ptr %"%ehci_qtd_pool_0", align 8
  br label %merge_303

then_304:                                         ; preds = %merge_303
  %"%1383.load" = load i64, ptr %"%qtdp", align 8
  store i64 %"%1383.load", ptr %"%ehci_qtd_pool_1", align 8
  br label %merge_305

then_306:                                         ; preds = %merge_305
  %"%1387.load" = load i64, ptr %"%qtdp", align 8
  store i64 %"%1387.load", ptr %"%ehci_qtd_pool_2", align 8
  br label %merge_307

then_308:                                         ; preds = %merge_307
  %"%1391.load" = load i64, ptr %"%qtdp", align 8
  store i64 %"%1391.load", ptr %"%ehci_qtd_pool_3", align 8
  br label %merge_309

then_310:                                         ; preds = %merge_309
  %"%1402.load" = load i64, ptr %"%dbuf", align 8
  store i64 %"%1402.load", ptr %"%ehci_data_buf_0", align 8
  br label %merge_311

then_312:                                         ; preds = %merge_311
  %"%1406.load" = load i64, ptr %"%dbuf", align 8
  store i64 %"%1406.load", ptr %"%ehci_data_buf_1", align 8
  br label %merge_313

then_314:                                         ; preds = %merge_313
  %"%1410.load" = load i64, ptr %"%dbuf", align 8
  store i64 %"%1410.load", ptr %"%ehci_data_buf_2", align 8
  br label %merge_315

then_316:                                         ; preds = %merge_315
  %"%1414.load" = load i64, ptr %"%dbuf", align 8
  store i64 %"%1414.load", ptr %"%ehci_data_buf_3", align 8
  br label %merge_317

then_318:                                         ; preds = %merge_317
  %"%1422.load" = load i64, ptr %"%sbuf", align 8
  store i64 %"%1422.load", ptr %"%ehci_setup_buf_0", align 8
  br label %merge_319

then_320:                                         ; preds = %merge_319
  %"%1426.load" = load i64, ptr %"%sbuf", align 8
  store i64 %"%1426.load", ptr %"%ehci_setup_buf_1", align 8
  br label %merge_321

then_322:                                         ; preds = %merge_321
  %"%1430.load" = load i64, ptr %"%sbuf", align 8
  store i64 %"%1430.load", ptr %"%ehci_setup_buf_2", align 8
  br label %merge_323

then_324:                                         ; preds = %merge_323
  %"%1434.load" = load i64, ptr %"%sbuf", align 8
  store i64 %"%1434.load", ptr %"%ehci_setup_buf_3", align 8
  br label %merge_325

then_326:                                         ; preds = %merge_325
  %"%1441.load" = load i64, ptr %"%aqh", align 8
  store i64 %"%1441.load", ptr %"%ehci_async_qh_0", align 8
  br label %merge_327

then_328:                                         ; preds = %merge_327
  %"%1445.load" = load i64, ptr %"%aqh", align 8
  store i64 %"%1445.load", ptr %"%ehci_async_qh_1", align 8
  br label %merge_329

then_330:                                         ; preds = %merge_329
  %"%1449.load" = load i64, ptr %"%aqh", align 8
  store i64 %"%1449.load", ptr %"%ehci_async_qh_2", align 8
  br label %merge_331

then_332:                                         ; preds = %merge_331
  %"%1453.load" = load i64, ptr %"%aqh", align 8
  store i64 %"%1453.load", ptr %"%ehci_async_qh_3", align 8
  br label %merge_333

then_337:                                         ; preds = %while_exit_336
  %calltmp131 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.34 to i64))
  %calltmp132 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hc_idx.load133" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp134 = icmp eq i64 %"%hc_idx.load133", 0
  br i1 %cmpeqtmp134, label %then_340, label %merge_341

then_340:                                         ; preds = %then_337
  store i64 2, ptr %"%ehci_status_0", align 8
  br label %merge_341

then_342:                                         ; preds = %merge_341
  store i64 2, ptr %"%ehci_status_1", align 8
  br label %merge_343

then_344:                                         ; preds = %merge_343
  store i64 2, ptr %"%ehci_status_2", align 8
  br label %merge_345

then_346:                                         ; preds = %merge_345
  store i64 2, ptr %"%ehci_status_3", align 8
  br label %merge_347

then_348:                                         ; preds = %else_339
  store i64 1, ptr %"%ehci_status_0", align 8
  br label %merge_349

then_350:                                         ; preds = %merge_349
  store i64 1, ptr %"%ehci_status_1", align 8
  br label %merge_351

then_352:                                         ; preds = %merge_351
  store i64 1, ptr %"%ehci_status_2", align 8
  br label %merge_353

then_354:                                         ; preds = %merge_353
  store i64 1, ptr %"%ehci_status_3", align 8
  br label %merge_355

then_359:                                         ; preds = %while_body_357
  %"%hc_idx.load156" = load i64, ptr %hc_idx1, align 8
  %"%p.load157" = load i64, ptr %"%p", align 8
  %"%EHCI_PORT_PP.load" = load i64, ptr @EHCI_PORT_PP, align 8
  %calltmp158 = call i64 @ehci_write_port(i64 %"%hc_idx.load156", i64 %"%p.load157", i64 %"%EHCI_PORT_PP.load")
  br label %merge_360

while_body_292:                                   ; preds = %while_cond_291
  %"%i.load61" = load i64, ptr %"%i", align 8
  %shltmp = shl i64 %"%i.load61", 2
  %"%periodic.load" = load i64, ptr %"%periodic", align 8
  %addtmp62 = add i64 %"%periodic.load", %shltmp
  call void @sad_ll_mem_write32(i64 %addtmp62, i64 1)
  %"%i.load63" = load i64, ptr %"%i", align 8
  %addtmp64 = add i64 %"%i.load63", 1
  store i64 %addtmp64, ptr %"%i", align 8
  br label %while_cond_291

while_body_335:                                   ; preds = %while_cond_334
  store i64 0, ptr %"%d", align 8
  br label %while_cond_334

while_body_357:                                   ; preds = %while_cond_356
  %"%p.load154" = load i64, ptr %"%p", align 8
  %cmplttmp155 = icmp slt i64 %"%p.load154", 8
  br i1 %cmplttmp155, label %then_359, label %merge_360

while_body_362:                                   ; preds = %while_cond_361
  store i64 0, ptr %"%d2", align 8
  br label %while_cond_361

while_cond_291:                                   ; preds = %while_body_292, %merge_290
  %"%i.load" = load i64, ptr %"%i", align 8
  %cmplttmp = icmp slt i64 %"%i.load", 1024
  br i1 %cmplttmp, label %while_body_292, label %while_exit_293

while_cond_334:                                   ; preds = %while_body_335, %merge_333
  call void @sad_ll_timer_get_ticks()
  %"%wait.load" = load i64, ptr %"%wait", align 8
  %subtmp = sub i64 0, %"%wait.load"
  %cmplttmp126 = icmp slt i64 %subtmp, 500
  br i1 %cmplttmp126, label %while_body_335, label %while_exit_336

while_cond_356:                                   ; preds = %merge_360, %merge_338
  %"%p.load" = load i64, ptr %"%p", align 8
  %"%num_ports.load" = load i64, ptr %"%num_ports", align 8
  %cmplttmp153 = icmp slt i64 %"%p.load", %"%num_ports.load"
  br i1 %cmplttmp153, label %while_body_357, label %while_exit_358

while_cond_361:                                   ; preds = %while_body_362, %while_exit_358
  call void @sad_ll_timer_get_ticks()
  %"%wait2.load" = load i64, ptr %"%wait2", align 8
  %subtmp161 = sub i64 0, %"%wait2.load"
  %cmplttmp162 = icmp slt i64 %subtmp161, 2000
  br i1 %cmplttmp162, label %while_body_362, label %while_exit_363

while_exit_293:                                   ; preds = %while_cond_291
  call void @sad_ll_kmalloc(i64 1024)
  store i64 0, ptr %"%qhp", align 8
  %"%hc_idx.load65" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp66 = icmp eq i64 %"%hc_idx.load65", 0
  br i1 %cmpeqtmp66, label %then_294, label %merge_295

while_exit_336:                                   ; preds = %while_cond_334
  %"%hc_idx.load127" = load i64, ptr %hc_idx1, align 8
  %"%EHCI_OP_USBSTS.load" = load i64, ptr @EHCI_OP_USBSTS, align 8
  %calltmp128 = call i64 @ehci_read_op(i64 %"%hc_idx.load127", i64 %"%EHCI_OP_USBSTS.load")
  store i64 %calltmp128, ptr %"%sts", align 8
  %"%sts.load" = load i64, ptr %"%sts", align 8
  %"%EHCI_STS_HCH.load" = load i64, ptr @EHCI_STS_HCH, align 8
  %divtmp = sdiv i64 %"%sts.load", %"%EHCI_STS_HCH.load"
  %andtmp129 = and i64 %divtmp, 1
  store i64 %andtmp129, ptr %"%halted", align 8
  %"%halted.load" = load i64, ptr %"%halted", align 8
  %cmpeqtmp130 = icmp eq i64 %"%halted.load", 0
  br i1 %cmpeqtmp130, label %then_337, label %else_339

while_exit_358:                                   ; preds = %while_cond_356
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%wait2", align 8
  br label %while_cond_361

while_exit_363:                                   ; preds = %while_cond_361
  %"%hc_idx.load163" = load i64, ptr %hc_idx1, align 8
  %calltmp164 = call i64 @ehci_scan_ports(i64 %"%hc_idx.load163")
  %calltmp165 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.36 to i64))
  %calltmp166 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1
}

define i64 @"ehci_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  %"%sts" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_364

merge_368:                                        ; preds = %then_367, %while_body_365
  %"%sts.load9" = load i64, ptr %"%sts", align 8
  %"%EHCI_STS_HSE.load" = load i64, ptr @EHCI_STS_HSE, align 8
  %divtmp10 = sdiv i64 %"%sts.load9", %"%EHCI_STS_HSE.load"
  %andtmp11 = and i64 %divtmp10, 1
  %cmpeqtmp12 = icmp eq i64 %andtmp11, 1
  br i1 %cmpeqtmp12, label %then_369, label %merge_370

merge_370:                                        ; preds = %then_369, %merge_368
  %"%i.load20" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load20", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_364

then_367:                                         ; preds = %while_body_365
  %calltmp2 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.37 to i64))
  %"%1554.load" = load i64, ptr %"%i", align 8
  %calltmp3 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1554.load")
  %calltmp4 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%i.load5" = load i64, ptr %"%i", align 8
  %"%EHCI_OP_USBSTS.load6" = load i64, ptr @EHCI_OP_USBSTS, align 8
  %"%EHCI_STS_PCD.load7" = load i64, ptr @EHCI_STS_PCD, align 8
  %calltmp8 = call i64 @ehci_write_op(i64 %"%i.load5", i64 %"%EHCI_OP_USBSTS.load6", i64 %"%EHCI_STS_PCD.load7")
  br label %merge_368

then_369:                                         ; preds = %merge_368
  %calltmp13 = call i64 @"ehci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.38 to i64))
  %"%1566.load" = load i64, ptr %"%i", align 8
  %calltmp14 = call i64 @"ehci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1566.load")
  %calltmp15 = call i64 @"ehci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%i.load16" = load i64, ptr %"%i", align 8
  %"%EHCI_OP_USBSTS.load17" = load i64, ptr @EHCI_OP_USBSTS, align 8
  %"%EHCI_STS_HSE.load18" = load i64, ptr @EHCI_STS_HSE, align 8
  %calltmp19 = call i64 @ehci_write_op(i64 %"%i.load16", i64 %"%EHCI_OP_USBSTS.load17", i64 %"%EHCI_STS_HSE.load18")
  br label %merge_370

while_body_365:                                   ; preds = %while_cond_364
  %"%i.load1" = load i64, ptr %"%i", align 8
  %"%EHCI_OP_USBSTS.load" = load i64, ptr @EHCI_OP_USBSTS, align 8
  %calltmp = call i64 @ehci_read_op(i64 %"%i.load1", i64 %"%EHCI_OP_USBSTS.load")
  store i64 %calltmp, ptr %"%sts", align 8
  %"%sts.load" = load i64, ptr %"%sts", align 8
  %"%EHCI_STS_PCD.load" = load i64, ptr @EHCI_STS_PCD, align 8
  %divtmp = sdiv i64 %"%sts.load", %"%EHCI_STS_PCD.load"
  %andtmp = and i64 %divtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_367, label %merge_368

while_cond_364:                                   ; preds = %merge_370, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%ehci_count.load" = load i64, ptr @ehci_count, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%ehci_count.load"
  br i1 %cmplttmp, label %while_body_365, label %while_exit_366

while_exit_366:                                   ; preds = %while_cond_364
  ret i64 0
}

define void @__sad_main() {
entry:
  %"%ehci_companion_routes" = alloca i64, align 8
  %"%ehci_port_resets" = alloca i64, align 8
  %"%ehci_transfer_errors" = alloca i64, align 8
  %"%ehci_total_transfers" = alloca i64, align 8
  %"%ehci_async_qh_3" = alloca i64, align 8
  %"%ehci_async_qh_2" = alloca i64, align 8
  %"%ehci_async_qh_1" = alloca i64, align 8
  %"%ehci_async_qh_0" = alloca i64, align 8
  %"%ehci_qtd_used_3" = alloca i64, align 8
  %"%ehci_qtd_used_2" = alloca i64, align 8
  %"%ehci_qtd_used_1" = alloca i64, align 8
  %"%ehci_qtd_used_0" = alloca i64, align 8
  %"%ehci_qh_used_3" = alloca i64, align 8
  %"%ehci_qh_used_2" = alloca i64, align 8
  %"%ehci_qh_used_1" = alloca i64, align 8
  %"%ehci_qh_used_0" = alloca i64, align 8
  %"%ehci_status_3" = alloca i64, align 8
  %"%ehci_status_2" = alloca i64, align 8
  %"%ehci_status_1" = alloca i64, align 8
  %"%ehci_status_0" = alloca i64, align 8
  %"%ehci_setup_buf_3" = alloca i64, align 8
  %"%ehci_setup_buf_2" = alloca i64, align 8
  %"%ehci_setup_buf_1" = alloca i64, align 8
  %"%ehci_setup_buf_0" = alloca i64, align 8
  %"%ehci_data_buf_3" = alloca i64, align 8
  %"%ehci_data_buf_2" = alloca i64, align 8
  %"%ehci_data_buf_1" = alloca i64, align 8
  %"%ehci_data_buf_0" = alloca i64, align 8
  %"%ehci_qtd_pool_3" = alloca i64, align 8
  %"%ehci_qtd_pool_2" = alloca i64, align 8
  %"%ehci_qtd_pool_1" = alloca i64, align 8
  %"%ehci_qtd_pool_0" = alloca i64, align 8
  %"%ehci_qh_pool_3" = alloca i64, align 8
  %"%ehci_qh_pool_2" = alloca i64, align 8
  %"%ehci_qh_pool_1" = alloca i64, align 8
  %"%ehci_qh_pool_0" = alloca i64, align 8
  %"%ehci_periodic_3" = alloca i64, align 8
  %"%ehci_periodic_2" = alloca i64, align 8
  %"%ehci_periodic_1" = alloca i64, align 8
  %"%ehci_periodic_0" = alloca i64, align 8
  %"%ehci_num_ports_3" = alloca i64, align 8
  %"%ehci_num_ports_2" = alloca i64, align 8
  %"%ehci_num_ports_1" = alloca i64, align 8
  %"%ehci_num_ports_0" = alloca i64, align 8
  %"%ehci_opbase_3" = alloca i64, align 8
  %"%ehci_opbase_2" = alloca i64, align 8
  %"%ehci_opbase_1" = alloca i64, align 8
  %"%ehci_opbase_0" = alloca i64, align 8
  %"%ehci_mmio_3" = alloca i64, align 8
  %"%ehci_mmio_2" = alloca i64, align 8
  %"%ehci_mmio_1" = alloca i64, align 8
  %"%ehci_mmio_0" = alloca i64, align 8
  %"%ehci_count" = alloca i64, align 8
  %"%EHCI_QTD_TOGGLE" = alloca i64, align 8
  %"%EHCI_QTD_PID_SETUP" = alloca i64, align 8
  %"%EHCI_QTD_PID_IN" = alloca i64, align 8
  %"%EHCI_QTD_PID_OUT" = alloca i64, align 8
  %"%EHCI_QTD_PING" = alloca i64, align 8
  %"%EHCI_QTD_SPLIT_XS" = alloca i64, align 8
  %"%EHCI_QTD_MISS_MF" = alloca i64, align 8
  %"%EHCI_QTD_XACT_ERR" = alloca i64, align 8
  %"%EHCI_QTD_BABBLE" = alloca i64, align 8
  %"%EHCI_QTD_DBUF_ERR" = alloca i64, align 8
  %"%EHCI_QTD_HALTED" = alloca i64, align 8
  %"%EHCI_QTD_ACTIVE" = alloca i64, align 8
  %"%EHCI_QH_ADDR_MASK" = alloca i64, align 8
  %"%EHCI_QH_INACT" = alloca i64, align 8
  %"%EHCI_QH_EP_SHIFT" = alloca i64, align 8
  %"%EHCI_QH_EPS_HIGH" = alloca i64, align 8
  %"%EHCI_QH_EPS_LOW" = alloca i64, align 8
  %"%EHCI_QH_EPS_FULL" = alloca i64, align 8
  %"%EHCI_QH_DTC" = alloca i64, align 8
  %"%EHCI_QH_H_BIT" = alloca i64, align 8
  %"%EHCI_QH_MAX_PKT_SHIFT" = alloca i64, align 8
  %"%EHCI_QH_C_BIT" = alloca i64, align 8
  %"%EHCI_QH_RL_SHIFT" = alloca i64, align 8
  %"%EHCI_PORT_PIC_GREEN" = alloca i64, align 8
  %"%EHCI_PORT_PIC_AMBER" = alloca i64, align 8
  %"%EHCI_PORT_PIC_OFF" = alloca i64, align 8
  %"%EHCI_PORT_OWNER" = alloca i64, align 8
  %"%EHCI_PORT_PP" = alloca i64, align 8
  %"%EHCI_PORT_LS_MASK" = alloca i64, align 8
  %"%EHCI_PORT_RESET" = alloca i64, align 8
  %"%EHCI_PORT_SUSPEND" = alloca i64, align 8
  %"%EHCI_PORT_FPR" = alloca i64, align 8
  %"%EHCI_PORT_OCC" = alloca i64, align 8
  %"%EHCI_PORT_OCA" = alloca i64, align 8
  %"%EHCI_PORT_ENC" = alloca i64, align 8
  %"%EHCI_PORT_ENABLE" = alloca i64, align 8
  %"%EHCI_PORT_CSC" = alloca i64, align 8
  %"%EHCI_PORT_CONNECT" = alloca i64, align 8
  %"%EHCI_STS_ASS" = alloca i64, align 8
  %"%EHCI_STS_PSS" = alloca i64, align 8
  %"%EHCI_STS_RECL" = alloca i64, align 8
  %"%EHCI_STS_HCH" = alloca i64, align 8
  %"%EHCI_STS_IAA" = alloca i64, align 8
  %"%EHCI_STS_HSE" = alloca i64, align 8
  %"%EHCI_STS_FLR" = alloca i64, align 8
  %"%EHCI_STS_PCD" = alloca i64, align 8
  %"%EHCI_STS_USBERR" = alloca i64, align 8
  %"%EHCI_STS_USBINT" = alloca i64, align 8
  %"%EHCI_CMD_ITC_1" = alloca i64, align 8
  %"%EHCI_CMD_IAAD" = alloca i64, align 8
  %"%EHCI_CMD_ASE" = alloca i64, align 8
  %"%EHCI_CMD_PSE" = alloca i64, align 8
  %"%EHCI_CMD_FLS_256" = alloca i64, align 8
  %"%EHCI_CMD_FLS_512" = alloca i64, align 8
  %"%EHCI_CMD_FLS_1024" = alloca i64, align 8
  %"%EHCI_CMD_HCRESET" = alloca i64, align 8
  %"%EHCI_CMD_RS" = alloca i64, align 8
  %"%EHCI_OP_PORTSC_BASE" = alloca i64, align 8
  %"%EHCI_OP_CONFIGFLAG" = alloca i64, align 8
  %"%EHCI_OP_ASYNCLIST" = alloca i64, align 8
  %"%EHCI_OP_PERIODICLIST" = alloca i64, align 8
  %"%EHCI_OP_CTRLDSSEG" = alloca i64, align 8
  %"%EHCI_OP_FRINDEX" = alloca i64, align 8
  %"%EHCI_OP_USBINTR" = alloca i64, align 8
  %"%EHCI_OP_USBSTS" = alloca i64, align 8
  %"%EHCI_OP_USBCMD" = alloca i64, align 8
  %"%EHCI_CAP_HCCPARAMS" = alloca i64, align 8
  %"%EHCI_CAP_HCSPARAMS" = alloca i64, align 8
  %"%EHCI_CAP_HCIVERSION" = alloca i64, align 8
  %"%EHCI_CAP_CAPLENGTH" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 0, ptr %"%EHCI_CAP_CAPLENGTH", align 8
  store i64 2, ptr %"%EHCI_CAP_HCIVERSION", align 8
  store i64 4, ptr %"%EHCI_CAP_HCSPARAMS", align 8
  store i64 8, ptr %"%EHCI_CAP_HCCPARAMS", align 8
  store i64 0, ptr %"%EHCI_OP_USBCMD", align 8
  store i64 4, ptr %"%EHCI_OP_USBSTS", align 8
  store i64 8, ptr %"%EHCI_OP_USBINTR", align 8
  store i64 12, ptr %"%EHCI_OP_FRINDEX", align 8
  store i64 16, ptr %"%EHCI_OP_CTRLDSSEG", align 8
  store i64 20, ptr %"%EHCI_OP_PERIODICLIST", align 8
  store i64 24, ptr %"%EHCI_OP_ASYNCLIST", align 8
  store i64 64, ptr %"%EHCI_OP_CONFIGFLAG", align 8
  store i64 68, ptr %"%EHCI_OP_PORTSC_BASE", align 8
  store i64 1, ptr %"%EHCI_CMD_RS", align 8
  store i64 2, ptr %"%EHCI_CMD_HCRESET", align 8
  store i64 0, ptr %"%EHCI_CMD_FLS_1024", align 8
  store i64 4, ptr %"%EHCI_CMD_FLS_512", align 8
  store i64 8, ptr %"%EHCI_CMD_FLS_256", align 8
  store i64 16, ptr %"%EHCI_CMD_PSE", align 8
  store i64 32, ptr %"%EHCI_CMD_ASE", align 8
  store i64 64, ptr %"%EHCI_CMD_IAAD", align 8
  store i64 65536, ptr %"%EHCI_CMD_ITC_1", align 8
  store i64 1, ptr %"%EHCI_STS_USBINT", align 8
  store i64 2, ptr %"%EHCI_STS_USBERR", align 8
  store i64 4, ptr %"%EHCI_STS_PCD", align 8
  store i64 8, ptr %"%EHCI_STS_FLR", align 8
  store i64 16, ptr %"%EHCI_STS_HSE", align 8
  store i64 32, ptr %"%EHCI_STS_IAA", align 8
  store i64 4096, ptr %"%EHCI_STS_HCH", align 8
  store i64 8192, ptr %"%EHCI_STS_RECL", align 8
  store i64 16384, ptr %"%EHCI_STS_PSS", align 8
  store i64 32768, ptr %"%EHCI_STS_ASS", align 8
  store i64 1, ptr %"%EHCI_PORT_CONNECT", align 8
  store i64 2, ptr %"%EHCI_PORT_CSC", align 8
  store i64 4, ptr %"%EHCI_PORT_ENABLE", align 8
  store i64 8, ptr %"%EHCI_PORT_ENC", align 8
  store i64 16, ptr %"%EHCI_PORT_OCA", align 8
  store i64 32, ptr %"%EHCI_PORT_OCC", align 8
  store i64 64, ptr %"%EHCI_PORT_FPR", align 8
  store i64 128, ptr %"%EHCI_PORT_SUSPEND", align 8
  store i64 256, ptr %"%EHCI_PORT_RESET", align 8
  store i64 3072, ptr %"%EHCI_PORT_LS_MASK", align 8
  store i64 4096, ptr %"%EHCI_PORT_PP", align 8
  store i64 8192, ptr %"%EHCI_PORT_OWNER", align 8
  store i64 0, ptr %"%EHCI_PORT_PIC_OFF", align 8
  store i64 16384, ptr %"%EHCI_PORT_PIC_AMBER", align 8
  store i64 32768, ptr %"%EHCI_PORT_PIC_GREEN", align 8
  store i64 28, ptr %"%EHCI_QH_RL_SHIFT", align 8
  store i64 134217728, ptr %"%EHCI_QH_C_BIT", align 8
  store i64 16, ptr %"%EHCI_QH_MAX_PKT_SHIFT", align 8
  store i64 32768, ptr %"%EHCI_QH_H_BIT", align 8
  store i64 16384, ptr %"%EHCI_QH_DTC", align 8
  store i64 0, ptr %"%EHCI_QH_EPS_FULL", align 8
  store i64 4096, ptr %"%EHCI_QH_EPS_LOW", align 8
  store i64 8192, ptr %"%EHCI_QH_EPS_HIGH", align 8
  store i64 8, ptr %"%EHCI_QH_EP_SHIFT", align 8
  store i64 128, ptr %"%EHCI_QH_INACT", align 8
  store i64 127, ptr %"%EHCI_QH_ADDR_MASK", align 8
  store i64 128, ptr %"%EHCI_QTD_ACTIVE", align 8
  store i64 64, ptr %"%EHCI_QTD_HALTED", align 8
  store i64 32, ptr %"%EHCI_QTD_DBUF_ERR", align 8
  store i64 16, ptr %"%EHCI_QTD_BABBLE", align 8
  store i64 8, ptr %"%EHCI_QTD_XACT_ERR", align 8
  store i64 4, ptr %"%EHCI_QTD_MISS_MF", align 8
  store i64 2, ptr %"%EHCI_QTD_SPLIT_XS", align 8
  store i64 1, ptr %"%EHCI_QTD_PING", align 8
  store i64 0, ptr %"%EHCI_QTD_PID_OUT", align 8
  store i64 256, ptr %"%EHCI_QTD_PID_IN", align 8
  store i64 512, ptr %"%EHCI_QTD_PID_SETUP", align 8
  store i64 2147483648, ptr %"%EHCI_QTD_TOGGLE", align 8
  store i64 0, ptr %"%ehci_count", align 8
  store i64 0, ptr %"%ehci_mmio_0", align 8
  store i64 0, ptr %"%ehci_mmio_1", align 8
  store i64 0, ptr %"%ehci_mmio_2", align 8
  store i64 0, ptr %"%ehci_mmio_3", align 8
  store i64 0, ptr %"%ehci_opbase_0", align 8
  store i64 0, ptr %"%ehci_opbase_1", align 8
  store i64 0, ptr %"%ehci_opbase_2", align 8
  store i64 0, ptr %"%ehci_opbase_3", align 8
  store i64 0, ptr %"%ehci_num_ports_0", align 8
  store i64 0, ptr %"%ehci_num_ports_1", align 8
  store i64 0, ptr %"%ehci_num_ports_2", align 8
  store i64 0, ptr %"%ehci_num_ports_3", align 8
  store i64 0, ptr %"%ehci_periodic_0", align 8
  store i64 0, ptr %"%ehci_periodic_1", align 8
  store i64 0, ptr %"%ehci_periodic_2", align 8
  store i64 0, ptr %"%ehci_periodic_3", align 8
  store i64 0, ptr %"%ehci_qh_pool_0", align 8
  store i64 0, ptr %"%ehci_qh_pool_1", align 8
  store i64 0, ptr %"%ehci_qh_pool_2", align 8
  store i64 0, ptr %"%ehci_qh_pool_3", align 8
  store i64 0, ptr %"%ehci_qtd_pool_0", align 8
  store i64 0, ptr %"%ehci_qtd_pool_1", align 8
  store i64 0, ptr %"%ehci_qtd_pool_2", align 8
  store i64 0, ptr %"%ehci_qtd_pool_3", align 8
  store i64 0, ptr %"%ehci_data_buf_0", align 8
  store i64 0, ptr %"%ehci_data_buf_1", align 8
  store i64 0, ptr %"%ehci_data_buf_2", align 8
  store i64 0, ptr %"%ehci_data_buf_3", align 8
  store i64 0, ptr %"%ehci_setup_buf_0", align 8
  store i64 0, ptr %"%ehci_setup_buf_1", align 8
  store i64 0, ptr %"%ehci_setup_buf_2", align 8
  store i64 0, ptr %"%ehci_setup_buf_3", align 8
  store i64 0, ptr %"%ehci_status_0", align 8
  store i64 0, ptr %"%ehci_status_1", align 8
  store i64 0, ptr %"%ehci_status_2", align 8
  store i64 0, ptr %"%ehci_status_3", align 8
  store i64 0, ptr %"%ehci_qh_used_0", align 8
  store i64 0, ptr %"%ehci_qh_used_1", align 8
  store i64 0, ptr %"%ehci_qh_used_2", align 8
  store i64 0, ptr %"%ehci_qh_used_3", align 8
  store i64 0, ptr %"%ehci_qtd_used_0", align 8
  store i64 0, ptr %"%ehci_qtd_used_1", align 8
  store i64 0, ptr %"%ehci_qtd_used_2", align 8
  store i64 0, ptr %"%ehci_qtd_used_3", align 8
  store i64 0, ptr %"%ehci_async_qh_0", align 8
  store i64 0, ptr %"%ehci_async_qh_1", align 8
  store i64 0, ptr %"%ehci_async_qh_2", align 8
  store i64 0, ptr %"%ehci_async_qh_3", align 8
  store i64 0, ptr %"%ehci_total_transfers", align 8
  store i64 0, ptr %"%ehci_transfer_errors", align 8
  store i64 0, ptr %"%ehci_port_resets", align 8
  store i64 0, ptr %"%ehci_companion_routes", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
