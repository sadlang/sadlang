; ModuleID = 'شبكة_tcp_نحلة'
source_filename = "\D8\B4\D8\A8\D9\83\D8\A9_tcp_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@COM1 = constant i64 1016
@IP_PROTO_TCP = constant i64 6
@IP_PROTO_UDP = constant i64 17
@UDP_HEADER_SIZE = constant i64 8
@TCP_HEADER_SIZE = constant i64 20
@TCP_FIN = constant i64 1
@TCP_SYN = constant i64 2
@TCP_RST = constant i64 4
@TCP_PSH = constant i64 8
@TCP_ACK = constant i64 16
@TCP_URG = constant i64 32
@TCP_STATE_CLOSED = constant i64 0
@TCP_STATE_LISTEN = constant i64 1
@TCP_STATE_SYN_SENT = constant i64 2
@TCP_STATE_SYN_RCVD = constant i64 3
@TCP_STATE_ESTABLISHED = constant i64 4
@TCP_STATE_FIN_WAIT1 = constant i64 5
@TCP_STATE_FIN_WAIT2 = constant i64 6
@TCP_STATE_CLOSE_WAIT = constant i64 7
@TCP_STATE_LAST_ACK = constant i64 8
@TCP_STATE_TIME_WAIT = constant i64 9
@TCP_WINDOW_SIZE = constant i64 4096
@TCP_MAX_SOCKETS = constant i64 8
@TCP_RETRANSMIT_TIMEOUT = constant i64 3000
@TCP_MAX_RETRIES = constant i64 5
@UDP_MAX_SOCKETS = constant i64 8
@tcp_state_0 = global i64 0
@tcp_state_1 = global i64 0
@tcp_state_2 = global i64 0
@tcp_state_3 = global i64 0
@tcp_state_4 = global i64 0
@tcp_state_5 = global i64 0
@tcp_state_6 = global i64 0
@tcp_state_7 = global i64 0
@tcp_lport_0 = global i64 0
@tcp_lport_1 = global i64 0
@tcp_lport_2 = global i64 0
@tcp_lport_3 = global i64 0
@tcp_lport_4 = global i64 0
@tcp_lport_5 = global i64 0
@tcp_lport_6 = global i64 0
@tcp_lport_7 = global i64 0
@tcp_rport_0 = global i64 0
@tcp_rport_1 = global i64 0
@tcp_rport_2 = global i64 0
@tcp_rport_3 = global i64 0
@tcp_rport_4 = global i64 0
@tcp_rport_5 = global i64 0
@tcp_rport_6 = global i64 0
@tcp_rport_7 = global i64 0
@tcp_rip_0 = global i64 0
@tcp_rip_1 = global i64 0
@tcp_rip_2 = global i64 0
@tcp_rip_3 = global i64 0
@tcp_rip_4 = global i64 0
@tcp_rip_5 = global i64 0
@tcp_rip_6 = global i64 0
@tcp_rip_7 = global i64 0
@tcp_seq_0 = global i64 0
@tcp_seq_1 = global i64 0
@tcp_seq_2 = global i64 0
@tcp_seq_3 = global i64 0
@tcp_seq_4 = global i64 0
@tcp_seq_5 = global i64 0
@tcp_seq_6 = global i64 0
@tcp_seq_7 = global i64 0
@tcp_ack_0 = global i64 0
@tcp_ack_1 = global i64 0
@tcp_ack_2 = global i64 0
@tcp_ack_3 = global i64 0
@tcp_ack_4 = global i64 0
@tcp_ack_5 = global i64 0
@tcp_ack_6 = global i64 0
@tcp_ack_7 = global i64 0
@udp_active_0 = global i64 0
@udp_active_1 = global i64 0
@udp_active_2 = global i64 0
@udp_active_3 = global i64 0
@udp_active_4 = global i64 0
@udp_active_5 = global i64 0
@udp_active_6 = global i64 0
@udp_active_7 = global i64 0
@udp_lport_0 = global i64 0
@udp_lport_1 = global i64 0
@udp_lport_2 = global i64 0
@udp_lport_3 = global i64 0
@udp_lport_4 = global i64 0
@udp_lport_5 = global i64 0
@udp_lport_6 = global i64 0
@udp_lport_7 = global i64 0
@udp_rxbuf_0 = global i64 0
@udp_rxbuf_1 = global i64 0
@udp_rxbuf_2 = global i64 0
@udp_rxbuf_3 = global i64 0
@udp_rxbuf_4 = global i64 0
@udp_rxbuf_5 = global i64 0
@udp_rxbuf_6 = global i64 0
@udp_rxbuf_7 = global i64 0
@udp_rxlen_0 = global i64 0
@udp_rxlen_1 = global i64 0
@udp_rxlen_2 = global i64 0
@udp_rxlen_3 = global i64 0
@udp_rxlen_4 = global i64 0
@udp_rxlen_5 = global i64 0
@udp_rxlen_6 = global i64 0
@udp_rxlen_7 = global i64 0
@udp_sender_0 = global i64 0
@udp_sender_1 = global i64 0
@udp_sender_2 = global i64 0
@udp_sender_3 = global i64 0
@udp_sender_4 = global i64 0
@udp_sender_5 = global i64 0
@udp_sender_6 = global i64 0
@udp_sender_7 = global i64 0
@udp_sport_0 = global i64 0
@udp_sport_1 = global i64 0
@udp_sport_2 = global i64 0
@udp_sport_3 = global i64 0
@udp_sport_4 = global i64 0
@udp_sport_5 = global i64 0
@udp_sport_6 = global i64 0
@udp_sport_7 = global i64 0
@tcp_connections = global i64 0
@tcp_tx_count = global i64 0
@tcp_rx_count = global i64 0
@tcp_errors = global i64 0
@udp_tx_count = global i64 0
@udp_rx_count = global i64 0
@udp_errors = global i64 0
@auto_port = global i64 49152
@str.const = private unnamed_addr constant [32 x i8] c"[UDP] \D9\84\D8\A7 \D9\85\D8\A2\D8\AE\D8\B0 \D9\85\D8\AA\D8\A7\D8\AD\D8\A9!\00", align 1
@str.const.1 = private unnamed_addr constant [28 x i8] c"[UDP] \D9\85\D8\A3\D8\AE\D8\B0 \D9\85\D9\81\D8\AA\D9\88\D8\AD: \00", align 1
@str.const.2 = private unnamed_addr constant [11 x i8] c" \D9\85\D9\86\D9\81\D8\B0=\00", align 1
@str.const.3 = private unnamed_addr constant [26 x i8] c"[UDP] \D9\85\D8\A3\D8\AE\D8\B0 \D9\85\D8\BA\D9\84\D9\82: \00", align 1
@str.const.4 = private unnamed_addr constant [33 x i8] c"[UDP] \D9\85\D8\A3\D8\AE\D8\B0 \D8\BA\D9\8A\D8\B1 \D9\85\D9\81\D8\AA\D9\88\D8\AD\00", align 1
@str.const.5 = private unnamed_addr constant [26 x i8] c"[UDP] \D8\AD\D8\B2\D9\85\D8\A9 \D9\82\D8\B5\D9\8A\D8\B1\D8\A9\00", align 1
@str.const.6 = private unnamed_addr constant [19 x i8] c"[UDP] RX src_port=\00", align 1
@str.const.7 = private unnamed_addr constant [11 x i8] c" dst_port=\00", align 1
@str.const.8 = private unnamed_addr constant [6 x i8] c" len=\00", align 1
@str.const.9 = private unnamed_addr constant [35 x i8] c"[UDP] \D9\84\D8\A7 \D9\85\D8\A3\D8\AE\D8\B0 \D9\84\D9\84\D9\85\D9\86\D9\81\D8\B0: \00", align 1
@str.const.10 = private unnamed_addr constant [32 x i8] c"[TCP] \D9\84\D8\A7 \D9\85\D8\A2\D8\AE\D8\B0 \D9\85\D8\AA\D8\A7\D8\AD\D8\A9!\00", align 1
@str.const.11 = private unnamed_addr constant [22 x i8] c"[TCP] SYN sent, sock=\00", align 1
@str.const.12 = private unnamed_addr constant [7 x i8] c" port=\00", align 1
@str.const.13 = private unnamed_addr constant [25 x i8] c"[TCP] Listening on port \00", align 1
@str.const.14 = private unnamed_addr constant [23 x i8] c"[TCP] \D8\BA\D9\8A\D8\B1 \D9\85\D8\AA\D8\B5\D9\84!\00", align 1
@str.const.15 = private unnamed_addr constant [22 x i8] c"[TCP] FIN sent, sock=\00", align 1
@str.const.16 = private unnamed_addr constant [26 x i8] c"[TCP] \D8\AD\D8\B2\D9\85\D8\A9 \D9\82\D8\B5\D9\8A\D8\B1\D8\A9\00", align 1
@str.const.17 = private unnamed_addr constant [15 x i8] c"[TCP] RX port=\00", align 1
@str.const.18 = private unnamed_addr constant [8 x i8] c" flags=\00", align 1
@str.const.19 = private unnamed_addr constant [35 x i8] c"[TCP] \D9\84\D8\A7 \D9\85\D8\A3\D8\AE\D8\B0 \D9\84\D9\84\D9\85\D9\86\D9\81\D8\B0: \00", align 1
@str.const.20 = private unnamed_addr constant [19 x i8] c"[TCP] SYN-ACK sent\00", align 1
@str.const.21 = private unnamed_addr constant [33 x i8] c"[TCP] \D8\A7\D8\AA\D8\B5\D8\A7\D9\84 \D9\85\D8\A4\D8\B3\D8\B3! sock=\00", align 1
@str.const.22 = private unnamed_addr constant [44 x i8] c"[TCP] \D8\A7\D8\AA\D8\B5\D8\A7\D9\84 \D9\85\D8\A4\D8\B3\D8\B3 (\D8\AE\D8\A7\D8\AF\D9\85)! sock=\00", align 1
@str.const.23 = private unnamed_addr constant [26 x i8] c"[TCP] FIN received, sock=\00", align 1
@str.const.24 = private unnamed_addr constant [23 x i8] c"[TCP] Data ACKed, len=\00", align 1
@str.const.25 = private unnamed_addr constant [34 x i8] c"[TCP] \D8\A7\D8\AA\D8\B5\D8\A7\D9\84 \D9\85\D8\BA\D9\84\D9\82\D8\8C sock=\00", align 1
@str.const.26 = private unnamed_addr constant [52 x i8] c"\E2\95\90\E2\95\90\E2\95\90 [TCP/UDP] \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D9\86\D9\82\D9\84 \E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.27 = private unnamed_addr constant [23 x i8] c"  TCP \D8\A7\D8\AA\D8\B5\D8\A7\D9\84\D8\A7\D8\AA: \00", align 1
@str.const.28 = private unnamed_addr constant [19 x i8] c"  TCP \D9\85\D9\8F\D8\B1\D8\B3\D9\84: \00", align 1
@str.const.29 = private unnamed_addr constant [23 x i8] c"  TCP \D9\85\D9\8F\D8\B3\D8\AA\D9\82\D8\A8\D9\84: \00", align 1
@str.const.30 = private unnamed_addr constant [19 x i8] c"  TCP \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1: \00", align 1
@str.const.31 = private unnamed_addr constant [19 x i8] c"  UDP \D9\85\D9\8F\D8\B1\D8\B3\D9\84: \00", align 1
@str.const.32 = private unnamed_addr constant [23 x i8] c"  UDP \D9\85\D9\8F\D8\B3\D8\AA\D9\82\D8\A8\D9\84: \00", align 1
@str.const.33 = private unnamed_addr constant [19 x i8] c"  UDP \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1: \00", align 1
@str.const.34 = private unnamed_addr constant [106 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.35 = private unnamed_addr constant [7 x i8] c"  TCP[\00", align 1
@str.const.36 = private unnamed_addr constant [12 x i8] c"] \D8\AD\D8\A7\D9\84\D8\A9=\00", align 1
@str.const.38 = private unnamed_addr constant [44 x i8] c"[TCP/UDP] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D8\B7\D8\A8\D9\82\D8\A9 \D8\A7\D9\84\D9\86\D9\82\D9\84...\00", align 1
@str.const.39 = private unnamed_addr constant [19 x i8] c"[TCP/UDP] \D8\AC\D8\A7\D9\87\D8\B2\00", align 1

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

declare void @ip_send(i64, i64, i64, i64)

declare void @ip_send_broadcast(i64, i64, i64)

declare void @ip_to_int(i64, i64, i64, i64)

declare void @ip_byte(i64, i64)

declare void @ip_parse_src(i64)

define i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
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

define i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
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

define i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"() {
entry:
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 13)
  %"%COM1.load1" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load1", i64 10)
  ret i64 0
}

define i64 @tcp_get_state(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_24, label %merge_25

merge_25:                                         ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_26, label %merge_27

merge_27:                                         ; preds = %merge_25
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_28, label %merge_29

merge_29:                                         ; preds = %merge_27
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_30, label %merge_31

merge_31:                                         ; preds = %merge_29
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_32, label %merge_33

merge_33:                                         ; preds = %merge_31
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_34, label %merge_35

merge_35:                                         ; preds = %merge_33
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_36, label %merge_37

merge_37:                                         ; preds = %merge_35
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_38, label %merge_39

merge_39:                                         ; preds = %merge_37
  ret i64 0

then_24:                                          ; preds = %entry
  %loadtmp = load i64, ptr @tcp_state_0, align 8
  ret i64 %loadtmp

then_26:                                          ; preds = %merge_25
  %loadtmp4 = load i64, ptr @tcp_state_1, align 8
  ret i64 %loadtmp4

then_28:                                          ; preds = %merge_27
  %loadtmp7 = load i64, ptr @tcp_state_2, align 8
  ret i64 %loadtmp7

then_30:                                          ; preds = %merge_29
  %loadtmp10 = load i64, ptr @tcp_state_3, align 8
  ret i64 %loadtmp10

then_32:                                          ; preds = %merge_31
  %loadtmp13 = load i64, ptr @tcp_state_4, align 8
  ret i64 %loadtmp13

then_34:                                          ; preds = %merge_33
  %loadtmp16 = load i64, ptr @tcp_state_5, align 8
  ret i64 %loadtmp16

then_36:                                          ; preds = %merge_35
  %loadtmp19 = load i64, ptr @tcp_state_6, align 8
  ret i64 %loadtmp19

then_38:                                          ; preds = %merge_37
  %loadtmp22 = load i64, ptr @tcp_state_7, align 8
  ret i64 %loadtmp22
}

define i64 @tcp_set_state(i64 %idx, i64 %val) {
entry:
  %"%tcp_state_7" = alloca i64, align 8
  %"%tcp_state_6" = alloca i64, align 8
  %"%tcp_state_5" = alloca i64, align 8
  %"%tcp_state_4" = alloca i64, align 8
  %"%tcp_state_3" = alloca i64, align 8
  %"%tcp_state_2" = alloca i64, align 8
  %"%tcp_state_1" = alloca i64, align 8
  %"%tcp_state_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_40, label %merge_41

merge_41:                                         ; preds = %then_40, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_42, label %merge_43

merge_43:                                         ; preds = %then_42, %merge_41
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_44, label %merge_45

merge_45:                                         ; preds = %then_44, %merge_43
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_46, label %merge_47

merge_47:                                         ; preds = %then_46, %merge_45
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_48, label %merge_49

merge_49:                                         ; preds = %then_48, %merge_47
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_50, label %merge_51

merge_51:                                         ; preds = %then_50, %merge_49
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_52, label %merge_53

merge_53:                                         ; preds = %then_52, %merge_51
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_54, label %merge_55

merge_55:                                         ; preds = %then_54, %merge_53
  ret i64 0

then_40:                                          ; preds = %entry
  %"%129.load" = load i64, ptr %val2, align 8
  store i64 %"%129.load", ptr %"%tcp_state_0", align 8
  br label %merge_41

then_42:                                          ; preds = %merge_41
  %"%133.load" = load i64, ptr %val2, align 8
  store i64 %"%133.load", ptr %"%tcp_state_1", align 8
  br label %merge_43

then_44:                                          ; preds = %merge_43
  %"%137.load" = load i64, ptr %val2, align 8
  store i64 %"%137.load", ptr %"%tcp_state_2", align 8
  br label %merge_45

then_46:                                          ; preds = %merge_45
  %"%141.load" = load i64, ptr %val2, align 8
  store i64 %"%141.load", ptr %"%tcp_state_3", align 8
  br label %merge_47

then_48:                                          ; preds = %merge_47
  %"%145.load" = load i64, ptr %val2, align 8
  store i64 %"%145.load", ptr %"%tcp_state_4", align 8
  br label %merge_49

then_50:                                          ; preds = %merge_49
  %"%149.load" = load i64, ptr %val2, align 8
  store i64 %"%149.load", ptr %"%tcp_state_5", align 8
  br label %merge_51

then_52:                                          ; preds = %merge_51
  %"%153.load" = load i64, ptr %val2, align 8
  store i64 %"%153.load", ptr %"%tcp_state_6", align 8
  br label %merge_53

then_54:                                          ; preds = %merge_53
  %"%157.load" = load i64, ptr %val2, align 8
  store i64 %"%157.load", ptr %"%tcp_state_7", align 8
  br label %merge_55
}

define i64 @tcp_get_lport(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_56, label %merge_57

merge_57:                                         ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_58, label %merge_59

merge_59:                                         ; preds = %merge_57
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_60, label %merge_61

merge_61:                                         ; preds = %merge_59
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_62, label %merge_63

merge_63:                                         ; preds = %merge_61
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_64, label %merge_65

merge_65:                                         ; preds = %merge_63
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_66, label %merge_67

merge_67:                                         ; preds = %merge_65
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_68, label %merge_69

merge_69:                                         ; preds = %merge_67
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_70, label %merge_71

merge_71:                                         ; preds = %merge_69
  ret i64 0

then_56:                                          ; preds = %entry
  %loadtmp = load i64, ptr @tcp_lport_0, align 8
  ret i64 %loadtmp

then_58:                                          ; preds = %merge_57
  %loadtmp4 = load i64, ptr @tcp_lport_1, align 8
  ret i64 %loadtmp4

then_60:                                          ; preds = %merge_59
  %loadtmp7 = load i64, ptr @tcp_lport_2, align 8
  ret i64 %loadtmp7

then_62:                                          ; preds = %merge_61
  %loadtmp10 = load i64, ptr @tcp_lport_3, align 8
  ret i64 %loadtmp10

then_64:                                          ; preds = %merge_63
  %loadtmp13 = load i64, ptr @tcp_lport_4, align 8
  ret i64 %loadtmp13

then_66:                                          ; preds = %merge_65
  %loadtmp16 = load i64, ptr @tcp_lport_5, align 8
  ret i64 %loadtmp16

then_68:                                          ; preds = %merge_67
  %loadtmp19 = load i64, ptr @tcp_lport_6, align 8
  ret i64 %loadtmp19

then_70:                                          ; preds = %merge_69
  %loadtmp22 = load i64, ptr @tcp_lport_7, align 8
  ret i64 %loadtmp22
}

define i64 @tcp_set_lport(i64 %idx, i64 %val) {
entry:
  %"%tcp_lport_7" = alloca i64, align 8
  %"%tcp_lport_6" = alloca i64, align 8
  %"%tcp_lport_5" = alloca i64, align 8
  %"%tcp_lport_4" = alloca i64, align 8
  %"%tcp_lport_3" = alloca i64, align 8
  %"%tcp_lport_2" = alloca i64, align 8
  %"%tcp_lport_1" = alloca i64, align 8
  %"%tcp_lport_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_72, label %merge_73

merge_73:                                         ; preds = %then_72, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_74, label %merge_75

merge_75:                                         ; preds = %then_74, %merge_73
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_76, label %merge_77

merge_77:                                         ; preds = %then_76, %merge_75
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_78, label %merge_79

merge_79:                                         ; preds = %then_78, %merge_77
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_80, label %merge_81

merge_81:                                         ; preds = %then_80, %merge_79
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_82, label %merge_83

merge_83:                                         ; preds = %then_82, %merge_81
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_84, label %merge_85

merge_85:                                         ; preds = %then_84, %merge_83
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_86, label %merge_87

merge_87:                                         ; preds = %then_86, %merge_85
  ret i64 0

then_72:                                          ; preds = %entry
  %"%203.load" = load i64, ptr %val2, align 8
  store i64 %"%203.load", ptr %"%tcp_lport_0", align 8
  br label %merge_73

then_74:                                          ; preds = %merge_73
  %"%207.load" = load i64, ptr %val2, align 8
  store i64 %"%207.load", ptr %"%tcp_lport_1", align 8
  br label %merge_75

then_76:                                          ; preds = %merge_75
  %"%211.load" = load i64, ptr %val2, align 8
  store i64 %"%211.load", ptr %"%tcp_lport_2", align 8
  br label %merge_77

then_78:                                          ; preds = %merge_77
  %"%215.load" = load i64, ptr %val2, align 8
  store i64 %"%215.load", ptr %"%tcp_lport_3", align 8
  br label %merge_79

then_80:                                          ; preds = %merge_79
  %"%219.load" = load i64, ptr %val2, align 8
  store i64 %"%219.load", ptr %"%tcp_lport_4", align 8
  br label %merge_81

then_82:                                          ; preds = %merge_81
  %"%223.load" = load i64, ptr %val2, align 8
  store i64 %"%223.load", ptr %"%tcp_lport_5", align 8
  br label %merge_83

then_84:                                          ; preds = %merge_83
  %"%227.load" = load i64, ptr %val2, align 8
  store i64 %"%227.load", ptr %"%tcp_lport_6", align 8
  br label %merge_85

then_86:                                          ; preds = %merge_85
  %"%231.load" = load i64, ptr %val2, align 8
  store i64 %"%231.load", ptr %"%tcp_lport_7", align 8
  br label %merge_87
}

define i64 @tcp_get_rport(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_88, label %merge_89

merge_101:                                        ; preds = %merge_99
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_102, label %merge_103

merge_103:                                        ; preds = %merge_101
  ret i64 0

merge_89:                                         ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_90, label %merge_91

merge_91:                                         ; preds = %merge_89
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_92, label %merge_93

merge_93:                                         ; preds = %merge_91
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_94, label %merge_95

merge_95:                                         ; preds = %merge_93
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_96, label %merge_97

merge_97:                                         ; preds = %merge_95
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_98, label %merge_99

merge_99:                                         ; preds = %merge_97
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_100, label %merge_101

then_100:                                         ; preds = %merge_99
  %loadtmp19 = load i64, ptr @tcp_rport_6, align 8
  ret i64 %loadtmp19

then_102:                                         ; preds = %merge_101
  %loadtmp22 = load i64, ptr @tcp_rport_7, align 8
  ret i64 %loadtmp22

then_88:                                          ; preds = %entry
  %loadtmp = load i64, ptr @tcp_rport_0, align 8
  ret i64 %loadtmp

then_90:                                          ; preds = %merge_89
  %loadtmp4 = load i64, ptr @tcp_rport_1, align 8
  ret i64 %loadtmp4

then_92:                                          ; preds = %merge_91
  %loadtmp7 = load i64, ptr @tcp_rport_2, align 8
  ret i64 %loadtmp7

then_94:                                          ; preds = %merge_93
  %loadtmp10 = load i64, ptr @tcp_rport_3, align 8
  ret i64 %loadtmp10

then_96:                                          ; preds = %merge_95
  %loadtmp13 = load i64, ptr @tcp_rport_4, align 8
  ret i64 %loadtmp13

then_98:                                          ; preds = %merge_97
  %loadtmp16 = load i64, ptr @tcp_rport_5, align 8
  ret i64 %loadtmp16
}

define i64 @tcp_set_rport(i64 %idx, i64 %val) {
entry:
  %"%tcp_rport_7" = alloca i64, align 8
  %"%tcp_rport_6" = alloca i64, align 8
  %"%tcp_rport_5" = alloca i64, align 8
  %"%tcp_rport_4" = alloca i64, align 8
  %"%tcp_rport_3" = alloca i64, align 8
  %"%tcp_rport_2" = alloca i64, align 8
  %"%tcp_rport_1" = alloca i64, align 8
  %"%tcp_rport_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_104, label %merge_105

merge_105:                                        ; preds = %then_104, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_106, label %merge_107

merge_107:                                        ; preds = %then_106, %merge_105
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_108, label %merge_109

merge_109:                                        ; preds = %then_108, %merge_107
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_110, label %merge_111

merge_111:                                        ; preds = %then_110, %merge_109
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_112, label %merge_113

merge_113:                                        ; preds = %then_112, %merge_111
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_114, label %merge_115

merge_115:                                        ; preds = %then_114, %merge_113
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_116, label %merge_117

merge_117:                                        ; preds = %then_116, %merge_115
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_118, label %merge_119

merge_119:                                        ; preds = %then_118, %merge_117
  ret i64 0

then_104:                                         ; preds = %entry
  %"%277.load" = load i64, ptr %val2, align 8
  store i64 %"%277.load", ptr %"%tcp_rport_0", align 8
  br label %merge_105

then_106:                                         ; preds = %merge_105
  %"%281.load" = load i64, ptr %val2, align 8
  store i64 %"%281.load", ptr %"%tcp_rport_1", align 8
  br label %merge_107

then_108:                                         ; preds = %merge_107
  %"%285.load" = load i64, ptr %val2, align 8
  store i64 %"%285.load", ptr %"%tcp_rport_2", align 8
  br label %merge_109

then_110:                                         ; preds = %merge_109
  %"%289.load" = load i64, ptr %val2, align 8
  store i64 %"%289.load", ptr %"%tcp_rport_3", align 8
  br label %merge_111

then_112:                                         ; preds = %merge_111
  %"%293.load" = load i64, ptr %val2, align 8
  store i64 %"%293.load", ptr %"%tcp_rport_4", align 8
  br label %merge_113

then_114:                                         ; preds = %merge_113
  %"%297.load" = load i64, ptr %val2, align 8
  store i64 %"%297.load", ptr %"%tcp_rport_5", align 8
  br label %merge_115

then_116:                                         ; preds = %merge_115
  %"%301.load" = load i64, ptr %val2, align 8
  store i64 %"%301.load", ptr %"%tcp_rport_6", align 8
  br label %merge_117

then_118:                                         ; preds = %merge_117
  %"%305.load" = load i64, ptr %val2, align 8
  store i64 %"%305.load", ptr %"%tcp_rport_7", align 8
  br label %merge_119
}

