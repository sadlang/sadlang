; ModuleID = 'usb_شبكة_نحلة'
source_filename = "usb_\D8\B4\D8\A8\D9\83\D8\A9_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@COM1 = constant i64 1016
@CDC_CLASS = constant i64 2
@CDC_DATA_CLASS = constant i64 10
@CDC_SUBCLASS_ACM = constant i64 2
@CDC_SUBCLASS_ECM = constant i64 6
@CDC_SUBCLASS_NCM = constant i64 13
@CDC_DESC_HEADER = constant i64 0
@CDC_DESC_CALL_MGMT = constant i64 1
@CDC_DESC_ACM = constant i64 2
@CDC_DESC_UNION = constant i64 6
@CDC_DESC_ETHERNET = constant i64 15
@CDC_SET_LINE_CODING = constant i64 32
@CDC_GET_LINE_CODING = constant i64 33
@CDC_SET_CONTROL_LINE = constant i64 34
@CDC_SEND_BREAK = constant i64 35
@CDC_NOTIFY_NETWORK_CONNECTION = constant i64 0
@CDC_NOTIFY_SPEED_CHANGE = constant i64 42
@CDC_NOTIFY_SERIAL_STATE = constant i64 32
@ECM_SET_ETHERNET_FILTER = constant i64 67
@ECM_GET_ETHERNET_STAT = constant i64 68
@NCM_GET_NTB_PARAMETERS = constant i64 128
@NCM_GET_NTB_INPUT_SIZE = constant i64 133
@NCM_SET_NTB_INPUT_SIZE = constant i64 134
@ETH_FILTER_PROMISCUOUS = constant i64 1
@ETH_FILTER_ALL_MULTICAST = constant i64 2
@ETH_FILTER_DIRECTED = constant i64 4
@ETH_FILTER_BROADCAST = constant i64 8
@ETH_FILTER_MULTICAST = constant i64 16
@LINK_DOWN = constant i64 0
@LINK_UP = constant i64 1
@ETH_MTU = constant i64 1500
@ETH_HEADER_SIZE = constant i64 14
@ETH_MAX_FRAME = constant i64 1514
@CDC_MAX_SEGMENT = constant i64 2048
@CDC_MAX_DEVICES = constant i64 4
@CDC_RX_RING_SIZE = constant i64 16
@CDC_TX_RING_SIZE = constant i64 16
@cdc_present_0 = global i64 0
@cdc_present_1 = global i64 0
@cdc_present_2 = global i64 0
@cdc_present_3 = global i64 0
@cdc_addr_0 = global i64 0
@cdc_addr_1 = global i64 0
@cdc_addr_2 = global i64 0
@cdc_addr_3 = global i64 0
@cdc_subclass_0 = global i64 0
@cdc_subclass_1 = global i64 0
@cdc_subclass_2 = global i64 0
@cdc_subclass_3 = global i64 0
@cdc_ctrl_iface_0 = global i64 0
@cdc_ctrl_iface_1 = global i64 0
@cdc_ctrl_iface_2 = global i64 0
@cdc_ctrl_iface_3 = global i64 0
@cdc_data_iface_0 = global i64 0
@cdc_data_iface_1 = global i64 0
@cdc_data_iface_2 = global i64 0
@cdc_data_iface_3 = global i64 0
@cdc_ep_in_0 = global i64 0
@cdc_ep_in_1 = global i64 0
@cdc_ep_in_2 = global i64 0
@cdc_ep_in_3 = global i64 0
@cdc_ep_out_0 = global i64 0
@cdc_ep_out_1 = global i64 0
@cdc_ep_out_2 = global i64 0
@cdc_ep_out_3 = global i64 0
@cdc_ep_int_0 = global i64 0
@cdc_ep_int_1 = global i64 0
@cdc_ep_int_2 = global i64 0
@cdc_ep_int_3 = global i64 0
@cdc_maxpkt_0 = global i64 512
@cdc_maxpkt_1 = global i64 512
@cdc_maxpkt_2 = global i64 512
@cdc_maxpkt_3 = global i64 512
@cdc_link_0 = global i64 0
@cdc_link_1 = global i64 0
@cdc_link_2 = global i64 0
@cdc_link_3 = global i64 0
@cdc_speed_0 = global i64 0
@cdc_speed_1 = global i64 0
@cdc_speed_2 = global i64 0
@cdc_speed_3 = global i64 0
@cdc_active_0 = global i64 0
@cdc_active_1 = global i64 0
@cdc_active_2 = global i64 0
@cdc_active_3 = global i64 0
@cdc_filter_0 = global i64 12
@cdc_filter_1 = global i64 12
@cdc_filter_2 = global i64 12
@cdc_filter_3 = global i64 12
@mac_0_0 = global i64 0
@mac_0_1 = global i64 0
@mac_0_2 = global i64 0
@mac_0_3 = global i64 0
@mac_0_4 = global i64 0
@mac_0_5 = global i64 0
@mac_1_0 = global i64 0
@mac_1_1 = global i64 0
@mac_1_2 = global i64 0
@mac_1_3 = global i64 0
@mac_1_4 = global i64 0
@mac_1_5 = global i64 0
@mac_2_0 = global i64 0
@mac_2_1 = global i64 0
@mac_2_2 = global i64 0
@mac_2_3 = global i64 0
@mac_2_4 = global i64 0
@mac_2_5 = global i64 0
@mac_3_0 = global i64 0
@mac_3_1 = global i64 0
@mac_3_2 = global i64 0
@mac_3_3 = global i64 0
@mac_3_4 = global i64 0
@mac_3_5 = global i64 0
@cdc_rx_buf_0 = global i64 0
@cdc_rx_buf_1 = global i64 0
@cdc_rx_buf_2 = global i64 0
@cdc_rx_buf_3 = global i64 0
@cdc_tx_buf_0 = global i64 0
@cdc_tx_buf_1 = global i64 0
@cdc_tx_buf_2 = global i64 0
@cdc_tx_buf_3 = global i64 0
@cdc_rx_head_0 = global i64 0
@cdc_rx_tail_0 = global i64 0
@cdc_tx_head_0 = global i64 0
@cdc_tx_tail_0 = global i64 0
@cdc_rx_head_1 = global i64 0
@cdc_rx_tail_1 = global i64 0
@cdc_tx_head_1 = global i64 0
@cdc_tx_tail_1 = global i64 0
@cdc_rx_head_2 = global i64 0
@cdc_rx_tail_2 = global i64 0
@cdc_tx_head_2 = global i64 0
@cdc_tx_tail_2 = global i64 0
@cdc_rx_head_3 = global i64 0
@cdc_rx_tail_3 = global i64 0
@cdc_tx_head_3 = global i64 0
@cdc_tx_tail_3 = global i64 0
@cdc_device_count = global i64 0
@cdc_tx_packets = global i64 0
@cdc_rx_packets = global i64 0
@cdc_tx_bytes = global i64 0
@cdc_rx_bytes = global i64 0
@cdc_errors = global i64 0
@cdc_dropped = global i64 0
@str.const = private unnamed_addr constant [14 x i8] c"[CDC] Device \00", align 1
@str.const.1 = private unnamed_addr constant [23 x i8] c" registered: USB addr=\00", align 1
@str.const.2 = private unnamed_addr constant [11 x i8] c" subclass=\00", align 1
@str.const.3 = private unnamed_addr constant [11 x i8] c"[CDC] Dev \00", align 1
@str.const.4 = private unnamed_addr constant [17 x i8] c" activated, MAC=\00", align 1
@str.const.6 = private unnamed_addr constant [13 x i8] c" deactivated\00", align 1
@str.const.7 = private unnamed_addr constant [50 x i8] c"\E2\95\90\E2\95\90\E2\95\90 [CDC] \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\B4\D8\A8\D9\83\D8\A9 USB \E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.8 = private unnamed_addr constant [15 x i8] c"  \D8\A3\D8\AC\D9\87\D8\B2\D8\A9: \00", align 1
@str.const.9 = private unnamed_addr constant [7 x i8] c"  TX: \00", align 1
@str.const.10 = private unnamed_addr constant [8 x i8] c" pkts, \00", align 1
@str.const.11 = private unnamed_addr constant [7 x i8] c" bytes\00", align 1
@str.const.12 = private unnamed_addr constant [7 x i8] c"  RX: \00", align 1
@str.const.15 = private unnamed_addr constant [15 x i8] c"  \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1: \00", align 1
@str.const.16 = private unnamed_addr constant [14 x i8] c" \D9\85\D8\B3\D9\82\D8\B7\D8\A9: \00", align 1
@str.const.17 = private unnamed_addr constant [106 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.18 = private unnamed_addr constant [7 x i8] c"  Dev \00", align 1
@str.const.19 = private unnamed_addr constant [7 x i8] c": MAC=\00", align 1
@str.const.20 = private unnamed_addr constant [10 x i8] c" LINK_UP \00", align 1
@str.const.21 = private unnamed_addr constant [5 x i8] c"Mbps\00", align 1
@str.const.22 = private unnamed_addr constant [11 x i8] c" LINK_DOWN\00", align 1
@str.const.23 = private unnamed_addr constant [46 x i8] c"[CDC] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D9\86\D8\B8\D8\A7\D9\85 \D8\B4\D8\A8\D9\83\D8\A9 USB CDC...\00", align 1
@str.const.24 = private unnamed_addr constant [48 x i8] c"[CDC] \D8\AC\D8\A7\D9\87\D8\B2 \E2\80\94 \D9\81\D9\8A \D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1 \D8\A3\D8\AC\D9\87\D8\B2\D8\A9\00", align 1

declare void @sad_ll_port_inb(i64)

declare void @sad_ll_port_outb(i64, i64)

declare void @sad_ll_mem_read8(i64)

declare void @sad_ll_mem_write8(i64, i64)

declare void @sad_ll_mem_read16(i64)

declare void @sad_ll_mem_write16(i64, i64)

declare void @sad_ll_mem_read32(i64)

declare void @sad_ll_mem_write32(i64, i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_kmalloc(i64)

declare void @sad_ll_memset(i64, i64, i64)

define i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
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

define i64 @"cdc_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
entry:
  %"%d2" = alloca i64, align 8
  %"%d3" = alloca i64, align 8
  %"%d4" = alloca i64, align 8
  %"%d5" = alloca i64, align 8
  %"%d6" = alloca i64, align 8
  %"%d7" = alloca i64, align 8
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
  %cmpgetmp19 = icmp sge i64 %"%\D9\86.load", 1000000
  br i1 %cmpgetmp19, label %then_12, label %merge_13

else_16:                                          ; preds = %merge_10
  %"%\D9\86.load31" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp32 = icmp sge i64 %"%\D9\86.load31", 100000
  br i1 %cmpgetmp32, label %then_17, label %merge_18

else_21:                                          ; preds = %merge_15
  %"%\D9\86.load44" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp45 = icmp sge i64 %"%\D9\86.load44", 10000
  br i1 %cmpgetmp45, label %then_22, label %merge_23

else_26:                                          ; preds = %merge_20
  %"%\D9\86.load57" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp58 = icmp sge i64 %"%\D9\86.load57", 1000
  br i1 %cmpgetmp58, label %then_27, label %merge_28

else_31:                                          ; preds = %merge_25
  %"%\D9\86.load70" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp71 = icmp sge i64 %"%\D9\86.load70", 100
  br i1 %cmpgetmp71, label %then_32, label %merge_33

merge_10:                                         ; preds = %merge_13, %then_9
  %"%val.load20" = load i64, ptr %"%val", align 8
  %cmpgetmp21 = icmp sge i64 %"%val.load20", 10000
  br i1 %cmpgetmp21, label %then_14, label %else_16

merge_13:                                         ; preds = %then_12, %else_11
  br label %merge_10

merge_15:                                         ; preds = %merge_18, %then_14
  %"%val.load33" = load i64, ptr %"%val", align 8
  %cmpgetmp34 = icmp sge i64 %"%val.load33", 1000
  br i1 %cmpgetmp34, label %then_19, label %else_21

merge_18:                                         ; preds = %then_17, %else_16
  br label %merge_15

merge_20:                                         ; preds = %merge_23, %then_19
  %"%val.load46" = load i64, ptr %"%val", align 8
  %cmpgetmp47 = icmp sge i64 %"%val.load46", 100
  br i1 %cmpgetmp47, label %then_24, label %else_26

merge_23:                                         ; preds = %then_22, %else_21
  br label %merge_20

merge_25:                                         ; preds = %merge_28, %then_24
  %"%val.load59" = load i64, ptr %"%val", align 8
  %cmpgetmp60 = icmp sge i64 %"%val.load59", 10
  br i1 %cmpgetmp60, label %then_29, label %else_31

merge_28:                                         ; preds = %then_27, %else_26
  br label %merge_25

merge_30:                                         ; preds = %merge_33, %then_29
  %"%val.load72" = load i64, ptr %"%val", align 8
  %addtmp73 = add i64 48, %"%val.load72"
  %"%COM1.load74" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load74", i64 %addtmp73)
  ret i64 0

merge_33:                                         ; preds = %then_32, %else_31
  br label %merge_30

merge_6:                                          ; preds = %then_5, %entry
  %"%val.load3" = load i64, ptr %"%val", align 8
  %cmpgetmp = icmp sge i64 %"%val.load3", 1000000
  br i1 %cmpgetmp, label %then_7, label %merge_8

merge_8:                                          ; preds = %then_7, %merge_6
  %"%val.load9" = load i64, ptr %"%val", align 8
  %cmpgetmp10 = icmp sge i64 %"%val.load9", 100000
  br i1 %cmpgetmp10, label %then_9, label %else_11

then_12:                                          ; preds = %else_11
  %"%COM1.load22" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load22", i64 48)
  br label %merge_13

then_14:                                          ; preds = %merge_10
  %"%val.load23" = load i64, ptr %"%val", align 8
  %divtmp24 = sdiv i64 %"%val.load23", 10000
  store i64 %divtmp24, ptr %"%d5", align 8
  %"%d5.load" = load i64, ptr %"%d5", align 8
  %addtmp25 = add i64 48, %"%d5.load"
  %"%COM1.load26" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load26", i64 %addtmp25)
  %"%d5.load27" = load i64, ptr %"%d5", align 8
  %multmp28 = mul i64 %"%d5.load27", 10000
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
  %divtmp37 = sdiv i64 %"%val.load36", 1000
  store i64 %divtmp37, ptr %"%d4", align 8
  %"%d4.load" = load i64, ptr %"%d4", align 8
  %addtmp38 = add i64 48, %"%d4.load"
  %"%COM1.load39" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load39", i64 %addtmp38)
  %"%d4.load40" = load i64, ptr %"%d4", align 8
  %multmp41 = mul i64 %"%d4.load40", 1000
  %"%val.load42" = load i64, ptr %"%val", align 8
  %subtmp43 = sub i64 %"%val.load42", %multmp41
  store i64 %subtmp43, ptr %"%val", align 8
  br label %merge_20

then_22:                                          ; preds = %else_21
  %"%COM1.load48" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load48", i64 48)
  br label %merge_23

then_24:                                          ; preds = %merge_20
  %"%val.load49" = load i64, ptr %"%val", align 8
  %divtmp50 = sdiv i64 %"%val.load49", 100
  store i64 %divtmp50, ptr %"%d3", align 8
  %"%d3.load" = load i64, ptr %"%d3", align 8
  %addtmp51 = add i64 48, %"%d3.load"
  %"%COM1.load52" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load52", i64 %addtmp51)
  %"%d3.load53" = load i64, ptr %"%d3", align 8
  %multmp54 = mul i64 %"%d3.load53", 100
  %"%val.load55" = load i64, ptr %"%val", align 8
  %subtmp56 = sub i64 %"%val.load55", %multmp54
  store i64 %subtmp56, ptr %"%val", align 8
  br label %merge_25

then_27:                                          ; preds = %else_26
  %"%COM1.load61" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load61", i64 48)
  br label %merge_28

then_29:                                          ; preds = %merge_25
  %"%val.load62" = load i64, ptr %"%val", align 8
  %divtmp63 = sdiv i64 %"%val.load62", 10
  store i64 %divtmp63, ptr %"%d2", align 8
  %"%d2.load" = load i64, ptr %"%d2", align 8
  %addtmp64 = add i64 48, %"%d2.load"
  %"%COM1.load65" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load65", i64 %addtmp64)
  %"%d2.load66" = load i64, ptr %"%d2", align 8
  %multmp67 = mul i64 %"%d2.load66", 10
  %"%val.load68" = load i64, ptr %"%val", align 8
  %subtmp69 = sub i64 %"%val.load68", %multmp67
  store i64 %subtmp69, ptr %"%val", align 8
  br label %merge_30

then_32:                                          ; preds = %else_31
  %"%COM1.load75" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load75", i64 48)
  br label %merge_33

then_5:                                           ; preds = %entry
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 45)
  %"%val.load2" = load i64, ptr %"%val", align 8
  %subtmp = sub i64 0, %"%val.load2"
  store i64 %subtmp, ptr %"%val", align 8
  br label %merge_6

then_7:                                           ; preds = %merge_6
  %"%val.load4" = load i64, ptr %"%val", align 8
  %divtmp = sdiv i64 %"%val.load4", 1000000
  store i64 %divtmp, ptr %"%d7", align 8
  %"%d7.load" = load i64, ptr %"%d7", align 8
  %addtmp = add i64 48, %"%d7.load"
  %"%COM1.load5" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load5", i64 %addtmp)
  %"%d7.load6" = load i64, ptr %"%d7", align 8
  %multmp = mul i64 %"%d7.load6", 1000000
  %"%val.load7" = load i64, ptr %"%val", align 8
  %subtmp8 = sub i64 %"%val.load7", %multmp
  store i64 %subtmp8, ptr %"%val", align 8
  br label %merge_8

then_9:                                           ; preds = %merge_8
  %"%val.load11" = load i64, ptr %"%val", align 8
  %divtmp12 = sdiv i64 %"%val.load11", 100000
  store i64 %divtmp12, ptr %"%d6", align 8
  %"%d6.load" = load i64, ptr %"%d6", align 8
  %addtmp13 = add i64 48, %"%d6.load"
  %"%COM1.load14" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load14", i64 %addtmp13)
  %"%d6.load15" = load i64, ptr %"%d6", align 8
  %multmp16 = mul i64 %"%d6.load15", 100000
  %"%val.load17" = load i64, ptr %"%val", align 8
  %subtmp18 = sub i64 %"%val.load17", %multmp16
  store i64 %subtmp18, ptr %"%val", align 8
  br label %merge_10
}

