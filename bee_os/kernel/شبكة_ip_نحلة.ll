; ModuleID = 'شبكة_ip_نحلة'
source_filename = "\D8\B4\D8\A8\D9\83\D8\A9_ip_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@COM1 = constant i64 1016
@ETHER_TYPE_IP = constant i64 2048
@IP_PROTO_ICMP = constant i64 1
@IP_PROTO_TCP = constant i64 6
@IP_PROTO_UDP = constant i64 17
@ETH_HEADER_SIZE = constant i64 14
@IP_HEADER_SIZE = constant i64 20
@IP_VERSION = constant i64 4
@IP_IHL = constant i64 5
@IP_TTL_DEFAULT = constant i64 64
@IP_MAX_PACKET = constant i64 1500
@IP_FLAG_DF = constant i64 2
@IP_FLAG_MF = constant i64 1
@ARP_RESOLVED = constant i64 2
@ARP_PERMANENT = constant i64 3
@local_ip_0 = global i64 0
@local_ip_1 = global i64 0
@local_ip_2 = global i64 0
@local_ip_3 = global i64 0
@local_mask_0 = global i64 255
@local_mask_1 = global i64 255
@local_mask_2 = global i64 255
@local_mask_3 = global i64 0
@local_gw_0 = global i64 0
@local_gw_1 = global i64 0
@local_gw_2 = global i64 0
@local_gw_3 = global i64 0
@local_mac_0 = global i64 0
@local_mac_1 = global i64 0
@local_mac_2 = global i64 0
@local_mac_3 = global i64 0
@local_mac_4 = global i64 0
@local_mac_5 = global i64 0
@ip_packet_id = global i64 1
@ip_tx_count = global i64 0
@ip_rx_count = global i64 0
@ip_rx_errors = global i64 0
@ip_tx_errors = global i64 0
@ip_checksum_errors = global i64 0
@ip_ttl_expired = global i64 0
@ip_forwarded = global i64 0
@ip_dropped = global i64 0
@route_dest_0 = global i64 0
@route_dest_1 = global i64 0
@route_dest_2 = global i64 0
@route_dest_3 = global i64 0
@route_mask_0 = global i64 0
@route_mask_1 = global i64 0
@route_mask_2 = global i64 0
@route_mask_3 = global i64 0
@route_gw_0 = global i64 0
@route_gw_1 = global i64 0
@route_gw_2 = global i64 0
@route_gw_3 = global i64 0
@route_active_0 = global i64 0
@route_active_1 = global i64 0
@route_active_2 = global i64 0
@route_active_3 = global i64 0
@ICMP_ECHO_REQUEST = constant i64 8
@ICMP_ECHO_REPLY = constant i64 0
@str.const = private unnamed_addr constant [34 x i8] c"[IP] \D8\AD\D8\B2\D9\85\D8\A9 \D9\83\D8\A8\D9\8A\D8\B1\D8\A9 \D8\AC\D8\AF\D8\A7\D9\8B\00", align 1
@str.const.1 = private unnamed_addr constant [38 x i8] c"[IP] \D9\81\D8\B4\D9\84 \D8\AA\D8\AE\D8\B5\D9\8A\D8\B5 \D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9\00", align 1
@str.const.2 = private unnamed_addr constant [29 x i8] c"[IP] ARP \D8\BA\D9\8A\D8\B1 \D9\85\D9\88\D8\AC\D9\88\D8\AF: \00", align 1
@str.const.3 = private unnamed_addr constant [32 x i8] c"[IP] ARP \D9\84\D9\85 \D9\8A\D9\8F\D8\AD\D9\84\D9\91 \D8\A8\D8\B9\D8\AF\00", align 1
@str.const.4 = private unnamed_addr constant [36 x i8] c"[IP] \D8\A5\D8\B5\D8\AF\D8\A7\D8\B1 \D8\BA\D9\8A\D8\B1 \D9\85\D8\AF\D8\B9\D9\88\D9\85: \00", align 1
@str.const.5 = private unnamed_addr constant [35 x i8] c"[IP] \D9\85\D8\AC\D9\85\D9\88\D8\B9 \D8\AA\D8\AD\D9\82\D9\82 \D8\AE\D8\A7\D8\B7\D8\A6!\00", align 1
@str.const.6 = private unnamed_addr constant [15 x i8] c"[IP] RX proto=\00", align 1
@str.const.7 = private unnamed_addr constant [7 x i8] c" from=\00", align 1
@str.const.8 = private unnamed_addr constant [6 x i8] c" len=\00", align 1
@str.const.9 = private unnamed_addr constant [34 x i8] c"[IP] TCP packet (not handled yet)\00", align 1
@str.const.10 = private unnamed_addr constant [34 x i8] c"[IP] UDP packet (not handled yet)\00", align 1
@str.const.11 = private unnamed_addr constant [42 x i8] c"[IP] \D8\A8\D8\B1\D9\88\D8\AA\D9\88\D9\83\D9\88\D9\84 \D8\BA\D9\8A\D8\B1 \D9\85\D8\B9\D8\B1\D9\88\D9\81: \00", align 1
@str.const.12 = private unnamed_addr constant [28 x i8] c"[IP] ICMP \D9\82\D8\B5\D9\8A\D8\B1 \D8\AC\D8\AF\D8\A7\D9\8B\00", align 1
@str.const.13 = private unnamed_addr constant [29 x i8] c"[IP] ICMP Echo Request from=\00", align 1
@str.const.14 = private unnamed_addr constant [27 x i8] c"[IP] ICMP Echo Reply from=\00", align 1
@str.const.15 = private unnamed_addr constant [16 x i8] c"[IP] ICMP type=\00", align 1
@str.const.16 = private unnamed_addr constant [7 x i8] c" code=\00", align 1
@str.const.17 = private unnamed_addr constant [19 x i8] c"[IP] Ping sent to \00", align 1
@str.const.18 = private unnamed_addr constant [6 x i8] c" seq=\00", align 1
@str.const.19 = private unnamed_addr constant [58 x i8] c"\E2\95\90\E2\95\90\E2\95\90 [IP] \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A8\D8\B1\D9\88\D8\AA\D9\88\D9\83\D9\88\D9\84 IPv4 \E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.20 = private unnamed_addr constant [18 x i8] c"  \D8\B9\D9\86\D9\88\D8\A7\D9\86 IP: \00", align 1
@str.const.21 = private unnamed_addr constant [17 x i8] c"  \D8\A7\D9\84\D9\82\D9\86\D8\A7\D8\B9: \00", align 1
@str.const.22 = private unnamed_addr constant [19 x i8] c"  \D8\A7\D9\84\D8\A8\D9\88\D8\A7\D8\A8\D8\A9: \00", align 1
@str.const.23 = private unnamed_addr constant [22 x i8] c"  \D8\AD\D8\B2\D9\85 \D9\85\D8\B1\D8\B3\D9\84\D8\A9: \00", align 1
@str.const.24 = private unnamed_addr constant [26 x i8] c"  \D8\AD\D8\B2\D9\85 \D9\85\D8\B3\D8\AA\D9\82\D8\A8\D9\84\D8\A9: \00", align 1
@str.const.25 = private unnamed_addr constant [26 x i8] c"  \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1 \D8\A5\D8\B1\D8\B3\D8\A7\D9\84: \00", align 1
@str.const.26 = private unnamed_addr constant [30 x i8] c"  \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1 \D8\A7\D8\B3\D8\AA\D9\82\D8\A8\D8\A7\D9\84: \00", align 1
@str.const.27 = private unnamed_addr constant [24 x i8] c"  \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1 checksum: \00", align 1
@str.const.28 = private unnamed_addr constant [24 x i8] c"  \D8\AD\D8\B2\D9\85 \D9\85\D9\8F\D8\B3\D9\82\D8\B7\D8\A9: \00", align 1
@str.const.29 = private unnamed_addr constant [106 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.30 = private unnamed_addr constant [41 x i8] c"[IP] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D8\A8\D8\B1\D9\88\D8\AA\D9\88\D9\83\D9\88\D9\84 IPv4...\00", align 1
@str.const.31 = private unnamed_addr constant [14 x i8] c"[IP] \D8\AC\D8\A7\D9\87\D8\B2\00", align 1

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

declare void @eth_build_header(i64, i64, i64, i64, i64, i64, i64, i64)

declare void @eth_send_to(i64, i64, i64, i64, i64, i64, i64, i64, i64)

declare void @eth_send_broadcast(i64, i64, i64)

declare void @net_send(i64, i64)

declare void @arp_lookup(i64)

declare void @arp_send_request(i64)

declare void @arp_get_mac_hi(i64)

declare void @arp_get_mac_lo(i64)

declare void @arp_get_state(i64)

define i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
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

define i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
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

define i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"() {
entry:
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 13)
  %"%COM1.load1" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load1", i64 10)
  ret i64 0
}

define i64 @ip_to_int(i64 %a, i64 %b, i64 %c, i64 %d) {
entry:
  %d4 = alloca i64, align 8
  store i64 %d, ptr %d4, align 8
  %c3 = alloca i64, align 8
  store i64 %c, ptr %c3, align 8
  %b2 = alloca i64, align 8
  store i64 %b, ptr %b2, align 8
  %a1 = alloca i64, align 8
  store i64 %a, ptr %a1, align 8
  %"%a.load" = load i64, ptr %a1, align 8
  %shltmp = shl i64 %"%a.load", 24
  %"%b.load" = load i64, ptr %b2, align 8
  %shltmp5 = shl i64 %"%b.load", 16
  %addtmp = add i64 %shltmp, %shltmp5
  %"%c.load" = load i64, ptr %c3, align 8
  %shltmp6 = shl i64 %"%c.load", 8
  %addtmp7 = add i64 %addtmp, %shltmp6
  %"%d.load" = load i64, ptr %d4, align 8
  %addtmp8 = add i64 %addtmp7, %"%d.load"
  ret i64 %addtmp8
}

define i64 @ip_byte(i64 %ip_int, i64 %byte_num) {
entry:
  %byte_num2 = alloca i64, align 8
  store i64 %byte_num, ptr %byte_num2, align 8
  %ip_int1 = alloca i64, align 8
  store i64 %ip_int, ptr %ip_int1, align 8
  %"%byte_num.load" = load i64, ptr %byte_num2, align 8
  %cmpeqtmp = icmp eq i64 %"%byte_num.load", 0
  br i1 %cmpeqtmp, label %then_24, label %merge_25

merge_25:                                         ; preds = %entry
  %"%byte_num.load3" = load i64, ptr %byte_num2, align 8
  %cmpeqtmp4 = icmp eq i64 %"%byte_num.load3", 1
  br i1 %cmpeqtmp4, label %then_26, label %merge_27

merge_27:                                         ; preds = %merge_25
  %"%byte_num.load8" = load i64, ptr %byte_num2, align 8
  %cmpeqtmp9 = icmp eq i64 %"%byte_num.load8", 2
  br i1 %cmpeqtmp9, label %then_28, label %merge_29

merge_29:                                         ; preds = %merge_27
  %"%ip_int.load13" = load i64, ptr %ip_int1, align 8
  %andtmp14 = and i64 %"%ip_int.load13", 255
  ret i64 %andtmp14

then_24:                                          ; preds = %entry
  %"%ip_int.load" = load i64, ptr %ip_int1, align 8
  %shrtmp = lshr i64 %"%ip_int.load", 24
  %andtmp = and i64 %shrtmp, 255
  ret i64 %andtmp

then_26:                                          ; preds = %merge_25
  %"%ip_int.load5" = load i64, ptr %ip_int1, align 8
  %shrtmp6 = lshr i64 %"%ip_int.load5", 16
  %andtmp7 = and i64 %shrtmp6, 255
  ret i64 %andtmp7

then_28:                                          ; preds = %merge_27
  %"%ip_int.load10" = load i64, ptr %ip_int1, align 8
  %shrtmp11 = lshr i64 %"%ip_int.load10", 8
  %andtmp12 = and i64 %shrtmp11, 255
  ret i64 %andtmp12
}

define i64 @mac_pack3(i64 %b0, i64 %b1, i64 %b2) {
entry:
  %b23 = alloca i64, align 8
  store i64 %b2, ptr %b23, align 8
  %b12 = alloca i64, align 8
  store i64 %b1, ptr %b12, align 8
  %b01 = alloca i64, align 8
  store i64 %b0, ptr %b01, align 8
  %"%b0.load" = load i64, ptr %b01, align 8
  %shltmp = shl i64 %"%b0.load", 16
  %"%b1.load" = load i64, ptr %b12, align 8
  %shltmp4 = shl i64 %"%b1.load", 8
  %addtmp = add i64 %shltmp, %shltmp4
  %"%b2.load" = load i64, ptr %b23, align 8
  %addtmp5 = add i64 %addtmp, %"%b2.load"
  ret i64 %addtmp5
}

define i64 @mac_unpack(i64 %packed, i64 %byte_num) {
entry:
  %byte_num2 = alloca i64, align 8
  store i64 %byte_num, ptr %byte_num2, align 8
  %packed1 = alloca i64, align 8
  store i64 %packed, ptr %packed1, align 8
  %"%byte_num.load" = load i64, ptr %byte_num2, align 8
  %cmpeqtmp = icmp eq i64 %"%byte_num.load", 0
  br i1 %cmpeqtmp, label %then_30, label %merge_31

merge_31:                                         ; preds = %entry
  %"%byte_num.load3" = load i64, ptr %byte_num2, align 8
  %cmpeqtmp4 = icmp eq i64 %"%byte_num.load3", 1
  br i1 %cmpeqtmp4, label %then_32, label %merge_33

merge_33:                                         ; preds = %merge_31
  %"%packed.load8" = load i64, ptr %packed1, align 8
  %andtmp9 = and i64 %"%packed.load8", 255
  ret i64 %andtmp9

then_30:                                          ; preds = %entry
  %"%packed.load" = load i64, ptr %packed1, align 8
  %shrtmp = lshr i64 %"%packed.load", 16
  %andtmp = and i64 %shrtmp, 255
  ret i64 %andtmp

then_32:                                          ; preds = %merge_31
  %"%packed.load5" = load i64, ptr %packed1, align 8
  %shrtmp6 = lshr i64 %"%packed.load5", 8
  %andtmp7 = and i64 %shrtmp6, 255
  ret i64 %andtmp7
}