define i64 @tcp_get_rip(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_120, label %merge_121

merge_121:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_122, label %merge_123

merge_123:                                        ; preds = %merge_121
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_124, label %merge_125

merge_125:                                        ; preds = %merge_123
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_126, label %merge_127

merge_127:                                        ; preds = %merge_125
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_128, label %merge_129

merge_129:                                        ; preds = %merge_127
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_130, label %merge_131

merge_131:                                        ; preds = %merge_129
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_132, label %merge_133

merge_133:                                        ; preds = %merge_131
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_134, label %merge_135

merge_135:                                        ; preds = %merge_133
  ret i64 0

then_120:                                         ; preds = %entry
  %loadtmp = load i64, ptr @tcp_rip_0, align 8
  ret i64 %loadtmp

then_122:                                         ; preds = %merge_121
  %loadtmp4 = load i64, ptr @tcp_rip_1, align 8
  ret i64 %loadtmp4

then_124:                                         ; preds = %merge_123
  %loadtmp7 = load i64, ptr @tcp_rip_2, align 8
  ret i64 %loadtmp7

then_126:                                         ; preds = %merge_125
  %loadtmp10 = load i64, ptr @tcp_rip_3, align 8
  ret i64 %loadtmp10

then_128:                                         ; preds = %merge_127
  %loadtmp13 = load i64, ptr @tcp_rip_4, align 8
  ret i64 %loadtmp13

then_130:                                         ; preds = %merge_129
  %loadtmp16 = load i64, ptr @tcp_rip_5, align 8
  ret i64 %loadtmp16

then_132:                                         ; preds = %merge_131
  %loadtmp19 = load i64, ptr @tcp_rip_6, align 8
  ret i64 %loadtmp19

then_134:                                         ; preds = %merge_133
  %loadtmp22 = load i64, ptr @tcp_rip_7, align 8
  ret i64 %loadtmp22
}

define i64 @tcp_set_rip(i64 %idx, i64 %val) {
entry:
  %"%tcp_rip_7" = alloca i64, align 8
  %"%tcp_rip_6" = alloca i64, align 8
  %"%tcp_rip_5" = alloca i64, align 8
  %"%tcp_rip_4" = alloca i64, align 8
  %"%tcp_rip_3" = alloca i64, align 8
  %"%tcp_rip_2" = alloca i64, align 8
  %"%tcp_rip_1" = alloca i64, align 8
  %"%tcp_rip_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_136, label %merge_137

merge_137:                                        ; preds = %then_136, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_138, label %merge_139

merge_139:                                        ; preds = %then_138, %merge_137
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_140, label %merge_141

merge_141:                                        ; preds = %then_140, %merge_139
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_142, label %merge_143

merge_143:                                        ; preds = %then_142, %merge_141
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_144, label %merge_145

merge_145:                                        ; preds = %then_144, %merge_143
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_146, label %merge_147

merge_147:                                        ; preds = %then_146, %merge_145
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_148, label %merge_149

merge_149:                                        ; preds = %then_148, %merge_147
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_150, label %merge_151

merge_151:                                        ; preds = %then_150, %merge_149
  ret i64 0

then_136:                                         ; preds = %entry
  %"%351.load" = load i64, ptr %val2, align 8
  store i64 %"%351.load", ptr %"%tcp_rip_0", align 8
  br label %merge_137

then_138:                                         ; preds = %merge_137
  %"%355.load" = load i64, ptr %val2, align 8
  store i64 %"%355.load", ptr %"%tcp_rip_1", align 8
  br label %merge_139

then_140:                                         ; preds = %merge_139
  %"%359.load" = load i64, ptr %val2, align 8
  store i64 %"%359.load", ptr %"%tcp_rip_2", align 8
  br label %merge_141

then_142:                                         ; preds = %merge_141
  %"%363.load" = load i64, ptr %val2, align 8
  store i64 %"%363.load", ptr %"%tcp_rip_3", align 8
  br label %merge_143

then_144:                                         ; preds = %merge_143
  %"%367.load" = load i64, ptr %val2, align 8
  store i64 %"%367.load", ptr %"%tcp_rip_4", align 8
  br label %merge_145

then_146:                                         ; preds = %merge_145
  %"%371.load" = load i64, ptr %val2, align 8
  store i64 %"%371.load", ptr %"%tcp_rip_5", align 8
  br label %merge_147

then_148:                                         ; preds = %merge_147
  %"%375.load" = load i64, ptr %val2, align 8
  store i64 %"%375.load", ptr %"%tcp_rip_6", align 8
  br label %merge_149

then_150:                                         ; preds = %merge_149
  %"%379.load" = load i64, ptr %val2, align 8
  store i64 %"%379.load", ptr %"%tcp_rip_7", align 8
  br label %merge_151
}

define i64 @tcp_get_seq(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_152, label %merge_153

merge_153:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_154, label %merge_155

merge_155:                                        ; preds = %merge_153
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_156, label %merge_157

merge_157:                                        ; preds = %merge_155
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_158, label %merge_159

merge_159:                                        ; preds = %merge_157
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_160, label %merge_161

merge_161:                                        ; preds = %merge_159
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_162, label %merge_163

merge_163:                                        ; preds = %merge_161
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_164, label %merge_165

merge_165:                                        ; preds = %merge_163
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_166, label %merge_167

merge_167:                                        ; preds = %merge_165
  ret i64 0

then_152:                                         ; preds = %entry
  %loadtmp = load i64, ptr @tcp_seq_0, align 8
  ret i64 %loadtmp

then_154:                                         ; preds = %merge_153
  %loadtmp4 = load i64, ptr @tcp_seq_1, align 8
  ret i64 %loadtmp4

then_156:                                         ; preds = %merge_155
  %loadtmp7 = load i64, ptr @tcp_seq_2, align 8
  ret i64 %loadtmp7

then_158:                                         ; preds = %merge_157
  %loadtmp10 = load i64, ptr @tcp_seq_3, align 8
  ret i64 %loadtmp10

then_160:                                         ; preds = %merge_159
  %loadtmp13 = load i64, ptr @tcp_seq_4, align 8
  ret i64 %loadtmp13

then_162:                                         ; preds = %merge_161
  %loadtmp16 = load i64, ptr @tcp_seq_5, align 8
  ret i64 %loadtmp16

then_164:                                         ; preds = %merge_163
  %loadtmp19 = load i64, ptr @tcp_seq_6, align 8
  ret i64 %loadtmp19

then_166:                                         ; preds = %merge_165
  %loadtmp22 = load i64, ptr @tcp_seq_7, align 8
  ret i64 %loadtmp22
}

define i64 @tcp_set_seq(i64 %idx, i64 %val) {
entry:
  %"%tcp_seq_7" = alloca i64, align 8
  %"%tcp_seq_6" = alloca i64, align 8
  %"%tcp_seq_5" = alloca i64, align 8
  %"%tcp_seq_4" = alloca i64, align 8
  %"%tcp_seq_3" = alloca i64, align 8
  %"%tcp_seq_2" = alloca i64, align 8
  %"%tcp_seq_1" = alloca i64, align 8
  %"%tcp_seq_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_168, label %merge_169

merge_169:                                        ; preds = %then_168, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_170, label %merge_171

merge_171:                                        ; preds = %then_170, %merge_169
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_172, label %merge_173

merge_173:                                        ; preds = %then_172, %merge_171
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_174, label %merge_175

merge_175:                                        ; preds = %then_174, %merge_173
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_176, label %merge_177

merge_177:                                        ; preds = %then_176, %merge_175
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_178, label %merge_179

merge_179:                                        ; preds = %then_178, %merge_177
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_180, label %merge_181

merge_181:                                        ; preds = %then_180, %merge_179
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_182, label %merge_183

merge_183:                                        ; preds = %then_182, %merge_181
  ret i64 0

then_168:                                         ; preds = %entry
  %"%425.load" = load i64, ptr %val2, align 8
  store i64 %"%425.load", ptr %"%tcp_seq_0", align 8
  br label %merge_169

then_170:                                         ; preds = %merge_169
  %"%429.load" = load i64, ptr %val2, align 8
  store i64 %"%429.load", ptr %"%tcp_seq_1", align 8
  br label %merge_171

then_172:                                         ; preds = %merge_171
  %"%433.load" = load i64, ptr %val2, align 8
  store i64 %"%433.load", ptr %"%tcp_seq_2", align 8
  br label %merge_173

then_174:                                         ; preds = %merge_173
  %"%437.load" = load i64, ptr %val2, align 8
  store i64 %"%437.load", ptr %"%tcp_seq_3", align 8
  br label %merge_175

then_176:                                         ; preds = %merge_175
  %"%441.load" = load i64, ptr %val2, align 8
  store i64 %"%441.load", ptr %"%tcp_seq_4", align 8
  br label %merge_177

then_178:                                         ; preds = %merge_177
  %"%445.load" = load i64, ptr %val2, align 8
  store i64 %"%445.load", ptr %"%tcp_seq_5", align 8
  br label %merge_179

then_180:                                         ; preds = %merge_179
  %"%449.load" = load i64, ptr %val2, align 8
  store i64 %"%449.load", ptr %"%tcp_seq_6", align 8
  br label %merge_181

then_182:                                         ; preds = %merge_181
  %"%453.load" = load i64, ptr %val2, align 8
  store i64 %"%453.load", ptr %"%tcp_seq_7", align 8
  br label %merge_183
}

define i64 @tcp_get_ack(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_184, label %merge_185

merge_185:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_186, label %merge_187

merge_187:                                        ; preds = %merge_185
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_188, label %merge_189

merge_189:                                        ; preds = %merge_187
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_190, label %merge_191

merge_191:                                        ; preds = %merge_189
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_192, label %merge_193

merge_193:                                        ; preds = %merge_191
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_194, label %merge_195

merge_195:                                        ; preds = %merge_193
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_196, label %merge_197

merge_197:                                        ; preds = %merge_195
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_198, label %merge_199

merge_199:                                        ; preds = %merge_197
  ret i64 0

then_184:                                         ; preds = %entry
  %loadtmp = load i64, ptr @tcp_ack_0, align 8
  ret i64 %loadtmp

then_186:                                         ; preds = %merge_185
  %loadtmp4 = load i64, ptr @tcp_ack_1, align 8
  ret i64 %loadtmp4

then_188:                                         ; preds = %merge_187
  %loadtmp7 = load i64, ptr @tcp_ack_2, align 8
  ret i64 %loadtmp7

then_190:                                         ; preds = %merge_189
  %loadtmp10 = load i64, ptr @tcp_ack_3, align 8
  ret i64 %loadtmp10

then_192:                                         ; preds = %merge_191
  %loadtmp13 = load i64, ptr @tcp_ack_4, align 8
  ret i64 %loadtmp13

then_194:                                         ; preds = %merge_193
  %loadtmp16 = load i64, ptr @tcp_ack_5, align 8
  ret i64 %loadtmp16

then_196:                                         ; preds = %merge_195
  %loadtmp19 = load i64, ptr @tcp_ack_6, align 8
  ret i64 %loadtmp19

then_198:                                         ; preds = %merge_197
  %loadtmp22 = load i64, ptr @tcp_ack_7, align 8
  ret i64 %loadtmp22
}

define i64 @tcp_set_ack(i64 %idx, i64 %val) {
entry:
  %"%tcp_ack_7" = alloca i64, align 8
  %"%tcp_ack_6" = alloca i64, align 8
  %"%tcp_ack_5" = alloca i64, align 8
  %"%tcp_ack_4" = alloca i64, align 8
  %"%tcp_ack_3" = alloca i64, align 8
  %"%tcp_ack_2" = alloca i64, align 8
  %"%tcp_ack_1" = alloca i64, align 8
  %"%tcp_ack_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_200, label %merge_201

merge_201:                                        ; preds = %then_200, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_202, label %merge_203

merge_203:                                        ; preds = %then_202, %merge_201
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_204, label %merge_205

merge_205:                                        ; preds = %then_204, %merge_203
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_206, label %merge_207

merge_207:                                        ; preds = %then_206, %merge_205
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_208, label %merge_209

merge_209:                                        ; preds = %then_208, %merge_207
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_210, label %merge_211

merge_211:                                        ; preds = %then_210, %merge_209
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_212, label %merge_213

merge_213:                                        ; preds = %then_212, %merge_211
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_214, label %merge_215

merge_215:                                        ; preds = %then_214, %merge_213
  ret i64 0

then_200:                                         ; preds = %entry
  %"%499.load" = load i64, ptr %val2, align 8
  store i64 %"%499.load", ptr %"%tcp_ack_0", align 8
  br label %merge_201

then_202:                                         ; preds = %merge_201
  %"%503.load" = load i64, ptr %val2, align 8
  store i64 %"%503.load", ptr %"%tcp_ack_1", align 8
  br label %merge_203

then_204:                                         ; preds = %merge_203
  %"%507.load" = load i64, ptr %val2, align 8
  store i64 %"%507.load", ptr %"%tcp_ack_2", align 8
  br label %merge_205

then_206:                                         ; preds = %merge_205
  %"%511.load" = load i64, ptr %val2, align 8
  store i64 %"%511.load", ptr %"%tcp_ack_3", align 8
  br label %merge_207

then_208:                                         ; preds = %merge_207
  %"%515.load" = load i64, ptr %val2, align 8
  store i64 %"%515.load", ptr %"%tcp_ack_4", align 8
  br label %merge_209

then_210:                                         ; preds = %merge_209
  %"%519.load" = load i64, ptr %val2, align 8
  store i64 %"%519.load", ptr %"%tcp_ack_5", align 8
  br label %merge_211

then_212:                                         ; preds = %merge_211
  %"%523.load" = load i64, ptr %val2, align 8
  store i64 %"%523.load", ptr %"%tcp_ack_6", align 8
  br label %merge_213

then_214:                                         ; preds = %merge_213
  %"%527.load" = load i64, ptr %val2, align 8
  store i64 %"%527.load", ptr %"%tcp_ack_7", align 8
  br label %merge_215
}

define i64 @udp_get_active(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_216, label %merge_217

merge_217:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_218, label %merge_219

merge_219:                                        ; preds = %merge_217
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_220, label %merge_221

merge_221:                                        ; preds = %merge_219
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_222, label %merge_223

merge_223:                                        ; preds = %merge_221
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_224, label %merge_225

merge_225:                                        ; preds = %merge_223
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_226, label %merge_227

merge_227:                                        ; preds = %merge_225
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_228, label %merge_229

merge_229:                                        ; preds = %merge_227
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_230, label %merge_231

merge_231:                                        ; preds = %merge_229
  ret i64 0

then_216:                                         ; preds = %entry
  %loadtmp = load i64, ptr @udp_active_0, align 8
  ret i64 %loadtmp

then_218:                                         ; preds = %merge_217
  %loadtmp4 = load i64, ptr @udp_active_1, align 8
  ret i64 %loadtmp4

then_220:                                         ; preds = %merge_219
  %loadtmp7 = load i64, ptr @udp_active_2, align 8
  ret i64 %loadtmp7

then_222:                                         ; preds = %merge_221
  %loadtmp10 = load i64, ptr @udp_active_3, align 8
  ret i64 %loadtmp10

then_224:                                         ; preds = %merge_223
  %loadtmp13 = load i64, ptr @udp_active_4, align 8
  ret i64 %loadtmp13

then_226:                                         ; preds = %merge_225
  %loadtmp16 = load i64, ptr @udp_active_5, align 8
  ret i64 %loadtmp16

then_228:                                         ; preds = %merge_227
  %loadtmp19 = load i64, ptr @udp_active_6, align 8
  ret i64 %loadtmp19

then_230:                                         ; preds = %merge_229
  %loadtmp22 = load i64, ptr @udp_active_7, align 8
  ret i64 %loadtmp22
}

define i64 @udp_set_active(i64 %idx, i64 %val) {
entry:
  %"%udp_active_7" = alloca i64, align 8
  %"%udp_active_6" = alloca i64, align 8
  %"%udp_active_5" = alloca i64, align 8
  %"%udp_active_4" = alloca i64, align 8
  %"%udp_active_3" = alloca i64, align 8
  %"%udp_active_2" = alloca i64, align 8
  %"%udp_active_1" = alloca i64, align 8
  %"%udp_active_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_232, label %merge_233

merge_233:                                        ; preds = %then_232, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_234, label %merge_235

merge_235:                                        ; preds = %then_234, %merge_233
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_236, label %merge_237

merge_237:                                        ; preds = %then_236, %merge_235
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_238, label %merge_239

merge_239:                                        ; preds = %then_238, %merge_237
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_240, label %merge_241

merge_241:                                        ; preds = %then_240, %merge_239
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_242, label %merge_243

merge_243:                                        ; preds = %then_242, %merge_241
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_244, label %merge_245

merge_245:                                        ; preds = %then_244, %merge_243
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_246, label %merge_247

merge_247:                                        ; preds = %then_246, %merge_245
  ret i64 0

then_232:                                         ; preds = %entry
  %"%573.load" = load i64, ptr %val2, align 8
  store i64 %"%573.load", ptr %"%udp_active_0", align 8
  br label %merge_233

then_234:                                         ; preds = %merge_233
  %"%577.load" = load i64, ptr %val2, align 8
  store i64 %"%577.load", ptr %"%udp_active_1", align 8
  br label %merge_235

then_236:                                         ; preds = %merge_235
  %"%581.load" = load i64, ptr %val2, align 8
  store i64 %"%581.load", ptr %"%udp_active_2", align 8
  br label %merge_237

then_238:                                         ; preds = %merge_237
  %"%585.load" = load i64, ptr %val2, align 8
  store i64 %"%585.load", ptr %"%udp_active_3", align 8
  br label %merge_239

then_240:                                         ; preds = %merge_239
  %"%589.load" = load i64, ptr %val2, align 8
  store i64 %"%589.load", ptr %"%udp_active_4", align 8
  br label %merge_241

then_242:                                         ; preds = %merge_241
  %"%593.load" = load i64, ptr %val2, align 8
  store i64 %"%593.load", ptr %"%udp_active_5", align 8
  br label %merge_243

then_244:                                         ; preds = %merge_243
  %"%597.load" = load i64, ptr %val2, align 8
  store i64 %"%597.load", ptr %"%udp_active_6", align 8
  br label %merge_245

then_246:                                         ; preds = %merge_245
  %"%601.load" = load i64, ptr %val2, align 8
  store i64 %"%601.load", ptr %"%udp_active_7", align 8
  br label %merge_247
}

define i64 @udp_get_lport(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_248, label %merge_249

merge_249:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_250, label %merge_251

merge_251:                                        ; preds = %merge_249
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_252, label %merge_253

merge_253:                                        ; preds = %merge_251
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_254, label %merge_255

merge_255:                                        ; preds = %merge_253
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_256, label %merge_257

merge_257:                                        ; preds = %merge_255
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_258, label %merge_259

merge_259:                                        ; preds = %merge_257
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_260, label %merge_261

merge_261:                                        ; preds = %merge_259
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_262, label %merge_263

merge_263:                                        ; preds = %merge_261
  ret i64 0

then_248:                                         ; preds = %entry
  %loadtmp = load i64, ptr @udp_lport_0, align 8
  ret i64 %loadtmp

then_250:                                         ; preds = %merge_249
  %loadtmp4 = load i64, ptr @udp_lport_1, align 8
  ret i64 %loadtmp4

then_252:                                         ; preds = %merge_251
  %loadtmp7 = load i64, ptr @udp_lport_2, align 8
  ret i64 %loadtmp7

then_254:                                         ; preds = %merge_253
  %loadtmp10 = load i64, ptr @udp_lport_3, align 8
  ret i64 %loadtmp10

then_256:                                         ; preds = %merge_255
  %loadtmp13 = load i64, ptr @udp_lport_4, align 8
  ret i64 %loadtmp13

then_258:                                         ; preds = %merge_257
  %loadtmp16 = load i64, ptr @udp_lport_5, align 8
  ret i64 %loadtmp16

then_260:                                         ; preds = %merge_259
  %loadtmp19 = load i64, ptr @udp_lport_6, align 8
  ret i64 %loadtmp19

then_262:                                         ; preds = %merge_261
  %loadtmp22 = load i64, ptr @udp_lport_7, align 8
  ret i64 %loadtmp22
}

define i64 @udp_set_lport(i64 %idx, i64 %val) {
entry:
  %"%udp_lport_7" = alloca i64, align 8
  %"%udp_lport_6" = alloca i64, align 8
  %"%udp_lport_5" = alloca i64, align 8
  %"%udp_lport_4" = alloca i64, align 8
  %"%udp_lport_3" = alloca i64, align 8
  %"%udp_lport_2" = alloca i64, align 8
  %"%udp_lport_1" = alloca i64, align 8
  %"%udp_lport_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_264, label %merge_265

merge_265:                                        ; preds = %then_264, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_266, label %merge_267

merge_267:                                        ; preds = %then_266, %merge_265
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_268, label %merge_269

merge_269:                                        ; preds = %then_268, %merge_267
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_270, label %merge_271

merge_271:                                        ; preds = %then_270, %merge_269
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_272, label %merge_273

merge_273:                                        ; preds = %then_272, %merge_271
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_274, label %merge_275

merge_275:                                        ; preds = %then_274, %merge_273
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_276, label %merge_277

merge_277:                                        ; preds = %then_276, %merge_275
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_278, label %merge_279

merge_279:                                        ; preds = %then_278, %merge_277
  ret i64 0

then_264:                                         ; preds = %entry
  %"%647.load" = load i64, ptr %val2, align 8
  store i64 %"%647.load", ptr %"%udp_lport_0", align 8
  br label %merge_265

then_266:                                         ; preds = %merge_265
  %"%651.load" = load i64, ptr %val2, align 8
  store i64 %"%651.load", ptr %"%udp_lport_1", align 8
  br label %merge_267

then_268:                                         ; preds = %merge_267
  %"%655.load" = load i64, ptr %val2, align 8
  store i64 %"%655.load", ptr %"%udp_lport_2", align 8
  br label %merge_269

then_270:                                         ; preds = %merge_269
  %"%659.load" = load i64, ptr %val2, align 8
  store i64 %"%659.load", ptr %"%udp_lport_3", align 8
  br label %merge_271

then_272:                                         ; preds = %merge_271
  %"%663.load" = load i64, ptr %val2, align 8
  store i64 %"%663.load", ptr %"%udp_lport_4", align 8
  br label %merge_273

then_274:                                         ; preds = %merge_273
  %"%667.load" = load i64, ptr %val2, align 8
  store i64 %"%667.load", ptr %"%udp_lport_5", align 8
  br label %merge_275

then_276:                                         ; preds = %merge_275
  %"%671.load" = load i64, ptr %val2, align 8
  store i64 %"%671.load", ptr %"%udp_lport_6", align 8
  br label %merge_277

then_278:                                         ; preds = %merge_277
  %"%675.load" = load i64, ptr %val2, align 8
  store i64 %"%675.load", ptr %"%udp_lport_7", align 8
  br label %merge_279
}

define i64 @udp_get_rxbuf(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_280, label %merge_281

merge_281:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_282, label %merge_283

merge_283:                                        ; preds = %merge_281
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_284, label %merge_285

merge_285:                                        ; preds = %merge_283
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_286, label %merge_287

merge_287:                                        ; preds = %merge_285
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_288, label %merge_289

merge_289:                                        ; preds = %merge_287
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_290, label %merge_291

merge_291:                                        ; preds = %merge_289
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_292, label %merge_293

merge_293:                                        ; preds = %merge_291
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_294, label %merge_295

merge_295:                                        ; preds = %merge_293
  ret i64 0

then_280:                                         ; preds = %entry
  %loadtmp = load i64, ptr @udp_rxbuf_0, align 8
  ret i64 %loadtmp

then_282:                                         ; preds = %merge_281
  %loadtmp4 = load i64, ptr @udp_rxbuf_1, align 8
  ret i64 %loadtmp4

then_284:                                         ; preds = %merge_283
  %loadtmp7 = load i64, ptr @udp_rxbuf_2, align 8
  ret i64 %loadtmp7

then_286:                                         ; preds = %merge_285
  %loadtmp10 = load i64, ptr @udp_rxbuf_3, align 8
  ret i64 %loadtmp10

then_288:                                         ; preds = %merge_287
  %loadtmp13 = load i64, ptr @udp_rxbuf_4, align 8
  ret i64 %loadtmp13

then_290:                                         ; preds = %merge_289
  %loadtmp16 = load i64, ptr @udp_rxbuf_5, align 8
  ret i64 %loadtmp16

then_292:                                         ; preds = %merge_291
  %loadtmp19 = load i64, ptr @udp_rxbuf_6, align 8
  ret i64 %loadtmp19

then_294:                                         ; preds = %merge_293
  %loadtmp22 = load i64, ptr @udp_rxbuf_7, align 8
  ret i64 %loadtmp22
}