define i64 @cdc_serial_hex8(i64 %val) {
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
  br i1 %cmplttmp, label %then_34, label %else_36

else_36:                                          ; preds = %entry
  %"%hi.load4" = load i64, ptr %"%hi", align 8
  %addtmp5 = add i64 55, %"%hi.load4"
  %"%COM1.load6" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load6", i64 %addtmp5)
  br label %merge_35

else_39:                                          ; preds = %merge_35
  %"%lo.load11" = load i64, ptr %"%lo", align 8
  %addtmp12 = add i64 55, %"%lo.load11"
  %"%COM1.load13" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load13", i64 %addtmp12)
  br label %merge_38

merge_35:                                         ; preds = %else_36, %then_34
  %"%lo.load" = load i64, ptr %"%lo", align 8
  %cmplttmp7 = icmp slt i64 %"%lo.load", 10
  br i1 %cmplttmp7, label %then_37, label %else_39

merge_38:                                         ; preds = %else_39, %then_37
  ret i64 0

then_34:                                          ; preds = %entry
  %"%hi.load3" = load i64, ptr %"%hi", align 8
  %addtmp = add i64 48, %"%hi.load3"
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 %addtmp)
  br label %merge_35

then_37:                                          ; preds = %merge_35
  %"%lo.load8" = load i64, ptr %"%lo", align 8
  %addtmp9 = add i64 48, %"%lo.load8"
  %"%COM1.load10" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load10", i64 %addtmp9)
  br label %merge_38
}

define i64 @"cdc_serial_\D8\B3\D8\B7\D8\B1"() {
entry:
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 13)
  %"%COM1.load1" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load1", i64 10)
  ret i64 0
}

define i64 @cdc_get_present(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_40, label %merge_41

merge_41:                                         ; preds = %entry
  %"%d.load2" = load i64, ptr %d1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%d.load2", 1
  br i1 %cmpeqtmp3, label %then_42, label %merge_43

merge_43:                                         ; preds = %merge_41
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_44, label %merge_45

merge_45:                                         ; preds = %merge_43
  %"%d.load8" = load i64, ptr %d1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 3
  br i1 %cmpeqtmp9, label %then_46, label %merge_47

merge_47:                                         ; preds = %merge_45
  ret i64 0

then_40:                                          ; preds = %entry
  %loadtmp = load i64, ptr @cdc_present_0, align 8
  ret i64 %loadtmp

then_42:                                          ; preds = %merge_41
  %loadtmp4 = load i64, ptr @cdc_present_1, align 8
  ret i64 %loadtmp4

then_44:                                          ; preds = %merge_43
  %loadtmp7 = load i64, ptr @cdc_present_2, align 8
  ret i64 %loadtmp7

then_46:                                          ; preds = %merge_45
  %loadtmp10 = load i64, ptr @cdc_present_3, align 8
  ret i64 %loadtmp10
}

define i64 @cdc_set_present(i64 %d, i64 %val) {
entry:
  %"%cdc_present_3" = alloca i64, align 8
  %"%cdc_present_2" = alloca i64, align 8
  %"%cdc_present_1" = alloca i64, align 8
  %"%cdc_present_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_48, label %merge_49

merge_49:                                         ; preds = %then_48, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_50, label %merge_51

merge_51:                                         ; preds = %then_50, %merge_49
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_52, label %merge_53

merge_53:                                         ; preds = %then_52, %merge_51
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_54, label %merge_55

merge_55:                                         ; preds = %then_54, %merge_53
  ret i64 0

then_48:                                          ; preds = %entry
  %"%158.load" = load i64, ptr %val2, align 8
  store i64 %"%158.load", ptr %"%cdc_present_0", align 8
  br label %merge_49

then_50:                                          ; preds = %merge_49
  %"%162.load" = load i64, ptr %val2, align 8
  store i64 %"%162.load", ptr %"%cdc_present_1", align 8
  br label %merge_51

then_52:                                          ; preds = %merge_51
  %"%166.load" = load i64, ptr %val2, align 8
  store i64 %"%166.load", ptr %"%cdc_present_2", align 8
  br label %merge_53

then_54:                                          ; preds = %merge_53
  %"%170.load" = load i64, ptr %val2, align 8
  store i64 %"%170.load", ptr %"%cdc_present_3", align 8
  br label %merge_55
}

define i64 @cdc_get_link(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_56, label %merge_57

merge_57:                                         ; preds = %entry
  %"%d.load2" = load i64, ptr %d1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%d.load2", 1
  br i1 %cmpeqtmp3, label %then_58, label %merge_59

merge_59:                                         ; preds = %merge_57
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_60, label %merge_61

merge_61:                                         ; preds = %merge_59
  %"%d.load8" = load i64, ptr %d1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 3
  br i1 %cmpeqtmp9, label %then_62, label %merge_63

merge_63:                                         ; preds = %merge_61
  ret i64 0

then_56:                                          ; preds = %entry
  %loadtmp = load i64, ptr @cdc_link_0, align 8
  ret i64 %loadtmp

then_58:                                          ; preds = %merge_57
  %loadtmp4 = load i64, ptr @cdc_link_1, align 8
  ret i64 %loadtmp4

then_60:                                          ; preds = %merge_59
  %loadtmp7 = load i64, ptr @cdc_link_2, align 8
  ret i64 %loadtmp7

then_62:                                          ; preds = %merge_61
  %loadtmp10 = load i64, ptr @cdc_link_3, align 8
  ret i64 %loadtmp10
}

define i64 @cdc_set_link(i64 %d, i64 %val) {
entry:
  %"%cdc_link_3" = alloca i64, align 8
  %"%cdc_link_2" = alloca i64, align 8
  %"%cdc_link_1" = alloca i64, align 8
  %"%cdc_link_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_64, label %merge_65

merge_65:                                         ; preds = %then_64, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_66, label %merge_67

merge_67:                                         ; preds = %then_66, %merge_65
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_68, label %merge_69

merge_69:                                         ; preds = %then_68, %merge_67
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_70, label %merge_71

merge_71:                                         ; preds = %then_70, %merge_69
  ret i64 0

then_64:                                          ; preds = %entry
  %"%196.load" = load i64, ptr %val2, align 8
  store i64 %"%196.load", ptr %"%cdc_link_0", align 8
  br label %merge_65

then_66:                                          ; preds = %merge_65
  %"%200.load" = load i64, ptr %val2, align 8
  store i64 %"%200.load", ptr %"%cdc_link_1", align 8
  br label %merge_67

then_68:                                          ; preds = %merge_67
  %"%204.load" = load i64, ptr %val2, align 8
  store i64 %"%204.load", ptr %"%cdc_link_2", align 8
  br label %merge_69

then_70:                                          ; preds = %merge_69
  %"%208.load" = load i64, ptr %val2, align 8
  store i64 %"%208.load", ptr %"%cdc_link_3", align 8
  br label %merge_71
}

define i64 @cdc_get_speed(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_72, label %merge_73

merge_73:                                         ; preds = %entry
  %"%d.load2" = load i64, ptr %d1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%d.load2", 1
  br i1 %cmpeqtmp3, label %then_74, label %merge_75

merge_75:                                         ; preds = %merge_73
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_76, label %merge_77

merge_77:                                         ; preds = %merge_75
  %"%d.load8" = load i64, ptr %d1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 3
  br i1 %cmpeqtmp9, label %then_78, label %merge_79

merge_79:                                         ; preds = %merge_77
  ret i64 0

then_72:                                          ; preds = %entry
  %loadtmp = load i64, ptr @cdc_speed_0, align 8
  ret i64 %loadtmp

then_74:                                          ; preds = %merge_73
  %loadtmp4 = load i64, ptr @cdc_speed_1, align 8
  ret i64 %loadtmp4

then_76:                                          ; preds = %merge_75
  %loadtmp7 = load i64, ptr @cdc_speed_2, align 8
  ret i64 %loadtmp7

then_78:                                          ; preds = %merge_77
  %loadtmp10 = load i64, ptr @cdc_speed_3, align 8
  ret i64 %loadtmp10
}

