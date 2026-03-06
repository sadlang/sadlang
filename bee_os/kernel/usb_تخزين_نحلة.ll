; ModuleID = 'usb_تخزين_نحلة'
source_filename = "usb_\D8\AA\D8\AE\D8\B2\D9\8A\D9\86_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@COM1 = constant i64 1016
@MSC_MAX_DEVICES = constant i64 8
@CBW_SIGNATURE = constant i64 1128420181
@CBW_SIZE = constant i64 31
@CSW_SIGNATURE = constant i64 1396855637
@CSW_SIZE = constant i64 13
@CSW_STATUS_PASSED = constant i64 0
@CSW_STATUS_FAILED = constant i64 1
@CSW_STATUS_PHASE_ERROR = constant i64 2
@SCSI_TEST_UNIT_READY = constant i64 0
@SCSI_REQUEST_SENSE = constant i64 3
@SCSI_INQUIRY = constant i64 18
@SCSI_READ_CAPACITY = constant i64 37
@SCSI_READ_10 = constant i64 40
@SCSI_WRITE_10 = constant i64 42
@SCSI_MODE_SENSE_6 = constant i64 26
@SECTOR_SIZE = constant i64 512
@MAX_RETRIES = constant i64 3
@DIR_OUT = constant i64 0
@DIR_IN = constant i64 128
@BULK_EP_IN = constant i64 129
@BULK_EP_OUT = constant i64 2
@msc_active_0 = global i64 0
@msc_active_1 = global i64 0
@msc_active_2 = global i64 0
@msc_active_3 = global i64 0
@msc_active_4 = global i64 0
@msc_active_5 = global i64 0
@msc_active_6 = global i64 0
@msc_active_7 = global i64 0
@msc_hc_idx_0 = global i64 0
@msc_hc_idx_1 = global i64 0
@msc_hc_idx_2 = global i64 0
@msc_hc_idx_3 = global i64 0
@msc_hc_idx_4 = global i64 0
@msc_hc_idx_5 = global i64 0
@msc_hc_idx_6 = global i64 0
@msc_hc_idx_7 = global i64 0
@msc_dev_addr_0 = global i64 0
@msc_dev_addr_1 = global i64 0
@msc_dev_addr_2 = global i64 0
@msc_dev_addr_3 = global i64 0
@msc_dev_addr_4 = global i64 0
@msc_dev_addr_5 = global i64 0
@msc_dev_addr_6 = global i64 0
@msc_dev_addr_7 = global i64 0
@msc_ep_in_0 = global i64 129
@msc_ep_in_1 = global i64 129
@msc_ep_in_2 = global i64 129
@msc_ep_in_3 = global i64 129
@msc_ep_in_4 = global i64 129
@msc_ep_in_5 = global i64 129
@msc_ep_in_6 = global i64 129
@msc_ep_in_7 = global i64 129
@msc_ep_out_0 = global i64 2
@msc_ep_out_1 = global i64 2
@msc_ep_out_2 = global i64 2
@msc_ep_out_3 = global i64 2
@msc_ep_out_4 = global i64 2
@msc_ep_out_5 = global i64 2
@msc_ep_out_6 = global i64 2
@msc_ep_out_7 = global i64 2
@msc_sectors_0 = global i64 0
@msc_sectors_1 = global i64 0
@msc_sectors_2 = global i64 0
@msc_sectors_3 = global i64 0
@msc_sectors_4 = global i64 0
@msc_sectors_5 = global i64 0
@msc_sectors_6 = global i64 0
@msc_sectors_7 = global i64 0
@msc_sector_size_0 = global i64 512
@msc_sector_size_1 = global i64 512
@msc_sector_size_2 = global i64 512
@msc_sector_size_3 = global i64 512
@msc_sector_size_4 = global i64 512
@msc_sector_size_5 = global i64 512
@msc_sector_size_6 = global i64 512
@msc_sector_size_7 = global i64 512
@msc_lun_0 = global i64 0
@msc_lun_1 = global i64 0
@msc_lun_2 = global i64 0
@msc_lun_3 = global i64 0
@msc_lun_4 = global i64 0
@msc_lun_5 = global i64 0
@msc_lun_6 = global i64 0
@msc_lun_7 = global i64 0
@msc_type_0 = global i64 0
@msc_type_1 = global i64 0
@msc_type_2 = global i64 0
@msc_type_3 = global i64 0
@msc_type_4 = global i64 0
@msc_type_5 = global i64 0
@msc_type_6 = global i64 0
@msc_type_7 = global i64 0
@msc_count = global i64 0
@msc_tag_counter = global i64 1
@msc_cbw_buf = global i64 0
@msc_csw_buf = global i64 0
@msc_data_buf = global i64 0
@msc_reads = global i64 0
@msc_writes = global i64 0
@msc_errors = global i64 0
@part_type_0 = global i64 0
@part_type_1 = global i64 0
@part_type_2 = global i64 0
@part_type_3 = global i64 0
@part_start_0 = global i64 0
@part_start_1 = global i64 0
@part_start_2 = global i64 0
@part_start_3 = global i64 0
@part_size_0 = global i64 0
@part_size_1 = global i64 0
@part_size_2 = global i64 0
@part_size_3 = global i64 0
@part_boot_0 = global i64 0
@part_boot_1 = global i64 0
@part_boot_2 = global i64 0
@part_boot_3 = global i64 0
@part_count = global i64 0
@str.const = private unnamed_addr constant [24 x i8] c"[MSC] CSW tag mismatch!\00", align 1
@str.const.1 = private unnamed_addr constant [22 x i8] c"[MSC] CBW send failed\00", align 1
@str.const.2 = private unnamed_addr constant [21 x i8] c"[MSC] Data IN failed\00", align 1
@str.const.3 = private unnamed_addr constant [22 x i8] c"[MSC] Data OUT failed\00", align 1
@str.const.4 = private unnamed_addr constant [25 x i8] c"[MSC] CSW receive failed\00", align 1
@str.const.5 = private unnamed_addr constant [21 x i8] c"[MSC] Capacity: LBA=\00", align 1
@str.const.6 = private unnamed_addr constant [8 x i8] c" Block=\00", align 1
@str.const.7 = private unnamed_addr constant [25 x i8] c"[MSC] Failed to read MBR\00", align 1
@str.const.8 = private unnamed_addr constant [28 x i8] c"[MSC] Invalid MBR signature\00", align 1
@str.const.10 = private unnamed_addr constant [39 x i8] c"[MSC] MBR valid, reading partitions...\00", align 1
@str.const.11 = private unnamed_addr constant [25 x i8] c"[MSC] Partitions found: \00", align 1
@str.const.12 = private unnamed_addr constant [8 x i8] c"  Part \00", align 1
@str.const.13 = private unnamed_addr constant [10 x i8] c": type=0x\00", align 1
@str.const.14 = private unnamed_addr constant [8 x i8] c" start=\00", align 1
@str.const.15 = private unnamed_addr constant [7 x i8] c" size=\00", align 1
@str.const.16 = private unnamed_addr constant [8 x i8] c" [BOOT]\00", align 1
@str.const.17 = private unnamed_addr constant [26 x i8] c"[MSC] Max devices reached\00", align 1
@str.const.18 = private unnamed_addr constant [34 x i8] c"[MSC] Registered storage device #\00", align 1
@str.const.19 = private unnamed_addr constant [7 x i8] c" addr=\00", align 1
@str.const.20 = private unnamed_addr constant [37 x i8] c"[MSC] Device not ready after retries\00", align 1
@str.const.21 = private unnamed_addr constant [20 x i8] c"[MSC] Device type: \00", align 1
@str.const.22 = private unnamed_addr constant [52 x i8] c"\E2\95\90\E2\95\90\E2\95\90 [MSC] \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D8\AA\D8\AE\D8\B2\D9\8A\D9\86 \E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.23 = private unnamed_addr constant [26 x i8] c"  \D8\A3\D8\AC\D9\87\D8\B2\D8\A9 \D8\AA\D8\AE\D8\B2\D9\8A\D9\86: \00", align 1
@str.const.24 = private unnamed_addr constant [17 x i8] c"  \D9\82\D8\B1\D8\A7\D8\A1\D8\A7\D8\AA: \00", align 1
@str.const.25 = private unnamed_addr constant [16 x i8] c" \D9\83\D8\AA\D8\A7\D8\A8\D8\A7\D8\AA: \00", align 1
@str.const.26 = private unnamed_addr constant [14 x i8] c" \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1: \00", align 1
@str.const.27 = private unnamed_addr constant [4 x i8] c"  [\00", align 1
@str.const.28 = private unnamed_addr constant [8 x i8] c"] addr=\00", align 1
@str.const.29 = private unnamed_addr constant [7 x i8] c" type=\00", align 1
@str.const.30 = private unnamed_addr constant [10 x i8] c" sectors=\00", align 1
@str.const.31 = private unnamed_addr constant [11 x i8] c" sec_size=\00", align 1
@str.const.32 = private unnamed_addr constant [15 x i8] c"  \D8\A3\D9\82\D8\B3\D8\A7\D9\85: \00", align 1
@str.const.33 = private unnamed_addr constant [106 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.34 = private unnamed_addr constant [54 x i8] c"[MSC] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D8\AA\D8\B9\D8\B1\D9\8A\D9\81\D8\A7\D8\AA \D8\A7\D9\84\D8\AA\D8\AE\D8\B2\D9\8A\D9\86 USB...\00", align 1
@str.const.35 = private unnamed_addr constant [39 x i8] c"[MSC] Found storage device at USB idx \00", align 1
@str.const.36 = private unnamed_addr constant [30 x i8] c"[MSC] No storage device found\00", align 1
@str.const.37 = private unnamed_addr constant [32 x i8] c"[MSC] \D8\AC\D8\A7\D9\87\D8\B2 \E2\80\94 \D8\A3\D8\AC\D9\87\D8\B2\D8\A9: \00", align 1

declare void @sad_ll_mem_read8(i64)

declare void @sad_ll_mem_write8(i64, i64)

declare void @sad_ll_mem_read32(i64)

declare void @sad_ll_mem_write32(i64, i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_kmalloc(i64)

declare void @sad_ll_memset(i64, i64, i64)

declare void @sad_ll_memcpy(i64, i64, i64)

declare void @sad_ll_timer_get_ticks()

declare void @usb_find_storage()

declare void @uhci_bulk_transfer(i64, i64, i64, i64, i64)

declare void @uhci_control_transfer(i64, i64, i64, i64, i64)

define i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
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

define i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
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

define i64 @msc_serial_hex8(i64 %val) {
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

define i64 @msc_serial_hex32(i64 %val) {
entry:
  %val1 = alloca i64, align 8
  store i64 %val, ptr %val1, align 8
  %"%val.load" = load i64, ptr %val1, align 8
  %shrtmp = lshr i64 %"%val.load", 24
  %calltmp = call i64 @msc_serial_hex8(i64 %shrtmp)
  %"%val.load2" = load i64, ptr %val1, align 8
  %shrtmp3 = lshr i64 %"%val.load2", 16
  %andtmp = and i64 %shrtmp3, 255
  %calltmp4 = call i64 @msc_serial_hex8(i64 %andtmp)
  %"%val.load5" = load i64, ptr %val1, align 8
  %shrtmp6 = lshr i64 %"%val.load5", 8
  %andtmp7 = and i64 %shrtmp6, 255
  %calltmp8 = call i64 @msc_serial_hex8(i64 %andtmp7)
  %"%val.load9" = load i64, ptr %val1, align 8
  %andtmp10 = and i64 %"%val.load9", 255
  %calltmp11 = call i64 @msc_serial_hex8(i64 %andtmp10)
  ret i64 0
}

define i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"() {
entry:
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 13)
  %"%COM1.load1" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load1", i64 10)
  ret i64 0
}

define i64 @msc_get_active(i64 %idx) {
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
  %loadtmp = load i64, ptr @msc_active_0, align 8
  ret i64 %loadtmp

then_32:                                          ; preds = %merge_31
  %loadtmp4 = load i64, ptr @msc_active_1, align 8
  ret i64 %loadtmp4

then_34:                                          ; preds = %merge_33
  %loadtmp7 = load i64, ptr @msc_active_2, align 8
  ret i64 %loadtmp7

then_36:                                          ; preds = %merge_35
  %loadtmp10 = load i64, ptr @msc_active_3, align 8
  ret i64 %loadtmp10

then_38:                                          ; preds = %merge_37
  %loadtmp13 = load i64, ptr @msc_active_4, align 8
  ret i64 %loadtmp13

then_40:                                          ; preds = %merge_39
  %loadtmp16 = load i64, ptr @msc_active_5, align 8
  ret i64 %loadtmp16

then_42:                                          ; preds = %merge_41
  %loadtmp19 = load i64, ptr @msc_active_6, align 8
  ret i64 %loadtmp19

then_44:                                          ; preds = %merge_43
  %loadtmp22 = load i64, ptr @msc_active_7, align 8
  ret i64 %loadtmp22
}

define i64 @msc_set_active(i64 %idx, i64 %val) {
entry:
  %"%msc_active_7" = alloca i64, align 8
  %"%msc_active_6" = alloca i64, align 8
  %"%msc_active_5" = alloca i64, align 8
  %"%msc_active_4" = alloca i64, align 8
  %"%msc_active_3" = alloca i64, align 8
  %"%msc_active_2" = alloca i64, align 8
  %"%msc_active_1" = alloca i64, align 8
  %"%msc_active_0" = alloca i64, align 8
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
  %"%167.load" = load i64, ptr %val2, align 8
  store i64 %"%167.load", ptr %"%msc_active_0", align 8
  br label %merge_47

then_48:                                          ; preds = %merge_47
  %"%171.load" = load i64, ptr %val2, align 8
  store i64 %"%171.load", ptr %"%msc_active_1", align 8
  br label %merge_49

then_50:                                          ; preds = %merge_49
  %"%175.load" = load i64, ptr %val2, align 8
  store i64 %"%175.load", ptr %"%msc_active_2", align 8
  br label %merge_51

then_52:                                          ; preds = %merge_51
  %"%179.load" = load i64, ptr %val2, align 8
  store i64 %"%179.load", ptr %"%msc_active_3", align 8
  br label %merge_53

then_54:                                          ; preds = %merge_53
  %"%183.load" = load i64, ptr %val2, align 8
  store i64 %"%183.load", ptr %"%msc_active_4", align 8
  br label %merge_55

then_56:                                          ; preds = %merge_55
  %"%187.load" = load i64, ptr %val2, align 8
  store i64 %"%187.load", ptr %"%msc_active_5", align 8
  br label %merge_57

then_58:                                          ; preds = %merge_57
  %"%191.load" = load i64, ptr %val2, align 8
  store i64 %"%191.load", ptr %"%msc_active_6", align 8
  br label %merge_59

then_60:                                          ; preds = %merge_59
  %"%195.load" = load i64, ptr %val2, align 8
  store i64 %"%195.load", ptr %"%msc_active_7", align 8
  br label %merge_61
}

define i64 @msc_get_hc(i64 %idx) {
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
  %loadtmp = load i64, ptr @msc_hc_idx_0, align 8
  ret i64 %loadtmp

then_64:                                          ; preds = %merge_63
  %loadtmp4 = load i64, ptr @msc_hc_idx_1, align 8
  ret i64 %loadtmp4

then_66:                                          ; preds = %merge_65
  %loadtmp7 = load i64, ptr @msc_hc_idx_2, align 8
  ret i64 %loadtmp7

then_68:                                          ; preds = %merge_67
  %loadtmp10 = load i64, ptr @msc_hc_idx_3, align 8
  ret i64 %loadtmp10

then_70:                                          ; preds = %merge_69
  %loadtmp13 = load i64, ptr @msc_hc_idx_4, align 8
  ret i64 %loadtmp13

then_72:                                          ; preds = %merge_71
  %loadtmp16 = load i64, ptr @msc_hc_idx_5, align 8
  ret i64 %loadtmp16

then_74:                                          ; preds = %merge_73
  %loadtmp19 = load i64, ptr @msc_hc_idx_6, align 8
  ret i64 %loadtmp19

then_76:                                          ; preds = %merge_75
  %loadtmp22 = load i64, ptr @msc_hc_idx_7, align 8
  ret i64 %loadtmp22
}

define i64 @msc_set_hc(i64 %idx, i64 %val) {
entry:
  %"%msc_hc_idx_7" = alloca i64, align 8
  %"%msc_hc_idx_6" = alloca i64, align 8
  %"%msc_hc_idx_5" = alloca i64, align 8
  %"%msc_hc_idx_4" = alloca i64, align 8
  %"%msc_hc_idx_3" = alloca i64, align 8
  %"%msc_hc_idx_2" = alloca i64, align 8
  %"%msc_hc_idx_1" = alloca i64, align 8
  %"%msc_hc_idx_0" = alloca i64, align 8
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
  %"%241.load" = load i64, ptr %val2, align 8
  store i64 %"%241.load", ptr %"%msc_hc_idx_0", align 8
  br label %merge_79

then_80:                                          ; preds = %merge_79
  %"%245.load" = load i64, ptr %val2, align 8
  store i64 %"%245.load", ptr %"%msc_hc_idx_1", align 8
  br label %merge_81

then_82:                                          ; preds = %merge_81
  %"%249.load" = load i64, ptr %val2, align 8
  store i64 %"%249.load", ptr %"%msc_hc_idx_2", align 8
  br label %merge_83

then_84:                                          ; preds = %merge_83
  %"%253.load" = load i64, ptr %val2, align 8
  store i64 %"%253.load", ptr %"%msc_hc_idx_3", align 8
  br label %merge_85

then_86:                                          ; preds = %merge_85
  %"%257.load" = load i64, ptr %val2, align 8
  store i64 %"%257.load", ptr %"%msc_hc_idx_4", align 8
  br label %merge_87

then_88:                                          ; preds = %merge_87
  %"%261.load" = load i64, ptr %val2, align 8
  store i64 %"%261.load", ptr %"%msc_hc_idx_5", align 8
  br label %merge_89

then_90:                                          ; preds = %merge_89
  %"%265.load" = load i64, ptr %val2, align 8
  store i64 %"%265.load", ptr %"%msc_hc_idx_6", align 8
  br label %merge_91

then_92:                                          ; preds = %merge_91
  %"%269.load" = load i64, ptr %val2, align 8
  store i64 %"%269.load", ptr %"%msc_hc_idx_7", align 8
  br label %merge_93
}

define i64 @msc_get_addr(i64 %idx) {
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
  %loadtmp10 = load i64, ptr @msc_dev_addr_3, align 8
  ret i64 %loadtmp10

then_102:                                         ; preds = %merge_101
  %loadtmp13 = load i64, ptr @msc_dev_addr_4, align 8
  ret i64 %loadtmp13

then_104:                                         ; preds = %merge_103
  %loadtmp16 = load i64, ptr @msc_dev_addr_5, align 8
  ret i64 %loadtmp16

then_106:                                         ; preds = %merge_105
  %loadtmp19 = load i64, ptr @msc_dev_addr_6, align 8
  ret i64 %loadtmp19

then_108:                                         ; preds = %merge_107
  %loadtmp22 = load i64, ptr @msc_dev_addr_7, align 8
  ret i64 %loadtmp22

then_94:                                          ; preds = %entry
  %loadtmp = load i64, ptr @msc_dev_addr_0, align 8
  ret i64 %loadtmp

then_96:                                          ; preds = %merge_95
  %loadtmp4 = load i64, ptr @msc_dev_addr_1, align 8
  ret i64 %loadtmp4

then_98:                                          ; preds = %merge_97
  %loadtmp7 = load i64, ptr @msc_dev_addr_2, align 8
  ret i64 %loadtmp7
}

define i64 @msc_set_addr(i64 %idx, i64 %val) {
entry:
  %"%msc_dev_addr_7" = alloca i64, align 8
  %"%msc_dev_addr_6" = alloca i64, align 8
  %"%msc_dev_addr_5" = alloca i64, align 8
  %"%msc_dev_addr_4" = alloca i64, align 8
  %"%msc_dev_addr_3" = alloca i64, align 8
  %"%msc_dev_addr_2" = alloca i64, align 8
  %"%msc_dev_addr_1" = alloca i64, align 8
  %"%msc_dev_addr_0" = alloca i64, align 8
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
  %"%315.load" = load i64, ptr %val2, align 8
  store i64 %"%315.load", ptr %"%msc_dev_addr_0", align 8
  br label %merge_111

then_112:                                         ; preds = %merge_111
  %"%319.load" = load i64, ptr %val2, align 8
  store i64 %"%319.load", ptr %"%msc_dev_addr_1", align 8
  br label %merge_113

then_114:                                         ; preds = %merge_113
  %"%323.load" = load i64, ptr %val2, align 8
  store i64 %"%323.load", ptr %"%msc_dev_addr_2", align 8
  br label %merge_115

then_116:                                         ; preds = %merge_115
  %"%327.load" = load i64, ptr %val2, align 8
  store i64 %"%327.load", ptr %"%msc_dev_addr_3", align 8
  br label %merge_117

then_118:                                         ; preds = %merge_117
  %"%331.load" = load i64, ptr %val2, align 8
  store i64 %"%331.load", ptr %"%msc_dev_addr_4", align 8
  br label %merge_119

then_120:                                         ; preds = %merge_119
  %"%335.load" = load i64, ptr %val2, align 8
  store i64 %"%335.load", ptr %"%msc_dev_addr_5", align 8
  br label %merge_121

then_122:                                         ; preds = %merge_121
  %"%339.load" = load i64, ptr %val2, align 8
  store i64 %"%339.load", ptr %"%msc_dev_addr_6", align 8
  br label %merge_123

then_124:                                         ; preds = %merge_123
  %"%343.load" = load i64, ptr %val2, align 8
  store i64 %"%343.load", ptr %"%msc_dev_addr_7", align 8
  br label %merge_125
}

define i64 @msc_get_sectors(i64 %idx) {
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
  %loadtmp = load i64, ptr @msc_sectors_0, align 8
  ret i64 %loadtmp

then_128:                                         ; preds = %merge_127
  %loadtmp4 = load i64, ptr @msc_sectors_1, align 8
  ret i64 %loadtmp4

then_130:                                         ; preds = %merge_129
  %loadtmp7 = load i64, ptr @msc_sectors_2, align 8
  ret i64 %loadtmp7

then_132:                                         ; preds = %merge_131
  %loadtmp10 = load i64, ptr @msc_sectors_3, align 8
  ret i64 %loadtmp10

then_134:                                         ; preds = %merge_133
  %loadtmp13 = load i64, ptr @msc_sectors_4, align 8
  ret i64 %loadtmp13

then_136:                                         ; preds = %merge_135
  %loadtmp16 = load i64, ptr @msc_sectors_5, align 8
  ret i64 %loadtmp16

then_138:                                         ; preds = %merge_137
  %loadtmp19 = load i64, ptr @msc_sectors_6, align 8
  ret i64 %loadtmp19

then_140:                                         ; preds = %merge_139
  %loadtmp22 = load i64, ptr @msc_sectors_7, align 8
  ret i64 %loadtmp22
}

define i64 @msc_set_sectors(i64 %idx, i64 %val) {
entry:
  %"%msc_sectors_7" = alloca i64, align 8
  %"%msc_sectors_6" = alloca i64, align 8
  %"%msc_sectors_5" = alloca i64, align 8
  %"%msc_sectors_4" = alloca i64, align 8
  %"%msc_sectors_3" = alloca i64, align 8
  %"%msc_sectors_2" = alloca i64, align 8
  %"%msc_sectors_1" = alloca i64, align 8
  %"%msc_sectors_0" = alloca i64, align 8
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
  %"%389.load" = load i64, ptr %val2, align 8
  store i64 %"%389.load", ptr %"%msc_sectors_0", align 8
  br label %merge_143

then_144:                                         ; preds = %merge_143
  %"%393.load" = load i64, ptr %val2, align 8
  store i64 %"%393.load", ptr %"%msc_sectors_1", align 8
  br label %merge_145

then_146:                                         ; preds = %merge_145
  %"%397.load" = load i64, ptr %val2, align 8
  store i64 %"%397.load", ptr %"%msc_sectors_2", align 8
  br label %merge_147

then_148:                                         ; preds = %merge_147
  %"%401.load" = load i64, ptr %val2, align 8
  store i64 %"%401.load", ptr %"%msc_sectors_3", align 8
  br label %merge_149

then_150:                                         ; preds = %merge_149
  %"%405.load" = load i64, ptr %val2, align 8
  store i64 %"%405.load", ptr %"%msc_sectors_4", align 8
  br label %merge_151

then_152:                                         ; preds = %merge_151
  %"%409.load" = load i64, ptr %val2, align 8
  store i64 %"%409.load", ptr %"%msc_sectors_5", align 8
  br label %merge_153

then_154:                                         ; preds = %merge_153
  %"%413.load" = load i64, ptr %val2, align 8
  store i64 %"%413.load", ptr %"%msc_sectors_6", align 8
  br label %merge_155

then_156:                                         ; preds = %merge_155
  %"%417.load" = load i64, ptr %val2, align 8
  store i64 %"%417.load", ptr %"%msc_sectors_7", align 8
  br label %merge_157
}

define i64 @msc_get_sector_size(i64 %idx) {
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
  ret i64 512

then_158:                                         ; preds = %entry
  %loadtmp = load i64, ptr @msc_sector_size_0, align 8
  ret i64 %loadtmp

then_160:                                         ; preds = %merge_159
  %loadtmp4 = load i64, ptr @msc_sector_size_1, align 8
  ret i64 %loadtmp4

then_162:                                         ; preds = %merge_161
  %loadtmp7 = load i64, ptr @msc_sector_size_2, align 8
  ret i64 %loadtmp7

then_164:                                         ; preds = %merge_163
  %loadtmp10 = load i64, ptr @msc_sector_size_3, align 8
  ret i64 %loadtmp10

then_166:                                         ; preds = %merge_165
  %loadtmp13 = load i64, ptr @msc_sector_size_4, align 8
  ret i64 %loadtmp13

then_168:                                         ; preds = %merge_167
  %loadtmp16 = load i64, ptr @msc_sector_size_5, align 8
  ret i64 %loadtmp16

then_170:                                         ; preds = %merge_169
  %loadtmp19 = load i64, ptr @msc_sector_size_6, align 8
  ret i64 %loadtmp19

then_172:                                         ; preds = %merge_171
  %loadtmp22 = load i64, ptr @msc_sector_size_7, align 8
  ret i64 %loadtmp22
}

define i64 @msc_set_sector_size(i64 %idx, i64 %val) {
entry:
  %"%msc_sector_size_7" = alloca i64, align 8
  %"%msc_sector_size_6" = alloca i64, align 8
  %"%msc_sector_size_5" = alloca i64, align 8
  %"%msc_sector_size_4" = alloca i64, align 8
  %"%msc_sector_size_3" = alloca i64, align 8
  %"%msc_sector_size_2" = alloca i64, align 8
  %"%msc_sector_size_1" = alloca i64, align 8
  %"%msc_sector_size_0" = alloca i64, align 8
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
  %"%463.load" = load i64, ptr %val2, align 8
  store i64 %"%463.load", ptr %"%msc_sector_size_0", align 8
  br label %merge_175

then_176:                                         ; preds = %merge_175
  %"%467.load" = load i64, ptr %val2, align 8
  store i64 %"%467.load", ptr %"%msc_sector_size_1", align 8
  br label %merge_177

then_178:                                         ; preds = %merge_177
  %"%471.load" = load i64, ptr %val2, align 8
  store i64 %"%471.load", ptr %"%msc_sector_size_2", align 8
  br label %merge_179

then_180:                                         ; preds = %merge_179
  %"%475.load" = load i64, ptr %val2, align 8
  store i64 %"%475.load", ptr %"%msc_sector_size_3", align 8
  br label %merge_181

then_182:                                         ; preds = %merge_181
  %"%479.load" = load i64, ptr %val2, align 8
  store i64 %"%479.load", ptr %"%msc_sector_size_4", align 8
  br label %merge_183

then_184:                                         ; preds = %merge_183
  %"%483.load" = load i64, ptr %val2, align 8
  store i64 %"%483.load", ptr %"%msc_sector_size_5", align 8
  br label %merge_185

then_186:                                         ; preds = %merge_185
  %"%487.load" = load i64, ptr %val2, align 8
  store i64 %"%487.load", ptr %"%msc_sector_size_6", align 8
  br label %merge_187

then_188:                                         ; preds = %merge_187
  %"%491.load" = load i64, ptr %val2, align 8
  store i64 %"%491.load", ptr %"%msc_sector_size_7", align 8
  br label %merge_189
}

define i64 @msc_get_type(i64 %idx) {
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
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_198, label %merge_199

merge_199:                                        ; preds = %merge_197
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_200, label %merge_201

merge_201:                                        ; preds = %merge_199
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_202, label %merge_203

merge_203:                                        ; preds = %merge_201
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_204, label %merge_205

merge_205:                                        ; preds = %merge_203
  ret i64 0

then_190:                                         ; preds = %entry
  %loadtmp = load i64, ptr @msc_type_0, align 8
  ret i64 %loadtmp

then_192:                                         ; preds = %merge_191
  %loadtmp4 = load i64, ptr @msc_type_1, align 8
  ret i64 %loadtmp4

then_194:                                         ; preds = %merge_193
  %loadtmp7 = load i64, ptr @msc_type_2, align 8
  ret i64 %loadtmp7

then_196:                                         ; preds = %merge_195
  %loadtmp10 = load i64, ptr @msc_type_3, align 8
  ret i64 %loadtmp10

then_198:                                         ; preds = %merge_197
  %loadtmp13 = load i64, ptr @msc_type_4, align 8
  ret i64 %loadtmp13

then_200:                                         ; preds = %merge_199
  %loadtmp16 = load i64, ptr @msc_type_5, align 8
  ret i64 %loadtmp16

then_202:                                         ; preds = %merge_201
  %loadtmp19 = load i64, ptr @msc_type_6, align 8
  ret i64 %loadtmp19

then_204:                                         ; preds = %merge_203
  %loadtmp22 = load i64, ptr @msc_type_7, align 8
  ret i64 %loadtmp22
}

define i64 @msc_set_type(i64 %idx, i64 %val) {
entry:
  %"%msc_type_7" = alloca i64, align 8
  %"%msc_type_6" = alloca i64, align 8
  %"%msc_type_5" = alloca i64, align 8
  %"%msc_type_4" = alloca i64, align 8
  %"%msc_type_3" = alloca i64, align 8
  %"%msc_type_2" = alloca i64, align 8
  %"%msc_type_1" = alloca i64, align 8
  %"%msc_type_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_206, label %merge_207

merge_207:                                        ; preds = %then_206, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_208, label %merge_209

merge_209:                                        ; preds = %then_208, %merge_207
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_210, label %merge_211

merge_211:                                        ; preds = %then_210, %merge_209
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_212, label %merge_213

merge_213:                                        ; preds = %then_212, %merge_211
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_214, label %merge_215

merge_215:                                        ; preds = %then_214, %merge_213
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_216, label %merge_217

merge_217:                                        ; preds = %then_216, %merge_215
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_218, label %merge_219

merge_219:                                        ; preds = %then_218, %merge_217
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_220, label %merge_221

merge_221:                                        ; preds = %then_220, %merge_219
  ret i64 0

then_206:                                         ; preds = %entry
  %"%537.load" = load i64, ptr %val2, align 8
  store i64 %"%537.load", ptr %"%msc_type_0", align 8
  br label %merge_207

then_208:                                         ; preds = %merge_207
  %"%541.load" = load i64, ptr %val2, align 8
  store i64 %"%541.load", ptr %"%msc_type_1", align 8
  br label %merge_209

then_210:                                         ; preds = %merge_209
  %"%545.load" = load i64, ptr %val2, align 8
  store i64 %"%545.load", ptr %"%msc_type_2", align 8
  br label %merge_211

then_212:                                         ; preds = %merge_211
  %"%549.load" = load i64, ptr %val2, align 8
  store i64 %"%549.load", ptr %"%msc_type_3", align 8
  br label %merge_213

then_214:                                         ; preds = %merge_213
  %"%553.load" = load i64, ptr %val2, align 8
  store i64 %"%553.load", ptr %"%msc_type_4", align 8
  br label %merge_215

then_216:                                         ; preds = %merge_215
  %"%557.load" = load i64, ptr %val2, align 8
  store i64 %"%557.load", ptr %"%msc_type_5", align 8
  br label %merge_217

then_218:                                         ; preds = %merge_217
  %"%561.load" = load i64, ptr %val2, align 8
  store i64 %"%561.load", ptr %"%msc_type_6", align 8
  br label %merge_219

then_220:                                         ; preds = %merge_219
  %"%565.load" = load i64, ptr %val2, align 8
  store i64 %"%565.load", ptr %"%msc_type_7", align 8
  br label %merge_221
}

define i64 @msc_build_cbw(i64 %cbw_buf, i64 %tag, i64 %xfer_len, i64 %direction, i64 %lun, i64 %cb_len) {
entry:
  %cb_len6 = alloca i64, align 8
  store i64 %cb_len, ptr %cb_len6, align 8
  %lun5 = alloca i64, align 8
  store i64 %lun, ptr %lun5, align 8
  %direction4 = alloca i64, align 8
  store i64 %direction, ptr %direction4, align 8
  %xfer_len3 = alloca i64, align 8
  store i64 %xfer_len, ptr %xfer_len3, align 8
  %tag2 = alloca i64, align 8
  store i64 %tag, ptr %tag2, align 8
  %cbw_buf1 = alloca i64, align 8
  store i64 %cbw_buf, ptr %cbw_buf1, align 8
  %"%cbw_buf.load" = load i64, ptr %cbw_buf1, align 8
  %"%CBW_SIZE.load" = load i64, ptr @CBW_SIZE, align 8
  call void @sad_ll_memset(i64 %"%cbw_buf.load", i64 0, i64 %"%CBW_SIZE.load")
  %"%570.load" = load i64, ptr %cbw_buf1, align 8
  call void @sad_ll_mem_write8(i64 %"%570.load", i64 85)
  %"%cbw_buf.load7" = load i64, ptr %cbw_buf1, align 8
  %addtmp = add i64 %"%cbw_buf.load7", 1
  call void @sad_ll_mem_write8(i64 %addtmp, i64 83)
  %"%cbw_buf.load8" = load i64, ptr %cbw_buf1, align 8
  %addtmp9 = add i64 %"%cbw_buf.load8", 2
  call void @sad_ll_mem_write8(i64 %addtmp9, i64 66)
  %"%cbw_buf.load10" = load i64, ptr %cbw_buf1, align 8
  %addtmp11 = add i64 %"%cbw_buf.load10", 3
  call void @sad_ll_mem_write8(i64 %addtmp11, i64 67)
  %"%cbw_buf.load12" = load i64, ptr %cbw_buf1, align 8
  %addtmp13 = add i64 %"%cbw_buf.load12", 4
  %"%tag.load" = load i64, ptr %tag2, align 8
  %andtmp = and i64 %"%tag.load", 255
  call void @sad_ll_mem_write8(i64 %addtmp13, i64 %andtmp)
  %"%cbw_buf.load14" = load i64, ptr %cbw_buf1, align 8
  %addtmp15 = add i64 %"%cbw_buf.load14", 5
  %"%tag.load16" = load i64, ptr %tag2, align 8
  %shrtmp = lshr i64 %"%tag.load16", 8
  %andtmp17 = and i64 %shrtmp, 255
  call void @sad_ll_mem_write8(i64 %addtmp15, i64 %andtmp17)
  %"%cbw_buf.load18" = load i64, ptr %cbw_buf1, align 8
  %addtmp19 = add i64 %"%cbw_buf.load18", 6
  %"%tag.load20" = load i64, ptr %tag2, align 8
  %shrtmp21 = lshr i64 %"%tag.load20", 16
  %andtmp22 = and i64 %shrtmp21, 255
  call void @sad_ll_mem_write8(i64 %addtmp19, i64 %andtmp22)
  %"%cbw_buf.load23" = load i64, ptr %cbw_buf1, align 8
  %addtmp24 = add i64 %"%cbw_buf.load23", 7
  %"%tag.load25" = load i64, ptr %tag2, align 8
  %shrtmp26 = lshr i64 %"%tag.load25", 24
  %andtmp27 = and i64 %shrtmp26, 255
  call void @sad_ll_mem_write8(i64 %addtmp24, i64 %andtmp27)
  %"%cbw_buf.load28" = load i64, ptr %cbw_buf1, align 8
  %addtmp29 = add i64 %"%cbw_buf.load28", 8
  %"%xfer_len.load" = load i64, ptr %xfer_len3, align 8
  %andtmp30 = and i64 %"%xfer_len.load", 255
  call void @sad_ll_mem_write8(i64 %addtmp29, i64 %andtmp30)
  %"%cbw_buf.load31" = load i64, ptr %cbw_buf1, align 8
  %addtmp32 = add i64 %"%cbw_buf.load31", 9
  %"%xfer_len.load33" = load i64, ptr %xfer_len3, align 8
  %shrtmp34 = lshr i64 %"%xfer_len.load33", 8
  %andtmp35 = and i64 %shrtmp34, 255
  call void @sad_ll_mem_write8(i64 %addtmp32, i64 %andtmp35)
  %"%cbw_buf.load36" = load i64, ptr %cbw_buf1, align 8
  %addtmp37 = add i64 %"%cbw_buf.load36", 10
  %"%xfer_len.load38" = load i64, ptr %xfer_len3, align 8
  %shrtmp39 = lshr i64 %"%xfer_len.load38", 16
  %andtmp40 = and i64 %shrtmp39, 255
  call void @sad_ll_mem_write8(i64 %addtmp37, i64 %andtmp40)
  %"%cbw_buf.load41" = load i64, ptr %cbw_buf1, align 8
  %addtmp42 = add i64 %"%cbw_buf.load41", 11
  %"%xfer_len.load43" = load i64, ptr %xfer_len3, align 8
  %shrtmp44 = lshr i64 %"%xfer_len.load43", 24
  %andtmp45 = and i64 %shrtmp44, 255
  call void @sad_ll_mem_write8(i64 %addtmp42, i64 %andtmp45)
  %"%cbw_buf.load46" = load i64, ptr %cbw_buf1, align 8
  %addtmp47 = add i64 %"%cbw_buf.load46", 12
  %"%640.load" = load i64, ptr %direction4, align 8
  call void @sad_ll_mem_write8(i64 %addtmp47, i64 %"%640.load")
  %"%cbw_buf.load48" = load i64, ptr %cbw_buf1, align 8
  %addtmp49 = add i64 %"%cbw_buf.load48", 13
  %"%645.load" = load i64, ptr %lun5, align 8
  call void @sad_ll_mem_write8(i64 %addtmp49, i64 %"%645.load")
  %"%cbw_buf.load50" = load i64, ptr %cbw_buf1, align 8
  %addtmp51 = add i64 %"%cbw_buf.load50", 14
  %"%650.load" = load i64, ptr %cb_len6, align 8
  call void @sad_ll_mem_write8(i64 %addtmp51, i64 %"%650.load")
  ret i64 0
}

define i64 @msc_parse_csw(i64 %csw_buf, i64 %expected_tag) {
entry:
  %"%status" = alloca i64, align 8
  %"%tag" = alloca i64, align 8
  %"%sig_3" = alloca i64, align 8
  %"%sig_2" = alloca i64, align 8
  %"%sig_1" = alloca i64, align 8
  %"%sig_0" = alloca i64, align 8
  %expected_tag2 = alloca i64, align 8
  store i64 %expected_tag, ptr %expected_tag2, align 8
  %csw_buf1 = alloca i64, align 8
  store i64 %csw_buf, ptr %csw_buf1, align 8
  %"%654.load" = load i64, ptr %csw_buf1, align 8
  call void @sad_ll_mem_read8(i64 %"%654.load")
  store i64 0, ptr %"%sig_0", align 8
  %"%csw_buf.load" = load i64, ptr %csw_buf1, align 8
  %addtmp = add i64 %"%csw_buf.load", 1
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%sig_1", align 8
  %"%csw_buf.load3" = load i64, ptr %csw_buf1, align 8
  %addtmp4 = add i64 %"%csw_buf.load3", 2
  call void @sad_ll_mem_read8(i64 %addtmp4)
  store i64 0, ptr %"%sig_2", align 8
  %"%csw_buf.load5" = load i64, ptr %csw_buf1, align 8
  %addtmp6 = add i64 %"%csw_buf.load5", 3
  call void @sad_ll_mem_read8(i64 %addtmp6)
  store i64 0, ptr %"%sig_3", align 8
  %"%sig_0.load" = load i64, ptr %"%sig_0", align 8
  %cmpnetmp = icmp ne i64 %"%sig_0.load", 85
  br i1 %cmpnetmp, label %then_222, label %merge_223

merge_223:                                        ; preds = %entry
  %"%sig_1.load" = load i64, ptr %"%sig_1", align 8
  %cmpnetmp7 = icmp ne i64 %"%sig_1.load", 83
  br i1 %cmpnetmp7, label %then_224, label %merge_225

merge_225:                                        ; preds = %merge_223
  %"%sig_2.load" = load i64, ptr %"%sig_2", align 8
  %cmpnetmp8 = icmp ne i64 %"%sig_2.load", 66
  br i1 %cmpnetmp8, label %then_226, label %merge_227

merge_227:                                        ; preds = %merge_225
  %"%sig_3.load" = load i64, ptr %"%sig_3", align 8
  %cmpnetmp9 = icmp ne i64 %"%sig_3.load", 83
  br i1 %cmpnetmp9, label %then_228, label %merge_229

merge_229:                                        ; preds = %merge_227
  %"%csw_buf.load10" = load i64, ptr %csw_buf1, align 8
  %addtmp11 = add i64 %"%csw_buf.load10", 4
  call void @sad_ll_mem_read8(i64 %addtmp11)
  store i64 0, ptr %"%tag", align 8
  %"%csw_buf.load12" = load i64, ptr %csw_buf1, align 8
  %addtmp13 = add i64 %"%csw_buf.load12", 5
  call void @sad_ll_mem_read8(i64 %addtmp13)
  %"%tag.load" = load i64, ptr %"%tag", align 8
  %addtmp14 = add i64 %"%tag.load", 0
  store i64 %addtmp14, ptr %"%tag", align 8
  %"%csw_buf.load15" = load i64, ptr %csw_buf1, align 8
  %addtmp16 = add i64 %"%csw_buf.load15", 6
  call void @sad_ll_mem_read8(i64 %addtmp16)
  %"%tag.load17" = load i64, ptr %"%tag", align 8
  %addtmp18 = add i64 %"%tag.load17", 0
  store i64 %addtmp18, ptr %"%tag", align 8
  %"%csw_buf.load19" = load i64, ptr %csw_buf1, align 8
  %addtmp20 = add i64 %"%csw_buf.load19", 7
  call void @sad_ll_mem_read8(i64 %addtmp20)
  %"%tag.load21" = load i64, ptr %"%tag", align 8
  %addtmp22 = add i64 %"%tag.load21", 0
  store i64 %addtmp22, ptr %"%tag", align 8
  %"%tag.load23" = load i64, ptr %"%tag", align 8
  %"%expected_tag.load" = load i64, ptr %expected_tag2, align 8
  %cmpnetmp24 = icmp ne i64 %"%tag.load23", %"%expected_tag.load"
  br i1 %cmpnetmp24, label %then_230, label %merge_231

merge_231:                                        ; preds = %merge_229
  %"%csw_buf.load26" = load i64, ptr %csw_buf1, align 8
  %addtmp27 = add i64 %"%csw_buf.load26", 12
  call void @sad_ll_mem_read8(i64 %addtmp27)
  store i64 0, ptr %"%status", align 8
  %loadtmp = load i64, ptr %"%status", align 8
  ret i64 %loadtmp

then_222:                                         ; preds = %entry
  ret i64 -1

then_224:                                         ; preds = %merge_223
  ret i64 -1

then_226:                                         ; preds = %merge_225
  ret i64 -1

then_228:                                         ; preds = %merge_227
  ret i64 -1

then_230:                                         ; preds = %merge_229
  %calltmp = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %calltmp25 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -2
}

define i64 @msc_bot_transfer(i64 %dev_idx, i64 %scsi_cmd, i64 %cmd_len, i64 %data_buf, i64 %data_len, i64 %direction) {
entry:
  %"%csw_status" = alloca i64, align 8
  %"%write_result" = alloca i64, align 8
  %"%read_result" = alloca i64, align 8
  %"%csw_result" = alloca i64, align 8
  %"%msc_errors" = alloca i64, align 8
  %"%send_result" = alloca i64, align 8
  %"%cmd_byte" = alloca i64, align 8
  %"%ci" = alloca i64, align 8
  %"%msc_tag_counter" = alloca i64, align 8
  %"%tag" = alloca i64, align 8
  %"%addr" = alloca i64, align 8
  %"%hc" = alloca i64, align 8
  %direction6 = alloca i64, align 8
  store i64 %direction, ptr %direction6, align 8
  %data_len5 = alloca i64, align 8
  store i64 %data_len, ptr %data_len5, align 8
  %data_buf4 = alloca i64, align 8
  store i64 %data_buf, ptr %data_buf4, align 8
  %cmd_len3 = alloca i64, align 8
  store i64 %cmd_len, ptr %cmd_len3, align 8
  %scsi_cmd2 = alloca i64, align 8
  store i64 %scsi_cmd, ptr %scsi_cmd2, align 8
  %dev_idx1 = alloca i64, align 8
  store i64 %dev_idx, ptr %dev_idx1, align 8
  %"%dev_idx.load" = load i64, ptr %dev_idx1, align 8
  %cmplttmp = icmp slt i64 %"%dev_idx.load", 0
  br i1 %cmplttmp, label %then_232, label %merge_233

else_247:                                         ; preds = %then_243
  %"%hc.load42" = load i64, ptr %"%hc", align 8
  %"%addr.load43" = load i64, ptr %"%addr", align 8
  %"%BULK_EP_OUT.load44" = load i64, ptr @BULK_EP_OUT, align 8
  %"%data_buf.load45" = load i64, ptr %data_buf4, align 8
  %"%data_len.load46" = load i64, ptr %data_len5, align 8
  call void @uhci_bulk_transfer(i64 %"%hc.load42", i64 %"%addr.load43", i64 %"%BULK_EP_OUT.load44", i64 %"%data_buf.load45", i64 %"%data_len.load46")
  store i64 0, ptr %"%write_result", align 8
  %"%write_result.load" = load i64, ptr %"%write_result", align 8
  %cmplttmp47 = icmp slt i64 %"%write_result.load", 0
  br i1 %cmplttmp47, label %then_250, label %merge_251

merge_233:                                        ; preds = %entry
  %"%dev_idx.load7" = load i64, ptr %dev_idx1, align 8
  %"%MSC_MAX_DEVICES.load" = load i64, ptr @MSC_MAX_DEVICES, align 8
  %cmpgetmp = icmp sge i64 %"%dev_idx.load7", %"%MSC_MAX_DEVICES.load"
  br i1 %cmpgetmp, label %then_234, label %merge_235

merge_235:                                        ; preds = %merge_233
  %"%dev_idx.load8" = load i64, ptr %dev_idx1, align 8
  %calltmp = call i64 @msc_get_active(i64 %"%dev_idx.load8")
  %cmpeqtmp = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp, label %then_236, label %merge_237

merge_237:                                        ; preds = %merge_235
  %"%dev_idx.load9" = load i64, ptr %dev_idx1, align 8
  %calltmp10 = call i64 @msc_get_hc(i64 %"%dev_idx.load9")
  store i64 %calltmp10, ptr %"%hc", align 8
  %"%dev_idx.load11" = load i64, ptr %dev_idx1, align 8
  %calltmp12 = call i64 @msc_get_addr(i64 %"%dev_idx.load11")
  store i64 %calltmp12, ptr %"%addr", align 8
  %"%761.load" = load i64, ptr @msc_tag_counter, align 8
  store i64 %"%761.load", ptr %"%tag", align 8
  %"%msc_tag_counter.load" = load i64, ptr @msc_tag_counter, align 8
  %addtmp = add i64 %"%msc_tag_counter.load", 1
  store i64 %addtmp, ptr %"%msc_tag_counter", align 8
  %"%msc_cbw_buf.load" = load i64, ptr @msc_cbw_buf, align 8
  %"%tag.load" = load i64, ptr %"%tag", align 8
  %"%data_len.load" = load i64, ptr %data_len5, align 8
  %"%direction.load" = load i64, ptr %direction6, align 8
  %"%cmd_len.load" = load i64, ptr %cmd_len3, align 8
  %calltmp13 = call i64 @msc_build_cbw(i64 %"%msc_cbw_buf.load", i64 %"%tag.load", i64 %"%data_len.load", i64 %"%direction.load", i64 0, i64 %"%cmd_len.load")
  store i64 0, ptr %"%ci", align 8
  br label %while_cond_238

merge_242:                                        ; preds = %while_exit_240
  %"%data_len.load29" = load i64, ptr %data_len5, align 8
  %cmpgttmp = icmp sgt i64 %"%data_len.load29", 0
  br i1 %cmpgttmp, label %then_243, label %merge_244

merge_244:                                        ; preds = %merge_246, %merge_242
  %"%msc_csw_buf.load" = load i64, ptr @msc_csw_buf, align 8
  %"%CSW_SIZE.load" = load i64, ptr @CSW_SIZE, align 8
  call void @sad_ll_memset(i64 %"%msc_csw_buf.load", i64 0, i64 %"%CSW_SIZE.load")
  %"%hc.load32" = load i64, ptr %"%hc", align 8
  %"%addr.load33" = load i64, ptr %"%addr", align 8
  %"%BULK_EP_IN.load" = load i64, ptr @BULK_EP_IN, align 8
  %"%msc_csw_buf.load34" = load i64, ptr @msc_csw_buf, align 8
  %"%CSW_SIZE.load35" = load i64, ptr @CSW_SIZE, align 8
  call void @uhci_bulk_transfer(i64 %"%hc.load32", i64 %"%addr.load33", i64 %"%BULK_EP_IN.load", i64 %"%msc_csw_buf.load34", i64 %"%CSW_SIZE.load35")
  store i64 0, ptr %"%csw_result", align 8
  %"%csw_result.load" = load i64, ptr %"%csw_result", align 8
  %cmplttmp36 = icmp slt i64 %"%csw_result.load", 0
  br i1 %cmplttmp36, label %then_252, label %merge_253

merge_246:                                        ; preds = %merge_251, %merge_249
  br label %merge_244

merge_249:                                        ; preds = %then_245
  br label %merge_246

merge_251:                                        ; preds = %else_247
  br label %merge_246

merge_253:                                        ; preds = %merge_244
  %"%msc_csw_buf.load60" = load i64, ptr @msc_csw_buf, align 8
  %"%tag.load61" = load i64, ptr %"%tag", align 8
  %calltmp62 = call i64 @msc_parse_csw(i64 %"%msc_csw_buf.load60", i64 %"%tag.load61")
  store i64 %calltmp62, ptr %"%csw_status", align 8
  %loadtmp = load i64, ptr %"%csw_status", align 8
  ret i64 %loadtmp

then_232:                                         ; preds = %entry
  ret i64 -1

then_234:                                         ; preds = %merge_233
  ret i64 -1

then_236:                                         ; preds = %merge_235
  ret i64 -1

then_241:                                         ; preds = %while_exit_240
  %calltmp26 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %calltmp27 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  %"%msc_errors.load" = load i64, ptr @msc_errors, align 8
  %addtmp28 = add i64 %"%msc_errors.load", 1
  store i64 %addtmp28, ptr %"%msc_errors", align 8
  ret i64 -3

then_243:                                         ; preds = %merge_242
  %"%direction.load30" = load i64, ptr %direction6, align 8
  %"%DIR_IN.load" = load i64, ptr @DIR_IN, align 8
  %cmpeqtmp31 = icmp eq i64 %"%direction.load30", %"%DIR_IN.load"
  br i1 %cmpeqtmp31, label %then_245, label %else_247

then_245:                                         ; preds = %then_243
  %"%hc.load37" = load i64, ptr %"%hc", align 8
  %"%addr.load38" = load i64, ptr %"%addr", align 8
  %"%BULK_EP_IN.load39" = load i64, ptr @BULK_EP_IN, align 8
  %"%data_buf.load" = load i64, ptr %data_buf4, align 8
  %"%data_len.load40" = load i64, ptr %data_len5, align 8
  call void @uhci_bulk_transfer(i64 %"%hc.load37", i64 %"%addr.load38", i64 %"%BULK_EP_IN.load39", i64 %"%data_buf.load", i64 %"%data_len.load40")
  store i64 0, ptr %"%read_result", align 8
  %"%read_result.load" = load i64, ptr %"%read_result", align 8
  %cmplttmp41 = icmp slt i64 %"%read_result.load", 0
  br i1 %cmplttmp41, label %then_248, label %merge_249

then_248:                                         ; preds = %then_245
  %calltmp48 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %calltmp49 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  %"%msc_errors.load50" = load i64, ptr %"%msc_errors", align 8
  %addtmp51 = add i64 %"%msc_errors.load50", 1
  store i64 %addtmp51, ptr %"%msc_errors", align 8
  ret i64 -4

then_250:                                         ; preds = %else_247
  %calltmp52 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %calltmp53 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  %"%msc_errors.load54" = load i64, ptr %"%msc_errors", align 8
  %addtmp55 = add i64 %"%msc_errors.load54", 1
  store i64 %addtmp55, ptr %"%msc_errors", align 8
  ret i64 -5

then_252:                                         ; preds = %merge_244
  %calltmp56 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %calltmp57 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  %"%msc_errors.load58" = load i64, ptr %"%msc_errors", align 8
  %addtmp59 = add i64 %"%msc_errors.load58", 1
  store i64 %addtmp59, ptr %"%msc_errors", align 8
  ret i64 -6

while_body_239:                                   ; preds = %while_cond_238
  %"%scsi_cmd.load" = load i64, ptr %scsi_cmd2, align 8
  %"%ci.load16" = load i64, ptr %"%ci", align 8
  %addtmp17 = add i64 %"%scsi_cmd.load", %"%ci.load16"
  call void @sad_ll_mem_read8(i64 %addtmp17)
  store i64 0, ptr %"%cmd_byte", align 8
  %"%msc_cbw_buf.load18" = load i64, ptr @msc_cbw_buf, align 8
  %addtmp19 = add i64 %"%msc_cbw_buf.load18", 15
  %"%ci.load20" = load i64, ptr %"%ci", align 8
  %addtmp21 = add i64 %addtmp19, %"%ci.load20"
  %"%cmd_byte.load" = load i64, ptr %"%cmd_byte", align 8
  call void @sad_ll_mem_write8(i64 %addtmp21, i64 %"%cmd_byte.load")
  %"%ci.load22" = load i64, ptr %"%ci", align 8
  %addtmp23 = add i64 %"%ci.load22", 1
  store i64 %addtmp23, ptr %"%ci", align 8
  br label %while_cond_238

while_cond_238:                                   ; preds = %while_body_239, %merge_237
  %"%ci.load" = load i64, ptr %"%ci", align 8
  %"%cmd_len.load14" = load i64, ptr %cmd_len3, align 8
  %cmplttmp15 = icmp slt i64 %"%ci.load", %"%cmd_len.load14"
  br i1 %cmplttmp15, label %while_body_239, label %while_exit_240

while_exit_240:                                   ; preds = %while_cond_238
  %"%hc.load" = load i64, ptr %"%hc", align 8
  %"%addr.load" = load i64, ptr %"%addr", align 8
  %"%BULK_EP_OUT.load" = load i64, ptr @BULK_EP_OUT, align 8
  %"%msc_cbw_buf.load24" = load i64, ptr @msc_cbw_buf, align 8
  %"%CBW_SIZE.load" = load i64, ptr @CBW_SIZE, align 8
  call void @uhci_bulk_transfer(i64 %"%hc.load", i64 %"%addr.load", i64 %"%BULK_EP_OUT.load", i64 %"%msc_cbw_buf.load24", i64 %"%CBW_SIZE.load")
  store i64 0, ptr %"%send_result", align 8
  %"%send_result.load" = load i64, ptr %"%send_result", align 8
  %cmplttmp25 = icmp slt i64 %"%send_result.load", 0
  br i1 %cmplttmp25, label %then_241, label %merge_242
}

define i64 @msc_test_unit_ready(i64 %dev_idx) {
entry:
  %"%result" = alloca i64, align 8
  %dev_idx1 = alloca i64, align 8
  store i64 %dev_idx, ptr %dev_idx1, align 8
  %"%msc_data_buf.load" = load i64, ptr @msc_data_buf, align 8
  call void @sad_ll_memset(i64 %"%msc_data_buf.load", i64 0, i64 6)
  %"%msc_data_buf.load2" = load i64, ptr @msc_data_buf, align 8
  %"%SCSI_TEST_UNIT_READY.load" = load i64, ptr @SCSI_TEST_UNIT_READY, align 8
  call void @sad_ll_mem_write8(i64 %"%msc_data_buf.load2", i64 %"%SCSI_TEST_UNIT_READY.load")
  %"%dev_idx.load" = load i64, ptr %dev_idx1, align 8
  %"%msc_data_buf.load3" = load i64, ptr @msc_data_buf, align 8
  %"%DIR_OUT.load" = load i64, ptr @DIR_OUT, align 8
  %calltmp = call i64 @msc_bot_transfer(i64 %"%dev_idx.load", i64 %"%msc_data_buf.load3", i64 6, i64 0, i64 0, i64 %"%DIR_OUT.load")
  store i64 %calltmp, ptr %"%result", align 8
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp
}

define i64 @msc_inquiry(i64 %dev_idx, i64 %resp_buf) {
entry:
  %"%dev_type" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %resp_buf2 = alloca i64, align 8
  store i64 %resp_buf, ptr %resp_buf2, align 8
  %dev_idx1 = alloca i64, align 8
  store i64 %dev_idx, ptr %dev_idx1, align 8
  %"%msc_data_buf.load" = load i64, ptr @msc_data_buf, align 8
  call void @sad_ll_memset(i64 %"%msc_data_buf.load", i64 0, i64 16)
  %"%msc_data_buf.load3" = load i64, ptr @msc_data_buf, align 8
  %"%SCSI_INQUIRY.load" = load i64, ptr @SCSI_INQUIRY, align 8
  call void @sad_ll_mem_write8(i64 %"%msc_data_buf.load3", i64 %"%SCSI_INQUIRY.load")
  %"%msc_data_buf.load4" = load i64, ptr @msc_data_buf, align 8
  %addtmp = add i64 %"%msc_data_buf.load4", 4
  call void @sad_ll_mem_write8(i64 %addtmp, i64 36)
  %"%dev_idx.load" = load i64, ptr %dev_idx1, align 8
  %"%msc_data_buf.load5" = load i64, ptr @msc_data_buf, align 8
  %"%resp_buf.load" = load i64, ptr %resp_buf2, align 8
  %"%DIR_IN.load" = load i64, ptr @DIR_IN, align 8
  %calltmp = call i64 @msc_bot_transfer(i64 %"%dev_idx.load", i64 %"%msc_data_buf.load5", i64 6, i64 %"%resp_buf.load", i64 36, i64 %"%DIR_IN.load")
  store i64 %calltmp, ptr %"%result", align 8
  %"%result.load" = load i64, ptr %"%result", align 8
  %"%CSW_STATUS_PASSED.load" = load i64, ptr @CSW_STATUS_PASSED, align 8
  %cmpeqtmp = icmp eq i64 %"%result.load", %"%CSW_STATUS_PASSED.load"
  br i1 %cmpeqtmp, label %then_254, label %merge_255

merge_255:                                        ; preds = %then_254, %entry
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

then_254:                                         ; preds = %entry
  %"%resp_buf.load6" = load i64, ptr %resp_buf2, align 8
  call void @sad_ll_mem_read8(i64 %"%resp_buf.load6")
  store i64 0, ptr %"%dev_type", align 8
  %"%dev_idx.load7" = load i64, ptr %dev_idx1, align 8
  %"%dev_type.load" = load i64, ptr %"%dev_type", align 8
  %calltmp8 = call i64 @msc_set_type(i64 %"%dev_idx.load7", i64 %"%dev_type.load")
  br label %merge_255
}

define i64 @msc_read_capacity(i64 %dev_idx) {
entry:
  %"%block_size" = alloca i64, align 8
  %"%last_lba" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%resp" = alloca i64, align 8
  %dev_idx1 = alloca i64, align 8
  store i64 %dev_idx, ptr %dev_idx1, align 8
  %"%msc_data_buf.load" = load i64, ptr @msc_data_buf, align 8
  call void @sad_ll_memset(i64 %"%msc_data_buf.load", i64 0, i64 16)
  %"%msc_data_buf.load2" = load i64, ptr @msc_data_buf, align 8
  %"%SCSI_READ_CAPACITY.load" = load i64, ptr @SCSI_READ_CAPACITY, align 8
  call void @sad_ll_mem_write8(i64 %"%msc_data_buf.load2", i64 %"%SCSI_READ_CAPACITY.load")
  call void @sad_ll_kmalloc(i64 8)
  store i64 0, ptr %"%resp", align 8
  %"%resp.load" = load i64, ptr %"%resp", align 8
  call void @sad_ll_memset(i64 %"%resp.load", i64 0, i64 8)
  %"%dev_idx.load" = load i64, ptr %dev_idx1, align 8
  %"%msc_data_buf.load3" = load i64, ptr @msc_data_buf, align 8
  %"%resp.load4" = load i64, ptr %"%resp", align 8
  %"%DIR_IN.load" = load i64, ptr @DIR_IN, align 8
  %calltmp = call i64 @msc_bot_transfer(i64 %"%dev_idx.load", i64 %"%msc_data_buf.load3", i64 10, i64 %"%resp.load4", i64 8, i64 %"%DIR_IN.load")
  store i64 %calltmp, ptr %"%result", align 8
  %"%result.load" = load i64, ptr %"%result", align 8
  %"%CSW_STATUS_PASSED.load" = load i64, ptr @CSW_STATUS_PASSED, align 8
  %cmpeqtmp = icmp eq i64 %"%result.load", %"%CSW_STATUS_PASSED.load"
  br i1 %cmpeqtmp, label %then_256, label %merge_257

merge_257:                                        ; preds = %then_256, %entry
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

then_256:                                         ; preds = %entry
  %"%resp.load5" = load i64, ptr %"%resp", align 8
  call void @sad_ll_mem_read8(i64 %"%resp.load5")
  store i64 0, ptr %"%last_lba", align 8
  %"%resp.load6" = load i64, ptr %"%resp", align 8
  %addtmp = add i64 %"%resp.load6", 1
  call void @sad_ll_mem_read8(i64 %addtmp)
  %"%last_lba.load" = load i64, ptr %"%last_lba", align 8
  %addtmp7 = add i64 %"%last_lba.load", 0
  store i64 %addtmp7, ptr %"%last_lba", align 8
  %"%resp.load8" = load i64, ptr %"%resp", align 8
  %addtmp9 = add i64 %"%resp.load8", 2
  call void @sad_ll_mem_read8(i64 %addtmp9)
  %"%last_lba.load10" = load i64, ptr %"%last_lba", align 8
  %addtmp11 = add i64 %"%last_lba.load10", 0
  store i64 %addtmp11, ptr %"%last_lba", align 8
  %"%resp.load12" = load i64, ptr %"%resp", align 8
  %addtmp13 = add i64 %"%resp.load12", 3
  call void @sad_ll_mem_read8(i64 %addtmp13)
  %"%last_lba.load14" = load i64, ptr %"%last_lba", align 8
  %addtmp15 = add i64 %"%last_lba.load14", 0
  store i64 %addtmp15, ptr %"%last_lba", align 8
  %"%resp.load16" = load i64, ptr %"%resp", align 8
  %addtmp17 = add i64 %"%resp.load16", 4
  call void @sad_ll_mem_read8(i64 %addtmp17)
  store i64 0, ptr %"%block_size", align 8
  %"%resp.load18" = load i64, ptr %"%resp", align 8
  %addtmp19 = add i64 %"%resp.load18", 5
  call void @sad_ll_mem_read8(i64 %addtmp19)
  %"%block_size.load" = load i64, ptr %"%block_size", align 8
  %addtmp20 = add i64 %"%block_size.load", 0
  store i64 %addtmp20, ptr %"%block_size", align 8
  %"%resp.load21" = load i64, ptr %"%resp", align 8
  %addtmp22 = add i64 %"%resp.load21", 6
  call void @sad_ll_mem_read8(i64 %addtmp22)
  %"%block_size.load23" = load i64, ptr %"%block_size", align 8
  %addtmp24 = add i64 %"%block_size.load23", 0
  store i64 %addtmp24, ptr %"%block_size", align 8
  %"%resp.load25" = load i64, ptr %"%resp", align 8
  %addtmp26 = add i64 %"%resp.load25", 7
  call void @sad_ll_mem_read8(i64 %addtmp26)
  %"%block_size.load27" = load i64, ptr %"%block_size", align 8
  %addtmp28 = add i64 %"%block_size.load27", 0
  store i64 %addtmp28, ptr %"%block_size", align 8
  %"%last_lba.load29" = load i64, ptr %"%last_lba", align 8
  %addtmp30 = add i64 %"%last_lba.load29", 1
  %"%dev_idx.load31" = load i64, ptr %dev_idx1, align 8
  %calltmp32 = call i64 @msc_set_sectors(i64 %"%dev_idx.load31", i64 %addtmp30)
  %"%dev_idx.load33" = load i64, ptr %dev_idx1, align 8
  %"%block_size.load34" = load i64, ptr %"%block_size", align 8
  %calltmp35 = call i64 @msc_set_sector_size(i64 %"%dev_idx.load33", i64 %"%block_size.load34")
  %calltmp36 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %calltmp37 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %addtmp30)
  %calltmp38 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  %"%950.load" = load i64, ptr %"%block_size", align 8
  %calltmp39 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %"%950.load")
  %calltmp40 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_257
}