define i64 @udp_set_rxbuf(i64 %idx, i64 %val) {
entry:
  %"%udp_rxbuf_7" = alloca i64, align 8
  %"%udp_rxbuf_6" = alloca i64, align 8
  %"%udp_rxbuf_5" = alloca i64, align 8
  %"%udp_rxbuf_4" = alloca i64, align 8
  %"%udp_rxbuf_3" = alloca i64, align 8
  %"%udp_rxbuf_2" = alloca i64, align 8
  %"%udp_rxbuf_1" = alloca i64, align 8
  %"%udp_rxbuf_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_296, label %merge_297

merge_297:                                        ; preds = %then_296, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_298, label %merge_299

merge_299:                                        ; preds = %then_298, %merge_297
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_300, label %merge_301

merge_301:                                        ; preds = %then_300, %merge_299
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_302, label %merge_303

merge_303:                                        ; preds = %then_302, %merge_301
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_304, label %merge_305

merge_305:                                        ; preds = %then_304, %merge_303
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_306, label %merge_307

merge_307:                                        ; preds = %then_306, %merge_305
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_308, label %merge_309

merge_309:                                        ; preds = %then_308, %merge_307
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_310, label %merge_311

merge_311:                                        ; preds = %then_310, %merge_309
  ret i64 0

then_296:                                         ; preds = %entry
  %"%721.load" = load i64, ptr %val2, align 8
  store i64 %"%721.load", ptr %"%udp_rxbuf_0", align 8
  br label %merge_297

then_298:                                         ; preds = %merge_297
  %"%725.load" = load i64, ptr %val2, align 8
  store i64 %"%725.load", ptr %"%udp_rxbuf_1", align 8
  br label %merge_299

then_300:                                         ; preds = %merge_299
  %"%729.load" = load i64, ptr %val2, align 8
  store i64 %"%729.load", ptr %"%udp_rxbuf_2", align 8
  br label %merge_301

then_302:                                         ; preds = %merge_301
  %"%733.load" = load i64, ptr %val2, align 8
  store i64 %"%733.load", ptr %"%udp_rxbuf_3", align 8
  br label %merge_303

then_304:                                         ; preds = %merge_303
  %"%737.load" = load i64, ptr %val2, align 8
  store i64 %"%737.load", ptr %"%udp_rxbuf_4", align 8
  br label %merge_305

then_306:                                         ; preds = %merge_305
  %"%741.load" = load i64, ptr %val2, align 8
  store i64 %"%741.load", ptr %"%udp_rxbuf_5", align 8
  br label %merge_307

then_308:                                         ; preds = %merge_307
  %"%745.load" = load i64, ptr %val2, align 8
  store i64 %"%745.load", ptr %"%udp_rxbuf_6", align 8
  br label %merge_309

then_310:                                         ; preds = %merge_309
  %"%749.load" = load i64, ptr %val2, align 8
  store i64 %"%749.load", ptr %"%udp_rxbuf_7", align 8
  br label %merge_311
}

define i64 @udp_get_rxlen(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_312, label %merge_313

merge_313:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_314, label %merge_315

merge_315:                                        ; preds = %merge_313
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_316, label %merge_317

merge_317:                                        ; preds = %merge_315
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_318, label %merge_319

merge_319:                                        ; preds = %merge_317
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_320, label %merge_321

merge_321:                                        ; preds = %merge_319
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_322, label %merge_323

merge_323:                                        ; preds = %merge_321
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_324, label %merge_325

merge_325:                                        ; preds = %merge_323
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_326, label %merge_327

merge_327:                                        ; preds = %merge_325
  ret i64 0

then_312:                                         ; preds = %entry
  %loadtmp = load i64, ptr @udp_rxlen_0, align 8
  ret i64 %loadtmp

then_314:                                         ; preds = %merge_313
  %loadtmp4 = load i64, ptr @udp_rxlen_1, align 8
  ret i64 %loadtmp4

then_316:                                         ; preds = %merge_315
  %loadtmp7 = load i64, ptr @udp_rxlen_2, align 8
  ret i64 %loadtmp7

then_318:                                         ; preds = %merge_317
  %loadtmp10 = load i64, ptr @udp_rxlen_3, align 8
  ret i64 %loadtmp10

then_320:                                         ; preds = %merge_319
  %loadtmp13 = load i64, ptr @udp_rxlen_4, align 8
  ret i64 %loadtmp13

then_322:                                         ; preds = %merge_321
  %loadtmp16 = load i64, ptr @udp_rxlen_5, align 8
  ret i64 %loadtmp16

then_324:                                         ; preds = %merge_323
  %loadtmp19 = load i64, ptr @udp_rxlen_6, align 8
  ret i64 %loadtmp19

then_326:                                         ; preds = %merge_325
  %loadtmp22 = load i64, ptr @udp_rxlen_7, align 8
  ret i64 %loadtmp22
}

define i64 @udp_set_rxlen(i64 %idx, i64 %val) {
entry:
  %"%udp_rxlen_7" = alloca i64, align 8
  %"%udp_rxlen_6" = alloca i64, align 8
  %"%udp_rxlen_5" = alloca i64, align 8
  %"%udp_rxlen_4" = alloca i64, align 8
  %"%udp_rxlen_3" = alloca i64, align 8
  %"%udp_rxlen_2" = alloca i64, align 8
  %"%udp_rxlen_1" = alloca i64, align 8
  %"%udp_rxlen_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_328, label %merge_329

merge_329:                                        ; preds = %then_328, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_330, label %merge_331

merge_331:                                        ; preds = %then_330, %merge_329
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_332, label %merge_333

merge_333:                                        ; preds = %then_332, %merge_331
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_334, label %merge_335

merge_335:                                        ; preds = %then_334, %merge_333
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_336, label %merge_337

merge_337:                                        ; preds = %then_336, %merge_335
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_338, label %merge_339

merge_339:                                        ; preds = %then_338, %merge_337
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_340, label %merge_341

merge_341:                                        ; preds = %then_340, %merge_339
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_342, label %merge_343

merge_343:                                        ; preds = %then_342, %merge_341
  ret i64 0

then_328:                                         ; preds = %entry
  %"%795.load" = load i64, ptr %val2, align 8
  store i64 %"%795.load", ptr %"%udp_rxlen_0", align 8
  br label %merge_329

then_330:                                         ; preds = %merge_329
  %"%799.load" = load i64, ptr %val2, align 8
  store i64 %"%799.load", ptr %"%udp_rxlen_1", align 8
  br label %merge_331

then_332:                                         ; preds = %merge_331
  %"%803.load" = load i64, ptr %val2, align 8
  store i64 %"%803.load", ptr %"%udp_rxlen_2", align 8
  br label %merge_333

then_334:                                         ; preds = %merge_333
  %"%807.load" = load i64, ptr %val2, align 8
  store i64 %"%807.load", ptr %"%udp_rxlen_3", align 8
  br label %merge_335

then_336:                                         ; preds = %merge_335
  %"%811.load" = load i64, ptr %val2, align 8
  store i64 %"%811.load", ptr %"%udp_rxlen_4", align 8
  br label %merge_337

then_338:                                         ; preds = %merge_337
  %"%815.load" = load i64, ptr %val2, align 8
  store i64 %"%815.load", ptr %"%udp_rxlen_5", align 8
  br label %merge_339

then_340:                                         ; preds = %merge_339
  %"%819.load" = load i64, ptr %val2, align 8
  store i64 %"%819.load", ptr %"%udp_rxlen_6", align 8
  br label %merge_341

then_342:                                         ; preds = %merge_341
  %"%823.load" = load i64, ptr %val2, align 8
  store i64 %"%823.load", ptr %"%udp_rxlen_7", align 8
  br label %merge_343
}

define i64 @udp_get_sender(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_344, label %merge_345

merge_345:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_346, label %merge_347

merge_347:                                        ; preds = %merge_345
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_348, label %merge_349

merge_349:                                        ; preds = %merge_347
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_350, label %merge_351

merge_351:                                        ; preds = %merge_349
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_352, label %merge_353

merge_353:                                        ; preds = %merge_351
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_354, label %merge_355

merge_355:                                        ; preds = %merge_353
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_356, label %merge_357

merge_357:                                        ; preds = %merge_355
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_358, label %merge_359

merge_359:                                        ; preds = %merge_357
  ret i64 0

then_344:                                         ; preds = %entry
  %loadtmp = load i64, ptr @udp_sender_0, align 8
  ret i64 %loadtmp

then_346:                                         ; preds = %merge_345
  %loadtmp4 = load i64, ptr @udp_sender_1, align 8
  ret i64 %loadtmp4

then_348:                                         ; preds = %merge_347
  %loadtmp7 = load i64, ptr @udp_sender_2, align 8
  ret i64 %loadtmp7

then_350:                                         ; preds = %merge_349
  %loadtmp10 = load i64, ptr @udp_sender_3, align 8
  ret i64 %loadtmp10

then_352:                                         ; preds = %merge_351
  %loadtmp13 = load i64, ptr @udp_sender_4, align 8
  ret i64 %loadtmp13

then_354:                                         ; preds = %merge_353
  %loadtmp16 = load i64, ptr @udp_sender_5, align 8
  ret i64 %loadtmp16

then_356:                                         ; preds = %merge_355
  %loadtmp19 = load i64, ptr @udp_sender_6, align 8
  ret i64 %loadtmp19

then_358:                                         ; preds = %merge_357
  %loadtmp22 = load i64, ptr @udp_sender_7, align 8
  ret i64 %loadtmp22
}

define i64 @udp_set_sender(i64 %idx, i64 %val) {
entry:
  %"%udp_sender_7" = alloca i64, align 8
  %"%udp_sender_6" = alloca i64, align 8
  %"%udp_sender_5" = alloca i64, align 8
  %"%udp_sender_4" = alloca i64, align 8
  %"%udp_sender_3" = alloca i64, align 8
  %"%udp_sender_2" = alloca i64, align 8
  %"%udp_sender_1" = alloca i64, align 8
  %"%udp_sender_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_360, label %merge_361

merge_361:                                        ; preds = %then_360, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_362, label %merge_363

merge_363:                                        ; preds = %then_362, %merge_361
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_364, label %merge_365

merge_365:                                        ; preds = %then_364, %merge_363
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_366, label %merge_367

merge_367:                                        ; preds = %then_366, %merge_365
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_368, label %merge_369

merge_369:                                        ; preds = %then_368, %merge_367
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_370, label %merge_371

merge_371:                                        ; preds = %then_370, %merge_369
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_372, label %merge_373

merge_373:                                        ; preds = %then_372, %merge_371
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_374, label %merge_375

merge_375:                                        ; preds = %then_374, %merge_373
  ret i64 0

then_360:                                         ; preds = %entry
  %"%869.load" = load i64, ptr %val2, align 8
  store i64 %"%869.load", ptr %"%udp_sender_0", align 8
  br label %merge_361

then_362:                                         ; preds = %merge_361
  %"%873.load" = load i64, ptr %val2, align 8
  store i64 %"%873.load", ptr %"%udp_sender_1", align 8
  br label %merge_363

then_364:                                         ; preds = %merge_363
  %"%877.load" = load i64, ptr %val2, align 8
  store i64 %"%877.load", ptr %"%udp_sender_2", align 8
  br label %merge_365

then_366:                                         ; preds = %merge_365
  %"%881.load" = load i64, ptr %val2, align 8
  store i64 %"%881.load", ptr %"%udp_sender_3", align 8
  br label %merge_367

then_368:                                         ; preds = %merge_367
  %"%885.load" = load i64, ptr %val2, align 8
  store i64 %"%885.load", ptr %"%udp_sender_4", align 8
  br label %merge_369

then_370:                                         ; preds = %merge_369
  %"%889.load" = load i64, ptr %val2, align 8
  store i64 %"%889.load", ptr %"%udp_sender_5", align 8
  br label %merge_371

then_372:                                         ; preds = %merge_371
  %"%893.load" = load i64, ptr %val2, align 8
  store i64 %"%893.load", ptr %"%udp_sender_6", align 8
  br label %merge_373

then_374:                                         ; preds = %merge_373
  %"%897.load" = load i64, ptr %val2, align 8
  store i64 %"%897.load", ptr %"%udp_sender_7", align 8
  br label %merge_375
}

define i64 @udp_get_sport(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_376, label %merge_377

merge_377:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_378, label %merge_379

merge_379:                                        ; preds = %merge_377
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_380, label %merge_381

merge_381:                                        ; preds = %merge_379
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_382, label %merge_383

merge_383:                                        ; preds = %merge_381
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_384, label %merge_385

merge_385:                                        ; preds = %merge_383
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_386, label %merge_387

merge_387:                                        ; preds = %merge_385
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_388, label %merge_389

merge_389:                                        ; preds = %merge_387
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_390, label %merge_391

merge_391:                                        ; preds = %merge_389
  ret i64 0

then_376:                                         ; preds = %entry
  %loadtmp = load i64, ptr @udp_sport_0, align 8
  ret i64 %loadtmp

then_378:                                         ; preds = %merge_377
  %loadtmp4 = load i64, ptr @udp_sport_1, align 8
  ret i64 %loadtmp4

then_380:                                         ; preds = %merge_379
  %loadtmp7 = load i64, ptr @udp_sport_2, align 8
  ret i64 %loadtmp7

then_382:                                         ; preds = %merge_381
  %loadtmp10 = load i64, ptr @udp_sport_3, align 8
  ret i64 %loadtmp10

then_384:                                         ; preds = %merge_383
  %loadtmp13 = load i64, ptr @udp_sport_4, align 8
  ret i64 %loadtmp13

then_386:                                         ; preds = %merge_385
  %loadtmp16 = load i64, ptr @udp_sport_5, align 8
  ret i64 %loadtmp16

then_388:                                         ; preds = %merge_387
  %loadtmp19 = load i64, ptr @udp_sport_6, align 8
  ret i64 %loadtmp19

then_390:                                         ; preds = %merge_389
  %loadtmp22 = load i64, ptr @udp_sport_7, align 8
  ret i64 %loadtmp22
}

define i64 @udp_set_sport(i64 %idx, i64 %val) {
entry:
  %"%udp_sport_7" = alloca i64, align 8
  %"%udp_sport_6" = alloca i64, align 8
  %"%udp_sport_5" = alloca i64, align 8
  %"%udp_sport_4" = alloca i64, align 8
  %"%udp_sport_3" = alloca i64, align 8
  %"%udp_sport_2" = alloca i64, align 8
  %"%udp_sport_1" = alloca i64, align 8
  %"%udp_sport_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_392, label %merge_393

merge_393:                                        ; preds = %then_392, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_394, label %merge_395

merge_395:                                        ; preds = %then_394, %merge_393
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_396, label %merge_397

merge_397:                                        ; preds = %then_396, %merge_395
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_398, label %merge_399

merge_399:                                        ; preds = %then_398, %merge_397
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_400, label %merge_401

merge_401:                                        ; preds = %then_400, %merge_399
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_402, label %merge_403

merge_403:                                        ; preds = %then_402, %merge_401
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_404, label %merge_405

merge_405:                                        ; preds = %then_404, %merge_403
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_406, label %merge_407

merge_407:                                        ; preds = %then_406, %merge_405
  ret i64 0

then_392:                                         ; preds = %entry
  %"%943.load" = load i64, ptr %val2, align 8
  store i64 %"%943.load", ptr %"%udp_sport_0", align 8
  br label %merge_393

then_394:                                         ; preds = %merge_393
  %"%947.load" = load i64, ptr %val2, align 8
  store i64 %"%947.load", ptr %"%udp_sport_1", align 8
  br label %merge_395

then_396:                                         ; preds = %merge_395
  %"%951.load" = load i64, ptr %val2, align 8
  store i64 %"%951.load", ptr %"%udp_sport_2", align 8
  br label %merge_397

then_398:                                         ; preds = %merge_397
  %"%955.load" = load i64, ptr %val2, align 8
  store i64 %"%955.load", ptr %"%udp_sport_3", align 8
  br label %merge_399

then_400:                                         ; preds = %merge_399
  %"%959.load" = load i64, ptr %val2, align 8
  store i64 %"%959.load", ptr %"%udp_sport_4", align 8
  br label %merge_401

then_402:                                         ; preds = %merge_401
  %"%963.load" = load i64, ptr %val2, align 8
  store i64 %"%963.load", ptr %"%udp_sport_5", align 8
  br label %merge_403

then_404:                                         ; preds = %merge_403
  %"%967.load" = load i64, ptr %val2, align 8
  store i64 %"%967.load", ptr %"%udp_sport_6", align 8
  br label %merge_405

then_406:                                         ; preds = %merge_405
  %"%971.load" = load i64, ptr %val2, align 8
  store i64 %"%971.load", ptr %"%udp_sport_7", align 8
  br label %merge_407
}

define i64 @alloc_port() {
entry:
  %"%auto_port" = alloca i64, align 8
  %"%port" = alloca i64, align 8
  %"%974.load" = load i64, ptr @auto_port, align 8
  store i64 %"%974.load", ptr %"%port", align 8
  %"%auto_port.load" = load i64, ptr @auto_port, align 8
  %addtmp = add i64 %"%auto_port.load", 1
  store i64 %addtmp, ptr %"%auto_port", align 8
  %"%auto_port.load1" = load i64, ptr %"%auto_port", align 8
  %cmpgttmp = icmp sgt i64 %"%auto_port.load1", 65535
  br i1 %cmpgttmp, label %then_408, label %merge_409

merge_409:                                        ; preds = %then_408, %entry
  %loadtmp = load i64, ptr %"%port", align 8
  ret i64 %loadtmp

then_408:                                         ; preds = %entry
  store i64 49152, ptr %"%auto_port", align 8
  br label %merge_409
}

define i64 @udp_build_header(i64 %buf, i64 %src_port, i64 %dst_port, i64 %data_len) {
entry:
  %"%total_len" = alloca i64, align 8
  %data_len4 = alloca i64, align 8
  store i64 %data_len, ptr %data_len4, align 8
  %dst_port3 = alloca i64, align 8
  store i64 %dst_port, ptr %dst_port3, align 8
  %src_port2 = alloca i64, align 8
  store i64 %src_port, ptr %src_port2, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%UDP_HEADER_SIZE.load" = load i64, ptr @UDP_HEADER_SIZE, align 8
  %"%data_len.load" = load i64, ptr %data_len4, align 8
  %addtmp = add i64 %"%UDP_HEADER_SIZE.load", %"%data_len.load"
  store i64 %addtmp, ptr %"%total_len", align 8
  %"%src_port.load" = load i64, ptr %src_port2, align 8
  %shrtmp = lshr i64 %"%src_port.load", 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  call void @sad_ll_mem_write8(i64 %"%buf.load", i64 %shrtmp)
  %"%buf.load5" = load i64, ptr %buf1, align 8
  %addtmp6 = add i64 %"%buf.load5", 1
  %"%src_port.load7" = load i64, ptr %src_port2, align 8
  %andtmp = and i64 %"%src_port.load7", 255
  call void @sad_ll_mem_write8(i64 %addtmp6, i64 %andtmp)
  %"%buf.load8" = load i64, ptr %buf1, align 8
  %addtmp9 = add i64 %"%buf.load8", 2
  %"%dst_port.load" = load i64, ptr %dst_port3, align 8
  %shrtmp10 = lshr i64 %"%dst_port.load", 8
  call void @sad_ll_mem_write8(i64 %addtmp9, i64 %shrtmp10)
  %"%buf.load11" = load i64, ptr %buf1, align 8
  %addtmp12 = add i64 %"%buf.load11", 3
  %"%dst_port.load13" = load i64, ptr %dst_port3, align 8
  %andtmp14 = and i64 %"%dst_port.load13", 255
  call void @sad_ll_mem_write8(i64 %addtmp12, i64 %andtmp14)
  %"%buf.load15" = load i64, ptr %buf1, align 8
  %addtmp16 = add i64 %"%buf.load15", 4
  %"%total_len.load" = load i64, ptr %"%total_len", align 8
  %shrtmp17 = lshr i64 %"%total_len.load", 8
  call void @sad_ll_mem_write8(i64 %addtmp16, i64 %shrtmp17)
  %"%buf.load18" = load i64, ptr %buf1, align 8
  %addtmp19 = add i64 %"%buf.load18", 5
  %"%total_len.load20" = load i64, ptr %"%total_len", align 8
  %andtmp21 = and i64 %"%total_len.load20", 255
  call void @sad_ll_mem_write8(i64 %addtmp19, i64 %andtmp21)
  %"%buf.load22" = load i64, ptr %buf1, align 8
  %addtmp23 = add i64 %"%buf.load22", 6
  call void @sad_ll_mem_write8(i64 %addtmp23, i64 0)
  %"%buf.load24" = load i64, ptr %buf1, align 8
  %addtmp25 = add i64 %"%buf.load24", 7
  call void @sad_ll_mem_write8(i64 %addtmp25, i64 0)
  %loadtmp = load i64, ptr @UDP_HEADER_SIZE, align 8
  ret i64 %loadtmp
}

define i64 @udp_parse_src_port(i64 %buf) {
entry:
  %"%lo" = alloca i64, align 8
  %"%hi" = alloca i64, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  call void @sad_ll_mem_read8(i64 %"%buf.load")
  store i64 0, ptr %"%hi", align 8
  %"%buf.load2" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load2", 1
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%lo", align 8
  %"%hi.load" = load i64, ptr %"%hi", align 8
  %shltmp = shl i64 %"%hi.load", 8
  %"%lo.load" = load i64, ptr %"%lo", align 8
  %addtmp3 = add i64 %shltmp, %"%lo.load"
  ret i64 %addtmp3
}

define i64 @udp_parse_dst_port(i64 %buf) {
entry:
  %"%lo" = alloca i64, align 8
  %"%hi" = alloca i64, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load", 2
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%hi", align 8
  %"%buf.load2" = load i64, ptr %buf1, align 8
  %addtmp3 = add i64 %"%buf.load2", 3
  call void @sad_ll_mem_read8(i64 %addtmp3)
  store i64 0, ptr %"%lo", align 8
  %"%hi.load" = load i64, ptr %"%hi", align 8
  %shltmp = shl i64 %"%hi.load", 8
  %"%lo.load" = load i64, ptr %"%lo", align 8
  %addtmp4 = add i64 %shltmp, %"%lo.load"
  ret i64 %addtmp4
}

define i64 @udp_parse_length(i64 %buf) {
entry:
  %"%lo" = alloca i64, align 8
  %"%hi" = alloca i64, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load", 4
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%hi", align 8
  %"%buf.load2" = load i64, ptr %buf1, align 8
  %addtmp3 = add i64 %"%buf.load2", 5
  call void @sad_ll_mem_read8(i64 %addtmp3)
  store i64 0, ptr %"%lo", align 8
  %"%hi.load" = load i64, ptr %"%hi", align 8
  %shltmp = shl i64 %"%hi.load", 8
  %"%lo.load" = load i64, ptr %"%lo", align 8
  %addtmp4 = add i64 %shltmp, %"%lo.load"
  ret i64 %addtmp4
}

define i64 @udp_open(i64 %port) {
entry:
  %"%rxbuf" = alloca i64, align 8
  %"%active" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %port1 = alloca i64, align 8
  store i64 %port, ptr %port1, align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_410

else_417:                                         ; preds = %then_413
  %"%i.load12" = load i64, ptr %"%i", align 8
  %"%1079.load" = load i64, ptr %port1, align 8
  %calltmp13 = call i64 @udp_set_lport(i64 %"%i.load12", i64 %"%1079.load")
  br label %merge_416

merge_414:                                        ; preds = %while_body_411
  %"%i.load8" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load8", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_410

merge_416:                                        ; preds = %else_417, %then_415
  %"%i.load14" = load i64, ptr %"%i", align 8
  %calltmp15 = call i64 @udp_set_rxlen(i64 %"%i.load14", i64 0)
  %"%i.load16" = load i64, ptr %"%i", align 8
  %calltmp17 = call i64 @udp_set_sender(i64 %"%i.load16", i64 0)
  %"%i.load18" = load i64, ptr %"%i", align 8
  %calltmp19 = call i64 @udp_set_sport(i64 %"%i.load18", i64 0)
  call void @sad_ll_kmalloc(i64 512)
  store i64 0, ptr %"%rxbuf", align 8
  %"%rxbuf.load" = load i64, ptr %"%rxbuf", align 8
  %cmpgttmp = icmp sgt i64 %"%rxbuf.load", 0
  br i1 %cmpgttmp, label %then_418, label %merge_419

merge_419:                                        ; preds = %then_418, %merge_416
  %calltmp24 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %"%i.load25" = load i64, ptr %"%i", align 8
  %calltmp26 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%i.load25")
  %calltmp27 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %"%i.load28" = load i64, ptr %"%i", align 8
  %calltmp29 = call i64 @udp_get_lport(i64 %"%i.load28")
  %calltmp30 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp29)
  %calltmp31 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr %"%i", align 8
  ret i64 %loadtmp

then_413:                                         ; preds = %while_body_411
  %"%i.load5" = load i64, ptr %"%i", align 8
  %calltmp6 = call i64 @udp_set_active(i64 %"%i.load5", i64 1)
  %"%port.load" = load i64, ptr %port1, align 8
  %cmpeqtmp7 = icmp eq i64 %"%port.load", 0
  br i1 %cmpeqtmp7, label %then_415, label %else_417

then_415:                                         ; preds = %then_413
  %calltmp9 = call i64 @alloc_port()
  %"%i.load10" = load i64, ptr %"%i", align 8
  %calltmp11 = call i64 @udp_set_lport(i64 %"%i.load10", i64 %calltmp9)
  br label %merge_416

then_418:                                         ; preds = %merge_416
  %"%rxbuf.load20" = load i64, ptr %"%rxbuf", align 8
  call void @sad_ll_memset(i64 %"%rxbuf.load20", i64 0, i64 512)
  %"%i.load21" = load i64, ptr %"%i", align 8
  %"%rxbuf.load22" = load i64, ptr %"%rxbuf", align 8
  %calltmp23 = call i64 @udp_set_rxbuf(i64 %"%i.load21", i64 %"%rxbuf.load22")
  br label %merge_419

while_body_411:                                   ; preds = %while_cond_410
  %"%i.load2" = load i64, ptr %"%i", align 8
  %calltmp = call i64 @udp_get_active(i64 %"%i.load2")
  store i64 %calltmp, ptr %"%active", align 8
  %"%active.load" = load i64, ptr %"%active", align 8
  %cmpeqtmp = icmp eq i64 %"%active.load", 0
  br i1 %cmpeqtmp, label %then_413, label %merge_414

while_cond_410:                                   ; preds = %merge_414, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%UDP_MAX_SOCKETS.load" = load i64, ptr @UDP_MAX_SOCKETS, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%UDP_MAX_SOCKETS.load"
  br i1 %cmplttmp, label %while_body_411, label %while_exit_412

while_exit_412:                                   ; preds = %while_cond_410
  %calltmp3 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %calltmp4 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -1
}

