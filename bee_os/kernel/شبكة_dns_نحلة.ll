; ModuleID = 'شبكة_dns_نحلة'
source_filename = "\D8\B4\D8\A8\D9\83\D8\A9_dns_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@COM1 = constant i64 1016
@DNS_PORT = constant i64 53
@DNS_HEADER_SIZE = constant i64 12
@DNS_TYPE_A = constant i64 1
@DNS_CLASS_IN = constant i64 1
@DNS_MAX_RESPONSE = constant i64 512
@DNS_RECURSION = constant i64 1
@DHCP_SERVER_PORT = constant i64 67
@DHCP_CLIENT_PORT = constant i64 68
@DHCP_MAGIC_1 = constant i64 99
@DHCP_MAGIC_2 = constant i64 130
@DHCP_MAGIC_3 = constant i64 83
@DHCP_MAGIC_4 = constant i64 99
@DHCP_DISCOVER = constant i64 1
@DHCP_OFFER = constant i64 2
@DHCP_REQUEST = constant i64 3
@DHCP_ACK = constant i64 5
@DHCP_NAK = constant i64 6
@DHCP_OPT_SUBNET = constant i64 1
@DHCP_OPT_ROUTER = constant i64 3
@DHCP_OPT_DNS = constant i64 6
@DHCP_OPT_HOSTNAME = constant i64 12
@DHCP_OPT_DOMAIN = constant i64 15
@DHCP_OPT_REQIP = constant i64 50
@DHCP_OPT_LEASE = constant i64 51
@DHCP_OPT_MSGTYPE = constant i64 53
@DHCP_OPT_SERVER = constant i64 54
@DHCP_OPT_PARAMLIST = constant i64 55
@DHCP_OPT_END = constant i64 255
@dns_server_0 = global i64 8
@dns_server_1 = global i64 8
@dns_server_2 = global i64 8
@dns_server_3 = global i64 8
@dns_query_id = global i64 1
@dns_sock = global i64 0
@dns_cache_hash_0 = global i64 0
@dns_cache_hash_1 = global i64 0
@dns_cache_hash_2 = global i64 0
@dns_cache_hash_3 = global i64 0
@dns_cache_hash_4 = global i64 0
@dns_cache_hash_5 = global i64 0
@dns_cache_hash_6 = global i64 0
@dns_cache_hash_7 = global i64 0
@dns_cache_ip_0 = global i64 0
@dns_cache_ip_1 = global i64 0
@dns_cache_ip_2 = global i64 0
@dns_cache_ip_3 = global i64 0
@dns_cache_ip_4 = global i64 0
@dns_cache_ip_5 = global i64 0
@dns_cache_ip_6 = global i64 0
@dns_cache_ip_7 = global i64 0
@dns_cache_ttl_0 = global i64 0
@dns_cache_ttl_1 = global i64 0
@dns_cache_ttl_2 = global i64 0
@dns_cache_ttl_3 = global i64 0
@dns_cache_ttl_4 = global i64 0
@dns_cache_ttl_5 = global i64 0
@dns_cache_ttl_6 = global i64 0
@dns_cache_ttl_7 = global i64 0
@dhcp_state = global i64 0
@dhcp_xid = global i64 0
@dhcp_server_ip = global i64 0
@dhcp_offered_ip = global i64 0
@dhcp_lease_time = global i64 0
@dhcp_sock = global i64 0
@local_mac_0 = global i64 0
@local_mac_1 = global i64 0
@local_mac_2 = global i64 0
@local_mac_3 = global i64 0
@local_mac_4 = global i64 0
@local_mac_5 = global i64 0
@dns_queries = global i64 0
@dns_responses = global i64 0
@dns_cache_hits = global i64 0
@dns_errors = global i64 0
@dhcp_discovers = global i64 0
@dhcp_offers = global i64 0
@dhcp_acks = global i64 0
@dhcp_naks = global i64 0
@str.const = private unnamed_addr constant [16 x i8] c"[DNS] Cache hit\00", align 1
@str.const.1 = private unnamed_addr constant [43 x i8] c"[DNS] \D9\81\D8\B4\D9\84 \D8\A5\D8\B1\D8\B3\D8\A7\D9\84 \D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\B9\D9\84\D8\A7\D9\85\00", align 1
@str.const.2 = private unnamed_addr constant [29 x i8] c"[DNS] Query sent, waiting...\00", align 1
@str.const.3 = private unnamed_addr constant [22 x i8] c"[DNS] \D9\81\D8\B4\D9\84 \D8\A7\D9\84\D8\AD\D9\84\00", align 1
@str.const.4 = private unnamed_addr constant [17 x i8] c"[DNS] Resolved: \00", align 1
@str.const.5 = private unnamed_addr constant [27 x i8] c"[DHCP] Sending DISCOVER...\00", align 1
@str.const.6 = private unnamed_addr constant [34 x i8] c"[DHCP] \D9\81\D8\B4\D9\84 \D9\81\D8\AA\D8\AD \D8\A7\D9\84\D9\85\D8\A3\D8\AE\D8\B0\00", align 1
@str.const.7 = private unnamed_addr constant [21 x i8] c"[DHCP] DISCOVER sent\00", align 1
@str.const.8 = private unnamed_addr constant [23 x i8] c"[DHCP] DISCOVER failed\00", align 1
@str.const.9 = private unnamed_addr constant [24 x i8] c"[DHCP] OFFER received: \00", align 1
@str.const.10 = private unnamed_addr constant [16 x i8] c"[DHCP] ACK! IP=\00", align 1
@str.const.11 = private unnamed_addr constant [8 x i8] c" lease=\00", align 1
@str.const.12 = private unnamed_addr constant [20 x i8] c"[DHCP] NAK received\00", align 1
@str.const.13 = private unnamed_addr constant [26 x i8] c"[DHCP] Sending REQUEST...\00", align 1
@str.const.14 = private unnamed_addr constant [20 x i8] c"[DHCP] REQUEST sent\00", align 1
@str.const.15 = private unnamed_addr constant [42 x i8] c"\E2\95\90\E2\95\90\E2\95\90 [DNS/DHCP] \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.16 = private unnamed_addr constant [17 x i8] c"  \D8\AE\D8\A7\D8\AF\D9\85 DNS: \00", align 1
@str.const.17 = private unnamed_addr constant [27 x i8] c"  \D8\A7\D8\B3\D8\AA\D8\B9\D9\84\D8\A7\D9\85\D8\A7\D8\AA DNS: \00", align 1
@str.const.18 = private unnamed_addr constant [25 x i8] c"  \D8\A7\D8\B3\D8\AA\D8\AC\D8\A7\D8\A8\D8\A7\D8\AA DNS: \00", align 1
@str.const.19 = private unnamed_addr constant [19 x i8] c"  DNS cache hits: \00", align 1
@str.const.20 = private unnamed_addr constant [19 x i8] c"  \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1 DNS: \00", align 1
@str.const.21 = private unnamed_addr constant [18 x i8] c"  DHCP \D8\AD\D8\A7\D9\84\D8\A9: \00", align 1
@str.const.22 = private unnamed_addr constant [22 x i8] c"  DHCP \D8\A7\D9\83\D8\AA\D8\B4\D8\A7\D9\81: \00", align 1
@str.const.23 = private unnamed_addr constant [18 x i8] c"  DHCP \D8\B9\D8\B1\D9\88\D8\B6: \00", align 1
@str.const.24 = private unnamed_addr constant [13 x i8] c"  DHCP ACK: \00", align 1
@str.const.25 = private unnamed_addr constant [106 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.26 = private unnamed_addr constant [25 x i8] c"[DNS/DHCP] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9...\00", align 1
@str.const.27 = private unnamed_addr constant [20 x i8] c"[DNS/DHCP] \D8\AC\D8\A7\D9\87\D8\B2\00", align 1

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

declare void @udp_open(i64)

declare void @udp_close(i64)

declare void @udp_sendto(i64, i64, i64, i64, i64)

declare void @udp_broadcast(i64, i64, i64, i64)

declare void @udp_recv(i64, i64, i64)

declare void @udp_get_sender(i64)

declare void @udp_get_sport(i64)

declare void @ip_to_int(i64, i64, i64, i64)

declare void @ip_byte(i64, i64)

declare void @"ip_\D8\B9\D9\8A\D9\86_\D8\B9\D9\86\D9\88\D8\A7\D9\86"(i64, i64, i64, i64)

declare void @"ip_\D8\B9\D9\8A\D9\86_\D9\82\D9\86\D8\A7\D8\B9"(i64, i64, i64, i64)

declare void @"ip_\D8\B9\D9\8A\D9\86_\D8\A8\D9\88\D8\A7\D8\A8\D8\A9"(i64, i64, i64, i64)

define i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
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

define i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
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

define i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"() {
entry:
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 13)
  %"%COM1.load1" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load1", i64 10)
  ret i64 0
}

define i64 @dns_cache_get_hash(i64 %idx) {
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
  %loadtmp = load i64, ptr @dns_cache_hash_0, align 8
  ret i64 %loadtmp

then_26:                                          ; preds = %merge_25
  %loadtmp4 = load i64, ptr @dns_cache_hash_1, align 8
  ret i64 %loadtmp4

then_28:                                          ; preds = %merge_27
  %loadtmp7 = load i64, ptr @dns_cache_hash_2, align 8
  ret i64 %loadtmp7

then_30:                                          ; preds = %merge_29
  %loadtmp10 = load i64, ptr @dns_cache_hash_3, align 8
  ret i64 %loadtmp10

then_32:                                          ; preds = %merge_31
  %loadtmp13 = load i64, ptr @dns_cache_hash_4, align 8
  ret i64 %loadtmp13

then_34:                                          ; preds = %merge_33
  %loadtmp16 = load i64, ptr @dns_cache_hash_5, align 8
  ret i64 %loadtmp16

then_36:                                          ; preds = %merge_35
  %loadtmp19 = load i64, ptr @dns_cache_hash_6, align 8
  ret i64 %loadtmp19

then_38:                                          ; preds = %merge_37
  %loadtmp22 = load i64, ptr @dns_cache_hash_7, align 8
  ret i64 %loadtmp22
}

define i64 @dns_cache_set_hash(i64 %idx, i64 %val) {
entry:
  %"%dns_cache_hash_7" = alloca i64, align 8
  %"%dns_cache_hash_6" = alloca i64, align 8
  %"%dns_cache_hash_5" = alloca i64, align 8
  %"%dns_cache_hash_4" = alloca i64, align 8
  %"%dns_cache_hash_3" = alloca i64, align 8
  %"%dns_cache_hash_2" = alloca i64, align 8
  %"%dns_cache_hash_1" = alloca i64, align 8
  %"%dns_cache_hash_0" = alloca i64, align 8
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
  store i64 %"%129.load", ptr %"%dns_cache_hash_0", align 8
  br label %merge_41

then_42:                                          ; preds = %merge_41
  %"%133.load" = load i64, ptr %val2, align 8
  store i64 %"%133.load", ptr %"%dns_cache_hash_1", align 8
  br label %merge_43

then_44:                                          ; preds = %merge_43
  %"%137.load" = load i64, ptr %val2, align 8
  store i64 %"%137.load", ptr %"%dns_cache_hash_2", align 8
  br label %merge_45

then_46:                                          ; preds = %merge_45
  %"%141.load" = load i64, ptr %val2, align 8
  store i64 %"%141.load", ptr %"%dns_cache_hash_3", align 8
  br label %merge_47

then_48:                                          ; preds = %merge_47
  %"%145.load" = load i64, ptr %val2, align 8
  store i64 %"%145.load", ptr %"%dns_cache_hash_4", align 8
  br label %merge_49

then_50:                                          ; preds = %merge_49
  %"%149.load" = load i64, ptr %val2, align 8
  store i64 %"%149.load", ptr %"%dns_cache_hash_5", align 8
  br label %merge_51

then_52:                                          ; preds = %merge_51
  %"%153.load" = load i64, ptr %val2, align 8
  store i64 %"%153.load", ptr %"%dns_cache_hash_6", align 8
  br label %merge_53

then_54:                                          ; preds = %merge_53
  %"%157.load" = load i64, ptr %val2, align 8
  store i64 %"%157.load", ptr %"%dns_cache_hash_7", align 8
  br label %merge_55
}

define i64 @dns_cache_get_ip(i64 %idx) {
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
  %loadtmp = load i64, ptr @dns_cache_ip_0, align 8
  ret i64 %loadtmp

then_58:                                          ; preds = %merge_57
  %loadtmp4 = load i64, ptr @dns_cache_ip_1, align 8
  ret i64 %loadtmp4

then_60:                                          ; preds = %merge_59
  %loadtmp7 = load i64, ptr @dns_cache_ip_2, align 8
  ret i64 %loadtmp7

then_62:                                          ; preds = %merge_61
  %loadtmp10 = load i64, ptr @dns_cache_ip_3, align 8
  ret i64 %loadtmp10

then_64:                                          ; preds = %merge_63
  %loadtmp13 = load i64, ptr @dns_cache_ip_4, align 8
  ret i64 %loadtmp13

then_66:                                          ; preds = %merge_65
  %loadtmp16 = load i64, ptr @dns_cache_ip_5, align 8
  ret i64 %loadtmp16

then_68:                                          ; preds = %merge_67
  %loadtmp19 = load i64, ptr @dns_cache_ip_6, align 8
  ret i64 %loadtmp19

then_70:                                          ; preds = %merge_69
  %loadtmp22 = load i64, ptr @dns_cache_ip_7, align 8
  ret i64 %loadtmp22
}

define i64 @dns_cache_set_ip(i64 %idx, i64 %val) {
entry:
  %"%dns_cache_ip_7" = alloca i64, align 8
  %"%dns_cache_ip_6" = alloca i64, align 8
  %"%dns_cache_ip_5" = alloca i64, align 8
  %"%dns_cache_ip_4" = alloca i64, align 8
  %"%dns_cache_ip_3" = alloca i64, align 8
  %"%dns_cache_ip_2" = alloca i64, align 8
  %"%dns_cache_ip_1" = alloca i64, align 8
  %"%dns_cache_ip_0" = alloca i64, align 8
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
  store i64 %"%203.load", ptr %"%dns_cache_ip_0", align 8
  br label %merge_73

then_74:                                          ; preds = %merge_73
  %"%207.load" = load i64, ptr %val2, align 8
  store i64 %"%207.load", ptr %"%dns_cache_ip_1", align 8
  br label %merge_75

then_76:                                          ; preds = %merge_75
  %"%211.load" = load i64, ptr %val2, align 8
  store i64 %"%211.load", ptr %"%dns_cache_ip_2", align 8
  br label %merge_77

then_78:                                          ; preds = %merge_77
  %"%215.load" = load i64, ptr %val2, align 8
  store i64 %"%215.load", ptr %"%dns_cache_ip_3", align 8
  br label %merge_79

then_80:                                          ; preds = %merge_79
  %"%219.load" = load i64, ptr %val2, align 8
  store i64 %"%219.load", ptr %"%dns_cache_ip_4", align 8
  br label %merge_81

then_82:                                          ; preds = %merge_81
  %"%223.load" = load i64, ptr %val2, align 8
  store i64 %"%223.load", ptr %"%dns_cache_ip_5", align 8
  br label %merge_83

then_84:                                          ; preds = %merge_83
  %"%227.load" = load i64, ptr %val2, align 8
  store i64 %"%227.load", ptr %"%dns_cache_ip_6", align 8
  br label %merge_85

then_86:                                          ; preds = %merge_85
  %"%231.load" = load i64, ptr %val2, align 8
  store i64 %"%231.load", ptr %"%dns_cache_ip_7", align 8
  br label %merge_87
}