define i64 @msc_request_sense(i64 %dev_idx, i64 %sense_buf) {
entry:
  %"%result" = alloca i64, align 8
  %sense_buf2 = alloca i64, align 8
  store i64 %sense_buf, ptr %sense_buf2, align 8
  %dev_idx1 = alloca i64, align 8
  store i64 %dev_idx, ptr %dev_idx1, align 8
  %"%msc_data_buf.load" = load i64, ptr @msc_data_buf, align 8
  call void @sad_ll_memset(i64 %"%msc_data_buf.load", i64 0, i64 16)
  %"%msc_data_buf.load3" = load i64, ptr @msc_data_buf, align 8
  %"%SCSI_REQUEST_SENSE.load" = load i64, ptr @SCSI_REQUEST_SENSE, align 8
  call void @sad_ll_mem_write8(i64 %"%msc_data_buf.load3", i64 %"%SCSI_REQUEST_SENSE.load")
  %"%msc_data_buf.load4" = load i64, ptr @msc_data_buf, align 8
  %addtmp = add i64 %"%msc_data_buf.load4", 4
  call void @sad_ll_mem_write8(i64 %addtmp, i64 18)
  %"%dev_idx.load" = load i64, ptr %dev_idx1, align 8
  %"%msc_data_buf.load5" = load i64, ptr @msc_data_buf, align 8
  %"%sense_buf.load" = load i64, ptr %sense_buf2, align 8
  %"%DIR_IN.load" = load i64, ptr @DIR_IN, align 8
  %calltmp = call i64 @msc_bot_transfer(i64 %"%dev_idx.load", i64 %"%msc_data_buf.load5", i64 6, i64 %"%sense_buf.load", i64 18, i64 %"%DIR_IN.load")
  store i64 %calltmp, ptr %"%result", align 8
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp
}

