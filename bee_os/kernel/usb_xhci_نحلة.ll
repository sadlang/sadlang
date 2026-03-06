; ModuleID = 'usb_xhci_نحلة'
source_filename = "usb_xhci_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@COM1 = constant i64 1016
@XHCI_MAX_HC = constant i64 4
@XHCI_MAX_PORTS = constant i64 16
@XHCI_MAX_SLOTS = constant i64 32
@XHCI_MAX_TRB_RING = constant i64 64
@XHCI_CAP_CAPLENGTH = constant i64 0
@XHCI_CAP_HCIVERSION = constant i64 2
@XHCI_CAP_HCSPARAMS1 = constant i64 4
@XHCI_CAP_HCSPARAMS2 = constant i64 8
@XHCI_CAP_HCSPARAMS3 = constant i64 12
@XHCI_CAP_HCCPARAMS1 = constant i64 16
@XHCI_CAP_DBOFF = constant i64 20
@XHCI_CAP_RTSOFF = constant i64 24
@XHCI_CAP_HCCPARAMS2 = constant i64 28
@XHCI_OP_USBCMD = constant i64 0
@XHCI_OP_USBSTS = constant i64 4
@XHCI_OP_PAGESIZE = constant i64 8
@XHCI_OP_DNCTRL = constant i64 20
@XHCI_OP_CRCR = constant i64 24
@XHCI_OP_DCBAAP = constant i64 48
@XHCI_OP_CONFIG = constant i64 56
@XHCI_PORTSC_OFFSET = constant i64 1024
@XHCI_PORTSC_SIZE = constant i64 16
@XHCI_CMD_RUN = constant i64 1
@XHCI_CMD_HCRST = constant i64 2
@XHCI_CMD_INTE = constant i64 4
@XHCI_CMD_HSEE = constant i64 8
@XHCI_STS_HCH = constant i64 1
@XHCI_STS_HSE = constant i64 4
@XHCI_STS_EINT = constant i64 8
@XHCI_STS_PCD = constant i64 16
@XHCI_STS_CNR = constant i64 2048
@PORTSC_CCS = constant i64 1
@PORTSC_PED = constant i64 2
@PORTSC_OCA = constant i64 8
@PORTSC_PR = constant i64 16
@PORTSC_PP = constant i64 512
@PORTSC_SPEED_MASK = constant i64 15360
@PORTSC_CSC = constant i64 131072
@PORTSC_PRC = constant i64 2097152
@PORTSC_WRC = constant i64 524288
@XHCI_SPEED_FULL = constant i64 1
@XHCI_SPEED_LOW = constant i64 2
@XHCI_SPEED_HIGH = constant i64 3
@XHCI_SPEED_SUPER = constant i64 4
@TRB_NORMAL = constant i64 1
@TRB_SETUP = constant i64 2
@TRB_DATA = constant i64 3
@TRB_STATUS = constant i64 4
@TRB_LINK = constant i64 6
@TRB_ENABLE_SLOT = constant i64 9
@TRB_DISABLE_SLOT = constant i64 10
@TRB_ADDRESS_DEVICE = constant i64 11
@TRB_CONFIG_EP = constant i64 12
@TRB_EVAL_CONTEXT = constant i64 13
@TRB_RESET_EP = constant i64 14
@TRB_NOOP = constant i64 23
@TRB_TRANSFER_EVENT = constant i64 32
@TRB_COMMAND_COMPLETE = constant i64 33
@TRB_PORT_STATUS_CHANGE = constant i64 34
@TRB_HOST_CONTROLLER = constant i64 37
@TRB_CC_SUCCESS = constant i64 1
@TRB_CC_SHORT_PACKET = constant i64 13
@TRB_CC_STALL = constant i64 6
@TRB_CC_USB_ERROR = constant i64 5
@xhci_base_0 = global i64 0
@xhci_base_1 = global i64 0
@xhci_base_2 = global i64 0
@xhci_base_3 = global i64 0
@xhci_cap_len_0 = global i64 0
@xhci_cap_len_1 = global i64 0
@xhci_cap_len_2 = global i64 0
@xhci_cap_len_3 = global i64 0
@xhci_op_base_0 = global i64 0
@xhci_op_base_1 = global i64 0
@xhci_op_base_2 = global i64 0
@xhci_op_base_3 = global i64 0
@xhci_db_off_0 = global i64 0
@xhci_db_off_1 = global i64 0
@xhci_db_off_2 = global i64 0
@xhci_db_off_3 = global i64 0
@xhci_rt_off_0 = global i64 0
@xhci_rt_off_1 = global i64 0
@xhci_rt_off_2 = global i64 0
@xhci_rt_off_3 = global i64 0
@xhci_num_ports_0 = global i64 0
@xhci_num_ports_1 = global i64 0
@xhci_num_ports_2 = global i64 0
@xhci_num_ports_3 = global i64 0
@xhci_max_slots_0 = global i64 0
@xhci_max_slots_1 = global i64 0
@xhci_max_slots_2 = global i64 0
@xhci_max_slots_3 = global i64 0
@xhci_pci_idx_0 = global i64 0
@xhci_pci_idx_1 = global i64 0
@xhci_pci_idx_2 = global i64 0
@xhci_pci_idx_3 = global i64 0
@xhci_irq_0 = global i64 0
@xhci_irq_1 = global i64 0
@xhci_irq_2 = global i64 0
@xhci_irq_3 = global i64 0
@xhci_active_0 = global i64 0
@xhci_active_1 = global i64 0
@xhci_active_2 = global i64 0
@xhci_active_3 = global i64 0
@xhci_count = global i64 0
@xhci_cmd_ring_0 = global i64 0
@xhci_cmd_ring_1 = global i64 0
@xhci_cmd_ring_2 = global i64 0
@xhci_cmd_ring_3 = global i64 0
@xhci_cmd_enq_0 = global i64 0
@xhci_cmd_enq_1 = global i64 0
@xhci_cmd_enq_2 = global i64 0
@xhci_cmd_enq_3 = global i64 0
@xhci_cmd_cycle_0 = global i64 1
@xhci_cmd_cycle_1 = global i64 1
@xhci_cmd_cycle_2 = global i64 1
@xhci_cmd_cycle_3 = global i64 1
@xhci_evt_ring_0 = global i64 0
@xhci_evt_ring_1 = global i64 0
@xhci_evt_ring_2 = global i64 0
@xhci_evt_ring_3 = global i64 0
@xhci_evt_deq_0 = global i64 0
@xhci_evt_deq_1 = global i64 0
@xhci_evt_deq_2 = global i64 0
@xhci_evt_deq_3 = global i64 0
@xhci_evt_cycle_0 = global i64 1
@xhci_evt_cycle_1 = global i64 1
@xhci_evt_cycle_2 = global i64 1
@xhci_evt_cycle_3 = global i64 1
@xhci_dcbaa_0 = global i64 0
@xhci_dcbaa_1 = global i64 0
@xhci_dcbaa_2 = global i64 0
@xhci_dcbaa_3 = global i64 0
@xhci_erst_0 = global i64 0
@xhci_erst_1 = global i64 0
@xhci_erst_2 = global i64 0
@xhci_erst_3 = global i64 0
@xhci_scratch_0 = global i64 0
@xhci_scratch_1 = global i64 0
@xhci_scratch_2 = global i64 0
@xhci_scratch_3 = global i64 0
@xhci_commands_sent = global i64 0
@xhci_events_received = global i64 0
@xhci_errors = global i64 0
@xhci_devices_found = global i64 0
@str.const = private unnamed_addr constant [21 x i8] c"[xHCI] Halt timeout!\00", align 1
@str.const.1 = private unnamed_addr constant [22 x i8] c"[xHCI] Resetting HC #\00", align 1
@str.const.2 = private unnamed_addr constant [22 x i8] c"[xHCI] Reset timeout!\00", align 1
@str.const.3 = private unnamed_addr constant [22 x i8] c"[xHCI] Reset complete\00", align 1
@str.const.4 = private unnamed_addr constant [31 x i8] c"[xHCI] Rings allocated: cmd=0x\00", align 1
@str.const.5 = private unnamed_addr constant [8 x i8] c" evt=0x\00", align 1
@str.const.6 = private unnamed_addr constant [28 x i8] c"[xHCI] Registers configured\00", align 1
@str.const.7 = private unnamed_addr constant [22 x i8] c"[xHCI] Start timeout!\00", align 1
@str.const.8 = private unnamed_addr constant [18 x i8] c"[xHCI] HC running\00", align 1
@str.const.9 = private unnamed_addr constant [34 x i8] c"[xHCI] Command completion timeout\00", align 1
@str.const.10 = private unnamed_addr constant [26 x i8] c"[xHCI] Command failed CC=\00", align 1
@str.const.11 = private unnamed_addr constant [22 x i8] c"[xHCI] Enable Slot...\00", align 1
@str.const.12 = private unnamed_addr constant [22 x i8] c"[xHCI] Slot enabled: \00", align 1
@str.const.13 = private unnamed_addr constant [23 x i8] c"[xHCI] Resetting port \00", align 1
@str.const.14 = private unnamed_addr constant [26 x i8] c"[xHCI] Port reset timeout\00", align 1
@str.const.15 = private unnamed_addr constant [13 x i8] c"[xHCI] Port \00", align 1
@str.const.16 = private unnamed_addr constant [12 x i8] c" reset done\00", align 1
@str.const.17 = private unnamed_addr constant [5 x i8] c"Full\00", align 1
@str.const.18 = private unnamed_addr constant [4 x i8] c"Low\00", align 1
@str.const.19 = private unnamed_addr constant [5 x i8] c"High\00", align 1
@str.const.20 = private unnamed_addr constant [6 x i8] c"Super\00", align 1
@str.const.21 = private unnamed_addr constant [2 x i8] c"?\00", align 1
@str.const.22 = private unnamed_addr constant [17 x i8] c"[xHCI] Scanning \00", align 1
@str.const.23 = private unnamed_addr constant [7 x i8] c" ports\00", align 1
@str.const.24 = private unnamed_addr constant [14 x i8] c"[xHCI] Found \00", align 1
@str.const.25 = private unnamed_addr constant [9 x i8] c" devices\00", align 1
@str.const.27 = private unnamed_addr constant [27 x i8] c": device connected, speed=\00", align 1
@str.const.28 = private unnamed_addr constant [45 x i8] c"[xHCI] Detecting xHCI controllers via PCI...\00", align 1
@str.const.29 = private unnamed_addr constant [32 x i8] c"[xHCI] Total xHCI controllers: \00", align 1
@str.const.30 = private unnamed_addr constant [30 x i8] c"[xHCI] Found xHCI at PCI idx \00", align 1
@str.const.31 = private unnamed_addr constant [9 x i8] c" BAR0=0x\00", align 1
@str.const.32 = private unnamed_addr constant [12 x i8] c"[xHCI] HC #\00", align 1
@str.const.33 = private unnamed_addr constant [8 x i8] c" ports=\00", align 1
@str.const.34 = private unnamed_addr constant [8 x i8] c" slots=\00", align 1
@str.const.35 = private unnamed_addr constant [38 x i8] c"\E2\95\90\E2\95\90\E2\95\90 [xHCI] \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.36 = private unnamed_addr constant [19 x i8] c"  \D9\85\D8\AA\D8\AD\D9\83\D9\85\D8\A7\D8\AA: \00", align 1
@str.const.37 = private unnamed_addr constant [15 x i8] c"  \D8\A3\D9\88\D8\A7\D9\85\D8\B1: \00", align 1
@str.const.38 = private unnamed_addr constant [14 x i8] c" \D8\A3\D8\AD\D8\AF\D8\A7\D8\AB: \00", align 1
@str.const.39 = private unnamed_addr constant [14 x i8] c" \D8\A3\D8\AC\D9\87\D8\B2\D8\A9: \00", align 1
@str.const.40 = private unnamed_addr constant [14 x i8] c" \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1: \00", align 1
@str.const.41 = private unnamed_addr constant [106 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.42 = private unnamed_addr constant [7 x i8] c"  HC #\00", align 1
@str.const.43 = private unnamed_addr constant [9 x i8] c" base=0x\00", align 1
@str.const.46 = private unnamed_addr constant [41 x i8] c"[xHCI] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D8\AA\D8\B9\D8\B1\D9\8A\D9\81\D8\A7\D8\AA xHCI...\00", align 1
@str.const.47 = private unnamed_addr constant [16 x i8] c"[xHCI] \D8\AC\D8\A7\D9\87\D8\B2\00", align 1

declare void @sad_ll_mem_read8(i64)

declare void @sad_ll_mem_write8(i64, i64)

declare void @sad_ll_mem_read32(i64)

declare void @sad_ll_mem_write32(i64, i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_kmalloc(i64)

declare void @sad_ll_memset(i64, i64, i64)

declare void @sad_ll_memcpy(i64, i64, i64)

declare void @sad_ll_timer_get_ticks()

declare void @pci_find_usb_controller(i64)

declare void @pci_get_usb_type(i64)

declare void @pci_get_bar0(i64)

declare void @pci_get_irq(i64)

declare void @pci_enable_bus_master(i64)

declare void @pci_bar_address(i64)

declare void @usb_register_device(i64, i64, i64, i64, i64, i64, i64)

declare void @usb_build_setup(i64, i64, i64, i64, i64, i64)

declare void @usb_parse_device_descriptor(i64, i64)

declare void @usb_classify_device(i64)

define i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
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

define i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
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

define i64 @xhci_serial_hex8(i64 %val) {
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

define i64 @xhci_serial_hex32(i64 %val) {
entry:
  %val1 = alloca i64, align 8
  store i64 %val, ptr %val1, align 8
  %"%val.load" = load i64, ptr %val1, align 8
  %shrtmp = lshr i64 %"%val.load", 24
  %calltmp = call i64 @xhci_serial_hex8(i64 %shrtmp)
  %"%val.load2" = load i64, ptr %val1, align 8
  %shrtmp3 = lshr i64 %"%val.load2", 16
  %andtmp = and i64 %shrtmp3, 255
  %calltmp4 = call i64 @xhci_serial_hex8(i64 %andtmp)
  %"%val.load5" = load i64, ptr %val1, align 8
  %shrtmp6 = lshr i64 %"%val.load5", 8
  %andtmp7 = and i64 %shrtmp6, 255
  %calltmp8 = call i64 @xhci_serial_hex8(i64 %andtmp7)
  %"%val.load9" = load i64, ptr %val1, align 8
  %andtmp10 = and i64 %"%val.load9", 255
  %calltmp11 = call i64 @xhci_serial_hex8(i64 %andtmp10)
  ret i64 0
}

define i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"() {
entry:
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 13)
  %"%COM1.load1" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load1", i64 10)
  ret i64 0
}

define i64 @xhci_get_base(i64 %idx) {
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
  %loadtmp = load i64, ptr @xhci_base_0, align 8
  ret i64 %loadtmp

then_32:                                          ; preds = %merge_31
  %loadtmp4 = load i64, ptr @xhci_base_1, align 8
  ret i64 %loadtmp4

then_34:                                          ; preds = %merge_33
  %loadtmp7 = load i64, ptr @xhci_base_2, align 8
  ret i64 %loadtmp7

then_36:                                          ; preds = %merge_35
  %loadtmp10 = load i64, ptr @xhci_base_3, align 8
  ret i64 %loadtmp10
}

define i64 @xhci_set_base(i64 %idx, i64 %val) {
entry:
  %"%xhci_base_3" = alloca i64, align 8
  %"%xhci_base_2" = alloca i64, align 8
  %"%xhci_base_1" = alloca i64, align 8
  %"%xhci_base_0" = alloca i64, align 8
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
  %"%147.load" = load i64, ptr %val2, align 8
  store i64 %"%147.load", ptr %"%xhci_base_0", align 8
  br label %merge_39

then_40:                                          ; preds = %merge_39
  %"%151.load" = load i64, ptr %val2, align 8
  store i64 %"%151.load", ptr %"%xhci_base_1", align 8
  br label %merge_41

then_42:                                          ; preds = %merge_41
  %"%155.load" = load i64, ptr %val2, align 8
  store i64 %"%155.load", ptr %"%xhci_base_2", align 8
  br label %merge_43

then_44:                                          ; preds = %merge_43
  %"%159.load" = load i64, ptr %val2, align 8
  store i64 %"%159.load", ptr %"%xhci_base_3", align 8
  br label %merge_45
}

define i64 @xhci_get_op_base(i64 %idx) {
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
  %loadtmp = load i64, ptr @xhci_op_base_0, align 8
  ret i64 %loadtmp

then_48:                                          ; preds = %merge_47
  %loadtmp4 = load i64, ptr @xhci_op_base_1, align 8
  ret i64 %loadtmp4

then_50:                                          ; preds = %merge_49
  %loadtmp7 = load i64, ptr @xhci_op_base_2, align 8
  ret i64 %loadtmp7

then_52:                                          ; preds = %merge_51
  %loadtmp10 = load i64, ptr @xhci_op_base_3, align 8
  ret i64 %loadtmp10
}

define i64 @xhci_set_op_base(i64 %idx, i64 %val) {
entry:
  %"%xhci_op_base_3" = alloca i64, align 8
  %"%xhci_op_base_2" = alloca i64, align 8
  %"%xhci_op_base_1" = alloca i64, align 8
  %"%xhci_op_base_0" = alloca i64, align 8
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
  %"%185.load" = load i64, ptr %val2, align 8
  store i64 %"%185.load", ptr %"%xhci_op_base_0", align 8
  br label %merge_55

then_56:                                          ; preds = %merge_55
  %"%189.load" = load i64, ptr %val2, align 8
  store i64 %"%189.load", ptr %"%xhci_op_base_1", align 8
  br label %merge_57

then_58:                                          ; preds = %merge_57
  %"%193.load" = load i64, ptr %val2, align 8
  store i64 %"%193.load", ptr %"%xhci_op_base_2", align 8
  br label %merge_59

then_60:                                          ; preds = %merge_59
  %"%197.load" = load i64, ptr %val2, align 8
  store i64 %"%197.load", ptr %"%xhci_op_base_3", align 8
  br label %merge_61
}

define i64 @xhci_get_num_ports(i64 %idx) {
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
  %loadtmp = load i64, ptr @xhci_num_ports_0, align 8
  ret i64 %loadtmp

then_64:                                          ; preds = %merge_63
  %loadtmp4 = load i64, ptr @xhci_num_ports_1, align 8
  ret i64 %loadtmp4

then_66:                                          ; preds = %merge_65
  %loadtmp7 = load i64, ptr @xhci_num_ports_2, align 8
  ret i64 %loadtmp7

then_68:                                          ; preds = %merge_67
  %loadtmp10 = load i64, ptr @xhci_num_ports_3, align 8
  ret i64 %loadtmp10
}

define i64 @xhci_set_num_ports(i64 %idx, i64 %val) {
entry:
  %"%xhci_num_ports_3" = alloca i64, align 8
  %"%xhci_num_ports_2" = alloca i64, align 8
  %"%xhci_num_ports_1" = alloca i64, align 8
  %"%xhci_num_ports_0" = alloca i64, align 8
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
  %"%223.load" = load i64, ptr %val2, align 8
  store i64 %"%223.load", ptr %"%xhci_num_ports_0", align 8
  br label %merge_71

then_72:                                          ; preds = %merge_71
  %"%227.load" = load i64, ptr %val2, align 8
  store i64 %"%227.load", ptr %"%xhci_num_ports_1", align 8
  br label %merge_73

then_74:                                          ; preds = %merge_73
  %"%231.load" = load i64, ptr %val2, align 8
  store i64 %"%231.load", ptr %"%xhci_num_ports_2", align 8
  br label %merge_75

then_76:                                          ; preds = %merge_75
  %"%235.load" = load i64, ptr %val2, align 8
  store i64 %"%235.load", ptr %"%xhci_num_ports_3", align 8
  br label %merge_77
}

define i64 @xhci_get_max_slots(i64 %idx) {
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
  %loadtmp = load i64, ptr @xhci_max_slots_0, align 8
  ret i64 %loadtmp

then_80:                                          ; preds = %merge_79
  %loadtmp4 = load i64, ptr @xhci_max_slots_1, align 8
  ret i64 %loadtmp4

then_82:                                          ; preds = %merge_81
  %loadtmp7 = load i64, ptr @xhci_max_slots_2, align 8
  ret i64 %loadtmp7

then_84:                                          ; preds = %merge_83
  %loadtmp10 = load i64, ptr @xhci_max_slots_3, align 8
  ret i64 %loadtmp10
}

define i64 @xhci_set_max_slots(i64 %idx, i64 %val) {
entry:
  %"%xhci_max_slots_3" = alloca i64, align 8
  %"%xhci_max_slots_2" = alloca i64, align 8
  %"%xhci_max_slots_1" = alloca i64, align 8
  %"%xhci_max_slots_0" = alloca i64, align 8
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
  %"%261.load" = load i64, ptr %val2, align 8
  store i64 %"%261.load", ptr %"%xhci_max_slots_0", align 8
  br label %merge_87

then_88:                                          ; preds = %merge_87
  %"%265.load" = load i64, ptr %val2, align 8
  store i64 %"%265.load", ptr %"%xhci_max_slots_1", align 8
  br label %merge_89

then_90:                                          ; preds = %merge_89
  %"%269.load" = load i64, ptr %val2, align 8
  store i64 %"%269.load", ptr %"%xhci_max_slots_2", align 8
  br label %merge_91

then_92:                                          ; preds = %merge_91
  %"%273.load" = load i64, ptr %val2, align 8
  store i64 %"%273.load", ptr %"%xhci_max_slots_3", align 8
  br label %merge_93
}

define i64 @xhci_get_active(i64 %idx) {
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
  %loadtmp10 = load i64, ptr @xhci_active_3, align 8
  ret i64 %loadtmp10

then_94:                                          ; preds = %entry
  %loadtmp = load i64, ptr @xhci_active_0, align 8
  ret i64 %loadtmp

then_96:                                          ; preds = %merge_95
  %loadtmp4 = load i64, ptr @xhci_active_1, align 8
  ret i64 %loadtmp4

then_98:                                          ; preds = %merge_97
  %loadtmp7 = load i64, ptr @xhci_active_2, align 8
  ret i64 %loadtmp7
}

define i64 @xhci_set_active(i64 %idx, i64 %val) {
entry:
  %"%xhci_active_3" = alloca i64, align 8
  %"%xhci_active_2" = alloca i64, align 8
  %"%xhci_active_1" = alloca i64, align 8
  %"%xhci_active_0" = alloca i64, align 8
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
  %"%299.load" = load i64, ptr %val2, align 8
  store i64 %"%299.load", ptr %"%xhci_active_0", align 8
  br label %merge_103

then_104:                                         ; preds = %merge_103
  %"%303.load" = load i64, ptr %val2, align 8
  store i64 %"%303.load", ptr %"%xhci_active_1", align 8
  br label %merge_105

then_106:                                         ; preds = %merge_105
  %"%307.load" = load i64, ptr %val2, align 8
  store i64 %"%307.load", ptr %"%xhci_active_2", align 8
  br label %merge_107

then_108:                                         ; preds = %merge_107
  %"%311.load" = load i64, ptr %val2, align 8
  store i64 %"%311.load", ptr %"%xhci_active_3", align 8
  br label %merge_109
}

define i64 @xhci_get_cmd_ring(i64 %idx) {
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
  %loadtmp = load i64, ptr @xhci_cmd_ring_0, align 8
  ret i64 %loadtmp

then_112:                                         ; preds = %merge_111
  %loadtmp4 = load i64, ptr @xhci_cmd_ring_1, align 8
  ret i64 %loadtmp4

then_114:                                         ; preds = %merge_113
  %loadtmp7 = load i64, ptr @xhci_cmd_ring_2, align 8
  ret i64 %loadtmp7

then_116:                                         ; preds = %merge_115
  %loadtmp10 = load i64, ptr @xhci_cmd_ring_3, align 8
  ret i64 %loadtmp10
}

define i64 @xhci_set_cmd_ring(i64 %idx, i64 %val) {
entry:
  %"%xhci_cmd_ring_3" = alloca i64, align 8
  %"%xhci_cmd_ring_2" = alloca i64, align 8
  %"%xhci_cmd_ring_1" = alloca i64, align 8
  %"%xhci_cmd_ring_0" = alloca i64, align 8
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
  %"%337.load" = load i64, ptr %val2, align 8
  store i64 %"%337.load", ptr %"%xhci_cmd_ring_0", align 8
  br label %merge_119

then_120:                                         ; preds = %merge_119
  %"%341.load" = load i64, ptr %val2, align 8
  store i64 %"%341.load", ptr %"%xhci_cmd_ring_1", align 8
  br label %merge_121

then_122:                                         ; preds = %merge_121
  %"%345.load" = load i64, ptr %val2, align 8
  store i64 %"%345.load", ptr %"%xhci_cmd_ring_2", align 8
  br label %merge_123

then_124:                                         ; preds = %merge_123
  %"%349.load" = load i64, ptr %val2, align 8
  store i64 %"%349.load", ptr %"%xhci_cmd_ring_3", align 8
  br label %merge_125
}

define i64 @xhci_get_cmd_enq(i64 %idx) {
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
  %loadtmp = load i64, ptr @xhci_cmd_enq_0, align 8
  ret i64 %loadtmp

then_128:                                         ; preds = %merge_127
  %loadtmp4 = load i64, ptr @xhci_cmd_enq_1, align 8
  ret i64 %loadtmp4

then_130:                                         ; preds = %merge_129
  %loadtmp7 = load i64, ptr @xhci_cmd_enq_2, align 8
  ret i64 %loadtmp7

then_132:                                         ; preds = %merge_131
  %loadtmp10 = load i64, ptr @xhci_cmd_enq_3, align 8
  ret i64 %loadtmp10
}

define i64 @xhci_set_cmd_enq(i64 %idx, i64 %val) {
entry:
  %"%xhci_cmd_enq_3" = alloca i64, align 8
  %"%xhci_cmd_enq_2" = alloca i64, align 8
  %"%xhci_cmd_enq_1" = alloca i64, align 8
  %"%xhci_cmd_enq_0" = alloca i64, align 8
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
  %"%375.load" = load i64, ptr %val2, align 8
  store i64 %"%375.load", ptr %"%xhci_cmd_enq_0", align 8
  br label %merge_135

then_136:                                         ; preds = %merge_135
  %"%379.load" = load i64, ptr %val2, align 8
  store i64 %"%379.load", ptr %"%xhci_cmd_enq_1", align 8
  br label %merge_137

then_138:                                         ; preds = %merge_137
  %"%383.load" = load i64, ptr %val2, align 8
  store i64 %"%383.load", ptr %"%xhci_cmd_enq_2", align 8
  br label %merge_139

then_140:                                         ; preds = %merge_139
  %"%387.load" = load i64, ptr %val2, align 8
  store i64 %"%387.load", ptr %"%xhci_cmd_enq_3", align 8
  br label %merge_141
}

define i64 @xhci_get_cmd_cycle(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_142, label %merge_143

merge_143:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_144, label %merge_145

merge_145:                                        ; preds = %merge_143
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_146, label %merge_147

merge_147:                                        ; preds = %merge_145
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_148, label %merge_149

merge_149:                                        ; preds = %merge_147
  ret i64 1

then_142:                                         ; preds = %entry
  %loadtmp = load i64, ptr @xhci_cmd_cycle_0, align 8
  ret i64 %loadtmp

then_144:                                         ; preds = %merge_143
  %loadtmp4 = load i64, ptr @xhci_cmd_cycle_1, align 8
  ret i64 %loadtmp4

then_146:                                         ; preds = %merge_145
  %loadtmp7 = load i64, ptr @xhci_cmd_cycle_2, align 8
  ret i64 %loadtmp7

then_148:                                         ; preds = %merge_147
  %loadtmp10 = load i64, ptr @xhci_cmd_cycle_3, align 8
  ret i64 %loadtmp10
}

define i64 @xhci_set_cmd_cycle(i64 %idx, i64 %val) {
entry:
  %"%xhci_cmd_cycle_3" = alloca i64, align 8
  %"%xhci_cmd_cycle_2" = alloca i64, align 8
  %"%xhci_cmd_cycle_1" = alloca i64, align 8
  %"%xhci_cmd_cycle_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_150, label %merge_151

merge_151:                                        ; preds = %then_150, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_152, label %merge_153

merge_153:                                        ; preds = %then_152, %merge_151
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_154, label %merge_155

merge_155:                                        ; preds = %then_154, %merge_153
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_156, label %merge_157

merge_157:                                        ; preds = %then_156, %merge_155
  ret i64 0

then_150:                                         ; preds = %entry
  %"%413.load" = load i64, ptr %val2, align 8
  store i64 %"%413.load", ptr %"%xhci_cmd_cycle_0", align 8
  br label %merge_151

then_152:                                         ; preds = %merge_151
  %"%417.load" = load i64, ptr %val2, align 8
  store i64 %"%417.load", ptr %"%xhci_cmd_cycle_1", align 8
  br label %merge_153

then_154:                                         ; preds = %merge_153
  %"%421.load" = load i64, ptr %val2, align 8
  store i64 %"%421.load", ptr %"%xhci_cmd_cycle_2", align 8
  br label %merge_155

then_156:                                         ; preds = %merge_155
  %"%425.load" = load i64, ptr %val2, align 8
  store i64 %"%425.load", ptr %"%xhci_cmd_cycle_3", align 8
  br label %merge_157
}

define i64 @xhci_get_evt_ring(i64 %idx) {
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
  ret i64 0

then_158:                                         ; preds = %entry
  %loadtmp = load i64, ptr @xhci_evt_ring_0, align 8
  ret i64 %loadtmp

then_160:                                         ; preds = %merge_159
  %loadtmp4 = load i64, ptr @xhci_evt_ring_1, align 8
  ret i64 %loadtmp4

then_162:                                         ; preds = %merge_161
  %loadtmp7 = load i64, ptr @xhci_evt_ring_2, align 8
  ret i64 %loadtmp7

then_164:                                         ; preds = %merge_163
  %loadtmp10 = load i64, ptr @xhci_evt_ring_3, align 8
  ret i64 %loadtmp10
}

define i64 @xhci_set_evt_ring(i64 %idx, i64 %val) {
entry:
  %"%xhci_evt_ring_3" = alloca i64, align 8
  %"%xhci_evt_ring_2" = alloca i64, align 8
  %"%xhci_evt_ring_1" = alloca i64, align 8
  %"%xhci_evt_ring_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_166, label %merge_167

merge_167:                                        ; preds = %then_166, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_168, label %merge_169

merge_169:                                        ; preds = %then_168, %merge_167
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_170, label %merge_171

merge_171:                                        ; preds = %then_170, %merge_169
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_172, label %merge_173

merge_173:                                        ; preds = %then_172, %merge_171
  ret i64 0

then_166:                                         ; preds = %entry
  %"%451.load" = load i64, ptr %val2, align 8
  store i64 %"%451.load", ptr %"%xhci_evt_ring_0", align 8
  br label %merge_167

then_168:                                         ; preds = %merge_167
  %"%455.load" = load i64, ptr %val2, align 8
  store i64 %"%455.load", ptr %"%xhci_evt_ring_1", align 8
  br label %merge_169

then_170:                                         ; preds = %merge_169
  %"%459.load" = load i64, ptr %val2, align 8
  store i64 %"%459.load", ptr %"%xhci_evt_ring_2", align 8
  br label %merge_171

then_172:                                         ; preds = %merge_171
  %"%463.load" = load i64, ptr %val2, align 8
  store i64 %"%463.load", ptr %"%xhci_evt_ring_3", align 8
  br label %merge_173
}

define i64 @xhci_get_evt_deq(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_174, label %merge_175

merge_175:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_176, label %merge_177

merge_177:                                        ; preds = %merge_175
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_178, label %merge_179

merge_179:                                        ; preds = %merge_177
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_180, label %merge_181

merge_181:                                        ; preds = %merge_179
  ret i64 0

then_174:                                         ; preds = %entry
  %loadtmp = load i64, ptr @xhci_evt_deq_0, align 8
  ret i64 %loadtmp

then_176:                                         ; preds = %merge_175
  %loadtmp4 = load i64, ptr @xhci_evt_deq_1, align 8
  ret i64 %loadtmp4

then_178:                                         ; preds = %merge_177
  %loadtmp7 = load i64, ptr @xhci_evt_deq_2, align 8
  ret i64 %loadtmp7

then_180:                                         ; preds = %merge_179
  %loadtmp10 = load i64, ptr @xhci_evt_deq_3, align 8
  ret i64 %loadtmp10
}

define i64 @xhci_set_evt_deq(i64 %idx, i64 %val) {
entry:
  %"%xhci_evt_deq_3" = alloca i64, align 8
  %"%xhci_evt_deq_2" = alloca i64, align 8
  %"%xhci_evt_deq_1" = alloca i64, align 8
  %"%xhci_evt_deq_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_182, label %merge_183

merge_183:                                        ; preds = %then_182, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_184, label %merge_185

merge_185:                                        ; preds = %then_184, %merge_183
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_186, label %merge_187

merge_187:                                        ; preds = %then_186, %merge_185
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_188, label %merge_189

merge_189:                                        ; preds = %then_188, %merge_187
  ret i64 0

then_182:                                         ; preds = %entry
  %"%489.load" = load i64, ptr %val2, align 8
  store i64 %"%489.load", ptr %"%xhci_evt_deq_0", align 8
  br label %merge_183

then_184:                                         ; preds = %merge_183
  %"%493.load" = load i64, ptr %val2, align 8
  store i64 %"%493.load", ptr %"%xhci_evt_deq_1", align 8
  br label %merge_185

then_186:                                         ; preds = %merge_185
  %"%497.load" = load i64, ptr %val2, align 8
  store i64 %"%497.load", ptr %"%xhci_evt_deq_2", align 8
  br label %merge_187

then_188:                                         ; preds = %merge_187
  %"%501.load" = load i64, ptr %val2, align 8
  store i64 %"%501.load", ptr %"%xhci_evt_deq_3", align 8
  br label %merge_189
}

define i64 @xhci_get_dcbaa(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_190, label %merge_191

merge_191:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_192, label %merge_193

merge_193:                                        ; preds = %merge_191
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_194, label %merge_195

merge_195:                                        ; preds = %merge_193
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_196, label %merge_197

merge_197:                                        ; preds = %merge_195
  ret i64 0

then_190:                                         ; preds = %entry
  %loadtmp = load i64, ptr @xhci_dcbaa_0, align 8
  ret i64 %loadtmp

then_192:                                         ; preds = %merge_191
  %loadtmp4 = load i64, ptr @xhci_dcbaa_1, align 8
  ret i64 %loadtmp4

then_194:                                         ; preds = %merge_193
  %loadtmp7 = load i64, ptr @xhci_dcbaa_2, align 8
  ret i64 %loadtmp7

then_196:                                         ; preds = %merge_195
  %loadtmp10 = load i64, ptr @xhci_dcbaa_3, align 8
  ret i64 %loadtmp10
}

define i64 @xhci_set_dcbaa(i64 %idx, i64 %val) {
entry:
  %"%xhci_dcbaa_3" = alloca i64, align 8
  %"%xhci_dcbaa_2" = alloca i64, align 8
  %"%xhci_dcbaa_1" = alloca i64, align 8
  %"%xhci_dcbaa_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_198, label %merge_199

merge_199:                                        ; preds = %then_198, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_200, label %merge_201

merge_201:                                        ; preds = %then_200, %merge_199
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_202, label %merge_203

merge_203:                                        ; preds = %then_202, %merge_201
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_204, label %merge_205

merge_205:                                        ; preds = %then_204, %merge_203
  ret i64 0

then_198:                                         ; preds = %entry
  %"%527.load" = load i64, ptr %val2, align 8
  store i64 %"%527.load", ptr %"%xhci_dcbaa_0", align 8
  br label %merge_199

then_200:                                         ; preds = %merge_199
  %"%531.load" = load i64, ptr %val2, align 8
  store i64 %"%531.load", ptr %"%xhci_dcbaa_1", align 8
  br label %merge_201

then_202:                                         ; preds = %merge_201
  %"%535.load" = load i64, ptr %val2, align 8
  store i64 %"%535.load", ptr %"%xhci_dcbaa_2", align 8
  br label %merge_203

then_204:                                         ; preds = %merge_203
  %"%539.load" = load i64, ptr %val2, align 8
  store i64 %"%539.load", ptr %"%xhci_dcbaa_3", align 8
  br label %merge_205
}

define i64 @xhci_get_db_off(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_206, label %merge_207

merge_207:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_208, label %merge_209

merge_209:                                        ; preds = %merge_207
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_210, label %merge_211

merge_211:                                        ; preds = %merge_209
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_212, label %merge_213

merge_213:                                        ; preds = %merge_211
  ret i64 0

then_206:                                         ; preds = %entry
  %loadtmp = load i64, ptr @xhci_db_off_0, align 8
  ret i64 %loadtmp

then_208:                                         ; preds = %merge_207
  %loadtmp4 = load i64, ptr @xhci_db_off_1, align 8
  ret i64 %loadtmp4

then_210:                                         ; preds = %merge_209
  %loadtmp7 = load i64, ptr @xhci_db_off_2, align 8
  ret i64 %loadtmp7

then_212:                                         ; preds = %merge_211
  %loadtmp10 = load i64, ptr @xhci_db_off_3, align 8
  ret i64 %loadtmp10
}

define i64 @xhci_set_db_off(i64 %idx, i64 %val) {
entry:
  %"%xhci_db_off_3" = alloca i64, align 8
  %"%xhci_db_off_2" = alloca i64, align 8
  %"%xhci_db_off_1" = alloca i64, align 8
  %"%xhci_db_off_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_214, label %merge_215

merge_215:                                        ; preds = %then_214, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_216, label %merge_217

merge_217:                                        ; preds = %then_216, %merge_215
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_218, label %merge_219

merge_219:                                        ; preds = %then_218, %merge_217
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_220, label %merge_221

merge_221:                                        ; preds = %then_220, %merge_219
  ret i64 0

then_214:                                         ; preds = %entry
  %"%565.load" = load i64, ptr %val2, align 8
  store i64 %"%565.load", ptr %"%xhci_db_off_0", align 8
  br label %merge_215

then_216:                                         ; preds = %merge_215
  %"%569.load" = load i64, ptr %val2, align 8
  store i64 %"%569.load", ptr %"%xhci_db_off_1", align 8
  br label %merge_217

then_218:                                         ; preds = %merge_217
  %"%573.load" = load i64, ptr %val2, align 8
  store i64 %"%573.load", ptr %"%xhci_db_off_2", align 8
  br label %merge_219

then_220:                                         ; preds = %merge_219
  %"%577.load" = load i64, ptr %val2, align 8
  store i64 %"%577.load", ptr %"%xhci_db_off_3", align 8
  br label %merge_221
}

define i64 @xhci_get_rt_off(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_222, label %merge_223

merge_223:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_224, label %merge_225

merge_225:                                        ; preds = %merge_223
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_226, label %merge_227

merge_227:                                        ; preds = %merge_225
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_228, label %merge_229

merge_229:                                        ; preds = %merge_227
  ret i64 0

then_222:                                         ; preds = %entry
  %loadtmp = load i64, ptr @xhci_rt_off_0, align 8
  ret i64 %loadtmp

then_224:                                         ; preds = %merge_223
  %loadtmp4 = load i64, ptr @xhci_rt_off_1, align 8
  ret i64 %loadtmp4

then_226:                                         ; preds = %merge_225
  %loadtmp7 = load i64, ptr @xhci_rt_off_2, align 8
  ret i64 %loadtmp7

then_228:                                         ; preds = %merge_227
  %loadtmp10 = load i64, ptr @xhci_rt_off_3, align 8
  ret i64 %loadtmp10
}

define i64 @xhci_set_rt_off(i64 %idx, i64 %val) {
entry:
  %"%xhci_rt_off_3" = alloca i64, align 8
  %"%xhci_rt_off_2" = alloca i64, align 8
  %"%xhci_rt_off_1" = alloca i64, align 8
  %"%xhci_rt_off_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_230, label %merge_231

merge_231:                                        ; preds = %then_230, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_232, label %merge_233

merge_233:                                        ; preds = %then_232, %merge_231
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_234, label %merge_235

merge_235:                                        ; preds = %then_234, %merge_233
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_236, label %merge_237

merge_237:                                        ; preds = %then_236, %merge_235
  ret i64 0

then_230:                                         ; preds = %entry
  %"%603.load" = load i64, ptr %val2, align 8
  store i64 %"%603.load", ptr %"%xhci_rt_off_0", align 8
  br label %merge_231

then_232:                                         ; preds = %merge_231
  %"%607.load" = load i64, ptr %val2, align 8
  store i64 %"%607.load", ptr %"%xhci_rt_off_1", align 8
  br label %merge_233

then_234:                                         ; preds = %merge_233
  %"%611.load" = load i64, ptr %val2, align 8
  store i64 %"%611.load", ptr %"%xhci_rt_off_2", align 8
  br label %merge_235

then_236:                                         ; preds = %merge_235
  %"%615.load" = load i64, ptr %val2, align 8
  store i64 %"%615.load", ptr %"%xhci_rt_off_3", align 8
  br label %merge_237
}

define i64 @xhci_read_cap(i64 %hc_idx, i64 %offset) {
entry:
  %"%base" = alloca i64, align 8
  %offset2 = alloca i64, align 8
  store i64 %offset, ptr %offset2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @xhci_get_base(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%base", align 8
  %"%base.load" = load i64, ptr %"%base", align 8
  %"%offset.load" = load i64, ptr %offset2, align 8
  %addtmp = add i64 %"%base.load", %"%offset.load"
  call void @sad_ll_mem_read32(i64 %addtmp)
  ret i64 0
}

define i64 @xhci_read_op(i64 %hc_idx, i64 %offset) {
entry:
  %"%op" = alloca i64, align 8
  %offset2 = alloca i64, align 8
  store i64 %offset, ptr %offset2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @xhci_get_op_base(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%op", align 8
  %"%op.load" = load i64, ptr %"%op", align 8
  %"%offset.load" = load i64, ptr %offset2, align 8
  %addtmp = add i64 %"%op.load", %"%offset.load"
  call void @sad_ll_mem_read32(i64 %addtmp)
  ret i64 0
}

define i64 @xhci_write_op(i64 %hc_idx, i64 %offset, i64 %val) {
entry:
  %"%op" = alloca i64, align 8
  %val3 = alloca i64, align 8
  store i64 %val, ptr %val3, align 8
  %offset2 = alloca i64, align 8
  store i64 %offset, ptr %offset2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @xhci_get_op_base(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%op", align 8
  %"%op.load" = load i64, ptr %"%op", align 8
  %"%offset.load" = load i64, ptr %offset2, align 8
  %addtmp = add i64 %"%op.load", %"%offset.load"
  %"%val.load" = load i64, ptr %val3, align 8
  call void @sad_ll_mem_write32(i64 %addtmp, i64 %"%val.load")
  ret i64 0
}

define i64 @xhci_read_port(i64 %hc_idx, i64 %port_num) {
entry:
  %"%port_off" = alloca i64, align 8
  %"%op" = alloca i64, align 8
  %port_num2 = alloca i64, align 8
  store i64 %port_num, ptr %port_num2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @xhci_get_op_base(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%op", align 8
  %"%port_num.load" = load i64, ptr %port_num2, align 8
  %"%XHCI_PORTSC_SIZE.load" = load i64, ptr @XHCI_PORTSC_SIZE, align 8
  %multmp = mul i64 %"%port_num.load", %"%XHCI_PORTSC_SIZE.load"
  %"%XHCI_PORTSC_OFFSET.load" = load i64, ptr @XHCI_PORTSC_OFFSET, align 8
  %addtmp = add i64 %"%XHCI_PORTSC_OFFSET.load", %multmp
  store i64 %addtmp, ptr %"%port_off", align 8
  %"%op.load" = load i64, ptr %"%op", align 8
  %"%port_off.load" = load i64, ptr %"%port_off", align 8
  %addtmp3 = add i64 %"%op.load", %"%port_off.load"
  call void @sad_ll_mem_read32(i64 %addtmp3)
  ret i64 0
}

define i64 @xhci_write_port(i64 %hc_idx, i64 %port_num, i64 %val) {
entry:
  %"%port_off" = alloca i64, align 8
  %"%op" = alloca i64, align 8
  %val3 = alloca i64, align 8
  store i64 %val, ptr %val3, align 8
  %port_num2 = alloca i64, align 8
  store i64 %port_num, ptr %port_num2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @xhci_get_op_base(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%op", align 8
  %"%port_num.load" = load i64, ptr %port_num2, align 8
  %"%XHCI_PORTSC_SIZE.load" = load i64, ptr @XHCI_PORTSC_SIZE, align 8
  %multmp = mul i64 %"%port_num.load", %"%XHCI_PORTSC_SIZE.load"
  %"%XHCI_PORTSC_OFFSET.load" = load i64, ptr @XHCI_PORTSC_OFFSET, align 8
  %addtmp = add i64 %"%XHCI_PORTSC_OFFSET.load", %multmp
  store i64 %addtmp, ptr %"%port_off", align 8
  %"%op.load" = load i64, ptr %"%op", align 8
  %"%port_off.load" = load i64, ptr %"%port_off", align 8
  %addtmp4 = add i64 %"%op.load", %"%port_off.load"
  %"%val.load" = load i64, ptr %val3, align 8
  call void @sad_ll_mem_write32(i64 %addtmp4, i64 %"%val.load")
  ret i64 0
}

define i64 @xhci_ring_doorbell(i64 %hc_idx, i64 %slot_id, i64 %target) {
entry:
  %"%db" = alloca i64, align 8
  %"%base" = alloca i64, align 8
  %target3 = alloca i64, align 8
  store i64 %target, ptr %target3, align 8
  %slot_id2 = alloca i64, align 8
  store i64 %slot_id, ptr %slot_id2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @xhci_get_base(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%base", align 8
  %"%hc_idx.load4" = load i64, ptr %hc_idx1, align 8
  %calltmp5 = call i64 @xhci_get_db_off(i64 %"%hc_idx.load4")
  store i64 %calltmp5, ptr %"%db", align 8
  %"%base.load" = load i64, ptr %"%base", align 8
  %"%db.load" = load i64, ptr %"%db", align 8
  %addtmp = add i64 %"%base.load", %"%db.load"
  %"%slot_id.load" = load i64, ptr %slot_id2, align 8
  %shltmp = shl i64 %"%slot_id.load", 2
  %addtmp6 = add i64 %addtmp, %shltmp
  %"%target.load" = load i64, ptr %target3, align 8
  call void @sad_ll_mem_write32(i64 %addtmp6, i64 %"%target.load")
  ret i64 0
}

define i64 @xhci_write_trb(i64 %trb_addr, i64 %dw0, i64 %dw1, i64 %dw2, i64 %dw3) {
entry:
  %dw35 = alloca i64, align 8
  store i64 %dw3, ptr %dw35, align 8
  %dw24 = alloca i64, align 8
  store i64 %dw2, ptr %dw24, align 8
  %dw13 = alloca i64, align 8
  store i64 %dw1, ptr %dw13, align 8
  %dw02 = alloca i64, align 8
  store i64 %dw0, ptr %dw02, align 8
  %trb_addr1 = alloca i64, align 8
  store i64 %trb_addr, ptr %trb_addr1, align 8
  %"%trb_addr.load" = load i64, ptr %trb_addr1, align 8
  %"%dw0.load" = load i64, ptr %dw02, align 8
  call void @sad_ll_mem_write32(i64 %"%trb_addr.load", i64 %"%dw0.load")
  %"%trb_addr.load6" = load i64, ptr %trb_addr1, align 8
  %addtmp = add i64 %"%trb_addr.load6", 4
  %"%dw1.load" = load i64, ptr %dw13, align 8
  call void @sad_ll_mem_write32(i64 %addtmp, i64 %"%dw1.load")
  %"%trb_addr.load7" = load i64, ptr %trb_addr1, align 8
  %addtmp8 = add i64 %"%trb_addr.load7", 8
  %"%dw2.load" = load i64, ptr %dw24, align 8
  call void @sad_ll_mem_write32(i64 %addtmp8, i64 %"%dw2.load")
  %"%trb_addr.load9" = load i64, ptr %trb_addr1, align 8
  %addtmp10 = add i64 %"%trb_addr.load9", 12
  %"%dw3.load" = load i64, ptr %dw35, align 8
  call void @sad_ll_mem_write32(i64 %addtmp10, i64 %"%dw3.load")
  ret i64 0
}

define i64 @xhci_read_trb_dw0(i64 %trb_addr) {
entry:
  %trb_addr1 = alloca i64, align 8
  store i64 %trb_addr, ptr %trb_addr1, align 8
  %"%trb_addr.load" = load i64, ptr %trb_addr1, align 8
  call void @sad_ll_mem_read32(i64 %"%trb_addr.load")
  ret i64 0
}

define i64 @xhci_read_trb_dw1(i64 %trb_addr) {
entry:
  %trb_addr1 = alloca i64, align 8
  store i64 %trb_addr, ptr %trb_addr1, align 8
  %"%trb_addr.load" = load i64, ptr %trb_addr1, align 8
  %addtmp = add i64 %"%trb_addr.load", 4
  call void @sad_ll_mem_read32(i64 %addtmp)
  ret i64 0
}

define i64 @xhci_read_trb_dw2(i64 %trb_addr) {
entry:
  %trb_addr1 = alloca i64, align 8
  store i64 %trb_addr, ptr %trb_addr1, align 8
  %"%trb_addr.load" = load i64, ptr %trb_addr1, align 8
  %addtmp = add i64 %"%trb_addr.load", 8
  call void @sad_ll_mem_read32(i64 %addtmp)
  ret i64 0
}

define i64 @xhci_read_trb_dw3(i64 %trb_addr) {
entry:
  %trb_addr1 = alloca i64, align 8
  store i64 %trb_addr, ptr %trb_addr1, align 8
  %"%trb_addr.load" = load i64, ptr %trb_addr1, align 8
  %addtmp = add i64 %"%trb_addr.load", 12
  call void @sad_ll_mem_read32(i64 %addtmp)
  ret i64 0
}

define i64 @xhci_trb_type(i64 %dw3) {
entry:
  %dw31 = alloca i64, align 8
  store i64 %dw3, ptr %dw31, align 8
  %"%dw3.load" = load i64, ptr %dw31, align 8
  %shrtmp = lshr i64 %"%dw3.load", 10
  %andtmp = and i64 %shrtmp, 63
  ret i64 %andtmp
}

define i64 @xhci_trb_cycle(i64 %dw3) {
entry:
  %dw31 = alloca i64, align 8
  store i64 %dw3, ptr %dw31, align 8
  %"%dw3.load" = load i64, ptr %dw31, align 8
  %andtmp = and i64 %"%dw3.load", 1
  ret i64 %andtmp
}

define i64 @xhci_trb_completion_code(i64 %dw2) {
entry:
  %dw21 = alloca i64, align 8
  store i64 %dw2, ptr %dw21, align 8
  %"%dw2.load" = load i64, ptr %dw21, align 8
  %shrtmp = lshr i64 %"%dw2.load", 24
  %andtmp = and i64 %shrtmp, 255
  ret i64 %andtmp
}

define i64 @xhci_make_trb_control(i64 %trb_type, i64 %cycle, i64 %flags) {
entry:
  %flags3 = alloca i64, align 8
  store i64 %flags, ptr %flags3, align 8
  %cycle2 = alloca i64, align 8
  store i64 %cycle, ptr %cycle2, align 8
  %trb_type1 = alloca i64, align 8
  store i64 %trb_type, ptr %trb_type1, align 8
  %"%trb_type.load" = load i64, ptr %trb_type1, align 8
  %shltmp = shl i64 %"%trb_type.load", 10
  %"%cycle.load" = load i64, ptr %cycle2, align 8
  %addtmp = add i64 %shltmp, %"%cycle.load"
  %"%flags.load" = load i64, ptr %flags3, align 8
  %addtmp4 = add i64 %addtmp, %"%flags.load"
  ret i64 %addtmp4
}

define i64 @xhci_halt(i64 %hc_idx) {
entry:
  %"%sts" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  %"%cmd" = alloca i64, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %"%XHCI_OP_USBCMD.load" = load i64, ptr @XHCI_OP_USBCMD, align 8
  %calltmp = call i64 @xhci_read_op(i64 %"%hc_idx.load", i64 %"%XHCI_OP_USBCMD.load")
  store i64 %calltmp, ptr %"%cmd", align 8
  %"%cmd.load" = load i64, ptr %"%cmd", align 8
  %andtmp = and i64 %"%cmd.load", 1
  %"%cmd.load2" = load i64, ptr %"%cmd", align 8
  %subtmp = sub i64 %"%cmd.load2", %andtmp
  store i64 %subtmp, ptr %"%cmd", align 8
  %"%hc_idx.load3" = load i64, ptr %hc_idx1, align 8
  %"%XHCI_OP_USBCMD.load4" = load i64, ptr @XHCI_OP_USBCMD, align 8
  %"%cmd.load5" = load i64, ptr %"%cmd", align 8
  %calltmp6 = call i64 @xhci_write_op(i64 %"%hc_idx.load3", i64 %"%XHCI_OP_USBCMD.load4", i64 %"%cmd.load5")
  store i64 1000, ptr %"%timeout", align 8
  br label %while_cond_238

merge_242:                                        ; preds = %while_body_239
  %"%timeout.load12" = load i64, ptr %"%timeout", align 8
  %subtmp13 = sub i64 %"%timeout.load12", 1
  store i64 %subtmp13, ptr %"%timeout", align 8
  br label %while_cond_238

then_241:                                         ; preds = %while_body_239
  ret i64 0

while_body_239:                                   ; preds = %while_cond_238
  %"%hc_idx.load7" = load i64, ptr %hc_idx1, align 8
  %"%XHCI_OP_USBSTS.load" = load i64, ptr @XHCI_OP_USBSTS, align 8
  %calltmp8 = call i64 @xhci_read_op(i64 %"%hc_idx.load7", i64 %"%XHCI_OP_USBSTS.load")
  store i64 %calltmp8, ptr %"%sts", align 8
  %"%sts.load" = load i64, ptr %"%sts", align 8
  %andtmp9 = and i64 %"%sts.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp9, 1
  br i1 %cmpeqtmp, label %then_241, label %merge_242

while_cond_238:                                   ; preds = %merge_242, %entry
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp, label %while_body_239, label %while_exit_240

while_exit_240:                                   ; preds = %while_cond_238
  %calltmp10 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %calltmp11 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -1
}

define i64 @xhci_reset(i64 %hc_idx) {
entry:
  %"%xhci_errors" = alloca i64, align 8
  %"%usbsts" = alloca i64, align 8
  %"%usbcmd" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  %"%cmd" = alloca i64, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %calltmp = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %"%752.load" = load i64, ptr %hc_idx1, align 8
  %calltmp2 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%752.load")
  %calltmp3 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp4 = call i64 @xhci_halt(i64 %"%hc_idx.load")
  %"%hc_idx.load5" = load i64, ptr %hc_idx1, align 8
  %"%XHCI_OP_USBCMD.load" = load i64, ptr @XHCI_OP_USBCMD, align 8
  %calltmp6 = call i64 @xhci_read_op(i64 %"%hc_idx.load5", i64 %"%XHCI_OP_USBCMD.load")
  store i64 %calltmp6, ptr %"%cmd", align 8
  %"%cmd.load" = load i64, ptr %"%cmd", align 8
  %"%XHCI_CMD_HCRST.load" = load i64, ptr @XHCI_CMD_HCRST, align 8
  %addtmp = add i64 %"%cmd.load", %"%XHCI_CMD_HCRST.load"
  %"%hc_idx.load7" = load i64, ptr %hc_idx1, align 8
  %"%XHCI_OP_USBCMD.load8" = load i64, ptr @XHCI_OP_USBCMD, align 8
  %calltmp9 = call i64 @xhci_write_op(i64 %"%hc_idx.load7", i64 %"%XHCI_OP_USBCMD.load8", i64 %addtmp)
  store i64 5000, ptr %"%timeout", align 8
  br label %while_cond_243

merge_247:                                        ; preds = %merge_249, %while_body_244
  %"%timeout.load21" = load i64, ptr %"%timeout", align 8
  %subtmp = sub i64 %"%timeout.load21", 1
  store i64 %subtmp, ptr %"%timeout", align 8
  br label %while_cond_243

merge_249:                                        ; preds = %then_246
  br label %merge_247

then_246:                                         ; preds = %while_body_244
  %"%usbsts.load" = load i64, ptr %"%usbsts", align 8
  %shrtmp18 = lshr i64 %"%usbsts.load", 11
  %andtmp19 = and i64 %shrtmp18, 1
  %cmpeqtmp20 = icmp eq i64 %andtmp19, 0
  br i1 %cmpeqtmp20, label %then_248, label %merge_249

then_248:                                         ; preds = %then_246
  %calltmp22 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %calltmp23 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

while_body_244:                                   ; preds = %while_cond_243
  %"%hc_idx.load10" = load i64, ptr %hc_idx1, align 8
  %"%XHCI_OP_USBCMD.load11" = load i64, ptr @XHCI_OP_USBCMD, align 8
  %calltmp12 = call i64 @xhci_read_op(i64 %"%hc_idx.load10", i64 %"%XHCI_OP_USBCMD.load11")
  store i64 %calltmp12, ptr %"%usbcmd", align 8
  %"%hc_idx.load13" = load i64, ptr %hc_idx1, align 8
  %"%XHCI_OP_USBSTS.load" = load i64, ptr @XHCI_OP_USBSTS, align 8
  %calltmp14 = call i64 @xhci_read_op(i64 %"%hc_idx.load13", i64 %"%XHCI_OP_USBSTS.load")
  store i64 %calltmp14, ptr %"%usbsts", align 8
  %"%usbcmd.load" = load i64, ptr %"%usbcmd", align 8
  %shrtmp = lshr i64 %"%usbcmd.load", 1
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_246, label %merge_247

while_cond_243:                                   ; preds = %merge_247, %entry
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp, label %while_body_244, label %while_exit_245

while_exit_245:                                   ; preds = %while_cond_243
  %calltmp15 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %calltmp16 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%xhci_errors.load" = load i64, ptr @xhci_errors, align 8
  %addtmp17 = add i64 %"%xhci_errors.load", 1
  store i64 %addtmp17, ptr %"%xhci_errors", align 8
  ret i64 -1
}

define i64 @xhci_alloc_rings(i64 %hc_idx) {
entry:
  %"%xhci_erst_3" = alloca i64, align 8
  %"%xhci_erst_2" = alloca i64, align 8
  %"%xhci_erst_1" = alloca i64, align 8
  %"%xhci_erst_0" = alloca i64, align 8
  %"%erst" = alloca i64, align 8
  %"%dcbaa_aligned" = alloca i64, align 8
  %"%dcbaa" = alloca i64, align 8
  %"%dcbaa_size" = alloca i64, align 8
  %"%max_slots" = alloca i64, align 8
  %"%evt_ring" = alloca i64, align 8
  %"%link_addr" = alloca i64, align 8
  %"%cmd_ring" = alloca i64, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  call void @sad_ll_kmalloc(i64 1024)
  store i64 0, ptr %"%cmd_ring", align 8
  %"%cmd_ring.load" = load i64, ptr %"%cmd_ring", align 8
  call void @sad_ll_memset(i64 %"%cmd_ring.load", i64 0, i64 1024)
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %"%cmd_ring.load2" = load i64, ptr %"%cmd_ring", align 8
  %calltmp = call i64 @xhci_set_cmd_ring(i64 %"%hc_idx.load", i64 %"%cmd_ring.load2")
  %"%hc_idx.load3" = load i64, ptr %hc_idx1, align 8
  %calltmp4 = call i64 @xhci_set_cmd_enq(i64 %"%hc_idx.load3", i64 0)
  %"%hc_idx.load5" = load i64, ptr %hc_idx1, align 8
  %calltmp6 = call i64 @xhci_set_cmd_cycle(i64 %"%hc_idx.load5", i64 1)
  %"%cmd_ring.load7" = load i64, ptr %"%cmd_ring", align 8
  %addtmp = add i64 %"%cmd_ring.load7", 1008
  store i64 %addtmp, ptr %"%link_addr", align 8
  %"%TRB_LINK.load" = load i64, ptr @TRB_LINK, align 8
  %calltmp8 = call i64 @xhci_make_trb_control(i64 %"%TRB_LINK.load", i64 1, i64 32)
  %"%link_addr.load" = load i64, ptr %"%link_addr", align 8
  %"%cmd_ring.load9" = load i64, ptr %"%cmd_ring", align 8
  %calltmp10 = call i64 @xhci_write_trb(i64 %"%link_addr.load", i64 %"%cmd_ring.load9", i64 0, i64 0, i64 %calltmp8)
  call void @sad_ll_kmalloc(i64 1024)
  store i64 0, ptr %"%evt_ring", align 8
  %"%evt_ring.load" = load i64, ptr %"%evt_ring", align 8
  call void @sad_ll_memset(i64 %"%evt_ring.load", i64 0, i64 1024)
  %"%hc_idx.load11" = load i64, ptr %hc_idx1, align 8
  %"%evt_ring.load12" = load i64, ptr %"%evt_ring", align 8
  %calltmp13 = call i64 @xhci_set_evt_ring(i64 %"%hc_idx.load11", i64 %"%evt_ring.load12")
  %"%hc_idx.load14" = load i64, ptr %hc_idx1, align 8
  %calltmp15 = call i64 @xhci_set_evt_deq(i64 %"%hc_idx.load14", i64 0)
  %"%hc_idx.load16" = load i64, ptr %hc_idx1, align 8
  %calltmp17 = call i64 @xhci_get_max_slots(i64 %"%hc_idx.load16")
  store i64 %calltmp17, ptr %"%max_slots", align 8
  %"%max_slots.load" = load i64, ptr %"%max_slots", align 8
  %"%XHCI_MAX_SLOTS.load" = load i64, ptr @XHCI_MAX_SLOTS, align 8
  %cmpgttmp = icmp sgt i64 %"%max_slots.load", %"%XHCI_MAX_SLOTS.load"
  br i1 %cmpgttmp, label %then_250, label %merge_251

merge_251:                                        ; preds = %then_250, %entry
  %"%max_slots.load19" = load i64, ptr %"%max_slots", align 8
  %addtmp20 = add i64 %"%max_slots.load19", 1
  %shltmp = shl i64 %addtmp20, 3
  store i64 %shltmp, ptr %"%dcbaa_size", align 8
  %"%dcbaa_size.load" = load i64, ptr %"%dcbaa_size", align 8
  %addtmp21 = add i64 %"%dcbaa_size.load", 64
  call void @sad_ll_kmalloc(i64 %addtmp21)
  store i64 0, ptr %"%dcbaa", align 8
  %"%dcbaa.load" = load i64, ptr %"%dcbaa", align 8
  %addtmp22 = add i64 %"%dcbaa.load", 63
  %shrtmp = lshr i64 %addtmp22, 6
  %shltmp23 = shl i64 %shrtmp, 6
  store i64 %shltmp23, ptr %"%dcbaa_aligned", align 8
  %"%dcbaa_aligned.load" = load i64, ptr %"%dcbaa_aligned", align 8
  %"%dcbaa_size.load24" = load i64, ptr %"%dcbaa_size", align 8
  call void @sad_ll_memset(i64 %"%dcbaa_aligned.load", i64 0, i64 %"%dcbaa_size.load24")
  %"%hc_idx.load25" = load i64, ptr %hc_idx1, align 8
  %"%dcbaa_aligned.load26" = load i64, ptr %"%dcbaa_aligned", align 8
  %calltmp27 = call i64 @xhci_set_dcbaa(i64 %"%hc_idx.load25", i64 %"%dcbaa_aligned.load26")
  call void @sad_ll_kmalloc(i64 64)
  store i64 0, ptr %"%erst", align 8
  %"%erst.load" = load i64, ptr %"%erst", align 8
  call void @sad_ll_memset(i64 %"%erst.load", i64 0, i64 64)
  %"%erst.load28" = load i64, ptr %"%erst", align 8
  %"%evt_ring.load29" = load i64, ptr %"%evt_ring", align 8
  call void @sad_ll_mem_write32(i64 %"%erst.load28", i64 %"%evt_ring.load29")
  %"%erst.load30" = load i64, ptr %"%erst", align 8
  %addtmp31 = add i64 %"%erst.load30", 4
  call void @sad_ll_mem_write32(i64 %addtmp31, i64 0)
  %"%erst.load32" = load i64, ptr %"%erst", align 8
  %addtmp33 = add i64 %"%erst.load32", 8
  %"%XHCI_MAX_TRB_RING.load" = load i64, ptr @XHCI_MAX_TRB_RING, align 8
  call void @sad_ll_mem_write32(i64 %addtmp33, i64 %"%XHCI_MAX_TRB_RING.load")
  %"%hc_idx.load34" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%hc_idx.load34", 0
  br i1 %cmpeqtmp, label %then_252, label %merge_253

merge_253:                                        ; preds = %then_252, %merge_251
  %"%hc_idx.load35" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%hc_idx.load35", 1
  br i1 %cmpeqtmp36, label %then_254, label %merge_255

merge_255:                                        ; preds = %then_254, %merge_253
  %"%hc_idx.load37" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp38 = icmp eq i64 %"%hc_idx.load37", 2
  br i1 %cmpeqtmp38, label %then_256, label %merge_257

merge_257:                                        ; preds = %then_256, %merge_255
  %"%hc_idx.load39" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp40 = icmp eq i64 %"%hc_idx.load39", 3
  br i1 %cmpeqtmp40, label %then_258, label %merge_259

merge_259:                                        ; preds = %then_258, %merge_257
  %calltmp41 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%cmd_ring.load42" = load i64, ptr %"%cmd_ring", align 8
  %calltmp43 = call i64 @xhci_serial_hex32(i64 %"%cmd_ring.load42")
  %calltmp44 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %"%evt_ring.load45" = load i64, ptr %"%evt_ring", align 8
  %calltmp46 = call i64 @xhci_serial_hex32(i64 %"%evt_ring.load45")
  %calltmp47 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_250:                                         ; preds = %entry
  %"%XHCI_MAX_SLOTS.load18" = load i64, ptr @XHCI_MAX_SLOTS, align 8
  store i64 %"%XHCI_MAX_SLOTS.load18", ptr %"%max_slots", align 8
  br label %merge_251

then_252:                                         ; preds = %merge_251
  %"%871.load" = load i64, ptr %"%erst", align 8
  store i64 %"%871.load", ptr %"%xhci_erst_0", align 8
  br label %merge_253

then_254:                                         ; preds = %merge_253
  %"%875.load" = load i64, ptr %"%erst", align 8
  store i64 %"%875.load", ptr %"%xhci_erst_1", align 8
  br label %merge_255

then_256:                                         ; preds = %merge_255
  %"%879.load" = load i64, ptr %"%erst", align 8
  store i64 %"%879.load", ptr %"%xhci_erst_2", align 8
  br label %merge_257

then_258:                                         ; preds = %merge_257
  %"%883.load" = load i64, ptr %"%erst", align 8
  store i64 %"%883.load", ptr %"%xhci_erst_3", align 8
  br label %merge_259
}

define i64 @xhci_setup_registers(i64 %hc_idx) {
entry:
  %"%iman" = alloca i64, align 8
  %"%erst" = alloca i64, align 8
  %"%evt_ring" = alloca i64, align 8
  %"%intr0" = alloca i64, align 8
  %"%rt_off" = alloca i64, align 8
  %"%base" = alloca i64, align 8
  %"%max_slots" = alloca i64, align 8
  %"%cmd_ring" = alloca i64, align 8
  %"%dcbaa" = alloca i64, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @xhci_get_dcbaa(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%dcbaa", align 8
  %"%hc_idx.load2" = load i64, ptr %hc_idx1, align 8
  %calltmp3 = call i64 @xhci_get_cmd_ring(i64 %"%hc_idx.load2")
  store i64 %calltmp3, ptr %"%cmd_ring", align 8
  %"%hc_idx.load4" = load i64, ptr %hc_idx1, align 8
  %calltmp5 = call i64 @xhci_get_max_slots(i64 %"%hc_idx.load4")
  store i64 %calltmp5, ptr %"%max_slots", align 8
  %"%max_slots.load" = load i64, ptr %"%max_slots", align 8
  %"%XHCI_MAX_SLOTS.load" = load i64, ptr @XHCI_MAX_SLOTS, align 8
  %cmpgttmp = icmp sgt i64 %"%max_slots.load", %"%XHCI_MAX_SLOTS.load"
  br i1 %cmpgttmp, label %then_260, label %merge_261

merge_261:                                        ; preds = %then_260, %entry
  %"%hc_idx.load7" = load i64, ptr %hc_idx1, align 8
  %"%XHCI_OP_CONFIG.load" = load i64, ptr @XHCI_OP_CONFIG, align 8
  %"%max_slots.load8" = load i64, ptr %"%max_slots", align 8
  %calltmp9 = call i64 @xhci_write_op(i64 %"%hc_idx.load7", i64 %"%XHCI_OP_CONFIG.load", i64 %"%max_slots.load8")
  %"%hc_idx.load10" = load i64, ptr %hc_idx1, align 8
  %"%XHCI_OP_DCBAAP.load" = load i64, ptr @XHCI_OP_DCBAAP, align 8
  %"%dcbaa.load" = load i64, ptr %"%dcbaa", align 8
  %calltmp11 = call i64 @xhci_write_op(i64 %"%hc_idx.load10", i64 %"%XHCI_OP_DCBAAP.load", i64 %"%dcbaa.load")
  %"%XHCI_OP_DCBAAP.load12" = load i64, ptr @XHCI_OP_DCBAAP, align 8
  %addtmp = add i64 %"%XHCI_OP_DCBAAP.load12", 4
  %"%hc_idx.load13" = load i64, ptr %hc_idx1, align 8
  %calltmp14 = call i64 @xhci_write_op(i64 %"%hc_idx.load13", i64 %addtmp, i64 0)
  %"%cmd_ring.load" = load i64, ptr %"%cmd_ring", align 8
  %addtmp15 = add i64 %"%cmd_ring.load", 1
  %"%hc_idx.load16" = load i64, ptr %hc_idx1, align 8
  %"%XHCI_OP_CRCR.load" = load i64, ptr @XHCI_OP_CRCR, align 8
  %calltmp17 = call i64 @xhci_write_op(i64 %"%hc_idx.load16", i64 %"%XHCI_OP_CRCR.load", i64 %addtmp15)
  %"%XHCI_OP_CRCR.load18" = load i64, ptr @XHCI_OP_CRCR, align 8
  %addtmp19 = add i64 %"%XHCI_OP_CRCR.load18", 4
  %"%hc_idx.load20" = load i64, ptr %hc_idx1, align 8
  %calltmp21 = call i64 @xhci_write_op(i64 %"%hc_idx.load20", i64 %addtmp19, i64 0)
  %"%hc_idx.load22" = load i64, ptr %hc_idx1, align 8
  %calltmp23 = call i64 @xhci_get_base(i64 %"%hc_idx.load22")
  store i64 %calltmp23, ptr %"%base", align 8
  %"%hc_idx.load24" = load i64, ptr %hc_idx1, align 8
  %calltmp25 = call i64 @xhci_get_rt_off(i64 %"%hc_idx.load24")
  store i64 %calltmp25, ptr %"%rt_off", align 8
  %"%base.load" = load i64, ptr %"%base", align 8
  %"%rt_off.load" = load i64, ptr %"%rt_off", align 8
  %addtmp26 = add i64 %"%base.load", %"%rt_off.load"
  %addtmp27 = add i64 %addtmp26, 32
  store i64 %addtmp27, ptr %"%intr0", align 8
  %"%intr0.load" = load i64, ptr %"%intr0", align 8
  %addtmp28 = add i64 %"%intr0.load", 8
  call void @sad_ll_mem_write32(i64 %addtmp28, i64 1)
  %"%hc_idx.load29" = load i64, ptr %hc_idx1, align 8
  %calltmp30 = call i64 @xhci_get_evt_ring(i64 %"%hc_idx.load29")
  store i64 %calltmp30, ptr %"%evt_ring", align 8
  %"%intr0.load31" = load i64, ptr %"%intr0", align 8
  %addtmp32 = add i64 %"%intr0.load31", 24
  %"%evt_ring.load" = load i64, ptr %"%evt_ring", align 8
  call void @sad_ll_mem_write32(i64 %addtmp32, i64 %"%evt_ring.load")
  %"%intr0.load33" = load i64, ptr %"%intr0", align 8
  %addtmp34 = add i64 %"%intr0.load33", 28
  call void @sad_ll_mem_write32(i64 %addtmp34, i64 0)
  store i64 0, ptr %"%erst", align 8
  %"%hc_idx.load35" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%hc_idx.load35", 0
  br i1 %cmpeqtmp, label %then_262, label %merge_263

merge_263:                                        ; preds = %then_262, %merge_261
  %"%hc_idx.load36" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp37 = icmp eq i64 %"%hc_idx.load36", 1
  br i1 %cmpeqtmp37, label %then_264, label %merge_265

merge_265:                                        ; preds = %then_264, %merge_263
  %"%hc_idx.load38" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%hc_idx.load38", 2
  br i1 %cmpeqtmp39, label %then_266, label %merge_267

merge_267:                                        ; preds = %then_266, %merge_265
  %"%hc_idx.load40" = load i64, ptr %hc_idx1, align 8
  %cmpeqtmp41 = icmp eq i64 %"%hc_idx.load40", 3
  br i1 %cmpeqtmp41, label %then_268, label %merge_269

merge_269:                                        ; preds = %then_268, %merge_267
  %"%intr0.load42" = load i64, ptr %"%intr0", align 8
  %addtmp43 = add i64 %"%intr0.load42", 16
  %"%erst.load" = load i64, ptr %"%erst", align 8
  call void @sad_ll_mem_write32(i64 %addtmp43, i64 %"%erst.load")
  %"%intr0.load44" = load i64, ptr %"%intr0", align 8
  %addtmp45 = add i64 %"%intr0.load44", 20
  call void @sad_ll_mem_write32(i64 %addtmp45, i64 0)
  %"%intr0.load46" = load i64, ptr %"%intr0", align 8
  call void @sad_ll_mem_read32(i64 %"%intr0.load46")
  store i64 0, ptr %"%iman", align 8
  %"%iman.load" = load i64, ptr %"%iman", align 8
  %addtmp47 = add i64 %"%iman.load", 2
  %"%intr0.load48" = load i64, ptr %"%intr0", align 8
  call void @sad_ll_mem_write32(i64 %"%intr0.load48", i64 %addtmp47)
  %calltmp49 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  %calltmp50 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_260:                                         ; preds = %entry
  %"%XHCI_MAX_SLOTS.load6" = load i64, ptr @XHCI_MAX_SLOTS, align 8
  store i64 %"%XHCI_MAX_SLOTS.load6", ptr %"%max_slots", align 8
  br label %merge_261

then_262:                                         ; preds = %merge_261
  %"%944.load" = load i64, ptr @xhci_erst_0, align 8
  store i64 %"%944.load", ptr %"%erst", align 8
  br label %merge_263

then_264:                                         ; preds = %merge_263
  %"%948.load" = load i64, ptr @xhci_erst_1, align 8
  store i64 %"%948.load", ptr %"%erst", align 8
  br label %merge_265

then_266:                                         ; preds = %merge_265
  %"%952.load" = load i64, ptr @xhci_erst_2, align 8
  store i64 %"%952.load", ptr %"%erst", align 8
  br label %merge_267

then_268:                                         ; preds = %merge_267
  %"%956.load" = load i64, ptr @xhci_erst_3, align 8
  store i64 %"%956.load", ptr %"%erst", align 8
  br label %merge_269
}

define i64 @xhci_start(i64 %hc_idx) {
entry:
  %"%sts" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  %"%cmd" = alloca i64, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %"%XHCI_OP_USBCMD.load" = load i64, ptr @XHCI_OP_USBCMD, align 8
  %calltmp = call i64 @xhci_read_op(i64 %"%hc_idx.load", i64 %"%XHCI_OP_USBCMD.load")
  store i64 %calltmp, ptr %"%cmd", align 8
  %"%cmd.load" = load i64, ptr %"%cmd", align 8
  %"%XHCI_CMD_RUN.load" = load i64, ptr @XHCI_CMD_RUN, align 8
  %addtmp = add i64 %"%cmd.load", %"%XHCI_CMD_RUN.load"
  %"%XHCI_CMD_INTE.load" = load i64, ptr @XHCI_CMD_INTE, align 8
  %addtmp2 = add i64 %addtmp, %"%XHCI_CMD_INTE.load"
  store i64 %addtmp2, ptr %"%cmd", align 8
  %"%hc_idx.load3" = load i64, ptr %hc_idx1, align 8
  %"%XHCI_OP_USBCMD.load4" = load i64, ptr @XHCI_OP_USBCMD, align 8
  %"%cmd.load5" = load i64, ptr %"%cmd", align 8
  %calltmp6 = call i64 @xhci_write_op(i64 %"%hc_idx.load3", i64 %"%XHCI_OP_USBCMD.load4", i64 %"%cmd.load5")
  store i64 1000, ptr %"%timeout", align 8
  br label %while_cond_270

merge_274:                                        ; preds = %while_body_271
  %"%timeout.load13" = load i64, ptr %"%timeout", align 8
  %subtmp = sub i64 %"%timeout.load13", 1
  store i64 %subtmp, ptr %"%timeout", align 8
  br label %while_cond_270

then_273:                                         ; preds = %while_body_271
  %calltmp11 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %calltmp12 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

while_body_271:                                   ; preds = %while_cond_270
  %"%hc_idx.load7" = load i64, ptr %hc_idx1, align 8
  %"%XHCI_OP_USBSTS.load" = load i64, ptr @XHCI_OP_USBSTS, align 8
  %calltmp8 = call i64 @xhci_read_op(i64 %"%hc_idx.load7", i64 %"%XHCI_OP_USBSTS.load")
  store i64 %calltmp8, ptr %"%sts", align 8
  %"%sts.load" = load i64, ptr %"%sts", align 8
  %andtmp = and i64 %"%sts.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_273, label %merge_274

while_cond_270:                                   ; preds = %merge_274, %entry
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp, label %while_body_271, label %while_exit_272

while_exit_272:                                   ; preds = %while_cond_270
  %calltmp9 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %calltmp10 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -1
}

define i64 @xhci_send_command(i64 %hc_idx, i64 %dw0, i64 %dw1, i64 %dw2, i64 %trb_type, i64 %flags) {
entry:
  %"%xhci_commands_sent" = alloca i64, align 8
  %"%control" = alloca i64, align 8
  %"%trb_addr" = alloca i64, align 8
  %"%cycle" = alloca i64, align 8
  %"%enq" = alloca i64, align 8
  %"%cmd_ring" = alloca i64, align 8
  %flags6 = alloca i64, align 8
  store i64 %flags, ptr %flags6, align 8
  %trb_type5 = alloca i64, align 8
  store i64 %trb_type, ptr %trb_type5, align 8
  %dw24 = alloca i64, align 8
  store i64 %dw2, ptr %dw24, align 8
  %dw13 = alloca i64, align 8
  store i64 %dw1, ptr %dw13, align 8
  %dw02 = alloca i64, align 8
  store i64 %dw0, ptr %dw02, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @xhci_get_cmd_ring(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%cmd_ring", align 8
  %"%hc_idx.load7" = load i64, ptr %hc_idx1, align 8
  %calltmp8 = call i64 @xhci_get_cmd_enq(i64 %"%hc_idx.load7")
  store i64 %calltmp8, ptr %"%enq", align 8
  %"%hc_idx.load9" = load i64, ptr %hc_idx1, align 8
  %calltmp10 = call i64 @xhci_get_cmd_cycle(i64 %"%hc_idx.load9")
  store i64 %calltmp10, ptr %"%cycle", align 8
  %"%enq.load" = load i64, ptr %"%enq", align 8
  %cmpgetmp = icmp sge i64 %"%enq.load", 63
  br i1 %cmpgetmp, label %then_275, label %merge_276

merge_276:                                        ; preds = %then_275, %entry
  %"%enq.load14" = load i64, ptr %"%enq", align 8
  %shltmp = shl i64 %"%enq.load14", 4
  %"%cmd_ring.load" = load i64, ptr %"%cmd_ring", align 8
  %addtmp = add i64 %"%cmd_ring.load", %shltmp
  store i64 %addtmp, ptr %"%trb_addr", align 8
  %"%trb_type.load" = load i64, ptr %trb_type5, align 8
  %"%cycle.load15" = load i64, ptr %"%cycle", align 8
  %"%flags.load" = load i64, ptr %flags6, align 8
  %calltmp16 = call i64 @xhci_make_trb_control(i64 %"%trb_type.load", i64 %"%cycle.load15", i64 %"%flags.load")
  store i64 %calltmp16, ptr %"%control", align 8
  %"%trb_addr.load" = load i64, ptr %"%trb_addr", align 8
  %"%dw0.load" = load i64, ptr %dw02, align 8
  %"%dw1.load" = load i64, ptr %dw13, align 8
  %"%dw2.load" = load i64, ptr %dw24, align 8
  %"%control.load" = load i64, ptr %"%control", align 8
  %calltmp17 = call i64 @xhci_write_trb(i64 %"%trb_addr.load", i64 %"%dw0.load", i64 %"%dw1.load", i64 %"%dw2.load", i64 %"%control.load")
  %"%enq.load18" = load i64, ptr %"%enq", align 8
  %addtmp19 = add i64 %"%enq.load18", 1
  %"%hc_idx.load20" = load i64, ptr %hc_idx1, align 8
  %calltmp21 = call i64 @xhci_set_cmd_enq(i64 %"%hc_idx.load20", i64 %addtmp19)
  %"%xhci_commands_sent.load" = load i64, ptr @xhci_commands_sent, align 8
  %addtmp22 = add i64 %"%xhci_commands_sent.load", 1
  store i64 %addtmp22, ptr %"%xhci_commands_sent", align 8
  %"%hc_idx.load23" = load i64, ptr %hc_idx1, align 8
  %calltmp24 = call i64 @xhci_ring_doorbell(i64 %"%hc_idx.load23", i64 0, i64 0)
  ret i64 0

then_275:                                         ; preds = %entry
  store i64 0, ptr %"%enq", align 8
  %"%cycle.load" = load i64, ptr %"%cycle", align 8
  %subtmp = sub i64 1, %"%cycle.load"
  store i64 %subtmp, ptr %"%cycle", align 8
  %"%hc_idx.load11" = load i64, ptr %hc_idx1, align 8
  %"%cycle.load12" = load i64, ptr %"%cycle", align 8
  %calltmp13 = call i64 @xhci_set_cmd_cycle(i64 %"%hc_idx.load11", i64 %"%cycle.load12")
  br label %merge_276
}

define i64 @xhci_wait_command_completion(i64 %hc_idx) {
entry:
  %"%slot_id" = alloca i64, align 8
  %"%xhci_events_received" = alloca i64, align 8
  %"%cc" = alloca i64, align 8
  %"%dw2" = alloca i64, align 8
  %"%trb_t" = alloca i64, align 8
  %"%xhci_errors" = alloca i64, align 8
  %"%evt_cycle" = alloca i64, align 8
  %"%dw3" = alloca i64, align 8
  %"%trb_addr" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  %"%deq" = alloca i64, align 8
  %"%evt_ring" = alloca i64, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @xhci_get_evt_ring(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%evt_ring", align 8
  %"%hc_idx.load2" = load i64, ptr %hc_idx1, align 8
  %calltmp3 = call i64 @xhci_get_evt_deq(i64 %"%hc_idx.load2")
  store i64 %calltmp3, ptr %"%deq", align 8
  store i64 5000, ptr %"%timeout", align 8
  br label %while_cond_277

else_286:                                         ; preds = %then_282
  %calltmp22 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %"%1080.load" = load i64, ptr %"%cc", align 8
  %calltmp23 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1080.load")
  %calltmp24 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%xhci_errors.load25" = load i64, ptr %"%xhci_errors", align 8
  %addtmp26 = add i64 %"%xhci_errors.load25", 1
  store i64 %addtmp26, ptr %"%xhci_errors", align 8
  %"%cc.load27" = load i64, ptr %"%cc", align 8
  %subtmp28 = sub i64 0, %"%cc.load27"
  ret i64 %subtmp28

merge_281:                                        ; preds = %merge_283, %while_body_278
  %"%timeout.load19" = load i64, ptr %"%timeout", align 8
  %subtmp = sub i64 %"%timeout.load19", 1
  store i64 %subtmp, ptr %"%timeout", align 8
  br label %while_cond_277

merge_283:                                        ; preds = %then_280
  br label %merge_281

then_280:                                         ; preds = %while_body_278
  %"%dw3.load8" = load i64, ptr %"%dw3", align 8
  %calltmp9 = call i64 @xhci_trb_type(i64 %"%dw3.load8")
  store i64 %calltmp9, ptr %"%trb_t", align 8
  %"%trb_addr.load10" = load i64, ptr %"%trb_addr", align 8
  %calltmp11 = call i64 @xhci_read_trb_dw2(i64 %"%trb_addr.load10")
  store i64 %calltmp11, ptr %"%dw2", align 8
  %"%dw2.load" = load i64, ptr %"%dw2", align 8
  %calltmp12 = call i64 @xhci_trb_completion_code(i64 %"%dw2.load")
  store i64 %calltmp12, ptr %"%cc", align 8
  %"%xhci_events_received.load" = load i64, ptr @xhci_events_received, align 8
  %addtmp13 = add i64 %"%xhci_events_received.load", 1
  store i64 %addtmp13, ptr %"%xhci_events_received", align 8
  %"%deq.load14" = load i64, ptr %"%deq", align 8
  %addtmp15 = add i64 %"%deq.load14", 1
  %"%XHCI_MAX_TRB_RING.load" = load i64, ptr @XHCI_MAX_TRB_RING, align 8
  %modtmp = srem i64 %addtmp15, %"%XHCI_MAX_TRB_RING.load"
  %"%hc_idx.load16" = load i64, ptr %hc_idx1, align 8
  %calltmp17 = call i64 @xhci_set_evt_deq(i64 %"%hc_idx.load16", i64 %modtmp)
  %"%trb_t.load" = load i64, ptr %"%trb_t", align 8
  %"%TRB_COMMAND_COMPLETE.load" = load i64, ptr @TRB_COMMAND_COMPLETE, align 8
  %cmpeqtmp18 = icmp eq i64 %"%trb_t.load", %"%TRB_COMMAND_COMPLETE.load"
  br i1 %cmpeqtmp18, label %then_282, label %merge_283

then_282:                                         ; preds = %then_280
  %"%cc.load" = load i64, ptr %"%cc", align 8
  %"%TRB_CC_SUCCESS.load" = load i64, ptr @TRB_CC_SUCCESS, align 8
  %cmpeqtmp20 = icmp eq i64 %"%cc.load", %"%TRB_CC_SUCCESS.load"
  br i1 %cmpeqtmp20, label %then_284, label %else_286

then_284:                                         ; preds = %then_282
  %"%dw3.load21" = load i64, ptr %"%dw3", align 8
  %shrtmp = lshr i64 %"%dw3.load21", 24
  store i64 %shrtmp, ptr %"%slot_id", align 8
  %loadtmp = load i64, ptr %"%slot_id", align 8
  ret i64 %loadtmp

while_body_278:                                   ; preds = %while_cond_277
  %"%deq.load" = load i64, ptr %"%deq", align 8
  %shltmp = shl i64 %"%deq.load", 4
  %"%evt_ring.load" = load i64, ptr %"%evt_ring", align 8
  %addtmp = add i64 %"%evt_ring.load", %shltmp
  store i64 %addtmp, ptr %"%trb_addr", align 8
  %"%trb_addr.load" = load i64, ptr %"%trb_addr", align 8
  %calltmp4 = call i64 @xhci_read_trb_dw3(i64 %"%trb_addr.load")
  store i64 %calltmp4, ptr %"%dw3", align 8
  %"%dw3.load" = load i64, ptr %"%dw3", align 8
  %andtmp = and i64 %"%dw3.load", 1
  store i64 %andtmp, ptr %"%evt_cycle", align 8
  %"%evt_cycle.load" = load i64, ptr %"%evt_cycle", align 8
  %cmpeqtmp = icmp eq i64 %"%evt_cycle.load", 1
  br i1 %cmpeqtmp, label %then_280, label %merge_281

while_cond_277:                                   ; preds = %merge_281, %entry
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp, label %while_body_278, label %while_exit_279

while_exit_279:                                   ; preds = %while_cond_277
  %calltmp5 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  %calltmp6 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%xhci_errors.load" = load i64, ptr @xhci_errors, align 8
  %addtmp7 = add i64 %"%xhci_errors.load", 1
  store i64 %addtmp7, ptr %"%xhci_errors", align 8
  ret i64 -255
}

define i64 @xhci_enable_slot(i64 %hc_idx) {
entry:
  %"%slot_id" = alloca i64, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %calltmp = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %calltmp2 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %"%TRB_ENABLE_SLOT.load" = load i64, ptr @TRB_ENABLE_SLOT, align 8
  %calltmp3 = call i64 @xhci_send_command(i64 %"%hc_idx.load", i64 0, i64 0, i64 0, i64 %"%TRB_ENABLE_SLOT.load", i64 0)
  %"%hc_idx.load4" = load i64, ptr %hc_idx1, align 8
  %calltmp5 = call i64 @xhci_wait_command_completion(i64 %"%hc_idx.load4")
  store i64 %calltmp5, ptr %"%slot_id", align 8
  %"%slot_id.load" = load i64, ptr %"%slot_id", align 8
  %cmpgttmp = icmp sgt i64 %"%slot_id.load", 0
  br i1 %cmpgttmp, label %then_287, label %merge_288

merge_288:                                        ; preds = %then_287, %entry
  %loadtmp = load i64, ptr %"%slot_id", align 8
  ret i64 %loadtmp

then_287:                                         ; preds = %entry
  %calltmp6 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %"%1115.load" = load i64, ptr %"%slot_id", align 8
  %calltmp7 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1115.load")
  %calltmp8 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_288
}

define i64 @xhci_port_reset(i64 %hc_idx, i64 %port_num) {
entry:
  %"%timeout" = alloca i64, align 8
  %"%preserve" = alloca i64, align 8
  %"%portsc" = alloca i64, align 8
  %port_num2 = alloca i64, align 8
  store i64 %port_num, ptr %port_num2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %calltmp = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %"%1124.load" = load i64, ptr %port_num2, align 8
  %calltmp3 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1124.load")
  %calltmp4 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %"%port_num.load" = load i64, ptr %port_num2, align 8
  %calltmp5 = call i64 @xhci_read_port(i64 %"%hc_idx.load", i64 %"%port_num.load")
  store i64 %calltmp5, ptr %"%portsc", align 8
  %"%portsc.load" = load i64, ptr %"%portsc", align 8
  %andtmp = and i64 %"%portsc.load", 1023
  store i64 %andtmp, ptr %"%preserve", align 8
  %"%preserve.load" = load i64, ptr %"%preserve", align 8
  %andtmp6 = and i64 %"%preserve.load", 31
  store i64 %andtmp6, ptr %"%preserve", align 8
  %"%preserve.load7" = load i64, ptr %"%preserve", align 8
  %shrtmp = lshr i64 %"%preserve.load7", 2
  %shltmp = shl i64 %shrtmp, 2
  store i64 %shltmp, ptr %"%preserve", align 8
  %"%PORTSC_PR.load" = load i64, ptr @PORTSC_PR, align 8
  %"%PORTSC_PP.load" = load i64, ptr @PORTSC_PP, align 8
  %addtmp = add i64 %"%PORTSC_PR.load", %"%PORTSC_PP.load"
  %"%hc_idx.load8" = load i64, ptr %hc_idx1, align 8
  %"%port_num.load9" = load i64, ptr %port_num2, align 8
  %calltmp10 = call i64 @xhci_write_port(i64 %"%hc_idx.load8", i64 %"%port_num.load9", i64 %addtmp)
  store i64 2000, ptr %"%timeout", align 8
  br label %while_cond_289

merge_293:                                        ; preds = %while_body_290
  %"%timeout.load28" = load i64, ptr %"%timeout", align 8
  %subtmp = sub i64 %"%timeout.load28", 1
  store i64 %subtmp, ptr %"%timeout", align 8
  br label %while_cond_289

then_292:                                         ; preds = %while_body_290
  %"%portsc.load18" = load i64, ptr %"%portsc", align 8
  %"%PORTSC_PRC.load19" = load i64, ptr @PORTSC_PRC, align 8
  %addtmp20 = add i64 %"%portsc.load18", %"%PORTSC_PRC.load19"
  %"%hc_idx.load21" = load i64, ptr %hc_idx1, align 8
  %"%port_num.load22" = load i64, ptr %port_num2, align 8
  %calltmp23 = call i64 @xhci_write_port(i64 %"%hc_idx.load21", i64 %"%port_num.load22", i64 %addtmp20)
  %calltmp24 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%1158.load" = load i64, ptr %port_num2, align 8
  %calltmp25 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1158.load")
  %calltmp26 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  %calltmp27 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

while_body_290:                                   ; preds = %while_cond_289
  %"%hc_idx.load11" = load i64, ptr %hc_idx1, align 8
  %"%port_num.load12" = load i64, ptr %port_num2, align 8
  %calltmp13 = call i64 @xhci_read_port(i64 %"%hc_idx.load11", i64 %"%port_num.load12")
  store i64 %calltmp13, ptr %"%portsc", align 8
  %"%portsc.load14" = load i64, ptr %"%portsc", align 8
  %"%PORTSC_PRC.load" = load i64, ptr @PORTSC_PRC, align 8
  %divtmp = sdiv i64 %"%portsc.load14", %"%PORTSC_PRC.load"
  %andtmp15 = and i64 %divtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp15, 1
  br i1 %cmpeqtmp, label %then_292, label %merge_293

while_cond_289:                                   ; preds = %merge_293, %entry
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp, label %while_body_290, label %while_exit_291

while_exit_291:                                   ; preds = %while_cond_289
  %calltmp16 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  %calltmp17 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -1
}

define i64 @xhci_port_speed(i64 %hc_idx, i64 %port_num) {
entry:
  %"%speed" = alloca i64, align 8
  %"%portsc" = alloca i64, align 8
  %port_num2 = alloca i64, align 8
  store i64 %port_num, ptr %port_num2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %"%port_num.load" = load i64, ptr %port_num2, align 8
  %calltmp = call i64 @xhci_read_port(i64 %"%hc_idx.load", i64 %"%port_num.load")
  store i64 %calltmp, ptr %"%portsc", align 8
  %"%portsc.load" = load i64, ptr %"%portsc", align 8
  %shrtmp = lshr i64 %"%portsc.load", 10
  %andtmp = and i64 %shrtmp, 15
  store i64 %andtmp, ptr %"%speed", align 8
  %loadtmp = load i64, ptr %"%speed", align 8
  ret i64 %loadtmp
}

define i64 @xhci_speed_name(i64 %speed) {
entry:
  %speed1 = alloca i64, align 8
  store i64 %speed, ptr %speed1, align 8
  %"%speed.load" = load i64, ptr %speed1, align 8
  %"%XHCI_SPEED_FULL.load" = load i64, ptr @XHCI_SPEED_FULL, align 8
  %cmpeqtmp = icmp eq i64 %"%speed.load", %"%XHCI_SPEED_FULL.load"
  br i1 %cmpeqtmp, label %then_294, label %else_296

else_296:                                         ; preds = %entry
  %"%speed.load2" = load i64, ptr %speed1, align 8
  %"%XHCI_SPEED_LOW.load" = load i64, ptr @XHCI_SPEED_LOW, align 8
  %cmpeqtmp3 = icmp eq i64 %"%speed.load2", %"%XHCI_SPEED_LOW.load"
  br i1 %cmpeqtmp3, label %then_297, label %else_299

else_299:                                         ; preds = %else_296
  %"%speed.load5" = load i64, ptr %speed1, align 8
  %"%XHCI_SPEED_HIGH.load" = load i64, ptr @XHCI_SPEED_HIGH, align 8
  %cmpeqtmp6 = icmp eq i64 %"%speed.load5", %"%XHCI_SPEED_HIGH.load"
  br i1 %cmpeqtmp6, label %then_300, label %else_302

else_302:                                         ; preds = %else_299
  %"%speed.load8" = load i64, ptr %speed1, align 8
  %"%XHCI_SPEED_SUPER.load" = load i64, ptr @XHCI_SPEED_SUPER, align 8
  %cmpeqtmp9 = icmp eq i64 %"%speed.load8", %"%XHCI_SPEED_SUPER.load"
  br i1 %cmpeqtmp9, label %then_303, label %else_305

else_305:                                         ; preds = %else_302
  %calltmp11 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  br label %merge_304

merge_295:                                        ; preds = %merge_298, %then_294
  ret i64 0

merge_298:                                        ; preds = %merge_301, %then_297
  br label %merge_295

merge_301:                                        ; preds = %merge_304, %then_300
  br label %merge_298

merge_304:                                        ; preds = %else_305, %then_303
  br label %merge_301

then_294:                                         ; preds = %entry
  %calltmp = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  br label %merge_295

then_297:                                         ; preds = %else_296
  %calltmp4 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  br label %merge_298

then_300:                                         ; preds = %else_299
  %calltmp7 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  br label %merge_301

then_303:                                         ; preds = %else_302
  %calltmp10 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  br label %merge_304
}

define i64 @xhci_scan_ports(i64 %hc_idx) {
entry:
  %"%xhci_devices_found" = alloca i64, align 8
  %"%dev_idx" = alloca i64, align 8
  %"%usb_speed" = alloca i64, align 8
  %"%speed" = alloca i64, align 8
  %"%portsc" = alloca i64, align 8
  %"%p" = alloca i64, align 8
  %"%found" = alloca i64, align 8
  %"%num_ports" = alloca i64, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp = call i64 @xhci_get_num_ports(i64 %"%hc_idx.load")
  store i64 %calltmp, ptr %"%num_ports", align 8
  store i64 0, ptr %"%found", align 8
  %calltmp2 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  %"%1207.load" = load i64, ptr %"%num_ports", align 8
  %calltmp3 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1207.load")
  %calltmp4 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  %calltmp5 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%p", align 8
  br label %while_cond_306

merge_310:                                        ; preds = %merge_320, %while_body_307
  %"%p.load26" = load i64, ptr %"%p", align 8
  %addtmp = add i64 %"%p.load26", 1
  store i64 %addtmp, ptr %"%p", align 8
  br label %while_cond_306

merge_312:                                        ; preds = %then_311, %then_309
  %"%speed.load27" = load i64, ptr %"%speed", align 8
  %"%XHCI_SPEED_HIGH.load" = load i64, ptr @XHCI_SPEED_HIGH, align 8
  %cmpeqtmp28 = icmp eq i64 %"%speed.load27", %"%XHCI_SPEED_HIGH.load"
  br i1 %cmpeqtmp28, label %then_313, label %merge_314

merge_314:                                        ; preds = %then_313, %merge_312
  %"%speed.load29" = load i64, ptr %"%speed", align 8
  %"%XHCI_SPEED_FULL.load" = load i64, ptr @XHCI_SPEED_FULL, align 8
  %cmpeqtmp30 = icmp eq i64 %"%speed.load29", %"%XHCI_SPEED_FULL.load"
  br i1 %cmpeqtmp30, label %then_315, label %merge_316

merge_316:                                        ; preds = %then_315, %merge_314
  %"%speed.load31" = load i64, ptr %"%speed", align 8
  %"%XHCI_SPEED_LOW.load" = load i64, ptr @XHCI_SPEED_LOW, align 8
  %cmpeqtmp32 = icmp eq i64 %"%speed.load31", %"%XHCI_SPEED_LOW.load"
  br i1 %cmpeqtmp32, label %then_317, label %merge_318

merge_318:                                        ; preds = %then_317, %merge_316
  %"%hc_idx.load33" = load i64, ptr %hc_idx1, align 8
  %addtmp34 = add i64 %"%hc_idx.load33", 10
  %"%p.load35" = load i64, ptr %"%p", align 8
  %"%usb_speed.load" = load i64, ptr %"%usb_speed", align 8
  call void @usb_register_device(i64 %addtmp34, i64 %"%p.load35", i64 %"%usb_speed.load", i64 0, i64 0, i64 0, i64 0)
  store i64 0, ptr %"%dev_idx", align 8
  %"%dev_idx.load" = load i64, ptr %"%dev_idx", align 8
  %cmpgetmp = icmp sge i64 %"%dev_idx.load", 0
  br i1 %cmpgetmp, label %then_319, label %merge_320

merge_320:                                        ; preds = %then_319, %merge_318
  br label %merge_310

then_309:                                         ; preds = %while_body_307
  %"%hc_idx.load13" = load i64, ptr %hc_idx1, align 8
  %"%p.load14" = load i64, ptr %"%p", align 8
  %calltmp15 = call i64 @xhci_port_speed(i64 %"%hc_idx.load13", i64 %"%p.load14")
  store i64 %calltmp15, ptr %"%speed", align 8
  %calltmp16 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%1229.load" = load i64, ptr %"%p", align 8
  %calltmp17 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1229.load")
  %calltmp18 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  %"%speed.load" = load i64, ptr %"%speed", align 8
  %calltmp19 = call i64 @xhci_speed_name(i64 %"%speed.load")
  %calltmp20 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%hc_idx.load21" = load i64, ptr %hc_idx1, align 8
  %"%p.load22" = load i64, ptr %"%p", align 8
  %calltmp23 = call i64 @xhci_port_reset(i64 %"%hc_idx.load21", i64 %"%p.load22")
  %"%1237.load" = load i64, ptr %"%speed", align 8
  store i64 %"%1237.load", ptr %"%usb_speed", align 8
  %"%speed.load24" = load i64, ptr %"%speed", align 8
  %"%XHCI_SPEED_SUPER.load" = load i64, ptr @XHCI_SPEED_SUPER, align 8
  %cmpeqtmp25 = icmp eq i64 %"%speed.load24", %"%XHCI_SPEED_SUPER.load"
  br i1 %cmpeqtmp25, label %then_311, label %merge_312

then_311:                                         ; preds = %then_309
  store i64 3, ptr %"%usb_speed", align 8
  br label %merge_312

then_313:                                         ; preds = %merge_312
  store i64 2, ptr %"%usb_speed", align 8
  br label %merge_314

then_315:                                         ; preds = %merge_314
  store i64 1, ptr %"%usb_speed", align 8
  br label %merge_316

then_317:                                         ; preds = %merge_316
  store i64 0, ptr %"%usb_speed", align 8
  br label %merge_318

then_319:                                         ; preds = %merge_318
  %"%found.load" = load i64, ptr %"%found", align 8
  %addtmp36 = add i64 %"%found.load", 1
  store i64 %addtmp36, ptr %"%found", align 8
  %"%xhci_devices_found.load" = load i64, ptr @xhci_devices_found, align 8
  %addtmp37 = add i64 %"%xhci_devices_found.load", 1
  store i64 %addtmp37, ptr %"%xhci_devices_found", align 8
  br label %merge_320

while_body_307:                                   ; preds = %while_cond_306
  %"%hc_idx.load6" = load i64, ptr %hc_idx1, align 8
  %"%p.load7" = load i64, ptr %"%p", align 8
  %calltmp8 = call i64 @xhci_read_port(i64 %"%hc_idx.load6", i64 %"%p.load7")
  store i64 %calltmp8, ptr %"%portsc", align 8
  %"%portsc.load" = load i64, ptr %"%portsc", align 8
  %andtmp = and i64 %"%portsc.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_309, label %merge_310

while_cond_306:                                   ; preds = %merge_310, %entry
  %"%p.load" = load i64, ptr %"%p", align 8
  %"%num_ports.load" = load i64, ptr %"%num_ports", align 8
  %cmplttmp = icmp slt i64 %"%p.load", %"%num_ports.load"
  br i1 %cmplttmp, label %while_body_307, label %while_exit_308

while_exit_308:                                   ; preds = %while_cond_306
  %calltmp9 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %"%1266.load" = load i64, ptr %"%found", align 8
  %calltmp10 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1266.load")
  %calltmp11 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  %calltmp12 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr %"%found", align 8
  ret i64 %loadtmp
}

define i64 @xhci_detect_controllers() {
entry:
  %"%xhci_count" = alloca i64, align 8
  %"%xhci_irq_3" = alloca i64, align 8
  %"%xhci_irq_2" = alloca i64, align 8
  %"%xhci_irq_1" = alloca i64, align 8
  %"%xhci_irq_0" = alloca i64, align 8
  %"%xhci_pci_idx_3" = alloca i64, align 8
  %"%xhci_pci_idx_2" = alloca i64, align 8
  %"%xhci_pci_idx_1" = alloca i64, align 8
  %"%xhci_pci_idx_0" = alloca i64, align 8
  %"%rtsoff" = alloca i64, align 8
  %"%dboff" = alloca i64, align 8
  %"%max_slots" = alloca i64, align 8
  %"%num_ports" = alloca i64, align 8
  %"%hcsparams1" = alloca i64, align 8
  %"%xhci_cap_len_3" = alloca i64, align 8
  %"%xhci_cap_len_2" = alloca i64, align 8
  %"%xhci_cap_len_1" = alloca i64, align 8
  %"%xhci_cap_len_0" = alloca i64, align 8
  %"%cap_len" = alloca i64, align 8
  %"%idx" = alloca i64, align 8
  %"%base" = alloca i64, align 8
  %"%irq" = alloca i64, align 8
  %"%bar0" = alloca i64, align 8
  %"%usb_type" = alloca i64, align 8
  %"%pci_idx" = alloca i64, align 8
  %"%start" = alloca i64, align 8
  %calltmp = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  %calltmp1 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%start", align 8
  br label %while_cond_321

merge_325:                                        ; preds = %then_324, %while_body_322
  %"%pci_idx.load6" = load i64, ptr %"%pci_idx", align 8
  call void @pci_get_usb_type(i64 %"%pci_idx.load6")
  store i64 0, ptr %"%usb_type", align 8
  %"%usb_type.load" = load i64, ptr %"%usb_type", align 8
  %cmpeqtmp = icmp eq i64 %"%usb_type.load", 48
  br i1 %cmpeqtmp, label %then_326, label %merge_327

merge_327:                                        ; preds = %merge_351, %merge_325
  %"%pci_idx.load20" = load i64, ptr %"%pci_idx", align 8
  %addtmp21 = add i64 %"%pci_idx.load20", 1
  store i64 %addtmp21, ptr %"%start", align 8
  br label %while_cond_321

merge_329:                                        ; preds = %then_328, %then_326
  %"%idx.load22" = load i64, ptr %"%idx", align 8
  %cmpeqtmp23 = icmp eq i64 %"%idx.load22", 1
  br i1 %cmpeqtmp23, label %then_330, label %merge_331

merge_331:                                        ; preds = %then_330, %merge_329
  %"%idx.load24" = load i64, ptr %"%idx", align 8
  %cmpeqtmp25 = icmp eq i64 %"%idx.load24", 2
  br i1 %cmpeqtmp25, label %then_332, label %merge_333

merge_333:                                        ; preds = %then_332, %merge_331
  %"%idx.load26" = load i64, ptr %"%idx", align 8
  %cmpeqtmp27 = icmp eq i64 %"%idx.load26", 3
  br i1 %cmpeqtmp27, label %then_334, label %merge_335

merge_335:                                        ; preds = %then_334, %merge_333
  %"%base.load28" = load i64, ptr %"%base", align 8
  %"%cap_len.load" = load i64, ptr %"%cap_len", align 8
  %addtmp29 = add i64 %"%base.load28", %"%cap_len.load"
  %"%idx.load30" = load i64, ptr %"%idx", align 8
  %calltmp31 = call i64 @xhci_set_op_base(i64 %"%idx.load30", i64 %addtmp29)
  %"%base.load32" = load i64, ptr %"%base", align 8
  %"%XHCI_CAP_HCSPARAMS1.load" = load i64, ptr @XHCI_CAP_HCSPARAMS1, align 8
  %addtmp33 = add i64 %"%base.load32", %"%XHCI_CAP_HCSPARAMS1.load"
  call void @sad_ll_mem_read32(i64 %addtmp33)
  store i64 0, ptr %"%hcsparams1", align 8
  %"%hcsparams1.load" = load i64, ptr %"%hcsparams1", align 8
  %shrtmp = lshr i64 %"%hcsparams1.load", 24
  %andtmp = and i64 %shrtmp, 255
  store i64 %andtmp, ptr %"%num_ports", align 8
  %"%hcsparams1.load34" = load i64, ptr %"%hcsparams1", align 8
  %andtmp35 = and i64 %"%hcsparams1.load34", 255
  store i64 %andtmp35, ptr %"%max_slots", align 8
  %"%idx.load36" = load i64, ptr %"%idx", align 8
  %"%num_ports.load" = load i64, ptr %"%num_ports", align 8
  %calltmp37 = call i64 @xhci_set_num_ports(i64 %"%idx.load36", i64 %"%num_ports.load")
  %"%idx.load38" = load i64, ptr %"%idx", align 8
  %"%max_slots.load" = load i64, ptr %"%max_slots", align 8
  %calltmp39 = call i64 @xhci_set_max_slots(i64 %"%idx.load38", i64 %"%max_slots.load")
  %"%base.load40" = load i64, ptr %"%base", align 8
  %"%XHCI_CAP_DBOFF.load" = load i64, ptr @XHCI_CAP_DBOFF, align 8
  %addtmp41 = add i64 %"%base.load40", %"%XHCI_CAP_DBOFF.load"
  call void @sad_ll_mem_read32(i64 %addtmp41)
  store i64 0, ptr %"%dboff", align 8
  %"%base.load42" = load i64, ptr %"%base", align 8
  %"%XHCI_CAP_RTSOFF.load" = load i64, ptr @XHCI_CAP_RTSOFF, align 8
  %addtmp43 = add i64 %"%base.load42", %"%XHCI_CAP_RTSOFF.load"
  call void @sad_ll_mem_read32(i64 %addtmp43)
  store i64 0, ptr %"%rtsoff", align 8
  %"%idx.load44" = load i64, ptr %"%idx", align 8
  %"%dboff.load" = load i64, ptr %"%dboff", align 8
  %calltmp45 = call i64 @xhci_set_db_off(i64 %"%idx.load44", i64 %"%dboff.load")
  %"%idx.load46" = load i64, ptr %"%idx", align 8
  %"%rtsoff.load" = load i64, ptr %"%rtsoff", align 8
  %calltmp47 = call i64 @xhci_set_rt_off(i64 %"%idx.load46", i64 %"%rtsoff.load")
  %"%idx.load48" = load i64, ptr %"%idx", align 8
  %cmpeqtmp49 = icmp eq i64 %"%idx.load48", 0
  br i1 %cmpeqtmp49, label %then_336, label %merge_337

merge_337:                                        ; preds = %then_336, %merge_335
  %"%idx.load50" = load i64, ptr %"%idx", align 8
  %cmpeqtmp51 = icmp eq i64 %"%idx.load50", 1
  br i1 %cmpeqtmp51, label %then_338, label %merge_339

merge_339:                                        ; preds = %then_338, %merge_337
  %"%idx.load52" = load i64, ptr %"%idx", align 8
  %cmpeqtmp53 = icmp eq i64 %"%idx.load52", 2
  br i1 %cmpeqtmp53, label %then_340, label %merge_341

merge_341:                                        ; preds = %then_340, %merge_339
  %"%idx.load54" = load i64, ptr %"%idx", align 8
  %cmpeqtmp55 = icmp eq i64 %"%idx.load54", 3
  br i1 %cmpeqtmp55, label %then_342, label %merge_343

merge_343:                                        ; preds = %then_342, %merge_341
  %"%idx.load56" = load i64, ptr %"%idx", align 8
  %cmpeqtmp57 = icmp eq i64 %"%idx.load56", 0
  br i1 %cmpeqtmp57, label %then_344, label %merge_345

merge_345:                                        ; preds = %then_344, %merge_343
  %"%idx.load58" = load i64, ptr %"%idx", align 8
  %cmpeqtmp59 = icmp eq i64 %"%idx.load58", 1
  br i1 %cmpeqtmp59, label %then_346, label %merge_347

merge_347:                                        ; preds = %then_346, %merge_345
  %"%idx.load60" = load i64, ptr %"%idx", align 8
  %cmpeqtmp61 = icmp eq i64 %"%idx.load60", 2
  br i1 %cmpeqtmp61, label %then_348, label %merge_349

merge_349:                                        ; preds = %then_348, %merge_347
  %"%idx.load62" = load i64, ptr %"%idx", align 8
  %cmpeqtmp63 = icmp eq i64 %"%idx.load62", 3
  br i1 %cmpeqtmp63, label %then_350, label %merge_351

merge_351:                                        ; preds = %then_350, %merge_349
  %"%idx.load64" = load i64, ptr %"%idx", align 8
  %calltmp65 = call i64 @xhci_set_active(i64 %"%idx.load64", i64 1)
  %calltmp66 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.32 to i64))
  %"%1394.load" = load i64, ptr %"%idx", align 8
  %calltmp67 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1394.load")
  %calltmp68 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  %"%1399.load" = load i64, ptr %"%num_ports", align 8
  %calltmp69 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1399.load")
  %calltmp70 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.34 to i64))
  %"%1404.load" = load i64, ptr %"%max_slots", align 8
  %calltmp71 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1404.load")
  %calltmp72 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%xhci_count.load73" = load i64, ptr @xhci_count, align 8
  %addtmp74 = add i64 %"%xhci_count.load73", 1
  store i64 %addtmp74, ptr %"%xhci_count", align 8
  br label %merge_327

then_324:                                         ; preds = %while_body_322
  br label %merge_325

then_326:                                         ; preds = %merge_325
  %"%pci_idx.load7" = load i64, ptr %"%pci_idx", align 8
  call void @pci_get_bar0(i64 %"%pci_idx.load7")
  store i64 0, ptr %"%bar0", align 8
  %"%pci_idx.load8" = load i64, ptr %"%pci_idx", align 8
  call void @pci_get_irq(i64 %"%pci_idx.load8")
  store i64 0, ptr %"%irq", align 8
  %"%bar0.load" = load i64, ptr %"%bar0", align 8
  call void @pci_bar_address(i64 %"%bar0.load")
  store i64 0, ptr %"%base", align 8
  %calltmp9 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.30 to i64))
  %"%1303.load" = load i64, ptr %"%pci_idx", align 8
  %calltmp10 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1303.load")
  %calltmp11 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  %"%base.load" = load i64, ptr %"%base", align 8
  %calltmp12 = call i64 @xhci_serial_hex32(i64 %"%base.load")
  %calltmp13 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%pci_idx.load14" = load i64, ptr %"%pci_idx", align 8
  call void @pci_enable_bus_master(i64 %"%pci_idx.load14")
  %"%1311.load" = load i64, ptr @xhci_count, align 8
  store i64 %"%1311.load", ptr %"%idx", align 8
  %"%idx.load" = load i64, ptr %"%idx", align 8
  %"%base.load15" = load i64, ptr %"%base", align 8
  %calltmp16 = call i64 @xhci_set_base(i64 %"%idx.load", i64 %"%base.load15")
  %"%base.load17" = load i64, ptr %"%base", align 8
  %"%XHCI_CAP_CAPLENGTH.load" = load i64, ptr @XHCI_CAP_CAPLENGTH, align 8
  %addtmp = add i64 %"%base.load17", %"%XHCI_CAP_CAPLENGTH.load"
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%cap_len", align 8
  %"%idx.load18" = load i64, ptr %"%idx", align 8
  %cmpeqtmp19 = icmp eq i64 %"%idx.load18", 0
  br i1 %cmpeqtmp19, label %then_328, label %merge_329

then_328:                                         ; preds = %then_326
  %"%1320.load" = load i64, ptr %"%cap_len", align 8
  store i64 %"%1320.load", ptr %"%xhci_cap_len_0", align 8
  br label %merge_329

then_330:                                         ; preds = %merge_329
  %"%1324.load" = load i64, ptr %"%cap_len", align 8
  store i64 %"%1324.load", ptr %"%xhci_cap_len_1", align 8
  br label %merge_331

then_332:                                         ; preds = %merge_331
  %"%1328.load" = load i64, ptr %"%cap_len", align 8
  store i64 %"%1328.load", ptr %"%xhci_cap_len_2", align 8
  br label %merge_333

then_334:                                         ; preds = %merge_333
  %"%1332.load" = load i64, ptr %"%cap_len", align 8
  store i64 %"%1332.load", ptr %"%xhci_cap_len_3", align 8
  br label %merge_335

then_336:                                         ; preds = %merge_335
  %"%1360.load" = load i64, ptr %"%pci_idx", align 8
  store i64 %"%1360.load", ptr %"%xhci_pci_idx_0", align 8
  br label %merge_337

then_338:                                         ; preds = %merge_337
  %"%1364.load" = load i64, ptr %"%pci_idx", align 8
  store i64 %"%1364.load", ptr %"%xhci_pci_idx_1", align 8
  br label %merge_339

then_340:                                         ; preds = %merge_339
  %"%1368.load" = load i64, ptr %"%pci_idx", align 8
  store i64 %"%1368.load", ptr %"%xhci_pci_idx_2", align 8
  br label %merge_341

then_342:                                         ; preds = %merge_341
  %"%1372.load" = load i64, ptr %"%pci_idx", align 8
  store i64 %"%1372.load", ptr %"%xhci_pci_idx_3", align 8
  br label %merge_343

then_344:                                         ; preds = %merge_343
  %"%1376.load" = load i64, ptr %"%irq", align 8
  store i64 %"%1376.load", ptr %"%xhci_irq_0", align 8
  br label %merge_345

then_346:                                         ; preds = %merge_345
  %"%1380.load" = load i64, ptr %"%irq", align 8
  store i64 %"%1380.load", ptr %"%xhci_irq_1", align 8
  br label %merge_347

then_348:                                         ; preds = %merge_347
  %"%1384.load" = load i64, ptr %"%irq", align 8
  store i64 %"%1384.load", ptr %"%xhci_irq_2", align 8
  br label %merge_349

then_350:                                         ; preds = %merge_349
  %"%1388.load" = load i64, ptr %"%irq", align 8
  store i64 %"%1388.load", ptr %"%xhci_irq_3", align 8
  br label %merge_351

while_body_322:                                   ; preds = %while_cond_321
  %"%start.load" = load i64, ptr %"%start", align 8
  call void @pci_find_usb_controller(i64 %"%start.load")
  store i64 0, ptr %"%pci_idx", align 8
  %"%pci_idx.load" = load i64, ptr %"%pci_idx", align 8
  %cmplttmp2 = icmp slt i64 %"%pci_idx.load", 0
  br i1 %cmplttmp2, label %then_324, label %merge_325

while_cond_321:                                   ; preds = %merge_327, %entry
  %"%xhci_count.load" = load i64, ptr @xhci_count, align 8
  %"%XHCI_MAX_HC.load" = load i64, ptr @XHCI_MAX_HC, align 8
  %cmplttmp = icmp slt i64 %"%xhci_count.load", %"%XHCI_MAX_HC.load"
  br i1 %cmplttmp, label %while_body_322, label %while_exit_323

while_exit_323:                                   ; preds = %while_cond_321
  %calltmp3 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  %"%1414.load" = load i64, ptr @xhci_count, align 8
  %calltmp4 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1414.load")
  %calltmp5 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr @xhci_count, align 8
  ret i64 %loadtmp
}

define i64 @"xhci_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %"%h" = alloca i64, align 8
  %calltmp = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.35 to i64))
  %calltmp1 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.36 to i64))
  %"%1426.load" = load i64, ptr @xhci_count, align 8
  %calltmp3 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1426.load")
  %calltmp4 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%h", align 8
  br label %while_cond_352