define i64 @dns_hash_name(i64 %name_addr) {
entry:
  %"%ch" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %"%hash" = alloca i64, align 8
  %name_addr1 = alloca i64, align 8
  store i64 %name_addr, ptr %name_addr1, align 8
  store i64 5381, ptr %"%hash", align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_88

merge_92:                                         ; preds = %while_body_89
  %"%hash.load" = load i64, ptr %"%hash", align 8
  %multmp = mul i64 %"%hash.load", 33
  %"%ch.load4" = load i64, ptr %"%ch", align 8
  %addtmp5 = add i64 %multmp, %"%ch.load4"
  store i64 %addtmp5, ptr %"%hash", align 8
  %"%hash.load6" = load i64, ptr %"%hash", align 8
  %cmplttmp7 = icmp slt i64 %"%hash.load6", 0
  br i1 %cmplttmp7, label %then_93, label %merge_94

merge_94:                                         ; preds = %then_93, %merge_92
  %"%i.load10" = load i64, ptr %"%i", align 8
  %addtmp11 = add i64 %"%i.load10", 1
  store i64 %addtmp11, ptr %"%i", align 8
  br label %while_cond_88

then_91:                                          ; preds = %while_body_89
  %loadtmp3 = load i64, ptr %"%hash", align 8
  ret i64 %loadtmp3

then_93:                                          ; preds = %merge_92
  %"%hash.load8" = load i64, ptr %"%hash", align 8
  %addtmp9 = add i64 %"%hash.load8", 2147483647
  store i64 %addtmp9, ptr %"%hash", align 8
  br label %merge_94

while_body_89:                                    ; preds = %while_cond_88
  %"%name_addr.load" = load i64, ptr %name_addr1, align 8
  %"%i.load2" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%name_addr.load", %"%i.load2"
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%ch", align 8
  %"%ch.load" = load i64, ptr %"%ch", align 8
  %cmpeqtmp = icmp eq i64 %"%ch.load", 0
  br i1 %cmpeqtmp, label %then_91, label %merge_92

while_cond_88:                                    ; preds = %merge_94, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %cmplttmp = icmp slt i64 %"%i.load", 64
  br i1 %cmplttmp, label %while_body_89, label %while_exit_90

while_exit_90:                                    ; preds = %while_cond_88
  %loadtmp = load i64, ptr %"%hash", align 8
  ret i64 %loadtmp
}

define i64 @dns_cache_lookup(i64 %name_hash) {
entry:
  %"%dns_cache_hits" = alloca i64, align 8
  %"%ip" = alloca i64, align 8
  %"%h" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %name_hash1 = alloca i64, align 8
  store i64 %name_hash, ptr %name_hash1, align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_95

merge_101:                                        ; preds = %then_98
  br label %merge_99

merge_99:                                         ; preds = %merge_101, %while_body_96
  %"%i.load5" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load5", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_95

then_100:                                         ; preds = %then_98
  %"%dns_cache_hits.load" = load i64, ptr @dns_cache_hits, align 8
  %addtmp6 = add i64 %"%dns_cache_hits.load", 1
  store i64 %addtmp6, ptr %"%dns_cache_hits", align 8
  %loadtmp = load i64, ptr %"%ip", align 8
  ret i64 %loadtmp

then_98:                                          ; preds = %while_body_96
  %"%i.load3" = load i64, ptr %"%i", align 8
  %calltmp4 = call i64 @dns_cache_get_ip(i64 %"%i.load3")
  store i64 %calltmp4, ptr %"%ip", align 8
  %"%ip.load" = load i64, ptr %"%ip", align 8
  %cmpgttmp = icmp sgt i64 %"%ip.load", 0
  br i1 %cmpgttmp, label %then_100, label %merge_101

while_body_96:                                    ; preds = %while_cond_95
  %"%i.load2" = load i64, ptr %"%i", align 8
  %calltmp = call i64 @dns_cache_get_hash(i64 %"%i.load2")
  store i64 %calltmp, ptr %"%h", align 8
  %"%h.load" = load i64, ptr %"%h", align 8
  %"%name_hash.load" = load i64, ptr %name_hash1, align 8
  %cmpeqtmp = icmp eq i64 %"%h.load", %"%name_hash.load"
  br i1 %cmpeqtmp, label %then_98, label %merge_99

while_cond_95:                                    ; preds = %merge_99, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %cmplttmp = icmp slt i64 %"%i.load", 8
  br i1 %cmplttmp, label %while_body_96, label %while_exit_97

while_exit_97:                                    ; preds = %while_cond_95
  ret i64 0
}

define i64 @dns_cache_add(i64 %name_hash, i64 %ip_val) {
entry:
  %"%h" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %ip_val2 = alloca i64, align 8
  store i64 %ip_val, ptr %ip_val2, align 8
  %name_hash1 = alloca i64, align 8
  store i64 %name_hash, ptr %name_hash1, align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_102

merge_106:                                        ; preds = %while_body_103
  %"%i.load12" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load12", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_102

then_105:                                         ; preds = %while_body_103
  %"%i.load6" = load i64, ptr %"%i", align 8
  %"%name_hash.load7" = load i64, ptr %name_hash1, align 8
  %calltmp8 = call i64 @dns_cache_set_hash(i64 %"%i.load6", i64 %"%name_hash.load7")
  %"%i.load9" = load i64, ptr %"%i", align 8
  %"%ip_val.load10" = load i64, ptr %ip_val2, align 8
  %calltmp11 = call i64 @dns_cache_set_ip(i64 %"%i.load9", i64 %"%ip_val.load10")
  %loadtmp = load i64, ptr %"%i", align 8
  ret i64 %loadtmp

while_body_103:                                   ; preds = %while_cond_102
  %"%i.load3" = load i64, ptr %"%i", align 8
  %calltmp = call i64 @dns_cache_get_hash(i64 %"%i.load3")
  store i64 %calltmp, ptr %"%h", align 8
  %"%h.load" = load i64, ptr %"%h", align 8
  %cmpeqtmp = icmp eq i64 %"%h.load", 0
  br i1 %cmpeqtmp, label %then_105, label %merge_106

while_cond_102:                                   ; preds = %merge_106, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %cmplttmp = icmp slt i64 %"%i.load", 8
  br i1 %cmplttmp, label %while_body_103, label %while_exit_104

while_exit_104:                                   ; preds = %while_cond_102
  %"%name_hash.load" = load i64, ptr %name_hash1, align 8
  %calltmp4 = call i64 @dns_cache_set_hash(i64 0, i64 %"%name_hash.load")
  %"%ip_val.load" = load i64, ptr %ip_val2, align 8
  %calltmp5 = call i64 @dns_cache_set_ip(i64 0, i64 %"%ip_val.load")
  ret i64 0
}

define i64 @dns_build_query(i64 %name_addr, i64 %buf) {
entry:
  %"%llen" = alloca i64, align 8
  %"%label_len" = alloca i64, align 8
  %"%ch" = alloca i64, align 8
  %"%label_start" = alloca i64, align 8
  %"%read_pos" = alloca i64, align 8
  %"%write_pos" = alloca i64, align 8
  %"%dns_query_id" = alloca i64, align 8
  %"%qid" = alloca i64, align 8
  %buf2 = alloca i64, align 8
  store i64 %buf, ptr %buf2, align 8
  %name_addr1 = alloca i64, align 8
  store i64 %name_addr, ptr %name_addr1, align 8
  %"%307.load" = load i64, ptr @dns_query_id, align 8
  store i64 %"%307.load", ptr %"%qid", align 8
  %"%dns_query_id.load" = load i64, ptr @dns_query_id, align 8
  %addtmp = add i64 %"%dns_query_id.load", 1
  store i64 %addtmp, ptr %"%dns_query_id", align 8
  %"%qid.load" = load i64, ptr %"%qid", align 8
  %shrtmp = lshr i64 %"%qid.load", 8
  %"%buf.load" = load i64, ptr %buf2, align 8
  call void @sad_ll_mem_write8(i64 %"%buf.load", i64 %shrtmp)
  %"%buf.load3" = load i64, ptr %buf2, align 8
  %addtmp4 = add i64 %"%buf.load3", 1
  %"%qid.load5" = load i64, ptr %"%qid", align 8
  %andtmp = and i64 %"%qid.load5", 255
  call void @sad_ll_mem_write8(i64 %addtmp4, i64 %andtmp)
  %"%buf.load6" = load i64, ptr %buf2, align 8
  %addtmp7 = add i64 %"%buf.load6", 2
  call void @sad_ll_mem_write8(i64 %addtmp7, i64 1)
  %"%buf.load8" = load i64, ptr %buf2, align 8
  %addtmp9 = add i64 %"%buf.load8", 3
  call void @sad_ll_mem_write8(i64 %addtmp9, i64 0)
  %"%buf.load10" = load i64, ptr %buf2, align 8
  %addtmp11 = add i64 %"%buf.load10", 4
  call void @sad_ll_mem_write8(i64 %addtmp11, i64 0)
  %"%buf.load12" = load i64, ptr %buf2, align 8
  %addtmp13 = add i64 %"%buf.load12", 5
  call void @sad_ll_mem_write8(i64 %addtmp13, i64 1)
  %"%buf.load14" = load i64, ptr %buf2, align 8
  %addtmp15 = add i64 %"%buf.load14", 6
  call void @sad_ll_mem_write8(i64 %addtmp15, i64 0)
  %"%buf.load16" = load i64, ptr %buf2, align 8
  %addtmp17 = add i64 %"%buf.load16", 7
  call void @sad_ll_mem_write8(i64 %addtmp17, i64 0)
  %"%buf.load18" = load i64, ptr %buf2, align 8
  %addtmp19 = add i64 %"%buf.load18", 8
  call void @sad_ll_mem_write8(i64 %addtmp19, i64 0)
  %"%buf.load20" = load i64, ptr %buf2, align 8
  %addtmp21 = add i64 %"%buf.load20", 9
  call void @sad_ll_mem_write8(i64 %addtmp21, i64 0)
  %"%buf.load22" = load i64, ptr %buf2, align 8
  %addtmp23 = add i64 %"%buf.load22", 10
  call void @sad_ll_mem_write8(i64 %addtmp23, i64 0)
  %"%buf.load24" = load i64, ptr %buf2, align 8
  %addtmp25 = add i64 %"%buf.load24", 11
  call void @sad_ll_mem_write8(i64 %addtmp25, i64 0)
  %"%359.load" = load i64, ptr @DNS_HEADER_SIZE, align 8
  store i64 %"%359.load", ptr %"%write_pos", align 8
  store i64 0, ptr %"%read_pos", align 8
  %"%364.load" = load i64, ptr %"%write_pos", align 8
  store i64 %"%364.load", ptr %"%label_start", align 8
  %"%write_pos.load" = load i64, ptr %"%write_pos", align 8
  %addtmp26 = add i64 %"%write_pos.load", 1
  store i64 %addtmp26, ptr %"%write_pos", align 8
  br label %while_cond_107

else_112:                                         ; preds = %while_body_108
  %"%ch.load45" = load i64, ptr %"%ch", align 8
  %cmpeqtmp46 = icmp eq i64 %"%ch.load45", 46
  br i1 %cmpeqtmp46, label %then_113, label %else_115

else_115:                                         ; preds = %else_112
  %"%buf.load58" = load i64, ptr %buf2, align 8
  %"%write_pos.load59" = load i64, ptr %"%write_pos", align 8
  %addtmp60 = add i64 %"%buf.load58", %"%write_pos.load59"
  %"%ch.load61" = load i64, ptr %"%ch", align 8
  call void @sad_ll_mem_write8(i64 %addtmp60, i64 %"%ch.load61")
  %"%write_pos.load62" = load i64, ptr %"%write_pos", align 8
  %addtmp63 = add i64 %"%write_pos.load62", 1
  store i64 %addtmp63, ptr %"%write_pos", align 8
  %"%read_pos.load64" = load i64, ptr %"%read_pos", align 8
  %addtmp65 = add i64 %"%read_pos.load64", 1
  store i64 %addtmp65, ptr %"%read_pos", align 8
  br label %merge_114

merge_111:                                        ; preds = %merge_114, %then_110
  br label %while_cond_107

merge_114:                                        ; preds = %else_115, %then_113
  br label %merge_111

then_110:                                         ; preds = %while_body_108
  %"%write_pos.load35" = load i64, ptr %"%write_pos", align 8
  %"%label_start.load" = load i64, ptr %"%label_start", align 8
  %subtmp = sub i64 %"%write_pos.load35", %"%label_start.load"
  %subtmp36 = sub i64 %subtmp, 1
  store i64 %subtmp36, ptr %"%label_len", align 8
  %"%buf.load37" = load i64, ptr %buf2, align 8
  %"%label_start.load38" = load i64, ptr %"%label_start", align 8
  %addtmp39 = add i64 %"%buf.load37", %"%label_start.load38"
  %"%label_len.load" = load i64, ptr %"%label_len", align 8
  call void @sad_ll_mem_write8(i64 %addtmp39, i64 %"%label_len.load")
  %"%buf.load40" = load i64, ptr %buf2, align 8
  %"%write_pos.load41" = load i64, ptr %"%write_pos", align 8
  %addtmp42 = add i64 %"%buf.load40", %"%write_pos.load41"
  call void @sad_ll_mem_write8(i64 %addtmp42, i64 0)
  %"%write_pos.load43" = load i64, ptr %"%write_pos", align 8
  %addtmp44 = add i64 %"%write_pos.load43", 1
  store i64 %addtmp44, ptr %"%write_pos", align 8
  store i64 256, ptr %"%read_pos", align 8
  br label %merge_111

then_113:                                         ; preds = %else_112
  %"%write_pos.load47" = load i64, ptr %"%write_pos", align 8
  %"%label_start.load48" = load i64, ptr %"%label_start", align 8
  %subtmp49 = sub i64 %"%write_pos.load47", %"%label_start.load48"
  %subtmp50 = sub i64 %subtmp49, 1
  store i64 %subtmp50, ptr %"%llen", align 8
  %"%buf.load51" = load i64, ptr %buf2, align 8
  %"%label_start.load52" = load i64, ptr %"%label_start", align 8
  %addtmp53 = add i64 %"%buf.load51", %"%label_start.load52"
  %"%llen.load" = load i64, ptr %"%llen", align 8
  call void @sad_ll_mem_write8(i64 %addtmp53, i64 %"%llen.load")
  %"%394.load" = load i64, ptr %"%write_pos", align 8
  store i64 %"%394.load", ptr %"%label_start", align 8
  %"%write_pos.load54" = load i64, ptr %"%write_pos", align 8
  %addtmp55 = add i64 %"%write_pos.load54", 1
  store i64 %addtmp55, ptr %"%write_pos", align 8
  %"%read_pos.load56" = load i64, ptr %"%read_pos", align 8
  %addtmp57 = add i64 %"%read_pos.load56", 1
  store i64 %addtmp57, ptr %"%read_pos", align 8
  br label %merge_114

while_body_108:                                   ; preds = %while_cond_107
  %"%name_addr.load" = load i64, ptr %name_addr1, align 8
  %"%read_pos.load27" = load i64, ptr %"%read_pos", align 8
  %addtmp28 = add i64 %"%name_addr.load", %"%read_pos.load27"
  call void @sad_ll_mem_read8(i64 %addtmp28)
  store i64 0, ptr %"%ch", align 8
  %"%ch.load" = load i64, ptr %"%ch", align 8
  %cmpeqtmp = icmp eq i64 %"%ch.load", 0
  br i1 %cmpeqtmp, label %then_110, label %else_112

while_cond_107:                                   ; preds = %merge_111, %entry
  %"%read_pos.load" = load i64, ptr %"%read_pos", align 8
  %cmplttmp = icmp slt i64 %"%read_pos.load", 256
  br i1 %cmplttmp, label %while_body_108, label %while_exit_109

while_exit_109:                                   ; preds = %while_cond_107
  %"%buf.load29" = load i64, ptr %buf2, align 8
  %"%write_pos.load30" = load i64, ptr %"%write_pos", align 8
  %addtmp31 = add i64 %"%buf.load29", %"%write_pos.load30"
  call void @sad_ll_mem_write8(i64 %addtmp31, i64 0)
  %addtmp32 = add i64 %addtmp31, 1
  %"%DNS_TYPE_A.load" = load i64, ptr @DNS_TYPE_A, align 8
  call void @sad_ll_mem_write8(i64 %addtmp32, i64 %"%DNS_TYPE_A.load")
  %"%write_pos.load33" = load i64, ptr %"%write_pos", align 8
  %addtmp34 = add i64 %"%write_pos.load33", 2
  store i64 %addtmp34, ptr %"%write_pos", align 8
  call void @sad_ll_mem_write8(i64 %addtmp31, i64 0)
  %"%DNS_CLASS_IN.load" = load i64, ptr @DNS_CLASS_IN, align 8
  call void @sad_ll_mem_write8(i64 %addtmp32, i64 %"%DNS_CLASS_IN.load")
  store i64 %addtmp34, ptr %"%write_pos", align 8
  %loadtmp = load i64, ptr %"%write_pos", align 8
  ret i64 %loadtmp
}