define i64 @msc_read_sectors(i64 %dev_idx, i64 %lba, i64 %count, i64 %dest_buf) {
entry:
  %"%msc_errors" = alloca i64, align 8
  %"%msc_reads" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%xfer_len" = alloca i64, align 8
  %"%sec_size" = alloca i64, align 8
  %dest_buf4 = alloca i64, align 8
  store i64 %dest_buf, ptr %dest_buf4, align 8
  %count3 = alloca i64, align 8
  store i64 %count, ptr %count3, align 8
  %lba2 = alloca i64, align 8
  store i64 %lba, ptr %lba2, align 8
  %dev_idx1 = alloca i64, align 8
  store i64 %dev_idx, ptr %dev_idx1, align 8
  %"%dev_idx.load" = load i64, ptr %dev_idx1, align 8
  %cmplttmp = icmp slt i64 %"%dev_idx.load", 0
  br i1 %cmplttmp, label %then_258, label %merge_259

else_264:                                         ; preds = %merge_261
  %"%msc_errors.load" = load i64, ptr @msc_errors, align 8
  %addtmp39 = add i64 %"%msc_errors.load", 1
  store i64 %addtmp39, ptr %"%msc_errors", align 8
  br label %merge_263

merge_259:                                        ; preds = %entry
  %"%dev_idx.load5" = load i64, ptr %dev_idx1, align 8
  %calltmp = call i64 @msc_get_active(i64 %"%dev_idx.load5")
  %cmpeqtmp = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp, label %then_260, label %merge_261

merge_261:                                        ; preds = %merge_259
  %"%dev_idx.load6" = load i64, ptr %dev_idx1, align 8
  %calltmp7 = call i64 @msc_get_sector_size(i64 %"%dev_idx.load6")
  store i64 %calltmp7, ptr %"%sec_size", align 8
  %"%count.load" = load i64, ptr %count3, align 8
  %"%sec_size.load" = load i64, ptr %"%sec_size", align 8
  %multmp = mul i64 %"%count.load", %"%sec_size.load"
  store i64 %multmp, ptr %"%xfer_len", align 8
  %"%msc_data_buf.load" = load i64, ptr @msc_data_buf, align 8
  call void @sad_ll_memset(i64 %"%msc_data_buf.load", i64 0, i64 16)
  %"%msc_data_buf.load8" = load i64, ptr @msc_data_buf, align 8
  %"%SCSI_READ_10.load" = load i64, ptr @SCSI_READ_10, align 8
  call void @sad_ll_mem_write8(i64 %"%msc_data_buf.load8", i64 %"%SCSI_READ_10.load")
  %"%msc_data_buf.load9" = load i64, ptr @msc_data_buf, align 8
  %addtmp = add i64 %"%msc_data_buf.load9", 2
  %"%lba.load" = load i64, ptr %lba2, align 8
  %shrtmp = lshr i64 %"%lba.load", 24
  %andtmp = and i64 %shrtmp, 255
  call void @sad_ll_mem_write8(i64 %addtmp, i64 %andtmp)
  %"%msc_data_buf.load10" = load i64, ptr @msc_data_buf, align 8
  %addtmp11 = add i64 %"%msc_data_buf.load10", 3
  %"%lba.load12" = load i64, ptr %lba2, align 8
  %shrtmp13 = lshr i64 %"%lba.load12", 16
  %andtmp14 = and i64 %shrtmp13, 255
  call void @sad_ll_mem_write8(i64 %addtmp11, i64 %andtmp14)
  %"%msc_data_buf.load15" = load i64, ptr @msc_data_buf, align 8
  %addtmp16 = add i64 %"%msc_data_buf.load15", 4
  %"%lba.load17" = load i64, ptr %lba2, align 8
  %shrtmp18 = lshr i64 %"%lba.load17", 8
  %andtmp19 = and i64 %shrtmp18, 255
  call void @sad_ll_mem_write8(i64 %addtmp16, i64 %andtmp19)
  %"%msc_data_buf.load20" = load i64, ptr @msc_data_buf, align 8
  %addtmp21 = add i64 %"%msc_data_buf.load20", 5
  %"%lba.load22" = load i64, ptr %lba2, align 8
  %andtmp23 = and i64 %"%lba.load22", 255
  call void @sad_ll_mem_write8(i64 %addtmp21, i64 %andtmp23)
  %"%msc_data_buf.load24" = load i64, ptr @msc_data_buf, align 8
  %addtmp25 = add i64 %"%msc_data_buf.load24", 7
  %"%count.load26" = load i64, ptr %count3, align 8
  %shrtmp27 = lshr i64 %"%count.load26", 8
  %andtmp28 = and i64 %shrtmp27, 255
  call void @sad_ll_mem_write8(i64 %addtmp25, i64 %andtmp28)
  %"%msc_data_buf.load29" = load i64, ptr @msc_data_buf, align 8
  %addtmp30 = add i64 %"%msc_data_buf.load29", 8
  %"%count.load31" = load i64, ptr %count3, align 8
  %andtmp32 = and i64 %"%count.load31", 255
  call void @sad_ll_mem_write8(i64 %addtmp30, i64 %andtmp32)
  %"%dev_idx.load33" = load i64, ptr %dev_idx1, align 8
  %"%msc_data_buf.load34" = load i64, ptr @msc_data_buf, align 8
  %"%dest_buf.load" = load i64, ptr %dest_buf4, align 8
  %"%xfer_len.load" = load i64, ptr %"%xfer_len", align 8
  %"%DIR_IN.load" = load i64, ptr @DIR_IN, align 8
  %calltmp35 = call i64 @msc_bot_transfer(i64 %"%dev_idx.load33", i64 %"%msc_data_buf.load34", i64 10, i64 %"%dest_buf.load", i64 %"%xfer_len.load", i64 %"%DIR_IN.load")
  store i64 %calltmp35, ptr %"%result", align 8
  %"%result.load" = load i64, ptr %"%result", align 8
  %"%CSW_STATUS_PASSED.load" = load i64, ptr @CSW_STATUS_PASSED, align 8
  %cmpeqtmp36 = icmp eq i64 %"%result.load", %"%CSW_STATUS_PASSED.load"
  br i1 %cmpeqtmp36, label %then_262, label %else_264

merge_263:                                        ; preds = %else_264, %then_262
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

then_258:                                         ; preds = %entry
  ret i64 -1

then_260:                                         ; preds = %merge_259
  ret i64 -1

then_262:                                         ; preds = %merge_261
  %"%msc_reads.load" = load i64, ptr @msc_reads, align 8
  %"%count.load37" = load i64, ptr %count3, align 8
  %addtmp38 = add i64 %"%msc_reads.load", %"%count.load37"
  store i64 %addtmp38, ptr %"%msc_reads", align 8
  br label %merge_263
}