define i64 @udp_close(i64 %sock_id) {
entry:
  %"%rxbuf" = alloca i64, align 8
  %sock_id1 = alloca i64, align 8
  store i64 %sock_id, ptr %sock_id1, align 8
  %"%sock_id.load" = load i64, ptr %sock_id1, align 8
  %cmplttmp = icmp slt i64 %"%sock_id.load", 0
  br i1 %cmplttmp, label %then_420, label %merge_421

merge_421:                                        ; preds = %entry
  %"%sock_id.load2" = load i64, ptr %sock_id1, align 8
  %"%UDP_MAX_SOCKETS.load" = load i64, ptr @UDP_MAX_SOCKETS, align 8
  %cmpgetmp = icmp sge i64 %"%sock_id.load2", %"%UDP_MAX_SOCKETS.load"
  br i1 %cmpgetmp, label %then_422, label %merge_423

merge_423:                                        ; preds = %merge_421
  %"%sock_id.load3" = load i64, ptr %sock_id1, align 8
  %calltmp = call i64 @udp_get_rxbuf(i64 %"%sock_id.load3")
  store i64 %calltmp, ptr %"%rxbuf", align 8
  %"%rxbuf.load" = load i64, ptr %"%rxbuf", align 8
  %cmpgttmp = icmp sgt i64 %"%rxbuf.load", 0
  br i1 %cmpgttmp, label %then_424, label %merge_425

merge_425:                                        ; preds = %then_424, %merge_423
  %"%sock_id.load5" = load i64, ptr %sock_id1, align 8
  %calltmp6 = call i64 @udp_set_active(i64 %"%sock_id.load5", i64 0)
  %"%sock_id.load7" = load i64, ptr %sock_id1, align 8
  %calltmp8 = call i64 @udp_set_lport(i64 %"%sock_id.load7", i64 0)
  %"%sock_id.load9" = load i64, ptr %sock_id1, align 8
  %calltmp10 = call i64 @udp_set_rxbuf(i64 %"%sock_id.load9", i64 0)
  %"%sock_id.load11" = load i64, ptr %sock_id1, align 8
  %calltmp12 = call i64 @udp_set_rxlen(i64 %"%sock_id.load11", i64 0)
  %"%sock_id.load13" = load i64, ptr %sock_id1, align 8
  %calltmp14 = call i64 @udp_set_sender(i64 %"%sock_id.load13", i64 0)
  %"%sock_id.load15" = load i64, ptr %sock_id1, align 8
  %calltmp16 = call i64 @udp_set_sport(i64 %"%sock_id.load15", i64 0)
  %calltmp17 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %"%sock_id.load18" = load i64, ptr %sock_id1, align 8
  %calltmp19 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%sock_id.load18")
  %calltmp20 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1

then_420:                                         ; preds = %entry
  ret i64 0

then_422:                                         ; preds = %merge_421
  ret i64 0

then_424:                                         ; preds = %merge_423
  %"%rxbuf.load4" = load i64, ptr %"%rxbuf", align 8
  call void @sad_ll_kfree(i64 %"%rxbuf.load4")
  br label %merge_425
}

define i64 @udp_sendto(i64 %sock_id, i64 %dst_ip, i64 %dst_port, i64 %data_addr, i64 %data_len) {
entry:
  %"%udp_tx_count" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%udp_errors" = alloca i64, align 8
  %"%udp_buf" = alloca i64, align 8
  %"%udp_total" = alloca i64, align 8
  %"%src_port" = alloca i64, align 8
  %"%active" = alloca i64, align 8
  %data_len5 = alloca i64, align 8
  store i64 %data_len, ptr %data_len5, align 8
  %data_addr4 = alloca i64, align 8
  store i64 %data_addr, ptr %data_addr4, align 8
  %dst_port3 = alloca i64, align 8
  store i64 %dst_port, ptr %dst_port3, align 8
  %dst_ip2 = alloca i64, align 8
  store i64 %dst_ip, ptr %dst_ip2, align 8
  %sock_id1 = alloca i64, align 8
  store i64 %sock_id, ptr %sock_id1, align 8
  %"%sock_id.load" = load i64, ptr %sock_id1, align 8
  %cmplttmp = icmp slt i64 %"%sock_id.load", 0
  br i1 %cmplttmp, label %then_426, label %merge_427

else_436:                                         ; preds = %merge_433
  %"%udp_errors.load27" = load i64, ptr %"%udp_errors", align 8
  %addtmp28 = add i64 %"%udp_errors.load27", 1
  store i64 %addtmp28, ptr %"%udp_errors", align 8
  br label %merge_435

merge_427:                                        ; preds = %entry
  %"%sock_id.load6" = load i64, ptr %sock_id1, align 8
  %"%UDP_MAX_SOCKETS.load" = load i64, ptr @UDP_MAX_SOCKETS, align 8
  %cmpgetmp = icmp sge i64 %"%sock_id.load6", %"%UDP_MAX_SOCKETS.load"
  br i1 %cmpgetmp, label %then_428, label %merge_429

merge_429:                                        ; preds = %merge_427
  %"%sock_id.load7" = load i64, ptr %sock_id1, align 8
  %calltmp = call i64 @udp_get_active(i64 %"%sock_id.load7")
  store i64 %calltmp, ptr %"%active", align 8
  %"%active.load" = load i64, ptr %"%active", align 8
  %cmpeqtmp = icmp eq i64 %"%active.load", 0
  br i1 %cmpeqtmp, label %then_430, label %merge_431

merge_431:                                        ; preds = %merge_429
  %"%sock_id.load10" = load i64, ptr %sock_id1, align 8
  %calltmp11 = call i64 @udp_get_lport(i64 %"%sock_id.load10")
  store i64 %calltmp11, ptr %"%src_port", align 8
  %"%UDP_HEADER_SIZE.load" = load i64, ptr @UDP_HEADER_SIZE, align 8
  %"%data_len.load" = load i64, ptr %data_len5, align 8
  %addtmp = add i64 %"%UDP_HEADER_SIZE.load", %"%data_len.load"
  store i64 %addtmp, ptr %"%udp_total", align 8
  %"%udp_total.load" = load i64, ptr %"%udp_total", align 8
  call void @sad_ll_kmalloc(i64 %"%udp_total.load")
  store i64 0, ptr %"%udp_buf", align 8
  %"%udp_buf.load" = load i64, ptr %"%udp_buf", align 8
  %cmpeqtmp12 = icmp eq i64 %"%udp_buf.load", 0
  br i1 %cmpeqtmp12, label %then_432, label %merge_433

merge_433:                                        ; preds = %merge_431
  %"%udp_buf.load14" = load i64, ptr %"%udp_buf", align 8
  %"%udp_total.load15" = load i64, ptr %"%udp_total", align 8
  call void @sad_ll_memset(i64 %"%udp_buf.load14", i64 0, i64 %"%udp_total.load15")
  %"%udp_buf.load16" = load i64, ptr %"%udp_buf", align 8
  %"%src_port.load" = load i64, ptr %"%src_port", align 8
  %"%dst_port.load" = load i64, ptr %dst_port3, align 8
  %"%data_len.load17" = load i64, ptr %data_len5, align 8
  %calltmp18 = call i64 @udp_build_header(i64 %"%udp_buf.load16", i64 %"%src_port.load", i64 %"%dst_port.load", i64 %"%data_len.load17")
  %"%udp_buf.load19" = load i64, ptr %"%udp_buf", align 8
  %"%UDP_HEADER_SIZE.load20" = load i64, ptr @UDP_HEADER_SIZE, align 8
  %addtmp21 = add i64 %"%udp_buf.load19", %"%UDP_HEADER_SIZE.load20"
  %"%data_addr.load" = load i64, ptr %data_addr4, align 8
  %"%data_len.load22" = load i64, ptr %data_len5, align 8
  call void @sad_ll_memcpy(i64 %addtmp21, i64 %"%data_addr.load", i64 %"%data_len.load22")
  %"%dst_ip.load" = load i64, ptr %dst_ip2, align 8
  %"%IP_PROTO_UDP.load" = load i64, ptr @IP_PROTO_UDP, align 8
  %"%udp_buf.load23" = load i64, ptr %"%udp_buf", align 8
  %"%udp_total.load24" = load i64, ptr %"%udp_total", align 8
  call void @ip_send(i64 %"%dst_ip.load", i64 %"%IP_PROTO_UDP.load", i64 %"%udp_buf.load23", i64 %"%udp_total.load24")
  store i64 0, ptr %"%result", align 8
  %"%udp_buf.load25" = load i64, ptr %"%udp_buf", align 8
  call void @sad_ll_kfree(i64 %"%udp_buf.load25")
  %"%result.load" = load i64, ptr %"%result", align 8
  %cmpgttmp = icmp sgt i64 %"%result.load", 0
  br i1 %cmpgttmp, label %then_434, label %else_436

merge_435:                                        ; preds = %else_436, %then_434
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

then_426:                                         ; preds = %entry
  ret i64 0

then_428:                                         ; preds = %merge_427
  ret i64 0

then_430:                                         ; preds = %merge_429
  %calltmp8 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %calltmp9 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_432:                                         ; preds = %merge_431
  %"%udp_errors.load" = load i64, ptr @udp_errors, align 8
  %addtmp13 = add i64 %"%udp_errors.load", 1
  store i64 %addtmp13, ptr %"%udp_errors", align 8
  ret i64 0

then_434:                                         ; preds = %merge_433
  %"%udp_tx_count.load" = load i64, ptr @udp_tx_count, align 8
  %addtmp26 = add i64 %"%udp_tx_count.load", 1
  store i64 %addtmp26, ptr %"%udp_tx_count", align 8
  br label %merge_435
}

define i64 @udp_broadcast(i64 %sock_id, i64 %dst_port, i64 %data_addr, i64 %data_len) {
entry:
  %"%udp_tx_count" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%udp_buf" = alloca i64, align 8
  %"%udp_total" = alloca i64, align 8
  %"%src_port" = alloca i64, align 8
  %data_len4 = alloca i64, align 8
  store i64 %data_len, ptr %data_len4, align 8
  %data_addr3 = alloca i64, align 8
  store i64 %data_addr, ptr %data_addr3, align 8
  %dst_port2 = alloca i64, align 8
  store i64 %dst_port, ptr %dst_port2, align 8
  %sock_id1 = alloca i64, align 8
  store i64 %sock_id, ptr %sock_id1, align 8
  %"%sock_id.load" = load i64, ptr %sock_id1, align 8
  %cmplttmp = icmp slt i64 %"%sock_id.load", 0
  br i1 %cmplttmp, label %then_437, label %merge_438

merge_438:                                        ; preds = %entry
  %"%sock_id.load5" = load i64, ptr %sock_id1, align 8
  %calltmp = call i64 @udp_get_lport(i64 %"%sock_id.load5")
  store i64 %calltmp, ptr %"%src_port", align 8
  %"%UDP_HEADER_SIZE.load" = load i64, ptr @UDP_HEADER_SIZE, align 8
  %"%data_len.load" = load i64, ptr %data_len4, align 8
  %addtmp = add i64 %"%UDP_HEADER_SIZE.load", %"%data_len.load"
  store i64 %addtmp, ptr %"%udp_total", align 8
  %"%udp_total.load" = load i64, ptr %"%udp_total", align 8
  call void @sad_ll_kmalloc(i64 %"%udp_total.load")
  store i64 0, ptr %"%udp_buf", align 8
  %"%udp_buf.load" = load i64, ptr %"%udp_buf", align 8
  %cmpeqtmp = icmp eq i64 %"%udp_buf.load", 0
  br i1 %cmpeqtmp, label %then_439, label %merge_440

merge_440:                                        ; preds = %merge_438
  %"%udp_buf.load6" = load i64, ptr %"%udp_buf", align 8
  %"%udp_total.load7" = load i64, ptr %"%udp_total", align 8
  call void @sad_ll_memset(i64 %"%udp_buf.load6", i64 0, i64 %"%udp_total.load7")
  %"%udp_buf.load8" = load i64, ptr %"%udp_buf", align 8
  %"%src_port.load" = load i64, ptr %"%src_port", align 8
  %"%dst_port.load" = load i64, ptr %dst_port2, align 8
  %"%data_len.load9" = load i64, ptr %data_len4, align 8
  %calltmp10 = call i64 @udp_build_header(i64 %"%udp_buf.load8", i64 %"%src_port.load", i64 %"%dst_port.load", i64 %"%data_len.load9")
  %"%udp_buf.load11" = load i64, ptr %"%udp_buf", align 8
  %"%UDP_HEADER_SIZE.load12" = load i64, ptr @UDP_HEADER_SIZE, align 8
  %addtmp13 = add i64 %"%udp_buf.load11", %"%UDP_HEADER_SIZE.load12"
  %"%data_addr.load" = load i64, ptr %data_addr3, align 8
  %"%data_len.load14" = load i64, ptr %data_len4, align 8
  call void @sad_ll_memcpy(i64 %addtmp13, i64 %"%data_addr.load", i64 %"%data_len.load14")
  %"%IP_PROTO_UDP.load" = load i64, ptr @IP_PROTO_UDP, align 8
  %"%udp_buf.load15" = load i64, ptr %"%udp_buf", align 8
  %"%udp_total.load16" = load i64, ptr %"%udp_total", align 8
  call void @ip_send_broadcast(i64 %"%IP_PROTO_UDP.load", i64 %"%udp_buf.load15", i64 %"%udp_total.load16")
  store i64 0, ptr %"%result", align 8
  %"%udp_buf.load17" = load i64, ptr %"%udp_buf", align 8
  call void @sad_ll_kfree(i64 %"%udp_buf.load17")
  %"%result.load" = load i64, ptr %"%result", align 8
  %cmpgttmp = icmp sgt i64 %"%result.load", 0
  br i1 %cmpgttmp, label %then_441, label %merge_442

merge_442:                                        ; preds = %then_441, %merge_440
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

then_437:                                         ; preds = %entry
  ret i64 0

then_439:                                         ; preds = %merge_438
  ret i64 0

then_441:                                         ; preds = %merge_440
  %"%udp_tx_count.load" = load i64, ptr @udp_tx_count, align 8
  %addtmp18 = add i64 %"%udp_tx_count.load", 1
  store i64 %addtmp18, ptr %"%udp_tx_count", align 8
  br label %merge_442
}

