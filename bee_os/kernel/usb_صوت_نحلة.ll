; ModuleID = 'usb_صوت_نحلة'
source_filename = "usb_\D8\B5\D9\88\D8\AA_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@COM1 = constant i64 1016
@UAC_CLASS = constant i64 1
@UAC_SUBCLASS_CONTROL = constant i64 1
@UAC_SUBCLASS_STREAM = constant i64 2
@UAC_SUBCLASS_MIDI = constant i64 3
@UAC_AC_HEADER = constant i64 1
@UAC_AC_INPUT_TERMINAL = constant i64 2
@UAC_AC_OUTPUT_TERMINAL = constant i64 3
@UAC_AC_MIXER_UNIT = constant i64 4
@UAC_AC_SELECTOR_UNIT = constant i64 5
@UAC_AC_FEATURE_UNIT = constant i64 6
@UAC_AC_PROCESSING_UNIT = constant i64 7
@UAC_AC_EXTENSION_UNIT = constant i64 8
@UAC_AS_GENERAL = constant i64 1
@UAC_AS_FORMAT_TYPE = constant i64 2
@UAC_AS_FORMAT_SPECIFIC = constant i64 3
@UAC_TT_USB_STREAMING = constant i64 257
@UAC_TT_SPEAKER = constant i64 769
@UAC_TT_HEADPHONES = constant i64 770
@UAC_TT_MICROPHONE = constant i64 513
@UAC_TT_DESKTOP_MIC = constant i64 514
@UAC_TT_LINE_CONNECTOR = constant i64 1538
@UAC_TT_SPDIF = constant i64 1539
@UAC_TT_HDMI = constant i64 1541
@UAC_FU_MUTE = constant i64 1
@UAC_FU_VOLUME = constant i64 2
@UAC_FU_BASS = constant i64 3
@UAC_FU_MID = constant i64 4
@UAC_FU_TREBLE = constant i64 5
@UAC_FU_EQUALIZER = constant i64 6
@UAC_FU_AGC = constant i64 7
@UAC_FU_DELAY = constant i64 8
@UAC_FU_BASS_BOOST = constant i64 9
@UAC_FU_LOUDNESS = constant i64 10
@UAC_SET_CUR = constant i64 1
@UAC_GET_CUR = constant i64 129
@UAC_SET_MIN = constant i64 2
@UAC_GET_MIN = constant i64 130
@UAC_SET_MAX = constant i64 3
@UAC_GET_MAX = constant i64 131
@UAC_SET_RES = constant i64 4
@UAC_GET_RES = constant i64 132
@UAC_RATE_8000 = constant i64 8000
@UAC_RATE_11025 = constant i64 11025
@UAC_RATE_16000 = constant i64 16000
@UAC_RATE_22050 = constant i64 22050
@UAC_RATE_32000 = constant i64 32000
@UAC_RATE_44100 = constant i64 44100
@UAC_RATE_48000 = constant i64 48000
@UAC_RATE_96000 = constant i64 96000
@UAC_FORMAT_PCM = constant i64 1
@UAC_FORMAT_PCM8 = constant i64 2
@UAC_FORMAT_IEEE_FLOAT = constant i64 3
@UAC_FORMAT_ALAW = constant i64 4
@UAC_FORMAT_MULAW = constant i64 5
@UAC_MAX_DEVICES = constant i64 4
@UAC_MAX_TERMINALS = constant i64 16
@UAC_MAX_FEATURES = constant i64 8
@uac_present_0 = global i64 0
@uac_present_1 = global i64 0
@uac_present_2 = global i64 0
@uac_present_3 = global i64 0
@uac_addr_0 = global i64 0
@uac_addr_1 = global i64 0
@uac_addr_2 = global i64 0
@uac_addr_3 = global i64 0
@uac_ctrl_iface_0 = global i64 0
@uac_ctrl_iface_1 = global i64 0
@uac_ctrl_iface_2 = global i64 0
@uac_ctrl_iface_3 = global i64 0
@uac_stream_out_iface_0 = global i64 0
@uac_stream_out_iface_1 = global i64 0
@uac_stream_out_iface_2 = global i64 0
@uac_stream_out_iface_3 = global i64 0
@uac_stream_in_iface_0 = global i64 0
@uac_stream_in_iface_1 = global i64 0
@uac_stream_in_iface_2 = global i64 0
@uac_stream_in_iface_3 = global i64 0
@uac_ep_out_0 = global i64 0
@uac_ep_out_1 = global i64 0
@uac_ep_out_2 = global i64 0
@uac_ep_out_3 = global i64 0
@uac_ep_in_0 = global i64 0
@uac_ep_in_1 = global i64 0
@uac_ep_in_2 = global i64 0
@uac_ep_in_3 = global i64 0
@uac_maxpkt_out_0 = global i64 0
@uac_maxpkt_out_1 = global i64 0
@uac_maxpkt_out_2 = global i64 0
@uac_maxpkt_out_3 = global i64 0
@uac_maxpkt_in_0 = global i64 0
@uac_maxpkt_in_1 = global i64 0
@uac_maxpkt_in_2 = global i64 0
@uac_maxpkt_in_3 = global i64 0
@uac_version_0 = global i64 0
@uac_version_1 = global i64 0
@uac_version_2 = global i64 0
@uac_version_3 = global i64 0
@uac_channels_0 = global i64 2
@uac_channels_1 = global i64 2
@uac_channels_2 = global i64 2
@uac_channels_3 = global i64 2
@uac_bits_0 = global i64 16
@uac_bits_1 = global i64 16
@uac_bits_2 = global i64 16
@uac_bits_3 = global i64 16
@uac_rate_0 = global i64 44100
@uac_rate_1 = global i64 44100
@uac_rate_2 = global i64 44100
@uac_rate_3 = global i64 44100
@uac_format_0 = global i64 1
@uac_format_1 = global i64 1
@uac_format_2 = global i64 1
@uac_format_3 = global i64 1
@uac_streaming_0 = global i64 0
@uac_streaming_1 = global i64 0
@uac_streaming_2 = global i64 0
@uac_streaming_3 = global i64 0
@term_id_0 = global i64 0
@term_id_1 = global i64 0
@term_id_2 = global i64 0
@term_id_3 = global i64 0
@term_id_4 = global i64 0
@term_id_5 = global i64 0
@term_id_6 = global i64 0
@term_id_7 = global i64 0
@term_id_8 = global i64 0
@term_id_9 = global i64 0
@term_id_10 = global i64 0
@term_id_11 = global i64 0
@term_id_12 = global i64 0
@term_id_13 = global i64 0
@term_id_14 = global i64 0
@term_id_15 = global i64 0
@term_type_0 = global i64 0
@term_type_1 = global i64 0
@term_type_2 = global i64 0
@term_type_3 = global i64 0
@term_type_4 = global i64 0
@term_type_5 = global i64 0
@term_type_6 = global i64 0
@term_type_7 = global i64 0
@term_type_8 = global i64 0
@term_type_9 = global i64 0
@term_type_10 = global i64 0
@term_type_11 = global i64 0
@term_type_12 = global i64 0
@term_type_13 = global i64 0
@term_type_14 = global i64 0
@term_type_15 = global i64 0
@term_dir_0 = global i64 0
@term_dir_1 = global i64 0
@term_dir_2 = global i64 0
@term_dir_3 = global i64 0
@term_dir_4 = global i64 0
@term_dir_5 = global i64 0
@term_dir_6 = global i64 0
@term_dir_7 = global i64 0
@term_dir_8 = global i64 0
@term_dir_9 = global i64 0
@term_dir_10 = global i64 0
@term_dir_11 = global i64 0
@term_dir_12 = global i64 0
@term_dir_13 = global i64 0
@term_dir_14 = global i64 0
@term_dir_15 = global i64 0
@term_ch_0 = global i64 0
@term_ch_1 = global i64 0
@term_ch_2 = global i64 0
@term_ch_3 = global i64 0
@term_ch_4 = global i64 0
@term_ch_5 = global i64 0
@term_ch_6 = global i64 0
@term_ch_7 = global i64 0
@term_ch_8 = global i64 0
@term_ch_9 = global i64 0
@term_ch_10 = global i64 0
@term_ch_11 = global i64 0
@term_ch_12 = global i64 0
@term_ch_13 = global i64 0
@term_ch_14 = global i64 0
@term_ch_15 = global i64 0
@term_dev_0 = global i64 0
@term_dev_1 = global i64 0
@term_dev_2 = global i64 0
@term_dev_3 = global i64 0
@term_dev_4 = global i64 0
@term_dev_5 = global i64 0
@term_dev_6 = global i64 0
@term_dev_7 = global i64 0
@term_dev_8 = global i64 0
@term_dev_9 = global i64 0
@term_dev_10 = global i64 0
@term_dev_11 = global i64 0
@term_dev_12 = global i64 0
@term_dev_13 = global i64 0
@term_dev_14 = global i64 0
@term_dev_15 = global i64 0
@uac_term_count = global i64 0
@fu_id_0 = global i64 0
@fu_id_1 = global i64 0
@fu_id_2 = global i64 0
@fu_id_3 = global i64 0
@fu_id_4 = global i64 0
@fu_id_5 = global i64 0
@fu_id_6 = global i64 0
@fu_id_7 = global i64 0
@fu_src_0 = global i64 0
@fu_src_1 = global i64 0
@fu_src_2 = global i64 0
@fu_src_3 = global i64 0
@fu_src_4 = global i64 0
@fu_src_5 = global i64 0
@fu_src_6 = global i64 0
@fu_src_7 = global i64 0
@fu_ctrl_0 = global i64 0
@fu_ctrl_1 = global i64 0
@fu_ctrl_2 = global i64 0
@fu_ctrl_3 = global i64 0
@fu_ctrl_4 = global i64 0
@fu_ctrl_5 = global i64 0
@fu_ctrl_6 = global i64 0
@fu_ctrl_7 = global i64 0
@fu_dev_0 = global i64 0
@fu_dev_1 = global i64 0
@fu_dev_2 = global i64 0
@fu_dev_3 = global i64 0
@fu_dev_4 = global i64 0
@fu_dev_5 = global i64 0
@fu_dev_6 = global i64 0
@fu_dev_7 = global i64 0
@fu_vol_0 = global i64 128
@fu_vol_1 = global i64 128
@fu_vol_2 = global i64 128
@fu_vol_3 = global i64 128
@fu_vol_4 = global i64 128
@fu_vol_5 = global i64 128
@fu_vol_6 = global i64 128
@fu_vol_7 = global i64 128
@fu_mute_0 = global i64 0
@fu_mute_1 = global i64 0
@fu_mute_2 = global i64 0
@fu_mute_3 = global i64 0
@fu_mute_4 = global i64 0
@fu_mute_5 = global i64 0
@fu_mute_6 = global i64 0
@fu_mute_7 = global i64 0
@uac_fu_count = global i64 0
@uac_device_count = global i64 0
@uac_packets_sent = global i64 0
@uac_packets_received = global i64 0
@uac_errors = global i64 0
@str.const = private unnamed_addr constant [16 x i8] c"[UAC] Terminal \00", align 1
@str.const.1 = private unnamed_addr constant [7 x i8] c" type=\00", align 1
@str.const.2 = private unnamed_addr constant [4 x i8] c" IN\00", align 1
@str.const.3 = private unnamed_addr constant [5 x i8] c" OUT\00", align 1
@str.const.4 = private unnamed_addr constant [5 x i8] c" ch=\00", align 1
@str.const.5 = private unnamed_addr constant [20 x i8] c"[UAC] Feature Unit \00", align 1
@str.const.6 = private unnamed_addr constant [6 x i8] c" src=\00", align 1
@str.const.7 = private unnamed_addr constant [7 x i8] c" ctrl=\00", align 1
@str.const.8 = private unnamed_addr constant [14 x i8] c"[UAC] Device \00", align 1
@str.const.9 = private unnamed_addr constant [25 x i8] c" registered at USB addr \00", align 1
@str.const.10 = private unnamed_addr constant [27 x i8] c"[UAC] Stream started: dev=\00", align 1
@str.const.11 = private unnamed_addr constant [7 x i8] c" rate=\00", align 1
@str.const.12 = private unnamed_addr constant [7 x i8] c" bits=\00", align 1
@str.const.14 = private unnamed_addr constant [27 x i8] c"[UAC] Stream stopped: dev=\00", align 1
@str.const.15 = private unnamed_addr constant [48 x i8] c"\E2\95\90\E2\95\90\E2\95\90 [UAC] \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\B5\D9\88\D8\AA USB \E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.16 = private unnamed_addr constant [15 x i8] c"  \D8\A3\D8\AC\D9\87\D8\B2\D8\A9: \00", align 1
@str.const.17 = private unnamed_addr constant [14 x i8] c"  terminals: \00", align 1
@str.const.18 = private unnamed_addr constant [18 x i8] c"  feature units: \00", align 1
@str.const.19 = private unnamed_addr constant [22 x i8] c"  \D8\AD\D8\B2\D9\85 \D9\85\D8\B1\D8\B3\D9\84\D8\A9: \00", align 1
@str.const.20 = private unnamed_addr constant [18 x i8] c" \D9\85\D8\B3\D8\AA\D9\82\D8\A8\D9\84\D8\A9: \00", align 1
@str.const.21 = private unnamed_addr constant [14 x i8] c" \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1: \00", align 1
@str.const.22 = private unnamed_addr constant [106 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.23 = private unnamed_addr constant [7 x i8] c"  Dev \00", align 1
@str.const.24 = private unnamed_addr constant [3 x i8] c": \00", align 1
@str.const.25 = private unnamed_addr constant [4 x i8] c"Hz \00", align 1
@str.const.26 = private unnamed_addr constant [5 x i8] c"bit \00", align 1
@str.const.27 = private unnamed_addr constant [3 x i8] c"ch\00", align 1
@str.const.28 = private unnamed_addr constant [11 x i8] c" [PLAYING]\00", align 1
@str.const.29 = private unnamed_addr constant [40 x i8] c"[UAC] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D9\86\D8\B8\D8\A7\D9\85 \D8\B5\D9\88\D8\AA USB...\00", align 1
@str.const.30 = private unnamed_addr constant [48 x i8] c"[UAC] \D8\AC\D8\A7\D9\87\D8\B2 \E2\80\94 \D9\81\D9\8A \D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1 \D8\A3\D8\AC\D9\87\D8\B2\D8\A9\00", align 1

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

define i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
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

define i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
entry:
  %"%d2" = alloca i64, align 8
  %"%d3" = alloca i64, align 8
  %"%d4" = alloca i64, align 8
  %"%d5" = alloca i64, align 8
  %"%d6" = alloca i64, align 8
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
  %cmpgetmp19 = icmp sge i64 %"%\D9\86.load", 100000
  br i1 %cmpgetmp19, label %then_12, label %merge_13

else_16:                                          ; preds = %merge_10
  %"%\D9\86.load31" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp32 = icmp sge i64 %"%\D9\86.load31", 10000
  br i1 %cmpgetmp32, label %then_17, label %merge_18

else_21:                                          ; preds = %merge_15
  %"%\D9\86.load44" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp45 = icmp sge i64 %"%\D9\86.load44", 1000
  br i1 %cmpgetmp45, label %then_22, label %merge_23

else_26:                                          ; preds = %merge_20
  %"%\D9\86.load57" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp58 = icmp sge i64 %"%\D9\86.load57", 100
  br i1 %cmpgetmp58, label %then_27, label %merge_28

merge_10:                                         ; preds = %merge_13, %then_9
  %"%val.load20" = load i64, ptr %"%val", align 8
  %cmpgetmp21 = icmp sge i64 %"%val.load20", 1000
  br i1 %cmpgetmp21, label %then_14, label %else_16

merge_13:                                         ; preds = %then_12, %else_11
  br label %merge_10

merge_15:                                         ; preds = %merge_18, %then_14
  %"%val.load33" = load i64, ptr %"%val", align 8
  %cmpgetmp34 = icmp sge i64 %"%val.load33", 100
  br i1 %cmpgetmp34, label %then_19, label %else_21

merge_18:                                         ; preds = %then_17, %else_16
  br label %merge_15

merge_20:                                         ; preds = %merge_23, %then_19
  %"%val.load46" = load i64, ptr %"%val", align 8
  %cmpgetmp47 = icmp sge i64 %"%val.load46", 10
  br i1 %cmpgetmp47, label %then_24, label %else_26

merge_23:                                         ; preds = %then_22, %else_21
  br label %merge_20

merge_25:                                         ; preds = %merge_28, %then_24
  %"%val.load59" = load i64, ptr %"%val", align 8
  %addtmp60 = add i64 48, %"%val.load59"
  %"%COM1.load61" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load61", i64 %addtmp60)
  ret i64 0

merge_28:                                         ; preds = %then_27, %else_26
  br label %merge_25

merge_6:                                          ; preds = %then_5, %entry
  %"%val.load3" = load i64, ptr %"%val", align 8
  %cmpgetmp = icmp sge i64 %"%val.load3", 100000
  br i1 %cmpgetmp, label %then_7, label %merge_8

merge_8:                                          ; preds = %then_7, %merge_6
  %"%val.load9" = load i64, ptr %"%val", align 8
  %cmpgetmp10 = icmp sge i64 %"%val.load9", 10000
  br i1 %cmpgetmp10, label %then_9, label %else_11

then_12:                                          ; preds = %else_11
  %"%COM1.load22" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load22", i64 48)
  br label %merge_13

then_14:                                          ; preds = %merge_10
  %"%val.load23" = load i64, ptr %"%val", align 8
  %divtmp24 = sdiv i64 %"%val.load23", 1000
  store i64 %divtmp24, ptr %"%d4", align 8
  %"%d4.load" = load i64, ptr %"%d4", align 8
  %addtmp25 = add i64 48, %"%d4.load"
  %"%COM1.load26" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load26", i64 %addtmp25)
  %"%d4.load27" = load i64, ptr %"%d4", align 8
  %multmp28 = mul i64 %"%d4.load27", 1000
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
  %divtmp37 = sdiv i64 %"%val.load36", 100
  store i64 %divtmp37, ptr %"%d3", align 8
  %"%d3.load" = load i64, ptr %"%d3", align 8
  %addtmp38 = add i64 48, %"%d3.load"
  %"%COM1.load39" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load39", i64 %addtmp38)
  %"%d3.load40" = load i64, ptr %"%d3", align 8
  %multmp41 = mul i64 %"%d3.load40", 100
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
  %divtmp50 = sdiv i64 %"%val.load49", 10
  store i64 %divtmp50, ptr %"%d2", align 8
  %"%d2.load" = load i64, ptr %"%d2", align 8
  %addtmp51 = add i64 48, %"%d2.load"
  %"%COM1.load52" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load52", i64 %addtmp51)
  %"%d2.load53" = load i64, ptr %"%d2", align 8
  %multmp54 = mul i64 %"%d2.load53", 10
  %"%val.load55" = load i64, ptr %"%val", align 8
  %subtmp56 = sub i64 %"%val.load55", %multmp54
  store i64 %subtmp56, ptr %"%val", align 8
  br label %merge_25

then_27:                                          ; preds = %else_26
  %"%COM1.load62" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load62", i64 48)
  br label %merge_28

then_5:                                           ; preds = %entry
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 45)
  %"%val.load2" = load i64, ptr %"%val", align 8
  %subtmp = sub i64 0, %"%val.load2"
  store i64 %subtmp, ptr %"%val", align 8
  br label %merge_6