define i64 @ip_checksum(i64 %buf, i64 %length) {
entry:
  %"%result" = alloca i64, align 8
  %"%carry" = alloca i64, align 8
  %"%last_byte" = alloca i64, align 8
  %"%word" = alloca i64, align 8
  %"%lo" = alloca i64, align 8
  %"%hi" = alloca i64, align 8
  %"%offset" = alloca i64, align 8
  %"%sum" = alloca i64, align 8
  %length2 = alloca i64, align 8
  store i64 %length, ptr %length2, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  store i64 0, ptr %"%sum", align 8
  store i64 0, ptr %"%offset", align 8
  br label %while_cond_34

merge_38:                                         ; preds = %then_37, %while_exit_36
  br label %while_cond_39

then_37:                                          ; preds = %while_exit_36
  %"%buf.load13" = load i64, ptr %buf1, align 8
  %"%offset.load14" = load i64, ptr %"%offset", align 8
  %addtmp15 = add i64 %"%buf.load13", %"%offset.load14"
  call void @sad_ll_mem_read8(i64 %addtmp15)
  store i64 0, ptr %"%last_byte", align 8
  %"%last_byte.load" = load i64, ptr %"%last_byte", align 8
  %shltmp16 = shl i64 %"%last_byte.load", 8
  %"%sum.load17" = load i64, ptr %"%sum", align 8
  %addtmp18 = add i64 %"%sum.load17", %shltmp16
  store i64 %addtmp18, ptr %"%sum", align 8
  br label %merge_38

while_body_35:                                    ; preds = %while_cond_34
  %"%buf.load" = load i64, ptr %buf1, align 8
  %"%offset.load3" = load i64, ptr %"%offset", align 8
  %addtmp4 = add i64 %"%buf.load", %"%offset.load3"
  call void @sad_ll_mem_read8(i64 %addtmp4)
  store i64 0, ptr %"%hi", align 8
  %addtmp5 = add i64 %addtmp4, 1
  call void @sad_ll_mem_read8(i64 %addtmp5)
  store i64 0, ptr %"%lo", align 8
  %"%hi.load" = load i64, ptr %"%hi", align 8
  %shltmp = shl i64 %"%hi.load", 8
  %"%lo.load" = load i64, ptr %"%lo", align 8
  %addtmp6 = add i64 %shltmp, %"%lo.load"
  store i64 %addtmp6, ptr %"%word", align 8
  %"%sum.load" = load i64, ptr %"%sum", align 8
  %"%word.load" = load i64, ptr %"%word", align 8
  %addtmp7 = add i64 %"%sum.load", %"%word.load"
  store i64 %addtmp7, ptr %"%sum", align 8
  %"%offset.load8" = load i64, ptr %"%offset", align 8
  %addtmp9 = add i64 %"%offset.load8", 2
  store i64 %addtmp9, ptr %"%offset", align 8
  br label %while_cond_34

while_body_40:                                    ; preds = %while_cond_39
  %"%sum.load20" = load i64, ptr %"%sum", align 8
  %shrtmp = lshr i64 %"%sum.load20", 16
  store i64 %shrtmp, ptr %"%carry", align 8
  %"%sum.load21" = load i64, ptr %"%sum", align 8
  %andtmp = and i64 %"%sum.load21", 65535
  %"%carry.load" = load i64, ptr %"%carry", align 8
  %addtmp22 = add i64 %andtmp, %"%carry.load"
  store i64 %addtmp22, ptr %"%sum", align 8
  br label %while_cond_39

while_cond_34:                                    ; preds = %while_body_35, %entry
  %"%offset.load" = load i64, ptr %"%offset", align 8
  %addtmp = add i64 %"%offset.load", 1
  %"%length.load" = load i64, ptr %length2, align 8
  %cmplttmp = icmp slt i64 %addtmp, %"%length.load"
  br i1 %cmplttmp, label %while_body_35, label %while_exit_36

while_cond_39:                                    ; preds = %while_body_40, %merge_38
  %"%sum.load19" = load i64, ptr %"%sum", align 8
  %cmpgttmp = icmp sgt i64 %"%sum.load19", 65535
  br i1 %cmpgttmp, label %while_body_40, label %while_exit_41

while_exit_36:                                    ; preds = %while_cond_34
  %"%offset.load10" = load i64, ptr %"%offset", align 8
  %"%length.load11" = load i64, ptr %length2, align 8
  %cmplttmp12 = icmp slt i64 %"%offset.load10", %"%length.load11"
  br i1 %cmplttmp12, label %then_37, label %merge_38

while_exit_41:                                    ; preds = %while_cond_39
  %"%sum.load23" = load i64, ptr %"%sum", align 8
  %subtmp = sub i64 65535, %"%sum.load23"
  store i64 %subtmp, ptr %"%result", align 8
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp
}

define i64 @ip_build_header(i64 %buf, i64 %total_len, i64 %protocol, i64 %dst_ip) {
entry:
  %"%cksum" = alloca i64, align 8
  %"%ip_packet_id" = alloca i64, align 8
  %"%pkt_id" = alloca i64, align 8
  %dst_ip4 = alloca i64, align 8
  store i64 %dst_ip, ptr %dst_ip4, align 8
  %protocol3 = alloca i64, align 8
  store i64 %protocol, ptr %protocol3, align 8
  %total_len2 = alloca i64, align 8
  store i64 %total_len, ptr %total_len2, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  call void @sad_ll_mem_write8(i64 %"%buf.load", i64 69)
  %"%buf.load5" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load5", 1
  call void @sad_ll_mem_write8(i64 %addtmp, i64 0)
  %"%buf.load6" = load i64, ptr %buf1, align 8
  %addtmp7 = add i64 %"%buf.load6", 2
  %"%total_len.load" = load i64, ptr %total_len2, align 8
  %shrtmp = lshr i64 %"%total_len.load", 8
  call void @sad_ll_mem_write8(i64 %addtmp7, i64 %shrtmp)
  %"%buf.load8" = load i64, ptr %buf1, align 8
  %addtmp9 = add i64 %"%buf.load8", 3
  %"%total_len.load10" = load i64, ptr %total_len2, align 8
  %andtmp = and i64 %"%total_len.load10", 255
  call void @sad_ll_mem_write8(i64 %addtmp9, i64 %andtmp)
  %"%205.load" = load i64, ptr @ip_packet_id, align 8
  store i64 %"%205.load", ptr %"%pkt_id", align 8
  %"%buf.load11" = load i64, ptr %buf1, align 8
  %addtmp12 = add i64 %"%buf.load11", 4
  %"%pkt_id.load" = load i64, ptr %"%pkt_id", align 8
  %shrtmp13 = lshr i64 %"%pkt_id.load", 8
  call void @sad_ll_mem_write8(i64 %addtmp12, i64 %shrtmp13)
  %"%buf.load14" = load i64, ptr %buf1, align 8
  %addtmp15 = add i64 %"%buf.load14", 5
  %"%pkt_id.load16" = load i64, ptr %"%pkt_id", align 8
  %andtmp17 = and i64 %"%pkt_id.load16", 255
  call void @sad_ll_mem_write8(i64 %addtmp15, i64 %andtmp17)
  %"%ip_packet_id.load" = load i64, ptr @ip_packet_id, align 8
  %addtmp18 = add i64 %"%ip_packet_id.load", 1
  store i64 %addtmp18, ptr %"%ip_packet_id", align 8
  %"%buf.load19" = load i64, ptr %buf1, align 8
  %addtmp20 = add i64 %"%buf.load19", 6
  call void @sad_ll_mem_write8(i64 %addtmp20, i64 64)
  %"%buf.load21" = load i64, ptr %buf1, align 8
  %addtmp22 = add i64 %"%buf.load21", 7
  call void @sad_ll_mem_write8(i64 %addtmp22, i64 0)
  %"%buf.load23" = load i64, ptr %buf1, align 8
  %addtmp24 = add i64 %"%buf.load23", 8
  %"%IP_TTL_DEFAULT.load" = load i64, ptr @IP_TTL_DEFAULT, align 8
  call void @sad_ll_mem_write8(i64 %addtmp24, i64 %"%IP_TTL_DEFAULT.load")
  %"%buf.load25" = load i64, ptr %buf1, align 8
  %addtmp26 = add i64 %"%buf.load25", 9
  %"%protocol.load" = load i64, ptr %protocol3, align 8
  call void @sad_ll_mem_write8(i64 %addtmp26, i64 %"%protocol.load")
  %"%buf.load27" = load i64, ptr %buf1, align 8
  %addtmp28 = add i64 %"%buf.load27", 10
  call void @sad_ll_mem_write8(i64 %addtmp28, i64 0)
  %"%buf.load29" = load i64, ptr %buf1, align 8
  %addtmp30 = add i64 %"%buf.load29", 11
  call void @sad_ll_mem_write8(i64 %addtmp30, i64 0)
  %"%buf.load31" = load i64, ptr %buf1, align 8
  %addtmp32 = add i64 %"%buf.load31", 12
  %"%243.load" = load i64, ptr @local_ip_0, align 8
  call void @sad_ll_mem_write8(i64 %addtmp32, i64 %"%243.load")
  %"%buf.load33" = load i64, ptr %buf1, align 8
  %addtmp34 = add i64 %"%buf.load33", 13
  %"%248.load" = load i64, ptr @local_ip_1, align 8
  call void @sad_ll_mem_write8(i64 %addtmp34, i64 %"%248.load")
  %"%buf.load35" = load i64, ptr %buf1, align 8
  %addtmp36 = add i64 %"%buf.load35", 14
  %"%253.load" = load i64, ptr @local_ip_2, align 8
  call void @sad_ll_mem_write8(i64 %addtmp36, i64 %"%253.load")
  %"%buf.load37" = load i64, ptr %buf1, align 8
  %addtmp38 = add i64 %"%buf.load37", 15
  %"%258.load" = load i64, ptr @local_ip_3, align 8
  call void @sad_ll_mem_write8(i64 %addtmp38, i64 %"%258.load")
  %"%buf.load39" = load i64, ptr %buf1, align 8
  %addtmp40 = add i64 %"%buf.load39", 16
  %"%dst_ip.load" = load i64, ptr %dst_ip4, align 8
  %calltmp = call i64 @ip_byte(i64 %"%dst_ip.load", i64 0)
  call void @sad_ll_mem_write8(i64 %addtmp40, i64 %calltmp)
  %"%buf.load41" = load i64, ptr %buf1, align 8
  %addtmp42 = add i64 %"%buf.load41", 17
  %"%dst_ip.load43" = load i64, ptr %dst_ip4, align 8
  %calltmp44 = call i64 @ip_byte(i64 %"%dst_ip.load43", i64 1)
  call void @sad_ll_mem_write8(i64 %addtmp42, i64 %calltmp44)
  %"%buf.load45" = load i64, ptr %buf1, align 8
  %addtmp46 = add i64 %"%buf.load45", 18
  %"%dst_ip.load47" = load i64, ptr %dst_ip4, align 8
  %calltmp48 = call i64 @ip_byte(i64 %"%dst_ip.load47", i64 2)
  call void @sad_ll_mem_write8(i64 %addtmp46, i64 %calltmp48)
  %"%buf.load49" = load i64, ptr %buf1, align 8
  %addtmp50 = add i64 %"%buf.load49", 19
  %"%dst_ip.load51" = load i64, ptr %dst_ip4, align 8
  %calltmp52 = call i64 @ip_byte(i64 %"%dst_ip.load51", i64 3)
  call void @sad_ll_mem_write8(i64 %addtmp50, i64 %calltmp52)
  %"%buf.load53" = load i64, ptr %buf1, align 8
  %"%IP_HEADER_SIZE.load" = load i64, ptr @IP_HEADER_SIZE, align 8
  %calltmp54 = call i64 @ip_checksum(i64 %"%buf.load53", i64 %"%IP_HEADER_SIZE.load")
  store i64 %calltmp54, ptr %"%cksum", align 8
  %"%cksum.load" = load i64, ptr %"%cksum", align 8
  %shrtmp55 = lshr i64 %"%cksum.load", 8
  call void @sad_ll_mem_write8(i64 %addtmp28, i64 %shrtmp55)
  %"%cksum.load56" = load i64, ptr %"%cksum", align 8
  %andtmp57 = and i64 %"%cksum.load56", 255
  call void @sad_ll_mem_write8(i64 %addtmp30, i64 %andtmp57)
  %loadtmp = load i64, ptr @IP_HEADER_SIZE, align 8
  ret i64 %loadtmp
}

define i64 @ip_parse_version(i64 %pkt) {
entry:
  %"%byte0" = alloca i64, align 8
  %pkt1 = alloca i64, align 8
  store i64 %pkt, ptr %pkt1, align 8
  %"%pkt.load" = load i64, ptr %pkt1, align 8
  call void @sad_ll_mem_read8(i64 %"%pkt.load")
  store i64 0, ptr %"%byte0", align 8
  %"%byte0.load" = load i64, ptr %"%byte0", align 8
  %shrtmp = lshr i64 %"%byte0.load", 4
  ret i64 %shrtmp
}

define i64 @ip_parse_ihl(i64 %pkt) {
entry:
  %"%ihl" = alloca i64, align 8
  %"%byte0" = alloca i64, align 8
  %pkt1 = alloca i64, align 8
  store i64 %pkt, ptr %pkt1, align 8
  %"%pkt.load" = load i64, ptr %pkt1, align 8
  call void @sad_ll_mem_read8(i64 %"%pkt.load")
  store i64 0, ptr %"%byte0", align 8
  %"%byte0.load" = load i64, ptr %"%byte0", align 8
  %andtmp = and i64 %"%byte0.load", 15
  store i64 %andtmp, ptr %"%ihl", align 8
  %"%ihl.load" = load i64, ptr %"%ihl", align 8
  %shltmp = shl i64 %"%ihl.load", 2
  ret i64 %shltmp
}

define i64 @ip_parse_total_length(i64 %pkt) {
entry:
  %"%lo" = alloca i64, align 8
  %"%hi" = alloca i64, align 8
  %pkt1 = alloca i64, align 8
  store i64 %pkt, ptr %pkt1, align 8
  %"%pkt.load" = load i64, ptr %pkt1, align 8
  %addtmp = add i64 %"%pkt.load", 2
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%hi", align 8
  %"%pkt.load2" = load i64, ptr %pkt1, align 8
  %addtmp3 = add i64 %"%pkt.load2", 3
  call void @sad_ll_mem_read8(i64 %addtmp3)
  store i64 0, ptr %"%lo", align 8
  %"%hi.load" = load i64, ptr %"%hi", align 8
  %shltmp = shl i64 %"%hi.load", 8
  %"%lo.load" = load i64, ptr %"%lo", align 8
  %addtmp4 = add i64 %shltmp, %"%lo.load"
  ret i64 %addtmp4
}

define i64 @ip_parse_protocol(i64 %pkt) {
entry:
  %pkt1 = alloca i64, align 8
  store i64 %pkt, ptr %pkt1, align 8
  %"%pkt.load" = load i64, ptr %pkt1, align 8
  %addtmp = add i64 %"%pkt.load", 9
  call void @sad_ll_mem_read8(i64 %addtmp)
  ret i64 0
}

define i64 @ip_parse_ttl(i64 %pkt) {
entry:
  %pkt1 = alloca i64, align 8
  store i64 %pkt, ptr %pkt1, align 8
  %"%pkt.load" = load i64, ptr %pkt1, align 8
  %addtmp = add i64 %"%pkt.load", 8
  call void @sad_ll_mem_read8(i64 %addtmp)
  ret i64 0
}

define i64 @ip_parse_src(i64 %pkt) {
entry:
  %"%d" = alloca i64, align 8
  %"%c" = alloca i64, align 8
  %"%b" = alloca i64, align 8
  %"%a" = alloca i64, align 8
  %pkt1 = alloca i64, align 8
  store i64 %pkt, ptr %pkt1, align 8
  %"%pkt.load" = load i64, ptr %pkt1, align 8
  %addtmp = add i64 %"%pkt.load", 12
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%a", align 8
  %"%pkt.load2" = load i64, ptr %pkt1, align 8
  %addtmp3 = add i64 %"%pkt.load2", 13
  call void @sad_ll_mem_read8(i64 %addtmp3)
  store i64 0, ptr %"%b", align 8
  %"%pkt.load4" = load i64, ptr %pkt1, align 8
  %addtmp5 = add i64 %"%pkt.load4", 14
  call void @sad_ll_mem_read8(i64 %addtmp5)
  store i64 0, ptr %"%c", align 8
  %"%pkt.load6" = load i64, ptr %pkt1, align 8
  %addtmp7 = add i64 %"%pkt.load6", 15
  call void @sad_ll_mem_read8(i64 %addtmp7)
  store i64 0, ptr %"%d", align 8
  %"%a.load" = load i64, ptr %"%a", align 8
  %"%b.load" = load i64, ptr %"%b", align 8
  %"%c.load" = load i64, ptr %"%c", align 8
  %"%d.load" = load i64, ptr %"%d", align 8
  %calltmp = call i64 @ip_to_int(i64 %"%a.load", i64 %"%b.load", i64 %"%c.load", i64 %"%d.load")
  ret i64 %calltmp
}