define i64 @udp_handle_packet(i64 %payload, i64 %payload_len, i64 %src_ip) {
entry:
  %"%copy_len" = alloca i64, align 8
  %"%rxbuf" = alloca i64, align 8
  %"%lport" = alloca i64, align 8
  %"%active" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %"%udp_rx_count" = alloca i64, align 8
  %"%data_addr" = alloca i64, align 8
  %"%data_len" = alloca i64, align 8
  %"%udp_len" = alloca i64, align 8
  %"%dst_port" = alloca i64, align 8
  %"%src_port" = alloca i64, align 8
  %"%udp_errors" = alloca i64, align 8
  %src_ip3 = alloca i64, align 8
  store i64 %src_ip, ptr %src_ip3, align 8
  %payload_len2 = alloca i64, align 8
  store i64 %payload_len, ptr %payload_len2, align 8
  %payload1 = alloca i64, align 8
  store i64 %payload, ptr %payload1, align 8
  %"%payload_len.load" = load i64, ptr %payload_len2, align 8
  %"%UDP_HEADER_SIZE.load" = load i64, ptr @UDP_HEADER_SIZE, align 8
  %cmplttmp = icmp slt i64 %"%payload_len.load", %"%UDP_HEADER_SIZE.load"
  br i1 %cmplttmp, label %then_443, label %merge_444

merge_444:                                        ; preds = %entry
  %"%payload.load" = load i64, ptr %payload1, align 8
  %calltmp5 = call i64 @udp_parse_src_port(i64 %"%payload.load")
  store i64 %calltmp5, ptr %"%src_port", align 8
  %"%payload.load6" = load i64, ptr %payload1, align 8
  %calltmp7 = call i64 @udp_parse_dst_port(i64 %"%payload.load6")
  store i64 %calltmp7, ptr %"%dst_port", align 8
  %"%payload.load8" = load i64, ptr %payload1, align 8
  %calltmp9 = call i64 @udp_parse_length(i64 %"%payload.load8")
  store i64 %calltmp9, ptr %"%udp_len", align 8
  %"%udp_len.load" = load i64, ptr %"%udp_len", align 8
  %"%UDP_HEADER_SIZE.load10" = load i64, ptr @UDP_HEADER_SIZE, align 8
  %subtmp = sub i64 %"%udp_len.load", %"%UDP_HEADER_SIZE.load10"
  store i64 %subtmp, ptr %"%data_len", align 8
  %"%payload.load11" = load i64, ptr %payload1, align 8
  %"%UDP_HEADER_SIZE.load12" = load i64, ptr @UDP_HEADER_SIZE, align 8
  %addtmp13 = add i64 %"%payload.load11", %"%UDP_HEADER_SIZE.load12"
  store i64 %addtmp13, ptr %"%data_addr", align 8
  %calltmp14 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  %"%src_port.load" = load i64, ptr %"%src_port", align 8
  %calltmp15 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%src_port.load")
  %calltmp16 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %"%dst_port.load" = load i64, ptr %"%dst_port", align 8
  %calltmp17 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%dst_port.load")
  %calltmp18 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %"%data_len.load" = load i64, ptr %"%data_len", align 8
  %calltmp19 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%data_len.load")
  %calltmp20 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  %"%udp_rx_count.load" = load i64, ptr @udp_rx_count, align 8
  %addtmp21 = add i64 %"%udp_rx_count.load", 1
  store i64 %addtmp21, ptr %"%udp_rx_count", align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_445

merge_449:                                        ; preds = %merge_451, %while_body_446
  %"%i.load32" = load i64, ptr %"%i", align 8
  %addtmp33 = add i64 %"%i.load32", 1
  store i64 %addtmp33, ptr %"%i", align 8
  br label %while_cond_445

merge_451:                                        ; preds = %then_448
  br label %merge_449

merge_453:                                        ; preds = %merge_455, %then_450
  ret i64 1

merge_455:                                        ; preds = %then_454, %then_452
  %"%rxbuf.load38" = load i64, ptr %"%rxbuf", align 8
  %"%data_addr.load" = load i64, ptr %"%data_addr", align 8
  %"%copy_len.load39" = load i64, ptr %"%copy_len", align 8
  call void @sad_ll_memcpy(i64 %"%rxbuf.load38", i64 %"%data_addr.load", i64 %"%copy_len.load39")
  %"%i.load40" = load i64, ptr %"%i", align 8
  %"%copy_len.load41" = load i64, ptr %"%copy_len", align 8
  %calltmp42 = call i64 @udp_set_rxlen(i64 %"%i.load40", i64 %"%copy_len.load41")
  %"%i.load43" = load i64, ptr %"%i", align 8
  %"%1274.load" = load i64, ptr %src_ip3, align 8
  %calltmp44 = call i64 @udp_set_sender(i64 %"%i.load43", i64 %"%1274.load")
  %"%i.load45" = load i64, ptr %"%i", align 8
  %"%src_port.load46" = load i64, ptr %"%src_port", align 8
  %calltmp47 = call i64 @udp_set_sport(i64 %"%i.load45", i64 %"%src_port.load46")
  br label %merge_453

then_443:                                         ; preds = %entry
  %calltmp = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %calltmp4 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  %"%udp_errors.load" = load i64, ptr @udp_errors, align 8
  %addtmp = add i64 %"%udp_errors.load", 1
  store i64 %addtmp, ptr %"%udp_errors", align 8
  ret i64 0

then_448:                                         ; preds = %while_body_446
  %"%i.load29" = load i64, ptr %"%i", align 8
  %calltmp30 = call i64 @udp_get_lport(i64 %"%i.load29")
  store i64 %calltmp30, ptr %"%lport", align 8
  %"%lport.load" = load i64, ptr %"%lport", align 8
  %"%dst_port.load31" = load i64, ptr %"%dst_port", align 8
  %cmpeqtmp = icmp eq i64 %"%lport.load", %"%dst_port.load31"
  br i1 %cmpeqtmp, label %then_450, label %merge_451

then_450:                                         ; preds = %then_448
  %"%i.load34" = load i64, ptr %"%i", align 8
  %calltmp35 = call i64 @udp_get_rxbuf(i64 %"%i.load34")
  store i64 %calltmp35, ptr %"%rxbuf", align 8
  %"%rxbuf.load" = load i64, ptr %"%rxbuf", align 8
  %cmpgttmp36 = icmp sgt i64 %"%rxbuf.load", 0
  br i1 %cmpgttmp36, label %then_452, label %merge_453

then_452:                                         ; preds = %then_450
  %"%1267.load" = load i64, ptr %"%data_len", align 8
  store i64 %"%1267.load", ptr %"%copy_len", align 8
  %"%copy_len.load" = load i64, ptr %"%copy_len", align 8
  %cmpgttmp37 = icmp sgt i64 %"%copy_len.load", 512
  br i1 %cmpgttmp37, label %then_454, label %merge_455

then_454:                                         ; preds = %then_452
  store i64 512, ptr %"%copy_len", align 8
  br label %merge_455

while_body_446:                                   ; preds = %while_cond_445
  %"%i.load23" = load i64, ptr %"%i", align 8
  %calltmp24 = call i64 @udp_get_active(i64 %"%i.load23")
  store i64 %calltmp24, ptr %"%active", align 8
  %"%active.load" = load i64, ptr %"%active", align 8
  %cmpgttmp = icmp sgt i64 %"%active.load", 0
  br i1 %cmpgttmp, label %then_448, label %merge_449

while_cond_445:                                   ; preds = %merge_449, %merge_444
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%UDP_MAX_SOCKETS.load" = load i64, ptr @UDP_MAX_SOCKETS, align 8
  %cmplttmp22 = icmp slt i64 %"%i.load", %"%UDP_MAX_SOCKETS.load"
  br i1 %cmplttmp22, label %while_body_446, label %while_exit_447

while_exit_447:                                   ; preds = %while_cond_445
  %calltmp25 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  %"%dst_port.load26" = load i64, ptr %"%dst_port", align 8
  %calltmp27 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%dst_port.load26")
  %calltmp28 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @udp_recv(i64 %sock_id, i64 %dest_addr, i64 %max_len) {
entry:
  %"%copy_len" = alloca i64, align 8
  %"%rxbuf" = alloca i64, align 8
  %"%rxlen" = alloca i64, align 8
  %max_len3 = alloca i64, align 8
  store i64 %max_len, ptr %max_len3, align 8
  %dest_addr2 = alloca i64, align 8
  store i64 %dest_addr, ptr %dest_addr2, align 8
  %sock_id1 = alloca i64, align 8
  store i64 %sock_id, ptr %sock_id1, align 8
  %"%sock_id.load" = load i64, ptr %sock_id1, align 8
  %cmplttmp = icmp slt i64 %"%sock_id.load", 0
  br i1 %cmplttmp, label %then_456, label %merge_457

merge_457:                                        ; preds = %entry
  %"%sock_id.load4" = load i64, ptr %sock_id1, align 8
  %calltmp = call i64 @udp_get_rxlen(i64 %"%sock_id.load4")
  store i64 %calltmp, ptr %"%rxlen", align 8
  %"%rxlen.load" = load i64, ptr %"%rxlen", align 8
  %cmpeqtmp = icmp eq i64 %"%rxlen.load", 0
  br i1 %cmpeqtmp, label %then_458, label %merge_459

merge_459:                                        ; preds = %merge_457
  %"%sock_id.load5" = load i64, ptr %sock_id1, align 8
  %calltmp6 = call i64 @udp_get_rxbuf(i64 %"%sock_id.load5")
  store i64 %calltmp6, ptr %"%rxbuf", align 8
  %"%rxbuf.load" = load i64, ptr %"%rxbuf", align 8
  %cmpeqtmp7 = icmp eq i64 %"%rxbuf.load", 0
  br i1 %cmpeqtmp7, label %then_460, label %merge_461

merge_461:                                        ; preds = %merge_459
  %"%1301.load" = load i64, ptr %"%rxlen", align 8
  store i64 %"%1301.load", ptr %"%copy_len", align 8
  %"%copy_len.load" = load i64, ptr %"%copy_len", align 8
  %"%max_len.load" = load i64, ptr %max_len3, align 8
  %cmpgttmp = icmp sgt i64 %"%copy_len.load", %"%max_len.load"
  br i1 %cmpgttmp, label %then_462, label %merge_463

merge_463:                                        ; preds = %then_462, %merge_461
  %"%dest_addr.load" = load i64, ptr %dest_addr2, align 8
  %"%rxbuf.load8" = load i64, ptr %"%rxbuf", align 8
  %"%copy_len.load9" = load i64, ptr %"%copy_len", align 8
  call void @sad_ll_memcpy(i64 %"%dest_addr.load", i64 %"%rxbuf.load8", i64 %"%copy_len.load9")
  %"%sock_id.load10" = load i64, ptr %sock_id1, align 8
  %calltmp11 = call i64 @udp_set_rxlen(i64 %"%sock_id.load10", i64 0)
  %loadtmp = load i64, ptr %"%copy_len", align 8
  ret i64 %loadtmp

then_456:                                         ; preds = %entry
  ret i64 0

then_458:                                         ; preds = %merge_457
  ret i64 0

then_460:                                         ; preds = %merge_459
  ret i64 0

then_462:                                         ; preds = %merge_461
  %"%1304.load" = load i64, ptr %max_len3, align 8
  store i64 %"%1304.load", ptr %"%copy_len", align 8
  br label %merge_463
}

define i64 @tcp_build_header(i64 %buf, i64 %src_port, i64 %dst_port, i64 %seq_num, i64 %ack_num, i64 %flags, i64 %window) {
entry:
  %window7 = alloca i64, align 8
  store i64 %window, ptr %window7, align 8
  %flags6 = alloca i64, align 8
  store i64 %flags, ptr %flags6, align 8
  %ack_num5 = alloca i64, align 8
  store i64 %ack_num, ptr %ack_num5, align 8
  %seq_num4 = alloca i64, align 8
  store i64 %seq_num, ptr %seq_num4, align 8
  %dst_port3 = alloca i64, align 8
  store i64 %dst_port, ptr %dst_port3, align 8
  %src_port2 = alloca i64, align 8
  store i64 %src_port, ptr %src_port2, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%src_port.load" = load i64, ptr %src_port2, align 8
  %shrtmp = lshr i64 %"%src_port.load", 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  call void @sad_ll_mem_write8(i64 %"%buf.load", i64 %shrtmp)
  %"%buf.load8" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load8", 1
  %"%src_port.load9" = load i64, ptr %src_port2, align 8
  %andtmp = and i64 %"%src_port.load9", 255
  call void @sad_ll_mem_write8(i64 %addtmp, i64 %andtmp)
  %"%buf.load10" = load i64, ptr %buf1, align 8
  %addtmp11 = add i64 %"%buf.load10", 2
  %"%dst_port.load" = load i64, ptr %dst_port3, align 8
  %shrtmp12 = lshr i64 %"%dst_port.load", 8
  call void @sad_ll_mem_write8(i64 %addtmp11, i64 %shrtmp12)
  %"%buf.load13" = load i64, ptr %buf1, align 8
  %addtmp14 = add i64 %"%buf.load13", 3
  %"%dst_port.load15" = load i64, ptr %dst_port3, align 8
  %andtmp16 = and i64 %"%dst_port.load15", 255
  call void @sad_ll_mem_write8(i64 %addtmp14, i64 %andtmp16)
  %"%buf.load17" = load i64, ptr %buf1, align 8
  %addtmp18 = add i64 %"%buf.load17", 4
  %"%seq_num.load" = load i64, ptr %seq_num4, align 8
  %shrtmp19 = lshr i64 %"%seq_num.load", 24
  %andtmp20 = and i64 %shrtmp19, 255
  call void @sad_ll_mem_write8(i64 %addtmp18, i64 %andtmp20)
  %"%buf.load21" = load i64, ptr %buf1, align 8
  %addtmp22 = add i64 %"%buf.load21", 5
  %"%seq_num.load23" = load i64, ptr %seq_num4, align 8
  %shrtmp24 = lshr i64 %"%seq_num.load23", 16
  %andtmp25 = and i64 %shrtmp24, 255
  call void @sad_ll_mem_write8(i64 %addtmp22, i64 %andtmp25)
  %"%buf.load26" = load i64, ptr %buf1, align 8
  %addtmp27 = add i64 %"%buf.load26", 6
  %"%seq_num.load28" = load i64, ptr %seq_num4, align 8
  %shrtmp29 = lshr i64 %"%seq_num.load28", 8
  %andtmp30 = and i64 %shrtmp29, 255
  call void @sad_ll_mem_write8(i64 %addtmp27, i64 %andtmp30)
  %"%buf.load31" = load i64, ptr %buf1, align 8
  %addtmp32 = add i64 %"%buf.load31", 7
  %"%seq_num.load33" = load i64, ptr %seq_num4, align 8
  %andtmp34 = and i64 %"%seq_num.load33", 255
  call void @sad_ll_mem_write8(i64 %addtmp32, i64 %andtmp34)
  %"%buf.load35" = load i64, ptr %buf1, align 8
  %addtmp36 = add i64 %"%buf.load35", 8
  %"%ack_num.load" = load i64, ptr %ack_num5, align 8
  %shrtmp37 = lshr i64 %"%ack_num.load", 24
  %andtmp38 = and i64 %shrtmp37, 255
  call void @sad_ll_mem_write8(i64 %addtmp36, i64 %andtmp38)
  %"%buf.load39" = load i64, ptr %buf1, align 8
  %addtmp40 = add i64 %"%buf.load39", 9
  %"%ack_num.load41" = load i64, ptr %ack_num5, align 8
  %shrtmp42 = lshr i64 %"%ack_num.load41", 16
  %andtmp43 = and i64 %shrtmp42, 255
  call void @sad_ll_mem_write8(i64 %addtmp40, i64 %andtmp43)
  %"%buf.load44" = load i64, ptr %buf1, align 8
  %addtmp45 = add i64 %"%buf.load44", 10
  %"%ack_num.load46" = load i64, ptr %ack_num5, align 8
  %shrtmp47 = lshr i64 %"%ack_num.load46", 8
  %andtmp48 = and i64 %shrtmp47, 255
  call void @sad_ll_mem_write8(i64 %addtmp45, i64 %andtmp48)
  %"%buf.load49" = load i64, ptr %buf1, align 8
  %addtmp50 = add i64 %"%buf.load49", 11
  %"%ack_num.load51" = load i64, ptr %ack_num5, align 8
  %andtmp52 = and i64 %"%ack_num.load51", 255
  call void @sad_ll_mem_write8(i64 %addtmp50, i64 %andtmp52)
  %"%buf.load53" = load i64, ptr %buf1, align 8
  %addtmp54 = add i64 %"%buf.load53", 12
  call void @sad_ll_mem_write8(i64 %addtmp54, i64 80)
  %"%buf.load55" = load i64, ptr %buf1, align 8
  %addtmp56 = add i64 %"%buf.load55", 13
  %"%flags.load" = load i64, ptr %flags6, align 8
  call void @sad_ll_mem_write8(i64 %addtmp56, i64 %"%flags.load")
  %"%buf.load57" = load i64, ptr %buf1, align 8
  %addtmp58 = add i64 %"%buf.load57", 14
  %"%window.load" = load i64, ptr %window7, align 8
  %shrtmp59 = lshr i64 %"%window.load", 8
  call void @sad_ll_mem_write8(i64 %addtmp58, i64 %shrtmp59)
  %"%buf.load60" = load i64, ptr %buf1, align 8
  %addtmp61 = add i64 %"%buf.load60", 15
  %"%window.load62" = load i64, ptr %window7, align 8
  %andtmp63 = and i64 %"%window.load62", 255
  call void @sad_ll_mem_write8(i64 %addtmp61, i64 %andtmp63)
  %"%buf.load64" = load i64, ptr %buf1, align 8
  %addtmp65 = add i64 %"%buf.load64", 16
  call void @sad_ll_mem_write8(i64 %addtmp65, i64 0)
  %"%buf.load66" = load i64, ptr %buf1, align 8
  %addtmp67 = add i64 %"%buf.load66", 17
  call void @sad_ll_mem_write8(i64 %addtmp67, i64 0)
  %"%buf.load68" = load i64, ptr %buf1, align 8
  %addtmp69 = add i64 %"%buf.load68", 18
  call void @sad_ll_mem_write8(i64 %addtmp69, i64 0)
  %"%buf.load70" = load i64, ptr %buf1, align 8
  %addtmp71 = add i64 %"%buf.load70", 19
  call void @sad_ll_mem_write8(i64 %addtmp71, i64 0)
  %loadtmp = load i64, ptr @TCP_HEADER_SIZE, align 8
  ret i64 %loadtmp
}

define i64 @tcp_parse_src_port(i64 %buf) {
entry:
  %"%lo" = alloca i64, align 8
  %"%hi" = alloca i64, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  call void @sad_ll_mem_read8(i64 %"%buf.load")
  store i64 0, ptr %"%hi", align 8
  %"%buf.load2" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load2", 1
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%lo", align 8
  %"%hi.load" = load i64, ptr %"%hi", align 8
  %shltmp = shl i64 %"%hi.load", 8
  %"%lo.load" = load i64, ptr %"%lo", align 8
  %addtmp3 = add i64 %shltmp, %"%lo.load"
  ret i64 %addtmp3
}

define i64 @tcp_parse_dst_port(i64 %buf) {
entry:
  %"%lo" = alloca i64, align 8
  %"%hi" = alloca i64, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load", 2
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%hi", align 8
  %"%buf.load2" = load i64, ptr %buf1, align 8
  %addtmp3 = add i64 %"%buf.load2", 3
  call void @sad_ll_mem_read8(i64 %addtmp3)
  store i64 0, ptr %"%lo", align 8
  %"%hi.load" = load i64, ptr %"%hi", align 8
  %shltmp = shl i64 %"%hi.load", 8
  %"%lo.load" = load i64, ptr %"%lo", align 8
  %addtmp4 = add i64 %shltmp, %"%lo.load"
  ret i64 %addtmp4
}

define i64 @tcp_parse_seq(i64 %buf) {
entry:
  %"%b3" = alloca i64, align 8
  %"%b2" = alloca i64, align 8
  %"%b1" = alloca i64, align 8
  %"%b0" = alloca i64, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load", 4
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%b0", align 8
  %"%buf.load2" = load i64, ptr %buf1, align 8
  %addtmp3 = add i64 %"%buf.load2", 5
  call void @sad_ll_mem_read8(i64 %addtmp3)
  store i64 0, ptr %"%b1", align 8
  %"%buf.load4" = load i64, ptr %buf1, align 8
  %addtmp5 = add i64 %"%buf.load4", 6
  call void @sad_ll_mem_read8(i64 %addtmp5)
  store i64 0, ptr %"%b2", align 8
  %"%buf.load6" = load i64, ptr %buf1, align 8
  %addtmp7 = add i64 %"%buf.load6", 7
  call void @sad_ll_mem_read8(i64 %addtmp7)
  store i64 0, ptr %"%b3", align 8
  %"%b0.load" = load i64, ptr %"%b0", align 8
  %shltmp = shl i64 %"%b0.load", 24
  %"%b1.load" = load i64, ptr %"%b1", align 8
  %shltmp8 = shl i64 %"%b1.load", 16
  %addtmp9 = add i64 %shltmp, %shltmp8
  %"%b2.load" = load i64, ptr %"%b2", align 8
  %shltmp10 = shl i64 %"%b2.load", 8
  %addtmp11 = add i64 %addtmp9, %shltmp10
  %"%b3.load" = load i64, ptr %"%b3", align 8
  %addtmp12 = add i64 %addtmp11, %"%b3.load"
  ret i64 %addtmp12
}

define i64 @tcp_parse_ack(i64 %buf) {
entry:
  %"%b3" = alloca i64, align 8
  %"%b2" = alloca i64, align 8
  %"%b1" = alloca i64, align 8
  %"%b0" = alloca i64, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load", 8
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%b0", align 8
  %"%buf.load2" = load i64, ptr %buf1, align 8
  %addtmp3 = add i64 %"%buf.load2", 9
  call void @sad_ll_mem_read8(i64 %addtmp3)
  store i64 0, ptr %"%b1", align 8
  %"%buf.load4" = load i64, ptr %buf1, align 8
  %addtmp5 = add i64 %"%buf.load4", 10
  call void @sad_ll_mem_read8(i64 %addtmp5)
  store i64 0, ptr %"%b2", align 8
  %"%buf.load6" = load i64, ptr %buf1, align 8
  %addtmp7 = add i64 %"%buf.load6", 11
  call void @sad_ll_mem_read8(i64 %addtmp7)
  store i64 0, ptr %"%b3", align 8
  %"%b0.load" = load i64, ptr %"%b0", align 8
  %shltmp = shl i64 %"%b0.load", 24
  %"%b1.load" = load i64, ptr %"%b1", align 8
  %shltmp8 = shl i64 %"%b1.load", 16
  %addtmp9 = add i64 %shltmp, %shltmp8
  %"%b2.load" = load i64, ptr %"%b2", align 8
  %shltmp10 = shl i64 %"%b2.load", 8
  %addtmp11 = add i64 %addtmp9, %shltmp10
  %"%b3.load" = load i64, ptr %"%b3", align 8
  %addtmp12 = add i64 %addtmp11, %"%b3.load"
  ret i64 %addtmp12
}

define i64 @tcp_parse_flags(i64 %buf) {
entry:
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load", 13
  call void @sad_ll_mem_read8(i64 %addtmp)
  ret i64 0
}

define i64 @tcp_parse_data_offset(i64 %buf) {
entry:
  %"%byte12" = alloca i64, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load", 12
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%byte12", align 8
  %"%byte12.load" = load i64, ptr %"%byte12", align 8
  %shrtmp = lshr i64 %"%byte12.load", 4
  %shltmp = shl i64 %shrtmp, 2
  ret i64 %shltmp
}

define i64 @tcp_connect(i64 %dst_ip, i64 %dst_port) {
entry:
  %"%tcp_connections" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%tcp_errors" = alloca i64, align 8
  %"%tcp_buf" = alloca i64, align 8
  %"%isn" = alloca i64, align 8
  %"%lport" = alloca i64, align 8
  %"%st" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %"%sock" = alloca i64, align 8
  %dst_port2 = alloca i64, align 8
  store i64 %dst_port, ptr %dst_port2, align 8
  %dst_ip1 = alloca i64, align 8
  store i64 %dst_ip, ptr %dst_ip1, align 8
  store i64 -1, ptr %"%sock", align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_464

else_475:                                         ; preds = %merge_472
  %"%tcp_errors.load45" = load i64, ptr %"%tcp_errors", align 8
  %addtmp46 = add i64 %"%tcp_errors.load45", 1
  store i64 %addtmp46, ptr %"%tcp_errors", align 8
  ret i64 -1

merge_468:                                        ; preds = %then_467, %while_body_465
  %"%i.load6" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load6", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_464

merge_470:                                        ; preds = %while_exit_466
  %calltmp9 = call i64 @alloc_port()
  store i64 %calltmp9, ptr %"%lport", align 8
  %"%sock.load10" = load i64, ptr %"%sock", align 8
  %"%lport.load" = load i64, ptr %"%lport", align 8
  %calltmp11 = call i64 @tcp_set_lport(i64 %"%sock.load10", i64 %"%lport.load")
  %"%sock.load12" = load i64, ptr %"%sock", align 8
  %"%1548.load" = load i64, ptr %dst_port2, align 8
  %calltmp13 = call i64 @tcp_set_rport(i64 %"%sock.load12", i64 %"%1548.load")
  %"%sock.load14" = load i64, ptr %"%sock", align 8
  %"%1551.load" = load i64, ptr %dst_ip1, align 8
  %calltmp15 = call i64 @tcp_set_rip(i64 %"%sock.load14", i64 %"%1551.load")
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%isn", align 8
  %"%sock.load16" = load i64, ptr %"%sock", align 8
  %"%isn.load" = load i64, ptr %"%isn", align 8
  %calltmp17 = call i64 @tcp_set_seq(i64 %"%sock.load16", i64 %"%isn.load")
  %"%sock.load18" = load i64, ptr %"%sock", align 8
  %calltmp19 = call i64 @tcp_set_ack(i64 %"%sock.load18", i64 0)
  %"%TCP_HEADER_SIZE.load" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @sad_ll_kmalloc(i64 %"%TCP_HEADER_SIZE.load")
  store i64 0, ptr %"%tcp_buf", align 8
  %"%tcp_buf.load" = load i64, ptr %"%tcp_buf", align 8
  %cmpeqtmp20 = icmp eq i64 %"%tcp_buf.load", 0
  br i1 %cmpeqtmp20, label %then_471, label %merge_472

merge_472:                                        ; preds = %merge_470
  %"%tcp_buf.load22" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_HEADER_SIZE.load23" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @sad_ll_memset(i64 %"%tcp_buf.load22", i64 0, i64 %"%TCP_HEADER_SIZE.load23")
  %"%tcp_buf.load24" = load i64, ptr %"%tcp_buf", align 8
  %"%lport.load25" = load i64, ptr %"%lport", align 8
  %"%1573.load" = load i64, ptr %dst_port2, align 8
  %"%isn.load26" = load i64, ptr %"%isn", align 8
  %"%TCP_SYN.load" = load i64, ptr @TCP_SYN, align 8
  %"%TCP_WINDOW_SIZE.load" = load i64, ptr @TCP_WINDOW_SIZE, align 8
  %calltmp27 = call i64 @tcp_build_header(i64 %"%tcp_buf.load24", i64 %"%lport.load25", i64 %"%1573.load", i64 %"%isn.load26", i64 0, i64 %"%TCP_SYN.load", i64 %"%TCP_WINDOW_SIZE.load")
  %"%dst_ip.load" = load i64, ptr %dst_ip1, align 8
  %"%IP_PROTO_TCP.load" = load i64, ptr @IP_PROTO_TCP, align 8
  %"%tcp_buf.load28" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_HEADER_SIZE.load29" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @ip_send(i64 %"%dst_ip.load", i64 %"%IP_PROTO_TCP.load", i64 %"%tcp_buf.load28", i64 %"%TCP_HEADER_SIZE.load29")
  store i64 0, ptr %"%result", align 8
  %"%tcp_buf.load30" = load i64, ptr %"%tcp_buf", align 8
  call void @sad_ll_kfree(i64 %"%tcp_buf.load30")
  %"%result.load" = load i64, ptr %"%result", align 8
  %cmpgttmp = icmp sgt i64 %"%result.load", 0
  br i1 %cmpgttmp, label %then_473, label %else_475

merge_474:                                        ; preds = %then_473
  %loadtmp = load i64, ptr %"%sock", align 8
  ret i64 %loadtmp

then_467:                                         ; preds = %while_body_465
  %"%1531.load" = load i64, ptr %"%i", align 8
  store i64 %"%1531.load", ptr %"%sock", align 8
  %"%TCP_MAX_SOCKETS.load5" = load i64, ptr @TCP_MAX_SOCKETS, align 8
  store i64 %"%TCP_MAX_SOCKETS.load5", ptr %"%i", align 8
  br label %merge_468

then_469:                                         ; preds = %while_exit_466
  %calltmp7 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %calltmp8 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -1

then_471:                                         ; preds = %merge_470
  %"%tcp_errors.load" = load i64, ptr @tcp_errors, align 8
  %addtmp21 = add i64 %"%tcp_errors.load", 1
  store i64 %addtmp21, ptr %"%tcp_errors", align 8
  ret i64 -1

then_473:                                         ; preds = %merge_472
  %"%sock.load31" = load i64, ptr %"%sock", align 8
  %"%TCP_STATE_SYN_SENT.load" = load i64, ptr @TCP_STATE_SYN_SENT, align 8
  %calltmp32 = call i64 @tcp_set_state(i64 %"%sock.load31", i64 %"%TCP_STATE_SYN_SENT.load")
  %"%isn.load33" = load i64, ptr %"%isn", align 8
  %addtmp34 = add i64 %"%isn.load33", 1
  %"%sock.load35" = load i64, ptr %"%sock", align 8
  %calltmp36 = call i64 @tcp_set_seq(i64 %"%sock.load35", i64 %addtmp34)
  %"%tcp_connections.load" = load i64, ptr @tcp_connections, align 8
  %addtmp37 = add i64 %"%tcp_connections.load", 1
  store i64 %addtmp37, ptr %"%tcp_connections", align 8
  %calltmp38 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %"%sock.load39" = load i64, ptr %"%sock", align 8
  %calltmp40 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%sock.load39")
  %calltmp41 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %"%lport.load42" = load i64, ptr %"%lport", align 8
  %calltmp43 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%lport.load42")
  %calltmp44 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_474

while_body_465:                                   ; preds = %while_cond_464
  %"%i.load3" = load i64, ptr %"%i", align 8
  %calltmp = call i64 @tcp_get_state(i64 %"%i.load3")
  store i64 %calltmp, ptr %"%st", align 8
  %"%st.load" = load i64, ptr %"%st", align 8
  %"%TCP_STATE_CLOSED.load" = load i64, ptr @TCP_STATE_CLOSED, align 8
  %cmpeqtmp = icmp eq i64 %"%st.load", %"%TCP_STATE_CLOSED.load"
  br i1 %cmpeqtmp, label %then_467, label %merge_468

while_cond_464:                                   ; preds = %merge_468, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%TCP_MAX_SOCKETS.load" = load i64, ptr @TCP_MAX_SOCKETS, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%TCP_MAX_SOCKETS.load"
  br i1 %cmplttmp, label %while_body_465, label %while_exit_466

while_exit_466:                                   ; preds = %while_cond_464
  %"%sock.load" = load i64, ptr %"%sock", align 8
  %cmplttmp4 = icmp slt i64 %"%sock.load", 0
  br i1 %cmplttmp4, label %then_469, label %merge_470
}

define i64 @tcp_listen(i64 %port) {
entry:
  %"%st" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %port1 = alloca i64, align 8
  store i64 %port, ptr %port1, align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_476

merge_480:                                        ; preds = %while_body_477
  %"%i.load18" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load18", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_476

then_479:                                         ; preds = %while_body_477
  %"%i.load3" = load i64, ptr %"%i", align 8
  %"%1613.load" = load i64, ptr %port1, align 8
  %calltmp4 = call i64 @tcp_set_lport(i64 %"%i.load3", i64 %"%1613.load")
  %"%i.load5" = load i64, ptr %"%i", align 8
  %calltmp6 = call i64 @tcp_set_rport(i64 %"%i.load5", i64 0)
  %"%i.load7" = load i64, ptr %"%i", align 8
  %calltmp8 = call i64 @tcp_set_rip(i64 %"%i.load7", i64 0)
  %"%i.load9" = load i64, ptr %"%i", align 8
  %calltmp10 = call i64 @tcp_set_seq(i64 %"%i.load9", i64 0)
  %"%i.load11" = load i64, ptr %"%i", align 8
  %calltmp12 = call i64 @tcp_set_ack(i64 %"%i.load11", i64 0)
  %"%i.load13" = load i64, ptr %"%i", align 8
  %"%TCP_STATE_LISTEN.load" = load i64, ptr @TCP_STATE_LISTEN, align 8
  %calltmp14 = call i64 @tcp_set_state(i64 %"%i.load13", i64 %"%TCP_STATE_LISTEN.load")
  %calltmp15 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %"%port.load" = load i64, ptr %port1, align 8
  %calltmp16 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%port.load")
  %calltmp17 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr %"%i", align 8
  ret i64 %loadtmp

while_body_477:                                   ; preds = %while_cond_476
  %"%i.load2" = load i64, ptr %"%i", align 8
  %calltmp = call i64 @tcp_get_state(i64 %"%i.load2")
  store i64 %calltmp, ptr %"%st", align 8
  %"%st.load" = load i64, ptr %"%st", align 8
  %"%TCP_STATE_CLOSED.load" = load i64, ptr @TCP_STATE_CLOSED, align 8
  %cmpeqtmp = icmp eq i64 %"%st.load", %"%TCP_STATE_CLOSED.load"
  br i1 %cmpeqtmp, label %then_479, label %merge_480

while_cond_476:                                   ; preds = %merge_480, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%TCP_MAX_SOCKETS.load" = load i64, ptr @TCP_MAX_SOCKETS, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%TCP_MAX_SOCKETS.load"
  br i1 %cmplttmp, label %while_body_477, label %while_exit_478

while_exit_478:                                   ; preds = %while_cond_476
  ret i64 -1
}

define i64 @tcp_send(i64 %sock_id, i64 %data_addr, i64 %data_len) {
entry:
  %"%tcp_tx_count" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%flags" = alloca i64, align 8
  %"%tcp_errors" = alloca i64, align 8
  %"%tcp_buf" = alloca i64, align 8
  %"%tcp_total" = alloca i64, align 8
  %"%ack" = alloca i64, align 8
  %"%seq" = alloca i64, align 8
  %"%dst_port" = alloca i64, align 8
  %"%src_port" = alloca i64, align 8
  %"%dst_ip" = alloca i64, align 8
  %"%state" = alloca i64, align 8
  %data_len3 = alloca i64, align 8
  store i64 %data_len, ptr %data_len3, align 8
  %data_addr2 = alloca i64, align 8
  store i64 %data_addr, ptr %data_addr2, align 8
  %sock_id1 = alloca i64, align 8
  store i64 %sock_id, ptr %sock_id1, align 8
  %"%sock_id.load" = load i64, ptr %sock_id1, align 8
  %cmplttmp = icmp slt i64 %"%sock_id.load", 0
  br i1 %cmplttmp, label %then_481, label %merge_482

else_491:                                         ; preds = %merge_488
  %"%tcp_errors.load37" = load i64, ptr %"%tcp_errors", align 8
  %addtmp38 = add i64 %"%tcp_errors.load37", 1
  store i64 %addtmp38, ptr %"%tcp_errors", align 8
  br label %merge_490

merge_482:                                        ; preds = %entry
  %"%sock_id.load4" = load i64, ptr %sock_id1, align 8
  %"%TCP_MAX_SOCKETS.load" = load i64, ptr @TCP_MAX_SOCKETS, align 8
  %cmpgetmp = icmp sge i64 %"%sock_id.load4", %"%TCP_MAX_SOCKETS.load"
  br i1 %cmpgetmp, label %then_483, label %merge_484

merge_484:                                        ; preds = %merge_482
  %"%sock_id.load5" = load i64, ptr %sock_id1, align 8
  %calltmp = call i64 @tcp_get_state(i64 %"%sock_id.load5")
  store i64 %calltmp, ptr %"%state", align 8
  %"%state.load" = load i64, ptr %"%state", align 8
  %"%TCP_STATE_ESTABLISHED.load" = load i64, ptr @TCP_STATE_ESTABLISHED, align 8
  %cmpnetmp = icmp ne i64 %"%state.load", %"%TCP_STATE_ESTABLISHED.load"
  br i1 %cmpnetmp, label %then_485, label %merge_486

merge_486:                                        ; preds = %merge_484
  %"%sock_id.load8" = load i64, ptr %sock_id1, align 8
  %calltmp9 = call i64 @tcp_get_rip(i64 %"%sock_id.load8")
  store i64 %calltmp9, ptr %"%dst_ip", align 8
  %"%sock_id.load10" = load i64, ptr %sock_id1, align 8
  %calltmp11 = call i64 @tcp_get_lport(i64 %"%sock_id.load10")
  store i64 %calltmp11, ptr %"%src_port", align 8
  %"%sock_id.load12" = load i64, ptr %sock_id1, align 8
  %calltmp13 = call i64 @tcp_get_rport(i64 %"%sock_id.load12")
  store i64 %calltmp13, ptr %"%dst_port", align 8
  %"%sock_id.load14" = load i64, ptr %sock_id1, align 8
  %calltmp15 = call i64 @tcp_get_seq(i64 %"%sock_id.load14")
  store i64 %calltmp15, ptr %"%seq", align 8
  %"%sock_id.load16" = load i64, ptr %sock_id1, align 8
  %calltmp17 = call i64 @tcp_get_ack(i64 %"%sock_id.load16")
  store i64 %calltmp17, ptr %"%ack", align 8
  %"%TCP_HEADER_SIZE.load" = load i64, ptr @TCP_HEADER_SIZE, align 8
  %"%data_len.load" = load i64, ptr %data_len3, align 8
  %addtmp = add i64 %"%TCP_HEADER_SIZE.load", %"%data_len.load"
  store i64 %addtmp, ptr %"%tcp_total", align 8
  %"%tcp_total.load" = load i64, ptr %"%tcp_total", align 8
  call void @sad_ll_kmalloc(i64 %"%tcp_total.load")
  store i64 0, ptr %"%tcp_buf", align 8
  %"%tcp_buf.load" = load i64, ptr %"%tcp_buf", align 8
  %cmpeqtmp = icmp eq i64 %"%tcp_buf.load", 0
  br i1 %cmpeqtmp, label %then_487, label %merge_488

merge_488:                                        ; preds = %merge_486
  %"%tcp_buf.load19" = load i64, ptr %"%tcp_buf", align 8
  %"%tcp_total.load20" = load i64, ptr %"%tcp_total", align 8
  call void @sad_ll_memset(i64 %"%tcp_buf.load19", i64 0, i64 %"%tcp_total.load20")
  %"%TCP_ACK.load" = load i64, ptr @TCP_ACK, align 8
  %"%TCP_PSH.load" = load i64, ptr @TCP_PSH, align 8
  %addtmp21 = add i64 %"%TCP_ACK.load", %"%TCP_PSH.load"
  store i64 %addtmp21, ptr %"%flags", align 8
  %"%tcp_buf.load22" = load i64, ptr %"%tcp_buf", align 8
  %"%src_port.load" = load i64, ptr %"%src_port", align 8
  %"%dst_port.load" = load i64, ptr %"%dst_port", align 8
  %"%seq.load" = load i64, ptr %"%seq", align 8
  %"%ack.load" = load i64, ptr %"%ack", align 8
  %"%flags.load" = load i64, ptr %"%flags", align 8
  %"%TCP_WINDOW_SIZE.load" = load i64, ptr @TCP_WINDOW_SIZE, align 8
  %calltmp23 = call i64 @tcp_build_header(i64 %"%tcp_buf.load22", i64 %"%src_port.load", i64 %"%dst_port.load", i64 %"%seq.load", i64 %"%ack.load", i64 %"%flags.load", i64 %"%TCP_WINDOW_SIZE.load")
  %"%tcp_buf.load24" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_HEADER_SIZE.load25" = load i64, ptr @TCP_HEADER_SIZE, align 8
  %addtmp26 = add i64 %"%tcp_buf.load24", %"%TCP_HEADER_SIZE.load25"
  %"%data_addr.load" = load i64, ptr %data_addr2, align 8
  %"%data_len.load27" = load i64, ptr %data_len3, align 8
  call void @sad_ll_memcpy(i64 %addtmp26, i64 %"%data_addr.load", i64 %"%data_len.load27")
  %"%dst_ip.load" = load i64, ptr %"%dst_ip", align 8
  %"%IP_PROTO_TCP.load" = load i64, ptr @IP_PROTO_TCP, align 8
  %"%tcp_buf.load28" = load i64, ptr %"%tcp_buf", align 8
  %"%tcp_total.load29" = load i64, ptr %"%tcp_total", align 8
  call void @ip_send(i64 %"%dst_ip.load", i64 %"%IP_PROTO_TCP.load", i64 %"%tcp_buf.load28", i64 %"%tcp_total.load29")
  store i64 0, ptr %"%result", align 8
  %"%tcp_buf.load30" = load i64, ptr %"%tcp_buf", align 8
  call void @sad_ll_kfree(i64 %"%tcp_buf.load30")
  %"%result.load" = load i64, ptr %"%result", align 8
  %cmpgttmp = icmp sgt i64 %"%result.load", 0
  br i1 %cmpgttmp, label %then_489, label %else_491

merge_490:                                        ; preds = %else_491, %then_489
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

then_481:                                         ; preds = %entry
  ret i64 0

then_483:                                         ; preds = %merge_482
  ret i64 0

then_485:                                         ; preds = %merge_484
  %calltmp6 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  %calltmp7 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_487:                                         ; preds = %merge_486
  %"%tcp_errors.load" = load i64, ptr @tcp_errors, align 8
  %addtmp18 = add i64 %"%tcp_errors.load", 1
  store i64 %addtmp18, ptr %"%tcp_errors", align 8
  ret i64 0

then_489:                                         ; preds = %merge_488
  %"%seq.load31" = load i64, ptr %"%seq", align 8
  %"%data_len.load32" = load i64, ptr %data_len3, align 8
  %addtmp33 = add i64 %"%seq.load31", %"%data_len.load32"
  %"%sock_id.load34" = load i64, ptr %sock_id1, align 8
  %calltmp35 = call i64 @tcp_set_seq(i64 %"%sock_id.load34", i64 %addtmp33)
  %"%tcp_tx_count.load" = load i64, ptr @tcp_tx_count, align 8
  %addtmp36 = add i64 %"%tcp_tx_count.load", 1
  store i64 %addtmp36, ptr %"%tcp_tx_count", align 8
  br label %merge_490
}

define i64 @tcp_close(i64 %sock_id) {
entry:
  %"%flags" = alloca i64, align 8
  %"%tcp_buf" = alloca i64, align 8
  %"%ack" = alloca i64, align 8
  %"%seq" = alloca i64, align 8
  %"%dst_port" = alloca i64, align 8
  %"%src_port" = alloca i64, align 8
  %"%dst_ip" = alloca i64, align 8
  %"%state" = alloca i64, align 8
  %sock_id1 = alloca i64, align 8
  store i64 %sock_id, ptr %sock_id1, align 8
  %"%sock_id.load" = load i64, ptr %sock_id1, align 8
  %cmplttmp = icmp slt i64 %"%sock_id.load", 0
  br i1 %cmplttmp, label %then_492, label %merge_493

else_500:                                         ; preds = %merge_497
  %"%sock_id.load16" = load i64, ptr %sock_id1, align 8
  %"%TCP_STATE_CLOSED.load17" = load i64, ptr @TCP_STATE_CLOSED, align 8
  %calltmp18 = call i64 @tcp_set_state(i64 %"%sock_id.load16", i64 %"%TCP_STATE_CLOSED.load17")
  br label %merge_499

merge_493:                                        ; preds = %entry
  %"%sock_id.load2" = load i64, ptr %sock_id1, align 8
  %"%TCP_MAX_SOCKETS.load" = load i64, ptr @TCP_MAX_SOCKETS, align 8
  %cmpgetmp = icmp sge i64 %"%sock_id.load2", %"%TCP_MAX_SOCKETS.load"
  br i1 %cmpgetmp, label %then_494, label %merge_495

merge_495:                                        ; preds = %merge_493
  %"%sock_id.load3" = load i64, ptr %sock_id1, align 8
  %calltmp = call i64 @tcp_get_state(i64 %"%sock_id.load3")
  store i64 %calltmp, ptr %"%state", align 8
  %"%state.load" = load i64, ptr %"%state", align 8
  %"%TCP_STATE_CLOSED.load" = load i64, ptr @TCP_STATE_CLOSED, align 8
  %cmpeqtmp = icmp eq i64 %"%state.load", %"%TCP_STATE_CLOSED.load"
  br i1 %cmpeqtmp, label %then_496, label %merge_497

merge_497:                                        ; preds = %merge_495
  %"%state.load4" = load i64, ptr %"%state", align 8
  %"%TCP_STATE_ESTABLISHED.load" = load i64, ptr @TCP_STATE_ESTABLISHED, align 8
  %cmpeqtmp5 = icmp eq i64 %"%state.load4", %"%TCP_STATE_ESTABLISHED.load"
  br i1 %cmpeqtmp5, label %then_498, label %else_500

merge_499:                                        ; preds = %merge_502, %else_500
  ret i64 1

merge_502:                                        ; preds = %then_501, %then_498
  %"%sock_id.load26" = load i64, ptr %sock_id1, align 8
  %"%TCP_STATE_FIN_WAIT1.load" = load i64, ptr @TCP_STATE_FIN_WAIT1, align 8
  %calltmp27 = call i64 @tcp_set_state(i64 %"%sock_id.load26", i64 %"%TCP_STATE_FIN_WAIT1.load")
  %calltmp28 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%sock_id.load29" = load i64, ptr %sock_id1, align 8
  %calltmp30 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%sock_id.load29")
  %calltmp31 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_499

then_492:                                         ; preds = %entry
  ret i64 0

then_494:                                         ; preds = %merge_493
  ret i64 0

then_496:                                         ; preds = %merge_495
  ret i64 1

then_498:                                         ; preds = %merge_497
  %"%sock_id.load6" = load i64, ptr %sock_id1, align 8
  %calltmp7 = call i64 @tcp_get_rip(i64 %"%sock_id.load6")
  store i64 %calltmp7, ptr %"%dst_ip", align 8
  %"%sock_id.load8" = load i64, ptr %sock_id1, align 8
  %calltmp9 = call i64 @tcp_get_lport(i64 %"%sock_id.load8")
  store i64 %calltmp9, ptr %"%src_port", align 8
  %"%sock_id.load10" = load i64, ptr %sock_id1, align 8
  %calltmp11 = call i64 @tcp_get_rport(i64 %"%sock_id.load10")
  store i64 %calltmp11, ptr %"%dst_port", align 8
  %"%sock_id.load12" = load i64, ptr %sock_id1, align 8
  %calltmp13 = call i64 @tcp_get_seq(i64 %"%sock_id.load12")
  store i64 %calltmp13, ptr %"%seq", align 8
  %"%sock_id.load14" = load i64, ptr %sock_id1, align 8
  %calltmp15 = call i64 @tcp_get_ack(i64 %"%sock_id.load14")
  store i64 %calltmp15, ptr %"%ack", align 8
  %"%TCP_HEADER_SIZE.load" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @sad_ll_kmalloc(i64 %"%TCP_HEADER_SIZE.load")
  store i64 0, ptr %"%tcp_buf", align 8
  %"%tcp_buf.load" = load i64, ptr %"%tcp_buf", align 8
  %cmpgttmp = icmp sgt i64 %"%tcp_buf.load", 0
  br i1 %cmpgttmp, label %then_501, label %merge_502

then_501:                                         ; preds = %then_498
  %"%tcp_buf.load19" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_HEADER_SIZE.load20" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @sad_ll_memset(i64 %"%tcp_buf.load19", i64 0, i64 %"%TCP_HEADER_SIZE.load20")
  %"%TCP_FIN.load" = load i64, ptr @TCP_FIN, align 8
  %"%TCP_ACK.load" = load i64, ptr @TCP_ACK, align 8
  %addtmp = add i64 %"%TCP_FIN.load", %"%TCP_ACK.load"
  store i64 %addtmp, ptr %"%flags", align 8
  %"%tcp_buf.load21" = load i64, ptr %"%tcp_buf", align 8
  %"%src_port.load" = load i64, ptr %"%src_port", align 8
  %"%dst_port.load" = load i64, ptr %"%dst_port", align 8
  %"%seq.load" = load i64, ptr %"%seq", align 8
  %"%ack.load" = load i64, ptr %"%ack", align 8
  %"%flags.load" = load i64, ptr %"%flags", align 8
  %"%TCP_WINDOW_SIZE.load" = load i64, ptr @TCP_WINDOW_SIZE, align 8
  %calltmp22 = call i64 @tcp_build_header(i64 %"%tcp_buf.load21", i64 %"%src_port.load", i64 %"%dst_port.load", i64 %"%seq.load", i64 %"%ack.load", i64 %"%flags.load", i64 %"%TCP_WINDOW_SIZE.load")
  %"%dst_ip.load" = load i64, ptr %"%dst_ip", align 8
  %"%IP_PROTO_TCP.load" = load i64, ptr @IP_PROTO_TCP, align 8
  %"%tcp_buf.load23" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_HEADER_SIZE.load24" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @ip_send(i64 %"%dst_ip.load", i64 %"%IP_PROTO_TCP.load", i64 %"%tcp_buf.load23", i64 %"%TCP_HEADER_SIZE.load24")
  %"%tcp_buf.load25" = load i64, ptr %"%tcp_buf", align 8
  call void @sad_ll_kfree(i64 %"%tcp_buf.load25")
  br label %merge_502
}

define i64 @tcp_handle_packet(i64 %payload, i64 %payload_len, i64 %src_ip) {
entry:
  %"%lport" = alloca i64, align 8
  %"%st" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %"%tcp_rx_count" = alloca i64, align 8
  %"%data_len" = alloca i64, align 8
  %"%data_offset" = alloca i64, align 8
  %"%flags" = alloca i64, align 8
  %"%ack" = alloca i64, align 8
  %"%seq" = alloca i64, align 8
  %"%dst_port" = alloca i64, align 8
  %"%src_port" = alloca i64, align 8
  %"%tcp_errors" = alloca i64, align 8
  %src_ip3 = alloca i64, align 8
  store i64 %src_ip, ptr %src_ip3, align 8
  %payload_len2 = alloca i64, align 8
  store i64 %payload_len, ptr %payload_len2, align 8
  %payload1 = alloca i64, align 8
  store i64 %payload, ptr %payload1, align 8
  %"%payload_len.load" = load i64, ptr %payload_len2, align 8
  %"%TCP_HEADER_SIZE.load" = load i64, ptr @TCP_HEADER_SIZE, align 8
  %cmplttmp = icmp slt i64 %"%payload_len.load", %"%TCP_HEADER_SIZE.load"
  br i1 %cmplttmp, label %then_503, label %merge_504

merge_504:                                        ; preds = %entry
  %"%payload.load" = load i64, ptr %payload1, align 8
  %calltmp5 = call i64 @tcp_parse_src_port(i64 %"%payload.load")
  store i64 %calltmp5, ptr %"%src_port", align 8
  %"%payload.load6" = load i64, ptr %payload1, align 8
  %calltmp7 = call i64 @tcp_parse_dst_port(i64 %"%payload.load6")
  store i64 %calltmp7, ptr %"%dst_port", align 8
  %"%payload.load8" = load i64, ptr %payload1, align 8
  %calltmp9 = call i64 @tcp_parse_seq(i64 %"%payload.load8")
  store i64 %calltmp9, ptr %"%seq", align 8
  %"%payload.load10" = load i64, ptr %payload1, align 8
  %calltmp11 = call i64 @tcp_parse_ack(i64 %"%payload.load10")
  store i64 %calltmp11, ptr %"%ack", align 8
  %"%payload.load12" = load i64, ptr %payload1, align 8
  %calltmp13 = call i64 @tcp_parse_flags(i64 %"%payload.load12")
  store i64 %calltmp13, ptr %"%flags", align 8
  %"%payload.load14" = load i64, ptr %payload1, align 8
  %calltmp15 = call i64 @tcp_parse_data_offset(i64 %"%payload.load14")
  store i64 %calltmp15, ptr %"%data_offset", align 8
  %"%payload_len.load16" = load i64, ptr %payload_len2, align 8
  %"%data_offset.load" = load i64, ptr %"%data_offset", align 8
  %subtmp = sub i64 %"%payload_len.load16", %"%data_offset.load"
  store i64 %subtmp, ptr %"%data_len", align 8
  %"%tcp_rx_count.load" = load i64, ptr @tcp_rx_count, align 8
  %addtmp17 = add i64 %"%tcp_rx_count.load", 1
  store i64 %addtmp17, ptr %"%tcp_rx_count", align 8
  %calltmp18 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %"%dst_port.load" = load i64, ptr %"%dst_port", align 8
  %calltmp19 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%dst_port.load")
  %calltmp20 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %"%flags.load" = load i64, ptr %"%flags", align 8
  %calltmp21 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%flags.load")
  %calltmp22 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%i", align 8
  br label %while_cond_505

merge_509:                                        ; preds = %merge_511, %while_body_506
  %"%i.load33" = load i64, ptr %"%i", align 8
  %addtmp34 = add i64 %"%i.load33", 1
  store i64 %addtmp34, ptr %"%i", align 8
  br label %while_cond_505

merge_511:                                        ; preds = %then_508
  br label %merge_509

then_503:                                         ; preds = %entry
  %calltmp = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  %calltmp4 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  %"%tcp_errors.load" = load i64, ptr @tcp_errors, align 8
  %addtmp = add i64 %"%tcp_errors.load", 1
  store i64 %addtmp, ptr %"%tcp_errors", align 8
  ret i64 0

then_508:                                         ; preds = %while_body_506
  %"%i.load30" = load i64, ptr %"%i", align 8
  %calltmp31 = call i64 @tcp_get_lport(i64 %"%i.load30")
  store i64 %calltmp31, ptr %"%lport", align 8
  %"%lport.load" = load i64, ptr %"%lport", align 8
  %"%dst_port.load32" = load i64, ptr %"%dst_port", align 8
  %cmpeqtmp = icmp eq i64 %"%lport.load", %"%dst_port.load32"
  br i1 %cmpeqtmp, label %then_510, label %merge_511

then_510:                                         ; preds = %then_508
  %"%payload.load35" = load i64, ptr %payload1, align 8
  %"%data_offset.load36" = load i64, ptr %"%data_offset", align 8
  %addtmp37 = add i64 %"%payload.load35", %"%data_offset.load36"
  %"%i.load38" = load i64, ptr %"%i", align 8
  %"%src_ip.load" = load i64, ptr %src_ip3, align 8
  %"%src_port.load" = load i64, ptr %"%src_port", align 8
  %"%seq.load" = load i64, ptr %"%seq", align 8
  %"%ack.load" = load i64, ptr %"%ack", align 8
  %"%flags.load39" = load i64, ptr %"%flags", align 8
  %"%data_len.load" = load i64, ptr %"%data_len", align 8
  %calltmp40 = call i64 @tcp_process_segment(i64 %"%i.load38", i64 %"%src_ip.load", i64 %"%src_port.load", i64 %"%seq.load", i64 %"%ack.load", i64 %"%flags.load39", i64 %addtmp37, i64 %"%data_len.load")
  ret i64 %calltmp40

while_body_506:                                   ; preds = %while_cond_505
  %"%i.load24" = load i64, ptr %"%i", align 8
  %calltmp25 = call i64 @tcp_get_state(i64 %"%i.load24")
  store i64 %calltmp25, ptr %"%st", align 8
  %"%st.load" = load i64, ptr %"%st", align 8
  %"%TCP_STATE_CLOSED.load" = load i64, ptr @TCP_STATE_CLOSED, align 8
  %cmpnetmp = icmp ne i64 %"%st.load", %"%TCP_STATE_CLOSED.load"
  br i1 %cmpnetmp, label %then_508, label %merge_509

while_cond_505:                                   ; preds = %merge_509, %merge_504
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%TCP_MAX_SOCKETS.load" = load i64, ptr @TCP_MAX_SOCKETS, align 8
  %cmplttmp23 = icmp slt i64 %"%i.load", %"%TCP_MAX_SOCKETS.load"
  br i1 %cmplttmp23, label %while_body_506, label %while_exit_507

while_exit_507:                                   ; preds = %while_cond_505
  %calltmp26 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  %"%dst_port.load27" = load i64, ptr %"%dst_port", align 8
  %calltmp28 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%dst_port.load27")
  %calltmp29 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @tcp_process_segment(i64 %sock, i64 %src_ip, i64 %src_port, i64 %seq, i64 %ack_num, i64 %flags, i64 %data_addr, i64 %data_len) {
entry:
  %"%fin_flags" = alloca i64, align 8
  %"%tcp_buf247" = alloca i64, align 8
  %"%has_fin210" = alloca i64, align 8
  %"%has_ack197" = alloca i64, align 8
  %"%has_fin" = alloca i64, align 8
  %"%has_ack" = alloca i64, align 8
  %"%has_syn" = alloca i64, align 8
  %"%syn_ack" = alloca i64, align 8
  %"%tcp_buf" = alloca i64, align 8
  %"%isn" = alloca i64, align 8
  %"%is_syn" = alloca i64, align 8
  %"%state" = alloca i64, align 8
  %data_len8 = alloca i64, align 8
  store i64 %data_len, ptr %data_len8, align 8
  %data_addr7 = alloca i64, align 8
  store i64 %data_addr, ptr %data_addr7, align 8
  %flags6 = alloca i64, align 8
  store i64 %flags, ptr %flags6, align 8
  %ack_num5 = alloca i64, align 8
  store i64 %ack_num, ptr %ack_num5, align 8
  %seq4 = alloca i64, align 8
  store i64 %seq, ptr %seq4, align 8
  %src_port3 = alloca i64, align 8
  store i64 %src_port, ptr %src_port3, align 8
  %src_ip2 = alloca i64, align 8
  store i64 %src_ip, ptr %src_ip2, align 8
  %sock1 = alloca i64, align 8
  store i64 %sock, ptr %sock1, align 8
  %"%sock.load" = load i64, ptr %sock1, align 8
  %calltmp = call i64 @tcp_get_state(i64 %"%sock.load")
  store i64 %calltmp, ptr %"%state", align 8
  %"%state.load" = load i64, ptr %"%state", align 8
  %"%TCP_STATE_LISTEN.load" = load i64, ptr @TCP_STATE_LISTEN, align 8
  %cmpeqtmp = icmp eq i64 %"%state.load", %"%TCP_STATE_LISTEN.load"
  br i1 %cmpeqtmp, label %then_512, label %merge_513

merge_513:                                        ; preds = %entry
  %"%state.load11" = load i64, ptr %"%state", align 8
  %"%TCP_STATE_SYN_SENT.load" = load i64, ptr @TCP_STATE_SYN_SENT, align 8
  %cmpeqtmp12 = icmp eq i64 %"%state.load11", %"%TCP_STATE_SYN_SENT.load"
  br i1 %cmpeqtmp12, label %then_518, label %merge_519

merge_515:                                        ; preds = %then_512
  ret i64 0

merge_517:                                        ; preds = %then_516, %then_514
  %"%isn.load35" = load i64, ptr %"%isn", align 8
  %addtmp36 = add i64 %"%isn.load35", 1
  %"%sock.load37" = load i64, ptr %sock1, align 8
  %calltmp38 = call i64 @tcp_set_seq(i64 %"%sock.load37", i64 %addtmp36)
  %"%sock.load39" = load i64, ptr %sock1, align 8
  %"%TCP_STATE_SYN_RCVD.load" = load i64, ptr @TCP_STATE_SYN_RCVD, align 8
  %calltmp40 = call i64 @tcp_set_state(i64 %"%sock.load39", i64 %"%TCP_STATE_SYN_RCVD.load")
  %calltmp41 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  %calltmp42 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1

merge_519:                                        ; preds = %merge_513
  %"%state.load59" = load i64, ptr %"%state", align 8
  %"%TCP_STATE_SYN_RCVD.load60" = load i64, ptr @TCP_STATE_SYN_RCVD, align 8
  %cmpeqtmp61 = icmp eq i64 %"%state.load59", %"%TCP_STATE_SYN_RCVD.load60"
  br i1 %cmpeqtmp61, label %then_526, label %merge_527

merge_521:                                        ; preds = %merge_523, %then_518
  ret i64 0

merge_523:                                        ; preds = %then_520
  br label %merge_521

merge_525:                                        ; preds = %then_524, %then_522
  %"%sock.load91" = load i64, ptr %sock1, align 8
  %"%TCP_STATE_ESTABLISHED.load" = load i64, ptr @TCP_STATE_ESTABLISHED, align 8
  %calltmp92 = call i64 @tcp_set_state(i64 %"%sock.load91", i64 %"%TCP_STATE_ESTABLISHED.load")
  %calltmp93 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  %"%sock.load94" = load i64, ptr %sock1, align 8
  %calltmp95 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%sock.load94")
  %calltmp96 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1

merge_527:                                        ; preds = %merge_519
  %"%state.load107" = load i64, ptr %"%state", align 8
  %"%TCP_STATE_ESTABLISHED.load108" = load i64, ptr @TCP_STATE_ESTABLISHED, align 8
  %cmpeqtmp109 = icmp eq i64 %"%state.load107", %"%TCP_STATE_ESTABLISHED.load108"
  br i1 %cmpeqtmp109, label %then_530, label %merge_531

merge_529:                                        ; preds = %then_526
  ret i64 0

merge_531:                                        ; preds = %merge_527
  %"%state.load121" = load i64, ptr %"%state", align 8
  %"%TCP_STATE_FIN_WAIT1.load" = load i64, ptr @TCP_STATE_FIN_WAIT1, align 8
  %cmpeqtmp122 = icmp eq i64 %"%state.load121", %"%TCP_STATE_FIN_WAIT1.load"
  br i1 %cmpeqtmp122, label %then_540, label %merge_541

merge_533:                                        ; preds = %then_530
  %"%data_len.load" = load i64, ptr %data_len8, align 8
  %cmpgttmp130 = icmp sgt i64 %"%data_len.load", 0
  br i1 %cmpgttmp130, label %then_536, label %merge_537

merge_535:                                        ; preds = %then_534, %then_532
  %"%sock.load151" = load i64, ptr %sock1, align 8
  %"%TCP_STATE_CLOSE_WAIT.load" = load i64, ptr @TCP_STATE_CLOSE_WAIT, align 8
  %calltmp152 = call i64 @tcp_set_state(i64 %"%sock.load151", i64 %"%TCP_STATE_CLOSE_WAIT.load")
  %calltmp153 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  %"%sock.load154" = load i64, ptr %sock1, align 8
  %calltmp155 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%sock.load154")
  %calltmp156 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1

merge_537:                                        ; preds = %merge_539, %merge_533
  ret i64 1

merge_539:                                        ; preds = %then_538, %then_536
  %calltmp186 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %"%data_len.load187" = load i64, ptr %data_len8, align 8
  %calltmp188 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%data_len.load187")
  %calltmp189 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_537

merge_541:                                        ; preds = %merge_531
  %"%state.load201" = load i64, ptr %"%state", align 8
  %"%TCP_STATE_FIN_WAIT2.load" = load i64, ptr @TCP_STATE_FIN_WAIT2, align 8
  %cmpeqtmp202 = icmp eq i64 %"%state.load201", %"%TCP_STATE_FIN_WAIT2.load"
  br i1 %cmpeqtmp202, label %then_544, label %merge_545

merge_543:                                        ; preds = %then_542, %then_540
  ret i64 1

merge_545:                                        ; preds = %merge_541
  %"%state.load214" = load i64, ptr %"%state", align 8
  %"%TCP_STATE_CLOSE_WAIT.load215" = load i64, ptr @TCP_STATE_CLOSE_WAIT, align 8
  %cmpeqtmp216 = icmp eq i64 %"%state.load214", %"%TCP_STATE_CLOSE_WAIT.load215"
  br i1 %cmpeqtmp216, label %then_550, label %merge_551

merge_547:                                        ; preds = %merge_549, %then_544
  ret i64 1

merge_549:                                        ; preds = %then_548, %then_546
  %"%sock.load244" = load i64, ptr %sock1, align 8
  %"%TCP_STATE_CLOSED.load" = load i64, ptr @TCP_STATE_CLOSED, align 8
  %calltmp245 = call i64 @tcp_set_state(i64 %"%sock.load244", i64 %"%TCP_STATE_CLOSED.load")
  br label %merge_547

merge_551:                                        ; preds = %merge_545
  %"%state.load250" = load i64, ptr %"%state", align 8
  %"%TCP_STATE_LAST_ACK.load" = load i64, ptr @TCP_STATE_LAST_ACK, align 8
  %cmpeqtmp251 = icmp eq i64 %"%state.load250", %"%TCP_STATE_LAST_ACK.load"
  br i1 %cmpeqtmp251, label %then_554, label %merge_555

merge_553:                                        ; preds = %then_552, %then_550
  %"%sock.load274" = load i64, ptr %sock1, align 8
  %"%TCP_STATE_LAST_ACK.load275" = load i64, ptr @TCP_STATE_LAST_ACK, align 8
  %calltmp276 = call i64 @tcp_set_state(i64 %"%sock.load274", i64 %"%TCP_STATE_LAST_ACK.load275")
  ret i64 1

merge_555:                                        ; preds = %merge_551
  ret i64 0

then_512:                                         ; preds = %entry
  %"%TCP_SYN.load" = load i64, ptr @TCP_SYN, align 8
  %shltmp = shl i64 %"%TCP_SYN.load", 1
  %"%flags.load" = load i64, ptr %flags6, align 8
  %divtmp = sdiv i64 %"%flags.load", %shltmp
  %multmp = mul i64 %divtmp, %shltmp
  %"%flags.load9" = load i64, ptr %flags6, align 8
  %subtmp = sub i64 %"%flags.load9", %multmp
  store i64 %subtmp, ptr %"%is_syn", align 8
  %"%is_syn.load" = load i64, ptr %"%is_syn", align 8
  %"%TCP_SYN.load10" = load i64, ptr @TCP_SYN, align 8
  %cmpgetmp = icmp sge i64 %"%is_syn.load", %"%TCP_SYN.load10"
  br i1 %cmpgetmp, label %then_514, label %merge_515

then_514:                                         ; preds = %then_512
  %"%sock.load13" = load i64, ptr %sock1, align 8
  %"%1809.load" = load i64, ptr %src_ip2, align 8
  %calltmp14 = call i64 @tcp_set_rip(i64 %"%sock.load13", i64 %"%1809.load")
  %"%sock.load15" = load i64, ptr %sock1, align 8
  %"%1812.load" = load i64, ptr %src_port3, align 8
  %calltmp16 = call i64 @tcp_set_rport(i64 %"%sock.load15", i64 %"%1812.load")
  %"%seq.load" = load i64, ptr %seq4, align 8
  %addtmp = add i64 %"%seq.load", 1
  %"%sock.load17" = load i64, ptr %sock1, align 8
  %calltmp18 = call i64 @tcp_set_ack(i64 %"%sock.load17", i64 %addtmp)
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%isn", align 8
  %"%sock.load19" = load i64, ptr %sock1, align 8
  %"%isn.load" = load i64, ptr %"%isn", align 8
  %calltmp20 = call i64 @tcp_set_seq(i64 %"%sock.load19", i64 %"%isn.load")
  %"%TCP_HEADER_SIZE.load" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @sad_ll_kmalloc(i64 %"%TCP_HEADER_SIZE.load")
  store i64 0, ptr %"%tcp_buf", align 8
  %"%tcp_buf.load" = load i64, ptr %"%tcp_buf", align 8
  %cmpgttmp = icmp sgt i64 %"%tcp_buf.load", 0
  br i1 %cmpgttmp, label %then_516, label %merge_517

then_516:                                         ; preds = %then_514
  %"%tcp_buf.load21" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_HEADER_SIZE.load22" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @sad_ll_memset(i64 %"%tcp_buf.load21", i64 0, i64 %"%TCP_HEADER_SIZE.load22")
  %"%TCP_SYN.load23" = load i64, ptr @TCP_SYN, align 8
  %"%TCP_ACK.load" = load i64, ptr @TCP_ACK, align 8
  %addtmp24 = add i64 %"%TCP_SYN.load23", %"%TCP_ACK.load"
  store i64 %addtmp24, ptr %"%syn_ack", align 8
  %"%sock.load25" = load i64, ptr %sock1, align 8
  %calltmp26 = call i64 @tcp_get_lport(i64 %"%sock.load25")
  %"%seq.load27" = load i64, ptr %seq4, align 8
  %addtmp28 = add i64 %"%seq.load27", 1
  %"%tcp_buf.load29" = load i64, ptr %"%tcp_buf", align 8
  %"%src_port.load" = load i64, ptr %src_port3, align 8
  %"%isn.load30" = load i64, ptr %"%isn", align 8
  %"%syn_ack.load" = load i64, ptr %"%syn_ack", align 8
  %"%TCP_WINDOW_SIZE.load" = load i64, ptr @TCP_WINDOW_SIZE, align 8
  %calltmp31 = call i64 @tcp_build_header(i64 %"%tcp_buf.load29", i64 %calltmp26, i64 %"%src_port.load", i64 %"%isn.load30", i64 %addtmp28, i64 %"%syn_ack.load", i64 %"%TCP_WINDOW_SIZE.load")
  %"%src_ip.load" = load i64, ptr %src_ip2, align 8
  %"%IP_PROTO_TCP.load" = load i64, ptr @IP_PROTO_TCP, align 8
  %"%tcp_buf.load32" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_HEADER_SIZE.load33" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @ip_send(i64 %"%src_ip.load", i64 %"%IP_PROTO_TCP.load", i64 %"%tcp_buf.load32", i64 %"%TCP_HEADER_SIZE.load33")
  %"%tcp_buf.load34" = load i64, ptr %"%tcp_buf", align 8
  call void @sad_ll_kfree(i64 %"%tcp_buf.load34")
  br label %merge_517

then_518:                                         ; preds = %merge_513
  %"%TCP_SYN.load43" = load i64, ptr @TCP_SYN, align 8
  %shltmp44 = shl i64 %"%TCP_SYN.load43", 1
  %"%flags.load45" = load i64, ptr %flags6, align 8
  %divtmp46 = sdiv i64 %"%flags.load45", %shltmp44
  %multmp47 = mul i64 %divtmp46, %shltmp44
  %"%flags.load48" = load i64, ptr %flags6, align 8
  %subtmp49 = sub i64 %"%flags.load48", %multmp47
  store i64 %subtmp49, ptr %"%has_syn", align 8
  %"%TCP_ACK.load50" = load i64, ptr @TCP_ACK, align 8
  %shltmp51 = shl i64 %"%TCP_ACK.load50", 1
  %"%flags.load52" = load i64, ptr %flags6, align 8
  %divtmp53 = sdiv i64 %"%flags.load52", %shltmp51
  %multmp54 = mul i64 %divtmp53, %shltmp51
  %"%flags.load55" = load i64, ptr %flags6, align 8
  %subtmp56 = sub i64 %"%flags.load55", %multmp54
  store i64 %subtmp56, ptr %"%has_ack", align 8
  %"%has_syn.load" = load i64, ptr %"%has_syn", align 8
  %"%TCP_SYN.load57" = load i64, ptr @TCP_SYN, align 8
  %cmpgetmp58 = icmp sge i64 %"%has_syn.load", %"%TCP_SYN.load57"
  br i1 %cmpgetmp58, label %then_520, label %merge_521

then_520:                                         ; preds = %then_518
  %"%has_ack.load" = load i64, ptr %"%has_ack", align 8
  %"%TCP_ACK.load62" = load i64, ptr @TCP_ACK, align 8
  %cmpgetmp63 = icmp sge i64 %"%has_ack.load", %"%TCP_ACK.load62"
  br i1 %cmpgetmp63, label %then_522, label %merge_523

then_522:                                         ; preds = %then_520
  %"%seq.load64" = load i64, ptr %seq4, align 8
  %addtmp65 = add i64 %"%seq.load64", 1
  %"%sock.load66" = load i64, ptr %sock1, align 8
  %calltmp67 = call i64 @tcp_set_ack(i64 %"%sock.load66", i64 %addtmp65)
  %"%TCP_HEADER_SIZE.load68" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @sad_ll_kmalloc(i64 %"%TCP_HEADER_SIZE.load68")
  store i64 0, ptr %"%tcp_buf", align 8
  %"%tcp_buf.load69" = load i64, ptr %"%tcp_buf", align 8
  %cmpgttmp70 = icmp sgt i64 %"%tcp_buf.load69", 0
  br i1 %cmpgttmp70, label %then_524, label %merge_525

then_524:                                         ; preds = %then_522
  %"%tcp_buf.load71" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_HEADER_SIZE.load72" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @sad_ll_memset(i64 %"%tcp_buf.load71", i64 0, i64 %"%TCP_HEADER_SIZE.load72")
  %"%sock.load73" = load i64, ptr %sock1, align 8
  %calltmp74 = call i64 @tcp_get_lport(i64 %"%sock.load73")
  %"%sock.load75" = load i64, ptr %sock1, align 8
  %calltmp76 = call i64 @tcp_get_rport(i64 %"%sock.load75")
  %"%sock.load77" = load i64, ptr %sock1, align 8
  %calltmp78 = call i64 @tcp_get_seq(i64 %"%sock.load77")
  %"%seq.load79" = load i64, ptr %seq4, align 8
  %addtmp80 = add i64 %"%seq.load79", 1
  %"%tcp_buf.load81" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_ACK.load82" = load i64, ptr @TCP_ACK, align 8
  %"%TCP_WINDOW_SIZE.load83" = load i64, ptr @TCP_WINDOW_SIZE, align 8
  %calltmp84 = call i64 @tcp_build_header(i64 %"%tcp_buf.load81", i64 %calltmp74, i64 %calltmp76, i64 %calltmp78, i64 %addtmp80, i64 %"%TCP_ACK.load82", i64 %"%TCP_WINDOW_SIZE.load83")
  %"%sock.load85" = load i64, ptr %sock1, align 8
  %calltmp86 = call i64 @tcp_get_rip(i64 %"%sock.load85")
  %"%IP_PROTO_TCP.load87" = load i64, ptr @IP_PROTO_TCP, align 8
  %"%tcp_buf.load88" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_HEADER_SIZE.load89" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @ip_send(i64 %calltmp86, i64 %"%IP_PROTO_TCP.load87", i64 %"%tcp_buf.load88", i64 %"%TCP_HEADER_SIZE.load89")
  %"%tcp_buf.load90" = load i64, ptr %"%tcp_buf", align 8
  call void @sad_ll_kfree(i64 %"%tcp_buf.load90")
  br label %merge_525

then_526:                                         ; preds = %merge_519
  %"%TCP_ACK.load97" = load i64, ptr @TCP_ACK, align 8
  %shltmp98 = shl i64 %"%TCP_ACK.load97", 1
  %"%flags.load99" = load i64, ptr %flags6, align 8
  %divtmp100 = sdiv i64 %"%flags.load99", %shltmp98
  %multmp101 = mul i64 %divtmp100, %shltmp98
  %"%flags.load102" = load i64, ptr %flags6, align 8
  %subtmp103 = sub i64 %"%flags.load102", %multmp101
  store i64 %subtmp103, ptr %"%has_ack", align 8
  %"%has_ack.load104" = load i64, ptr %"%has_ack", align 8
  %"%TCP_ACK.load105" = load i64, ptr @TCP_ACK, align 8
  %cmpgetmp106 = icmp sge i64 %"%has_ack.load104", %"%TCP_ACK.load105"
  br i1 %cmpgetmp106, label %then_528, label %merge_529

then_528:                                         ; preds = %then_526
  %"%sock.load110" = load i64, ptr %sock1, align 8
  %"%TCP_STATE_ESTABLISHED.load111" = load i64, ptr @TCP_STATE_ESTABLISHED, align 8
  %calltmp112 = call i64 @tcp_set_state(i64 %"%sock.load110", i64 %"%TCP_STATE_ESTABLISHED.load111")
  %calltmp113 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  %"%sock.load114" = load i64, ptr %sock1, align 8
  %calltmp115 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%sock.load114")
  %calltmp116 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1

then_530:                                         ; preds = %merge_527
  %"%TCP_FIN.load" = load i64, ptr @TCP_FIN, align 8
  %shltmp117 = shl i64 %"%TCP_FIN.load", 1
  %"%flags.load118" = load i64, ptr %flags6, align 8
  %modtmp = srem i64 %"%flags.load118", %shltmp117
  store i64 %modtmp, ptr %"%has_fin", align 8
  %"%has_fin.load" = load i64, ptr %"%has_fin", align 8
  %"%TCP_FIN.load119" = load i64, ptr @TCP_FIN, align 8
  %cmpgetmp120 = icmp sge i64 %"%has_fin.load", %"%TCP_FIN.load119"
  br i1 %cmpgetmp120, label %then_532, label %merge_533

then_532:                                         ; preds = %then_530
  %"%seq.load123" = load i64, ptr %seq4, align 8
  %addtmp124 = add i64 %"%seq.load123", 1
  %"%sock.load125" = load i64, ptr %sock1, align 8
  %calltmp126 = call i64 @tcp_set_ack(i64 %"%sock.load125", i64 %addtmp124)
  %"%TCP_HEADER_SIZE.load127" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @sad_ll_kmalloc(i64 %"%TCP_HEADER_SIZE.load127")
  store i64 0, ptr %"%tcp_buf", align 8
  %"%tcp_buf.load128" = load i64, ptr %"%tcp_buf", align 8
  %cmpgttmp129 = icmp sgt i64 %"%tcp_buf.load128", 0
  br i1 %cmpgttmp129, label %then_534, label %merge_535

then_534:                                         ; preds = %then_532
  %"%tcp_buf.load131" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_HEADER_SIZE.load132" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @sad_ll_memset(i64 %"%tcp_buf.load131", i64 0, i64 %"%TCP_HEADER_SIZE.load132")
  %"%sock.load133" = load i64, ptr %sock1, align 8
  %calltmp134 = call i64 @tcp_get_lport(i64 %"%sock.load133")
  %"%sock.load135" = load i64, ptr %sock1, align 8
  %calltmp136 = call i64 @tcp_get_rport(i64 %"%sock.load135")
  %"%sock.load137" = load i64, ptr %sock1, align 8
  %calltmp138 = call i64 @tcp_get_seq(i64 %"%sock.load137")
  %"%seq.load139" = load i64, ptr %seq4, align 8
  %addtmp140 = add i64 %"%seq.load139", 1
  %"%tcp_buf.load141" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_ACK.load142" = load i64, ptr @TCP_ACK, align 8
  %"%TCP_WINDOW_SIZE.load143" = load i64, ptr @TCP_WINDOW_SIZE, align 8
  %calltmp144 = call i64 @tcp_build_header(i64 %"%tcp_buf.load141", i64 %calltmp134, i64 %calltmp136, i64 %calltmp138, i64 %addtmp140, i64 %"%TCP_ACK.load142", i64 %"%TCP_WINDOW_SIZE.load143")
  %"%sock.load145" = load i64, ptr %sock1, align 8
  %calltmp146 = call i64 @tcp_get_rip(i64 %"%sock.load145")
  %"%IP_PROTO_TCP.load147" = load i64, ptr @IP_PROTO_TCP, align 8
  %"%tcp_buf.load148" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_HEADER_SIZE.load149" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @ip_send(i64 %calltmp146, i64 %"%IP_PROTO_TCP.load147", i64 %"%tcp_buf.load148", i64 %"%TCP_HEADER_SIZE.load149")
  %"%tcp_buf.load150" = load i64, ptr %"%tcp_buf", align 8
  call void @sad_ll_kfree(i64 %"%tcp_buf.load150")
  br label %merge_535

then_536:                                         ; preds = %merge_533
  %"%seq.load157" = load i64, ptr %seq4, align 8
  %"%data_len.load158" = load i64, ptr %data_len8, align 8
  %addtmp159 = add i64 %"%seq.load157", %"%data_len.load158"
  %"%sock.load160" = load i64, ptr %sock1, align 8
  %calltmp161 = call i64 @tcp_set_ack(i64 %"%sock.load160", i64 %addtmp159)
  %"%TCP_HEADER_SIZE.load162" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @sad_ll_kmalloc(i64 %"%TCP_HEADER_SIZE.load162")
  store i64 0, ptr %"%tcp_buf", align 8
  %"%tcp_buf.load163" = load i64, ptr %"%tcp_buf", align 8
  %cmpgttmp164 = icmp sgt i64 %"%tcp_buf.load163", 0
  br i1 %cmpgttmp164, label %then_538, label %merge_539

then_538:                                         ; preds = %then_536
  %"%tcp_buf.load165" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_HEADER_SIZE.load166" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @sad_ll_memset(i64 %"%tcp_buf.load165", i64 0, i64 %"%TCP_HEADER_SIZE.load166")
  %"%sock.load167" = load i64, ptr %sock1, align 8
  %calltmp168 = call i64 @tcp_get_lport(i64 %"%sock.load167")
  %"%sock.load169" = load i64, ptr %sock1, align 8
  %calltmp170 = call i64 @tcp_get_rport(i64 %"%sock.load169")
  %"%sock.load171" = load i64, ptr %sock1, align 8
  %calltmp172 = call i64 @tcp_get_seq(i64 %"%sock.load171")
  %"%seq.load173" = load i64, ptr %seq4, align 8
  %"%data_len.load174" = load i64, ptr %data_len8, align 8
  %addtmp175 = add i64 %"%seq.load173", %"%data_len.load174"
  %"%tcp_buf.load176" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_ACK.load177" = load i64, ptr @TCP_ACK, align 8
  %"%TCP_WINDOW_SIZE.load178" = load i64, ptr @TCP_WINDOW_SIZE, align 8
  %calltmp179 = call i64 @tcp_build_header(i64 %"%tcp_buf.load176", i64 %calltmp168, i64 %calltmp170, i64 %calltmp172, i64 %addtmp175, i64 %"%TCP_ACK.load177", i64 %"%TCP_WINDOW_SIZE.load178")
  %"%sock.load180" = load i64, ptr %sock1, align 8
  %calltmp181 = call i64 @tcp_get_rip(i64 %"%sock.load180")
  %"%IP_PROTO_TCP.load182" = load i64, ptr @IP_PROTO_TCP, align 8
  %"%tcp_buf.load183" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_HEADER_SIZE.load184" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @ip_send(i64 %calltmp181, i64 %"%IP_PROTO_TCP.load182", i64 %"%tcp_buf.load183", i64 %"%TCP_HEADER_SIZE.load184")
  %"%tcp_buf.load185" = load i64, ptr %"%tcp_buf", align 8
  call void @sad_ll_kfree(i64 %"%tcp_buf.load185")
  br label %merge_539

then_540:                                         ; preds = %merge_531
  %"%TCP_ACK.load190" = load i64, ptr @TCP_ACK, align 8
  %shltmp191 = shl i64 %"%TCP_ACK.load190", 1
  %"%flags.load192" = load i64, ptr %flags6, align 8
  %divtmp193 = sdiv i64 %"%flags.load192", %shltmp191
  %multmp194 = mul i64 %divtmp193, %shltmp191
  %"%flags.load195" = load i64, ptr %flags6, align 8
  %subtmp196 = sub i64 %"%flags.load195", %multmp194
  store i64 %subtmp196, ptr %"%has_ack197", align 8
  %"%has_ack.load198" = load i64, ptr %"%has_ack197", align 8
  %"%TCP_ACK.load199" = load i64, ptr @TCP_ACK, align 8
  %cmpgetmp200 = icmp sge i64 %"%has_ack.load198", %"%TCP_ACK.load199"
  br i1 %cmpgetmp200, label %then_542, label %merge_543

then_542:                                         ; preds = %then_540
  %"%sock.load203" = load i64, ptr %sock1, align 8
  %"%TCP_STATE_FIN_WAIT2.load204" = load i64, ptr @TCP_STATE_FIN_WAIT2, align 8
  %calltmp205 = call i64 @tcp_set_state(i64 %"%sock.load203", i64 %"%TCP_STATE_FIN_WAIT2.load204")
  br label %merge_543

then_544:                                         ; preds = %merge_541
  %"%TCP_FIN.load206" = load i64, ptr @TCP_FIN, align 8
  %shltmp207 = shl i64 %"%TCP_FIN.load206", 1
  %"%flags.load208" = load i64, ptr %flags6, align 8
  %modtmp209 = srem i64 %"%flags.load208", %shltmp207
  store i64 %modtmp209, ptr %"%has_fin210", align 8
  %"%has_fin.load211" = load i64, ptr %"%has_fin210", align 8
  %"%TCP_FIN.load212" = load i64, ptr @TCP_FIN, align 8
  %cmpgetmp213 = icmp sge i64 %"%has_fin.load211", %"%TCP_FIN.load212"
  br i1 %cmpgetmp213, label %then_546, label %merge_547

then_546:                                         ; preds = %then_544
  %"%seq.load217" = load i64, ptr %seq4, align 8
  %addtmp218 = add i64 %"%seq.load217", 1
  %"%sock.load219" = load i64, ptr %sock1, align 8
  %calltmp220 = call i64 @tcp_set_ack(i64 %"%sock.load219", i64 %addtmp218)
  %"%TCP_HEADER_SIZE.load221" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @sad_ll_kmalloc(i64 %"%TCP_HEADER_SIZE.load221")
  store i64 0, ptr %"%tcp_buf", align 8
  %"%tcp_buf.load222" = load i64, ptr %"%tcp_buf", align 8
  %cmpgttmp223 = icmp sgt i64 %"%tcp_buf.load222", 0
  br i1 %cmpgttmp223, label %then_548, label %merge_549

then_548:                                         ; preds = %then_546
  %"%tcp_buf.load224" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_HEADER_SIZE.load225" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @sad_ll_memset(i64 %"%tcp_buf.load224", i64 0, i64 %"%TCP_HEADER_SIZE.load225")
  %"%sock.load226" = load i64, ptr %sock1, align 8
  %calltmp227 = call i64 @tcp_get_lport(i64 %"%sock.load226")
  %"%sock.load228" = load i64, ptr %sock1, align 8
  %calltmp229 = call i64 @tcp_get_rport(i64 %"%sock.load228")
  %"%sock.load230" = load i64, ptr %sock1, align 8
  %calltmp231 = call i64 @tcp_get_seq(i64 %"%sock.load230")
  %"%seq.load232" = load i64, ptr %seq4, align 8
  %addtmp233 = add i64 %"%seq.load232", 1
  %"%tcp_buf.load234" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_ACK.load235" = load i64, ptr @TCP_ACK, align 8
  %"%TCP_WINDOW_SIZE.load236" = load i64, ptr @TCP_WINDOW_SIZE, align 8
  %calltmp237 = call i64 @tcp_build_header(i64 %"%tcp_buf.load234", i64 %calltmp227, i64 %calltmp229, i64 %calltmp231, i64 %addtmp233, i64 %"%TCP_ACK.load235", i64 %"%TCP_WINDOW_SIZE.load236")
  %"%sock.load238" = load i64, ptr %sock1, align 8
  %calltmp239 = call i64 @tcp_get_rip(i64 %"%sock.load238")
  %"%IP_PROTO_TCP.load240" = load i64, ptr @IP_PROTO_TCP, align 8
  %"%tcp_buf.load241" = load i64, ptr %"%tcp_buf", align 8
  %"%TCP_HEADER_SIZE.load242" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @ip_send(i64 %calltmp239, i64 %"%IP_PROTO_TCP.load240", i64 %"%tcp_buf.load241", i64 %"%TCP_HEADER_SIZE.load242")
  %"%tcp_buf.load243" = load i64, ptr %"%tcp_buf", align 8
  call void @sad_ll_kfree(i64 %"%tcp_buf.load243")
  br label %merge_549

then_550:                                         ; preds = %merge_545
  %"%TCP_HEADER_SIZE.load246" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @sad_ll_kmalloc(i64 %"%TCP_HEADER_SIZE.load246")
  store i64 0, ptr %"%tcp_buf247", align 8
  %"%tcp_buf.load248" = load i64, ptr %"%tcp_buf247", align 8
  %cmpgttmp249 = icmp sgt i64 %"%tcp_buf.load248", 0
  br i1 %cmpgttmp249, label %then_552, label %merge_553

then_552:                                         ; preds = %then_550
  %"%tcp_buf.load252" = load i64, ptr %"%tcp_buf247", align 8
  %"%TCP_HEADER_SIZE.load253" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @sad_ll_memset(i64 %"%tcp_buf.load252", i64 0, i64 %"%TCP_HEADER_SIZE.load253")
  %"%TCP_FIN.load254" = load i64, ptr @TCP_FIN, align 8
  %"%TCP_ACK.load255" = load i64, ptr @TCP_ACK, align 8
  %addtmp256 = add i64 %"%TCP_FIN.load254", %"%TCP_ACK.load255"
  store i64 %addtmp256, ptr %"%fin_flags", align 8
  %"%sock.load257" = load i64, ptr %sock1, align 8
  %calltmp258 = call i64 @tcp_get_lport(i64 %"%sock.load257")
  %"%sock.load259" = load i64, ptr %sock1, align 8
  %calltmp260 = call i64 @tcp_get_rport(i64 %"%sock.load259")
  %"%sock.load261" = load i64, ptr %sock1, align 8
  %calltmp262 = call i64 @tcp_get_seq(i64 %"%sock.load261")
  %"%sock.load263" = load i64, ptr %sock1, align 8
  %calltmp264 = call i64 @tcp_get_ack(i64 %"%sock.load263")
  %"%tcp_buf.load265" = load i64, ptr %"%tcp_buf247", align 8
  %"%fin_flags.load" = load i64, ptr %"%fin_flags", align 8
  %"%TCP_WINDOW_SIZE.load266" = load i64, ptr @TCP_WINDOW_SIZE, align 8
  %calltmp267 = call i64 @tcp_build_header(i64 %"%tcp_buf.load265", i64 %calltmp258, i64 %calltmp260, i64 %calltmp262, i64 %calltmp264, i64 %"%fin_flags.load", i64 %"%TCP_WINDOW_SIZE.load266")
  %"%sock.load268" = load i64, ptr %sock1, align 8
  %calltmp269 = call i64 @tcp_get_rip(i64 %"%sock.load268")
  %"%IP_PROTO_TCP.load270" = load i64, ptr @IP_PROTO_TCP, align 8
  %"%tcp_buf.load271" = load i64, ptr %"%tcp_buf247", align 8
  %"%TCP_HEADER_SIZE.load272" = load i64, ptr @TCP_HEADER_SIZE, align 8
  call void @ip_send(i64 %calltmp269, i64 %"%IP_PROTO_TCP.load270", i64 %"%tcp_buf.load271", i64 %"%TCP_HEADER_SIZE.load272")
  %"%tcp_buf.load273" = load i64, ptr %"%tcp_buf247", align 8
  call void @sad_ll_kfree(i64 %"%tcp_buf.load273")
  br label %merge_553

then_554:                                         ; preds = %merge_551
  %"%sock.load277" = load i64, ptr %sock1, align 8
  %"%TCP_STATE_CLOSED.load278" = load i64, ptr @TCP_STATE_CLOSED, align 8
  %calltmp279 = call i64 @tcp_set_state(i64 %"%sock.load277", i64 %"%TCP_STATE_CLOSED.load278")
  %calltmp280 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  %"%sock.load281" = load i64, ptr %sock1, align 8
  %calltmp282 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%sock.load281")
  %calltmp283 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1
}

define i64 @"tcp_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %"%st" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %calltmp = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  %calltmp1 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  %"%2027.load" = load i64, ptr @tcp_connections, align 8
  %calltmp3 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%2027.load")
  %calltmp4 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp5 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  %"%2033.load" = load i64, ptr @tcp_tx_count, align 8
  %calltmp6 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%2033.load")
  %calltmp7 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp8 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  %"%2039.load" = load i64, ptr @tcp_rx_count, align 8
  %calltmp9 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%2039.load")
  %calltmp10 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp11 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.30 to i64))
  %"%2045.load" = load i64, ptr @tcp_errors, align 8
  %calltmp12 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%2045.load")
  %calltmp13 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp14 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  %"%2051.load" = load i64, ptr @udp_tx_count, align 8
  %calltmp15 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%2051.load")
  %calltmp16 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp17 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.32 to i64))
  %"%2057.load" = load i64, ptr @udp_rx_count, align 8
  %calltmp18 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%2057.load")
  %calltmp19 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp20 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  %"%2063.load" = load i64, ptr @udp_errors, align 8
  %calltmp21 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%2063.load")
  %calltmp22 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%i", align 8
  br label %while_cond_556