merge_356:                                        ; preds = %then_355, %while_body_353
  %"%h.load33" = load i64, ptr %"%h", align 8
  %addtmp = add i64 %"%h.load33", 1
  store i64 %addtmp, ptr %"%h", align 8
  br label %while_cond_352

then_355:                                         ; preds = %while_body_353
  %calltmp18 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.42 to i64))
  %"%1439.load" = load i64, ptr %"%h", align 8
  %calltmp19 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1439.load")
  %calltmp20 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.43 to i64))
  %"%h.load21" = load i64, ptr %"%h", align 8
  %calltmp22 = call i64 @xhci_get_base(i64 %"%h.load21")
  %calltmp23 = call i64 @xhci_serial_hex32(i64 %calltmp22)
  %calltmp24 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  %"%h.load25" = load i64, ptr %"%h", align 8
  %calltmp26 = call i64 @xhci_get_num_ports(i64 %"%h.load25")
  %calltmp27 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp26)
  %calltmp28 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.34 to i64))
  %"%h.load29" = load i64, ptr %"%h", align 8
  %calltmp30 = call i64 @xhci_get_max_slots(i64 %"%h.load29")
  %calltmp31 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp30)
  %calltmp32 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_356

while_body_353:                                   ; preds = %while_cond_352
  %"%h.load5" = load i64, ptr %"%h", align 8
  %calltmp6 = call i64 @xhci_get_active(i64 %"%h.load5")
  %cmpeqtmp = icmp eq i64 %calltmp6, 1
  br i1 %cmpeqtmp, label %then_355, label %merge_356