define i64 @ip_parse_dst(i64 %pkt) {
entry:
  %"%d" = alloca i64, align 8
  %"%c" = alloca i64, align 8
  %"%b" = alloca i64, align 8
  %"%a" = alloca i64, align 8
  %pkt1 = alloca i64, align 8
  store i64 %pkt, ptr %pkt1, align 8
  %"%pkt.load" = load i64, ptr %pkt1, align 8
  %addtmp = add i64 %"%pkt.load", 16
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%a", align 8
  %"%pkt.load2" = load i64, ptr %pkt1, align 8
  %addtmp3 = add i64 %"%pkt.load2", 17
  call void @sad_ll_mem_read8(i64 %addtmp3)
  store i64 0, ptr %"%b", align 8
  %"%pkt.load4" = load i64, ptr %pkt1, align 8
  %addtmp5 = add i64 %"%pkt.load4", 18
  call void @sad_ll_mem_read8(i64 %addtmp5)
  store i64 0, ptr %"%c", align 8
  %"%pkt.load6" = load i64, ptr %pkt1, align 8
  %addtmp7 = add i64 %"%pkt.load6", 19
  call void @sad_ll_mem_read8(i64 %addtmp7)
  store i64 0, ptr %"%d", align 8
  %"%a.load" = load i64, ptr %"%a", align 8
  %"%b.load" = load i64, ptr %"%b", align 8
  %"%c.load" = load i64, ptr %"%c", align 8
  %"%d.load" = load i64, ptr %"%d", align 8
  %calltmp = call i64 @ip_to_int(i64 %"%a.load", i64 %"%b.load", i64 %"%c.load", i64 %"%d.load")
  ret i64 %calltmp
}

define i64 @ip_is_for_us(i64 %pkt) {
entry:
  %"%subnet_broadcast" = alloca i64, align 8
  %"%broadcast" = alloca i64, align 8
  %"%our_ip" = alloca i64, align 8
  %"%dst_ip" = alloca i64, align 8
  %pkt1 = alloca i64, align 8
  store i64 %pkt, ptr %pkt1, align 8
  %"%pkt.load" = load i64, ptr %pkt1, align 8
  %calltmp = call i64 @ip_parse_dst(i64 %"%pkt.load")
  store i64 %calltmp, ptr %"%dst_ip", align 8
  %"%384.load" = load i64, ptr @local_ip_0, align 8
  %"%386.load" = load i64, ptr @local_ip_1, align 8
  %"%388.load" = load i64, ptr @local_ip_2, align 8
  %"%390.load" = load i64, ptr @local_ip_3, align 8
  %calltmp2 = call i64 @ip_to_int(i64 %"%384.load", i64 %"%386.load", i64 %"%388.load", i64 %"%390.load")
  store i64 %calltmp2, ptr %"%our_ip", align 8
  %"%dst_ip.load" = load i64, ptr %"%dst_ip", align 8
  %"%our_ip.load" = load i64, ptr %"%our_ip", align 8
  %cmpeqtmp = icmp eq i64 %"%dst_ip.load", %"%our_ip.load"
  br i1 %cmpeqtmp, label %then_42, label %merge_43

merge_43:                                         ; preds = %entry
  %calltmp3 = call i64 @ip_to_int(i64 255, i64 255, i64 255, i64 255)
  store i64 %calltmp3, ptr %"%broadcast", align 8
  %"%dst_ip.load4" = load i64, ptr %"%dst_ip", align 8
  %"%broadcast.load" = load i64, ptr %"%broadcast", align 8
  %cmpeqtmp5 = icmp eq i64 %"%dst_ip.load4", %"%broadcast.load"
  br i1 %cmpeqtmp5, label %then_44, label %merge_45

merge_45:                                         ; preds = %merge_43
  %"%local_mask_0.load" = load i64, ptr @local_mask_0, align 8
  %subtmp = sub i64 255, %"%local_mask_0.load"
  %"%402.load" = load i64, ptr @local_ip_0, align 8
  %addtmp = add i64 %"%402.load", %subtmp
  %"%local_mask_1.load" = load i64, ptr @local_mask_1, align 8
  %subtmp6 = sub i64 255, %"%local_mask_1.load"
  %"%409.load" = load i64, ptr @local_ip_1, align 8
  %addtmp7 = add i64 %"%409.load", %subtmp6
  %"%local_mask_2.load" = load i64, ptr @local_mask_2, align 8
  %subtmp8 = sub i64 255, %"%local_mask_2.load"
  %"%416.load" = load i64, ptr @local_ip_2, align 8
  %addtmp9 = add i64 %"%416.load", %subtmp8
  %"%local_mask_3.load" = load i64, ptr @local_mask_3, align 8
  %subtmp10 = sub i64 255, %"%local_mask_3.load"
  %"%423.load" = load i64, ptr @local_ip_3, align 8
  %addtmp11 = add i64 %"%423.load", %subtmp10
  %calltmp12 = call i64 @ip_to_int(i64 %addtmp, i64 %addtmp7, i64 %addtmp9, i64 %addtmp11)
  store i64 %calltmp12, ptr %"%subnet_broadcast", align 8
  %"%dst_ip.load13" = load i64, ptr %"%dst_ip", align 8
  %"%subnet_broadcast.load" = load i64, ptr %"%subnet_broadcast", align 8
  %cmpeqtmp14 = icmp eq i64 %"%dst_ip.load13", %"%subnet_broadcast.load"
  br i1 %cmpeqtmp14, label %then_46, label %merge_47

merge_47:                                         ; preds = %merge_45
  ret i64 0

then_42:                                          ; preds = %entry
  ret i64 1

then_44:                                          ; preds = %merge_43
  ret i64 1

then_46:                                          ; preds = %merge_45
  ret i64 1
}

define i64 @ip_verify_checksum(i64 %pkt) {
entry:
  %"%cksum" = alloca i64, align 8
  %"%ihl" = alloca i64, align 8
  %pkt1 = alloca i64, align 8
  store i64 %pkt, ptr %pkt1, align 8
  %"%pkt.load" = load i64, ptr %pkt1, align 8
  %calltmp = call i64 @ip_parse_ihl(i64 %"%pkt.load")
  store i64 %calltmp, ptr %"%ihl", align 8
  %"%pkt.load2" = load i64, ptr %pkt1, align 8
  %"%ihl.load" = load i64, ptr %"%ihl", align 8
  %calltmp3 = call i64 @ip_checksum(i64 %"%pkt.load2", i64 %"%ihl.load")
  store i64 %calltmp3, ptr %"%cksum", align 8
  %"%cksum.load" = load i64, ptr %"%cksum", align 8
  %cmpeqtmp = icmp eq i64 %"%cksum.load", 0
  br i1 %cmpeqtmp, label %then_48, label %merge_49

merge_49:                                         ; preds = %entry
  ret i64 0

then_48:                                          ; preds = %entry
  ret i64 1
}

define i64 @ip_send(i64 %dst_ip, i64 %protocol, i64 %payload_addr, i64 %payload_len) {
entry:
  %"%ip_tx_count" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%d5" = alloca i64, align 8
  %"%d4" = alloca i64, align 8
  %"%d3" = alloca i64, align 8
  %"%d2" = alloca i64, align 8
  %"%d1" = alloca i64, align 8
  %"%d0" = alloca i64, align 8
  %"%mac_lo" = alloca i64, align 8
  %"%mac_hi" = alloca i64, align 8
  %"%arp_state" = alloca i64, align 8
  %"%arp_idx" = alloca i64, align 8
  %"%next_hop" = alloca i64, align 8
  %"%frame_buf" = alloca i64, align 8
  %"%frame_size" = alloca i64, align 8
  %"%ip_tx_errors" = alloca i64, align 8
  %"%total_len" = alloca i64, align 8
  %payload_len4 = alloca i64, align 8
  store i64 %payload_len, ptr %payload_len4, align 8
  %payload_addr3 = alloca i64, align 8
  store i64 %payload_addr, ptr %payload_addr3, align 8
  %protocol2 = alloca i64, align 8
  store i64 %protocol, ptr %protocol2, align 8
  %dst_ip1 = alloca i64, align 8
  store i64 %dst_ip, ptr %dst_ip1, align 8
  %"%IP_HEADER_SIZE.load" = load i64, ptr @IP_HEADER_SIZE, align 8
  %"%payload_len.load" = load i64, ptr %payload_len4, align 8
  %addtmp = add i64 %"%IP_HEADER_SIZE.load", %"%payload_len.load"
  store i64 %addtmp, ptr %"%total_len", align 8
  %"%total_len.load" = load i64, ptr %"%total_len", align 8
  %"%IP_MAX_PACKET.load" = load i64, ptr @IP_MAX_PACKET, align 8
  %cmpgttmp = icmp sgt i64 %"%total_len.load", %"%IP_MAX_PACKET.load"
  br i1 %cmpgttmp, label %then_50, label %merge_51

else_62:                                          ; preds = %merge_57
  %"%ip_tx_errors.load71" = load i64, ptr %"%ip_tx_errors", align 8
  %addtmp72 = add i64 %"%ip_tx_errors.load71", 1
  store i64 %addtmp72, ptr %"%ip_tx_errors", align 8
  br label %merge_61

merge_51:                                         ; preds = %entry
  %"%ETH_HEADER_SIZE.load" = load i64, ptr @ETH_HEADER_SIZE, align 8
  %"%total_len.load7" = load i64, ptr %"%total_len", align 8
  %addtmp8 = add i64 %"%ETH_HEADER_SIZE.load", %"%total_len.load7"
  store i64 %addtmp8, ptr %"%frame_size", align 8
  %"%frame_size.load" = load i64, ptr %"%frame_size", align 8
  call void @sad_ll_kmalloc(i64 %"%frame_size.load")
  store i64 0, ptr %"%frame_buf", align 8
  %"%frame_buf.load" = load i64, ptr %"%frame_buf", align 8
  %cmpeqtmp = icmp eq i64 %"%frame_buf.load", 0
  br i1 %cmpeqtmp, label %then_52, label %merge_53

merge_53:                                         ; preds = %merge_51
  %"%frame_buf.load13" = load i64, ptr %"%frame_buf", align 8
  %"%frame_size.load14" = load i64, ptr %"%frame_size", align 8
  call void @sad_ll_memset(i64 %"%frame_buf.load13", i64 0, i64 %"%frame_size.load14")
  %"%dst_ip.load" = load i64, ptr %dst_ip1, align 8
  %calltmp15 = call i64 @ip_resolve_next_hop(i64 %"%dst_ip.load")
  store i64 %calltmp15, ptr %"%next_hop", align 8
  %"%next_hop.load" = load i64, ptr %"%next_hop", align 8
  call void @arp_lookup(i64 %"%next_hop.load")
  store i64 0, ptr %"%arp_idx", align 8
  %"%arp_idx.load" = load i64, ptr %"%arp_idx", align 8
  %cmplttmp = icmp slt i64 %"%arp_idx.load", 0
  br i1 %cmplttmp, label %then_54, label %merge_55

merge_55:                                         ; preds = %merge_53
  %"%arp_idx.load36" = load i64, ptr %"%arp_idx", align 8
  call void @arp_get_state(i64 %"%arp_idx.load36")
  store i64 0, ptr %"%arp_state", align 8
  %"%arp_state.load" = load i64, ptr %"%arp_state", align 8
  %"%ARP_RESOLVED.load" = load i64, ptr @ARP_RESOLVED, align 8
  %cmpnetmp = icmp ne i64 %"%arp_state.load", %"%ARP_RESOLVED.load"
  br i1 %cmpnetmp, label %then_56, label %merge_57

merge_57:                                         ; preds = %merge_59, %merge_55
  %"%arp_idx.load39" = load i64, ptr %"%arp_idx", align 8
  call void @arp_get_mac_hi(i64 %"%arp_idx.load39")
  store i64 0, ptr %"%mac_hi", align 8
  %"%arp_idx.load40" = load i64, ptr %"%arp_idx", align 8
  call void @arp_get_mac_lo(i64 %"%arp_idx.load40")
  store i64 0, ptr %"%mac_lo", align 8
  %"%mac_hi.load" = load i64, ptr %"%mac_hi", align 8
  %calltmp41 = call i64 @mac_unpack(i64 %"%mac_hi.load", i64 0)
  store i64 %calltmp41, ptr %"%d0", align 8
  %"%mac_hi.load42" = load i64, ptr %"%mac_hi", align 8
  %calltmp43 = call i64 @mac_unpack(i64 %"%mac_hi.load42", i64 1)
  store i64 %calltmp43, ptr %"%d1", align 8
  %"%mac_hi.load44" = load i64, ptr %"%mac_hi", align 8
  %calltmp45 = call i64 @mac_unpack(i64 %"%mac_hi.load44", i64 2)
  store i64 %calltmp45, ptr %"%d2", align 8
  %"%mac_lo.load" = load i64, ptr %"%mac_lo", align 8
  %calltmp46 = call i64 @mac_unpack(i64 %"%mac_lo.load", i64 0)
  store i64 %calltmp46, ptr %"%d3", align 8
  %"%mac_lo.load47" = load i64, ptr %"%mac_lo", align 8
  %calltmp48 = call i64 @mac_unpack(i64 %"%mac_lo.load47", i64 1)
  store i64 %calltmp48, ptr %"%d4", align 8
  %"%mac_lo.load49" = load i64, ptr %"%mac_lo", align 8
  %calltmp50 = call i64 @mac_unpack(i64 %"%mac_lo.load49", i64 2)
  store i64 %calltmp50, ptr %"%d5", align 8
  %"%frame_buf.load51" = load i64, ptr %"%frame_buf", align 8
  %"%d0.load" = load i64, ptr %"%d0", align 8
  %"%d1.load" = load i64, ptr %"%d1", align 8
  %"%d2.load" = load i64, ptr %"%d2", align 8
  %"%d3.load" = load i64, ptr %"%d3", align 8
  %"%d4.load" = load i64, ptr %"%d4", align 8
  %"%d5.load" = load i64, ptr %"%d5", align 8
  %"%ETHER_TYPE_IP.load" = load i64, ptr @ETHER_TYPE_IP, align 8
  call void @eth_build_header(i64 %"%frame_buf.load51", i64 %"%d0.load", i64 %"%d1.load", i64 %"%d2.load", i64 %"%d3.load", i64 %"%d4.load", i64 %"%d5.load", i64 %"%ETHER_TYPE_IP.load")
  %"%frame_buf.load52" = load i64, ptr %"%frame_buf", align 8
  %"%ETH_HEADER_SIZE.load53" = load i64, ptr @ETH_HEADER_SIZE, align 8
  %addtmp54 = add i64 %"%frame_buf.load52", %"%ETH_HEADER_SIZE.load53"
  %"%total_len.load55" = load i64, ptr %"%total_len", align 8
  %"%protocol.load" = load i64, ptr %protocol2, align 8
  %"%dst_ip.load56" = load i64, ptr %dst_ip1, align 8
  %calltmp57 = call i64 @ip_build_header(i64 %addtmp54, i64 %"%total_len.load55", i64 %"%protocol.load", i64 %"%dst_ip.load56")
  %"%IP_HEADER_SIZE.load58" = load i64, ptr @IP_HEADER_SIZE, align 8
  %addtmp59 = add i64 %addtmp54, %"%IP_HEADER_SIZE.load58"
  %"%payload_addr.load" = load i64, ptr %payload_addr3, align 8
  %"%payload_len.load60" = load i64, ptr %payload_len4, align 8
  call void @sad_ll_memcpy(i64 %addtmp59, i64 %"%payload_addr.load", i64 %"%payload_len.load60")
  %"%frame_buf.load61" = load i64, ptr %"%frame_buf", align 8
  %"%frame_size.load62" = load i64, ptr %"%frame_size", align 8
  call void @net_send(i64 %"%frame_buf.load61", i64 %"%frame_size.load62")
  store i64 0, ptr %"%result", align 8
  %"%frame_buf.load63" = load i64, ptr %"%frame_buf", align 8
  call void @sad_ll_kfree(i64 %"%frame_buf.load63")
  %"%result.load" = load i64, ptr %"%result", align 8
  %cmpgttmp64 = icmp sgt i64 %"%result.load", 0
  br i1 %cmpgttmp64, label %then_60, label %else_62

merge_59:                                         ; preds = %then_56
  br label %merge_57

merge_61:                                         ; preds = %else_62, %then_60
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

then_50:                                          ; preds = %entry
  %calltmp = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %calltmp5 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %"%ip_tx_errors.load" = load i64, ptr @ip_tx_errors, align 8
  %addtmp6 = add i64 %"%ip_tx_errors.load", 1
  store i64 %addtmp6, ptr %"%ip_tx_errors", align 8
  ret i64 0

then_52:                                          ; preds = %merge_51
  %calltmp9 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %calltmp10 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %"%ip_tx_errors.load11" = load i64, ptr %"%ip_tx_errors", align 8
  %addtmp12 = add i64 %"%ip_tx_errors.load11", 1
  store i64 %addtmp12, ptr %"%ip_tx_errors", align 8
  ret i64 0

then_54:                                          ; preds = %merge_53
  %"%next_hop.load16" = load i64, ptr %"%next_hop", align 8
  call void @arp_send_request(i64 %"%next_hop.load16")
  %calltmp17 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %"%next_hop.load18" = load i64, ptr %"%next_hop", align 8
  %calltmp19 = call i64 @ip_byte(i64 %"%next_hop.load18", i64 0)
  %calltmp20 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp19)
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 46)
  %"%next_hop.load21" = load i64, ptr %"%next_hop", align 8
  %calltmp22 = call i64 @ip_byte(i64 %"%next_hop.load21", i64 1)
  %calltmp23 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp22)
  %"%COM1.load24" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load24", i64 46)
  %"%next_hop.load25" = load i64, ptr %"%next_hop", align 8
  %calltmp26 = call i64 @ip_byte(i64 %"%next_hop.load25", i64 2)
  %calltmp27 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp26)
  %"%COM1.load28" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load28", i64 46)
  %"%next_hop.load29" = load i64, ptr %"%next_hop", align 8
  %calltmp30 = call i64 @ip_byte(i64 %"%next_hop.load29", i64 3)
  %calltmp31 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp30)
  %calltmp32 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %"%frame_buf.load33" = load i64, ptr %"%frame_buf", align 8
  call void @sad_ll_kfree(i64 %"%frame_buf.load33")
  %"%ip_tx_errors.load34" = load i64, ptr %"%ip_tx_errors", align 8
  %addtmp35 = add i64 %"%ip_tx_errors.load34", 1
  store i64 %addtmp35, ptr %"%ip_tx_errors", align 8
  ret i64 0