then_7:                                           ; preds = %merge_6
  %"%val.load4" = load i64, ptr %"%val", align 8
  %divtmp = sdiv i64 %"%val.load4", 100000
  store i64 %divtmp, ptr %"%d6", align 8
  %"%d6.load" = load i64, ptr %"%d6", align 8
  %addtmp = add i64 48, %"%d6.load"
  %"%COM1.load5" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load5", i64 %addtmp)
  %"%d6.load6" = load i64, ptr %"%d6", align 8
  %multmp = mul i64 %"%d6.load6", 100000
  %"%val.load7" = load i64, ptr %"%val", align 8
  %subtmp8 = sub i64 %"%val.load7", %multmp
  store i64 %subtmp8, ptr %"%val", align 8
  br label %merge_8

then_9:                                           ; preds = %merge_8
  %"%val.load11" = load i64, ptr %"%val", align 8
  %divtmp12 = sdiv i64 %"%val.load11", 10000
  store i64 %divtmp12, ptr %"%d5", align 8
  %"%d5.load" = load i64, ptr %"%d5", align 8
  %addtmp13 = add i64 48, %"%d5.load"
  %"%COM1.load14" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load14", i64 %addtmp13)
  %"%d5.load15" = load i64, ptr %"%d5", align 8
  %multmp16 = mul i64 %"%d5.load15", 10000
  %"%val.load17" = load i64, ptr %"%val", align 8
  %subtmp18 = sub i64 %"%val.load17", %multmp16
  store i64 %subtmp18, ptr %"%val", align 8
  br label %merge_10
}

define i64 @"uac_serial_\D8\B3\D8\B7\D8\B1"() {
entry:
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 13)
  %"%COM1.load1" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load1", i64 10)
  ret i64 0
}

define i64 @uac_get_present(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_29, label %merge_30

merge_30:                                         ; preds = %entry
  %"%d.load2" = load i64, ptr %d1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%d.load2", 1
  br i1 %cmpeqtmp3, label %then_31, label %merge_32

merge_32:                                         ; preds = %merge_30
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_33, label %merge_34

merge_34:                                         ; preds = %merge_32
  %"%d.load8" = load i64, ptr %d1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 3
  br i1 %cmpeqtmp9, label %then_35, label %merge_36

merge_36:                                         ; preds = %merge_34
  ret i64 0

then_29:                                          ; preds = %entry
  %loadtmp = load i64, ptr @uac_present_0, align 8
  ret i64 %loadtmp

then_31:                                          ; preds = %merge_30
  %loadtmp4 = load i64, ptr @uac_present_1, align 8
  ret i64 %loadtmp4

then_33:                                          ; preds = %merge_32
  %loadtmp7 = load i64, ptr @uac_present_2, align 8
  ret i64 %loadtmp7

then_35:                                          ; preds = %merge_34
  %loadtmp10 = load i64, ptr @uac_present_3, align 8
  ret i64 %loadtmp10
}

define i64 @uac_set_present(i64 %d, i64 %val) {
entry:
  %"%uac_present_3" = alloca i64, align 8
  %"%uac_present_2" = alloca i64, align 8
  %"%uac_present_1" = alloca i64, align 8
  %"%uac_present_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_37, label %merge_38

merge_38:                                         ; preds = %then_37, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_39, label %merge_40

merge_40:                                         ; preds = %then_39, %merge_38
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_41, label %merge_42

merge_42:                                         ; preds = %then_41, %merge_40
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_43, label %merge_44

merge_44:                                         ; preds = %then_43, %merge_42
  ret i64 0

then_37:                                          ; preds = %entry
  %"%123.load" = load i64, ptr %val2, align 8
  store i64 %"%123.load", ptr %"%uac_present_0", align 8
  br label %merge_38

then_39:                                          ; preds = %merge_38
  %"%127.load" = load i64, ptr %val2, align 8
  store i64 %"%127.load", ptr %"%uac_present_1", align 8
  br label %merge_40

then_41:                                          ; preds = %merge_40
  %"%131.load" = load i64, ptr %val2, align 8
  store i64 %"%131.load", ptr %"%uac_present_2", align 8
  br label %merge_42

then_43:                                          ; preds = %merge_42
  %"%135.load" = load i64, ptr %val2, align 8
  store i64 %"%135.load", ptr %"%uac_present_3", align 8
  br label %merge_44
}

define i64 @uac_set_addr(i64 %d, i64 %val) {
entry:
  %"%uac_addr_3" = alloca i64, align 8
  %"%uac_addr_2" = alloca i64, align 8
  %"%uac_addr_1" = alloca i64, align 8
  %"%uac_addr_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_45, label %merge_46

merge_46:                                         ; preds = %then_45, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_47, label %merge_48

merge_48:                                         ; preds = %then_47, %merge_46
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_49, label %merge_50

merge_50:                                         ; preds = %then_49, %merge_48
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_51, label %merge_52

merge_52:                                         ; preds = %then_51, %merge_50
  ret i64 0

then_45:                                          ; preds = %entry
  %"%140.load" = load i64, ptr %val2, align 8
  store i64 %"%140.load", ptr %"%uac_addr_0", align 8
  br label %merge_46

then_47:                                          ; preds = %merge_46
  %"%144.load" = load i64, ptr %val2, align 8
  store i64 %"%144.load", ptr %"%uac_addr_1", align 8
  br label %merge_48

then_49:                                          ; preds = %merge_48
  %"%148.load" = load i64, ptr %val2, align 8
  store i64 %"%148.load", ptr %"%uac_addr_2", align 8
  br label %merge_50

then_51:                                          ; preds = %merge_50
  %"%152.load" = load i64, ptr %val2, align 8
  store i64 %"%152.load", ptr %"%uac_addr_3", align 8
  br label %merge_52
}

define i64 @uac_get_addr(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_53, label %merge_54

merge_54:                                         ; preds = %entry
  %"%d.load2" = load i64, ptr %d1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%d.load2", 1
  br i1 %cmpeqtmp3, label %then_55, label %merge_56

merge_56:                                         ; preds = %merge_54
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_57, label %merge_58

merge_58:                                         ; preds = %merge_56
  %"%d.load8" = load i64, ptr %d1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 3
  br i1 %cmpeqtmp9, label %then_59, label %merge_60

merge_60:                                         ; preds = %merge_58
  ret i64 0

then_53:                                          ; preds = %entry
  %loadtmp = load i64, ptr @uac_addr_0, align 8
  ret i64 %loadtmp

then_55:                                          ; preds = %merge_54
  %loadtmp4 = load i64, ptr @uac_addr_1, align 8
  ret i64 %loadtmp4

then_57:                                          ; preds = %merge_56
  %loadtmp7 = load i64, ptr @uac_addr_2, align 8
  ret i64 %loadtmp7

then_59:                                          ; preds = %merge_58
  %loadtmp10 = load i64, ptr @uac_addr_3, align 8
  ret i64 %loadtmp10
}

define i64 @uac_get_channels(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_61, label %merge_62

merge_62:                                         ; preds = %entry
  %"%d.load2" = load i64, ptr %d1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%d.load2", 1
  br i1 %cmpeqtmp3, label %then_63, label %merge_64

merge_64:                                         ; preds = %merge_62
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_65, label %merge_66

merge_66:                                         ; preds = %merge_64
  %"%d.load8" = load i64, ptr %d1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 3
  br i1 %cmpeqtmp9, label %then_67, label %merge_68

merge_68:                                         ; preds = %merge_66
  ret i64 2

then_61:                                          ; preds = %entry
  %loadtmp = load i64, ptr @uac_channels_0, align 8
  ret i64 %loadtmp

then_63:                                          ; preds = %merge_62
  %loadtmp4 = load i64, ptr @uac_channels_1, align 8
  ret i64 %loadtmp4

then_65:                                          ; preds = %merge_64
  %loadtmp7 = load i64, ptr @uac_channels_2, align 8
  ret i64 %loadtmp7

then_67:                                          ; preds = %merge_66
  %loadtmp10 = load i64, ptr @uac_channels_3, align 8
  ret i64 %loadtmp10
}

define i64 @uac_set_channels(i64 %d, i64 %val) {
entry:
  %"%uac_channels_3" = alloca i64, align 8
  %"%uac_channels_2" = alloca i64, align 8
  %"%uac_channels_1" = alloca i64, align 8
  %"%uac_channels_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_69, label %merge_70

merge_70:                                         ; preds = %then_69, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_71, label %merge_72

merge_72:                                         ; preds = %then_71, %merge_70
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_73, label %merge_74

merge_74:                                         ; preds = %then_73, %merge_72
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_75, label %merge_76

merge_76:                                         ; preds = %then_75, %merge_74
  ret i64 0

then_69:                                          ; preds = %entry
  %"%199.load" = load i64, ptr %val2, align 8
  store i64 %"%199.load", ptr %"%uac_channels_0", align 8
  br label %merge_70

then_71:                                          ; preds = %merge_70
  %"%203.load" = load i64, ptr %val2, align 8
  store i64 %"%203.load", ptr %"%uac_channels_1", align 8
  br label %merge_72

then_73:                                          ; preds = %merge_72
  %"%207.load" = load i64, ptr %val2, align 8
  store i64 %"%207.load", ptr %"%uac_channels_2", align 8
  br label %merge_74

then_75:                                          ; preds = %merge_74
  %"%211.load" = load i64, ptr %val2, align 8
  store i64 %"%211.load", ptr %"%uac_channels_3", align 8
  br label %merge_76
}

define i64 @uac_get_bits(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_77, label %merge_78

merge_78:                                         ; preds = %entry
  %"%d.load2" = load i64, ptr %d1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%d.load2", 1
  br i1 %cmpeqtmp3, label %then_79, label %merge_80

merge_80:                                         ; preds = %merge_78
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_81, label %merge_82

merge_82:                                         ; preds = %merge_80
  %"%d.load8" = load i64, ptr %d1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 3
  br i1 %cmpeqtmp9, label %then_83, label %merge_84

merge_84:                                         ; preds = %merge_82
  ret i64 16

then_77:                                          ; preds = %entry
  %loadtmp = load i64, ptr @uac_bits_0, align 8
  ret i64 %loadtmp

then_79:                                          ; preds = %merge_78
  %loadtmp4 = load i64, ptr @uac_bits_1, align 8
  ret i64 %loadtmp4

then_81:                                          ; preds = %merge_80
  %loadtmp7 = load i64, ptr @uac_bits_2, align 8
  ret i64 %loadtmp7

then_83:                                          ; preds = %merge_82
  %loadtmp10 = load i64, ptr @uac_bits_3, align 8
  ret i64 %loadtmp10
}

define i64 @uac_set_bits(i64 %d, i64 %val) {
entry:
  %"%uac_bits_3" = alloca i64, align 8
  %"%uac_bits_2" = alloca i64, align 8
  %"%uac_bits_1" = alloca i64, align 8
  %"%uac_bits_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_85, label %merge_86

merge_86:                                         ; preds = %then_85, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_87, label %merge_88

merge_88:                                         ; preds = %then_87, %merge_86
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_89, label %merge_90

merge_90:                                         ; preds = %then_89, %merge_88
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_91, label %merge_92

merge_92:                                         ; preds = %then_91, %merge_90
  ret i64 0

then_85:                                          ; preds = %entry
  %"%237.load" = load i64, ptr %val2, align 8
  store i64 %"%237.load", ptr %"%uac_bits_0", align 8
  br label %merge_86

then_87:                                          ; preds = %merge_86
  %"%241.load" = load i64, ptr %val2, align 8
  store i64 %"%241.load", ptr %"%uac_bits_1", align 8
  br label %merge_88

then_89:                                          ; preds = %merge_88
  %"%245.load" = load i64, ptr %val2, align 8
  store i64 %"%245.load", ptr %"%uac_bits_2", align 8
  br label %merge_90

then_91:                                          ; preds = %merge_90
  %"%249.load" = load i64, ptr %val2, align 8
  store i64 %"%249.load", ptr %"%uac_bits_3", align 8
  br label %merge_92
}

define i64 @uac_get_rate(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_93, label %merge_94

merge_100:                                        ; preds = %merge_98
  ret i64 44100

merge_94:                                         ; preds = %entry
  %"%d.load2" = load i64, ptr %d1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%d.load2", 1
  br i1 %cmpeqtmp3, label %then_95, label %merge_96

merge_96:                                         ; preds = %merge_94
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_97, label %merge_98

merge_98:                                         ; preds = %merge_96
  %"%d.load8" = load i64, ptr %d1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 3
  br i1 %cmpeqtmp9, label %then_99, label %merge_100

then_93:                                          ; preds = %entry
  %loadtmp = load i64, ptr @uac_rate_0, align 8
  ret i64 %loadtmp

then_95:                                          ; preds = %merge_94
  %loadtmp4 = load i64, ptr @uac_rate_1, align 8
  ret i64 %loadtmp4

then_97:                                          ; preds = %merge_96
  %loadtmp7 = load i64, ptr @uac_rate_2, align 8
  ret i64 %loadtmp7

then_99:                                          ; preds = %merge_98
  %loadtmp10 = load i64, ptr @uac_rate_3, align 8
  ret i64 %loadtmp10
}

define i64 @uac_set_rate(i64 %d, i64 %val) {
entry:
  %"%uac_rate_3" = alloca i64, align 8
  %"%uac_rate_2" = alloca i64, align 8
  %"%uac_rate_1" = alloca i64, align 8
  %"%uac_rate_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_101, label %merge_102

merge_102:                                        ; preds = %then_101, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_103, label %merge_104

merge_104:                                        ; preds = %then_103, %merge_102
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_105, label %merge_106

merge_106:                                        ; preds = %then_105, %merge_104
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_107, label %merge_108

merge_108:                                        ; preds = %then_107, %merge_106
  ret i64 0

then_101:                                         ; preds = %entry
  %"%275.load" = load i64, ptr %val2, align 8
  store i64 %"%275.load", ptr %"%uac_rate_0", align 8
  br label %merge_102

then_103:                                         ; preds = %merge_102
  %"%279.load" = load i64, ptr %val2, align 8
  store i64 %"%279.load", ptr %"%uac_rate_1", align 8
  br label %merge_104

then_105:                                         ; preds = %merge_104
  %"%283.load" = load i64, ptr %val2, align 8
  store i64 %"%283.load", ptr %"%uac_rate_2", align 8
  br label %merge_106

then_107:                                         ; preds = %merge_106
  %"%287.load" = load i64, ptr %val2, align 8
  store i64 %"%287.load", ptr %"%uac_rate_3", align 8
  br label %merge_108
}

define i64 @uac_get_streaming(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_109, label %merge_110

merge_110:                                        ; preds = %entry
  %"%d.load2" = load i64, ptr %d1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%d.load2", 1
  br i1 %cmpeqtmp3, label %then_111, label %merge_112

merge_112:                                        ; preds = %merge_110
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_113, label %merge_114

merge_114:                                        ; preds = %merge_112
  %"%d.load8" = load i64, ptr %d1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 3
  br i1 %cmpeqtmp9, label %then_115, label %merge_116

merge_116:                                        ; preds = %merge_114
  ret i64 0

then_109:                                         ; preds = %entry
  %loadtmp = load i64, ptr @uac_streaming_0, align 8
  ret i64 %loadtmp

then_111:                                         ; preds = %merge_110
  %loadtmp4 = load i64, ptr @uac_streaming_1, align 8
  ret i64 %loadtmp4

then_113:                                         ; preds = %merge_112
  %loadtmp7 = load i64, ptr @uac_streaming_2, align 8
  ret i64 %loadtmp7

then_115:                                         ; preds = %merge_114
  %loadtmp10 = load i64, ptr @uac_streaming_3, align 8
  ret i64 %loadtmp10
}

define i64 @uac_set_streaming(i64 %d, i64 %val) {
entry:
  %"%uac_streaming_3" = alloca i64, align 8
  %"%uac_streaming_2" = alloca i64, align 8
  %"%uac_streaming_1" = alloca i64, align 8
  %"%uac_streaming_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_117, label %merge_118

merge_118:                                        ; preds = %then_117, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_119, label %merge_120

merge_120:                                        ; preds = %then_119, %merge_118
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_121, label %merge_122

merge_122:                                        ; preds = %then_121, %merge_120
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_123, label %merge_124

merge_124:                                        ; preds = %then_123, %merge_122
  ret i64 0

then_117:                                         ; preds = %entry
  %"%313.load" = load i64, ptr %val2, align 8
  store i64 %"%313.load", ptr %"%uac_streaming_0", align 8
  br label %merge_118

then_119:                                         ; preds = %merge_118
  %"%317.load" = load i64, ptr %val2, align 8
  store i64 %"%317.load", ptr %"%uac_streaming_1", align 8
  br label %merge_120

then_121:                                         ; preds = %merge_120
  %"%321.load" = load i64, ptr %val2, align 8
  store i64 %"%321.load", ptr %"%uac_streaming_2", align 8
  br label %merge_122

then_123:                                         ; preds = %merge_122
  %"%325.load" = load i64, ptr %val2, align 8
  store i64 %"%325.load", ptr %"%uac_streaming_3", align 8
  br label %merge_124
}