define i64 @cdc_set_speed(i64 %d, i64 %val) {
entry:
  %"%cdc_speed_3" = alloca i64, align 8
  %"%cdc_speed_2" = alloca i64, align 8
  %"%cdc_speed_1" = alloca i64, align 8
  %"%cdc_speed_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_80, label %merge_81

merge_81:                                         ; preds = %then_80, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_82, label %merge_83

merge_83:                                         ; preds = %then_82, %merge_81
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_84, label %merge_85

merge_85:                                         ; preds = %then_84, %merge_83
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_86, label %merge_87

merge_87:                                         ; preds = %then_86, %merge_85
  ret i64 0

then_80:                                          ; preds = %entry
  %"%234.load" = load i64, ptr %val2, align 8
  store i64 %"%234.load", ptr %"%cdc_speed_0", align 8
  br label %merge_81

then_82:                                          ; preds = %merge_81
  %"%238.load" = load i64, ptr %val2, align 8
  store i64 %"%238.load", ptr %"%cdc_speed_1", align 8
  br label %merge_83

then_84:                                          ; preds = %merge_83
  %"%242.load" = load i64, ptr %val2, align 8
  store i64 %"%242.load", ptr %"%cdc_speed_2", align 8
  br label %merge_85

then_86:                                          ; preds = %merge_85
  %"%246.load" = load i64, ptr %val2, align 8
  store i64 %"%246.load", ptr %"%cdc_speed_3", align 8
  br label %merge_87
}

define i64 @cdc_set_active(i64 %d, i64 %val) {
entry:
  %"%cdc_active_3" = alloca i64, align 8
  %"%cdc_active_2" = alloca i64, align 8
  %"%cdc_active_1" = alloca i64, align 8
  %"%cdc_active_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_88, label %merge_89

merge_89:                                         ; preds = %then_88, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_90, label %merge_91

merge_91:                                         ; preds = %then_90, %merge_89
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_92, label %merge_93

merge_93:                                         ; preds = %then_92, %merge_91
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_94, label %merge_95

merge_95:                                         ; preds = %then_94, %merge_93
  ret i64 0

then_88:                                          ; preds = %entry
  %"%251.load" = load i64, ptr %val2, align 8
  store i64 %"%251.load", ptr %"%cdc_active_0", align 8
  br label %merge_89

then_90:                                          ; preds = %merge_89
  %"%255.load" = load i64, ptr %val2, align 8
  store i64 %"%255.load", ptr %"%cdc_active_1", align 8
  br label %merge_91

then_92:                                          ; preds = %merge_91
  %"%259.load" = load i64, ptr %val2, align 8
  store i64 %"%259.load", ptr %"%cdc_active_2", align 8
  br label %merge_93

then_94:                                          ; preds = %merge_93
  %"%263.load" = load i64, ptr %val2, align 8
  store i64 %"%263.load", ptr %"%cdc_active_3", align 8
  br label %merge_95
}

define i64 @cdc_get_active(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_96, label %merge_97

merge_101:                                        ; preds = %merge_99
  %"%d.load8" = load i64, ptr %d1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 3
  br i1 %cmpeqtmp9, label %then_102, label %merge_103

merge_103:                                        ; preds = %merge_101
  ret i64 0

merge_97:                                         ; preds = %entry
  %"%d.load2" = load i64, ptr %d1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%d.load2", 1
  br i1 %cmpeqtmp3, label %then_98, label %merge_99

merge_99:                                         ; preds = %merge_97
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_100, label %merge_101

then_100:                                         ; preds = %merge_99
  %loadtmp7 = load i64, ptr @cdc_active_2, align 8
  ret i64 %loadtmp7

then_102:                                         ; preds = %merge_101
  %loadtmp10 = load i64, ptr @cdc_active_3, align 8
  ret i64 %loadtmp10

then_96:                                          ; preds = %entry
  %loadtmp = load i64, ptr @cdc_active_0, align 8
  ret i64 %loadtmp

then_98:                                          ; preds = %merge_97
  %loadtmp4 = load i64, ptr @cdc_active_1, align 8
  ret i64 %loadtmp4
}

define i64 @cdc_get_mac(i64 %d, i64 %byte_idx) {
entry:
  %byte_idx2 = alloca i64, align 8
  store i64 %byte_idx, ptr %byte_idx2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_104, label %merge_105

merge_105:                                        ; preds = %merge_117, %entry
  %"%d.load4" = load i64, ptr %d1, align 8
  %cmpeqtmp5 = icmp eq i64 %"%d.load4", 1
  br i1 %cmpeqtmp5, label %then_118, label %merge_119

merge_107:                                        ; preds = %then_104
  %"%byte_idx.load6" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp7 = icmp eq i64 %"%byte_idx.load6", 1
  br i1 %cmpeqtmp7, label %then_108, label %merge_109

merge_109:                                        ; preds = %merge_107
  %"%byte_idx.load9" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp10 = icmp eq i64 %"%byte_idx.load9", 2
  br i1 %cmpeqtmp10, label %then_110, label %merge_111

merge_111:                                        ; preds = %merge_109
  %"%byte_idx.load12" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp13 = icmp eq i64 %"%byte_idx.load12", 3
  br i1 %cmpeqtmp13, label %then_112, label %merge_113

merge_113:                                        ; preds = %merge_111
  %"%byte_idx.load15" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp16 = icmp eq i64 %"%byte_idx.load15", 4
  br i1 %cmpeqtmp16, label %then_114, label %merge_115

merge_115:                                        ; preds = %merge_113
  %"%byte_idx.load18" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp19 = icmp eq i64 %"%byte_idx.load18", 5
  br i1 %cmpeqtmp19, label %then_116, label %merge_117

merge_117:                                        ; preds = %merge_115
  br label %merge_105

merge_119:                                        ; preds = %merge_131, %merge_105
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 2
  br i1 %cmpeqtmp24, label %then_132, label %merge_133

merge_121:                                        ; preds = %then_118
  %"%byte_idx.load26" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp27 = icmp eq i64 %"%byte_idx.load26", 1
  br i1 %cmpeqtmp27, label %then_122, label %merge_123

merge_123:                                        ; preds = %merge_121
  %"%byte_idx.load29" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp30 = icmp eq i64 %"%byte_idx.load29", 2
  br i1 %cmpeqtmp30, label %then_124, label %merge_125

merge_125:                                        ; preds = %merge_123
  %"%byte_idx.load32" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp33 = icmp eq i64 %"%byte_idx.load32", 3
  br i1 %cmpeqtmp33, label %then_126, label %merge_127

merge_127:                                        ; preds = %merge_125
  %"%byte_idx.load35" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp36 = icmp eq i64 %"%byte_idx.load35", 4
  br i1 %cmpeqtmp36, label %then_128, label %merge_129

merge_129:                                        ; preds = %merge_127
  %"%byte_idx.load38" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp39 = icmp eq i64 %"%byte_idx.load38", 5
  br i1 %cmpeqtmp39, label %then_130, label %merge_131

merge_131:                                        ; preds = %merge_129
  br label %merge_119

merge_133:                                        ; preds = %merge_145, %merge_119
  %"%d.load43" = load i64, ptr %d1, align 8
  %cmpeqtmp44 = icmp eq i64 %"%d.load43", 3
  br i1 %cmpeqtmp44, label %then_146, label %merge_147

merge_135:                                        ; preds = %then_132
  %"%byte_idx.load46" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp47 = icmp eq i64 %"%byte_idx.load46", 1
  br i1 %cmpeqtmp47, label %then_136, label %merge_137

merge_137:                                        ; preds = %merge_135
  %"%byte_idx.load49" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp50 = icmp eq i64 %"%byte_idx.load49", 2
  br i1 %cmpeqtmp50, label %then_138, label %merge_139

merge_139:                                        ; preds = %merge_137
  %"%byte_idx.load52" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp53 = icmp eq i64 %"%byte_idx.load52", 3
  br i1 %cmpeqtmp53, label %then_140, label %merge_141

merge_141:                                        ; preds = %merge_139
  %"%byte_idx.load55" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp56 = icmp eq i64 %"%byte_idx.load55", 4
  br i1 %cmpeqtmp56, label %then_142, label %merge_143

merge_143:                                        ; preds = %merge_141
  %"%byte_idx.load58" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp59 = icmp eq i64 %"%byte_idx.load58", 5
  br i1 %cmpeqtmp59, label %then_144, label %merge_145

merge_145:                                        ; preds = %merge_143
  br label %merge_133

merge_147:                                        ; preds = %merge_159, %merge_133
  ret i64 0

merge_149:                                        ; preds = %then_146
  %"%byte_idx.load64" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp65 = icmp eq i64 %"%byte_idx.load64", 1
  br i1 %cmpeqtmp65, label %then_150, label %merge_151

merge_151:                                        ; preds = %merge_149
  %"%byte_idx.load67" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp68 = icmp eq i64 %"%byte_idx.load67", 2
  br i1 %cmpeqtmp68, label %then_152, label %merge_153

merge_153:                                        ; preds = %merge_151
  %"%byte_idx.load70" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp71 = icmp eq i64 %"%byte_idx.load70", 3
  br i1 %cmpeqtmp71, label %then_154, label %merge_155

merge_155:                                        ; preds = %merge_153
  %"%byte_idx.load73" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp74 = icmp eq i64 %"%byte_idx.load73", 4
  br i1 %cmpeqtmp74, label %then_156, label %merge_157

merge_157:                                        ; preds = %merge_155
  %"%byte_idx.load76" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp77 = icmp eq i64 %"%byte_idx.load76", 5
  br i1 %cmpeqtmp77, label %then_158, label %merge_159

merge_159:                                        ; preds = %merge_157
  br label %merge_147

then_104:                                         ; preds = %entry
  %"%byte_idx.load" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp3 = icmp eq i64 %"%byte_idx.load", 0
  br i1 %cmpeqtmp3, label %then_106, label %merge_107

then_106:                                         ; preds = %then_104
  %loadtmp = load i64, ptr @mac_0_0, align 8
  ret i64 %loadtmp

then_108:                                         ; preds = %merge_107
  %loadtmp8 = load i64, ptr @mac_0_1, align 8
  ret i64 %loadtmp8

then_110:                                         ; preds = %merge_109
  %loadtmp11 = load i64, ptr @mac_0_2, align 8
  ret i64 %loadtmp11

then_112:                                         ; preds = %merge_111
  %loadtmp14 = load i64, ptr @mac_0_3, align 8
  ret i64 %loadtmp14

then_114:                                         ; preds = %merge_113
  %loadtmp17 = load i64, ptr @mac_0_4, align 8
  ret i64 %loadtmp17

then_116:                                         ; preds = %merge_115
  %loadtmp20 = load i64, ptr @mac_0_5, align 8
  ret i64 %loadtmp20

then_118:                                         ; preds = %merge_105
  %"%byte_idx.load21" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp22 = icmp eq i64 %"%byte_idx.load21", 0
  br i1 %cmpeqtmp22, label %then_120, label %merge_121

then_120:                                         ; preds = %then_118
  %loadtmp25 = load i64, ptr @mac_1_0, align 8
  ret i64 %loadtmp25

then_122:                                         ; preds = %merge_121
  %loadtmp28 = load i64, ptr @mac_1_1, align 8
  ret i64 %loadtmp28

then_124:                                         ; preds = %merge_123
  %loadtmp31 = load i64, ptr @mac_1_2, align 8
  ret i64 %loadtmp31

then_126:                                         ; preds = %merge_125
  %loadtmp34 = load i64, ptr @mac_1_3, align 8
  ret i64 %loadtmp34

then_128:                                         ; preds = %merge_127
  %loadtmp37 = load i64, ptr @mac_1_4, align 8
  ret i64 %loadtmp37

then_130:                                         ; preds = %merge_129
  %loadtmp40 = load i64, ptr @mac_1_5, align 8
  ret i64 %loadtmp40

then_132:                                         ; preds = %merge_119
  %"%byte_idx.load41" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp42 = icmp eq i64 %"%byte_idx.load41", 0
  br i1 %cmpeqtmp42, label %then_134, label %merge_135

then_134:                                         ; preds = %then_132
  %loadtmp45 = load i64, ptr @mac_2_0, align 8
  ret i64 %loadtmp45

then_136:                                         ; preds = %merge_135
  %loadtmp48 = load i64, ptr @mac_2_1, align 8
  ret i64 %loadtmp48

then_138:                                         ; preds = %merge_137
  %loadtmp51 = load i64, ptr @mac_2_2, align 8
  ret i64 %loadtmp51

then_140:                                         ; preds = %merge_139
  %loadtmp54 = load i64, ptr @mac_2_3, align 8
  ret i64 %loadtmp54

then_142:                                         ; preds = %merge_141
  %loadtmp57 = load i64, ptr @mac_2_4, align 8
  ret i64 %loadtmp57

then_144:                                         ; preds = %merge_143
  %loadtmp60 = load i64, ptr @mac_2_5, align 8
  ret i64 %loadtmp60

then_146:                                         ; preds = %merge_133
  %"%byte_idx.load61" = load i64, ptr %byte_idx2, align 8
  %cmpeqtmp62 = icmp eq i64 %"%byte_idx.load61", 0
  br i1 %cmpeqtmp62, label %then_148, label %merge_149

then_148:                                         ; preds = %then_146
  %loadtmp63 = load i64, ptr @mac_3_0, align 8
  ret i64 %loadtmp63

then_150:                                         ; preds = %merge_149
  %loadtmp66 = load i64, ptr @mac_3_1, align 8
  ret i64 %loadtmp66

then_152:                                         ; preds = %merge_151
  %loadtmp69 = load i64, ptr @mac_3_2, align 8
  ret i64 %loadtmp69

then_154:                                         ; preds = %merge_153
  %loadtmp72 = load i64, ptr @mac_3_3, align 8
  ret i64 %loadtmp72

then_156:                                         ; preds = %merge_155
  %loadtmp75 = load i64, ptr @mac_3_4, align 8
  ret i64 %loadtmp75

then_158:                                         ; preds = %merge_157
  %loadtmp78 = load i64, ptr @mac_3_5, align 8
  ret i64 %loadtmp78
}