define i64 @msc_write_sectors(i64 %dev_idx, i64 %lba, i64 %count, i64 %src_buf) {
entry:
  %"%msc_errors" = alloca i64, align 8
  %"%msc_writes" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%xfer_len" = alloca i64, align 8
  %"%sec_size" = alloca i64, align 8
  %src_buf4 = alloca i64, align 8
  store i64 %src_buf, ptr %src_buf4, align 8
  %count3 = alloca i64, align 8
  store i64 %count, ptr %count3, align 8
  %lba2 = alloca i64, align 8
  store i64 %lba, ptr %lba2, align 8
  %dev_idx1 = alloca i64, align 8
  store i64 %dev_idx, ptr %dev_idx1, align 8
  %"%dev_idx.load" = load i64, ptr %dev_idx1, align 8
  %cmplttmp = icmp slt i64 %"%dev_idx.load", 0
  br i1 %cmplttmp, label %then_265, label %merge_266

else_271:                                         ; preds = %merge_268
  %"%msc_errors.load" = load i64, ptr @msc_errors, align 8
  %addtmp39 = add i64 %"%msc_errors.load", 1
  store i64 %addtmp39, ptr %"%msc_errors", align 8
  br label %merge_270

merge_266:                                        ; preds = %entry
  %"%dev_idx.load5" = load i64, ptr %dev_idx1, align 8
  %calltmp = call i64 @msc_get_active(i64 %"%dev_idx.load5")
  %cmpeqtmp = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp, label %then_267, label %merge_268

merge_268:                                        ; preds = %merge_266
  %"%dev_idx.load6" = load i64, ptr %dev_idx1, align 8
  %calltmp7 = call i64 @msc_get_sector_size(i64 %"%dev_idx.load6")
  store i64 %calltmp7, ptr %"%sec_size", align 8
  %"%count.load" = load i64, ptr %count3, align 8
  %"%sec_size.load" = load i64, ptr %"%sec_size", align 8
  %multmp = mul i64 %"%count.load", %"%sec_size.load"
  store i64 %multmp, ptr %"%xfer_len", align 8
  %"%msc_data_buf.load" = load i64, ptr @msc_data_buf, align 8
  call void @sad_ll_memset(i64 %"%msc_data_buf.load", i64 0, i64 16)
  %"%msc_data_buf.load8" = load i64, ptr @msc_data_buf, align 8
  %"%SCSI_WRITE_10.load" = load i64, ptr @SCSI_WRITE_10, align 8
  call void @sad_ll_mem_write8(i64 %"%msc_data_buf.load8", i64 %"%SCSI_WRITE_10.load")
  %"%msc_data_buf.load9" = load i64, ptr @msc_data_buf, align 8
  %addtmp = add i64 %"%msc_data_buf.load9", 2
  %"%lba.load" = load i64, ptr %lba2, align 8
  %shrtmp = lshr i64 %"%lba.load", 24
  %andtmp = and i64 %shrtmp, 255
  call void @sad_ll_mem_write8(i64 %addtmp, i64 %andtmp)
  %"%msc_data_buf.load10" = load i64, ptr @msc_data_buf, align 8
  %addtmp11 = add i64 %"%msc_data_buf.load10", 3
  %"%lba.load12" = load i64, ptr %lba2, align 8
  %shrtmp13 = lshr i64 %"%lba.load12", 16
  %andtmp14 = and i64 %shrtmp13, 255
  call void @sad_ll_mem_write8(i64 %addtmp11, i64 %andtmp14)
  %"%msc_data_buf.load15" = load i64, ptr @msc_data_buf, align 8
  %addtmp16 = add i64 %"%msc_data_buf.load15", 4
  %"%lba.load17" = load i64, ptr %lba2, align 8
  %shrtmp18 = lshr i64 %"%lba.load17", 8
  %andtmp19 = and i64 %shrtmp18, 255
  call void @sad_ll_mem_write8(i64 %addtmp16, i64 %andtmp19)
  %"%msc_data_buf.load20" = load i64, ptr @msc_data_buf, align 8
  %addtmp21 = add i64 %"%msc_data_buf.load20", 5
  %"%lba.load22" = load i64, ptr %lba2, align 8
  %andtmp23 = and i64 %"%lba.load22", 255
  call void @sad_ll_mem_write8(i64 %addtmp21, i64 %andtmp23)
  %"%msc_data_buf.load24" = load i64, ptr @msc_data_buf, align 8
  %addtmp25 = add i64 %"%msc_data_buf.load24", 7
  %"%count.load26" = load i64, ptr %count3, align 8
  %shrtmp27 = lshr i64 %"%count.load26", 8
  %andtmp28 = and i64 %shrtmp27, 255
  call void @sad_ll_mem_write8(i64 %addtmp25, i64 %andtmp28)
  %"%msc_data_buf.load29" = load i64, ptr @msc_data_buf, align 8
  %addtmp30 = add i64 %"%msc_data_buf.load29", 8
  %"%count.load31" = load i64, ptr %count3, align 8
  %andtmp32 = and i64 %"%count.load31", 255
  call void @sad_ll_mem_write8(i64 %addtmp30, i64 %andtmp32)
  %"%dev_idx.load33" = load i64, ptr %dev_idx1, align 8
  %"%msc_data_buf.load34" = load i64, ptr @msc_data_buf, align 8
  %"%src_buf.load" = load i64, ptr %src_buf4, align 8
  %"%xfer_len.load" = load i64, ptr %"%xfer_len", align 8
  %"%DIR_OUT.load" = load i64, ptr @DIR_OUT, align 8
  %calltmp35 = call i64 @msc_bot_transfer(i64 %"%dev_idx.load33", i64 %"%msc_data_buf.load34", i64 10, i64 %"%src_buf.load", i64 %"%xfer_len.load", i64 %"%DIR_OUT.load")
  store i64 %calltmp35, ptr %"%result", align 8
  %"%result.load" = load i64, ptr %"%result", align 8
  %"%CSW_STATUS_PASSED.load" = load i64, ptr @CSW_STATUS_PASSED, align 8
  %cmpeqtmp36 = icmp eq i64 %"%result.load", %"%CSW_STATUS_PASSED.load"
  br i1 %cmpeqtmp36, label %then_269, label %else_271

merge_270:                                        ; preds = %else_271, %then_269
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

then_265:                                         ; preds = %entry
  ret i64 -1

then_267:                                         ; preds = %merge_266
  ret i64 -1

then_269:                                         ; preds = %merge_268
  %"%msc_writes.load" = load i64, ptr @msc_writes, align 8
  %"%count.load37" = load i64, ptr %count3, align 8
  %addtmp38 = add i64 %"%msc_writes.load", %"%count.load37"
  store i64 %addtmp38, ptr %"%msc_writes", align 8
  br label %merge_270
}