define i64 @uac_set_term_id(i64 %t, i64 %val) {
entry:
  %"%term_id_15" = alloca i64, align 8
  %"%term_id_14" = alloca i64, align 8
  %"%term_id_13" = alloca i64, align 8
  %"%term_id_12" = alloca i64, align 8
  %"%term_id_11" = alloca i64, align 8
  %"%term_id_10" = alloca i64, align 8
  %"%term_id_9" = alloca i64, align 8
  %"%term_id_8" = alloca i64, align 8
  %"%term_id_7" = alloca i64, align 8
  %"%term_id_6" = alloca i64, align 8
  %"%term_id_5" = alloca i64, align 8
  %"%term_id_4" = alloca i64, align 8
  %"%term_id_3" = alloca i64, align 8
  %"%term_id_2" = alloca i64, align 8
  %"%term_id_1" = alloca i64, align 8
  %"%term_id_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %t1 = alloca i64, align 8
  store i64 %t, ptr %t1, align 8
  %"%t.load" = load i64, ptr %t1, align 8
  %cmpeqtmp = icmp eq i64 %"%t.load", 0
  br i1 %cmpeqtmp, label %then_125, label %merge_126

merge_126:                                        ; preds = %then_125, %entry
  %"%t.load3" = load i64, ptr %t1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%t.load3", 1
  br i1 %cmpeqtmp4, label %then_127, label %merge_128

merge_128:                                        ; preds = %then_127, %merge_126
  %"%t.load5" = load i64, ptr %t1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%t.load5", 2
  br i1 %cmpeqtmp6, label %then_129, label %merge_130

merge_130:                                        ; preds = %then_129, %merge_128
  %"%t.load7" = load i64, ptr %t1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%t.load7", 3
  br i1 %cmpeqtmp8, label %then_131, label %merge_132

merge_132:                                        ; preds = %then_131, %merge_130
  %"%t.load9" = load i64, ptr %t1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%t.load9", 4
  br i1 %cmpeqtmp10, label %then_133, label %merge_134

merge_134:                                        ; preds = %then_133, %merge_132
  %"%t.load11" = load i64, ptr %t1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%t.load11", 5
  br i1 %cmpeqtmp12, label %then_135, label %merge_136

merge_136:                                        ; preds = %then_135, %merge_134
  %"%t.load13" = load i64, ptr %t1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%t.load13", 6
  br i1 %cmpeqtmp14, label %then_137, label %merge_138

merge_138:                                        ; preds = %then_137, %merge_136
  %"%t.load15" = load i64, ptr %t1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%t.load15", 7
  br i1 %cmpeqtmp16, label %then_139, label %merge_140

merge_140:                                        ; preds = %then_139, %merge_138
  %"%t.load17" = load i64, ptr %t1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%t.load17", 8
  br i1 %cmpeqtmp18, label %then_141, label %merge_142

merge_142:                                        ; preds = %then_141, %merge_140
  %"%t.load19" = load i64, ptr %t1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%t.load19", 9
  br i1 %cmpeqtmp20, label %then_143, label %merge_144

merge_144:                                        ; preds = %then_143, %merge_142
  %"%t.load21" = load i64, ptr %t1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%t.load21", 10
  br i1 %cmpeqtmp22, label %then_145, label %merge_146

merge_146:                                        ; preds = %then_145, %merge_144
  %"%t.load23" = load i64, ptr %t1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%t.load23", 11
  br i1 %cmpeqtmp24, label %then_147, label %merge_148

merge_148:                                        ; preds = %then_147, %merge_146
  %"%t.load25" = load i64, ptr %t1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%t.load25", 12
  br i1 %cmpeqtmp26, label %then_149, label %merge_150

merge_150:                                        ; preds = %then_149, %merge_148
  %"%t.load27" = load i64, ptr %t1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%t.load27", 13
  br i1 %cmpeqtmp28, label %then_151, label %merge_152

merge_152:                                        ; preds = %then_151, %merge_150
  %"%t.load29" = load i64, ptr %t1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%t.load29", 14
  br i1 %cmpeqtmp30, label %then_153, label %merge_154

merge_154:                                        ; preds = %then_153, %merge_152
  %"%t.load31" = load i64, ptr %t1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%t.load31", 15
  br i1 %cmpeqtmp32, label %then_155, label %merge_156

merge_156:                                        ; preds = %then_155, %merge_154
  ret i64 0

then_125:                                         ; preds = %entry
  %"%330.load" = load i64, ptr %val2, align 8
  store i64 %"%330.load", ptr %"%term_id_0", align 8
  br label %merge_126

then_127:                                         ; preds = %merge_126
  %"%334.load" = load i64, ptr %val2, align 8
  store i64 %"%334.load", ptr %"%term_id_1", align 8
  br label %merge_128

then_129:                                         ; preds = %merge_128
  %"%338.load" = load i64, ptr %val2, align 8
  store i64 %"%338.load", ptr %"%term_id_2", align 8
  br label %merge_130

then_131:                                         ; preds = %merge_130
  %"%342.load" = load i64, ptr %val2, align 8
  store i64 %"%342.load", ptr %"%term_id_3", align 8
  br label %merge_132

then_133:                                         ; preds = %merge_132
  %"%346.load" = load i64, ptr %val2, align 8
  store i64 %"%346.load", ptr %"%term_id_4", align 8
  br label %merge_134

then_135:                                         ; preds = %merge_134
  %"%350.load" = load i64, ptr %val2, align 8
  store i64 %"%350.load", ptr %"%term_id_5", align 8
  br label %merge_136

then_137:                                         ; preds = %merge_136
  %"%354.load" = load i64, ptr %val2, align 8
  store i64 %"%354.load", ptr %"%term_id_6", align 8
  br label %merge_138

then_139:                                         ; preds = %merge_138
  %"%358.load" = load i64, ptr %val2, align 8
  store i64 %"%358.load", ptr %"%term_id_7", align 8
  br label %merge_140

then_141:                                         ; preds = %merge_140
  %"%362.load" = load i64, ptr %val2, align 8
  store i64 %"%362.load", ptr %"%term_id_8", align 8
  br label %merge_142

then_143:                                         ; preds = %merge_142
  %"%366.load" = load i64, ptr %val2, align 8
  store i64 %"%366.load", ptr %"%term_id_9", align 8
  br label %merge_144

then_145:                                         ; preds = %merge_144
  %"%370.load" = load i64, ptr %val2, align 8
  store i64 %"%370.load", ptr %"%term_id_10", align 8
  br label %merge_146

then_147:                                         ; preds = %merge_146
  %"%374.load" = load i64, ptr %val2, align 8
  store i64 %"%374.load", ptr %"%term_id_11", align 8
  br label %merge_148

then_149:                                         ; preds = %merge_148
  %"%378.load" = load i64, ptr %val2, align 8
  store i64 %"%378.load", ptr %"%term_id_12", align 8
  br label %merge_150

then_151:                                         ; preds = %merge_150
  %"%382.load" = load i64, ptr %val2, align 8
  store i64 %"%382.load", ptr %"%term_id_13", align 8
  br label %merge_152

then_153:                                         ; preds = %merge_152
  %"%386.load" = load i64, ptr %val2, align 8
  store i64 %"%386.load", ptr %"%term_id_14", align 8
  br label %merge_154

then_155:                                         ; preds = %merge_154
  %"%390.load" = load i64, ptr %val2, align 8
  store i64 %"%390.load", ptr %"%term_id_15", align 8
  br label %merge_156
}

define i64 @uac_set_term_type(i64 %t, i64 %val) {
entry:
  %"%term_type_15" = alloca i64, align 8
  %"%term_type_14" = alloca i64, align 8
  %"%term_type_13" = alloca i64, align 8
  %"%term_type_12" = alloca i64, align 8
  %"%term_type_11" = alloca i64, align 8
  %"%term_type_10" = alloca i64, align 8
  %"%term_type_9" = alloca i64, align 8
  %"%term_type_8" = alloca i64, align 8
  %"%term_type_7" = alloca i64, align 8
  %"%term_type_6" = alloca i64, align 8
  %"%term_type_5" = alloca i64, align 8
  %"%term_type_4" = alloca i64, align 8
  %"%term_type_3" = alloca i64, align 8
  %"%term_type_2" = alloca i64, align 8
  %"%term_type_1" = alloca i64, align 8
  %"%term_type_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %t1 = alloca i64, align 8
  store i64 %t, ptr %t1, align 8
  %"%t.load" = load i64, ptr %t1, align 8
  %cmpeqtmp = icmp eq i64 %"%t.load", 0
  br i1 %cmpeqtmp, label %then_157, label %merge_158

merge_158:                                        ; preds = %then_157, %entry
  %"%t.load3" = load i64, ptr %t1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%t.load3", 1
  br i1 %cmpeqtmp4, label %then_159, label %merge_160

merge_160:                                        ; preds = %then_159, %merge_158
  %"%t.load5" = load i64, ptr %t1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%t.load5", 2
  br i1 %cmpeqtmp6, label %then_161, label %merge_162

merge_162:                                        ; preds = %then_161, %merge_160
  %"%t.load7" = load i64, ptr %t1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%t.load7", 3
  br i1 %cmpeqtmp8, label %then_163, label %merge_164

merge_164:                                        ; preds = %then_163, %merge_162
  %"%t.load9" = load i64, ptr %t1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%t.load9", 4
  br i1 %cmpeqtmp10, label %then_165, label %merge_166

merge_166:                                        ; preds = %then_165, %merge_164
  %"%t.load11" = load i64, ptr %t1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%t.load11", 5
  br i1 %cmpeqtmp12, label %then_167, label %merge_168

merge_168:                                        ; preds = %then_167, %merge_166
  %"%t.load13" = load i64, ptr %t1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%t.load13", 6
  br i1 %cmpeqtmp14, label %then_169, label %merge_170

merge_170:                                        ; preds = %then_169, %merge_168
  %"%t.load15" = load i64, ptr %t1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%t.load15", 7
  br i1 %cmpeqtmp16, label %then_171, label %merge_172

merge_172:                                        ; preds = %then_171, %merge_170
  %"%t.load17" = load i64, ptr %t1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%t.load17", 8
  br i1 %cmpeqtmp18, label %then_173, label %merge_174

merge_174:                                        ; preds = %then_173, %merge_172
  %"%t.load19" = load i64, ptr %t1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%t.load19", 9
  br i1 %cmpeqtmp20, label %then_175, label %merge_176

merge_176:                                        ; preds = %then_175, %merge_174
  %"%t.load21" = load i64, ptr %t1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%t.load21", 10
  br i1 %cmpeqtmp22, label %then_177, label %merge_178

merge_178:                                        ; preds = %then_177, %merge_176
  %"%t.load23" = load i64, ptr %t1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%t.load23", 11
  br i1 %cmpeqtmp24, label %then_179, label %merge_180

merge_180:                                        ; preds = %then_179, %merge_178
  %"%t.load25" = load i64, ptr %t1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%t.load25", 12
  br i1 %cmpeqtmp26, label %then_181, label %merge_182

merge_182:                                        ; preds = %then_181, %merge_180
  %"%t.load27" = load i64, ptr %t1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%t.load27", 13
  br i1 %cmpeqtmp28, label %then_183, label %merge_184

merge_184:                                        ; preds = %then_183, %merge_182
  %"%t.load29" = load i64, ptr %t1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%t.load29", 14
  br i1 %cmpeqtmp30, label %then_185, label %merge_186

merge_186:                                        ; preds = %then_185, %merge_184
  %"%t.load31" = load i64, ptr %t1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%t.load31", 15
  br i1 %cmpeqtmp32, label %then_187, label %merge_188

merge_188:                                        ; preds = %then_187, %merge_186
  ret i64 0

then_157:                                         ; preds = %entry
  %"%395.load" = load i64, ptr %val2, align 8
  store i64 %"%395.load", ptr %"%term_type_0", align 8
  br label %merge_158

then_159:                                         ; preds = %merge_158
  %"%399.load" = load i64, ptr %val2, align 8
  store i64 %"%399.load", ptr %"%term_type_1", align 8
  br label %merge_160

then_161:                                         ; preds = %merge_160
  %"%403.load" = load i64, ptr %val2, align 8
  store i64 %"%403.load", ptr %"%term_type_2", align 8
  br label %merge_162

then_163:                                         ; preds = %merge_162
  %"%407.load" = load i64, ptr %val2, align 8
  store i64 %"%407.load", ptr %"%term_type_3", align 8
  br label %merge_164

then_165:                                         ; preds = %merge_164
  %"%411.load" = load i64, ptr %val2, align 8
  store i64 %"%411.load", ptr %"%term_type_4", align 8
  br label %merge_166

then_167:                                         ; preds = %merge_166
  %"%415.load" = load i64, ptr %val2, align 8
  store i64 %"%415.load", ptr %"%term_type_5", align 8
  br label %merge_168

then_169:                                         ; preds = %merge_168
  %"%419.load" = load i64, ptr %val2, align 8
  store i64 %"%419.load", ptr %"%term_type_6", align 8
  br label %merge_170

then_171:                                         ; preds = %merge_170
  %"%423.load" = load i64, ptr %val2, align 8
  store i64 %"%423.load", ptr %"%term_type_7", align 8
  br label %merge_172

then_173:                                         ; preds = %merge_172
  %"%427.load" = load i64, ptr %val2, align 8
  store i64 %"%427.load", ptr %"%term_type_8", align 8
  br label %merge_174

then_175:                                         ; preds = %merge_174
  %"%431.load" = load i64, ptr %val2, align 8
  store i64 %"%431.load", ptr %"%term_type_9", align 8
  br label %merge_176

then_177:                                         ; preds = %merge_176
  %"%435.load" = load i64, ptr %val2, align 8
  store i64 %"%435.load", ptr %"%term_type_10", align 8
  br label %merge_178

then_179:                                         ; preds = %merge_178
  %"%439.load" = load i64, ptr %val2, align 8
  store i64 %"%439.load", ptr %"%term_type_11", align 8
  br label %merge_180

then_181:                                         ; preds = %merge_180
  %"%443.load" = load i64, ptr %val2, align 8
  store i64 %"%443.load", ptr %"%term_type_12", align 8
  br label %merge_182

then_183:                                         ; preds = %merge_182
  %"%447.load" = load i64, ptr %val2, align 8
  store i64 %"%447.load", ptr %"%term_type_13", align 8
  br label %merge_184

then_185:                                         ; preds = %merge_184
  %"%451.load" = load i64, ptr %val2, align 8
  store i64 %"%451.load", ptr %"%term_type_14", align 8
  br label %merge_186

then_187:                                         ; preds = %merge_186
  %"%455.load" = load i64, ptr %val2, align 8
  store i64 %"%455.load", ptr %"%term_type_15", align 8
  br label %merge_188
}

define i64 @uac_set_term_dir(i64 %t, i64 %val) {
entry:
  %"%term_dir_15" = alloca i64, align 8
  %"%term_dir_14" = alloca i64, align 8
  %"%term_dir_13" = alloca i64, align 8
  %"%term_dir_12" = alloca i64, align 8
  %"%term_dir_11" = alloca i64, align 8
  %"%term_dir_10" = alloca i64, align 8
  %"%term_dir_9" = alloca i64, align 8
  %"%term_dir_8" = alloca i64, align 8
  %"%term_dir_7" = alloca i64, align 8
  %"%term_dir_6" = alloca i64, align 8
  %"%term_dir_5" = alloca i64, align 8
  %"%term_dir_4" = alloca i64, align 8
  %"%term_dir_3" = alloca i64, align 8
  %"%term_dir_2" = alloca i64, align 8
  %"%term_dir_1" = alloca i64, align 8
  %"%term_dir_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %t1 = alloca i64, align 8
  store i64 %t, ptr %t1, align 8
  %"%t.load" = load i64, ptr %t1, align 8
  %cmpeqtmp = icmp eq i64 %"%t.load", 0
  br i1 %cmpeqtmp, label %then_189, label %merge_190

merge_190:                                        ; preds = %then_189, %entry
  %"%t.load3" = load i64, ptr %t1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%t.load3", 1
  br i1 %cmpeqtmp4, label %then_191, label %merge_192

merge_192:                                        ; preds = %then_191, %merge_190
  %"%t.load5" = load i64, ptr %t1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%t.load5", 2
  br i1 %cmpeqtmp6, label %then_193, label %merge_194

merge_194:                                        ; preds = %then_193, %merge_192
  %"%t.load7" = load i64, ptr %t1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%t.load7", 3
  br i1 %cmpeqtmp8, label %then_195, label %merge_196

merge_196:                                        ; preds = %then_195, %merge_194
  %"%t.load9" = load i64, ptr %t1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%t.load9", 4
  br i1 %cmpeqtmp10, label %then_197, label %merge_198

merge_198:                                        ; preds = %then_197, %merge_196
  %"%t.load11" = load i64, ptr %t1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%t.load11", 5
  br i1 %cmpeqtmp12, label %then_199, label %merge_200

merge_200:                                        ; preds = %then_199, %merge_198
  %"%t.load13" = load i64, ptr %t1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%t.load13", 6
  br i1 %cmpeqtmp14, label %then_201, label %merge_202

merge_202:                                        ; preds = %then_201, %merge_200
  %"%t.load15" = load i64, ptr %t1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%t.load15", 7
  br i1 %cmpeqtmp16, label %then_203, label %merge_204

merge_204:                                        ; preds = %then_203, %merge_202
  %"%t.load17" = load i64, ptr %t1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%t.load17", 8
  br i1 %cmpeqtmp18, label %then_205, label %merge_206

merge_206:                                        ; preds = %then_205, %merge_204
  %"%t.load19" = load i64, ptr %t1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%t.load19", 9
  br i1 %cmpeqtmp20, label %then_207, label %merge_208

merge_208:                                        ; preds = %then_207, %merge_206
  %"%t.load21" = load i64, ptr %t1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%t.load21", 10
  br i1 %cmpeqtmp22, label %then_209, label %merge_210

merge_210:                                        ; preds = %then_209, %merge_208
  %"%t.load23" = load i64, ptr %t1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%t.load23", 11
  br i1 %cmpeqtmp24, label %then_211, label %merge_212

merge_212:                                        ; preds = %then_211, %merge_210
  %"%t.load25" = load i64, ptr %t1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%t.load25", 12
  br i1 %cmpeqtmp26, label %then_213, label %merge_214

merge_214:                                        ; preds = %then_213, %merge_212
  %"%t.load27" = load i64, ptr %t1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%t.load27", 13
  br i1 %cmpeqtmp28, label %then_215, label %merge_216

merge_216:                                        ; preds = %then_215, %merge_214
  %"%t.load29" = load i64, ptr %t1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%t.load29", 14
  br i1 %cmpeqtmp30, label %then_217, label %merge_218

merge_218:                                        ; preds = %then_217, %merge_216
  %"%t.load31" = load i64, ptr %t1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%t.load31", 15
  br i1 %cmpeqtmp32, label %then_219, label %merge_220

merge_220:                                        ; preds = %then_219, %merge_218
  ret i64 0

then_189:                                         ; preds = %entry
  %"%460.load" = load i64, ptr %val2, align 8
  store i64 %"%460.load", ptr %"%term_dir_0", align 8
  br label %merge_190

then_191:                                         ; preds = %merge_190
  %"%464.load" = load i64, ptr %val2, align 8
  store i64 %"%464.load", ptr %"%term_dir_1", align 8
  br label %merge_192

then_193:                                         ; preds = %merge_192
  %"%468.load" = load i64, ptr %val2, align 8
  store i64 %"%468.load", ptr %"%term_dir_2", align 8
  br label %merge_194

then_195:                                         ; preds = %merge_194
  %"%472.load" = load i64, ptr %val2, align 8
  store i64 %"%472.load", ptr %"%term_dir_3", align 8
  br label %merge_196

then_197:                                         ; preds = %merge_196
  %"%476.load" = load i64, ptr %val2, align 8
  store i64 %"%476.load", ptr %"%term_dir_4", align 8
  br label %merge_198

then_199:                                         ; preds = %merge_198
  %"%480.load" = load i64, ptr %val2, align 8
  store i64 %"%480.load", ptr %"%term_dir_5", align 8
  br label %merge_200

then_201:                                         ; preds = %merge_200
  %"%484.load" = load i64, ptr %val2, align 8
  store i64 %"%484.load", ptr %"%term_dir_6", align 8
  br label %merge_202

then_203:                                         ; preds = %merge_202
  %"%488.load" = load i64, ptr %val2, align 8
  store i64 %"%488.load", ptr %"%term_dir_7", align 8
  br label %merge_204

then_205:                                         ; preds = %merge_204
  %"%492.load" = load i64, ptr %val2, align 8
  store i64 %"%492.load", ptr %"%term_dir_8", align 8
  br label %merge_206

then_207:                                         ; preds = %merge_206
  %"%496.load" = load i64, ptr %val2, align 8
  store i64 %"%496.load", ptr %"%term_dir_9", align 8
  br label %merge_208

then_209:                                         ; preds = %merge_208
  %"%500.load" = load i64, ptr %val2, align 8
  store i64 %"%500.load", ptr %"%term_dir_10", align 8
  br label %merge_210

then_211:                                         ; preds = %merge_210
  %"%504.load" = load i64, ptr %val2, align 8
  store i64 %"%504.load", ptr %"%term_dir_11", align 8
  br label %merge_212

then_213:                                         ; preds = %merge_212
  %"%508.load" = load i64, ptr %val2, align 8
  store i64 %"%508.load", ptr %"%term_dir_12", align 8
  br label %merge_214

then_215:                                         ; preds = %merge_214
  %"%512.load" = load i64, ptr %val2, align 8
  store i64 %"%512.load", ptr %"%term_dir_13", align 8
  br label %merge_216

then_217:                                         ; preds = %merge_216
  %"%516.load" = load i64, ptr %val2, align 8
  store i64 %"%516.load", ptr %"%term_dir_14", align 8
  br label %merge_218

then_219:                                         ; preds = %merge_218
  %"%520.load" = load i64, ptr %val2, align 8
  store i64 %"%520.load", ptr %"%term_dir_15", align 8
  br label %merge_220
}