define i64 @cdc_set_mac(i64 %d, i64 %b0, i64 %b1, i64 %b2, i64 %b3, i64 %b4, i64 %b5) {
entry:
  %"%mac_3_5" = alloca i64, align 8
  %"%mac_3_4" = alloca i64, align 8
  %"%mac_3_3" = alloca i64, align 8
  %"%mac_3_2" = alloca i64, align 8
  %"%mac_3_1" = alloca i64, align 8
  %"%mac_3_0" = alloca i64, align 8
  %"%mac_2_5" = alloca i64, align 8
  %"%mac_2_4" = alloca i64, align 8
  %"%mac_2_3" = alloca i64, align 8
  %"%mac_2_2" = alloca i64, align 8
  %"%mac_2_1" = alloca i64, align 8
  %"%mac_2_0" = alloca i64, align 8
  %"%mac_1_5" = alloca i64, align 8
  %"%mac_1_4" = alloca i64, align 8
  %"%mac_1_3" = alloca i64, align 8
  %"%mac_1_2" = alloca i64, align 8
  %"%mac_1_1" = alloca i64, align 8
  %"%mac_1_0" = alloca i64, align 8
  %"%mac_0_5" = alloca i64, align 8
  %"%mac_0_4" = alloca i64, align 8
  %"%mac_0_3" = alloca i64, align 8
  %"%mac_0_2" = alloca i64, align 8
  %"%mac_0_1" = alloca i64, align 8
  %"%mac_0_0" = alloca i64, align 8
  %b57 = alloca i64, align 8
  store i64 %b5, ptr %b57, align 8
  %b46 = alloca i64, align 8
  store i64 %b4, ptr %b46, align 8
  %b35 = alloca i64, align 8
  store i64 %b3, ptr %b35, align 8
  %b24 = alloca i64, align 8
  store i64 %b2, ptr %b24, align 8
  %b13 = alloca i64, align 8
  store i64 %b1, ptr %b13, align 8
  %b02 = alloca i64, align 8
  store i64 %b0, ptr %b02, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_160, label %merge_161

merge_161:                                        ; preds = %then_160, %entry
  %"%d.load8" = load i64, ptr %d1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 1
  br i1 %cmpeqtmp9, label %then_162, label %merge_163

merge_163:                                        ; preds = %then_162, %merge_161
  %"%d.load10" = load i64, ptr %d1, align 8
  %cmpeqtmp11 = icmp eq i64 %"%d.load10", 2
  br i1 %cmpeqtmp11, label %then_164, label %merge_165

merge_165:                                        ; preds = %then_164, %merge_163
  %"%d.load12" = load i64, ptr %d1, align 8
  %cmpeqtmp13 = icmp eq i64 %"%d.load12", 3
  br i1 %cmpeqtmp13, label %then_166, label %merge_167

merge_167:                                        ; preds = %then_166, %merge_165
  ret i64 0

then_160:                                         ; preds = %entry
  %"%418.load" = load i64, ptr %b02, align 8
  store i64 %"%418.load", ptr %"%mac_0_0", align 8
  %"%420.load" = load i64, ptr %b13, align 8
  store i64 %"%420.load", ptr %"%mac_0_1", align 8
  %"%422.load" = load i64, ptr %b24, align 8
  store i64 %"%422.load", ptr %"%mac_0_2", align 8
  %"%424.load" = load i64, ptr %b35, align 8
  store i64 %"%424.load", ptr %"%mac_0_3", align 8
  %"%426.load" = load i64, ptr %b46, align 8
  store i64 %"%426.load", ptr %"%mac_0_4", align 8
  %"%428.load" = load i64, ptr %b57, align 8
  store i64 %"%428.load", ptr %"%mac_0_5", align 8
  br label %merge_161

then_162:                                         ; preds = %merge_161
  %"%432.load" = load i64, ptr %b02, align 8
  store i64 %"%432.load", ptr %"%mac_1_0", align 8
  %"%434.load" = load i64, ptr %b13, align 8
  store i64 %"%434.load", ptr %"%mac_1_1", align 8
  %"%436.load" = load i64, ptr %b24, align 8
  store i64 %"%436.load", ptr %"%mac_1_2", align 8
  %"%438.load" = load i64, ptr %b35, align 8
  store i64 %"%438.load", ptr %"%mac_1_3", align 8
  %"%440.load" = load i64, ptr %b46, align 8
  store i64 %"%440.load", ptr %"%mac_1_4", align 8
  %"%442.load" = load i64, ptr %b57, align 8
  store i64 %"%442.load", ptr %"%mac_1_5", align 8
  br label %merge_163

then_164:                                         ; preds = %merge_163
  %"%446.load" = load i64, ptr %b02, align 8
  store i64 %"%446.load", ptr %"%mac_2_0", align 8
  %"%448.load" = load i64, ptr %b13, align 8
  store i64 %"%448.load", ptr %"%mac_2_1", align 8
  %"%450.load" = load i64, ptr %b24, align 8
  store i64 %"%450.load", ptr %"%mac_2_2", align 8
  %"%452.load" = load i64, ptr %b35, align 8
  store i64 %"%452.load", ptr %"%mac_2_3", align 8
  %"%454.load" = load i64, ptr %b46, align 8
  store i64 %"%454.load", ptr %"%mac_2_4", align 8
  %"%456.load" = load i64, ptr %b57, align 8
  store i64 %"%456.load", ptr %"%mac_2_5", align 8
  br label %merge_165

then_166:                                         ; preds = %merge_165
  %"%460.load" = load i64, ptr %b02, align 8
  store i64 %"%460.load", ptr %"%mac_3_0", align 8
  %"%462.load" = load i64, ptr %b13, align 8
  store i64 %"%462.load", ptr %"%mac_3_1", align 8
  %"%464.load" = load i64, ptr %b24, align 8
  store i64 %"%464.load", ptr %"%mac_3_2", align 8
  %"%466.load" = load i64, ptr %b35, align 8
  store i64 %"%466.load", ptr %"%mac_3_3", align 8
  %"%468.load" = load i64, ptr %b46, align 8
  store i64 %"%468.load", ptr %"%mac_3_4", align 8
  %"%470.load" = load i64, ptr %b57, align 8
  store i64 %"%470.load", ptr %"%mac_3_5", align 8
  br label %merge_167
}

define i64 @cdc_print_mac(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %calltmp = call i64 @cdc_get_mac(i64 %"%d.load", i64 0)
  %calltmp2 = call i64 @cdc_serial_hex8(i64 %calltmp)
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 58)
  %"%d.load3" = load i64, ptr %d1, align 8
  %calltmp4 = call i64 @cdc_get_mac(i64 %"%d.load3", i64 1)
  %calltmp5 = call i64 @cdc_serial_hex8(i64 %calltmp4)
  %"%COM1.load6" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load6", i64 58)
  %"%d.load7" = load i64, ptr %d1, align 8
  %calltmp8 = call i64 @cdc_get_mac(i64 %"%d.load7", i64 2)
  %calltmp9 = call i64 @cdc_serial_hex8(i64 %calltmp8)
  %"%COM1.load10" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load10", i64 58)
  %"%d.load11" = load i64, ptr %d1, align 8
  %calltmp12 = call i64 @cdc_get_mac(i64 %"%d.load11", i64 3)
  %calltmp13 = call i64 @cdc_serial_hex8(i64 %calltmp12)
  %"%COM1.load14" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load14", i64 58)
  %"%d.load15" = load i64, ptr %d1, align 8
  %calltmp16 = call i64 @cdc_get_mac(i64 %"%d.load15", i64 4)
  %calltmp17 = call i64 @cdc_serial_hex8(i64 %calltmp16)
  %"%COM1.load18" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load18", i64 58)
  %"%d.load19" = load i64, ptr %d1, align 8
  %calltmp20 = call i64 @cdc_get_mac(i64 %"%d.load19", i64 5)
  %calltmp21 = call i64 @cdc_serial_hex8(i64 %calltmp20)
  ret i64 0
}