define i64 @dns_parse_response(i64 %buf, i64 %len) {
entry:
  %"%label_len" = alloca i64, align 8
  %"%pos" = alloca i64, align 8
  %"%ancount" = alloca i64, align 8
  %"%ancount_lo" = alloca i64, align 8
  %"%ancount_hi" = alloca i64, align 8
  %"%flags_hi" = alloca i64, align 8
  %len2 = alloca i64, align 8
  store i64 %len, ptr %len2, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%len.load" = load i64, ptr %len2, align 8
  %"%DNS_HEADER_SIZE.load" = load i64, ptr @DNS_HEADER_SIZE, align 8
  %cmplttmp = icmp slt i64 %"%len.load", %"%DNS_HEADER_SIZE.load"
  br i1 %cmplttmp, label %then_116, label %merge_117

merge_117:                                        ; preds = %entry
  %"%buf.load" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load", 2
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%flags_hi", align 8
  %"%flags_hi.load" = load i64, ptr %"%flags_hi", align 8
  %cmplttmp3 = icmp slt i64 %"%flags_hi.load", 128
  br i1 %cmplttmp3, label %then_118, label %merge_119

merge_119:                                        ; preds = %merge_117
  %"%buf.load4" = load i64, ptr %buf1, align 8
  %addtmp5 = add i64 %"%buf.load4", 6
  call void @sad_ll_mem_read8(i64 %addtmp5)
  store i64 0, ptr %"%ancount_hi", align 8
  %"%buf.load6" = load i64, ptr %buf1, align 8
  %addtmp7 = add i64 %"%buf.load6", 7
  call void @sad_ll_mem_read8(i64 %addtmp7)
  store i64 0, ptr %"%ancount_lo", align 8
  %"%ancount_hi.load" = load i64, ptr %"%ancount_hi", align 8
  %shltmp = shl i64 %"%ancount_hi.load", 8
  %"%ancount_lo.load" = load i64, ptr %"%ancount_lo", align 8
  %addtmp8 = add i64 %shltmp, %"%ancount_lo.load"
  store i64 %addtmp8, ptr %"%ancount", align 8
  %"%ancount.load" = load i64, ptr %"%ancount", align 8
  %cmpeqtmp = icmp eq i64 %"%ancount.load", 0
  br i1 %cmpeqtmp, label %then_120, label %merge_121

merge_121:                                        ; preds = %merge_119
  %"%453.load" = load i64, ptr @DNS_HEADER_SIZE, align 8
  store i64 %"%453.load", ptr %"%pos", align 8
  br label %while_cond_122

merge_126:                                        ; preds = %while_body_123
  %"%label_len.load23" = load i64, ptr %"%label_len", align 8
  %cmpgetmp = icmp sge i64 %"%label_len.load23", 192
  br i1 %cmpgetmp, label %then_127, label %merge_128

merge_128:                                        ; preds = %merge_126
  %"%pos.load33" = load i64, ptr %"%pos", align 8
  %"%label_len.load34" = load i64, ptr %"%label_len", align 8
  %addtmp35 = add i64 %"%pos.load33", %"%label_len.load34"
  %addtmp36 = add i64 %addtmp35, 1
  store i64 %addtmp36, ptr %"%pos", align 8
  br label %while_cond_122

then_116:                                         ; preds = %entry
  ret i64 0

then_118:                                         ; preds = %merge_117
  ret i64 0

then_120:                                         ; preds = %merge_119
  ret i64 0

then_125:                                         ; preds = %while_body_123
  %"%pos.load15" = load i64, ptr %"%pos", align 8
  %addtmp16 = add i64 %"%pos.load15", 1
  store i64 %addtmp16, ptr %"%pos", align 8
  %"%pos.load17" = load i64, ptr %"%pos", align 8
  %addtmp18 = add i64 %"%pos.load17", 4
  store i64 %addtmp18, ptr %"%pos", align 8
  %"%buf.load19" = load i64, ptr %buf1, align 8
  %"%len.load20" = load i64, ptr %len2, align 8
  %"%pos.load21" = load i64, ptr %"%pos", align 8
  %"%ancount.load22" = load i64, ptr %"%ancount", align 8
  %calltmp = call i64 @dns_parse_answers(i64 %"%buf.load19", i64 %"%len.load20", i64 %"%pos.load21", i64 %"%ancount.load22")
  ret i64 %calltmp

then_127:                                         ; preds = %merge_126
  %"%pos.load24" = load i64, ptr %"%pos", align 8
  %addtmp25 = add i64 %"%pos.load24", 2
  store i64 %addtmp25, ptr %"%pos", align 8
  %"%pos.load26" = load i64, ptr %"%pos", align 8
  %addtmp27 = add i64 %"%pos.load26", 4
  store i64 %addtmp27, ptr %"%pos", align 8
  %"%buf.load28" = load i64, ptr %buf1, align 8
  %"%len.load29" = load i64, ptr %len2, align 8
  %"%pos.load30" = load i64, ptr %"%pos", align 8
  %"%ancount.load31" = load i64, ptr %"%ancount", align 8
  %calltmp32 = call i64 @dns_parse_answers(i64 %"%buf.load28", i64 %"%len.load29", i64 %"%pos.load30", i64 %"%ancount.load31")
  ret i64 %calltmp32

while_body_123:                                   ; preds = %while_cond_122
  %"%buf.load11" = load i64, ptr %buf1, align 8
  %"%pos.load12" = load i64, ptr %"%pos", align 8
  %addtmp13 = add i64 %"%buf.load11", %"%pos.load12"
  call void @sad_ll_mem_read8(i64 %addtmp13)
  store i64 0, ptr %"%label_len", align 8
  %"%label_len.load" = load i64, ptr %"%label_len", align 8
  %cmpeqtmp14 = icmp eq i64 %"%label_len.load", 0
  br i1 %cmpeqtmp14, label %then_125, label %merge_126

while_cond_122:                                   ; preds = %merge_128, %merge_121
  %"%pos.load" = load i64, ptr %"%pos", align 8
  %"%len.load9" = load i64, ptr %len2, align 8
  %cmplttmp10 = icmp slt i64 %"%pos.load", %"%len.load9"
  br i1 %cmplttmp10, label %while_body_123, label %while_exit_124

while_exit_124:                                   ; preds = %while_cond_122
  ret i64 0
}

define i64 @dns_parse_answers(i64 %buf, i64 %len, i64 %start_pos, i64 %count) {
entry:
  %"%d" = alloca i64, align 8
  %"%c" = alloca i64, align 8
  %"%b" = alloca i64, align 8
  %"%a" = alloca i64, align 8
  %"%rdlen" = alloca i64, align 8
  %"%rdlen_lo" = alloca i64, align 8
  %"%rdlen_hi" = alloca i64, align 8
  %"%rtype" = alloca i64, align 8
  %"%rtype_lo" = alloca i64, align 8
  %"%rtype_hi" = alloca i64, align 8
  %"%name_byte" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %"%pos" = alloca i64, align 8
  %count4 = alloca i64, align 8
  store i64 %count, ptr %count4, align 8
  %start_pos3 = alloca i64, align 8
  store i64 %start_pos, ptr %start_pos3, align 8
  %len2 = alloca i64, align 8
  store i64 %len, ptr %len2, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%480.load" = load i64, ptr %start_pos3, align 8
  store i64 %"%480.load", ptr %"%pos", align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_129

else_136:                                         ; preds = %merge_133
  br label %while_cond_137

else_142:                                         ; preds = %while_body_138
  %"%pos.load27" = load i64, ptr %"%pos", align 8
  %"%name_byte.load28" = load i64, ptr %"%name_byte", align 8
  %addtmp29 = add i64 %"%pos.load27", %"%name_byte.load28"
  %addtmp30 = add i64 %addtmp29, 1
  store i64 %addtmp30, ptr %"%pos", align 8
  %"%buf.load31" = load i64, ptr %buf1, align 8
  %"%pos.load32" = load i64, ptr %"%pos", align 8
  %addtmp33 = add i64 %"%buf.load31", %"%pos.load32"
  call void @sad_ll_mem_read8(i64 %addtmp33)
  store i64 0, ptr %"%name_byte", align 8
  br label %merge_141

merge_133:                                        ; preds = %while_body_130
  %"%buf.load" = load i64, ptr %buf1, align 8
  %"%pos.load5" = load i64, ptr %"%pos", align 8
  %addtmp = add i64 %"%buf.load", %"%pos.load5"
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%name_byte", align 8
  %"%name_byte.load" = load i64, ptr %"%name_byte", align 8
  %cmpgetmp6 = icmp sge i64 %"%name_byte.load", 192
  br i1 %cmpgetmp6, label %then_134, label %else_136

merge_135:                                        ; preds = %merge_144, %then_134
  %"%buf.load9" = load i64, ptr %buf1, align 8
  %"%pos.load10" = load i64, ptr %"%pos", align 8
  %addtmp11 = add i64 %"%buf.load9", %"%pos.load10"
  call void @sad_ll_mem_read8(i64 %addtmp11)
  store i64 0, ptr %"%rtype_hi", align 8
  %addtmp12 = add i64 %addtmp11, 1
  call void @sad_ll_mem_read8(i64 %addtmp12)
  store i64 0, ptr %"%rtype_lo", align 8
  %"%rtype_hi.load" = load i64, ptr %"%rtype_hi", align 8
  %shltmp = shl i64 %"%rtype_hi.load", 8
  %"%rtype_lo.load" = load i64, ptr %"%rtype_lo", align 8
  %addtmp13 = add i64 %shltmp, %"%rtype_lo.load"
  store i64 %addtmp13, ptr %"%rtype", align 8
  %"%pos.load14" = load i64, ptr %"%pos", align 8
  %addtmp15 = add i64 %"%pos.load14", 2
  store i64 %addtmp15, ptr %"%pos", align 8
  store i64 %addtmp15, ptr %"%pos", align 8
  %"%pos.load16" = load i64, ptr %"%pos", align 8
  %addtmp17 = add i64 %"%pos.load16", 4
  store i64 %addtmp17, ptr %"%pos", align 8
  call void @sad_ll_mem_read8(i64 %addtmp11)
  store i64 0, ptr %"%rdlen_hi", align 8
  call void @sad_ll_mem_read8(i64 %addtmp12)
  store i64 0, ptr %"%rdlen_lo", align 8
  %"%rdlen_hi.load" = load i64, ptr %"%rdlen_hi", align 8
  %shltmp18 = shl i64 %"%rdlen_hi.load", 8
  %"%rdlen_lo.load" = load i64, ptr %"%rdlen_lo", align 8
  %addtmp19 = add i64 %shltmp18, %"%rdlen_lo.load"
  store i64 %addtmp19, ptr %"%rdlen", align 8
  store i64 %addtmp15, ptr %"%pos", align 8
  %"%rtype.load" = load i64, ptr %"%rtype", align 8
  %"%DNS_TYPE_A.load" = load i64, ptr @DNS_TYPE_A, align 8
  %cmpeqtmp = icmp eq i64 %"%rtype.load", %"%DNS_TYPE_A.load"
  br i1 %cmpeqtmp, label %then_145, label %merge_146

merge_141:                                        ; preds = %else_142, %then_140
  br label %while_cond_137

merge_144:                                        ; preds = %then_143, %while_exit_139
  br label %merge_135

merge_146:                                        ; preds = %merge_148, %merge_135
  %"%pos.load37" = load i64, ptr %"%pos", align 8
  %"%rdlen.load38" = load i64, ptr %"%rdlen", align 8
  %addtmp39 = add i64 %"%pos.load37", %"%rdlen.load38"
  store i64 %addtmp39, ptr %"%pos", align 8
  %"%i.load40" = load i64, ptr %"%i", align 8
  %addtmp41 = add i64 %"%i.load40", 1
  store i64 %addtmp41, ptr %"%i", align 8
  br label %while_cond_129

merge_148:                                        ; preds = %then_145
  br label %merge_146

then_132:                                         ; preds = %while_body_130
  ret i64 0

then_134:                                         ; preds = %merge_133
  %"%pos.load7" = load i64, ptr %"%pos", align 8
  %addtmp8 = add i64 %"%pos.load7", 2
  store i64 %addtmp8, ptr %"%pos", align 8
  br label %merge_135

then_140:                                         ; preds = %while_body_138
  %"%pos.load25" = load i64, ptr %"%pos", align 8
  %addtmp26 = add i64 %"%pos.load25", 2
  store i64 %addtmp26, ptr %"%pos", align 8
  store i64 0, ptr %"%name_byte", align 8
  br label %merge_141

then_143:                                         ; preds = %while_exit_139
  %"%pos.load34" = load i64, ptr %"%pos", align 8
  %addtmp35 = add i64 %"%pos.load34", 1
  store i64 %addtmp35, ptr %"%pos", align 8
  br label %merge_144

then_145:                                         ; preds = %merge_135
  %"%rdlen.load" = load i64, ptr %"%rdlen", align 8
  %cmpeqtmp36 = icmp eq i64 %"%rdlen.load", 4
  br i1 %cmpeqtmp36, label %then_147, label %merge_148

then_147:                                         ; preds = %then_145
  %"%buf.load42" = load i64, ptr %buf1, align 8
  %"%pos.load43" = load i64, ptr %"%pos", align 8
  %addtmp44 = add i64 %"%buf.load42", %"%pos.load43"
  call void @sad_ll_mem_read8(i64 %addtmp44)
  store i64 0, ptr %"%a", align 8
  %addtmp45 = add i64 %addtmp44, 1
  call void @sad_ll_mem_read8(i64 %addtmp45)
  store i64 0, ptr %"%b", align 8
  %addtmp46 = add i64 %addtmp44, 2
  call void @sad_ll_mem_read8(i64 %addtmp46)
  store i64 0, ptr %"%c", align 8
  %addtmp47 = add i64 %addtmp44, 3
  call void @sad_ll_mem_read8(i64 %addtmp47)
  store i64 0, ptr %"%d", align 8
  %"%a.load" = load i64, ptr %"%a", align 8
  %"%b.load" = load i64, ptr %"%b", align 8
  %"%c.load" = load i64, ptr %"%c", align 8
  %"%d.load" = load i64, ptr %"%d", align 8
  call void @ip_to_int(i64 %"%a.load", i64 %"%b.load", i64 %"%c.load", i64 %"%d.load")
  ret i64 0

while_body_130:                                   ; preds = %while_cond_129
  %"%pos.load" = load i64, ptr %"%pos", align 8
  %"%len.load" = load i64, ptr %len2, align 8
  %cmpgetmp = icmp sge i64 %"%pos.load", %"%len.load"
  br i1 %cmpgetmp, label %then_132, label %merge_133

while_body_138:                                   ; preds = %while_cond_137
  %"%name_byte.load21" = load i64, ptr %"%name_byte", align 8
  %cmpgetmp22 = icmp sge i64 %"%name_byte.load21", 192
  br i1 %cmpgetmp22, label %then_140, label %else_142

while_cond_129:                                   ; preds = %merge_146, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%count.load" = load i64, ptr %count4, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%count.load"
  br i1 %cmplttmp, label %while_body_130, label %while_exit_131

while_cond_137:                                   ; preds = %merge_141, %else_136
  %"%name_byte.load20" = load i64, ptr %"%name_byte", align 8
  %cmpgttmp = icmp sgt i64 %"%name_byte.load20", 0
  br i1 %cmpgttmp, label %while_body_138, label %while_exit_139

while_exit_131:                                   ; preds = %while_cond_129
  ret i64 0

while_exit_139:                                   ; preds = %while_cond_137
  %"%name_byte.load23" = load i64, ptr %"%name_byte", align 8
  %cmpeqtmp24 = icmp eq i64 %"%name_byte.load23", 0
  br i1 %cmpeqtmp24, label %then_143, label %merge_144
}