merge_560:                                        ; preds = %then_559, %while_body_557
  %"%i.load38" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load38", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_556

then_559:                                         ; preds = %while_body_557
  %calltmp27 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.35 to i64))
  %"%i.load28" = load i64, ptr %"%i", align 8
  %calltmp29 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%i.load28")
  %calltmp30 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.36 to i64))
  %"%st.load31" = load i64, ptr %"%st", align 8
  %calltmp32 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %"%st.load31")
  %calltmp33 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %"%i.load34" = load i64, ptr %"%i", align 8
  %calltmp35 = call i64 @tcp_get_lport(i64 %"%i.load34")
  %calltmp36 = call i64 @"tcp_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp35)
  %calltmp37 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_560

while_body_557:                                   ; preds = %while_cond_556
  %"%i.load23" = load i64, ptr %"%i", align 8
  %calltmp24 = call i64 @tcp_get_state(i64 %"%i.load23")
  store i64 %calltmp24, ptr %"%st", align 8
  %"%st.load" = load i64, ptr %"%st", align 8
  %"%TCP_STATE_CLOSED.load" = load i64, ptr @TCP_STATE_CLOSED, align 8
  %cmpnetmp = icmp ne i64 %"%st.load", %"%TCP_STATE_CLOSED.load"
  br i1 %cmpnetmp, label %then_559, label %merge_560