define i64 @msc_read_sector(i64 %dev_idx, i64 %lba, i64 %dest_buf) {
entry:
  %dest_buf3 = alloca i64, align 8
  store i64 %dest_buf, ptr %dest_buf3, align 8
  %lba2 = alloca i64, align 8
  store i64 %lba, ptr %lba2, align 8
  %dev_idx1 = alloca i64, align 8
  store i64 %dev_idx, ptr %dev_idx1, align 8
  %"%dev_idx.load" = load i64, ptr %dev_idx1, align 8
  %"%lba.load" = load i64, ptr %lba2, align 8
  %"%dest_buf.load" = load i64, ptr %dest_buf3, align 8
  %calltmp = call i64 @msc_read_sectors(i64 %"%dev_idx.load", i64 %"%lba.load", i64 1, i64 %"%dest_buf.load")
  ret i64 %calltmp
}

define i64 @msc_write_sector(i64 %dev_idx, i64 %lba, i64 %src_buf) {
entry:
  %src_buf3 = alloca i64, align 8
  store i64 %src_buf, ptr %src_buf3, align 8
  %lba2 = alloca i64, align 8
  store i64 %lba, ptr %lba2, align 8
  %dev_idx1 = alloca i64, align 8
  store i64 %dev_idx, ptr %dev_idx1, align 8
  %"%dev_idx.load" = load i64, ptr %dev_idx1, align 8
  %"%lba.load" = load i64, ptr %lba2, align 8
  %"%src_buf.load" = load i64, ptr %src_buf3, align 8
  %calltmp = call i64 @msc_write_sectors(i64 %"%dev_idx.load", i64 %"%lba.load", i64 1, i64 %"%src_buf.load")
  ret i64 %calltmp
}