while_cond_352:                                   ; preds = %merge_356, %entry
  %"%h.load" = load i64, ptr %"%h", align 8
  %"%xhci_count.load" = load i64, ptr @xhci_count, align 8
  %cmplttmp = icmp slt i64 %"%h.load", %"%xhci_count.load"
  br i1 %cmplttmp, label %while_body_353, label %while_exit_354

while_exit_354:                                   ; preds = %while_cond_352
  %calltmp7 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.37 to i64))
  %"%1465.load" = load i64, ptr @xhci_commands_sent, align 8
  %calltmp8 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1465.load")
  %calltmp9 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.38 to i64))
  %"%1470.load" = load i64, ptr @xhci_events_received, align 8
  %calltmp10 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1470.load")
  %calltmp11 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.39 to i64))
  %"%1475.load" = load i64, ptr @xhci_devices_found, align 8
  %calltmp12 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1475.load")
  %calltmp13 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.40 to i64))
  %"%1480.load" = load i64, ptr @xhci_errors, align 8
  %calltmp14 = call i64 @"xhci_serial_\D8\B1\D9\82\D9\85"(i64 %"%1480.load")
  %calltmp15 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp16 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.41 to i64))
  %calltmp17 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @"xhci_\D9\87\D9\8A\D8\A6"() {
entry:
  %"%h" = alloca i64, align 8
  %"%xhci_devices_found" = alloca i64, align 8
  %"%xhci_errors" = alloca i64, align 8
  %"%xhci_events_received" = alloca i64, align 8
  %"%xhci_commands_sent" = alloca i64, align 8
  %"%xhci_count" = alloca i64, align 8
  %calltmp = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.46 to i64))
  %calltmp1 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%xhci_count", align 8
  store i64 0, ptr %"%xhci_commands_sent", align 8
  store i64 0, ptr %"%xhci_events_received", align 8
  store i64 0, ptr %"%xhci_errors", align 8
  store i64 0, ptr %"%xhci_devices_found", align 8
  %calltmp2 = call i64 @xhci_detect_controllers()
  store i64 0, ptr %"%h", align 8
  br label %while_cond_357