while_cond_556:                                   ; preds = %merge_560, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%TCP_MAX_SOCKETS.load" = load i64, ptr @TCP_MAX_SOCKETS, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%TCP_MAX_SOCKETS.load"
  br i1 %cmplttmp, label %while_body_557, label %while_exit_558

while_exit_558:                                   ; preds = %while_cond_556
  %calltmp25 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.34 to i64))
  %calltmp26 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @"tcp_\D9\87\D9\8A\D8\A6"() {
entry:
  %"%auto_port" = alloca i64, align 8
  %"%udp_errors" = alloca i64, align 8
  %"%udp_rx_count" = alloca i64, align 8
  %"%udp_tx_count" = alloca i64, align 8
  %"%tcp_errors" = alloca i64, align 8
  %"%tcp_rx_count" = alloca i64, align 8
  %"%tcp_tx_count" = alloca i64, align 8
  %"%tcp_connections" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %calltmp = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.38 to i64))
  %calltmp1 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%i", align 8
  br label %while_cond_561

while_body_562:                                   ; preds = %while_cond_561
  %"%i.load2" = load i64, ptr %"%i", align 8
  %"%TCP_STATE_CLOSED.load" = load i64, ptr @TCP_STATE_CLOSED, align 8
  %calltmp3 = call i64 @tcp_set_state(i64 %"%i.load2", i64 %"%TCP_STATE_CLOSED.load")
  %"%i.load4" = load i64, ptr %"%i", align 8
  %calltmp5 = call i64 @tcp_set_lport(i64 %"%i.load4", i64 0)
  %"%i.load6" = load i64, ptr %"%i", align 8
  %calltmp7 = call i64 @tcp_set_rport(i64 %"%i.load6", i64 0)
  %"%i.load8" = load i64, ptr %"%i", align 8
  %calltmp9 = call i64 @tcp_set_rip(i64 %"%i.load8", i64 0)
  %"%i.load10" = load i64, ptr %"%i", align 8
  %calltmp11 = call i64 @tcp_set_seq(i64 %"%i.load10", i64 0)
  %"%i.load12" = load i64, ptr %"%i", align 8
  %calltmp13 = call i64 @tcp_set_ack(i64 %"%i.load12", i64 0)
  %"%i.load14" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load14", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_561