then_56:                                          ; preds = %merge_55
  %"%arp_state.load37" = load i64, ptr %"%arp_state", align 8
  %"%ARP_PERMANENT.load" = load i64, ptr @ARP_PERMANENT, align 8
  %cmpnetmp38 = icmp ne i64 %"%arp_state.load37", %"%ARP_PERMANENT.load"
  br i1 %cmpnetmp38, label %then_58, label %merge_59

then_58:                                          ; preds = %then_56
  %calltmp65 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %calltmp66 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %"%frame_buf.load67" = load i64, ptr %"%frame_buf", align 8
  call void @sad_ll_kfree(i64 %"%frame_buf.load67")
  %"%ip_tx_errors.load68" = load i64, ptr %"%ip_tx_errors", align 8
  %addtmp69 = add i64 %"%ip_tx_errors.load68", 1
  store i64 %addtmp69, ptr %"%ip_tx_errors", align 8
  ret i64 0

then_60:                                          ; preds = %merge_57
  %"%ip_tx_count.load" = load i64, ptr @ip_tx_count, align 8
  %addtmp70 = add i64 %"%ip_tx_count.load", 1
  store i64 %addtmp70, ptr %"%ip_tx_count", align 8
  br label %merge_61
}

define i64 @ip_send_broadcast(i64 %protocol, i64 %payload_addr, i64 %payload_len) {
entry:
  %"%ip_tx_count" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%ip_tx_errors" = alloca i64, align 8
  %"%ip_buf" = alloca i64, align 8
  %"%dst_ip" = alloca i64, align 8
  %"%total_len" = alloca i64, align 8
  %payload_len3 = alloca i64, align 8
  store i64 %payload_len, ptr %payload_len3, align 8
  %payload_addr2 = alloca i64, align 8
  store i64 %payload_addr, ptr %payload_addr2, align 8
  %protocol1 = alloca i64, align 8
  store i64 %protocol, ptr %protocol1, align 8
  %"%IP_HEADER_SIZE.load" = load i64, ptr @IP_HEADER_SIZE, align 8
  %"%payload_len.load" = load i64, ptr %payload_len3, align 8
  %addtmp = add i64 %"%IP_HEADER_SIZE.load", %"%payload_len.load"
  store i64 %addtmp, ptr %"%total_len", align 8
  %calltmp = call i64 @ip_to_int(i64 255, i64 255, i64 255, i64 255)
  store i64 %calltmp, ptr %"%dst_ip", align 8
  %"%total_len.load" = load i64, ptr %"%total_len", align 8
  call void @sad_ll_kmalloc(i64 %"%total_len.load")
  store i64 0, ptr %"%ip_buf", align 8
  %"%ip_buf.load" = load i64, ptr %"%ip_buf", align 8
  %cmpeqtmp = icmp eq i64 %"%ip_buf.load", 0
  br i1 %cmpeqtmp, label %then_63, label %merge_64

else_67:                                          ; preds = %merge_64
  %"%ip_tx_errors.load18" = load i64, ptr %"%ip_tx_errors", align 8
  %addtmp19 = add i64 %"%ip_tx_errors.load18", 1
  store i64 %addtmp19, ptr %"%ip_tx_errors", align 8
  br label %merge_66

merge_64:                                         ; preds = %entry
  %"%ip_buf.load5" = load i64, ptr %"%ip_buf", align 8
  %"%total_len.load6" = load i64, ptr %"%total_len", align 8
  call void @sad_ll_memset(i64 %"%ip_buf.load5", i64 0, i64 %"%total_len.load6")
  %"%ip_buf.load7" = load i64, ptr %"%ip_buf", align 8
  %"%total_len.load8" = load i64, ptr %"%total_len", align 8
  %"%protocol.load" = load i64, ptr %protocol1, align 8
  %"%dst_ip.load" = load i64, ptr %"%dst_ip", align 8
  %calltmp9 = call i64 @ip_build_header(i64 %"%ip_buf.load7", i64 %"%total_len.load8", i64 %"%protocol.load", i64 %"%dst_ip.load")
  %"%ip_buf.load10" = load i64, ptr %"%ip_buf", align 8
  %"%IP_HEADER_SIZE.load11" = load i64, ptr @IP_HEADER_SIZE, align 8
  %addtmp12 = add i64 %"%ip_buf.load10", %"%IP_HEADER_SIZE.load11"
  %"%payload_addr.load" = load i64, ptr %payload_addr2, align 8
  %"%payload_len.load13" = load i64, ptr %payload_len3, align 8
  call void @sad_ll_memcpy(i64 %addtmp12, i64 %"%payload_addr.load", i64 %"%payload_len.load13")
  %"%ip_buf.load14" = load i64, ptr %"%ip_buf", align 8
  %"%total_len.load15" = load i64, ptr %"%total_len", align 8
  %"%ETHER_TYPE_IP.load" = load i64, ptr @ETHER_TYPE_IP, align 8
  call void @eth_send_broadcast(i64 %"%ip_buf.load14", i64 %"%total_len.load15", i64 %"%ETHER_TYPE_IP.load")
  store i64 0, ptr %"%result", align 8
  %"%ip_buf.load16" = load i64, ptr %"%ip_buf", align 8
  call void @sad_ll_kfree(i64 %"%ip_buf.load16")
  %"%result.load" = load i64, ptr %"%result", align 8
  %cmpgttmp = icmp sgt i64 %"%result.load", 0
  br i1 %cmpgttmp, label %then_65, label %else_67

merge_66:                                         ; preds = %else_67, %then_65
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

then_63:                                          ; preds = %entry
  %"%ip_tx_errors.load" = load i64, ptr @ip_tx_errors, align 8
  %addtmp4 = add i64 %"%ip_tx_errors.load", 1
  store i64 %addtmp4, ptr %"%ip_tx_errors", align 8
  ret i64 0

then_65:                                          ; preds = %merge_64
  %"%ip_tx_count.load" = load i64, ptr @ip_tx_count, align 8
  %addtmp17 = add i64 %"%ip_tx_count.load", 1
  store i64 %addtmp17, ptr %"%ip_tx_count", align 8
  br label %merge_66
}