define i64 @msc_read_mbr(i64 %dev_idx) {
entry:
  %"%part_boot_3" = alloca i64, align 8
  %"%part_size_3" = alloca i64, align 8
  %"%part_start_3" = alloca i64, align 8
  %"%part_type_3" = alloca i64, align 8
  %"%part_boot_2" = alloca i64, align 8
  %"%part_size_2" = alloca i64, align 8
  %"%part_start_2" = alloca i64, align 8
  %"%part_type_2" = alloca i64, align 8
  %"%part_boot_1" = alloca i64, align 8
  %"%part_size_1" = alloca i64, align 8
  %"%part_start_1" = alloca i64, align 8
  %"%part_type_1" = alloca i64, align 8
  %"%part_boot_0" = alloca i64, align 8
  %"%part_size_0" = alloca i64, align 8
  %"%part_start_0" = alloca i64, align 8
  %"%part_type_0" = alloca i64, align 8
  %"%bootable" = alloca i64, align 8
  %"%psize" = alloca i64, align 8
  %"%start_lba" = alloca i64, align 8
  %"%ptype" = alloca i64, align 8
  %"%boot_flag" = alloca i64, align 8
  %"%base" = alloca i64, align 8
  %"%p" = alloca i64, align 8
  %"%part_count" = alloca i64, align 8
  %"%sig_hi" = alloca i64, align 8
  %"%sig_lo" = alloca i64, align 8
  %"%result" = alloca i64, align 8
  %"%mbr_buf" = alloca i64, align 8
  %dev_idx1 = alloca i64, align 8
  store i64 %dev_idx, ptr %dev_idx1, align 8
  call void @sad_ll_kmalloc(i64 512)
  store i64 0, ptr %"%mbr_buf", align 8
  %"%mbr_buf.load" = load i64, ptr %"%mbr_buf", align 8
  call void @sad_ll_memset(i64 %"%mbr_buf.load", i64 0, i64 512)
  %"%dev_idx.load" = load i64, ptr %dev_idx1, align 8
  %"%mbr_buf.load2" = load i64, ptr %"%mbr_buf", align 8
  %calltmp = call i64 @msc_read_sector(i64 %"%dev_idx.load", i64 0, i64 %"%mbr_buf.load2")
  store i64 %calltmp, ptr %"%result", align 8
  %"%result.load" = load i64, ptr %"%result", align 8
  %"%CSW_STATUS_PASSED.load" = load i64, ptr @CSW_STATUS_PASSED, align 8
  %cmpnetmp = icmp ne i64 %"%result.load", %"%CSW_STATUS_PASSED.load"
  br i1 %cmpnetmp, label %then_272, label %merge_273

merge_273:                                        ; preds = %entry
  %"%mbr_buf.load5" = load i64, ptr %"%mbr_buf", align 8
  %addtmp = add i64 %"%mbr_buf.load5", 510
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%sig_lo", align 8
  %"%mbr_buf.load6" = load i64, ptr %"%mbr_buf", align 8
  %addtmp7 = add i64 %"%mbr_buf.load6", 511
  call void @sad_ll_mem_read8(i64 %addtmp7)
  store i64 0, ptr %"%sig_hi", align 8
  %"%sig_lo.load" = load i64, ptr %"%sig_lo", align 8
  %cmpnetmp8 = icmp ne i64 %"%sig_lo.load", 85
  br i1 %cmpnetmp8, label %then_274, label %merge_275

merge_275:                                        ; preds = %merge_273
  %"%sig_hi.load" = load i64, ptr %"%sig_hi", align 8
  %cmpnetmp11 = icmp ne i64 %"%sig_hi.load", 170
  br i1 %cmpnetmp11, label %then_276, label %merge_277

merge_277:                                        ; preds = %merge_275
  %calltmp14 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %calltmp15 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%part_count", align 8
  store i64 0, ptr %"%p", align 8
  br label %while_cond_278

merge_282:                                        ; preds = %then_281, %while_body_279
  %"%p.load51" = load i64, ptr %"%p", align 8
  %cmpeqtmp52 = icmp eq i64 %"%p.load51", 0
  br i1 %cmpeqtmp52, label %then_283, label %merge_284

merge_284:                                        ; preds = %then_283, %merge_282
  %"%p.load53" = load i64, ptr %"%p", align 8
  %cmpeqtmp54 = icmp eq i64 %"%p.load53", 1
  br i1 %cmpeqtmp54, label %then_285, label %merge_286

merge_286:                                        ; preds = %then_285, %merge_284
  %"%p.load55" = load i64, ptr %"%p", align 8
  %cmpeqtmp56 = icmp eq i64 %"%p.load55", 2
  br i1 %cmpeqtmp56, label %then_287, label %merge_288

merge_288:                                        ; preds = %then_287, %merge_286
  %"%p.load57" = load i64, ptr %"%p", align 8
  %cmpeqtmp58 = icmp eq i64 %"%p.load57", 3
  br i1 %cmpeqtmp58, label %then_289, label %merge_290

merge_290:                                        ; preds = %then_289, %merge_288
  %"%ptype.load" = load i64, ptr %"%ptype", align 8
  %cmpnetmp59 = icmp ne i64 %"%ptype.load", 0
  br i1 %cmpnetmp59, label %then_291, label %merge_292

merge_292:                                        ; preds = %merge_294, %merge_290
  %"%p.load70" = load i64, ptr %"%p", align 8
  %addtmp71 = add i64 %"%p.load70", 1
  store i64 %addtmp71, ptr %"%p", align 8
  br label %while_cond_278

merge_294:                                        ; preds = %then_293, %then_291
  %calltmp73 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_292

then_272:                                         ; preds = %entry
  %calltmp3 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %calltmp4 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -1

then_274:                                         ; preds = %merge_273
  %calltmp9 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %calltmp10 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -2

then_276:                                         ; preds = %merge_275
  %calltmp12 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %calltmp13 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -2

then_281:                                         ; preds = %while_body_279
  store i64 1, ptr %"%bootable", align 8
  br label %merge_282

then_283:                                         ; preds = %merge_282
  %"%1246.load" = load i64, ptr %"%ptype", align 8
  store i64 %"%1246.load", ptr %"%part_type_0", align 8
  %"%1248.load" = load i64, ptr %"%start_lba", align 8
  store i64 %"%1248.load", ptr %"%part_start_0", align 8
  %"%1250.load" = load i64, ptr %"%psize", align 8
  store i64 %"%1250.load", ptr %"%part_size_0", align 8
  %"%1252.load" = load i64, ptr %"%bootable", align 8
  store i64 %"%1252.load", ptr %"%part_boot_0", align 8
  br label %merge_284

then_285:                                         ; preds = %merge_284
  %"%1256.load" = load i64, ptr %"%ptype", align 8
  store i64 %"%1256.load", ptr %"%part_type_1", align 8
  %"%1258.load" = load i64, ptr %"%start_lba", align 8
  store i64 %"%1258.load", ptr %"%part_start_1", align 8
  %"%1260.load" = load i64, ptr %"%psize", align 8
  store i64 %"%1260.load", ptr %"%part_size_1", align 8
  %"%1262.load" = load i64, ptr %"%bootable", align 8
  store i64 %"%1262.load", ptr %"%part_boot_1", align 8
  br label %merge_286

then_287:                                         ; preds = %merge_286
  %"%1266.load" = load i64, ptr %"%ptype", align 8
  store i64 %"%1266.load", ptr %"%part_type_2", align 8
  %"%1268.load" = load i64, ptr %"%start_lba", align 8
  store i64 %"%1268.load", ptr %"%part_start_2", align 8
  %"%1270.load" = load i64, ptr %"%psize", align 8
  store i64 %"%1270.load", ptr %"%part_size_2", align 8
  %"%1272.load" = load i64, ptr %"%bootable", align 8
  store i64 %"%1272.load", ptr %"%part_boot_2", align 8
  br label %merge_288

then_289:                                         ; preds = %merge_288
  %"%1276.load" = load i64, ptr %"%ptype", align 8
  store i64 %"%1276.load", ptr %"%part_type_3", align 8
  %"%1278.load" = load i64, ptr %"%start_lba", align 8
  store i64 %"%1278.load", ptr %"%part_start_3", align 8
  %"%1280.load" = load i64, ptr %"%psize", align 8
  store i64 %"%1280.load", ptr %"%part_size_3", align 8
  %"%1282.load" = load i64, ptr %"%bootable", align 8
  store i64 %"%1282.load", ptr %"%part_boot_3", align 8
  br label %merge_290

then_291:                                         ; preds = %merge_290
  %"%part_count.load" = load i64, ptr %"%part_count", align 8
  %addtmp60 = add i64 %"%part_count.load", 1
  store i64 %addtmp60, ptr %"%part_count", align 8
  %calltmp61 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %"%1290.load" = load i64, ptr %"%p", align 8
  %calltmp62 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %"%1290.load")
  %calltmp63 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %"%1295.load" = load i64, ptr %"%ptype", align 8
  %calltmp64 = call i64 @msc_serial_hex8(i64 %"%1295.load")
  %calltmp65 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  %"%1300.load" = load i64, ptr %"%start_lba", align 8
  %calltmp66 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %"%1300.load")
  %calltmp67 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%1305.load" = load i64, ptr %"%psize", align 8
  %calltmp68 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %"%1305.load")
  %"%bootable.load" = load i64, ptr %"%bootable", align 8
  %cmpeqtmp69 = icmp eq i64 %"%bootable.load", 1
  br i1 %cmpeqtmp69, label %then_293, label %merge_294

then_293:                                         ; preds = %then_291
  %calltmp72 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  br label %merge_294

while_body_279:                                   ; preds = %while_cond_278
  %"%mbr_buf.load16" = load i64, ptr %"%mbr_buf", align 8
  %addtmp17 = add i64 %"%mbr_buf.load16", 446
  %"%p.load18" = load i64, ptr %"%p", align 8
  %shltmp = shl i64 %"%p.load18", 4
  %addtmp19 = add i64 %addtmp17, %shltmp
  store i64 %addtmp19, ptr %"%base", align 8
  %"%base.load" = load i64, ptr %"%base", align 8
  call void @sad_ll_mem_read8(i64 %"%base.load")
  store i64 0, ptr %"%boot_flag", align 8
  %"%base.load20" = load i64, ptr %"%base", align 8
  %addtmp21 = add i64 %"%base.load20", 4
  call void @sad_ll_mem_read8(i64 %addtmp21)
  store i64 0, ptr %"%ptype", align 8
  %"%base.load22" = load i64, ptr %"%base", align 8
  %addtmp23 = add i64 %"%base.load22", 8
  call void @sad_ll_mem_read8(i64 %addtmp23)
  store i64 0, ptr %"%start_lba", align 8
  %"%base.load24" = load i64, ptr %"%base", align 8
  %addtmp25 = add i64 %"%base.load24", 9
  call void @sad_ll_mem_read8(i64 %addtmp25)
  %"%start_lba.load" = load i64, ptr %"%start_lba", align 8
  %addtmp26 = add i64 %"%start_lba.load", 0
  store i64 %addtmp26, ptr %"%start_lba", align 8
  %"%base.load27" = load i64, ptr %"%base", align 8
  %addtmp28 = add i64 %"%base.load27", 10
  call void @sad_ll_mem_read8(i64 %addtmp28)
  %"%start_lba.load29" = load i64, ptr %"%start_lba", align 8
  %addtmp30 = add i64 %"%start_lba.load29", 0
  store i64 %addtmp30, ptr %"%start_lba", align 8
  %"%base.load31" = load i64, ptr %"%base", align 8
  %addtmp32 = add i64 %"%base.load31", 11
  call void @sad_ll_mem_read8(i64 %addtmp32)
  %"%start_lba.load33" = load i64, ptr %"%start_lba", align 8
  %addtmp34 = add i64 %"%start_lba.load33", 0
  store i64 %addtmp34, ptr %"%start_lba", align 8
  %"%base.load35" = load i64, ptr %"%base", align 8
  %addtmp36 = add i64 %"%base.load35", 12
  call void @sad_ll_mem_read8(i64 %addtmp36)
  store i64 0, ptr %"%psize", align 8
  %"%base.load37" = load i64, ptr %"%base", align 8
  %addtmp38 = add i64 %"%base.load37", 13
  call void @sad_ll_mem_read8(i64 %addtmp38)
  %"%psize.load" = load i64, ptr %"%psize", align 8
  %addtmp39 = add i64 %"%psize.load", 0
  store i64 %addtmp39, ptr %"%psize", align 8
  %"%base.load40" = load i64, ptr %"%base", align 8
  %addtmp41 = add i64 %"%base.load40", 14
  call void @sad_ll_mem_read8(i64 %addtmp41)
  %"%psize.load42" = load i64, ptr %"%psize", align 8
  %addtmp43 = add i64 %"%psize.load42", 0
  store i64 %addtmp43, ptr %"%psize", align 8
  %"%base.load44" = load i64, ptr %"%base", align 8
  %addtmp45 = add i64 %"%base.load44", 15
  call void @sad_ll_mem_read8(i64 %addtmp45)
  %"%psize.load46" = load i64, ptr %"%psize", align 8
  %addtmp47 = add i64 %"%psize.load46", 0
  store i64 %addtmp47, ptr %"%psize", align 8
  store i64 0, ptr %"%bootable", align 8
  %"%boot_flag.load" = load i64, ptr %"%boot_flag", align 8
  %cmpeqtmp = icmp eq i64 %"%boot_flag.load", 128
  br i1 %cmpeqtmp, label %then_281, label %merge_282

