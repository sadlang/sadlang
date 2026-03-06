; ModuleID = 'أجهزة_مدير_نحلة'
source_filename = "\D8\A3\D8\AC\D9\87\D8\B2\D8\A9_\D9\85\D8\AF\D9\8A\D8\B1_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@COM1 = constant i64 1016
@DEV_TYPE_NONE = constant i64 0
@DEV_TYPE_PCI = constant i64 1
@DEV_TYPE_USB = constant i64 2
@DEV_TYPE_PS2 = constant i64 3
@DEV_TYPE_ATA = constant i64 4
@DEV_TYPE_NET = constant i64 5
@DEV_TYPE_GFX = constant i64 6
@DEV_TYPE_AUDIO = constant i64 7
@DEV_TYPE_SERIAL = constant i64 8
@DEV_TYPE_TIMER = constant i64 9
@DEV_TYPE_ACPI = constant i64 10
@DEV_TYPE_BUS = constant i64 11
@DEV_TYPE_HID = constant i64 12
@DEV_TYPE_STORAGE = constant i64 13
@DEV_TYPE_HUB = constant i64 14
@DEV_TYPE_PLATFORM = constant i64 15
@DEV_STATE_UNKNOWN = constant i64 0
@DEV_STATE_DETECTED = constant i64 1
@DEV_STATE_INIT = constant i64 2
@DEV_STATE_ACTIVE = constant i64 3
@DEV_STATE_SUSPENDED = constant i64 4
@DEV_STATE_ERROR = constant i64 5
@DEV_STATE_REMOVED = constant i64 6
@DEV_STATE_DISABLED = constant i64 7
@RES_TYPE_NONE = constant i64 0
@RES_TYPE_IO = constant i64 1
@RES_TYPE_MEM = constant i64 2
@RES_TYPE_IRQ = constant i64 3
@RES_TYPE_DMA = constant i64 4
@MAX_DEVICES = constant i64 64
@MAX_DRIVERS = constant i64 32
@MAX_RESOURCES = constant i64 128
@MAX_IRQ = constant i64 16
@MAX_IO_REGIONS = constant i64 32
@dev_type_0 = global i64 0
@dev_type_1 = global i64 0
@dev_type_2 = global i64 0
@dev_type_3 = global i64 0
@dev_type_4 = global i64 0
@dev_type_5 = global i64 0
@dev_type_6 = global i64 0
@dev_type_7 = global i64 0
@dev_type_8 = global i64 0
@dev_type_9 = global i64 0
@dev_type_10 = global i64 0
@dev_type_11 = global i64 0
@dev_type_12 = global i64 0
@dev_type_13 = global i64 0
@dev_type_14 = global i64 0
@dev_type_15 = global i64 0
@dev_type_16 = global i64 0
@dev_type_17 = global i64 0
@dev_type_18 = global i64 0
@dev_type_19 = global i64 0
@dev_type_20 = global i64 0
@dev_type_21 = global i64 0
@dev_type_22 = global i64 0
@dev_type_23 = global i64 0
@dev_type_24 = global i64 0
@dev_type_25 = global i64 0
@dev_type_26 = global i64 0
@dev_type_27 = global i64 0
@dev_type_28 = global i64 0
@dev_type_29 = global i64 0
@dev_type_30 = global i64 0
@dev_type_31 = global i64 0
@dev_type_32 = global i64 0
@dev_type_33 = global i64 0
@dev_type_34 = global i64 0
@dev_type_35 = global i64 0
@dev_type_36 = global i64 0
@dev_type_37 = global i64 0
@dev_type_38 = global i64 0
@dev_type_39 = global i64 0
@dev_type_40 = global i64 0
@dev_type_41 = global i64 0
@dev_type_42 = global i64 0
@dev_type_43 = global i64 0
@dev_type_44 = global i64 0
@dev_type_45 = global i64 0
@dev_type_46 = global i64 0
@dev_type_47 = global i64 0
@dev_type_48 = global i64 0
@dev_type_49 = global i64 0
@dev_type_50 = global i64 0
@dev_type_51 = global i64 0
@dev_type_52 = global i64 0
@dev_type_53 = global i64 0
@dev_type_54 = global i64 0
@dev_type_55 = global i64 0
@dev_type_56 = global i64 0
@dev_type_57 = global i64 0
@dev_type_58 = global i64 0
@dev_type_59 = global i64 0
@dev_type_60 = global i64 0
@dev_type_61 = global i64 0
@dev_type_62 = global i64 0
@dev_type_63 = global i64 0
@dev_state_0 = global i64 0
@dev_state_1 = global i64 0
@dev_state_2 = global i64 0
@dev_state_3 = global i64 0
@dev_state_4 = global i64 0
@dev_state_5 = global i64 0
@dev_state_6 = global i64 0
@dev_state_7 = global i64 0
@dev_state_8 = global i64 0
@dev_state_9 = global i64 0
@dev_state_10 = global i64 0
@dev_state_11 = global i64 0
@dev_state_12 = global i64 0
@dev_state_13 = global i64 0
@dev_state_14 = global i64 0
@dev_state_15 = global i64 0
@dev_state_16 = global i64 0
@dev_state_17 = global i64 0
@dev_state_18 = global i64 0
@dev_state_19 = global i64 0
@dev_state_20 = global i64 0
@dev_state_21 = global i64 0
@dev_state_22 = global i64 0
@dev_state_23 = global i64 0
@dev_state_24 = global i64 0
@dev_state_25 = global i64 0
@dev_state_26 = global i64 0
@dev_state_27 = global i64 0
@dev_state_28 = global i64 0
@dev_state_29 = global i64 0
@dev_state_30 = global i64 0
@dev_state_31 = global i64 0
@dev_state_32 = global i64 0
@dev_state_33 = global i64 0
@dev_state_34 = global i64 0
@dev_state_35 = global i64 0
@dev_state_36 = global i64 0
@dev_state_37 = global i64 0
@dev_state_38 = global i64 0
@dev_state_39 = global i64 0
@dev_state_40 = global i64 0
@dev_state_41 = global i64 0
@dev_state_42 = global i64 0
@dev_state_43 = global i64 0
@dev_state_44 = global i64 0
@dev_state_45 = global i64 0
@dev_state_46 = global i64 0
@dev_state_47 = global i64 0
@dev_state_48 = global i64 0
@dev_state_49 = global i64 0
@dev_state_50 = global i64 0
@dev_state_51 = global i64 0
@dev_state_52 = global i64 0
@dev_state_53 = global i64 0
@dev_state_54 = global i64 0
@dev_state_55 = global i64 0
@dev_state_56 = global i64 0
@dev_state_57 = global i64 0
@dev_state_58 = global i64 0
@dev_state_59 = global i64 0
@dev_state_60 = global i64 0
@dev_state_61 = global i64 0
@dev_state_62 = global i64 0
@dev_state_63 = global i64 0
@dev_vid_0 = global i64 0
@dev_vid_1 = global i64 0
@dev_vid_2 = global i64 0
@dev_vid_3 = global i64 0
@dev_vid_4 = global i64 0
@dev_vid_5 = global i64 0
@dev_vid_6 = global i64 0
@dev_vid_7 = global i64 0
@dev_vid_8 = global i64 0
@dev_vid_9 = global i64 0
@dev_vid_10 = global i64 0
@dev_vid_11 = global i64 0
@dev_vid_12 = global i64 0
@dev_vid_13 = global i64 0
@dev_vid_14 = global i64 0
@dev_vid_15 = global i64 0
@dev_vid_16 = global i64 0
@dev_vid_17 = global i64 0
@dev_vid_18 = global i64 0
@dev_vid_19 = global i64 0
@dev_vid_20 = global i64 0
@dev_vid_21 = global i64 0
@dev_vid_22 = global i64 0
@dev_vid_23 = global i64 0
@dev_vid_24 = global i64 0
@dev_vid_25 = global i64 0
@dev_vid_26 = global i64 0
@dev_vid_27 = global i64 0
@dev_vid_28 = global i64 0
@dev_vid_29 = global i64 0
@dev_vid_30 = global i64 0
@dev_vid_31 = global i64 0
@dev_did_0 = global i64 0
@dev_did_1 = global i64 0
@dev_did_2 = global i64 0
@dev_did_3 = global i64 0
@dev_did_4 = global i64 0
@dev_did_5 = global i64 0
@dev_did_6 = global i64 0
@dev_did_7 = global i64 0
@dev_did_8 = global i64 0
@dev_did_9 = global i64 0
@dev_did_10 = global i64 0
@dev_did_11 = global i64 0
@dev_did_12 = global i64 0
@dev_did_13 = global i64 0
@dev_did_14 = global i64 0
@dev_did_15 = global i64 0
@dev_did_16 = global i64 0
@dev_did_17 = global i64 0
@dev_did_18 = global i64 0
@dev_did_19 = global i64 0
@dev_did_20 = global i64 0
@dev_did_21 = global i64 0
@dev_did_22 = global i64 0
@dev_did_23 = global i64 0
@dev_did_24 = global i64 0
@dev_did_25 = global i64 0
@dev_did_26 = global i64 0
@dev_did_27 = global i64 0
@dev_did_28 = global i64 0
@dev_did_29 = global i64 0
@dev_did_30 = global i64 0
@dev_did_31 = global i64 0
@dev_parent_0 = global i64 0
@dev_parent_1 = global i64 0
@dev_parent_2 = global i64 0
@dev_parent_3 = global i64 0
@dev_parent_4 = global i64 0
@dev_parent_5 = global i64 0
@dev_parent_6 = global i64 0
@dev_parent_7 = global i64 0
@dev_parent_8 = global i64 0
@dev_parent_9 = global i64 0
@dev_parent_10 = global i64 0
@dev_parent_11 = global i64 0
@dev_parent_12 = global i64 0
@dev_parent_13 = global i64 0
@dev_parent_14 = global i64 0
@dev_parent_15 = global i64 0
@dev_parent_16 = global i64 0
@dev_parent_17 = global i64 0
@dev_parent_18 = global i64 0
@dev_parent_19 = global i64 0
@dev_parent_20 = global i64 0
@dev_parent_21 = global i64 0
@dev_parent_22 = global i64 0
@dev_parent_23 = global i64 0
@dev_parent_24 = global i64 0
@dev_parent_25 = global i64 0
@dev_parent_26 = global i64 0
@dev_parent_27 = global i64 0
@dev_parent_28 = global i64 0
@dev_parent_29 = global i64 0
@dev_parent_30 = global i64 0
@dev_parent_31 = global i64 0
@dev_drv_0 = global i64 0
@dev_drv_1 = global i64 0
@dev_drv_2 = global i64 0
@dev_drv_3 = global i64 0
@dev_drv_4 = global i64 0
@dev_drv_5 = global i64 0
@dev_drv_6 = global i64 0
@dev_drv_7 = global i64 0
@dev_drv_8 = global i64 0
@dev_drv_9 = global i64 0
@dev_drv_10 = global i64 0
@dev_drv_11 = global i64 0
@dev_drv_12 = global i64 0
@dev_drv_13 = global i64 0
@dev_drv_14 = global i64 0
@dev_drv_15 = global i64 0
@dev_drv_16 = global i64 0
@dev_drv_17 = global i64 0
@dev_drv_18 = global i64 0
@dev_drv_19 = global i64 0
@dev_drv_20 = global i64 0
@dev_drv_21 = global i64 0
@dev_drv_22 = global i64 0
@dev_drv_23 = global i64 0
@dev_drv_24 = global i64 0
@dev_drv_25 = global i64 0
@dev_drv_26 = global i64 0
@dev_drv_27 = global i64 0
@dev_drv_28 = global i64 0
@dev_drv_29 = global i64 0
@dev_drv_30 = global i64 0
@dev_drv_31 = global i64 0
@dev_class_0 = global i64 0
@dev_class_1 = global i64 0
@dev_class_2 = global i64 0
@dev_class_3 = global i64 0
@dev_class_4 = global i64 0
@dev_class_5 = global i64 0
@dev_class_6 = global i64 0
@dev_class_7 = global i64 0
@dev_class_8 = global i64 0
@dev_class_9 = global i64 0
@dev_class_10 = global i64 0
@dev_class_11 = global i64 0
@dev_class_12 = global i64 0
@dev_class_13 = global i64 0
@dev_class_14 = global i64 0
@dev_class_15 = global i64 0
@dev_class_16 = global i64 0
@dev_class_17 = global i64 0
@dev_class_18 = global i64 0
@dev_class_19 = global i64 0
@dev_class_20 = global i64 0
@dev_class_21 = global i64 0
@dev_class_22 = global i64 0
@dev_class_23 = global i64 0
@dev_class_24 = global i64 0
@dev_class_25 = global i64 0
@dev_class_26 = global i64 0
@dev_class_27 = global i64 0
@dev_class_28 = global i64 0
@dev_class_29 = global i64 0
@dev_class_30 = global i64 0
@dev_class_31 = global i64 0
@dev_sub_0 = global i64 0
@dev_sub_1 = global i64 0
@dev_sub_2 = global i64 0
@dev_sub_3 = global i64 0
@dev_sub_4 = global i64 0
@dev_sub_5 = global i64 0
@dev_sub_6 = global i64 0
@dev_sub_7 = global i64 0
@dev_sub_8 = global i64 0
@dev_sub_9 = global i64 0
@dev_sub_10 = global i64 0
@dev_sub_11 = global i64 0
@dev_sub_12 = global i64 0
@dev_sub_13 = global i64 0
@dev_sub_14 = global i64 0
@dev_sub_15 = global i64 0
@dev_sub_16 = global i64 0
@dev_sub_17 = global i64 0
@dev_sub_18 = global i64 0
@dev_sub_19 = global i64 0
@dev_sub_20 = global i64 0
@dev_sub_21 = global i64 0
@dev_sub_22 = global i64 0
@dev_sub_23 = global i64 0
@dev_sub_24 = global i64 0
@dev_sub_25 = global i64 0
@dev_sub_26 = global i64 0
@dev_sub_27 = global i64 0
@dev_sub_28 = global i64 0
@dev_sub_29 = global i64 0
@dev_sub_30 = global i64 0
@dev_sub_31 = global i64 0
@dev_irq_0 = global i64 0
@dev_irq_1 = global i64 0
@dev_irq_2 = global i64 0
@dev_irq_3 = global i64 0
@dev_irq_4 = global i64 0
@dev_irq_5 = global i64 0
@dev_irq_6 = global i64 0
@dev_irq_7 = global i64 0
@dev_irq_8 = global i64 0
@dev_irq_9 = global i64 0
@dev_irq_10 = global i64 0
@dev_irq_11 = global i64 0
@dev_irq_12 = global i64 0
@dev_irq_13 = global i64 0
@dev_irq_14 = global i64 0
@dev_irq_15 = global i64 0
@dev_irq_16 = global i64 0
@dev_irq_17 = global i64 0
@dev_irq_18 = global i64 0
@dev_irq_19 = global i64 0
@dev_irq_20 = global i64 0
@dev_irq_21 = global i64 0
@dev_irq_22 = global i64 0
@dev_irq_23 = global i64 0
@dev_irq_24 = global i64 0
@dev_irq_25 = global i64 0
@dev_irq_26 = global i64 0
@dev_irq_27 = global i64 0
@dev_irq_28 = global i64 0
@dev_irq_29 = global i64 0
@dev_irq_30 = global i64 0
@dev_irq_31 = global i64 0
@dev_io_0 = global i64 0
@dev_io_1 = global i64 0
@dev_io_2 = global i64 0
@dev_io_3 = global i64 0
@dev_io_4 = global i64 0
@dev_io_5 = global i64 0
@dev_io_6 = global i64 0
@dev_io_7 = global i64 0
@dev_io_8 = global i64 0
@dev_io_9 = global i64 0
@dev_io_10 = global i64 0
@dev_io_11 = global i64 0
@dev_io_12 = global i64 0
@dev_io_13 = global i64 0
@dev_io_14 = global i64 0
@dev_io_15 = global i64 0
@dev_io_16 = global i64 0
@dev_io_17 = global i64 0
@dev_io_18 = global i64 0
@dev_io_19 = global i64 0
@dev_io_20 = global i64 0
@dev_io_21 = global i64 0
@dev_io_22 = global i64 0
@dev_io_23 = global i64 0
@dev_io_24 = global i64 0
@dev_io_25 = global i64 0
@dev_io_26 = global i64 0
@dev_io_27 = global i64 0
@dev_io_28 = global i64 0
@dev_io_29 = global i64 0
@dev_io_30 = global i64 0
@dev_io_31 = global i64 0
@dev_mmio_0 = global i64 0
@dev_mmio_1 = global i64 0
@dev_mmio_2 = global i64 0
@dev_mmio_3 = global i64 0
@dev_mmio_4 = global i64 0
@dev_mmio_5 = global i64 0
@dev_mmio_6 = global i64 0
@dev_mmio_7 = global i64 0
@dev_mmio_8 = global i64 0
@dev_mmio_9 = global i64 0
@dev_mmio_10 = global i64 0
@dev_mmio_11 = global i64 0
@dev_mmio_12 = global i64 0
@dev_mmio_13 = global i64 0
@dev_mmio_14 = global i64 0
@dev_mmio_15 = global i64 0
@dev_mmio_16 = global i64 0
@dev_mmio_17 = global i64 0
@dev_mmio_18 = global i64 0
@dev_mmio_19 = global i64 0
@dev_mmio_20 = global i64 0
@dev_mmio_21 = global i64 0
@dev_mmio_22 = global i64 0
@dev_mmio_23 = global i64 0
@dev_mmio_24 = global i64 0
@dev_mmio_25 = global i64 0
@dev_mmio_26 = global i64 0
@dev_mmio_27 = global i64 0
@dev_mmio_28 = global i64 0
@dev_mmio_29 = global i64 0
@dev_mmio_30 = global i64 0
@dev_mmio_31 = global i64 0
@drv_type_0 = global i64 0
@drv_type_1 = global i64 0
@drv_type_2 = global i64 0
@drv_type_3 = global i64 0
@drv_type_4 = global i64 0
@drv_type_5 = global i64 0
@drv_type_6 = global i64 0
@drv_type_7 = global i64 0
@drv_type_8 = global i64 0
@drv_type_9 = global i64 0
@drv_type_10 = global i64 0
@drv_type_11 = global i64 0
@drv_type_12 = global i64 0
@drv_type_13 = global i64 0
@drv_type_14 = global i64 0
@drv_type_15 = global i64 0
@drv_reg_0 = global i64 0
@drv_reg_1 = global i64 0
@drv_reg_2 = global i64 0
@drv_reg_3 = global i64 0
@drv_reg_4 = global i64 0
@drv_reg_5 = global i64 0
@drv_reg_6 = global i64 0
@drv_reg_7 = global i64 0
@drv_reg_8 = global i64 0
@drv_reg_9 = global i64 0
@drv_reg_10 = global i64 0
@drv_reg_11 = global i64 0
@drv_reg_12 = global i64 0
@drv_reg_13 = global i64 0
@drv_reg_14 = global i64 0
@drv_reg_15 = global i64 0
@drv_pclass_0 = global i64 0
@drv_pclass_1 = global i64 0
@drv_pclass_2 = global i64 0
@drv_pclass_3 = global i64 0
@drv_pclass_4 = global i64 0
@drv_pclass_5 = global i64 0
@drv_pclass_6 = global i64 0
@drv_pclass_7 = global i64 0
@drv_pclass_8 = global i64 0
@drv_pclass_9 = global i64 0
@drv_pclass_10 = global i64 0
@drv_pclass_11 = global i64 0
@drv_pclass_12 = global i64 0
@drv_pclass_13 = global i64 0
@drv_pclass_14 = global i64 0
@drv_pclass_15 = global i64 0
@drv_psub_0 = global i64 0
@drv_psub_1 = global i64 0
@drv_psub_2 = global i64 0
@drv_psub_3 = global i64 0
@drv_psub_4 = global i64 0
@drv_psub_5 = global i64 0
@drv_psub_6 = global i64 0
@drv_psub_7 = global i64 0
@drv_psub_8 = global i64 0
@drv_psub_9 = global i64 0
@drv_psub_10 = global i64 0
@drv_psub_11 = global i64 0
@drv_psub_12 = global i64 0
@drv_psub_13 = global i64 0
@drv_psub_14 = global i64 0
@drv_psub_15 = global i64 0
@drv_devcount_0 = global i64 0
@drv_devcount_1 = global i64 0
@drv_devcount_2 = global i64 0
@drv_devcount_3 = global i64 0
@drv_devcount_4 = global i64 0
@drv_devcount_5 = global i64 0
@drv_devcount_6 = global i64 0
@drv_devcount_7 = global i64 0
@drv_devcount_8 = global i64 0
@drv_devcount_9 = global i64 0
@drv_devcount_10 = global i64 0
@drv_devcount_11 = global i64 0
@drv_devcount_12 = global i64 0
@drv_devcount_13 = global i64 0
@drv_devcount_14 = global i64 0
@drv_devcount_15 = global i64 0
@irq_owner_0 = global i64 0
@irq_owner_1 = global i64 0
@irq_owner_2 = global i64 0
@irq_owner_3 = global i64 0
@irq_owner_4 = global i64 0
@irq_owner_5 = global i64 0
@irq_owner_6 = global i64 0
@irq_owner_7 = global i64 0
@irq_owner_8 = global i64 0
@irq_owner_9 = global i64 0
@irq_owner_10 = global i64 0
@irq_owner_11 = global i64 0
@irq_owner_12 = global i64 0
@irq_owner_13 = global i64 0
@irq_owner_14 = global i64 0
@irq_owner_15 = global i64 0
@devmgr_dev_count = global i64 0
@devmgr_drv_count = global i64 0
@devmgr_bindings = global i64 0
@str.const = private unnamed_addr constant [20 x i8] c"[DM] IRQ conflict: \00", align 1
@str.const.1 = private unnamed_addr constant [13 x i8] c"[DM] Device \00", align 1
@str.const.2 = private unnamed_addr constant [19 x i8] c" registered, type=\00", align 1
@str.const.3 = private unnamed_addr constant [8 x i8] c" VID=0x\00", align 1
@str.const.4 = private unnamed_addr constant [8 x i8] c" DID=0x\00", align 1
@str.const.5 = private unnamed_addr constant [13 x i8] c"[DM] Driver \00", align 1
@str.const.6 = private unnamed_addr constant [22 x i8] c" registered for type=\00", align 1
@str.const.7 = private unnamed_addr constant [16 x i8] c"[DM] Bound dev \00", align 1
@str.const.8 = private unnamed_addr constant [9 x i8] c" -> drv \00", align 1
@str.const.9 = private unnamed_addr constant [60 x i8] c"\E2\95\90\E2\95\90\E2\95\90 [DM] \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D9\85\D8\AF\D9\8A\D8\B1 \D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9 \E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.10 = private unnamed_addr constant [26 x i8] c"  \D8\A3\D8\AC\D9\87\D8\B2\D8\A9 \D9\85\D8\B3\D8\AC\D9\84\D8\A9: \00", align 1
@str.const.11 = private unnamed_addr constant [30 x i8] c"  \D8\AA\D8\B9\D8\B1\D9\8A\D9\81\D8\A7\D8\AA \D9\85\D8\B3\D8\AC\D9\84\D8\A9: \00", align 1
@str.const.12 = private unnamed_addr constant [24 x i8] c"  \D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA \D8\B1\D8\A8\D8\B7: \00", align 1
@str.const.13 = private unnamed_addr constant [11 x i8] c"  \D9\86\D8\B4\D8\B7: \00", align 1
@str.const.14 = private unnamed_addr constant [25 x i8] c" \D9\82\D9\8A\D8\AF \D8\A7\D9\84\D8\AA\D9\87\D9\8A\D8\A6\D8\A9: \00", align 1
@str.const.15 = private unnamed_addr constant [14 x i8] c" \D9\85\D9\83\D8\AA\D8\B4\D9\81: \00", align 1
@str.const.16 = private unnamed_addr constant [10 x i8] c" \D8\AE\D8\B7\D8\A3: \00", align 1
@str.const.17 = private unnamed_addr constant [106 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.18 = private unnamed_addr constant [4 x i8] c"  [\00", align 1
@str.const.19 = private unnamed_addr constant [8 x i8] c"] type=\00", align 1
@str.const.20 = private unnamed_addr constant [8 x i8] c" state=\00", align 1
@str.const.21 = private unnamed_addr constant [43 x i8] c"[DM] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D9\85\D8\AF\D9\8A\D8\B1 \D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9...\00", align 1
@str.const.22 = private unnamed_addr constant [19 x i8] c"[DM] \D8\AC\D8\A7\D9\87\D8\B2 \E2\80\94 \00", align 1
@str.const.23 = private unnamed_addr constant [11 x i8] c" devices, \00", align 1
@str.const.24 = private unnamed_addr constant [7 x i8] c" bound\00", align 1

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

define i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
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

define i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
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

define i64 @dm_serial_hex8(i64 %val) {
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

define i64 @dm_serial_hex16(i64 %val) {
entry:
  %val1 = alloca i64, align 8
  store i64 %val, ptr %val1, align 8
  %"%val.load" = load i64, ptr %val1, align 8
  %shrtmp = lshr i64 %"%val.load", 8
  %calltmp = call i64 @dm_serial_hex8(i64 %shrtmp)
  %"%val.load2" = load i64, ptr %val1, align 8
  %andtmp = and i64 %"%val.load2", 255
  %calltmp3 = call i64 @dm_serial_hex8(i64 %andtmp)
  ret i64 0
}

define i64 @"dm_serial_\D8\B3\D8\B7\D8\B1"() {
entry:
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 13)
  %"%COM1.load1" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load1", i64 10)
  ret i64 0
}

define i64 @dm_get_type(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_30, label %merge_31

merge_31:                                         ; preds = %entry
  %"%d.load2" = load i64, ptr %d1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%d.load2", 1
  br i1 %cmpeqtmp3, label %then_32, label %merge_33

merge_33:                                         ; preds = %merge_31
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_34, label %merge_35

merge_35:                                         ; preds = %merge_33
  %"%d.load8" = load i64, ptr %d1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 3
  br i1 %cmpeqtmp9, label %then_36, label %merge_37

merge_37:                                         ; preds = %merge_35
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 4
  br i1 %cmpeqtmp12, label %then_38, label %merge_39

merge_39:                                         ; preds = %merge_37
  %"%d.load14" = load i64, ptr %d1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%d.load14", 5
  br i1 %cmpeqtmp15, label %then_40, label %merge_41

merge_41:                                         ; preds = %merge_39
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 6
  br i1 %cmpeqtmp18, label %then_42, label %merge_43

merge_43:                                         ; preds = %merge_41
  %"%d.load20" = load i64, ptr %d1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%d.load20", 7
  br i1 %cmpeqtmp21, label %then_44, label %merge_45

merge_45:                                         ; preds = %merge_43
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 8
  br i1 %cmpeqtmp24, label %then_46, label %merge_47

merge_47:                                         ; preds = %merge_45
  %"%d.load26" = load i64, ptr %d1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%d.load26", 9
  br i1 %cmpeqtmp27, label %then_48, label %merge_49

merge_49:                                         ; preds = %merge_47
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 10
  br i1 %cmpeqtmp30, label %then_50, label %merge_51

merge_51:                                         ; preds = %merge_49
  %"%d.load32" = load i64, ptr %d1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%d.load32", 11
  br i1 %cmpeqtmp33, label %then_52, label %merge_53

merge_53:                                         ; preds = %merge_51
  %"%d.load35" = load i64, ptr %d1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%d.load35", 12
  br i1 %cmpeqtmp36, label %then_54, label %merge_55

merge_55:                                         ; preds = %merge_53
  %"%d.load38" = load i64, ptr %d1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%d.load38", 13
  br i1 %cmpeqtmp39, label %then_56, label %merge_57

merge_57:                                         ; preds = %merge_55
  %"%d.load41" = load i64, ptr %d1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%d.load41", 14
  br i1 %cmpeqtmp42, label %then_58, label %merge_59

merge_59:                                         ; preds = %merge_57
  %"%d.load44" = load i64, ptr %d1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%d.load44", 15
  br i1 %cmpeqtmp45, label %then_60, label %merge_61

merge_61:                                         ; preds = %merge_59
  %"%d.load47" = load i64, ptr %d1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%d.load47", 16
  br i1 %cmpeqtmp48, label %then_62, label %merge_63

merge_63:                                         ; preds = %merge_61
  %"%d.load50" = load i64, ptr %d1, align 8
  %cmpeqtmp51 = icmp eq i64 %"%d.load50", 17
  br i1 %cmpeqtmp51, label %then_64, label %merge_65

merge_65:                                         ; preds = %merge_63
  %"%d.load53" = load i64, ptr %d1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%d.load53", 18
  br i1 %cmpeqtmp54, label %then_66, label %merge_67

merge_67:                                         ; preds = %merge_65
  %"%d.load56" = load i64, ptr %d1, align 8
  %cmpeqtmp57 = icmp eq i64 %"%d.load56", 19
  br i1 %cmpeqtmp57, label %then_68, label %merge_69

merge_69:                                         ; preds = %merge_67
  %"%d.load59" = load i64, ptr %d1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%d.load59", 20
  br i1 %cmpeqtmp60, label %then_70, label %merge_71

merge_71:                                         ; preds = %merge_69
  %"%d.load62" = load i64, ptr %d1, align 8
  %cmpeqtmp63 = icmp eq i64 %"%d.load62", 21
  br i1 %cmpeqtmp63, label %then_72, label %merge_73

merge_73:                                         ; preds = %merge_71
  %"%d.load65" = load i64, ptr %d1, align 8
  %cmpeqtmp66 = icmp eq i64 %"%d.load65", 22
  br i1 %cmpeqtmp66, label %then_74, label %merge_75

merge_75:                                         ; preds = %merge_73
  %"%d.load68" = load i64, ptr %d1, align 8
  %cmpeqtmp69 = icmp eq i64 %"%d.load68", 23
  br i1 %cmpeqtmp69, label %then_76, label %merge_77

merge_77:                                         ; preds = %merge_75
  %"%d.load71" = load i64, ptr %d1, align 8
  %cmpeqtmp72 = icmp eq i64 %"%d.load71", 24
  br i1 %cmpeqtmp72, label %then_78, label %merge_79

merge_79:                                         ; preds = %merge_77
  %"%d.load74" = load i64, ptr %d1, align 8
  %cmpeqtmp75 = icmp eq i64 %"%d.load74", 25
  br i1 %cmpeqtmp75, label %then_80, label %merge_81

merge_81:                                         ; preds = %merge_79
  %"%d.load77" = load i64, ptr %d1, align 8
  %cmpeqtmp78 = icmp eq i64 %"%d.load77", 26
  br i1 %cmpeqtmp78, label %then_82, label %merge_83

merge_83:                                         ; preds = %merge_81
  %"%d.load80" = load i64, ptr %d1, align 8
  %cmpeqtmp81 = icmp eq i64 %"%d.load80", 27
  br i1 %cmpeqtmp81, label %then_84, label %merge_85

merge_85:                                         ; preds = %merge_83
  %"%d.load83" = load i64, ptr %d1, align 8
  %cmpeqtmp84 = icmp eq i64 %"%d.load83", 28
  br i1 %cmpeqtmp84, label %then_86, label %merge_87

merge_87:                                         ; preds = %merge_85
  %"%d.load86" = load i64, ptr %d1, align 8
  %cmpeqtmp87 = icmp eq i64 %"%d.load86", 29
  br i1 %cmpeqtmp87, label %then_88, label %merge_89

merge_89:                                         ; preds = %merge_87
  %"%d.load89" = load i64, ptr %d1, align 8
  %cmpeqtmp90 = icmp eq i64 %"%d.load89", 30
  br i1 %cmpeqtmp90, label %then_90, label %merge_91

merge_91:                                         ; preds = %merge_89
  %"%d.load92" = load i64, ptr %d1, align 8
  %cmpeqtmp93 = icmp eq i64 %"%d.load92", 31
  br i1 %cmpeqtmp93, label %then_92, label %merge_93

merge_93:                                         ; preds = %merge_91
  ret i64 0

then_30:                                          ; preds = %entry
  %loadtmp = load i64, ptr @dev_type_0, align 8
  ret i64 %loadtmp

then_32:                                          ; preds = %merge_31
  %loadtmp4 = load i64, ptr @dev_type_1, align 8
  ret i64 %loadtmp4

then_34:                                          ; preds = %merge_33
  %loadtmp7 = load i64, ptr @dev_type_2, align 8
  ret i64 %loadtmp7

then_36:                                          ; preds = %merge_35
  %loadtmp10 = load i64, ptr @dev_type_3, align 8
  ret i64 %loadtmp10

then_38:                                          ; preds = %merge_37
  %loadtmp13 = load i64, ptr @dev_type_4, align 8
  ret i64 %loadtmp13

then_40:                                          ; preds = %merge_39
  %loadtmp16 = load i64, ptr @dev_type_5, align 8
  ret i64 %loadtmp16

then_42:                                          ; preds = %merge_41
  %loadtmp19 = load i64, ptr @dev_type_6, align 8
  ret i64 %loadtmp19

then_44:                                          ; preds = %merge_43
  %loadtmp22 = load i64, ptr @dev_type_7, align 8
  ret i64 %loadtmp22

then_46:                                          ; preds = %merge_45
  %loadtmp25 = load i64, ptr @dev_type_8, align 8
  ret i64 %loadtmp25

then_48:                                          ; preds = %merge_47
  %loadtmp28 = load i64, ptr @dev_type_9, align 8
  ret i64 %loadtmp28

then_50:                                          ; preds = %merge_49
  %loadtmp31 = load i64, ptr @dev_type_10, align 8
  ret i64 %loadtmp31

then_52:                                          ; preds = %merge_51
  %loadtmp34 = load i64, ptr @dev_type_11, align 8
  ret i64 %loadtmp34

then_54:                                          ; preds = %merge_53
  %loadtmp37 = load i64, ptr @dev_type_12, align 8
  ret i64 %loadtmp37

then_56:                                          ; preds = %merge_55
  %loadtmp40 = load i64, ptr @dev_type_13, align 8
  ret i64 %loadtmp40

then_58:                                          ; preds = %merge_57
  %loadtmp43 = load i64, ptr @dev_type_14, align 8
  ret i64 %loadtmp43

then_60:                                          ; preds = %merge_59
  %loadtmp46 = load i64, ptr @dev_type_15, align 8
  ret i64 %loadtmp46

then_62:                                          ; preds = %merge_61
  %loadtmp49 = load i64, ptr @dev_type_16, align 8
  ret i64 %loadtmp49

then_64:                                          ; preds = %merge_63
  %loadtmp52 = load i64, ptr @dev_type_17, align 8
  ret i64 %loadtmp52

then_66:                                          ; preds = %merge_65
  %loadtmp55 = load i64, ptr @dev_type_18, align 8
  ret i64 %loadtmp55

then_68:                                          ; preds = %merge_67
  %loadtmp58 = load i64, ptr @dev_type_19, align 8
  ret i64 %loadtmp58

then_70:                                          ; preds = %merge_69
  %loadtmp61 = load i64, ptr @dev_type_20, align 8
  ret i64 %loadtmp61

then_72:                                          ; preds = %merge_71
  %loadtmp64 = load i64, ptr @dev_type_21, align 8
  ret i64 %loadtmp64

then_74:                                          ; preds = %merge_73
  %loadtmp67 = load i64, ptr @dev_type_22, align 8
  ret i64 %loadtmp67

then_76:                                          ; preds = %merge_75
  %loadtmp70 = load i64, ptr @dev_type_23, align 8
  ret i64 %loadtmp70

then_78:                                          ; preds = %merge_77
  %loadtmp73 = load i64, ptr @dev_type_24, align 8
  ret i64 %loadtmp73

then_80:                                          ; preds = %merge_79
  %loadtmp76 = load i64, ptr @dev_type_25, align 8
  ret i64 %loadtmp76

then_82:                                          ; preds = %merge_81
  %loadtmp79 = load i64, ptr @dev_type_26, align 8
  ret i64 %loadtmp79

then_84:                                          ; preds = %merge_83
  %loadtmp82 = load i64, ptr @dev_type_27, align 8
  ret i64 %loadtmp82

then_86:                                          ; preds = %merge_85
  %loadtmp85 = load i64, ptr @dev_type_28, align 8
  ret i64 %loadtmp85

then_88:                                          ; preds = %merge_87
  %loadtmp88 = load i64, ptr @dev_type_29, align 8
  ret i64 %loadtmp88

then_90:                                          ; preds = %merge_89
  %loadtmp91 = load i64, ptr @dev_type_30, align 8
  ret i64 %loadtmp91

then_92:                                          ; preds = %merge_91
  %loadtmp94 = load i64, ptr @dev_type_31, align 8
  ret i64 %loadtmp94
}

define i64 @dm_set_type(i64 %d, i64 %val) {
entry:
  %"%dev_type_31" = alloca i64, align 8
  %"%dev_type_30" = alloca i64, align 8
  %"%dev_type_29" = alloca i64, align 8
  %"%dev_type_28" = alloca i64, align 8
  %"%dev_type_27" = alloca i64, align 8
  %"%dev_type_26" = alloca i64, align 8
  %"%dev_type_25" = alloca i64, align 8
  %"%dev_type_24" = alloca i64, align 8
  %"%dev_type_23" = alloca i64, align 8
  %"%dev_type_22" = alloca i64, align 8
  %"%dev_type_21" = alloca i64, align 8
  %"%dev_type_20" = alloca i64, align 8
  %"%dev_type_19" = alloca i64, align 8
  %"%dev_type_18" = alloca i64, align 8
  %"%dev_type_17" = alloca i64, align 8
  %"%dev_type_16" = alloca i64, align 8
  %"%dev_type_15" = alloca i64, align 8
  %"%dev_type_14" = alloca i64, align 8
  %"%dev_type_13" = alloca i64, align 8
  %"%dev_type_12" = alloca i64, align 8
  %"%dev_type_11" = alloca i64, align 8
  %"%dev_type_10" = alloca i64, align 8
  %"%dev_type_9" = alloca i64, align 8
  %"%dev_type_8" = alloca i64, align 8
  %"%dev_type_7" = alloca i64, align 8
  %"%dev_type_6" = alloca i64, align 8
  %"%dev_type_5" = alloca i64, align 8
  %"%dev_type_4" = alloca i64, align 8
  %"%dev_type_3" = alloca i64, align 8
  %"%dev_type_2" = alloca i64, align 8
  %"%dev_type_1" = alloca i64, align 8
  %"%dev_type_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_94, label %merge_95

merge_101:                                        ; preds = %then_100, %merge_99
  %"%d.load9" = load i64, ptr %d1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%d.load9", 4
  br i1 %cmpeqtmp10, label %then_102, label %merge_103

merge_103:                                        ; preds = %then_102, %merge_101
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 5
  br i1 %cmpeqtmp12, label %then_104, label %merge_105

merge_105:                                        ; preds = %then_104, %merge_103
  %"%d.load13" = load i64, ptr %d1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%d.load13", 6
  br i1 %cmpeqtmp14, label %then_106, label %merge_107

merge_107:                                        ; preds = %then_106, %merge_105
  %"%d.load15" = load i64, ptr %d1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%d.load15", 7
  br i1 %cmpeqtmp16, label %then_108, label %merge_109

merge_109:                                        ; preds = %then_108, %merge_107
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 8
  br i1 %cmpeqtmp18, label %then_110, label %merge_111

merge_111:                                        ; preds = %then_110, %merge_109
  %"%d.load19" = load i64, ptr %d1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%d.load19", 9
  br i1 %cmpeqtmp20, label %then_112, label %merge_113

merge_113:                                        ; preds = %then_112, %merge_111
  %"%d.load21" = load i64, ptr %d1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%d.load21", 10
  br i1 %cmpeqtmp22, label %then_114, label %merge_115

merge_115:                                        ; preds = %then_114, %merge_113
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 11
  br i1 %cmpeqtmp24, label %then_116, label %merge_117

merge_117:                                        ; preds = %then_116, %merge_115
  %"%d.load25" = load i64, ptr %d1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%d.load25", 12
  br i1 %cmpeqtmp26, label %then_118, label %merge_119

merge_119:                                        ; preds = %then_118, %merge_117
  %"%d.load27" = load i64, ptr %d1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%d.load27", 13
  br i1 %cmpeqtmp28, label %then_120, label %merge_121

merge_121:                                        ; preds = %then_120, %merge_119
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 14
  br i1 %cmpeqtmp30, label %then_122, label %merge_123

merge_123:                                        ; preds = %then_122, %merge_121
  %"%d.load31" = load i64, ptr %d1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%d.load31", 15
  br i1 %cmpeqtmp32, label %then_124, label %merge_125

merge_125:                                        ; preds = %then_124, %merge_123
  %"%d.load33" = load i64, ptr %d1, align 8
  %cmpeqtmp34 = icmp eq i64 %"%d.load33", 16
  br i1 %cmpeqtmp34, label %then_126, label %merge_127

merge_127:                                        ; preds = %then_126, %merge_125
  %"%d.load35" = load i64, ptr %d1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%d.load35", 17
  br i1 %cmpeqtmp36, label %then_128, label %merge_129

merge_129:                                        ; preds = %then_128, %merge_127
  %"%d.load37" = load i64, ptr %d1, align 8
  %cmpeqtmp38 = icmp eq i64 %"%d.load37", 18
  br i1 %cmpeqtmp38, label %then_130, label %merge_131

merge_131:                                        ; preds = %then_130, %merge_129
  %"%d.load39" = load i64, ptr %d1, align 8
  %cmpeqtmp40 = icmp eq i64 %"%d.load39", 19
  br i1 %cmpeqtmp40, label %then_132, label %merge_133

merge_133:                                        ; preds = %then_132, %merge_131
  %"%d.load41" = load i64, ptr %d1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%d.load41", 20
  br i1 %cmpeqtmp42, label %then_134, label %merge_135

merge_135:                                        ; preds = %then_134, %merge_133
  %"%d.load43" = load i64, ptr %d1, align 8
  %cmpeqtmp44 = icmp eq i64 %"%d.load43", 21
  br i1 %cmpeqtmp44, label %then_136, label %merge_137

merge_137:                                        ; preds = %then_136, %merge_135
  %"%d.load45" = load i64, ptr %d1, align 8
  %cmpeqtmp46 = icmp eq i64 %"%d.load45", 22
  br i1 %cmpeqtmp46, label %then_138, label %merge_139

merge_139:                                        ; preds = %then_138, %merge_137
  %"%d.load47" = load i64, ptr %d1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%d.load47", 23
  br i1 %cmpeqtmp48, label %then_140, label %merge_141

merge_141:                                        ; preds = %then_140, %merge_139
  %"%d.load49" = load i64, ptr %d1, align 8
  %cmpeqtmp50 = icmp eq i64 %"%d.load49", 24
  br i1 %cmpeqtmp50, label %then_142, label %merge_143

merge_143:                                        ; preds = %then_142, %merge_141
  %"%d.load51" = load i64, ptr %d1, align 8
  %cmpeqtmp52 = icmp eq i64 %"%d.load51", 25
  br i1 %cmpeqtmp52, label %then_144, label %merge_145

merge_145:                                        ; preds = %then_144, %merge_143
  %"%d.load53" = load i64, ptr %d1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%d.load53", 26
  br i1 %cmpeqtmp54, label %then_146, label %merge_147

merge_147:                                        ; preds = %then_146, %merge_145
  %"%d.load55" = load i64, ptr %d1, align 8
  %cmpeqtmp56 = icmp eq i64 %"%d.load55", 27
  br i1 %cmpeqtmp56, label %then_148, label %merge_149

merge_149:                                        ; preds = %then_148, %merge_147
  %"%d.load57" = load i64, ptr %d1, align 8
  %cmpeqtmp58 = icmp eq i64 %"%d.load57", 28
  br i1 %cmpeqtmp58, label %then_150, label %merge_151

merge_151:                                        ; preds = %then_150, %merge_149
  %"%d.load59" = load i64, ptr %d1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%d.load59", 29
  br i1 %cmpeqtmp60, label %then_152, label %merge_153

merge_153:                                        ; preds = %then_152, %merge_151
  %"%d.load61" = load i64, ptr %d1, align 8
  %cmpeqtmp62 = icmp eq i64 %"%d.load61", 30
  br i1 %cmpeqtmp62, label %then_154, label %merge_155

merge_155:                                        ; preds = %then_154, %merge_153
  %"%d.load63" = load i64, ptr %d1, align 8
  %cmpeqtmp64 = icmp eq i64 %"%d.load63", 31
  br i1 %cmpeqtmp64, label %then_156, label %merge_157

merge_157:                                        ; preds = %then_156, %merge_155
  ret i64 0

merge_95:                                         ; preds = %then_94, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_96, label %merge_97

merge_97:                                         ; preds = %then_96, %merge_95
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_98, label %merge_99

merge_99:                                         ; preds = %then_98, %merge_97
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_100, label %merge_101

then_100:                                         ; preds = %merge_99
  %"%289.load" = load i64, ptr %val2, align 8
  store i64 %"%289.load", ptr %"%dev_type_3", align 8
  br label %merge_101

then_102:                                         ; preds = %merge_101
  %"%293.load" = load i64, ptr %val2, align 8
  store i64 %"%293.load", ptr %"%dev_type_4", align 8
  br label %merge_103

then_104:                                         ; preds = %merge_103
  %"%297.load" = load i64, ptr %val2, align 8
  store i64 %"%297.load", ptr %"%dev_type_5", align 8
  br label %merge_105

then_106:                                         ; preds = %merge_105
  %"%301.load" = load i64, ptr %val2, align 8
  store i64 %"%301.load", ptr %"%dev_type_6", align 8
  br label %merge_107

then_108:                                         ; preds = %merge_107
  %"%305.load" = load i64, ptr %val2, align 8
  store i64 %"%305.load", ptr %"%dev_type_7", align 8
  br label %merge_109

then_110:                                         ; preds = %merge_109
  %"%309.load" = load i64, ptr %val2, align 8
  store i64 %"%309.load", ptr %"%dev_type_8", align 8
  br label %merge_111

then_112:                                         ; preds = %merge_111
  %"%313.load" = load i64, ptr %val2, align 8
  store i64 %"%313.load", ptr %"%dev_type_9", align 8
  br label %merge_113

then_114:                                         ; preds = %merge_113
  %"%317.load" = load i64, ptr %val2, align 8
  store i64 %"%317.load", ptr %"%dev_type_10", align 8
  br label %merge_115

then_116:                                         ; preds = %merge_115
  %"%321.load" = load i64, ptr %val2, align 8
  store i64 %"%321.load", ptr %"%dev_type_11", align 8
  br label %merge_117

then_118:                                         ; preds = %merge_117
  %"%325.load" = load i64, ptr %val2, align 8
  store i64 %"%325.load", ptr %"%dev_type_12", align 8
  br label %merge_119

then_120:                                         ; preds = %merge_119
  %"%329.load" = load i64, ptr %val2, align 8
  store i64 %"%329.load", ptr %"%dev_type_13", align 8
  br label %merge_121

then_122:                                         ; preds = %merge_121
  %"%333.load" = load i64, ptr %val2, align 8
  store i64 %"%333.load", ptr %"%dev_type_14", align 8
  br label %merge_123

then_124:                                         ; preds = %merge_123
  %"%337.load" = load i64, ptr %val2, align 8
  store i64 %"%337.load", ptr %"%dev_type_15", align 8
  br label %merge_125

then_126:                                         ; preds = %merge_125
  %"%341.load" = load i64, ptr %val2, align 8
  store i64 %"%341.load", ptr %"%dev_type_16", align 8
  br label %merge_127

then_128:                                         ; preds = %merge_127
  %"%345.load" = load i64, ptr %val2, align 8
  store i64 %"%345.load", ptr %"%dev_type_17", align 8
  br label %merge_129

then_130:                                         ; preds = %merge_129
  %"%349.load" = load i64, ptr %val2, align 8
  store i64 %"%349.load", ptr %"%dev_type_18", align 8
  br label %merge_131

then_132:                                         ; preds = %merge_131
  %"%353.load" = load i64, ptr %val2, align 8
  store i64 %"%353.load", ptr %"%dev_type_19", align 8
  br label %merge_133

then_134:                                         ; preds = %merge_133
  %"%357.load" = load i64, ptr %val2, align 8
  store i64 %"%357.load", ptr %"%dev_type_20", align 8
  br label %merge_135

then_136:                                         ; preds = %merge_135
  %"%361.load" = load i64, ptr %val2, align 8
  store i64 %"%361.load", ptr %"%dev_type_21", align 8
  br label %merge_137

then_138:                                         ; preds = %merge_137
  %"%365.load" = load i64, ptr %val2, align 8
  store i64 %"%365.load", ptr %"%dev_type_22", align 8
  br label %merge_139

then_140:                                         ; preds = %merge_139
  %"%369.load" = load i64, ptr %val2, align 8
  store i64 %"%369.load", ptr %"%dev_type_23", align 8
  br label %merge_141

then_142:                                         ; preds = %merge_141
  %"%373.load" = load i64, ptr %val2, align 8
  store i64 %"%373.load", ptr %"%dev_type_24", align 8
  br label %merge_143

then_144:                                         ; preds = %merge_143
  %"%377.load" = load i64, ptr %val2, align 8
  store i64 %"%377.load", ptr %"%dev_type_25", align 8
  br label %merge_145

then_146:                                         ; preds = %merge_145
  %"%381.load" = load i64, ptr %val2, align 8
  store i64 %"%381.load", ptr %"%dev_type_26", align 8
  br label %merge_147

then_148:                                         ; preds = %merge_147
  %"%385.load" = load i64, ptr %val2, align 8
  store i64 %"%385.load", ptr %"%dev_type_27", align 8
  br label %merge_149

then_150:                                         ; preds = %merge_149
  %"%389.load" = load i64, ptr %val2, align 8
  store i64 %"%389.load", ptr %"%dev_type_28", align 8
  br label %merge_151

then_152:                                         ; preds = %merge_151
  %"%393.load" = load i64, ptr %val2, align 8
  store i64 %"%393.load", ptr %"%dev_type_29", align 8
  br label %merge_153

then_154:                                         ; preds = %merge_153
  %"%397.load" = load i64, ptr %val2, align 8
  store i64 %"%397.load", ptr %"%dev_type_30", align 8
  br label %merge_155

then_156:                                         ; preds = %merge_155
  %"%401.load" = load i64, ptr %val2, align 8
  store i64 %"%401.load", ptr %"%dev_type_31", align 8
  br label %merge_157

then_94:                                          ; preds = %entry
  %"%277.load" = load i64, ptr %val2, align 8
  store i64 %"%277.load", ptr %"%dev_type_0", align 8
  br label %merge_95

then_96:                                          ; preds = %merge_95
  %"%281.load" = load i64, ptr %val2, align 8
  store i64 %"%281.load", ptr %"%dev_type_1", align 8
  br label %merge_97

then_98:                                          ; preds = %merge_97
  %"%285.load" = load i64, ptr %val2, align 8
  store i64 %"%285.load", ptr %"%dev_type_2", align 8
  br label %merge_99
}

define i64 @dm_get_state(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_158, label %merge_159

merge_159:                                        ; preds = %entry
  %"%d.load2" = load i64, ptr %d1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%d.load2", 1
  br i1 %cmpeqtmp3, label %then_160, label %merge_161

merge_161:                                        ; preds = %merge_159
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_162, label %merge_163

merge_163:                                        ; preds = %merge_161
  %"%d.load8" = load i64, ptr %d1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 3
  br i1 %cmpeqtmp9, label %then_164, label %merge_165

merge_165:                                        ; preds = %merge_163
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 4
  br i1 %cmpeqtmp12, label %then_166, label %merge_167

merge_167:                                        ; preds = %merge_165
  %"%d.load14" = load i64, ptr %d1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%d.load14", 5
  br i1 %cmpeqtmp15, label %then_168, label %merge_169

merge_169:                                        ; preds = %merge_167
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 6
  br i1 %cmpeqtmp18, label %then_170, label %merge_171

merge_171:                                        ; preds = %merge_169
  %"%d.load20" = load i64, ptr %d1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%d.load20", 7
  br i1 %cmpeqtmp21, label %then_172, label %merge_173

merge_173:                                        ; preds = %merge_171
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 8
  br i1 %cmpeqtmp24, label %then_174, label %merge_175

merge_175:                                        ; preds = %merge_173
  %"%d.load26" = load i64, ptr %d1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%d.load26", 9
  br i1 %cmpeqtmp27, label %then_176, label %merge_177

merge_177:                                        ; preds = %merge_175
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 10
  br i1 %cmpeqtmp30, label %then_178, label %merge_179

merge_179:                                        ; preds = %merge_177
  %"%d.load32" = load i64, ptr %d1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%d.load32", 11
  br i1 %cmpeqtmp33, label %then_180, label %merge_181

merge_181:                                        ; preds = %merge_179
  %"%d.load35" = load i64, ptr %d1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%d.load35", 12
  br i1 %cmpeqtmp36, label %then_182, label %merge_183

merge_183:                                        ; preds = %merge_181
  %"%d.load38" = load i64, ptr %d1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%d.load38", 13
  br i1 %cmpeqtmp39, label %then_184, label %merge_185

merge_185:                                        ; preds = %merge_183
  %"%d.load41" = load i64, ptr %d1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%d.load41", 14
  br i1 %cmpeqtmp42, label %then_186, label %merge_187

merge_187:                                        ; preds = %merge_185
  %"%d.load44" = load i64, ptr %d1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%d.load44", 15
  br i1 %cmpeqtmp45, label %then_188, label %merge_189

merge_189:                                        ; preds = %merge_187
  %"%d.load47" = load i64, ptr %d1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%d.load47", 16
  br i1 %cmpeqtmp48, label %then_190, label %merge_191

merge_191:                                        ; preds = %merge_189
  %"%d.load50" = load i64, ptr %d1, align 8
  %cmpeqtmp51 = icmp eq i64 %"%d.load50", 17
  br i1 %cmpeqtmp51, label %then_192, label %merge_193

merge_193:                                        ; preds = %merge_191
  %"%d.load53" = load i64, ptr %d1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%d.load53", 18
  br i1 %cmpeqtmp54, label %then_194, label %merge_195

merge_195:                                        ; preds = %merge_193
  %"%d.load56" = load i64, ptr %d1, align 8
  %cmpeqtmp57 = icmp eq i64 %"%d.load56", 19
  br i1 %cmpeqtmp57, label %then_196, label %merge_197

merge_197:                                        ; preds = %merge_195
  %"%d.load59" = load i64, ptr %d1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%d.load59", 20
  br i1 %cmpeqtmp60, label %then_198, label %merge_199

merge_199:                                        ; preds = %merge_197
  %"%d.load62" = load i64, ptr %d1, align 8
  %cmpeqtmp63 = icmp eq i64 %"%d.load62", 21
  br i1 %cmpeqtmp63, label %then_200, label %merge_201

merge_201:                                        ; preds = %merge_199
  %"%d.load65" = load i64, ptr %d1, align 8
  %cmpeqtmp66 = icmp eq i64 %"%d.load65", 22
  br i1 %cmpeqtmp66, label %then_202, label %merge_203

merge_203:                                        ; preds = %merge_201
  %"%d.load68" = load i64, ptr %d1, align 8
  %cmpeqtmp69 = icmp eq i64 %"%d.load68", 23
  br i1 %cmpeqtmp69, label %then_204, label %merge_205

merge_205:                                        ; preds = %merge_203
  %"%d.load71" = load i64, ptr %d1, align 8
  %cmpeqtmp72 = icmp eq i64 %"%d.load71", 24
  br i1 %cmpeqtmp72, label %then_206, label %merge_207

merge_207:                                        ; preds = %merge_205
  %"%d.load74" = load i64, ptr %d1, align 8
  %cmpeqtmp75 = icmp eq i64 %"%d.load74", 25
  br i1 %cmpeqtmp75, label %then_208, label %merge_209

merge_209:                                        ; preds = %merge_207
  %"%d.load77" = load i64, ptr %d1, align 8
  %cmpeqtmp78 = icmp eq i64 %"%d.load77", 26
  br i1 %cmpeqtmp78, label %then_210, label %merge_211

merge_211:                                        ; preds = %merge_209
  %"%d.load80" = load i64, ptr %d1, align 8
  %cmpeqtmp81 = icmp eq i64 %"%d.load80", 27
  br i1 %cmpeqtmp81, label %then_212, label %merge_213

merge_213:                                        ; preds = %merge_211
  %"%d.load83" = load i64, ptr %d1, align 8
  %cmpeqtmp84 = icmp eq i64 %"%d.load83", 28
  br i1 %cmpeqtmp84, label %then_214, label %merge_215

merge_215:                                        ; preds = %merge_213
  %"%d.load86" = load i64, ptr %d1, align 8
  %cmpeqtmp87 = icmp eq i64 %"%d.load86", 29
  br i1 %cmpeqtmp87, label %then_216, label %merge_217

merge_217:                                        ; preds = %merge_215
  %"%d.load89" = load i64, ptr %d1, align 8
  %cmpeqtmp90 = icmp eq i64 %"%d.load89", 30
  br i1 %cmpeqtmp90, label %then_218, label %merge_219

merge_219:                                        ; preds = %merge_217
  %"%d.load92" = load i64, ptr %d1, align 8
  %cmpeqtmp93 = icmp eq i64 %"%d.load92", 31
  br i1 %cmpeqtmp93, label %then_220, label %merge_221

merge_221:                                        ; preds = %merge_219
  ret i64 0

then_158:                                         ; preds = %entry
  %loadtmp = load i64, ptr @dev_state_0, align 8
  ret i64 %loadtmp

then_160:                                         ; preds = %merge_159
  %loadtmp4 = load i64, ptr @dev_state_1, align 8
  ret i64 %loadtmp4

then_162:                                         ; preds = %merge_161
  %loadtmp7 = load i64, ptr @dev_state_2, align 8
  ret i64 %loadtmp7

then_164:                                         ; preds = %merge_163
  %loadtmp10 = load i64, ptr @dev_state_3, align 8
  ret i64 %loadtmp10

then_166:                                         ; preds = %merge_165
  %loadtmp13 = load i64, ptr @dev_state_4, align 8
  ret i64 %loadtmp13

then_168:                                         ; preds = %merge_167
  %loadtmp16 = load i64, ptr @dev_state_5, align 8
  ret i64 %loadtmp16

then_170:                                         ; preds = %merge_169
  %loadtmp19 = load i64, ptr @dev_state_6, align 8
  ret i64 %loadtmp19

then_172:                                         ; preds = %merge_171
  %loadtmp22 = load i64, ptr @dev_state_7, align 8
  ret i64 %loadtmp22

then_174:                                         ; preds = %merge_173
  %loadtmp25 = load i64, ptr @dev_state_8, align 8
  ret i64 %loadtmp25

then_176:                                         ; preds = %merge_175
  %loadtmp28 = load i64, ptr @dev_state_9, align 8
  ret i64 %loadtmp28

then_178:                                         ; preds = %merge_177
  %loadtmp31 = load i64, ptr @dev_state_10, align 8
  ret i64 %loadtmp31

then_180:                                         ; preds = %merge_179
  %loadtmp34 = load i64, ptr @dev_state_11, align 8
  ret i64 %loadtmp34

then_182:                                         ; preds = %merge_181
  %loadtmp37 = load i64, ptr @dev_state_12, align 8
  ret i64 %loadtmp37

then_184:                                         ; preds = %merge_183
  %loadtmp40 = load i64, ptr @dev_state_13, align 8
  ret i64 %loadtmp40

then_186:                                         ; preds = %merge_185
  %loadtmp43 = load i64, ptr @dev_state_14, align 8
  ret i64 %loadtmp43

then_188:                                         ; preds = %merge_187
  %loadtmp46 = load i64, ptr @dev_state_15, align 8
  ret i64 %loadtmp46

then_190:                                         ; preds = %merge_189
  %loadtmp49 = load i64, ptr @dev_state_16, align 8
  ret i64 %loadtmp49

then_192:                                         ; preds = %merge_191
  %loadtmp52 = load i64, ptr @dev_state_17, align 8
  ret i64 %loadtmp52

then_194:                                         ; preds = %merge_193
  %loadtmp55 = load i64, ptr @dev_state_18, align 8
  ret i64 %loadtmp55

then_196:                                         ; preds = %merge_195
  %loadtmp58 = load i64, ptr @dev_state_19, align 8
  ret i64 %loadtmp58

then_198:                                         ; preds = %merge_197
  %loadtmp61 = load i64, ptr @dev_state_20, align 8
  ret i64 %loadtmp61

then_200:                                         ; preds = %merge_199
  %loadtmp64 = load i64, ptr @dev_state_21, align 8
  ret i64 %loadtmp64

then_202:                                         ; preds = %merge_201
  %loadtmp67 = load i64, ptr @dev_state_22, align 8
  ret i64 %loadtmp67

then_204:                                         ; preds = %merge_203
  %loadtmp70 = load i64, ptr @dev_state_23, align 8
  ret i64 %loadtmp70

then_206:                                         ; preds = %merge_205
  %loadtmp73 = load i64, ptr @dev_state_24, align 8
  ret i64 %loadtmp73

then_208:                                         ; preds = %merge_207
  %loadtmp76 = load i64, ptr @dev_state_25, align 8
  ret i64 %loadtmp76

then_210:                                         ; preds = %merge_209
  %loadtmp79 = load i64, ptr @dev_state_26, align 8
  ret i64 %loadtmp79

then_212:                                         ; preds = %merge_211
  %loadtmp82 = load i64, ptr @dev_state_27, align 8
  ret i64 %loadtmp82

then_214:                                         ; preds = %merge_213
  %loadtmp85 = load i64, ptr @dev_state_28, align 8
  ret i64 %loadtmp85

then_216:                                         ; preds = %merge_215
  %loadtmp88 = load i64, ptr @dev_state_29, align 8
  ret i64 %loadtmp88

then_218:                                         ; preds = %merge_217
  %loadtmp91 = load i64, ptr @dev_state_30, align 8
  ret i64 %loadtmp91

then_220:                                         ; preds = %merge_219
  %loadtmp94 = load i64, ptr @dev_state_31, align 8
  ret i64 %loadtmp94
}

define i64 @dm_set_state(i64 %d, i64 %val) {
entry:
  %"%dev_state_31" = alloca i64, align 8
  %"%dev_state_30" = alloca i64, align 8
  %"%dev_state_29" = alloca i64, align 8
  %"%dev_state_28" = alloca i64, align 8
  %"%dev_state_27" = alloca i64, align 8
  %"%dev_state_26" = alloca i64, align 8
  %"%dev_state_25" = alloca i64, align 8
  %"%dev_state_24" = alloca i64, align 8
  %"%dev_state_23" = alloca i64, align 8
  %"%dev_state_22" = alloca i64, align 8
  %"%dev_state_21" = alloca i64, align 8
  %"%dev_state_20" = alloca i64, align 8
  %"%dev_state_19" = alloca i64, align 8
  %"%dev_state_18" = alloca i64, align 8
  %"%dev_state_17" = alloca i64, align 8
  %"%dev_state_16" = alloca i64, align 8
  %"%dev_state_15" = alloca i64, align 8
  %"%dev_state_14" = alloca i64, align 8
  %"%dev_state_13" = alloca i64, align 8
  %"%dev_state_12" = alloca i64, align 8
  %"%dev_state_11" = alloca i64, align 8
  %"%dev_state_10" = alloca i64, align 8
  %"%dev_state_9" = alloca i64, align 8
  %"%dev_state_8" = alloca i64, align 8
  %"%dev_state_7" = alloca i64, align 8
  %"%dev_state_6" = alloca i64, align 8
  %"%dev_state_5" = alloca i64, align 8
  %"%dev_state_4" = alloca i64, align 8
  %"%dev_state_3" = alloca i64, align 8
  %"%dev_state_2" = alloca i64, align 8
  %"%dev_state_1" = alloca i64, align 8
  %"%dev_state_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_222, label %merge_223

merge_223:                                        ; preds = %then_222, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_224, label %merge_225

merge_225:                                        ; preds = %then_224, %merge_223
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_226, label %merge_227

merge_227:                                        ; preds = %then_226, %merge_225
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_228, label %merge_229

merge_229:                                        ; preds = %then_228, %merge_227
  %"%d.load9" = load i64, ptr %d1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%d.load9", 4
  br i1 %cmpeqtmp10, label %then_230, label %merge_231

merge_231:                                        ; preds = %then_230, %merge_229
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 5
  br i1 %cmpeqtmp12, label %then_232, label %merge_233

merge_233:                                        ; preds = %then_232, %merge_231
  %"%d.load13" = load i64, ptr %d1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%d.load13", 6
  br i1 %cmpeqtmp14, label %then_234, label %merge_235

merge_235:                                        ; preds = %then_234, %merge_233
  %"%d.load15" = load i64, ptr %d1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%d.load15", 7
  br i1 %cmpeqtmp16, label %then_236, label %merge_237

merge_237:                                        ; preds = %then_236, %merge_235
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 8
  br i1 %cmpeqtmp18, label %then_238, label %merge_239

merge_239:                                        ; preds = %then_238, %merge_237
  %"%d.load19" = load i64, ptr %d1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%d.load19", 9
  br i1 %cmpeqtmp20, label %then_240, label %merge_241

merge_241:                                        ; preds = %then_240, %merge_239
  %"%d.load21" = load i64, ptr %d1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%d.load21", 10
  br i1 %cmpeqtmp22, label %then_242, label %merge_243

merge_243:                                        ; preds = %then_242, %merge_241
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 11
  br i1 %cmpeqtmp24, label %then_244, label %merge_245

merge_245:                                        ; preds = %then_244, %merge_243
  %"%d.load25" = load i64, ptr %d1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%d.load25", 12
  br i1 %cmpeqtmp26, label %then_246, label %merge_247

merge_247:                                        ; preds = %then_246, %merge_245
  %"%d.load27" = load i64, ptr %d1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%d.load27", 13
  br i1 %cmpeqtmp28, label %then_248, label %merge_249

merge_249:                                        ; preds = %then_248, %merge_247
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 14
  br i1 %cmpeqtmp30, label %then_250, label %merge_251

merge_251:                                        ; preds = %then_250, %merge_249
  %"%d.load31" = load i64, ptr %d1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%d.load31", 15
  br i1 %cmpeqtmp32, label %then_252, label %merge_253

merge_253:                                        ; preds = %then_252, %merge_251
  %"%d.load33" = load i64, ptr %d1, align 8
  %cmpeqtmp34 = icmp eq i64 %"%d.load33", 16
  br i1 %cmpeqtmp34, label %then_254, label %merge_255

merge_255:                                        ; preds = %then_254, %merge_253
  %"%d.load35" = load i64, ptr %d1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%d.load35", 17
  br i1 %cmpeqtmp36, label %then_256, label %merge_257

merge_257:                                        ; preds = %then_256, %merge_255
  %"%d.load37" = load i64, ptr %d1, align 8
  %cmpeqtmp38 = icmp eq i64 %"%d.load37", 18
  br i1 %cmpeqtmp38, label %then_258, label %merge_259

merge_259:                                        ; preds = %then_258, %merge_257
  %"%d.load39" = load i64, ptr %d1, align 8
  %cmpeqtmp40 = icmp eq i64 %"%d.load39", 19
  br i1 %cmpeqtmp40, label %then_260, label %merge_261

merge_261:                                        ; preds = %then_260, %merge_259
  %"%d.load41" = load i64, ptr %d1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%d.load41", 20
  br i1 %cmpeqtmp42, label %then_262, label %merge_263

merge_263:                                        ; preds = %then_262, %merge_261
  %"%d.load43" = load i64, ptr %d1, align 8
  %cmpeqtmp44 = icmp eq i64 %"%d.load43", 21
  br i1 %cmpeqtmp44, label %then_264, label %merge_265

merge_265:                                        ; preds = %then_264, %merge_263
  %"%d.load45" = load i64, ptr %d1, align 8
  %cmpeqtmp46 = icmp eq i64 %"%d.load45", 22
  br i1 %cmpeqtmp46, label %then_266, label %merge_267

merge_267:                                        ; preds = %then_266, %merge_265
  %"%d.load47" = load i64, ptr %d1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%d.load47", 23
  br i1 %cmpeqtmp48, label %then_268, label %merge_269

merge_269:                                        ; preds = %then_268, %merge_267
  %"%d.load49" = load i64, ptr %d1, align 8
  %cmpeqtmp50 = icmp eq i64 %"%d.load49", 24
  br i1 %cmpeqtmp50, label %then_270, label %merge_271

merge_271:                                        ; preds = %then_270, %merge_269
  %"%d.load51" = load i64, ptr %d1, align 8
  %cmpeqtmp52 = icmp eq i64 %"%d.load51", 25
  br i1 %cmpeqtmp52, label %then_272, label %merge_273

merge_273:                                        ; preds = %then_272, %merge_271
  %"%d.load53" = load i64, ptr %d1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%d.load53", 26
  br i1 %cmpeqtmp54, label %then_274, label %merge_275

merge_275:                                        ; preds = %then_274, %merge_273
  %"%d.load55" = load i64, ptr %d1, align 8
  %cmpeqtmp56 = icmp eq i64 %"%d.load55", 27
  br i1 %cmpeqtmp56, label %then_276, label %merge_277

merge_277:                                        ; preds = %then_276, %merge_275
  %"%d.load57" = load i64, ptr %d1, align 8
  %cmpeqtmp58 = icmp eq i64 %"%d.load57", 28
  br i1 %cmpeqtmp58, label %then_278, label %merge_279

merge_279:                                        ; preds = %then_278, %merge_277
  %"%d.load59" = load i64, ptr %d1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%d.load59", 29
  br i1 %cmpeqtmp60, label %then_280, label %merge_281

merge_281:                                        ; preds = %then_280, %merge_279
  %"%d.load61" = load i64, ptr %d1, align 8
  %cmpeqtmp62 = icmp eq i64 %"%d.load61", 30
  br i1 %cmpeqtmp62, label %then_282, label %merge_283

merge_283:                                        ; preds = %then_282, %merge_281
  %"%d.load63" = load i64, ptr %d1, align 8
  %cmpeqtmp64 = icmp eq i64 %"%d.load63", 31
  br i1 %cmpeqtmp64, label %then_284, label %merge_285

merge_285:                                        ; preds = %then_284, %merge_283
  ret i64 0

then_222:                                         ; preds = %entry
  %"%567.load" = load i64, ptr %val2, align 8
  store i64 %"%567.load", ptr %"%dev_state_0", align 8
  br label %merge_223

then_224:                                         ; preds = %merge_223
  %"%571.load" = load i64, ptr %val2, align 8
  store i64 %"%571.load", ptr %"%dev_state_1", align 8
  br label %merge_225

then_226:                                         ; preds = %merge_225
  %"%575.load" = load i64, ptr %val2, align 8
  store i64 %"%575.load", ptr %"%dev_state_2", align 8
  br label %merge_227

then_228:                                         ; preds = %merge_227
  %"%579.load" = load i64, ptr %val2, align 8
  store i64 %"%579.load", ptr %"%dev_state_3", align 8
  br label %merge_229

then_230:                                         ; preds = %merge_229
  %"%583.load" = load i64, ptr %val2, align 8
  store i64 %"%583.load", ptr %"%dev_state_4", align 8
  br label %merge_231

then_232:                                         ; preds = %merge_231
  %"%587.load" = load i64, ptr %val2, align 8
  store i64 %"%587.load", ptr %"%dev_state_5", align 8
  br label %merge_233

then_234:                                         ; preds = %merge_233
  %"%591.load" = load i64, ptr %val2, align 8
  store i64 %"%591.load", ptr %"%dev_state_6", align 8
  br label %merge_235

then_236:                                         ; preds = %merge_235
  %"%595.load" = load i64, ptr %val2, align 8
  store i64 %"%595.load", ptr %"%dev_state_7", align 8
  br label %merge_237

then_238:                                         ; preds = %merge_237
  %"%599.load" = load i64, ptr %val2, align 8
  store i64 %"%599.load", ptr %"%dev_state_8", align 8
  br label %merge_239

then_240:                                         ; preds = %merge_239
  %"%603.load" = load i64, ptr %val2, align 8
  store i64 %"%603.load", ptr %"%dev_state_9", align 8
  br label %merge_241

then_242:                                         ; preds = %merge_241
  %"%607.load" = load i64, ptr %val2, align 8
  store i64 %"%607.load", ptr %"%dev_state_10", align 8
  br label %merge_243

then_244:                                         ; preds = %merge_243
  %"%611.load" = load i64, ptr %val2, align 8
  store i64 %"%611.load", ptr %"%dev_state_11", align 8
  br label %merge_245

then_246:                                         ; preds = %merge_245
  %"%615.load" = load i64, ptr %val2, align 8
  store i64 %"%615.load", ptr %"%dev_state_12", align 8
  br label %merge_247

then_248:                                         ; preds = %merge_247
  %"%619.load" = load i64, ptr %val2, align 8
  store i64 %"%619.load", ptr %"%dev_state_13", align 8
  br label %merge_249

then_250:                                         ; preds = %merge_249
  %"%623.load" = load i64, ptr %val2, align 8
  store i64 %"%623.load", ptr %"%dev_state_14", align 8
  br label %merge_251

then_252:                                         ; preds = %merge_251
  %"%627.load" = load i64, ptr %val2, align 8
  store i64 %"%627.load", ptr %"%dev_state_15", align 8
  br label %merge_253

then_254:                                         ; preds = %merge_253
  %"%631.load" = load i64, ptr %val2, align 8
  store i64 %"%631.load", ptr %"%dev_state_16", align 8
  br label %merge_255

then_256:                                         ; preds = %merge_255
  %"%635.load" = load i64, ptr %val2, align 8
  store i64 %"%635.load", ptr %"%dev_state_17", align 8
  br label %merge_257

then_258:                                         ; preds = %merge_257
  %"%639.load" = load i64, ptr %val2, align 8
  store i64 %"%639.load", ptr %"%dev_state_18", align 8
  br label %merge_259

then_260:                                         ; preds = %merge_259
  %"%643.load" = load i64, ptr %val2, align 8
  store i64 %"%643.load", ptr %"%dev_state_19", align 8
  br label %merge_261

then_262:                                         ; preds = %merge_261
  %"%647.load" = load i64, ptr %val2, align 8
  store i64 %"%647.load", ptr %"%dev_state_20", align 8
  br label %merge_263

then_264:                                         ; preds = %merge_263
  %"%651.load" = load i64, ptr %val2, align 8
  store i64 %"%651.load", ptr %"%dev_state_21", align 8
  br label %merge_265

then_266:                                         ; preds = %merge_265
  %"%655.load" = load i64, ptr %val2, align 8
  store i64 %"%655.load", ptr %"%dev_state_22", align 8
  br label %merge_267

then_268:                                         ; preds = %merge_267
  %"%659.load" = load i64, ptr %val2, align 8
  store i64 %"%659.load", ptr %"%dev_state_23", align 8
  br label %merge_269

then_270:                                         ; preds = %merge_269
  %"%663.load" = load i64, ptr %val2, align 8
  store i64 %"%663.load", ptr %"%dev_state_24", align 8
  br label %merge_271

then_272:                                         ; preds = %merge_271
  %"%667.load" = load i64, ptr %val2, align 8
  store i64 %"%667.load", ptr %"%dev_state_25", align 8
  br label %merge_273

then_274:                                         ; preds = %merge_273
  %"%671.load" = load i64, ptr %val2, align 8
  store i64 %"%671.load", ptr %"%dev_state_26", align 8
  br label %merge_275

then_276:                                         ; preds = %merge_275
  %"%675.load" = load i64, ptr %val2, align 8
  store i64 %"%675.load", ptr %"%dev_state_27", align 8
  br label %merge_277

then_278:                                         ; preds = %merge_277
  %"%679.load" = load i64, ptr %val2, align 8
  store i64 %"%679.load", ptr %"%dev_state_28", align 8
  br label %merge_279

then_280:                                         ; preds = %merge_279
  %"%683.load" = load i64, ptr %val2, align 8
  store i64 %"%683.load", ptr %"%dev_state_29", align 8
  br label %merge_281

then_282:                                         ; preds = %merge_281
  %"%687.load" = load i64, ptr %val2, align 8
  store i64 %"%687.load", ptr %"%dev_state_30", align 8
  br label %merge_283

then_284:                                         ; preds = %merge_283
  %"%691.load" = load i64, ptr %val2, align 8
  store i64 %"%691.load", ptr %"%dev_state_31", align 8
  br label %merge_285
}

define i64 @dm_set_vid(i64 %d, i64 %val) {
entry:
  %"%dev_vid_15" = alloca i64, align 8
  %"%dev_vid_14" = alloca i64, align 8
  %"%dev_vid_13" = alloca i64, align 8
  %"%dev_vid_12" = alloca i64, align 8
  %"%dev_vid_11" = alloca i64, align 8
  %"%dev_vid_10" = alloca i64, align 8
  %"%dev_vid_9" = alloca i64, align 8
  %"%dev_vid_8" = alloca i64, align 8
  %"%dev_vid_7" = alloca i64, align 8
  %"%dev_vid_6" = alloca i64, align 8
  %"%dev_vid_5" = alloca i64, align 8
  %"%dev_vid_4" = alloca i64, align 8
  %"%dev_vid_3" = alloca i64, align 8
  %"%dev_vid_2" = alloca i64, align 8
  %"%dev_vid_1" = alloca i64, align 8
  %"%dev_vid_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_286, label %merge_287

merge_287:                                        ; preds = %then_286, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_288, label %merge_289

merge_289:                                        ; preds = %then_288, %merge_287
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_290, label %merge_291

merge_291:                                        ; preds = %then_290, %merge_289
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_292, label %merge_293

merge_293:                                        ; preds = %then_292, %merge_291
  %"%d.load9" = load i64, ptr %d1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%d.load9", 4
  br i1 %cmpeqtmp10, label %then_294, label %merge_295

merge_295:                                        ; preds = %then_294, %merge_293
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 5
  br i1 %cmpeqtmp12, label %then_296, label %merge_297

merge_297:                                        ; preds = %then_296, %merge_295
  %"%d.load13" = load i64, ptr %d1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%d.load13", 6
  br i1 %cmpeqtmp14, label %then_298, label %merge_299

merge_299:                                        ; preds = %then_298, %merge_297
  %"%d.load15" = load i64, ptr %d1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%d.load15", 7
  br i1 %cmpeqtmp16, label %then_300, label %merge_301

merge_301:                                        ; preds = %then_300, %merge_299
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 8
  br i1 %cmpeqtmp18, label %then_302, label %merge_303

merge_303:                                        ; preds = %then_302, %merge_301
  %"%d.load19" = load i64, ptr %d1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%d.load19", 9
  br i1 %cmpeqtmp20, label %then_304, label %merge_305

merge_305:                                        ; preds = %then_304, %merge_303
  %"%d.load21" = load i64, ptr %d1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%d.load21", 10
  br i1 %cmpeqtmp22, label %then_306, label %merge_307

merge_307:                                        ; preds = %then_306, %merge_305
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 11
  br i1 %cmpeqtmp24, label %then_308, label %merge_309

merge_309:                                        ; preds = %then_308, %merge_307
  %"%d.load25" = load i64, ptr %d1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%d.load25", 12
  br i1 %cmpeqtmp26, label %then_310, label %merge_311

merge_311:                                        ; preds = %then_310, %merge_309
  %"%d.load27" = load i64, ptr %d1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%d.load27", 13
  br i1 %cmpeqtmp28, label %then_312, label %merge_313

merge_313:                                        ; preds = %then_312, %merge_311
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 14
  br i1 %cmpeqtmp30, label %then_314, label %merge_315

merge_315:                                        ; preds = %then_314, %merge_313
  %"%d.load31" = load i64, ptr %d1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%d.load31", 15
  br i1 %cmpeqtmp32, label %then_316, label %merge_317

merge_317:                                        ; preds = %then_316, %merge_315
  ret i64 0

then_286:                                         ; preds = %entry
  %"%696.load" = load i64, ptr %val2, align 8
  store i64 %"%696.load", ptr %"%dev_vid_0", align 8
  br label %merge_287

then_288:                                         ; preds = %merge_287
  %"%700.load" = load i64, ptr %val2, align 8
  store i64 %"%700.load", ptr %"%dev_vid_1", align 8
  br label %merge_289

then_290:                                         ; preds = %merge_289
  %"%704.load" = load i64, ptr %val2, align 8
  store i64 %"%704.load", ptr %"%dev_vid_2", align 8
  br label %merge_291

then_292:                                         ; preds = %merge_291
  %"%708.load" = load i64, ptr %val2, align 8
  store i64 %"%708.load", ptr %"%dev_vid_3", align 8
  br label %merge_293

then_294:                                         ; preds = %merge_293
  %"%712.load" = load i64, ptr %val2, align 8
  store i64 %"%712.load", ptr %"%dev_vid_4", align 8
  br label %merge_295

then_296:                                         ; preds = %merge_295
  %"%716.load" = load i64, ptr %val2, align 8
  store i64 %"%716.load", ptr %"%dev_vid_5", align 8
  br label %merge_297

then_298:                                         ; preds = %merge_297
  %"%720.load" = load i64, ptr %val2, align 8
  store i64 %"%720.load", ptr %"%dev_vid_6", align 8
  br label %merge_299

then_300:                                         ; preds = %merge_299
  %"%724.load" = load i64, ptr %val2, align 8
  store i64 %"%724.load", ptr %"%dev_vid_7", align 8
  br label %merge_301

then_302:                                         ; preds = %merge_301
  %"%728.load" = load i64, ptr %val2, align 8
  store i64 %"%728.load", ptr %"%dev_vid_8", align 8
  br label %merge_303

then_304:                                         ; preds = %merge_303
  %"%732.load" = load i64, ptr %val2, align 8
  store i64 %"%732.load", ptr %"%dev_vid_9", align 8
  br label %merge_305

then_306:                                         ; preds = %merge_305
  %"%736.load" = load i64, ptr %val2, align 8
  store i64 %"%736.load", ptr %"%dev_vid_10", align 8
  br label %merge_307

then_308:                                         ; preds = %merge_307
  %"%740.load" = load i64, ptr %val2, align 8
  store i64 %"%740.load", ptr %"%dev_vid_11", align 8
  br label %merge_309

then_310:                                         ; preds = %merge_309
  %"%744.load" = load i64, ptr %val2, align 8
  store i64 %"%744.load", ptr %"%dev_vid_12", align 8
  br label %merge_311

then_312:                                         ; preds = %merge_311
  %"%748.load" = load i64, ptr %val2, align 8
  store i64 %"%748.load", ptr %"%dev_vid_13", align 8
  br label %merge_313

then_314:                                         ; preds = %merge_313
  %"%752.load" = load i64, ptr %val2, align 8
  store i64 %"%752.load", ptr %"%dev_vid_14", align 8
  br label %merge_315

then_316:                                         ; preds = %merge_315
  %"%756.load" = load i64, ptr %val2, align 8
  store i64 %"%756.load", ptr %"%dev_vid_15", align 8
  br label %merge_317
}

define i64 @dm_set_did(i64 %d, i64 %val) {
entry:
  %"%dev_did_15" = alloca i64, align 8
  %"%dev_did_14" = alloca i64, align 8
  %"%dev_did_13" = alloca i64, align 8
  %"%dev_did_12" = alloca i64, align 8
  %"%dev_did_11" = alloca i64, align 8
  %"%dev_did_10" = alloca i64, align 8
  %"%dev_did_9" = alloca i64, align 8
  %"%dev_did_8" = alloca i64, align 8
  %"%dev_did_7" = alloca i64, align 8
  %"%dev_did_6" = alloca i64, align 8
  %"%dev_did_5" = alloca i64, align 8
  %"%dev_did_4" = alloca i64, align 8
  %"%dev_did_3" = alloca i64, align 8
  %"%dev_did_2" = alloca i64, align 8
  %"%dev_did_1" = alloca i64, align 8
  %"%dev_did_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_318, label %merge_319

merge_319:                                        ; preds = %then_318, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_320, label %merge_321

merge_321:                                        ; preds = %then_320, %merge_319
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_322, label %merge_323

merge_323:                                        ; preds = %then_322, %merge_321
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_324, label %merge_325

merge_325:                                        ; preds = %then_324, %merge_323
  %"%d.load9" = load i64, ptr %d1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%d.load9", 4
  br i1 %cmpeqtmp10, label %then_326, label %merge_327

merge_327:                                        ; preds = %then_326, %merge_325
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 5
  br i1 %cmpeqtmp12, label %then_328, label %merge_329

merge_329:                                        ; preds = %then_328, %merge_327
  %"%d.load13" = load i64, ptr %d1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%d.load13", 6
  br i1 %cmpeqtmp14, label %then_330, label %merge_331

merge_331:                                        ; preds = %then_330, %merge_329
  %"%d.load15" = load i64, ptr %d1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%d.load15", 7
  br i1 %cmpeqtmp16, label %then_332, label %merge_333

merge_333:                                        ; preds = %then_332, %merge_331
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 8
  br i1 %cmpeqtmp18, label %then_334, label %merge_335

merge_335:                                        ; preds = %then_334, %merge_333
  %"%d.load19" = load i64, ptr %d1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%d.load19", 9
  br i1 %cmpeqtmp20, label %then_336, label %merge_337

merge_337:                                        ; preds = %then_336, %merge_335
  %"%d.load21" = load i64, ptr %d1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%d.load21", 10
  br i1 %cmpeqtmp22, label %then_338, label %merge_339

merge_339:                                        ; preds = %then_338, %merge_337
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 11
  br i1 %cmpeqtmp24, label %then_340, label %merge_341

merge_341:                                        ; preds = %then_340, %merge_339
  %"%d.load25" = load i64, ptr %d1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%d.load25", 12
  br i1 %cmpeqtmp26, label %then_342, label %merge_343

merge_343:                                        ; preds = %then_342, %merge_341
  %"%d.load27" = load i64, ptr %d1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%d.load27", 13
  br i1 %cmpeqtmp28, label %then_344, label %merge_345

merge_345:                                        ; preds = %then_344, %merge_343
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 14
  br i1 %cmpeqtmp30, label %then_346, label %merge_347

merge_347:                                        ; preds = %then_346, %merge_345
  %"%d.load31" = load i64, ptr %d1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%d.load31", 15
  br i1 %cmpeqtmp32, label %then_348, label %merge_349

merge_349:                                        ; preds = %then_348, %merge_347
  ret i64 0

then_318:                                         ; preds = %entry
  %"%761.load" = load i64, ptr %val2, align 8
  store i64 %"%761.load", ptr %"%dev_did_0", align 8
  br label %merge_319

then_320:                                         ; preds = %merge_319
  %"%765.load" = load i64, ptr %val2, align 8
  store i64 %"%765.load", ptr %"%dev_did_1", align 8
  br label %merge_321

then_322:                                         ; preds = %merge_321
  %"%769.load" = load i64, ptr %val2, align 8
  store i64 %"%769.load", ptr %"%dev_did_2", align 8
  br label %merge_323

then_324:                                         ; preds = %merge_323
  %"%773.load" = load i64, ptr %val2, align 8
  store i64 %"%773.load", ptr %"%dev_did_3", align 8
  br label %merge_325

then_326:                                         ; preds = %merge_325
  %"%777.load" = load i64, ptr %val2, align 8
  store i64 %"%777.load", ptr %"%dev_did_4", align 8
  br label %merge_327

then_328:                                         ; preds = %merge_327
  %"%781.load" = load i64, ptr %val2, align 8
  store i64 %"%781.load", ptr %"%dev_did_5", align 8
  br label %merge_329

then_330:                                         ; preds = %merge_329
  %"%785.load" = load i64, ptr %val2, align 8
  store i64 %"%785.load", ptr %"%dev_did_6", align 8
  br label %merge_331

then_332:                                         ; preds = %merge_331
  %"%789.load" = load i64, ptr %val2, align 8
  store i64 %"%789.load", ptr %"%dev_did_7", align 8
  br label %merge_333

then_334:                                         ; preds = %merge_333
  %"%793.load" = load i64, ptr %val2, align 8
  store i64 %"%793.load", ptr %"%dev_did_8", align 8
  br label %merge_335

then_336:                                         ; preds = %merge_335
  %"%797.load" = load i64, ptr %val2, align 8
  store i64 %"%797.load", ptr %"%dev_did_9", align 8
  br label %merge_337

then_338:                                         ; preds = %merge_337
  %"%801.load" = load i64, ptr %val2, align 8
  store i64 %"%801.load", ptr %"%dev_did_10", align 8
  br label %merge_339

then_340:                                         ; preds = %merge_339
  %"%805.load" = load i64, ptr %val2, align 8
  store i64 %"%805.load", ptr %"%dev_did_11", align 8
  br label %merge_341

then_342:                                         ; preds = %merge_341
  %"%809.load" = load i64, ptr %val2, align 8
  store i64 %"%809.load", ptr %"%dev_did_12", align 8
  br label %merge_343

then_344:                                         ; preds = %merge_343
  %"%813.load" = load i64, ptr %val2, align 8
  store i64 %"%813.load", ptr %"%dev_did_13", align 8
  br label %merge_345

then_346:                                         ; preds = %merge_345
  %"%817.load" = load i64, ptr %val2, align 8
  store i64 %"%817.load", ptr %"%dev_did_14", align 8
  br label %merge_347

then_348:                                         ; preds = %merge_347
  %"%821.load" = load i64, ptr %val2, align 8
  store i64 %"%821.load", ptr %"%dev_did_15", align 8
  br label %merge_349
}

define i64 @dm_set_parent(i64 %d, i64 %val) {
entry:
  %"%dev_parent_15" = alloca i64, align 8
  %"%dev_parent_14" = alloca i64, align 8
  %"%dev_parent_13" = alloca i64, align 8
  %"%dev_parent_12" = alloca i64, align 8
  %"%dev_parent_11" = alloca i64, align 8
  %"%dev_parent_10" = alloca i64, align 8
  %"%dev_parent_9" = alloca i64, align 8
  %"%dev_parent_8" = alloca i64, align 8
  %"%dev_parent_7" = alloca i64, align 8
  %"%dev_parent_6" = alloca i64, align 8
  %"%dev_parent_5" = alloca i64, align 8
  %"%dev_parent_4" = alloca i64, align 8
  %"%dev_parent_3" = alloca i64, align 8
  %"%dev_parent_2" = alloca i64, align 8
  %"%dev_parent_1" = alloca i64, align 8
  %"%dev_parent_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_350, label %merge_351

merge_351:                                        ; preds = %then_350, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_352, label %merge_353

merge_353:                                        ; preds = %then_352, %merge_351
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_354, label %merge_355

merge_355:                                        ; preds = %then_354, %merge_353
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_356, label %merge_357

merge_357:                                        ; preds = %then_356, %merge_355
  %"%d.load9" = load i64, ptr %d1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%d.load9", 4
  br i1 %cmpeqtmp10, label %then_358, label %merge_359

merge_359:                                        ; preds = %then_358, %merge_357
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 5
  br i1 %cmpeqtmp12, label %then_360, label %merge_361

merge_361:                                        ; preds = %then_360, %merge_359
  %"%d.load13" = load i64, ptr %d1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%d.load13", 6
  br i1 %cmpeqtmp14, label %then_362, label %merge_363

merge_363:                                        ; preds = %then_362, %merge_361
  %"%d.load15" = load i64, ptr %d1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%d.load15", 7
  br i1 %cmpeqtmp16, label %then_364, label %merge_365

merge_365:                                        ; preds = %then_364, %merge_363
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 8
  br i1 %cmpeqtmp18, label %then_366, label %merge_367

merge_367:                                        ; preds = %then_366, %merge_365
  %"%d.load19" = load i64, ptr %d1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%d.load19", 9
  br i1 %cmpeqtmp20, label %then_368, label %merge_369

merge_369:                                        ; preds = %then_368, %merge_367
  %"%d.load21" = load i64, ptr %d1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%d.load21", 10
  br i1 %cmpeqtmp22, label %then_370, label %merge_371

merge_371:                                        ; preds = %then_370, %merge_369
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 11
  br i1 %cmpeqtmp24, label %then_372, label %merge_373

merge_373:                                        ; preds = %then_372, %merge_371
  %"%d.load25" = load i64, ptr %d1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%d.load25", 12
  br i1 %cmpeqtmp26, label %then_374, label %merge_375

merge_375:                                        ; preds = %then_374, %merge_373
  %"%d.load27" = load i64, ptr %d1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%d.load27", 13
  br i1 %cmpeqtmp28, label %then_376, label %merge_377

merge_377:                                        ; preds = %then_376, %merge_375
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 14
  br i1 %cmpeqtmp30, label %then_378, label %merge_379

merge_379:                                        ; preds = %then_378, %merge_377
  %"%d.load31" = load i64, ptr %d1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%d.load31", 15
  br i1 %cmpeqtmp32, label %then_380, label %merge_381

merge_381:                                        ; preds = %then_380, %merge_379
  ret i64 0

then_350:                                         ; preds = %entry
  %"%826.load" = load i64, ptr %val2, align 8
  store i64 %"%826.load", ptr %"%dev_parent_0", align 8
  br label %merge_351

then_352:                                         ; preds = %merge_351
  %"%830.load" = load i64, ptr %val2, align 8
  store i64 %"%830.load", ptr %"%dev_parent_1", align 8
  br label %merge_353

then_354:                                         ; preds = %merge_353
  %"%834.load" = load i64, ptr %val2, align 8
  store i64 %"%834.load", ptr %"%dev_parent_2", align 8
  br label %merge_355

then_356:                                         ; preds = %merge_355
  %"%838.load" = load i64, ptr %val2, align 8
  store i64 %"%838.load", ptr %"%dev_parent_3", align 8
  br label %merge_357

then_358:                                         ; preds = %merge_357
  %"%842.load" = load i64, ptr %val2, align 8
  store i64 %"%842.load", ptr %"%dev_parent_4", align 8
  br label %merge_359

then_360:                                         ; preds = %merge_359
  %"%846.load" = load i64, ptr %val2, align 8
  store i64 %"%846.load", ptr %"%dev_parent_5", align 8
  br label %merge_361

then_362:                                         ; preds = %merge_361
  %"%850.load" = load i64, ptr %val2, align 8
  store i64 %"%850.load", ptr %"%dev_parent_6", align 8
  br label %merge_363

then_364:                                         ; preds = %merge_363
  %"%854.load" = load i64, ptr %val2, align 8
  store i64 %"%854.load", ptr %"%dev_parent_7", align 8
  br label %merge_365

then_366:                                         ; preds = %merge_365
  %"%858.load" = load i64, ptr %val2, align 8
  store i64 %"%858.load", ptr %"%dev_parent_8", align 8
  br label %merge_367

then_368:                                         ; preds = %merge_367
  %"%862.load" = load i64, ptr %val2, align 8
  store i64 %"%862.load", ptr %"%dev_parent_9", align 8
  br label %merge_369

then_370:                                         ; preds = %merge_369
  %"%866.load" = load i64, ptr %val2, align 8
  store i64 %"%866.load", ptr %"%dev_parent_10", align 8
  br label %merge_371

then_372:                                         ; preds = %merge_371
  %"%870.load" = load i64, ptr %val2, align 8
  store i64 %"%870.load", ptr %"%dev_parent_11", align 8
  br label %merge_373

then_374:                                         ; preds = %merge_373
  %"%874.load" = load i64, ptr %val2, align 8
  store i64 %"%874.load", ptr %"%dev_parent_12", align 8
  br label %merge_375

then_376:                                         ; preds = %merge_375
  %"%878.load" = load i64, ptr %val2, align 8
  store i64 %"%878.load", ptr %"%dev_parent_13", align 8
  br label %merge_377

then_378:                                         ; preds = %merge_377
  %"%882.load" = load i64, ptr %val2, align 8
  store i64 %"%882.load", ptr %"%dev_parent_14", align 8
  br label %merge_379

then_380:                                         ; preds = %merge_379
  %"%886.load" = load i64, ptr %val2, align 8
  store i64 %"%886.load", ptr %"%dev_parent_15", align 8
  br label %merge_381
}

define i64 @dm_set_class(i64 %d, i64 %val) {
entry:
  %"%dev_class_15" = alloca i64, align 8
  %"%dev_class_14" = alloca i64, align 8
  %"%dev_class_13" = alloca i64, align 8
  %"%dev_class_12" = alloca i64, align 8
  %"%dev_class_11" = alloca i64, align 8
  %"%dev_class_10" = alloca i64, align 8
  %"%dev_class_9" = alloca i64, align 8
  %"%dev_class_8" = alloca i64, align 8
  %"%dev_class_7" = alloca i64, align 8
  %"%dev_class_6" = alloca i64, align 8
  %"%dev_class_5" = alloca i64, align 8
  %"%dev_class_4" = alloca i64, align 8
  %"%dev_class_3" = alloca i64, align 8
  %"%dev_class_2" = alloca i64, align 8
  %"%dev_class_1" = alloca i64, align 8
  %"%dev_class_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_382, label %merge_383

merge_383:                                        ; preds = %then_382, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_384, label %merge_385

merge_385:                                        ; preds = %then_384, %merge_383
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_386, label %merge_387

merge_387:                                        ; preds = %then_386, %merge_385
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_388, label %merge_389

merge_389:                                        ; preds = %then_388, %merge_387
  %"%d.load9" = load i64, ptr %d1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%d.load9", 4
  br i1 %cmpeqtmp10, label %then_390, label %merge_391

merge_391:                                        ; preds = %then_390, %merge_389
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 5
  br i1 %cmpeqtmp12, label %then_392, label %merge_393

merge_393:                                        ; preds = %then_392, %merge_391
  %"%d.load13" = load i64, ptr %d1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%d.load13", 6
  br i1 %cmpeqtmp14, label %then_394, label %merge_395

merge_395:                                        ; preds = %then_394, %merge_393
  %"%d.load15" = load i64, ptr %d1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%d.load15", 7
  br i1 %cmpeqtmp16, label %then_396, label %merge_397

merge_397:                                        ; preds = %then_396, %merge_395
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 8
  br i1 %cmpeqtmp18, label %then_398, label %merge_399

merge_399:                                        ; preds = %then_398, %merge_397
  %"%d.load19" = load i64, ptr %d1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%d.load19", 9
  br i1 %cmpeqtmp20, label %then_400, label %merge_401

merge_401:                                        ; preds = %then_400, %merge_399
  %"%d.load21" = load i64, ptr %d1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%d.load21", 10
  br i1 %cmpeqtmp22, label %then_402, label %merge_403

merge_403:                                        ; preds = %then_402, %merge_401
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 11
  br i1 %cmpeqtmp24, label %then_404, label %merge_405

merge_405:                                        ; preds = %then_404, %merge_403
  %"%d.load25" = load i64, ptr %d1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%d.load25", 12
  br i1 %cmpeqtmp26, label %then_406, label %merge_407

merge_407:                                        ; preds = %then_406, %merge_405
  %"%d.load27" = load i64, ptr %d1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%d.load27", 13
  br i1 %cmpeqtmp28, label %then_408, label %merge_409

merge_409:                                        ; preds = %then_408, %merge_407
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 14
  br i1 %cmpeqtmp30, label %then_410, label %merge_411

merge_411:                                        ; preds = %then_410, %merge_409
  %"%d.load31" = load i64, ptr %d1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%d.load31", 15
  br i1 %cmpeqtmp32, label %then_412, label %merge_413

merge_413:                                        ; preds = %then_412, %merge_411
  ret i64 0

then_382:                                         ; preds = %entry
  %"%891.load" = load i64, ptr %val2, align 8
  store i64 %"%891.load", ptr %"%dev_class_0", align 8
  br label %merge_383

then_384:                                         ; preds = %merge_383
  %"%895.load" = load i64, ptr %val2, align 8
  store i64 %"%895.load", ptr %"%dev_class_1", align 8
  br label %merge_385

then_386:                                         ; preds = %merge_385
  %"%899.load" = load i64, ptr %val2, align 8
  store i64 %"%899.load", ptr %"%dev_class_2", align 8
  br label %merge_387

then_388:                                         ; preds = %merge_387
  %"%903.load" = load i64, ptr %val2, align 8
  store i64 %"%903.load", ptr %"%dev_class_3", align 8
  br label %merge_389

then_390:                                         ; preds = %merge_389
  %"%907.load" = load i64, ptr %val2, align 8
  store i64 %"%907.load", ptr %"%dev_class_4", align 8
  br label %merge_391

then_392:                                         ; preds = %merge_391
  %"%911.load" = load i64, ptr %val2, align 8
  store i64 %"%911.load", ptr %"%dev_class_5", align 8
  br label %merge_393

then_394:                                         ; preds = %merge_393
  %"%915.load" = load i64, ptr %val2, align 8
  store i64 %"%915.load", ptr %"%dev_class_6", align 8
  br label %merge_395

then_396:                                         ; preds = %merge_395
  %"%919.load" = load i64, ptr %val2, align 8
  store i64 %"%919.load", ptr %"%dev_class_7", align 8
  br label %merge_397

then_398:                                         ; preds = %merge_397
  %"%923.load" = load i64, ptr %val2, align 8
  store i64 %"%923.load", ptr %"%dev_class_8", align 8
  br label %merge_399

then_400:                                         ; preds = %merge_399
  %"%927.load" = load i64, ptr %val2, align 8
  store i64 %"%927.load", ptr %"%dev_class_9", align 8
  br label %merge_401

then_402:                                         ; preds = %merge_401
  %"%931.load" = load i64, ptr %val2, align 8
  store i64 %"%931.load", ptr %"%dev_class_10", align 8
  br label %merge_403

then_404:                                         ; preds = %merge_403
  %"%935.load" = load i64, ptr %val2, align 8
  store i64 %"%935.load", ptr %"%dev_class_11", align 8
  br label %merge_405

then_406:                                         ; preds = %merge_405
  %"%939.load" = load i64, ptr %val2, align 8
  store i64 %"%939.load", ptr %"%dev_class_12", align 8
  br label %merge_407

then_408:                                         ; preds = %merge_407
  %"%943.load" = load i64, ptr %val2, align 8
  store i64 %"%943.load", ptr %"%dev_class_13", align 8
  br label %merge_409

then_410:                                         ; preds = %merge_409
  %"%947.load" = load i64, ptr %val2, align 8
  store i64 %"%947.load", ptr %"%dev_class_14", align 8
  br label %merge_411

then_412:                                         ; preds = %merge_411
  %"%951.load" = load i64, ptr %val2, align 8
  store i64 %"%951.load", ptr %"%dev_class_15", align 8
  br label %merge_413
}

define i64 @dm_get_class(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_414, label %merge_415

merge_415:                                        ; preds = %entry
  %"%d.load2" = load i64, ptr %d1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%d.load2", 1
  br i1 %cmpeqtmp3, label %then_416, label %merge_417

merge_417:                                        ; preds = %merge_415
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_418, label %merge_419

merge_419:                                        ; preds = %merge_417
  %"%d.load8" = load i64, ptr %d1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 3
  br i1 %cmpeqtmp9, label %then_420, label %merge_421

merge_421:                                        ; preds = %merge_419
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 4
  br i1 %cmpeqtmp12, label %then_422, label %merge_423

merge_423:                                        ; preds = %merge_421
  %"%d.load14" = load i64, ptr %d1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%d.load14", 5
  br i1 %cmpeqtmp15, label %then_424, label %merge_425

merge_425:                                        ; preds = %merge_423
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 6
  br i1 %cmpeqtmp18, label %then_426, label %merge_427

merge_427:                                        ; preds = %merge_425
  %"%d.load20" = load i64, ptr %d1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%d.load20", 7
  br i1 %cmpeqtmp21, label %then_428, label %merge_429

merge_429:                                        ; preds = %merge_427
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 8
  br i1 %cmpeqtmp24, label %then_430, label %merge_431

merge_431:                                        ; preds = %merge_429
  %"%d.load26" = load i64, ptr %d1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%d.load26", 9
  br i1 %cmpeqtmp27, label %then_432, label %merge_433

merge_433:                                        ; preds = %merge_431
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 10
  br i1 %cmpeqtmp30, label %then_434, label %merge_435

merge_435:                                        ; preds = %merge_433
  %"%d.load32" = load i64, ptr %d1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%d.load32", 11
  br i1 %cmpeqtmp33, label %then_436, label %merge_437

merge_437:                                        ; preds = %merge_435
  %"%d.load35" = load i64, ptr %d1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%d.load35", 12
  br i1 %cmpeqtmp36, label %then_438, label %merge_439

merge_439:                                        ; preds = %merge_437
  %"%d.load38" = load i64, ptr %d1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%d.load38", 13
  br i1 %cmpeqtmp39, label %then_440, label %merge_441

merge_441:                                        ; preds = %merge_439
  %"%d.load41" = load i64, ptr %d1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%d.load41", 14
  br i1 %cmpeqtmp42, label %then_442, label %merge_443

merge_443:                                        ; preds = %merge_441
  %"%d.load44" = load i64, ptr %d1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%d.load44", 15
  br i1 %cmpeqtmp45, label %then_444, label %merge_445

merge_445:                                        ; preds = %merge_443
  ret i64 0

then_414:                                         ; preds = %entry
  %loadtmp = load i64, ptr @dev_class_0, align 8
  ret i64 %loadtmp

then_416:                                         ; preds = %merge_415
  %loadtmp4 = load i64, ptr @dev_class_1, align 8
  ret i64 %loadtmp4

then_418:                                         ; preds = %merge_417
  %loadtmp7 = load i64, ptr @dev_class_2, align 8
  ret i64 %loadtmp7

then_420:                                         ; preds = %merge_419
  %loadtmp10 = load i64, ptr @dev_class_3, align 8
  ret i64 %loadtmp10

then_422:                                         ; preds = %merge_421
  %loadtmp13 = load i64, ptr @dev_class_4, align 8
  ret i64 %loadtmp13

then_424:                                         ; preds = %merge_423
  %loadtmp16 = load i64, ptr @dev_class_5, align 8
  ret i64 %loadtmp16

then_426:                                         ; preds = %merge_425
  %loadtmp19 = load i64, ptr @dev_class_6, align 8
  ret i64 %loadtmp19

then_428:                                         ; preds = %merge_427
  %loadtmp22 = load i64, ptr @dev_class_7, align 8
  ret i64 %loadtmp22

then_430:                                         ; preds = %merge_429
  %loadtmp25 = load i64, ptr @dev_class_8, align 8
  ret i64 %loadtmp25

then_432:                                         ; preds = %merge_431
  %loadtmp28 = load i64, ptr @dev_class_9, align 8
  ret i64 %loadtmp28

then_434:                                         ; preds = %merge_433
  %loadtmp31 = load i64, ptr @dev_class_10, align 8
  ret i64 %loadtmp31

then_436:                                         ; preds = %merge_435
  %loadtmp34 = load i64, ptr @dev_class_11, align 8
  ret i64 %loadtmp34

then_438:                                         ; preds = %merge_437
  %loadtmp37 = load i64, ptr @dev_class_12, align 8
  ret i64 %loadtmp37

then_440:                                         ; preds = %merge_439
  %loadtmp40 = load i64, ptr @dev_class_13, align 8
  ret i64 %loadtmp40

then_442:                                         ; preds = %merge_441
  %loadtmp43 = load i64, ptr @dev_class_14, align 8
  ret i64 %loadtmp43

then_444:                                         ; preds = %merge_443
  %loadtmp46 = load i64, ptr @dev_class_15, align 8
  ret i64 %loadtmp46
}

define i64 @dm_set_sub(i64 %d, i64 %val) {
entry:
  %"%dev_sub_15" = alloca i64, align 8
  %"%dev_sub_14" = alloca i64, align 8
  %"%dev_sub_13" = alloca i64, align 8
  %"%dev_sub_12" = alloca i64, align 8
  %"%dev_sub_11" = alloca i64, align 8
  %"%dev_sub_10" = alloca i64, align 8
  %"%dev_sub_9" = alloca i64, align 8
  %"%dev_sub_8" = alloca i64, align 8
  %"%dev_sub_7" = alloca i64, align 8
  %"%dev_sub_6" = alloca i64, align 8
  %"%dev_sub_5" = alloca i64, align 8
  %"%dev_sub_4" = alloca i64, align 8
  %"%dev_sub_3" = alloca i64, align 8
  %"%dev_sub_2" = alloca i64, align 8
  %"%dev_sub_1" = alloca i64, align 8
  %"%dev_sub_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_446, label %merge_447

merge_447:                                        ; preds = %then_446, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_448, label %merge_449

merge_449:                                        ; preds = %then_448, %merge_447
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_450, label %merge_451

merge_451:                                        ; preds = %then_450, %merge_449
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_452, label %merge_453

merge_453:                                        ; preds = %then_452, %merge_451
  %"%d.load9" = load i64, ptr %d1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%d.load9", 4
  br i1 %cmpeqtmp10, label %then_454, label %merge_455

merge_455:                                        ; preds = %then_454, %merge_453
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 5
  br i1 %cmpeqtmp12, label %then_456, label %merge_457

merge_457:                                        ; preds = %then_456, %merge_455
  %"%d.load13" = load i64, ptr %d1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%d.load13", 6
  br i1 %cmpeqtmp14, label %then_458, label %merge_459

merge_459:                                        ; preds = %then_458, %merge_457
  %"%d.load15" = load i64, ptr %d1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%d.load15", 7
  br i1 %cmpeqtmp16, label %then_460, label %merge_461

merge_461:                                        ; preds = %then_460, %merge_459
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 8
  br i1 %cmpeqtmp18, label %then_462, label %merge_463

merge_463:                                        ; preds = %then_462, %merge_461
  %"%d.load19" = load i64, ptr %d1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%d.load19", 9
  br i1 %cmpeqtmp20, label %then_464, label %merge_465

merge_465:                                        ; preds = %then_464, %merge_463
  %"%d.load21" = load i64, ptr %d1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%d.load21", 10
  br i1 %cmpeqtmp22, label %then_466, label %merge_467

merge_467:                                        ; preds = %then_466, %merge_465
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 11
  br i1 %cmpeqtmp24, label %then_468, label %merge_469

merge_469:                                        ; preds = %then_468, %merge_467
  %"%d.load25" = load i64, ptr %d1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%d.load25", 12
  br i1 %cmpeqtmp26, label %then_470, label %merge_471

merge_471:                                        ; preds = %then_470, %merge_469
  %"%d.load27" = load i64, ptr %d1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%d.load27", 13
  br i1 %cmpeqtmp28, label %then_472, label %merge_473

merge_473:                                        ; preds = %then_472, %merge_471
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 14
  br i1 %cmpeqtmp30, label %then_474, label %merge_475

merge_475:                                        ; preds = %then_474, %merge_473
  %"%d.load31" = load i64, ptr %d1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%d.load31", 15
  br i1 %cmpeqtmp32, label %then_476, label %merge_477

merge_477:                                        ; preds = %then_476, %merge_475
  ret i64 0

then_446:                                         ; preds = %entry
  %"%1037.load" = load i64, ptr %val2, align 8
  store i64 %"%1037.load", ptr %"%dev_sub_0", align 8
  br label %merge_447

then_448:                                         ; preds = %merge_447
  %"%1041.load" = load i64, ptr %val2, align 8
  store i64 %"%1041.load", ptr %"%dev_sub_1", align 8
  br label %merge_449

then_450:                                         ; preds = %merge_449
  %"%1045.load" = load i64, ptr %val2, align 8
  store i64 %"%1045.load", ptr %"%dev_sub_2", align 8
  br label %merge_451

then_452:                                         ; preds = %merge_451
  %"%1049.load" = load i64, ptr %val2, align 8
  store i64 %"%1049.load", ptr %"%dev_sub_3", align 8
  br label %merge_453

then_454:                                         ; preds = %merge_453
  %"%1053.load" = load i64, ptr %val2, align 8
  store i64 %"%1053.load", ptr %"%dev_sub_4", align 8
  br label %merge_455

then_456:                                         ; preds = %merge_455
  %"%1057.load" = load i64, ptr %val2, align 8
  store i64 %"%1057.load", ptr %"%dev_sub_5", align 8
  br label %merge_457

then_458:                                         ; preds = %merge_457
  %"%1061.load" = load i64, ptr %val2, align 8
  store i64 %"%1061.load", ptr %"%dev_sub_6", align 8
  br label %merge_459

then_460:                                         ; preds = %merge_459
  %"%1065.load" = load i64, ptr %val2, align 8
  store i64 %"%1065.load", ptr %"%dev_sub_7", align 8
  br label %merge_461

then_462:                                         ; preds = %merge_461
  %"%1069.load" = load i64, ptr %val2, align 8
  store i64 %"%1069.load", ptr %"%dev_sub_8", align 8
  br label %merge_463

then_464:                                         ; preds = %merge_463
  %"%1073.load" = load i64, ptr %val2, align 8
  store i64 %"%1073.load", ptr %"%dev_sub_9", align 8
  br label %merge_465

then_466:                                         ; preds = %merge_465
  %"%1077.load" = load i64, ptr %val2, align 8
  store i64 %"%1077.load", ptr %"%dev_sub_10", align 8
  br label %merge_467

then_468:                                         ; preds = %merge_467
  %"%1081.load" = load i64, ptr %val2, align 8
  store i64 %"%1081.load", ptr %"%dev_sub_11", align 8
  br label %merge_469

then_470:                                         ; preds = %merge_469
  %"%1085.load" = load i64, ptr %val2, align 8
  store i64 %"%1085.load", ptr %"%dev_sub_12", align 8
  br label %merge_471

then_472:                                         ; preds = %merge_471
  %"%1089.load" = load i64, ptr %val2, align 8
  store i64 %"%1089.load", ptr %"%dev_sub_13", align 8
  br label %merge_473

then_474:                                         ; preds = %merge_473
  %"%1093.load" = load i64, ptr %val2, align 8
  store i64 %"%1093.load", ptr %"%dev_sub_14", align 8
  br label %merge_475

then_476:                                         ; preds = %merge_475
  %"%1097.load" = load i64, ptr %val2, align 8
  store i64 %"%1097.load", ptr %"%dev_sub_15", align 8
  br label %merge_477
}

define i64 @dm_set_irq(i64 %d, i64 %val) {
entry:
  %"%dev_irq_15" = alloca i64, align 8
  %"%dev_irq_14" = alloca i64, align 8
  %"%dev_irq_13" = alloca i64, align 8
  %"%dev_irq_12" = alloca i64, align 8
  %"%dev_irq_11" = alloca i64, align 8
  %"%dev_irq_10" = alloca i64, align 8
  %"%dev_irq_9" = alloca i64, align 8
  %"%dev_irq_8" = alloca i64, align 8
  %"%dev_irq_7" = alloca i64, align 8
  %"%dev_irq_6" = alloca i64, align 8
  %"%dev_irq_5" = alloca i64, align 8
  %"%dev_irq_4" = alloca i64, align 8
  %"%dev_irq_3" = alloca i64, align 8
  %"%dev_irq_2" = alloca i64, align 8
  %"%dev_irq_1" = alloca i64, align 8
  %"%dev_irq_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_478, label %merge_479

merge_479:                                        ; preds = %then_478, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_480, label %merge_481

merge_481:                                        ; preds = %then_480, %merge_479
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_482, label %merge_483

merge_483:                                        ; preds = %then_482, %merge_481
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_484, label %merge_485

merge_485:                                        ; preds = %then_484, %merge_483
  %"%d.load9" = load i64, ptr %d1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%d.load9", 4
  br i1 %cmpeqtmp10, label %then_486, label %merge_487

merge_487:                                        ; preds = %then_486, %merge_485
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 5
  br i1 %cmpeqtmp12, label %then_488, label %merge_489

merge_489:                                        ; preds = %then_488, %merge_487
  %"%d.load13" = load i64, ptr %d1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%d.load13", 6
  br i1 %cmpeqtmp14, label %then_490, label %merge_491

merge_491:                                        ; preds = %then_490, %merge_489
  %"%d.load15" = load i64, ptr %d1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%d.load15", 7
  br i1 %cmpeqtmp16, label %then_492, label %merge_493

merge_493:                                        ; preds = %then_492, %merge_491
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 8
  br i1 %cmpeqtmp18, label %then_494, label %merge_495

merge_495:                                        ; preds = %then_494, %merge_493
  %"%d.load19" = load i64, ptr %d1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%d.load19", 9
  br i1 %cmpeqtmp20, label %then_496, label %merge_497

merge_497:                                        ; preds = %then_496, %merge_495
  %"%d.load21" = load i64, ptr %d1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%d.load21", 10
  br i1 %cmpeqtmp22, label %then_498, label %merge_499

merge_499:                                        ; preds = %then_498, %merge_497
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 11
  br i1 %cmpeqtmp24, label %then_500, label %merge_501

merge_501:                                        ; preds = %then_500, %merge_499
  %"%d.load25" = load i64, ptr %d1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%d.load25", 12
  br i1 %cmpeqtmp26, label %then_502, label %merge_503

merge_503:                                        ; preds = %then_502, %merge_501
  %"%d.load27" = load i64, ptr %d1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%d.load27", 13
  br i1 %cmpeqtmp28, label %then_504, label %merge_505

merge_505:                                        ; preds = %then_504, %merge_503
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 14
  br i1 %cmpeqtmp30, label %then_506, label %merge_507

merge_507:                                        ; preds = %then_506, %merge_505
  %"%d.load31" = load i64, ptr %d1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%d.load31", 15
  br i1 %cmpeqtmp32, label %then_508, label %merge_509

merge_509:                                        ; preds = %then_508, %merge_507
  ret i64 0

then_478:                                         ; preds = %entry
  %"%1102.load" = load i64, ptr %val2, align 8
  store i64 %"%1102.load", ptr %"%dev_irq_0", align 8
  br label %merge_479

then_480:                                         ; preds = %merge_479
  %"%1106.load" = load i64, ptr %val2, align 8
  store i64 %"%1106.load", ptr %"%dev_irq_1", align 8
  br label %merge_481

then_482:                                         ; preds = %merge_481
  %"%1110.load" = load i64, ptr %val2, align 8
  store i64 %"%1110.load", ptr %"%dev_irq_2", align 8
  br label %merge_483

then_484:                                         ; preds = %merge_483
  %"%1114.load" = load i64, ptr %val2, align 8
  store i64 %"%1114.load", ptr %"%dev_irq_3", align 8
  br label %merge_485

then_486:                                         ; preds = %merge_485
  %"%1118.load" = load i64, ptr %val2, align 8
  store i64 %"%1118.load", ptr %"%dev_irq_4", align 8
  br label %merge_487

then_488:                                         ; preds = %merge_487
  %"%1122.load" = load i64, ptr %val2, align 8
  store i64 %"%1122.load", ptr %"%dev_irq_5", align 8
  br label %merge_489

then_490:                                         ; preds = %merge_489
  %"%1126.load" = load i64, ptr %val2, align 8
  store i64 %"%1126.load", ptr %"%dev_irq_6", align 8
  br label %merge_491

then_492:                                         ; preds = %merge_491
  %"%1130.load" = load i64, ptr %val2, align 8
  store i64 %"%1130.load", ptr %"%dev_irq_7", align 8
  br label %merge_493

then_494:                                         ; preds = %merge_493
  %"%1134.load" = load i64, ptr %val2, align 8
  store i64 %"%1134.load", ptr %"%dev_irq_8", align 8
  br label %merge_495

then_496:                                         ; preds = %merge_495
  %"%1138.load" = load i64, ptr %val2, align 8
  store i64 %"%1138.load", ptr %"%dev_irq_9", align 8
  br label %merge_497

then_498:                                         ; preds = %merge_497
  %"%1142.load" = load i64, ptr %val2, align 8
  store i64 %"%1142.load", ptr %"%dev_irq_10", align 8
  br label %merge_499

then_500:                                         ; preds = %merge_499
  %"%1146.load" = load i64, ptr %val2, align 8
  store i64 %"%1146.load", ptr %"%dev_irq_11", align 8
  br label %merge_501

then_502:                                         ; preds = %merge_501
  %"%1150.load" = load i64, ptr %val2, align 8
  store i64 %"%1150.load", ptr %"%dev_irq_12", align 8
  br label %merge_503

then_504:                                         ; preds = %merge_503
  %"%1154.load" = load i64, ptr %val2, align 8
  store i64 %"%1154.load", ptr %"%dev_irq_13", align 8
  br label %merge_505

then_506:                                         ; preds = %merge_505
  %"%1158.load" = load i64, ptr %val2, align 8
  store i64 %"%1158.load", ptr %"%dev_irq_14", align 8
  br label %merge_507

then_508:                                         ; preds = %merge_507
  %"%1162.load" = load i64, ptr %val2, align 8
  store i64 %"%1162.load", ptr %"%dev_irq_15", align 8
  br label %merge_509
}

define i64 @dm_set_io(i64 %d, i64 %val) {
entry:
  %"%dev_io_15" = alloca i64, align 8
  %"%dev_io_14" = alloca i64, align 8
  %"%dev_io_13" = alloca i64, align 8
  %"%dev_io_12" = alloca i64, align 8
  %"%dev_io_11" = alloca i64, align 8
  %"%dev_io_10" = alloca i64, align 8
  %"%dev_io_9" = alloca i64, align 8
  %"%dev_io_8" = alloca i64, align 8
  %"%dev_io_7" = alloca i64, align 8
  %"%dev_io_6" = alloca i64, align 8
  %"%dev_io_5" = alloca i64, align 8
  %"%dev_io_4" = alloca i64, align 8
  %"%dev_io_3" = alloca i64, align 8
  %"%dev_io_2" = alloca i64, align 8
  %"%dev_io_1" = alloca i64, align 8
  %"%dev_io_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_510, label %merge_511

merge_511:                                        ; preds = %then_510, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_512, label %merge_513

merge_513:                                        ; preds = %then_512, %merge_511
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_514, label %merge_515

merge_515:                                        ; preds = %then_514, %merge_513
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_516, label %merge_517

merge_517:                                        ; preds = %then_516, %merge_515
  %"%d.load9" = load i64, ptr %d1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%d.load9", 4
  br i1 %cmpeqtmp10, label %then_518, label %merge_519

merge_519:                                        ; preds = %then_518, %merge_517
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 5
  br i1 %cmpeqtmp12, label %then_520, label %merge_521

merge_521:                                        ; preds = %then_520, %merge_519
  %"%d.load13" = load i64, ptr %d1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%d.load13", 6
  br i1 %cmpeqtmp14, label %then_522, label %merge_523

merge_523:                                        ; preds = %then_522, %merge_521
  %"%d.load15" = load i64, ptr %d1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%d.load15", 7
  br i1 %cmpeqtmp16, label %then_524, label %merge_525

merge_525:                                        ; preds = %then_524, %merge_523
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 8
  br i1 %cmpeqtmp18, label %then_526, label %merge_527

merge_527:                                        ; preds = %then_526, %merge_525
  %"%d.load19" = load i64, ptr %d1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%d.load19", 9
  br i1 %cmpeqtmp20, label %then_528, label %merge_529

merge_529:                                        ; preds = %then_528, %merge_527
  %"%d.load21" = load i64, ptr %d1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%d.load21", 10
  br i1 %cmpeqtmp22, label %then_530, label %merge_531

merge_531:                                        ; preds = %then_530, %merge_529
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 11
  br i1 %cmpeqtmp24, label %then_532, label %merge_533

merge_533:                                        ; preds = %then_532, %merge_531
  %"%d.load25" = load i64, ptr %d1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%d.load25", 12
  br i1 %cmpeqtmp26, label %then_534, label %merge_535

merge_535:                                        ; preds = %then_534, %merge_533
  %"%d.load27" = load i64, ptr %d1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%d.load27", 13
  br i1 %cmpeqtmp28, label %then_536, label %merge_537

merge_537:                                        ; preds = %then_536, %merge_535
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 14
  br i1 %cmpeqtmp30, label %then_538, label %merge_539

merge_539:                                        ; preds = %then_538, %merge_537
  %"%d.load31" = load i64, ptr %d1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%d.load31", 15
  br i1 %cmpeqtmp32, label %then_540, label %merge_541

merge_541:                                        ; preds = %then_540, %merge_539
  ret i64 0

then_510:                                         ; preds = %entry
  %"%1167.load" = load i64, ptr %val2, align 8
  store i64 %"%1167.load", ptr %"%dev_io_0", align 8
  br label %merge_511

then_512:                                         ; preds = %merge_511
  %"%1171.load" = load i64, ptr %val2, align 8
  store i64 %"%1171.load", ptr %"%dev_io_1", align 8
  br label %merge_513

then_514:                                         ; preds = %merge_513
  %"%1175.load" = load i64, ptr %val2, align 8
  store i64 %"%1175.load", ptr %"%dev_io_2", align 8
  br label %merge_515

then_516:                                         ; preds = %merge_515
  %"%1179.load" = load i64, ptr %val2, align 8
  store i64 %"%1179.load", ptr %"%dev_io_3", align 8
  br label %merge_517

then_518:                                         ; preds = %merge_517
  %"%1183.load" = load i64, ptr %val2, align 8
  store i64 %"%1183.load", ptr %"%dev_io_4", align 8
  br label %merge_519

then_520:                                         ; preds = %merge_519
  %"%1187.load" = load i64, ptr %val2, align 8
  store i64 %"%1187.load", ptr %"%dev_io_5", align 8
  br label %merge_521

then_522:                                         ; preds = %merge_521
  %"%1191.load" = load i64, ptr %val2, align 8
  store i64 %"%1191.load", ptr %"%dev_io_6", align 8
  br label %merge_523

then_524:                                         ; preds = %merge_523
  %"%1195.load" = load i64, ptr %val2, align 8
  store i64 %"%1195.load", ptr %"%dev_io_7", align 8
  br label %merge_525

then_526:                                         ; preds = %merge_525
  %"%1199.load" = load i64, ptr %val2, align 8
  store i64 %"%1199.load", ptr %"%dev_io_8", align 8
  br label %merge_527

then_528:                                         ; preds = %merge_527
  %"%1203.load" = load i64, ptr %val2, align 8
  store i64 %"%1203.load", ptr %"%dev_io_9", align 8
  br label %merge_529

then_530:                                         ; preds = %merge_529
  %"%1207.load" = load i64, ptr %val2, align 8
  store i64 %"%1207.load", ptr %"%dev_io_10", align 8
  br label %merge_531

then_532:                                         ; preds = %merge_531
  %"%1211.load" = load i64, ptr %val2, align 8
  store i64 %"%1211.load", ptr %"%dev_io_11", align 8
  br label %merge_533

then_534:                                         ; preds = %merge_533
  %"%1215.load" = load i64, ptr %val2, align 8
  store i64 %"%1215.load", ptr %"%dev_io_12", align 8
  br label %merge_535

then_536:                                         ; preds = %merge_535
  %"%1219.load" = load i64, ptr %val2, align 8
  store i64 %"%1219.load", ptr %"%dev_io_13", align 8
  br label %merge_537

then_538:                                         ; preds = %merge_537
  %"%1223.load" = load i64, ptr %val2, align 8
  store i64 %"%1223.load", ptr %"%dev_io_14", align 8
  br label %merge_539

then_540:                                         ; preds = %merge_539
  %"%1227.load" = load i64, ptr %val2, align 8
  store i64 %"%1227.load", ptr %"%dev_io_15", align 8
  br label %merge_541
}

define i64 @dm_set_mmio(i64 %d, i64 %val) {
entry:
  %"%dev_mmio_15" = alloca i64, align 8
  %"%dev_mmio_14" = alloca i64, align 8
  %"%dev_mmio_13" = alloca i64, align 8
  %"%dev_mmio_12" = alloca i64, align 8
  %"%dev_mmio_11" = alloca i64, align 8
  %"%dev_mmio_10" = alloca i64, align 8
  %"%dev_mmio_9" = alloca i64, align 8
  %"%dev_mmio_8" = alloca i64, align 8
  %"%dev_mmio_7" = alloca i64, align 8
  %"%dev_mmio_6" = alloca i64, align 8
  %"%dev_mmio_5" = alloca i64, align 8
  %"%dev_mmio_4" = alloca i64, align 8
  %"%dev_mmio_3" = alloca i64, align 8
  %"%dev_mmio_2" = alloca i64, align 8
  %"%dev_mmio_1" = alloca i64, align 8
  %"%dev_mmio_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_542, label %merge_543

merge_543:                                        ; preds = %then_542, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_544, label %merge_545

merge_545:                                        ; preds = %then_544, %merge_543
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_546, label %merge_547

merge_547:                                        ; preds = %then_546, %merge_545
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_548, label %merge_549

merge_549:                                        ; preds = %then_548, %merge_547
  %"%d.load9" = load i64, ptr %d1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%d.load9", 4
  br i1 %cmpeqtmp10, label %then_550, label %merge_551

merge_551:                                        ; preds = %then_550, %merge_549
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 5
  br i1 %cmpeqtmp12, label %then_552, label %merge_553

merge_553:                                        ; preds = %then_552, %merge_551
  %"%d.load13" = load i64, ptr %d1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%d.load13", 6
  br i1 %cmpeqtmp14, label %then_554, label %merge_555

merge_555:                                        ; preds = %then_554, %merge_553
  %"%d.load15" = load i64, ptr %d1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%d.load15", 7
  br i1 %cmpeqtmp16, label %then_556, label %merge_557

merge_557:                                        ; preds = %then_556, %merge_555
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 8
  br i1 %cmpeqtmp18, label %then_558, label %merge_559

merge_559:                                        ; preds = %then_558, %merge_557
  %"%d.load19" = load i64, ptr %d1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%d.load19", 9
  br i1 %cmpeqtmp20, label %then_560, label %merge_561

merge_561:                                        ; preds = %then_560, %merge_559
  %"%d.load21" = load i64, ptr %d1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%d.load21", 10
  br i1 %cmpeqtmp22, label %then_562, label %merge_563

merge_563:                                        ; preds = %then_562, %merge_561
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 11
  br i1 %cmpeqtmp24, label %then_564, label %merge_565

merge_565:                                        ; preds = %then_564, %merge_563
  %"%d.load25" = load i64, ptr %d1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%d.load25", 12
  br i1 %cmpeqtmp26, label %then_566, label %merge_567

merge_567:                                        ; preds = %then_566, %merge_565
  %"%d.load27" = load i64, ptr %d1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%d.load27", 13
  br i1 %cmpeqtmp28, label %then_568, label %merge_569

merge_569:                                        ; preds = %then_568, %merge_567
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 14
  br i1 %cmpeqtmp30, label %then_570, label %merge_571

merge_571:                                        ; preds = %then_570, %merge_569
  %"%d.load31" = load i64, ptr %d1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%d.load31", 15
  br i1 %cmpeqtmp32, label %then_572, label %merge_573

merge_573:                                        ; preds = %then_572, %merge_571
  ret i64 0

then_542:                                         ; preds = %entry
  %"%1232.load" = load i64, ptr %val2, align 8
  store i64 %"%1232.load", ptr %"%dev_mmio_0", align 8
  br label %merge_543

then_544:                                         ; preds = %merge_543
  %"%1236.load" = load i64, ptr %val2, align 8
  store i64 %"%1236.load", ptr %"%dev_mmio_1", align 8
  br label %merge_545

then_546:                                         ; preds = %merge_545
  %"%1240.load" = load i64, ptr %val2, align 8
  store i64 %"%1240.load", ptr %"%dev_mmio_2", align 8
  br label %merge_547

then_548:                                         ; preds = %merge_547
  %"%1244.load" = load i64, ptr %val2, align 8
  store i64 %"%1244.load", ptr %"%dev_mmio_3", align 8
  br label %merge_549

then_550:                                         ; preds = %merge_549
  %"%1248.load" = load i64, ptr %val2, align 8
  store i64 %"%1248.load", ptr %"%dev_mmio_4", align 8
  br label %merge_551

then_552:                                         ; preds = %merge_551
  %"%1252.load" = load i64, ptr %val2, align 8
  store i64 %"%1252.load", ptr %"%dev_mmio_5", align 8
  br label %merge_553

then_554:                                         ; preds = %merge_553
  %"%1256.load" = load i64, ptr %val2, align 8
  store i64 %"%1256.load", ptr %"%dev_mmio_6", align 8
  br label %merge_555

then_556:                                         ; preds = %merge_555
  %"%1260.load" = load i64, ptr %val2, align 8
  store i64 %"%1260.load", ptr %"%dev_mmio_7", align 8
  br label %merge_557

then_558:                                         ; preds = %merge_557
  %"%1264.load" = load i64, ptr %val2, align 8
  store i64 %"%1264.load", ptr %"%dev_mmio_8", align 8
  br label %merge_559

then_560:                                         ; preds = %merge_559
  %"%1268.load" = load i64, ptr %val2, align 8
  store i64 %"%1268.load", ptr %"%dev_mmio_9", align 8
  br label %merge_561

then_562:                                         ; preds = %merge_561
  %"%1272.load" = load i64, ptr %val2, align 8
  store i64 %"%1272.load", ptr %"%dev_mmio_10", align 8
  br label %merge_563

then_564:                                         ; preds = %merge_563
  %"%1276.load" = load i64, ptr %val2, align 8
  store i64 %"%1276.load", ptr %"%dev_mmio_11", align 8
  br label %merge_565

then_566:                                         ; preds = %merge_565
  %"%1280.load" = load i64, ptr %val2, align 8
  store i64 %"%1280.load", ptr %"%dev_mmio_12", align 8
  br label %merge_567

then_568:                                         ; preds = %merge_567
  %"%1284.load" = load i64, ptr %val2, align 8
  store i64 %"%1284.load", ptr %"%dev_mmio_13", align 8
  br label %merge_569

then_570:                                         ; preds = %merge_569
  %"%1288.load" = load i64, ptr %val2, align 8
  store i64 %"%1288.load", ptr %"%dev_mmio_14", align 8
  br label %merge_571

then_572:                                         ; preds = %merge_571
  %"%1292.load" = load i64, ptr %val2, align 8
  store i64 %"%1292.load", ptr %"%dev_mmio_15", align 8
  br label %merge_573
}

define i64 @dm_set_driver(i64 %d, i64 %val) {
entry:
  %"%dev_drv_15" = alloca i64, align 8
  %"%dev_drv_14" = alloca i64, align 8
  %"%dev_drv_13" = alloca i64, align 8
  %"%dev_drv_12" = alloca i64, align 8
  %"%dev_drv_11" = alloca i64, align 8
  %"%dev_drv_10" = alloca i64, align 8
  %"%dev_drv_9" = alloca i64, align 8
  %"%dev_drv_8" = alloca i64, align 8
  %"%dev_drv_7" = alloca i64, align 8
  %"%dev_drv_6" = alloca i64, align 8
  %"%dev_drv_5" = alloca i64, align 8
  %"%dev_drv_4" = alloca i64, align 8
  %"%dev_drv_3" = alloca i64, align 8
  %"%dev_drv_2" = alloca i64, align 8
  %"%dev_drv_1" = alloca i64, align 8
  %"%dev_drv_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_574, label %merge_575

merge_575:                                        ; preds = %then_574, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_576, label %merge_577

merge_577:                                        ; preds = %then_576, %merge_575
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_578, label %merge_579

merge_579:                                        ; preds = %then_578, %merge_577
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_580, label %merge_581

merge_581:                                        ; preds = %then_580, %merge_579
  %"%d.load9" = load i64, ptr %d1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%d.load9", 4
  br i1 %cmpeqtmp10, label %then_582, label %merge_583

merge_583:                                        ; preds = %then_582, %merge_581
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 5
  br i1 %cmpeqtmp12, label %then_584, label %merge_585

merge_585:                                        ; preds = %then_584, %merge_583
  %"%d.load13" = load i64, ptr %d1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%d.load13", 6
  br i1 %cmpeqtmp14, label %then_586, label %merge_587

merge_587:                                        ; preds = %then_586, %merge_585
  %"%d.load15" = load i64, ptr %d1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%d.load15", 7
  br i1 %cmpeqtmp16, label %then_588, label %merge_589

merge_589:                                        ; preds = %then_588, %merge_587
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 8
  br i1 %cmpeqtmp18, label %then_590, label %merge_591

merge_591:                                        ; preds = %then_590, %merge_589
  %"%d.load19" = load i64, ptr %d1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%d.load19", 9
  br i1 %cmpeqtmp20, label %then_592, label %merge_593

merge_593:                                        ; preds = %then_592, %merge_591
  %"%d.load21" = load i64, ptr %d1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%d.load21", 10
  br i1 %cmpeqtmp22, label %then_594, label %merge_595

merge_595:                                        ; preds = %then_594, %merge_593
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 11
  br i1 %cmpeqtmp24, label %then_596, label %merge_597

merge_597:                                        ; preds = %then_596, %merge_595
  %"%d.load25" = load i64, ptr %d1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%d.load25", 12
  br i1 %cmpeqtmp26, label %then_598, label %merge_599

merge_599:                                        ; preds = %then_598, %merge_597
  %"%d.load27" = load i64, ptr %d1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%d.load27", 13
  br i1 %cmpeqtmp28, label %then_600, label %merge_601

merge_601:                                        ; preds = %then_600, %merge_599
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 14
  br i1 %cmpeqtmp30, label %then_602, label %merge_603

merge_603:                                        ; preds = %then_602, %merge_601
  %"%d.load31" = load i64, ptr %d1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%d.load31", 15
  br i1 %cmpeqtmp32, label %then_604, label %merge_605

merge_605:                                        ; preds = %then_604, %merge_603
  ret i64 0

then_574:                                         ; preds = %entry
  %"%1297.load" = load i64, ptr %val2, align 8
  store i64 %"%1297.load", ptr %"%dev_drv_0", align 8
  br label %merge_575

then_576:                                         ; preds = %merge_575
  %"%1301.load" = load i64, ptr %val2, align 8
  store i64 %"%1301.load", ptr %"%dev_drv_1", align 8
  br label %merge_577

then_578:                                         ; preds = %merge_577
  %"%1305.load" = load i64, ptr %val2, align 8
  store i64 %"%1305.load", ptr %"%dev_drv_2", align 8
  br label %merge_579

then_580:                                         ; preds = %merge_579
  %"%1309.load" = load i64, ptr %val2, align 8
  store i64 %"%1309.load", ptr %"%dev_drv_3", align 8
  br label %merge_581

then_582:                                         ; preds = %merge_581
  %"%1313.load" = load i64, ptr %val2, align 8
  store i64 %"%1313.load", ptr %"%dev_drv_4", align 8
  br label %merge_583

then_584:                                         ; preds = %merge_583
  %"%1317.load" = load i64, ptr %val2, align 8
  store i64 %"%1317.load", ptr %"%dev_drv_5", align 8
  br label %merge_585

then_586:                                         ; preds = %merge_585
  %"%1321.load" = load i64, ptr %val2, align 8
  store i64 %"%1321.load", ptr %"%dev_drv_6", align 8
  br label %merge_587

then_588:                                         ; preds = %merge_587
  %"%1325.load" = load i64, ptr %val2, align 8
  store i64 %"%1325.load", ptr %"%dev_drv_7", align 8
  br label %merge_589

then_590:                                         ; preds = %merge_589
  %"%1329.load" = load i64, ptr %val2, align 8
  store i64 %"%1329.load", ptr %"%dev_drv_8", align 8
  br label %merge_591

then_592:                                         ; preds = %merge_591
  %"%1333.load" = load i64, ptr %val2, align 8
  store i64 %"%1333.load", ptr %"%dev_drv_9", align 8
  br label %merge_593

then_594:                                         ; preds = %merge_593
  %"%1337.load" = load i64, ptr %val2, align 8
  store i64 %"%1337.load", ptr %"%dev_drv_10", align 8
  br label %merge_595

then_596:                                         ; preds = %merge_595
  %"%1341.load" = load i64, ptr %val2, align 8
  store i64 %"%1341.load", ptr %"%dev_drv_11", align 8
  br label %merge_597

then_598:                                         ; preds = %merge_597
  %"%1345.load" = load i64, ptr %val2, align 8
  store i64 %"%1345.load", ptr %"%dev_drv_12", align 8
  br label %merge_599

then_600:                                         ; preds = %merge_599
  %"%1349.load" = load i64, ptr %val2, align 8
  store i64 %"%1349.load", ptr %"%dev_drv_13", align 8
  br label %merge_601

then_602:                                         ; preds = %merge_601
  %"%1353.load" = load i64, ptr %val2, align 8
  store i64 %"%1353.load", ptr %"%dev_drv_14", align 8
  br label %merge_603

then_604:                                         ; preds = %merge_603
  %"%1357.load" = load i64, ptr %val2, align 8
  store i64 %"%1357.load", ptr %"%dev_drv_15", align 8
  br label %merge_605
}

define i64 @dm_get_drv_type(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_606, label %merge_607

merge_607:                                        ; preds = %entry
  %"%d.load2" = load i64, ptr %d1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%d.load2", 1
  br i1 %cmpeqtmp3, label %then_608, label %merge_609

merge_609:                                        ; preds = %merge_607
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_610, label %merge_611

merge_611:                                        ; preds = %merge_609
  %"%d.load8" = load i64, ptr %d1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 3
  br i1 %cmpeqtmp9, label %then_612, label %merge_613

merge_613:                                        ; preds = %merge_611
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 4
  br i1 %cmpeqtmp12, label %then_614, label %merge_615

merge_615:                                        ; preds = %merge_613
  %"%d.load14" = load i64, ptr %d1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%d.load14", 5
  br i1 %cmpeqtmp15, label %then_616, label %merge_617

merge_617:                                        ; preds = %merge_615
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 6
  br i1 %cmpeqtmp18, label %then_618, label %merge_619

merge_619:                                        ; preds = %merge_617
  %"%d.load20" = load i64, ptr %d1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%d.load20", 7
  br i1 %cmpeqtmp21, label %then_620, label %merge_621

merge_621:                                        ; preds = %merge_619
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 8
  br i1 %cmpeqtmp24, label %then_622, label %merge_623

merge_623:                                        ; preds = %merge_621
  %"%d.load26" = load i64, ptr %d1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%d.load26", 9
  br i1 %cmpeqtmp27, label %then_624, label %merge_625

merge_625:                                        ; preds = %merge_623
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 10
  br i1 %cmpeqtmp30, label %then_626, label %merge_627

merge_627:                                        ; preds = %merge_625
  %"%d.load32" = load i64, ptr %d1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%d.load32", 11
  br i1 %cmpeqtmp33, label %then_628, label %merge_629

merge_629:                                        ; preds = %merge_627
  %"%d.load35" = load i64, ptr %d1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%d.load35", 12
  br i1 %cmpeqtmp36, label %then_630, label %merge_631

merge_631:                                        ; preds = %merge_629
  %"%d.load38" = load i64, ptr %d1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%d.load38", 13
  br i1 %cmpeqtmp39, label %then_632, label %merge_633

merge_633:                                        ; preds = %merge_631
  %"%d.load41" = load i64, ptr %d1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%d.load41", 14
  br i1 %cmpeqtmp42, label %then_634, label %merge_635

merge_635:                                        ; preds = %merge_633
  %"%d.load44" = load i64, ptr %d1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%d.load44", 15
  br i1 %cmpeqtmp45, label %then_636, label %merge_637

merge_637:                                        ; preds = %merge_635
  ret i64 0

then_606:                                         ; preds = %entry
  %loadtmp = load i64, ptr @drv_type_0, align 8
  ret i64 %loadtmp

then_608:                                         ; preds = %merge_607
  %loadtmp4 = load i64, ptr @drv_type_1, align 8
  ret i64 %loadtmp4

then_610:                                         ; preds = %merge_609
  %loadtmp7 = load i64, ptr @drv_type_2, align 8
  ret i64 %loadtmp7

then_612:                                         ; preds = %merge_611
  %loadtmp10 = load i64, ptr @drv_type_3, align 8
  ret i64 %loadtmp10

then_614:                                         ; preds = %merge_613
  %loadtmp13 = load i64, ptr @drv_type_4, align 8
  ret i64 %loadtmp13

then_616:                                         ; preds = %merge_615
  %loadtmp16 = load i64, ptr @drv_type_5, align 8
  ret i64 %loadtmp16

then_618:                                         ; preds = %merge_617
  %loadtmp19 = load i64, ptr @drv_type_6, align 8
  ret i64 %loadtmp19

then_620:                                         ; preds = %merge_619
  %loadtmp22 = load i64, ptr @drv_type_7, align 8
  ret i64 %loadtmp22

then_622:                                         ; preds = %merge_621
  %loadtmp25 = load i64, ptr @drv_type_8, align 8
  ret i64 %loadtmp25

then_624:                                         ; preds = %merge_623
  %loadtmp28 = load i64, ptr @drv_type_9, align 8
  ret i64 %loadtmp28

then_626:                                         ; preds = %merge_625
  %loadtmp31 = load i64, ptr @drv_type_10, align 8
  ret i64 %loadtmp31

then_628:                                         ; preds = %merge_627
  %loadtmp34 = load i64, ptr @drv_type_11, align 8
  ret i64 %loadtmp34

then_630:                                         ; preds = %merge_629
  %loadtmp37 = load i64, ptr @drv_type_12, align 8
  ret i64 %loadtmp37

then_632:                                         ; preds = %merge_631
  %loadtmp40 = load i64, ptr @drv_type_13, align 8
  ret i64 %loadtmp40

then_634:                                         ; preds = %merge_633
  %loadtmp43 = load i64, ptr @drv_type_14, align 8
  ret i64 %loadtmp43

then_636:                                         ; preds = %merge_635
  %loadtmp46 = load i64, ptr @drv_type_15, align 8
  ret i64 %loadtmp46
}

define i64 @dm_set_drv_type(i64 %d, i64 %val) {
entry:
  %"%drv_type_15" = alloca i64, align 8
  %"%drv_type_14" = alloca i64, align 8
  %"%drv_type_13" = alloca i64, align 8
  %"%drv_type_12" = alloca i64, align 8
  %"%drv_type_11" = alloca i64, align 8
  %"%drv_type_10" = alloca i64, align 8
  %"%drv_type_9" = alloca i64, align 8
  %"%drv_type_8" = alloca i64, align 8
  %"%drv_type_7" = alloca i64, align 8
  %"%drv_type_6" = alloca i64, align 8
  %"%drv_type_5" = alloca i64, align 8
  %"%drv_type_4" = alloca i64, align 8
  %"%drv_type_3" = alloca i64, align 8
  %"%drv_type_2" = alloca i64, align 8
  %"%drv_type_1" = alloca i64, align 8
  %"%drv_type_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_638, label %merge_639

merge_639:                                        ; preds = %then_638, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_640, label %merge_641

merge_641:                                        ; preds = %then_640, %merge_639
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_642, label %merge_643

merge_643:                                        ; preds = %then_642, %merge_641
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_644, label %merge_645

merge_645:                                        ; preds = %then_644, %merge_643
  %"%d.load9" = load i64, ptr %d1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%d.load9", 4
  br i1 %cmpeqtmp10, label %then_646, label %merge_647

merge_647:                                        ; preds = %then_646, %merge_645
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 5
  br i1 %cmpeqtmp12, label %then_648, label %merge_649

merge_649:                                        ; preds = %then_648, %merge_647
  %"%d.load13" = load i64, ptr %d1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%d.load13", 6
  br i1 %cmpeqtmp14, label %then_650, label %merge_651

merge_651:                                        ; preds = %then_650, %merge_649
  %"%d.load15" = load i64, ptr %d1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%d.load15", 7
  br i1 %cmpeqtmp16, label %then_652, label %merge_653

merge_653:                                        ; preds = %then_652, %merge_651
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 8
  br i1 %cmpeqtmp18, label %then_654, label %merge_655

merge_655:                                        ; preds = %then_654, %merge_653
  %"%d.load19" = load i64, ptr %d1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%d.load19", 9
  br i1 %cmpeqtmp20, label %then_656, label %merge_657

merge_657:                                        ; preds = %then_656, %merge_655
  %"%d.load21" = load i64, ptr %d1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%d.load21", 10
  br i1 %cmpeqtmp22, label %then_658, label %merge_659

merge_659:                                        ; preds = %then_658, %merge_657
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 11
  br i1 %cmpeqtmp24, label %then_660, label %merge_661

merge_661:                                        ; preds = %then_660, %merge_659
  %"%d.load25" = load i64, ptr %d1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%d.load25", 12
  br i1 %cmpeqtmp26, label %then_662, label %merge_663

merge_663:                                        ; preds = %then_662, %merge_661
  %"%d.load27" = load i64, ptr %d1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%d.load27", 13
  br i1 %cmpeqtmp28, label %then_664, label %merge_665

merge_665:                                        ; preds = %then_664, %merge_663
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 14
  br i1 %cmpeqtmp30, label %then_666, label %merge_667

merge_667:                                        ; preds = %then_666, %merge_665
  %"%d.load31" = load i64, ptr %d1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%d.load31", 15
  br i1 %cmpeqtmp32, label %then_668, label %merge_669

merge_669:                                        ; preds = %then_668, %merge_667
  ret i64 0

then_638:                                         ; preds = %entry
  %"%1443.load" = load i64, ptr %val2, align 8
  store i64 %"%1443.load", ptr %"%drv_type_0", align 8
  br label %merge_639

then_640:                                         ; preds = %merge_639
  %"%1447.load" = load i64, ptr %val2, align 8
  store i64 %"%1447.load", ptr %"%drv_type_1", align 8
  br label %merge_641

then_642:                                         ; preds = %merge_641
  %"%1451.load" = load i64, ptr %val2, align 8
  store i64 %"%1451.load", ptr %"%drv_type_2", align 8
  br label %merge_643

then_644:                                         ; preds = %merge_643
  %"%1455.load" = load i64, ptr %val2, align 8
  store i64 %"%1455.load", ptr %"%drv_type_3", align 8
  br label %merge_645

then_646:                                         ; preds = %merge_645
  %"%1459.load" = load i64, ptr %val2, align 8
  store i64 %"%1459.load", ptr %"%drv_type_4", align 8
  br label %merge_647

then_648:                                         ; preds = %merge_647
  %"%1463.load" = load i64, ptr %val2, align 8
  store i64 %"%1463.load", ptr %"%drv_type_5", align 8
  br label %merge_649

then_650:                                         ; preds = %merge_649
  %"%1467.load" = load i64, ptr %val2, align 8
  store i64 %"%1467.load", ptr %"%drv_type_6", align 8
  br label %merge_651

then_652:                                         ; preds = %merge_651
  %"%1471.load" = load i64, ptr %val2, align 8
  store i64 %"%1471.load", ptr %"%drv_type_7", align 8
  br label %merge_653

then_654:                                         ; preds = %merge_653
  %"%1475.load" = load i64, ptr %val2, align 8
  store i64 %"%1475.load", ptr %"%drv_type_8", align 8
  br label %merge_655

then_656:                                         ; preds = %merge_655
  %"%1479.load" = load i64, ptr %val2, align 8
  store i64 %"%1479.load", ptr %"%drv_type_9", align 8
  br label %merge_657

then_658:                                         ; preds = %merge_657
  %"%1483.load" = load i64, ptr %val2, align 8
  store i64 %"%1483.load", ptr %"%drv_type_10", align 8
  br label %merge_659

then_660:                                         ; preds = %merge_659
  %"%1487.load" = load i64, ptr %val2, align 8
  store i64 %"%1487.load", ptr %"%drv_type_11", align 8
  br label %merge_661

then_662:                                         ; preds = %merge_661
  %"%1491.load" = load i64, ptr %val2, align 8
  store i64 %"%1491.load", ptr %"%drv_type_12", align 8
  br label %merge_663

then_664:                                         ; preds = %merge_663
  %"%1495.load" = load i64, ptr %val2, align 8
  store i64 %"%1495.load", ptr %"%drv_type_13", align 8
  br label %merge_665

then_666:                                         ; preds = %merge_665
  %"%1499.load" = load i64, ptr %val2, align 8
  store i64 %"%1499.load", ptr %"%drv_type_14", align 8
  br label %merge_667

then_668:                                         ; preds = %merge_667
  %"%1503.load" = load i64, ptr %val2, align 8
  store i64 %"%1503.load", ptr %"%drv_type_15", align 8
  br label %merge_669
}

define i64 @dm_set_drv_reg(i64 %d, i64 %val) {
entry:
  %"%drv_reg_15" = alloca i64, align 8
  %"%drv_reg_14" = alloca i64, align 8
  %"%drv_reg_13" = alloca i64, align 8
  %"%drv_reg_12" = alloca i64, align 8
  %"%drv_reg_11" = alloca i64, align 8
  %"%drv_reg_10" = alloca i64, align 8
  %"%drv_reg_9" = alloca i64, align 8
  %"%drv_reg_8" = alloca i64, align 8
  %"%drv_reg_7" = alloca i64, align 8
  %"%drv_reg_6" = alloca i64, align 8
  %"%drv_reg_5" = alloca i64, align 8
  %"%drv_reg_4" = alloca i64, align 8
  %"%drv_reg_3" = alloca i64, align 8
  %"%drv_reg_2" = alloca i64, align 8
  %"%drv_reg_1" = alloca i64, align 8
  %"%drv_reg_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_670, label %merge_671

merge_671:                                        ; preds = %then_670, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_672, label %merge_673

merge_673:                                        ; preds = %then_672, %merge_671
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_674, label %merge_675

merge_675:                                        ; preds = %then_674, %merge_673
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_676, label %merge_677

merge_677:                                        ; preds = %then_676, %merge_675
  %"%d.load9" = load i64, ptr %d1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%d.load9", 4
  br i1 %cmpeqtmp10, label %then_678, label %merge_679

merge_679:                                        ; preds = %then_678, %merge_677
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 5
  br i1 %cmpeqtmp12, label %then_680, label %merge_681

merge_681:                                        ; preds = %then_680, %merge_679
  %"%d.load13" = load i64, ptr %d1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%d.load13", 6
  br i1 %cmpeqtmp14, label %then_682, label %merge_683

merge_683:                                        ; preds = %then_682, %merge_681
  %"%d.load15" = load i64, ptr %d1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%d.load15", 7
  br i1 %cmpeqtmp16, label %then_684, label %merge_685

merge_685:                                        ; preds = %then_684, %merge_683
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 8
  br i1 %cmpeqtmp18, label %then_686, label %merge_687

merge_687:                                        ; preds = %then_686, %merge_685
  %"%d.load19" = load i64, ptr %d1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%d.load19", 9
  br i1 %cmpeqtmp20, label %then_688, label %merge_689

merge_689:                                        ; preds = %then_688, %merge_687
  %"%d.load21" = load i64, ptr %d1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%d.load21", 10
  br i1 %cmpeqtmp22, label %then_690, label %merge_691

merge_691:                                        ; preds = %then_690, %merge_689
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 11
  br i1 %cmpeqtmp24, label %then_692, label %merge_693

merge_693:                                        ; preds = %then_692, %merge_691
  %"%d.load25" = load i64, ptr %d1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%d.load25", 12
  br i1 %cmpeqtmp26, label %then_694, label %merge_695

merge_695:                                        ; preds = %then_694, %merge_693
  %"%d.load27" = load i64, ptr %d1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%d.load27", 13
  br i1 %cmpeqtmp28, label %then_696, label %merge_697

merge_697:                                        ; preds = %then_696, %merge_695
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 14
  br i1 %cmpeqtmp30, label %then_698, label %merge_699

merge_699:                                        ; preds = %then_698, %merge_697
  %"%d.load31" = load i64, ptr %d1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%d.load31", 15
  br i1 %cmpeqtmp32, label %then_700, label %merge_701

merge_701:                                        ; preds = %then_700, %merge_699
  ret i64 0

then_670:                                         ; preds = %entry
  %"%1508.load" = load i64, ptr %val2, align 8
  store i64 %"%1508.load", ptr %"%drv_reg_0", align 8
  br label %merge_671

then_672:                                         ; preds = %merge_671
  %"%1512.load" = load i64, ptr %val2, align 8
  store i64 %"%1512.load", ptr %"%drv_reg_1", align 8
  br label %merge_673

then_674:                                         ; preds = %merge_673
  %"%1516.load" = load i64, ptr %val2, align 8
  store i64 %"%1516.load", ptr %"%drv_reg_2", align 8
  br label %merge_675

then_676:                                         ; preds = %merge_675
  %"%1520.load" = load i64, ptr %val2, align 8
  store i64 %"%1520.load", ptr %"%drv_reg_3", align 8
  br label %merge_677

then_678:                                         ; preds = %merge_677
  %"%1524.load" = load i64, ptr %val2, align 8
  store i64 %"%1524.load", ptr %"%drv_reg_4", align 8
  br label %merge_679

then_680:                                         ; preds = %merge_679
  %"%1528.load" = load i64, ptr %val2, align 8
  store i64 %"%1528.load", ptr %"%drv_reg_5", align 8
  br label %merge_681

then_682:                                         ; preds = %merge_681
  %"%1532.load" = load i64, ptr %val2, align 8
  store i64 %"%1532.load", ptr %"%drv_reg_6", align 8
  br label %merge_683

then_684:                                         ; preds = %merge_683
  %"%1536.load" = load i64, ptr %val2, align 8
  store i64 %"%1536.load", ptr %"%drv_reg_7", align 8
  br label %merge_685

then_686:                                         ; preds = %merge_685
  %"%1540.load" = load i64, ptr %val2, align 8
  store i64 %"%1540.load", ptr %"%drv_reg_8", align 8
  br label %merge_687

then_688:                                         ; preds = %merge_687
  %"%1544.load" = load i64, ptr %val2, align 8
  store i64 %"%1544.load", ptr %"%drv_reg_9", align 8
  br label %merge_689

then_690:                                         ; preds = %merge_689
  %"%1548.load" = load i64, ptr %val2, align 8
  store i64 %"%1548.load", ptr %"%drv_reg_10", align 8
  br label %merge_691

then_692:                                         ; preds = %merge_691
  %"%1552.load" = load i64, ptr %val2, align 8
  store i64 %"%1552.load", ptr %"%drv_reg_11", align 8
  br label %merge_693

then_694:                                         ; preds = %merge_693
  %"%1556.load" = load i64, ptr %val2, align 8
  store i64 %"%1556.load", ptr %"%drv_reg_12", align 8
  br label %merge_695

then_696:                                         ; preds = %merge_695
  %"%1560.load" = load i64, ptr %val2, align 8
  store i64 %"%1560.load", ptr %"%drv_reg_13", align 8
  br label %merge_697

then_698:                                         ; preds = %merge_697
  %"%1564.load" = load i64, ptr %val2, align 8
  store i64 %"%1564.load", ptr %"%drv_reg_14", align 8
  br label %merge_699

then_700:                                         ; preds = %merge_699
  %"%1568.load" = load i64, ptr %val2, align 8
  store i64 %"%1568.load", ptr %"%drv_reg_15", align 8
  br label %merge_701
}

define i64 @dm_set_drv_pclass(i64 %d, i64 %val) {
entry:
  %"%drv_pclass_15" = alloca i64, align 8
  %"%drv_pclass_14" = alloca i64, align 8
  %"%drv_pclass_13" = alloca i64, align 8
  %"%drv_pclass_12" = alloca i64, align 8
  %"%drv_pclass_11" = alloca i64, align 8
  %"%drv_pclass_10" = alloca i64, align 8
  %"%drv_pclass_9" = alloca i64, align 8
  %"%drv_pclass_8" = alloca i64, align 8
  %"%drv_pclass_7" = alloca i64, align 8
  %"%drv_pclass_6" = alloca i64, align 8
  %"%drv_pclass_5" = alloca i64, align 8
  %"%drv_pclass_4" = alloca i64, align 8
  %"%drv_pclass_3" = alloca i64, align 8
  %"%drv_pclass_2" = alloca i64, align 8
  %"%drv_pclass_1" = alloca i64, align 8
  %"%drv_pclass_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_702, label %merge_703

merge_703:                                        ; preds = %then_702, %entry
  %"%d.load3" = load i64, ptr %d1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%d.load3", 1
  br i1 %cmpeqtmp4, label %then_704, label %merge_705

merge_705:                                        ; preds = %then_704, %merge_703
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_706, label %merge_707

merge_707:                                        ; preds = %then_706, %merge_705
  %"%d.load7" = load i64, ptr %d1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d.load7", 3
  br i1 %cmpeqtmp8, label %then_708, label %merge_709

merge_709:                                        ; preds = %then_708, %merge_707
  %"%d.load9" = load i64, ptr %d1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%d.load9", 4
  br i1 %cmpeqtmp10, label %then_710, label %merge_711

merge_711:                                        ; preds = %then_710, %merge_709
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 5
  br i1 %cmpeqtmp12, label %then_712, label %merge_713

merge_713:                                        ; preds = %then_712, %merge_711
  %"%d.load13" = load i64, ptr %d1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%d.load13", 6
  br i1 %cmpeqtmp14, label %then_714, label %merge_715

merge_715:                                        ; preds = %then_714, %merge_713
  %"%d.load15" = load i64, ptr %d1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%d.load15", 7
  br i1 %cmpeqtmp16, label %then_716, label %merge_717

merge_717:                                        ; preds = %then_716, %merge_715
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 8
  br i1 %cmpeqtmp18, label %then_718, label %merge_719

merge_719:                                        ; preds = %then_718, %merge_717
  %"%d.load19" = load i64, ptr %d1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%d.load19", 9
  br i1 %cmpeqtmp20, label %then_720, label %merge_721

merge_721:                                        ; preds = %then_720, %merge_719
  %"%d.load21" = load i64, ptr %d1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%d.load21", 10
  br i1 %cmpeqtmp22, label %then_722, label %merge_723

merge_723:                                        ; preds = %then_722, %merge_721
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 11
  br i1 %cmpeqtmp24, label %then_724, label %merge_725

merge_725:                                        ; preds = %then_724, %merge_723
  %"%d.load25" = load i64, ptr %d1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%d.load25", 12
  br i1 %cmpeqtmp26, label %then_726, label %merge_727

merge_727:                                        ; preds = %then_726, %merge_725
  %"%d.load27" = load i64, ptr %d1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%d.load27", 13
  br i1 %cmpeqtmp28, label %then_728, label %merge_729

merge_729:                                        ; preds = %then_728, %merge_727
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 14
  br i1 %cmpeqtmp30, label %then_730, label %merge_731

merge_731:                                        ; preds = %then_730, %merge_729
  %"%d.load31" = load i64, ptr %d1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%d.load31", 15
  br i1 %cmpeqtmp32, label %then_732, label %merge_733

merge_733:                                        ; preds = %then_732, %merge_731
  ret i64 0

then_702:                                         ; preds = %entry
  %"%1573.load" = load i64, ptr %val2, align 8
  store i64 %"%1573.load", ptr %"%drv_pclass_0", align 8
  br label %merge_703

then_704:                                         ; preds = %merge_703
  %"%1577.load" = load i64, ptr %val2, align 8
  store i64 %"%1577.load", ptr %"%drv_pclass_1", align 8
  br label %merge_705

then_706:                                         ; preds = %merge_705
  %"%1581.load" = load i64, ptr %val2, align 8
  store i64 %"%1581.load", ptr %"%drv_pclass_2", align 8
  br label %merge_707

then_708:                                         ; preds = %merge_707
  %"%1585.load" = load i64, ptr %val2, align 8
  store i64 %"%1585.load", ptr %"%drv_pclass_3", align 8
  br label %merge_709

then_710:                                         ; preds = %merge_709
  %"%1589.load" = load i64, ptr %val2, align 8
  store i64 %"%1589.load", ptr %"%drv_pclass_4", align 8
  br label %merge_711

then_712:                                         ; preds = %merge_711
  %"%1593.load" = load i64, ptr %val2, align 8
  store i64 %"%1593.load", ptr %"%drv_pclass_5", align 8
  br label %merge_713

then_714:                                         ; preds = %merge_713
  %"%1597.load" = load i64, ptr %val2, align 8
  store i64 %"%1597.load", ptr %"%drv_pclass_6", align 8
  br label %merge_715

then_716:                                         ; preds = %merge_715
  %"%1601.load" = load i64, ptr %val2, align 8
  store i64 %"%1601.load", ptr %"%drv_pclass_7", align 8
  br label %merge_717

then_718:                                         ; preds = %merge_717
  %"%1605.load" = load i64, ptr %val2, align 8
  store i64 %"%1605.load", ptr %"%drv_pclass_8", align 8
  br label %merge_719

then_720:                                         ; preds = %merge_719
  %"%1609.load" = load i64, ptr %val2, align 8
  store i64 %"%1609.load", ptr %"%drv_pclass_9", align 8
  br label %merge_721

then_722:                                         ; preds = %merge_721
  %"%1613.load" = load i64, ptr %val2, align 8
  store i64 %"%1613.load", ptr %"%drv_pclass_10", align 8
  br label %merge_723

then_724:                                         ; preds = %merge_723
  %"%1617.load" = load i64, ptr %val2, align 8
  store i64 %"%1617.load", ptr %"%drv_pclass_11", align 8
  br label %merge_725

then_726:                                         ; preds = %merge_725
  %"%1621.load" = load i64, ptr %val2, align 8
  store i64 %"%1621.load", ptr %"%drv_pclass_12", align 8
  br label %merge_727

then_728:                                         ; preds = %merge_727
  %"%1625.load" = load i64, ptr %val2, align 8
  store i64 %"%1625.load", ptr %"%drv_pclass_13", align 8
  br label %merge_729

then_730:                                         ; preds = %merge_729
  %"%1629.load" = load i64, ptr %val2, align 8
  store i64 %"%1629.load", ptr %"%drv_pclass_14", align 8
  br label %merge_731

then_732:                                         ; preds = %merge_731
  %"%1633.load" = load i64, ptr %val2, align 8
  store i64 %"%1633.load", ptr %"%drv_pclass_15", align 8
  br label %merge_733
}

define i64 @dm_get_drv_pclass(i64 %d) {
entry:
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_734, label %merge_735

merge_735:                                        ; preds = %entry
  %"%d.load2" = load i64, ptr %d1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%d.load2", 1
  br i1 %cmpeqtmp3, label %then_736, label %merge_737

merge_737:                                        ; preds = %merge_735
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_738, label %merge_739

merge_739:                                        ; preds = %merge_737
  %"%d.load8" = load i64, ptr %d1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 3
  br i1 %cmpeqtmp9, label %then_740, label %merge_741

merge_741:                                        ; preds = %merge_739
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 4
  br i1 %cmpeqtmp12, label %then_742, label %merge_743

merge_743:                                        ; preds = %merge_741
  %"%d.load14" = load i64, ptr %d1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%d.load14", 5
  br i1 %cmpeqtmp15, label %then_744, label %merge_745

merge_745:                                        ; preds = %merge_743
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 6
  br i1 %cmpeqtmp18, label %then_746, label %merge_747

merge_747:                                        ; preds = %merge_745
  %"%d.load20" = load i64, ptr %d1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%d.load20", 7
  br i1 %cmpeqtmp21, label %then_748, label %merge_749

merge_749:                                        ; preds = %merge_747
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 8
  br i1 %cmpeqtmp24, label %then_750, label %merge_751

merge_751:                                        ; preds = %merge_749
  %"%d.load26" = load i64, ptr %d1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%d.load26", 9
  br i1 %cmpeqtmp27, label %then_752, label %merge_753

merge_753:                                        ; preds = %merge_751
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 10
  br i1 %cmpeqtmp30, label %then_754, label %merge_755

merge_755:                                        ; preds = %merge_753
  %"%d.load32" = load i64, ptr %d1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%d.load32", 11
  br i1 %cmpeqtmp33, label %then_756, label %merge_757

merge_757:                                        ; preds = %merge_755
  %"%d.load35" = load i64, ptr %d1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%d.load35", 12
  br i1 %cmpeqtmp36, label %then_758, label %merge_759

merge_759:                                        ; preds = %merge_757
  %"%d.load38" = load i64, ptr %d1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%d.load38", 13
  br i1 %cmpeqtmp39, label %then_760, label %merge_761

merge_761:                                        ; preds = %merge_759
  %"%d.load41" = load i64, ptr %d1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%d.load41", 14
  br i1 %cmpeqtmp42, label %then_762, label %merge_763

merge_763:                                        ; preds = %merge_761
  %"%d.load44" = load i64, ptr %d1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%d.load44", 15
  br i1 %cmpeqtmp45, label %then_764, label %merge_765

merge_765:                                        ; preds = %merge_763
  ret i64 -1

then_734:                                         ; preds = %entry
  %loadtmp = load i64, ptr @drv_pclass_0, align 8
  ret i64 %loadtmp

then_736:                                         ; preds = %merge_735
  %loadtmp4 = load i64, ptr @drv_pclass_1, align 8
  ret i64 %loadtmp4

then_738:                                         ; preds = %merge_737
  %loadtmp7 = load i64, ptr @drv_pclass_2, align 8
  ret i64 %loadtmp7

then_740:                                         ; preds = %merge_739
  %loadtmp10 = load i64, ptr @drv_pclass_3, align 8
  ret i64 %loadtmp10

then_742:                                         ; preds = %merge_741
  %loadtmp13 = load i64, ptr @drv_pclass_4, align 8
  ret i64 %loadtmp13

then_744:                                         ; preds = %merge_743
  %loadtmp16 = load i64, ptr @drv_pclass_5, align 8
  ret i64 %loadtmp16

then_746:                                         ; preds = %merge_745
  %loadtmp19 = load i64, ptr @drv_pclass_6, align 8
  ret i64 %loadtmp19

then_748:                                         ; preds = %merge_747
  %loadtmp22 = load i64, ptr @drv_pclass_7, align 8
  ret i64 %loadtmp22

then_750:                                         ; preds = %merge_749
  %loadtmp25 = load i64, ptr @drv_pclass_8, align 8
  ret i64 %loadtmp25

then_752:                                         ; preds = %merge_751
  %loadtmp28 = load i64, ptr @drv_pclass_9, align 8
  ret i64 %loadtmp28

then_754:                                         ; preds = %merge_753
  %loadtmp31 = load i64, ptr @drv_pclass_10, align 8
  ret i64 %loadtmp31

then_756:                                         ; preds = %merge_755
  %loadtmp34 = load i64, ptr @drv_pclass_11, align 8
  ret i64 %loadtmp34

then_758:                                         ; preds = %merge_757
  %loadtmp37 = load i64, ptr @drv_pclass_12, align 8
  ret i64 %loadtmp37

then_760:                                         ; preds = %merge_759
  %loadtmp40 = load i64, ptr @drv_pclass_13, align 8
  ret i64 %loadtmp40

then_762:                                         ; preds = %merge_761
  %loadtmp43 = load i64, ptr @drv_pclass_14, align 8
  ret i64 %loadtmp43

then_764:                                         ; preds = %merge_763
  %loadtmp46 = load i64, ptr @drv_pclass_15, align 8
  ret i64 %loadtmp46
}

define i64 @dm_inc_drv_devcount(i64 %d) {
entry:
  %"%drv_devcount_15" = alloca i64, align 8
  %"%drv_devcount_14" = alloca i64, align 8
  %"%drv_devcount_13" = alloca i64, align 8
  %"%drv_devcount_12" = alloca i64, align 8
  %"%drv_devcount_11" = alloca i64, align 8
  %"%drv_devcount_10" = alloca i64, align 8
  %"%drv_devcount_9" = alloca i64, align 8
  %"%drv_devcount_8" = alloca i64, align 8
  %"%drv_devcount_7" = alloca i64, align 8
  %"%drv_devcount_6" = alloca i64, align 8
  %"%drv_devcount_5" = alloca i64, align 8
  %"%drv_devcount_4" = alloca i64, align 8
  %"%drv_devcount_3" = alloca i64, align 8
  %"%drv_devcount_2" = alloca i64, align 8
  %"%drv_devcount_1" = alloca i64, align 8
  %"%drv_devcount_0" = alloca i64, align 8
  %d1 = alloca i64, align 8
  store i64 %d, ptr %d1, align 8
  %"%d.load" = load i64, ptr %d1, align 8
  %cmpeqtmp = icmp eq i64 %"%d.load", 0
  br i1 %cmpeqtmp, label %then_766, label %merge_767

merge_767:                                        ; preds = %then_766, %entry
  %"%d.load2" = load i64, ptr %d1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%d.load2", 1
  br i1 %cmpeqtmp3, label %then_768, label %merge_769

merge_769:                                        ; preds = %then_768, %merge_767
  %"%d.load5" = load i64, ptr %d1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%d.load5", 2
  br i1 %cmpeqtmp6, label %then_770, label %merge_771

merge_771:                                        ; preds = %then_770, %merge_769
  %"%d.load8" = load i64, ptr %d1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d.load8", 3
  br i1 %cmpeqtmp9, label %then_772, label %merge_773

merge_773:                                        ; preds = %then_772, %merge_771
  %"%d.load11" = load i64, ptr %d1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%d.load11", 4
  br i1 %cmpeqtmp12, label %then_774, label %merge_775

merge_775:                                        ; preds = %then_774, %merge_773
  %"%d.load14" = load i64, ptr %d1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%d.load14", 5
  br i1 %cmpeqtmp15, label %then_776, label %merge_777

merge_777:                                        ; preds = %then_776, %merge_775
  %"%d.load17" = load i64, ptr %d1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d.load17", 6
  br i1 %cmpeqtmp18, label %then_778, label %merge_779

merge_779:                                        ; preds = %then_778, %merge_777
  %"%d.load20" = load i64, ptr %d1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%d.load20", 7
  br i1 %cmpeqtmp21, label %then_780, label %merge_781

merge_781:                                        ; preds = %then_780, %merge_779
  %"%d.load23" = load i64, ptr %d1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%d.load23", 8
  br i1 %cmpeqtmp24, label %then_782, label %merge_783

merge_783:                                        ; preds = %then_782, %merge_781
  %"%d.load26" = load i64, ptr %d1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%d.load26", 9
  br i1 %cmpeqtmp27, label %then_784, label %merge_785

merge_785:                                        ; preds = %then_784, %merge_783
  %"%d.load29" = load i64, ptr %d1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%d.load29", 10
  br i1 %cmpeqtmp30, label %then_786, label %merge_787

merge_787:                                        ; preds = %then_786, %merge_785
  %"%d.load32" = load i64, ptr %d1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%d.load32", 11
  br i1 %cmpeqtmp33, label %then_788, label %merge_789

merge_789:                                        ; preds = %then_788, %merge_787
  %"%d.load35" = load i64, ptr %d1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%d.load35", 12
  br i1 %cmpeqtmp36, label %then_790, label %merge_791

merge_791:                                        ; preds = %then_790, %merge_789
  %"%d.load38" = load i64, ptr %d1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%d.load38", 13
  br i1 %cmpeqtmp39, label %then_792, label %merge_793

merge_793:                                        ; preds = %then_792, %merge_791
  %"%d.load41" = load i64, ptr %d1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%d.load41", 14
  br i1 %cmpeqtmp42, label %then_794, label %merge_795

merge_795:                                        ; preds = %then_794, %merge_793
  %"%d.load44" = load i64, ptr %d1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%d.load44", 15
  br i1 %cmpeqtmp45, label %then_796, label %merge_797

merge_797:                                        ; preds = %then_796, %merge_795
  ret i64 0

then_766:                                         ; preds = %entry
  %"%drv_devcount_0.load" = load i64, ptr @drv_devcount_0, align 8
  %addtmp = add i64 %"%drv_devcount_0.load", 1
  store i64 %addtmp, ptr %"%drv_devcount_0", align 8
  br label %merge_767

then_768:                                         ; preds = %merge_767
  %"%drv_devcount_1.load" = load i64, ptr @drv_devcount_1, align 8
  %addtmp4 = add i64 %"%drv_devcount_1.load", 1
  store i64 %addtmp4, ptr %"%drv_devcount_1", align 8
  br label %merge_769

then_770:                                         ; preds = %merge_769
  %"%drv_devcount_2.load" = load i64, ptr @drv_devcount_2, align 8
  %addtmp7 = add i64 %"%drv_devcount_2.load", 1
  store i64 %addtmp7, ptr %"%drv_devcount_2", align 8
  br label %merge_771

then_772:                                         ; preds = %merge_771
  %"%drv_devcount_3.load" = load i64, ptr @drv_devcount_3, align 8
  %addtmp10 = add i64 %"%drv_devcount_3.load", 1
  store i64 %addtmp10, ptr %"%drv_devcount_3", align 8
  br label %merge_773

then_774:                                         ; preds = %merge_773
  %"%drv_devcount_4.load" = load i64, ptr @drv_devcount_4, align 8
  %addtmp13 = add i64 %"%drv_devcount_4.load", 1
  store i64 %addtmp13, ptr %"%drv_devcount_4", align 8
  br label %merge_775

then_776:                                         ; preds = %merge_775
  %"%drv_devcount_5.load" = load i64, ptr @drv_devcount_5, align 8
  %addtmp16 = add i64 %"%drv_devcount_5.load", 1
  store i64 %addtmp16, ptr %"%drv_devcount_5", align 8
  br label %merge_777

then_778:                                         ; preds = %merge_777
  %"%drv_devcount_6.load" = load i64, ptr @drv_devcount_6, align 8
  %addtmp19 = add i64 %"%drv_devcount_6.load", 1
  store i64 %addtmp19, ptr %"%drv_devcount_6", align 8
  br label %merge_779

then_780:                                         ; preds = %merge_779
  %"%drv_devcount_7.load" = load i64, ptr @drv_devcount_7, align 8
  %addtmp22 = add i64 %"%drv_devcount_7.load", 1
  store i64 %addtmp22, ptr %"%drv_devcount_7", align 8
  br label %merge_781

then_782:                                         ; preds = %merge_781
  %"%drv_devcount_8.load" = load i64, ptr @drv_devcount_8, align 8
  %addtmp25 = add i64 %"%drv_devcount_8.load", 1
  store i64 %addtmp25, ptr %"%drv_devcount_8", align 8
  br label %merge_783

then_784:                                         ; preds = %merge_783
  %"%drv_devcount_9.load" = load i64, ptr @drv_devcount_9, align 8
  %addtmp28 = add i64 %"%drv_devcount_9.load", 1
  store i64 %addtmp28, ptr %"%drv_devcount_9", align 8
  br label %merge_785

then_786:                                         ; preds = %merge_785
  %"%drv_devcount_10.load" = load i64, ptr @drv_devcount_10, align 8
  %addtmp31 = add i64 %"%drv_devcount_10.load", 1
  store i64 %addtmp31, ptr %"%drv_devcount_10", align 8
  br label %merge_787

then_788:                                         ; preds = %merge_787
  %"%drv_devcount_11.load" = load i64, ptr @drv_devcount_11, align 8
  %addtmp34 = add i64 %"%drv_devcount_11.load", 1
  store i64 %addtmp34, ptr %"%drv_devcount_11", align 8
  br label %merge_789

then_790:                                         ; preds = %merge_789
  %"%drv_devcount_12.load" = load i64, ptr @drv_devcount_12, align 8
  %addtmp37 = add i64 %"%drv_devcount_12.load", 1
  store i64 %addtmp37, ptr %"%drv_devcount_12", align 8
  br label %merge_791

then_792:                                         ; preds = %merge_791
  %"%drv_devcount_13.load" = load i64, ptr @drv_devcount_13, align 8
  %addtmp40 = add i64 %"%drv_devcount_13.load", 1
  store i64 %addtmp40, ptr %"%drv_devcount_13", align 8
  br label %merge_793

then_794:                                         ; preds = %merge_793
  %"%drv_devcount_14.load" = load i64, ptr @drv_devcount_14, align 8
  %addtmp43 = add i64 %"%drv_devcount_14.load", 1
  store i64 %addtmp43, ptr %"%drv_devcount_14", align 8
  br label %merge_795

then_796:                                         ; preds = %merge_795
  %"%drv_devcount_15.load" = load i64, ptr @drv_devcount_15, align 8
  %addtmp46 = add i64 %"%drv_devcount_15.load", 1
  store i64 %addtmp46, ptr %"%drv_devcount_15", align 8
  br label %merge_797
}

define i64 @dm_get_irq_owner(i64 %irq) {
entry:
  %irq1 = alloca i64, align 8
  store i64 %irq, ptr %irq1, align 8
  %"%irq.load" = load i64, ptr %irq1, align 8
  %cmpeqtmp = icmp eq i64 %"%irq.load", 0
  br i1 %cmpeqtmp, label %then_798, label %merge_799

merge_799:                                        ; preds = %entry
  %"%irq.load2" = load i64, ptr %irq1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%irq.load2", 1
  br i1 %cmpeqtmp3, label %then_800, label %merge_801

merge_801:                                        ; preds = %merge_799
  %"%irq.load5" = load i64, ptr %irq1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%irq.load5", 2
  br i1 %cmpeqtmp6, label %then_802, label %merge_803

merge_803:                                        ; preds = %merge_801
  %"%irq.load8" = load i64, ptr %irq1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%irq.load8", 3
  br i1 %cmpeqtmp9, label %then_804, label %merge_805

merge_805:                                        ; preds = %merge_803
  %"%irq.load11" = load i64, ptr %irq1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%irq.load11", 4
  br i1 %cmpeqtmp12, label %then_806, label %merge_807

merge_807:                                        ; preds = %merge_805
  %"%irq.load14" = load i64, ptr %irq1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%irq.load14", 5
  br i1 %cmpeqtmp15, label %then_808, label %merge_809

merge_809:                                        ; preds = %merge_807
  %"%irq.load17" = load i64, ptr %irq1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%irq.load17", 6
  br i1 %cmpeqtmp18, label %then_810, label %merge_811

merge_811:                                        ; preds = %merge_809
  %"%irq.load20" = load i64, ptr %irq1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%irq.load20", 7
  br i1 %cmpeqtmp21, label %then_812, label %merge_813

merge_813:                                        ; preds = %merge_811
  %"%irq.load23" = load i64, ptr %irq1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%irq.load23", 8
  br i1 %cmpeqtmp24, label %then_814, label %merge_815

merge_815:                                        ; preds = %merge_813
  %"%irq.load26" = load i64, ptr %irq1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%irq.load26", 9
  br i1 %cmpeqtmp27, label %then_816, label %merge_817

merge_817:                                        ; preds = %merge_815
  %"%irq.load29" = load i64, ptr %irq1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%irq.load29", 10
  br i1 %cmpeqtmp30, label %then_818, label %merge_819

merge_819:                                        ; preds = %merge_817
  %"%irq.load32" = load i64, ptr %irq1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%irq.load32", 11
  br i1 %cmpeqtmp33, label %then_820, label %merge_821

merge_821:                                        ; preds = %merge_819
  %"%irq.load35" = load i64, ptr %irq1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%irq.load35", 12
  br i1 %cmpeqtmp36, label %then_822, label %merge_823

merge_823:                                        ; preds = %merge_821
  %"%irq.load38" = load i64, ptr %irq1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%irq.load38", 13
  br i1 %cmpeqtmp39, label %then_824, label %merge_825

merge_825:                                        ; preds = %merge_823
  %"%irq.load41" = load i64, ptr %irq1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%irq.load41", 14
  br i1 %cmpeqtmp42, label %then_826, label %merge_827

merge_827:                                        ; preds = %merge_825
  %"%irq.load44" = load i64, ptr %irq1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%irq.load44", 15
  br i1 %cmpeqtmp45, label %then_828, label %merge_829

merge_829:                                        ; preds = %merge_827
  ret i64 -1

then_798:                                         ; preds = %entry
  %loadtmp = load i64, ptr @irq_owner_0, align 8
  ret i64 %loadtmp

then_800:                                         ; preds = %merge_799
  %loadtmp4 = load i64, ptr @irq_owner_1, align 8
  ret i64 %loadtmp4

then_802:                                         ; preds = %merge_801
  %loadtmp7 = load i64, ptr @irq_owner_2, align 8
  ret i64 %loadtmp7

then_804:                                         ; preds = %merge_803
  %loadtmp10 = load i64, ptr @irq_owner_3, align 8
  ret i64 %loadtmp10

then_806:                                         ; preds = %merge_805
  %loadtmp13 = load i64, ptr @irq_owner_4, align 8
  ret i64 %loadtmp13

then_808:                                         ; preds = %merge_807
  %loadtmp16 = load i64, ptr @irq_owner_5, align 8
  ret i64 %loadtmp16

then_810:                                         ; preds = %merge_809
  %loadtmp19 = load i64, ptr @irq_owner_6, align 8
  ret i64 %loadtmp19

then_812:                                         ; preds = %merge_811
  %loadtmp22 = load i64, ptr @irq_owner_7, align 8
  ret i64 %loadtmp22

then_814:                                         ; preds = %merge_813
  %loadtmp25 = load i64, ptr @irq_owner_8, align 8
  ret i64 %loadtmp25

then_816:                                         ; preds = %merge_815
  %loadtmp28 = load i64, ptr @irq_owner_9, align 8
  ret i64 %loadtmp28

then_818:                                         ; preds = %merge_817
  %loadtmp31 = load i64, ptr @irq_owner_10, align 8
  ret i64 %loadtmp31

then_820:                                         ; preds = %merge_819
  %loadtmp34 = load i64, ptr @irq_owner_11, align 8
  ret i64 %loadtmp34

then_822:                                         ; preds = %merge_821
  %loadtmp37 = load i64, ptr @irq_owner_12, align 8
  ret i64 %loadtmp37

then_824:                                         ; preds = %merge_823
  %loadtmp40 = load i64, ptr @irq_owner_13, align 8
  ret i64 %loadtmp40

then_826:                                         ; preds = %merge_825
  %loadtmp43 = load i64, ptr @irq_owner_14, align 8
  ret i64 %loadtmp43

then_828:                                         ; preds = %merge_827
  %loadtmp46 = load i64, ptr @irq_owner_15, align 8
  ret i64 %loadtmp46
}

define i64 @dm_set_irq_owner(i64 %irq, i64 %dev) {
entry:
  %"%irq_owner_15" = alloca i64, align 8
  %"%irq_owner_14" = alloca i64, align 8
  %"%irq_owner_13" = alloca i64, align 8
  %"%irq_owner_12" = alloca i64, align 8
  %"%irq_owner_11" = alloca i64, align 8
  %"%irq_owner_10" = alloca i64, align 8
  %"%irq_owner_9" = alloca i64, align 8
  %"%irq_owner_8" = alloca i64, align 8
  %"%irq_owner_7" = alloca i64, align 8
  %"%irq_owner_6" = alloca i64, align 8
  %"%irq_owner_5" = alloca i64, align 8
  %"%irq_owner_4" = alloca i64, align 8
  %"%irq_owner_3" = alloca i64, align 8
  %"%irq_owner_2" = alloca i64, align 8
  %"%irq_owner_1" = alloca i64, align 8
  %"%irq_owner_0" = alloca i64, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %irq1 = alloca i64, align 8
  store i64 %irq, ptr %irq1, align 8
  %"%irq.load" = load i64, ptr %irq1, align 8
  %cmpeqtmp = icmp eq i64 %"%irq.load", 0
  br i1 %cmpeqtmp, label %then_830, label %merge_831

merge_831:                                        ; preds = %then_830, %entry
  %"%irq.load3" = load i64, ptr %irq1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%irq.load3", 1
  br i1 %cmpeqtmp4, label %then_832, label %merge_833

merge_833:                                        ; preds = %then_832, %merge_831
  %"%irq.load5" = load i64, ptr %irq1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%irq.load5", 2
  br i1 %cmpeqtmp6, label %then_834, label %merge_835

merge_835:                                        ; preds = %then_834, %merge_833
  %"%irq.load7" = load i64, ptr %irq1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%irq.load7", 3
  br i1 %cmpeqtmp8, label %then_836, label %merge_837

merge_837:                                        ; preds = %then_836, %merge_835
  %"%irq.load9" = load i64, ptr %irq1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%irq.load9", 4
  br i1 %cmpeqtmp10, label %then_838, label %merge_839

merge_839:                                        ; preds = %then_838, %merge_837
  %"%irq.load11" = load i64, ptr %irq1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%irq.load11", 5
  br i1 %cmpeqtmp12, label %then_840, label %merge_841

merge_841:                                        ; preds = %then_840, %merge_839
  %"%irq.load13" = load i64, ptr %irq1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%irq.load13", 6
  br i1 %cmpeqtmp14, label %then_842, label %merge_843

merge_843:                                        ; preds = %then_842, %merge_841
  %"%irq.load15" = load i64, ptr %irq1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%irq.load15", 7
  br i1 %cmpeqtmp16, label %then_844, label %merge_845

merge_845:                                        ; preds = %then_844, %merge_843
  %"%irq.load17" = load i64, ptr %irq1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%irq.load17", 8
  br i1 %cmpeqtmp18, label %then_846, label %merge_847

merge_847:                                        ; preds = %then_846, %merge_845
  %"%irq.load19" = load i64, ptr %irq1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%irq.load19", 9
  br i1 %cmpeqtmp20, label %then_848, label %merge_849

merge_849:                                        ; preds = %then_848, %merge_847
  %"%irq.load21" = load i64, ptr %irq1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%irq.load21", 10
  br i1 %cmpeqtmp22, label %then_850, label %merge_851

merge_851:                                        ; preds = %then_850, %merge_849
  %"%irq.load23" = load i64, ptr %irq1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%irq.load23", 11
  br i1 %cmpeqtmp24, label %then_852, label %merge_853

merge_853:                                        ; preds = %then_852, %merge_851
  %"%irq.load25" = load i64, ptr %irq1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%irq.load25", 12
  br i1 %cmpeqtmp26, label %then_854, label %merge_855

merge_855:                                        ; preds = %then_854, %merge_853
  %"%irq.load27" = load i64, ptr %irq1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%irq.load27", 13
  br i1 %cmpeqtmp28, label %then_856, label %merge_857

merge_857:                                        ; preds = %then_856, %merge_855
  %"%irq.load29" = load i64, ptr %irq1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%irq.load29", 14
  br i1 %cmpeqtmp30, label %then_858, label %merge_859

merge_859:                                        ; preds = %then_858, %merge_857
  %"%irq.load31" = load i64, ptr %irq1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%irq.load31", 15
  br i1 %cmpeqtmp32, label %then_860, label %merge_861

merge_861:                                        ; preds = %then_860, %merge_859
  ret i64 0

then_830:                                         ; preds = %entry
  %"%1871.load" = load i64, ptr %dev2, align 8
  store i64 %"%1871.load", ptr %"%irq_owner_0", align 8
  br label %merge_831

then_832:                                         ; preds = %merge_831
  %"%1875.load" = load i64, ptr %dev2, align 8
  store i64 %"%1875.load", ptr %"%irq_owner_1", align 8
  br label %merge_833

then_834:                                         ; preds = %merge_833
  %"%1879.load" = load i64, ptr %dev2, align 8
  store i64 %"%1879.load", ptr %"%irq_owner_2", align 8
  br label %merge_835

then_836:                                         ; preds = %merge_835
  %"%1883.load" = load i64, ptr %dev2, align 8
  store i64 %"%1883.load", ptr %"%irq_owner_3", align 8
  br label %merge_837

then_838:                                         ; preds = %merge_837
  %"%1887.load" = load i64, ptr %dev2, align 8
  store i64 %"%1887.load", ptr %"%irq_owner_4", align 8
  br label %merge_839

then_840:                                         ; preds = %merge_839
  %"%1891.load" = load i64, ptr %dev2, align 8
  store i64 %"%1891.load", ptr %"%irq_owner_5", align 8
  br label %merge_841

then_842:                                         ; preds = %merge_841
  %"%1895.load" = load i64, ptr %dev2, align 8
  store i64 %"%1895.load", ptr %"%irq_owner_6", align 8
  br label %merge_843

then_844:                                         ; preds = %merge_843
  %"%1899.load" = load i64, ptr %dev2, align 8
  store i64 %"%1899.load", ptr %"%irq_owner_7", align 8
  br label %merge_845

then_846:                                         ; preds = %merge_845
  %"%1903.load" = load i64, ptr %dev2, align 8
  store i64 %"%1903.load", ptr %"%irq_owner_8", align 8
  br label %merge_847

then_848:                                         ; preds = %merge_847
  %"%1907.load" = load i64, ptr %dev2, align 8
  store i64 %"%1907.load", ptr %"%irq_owner_9", align 8
  br label %merge_849

then_850:                                         ; preds = %merge_849
  %"%1911.load" = load i64, ptr %dev2, align 8
  store i64 %"%1911.load", ptr %"%irq_owner_10", align 8
  br label %merge_851

then_852:                                         ; preds = %merge_851
  %"%1915.load" = load i64, ptr %dev2, align 8
  store i64 %"%1915.load", ptr %"%irq_owner_11", align 8
  br label %merge_853

then_854:                                         ; preds = %merge_853
  %"%1919.load" = load i64, ptr %dev2, align 8
  store i64 %"%1919.load", ptr %"%irq_owner_12", align 8
  br label %merge_855

then_856:                                         ; preds = %merge_855
  %"%1923.load" = load i64, ptr %dev2, align 8
  store i64 %"%1923.load", ptr %"%irq_owner_13", align 8
  br label %merge_857

then_858:                                         ; preds = %merge_857
  %"%1927.load" = load i64, ptr %dev2, align 8
  store i64 %"%1927.load", ptr %"%irq_owner_14", align 8
  br label %merge_859

then_860:                                         ; preds = %merge_859
  %"%1931.load" = load i64, ptr %dev2, align 8
  store i64 %"%1931.load", ptr %"%irq_owner_15", align 8
  br label %merge_861
}

define i64 @dm_allocate_irq(i64 %irq, i64 %dev) {
entry:
  %"%owner" = alloca i64, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %irq1 = alloca i64, align 8
  store i64 %irq, ptr %irq1, align 8
  %"%irq.load" = load i64, ptr %irq1, align 8
  %cmplttmp = icmp slt i64 %"%irq.load", 0
  br i1 %cmplttmp, label %then_862, label %merge_863

merge_863:                                        ; preds = %entry
  %"%irq.load3" = load i64, ptr %irq1, align 8
  %cmpgttmp = icmp sgt i64 %"%irq.load3", 15
  br i1 %cmpgttmp, label %then_864, label %merge_865

merge_865:                                        ; preds = %merge_863
  %"%irq.load4" = load i64, ptr %irq1, align 8
  %calltmp = call i64 @dm_get_irq_owner(i64 %"%irq.load4")
  store i64 %calltmp, ptr %"%owner", align 8
  %"%owner.load" = load i64, ptr %"%owner", align 8
  %cmpgetmp = icmp sge i64 %"%owner.load", 0
  br i1 %cmpgetmp, label %then_866, label %merge_867

merge_867:                                        ; preds = %merge_869, %merge_865
  %"%irq.load6" = load i64, ptr %irq1, align 8
  %"%dev.load7" = load i64, ptr %dev2, align 8
  %calltmp8 = call i64 @dm_set_irq_owner(i64 %"%irq.load6", i64 %"%dev.load7")
  %"%dev.load9" = load i64, ptr %dev2, align 8
  %"%irq.load10" = load i64, ptr %irq1, align 8
  %calltmp11 = call i64 @dm_set_irq(i64 %"%dev.load9", i64 %"%irq.load10")
  ret i64 0

merge_869:                                        ; preds = %then_866
  br label %merge_867

then_862:                                         ; preds = %entry
  ret i64 -1

then_864:                                         ; preds = %merge_863
  ret i64 -2

then_866:                                         ; preds = %merge_865
  %"%owner.load5" = load i64, ptr %"%owner", align 8
  %"%dev.load" = load i64, ptr %dev2, align 8
  %cmpnetmp = icmp ne i64 %"%owner.load5", %"%dev.load"
  br i1 %cmpnetmp, label %then_868, label %merge_869

then_868:                                         ; preds = %then_866
  %calltmp12 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %"%1954.load" = load i64, ptr %irq1, align 8
  %calltmp13 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%1954.load")
  %calltmp14 = call i64 @"dm_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -3
}

define i64 @dm_register_device(i64 %dtype, i64 %vid, i64 %did, i64 %parent, i64 %cls, i64 %scls, i64 %irq, i64 %io_base, i64 %mmio_base) {
entry:
  %"%devmgr_dev_count" = alloca i64, align 8
  %"%idx" = alloca i64, align 8
  %mmio_base9 = alloca i64, align 8
  store i64 %mmio_base, ptr %mmio_base9, align 8
  %io_base8 = alloca i64, align 8
  store i64 %io_base, ptr %io_base8, align 8
  %irq7 = alloca i64, align 8
  store i64 %irq, ptr %irq7, align 8
  %scls6 = alloca i64, align 8
  store i64 %scls, ptr %scls6, align 8
  %cls5 = alloca i64, align 8
  store i64 %cls, ptr %cls5, align 8
  %parent4 = alloca i64, align 8
  store i64 %parent, ptr %parent4, align 8
  %did3 = alloca i64, align 8
  store i64 %did, ptr %did3, align 8
  %vid2 = alloca i64, align 8
  store i64 %vid, ptr %vid2, align 8
  %dtype1 = alloca i64, align 8
  store i64 %dtype, ptr %dtype1, align 8
  %"%1965.load" = load i64, ptr @devmgr_dev_count, align 8
  store i64 %"%1965.load", ptr %"%idx", align 8
  %"%idx.load" = load i64, ptr %"%idx", align 8
  %cmpgetmp = icmp sge i64 %"%idx.load", 32
  br i1 %cmpgetmp, label %then_870, label %merge_871

merge_871:                                        ; preds = %entry
  %"%idx.load10" = load i64, ptr %"%idx", align 8
  %"%dtype.load" = load i64, ptr %dtype1, align 8
  %calltmp = call i64 @dm_set_type(i64 %"%idx.load10", i64 %"%dtype.load")
  %"%idx.load11" = load i64, ptr %"%idx", align 8
  %"%DEV_STATE_DETECTED.load" = load i64, ptr @DEV_STATE_DETECTED, align 8
  %calltmp12 = call i64 @dm_set_state(i64 %"%idx.load11", i64 %"%DEV_STATE_DETECTED.load")
  %"%idx.load13" = load i64, ptr %"%idx", align 8
  %"%vid.load" = load i64, ptr %vid2, align 8
  %calltmp14 = call i64 @dm_set_vid(i64 %"%idx.load13", i64 %"%vid.load")
  %"%idx.load15" = load i64, ptr %"%idx", align 8
  %"%did.load" = load i64, ptr %did3, align 8
  %calltmp16 = call i64 @dm_set_did(i64 %"%idx.load15", i64 %"%did.load")
  %"%idx.load17" = load i64, ptr %"%idx", align 8
  %"%parent.load" = load i64, ptr %parent4, align 8
  %calltmp18 = call i64 @dm_set_parent(i64 %"%idx.load17", i64 %"%parent.load")
  %"%idx.load19" = load i64, ptr %"%idx", align 8
  %"%cls.load" = load i64, ptr %cls5, align 8
  %calltmp20 = call i64 @dm_set_class(i64 %"%idx.load19", i64 %"%cls.load")
  %"%idx.load21" = load i64, ptr %"%idx", align 8
  %"%scls.load" = load i64, ptr %scls6, align 8
  %calltmp22 = call i64 @dm_set_sub(i64 %"%idx.load21", i64 %"%scls.load")
  %"%idx.load23" = load i64, ptr %"%idx", align 8
  %"%io_base.load" = load i64, ptr %io_base8, align 8
  %calltmp24 = call i64 @dm_set_io(i64 %"%idx.load23", i64 %"%io_base.load")
  %"%idx.load25" = load i64, ptr %"%idx", align 8
  %"%mmio_base.load" = load i64, ptr %mmio_base9, align 8
  %calltmp26 = call i64 @dm_set_mmio(i64 %"%idx.load25", i64 %"%mmio_base.load")
  %"%irq.load" = load i64, ptr %irq7, align 8
  %cmpgetmp27 = icmp sge i64 %"%irq.load", 0
  br i1 %cmpgetmp27, label %then_872, label %merge_873

merge_873:                                        ; preds = %then_872, %merge_871
  %calltmp31 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %"%1987.load" = load i64, ptr %"%idx", align 8
  %calltmp32 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%1987.load")
  %calltmp33 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %"%1992.load" = load i64, ptr %dtype1, align 8
  %calltmp34 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%1992.load")
  %calltmp35 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %"%1997.load" = load i64, ptr %vid2, align 8
  %calltmp36 = call i64 @dm_serial_hex16(i64 %"%1997.load")
  %calltmp37 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%2002.load" = load i64, ptr %did3, align 8
  %calltmp38 = call i64 @dm_serial_hex16(i64 %"%2002.load")
  %calltmp39 = call i64 @"dm_serial_\D8\B3\D8\B7\D8\B1"()
  %"%devmgr_dev_count.load" = load i64, ptr @devmgr_dev_count, align 8
  %addtmp = add i64 %"%devmgr_dev_count.load", 1
  store i64 %addtmp, ptr %"%devmgr_dev_count", align 8
  %loadtmp = load i64, ptr %"%idx", align 8
  ret i64 %loadtmp

then_870:                                         ; preds = %entry
  ret i64 -1

then_872:                                         ; preds = %merge_871
  %"%irq.load28" = load i64, ptr %irq7, align 8
  %"%idx.load29" = load i64, ptr %"%idx", align 8
  %calltmp30 = call i64 @dm_allocate_irq(i64 %"%irq.load28", i64 %"%idx.load29")
  br label %merge_873
}

define i64 @dm_register_driver(i64 %dtype, i64 %pclass, i64 %psub) {
entry:
  %"%devmgr_drv_count" = alloca i64, align 8
  %"%idx" = alloca i64, align 8
  %psub3 = alloca i64, align 8
  store i64 %psub, ptr %psub3, align 8
  %pclass2 = alloca i64, align 8
  store i64 %pclass, ptr %pclass2, align 8
  %dtype1 = alloca i64, align 8
  store i64 %dtype, ptr %dtype1, align 8
  %"%2011.load" = load i64, ptr @devmgr_drv_count, align 8
  store i64 %"%2011.load", ptr %"%idx", align 8
  %"%idx.load" = load i64, ptr %"%idx", align 8
  %cmpgetmp = icmp sge i64 %"%idx.load", 16
  br i1 %cmpgetmp, label %then_874, label %merge_875

merge_875:                                        ; preds = %entry
  %"%idx.load4" = load i64, ptr %"%idx", align 8
  %"%dtype.load" = load i64, ptr %dtype1, align 8
  %calltmp = call i64 @dm_set_drv_type(i64 %"%idx.load4", i64 %"%dtype.load")
  %"%idx.load5" = load i64, ptr %"%idx", align 8
  %calltmp6 = call i64 @dm_set_drv_reg(i64 %"%idx.load5", i64 1)
  %"%idx.load7" = load i64, ptr %"%idx", align 8
  %"%pclass.load" = load i64, ptr %pclass2, align 8
  %calltmp8 = call i64 @dm_set_drv_pclass(i64 %"%idx.load7", i64 %"%pclass.load")
  %calltmp9 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %"%2025.load" = load i64, ptr %"%idx", align 8
  %calltmp10 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%2025.load")
  %calltmp11 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  %"%2030.load" = load i64, ptr %dtype1, align 8
  %calltmp12 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%2030.load")
  %calltmp13 = call i64 @"dm_serial_\D8\B3\D8\B7\D8\B1"()
  %"%devmgr_drv_count.load" = load i64, ptr @devmgr_drv_count, align 8
  %addtmp = add i64 %"%devmgr_drv_count.load", 1
  store i64 %addtmp, ptr %"%devmgr_drv_count", align 8
  %loadtmp = load i64, ptr %"%idx", align 8
  ret i64 %loadtmp

then_874:                                         ; preds = %entry
  ret i64 -1
}

define i64 @dm_auto_bind() {
entry:
  %"%drv_t" = alloca i64, align 8
  %"%drv" = alloca i64, align 8
  %"%devmgr_bindings" = alloca i64, align 8
  %"%dstate" = alloca i64, align 8
  %"%dtype" = alloca i64, align 8
  %"%d" = alloca i64, align 8
  %"%bound" = alloca i64, align 8
  store i64 0, ptr %"%bound", align 8
  store i64 0, ptr %"%d", align 8
  br label %while_cond_876

merge_880:                                        ; preds = %while_exit_883, %while_body_877
  %"%d.load4" = load i64, ptr %"%d", align 8
  %addtmp5 = add i64 %"%d.load4", 1
  store i64 %addtmp5, ptr %"%d", align 8
  br label %while_cond_876

merge_885:                                        ; preds = %then_884, %while_body_882
  %"%drv.load24" = load i64, ptr %"%drv", align 8
  %addtmp25 = add i64 %"%drv.load24", 1
  store i64 %addtmp25, ptr %"%drv", align 8
  br label %while_cond_881

then_879:                                         ; preds = %while_body_877
  store i64 0, ptr %"%drv", align 8
  br label %while_cond_881

then_884:                                         ; preds = %while_body_882
  %"%d.load10" = load i64, ptr %"%d", align 8
  %"%drv.load11" = load i64, ptr %"%drv", align 8
  %calltmp12 = call i64 @dm_set_driver(i64 %"%d.load10", i64 %"%drv.load11")
  %"%d.load13" = load i64, ptr %"%d", align 8
  %"%DEV_STATE_INIT.load" = load i64, ptr @DEV_STATE_INIT, align 8
  %calltmp14 = call i64 @dm_set_state(i64 %"%d.load13", i64 %"%DEV_STATE_INIT.load")
  %"%drv.load15" = load i64, ptr %"%drv", align 8
  %calltmp16 = call i64 @dm_inc_drv_devcount(i64 %"%drv.load15")
  %"%bound.load17" = load i64, ptr %"%bound", align 8
  %addtmp18 = add i64 %"%bound.load17", 1
  store i64 %addtmp18, ptr %"%bound", align 8
  %calltmp19 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %"%2068.load" = load i64, ptr %"%d", align 8
  %calltmp20 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%2068.load")
  %calltmp21 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %"%2073.load" = load i64, ptr %"%drv", align 8
  %calltmp22 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%2073.load")
  %calltmp23 = call i64 @"dm_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_885

while_body_877:                                   ; preds = %while_cond_876
  %"%d.load1" = load i64, ptr %"%d", align 8
  %calltmp = call i64 @dm_get_type(i64 %"%d.load1")
  store i64 %calltmp, ptr %"%dtype", align 8
  %"%d.load2" = load i64, ptr %"%d", align 8
  %calltmp3 = call i64 @dm_get_state(i64 %"%d.load2")
  store i64 %calltmp3, ptr %"%dstate", align 8
  %"%dstate.load" = load i64, ptr %"%dstate", align 8
  %"%DEV_STATE_DETECTED.load" = load i64, ptr @DEV_STATE_DETECTED, align 8
  %cmpeqtmp = icmp eq i64 %"%dstate.load", %"%DEV_STATE_DETECTED.load"
  br i1 %cmpeqtmp, label %then_879, label %merge_880

while_body_882:                                   ; preds = %while_cond_881
  %"%drv.load7" = load i64, ptr %"%drv", align 8
  %calltmp8 = call i64 @dm_get_drv_type(i64 %"%drv.load7")
  store i64 %calltmp8, ptr %"%drv_t", align 8
  %"%drv_t.load" = load i64, ptr %"%drv_t", align 8
  %"%dtype.load" = load i64, ptr %"%dtype", align 8
  %cmpeqtmp9 = icmp eq i64 %"%drv_t.load", %"%dtype.load"
  br i1 %cmpeqtmp9, label %then_884, label %merge_885

while_cond_876:                                   ; preds = %merge_880, %entry
  %"%d.load" = load i64, ptr %"%d", align 8
  %"%devmgr_dev_count.load" = load i64, ptr @devmgr_dev_count, align 8
  %cmplttmp = icmp slt i64 %"%d.load", %"%devmgr_dev_count.load"
  br i1 %cmplttmp, label %while_body_877, label %while_exit_878

while_cond_881:                                   ; preds = %merge_885, %then_879
  %"%drv.load" = load i64, ptr %"%drv", align 8
  %"%devmgr_drv_count.load" = load i64, ptr @devmgr_drv_count, align 8
  %cmplttmp6 = icmp slt i64 %"%drv.load", %"%devmgr_drv_count.load"
  br i1 %cmplttmp6, label %while_body_882, label %while_exit_883

while_exit_878:                                   ; preds = %while_cond_876
  %"%devmgr_bindings.load" = load i64, ptr @devmgr_bindings, align 8
  %"%bound.load" = load i64, ptr %"%bound", align 8
  %addtmp = add i64 %"%devmgr_bindings.load", %"%bound.load"
  store i64 %addtmp, ptr %"%devmgr_bindings", align 8
  %loadtmp = load i64, ptr %"%bound", align 8
  ret i64 %loadtmp

while_exit_883:                                   ; preds = %while_cond_881
  br label %merge_880
}

define i64 @dm_find_device_by_type(i64 %dtype) {
entry:
  %"%d" = alloca i64, align 8
  %dtype1 = alloca i64, align 8
  store i64 %dtype, ptr %dtype1, align 8
  store i64 0, ptr %"%d", align 8
  br label %while_cond_886

merge_890:                                        ; preds = %while_body_887
  %"%d.load3" = load i64, ptr %"%d", align 8
  %addtmp = add i64 %"%d.load3", 1
  store i64 %addtmp, ptr %"%d", align 8
  br label %while_cond_886

then_889:                                         ; preds = %while_body_887
  %loadtmp = load i64, ptr %"%d", align 8
  ret i64 %loadtmp

while_body_887:                                   ; preds = %while_cond_886
  %"%d.load2" = load i64, ptr %"%d", align 8
  %calltmp = call i64 @dm_get_type(i64 %"%d.load2")
  %"%dtype.load" = load i64, ptr %dtype1, align 8
  %cmpeqtmp = icmp eq i64 %calltmp, %"%dtype.load"
  br i1 %cmpeqtmp, label %then_889, label %merge_890

while_cond_886:                                   ; preds = %merge_890, %entry
  %"%d.load" = load i64, ptr %"%d", align 8
  %"%devmgr_dev_count.load" = load i64, ptr @devmgr_dev_count, align 8
  %cmplttmp = icmp slt i64 %"%d.load", %"%devmgr_dev_count.load"
  br i1 %cmplttmp, label %while_body_887, label %while_exit_888

while_exit_888:                                   ; preds = %while_cond_886
  ret i64 -1
}

define i64 @dm_find_device_by_class(i64 %cls) {
entry:
  %"%d" = alloca i64, align 8
  %cls1 = alloca i64, align 8
  store i64 %cls, ptr %cls1, align 8
  store i64 0, ptr %"%d", align 8
  br label %while_cond_891

merge_895:                                        ; preds = %while_body_892
  %"%d.load3" = load i64, ptr %"%d", align 8
  %addtmp = add i64 %"%d.load3", 1
  store i64 %addtmp, ptr %"%d", align 8
  br label %while_cond_891

then_894:                                         ; preds = %while_body_892
  %loadtmp = load i64, ptr %"%d", align 8
  ret i64 %loadtmp

while_body_892:                                   ; preds = %while_cond_891
  %"%d.load2" = load i64, ptr %"%d", align 8
  %calltmp = call i64 @dm_get_class(i64 %"%d.load2")
  %"%cls.load" = load i64, ptr %cls1, align 8
  %cmpeqtmp = icmp eq i64 %calltmp, %"%cls.load"
  br i1 %cmpeqtmp, label %then_894, label %merge_895

while_cond_891:                                   ; preds = %merge_895, %entry
  %"%d.load" = load i64, ptr %"%d", align 8
  %"%devmgr_dev_count.load" = load i64, ptr @devmgr_dev_count, align 8
  %cmplttmp = icmp slt i64 %"%d.load", %"%devmgr_dev_count.load"
  br i1 %cmplttmp, label %while_body_892, label %while_exit_893

while_exit_893:                                   ; preds = %while_cond_891
  ret i64 -1
}

define i64 @dm_register_builtin_drivers() {
entry:
  %"%DEV_TYPE_BUS.load" = load i64, ptr @DEV_TYPE_BUS, align 8
  %calltmp = call i64 @dm_register_driver(i64 %"%DEV_TYPE_BUS.load", i64 6, i64 0)
  %"%DEV_TYPE_ATA.load" = load i64, ptr @DEV_TYPE_ATA, align 8
  %calltmp1 = call i64 @dm_register_driver(i64 %"%DEV_TYPE_ATA.load", i64 1, i64 1)
  %"%DEV_TYPE_USB.load" = load i64, ptr @DEV_TYPE_USB, align 8
  %calltmp2 = call i64 @dm_register_driver(i64 %"%DEV_TYPE_USB.load", i64 12, i64 3)
  %"%DEV_TYPE_NET.load" = load i64, ptr @DEV_TYPE_NET, align 8
  %calltmp3 = call i64 @dm_register_driver(i64 %"%DEV_TYPE_NET.load", i64 2, i64 0)
  %"%DEV_TYPE_GFX.load" = load i64, ptr @DEV_TYPE_GFX, align 8
  %calltmp4 = call i64 @dm_register_driver(i64 %"%DEV_TYPE_GFX.load", i64 3, i64 0)
  %"%DEV_TYPE_AUDIO.load" = load i64, ptr @DEV_TYPE_AUDIO, align 8
  %calltmp5 = call i64 @dm_register_driver(i64 %"%DEV_TYPE_AUDIO.load", i64 4, i64 0)
  %"%DEV_TYPE_PS2.load" = load i64, ptr @DEV_TYPE_PS2, align 8
  %calltmp6 = call i64 @dm_register_driver(i64 %"%DEV_TYPE_PS2.load", i64 -1, i64 -1)
  %"%DEV_TYPE_ACPI.load" = load i64, ptr @DEV_TYPE_ACPI, align 8
  %calltmp7 = call i64 @dm_register_driver(i64 %"%DEV_TYPE_ACPI.load", i64 -1, i64 -1)
  ret i64 0
}

define i64 @dm_register_platform_devices() {
entry:
  %"%DEV_TYPE_PS2.load" = load i64, ptr @DEV_TYPE_PS2, align 8
  %calltmp = call i64 @dm_register_device(i64 %"%DEV_TYPE_PS2.load", i64 0, i64 0, i64 -1, i64 0, i64 0, i64 1, i64 96, i64 0)
  %"%DEV_TYPE_PS2.load1" = load i64, ptr @DEV_TYPE_PS2, align 8
  %calltmp2 = call i64 @dm_register_device(i64 %"%DEV_TYPE_PS2.load1", i64 0, i64 0, i64 -1, i64 0, i64 0, i64 12, i64 96, i64 0)
  %"%DEV_TYPE_ATA.load" = load i64, ptr @DEV_TYPE_ATA, align 8
  %calltmp3 = call i64 @dm_register_device(i64 %"%DEV_TYPE_ATA.load", i64 0, i64 0, i64 -1, i64 1, i64 1, i64 14, i64 496, i64 0)
  %"%DEV_TYPE_ATA.load4" = load i64, ptr @DEV_TYPE_ATA, align 8
  %calltmp5 = call i64 @dm_register_device(i64 %"%DEV_TYPE_ATA.load4", i64 0, i64 0, i64 -1, i64 1, i64 1, i64 15, i64 368, i64 0)
  %"%DEV_TYPE_SERIAL.load" = load i64, ptr @DEV_TYPE_SERIAL, align 8
  %calltmp6 = call i64 @dm_register_device(i64 %"%DEV_TYPE_SERIAL.load", i64 0, i64 0, i64 -1, i64 0, i64 0, i64 4, i64 1016, i64 0)
  %"%DEV_TYPE_TIMER.load" = load i64, ptr @DEV_TYPE_TIMER, align 8
  %calltmp7 = call i64 @dm_register_device(i64 %"%DEV_TYPE_TIMER.load", i64 0, i64 0, i64 -1, i64 0, i64 0, i64 0, i64 64, i64 0)
  %"%DEV_TYPE_ACPI.load" = load i64, ptr @DEV_TYPE_ACPI, align 8
  %calltmp8 = call i64 @dm_register_device(i64 %"%DEV_TYPE_ACPI.load", i64 0, i64 0, i64 -1, i64 0, i64 0, i64 9, i64 0, i64 0)
  ret i64 0
}

define i64 @dm_count_by_state(i64 %state) {
entry:
  %"%d" = alloca i64, align 8
  %"%count" = alloca i64, align 8
  %state1 = alloca i64, align 8
  store i64 %state, ptr %state1, align 8
  store i64 0, ptr %"%count", align 8
  store i64 0, ptr %"%d", align 8
  br label %while_cond_896

merge_900:                                        ; preds = %then_899, %while_body_897
  %"%d.load3" = load i64, ptr %"%d", align 8
  %addtmp4 = add i64 %"%d.load3", 1
  store i64 %addtmp4, ptr %"%d", align 8
  br label %while_cond_896

then_899:                                         ; preds = %while_body_897
  %"%count.load" = load i64, ptr %"%count", align 8
  %addtmp = add i64 %"%count.load", 1
  store i64 %addtmp, ptr %"%count", align 8
  br label %merge_900

while_body_897:                                   ; preds = %while_cond_896
  %"%d.load2" = load i64, ptr %"%d", align 8
  %calltmp = call i64 @dm_get_state(i64 %"%d.load2")
  %"%state.load" = load i64, ptr %state1, align 8
  %cmpeqtmp = icmp eq i64 %calltmp, %"%state.load"
  br i1 %cmpeqtmp, label %then_899, label %merge_900

while_cond_896:                                   ; preds = %merge_900, %entry
  %"%d.load" = load i64, ptr %"%d", align 8
  %"%devmgr_dev_count.load" = load i64, ptr @devmgr_dev_count, align 8
  %cmplttmp = icmp slt i64 %"%d.load", %"%devmgr_dev_count.load"
  br i1 %cmplttmp, label %while_body_897, label %while_exit_898

while_exit_898:                                   ; preds = %while_cond_896
  %loadtmp = load i64, ptr %"%count", align 8
  ret i64 %loadtmp
}

define i64 @"dm_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %"%t" = alloca i64, align 8
  %"%d" = alloca i64, align 8
  %"%err_count" = alloca i64, align 8
  %"%detected" = alloca i64, align 8
  %"%init" = alloca i64, align 8
  %"%active" = alloca i64, align 8
  %calltmp = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  %calltmp1 = call i64 @"dm_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %"%2247.load" = load i64, ptr @devmgr_dev_count, align 8
  %calltmp3 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%2247.load")
  %calltmp4 = call i64 @"dm_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp5 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %"%2253.load" = load i64, ptr @devmgr_drv_count, align 8
  %calltmp6 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%2253.load")
  %calltmp7 = call i64 @"dm_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp8 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %"%2259.load" = load i64, ptr @devmgr_bindings, align 8
  %calltmp9 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%2259.load")
  %calltmp10 = call i64 @"dm_serial_\D8\B3\D8\B7\D8\B1"()
  %"%DEV_STATE_ACTIVE.load" = load i64, ptr @DEV_STATE_ACTIVE, align 8
  %calltmp11 = call i64 @dm_count_by_state(i64 %"%DEV_STATE_ACTIVE.load")
  store i64 %calltmp11, ptr %"%active", align 8
  %"%DEV_STATE_INIT.load" = load i64, ptr @DEV_STATE_INIT, align 8
  %calltmp12 = call i64 @dm_count_by_state(i64 %"%DEV_STATE_INIT.load")
  store i64 %calltmp12, ptr %"%init", align 8
  %"%DEV_STATE_DETECTED.load" = load i64, ptr @DEV_STATE_DETECTED, align 8
  %calltmp13 = call i64 @dm_count_by_state(i64 %"%DEV_STATE_DETECTED.load")
  store i64 %calltmp13, ptr %"%detected", align 8
  %"%DEV_STATE_ERROR.load" = load i64, ptr @DEV_STATE_ERROR, align 8
  %calltmp14 = call i64 @dm_count_by_state(i64 %"%DEV_STATE_ERROR.load")
  store i64 %calltmp14, ptr %"%err_count", align 8
  %calltmp15 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %"%2277.load" = load i64, ptr %"%active", align 8
  %calltmp16 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%2277.load")
  %calltmp17 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  %"%2282.load" = load i64, ptr %"%init", align 8
  %calltmp18 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%2282.load")
  %calltmp19 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%2287.load" = load i64, ptr %"%detected", align 8
  %calltmp20 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%2287.load")
  %calltmp21 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  %"%2292.load" = load i64, ptr %"%err_count", align 8
  %calltmp22 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%2292.load")
  %calltmp23 = call i64 @"dm_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%d", align 8
  br label %while_cond_901

merge_905:                                        ; preds = %then_904, %while_body_902
  %"%d.load37" = load i64, ptr %"%d", align 8
  %addtmp = add i64 %"%d.load37", 1
  store i64 %addtmp, ptr %"%d", align 8
  br label %while_cond_901

then_904:                                         ; preds = %while_body_902
  %calltmp28 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %"%2307.load" = load i64, ptr %"%d", align 8
  %calltmp29 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%2307.load")
  %calltmp30 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  %"%2312.load" = load i64, ptr %"%t", align 8
  %calltmp31 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%2312.load")
  %calltmp32 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  %"%d.load33" = load i64, ptr %"%d", align 8
  %calltmp34 = call i64 @dm_get_state(i64 %"%d.load33")
  %calltmp35 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp34)
  %calltmp36 = call i64 @"dm_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_905