define i64 @cdc_register_device(i64 %usb_addr, i64 %subclass, i64 %ctrl_iface, i64 %data_iface, i64 %ep_in, i64 %ep_out, i64 %ep_int) {
entry:
  %"%cdc_device_count" = alloca i64, align 8
  %"%cdc_tx_buf_3" = alloca i64, align 8
  %"%cdc_rx_buf_3" = alloca i64, align 8
  %"%cdc_tx_buf_2" = alloca i64, align 8
  %"%cdc_rx_buf_2" = alloca i64, align 8
  %"%cdc_tx_buf_1" = alloca i64, align 8
  %"%cdc_rx_buf_1" = alloca i64, align 8
  %"%cdc_tx_buf_0" = alloca i64, align 8
  %"%cdc_rx_buf_0" = alloca i64, align 8
  %"%cdc_ep_int_3" = alloca i64, align 8
  %"%cdc_ep_out_3" = alloca i64, align 8
  %"%cdc_ep_in_3" = alloca i64, align 8
  %"%cdc_data_iface_3" = alloca i64, align 8
  %"%cdc_ctrl_iface_3" = alloca i64, align 8
  %"%cdc_subclass_3" = alloca i64, align 8
  %"%cdc_addr_3" = alloca i64, align 8
  %"%cdc_ep_int_2" = alloca i64, align 8
  %"%cdc_ep_out_2" = alloca i64, align 8
  %"%cdc_ep_in_2" = alloca i64, align 8
  %"%cdc_data_iface_2" = alloca i64, align 8
  %"%cdc_ctrl_iface_2" = alloca i64, align 8
  %"%cdc_subclass_2" = alloca i64, align 8
  %"%cdc_addr_2" = alloca i64, align 8
  %"%cdc_ep_int_1" = alloca i64, align 8
  %"%cdc_ep_out_1" = alloca i64, align 8
  %"%cdc_ep_in_1" = alloca i64, align 8
  %"%cdc_data_iface_1" = alloca i64, align 8
  %"%cdc_ctrl_iface_1" = alloca i64, align 8
  %"%cdc_subclass_1" = alloca i64, align 8
  %"%cdc_addr_1" = alloca i64, align 8
  %"%cdc_ep_int_0" = alloca i64, align 8
  %"%cdc_ep_out_0" = alloca i64, align 8
  %"%cdc_ep_in_0" = alloca i64, align 8
  %"%cdc_data_iface_0" = alloca i64, align 8
  %"%cdc_ctrl_iface_0" = alloca i64, align 8
  %"%cdc_subclass_0" = alloca i64, align 8
  %"%cdc_addr_0" = alloca i64, align 8
  %"%idx" = alloca i64, align 8
  %ep_int7 = alloca i64, align 8
  store i64 %ep_int, ptr %ep_int7, align 8
  %ep_out6 = alloca i64, align 8
  store i64 %ep_out, ptr %ep_out6, align 8
  %ep_in5 = alloca i64, align 8
  store i64 %ep_in, ptr %ep_in5, align 8
  %data_iface4 = alloca i64, align 8
  store i64 %data_iface, ptr %data_iface4, align 8
  %ctrl_iface3 = alloca i64, align 8
  store i64 %ctrl_iface, ptr %ctrl_iface3, align 8
  %subclass2 = alloca i64, align 8
  store i64 %subclass, ptr %subclass2, align 8
  %usb_addr1 = alloca i64, align 8
  store i64 %usb_addr, ptr %usb_addr1, align 8
  %"%514.load" = load i64, ptr @cdc_device_count, align 8
  store i64 %"%514.load", ptr %"%idx", align 8
  %"%idx.load" = load i64, ptr %"%idx", align 8
  %cmpgetmp = icmp sge i64 %"%idx.load", 4
  br i1 %cmpgetmp, label %then_168, label %merge_169

merge_169:                                        ; preds = %entry
  %"%idx.load8" = load i64, ptr %"%idx", align 8
  %calltmp = call i64 @cdc_set_present(i64 %"%idx.load8", i64 1)
  %"%idx.load9" = load i64, ptr %"%idx", align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load9", 0
  br i1 %cmpeqtmp, label %then_170, label %merge_171

merge_171:                                        ; preds = %then_170, %merge_169
  %"%idx.load10" = load i64, ptr %"%idx", align 8
  %cmpeqtmp11 = icmp eq i64 %"%idx.load10", 1
  br i1 %cmpeqtmp11, label %then_172, label %merge_173

merge_173:                                        ; preds = %then_172, %merge_171
  %"%idx.load12" = load i64, ptr %"%idx", align 8
  %cmpeqtmp13 = icmp eq i64 %"%idx.load12", 2
  br i1 %cmpeqtmp13, label %then_174, label %merge_175

merge_175:                                        ; preds = %then_174, %merge_173
  %"%idx.load14" = load i64, ptr %"%idx", align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 3
  br i1 %cmpeqtmp15, label %then_176, label %merge_177

merge_177:                                        ; preds = %then_176, %merge_175
  %"%idx.load16" = load i64, ptr %"%idx", align 8
  %cmpeqtmp17 = icmp eq i64 %"%idx.load16", 0
  br i1 %cmpeqtmp17, label %then_178, label %merge_179

merge_179:                                        ; preds = %then_178, %merge_177
  %"%idx.load21" = load i64, ptr %"%idx", align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 1
  br i1 %cmpeqtmp22, label %then_180, label %merge_181

merge_181:                                        ; preds = %then_180, %merge_179
  %"%idx.load27" = load i64, ptr %"%idx", align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 2
  br i1 %cmpeqtmp28, label %then_182, label %merge_183

merge_183:                                        ; preds = %then_182, %merge_181
  %"%idx.load33" = load i64, ptr %"%idx", align 8
  %cmpeqtmp34 = icmp eq i64 %"%idx.load33", 3
  br i1 %cmpeqtmp34, label %then_184, label %merge_185

merge_185:                                        ; preds = %then_184, %merge_183
  %calltmp39 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %"%638.load" = load i64, ptr %"%idx", align 8
  %calltmp40 = call i64 @"cdc_serial_\D8\B1\D9\82\D9\85"(i64 %"%638.load")
  %calltmp41 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %"%643.load" = load i64, ptr %usb_addr1, align 8
  %calltmp42 = call i64 @"cdc_serial_\D8\B1\D9\82\D9\85"(i64 %"%643.load")
  %calltmp43 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %"%648.load" = load i64, ptr %subclass2, align 8
  %calltmp44 = call i64 @"cdc_serial_\D8\B1\D9\82\D9\85"(i64 %"%648.load")
  %calltmp45 = call i64 @"cdc_serial_\D8\B3\D8\B7\D8\B1"()
  %"%cdc_device_count.load" = load i64, ptr @cdc_device_count, align 8
  %addtmp = add i64 %"%cdc_device_count.load", 1
  store i64 %addtmp, ptr %"%cdc_device_count", align 8
  %loadtmp = load i64, ptr %"%idx", align 8
  ret i64 %loadtmp

then_168:                                         ; preds = %entry
  ret i64 -1

then_170:                                         ; preds = %merge_169
  %"%526.load" = load i64, ptr %usb_addr1, align 8
  store i64 %"%526.load", ptr %"%cdc_addr_0", align 8
  %"%528.load" = load i64, ptr %subclass2, align 8
  store i64 %"%528.load", ptr %"%cdc_subclass_0", align 8
  %"%530.load" = load i64, ptr %ctrl_iface3, align 8
  store i64 %"%530.load", ptr %"%cdc_ctrl_iface_0", align 8
  %"%532.load" = load i64, ptr %data_iface4, align 8
  store i64 %"%532.load", ptr %"%cdc_data_iface_0", align 8
  %"%534.load" = load i64, ptr %ep_in5, align 8
  store i64 %"%534.load", ptr %"%cdc_ep_in_0", align 8
  %"%536.load" = load i64, ptr %ep_out6, align 8
  store i64 %"%536.load", ptr %"%cdc_ep_out_0", align 8
  %"%538.load" = load i64, ptr %ep_int7, align 8
  store i64 %"%538.load", ptr %"%cdc_ep_int_0", align 8
  br label %merge_171

then_172:                                         ; preds = %merge_171
  %"%542.load" = load i64, ptr %usb_addr1, align 8
  store i64 %"%542.load", ptr %"%cdc_addr_1", align 8
  %"%544.load" = load i64, ptr %subclass2, align 8
  store i64 %"%544.load", ptr %"%cdc_subclass_1", align 8
  %"%546.load" = load i64, ptr %ctrl_iface3, align 8
  store i64 %"%546.load", ptr %"%cdc_ctrl_iface_1", align 8
  %"%548.load" = load i64, ptr %data_iface4, align 8
  store i64 %"%548.load", ptr %"%cdc_data_iface_1", align 8
  %"%550.load" = load i64, ptr %ep_in5, align 8
  store i64 %"%550.load", ptr %"%cdc_ep_in_1", align 8
  %"%552.load" = load i64, ptr %ep_out6, align 8
  store i64 %"%552.load", ptr %"%cdc_ep_out_1", align 8
  %"%554.load" = load i64, ptr %ep_int7, align 8
  store i64 %"%554.load", ptr %"%cdc_ep_int_1", align 8
  br label %merge_173

then_174:                                         ; preds = %merge_173
  %"%558.load" = load i64, ptr %usb_addr1, align 8
  store i64 %"%558.load", ptr %"%cdc_addr_2", align 8
  %"%560.load" = load i64, ptr %subclass2, align 8
  store i64 %"%560.load", ptr %"%cdc_subclass_2", align 8
  %"%562.load" = load i64, ptr %ctrl_iface3, align 8
  store i64 %"%562.load", ptr %"%cdc_ctrl_iface_2", align 8
  %"%564.load" = load i64, ptr %data_iface4, align 8
  store i64 %"%564.load", ptr %"%cdc_data_iface_2", align 8
  %"%566.load" = load i64, ptr %ep_in5, align 8
  store i64 %"%566.load", ptr %"%cdc_ep_in_2", align 8
  %"%568.load" = load i64, ptr %ep_out6, align 8
  store i64 %"%568.load", ptr %"%cdc_ep_out_2", align 8
  %"%570.load" = load i64, ptr %ep_int7, align 8
  store i64 %"%570.load", ptr %"%cdc_ep_int_2", align 8
  br label %merge_175

then_176:                                         ; preds = %merge_175
  %"%574.load" = load i64, ptr %usb_addr1, align 8
  store i64 %"%574.load", ptr %"%cdc_addr_3", align 8
  %"%576.load" = load i64, ptr %subclass2, align 8
  store i64 %"%576.load", ptr %"%cdc_subclass_3", align 8
  %"%578.load" = load i64, ptr %ctrl_iface3, align 8
  store i64 %"%578.load", ptr %"%cdc_ctrl_iface_3", align 8
  %"%580.load" = load i64, ptr %data_iface4, align 8
  store i64 %"%580.load", ptr %"%cdc_data_iface_3", align 8
  %"%582.load" = load i64, ptr %ep_in5, align 8
  store i64 %"%582.load", ptr %"%cdc_ep_in_3", align 8
  %"%584.load" = load i64, ptr %ep_out6, align 8
  store i64 %"%584.load", ptr %"%cdc_ep_out_3", align 8
  %"%586.load" = load i64, ptr %ep_int7, align 8
  store i64 %"%586.load", ptr %"%cdc_ep_int_3", align 8
  br label %merge_177

then_178:                                         ; preds = %merge_177
  %"%CDC_MAX_SEGMENT.load" = load i64, ptr @CDC_MAX_SEGMENT, align 8
  call void @sad_ll_kmalloc(i64 %"%CDC_MAX_SEGMENT.load")
  store i64 0, ptr %"%cdc_rx_buf_0", align 8
  %"%CDC_MAX_SEGMENT.load18" = load i64, ptr @CDC_MAX_SEGMENT, align 8
  call void @sad_ll_kmalloc(i64 %"%CDC_MAX_SEGMENT.load18")
  store i64 0, ptr %"%cdc_tx_buf_0", align 8
  %"%cdc_rx_buf_0.load" = load i64, ptr %"%cdc_rx_buf_0", align 8
  %"%CDC_MAX_SEGMENT.load19" = load i64, ptr @CDC_MAX_SEGMENT, align 8
  call void @sad_ll_memset(i64 %"%cdc_rx_buf_0.load", i64 0, i64 %"%CDC_MAX_SEGMENT.load19")
  %"%cdc_tx_buf_0.load" = load i64, ptr %"%cdc_tx_buf_0", align 8
  %"%CDC_MAX_SEGMENT.load20" = load i64, ptr @CDC_MAX_SEGMENT, align 8
  call void @sad_ll_memset(i64 %"%cdc_tx_buf_0.load", i64 0, i64 %"%CDC_MAX_SEGMENT.load20")
  br label %merge_179

then_180:                                         ; preds = %merge_179
  %"%CDC_MAX_SEGMENT.load23" = load i64, ptr @CDC_MAX_SEGMENT, align 8
  call void @sad_ll_kmalloc(i64 %"%CDC_MAX_SEGMENT.load23")
  store i64 0, ptr %"%cdc_rx_buf_1", align 8
  %"%CDC_MAX_SEGMENT.load24" = load i64, ptr @CDC_MAX_SEGMENT, align 8
  call void @sad_ll_kmalloc(i64 %"%CDC_MAX_SEGMENT.load24")
  store i64 0, ptr %"%cdc_tx_buf_1", align 8
  %"%cdc_rx_buf_1.load" = load i64, ptr %"%cdc_rx_buf_1", align 8
  %"%CDC_MAX_SEGMENT.load25" = load i64, ptr @CDC_MAX_SEGMENT, align 8
  call void @sad_ll_memset(i64 %"%cdc_rx_buf_1.load", i64 0, i64 %"%CDC_MAX_SEGMENT.load25")
  %"%cdc_tx_buf_1.load" = load i64, ptr %"%cdc_tx_buf_1", align 8
  %"%CDC_MAX_SEGMENT.load26" = load i64, ptr @CDC_MAX_SEGMENT, align 8
  call void @sad_ll_memset(i64 %"%cdc_tx_buf_1.load", i64 0, i64 %"%CDC_MAX_SEGMENT.load26")
  br label %merge_181

then_182:                                         ; preds = %merge_181
  %"%CDC_MAX_SEGMENT.load29" = load i64, ptr @CDC_MAX_SEGMENT, align 8
  call void @sad_ll_kmalloc(i64 %"%CDC_MAX_SEGMENT.load29")
  store i64 0, ptr %"%cdc_rx_buf_2", align 8
  %"%CDC_MAX_SEGMENT.load30" = load i64, ptr @CDC_MAX_SEGMENT, align 8
  call void @sad_ll_kmalloc(i64 %"%CDC_MAX_SEGMENT.load30")
  store i64 0, ptr %"%cdc_tx_buf_2", align 8
  %"%cdc_rx_buf_2.load" = load i64, ptr %"%cdc_rx_buf_2", align 8
  %"%CDC_MAX_SEGMENT.load31" = load i64, ptr @CDC_MAX_SEGMENT, align 8
  call void @sad_ll_memset(i64 %"%cdc_rx_buf_2.load", i64 0, i64 %"%CDC_MAX_SEGMENT.load31")
  %"%cdc_tx_buf_2.load" = load i64, ptr %"%cdc_tx_buf_2", align 8
  %"%CDC_MAX_SEGMENT.load32" = load i64, ptr @CDC_MAX_SEGMENT, align 8
  call void @sad_ll_memset(i64 %"%cdc_tx_buf_2.load", i64 0, i64 %"%CDC_MAX_SEGMENT.load32")
  br label %merge_183

then_184:                                         ; preds = %merge_183
  %"%CDC_MAX_SEGMENT.load35" = load i64, ptr @CDC_MAX_SEGMENT, align 8
  call void @sad_ll_kmalloc(i64 %"%CDC_MAX_SEGMENT.load35")
  store i64 0, ptr %"%cdc_rx_buf_3", align 8
  %"%CDC_MAX_SEGMENT.load36" = load i64, ptr @CDC_MAX_SEGMENT, align 8
  call void @sad_ll_kmalloc(i64 %"%CDC_MAX_SEGMENT.load36")
  store i64 0, ptr %"%cdc_tx_buf_3", align 8
  %"%cdc_rx_buf_3.load" = load i64, ptr %"%cdc_rx_buf_3", align 8
  %"%CDC_MAX_SEGMENT.load37" = load i64, ptr @CDC_MAX_SEGMENT, align 8
  call void @sad_ll_memset(i64 %"%cdc_rx_buf_3.load", i64 0, i64 %"%CDC_MAX_SEGMENT.load37")
  %"%cdc_tx_buf_3.load" = load i64, ptr %"%cdc_tx_buf_3", align 8
  %"%CDC_MAX_SEGMENT.load38" = load i64, ptr @CDC_MAX_SEGMENT, align 8
  call void @sad_ll_memset(i64 %"%cdc_tx_buf_3.load", i64 0, i64 %"%CDC_MAX_SEGMENT.load38")
  br label %merge_185
}