define i64 @uac_set_term_ch(i64 %t, i64 %val) {
entry:
  %"%term_ch_15" = alloca i64, align 8
  %"%term_ch_14" = alloca i64, align 8
  %"%term_ch_13" = alloca i64, align 8
  %"%term_ch_12" = alloca i64, align 8
  %"%term_ch_11" = alloca i64, align 8
  %"%term_ch_10" = alloca i64, align 8
  %"%term_ch_9" = alloca i64, align 8
  %"%term_ch_8" = alloca i64, align 8
  %"%term_ch_7" = alloca i64, align 8
  %"%term_ch_6" = alloca i64, align 8
  %"%term_ch_5" = alloca i64, align 8
  %"%term_ch_4" = alloca i64, align 8
  %"%term_ch_3" = alloca i64, align 8
  %"%term_ch_2" = alloca i64, align 8
  %"%term_ch_1" = alloca i64, align 8
  %"%term_ch_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %t1 = alloca i64, align 8
  store i64 %t, ptr %t1, align 8
  %"%t.load" = load i64, ptr %t1, align 8
  %cmpeqtmp = icmp eq i64 %"%t.load", 0
  br i1 %cmpeqtmp, label %then_221, label %merge_222

merge_222:                                        ; preds = %then_221, %entry
  %"%t.load3" = load i64, ptr %t1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%t.load3", 1
  br i1 %cmpeqtmp4, label %then_223, label %merge_224

merge_224:                                        ; preds = %then_223, %merge_222
  %"%t.load5" = load i64, ptr %t1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%t.load5", 2
  br i1 %cmpeqtmp6, label %then_225, label %merge_226

merge_226:                                        ; preds = %then_225, %merge_224
  %"%t.load7" = load i64, ptr %t1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%t.load7", 3
  br i1 %cmpeqtmp8, label %then_227, label %merge_228

merge_228:                                        ; preds = %then_227, %merge_226
  %"%t.load9" = load i64, ptr %t1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%t.load9", 4
  br i1 %cmpeqtmp10, label %then_229, label %merge_230

merge_230:                                        ; preds = %then_229, %merge_228
  %"%t.load11" = load i64, ptr %t1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%t.load11", 5
  br i1 %cmpeqtmp12, label %then_231, label %merge_232

merge_232:                                        ; preds = %then_231, %merge_230
  %"%t.load13" = load i64, ptr %t1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%t.load13", 6
  br i1 %cmpeqtmp14, label %then_233, label %merge_234

merge_234:                                        ; preds = %then_233, %merge_232
  %"%t.load15" = load i64, ptr %t1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%t.load15", 7
  br i1 %cmpeqtmp16, label %then_235, label %merge_236

merge_236:                                        ; preds = %then_235, %merge_234
  %"%t.load17" = load i64, ptr %t1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%t.load17", 8
  br i1 %cmpeqtmp18, label %then_237, label %merge_238

merge_238:                                        ; preds = %then_237, %merge_236
  %"%t.load19" = load i64, ptr %t1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%t.load19", 9
  br i1 %cmpeqtmp20, label %then_239, label %merge_240

merge_240:                                        ; preds = %then_239, %merge_238
  %"%t.load21" = load i64, ptr %t1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%t.load21", 10
  br i1 %cmpeqtmp22, label %then_241, label %merge_242

merge_242:                                        ; preds = %then_241, %merge_240
  %"%t.load23" = load i64, ptr %t1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%t.load23", 11
  br i1 %cmpeqtmp24, label %then_243, label %merge_244

merge_244:                                        ; preds = %then_243, %merge_242
  %"%t.load25" = load i64, ptr %t1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%t.load25", 12
  br i1 %cmpeqtmp26, label %then_245, label %merge_246

merge_246:                                        ; preds = %then_245, %merge_244
  %"%t.load27" = load i64, ptr %t1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%t.load27", 13
  br i1 %cmpeqtmp28, label %then_247, label %merge_248

merge_248:                                        ; preds = %then_247, %merge_246
  %"%t.load29" = load i64, ptr %t1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%t.load29", 14
  br i1 %cmpeqtmp30, label %then_249, label %merge_250

merge_250:                                        ; preds = %then_249, %merge_248
  %"%t.load31" = load i64, ptr %t1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%t.load31", 15
  br i1 %cmpeqtmp32, label %then_251, label %merge_252

merge_252:                                        ; preds = %then_251, %merge_250
  ret i64 0

then_221:                                         ; preds = %entry
  %"%525.load" = load i64, ptr %val2, align 8
  store i64 %"%525.load", ptr %"%term_ch_0", align 8
  br label %merge_222

then_223:                                         ; preds = %merge_222
  %"%529.load" = load i64, ptr %val2, align 8
  store i64 %"%529.load", ptr %"%term_ch_1", align 8
  br label %merge_224

then_225:                                         ; preds = %merge_224
  %"%533.load" = load i64, ptr %val2, align 8
  store i64 %"%533.load", ptr %"%term_ch_2", align 8
  br label %merge_226

then_227:                                         ; preds = %merge_226
  %"%537.load" = load i64, ptr %val2, align 8
  store i64 %"%537.load", ptr %"%term_ch_3", align 8
  br label %merge_228

then_229:                                         ; preds = %merge_228
  %"%541.load" = load i64, ptr %val2, align 8
  store i64 %"%541.load", ptr %"%term_ch_4", align 8
  br label %merge_230

then_231:                                         ; preds = %merge_230
  %"%545.load" = load i64, ptr %val2, align 8
  store i64 %"%545.load", ptr %"%term_ch_5", align 8
  br label %merge_232

then_233:                                         ; preds = %merge_232
  %"%549.load" = load i64, ptr %val2, align 8
  store i64 %"%549.load", ptr %"%term_ch_6", align 8
  br label %merge_234

then_235:                                         ; preds = %merge_234
  %"%553.load" = load i64, ptr %val2, align 8
  store i64 %"%553.load", ptr %"%term_ch_7", align 8
  br label %merge_236

then_237:                                         ; preds = %merge_236
  %"%557.load" = load i64, ptr %val2, align 8
  store i64 %"%557.load", ptr %"%term_ch_8", align 8
  br label %merge_238

then_239:                                         ; preds = %merge_238
  %"%561.load" = load i64, ptr %val2, align 8
  store i64 %"%561.load", ptr %"%term_ch_9", align 8
  br label %merge_240

then_241:                                         ; preds = %merge_240
  %"%565.load" = load i64, ptr %val2, align 8
  store i64 %"%565.load", ptr %"%term_ch_10", align 8
  br label %merge_242

then_243:                                         ; preds = %merge_242
  %"%569.load" = load i64, ptr %val2, align 8
  store i64 %"%569.load", ptr %"%term_ch_11", align 8
  br label %merge_244

then_245:                                         ; preds = %merge_244
  %"%573.load" = load i64, ptr %val2, align 8
  store i64 %"%573.load", ptr %"%term_ch_12", align 8
  br label %merge_246

then_247:                                         ; preds = %merge_246
  %"%577.load" = load i64, ptr %val2, align 8
  store i64 %"%577.load", ptr %"%term_ch_13", align 8
  br label %merge_248

then_249:                                         ; preds = %merge_248
  %"%581.load" = load i64, ptr %val2, align 8
  store i64 %"%581.load", ptr %"%term_ch_14", align 8
  br label %merge_250

then_251:                                         ; preds = %merge_250
  %"%585.load" = load i64, ptr %val2, align 8
  store i64 %"%585.load", ptr %"%term_ch_15", align 8
  br label %merge_252
}

define i64 @uac_set_term_dev(i64 %t, i64 %val) {
entry:
  %"%term_dev_15" = alloca i64, align 8
  %"%term_dev_14" = alloca i64, align 8
  %"%term_dev_13" = alloca i64, align 8
  %"%term_dev_12" = alloca i64, align 8
  %"%term_dev_11" = alloca i64, align 8
  %"%term_dev_10" = alloca i64, align 8
  %"%term_dev_9" = alloca i64, align 8
  %"%term_dev_8" = alloca i64, align 8
  %"%term_dev_7" = alloca i64, align 8
  %"%term_dev_6" = alloca i64, align 8
  %"%term_dev_5" = alloca i64, align 8
  %"%term_dev_4" = alloca i64, align 8
  %"%term_dev_3" = alloca i64, align 8
  %"%term_dev_2" = alloca i64, align 8
  %"%term_dev_1" = alloca i64, align 8
  %"%term_dev_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %t1 = alloca i64, align 8
  store i64 %t, ptr %t1, align 8
  %"%t.load" = load i64, ptr %t1, align 8
  %cmpeqtmp = icmp eq i64 %"%t.load", 0
  br i1 %cmpeqtmp, label %then_253, label %merge_254

merge_254:                                        ; preds = %then_253, %entry
  %"%t.load3" = load i64, ptr %t1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%t.load3", 1
  br i1 %cmpeqtmp4, label %then_255, label %merge_256

merge_256:                                        ; preds = %then_255, %merge_254
  %"%t.load5" = load i64, ptr %t1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%t.load5", 2
  br i1 %cmpeqtmp6, label %then_257, label %merge_258

merge_258:                                        ; preds = %then_257, %merge_256
  %"%t.load7" = load i64, ptr %t1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%t.load7", 3
  br i1 %cmpeqtmp8, label %then_259, label %merge_260

merge_260:                                        ; preds = %then_259, %merge_258
  %"%t.load9" = load i64, ptr %t1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%t.load9", 4
  br i1 %cmpeqtmp10, label %then_261, label %merge_262

merge_262:                                        ; preds = %then_261, %merge_260
  %"%t.load11" = load i64, ptr %t1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%t.load11", 5
  br i1 %cmpeqtmp12, label %then_263, label %merge_264

merge_264:                                        ; preds = %then_263, %merge_262
  %"%t.load13" = load i64, ptr %t1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%t.load13", 6
  br i1 %cmpeqtmp14, label %then_265, label %merge_266

merge_266:                                        ; preds = %then_265, %merge_264
  %"%t.load15" = load i64, ptr %t1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%t.load15", 7
  br i1 %cmpeqtmp16, label %then_267, label %merge_268

merge_268:                                        ; preds = %then_267, %merge_266
  %"%t.load17" = load i64, ptr %t1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%t.load17", 8
  br i1 %cmpeqtmp18, label %then_269, label %merge_270

merge_270:                                        ; preds = %then_269, %merge_268
  %"%t.load19" = load i64, ptr %t1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%t.load19", 9
  br i1 %cmpeqtmp20, label %then_271, label %merge_272

merge_272:                                        ; preds = %then_271, %merge_270
  %"%t.load21" = load i64, ptr %t1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%t.load21", 10
  br i1 %cmpeqtmp22, label %then_273, label %merge_274

merge_274:                                        ; preds = %then_273, %merge_272
  %"%t.load23" = load i64, ptr %t1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%t.load23", 11
  br i1 %cmpeqtmp24, label %then_275, label %merge_276

merge_276:                                        ; preds = %then_275, %merge_274
  %"%t.load25" = load i64, ptr %t1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%t.load25", 12
  br i1 %cmpeqtmp26, label %then_277, label %merge_278

merge_278:                                        ; preds = %then_277, %merge_276
  %"%t.load27" = load i64, ptr %t1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%t.load27", 13
  br i1 %cmpeqtmp28, label %then_279, label %merge_280

merge_280:                                        ; preds = %then_279, %merge_278
  %"%t.load29" = load i64, ptr %t1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%t.load29", 14
  br i1 %cmpeqtmp30, label %then_281, label %merge_282

merge_282:                                        ; preds = %then_281, %merge_280
  %"%t.load31" = load i64, ptr %t1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%t.load31", 15
  br i1 %cmpeqtmp32, label %then_283, label %merge_284

merge_284:                                        ; preds = %then_283, %merge_282
  ret i64 0

then_253:                                         ; preds = %entry
  %"%590.load" = load i64, ptr %val2, align 8
  store i64 %"%590.load", ptr %"%term_dev_0", align 8
  br label %merge_254

then_255:                                         ; preds = %merge_254
  %"%594.load" = load i64, ptr %val2, align 8
  store i64 %"%594.load", ptr %"%term_dev_1", align 8
  br label %merge_256

then_257:                                         ; preds = %merge_256
  %"%598.load" = load i64, ptr %val2, align 8
  store i64 %"%598.load", ptr %"%term_dev_2", align 8
  br label %merge_258

then_259:                                         ; preds = %merge_258
  %"%602.load" = load i64, ptr %val2, align 8
  store i64 %"%602.load", ptr %"%term_dev_3", align 8
  br label %merge_260

then_261:                                         ; preds = %merge_260
  %"%606.load" = load i64, ptr %val2, align 8
  store i64 %"%606.load", ptr %"%term_dev_4", align 8
  br label %merge_262

then_263:                                         ; preds = %merge_262
  %"%610.load" = load i64, ptr %val2, align 8
  store i64 %"%610.load", ptr %"%term_dev_5", align 8
  br label %merge_264

then_265:                                         ; preds = %merge_264
  %"%614.load" = load i64, ptr %val2, align 8
  store i64 %"%614.load", ptr %"%term_dev_6", align 8
  br label %merge_266

then_267:                                         ; preds = %merge_266
  %"%618.load" = load i64, ptr %val2, align 8
  store i64 %"%618.load", ptr %"%term_dev_7", align 8
  br label %merge_268

then_269:                                         ; preds = %merge_268
  %"%622.load" = load i64, ptr %val2, align 8
  store i64 %"%622.load", ptr %"%term_dev_8", align 8
  br label %merge_270

then_271:                                         ; preds = %merge_270
  %"%626.load" = load i64, ptr %val2, align 8
  store i64 %"%626.load", ptr %"%term_dev_9", align 8
  br label %merge_272

then_273:                                         ; preds = %merge_272
  %"%630.load" = load i64, ptr %val2, align 8
  store i64 %"%630.load", ptr %"%term_dev_10", align 8
  br label %merge_274

then_275:                                         ; preds = %merge_274
  %"%634.load" = load i64, ptr %val2, align 8
  store i64 %"%634.load", ptr %"%term_dev_11", align 8
  br label %merge_276

then_277:                                         ; preds = %merge_276
  %"%638.load" = load i64, ptr %val2, align 8
  store i64 %"%638.load", ptr %"%term_dev_12", align 8
  br label %merge_278

then_279:                                         ; preds = %merge_278
  %"%642.load" = load i64, ptr %val2, align 8
  store i64 %"%642.load", ptr %"%term_dev_13", align 8
  br label %merge_280

then_281:                                         ; preds = %merge_280
  %"%646.load" = load i64, ptr %val2, align 8
  store i64 %"%646.load", ptr %"%term_dev_14", align 8
  br label %merge_282

then_283:                                         ; preds = %merge_282
  %"%650.load" = load i64, ptr %val2, align 8
  store i64 %"%650.load", ptr %"%term_dev_15", align 8
  br label %merge_284
}

define i64 @uac_set_fu_id(i64 %f, i64 %val) {
entry:
  %"%fu_id_7" = alloca i64, align 8
  %"%fu_id_6" = alloca i64, align 8
  %"%fu_id_5" = alloca i64, align 8
  %"%fu_id_4" = alloca i64, align 8
  %"%fu_id_3" = alloca i64, align 8
  %"%fu_id_2" = alloca i64, align 8
  %"%fu_id_1" = alloca i64, align 8
  %"%fu_id_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %f1 = alloca i64, align 8
  store i64 %f, ptr %f1, align 8
  %"%f.load" = load i64, ptr %f1, align 8
  %cmpeqtmp = icmp eq i64 %"%f.load", 0
  br i1 %cmpeqtmp, label %then_285, label %merge_286

merge_286:                                        ; preds = %then_285, %entry
  %"%f.load3" = load i64, ptr %f1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%f.load3", 1
  br i1 %cmpeqtmp4, label %then_287, label %merge_288

merge_288:                                        ; preds = %then_287, %merge_286
  %"%f.load5" = load i64, ptr %f1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%f.load5", 2
  br i1 %cmpeqtmp6, label %then_289, label %merge_290

merge_290:                                        ; preds = %then_289, %merge_288
  %"%f.load7" = load i64, ptr %f1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%f.load7", 3
  br i1 %cmpeqtmp8, label %then_291, label %merge_292

merge_292:                                        ; preds = %then_291, %merge_290
  %"%f.load9" = load i64, ptr %f1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%f.load9", 4
  br i1 %cmpeqtmp10, label %then_293, label %merge_294

merge_294:                                        ; preds = %then_293, %merge_292
  %"%f.load11" = load i64, ptr %f1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%f.load11", 5
  br i1 %cmpeqtmp12, label %then_295, label %merge_296

merge_296:                                        ; preds = %then_295, %merge_294
  %"%f.load13" = load i64, ptr %f1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%f.load13", 6
  br i1 %cmpeqtmp14, label %then_297, label %merge_298

merge_298:                                        ; preds = %then_297, %merge_296
  %"%f.load15" = load i64, ptr %f1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%f.load15", 7
  br i1 %cmpeqtmp16, label %then_299, label %merge_300

merge_300:                                        ; preds = %then_299, %merge_298
  ret i64 0

then_285:                                         ; preds = %entry
  %"%655.load" = load i64, ptr %val2, align 8
  store i64 %"%655.load", ptr %"%fu_id_0", align 8
  br label %merge_286

then_287:                                         ; preds = %merge_286
  %"%659.load" = load i64, ptr %val2, align 8
  store i64 %"%659.load", ptr %"%fu_id_1", align 8
  br label %merge_288

then_289:                                         ; preds = %merge_288
  %"%663.load" = load i64, ptr %val2, align 8
  store i64 %"%663.load", ptr %"%fu_id_2", align 8
  br label %merge_290

then_291:                                         ; preds = %merge_290
  %"%667.load" = load i64, ptr %val2, align 8
  store i64 %"%667.load", ptr %"%fu_id_3", align 8
  br label %merge_292

then_293:                                         ; preds = %merge_292
  %"%671.load" = load i64, ptr %val2, align 8
  store i64 %"%671.load", ptr %"%fu_id_4", align 8
  br label %merge_294

then_295:                                         ; preds = %merge_294
  %"%675.load" = load i64, ptr %val2, align 8
  store i64 %"%675.load", ptr %"%fu_id_5", align 8
  br label %merge_296

then_297:                                         ; preds = %merge_296
  %"%679.load" = load i64, ptr %val2, align 8
  store i64 %"%679.load", ptr %"%fu_id_6", align 8
  br label %merge_298

then_299:                                         ; preds = %merge_298
  %"%683.load" = load i64, ptr %val2, align 8
  store i64 %"%683.load", ptr %"%fu_id_7", align 8
  br label %merge_300
}