define i64 @dns_resolve(i64 %name_addr) {
entry:
  %"%dns_responses" = alloca i64, align 8
  %"%recv_len" = alloca i64, align 8
  %"%resolved_ip" = alloca i64, align 8
  %"%attempts" = alloca i64, align 8
  %"%resp_buf" = alloca i64, align 8
  %"%dns_queries" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%dns_ip" = alloca i64, align 8
  %"%dns_sock" = alloca i64, align 8
  %"%query_len" = alloca i64, align 8
  %"%dns_errors" = alloca i64, align 8
  %"%query_buf" = alloca i64, align 8
  %"%cached" = alloca i64, align 8
  %"%name_hash" = alloca i64, align 8
  %name_addr1 = alloca i64, align 8
  store i64 %name_addr, ptr %name_addr1, align 8
  %"%name_addr.load" = load i64, ptr %name_addr1, align 8
  %calltmp = call i64 @dns_hash_name(i64 %"%name_addr.load")
  store i64 %calltmp, ptr %"%name_hash", align 8
  %"%name_hash.load" = load i64, ptr %"%name_hash", align 8
  %calltmp2 = call i64 @dns_cache_lookup(i64 %"%name_hash.load")
  store i64 %calltmp2, ptr %"%cached", align 8
  %"%cached.load" = load i64, ptr %"%cached", align 8
  %cmpgttmp = icmp sgt i64 %"%cached.load", 0
  br i1 %cmpgttmp, label %then_149, label %merge_150

merge_150:                                        ; preds = %entry
  call void @sad_ll_kmalloc(i64 256)
  store i64 0, ptr %"%query_buf", align 8
  %"%query_buf.load" = load i64, ptr %"%query_buf", align 8
  %cmpeqtmp = icmp eq i64 %"%query_buf.load", 0
  br i1 %cmpeqtmp, label %then_151, label %merge_152

merge_152:                                        ; preds = %merge_150
  %"%query_buf.load5" = load i64, ptr %"%query_buf", align 8
  call void @sad_ll_memset(i64 %"%query_buf.load5", i64 0, i64 256)
  %"%name_addr.load6" = load i64, ptr %name_addr1, align 8
  %"%query_buf.load7" = load i64, ptr %"%query_buf", align 8
  %calltmp8 = call i64 @dns_build_query(i64 %"%name_addr.load6", i64 %"%query_buf.load7")
  store i64 %calltmp8, ptr %"%query_len", align 8
  %"%dns_sock.load" = load i64, ptr @dns_sock, align 8
  %cmpeqtmp9 = icmp eq i64 %"%dns_sock.load", 0
  br i1 %cmpeqtmp9, label %then_153, label %merge_154

merge_154:                                        ; preds = %merge_156, %merge_152
  %"%620.load" = load i64, ptr @dns_server_0, align 8
  %"%622.load" = load i64, ptr @dns_server_1, align 8
  %"%624.load" = load i64, ptr @dns_server_2, align 8
  %"%626.load" = load i64, ptr @dns_server_3, align 8
  call void @ip_to_int(i64 %"%620.load", i64 %"%622.load", i64 %"%624.load", i64 %"%626.load")
  store i64 0, ptr %"%dns_ip", align 8
  %"%dns_sock.load11" = load i64, ptr %"%dns_sock", align 8
  %"%dns_ip.load" = load i64, ptr %"%dns_ip", align 8
  %"%DNS_PORT.load" = load i64, ptr @DNS_PORT, align 8
  %"%query_buf.load12" = load i64, ptr %"%query_buf", align 8
  %"%query_len.load" = load i64, ptr %"%query_len", align 8
  call void @udp_sendto(i64 %"%dns_sock.load11", i64 %"%dns_ip.load", i64 %"%DNS_PORT.load", i64 %"%query_buf.load12", i64 %"%query_len.load")
  store i64 0, ptr %"%result", align 8
  %"%query_buf.load13" = load i64, ptr %"%query_buf", align 8
  call void @sad_ll_kfree(i64 %"%query_buf.load13")
  %"%result.load" = load i64, ptr %"%result", align 8
  %cmpletmp = icmp sle i64 %"%result.load", 0
  br i1 %cmpletmp, label %then_157, label %merge_158

merge_156:                                        ; preds = %then_153
  br label %merge_154

merge_158:                                        ; preds = %merge_154
  %"%dns_queries.load" = load i64, ptr @dns_queries, align 8
  %addtmp21 = add i64 %"%dns_queries.load", 1
  store i64 %addtmp21, ptr %"%dns_queries", align 8
  %calltmp22 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %calltmp23 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  %"%DNS_MAX_RESPONSE.load" = load i64, ptr @DNS_MAX_RESPONSE, align 8
  call void @sad_ll_kmalloc(i64 %"%DNS_MAX_RESPONSE.load")
  store i64 0, ptr %"%resp_buf", align 8
  %"%resp_buf.load" = load i64, ptr %"%resp_buf", align 8
  %cmpeqtmp24 = icmp eq i64 %"%resp_buf.load", 0
  br i1 %cmpeqtmp24, label %then_159, label %merge_160

merge_160:                                        ; preds = %merge_158
  store i64 0, ptr %"%attempts", align 8
  store i64 0, ptr %"%resolved_ip", align 8
  br label %while_cond_161

merge_165:                                        ; preds = %merge_167, %while_body_162
  %"%attempts.load39" = load i64, ptr %"%attempts", align 8
  %addtmp40 = add i64 %"%attempts.load39", 1
  store i64 %addtmp40, ptr %"%attempts", align 8
  br label %while_cond_161

merge_167:                                        ; preds = %then_164
  store i64 100, ptr %"%attempts", align 8
  br label %merge_165

then_149:                                         ; preds = %entry
  %calltmp3 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %calltmp4 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr %"%cached", align 8
  ret i64 %loadtmp

then_151:                                         ; preds = %merge_150
  %"%dns_errors.load" = load i64, ptr @dns_errors, align 8
  %addtmp = add i64 %"%dns_errors.load", 1
  store i64 %addtmp, ptr %"%dns_errors", align 8
  ret i64 0

then_153:                                         ; preds = %merge_152
  call void @udp_open(i64 0)
  store i64 0, ptr %"%dns_sock", align 8
  %"%dns_sock.load10" = load i64, ptr %"%dns_sock", align 8
  %cmplttmp = icmp slt i64 %"%dns_sock.load10", 0
  br i1 %cmplttmp, label %then_155, label %merge_156

then_155:                                         ; preds = %then_153
  %"%query_buf.load14" = load i64, ptr %"%query_buf", align 8
  call void @sad_ll_kfree(i64 %"%query_buf.load14")
  %"%dns_errors.load15" = load i64, ptr %"%dns_errors", align 8
  %addtmp16 = add i64 %"%dns_errors.load15", 1
  store i64 %addtmp16, ptr %"%dns_errors", align 8
  ret i64 0

then_157:                                         ; preds = %merge_154
  %calltmp17 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %calltmp18 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  %"%dns_errors.load19" = load i64, ptr %"%dns_errors", align 8
  %addtmp20 = add i64 %"%dns_errors.load19", 1
  store i64 %addtmp20, ptr %"%dns_errors", align 8
  ret i64 0

then_159:                                         ; preds = %merge_158
  ret i64 0

then_164:                                         ; preds = %while_body_162
  %"%resp_buf.load35" = load i64, ptr %"%resp_buf", align 8
  %"%recv_len.load36" = load i64, ptr %"%recv_len", align 8
  %calltmp37 = call i64 @dns_parse_response(i64 %"%resp_buf.load35", i64 %"%recv_len.load36")
  store i64 %calltmp37, ptr %"%resolved_ip", align 8
  %"%resolved_ip.load" = load i64, ptr %"%resolved_ip", align 8
  %cmpgttmp38 = icmp sgt i64 %"%resolved_ip.load", 0
  br i1 %cmpgttmp38, label %then_166, label %merge_167

then_166:                                         ; preds = %then_164
  %"%dns_responses.load" = load i64, ptr @dns_responses, align 8
  %addtmp41 = add i64 %"%dns_responses.load", 1
  store i64 %addtmp41, ptr %"%dns_responses", align 8
  %"%name_hash.load42" = load i64, ptr %"%name_hash", align 8
  %"%resolved_ip.load43" = load i64, ptr %"%resolved_ip", align 8
  %calltmp44 = call i64 @dns_cache_add(i64 %"%name_hash.load42", i64 %"%resolved_ip.load43")
  %calltmp45 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%resolved_ip.load46" = load i64, ptr %"%resolved_ip", align 8
  call void @ip_byte(i64 %"%resolved_ip.load46", i64 0)
  %calltmp47 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 0)
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 46)
  %"%resolved_ip.load48" = load i64, ptr %"%resolved_ip", align 8
  call void @ip_byte(i64 %"%resolved_ip.load48", i64 1)
  %calltmp49 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 0)
  %"%COM1.load50" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load50", i64 46)
  %"%resolved_ip.load51" = load i64, ptr %"%resolved_ip", align 8
  call void @ip_byte(i64 %"%resolved_ip.load51", i64 2)
  %calltmp52 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 0)
  %"%COM1.load53" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load53", i64 46)
  %"%resolved_ip.load54" = load i64, ptr %"%resolved_ip", align 8
  call void @ip_byte(i64 %"%resolved_ip.load54", i64 3)
  %calltmp55 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 0)
  %calltmp56 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  %"%resp_buf.load57" = load i64, ptr %"%resp_buf", align 8
  call void @sad_ll_kfree(i64 %"%resp_buf.load57")
  %loadtmp58 = load i64, ptr %"%resolved_ip", align 8
  ret i64 %loadtmp58

while_body_162:                                   ; preds = %while_cond_161
  %"%dns_sock.load26" = load i64, ptr %"%dns_sock", align 8
  %"%resp_buf.load27" = load i64, ptr %"%resp_buf", align 8
  %"%DNS_MAX_RESPONSE.load28" = load i64, ptr @DNS_MAX_RESPONSE, align 8
  call void @udp_recv(i64 %"%dns_sock.load26", i64 %"%resp_buf.load27", i64 %"%DNS_MAX_RESPONSE.load28")
  store i64 0, ptr %"%recv_len", align 8
  %"%recv_len.load" = load i64, ptr %"%recv_len", align 8
  %cmpgttmp29 = icmp sgt i64 %"%recv_len.load", 0
  br i1 %cmpgttmp29, label %then_164, label %merge_165

while_cond_161:                                   ; preds = %merge_165, %merge_160
  %"%attempts.load" = load i64, ptr %"%attempts", align 8
  %cmplttmp25 = icmp slt i64 %"%attempts.load", 100
  br i1 %cmplttmp25, label %while_body_162, label %while_exit_163

while_exit_163:                                   ; preds = %while_cond_161
  %"%resp_buf.load30" = load i64, ptr %"%resp_buf", align 8
  call void @sad_ll_kfree(i64 %"%resp_buf.load30")
  %calltmp31 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %calltmp32 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  %"%dns_errors.load33" = load i64, ptr %"%dns_errors", align 8
  %addtmp34 = add i64 %"%dns_errors.load33", 1
  store i64 %addtmp34, ptr %"%dns_errors", align 8
  ret i64 0
}