while_body_358:                                   ; preds = %while_cond_357
  %"%h.load3" = load i64, ptr %"%h", align 8
  %calltmp4 = call i64 @xhci_reset(i64 %"%h.load3")
  %"%h.load5" = load i64, ptr %"%h", align 8
  %calltmp6 = call i64 @xhci_alloc_rings(i64 %"%h.load5")
  %"%h.load7" = load i64, ptr %"%h", align 8
  %calltmp8 = call i64 @xhci_setup_registers(i64 %"%h.load7")
  %"%h.load9" = load i64, ptr %"%h", align 8
  %calltmp10 = call i64 @xhci_start(i64 %"%h.load9")
  %"%h.load11" = load i64, ptr %"%h", align 8
  %calltmp12 = call i64 @xhci_scan_ports(i64 %"%h.load11")
  %"%h.load13" = load i64, ptr %"%h", align 8
  %addtmp = add i64 %"%h.load13", 1
  store i64 %addtmp, ptr %"%h", align 8
  br label %while_cond_357

while_cond_357:                                   ; preds = %while_body_358, %entry
  %"%h.load" = load i64, ptr %"%h", align 8
  %"%xhci_count.load" = load i64, ptr %"%xhci_count", align 8
  %cmplttmp = icmp slt i64 %"%h.load", %"%xhci_count.load"
  br i1 %cmplttmp, label %while_body_358, label %while_exit_359