while_body_902:                                   ; preds = %while_cond_901
  %"%d.load24" = load i64, ptr %"%d", align 8
  %calltmp25 = call i64 @dm_get_type(i64 %"%d.load24")
  store i64 %calltmp25, ptr %"%t", align 8
  %"%t.load" = load i64, ptr %"%t", align 8
  %cmpgttmp = icmp sgt i64 %"%t.load", 0
  br i1 %cmpgttmp, label %then_904, label %merge_905

while_cond_901:                                   ; preds = %merge_905, %entry
  %"%d.load" = load i64, ptr %"%d", align 8
  %"%devmgr_dev_count.load" = load i64, ptr @devmgr_dev_count, align 8
  %cmplttmp = icmp slt i64 %"%d.load", %"%devmgr_dev_count.load"
  br i1 %cmplttmp, label %while_body_902, label %while_exit_903

while_exit_903:                                   ; preds = %while_cond_901
  %calltmp26 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %calltmp27 = call i64 @"dm_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @"dm_\D9\87\D9\8A\D8\A6"() {
entry:
  %"%bound" = alloca i64, align 8
  %calltmp = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  %calltmp1 = call i64 @"dm_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @dm_register_builtin_drivers()
  %calltmp3 = call i64 @dm_register_platform_devices()
  %calltmp4 = call i64 @dm_auto_bind()
  store i64 %calltmp4, ptr %"%bound", align 8
  %calltmp5 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  %"%2338.load" = load i64, ptr @devmgr_dev_count, align 8
  %calltmp6 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%2338.load")
  %calltmp7 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  %"%2343.load" = load i64, ptr %"%bound", align 8
  %calltmp8 = call i64 @"dm_serial_\D8\B1\D9\82\D9\85"(i64 %"%2343.load")
  %calltmp9 = call i64 @"dm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %calltmp10 = call i64 @"dm_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @"dm_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  ret i64 0
}