while_cond_278:                                   ; preds = %merge_292, %merge_277
  %"%p.load" = load i64, ptr %"%p", align 8
  %cmplttmp = icmp slt i64 %"%p.load", 4
  br i1 %cmplttmp, label %while_body_279, label %while_exit_280

while_exit_280:                                   ; preds = %while_cond_278
  %calltmp48 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %"%1317.load" = load i64, ptr %"%part_count", align 8
  %calltmp49 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %"%1317.load")
  %calltmp50 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr %"%part_count", align 8
  ret i64 %loadtmp
}

define i64 @msc_get_part_type(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_295, label %merge_296

merge_296:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_297, label %merge_298

merge_298:                                        ; preds = %merge_296
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_299, label %merge_300

merge_300:                                        ; preds = %merge_298
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_301, label %merge_302

merge_302:                                        ; preds = %merge_300
  ret i64 0

then_295:                                         ; preds = %entry
  %loadtmp = load i64, ptr @part_type_0, align 8
  ret i64 %loadtmp

then_297:                                         ; preds = %merge_296
  %loadtmp4 = load i64, ptr @part_type_1, align 8
  ret i64 %loadtmp4

then_299:                                         ; preds = %merge_298
  %loadtmp7 = load i64, ptr @part_type_2, align 8
  ret i64 %loadtmp7

then_301:                                         ; preds = %merge_300
  %loadtmp10 = load i64, ptr @part_type_3, align 8
  ret i64 %loadtmp10
}

define i64 @msc_get_part_start(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_303, label %merge_304

merge_304:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_305, label %merge_306

merge_306:                                        ; preds = %merge_304
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_307, label %merge_308

merge_308:                                        ; preds = %merge_306
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_309, label %merge_310

merge_310:                                        ; preds = %merge_308
  ret i64 0

then_303:                                         ; preds = %entry
  %loadtmp = load i64, ptr @part_start_0, align 8
  ret i64 %loadtmp

then_305:                                         ; preds = %merge_304
  %loadtmp4 = load i64, ptr @part_start_1, align 8
  ret i64 %loadtmp4

then_307:                                         ; preds = %merge_306
  %loadtmp7 = load i64, ptr @part_start_2, align 8
  ret i64 %loadtmp7

then_309:                                         ; preds = %merge_308
  %loadtmp10 = load i64, ptr @part_start_3, align 8
  ret i64 %loadtmp10
}

define i64 @msc_get_part_size(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_311, label %merge_312

merge_312:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_313, label %merge_314

merge_314:                                        ; preds = %merge_312
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_315, label %merge_316

merge_316:                                        ; preds = %merge_314
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_317, label %merge_318

merge_318:                                        ; preds = %merge_316
  ret i64 0

then_311:                                         ; preds = %entry
  %loadtmp = load i64, ptr @part_size_0, align 8
  ret i64 %loadtmp

then_313:                                         ; preds = %merge_312
  %loadtmp4 = load i64, ptr @part_size_1, align 8
  ret i64 %loadtmp4

then_315:                                         ; preds = %merge_314
  %loadtmp7 = load i64, ptr @part_size_2, align 8
  ret i64 %loadtmp7

then_317:                                         ; preds = %merge_316
  %loadtmp10 = load i64, ptr @part_size_3, align 8
  ret i64 %loadtmp10
}

define i64 @msc_get_part_bootable(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_319, label %merge_320

merge_320:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_321, label %merge_322

merge_322:                                        ; preds = %merge_320
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_323, label %merge_324

merge_324:                                        ; preds = %merge_322
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_325, label %merge_326

merge_326:                                        ; preds = %merge_324
  ret i64 0

then_319:                                         ; preds = %entry
  %loadtmp = load i64, ptr @part_boot_0, align 8
  ret i64 %loadtmp

then_321:                                         ; preds = %merge_320
  %loadtmp4 = load i64, ptr @part_boot_1, align 8
  ret i64 %loadtmp4

then_323:                                         ; preds = %merge_322
  %loadtmp7 = load i64, ptr @part_boot_2, align 8
  ret i64 %loadtmp7

then_325:                                         ; preds = %merge_324
  %loadtmp10 = load i64, ptr @part_boot_3, align 8
  ret i64 %loadtmp10
}

define i64 @msc_get_part_count() {
entry:
  %loadtmp = load i64, ptr @part_count, align 8
  ret i64 %loadtmp
}

define i64 @msc_register_device(i64 %hc_idx, i64 %dev_addr) {
entry:
  %"%msc_count" = alloca i64, align 8
  %"%inq_buf" = alloca i64, align 8
  %"%tur" = alloca i64, align 8
  %"%ready" = alloca i64, align 8
  %"%tries" = alloca i64, align 8
  %"%idx" = alloca i64, align 8
  %dev_addr2 = alloca i64, align 8
  store i64 %dev_addr, ptr %dev_addr2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%msc_count.load" = load i64, ptr @msc_count, align 8
  %"%MSC_MAX_DEVICES.load" = load i64, ptr @MSC_MAX_DEVICES, align 8
  %cmpgetmp = icmp sge i64 %"%msc_count.load", %"%MSC_MAX_DEVICES.load"
  br i1 %cmpgetmp, label %then_327, label %merge_328

merge_328:                                        ; preds = %entry
  %"%1419.load" = load i64, ptr @msc_count, align 8
  store i64 %"%1419.load", ptr %"%idx", align 8
  %"%idx.load" = load i64, ptr %"%idx", align 8
  %calltmp4 = call i64 @msc_set_active(i64 %"%idx.load", i64 1)
  %"%idx.load5" = load i64, ptr %"%idx", align 8
  %"%hc_idx.load" = load i64, ptr %hc_idx1, align 8
  %calltmp6 = call i64 @msc_set_hc(i64 %"%idx.load5", i64 %"%hc_idx.load")
  %"%idx.load7" = load i64, ptr %"%idx", align 8
  %"%dev_addr.load" = load i64, ptr %dev_addr2, align 8
  %calltmp8 = call i64 @msc_set_addr(i64 %"%idx.load7", i64 %"%dev_addr.load")
  %calltmp9 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %"%1427.load" = load i64, ptr %"%idx", align 8
  %calltmp10 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %"%1427.load")
  %calltmp11 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  %"%1432.load" = load i64, ptr %dev_addr2, align 8
  %calltmp12 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %"%1432.load")
  %calltmp13 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%tries", align 8
  store i64 0, ptr %"%ready", align 8
  br label %while_cond_329

merge_333:                                        ; preds = %then_332, %while_body_330
  %"%tries.load17" = load i64, ptr %"%tries", align 8
  %addtmp = add i64 %"%tries.load17", 1
  store i64 %addtmp, ptr %"%tries", align 8
  br label %while_cond_329

merge_335:                                        ; preds = %then_334, %while_exit_331
  call void @sad_ll_kmalloc(i64 36)
  store i64 0, ptr %"%inq_buf", align 8
  %"%inq_buf.load" = load i64, ptr %"%inq_buf", align 8
  call void @sad_ll_memset(i64 %"%inq_buf.load", i64 0, i64 36)
  %"%idx.load20" = load i64, ptr %"%idx", align 8
  %"%inq_buf.load21" = load i64, ptr %"%inq_buf", align 8
  %calltmp22 = call i64 @msc_inquiry(i64 %"%idx.load20", i64 %"%inq_buf.load21")
  %calltmp23 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  %"%idx.load24" = load i64, ptr %"%idx", align 8
  %calltmp25 = call i64 @msc_get_type(i64 %"%idx.load24")
  %calltmp26 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp25)
  %calltmp27 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  %"%idx.load28" = load i64, ptr %"%idx", align 8
  %calltmp29 = call i64 @msc_read_capacity(i64 %"%idx.load28")
  %"%msc_count.load30" = load i64, ptr @msc_count, align 8
  %addtmp31 = add i64 %"%msc_count.load30", 1
  store i64 %addtmp31, ptr %"%msc_count", align 8
  %loadtmp = load i64, ptr %"%idx", align 8
  ret i64 %loadtmp

then_327:                                         ; preds = %entry
  %calltmp = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %calltmp3 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -1

then_332:                                         ; preds = %while_body_330
  store i64 1, ptr %"%ready", align 8
  br label %merge_333

then_334:                                         ; preds = %while_exit_331
  %calltmp18 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  %calltmp19 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_335

while_body_330:                                   ; preds = %while_cond_329
  %"%idx.load14" = load i64, ptr %"%idx", align 8
  %calltmp15 = call i64 @msc_test_unit_ready(i64 %"%idx.load14")
  store i64 %calltmp15, ptr %"%tur", align 8
  %"%tur.load" = load i64, ptr %"%tur", align 8
  %"%CSW_STATUS_PASSED.load" = load i64, ptr @CSW_STATUS_PASSED, align 8
  %cmpeqtmp = icmp eq i64 %"%tur.load", %"%CSW_STATUS_PASSED.load"
  br i1 %cmpeqtmp, label %then_332, label %merge_333

while_cond_329:                                   ; preds = %merge_333, %merge_328
  %"%tries.load" = load i64, ptr %"%tries", align 8
  %"%MAX_RETRIES.load" = load i64, ptr @MAX_RETRIES, align 8
  %cmplttmp = icmp slt i64 %"%tries.load", %"%MAX_RETRIES.load"
  br i1 %cmplttmp, label %while_body_330, label %while_exit_331

while_exit_331:                                   ; preds = %while_cond_329
  %"%ready.load" = load i64, ptr %"%ready", align 8
  %cmpeqtmp16 = icmp eq i64 %"%ready.load", 0
  br i1 %cmpeqtmp16, label %then_334, label %merge_335
}

define i64 @"msc_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %"%d" = alloca i64, align 8
  %calltmp = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  %calltmp1 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  %"%1481.load" = load i64, ptr @msc_count, align 8
  %calltmp3 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %"%1481.load")
  %calltmp4 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%d", align 8
  br label %while_cond_336

merge_340:                                        ; preds = %then_339, %while_body_337
  %"%d.load33" = load i64, ptr %"%d", align 8
  %addtmp = add i64 %"%d.load33", 1
  store i64 %addtmp, ptr %"%d", align 8
  br label %while_cond_336

merge_342:                                        ; preds = %then_341, %while_exit_338
  %calltmp37 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  %calltmp38 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_339:                                         ; preds = %while_body_337
  %calltmp14 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  %"%1494.load" = load i64, ptr %"%d", align 8
  %calltmp15 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %"%1494.load")
  %calltmp16 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  %"%d.load17" = load i64, ptr %"%d", align 8
  %calltmp18 = call i64 @msc_get_addr(i64 %"%d.load17")
  %calltmp19 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp18)
  %calltmp20 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  %"%d.load21" = load i64, ptr %"%d", align 8
  %calltmp22 = call i64 @msc_get_type(i64 %"%d.load21")
  %calltmp23 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp22)
  %calltmp24 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.30 to i64))
  %"%d.load25" = load i64, ptr %"%d", align 8
  %calltmp26 = call i64 @msc_get_sectors(i64 %"%d.load25")
  %calltmp27 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp26)
  %calltmp28 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  %"%d.load29" = load i64, ptr %"%d", align 8
  %calltmp30 = call i64 @msc_get_sector_size(i64 %"%d.load29")
  %calltmp31 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp30)
  %calltmp32 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_340

then_341:                                         ; preds = %while_exit_338
  %calltmp34 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.32 to i64))
  %"%1544.load" = load i64, ptr @part_count, align 8
  %calltmp35 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %"%1544.load")
  %calltmp36 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_342

while_body_337:                                   ; preds = %while_cond_336
  %"%d.load5" = load i64, ptr %"%d", align 8
  %calltmp6 = call i64 @msc_get_active(i64 %"%d.load5")
  %cmpeqtmp = icmp eq i64 %calltmp6, 1
  br i1 %cmpeqtmp, label %then_339, label %merge_340

while_cond_336:                                   ; preds = %merge_340, %entry
  %"%d.load" = load i64, ptr %"%d", align 8
  %"%msc_count.load" = load i64, ptr @msc_count, align 8
  %cmplttmp = icmp slt i64 %"%d.load", %"%msc_count.load"
  br i1 %cmplttmp, label %while_body_337, label %while_exit_338

while_exit_338:                                   ; preds = %while_cond_336
  %calltmp7 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %"%1526.load" = load i64, ptr @msc_reads, align 8
  %calltmp8 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %"%1526.load")
  %calltmp9 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  %"%1531.load" = load i64, ptr @msc_writes, align 8
  %calltmp10 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %"%1531.load")
  %calltmp11 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  %"%1536.load" = load i64, ptr @msc_errors, align 8
  %calltmp12 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %"%1536.load")
  %calltmp13 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  %"%part_count.load" = load i64, ptr @part_count, align 8
  %cmpgttmp = icmp sgt i64 %"%part_count.load", 0
  br i1 %cmpgttmp, label %then_341, label %merge_342
}

define i64 @"msc_\D9\87\D9\8A\D8\A6"() {
entry:
  %"%storage_idx" = alloca i64, align 8
  %"%part_count" = alloca i64, align 8
  %"%msc_errors" = alloca i64, align 8
  %"%msc_writes" = alloca i64, align 8
  %"%msc_reads" = alloca i64, align 8
  %"%msc_tag_counter" = alloca i64, align 8
  %"%msc_count" = alloca i64, align 8
  %"%msc_data_buf" = alloca i64, align 8
  %"%msc_csw_buf" = alloca i64, align 8
  %"%msc_cbw_buf" = alloca i64, align 8
  %calltmp = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.34 to i64))
  %calltmp1 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  call void @sad_ll_kmalloc(i64 64)
  store i64 0, ptr %"%msc_cbw_buf", align 8
  call void @sad_ll_kmalloc(i64 16)
  store i64 0, ptr %"%msc_csw_buf", align 8
  call void @sad_ll_kmalloc(i64 1024)
  store i64 0, ptr %"%msc_data_buf", align 8
  %"%msc_cbw_buf.load" = load i64, ptr %"%msc_cbw_buf", align 8
  call void @sad_ll_memset(i64 %"%msc_cbw_buf.load", i64 0, i64 64)
  %"%msc_csw_buf.load" = load i64, ptr %"%msc_csw_buf", align 8
  call void @sad_ll_memset(i64 %"%msc_csw_buf.load", i64 0, i64 16)
  %"%msc_data_buf.load" = load i64, ptr %"%msc_data_buf", align 8
  call void @sad_ll_memset(i64 %"%msc_data_buf.load", i64 0, i64 1024)
  store i64 0, ptr %"%msc_count", align 8
  store i64 1, ptr %"%msc_tag_counter", align 8
  store i64 0, ptr %"%msc_reads", align 8
  store i64 0, ptr %"%msc_writes", align 8
  store i64 0, ptr %"%msc_errors", align 8
  store i64 0, ptr %"%part_count", align 8
  call void @usb_find_storage()
  store i64 0, ptr %"%storage_idx", align 8
  %"%storage_idx.load" = load i64, ptr %"%storage_idx", align 8
  %cmpgetmp = icmp sge i64 %"%storage_idx.load", 0
  br i1 %cmpgetmp, label %then_343, label %else_345