define i64 @dhcp_build_discover(i64 %buf) {
entry:
  %"%opt_pos" = alloca i64, align 8
  %"%dhcp_xid" = alloca i64, align 8
  %"%xid" = alloca i64, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  call void @sad_ll_memset(i64 %"%buf.load", i64 0, i64 548)
  %"%buf.load2" = load i64, ptr %buf1, align 8
  call void @sad_ll_mem_write8(i64 %"%buf.load2", i64 1)
  %"%buf.load3" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load3", 1
  call void @sad_ll_mem_write8(i64 %addtmp, i64 1)
  %"%buf.load4" = load i64, ptr %buf1, align 8
  %addtmp5 = add i64 %"%buf.load4", 2
  call void @sad_ll_mem_write8(i64 %addtmp5, i64 6)
  %"%buf.load6" = load i64, ptr %buf1, align 8
  %addtmp7 = add i64 %"%buf.load6", 3
  call void @sad_ll_mem_write8(i64 %addtmp7, i64 0)
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%xid", align 8
  %"%728.load" = load i64, ptr %"%xid", align 8
  store i64 %"%728.load", ptr %"%dhcp_xid", align 8
  %"%buf.load8" = load i64, ptr %buf1, align 8
  %addtmp9 = add i64 %"%buf.load8", 4
  %"%xid.load" = load i64, ptr %"%xid", align 8
  %shrtmp = lshr i64 %"%xid.load", 24
  %andtmp = and i64 %shrtmp, 255
  call void @sad_ll_mem_write8(i64 %addtmp9, i64 %andtmp)
  %"%buf.load10" = load i64, ptr %buf1, align 8
  %addtmp11 = add i64 %"%buf.load10", 5
  %"%xid.load12" = load i64, ptr %"%xid", align 8
  %shrtmp13 = lshr i64 %"%xid.load12", 16
  %andtmp14 = and i64 %shrtmp13, 255
  call void @sad_ll_mem_write8(i64 %addtmp11, i64 %andtmp14)
  %"%buf.load15" = load i64, ptr %buf1, align 8
  %addtmp16 = add i64 %"%buf.load15", 6
  %"%xid.load17" = load i64, ptr %"%xid", align 8
  %shrtmp18 = lshr i64 %"%xid.load17", 8
  %andtmp19 = and i64 %shrtmp18, 255
  call void @sad_ll_mem_write8(i64 %addtmp16, i64 %andtmp19)
  %"%buf.load20" = load i64, ptr %buf1, align 8
  %addtmp21 = add i64 %"%buf.load20", 7
  %"%xid.load22" = load i64, ptr %"%xid", align 8
  %andtmp23 = and i64 %"%xid.load22", 255
  call void @sad_ll_mem_write8(i64 %addtmp21, i64 %andtmp23)
  %"%buf.load24" = load i64, ptr %buf1, align 8
  %addtmp25 = add i64 %"%buf.load24", 8
  call void @sad_ll_mem_write8(i64 %addtmp25, i64 0)
  %"%buf.load26" = load i64, ptr %buf1, align 8
  %addtmp27 = add i64 %"%buf.load26", 9
  call void @sad_ll_mem_write8(i64 %addtmp27, i64 0)
  %"%buf.load28" = load i64, ptr %buf1, align 8
  %addtmp29 = add i64 %"%buf.load28", 10
  call void @sad_ll_mem_write8(i64 %addtmp29, i64 128)
  %"%buf.load30" = load i64, ptr %buf1, align 8
  %addtmp31 = add i64 %"%buf.load30", 11
  call void @sad_ll_mem_write8(i64 %addtmp31, i64 0)
  %"%buf.load32" = load i64, ptr %buf1, align 8
  %addtmp33 = add i64 %"%buf.load32", 28
  %"%774.load" = load i64, ptr @local_mac_0, align 8
  call void @sad_ll_mem_write8(i64 %addtmp33, i64 %"%774.load")
  %"%buf.load34" = load i64, ptr %buf1, align 8
  %addtmp35 = add i64 %"%buf.load34", 29
  %"%779.load" = load i64, ptr @local_mac_1, align 8
  call void @sad_ll_mem_write8(i64 %addtmp35, i64 %"%779.load")
  %"%buf.load36" = load i64, ptr %buf1, align 8
  %addtmp37 = add i64 %"%buf.load36", 30
  %"%784.load" = load i64, ptr @local_mac_2, align 8
  call void @sad_ll_mem_write8(i64 %addtmp37, i64 %"%784.load")
  %"%buf.load38" = load i64, ptr %buf1, align 8
  %addtmp39 = add i64 %"%buf.load38", 31
  %"%789.load" = load i64, ptr @local_mac_3, align 8
  call void @sad_ll_mem_write8(i64 %addtmp39, i64 %"%789.load")
  %"%buf.load40" = load i64, ptr %buf1, align 8
  %addtmp41 = add i64 %"%buf.load40", 32
  %"%794.load" = load i64, ptr @local_mac_4, align 8
  call void @sad_ll_mem_write8(i64 %addtmp41, i64 %"%794.load")
  %"%buf.load42" = load i64, ptr %buf1, align 8
  %addtmp43 = add i64 %"%buf.load42", 33
  %"%799.load" = load i64, ptr @local_mac_5, align 8
  call void @sad_ll_mem_write8(i64 %addtmp43, i64 %"%799.load")
  %"%buf.load44" = load i64, ptr %buf1, align 8
  %addtmp45 = add i64 %"%buf.load44", 236
  %"%DHCP_MAGIC_1.load" = load i64, ptr @DHCP_MAGIC_1, align 8
  call void @sad_ll_mem_write8(i64 %addtmp45, i64 %"%DHCP_MAGIC_1.load")
  %"%buf.load46" = load i64, ptr %buf1, align 8
  %addtmp47 = add i64 %"%buf.load46", 237
  %"%DHCP_MAGIC_2.load" = load i64, ptr @DHCP_MAGIC_2, align 8
  call void @sad_ll_mem_write8(i64 %addtmp47, i64 %"%DHCP_MAGIC_2.load")
  %"%buf.load48" = load i64, ptr %buf1, align 8
  %addtmp49 = add i64 %"%buf.load48", 238
  %"%DHCP_MAGIC_3.load" = load i64, ptr @DHCP_MAGIC_3, align 8
  call void @sad_ll_mem_write8(i64 %addtmp49, i64 %"%DHCP_MAGIC_3.load")
  %"%buf.load50" = load i64, ptr %buf1, align 8
  %addtmp51 = add i64 %"%buf.load50", 239
  %"%DHCP_MAGIC_4.load" = load i64, ptr @DHCP_MAGIC_4, align 8
  call void @sad_ll_mem_write8(i64 %addtmp51, i64 %"%DHCP_MAGIC_4.load")
  store i64 240, ptr %"%opt_pos", align 8
  %"%buf.load52" = load i64, ptr %buf1, align 8
  %"%opt_pos.load" = load i64, ptr %"%opt_pos", align 8
  %addtmp53 = add i64 %"%buf.load52", %"%opt_pos.load"
  %"%DHCP_OPT_MSGTYPE.load" = load i64, ptr @DHCP_OPT_MSGTYPE, align 8
  call void @sad_ll_mem_write8(i64 %addtmp53, i64 %"%DHCP_OPT_MSGTYPE.load")
  %addtmp54 = add i64 %addtmp53, 1
  call void @sad_ll_mem_write8(i64 %addtmp54, i64 1)
  %addtmp55 = add i64 %addtmp53, 2
  %"%DHCP_DISCOVER.load" = load i64, ptr @DHCP_DISCOVER, align 8
  call void @sad_ll_mem_write8(i64 %addtmp55, i64 %"%DHCP_DISCOVER.load")
  %"%opt_pos.load56" = load i64, ptr %"%opt_pos", align 8
  %addtmp57 = add i64 %"%opt_pos.load56", 3
  store i64 %addtmp57, ptr %"%opt_pos", align 8
  %"%DHCP_OPT_PARAMLIST.load" = load i64, ptr @DHCP_OPT_PARAMLIST, align 8
  call void @sad_ll_mem_write8(i64 %addtmp53, i64 %"%DHCP_OPT_PARAMLIST.load")
  call void @sad_ll_mem_write8(i64 %addtmp54, i64 3)
  %"%DHCP_OPT_SUBNET.load" = load i64, ptr @DHCP_OPT_SUBNET, align 8
  call void @sad_ll_mem_write8(i64 %addtmp55, i64 %"%DHCP_OPT_SUBNET.load")
  %addtmp58 = add i64 %addtmp53, 3
  %"%DHCP_OPT_ROUTER.load" = load i64, ptr @DHCP_OPT_ROUTER, align 8
  call void @sad_ll_mem_write8(i64 %addtmp58, i64 %"%DHCP_OPT_ROUTER.load")
  %addtmp59 = add i64 %addtmp53, 4
  %"%DHCP_OPT_DNS.load" = load i64, ptr @DHCP_OPT_DNS, align 8
  call void @sad_ll_mem_write8(i64 %addtmp59, i64 %"%DHCP_OPT_DNS.load")
  %"%opt_pos.load60" = load i64, ptr %"%opt_pos", align 8
  %addtmp61 = add i64 %"%opt_pos.load60", 5
  store i64 %addtmp61, ptr %"%opt_pos", align 8
  %"%DHCP_OPT_END.load" = load i64, ptr @DHCP_OPT_END, align 8
  call void @sad_ll_mem_write8(i64 %addtmp53, i64 %"%DHCP_OPT_END.load")
  %"%opt_pos.load62" = load i64, ptr %"%opt_pos", align 8
  %addtmp63 = add i64 %"%opt_pos.load62", 1
  store i64 %addtmp63, ptr %"%opt_pos", align 8
  %loadtmp = load i64, ptr %"%opt_pos", align 8
  ret i64 %loadtmp
}

define i64 @dhcp_build_request(i64 %buf, i64 %offered_ip, i64 %server_ip) {
entry:
  %"%opt_pos" = alloca i64, align 8
  %"%pkt_len" = alloca i64, align 8
  %server_ip3 = alloca i64, align 8
  store i64 %server_ip, ptr %server_ip3, align 8
  %offered_ip2 = alloca i64, align 8
  store i64 %offered_ip, ptr %offered_ip2, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  %calltmp = call i64 @dhcp_build_discover(i64 %"%buf.load")
  store i64 %calltmp, ptr %"%pkt_len", align 8
  %"%buf.load4" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load4", 4
  %"%dhcp_xid.load" = load i64, ptr @dhcp_xid, align 8
  %shrtmp = lshr i64 %"%dhcp_xid.load", 24
  %andtmp = and i64 %shrtmp, 255
  call void @sad_ll_mem_write8(i64 %addtmp, i64 %andtmp)
  %"%buf.load5" = load i64, ptr %buf1, align 8
  %addtmp6 = add i64 %"%buf.load5", 5
  %"%dhcp_xid.load7" = load i64, ptr @dhcp_xid, align 8
  %shrtmp8 = lshr i64 %"%dhcp_xid.load7", 16
  %andtmp9 = and i64 %shrtmp8, 255
  call void @sad_ll_mem_write8(i64 %addtmp6, i64 %andtmp9)
  %"%buf.load10" = load i64, ptr %buf1, align 8
  %addtmp11 = add i64 %"%buf.load10", 6
  %"%dhcp_xid.load12" = load i64, ptr @dhcp_xid, align 8
  %shrtmp13 = lshr i64 %"%dhcp_xid.load12", 8
  %andtmp14 = and i64 %shrtmp13, 255
  call void @sad_ll_mem_write8(i64 %addtmp11, i64 %andtmp14)
  %"%buf.load15" = load i64, ptr %buf1, align 8
  %addtmp16 = add i64 %"%buf.load15", 7
  %"%dhcp_xid.load17" = load i64, ptr @dhcp_xid, align 8
  %andtmp18 = and i64 %"%dhcp_xid.load17", 255
  call void @sad_ll_mem_write8(i64 %addtmp16, i64 %andtmp18)
  store i64 240, ptr %"%opt_pos", align 8
  %"%buf.load19" = load i64, ptr %buf1, align 8
  %"%opt_pos.load" = load i64, ptr %"%opt_pos", align 8
  %addtmp20 = add i64 %"%buf.load19", %"%opt_pos.load"
  %"%DHCP_OPT_MSGTYPE.load" = load i64, ptr @DHCP_OPT_MSGTYPE, align 8
  call void @sad_ll_mem_write8(i64 %addtmp20, i64 %"%DHCP_OPT_MSGTYPE.load")
  %addtmp21 = add i64 %addtmp20, 1
  call void @sad_ll_mem_write8(i64 %addtmp21, i64 1)
  %addtmp22 = add i64 %addtmp20, 2
  %"%DHCP_REQUEST.load" = load i64, ptr @DHCP_REQUEST, align 8
  call void @sad_ll_mem_write8(i64 %addtmp22, i64 %"%DHCP_REQUEST.load")
  %"%opt_pos.load23" = load i64, ptr %"%opt_pos", align 8
  %addtmp24 = add i64 %"%opt_pos.load23", 3
  store i64 %addtmp24, ptr %"%opt_pos", align 8
  %"%DHCP_OPT_REQIP.load" = load i64, ptr @DHCP_OPT_REQIP, align 8
  call void @sad_ll_mem_write8(i64 %addtmp20, i64 %"%DHCP_OPT_REQIP.load")
  call void @sad_ll_mem_write8(i64 %addtmp21, i64 4)
  %"%offered_ip.load" = load i64, ptr %offered_ip2, align 8
  call void @ip_byte(i64 %"%offered_ip.load", i64 0)
  call void @sad_ll_mem_write8(i64 %addtmp22, i64 0)
  %addtmp25 = add i64 %addtmp20, 3
  %"%offered_ip.load26" = load i64, ptr %offered_ip2, align 8
  call void @ip_byte(i64 %"%offered_ip.load26", i64 1)
  call void @sad_ll_mem_write8(i64 %addtmp25, i64 0)
  %addtmp27 = add i64 %addtmp20, 4
  %"%offered_ip.load28" = load i64, ptr %offered_ip2, align 8
  call void @ip_byte(i64 %"%offered_ip.load28", i64 2)
  call void @sad_ll_mem_write8(i64 %addtmp27, i64 0)
  %addtmp29 = add i64 %addtmp20, 5
  %"%offered_ip.load30" = load i64, ptr %offered_ip2, align 8
  call void @ip_byte(i64 %"%offered_ip.load30", i64 3)
  call void @sad_ll_mem_write8(i64 %addtmp29, i64 0)
  %"%opt_pos.load31" = load i64, ptr %"%opt_pos", align 8
  %addtmp32 = add i64 %"%opt_pos.load31", 6
  store i64 %addtmp32, ptr %"%opt_pos", align 8
  %"%DHCP_OPT_SERVER.load" = load i64, ptr @DHCP_OPT_SERVER, align 8
  call void @sad_ll_mem_write8(i64 %addtmp20, i64 %"%DHCP_OPT_SERVER.load")
  call void @sad_ll_mem_write8(i64 %addtmp21, i64 4)
  %"%server_ip.load" = load i64, ptr %server_ip3, align 8
  call void @ip_byte(i64 %"%server_ip.load", i64 0)
  call void @sad_ll_mem_write8(i64 %addtmp22, i64 0)
  %"%server_ip.load33" = load i64, ptr %server_ip3, align 8
  call void @ip_byte(i64 %"%server_ip.load33", i64 1)
  call void @sad_ll_mem_write8(i64 %addtmp25, i64 0)
  %"%server_ip.load34" = load i64, ptr %server_ip3, align 8
  call void @ip_byte(i64 %"%server_ip.load34", i64 2)
  call void @sad_ll_mem_write8(i64 %addtmp27, i64 0)
  %"%server_ip.load35" = load i64, ptr %server_ip3, align 8
  call void @ip_byte(i64 %"%server_ip.load35", i64 3)
  call void @sad_ll_mem_write8(i64 %addtmp29, i64 0)
  store i64 %addtmp32, ptr %"%opt_pos", align 8
  %"%DHCP_OPT_END.load" = load i64, ptr @DHCP_OPT_END, align 8
  call void @sad_ll_mem_write8(i64 %addtmp20, i64 %"%DHCP_OPT_END.load")
  %"%opt_pos.load36" = load i64, ptr %"%opt_pos", align 8
  %addtmp37 = add i64 %"%opt_pos.load36", 1
  store i64 %addtmp37, ptr %"%opt_pos", align 8
  %loadtmp = load i64, ptr %"%opt_pos", align 8
  ret i64 %loadtmp
}