define i64 @cdc_activate(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %calltmp = call i64 @cdc_get_present(i64 %"%d.load")
  %cmpeqtmp = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp, label %then_186, label %merge_187

merge_187:                                        ; preds = %entry
  %"%d.load2" = load i64, ptr %d1, align 8
  %calltmp3 = call i64 @cdc_set_active(i64 %"%d.load2", i64 1)
  %"%d.load4" = load i64, ptr %d1, align 8
  %"%LINK_UP.load" = load i64, ptr @LINK_UP, align 8
  %calltmp5 = call i64 @cdc_set_link(i64 %"%d.load4", i64 %"%LINK_UP.load")
  %"%d.load6" = load i64, ptr %d1, align 8
  %calltmp7 = call i64 @cdc_set_speed(i64 %"%d.load6", i64 100)
  %calltmp8 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %"%671.load" = load i64, ptr %d1, align 8
  %calltmp9 = call i64 @"cdc_serial_\D8\B1\D9\82\D9\85"(i64 %"%671.load")
  %calltmp10 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%d.load11" = load i64, ptr %d1, align 8
  %calltmp12 = call i64 @cdc_print_mac(i64 %"%d.load11")
  %calltmp13 = call i64 @"cdc_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_186:                                         ; preds = %entry
  ret i64 -1
}

define i64 @cdc_deactivate(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %calltmp = call i64 @cdc_set_active(i64 %"%d.load", i64 0)
  %"%d.load2" = load i64, ptr %d1, align 8
  %"%LINK_DOWN.load" = load i64, ptr @LINK_DOWN, align 8
  %calltmp3 = call i64 @cdc_set_link(i64 %"%d.load2", i64 %"%LINK_DOWN.load")
  %calltmp4 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %"%684.load" = load i64, ptr %d1, align 8
  %calltmp5 = call i64 @"cdc_serial_\D8\B1\D9\82\D9\85"(i64 %"%684.load")
  %calltmp6 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  %calltmp7 = call i64 @"cdc_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @cdc_tx_packet(i64 %d, i64 %buf, i64 %length) {
entry:
  %"%cdc_tx_bytes" = alloca i64, align 8
  %"%cdc_tx_packets" = alloca i64, align 8
  %"%b" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %"%tx_buf" = alloca i64, align 8
  %"%cdc_errors" = alloca i64, align 8
  %length3 = alloca i64, align 8
  store i64 %length, ptr %length3, align 8
  %buf2 = alloca i64, align 8
  store i64 %buf, ptr %buf2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %calltmp = call i64 @cdc_get_active(i64 %"%d.load")
  %cmpeqtmp = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp, label %then_188, label %merge_189

merge_189:                                        ; preds = %entry
  %"%d.load4" = load i64, ptr %d1, align 8
  %calltmp5 = call i64 @cdc_get_link(i64 %"%d.load4")
  %cmpeqtmp6 = icmp eq i64 %calltmp5, 0
  br i1 %cmpeqtmp6, label %then_190, label %merge_191

merge_191:                                        ; preds = %merge_189
  %"%length.load" = load i64, ptr %length3, align 8
  %"%ETH_MAX_FRAME.load" = load i64, ptr @ETH_MAX_FRAME, align 8
  %cmpgttmp = icmp sgt i64 %"%length.load", %"%ETH_MAX_FRAME.load"
  br i1 %cmpgttmp, label %then_192, label %merge_193

merge_193:                                        ; preds = %merge_191
  %"%length.load7" = load i64, ptr %length3, align 8
  %cmplttmp = icmp slt i64 %"%length.load7", 14
  br i1 %cmplttmp, label %then_194, label %merge_195

merge_195:                                        ; preds = %merge_193
  store i64 0, ptr %"%tx_buf", align 8
  %"%d.load10" = load i64, ptr %d1, align 8
  %cmpeqtmp11 = icmp eq i64 %"%d.load10", 0
  br i1 %cmpeqtmp11, label %then_196, label %merge_197

merge_197:                                        ; preds = %then_196, %merge_195
  %"%d.load12" = load i64, ptr %d1, align 8
  %cmpeqtmp13 = icmp eq i64 %"%d.load12", 1
  br i1 %cmpeqtmp13, label %then_198, label %merge_199

merge_199:                                        ; preds = %then_198, %merge_197
  %"%d.load14" = load i64, ptr %d1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%d.load14", 2
  br i1 %cmpeqtmp15, label %then_200, label %merge_201

merge_201:                                        ; preds = %then_200, %merge_199
  %"%d.load16" = load i64, ptr %d1, align 8
  %cmpeqtmp17 = icmp eq i64 %"%d.load16", 3
  br i1 %cmpeqtmp17, label %then_202, label %merge_203

merge_203:                                        ; preds = %then_202, %merge_201
  store i64 0, ptr %"%i", align 8
  br label %while_cond_204

then_188:                                         ; preds = %entry
  ret i64 -1

then_190:                                         ; preds = %merge_189
  ret i64 -2

then_192:                                         ; preds = %merge_191
  %"%cdc_errors.load" = load i64, ptr @cdc_errors, align 8
  %addtmp = add i64 %"%cdc_errors.load", 1
  store i64 %addtmp, ptr %"%cdc_errors", align 8
  ret i64 -3

then_194:                                         ; preds = %merge_193
  %"%cdc_errors.load8" = load i64, ptr %"%cdc_errors", align 8
  %addtmp9 = add i64 %"%cdc_errors.load8", 1
  store i64 %addtmp9, ptr %"%cdc_errors", align 8
  ret i64 -4

then_196:                                         ; preds = %merge_195
  %"%725.load" = load i64, ptr @cdc_tx_buf_0, align 8
  store i64 %"%725.load", ptr %"%tx_buf", align 8
  br label %merge_197

then_198:                                         ; preds = %merge_197
  %"%729.load" = load i64, ptr @cdc_tx_buf_1, align 8
  store i64 %"%729.load", ptr %"%tx_buf", align 8
  br label %merge_199

then_200:                                         ; preds = %merge_199
  %"%733.load" = load i64, ptr @cdc_tx_buf_2, align 8
  store i64 %"%733.load", ptr %"%tx_buf", align 8
  br label %merge_201

then_202:                                         ; preds = %merge_201
  %"%737.load" = load i64, ptr @cdc_tx_buf_3, align 8
  store i64 %"%737.load", ptr %"%tx_buf", align 8
  br label %merge_203

while_body_205:                                   ; preds = %while_cond_204
  %"%buf.load" = load i64, ptr %buf2, align 8
  %"%i.load20" = load i64, ptr %"%i", align 8
  %addtmp21 = add i64 %"%buf.load", %"%i.load20"
  call void @sad_ll_mem_read8(i64 %addtmp21)
  store i64 0, ptr %"%b", align 8
  %"%tx_buf.load" = load i64, ptr %"%tx_buf", align 8
  %"%i.load22" = load i64, ptr %"%i", align 8
  %addtmp23 = add i64 %"%tx_buf.load", %"%i.load22"
  %"%b.load" = load i64, ptr %"%b", align 8
  call void @sad_ll_mem_write8(i64 %addtmp23, i64 %"%b.load")
  %"%i.load24" = load i64, ptr %"%i", align 8
  %addtmp25 = add i64 %"%i.load24", 1
  store i64 %addtmp25, ptr %"%i", align 8
  br label %while_cond_204

while_cond_204:                                   ; preds = %while_body_205, %merge_203
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%length.load18" = load i64, ptr %length3, align 8
  %cmplttmp19 = icmp slt i64 %"%i.load", %"%length.load18"
  br i1 %cmplttmp19, label %while_body_205, label %while_exit_206

while_exit_206:                                   ; preds = %while_cond_204
  %"%cdc_tx_packets.load" = load i64, ptr @cdc_tx_packets, align 8
  %addtmp26 = add i64 %"%cdc_tx_packets.load", 1
  store i64 %addtmp26, ptr %"%cdc_tx_packets", align 8
  %"%cdc_tx_bytes.load" = load i64, ptr @cdc_tx_bytes, align 8
  %"%length.load27" = load i64, ptr %length3, align 8
  %addtmp28 = add i64 %"%cdc_tx_bytes.load", %"%length.load27"
  store i64 %addtmp28, ptr %"%cdc_tx_bytes", align 8
  %loadtmp = load i64, ptr %length3, align 8
  ret i64 %loadtmp
}

define i64 @cdc_rx_packet(i64 %d, i64 %buf, i64 %max_len) {
entry:
  %max_len3 = alloca i64, align 8
  store i64 %max_len, ptr %max_len3, align 8
  %buf2 = alloca i64, align 8
  store i64 %buf, ptr %buf2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %calltmp = call i64 @cdc_get_active(i64 %"%d.load")
  %cmpeqtmp = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp, label %then_207, label %merge_208

merge_208:                                        ; preds = %entry
  %"%d.load4" = load i64, ptr %d1, align 8
  %calltmp5 = call i64 @cdc_get_link(i64 %"%d.load4")
  %cmpeqtmp6 = icmp eq i64 %calltmp5, 0
  br i1 %cmpeqtmp6, label %then_209, label %merge_210

merge_210:                                        ; preds = %merge_208
  ret i64 0

then_207:                                         ; preds = %entry
  ret i64 -1

then_209:                                         ; preds = %merge_208
  ret i64 0
}

define i64 @cdc_build_eth_header(i64 %buf, i64 %src_dev, i64 %dst_mac_buf, i64 %ethertype) {
entry:
  %"%b13" = alloca i64, align 8
  %"%b" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %ethertype4 = alloca i64, align 8
  store i64 %ethertype, ptr %ethertype4, align 8
  %dst_mac_buf3 = alloca i64, align 8
  store i64 %dst_mac_buf, ptr %dst_mac_buf3, align 8
  %src_dev2 = alloca i64, align 8
  store i64 %src_dev, ptr %src_dev2, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_211

while_body_212:                                   ; preds = %while_cond_211
  %"%dst_mac_buf.load" = load i64, ptr %dst_mac_buf3, align 8
  %"%i.load5" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%dst_mac_buf.load", %"%i.load5"
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%b", align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  %"%i.load6" = load i64, ptr %"%i", align 8
  %addtmp7 = add i64 %"%buf.load", %"%i.load6"
  %"%b.load" = load i64, ptr %"%b", align 8
  call void @sad_ll_mem_write8(i64 %addtmp7, i64 %"%b.load")
  %"%i.load8" = load i64, ptr %"%i", align 8
  %addtmp9 = add i64 %"%i.load8", 1
  store i64 %addtmp9, ptr %"%i", align 8
  br label %while_cond_211

while_body_215:                                   ; preds = %while_cond_214
  %"%src_dev.load" = load i64, ptr %src_dev2, align 8
  %"%i.load12" = load i64, ptr %"%i", align 8
  %calltmp = call i64 @cdc_get_mac(i64 %"%src_dev.load", i64 %"%i.load12")
  store i64 %calltmp, ptr %"%b13", align 8
  %"%buf.load14" = load i64, ptr %buf1, align 8
  %addtmp15 = add i64 %"%buf.load14", 6
  %"%i.load16" = load i64, ptr %"%i", align 8
  %addtmp17 = add i64 %addtmp15, %"%i.load16"
  %"%b.load18" = load i64, ptr %"%b13", align 8
  call void @sad_ll_mem_write8(i64 %addtmp17, i64 %"%b.load18")
  %"%i.load19" = load i64, ptr %"%i", align 8
  %addtmp20 = add i64 %"%i.load19", 1
  store i64 %addtmp20, ptr %"%i", align 8
  br label %while_cond_214

while_cond_211:                                   ; preds = %while_body_212, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %cmplttmp = icmp slt i64 %"%i.load", 6
  br i1 %cmplttmp, label %while_body_212, label %while_exit_213

while_cond_214:                                   ; preds = %while_body_215, %while_exit_213
  %"%i.load10" = load i64, ptr %"%i", align 8
  %cmplttmp11 = icmp slt i64 %"%i.load10", 6
  br i1 %cmplttmp11, label %while_body_215, label %while_exit_216

while_exit_213:                                   ; preds = %while_cond_211
  store i64 0, ptr %"%i", align 8
  br label %while_cond_214

while_exit_216:                                   ; preds = %while_cond_214
  %"%buf.load21" = load i64, ptr %buf1, align 8
  %addtmp22 = add i64 %"%buf.load21", 12
  %"%ethertype.load" = load i64, ptr %ethertype4, align 8
  %shrtmp = lshr i64 %"%ethertype.load", 8
  call void @sad_ll_mem_write8(i64 %addtmp22, i64 %shrtmp)
  %"%buf.load23" = load i64, ptr %buf1, align 8
  %addtmp24 = add i64 %"%buf.load23", 13
  %"%ethertype.load25" = load i64, ptr %ethertype4, align 8
  %andtmp = and i64 %"%ethertype.load25", 255
  call void @sad_ll_mem_write8(i64 %addtmp24, i64 %andtmp)
  ret i64 14
}