define i64 @uac_set_fu_src(i64 %f, i64 %val) {
entry:
  %"%fu_src_7" = alloca i64, align 8
  %"%fu_src_6" = alloca i64, align 8
  %"%fu_src_5" = alloca i64, align 8
  %"%fu_src_4" = alloca i64, align 8
  %"%fu_src_3" = alloca i64, align 8
  %"%fu_src_2" = alloca i64, align 8
  %"%fu_src_1" = alloca i64, align 8
  %"%fu_src_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %f1 = alloca i64, align 8
  store i64 %f, ptr %f1, align 8
  %"%f.load" = load i64, ptr %f1, align 8
  %cmpeqtmp = icmp eq i64 %"%f.load", 0
  br i1 %cmpeqtmp, label %then_301, label %merge_302

merge_302:                                        ; preds = %then_301, %entry
  %"%f.load3" = load i64, ptr %f1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%f.load3", 1
  br i1 %cmpeqtmp4, label %then_303, label %merge_304

merge_304:                                        ; preds = %then_303, %merge_302
  %"%f.load5" = load i64, ptr %f1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%f.load5", 2
  br i1 %cmpeqtmp6, label %then_305, label %merge_306

merge_306:                                        ; preds = %then_305, %merge_304
  %"%f.load7" = load i64, ptr %f1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%f.load7", 3
  br i1 %cmpeqtmp8, label %then_307, label %merge_308

merge_308:                                        ; preds = %then_307, %merge_306
  %"%f.load9" = load i64, ptr %f1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%f.load9", 4
  br i1 %cmpeqtmp10, label %then_309, label %merge_310

merge_310:                                        ; preds = %then_309, %merge_308
  %"%f.load11" = load i64, ptr %f1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%f.load11", 5
  br i1 %cmpeqtmp12, label %then_311, label %merge_312

merge_312:                                        ; preds = %then_311, %merge_310
  %"%f.load13" = load i64, ptr %f1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%f.load13", 6
  br i1 %cmpeqtmp14, label %then_313, label %merge_314

merge_314:                                        ; preds = %then_313, %merge_312
  %"%f.load15" = load i64, ptr %f1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%f.load15", 7
  br i1 %cmpeqtmp16, label %then_315, label %merge_316

merge_316:                                        ; preds = %then_315, %merge_314
  ret i64 0

then_301:                                         ; preds = %entry
  %"%688.load" = load i64, ptr %val2, align 8
  store i64 %"%688.load", ptr %"%fu_src_0", align 8
  br label %merge_302

then_303:                                         ; preds = %merge_302
  %"%692.load" = load i64, ptr %val2, align 8
  store i64 %"%692.load", ptr %"%fu_src_1", align 8
  br label %merge_304

then_305:                                         ; preds = %merge_304
  %"%696.load" = load i64, ptr %val2, align 8
  store i64 %"%696.load", ptr %"%fu_src_2", align 8
  br label %merge_306

then_307:                                         ; preds = %merge_306
  %"%700.load" = load i64, ptr %val2, align 8
  store i64 %"%700.load", ptr %"%fu_src_3", align 8
  br label %merge_308

then_309:                                         ; preds = %merge_308
  %"%704.load" = load i64, ptr %val2, align 8
  store i64 %"%704.load", ptr %"%fu_src_4", align 8
  br label %merge_310

then_311:                                         ; preds = %merge_310
  %"%708.load" = load i64, ptr %val2, align 8
  store i64 %"%708.load", ptr %"%fu_src_5", align 8
  br label %merge_312

then_313:                                         ; preds = %merge_312
  %"%712.load" = load i64, ptr %val2, align 8
  store i64 %"%712.load", ptr %"%fu_src_6", align 8
  br label %merge_314

then_315:                                         ; preds = %merge_314
  %"%716.load" = load i64, ptr %val2, align 8
  store i64 %"%716.load", ptr %"%fu_src_7", align 8
  br label %merge_316
}

define i64 @uac_set_fu_ctrl(i64 %f, i64 %val) {
entry:
  %"%fu_ctrl_7" = alloca i64, align 8
  %"%fu_ctrl_6" = alloca i64, align 8
  %"%fu_ctrl_5" = alloca i64, align 8
  %"%fu_ctrl_4" = alloca i64, align 8
  %"%fu_ctrl_3" = alloca i64, align 8
  %"%fu_ctrl_2" = alloca i64, align 8
  %"%fu_ctrl_1" = alloca i64, align 8
  %"%fu_ctrl_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %f1 = alloca i64, align 8
  store i64 %f, ptr %f1, align 8
  %"%f.load" = load i64, ptr %f1, align 8
  %cmpeqtmp = icmp eq i64 %"%f.load", 0
  br i1 %cmpeqtmp, label %then_317, label %merge_318

merge_318:                                        ; preds = %then_317, %entry
  %"%f.load3" = load i64, ptr %f1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%f.load3", 1
  br i1 %cmpeqtmp4, label %then_319, label %merge_320

merge_320:                                        ; preds = %then_319, %merge_318
  %"%f.load5" = load i64, ptr %f1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%f.load5", 2
  br i1 %cmpeqtmp6, label %then_321, label %merge_322

merge_322:                                        ; preds = %then_321, %merge_320
  %"%f.load7" = load i64, ptr %f1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%f.load7", 3
  br i1 %cmpeqtmp8, label %then_323, label %merge_324

merge_324:                                        ; preds = %then_323, %merge_322
  %"%f.load9" = load i64, ptr %f1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%f.load9", 4
  br i1 %cmpeqtmp10, label %then_325, label %merge_326

merge_326:                                        ; preds = %then_325, %merge_324
  %"%f.load11" = load i64, ptr %f1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%f.load11", 5
  br i1 %cmpeqtmp12, label %then_327, label %merge_328

merge_328:                                        ; preds = %then_327, %merge_326
  %"%f.load13" = load i64, ptr %f1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%f.load13", 6
  br i1 %cmpeqtmp14, label %then_329, label %merge_330

merge_330:                                        ; preds = %then_329, %merge_328
  %"%f.load15" = load i64, ptr %f1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%f.load15", 7
  br i1 %cmpeqtmp16, label %then_331, label %merge_332

merge_332:                                        ; preds = %then_331, %merge_330
  ret i64 0

then_317:                                         ; preds = %entry
  %"%721.load" = load i64, ptr %val2, align 8
  store i64 %"%721.load", ptr %"%fu_ctrl_0", align 8
  br label %merge_318

then_319:                                         ; preds = %merge_318
  %"%725.load" = load i64, ptr %val2, align 8
  store i64 %"%725.load", ptr %"%fu_ctrl_1", align 8
  br label %merge_320

then_321:                                         ; preds = %merge_320
  %"%729.load" = load i64, ptr %val2, align 8
  store i64 %"%729.load", ptr %"%fu_ctrl_2", align 8
  br label %merge_322

then_323:                                         ; preds = %merge_322
  %"%733.load" = load i64, ptr %val2, align 8
  store i64 %"%733.load", ptr %"%fu_ctrl_3", align 8
  br label %merge_324

then_325:                                         ; preds = %merge_324
  %"%737.load" = load i64, ptr %val2, align 8
  store i64 %"%737.load", ptr %"%fu_ctrl_4", align 8
  br label %merge_326

then_327:                                         ; preds = %merge_326
  %"%741.load" = load i64, ptr %val2, align 8
  store i64 %"%741.load", ptr %"%fu_ctrl_5", align 8
  br label %merge_328

then_329:                                         ; preds = %merge_328
  %"%745.load" = load i64, ptr %val2, align 8
  store i64 %"%745.load", ptr %"%fu_ctrl_6", align 8
  br label %merge_330

then_331:                                         ; preds = %merge_330
  %"%749.load" = load i64, ptr %val2, align 8
  store i64 %"%749.load", ptr %"%fu_ctrl_7", align 8
  br label %merge_332
}

define i64 @uac_set_fu_dev(i64 %f, i64 %val) {
entry:
  %"%fu_dev_7" = alloca i64, align 8
  %"%fu_dev_6" = alloca i64, align 8
  %"%fu_dev_5" = alloca i64, align 8
  %"%fu_dev_4" = alloca i64, align 8
  %"%fu_dev_3" = alloca i64, align 8
  %"%fu_dev_2" = alloca i64, align 8
  %"%fu_dev_1" = alloca i64, align 8
  %"%fu_dev_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %f1 = alloca i64, align 8
  store i64 %f, ptr %f1, align 8
  %"%f.load" = load i64, ptr %f1, align 8
  %cmpeqtmp = icmp eq i64 %"%f.load", 0
  br i1 %cmpeqtmp, label %then_333, label %merge_334

merge_334:                                        ; preds = %then_333, %entry
  %"%f.load3" = load i64, ptr %f1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%f.load3", 1
  br i1 %cmpeqtmp4, label %then_335, label %merge_336

merge_336:                                        ; preds = %then_335, %merge_334
  %"%f.load5" = load i64, ptr %f1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%f.load5", 2
  br i1 %cmpeqtmp6, label %then_337, label %merge_338

merge_338:                                        ; preds = %then_337, %merge_336
  %"%f.load7" = load i64, ptr %f1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%f.load7", 3
  br i1 %cmpeqtmp8, label %then_339, label %merge_340

merge_340:                                        ; preds = %then_339, %merge_338
  %"%f.load9" = load i64, ptr %f1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%f.load9", 4
  br i1 %cmpeqtmp10, label %then_341, label %merge_342

merge_342:                                        ; preds = %then_341, %merge_340
  %"%f.load11" = load i64, ptr %f1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%f.load11", 5
  br i1 %cmpeqtmp12, label %then_343, label %merge_344

merge_344:                                        ; preds = %then_343, %merge_342
  %"%f.load13" = load i64, ptr %f1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%f.load13", 6
  br i1 %cmpeqtmp14, label %then_345, label %merge_346

merge_346:                                        ; preds = %then_345, %merge_344
  %"%f.load15" = load i64, ptr %f1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%f.load15", 7
  br i1 %cmpeqtmp16, label %then_347, label %merge_348

merge_348:                                        ; preds = %then_347, %merge_346
  ret i64 0

then_333:                                         ; preds = %entry
  %"%754.load" = load i64, ptr %val2, align 8
  store i64 %"%754.load", ptr %"%fu_dev_0", align 8
  br label %merge_334

then_335:                                         ; preds = %merge_334
  %"%758.load" = load i64, ptr %val2, align 8
  store i64 %"%758.load", ptr %"%fu_dev_1", align 8
  br label %merge_336

then_337:                                         ; preds = %merge_336
  %"%762.load" = load i64, ptr %val2, align 8
  store i64 %"%762.load", ptr %"%fu_dev_2", align 8
  br label %merge_338

then_339:                                         ; preds = %merge_338
  %"%766.load" = load i64, ptr %val2, align 8
  store i64 %"%766.load", ptr %"%fu_dev_3", align 8
  br label %merge_340

then_341:                                         ; preds = %merge_340
  %"%770.load" = load i64, ptr %val2, align 8
  store i64 %"%770.load", ptr %"%fu_dev_4", align 8
  br label %merge_342

then_343:                                         ; preds = %merge_342
  %"%774.load" = load i64, ptr %val2, align 8
  store i64 %"%774.load", ptr %"%fu_dev_5", align 8
  br label %merge_344

then_345:                                         ; preds = %merge_344
  %"%778.load" = load i64, ptr %val2, align 8
  store i64 %"%778.load", ptr %"%fu_dev_6", align 8
  br label %merge_346

then_347:                                         ; preds = %merge_346
  %"%782.load" = load i64, ptr %val2, align 8
  store i64 %"%782.load", ptr %"%fu_dev_7", align 8
  br label %merge_348
}

define i64 @uac_get_volume(i64 %f) {
entry:
  %f1 = alloca i64, align 8
  store i64 %f, ptr %f1, align 8
  %"%f.load" = load i64, ptr %f1, align 8
  %cmpeqtmp = icmp eq i64 %"%f.load", 0
  br i1 %cmpeqtmp, label %then_349, label %merge_350

merge_350:                                        ; preds = %entry
  %"%f.load2" = load i64, ptr %f1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%f.load2", 1
  br i1 %cmpeqtmp3, label %then_351, label %merge_352

merge_352:                                        ; preds = %merge_350
  %"%f.load5" = load i64, ptr %f1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%f.load5", 2
  br i1 %cmpeqtmp6, label %then_353, label %merge_354

merge_354:                                        ; preds = %merge_352
  %"%f.load8" = load i64, ptr %f1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%f.load8", 3
  br i1 %cmpeqtmp9, label %then_355, label %merge_356

merge_356:                                        ; preds = %merge_354
  %"%f.load11" = load i64, ptr %f1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%f.load11", 4
  br i1 %cmpeqtmp12, label %then_357, label %merge_358

merge_358:                                        ; preds = %merge_356
  %"%f.load14" = load i64, ptr %f1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%f.load14", 5
  br i1 %cmpeqtmp15, label %then_359, label %merge_360

merge_360:                                        ; preds = %merge_358
  %"%f.load17" = load i64, ptr %f1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%f.load17", 6
  br i1 %cmpeqtmp18, label %then_361, label %merge_362

merge_362:                                        ; preds = %merge_360
  %"%f.load20" = load i64, ptr %f1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%f.load20", 7
  br i1 %cmpeqtmp21, label %then_363, label %merge_364

merge_364:                                        ; preds = %merge_362
  ret i64 0

then_349:                                         ; preds = %entry
  %loadtmp = load i64, ptr @fu_vol_0, align 8
  ret i64 %loadtmp

then_351:                                         ; preds = %merge_350
  %loadtmp4 = load i64, ptr @fu_vol_1, align 8
  ret i64 %loadtmp4

then_353:                                         ; preds = %merge_352
  %loadtmp7 = load i64, ptr @fu_vol_2, align 8
  ret i64 %loadtmp7

then_355:                                         ; preds = %merge_354
  %loadtmp10 = load i64, ptr @fu_vol_3, align 8
  ret i64 %loadtmp10

then_357:                                         ; preds = %merge_356
  %loadtmp13 = load i64, ptr @fu_vol_4, align 8
  ret i64 %loadtmp13

then_359:                                         ; preds = %merge_358
  %loadtmp16 = load i64, ptr @fu_vol_5, align 8
  ret i64 %loadtmp16

then_361:                                         ; preds = %merge_360
  %loadtmp19 = load i64, ptr @fu_vol_6, align 8
  ret i64 %loadtmp19

then_363:                                         ; preds = %merge_362
  %loadtmp22 = load i64, ptr @fu_vol_7, align 8
  ret i64 %loadtmp22
}

define i64 @uac_set_volume(i64 %f, i64 %val) {
entry:
  %"%fu_vol_7" = alloca i64, align 8
  %"%fu_vol_6" = alloca i64, align 8
  %"%fu_vol_5" = alloca i64, align 8
  %"%fu_vol_4" = alloca i64, align 8
  %"%fu_vol_3" = alloca i64, align 8
  %"%fu_vol_2" = alloca i64, align 8
  %"%fu_vol_1" = alloca i64, align 8
  %"%fu_vol_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %f1 = alloca i64, align 8
  store i64 %f, ptr %f1, align 8
  %"%val.load" = load i64, ptr %val2, align 8
  %cmplttmp = icmp slt i64 %"%val.load", 0
  br i1 %cmplttmp, label %then_365, label %merge_366

merge_366:                                        ; preds = %then_365, %entry
  %"%val.load3" = load i64, ptr %val2, align 8
  %cmpgttmp = icmp sgt i64 %"%val.load3", 255
  br i1 %cmpgttmp, label %then_367, label %merge_368

merge_368:                                        ; preds = %then_367, %merge_366
  %"%f.load" = load i64, ptr %f1, align 8
  %cmpeqtmp = icmp eq i64 %"%f.load", 0
  br i1 %cmpeqtmp, label %then_369, label %merge_370

merge_370:                                        ; preds = %then_369, %merge_368
  %"%f.load4" = load i64, ptr %f1, align 8
  %cmpeqtmp5 = icmp eq i64 %"%f.load4", 1
  br i1 %cmpeqtmp5, label %then_371, label %merge_372

merge_372:                                        ; preds = %then_371, %merge_370
  %"%f.load6" = load i64, ptr %f1, align 8
  %cmpeqtmp7 = icmp eq i64 %"%f.load6", 2
  br i1 %cmpeqtmp7, label %then_373, label %merge_374

merge_374:                                        ; preds = %then_373, %merge_372
  %"%f.load8" = load i64, ptr %f1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%f.load8", 3
  br i1 %cmpeqtmp9, label %then_375, label %merge_376

merge_376:                                        ; preds = %then_375, %merge_374
  %"%f.load10" = load i64, ptr %f1, align 8
  %cmpeqtmp11 = icmp eq i64 %"%f.load10", 4
  br i1 %cmpeqtmp11, label %then_377, label %merge_378

merge_378:                                        ; preds = %then_377, %merge_376
  %"%f.load12" = load i64, ptr %f1, align 8
  %cmpeqtmp13 = icmp eq i64 %"%f.load12", 5
  br i1 %cmpeqtmp13, label %then_379, label %merge_380

merge_380:                                        ; preds = %then_379, %merge_378
  %"%f.load14" = load i64, ptr %f1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%f.load14", 6
  br i1 %cmpeqtmp15, label %then_381, label %merge_382

merge_382:                                        ; preds = %then_381, %merge_380
  %"%f.load16" = load i64, ptr %f1, align 8
  %cmpeqtmp17 = icmp eq i64 %"%f.load16", 7
  br i1 %cmpeqtmp17, label %then_383, label %merge_384

merge_384:                                        ; preds = %then_383, %merge_382
  ret i64 0

then_365:                                         ; preds = %entry
  br label %merge_366

then_367:                                         ; preds = %merge_366
  br label %merge_368

then_369:                                         ; preds = %merge_368
  %"%832.load" = load i64, ptr %val2, align 8
  store i64 %"%832.load", ptr %"%fu_vol_0", align 8
  br label %merge_370

then_371:                                         ; preds = %merge_370
  %"%836.load" = load i64, ptr %val2, align 8
  store i64 %"%836.load", ptr %"%fu_vol_1", align 8
  br label %merge_372

then_373:                                         ; preds = %merge_372
  %"%840.load" = load i64, ptr %val2, align 8
  store i64 %"%840.load", ptr %"%fu_vol_2", align 8
  br label %merge_374

then_375:                                         ; preds = %merge_374
  %"%844.load" = load i64, ptr %val2, align 8
  store i64 %"%844.load", ptr %"%fu_vol_3", align 8
  br label %merge_376

then_377:                                         ; preds = %merge_376
  %"%848.load" = load i64, ptr %val2, align 8
  store i64 %"%848.load", ptr %"%fu_vol_4", align 8
  br label %merge_378

then_379:                                         ; preds = %merge_378
  %"%852.load" = load i64, ptr %val2, align 8
  store i64 %"%852.load", ptr %"%fu_vol_5", align 8
  br label %merge_380

then_381:                                         ; preds = %merge_380
  %"%856.load" = load i64, ptr %val2, align 8
  store i64 %"%856.load", ptr %"%fu_vol_6", align 8
  br label %merge_382

then_383:                                         ; preds = %merge_382
  %"%860.load" = load i64, ptr %val2, align 8
  store i64 %"%860.load", ptr %"%fu_vol_7", align 8
  br label %merge_384
}

define i64 @uac_get_mute(i64 %f) {
entry:
  %f1 = alloca i64, align 8
  store i64 %f, ptr %f1, align 8
  %"%f.load" = load i64, ptr %f1, align 8
  %cmpeqtmp = icmp eq i64 %"%f.load", 0
  br i1 %cmpeqtmp, label %then_385, label %merge_386

merge_386:                                        ; preds = %entry
  %"%f.load2" = load i64, ptr %f1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%f.load2", 1
  br i1 %cmpeqtmp3, label %then_387, label %merge_388

merge_388:                                        ; preds = %merge_386
  %"%f.load5" = load i64, ptr %f1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%f.load5", 2
  br i1 %cmpeqtmp6, label %then_389, label %merge_390

merge_390:                                        ; preds = %merge_388
  %"%f.load8" = load i64, ptr %f1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%f.load8", 3
  br i1 %cmpeqtmp9, label %then_391, label %merge_392

merge_392:                                        ; preds = %merge_390
  %"%f.load11" = load i64, ptr %f1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%f.load11", 4
  br i1 %cmpeqtmp12, label %then_393, label %merge_394

merge_394:                                        ; preds = %merge_392
  %"%f.load14" = load i64, ptr %f1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%f.load14", 5
  br i1 %cmpeqtmp15, label %then_395, label %merge_396

merge_396:                                        ; preds = %merge_394
  %"%f.load17" = load i64, ptr %f1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%f.load17", 6
  br i1 %cmpeqtmp18, label %then_397, label %merge_398

merge_398:                                        ; preds = %merge_396
  %"%f.load20" = load i64, ptr %f1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%f.load20", 7
  br i1 %cmpeqtmp21, label %then_399, label %merge_400

merge_400:                                        ; preds = %merge_398
  ret i64 0

then_385:                                         ; preds = %entry
  %loadtmp = load i64, ptr @fu_mute_0, align 8
  ret i64 %loadtmp

then_387:                                         ; preds = %merge_386
  %loadtmp4 = load i64, ptr @fu_mute_1, align 8
  ret i64 %loadtmp4

then_389:                                         ; preds = %merge_388
  %loadtmp7 = load i64, ptr @fu_mute_2, align 8
  ret i64 %loadtmp7

then_391:                                         ; preds = %merge_390
  %loadtmp10 = load i64, ptr @fu_mute_3, align 8
  ret i64 %loadtmp10

then_393:                                         ; preds = %merge_392
  %loadtmp13 = load i64, ptr @fu_mute_4, align 8
  ret i64 %loadtmp13

then_395:                                         ; preds = %merge_394
  %loadtmp16 = load i64, ptr @fu_mute_5, align 8
  ret i64 %loadtmp16

then_397:                                         ; preds = %merge_396
  %loadtmp19 = load i64, ptr @fu_mute_6, align 8
  ret i64 %loadtmp19

then_399:                                         ; preds = %merge_398
  %loadtmp22 = load i64, ptr @fu_mute_7, align 8
  ret i64 %loadtmp22
}