define i64 @ip_resolve_next_hop(i64 %dst_ip) {
entry:
  %"%gw_ip" = alloca i64, align 8
  %"%r3_match" = alloca i64, align 8
  %"%r2_match" = alloca i64, align 8
  %"%r1_match" = alloca i64, align 8
  %"%r0_match" = alloca i64, align 8
  %"%src_net_3" = alloca i64, align 8
  %"%src_net_2" = alloca i64, align 8
  %"%src_net_1" = alloca i64, align 8
  %"%src_net_0" = alloca i64, align 8
  %"%dst_net_3" = alloca i64, align 8
  %"%dst_net_2" = alloca i64, align 8
  %"%dst_net_1" = alloca i64, align 8
  %"%dst_net_0" = alloca i64, align 8
  %"%dst_3" = alloca i64, align 8
  %"%dst_2" = alloca i64, align 8
  %"%dst_1" = alloca i64, align 8
  %"%dst_0" = alloca i64, align 8
  %"%mask_int" = alloca i64, align 8
  %"%our_net" = alloca i64, align 8
  %dst_ip1 = alloca i64, align 8
  store i64 %dst_ip, ptr %dst_ip1, align 8
  %"%593.load" = load i64, ptr @local_ip_0, align 8
  %"%595.load" = load i64, ptr @local_ip_1, align 8
  %"%597.load" = load i64, ptr @local_ip_2, align 8
  %"%599.load" = load i64, ptr @local_ip_3, align 8
  %calltmp = call i64 @ip_to_int(i64 %"%593.load", i64 %"%595.load", i64 %"%597.load", i64 %"%599.load")
  store i64 %calltmp, ptr %"%our_net", align 8
  %"%602.load" = load i64, ptr @local_mask_0, align 8
  %"%604.load" = load i64, ptr @local_mask_1, align 8
  %"%606.load" = load i64, ptr @local_mask_2, align 8
  %"%608.load" = load i64, ptr @local_mask_3, align 8
  %calltmp2 = call i64 @ip_to_int(i64 %"%602.load", i64 %"%604.load", i64 %"%606.load", i64 %"%608.load")
  store i64 %calltmp2, ptr %"%mask_int", align 8
  %"%dst_ip.load" = load i64, ptr %dst_ip1, align 8
  %calltmp3 = call i64 @ip_byte(i64 %"%dst_ip.load", i64 0)
  store i64 %calltmp3, ptr %"%dst_0", align 8
  %"%dst_ip.load4" = load i64, ptr %dst_ip1, align 8
  %calltmp5 = call i64 @ip_byte(i64 %"%dst_ip.load4", i64 1)
  store i64 %calltmp5, ptr %"%dst_1", align 8
  %"%dst_ip.load6" = load i64, ptr %dst_ip1, align 8
  %calltmp7 = call i64 @ip_byte(i64 %"%dst_ip.load6", i64 2)
  store i64 %calltmp7, ptr %"%dst_2", align 8
  %"%dst_ip.load8" = load i64, ptr %dst_ip1, align 8
  %calltmp9 = call i64 @ip_byte(i64 %"%dst_ip.load8", i64 3)
  store i64 %calltmp9, ptr %"%dst_3", align 8
  %"%local_mask_0.load" = load i64, ptr @local_mask_0, align 8
  %subtmp = sub i64 256, %"%local_mask_0.load"
  %"%dst_0.load" = load i64, ptr %"%dst_0", align 8
  %modtmp = srem i64 %"%dst_0.load", %subtmp
  %"%dst_0.load10" = load i64, ptr %"%dst_0", align 8
  %subtmp11 = sub i64 %"%dst_0.load10", %modtmp
  store i64 %subtmp11, ptr %"%dst_net_0", align 8
  %"%local_mask_1.load" = load i64, ptr @local_mask_1, align 8
  %subtmp12 = sub i64 256, %"%local_mask_1.load"
  %"%dst_1.load" = load i64, ptr %"%dst_1", align 8
  %modtmp13 = srem i64 %"%dst_1.load", %subtmp12
  %"%dst_1.load14" = load i64, ptr %"%dst_1", align 8
  %subtmp15 = sub i64 %"%dst_1.load14", %modtmp13
  store i64 %subtmp15, ptr %"%dst_net_1", align 8
  %"%local_mask_2.load" = load i64, ptr @local_mask_2, align 8
  %subtmp16 = sub i64 256, %"%local_mask_2.load"
  %"%dst_2.load" = load i64, ptr %"%dst_2", align 8
  %modtmp17 = srem i64 %"%dst_2.load", %subtmp16
  %"%dst_2.load18" = load i64, ptr %"%dst_2", align 8
  %subtmp19 = sub i64 %"%dst_2.load18", %modtmp17
  store i64 %subtmp19, ptr %"%dst_net_2", align 8
  %"%local_mask_3.load" = load i64, ptr @local_mask_3, align 8
  %subtmp20 = sub i64 256, %"%local_mask_3.load"
  %"%dst_3.load" = load i64, ptr %"%dst_3", align 8
  %modtmp21 = srem i64 %"%dst_3.load", %subtmp20
  %"%dst_3.load22" = load i64, ptr %"%dst_3", align 8
  %subtmp23 = sub i64 %"%dst_3.load22", %modtmp21
  store i64 %subtmp23, ptr %"%dst_net_3", align 8
  %"%593.load24" = load i64, ptr @local_ip_0, align 8
  %modtmp25 = srem i64 %"%593.load24", %subtmp
  %"%593.load26" = load i64, ptr @local_ip_0, align 8
  %subtmp27 = sub i64 %"%593.load26", %modtmp25
  store i64 %subtmp27, ptr %"%src_net_0", align 8
  %"%595.load28" = load i64, ptr @local_ip_1, align 8
  %modtmp29 = srem i64 %"%595.load28", %subtmp12
  %"%595.load30" = load i64, ptr @local_ip_1, align 8
  %subtmp31 = sub i64 %"%595.load30", %modtmp29
  store i64 %subtmp31, ptr %"%src_net_1", align 8
  %"%597.load32" = load i64, ptr @local_ip_2, align 8
  %modtmp33 = srem i64 %"%597.load32", %subtmp16
  %"%597.load34" = load i64, ptr @local_ip_2, align 8
  %subtmp35 = sub i64 %"%597.load34", %modtmp33
  store i64 %subtmp35, ptr %"%src_net_2", align 8
  %"%599.load36" = load i64, ptr @local_ip_3, align 8
  %modtmp37 = srem i64 %"%599.load36", %subtmp20
  %"%599.load38" = load i64, ptr @local_ip_3, align 8
  %subtmp39 = sub i64 %"%599.load38", %modtmp37
  store i64 %subtmp39, ptr %"%src_net_3", align 8
  %"%dst_net_0.load" = load i64, ptr %"%dst_net_0", align 8
  %"%src_net_0.load" = load i64, ptr %"%src_net_0", align 8
  %cmpeqtmp = icmp eq i64 %"%dst_net_0.load", %"%src_net_0.load"
  br i1 %cmpeqtmp, label %then_68, label %merge_69

merge_69:                                         ; preds = %merge_71, %entry
  %"%route_active_0.load" = load i64, ptr @route_active_0, align 8
  %cmpgttmp = icmp sgt i64 %"%route_active_0.load", 0
  br i1 %cmpgttmp, label %then_76, label %merge_77

merge_71:                                         ; preds = %merge_73, %then_68
  br label %merge_69

merge_73:                                         ; preds = %merge_75, %then_70
  br label %merge_71

merge_75:                                         ; preds = %then_72
  br label %merge_73

merge_77:                                         ; preds = %merge_79, %merge_69
  %"%route_active_1.load" = load i64, ptr @route_active_1, align 8
  %cmpgttmp46 = icmp sgt i64 %"%route_active_1.load", 0
  br i1 %cmpgttmp46, label %then_80, label %merge_81

merge_79:                                         ; preds = %then_76
  br label %merge_77

merge_81:                                         ; preds = %merge_83, %merge_77
  %"%route_active_2.load" = load i64, ptr @route_active_2, align 8
  %cmpgttmp51 = icmp sgt i64 %"%route_active_2.load", 0
  br i1 %cmpgttmp51, label %then_84, label %merge_85

merge_83:                                         ; preds = %then_80
  br label %merge_81

merge_85:                                         ; preds = %merge_87, %merge_81
  %"%route_active_3.load" = load i64, ptr @route_active_3, align 8
  %cmpgttmp56 = icmp sgt i64 %"%route_active_3.load", 0
  br i1 %cmpgttmp56, label %then_88, label %merge_89

merge_87:                                         ; preds = %then_84
  br label %merge_85

merge_89:                                         ; preds = %merge_91, %merge_85
  %"%738.load" = load i64, ptr @local_gw_0, align 8
  %"%740.load" = load i64, ptr @local_gw_1, align 8
  %"%742.load" = load i64, ptr @local_gw_2, align 8
  %"%744.load" = load i64, ptr @local_gw_3, align 8
  %calltmp61 = call i64 @ip_to_int(i64 %"%738.load", i64 %"%740.load", i64 %"%742.load", i64 %"%744.load")
  store i64 %calltmp61, ptr %"%gw_ip", align 8
  %loadtmp62 = load i64, ptr %"%gw_ip", align 8
  ret i64 %loadtmp62

merge_91:                                         ; preds = %then_88
  br label %merge_89

then_68:                                          ; preds = %entry
  %"%dst_net_1.load" = load i64, ptr %"%dst_net_1", align 8
  %"%src_net_1.load" = load i64, ptr %"%src_net_1", align 8
  %cmpeqtmp40 = icmp eq i64 %"%dst_net_1.load", %"%src_net_1.load"
  br i1 %cmpeqtmp40, label %then_70, label %merge_71

then_70:                                          ; preds = %then_68
  %"%dst_net_2.load" = load i64, ptr %"%dst_net_2", align 8
  %"%src_net_2.load" = load i64, ptr %"%src_net_2", align 8
  %cmpeqtmp41 = icmp eq i64 %"%dst_net_2.load", %"%src_net_2.load"
  br i1 %cmpeqtmp41, label %then_72, label %merge_73

then_72:                                          ; preds = %then_70
  %"%dst_net_3.load" = load i64, ptr %"%dst_net_3", align 8
  %"%src_net_3.load" = load i64, ptr %"%src_net_3", align 8
  %cmpeqtmp42 = icmp eq i64 %"%dst_net_3.load", %"%src_net_3.load"
  br i1 %cmpeqtmp42, label %then_74, label %merge_75

then_74:                                          ; preds = %then_72
  %loadtmp = load i64, ptr %dst_ip1, align 8
  ret i64 %loadtmp

then_76:                                          ; preds = %merge_69
  %"%dst_ip.load43" = load i64, ptr %dst_ip1, align 8
  %"%684.load" = load i64, ptr @route_dest_0, align 8
  %"%686.load" = load i64, ptr @route_mask_0, align 8
  %calltmp44 = call i64 @ip_route_matches(i64 %"%dst_ip.load43", i64 %"%684.load", i64 %"%686.load")
  store i64 %calltmp44, ptr %"%r0_match", align 8
  %"%r0_match.load" = load i64, ptr %"%r0_match", align 8
  %cmpgttmp45 = icmp sgt i64 %"%r0_match.load", 0
  br i1 %cmpgttmp45, label %then_78, label %merge_79

then_78:                                          ; preds = %then_76
  %loadtmp47 = load i64, ptr @route_gw_0, align 8
  ret i64 %loadtmp47

then_80:                                          ; preds = %merge_77
  %"%dst_ip.load48" = load i64, ptr %dst_ip1, align 8
  %"%698.load" = load i64, ptr @route_dest_1, align 8
  %"%700.load" = load i64, ptr @route_mask_1, align 8
  %calltmp49 = call i64 @ip_route_matches(i64 %"%dst_ip.load48", i64 %"%698.load", i64 %"%700.load")
  store i64 %calltmp49, ptr %"%r1_match", align 8
  %"%r1_match.load" = load i64, ptr %"%r1_match", align 8
  %cmpgttmp50 = icmp sgt i64 %"%r1_match.load", 0
  br i1 %cmpgttmp50, label %then_82, label %merge_83

then_82:                                          ; preds = %then_80
  %loadtmp52 = load i64, ptr @route_gw_1, align 8
  ret i64 %loadtmp52

then_84:                                          ; preds = %merge_81
  %"%dst_ip.load53" = load i64, ptr %dst_ip1, align 8
  %"%712.load" = load i64, ptr @route_dest_2, align 8
  %"%714.load" = load i64, ptr @route_mask_2, align 8
  %calltmp54 = call i64 @ip_route_matches(i64 %"%dst_ip.load53", i64 %"%712.load", i64 %"%714.load")
  store i64 %calltmp54, ptr %"%r2_match", align 8
  %"%r2_match.load" = load i64, ptr %"%r2_match", align 8
  %cmpgttmp55 = icmp sgt i64 %"%r2_match.load", 0
  br i1 %cmpgttmp55, label %then_86, label %merge_87

then_86:                                          ; preds = %then_84
  %loadtmp57 = load i64, ptr @route_gw_2, align 8
  ret i64 %loadtmp57

then_88:                                          ; preds = %merge_85
  %"%dst_ip.load58" = load i64, ptr %dst_ip1, align 8
  %"%726.load" = load i64, ptr @route_dest_3, align 8
  %"%728.load" = load i64, ptr @route_mask_3, align 8
  %calltmp59 = call i64 @ip_route_matches(i64 %"%dst_ip.load58", i64 %"%726.load", i64 %"%728.load")
  store i64 %calltmp59, ptr %"%r3_match", align 8
  %"%r3_match.load" = load i64, ptr %"%r3_match", align 8
  %cmpgttmp60 = icmp sgt i64 %"%r3_match.load", 0
  br i1 %cmpgttmp60, label %then_90, label %merge_91

then_90:                                          ; preds = %then_88
  %loadtmp63 = load i64, ptr @route_gw_3, align 8
  ret i64 %loadtmp63
}

define i64 @ip_route_matches(i64 %ip_val, i64 %route_dest, i64 %route_mask) {
entry:
  %"%masked_dest" = alloca i64, align 8
  %"%masked_ip" = alloca i64, align 8
  %"%mask_b" = alloca i64, align 8
  %"%dest_b" = alloca i64, align 8
  %"%ip_b" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %route_mask3 = alloca i64, align 8
  store i64 %route_mask, ptr %route_mask3, align 8
  %route_dest2 = alloca i64, align 8
  store i64 %route_dest, ptr %route_dest2, align 8
  %ip_val1 = alloca i64, align 8
  store i64 %ip_val, ptr %ip_val1, align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_92

merge_96:                                         ; preds = %while_body_93
  %"%i.load14" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load14", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_92

then_95:                                          ; preds = %while_body_93
  ret i64 0

while_body_93:                                    ; preds = %while_cond_92
  %"%ip_val.load" = load i64, ptr %ip_val1, align 8
  %"%i.load4" = load i64, ptr %"%i", align 8
  %calltmp = call i64 @ip_byte(i64 %"%ip_val.load", i64 %"%i.load4")
  store i64 %calltmp, ptr %"%ip_b", align 8
  %"%route_dest.load" = load i64, ptr %route_dest2, align 8
  %"%i.load5" = load i64, ptr %"%i", align 8
  %calltmp6 = call i64 @ip_byte(i64 %"%route_dest.load", i64 %"%i.load5")
  store i64 %calltmp6, ptr %"%dest_b", align 8
  %"%route_mask.load" = load i64, ptr %route_mask3, align 8
  %"%i.load7" = load i64, ptr %"%i", align 8
  %calltmp8 = call i64 @ip_byte(i64 %"%route_mask.load", i64 %"%i.load7")
  store i64 %calltmp8, ptr %"%mask_b", align 8
  %"%mask_b.load" = load i64, ptr %"%mask_b", align 8
  %subtmp = sub i64 256, %"%mask_b.load"
  %"%ip_b.load" = load i64, ptr %"%ip_b", align 8
  %modtmp = srem i64 %"%ip_b.load", %subtmp
  %"%ip_b.load9" = load i64, ptr %"%ip_b", align 8
  %subtmp10 = sub i64 %"%ip_b.load9", %modtmp
  store i64 %subtmp10, ptr %"%masked_ip", align 8
  %"%dest_b.load" = load i64, ptr %"%dest_b", align 8
  %modtmp11 = srem i64 %"%dest_b.load", %subtmp
  %"%dest_b.load12" = load i64, ptr %"%dest_b", align 8
  %subtmp13 = sub i64 %"%dest_b.load12", %modtmp11
  store i64 %subtmp13, ptr %"%masked_dest", align 8
  %"%masked_ip.load" = load i64, ptr %"%masked_ip", align 8
  %"%masked_dest.load" = load i64, ptr %"%masked_dest", align 8
  %cmpnetmp = icmp ne i64 %"%masked_ip.load", %"%masked_dest.load"
  br i1 %cmpnetmp, label %then_95, label %merge_96

while_cond_92:                                    ; preds = %merge_96, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %cmplttmp = icmp slt i64 %"%i.load", 4
  br i1 %cmplttmp, label %while_body_93, label %while_exit_94

while_exit_94:                                    ; preds = %while_cond_92
  ret i64 1
}

define i64 @ip_add_route(i64 %dest_ip, i64 %mask_ip, i64 %gw_ip) {
entry:
  %"%route_active_3" = alloca i64, align 8
  %"%route_gw_3" = alloca i64, align 8
  %"%route_mask_3" = alloca i64, align 8
  %"%route_dest_3" = alloca i64, align 8
  %"%route_active_2" = alloca i64, align 8
  %"%route_gw_2" = alloca i64, align 8
  %"%route_mask_2" = alloca i64, align 8
  %"%route_dest_2" = alloca i64, align 8
  %"%route_active_1" = alloca i64, align 8
  %"%route_gw_1" = alloca i64, align 8
  %"%route_mask_1" = alloca i64, align 8
  %"%route_dest_1" = alloca i64, align 8
  %"%route_active_0" = alloca i64, align 8
  %"%route_gw_0" = alloca i64, align 8
  %"%route_mask_0" = alloca i64, align 8
  %"%route_dest_0" = alloca i64, align 8
  %gw_ip3 = alloca i64, align 8
  store i64 %gw_ip, ptr %gw_ip3, align 8
  %mask_ip2 = alloca i64, align 8
  store i64 %mask_ip, ptr %mask_ip2, align 8
  %dest_ip1 = alloca i64, align 8
  store i64 %dest_ip, ptr %dest_ip1, align 8
  %"%route_active_0.load" = load i64, ptr @route_active_0, align 8
  %cmpeqtmp = icmp eq i64 %"%route_active_0.load", 0
  br i1 %cmpeqtmp, label %then_97, label %merge_98

merge_100:                                        ; preds = %merge_98
  %"%route_active_2.load" = load i64, ptr @route_active_2, align 8
  %cmpeqtmp5 = icmp eq i64 %"%route_active_2.load", 0
  br i1 %cmpeqtmp5, label %then_101, label %merge_102

merge_102:                                        ; preds = %merge_100
  %"%route_active_3.load" = load i64, ptr @route_active_3, align 8
  %cmpeqtmp6 = icmp eq i64 %"%route_active_3.load", 0
  br i1 %cmpeqtmp6, label %then_103, label %merge_104

merge_104:                                        ; preds = %merge_102
  ret i64 -1

merge_98:                                         ; preds = %entry
  %"%route_active_1.load" = load i64, ptr @route_active_1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%route_active_1.load", 0
  br i1 %cmpeqtmp4, label %then_99, label %merge_100

then_101:                                         ; preds = %merge_100
  %"%799.load" = load i64, ptr %dest_ip1, align 8
  store i64 %"%799.load", ptr %"%route_dest_2", align 8
  %"%801.load" = load i64, ptr %mask_ip2, align 8
  store i64 %"%801.load", ptr %"%route_mask_2", align 8
  %"%803.load" = load i64, ptr %gw_ip3, align 8
  store i64 %"%803.load", ptr %"%route_gw_2", align 8
  store i64 1, ptr %"%route_active_2", align 8
  ret i64 2

then_103:                                         ; preds = %merge_102
  %"%809.load" = load i64, ptr %dest_ip1, align 8
  store i64 %"%809.load", ptr %"%route_dest_3", align 8
  %"%811.load" = load i64, ptr %mask_ip2, align 8
  store i64 %"%811.load", ptr %"%route_mask_3", align 8
  %"%813.load" = load i64, ptr %gw_ip3, align 8
  store i64 %"%813.load", ptr %"%route_gw_3", align 8
  store i64 1, ptr %"%route_active_3", align 8
  ret i64 3

then_97:                                          ; preds = %entry
  %"%779.load" = load i64, ptr %dest_ip1, align 8
  store i64 %"%779.load", ptr %"%route_dest_0", align 8
  %"%781.load" = load i64, ptr %mask_ip2, align 8
  store i64 %"%781.load", ptr %"%route_mask_0", align 8
  %"%783.load" = load i64, ptr %gw_ip3, align 8
  store i64 %"%783.load", ptr %"%route_gw_0", align 8
  store i64 1, ptr %"%route_active_0", align 8
  ret i64 0

then_99:                                          ; preds = %merge_98
  %"%789.load" = load i64, ptr %dest_ip1, align 8
  store i64 %"%789.load", ptr %"%route_dest_1", align 8
  %"%791.load" = load i64, ptr %mask_ip2, align 8
  store i64 %"%791.load", ptr %"%route_mask_1", align 8
  %"%793.load" = load i64, ptr %gw_ip3, align 8
  store i64 %"%793.load", ptr %"%route_gw_1", align 8
  store i64 1, ptr %"%route_active_1", align 8
  ret i64 1
}