while_body_565:                                   ; preds = %while_cond_564
  %"%i.load17" = load i64, ptr %"%i", align 8
  %calltmp18 = call i64 @udp_set_active(i64 %"%i.load17", i64 0)
  %"%i.load19" = load i64, ptr %"%i", align 8
  %calltmp20 = call i64 @udp_set_lport(i64 %"%i.load19", i64 0)
  %"%i.load21" = load i64, ptr %"%i", align 8
  %calltmp22 = call i64 @udp_set_rxbuf(i64 %"%i.load21", i64 0)
  %"%i.load23" = load i64, ptr %"%i", align 8
  %calltmp24 = call i64 @udp_set_rxlen(i64 %"%i.load23", i64 0)
  %"%i.load25" = load i64, ptr %"%i", align 8
  %calltmp26 = call i64 @udp_set_sender(i64 %"%i.load25", i64 0)
  %"%i.load27" = load i64, ptr %"%i", align 8
  %calltmp28 = call i64 @udp_set_sport(i64 %"%i.load27", i64 0)
  %"%i.load29" = load i64, ptr %"%i", align 8
  %addtmp30 = add i64 %"%i.load29", 1
  store i64 %addtmp30, ptr %"%i", align 8
  br label %while_cond_564

while_cond_561:                                   ; preds = %while_body_562, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%TCP_MAX_SOCKETS.load" = load i64, ptr @TCP_MAX_SOCKETS, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%TCP_MAX_SOCKETS.load"
  br i1 %cmplttmp, label %while_body_562, label %while_exit_563

while_cond_564:                                   ; preds = %while_body_565, %while_exit_563
  %"%i.load15" = load i64, ptr %"%i", align 8
  %"%UDP_MAX_SOCKETS.load" = load i64, ptr @UDP_MAX_SOCKETS, align 8
  %cmplttmp16 = icmp slt i64 %"%i.load15", %"%UDP_MAX_SOCKETS.load"
  br i1 %cmplttmp16, label %while_body_565, label %while_exit_566

while_exit_563:                                   ; preds = %while_cond_561
  store i64 0, ptr %"%i", align 8
  br label %while_cond_564

while_exit_566:                                   ; preds = %while_cond_564
  store i64 0, ptr %"%tcp_connections", align 8
  store i64 0, ptr %"%tcp_tx_count", align 8
  store i64 0, ptr %"%tcp_rx_count", align 8
  store i64 0, ptr %"%tcp_errors", align 8
  store i64 0, ptr %"%udp_tx_count", align 8
  store i64 0, ptr %"%udp_rx_count", align 8
  store i64 0, ptr %"%udp_errors", align 8
  store i64 49152, ptr %"%auto_port", align 8
  %calltmp31 = call i64 @"tcp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.39 to i64))
  %calltmp32 = call i64 @"tcp_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1
}

define i64 @"tcp_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  ret i64 0
}

define void @__sad_main() {
entry:
  %"%auto_port" = alloca i64, align 8
  %"%udp_errors" = alloca i64, align 8
  %"%udp_rx_count" = alloca i64, align 8
  %"%udp_tx_count" = alloca i64, align 8
  %"%tcp_errors" = alloca i64, align 8
  %"%tcp_rx_count" = alloca i64, align 8
  %"%tcp_tx_count" = alloca i64, align 8
  %"%tcp_connections" = alloca i64, align 8
  %"%udp_sport_7" = alloca i64, align 8
  %"%udp_sport_6" = alloca i64, align 8
  %"%udp_sport_5" = alloca i64, align 8
  %"%udp_sport_4" = alloca i64, align 8
  %"%udp_sport_3" = alloca i64, align 8
  %"%udp_sport_2" = alloca i64, align 8
  %"%udp_sport_1" = alloca i64, align 8
  %"%udp_sport_0" = alloca i64, align 8
  %"%udp_sender_7" = alloca i64, align 8
  %"%udp_sender_6" = alloca i64, align 8
  %"%udp_sender_5" = alloca i64, align 8
  %"%udp_sender_4" = alloca i64, align 8
  %"%udp_sender_3" = alloca i64, align 8
  %"%udp_sender_2" = alloca i64, align 8
  %"%udp_sender_1" = alloca i64, align 8
  %"%udp_sender_0" = alloca i64, align 8
  %"%udp_rxlen_7" = alloca i64, align 8
  %"%udp_rxlen_6" = alloca i64, align 8
  %"%udp_rxlen_5" = alloca i64, align 8
  %"%udp_rxlen_4" = alloca i64, align 8
  %"%udp_rxlen_3" = alloca i64, align 8
  %"%udp_rxlen_2" = alloca i64, align 8
  %"%udp_rxlen_1" = alloca i64, align 8
  %"%udp_rxlen_0" = alloca i64, align 8
  %"%udp_rxbuf_7" = alloca i64, align 8
  %"%udp_rxbuf_6" = alloca i64, align 8
  %"%udp_rxbuf_5" = alloca i64, align 8
  %"%udp_rxbuf_4" = alloca i64, align 8
  %"%udp_rxbuf_3" = alloca i64, align 8
  %"%udp_rxbuf_2" = alloca i64, align 8
  %"%udp_rxbuf_1" = alloca i64, align 8
  %"%udp_rxbuf_0" = alloca i64, align 8
  %"%udp_lport_7" = alloca i64, align 8
  %"%udp_lport_6" = alloca i64, align 8
  %"%udp_lport_5" = alloca i64, align 8
  %"%udp_lport_4" = alloca i64, align 8
  %"%udp_lport_3" = alloca i64, align 8
  %"%udp_lport_2" = alloca i64, align 8
  %"%udp_lport_1" = alloca i64, align 8
  %"%udp_lport_0" = alloca i64, align 8
  %"%udp_active_7" = alloca i64, align 8
  %"%udp_active_6" = alloca i64, align 8
  %"%udp_active_5" = alloca i64, align 8
  %"%udp_active_4" = alloca i64, align 8
  %"%udp_active_3" = alloca i64, align 8
  %"%udp_active_2" = alloca i64, align 8
  %"%udp_active_1" = alloca i64, align 8
  %"%udp_active_0" = alloca i64, align 8
  %"%tcp_ack_7" = alloca i64, align 8
  %"%tcp_ack_6" = alloca i64, align 8
  %"%tcp_ack_5" = alloca i64, align 8
  %"%tcp_ack_4" = alloca i64, align 8
  %"%tcp_ack_3" = alloca i64, align 8
  %"%tcp_ack_2" = alloca i64, align 8
  %"%tcp_ack_1" = alloca i64, align 8
  %"%tcp_ack_0" = alloca i64, align 8
  %"%tcp_seq_7" = alloca i64, align 8
  %"%tcp_seq_6" = alloca i64, align 8
  %"%tcp_seq_5" = alloca i64, align 8
  %"%tcp_seq_4" = alloca i64, align 8
  %"%tcp_seq_3" = alloca i64, align 8
  %"%tcp_seq_2" = alloca i64, align 8
  %"%tcp_seq_1" = alloca i64, align 8
  %"%tcp_seq_0" = alloca i64, align 8
  %"%tcp_rip_7" = alloca i64, align 8
  %"%tcp_rip_6" = alloca i64, align 8
  %"%tcp_rip_5" = alloca i64, align 8
  %"%tcp_rip_4" = alloca i64, align 8
  %"%tcp_rip_3" = alloca i64, align 8
  %"%tcp_rip_2" = alloca i64, align 8
  %"%tcp_rip_1" = alloca i64, align 8
  %"%tcp_rip_0" = alloca i64, align 8
  %"%tcp_rport_7" = alloca i64, align 8
  %"%tcp_rport_6" = alloca i64, align 8
  %"%tcp_rport_5" = alloca i64, align 8
  %"%tcp_rport_4" = alloca i64, align 8
  %"%tcp_rport_3" = alloca i64, align 8
  %"%tcp_rport_2" = alloca i64, align 8
  %"%tcp_rport_1" = alloca i64, align 8
  %"%tcp_rport_0" = alloca i64, align 8
  %"%tcp_lport_7" = alloca i64, align 8
  %"%tcp_lport_6" = alloca i64, align 8
  %"%tcp_lport_5" = alloca i64, align 8
  %"%tcp_lport_4" = alloca i64, align 8
  %"%tcp_lport_3" = alloca i64, align 8
  %"%tcp_lport_2" = alloca i64, align 8
  %"%tcp_lport_1" = alloca i64, align 8
  %"%tcp_lport_0" = alloca i64, align 8
  %"%tcp_state_7" = alloca i64, align 8
  %"%tcp_state_6" = alloca i64, align 8
  %"%tcp_state_5" = alloca i64, align 8
  %"%tcp_state_4" = alloca i64, align 8
  %"%tcp_state_3" = alloca i64, align 8
  %"%tcp_state_2" = alloca i64, align 8
  %"%tcp_state_1" = alloca i64, align 8
  %"%tcp_state_0" = alloca i64, align 8
  %"%UDP_MAX_SOCKETS" = alloca i64, align 8
  %"%TCP_MAX_RETRIES" = alloca i64, align 8
  %"%TCP_RETRANSMIT_TIMEOUT" = alloca i64, align 8
  %"%TCP_MAX_SOCKETS" = alloca i64, align 8
  %"%TCP_WINDOW_SIZE" = alloca i64, align 8
  %"%TCP_STATE_TIME_WAIT" = alloca i64, align 8
  %"%TCP_STATE_LAST_ACK" = alloca i64, align 8
  %"%TCP_STATE_CLOSE_WAIT" = alloca i64, align 8
  %"%TCP_STATE_FIN_WAIT2" = alloca i64, align 8
  %"%TCP_STATE_FIN_WAIT1" = alloca i64, align 8
  %"%TCP_STATE_ESTABLISHED" = alloca i64, align 8
  %"%TCP_STATE_SYN_RCVD" = alloca i64, align 8
  %"%TCP_STATE_SYN_SENT" = alloca i64, align 8
  %"%TCP_STATE_LISTEN" = alloca i64, align 8
  %"%TCP_STATE_CLOSED" = alloca i64, align 8
  %"%TCP_URG" = alloca i64, align 8
  %"%TCP_ACK" = alloca i64, align 8
  %"%TCP_PSH" = alloca i64, align 8
  %"%TCP_RST" = alloca i64, align 8
  %"%TCP_SYN" = alloca i64, align 8
  %"%TCP_FIN" = alloca i64, align 8
  %"%TCP_HEADER_SIZE" = alloca i64, align 8
  %"%UDP_HEADER_SIZE" = alloca i64, align 8
  %"%IP_PROTO_UDP" = alloca i64, align 8
  %"%IP_PROTO_TCP" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 6, ptr %"%IP_PROTO_TCP", align 8
  store i64 17, ptr %"%IP_PROTO_UDP", align 8
  store i64 8, ptr %"%UDP_HEADER_SIZE", align 8
  store i64 20, ptr %"%TCP_HEADER_SIZE", align 8
  store i64 1, ptr %"%TCP_FIN", align 8
  store i64 2, ptr %"%TCP_SYN", align 8
  store i64 4, ptr %"%TCP_RST", align 8
  store i64 8, ptr %"%TCP_PSH", align 8
  store i64 16, ptr %"%TCP_ACK", align 8
  store i64 32, ptr %"%TCP_URG", align 8
  store i64 0, ptr %"%TCP_STATE_CLOSED", align 8
  store i64 1, ptr %"%TCP_STATE_LISTEN", align 8
  store i64 2, ptr %"%TCP_STATE_SYN_SENT", align 8
  store i64 3, ptr %"%TCP_STATE_SYN_RCVD", align 8
  store i64 4, ptr %"%TCP_STATE_ESTABLISHED", align 8
  store i64 5, ptr %"%TCP_STATE_FIN_WAIT1", align 8
  store i64 6, ptr %"%TCP_STATE_FIN_WAIT2", align 8
  store i64 7, ptr %"%TCP_STATE_CLOSE_WAIT", align 8
  store i64 8, ptr %"%TCP_STATE_LAST_ACK", align 8
  store i64 9, ptr %"%TCP_STATE_TIME_WAIT", align 8
  store i64 4096, ptr %"%TCP_WINDOW_SIZE", align 8
  store i64 8, ptr %"%TCP_MAX_SOCKETS", align 8
  store i64 3000, ptr %"%TCP_RETRANSMIT_TIMEOUT", align 8
  store i64 5, ptr %"%TCP_MAX_RETRIES", align 8
  store i64 8, ptr %"%UDP_MAX_SOCKETS", align 8
  store i64 0, ptr %"%tcp_state_0", align 8
  store i64 0, ptr %"%tcp_state_1", align 8
  store i64 0, ptr %"%tcp_state_2", align 8
  store i64 0, ptr %"%tcp_state_3", align 8
  store i64 0, ptr %"%tcp_state_4", align 8
  store i64 0, ptr %"%tcp_state_5", align 8
  store i64 0, ptr %"%tcp_state_6", align 8
  store i64 0, ptr %"%tcp_state_7", align 8
  store i64 0, ptr %"%tcp_lport_0", align 8
  store i64 0, ptr %"%tcp_lport_1", align 8
  store i64 0, ptr %"%tcp_lport_2", align 8
  store i64 0, ptr %"%tcp_lport_3", align 8
  store i64 0, ptr %"%tcp_lport_4", align 8
  store i64 0, ptr %"%tcp_lport_5", align 8
  store i64 0, ptr %"%tcp_lport_6", align 8
  store i64 0, ptr %"%tcp_lport_7", align 8
  store i64 0, ptr %"%tcp_rport_0", align 8
  store i64 0, ptr %"%tcp_rport_1", align 8
  store i64 0, ptr %"%tcp_rport_2", align 8
  store i64 0, ptr %"%tcp_rport_3", align 8
  store i64 0, ptr %"%tcp_rport_4", align 8
  store i64 0, ptr %"%tcp_rport_5", align 8
  store i64 0, ptr %"%tcp_rport_6", align 8
  store i64 0, ptr %"%tcp_rport_7", align 8
  store i64 0, ptr %"%tcp_rip_0", align 8
  store i64 0, ptr %"%tcp_rip_1", align 8
  store i64 0, ptr %"%tcp_rip_2", align 8
  store i64 0, ptr %"%tcp_rip_3", align 8
  store i64 0, ptr %"%tcp_rip_4", align 8
  store i64 0, ptr %"%tcp_rip_5", align 8
  store i64 0, ptr %"%tcp_rip_6", align 8
  store i64 0, ptr %"%tcp_rip_7", align 8
  store i64 0, ptr %"%tcp_seq_0", align 8
  store i64 0, ptr %"%tcp_seq_1", align 8
  store i64 0, ptr %"%tcp_seq_2", align 8
  store i64 0, ptr %"%tcp_seq_3", align 8
  store i64 0, ptr %"%tcp_seq_4", align 8
  store i64 0, ptr %"%tcp_seq_5", align 8
  store i64 0, ptr %"%tcp_seq_6", align 8
  store i64 0, ptr %"%tcp_seq_7", align 8
  store i64 0, ptr %"%tcp_ack_0", align 8
  store i64 0, ptr %"%tcp_ack_1", align 8
  store i64 0, ptr %"%tcp_ack_2", align 8
  store i64 0, ptr %"%tcp_ack_3", align 8
  store i64 0, ptr %"%tcp_ack_4", align 8
  store i64 0, ptr %"%tcp_ack_5", align 8
  store i64 0, ptr %"%tcp_ack_6", align 8
  store i64 0, ptr %"%tcp_ack_7", align 8
  store i64 0, ptr %"%udp_active_0", align 8
  store i64 0, ptr %"%udp_active_1", align 8
  store i64 0, ptr %"%udp_active_2", align 8
  store i64 0, ptr %"%udp_active_3", align 8
  store i64 0, ptr %"%udp_active_4", align 8
  store i64 0, ptr %"%udp_active_5", align 8
  store i64 0, ptr %"%udp_active_6", align 8
  store i64 0, ptr %"%udp_active_7", align 8
  store i64 0, ptr %"%udp_lport_0", align 8
  store i64 0, ptr %"%udp_lport_1", align 8
  store i64 0, ptr %"%udp_lport_2", align 8
  store i64 0, ptr %"%udp_lport_3", align 8
  store i64 0, ptr %"%udp_lport_4", align 8
  store i64 0, ptr %"%udp_lport_5", align 8
  store i64 0, ptr %"%udp_lport_6", align 8
  store i64 0, ptr %"%udp_lport_7", align 8
  store i64 0, ptr %"%udp_rxbuf_0", align 8
  store i64 0, ptr %"%udp_rxbuf_1", align 8
  store i64 0, ptr %"%udp_rxbuf_2", align 8
  store i64 0, ptr %"%udp_rxbuf_3", align 8
  store i64 0, ptr %"%udp_rxbuf_4", align 8
  store i64 0, ptr %"%udp_rxbuf_5", align 8
  store i64 0, ptr %"%udp_rxbuf_6", align 8
  store i64 0, ptr %"%udp_rxbuf_7", align 8
  store i64 0, ptr %"%udp_rxlen_0", align 8
  store i64 0, ptr %"%udp_rxlen_1", align 8
  store i64 0, ptr %"%udp_rxlen_2", align 8
  store i64 0, ptr %"%udp_rxlen_3", align 8
  store i64 0, ptr %"%udp_rxlen_4", align 8
  store i64 0, ptr %"%udp_rxlen_5", align 8
  store i64 0, ptr %"%udp_rxlen_6", align 8
  store i64 0, ptr %"%udp_rxlen_7", align 8
  store i64 0, ptr %"%udp_sender_0", align 8
  store i64 0, ptr %"%udp_sender_1", align 8
  store i64 0, ptr %"%udp_sender_2", align 8
  store i64 0, ptr %"%udp_sender_3", align 8
  store i64 0, ptr %"%udp_sender_4", align 8
  store i64 0, ptr %"%udp_sender_5", align 8
  store i64 0, ptr %"%udp_sender_6", align 8
  store i64 0, ptr %"%udp_sender_7", align 8
  store i64 0, ptr %"%udp_sport_0", align 8
  store i64 0, ptr %"%udp_sport_1", align 8
  store i64 0, ptr %"%udp_sport_2", align 8
  store i64 0, ptr %"%udp_sport_3", align 8
  store i64 0, ptr %"%udp_sport_4", align 8
  store i64 0, ptr %"%udp_sport_5", align 8
  store i64 0, ptr %"%udp_sport_6", align 8
  store i64 0, ptr %"%udp_sport_7", align 8
  store i64 0, ptr %"%tcp_connections", align 8
  store i64 0, ptr %"%tcp_tx_count", align 8
  store i64 0, ptr %"%tcp_rx_count", align 8
  store i64 0, ptr %"%tcp_errors", align 8
  store i64 0, ptr %"%udp_tx_count", align 8
  store i64 0, ptr %"%udp_rx_count", align 8
  store i64 0, ptr %"%udp_errors", align 8
  store i64 49152, ptr %"%auto_port", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