define i64 @uac_set_mute(i64 %f, i64 %val) {
entry:
  %"%fu_mute_7" = alloca i64, align 8
  %"%fu_mute_6" = alloca i64, align 8
  %"%fu_mute_5" = alloca i64, align 8
  %"%fu_mute_4" = alloca i64, align 8
  %"%fu_mute_3" = alloca i64, align 8
  %"%fu_mute_2" = alloca i64, align 8
  %"%fu_mute_1" = alloca i64, align 8
  %"%fu_mute_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %f1 = alloca i64, align 8
  store i64 %f, ptr %f1, align 8
  %"%f.load" = load i64, ptr %f1, align 8
  %cmpeqtmp = icmp eq i64 %"%f.load", 0
  br i1 %cmpeqtmp, label %then_401, label %merge_402

merge_402:                                        ; preds = %then_401, %entry
  %"%f.load3" = load i64, ptr %f1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%f.load3", 1
  br i1 %cmpeqtmp4, label %then_403, label %merge_404

merge_404:                                        ; preds = %then_403, %merge_402
  %"%f.load5" = load i64, ptr %f1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%f.load5", 2
  br i1 %cmpeqtmp6, label %then_405, label %merge_406

merge_406:                                        ; preds = %then_405, %merge_404
  %"%f.load7" = load i64, ptr %f1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%f.load7", 3
  br i1 %cmpeqtmp8, label %then_407, label %merge_408

merge_408:                                        ; preds = %then_407, %merge_406
  %"%f.load9" = load i64, ptr %f1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%f.load9", 4
  br i1 %cmpeqtmp10, label %then_409, label %merge_410

merge_410:                                        ; preds = %then_409, %merge_408
  %"%f.load11" = load i64, ptr %f1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%f.load11", 5
  br i1 %cmpeqtmp12, label %then_411, label %merge_412

merge_412:                                        ; preds = %then_411, %merge_410
  %"%f.load13" = load i64, ptr %f1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%f.load13", 6
  br i1 %cmpeqtmp14, label %then_413, label %merge_414

merge_414:                                        ; preds = %then_413, %merge_412
  %"%f.load15" = load i64, ptr %f1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%f.load15", 7
  br i1 %cmpeqtmp16, label %then_415, label %merge_416

merge_416:                                        ; preds = %then_415, %merge_414
  ret i64 0

then_401:                                         ; preds = %entry
  %"%906.load" = load i64, ptr %val2, align 8
  store i64 %"%906.load", ptr %"%fu_mute_0", align 8
  br label %merge_402

then_403:                                         ; preds = %merge_402
  %"%910.load" = load i64, ptr %val2, align 8
  store i64 %"%910.load", ptr %"%fu_mute_1", align 8
  br label %merge_404

then_405:                                         ; preds = %merge_404
  %"%914.load" = load i64, ptr %val2, align 8
  store i64 %"%914.load", ptr %"%fu_mute_2", align 8
  br label %merge_406

then_407:                                         ; preds = %merge_406
  %"%918.load" = load i64, ptr %val2, align 8
  store i64 %"%918.load", ptr %"%fu_mute_3", align 8
  br label %merge_408

then_409:                                         ; preds = %merge_408
  %"%922.load" = load i64, ptr %val2, align 8
  store i64 %"%922.load", ptr %"%fu_mute_4", align 8
  br label %merge_410

then_411:                                         ; preds = %merge_410
  %"%926.load" = load i64, ptr %val2, align 8
  store i64 %"%926.load", ptr %"%fu_mute_5", align 8
  br label %merge_412

then_413:                                         ; preds = %merge_412
  %"%930.load" = load i64, ptr %val2, align 8
  store i64 %"%930.load", ptr %"%fu_mute_6", align 8
  br label %merge_414

then_415:                                         ; preds = %merge_414
  %"%934.load" = load i64, ptr %val2, align 8
  store i64 %"%934.load", ptr %"%fu_mute_7", align 8
  br label %merge_416
}

define i64 @uac_add_terminal(i64 %dev_id, i64 %tid, i64 %ttype, i64 %direction, i64 %channels) {
entry:
  %"%uac_term_count" = alloca i64, align 8
  %"%idx" = alloca i64, align 8
  %channels5 = alloca i64, align 8
  store i64 %channels, ptr %channels5, align 8
  %direction4 = alloca i64, align 8
  store i64 %direction, ptr %direction4, align 8
  %ttype3 = alloca i64, align 8
  store i64 %ttype, ptr %ttype3, align 8
  %tid2 = alloca i64, align 8
  store i64 %tid, ptr %tid2, align 8
  %dev_id1 = alloca i64, align 8
  store i64 %dev_id, ptr %dev_id1, align 8
  %"%937.load" = load i64, ptr @uac_term_count, align 8
  store i64 %"%937.load", ptr %"%idx", align 8
  %"%idx.load" = load i64, ptr %"%idx", align 8
  %cmpgetmp = icmp sge i64 %"%idx.load", 16
  br i1 %cmpgetmp, label %then_417, label %merge_418

else_421:                                         ; preds = %merge_418
  %calltmp21 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  br label %merge_420

merge_418:                                        ; preds = %entry
  %"%idx.load6" = load i64, ptr %"%idx", align 8
  %"%tid.load" = load i64, ptr %tid2, align 8
  %calltmp = call i64 @uac_set_term_id(i64 %"%idx.load6", i64 %"%tid.load")
  %"%idx.load7" = load i64, ptr %"%idx", align 8
  %"%ttype.load" = load i64, ptr %ttype3, align 8
  %calltmp8 = call i64 @uac_set_term_type(i64 %"%idx.load7", i64 %"%ttype.load")
  %"%idx.load9" = load i64, ptr %"%idx", align 8
  %"%direction.load" = load i64, ptr %direction4, align 8
  %calltmp10 = call i64 @uac_set_term_dir(i64 %"%idx.load9", i64 %"%direction.load")
  %"%idx.load11" = load i64, ptr %"%idx", align 8
  %"%channels.load" = load i64, ptr %channels5, align 8
  %calltmp12 = call i64 @uac_set_term_ch(i64 %"%idx.load11", i64 %"%channels.load")
  %"%idx.load13" = load i64, ptr %"%idx", align 8
  %"%dev_id.load" = load i64, ptr %dev_id1, align 8
  %calltmp14 = call i64 @uac_set_term_dev(i64 %"%idx.load13", i64 %"%dev_id.load")
  %calltmp15 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %"%952.load" = load i64, ptr %tid2, align 8
  %calltmp16 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"%952.load")
  %calltmp17 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %"%957.load" = load i64, ptr %ttype3, align 8
  %calltmp18 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"%957.load")
  %"%direction.load19" = load i64, ptr %direction4, align 8
  %cmpeqtmp = icmp eq i64 %"%direction.load19", 0
  br i1 %cmpeqtmp, label %then_419, label %else_421

merge_420:                                        ; preds = %else_421, %then_419
  %calltmp22 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%968.load" = load i64, ptr %channels5, align 8
  %calltmp23 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"%968.load")
  %calltmp24 = call i64 @"uac_serial_\D8\B3\D8\B7\D8\B1"()
  %"%uac_term_count.load" = load i64, ptr @uac_term_count, align 8
  %addtmp = add i64 %"%uac_term_count.load", 1
  store i64 %addtmp, ptr %"%uac_term_count", align 8
  %loadtmp = load i64, ptr %"%idx", align 8
  ret i64 %loadtmp

then_417:                                         ; preds = %entry
  ret i64 -1

then_419:                                         ; preds = %merge_418
  %calltmp20 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  br label %merge_420
}

define i64 @uac_add_feature_unit(i64 %dev_id, i64 %fid, i64 %source, i64 %controls) {
entry:
  %"%uac_fu_count" = alloca i64, align 8
  %"%idx" = alloca i64, align 8
  %controls4 = alloca i64, align 8
  store i64 %controls, ptr %controls4, align 8
  %source3 = alloca i64, align 8
  store i64 %source, ptr %source3, align 8
  %fid2 = alloca i64, align 8
  store i64 %fid, ptr %fid2, align 8
  %dev_id1 = alloca i64, align 8
  store i64 %dev_id, ptr %dev_id1, align 8
  %"%977.load" = load i64, ptr @uac_fu_count, align 8
  store i64 %"%977.load", ptr %"%idx", align 8
  %"%idx.load" = load i64, ptr %"%idx", align 8
  %cmpgetmp = icmp sge i64 %"%idx.load", 8
  br i1 %cmpgetmp, label %then_422, label %merge_423

merge_423:                                        ; preds = %entry
  %"%idx.load5" = load i64, ptr %"%idx", align 8
  %"%fid.load" = load i64, ptr %fid2, align 8
  %calltmp = call i64 @uac_set_fu_id(i64 %"%idx.load5", i64 %"%fid.load")
  %"%idx.load6" = load i64, ptr %"%idx", align 8
  %"%source.load" = load i64, ptr %source3, align 8
  %calltmp7 = call i64 @uac_set_fu_src(i64 %"%idx.load6", i64 %"%source.load")
  %"%idx.load8" = load i64, ptr %"%idx", align 8
  %"%controls.load" = load i64, ptr %controls4, align 8
  %calltmp9 = call i64 @uac_set_fu_ctrl(i64 %"%idx.load8", i64 %"%controls.load")
  %"%idx.load10" = load i64, ptr %"%idx", align 8
  %"%dev_id.load" = load i64, ptr %dev_id1, align 8
  %calltmp11 = call i64 @uac_set_fu_dev(i64 %"%idx.load10", i64 %"%dev_id.load")
  %calltmp12 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %"%991.load" = load i64, ptr %fid2, align 8
  %calltmp13 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"%991.load")
  %calltmp14 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  %"%996.load" = load i64, ptr %source3, align 8
  %calltmp15 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"%996.load")
  %calltmp16 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %"%1001.load" = load i64, ptr %controls4, align 8
  %calltmp17 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"%1001.load")
  %calltmp18 = call i64 @"uac_serial_\D8\B3\D8\B7\D8\B1"()
  %"%uac_fu_count.load" = load i64, ptr @uac_fu_count, align 8
  %addtmp = add i64 %"%uac_fu_count.load", 1
  store i64 %addtmp, ptr %"%uac_fu_count", align 8
  %loadtmp = load i64, ptr %"%idx", align 8
  ret i64 %loadtmp

then_422:                                         ; preds = %entry
  ret i64 -1
}

define i64 @uac_register_device(i64 %usb_addr, i64 %ctrl_iface, i64 %out_iface, i64 %in_iface, i64 %out_ep, i64 %in_ep) {
entry:
  %"%uac_device_count" = alloca i64, align 8
  %"%uac_ep_in_3" = alloca i64, align 8
  %"%uac_ep_out_3" = alloca i64, align 8
  %"%uac_stream_in_iface_3" = alloca i64, align 8
  %"%uac_stream_out_iface_3" = alloca i64, align 8
  %"%uac_ctrl_iface_3" = alloca i64, align 8
  %"%uac_ep_in_2" = alloca i64, align 8
  %"%uac_ep_out_2" = alloca i64, align 8
  %"%uac_stream_in_iface_2" = alloca i64, align 8
  %"%uac_stream_out_iface_2" = alloca i64, align 8
  %"%uac_ctrl_iface_2" = alloca i64, align 8
  %"%uac_ep_in_1" = alloca i64, align 8
  %"%uac_ep_out_1" = alloca i64, align 8
  %"%uac_stream_in_iface_1" = alloca i64, align 8
  %"%uac_stream_out_iface_1" = alloca i64, align 8
  %"%uac_ctrl_iface_1" = alloca i64, align 8
  %"%uac_ep_in_0" = alloca i64, align 8
  %"%uac_ep_out_0" = alloca i64, align 8
  %"%uac_stream_in_iface_0" = alloca i64, align 8
  %"%uac_stream_out_iface_0" = alloca i64, align 8
  %"%uac_ctrl_iface_0" = alloca i64, align 8
  %"%idx" = alloca i64, align 8
  %in_ep6 = alloca i64, align 8
  store i64 %in_ep, ptr %in_ep6, align 8
  %out_ep5 = alloca i64, align 8
  store i64 %out_ep, ptr %out_ep5, align 8
  %in_iface4 = alloca i64, align 8
  store i64 %in_iface, ptr %in_iface4, align 8
  %out_iface3 = alloca i64, align 8
  store i64 %out_iface, ptr %out_iface3, align 8
  %ctrl_iface2 = alloca i64, align 8
  store i64 %ctrl_iface, ptr %ctrl_iface2, align 8
  %usb_addr1 = alloca i64, align 8
  store i64 %usb_addr, ptr %usb_addr1, align 8
  %"%1010.load" = load i64, ptr @uac_device_count, align 8
  store i64 %"%1010.load", ptr %"%idx", align 8
  %"%idx.load" = load i64, ptr %"%idx", align 8
  %cmpgetmp = icmp sge i64 %"%idx.load", 4
  br i1 %cmpgetmp, label %then_424, label %merge_425

merge_425:                                        ; preds = %entry
  %"%idx.load7" = load i64, ptr %"%idx", align 8
  %calltmp = call i64 @uac_set_present(i64 %"%idx.load7", i64 1)
  %"%idx.load8" = load i64, ptr %"%idx", align 8
  %"%usb_addr.load" = load i64, ptr %usb_addr1, align 8
  %calltmp9 = call i64 @uac_set_addr(i64 %"%idx.load8", i64 %"%usb_addr.load")
  %"%idx.load10" = load i64, ptr %"%idx", align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load10", 0
  br i1 %cmpeqtmp, label %then_426, label %merge_427

merge_427:                                        ; preds = %then_426, %merge_425
  %"%idx.load11" = load i64, ptr %"%idx", align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 1
  br i1 %cmpeqtmp12, label %then_428, label %merge_429

merge_429:                                        ; preds = %then_428, %merge_427
  %"%idx.load13" = load i64, ptr %"%idx", align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 2
  br i1 %cmpeqtmp14, label %then_430, label %merge_431

merge_431:                                        ; preds = %then_430, %merge_429
  %"%idx.load15" = load i64, ptr %"%idx", align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 3
  br i1 %cmpeqtmp16, label %then_432, label %merge_433

merge_433:                                        ; preds = %then_432, %merge_431
  %calltmp17 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %"%1071.load" = load i64, ptr %"%idx", align 8
  %calltmp18 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"%1071.load")
  %calltmp19 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  %"%1076.load" = load i64, ptr %usb_addr1, align 8
  %calltmp20 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"%1076.load")
  %calltmp21 = call i64 @"uac_serial_\D8\B3\D8\B7\D8\B1"()
  %"%uac_device_count.load" = load i64, ptr @uac_device_count, align 8
  %addtmp = add i64 %"%uac_device_count.load", 1
  store i64 %addtmp, ptr %"%uac_device_count", align 8
  %loadtmp = load i64, ptr %"%idx", align 8
  ret i64 %loadtmp

then_424:                                         ; preds = %entry
  ret i64 -1

then_426:                                         ; preds = %merge_425
  %"%1023.load" = load i64, ptr %ctrl_iface2, align 8
  store i64 %"%1023.load", ptr %"%uac_ctrl_iface_0", align 8
  %"%1025.load" = load i64, ptr %out_iface3, align 8
  store i64 %"%1025.load", ptr %"%uac_stream_out_iface_0", align 8
  %"%1027.load" = load i64, ptr %in_iface4, align 8
  store i64 %"%1027.load", ptr %"%uac_stream_in_iface_0", align 8
  %"%1029.load" = load i64, ptr %out_ep5, align 8
  store i64 %"%1029.load", ptr %"%uac_ep_out_0", align 8
  %"%1031.load" = load i64, ptr %in_ep6, align 8
  store i64 %"%1031.load", ptr %"%uac_ep_in_0", align 8
  br label %merge_427

then_428:                                         ; preds = %merge_427
  %"%1035.load" = load i64, ptr %ctrl_iface2, align 8
  store i64 %"%1035.load", ptr %"%uac_ctrl_iface_1", align 8
  %"%1037.load" = load i64, ptr %out_iface3, align 8
  store i64 %"%1037.load", ptr %"%uac_stream_out_iface_1", align 8
  %"%1039.load" = load i64, ptr %in_iface4, align 8
  store i64 %"%1039.load", ptr %"%uac_stream_in_iface_1", align 8
  %"%1041.load" = load i64, ptr %out_ep5, align 8
  store i64 %"%1041.load", ptr %"%uac_ep_out_1", align 8
  %"%1043.load" = load i64, ptr %in_ep6, align 8
  store i64 %"%1043.load", ptr %"%uac_ep_in_1", align 8
  br label %merge_429

then_430:                                         ; preds = %merge_429
  %"%1047.load" = load i64, ptr %ctrl_iface2, align 8
  store i64 %"%1047.load", ptr %"%uac_ctrl_iface_2", align 8
  %"%1049.load" = load i64, ptr %out_iface3, align 8
  store i64 %"%1049.load", ptr %"%uac_stream_out_iface_2", align 8
  %"%1051.load" = load i64, ptr %in_iface4, align 8
  store i64 %"%1051.load", ptr %"%uac_stream_in_iface_2", align 8
  %"%1053.load" = load i64, ptr %out_ep5, align 8
  store i64 %"%1053.load", ptr %"%uac_ep_out_2", align 8
  %"%1055.load" = load i64, ptr %in_ep6, align 8
  store i64 %"%1055.load", ptr %"%uac_ep_in_2", align 8
  br label %merge_431

then_432:                                         ; preds = %merge_431
  %"%1059.load" = load i64, ptr %ctrl_iface2, align 8
  store i64 %"%1059.load", ptr %"%uac_ctrl_iface_3", align 8
  %"%1061.load" = load i64, ptr %out_iface3, align 8
  store i64 %"%1061.load", ptr %"%uac_stream_out_iface_3", align 8
  %"%1063.load" = load i64, ptr %in_iface4, align 8
  store i64 %"%1063.load", ptr %"%uac_stream_in_iface_3", align 8
  %"%1065.load" = load i64, ptr %out_ep5, align 8
  store i64 %"%1065.load", ptr %"%uac_ep_out_3", align 8
  %"%1067.load" = load i64, ptr %in_ep6, align 8
  store i64 %"%1067.load", ptr %"%uac_ep_in_3", align 8
  br label %merge_433
}