define i64 @ip_handle_packet(i64 %pkt_addr, i64 %pkt_len) {
entry:
  %"%src_ip" = alloca i64, align 8
  %"%payload_len" = alloca i64, align 8
  %"%payload_addr" = alloca i64, align 8
  %"%total_len" = alloca i64, align 8
  %"%ihl" = alloca i64, align 8
  %"%protocol" = alloca i64, align 8
  %"%ip_rx_count" = alloca i64, align 8
  %"%ip_dropped" = alloca i64, align 8
  %"%for_us" = alloca i64, align 8
  %"%ip_checksum_errors" = alloca i64, align 8
  %"%cksum_ok" = alloca i64, align 8
  %"%ip_rx_errors" = alloca i64, align 8
  %"%version" = alloca i64, align 8
  %"%ip_hdr" = alloca i64, align 8
  %pkt_len2 = alloca i64, align 8
  store i64 %pkt_len, ptr %pkt_len2, align 8
  %pkt_addr1 = alloca i64, align 8
  store i64 %pkt_addr, ptr %pkt_addr1, align 8
  %"%pkt_addr.load" = load i64, ptr %pkt_addr1, align 8
  %"%ETH_HEADER_SIZE.load" = load i64, ptr @ETH_HEADER_SIZE, align 8
  %addtmp = add i64 %"%pkt_addr.load", %"%ETH_HEADER_SIZE.load"
  store i64 %addtmp, ptr %"%ip_hdr", align 8
  %"%ip_hdr.load" = load i64, ptr %"%ip_hdr", align 8
  %calltmp = call i64 @ip_parse_version(i64 %"%ip_hdr.load")
  store i64 %calltmp, ptr %"%version", align 8
  %"%version.load" = load i64, ptr %"%version", align 8
  %cmpnetmp = icmp ne i64 %"%version.load", 4
  br i1 %cmpnetmp, label %then_105, label %merge_106

merge_106:                                        ; preds = %entry
  %"%ip_hdr.load8" = load i64, ptr %"%ip_hdr", align 8
  %calltmp9 = call i64 @ip_verify_checksum(i64 %"%ip_hdr.load8")
  store i64 %calltmp9, ptr %"%cksum_ok", align 8
  %"%cksum_ok.load" = load i64, ptr %"%cksum_ok", align 8
  %cmpeqtmp = icmp eq i64 %"%cksum_ok.load", 0
  br i1 %cmpeqtmp, label %then_107, label %merge_108

merge_108:                                        ; preds = %merge_106
  %"%ip_hdr.load15" = load i64, ptr %"%ip_hdr", align 8
  %calltmp16 = call i64 @ip_is_for_us(i64 %"%ip_hdr.load15")
  store i64 %calltmp16, ptr %"%for_us", align 8
  %"%for_us.load" = load i64, ptr %"%for_us", align 8
  %cmpeqtmp17 = icmp eq i64 %"%for_us.load", 0
  br i1 %cmpeqtmp17, label %then_109, label %merge_110

merge_110:                                        ; preds = %merge_108
  %"%ip_rx_count.load" = load i64, ptr @ip_rx_count, align 8
  %addtmp19 = add i64 %"%ip_rx_count.load", 1
  store i64 %addtmp19, ptr %"%ip_rx_count", align 8
  %"%ip_hdr.load20" = load i64, ptr %"%ip_hdr", align 8
  %calltmp21 = call i64 @ip_parse_protocol(i64 %"%ip_hdr.load20")
  store i64 %calltmp21, ptr %"%protocol", align 8
  %"%ip_hdr.load22" = load i64, ptr %"%ip_hdr", align 8
  %calltmp23 = call i64 @ip_parse_ihl(i64 %"%ip_hdr.load22")
  store i64 %calltmp23, ptr %"%ihl", align 8
  %"%ip_hdr.load24" = load i64, ptr %"%ip_hdr", align 8
  %calltmp25 = call i64 @ip_parse_total_length(i64 %"%ip_hdr.load24")
  store i64 %calltmp25, ptr %"%total_len", align 8
  %"%ip_hdr.load26" = load i64, ptr %"%ip_hdr", align 8
  %"%ihl.load" = load i64, ptr %"%ihl", align 8
  %addtmp27 = add i64 %"%ip_hdr.load26", %"%ihl.load"
  store i64 %addtmp27, ptr %"%payload_addr", align 8
  %"%total_len.load" = load i64, ptr %"%total_len", align 8
  %"%ihl.load28" = load i64, ptr %"%ihl", align 8
  %subtmp = sub i64 %"%total_len.load", %"%ihl.load28"
  store i64 %subtmp, ptr %"%payload_len", align 8
  %"%ip_hdr.load29" = load i64, ptr %"%ip_hdr", align 8
  %calltmp30 = call i64 @ip_parse_src(i64 %"%ip_hdr.load29")
  store i64 %calltmp30, ptr %"%src_ip", align 8
  %calltmp31 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  %"%protocol.load" = load i64, ptr %"%protocol", align 8
  %calltmp32 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%protocol.load")
  %calltmp33 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %"%src_ip.load" = load i64, ptr %"%src_ip", align 8
  %calltmp34 = call i64 @ip_byte(i64 %"%src_ip.load", i64 0)
  %calltmp35 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp34)
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 46)
  %"%src_ip.load36" = load i64, ptr %"%src_ip", align 8
  %calltmp37 = call i64 @ip_byte(i64 %"%src_ip.load36", i64 1)
  %calltmp38 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp37)
  %"%COM1.load39" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load39", i64 46)
  %"%src_ip.load40" = load i64, ptr %"%src_ip", align 8
  %calltmp41 = call i64 @ip_byte(i64 %"%src_ip.load40", i64 2)
  %calltmp42 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp41)
  %"%COM1.load43" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load43", i64 46)
  %"%src_ip.load44" = load i64, ptr %"%src_ip", align 8
  %calltmp45 = call i64 @ip_byte(i64 %"%src_ip.load44", i64 3)
  %calltmp46 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp45)
  %calltmp47 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %"%payload_len.load" = load i64, ptr %"%payload_len", align 8
  %calltmp48 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%payload_len.load")
  %calltmp49 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %"%protocol.load50" = load i64, ptr %"%protocol", align 8
  %"%IP_PROTO_ICMP.load" = load i64, ptr @IP_PROTO_ICMP, align 8
  %cmpeqtmp51 = icmp eq i64 %"%protocol.load50", %"%IP_PROTO_ICMP.load"
  br i1 %cmpeqtmp51, label %then_111, label %merge_112

merge_112:                                        ; preds = %merge_110
  %"%protocol.load55" = load i64, ptr %"%protocol", align 8
  %"%IP_PROTO_TCP.load" = load i64, ptr @IP_PROTO_TCP, align 8
  %cmpeqtmp56 = icmp eq i64 %"%protocol.load55", %"%IP_PROTO_TCP.load"
  br i1 %cmpeqtmp56, label %then_113, label %merge_114

merge_114:                                        ; preds = %merge_112
  %"%protocol.load59" = load i64, ptr %"%protocol", align 8
  %"%IP_PROTO_UDP.load" = load i64, ptr @IP_PROTO_UDP, align 8
  %cmpeqtmp60 = icmp eq i64 %"%protocol.load59", %"%IP_PROTO_UDP.load"
  br i1 %cmpeqtmp60, label %then_115, label %merge_116

merge_116:                                        ; preds = %merge_114
  %calltmp63 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %"%protocol.load64" = load i64, ptr %"%protocol", align 8
  %calltmp65 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%protocol.load64")
  %calltmp66 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_105:                                         ; preds = %entry
  %calltmp3 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%version.load4" = load i64, ptr %"%version", align 8
  %calltmp5 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%version.load4")
  %calltmp6 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %"%ip_rx_errors.load" = load i64, ptr @ip_rx_errors, align 8
  %addtmp7 = add i64 %"%ip_rx_errors.load", 1
  store i64 %addtmp7, ptr %"%ip_rx_errors", align 8
  ret i64 0

then_107:                                         ; preds = %merge_106
  %calltmp10 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %calltmp11 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %"%ip_checksum_errors.load" = load i64, ptr @ip_checksum_errors, align 8
  %addtmp12 = add i64 %"%ip_checksum_errors.load", 1
  store i64 %addtmp12, ptr %"%ip_checksum_errors", align 8
  %"%ip_rx_errors.load13" = load i64, ptr %"%ip_rx_errors", align 8
  %addtmp14 = add i64 %"%ip_rx_errors.load13", 1
  store i64 %addtmp14, ptr %"%ip_rx_errors", align 8
  ret i64 0

then_109:                                         ; preds = %merge_108
  %"%ip_dropped.load" = load i64, ptr @ip_dropped, align 8
  %addtmp18 = add i64 %"%ip_dropped.load", 1
  store i64 %addtmp18, ptr %"%ip_dropped", align 8
  ret i64 0

then_111:                                         ; preds = %merge_110
  %"%payload_addr.load" = load i64, ptr %"%payload_addr", align 8
  %"%payload_len.load52" = load i64, ptr %"%payload_len", align 8
  %"%src_ip.load53" = load i64, ptr %"%src_ip", align 8
  %calltmp54 = call i64 @ip_handle_icmp(i64 %"%payload_addr.load", i64 %"%payload_len.load52", i64 %"%src_ip.load53")
  ret i64 %calltmp54

then_113:                                         ; preds = %merge_112
  %calltmp57 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  %calltmp58 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1

then_115:                                         ; preds = %merge_114
  %calltmp61 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %calltmp62 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1
}

define i64 @icmp_checksum(i64 %buf, i64 %length) {
entry:
  %length2 = alloca i64, align 8
  store i64 %length, ptr %length2, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  %"%length.load" = load i64, ptr %length2, align 8
  %calltmp = call i64 @ip_checksum(i64 %"%buf.load", i64 %"%length.load")
  ret i64 %calltmp
}

define i64 @ip_handle_icmp(i64 %payload_addr, i64 %payload_len, i64 %src_ip) {
entry:
  %"%icmp_code" = alloca i64, align 8
  %"%icmp_type" = alloca i64, align 8
  %src_ip3 = alloca i64, align 8
  store i64 %src_ip, ptr %src_ip3, align 8
  %payload_len2 = alloca i64, align 8
  store i64 %payload_len, ptr %payload_len2, align 8
  %payload_addr1 = alloca i64, align 8
  store i64 %payload_addr, ptr %payload_addr1, align 8
  %"%payload_len.load" = load i64, ptr %payload_len2, align 8
  %cmplttmp = icmp slt i64 %"%payload_len.load", 8
  br i1 %cmplttmp, label %then_117, label %merge_118

merge_118:                                        ; preds = %entry
  %"%payload_addr.load" = load i64, ptr %payload_addr1, align 8
  call void @sad_ll_mem_read8(i64 %"%payload_addr.load")
  store i64 0, ptr %"%icmp_type", align 8
  %"%payload_addr.load5" = load i64, ptr %payload_addr1, align 8
  %addtmp = add i64 %"%payload_addr.load5", 1
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%icmp_code", align 8
  %"%icmp_type.load" = load i64, ptr %"%icmp_type", align 8
  %"%ICMP_ECHO_REQUEST.load" = load i64, ptr @ICMP_ECHO_REQUEST, align 8
  %cmpeqtmp = icmp eq i64 %"%icmp_type.load", %"%ICMP_ECHO_REQUEST.load"
  br i1 %cmpeqtmp, label %then_119, label %merge_120

merge_120:                                        ; preds = %merge_118
  %"%icmp_type.load25" = load i64, ptr %"%icmp_type", align 8
  %"%ICMP_ECHO_REPLY.load" = load i64, ptr @ICMP_ECHO_REPLY, align 8
  %cmpeqtmp26 = icmp eq i64 %"%icmp_type.load25", %"%ICMP_ECHO_REPLY.load"
  br i1 %cmpeqtmp26, label %then_121, label %merge_122

merge_122:                                        ; preds = %merge_120
  %calltmp44 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%icmp_type.load45" = load i64, ptr %"%icmp_type", align 8
  %calltmp46 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%icmp_type.load45")
  %calltmp47 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  %"%icmp_code.load" = load i64, ptr %"%icmp_code", align 8
  %calltmp48 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%icmp_code.load")
  %calltmp49 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_117:                                         ; preds = %entry
  %calltmp = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %calltmp4 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_119:                                         ; preds = %merge_118
  %calltmp6 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %"%src_ip.load" = load i64, ptr %src_ip3, align 8
  %calltmp7 = call i64 @ip_byte(i64 %"%src_ip.load", i64 0)
  %calltmp8 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp7)
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 46)
  %"%src_ip.load9" = load i64, ptr %src_ip3, align 8
  %calltmp10 = call i64 @ip_byte(i64 %"%src_ip.load9", i64 1)
  %calltmp11 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp10)
  %"%COM1.load12" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load12", i64 46)
  %"%src_ip.load13" = load i64, ptr %src_ip3, align 8
  %calltmp14 = call i64 @ip_byte(i64 %"%src_ip.load13", i64 2)
  %calltmp15 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp14)
  %"%COM1.load16" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load16", i64 46)
  %"%src_ip.load17" = load i64, ptr %src_ip3, align 8
  %calltmp18 = call i64 @ip_byte(i64 %"%src_ip.load17", i64 3)
  %calltmp19 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp18)
  %calltmp20 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %"%src_ip.load21" = load i64, ptr %src_ip3, align 8
  %"%payload_addr.load22" = load i64, ptr %payload_addr1, align 8
  %"%payload_len.load23" = load i64, ptr %payload_len2, align 8
  %calltmp24 = call i64 @ip_send_icmp_reply(i64 %"%src_ip.load21", i64 %"%payload_addr.load22", i64 %"%payload_len.load23")
  ret i64 %calltmp24

then_121:                                         ; preds = %merge_120
  %calltmp27 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  %"%src_ip.load28" = load i64, ptr %src_ip3, align 8
  %calltmp29 = call i64 @ip_byte(i64 %"%src_ip.load28", i64 0)
  %calltmp30 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp29)
  %"%COM1.load31" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load31", i64 46)
  %"%src_ip.load32" = load i64, ptr %src_ip3, align 8
  %calltmp33 = call i64 @ip_byte(i64 %"%src_ip.load32", i64 1)
  %calltmp34 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp33)
  %"%COM1.load35" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load35", i64 46)
  %"%src_ip.load36" = load i64, ptr %src_ip3, align 8
  %calltmp37 = call i64 @ip_byte(i64 %"%src_ip.load36", i64 2)
  %calltmp38 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp37)
  %"%COM1.load39" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load39", i64 46)
  %"%src_ip.load40" = load i64, ptr %src_ip3, align 8
  %calltmp41 = call i64 @ip_byte(i64 %"%src_ip.load40", i64 3)
  %calltmp42 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp41)
  %calltmp43 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1
}