define i64 @dhcp_parse_response(i64 %buf, i64 %len) {
entry:
  %"%dhcp_lease_time" = alloca i64, align 8
  %"%t3" = alloca i64, align 8
  %"%t2" = alloca i64, align 8
  %"%t1" = alloca i64, align 8
  %"%t0" = alloca i64, align 8
  %"%dhcp_server_ip" = alloca i64, align 8
  %"%sv3" = alloca i64, align 8
  %"%sv2" = alloca i64, align 8
  %"%sv1" = alloca i64, align 8
  %"%sv0" = alloca i64, align 8
  %"%dns_server_3" = alloca i64, align 8
  %"%dns_server_2" = alloca i64, align 8
  %"%dns_server_1" = alloca i64, align 8
  %"%dns_server_0" = alloca i64, align 8
  %"%g3" = alloca i64, align 8
  %"%g2" = alloca i64, align 8
  %"%g1" = alloca i64, align 8
  %"%g0" = alloca i64, align 8
  %"%s3" = alloca i64, align 8
  %"%s2" = alloca i64, align 8
  %"%s1" = alloca i64, align 8
  %"%s0" = alloca i64, align 8
  %"%opt_len" = alloca i64, align 8
  %"%opt_code" = alloca i64, align 8
  %"%msg_type" = alloca i64, align 8
  %"%pos" = alloca i64, align 8
  %"%dhcp_offered_ip" = alloca i64, align 8
  %"%y3" = alloca i64, align 8
  %"%y2" = alloca i64, align 8
  %"%y1" = alloca i64, align 8
  %"%y0" = alloca i64, align 8
  %"%m2" = alloca i64, align 8
  %"%m1" = alloca i64, align 8
  %"%op" = alloca i64, align 8
  %len2 = alloca i64, align 8
  store i64 %len, ptr %len2, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%len.load" = load i64, ptr %len2, align 8
  %cmplttmp = icmp slt i64 %"%len.load", 240
  br i1 %cmplttmp, label %then_168, label %merge_169

merge_169:                                        ; preds = %entry
  %"%buf.load" = load i64, ptr %buf1, align 8
  call void @sad_ll_mem_read8(i64 %"%buf.load")
  store i64 0, ptr %"%op", align 8
  %"%op.load" = load i64, ptr %"%op", align 8
  %cmpnetmp = icmp ne i64 %"%op.load", 2
  br i1 %cmpnetmp, label %then_170, label %merge_171

merge_171:                                        ; preds = %merge_169
  %"%buf.load3" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load3", 236
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%m1", align 8
  %"%buf.load4" = load i64, ptr %buf1, align 8
  %addtmp5 = add i64 %"%buf.load4", 237
  call void @sad_ll_mem_read8(i64 %addtmp5)
  store i64 0, ptr %"%m2", align 8
  %"%m1.load" = load i64, ptr %"%m1", align 8
  %"%DHCP_MAGIC_1.load" = load i64, ptr @DHCP_MAGIC_1, align 8
  %cmpnetmp6 = icmp ne i64 %"%m1.load", %"%DHCP_MAGIC_1.load"
  br i1 %cmpnetmp6, label %then_172, label %merge_173

merge_173:                                        ; preds = %merge_171
  %"%m2.load" = load i64, ptr %"%m2", align 8
  %"%DHCP_MAGIC_2.load" = load i64, ptr @DHCP_MAGIC_2, align 8
  %cmpnetmp7 = icmp ne i64 %"%m2.load", %"%DHCP_MAGIC_2.load"
  br i1 %cmpnetmp7, label %then_174, label %merge_175

merge_175:                                        ; preds = %merge_173
  %"%buf.load8" = load i64, ptr %buf1, align 8
  %addtmp9 = add i64 %"%buf.load8", 16
  call void @sad_ll_mem_read8(i64 %addtmp9)
  store i64 0, ptr %"%y0", align 8
  %"%buf.load10" = load i64, ptr %buf1, align 8
  %addtmp11 = add i64 %"%buf.load10", 17
  call void @sad_ll_mem_read8(i64 %addtmp11)
  store i64 0, ptr %"%y1", align 8
  %"%buf.load12" = load i64, ptr %buf1, align 8
  %addtmp13 = add i64 %"%buf.load12", 18
  call void @sad_ll_mem_read8(i64 %addtmp13)
  store i64 0, ptr %"%y2", align 8
  %"%buf.load14" = load i64, ptr %buf1, align 8
  %addtmp15 = add i64 %"%buf.load14", 19
  call void @sad_ll_mem_read8(i64 %addtmp15)
  store i64 0, ptr %"%y3", align 8
  %"%y0.load" = load i64, ptr %"%y0", align 8
  %"%y1.load" = load i64, ptr %"%y1", align 8
  %"%y2.load" = load i64, ptr %"%y2", align 8
  %"%y3.load" = load i64, ptr %"%y3", align 8
  call void @ip_to_int(i64 %"%y0.load", i64 %"%y1.load", i64 %"%y2.load", i64 %"%y3.load")
  store i64 0, ptr %"%dhcp_offered_ip", align 8
  store i64 240, ptr %"%pos", align 8
  store i64 0, ptr %"%msg_type", align 8
  br label %while_cond_176

merge_180:                                        ; preds = %while_body_177
  %"%opt_code.load22" = load i64, ptr %"%opt_code", align 8
  %cmpeqtmp23 = icmp eq i64 %"%opt_code.load22", 0
  br i1 %cmpeqtmp23, label %then_181, label %merge_182

merge_182:                                        ; preds = %then_181, %merge_180
  %"%buf.load26" = load i64, ptr %buf1, align 8
  %"%pos.load27" = load i64, ptr %"%pos", align 8
  %addtmp28 = add i64 %"%buf.load26", %"%pos.load27"
  %addtmp29 = add i64 %addtmp28, 1
  call void @sad_ll_mem_read8(i64 %addtmp29)
  store i64 0, ptr %"%opt_len", align 8
  %"%opt_code.load30" = load i64, ptr %"%opt_code", align 8
  %"%DHCP_OPT_MSGTYPE.load" = load i64, ptr @DHCP_OPT_MSGTYPE, align 8
  %cmpeqtmp31 = icmp eq i64 %"%opt_code.load30", %"%DHCP_OPT_MSGTYPE.load"
  br i1 %cmpeqtmp31, label %then_183, label %merge_184

merge_184:                                        ; preds = %then_183, %merge_182
  %"%opt_code.load36" = load i64, ptr %"%opt_code", align 8
  %"%DHCP_OPT_SUBNET.load" = load i64, ptr @DHCP_OPT_SUBNET, align 8
  %cmpeqtmp37 = icmp eq i64 %"%opt_code.load36", %"%DHCP_OPT_SUBNET.load"
  br i1 %cmpeqtmp37, label %then_185, label %merge_186

merge_186:                                        ; preds = %merge_188, %merge_184
  %"%opt_code.load39" = load i64, ptr %"%opt_code", align 8
  %"%DHCP_OPT_ROUTER.load" = load i64, ptr @DHCP_OPT_ROUTER, align 8
  %cmpeqtmp40 = icmp eq i64 %"%opt_code.load39", %"%DHCP_OPT_ROUTER.load"
  br i1 %cmpeqtmp40, label %then_189, label %merge_190

merge_188:                                        ; preds = %then_187, %then_185
  br label %merge_186

merge_190:                                        ; preds = %merge_192, %merge_186
  %"%opt_code.load49" = load i64, ptr %"%opt_code", align 8
  %"%DHCP_OPT_DNS.load" = load i64, ptr @DHCP_OPT_DNS, align 8
  %cmpeqtmp50 = icmp eq i64 %"%opt_code.load49", %"%DHCP_OPT_DNS.load"
  br i1 %cmpeqtmp50, label %then_193, label %merge_194

merge_192:                                        ; preds = %then_191, %then_189
  br label %merge_190

merge_194:                                        ; preds = %merge_196, %merge_190
  %"%opt_code.load60" = load i64, ptr %"%opt_code", align 8
  %"%DHCP_OPT_SERVER.load" = load i64, ptr @DHCP_OPT_SERVER, align 8
  %cmpeqtmp61 = icmp eq i64 %"%opt_code.load60", %"%DHCP_OPT_SERVER.load"
  br i1 %cmpeqtmp61, label %then_197, label %merge_198

merge_196:                                        ; preds = %then_195, %then_193
  br label %merge_194

merge_198:                                        ; preds = %merge_200, %merge_194
  %"%opt_code.load71" = load i64, ptr %"%opt_code", align 8
  %"%DHCP_OPT_LEASE.load" = load i64, ptr @DHCP_OPT_LEASE, align 8
  %cmpeqtmp72 = icmp eq i64 %"%opt_code.load71", %"%DHCP_OPT_LEASE.load"
  br i1 %cmpeqtmp72, label %then_201, label %merge_202

merge_200:                                        ; preds = %then_199, %then_197
  br label %merge_198

merge_202:                                        ; preds = %merge_204, %merge_198
  %"%pos.load82" = load i64, ptr %"%pos", align 8
  %addtmp83 = add i64 %"%pos.load82", 2
  %"%opt_len.load84" = load i64, ptr %"%opt_len", align 8
  %addtmp85 = add i64 %addtmp83, %"%opt_len.load84"
  store i64 %addtmp85, ptr %"%pos", align 8
  br label %while_cond_176

merge_204:                                        ; preds = %then_203, %then_201
  br label %merge_202

then_168:                                         ; preds = %entry
  ret i64 0

then_170:                                         ; preds = %merge_169
  ret i64 0

then_172:                                         ; preds = %merge_171
  ret i64 0

then_174:                                         ; preds = %merge_173
  ret i64 0

then_179:                                         ; preds = %while_body_177
  %loadtmp21 = load i64, ptr %"%msg_type", align 8
  ret i64 %loadtmp21

then_181:                                         ; preds = %merge_180
  %"%pos.load24" = load i64, ptr %"%pos", align 8
  %addtmp25 = add i64 %"%pos.load24", 1
  store i64 %addtmp25, ptr %"%pos", align 8
  br label %merge_182

then_183:                                         ; preds = %merge_182
  %"%buf.load32" = load i64, ptr %buf1, align 8
  %"%pos.load33" = load i64, ptr %"%pos", align 8
  %addtmp34 = add i64 %"%buf.load32", %"%pos.load33"
  %addtmp35 = add i64 %addtmp34, 2
  call void @sad_ll_mem_read8(i64 %addtmp35)
  store i64 0, ptr %"%msg_type", align 8
  br label %merge_184

then_185:                                         ; preds = %merge_184
  %"%opt_len.load" = load i64, ptr %"%opt_len", align 8
  %cmpeqtmp38 = icmp eq i64 %"%opt_len.load", 4
  br i1 %cmpeqtmp38, label %then_187, label %merge_188

then_187:                                         ; preds = %then_185
  %"%buf.load41" = load i64, ptr %buf1, align 8
  %"%pos.load42" = load i64, ptr %"%pos", align 8
  %addtmp43 = add i64 %"%buf.load41", %"%pos.load42"
  %addtmp44 = add i64 %addtmp43, 2
  call void @sad_ll_mem_read8(i64 %addtmp44)
  store i64 0, ptr %"%s0", align 8
  %addtmp45 = add i64 %addtmp43, 3
  call void @sad_ll_mem_read8(i64 %addtmp45)
  store i64 0, ptr %"%s1", align 8
  %addtmp46 = add i64 %addtmp43, 4
  call void @sad_ll_mem_read8(i64 %addtmp46)
  store i64 0, ptr %"%s2", align 8
  %addtmp47 = add i64 %addtmp43, 5
  call void @sad_ll_mem_read8(i64 %addtmp47)
  store i64 0, ptr %"%s3", align 8
  %"%s0.load" = load i64, ptr %"%s0", align 8
  %"%s1.load" = load i64, ptr %"%s1", align 8
  %"%s2.load" = load i64, ptr %"%s2", align 8
  %"%s3.load" = load i64, ptr %"%s3", align 8
  call void @"ip_\D8\B9\D9\8A\D9\86_\D9\82\D9\86\D8\A7\D8\B9"(i64 %"%s0.load", i64 %"%s1.load", i64 %"%s2.load", i64 %"%s3.load")
  br label %merge_188

then_189:                                         ; preds = %merge_186
  %"%opt_len.load48" = load i64, ptr %"%opt_len", align 8
  %cmpgetmp = icmp sge i64 %"%opt_len.load48", 4
  br i1 %cmpgetmp, label %then_191, label %merge_192

then_191:                                         ; preds = %then_189
  %"%buf.load51" = load i64, ptr %buf1, align 8
  %"%pos.load52" = load i64, ptr %"%pos", align 8
  %addtmp53 = add i64 %"%buf.load51", %"%pos.load52"
  %addtmp54 = add i64 %addtmp53, 2
  call void @sad_ll_mem_read8(i64 %addtmp54)
  store i64 0, ptr %"%g0", align 8
  %addtmp55 = add i64 %addtmp53, 3
  call void @sad_ll_mem_read8(i64 %addtmp55)
  store i64 0, ptr %"%g1", align 8
  %addtmp56 = add i64 %addtmp53, 4
  call void @sad_ll_mem_read8(i64 %addtmp56)
  store i64 0, ptr %"%g2", align 8
  %addtmp57 = add i64 %addtmp53, 5
  call void @sad_ll_mem_read8(i64 %addtmp57)
  store i64 0, ptr %"%g3", align 8
  %"%g0.load" = load i64, ptr %"%g0", align 8
  %"%g1.load" = load i64, ptr %"%g1", align 8
  %"%g2.load" = load i64, ptr %"%g2", align 8
  %"%g3.load" = load i64, ptr %"%g3", align 8
  call void @"ip_\D8\B9\D9\8A\D9\86_\D8\A8\D9\88\D8\A7\D8\A8\D8\A9"(i64 %"%g0.load", i64 %"%g1.load", i64 %"%g2.load", i64 %"%g3.load")
  br label %merge_192

then_193:                                         ; preds = %merge_190
  %"%opt_len.load58" = load i64, ptr %"%opt_len", align 8
  %cmpgetmp59 = icmp sge i64 %"%opt_len.load58", 4
  br i1 %cmpgetmp59, label %then_195, label %merge_196

then_195:                                         ; preds = %then_193
  %"%buf.load62" = load i64, ptr %buf1, align 8
  %"%pos.load63" = load i64, ptr %"%pos", align 8
  %addtmp64 = add i64 %"%buf.load62", %"%pos.load63"
  %addtmp65 = add i64 %addtmp64, 2
  call void @sad_ll_mem_read8(i64 %addtmp65)
  store i64 0, ptr %"%dns_server_0", align 8
  %addtmp66 = add i64 %addtmp64, 3
  call void @sad_ll_mem_read8(i64 %addtmp66)
  store i64 0, ptr %"%dns_server_1", align 8
  %addtmp67 = add i64 %addtmp64, 4
  call void @sad_ll_mem_read8(i64 %addtmp67)
  store i64 0, ptr %"%dns_server_2", align 8
  %addtmp68 = add i64 %addtmp64, 5
  call void @sad_ll_mem_read8(i64 %addtmp68)
  store i64 0, ptr %"%dns_server_3", align 8
  br label %merge_196

then_197:                                         ; preds = %merge_194
  %"%opt_len.load69" = load i64, ptr %"%opt_len", align 8
  %cmpeqtmp70 = icmp eq i64 %"%opt_len.load69", 4
  br i1 %cmpeqtmp70, label %then_199, label %merge_200

then_199:                                         ; preds = %then_197
  %"%buf.load73" = load i64, ptr %buf1, align 8
  %"%pos.load74" = load i64, ptr %"%pos", align 8
  %addtmp75 = add i64 %"%buf.load73", %"%pos.load74"
  %addtmp76 = add i64 %addtmp75, 2
  call void @sad_ll_mem_read8(i64 %addtmp76)
  store i64 0, ptr %"%sv0", align 8
  %addtmp77 = add i64 %addtmp75, 3
  call void @sad_ll_mem_read8(i64 %addtmp77)
  store i64 0, ptr %"%sv1", align 8
  %addtmp78 = add i64 %addtmp75, 4
  call void @sad_ll_mem_read8(i64 %addtmp78)
  store i64 0, ptr %"%sv2", align 8
  %addtmp79 = add i64 %addtmp75, 5
  call void @sad_ll_mem_read8(i64 %addtmp79)
  store i64 0, ptr %"%sv3", align 8
  %"%sv0.load" = load i64, ptr %"%sv0", align 8
  %"%sv1.load" = load i64, ptr %"%sv1", align 8
  %"%sv2.load" = load i64, ptr %"%sv2", align 8
  %"%sv3.load" = load i64, ptr %"%sv3", align 8
  call void @ip_to_int(i64 %"%sv0.load", i64 %"%sv1.load", i64 %"%sv2.load", i64 %"%sv3.load")
  store i64 0, ptr %"%dhcp_server_ip", align 8
  br label %merge_200

then_201:                                         ; preds = %merge_198
  %"%opt_len.load80" = load i64, ptr %"%opt_len", align 8
  %cmpeqtmp81 = icmp eq i64 %"%opt_len.load80", 4
  br i1 %cmpeqtmp81, label %then_203, label %merge_204

then_203:                                         ; preds = %then_201
  %"%buf.load86" = load i64, ptr %buf1, align 8
  %"%pos.load87" = load i64, ptr %"%pos", align 8
  %addtmp88 = add i64 %"%buf.load86", %"%pos.load87"
  %addtmp89 = add i64 %addtmp88, 2
  call void @sad_ll_mem_read8(i64 %addtmp89)
  store i64 0, ptr %"%t0", align 8
  %addtmp90 = add i64 %addtmp88, 3
  call void @sad_ll_mem_read8(i64 %addtmp90)
  store i64 0, ptr %"%t1", align 8
  %addtmp91 = add i64 %addtmp88, 4
  call void @sad_ll_mem_read8(i64 %addtmp91)
  store i64 0, ptr %"%t2", align 8
  %addtmp92 = add i64 %addtmp88, 5
  call void @sad_ll_mem_read8(i64 %addtmp92)
  store i64 0, ptr %"%t3", align 8
  %"%t0.load" = load i64, ptr %"%t0", align 8
  %shltmp = shl i64 %"%t0.load", 24
  %"%t1.load" = load i64, ptr %"%t1", align 8
  %shltmp93 = shl i64 %"%t1.load", 16
  %addtmp94 = add i64 %shltmp, %shltmp93
  %"%t2.load" = load i64, ptr %"%t2", align 8
  %shltmp95 = shl i64 %"%t2.load", 8
  %addtmp96 = add i64 %addtmp94, %shltmp95
  %"%t3.load" = load i64, ptr %"%t3", align 8
  %addtmp97 = add i64 %addtmp96, %"%t3.load"
  store i64 %addtmp97, ptr %"%dhcp_lease_time", align 8
  br label %merge_204

while_body_177:                                   ; preds = %while_cond_176
  %"%buf.load18" = load i64, ptr %buf1, align 8
  %"%pos.load19" = load i64, ptr %"%pos", align 8
  %addtmp20 = add i64 %"%buf.load18", %"%pos.load19"
  call void @sad_ll_mem_read8(i64 %addtmp20)
  store i64 0, ptr %"%opt_code", align 8
  %"%opt_code.load" = load i64, ptr %"%opt_code", align 8
  %"%DHCP_OPT_END.load" = load i64, ptr @DHCP_OPT_END, align 8
  %cmpeqtmp = icmp eq i64 %"%opt_code.load", %"%DHCP_OPT_END.load"
  br i1 %cmpeqtmp, label %then_179, label %merge_180

while_cond_176:                                   ; preds = %merge_202, %merge_175
  %"%pos.load" = load i64, ptr %"%pos", align 8
  %"%len.load16" = load i64, ptr %len2, align 8
  %cmplttmp17 = icmp slt i64 %"%pos.load", %"%len.load16"
  br i1 %cmplttmp17, label %while_body_177, label %while_exit_178

while_exit_178:                                   ; preds = %while_cond_176
  %loadtmp = load i64, ptr %"%msg_type", align 8
  ret i64 %loadtmp
}