while_exit_359:                                   ; preds = %while_cond_357
  %calltmp14 = call i64 @"xhci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.47 to i64))
  %calltmp15 = call i64 @"xhci_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr %"%xhci_count", align 8
  ret i64 %loadtmp
}

define i64 @"xhci_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  ret i64 0
}

define void @__sad_main() {
entry:
  %"%xhci_devices_found" = alloca i64, align 8
  %"%xhci_errors" = alloca i64, align 8
  %"%xhci_events_received" = alloca i64, align 8
  %"%xhci_commands_sent" = alloca i64, align 8
  %"%xhci_scratch_3" = alloca i64, align 8
  %"%xhci_scratch_2" = alloca i64, align 8
  %"%xhci_scratch_1" = alloca i64, align 8
  %"%xhci_scratch_0" = alloca i64, align 8
  %"%xhci_erst_3" = alloca i64, align 8
  %"%xhci_erst_2" = alloca i64, align 8
  %"%xhci_erst_1" = alloca i64, align 8
  %"%xhci_erst_0" = alloca i64, align 8
  %"%xhci_dcbaa_3" = alloca i64, align 8
  %"%xhci_dcbaa_2" = alloca i64, align 8
  %"%xhci_dcbaa_1" = alloca i64, align 8
  %"%xhci_dcbaa_0" = alloca i64, align 8
  %"%xhci_evt_cycle_3" = alloca i64, align 8
  %"%xhci_evt_cycle_2" = alloca i64, align 8
  %"%xhci_evt_cycle_1" = alloca i64, align 8
  %"%xhci_evt_cycle_0" = alloca i64, align 8
  %"%xhci_evt_deq_3" = alloca i64, align 8
  %"%xhci_evt_deq_2" = alloca i64, align 8
  %"%xhci_evt_deq_1" = alloca i64, align 8
  %"%xhci_evt_deq_0" = alloca i64, align 8
  %"%xhci_evt_ring_3" = alloca i64, align 8
  %"%xhci_evt_ring_2" = alloca i64, align 8
  %"%xhci_evt_ring_1" = alloca i64, align 8
  %"%xhci_evt_ring_0" = alloca i64, align 8
  %"%xhci_cmd_cycle_3" = alloca i64, align 8
  %"%xhci_cmd_cycle_2" = alloca i64, align 8
  %"%xhci_cmd_cycle_1" = alloca i64, align 8
  %"%xhci_cmd_cycle_0" = alloca i64, align 8
  %"%xhci_cmd_enq_3" = alloca i64, align 8
  %"%xhci_cmd_enq_2" = alloca i64, align 8
  %"%xhci_cmd_enq_1" = alloca i64, align 8
  %"%xhci_cmd_enq_0" = alloca i64, align 8
  %"%xhci_cmd_ring_3" = alloca i64, align 8
  %"%xhci_cmd_ring_2" = alloca i64, align 8
  %"%xhci_cmd_ring_1" = alloca i64, align 8
  %"%xhci_cmd_ring_0" = alloca i64, align 8
  %"%xhci_count" = alloca i64, align 8
  %"%xhci_active_3" = alloca i64, align 8
  %"%xhci_active_2" = alloca i64, align 8
  %"%xhci_active_1" = alloca i64, align 8
  %"%xhci_active_0" = alloca i64, align 8
  %"%xhci_irq_3" = alloca i64, align 8
  %"%xhci_irq_2" = alloca i64, align 8
  %"%xhci_irq_1" = alloca i64, align 8
  %"%xhci_irq_0" = alloca i64, align 8
  %"%xhci_pci_idx_3" = alloca i64, align 8
  %"%xhci_pci_idx_2" = alloca i64, align 8
  %"%xhci_pci_idx_1" = alloca i64, align 8
  %"%xhci_pci_idx_0" = alloca i64, align 8
  %"%xhci_max_slots_3" = alloca i64, align 8
  %"%xhci_max_slots_2" = alloca i64, align 8
  %"%xhci_max_slots_1" = alloca i64, align 8
  %"%xhci_max_slots_0" = alloca i64, align 8
  %"%xhci_num_ports_3" = alloca i64, align 8
  %"%xhci_num_ports_2" = alloca i64, align 8
  %"%xhci_num_ports_1" = alloca i64, align 8
  %"%xhci_num_ports_0" = alloca i64, align 8
  %"%xhci_rt_off_3" = alloca i64, align 8
  %"%xhci_rt_off_2" = alloca i64, align 8
  %"%xhci_rt_off_1" = alloca i64, align 8
  %"%xhci_rt_off_0" = alloca i64, align 8
  %"%xhci_db_off_3" = alloca i64, align 8
  %"%xhci_db_off_2" = alloca i64, align 8
  %"%xhci_db_off_1" = alloca i64, align 8
  %"%xhci_db_off_0" = alloca i64, align 8
  %"%xhci_op_base_3" = alloca i64, align 8
  %"%xhci_op_base_2" = alloca i64, align 8
  %"%xhci_op_base_1" = alloca i64, align 8
  %"%xhci_op_base_0" = alloca i64, align 8
  %"%xhci_cap_len_3" = alloca i64, align 8
  %"%xhci_cap_len_2" = alloca i64, align 8
  %"%xhci_cap_len_1" = alloca i64, align 8
  %"%xhci_cap_len_0" = alloca i64, align 8
  %"%xhci_base_3" = alloca i64, align 8
  %"%xhci_base_2" = alloca i64, align 8
  %"%xhci_base_1" = alloca i64, align 8
  %"%xhci_base_0" = alloca i64, align 8
  %"%TRB_CC_USB_ERROR" = alloca i64, align 8
  %"%TRB_CC_STALL" = alloca i64, align 8
  %"%TRB_CC_SHORT_PACKET" = alloca i64, align 8
  %"%TRB_CC_SUCCESS" = alloca i64, align 8
  %"%TRB_HOST_CONTROLLER" = alloca i64, align 8
  %"%TRB_PORT_STATUS_CHANGE" = alloca i64, align 8
  %"%TRB_COMMAND_COMPLETE" = alloca i64, align 8
  %"%TRB_TRANSFER_EVENT" = alloca i64, align 8
  %"%TRB_NOOP" = alloca i64, align 8
  %"%TRB_RESET_EP" = alloca i64, align 8
  %"%TRB_EVAL_CONTEXT" = alloca i64, align 8
  %"%TRB_CONFIG_EP" = alloca i64, align 8
  %"%TRB_ADDRESS_DEVICE" = alloca i64, align 8
  %"%TRB_DISABLE_SLOT" = alloca i64, align 8
  %"%TRB_ENABLE_SLOT" = alloca i64, align 8
  %"%TRB_LINK" = alloca i64, align 8
  %"%TRB_STATUS" = alloca i64, align 8
  %"%TRB_DATA" = alloca i64, align 8
  %"%TRB_SETUP" = alloca i64, align 8
  %"%TRB_NORMAL" = alloca i64, align 8
  %"%XHCI_SPEED_SUPER" = alloca i64, align 8
  %"%XHCI_SPEED_HIGH" = alloca i64, align 8
  %"%XHCI_SPEED_LOW" = alloca i64, align 8
  %"%XHCI_SPEED_FULL" = alloca i64, align 8
  %"%PORTSC_WRC" = alloca i64, align 8
  %"%PORTSC_PRC" = alloca i64, align 8
  %"%PORTSC_CSC" = alloca i64, align 8
  %"%PORTSC_SPEED_MASK" = alloca i64, align 8
  %"%PORTSC_PP" = alloca i64, align 8
  %"%PORTSC_PR" = alloca i64, align 8
  %"%PORTSC_OCA" = alloca i64, align 8
  %"%PORTSC_PED" = alloca i64, align 8
  %"%PORTSC_CCS" = alloca i64, align 8
  %"%XHCI_STS_CNR" = alloca i64, align 8
  %"%XHCI_STS_PCD" = alloca i64, align 8
  %"%XHCI_STS_EINT" = alloca i64, align 8
  %"%XHCI_STS_HSE" = alloca i64, align 8
  %"%XHCI_STS_HCH" = alloca i64, align 8
  %"%XHCI_CMD_HSEE" = alloca i64, align 8
  %"%XHCI_CMD_INTE" = alloca i64, align 8
  %"%XHCI_CMD_HCRST" = alloca i64, align 8
  %"%XHCI_CMD_RUN" = alloca i64, align 8
  %"%XHCI_PORTSC_SIZE" = alloca i64, align 8
  %"%XHCI_PORTSC_OFFSET" = alloca i64, align 8
  %"%XHCI_OP_CONFIG" = alloca i64, align 8
  %"%XHCI_OP_DCBAAP" = alloca i64, align 8
  %"%XHCI_OP_CRCR" = alloca i64, align 8
  %"%XHCI_OP_DNCTRL" = alloca i64, align 8
  %"%XHCI_OP_PAGESIZE" = alloca i64, align 8
  %"%XHCI_OP_USBSTS" = alloca i64, align 8
  %"%XHCI_OP_USBCMD" = alloca i64, align 8
  %"%XHCI_CAP_HCCPARAMS2" = alloca i64, align 8
  %"%XHCI_CAP_RTSOFF" = alloca i64, align 8
  %"%XHCI_CAP_DBOFF" = alloca i64, align 8
  %"%XHCI_CAP_HCCPARAMS1" = alloca i64, align 8
  %"%XHCI_CAP_HCSPARAMS3" = alloca i64, align 8
  %"%XHCI_CAP_HCSPARAMS2" = alloca i64, align 8
  %"%XHCI_CAP_HCSPARAMS1" = alloca i64, align 8
  %"%XHCI_CAP_HCIVERSION" = alloca i64, align 8
  %"%XHCI_CAP_CAPLENGTH" = alloca i64, align 8
  %"%XHCI_MAX_TRB_RING" = alloca i64, align 8
  %"%XHCI_MAX_SLOTS" = alloca i64, align 8
  %"%XHCI_MAX_PORTS" = alloca i64, align 8
  %"%XHCI_MAX_HC" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 4, ptr %"%XHCI_MAX_HC", align 8
  store i64 16, ptr %"%XHCI_MAX_PORTS", align 8
  store i64 32, ptr %"%XHCI_MAX_SLOTS", align 8
  store i64 64, ptr %"%XHCI_MAX_TRB_RING", align 8
  store i64 0, ptr %"%XHCI_CAP_CAPLENGTH", align 8
  store i64 2, ptr %"%XHCI_CAP_HCIVERSION", align 8
  store i64 4, ptr %"%XHCI_CAP_HCSPARAMS1", align 8
  store i64 8, ptr %"%XHCI_CAP_HCSPARAMS2", align 8
  store i64 12, ptr %"%XHCI_CAP_HCSPARAMS3", align 8
  store i64 16, ptr %"%XHCI_CAP_HCCPARAMS1", align 8
  store i64 20, ptr %"%XHCI_CAP_DBOFF", align 8
  store i64 24, ptr %"%XHCI_CAP_RTSOFF", align 8
  store i64 28, ptr %"%XHCI_CAP_HCCPARAMS2", align 8
  store i64 0, ptr %"%XHCI_OP_USBCMD", align 8
  store i64 4, ptr %"%XHCI_OP_USBSTS", align 8
  store i64 8, ptr %"%XHCI_OP_PAGESIZE", align 8
  store i64 20, ptr %"%XHCI_OP_DNCTRL", align 8
  store i64 24, ptr %"%XHCI_OP_CRCR", align 8
  store i64 48, ptr %"%XHCI_OP_DCBAAP", align 8
  store i64 56, ptr %"%XHCI_OP_CONFIG", align 8
  store i64 1024, ptr %"%XHCI_PORTSC_OFFSET", align 8
  store i64 16, ptr %"%XHCI_PORTSC_SIZE", align 8
  store i64 1, ptr %"%XHCI_CMD_RUN", align 8
  store i64 2, ptr %"%XHCI_CMD_HCRST", align 8
  store i64 4, ptr %"%XHCI_CMD_INTE", align 8
  store i64 8, ptr %"%XHCI_CMD_HSEE", align 8
  store i64 1, ptr %"%XHCI_STS_HCH", align 8
  store i64 4, ptr %"%XHCI_STS_HSE", align 8
  store i64 8, ptr %"%XHCI_STS_EINT", align 8
  store i64 16, ptr %"%XHCI_STS_PCD", align 8
  store i64 2048, ptr %"%XHCI_STS_CNR", align 8
  store i64 1, ptr %"%PORTSC_CCS", align 8
  store i64 2, ptr %"%PORTSC_PED", align 8
  store i64 8, ptr %"%PORTSC_OCA", align 8
  store i64 16, ptr %"%PORTSC_PR", align 8
  store i64 512, ptr %"%PORTSC_PP", align 8
  store i64 15360, ptr %"%PORTSC_SPEED_MASK", align 8
  store i64 131072, ptr %"%PORTSC_CSC", align 8
  store i64 2097152, ptr %"%PORTSC_PRC", align 8
  store i64 524288, ptr %"%PORTSC_WRC", align 8
  store i64 1, ptr %"%XHCI_SPEED_FULL", align 8
  store i64 2, ptr %"%XHCI_SPEED_LOW", align 8
  store i64 3, ptr %"%XHCI_SPEED_HIGH", align 8
  store i64 4, ptr %"%XHCI_SPEED_SUPER", align 8
  store i64 1, ptr %"%TRB_NORMAL", align 8
  store i64 2, ptr %"%TRB_SETUP", align 8
  store i64 3, ptr %"%TRB_DATA", align 8
  store i64 4, ptr %"%TRB_STATUS", align 8
  store i64 6, ptr %"%TRB_LINK", align 8
  store i64 9, ptr %"%TRB_ENABLE_SLOT", align 8
  store i64 10, ptr %"%TRB_DISABLE_SLOT", align 8
  store i64 11, ptr %"%TRB_ADDRESS_DEVICE", align 8
  store i64 12, ptr %"%TRB_CONFIG_EP", align 8
  store i64 13, ptr %"%TRB_EVAL_CONTEXT", align 8
  store i64 14, ptr %"%TRB_RESET_EP", align 8
  store i64 23, ptr %"%TRB_NOOP", align 8
  store i64 32, ptr %"%TRB_TRANSFER_EVENT", align 8
  store i64 33, ptr %"%TRB_COMMAND_COMPLETE", align 8
  store i64 34, ptr %"%TRB_PORT_STATUS_CHANGE", align 8
  store i64 37, ptr %"%TRB_HOST_CONTROLLER", align 8
  store i64 1, ptr %"%TRB_CC_SUCCESS", align 8
  store i64 13, ptr %"%TRB_CC_SHORT_PACKET", align 8
  store i64 6, ptr %"%TRB_CC_STALL", align 8
  store i64 5, ptr %"%TRB_CC_USB_ERROR", align 8
  store i64 0, ptr %"%xhci_base_0", align 8
  store i64 0, ptr %"%xhci_base_1", align 8
  store i64 0, ptr %"%xhci_base_2", align 8
  store i64 0, ptr %"%xhci_base_3", align 8
  store i64 0, ptr %"%xhci_cap_len_0", align 8
  store i64 0, ptr %"%xhci_cap_len_1", align 8
  store i64 0, ptr %"%xhci_cap_len_2", align 8
  store i64 0, ptr %"%xhci_cap_len_3", align 8
  store i64 0, ptr %"%xhci_op_base_0", align 8
  store i64 0, ptr %"%xhci_op_base_1", align 8
  store i64 0, ptr %"%xhci_op_base_2", align 8
  store i64 0, ptr %"%xhci_op_base_3", align 8
  store i64 0, ptr %"%xhci_db_off_0", align 8
  store i64 0, ptr %"%xhci_db_off_1", align 8
  store i64 0, ptr %"%xhci_db_off_2", align 8
  store i64 0, ptr %"%xhci_db_off_3", align 8
  store i64 0, ptr %"%xhci_rt_off_0", align 8
  store i64 0, ptr %"%xhci_rt_off_1", align 8
  store i64 0, ptr %"%xhci_rt_off_2", align 8
  store i64 0, ptr %"%xhci_rt_off_3", align 8
  store i64 0, ptr %"%xhci_num_ports_0", align 8
  store i64 0, ptr %"%xhci_num_ports_1", align 8
  store i64 0, ptr %"%xhci_num_ports_2", align 8
  store i64 0, ptr %"%xhci_num_ports_3", align 8
  store i64 0, ptr %"%xhci_max_slots_0", align 8
  store i64 0, ptr %"%xhci_max_slots_1", align 8
  store i64 0, ptr %"%xhci_max_slots_2", align 8
  store i64 0, ptr %"%xhci_max_slots_3", align 8
  store i64 0, ptr %"%xhci_pci_idx_0", align 8
  store i64 0, ptr %"%xhci_pci_idx_1", align 8
  store i64 0, ptr %"%xhci_pci_idx_2", align 8
  store i64 0, ptr %"%xhci_pci_idx_3", align 8
  store i64 0, ptr %"%xhci_irq_0", align 8
  store i64 0, ptr %"%xhci_irq_1", align 8
  store i64 0, ptr %"%xhci_irq_2", align 8
  store i64 0, ptr %"%xhci_irq_3", align 8
  store i64 0, ptr %"%xhci_active_0", align 8
  store i64 0, ptr %"%xhci_active_1", align 8
  store i64 0, ptr %"%xhci_active_2", align 8
  store i64 0, ptr %"%xhci_active_3", align 8
  store i64 0, ptr %"%xhci_count", align 8
  store i64 0, ptr %"%xhci_cmd_ring_0", align 8
  store i64 0, ptr %"%xhci_cmd_ring_1", align 8
  store i64 0, ptr %"%xhci_cmd_ring_2", align 8
  store i64 0, ptr %"%xhci_cmd_ring_3", align 8
  store i64 0, ptr %"%xhci_cmd_enq_0", align 8
  store i64 0, ptr %"%xhci_cmd_enq_1", align 8
  store i64 0, ptr %"%xhci_cmd_enq_2", align 8
  store i64 0, ptr %"%xhci_cmd_enq_3", align 8
  store i64 1, ptr %"%xhci_cmd_cycle_0", align 8
  store i64 1, ptr %"%xhci_cmd_cycle_1", align 8
  store i64 1, ptr %"%xhci_cmd_cycle_2", align 8
  store i64 1, ptr %"%xhci_cmd_cycle_3", align 8
  store i64 0, ptr %"%xhci_evt_ring_0", align 8
  store i64 0, ptr %"%xhci_evt_ring_1", align 8
  store i64 0, ptr %"%xhci_evt_ring_2", align 8
  store i64 0, ptr %"%xhci_evt_ring_3", align 8
  store i64 0, ptr %"%xhci_evt_deq_0", align 8
  store i64 0, ptr %"%xhci_evt_deq_1", align 8
  store i64 0, ptr %"%xhci_evt_deq_2", align 8
  store i64 0, ptr %"%xhci_evt_deq_3", align 8
  store i64 1, ptr %"%xhci_evt_cycle_0", align 8
  store i64 1, ptr %"%xhci_evt_cycle_1", align 8
  store i64 1, ptr %"%xhci_evt_cycle_2", align 8
  store i64 1, ptr %"%xhci_evt_cycle_3", align 8
  store i64 0, ptr %"%xhci_dcbaa_0", align 8
  store i64 0, ptr %"%xhci_dcbaa_1", align 8
  store i64 0, ptr %"%xhci_dcbaa_2", align 8
  store i64 0, ptr %"%xhci_dcbaa_3", align 8
  store i64 0, ptr %"%xhci_erst_0", align 8
  store i64 0, ptr %"%xhci_erst_1", align 8
  store i64 0, ptr %"%xhci_erst_2", align 8
  store i64 0, ptr %"%xhci_erst_3", align 8
  store i64 0, ptr %"%xhci_scratch_0", align 8
  store i64 0, ptr %"%xhci_scratch_1", align 8
  store i64 0, ptr %"%xhci_scratch_2", align 8
  store i64 0, ptr %"%xhci_scratch_3", align 8
  store i64 0, ptr %"%xhci_commands_sent", align 8
  store i64 0, ptr %"%xhci_events_received", align 8
  store i64 0, ptr %"%xhci_errors", align 8
  store i64 0, ptr %"%xhci_devices_found", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