define void @__sad_main() {
entry:
  %"%devmgr_bindings" = alloca i64, align 8
  %"%devmgr_drv_count" = alloca i64, align 8
  %"%devmgr_dev_count" = alloca i64, align 8
  %"%irq_owner_15" = alloca i64, align 8
  %"%irq_owner_14" = alloca i64, align 8
  %"%irq_owner_13" = alloca i64, align 8
  %"%irq_owner_12" = alloca i64, align 8
  %"%irq_owner_11" = alloca i64, align 8
  %"%irq_owner_10" = alloca i64, align 8
  %"%irq_owner_9" = alloca i64, align 8
  %"%irq_owner_8" = alloca i64, align 8
  %"%irq_owner_7" = alloca i64, align 8
  %"%irq_owner_6" = alloca i64, align 8
  %"%irq_owner_5" = alloca i64, align 8
  %"%irq_owner_4" = alloca i64, align 8
  %"%irq_owner_3" = alloca i64, align 8
  %"%irq_owner_2" = alloca i64, align 8
  %"%irq_owner_1" = alloca i64, align 8
  %"%irq_owner_0" = alloca i64, align 8
  %"%drv_devcount_15" = alloca i64, align 8
  %"%drv_devcount_14" = alloca i64, align 8
  %"%drv_devcount_13" = alloca i64, align 8
  %"%drv_devcount_12" = alloca i64, align 8
  %"%drv_devcount_11" = alloca i64, align 8
  %"%drv_devcount_10" = alloca i64, align 8
  %"%drv_devcount_9" = alloca i64, align 8
  %"%drv_devcount_8" = alloca i64, align 8
  %"%drv_devcount_7" = alloca i64, align 8
  %"%drv_devcount_6" = alloca i64, align 8
  %"%drv_devcount_5" = alloca i64, align 8
  %"%drv_devcount_4" = alloca i64, align 8
  %"%drv_devcount_3" = alloca i64, align 8
  %"%drv_devcount_2" = alloca i64, align 8
  %"%drv_devcount_1" = alloca i64, align 8
  %"%drv_devcount_0" = alloca i64, align 8
  %"%drv_psub_15" = alloca i64, align 8
  %"%drv_psub_14" = alloca i64, align 8
  %"%drv_psub_13" = alloca i64, align 8
  %"%drv_psub_12" = alloca i64, align 8
  %"%drv_psub_11" = alloca i64, align 8
  %"%drv_psub_10" = alloca i64, align 8
  %"%drv_psub_9" = alloca i64, align 8
  %"%drv_psub_8" = alloca i64, align 8
  %"%drv_psub_7" = alloca i64, align 8
  %"%drv_psub_6" = alloca i64, align 8
  %"%drv_psub_5" = alloca i64, align 8
  %"%drv_psub_4" = alloca i64, align 8
  %"%drv_psub_3" = alloca i64, align 8
  %"%drv_psub_2" = alloca i64, align 8
  %"%drv_psub_1" = alloca i64, align 8
  %"%drv_psub_0" = alloca i64, align 8
  %"%drv_pclass_15" = alloca i64, align 8
  %"%drv_pclass_14" = alloca i64, align 8
  %"%drv_pclass_13" = alloca i64, align 8
  %"%drv_pclass_12" = alloca i64, align 8
  %"%drv_pclass_11" = alloca i64, align 8
  %"%drv_pclass_10" = alloca i64, align 8
  %"%drv_pclass_9" = alloca i64, align 8
  %"%drv_pclass_8" = alloca i64, align 8
  %"%drv_pclass_7" = alloca i64, align 8
  %"%drv_pclass_6" = alloca i64, align 8
  %"%drv_pclass_5" = alloca i64, align 8
  %"%drv_pclass_4" = alloca i64, align 8
  %"%drv_pclass_3" = alloca i64, align 8
  %"%drv_pclass_2" = alloca i64, align 8
  %"%drv_pclass_1" = alloca i64, align 8
  %"%drv_pclass_0" = alloca i64, align 8
  %"%drv_reg_15" = alloca i64, align 8
  %"%drv_reg_14" = alloca i64, align 8
  %"%drv_reg_13" = alloca i64, align 8
  %"%drv_reg_12" = alloca i64, align 8
  %"%drv_reg_11" = alloca i64, align 8
  %"%drv_reg_10" = alloca i64, align 8
  %"%drv_reg_9" = alloca i64, align 8
  %"%drv_reg_8" = alloca i64, align 8
  %"%drv_reg_7" = alloca i64, align 8
  %"%drv_reg_6" = alloca i64, align 8
  %"%drv_reg_5" = alloca i64, align 8
  %"%drv_reg_4" = alloca i64, align 8
  %"%drv_reg_3" = alloca i64, align 8
  %"%drv_reg_2" = alloca i64, align 8
  %"%drv_reg_1" = alloca i64, align 8
  %"%drv_reg_0" = alloca i64, align 8
  %"%drv_type_15" = alloca i64, align 8
  %"%drv_type_14" = alloca i64, align 8
  %"%drv_type_13" = alloca i64, align 8
  %"%drv_type_12" = alloca i64, align 8
  %"%drv_type_11" = alloca i64, align 8
  %"%drv_type_10" = alloca i64, align 8
  %"%drv_type_9" = alloca i64, align 8
  %"%drv_type_8" = alloca i64, align 8
  %"%drv_type_7" = alloca i64, align 8
  %"%drv_type_6" = alloca i64, align 8
  %"%drv_type_5" = alloca i64, align 8
  %"%drv_type_4" = alloca i64, align 8
  %"%drv_type_3" = alloca i64, align 8
  %"%drv_type_2" = alloca i64, align 8
  %"%drv_type_1" = alloca i64, align 8
  %"%drv_type_0" = alloca i64, align 8
  %"%dev_mmio_31" = alloca i64, align 8
  %"%dev_mmio_30" = alloca i64, align 8
  %"%dev_mmio_29" = alloca i64, align 8
  %"%dev_mmio_28" = alloca i64, align 8
  %"%dev_mmio_27" = alloca i64, align 8
  %"%dev_mmio_26" = alloca i64, align 8
  %"%dev_mmio_25" = alloca i64, align 8
  %"%dev_mmio_24" = alloca i64, align 8
  %"%dev_mmio_23" = alloca i64, align 8
  %"%dev_mmio_22" = alloca i64, align 8
  %"%dev_mmio_21" = alloca i64, align 8
  %"%dev_mmio_20" = alloca i64, align 8
  %"%dev_mmio_19" = alloca i64, align 8
  %"%dev_mmio_18" = alloca i64, align 8
  %"%dev_mmio_17" = alloca i64, align 8
  %"%dev_mmio_16" = alloca i64, align 8
  %"%dev_mmio_15" = alloca i64, align 8
  %"%dev_mmio_14" = alloca i64, align 8
  %"%dev_mmio_13" = alloca i64, align 8
  %"%dev_mmio_12" = alloca i64, align 8
  %"%dev_mmio_11" = alloca i64, align 8
  %"%dev_mmio_10" = alloca i64, align 8
  %"%dev_mmio_9" = alloca i64, align 8
  %"%dev_mmio_8" = alloca i64, align 8
  %"%dev_mmio_7" = alloca i64, align 8
  %"%dev_mmio_6" = alloca i64, align 8
  %"%dev_mmio_5" = alloca i64, align 8
  %"%dev_mmio_4" = alloca i64, align 8
  %"%dev_mmio_3" = alloca i64, align 8
  %"%dev_mmio_2" = alloca i64, align 8
  %"%dev_mmio_1" = alloca i64, align 8
  %"%dev_mmio_0" = alloca i64, align 8
  %"%dev_io_31" = alloca i64, align 8
  %"%dev_io_30" = alloca i64, align 8
  %"%dev_io_29" = alloca i64, align 8
  %"%dev_io_28" = alloca i64, align 8
  %"%dev_io_27" = alloca i64, align 8
  %"%dev_io_26" = alloca i64, align 8
  %"%dev_io_25" = alloca i64, align 8
  %"%dev_io_24" = alloca i64, align 8
  %"%dev_io_23" = alloca i64, align 8
  %"%dev_io_22" = alloca i64, align 8
  %"%dev_io_21" = alloca i64, align 8
  %"%dev_io_20" = alloca i64, align 8
  %"%dev_io_19" = alloca i64, align 8
  %"%dev_io_18" = alloca i64, align 8
  %"%dev_io_17" = alloca i64, align 8
  %"%dev_io_16" = alloca i64, align 8
  %"%dev_io_15" = alloca i64, align 8
  %"%dev_io_14" = alloca i64, align 8
  %"%dev_io_13" = alloca i64, align 8
  %"%dev_io_12" = alloca i64, align 8
  %"%dev_io_11" = alloca i64, align 8
  %"%dev_io_10" = alloca i64, align 8
  %"%dev_io_9" = alloca i64, align 8
  %"%dev_io_8" = alloca i64, align 8
  %"%dev_io_7" = alloca i64, align 8
  %"%dev_io_6" = alloca i64, align 8
  %"%dev_io_5" = alloca i64, align 8
  %"%dev_io_4" = alloca i64, align 8
  %"%dev_io_3" = alloca i64, align 8
  %"%dev_io_2" = alloca i64, align 8
  %"%dev_io_1" = alloca i64, align 8
  %"%dev_io_0" = alloca i64, align 8
  %"%dev_irq_31" = alloca i64, align 8
  %"%dev_irq_30" = alloca i64, align 8
  %"%dev_irq_29" = alloca i64, align 8
  %"%dev_irq_28" = alloca i64, align 8
  %"%dev_irq_27" = alloca i64, align 8
  %"%dev_irq_26" = alloca i64, align 8
  %"%dev_irq_25" = alloca i64, align 8
  %"%dev_irq_24" = alloca i64, align 8
  %"%dev_irq_23" = alloca i64, align 8
  %"%dev_irq_22" = alloca i64, align 8
  %"%dev_irq_21" = alloca i64, align 8
  %"%dev_irq_20" = alloca i64, align 8
  %"%dev_irq_19" = alloca i64, align 8
  %"%dev_irq_18" = alloca i64, align 8
  %"%dev_irq_17" = alloca i64, align 8
  %"%dev_irq_16" = alloca i64, align 8
  %"%dev_irq_15" = alloca i64, align 8
  %"%dev_irq_14" = alloca i64, align 8
  %"%dev_irq_13" = alloca i64, align 8
  %"%dev_irq_12" = alloca i64, align 8
  %"%dev_irq_11" = alloca i64, align 8
  %"%dev_irq_10" = alloca i64, align 8
  %"%dev_irq_9" = alloca i64, align 8
  %"%dev_irq_8" = alloca i64, align 8
  %"%dev_irq_7" = alloca i64, align 8
  %"%dev_irq_6" = alloca i64, align 8
  %"%dev_irq_5" = alloca i64, align 8
  %"%dev_irq_4" = alloca i64, align 8
  %"%dev_irq_3" = alloca i64, align 8
  %"%dev_irq_2" = alloca i64, align 8
  %"%dev_irq_1" = alloca i64, align 8
  %"%dev_irq_0" = alloca i64, align 8
  %"%dev_sub_31" = alloca i64, align 8
  %"%dev_sub_30" = alloca i64, align 8
  %"%dev_sub_29" = alloca i64, align 8
  %"%dev_sub_28" = alloca i64, align 8
  %"%dev_sub_27" = alloca i64, align 8
  %"%dev_sub_26" = alloca i64, align 8
  %"%dev_sub_25" = alloca i64, align 8
  %"%dev_sub_24" = alloca i64, align 8
  %"%dev_sub_23" = alloca i64, align 8
  %"%dev_sub_22" = alloca i64, align 8
  %"%dev_sub_21" = alloca i64, align 8
  %"%dev_sub_20" = alloca i64, align 8
  %"%dev_sub_19" = alloca i64, align 8
  %"%dev_sub_18" = alloca i64, align 8
  %"%dev_sub_17" = alloca i64, align 8
  %"%dev_sub_16" = alloca i64, align 8
  %"%dev_sub_15" = alloca i64, align 8
  %"%dev_sub_14" = alloca i64, align 8
  %"%dev_sub_13" = alloca i64, align 8
  %"%dev_sub_12" = alloca i64, align 8
  %"%dev_sub_11" = alloca i64, align 8
  %"%dev_sub_10" = alloca i64, align 8
  %"%dev_sub_9" = alloca i64, align 8
  %"%dev_sub_8" = alloca i64, align 8
  %"%dev_sub_7" = alloca i64, align 8
  %"%dev_sub_6" = alloca i64, align 8
  %"%dev_sub_5" = alloca i64, align 8
  %"%dev_sub_4" = alloca i64, align 8
  %"%dev_sub_3" = alloca i64, align 8
  %"%dev_sub_2" = alloca i64, align 8
  %"%dev_sub_1" = alloca i64, align 8
  %"%dev_sub_0" = alloca i64, align 8
  %"%dev_class_31" = alloca i64, align 8
  %"%dev_class_30" = alloca i64, align 8
  %"%dev_class_29" = alloca i64, align 8
  %"%dev_class_28" = alloca i64, align 8
  %"%dev_class_27" = alloca i64, align 8
  %"%dev_class_26" = alloca i64, align 8
  %"%dev_class_25" = alloca i64, align 8
  %"%dev_class_24" = alloca i64, align 8
  %"%dev_class_23" = alloca i64, align 8
  %"%dev_class_22" = alloca i64, align 8
  %"%dev_class_21" = alloca i64, align 8
  %"%dev_class_20" = alloca i64, align 8
  %"%dev_class_19" = alloca i64, align 8
  %"%dev_class_18" = alloca i64, align 8
  %"%dev_class_17" = alloca i64, align 8
  %"%dev_class_16" = alloca i64, align 8
  %"%dev_class_15" = alloca i64, align 8
  %"%dev_class_14" = alloca i64, align 8
  %"%dev_class_13" = alloca i64, align 8
  %"%dev_class_12" = alloca i64, align 8
  %"%dev_class_11" = alloca i64, align 8
  %"%dev_class_10" = alloca i64, align 8
  %"%dev_class_9" = alloca i64, align 8
  %"%dev_class_8" = alloca i64, align 8
  %"%dev_class_7" = alloca i64, align 8
  %"%dev_class_6" = alloca i64, align 8
  %"%dev_class_5" = alloca i64, align 8
  %"%dev_class_4" = alloca i64, align 8
  %"%dev_class_3" = alloca i64, align 8
  %"%dev_class_2" = alloca i64, align 8
  %"%dev_class_1" = alloca i64, align 8
  %"%dev_class_0" = alloca i64, align 8
  %"%dev_drv_31" = alloca i64, align 8
  %"%dev_drv_30" = alloca i64, align 8
  %"%dev_drv_29" = alloca i64, align 8
  %"%dev_drv_28" = alloca i64, align 8
  %"%dev_drv_27" = alloca i64, align 8
  %"%dev_drv_26" = alloca i64, align 8
  %"%dev_drv_25" = alloca i64, align 8
  %"%dev_drv_24" = alloca i64, align 8
  %"%dev_drv_23" = alloca i64, align 8
  %"%dev_drv_22" = alloca i64, align 8
  %"%dev_drv_21" = alloca i64, align 8
  %"%dev_drv_20" = alloca i64, align 8
  %"%dev_drv_19" = alloca i64, align 8
  %"%dev_drv_18" = alloca i64, align 8
  %"%dev_drv_17" = alloca i64, align 8
  %"%dev_drv_16" = alloca i64, align 8
  %"%dev_drv_15" = alloca i64, align 8
  %"%dev_drv_14" = alloca i64, align 8
  %"%dev_drv_13" = alloca i64, align 8
  %"%dev_drv_12" = alloca i64, align 8
  %"%dev_drv_11" = alloca i64, align 8
  %"%dev_drv_10" = alloca i64, align 8
  %"%dev_drv_9" = alloca i64, align 8
  %"%dev_drv_8" = alloca i64, align 8
  %"%dev_drv_7" = alloca i64, align 8
  %"%dev_drv_6" = alloca i64, align 8
  %"%dev_drv_5" = alloca i64, align 8
  %"%dev_drv_4" = alloca i64, align 8
  %"%dev_drv_3" = alloca i64, align 8
  %"%dev_drv_2" = alloca i64, align 8
  %"%dev_drv_1" = alloca i64, align 8
  %"%dev_drv_0" = alloca i64, align 8
  %"%dev_parent_31" = alloca i64, align 8
  %"%dev_parent_30" = alloca i64, align 8
  %"%dev_parent_29" = alloca i64, align 8
  %"%dev_parent_28" = alloca i64, align 8
  %"%dev_parent_27" = alloca i64, align 8
  %"%dev_parent_26" = alloca i64, align 8
  %"%dev_parent_25" = alloca i64, align 8
  %"%dev_parent_24" = alloca i64, align 8
  %"%dev_parent_23" = alloca i64, align 8
  %"%dev_parent_22" = alloca i64, align 8
  %"%dev_parent_21" = alloca i64, align 8
  %"%dev_parent_20" = alloca i64, align 8
  %"%dev_parent_19" = alloca i64, align 8
  %"%dev_parent_18" = alloca i64, align 8
  %"%dev_parent_17" = alloca i64, align 8
  %"%dev_parent_16" = alloca i64, align 8
  %"%dev_parent_15" = alloca i64, align 8
  %"%dev_parent_14" = alloca i64, align 8
  %"%dev_parent_13" = alloca i64, align 8
  %"%dev_parent_12" = alloca i64, align 8
  %"%dev_parent_11" = alloca i64, align 8
  %"%dev_parent_10" = alloca i64, align 8
  %"%dev_parent_9" = alloca i64, align 8
  %"%dev_parent_8" = alloca i64, align 8
  %"%dev_parent_7" = alloca i64, align 8
  %"%dev_parent_6" = alloca i64, align 8
  %"%dev_parent_5" = alloca i64, align 8
  %"%dev_parent_4" = alloca i64, align 8
  %"%dev_parent_3" = alloca i64, align 8
  %"%dev_parent_2" = alloca i64, align 8
  %"%dev_parent_1" = alloca i64, align 8
  %"%dev_parent_0" = alloca i64, align 8
  %"%dev_did_31" = alloca i64, align 8
  %"%dev_did_30" = alloca i64, align 8
  %"%dev_did_29" = alloca i64, align 8
  %"%dev_did_28" = alloca i64, align 8
  %"%dev_did_27" = alloca i64, align 8
  %"%dev_did_26" = alloca i64, align 8
  %"%dev_did_25" = alloca i64, align 8
  %"%dev_did_24" = alloca i64, align 8
  %"%dev_did_23" = alloca i64, align 8
  %"%dev_did_22" = alloca i64, align 8
  %"%dev_did_21" = alloca i64, align 8
  %"%dev_did_20" = alloca i64, align 8
  %"%dev_did_19" = alloca i64, align 8
  %"%dev_did_18" = alloca i64, align 8
  %"%dev_did_17" = alloca i64, align 8
  %"%dev_did_16" = alloca i64, align 8
  %"%dev_did_15" = alloca i64, align 8
  %"%dev_did_14" = alloca i64, align 8
  %"%dev_did_13" = alloca i64, align 8
  %"%dev_did_12" = alloca i64, align 8
  %"%dev_did_11" = alloca i64, align 8
  %"%dev_did_10" = alloca i64, align 8
  %"%dev_did_9" = alloca i64, align 8
  %"%dev_did_8" = alloca i64, align 8
  %"%dev_did_7" = alloca i64, align 8
  %"%dev_did_6" = alloca i64, align 8
  %"%dev_did_5" = alloca i64, align 8
  %"%dev_did_4" = alloca i64, align 8
  %"%dev_did_3" = alloca i64, align 8
  %"%dev_did_2" = alloca i64, align 8
  %"%dev_did_1" = alloca i64, align 8
  %"%dev_did_0" = alloca i64, align 8
  %"%dev_vid_31" = alloca i64, align 8
  %"%dev_vid_30" = alloca i64, align 8
  %"%dev_vid_29" = alloca i64, align 8
  %"%dev_vid_28" = alloca i64, align 8
  %"%dev_vid_27" = alloca i64, align 8
  %"%dev_vid_26" = alloca i64, align 8
  %"%dev_vid_25" = alloca i64, align 8
  %"%dev_vid_24" = alloca i64, align 8
  %"%dev_vid_23" = alloca i64, align 8
  %"%dev_vid_22" = alloca i64, align 8
  %"%dev_vid_21" = alloca i64, align 8
  %"%dev_vid_20" = alloca i64, align 8
  %"%dev_vid_19" = alloca i64, align 8
  %"%dev_vid_18" = alloca i64, align 8
  %"%dev_vid_17" = alloca i64, align 8
  %"%dev_vid_16" = alloca i64, align 8
  %"%dev_vid_15" = alloca i64, align 8
  %"%dev_vid_14" = alloca i64, align 8
  %"%dev_vid_13" = alloca i64, align 8
  %"%dev_vid_12" = alloca i64, align 8
  %"%dev_vid_11" = alloca i64, align 8
  %"%dev_vid_10" = alloca i64, align 8
  %"%dev_vid_9" = alloca i64, align 8
  %"%dev_vid_8" = alloca i64, align 8
  %"%dev_vid_7" = alloca i64, align 8
  %"%dev_vid_6" = alloca i64, align 8
  %"%dev_vid_5" = alloca i64, align 8
  %"%dev_vid_4" = alloca i64, align 8
  %"%dev_vid_3" = alloca i64, align 8
  %"%dev_vid_2" = alloca i64, align 8
  %"%dev_vid_1" = alloca i64, align 8
  %"%dev_vid_0" = alloca i64, align 8
  %"%dev_state_63" = alloca i64, align 8
  %"%dev_state_62" = alloca i64, align 8
  %"%dev_state_61" = alloca i64, align 8
  %"%dev_state_60" = alloca i64, align 8
  %"%dev_state_59" = alloca i64, align 8
  %"%dev_state_58" = alloca i64, align 8
  %"%dev_state_57" = alloca i64, align 8
  %"%dev_state_56" = alloca i64, align 8
  %"%dev_state_55" = alloca i64, align 8
  %"%dev_state_54" = alloca i64, align 8
  %"%dev_state_53" = alloca i64, align 8
  %"%dev_state_52" = alloca i64, align 8
  %"%dev_state_51" = alloca i64, align 8
  %"%dev_state_50" = alloca i64, align 8
  %"%dev_state_49" = alloca i64, align 8
  %"%dev_state_48" = alloca i64, align 8
  %"%dev_state_47" = alloca i64, align 8
  %"%dev_state_46" = alloca i64, align 8
  %"%dev_state_45" = alloca i64, align 8
  %"%dev_state_44" = alloca i64, align 8
  %"%dev_state_43" = alloca i64, align 8
  %"%dev_state_42" = alloca i64, align 8
  %"%dev_state_41" = alloca i64, align 8
  %"%dev_state_40" = alloca i64, align 8
  %"%dev_state_39" = alloca i64, align 8
  %"%dev_state_38" = alloca i64, align 8
  %"%dev_state_37" = alloca i64, align 8
  %"%dev_state_36" = alloca i64, align 8
  %"%dev_state_35" = alloca i64, align 8
  %"%dev_state_34" = alloca i64, align 8
  %"%dev_state_33" = alloca i64, align 8
  %"%dev_state_32" = alloca i64, align 8
  %"%dev_state_31" = alloca i64, align 8
  %"%dev_state_30" = alloca i64, align 8
  %"%dev_state_29" = alloca i64, align 8
  %"%dev_state_28" = alloca i64, align 8
  %"%dev_state_27" = alloca i64, align 8
  %"%dev_state_26" = alloca i64, align 8
  %"%dev_state_25" = alloca i64, align 8
  %"%dev_state_24" = alloca i64, align 8
  %"%dev_state_23" = alloca i64, align 8
  %"%dev_state_22" = alloca i64, align 8
  %"%dev_state_21" = alloca i64, align 8
  %"%dev_state_20" = alloca i64, align 8
  %"%dev_state_19" = alloca i64, align 8
  %"%dev_state_18" = alloca i64, align 8
  %"%dev_state_17" = alloca i64, align 8
  %"%dev_state_16" = alloca i64, align 8
  %"%dev_state_15" = alloca i64, align 8
  %"%dev_state_14" = alloca i64, align 8
  %"%dev_state_13" = alloca i64, align 8
  %"%dev_state_12" = alloca i64, align 8
  %"%dev_state_11" = alloca i64, align 8
  %"%dev_state_10" = alloca i64, align 8
  %"%dev_state_9" = alloca i64, align 8
  %"%dev_state_8" = alloca i64, align 8
  %"%dev_state_7" = alloca i64, align 8
  %"%dev_state_6" = alloca i64, align 8
  %"%dev_state_5" = alloca i64, align 8
  %"%dev_state_4" = alloca i64, align 8
  %"%dev_state_3" = alloca i64, align 8
  %"%dev_state_2" = alloca i64, align 8
  %"%dev_state_1" = alloca i64, align 8
  %"%dev_state_0" = alloca i64, align 8
  %"%dev_type_63" = alloca i64, align 8
  %"%dev_type_62" = alloca i64, align 8
  %"%dev_type_61" = alloca i64, align 8
  %"%dev_type_60" = alloca i64, align 8
  %"%dev_type_59" = alloca i64, align 8
  %"%dev_type_58" = alloca i64, align 8
  %"%dev_type_57" = alloca i64, align 8
  %"%dev_type_56" = alloca i64, align 8
  %"%dev_type_55" = alloca i64, align 8
  %"%dev_type_54" = alloca i64, align 8
  %"%dev_type_53" = alloca i64, align 8
  %"%dev_type_52" = alloca i64, align 8
  %"%dev_type_51" = alloca i64, align 8
  %"%dev_type_50" = alloca i64, align 8
  %"%dev_type_49" = alloca i64, align 8
  %"%dev_type_48" = alloca i64, align 8
  %"%dev_type_47" = alloca i64, align 8
  %"%dev_type_46" = alloca i64, align 8
  %"%dev_type_45" = alloca i64, align 8
  %"%dev_type_44" = alloca i64, align 8
  %"%dev_type_43" = alloca i64, align 8
  %"%dev_type_42" = alloca i64, align 8
  %"%dev_type_41" = alloca i64, align 8
  %"%dev_type_40" = alloca i64, align 8
  %"%dev_type_39" = alloca i64, align 8
  %"%dev_type_38" = alloca i64, align 8
  %"%dev_type_37" = alloca i64, align 8
  %"%dev_type_36" = alloca i64, align 8
  %"%dev_type_35" = alloca i64, align 8
  %"%dev_type_34" = alloca i64, align 8
  %"%dev_type_33" = alloca i64, align 8
  %"%dev_type_32" = alloca i64, align 8
  %"%dev_type_31" = alloca i64, align 8
  %"%dev_type_30" = alloca i64, align 8
  %"%dev_type_29" = alloca i64, align 8
  %"%dev_type_28" = alloca i64, align 8
  %"%dev_type_27" = alloca i64, align 8
  %"%dev_type_26" = alloca i64, align 8
  %"%dev_type_25" = alloca i64, align 8
  %"%dev_type_24" = alloca i64, align 8
  %"%dev_type_23" = alloca i64, align 8
  %"%dev_type_22" = alloca i64, align 8
  %"%dev_type_21" = alloca i64, align 8
  %"%dev_type_20" = alloca i64, align 8
  %"%dev_type_19" = alloca i64, align 8
  %"%dev_type_18" = alloca i64, align 8
  %"%dev_type_17" = alloca i64, align 8
  %"%dev_type_16" = alloca i64, align 8
  %"%dev_type_15" = alloca i64, align 8
  %"%dev_type_14" = alloca i64, align 8
  %"%dev_type_13" = alloca i64, align 8
  %"%dev_type_12" = alloca i64, align 8
  %"%dev_type_11" = alloca i64, align 8
  %"%dev_type_10" = alloca i64, align 8
  %"%dev_type_9" = alloca i64, align 8
  %"%dev_type_8" = alloca i64, align 8
  %"%dev_type_7" = alloca i64, align 8
  %"%dev_type_6" = alloca i64, align 8
  %"%dev_type_5" = alloca i64, align 8
  %"%dev_type_4" = alloca i64, align 8
  %"%dev_type_3" = alloca i64, align 8
  %"%dev_type_2" = alloca i64, align 8
  %"%dev_type_1" = alloca i64, align 8
  %"%dev_type_0" = alloca i64, align 8
  %"%MAX_IO_REGIONS" = alloca i64, align 8
  %"%MAX_IRQ" = alloca i64, align 8
  %"%MAX_RESOURCES" = alloca i64, align 8
  %"%MAX_DRIVERS" = alloca i64, align 8
  %"%MAX_DEVICES" = alloca i64, align 8
  %"%RES_TYPE_DMA" = alloca i64, align 8
  %"%RES_TYPE_IRQ" = alloca i64, align 8
  %"%RES_TYPE_MEM" = alloca i64, align 8
  %"%RES_TYPE_IO" = alloca i64, align 8
  %"%RES_TYPE_NONE" = alloca i64, align 8
  %"%DEV_STATE_DISABLED" = alloca i64, align 8
  %"%DEV_STATE_REMOVED" = alloca i64, align 8
  %"%DEV_STATE_ERROR" = alloca i64, align 8
  %"%DEV_STATE_SUSPENDED" = alloca i64, align 8
  %"%DEV_STATE_ACTIVE" = alloca i64, align 8
  %"%DEV_STATE_INIT" = alloca i64, align 8
  %"%DEV_STATE_DETECTED" = alloca i64, align 8
  %"%DEV_STATE_UNKNOWN" = alloca i64, align 8
  %"%DEV_TYPE_PLATFORM" = alloca i64, align 8
  %"%DEV_TYPE_HUB" = alloca i64, align 8
  %"%DEV_TYPE_STORAGE" = alloca i64, align 8
  %"%DEV_TYPE_HID" = alloca i64, align 8
  %"%DEV_TYPE_BUS" = alloca i64, align 8
  %"%DEV_TYPE_ACPI" = alloca i64, align 8
  %"%DEV_TYPE_TIMER" = alloca i64, align 8
  %"%DEV_TYPE_SERIAL" = alloca i64, align 8
  %"%DEV_TYPE_AUDIO" = alloca i64, align 8
  %"%DEV_TYPE_GFX" = alloca i64, align 8
  %"%DEV_TYPE_NET" = alloca i64, align 8
  %"%DEV_TYPE_ATA" = alloca i64, align 8
  %"%DEV_TYPE_PS2" = alloca i64, align 8
  %"%DEV_TYPE_USB" = alloca i64, align 8
  %"%DEV_TYPE_PCI" = alloca i64, align 8
  %"%DEV_TYPE_NONE" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 0, ptr %"%DEV_TYPE_NONE", align 8
  store i64 1, ptr %"%DEV_TYPE_PCI", align 8
  store i64 2, ptr %"%DEV_TYPE_USB", align 8
  store i64 3, ptr %"%DEV_TYPE_PS2", align 8
  store i64 4, ptr %"%DEV_TYPE_ATA", align 8
  store i64 5, ptr %"%DEV_TYPE_NET", align 8
  store i64 6, ptr %"%DEV_TYPE_GFX", align 8
  store i64 7, ptr %"%DEV_TYPE_AUDIO", align 8
  store i64 8, ptr %"%DEV_TYPE_SERIAL", align 8
  store i64 9, ptr %"%DEV_TYPE_TIMER", align 8
  store i64 10, ptr %"%DEV_TYPE_ACPI", align 8
  store i64 11, ptr %"%DEV_TYPE_BUS", align 8
  store i64 12, ptr %"%DEV_TYPE_HID", align 8
  store i64 13, ptr %"%DEV_TYPE_STORAGE", align 8
  store i64 14, ptr %"%DEV_TYPE_HUB", align 8
  store i64 15, ptr %"%DEV_TYPE_PLATFORM", align 8
  store i64 0, ptr %"%DEV_STATE_UNKNOWN", align 8
  store i64 1, ptr %"%DEV_STATE_DETECTED", align 8
  store i64 2, ptr %"%DEV_STATE_INIT", align 8
  store i64 3, ptr %"%DEV_STATE_ACTIVE", align 8
  store i64 4, ptr %"%DEV_STATE_SUSPENDED", align 8
  store i64 5, ptr %"%DEV_STATE_ERROR", align 8
  store i64 6, ptr %"%DEV_STATE_REMOVED", align 8
  store i64 7, ptr %"%DEV_STATE_DISABLED", align 8
  store i64 0, ptr %"%RES_TYPE_NONE", align 8
  store i64 1, ptr %"%RES_TYPE_IO", align 8
  store i64 2, ptr %"%RES_TYPE_MEM", align 8
  store i64 3, ptr %"%RES_TYPE_IRQ", align 8
  store i64 4, ptr %"%RES_TYPE_DMA", align 8
  store i64 64, ptr %"%MAX_DEVICES", align 8
  store i64 32, ptr %"%MAX_DRIVERS", align 8
  store i64 128, ptr %"%MAX_RESOURCES", align 8
  store i64 16, ptr %"%MAX_IRQ", align 8
  store i64 32, ptr %"%MAX_IO_REGIONS", align 8
  store i64 0, ptr %"%dev_type_0", align 8
  store i64 0, ptr %"%dev_type_1", align 8
  store i64 0, ptr %"%dev_type_2", align 8
  store i64 0, ptr %"%dev_type_3", align 8
  store i64 0, ptr %"%dev_type_4", align 8
  store i64 0, ptr %"%dev_type_5", align 8
  store i64 0, ptr %"%dev_type_6", align 8
  store i64 0, ptr %"%dev_type_7", align 8
  store i64 0, ptr %"%dev_type_8", align 8
  store i64 0, ptr %"%dev_type_9", align 8
  store i64 0, ptr %"%dev_type_10", align 8
  store i64 0, ptr %"%dev_type_11", align 8
  store i64 0, ptr %"%dev_type_12", align 8
  store i64 0, ptr %"%dev_type_13", align 8
  store i64 0, ptr %"%dev_type_14", align 8
  store i64 0, ptr %"%dev_type_15", align 8
  store i64 0, ptr %"%dev_type_16", align 8
  store i64 0, ptr %"%dev_type_17", align 8
  store i64 0, ptr %"%dev_type_18", align 8
  store i64 0, ptr %"%dev_type_19", align 8
  store i64 0, ptr %"%dev_type_20", align 8
  store i64 0, ptr %"%dev_type_21", align 8
  store i64 0, ptr %"%dev_type_22", align 8
  store i64 0, ptr %"%dev_type_23", align 8
  store i64 0, ptr %"%dev_type_24", align 8
  store i64 0, ptr %"%dev_type_25", align 8
  store i64 0, ptr %"%dev_type_26", align 8
  store i64 0, ptr %"%dev_type_27", align 8
  store i64 0, ptr %"%dev_type_28", align 8
  store i64 0, ptr %"%dev_type_29", align 8
  store i64 0, ptr %"%dev_type_30", align 8
  store i64 0, ptr %"%dev_type_31", align 8
  store i64 0, ptr %"%dev_type_32", align 8
  store i64 0, ptr %"%dev_type_33", align 8
  store i64 0, ptr %"%dev_type_34", align 8
  store i64 0, ptr %"%dev_type_35", align 8
  store i64 0, ptr %"%dev_type_36", align 8
  store i64 0, ptr %"%dev_type_37", align 8
  store i64 0, ptr %"%dev_type_38", align 8
  store i64 0, ptr %"%dev_type_39", align 8
  store i64 0, ptr %"%dev_type_40", align 8
  store i64 0, ptr %"%dev_type_41", align 8
  store i64 0, ptr %"%dev_type_42", align 8
  store i64 0, ptr %"%dev_type_43", align 8
  store i64 0, ptr %"%dev_type_44", align 8
  store i64 0, ptr %"%dev_type_45", align 8
  store i64 0, ptr %"%dev_type_46", align 8
  store i64 0, ptr %"%dev_type_47", align 8
  store i64 0, ptr %"%dev_type_48", align 8
  store i64 0, ptr %"%dev_type_49", align 8
  store i64 0, ptr %"%dev_type_50", align 8
  store i64 0, ptr %"%dev_type_51", align 8
  store i64 0, ptr %"%dev_type_52", align 8
  store i64 0, ptr %"%dev_type_53", align 8
  store i64 0, ptr %"%dev_type_54", align 8
  store i64 0, ptr %"%dev_type_55", align 8
  store i64 0, ptr %"%dev_type_56", align 8
  store i64 0, ptr %"%dev_type_57", align 8
  store i64 0, ptr %"%dev_type_58", align 8
  store i64 0, ptr %"%dev_type_59", align 8
  store i64 0, ptr %"%dev_type_60", align 8
  store i64 0, ptr %"%dev_type_61", align 8
  store i64 0, ptr %"%dev_type_62", align 8
  store i64 0, ptr %"%dev_type_63", align 8
  store i64 0, ptr %"%dev_state_0", align 8
  store i64 0, ptr %"%dev_state_1", align 8
  store i64 0, ptr %"%dev_state_2", align 8
  store i64 0, ptr %"%dev_state_3", align 8
  store i64 0, ptr %"%dev_state_4", align 8
  store i64 0, ptr %"%dev_state_5", align 8
  store i64 0, ptr %"%dev_state_6", align 8
  store i64 0, ptr %"%dev_state_7", align 8
  store i64 0, ptr %"%dev_state_8", align 8
  store i64 0, ptr %"%dev_state_9", align 8
  store i64 0, ptr %"%dev_state_10", align 8
  store i64 0, ptr %"%dev_state_11", align 8
  store i64 0, ptr %"%dev_state_12", align 8
  store i64 0, ptr %"%dev_state_13", align 8
  store i64 0, ptr %"%dev_state_14", align 8
  store i64 0, ptr %"%dev_state_15", align 8
  store i64 0, ptr %"%dev_state_16", align 8
  store i64 0, ptr %"%dev_state_17", align 8
  store i64 0, ptr %"%dev_state_18", align 8
  store i64 0, ptr %"%dev_state_19", align 8
  store i64 0, ptr %"%dev_state_20", align 8
  store i64 0, ptr %"%dev_state_21", align 8
  store i64 0, ptr %"%dev_state_22", align 8
  store i64 0, ptr %"%dev_state_23", align 8
  store i64 0, ptr %"%dev_state_24", align 8
  store i64 0, ptr %"%dev_state_25", align 8
  store i64 0, ptr %"%dev_state_26", align 8
  store i64 0, ptr %"%dev_state_27", align 8
  store i64 0, ptr %"%dev_state_28", align 8
  store i64 0, ptr %"%dev_state_29", align 8
  store i64 0, ptr %"%dev_state_30", align 8
  store i64 0, ptr %"%dev_state_31", align 8
  store i64 0, ptr %"%dev_state_32", align 8
  store i64 0, ptr %"%dev_state_33", align 8
  store i64 0, ptr %"%dev_state_34", align 8
  store i64 0, ptr %"%dev_state_35", align 8
  store i64 0, ptr %"%dev_state_36", align 8
  store i64 0, ptr %"%dev_state_37", align 8
  store i64 0, ptr %"%dev_state_38", align 8
  store i64 0, ptr %"%dev_state_39", align 8
  store i64 0, ptr %"%dev_state_40", align 8
  store i64 0, ptr %"%dev_state_41", align 8
  store i64 0, ptr %"%dev_state_42", align 8
  store i64 0, ptr %"%dev_state_43", align 8
  store i64 0, ptr %"%dev_state_44", align 8
  store i64 0, ptr %"%dev_state_45", align 8
  store i64 0, ptr %"%dev_state_46", align 8
  store i64 0, ptr %"%dev_state_47", align 8
  store i64 0, ptr %"%dev_state_48", align 8
  store i64 0, ptr %"%dev_state_49", align 8
  store i64 0, ptr %"%dev_state_50", align 8
  store i64 0, ptr %"%dev_state_51", align 8
  store i64 0, ptr %"%dev_state_52", align 8
  store i64 0, ptr %"%dev_state_53", align 8
  store i64 0, ptr %"%dev_state_54", align 8
  store i64 0, ptr %"%dev_state_55", align 8
  store i64 0, ptr %"%dev_state_56", align 8
  store i64 0, ptr %"%dev_state_57", align 8
  store i64 0, ptr %"%dev_state_58", align 8
  store i64 0, ptr %"%dev_state_59", align 8
  store i64 0, ptr %"%dev_state_60", align 8
  store i64 0, ptr %"%dev_state_61", align 8
  store i64 0, ptr %"%dev_state_62", align 8
  store i64 0, ptr %"%dev_state_63", align 8
  store i64 0, ptr %"%dev_vid_0", align 8
  store i64 0, ptr %"%dev_vid_1", align 8
  store i64 0, ptr %"%dev_vid_2", align 8
  store i64 0, ptr %"%dev_vid_3", align 8
  store i64 0, ptr %"%dev_vid_4", align 8
  store i64 0, ptr %"%dev_vid_5", align 8
  store i64 0, ptr %"%dev_vid_6", align 8
  store i64 0, ptr %"%dev_vid_7", align 8
  store i64 0, ptr %"%dev_vid_8", align 8
  store i64 0, ptr %"%dev_vid_9", align 8
  store i64 0, ptr %"%dev_vid_10", align 8
  store i64 0, ptr %"%dev_vid_11", align 8
  store i64 0, ptr %"%dev_vid_12", align 8
  store i64 0, ptr %"%dev_vid_13", align 8
  store i64 0, ptr %"%dev_vid_14", align 8
  store i64 0, ptr %"%dev_vid_15", align 8
  store i64 0, ptr %"%dev_vid_16", align 8
  store i64 0, ptr %"%dev_vid_17", align 8
  store i64 0, ptr %"%dev_vid_18", align 8
  store i64 0, ptr %"%dev_vid_19", align 8
  store i64 0, ptr %"%dev_vid_20", align 8
  store i64 0, ptr %"%dev_vid_21", align 8
  store i64 0, ptr %"%dev_vid_22", align 8
  store i64 0, ptr %"%dev_vid_23", align 8
  store i64 0, ptr %"%dev_vid_24", align 8
  store i64 0, ptr %"%dev_vid_25", align 8
  store i64 0, ptr %"%dev_vid_26", align 8
  store i64 0, ptr %"%dev_vid_27", align 8
  store i64 0, ptr %"%dev_vid_28", align 8
  store i64 0, ptr %"%dev_vid_29", align 8
  store i64 0, ptr %"%dev_vid_30", align 8
  store i64 0, ptr %"%dev_vid_31", align 8
  store i64 0, ptr %"%dev_did_0", align 8
  store i64 0, ptr %"%dev_did_1", align 8
  store i64 0, ptr %"%dev_did_2", align 8
  store i64 0, ptr %"%dev_did_3", align 8
  store i64 0, ptr %"%dev_did_4", align 8
  store i64 0, ptr %"%dev_did_5", align 8
  store i64 0, ptr %"%dev_did_6", align 8
  store i64 0, ptr %"%dev_did_7", align 8
  store i64 0, ptr %"%dev_did_8", align 8
  store i64 0, ptr %"%dev_did_9", align 8
  store i64 0, ptr %"%dev_did_10", align 8
  store i64 0, ptr %"%dev_did_11", align 8
  store i64 0, ptr %"%dev_did_12", align 8
  store i64 0, ptr %"%dev_did_13", align 8
  store i64 0, ptr %"%dev_did_14", align 8
  store i64 0, ptr %"%dev_did_15", align 8
  store i64 0, ptr %"%dev_did_16", align 8
  store i64 0, ptr %"%dev_did_17", align 8
  store i64 0, ptr %"%dev_did_18", align 8
  store i64 0, ptr %"%dev_did_19", align 8
  store i64 0, ptr %"%dev_did_20", align 8
  store i64 0, ptr %"%dev_did_21", align 8
  store i64 0, ptr %"%dev_did_22", align 8
  store i64 0, ptr %"%dev_did_23", align 8
  store i64 0, ptr %"%dev_did_24", align 8
  store i64 0, ptr %"%dev_did_25", align 8
  store i64 0, ptr %"%dev_did_26", align 8
  store i64 0, ptr %"%dev_did_27", align 8
  store i64 0, ptr %"%dev_did_28", align 8
  store i64 0, ptr %"%dev_did_29", align 8
  store i64 0, ptr %"%dev_did_30", align 8
  store i64 0, ptr %"%dev_did_31", align 8
  store i64 -1, ptr %"%dev_parent_0", align 8
  store i64 -1, ptr %"%dev_parent_1", align 8
  store i64 -1, ptr %"%dev_parent_2", align 8
  store i64 -1, ptr %"%dev_parent_3", align 8
  store i64 -1, ptr %"%dev_parent_4", align 8
  store i64 -1, ptr %"%dev_parent_5", align 8
  store i64 -1, ptr %"%dev_parent_6", align 8
  store i64 -1, ptr %"%dev_parent_7", align 8
  store i64 -1, ptr %"%dev_parent_8", align 8
  store i64 -1, ptr %"%dev_parent_9", align 8
  store i64 -1, ptr %"%dev_parent_10", align 8
  store i64 -1, ptr %"%dev_parent_11", align 8
  store i64 -1, ptr %"%dev_parent_12", align 8
  store i64 -1, ptr %"%dev_parent_13", align 8
  store i64 -1, ptr %"%dev_parent_14", align 8
  store i64 -1, ptr %"%dev_parent_15", align 8
  store i64 -1, ptr %"%dev_parent_16", align 8
  store i64 -1, ptr %"%dev_parent_17", align 8
  store i64 -1, ptr %"%dev_parent_18", align 8
  store i64 -1, ptr %"%dev_parent_19", align 8
  store i64 -1, ptr %"%dev_parent_20", align 8
  store i64 -1, ptr %"%dev_parent_21", align 8
  store i64 -1, ptr %"%dev_parent_22", align 8
  store i64 -1, ptr %"%dev_parent_23", align 8
  store i64 -1, ptr %"%dev_parent_24", align 8
  store i64 -1, ptr %"%dev_parent_25", align 8
  store i64 -1, ptr %"%dev_parent_26", align 8
  store i64 -1, ptr %"%dev_parent_27", align 8
  store i64 -1, ptr %"%dev_parent_28", align 8
  store i64 -1, ptr %"%dev_parent_29", align 8
  store i64 -1, ptr %"%dev_parent_30", align 8
  store i64 -1, ptr %"%dev_parent_31", align 8
  store i64 -1, ptr %"%dev_drv_0", align 8
  store i64 -1, ptr %"%dev_drv_1", align 8
  store i64 -1, ptr %"%dev_drv_2", align 8
  store i64 -1, ptr %"%dev_drv_3", align 8
  store i64 -1, ptr %"%dev_drv_4", align 8
  store i64 -1, ptr %"%dev_drv_5", align 8
  store i64 -1, ptr %"%dev_drv_6", align 8
  store i64 -1, ptr %"%dev_drv_7", align 8
  store i64 -1, ptr %"%dev_drv_8", align 8
  store i64 -1, ptr %"%dev_drv_9", align 8
  store i64 -1, ptr %"%dev_drv_10", align 8
  store i64 -1, ptr %"%dev_drv_11", align 8
  store i64 -1, ptr %"%dev_drv_12", align 8
  store i64 -1, ptr %"%dev_drv_13", align 8
  store i64 -1, ptr %"%dev_drv_14", align 8
  store i64 -1, ptr %"%dev_drv_15", align 8
  store i64 -1, ptr %"%dev_drv_16", align 8
  store i64 -1, ptr %"%dev_drv_17", align 8
  store i64 -1, ptr %"%dev_drv_18", align 8
  store i64 -1, ptr %"%dev_drv_19", align 8
  store i64 -1, ptr %"%dev_drv_20", align 8
  store i64 -1, ptr %"%dev_drv_21", align 8
  store i64 -1, ptr %"%dev_drv_22", align 8
  store i64 -1, ptr %"%dev_drv_23", align 8
  store i64 -1, ptr %"%dev_drv_24", align 8
  store i64 -1, ptr %"%dev_drv_25", align 8
  store i64 -1, ptr %"%dev_drv_26", align 8
  store i64 -1, ptr %"%dev_drv_27", align 8
  store i64 -1, ptr %"%dev_drv_28", align 8
  store i64 -1, ptr %"%dev_drv_29", align 8
  store i64 -1, ptr %"%dev_drv_30", align 8
  store i64 -1, ptr %"%dev_drv_31", align 8
  store i64 0, ptr %"%dev_class_0", align 8
  store i64 0, ptr %"%dev_class_1", align 8
  store i64 0, ptr %"%dev_class_2", align 8
  store i64 0, ptr %"%dev_class_3", align 8
  store i64 0, ptr %"%dev_class_4", align 8
  store i64 0, ptr %"%dev_class_5", align 8
  store i64 0, ptr %"%dev_class_6", align 8
  store i64 0, ptr %"%dev_class_7", align 8
  store i64 0, ptr %"%dev_class_8", align 8
  store i64 0, ptr %"%dev_class_9", align 8
  store i64 0, ptr %"%dev_class_10", align 8
  store i64 0, ptr %"%dev_class_11", align 8
  store i64 0, ptr %"%dev_class_12", align 8
  store i64 0, ptr %"%dev_class_13", align 8
  store i64 0, ptr %"%dev_class_14", align 8
  store i64 0, ptr %"%dev_class_15", align 8
  store i64 0, ptr %"%dev_class_16", align 8
  store i64 0, ptr %"%dev_class_17", align 8
  store i64 0, ptr %"%dev_class_18", align 8
  store i64 0, ptr %"%dev_class_19", align 8
  store i64 0, ptr %"%dev_class_20", align 8
  store i64 0, ptr %"%dev_class_21", align 8
  store i64 0, ptr %"%dev_class_22", align 8
  store i64 0, ptr %"%dev_class_23", align 8
  store i64 0, ptr %"%dev_class_24", align 8
  store i64 0, ptr %"%dev_class_25", align 8
  store i64 0, ptr %"%dev_class_26", align 8
  store i64 0, ptr %"%dev_class_27", align 8
  store i64 0, ptr %"%dev_class_28", align 8
  store i64 0, ptr %"%dev_class_29", align 8
  store i64 0, ptr %"%dev_class_30", align 8
  store i64 0, ptr %"%dev_class_31", align 8
  store i64 0, ptr %"%dev_sub_0", align 8
  store i64 0, ptr %"%dev_sub_1", align 8
  store i64 0, ptr %"%dev_sub_2", align 8
  store i64 0, ptr %"%dev_sub_3", align 8
  store i64 0, ptr %"%dev_sub_4", align 8
  store i64 0, ptr %"%dev_sub_5", align 8
  store i64 0, ptr %"%dev_sub_6", align 8
  store i64 0, ptr %"%dev_sub_7", align 8
  store i64 0, ptr %"%dev_sub_8", align 8
  store i64 0, ptr %"%dev_sub_9", align 8
  store i64 0, ptr %"%dev_sub_10", align 8
  store i64 0, ptr %"%dev_sub_11", align 8
  store i64 0, ptr %"%dev_sub_12", align 8
  store i64 0, ptr %"%dev_sub_13", align 8
  store i64 0, ptr %"%dev_sub_14", align 8
  store i64 0, ptr %"%dev_sub_15", align 8
  store i64 0, ptr %"%dev_sub_16", align 8
  store i64 0, ptr %"%dev_sub_17", align 8
  store i64 0, ptr %"%dev_sub_18", align 8
  store i64 0, ptr %"%dev_sub_19", align 8
  store i64 0, ptr %"%dev_sub_20", align 8
  store i64 0, ptr %"%dev_sub_21", align 8
  store i64 0, ptr %"%dev_sub_22", align 8
  store i64 0, ptr %"%dev_sub_23", align 8
  store i64 0, ptr %"%dev_sub_24", align 8
  store i64 0, ptr %"%dev_sub_25", align 8
  store i64 0, ptr %"%dev_sub_26", align 8
  store i64 0, ptr %"%dev_sub_27", align 8
  store i64 0, ptr %"%dev_sub_28", align 8
  store i64 0, ptr %"%dev_sub_29", align 8
  store i64 0, ptr %"%dev_sub_30", align 8
  store i64 0, ptr %"%dev_sub_31", align 8
  store i64 -1, ptr %"%dev_irq_0", align 8
  store i64 -1, ptr %"%dev_irq_1", align 8
  store i64 -1, ptr %"%dev_irq_2", align 8
  store i64 -1, ptr %"%dev_irq_3", align 8
  store i64 -1, ptr %"%dev_irq_4", align 8
  store i64 -1, ptr %"%dev_irq_5", align 8
  store i64 -1, ptr %"%dev_irq_6", align 8
  store i64 -1, ptr %"%dev_irq_7", align 8
  store i64 -1, ptr %"%dev_irq_8", align 8
  store i64 -1, ptr %"%dev_irq_9", align 8
  store i64 -1, ptr %"%dev_irq_10", align 8
  store i64 -1, ptr %"%dev_irq_11", align 8
  store i64 -1, ptr %"%dev_irq_12", align 8
  store i64 -1, ptr %"%dev_irq_13", align 8
  store i64 -1, ptr %"%dev_irq_14", align 8
  store i64 -1, ptr %"%dev_irq_15", align 8
  store i64 -1, ptr %"%dev_irq_16", align 8
  store i64 -1, ptr %"%dev_irq_17", align 8
  store i64 -1, ptr %"%dev_irq_18", align 8
  store i64 -1, ptr %"%dev_irq_19", align 8
  store i64 -1, ptr %"%dev_irq_20", align 8
  store i64 -1, ptr %"%dev_irq_21", align 8
  store i64 -1, ptr %"%dev_irq_22", align 8
  store i64 -1, ptr %"%dev_irq_23", align 8
  store i64 -1, ptr %"%dev_irq_24", align 8
  store i64 -1, ptr %"%dev_irq_25", align 8
  store i64 -1, ptr %"%dev_irq_26", align 8
  store i64 -1, ptr %"%dev_irq_27", align 8
  store i64 -1, ptr %"%dev_irq_28", align 8
  store i64 -1, ptr %"%dev_irq_29", align 8
  store i64 -1, ptr %"%dev_irq_30", align 8
  store i64 -1, ptr %"%dev_irq_31", align 8
  store i64 0, ptr %"%dev_io_0", align 8
  store i64 0, ptr %"%dev_io_1", align 8
  store i64 0, ptr %"%dev_io_2", align 8
  store i64 0, ptr %"%dev_io_3", align 8
  store i64 0, ptr %"%dev_io_4", align 8
  store i64 0, ptr %"%dev_io_5", align 8
  store i64 0, ptr %"%dev_io_6", align 8
  store i64 0, ptr %"%dev_io_7", align 8
  store i64 0, ptr %"%dev_io_8", align 8
  store i64 0, ptr %"%dev_io_9", align 8
  store i64 0, ptr %"%dev_io_10", align 8
  store i64 0, ptr %"%dev_io_11", align 8
  store i64 0, ptr %"%dev_io_12", align 8
  store i64 0, ptr %"%dev_io_13", align 8
  store i64 0, ptr %"%dev_io_14", align 8
  store i64 0, ptr %"%dev_io_15", align 8
  store i64 0, ptr %"%dev_io_16", align 8
  store i64 0, ptr %"%dev_io_17", align 8
  store i64 0, ptr %"%dev_io_18", align 8
  store i64 0, ptr %"%dev_io_19", align 8
  store i64 0, ptr %"%dev_io_20", align 8
  store i64 0, ptr %"%dev_io_21", align 8
  store i64 0, ptr %"%dev_io_22", align 8
  store i64 0, ptr %"%dev_io_23", align 8
  store i64 0, ptr %"%dev_io_24", align 8
  store i64 0, ptr %"%dev_io_25", align 8
  store i64 0, ptr %"%dev_io_26", align 8
  store i64 0, ptr %"%dev_io_27", align 8
  store i64 0, ptr %"%dev_io_28", align 8
  store i64 0, ptr %"%dev_io_29", align 8
  store i64 0, ptr %"%dev_io_30", align 8
  store i64 0, ptr %"%dev_io_31", align 8
  store i64 0, ptr %"%dev_mmio_0", align 8
  store i64 0, ptr %"%dev_mmio_1", align 8
  store i64 0, ptr %"%dev_mmio_2", align 8
  store i64 0, ptr %"%dev_mmio_3", align 8
  store i64 0, ptr %"%dev_mmio_4", align 8
  store i64 0, ptr %"%dev_mmio_5", align 8
  store i64 0, ptr %"%dev_mmio_6", align 8
  store i64 0, ptr %"%dev_mmio_7", align 8
  store i64 0, ptr %"%dev_mmio_8", align 8
  store i64 0, ptr %"%dev_mmio_9", align 8
  store i64 0, ptr %"%dev_mmio_10", align 8
  store i64 0, ptr %"%dev_mmio_11", align 8
  store i64 0, ptr %"%dev_mmio_12", align 8
  store i64 0, ptr %"%dev_mmio_13", align 8
  store i64 0, ptr %"%dev_mmio_14", align 8
  store i64 0, ptr %"%dev_mmio_15", align 8
  store i64 0, ptr %"%dev_mmio_16", align 8
  store i64 0, ptr %"%dev_mmio_17", align 8
  store i64 0, ptr %"%dev_mmio_18", align 8
  store i64 0, ptr %"%dev_mmio_19", align 8
  store i64 0, ptr %"%dev_mmio_20", align 8
  store i64 0, ptr %"%dev_mmio_21", align 8
  store i64 0, ptr %"%dev_mmio_22", align 8
  store i64 0, ptr %"%dev_mmio_23", align 8
  store i64 0, ptr %"%dev_mmio_24", align 8
  store i64 0, ptr %"%dev_mmio_25", align 8
  store i64 0, ptr %"%dev_mmio_26", align 8
  store i64 0, ptr %"%dev_mmio_27", align 8
  store i64 0, ptr %"%dev_mmio_28", align 8
  store i64 0, ptr %"%dev_mmio_29", align 8
  store i64 0, ptr %"%dev_mmio_30", align 8
  store i64 0, ptr %"%dev_mmio_31", align 8
  store i64 0, ptr %"%drv_type_0", align 8
  store i64 0, ptr %"%drv_type_1", align 8
  store i64 0, ptr %"%drv_type_2", align 8
  store i64 0, ptr %"%drv_type_3", align 8
  store i64 0, ptr %"%drv_type_4", align 8
  store i64 0, ptr %"%drv_type_5", align 8
  store i64 0, ptr %"%drv_type_6", align 8
  store i64 0, ptr %"%drv_type_7", align 8
  store i64 0, ptr %"%drv_type_8", align 8
  store i64 0, ptr %"%drv_type_9", align 8
  store i64 0, ptr %"%drv_type_10", align 8
  store i64 0, ptr %"%drv_type_11", align 8
  store i64 0, ptr %"%drv_type_12", align 8
  store i64 0, ptr %"%drv_type_13", align 8
  store i64 0, ptr %"%drv_type_14", align 8
  store i64 0, ptr %"%drv_type_15", align 8
  store i64 0, ptr %"%drv_reg_0", align 8
  store i64 0, ptr %"%drv_reg_1", align 8
  store i64 0, ptr %"%drv_reg_2", align 8
  store i64 0, ptr %"%drv_reg_3", align 8
  store i64 0, ptr %"%drv_reg_4", align 8
  store i64 0, ptr %"%drv_reg_5", align 8
  store i64 0, ptr %"%drv_reg_6", align 8
  store i64 0, ptr %"%drv_reg_7", align 8
  store i64 0, ptr %"%drv_reg_8", align 8
  store i64 0, ptr %"%drv_reg_9", align 8
  store i64 0, ptr %"%drv_reg_10", align 8
  store i64 0, ptr %"%drv_reg_11", align 8
  store i64 0, ptr %"%drv_reg_12", align 8
  store i64 0, ptr %"%drv_reg_13", align 8
  store i64 0, ptr %"%drv_reg_14", align 8
  store i64 0, ptr %"%drv_reg_15", align 8
  store i64 -1, ptr %"%drv_pclass_0", align 8
  store i64 -1, ptr %"%drv_pclass_1", align 8
  store i64 -1, ptr %"%drv_pclass_2", align 8
  store i64 -1, ptr %"%drv_pclass_3", align 8
  store i64 -1, ptr %"%drv_pclass_4", align 8
  store i64 -1, ptr %"%drv_pclass_5", align 8
  store i64 -1, ptr %"%drv_pclass_6", align 8
  store i64 -1, ptr %"%drv_pclass_7", align 8
  store i64 -1, ptr %"%drv_pclass_8", align 8
  store i64 -1, ptr %"%drv_pclass_9", align 8
  store i64 -1, ptr %"%drv_pclass_10", align 8
  store i64 -1, ptr %"%drv_pclass_11", align 8
  store i64 -1, ptr %"%drv_pclass_12", align 8
  store i64 -1, ptr %"%drv_pclass_13", align 8
  store i64 -1, ptr %"%drv_pclass_14", align 8
  store i64 -1, ptr %"%drv_pclass_15", align 8
  store i64 -1, ptr %"%drv_psub_0", align 8
  store i64 -1, ptr %"%drv_psub_1", align 8
  store i64 -1, ptr %"%drv_psub_2", align 8
  store i64 -1, ptr %"%drv_psub_3", align 8
  store i64 -1, ptr %"%drv_psub_4", align 8
  store i64 -1, ptr %"%drv_psub_5", align 8
  store i64 -1, ptr %"%drv_psub_6", align 8
  store i64 -1, ptr %"%drv_psub_7", align 8
  store i64 -1, ptr %"%drv_psub_8", align 8
  store i64 -1, ptr %"%drv_psub_9", align 8
  store i64 -1, ptr %"%drv_psub_10", align 8
  store i64 -1, ptr %"%drv_psub_11", align 8
  store i64 -1, ptr %"%drv_psub_12", align 8
  store i64 -1, ptr %"%drv_psub_13", align 8
  store i64 -1, ptr %"%drv_psub_14", align 8
  store i64 -1, ptr %"%drv_psub_15", align 8
  store i64 0, ptr %"%drv_devcount_0", align 8
  store i64 0, ptr %"%drv_devcount_1", align 8
  store i64 0, ptr %"%drv_devcount_2", align 8
  store i64 0, ptr %"%drv_devcount_3", align 8
  store i64 0, ptr %"%drv_devcount_4", align 8
  store i64 0, ptr %"%drv_devcount_5", align 8
  store i64 0, ptr %"%drv_devcount_6", align 8
  store i64 0, ptr %"%drv_devcount_7", align 8
  store i64 0, ptr %"%drv_devcount_8", align 8
  store i64 0, ptr %"%drv_devcount_9", align 8
  store i64 0, ptr %"%drv_devcount_10", align 8
  store i64 0, ptr %"%drv_devcount_11", align 8
  store i64 0, ptr %"%drv_devcount_12", align 8
  store i64 0, ptr %"%drv_devcount_13", align 8
  store i64 0, ptr %"%drv_devcount_14", align 8
  store i64 0, ptr %"%drv_devcount_15", align 8
  store i64 -1, ptr %"%irq_owner_0", align 8
  store i64 -1, ptr %"%irq_owner_1", align 8
  store i64 -1, ptr %"%irq_owner_2", align 8
  store i64 -1, ptr %"%irq_owner_3", align 8
  store i64 -1, ptr %"%irq_owner_4", align 8
  store i64 -1, ptr %"%irq_owner_5", align 8
  store i64 -1, ptr %"%irq_owner_6", align 8
  store i64 -1, ptr %"%irq_owner_7", align 8
  store i64 -1, ptr %"%irq_owner_8", align 8
  store i64 -1, ptr %"%irq_owner_9", align 8
  store i64 -1, ptr %"%irq_owner_10", align 8
  store i64 -1, ptr %"%irq_owner_11", align 8
  store i64 -1, ptr %"%irq_owner_12", align 8
  store i64 -1, ptr %"%irq_owner_13", align 8
  store i64 -1, ptr %"%irq_owner_14", align 8
  store i64 -1, ptr %"%irq_owner_15", align 8
  store i64 0, ptr %"%devmgr_dev_count", align 8
  store i64 0, ptr %"%devmgr_drv_count", align 8
  store i64 0, ptr %"%devmgr_bindings", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