define i64 @dhcp_discover() {
entry:
  %"%dhcp_discovers" = alloca i64, align 8
  %"%dhcp_state" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%pkt_len" = alloca i64, align 8
  %"%pkt_buf" = alloca i64, align 8
  %"%dhcp_sock" = alloca i64, align 8
  %calltmp = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %calltmp1 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  %"%dhcp_sock.load" = load i64, ptr @dhcp_sock, align 8
  %cmpeqtmp = icmp eq i64 %"%dhcp_sock.load", 0
  br i1 %cmpeqtmp, label %then_205, label %merge_206

else_213:                                         ; preds = %merge_210
  %calltmp13 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %calltmp14 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_212

merge_206:                                        ; preds = %merge_208, %entry
  call void @sad_ll_kmalloc(i64 548)
  store i64 0, ptr %"%pkt_buf", align 8
  %"%pkt_buf.load" = load i64, ptr %"%pkt_buf", align 8
  %cmpeqtmp3 = icmp eq i64 %"%pkt_buf.load", 0
  br i1 %cmpeqtmp3, label %then_209, label %merge_210

merge_208:                                        ; preds = %then_205
  br label %merge_206

merge_210:                                        ; preds = %merge_206
  %"%pkt_buf.load6" = load i64, ptr %"%pkt_buf", align 8
  %calltmp7 = call i64 @dhcp_build_discover(i64 %"%pkt_buf.load6")
  store i64 %calltmp7, ptr %"%pkt_len", align 8
  %"%dhcp_sock.load8" = load i64, ptr %"%dhcp_sock", align 8
  %"%DHCP_SERVER_PORT.load" = load i64, ptr @DHCP_SERVER_PORT, align 8
  %"%pkt_buf.load9" = load i64, ptr %"%pkt_buf", align 8
  %"%pkt_len.load" = load i64, ptr %"%pkt_len", align 8
  call void @udp_broadcast(i64 %"%dhcp_sock.load8", i64 %"%DHCP_SERVER_PORT.load", i64 %"%pkt_buf.load9", i64 %"%pkt_len.load")
  store i64 0, ptr %"%result", align 8
  %"%pkt_buf.load10" = load i64, ptr %"%pkt_buf", align 8
  call void @sad_ll_kfree(i64 %"%pkt_buf.load10")
  %"%result.load" = load i64, ptr %"%result", align 8
  %cmpgttmp = icmp sgt i64 %"%result.load", 0
  br i1 %cmpgttmp, label %then_211, label %else_213

merge_212:                                        ; preds = %else_213, %then_211
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

then_205:                                         ; preds = %entry
  %"%DHCP_CLIENT_PORT.load" = load i64, ptr @DHCP_CLIENT_PORT, align 8
  call void @udp_open(i64 %"%DHCP_CLIENT_PORT.load")
  store i64 0, ptr %"%dhcp_sock", align 8
  %"%dhcp_sock.load2" = load i64, ptr %"%dhcp_sock", align 8
  %cmplttmp = icmp slt i64 %"%dhcp_sock.load2", 0
  br i1 %cmplttmp, label %then_207, label %merge_208

then_207:                                         ; preds = %then_205
  %calltmp4 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  %calltmp5 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_209:                                         ; preds = %merge_206
  ret i64 0

then_211:                                         ; preds = %merge_210
  store i64 1, ptr %"%dhcp_state", align 8
  %"%dhcp_discovers.load" = load i64, ptr @dhcp_discovers, align 8
  %addtmp = add i64 %"%dhcp_discovers.load", 1
  store i64 %addtmp, ptr %"%dhcp_discovers", align 8
  %calltmp11 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %calltmp12 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_212
}

define i64 @dhcp_handle_response() {
entry:
  %"%dhcp_naks" = alloca i64, align 8
  %"%dhcp_state" = alloca i64, align 8
  %"%dhcp_acks" = alloca i64, align 8
  %"%dhcp_offers" = alloca i64, align 8
  %"%msg_type" = alloca i64, align 8
  %"%recv_len" = alloca i64, align 8
  %"%resp_buf" = alloca i64, align 8
  %"%dhcp_sock.load" = load i64, ptr @dhcp_sock, align 8
  %cmplttmp = icmp slt i64 %"%dhcp_sock.load", 0
  br i1 %cmplttmp, label %then_214, label %merge_215

merge_215:                                        ; preds = %entry
  call void @sad_ll_kmalloc(i64 548)
  store i64 0, ptr %"%resp_buf", align 8
  %"%resp_buf.load" = load i64, ptr %"%resp_buf", align 8
  %cmpeqtmp = icmp eq i64 %"%resp_buf.load", 0
  br i1 %cmpeqtmp, label %then_216, label %merge_217

merge_217:                                        ; preds = %merge_215
  %"%dhcp_sock.load1" = load i64, ptr @dhcp_sock, align 8
  %"%resp_buf.load2" = load i64, ptr %"%resp_buf", align 8
  call void @udp_recv(i64 %"%dhcp_sock.load1", i64 %"%resp_buf.load2", i64 548)
  store i64 0, ptr %"%recv_len", align 8
  %"%recv_len.load" = load i64, ptr %"%recv_len", align 8
  %cmpletmp = icmp sle i64 %"%recv_len.load", 0
  br i1 %cmpletmp, label %then_218, label %merge_219

merge_219:                                        ; preds = %merge_217
  %"%resp_buf.load4" = load i64, ptr %"%resp_buf", align 8
  %"%recv_len.load5" = load i64, ptr %"%recv_len", align 8
  %calltmp = call i64 @dhcp_parse_response(i64 %"%resp_buf.load4", i64 %"%recv_len.load5")
  store i64 %calltmp, ptr %"%msg_type", align 8
  %"%resp_buf.load6" = load i64, ptr %"%resp_buf", align 8
  call void @sad_ll_kfree(i64 %"%resp_buf.load6")
  %"%msg_type.load" = load i64, ptr %"%msg_type", align 8
  %"%DHCP_OFFER.load" = load i64, ptr @DHCP_OFFER, align 8
  %cmpeqtmp7 = icmp eq i64 %"%msg_type.load", %"%DHCP_OFFER.load"
  br i1 %cmpeqtmp7, label %then_220, label %merge_221

merge_221:                                        ; preds = %merge_219
  %"%msg_type.load20" = load i64, ptr %"%msg_type", align 8
  %"%DHCP_ACK.load" = load i64, ptr @DHCP_ACK, align 8
  %cmpeqtmp21 = icmp eq i64 %"%msg_type.load20", %"%DHCP_ACK.load"
  br i1 %cmpeqtmp21, label %then_222, label %merge_223

merge_223:                                        ; preds = %merge_221
  %"%msg_type.load42" = load i64, ptr %"%msg_type", align 8
  %"%DHCP_NAK.load" = load i64, ptr @DHCP_NAK, align 8
  %cmpeqtmp43 = icmp eq i64 %"%msg_type.load42", %"%DHCP_NAK.load"
  br i1 %cmpeqtmp43, label %then_224, label %merge_225

merge_225:                                        ; preds = %merge_223
  ret i64 0

then_214:                                         ; preds = %entry
  ret i64 0

then_216:                                         ; preds = %merge_215
  ret i64 0

then_218:                                         ; preds = %merge_217
  %"%resp_buf.load3" = load i64, ptr %"%resp_buf", align 8
  call void @sad_ll_kfree(i64 %"%resp_buf.load3")
  ret i64 0

then_220:                                         ; preds = %merge_219
  %"%dhcp_offers.load" = load i64, ptr @dhcp_offers, align 8
  %addtmp = add i64 %"%dhcp_offers.load", 1
  store i64 %addtmp, ptr %"%dhcp_offers", align 8
  %calltmp8 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  %"%dhcp_offered_ip.load" = load i64, ptr @dhcp_offered_ip, align 8
  call void @ip_byte(i64 %"%dhcp_offered_ip.load", i64 0)
  %calltmp9 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 0)
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 46)
  %"%dhcp_offered_ip.load10" = load i64, ptr @dhcp_offered_ip, align 8
  call void @ip_byte(i64 %"%dhcp_offered_ip.load10", i64 1)
  %calltmp11 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 0)
  %"%COM1.load12" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load12", i64 46)
  %"%dhcp_offered_ip.load13" = load i64, ptr @dhcp_offered_ip, align 8
  call void @ip_byte(i64 %"%dhcp_offered_ip.load13", i64 2)
  %calltmp14 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 0)
  %"%COM1.load15" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load15", i64 46)
  %"%dhcp_offered_ip.load16" = load i64, ptr @dhcp_offered_ip, align 8
  call void @ip_byte(i64 %"%dhcp_offered_ip.load16", i64 3)
  %calltmp17 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 0)
  %calltmp18 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp19 = call i64 @dhcp_request()
  ret i64 %calltmp19

then_222:                                         ; preds = %merge_221
  %"%dhcp_acks.load" = load i64, ptr @dhcp_acks, align 8
  %addtmp22 = add i64 %"%dhcp_acks.load", 1
  store i64 %addtmp22, ptr %"%dhcp_acks", align 8
  store i64 3, ptr %"%dhcp_state", align 8
  %"%dhcp_offered_ip.load23" = load i64, ptr @dhcp_offered_ip, align 8
  call void @ip_byte(i64 %"%dhcp_offered_ip.load23", i64 0)
  %"%dhcp_offered_ip.load24" = load i64, ptr @dhcp_offered_ip, align 8
  call void @ip_byte(i64 %"%dhcp_offered_ip.load24", i64 1)
  %"%dhcp_offered_ip.load25" = load i64, ptr @dhcp_offered_ip, align 8
  call void @ip_byte(i64 %"%dhcp_offered_ip.load25", i64 2)
  %"%dhcp_offered_ip.load26" = load i64, ptr @dhcp_offered_ip, align 8
  call void @ip_byte(i64 %"%dhcp_offered_ip.load26", i64 3)
  call void @"ip_\D8\B9\D9\8A\D9\86_\D8\B9\D9\86\D9\88\D8\A7\D9\86"(i64 0, i64 0, i64 0, i64 0)
  %calltmp27 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %"%dhcp_offered_ip.load28" = load i64, ptr @dhcp_offered_ip, align 8
  call void @ip_byte(i64 %"%dhcp_offered_ip.load28", i64 0)
  %calltmp29 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 0)
  %"%COM1.load30" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load30", i64 46)
  %"%dhcp_offered_ip.load31" = load i64, ptr @dhcp_offered_ip, align 8
  call void @ip_byte(i64 %"%dhcp_offered_ip.load31", i64 1)
  %calltmp32 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 0)
  %"%COM1.load33" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load33", i64 46)
  %"%dhcp_offered_ip.load34" = load i64, ptr @dhcp_offered_ip, align 8
  call void @ip_byte(i64 %"%dhcp_offered_ip.load34", i64 2)
  %calltmp35 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 0)
  %"%COM1.load36" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load36", i64 46)
  %"%dhcp_offered_ip.load37" = load i64, ptr @dhcp_offered_ip, align 8
  call void @ip_byte(i64 %"%dhcp_offered_ip.load37", i64 3)
  %calltmp38 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 0)
  %calltmp39 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %"%1334.load" = load i64, ptr @dhcp_lease_time, align 8
  %calltmp40 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 %"%1334.load")
  %calltmp41 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1

then_224:                                         ; preds = %merge_223
  %"%dhcp_naks.load" = load i64, ptr @dhcp_naks, align 8
  %addtmp44 = add i64 %"%dhcp_naks.load", 1
  store i64 %addtmp44, ptr %"%dhcp_naks", align 8
  store i64 0, ptr %"%dhcp_state", align 8
  %calltmp45 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %calltmp46 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @dhcp_request() {
entry:
  %"%dhcp_state" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%pkt_len" = alloca i64, align 8
  %"%pkt_buf" = alloca i64, align 8
  %calltmp = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %calltmp1 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  call void @sad_ll_kmalloc(i64 548)
  store i64 0, ptr %"%pkt_buf", align 8
  %"%pkt_buf.load" = load i64, ptr %"%pkt_buf", align 8
  %cmpeqtmp = icmp eq i64 %"%pkt_buf.load", 0
  br i1 %cmpeqtmp, label %then_226, label %merge_227

merge_227:                                        ; preds = %entry
  %"%pkt_buf.load2" = load i64, ptr %"%pkt_buf", align 8
  %"%1358.load" = load i64, ptr @dhcp_offered_ip, align 8
  %"%1360.load" = load i64, ptr @dhcp_server_ip, align 8
  %calltmp3 = call i64 @dhcp_build_request(i64 %"%pkt_buf.load2", i64 %"%1358.load", i64 %"%1360.load")
  store i64 %calltmp3, ptr %"%pkt_len", align 8
  %"%dhcp_sock.load" = load i64, ptr @dhcp_sock, align 8
  %"%DHCP_SERVER_PORT.load" = load i64, ptr @DHCP_SERVER_PORT, align 8
  %"%pkt_buf.load4" = load i64, ptr %"%pkt_buf", align 8
  %"%pkt_len.load" = load i64, ptr %"%pkt_len", align 8
  call void @udp_broadcast(i64 %"%dhcp_sock.load", i64 %"%DHCP_SERVER_PORT.load", i64 %"%pkt_buf.load4", i64 %"%pkt_len.load")
  store i64 0, ptr %"%result", align 8
  %"%pkt_buf.load5" = load i64, ptr %"%pkt_buf", align 8
  call void @sad_ll_kfree(i64 %"%pkt_buf.load5")
  %"%result.load" = load i64, ptr %"%result", align 8
  %cmpgttmp = icmp sgt i64 %"%result.load", 0
  br i1 %cmpgttmp, label %then_228, label %merge_229

merge_229:                                        ; preds = %then_228, %merge_227
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

then_226:                                         ; preds = %entry
  ret i64 0

then_228:                                         ; preds = %merge_227
  store i64 2, ptr %"%dhcp_state", align 8
  %calltmp6 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  %calltmp7 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_229
}

define i64 @"dns_\D8\B9\D9\8A\D9\86_\D8\AE\D8\A7\D8\AF\D9\85"(i64 %a, i64 %b, i64 %c, i64 %d) {
entry:
  %"%dns_server_3" = alloca i64, align 8
  %"%dns_server_2" = alloca i64, align 8
  %"%dns_server_1" = alloca i64, align 8
  %"%dns_server_0" = alloca i64, align 8
  %d4 = alloca i64, align 8
  store i64 %d, ptr %d4, align 8
  %c3 = alloca i64, align 8
  store i64 %c, ptr %c3, align 8
  %b2 = alloca i64, align 8
  store i64 %b, ptr %b2, align 8
  %a1 = alloca i64, align 8
  store i64 %a, ptr %a1, align 8
  %"%1378.load" = load i64, ptr %a1, align 8
  store i64 %"%1378.load", ptr %"%dns_server_0", align 8
  %"%1380.load" = load i64, ptr %b2, align 8
  store i64 %"%1380.load", ptr %"%dns_server_1", align 8
  %"%1382.load" = load i64, ptr %c3, align 8
  store i64 %"%1382.load", ptr %"%dns_server_2", align 8
  %"%1384.load" = load i64, ptr %d4, align 8
  store i64 %"%1384.load", ptr %"%dns_server_3", align 8
  ret i64 1
}

define i64 @"dhcp_\D8\B9\D9\8A\D9\86_mac"(i64 %m0, i64 %m1, i64 %m2, i64 %m3, i64 %m4, i64 %m5) {
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
  %"%1387.load" = load i64, ptr %m01, align 8
  store i64 %"%1387.load", ptr %"%local_mac_0", align 8
  %"%1389.load" = load i64, ptr %m12, align 8
  store i64 %"%1389.load", ptr %"%local_mac_1", align 8
  %"%1391.load" = load i64, ptr %m23, align 8
  store i64 %"%1391.load", ptr %"%local_mac_2", align 8
  %"%1393.load" = load i64, ptr %m34, align 8
  store i64 %"%1393.load", ptr %"%local_mac_3", align 8
  %"%1395.load" = load i64, ptr %m45, align 8
  store i64 %"%1395.load", ptr %"%local_mac_4", align 8
  %"%1397.load" = load i64, ptr %m56, align 8
  store i64 %"%1397.load", ptr %"%local_mac_5", align 8
  ret i64 1
}