else_345:                                         ; preds = %entry
  %calltmp7 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.36 to i64))
  %calltmp8 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_344

merge_344:                                        ; preds = %else_345, %then_343
  %calltmp9 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.37 to i64))
  %"%1602.load" = load i64, ptr %"%msc_count", align 8
  %calltmp10 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %"%1602.load")
  %calltmp11 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr %"%msc_count", align 8
  ret i64 %loadtmp

then_343:                                         ; preds = %entry
  %calltmp2 = call i64 @"msc_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.35 to i64))
  %"%1589.load" = load i64, ptr %"%storage_idx", align 8
  %calltmp3 = call i64 @"msc_serial_\D8\B1\D9\82\D9\85"(i64 %"%1589.load")
  %calltmp4 = call i64 @"msc_serial_\D8\B3\D8\B7\D8\B1"()
  %"%storage_idx.load5" = load i64, ptr %"%storage_idx", align 8
  %addtmp = add i64 %"%storage_idx.load5", 1
  %calltmp6 = call i64 @msc_register_device(i64 0, i64 %addtmp)
  br label %merge_344
}

define i64 @"msc_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  ret i64 0
}

define void @__sad_main() {
entry:
  %"%part_count" = alloca i64, align 8
  %"%part_boot_3" = alloca i64, align 8
  %"%part_boot_2" = alloca i64, align 8
  %"%part_boot_1" = alloca i64, align 8
  %"%part_boot_0" = alloca i64, align 8
  %"%part_size_3" = alloca i64, align 8
  %"%part_size_2" = alloca i64, align 8
  %"%part_size_1" = alloca i64, align 8
  %"%part_size_0" = alloca i64, align 8
  %"%part_start_3" = alloca i64, align 8
  %"%part_start_2" = alloca i64, align 8
  %"%part_start_1" = alloca i64, align 8
  %"%part_start_0" = alloca i64, align 8
  %"%part_type_3" = alloca i64, align 8
  %"%part_type_2" = alloca i64, align 8
  %"%part_type_1" = alloca i64, align 8
  %"%part_type_0" = alloca i64, align 8
  %"%msc_errors" = alloca i64, align 8
  %"%msc_writes" = alloca i64, align 8
  %"%msc_reads" = alloca i64, align 8
  %"%msc_data_buf" = alloca i64, align 8
  %"%msc_csw_buf" = alloca i64, align 8
  %"%msc_cbw_buf" = alloca i64, align 8
  %"%msc_tag_counter" = alloca i64, align 8
  %"%msc_count" = alloca i64, align 8
  %"%msc_type_7" = alloca i64, align 8
  %"%msc_type_6" = alloca i64, align 8
  %"%msc_type_5" = alloca i64, align 8
  %"%msc_type_4" = alloca i64, align 8
  %"%msc_type_3" = alloca i64, align 8
  %"%msc_type_2" = alloca i64, align 8
  %"%msc_type_1" = alloca i64, align 8
  %"%msc_type_0" = alloca i64, align 8
  %"%msc_lun_7" = alloca i64, align 8
  %"%msc_lun_6" = alloca i64, align 8
  %"%msc_lun_5" = alloca i64, align 8
  %"%msc_lun_4" = alloca i64, align 8
  %"%msc_lun_3" = alloca i64, align 8
  %"%msc_lun_2" = alloca i64, align 8
  %"%msc_lun_1" = alloca i64, align 8
  %"%msc_lun_0" = alloca i64, align 8
  %"%msc_sector_size_7" = alloca i64, align 8
  %"%msc_sector_size_6" = alloca i64, align 8
  %"%msc_sector_size_5" = alloca i64, align 8
  %"%msc_sector_size_4" = alloca i64, align 8
  %"%msc_sector_size_3" = alloca i64, align 8
  %"%msc_sector_size_2" = alloca i64, align 8
  %"%msc_sector_size_1" = alloca i64, align 8
  %"%msc_sector_size_0" = alloca i64, align 8
  %"%msc_sectors_7" = alloca i64, align 8
  %"%msc_sectors_6" = alloca i64, align 8
  %"%msc_sectors_5" = alloca i64, align 8
  %"%msc_sectors_4" = alloca i64, align 8
  %"%msc_sectors_3" = alloca i64, align 8
  %"%msc_sectors_2" = alloca i64, align 8
  %"%msc_sectors_1" = alloca i64, align 8
  %"%msc_sectors_0" = alloca i64, align 8
  %"%msc_ep_out_7" = alloca i64, align 8
  %"%msc_ep_out_6" = alloca i64, align 8
  %"%msc_ep_out_5" = alloca i64, align 8
  %"%msc_ep_out_4" = alloca i64, align 8
  %"%msc_ep_out_3" = alloca i64, align 8
  %"%msc_ep_out_2" = alloca i64, align 8
  %"%msc_ep_out_1" = alloca i64, align 8
  %"%msc_ep_out_0" = alloca i64, align 8
  %"%msc_ep_in_7" = alloca i64, align 8
  %"%msc_ep_in_6" = alloca i64, align 8
  %"%msc_ep_in_5" = alloca i64, align 8
  %"%msc_ep_in_4" = alloca i64, align 8
  %"%msc_ep_in_3" = alloca i64, align 8
  %"%msc_ep_in_2" = alloca i64, align 8
  %"%msc_ep_in_1" = alloca i64, align 8
  %"%msc_ep_in_0" = alloca i64, align 8
  %"%msc_dev_addr_7" = alloca i64, align 8
  %"%msc_dev_addr_6" = alloca i64, align 8
  %"%msc_dev_addr_5" = alloca i64, align 8
  %"%msc_dev_addr_4" = alloca i64, align 8
  %"%msc_dev_addr_3" = alloca i64, align 8
  %"%msc_dev_addr_2" = alloca i64, align 8
  %"%msc_dev_addr_1" = alloca i64, align 8
  %"%msc_dev_addr_0" = alloca i64, align 8
  %"%msc_hc_idx_7" = alloca i64, align 8
  %"%msc_hc_idx_6" = alloca i64, align 8
  %"%msc_hc_idx_5" = alloca i64, align 8
  %"%msc_hc_idx_4" = alloca i64, align 8
  %"%msc_hc_idx_3" = alloca i64, align 8
  %"%msc_hc_idx_2" = alloca i64, align 8
  %"%msc_hc_idx_1" = alloca i64, align 8
  %"%msc_hc_idx_0" = alloca i64, align 8
  %"%msc_active_7" = alloca i64, align 8
  %"%msc_active_6" = alloca i64, align 8
  %"%msc_active_5" = alloca i64, align 8
  %"%msc_active_4" = alloca i64, align 8
  %"%msc_active_3" = alloca i64, align 8
  %"%msc_active_2" = alloca i64, align 8
  %"%msc_active_1" = alloca i64, align 8
  %"%msc_active_0" = alloca i64, align 8
  %"%BULK_EP_OUT" = alloca i64, align 8
  %"%BULK_EP_IN" = alloca i64, align 8
  %"%DIR_IN" = alloca i64, align 8
  %"%DIR_OUT" = alloca i64, align 8
  %"%MAX_RETRIES" = alloca i64, align 8
  %"%SECTOR_SIZE" = alloca i64, align 8
  %"%SCSI_MODE_SENSE_6" = alloca i64, align 8
  %"%SCSI_WRITE_10" = alloca i64, align 8
  %"%SCSI_READ_10" = alloca i64, align 8
  %"%SCSI_READ_CAPACITY" = alloca i64, align 8
  %"%SCSI_INQUIRY" = alloca i64, align 8
  %"%SCSI_REQUEST_SENSE" = alloca i64, align 8
  %"%SCSI_TEST_UNIT_READY" = alloca i64, align 8
  %"%CSW_STATUS_PHASE_ERROR" = alloca i64, align 8
  %"%CSW_STATUS_FAILED" = alloca i64, align 8
  %"%CSW_STATUS_PASSED" = alloca i64, align 8
  %"%CSW_SIZE" = alloca i64, align 8
  %"%CSW_SIGNATURE" = alloca i64, align 8
  %"%CBW_SIZE" = alloca i64, align 8
  %"%CBW_SIGNATURE" = alloca i64, align 8
  %"%MSC_MAX_DEVICES" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 8, ptr %"%MSC_MAX_DEVICES", align 8
  store i64 1128420181, ptr %"%CBW_SIGNATURE", align 8
  store i64 31, ptr %"%CBW_SIZE", align 8
  store i64 1396855637, ptr %"%CSW_SIGNATURE", align 8
  store i64 13, ptr %"%CSW_SIZE", align 8
  store i64 0, ptr %"%CSW_STATUS_PASSED", align 8
  store i64 1, ptr %"%CSW_STATUS_FAILED", align 8
  store i64 2, ptr %"%CSW_STATUS_PHASE_ERROR", align 8
  store i64 0, ptr %"%SCSI_TEST_UNIT_READY", align 8
  store i64 3, ptr %"%SCSI_REQUEST_SENSE", align 8
  store i64 18, ptr %"%SCSI_INQUIRY", align 8
  store i64 37, ptr %"%SCSI_READ_CAPACITY", align 8
  store i64 40, ptr %"%SCSI_READ_10", align 8
  store i64 42, ptr %"%SCSI_WRITE_10", align 8
  store i64 26, ptr %"%SCSI_MODE_SENSE_6", align 8
  store i64 512, ptr %"%SECTOR_SIZE", align 8
  store i64 3, ptr %"%MAX_RETRIES", align 8
  store i64 0, ptr %"%DIR_OUT", align 8
  store i64 128, ptr %"%DIR_IN", align 8
  store i64 129, ptr %"%BULK_EP_IN", align 8
  store i64 2, ptr %"%BULK_EP_OUT", align 8
  store i64 0, ptr %"%msc_active_0", align 8
  store i64 0, ptr %"%msc_active_1", align 8
  store i64 0, ptr %"%msc_active_2", align 8
  store i64 0, ptr %"%msc_active_3", align 8
  store i64 0, ptr %"%msc_active_4", align 8
  store i64 0, ptr %"%msc_active_5", align 8
  store i64 0, ptr %"%msc_active_6", align 8
  store i64 0, ptr %"%msc_active_7", align 8
  store i64 0, ptr %"%msc_hc_idx_0", align 8
  store i64 0, ptr %"%msc_hc_idx_1", align 8
  store i64 0, ptr %"%msc_hc_idx_2", align 8
  store i64 0, ptr %"%msc_hc_idx_3", align 8
  store i64 0, ptr %"%msc_hc_idx_4", align 8
  store i64 0, ptr %"%msc_hc_idx_5", align 8
  store i64 0, ptr %"%msc_hc_idx_6", align 8
  store i64 0, ptr %"%msc_hc_idx_7", align 8
  store i64 0, ptr %"%msc_dev_addr_0", align 8
  store i64 0, ptr %"%msc_dev_addr_1", align 8
  store i64 0, ptr %"%msc_dev_addr_2", align 8
  store i64 0, ptr %"%msc_dev_addr_3", align 8
  store i64 0, ptr %"%msc_dev_addr_4", align 8
  store i64 0, ptr %"%msc_dev_addr_5", align 8
  store i64 0, ptr %"%msc_dev_addr_6", align 8
  store i64 0, ptr %"%msc_dev_addr_7", align 8
  store i64 129, ptr %"%msc_ep_in_0", align 8
  store i64 129, ptr %"%msc_ep_in_1", align 8
  store i64 129, ptr %"%msc_ep_in_2", align 8
  store i64 129, ptr %"%msc_ep_in_3", align 8
  store i64 129, ptr %"%msc_ep_in_4", align 8
  store i64 129, ptr %"%msc_ep_in_5", align 8
  store i64 129, ptr %"%msc_ep_in_6", align 8
  store i64 129, ptr %"%msc_ep_in_7", align 8
  store i64 2, ptr %"%msc_ep_out_0", align 8
  store i64 2, ptr %"%msc_ep_out_1", align 8
  store i64 2, ptr %"%msc_ep_out_2", align 8
  store i64 2, ptr %"%msc_ep_out_3", align 8
  store i64 2, ptr %"%msc_ep_out_4", align 8
  store i64 2, ptr %"%msc_ep_out_5", align 8
  store i64 2, ptr %"%msc_ep_out_6", align 8
  store i64 2, ptr %"%msc_ep_out_7", align 8
  store i64 0, ptr %"%msc_sectors_0", align 8
  store i64 0, ptr %"%msc_sectors_1", align 8
  store i64 0, ptr %"%msc_sectors_2", align 8
  store i64 0, ptr %"%msc_sectors_3", align 8
  store i64 0, ptr %"%msc_sectors_4", align 8
  store i64 0, ptr %"%msc_sectors_5", align 8
  store i64 0, ptr %"%msc_sectors_6", align 8
  store i64 0, ptr %"%msc_sectors_7", align 8
  store i64 512, ptr %"%msc_sector_size_0", align 8
  store i64 512, ptr %"%msc_sector_size_1", align 8
  store i64 512, ptr %"%msc_sector_size_2", align 8
  store i64 512, ptr %"%msc_sector_size_3", align 8
  store i64 512, ptr %"%msc_sector_size_4", align 8
  store i64 512, ptr %"%msc_sector_size_5", align 8
  store i64 512, ptr %"%msc_sector_size_6", align 8
  store i64 512, ptr %"%msc_sector_size_7", align 8
  store i64 0, ptr %"%msc_lun_0", align 8
  store i64 0, ptr %"%msc_lun_1", align 8
  store i64 0, ptr %"%msc_lun_2", align 8
  store i64 0, ptr %"%msc_lun_3", align 8
  store i64 0, ptr %"%msc_lun_4", align 8
  store i64 0, ptr %"%msc_lun_5", align 8
  store i64 0, ptr %"%msc_lun_6", align 8
  store i64 0, ptr %"%msc_lun_7", align 8
  store i64 0, ptr %"%msc_type_0", align 8
  store i64 0, ptr %"%msc_type_1", align 8
  store i64 0, ptr %"%msc_type_2", align 8
  store i64 0, ptr %"%msc_type_3", align 8
  store i64 0, ptr %"%msc_type_4", align 8
  store i64 0, ptr %"%msc_type_5", align 8
  store i64 0, ptr %"%msc_type_6", align 8
  store i64 0, ptr %"%msc_type_7", align 8
  store i64 0, ptr %"%msc_count", align 8
  store i64 1, ptr %"%msc_tag_counter", align 8
  store i64 0, ptr %"%msc_cbw_buf", align 8
  store i64 0, ptr %"%msc_csw_buf", align 8
  store i64 0, ptr %"%msc_data_buf", align 8
  store i64 0, ptr %"%msc_reads", align 8
  store i64 0, ptr %"%msc_writes", align 8
  store i64 0, ptr %"%msc_errors", align 8
  store i64 0, ptr %"%part_type_0", align 8
  store i64 0, ptr %"%part_type_1", align 8
  store i64 0, ptr %"%part_type_2", align 8
  store i64 0, ptr %"%part_type_3", align 8
  store i64 0, ptr %"%part_start_0", align 8
  store i64 0, ptr %"%part_start_1", align 8
  store i64 0, ptr %"%part_start_2", align 8
  store i64 0, ptr %"%part_start_3", align 8
  store i64 0, ptr %"%part_size_0", align 8
  store i64 0, ptr %"%part_size_1", align 8
  store i64 0, ptr %"%part_size_2", align 8
  store i64 0, ptr %"%part_size_3", align 8
  store i64 0, ptr %"%part_boot_0", align 8
  store i64 0, ptr %"%part_boot_1", align 8
  store i64 0, ptr %"%part_boot_2", align 8
  store i64 0, ptr %"%part_boot_3", align 8
  store i64 0, ptr %"%part_count", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