define i64 @ip_send_icmp_reply(i64 %dst_ip, i64 %req_payload, i64 %req_len) {
entry:
  %"%result" = alloca i64, align 8
  %"%cksum" = alloca i64, align 8
  %"%reply_buf" = alloca i64, align 8
  %req_len3 = alloca i64, align 8
  store i64 %req_len, ptr %req_len3, align 8
  %req_payload2 = alloca i64, align 8
  store i64 %req_payload, ptr %req_payload2, align 8
  %dst_ip1 = alloca i64, align 8
  store i64 %dst_ip, ptr %dst_ip1, align 8
  %"%req_len.load" = load i64, ptr %req_len3, align 8
  call void @sad_ll_kmalloc(i64 %"%req_len.load")
  store i64 0, ptr %"%reply_buf", align 8
  %"%reply_buf.load" = load i64, ptr %"%reply_buf", align 8
  %cmpeqtmp = icmp eq i64 %"%reply_buf.load", 0
  br i1 %cmpeqtmp, label %then_123, label %merge_124

merge_124:                                        ; preds = %entry
  %"%reply_buf.load4" = load i64, ptr %"%reply_buf", align 8
  %"%req_payload.load" = load i64, ptr %req_payload2, align 8
  %"%req_len.load5" = load i64, ptr %req_len3, align 8
  call void @sad_ll_memcpy(i64 %"%reply_buf.load4", i64 %"%req_payload.load", i64 %"%req_len.load5")
  %"%reply_buf.load6" = load i64, ptr %"%reply_buf", align 8
  %"%ICMP_ECHO_REPLY.load" = load i64, ptr @ICMP_ECHO_REPLY, align 8
  call void @sad_ll_mem_write8(i64 %"%reply_buf.load6", i64 %"%ICMP_ECHO_REPLY.load")
  %"%reply_buf.load7" = load i64, ptr %"%reply_buf", align 8
  %addtmp = add i64 %"%reply_buf.load7", 1
  call void @sad_ll_mem_write8(i64 %addtmp, i64 0)
  %"%reply_buf.load8" = load i64, ptr %"%reply_buf", align 8
  %addtmp9 = add i64 %"%reply_buf.load8", 2
  call void @sad_ll_mem_write8(i64 %addtmp9, i64 0)
  %"%reply_buf.load10" = load i64, ptr %"%reply_buf", align 8
  %addtmp11 = add i64 %"%reply_buf.load10", 3
  call void @sad_ll_mem_write8(i64 %addtmp11, i64 0)
  %"%reply_buf.load12" = load i64, ptr %"%reply_buf", align 8
  %"%req_len.load13" = load i64, ptr %req_len3, align 8
  %calltmp = call i64 @icmp_checksum(i64 %"%reply_buf.load12", i64 %"%req_len.load13")
  store i64 %calltmp, ptr %"%cksum", align 8
  %"%cksum.load" = load i64, ptr %"%cksum", align 8
  %shrtmp = lshr i64 %"%cksum.load", 8
  call void @sad_ll_mem_write8(i64 %addtmp9, i64 %shrtmp)
  %"%cksum.load14" = load i64, ptr %"%cksum", align 8
  %andtmp = and i64 %"%cksum.load14", 255
  call void @sad_ll_mem_write8(i64 %addtmp11, i64 %andtmp)
  %"%dst_ip.load" = load i64, ptr %dst_ip1, align 8
  %"%IP_PROTO_ICMP.load" = load i64, ptr @IP_PROTO_ICMP, align 8
  %"%reply_buf.load15" = load i64, ptr %"%reply_buf", align 8
  %"%req_len.load16" = load i64, ptr %req_len3, align 8
  %calltmp17 = call i64 @ip_send(i64 %"%dst_ip.load", i64 %"%IP_PROTO_ICMP.load", i64 %"%reply_buf.load15", i64 %"%req_len.load16")
  store i64 %calltmp17, ptr %"%result", align 8
  %"%reply_buf.load18" = load i64, ptr %"%reply_buf", align 8
  call void @sad_ll_kfree(i64 %"%reply_buf.load18")
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

then_123:                                         ; preds = %entry
  ret i64 0
}

define i64 @ip_ping(i64 %dst_ip, i64 %seq_num) {
entry:
  %"%result" = alloca i64, align 8
  %"%cksum" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %"%icmp_buf" = alloca i64, align 8
  %"%icmp_len" = alloca i64, align 8
  %"%PING_DATA_SIZE" = alloca i64, align 8
  %seq_num2 = alloca i64, align 8
  store i64 %seq_num, ptr %seq_num2, align 8
  %dst_ip1 = alloca i64, align 8
  store i64 %dst_ip, ptr %dst_ip1, align 8
  store i64 32, ptr %"%PING_DATA_SIZE", align 8
  %"%PING_DATA_SIZE.load" = load i64, ptr %"%PING_DATA_SIZE", align 8
  %addtmp = add i64 8, %"%PING_DATA_SIZE.load"
  store i64 %addtmp, ptr %"%icmp_len", align 8
  %"%icmp_len.load" = load i64, ptr %"%icmp_len", align 8
  call void @sad_ll_kmalloc(i64 %"%icmp_len.load")
  store i64 0, ptr %"%icmp_buf", align 8
  %"%icmp_buf.load" = load i64, ptr %"%icmp_buf", align 8
  %cmpeqtmp = icmp eq i64 %"%icmp_buf.load", 0
  br i1 %cmpeqtmp, label %then_125, label %merge_126

merge_126:                                        ; preds = %entry
  %"%icmp_buf.load3" = load i64, ptr %"%icmp_buf", align 8
  %"%icmp_len.load4" = load i64, ptr %"%icmp_len", align 8
  call void @sad_ll_memset(i64 %"%icmp_buf.load3", i64 0, i64 %"%icmp_len.load4")
  %"%icmp_buf.load5" = load i64, ptr %"%icmp_buf", align 8
  %"%ICMP_ECHO_REQUEST.load" = load i64, ptr @ICMP_ECHO_REQUEST, align 8
  call void @sad_ll_mem_write8(i64 %"%icmp_buf.load5", i64 %"%ICMP_ECHO_REQUEST.load")
  %"%icmp_buf.load6" = load i64, ptr %"%icmp_buf", align 8
  %addtmp7 = add i64 %"%icmp_buf.load6", 1
  call void @sad_ll_mem_write8(i64 %addtmp7, i64 0)
  %"%icmp_buf.load8" = load i64, ptr %"%icmp_buf", align 8
  %addtmp9 = add i64 %"%icmp_buf.load8", 2
  call void @sad_ll_mem_write8(i64 %addtmp9, i64 0)
  %"%icmp_buf.load10" = load i64, ptr %"%icmp_buf", align 8
  %addtmp11 = add i64 %"%icmp_buf.load10", 3
  call void @sad_ll_mem_write8(i64 %addtmp11, i64 0)
  %"%icmp_buf.load12" = load i64, ptr %"%icmp_buf", align 8
  %addtmp13 = add i64 %"%icmp_buf.load12", 4
  call void @sad_ll_mem_write8(i64 %addtmp13, i64 0)
  %"%icmp_buf.load14" = load i64, ptr %"%icmp_buf", align 8
  %addtmp15 = add i64 %"%icmp_buf.load14", 5
  call void @sad_ll_mem_write8(i64 %addtmp15, i64 1)
  %"%icmp_buf.load16" = load i64, ptr %"%icmp_buf", align 8
  %addtmp17 = add i64 %"%icmp_buf.load16", 6
  %"%seq_num.load" = load i64, ptr %seq_num2, align 8
  %shrtmp = lshr i64 %"%seq_num.load", 8
  call void @sad_ll_mem_write8(i64 %addtmp17, i64 %shrtmp)
  %"%icmp_buf.load18" = load i64, ptr %"%icmp_buf", align 8
  %addtmp19 = add i64 %"%icmp_buf.load18", 7
  %"%seq_num.load20" = load i64, ptr %seq_num2, align 8
  %andtmp = and i64 %"%seq_num.load20", 255
  call void @sad_ll_mem_write8(i64 %addtmp19, i64 %andtmp)
  store i64 0, ptr %"%i", align 8
  br label %while_cond_127

merge_131:                                        ; preds = %then_130, %while_exit_129
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

then_125:                                         ; preds = %entry
  ret i64 0

then_130:                                         ; preds = %while_exit_129
  %calltmp44 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %"%dst_ip.load45" = load i64, ptr %dst_ip1, align 8
  %calltmp46 = call i64 @ip_byte(i64 %"%dst_ip.load45", i64 0)
  %calltmp47 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp46)
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 46)
  %"%dst_ip.load48" = load i64, ptr %dst_ip1, align 8
  %calltmp49 = call i64 @ip_byte(i64 %"%dst_ip.load48", i64 1)
  %calltmp50 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp49)
  %"%COM1.load51" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load51", i64 46)
  %"%dst_ip.load52" = load i64, ptr %dst_ip1, align 8
  %calltmp53 = call i64 @ip_byte(i64 %"%dst_ip.load52", i64 2)
  %calltmp54 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp53)
  %"%COM1.load55" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load55", i64 46)
  %"%dst_ip.load56" = load i64, ptr %dst_ip1, align 8
  %calltmp57 = call i64 @ip_byte(i64 %"%dst_ip.load56", i64 3)
  %calltmp58 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp57)
  %calltmp59 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %"%seq_num.load60" = load i64, ptr %seq_num2, align 8
  %calltmp61 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%seq_num.load60")
  %calltmp62 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_131

while_body_128:                                   ; preds = %while_cond_127
  %"%icmp_buf.load22" = load i64, ptr %"%icmp_buf", align 8
  %addtmp23 = add i64 %"%icmp_buf.load22", 8
  %"%i.load24" = load i64, ptr %"%i", align 8
  %addtmp25 = add i64 %addtmp23, %"%i.load24"
  %"%i.load26" = load i64, ptr %"%i", align 8
  %addtmp27 = add i64 %"%i.load26", 65
  %andtmp28 = and i64 %addtmp27, 255
  call void @sad_ll_mem_write8(i64 %addtmp25, i64 %andtmp28)
  %"%i.load29" = load i64, ptr %"%i", align 8
  %addtmp30 = add i64 %"%i.load29", 1
  store i64 %addtmp30, ptr %"%i", align 8
  br label %while_cond_127

while_cond_127:                                   ; preds = %while_body_128, %merge_126
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%PING_DATA_SIZE.load21" = load i64, ptr %"%PING_DATA_SIZE", align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%PING_DATA_SIZE.load21"
  br i1 %cmplttmp, label %while_body_128, label %while_exit_129

while_exit_129:                                   ; preds = %while_cond_127
  %"%icmp_buf.load31" = load i64, ptr %"%icmp_buf", align 8
  %"%icmp_len.load32" = load i64, ptr %"%icmp_len", align 8
  %calltmp = call i64 @icmp_checksum(i64 %"%icmp_buf.load31", i64 %"%icmp_len.load32")
  store i64 %calltmp, ptr %"%cksum", align 8
  %"%icmp_buf.load33" = load i64, ptr %"%icmp_buf", align 8
  %addtmp34 = add i64 %"%icmp_buf.load33", 2
  %"%cksum.load" = load i64, ptr %"%cksum", align 8
  %shrtmp35 = lshr i64 %"%cksum.load", 8
  call void @sad_ll_mem_write8(i64 %addtmp34, i64 %shrtmp35)
  %"%icmp_buf.load36" = load i64, ptr %"%icmp_buf", align 8
  %addtmp37 = add i64 %"%icmp_buf.load36", 3
  %"%cksum.load38" = load i64, ptr %"%cksum", align 8
  %andtmp39 = and i64 %"%cksum.load38", 255
  call void @sad_ll_mem_write8(i64 %addtmp37, i64 %andtmp39)
  %"%dst_ip.load" = load i64, ptr %dst_ip1, align 8
  %"%IP_PROTO_ICMP.load" = load i64, ptr @IP_PROTO_ICMP, align 8
  %"%icmp_buf.load40" = load i64, ptr %"%icmp_buf", align 8
  %"%icmp_len.load41" = load i64, ptr %"%icmp_len", align 8
  %calltmp42 = call i64 @ip_send(i64 %"%dst_ip.load", i64 %"%IP_PROTO_ICMP.load", i64 %"%icmp_buf.load40", i64 %"%icmp_len.load41")
  store i64 %calltmp42, ptr %"%result", align 8
  %"%icmp_buf.load43" = load i64, ptr %"%icmp_buf", align 8
  call void @sad_ll_kfree(i64 %"%icmp_buf.load43")
  %"%result.load" = load i64, ptr %"%result", align 8
  %cmpgttmp = icmp sgt i64 %"%result.load", 0
  br i1 %cmpgttmp, label %then_130, label %merge_131
}

define i64 @"ip_\D8\B9\D9\8A\D9\86_\D8\B9\D9\86\D9\88\D8\A7\D9\86"(i64 %a, i64 %b, i64 %c, i64 %d) {
entry:
  %"%local_ip_3" = alloca i64, align 8
  %"%local_ip_2" = alloca i64, align 8
  %"%local_ip_1" = alloca i64, align 8
  %"%local_ip_0" = alloca i64, align 8
  %d4 = alloca i64, align 8
  store i64 %d, ptr %d4, align 8
  %c3 = alloca i64, align 8
  store i64 %c, ptr %c3, align 8
  %b2 = alloca i64, align 8
  store i64 %b, ptr %b2, align 8
  %a1 = alloca i64, align 8
  store i64 %a, ptr %a1, align 8
  %"%1129.load" = load i64, ptr %a1, align 8
  store i64 %"%1129.load", ptr %"%local_ip_0", align 8
  %"%1131.load" = load i64, ptr %b2, align 8
  store i64 %"%1131.load", ptr %"%local_ip_1", align 8
  %"%1133.load" = load i64, ptr %c3, align 8
  store i64 %"%1133.load", ptr %"%local_ip_2", align 8
  %"%1135.load" = load i64, ptr %d4, align 8
  store i64 %"%1135.load", ptr %"%local_ip_3", align 8
  ret i64 1
}

define i64 @"ip_\D8\B9\D9\8A\D9\86_\D9\82\D9\86\D8\A7\D8\B9"(i64 %a, i64 %b, i64 %c, i64 %d) {
entry:
  %"%local_mask_3" = alloca i64, align 8
  %"%local_mask_2" = alloca i64, align 8
  %"%local_mask_1" = alloca i64, align 8
  %"%local_mask_0" = alloca i64, align 8
  %d4 = alloca i64, align 8
  store i64 %d, ptr %d4, align 8
  %c3 = alloca i64, align 8
  store i64 %c, ptr %c3, align 8
  %b2 = alloca i64, align 8
  store i64 %b, ptr %b2, align 8
  %a1 = alloca i64, align 8
  store i64 %a, ptr %a1, align 8
  %"%1138.load" = load i64, ptr %a1, align 8
  store i64 %"%1138.load", ptr %"%local_mask_0", align 8
  %"%1140.load" = load i64, ptr %b2, align 8
  store i64 %"%1140.load", ptr %"%local_mask_1", align 8
  %"%1142.load" = load i64, ptr %c3, align 8
  store i64 %"%1142.load", ptr %"%local_mask_2", align 8
  %"%1144.load" = load i64, ptr %d4, align 8
  store i64 %"%1144.load", ptr %"%local_mask_3", align 8
  ret i64 1
}