define i64 @"dns_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %calltmp = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %calltmp1 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  %"%1405.load" = load i64, ptr @dns_server_0, align 8
  %calltmp3 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 %"%1405.load")
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 46)
  %"%1410.load" = load i64, ptr @dns_server_1, align 8
  %calltmp4 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 %"%1410.load")
  %"%COM1.load5" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load5", i64 46)
  %"%1415.load" = load i64, ptr @dns_server_2, align 8
  %calltmp6 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 %"%1415.load")
  %"%COM1.load7" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load7", i64 46)
  %"%1420.load" = load i64, ptr @dns_server_3, align 8
  %calltmp8 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 %"%1420.load")
  %calltmp9 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp10 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %"%1426.load" = load i64, ptr @dns_queries, align 8
  %calltmp11 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 %"%1426.load")
  %calltmp12 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp13 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %"%1432.load" = load i64, ptr @dns_responses, align 8
  %calltmp14 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 %"%1432.load")
  %calltmp15 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp16 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  %"%1438.load" = load i64, ptr @dns_cache_hits, align 8
  %calltmp17 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 %"%1438.load")
  %calltmp18 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp19 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  %"%1444.load" = load i64, ptr @dns_errors, align 8
  %calltmp20 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 %"%1444.load")
  %calltmp21 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp22 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  %"%1450.load" = load i64, ptr @dhcp_state, align 8
  %calltmp23 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 %"%1450.load")
  %calltmp24 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp25 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  %"%1456.load" = load i64, ptr @dhcp_discovers, align 8
  %calltmp26 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 %"%1456.load")
  %calltmp27 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp28 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  %"%1462.load" = load i64, ptr @dhcp_offers, align 8
  %calltmp29 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 %"%1462.load")
  %calltmp30 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp31 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %"%1468.load" = load i64, ptr @dhcp_acks, align 8
  %calltmp32 = call i64 @"dns_serial_\D8\B1\D9\82\D9\85"(i64 %"%1468.load")
  %calltmp33 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp34 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  %calltmp35 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @"dns_\D9\87\D9\8A\D8\A6"() {
entry:
  %"%dns_query_id" = alloca i64, align 8
  %"%dhcp_sock" = alloca i64, align 8
  %"%dns_sock" = alloca i64, align 8
  %"%dhcp_lease_time" = alloca i64, align 8
  %"%dhcp_offered_ip" = alloca i64, align 8
  %"%dhcp_server_ip" = alloca i64, align 8
  %"%dhcp_xid" = alloca i64, align 8
  %"%dhcp_state" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %"%dhcp_naks" = alloca i64, align 8
  %"%dhcp_acks" = alloca i64, align 8
  %"%dhcp_offers" = alloca i64, align 8
  %"%dhcp_discovers" = alloca i64, align 8
  %"%dns_errors" = alloca i64, align 8
  %"%dns_cache_hits" = alloca i64, align 8
  %"%dns_responses" = alloca i64, align 8
  %"%dns_queries" = alloca i64, align 8
  %calltmp = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  %calltmp1 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%dns_queries", align 8
  store i64 0, ptr %"%dns_responses", align 8
  store i64 0, ptr %"%dns_cache_hits", align 8
  store i64 0, ptr %"%dns_errors", align 8
  store i64 0, ptr %"%dhcp_discovers", align 8
  store i64 0, ptr %"%dhcp_offers", align 8
  store i64 0, ptr %"%dhcp_acks", align 8
  store i64 0, ptr %"%dhcp_naks", align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_230

while_body_231:                                   ; preds = %while_cond_230
  %"%i.load2" = load i64, ptr %"%i", align 8
  %calltmp3 = call i64 @dns_cache_set_hash(i64 %"%i.load2", i64 0)
  %"%i.load4" = load i64, ptr %"%i", align 8
  %calltmp5 = call i64 @dns_cache_set_ip(i64 %"%i.load4", i64 0)
  %"%i.load6" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load6", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_230

while_cond_230:                                   ; preds = %while_body_231, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %cmplttmp = icmp slt i64 %"%i.load", 8
  br i1 %cmplttmp, label %while_body_231, label %while_exit_232

while_exit_232:                                   ; preds = %while_cond_230
  store i64 0, ptr %"%dhcp_state", align 8
  store i64 0, ptr %"%dhcp_xid", align 8
  store i64 0, ptr %"%dhcp_server_ip", align 8
  store i64 0, ptr %"%dhcp_offered_ip", align 8
  store i64 0, ptr %"%dhcp_lease_time", align 8
  store i64 0, ptr %"%dns_sock", align 8
  store i64 0, ptr %"%dhcp_sock", align 8
  store i64 1, ptr %"%dns_query_id", align 8
  %calltmp7 = call i64 @"dns_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  %calltmp8 = call i64 @"dns_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1
}

define i64 @"dns_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  %"%dhcp_state.load" = load i64, ptr @dhcp_state, align 8
  %cmpeqtmp = icmp eq i64 %"%dhcp_state.load", 1
  br i1 %cmpeqtmp, label %then_233, label %merge_234

merge_234:                                        ; preds = %then_233, %entry
  %"%dhcp_state.load1" = load i64, ptr @dhcp_state, align 8
  %cmpeqtmp2 = icmp eq i64 %"%dhcp_state.load1", 2
  br i1 %cmpeqtmp2, label %then_235, label %merge_236

merge_236:                                        ; preds = %then_235, %merge_234
  ret i64 0

then_233:                                         ; preds = %entry
  %calltmp = call i64 @dhcp_handle_response()
  br label %merge_234

then_235:                                         ; preds = %merge_234
  %calltmp3 = call i64 @dhcp_handle_response()
  br label %merge_236
}

define void @__sad_main() {
entry:
  %"%dhcp_naks" = alloca i64, align 8
  %"%dhcp_acks" = alloca i64, align 8
  %"%dhcp_offers" = alloca i64, align 8
  %"%dhcp_discovers" = alloca i64, align 8
  %"%dns_errors" = alloca i64, align 8
  %"%dns_cache_hits" = alloca i64, align 8
  %"%dns_responses" = alloca i64, align 8
  %"%dns_queries" = alloca i64, align 8
  %"%local_mac_5" = alloca i64, align 8
  %"%local_mac_4" = alloca i64, align 8
  %"%local_mac_3" = alloca i64, align 8
  %"%local_mac_2" = alloca i64, align 8
  %"%local_mac_1" = alloca i64, align 8
  %"%local_mac_0" = alloca i64, align 8
  %"%dhcp_sock" = alloca i64, align 8
  %"%dhcp_lease_time" = alloca i64, align 8
  %"%dhcp_offered_ip" = alloca i64, align 8
  %"%dhcp_server_ip" = alloca i64, align 8
  %"%dhcp_xid" = alloca i64, align 8
  %"%dhcp_state" = alloca i64, align 8
  %"%dns_cache_ttl_7" = alloca i64, align 8
  %"%dns_cache_ttl_6" = alloca i64, align 8
  %"%dns_cache_ttl_5" = alloca i64, align 8
  %"%dns_cache_ttl_4" = alloca i64, align 8
  %"%dns_cache_ttl_3" = alloca i64, align 8
  %"%dns_cache_ttl_2" = alloca i64, align 8
  %"%dns_cache_ttl_1" = alloca i64, align 8
  %"%dns_cache_ttl_0" = alloca i64, align 8
  %"%dns_cache_ip_7" = alloca i64, align 8
  %"%dns_cache_ip_6" = alloca i64, align 8
  %"%dns_cache_ip_5" = alloca i64, align 8
  %"%dns_cache_ip_4" = alloca i64, align 8
  %"%dns_cache_ip_3" = alloca i64, align 8
  %"%dns_cache_ip_2" = alloca i64, align 8
  %"%dns_cache_ip_1" = alloca i64, align 8
  %"%dns_cache_ip_0" = alloca i64, align 8
  %"%dns_cache_hash_7" = alloca i64, align 8
  %"%dns_cache_hash_6" = alloca i64, align 8
  %"%dns_cache_hash_5" = alloca i64, align 8
  %"%dns_cache_hash_4" = alloca i64, align 8
  %"%dns_cache_hash_3" = alloca i64, align 8
  %"%dns_cache_hash_2" = alloca i64, align 8
  %"%dns_cache_hash_1" = alloca i64, align 8
  %"%dns_cache_hash_0" = alloca i64, align 8
  %"%dns_sock" = alloca i64, align 8
  %"%dns_query_id" = alloca i64, align 8
  %"%dns_server_3" = alloca i64, align 8
  %"%dns_server_2" = alloca i64, align 8
  %"%dns_server_1" = alloca i64, align 8
  %"%dns_server_0" = alloca i64, align 8
  %"%DHCP_OPT_END" = alloca i64, align 8
  %"%DHCP_OPT_PARAMLIST" = alloca i64, align 8
  %"%DHCP_OPT_SERVER" = alloca i64, align 8
  %"%DHCP_OPT_MSGTYPE" = alloca i64, align 8
  %"%DHCP_OPT_LEASE" = alloca i64, align 8
  %"%DHCP_OPT_REQIP" = alloca i64, align 8
  %"%DHCP_OPT_DOMAIN" = alloca i64, align 8
  %"%DHCP_OPT_HOSTNAME" = alloca i64, align 8
  %"%DHCP_OPT_DNS" = alloca i64, align 8
  %"%DHCP_OPT_ROUTER" = alloca i64, align 8
  %"%DHCP_OPT_SUBNET" = alloca i64, align 8
  %"%DHCP_NAK" = alloca i64, align 8
  %"%DHCP_ACK" = alloca i64, align 8
  %"%DHCP_REQUEST" = alloca i64, align 8
  %"%DHCP_OFFER" = alloca i64, align 8
  %"%DHCP_DISCOVER" = alloca i64, align 8
  %"%DHCP_MAGIC_4" = alloca i64, align 8
  %"%DHCP_MAGIC_3" = alloca i64, align 8
  %"%DHCP_MAGIC_2" = alloca i64, align 8
  %"%DHCP_MAGIC_1" = alloca i64, align 8
  %"%DHCP_CLIENT_PORT" = alloca i64, align 8
  %"%DHCP_SERVER_PORT" = alloca i64, align 8
  %"%DNS_RECURSION" = alloca i64, align 8
  %"%DNS_MAX_RESPONSE" = alloca i64, align 8
  %"%DNS_CLASS_IN" = alloca i64, align 8
  %"%DNS_TYPE_A" = alloca i64, align 8
  %"%DNS_HEADER_SIZE" = alloca i64, align 8
  %"%DNS_PORT" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 53, ptr %"%DNS_PORT", align 8
  store i64 12, ptr %"%DNS_HEADER_SIZE", align 8
  store i64 1, ptr %"%DNS_TYPE_A", align 8
  store i64 1, ptr %"%DNS_CLASS_IN", align 8
  store i64 512, ptr %"%DNS_MAX_RESPONSE", align 8
  store i64 1, ptr %"%DNS_RECURSION", align 8
  store i64 67, ptr %"%DHCP_SERVER_PORT", align 8
  store i64 68, ptr %"%DHCP_CLIENT_PORT", align 8
  store i64 99, ptr %"%DHCP_MAGIC_1", align 8
  store i64 130, ptr %"%DHCP_MAGIC_2", align 8
  store i64 83, ptr %"%DHCP_MAGIC_3", align 8
  store i64 99, ptr %"%DHCP_MAGIC_4", align 8
  store i64 1, ptr %"%DHCP_DISCOVER", align 8
  store i64 2, ptr %"%DHCP_OFFER", align 8
  store i64 3, ptr %"%DHCP_REQUEST", align 8
  store i64 5, ptr %"%DHCP_ACK", align 8
  store i64 6, ptr %"%DHCP_NAK", align 8
  store i64 1, ptr %"%DHCP_OPT_SUBNET", align 8
  store i64 3, ptr %"%DHCP_OPT_ROUTER", align 8
  store i64 6, ptr %"%DHCP_OPT_DNS", align 8
  store i64 12, ptr %"%DHCP_OPT_HOSTNAME", align 8
  store i64 15, ptr %"%DHCP_OPT_DOMAIN", align 8
  store i64 50, ptr %"%DHCP_OPT_REQIP", align 8
  store i64 51, ptr %"%DHCP_OPT_LEASE", align 8
  store i64 53, ptr %"%DHCP_OPT_MSGTYPE", align 8
  store i64 54, ptr %"%DHCP_OPT_SERVER", align 8
  store i64 55, ptr %"%DHCP_OPT_PARAMLIST", align 8
  store i64 255, ptr %"%DHCP_OPT_END", align 8
  store i64 8, ptr %"%dns_server_0", align 8
  store i64 8, ptr %"%dns_server_1", align 8
  store i64 8, ptr %"%dns_server_2", align 8
  store i64 8, ptr %"%dns_server_3", align 8
  store i64 1, ptr %"%dns_query_id", align 8
  store i64 0, ptr %"%dns_sock", align 8
  store i64 0, ptr %"%dns_cache_hash_0", align 8
  store i64 0, ptr %"%dns_cache_hash_1", align 8
  store i64 0, ptr %"%dns_cache_hash_2", align 8
  store i64 0, ptr %"%dns_cache_hash_3", align 8
  store i64 0, ptr %"%dns_cache_hash_4", align 8
  store i64 0, ptr %"%dns_cache_hash_5", align 8
  store i64 0, ptr %"%dns_cache_hash_6", align 8
  store i64 0, ptr %"%dns_cache_hash_7", align 8
  store i64 0, ptr %"%dns_cache_ip_0", align 8
  store i64 0, ptr %"%dns_cache_ip_1", align 8
  store i64 0, ptr %"%dns_cache_ip_2", align 8
  store i64 0, ptr %"%dns_cache_ip_3", align 8
  store i64 0, ptr %"%dns_cache_ip_4", align 8
  store i64 0, ptr %"%dns_cache_ip_5", align 8
  store i64 0, ptr %"%dns_cache_ip_6", align 8
  store i64 0, ptr %"%dns_cache_ip_7", align 8
  store i64 0, ptr %"%dns_cache_ttl_0", align 8
  store i64 0, ptr %"%dns_cache_ttl_1", align 8
  store i64 0, ptr %"%dns_cache_ttl_2", align 8
  store i64 0, ptr %"%dns_cache_ttl_3", align 8
  store i64 0, ptr %"%dns_cache_ttl_4", align 8
  store i64 0, ptr %"%dns_cache_ttl_5", align 8
  store i64 0, ptr %"%dns_cache_ttl_6", align 8
  store i64 0, ptr %"%dns_cache_ttl_7", align 8
  store i64 0, ptr %"%dhcp_state", align 8
  store i64 0, ptr %"%dhcp_xid", align 8
  store i64 0, ptr %"%dhcp_server_ip", align 8
  store i64 0, ptr %"%dhcp_offered_ip", align 8
  store i64 0, ptr %"%dhcp_lease_time", align 8
  store i64 0, ptr %"%dhcp_sock", align 8
  store i64 0, ptr %"%local_mac_0", align 8
  store i64 0, ptr %"%local_mac_1", align 8
  store i64 0, ptr %"%local_mac_2", align 8
  store i64 0, ptr %"%local_mac_3", align 8
  store i64 0, ptr %"%local_mac_4", align 8
  store i64 0, ptr %"%local_mac_5", align 8
  store i64 0, ptr %"%dns_queries", align 8
  store i64 0, ptr %"%dns_responses", align 8
  store i64 0, ptr %"%dns_cache_hits", align 8
  store i64 0, ptr %"%dns_errors", align 8
  store i64 0, ptr %"%dhcp_discovers", align 8
  store i64 0, ptr %"%dhcp_offers", align 8
  store i64 0, ptr %"%dhcp_acks", align 8
  store i64 0, ptr %"%dhcp_naks", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