define i64 @uac_start_stream(i64 %dev) {
entry:
  %dev1 = alloca i64, align 8
  store i64 %dev, ptr %dev1, align 8
  %"%dev.load" = load i64, ptr %dev1, align 8
  %"%uac_device_count.load" = load i64, ptr @uac_device_count, align 8
  %cmpgetmp = icmp sge i64 %"%dev.load", %"%uac_device_count.load"
  br i1 %cmpgetmp, label %then_434, label %merge_435

merge_435:                                        ; preds = %entry
  %"%dev.load2" = load i64, ptr %dev1, align 8
  %calltmp = call i64 @uac_get_present(i64 %"%dev.load2")
  %cmpeqtmp = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp, label %then_436, label %merge_437

merge_437:                                        ; preds = %merge_435
  %"%dev.load3" = load i64, ptr %dev1, align 8
  %calltmp4 = call i64 @uac_set_streaming(i64 %"%dev.load3", i64 1)
  %calltmp5 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %"%1101.load" = load i64, ptr %dev1, align 8
  %calltmp6 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"%1101.load")
  %calltmp7 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %"%dev.load8" = load i64, ptr %dev1, align 8
  %calltmp9 = call i64 @uac_get_rate(i64 %"%dev.load8")
  %calltmp10 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp9)
  %calltmp11 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %"%dev.load12" = load i64, ptr %dev1, align 8
  %calltmp13 = call i64 @uac_get_bits(i64 %"%dev.load12")
  %calltmp14 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp13)
  %calltmp15 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%dev.load16" = load i64, ptr %dev1, align 8
  %calltmp17 = call i64 @uac_get_channels(i64 %"%dev.load16")
  %calltmp18 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp17)
  %calltmp19 = call i64 @"uac_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_434:                                         ; preds = %entry
  ret i64 -1

then_436:                                         ; preds = %merge_435
  ret i64 -2
}

define i64 @uac_stop_stream(i64 %dev) {
entry:
  %dev1 = alloca i64, align 8
  store i64 %dev, ptr %dev1, align 8
  %"%dev.load" = load i64, ptr %dev1, align 8
  %"%uac_device_count.load" = load i64, ptr @uac_device_count, align 8
  %cmpgetmp = icmp sge i64 %"%dev.load", %"%uac_device_count.load"
  br i1 %cmpgetmp, label %then_438, label %merge_439

merge_439:                                        ; preds = %entry
  %"%dev.load2" = load i64, ptr %dev1, align 8
  %calltmp = call i64 @uac_set_streaming(i64 %"%dev.load2", i64 0)
  %calltmp3 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  %"%1133.load" = load i64, ptr %dev1, align 8
  %calltmp4 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"%1133.load")
  %calltmp5 = call i64 @"uac_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_438:                                         ; preds = %entry
  ret i64 -1
}

define i64 @uac_calc_frame_size(i64 %dev) {
entry:
  %"%bytes_per_sample" = alloca i64, align 8
  %"%bits" = alloca i64, align 8
  %"%ch" = alloca i64, align 8
  %dev1 = alloca i64, align 8
  store i64 %dev, ptr %dev1, align 8
  %"%dev.load" = load i64, ptr %dev1, align 8
  %calltmp = call i64 @uac_get_channels(i64 %"%dev.load")
  store i64 %calltmp, ptr %"%ch", align 8
  %"%dev.load2" = load i64, ptr %dev1, align 8
  %calltmp3 = call i64 @uac_get_bits(i64 %"%dev.load2")
  store i64 %calltmp3, ptr %"%bits", align 8
  %"%bits.load" = load i64, ptr %"%bits", align 8
  %shrtmp = lshr i64 %"%bits.load", 3
  store i64 %shrtmp, ptr %"%bytes_per_sample", align 8
  %"%ch.load" = load i64, ptr %"%ch", align 8
  %"%bytes_per_sample.load" = load i64, ptr %"%bytes_per_sample", align 8
  %multmp = mul i64 %"%ch.load", %"%bytes_per_sample.load"
  ret i64 %multmp
}

define i64 @uac_calc_ms_buffer(i64 %dev) {
entry:
  %"%samples_per_ms" = alloca i64, align 8
  %"%frame" = alloca i64, align 8
  %"%rate" = alloca i64, align 8
  %dev1 = alloca i64, align 8
  store i64 %dev, ptr %dev1, align 8
  %"%dev.load" = load i64, ptr %dev1, align 8
  %calltmp = call i64 @uac_get_rate(i64 %"%dev.load")
  store i64 %calltmp, ptr %"%rate", align 8
  %"%dev.load2" = load i64, ptr %dev1, align 8
  %calltmp3 = call i64 @uac_calc_frame_size(i64 %"%dev.load2")
  store i64 %calltmp3, ptr %"%frame", align 8
  %"%rate.load" = load i64, ptr %"%rate", align 8
  %divtmp = sdiv i64 %"%rate.load", 1000
  store i64 %divtmp, ptr %"%samples_per_ms", align 8
  %"%samples_per_ms.load" = load i64, ptr %"%samples_per_ms", align 8
  %"%frame.load" = load i64, ptr %"%frame", align 8
  %multmp = mul i64 %"%samples_per_ms.load", %"%frame.load"
  ret i64 %multmp
}

define i64 @"uac_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %"%d" = alloca i64, align 8
  %calltmp = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %calltmp1 = call i64 @"uac_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  %"%1163.load" = load i64, ptr @uac_device_count, align 8
  %calltmp3 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"%1163.load")
  %calltmp4 = call i64 @"uac_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp5 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %"%1169.load" = load i64, ptr @uac_term_count, align 8
  %calltmp6 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"%1169.load")
  %calltmp7 = call i64 @"uac_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp8 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %"%1175.load" = load i64, ptr @uac_fu_count, align 8
  %calltmp9 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"%1175.load")
  %calltmp10 = call i64 @"uac_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%d", align 8
  br label %while_cond_440

merge_444:                                        ; preds = %merge_446, %while_body_441
  %"%d.load40" = load i64, ptr %"%d", align 8
  %addtmp = add i64 %"%d.load40", 1
  store i64 %addtmp, ptr %"%d", align 8
  br label %while_cond_440

merge_446:                                        ; preds = %then_445, %then_443
  %calltmp42 = call i64 @"uac_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_444

then_443:                                         ; preds = %while_body_441
  %calltmp22 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  %"%1188.load" = load i64, ptr %"%d", align 8
  %calltmp23 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"%1188.load")
  %calltmp24 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %"%d.load25" = load i64, ptr %"%d", align 8
  %calltmp26 = call i64 @uac_get_rate(i64 %"%d.load25")
  %calltmp27 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp26)
  %calltmp28 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  %"%d.load29" = load i64, ptr %"%d", align 8
  %calltmp30 = call i64 @uac_get_bits(i64 %"%d.load29")
  %calltmp31 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp30)
  %calltmp32 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  %"%d.load33" = load i64, ptr %"%d", align 8
  %calltmp34 = call i64 @uac_get_channels(i64 %"%d.load33")
  %calltmp35 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp34)
  %calltmp36 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  %"%d.load37" = load i64, ptr %"%d", align 8
  %calltmp38 = call i64 @uac_get_streaming(i64 %"%d.load37")
  %cmpeqtmp39 = icmp eq i64 %calltmp38, 1
  br i1 %cmpeqtmp39, label %then_445, label %merge_446

then_445:                                         ; preds = %then_443
  %calltmp41 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  br label %merge_446

while_body_441:                                   ; preds = %while_cond_440
  %"%d.load11" = load i64, ptr %"%d", align 8
  %calltmp12 = call i64 @uac_get_present(i64 %"%d.load11")
  %cmpeqtmp = icmp eq i64 %calltmp12, 1
  br i1 %cmpeqtmp, label %then_443, label %merge_444

while_cond_440:                                   ; preds = %merge_444, %entry
  %"%d.load" = load i64, ptr %"%d", align 8
  %"%uac_device_count.load" = load i64, ptr @uac_device_count, align 8
  %cmplttmp = icmp slt i64 %"%d.load", %"%uac_device_count.load"
  br i1 %cmplttmp, label %while_body_441, label %while_exit_442

while_exit_442:                                   ; preds = %while_cond_440
  %calltmp13 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  %"%1221.load" = load i64, ptr @uac_packets_sent, align 8
  %calltmp14 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"%1221.load")
  %calltmp15 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  %"%1226.load" = load i64, ptr @uac_packets_received, align 8
  %calltmp16 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"%1226.load")
  %calltmp17 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  %"%1231.load" = load i64, ptr @uac_errors, align 8
  %calltmp18 = call i64 @"uac_serial_\D8\B1\D9\82\D9\85"(i64 %"%1231.load")
  %calltmp19 = call i64 @"uac_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp20 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  %calltmp21 = call i64 @"uac_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @"uac_\D9\87\D9\8A\D8\A6"() {
entry:
  %calltmp = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  %calltmp1 = call i64 @"uac_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"uac_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.30 to i64))
  %calltmp3 = call i64 @"uac_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @"uac_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  ret i64 0
}