define i64 @cdc_find_by_subclass(i64 %subclass) {
entry:
  %"%sc" = alloca i64, align 8
  %"%d" = alloca i64, align 8
  %subclass1 = alloca i64, align 8
  store i64 %subclass, ptr %subclass1, align 8
  store i64 0, ptr %"%d", align 8
  br label %while_cond_217

merge_221:                                        ; preds = %merge_231, %while_body_218
  %"%d.load5" = load i64, ptr %"%d", align 8
  %addtmp = add i64 %"%d.load5", 1
  store i64 %addtmp, ptr %"%d", align 8
  br label %while_cond_217

merge_223:                                        ; preds = %then_222, %then_220
  %"%d.load6" = load i64, ptr %"%d", align 8
  %cmpeqtmp7 = icmp eq i64 %"%d.load6", 1
  br i1 %cmpeqtmp7, label %then_224, label %merge_225

merge_225:                                        ; preds = %then_224, %merge_223
  %"%d.load8" = load i64, ptr %"%d", align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 2
  br i1 %cmpeqtmp9, label %then_226, label %merge_227

merge_227:                                        ; preds = %then_226, %merge_225
  %"%d.load10" = load i64, ptr %"%d", align 8
  %cmpeqtmp11 = icmp eq i64 %"%d.load10", 3
  br i1 %cmpeqtmp11, label %then_228, label %merge_229

merge_229:                                        ; preds = %then_228, %merge_227
  %"%sc.load" = load i64, ptr %"%sc", align 8
  %"%subclass.load" = load i64, ptr %subclass1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%sc.load", %"%subclass.load"
  br i1 %cmpeqtmp12, label %then_230, label %merge_231

merge_231:                                        ; preds = %merge_229
  br label %merge_221

then_220:                                         ; preds = %while_body_218
  store i64 0, ptr %"%sc", align 8
  %"%d.load3" = load i64, ptr %"%d", align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 0
  br i1 %cmpeqtmp4, label %then_222, label %merge_223

then_222:                                         ; preds = %then_220
  %"%817.load" = load i64, ptr @cdc_subclass_0, align 8
  store i64 %"%817.load", ptr %"%sc", align 8
  br label %merge_223

then_224:                                         ; preds = %merge_223
  %"%821.load" = load i64, ptr @cdc_subclass_1, align 8
  store i64 %"%821.load", ptr %"%sc", align 8
  br label %merge_225

then_226:                                         ; preds = %merge_225
  %"%825.load" = load i64, ptr @cdc_subclass_2, align 8
  store i64 %"%825.load", ptr %"%sc", align 8
  br label %merge_227

then_228:                                         ; preds = %merge_227
  %"%829.load" = load i64, ptr @cdc_subclass_3, align 8
  store i64 %"%829.load", ptr %"%sc", align 8
  br label %merge_229

then_230:                                         ; preds = %merge_229
  %loadtmp = load i64, ptr %"%d", align 8
  ret i64 %loadtmp

while_body_218:                                   ; preds = %while_cond_217
  %"%d.load2" = load i64, ptr %"%d", align 8
  %calltmp = call i64 @cdc_get_present(i64 %"%d.load2")
  %cmpeqtmp = icmp eq i64 %calltmp, 1
  br i1 %cmpeqtmp, label %then_220, label %merge_221

while_cond_217:                                   ; preds = %merge_221, %entry
  %"%d.load" = load i64, ptr %"%d", align 8
  %"%cdc_device_count.load" = load i64, ptr @cdc_device_count, align 8
  %cmplttmp = icmp slt i64 %"%d.load", %"%cdc_device_count.load"
  br i1 %cmplttmp, label %while_body_218, label %while_exit_219

while_exit_219:                                   ; preds = %while_cond_217
  ret i64 -1
}

define i64 @"cdc_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %"%d" = alloca i64, align 8
  %calltmp = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %calltmp1 = call i64 @"cdc_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %"%846.load" = load i64, ptr @cdc_device_count, align 8
  %calltmp3 = call i64 @"cdc_serial_\D8\B1\D9\82\D9\85"(i64 %"%846.load")
  %calltmp4 = call i64 @"cdc_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%d", align 8
  br label %while_cond_232

else_239:                                         ; preds = %then_235
  %calltmp40 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  br label %merge_238

merge_236:                                        ; preds = %merge_238, %while_body_233
  %"%d.load34" = load i64, ptr %"%d", align 8
  %addtmp = add i64 %"%d.load34", 1
  store i64 %addtmp, ptr %"%d", align 8
  br label %while_cond_232

merge_238:                                        ; preds = %else_239, %then_237
  %calltmp41 = call i64 @"cdc_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_236

then_235:                                         ; preds = %while_body_233
  %calltmp26 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %"%859.load" = load i64, ptr %"%d", align 8
  %calltmp27 = call i64 @"cdc_serial_\D8\B1\D9\82\D9\85"(i64 %"%859.load")
  %calltmp28 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  %"%d.load29" = load i64, ptr %"%d", align 8
  %calltmp30 = call i64 @cdc_print_mac(i64 %"%d.load29")
  %"%d.load31" = load i64, ptr %"%d", align 8
  %calltmp32 = call i64 @cdc_get_link(i64 %"%d.load31")
  %cmpeqtmp33 = icmp eq i64 %calltmp32, 1
  br i1 %cmpeqtmp33, label %then_237, label %else_239

then_237:                                         ; preds = %then_235
  %calltmp35 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  %"%d.load36" = load i64, ptr %"%d", align 8
  %calltmp37 = call i64 @cdc_get_speed(i64 %"%d.load36")
  %calltmp38 = call i64 @"cdc_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp37)
  %calltmp39 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  br label %merge_238

while_body_233:                                   ; preds = %while_cond_232
  %"%d.load5" = load i64, ptr %"%d", align 8
  %calltmp6 = call i64 @cdc_get_present(i64 %"%d.load5")
  %cmpeqtmp = icmp eq i64 %calltmp6, 1
  br i1 %cmpeqtmp, label %then_235, label %merge_236

while_cond_232:                                   ; preds = %merge_236, %entry
  %"%d.load" = load i64, ptr %"%d", align 8
  %"%cdc_device_count.load" = load i64, ptr @cdc_device_count, align 8
  %cmplttmp = icmp slt i64 %"%d.load", %"%cdc_device_count.load"
  br i1 %cmplttmp, label %while_body_233, label %while_exit_234

while_exit_234:                                   ; preds = %while_cond_232
  %calltmp7 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  %"%883.load" = load i64, ptr @cdc_tx_packets, align 8
  %calltmp8 = call i64 @"cdc_serial_\D8\B1\D9\82\D9\85"(i64 %"%883.load")
  %calltmp9 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %"%888.load" = load i64, ptr @cdc_tx_bytes, align 8
  %calltmp10 = call i64 @"cdc_serial_\D8\B1\D9\82\D9\85"(i64 %"%888.load")
  %calltmp11 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %calltmp12 = call i64 @"cdc_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp13 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %"%896.load" = load i64, ptr @cdc_rx_packets, align 8
  %calltmp14 = call i64 @"cdc_serial_\D8\B1\D9\82\D9\85"(i64 %"%896.load")
  %calltmp15 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %"%901.load" = load i64, ptr @cdc_rx_bytes, align 8
  %calltmp16 = call i64 @"cdc_serial_\D8\B1\D9\82\D9\85"(i64 %"%901.load")
  %calltmp17 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %calltmp18 = call i64 @"cdc_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp19 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%909.load" = load i64, ptr @cdc_errors, align 8
  %calltmp20 = call i64 @"cdc_serial_\D8\B1\D9\82\D9\85"(i64 %"%909.load")
  %calltmp21 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  %"%914.load" = load i64, ptr @cdc_dropped, align 8
  %calltmp22 = call i64 @"cdc_serial_\D8\B1\D9\82\D9\85"(i64 %"%914.load")
  %calltmp23 = call i64 @"cdc_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp24 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %calltmp25 = call i64 @"cdc_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @"cdc_\D9\87\D9\8A\D8\A6"() {
entry:
  %calltmp = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  %calltmp1 = call i64 @"cdc_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"cdc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %calltmp3 = call i64 @"cdc_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @"cdc_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  ret i64 0
}