define i64 @"ip_\D8\B9\D9\8A\D9\86_\D8\A8\D9\88\D8\A7\D8\A8\D8\A9"(i64 %a, i64 %b, i64 %c, i64 %d) {
entry:
  %"%local_gw_3" = alloca i64, align 8
  %"%local_gw_2" = alloca i64, align 8
  %"%local_gw_1" = alloca i64, align 8
  %"%local_gw_0" = alloca i64, align 8
  %d4 = alloca i64, align 8
  store i64 %d, ptr %d4, align 8
  %c3 = alloca i64, align 8
  store i64 %c, ptr %c3, align 8
  %b2 = alloca i64, align 8
  store i64 %b, ptr %b2, align 8
  %a1 = alloca i64, align 8
  store i64 %a, ptr %a1, align 8
  %"%1147.load" = load i64, ptr %a1, align 8
  store i64 %"%1147.load", ptr %"%local_gw_0", align 8
  %"%1149.load" = load i64, ptr %b2, align 8
  store i64 %"%1149.load", ptr %"%local_gw_1", align 8
  %"%1151.load" = load i64, ptr %c3, align 8
  store i64 %"%1151.load", ptr %"%local_gw_2", align 8
  %"%1153.load" = load i64, ptr %d4, align 8
  store i64 %"%1153.load", ptr %"%local_gw_3", align 8
  ret i64 1
}

define i64 @"ip_\D8\B9\D9\8A\D9\86_mac"(i64 %m0, i64 %m1, i64 %m2, i64 %m3, i64 %m4, i64 %m5) {
entry:
  %"%local_mac_5" = alloca i64, align 8
  %"%local_mac_4" = alloca i64, align 8
  %"%local_mac_3" = alloca i64, align 8
  %"%local_mac_2" = alloca i64, align 8
  %"%local_mac_1" = alloca i64, align 8
  %"%local_mac_0" = alloca i64, align 8
  %m56 = alloca i64, align 8
  store i64 %m5, ptr %m56, align 8
  %m45 = alloca i64, align 8
  store i64 %m4, ptr %m45, align 8
  %m34 = alloca i64, align 8
  store i64 %m3, ptr %m34, align 8
  %m23 = alloca i64, align 8
  store i64 %m2, ptr %m23, align 8
  %m12 = alloca i64, align 8
  store i64 %m1, ptr %m12, align 8
  %m01 = alloca i64, align 8
  store i64 %m0, ptr %m01, align 8
  %"%1156.load" = load i64, ptr %m01, align 8
  store i64 %"%1156.load", ptr %"%local_mac_0", align 8
  %"%1158.load" = load i64, ptr %m12, align 8
  store i64 %"%1158.load", ptr %"%local_mac_1", align 8
  %"%1160.load" = load i64, ptr %m23, align 8
  store i64 %"%1160.load", ptr %"%local_mac_2", align 8
  %"%1162.load" = load i64, ptr %m34, align 8
  store i64 %"%1162.load", ptr %"%local_mac_3", align 8
  %"%1164.load" = load i64, ptr %m45, align 8
  store i64 %"%1164.load", ptr %"%local_mac_4", align 8
  %"%1166.load" = load i64, ptr %m56, align 8
  store i64 %"%1166.load", ptr %"%local_mac_5", align 8
  ret i64 1
}

define i64 @"ip_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %calltmp = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  %calltmp1 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  %"%1174.load" = load i64, ptr @local_ip_0, align 8
  %calltmp3 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1174.load")
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 46)
  %"%1179.load" = load i64, ptr @local_ip_1, align 8
  %calltmp4 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1179.load")
  %"%COM1.load5" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load5", i64 46)
  %"%1184.load" = load i64, ptr @local_ip_2, align 8
  %calltmp6 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1184.load")
  %"%COM1.load7" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load7", i64 46)
  %"%1189.load" = load i64, ptr @local_ip_3, align 8
  %calltmp8 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1189.load")
  %calltmp9 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp10 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  %"%1195.load" = load i64, ptr @local_mask_0, align 8
  %calltmp11 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1195.load")
  %"%COM1.load12" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load12", i64 46)
  %"%1200.load" = load i64, ptr @local_mask_1, align 8
  %calltmp13 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1200.load")
  %"%COM1.load14" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load14", i64 46)
  %"%1205.load" = load i64, ptr @local_mask_2, align 8
  %calltmp15 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1205.load")
  %"%COM1.load16" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load16", i64 46)
  %"%1210.load" = load i64, ptr @local_mask_3, align 8
  %calltmp17 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1210.load")
  %calltmp18 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp19 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  %"%1216.load" = load i64, ptr @local_gw_0, align 8
  %calltmp20 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1216.load")
  %"%COM1.load21" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load21", i64 46)
  %"%1221.load" = load i64, ptr @local_gw_1, align 8
  %calltmp22 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1221.load")
  %"%COM1.load23" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load23", i64 46)
  %"%1226.load" = load i64, ptr @local_gw_2, align 8
  %calltmp24 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1226.load")
  %"%COM1.load25" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load25", i64 46)
  %"%1231.load" = load i64, ptr @local_gw_3, align 8
  %calltmp26 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1231.load")
  %calltmp27 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp28 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  %"%1237.load" = load i64, ptr @ip_tx_count, align 8
  %calltmp29 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1237.load")
  %calltmp30 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp31 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %"%1243.load" = load i64, ptr @ip_rx_count, align 8
  %calltmp32 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1243.load")
  %calltmp33 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp34 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  %"%1249.load" = load i64, ptr @ip_tx_errors, align 8
  %calltmp35 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1249.load")
  %calltmp36 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp37 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  %"%1255.load" = load i64, ptr @ip_rx_errors, align 8
  %calltmp38 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1255.load")
  %calltmp39 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp40 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  %"%1261.load" = load i64, ptr @ip_checksum_errors, align 8
  %calltmp41 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1261.load")
  %calltmp42 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp43 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  %"%1267.load" = load i64, ptr @ip_dropped, align 8
  %calltmp44 = call i64 @"ip_serial_\D8\B1\D9\82\D9\85"(i64 %"%1267.load")
  %calltmp45 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp46 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  %calltmp47 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @"ip_\D9\87\D9\8A\D8\A6"() {
entry:
  %"%route_active_3" = alloca i64, align 8
  %"%route_active_2" = alloca i64, align 8
  %"%route_active_1" = alloca i64, align 8
  %"%route_active_0" = alloca i64, align 8
  %"%ip_packet_id" = alloca i64, align 8
  %"%ip_dropped" = alloca i64, align 8
  %"%ip_forwarded" = alloca i64, align 8
  %"%ip_ttl_expired" = alloca i64, align 8
  %"%ip_checksum_errors" = alloca i64, align 8
  %"%ip_tx_errors" = alloca i64, align 8
  %"%ip_rx_errors" = alloca i64, align 8
  %"%ip_rx_count" = alloca i64, align 8
  %"%ip_tx_count" = alloca i64, align 8
  %calltmp = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.30 to i64))
  %calltmp1 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%ip_tx_count", align 8
  store i64 0, ptr %"%ip_rx_count", align 8
  store i64 0, ptr %"%ip_rx_errors", align 8
  store i64 0, ptr %"%ip_tx_errors", align 8
  store i64 0, ptr %"%ip_checksum_errors", align 8
  store i64 0, ptr %"%ip_ttl_expired", align 8
  store i64 0, ptr %"%ip_forwarded", align 8
  store i64 0, ptr %"%ip_dropped", align 8
  store i64 1, ptr %"%ip_packet_id", align 8
  store i64 0, ptr %"%route_active_0", align 8
  store i64 0, ptr %"%route_active_1", align 8
  store i64 0, ptr %"%route_active_2", align 8
  store i64 0, ptr %"%route_active_3", align 8
  %calltmp2 = call i64 @"ip_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  %calltmp3 = call i64 @"ip_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1
}

define i64 @"ip_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  ret i64 0
}

define void @__sad_main() {
entry:
  %"%ICMP_ECHO_REPLY" = alloca i64, align 8
  %"%ICMP_ECHO_REQUEST" = alloca i64, align 8
  %"%route_active_3" = alloca i64, align 8
  %"%route_active_2" = alloca i64, align 8
  %"%route_active_1" = alloca i64, align 8
  %"%route_active_0" = alloca i64, align 8
  %"%route_gw_3" = alloca i64, align 8
  %"%route_gw_2" = alloca i64, align 8
  %"%route_gw_1" = alloca i64, align 8
  %"%route_gw_0" = alloca i64, align 8
  %"%route_mask_3" = alloca i64, align 8
  %"%route_mask_2" = alloca i64, align 8
  %"%route_mask_1" = alloca i64, align 8
  %"%route_mask_0" = alloca i64, align 8
  %"%route_dest_3" = alloca i64, align 8
  %"%route_dest_2" = alloca i64, align 8
  %"%route_dest_1" = alloca i64, align 8
  %"%route_dest_0" = alloca i64, align 8
  %"%ip_dropped" = alloca i64, align 8
  %"%ip_forwarded" = alloca i64, align 8
  %"%ip_ttl_expired" = alloca i64, align 8
  %"%ip_checksum_errors" = alloca i64, align 8
  %"%ip_tx_errors" = alloca i64, align 8
  %"%ip_rx_errors" = alloca i64, align 8
  %"%ip_rx_count" = alloca i64, align 8
  %"%ip_tx_count" = alloca i64, align 8
  %"%ip_packet_id" = alloca i64, align 8
  %"%local_mac_5" = alloca i64, align 8
  %"%local_mac_4" = alloca i64, align 8
  %"%local_mac_3" = alloca i64, align 8
  %"%local_mac_2" = alloca i64, align 8
  %"%local_mac_1" = alloca i64, align 8
  %"%local_mac_0" = alloca i64, align 8
  %"%local_gw_3" = alloca i64, align 8
  %"%local_gw_2" = alloca i64, align 8
  %"%local_gw_1" = alloca i64, align 8
  %"%local_gw_0" = alloca i64, align 8
  %"%local_mask_3" = alloca i64, align 8
  %"%local_mask_2" = alloca i64, align 8
  %"%local_mask_1" = alloca i64, align 8
  %"%local_mask_0" = alloca i64, align 8
  %"%local_ip_3" = alloca i64, align 8
  %"%local_ip_2" = alloca i64, align 8
  %"%local_ip_1" = alloca i64, align 8
  %"%local_ip_0" = alloca i64, align 8
  %"%ARP_PERMANENT" = alloca i64, align 8
  %"%ARP_RESOLVED" = alloca i64, align 8
  %"%IP_FLAG_MF" = alloca i64, align 8
  %"%IP_FLAG_DF" = alloca i64, align 8
  %"%IP_MAX_PACKET" = alloca i64, align 8
  %"%IP_TTL_DEFAULT" = alloca i64, align 8
  %"%IP_IHL" = alloca i64, align 8
  %"%IP_VERSION" = alloca i64, align 8
  %"%IP_HEADER_SIZE" = alloca i64, align 8
  %"%ETH_HEADER_SIZE" = alloca i64, align 8
  %"%IP_PROTO_UDP" = alloca i64, align 8
  %"%IP_PROTO_TCP" = alloca i64, align 8
  %"%IP_PROTO_ICMP" = alloca i64, align 8
  %"%ETHER_TYPE_IP" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 2048, ptr %"%ETHER_TYPE_IP", align 8
  store i64 1, ptr %"%IP_PROTO_ICMP", align 8
  store i64 6, ptr %"%IP_PROTO_TCP", align 8
  store i64 17, ptr %"%IP_PROTO_UDP", align 8
  store i64 14, ptr %"%ETH_HEADER_SIZE", align 8
  store i64 20, ptr %"%IP_HEADER_SIZE", align 8
  store i64 4, ptr %"%IP_VERSION", align 8
  store i64 5, ptr %"%IP_IHL", align 8
  store i64 64, ptr %"%IP_TTL_DEFAULT", align 8
  store i64 1500, ptr %"%IP_MAX_PACKET", align 8
  store i64 2, ptr %"%IP_FLAG_DF", align 8
  store i64 1, ptr %"%IP_FLAG_MF", align 8
  store i64 2, ptr %"%ARP_RESOLVED", align 8
  store i64 3, ptr %"%ARP_PERMANENT", align 8
  store i64 0, ptr %"%local_ip_0", align 8
  store i64 0, ptr %"%local_ip_1", align 8
  store i64 0, ptr %"%local_ip_2", align 8
  store i64 0, ptr %"%local_ip_3", align 8
  store i64 255, ptr %"%local_mask_0", align 8
  store i64 255, ptr %"%local_mask_1", align 8
  store i64 255, ptr %"%local_mask_2", align 8
  store i64 0, ptr %"%local_mask_3", align 8
  store i64 0, ptr %"%local_gw_0", align 8
  store i64 0, ptr %"%local_gw_1", align 8
  store i64 0, ptr %"%local_gw_2", align 8
  store i64 0, ptr %"%local_gw_3", align 8
  store i64 0, ptr %"%local_mac_0", align 8
  store i64 0, ptr %"%local_mac_1", align 8
  store i64 0, ptr %"%local_mac_2", align 8
  store i64 0, ptr %"%local_mac_3", align 8
  store i64 0, ptr %"%local_mac_4", align 8
  store i64 0, ptr %"%local_mac_5", align 8
  store i64 1, ptr %"%ip_packet_id", align 8
  store i64 0, ptr %"%ip_tx_count", align 8
  store i64 0, ptr %"%ip_rx_count", align 8
  store i64 0, ptr %"%ip_rx_errors", align 8
  store i64 0, ptr %"%ip_tx_errors", align 8
  store i64 0, ptr %"%ip_checksum_errors", align 8
  store i64 0, ptr %"%ip_ttl_expired", align 8
  store i64 0, ptr %"%ip_forwarded", align 8
  store i64 0, ptr %"%ip_dropped", align 8
  store i64 0, ptr %"%route_dest_0", align 8
  store i64 0, ptr %"%route_dest_1", align 8
  store i64 0, ptr %"%route_dest_2", align 8
  store i64 0, ptr %"%route_dest_3", align 8
  store i64 0, ptr %"%route_mask_0", align 8
  store i64 0, ptr %"%route_mask_1", align 8
  store i64 0, ptr %"%route_mask_2", align 8
  store i64 0, ptr %"%route_mask_3", align 8
  store i64 0, ptr %"%route_gw_0", align 8
  store i64 0, ptr %"%route_gw_1", align 8
  store i64 0, ptr %"%route_gw_2", align 8
  store i64 0, ptr %"%route_gw_3", align 8
  store i64 0, ptr %"%route_active_0", align 8
  store i64 0, ptr %"%route_active_1", align 8
  store i64 0, ptr %"%route_active_2", align 8
  store i64 0, ptr %"%route_active_3", align 8
  store i64 8, ptr %"%ICMP_ECHO_REQUEST", align 8
  store i64 0, ptr %"%ICMP_ECHO_REPLY", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