define void @__sad_main() {
entry:
  %"%uac_errors" = alloca i64, align 8
  %"%uac_packets_received" = alloca i64, align 8
  %"%uac_packets_sent" = alloca i64, align 8
  %"%uac_device_count" = alloca i64, align 8
  %"%uac_fu_count" = alloca i64, align 8
  %"%fu_mute_7" = alloca i64, align 8
  %"%fu_mute_6" = alloca i64, align 8
  %"%fu_mute_5" = alloca i64, align 8
  %"%fu_mute_4" = alloca i64, align 8
  %"%fu_mute_3" = alloca i64, align 8
  %"%fu_mute_2" = alloca i64, align 8
  %"%fu_mute_1" = alloca i64, align 8
  %"%fu_mute_0" = alloca i64, align 8
  %"%fu_vol_7" = alloca i64, align 8
  %"%fu_vol_6" = alloca i64, align 8
  %"%fu_vol_5" = alloca i64, align 8
  %"%fu_vol_4" = alloca i64, align 8
  %"%fu_vol_3" = alloca i64, align 8
  %"%fu_vol_2" = alloca i64, align 8
  %"%fu_vol_1" = alloca i64, align 8
  %"%fu_vol_0" = alloca i64, align 8
  %"%fu_dev_7" = alloca i64, align 8
  %"%fu_dev_6" = alloca i64, align 8
  %"%fu_dev_5" = alloca i64, align 8
  %"%fu_dev_4" = alloca i64, align 8
  %"%fu_dev_3" = alloca i64, align 8
  %"%fu_dev_2" = alloca i64, align 8
  %"%fu_dev_1" = alloca i64, align 8
  %"%fu_dev_0" = alloca i64, align 8
  %"%fu_ctrl_7" = alloca i64, align 8
  %"%fu_ctrl_6" = alloca i64, align 8
  %"%fu_ctrl_5" = alloca i64, align 8
  %"%fu_ctrl_4" = alloca i64, align 8
  %"%fu_ctrl_3" = alloca i64, align 8
  %"%fu_ctrl_2" = alloca i64, align 8
  %"%fu_ctrl_1" = alloca i64, align 8
  %"%fu_ctrl_0" = alloca i64, align 8
  %"%fu_src_7" = alloca i64, align 8
  %"%fu_src_6" = alloca i64, align 8
  %"%fu_src_5" = alloca i64, align 8
  %"%fu_src_4" = alloca i64, align 8
  %"%fu_src_3" = alloca i64, align 8
  %"%fu_src_2" = alloca i64, align 8
  %"%fu_src_1" = alloca i64, align 8
  %"%fu_src_0" = alloca i64, align 8
  %"%fu_id_7" = alloca i64, align 8
  %"%fu_id_6" = alloca i64, align 8
  %"%fu_id_5" = alloca i64, align 8
  %"%fu_id_4" = alloca i64, align 8
  %"%fu_id_3" = alloca i64, align 8
  %"%fu_id_2" = alloca i64, align 8
  %"%fu_id_1" = alloca i64, align 8
  %"%fu_id_0" = alloca i64, align 8
  %"%uac_term_count" = alloca i64, align 8
  %"%term_dev_15" = alloca i64, align 8
  %"%term_dev_14" = alloca i64, align 8
  %"%term_dev_13" = alloca i64, align 8
  %"%term_dev_12" = alloca i64, align 8
  %"%term_dev_11" = alloca i64, align 8
  %"%term_dev_10" = alloca i64, align 8
  %"%term_dev_9" = alloca i64, align 8
  %"%term_dev_8" = alloca i64, align 8
  %"%term_dev_7" = alloca i64, align 8
  %"%term_dev_6" = alloca i64, align 8
  %"%term_dev_5" = alloca i64, align 8
  %"%term_dev_4" = alloca i64, align 8
  %"%term_dev_3" = alloca i64, align 8
  %"%term_dev_2" = alloca i64, align 8
  %"%term_dev_1" = alloca i64, align 8
  %"%term_dev_0" = alloca i64, align 8
  %"%term_ch_15" = alloca i64, align 8
  %"%term_ch_14" = alloca i64, align 8
  %"%term_ch_13" = alloca i64, align 8
  %"%term_ch_12" = alloca i64, align 8
  %"%term_ch_11" = alloca i64, align 8
  %"%term_ch_10" = alloca i64, align 8
  %"%term_ch_9" = alloca i64, align 8
  %"%term_ch_8" = alloca i64, align 8
  %"%term_ch_7" = alloca i64, align 8
  %"%term_ch_6" = alloca i64, align 8
  %"%term_ch_5" = alloca i64, align 8
  %"%term_ch_4" = alloca i64, align 8
  %"%term_ch_3" = alloca i64, align 8
  %"%term_ch_2" = alloca i64, align 8
  %"%term_ch_1" = alloca i64, align 8
  %"%term_ch_0" = alloca i64, align 8
  %"%term_dir_15" = alloca i64, align 8
  %"%term_dir_14" = alloca i64, align 8
  %"%term_dir_13" = alloca i64, align 8
  %"%term_dir_12" = alloca i64, align 8
  %"%term_dir_11" = alloca i64, align 8
  %"%term_dir_10" = alloca i64, align 8
  %"%term_dir_9" = alloca i64, align 8
  %"%term_dir_8" = alloca i64, align 8
  %"%term_dir_7" = alloca i64, align 8
  %"%term_dir_6" = alloca i64, align 8
  %"%term_dir_5" = alloca i64, align 8
  %"%term_dir_4" = alloca i64, align 8
  %"%term_dir_3" = alloca i64, align 8
  %"%term_dir_2" = alloca i64, align 8
  %"%term_dir_1" = alloca i64, align 8
  %"%term_dir_0" = alloca i64, align 8
  %"%term_type_15" = alloca i64, align 8
  %"%term_type_14" = alloca i64, align 8
  %"%term_type_13" = alloca i64, align 8
  %"%term_type_12" = alloca i64, align 8
  %"%term_type_11" = alloca i64, align 8
  %"%term_type_10" = alloca i64, align 8
  %"%term_type_9" = alloca i64, align 8
  %"%term_type_8" = alloca i64, align 8
  %"%term_type_7" = alloca i64, align 8
  %"%term_type_6" = alloca i64, align 8
  %"%term_type_5" = alloca i64, align 8
  %"%term_type_4" = alloca i64, align 8
  %"%term_type_3" = alloca i64, align 8
  %"%term_type_2" = alloca i64, align 8
  %"%term_type_1" = alloca i64, align 8
  %"%term_type_0" = alloca i64, align 8
  %"%term_id_15" = alloca i64, align 8
  %"%term_id_14" = alloca i64, align 8
  %"%term_id_13" = alloca i64, align 8
  %"%term_id_12" = alloca i64, align 8
  %"%term_id_11" = alloca i64, align 8
  %"%term_id_10" = alloca i64, align 8
  %"%term_id_9" = alloca i64, align 8
  %"%term_id_8" = alloca i64, align 8
  %"%term_id_7" = alloca i64, align 8
  %"%term_id_6" = alloca i64, align 8
  %"%term_id_5" = alloca i64, align 8
  %"%term_id_4" = alloca i64, align 8
  %"%term_id_3" = alloca i64, align 8
  %"%term_id_2" = alloca i64, align 8
  %"%term_id_1" = alloca i64, align 8
  %"%term_id_0" = alloca i64, align 8
  %"%uac_streaming_3" = alloca i64, align 8
  %"%uac_streaming_2" = alloca i64, align 8
  %"%uac_streaming_1" = alloca i64, align 8
  %"%uac_streaming_0" = alloca i64, align 8
  %"%uac_format_3" = alloca i64, align 8
  %"%uac_format_2" = alloca i64, align 8
  %"%uac_format_1" = alloca i64, align 8
  %"%uac_format_0" = alloca i64, align 8
  %"%uac_rate_3" = alloca i64, align 8
  %"%uac_rate_2" = alloca i64, align 8
  %"%uac_rate_1" = alloca i64, align 8
  %"%uac_rate_0" = alloca i64, align 8
  %"%uac_bits_3" = alloca i64, align 8
  %"%uac_bits_2" = alloca i64, align 8
  %"%uac_bits_1" = alloca i64, align 8
  %"%uac_bits_0" = alloca i64, align 8
  %"%uac_channels_3" = alloca i64, align 8
  %"%uac_channels_2" = alloca i64, align 8
  %"%uac_channels_1" = alloca i64, align 8
  %"%uac_channels_0" = alloca i64, align 8
  %"%uac_version_3" = alloca i64, align 8
  %"%uac_version_2" = alloca i64, align 8
  %"%uac_version_1" = alloca i64, align 8
  %"%uac_version_0" = alloca i64, align 8
  %"%uac_maxpkt_in_3" = alloca i64, align 8
  %"%uac_maxpkt_in_2" = alloca i64, align 8
  %"%uac_maxpkt_in_1" = alloca i64, align 8
  %"%uac_maxpkt_in_0" = alloca i64, align 8
  %"%uac_maxpkt_out_3" = alloca i64, align 8
  %"%uac_maxpkt_out_2" = alloca i64, align 8
  %"%uac_maxpkt_out_1" = alloca i64, align 8
  %"%uac_maxpkt_out_0" = alloca i64, align 8
  %"%uac_ep_in_3" = alloca i64, align 8
  %"%uac_ep_in_2" = alloca i64, align 8
  %"%uac_ep_in_1" = alloca i64, align 8
  %"%uac_ep_in_0" = alloca i64, align 8
  %"%uac_ep_out_3" = alloca i64, align 8
  %"%uac_ep_out_2" = alloca i64, align 8
  %"%uac_ep_out_1" = alloca i64, align 8
  %"%uac_ep_out_0" = alloca i64, align 8
  %"%uac_stream_in_iface_3" = alloca i64, align 8
  %"%uac_stream_in_iface_2" = alloca i64, align 8
  %"%uac_stream_in_iface_1" = alloca i64, align 8
  %"%uac_stream_in_iface_0" = alloca i64, align 8
  %"%uac_stream_out_iface_3" = alloca i64, align 8
  %"%uac_stream_out_iface_2" = alloca i64, align 8
  %"%uac_stream_out_iface_1" = alloca i64, align 8
  %"%uac_stream_out_iface_0" = alloca i64, align 8
  %"%uac_ctrl_iface_3" = alloca i64, align 8
  %"%uac_ctrl_iface_2" = alloca i64, align 8
  %"%uac_ctrl_iface_1" = alloca i64, align 8
  %"%uac_ctrl_iface_0" = alloca i64, align 8
  %"%uac_addr_3" = alloca i64, align 8
  %"%uac_addr_2" = alloca i64, align 8
  %"%uac_addr_1" = alloca i64, align 8
  %"%uac_addr_0" = alloca i64, align 8
  %"%uac_present_3" = alloca i64, align 8
  %"%uac_present_2" = alloca i64, align 8
  %"%uac_present_1" = alloca i64, align 8
  %"%uac_present_0" = alloca i64, align 8
  %"%UAC_MAX_FEATURES" = alloca i64, align 8
  %"%UAC_MAX_TERMINALS" = alloca i64, align 8
  %"%UAC_MAX_DEVICES" = alloca i64, align 8
  %"%UAC_FORMAT_MULAW" = alloca i64, align 8
  %"%UAC_FORMAT_ALAW" = alloca i64, align 8
  %"%UAC_FORMAT_IEEE_FLOAT" = alloca i64, align 8
  %"%UAC_FORMAT_PCM8" = alloca i64, align 8
  %"%UAC_FORMAT_PCM" = alloca i64, align 8
  %"%UAC_RATE_96000" = alloca i64, align 8
  %"%UAC_RATE_48000" = alloca i64, align 8
  %"%UAC_RATE_44100" = alloca i64, align 8
  %"%UAC_RATE_32000" = alloca i64, align 8
  %"%UAC_RATE_22050" = alloca i64, align 8
  %"%UAC_RATE_16000" = alloca i64, align 8
  %"%UAC_RATE_11025" = alloca i64, align 8
  %"%UAC_RATE_8000" = alloca i64, align 8
  %"%UAC_GET_RES" = alloca i64, align 8
  %"%UAC_SET_RES" = alloca i64, align 8
  %"%UAC_GET_MAX" = alloca i64, align 8
  %"%UAC_SET_MAX" = alloca i64, align 8
  %"%UAC_GET_MIN" = alloca i64, align 8
  %"%UAC_SET_MIN" = alloca i64, align 8
  %"%UAC_GET_CUR" = alloca i64, align 8
  %"%UAC_SET_CUR" = alloca i64, align 8
  %"%UAC_FU_LOUDNESS" = alloca i64, align 8
  %"%UAC_FU_BASS_BOOST" = alloca i64, align 8
  %"%UAC_FU_DELAY" = alloca i64, align 8
  %"%UAC_FU_AGC" = alloca i64, align 8
  %"%UAC_FU_EQUALIZER" = alloca i64, align 8
  %"%UAC_FU_TREBLE" = alloca i64, align 8
  %"%UAC_FU_MID" = alloca i64, align 8
  %"%UAC_FU_BASS" = alloca i64, align 8
  %"%UAC_FU_VOLUME" = alloca i64, align 8
  %"%UAC_FU_MUTE" = alloca i64, align 8
  %"%UAC_TT_HDMI" = alloca i64, align 8
  %"%UAC_TT_SPDIF" = alloca i64, align 8
  %"%UAC_TT_LINE_CONNECTOR" = alloca i64, align 8
  %"%UAC_TT_DESKTOP_MIC" = alloca i64, align 8
  %"%UAC_TT_MICROPHONE" = alloca i64, align 8
  %"%UAC_TT_HEADPHONES" = alloca i64, align 8
  %"%UAC_TT_SPEAKER" = alloca i64, align 8
  %"%UAC_TT_USB_STREAMING" = alloca i64, align 8
  %"%UAC_AS_FORMAT_SPECIFIC" = alloca i64, align 8
  %"%UAC_AS_FORMAT_TYPE" = alloca i64, align 8
  %"%UAC_AS_GENERAL" = alloca i64, align 8
  %"%UAC_AC_EXTENSION_UNIT" = alloca i64, align 8
  %"%UAC_AC_PROCESSING_UNIT" = alloca i64, align 8
  %"%UAC_AC_FEATURE_UNIT" = alloca i64, align 8
  %"%UAC_AC_SELECTOR_UNIT" = alloca i64, align 8
  %"%UAC_AC_MIXER_UNIT" = alloca i64, align 8
  %"%UAC_AC_OUTPUT_TERMINAL" = alloca i64, align 8
  %"%UAC_AC_INPUT_TERMINAL" = alloca i64, align 8
  %"%UAC_AC_HEADER" = alloca i64, align 8
  %"%UAC_SUBCLASS_MIDI" = alloca i64, align 8
  %"%UAC_SUBCLASS_STREAM" = alloca i64, align 8
  %"%UAC_SUBCLASS_CONTROL" = alloca i64, align 8
  %"%UAC_CLASS" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 1, ptr %"%UAC_CLASS", align 8
  store i64 1, ptr %"%UAC_SUBCLASS_CONTROL", align 8
  store i64 2, ptr %"%UAC_SUBCLASS_STREAM", align 8
  store i64 3, ptr %"%UAC_SUBCLASS_MIDI", align 8
  store i64 1, ptr %"%UAC_AC_HEADER", align 8
  store i64 2, ptr %"%UAC_AC_INPUT_TERMINAL", align 8
  store i64 3, ptr %"%UAC_AC_OUTPUT_TERMINAL", align 8
  store i64 4, ptr %"%UAC_AC_MIXER_UNIT", align 8
  store i64 5, ptr %"%UAC_AC_SELECTOR_UNIT", align 8
  store i64 6, ptr %"%UAC_AC_FEATURE_UNIT", align 8
  store i64 7, ptr %"%UAC_AC_PROCESSING_UNIT", align 8
  store i64 8, ptr %"%UAC_AC_EXTENSION_UNIT", align 8
  store i64 1, ptr %"%UAC_AS_GENERAL", align 8
  store i64 2, ptr %"%UAC_AS_FORMAT_TYPE", align 8
  store i64 3, ptr %"%UAC_AS_FORMAT_SPECIFIC", align 8
  store i64 257, ptr %"%UAC_TT_USB_STREAMING", align 8
  store i64 769, ptr %"%UAC_TT_SPEAKER", align 8
  store i64 770, ptr %"%UAC_TT_HEADPHONES", align 8
  store i64 513, ptr %"%UAC_TT_MICROPHONE", align 8
  store i64 514, ptr %"%UAC_TT_DESKTOP_MIC", align 8
  store i64 1538, ptr %"%UAC_TT_LINE_CONNECTOR", align 8
  store i64 1539, ptr %"%UAC_TT_SPDIF", align 8
  store i64 1541, ptr %"%UAC_TT_HDMI", align 8
  store i64 1, ptr %"%UAC_FU_MUTE", align 8
  store i64 2, ptr %"%UAC_FU_VOLUME", align 8
  store i64 3, ptr %"%UAC_FU_BASS", align 8
  store i64 4, ptr %"%UAC_FU_MID", align 8
  store i64 5, ptr %"%UAC_FU_TREBLE", align 8
  store i64 6, ptr %"%UAC_FU_EQUALIZER", align 8
  store i64 7, ptr %"%UAC_FU_AGC", align 8
  store i64 8, ptr %"%UAC_FU_DELAY", align 8
  store i64 9, ptr %"%UAC_FU_BASS_BOOST", align 8
  store i64 10, ptr %"%UAC_FU_LOUDNESS", align 8
  store i64 1, ptr %"%UAC_SET_CUR", align 8
  store i64 129, ptr %"%UAC_GET_CUR", align 8
  store i64 2, ptr %"%UAC_SET_MIN", align 8
  store i64 130, ptr %"%UAC_GET_MIN", align 8
  store i64 3, ptr %"%UAC_SET_MAX", align 8
  store i64 131, ptr %"%UAC_GET_MAX", align 8
  store i64 4, ptr %"%UAC_SET_RES", align 8
  store i64 132, ptr %"%UAC_GET_RES", align 8
  store i64 8000, ptr %"%UAC_RATE_8000", align 8
  store i64 11025, ptr %"%UAC_RATE_11025", align 8
  store i64 16000, ptr %"%UAC_RATE_16000", align 8
  store i64 22050, ptr %"%UAC_RATE_22050", align 8
  store i64 32000, ptr %"%UAC_RATE_32000", align 8
  store i64 44100, ptr %"%UAC_RATE_44100", align 8
  store i64 48000, ptr %"%UAC_RATE_48000", align 8
  store i64 96000, ptr %"%UAC_RATE_96000", align 8
  store i64 1, ptr %"%UAC_FORMAT_PCM", align 8
  store i64 2, ptr %"%UAC_FORMAT_PCM8", align 8
  store i64 3, ptr %"%UAC_FORMAT_IEEE_FLOAT", align 8
  store i64 4, ptr %"%UAC_FORMAT_ALAW", align 8
  store i64 5, ptr %"%UAC_FORMAT_MULAW", align 8
  store i64 4, ptr %"%UAC_MAX_DEVICES", align 8
  store i64 16, ptr %"%UAC_MAX_TERMINALS", align 8
  store i64 8, ptr %"%UAC_MAX_FEATURES", align 8
  store i64 0, ptr %"%uac_present_0", align 8
  store i64 0, ptr %"%uac_present_1", align 8
  store i64 0, ptr %"%uac_present_2", align 8
  store i64 0, ptr %"%uac_present_3", align 8
  store i64 0, ptr %"%uac_addr_0", align 8
  store i64 0, ptr %"%uac_addr_1", align 8
  store i64 0, ptr %"%uac_addr_2", align 8
  store i64 0, ptr %"%uac_addr_3", align 8
  store i64 0, ptr %"%uac_ctrl_iface_0", align 8
  store i64 0, ptr %"%uac_ctrl_iface_1", align 8
  store i64 0, ptr %"%uac_ctrl_iface_2", align 8
  store i64 0, ptr %"%uac_ctrl_iface_3", align 8
  store i64 0, ptr %"%uac_stream_out_iface_0", align 8
  store i64 0, ptr %"%uac_stream_out_iface_1", align 8
  store i64 0, ptr %"%uac_stream_out_iface_2", align 8
  store i64 0, ptr %"%uac_stream_out_iface_3", align 8
  store i64 0, ptr %"%uac_stream_in_iface_0", align 8
  store i64 0, ptr %"%uac_stream_in_iface_1", align 8
  store i64 0, ptr %"%uac_stream_in_iface_2", align 8
  store i64 0, ptr %"%uac_stream_in_iface_3", align 8
  store i64 0, ptr %"%uac_ep_out_0", align 8
  store i64 0, ptr %"%uac_ep_out_1", align 8
  store i64 0, ptr %"%uac_ep_out_2", align 8
  store i64 0, ptr %"%uac_ep_out_3", align 8
  store i64 0, ptr %"%uac_ep_in_0", align 8
  store i64 0, ptr %"%uac_ep_in_1", align 8
  store i64 0, ptr %"%uac_ep_in_2", align 8
  store i64 0, ptr %"%uac_ep_in_3", align 8
  store i64 0, ptr %"%uac_maxpkt_out_0", align 8
  store i64 0, ptr %"%uac_maxpkt_out_1", align 8
  store i64 0, ptr %"%uac_maxpkt_out_2", align 8
  store i64 0, ptr %"%uac_maxpkt_out_3", align 8
  store i64 0, ptr %"%uac_maxpkt_in_0", align 8
  store i64 0, ptr %"%uac_maxpkt_in_1", align 8
  store i64 0, ptr %"%uac_maxpkt_in_2", align 8
  store i64 0, ptr %"%uac_maxpkt_in_3", align 8
  store i64 0, ptr %"%uac_version_0", align 8
  store i64 0, ptr %"%uac_version_1", align 8
  store i64 0, ptr %"%uac_version_2", align 8
  store i64 0, ptr %"%uac_version_3", align 8
  store i64 2, ptr %"%uac_channels_0", align 8
  store i64 2, ptr %"%uac_channels_1", align 8
  store i64 2, ptr %"%uac_channels_2", align 8
  store i64 2, ptr %"%uac_channels_3", align 8
  store i64 16, ptr %"%uac_bits_0", align 8
  store i64 16, ptr %"%uac_bits_1", align 8
  store i64 16, ptr %"%uac_bits_2", align 8
  store i64 16, ptr %"%uac_bits_3", align 8
  store i64 44100, ptr %"%uac_rate_0", align 8
  store i64 44100, ptr %"%uac_rate_1", align 8
  store i64 44100, ptr %"%uac_rate_2", align 8
  store i64 44100, ptr %"%uac_rate_3", align 8
  store i64 1, ptr %"%uac_format_0", align 8
  store i64 1, ptr %"%uac_format_1", align 8
  store i64 1, ptr %"%uac_format_2", align 8
  store i64 1, ptr %"%uac_format_3", align 8
  store i64 0, ptr %"%uac_streaming_0", align 8
  store i64 0, ptr %"%uac_streaming_1", align 8
  store i64 0, ptr %"%uac_streaming_2", align 8
  store i64 0, ptr %"%uac_streaming_3", align 8
  store i64 0, ptr %"%term_id_0", align 8
  store i64 0, ptr %"%term_id_1", align 8
  store i64 0, ptr %"%term_id_2", align 8
  store i64 0, ptr %"%term_id_3", align 8
  store i64 0, ptr %"%term_id_4", align 8
  store i64 0, ptr %"%term_id_5", align 8
  store i64 0, ptr %"%term_id_6", align 8
  store i64 0, ptr %"%term_id_7", align 8
  store i64 0, ptr %"%term_id_8", align 8
  store i64 0, ptr %"%term_id_9", align 8
  store i64 0, ptr %"%term_id_10", align 8
  store i64 0, ptr %"%term_id_11", align 8
  store i64 0, ptr %"%term_id_12", align 8
  store i64 0, ptr %"%term_id_13", align 8
  store i64 0, ptr %"%term_id_14", align 8
  store i64 0, ptr %"%term_id_15", align 8
  store i64 0, ptr %"%term_type_0", align 8
  store i64 0, ptr %"%term_type_1", align 8
  store i64 0, ptr %"%term_type_2", align 8
  store i64 0, ptr %"%term_type_3", align 8
  store i64 0, ptr %"%term_type_4", align 8
  store i64 0, ptr %"%term_type_5", align 8
  store i64 0, ptr %"%term_type_6", align 8
  store i64 0, ptr %"%term_type_7", align 8
  store i64 0, ptr %"%term_type_8", align 8
  store i64 0, ptr %"%term_type_9", align 8
  store i64 0, ptr %"%term_type_10", align 8
  store i64 0, ptr %"%term_type_11", align 8
  store i64 0, ptr %"%term_type_12", align 8
  store i64 0, ptr %"%term_type_13", align 8
  store i64 0, ptr %"%term_type_14", align 8
  store i64 0, ptr %"%term_type_15", align 8
  store i64 0, ptr %"%term_dir_0", align 8
  store i64 0, ptr %"%term_dir_1", align 8
  store i64 0, ptr %"%term_dir_2", align 8
  store i64 0, ptr %"%term_dir_3", align 8
  store i64 0, ptr %"%term_dir_4", align 8
  store i64 0, ptr %"%term_dir_5", align 8
  store i64 0, ptr %"%term_dir_6", align 8
  store i64 0, ptr %"%term_dir_7", align 8
  store i64 0, ptr %"%term_dir_8", align 8
  store i64 0, ptr %"%term_dir_9", align 8
  store i64 0, ptr %"%term_dir_10", align 8
  store i64 0, ptr %"%term_dir_11", align 8
  store i64 0, ptr %"%term_dir_12", align 8
  store i64 0, ptr %"%term_dir_13", align 8
  store i64 0, ptr %"%term_dir_14", align 8
  store i64 0, ptr %"%term_dir_15", align 8
  store i64 0, ptr %"%term_ch_0", align 8
  store i64 0, ptr %"%term_ch_1", align 8
  store i64 0, ptr %"%term_ch_2", align 8
  store i64 0, ptr %"%term_ch_3", align 8
  store i64 0, ptr %"%term_ch_4", align 8
  store i64 0, ptr %"%term_ch_5", align 8
  store i64 0, ptr %"%term_ch_6", align 8
  store i64 0, ptr %"%term_ch_7", align 8
  store i64 0, ptr %"%term_ch_8", align 8
  store i64 0, ptr %"%term_ch_9", align 8
  store i64 0, ptr %"%term_ch_10", align 8
  store i64 0, ptr %"%term_ch_11", align 8
  store i64 0, ptr %"%term_ch_12", align 8
  store i64 0, ptr %"%term_ch_13", align 8
  store i64 0, ptr %"%term_ch_14", align 8
  store i64 0, ptr %"%term_ch_15", align 8
  store i64 0, ptr %"%term_dev_0", align 8
  store i64 0, ptr %"%term_dev_1", align 8
  store i64 0, ptr %"%term_dev_2", align 8
  store i64 0, ptr %"%term_dev_3", align 8
  store i64 0, ptr %"%term_dev_4", align 8
  store i64 0, ptr %"%term_dev_5", align 8
  store i64 0, ptr %"%term_dev_6", align 8
  store i64 0, ptr %"%term_dev_7", align 8
  store i64 0, ptr %"%term_dev_8", align 8
  store i64 0, ptr %"%term_dev_9", align 8
  store i64 0, ptr %"%term_dev_10", align 8
  store i64 0, ptr %"%term_dev_11", align 8
  store i64 0, ptr %"%term_dev_12", align 8
  store i64 0, ptr %"%term_dev_13", align 8
  store i64 0, ptr %"%term_dev_14", align 8
  store i64 0, ptr %"%term_dev_15", align 8
  store i64 0, ptr %"%uac_term_count", align 8
  store i64 0, ptr %"%fu_id_0", align 8
  store i64 0, ptr %"%fu_id_1", align 8
  store i64 0, ptr %"%fu_id_2", align 8
  store i64 0, ptr %"%fu_id_3", align 8
  store i64 0, ptr %"%fu_id_4", align 8
  store i64 0, ptr %"%fu_id_5", align 8
  store i64 0, ptr %"%fu_id_6", align 8
  store i64 0, ptr %"%fu_id_7", align 8
  store i64 0, ptr %"%fu_src_0", align 8
  store i64 0, ptr %"%fu_src_1", align 8
  store i64 0, ptr %"%fu_src_2", align 8
  store i64 0, ptr %"%fu_src_3", align 8
  store i64 0, ptr %"%fu_src_4", align 8
  store i64 0, ptr %"%fu_src_5", align 8
  store i64 0, ptr %"%fu_src_6", align 8
  store i64 0, ptr %"%fu_src_7", align 8
  store i64 0, ptr %"%fu_ctrl_0", align 8
  store i64 0, ptr %"%fu_ctrl_1", align 8
  store i64 0, ptr %"%fu_ctrl_2", align 8
  store i64 0, ptr %"%fu_ctrl_3", align 8
  store i64 0, ptr %"%fu_ctrl_4", align 8
  store i64 0, ptr %"%fu_ctrl_5", align 8
  store i64 0, ptr %"%fu_ctrl_6", align 8
  store i64 0, ptr %"%fu_ctrl_7", align 8
  store i64 0, ptr %"%fu_dev_0", align 8
  store i64 0, ptr %"%fu_dev_1", align 8
  store i64 0, ptr %"%fu_dev_2", align 8
  store i64 0, ptr %"%fu_dev_3", align 8
  store i64 0, ptr %"%fu_dev_4", align 8
  store i64 0, ptr %"%fu_dev_5", align 8
  store i64 0, ptr %"%fu_dev_6", align 8
  store i64 0, ptr %"%fu_dev_7", align 8
  store i64 128, ptr %"%fu_vol_0", align 8
  store i64 128, ptr %"%fu_vol_1", align 8
  store i64 128, ptr %"%fu_vol_2", align 8
  store i64 128, ptr %"%fu_vol_3", align 8
  store i64 128, ptr %"%fu_vol_4", align 8
  store i64 128, ptr %"%fu_vol_5", align 8
  store i64 128, ptr %"%fu_vol_6", align 8
  store i64 128, ptr %"%fu_vol_7", align 8
  store i64 0, ptr %"%fu_mute_0", align 8
  store i64 0, ptr %"%fu_mute_1", align 8
  store i64 0, ptr %"%fu_mute_2", align 8
  store i64 0, ptr %"%fu_mute_3", align 8
  store i64 0, ptr %"%fu_mute_4", align 8
  store i64 0, ptr %"%fu_mute_5", align 8
  store i64 0, ptr %"%fu_mute_6", align 8
  store i64 0, ptr %"%fu_mute_7", align 8
  store i64 0, ptr %"%uac_fu_count", align 8
  store i64 0, ptr %"%uac_device_count", align 8
  store i64 0, ptr %"%uac_packets_sent", align 8
  store i64 0, ptr %"%uac_packets_received", align 8
  store i64 0, ptr %"%uac_errors", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