define void @__sad_main() {
entry:
  %"%cdc_dropped" = alloca i64, align 8
  %"%cdc_errors" = alloca i64, align 8
  %"%cdc_rx_bytes" = alloca i64, align 8
  %"%cdc_tx_bytes" = alloca i64, align 8
  %"%cdc_rx_packets" = alloca i64, align 8
  %"%cdc_tx_packets" = alloca i64, align 8
  %"%cdc_device_count" = alloca i64, align 8
  %"%cdc_tx_tail_3" = alloca i64, align 8
  %"%cdc_tx_head_3" = alloca i64, align 8
  %"%cdc_rx_tail_3" = alloca i64, align 8
  %"%cdc_rx_head_3" = alloca i64, align 8
  %"%cdc_tx_tail_2" = alloca i64, align 8
  %"%cdc_tx_head_2" = alloca i64, align 8
  %"%cdc_rx_tail_2" = alloca i64, align 8
  %"%cdc_rx_head_2" = alloca i64, align 8
  %"%cdc_tx_tail_1" = alloca i64, align 8
  %"%cdc_tx_head_1" = alloca i64, align 8
  %"%cdc_rx_tail_1" = alloca i64, align 8
  %"%cdc_rx_head_1" = alloca i64, align 8
  %"%cdc_tx_tail_0" = alloca i64, align 8
  %"%cdc_tx_head_0" = alloca i64, align 8
  %"%cdc_rx_tail_0" = alloca i64, align 8
  %"%cdc_rx_head_0" = alloca i64, align 8
  %"%cdc_tx_buf_3" = alloca i64, align 8
  %"%cdc_tx_buf_2" = alloca i64, align 8
  %"%cdc_tx_buf_1" = alloca i64, align 8
  %"%cdc_tx_buf_0" = alloca i64, align 8
  %"%cdc_rx_buf_3" = alloca i64, align 8
  %"%cdc_rx_buf_2" = alloca i64, align 8
  %"%cdc_rx_buf_1" = alloca i64, align 8
  %"%cdc_rx_buf_0" = alloca i64, align 8
  %"%mac_3_5" = alloca i64, align 8
  %"%mac_3_4" = alloca i64, align 8
  %"%mac_3_3" = alloca i64, align 8
  %"%mac_3_2" = alloca i64, align 8
  %"%mac_3_1" = alloca i64, align 8
  %"%mac_3_0" = alloca i64, align 8
  %"%mac_2_5" = alloca i64, align 8
  %"%mac_2_4" = alloca i64, align 8
  %"%mac_2_3" = alloca i64, align 8
  %"%mac_2_2" = alloca i64, align 8
  %"%mac_2_1" = alloca i64, align 8
  %"%mac_2_0" = alloca i64, align 8
  %"%mac_1_5" = alloca i64, align 8
  %"%mac_1_4" = alloca i64, align 8
  %"%mac_1_3" = alloca i64, align 8
  %"%mac_1_2" = alloca i64, align 8
  %"%mac_1_1" = alloca i64, align 8
  %"%mac_1_0" = alloca i64, align 8
  %"%mac_0_5" = alloca i64, align 8
  %"%mac_0_4" = alloca i64, align 8
  %"%mac_0_3" = alloca i64, align 8
  %"%mac_0_2" = alloca i64, align 8
  %"%mac_0_1" = alloca i64, align 8
  %"%mac_0_0" = alloca i64, align 8
  %"%cdc_filter_3" = alloca i64, align 8
  %"%cdc_filter_2" = alloca i64, align 8
  %"%cdc_filter_1" = alloca i64, align 8
  %"%cdc_filter_0" = alloca i64, align 8
  %"%cdc_active_3" = alloca i64, align 8
  %"%cdc_active_2" = alloca i64, align 8
  %"%cdc_active_1" = alloca i64, align 8
  %"%cdc_active_0" = alloca i64, align 8
  %"%cdc_speed_3" = alloca i64, align 8
  %"%cdc_speed_2" = alloca i64, align 8
  %"%cdc_speed_1" = alloca i64, align 8
  %"%cdc_speed_0" = alloca i64, align 8
  %"%cdc_link_3" = alloca i64, align 8
  %"%cdc_link_2" = alloca i64, align 8
  %"%cdc_link_1" = alloca i64, align 8
  %"%cdc_link_0" = alloca i64, align 8
  %"%cdc_maxpkt_3" = alloca i64, align 8
  %"%cdc_maxpkt_2" = alloca i64, align 8
  %"%cdc_maxpkt_1" = alloca i64, align 8
  %"%cdc_maxpkt_0" = alloca i64, align 8
  %"%cdc_ep_int_3" = alloca i64, align 8
  %"%cdc_ep_int_2" = alloca i64, align 8
  %"%cdc_ep_int_1" = alloca i64, align 8
  %"%cdc_ep_int_0" = alloca i64, align 8
  %"%cdc_ep_out_3" = alloca i64, align 8
  %"%cdc_ep_out_2" = alloca i64, align 8
  %"%cdc_ep_out_1" = alloca i64, align 8
  %"%cdc_ep_out_0" = alloca i64, align 8
  %"%cdc_ep_in_3" = alloca i64, align 8
  %"%cdc_ep_in_2" = alloca i64, align 8
  %"%cdc_ep_in_1" = alloca i64, align 8
  %"%cdc_ep_in_0" = alloca i64, align 8
  %"%cdc_data_iface_3" = alloca i64, align 8
  %"%cdc_data_iface_2" = alloca i64, align 8
  %"%cdc_data_iface_1" = alloca i64, align 8
  %"%cdc_data_iface_0" = alloca i64, align 8
  %"%cdc_ctrl_iface_3" = alloca i64, align 8
  %"%cdc_ctrl_iface_2" = alloca i64, align 8
  %"%cdc_ctrl_iface_1" = alloca i64, align 8
  %"%cdc_ctrl_iface_0" = alloca i64, align 8
  %"%cdc_subclass_3" = alloca i64, align 8
  %"%cdc_subclass_2" = alloca i64, align 8
  %"%cdc_subclass_1" = alloca i64, align 8
  %"%cdc_subclass_0" = alloca i64, align 8
  %"%cdc_addr_3" = alloca i64, align 8
  %"%cdc_addr_2" = alloca i64, align 8
  %"%cdc_addr_1" = alloca i64, align 8
  %"%cdc_addr_0" = alloca i64, align 8
  %"%cdc_present_3" = alloca i64, align 8
  %"%cdc_present_2" = alloca i64, align 8
  %"%cdc_present_1" = alloca i64, align 8
  %"%cdc_present_0" = alloca i64, align 8
  %"%CDC_TX_RING_SIZE" = alloca i64, align 8
  %"%CDC_RX_RING_SIZE" = alloca i64, align 8
  %"%CDC_MAX_DEVICES" = alloca i64, align 8
  %"%CDC_MAX_SEGMENT" = alloca i64, align 8
  %"%ETH_MAX_FRAME" = alloca i64, align 8
  %"%ETH_HEADER_SIZE" = alloca i64, align 8
  %"%ETH_MTU" = alloca i64, align 8
  %"%LINK_UP" = alloca i64, align 8
  %"%LINK_DOWN" = alloca i64, align 8
  %"%ETH_FILTER_MULTICAST" = alloca i64, align 8
  %"%ETH_FILTER_BROADCAST" = alloca i64, align 8
  %"%ETH_FILTER_DIRECTED" = alloca i64, align 8
  %"%ETH_FILTER_ALL_MULTICAST" = alloca i64, align 8
  %"%ETH_FILTER_PROMISCUOUS" = alloca i64, align 8
  %"%NCM_SET_NTB_INPUT_SIZE" = alloca i64, align 8
  %"%NCM_GET_NTB_INPUT_SIZE" = alloca i64, align 8
  %"%NCM_GET_NTB_PARAMETERS" = alloca i64, align 8
  %"%ECM_GET_ETHERNET_STAT" = alloca i64, align 8
  %"%ECM_SET_ETHERNET_FILTER" = alloca i64, align 8
  %"%CDC_NOTIFY_SERIAL_STATE" = alloca i64, align 8
  %"%CDC_NOTIFY_SPEED_CHANGE" = alloca i64, align 8
  %"%CDC_NOTIFY_NETWORK_CONNECTION" = alloca i64, align 8
  %"%CDC_SEND_BREAK" = alloca i64, align 8
  %"%CDC_SET_CONTROL_LINE" = alloca i64, align 8
  %"%CDC_GET_LINE_CODING" = alloca i64, align 8
  %"%CDC_SET_LINE_CODING" = alloca i64, align 8
  %"%CDC_DESC_ETHERNET" = alloca i64, align 8
  %"%CDC_DESC_UNION" = alloca i64, align 8
  %"%CDC_DESC_ACM" = alloca i64, align 8
  %"%CDC_DESC_CALL_MGMT" = alloca i64, align 8
  %"%CDC_DESC_HEADER" = alloca i64, align 8
  %"%CDC_SUBCLASS_NCM" = alloca i64, align 8
  %"%CDC_SUBCLASS_ECM" = alloca i64, align 8
  %"%CDC_SUBCLASS_ACM" = alloca i64, align 8
  %"%CDC_DATA_CLASS" = alloca i64, align 8
  %"%CDC_CLASS" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 2, ptr %"%CDC_CLASS", align 8
  store i64 10, ptr %"%CDC_DATA_CLASS", align 8
  store i64 2, ptr %"%CDC_SUBCLASS_ACM", align 8
  store i64 6, ptr %"%CDC_SUBCLASS_ECM", align 8
  store i64 13, ptr %"%CDC_SUBCLASS_NCM", align 8
  store i64 0, ptr %"%CDC_DESC_HEADER", align 8
  store i64 1, ptr %"%CDC_DESC_CALL_MGMT", align 8
  store i64 2, ptr %"%CDC_DESC_ACM", align 8
  store i64 6, ptr %"%CDC_DESC_UNION", align 8
  store i64 15, ptr %"%CDC_DESC_ETHERNET", align 8
  store i64 32, ptr %"%CDC_SET_LINE_CODING", align 8
  store i64 33, ptr %"%CDC_GET_LINE_CODING", align 8
  store i64 34, ptr %"%CDC_SET_CONTROL_LINE", align 8
  store i64 35, ptr %"%CDC_SEND_BREAK", align 8
  store i64 0, ptr %"%CDC_NOTIFY_NETWORK_CONNECTION", align 8
  store i64 42, ptr %"%CDC_NOTIFY_SPEED_CHANGE", align 8
  store i64 32, ptr %"%CDC_NOTIFY_SERIAL_STATE", align 8
  store i64 67, ptr %"%ECM_SET_ETHERNET_FILTER", align 8
  store i64 68, ptr %"%ECM_GET_ETHERNET_STAT", align 8
  store i64 128, ptr %"%NCM_GET_NTB_PARAMETERS", align 8
  store i64 133, ptr %"%NCM_GET_NTB_INPUT_SIZE", align 8
  store i64 134, ptr %"%NCM_SET_NTB_INPUT_SIZE", align 8
  store i64 1, ptr %"%ETH_FILTER_PROMISCUOUS", align 8
  store i64 2, ptr %"%ETH_FILTER_ALL_MULTICAST", align 8
  store i64 4, ptr %"%ETH_FILTER_DIRECTED", align 8
  store i64 8, ptr %"%ETH_FILTER_BROADCAST", align 8
  store i64 16, ptr %"%ETH_FILTER_MULTICAST", align 8
  store i64 0, ptr %"%LINK_DOWN", align 8
  store i64 1, ptr %"%LINK_UP", align 8
  store i64 1500, ptr %"%ETH_MTU", align 8
  store i64 14, ptr %"%ETH_HEADER_SIZE", align 8
  store i64 1514, ptr %"%ETH_MAX_FRAME", align 8
  store i64 2048, ptr %"%CDC_MAX_SEGMENT", align 8
  store i64 4, ptr %"%CDC_MAX_DEVICES", align 8
  store i64 16, ptr %"%CDC_RX_RING_SIZE", align 8
  store i64 16, ptr %"%CDC_TX_RING_SIZE", align 8
  store i64 0, ptr %"%cdc_present_0", align 8
  store i64 0, ptr %"%cdc_present_1", align 8
  store i64 0, ptr %"%cdc_present_2", align 8
  store i64 0, ptr %"%cdc_present_3", align 8
  store i64 0, ptr %"%cdc_addr_0", align 8
  store i64 0, ptr %"%cdc_addr_1", align 8
  store i64 0, ptr %"%cdc_addr_2", align 8
  store i64 0, ptr %"%cdc_addr_3", align 8
  store i64 0, ptr %"%cdc_subclass_0", align 8
  store i64 0, ptr %"%cdc_subclass_1", align 8
  store i64 0, ptr %"%cdc_subclass_2", align 8
  store i64 0, ptr %"%cdc_subclass_3", align 8
  store i64 0, ptr %"%cdc_ctrl_iface_0", align 8
  store i64 0, ptr %"%cdc_ctrl_iface_1", align 8
  store i64 0, ptr %"%cdc_ctrl_iface_2", align 8
  store i64 0, ptr %"%cdc_ctrl_iface_3", align 8
  store i64 0, ptr %"%cdc_data_iface_0", align 8
  store i64 0, ptr %"%cdc_data_iface_1", align 8
  store i64 0, ptr %"%cdc_data_iface_2", align 8
  store i64 0, ptr %"%cdc_data_iface_3", align 8
  store i64 0, ptr %"%cdc_ep_in_0", align 8
  store i64 0, ptr %"%cdc_ep_in_1", align 8
  store i64 0, ptr %"%cdc_ep_in_2", align 8
  store i64 0, ptr %"%cdc_ep_in_3", align 8
  store i64 0, ptr %"%cdc_ep_out_0", align 8
  store i64 0, ptr %"%cdc_ep_out_1", align 8
  store i64 0, ptr %"%cdc_ep_out_2", align 8
  store i64 0, ptr %"%cdc_ep_out_3", align 8
  store i64 0, ptr %"%cdc_ep_int_0", align 8
  store i64 0, ptr %"%cdc_ep_int_1", align 8
  store i64 0, ptr %"%cdc_ep_int_2", align 8
  store i64 0, ptr %"%cdc_ep_int_3", align 8
  store i64 512, ptr %"%cdc_maxpkt_0", align 8
  store i64 512, ptr %"%cdc_maxpkt_1", align 8
  store i64 512, ptr %"%cdc_maxpkt_2", align 8
  store i64 512, ptr %"%cdc_maxpkt_3", align 8
  store i64 0, ptr %"%cdc_link_0", align 8
  store i64 0, ptr %"%cdc_link_1", align 8
  store i64 0, ptr %"%cdc_link_2", align 8
  store i64 0, ptr %"%cdc_link_3", align 8
  store i64 0, ptr %"%cdc_speed_0", align 8
  store i64 0, ptr %"%cdc_speed_1", align 8
  store i64 0, ptr %"%cdc_speed_2", align 8
  store i64 0, ptr %"%cdc_speed_3", align 8
  store i64 0, ptr %"%cdc_active_0", align 8
  store i64 0, ptr %"%cdc_active_1", align 8
  store i64 0, ptr %"%cdc_active_2", align 8
  store i64 0, ptr %"%cdc_active_3", align 8
  store i64 12, ptr %"%cdc_filter_0", align 8
  store i64 12, ptr %"%cdc_filter_1", align 8
  store i64 12, ptr %"%cdc_filter_2", align 8
  store i64 12, ptr %"%cdc_filter_3", align 8
  store i64 0, ptr %"%mac_0_0", align 8
  store i64 0, ptr %"%mac_0_1", align 8
  store i64 0, ptr %"%mac_0_2", align 8
  store i64 0, ptr %"%mac_0_3", align 8
  store i64 0, ptr %"%mac_0_4", align 8
  store i64 0, ptr %"%mac_0_5", align 8
  store i64 0, ptr %"%mac_1_0", align 8
  store i64 0, ptr %"%mac_1_1", align 8
  store i64 0, ptr %"%mac_1_2", align 8
  store i64 0, ptr %"%mac_1_3", align 8
  store i64 0, ptr %"%mac_1_4", align 8
  store i64 0, ptr %"%mac_1_5", align 8
  store i64 0, ptr %"%mac_2_0", align 8
  store i64 0, ptr %"%mac_2_1", align 8
  store i64 0, ptr %"%mac_2_2", align 8
  store i64 0, ptr %"%mac_2_3", align 8
  store i64 0, ptr %"%mac_2_4", align 8
  store i64 0, ptr %"%mac_2_5", align 8
  store i64 0, ptr %"%mac_3_0", align 8
  store i64 0, ptr %"%mac_3_1", align 8
  store i64 0, ptr %"%mac_3_2", align 8
  store i64 0, ptr %"%mac_3_3", align 8
  store i64 0, ptr %"%mac_3_4", align 8
  store i64 0, ptr %"%mac_3_5", align 8
  store i64 0, ptr %"%cdc_rx_buf_0", align 8
  store i64 0, ptr %"%cdc_rx_buf_1", align 8
  store i64 0, ptr %"%cdc_rx_buf_2", align 8
  store i64 0, ptr %"%cdc_rx_buf_3", align 8
  store i64 0, ptr %"%cdc_tx_buf_0", align 8
  store i64 0, ptr %"%cdc_tx_buf_1", align 8
  store i64 0, ptr %"%cdc_tx_buf_2", align 8
  store i64 0, ptr %"%cdc_tx_buf_3", align 8
  store i64 0, ptr %"%cdc_rx_head_0", align 8
  store i64 0, ptr %"%cdc_rx_tail_0", align 8
  store i64 0, ptr %"%cdc_tx_head_0", align 8
  store i64 0, ptr %"%cdc_tx_tail_0", align 8
  store i64 0, ptr %"%cdc_rx_head_1", align 8
  store i64 0, ptr %"%cdc_rx_tail_1", align 8
  store i64 0, ptr %"%cdc_tx_head_1", align 8
  store i64 0, ptr %"%cdc_tx_tail_1", align 8
  store i64 0, ptr %"%cdc_rx_head_2", align 8
  store i64 0, ptr %"%cdc_rx_tail_2", align 8
  store i64 0, ptr %"%cdc_tx_head_2", align 8
  store i64 0, ptr %"%cdc_tx_tail_2", align 8
  store i64 0, ptr %"%cdc_rx_head_3", align 8
  store i64 0, ptr %"%cdc_rx_tail_3", align 8
  store i64 0, ptr %"%cdc_tx_head_3", align 8
  store i64 0, ptr %"%cdc_tx_tail_3", align 8
  store i64 0, ptr %"%cdc_device_count", align 8
  store i64 0, ptr %"%cdc_tx_packets", align 8
  store i64 0, ptr %"%cdc_rx_packets", align 8
  store i64 0, ptr %"%cdc_tx_bytes", align 8
  store i64 0, ptr %"%cdc_rx_bytes", align 8
  store i64 0, ptr %"%cdc_errors", align 8
  store i64 0, ptr %"%cdc_dropped", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
