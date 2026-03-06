; ModuleID = 'pci_نحلة'
source_filename = "pci_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@COM1 = constant i64 1016
@PCI_CONFIG_ADDR = constant i64 3320
@PCI_CONFIG_DATA = constant i64 3324
@PCI_VENDOR_ID = constant i64 0
@PCI_DEVICE_ID = constant i64 2
@PCI_COMMAND = constant i64 4
@PCI_STATUS = constant i64 6
@PCI_REVISION = constant i64 8
@PCI_PROG_IF = constant i64 9
@PCI_SUBCLASS = constant i64 10
@PCI_CLASS_CODE = constant i64 11
@PCI_CACHE_LINE = constant i64 12
@PCI_LATENCY_TIMER = constant i64 13
@PCI_HEADER_TYPE = constant i64 14
@PCI_BIST = constant i64 15
@PCI_BAR0 = constant i64 16
@PCI_BAR1 = constant i64 20
@PCI_BAR2 = constant i64 24
@PCI_BAR3 = constant i64 28
@PCI_BAR4 = constant i64 32
@PCI_BAR5 = constant i64 36
@PCI_SUBSYSTEM_VENDOR = constant i64 44
@PCI_SUBSYSTEM_ID = constant i64 46
@PCI_IRQ_LINE = constant i64 60
@PCI_IRQ_PIN = constant i64 61
@PCI_MIN_GRANT = constant i64 62
@PCI_MAX_LATENCY = constant i64 63
@PCI_CMD_IO_ENABLE = constant i64 1
@PCI_CMD_MEM_ENABLE = constant i64 2
@PCI_CMD_BUS_MASTER = constant i64 4
@PCI_CMD_SPECIAL = constant i64 8
@PCI_CMD_INVALIDATE = constant i64 16
@PCI_CMD_PALETTE = constant i64 32
@PCI_CMD_PARITY = constant i64 64
@PCI_CMD_SERR = constant i64 256
@PCI_CMD_FAST_BTB = constant i64 512
@PCI_CMD_INT_DISABLE = constant i64 1024
@PCI_CLASS_STORAGE = constant i64 1
@PCI_CLASS_NETWORK = constant i64 2
@PCI_CLASS_DISPLAY = constant i64 3
@PCI_CLASS_MULTIMEDIA = constant i64 4
@PCI_CLASS_MEMORY = constant i64 5
@PCI_CLASS_BRIDGE = constant i64 6
@PCI_CLASS_COMM = constant i64 7
@PCI_CLASS_SYSTEM = constant i64 8
@PCI_CLASS_INPUT = constant i64 9
@PCI_CLASS_DOCK = constant i64 10
@PCI_CLASS_PROCESSOR = constant i64 11
@PCI_CLASS_SERIAL = constant i64 12
@PCI_CLASS_WIRELESS = constant i64 13
@PCI_SUBCLASS_USB = constant i64 3
@PCI_SUBCLASS_FIREWIRE = constant i64 0
@PCI_SUBCLASS_SMBUS = constant i64 5
@PCI_PROGIF_UHCI = constant i64 0
@PCI_PROGIF_OHCI = constant i64 16
@PCI_PROGIF_EHCI = constant i64 32
@PCI_PROGIF_XHCI = constant i64 48
@PCI_MAX_BUS = constant i64 8
@PCI_MAX_DEV = constant i64 32
@PCI_MAX_FUNC = constant i64 8
@PCI_VENDOR_INVALID = constant i64 65535
@pci_dev_count = global i64 0
@pci_loc_0 = global i64 0
@pci_loc_1 = global i64 0
@pci_loc_2 = global i64 0
@pci_loc_3 = global i64 0
@pci_loc_4 = global i64 0
@pci_loc_5 = global i64 0
@pci_loc_6 = global i64 0
@pci_loc_7 = global i64 0
@pci_loc_8 = global i64 0
@pci_loc_9 = global i64 0
@pci_loc_10 = global i64 0
@pci_loc_11 = global i64 0
@pci_loc_12 = global i64 0
@pci_loc_13 = global i64 0
@pci_loc_14 = global i64 0
@pci_loc_15 = global i64 0
@pci_loc_16 = global i64 0
@pci_loc_17 = global i64 0
@pci_loc_18 = global i64 0
@pci_loc_19 = global i64 0
@pci_loc_20 = global i64 0
@pci_loc_21 = global i64 0
@pci_loc_22 = global i64 0
@pci_loc_23 = global i64 0
@pci_loc_24 = global i64 0
@pci_loc_25 = global i64 0
@pci_loc_26 = global i64 0
@pci_loc_27 = global i64 0
@pci_loc_28 = global i64 0
@pci_loc_29 = global i64 0
@pci_loc_30 = global i64 0
@pci_loc_31 = global i64 0
@pci_loc_32 = global i64 0
@pci_loc_33 = global i64 0
@pci_loc_34 = global i64 0
@pci_loc_35 = global i64 0
@pci_loc_36 = global i64 0
@pci_loc_37 = global i64 0
@pci_loc_38 = global i64 0
@pci_loc_39 = global i64 0
@pci_loc_40 = global i64 0
@pci_loc_41 = global i64 0
@pci_loc_42 = global i64 0
@pci_loc_43 = global i64 0
@pci_loc_44 = global i64 0
@pci_loc_45 = global i64 0
@pci_loc_46 = global i64 0
@pci_loc_47 = global i64 0
@pci_loc_48 = global i64 0
@pci_loc_49 = global i64 0
@pci_loc_50 = global i64 0
@pci_loc_51 = global i64 0
@pci_loc_52 = global i64 0
@pci_loc_53 = global i64 0
@pci_loc_54 = global i64 0
@pci_loc_55 = global i64 0
@pci_loc_56 = global i64 0
@pci_loc_57 = global i64 0
@pci_loc_58 = global i64 0
@pci_loc_59 = global i64 0
@pci_loc_60 = global i64 0
@pci_loc_61 = global i64 0
@pci_loc_62 = global i64 0
@pci_loc_63 = global i64 0
@pci_vid_0 = global i64 0
@pci_vid_1 = global i64 0
@pci_vid_2 = global i64 0
@pci_vid_3 = global i64 0
@pci_vid_4 = global i64 0
@pci_vid_5 = global i64 0
@pci_vid_6 = global i64 0
@pci_vid_7 = global i64 0
@pci_vid_8 = global i64 0
@pci_vid_9 = global i64 0
@pci_vid_10 = global i64 0
@pci_vid_11 = global i64 0
@pci_vid_12 = global i64 0
@pci_vid_13 = global i64 0
@pci_vid_14 = global i64 0
@pci_vid_15 = global i64 0
@pci_vid_16 = global i64 0
@pci_vid_17 = global i64 0
@pci_vid_18 = global i64 0
@pci_vid_19 = global i64 0
@pci_vid_20 = global i64 0
@pci_vid_21 = global i64 0
@pci_vid_22 = global i64 0
@pci_vid_23 = global i64 0
@pci_vid_24 = global i64 0
@pci_vid_25 = global i64 0
@pci_vid_26 = global i64 0
@pci_vid_27 = global i64 0
@pci_vid_28 = global i64 0
@pci_vid_29 = global i64 0
@pci_vid_30 = global i64 0
@pci_vid_31 = global i64 0
@pci_vid_32 = global i64 0
@pci_vid_33 = global i64 0
@pci_vid_34 = global i64 0
@pci_vid_35 = global i64 0
@pci_vid_36 = global i64 0
@pci_vid_37 = global i64 0
@pci_vid_38 = global i64 0
@pci_vid_39 = global i64 0
@pci_vid_40 = global i64 0
@pci_vid_41 = global i64 0
@pci_vid_42 = global i64 0
@pci_vid_43 = global i64 0
@pci_vid_44 = global i64 0
@pci_vid_45 = global i64 0
@pci_vid_46 = global i64 0
@pci_vid_47 = global i64 0
@pci_vid_48 = global i64 0
@pci_vid_49 = global i64 0
@pci_vid_50 = global i64 0
@pci_vid_51 = global i64 0
@pci_vid_52 = global i64 0
@pci_vid_53 = global i64 0
@pci_vid_54 = global i64 0
@pci_vid_55 = global i64 0
@pci_vid_56 = global i64 0
@pci_vid_57 = global i64 0
@pci_vid_58 = global i64 0
@pci_vid_59 = global i64 0
@pci_vid_60 = global i64 0
@pci_vid_61 = global i64 0
@pci_vid_62 = global i64 0
@pci_vid_63 = global i64 0
@pci_did_0 = global i64 0
@pci_did_1 = global i64 0
@pci_did_2 = global i64 0
@pci_did_3 = global i64 0
@pci_did_4 = global i64 0
@pci_did_5 = global i64 0
@pci_did_6 = global i64 0
@pci_did_7 = global i64 0
@pci_did_8 = global i64 0
@pci_did_9 = global i64 0
@pci_did_10 = global i64 0
@pci_did_11 = global i64 0
@pci_did_12 = global i64 0
@pci_did_13 = global i64 0
@pci_did_14 = global i64 0
@pci_did_15 = global i64 0
@pci_did_16 = global i64 0
@pci_did_17 = global i64 0
@pci_did_18 = global i64 0
@pci_did_19 = global i64 0
@pci_did_20 = global i64 0
@pci_did_21 = global i64 0
@pci_did_22 = global i64 0
@pci_did_23 = global i64 0
@pci_did_24 = global i64 0
@pci_did_25 = global i64 0
@pci_did_26 = global i64 0
@pci_did_27 = global i64 0
@pci_did_28 = global i64 0
@pci_did_29 = global i64 0
@pci_did_30 = global i64 0
@pci_did_31 = global i64 0
@pci_did_32 = global i64 0
@pci_did_33 = global i64 0
@pci_did_34 = global i64 0
@pci_did_35 = global i64 0
@pci_did_36 = global i64 0
@pci_did_37 = global i64 0
@pci_did_38 = global i64 0
@pci_did_39 = global i64 0
@pci_did_40 = global i64 0
@pci_did_41 = global i64 0
@pci_did_42 = global i64 0
@pci_did_43 = global i64 0
@pci_did_44 = global i64 0
@pci_did_45 = global i64 0
@pci_did_46 = global i64 0
@pci_did_47 = global i64 0
@pci_did_48 = global i64 0
@pci_did_49 = global i64 0
@pci_did_50 = global i64 0
@pci_did_51 = global i64 0
@pci_did_52 = global i64 0
@pci_did_53 = global i64 0
@pci_did_54 = global i64 0
@pci_did_55 = global i64 0
@pci_did_56 = global i64 0
@pci_did_57 = global i64 0
@pci_did_58 = global i64 0
@pci_did_59 = global i64 0
@pci_did_60 = global i64 0
@pci_did_61 = global i64 0
@pci_did_62 = global i64 0
@pci_did_63 = global i64 0
@pci_class_0 = global i64 0
@pci_class_1 = global i64 0
@pci_class_2 = global i64 0
@pci_class_3 = global i64 0
@pci_class_4 = global i64 0
@pci_class_5 = global i64 0
@pci_class_6 = global i64 0
@pci_class_7 = global i64 0
@pci_class_8 = global i64 0
@pci_class_9 = global i64 0
@pci_class_10 = global i64 0
@pci_class_11 = global i64 0
@pci_class_12 = global i64 0
@pci_class_13 = global i64 0
@pci_class_14 = global i64 0
@pci_class_15 = global i64 0
@pci_class_16 = global i64 0
@pci_class_17 = global i64 0
@pci_class_18 = global i64 0
@pci_class_19 = global i64 0
@pci_class_20 = global i64 0
@pci_class_21 = global i64 0
@pci_class_22 = global i64 0
@pci_class_23 = global i64 0
@pci_class_24 = global i64 0
@pci_class_25 = global i64 0
@pci_class_26 = global i64 0
@pci_class_27 = global i64 0
@pci_class_28 = global i64 0
@pci_class_29 = global i64 0
@pci_class_30 = global i64 0
@pci_class_31 = global i64 0
@pci_class_32 = global i64 0
@pci_class_33 = global i64 0
@pci_class_34 = global i64 0
@pci_class_35 = global i64 0
@pci_class_36 = global i64 0
@pci_class_37 = global i64 0
@pci_class_38 = global i64 0
@pci_class_39 = global i64 0
@pci_class_40 = global i64 0
@pci_class_41 = global i64 0
@pci_class_42 = global i64 0
@pci_class_43 = global i64 0
@pci_class_44 = global i64 0
@pci_class_45 = global i64 0
@pci_class_46 = global i64 0
@pci_class_47 = global i64 0
@pci_class_48 = global i64 0
@pci_class_49 = global i64 0
@pci_class_50 = global i64 0
@pci_class_51 = global i64 0
@pci_class_52 = global i64 0
@pci_class_53 = global i64 0
@pci_class_54 = global i64 0
@pci_class_55 = global i64 0
@pci_class_56 = global i64 0
@pci_class_57 = global i64 0
@pci_class_58 = global i64 0
@pci_class_59 = global i64 0
@pci_class_60 = global i64 0
@pci_class_61 = global i64 0
@pci_class_62 = global i64 0
@pci_class_63 = global i64 0
@pci_bar0_0 = global i64 0
@pci_bar0_1 = global i64 0
@pci_bar0_2 = global i64 0
@pci_bar0_3 = global i64 0
@pci_bar0_4 = global i64 0
@pci_bar0_5 = global i64 0
@pci_bar0_6 = global i64 0
@pci_bar0_7 = global i64 0
@pci_bar0_8 = global i64 0
@pci_bar0_9 = global i64 0
@pci_bar0_10 = global i64 0
@pci_bar0_11 = global i64 0
@pci_bar0_12 = global i64 0
@pci_bar0_13 = global i64 0
@pci_bar0_14 = global i64 0
@pci_bar0_15 = global i64 0
@pci_bar0_16 = global i64 0
@pci_bar0_17 = global i64 0
@pci_bar0_18 = global i64 0
@pci_bar0_19 = global i64 0
@pci_bar0_20 = global i64 0
@pci_bar0_21 = global i64 0
@pci_bar0_22 = global i64 0
@pci_bar0_23 = global i64 0
@pci_bar0_24 = global i64 0
@pci_bar0_25 = global i64 0
@pci_bar0_26 = global i64 0
@pci_bar0_27 = global i64 0
@pci_bar0_28 = global i64 0
@pci_bar0_29 = global i64 0
@pci_bar0_30 = global i64 0
@pci_bar0_31 = global i64 0
@pci_bar0_32 = global i64 0
@pci_bar0_33 = global i64 0
@pci_bar0_34 = global i64 0
@pci_bar0_35 = global i64 0
@pci_bar0_36 = global i64 0
@pci_bar0_37 = global i64 0
@pci_bar0_38 = global i64 0
@pci_bar0_39 = global i64 0
@pci_bar0_40 = global i64 0
@pci_bar0_41 = global i64 0
@pci_bar0_42 = global i64 0
@pci_bar0_43 = global i64 0
@pci_bar0_44 = global i64 0
@pci_bar0_45 = global i64 0
@pci_bar0_46 = global i64 0
@pci_bar0_47 = global i64 0
@pci_bar0_48 = global i64 0
@pci_bar0_49 = global i64 0
@pci_bar0_50 = global i64 0
@pci_bar0_51 = global i64 0
@pci_bar0_52 = global i64 0
@pci_bar0_53 = global i64 0
@pci_bar0_54 = global i64 0
@pci_bar0_55 = global i64 0
@pci_bar0_56 = global i64 0
@pci_bar0_57 = global i64 0
@pci_bar0_58 = global i64 0
@pci_bar0_59 = global i64 0
@pci_bar0_60 = global i64 0
@pci_bar0_61 = global i64 0
@pci_bar0_62 = global i64 0
@pci_bar0_63 = global i64 0
@pci_bar1_0 = global i64 0
@pci_bar1_1 = global i64 0
@pci_bar1_2 = global i64 0
@pci_bar1_3 = global i64 0
@pci_bar1_4 = global i64 0
@pci_bar1_5 = global i64 0
@pci_bar1_6 = global i64 0
@pci_bar1_7 = global i64 0
@pci_bar1_8 = global i64 0
@pci_bar1_9 = global i64 0
@pci_bar1_10 = global i64 0
@pci_bar1_11 = global i64 0
@pci_bar1_12 = global i64 0
@pci_bar1_13 = global i64 0
@pci_bar1_14 = global i64 0
@pci_bar1_15 = global i64 0
@pci_bar1_16 = global i64 0
@pci_bar1_17 = global i64 0
@pci_bar1_18 = global i64 0
@pci_bar1_19 = global i64 0
@pci_bar1_20 = global i64 0
@pci_bar1_21 = global i64 0
@pci_bar1_22 = global i64 0
@pci_bar1_23 = global i64 0
@pci_bar1_24 = global i64 0
@pci_bar1_25 = global i64 0
@pci_bar1_26 = global i64 0
@pci_bar1_27 = global i64 0
@pci_bar1_28 = global i64 0
@pci_bar1_29 = global i64 0
@pci_bar1_30 = global i64 0
@pci_bar1_31 = global i64 0
@pci_bar1_32 = global i64 0
@pci_bar1_33 = global i64 0
@pci_bar1_34 = global i64 0
@pci_bar1_35 = global i64 0
@pci_bar1_36 = global i64 0
@pci_bar1_37 = global i64 0
@pci_bar1_38 = global i64 0
@pci_bar1_39 = global i64 0
@pci_bar1_40 = global i64 0
@pci_bar1_41 = global i64 0
@pci_bar1_42 = global i64 0
@pci_bar1_43 = global i64 0
@pci_bar1_44 = global i64 0
@pci_bar1_45 = global i64 0
@pci_bar1_46 = global i64 0
@pci_bar1_47 = global i64 0
@pci_bar1_48 = global i64 0
@pci_bar1_49 = global i64 0
@pci_bar1_50 = global i64 0
@pci_bar1_51 = global i64 0
@pci_bar1_52 = global i64 0
@pci_bar1_53 = global i64 0
@pci_bar1_54 = global i64 0
@pci_bar1_55 = global i64 0
@pci_bar1_56 = global i64 0
@pci_bar1_57 = global i64 0
@pci_bar1_58 = global i64 0
@pci_bar1_59 = global i64 0
@pci_bar1_60 = global i64 0
@pci_bar1_61 = global i64 0
@pci_bar1_62 = global i64 0
@pci_bar1_63 = global i64 0
@pci_irq_0 = global i64 0
@pci_irq_1 = global i64 0
@pci_irq_2 = global i64 0
@pci_irq_3 = global i64 0
@pci_irq_4 = global i64 0
@pci_irq_5 = global i64 0
@pci_irq_6 = global i64 0
@pci_irq_7 = global i64 0
@pci_irq_8 = global i64 0
@pci_irq_9 = global i64 0
@pci_irq_10 = global i64 0
@pci_irq_11 = global i64 0
@pci_irq_12 = global i64 0
@pci_irq_13 = global i64 0
@pci_irq_14 = global i64 0
@pci_irq_15 = global i64 0
@pci_irq_16 = global i64 0
@pci_irq_17 = global i64 0
@pci_irq_18 = global i64 0
@pci_irq_19 = global i64 0
@pci_irq_20 = global i64 0
@pci_irq_21 = global i64 0
@pci_irq_22 = global i64 0
@pci_irq_23 = global i64 0
@pci_irq_24 = global i64 0
@pci_irq_25 = global i64 0
@pci_irq_26 = global i64 0
@pci_irq_27 = global i64 0
@pci_irq_28 = global i64 0
@pci_irq_29 = global i64 0
@pci_irq_30 = global i64 0
@pci_irq_31 = global i64 0
@pci_irq_32 = global i64 0
@pci_irq_33 = global i64 0
@pci_irq_34 = global i64 0
@pci_irq_35 = global i64 0
@pci_irq_36 = global i64 0
@pci_irq_37 = global i64 0
@pci_irq_38 = global i64 0
@pci_irq_39 = global i64 0
@pci_irq_40 = global i64 0
@pci_irq_41 = global i64 0
@pci_irq_42 = global i64 0
@pci_irq_43 = global i64 0
@pci_irq_44 = global i64 0
@pci_irq_45 = global i64 0
@pci_irq_46 = global i64 0
@pci_irq_47 = global i64 0
@pci_irq_48 = global i64 0
@pci_irq_49 = global i64 0
@pci_irq_50 = global i64 0
@pci_irq_51 = global i64 0
@pci_irq_52 = global i64 0
@pci_irq_53 = global i64 0
@pci_irq_54 = global i64 0
@pci_irq_55 = global i64 0
@pci_irq_56 = global i64 0
@pci_irq_57 = global i64 0
@pci_irq_58 = global i64 0
@pci_irq_59 = global i64 0
@pci_irq_60 = global i64 0
@pci_irq_61 = global i64 0
@pci_irq_62 = global i64 0
@pci_irq_63 = global i64 0
@pci_usb_count = global i64 0
@pci_net_count = global i64 0
@pci_disk_count = global i64 0
@pci_display_count = global i64 0
@pci_audio_count = global i64 0
@pci_bridge_count = global i64 0
@pci_other_count = global i64 0
@PCI_CAP_MSI = constant i64 5
@str.const = private unnamed_addr constant [7 x i8] c"[PCI] \00", align 1
@str.const.1 = private unnamed_addr constant [8 x i8] c" VID=0x\00", align 1
@str.const.2 = private unnamed_addr constant [8 x i8] c" DID=0x\00", align 1
@str.const.3 = private unnamed_addr constant [8 x i8] c" Class=\00", align 1
@str.const.4 = private unnamed_addr constant [9 x i8] c" BAR0=0x\00", align 1
@str.const.5 = private unnamed_addr constant [6 x i8] c" IRQ=\00", align 1
@str.const.6 = private unnamed_addr constant [28 x i8] c"[PCI] Scanning buses 0-7...\00", align 1
@str.const.7 = private unnamed_addr constant [22 x i8] c"[PCI] Scan complete: \00", align 1
@str.const.8 = private unnamed_addr constant [15 x i8] c" devices found\00", align 1
@str.const.9 = private unnamed_addr constant [24 x i8] c"[PCI] MSI not supported\00", align 1
@str.const.10 = private unnamed_addr constant [27 x i8] c"[PCI] MSI enabled, vector=\00", align 1
@str.const.11 = private unnamed_addr constant [52 x i8] c"\E2\95\90\E2\95\90\E2\95\90 [PCI] \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9 \E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.12 = private unnamed_addr constant [32 x i8] c"  \D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A \D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9: \00", align 1
@str.const.13 = private unnamed_addr constant [15 x i8] c"  \D8\AA\D8\AE\D8\B2\D9\8A\D9\86: \00", align 1
@str.const.14 = private unnamed_addr constant [13 x i8] c"  \D8\B4\D8\A8\D9\83\D8\A9: \00", align 1
@str.const.15 = private unnamed_addr constant [11 x i8] c"  \D8\B9\D8\B1\D8\B6: \00", align 1
@str.const.16 = private unnamed_addr constant [11 x i8] c"  \D8\B5\D9\88\D8\AA: \00", align 1
@str.const.17 = private unnamed_addr constant [8 x i8] c"  USB: \00", align 1
@str.const.18 = private unnamed_addr constant [13 x i8] c"  \D8\AC\D8\B3\D9\88\D8\B1: \00", align 1
@str.const.19 = private unnamed_addr constant [4 x i8] c"  [\00", align 1
@str.const.20 = private unnamed_addr constant [3 x i8] c"] \00", align 1
@str.const.21 = private unnamed_addr constant [4 x i8] c" 0x\00", align 1
@str.const.22 = private unnamed_addr constant [4 x i8] c" C=\00", align 1
@str.const.23 = private unnamed_addr constant [7 x i8] c" B0=0x\00", align 1
@str.const.24 = private unnamed_addr constant [106 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.25 = private unnamed_addr constant [33 x i8] c"[PCI] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D9\86\D8\B8\D8\A7\D9\85 PCI...\00", align 1
@str.const.26 = private unnamed_addr constant [15 x i8] c"[PCI] \D8\AC\D8\A7\D9\87\D8\B2\00", align 1

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

define i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
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

define i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
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

define i64 @pci_serial_hex8(i64 %val) {
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

define i64 @pci_serial_hex16(i64 %val) {
entry:
  %val1 = alloca i64, align 8
  store i64 %val, ptr %val1, align 8
  %"%val.load" = load i64, ptr %val1, align 8
  %shrtmp = lshr i64 %"%val.load", 8
  %calltmp = call i64 @pci_serial_hex8(i64 %shrtmp)
  %"%val.load2" = load i64, ptr %val1, align 8
  %andtmp = and i64 %"%val.load2", 255
  %calltmp3 = call i64 @pci_serial_hex8(i64 %andtmp)
  ret i64 0
}

define i64 @pci_serial_hex32(i64 %val) {
entry:
  %val1 = alloca i64, align 8
  store i64 %val, ptr %val1, align 8
  %"%val.load" = load i64, ptr %val1, align 8
  %shrtmp = lshr i64 %"%val.load", 16
  %calltmp = call i64 @pci_serial_hex16(i64 %shrtmp)
  %"%val.load2" = load i64, ptr %val1, align 8
  %andtmp = and i64 %"%val.load2", 65535
  %calltmp3 = call i64 @pci_serial_hex16(i64 %andtmp)
  ret i64 0
}

define i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"() {
entry:
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 13)
  %"%COM1.load1" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load1", i64 10)
  ret i64 0
}

define i64 @pci_get_loc(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_30, label %merge_31

merge_101:                                        ; preds = %merge_99
  %"%idx.load107" = load i64, ptr %idx1, align 8
  %cmpeqtmp108 = icmp eq i64 %"%idx.load107", 36
  br i1 %cmpeqtmp108, label %then_102, label %merge_103

merge_103:                                        ; preds = %merge_101
  %"%idx.load110" = load i64, ptr %idx1, align 8
  %cmpeqtmp111 = icmp eq i64 %"%idx.load110", 37
  br i1 %cmpeqtmp111, label %then_104, label %merge_105

merge_105:                                        ; preds = %merge_103
  %"%idx.load113" = load i64, ptr %idx1, align 8
  %cmpeqtmp114 = icmp eq i64 %"%idx.load113", 38
  br i1 %cmpeqtmp114, label %then_106, label %merge_107

merge_107:                                        ; preds = %merge_105
  %"%idx.load116" = load i64, ptr %idx1, align 8
  %cmpeqtmp117 = icmp eq i64 %"%idx.load116", 39
  br i1 %cmpeqtmp117, label %then_108, label %merge_109

merge_109:                                        ; preds = %merge_107
  %"%idx.load119" = load i64, ptr %idx1, align 8
  %cmpeqtmp120 = icmp eq i64 %"%idx.load119", 40
  br i1 %cmpeqtmp120, label %then_110, label %merge_111

merge_111:                                        ; preds = %merge_109
  %"%idx.load122" = load i64, ptr %idx1, align 8
  %cmpeqtmp123 = icmp eq i64 %"%idx.load122", 41
  br i1 %cmpeqtmp123, label %then_112, label %merge_113

merge_113:                                        ; preds = %merge_111
  %"%idx.load125" = load i64, ptr %idx1, align 8
  %cmpeqtmp126 = icmp eq i64 %"%idx.load125", 42
  br i1 %cmpeqtmp126, label %then_114, label %merge_115

merge_115:                                        ; preds = %merge_113
  %"%idx.load128" = load i64, ptr %idx1, align 8
  %cmpeqtmp129 = icmp eq i64 %"%idx.load128", 43
  br i1 %cmpeqtmp129, label %then_116, label %merge_117

merge_117:                                        ; preds = %merge_115
  %"%idx.load131" = load i64, ptr %idx1, align 8
  %cmpeqtmp132 = icmp eq i64 %"%idx.load131", 44
  br i1 %cmpeqtmp132, label %then_118, label %merge_119

merge_119:                                        ; preds = %merge_117
  %"%idx.load134" = load i64, ptr %idx1, align 8
  %cmpeqtmp135 = icmp eq i64 %"%idx.load134", 45
  br i1 %cmpeqtmp135, label %then_120, label %merge_121

merge_121:                                        ; preds = %merge_119
  %"%idx.load137" = load i64, ptr %idx1, align 8
  %cmpeqtmp138 = icmp eq i64 %"%idx.load137", 46
  br i1 %cmpeqtmp138, label %then_122, label %merge_123

merge_123:                                        ; preds = %merge_121
  %"%idx.load140" = load i64, ptr %idx1, align 8
  %cmpeqtmp141 = icmp eq i64 %"%idx.load140", 47
  br i1 %cmpeqtmp141, label %then_124, label %merge_125

merge_125:                                        ; preds = %merge_123
  %"%idx.load143" = load i64, ptr %idx1, align 8
  %cmpeqtmp144 = icmp eq i64 %"%idx.load143", 48
  br i1 %cmpeqtmp144, label %then_126, label %merge_127

merge_127:                                        ; preds = %merge_125
  %"%idx.load146" = load i64, ptr %idx1, align 8
  %cmpeqtmp147 = icmp eq i64 %"%idx.load146", 49
  br i1 %cmpeqtmp147, label %then_128, label %merge_129

merge_129:                                        ; preds = %merge_127
  %"%idx.load149" = load i64, ptr %idx1, align 8
  %cmpeqtmp150 = icmp eq i64 %"%idx.load149", 50
  br i1 %cmpeqtmp150, label %then_130, label %merge_131

merge_131:                                        ; preds = %merge_129
  %"%idx.load152" = load i64, ptr %idx1, align 8
  %cmpeqtmp153 = icmp eq i64 %"%idx.load152", 51
  br i1 %cmpeqtmp153, label %then_132, label %merge_133

merge_133:                                        ; preds = %merge_131
  %"%idx.load155" = load i64, ptr %idx1, align 8
  %cmpeqtmp156 = icmp eq i64 %"%idx.load155", 52
  br i1 %cmpeqtmp156, label %then_134, label %merge_135

merge_135:                                        ; preds = %merge_133
  %"%idx.load158" = load i64, ptr %idx1, align 8
  %cmpeqtmp159 = icmp eq i64 %"%idx.load158", 53
  br i1 %cmpeqtmp159, label %then_136, label %merge_137

merge_137:                                        ; preds = %merge_135
  %"%idx.load161" = load i64, ptr %idx1, align 8
  %cmpeqtmp162 = icmp eq i64 %"%idx.load161", 54
  br i1 %cmpeqtmp162, label %then_138, label %merge_139

merge_139:                                        ; preds = %merge_137
  %"%idx.load164" = load i64, ptr %idx1, align 8
  %cmpeqtmp165 = icmp eq i64 %"%idx.load164", 55
  br i1 %cmpeqtmp165, label %then_140, label %merge_141

merge_141:                                        ; preds = %merge_139
  %"%idx.load167" = load i64, ptr %idx1, align 8
  %cmpeqtmp168 = icmp eq i64 %"%idx.load167", 56
  br i1 %cmpeqtmp168, label %then_142, label %merge_143

merge_143:                                        ; preds = %merge_141
  %"%idx.load170" = load i64, ptr %idx1, align 8
  %cmpeqtmp171 = icmp eq i64 %"%idx.load170", 57
  br i1 %cmpeqtmp171, label %then_144, label %merge_145

merge_145:                                        ; preds = %merge_143
  %"%idx.load173" = load i64, ptr %idx1, align 8
  %cmpeqtmp174 = icmp eq i64 %"%idx.load173", 58
  br i1 %cmpeqtmp174, label %then_146, label %merge_147

merge_147:                                        ; preds = %merge_145
  %"%idx.load176" = load i64, ptr %idx1, align 8
  %cmpeqtmp177 = icmp eq i64 %"%idx.load176", 59
  br i1 %cmpeqtmp177, label %then_148, label %merge_149

merge_149:                                        ; preds = %merge_147
  %"%idx.load179" = load i64, ptr %idx1, align 8
  %cmpeqtmp180 = icmp eq i64 %"%idx.load179", 60
  br i1 %cmpeqtmp180, label %then_150, label %merge_151

merge_151:                                        ; preds = %merge_149
  %"%idx.load182" = load i64, ptr %idx1, align 8
  %cmpeqtmp183 = icmp eq i64 %"%idx.load182", 61
  br i1 %cmpeqtmp183, label %then_152, label %merge_153

merge_153:                                        ; preds = %merge_151
  %"%idx.load185" = load i64, ptr %idx1, align 8
  %cmpeqtmp186 = icmp eq i64 %"%idx.load185", 62
  br i1 %cmpeqtmp186, label %then_154, label %merge_155

merge_155:                                        ; preds = %merge_153
  %"%idx.load188" = load i64, ptr %idx1, align 8
  %cmpeqtmp189 = icmp eq i64 %"%idx.load188", 63
  br i1 %cmpeqtmp189, label %then_156, label %merge_157

merge_157:                                        ; preds = %merge_155
  ret i64 0

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
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 8
  br i1 %cmpeqtmp24, label %then_46, label %merge_47

merge_47:                                         ; preds = %merge_45
  %"%idx.load26" = load i64, ptr %idx1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%idx.load26", 9
  br i1 %cmpeqtmp27, label %then_48, label %merge_49

merge_49:                                         ; preds = %merge_47
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 10
  br i1 %cmpeqtmp30, label %then_50, label %merge_51

merge_51:                                         ; preds = %merge_49
  %"%idx.load32" = load i64, ptr %idx1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%idx.load32", 11
  br i1 %cmpeqtmp33, label %then_52, label %merge_53

merge_53:                                         ; preds = %merge_51
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 12
  br i1 %cmpeqtmp36, label %then_54, label %merge_55

merge_55:                                         ; preds = %merge_53
  %"%idx.load38" = load i64, ptr %idx1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%idx.load38", 13
  br i1 %cmpeqtmp39, label %then_56, label %merge_57

merge_57:                                         ; preds = %merge_55
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 14
  br i1 %cmpeqtmp42, label %then_58, label %merge_59

merge_59:                                         ; preds = %merge_57
  %"%idx.load44" = load i64, ptr %idx1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%idx.load44", 15
  br i1 %cmpeqtmp45, label %then_60, label %merge_61

merge_61:                                         ; preds = %merge_59
  %"%idx.load47" = load i64, ptr %idx1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%idx.load47", 16
  br i1 %cmpeqtmp48, label %then_62, label %merge_63

merge_63:                                         ; preds = %merge_61
  %"%idx.load50" = load i64, ptr %idx1, align 8
  %cmpeqtmp51 = icmp eq i64 %"%idx.load50", 17
  br i1 %cmpeqtmp51, label %then_64, label %merge_65

merge_65:                                         ; preds = %merge_63
  %"%idx.load53" = load i64, ptr %idx1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%idx.load53", 18
  br i1 %cmpeqtmp54, label %then_66, label %merge_67

merge_67:                                         ; preds = %merge_65
  %"%idx.load56" = load i64, ptr %idx1, align 8
  %cmpeqtmp57 = icmp eq i64 %"%idx.load56", 19
  br i1 %cmpeqtmp57, label %then_68, label %merge_69

merge_69:                                         ; preds = %merge_67
  %"%idx.load59" = load i64, ptr %idx1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%idx.load59", 20
  br i1 %cmpeqtmp60, label %then_70, label %merge_71

merge_71:                                         ; preds = %merge_69
  %"%idx.load62" = load i64, ptr %idx1, align 8
  %cmpeqtmp63 = icmp eq i64 %"%idx.load62", 21
  br i1 %cmpeqtmp63, label %then_72, label %merge_73

merge_73:                                         ; preds = %merge_71
  %"%idx.load65" = load i64, ptr %idx1, align 8
  %cmpeqtmp66 = icmp eq i64 %"%idx.load65", 22
  br i1 %cmpeqtmp66, label %then_74, label %merge_75

merge_75:                                         ; preds = %merge_73
  %"%idx.load68" = load i64, ptr %idx1, align 8
  %cmpeqtmp69 = icmp eq i64 %"%idx.load68", 23
  br i1 %cmpeqtmp69, label %then_76, label %merge_77

merge_77:                                         ; preds = %merge_75
  %"%idx.load71" = load i64, ptr %idx1, align 8
  %cmpeqtmp72 = icmp eq i64 %"%idx.load71", 24
  br i1 %cmpeqtmp72, label %then_78, label %merge_79

merge_79:                                         ; preds = %merge_77
  %"%idx.load74" = load i64, ptr %idx1, align 8
  %cmpeqtmp75 = icmp eq i64 %"%idx.load74", 25
  br i1 %cmpeqtmp75, label %then_80, label %merge_81

merge_81:                                         ; preds = %merge_79
  %"%idx.load77" = load i64, ptr %idx1, align 8
  %cmpeqtmp78 = icmp eq i64 %"%idx.load77", 26
  br i1 %cmpeqtmp78, label %then_82, label %merge_83

merge_83:                                         ; preds = %merge_81
  %"%idx.load80" = load i64, ptr %idx1, align 8
  %cmpeqtmp81 = icmp eq i64 %"%idx.load80", 27
  br i1 %cmpeqtmp81, label %then_84, label %merge_85

merge_85:                                         ; preds = %merge_83
  %"%idx.load83" = load i64, ptr %idx1, align 8
  %cmpeqtmp84 = icmp eq i64 %"%idx.load83", 28
  br i1 %cmpeqtmp84, label %then_86, label %merge_87

merge_87:                                         ; preds = %merge_85
  %"%idx.load86" = load i64, ptr %idx1, align 8
  %cmpeqtmp87 = icmp eq i64 %"%idx.load86", 29
  br i1 %cmpeqtmp87, label %then_88, label %merge_89

merge_89:                                         ; preds = %merge_87
  %"%idx.load89" = load i64, ptr %idx1, align 8
  %cmpeqtmp90 = icmp eq i64 %"%idx.load89", 30
  br i1 %cmpeqtmp90, label %then_90, label %merge_91

merge_91:                                         ; preds = %merge_89
  %"%idx.load92" = load i64, ptr %idx1, align 8
  %cmpeqtmp93 = icmp eq i64 %"%idx.load92", 31
  br i1 %cmpeqtmp93, label %then_92, label %merge_93

merge_93:                                         ; preds = %merge_91
  %"%idx.load95" = load i64, ptr %idx1, align 8
  %cmpeqtmp96 = icmp eq i64 %"%idx.load95", 32
  br i1 %cmpeqtmp96, label %then_94, label %merge_95

merge_95:                                         ; preds = %merge_93
  %"%idx.load98" = load i64, ptr %idx1, align 8
  %cmpeqtmp99 = icmp eq i64 %"%idx.load98", 33
  br i1 %cmpeqtmp99, label %then_96, label %merge_97

merge_97:                                         ; preds = %merge_95
  %"%idx.load101" = load i64, ptr %idx1, align 8
  %cmpeqtmp102 = icmp eq i64 %"%idx.load101", 34
  br i1 %cmpeqtmp102, label %then_98, label %merge_99

merge_99:                                         ; preds = %merge_97
  %"%idx.load104" = load i64, ptr %idx1, align 8
  %cmpeqtmp105 = icmp eq i64 %"%idx.load104", 35
  br i1 %cmpeqtmp105, label %then_100, label %merge_101

then_100:                                         ; preds = %merge_99
  %loadtmp106 = load i64, ptr @pci_loc_35, align 8
  ret i64 %loadtmp106

then_102:                                         ; preds = %merge_101
  %loadtmp109 = load i64, ptr @pci_loc_36, align 8
  ret i64 %loadtmp109

then_104:                                         ; preds = %merge_103
  %loadtmp112 = load i64, ptr @pci_loc_37, align 8
  ret i64 %loadtmp112

then_106:                                         ; preds = %merge_105
  %loadtmp115 = load i64, ptr @pci_loc_38, align 8
  ret i64 %loadtmp115

then_108:                                         ; preds = %merge_107
  %loadtmp118 = load i64, ptr @pci_loc_39, align 8
  ret i64 %loadtmp118

then_110:                                         ; preds = %merge_109
  %loadtmp121 = load i64, ptr @pci_loc_40, align 8
  ret i64 %loadtmp121

then_112:                                         ; preds = %merge_111
  %loadtmp124 = load i64, ptr @pci_loc_41, align 8
  ret i64 %loadtmp124

then_114:                                         ; preds = %merge_113
  %loadtmp127 = load i64, ptr @pci_loc_42, align 8
  ret i64 %loadtmp127

then_116:                                         ; preds = %merge_115
  %loadtmp130 = load i64, ptr @pci_loc_43, align 8
  ret i64 %loadtmp130

then_118:                                         ; preds = %merge_117
  %loadtmp133 = load i64, ptr @pci_loc_44, align 8
  ret i64 %loadtmp133

then_120:                                         ; preds = %merge_119
  %loadtmp136 = load i64, ptr @pci_loc_45, align 8
  ret i64 %loadtmp136

then_122:                                         ; preds = %merge_121
  %loadtmp139 = load i64, ptr @pci_loc_46, align 8
  ret i64 %loadtmp139

then_124:                                         ; preds = %merge_123
  %loadtmp142 = load i64, ptr @pci_loc_47, align 8
  ret i64 %loadtmp142

then_126:                                         ; preds = %merge_125
  %loadtmp145 = load i64, ptr @pci_loc_48, align 8
  ret i64 %loadtmp145

then_128:                                         ; preds = %merge_127
  %loadtmp148 = load i64, ptr @pci_loc_49, align 8
  ret i64 %loadtmp148

then_130:                                         ; preds = %merge_129
  %loadtmp151 = load i64, ptr @pci_loc_50, align 8
  ret i64 %loadtmp151

then_132:                                         ; preds = %merge_131
  %loadtmp154 = load i64, ptr @pci_loc_51, align 8
  ret i64 %loadtmp154

then_134:                                         ; preds = %merge_133
  %loadtmp157 = load i64, ptr @pci_loc_52, align 8
  ret i64 %loadtmp157

then_136:                                         ; preds = %merge_135
  %loadtmp160 = load i64, ptr @pci_loc_53, align 8
  ret i64 %loadtmp160

then_138:                                         ; preds = %merge_137
  %loadtmp163 = load i64, ptr @pci_loc_54, align 8
  ret i64 %loadtmp163

then_140:                                         ; preds = %merge_139
  %loadtmp166 = load i64, ptr @pci_loc_55, align 8
  ret i64 %loadtmp166

then_142:                                         ; preds = %merge_141
  %loadtmp169 = load i64, ptr @pci_loc_56, align 8
  ret i64 %loadtmp169

then_144:                                         ; preds = %merge_143
  %loadtmp172 = load i64, ptr @pci_loc_57, align 8
  ret i64 %loadtmp172

then_146:                                         ; preds = %merge_145
  %loadtmp175 = load i64, ptr @pci_loc_58, align 8
  ret i64 %loadtmp175

then_148:                                         ; preds = %merge_147
  %loadtmp178 = load i64, ptr @pci_loc_59, align 8
  ret i64 %loadtmp178

then_150:                                         ; preds = %merge_149
  %loadtmp181 = load i64, ptr @pci_loc_60, align 8
  ret i64 %loadtmp181

then_152:                                         ; preds = %merge_151
  %loadtmp184 = load i64, ptr @pci_loc_61, align 8
  ret i64 %loadtmp184

then_154:                                         ; preds = %merge_153
  %loadtmp187 = load i64, ptr @pci_loc_62, align 8
  ret i64 %loadtmp187

then_156:                                         ; preds = %merge_155
  %loadtmp190 = load i64, ptr @pci_loc_63, align 8
  ret i64 %loadtmp190

then_30:                                          ; preds = %entry
  %loadtmp = load i64, ptr @pci_loc_0, align 8
  ret i64 %loadtmp

then_32:                                          ; preds = %merge_31
  %loadtmp4 = load i64, ptr @pci_loc_1, align 8
  ret i64 %loadtmp4

then_34:                                          ; preds = %merge_33
  %loadtmp7 = load i64, ptr @pci_loc_2, align 8
  ret i64 %loadtmp7

then_36:                                          ; preds = %merge_35
  %loadtmp10 = load i64, ptr @pci_loc_3, align 8
  ret i64 %loadtmp10

then_38:                                          ; preds = %merge_37
  %loadtmp13 = load i64, ptr @pci_loc_4, align 8
  ret i64 %loadtmp13

then_40:                                          ; preds = %merge_39
  %loadtmp16 = load i64, ptr @pci_loc_5, align 8
  ret i64 %loadtmp16

then_42:                                          ; preds = %merge_41
  %loadtmp19 = load i64, ptr @pci_loc_6, align 8
  ret i64 %loadtmp19

then_44:                                          ; preds = %merge_43
  %loadtmp22 = load i64, ptr @pci_loc_7, align 8
  ret i64 %loadtmp22

then_46:                                          ; preds = %merge_45
  %loadtmp25 = load i64, ptr @pci_loc_8, align 8
  ret i64 %loadtmp25

then_48:                                          ; preds = %merge_47
  %loadtmp28 = load i64, ptr @pci_loc_9, align 8
  ret i64 %loadtmp28

then_50:                                          ; preds = %merge_49
  %loadtmp31 = load i64, ptr @pci_loc_10, align 8
  ret i64 %loadtmp31

then_52:                                          ; preds = %merge_51
  %loadtmp34 = load i64, ptr @pci_loc_11, align 8
  ret i64 %loadtmp34

then_54:                                          ; preds = %merge_53
  %loadtmp37 = load i64, ptr @pci_loc_12, align 8
  ret i64 %loadtmp37

then_56:                                          ; preds = %merge_55
  %loadtmp40 = load i64, ptr @pci_loc_13, align 8
  ret i64 %loadtmp40

then_58:                                          ; preds = %merge_57
  %loadtmp43 = load i64, ptr @pci_loc_14, align 8
  ret i64 %loadtmp43

then_60:                                          ; preds = %merge_59
  %loadtmp46 = load i64, ptr @pci_loc_15, align 8
  ret i64 %loadtmp46

then_62:                                          ; preds = %merge_61
  %loadtmp49 = load i64, ptr @pci_loc_16, align 8
  ret i64 %loadtmp49

then_64:                                          ; preds = %merge_63
  %loadtmp52 = load i64, ptr @pci_loc_17, align 8
  ret i64 %loadtmp52

then_66:                                          ; preds = %merge_65
  %loadtmp55 = load i64, ptr @pci_loc_18, align 8
  ret i64 %loadtmp55

then_68:                                          ; preds = %merge_67
  %loadtmp58 = load i64, ptr @pci_loc_19, align 8
  ret i64 %loadtmp58

then_70:                                          ; preds = %merge_69
  %loadtmp61 = load i64, ptr @pci_loc_20, align 8
  ret i64 %loadtmp61

then_72:                                          ; preds = %merge_71
  %loadtmp64 = load i64, ptr @pci_loc_21, align 8
  ret i64 %loadtmp64

then_74:                                          ; preds = %merge_73
  %loadtmp67 = load i64, ptr @pci_loc_22, align 8
  ret i64 %loadtmp67

then_76:                                          ; preds = %merge_75
  %loadtmp70 = load i64, ptr @pci_loc_23, align 8
  ret i64 %loadtmp70

then_78:                                          ; preds = %merge_77
  %loadtmp73 = load i64, ptr @pci_loc_24, align 8
  ret i64 %loadtmp73

then_80:                                          ; preds = %merge_79
  %loadtmp76 = load i64, ptr @pci_loc_25, align 8
  ret i64 %loadtmp76

then_82:                                          ; preds = %merge_81
  %loadtmp79 = load i64, ptr @pci_loc_26, align 8
  ret i64 %loadtmp79

then_84:                                          ; preds = %merge_83
  %loadtmp82 = load i64, ptr @pci_loc_27, align 8
  ret i64 %loadtmp82

then_86:                                          ; preds = %merge_85
  %loadtmp85 = load i64, ptr @pci_loc_28, align 8
  ret i64 %loadtmp85

then_88:                                          ; preds = %merge_87
  %loadtmp88 = load i64, ptr @pci_loc_29, align 8
  ret i64 %loadtmp88

then_90:                                          ; preds = %merge_89
  %loadtmp91 = load i64, ptr @pci_loc_30, align 8
  ret i64 %loadtmp91

then_92:                                          ; preds = %merge_91
  %loadtmp94 = load i64, ptr @pci_loc_31, align 8
  ret i64 %loadtmp94

then_94:                                          ; preds = %merge_93
  %loadtmp97 = load i64, ptr @pci_loc_32, align 8
  ret i64 %loadtmp97

then_96:                                          ; preds = %merge_95
  %loadtmp100 = load i64, ptr @pci_loc_33, align 8
  ret i64 %loadtmp100

then_98:                                          ; preds = %merge_97
  %loadtmp103 = load i64, ptr @pci_loc_34, align 8
  ret i64 %loadtmp103
}

define i64 @pci_set_loc(i64 %idx, i64 %val) {
entry:
  %"%pci_loc_63" = alloca i64, align 8
  %"%pci_loc_62" = alloca i64, align 8
  %"%pci_loc_61" = alloca i64, align 8
  %"%pci_loc_60" = alloca i64, align 8
  %"%pci_loc_59" = alloca i64, align 8
  %"%pci_loc_58" = alloca i64, align 8
  %"%pci_loc_57" = alloca i64, align 8
  %"%pci_loc_56" = alloca i64, align 8
  %"%pci_loc_55" = alloca i64, align 8
  %"%pci_loc_54" = alloca i64, align 8
  %"%pci_loc_53" = alloca i64, align 8
  %"%pci_loc_52" = alloca i64, align 8
  %"%pci_loc_51" = alloca i64, align 8
  %"%pci_loc_50" = alloca i64, align 8
  %"%pci_loc_49" = alloca i64, align 8
  %"%pci_loc_48" = alloca i64, align 8
  %"%pci_loc_47" = alloca i64, align 8
  %"%pci_loc_46" = alloca i64, align 8
  %"%pci_loc_45" = alloca i64, align 8
  %"%pci_loc_44" = alloca i64, align 8
  %"%pci_loc_43" = alloca i64, align 8
  %"%pci_loc_42" = alloca i64, align 8
  %"%pci_loc_41" = alloca i64, align 8
  %"%pci_loc_40" = alloca i64, align 8
  %"%pci_loc_39" = alloca i64, align 8
  %"%pci_loc_38" = alloca i64, align 8
  %"%pci_loc_37" = alloca i64, align 8
  %"%pci_loc_36" = alloca i64, align 8
  %"%pci_loc_35" = alloca i64, align 8
  %"%pci_loc_34" = alloca i64, align 8
  %"%pci_loc_33" = alloca i64, align 8
  %"%pci_loc_32" = alloca i64, align 8
  %"%pci_loc_31" = alloca i64, align 8
  %"%pci_loc_30" = alloca i64, align 8
  %"%pci_loc_29" = alloca i64, align 8
  %"%pci_loc_28" = alloca i64, align 8
  %"%pci_loc_27" = alloca i64, align 8
  %"%pci_loc_26" = alloca i64, align 8
  %"%pci_loc_25" = alloca i64, align 8
  %"%pci_loc_24" = alloca i64, align 8
  %"%pci_loc_23" = alloca i64, align 8
  %"%pci_loc_22" = alloca i64, align 8
  %"%pci_loc_21" = alloca i64, align 8
  %"%pci_loc_20" = alloca i64, align 8
  %"%pci_loc_19" = alloca i64, align 8
  %"%pci_loc_18" = alloca i64, align 8
  %"%pci_loc_17" = alloca i64, align 8
  %"%pci_loc_16" = alloca i64, align 8
  %"%pci_loc_15" = alloca i64, align 8
  %"%pci_loc_14" = alloca i64, align 8
  %"%pci_loc_13" = alloca i64, align 8
  %"%pci_loc_12" = alloca i64, align 8
  %"%pci_loc_11" = alloca i64, align 8
  %"%pci_loc_10" = alloca i64, align 8
  %"%pci_loc_9" = alloca i64, align 8
  %"%pci_loc_8" = alloca i64, align 8
  %"%pci_loc_7" = alloca i64, align 8
  %"%pci_loc_6" = alloca i64, align 8
  %"%pci_loc_5" = alloca i64, align 8
  %"%pci_loc_4" = alloca i64, align 8
  %"%pci_loc_3" = alloca i64, align 8
  %"%pci_loc_2" = alloca i64, align 8
  %"%pci_loc_1" = alloca i64, align 8
  %"%pci_loc_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_158, label %merge_159

merge_159:                                        ; preds = %then_158, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_160, label %merge_161

merge_161:                                        ; preds = %then_160, %merge_159
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_162, label %merge_163

merge_163:                                        ; preds = %then_162, %merge_161
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_164, label %merge_165

merge_165:                                        ; preds = %then_164, %merge_163
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_166, label %merge_167

merge_167:                                        ; preds = %then_166, %merge_165
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_168, label %merge_169

merge_169:                                        ; preds = %then_168, %merge_167
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_170, label %merge_171

merge_171:                                        ; preds = %then_170, %merge_169
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_172, label %merge_173

merge_173:                                        ; preds = %then_172, %merge_171
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_174, label %merge_175

merge_175:                                        ; preds = %then_174, %merge_173
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_176, label %merge_177

merge_177:                                        ; preds = %then_176, %merge_175
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_178, label %merge_179

merge_179:                                        ; preds = %then_178, %merge_177
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_180, label %merge_181

merge_181:                                        ; preds = %then_180, %merge_179
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_182, label %merge_183

merge_183:                                        ; preds = %then_182, %merge_181
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_184, label %merge_185

merge_185:                                        ; preds = %then_184, %merge_183
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_186, label %merge_187

merge_187:                                        ; preds = %then_186, %merge_185
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_188, label %merge_189

merge_189:                                        ; preds = %then_188, %merge_187
  %"%idx.load33" = load i64, ptr %idx1, align 8
  %cmpeqtmp34 = icmp eq i64 %"%idx.load33", 16
  br i1 %cmpeqtmp34, label %then_190, label %merge_191

merge_191:                                        ; preds = %then_190, %merge_189
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 17
  br i1 %cmpeqtmp36, label %then_192, label %merge_193

merge_193:                                        ; preds = %then_192, %merge_191
  %"%idx.load37" = load i64, ptr %idx1, align 8
  %cmpeqtmp38 = icmp eq i64 %"%idx.load37", 18
  br i1 %cmpeqtmp38, label %then_194, label %merge_195

merge_195:                                        ; preds = %then_194, %merge_193
  %"%idx.load39" = load i64, ptr %idx1, align 8
  %cmpeqtmp40 = icmp eq i64 %"%idx.load39", 19
  br i1 %cmpeqtmp40, label %then_196, label %merge_197

merge_197:                                        ; preds = %then_196, %merge_195
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 20
  br i1 %cmpeqtmp42, label %then_198, label %merge_199

merge_199:                                        ; preds = %then_198, %merge_197
  %"%idx.load43" = load i64, ptr %idx1, align 8
  %cmpeqtmp44 = icmp eq i64 %"%idx.load43", 21
  br i1 %cmpeqtmp44, label %then_200, label %merge_201

merge_201:                                        ; preds = %then_200, %merge_199
  %"%idx.load45" = load i64, ptr %idx1, align 8
  %cmpeqtmp46 = icmp eq i64 %"%idx.load45", 22
  br i1 %cmpeqtmp46, label %then_202, label %merge_203

merge_203:                                        ; preds = %then_202, %merge_201
  %"%idx.load47" = load i64, ptr %idx1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%idx.load47", 23
  br i1 %cmpeqtmp48, label %then_204, label %merge_205

merge_205:                                        ; preds = %then_204, %merge_203
  %"%idx.load49" = load i64, ptr %idx1, align 8
  %cmpeqtmp50 = icmp eq i64 %"%idx.load49", 24
  br i1 %cmpeqtmp50, label %then_206, label %merge_207

merge_207:                                        ; preds = %then_206, %merge_205
  %"%idx.load51" = load i64, ptr %idx1, align 8
  %cmpeqtmp52 = icmp eq i64 %"%idx.load51", 25
  br i1 %cmpeqtmp52, label %then_208, label %merge_209

merge_209:                                        ; preds = %then_208, %merge_207
  %"%idx.load53" = load i64, ptr %idx1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%idx.load53", 26
  br i1 %cmpeqtmp54, label %then_210, label %merge_211

merge_211:                                        ; preds = %then_210, %merge_209
  %"%idx.load55" = load i64, ptr %idx1, align 8
  %cmpeqtmp56 = icmp eq i64 %"%idx.load55", 27
  br i1 %cmpeqtmp56, label %then_212, label %merge_213

merge_213:                                        ; preds = %then_212, %merge_211
  %"%idx.load57" = load i64, ptr %idx1, align 8
  %cmpeqtmp58 = icmp eq i64 %"%idx.load57", 28
  br i1 %cmpeqtmp58, label %then_214, label %merge_215

merge_215:                                        ; preds = %then_214, %merge_213
  %"%idx.load59" = load i64, ptr %idx1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%idx.load59", 29
  br i1 %cmpeqtmp60, label %then_216, label %merge_217

merge_217:                                        ; preds = %then_216, %merge_215
  %"%idx.load61" = load i64, ptr %idx1, align 8
  %cmpeqtmp62 = icmp eq i64 %"%idx.load61", 30
  br i1 %cmpeqtmp62, label %then_218, label %merge_219

merge_219:                                        ; preds = %then_218, %merge_217
  %"%idx.load63" = load i64, ptr %idx1, align 8
  %cmpeqtmp64 = icmp eq i64 %"%idx.load63", 31
  br i1 %cmpeqtmp64, label %then_220, label %merge_221

merge_221:                                        ; preds = %then_220, %merge_219
  %"%idx.load65" = load i64, ptr %idx1, align 8
  %cmpeqtmp66 = icmp eq i64 %"%idx.load65", 32
  br i1 %cmpeqtmp66, label %then_222, label %merge_223

merge_223:                                        ; preds = %then_222, %merge_221
  %"%idx.load67" = load i64, ptr %idx1, align 8
  %cmpeqtmp68 = icmp eq i64 %"%idx.load67", 33
  br i1 %cmpeqtmp68, label %then_224, label %merge_225

merge_225:                                        ; preds = %then_224, %merge_223
  %"%idx.load69" = load i64, ptr %idx1, align 8
  %cmpeqtmp70 = icmp eq i64 %"%idx.load69", 34
  br i1 %cmpeqtmp70, label %then_226, label %merge_227

merge_227:                                        ; preds = %then_226, %merge_225
  %"%idx.load71" = load i64, ptr %idx1, align 8
  %cmpeqtmp72 = icmp eq i64 %"%idx.load71", 35
  br i1 %cmpeqtmp72, label %then_228, label %merge_229

merge_229:                                        ; preds = %then_228, %merge_227
  %"%idx.load73" = load i64, ptr %idx1, align 8
  %cmpeqtmp74 = icmp eq i64 %"%idx.load73", 36
  br i1 %cmpeqtmp74, label %then_230, label %merge_231

merge_231:                                        ; preds = %then_230, %merge_229
  %"%idx.load75" = load i64, ptr %idx1, align 8
  %cmpeqtmp76 = icmp eq i64 %"%idx.load75", 37
  br i1 %cmpeqtmp76, label %then_232, label %merge_233

merge_233:                                        ; preds = %then_232, %merge_231
  %"%idx.load77" = load i64, ptr %idx1, align 8
  %cmpeqtmp78 = icmp eq i64 %"%idx.load77", 38
  br i1 %cmpeqtmp78, label %then_234, label %merge_235

merge_235:                                        ; preds = %then_234, %merge_233
  %"%idx.load79" = load i64, ptr %idx1, align 8
  %cmpeqtmp80 = icmp eq i64 %"%idx.load79", 39
  br i1 %cmpeqtmp80, label %then_236, label %merge_237

merge_237:                                        ; preds = %then_236, %merge_235
  %"%idx.load81" = load i64, ptr %idx1, align 8
  %cmpeqtmp82 = icmp eq i64 %"%idx.load81", 40
  br i1 %cmpeqtmp82, label %then_238, label %merge_239

merge_239:                                        ; preds = %then_238, %merge_237
  %"%idx.load83" = load i64, ptr %idx1, align 8
  %cmpeqtmp84 = icmp eq i64 %"%idx.load83", 41
  br i1 %cmpeqtmp84, label %then_240, label %merge_241

merge_241:                                        ; preds = %then_240, %merge_239
  %"%idx.load85" = load i64, ptr %idx1, align 8
  %cmpeqtmp86 = icmp eq i64 %"%idx.load85", 42
  br i1 %cmpeqtmp86, label %then_242, label %merge_243

merge_243:                                        ; preds = %then_242, %merge_241
  %"%idx.load87" = load i64, ptr %idx1, align 8
  %cmpeqtmp88 = icmp eq i64 %"%idx.load87", 43
  br i1 %cmpeqtmp88, label %then_244, label %merge_245

merge_245:                                        ; preds = %then_244, %merge_243
  %"%idx.load89" = load i64, ptr %idx1, align 8
  %cmpeqtmp90 = icmp eq i64 %"%idx.load89", 44
  br i1 %cmpeqtmp90, label %then_246, label %merge_247

merge_247:                                        ; preds = %then_246, %merge_245
  %"%idx.load91" = load i64, ptr %idx1, align 8
  %cmpeqtmp92 = icmp eq i64 %"%idx.load91", 45
  br i1 %cmpeqtmp92, label %then_248, label %merge_249

merge_249:                                        ; preds = %then_248, %merge_247
  %"%idx.load93" = load i64, ptr %idx1, align 8
  %cmpeqtmp94 = icmp eq i64 %"%idx.load93", 46
  br i1 %cmpeqtmp94, label %then_250, label %merge_251

merge_251:                                        ; preds = %then_250, %merge_249
  %"%idx.load95" = load i64, ptr %idx1, align 8
  %cmpeqtmp96 = icmp eq i64 %"%idx.load95", 47
  br i1 %cmpeqtmp96, label %then_252, label %merge_253

merge_253:                                        ; preds = %then_252, %merge_251
  %"%idx.load97" = load i64, ptr %idx1, align 8
  %cmpeqtmp98 = icmp eq i64 %"%idx.load97", 48
  br i1 %cmpeqtmp98, label %then_254, label %merge_255

merge_255:                                        ; preds = %then_254, %merge_253
  %"%idx.load99" = load i64, ptr %idx1, align 8
  %cmpeqtmp100 = icmp eq i64 %"%idx.load99", 49
  br i1 %cmpeqtmp100, label %then_256, label %merge_257

merge_257:                                        ; preds = %then_256, %merge_255
  %"%idx.load101" = load i64, ptr %idx1, align 8
  %cmpeqtmp102 = icmp eq i64 %"%idx.load101", 50
  br i1 %cmpeqtmp102, label %then_258, label %merge_259

merge_259:                                        ; preds = %then_258, %merge_257
  %"%idx.load103" = load i64, ptr %idx1, align 8
  %cmpeqtmp104 = icmp eq i64 %"%idx.load103", 51
  br i1 %cmpeqtmp104, label %then_260, label %merge_261

merge_261:                                        ; preds = %then_260, %merge_259
  %"%idx.load105" = load i64, ptr %idx1, align 8
  %cmpeqtmp106 = icmp eq i64 %"%idx.load105", 52
  br i1 %cmpeqtmp106, label %then_262, label %merge_263

merge_263:                                        ; preds = %then_262, %merge_261
  %"%idx.load107" = load i64, ptr %idx1, align 8
  %cmpeqtmp108 = icmp eq i64 %"%idx.load107", 53
  br i1 %cmpeqtmp108, label %then_264, label %merge_265

merge_265:                                        ; preds = %then_264, %merge_263
  %"%idx.load109" = load i64, ptr %idx1, align 8
  %cmpeqtmp110 = icmp eq i64 %"%idx.load109", 54
  br i1 %cmpeqtmp110, label %then_266, label %merge_267

merge_267:                                        ; preds = %then_266, %merge_265
  %"%idx.load111" = load i64, ptr %idx1, align 8
  %cmpeqtmp112 = icmp eq i64 %"%idx.load111", 55
  br i1 %cmpeqtmp112, label %then_268, label %merge_269

merge_269:                                        ; preds = %then_268, %merge_267
  %"%idx.load113" = load i64, ptr %idx1, align 8
  %cmpeqtmp114 = icmp eq i64 %"%idx.load113", 56
  br i1 %cmpeqtmp114, label %then_270, label %merge_271

merge_271:                                        ; preds = %then_270, %merge_269
  %"%idx.load115" = load i64, ptr %idx1, align 8
  %cmpeqtmp116 = icmp eq i64 %"%idx.load115", 57
  br i1 %cmpeqtmp116, label %then_272, label %merge_273

merge_273:                                        ; preds = %then_272, %merge_271
  %"%idx.load117" = load i64, ptr %idx1, align 8
  %cmpeqtmp118 = icmp eq i64 %"%idx.load117", 58
  br i1 %cmpeqtmp118, label %then_274, label %merge_275

merge_275:                                        ; preds = %then_274, %merge_273
  %"%idx.load119" = load i64, ptr %idx1, align 8
  %cmpeqtmp120 = icmp eq i64 %"%idx.load119", 59
  br i1 %cmpeqtmp120, label %then_276, label %merge_277

merge_277:                                        ; preds = %then_276, %merge_275
  %"%idx.load121" = load i64, ptr %idx1, align 8
  %cmpeqtmp122 = icmp eq i64 %"%idx.load121", 60
  br i1 %cmpeqtmp122, label %then_278, label %merge_279

merge_279:                                        ; preds = %then_278, %merge_277
  %"%idx.load123" = load i64, ptr %idx1, align 8
  %cmpeqtmp124 = icmp eq i64 %"%idx.load123", 61
  br i1 %cmpeqtmp124, label %then_280, label %merge_281

merge_281:                                        ; preds = %then_280, %merge_279
  %"%idx.load125" = load i64, ptr %idx1, align 8
  %cmpeqtmp126 = icmp eq i64 %"%idx.load125", 62
  br i1 %cmpeqtmp126, label %then_282, label %merge_283

merge_283:                                        ; preds = %then_282, %merge_281
  %"%idx.load127" = load i64, ptr %idx1, align 8
  %cmpeqtmp128 = icmp eq i64 %"%idx.load127", 63
  br i1 %cmpeqtmp128, label %then_284, label %merge_285

merge_285:                                        ; preds = %then_284, %merge_283
  ret i64 0

then_158:                                         ; preds = %entry
  %"%446.load" = load i64, ptr %val2, align 8
  store i64 %"%446.load", ptr %"%pci_loc_0", align 8
  br label %merge_159

then_160:                                         ; preds = %merge_159
  %"%450.load" = load i64, ptr %val2, align 8
  store i64 %"%450.load", ptr %"%pci_loc_1", align 8
  br label %merge_161

then_162:                                         ; preds = %merge_161
  %"%454.load" = load i64, ptr %val2, align 8
  store i64 %"%454.load", ptr %"%pci_loc_2", align 8
  br label %merge_163

then_164:                                         ; preds = %merge_163
  %"%458.load" = load i64, ptr %val2, align 8
  store i64 %"%458.load", ptr %"%pci_loc_3", align 8
  br label %merge_165

then_166:                                         ; preds = %merge_165
  %"%462.load" = load i64, ptr %val2, align 8
  store i64 %"%462.load", ptr %"%pci_loc_4", align 8
  br label %merge_167

then_168:                                         ; preds = %merge_167
  %"%466.load" = load i64, ptr %val2, align 8
  store i64 %"%466.load", ptr %"%pci_loc_5", align 8
  br label %merge_169

then_170:                                         ; preds = %merge_169
  %"%470.load" = load i64, ptr %val2, align 8
  store i64 %"%470.load", ptr %"%pci_loc_6", align 8
  br label %merge_171

then_172:                                         ; preds = %merge_171
  %"%474.load" = load i64, ptr %val2, align 8
  store i64 %"%474.load", ptr %"%pci_loc_7", align 8
  br label %merge_173

then_174:                                         ; preds = %merge_173
  %"%478.load" = load i64, ptr %val2, align 8
  store i64 %"%478.load", ptr %"%pci_loc_8", align 8
  br label %merge_175

then_176:                                         ; preds = %merge_175
  %"%482.load" = load i64, ptr %val2, align 8
  store i64 %"%482.load", ptr %"%pci_loc_9", align 8
  br label %merge_177

then_178:                                         ; preds = %merge_177
  %"%486.load" = load i64, ptr %val2, align 8
  store i64 %"%486.load", ptr %"%pci_loc_10", align 8
  br label %merge_179

then_180:                                         ; preds = %merge_179
  %"%490.load" = load i64, ptr %val2, align 8
  store i64 %"%490.load", ptr %"%pci_loc_11", align 8
  br label %merge_181

then_182:                                         ; preds = %merge_181
  %"%494.load" = load i64, ptr %val2, align 8
  store i64 %"%494.load", ptr %"%pci_loc_12", align 8
  br label %merge_183

then_184:                                         ; preds = %merge_183
  %"%498.load" = load i64, ptr %val2, align 8
  store i64 %"%498.load", ptr %"%pci_loc_13", align 8
  br label %merge_185

then_186:                                         ; preds = %merge_185
  %"%502.load" = load i64, ptr %val2, align 8
  store i64 %"%502.load", ptr %"%pci_loc_14", align 8
  br label %merge_187

then_188:                                         ; preds = %merge_187
  %"%506.load" = load i64, ptr %val2, align 8
  store i64 %"%506.load", ptr %"%pci_loc_15", align 8
  br label %merge_189

then_190:                                         ; preds = %merge_189
  %"%510.load" = load i64, ptr %val2, align 8
  store i64 %"%510.load", ptr %"%pci_loc_16", align 8
  br label %merge_191

then_192:                                         ; preds = %merge_191
  %"%514.load" = load i64, ptr %val2, align 8
  store i64 %"%514.load", ptr %"%pci_loc_17", align 8
  br label %merge_193

then_194:                                         ; preds = %merge_193
  %"%518.load" = load i64, ptr %val2, align 8
  store i64 %"%518.load", ptr %"%pci_loc_18", align 8
  br label %merge_195

then_196:                                         ; preds = %merge_195
  %"%522.load" = load i64, ptr %val2, align 8
  store i64 %"%522.load", ptr %"%pci_loc_19", align 8
  br label %merge_197

then_198:                                         ; preds = %merge_197
  %"%526.load" = load i64, ptr %val2, align 8
  store i64 %"%526.load", ptr %"%pci_loc_20", align 8
  br label %merge_199

then_200:                                         ; preds = %merge_199
  %"%530.load" = load i64, ptr %val2, align 8
  store i64 %"%530.load", ptr %"%pci_loc_21", align 8
  br label %merge_201

then_202:                                         ; preds = %merge_201
  %"%534.load" = load i64, ptr %val2, align 8
  store i64 %"%534.load", ptr %"%pci_loc_22", align 8
  br label %merge_203

then_204:                                         ; preds = %merge_203
  %"%538.load" = load i64, ptr %val2, align 8
  store i64 %"%538.load", ptr %"%pci_loc_23", align 8
  br label %merge_205

then_206:                                         ; preds = %merge_205
  %"%542.load" = load i64, ptr %val2, align 8
  store i64 %"%542.load", ptr %"%pci_loc_24", align 8
  br label %merge_207

then_208:                                         ; preds = %merge_207
  %"%546.load" = load i64, ptr %val2, align 8
  store i64 %"%546.load", ptr %"%pci_loc_25", align 8
  br label %merge_209

then_210:                                         ; preds = %merge_209
  %"%550.load" = load i64, ptr %val2, align 8
  store i64 %"%550.load", ptr %"%pci_loc_26", align 8
  br label %merge_211

then_212:                                         ; preds = %merge_211
  %"%554.load" = load i64, ptr %val2, align 8
  store i64 %"%554.load", ptr %"%pci_loc_27", align 8
  br label %merge_213

then_214:                                         ; preds = %merge_213
  %"%558.load" = load i64, ptr %val2, align 8
  store i64 %"%558.load", ptr %"%pci_loc_28", align 8
  br label %merge_215

then_216:                                         ; preds = %merge_215
  %"%562.load" = load i64, ptr %val2, align 8
  store i64 %"%562.load", ptr %"%pci_loc_29", align 8
  br label %merge_217

then_218:                                         ; preds = %merge_217
  %"%566.load" = load i64, ptr %val2, align 8
  store i64 %"%566.load", ptr %"%pci_loc_30", align 8
  br label %merge_219

then_220:                                         ; preds = %merge_219
  %"%570.load" = load i64, ptr %val2, align 8
  store i64 %"%570.load", ptr %"%pci_loc_31", align 8
  br label %merge_221

then_222:                                         ; preds = %merge_221
  %"%574.load" = load i64, ptr %val2, align 8
  store i64 %"%574.load", ptr %"%pci_loc_32", align 8
  br label %merge_223

then_224:                                         ; preds = %merge_223
  %"%578.load" = load i64, ptr %val2, align 8
  store i64 %"%578.load", ptr %"%pci_loc_33", align 8
  br label %merge_225

then_226:                                         ; preds = %merge_225
  %"%582.load" = load i64, ptr %val2, align 8
  store i64 %"%582.load", ptr %"%pci_loc_34", align 8
  br label %merge_227

then_228:                                         ; preds = %merge_227
  %"%586.load" = load i64, ptr %val2, align 8
  store i64 %"%586.load", ptr %"%pci_loc_35", align 8
  br label %merge_229

then_230:                                         ; preds = %merge_229
  %"%590.load" = load i64, ptr %val2, align 8
  store i64 %"%590.load", ptr %"%pci_loc_36", align 8
  br label %merge_231

then_232:                                         ; preds = %merge_231
  %"%594.load" = load i64, ptr %val2, align 8
  store i64 %"%594.load", ptr %"%pci_loc_37", align 8
  br label %merge_233

then_234:                                         ; preds = %merge_233
  %"%598.load" = load i64, ptr %val2, align 8
  store i64 %"%598.load", ptr %"%pci_loc_38", align 8
  br label %merge_235

then_236:                                         ; preds = %merge_235
  %"%602.load" = load i64, ptr %val2, align 8
  store i64 %"%602.load", ptr %"%pci_loc_39", align 8
  br label %merge_237

then_238:                                         ; preds = %merge_237
  %"%606.load" = load i64, ptr %val2, align 8
  store i64 %"%606.load", ptr %"%pci_loc_40", align 8
  br label %merge_239

then_240:                                         ; preds = %merge_239
  %"%610.load" = load i64, ptr %val2, align 8
  store i64 %"%610.load", ptr %"%pci_loc_41", align 8
  br label %merge_241

then_242:                                         ; preds = %merge_241
  %"%614.load" = load i64, ptr %val2, align 8
  store i64 %"%614.load", ptr %"%pci_loc_42", align 8
  br label %merge_243

then_244:                                         ; preds = %merge_243
  %"%618.load" = load i64, ptr %val2, align 8
  store i64 %"%618.load", ptr %"%pci_loc_43", align 8
  br label %merge_245

then_246:                                         ; preds = %merge_245
  %"%622.load" = load i64, ptr %val2, align 8
  store i64 %"%622.load", ptr %"%pci_loc_44", align 8
  br label %merge_247

then_248:                                         ; preds = %merge_247
  %"%626.load" = load i64, ptr %val2, align 8
  store i64 %"%626.load", ptr %"%pci_loc_45", align 8
  br label %merge_249

then_250:                                         ; preds = %merge_249
  %"%630.load" = load i64, ptr %val2, align 8
  store i64 %"%630.load", ptr %"%pci_loc_46", align 8
  br label %merge_251

then_252:                                         ; preds = %merge_251
  %"%634.load" = load i64, ptr %val2, align 8
  store i64 %"%634.load", ptr %"%pci_loc_47", align 8
  br label %merge_253

then_254:                                         ; preds = %merge_253
  %"%638.load" = load i64, ptr %val2, align 8
  store i64 %"%638.load", ptr %"%pci_loc_48", align 8
  br label %merge_255

then_256:                                         ; preds = %merge_255
  %"%642.load" = load i64, ptr %val2, align 8
  store i64 %"%642.load", ptr %"%pci_loc_49", align 8
  br label %merge_257

then_258:                                         ; preds = %merge_257
  %"%646.load" = load i64, ptr %val2, align 8
  store i64 %"%646.load", ptr %"%pci_loc_50", align 8
  br label %merge_259

then_260:                                         ; preds = %merge_259
  %"%650.load" = load i64, ptr %val2, align 8
  store i64 %"%650.load", ptr %"%pci_loc_51", align 8
  br label %merge_261

then_262:                                         ; preds = %merge_261
  %"%654.load" = load i64, ptr %val2, align 8
  store i64 %"%654.load", ptr %"%pci_loc_52", align 8
  br label %merge_263

then_264:                                         ; preds = %merge_263
  %"%658.load" = load i64, ptr %val2, align 8
  store i64 %"%658.load", ptr %"%pci_loc_53", align 8
  br label %merge_265

then_266:                                         ; preds = %merge_265
  %"%662.load" = load i64, ptr %val2, align 8
  store i64 %"%662.load", ptr %"%pci_loc_54", align 8
  br label %merge_267

then_268:                                         ; preds = %merge_267
  %"%666.load" = load i64, ptr %val2, align 8
  store i64 %"%666.load", ptr %"%pci_loc_55", align 8
  br label %merge_269

then_270:                                         ; preds = %merge_269
  %"%670.load" = load i64, ptr %val2, align 8
  store i64 %"%670.load", ptr %"%pci_loc_56", align 8
  br label %merge_271

then_272:                                         ; preds = %merge_271
  %"%674.load" = load i64, ptr %val2, align 8
  store i64 %"%674.load", ptr %"%pci_loc_57", align 8
  br label %merge_273

then_274:                                         ; preds = %merge_273
  %"%678.load" = load i64, ptr %val2, align 8
  store i64 %"%678.load", ptr %"%pci_loc_58", align 8
  br label %merge_275

then_276:                                         ; preds = %merge_275
  %"%682.load" = load i64, ptr %val2, align 8
  store i64 %"%682.load", ptr %"%pci_loc_59", align 8
  br label %merge_277

then_278:                                         ; preds = %merge_277
  %"%686.load" = load i64, ptr %val2, align 8
  store i64 %"%686.load", ptr %"%pci_loc_60", align 8
  br label %merge_279

then_280:                                         ; preds = %merge_279
  %"%690.load" = load i64, ptr %val2, align 8
  store i64 %"%690.load", ptr %"%pci_loc_61", align 8
  br label %merge_281

then_282:                                         ; preds = %merge_281
  %"%694.load" = load i64, ptr %val2, align 8
  store i64 %"%694.load", ptr %"%pci_loc_62", align 8
  br label %merge_283

then_284:                                         ; preds = %merge_283
  %"%698.load" = load i64, ptr %val2, align 8
  store i64 %"%698.load", ptr %"%pci_loc_63", align 8
  br label %merge_285
}

define i64 @pci_get_vid(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_286, label %merge_287

merge_287:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_288, label %merge_289

merge_289:                                        ; preds = %merge_287
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_290, label %merge_291

merge_291:                                        ; preds = %merge_289
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_292, label %merge_293

merge_293:                                        ; preds = %merge_291
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_294, label %merge_295

merge_295:                                        ; preds = %merge_293
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_296, label %merge_297

merge_297:                                        ; preds = %merge_295
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_298, label %merge_299

merge_299:                                        ; preds = %merge_297
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_300, label %merge_301

merge_301:                                        ; preds = %merge_299
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 8
  br i1 %cmpeqtmp24, label %then_302, label %merge_303

merge_303:                                        ; preds = %merge_301
  %"%idx.load26" = load i64, ptr %idx1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%idx.load26", 9
  br i1 %cmpeqtmp27, label %then_304, label %merge_305

merge_305:                                        ; preds = %merge_303
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 10
  br i1 %cmpeqtmp30, label %then_306, label %merge_307

merge_307:                                        ; preds = %merge_305
  %"%idx.load32" = load i64, ptr %idx1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%idx.load32", 11
  br i1 %cmpeqtmp33, label %then_308, label %merge_309

merge_309:                                        ; preds = %merge_307
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 12
  br i1 %cmpeqtmp36, label %then_310, label %merge_311

merge_311:                                        ; preds = %merge_309
  %"%idx.load38" = load i64, ptr %idx1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%idx.load38", 13
  br i1 %cmpeqtmp39, label %then_312, label %merge_313

merge_313:                                        ; preds = %merge_311
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 14
  br i1 %cmpeqtmp42, label %then_314, label %merge_315

merge_315:                                        ; preds = %merge_313
  %"%idx.load44" = load i64, ptr %idx1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%idx.load44", 15
  br i1 %cmpeqtmp45, label %then_316, label %merge_317

merge_317:                                        ; preds = %merge_315
  %"%idx.load47" = load i64, ptr %idx1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%idx.load47", 16
  br i1 %cmpeqtmp48, label %then_318, label %merge_319

merge_319:                                        ; preds = %merge_317
  %"%idx.load50" = load i64, ptr %idx1, align 8
  %cmpeqtmp51 = icmp eq i64 %"%idx.load50", 17
  br i1 %cmpeqtmp51, label %then_320, label %merge_321

merge_321:                                        ; preds = %merge_319
  %"%idx.load53" = load i64, ptr %idx1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%idx.load53", 18
  br i1 %cmpeqtmp54, label %then_322, label %merge_323

merge_323:                                        ; preds = %merge_321
  %"%idx.load56" = load i64, ptr %idx1, align 8
  %cmpeqtmp57 = icmp eq i64 %"%idx.load56", 19
  br i1 %cmpeqtmp57, label %then_324, label %merge_325

merge_325:                                        ; preds = %merge_323
  %"%idx.load59" = load i64, ptr %idx1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%idx.load59", 20
  br i1 %cmpeqtmp60, label %then_326, label %merge_327

merge_327:                                        ; preds = %merge_325
  %"%idx.load62" = load i64, ptr %idx1, align 8
  %cmpeqtmp63 = icmp eq i64 %"%idx.load62", 21
  br i1 %cmpeqtmp63, label %then_328, label %merge_329

merge_329:                                        ; preds = %merge_327
  %"%idx.load65" = load i64, ptr %idx1, align 8
  %cmpeqtmp66 = icmp eq i64 %"%idx.load65", 22
  br i1 %cmpeqtmp66, label %then_330, label %merge_331

merge_331:                                        ; preds = %merge_329
  %"%idx.load68" = load i64, ptr %idx1, align 8
  %cmpeqtmp69 = icmp eq i64 %"%idx.load68", 23
  br i1 %cmpeqtmp69, label %then_332, label %merge_333

merge_333:                                        ; preds = %merge_331
  %"%idx.load71" = load i64, ptr %idx1, align 8
  %cmpeqtmp72 = icmp eq i64 %"%idx.load71", 24
  br i1 %cmpeqtmp72, label %then_334, label %merge_335

merge_335:                                        ; preds = %merge_333
  %"%idx.load74" = load i64, ptr %idx1, align 8
  %cmpeqtmp75 = icmp eq i64 %"%idx.load74", 25
  br i1 %cmpeqtmp75, label %then_336, label %merge_337

merge_337:                                        ; preds = %merge_335
  %"%idx.load77" = load i64, ptr %idx1, align 8
  %cmpeqtmp78 = icmp eq i64 %"%idx.load77", 26
  br i1 %cmpeqtmp78, label %then_338, label %merge_339

merge_339:                                        ; preds = %merge_337
  %"%idx.load80" = load i64, ptr %idx1, align 8
  %cmpeqtmp81 = icmp eq i64 %"%idx.load80", 27
  br i1 %cmpeqtmp81, label %then_340, label %merge_341

merge_341:                                        ; preds = %merge_339
  %"%idx.load83" = load i64, ptr %idx1, align 8
  %cmpeqtmp84 = icmp eq i64 %"%idx.load83", 28
  br i1 %cmpeqtmp84, label %then_342, label %merge_343

merge_343:                                        ; preds = %merge_341
  %"%idx.load86" = load i64, ptr %idx1, align 8
  %cmpeqtmp87 = icmp eq i64 %"%idx.load86", 29
  br i1 %cmpeqtmp87, label %then_344, label %merge_345

merge_345:                                        ; preds = %merge_343
  %"%idx.load89" = load i64, ptr %idx1, align 8
  %cmpeqtmp90 = icmp eq i64 %"%idx.load89", 30
  br i1 %cmpeqtmp90, label %then_346, label %merge_347

merge_347:                                        ; preds = %merge_345
  %"%idx.load92" = load i64, ptr %idx1, align 8
  %cmpeqtmp93 = icmp eq i64 %"%idx.load92", 31
  br i1 %cmpeqtmp93, label %then_348, label %merge_349

merge_349:                                        ; preds = %merge_347
  ret i64 0

then_286:                                         ; preds = %entry
  %loadtmp = load i64, ptr @pci_vid_0, align 8
  ret i64 %loadtmp

then_288:                                         ; preds = %merge_287
  %loadtmp4 = load i64, ptr @pci_vid_1, align 8
  ret i64 %loadtmp4

then_290:                                         ; preds = %merge_289
  %loadtmp7 = load i64, ptr @pci_vid_2, align 8
  ret i64 %loadtmp7

then_292:                                         ; preds = %merge_291
  %loadtmp10 = load i64, ptr @pci_vid_3, align 8
  ret i64 %loadtmp10

then_294:                                         ; preds = %merge_293
  %loadtmp13 = load i64, ptr @pci_vid_4, align 8
  ret i64 %loadtmp13

then_296:                                         ; preds = %merge_295
  %loadtmp16 = load i64, ptr @pci_vid_5, align 8
  ret i64 %loadtmp16

then_298:                                         ; preds = %merge_297
  %loadtmp19 = load i64, ptr @pci_vid_6, align 8
  ret i64 %loadtmp19

then_300:                                         ; preds = %merge_299
  %loadtmp22 = load i64, ptr @pci_vid_7, align 8
  ret i64 %loadtmp22

then_302:                                         ; preds = %merge_301
  %loadtmp25 = load i64, ptr @pci_vid_8, align 8
  ret i64 %loadtmp25

then_304:                                         ; preds = %merge_303
  %loadtmp28 = load i64, ptr @pci_vid_9, align 8
  ret i64 %loadtmp28

then_306:                                         ; preds = %merge_305
  %loadtmp31 = load i64, ptr @pci_vid_10, align 8
  ret i64 %loadtmp31

then_308:                                         ; preds = %merge_307
  %loadtmp34 = load i64, ptr @pci_vid_11, align 8
  ret i64 %loadtmp34

then_310:                                         ; preds = %merge_309
  %loadtmp37 = load i64, ptr @pci_vid_12, align 8
  ret i64 %loadtmp37

then_312:                                         ; preds = %merge_311
  %loadtmp40 = load i64, ptr @pci_vid_13, align 8
  ret i64 %loadtmp40

then_314:                                         ; preds = %merge_313
  %loadtmp43 = load i64, ptr @pci_vid_14, align 8
  ret i64 %loadtmp43

then_316:                                         ; preds = %merge_315
  %loadtmp46 = load i64, ptr @pci_vid_15, align 8
  ret i64 %loadtmp46

then_318:                                         ; preds = %merge_317
  %loadtmp49 = load i64, ptr @pci_vid_16, align 8
  ret i64 %loadtmp49

then_320:                                         ; preds = %merge_319
  %loadtmp52 = load i64, ptr @pci_vid_17, align 8
  ret i64 %loadtmp52

then_322:                                         ; preds = %merge_321
  %loadtmp55 = load i64, ptr @pci_vid_18, align 8
  ret i64 %loadtmp55

then_324:                                         ; preds = %merge_323
  %loadtmp58 = load i64, ptr @pci_vid_19, align 8
  ret i64 %loadtmp58

then_326:                                         ; preds = %merge_325
  %loadtmp61 = load i64, ptr @pci_vid_20, align 8
  ret i64 %loadtmp61

then_328:                                         ; preds = %merge_327
  %loadtmp64 = load i64, ptr @pci_vid_21, align 8
  ret i64 %loadtmp64

then_330:                                         ; preds = %merge_329
  %loadtmp67 = load i64, ptr @pci_vid_22, align 8
  ret i64 %loadtmp67

then_332:                                         ; preds = %merge_331
  %loadtmp70 = load i64, ptr @pci_vid_23, align 8
  ret i64 %loadtmp70

then_334:                                         ; preds = %merge_333
  %loadtmp73 = load i64, ptr @pci_vid_24, align 8
  ret i64 %loadtmp73

then_336:                                         ; preds = %merge_335
  %loadtmp76 = load i64, ptr @pci_vid_25, align 8
  ret i64 %loadtmp76

then_338:                                         ; preds = %merge_337
  %loadtmp79 = load i64, ptr @pci_vid_26, align 8
  ret i64 %loadtmp79

then_340:                                         ; preds = %merge_339
  %loadtmp82 = load i64, ptr @pci_vid_27, align 8
  ret i64 %loadtmp82

then_342:                                         ; preds = %merge_341
  %loadtmp85 = load i64, ptr @pci_vid_28, align 8
  ret i64 %loadtmp85

then_344:                                         ; preds = %merge_343
  %loadtmp88 = load i64, ptr @pci_vid_29, align 8
  ret i64 %loadtmp88

then_346:                                         ; preds = %merge_345
  %loadtmp91 = load i64, ptr @pci_vid_30, align 8
  ret i64 %loadtmp91

then_348:                                         ; preds = %merge_347
  %loadtmp94 = load i64, ptr @pci_vid_31, align 8
  ret i64 %loadtmp94
}

define i64 @pci_set_vid(i64 %idx, i64 %val) {
entry:
  %"%pci_vid_31" = alloca i64, align 8
  %"%pci_vid_30" = alloca i64, align 8
  %"%pci_vid_29" = alloca i64, align 8
  %"%pci_vid_28" = alloca i64, align 8
  %"%pci_vid_27" = alloca i64, align 8
  %"%pci_vid_26" = alloca i64, align 8
  %"%pci_vid_25" = alloca i64, align 8
  %"%pci_vid_24" = alloca i64, align 8
  %"%pci_vid_23" = alloca i64, align 8
  %"%pci_vid_22" = alloca i64, align 8
  %"%pci_vid_21" = alloca i64, align 8
  %"%pci_vid_20" = alloca i64, align 8
  %"%pci_vid_19" = alloca i64, align 8
  %"%pci_vid_18" = alloca i64, align 8
  %"%pci_vid_17" = alloca i64, align 8
  %"%pci_vid_16" = alloca i64, align 8
  %"%pci_vid_15" = alloca i64, align 8
  %"%pci_vid_14" = alloca i64, align 8
  %"%pci_vid_13" = alloca i64, align 8
  %"%pci_vid_12" = alloca i64, align 8
  %"%pci_vid_11" = alloca i64, align 8
  %"%pci_vid_10" = alloca i64, align 8
  %"%pci_vid_9" = alloca i64, align 8
  %"%pci_vid_8" = alloca i64, align 8
  %"%pci_vid_7" = alloca i64, align 8
  %"%pci_vid_6" = alloca i64, align 8
  %"%pci_vid_5" = alloca i64, align 8
  %"%pci_vid_4" = alloca i64, align 8
  %"%pci_vid_3" = alloca i64, align 8
  %"%pci_vid_2" = alloca i64, align 8
  %"%pci_vid_1" = alloca i64, align 8
  %"%pci_vid_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_350, label %merge_351

merge_351:                                        ; preds = %then_350, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_352, label %merge_353

merge_353:                                        ; preds = %then_352, %merge_351
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_354, label %merge_355

merge_355:                                        ; preds = %then_354, %merge_353
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_356, label %merge_357

merge_357:                                        ; preds = %then_356, %merge_355
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_358, label %merge_359

merge_359:                                        ; preds = %then_358, %merge_357
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_360, label %merge_361

merge_361:                                        ; preds = %then_360, %merge_359
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_362, label %merge_363

merge_363:                                        ; preds = %then_362, %merge_361
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_364, label %merge_365

merge_365:                                        ; preds = %then_364, %merge_363
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_366, label %merge_367

merge_367:                                        ; preds = %then_366, %merge_365
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_368, label %merge_369

merge_369:                                        ; preds = %then_368, %merge_367
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_370, label %merge_371

merge_371:                                        ; preds = %then_370, %merge_369
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_372, label %merge_373

merge_373:                                        ; preds = %then_372, %merge_371
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_374, label %merge_375

merge_375:                                        ; preds = %then_374, %merge_373
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_376, label %merge_377

merge_377:                                        ; preds = %then_376, %merge_375
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_378, label %merge_379

merge_379:                                        ; preds = %then_378, %merge_377
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_380, label %merge_381

merge_381:                                        ; preds = %then_380, %merge_379
  %"%idx.load33" = load i64, ptr %idx1, align 8
  %cmpeqtmp34 = icmp eq i64 %"%idx.load33", 16
  br i1 %cmpeqtmp34, label %then_382, label %merge_383

merge_383:                                        ; preds = %then_382, %merge_381
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 17
  br i1 %cmpeqtmp36, label %then_384, label %merge_385

merge_385:                                        ; preds = %then_384, %merge_383
  %"%idx.load37" = load i64, ptr %idx1, align 8
  %cmpeqtmp38 = icmp eq i64 %"%idx.load37", 18
  br i1 %cmpeqtmp38, label %then_386, label %merge_387

merge_387:                                        ; preds = %then_386, %merge_385
  %"%idx.load39" = load i64, ptr %idx1, align 8
  %cmpeqtmp40 = icmp eq i64 %"%idx.load39", 19
  br i1 %cmpeqtmp40, label %then_388, label %merge_389

merge_389:                                        ; preds = %then_388, %merge_387
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 20
  br i1 %cmpeqtmp42, label %then_390, label %merge_391

merge_391:                                        ; preds = %then_390, %merge_389
  %"%idx.load43" = load i64, ptr %idx1, align 8
  %cmpeqtmp44 = icmp eq i64 %"%idx.load43", 21
  br i1 %cmpeqtmp44, label %then_392, label %merge_393

merge_393:                                        ; preds = %then_392, %merge_391
  %"%idx.load45" = load i64, ptr %idx1, align 8
  %cmpeqtmp46 = icmp eq i64 %"%idx.load45", 22
  br i1 %cmpeqtmp46, label %then_394, label %merge_395

merge_395:                                        ; preds = %then_394, %merge_393
  %"%idx.load47" = load i64, ptr %idx1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%idx.load47", 23
  br i1 %cmpeqtmp48, label %then_396, label %merge_397

merge_397:                                        ; preds = %then_396, %merge_395
  %"%idx.load49" = load i64, ptr %idx1, align 8
  %cmpeqtmp50 = icmp eq i64 %"%idx.load49", 24
  br i1 %cmpeqtmp50, label %then_398, label %merge_399

merge_399:                                        ; preds = %then_398, %merge_397
  %"%idx.load51" = load i64, ptr %idx1, align 8
  %cmpeqtmp52 = icmp eq i64 %"%idx.load51", 25
  br i1 %cmpeqtmp52, label %then_400, label %merge_401

merge_401:                                        ; preds = %then_400, %merge_399
  %"%idx.load53" = load i64, ptr %idx1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%idx.load53", 26
  br i1 %cmpeqtmp54, label %then_402, label %merge_403

merge_403:                                        ; preds = %then_402, %merge_401
  %"%idx.load55" = load i64, ptr %idx1, align 8
  %cmpeqtmp56 = icmp eq i64 %"%idx.load55", 27
  br i1 %cmpeqtmp56, label %then_404, label %merge_405

merge_405:                                        ; preds = %then_404, %merge_403
  %"%idx.load57" = load i64, ptr %idx1, align 8
  %cmpeqtmp58 = icmp eq i64 %"%idx.load57", 28
  br i1 %cmpeqtmp58, label %then_406, label %merge_407

merge_407:                                        ; preds = %then_406, %merge_405
  %"%idx.load59" = load i64, ptr %idx1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%idx.load59", 29
  br i1 %cmpeqtmp60, label %then_408, label %merge_409

merge_409:                                        ; preds = %then_408, %merge_407
  %"%idx.load61" = load i64, ptr %idx1, align 8
  %cmpeqtmp62 = icmp eq i64 %"%idx.load61", 30
  br i1 %cmpeqtmp62, label %then_410, label %merge_411

merge_411:                                        ; preds = %then_410, %merge_409
  %"%idx.load63" = load i64, ptr %idx1, align 8
  %cmpeqtmp64 = icmp eq i64 %"%idx.load63", 31
  br i1 %cmpeqtmp64, label %then_412, label %merge_413

merge_413:                                        ; preds = %then_412, %merge_411
  ret i64 0

then_350:                                         ; preds = %entry
  %"%864.load" = load i64, ptr %val2, align 8
  store i64 %"%864.load", ptr %"%pci_vid_0", align 8
  br label %merge_351

then_352:                                         ; preds = %merge_351
  %"%868.load" = load i64, ptr %val2, align 8
  store i64 %"%868.load", ptr %"%pci_vid_1", align 8
  br label %merge_353

then_354:                                         ; preds = %merge_353
  %"%872.load" = load i64, ptr %val2, align 8
  store i64 %"%872.load", ptr %"%pci_vid_2", align 8
  br label %merge_355

then_356:                                         ; preds = %merge_355
  %"%876.load" = load i64, ptr %val2, align 8
  store i64 %"%876.load", ptr %"%pci_vid_3", align 8
  br label %merge_357

then_358:                                         ; preds = %merge_357
  %"%880.load" = load i64, ptr %val2, align 8
  store i64 %"%880.load", ptr %"%pci_vid_4", align 8
  br label %merge_359

then_360:                                         ; preds = %merge_359
  %"%884.load" = load i64, ptr %val2, align 8
  store i64 %"%884.load", ptr %"%pci_vid_5", align 8
  br label %merge_361

then_362:                                         ; preds = %merge_361
  %"%888.load" = load i64, ptr %val2, align 8
  store i64 %"%888.load", ptr %"%pci_vid_6", align 8
  br label %merge_363

then_364:                                         ; preds = %merge_363
  %"%892.load" = load i64, ptr %val2, align 8
  store i64 %"%892.load", ptr %"%pci_vid_7", align 8
  br label %merge_365

then_366:                                         ; preds = %merge_365
  %"%896.load" = load i64, ptr %val2, align 8
  store i64 %"%896.load", ptr %"%pci_vid_8", align 8
  br label %merge_367

then_368:                                         ; preds = %merge_367
  %"%900.load" = load i64, ptr %val2, align 8
  store i64 %"%900.load", ptr %"%pci_vid_9", align 8
  br label %merge_369

then_370:                                         ; preds = %merge_369
  %"%904.load" = load i64, ptr %val2, align 8
  store i64 %"%904.load", ptr %"%pci_vid_10", align 8
  br label %merge_371

then_372:                                         ; preds = %merge_371
  %"%908.load" = load i64, ptr %val2, align 8
  store i64 %"%908.load", ptr %"%pci_vid_11", align 8
  br label %merge_373

then_374:                                         ; preds = %merge_373
  %"%912.load" = load i64, ptr %val2, align 8
  store i64 %"%912.load", ptr %"%pci_vid_12", align 8
  br label %merge_375

then_376:                                         ; preds = %merge_375
  %"%916.load" = load i64, ptr %val2, align 8
  store i64 %"%916.load", ptr %"%pci_vid_13", align 8
  br label %merge_377

then_378:                                         ; preds = %merge_377
  %"%920.load" = load i64, ptr %val2, align 8
  store i64 %"%920.load", ptr %"%pci_vid_14", align 8
  br label %merge_379

then_380:                                         ; preds = %merge_379
  %"%924.load" = load i64, ptr %val2, align 8
  store i64 %"%924.load", ptr %"%pci_vid_15", align 8
  br label %merge_381

then_382:                                         ; preds = %merge_381
  %"%928.load" = load i64, ptr %val2, align 8
  store i64 %"%928.load", ptr %"%pci_vid_16", align 8
  br label %merge_383

then_384:                                         ; preds = %merge_383
  %"%932.load" = load i64, ptr %val2, align 8
  store i64 %"%932.load", ptr %"%pci_vid_17", align 8
  br label %merge_385

then_386:                                         ; preds = %merge_385
  %"%936.load" = load i64, ptr %val2, align 8
  store i64 %"%936.load", ptr %"%pci_vid_18", align 8
  br label %merge_387

then_388:                                         ; preds = %merge_387
  %"%940.load" = load i64, ptr %val2, align 8
  store i64 %"%940.load", ptr %"%pci_vid_19", align 8
  br label %merge_389

then_390:                                         ; preds = %merge_389
  %"%944.load" = load i64, ptr %val2, align 8
  store i64 %"%944.load", ptr %"%pci_vid_20", align 8
  br label %merge_391

then_392:                                         ; preds = %merge_391
  %"%948.load" = load i64, ptr %val2, align 8
  store i64 %"%948.load", ptr %"%pci_vid_21", align 8
  br label %merge_393

then_394:                                         ; preds = %merge_393
  %"%952.load" = load i64, ptr %val2, align 8
  store i64 %"%952.load", ptr %"%pci_vid_22", align 8
  br label %merge_395

then_396:                                         ; preds = %merge_395
  %"%956.load" = load i64, ptr %val2, align 8
  store i64 %"%956.load", ptr %"%pci_vid_23", align 8
  br label %merge_397

then_398:                                         ; preds = %merge_397
  %"%960.load" = load i64, ptr %val2, align 8
  store i64 %"%960.load", ptr %"%pci_vid_24", align 8
  br label %merge_399

then_400:                                         ; preds = %merge_399
  %"%964.load" = load i64, ptr %val2, align 8
  store i64 %"%964.load", ptr %"%pci_vid_25", align 8
  br label %merge_401

then_402:                                         ; preds = %merge_401
  %"%968.load" = load i64, ptr %val2, align 8
  store i64 %"%968.load", ptr %"%pci_vid_26", align 8
  br label %merge_403

then_404:                                         ; preds = %merge_403
  %"%972.load" = load i64, ptr %val2, align 8
  store i64 %"%972.load", ptr %"%pci_vid_27", align 8
  br label %merge_405

then_406:                                         ; preds = %merge_405
  %"%976.load" = load i64, ptr %val2, align 8
  store i64 %"%976.load", ptr %"%pci_vid_28", align 8
  br label %merge_407

then_408:                                         ; preds = %merge_407
  %"%980.load" = load i64, ptr %val2, align 8
  store i64 %"%980.load", ptr %"%pci_vid_29", align 8
  br label %merge_409

then_410:                                         ; preds = %merge_409
  %"%984.load" = load i64, ptr %val2, align 8
  store i64 %"%984.load", ptr %"%pci_vid_30", align 8
  br label %merge_411

then_412:                                         ; preds = %merge_411
  %"%988.load" = load i64, ptr %val2, align 8
  store i64 %"%988.load", ptr %"%pci_vid_31", align 8
  br label %merge_413
}

define i64 @pci_get_did(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_414, label %merge_415

merge_415:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_416, label %merge_417

merge_417:                                        ; preds = %merge_415
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_418, label %merge_419

merge_419:                                        ; preds = %merge_417
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_420, label %merge_421

merge_421:                                        ; preds = %merge_419
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_422, label %merge_423

merge_423:                                        ; preds = %merge_421
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_424, label %merge_425

merge_425:                                        ; preds = %merge_423
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_426, label %merge_427

merge_427:                                        ; preds = %merge_425
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_428, label %merge_429

merge_429:                                        ; preds = %merge_427
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 8
  br i1 %cmpeqtmp24, label %then_430, label %merge_431

merge_431:                                        ; preds = %merge_429
  %"%idx.load26" = load i64, ptr %idx1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%idx.load26", 9
  br i1 %cmpeqtmp27, label %then_432, label %merge_433

merge_433:                                        ; preds = %merge_431
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 10
  br i1 %cmpeqtmp30, label %then_434, label %merge_435

merge_435:                                        ; preds = %merge_433
  %"%idx.load32" = load i64, ptr %idx1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%idx.load32", 11
  br i1 %cmpeqtmp33, label %then_436, label %merge_437

merge_437:                                        ; preds = %merge_435
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 12
  br i1 %cmpeqtmp36, label %then_438, label %merge_439

merge_439:                                        ; preds = %merge_437
  %"%idx.load38" = load i64, ptr %idx1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%idx.load38", 13
  br i1 %cmpeqtmp39, label %then_440, label %merge_441

merge_441:                                        ; preds = %merge_439
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 14
  br i1 %cmpeqtmp42, label %then_442, label %merge_443

merge_443:                                        ; preds = %merge_441
  %"%idx.load44" = load i64, ptr %idx1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%idx.load44", 15
  br i1 %cmpeqtmp45, label %then_444, label %merge_445

merge_445:                                        ; preds = %merge_443
  %"%idx.load47" = load i64, ptr %idx1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%idx.load47", 16
  br i1 %cmpeqtmp48, label %then_446, label %merge_447

merge_447:                                        ; preds = %merge_445
  %"%idx.load50" = load i64, ptr %idx1, align 8
  %cmpeqtmp51 = icmp eq i64 %"%idx.load50", 17
  br i1 %cmpeqtmp51, label %then_448, label %merge_449

merge_449:                                        ; preds = %merge_447
  %"%idx.load53" = load i64, ptr %idx1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%idx.load53", 18
  br i1 %cmpeqtmp54, label %then_450, label %merge_451

merge_451:                                        ; preds = %merge_449
  %"%idx.load56" = load i64, ptr %idx1, align 8
  %cmpeqtmp57 = icmp eq i64 %"%idx.load56", 19
  br i1 %cmpeqtmp57, label %then_452, label %merge_453

merge_453:                                        ; preds = %merge_451
  %"%idx.load59" = load i64, ptr %idx1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%idx.load59", 20
  br i1 %cmpeqtmp60, label %then_454, label %merge_455

merge_455:                                        ; preds = %merge_453
  %"%idx.load62" = load i64, ptr %idx1, align 8
  %cmpeqtmp63 = icmp eq i64 %"%idx.load62", 21
  br i1 %cmpeqtmp63, label %then_456, label %merge_457

merge_457:                                        ; preds = %merge_455
  %"%idx.load65" = load i64, ptr %idx1, align 8
  %cmpeqtmp66 = icmp eq i64 %"%idx.load65", 22
  br i1 %cmpeqtmp66, label %then_458, label %merge_459

merge_459:                                        ; preds = %merge_457
  %"%idx.load68" = load i64, ptr %idx1, align 8
  %cmpeqtmp69 = icmp eq i64 %"%idx.load68", 23
  br i1 %cmpeqtmp69, label %then_460, label %merge_461

merge_461:                                        ; preds = %merge_459
  %"%idx.load71" = load i64, ptr %idx1, align 8
  %cmpeqtmp72 = icmp eq i64 %"%idx.load71", 24
  br i1 %cmpeqtmp72, label %then_462, label %merge_463

merge_463:                                        ; preds = %merge_461
  %"%idx.load74" = load i64, ptr %idx1, align 8
  %cmpeqtmp75 = icmp eq i64 %"%idx.load74", 25
  br i1 %cmpeqtmp75, label %then_464, label %merge_465

merge_465:                                        ; preds = %merge_463
  %"%idx.load77" = load i64, ptr %idx1, align 8
  %cmpeqtmp78 = icmp eq i64 %"%idx.load77", 26
  br i1 %cmpeqtmp78, label %then_466, label %merge_467

merge_467:                                        ; preds = %merge_465
  %"%idx.load80" = load i64, ptr %idx1, align 8
  %cmpeqtmp81 = icmp eq i64 %"%idx.load80", 27
  br i1 %cmpeqtmp81, label %then_468, label %merge_469

merge_469:                                        ; preds = %merge_467
  %"%idx.load83" = load i64, ptr %idx1, align 8
  %cmpeqtmp84 = icmp eq i64 %"%idx.load83", 28
  br i1 %cmpeqtmp84, label %then_470, label %merge_471

merge_471:                                        ; preds = %merge_469
  %"%idx.load86" = load i64, ptr %idx1, align 8
  %cmpeqtmp87 = icmp eq i64 %"%idx.load86", 29
  br i1 %cmpeqtmp87, label %then_472, label %merge_473

merge_473:                                        ; preds = %merge_471
  %"%idx.load89" = load i64, ptr %idx1, align 8
  %cmpeqtmp90 = icmp eq i64 %"%idx.load89", 30
  br i1 %cmpeqtmp90, label %then_474, label %merge_475

merge_475:                                        ; preds = %merge_473
  %"%idx.load92" = load i64, ptr %idx1, align 8
  %cmpeqtmp93 = icmp eq i64 %"%idx.load92", 31
  br i1 %cmpeqtmp93, label %then_476, label %merge_477

merge_477:                                        ; preds = %merge_475
  ret i64 0

then_414:                                         ; preds = %entry
  %loadtmp = load i64, ptr @pci_did_0, align 8
  ret i64 %loadtmp

then_416:                                         ; preds = %merge_415
  %loadtmp4 = load i64, ptr @pci_did_1, align 8
  ret i64 %loadtmp4

then_418:                                         ; preds = %merge_417
  %loadtmp7 = load i64, ptr @pci_did_2, align 8
  ret i64 %loadtmp7

then_420:                                         ; preds = %merge_419
  %loadtmp10 = load i64, ptr @pci_did_3, align 8
  ret i64 %loadtmp10

then_422:                                         ; preds = %merge_421
  %loadtmp13 = load i64, ptr @pci_did_4, align 8
  ret i64 %loadtmp13

then_424:                                         ; preds = %merge_423
  %loadtmp16 = load i64, ptr @pci_did_5, align 8
  ret i64 %loadtmp16

then_426:                                         ; preds = %merge_425
  %loadtmp19 = load i64, ptr @pci_did_6, align 8
  ret i64 %loadtmp19

then_428:                                         ; preds = %merge_427
  %loadtmp22 = load i64, ptr @pci_did_7, align 8
  ret i64 %loadtmp22

then_430:                                         ; preds = %merge_429
  %loadtmp25 = load i64, ptr @pci_did_8, align 8
  ret i64 %loadtmp25

then_432:                                         ; preds = %merge_431
  %loadtmp28 = load i64, ptr @pci_did_9, align 8
  ret i64 %loadtmp28

then_434:                                         ; preds = %merge_433
  %loadtmp31 = load i64, ptr @pci_did_10, align 8
  ret i64 %loadtmp31

then_436:                                         ; preds = %merge_435
  %loadtmp34 = load i64, ptr @pci_did_11, align 8
  ret i64 %loadtmp34

then_438:                                         ; preds = %merge_437
  %loadtmp37 = load i64, ptr @pci_did_12, align 8
  ret i64 %loadtmp37

then_440:                                         ; preds = %merge_439
  %loadtmp40 = load i64, ptr @pci_did_13, align 8
  ret i64 %loadtmp40

then_442:                                         ; preds = %merge_441
  %loadtmp43 = load i64, ptr @pci_did_14, align 8
  ret i64 %loadtmp43

then_444:                                         ; preds = %merge_443
  %loadtmp46 = load i64, ptr @pci_did_15, align 8
  ret i64 %loadtmp46

then_446:                                         ; preds = %merge_445
  %loadtmp49 = load i64, ptr @pci_did_16, align 8
  ret i64 %loadtmp49

then_448:                                         ; preds = %merge_447
  %loadtmp52 = load i64, ptr @pci_did_17, align 8
  ret i64 %loadtmp52

then_450:                                         ; preds = %merge_449
  %loadtmp55 = load i64, ptr @pci_did_18, align 8
  ret i64 %loadtmp55

then_452:                                         ; preds = %merge_451
  %loadtmp58 = load i64, ptr @pci_did_19, align 8
  ret i64 %loadtmp58

then_454:                                         ; preds = %merge_453
  %loadtmp61 = load i64, ptr @pci_did_20, align 8
  ret i64 %loadtmp61

then_456:                                         ; preds = %merge_455
  %loadtmp64 = load i64, ptr @pci_did_21, align 8
  ret i64 %loadtmp64

then_458:                                         ; preds = %merge_457
  %loadtmp67 = load i64, ptr @pci_did_22, align 8
  ret i64 %loadtmp67

then_460:                                         ; preds = %merge_459
  %loadtmp70 = load i64, ptr @pci_did_23, align 8
  ret i64 %loadtmp70

then_462:                                         ; preds = %merge_461
  %loadtmp73 = load i64, ptr @pci_did_24, align 8
  ret i64 %loadtmp73

then_464:                                         ; preds = %merge_463
  %loadtmp76 = load i64, ptr @pci_did_25, align 8
  ret i64 %loadtmp76

then_466:                                         ; preds = %merge_465
  %loadtmp79 = load i64, ptr @pci_did_26, align 8
  ret i64 %loadtmp79

then_468:                                         ; preds = %merge_467
  %loadtmp82 = load i64, ptr @pci_did_27, align 8
  ret i64 %loadtmp82

then_470:                                         ; preds = %merge_469
  %loadtmp85 = load i64, ptr @pci_did_28, align 8
  ret i64 %loadtmp85

then_472:                                         ; preds = %merge_471
  %loadtmp88 = load i64, ptr @pci_did_29, align 8
  ret i64 %loadtmp88

then_474:                                         ; preds = %merge_473
  %loadtmp91 = load i64, ptr @pci_did_30, align 8
  ret i64 %loadtmp91

then_476:                                         ; preds = %merge_475
  %loadtmp94 = load i64, ptr @pci_did_31, align 8
  ret i64 %loadtmp94
}

define i64 @pci_set_did(i64 %idx, i64 %val) {
entry:
  %"%pci_did_31" = alloca i64, align 8
  %"%pci_did_30" = alloca i64, align 8
  %"%pci_did_29" = alloca i64, align 8
  %"%pci_did_28" = alloca i64, align 8
  %"%pci_did_27" = alloca i64, align 8
  %"%pci_did_26" = alloca i64, align 8
  %"%pci_did_25" = alloca i64, align 8
  %"%pci_did_24" = alloca i64, align 8
  %"%pci_did_23" = alloca i64, align 8
  %"%pci_did_22" = alloca i64, align 8
  %"%pci_did_21" = alloca i64, align 8
  %"%pci_did_20" = alloca i64, align 8
  %"%pci_did_19" = alloca i64, align 8
  %"%pci_did_18" = alloca i64, align 8
  %"%pci_did_17" = alloca i64, align 8
  %"%pci_did_16" = alloca i64, align 8
  %"%pci_did_15" = alloca i64, align 8
  %"%pci_did_14" = alloca i64, align 8
  %"%pci_did_13" = alloca i64, align 8
  %"%pci_did_12" = alloca i64, align 8
  %"%pci_did_11" = alloca i64, align 8
  %"%pci_did_10" = alloca i64, align 8
  %"%pci_did_9" = alloca i64, align 8
  %"%pci_did_8" = alloca i64, align 8
  %"%pci_did_7" = alloca i64, align 8
  %"%pci_did_6" = alloca i64, align 8
  %"%pci_did_5" = alloca i64, align 8
  %"%pci_did_4" = alloca i64, align 8
  %"%pci_did_3" = alloca i64, align 8
  %"%pci_did_2" = alloca i64, align 8
  %"%pci_did_1" = alloca i64, align 8
  %"%pci_did_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_478, label %merge_479

merge_479:                                        ; preds = %then_478, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_480, label %merge_481

merge_481:                                        ; preds = %then_480, %merge_479
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_482, label %merge_483

merge_483:                                        ; preds = %then_482, %merge_481
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_484, label %merge_485

merge_485:                                        ; preds = %then_484, %merge_483
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_486, label %merge_487

merge_487:                                        ; preds = %then_486, %merge_485
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_488, label %merge_489

merge_489:                                        ; preds = %then_488, %merge_487
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_490, label %merge_491

merge_491:                                        ; preds = %then_490, %merge_489
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_492, label %merge_493

merge_493:                                        ; preds = %then_492, %merge_491
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_494, label %merge_495

merge_495:                                        ; preds = %then_494, %merge_493
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_496, label %merge_497

merge_497:                                        ; preds = %then_496, %merge_495
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_498, label %merge_499

merge_499:                                        ; preds = %then_498, %merge_497
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_500, label %merge_501

merge_501:                                        ; preds = %then_500, %merge_499
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_502, label %merge_503

merge_503:                                        ; preds = %then_502, %merge_501
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_504, label %merge_505

merge_505:                                        ; preds = %then_504, %merge_503
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_506, label %merge_507

merge_507:                                        ; preds = %then_506, %merge_505
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_508, label %merge_509

merge_509:                                        ; preds = %then_508, %merge_507
  %"%idx.load33" = load i64, ptr %idx1, align 8
  %cmpeqtmp34 = icmp eq i64 %"%idx.load33", 16
  br i1 %cmpeqtmp34, label %then_510, label %merge_511

merge_511:                                        ; preds = %then_510, %merge_509
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 17
  br i1 %cmpeqtmp36, label %then_512, label %merge_513

merge_513:                                        ; preds = %then_512, %merge_511
  %"%idx.load37" = load i64, ptr %idx1, align 8
  %cmpeqtmp38 = icmp eq i64 %"%idx.load37", 18
  br i1 %cmpeqtmp38, label %then_514, label %merge_515

merge_515:                                        ; preds = %then_514, %merge_513
  %"%idx.load39" = load i64, ptr %idx1, align 8
  %cmpeqtmp40 = icmp eq i64 %"%idx.load39", 19
  br i1 %cmpeqtmp40, label %then_516, label %merge_517

merge_517:                                        ; preds = %then_516, %merge_515
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 20
  br i1 %cmpeqtmp42, label %then_518, label %merge_519

merge_519:                                        ; preds = %then_518, %merge_517
  %"%idx.load43" = load i64, ptr %idx1, align 8
  %cmpeqtmp44 = icmp eq i64 %"%idx.load43", 21
  br i1 %cmpeqtmp44, label %then_520, label %merge_521

merge_521:                                        ; preds = %then_520, %merge_519
  %"%idx.load45" = load i64, ptr %idx1, align 8
  %cmpeqtmp46 = icmp eq i64 %"%idx.load45", 22
  br i1 %cmpeqtmp46, label %then_522, label %merge_523

merge_523:                                        ; preds = %then_522, %merge_521
  %"%idx.load47" = load i64, ptr %idx1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%idx.load47", 23
  br i1 %cmpeqtmp48, label %then_524, label %merge_525

merge_525:                                        ; preds = %then_524, %merge_523
  %"%idx.load49" = load i64, ptr %idx1, align 8
  %cmpeqtmp50 = icmp eq i64 %"%idx.load49", 24
  br i1 %cmpeqtmp50, label %then_526, label %merge_527

merge_527:                                        ; preds = %then_526, %merge_525
  %"%idx.load51" = load i64, ptr %idx1, align 8
  %cmpeqtmp52 = icmp eq i64 %"%idx.load51", 25
  br i1 %cmpeqtmp52, label %then_528, label %merge_529

merge_529:                                        ; preds = %then_528, %merge_527
  %"%idx.load53" = load i64, ptr %idx1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%idx.load53", 26
  br i1 %cmpeqtmp54, label %then_530, label %merge_531

merge_531:                                        ; preds = %then_530, %merge_529
  %"%idx.load55" = load i64, ptr %idx1, align 8
  %cmpeqtmp56 = icmp eq i64 %"%idx.load55", 27
  br i1 %cmpeqtmp56, label %then_532, label %merge_533

merge_533:                                        ; preds = %then_532, %merge_531
  %"%idx.load57" = load i64, ptr %idx1, align 8
  %cmpeqtmp58 = icmp eq i64 %"%idx.load57", 28
  br i1 %cmpeqtmp58, label %then_534, label %merge_535

merge_535:                                        ; preds = %then_534, %merge_533
  %"%idx.load59" = load i64, ptr %idx1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%idx.load59", 29
  br i1 %cmpeqtmp60, label %then_536, label %merge_537

merge_537:                                        ; preds = %then_536, %merge_535
  %"%idx.load61" = load i64, ptr %idx1, align 8
  %cmpeqtmp62 = icmp eq i64 %"%idx.load61", 30
  br i1 %cmpeqtmp62, label %then_538, label %merge_539

merge_539:                                        ; preds = %then_538, %merge_537
  %"%idx.load63" = load i64, ptr %idx1, align 8
  %cmpeqtmp64 = icmp eq i64 %"%idx.load63", 31
  br i1 %cmpeqtmp64, label %then_540, label %merge_541

merge_541:                                        ; preds = %then_540, %merge_539
  ret i64 0

then_478:                                         ; preds = %entry
  %"%1154.load" = load i64, ptr %val2, align 8
  store i64 %"%1154.load", ptr %"%pci_did_0", align 8
  br label %merge_479

then_480:                                         ; preds = %merge_479
  %"%1158.load" = load i64, ptr %val2, align 8
  store i64 %"%1158.load", ptr %"%pci_did_1", align 8
  br label %merge_481

then_482:                                         ; preds = %merge_481
  %"%1162.load" = load i64, ptr %val2, align 8
  store i64 %"%1162.load", ptr %"%pci_did_2", align 8
  br label %merge_483

then_484:                                         ; preds = %merge_483
  %"%1166.load" = load i64, ptr %val2, align 8
  store i64 %"%1166.load", ptr %"%pci_did_3", align 8
  br label %merge_485

then_486:                                         ; preds = %merge_485
  %"%1170.load" = load i64, ptr %val2, align 8
  store i64 %"%1170.load", ptr %"%pci_did_4", align 8
  br label %merge_487

then_488:                                         ; preds = %merge_487
  %"%1174.load" = load i64, ptr %val2, align 8
  store i64 %"%1174.load", ptr %"%pci_did_5", align 8
  br label %merge_489

then_490:                                         ; preds = %merge_489
  %"%1178.load" = load i64, ptr %val2, align 8
  store i64 %"%1178.load", ptr %"%pci_did_6", align 8
  br label %merge_491

then_492:                                         ; preds = %merge_491
  %"%1182.load" = load i64, ptr %val2, align 8
  store i64 %"%1182.load", ptr %"%pci_did_7", align 8
  br label %merge_493

then_494:                                         ; preds = %merge_493
  %"%1186.load" = load i64, ptr %val2, align 8
  store i64 %"%1186.load", ptr %"%pci_did_8", align 8
  br label %merge_495

then_496:                                         ; preds = %merge_495
  %"%1190.load" = load i64, ptr %val2, align 8
  store i64 %"%1190.load", ptr %"%pci_did_9", align 8
  br label %merge_497

then_498:                                         ; preds = %merge_497
  %"%1194.load" = load i64, ptr %val2, align 8
  store i64 %"%1194.load", ptr %"%pci_did_10", align 8
  br label %merge_499

then_500:                                         ; preds = %merge_499
  %"%1198.load" = load i64, ptr %val2, align 8
  store i64 %"%1198.load", ptr %"%pci_did_11", align 8
  br label %merge_501

then_502:                                         ; preds = %merge_501
  %"%1202.load" = load i64, ptr %val2, align 8
  store i64 %"%1202.load", ptr %"%pci_did_12", align 8
  br label %merge_503

then_504:                                         ; preds = %merge_503
  %"%1206.load" = load i64, ptr %val2, align 8
  store i64 %"%1206.load", ptr %"%pci_did_13", align 8
  br label %merge_505

then_506:                                         ; preds = %merge_505
  %"%1210.load" = load i64, ptr %val2, align 8
  store i64 %"%1210.load", ptr %"%pci_did_14", align 8
  br label %merge_507

then_508:                                         ; preds = %merge_507
  %"%1214.load" = load i64, ptr %val2, align 8
  store i64 %"%1214.load", ptr %"%pci_did_15", align 8
  br label %merge_509

then_510:                                         ; preds = %merge_509
  %"%1218.load" = load i64, ptr %val2, align 8
  store i64 %"%1218.load", ptr %"%pci_did_16", align 8
  br label %merge_511

then_512:                                         ; preds = %merge_511
  %"%1222.load" = load i64, ptr %val2, align 8
  store i64 %"%1222.load", ptr %"%pci_did_17", align 8
  br label %merge_513

then_514:                                         ; preds = %merge_513
  %"%1226.load" = load i64, ptr %val2, align 8
  store i64 %"%1226.load", ptr %"%pci_did_18", align 8
  br label %merge_515

then_516:                                         ; preds = %merge_515
  %"%1230.load" = load i64, ptr %val2, align 8
  store i64 %"%1230.load", ptr %"%pci_did_19", align 8
  br label %merge_517

then_518:                                         ; preds = %merge_517
  %"%1234.load" = load i64, ptr %val2, align 8
  store i64 %"%1234.load", ptr %"%pci_did_20", align 8
  br label %merge_519

then_520:                                         ; preds = %merge_519
  %"%1238.load" = load i64, ptr %val2, align 8
  store i64 %"%1238.load", ptr %"%pci_did_21", align 8
  br label %merge_521

then_522:                                         ; preds = %merge_521
  %"%1242.load" = load i64, ptr %val2, align 8
  store i64 %"%1242.load", ptr %"%pci_did_22", align 8
  br label %merge_523

then_524:                                         ; preds = %merge_523
  %"%1246.load" = load i64, ptr %val2, align 8
  store i64 %"%1246.load", ptr %"%pci_did_23", align 8
  br label %merge_525

then_526:                                         ; preds = %merge_525
  %"%1250.load" = load i64, ptr %val2, align 8
  store i64 %"%1250.load", ptr %"%pci_did_24", align 8
  br label %merge_527

then_528:                                         ; preds = %merge_527
  %"%1254.load" = load i64, ptr %val2, align 8
  store i64 %"%1254.load", ptr %"%pci_did_25", align 8
  br label %merge_529

then_530:                                         ; preds = %merge_529
  %"%1258.load" = load i64, ptr %val2, align 8
  store i64 %"%1258.load", ptr %"%pci_did_26", align 8
  br label %merge_531

then_532:                                         ; preds = %merge_531
  %"%1262.load" = load i64, ptr %val2, align 8
  store i64 %"%1262.load", ptr %"%pci_did_27", align 8
  br label %merge_533

then_534:                                         ; preds = %merge_533
  %"%1266.load" = load i64, ptr %val2, align 8
  store i64 %"%1266.load", ptr %"%pci_did_28", align 8
  br label %merge_535

then_536:                                         ; preds = %merge_535
  %"%1270.load" = load i64, ptr %val2, align 8
  store i64 %"%1270.load", ptr %"%pci_did_29", align 8
  br label %merge_537

then_538:                                         ; preds = %merge_537
  %"%1274.load" = load i64, ptr %val2, align 8
  store i64 %"%1274.load", ptr %"%pci_did_30", align 8
  br label %merge_539

then_540:                                         ; preds = %merge_539
  %"%1278.load" = load i64, ptr %val2, align 8
  store i64 %"%1278.load", ptr %"%pci_did_31", align 8
  br label %merge_541
}

define i64 @pci_get_class(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_542, label %merge_543

merge_543:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_544, label %merge_545

merge_545:                                        ; preds = %merge_543
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_546, label %merge_547

merge_547:                                        ; preds = %merge_545
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_548, label %merge_549

merge_549:                                        ; preds = %merge_547
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_550, label %merge_551

merge_551:                                        ; preds = %merge_549
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_552, label %merge_553

merge_553:                                        ; preds = %merge_551
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_554, label %merge_555

merge_555:                                        ; preds = %merge_553
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_556, label %merge_557

merge_557:                                        ; preds = %merge_555
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 8
  br i1 %cmpeqtmp24, label %then_558, label %merge_559

merge_559:                                        ; preds = %merge_557
  %"%idx.load26" = load i64, ptr %idx1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%idx.load26", 9
  br i1 %cmpeqtmp27, label %then_560, label %merge_561

merge_561:                                        ; preds = %merge_559
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 10
  br i1 %cmpeqtmp30, label %then_562, label %merge_563

merge_563:                                        ; preds = %merge_561
  %"%idx.load32" = load i64, ptr %idx1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%idx.load32", 11
  br i1 %cmpeqtmp33, label %then_564, label %merge_565

merge_565:                                        ; preds = %merge_563
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 12
  br i1 %cmpeqtmp36, label %then_566, label %merge_567

merge_567:                                        ; preds = %merge_565
  %"%idx.load38" = load i64, ptr %idx1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%idx.load38", 13
  br i1 %cmpeqtmp39, label %then_568, label %merge_569

merge_569:                                        ; preds = %merge_567
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 14
  br i1 %cmpeqtmp42, label %then_570, label %merge_571

merge_571:                                        ; preds = %merge_569
  %"%idx.load44" = load i64, ptr %idx1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%idx.load44", 15
  br i1 %cmpeqtmp45, label %then_572, label %merge_573

merge_573:                                        ; preds = %merge_571
  %"%idx.load47" = load i64, ptr %idx1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%idx.load47", 16
  br i1 %cmpeqtmp48, label %then_574, label %merge_575

merge_575:                                        ; preds = %merge_573
  %"%idx.load50" = load i64, ptr %idx1, align 8
  %cmpeqtmp51 = icmp eq i64 %"%idx.load50", 17
  br i1 %cmpeqtmp51, label %then_576, label %merge_577

merge_577:                                        ; preds = %merge_575
  %"%idx.load53" = load i64, ptr %idx1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%idx.load53", 18
  br i1 %cmpeqtmp54, label %then_578, label %merge_579

merge_579:                                        ; preds = %merge_577
  %"%idx.load56" = load i64, ptr %idx1, align 8
  %cmpeqtmp57 = icmp eq i64 %"%idx.load56", 19
  br i1 %cmpeqtmp57, label %then_580, label %merge_581

merge_581:                                        ; preds = %merge_579
  %"%idx.load59" = load i64, ptr %idx1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%idx.load59", 20
  br i1 %cmpeqtmp60, label %then_582, label %merge_583

merge_583:                                        ; preds = %merge_581
  %"%idx.load62" = load i64, ptr %idx1, align 8
  %cmpeqtmp63 = icmp eq i64 %"%idx.load62", 21
  br i1 %cmpeqtmp63, label %then_584, label %merge_585

merge_585:                                        ; preds = %merge_583
  %"%idx.load65" = load i64, ptr %idx1, align 8
  %cmpeqtmp66 = icmp eq i64 %"%idx.load65", 22
  br i1 %cmpeqtmp66, label %then_586, label %merge_587

merge_587:                                        ; preds = %merge_585
  %"%idx.load68" = load i64, ptr %idx1, align 8
  %cmpeqtmp69 = icmp eq i64 %"%idx.load68", 23
  br i1 %cmpeqtmp69, label %then_588, label %merge_589

merge_589:                                        ; preds = %merge_587
  %"%idx.load71" = load i64, ptr %idx1, align 8
  %cmpeqtmp72 = icmp eq i64 %"%idx.load71", 24
  br i1 %cmpeqtmp72, label %then_590, label %merge_591

merge_591:                                        ; preds = %merge_589
  %"%idx.load74" = load i64, ptr %idx1, align 8
  %cmpeqtmp75 = icmp eq i64 %"%idx.load74", 25
  br i1 %cmpeqtmp75, label %then_592, label %merge_593

merge_593:                                        ; preds = %merge_591
  %"%idx.load77" = load i64, ptr %idx1, align 8
  %cmpeqtmp78 = icmp eq i64 %"%idx.load77", 26
  br i1 %cmpeqtmp78, label %then_594, label %merge_595

merge_595:                                        ; preds = %merge_593
  %"%idx.load80" = load i64, ptr %idx1, align 8
  %cmpeqtmp81 = icmp eq i64 %"%idx.load80", 27
  br i1 %cmpeqtmp81, label %then_596, label %merge_597

merge_597:                                        ; preds = %merge_595
  %"%idx.load83" = load i64, ptr %idx1, align 8
  %cmpeqtmp84 = icmp eq i64 %"%idx.load83", 28
  br i1 %cmpeqtmp84, label %then_598, label %merge_599

merge_599:                                        ; preds = %merge_597
  %"%idx.load86" = load i64, ptr %idx1, align 8
  %cmpeqtmp87 = icmp eq i64 %"%idx.load86", 29
  br i1 %cmpeqtmp87, label %then_600, label %merge_601

merge_601:                                        ; preds = %merge_599
  %"%idx.load89" = load i64, ptr %idx1, align 8
  %cmpeqtmp90 = icmp eq i64 %"%idx.load89", 30
  br i1 %cmpeqtmp90, label %then_602, label %merge_603

merge_603:                                        ; preds = %merge_601
  %"%idx.load92" = load i64, ptr %idx1, align 8
  %cmpeqtmp93 = icmp eq i64 %"%idx.load92", 31
  br i1 %cmpeqtmp93, label %then_604, label %merge_605

merge_605:                                        ; preds = %merge_603
  ret i64 0

then_542:                                         ; preds = %entry
  %loadtmp = load i64, ptr @pci_class_0, align 8
  ret i64 %loadtmp

then_544:                                         ; preds = %merge_543
  %loadtmp4 = load i64, ptr @pci_class_1, align 8
  ret i64 %loadtmp4

then_546:                                         ; preds = %merge_545
  %loadtmp7 = load i64, ptr @pci_class_2, align 8
  ret i64 %loadtmp7

then_548:                                         ; preds = %merge_547
  %loadtmp10 = load i64, ptr @pci_class_3, align 8
  ret i64 %loadtmp10

then_550:                                         ; preds = %merge_549
  %loadtmp13 = load i64, ptr @pci_class_4, align 8
  ret i64 %loadtmp13

then_552:                                         ; preds = %merge_551
  %loadtmp16 = load i64, ptr @pci_class_5, align 8
  ret i64 %loadtmp16

then_554:                                         ; preds = %merge_553
  %loadtmp19 = load i64, ptr @pci_class_6, align 8
  ret i64 %loadtmp19

then_556:                                         ; preds = %merge_555
  %loadtmp22 = load i64, ptr @pci_class_7, align 8
  ret i64 %loadtmp22

then_558:                                         ; preds = %merge_557
  %loadtmp25 = load i64, ptr @pci_class_8, align 8
  ret i64 %loadtmp25

then_560:                                         ; preds = %merge_559
  %loadtmp28 = load i64, ptr @pci_class_9, align 8
  ret i64 %loadtmp28

then_562:                                         ; preds = %merge_561
  %loadtmp31 = load i64, ptr @pci_class_10, align 8
  ret i64 %loadtmp31

then_564:                                         ; preds = %merge_563
  %loadtmp34 = load i64, ptr @pci_class_11, align 8
  ret i64 %loadtmp34

then_566:                                         ; preds = %merge_565
  %loadtmp37 = load i64, ptr @pci_class_12, align 8
  ret i64 %loadtmp37

then_568:                                         ; preds = %merge_567
  %loadtmp40 = load i64, ptr @pci_class_13, align 8
  ret i64 %loadtmp40

then_570:                                         ; preds = %merge_569
  %loadtmp43 = load i64, ptr @pci_class_14, align 8
  ret i64 %loadtmp43

then_572:                                         ; preds = %merge_571
  %loadtmp46 = load i64, ptr @pci_class_15, align 8
  ret i64 %loadtmp46

then_574:                                         ; preds = %merge_573
  %loadtmp49 = load i64, ptr @pci_class_16, align 8
  ret i64 %loadtmp49

then_576:                                         ; preds = %merge_575
  %loadtmp52 = load i64, ptr @pci_class_17, align 8
  ret i64 %loadtmp52

then_578:                                         ; preds = %merge_577
  %loadtmp55 = load i64, ptr @pci_class_18, align 8
  ret i64 %loadtmp55

then_580:                                         ; preds = %merge_579
  %loadtmp58 = load i64, ptr @pci_class_19, align 8
  ret i64 %loadtmp58

then_582:                                         ; preds = %merge_581
  %loadtmp61 = load i64, ptr @pci_class_20, align 8
  ret i64 %loadtmp61

then_584:                                         ; preds = %merge_583
  %loadtmp64 = load i64, ptr @pci_class_21, align 8
  ret i64 %loadtmp64

then_586:                                         ; preds = %merge_585
  %loadtmp67 = load i64, ptr @pci_class_22, align 8
  ret i64 %loadtmp67

then_588:                                         ; preds = %merge_587
  %loadtmp70 = load i64, ptr @pci_class_23, align 8
  ret i64 %loadtmp70

then_590:                                         ; preds = %merge_589
  %loadtmp73 = load i64, ptr @pci_class_24, align 8
  ret i64 %loadtmp73

then_592:                                         ; preds = %merge_591
  %loadtmp76 = load i64, ptr @pci_class_25, align 8
  ret i64 %loadtmp76

then_594:                                         ; preds = %merge_593
  %loadtmp79 = load i64, ptr @pci_class_26, align 8
  ret i64 %loadtmp79

then_596:                                         ; preds = %merge_595
  %loadtmp82 = load i64, ptr @pci_class_27, align 8
  ret i64 %loadtmp82

then_598:                                         ; preds = %merge_597
  %loadtmp85 = load i64, ptr @pci_class_28, align 8
  ret i64 %loadtmp85

then_600:                                         ; preds = %merge_599
  %loadtmp88 = load i64, ptr @pci_class_29, align 8
  ret i64 %loadtmp88

then_602:                                         ; preds = %merge_601
  %loadtmp91 = load i64, ptr @pci_class_30, align 8
  ret i64 %loadtmp91

then_604:                                         ; preds = %merge_603
  %loadtmp94 = load i64, ptr @pci_class_31, align 8
  ret i64 %loadtmp94
}

define i64 @pci_set_class(i64 %idx, i64 %val) {
entry:
  %"%pci_class_31" = alloca i64, align 8
  %"%pci_class_30" = alloca i64, align 8
  %"%pci_class_29" = alloca i64, align 8
  %"%pci_class_28" = alloca i64, align 8
  %"%pci_class_27" = alloca i64, align 8
  %"%pci_class_26" = alloca i64, align 8
  %"%pci_class_25" = alloca i64, align 8
  %"%pci_class_24" = alloca i64, align 8
  %"%pci_class_23" = alloca i64, align 8
  %"%pci_class_22" = alloca i64, align 8
  %"%pci_class_21" = alloca i64, align 8
  %"%pci_class_20" = alloca i64, align 8
  %"%pci_class_19" = alloca i64, align 8
  %"%pci_class_18" = alloca i64, align 8
  %"%pci_class_17" = alloca i64, align 8
  %"%pci_class_16" = alloca i64, align 8
  %"%pci_class_15" = alloca i64, align 8
  %"%pci_class_14" = alloca i64, align 8
  %"%pci_class_13" = alloca i64, align 8
  %"%pci_class_12" = alloca i64, align 8
  %"%pci_class_11" = alloca i64, align 8
  %"%pci_class_10" = alloca i64, align 8
  %"%pci_class_9" = alloca i64, align 8
  %"%pci_class_8" = alloca i64, align 8
  %"%pci_class_7" = alloca i64, align 8
  %"%pci_class_6" = alloca i64, align 8
  %"%pci_class_5" = alloca i64, align 8
  %"%pci_class_4" = alloca i64, align 8
  %"%pci_class_3" = alloca i64, align 8
  %"%pci_class_2" = alloca i64, align 8
  %"%pci_class_1" = alloca i64, align 8
  %"%pci_class_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_606, label %merge_607

merge_607:                                        ; preds = %then_606, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_608, label %merge_609

merge_609:                                        ; preds = %then_608, %merge_607
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_610, label %merge_611

merge_611:                                        ; preds = %then_610, %merge_609
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_612, label %merge_613

merge_613:                                        ; preds = %then_612, %merge_611
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_614, label %merge_615

merge_615:                                        ; preds = %then_614, %merge_613
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_616, label %merge_617

merge_617:                                        ; preds = %then_616, %merge_615
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_618, label %merge_619

merge_619:                                        ; preds = %then_618, %merge_617
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_620, label %merge_621

merge_621:                                        ; preds = %then_620, %merge_619
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_622, label %merge_623

merge_623:                                        ; preds = %then_622, %merge_621
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_624, label %merge_625

merge_625:                                        ; preds = %then_624, %merge_623
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_626, label %merge_627

merge_627:                                        ; preds = %then_626, %merge_625
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_628, label %merge_629

merge_629:                                        ; preds = %then_628, %merge_627
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_630, label %merge_631

merge_631:                                        ; preds = %then_630, %merge_629
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_632, label %merge_633

merge_633:                                        ; preds = %then_632, %merge_631
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_634, label %merge_635

merge_635:                                        ; preds = %then_634, %merge_633
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_636, label %merge_637

merge_637:                                        ; preds = %then_636, %merge_635
  %"%idx.load33" = load i64, ptr %idx1, align 8
  %cmpeqtmp34 = icmp eq i64 %"%idx.load33", 16
  br i1 %cmpeqtmp34, label %then_638, label %merge_639

merge_639:                                        ; preds = %then_638, %merge_637
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 17
  br i1 %cmpeqtmp36, label %then_640, label %merge_641

merge_641:                                        ; preds = %then_640, %merge_639
  %"%idx.load37" = load i64, ptr %idx1, align 8
  %cmpeqtmp38 = icmp eq i64 %"%idx.load37", 18
  br i1 %cmpeqtmp38, label %then_642, label %merge_643

merge_643:                                        ; preds = %then_642, %merge_641
  %"%idx.load39" = load i64, ptr %idx1, align 8
  %cmpeqtmp40 = icmp eq i64 %"%idx.load39", 19
  br i1 %cmpeqtmp40, label %then_644, label %merge_645

merge_645:                                        ; preds = %then_644, %merge_643
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 20
  br i1 %cmpeqtmp42, label %then_646, label %merge_647

merge_647:                                        ; preds = %then_646, %merge_645
  %"%idx.load43" = load i64, ptr %idx1, align 8
  %cmpeqtmp44 = icmp eq i64 %"%idx.load43", 21
  br i1 %cmpeqtmp44, label %then_648, label %merge_649

merge_649:                                        ; preds = %then_648, %merge_647
  %"%idx.load45" = load i64, ptr %idx1, align 8
  %cmpeqtmp46 = icmp eq i64 %"%idx.load45", 22
  br i1 %cmpeqtmp46, label %then_650, label %merge_651

merge_651:                                        ; preds = %then_650, %merge_649
  %"%idx.load47" = load i64, ptr %idx1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%idx.load47", 23
  br i1 %cmpeqtmp48, label %then_652, label %merge_653

merge_653:                                        ; preds = %then_652, %merge_651
  %"%idx.load49" = load i64, ptr %idx1, align 8
  %cmpeqtmp50 = icmp eq i64 %"%idx.load49", 24
  br i1 %cmpeqtmp50, label %then_654, label %merge_655

merge_655:                                        ; preds = %then_654, %merge_653
  %"%idx.load51" = load i64, ptr %idx1, align 8
  %cmpeqtmp52 = icmp eq i64 %"%idx.load51", 25
  br i1 %cmpeqtmp52, label %then_656, label %merge_657

merge_657:                                        ; preds = %then_656, %merge_655
  %"%idx.load53" = load i64, ptr %idx1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%idx.load53", 26
  br i1 %cmpeqtmp54, label %then_658, label %merge_659

merge_659:                                        ; preds = %then_658, %merge_657
  %"%idx.load55" = load i64, ptr %idx1, align 8
  %cmpeqtmp56 = icmp eq i64 %"%idx.load55", 27
  br i1 %cmpeqtmp56, label %then_660, label %merge_661

merge_661:                                        ; preds = %then_660, %merge_659
  %"%idx.load57" = load i64, ptr %idx1, align 8
  %cmpeqtmp58 = icmp eq i64 %"%idx.load57", 28
  br i1 %cmpeqtmp58, label %then_662, label %merge_663

merge_663:                                        ; preds = %then_662, %merge_661
  %"%idx.load59" = load i64, ptr %idx1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%idx.load59", 29
  br i1 %cmpeqtmp60, label %then_664, label %merge_665

merge_665:                                        ; preds = %then_664, %merge_663
  %"%idx.load61" = load i64, ptr %idx1, align 8
  %cmpeqtmp62 = icmp eq i64 %"%idx.load61", 30
  br i1 %cmpeqtmp62, label %then_666, label %merge_667

merge_667:                                        ; preds = %then_666, %merge_665
  %"%idx.load63" = load i64, ptr %idx1, align 8
  %cmpeqtmp64 = icmp eq i64 %"%idx.load63", 31
  br i1 %cmpeqtmp64, label %then_668, label %merge_669

merge_669:                                        ; preds = %then_668, %merge_667
  ret i64 0

then_606:                                         ; preds = %entry
  %"%1444.load" = load i64, ptr %val2, align 8
  store i64 %"%1444.load", ptr %"%pci_class_0", align 8
  br label %merge_607

then_608:                                         ; preds = %merge_607
  %"%1448.load" = load i64, ptr %val2, align 8
  store i64 %"%1448.load", ptr %"%pci_class_1", align 8
  br label %merge_609

then_610:                                         ; preds = %merge_609
  %"%1452.load" = load i64, ptr %val2, align 8
  store i64 %"%1452.load", ptr %"%pci_class_2", align 8
  br label %merge_611

then_612:                                         ; preds = %merge_611
  %"%1456.load" = load i64, ptr %val2, align 8
  store i64 %"%1456.load", ptr %"%pci_class_3", align 8
  br label %merge_613

then_614:                                         ; preds = %merge_613
  %"%1460.load" = load i64, ptr %val2, align 8
  store i64 %"%1460.load", ptr %"%pci_class_4", align 8
  br label %merge_615

then_616:                                         ; preds = %merge_615
  %"%1464.load" = load i64, ptr %val2, align 8
  store i64 %"%1464.load", ptr %"%pci_class_5", align 8
  br label %merge_617

then_618:                                         ; preds = %merge_617
  %"%1468.load" = load i64, ptr %val2, align 8
  store i64 %"%1468.load", ptr %"%pci_class_6", align 8
  br label %merge_619

then_620:                                         ; preds = %merge_619
  %"%1472.load" = load i64, ptr %val2, align 8
  store i64 %"%1472.load", ptr %"%pci_class_7", align 8
  br label %merge_621

then_622:                                         ; preds = %merge_621
  %"%1476.load" = load i64, ptr %val2, align 8
  store i64 %"%1476.load", ptr %"%pci_class_8", align 8
  br label %merge_623

then_624:                                         ; preds = %merge_623
  %"%1480.load" = load i64, ptr %val2, align 8
  store i64 %"%1480.load", ptr %"%pci_class_9", align 8
  br label %merge_625

then_626:                                         ; preds = %merge_625
  %"%1484.load" = load i64, ptr %val2, align 8
  store i64 %"%1484.load", ptr %"%pci_class_10", align 8
  br label %merge_627

then_628:                                         ; preds = %merge_627
  %"%1488.load" = load i64, ptr %val2, align 8
  store i64 %"%1488.load", ptr %"%pci_class_11", align 8
  br label %merge_629

then_630:                                         ; preds = %merge_629
  %"%1492.load" = load i64, ptr %val2, align 8
  store i64 %"%1492.load", ptr %"%pci_class_12", align 8
  br label %merge_631

then_632:                                         ; preds = %merge_631
  %"%1496.load" = load i64, ptr %val2, align 8
  store i64 %"%1496.load", ptr %"%pci_class_13", align 8
  br label %merge_633

then_634:                                         ; preds = %merge_633
  %"%1500.load" = load i64, ptr %val2, align 8
  store i64 %"%1500.load", ptr %"%pci_class_14", align 8
  br label %merge_635

then_636:                                         ; preds = %merge_635
  %"%1504.load" = load i64, ptr %val2, align 8
  store i64 %"%1504.load", ptr %"%pci_class_15", align 8
  br label %merge_637

then_638:                                         ; preds = %merge_637
  %"%1508.load" = load i64, ptr %val2, align 8
  store i64 %"%1508.load", ptr %"%pci_class_16", align 8
  br label %merge_639

then_640:                                         ; preds = %merge_639
  %"%1512.load" = load i64, ptr %val2, align 8
  store i64 %"%1512.load", ptr %"%pci_class_17", align 8
  br label %merge_641

then_642:                                         ; preds = %merge_641
  %"%1516.load" = load i64, ptr %val2, align 8
  store i64 %"%1516.load", ptr %"%pci_class_18", align 8
  br label %merge_643

then_644:                                         ; preds = %merge_643
  %"%1520.load" = load i64, ptr %val2, align 8
  store i64 %"%1520.load", ptr %"%pci_class_19", align 8
  br label %merge_645

then_646:                                         ; preds = %merge_645
  %"%1524.load" = load i64, ptr %val2, align 8
  store i64 %"%1524.load", ptr %"%pci_class_20", align 8
  br label %merge_647

then_648:                                         ; preds = %merge_647
  %"%1528.load" = load i64, ptr %val2, align 8
  store i64 %"%1528.load", ptr %"%pci_class_21", align 8
  br label %merge_649

then_650:                                         ; preds = %merge_649
  %"%1532.load" = load i64, ptr %val2, align 8
  store i64 %"%1532.load", ptr %"%pci_class_22", align 8
  br label %merge_651

then_652:                                         ; preds = %merge_651
  %"%1536.load" = load i64, ptr %val2, align 8
  store i64 %"%1536.load", ptr %"%pci_class_23", align 8
  br label %merge_653

then_654:                                         ; preds = %merge_653
  %"%1540.load" = load i64, ptr %val2, align 8
  store i64 %"%1540.load", ptr %"%pci_class_24", align 8
  br label %merge_655

then_656:                                         ; preds = %merge_655
  %"%1544.load" = load i64, ptr %val2, align 8
  store i64 %"%1544.load", ptr %"%pci_class_25", align 8
  br label %merge_657

then_658:                                         ; preds = %merge_657
  %"%1548.load" = load i64, ptr %val2, align 8
  store i64 %"%1548.load", ptr %"%pci_class_26", align 8
  br label %merge_659

then_660:                                         ; preds = %merge_659
  %"%1552.load" = load i64, ptr %val2, align 8
  store i64 %"%1552.load", ptr %"%pci_class_27", align 8
  br label %merge_661

then_662:                                         ; preds = %merge_661
  %"%1556.load" = load i64, ptr %val2, align 8
  store i64 %"%1556.load", ptr %"%pci_class_28", align 8
  br label %merge_663

then_664:                                         ; preds = %merge_663
  %"%1560.load" = load i64, ptr %val2, align 8
  store i64 %"%1560.load", ptr %"%pci_class_29", align 8
  br label %merge_665

then_666:                                         ; preds = %merge_665
  %"%1564.load" = load i64, ptr %val2, align 8
  store i64 %"%1564.load", ptr %"%pci_class_30", align 8
  br label %merge_667

then_668:                                         ; preds = %merge_667
  %"%1568.load" = load i64, ptr %val2, align 8
  store i64 %"%1568.load", ptr %"%pci_class_31", align 8
  br label %merge_669
}

define i64 @pci_get_bar0(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_670, label %merge_671

merge_671:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_672, label %merge_673

merge_673:                                        ; preds = %merge_671
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_674, label %merge_675

merge_675:                                        ; preds = %merge_673
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_676, label %merge_677

merge_677:                                        ; preds = %merge_675
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_678, label %merge_679

merge_679:                                        ; preds = %merge_677
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_680, label %merge_681

merge_681:                                        ; preds = %merge_679
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_682, label %merge_683

merge_683:                                        ; preds = %merge_681
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_684, label %merge_685

merge_685:                                        ; preds = %merge_683
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 8
  br i1 %cmpeqtmp24, label %then_686, label %merge_687

merge_687:                                        ; preds = %merge_685
  %"%idx.load26" = load i64, ptr %idx1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%idx.load26", 9
  br i1 %cmpeqtmp27, label %then_688, label %merge_689

merge_689:                                        ; preds = %merge_687
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 10
  br i1 %cmpeqtmp30, label %then_690, label %merge_691

merge_691:                                        ; preds = %merge_689
  %"%idx.load32" = load i64, ptr %idx1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%idx.load32", 11
  br i1 %cmpeqtmp33, label %then_692, label %merge_693

merge_693:                                        ; preds = %merge_691
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 12
  br i1 %cmpeqtmp36, label %then_694, label %merge_695

merge_695:                                        ; preds = %merge_693
  %"%idx.load38" = load i64, ptr %idx1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%idx.load38", 13
  br i1 %cmpeqtmp39, label %then_696, label %merge_697

merge_697:                                        ; preds = %merge_695
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 14
  br i1 %cmpeqtmp42, label %then_698, label %merge_699

merge_699:                                        ; preds = %merge_697
  %"%idx.load44" = load i64, ptr %idx1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%idx.load44", 15
  br i1 %cmpeqtmp45, label %then_700, label %merge_701

merge_701:                                        ; preds = %merge_699
  %"%idx.load47" = load i64, ptr %idx1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%idx.load47", 16
  br i1 %cmpeqtmp48, label %then_702, label %merge_703

merge_703:                                        ; preds = %merge_701
  %"%idx.load50" = load i64, ptr %idx1, align 8
  %cmpeqtmp51 = icmp eq i64 %"%idx.load50", 17
  br i1 %cmpeqtmp51, label %then_704, label %merge_705

merge_705:                                        ; preds = %merge_703
  %"%idx.load53" = load i64, ptr %idx1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%idx.load53", 18
  br i1 %cmpeqtmp54, label %then_706, label %merge_707

merge_707:                                        ; preds = %merge_705
  %"%idx.load56" = load i64, ptr %idx1, align 8
  %cmpeqtmp57 = icmp eq i64 %"%idx.load56", 19
  br i1 %cmpeqtmp57, label %then_708, label %merge_709

merge_709:                                        ; preds = %merge_707
  %"%idx.load59" = load i64, ptr %idx1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%idx.load59", 20
  br i1 %cmpeqtmp60, label %then_710, label %merge_711

merge_711:                                        ; preds = %merge_709
  %"%idx.load62" = load i64, ptr %idx1, align 8
  %cmpeqtmp63 = icmp eq i64 %"%idx.load62", 21
  br i1 %cmpeqtmp63, label %then_712, label %merge_713

merge_713:                                        ; preds = %merge_711
  %"%idx.load65" = load i64, ptr %idx1, align 8
  %cmpeqtmp66 = icmp eq i64 %"%idx.load65", 22
  br i1 %cmpeqtmp66, label %then_714, label %merge_715

merge_715:                                        ; preds = %merge_713
  %"%idx.load68" = load i64, ptr %idx1, align 8
  %cmpeqtmp69 = icmp eq i64 %"%idx.load68", 23
  br i1 %cmpeqtmp69, label %then_716, label %merge_717

merge_717:                                        ; preds = %merge_715
  %"%idx.load71" = load i64, ptr %idx1, align 8
  %cmpeqtmp72 = icmp eq i64 %"%idx.load71", 24
  br i1 %cmpeqtmp72, label %then_718, label %merge_719

merge_719:                                        ; preds = %merge_717
  %"%idx.load74" = load i64, ptr %idx1, align 8
  %cmpeqtmp75 = icmp eq i64 %"%idx.load74", 25
  br i1 %cmpeqtmp75, label %then_720, label %merge_721

merge_721:                                        ; preds = %merge_719
  %"%idx.load77" = load i64, ptr %idx1, align 8
  %cmpeqtmp78 = icmp eq i64 %"%idx.load77", 26
  br i1 %cmpeqtmp78, label %then_722, label %merge_723

merge_723:                                        ; preds = %merge_721
  %"%idx.load80" = load i64, ptr %idx1, align 8
  %cmpeqtmp81 = icmp eq i64 %"%idx.load80", 27
  br i1 %cmpeqtmp81, label %then_724, label %merge_725

merge_725:                                        ; preds = %merge_723
  %"%idx.load83" = load i64, ptr %idx1, align 8
  %cmpeqtmp84 = icmp eq i64 %"%idx.load83", 28
  br i1 %cmpeqtmp84, label %then_726, label %merge_727

merge_727:                                        ; preds = %merge_725
  %"%idx.load86" = load i64, ptr %idx1, align 8
  %cmpeqtmp87 = icmp eq i64 %"%idx.load86", 29
  br i1 %cmpeqtmp87, label %then_728, label %merge_729

merge_729:                                        ; preds = %merge_727
  %"%idx.load89" = load i64, ptr %idx1, align 8
  %cmpeqtmp90 = icmp eq i64 %"%idx.load89", 30
  br i1 %cmpeqtmp90, label %then_730, label %merge_731

merge_731:                                        ; preds = %merge_729
  %"%idx.load92" = load i64, ptr %idx1, align 8
  %cmpeqtmp93 = icmp eq i64 %"%idx.load92", 31
  br i1 %cmpeqtmp93, label %then_732, label %merge_733

merge_733:                                        ; preds = %merge_731
  ret i64 0

then_670:                                         ; preds = %entry
  %loadtmp = load i64, ptr @pci_bar0_0, align 8
  ret i64 %loadtmp

then_672:                                         ; preds = %merge_671
  %loadtmp4 = load i64, ptr @pci_bar0_1, align 8
  ret i64 %loadtmp4

then_674:                                         ; preds = %merge_673
  %loadtmp7 = load i64, ptr @pci_bar0_2, align 8
  ret i64 %loadtmp7

then_676:                                         ; preds = %merge_675
  %loadtmp10 = load i64, ptr @pci_bar0_3, align 8
  ret i64 %loadtmp10

then_678:                                         ; preds = %merge_677
  %loadtmp13 = load i64, ptr @pci_bar0_4, align 8
  ret i64 %loadtmp13

then_680:                                         ; preds = %merge_679
  %loadtmp16 = load i64, ptr @pci_bar0_5, align 8
  ret i64 %loadtmp16

then_682:                                         ; preds = %merge_681
  %loadtmp19 = load i64, ptr @pci_bar0_6, align 8
  ret i64 %loadtmp19

then_684:                                         ; preds = %merge_683
  %loadtmp22 = load i64, ptr @pci_bar0_7, align 8
  ret i64 %loadtmp22

then_686:                                         ; preds = %merge_685
  %loadtmp25 = load i64, ptr @pci_bar0_8, align 8
  ret i64 %loadtmp25

then_688:                                         ; preds = %merge_687
  %loadtmp28 = load i64, ptr @pci_bar0_9, align 8
  ret i64 %loadtmp28

then_690:                                         ; preds = %merge_689
  %loadtmp31 = load i64, ptr @pci_bar0_10, align 8
  ret i64 %loadtmp31

then_692:                                         ; preds = %merge_691
  %loadtmp34 = load i64, ptr @pci_bar0_11, align 8
  ret i64 %loadtmp34

then_694:                                         ; preds = %merge_693
  %loadtmp37 = load i64, ptr @pci_bar0_12, align 8
  ret i64 %loadtmp37

then_696:                                         ; preds = %merge_695
  %loadtmp40 = load i64, ptr @pci_bar0_13, align 8
  ret i64 %loadtmp40

then_698:                                         ; preds = %merge_697
  %loadtmp43 = load i64, ptr @pci_bar0_14, align 8
  ret i64 %loadtmp43

then_700:                                         ; preds = %merge_699
  %loadtmp46 = load i64, ptr @pci_bar0_15, align 8
  ret i64 %loadtmp46

then_702:                                         ; preds = %merge_701
  %loadtmp49 = load i64, ptr @pci_bar0_16, align 8
  ret i64 %loadtmp49

then_704:                                         ; preds = %merge_703
  %loadtmp52 = load i64, ptr @pci_bar0_17, align 8
  ret i64 %loadtmp52

then_706:                                         ; preds = %merge_705
  %loadtmp55 = load i64, ptr @pci_bar0_18, align 8
  ret i64 %loadtmp55

then_708:                                         ; preds = %merge_707
  %loadtmp58 = load i64, ptr @pci_bar0_19, align 8
  ret i64 %loadtmp58

then_710:                                         ; preds = %merge_709
  %loadtmp61 = load i64, ptr @pci_bar0_20, align 8
  ret i64 %loadtmp61

then_712:                                         ; preds = %merge_711
  %loadtmp64 = load i64, ptr @pci_bar0_21, align 8
  ret i64 %loadtmp64

then_714:                                         ; preds = %merge_713
  %loadtmp67 = load i64, ptr @pci_bar0_22, align 8
  ret i64 %loadtmp67

then_716:                                         ; preds = %merge_715
  %loadtmp70 = load i64, ptr @pci_bar0_23, align 8
  ret i64 %loadtmp70

then_718:                                         ; preds = %merge_717
  %loadtmp73 = load i64, ptr @pci_bar0_24, align 8
  ret i64 %loadtmp73

then_720:                                         ; preds = %merge_719
  %loadtmp76 = load i64, ptr @pci_bar0_25, align 8
  ret i64 %loadtmp76

then_722:                                         ; preds = %merge_721
  %loadtmp79 = load i64, ptr @pci_bar0_26, align 8
  ret i64 %loadtmp79

then_724:                                         ; preds = %merge_723
  %loadtmp82 = load i64, ptr @pci_bar0_27, align 8
  ret i64 %loadtmp82

then_726:                                         ; preds = %merge_725
  %loadtmp85 = load i64, ptr @pci_bar0_28, align 8
  ret i64 %loadtmp85

then_728:                                         ; preds = %merge_727
  %loadtmp88 = load i64, ptr @pci_bar0_29, align 8
  ret i64 %loadtmp88

then_730:                                         ; preds = %merge_729
  %loadtmp91 = load i64, ptr @pci_bar0_30, align 8
  ret i64 %loadtmp91

then_732:                                         ; preds = %merge_731
  %loadtmp94 = load i64, ptr @pci_bar0_31, align 8
  ret i64 %loadtmp94
}

define i64 @pci_set_bar0(i64 %idx, i64 %val) {
entry:
  %"%pci_bar0_31" = alloca i64, align 8
  %"%pci_bar0_30" = alloca i64, align 8
  %"%pci_bar0_29" = alloca i64, align 8
  %"%pci_bar0_28" = alloca i64, align 8
  %"%pci_bar0_27" = alloca i64, align 8
  %"%pci_bar0_26" = alloca i64, align 8
  %"%pci_bar0_25" = alloca i64, align 8
  %"%pci_bar0_24" = alloca i64, align 8
  %"%pci_bar0_23" = alloca i64, align 8
  %"%pci_bar0_22" = alloca i64, align 8
  %"%pci_bar0_21" = alloca i64, align 8
  %"%pci_bar0_20" = alloca i64, align 8
  %"%pci_bar0_19" = alloca i64, align 8
  %"%pci_bar0_18" = alloca i64, align 8
  %"%pci_bar0_17" = alloca i64, align 8
  %"%pci_bar0_16" = alloca i64, align 8
  %"%pci_bar0_15" = alloca i64, align 8
  %"%pci_bar0_14" = alloca i64, align 8
  %"%pci_bar0_13" = alloca i64, align 8
  %"%pci_bar0_12" = alloca i64, align 8
  %"%pci_bar0_11" = alloca i64, align 8
  %"%pci_bar0_10" = alloca i64, align 8
  %"%pci_bar0_9" = alloca i64, align 8
  %"%pci_bar0_8" = alloca i64, align 8
  %"%pci_bar0_7" = alloca i64, align 8
  %"%pci_bar0_6" = alloca i64, align 8
  %"%pci_bar0_5" = alloca i64, align 8
  %"%pci_bar0_4" = alloca i64, align 8
  %"%pci_bar0_3" = alloca i64, align 8
  %"%pci_bar0_2" = alloca i64, align 8
  %"%pci_bar0_1" = alloca i64, align 8
  %"%pci_bar0_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_734, label %merge_735

merge_735:                                        ; preds = %then_734, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_736, label %merge_737

merge_737:                                        ; preds = %then_736, %merge_735
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_738, label %merge_739

merge_739:                                        ; preds = %then_738, %merge_737
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_740, label %merge_741

merge_741:                                        ; preds = %then_740, %merge_739
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_742, label %merge_743

merge_743:                                        ; preds = %then_742, %merge_741
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_744, label %merge_745

merge_745:                                        ; preds = %then_744, %merge_743
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_746, label %merge_747

merge_747:                                        ; preds = %then_746, %merge_745
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_748, label %merge_749

merge_749:                                        ; preds = %then_748, %merge_747
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_750, label %merge_751

merge_751:                                        ; preds = %then_750, %merge_749
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_752, label %merge_753

merge_753:                                        ; preds = %then_752, %merge_751
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_754, label %merge_755

merge_755:                                        ; preds = %then_754, %merge_753
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_756, label %merge_757

merge_757:                                        ; preds = %then_756, %merge_755
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_758, label %merge_759

merge_759:                                        ; preds = %then_758, %merge_757
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_760, label %merge_761

merge_761:                                        ; preds = %then_760, %merge_759
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_762, label %merge_763

merge_763:                                        ; preds = %then_762, %merge_761
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_764, label %merge_765

merge_765:                                        ; preds = %then_764, %merge_763
  %"%idx.load33" = load i64, ptr %idx1, align 8
  %cmpeqtmp34 = icmp eq i64 %"%idx.load33", 16
  br i1 %cmpeqtmp34, label %then_766, label %merge_767

merge_767:                                        ; preds = %then_766, %merge_765
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 17
  br i1 %cmpeqtmp36, label %then_768, label %merge_769

merge_769:                                        ; preds = %then_768, %merge_767
  %"%idx.load37" = load i64, ptr %idx1, align 8
  %cmpeqtmp38 = icmp eq i64 %"%idx.load37", 18
  br i1 %cmpeqtmp38, label %then_770, label %merge_771

merge_771:                                        ; preds = %then_770, %merge_769
  %"%idx.load39" = load i64, ptr %idx1, align 8
  %cmpeqtmp40 = icmp eq i64 %"%idx.load39", 19
  br i1 %cmpeqtmp40, label %then_772, label %merge_773

merge_773:                                        ; preds = %then_772, %merge_771
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 20
  br i1 %cmpeqtmp42, label %then_774, label %merge_775

merge_775:                                        ; preds = %then_774, %merge_773
  %"%idx.load43" = load i64, ptr %idx1, align 8
  %cmpeqtmp44 = icmp eq i64 %"%idx.load43", 21
  br i1 %cmpeqtmp44, label %then_776, label %merge_777

merge_777:                                        ; preds = %then_776, %merge_775
  %"%idx.load45" = load i64, ptr %idx1, align 8
  %cmpeqtmp46 = icmp eq i64 %"%idx.load45", 22
  br i1 %cmpeqtmp46, label %then_778, label %merge_779

merge_779:                                        ; preds = %then_778, %merge_777
  %"%idx.load47" = load i64, ptr %idx1, align 8
  %cmpeqtmp48 = icmp eq i64 %"%idx.load47", 23
  br i1 %cmpeqtmp48, label %then_780, label %merge_781

merge_781:                                        ; preds = %then_780, %merge_779
  %"%idx.load49" = load i64, ptr %idx1, align 8
  %cmpeqtmp50 = icmp eq i64 %"%idx.load49", 24
  br i1 %cmpeqtmp50, label %then_782, label %merge_783

merge_783:                                        ; preds = %then_782, %merge_781
  %"%idx.load51" = load i64, ptr %idx1, align 8
  %cmpeqtmp52 = icmp eq i64 %"%idx.load51", 25
  br i1 %cmpeqtmp52, label %then_784, label %merge_785

merge_785:                                        ; preds = %then_784, %merge_783
  %"%idx.load53" = load i64, ptr %idx1, align 8
  %cmpeqtmp54 = icmp eq i64 %"%idx.load53", 26
  br i1 %cmpeqtmp54, label %then_786, label %merge_787

merge_787:                                        ; preds = %then_786, %merge_785
  %"%idx.load55" = load i64, ptr %idx1, align 8
  %cmpeqtmp56 = icmp eq i64 %"%idx.load55", 27
  br i1 %cmpeqtmp56, label %then_788, label %merge_789

merge_789:                                        ; preds = %then_788, %merge_787
  %"%idx.load57" = load i64, ptr %idx1, align 8
  %cmpeqtmp58 = icmp eq i64 %"%idx.load57", 28
  br i1 %cmpeqtmp58, label %then_790, label %merge_791

merge_791:                                        ; preds = %then_790, %merge_789
  %"%idx.load59" = load i64, ptr %idx1, align 8
  %cmpeqtmp60 = icmp eq i64 %"%idx.load59", 29
  br i1 %cmpeqtmp60, label %then_792, label %merge_793

merge_793:                                        ; preds = %then_792, %merge_791
  %"%idx.load61" = load i64, ptr %idx1, align 8
  %cmpeqtmp62 = icmp eq i64 %"%idx.load61", 30
  br i1 %cmpeqtmp62, label %then_794, label %merge_795

merge_795:                                        ; preds = %then_794, %merge_793
  %"%idx.load63" = load i64, ptr %idx1, align 8
  %cmpeqtmp64 = icmp eq i64 %"%idx.load63", 31
  br i1 %cmpeqtmp64, label %then_796, label %merge_797

merge_797:                                        ; preds = %then_796, %merge_795
  ret i64 0

then_734:                                         ; preds = %entry
  %"%1734.load" = load i64, ptr %val2, align 8
  store i64 %"%1734.load", ptr %"%pci_bar0_0", align 8
  br label %merge_735

then_736:                                         ; preds = %merge_735
  %"%1738.load" = load i64, ptr %val2, align 8
  store i64 %"%1738.load", ptr %"%pci_bar0_1", align 8
  br label %merge_737

then_738:                                         ; preds = %merge_737
  %"%1742.load" = load i64, ptr %val2, align 8
  store i64 %"%1742.load", ptr %"%pci_bar0_2", align 8
  br label %merge_739

then_740:                                         ; preds = %merge_739
  %"%1746.load" = load i64, ptr %val2, align 8
  store i64 %"%1746.load", ptr %"%pci_bar0_3", align 8
  br label %merge_741

then_742:                                         ; preds = %merge_741
  %"%1750.load" = load i64, ptr %val2, align 8
  store i64 %"%1750.load", ptr %"%pci_bar0_4", align 8
  br label %merge_743

then_744:                                         ; preds = %merge_743
  %"%1754.load" = load i64, ptr %val2, align 8
  store i64 %"%1754.load", ptr %"%pci_bar0_5", align 8
  br label %merge_745

then_746:                                         ; preds = %merge_745
  %"%1758.load" = load i64, ptr %val2, align 8
  store i64 %"%1758.load", ptr %"%pci_bar0_6", align 8
  br label %merge_747

then_748:                                         ; preds = %merge_747
  %"%1762.load" = load i64, ptr %val2, align 8
  store i64 %"%1762.load", ptr %"%pci_bar0_7", align 8
  br label %merge_749

then_750:                                         ; preds = %merge_749
  %"%1766.load" = load i64, ptr %val2, align 8
  store i64 %"%1766.load", ptr %"%pci_bar0_8", align 8
  br label %merge_751

then_752:                                         ; preds = %merge_751
  %"%1770.load" = load i64, ptr %val2, align 8
  store i64 %"%1770.load", ptr %"%pci_bar0_9", align 8
  br label %merge_753

then_754:                                         ; preds = %merge_753
  %"%1774.load" = load i64, ptr %val2, align 8
  store i64 %"%1774.load", ptr %"%pci_bar0_10", align 8
  br label %merge_755

then_756:                                         ; preds = %merge_755
  %"%1778.load" = load i64, ptr %val2, align 8
  store i64 %"%1778.load", ptr %"%pci_bar0_11", align 8
  br label %merge_757

then_758:                                         ; preds = %merge_757
  %"%1782.load" = load i64, ptr %val2, align 8
  store i64 %"%1782.load", ptr %"%pci_bar0_12", align 8
  br label %merge_759

then_760:                                         ; preds = %merge_759
  %"%1786.load" = load i64, ptr %val2, align 8
  store i64 %"%1786.load", ptr %"%pci_bar0_13", align 8
  br label %merge_761

then_762:                                         ; preds = %merge_761
  %"%1790.load" = load i64, ptr %val2, align 8
  store i64 %"%1790.load", ptr %"%pci_bar0_14", align 8
  br label %merge_763

then_764:                                         ; preds = %merge_763
  %"%1794.load" = load i64, ptr %val2, align 8
  store i64 %"%1794.load", ptr %"%pci_bar0_15", align 8
  br label %merge_765

then_766:                                         ; preds = %merge_765
  %"%1798.load" = load i64, ptr %val2, align 8
  store i64 %"%1798.load", ptr %"%pci_bar0_16", align 8
  br label %merge_767

then_768:                                         ; preds = %merge_767
  %"%1802.load" = load i64, ptr %val2, align 8
  store i64 %"%1802.load", ptr %"%pci_bar0_17", align 8
  br label %merge_769

then_770:                                         ; preds = %merge_769
  %"%1806.load" = load i64, ptr %val2, align 8
  store i64 %"%1806.load", ptr %"%pci_bar0_18", align 8
  br label %merge_771

then_772:                                         ; preds = %merge_771
  %"%1810.load" = load i64, ptr %val2, align 8
  store i64 %"%1810.load", ptr %"%pci_bar0_19", align 8
  br label %merge_773

then_774:                                         ; preds = %merge_773
  %"%1814.load" = load i64, ptr %val2, align 8
  store i64 %"%1814.load", ptr %"%pci_bar0_20", align 8
  br label %merge_775

then_776:                                         ; preds = %merge_775
  %"%1818.load" = load i64, ptr %val2, align 8
  store i64 %"%1818.load", ptr %"%pci_bar0_21", align 8
  br label %merge_777

then_778:                                         ; preds = %merge_777
  %"%1822.load" = load i64, ptr %val2, align 8
  store i64 %"%1822.load", ptr %"%pci_bar0_22", align 8
  br label %merge_779

then_780:                                         ; preds = %merge_779
  %"%1826.load" = load i64, ptr %val2, align 8
  store i64 %"%1826.load", ptr %"%pci_bar0_23", align 8
  br label %merge_781

then_782:                                         ; preds = %merge_781
  %"%1830.load" = load i64, ptr %val2, align 8
  store i64 %"%1830.load", ptr %"%pci_bar0_24", align 8
  br label %merge_783

then_784:                                         ; preds = %merge_783
  %"%1834.load" = load i64, ptr %val2, align 8
  store i64 %"%1834.load", ptr %"%pci_bar0_25", align 8
  br label %merge_785

then_786:                                         ; preds = %merge_785
  %"%1838.load" = load i64, ptr %val2, align 8
  store i64 %"%1838.load", ptr %"%pci_bar0_26", align 8
  br label %merge_787

then_788:                                         ; preds = %merge_787
  %"%1842.load" = load i64, ptr %val2, align 8
  store i64 %"%1842.load", ptr %"%pci_bar0_27", align 8
  br label %merge_789

then_790:                                         ; preds = %merge_789
  %"%1846.load" = load i64, ptr %val2, align 8
  store i64 %"%1846.load", ptr %"%pci_bar0_28", align 8
  br label %merge_791

then_792:                                         ; preds = %merge_791
  %"%1850.load" = load i64, ptr %val2, align 8
  store i64 %"%1850.load", ptr %"%pci_bar0_29", align 8
  br label %merge_793

then_794:                                         ; preds = %merge_793
  %"%1854.load" = load i64, ptr %val2, align 8
  store i64 %"%1854.load", ptr %"%pci_bar0_30", align 8
  br label %merge_795

then_796:                                         ; preds = %merge_795
  %"%1858.load" = load i64, ptr %val2, align 8
  store i64 %"%1858.load", ptr %"%pci_bar0_31", align 8
  br label %merge_797
}

define i64 @pci_get_bar1(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_798, label %merge_799

merge_799:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_800, label %merge_801

merge_801:                                        ; preds = %merge_799
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_802, label %merge_803

merge_803:                                        ; preds = %merge_801
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_804, label %merge_805

merge_805:                                        ; preds = %merge_803
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_806, label %merge_807

merge_807:                                        ; preds = %merge_805
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_808, label %merge_809

merge_809:                                        ; preds = %merge_807
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_810, label %merge_811

merge_811:                                        ; preds = %merge_809
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_812, label %merge_813

merge_813:                                        ; preds = %merge_811
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 8
  br i1 %cmpeqtmp24, label %then_814, label %merge_815

merge_815:                                        ; preds = %merge_813
  %"%idx.load26" = load i64, ptr %idx1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%idx.load26", 9
  br i1 %cmpeqtmp27, label %then_816, label %merge_817

merge_817:                                        ; preds = %merge_815
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 10
  br i1 %cmpeqtmp30, label %then_818, label %merge_819

merge_819:                                        ; preds = %merge_817
  %"%idx.load32" = load i64, ptr %idx1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%idx.load32", 11
  br i1 %cmpeqtmp33, label %then_820, label %merge_821

merge_821:                                        ; preds = %merge_819
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 12
  br i1 %cmpeqtmp36, label %then_822, label %merge_823

merge_823:                                        ; preds = %merge_821
  %"%idx.load38" = load i64, ptr %idx1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%idx.load38", 13
  br i1 %cmpeqtmp39, label %then_824, label %merge_825

merge_825:                                        ; preds = %merge_823
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 14
  br i1 %cmpeqtmp42, label %then_826, label %merge_827

merge_827:                                        ; preds = %merge_825
  %"%idx.load44" = load i64, ptr %idx1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%idx.load44", 15
  br i1 %cmpeqtmp45, label %then_828, label %merge_829

merge_829:                                        ; preds = %merge_827
  ret i64 0

then_798:                                         ; preds = %entry
  %loadtmp = load i64, ptr @pci_bar1_0, align 8
  ret i64 %loadtmp

then_800:                                         ; preds = %merge_799
  %loadtmp4 = load i64, ptr @pci_bar1_1, align 8
  ret i64 %loadtmp4

then_802:                                         ; preds = %merge_801
  %loadtmp7 = load i64, ptr @pci_bar1_2, align 8
  ret i64 %loadtmp7

then_804:                                         ; preds = %merge_803
  %loadtmp10 = load i64, ptr @pci_bar1_3, align 8
  ret i64 %loadtmp10

then_806:                                         ; preds = %merge_805
  %loadtmp13 = load i64, ptr @pci_bar1_4, align 8
  ret i64 %loadtmp13

then_808:                                         ; preds = %merge_807
  %loadtmp16 = load i64, ptr @pci_bar1_5, align 8
  ret i64 %loadtmp16

then_810:                                         ; preds = %merge_809
  %loadtmp19 = load i64, ptr @pci_bar1_6, align 8
  ret i64 %loadtmp19

then_812:                                         ; preds = %merge_811
  %loadtmp22 = load i64, ptr @pci_bar1_7, align 8
  ret i64 %loadtmp22

then_814:                                         ; preds = %merge_813
  %loadtmp25 = load i64, ptr @pci_bar1_8, align 8
  ret i64 %loadtmp25

then_816:                                         ; preds = %merge_815
  %loadtmp28 = load i64, ptr @pci_bar1_9, align 8
  ret i64 %loadtmp28

then_818:                                         ; preds = %merge_817
  %loadtmp31 = load i64, ptr @pci_bar1_10, align 8
  ret i64 %loadtmp31

then_820:                                         ; preds = %merge_819
  %loadtmp34 = load i64, ptr @pci_bar1_11, align 8
  ret i64 %loadtmp34

then_822:                                         ; preds = %merge_821
  %loadtmp37 = load i64, ptr @pci_bar1_12, align 8
  ret i64 %loadtmp37

then_824:                                         ; preds = %merge_823
  %loadtmp40 = load i64, ptr @pci_bar1_13, align 8
  ret i64 %loadtmp40

then_826:                                         ; preds = %merge_825
  %loadtmp43 = load i64, ptr @pci_bar1_14, align 8
  ret i64 %loadtmp43

then_828:                                         ; preds = %merge_827
  %loadtmp46 = load i64, ptr @pci_bar1_15, align 8
  ret i64 %loadtmp46
}

define i64 @pci_set_bar1(i64 %idx, i64 %val) {
entry:
  %"%pci_bar1_15" = alloca i64, align 8
  %"%pci_bar1_14" = alloca i64, align 8
  %"%pci_bar1_13" = alloca i64, align 8
  %"%pci_bar1_12" = alloca i64, align 8
  %"%pci_bar1_11" = alloca i64, align 8
  %"%pci_bar1_10" = alloca i64, align 8
  %"%pci_bar1_9" = alloca i64, align 8
  %"%pci_bar1_8" = alloca i64, align 8
  %"%pci_bar1_7" = alloca i64, align 8
  %"%pci_bar1_6" = alloca i64, align 8
  %"%pci_bar1_5" = alloca i64, align 8
  %"%pci_bar1_4" = alloca i64, align 8
  %"%pci_bar1_3" = alloca i64, align 8
  %"%pci_bar1_2" = alloca i64, align 8
  %"%pci_bar1_1" = alloca i64, align 8
  %"%pci_bar1_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_830, label %merge_831

merge_831:                                        ; preds = %then_830, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_832, label %merge_833

merge_833:                                        ; preds = %then_832, %merge_831
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_834, label %merge_835

merge_835:                                        ; preds = %then_834, %merge_833
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_836, label %merge_837

merge_837:                                        ; preds = %then_836, %merge_835
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_838, label %merge_839

merge_839:                                        ; preds = %then_838, %merge_837
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_840, label %merge_841

merge_841:                                        ; preds = %then_840, %merge_839
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_842, label %merge_843

merge_843:                                        ; preds = %then_842, %merge_841
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_844, label %merge_845

merge_845:                                        ; preds = %then_844, %merge_843
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_846, label %merge_847

merge_847:                                        ; preds = %then_846, %merge_845
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_848, label %merge_849

merge_849:                                        ; preds = %then_848, %merge_847
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_850, label %merge_851

merge_851:                                        ; preds = %then_850, %merge_849
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_852, label %merge_853

merge_853:                                        ; preds = %then_852, %merge_851
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_854, label %merge_855

merge_855:                                        ; preds = %then_854, %merge_853
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_856, label %merge_857

merge_857:                                        ; preds = %then_856, %merge_855
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_858, label %merge_859

merge_859:                                        ; preds = %then_858, %merge_857
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_860, label %merge_861

merge_861:                                        ; preds = %then_860, %merge_859
  ret i64 0

then_830:                                         ; preds = %entry
  %"%1944.load" = load i64, ptr %val2, align 8
  store i64 %"%1944.load", ptr %"%pci_bar1_0", align 8
  br label %merge_831

then_832:                                         ; preds = %merge_831
  %"%1948.load" = load i64, ptr %val2, align 8
  store i64 %"%1948.load", ptr %"%pci_bar1_1", align 8
  br label %merge_833

then_834:                                         ; preds = %merge_833
  %"%1952.load" = load i64, ptr %val2, align 8
  store i64 %"%1952.load", ptr %"%pci_bar1_2", align 8
  br label %merge_835

then_836:                                         ; preds = %merge_835
  %"%1956.load" = load i64, ptr %val2, align 8
  store i64 %"%1956.load", ptr %"%pci_bar1_3", align 8
  br label %merge_837

then_838:                                         ; preds = %merge_837
  %"%1960.load" = load i64, ptr %val2, align 8
  store i64 %"%1960.load", ptr %"%pci_bar1_4", align 8
  br label %merge_839

then_840:                                         ; preds = %merge_839
  %"%1964.load" = load i64, ptr %val2, align 8
  store i64 %"%1964.load", ptr %"%pci_bar1_5", align 8
  br label %merge_841

then_842:                                         ; preds = %merge_841
  %"%1968.load" = load i64, ptr %val2, align 8
  store i64 %"%1968.load", ptr %"%pci_bar1_6", align 8
  br label %merge_843

then_844:                                         ; preds = %merge_843
  %"%1972.load" = load i64, ptr %val2, align 8
  store i64 %"%1972.load", ptr %"%pci_bar1_7", align 8
  br label %merge_845

then_846:                                         ; preds = %merge_845
  %"%1976.load" = load i64, ptr %val2, align 8
  store i64 %"%1976.load", ptr %"%pci_bar1_8", align 8
  br label %merge_847

then_848:                                         ; preds = %merge_847
  %"%1980.load" = load i64, ptr %val2, align 8
  store i64 %"%1980.load", ptr %"%pci_bar1_9", align 8
  br label %merge_849

then_850:                                         ; preds = %merge_849
  %"%1984.load" = load i64, ptr %val2, align 8
  store i64 %"%1984.load", ptr %"%pci_bar1_10", align 8
  br label %merge_851

then_852:                                         ; preds = %merge_851
  %"%1988.load" = load i64, ptr %val2, align 8
  store i64 %"%1988.load", ptr %"%pci_bar1_11", align 8
  br label %merge_853

then_854:                                         ; preds = %merge_853
  %"%1992.load" = load i64, ptr %val2, align 8
  store i64 %"%1992.load", ptr %"%pci_bar1_12", align 8
  br label %merge_855

then_856:                                         ; preds = %merge_855
  %"%1996.load" = load i64, ptr %val2, align 8
  store i64 %"%1996.load", ptr %"%pci_bar1_13", align 8
  br label %merge_857

then_858:                                         ; preds = %merge_857
  %"%2000.load" = load i64, ptr %val2, align 8
  store i64 %"%2000.load", ptr %"%pci_bar1_14", align 8
  br label %merge_859

then_860:                                         ; preds = %merge_859
  %"%2004.load" = load i64, ptr %val2, align 8
  store i64 %"%2004.load", ptr %"%pci_bar1_15", align 8
  br label %merge_861
}

define i64 @pci_get_irq(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_862, label %merge_863

merge_863:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_864, label %merge_865

merge_865:                                        ; preds = %merge_863
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_866, label %merge_867

merge_867:                                        ; preds = %merge_865
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_868, label %merge_869

merge_869:                                        ; preds = %merge_867
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_870, label %merge_871

merge_871:                                        ; preds = %merge_869
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_872, label %merge_873

merge_873:                                        ; preds = %merge_871
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_874, label %merge_875

merge_875:                                        ; preds = %merge_873
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_876, label %merge_877

merge_877:                                        ; preds = %merge_875
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 8
  br i1 %cmpeqtmp24, label %then_878, label %merge_879

merge_879:                                        ; preds = %merge_877
  %"%idx.load26" = load i64, ptr %idx1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%idx.load26", 9
  br i1 %cmpeqtmp27, label %then_880, label %merge_881

merge_881:                                        ; preds = %merge_879
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 10
  br i1 %cmpeqtmp30, label %then_882, label %merge_883

merge_883:                                        ; preds = %merge_881
  %"%idx.load32" = load i64, ptr %idx1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%idx.load32", 11
  br i1 %cmpeqtmp33, label %then_884, label %merge_885

merge_885:                                        ; preds = %merge_883
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 12
  br i1 %cmpeqtmp36, label %then_886, label %merge_887

merge_887:                                        ; preds = %merge_885
  %"%idx.load38" = load i64, ptr %idx1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%idx.load38", 13
  br i1 %cmpeqtmp39, label %then_888, label %merge_889

merge_889:                                        ; preds = %merge_887
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 14
  br i1 %cmpeqtmp42, label %then_890, label %merge_891

merge_891:                                        ; preds = %merge_889
  %"%idx.load44" = load i64, ptr %idx1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%idx.load44", 15
  br i1 %cmpeqtmp45, label %then_892, label %merge_893

merge_893:                                        ; preds = %merge_891
  ret i64 0

then_862:                                         ; preds = %entry
  %loadtmp = load i64, ptr @pci_irq_0, align 8
  ret i64 %loadtmp

then_864:                                         ; preds = %merge_863
  %loadtmp4 = load i64, ptr @pci_irq_1, align 8
  ret i64 %loadtmp4

then_866:                                         ; preds = %merge_865
  %loadtmp7 = load i64, ptr @pci_irq_2, align 8
  ret i64 %loadtmp7

then_868:                                         ; preds = %merge_867
  %loadtmp10 = load i64, ptr @pci_irq_3, align 8
  ret i64 %loadtmp10

then_870:                                         ; preds = %merge_869
  %loadtmp13 = load i64, ptr @pci_irq_4, align 8
  ret i64 %loadtmp13

then_872:                                         ; preds = %merge_871
  %loadtmp16 = load i64, ptr @pci_irq_5, align 8
  ret i64 %loadtmp16

then_874:                                         ; preds = %merge_873
  %loadtmp19 = load i64, ptr @pci_irq_6, align 8
  ret i64 %loadtmp19

then_876:                                         ; preds = %merge_875
  %loadtmp22 = load i64, ptr @pci_irq_7, align 8
  ret i64 %loadtmp22

then_878:                                         ; preds = %merge_877
  %loadtmp25 = load i64, ptr @pci_irq_8, align 8
  ret i64 %loadtmp25

then_880:                                         ; preds = %merge_879
  %loadtmp28 = load i64, ptr @pci_irq_9, align 8
  ret i64 %loadtmp28

then_882:                                         ; preds = %merge_881
  %loadtmp31 = load i64, ptr @pci_irq_10, align 8
  ret i64 %loadtmp31

then_884:                                         ; preds = %merge_883
  %loadtmp34 = load i64, ptr @pci_irq_11, align 8
  ret i64 %loadtmp34

then_886:                                         ; preds = %merge_885
  %loadtmp37 = load i64, ptr @pci_irq_12, align 8
  ret i64 %loadtmp37

then_888:                                         ; preds = %merge_887
  %loadtmp40 = load i64, ptr @pci_irq_13, align 8
  ret i64 %loadtmp40

then_890:                                         ; preds = %merge_889
  %loadtmp43 = load i64, ptr @pci_irq_14, align 8
  ret i64 %loadtmp43

then_892:                                         ; preds = %merge_891
  %loadtmp46 = load i64, ptr @pci_irq_15, align 8
  ret i64 %loadtmp46
}

define i64 @pci_set_irq(i64 %idx, i64 %val) {
entry:
  %"%pci_irq_15" = alloca i64, align 8
  %"%pci_irq_14" = alloca i64, align 8
  %"%pci_irq_13" = alloca i64, align 8
  %"%pci_irq_12" = alloca i64, align 8
  %"%pci_irq_11" = alloca i64, align 8
  %"%pci_irq_10" = alloca i64, align 8
  %"%pci_irq_9" = alloca i64, align 8
  %"%pci_irq_8" = alloca i64, align 8
  %"%pci_irq_7" = alloca i64, align 8
  %"%pci_irq_6" = alloca i64, align 8
  %"%pci_irq_5" = alloca i64, align 8
  %"%pci_irq_4" = alloca i64, align 8
  %"%pci_irq_3" = alloca i64, align 8
  %"%pci_irq_2" = alloca i64, align 8
  %"%pci_irq_1" = alloca i64, align 8
  %"%pci_irq_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_894, label %merge_895

merge_895:                                        ; preds = %then_894, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_896, label %merge_897

merge_897:                                        ; preds = %then_896, %merge_895
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_898, label %merge_899

merge_899:                                        ; preds = %then_898, %merge_897
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_900, label %merge_901

merge_901:                                        ; preds = %then_900, %merge_899
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_902, label %merge_903

merge_903:                                        ; preds = %then_902, %merge_901
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_904, label %merge_905

merge_905:                                        ; preds = %then_904, %merge_903
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_906, label %merge_907

merge_907:                                        ; preds = %then_906, %merge_905
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_908, label %merge_909

merge_909:                                        ; preds = %then_908, %merge_907
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_910, label %merge_911

merge_911:                                        ; preds = %then_910, %merge_909
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_912, label %merge_913

merge_913:                                        ; preds = %then_912, %merge_911
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_914, label %merge_915

merge_915:                                        ; preds = %then_914, %merge_913
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_916, label %merge_917

merge_917:                                        ; preds = %then_916, %merge_915
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_918, label %merge_919

merge_919:                                        ; preds = %then_918, %merge_917
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_920, label %merge_921

merge_921:                                        ; preds = %then_920, %merge_919
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_922, label %merge_923

merge_923:                                        ; preds = %then_922, %merge_921
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_924, label %merge_925

merge_925:                                        ; preds = %then_924, %merge_923
  ret i64 0

then_894:                                         ; preds = %entry
  %"%2090.load" = load i64, ptr %val2, align 8
  store i64 %"%2090.load", ptr %"%pci_irq_0", align 8
  br label %merge_895

then_896:                                         ; preds = %merge_895
  %"%2094.load" = load i64, ptr %val2, align 8
  store i64 %"%2094.load", ptr %"%pci_irq_1", align 8
  br label %merge_897

then_898:                                         ; preds = %merge_897
  %"%2098.load" = load i64, ptr %val2, align 8
  store i64 %"%2098.load", ptr %"%pci_irq_2", align 8
  br label %merge_899

then_900:                                         ; preds = %merge_899
  %"%2102.load" = load i64, ptr %val2, align 8
  store i64 %"%2102.load", ptr %"%pci_irq_3", align 8
  br label %merge_901

then_902:                                         ; preds = %merge_901
  %"%2106.load" = load i64, ptr %val2, align 8
  store i64 %"%2106.load", ptr %"%pci_irq_4", align 8
  br label %merge_903

then_904:                                         ; preds = %merge_903
  %"%2110.load" = load i64, ptr %val2, align 8
  store i64 %"%2110.load", ptr %"%pci_irq_5", align 8
  br label %merge_905

then_906:                                         ; preds = %merge_905
  %"%2114.load" = load i64, ptr %val2, align 8
  store i64 %"%2114.load", ptr %"%pci_irq_6", align 8
  br label %merge_907

then_908:                                         ; preds = %merge_907
  %"%2118.load" = load i64, ptr %val2, align 8
  store i64 %"%2118.load", ptr %"%pci_irq_7", align 8
  br label %merge_909

then_910:                                         ; preds = %merge_909
  %"%2122.load" = load i64, ptr %val2, align 8
  store i64 %"%2122.load", ptr %"%pci_irq_8", align 8
  br label %merge_911

then_912:                                         ; preds = %merge_911
  %"%2126.load" = load i64, ptr %val2, align 8
  store i64 %"%2126.load", ptr %"%pci_irq_9", align 8
  br label %merge_913

then_914:                                         ; preds = %merge_913
  %"%2130.load" = load i64, ptr %val2, align 8
  store i64 %"%2130.load", ptr %"%pci_irq_10", align 8
  br label %merge_915

then_916:                                         ; preds = %merge_915
  %"%2134.load" = load i64, ptr %val2, align 8
  store i64 %"%2134.load", ptr %"%pci_irq_11", align 8
  br label %merge_917

then_918:                                         ; preds = %merge_917
  %"%2138.load" = load i64, ptr %val2, align 8
  store i64 %"%2138.load", ptr %"%pci_irq_12", align 8
  br label %merge_919

then_920:                                         ; preds = %merge_919
  %"%2142.load" = load i64, ptr %val2, align 8
  store i64 %"%2142.load", ptr %"%pci_irq_13", align 8
  br label %merge_921

then_922:                                         ; preds = %merge_921
  %"%2146.load" = load i64, ptr %val2, align 8
  store i64 %"%2146.load", ptr %"%pci_irq_14", align 8
  br label %merge_923

then_924:                                         ; preds = %merge_923
  %"%2150.load" = load i64, ptr %val2, align 8
  store i64 %"%2150.load", ptr %"%pci_irq_15", align 8
  br label %merge_925
}

define i64 @pci_config_address(i64 %bus, i64 %dev, i64 %func, i64 %reg) {
entry:
  %"%reg_aligned" = alloca i64, align 8
  %"%addr" = alloca i64, align 8
  %reg4 = alloca i64, align 8
  store i64 %reg, ptr %reg4, align 8
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  store i64 2147483648, ptr %"%addr", align 8
  %"%bus.load" = load i64, ptr %bus1, align 8
  %shltmp = shl i64 %"%bus.load", 16
  %"%addr.load" = load i64, ptr %"%addr", align 8
  %addtmp = add i64 %"%addr.load", %shltmp
  store i64 %addtmp, ptr %"%addr", align 8
  %"%dev.load" = load i64, ptr %dev2, align 8
  %shltmp5 = shl i64 %"%dev.load", 11
  %"%addr.load6" = load i64, ptr %"%addr", align 8
  %addtmp7 = add i64 %"%addr.load6", %shltmp5
  store i64 %addtmp7, ptr %"%addr", align 8
  %"%func.load" = load i64, ptr %func3, align 8
  %shltmp8 = shl i64 %"%func.load", 8
  %"%addr.load9" = load i64, ptr %"%addr", align 8
  %addtmp10 = add i64 %"%addr.load9", %shltmp8
  store i64 %addtmp10, ptr %"%addr", align 8
  %"%reg.load" = load i64, ptr %reg4, align 8
  %shrtmp = lshr i64 %"%reg.load", 2
  %shltmp11 = shl i64 %shrtmp, 2
  store i64 %shltmp11, ptr %"%reg_aligned", align 8
  %"%addr.load12" = load i64, ptr %"%addr", align 8
  %"%reg_aligned.load" = load i64, ptr %"%reg_aligned", align 8
  %addtmp13 = add i64 %"%addr.load12", %"%reg_aligned.load"
  store i64 %addtmp13, ptr %"%addr", align 8
  %loadtmp = load i64, ptr %"%addr", align 8
  ret i64 %loadtmp
}

define i64 @pci_read32(i64 %bus, i64 %dev, i64 %func, i64 %reg) {
entry:
  %"%val" = alloca i64, align 8
  %"%addr" = alloca i64, align 8
  %reg4 = alloca i64, align 8
  store i64 %reg, ptr %reg4, align 8
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2173.load" = load i64, ptr %bus1, align 8
  %"%2175.load" = load i64, ptr %dev2, align 8
  %"%2177.load" = load i64, ptr %func3, align 8
  %"%2179.load" = load i64, ptr %reg4, align 8
  %calltmp = call i64 @pci_config_address(i64 %"%2173.load", i64 %"%2175.load", i64 %"%2177.load", i64 %"%2179.load")
  store i64 %calltmp, ptr %"%addr", align 8
  %"%PCI_CONFIG_ADDR.load" = load i64, ptr @PCI_CONFIG_ADDR, align 8
  %"%addr.load" = load i64, ptr %"%addr", align 8
  call void @sad_ll_port_outl(i64 %"%PCI_CONFIG_ADDR.load", i64 %"%addr.load")
  %"%PCI_CONFIG_DATA.load" = load i64, ptr @PCI_CONFIG_DATA, align 8
  call void @sad_ll_port_inl(i64 %"%PCI_CONFIG_DATA.load")
  store i64 0, ptr %"%val", align 8
  %loadtmp = load i64, ptr %"%val", align 8
  ret i64 %loadtmp
}

define i64 @pci_write32(i64 %bus, i64 %dev, i64 %func, i64 %reg, i64 %val) {
entry:
  %"%addr" = alloca i64, align 8
  %val5 = alloca i64, align 8
  store i64 %val, ptr %val5, align 8
  %reg4 = alloca i64, align 8
  store i64 %reg, ptr %reg4, align 8
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2191.load" = load i64, ptr %bus1, align 8
  %"%2193.load" = load i64, ptr %dev2, align 8
  %"%2195.load" = load i64, ptr %func3, align 8
  %"%2197.load" = load i64, ptr %reg4, align 8
  %calltmp = call i64 @pci_config_address(i64 %"%2191.load", i64 %"%2193.load", i64 %"%2195.load", i64 %"%2197.load")
  store i64 %calltmp, ptr %"%addr", align 8
  %"%PCI_CONFIG_ADDR.load" = load i64, ptr @PCI_CONFIG_ADDR, align 8
  %"%addr.load" = load i64, ptr %"%addr", align 8
  call void @sad_ll_port_outl(i64 %"%PCI_CONFIG_ADDR.load", i64 %"%addr.load")
  %"%PCI_CONFIG_DATA.load" = load i64, ptr @PCI_CONFIG_DATA, align 8
  %"%2203.load" = load i64, ptr %val5, align 8
  call void @sad_ll_port_outl(i64 %"%PCI_CONFIG_DATA.load", i64 %"%2203.load")
  ret i64 0
}

define i64 @pci_read16(i64 %bus, i64 %dev, i64 %func, i64 %reg) {
entry:
  %"%shift" = alloca i64, align 8
  %"%val32" = alloca i64, align 8
  %reg4 = alloca i64, align 8
  store i64 %reg, ptr %reg4, align 8
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2207.load" = load i64, ptr %bus1, align 8
  %"%2209.load" = load i64, ptr %dev2, align 8
  %"%2211.load" = load i64, ptr %func3, align 8
  %"%2213.load" = load i64, ptr %reg4, align 8
  %calltmp = call i64 @pci_read32(i64 %"%2207.load", i64 %"%2209.load", i64 %"%2211.load", i64 %"%2213.load")
  store i64 %calltmp, ptr %"%val32", align 8
  %"%reg.load" = load i64, ptr %reg4, align 8
  %andtmp = and i64 %"%reg.load", 3
  %shltmp = shl i64 %andtmp, 3
  store i64 %shltmp, ptr %"%shift", align 8
  %"%shift.load" = load i64, ptr %"%shift", align 8
  %cmpeqtmp = icmp eq i64 %"%shift.load", 0
  br i1 %cmpeqtmp, label %then_926, label %merge_927

merge_927:                                        ; preds = %entry
  %"%shift.load6" = load i64, ptr %"%shift", align 8
  %cmpeqtmp7 = icmp eq i64 %"%shift.load6", 16
  br i1 %cmpeqtmp7, label %then_928, label %merge_929

merge_929:                                        ; preds = %merge_927
  %"%val32.load9" = load i64, ptr %"%val32", align 8
  %andtmp10 = and i64 %"%val32.load9", 65535
  ret i64 %andtmp10

then_926:                                         ; preds = %entry
  %"%val32.load" = load i64, ptr %"%val32", align 8
  %andtmp5 = and i64 %"%val32.load", 65535
  ret i64 %andtmp5

then_928:                                         ; preds = %merge_927
  %"%val32.load8" = load i64, ptr %"%val32", align 8
  %shrtmp = lshr i64 %"%val32.load8", 16
  ret i64 %shrtmp
}

define i64 @pci_read8(i64 %bus, i64 %dev, i64 %func, i64 %reg) {
entry:
  %"%shift" = alloca i64, align 8
  %"%val32" = alloca i64, align 8
  %reg4 = alloca i64, align 8
  store i64 %reg, ptr %reg4, align 8
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2235.load" = load i64, ptr %bus1, align 8
  %"%2237.load" = load i64, ptr %dev2, align 8
  %"%2239.load" = load i64, ptr %func3, align 8
  %"%2241.load" = load i64, ptr %reg4, align 8
  %calltmp = call i64 @pci_read32(i64 %"%2235.load", i64 %"%2237.load", i64 %"%2239.load", i64 %"%2241.load")
  store i64 %calltmp, ptr %"%val32", align 8
  %"%reg.load" = load i64, ptr %reg4, align 8
  %andtmp = and i64 %"%reg.load", 3
  %shltmp = shl i64 %andtmp, 3
  store i64 %shltmp, ptr %"%shift", align 8
  %"%shift.load" = load i64, ptr %"%shift", align 8
  %cmpeqtmp = icmp eq i64 %"%shift.load", 0
  br i1 %cmpeqtmp, label %then_930, label %merge_931

merge_931:                                        ; preds = %entry
  %"%shift.load6" = load i64, ptr %"%shift", align 8
  %cmpeqtmp7 = icmp eq i64 %"%shift.load6", 8
  br i1 %cmpeqtmp7, label %then_932, label %merge_933

merge_933:                                        ; preds = %merge_931
  %"%shift.load10" = load i64, ptr %"%shift", align 8
  %cmpeqtmp11 = icmp eq i64 %"%shift.load10", 16
  br i1 %cmpeqtmp11, label %then_934, label %merge_935

merge_935:                                        ; preds = %merge_933
  %"%shift.load15" = load i64, ptr %"%shift", align 8
  %cmpeqtmp16 = icmp eq i64 %"%shift.load15", 24
  br i1 %cmpeqtmp16, label %then_936, label %merge_937

merge_937:                                        ; preds = %merge_935
  %"%val32.load20" = load i64, ptr %"%val32", align 8
  %andtmp21 = and i64 %"%val32.load20", 255
  ret i64 %andtmp21

then_930:                                         ; preds = %entry
  %"%val32.load" = load i64, ptr %"%val32", align 8
  %andtmp5 = and i64 %"%val32.load", 255
  ret i64 %andtmp5

then_932:                                         ; preds = %merge_931
  %"%val32.load8" = load i64, ptr %"%val32", align 8
  %shrtmp = lshr i64 %"%val32.load8", 8
  %andtmp9 = and i64 %shrtmp, 255
  ret i64 %andtmp9

then_934:                                         ; preds = %merge_933
  %"%val32.load12" = load i64, ptr %"%val32", align 8
  %shrtmp13 = lshr i64 %"%val32.load12", 16
  %andtmp14 = and i64 %shrtmp13, 255
  ret i64 %andtmp14

then_936:                                         ; preds = %merge_935
  %"%val32.load17" = load i64, ptr %"%val32", align 8
  %shrtmp18 = lshr i64 %"%val32.load17", 24
  %andtmp19 = and i64 %shrtmp18, 255
  ret i64 %andtmp19
}

define i64 @pci_write16(i64 %bus, i64 %dev, i64 %func, i64 %reg, i64 %val) {
entry:
  %"%shift" = alloca i64, align 8
  %"%val32" = alloca i64, align 8
  %val5 = alloca i64, align 8
  store i64 %val, ptr %val5, align 8
  %reg4 = alloca i64, align 8
  store i64 %reg, ptr %reg4, align 8
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2279.load" = load i64, ptr %bus1, align 8
  %"%2281.load" = load i64, ptr %dev2, align 8
  %"%2283.load" = load i64, ptr %func3, align 8
  %"%2285.load" = load i64, ptr %reg4, align 8
  %calltmp = call i64 @pci_read32(i64 %"%2279.load", i64 %"%2281.load", i64 %"%2283.load", i64 %"%2285.load")
  store i64 %calltmp, ptr %"%val32", align 8
  %"%reg.load" = load i64, ptr %reg4, align 8
  %andtmp = and i64 %"%reg.load", 3
  %shltmp = shl i64 %andtmp, 3
  store i64 %shltmp, ptr %"%shift", align 8
  %"%shift.load" = load i64, ptr %"%shift", align 8
  %cmpeqtmp = icmp eq i64 %"%shift.load", 0
  br i1 %cmpeqtmp, label %then_938, label %merge_939

merge_939:                                        ; preds = %then_938, %entry
  %"%shift.load8" = load i64, ptr %"%shift", align 8
  %cmpeqtmp9 = icmp eq i64 %"%shift.load8", 16
  br i1 %cmpeqtmp9, label %then_940, label %merge_941

merge_941:                                        ; preds = %then_940, %merge_939
  %"%2312.load" = load i64, ptr %bus1, align 8
  %"%2314.load" = load i64, ptr %dev2, align 8
  %"%2316.load" = load i64, ptr %func3, align 8
  %"%2318.load" = load i64, ptr %reg4, align 8
  %"%val32.load16" = load i64, ptr %"%val32", align 8
  %calltmp17 = call i64 @pci_write32(i64 %"%2312.load", i64 %"%2314.load", i64 %"%2316.load", i64 %"%2318.load", i64 %"%val32.load16")
  ret i64 0

then_938:                                         ; preds = %entry
  %"%val32.load" = load i64, ptr %"%val32", align 8
  %shrtmp = lshr i64 %"%val32.load", 16
  %shltmp6 = shl i64 %shrtmp, 16
  %"%val.load" = load i64, ptr %val5, align 8
  %andtmp7 = and i64 %"%val.load", 65535
  %addtmp = add i64 %shltmp6, %andtmp7
  store i64 %addtmp, ptr %"%val32", align 8
  br label %merge_939

then_940:                                         ; preds = %merge_939
  %"%val32.load10" = load i64, ptr %"%val32", align 8
  %andtmp11 = and i64 %"%val32.load10", 65535
  %"%val.load12" = load i64, ptr %val5, align 8
  %andtmp13 = and i64 %"%val.load12", 65535
  %shltmp14 = shl i64 %andtmp13, 16
  %addtmp15 = add i64 %andtmp11, %shltmp14
  store i64 %addtmp15, ptr %"%val32", align 8
  br label %merge_941
}

define i64 @pci_get_vendor_id(i64 %bus, i64 %dev, i64 %func) {
entry:
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2322.load" = load i64, ptr %bus1, align 8
  %"%2324.load" = load i64, ptr %dev2, align 8
  %"%2326.load" = load i64, ptr %func3, align 8
  %"%PCI_VENDOR_ID.load" = load i64, ptr @PCI_VENDOR_ID, align 8
  %calltmp = call i64 @pci_read16(i64 %"%2322.load", i64 %"%2324.load", i64 %"%2326.load", i64 %"%PCI_VENDOR_ID.load")
  ret i64 %calltmp
}

define i64 @pci_get_device_id(i64 %bus, i64 %dev, i64 %func) {
entry:
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2332.load" = load i64, ptr %bus1, align 8
  %"%2334.load" = load i64, ptr %dev2, align 8
  %"%2336.load" = load i64, ptr %func3, align 8
  %"%PCI_DEVICE_ID.load" = load i64, ptr @PCI_DEVICE_ID, align 8
  %calltmp = call i64 @pci_read16(i64 %"%2332.load", i64 %"%2334.load", i64 %"%2336.load", i64 %"%PCI_DEVICE_ID.load")
  ret i64 %calltmp
}

define i64 @pci_get_class_code(i64 %bus, i64 %dev, i64 %func) {
entry:
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2342.load" = load i64, ptr %bus1, align 8
  %"%2344.load" = load i64, ptr %dev2, align 8
  %"%2346.load" = load i64, ptr %func3, align 8
  %"%PCI_CLASS_CODE.load" = load i64, ptr @PCI_CLASS_CODE, align 8
  %calltmp = call i64 @pci_read8(i64 %"%2342.load", i64 %"%2344.load", i64 %"%2346.load", i64 %"%PCI_CLASS_CODE.load")
  ret i64 %calltmp
}

define i64 @pci_get_subclass_code(i64 %bus, i64 %dev, i64 %func) {
entry:
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2352.load" = load i64, ptr %bus1, align 8
  %"%2354.load" = load i64, ptr %dev2, align 8
  %"%2356.load" = load i64, ptr %func3, align 8
  %"%PCI_SUBCLASS.load" = load i64, ptr @PCI_SUBCLASS, align 8
  %calltmp = call i64 @pci_read8(i64 %"%2352.load", i64 %"%2354.load", i64 %"%2356.load", i64 %"%PCI_SUBCLASS.load")
  ret i64 %calltmp
}

define i64 @pci_get_prog_if(i64 %bus, i64 %dev, i64 %func) {
entry:
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2362.load" = load i64, ptr %bus1, align 8
  %"%2364.load" = load i64, ptr %dev2, align 8
  %"%2366.load" = load i64, ptr %func3, align 8
  %"%PCI_PROG_IF.load" = load i64, ptr @PCI_PROG_IF, align 8
  %calltmp = call i64 @pci_read8(i64 %"%2362.load", i64 %"%2364.load", i64 %"%2366.load", i64 %"%PCI_PROG_IF.load")
  ret i64 %calltmp
}

define i64 @pci_get_header_type(i64 %bus, i64 %dev, i64 %func) {
entry:
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2372.load" = load i64, ptr %bus1, align 8
  %"%2374.load" = load i64, ptr %dev2, align 8
  %"%2376.load" = load i64, ptr %func3, align 8
  %"%PCI_HEADER_TYPE.load" = load i64, ptr @PCI_HEADER_TYPE, align 8
  %calltmp = call i64 @pci_read8(i64 %"%2372.load", i64 %"%2374.load", i64 %"%2376.load", i64 %"%PCI_HEADER_TYPE.load")
  ret i64 %calltmp
}

define i64 @pci_get_irq_line(i64 %bus, i64 %dev, i64 %func) {
entry:
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2382.load" = load i64, ptr %bus1, align 8
  %"%2384.load" = load i64, ptr %dev2, align 8
  %"%2386.load" = load i64, ptr %func3, align 8
  %"%PCI_IRQ_LINE.load" = load i64, ptr @PCI_IRQ_LINE, align 8
  %calltmp = call i64 @pci_read8(i64 %"%2382.load", i64 %"%2384.load", i64 %"%2386.load", i64 %"%PCI_IRQ_LINE.load")
  ret i64 %calltmp
}

define i64 @pci_get_bar(i64 %bus, i64 %dev, i64 %func, i64 %bar_num) {
entry:
  %"%bar_reg" = alloca i64, align 8
  %bar_num4 = alloca i64, align 8
  store i64 %bar_num, ptr %bar_num4, align 8
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%bar_num.load" = load i64, ptr %bar_num4, align 8
  %shltmp = shl i64 %"%bar_num.load", 2
  %"%PCI_BAR0.load" = load i64, ptr @PCI_BAR0, align 8
  %addtmp = add i64 %"%PCI_BAR0.load", %shltmp
  store i64 %addtmp, ptr %"%bar_reg", align 8
  %"%2395.load" = load i64, ptr %bus1, align 8
  %"%2397.load" = load i64, ptr %dev2, align 8
  %"%2399.load" = load i64, ptr %func3, align 8
  %"%bar_reg.load" = load i64, ptr %"%bar_reg", align 8
  %calltmp = call i64 @pci_read32(i64 %"%2395.load", i64 %"%2397.load", i64 %"%2399.load", i64 %"%bar_reg.load")
  ret i64 %calltmp
}

define i64 @pci_enable_bus_master(i64 %bus, i64 %dev, i64 %func) {
entry:
  %"%new_cmd" = alloca i64, align 8
  %"%cmd" = alloca i64, align 8
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2405.load" = load i64, ptr %bus1, align 8
  %"%2407.load" = load i64, ptr %dev2, align 8
  %"%2409.load" = load i64, ptr %func3, align 8
  %"%PCI_COMMAND.load" = load i64, ptr @PCI_COMMAND, align 8
  %calltmp = call i64 @pci_read16(i64 %"%2405.load", i64 %"%2407.load", i64 %"%2409.load", i64 %"%PCI_COMMAND.load")
  store i64 %calltmp, ptr %"%cmd", align 8
  %"%2414.load" = load i64, ptr %"%cmd", align 8
  store i64 %"%2414.load", ptr %"%new_cmd", align 8
  %"%new_cmd.load" = load i64, ptr %"%new_cmd", align 8
  %andtmp = and i64 %"%new_cmd.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_942, label %merge_943

merge_943:                                        ; preds = %then_942, %entry
  %"%new_cmd.load5" = load i64, ptr %"%new_cmd", align 8
  %shrtmp = lshr i64 %"%new_cmd.load5", 1
  %andtmp6 = and i64 %shrtmp, 1
  %cmpeqtmp7 = icmp eq i64 %andtmp6, 0
  br i1 %cmpeqtmp7, label %then_944, label %merge_945

merge_945:                                        ; preds = %then_944, %merge_943
  %"%new_cmd.load10" = load i64, ptr %"%new_cmd", align 8
  %shrtmp11 = lshr i64 %"%new_cmd.load10", 2
  %andtmp12 = and i64 %shrtmp11, 1
  %cmpeqtmp13 = icmp eq i64 %andtmp12, 0
  br i1 %cmpeqtmp13, label %then_946, label %merge_947

merge_947:                                        ; preds = %then_946, %merge_945
  %"%2435.load" = load i64, ptr %bus1, align 8
  %"%2437.load" = load i64, ptr %dev2, align 8
  %"%2439.load" = load i64, ptr %func3, align 8
  %"%PCI_COMMAND.load16" = load i64, ptr @PCI_COMMAND, align 8
  %"%new_cmd.load17" = load i64, ptr %"%new_cmd", align 8
  %calltmp18 = call i64 @pci_write16(i64 %"%2435.load", i64 %"%2437.load", i64 %"%2439.load", i64 %"%PCI_COMMAND.load16", i64 %"%new_cmd.load17")
  %loadtmp = load i64, ptr %"%new_cmd", align 8
  ret i64 %loadtmp

then_942:                                         ; preds = %entry
  %"%new_cmd.load4" = load i64, ptr %"%new_cmd", align 8
  %"%PCI_CMD_IO_ENABLE.load" = load i64, ptr @PCI_CMD_IO_ENABLE, align 8
  %addtmp = add i64 %"%new_cmd.load4", %"%PCI_CMD_IO_ENABLE.load"
  store i64 %addtmp, ptr %"%new_cmd", align 8
  br label %merge_943

then_944:                                         ; preds = %merge_943
  %"%new_cmd.load8" = load i64, ptr %"%new_cmd", align 8
  %"%PCI_CMD_MEM_ENABLE.load" = load i64, ptr @PCI_CMD_MEM_ENABLE, align 8
  %addtmp9 = add i64 %"%new_cmd.load8", %"%PCI_CMD_MEM_ENABLE.load"
  store i64 %addtmp9, ptr %"%new_cmd", align 8
  br label %merge_945

then_946:                                         ; preds = %merge_945
  %"%new_cmd.load14" = load i64, ptr %"%new_cmd", align 8
  %"%PCI_CMD_BUS_MASTER.load" = load i64, ptr @PCI_CMD_BUS_MASTER, align 8
  %addtmp15 = add i64 %"%new_cmd.load14", %"%PCI_CMD_BUS_MASTER.load"
  store i64 %addtmp15, ptr %"%new_cmd", align 8
  br label %merge_947
}

define i64 @pci_enable_io(i64 %bus, i64 %dev, i64 %func) {
entry:
  %"%cmd" = alloca i64, align 8
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2445.load" = load i64, ptr %bus1, align 8
  %"%2447.load" = load i64, ptr %dev2, align 8
  %"%2449.load" = load i64, ptr %func3, align 8
  %"%PCI_COMMAND.load" = load i64, ptr @PCI_COMMAND, align 8
  %calltmp = call i64 @pci_read16(i64 %"%2445.load", i64 %"%2447.load", i64 %"%2449.load", i64 %"%PCI_COMMAND.load")
  store i64 %calltmp, ptr %"%cmd", align 8
  %"%cmd.load" = load i64, ptr %"%cmd", align 8
  %andtmp = and i64 %"%cmd.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_948, label %merge_949

merge_949:                                        ; preds = %then_948, %entry
  %loadtmp = load i64, ptr %"%cmd", align 8
  ret i64 %loadtmp

then_948:                                         ; preds = %entry
  %"%cmd.load4" = load i64, ptr %"%cmd", align 8
  %"%PCI_CMD_IO_ENABLE.load" = load i64, ptr @PCI_CMD_IO_ENABLE, align 8
  %addtmp = add i64 %"%cmd.load4", %"%PCI_CMD_IO_ENABLE.load"
  store i64 %addtmp, ptr %"%cmd", align 8
  %"%2459.load" = load i64, ptr %bus1, align 8
  %"%2461.load" = load i64, ptr %dev2, align 8
  %"%2463.load" = load i64, ptr %func3, align 8
  %"%PCI_COMMAND.load5" = load i64, ptr @PCI_COMMAND, align 8
  %"%cmd.load6" = load i64, ptr %"%cmd", align 8
  %calltmp7 = call i64 @pci_write16(i64 %"%2459.load", i64 %"%2461.load", i64 %"%2463.load", i64 %"%PCI_COMMAND.load5", i64 %"%cmd.load6")
  br label %merge_949
}

define i64 @pci_enable_memory(i64 %bus, i64 %dev, i64 %func) {
entry:
  %"%cmd" = alloca i64, align 8
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2469.load" = load i64, ptr %bus1, align 8
  %"%2471.load" = load i64, ptr %dev2, align 8
  %"%2473.load" = load i64, ptr %func3, align 8
  %"%PCI_COMMAND.load" = load i64, ptr @PCI_COMMAND, align 8
  %calltmp = call i64 @pci_read16(i64 %"%2469.load", i64 %"%2471.load", i64 %"%2473.load", i64 %"%PCI_COMMAND.load")
  store i64 %calltmp, ptr %"%cmd", align 8
  %"%cmd.load" = load i64, ptr %"%cmd", align 8
  %shrtmp = lshr i64 %"%cmd.load", 1
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_950, label %merge_951

merge_951:                                        ; preds = %then_950, %entry
  %loadtmp = load i64, ptr %"%cmd", align 8
  ret i64 %loadtmp

then_950:                                         ; preds = %entry
  %"%cmd.load4" = load i64, ptr %"%cmd", align 8
  %"%PCI_CMD_MEM_ENABLE.load" = load i64, ptr @PCI_CMD_MEM_ENABLE, align 8
  %addtmp = add i64 %"%cmd.load4", %"%PCI_CMD_MEM_ENABLE.load"
  store i64 %addtmp, ptr %"%cmd", align 8
  %"%2485.load" = load i64, ptr %bus1, align 8
  %"%2487.load" = load i64, ptr %dev2, align 8
  %"%2489.load" = load i64, ptr %func3, align 8
  %"%PCI_COMMAND.load5" = load i64, ptr @PCI_COMMAND, align 8
  %"%cmd.load6" = load i64, ptr %"%cmd", align 8
  %calltmp7 = call i64 @pci_write16(i64 %"%2485.load", i64 %"%2487.load", i64 %"%2489.load", i64 %"%PCI_COMMAND.load5", i64 %"%cmd.load6")
  br label %merge_951
}

define i64 @pci_get_bar_size(i64 %bus, i64 %dev, i64 %func, i64 %bar_num) {
entry:
  %"%inverted" = alloca i64, align 8
  %"%size_mask" = alloca i64, align 8
  %"%original" = alloca i64, align 8
  %"%bar_reg" = alloca i64, align 8
  %bar_num4 = alloca i64, align 8
  store i64 %bar_num, ptr %bar_num4, align 8
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%bar_num.load" = load i64, ptr %bar_num4, align 8
  %shltmp = shl i64 %"%bar_num.load", 2
  %"%PCI_BAR0.load" = load i64, ptr @PCI_BAR0, align 8
  %addtmp = add i64 %"%PCI_BAR0.load", %shltmp
  store i64 %addtmp, ptr %"%bar_reg", align 8
  %"%2498.load" = load i64, ptr %bus1, align 8
  %"%2500.load" = load i64, ptr %dev2, align 8
  %"%2502.load" = load i64, ptr %func3, align 8
  %"%bar_reg.load" = load i64, ptr %"%bar_reg", align 8
  %calltmp = call i64 @pci_read32(i64 %"%2498.load", i64 %"%2500.load", i64 %"%2502.load", i64 %"%bar_reg.load")
  store i64 %calltmp, ptr %"%original", align 8
  %"%2498.load5" = load i64, ptr %bus1, align 8
  %"%2500.load6" = load i64, ptr %dev2, align 8
  %"%2502.load7" = load i64, ptr %func3, align 8
  %"%bar_reg.load8" = load i64, ptr %"%bar_reg", align 8
  %calltmp9 = call i64 @pci_write32(i64 %"%2498.load5", i64 %"%2500.load6", i64 %"%2502.load7", i64 %"%bar_reg.load8", i64 4294967295)
  %"%2498.load10" = load i64, ptr %bus1, align 8
  %"%2500.load11" = load i64, ptr %dev2, align 8
  %"%2502.load12" = load i64, ptr %func3, align 8
  %"%bar_reg.load13" = load i64, ptr %"%bar_reg", align 8
  %calltmp14 = call i64 @pci_read32(i64 %"%2498.load10", i64 %"%2500.load11", i64 %"%2502.load12", i64 %"%bar_reg.load13")
  store i64 %calltmp14, ptr %"%size_mask", align 8
  %"%2498.load15" = load i64, ptr %bus1, align 8
  %"%2500.load16" = load i64, ptr %dev2, align 8
  %"%2502.load17" = load i64, ptr %func3, align 8
  %"%bar_reg.load18" = load i64, ptr %"%bar_reg", align 8
  %"%original.load" = load i64, ptr %"%original", align 8
  %calltmp19 = call i64 @pci_write32(i64 %"%2498.load15", i64 %"%2500.load16", i64 %"%2502.load17", i64 %"%bar_reg.load18", i64 %"%original.load")
  %"%size_mask.load" = load i64, ptr %"%size_mask", align 8
  %cmpeqtmp = icmp eq i64 %"%size_mask.load", 0
  br i1 %cmpeqtmp, label %then_952, label %merge_953

else_956:                                         ; preds = %merge_953
  %"%size_mask.load24" = load i64, ptr %"%size_mask", align 8
  %shrtmp25 = lshr i64 %"%size_mask.load24", 4
  %shltmp26 = shl i64 %shrtmp25, 4
  store i64 %shltmp26, ptr %"%size_mask", align 8
  br label %merge_955

merge_953:                                        ; preds = %entry
  %"%original.load20" = load i64, ptr %"%original", align 8
  %andtmp = and i64 %"%original.load20", 1
  %cmpeqtmp21 = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp21, label %then_954, label %else_956

merge_955:                                        ; preds = %else_956, %then_954
  %"%size_mask.load27" = load i64, ptr %"%size_mask", align 8
  %subtmp = sub i64 4294967295, %"%size_mask.load27"
  store i64 %subtmp, ptr %"%inverted", align 8
  %"%inverted.load" = load i64, ptr %"%inverted", align 8
  %addtmp28 = add i64 %"%inverted.load", 1
  ret i64 %addtmp28

then_952:                                         ; preds = %entry
  ret i64 0

then_954:                                         ; preds = %merge_953
  %"%size_mask.load22" = load i64, ptr %"%size_mask", align 8
  %shrtmp = lshr i64 %"%size_mask.load22", 2
  %shltmp23 = shl i64 %shrtmp, 2
  store i64 %shltmp23, ptr %"%size_mask", align 8
  br label %merge_955
}

define i64 @pci_bar_is_io(i64 %bar_val) {
entry:
  %bar_val1 = alloca i64, align 8
  store i64 %bar_val, ptr %bar_val1, align 8
  %"%bar_val.load" = load i64, ptr %bar_val1, align 8
  %andtmp = and i64 %"%bar_val.load", 1
  ret i64 %andtmp
}

define i64 @pci_bar_address(i64 %bar_val) {
entry:
  %bar_val1 = alloca i64, align 8
  store i64 %bar_val, ptr %bar_val1, align 8
  %"%bar_val.load" = load i64, ptr %bar_val1, align 8
  %andtmp = and i64 %"%bar_val.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_957, label %else_959

else_959:                                         ; preds = %entry
  %"%bar_val.load3" = load i64, ptr %bar_val1, align 8
  %shrtmp4 = lshr i64 %"%bar_val.load3", 4
  %shltmp5 = shl i64 %shrtmp4, 4
  ret i64 %shltmp5

then_957:                                         ; preds = %entry
  %"%bar_val.load2" = load i64, ptr %bar_val1, align 8
  %shrtmp = lshr i64 %"%bar_val.load2", 2
  %shltmp = shl i64 %shrtmp, 2
  ret i64 %shltmp
}

define i64 @pci_classify_device(i64 %class_code) {
entry:
  %"%pci_usb_count" = alloca i64, align 8
  %"%sub" = alloca i64, align 8
  %"%cls" = alloca i64, align 8
  %"%tmp" = alloca i64, align 8
  %"%pci_bridge_count" = alloca i64, align 8
  %"%pci_audio_count" = alloca i64, align 8
  %"%pci_display_count" = alloca i64, align 8
  %"%pci_net_count" = alloca i64, align 8
  %"%pci_disk_count" = alloca i64, align 8
  %class_code1 = alloca i64, align 8
  store i64 %class_code, ptr %class_code1, align 8
  %"%class_code.load" = load i64, ptr %class_code1, align 8
  %"%PCI_CLASS_STORAGE.load" = load i64, ptr @PCI_CLASS_STORAGE, align 8
  %cmpeqtmp = icmp eq i64 %"%class_code.load", %"%PCI_CLASS_STORAGE.load"
  br i1 %cmpeqtmp, label %then_960, label %merge_961

merge_961:                                        ; preds = %then_960, %entry
  %"%class_code.load2" = load i64, ptr %class_code1, align 8
  %"%PCI_CLASS_NETWORK.load" = load i64, ptr @PCI_CLASS_NETWORK, align 8
  %cmpeqtmp3 = icmp eq i64 %"%class_code.load2", %"%PCI_CLASS_NETWORK.load"
  br i1 %cmpeqtmp3, label %then_962, label %merge_963

merge_963:                                        ; preds = %then_962, %merge_961
  %"%class_code.load5" = load i64, ptr %class_code1, align 8
  %"%PCI_CLASS_DISPLAY.load" = load i64, ptr @PCI_CLASS_DISPLAY, align 8
  %cmpeqtmp6 = icmp eq i64 %"%class_code.load5", %"%PCI_CLASS_DISPLAY.load"
  br i1 %cmpeqtmp6, label %then_964, label %merge_965

merge_965:                                        ; preds = %then_964, %merge_963
  %"%class_code.load8" = load i64, ptr %class_code1, align 8
  %"%PCI_CLASS_MULTIMEDIA.load" = load i64, ptr @PCI_CLASS_MULTIMEDIA, align 8
  %cmpeqtmp9 = icmp eq i64 %"%class_code.load8", %"%PCI_CLASS_MULTIMEDIA.load"
  br i1 %cmpeqtmp9, label %then_966, label %merge_967

merge_967:                                        ; preds = %then_966, %merge_965
  %"%class_code.load11" = load i64, ptr %class_code1, align 8
  %"%PCI_CLASS_BRIDGE.load" = load i64, ptr @PCI_CLASS_BRIDGE, align 8
  %cmpeqtmp12 = icmp eq i64 %"%class_code.load11", %"%PCI_CLASS_BRIDGE.load"
  br i1 %cmpeqtmp12, label %then_968, label %merge_969

merge_969:                                        ; preds = %then_968, %merge_967
  %"%class_code.load14" = load i64, ptr %class_code1, align 8
  %"%PCI_CLASS_SERIAL.load" = load i64, ptr @PCI_CLASS_SERIAL, align 8
  %cmpeqtmp15 = icmp eq i64 %"%class_code.load14", %"%PCI_CLASS_SERIAL.load"
  br i1 %cmpeqtmp15, label %then_970, label %merge_971

merge_971:                                        ; preds = %merge_973, %merge_969
  ret i64 0

merge_973:                                        ; preds = %merge_975, %then_970
  br label %merge_971

merge_975:                                        ; preds = %then_974, %then_972
  br label %merge_973

then_960:                                         ; preds = %entry
  %"%pci_disk_count.load" = load i64, ptr @pci_disk_count, align 8
  %addtmp = add i64 %"%pci_disk_count.load", 1
  store i64 %addtmp, ptr %"%pci_disk_count", align 8
  br label %merge_961

then_962:                                         ; preds = %merge_961
  %"%pci_net_count.load" = load i64, ptr @pci_net_count, align 8
  %addtmp4 = add i64 %"%pci_net_count.load", 1
  store i64 %addtmp4, ptr %"%pci_net_count", align 8
  br label %merge_963

then_964:                                         ; preds = %merge_963
  %"%pci_display_count.load" = load i64, ptr @pci_display_count, align 8
  %addtmp7 = add i64 %"%pci_display_count.load", 1
  store i64 %addtmp7, ptr %"%pci_display_count", align 8
  br label %merge_965

then_966:                                         ; preds = %merge_965
  %"%pci_audio_count.load" = load i64, ptr @pci_audio_count, align 8
  %addtmp10 = add i64 %"%pci_audio_count.load", 1
  store i64 %addtmp10, ptr %"%pci_audio_count", align 8
  br label %merge_967

then_968:                                         ; preds = %merge_967
  %"%pci_bridge_count.load" = load i64, ptr @pci_bridge_count, align 8
  %addtmp13 = add i64 %"%pci_bridge_count.load", 1
  store i64 %addtmp13, ptr %"%pci_bridge_count", align 8
  br label %merge_969

then_970:                                         ; preds = %merge_969
  %"%pci_dev_count.load" = load i64, ptr @pci_dev_count, align 8
  %subtmp = sub i64 %"%pci_dev_count.load", 1
  store i64 %subtmp, ptr %"%tmp", align 8
  %"%tmp.load" = load i64, ptr %"%tmp", align 8
  %cmpgetmp = icmp sge i64 %"%tmp.load", 0
  br i1 %cmpgetmp, label %then_972, label %merge_973

then_972:                                         ; preds = %then_970
  %"%tmp.load16" = load i64, ptr %"%tmp", align 8
  %calltmp = call i64 @pci_get_class(i64 %"%tmp.load16")
  store i64 %calltmp, ptr %"%cls", align 8
  %"%cls.load" = load i64, ptr %"%cls", align 8
  %shrtmp = lshr i64 %"%cls.load", 8
  %andtmp = and i64 %shrtmp, 255
  store i64 %andtmp, ptr %"%sub", align 8
  %"%sub.load" = load i64, ptr %"%sub", align 8
  %"%PCI_SUBCLASS_USB.load" = load i64, ptr @PCI_SUBCLASS_USB, align 8
  %cmpeqtmp17 = icmp eq i64 %"%sub.load", %"%PCI_SUBCLASS_USB.load"
  br i1 %cmpeqtmp17, label %then_974, label %merge_975

then_974:                                         ; preds = %then_972
  %"%pci_usb_count.load" = load i64, ptr @pci_usb_count, align 8
  %addtmp18 = add i64 %"%pci_usb_count.load", 1
  store i64 %addtmp18, ptr %"%pci_usb_count", align 8
  br label %merge_975
}

define i64 @pci_register_device(i64 %bus, i64 %dev, i64 %func, i64 %vid, i64 %did, i64 %class_full, i64 %bar0_val, i64 %bar1_val, i64 %irq_val) {
entry:
  %"%class_code" = alloca i64, align 8
  %"%pci_dev_count" = alloca i64, align 8
  %"%loc" = alloca i64, align 8
  %"%idx" = alloca i64, align 8
  %irq_val9 = alloca i64, align 8
  store i64 %irq_val, ptr %irq_val9, align 8
  %bar1_val8 = alloca i64, align 8
  store i64 %bar1_val, ptr %bar1_val8, align 8
  %bar0_val7 = alloca i64, align 8
  store i64 %bar0_val, ptr %bar0_val7, align 8
  %class_full6 = alloca i64, align 8
  store i64 %class_full, ptr %class_full6, align 8
  %did5 = alloca i64, align 8
  store i64 %did, ptr %did5, align 8
  %vid4 = alloca i64, align 8
  store i64 %vid, ptr %vid4, align 8
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2599.load" = load i64, ptr @pci_dev_count, align 8
  store i64 %"%2599.load", ptr %"%idx", align 8
  %"%idx.load" = load i64, ptr %"%idx", align 8
  %cmpgetmp = icmp sge i64 %"%idx.load", 64
  br i1 %cmpgetmp, label %then_976, label %merge_977

merge_977:                                        ; preds = %entry
  %"%bus.load" = load i64, ptr %bus1, align 8
  %shltmp = shl i64 %"%bus.load", 8
  %"%dev.load" = load i64, ptr %dev2, align 8
  %shltmp10 = shl i64 %"%dev.load", 3
  %addtmp = add i64 %shltmp, %shltmp10
  %"%func.load" = load i64, ptr %func3, align 8
  %addtmp11 = add i64 %addtmp, %"%func.load"
  store i64 %addtmp11, ptr %"%loc", align 8
  %"%idx.load12" = load i64, ptr %"%idx", align 8
  %"%loc.load" = load i64, ptr %"%loc", align 8
  %calltmp = call i64 @pci_set_loc(i64 %"%idx.load12", i64 %"%loc.load")
  %"%idx.load13" = load i64, ptr %"%idx", align 8
  %"%vid.load" = load i64, ptr %vid4, align 8
  %calltmp14 = call i64 @pci_set_vid(i64 %"%idx.load13", i64 %"%vid.load")
  %"%idx.load15" = load i64, ptr %"%idx", align 8
  %"%did.load" = load i64, ptr %did5, align 8
  %calltmp16 = call i64 @pci_set_did(i64 %"%idx.load15", i64 %"%did.load")
  %"%idx.load17" = load i64, ptr %"%idx", align 8
  %"%class_full.load" = load i64, ptr %class_full6, align 8
  %calltmp18 = call i64 @pci_set_class(i64 %"%idx.load17", i64 %"%class_full.load")
  %"%idx.load19" = load i64, ptr %"%idx", align 8
  %"%bar0_val.load" = load i64, ptr %bar0_val7, align 8
  %calltmp20 = call i64 @pci_set_bar0(i64 %"%idx.load19", i64 %"%bar0_val.load")
  %"%idx.load21" = load i64, ptr %"%idx", align 8
  %"%bar1_val.load" = load i64, ptr %bar1_val8, align 8
  %calltmp22 = call i64 @pci_set_bar1(i64 %"%idx.load21", i64 %"%bar1_val.load")
  %"%idx.load23" = load i64, ptr %"%idx", align 8
  %"%irq_val.load" = load i64, ptr %irq_val9, align 8
  %calltmp24 = call i64 @pci_set_irq(i64 %"%idx.load23", i64 %"%irq_val.load")
  %"%pci_dev_count.load" = load i64, ptr @pci_dev_count, align 8
  %addtmp25 = add i64 %"%pci_dev_count.load", 1
  store i64 %addtmp25, ptr %"%pci_dev_count", align 8
  %"%class_full.load26" = load i64, ptr %class_full6, align 8
  %shrtmp = lshr i64 %"%class_full.load26", 16
  store i64 %shrtmp, ptr %"%class_code", align 8
  %"%class_code.load" = load i64, ptr %"%class_code", align 8
  %calltmp27 = call i64 @pci_classify_device(i64 %"%class_code.load")
  %loadtmp = load i64, ptr %"%idx", align 8
  ret i64 %loadtmp

then_976:                                         ; preds = %entry
  ret i64 0
}

define i64 @pci_scan_function(i64 %bus, i64 %dev, i64 %func) {
entry:
  %"%class_full" = alloca i64, align 8
  %"%irq" = alloca i64, align 8
  %"%bar1" = alloca i64, align 8
  %"%bar0" = alloca i64, align 8
  %"%pif" = alloca i64, align 8
  %"%sub" = alloca i64, align 8
  %"%cls" = alloca i64, align 8
  %"%did" = alloca i64, align 8
  %"%vid" = alloca i64, align 8
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2625.load" = load i64, ptr %bus1, align 8
  %"%2627.load" = load i64, ptr %dev2, align 8
  %"%2629.load" = load i64, ptr %func3, align 8
  %calltmp = call i64 @pci_get_vendor_id(i64 %"%2625.load", i64 %"%2627.load", i64 %"%2629.load")
  store i64 %calltmp, ptr %"%vid", align 8
  %"%vid.load" = load i64, ptr %"%vid", align 8
  %"%PCI_VENDOR_INVALID.load" = load i64, ptr @PCI_VENDOR_INVALID, align 8
  %cmpeqtmp = icmp eq i64 %"%vid.load", %"%PCI_VENDOR_INVALID.load"
  br i1 %cmpeqtmp, label %then_978, label %merge_979

merge_979:                                        ; preds = %entry
  %"%vid.load4" = load i64, ptr %"%vid", align 8
  %cmpeqtmp5 = icmp eq i64 %"%vid.load4", 0
  br i1 %cmpeqtmp5, label %then_980, label %merge_981

merge_981:                                        ; preds = %merge_979
  %"%2639.load" = load i64, ptr %bus1, align 8
  %"%2641.load" = load i64, ptr %dev2, align 8
  %"%2643.load" = load i64, ptr %func3, align 8
  %calltmp6 = call i64 @pci_get_device_id(i64 %"%2639.load", i64 %"%2641.load", i64 %"%2643.load")
  store i64 %calltmp6, ptr %"%did", align 8
  %"%2639.load7" = load i64, ptr %bus1, align 8
  %"%2641.load8" = load i64, ptr %dev2, align 8
  %"%2643.load9" = load i64, ptr %func3, align 8
  %calltmp10 = call i64 @pci_get_class_code(i64 %"%2639.load7", i64 %"%2641.load8", i64 %"%2643.load9")
  store i64 %calltmp10, ptr %"%cls", align 8
  %"%2639.load11" = load i64, ptr %bus1, align 8
  %"%2641.load12" = load i64, ptr %dev2, align 8
  %"%2643.load13" = load i64, ptr %func3, align 8
  %calltmp14 = call i64 @pci_get_subclass_code(i64 %"%2639.load11", i64 %"%2641.load12", i64 %"%2643.load13")
  store i64 %calltmp14, ptr %"%sub", align 8
  %"%2639.load15" = load i64, ptr %bus1, align 8
  %"%2641.load16" = load i64, ptr %dev2, align 8
  %"%2643.load17" = load i64, ptr %func3, align 8
  %calltmp18 = call i64 @pci_get_prog_if(i64 %"%2639.load15", i64 %"%2641.load16", i64 %"%2643.load17")
  store i64 %calltmp18, ptr %"%pif", align 8
  %"%2639.load19" = load i64, ptr %bus1, align 8
  %"%2641.load20" = load i64, ptr %dev2, align 8
  %"%2643.load21" = load i64, ptr %func3, align 8
  %calltmp22 = call i64 @pci_get_bar(i64 %"%2639.load19", i64 %"%2641.load20", i64 %"%2643.load21", i64 0)
  store i64 %calltmp22, ptr %"%bar0", align 8
  %"%2639.load23" = load i64, ptr %bus1, align 8
  %"%2641.load24" = load i64, ptr %dev2, align 8
  %"%2643.load25" = load i64, ptr %func3, align 8
  %calltmp26 = call i64 @pci_get_bar(i64 %"%2639.load23", i64 %"%2641.load24", i64 %"%2643.load25", i64 1)
  store i64 %calltmp26, ptr %"%bar1", align 8
  %"%2639.load27" = load i64, ptr %bus1, align 8
  %"%2641.load28" = load i64, ptr %dev2, align 8
  %"%2643.load29" = load i64, ptr %func3, align 8
  %calltmp30 = call i64 @pci_get_irq_line(i64 %"%2639.load27", i64 %"%2641.load28", i64 %"%2643.load29")
  store i64 %calltmp30, ptr %"%irq", align 8
  %"%cls.load" = load i64, ptr %"%cls", align 8
  %shltmp = shl i64 %"%cls.load", 16
  %"%sub.load" = load i64, ptr %"%sub", align 8
  %shltmp31 = shl i64 %"%sub.load", 8
  %addtmp = add i64 %shltmp, %shltmp31
  %"%pif.load" = load i64, ptr %"%pif", align 8
  %addtmp32 = add i64 %addtmp, %"%pif.load"
  store i64 %addtmp32, ptr %"%class_full", align 8
  %"%2639.load33" = load i64, ptr %bus1, align 8
  %"%2641.load34" = load i64, ptr %dev2, align 8
  %"%2643.load35" = load i64, ptr %func3, align 8
  %"%2716.load" = load i64, ptr %"%vid", align 8
  %"%2718.load" = load i64, ptr %"%did", align 8
  %"%2720.load" = load i64, ptr %"%class_full", align 8
  %"%2722.load" = load i64, ptr %"%bar0", align 8
  %"%2724.load" = load i64, ptr %"%bar1", align 8
  %"%2726.load" = load i64, ptr %"%irq", align 8
  %calltmp36 = call i64 @pci_register_device(i64 %"%2639.load33", i64 %"%2641.load34", i64 %"%2643.load35", i64 %"%2716.load", i64 %"%2718.load", i64 %"%2720.load", i64 %"%2722.load", i64 %"%2724.load", i64 %"%2726.load")
  %calltmp37 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %"%2639.load38" = load i64, ptr %bus1, align 8
  %calltmp39 = call i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"%2639.load38")
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 58)
  %"%2641.load40" = load i64, ptr %dev2, align 8
  %calltmp41 = call i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"%2641.load40")
  %"%COM1.load42" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load42", i64 46)
  %"%2643.load43" = load i64, ptr %func3, align 8
  %calltmp44 = call i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"%2643.load43")
  %calltmp45 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %"%2716.load46" = load i64, ptr %"%vid", align 8
  %calltmp47 = call i64 @pci_serial_hex16(i64 %"%2716.load46")
  %calltmp48 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %"%2718.load49" = load i64, ptr %"%did", align 8
  %calltmp50 = call i64 @pci_serial_hex16(i64 %"%2718.load49")
  %calltmp51 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %"%2756.load" = load i64, ptr %"%cls", align 8
  %calltmp52 = call i64 @pci_serial_hex8(i64 %"%2756.load")
  %"%COM1.load53" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load53", i64 58)
  %"%2761.load" = load i64, ptr %"%sub", align 8
  %calltmp54 = call i64 @pci_serial_hex8(i64 %"%2761.load")
  %"%COM1.load55" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load55", i64 58)
  %"%2766.load" = load i64, ptr %"%pif", align 8
  %calltmp56 = call i64 @pci_serial_hex8(i64 %"%2766.load")
  %calltmp57 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%2722.load58" = load i64, ptr %"%bar0", align 8
  %calltmp59 = call i64 @pci_serial_hex32(i64 %"%2722.load58")
  %calltmp60 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %"%2726.load61" = load i64, ptr %"%irq", align 8
  %calltmp62 = call i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"%2726.load61")
  %calltmp63 = call i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1

then_978:                                         ; preds = %entry
  ret i64 0

then_980:                                         ; preds = %merge_979
  ret i64 0
}

define i64 @pci_scan_device(i64 %bus, i64 %dev) {
entry:
  %"%f" = alloca i64, align 8
  %"%header" = alloca i64, align 8
  %"%vid" = alloca i64, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2781.load" = load i64, ptr %bus1, align 8
  %"%2783.load" = load i64, ptr %dev2, align 8
  %calltmp = call i64 @pci_get_vendor_id(i64 %"%2781.load", i64 %"%2783.load", i64 0)
  store i64 %calltmp, ptr %"%vid", align 8
  %"%vid.load" = load i64, ptr %"%vid", align 8
  %"%PCI_VENDOR_INVALID.load" = load i64, ptr @PCI_VENDOR_INVALID, align 8
  %cmpeqtmp = icmp eq i64 %"%vid.load", %"%PCI_VENDOR_INVALID.load"
  br i1 %cmpeqtmp, label %then_982, label %merge_983

merge_983:                                        ; preds = %entry
  %"%vid.load3" = load i64, ptr %"%vid", align 8
  %cmpeqtmp4 = icmp eq i64 %"%vid.load3", 0
  br i1 %cmpeqtmp4, label %then_984, label %merge_985

merge_985:                                        ; preds = %merge_983
  %"%2794.load" = load i64, ptr %bus1, align 8
  %"%2796.load" = load i64, ptr %dev2, align 8
  %calltmp5 = call i64 @pci_scan_function(i64 %"%2794.load", i64 %"%2796.load", i64 0)
  %"%2794.load6" = load i64, ptr %bus1, align 8
  %"%2796.load7" = load i64, ptr %dev2, align 8
  %calltmp8 = call i64 @pci_get_header_type(i64 %"%2794.load6", i64 %"%2796.load7", i64 0)
  store i64 %calltmp8, ptr %"%header", align 8
  %"%header.load" = load i64, ptr %"%header", align 8
  %cmpgetmp = icmp sge i64 %"%header.load", 128
  br i1 %cmpgetmp, label %then_986, label %merge_987

merge_987:                                        ; preds = %while_exit_990, %merge_985
  ret i64 1

then_982:                                         ; preds = %entry
  ret i64 0

then_984:                                         ; preds = %merge_983
  ret i64 0

then_986:                                         ; preds = %merge_985
  store i64 1, ptr %"%f", align 8
  br label %while_cond_988

while_body_989:                                   ; preds = %while_cond_988
  %"%2814.load" = load i64, ptr %bus1, align 8
  %"%2816.load" = load i64, ptr %dev2, align 8
  %"%2818.load" = load i64, ptr %"%f", align 8
  %calltmp9 = call i64 @pci_scan_function(i64 %"%2814.load", i64 %"%2816.load", i64 %"%2818.load")
  %"%f.load10" = load i64, ptr %"%f", align 8
  %addtmp = add i64 %"%f.load10", 1
  store i64 %addtmp, ptr %"%f", align 8
  br label %while_cond_988

while_cond_988:                                   ; preds = %while_body_989, %then_986
  %"%f.load" = load i64, ptr %"%f", align 8
  %"%PCI_MAX_FUNC.load" = load i64, ptr @PCI_MAX_FUNC, align 8
  %cmplttmp = icmp slt i64 %"%f.load", %"%PCI_MAX_FUNC.load"
  br i1 %cmplttmp, label %while_body_989, label %while_exit_990

while_exit_990:                                   ; preds = %while_cond_988
  br label %merge_987
}

define i64 @pci_scan_bus(i64 %bus) {
entry:
  %"%dev" = alloca i64, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  store i64 0, ptr %"%dev", align 8
  br label %while_cond_991

while_body_992:                                   ; preds = %while_cond_991
  %"%2828.load" = load i64, ptr %bus1, align 8
  %"%2830.load" = load i64, ptr %"%dev", align 8
  %calltmp = call i64 @pci_scan_device(i64 %"%2828.load", i64 %"%2830.load")
  %"%dev.load2" = load i64, ptr %"%dev", align 8
  %addtmp = add i64 %"%dev.load2", 1
  store i64 %addtmp, ptr %"%dev", align 8
  br label %while_cond_991

while_cond_991:                                   ; preds = %while_body_992, %entry
  %"%dev.load" = load i64, ptr %"%dev", align 8
  %"%PCI_MAX_DEV.load" = load i64, ptr @PCI_MAX_DEV, align 8
  %cmplttmp = icmp slt i64 %"%dev.load", %"%PCI_MAX_DEV.load"
  br i1 %cmplttmp, label %while_body_992, label %while_exit_993

while_exit_993:                                   ; preds = %while_cond_991
  ret i64 0
}

define i64 @pci_scan_all() {
entry:
  %"%bus" = alloca i64, align 8
  %calltmp = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  %calltmp1 = call i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%bus", align 8
  br label %while_cond_994

while_body_995:                                   ; preds = %while_cond_994
  %"%2843.load" = load i64, ptr %"%bus", align 8
  %calltmp2 = call i64 @pci_scan_bus(i64 %"%2843.load")
  %"%bus.load3" = load i64, ptr %"%bus", align 8
  %addtmp = add i64 %"%bus.load3", 1
  store i64 %addtmp, ptr %"%bus", align 8
  br label %while_cond_994

while_cond_994:                                   ; preds = %while_body_995, %entry
  %"%bus.load" = load i64, ptr %"%bus", align 8
  %"%PCI_MAX_BUS.load" = load i64, ptr @PCI_MAX_BUS, align 8
  %cmplttmp = icmp slt i64 %"%bus.load", %"%PCI_MAX_BUS.load"
  br i1 %cmplttmp, label %while_body_995, label %while_exit_996

while_exit_996:                                   ; preds = %while_cond_994
  %calltmp4 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %"%2850.load" = load i64, ptr @pci_dev_count, align 8
  %calltmp5 = call i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"%2850.load")
  %calltmp6 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %calltmp7 = call i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr @pci_dev_count, align 8
  ret i64 %loadtmp
}

define i64 @pci_find_device_by_class(i64 %class_code, i64 %subclass) {
entry:
  %"%dev_sub" = alloca i64, align 8
  %"%dev_class" = alloca i64, align 8
  %"%cls" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %subclass2 = alloca i64, align 8
  store i64 %subclass, ptr %subclass2, align 8
  %class_code1 = alloca i64, align 8
  store i64 %class_code, ptr %class_code1, align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_997

merge_1001:                                       ; preds = %merge_1003, %while_body_998
  %"%i.load7" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load7", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_997

merge_1003:                                       ; preds = %then_1000
  br label %merge_1001

then_1000:                                        ; preds = %while_body_998
  %"%dev_sub.load" = load i64, ptr %"%dev_sub", align 8
  %"%subclass.load" = load i64, ptr %subclass2, align 8
  %cmpeqtmp6 = icmp eq i64 %"%dev_sub.load", %"%subclass.load"
  br i1 %cmpeqtmp6, label %then_1002, label %merge_1003

then_1002:                                        ; preds = %then_1000
  %loadtmp = load i64, ptr %"%i", align 8
  ret i64 %loadtmp

while_body_998:                                   ; preds = %while_cond_997
  %"%i.load3" = load i64, ptr %"%i", align 8
  %calltmp = call i64 @pci_get_class(i64 %"%i.load3")
  store i64 %calltmp, ptr %"%cls", align 8
  %"%cls.load" = load i64, ptr %"%cls", align 8
  %shrtmp = lshr i64 %"%cls.load", 16
  store i64 %shrtmp, ptr %"%dev_class", align 8
  %"%cls.load4" = load i64, ptr %"%cls", align 8
  %shrtmp5 = lshr i64 %"%cls.load4", 8
  %andtmp = and i64 %shrtmp5, 255
  store i64 %andtmp, ptr %"%dev_sub", align 8
  %"%dev_class.load" = load i64, ptr %"%dev_class", align 8
  %"%class_code.load" = load i64, ptr %class_code1, align 8
  %cmpeqtmp = icmp eq i64 %"%dev_class.load", %"%class_code.load"
  br i1 %cmpeqtmp, label %then_1000, label %merge_1001

while_cond_997:                                   ; preds = %merge_1001, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%pci_dev_count.load" = load i64, ptr @pci_dev_count, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%pci_dev_count.load"
  br i1 %cmplttmp, label %while_body_998, label %while_exit_999

while_exit_999:                                   ; preds = %while_cond_997
  ret i64 -1
}

define i64 @pci_find_device_by_class_if(i64 %class_code, i64 %subclass, i64 %prog_if) {
entry:
  %"%dev_pif" = alloca i64, align 8
  %"%dev_sub" = alloca i64, align 8
  %"%dev_class" = alloca i64, align 8
  %"%cls" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %prog_if3 = alloca i64, align 8
  store i64 %prog_if, ptr %prog_if3, align 8
  %subclass2 = alloca i64, align 8
  store i64 %subclass, ptr %subclass2, align 8
  %class_code1 = alloca i64, align 8
  store i64 %class_code, ptr %class_code1, align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_1004

merge_1008:                                       ; preds = %merge_1010, %while_body_1005
  %"%i.load10" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load10", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_1004

merge_1010:                                       ; preds = %merge_1012, %then_1007
  br label %merge_1008

merge_1012:                                       ; preds = %then_1009
  br label %merge_1010

then_1007:                                        ; preds = %while_body_1005
  %"%dev_sub.load" = load i64, ptr %"%dev_sub", align 8
  %"%subclass.load" = load i64, ptr %subclass2, align 8
  %cmpeqtmp9 = icmp eq i64 %"%dev_sub.load", %"%subclass.load"
  br i1 %cmpeqtmp9, label %then_1009, label %merge_1010

then_1009:                                        ; preds = %then_1007
  %"%dev_pif.load" = load i64, ptr %"%dev_pif", align 8
  %"%prog_if.load" = load i64, ptr %prog_if3, align 8
  %cmpeqtmp11 = icmp eq i64 %"%dev_pif.load", %"%prog_if.load"
  br i1 %cmpeqtmp11, label %then_1011, label %merge_1012

then_1011:                                        ; preds = %then_1009
  %loadtmp = load i64, ptr %"%i", align 8
  ret i64 %loadtmp

while_body_1005:                                  ; preds = %while_cond_1004
  %"%i.load4" = load i64, ptr %"%i", align 8
  %calltmp = call i64 @pci_get_class(i64 %"%i.load4")
  store i64 %calltmp, ptr %"%cls", align 8
  %"%cls.load" = load i64, ptr %"%cls", align 8
  %shrtmp = lshr i64 %"%cls.load", 16
  store i64 %shrtmp, ptr %"%dev_class", align 8
  %"%cls.load5" = load i64, ptr %"%cls", align 8
  %shrtmp6 = lshr i64 %"%cls.load5", 8
  %andtmp = and i64 %shrtmp6, 255
  store i64 %andtmp, ptr %"%dev_sub", align 8
  %"%cls.load7" = load i64, ptr %"%cls", align 8
  %andtmp8 = and i64 %"%cls.load7", 255
  store i64 %andtmp8, ptr %"%dev_pif", align 8
  %"%dev_class.load" = load i64, ptr %"%dev_class", align 8
  %"%class_code.load" = load i64, ptr %class_code1, align 8
  %cmpeqtmp = icmp eq i64 %"%dev_class.load", %"%class_code.load"
  br i1 %cmpeqtmp, label %then_1007, label %merge_1008

while_cond_1004:                                  ; preds = %merge_1008, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%pci_dev_count.load" = load i64, ptr @pci_dev_count, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%pci_dev_count.load"
  br i1 %cmplttmp, label %while_body_1005, label %while_exit_1006

while_exit_1006:                                  ; preds = %while_cond_1004
  ret i64 -1
}

define i64 @pci_find_device_by_id(i64 %vendor_id, i64 %device_id) {
entry:
  %"%did" = alloca i64, align 8
  %"%vid" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %device_id2 = alloca i64, align 8
  store i64 %device_id, ptr %device_id2, align 8
  %vendor_id1 = alloca i64, align 8
  store i64 %vendor_id, ptr %vendor_id1, align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_1013

merge_1017:                                       ; preds = %merge_1019, %while_body_1014
  %"%i.load7" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load7", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_1013

merge_1019:                                       ; preds = %then_1016
  br label %merge_1017

then_1016:                                        ; preds = %while_body_1014
  %"%did.load" = load i64, ptr %"%did", align 8
  %"%device_id.load" = load i64, ptr %device_id2, align 8
  %cmpeqtmp6 = icmp eq i64 %"%did.load", %"%device_id.load"
  br i1 %cmpeqtmp6, label %then_1018, label %merge_1019

then_1018:                                        ; preds = %then_1016
  %loadtmp = load i64, ptr %"%i", align 8
  ret i64 %loadtmp

while_body_1014:                                  ; preds = %while_cond_1013
  %"%i.load3" = load i64, ptr %"%i", align 8
  %calltmp = call i64 @pci_get_vid(i64 %"%i.load3")
  store i64 %calltmp, ptr %"%vid", align 8
  %"%i.load4" = load i64, ptr %"%i", align 8
  %calltmp5 = call i64 @pci_get_did(i64 %"%i.load4")
  store i64 %calltmp5, ptr %"%did", align 8
  %"%vid.load" = load i64, ptr %"%vid", align 8
  %"%vendor_id.load" = load i64, ptr %vendor_id1, align 8
  %cmpeqtmp = icmp eq i64 %"%vid.load", %"%vendor_id.load"
  br i1 %cmpeqtmp, label %then_1016, label %merge_1017

while_cond_1013:                                  ; preds = %merge_1017, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%pci_dev_count.load" = load i64, ptr @pci_dev_count, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%pci_dev_count.load"
  br i1 %cmplttmp, label %while_body_1014, label %while_exit_1015

while_exit_1015:                                  ; preds = %while_cond_1013
  ret i64 -1
}

define i64 @pci_find_usb_controller(i64 %search_index) {
entry:
  %"%dev_sub" = alloca i64, align 8
  %"%dev_class" = alloca i64, align 8
  %"%cls" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %"%found" = alloca i64, align 8
  %search_index1 = alloca i64, align 8
  store i64 %search_index, ptr %search_index1, align 8
  store i64 0, ptr %"%found", align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_1020

merge_1024:                                       ; preds = %merge_1026, %while_body_1021
  %"%i.load6" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load6", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_1020

merge_1026:                                       ; preds = %merge_1028, %then_1023
  br label %merge_1024

merge_1028:                                       ; preds = %then_1025
  %"%found.load8" = load i64, ptr %"%found", align 8
  %addtmp9 = add i64 %"%found.load8", 1
  store i64 %addtmp9, ptr %"%found", align 8
  br label %merge_1026

then_1023:                                        ; preds = %while_body_1021
  %"%dev_sub.load" = load i64, ptr %"%dev_sub", align 8
  %"%PCI_SUBCLASS_USB.load" = load i64, ptr @PCI_SUBCLASS_USB, align 8
  %cmpeqtmp5 = icmp eq i64 %"%dev_sub.load", %"%PCI_SUBCLASS_USB.load"
  br i1 %cmpeqtmp5, label %then_1025, label %merge_1026

then_1025:                                        ; preds = %then_1023
  %"%found.load" = load i64, ptr %"%found", align 8
  %"%search_index.load" = load i64, ptr %search_index1, align 8
  %cmpeqtmp7 = icmp eq i64 %"%found.load", %"%search_index.load"
  br i1 %cmpeqtmp7, label %then_1027, label %merge_1028

then_1027:                                        ; preds = %then_1025
  %loadtmp = load i64, ptr %"%i", align 8
  ret i64 %loadtmp

while_body_1021:                                  ; preds = %while_cond_1020
  %"%i.load2" = load i64, ptr %"%i", align 8
  %calltmp = call i64 @pci_get_class(i64 %"%i.load2")
  store i64 %calltmp, ptr %"%cls", align 8
  %"%cls.load" = load i64, ptr %"%cls", align 8
  %shrtmp = lshr i64 %"%cls.load", 16
  store i64 %shrtmp, ptr %"%dev_class", align 8
  %"%cls.load3" = load i64, ptr %"%cls", align 8
  %shrtmp4 = lshr i64 %"%cls.load3", 8
  %andtmp = and i64 %shrtmp4, 255
  store i64 %andtmp, ptr %"%dev_sub", align 8
  %"%dev_class.load" = load i64, ptr %"%dev_class", align 8
  %"%PCI_CLASS_SERIAL.load" = load i64, ptr @PCI_CLASS_SERIAL, align 8
  %cmpeqtmp = icmp eq i64 %"%dev_class.load", %"%PCI_CLASS_SERIAL.load"
  br i1 %cmpeqtmp, label %then_1023, label %merge_1024

while_cond_1020:                                  ; preds = %merge_1024, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%pci_dev_count.load" = load i64, ptr @pci_dev_count, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%pci_dev_count.load"
  br i1 %cmplttmp, label %while_body_1021, label %while_exit_1022

while_exit_1022:                                  ; preds = %while_cond_1020
  ret i64 -1
}

define i64 @pci_get_usb_type(i64 %idx) {
entry:
  %"%cls" = alloca i64, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmplttmp = icmp slt i64 %"%idx.load", 0
  br i1 %cmplttmp, label %then_1029, label %merge_1030

merge_1030:                                       ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %"%pci_dev_count.load" = load i64, ptr @pci_dev_count, align 8
  %cmpgetmp = icmp sge i64 %"%idx.load2", %"%pci_dev_count.load"
  br i1 %cmpgetmp, label %then_1031, label %merge_1032

merge_1032:                                       ; preds = %merge_1030
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %calltmp = call i64 @pci_get_class(i64 %"%idx.load3")
  store i64 %calltmp, ptr %"%cls", align 8
  %"%cls.load" = load i64, ptr %"%cls", align 8
  %andtmp = and i64 %"%cls.load", 255
  ret i64 %andtmp

then_1029:                                        ; preds = %entry
  ret i64 -1

then_1031:                                        ; preds = %merge_1030
  ret i64 -1
}

define i64 @pci_loc_bus(i64 %idx) {
entry:
  %"%loc" = alloca i64, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %calltmp = call i64 @pci_get_loc(i64 %"%idx.load")
  store i64 %calltmp, ptr %"%loc", align 8
  %"%loc.load" = load i64, ptr %"%loc", align 8
  %shrtmp = lshr i64 %"%loc.load", 8
  ret i64 %shrtmp
}

define i64 @pci_loc_dev(i64 %idx) {
entry:
  %"%loc" = alloca i64, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %calltmp = call i64 @pci_get_loc(i64 %"%idx.load")
  store i64 %calltmp, ptr %"%loc", align 8
  %"%loc.load" = load i64, ptr %"%loc", align 8
  %andtmp = and i64 %"%loc.load", 255
  %shrtmp = lshr i64 %andtmp, 3
  ret i64 %shrtmp
}

define i64 @pci_loc_func(i64 %idx) {
entry:
  %"%loc" = alloca i64, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %calltmp = call i64 @pci_get_loc(i64 %"%idx.load")
  store i64 %calltmp, ptr %"%loc", align 8
  %"%loc.load" = load i64, ptr %"%loc", align 8
  %andtmp = and i64 %"%loc.load", 7
  ret i64 %andtmp
}

define i64 @pci_find_capability(i64 %bus, i64 %dev, i64 %func, i64 %cap_id) {
entry:
  %"%id" = alloca i64, align 8
  %"%iterations" = alloca i64, align 8
  %"%cap_ptr" = alloca i64, align 8
  %"%status" = alloca i64, align 8
  %cap_id4 = alloca i64, align 8
  store i64 %cap_id, ptr %cap_id4, align 8
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%2998.load" = load i64, ptr %bus1, align 8
  %"%3000.load" = load i64, ptr %dev2, align 8
  %"%3002.load" = load i64, ptr %func3, align 8
  %"%PCI_STATUS.load" = load i64, ptr @PCI_STATUS, align 8
  %calltmp = call i64 @pci_read16(i64 %"%2998.load", i64 %"%3000.load", i64 %"%3002.load", i64 %"%PCI_STATUS.load")
  store i64 %calltmp, ptr %"%status", align 8
  %"%status.load" = load i64, ptr %"%status", align 8
  %shrtmp = lshr i64 %"%status.load", 4
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_1033, label %merge_1034

merge_1034:                                       ; preds = %entry
  %"%3014.load" = load i64, ptr %bus1, align 8
  %"%3016.load" = load i64, ptr %dev2, align 8
  %"%3018.load" = load i64, ptr %func3, align 8
  %calltmp5 = call i64 @pci_read8(i64 %"%3014.load", i64 %"%3016.load", i64 %"%3018.load", i64 52)
  store i64 %calltmp5, ptr %"%cap_ptr", align 8
  %"%cap_ptr.load" = load i64, ptr %"%cap_ptr", align 8
  %shrtmp6 = lshr i64 %"%cap_ptr.load", 2
  %shltmp = shl i64 %shrtmp6, 2
  store i64 %shltmp, ptr %"%cap_ptr", align 8
  store i64 0, ptr %"%iterations", align 8
  br label %while_cond_1035

merge_1039:                                       ; preds = %while_body_1036
  %"%3036.load" = load i64, ptr %bus1, align 8
  %"%3038.load" = load i64, ptr %dev2, align 8
  %"%3040.load" = load i64, ptr %func3, align 8
  %"%cap_ptr.load9" = load i64, ptr %"%cap_ptr", align 8
  %calltmp10 = call i64 @pci_read8(i64 %"%3036.load", i64 %"%3038.load", i64 %"%3040.load", i64 %"%cap_ptr.load9")
  store i64 %calltmp10, ptr %"%id", align 8
  %"%id.load" = load i64, ptr %"%id", align 8
  %"%cap_id.load" = load i64, ptr %cap_id4, align 8
  %cmpeqtmp11 = icmp eq i64 %"%id.load", %"%cap_id.load"
  br i1 %cmpeqtmp11, label %then_1040, label %merge_1041

merge_1041:                                       ; preds = %merge_1039
  %"%cap_ptr.load12" = load i64, ptr %"%cap_ptr", align 8
  %addtmp = add i64 %"%cap_ptr.load12", 1
  %"%3049.load" = load i64, ptr %bus1, align 8
  %"%3051.load" = load i64, ptr %dev2, align 8
  %"%3053.load" = load i64, ptr %func3, align 8
  %calltmp13 = call i64 @pci_read8(i64 %"%3049.load", i64 %"%3051.load", i64 %"%3053.load", i64 %addtmp)
  store i64 %calltmp13, ptr %"%cap_ptr", align 8
  %"%cap_ptr.load14" = load i64, ptr %"%cap_ptr", align 8
  %shrtmp15 = lshr i64 %"%cap_ptr.load14", 2
  %shltmp16 = shl i64 %shrtmp15, 2
  store i64 %shltmp16, ptr %"%cap_ptr", align 8
  %"%iterations.load17" = load i64, ptr %"%iterations", align 8
  %addtmp18 = add i64 %"%iterations.load17", 1
  store i64 %addtmp18, ptr %"%iterations", align 8
  br label %while_cond_1035

then_1033:                                        ; preds = %entry
  ret i64 0

then_1038:                                        ; preds = %while_body_1036
  ret i64 0

then_1040:                                        ; preds = %merge_1039
  %loadtmp = load i64, ptr %"%cap_ptr", align 8
  ret i64 %loadtmp

while_body_1036:                                  ; preds = %while_cond_1035
  %"%iterations.load" = load i64, ptr %"%iterations", align 8
  %cmpgttmp8 = icmp sgt i64 %"%iterations.load", 48
  br i1 %cmpgttmp8, label %then_1038, label %merge_1039

while_cond_1035:                                  ; preds = %merge_1041, %merge_1034
  %"%cap_ptr.load7" = load i64, ptr %"%cap_ptr", align 8
  %cmpgttmp = icmp sgt i64 %"%cap_ptr.load7", 0
  br i1 %cmpgttmp, label %while_body_1036, label %while_exit_1037

while_exit_1037:                                  ; preds = %while_cond_1035
  ret i64 0
}

define i64 @pci_enable_msi(i64 %bus, i64 %dev, i64 %func, i64 %vector) {
entry:
  %"%msi_ctrl" = alloca i64, align 8
  %"%cap_pos" = alloca i64, align 8
  %vector4 = alloca i64, align 8
  store i64 %vector, ptr %vector4, align 8
  %func3 = alloca i64, align 8
  store i64 %func, ptr %func3, align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%3067.load" = load i64, ptr %bus1, align 8
  %"%3069.load" = load i64, ptr %dev2, align 8
  %"%3071.load" = load i64, ptr %func3, align 8
  %"%PCI_CAP_MSI.load" = load i64, ptr @PCI_CAP_MSI, align 8
  %calltmp = call i64 @pci_find_capability(i64 %"%3067.load", i64 %"%3069.load", i64 %"%3071.load", i64 %"%PCI_CAP_MSI.load")
  store i64 %calltmp, ptr %"%cap_pos", align 8
  %"%cap_pos.load" = load i64, ptr %"%cap_pos", align 8
  %cmpeqtmp = icmp eq i64 %"%cap_pos.load", 0
  br i1 %cmpeqtmp, label %then_1042, label %merge_1043

else_1046:                                        ; preds = %merge_1043
  %"%cap_pos.load25" = load i64, ptr %"%cap_pos", align 8
  %addtmp26 = add i64 %"%cap_pos.load25", 8
  %"%3128.load" = load i64, ptr %bus1, align 8
  %"%3130.load" = load i64, ptr %dev2, align 8
  %"%3132.load" = load i64, ptr %func3, align 8
  %"%vector.load27" = load i64, ptr %vector4, align 8
  %calltmp28 = call i64 @pci_write16(i64 %"%3128.load", i64 %"%3130.load", i64 %"%3132.load", i64 %addtmp26, i64 %"%vector.load27")
  br label %merge_1045

merge_1043:                                       ; preds = %entry
  %"%cap_pos.load7" = load i64, ptr %"%cap_pos", align 8
  %addtmp = add i64 %"%cap_pos.load7", 2
  %"%3082.load" = load i64, ptr %bus1, align 8
  %"%3084.load" = load i64, ptr %dev2, align 8
  %"%3086.load" = load i64, ptr %func3, align 8
  %calltmp8 = call i64 @pci_read16(i64 %"%3082.load", i64 %"%3084.load", i64 %"%3086.load", i64 %addtmp)
  store i64 %calltmp8, ptr %"%msi_ctrl", align 8
  %"%cap_pos.load9" = load i64, ptr %"%cap_pos", align 8
  %addtmp10 = add i64 %"%cap_pos.load9", 4
  %"%3082.load11" = load i64, ptr %bus1, align 8
  %"%3084.load12" = load i64, ptr %dev2, align 8
  %"%3086.load13" = load i64, ptr %func3, align 8
  %calltmp14 = call i64 @pci_write32(i64 %"%3082.load11", i64 %"%3084.load12", i64 %"%3086.load13", i64 %addtmp10, i64 4276092928)
  %"%msi_ctrl.load" = load i64, ptr %"%msi_ctrl", align 8
  %shrtmp = lshr i64 %"%msi_ctrl.load", 7
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp15 = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp15, label %then_1044, label %else_1046

merge_1045:                                       ; preds = %else_1046, %then_1044
  %"%msi_ctrl.load29" = load i64, ptr %"%msi_ctrl", align 8
  %andtmp30 = and i64 %"%msi_ctrl.load29", 1
  %cmpeqtmp31 = icmp eq i64 %andtmp30, 0
  br i1 %cmpeqtmp31, label %then_1047, label %merge_1048

merge_1048:                                       ; preds = %then_1047, %merge_1045
  %"%cap_pos.load34" = load i64, ptr %"%cap_pos", align 8
  %addtmp35 = add i64 %"%cap_pos.load34", 2
  %"%3143.load" = load i64, ptr %bus1, align 8
  %"%3145.load" = load i64, ptr %dev2, align 8
  %"%3147.load" = load i64, ptr %func3, align 8
  %"%msi_ctrl.load36" = load i64, ptr %"%msi_ctrl", align 8
  %calltmp37 = call i64 @pci_write16(i64 %"%3143.load", i64 %"%3145.load", i64 %"%3147.load", i64 %addtmp35, i64 %"%msi_ctrl.load36")
  %calltmp38 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %"%3154.load" = load i64, ptr %vector4, align 8
  %calltmp39 = call i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"%3154.load")
  %calltmp40 = call i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1

then_1042:                                        ; preds = %entry
  %calltmp5 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  %calltmp6 = call i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_1044:                                        ; preds = %merge_1043
  %"%cap_pos.load16" = load i64, ptr %"%cap_pos", align 8
  %addtmp17 = add i64 %"%cap_pos.load16", 8
  %"%3109.load" = load i64, ptr %bus1, align 8
  %"%3111.load" = load i64, ptr %dev2, align 8
  %"%3113.load" = load i64, ptr %func3, align 8
  %calltmp18 = call i64 @pci_write32(i64 %"%3109.load", i64 %"%3111.load", i64 %"%3113.load", i64 %addtmp17, i64 0)
  %"%cap_pos.load19" = load i64, ptr %"%cap_pos", align 8
  %addtmp20 = add i64 %"%cap_pos.load19", 12
  %"%3109.load21" = load i64, ptr %bus1, align 8
  %"%3111.load22" = load i64, ptr %dev2, align 8
  %"%3113.load23" = load i64, ptr %func3, align 8
  %"%vector.load" = load i64, ptr %vector4, align 8
  %calltmp24 = call i64 @pci_write16(i64 %"%3109.load21", i64 %"%3111.load22", i64 %"%3113.load23", i64 %addtmp20, i64 %"%vector.load")
  br label %merge_1045

then_1047:                                        ; preds = %merge_1045
  %"%msi_ctrl.load32" = load i64, ptr %"%msi_ctrl", align 8
  %addtmp33 = add i64 %"%msi_ctrl.load32", 1
  store i64 %addtmp33, ptr %"%msi_ctrl", align 8
  br label %merge_1048
}

define i64 @"pci_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %"%bar0" = alloca i64, align 8
  %"%cls" = alloca i64, align 8
  %"%did" = alloca i64, align 8
  %"%vid" = alloca i64, align 8
  %"%func" = alloca i64, align 8
  %"%dev" = alloca i64, align 8
  %"%bus" = alloca i64, align 8
  %"%loc" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %calltmp = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %calltmp1 = call i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %"%3164.load" = load i64, ptr @pci_dev_count, align 8
  %calltmp3 = call i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"%3164.load")
  %calltmp4 = call i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp5 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %"%3170.load" = load i64, ptr @pci_disk_count, align 8
  %calltmp6 = call i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"%3170.load")
  %calltmp7 = call i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp8 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  %"%3176.load" = load i64, ptr @pci_net_count, align 8
  %calltmp9 = call i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"%3176.load")
  %calltmp10 = call i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp11 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%3182.load" = load i64, ptr @pci_display_count, align 8
  %calltmp12 = call i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"%3182.load")
  %calltmp13 = call i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp14 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  %"%3188.load" = load i64, ptr @pci_audio_count, align 8
  %calltmp15 = call i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"%3188.load")
  %calltmp16 = call i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp17 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %"%3194.load" = load i64, ptr @pci_usb_count, align 8
  %calltmp18 = call i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"%3194.load")
  %calltmp19 = call i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp20 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %"%3200.load" = load i64, ptr @pci_bridge_count, align 8
  %calltmp21 = call i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"%3200.load")
  %calltmp22 = call i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%i", align 8
  br label %while_cond_1049

while_body_1050:                                  ; preds = %while_cond_1049
  %"%i.load23" = load i64, ptr %"%i", align 8
  %calltmp24 = call i64 @pci_get_loc(i64 %"%i.load23")
  store i64 %calltmp24, ptr %"%loc", align 8
  %"%loc.load" = load i64, ptr %"%loc", align 8
  %shrtmp = lshr i64 %"%loc.load", 8
  store i64 %shrtmp, ptr %"%bus", align 8
  %"%loc.load25" = load i64, ptr %"%loc", align 8
  %andtmp = and i64 %"%loc.load25", 255
  %shrtmp26 = lshr i64 %andtmp, 3
  store i64 %shrtmp26, ptr %"%dev", align 8
  %"%loc.load27" = load i64, ptr %"%loc", align 8
  %andtmp28 = and i64 %"%loc.load27", 7
  store i64 %andtmp28, ptr %"%func", align 8
  %"%i.load29" = load i64, ptr %"%i", align 8
  %calltmp30 = call i64 @pci_get_vid(i64 %"%i.load29")
  store i64 %calltmp30, ptr %"%vid", align 8
  %"%i.load31" = load i64, ptr %"%i", align 8
  %calltmp32 = call i64 @pci_get_did(i64 %"%i.load31")
  store i64 %calltmp32, ptr %"%did", align 8
  %"%i.load33" = load i64, ptr %"%i", align 8
  %calltmp34 = call i64 @pci_get_class(i64 %"%i.load33")
  store i64 %calltmp34, ptr %"%cls", align 8
  %"%i.load35" = load i64, ptr %"%i", align 8
  %calltmp36 = call i64 @pci_get_bar0(i64 %"%i.load35")
  store i64 %calltmp36, ptr %"%bar0", align 8
  %calltmp37 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  %"%3233.load" = load i64, ptr %"%i", align 8
  %calltmp38 = call i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"%3233.load")
  %calltmp39 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  %"%3238.load" = load i64, ptr %"%bus", align 8
  %calltmp40 = call i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"%3238.load")
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 58)
  %"%3243.load" = load i64, ptr %"%dev", align 8
  %calltmp41 = call i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"%3243.load")
  %"%COM1.load42" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load42", i64 46)
  %"%3248.load" = load i64, ptr %"%func", align 8
  %calltmp43 = call i64 @"pci_serial_\D8\B1\D9\82\D9\85"(i64 %"%3248.load")
  %calltmp44 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  %"%3253.load" = load i64, ptr %"%vid", align 8
  %calltmp45 = call i64 @pci_serial_hex16(i64 %"%3253.load")
  %"%COM1.load46" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load46", i64 58)
  %"%3258.load" = load i64, ptr %"%did", align 8
  %calltmp47 = call i64 @pci_serial_hex16(i64 %"%3258.load")
  %calltmp48 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  %"%cls.load" = load i64, ptr %"%cls", align 8
  %shrtmp49 = lshr i64 %"%cls.load", 16
  %calltmp50 = call i64 @pci_serial_hex8(i64 %shrtmp49)
  %"%COM1.load51" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load51", i64 58)
  %"%cls.load52" = load i64, ptr %"%cls", align 8
  %shrtmp53 = lshr i64 %"%cls.load52", 8
  %andtmp54 = and i64 %shrtmp53, 255
  %calltmp55 = call i64 @pci_serial_hex8(i64 %andtmp54)
  %calltmp56 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  %"%3275.load" = load i64, ptr %"%bar0", align 8
  %calltmp57 = call i64 @pci_serial_hex32(i64 %"%3275.load")
  %calltmp58 = call i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"()
  %"%i.load59" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load59", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_1049

while_cond_1049:                                  ; preds = %while_body_1050, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%pci_dev_count.load" = load i64, ptr @pci_dev_count, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%pci_dev_count.load"
  br i1 %cmplttmp, label %while_body_1050, label %while_exit_1051

while_exit_1051:                                  ; preds = %while_cond_1049
  %calltmp60 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %calltmp61 = call i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @"pci_\D9\87\D9\8A\D8\A6"() {
entry:
  %"%pci_other_count" = alloca i64, align 8
  %"%pci_bridge_count" = alloca i64, align 8
  %"%pci_audio_count" = alloca i64, align 8
  %"%pci_display_count" = alloca i64, align 8
  %"%pci_disk_count" = alloca i64, align 8
  %"%pci_net_count" = alloca i64, align 8
  %"%pci_usb_count" = alloca i64, align 8
  %"%pci_dev_count" = alloca i64, align 8
  %calltmp = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  %calltmp1 = call i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%pci_dev_count", align 8
  store i64 0, ptr %"%pci_usb_count", align 8
  store i64 0, ptr %"%pci_net_count", align 8
  store i64 0, ptr %"%pci_disk_count", align 8
  store i64 0, ptr %"%pci_display_count", align 8
  store i64 0, ptr %"%pci_audio_count", align 8
  store i64 0, ptr %"%pci_bridge_count", align 8
  store i64 0, ptr %"%pci_other_count", align 8
  %calltmp2 = call i64 @pci_scan_all()
  %calltmp3 = call i64 @"pci_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"()
  %calltmp4 = call i64 @"pci_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  %calltmp5 = call i64 @"pci_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr %"%pci_dev_count", align 8
  ret i64 %loadtmp
}

define i64 @"pci_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  ret i64 0
}

define void @__sad_main() {
entry:
  %"%PCI_CAP_MSI" = alloca i64, align 8
  %"%pci_other_count" = alloca i64, align 8
  %"%pci_bridge_count" = alloca i64, align 8
  %"%pci_audio_count" = alloca i64, align 8
  %"%pci_display_count" = alloca i64, align 8
  %"%pci_disk_count" = alloca i64, align 8
  %"%pci_net_count" = alloca i64, align 8
  %"%pci_usb_count" = alloca i64, align 8
  %"%pci_irq_63" = alloca i64, align 8
  %"%pci_irq_62" = alloca i64, align 8
  %"%pci_irq_61" = alloca i64, align 8
  %"%pci_irq_60" = alloca i64, align 8
  %"%pci_irq_59" = alloca i64, align 8
  %"%pci_irq_58" = alloca i64, align 8
  %"%pci_irq_57" = alloca i64, align 8
  %"%pci_irq_56" = alloca i64, align 8
  %"%pci_irq_55" = alloca i64, align 8
  %"%pci_irq_54" = alloca i64, align 8
  %"%pci_irq_53" = alloca i64, align 8
  %"%pci_irq_52" = alloca i64, align 8
  %"%pci_irq_51" = alloca i64, align 8
  %"%pci_irq_50" = alloca i64, align 8
  %"%pci_irq_49" = alloca i64, align 8
  %"%pci_irq_48" = alloca i64, align 8
  %"%pci_irq_47" = alloca i64, align 8
  %"%pci_irq_46" = alloca i64, align 8
  %"%pci_irq_45" = alloca i64, align 8
  %"%pci_irq_44" = alloca i64, align 8
  %"%pci_irq_43" = alloca i64, align 8
  %"%pci_irq_42" = alloca i64, align 8
  %"%pci_irq_41" = alloca i64, align 8
  %"%pci_irq_40" = alloca i64, align 8
  %"%pci_irq_39" = alloca i64, align 8
  %"%pci_irq_38" = alloca i64, align 8
  %"%pci_irq_37" = alloca i64, align 8
  %"%pci_irq_36" = alloca i64, align 8
  %"%pci_irq_35" = alloca i64, align 8
  %"%pci_irq_34" = alloca i64, align 8
  %"%pci_irq_33" = alloca i64, align 8
  %"%pci_irq_32" = alloca i64, align 8
  %"%pci_irq_31" = alloca i64, align 8
  %"%pci_irq_30" = alloca i64, align 8
  %"%pci_irq_29" = alloca i64, align 8
  %"%pci_irq_28" = alloca i64, align 8
  %"%pci_irq_27" = alloca i64, align 8
  %"%pci_irq_26" = alloca i64, align 8
  %"%pci_irq_25" = alloca i64, align 8
  %"%pci_irq_24" = alloca i64, align 8
  %"%pci_irq_23" = alloca i64, align 8
  %"%pci_irq_22" = alloca i64, align 8
  %"%pci_irq_21" = alloca i64, align 8
  %"%pci_irq_20" = alloca i64, align 8
  %"%pci_irq_19" = alloca i64, align 8
  %"%pci_irq_18" = alloca i64, align 8
  %"%pci_irq_17" = alloca i64, align 8
  %"%pci_irq_16" = alloca i64, align 8
  %"%pci_irq_15" = alloca i64, align 8
  %"%pci_irq_14" = alloca i64, align 8
  %"%pci_irq_13" = alloca i64, align 8
  %"%pci_irq_12" = alloca i64, align 8
  %"%pci_irq_11" = alloca i64, align 8
  %"%pci_irq_10" = alloca i64, align 8
  %"%pci_irq_9" = alloca i64, align 8
  %"%pci_irq_8" = alloca i64, align 8
  %"%pci_irq_7" = alloca i64, align 8
  %"%pci_irq_6" = alloca i64, align 8
  %"%pci_irq_5" = alloca i64, align 8
  %"%pci_irq_4" = alloca i64, align 8
  %"%pci_irq_3" = alloca i64, align 8
  %"%pci_irq_2" = alloca i64, align 8
  %"%pci_irq_1" = alloca i64, align 8
  %"%pci_irq_0" = alloca i64, align 8
  %"%pci_bar1_63" = alloca i64, align 8
  %"%pci_bar1_62" = alloca i64, align 8
  %"%pci_bar1_61" = alloca i64, align 8
  %"%pci_bar1_60" = alloca i64, align 8
  %"%pci_bar1_59" = alloca i64, align 8
  %"%pci_bar1_58" = alloca i64, align 8
  %"%pci_bar1_57" = alloca i64, align 8
  %"%pci_bar1_56" = alloca i64, align 8
  %"%pci_bar1_55" = alloca i64, align 8
  %"%pci_bar1_54" = alloca i64, align 8
  %"%pci_bar1_53" = alloca i64, align 8
  %"%pci_bar1_52" = alloca i64, align 8
  %"%pci_bar1_51" = alloca i64, align 8
  %"%pci_bar1_50" = alloca i64, align 8
  %"%pci_bar1_49" = alloca i64, align 8
  %"%pci_bar1_48" = alloca i64, align 8
  %"%pci_bar1_47" = alloca i64, align 8
  %"%pci_bar1_46" = alloca i64, align 8
  %"%pci_bar1_45" = alloca i64, align 8
  %"%pci_bar1_44" = alloca i64, align 8
  %"%pci_bar1_43" = alloca i64, align 8
  %"%pci_bar1_42" = alloca i64, align 8
  %"%pci_bar1_41" = alloca i64, align 8
  %"%pci_bar1_40" = alloca i64, align 8
  %"%pci_bar1_39" = alloca i64, align 8
  %"%pci_bar1_38" = alloca i64, align 8
  %"%pci_bar1_37" = alloca i64, align 8
  %"%pci_bar1_36" = alloca i64, align 8
  %"%pci_bar1_35" = alloca i64, align 8
  %"%pci_bar1_34" = alloca i64, align 8
  %"%pci_bar1_33" = alloca i64, align 8
  %"%pci_bar1_32" = alloca i64, align 8
  %"%pci_bar1_31" = alloca i64, align 8
  %"%pci_bar1_30" = alloca i64, align 8
  %"%pci_bar1_29" = alloca i64, align 8
  %"%pci_bar1_28" = alloca i64, align 8
  %"%pci_bar1_27" = alloca i64, align 8
  %"%pci_bar1_26" = alloca i64, align 8
  %"%pci_bar1_25" = alloca i64, align 8
  %"%pci_bar1_24" = alloca i64, align 8
  %"%pci_bar1_23" = alloca i64, align 8
  %"%pci_bar1_22" = alloca i64, align 8
  %"%pci_bar1_21" = alloca i64, align 8
  %"%pci_bar1_20" = alloca i64, align 8
  %"%pci_bar1_19" = alloca i64, align 8
  %"%pci_bar1_18" = alloca i64, align 8
  %"%pci_bar1_17" = alloca i64, align 8
  %"%pci_bar1_16" = alloca i64, align 8
  %"%pci_bar1_15" = alloca i64, align 8
  %"%pci_bar1_14" = alloca i64, align 8
  %"%pci_bar1_13" = alloca i64, align 8
  %"%pci_bar1_12" = alloca i64, align 8
  %"%pci_bar1_11" = alloca i64, align 8
  %"%pci_bar1_10" = alloca i64, align 8
  %"%pci_bar1_9" = alloca i64, align 8
  %"%pci_bar1_8" = alloca i64, align 8
  %"%pci_bar1_7" = alloca i64, align 8
  %"%pci_bar1_6" = alloca i64, align 8
  %"%pci_bar1_5" = alloca i64, align 8
  %"%pci_bar1_4" = alloca i64, align 8
  %"%pci_bar1_3" = alloca i64, align 8
  %"%pci_bar1_2" = alloca i64, align 8
  %"%pci_bar1_1" = alloca i64, align 8
  %"%pci_bar1_0" = alloca i64, align 8
  %"%pci_bar0_63" = alloca i64, align 8
  %"%pci_bar0_62" = alloca i64, align 8
  %"%pci_bar0_61" = alloca i64, align 8
  %"%pci_bar0_60" = alloca i64, align 8
  %"%pci_bar0_59" = alloca i64, align 8
  %"%pci_bar0_58" = alloca i64, align 8
  %"%pci_bar0_57" = alloca i64, align 8
  %"%pci_bar0_56" = alloca i64, align 8
  %"%pci_bar0_55" = alloca i64, align 8
  %"%pci_bar0_54" = alloca i64, align 8
  %"%pci_bar0_53" = alloca i64, align 8
  %"%pci_bar0_52" = alloca i64, align 8
  %"%pci_bar0_51" = alloca i64, align 8
  %"%pci_bar0_50" = alloca i64, align 8
  %"%pci_bar0_49" = alloca i64, align 8
  %"%pci_bar0_48" = alloca i64, align 8
  %"%pci_bar0_47" = alloca i64, align 8
  %"%pci_bar0_46" = alloca i64, align 8
  %"%pci_bar0_45" = alloca i64, align 8
  %"%pci_bar0_44" = alloca i64, align 8
  %"%pci_bar0_43" = alloca i64, align 8
  %"%pci_bar0_42" = alloca i64, align 8
  %"%pci_bar0_41" = alloca i64, align 8
  %"%pci_bar0_40" = alloca i64, align 8
  %"%pci_bar0_39" = alloca i64, align 8
  %"%pci_bar0_38" = alloca i64, align 8
  %"%pci_bar0_37" = alloca i64, align 8
  %"%pci_bar0_36" = alloca i64, align 8
  %"%pci_bar0_35" = alloca i64, align 8
  %"%pci_bar0_34" = alloca i64, align 8
  %"%pci_bar0_33" = alloca i64, align 8
  %"%pci_bar0_32" = alloca i64, align 8
  %"%pci_bar0_31" = alloca i64, align 8
  %"%pci_bar0_30" = alloca i64, align 8
  %"%pci_bar0_29" = alloca i64, align 8
  %"%pci_bar0_28" = alloca i64, align 8
  %"%pci_bar0_27" = alloca i64, align 8
  %"%pci_bar0_26" = alloca i64, align 8
  %"%pci_bar0_25" = alloca i64, align 8
  %"%pci_bar0_24" = alloca i64, align 8
  %"%pci_bar0_23" = alloca i64, align 8
  %"%pci_bar0_22" = alloca i64, align 8
  %"%pci_bar0_21" = alloca i64, align 8
  %"%pci_bar0_20" = alloca i64, align 8
  %"%pci_bar0_19" = alloca i64, align 8
  %"%pci_bar0_18" = alloca i64, align 8
  %"%pci_bar0_17" = alloca i64, align 8
  %"%pci_bar0_16" = alloca i64, align 8
  %"%pci_bar0_15" = alloca i64, align 8
  %"%pci_bar0_14" = alloca i64, align 8
  %"%pci_bar0_13" = alloca i64, align 8
  %"%pci_bar0_12" = alloca i64, align 8
  %"%pci_bar0_11" = alloca i64, align 8
  %"%pci_bar0_10" = alloca i64, align 8
  %"%pci_bar0_9" = alloca i64, align 8
  %"%pci_bar0_8" = alloca i64, align 8
  %"%pci_bar0_7" = alloca i64, align 8
  %"%pci_bar0_6" = alloca i64, align 8
  %"%pci_bar0_5" = alloca i64, align 8
  %"%pci_bar0_4" = alloca i64, align 8
  %"%pci_bar0_3" = alloca i64, align 8
  %"%pci_bar0_2" = alloca i64, align 8
  %"%pci_bar0_1" = alloca i64, align 8
  %"%pci_bar0_0" = alloca i64, align 8
  %"%pci_class_63" = alloca i64, align 8
  %"%pci_class_62" = alloca i64, align 8
  %"%pci_class_61" = alloca i64, align 8
  %"%pci_class_60" = alloca i64, align 8
  %"%pci_class_59" = alloca i64, align 8
  %"%pci_class_58" = alloca i64, align 8
  %"%pci_class_57" = alloca i64, align 8
  %"%pci_class_56" = alloca i64, align 8
  %"%pci_class_55" = alloca i64, align 8
  %"%pci_class_54" = alloca i64, align 8
  %"%pci_class_53" = alloca i64, align 8
  %"%pci_class_52" = alloca i64, align 8
  %"%pci_class_51" = alloca i64, align 8
  %"%pci_class_50" = alloca i64, align 8
  %"%pci_class_49" = alloca i64, align 8
  %"%pci_class_48" = alloca i64, align 8
  %"%pci_class_47" = alloca i64, align 8
  %"%pci_class_46" = alloca i64, align 8
  %"%pci_class_45" = alloca i64, align 8
  %"%pci_class_44" = alloca i64, align 8
  %"%pci_class_43" = alloca i64, align 8
  %"%pci_class_42" = alloca i64, align 8
  %"%pci_class_41" = alloca i64, align 8
  %"%pci_class_40" = alloca i64, align 8
  %"%pci_class_39" = alloca i64, align 8
  %"%pci_class_38" = alloca i64, align 8
  %"%pci_class_37" = alloca i64, align 8
  %"%pci_class_36" = alloca i64, align 8
  %"%pci_class_35" = alloca i64, align 8
  %"%pci_class_34" = alloca i64, align 8
  %"%pci_class_33" = alloca i64, align 8
  %"%pci_class_32" = alloca i64, align 8
  %"%pci_class_31" = alloca i64, align 8
  %"%pci_class_30" = alloca i64, align 8
  %"%pci_class_29" = alloca i64, align 8
  %"%pci_class_28" = alloca i64, align 8
  %"%pci_class_27" = alloca i64, align 8
  %"%pci_class_26" = alloca i64, align 8
  %"%pci_class_25" = alloca i64, align 8
  %"%pci_class_24" = alloca i64, align 8
  %"%pci_class_23" = alloca i64, align 8
  %"%pci_class_22" = alloca i64, align 8
  %"%pci_class_21" = alloca i64, align 8
  %"%pci_class_20" = alloca i64, align 8
  %"%pci_class_19" = alloca i64, align 8
  %"%pci_class_18" = alloca i64, align 8
  %"%pci_class_17" = alloca i64, align 8
  %"%pci_class_16" = alloca i64, align 8
  %"%pci_class_15" = alloca i64, align 8
  %"%pci_class_14" = alloca i64, align 8
  %"%pci_class_13" = alloca i64, align 8
  %"%pci_class_12" = alloca i64, align 8
  %"%pci_class_11" = alloca i64, align 8
  %"%pci_class_10" = alloca i64, align 8
  %"%pci_class_9" = alloca i64, align 8
  %"%pci_class_8" = alloca i64, align 8
  %"%pci_class_7" = alloca i64, align 8
  %"%pci_class_6" = alloca i64, align 8
  %"%pci_class_5" = alloca i64, align 8
  %"%pci_class_4" = alloca i64, align 8
  %"%pci_class_3" = alloca i64, align 8
  %"%pci_class_2" = alloca i64, align 8
  %"%pci_class_1" = alloca i64, align 8
  %"%pci_class_0" = alloca i64, align 8
  %"%pci_did_63" = alloca i64, align 8
  %"%pci_did_62" = alloca i64, align 8
  %"%pci_did_61" = alloca i64, align 8
  %"%pci_did_60" = alloca i64, align 8
  %"%pci_did_59" = alloca i64, align 8
  %"%pci_did_58" = alloca i64, align 8
  %"%pci_did_57" = alloca i64, align 8
  %"%pci_did_56" = alloca i64, align 8
  %"%pci_did_55" = alloca i64, align 8
  %"%pci_did_54" = alloca i64, align 8
  %"%pci_did_53" = alloca i64, align 8
  %"%pci_did_52" = alloca i64, align 8
  %"%pci_did_51" = alloca i64, align 8
  %"%pci_did_50" = alloca i64, align 8
  %"%pci_did_49" = alloca i64, align 8
  %"%pci_did_48" = alloca i64, align 8
  %"%pci_did_47" = alloca i64, align 8
  %"%pci_did_46" = alloca i64, align 8
  %"%pci_did_45" = alloca i64, align 8
  %"%pci_did_44" = alloca i64, align 8
  %"%pci_did_43" = alloca i64, align 8
  %"%pci_did_42" = alloca i64, align 8
  %"%pci_did_41" = alloca i64, align 8
  %"%pci_did_40" = alloca i64, align 8
  %"%pci_did_39" = alloca i64, align 8
  %"%pci_did_38" = alloca i64, align 8
  %"%pci_did_37" = alloca i64, align 8
  %"%pci_did_36" = alloca i64, align 8
  %"%pci_did_35" = alloca i64, align 8
  %"%pci_did_34" = alloca i64, align 8
  %"%pci_did_33" = alloca i64, align 8
  %"%pci_did_32" = alloca i64, align 8
  %"%pci_did_31" = alloca i64, align 8
  %"%pci_did_30" = alloca i64, align 8
  %"%pci_did_29" = alloca i64, align 8
  %"%pci_did_28" = alloca i64, align 8
  %"%pci_did_27" = alloca i64, align 8
  %"%pci_did_26" = alloca i64, align 8
  %"%pci_did_25" = alloca i64, align 8
  %"%pci_did_24" = alloca i64, align 8
  %"%pci_did_23" = alloca i64, align 8
  %"%pci_did_22" = alloca i64, align 8
  %"%pci_did_21" = alloca i64, align 8
  %"%pci_did_20" = alloca i64, align 8
  %"%pci_did_19" = alloca i64, align 8
  %"%pci_did_18" = alloca i64, align 8
  %"%pci_did_17" = alloca i64, align 8
  %"%pci_did_16" = alloca i64, align 8
  %"%pci_did_15" = alloca i64, align 8
  %"%pci_did_14" = alloca i64, align 8
  %"%pci_did_13" = alloca i64, align 8
  %"%pci_did_12" = alloca i64, align 8
  %"%pci_did_11" = alloca i64, align 8
  %"%pci_did_10" = alloca i64, align 8
  %"%pci_did_9" = alloca i64, align 8
  %"%pci_did_8" = alloca i64, align 8
  %"%pci_did_7" = alloca i64, align 8
  %"%pci_did_6" = alloca i64, align 8
  %"%pci_did_5" = alloca i64, align 8
  %"%pci_did_4" = alloca i64, align 8
  %"%pci_did_3" = alloca i64, align 8
  %"%pci_did_2" = alloca i64, align 8
  %"%pci_did_1" = alloca i64, align 8
  %"%pci_did_0" = alloca i64, align 8
  %"%pci_vid_63" = alloca i64, align 8
  %"%pci_vid_62" = alloca i64, align 8
  %"%pci_vid_61" = alloca i64, align 8
  %"%pci_vid_60" = alloca i64, align 8
  %"%pci_vid_59" = alloca i64, align 8
  %"%pci_vid_58" = alloca i64, align 8
  %"%pci_vid_57" = alloca i64, align 8
  %"%pci_vid_56" = alloca i64, align 8
  %"%pci_vid_55" = alloca i64, align 8
  %"%pci_vid_54" = alloca i64, align 8
  %"%pci_vid_53" = alloca i64, align 8
  %"%pci_vid_52" = alloca i64, align 8
  %"%pci_vid_51" = alloca i64, align 8
  %"%pci_vid_50" = alloca i64, align 8
  %"%pci_vid_49" = alloca i64, align 8
  %"%pci_vid_48" = alloca i64, align 8
  %"%pci_vid_47" = alloca i64, align 8
  %"%pci_vid_46" = alloca i64, align 8
  %"%pci_vid_45" = alloca i64, align 8
  %"%pci_vid_44" = alloca i64, align 8
  %"%pci_vid_43" = alloca i64, align 8
  %"%pci_vid_42" = alloca i64, align 8
  %"%pci_vid_41" = alloca i64, align 8
  %"%pci_vid_40" = alloca i64, align 8
  %"%pci_vid_39" = alloca i64, align 8
  %"%pci_vid_38" = alloca i64, align 8
  %"%pci_vid_37" = alloca i64, align 8
  %"%pci_vid_36" = alloca i64, align 8
  %"%pci_vid_35" = alloca i64, align 8
  %"%pci_vid_34" = alloca i64, align 8
  %"%pci_vid_33" = alloca i64, align 8
  %"%pci_vid_32" = alloca i64, align 8
  %"%pci_vid_31" = alloca i64, align 8
  %"%pci_vid_30" = alloca i64, align 8
  %"%pci_vid_29" = alloca i64, align 8
  %"%pci_vid_28" = alloca i64, align 8
  %"%pci_vid_27" = alloca i64, align 8
  %"%pci_vid_26" = alloca i64, align 8
  %"%pci_vid_25" = alloca i64, align 8
  %"%pci_vid_24" = alloca i64, align 8
  %"%pci_vid_23" = alloca i64, align 8
  %"%pci_vid_22" = alloca i64, align 8
  %"%pci_vid_21" = alloca i64, align 8
  %"%pci_vid_20" = alloca i64, align 8
  %"%pci_vid_19" = alloca i64, align 8
  %"%pci_vid_18" = alloca i64, align 8
  %"%pci_vid_17" = alloca i64, align 8
  %"%pci_vid_16" = alloca i64, align 8
  %"%pci_vid_15" = alloca i64, align 8
  %"%pci_vid_14" = alloca i64, align 8
  %"%pci_vid_13" = alloca i64, align 8
  %"%pci_vid_12" = alloca i64, align 8
  %"%pci_vid_11" = alloca i64, align 8
  %"%pci_vid_10" = alloca i64, align 8
  %"%pci_vid_9" = alloca i64, align 8
  %"%pci_vid_8" = alloca i64, align 8
  %"%pci_vid_7" = alloca i64, align 8
  %"%pci_vid_6" = alloca i64, align 8
  %"%pci_vid_5" = alloca i64, align 8
  %"%pci_vid_4" = alloca i64, align 8
  %"%pci_vid_3" = alloca i64, align 8
  %"%pci_vid_2" = alloca i64, align 8
  %"%pci_vid_1" = alloca i64, align 8
  %"%pci_vid_0" = alloca i64, align 8
  %"%pci_loc_63" = alloca i64, align 8
  %"%pci_loc_62" = alloca i64, align 8
  %"%pci_loc_61" = alloca i64, align 8
  %"%pci_loc_60" = alloca i64, align 8
  %"%pci_loc_59" = alloca i64, align 8
  %"%pci_loc_58" = alloca i64, align 8
  %"%pci_loc_57" = alloca i64, align 8
  %"%pci_loc_56" = alloca i64, align 8
  %"%pci_loc_55" = alloca i64, align 8
  %"%pci_loc_54" = alloca i64, align 8
  %"%pci_loc_53" = alloca i64, align 8
  %"%pci_loc_52" = alloca i64, align 8
  %"%pci_loc_51" = alloca i64, align 8
  %"%pci_loc_50" = alloca i64, align 8
  %"%pci_loc_49" = alloca i64, align 8
  %"%pci_loc_48" = alloca i64, align 8
  %"%pci_loc_47" = alloca i64, align 8
  %"%pci_loc_46" = alloca i64, align 8
  %"%pci_loc_45" = alloca i64, align 8
  %"%pci_loc_44" = alloca i64, align 8
  %"%pci_loc_43" = alloca i64, align 8
  %"%pci_loc_42" = alloca i64, align 8
  %"%pci_loc_41" = alloca i64, align 8
  %"%pci_loc_40" = alloca i64, align 8
  %"%pci_loc_39" = alloca i64, align 8
  %"%pci_loc_38" = alloca i64, align 8
  %"%pci_loc_37" = alloca i64, align 8
  %"%pci_loc_36" = alloca i64, align 8
  %"%pci_loc_35" = alloca i64, align 8
  %"%pci_loc_34" = alloca i64, align 8
  %"%pci_loc_33" = alloca i64, align 8
  %"%pci_loc_32" = alloca i64, align 8
  %"%pci_loc_31" = alloca i64, align 8
  %"%pci_loc_30" = alloca i64, align 8
  %"%pci_loc_29" = alloca i64, align 8
  %"%pci_loc_28" = alloca i64, align 8
  %"%pci_loc_27" = alloca i64, align 8
  %"%pci_loc_26" = alloca i64, align 8
  %"%pci_loc_25" = alloca i64, align 8
  %"%pci_loc_24" = alloca i64, align 8
  %"%pci_loc_23" = alloca i64, align 8
  %"%pci_loc_22" = alloca i64, align 8
  %"%pci_loc_21" = alloca i64, align 8
  %"%pci_loc_20" = alloca i64, align 8
  %"%pci_loc_19" = alloca i64, align 8
  %"%pci_loc_18" = alloca i64, align 8
  %"%pci_loc_17" = alloca i64, align 8
  %"%pci_loc_16" = alloca i64, align 8
  %"%pci_loc_15" = alloca i64, align 8
  %"%pci_loc_14" = alloca i64, align 8
  %"%pci_loc_13" = alloca i64, align 8
  %"%pci_loc_12" = alloca i64, align 8
  %"%pci_loc_11" = alloca i64, align 8
  %"%pci_loc_10" = alloca i64, align 8
  %"%pci_loc_9" = alloca i64, align 8
  %"%pci_loc_8" = alloca i64, align 8
  %"%pci_loc_7" = alloca i64, align 8
  %"%pci_loc_6" = alloca i64, align 8
  %"%pci_loc_5" = alloca i64, align 8
  %"%pci_loc_4" = alloca i64, align 8
  %"%pci_loc_3" = alloca i64, align 8
  %"%pci_loc_2" = alloca i64, align 8
  %"%pci_loc_1" = alloca i64, align 8
  %"%pci_loc_0" = alloca i64, align 8
  %"%pci_dev_count" = alloca i64, align 8
  %"%PCI_VENDOR_INVALID" = alloca i64, align 8
  %"%PCI_MAX_FUNC" = alloca i64, align 8
  %"%PCI_MAX_DEV" = alloca i64, align 8
  %"%PCI_MAX_BUS" = alloca i64, align 8
  %"%PCI_PROGIF_XHCI" = alloca i64, align 8
  %"%PCI_PROGIF_EHCI" = alloca i64, align 8
  %"%PCI_PROGIF_OHCI" = alloca i64, align 8
  %"%PCI_PROGIF_UHCI" = alloca i64, align 8
  %"%PCI_SUBCLASS_SMBUS" = alloca i64, align 8
  %"%PCI_SUBCLASS_FIREWIRE" = alloca i64, align 8
  %"%PCI_SUBCLASS_USB" = alloca i64, align 8
  %"%PCI_CLASS_WIRELESS" = alloca i64, align 8
  %"%PCI_CLASS_SERIAL" = alloca i64, align 8
  %"%PCI_CLASS_PROCESSOR" = alloca i64, align 8
  %"%PCI_CLASS_DOCK" = alloca i64, align 8
  %"%PCI_CLASS_INPUT" = alloca i64, align 8
  %"%PCI_CLASS_SYSTEM" = alloca i64, align 8
  %"%PCI_CLASS_COMM" = alloca i64, align 8
  %"%PCI_CLASS_BRIDGE" = alloca i64, align 8
  %"%PCI_CLASS_MEMORY" = alloca i64, align 8
  %"%PCI_CLASS_MULTIMEDIA" = alloca i64, align 8
  %"%PCI_CLASS_DISPLAY" = alloca i64, align 8
  %"%PCI_CLASS_NETWORK" = alloca i64, align 8
  %"%PCI_CLASS_STORAGE" = alloca i64, align 8
  %"%PCI_CMD_INT_DISABLE" = alloca i64, align 8
  %"%PCI_CMD_FAST_BTB" = alloca i64, align 8
  %"%PCI_CMD_SERR" = alloca i64, align 8
  %"%PCI_CMD_PARITY" = alloca i64, align 8
  %"%PCI_CMD_PALETTE" = alloca i64, align 8
  %"%PCI_CMD_INVALIDATE" = alloca i64, align 8
  %"%PCI_CMD_SPECIAL" = alloca i64, align 8
  %"%PCI_CMD_BUS_MASTER" = alloca i64, align 8
  %"%PCI_CMD_MEM_ENABLE" = alloca i64, align 8
  %"%PCI_CMD_IO_ENABLE" = alloca i64, align 8
  %"%PCI_MAX_LATENCY" = alloca i64, align 8
  %"%PCI_MIN_GRANT" = alloca i64, align 8
  %"%PCI_IRQ_PIN" = alloca i64, align 8
  %"%PCI_IRQ_LINE" = alloca i64, align 8
  %"%PCI_SUBSYSTEM_ID" = alloca i64, align 8
  %"%PCI_SUBSYSTEM_VENDOR" = alloca i64, align 8
  %"%PCI_BAR5" = alloca i64, align 8
  %"%PCI_BAR4" = alloca i64, align 8
  %"%PCI_BAR3" = alloca i64, align 8
  %"%PCI_BAR2" = alloca i64, align 8
  %"%PCI_BAR1" = alloca i64, align 8
  %"%PCI_BAR0" = alloca i64, align 8
  %"%PCI_BIST" = alloca i64, align 8
  %"%PCI_HEADER_TYPE" = alloca i64, align 8
  %"%PCI_LATENCY_TIMER" = alloca i64, align 8
  %"%PCI_CACHE_LINE" = alloca i64, align 8
  %"%PCI_CLASS_CODE" = alloca i64, align 8
  %"%PCI_SUBCLASS" = alloca i64, align 8
  %"%PCI_PROG_IF" = alloca i64, align 8
  %"%PCI_REVISION" = alloca i64, align 8
  %"%PCI_STATUS" = alloca i64, align 8
  %"%PCI_COMMAND" = alloca i64, align 8
  %"%PCI_DEVICE_ID" = alloca i64, align 8
  %"%PCI_VENDOR_ID" = alloca i64, align 8
  %"%PCI_CONFIG_DATA" = alloca i64, align 8
  %"%PCI_CONFIG_ADDR" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 3320, ptr %"%PCI_CONFIG_ADDR", align 8
  store i64 3324, ptr %"%PCI_CONFIG_DATA", align 8
  store i64 0, ptr %"%PCI_VENDOR_ID", align 8
  store i64 2, ptr %"%PCI_DEVICE_ID", align 8
  store i64 4, ptr %"%PCI_COMMAND", align 8
  store i64 6, ptr %"%PCI_STATUS", align 8
  store i64 8, ptr %"%PCI_REVISION", align 8
  store i64 9, ptr %"%PCI_PROG_IF", align 8
  store i64 10, ptr %"%PCI_SUBCLASS", align 8
  store i64 11, ptr %"%PCI_CLASS_CODE", align 8
  store i64 12, ptr %"%PCI_CACHE_LINE", align 8
  store i64 13, ptr %"%PCI_LATENCY_TIMER", align 8
  store i64 14, ptr %"%PCI_HEADER_TYPE", align 8
  store i64 15, ptr %"%PCI_BIST", align 8
  store i64 16, ptr %"%PCI_BAR0", align 8
  store i64 20, ptr %"%PCI_BAR1", align 8
  store i64 24, ptr %"%PCI_BAR2", align 8
  store i64 28, ptr %"%PCI_BAR3", align 8
  store i64 32, ptr %"%PCI_BAR4", align 8
  store i64 36, ptr %"%PCI_BAR5", align 8
  store i64 44, ptr %"%PCI_SUBSYSTEM_VENDOR", align 8
  store i64 46, ptr %"%PCI_SUBSYSTEM_ID", align 8
  store i64 60, ptr %"%PCI_IRQ_LINE", align 8
  store i64 61, ptr %"%PCI_IRQ_PIN", align 8
  store i64 62, ptr %"%PCI_MIN_GRANT", align 8
  store i64 63, ptr %"%PCI_MAX_LATENCY", align 8
  store i64 1, ptr %"%PCI_CMD_IO_ENABLE", align 8
  store i64 2, ptr %"%PCI_CMD_MEM_ENABLE", align 8
  store i64 4, ptr %"%PCI_CMD_BUS_MASTER", align 8
  store i64 8, ptr %"%PCI_CMD_SPECIAL", align 8
  store i64 16, ptr %"%PCI_CMD_INVALIDATE", align 8
  store i64 32, ptr %"%PCI_CMD_PALETTE", align 8
  store i64 64, ptr %"%PCI_CMD_PARITY", align 8
  store i64 256, ptr %"%PCI_CMD_SERR", align 8
  store i64 512, ptr %"%PCI_CMD_FAST_BTB", align 8
  store i64 1024, ptr %"%PCI_CMD_INT_DISABLE", align 8
  store i64 1, ptr %"%PCI_CLASS_STORAGE", align 8
  store i64 2, ptr %"%PCI_CLASS_NETWORK", align 8
  store i64 3, ptr %"%PCI_CLASS_DISPLAY", align 8
  store i64 4, ptr %"%PCI_CLASS_MULTIMEDIA", align 8
  store i64 5, ptr %"%PCI_CLASS_MEMORY", align 8
  store i64 6, ptr %"%PCI_CLASS_BRIDGE", align 8
  store i64 7, ptr %"%PCI_CLASS_COMM", align 8
  store i64 8, ptr %"%PCI_CLASS_SYSTEM", align 8
  store i64 9, ptr %"%PCI_CLASS_INPUT", align 8
  store i64 10, ptr %"%PCI_CLASS_DOCK", align 8
  store i64 11, ptr %"%PCI_CLASS_PROCESSOR", align 8
  store i64 12, ptr %"%PCI_CLASS_SERIAL", align 8
  store i64 13, ptr %"%PCI_CLASS_WIRELESS", align 8
  store i64 3, ptr %"%PCI_SUBCLASS_USB", align 8
  store i64 0, ptr %"%PCI_SUBCLASS_FIREWIRE", align 8
  store i64 5, ptr %"%PCI_SUBCLASS_SMBUS", align 8
  store i64 0, ptr %"%PCI_PROGIF_UHCI", align 8
  store i64 16, ptr %"%PCI_PROGIF_OHCI", align 8
  store i64 32, ptr %"%PCI_PROGIF_EHCI", align 8
  store i64 48, ptr %"%PCI_PROGIF_XHCI", align 8
  store i64 8, ptr %"%PCI_MAX_BUS", align 8
  store i64 32, ptr %"%PCI_MAX_DEV", align 8
  store i64 8, ptr %"%PCI_MAX_FUNC", align 8
  store i64 65535, ptr %"%PCI_VENDOR_INVALID", align 8
  store i64 0, ptr %"%pci_dev_count", align 8
  store i64 0, ptr %"%pci_loc_0", align 8
  store i64 0, ptr %"%pci_loc_1", align 8
  store i64 0, ptr %"%pci_loc_2", align 8
  store i64 0, ptr %"%pci_loc_3", align 8
  store i64 0, ptr %"%pci_loc_4", align 8
  store i64 0, ptr %"%pci_loc_5", align 8
  store i64 0, ptr %"%pci_loc_6", align 8
  store i64 0, ptr %"%pci_loc_7", align 8
  store i64 0, ptr %"%pci_loc_8", align 8
  store i64 0, ptr %"%pci_loc_9", align 8
  store i64 0, ptr %"%pci_loc_10", align 8
  store i64 0, ptr %"%pci_loc_11", align 8
  store i64 0, ptr %"%pci_loc_12", align 8
  store i64 0, ptr %"%pci_loc_13", align 8
  store i64 0, ptr %"%pci_loc_14", align 8
  store i64 0, ptr %"%pci_loc_15", align 8
  store i64 0, ptr %"%pci_loc_16", align 8
  store i64 0, ptr %"%pci_loc_17", align 8
  store i64 0, ptr %"%pci_loc_18", align 8
  store i64 0, ptr %"%pci_loc_19", align 8
  store i64 0, ptr %"%pci_loc_20", align 8
  store i64 0, ptr %"%pci_loc_21", align 8
  store i64 0, ptr %"%pci_loc_22", align 8
  store i64 0, ptr %"%pci_loc_23", align 8
  store i64 0, ptr %"%pci_loc_24", align 8
  store i64 0, ptr %"%pci_loc_25", align 8
  store i64 0, ptr %"%pci_loc_26", align 8
  store i64 0, ptr %"%pci_loc_27", align 8
  store i64 0, ptr %"%pci_loc_28", align 8
  store i64 0, ptr %"%pci_loc_29", align 8
  store i64 0, ptr %"%pci_loc_30", align 8
  store i64 0, ptr %"%pci_loc_31", align 8
  store i64 0, ptr %"%pci_loc_32", align 8
  store i64 0, ptr %"%pci_loc_33", align 8
  store i64 0, ptr %"%pci_loc_34", align 8
  store i64 0, ptr %"%pci_loc_35", align 8
  store i64 0, ptr %"%pci_loc_36", align 8
  store i64 0, ptr %"%pci_loc_37", align 8
  store i64 0, ptr %"%pci_loc_38", align 8
  store i64 0, ptr %"%pci_loc_39", align 8
  store i64 0, ptr %"%pci_loc_40", align 8
  store i64 0, ptr %"%pci_loc_41", align 8
  store i64 0, ptr %"%pci_loc_42", align 8
  store i64 0, ptr %"%pci_loc_43", align 8
  store i64 0, ptr %"%pci_loc_44", align 8
  store i64 0, ptr %"%pci_loc_45", align 8
  store i64 0, ptr %"%pci_loc_46", align 8
  store i64 0, ptr %"%pci_loc_47", align 8
  store i64 0, ptr %"%pci_loc_48", align 8
  store i64 0, ptr %"%pci_loc_49", align 8
  store i64 0, ptr %"%pci_loc_50", align 8
  store i64 0, ptr %"%pci_loc_51", align 8
  store i64 0, ptr %"%pci_loc_52", align 8
  store i64 0, ptr %"%pci_loc_53", align 8
  store i64 0, ptr %"%pci_loc_54", align 8
  store i64 0, ptr %"%pci_loc_55", align 8
  store i64 0, ptr %"%pci_loc_56", align 8
  store i64 0, ptr %"%pci_loc_57", align 8
  store i64 0, ptr %"%pci_loc_58", align 8
  store i64 0, ptr %"%pci_loc_59", align 8
  store i64 0, ptr %"%pci_loc_60", align 8
  store i64 0, ptr %"%pci_loc_61", align 8
  store i64 0, ptr %"%pci_loc_62", align 8
  store i64 0, ptr %"%pci_loc_63", align 8
  store i64 0, ptr %"%pci_vid_0", align 8
  store i64 0, ptr %"%pci_vid_1", align 8
  store i64 0, ptr %"%pci_vid_2", align 8
  store i64 0, ptr %"%pci_vid_3", align 8
  store i64 0, ptr %"%pci_vid_4", align 8
  store i64 0, ptr %"%pci_vid_5", align 8
  store i64 0, ptr %"%pci_vid_6", align 8
  store i64 0, ptr %"%pci_vid_7", align 8
  store i64 0, ptr %"%pci_vid_8", align 8
  store i64 0, ptr %"%pci_vid_9", align 8
  store i64 0, ptr %"%pci_vid_10", align 8
  store i64 0, ptr %"%pci_vid_11", align 8
  store i64 0, ptr %"%pci_vid_12", align 8
  store i64 0, ptr %"%pci_vid_13", align 8
  store i64 0, ptr %"%pci_vid_14", align 8
  store i64 0, ptr %"%pci_vid_15", align 8
  store i64 0, ptr %"%pci_vid_16", align 8
  store i64 0, ptr %"%pci_vid_17", align 8
  store i64 0, ptr %"%pci_vid_18", align 8
  store i64 0, ptr %"%pci_vid_19", align 8
  store i64 0, ptr %"%pci_vid_20", align 8
  store i64 0, ptr %"%pci_vid_21", align 8
  store i64 0, ptr %"%pci_vid_22", align 8
  store i64 0, ptr %"%pci_vid_23", align 8
  store i64 0, ptr %"%pci_vid_24", align 8
  store i64 0, ptr %"%pci_vid_25", align 8
  store i64 0, ptr %"%pci_vid_26", align 8
  store i64 0, ptr %"%pci_vid_27", align 8
  store i64 0, ptr %"%pci_vid_28", align 8
  store i64 0, ptr %"%pci_vid_29", align 8
  store i64 0, ptr %"%pci_vid_30", align 8
  store i64 0, ptr %"%pci_vid_31", align 8
  store i64 0, ptr %"%pci_vid_32", align 8
  store i64 0, ptr %"%pci_vid_33", align 8
  store i64 0, ptr %"%pci_vid_34", align 8
  store i64 0, ptr %"%pci_vid_35", align 8
  store i64 0, ptr %"%pci_vid_36", align 8
  store i64 0, ptr %"%pci_vid_37", align 8
  store i64 0, ptr %"%pci_vid_38", align 8
  store i64 0, ptr %"%pci_vid_39", align 8
  store i64 0, ptr %"%pci_vid_40", align 8
  store i64 0, ptr %"%pci_vid_41", align 8
  store i64 0, ptr %"%pci_vid_42", align 8
  store i64 0, ptr %"%pci_vid_43", align 8
  store i64 0, ptr %"%pci_vid_44", align 8
  store i64 0, ptr %"%pci_vid_45", align 8
  store i64 0, ptr %"%pci_vid_46", align 8
  store i64 0, ptr %"%pci_vid_47", align 8
  store i64 0, ptr %"%pci_vid_48", align 8
  store i64 0, ptr %"%pci_vid_49", align 8
  store i64 0, ptr %"%pci_vid_50", align 8
  store i64 0, ptr %"%pci_vid_51", align 8
  store i64 0, ptr %"%pci_vid_52", align 8
  store i64 0, ptr %"%pci_vid_53", align 8
  store i64 0, ptr %"%pci_vid_54", align 8
  store i64 0, ptr %"%pci_vid_55", align 8
  store i64 0, ptr %"%pci_vid_56", align 8
  store i64 0, ptr %"%pci_vid_57", align 8
  store i64 0, ptr %"%pci_vid_58", align 8
  store i64 0, ptr %"%pci_vid_59", align 8
  store i64 0, ptr %"%pci_vid_60", align 8
  store i64 0, ptr %"%pci_vid_61", align 8
  store i64 0, ptr %"%pci_vid_62", align 8
  store i64 0, ptr %"%pci_vid_63", align 8
  store i64 0, ptr %"%pci_did_0", align 8
  store i64 0, ptr %"%pci_did_1", align 8
  store i64 0, ptr %"%pci_did_2", align 8
  store i64 0, ptr %"%pci_did_3", align 8
  store i64 0, ptr %"%pci_did_4", align 8
  store i64 0, ptr %"%pci_did_5", align 8
  store i64 0, ptr %"%pci_did_6", align 8
  store i64 0, ptr %"%pci_did_7", align 8
  store i64 0, ptr %"%pci_did_8", align 8
  store i64 0, ptr %"%pci_did_9", align 8
  store i64 0, ptr %"%pci_did_10", align 8
  store i64 0, ptr %"%pci_did_11", align 8
  store i64 0, ptr %"%pci_did_12", align 8
  store i64 0, ptr %"%pci_did_13", align 8
  store i64 0, ptr %"%pci_did_14", align 8
  store i64 0, ptr %"%pci_did_15", align 8
  store i64 0, ptr %"%pci_did_16", align 8
  store i64 0, ptr %"%pci_did_17", align 8
  store i64 0, ptr %"%pci_did_18", align 8
  store i64 0, ptr %"%pci_did_19", align 8
  store i64 0, ptr %"%pci_did_20", align 8
  store i64 0, ptr %"%pci_did_21", align 8
  store i64 0, ptr %"%pci_did_22", align 8
  store i64 0, ptr %"%pci_did_23", align 8
  store i64 0, ptr %"%pci_did_24", align 8
  store i64 0, ptr %"%pci_did_25", align 8
  store i64 0, ptr %"%pci_did_26", align 8
  store i64 0, ptr %"%pci_did_27", align 8
  store i64 0, ptr %"%pci_did_28", align 8
  store i64 0, ptr %"%pci_did_29", align 8
  store i64 0, ptr %"%pci_did_30", align 8
  store i64 0, ptr %"%pci_did_31", align 8
  store i64 0, ptr %"%pci_did_32", align 8
  store i64 0, ptr %"%pci_did_33", align 8
  store i64 0, ptr %"%pci_did_34", align 8
  store i64 0, ptr %"%pci_did_35", align 8
  store i64 0, ptr %"%pci_did_36", align 8
  store i64 0, ptr %"%pci_did_37", align 8
  store i64 0, ptr %"%pci_did_38", align 8
  store i64 0, ptr %"%pci_did_39", align 8
  store i64 0, ptr %"%pci_did_40", align 8
  store i64 0, ptr %"%pci_did_41", align 8
  store i64 0, ptr %"%pci_did_42", align 8
  store i64 0, ptr %"%pci_did_43", align 8
  store i64 0, ptr %"%pci_did_44", align 8
  store i64 0, ptr %"%pci_did_45", align 8
  store i64 0, ptr %"%pci_did_46", align 8
  store i64 0, ptr %"%pci_did_47", align 8
  store i64 0, ptr %"%pci_did_48", align 8
  store i64 0, ptr %"%pci_did_49", align 8
  store i64 0, ptr %"%pci_did_50", align 8
  store i64 0, ptr %"%pci_did_51", align 8
  store i64 0, ptr %"%pci_did_52", align 8
  store i64 0, ptr %"%pci_did_53", align 8
  store i64 0, ptr %"%pci_did_54", align 8
  store i64 0, ptr %"%pci_did_55", align 8
  store i64 0, ptr %"%pci_did_56", align 8
  store i64 0, ptr %"%pci_did_57", align 8
  store i64 0, ptr %"%pci_did_58", align 8
  store i64 0, ptr %"%pci_did_59", align 8
  store i64 0, ptr %"%pci_did_60", align 8
  store i64 0, ptr %"%pci_did_61", align 8
  store i64 0, ptr %"%pci_did_62", align 8
  store i64 0, ptr %"%pci_did_63", align 8
  store i64 0, ptr %"%pci_class_0", align 8
  store i64 0, ptr %"%pci_class_1", align 8
  store i64 0, ptr %"%pci_class_2", align 8
  store i64 0, ptr %"%pci_class_3", align 8
  store i64 0, ptr %"%pci_class_4", align 8
  store i64 0, ptr %"%pci_class_5", align 8
  store i64 0, ptr %"%pci_class_6", align 8
  store i64 0, ptr %"%pci_class_7", align 8
  store i64 0, ptr %"%pci_class_8", align 8
  store i64 0, ptr %"%pci_class_9", align 8
  store i64 0, ptr %"%pci_class_10", align 8
  store i64 0, ptr %"%pci_class_11", align 8
  store i64 0, ptr %"%pci_class_12", align 8
  store i64 0, ptr %"%pci_class_13", align 8
  store i64 0, ptr %"%pci_class_14", align 8
  store i64 0, ptr %"%pci_class_15", align 8
  store i64 0, ptr %"%pci_class_16", align 8
  store i64 0, ptr %"%pci_class_17", align 8
  store i64 0, ptr %"%pci_class_18", align 8
  store i64 0, ptr %"%pci_class_19", align 8
  store i64 0, ptr %"%pci_class_20", align 8
  store i64 0, ptr %"%pci_class_21", align 8
  store i64 0, ptr %"%pci_class_22", align 8
  store i64 0, ptr %"%pci_class_23", align 8
  store i64 0, ptr %"%pci_class_24", align 8
  store i64 0, ptr %"%pci_class_25", align 8
  store i64 0, ptr %"%pci_class_26", align 8
  store i64 0, ptr %"%pci_class_27", align 8
  store i64 0, ptr %"%pci_class_28", align 8
  store i64 0, ptr %"%pci_class_29", align 8
  store i64 0, ptr %"%pci_class_30", align 8
  store i64 0, ptr %"%pci_class_31", align 8
  store i64 0, ptr %"%pci_class_32", align 8
  store i64 0, ptr %"%pci_class_33", align 8
  store i64 0, ptr %"%pci_class_34", align 8
  store i64 0, ptr %"%pci_class_35", align 8
  store i64 0, ptr %"%pci_class_36", align 8
  store i64 0, ptr %"%pci_class_37", align 8
  store i64 0, ptr %"%pci_class_38", align 8
  store i64 0, ptr %"%pci_class_39", align 8
  store i64 0, ptr %"%pci_class_40", align 8
  store i64 0, ptr %"%pci_class_41", align 8
  store i64 0, ptr %"%pci_class_42", align 8
  store i64 0, ptr %"%pci_class_43", align 8
  store i64 0, ptr %"%pci_class_44", align 8
  store i64 0, ptr %"%pci_class_45", align 8
  store i64 0, ptr %"%pci_class_46", align 8
  store i64 0, ptr %"%pci_class_47", align 8
  store i64 0, ptr %"%pci_class_48", align 8
  store i64 0, ptr %"%pci_class_49", align 8
  store i64 0, ptr %"%pci_class_50", align 8
  store i64 0, ptr %"%pci_class_51", align 8
  store i64 0, ptr %"%pci_class_52", align 8
  store i64 0, ptr %"%pci_class_53", align 8
  store i64 0, ptr %"%pci_class_54", align 8
  store i64 0, ptr %"%pci_class_55", align 8
  store i64 0, ptr %"%pci_class_56", align 8
  store i64 0, ptr %"%pci_class_57", align 8
  store i64 0, ptr %"%pci_class_58", align 8
  store i64 0, ptr %"%pci_class_59", align 8
  store i64 0, ptr %"%pci_class_60", align 8
  store i64 0, ptr %"%pci_class_61", align 8
  store i64 0, ptr %"%pci_class_62", align 8
  store i64 0, ptr %"%pci_class_63", align 8
  store i64 0, ptr %"%pci_bar0_0", align 8
  store i64 0, ptr %"%pci_bar0_1", align 8
  store i64 0, ptr %"%pci_bar0_2", align 8
  store i64 0, ptr %"%pci_bar0_3", align 8
  store i64 0, ptr %"%pci_bar0_4", align 8
  store i64 0, ptr %"%pci_bar0_5", align 8
  store i64 0, ptr %"%pci_bar0_6", align 8
  store i64 0, ptr %"%pci_bar0_7", align 8
  store i64 0, ptr %"%pci_bar0_8", align 8
  store i64 0, ptr %"%pci_bar0_9", align 8
  store i64 0, ptr %"%pci_bar0_10", align 8
  store i64 0, ptr %"%pci_bar0_11", align 8
  store i64 0, ptr %"%pci_bar0_12", align 8
  store i64 0, ptr %"%pci_bar0_13", align 8
  store i64 0, ptr %"%pci_bar0_14", align 8
  store i64 0, ptr %"%pci_bar0_15", align 8
  store i64 0, ptr %"%pci_bar0_16", align 8
  store i64 0, ptr %"%pci_bar0_17", align 8
  store i64 0, ptr %"%pci_bar0_18", align 8
  store i64 0, ptr %"%pci_bar0_19", align 8
  store i64 0, ptr %"%pci_bar0_20", align 8
  store i64 0, ptr %"%pci_bar0_21", align 8
  store i64 0, ptr %"%pci_bar0_22", align 8
  store i64 0, ptr %"%pci_bar0_23", align 8
  store i64 0, ptr %"%pci_bar0_24", align 8
  store i64 0, ptr %"%pci_bar0_25", align 8
  store i64 0, ptr %"%pci_bar0_26", align 8
  store i64 0, ptr %"%pci_bar0_27", align 8
  store i64 0, ptr %"%pci_bar0_28", align 8
  store i64 0, ptr %"%pci_bar0_29", align 8
  store i64 0, ptr %"%pci_bar0_30", align 8
  store i64 0, ptr %"%pci_bar0_31", align 8
  store i64 0, ptr %"%pci_bar0_32", align 8
  store i64 0, ptr %"%pci_bar0_33", align 8
  store i64 0, ptr %"%pci_bar0_34", align 8
  store i64 0, ptr %"%pci_bar0_35", align 8
  store i64 0, ptr %"%pci_bar0_36", align 8
  store i64 0, ptr %"%pci_bar0_37", align 8
  store i64 0, ptr %"%pci_bar0_38", align 8
  store i64 0, ptr %"%pci_bar0_39", align 8
  store i64 0, ptr %"%pci_bar0_40", align 8
  store i64 0, ptr %"%pci_bar0_41", align 8
  store i64 0, ptr %"%pci_bar0_42", align 8
  store i64 0, ptr %"%pci_bar0_43", align 8
  store i64 0, ptr %"%pci_bar0_44", align 8
  store i64 0, ptr %"%pci_bar0_45", align 8
  store i64 0, ptr %"%pci_bar0_46", align 8
  store i64 0, ptr %"%pci_bar0_47", align 8
  store i64 0, ptr %"%pci_bar0_48", align 8
  store i64 0, ptr %"%pci_bar0_49", align 8
  store i64 0, ptr %"%pci_bar0_50", align 8
  store i64 0, ptr %"%pci_bar0_51", align 8
  store i64 0, ptr %"%pci_bar0_52", align 8
  store i64 0, ptr %"%pci_bar0_53", align 8
  store i64 0, ptr %"%pci_bar0_54", align 8
  store i64 0, ptr %"%pci_bar0_55", align 8
  store i64 0, ptr %"%pci_bar0_56", align 8
  store i64 0, ptr %"%pci_bar0_57", align 8
  store i64 0, ptr %"%pci_bar0_58", align 8
  store i64 0, ptr %"%pci_bar0_59", align 8
  store i64 0, ptr %"%pci_bar0_60", align 8
  store i64 0, ptr %"%pci_bar0_61", align 8
  store i64 0, ptr %"%pci_bar0_62", align 8
  store i64 0, ptr %"%pci_bar0_63", align 8
  store i64 0, ptr %"%pci_bar1_0", align 8
  store i64 0, ptr %"%pci_bar1_1", align 8
  store i64 0, ptr %"%pci_bar1_2", align 8
  store i64 0, ptr %"%pci_bar1_3", align 8
  store i64 0, ptr %"%pci_bar1_4", align 8
  store i64 0, ptr %"%pci_bar1_5", align 8
  store i64 0, ptr %"%pci_bar1_6", align 8
  store i64 0, ptr %"%pci_bar1_7", align 8
  store i64 0, ptr %"%pci_bar1_8", align 8
  store i64 0, ptr %"%pci_bar1_9", align 8
  store i64 0, ptr %"%pci_bar1_10", align 8
  store i64 0, ptr %"%pci_bar1_11", align 8
  store i64 0, ptr %"%pci_bar1_12", align 8
  store i64 0, ptr %"%pci_bar1_13", align 8
  store i64 0, ptr %"%pci_bar1_14", align 8
  store i64 0, ptr %"%pci_bar1_15", align 8
  store i64 0, ptr %"%pci_bar1_16", align 8
  store i64 0, ptr %"%pci_bar1_17", align 8
  store i64 0, ptr %"%pci_bar1_18", align 8
  store i64 0, ptr %"%pci_bar1_19", align 8
  store i64 0, ptr %"%pci_bar1_20", align 8
  store i64 0, ptr %"%pci_bar1_21", align 8
  store i64 0, ptr %"%pci_bar1_22", align 8
  store i64 0, ptr %"%pci_bar1_23", align 8
  store i64 0, ptr %"%pci_bar1_24", align 8
  store i64 0, ptr %"%pci_bar1_25", align 8
  store i64 0, ptr %"%pci_bar1_26", align 8
  store i64 0, ptr %"%pci_bar1_27", align 8
  store i64 0, ptr %"%pci_bar1_28", align 8
  store i64 0, ptr %"%pci_bar1_29", align 8
  store i64 0, ptr %"%pci_bar1_30", align 8
  store i64 0, ptr %"%pci_bar1_31", align 8
  store i64 0, ptr %"%pci_bar1_32", align 8
  store i64 0, ptr %"%pci_bar1_33", align 8
  store i64 0, ptr %"%pci_bar1_34", align 8
  store i64 0, ptr %"%pci_bar1_35", align 8
  store i64 0, ptr %"%pci_bar1_36", align 8
  store i64 0, ptr %"%pci_bar1_37", align 8
  store i64 0, ptr %"%pci_bar1_38", align 8
  store i64 0, ptr %"%pci_bar1_39", align 8
  store i64 0, ptr %"%pci_bar1_40", align 8
  store i64 0, ptr %"%pci_bar1_41", align 8
  store i64 0, ptr %"%pci_bar1_42", align 8
  store i64 0, ptr %"%pci_bar1_43", align 8
  store i64 0, ptr %"%pci_bar1_44", align 8
  store i64 0, ptr %"%pci_bar1_45", align 8
  store i64 0, ptr %"%pci_bar1_46", align 8
  store i64 0, ptr %"%pci_bar1_47", align 8
  store i64 0, ptr %"%pci_bar1_48", align 8
  store i64 0, ptr %"%pci_bar1_49", align 8
  store i64 0, ptr %"%pci_bar1_50", align 8
  store i64 0, ptr %"%pci_bar1_51", align 8
  store i64 0, ptr %"%pci_bar1_52", align 8
  store i64 0, ptr %"%pci_bar1_53", align 8
  store i64 0, ptr %"%pci_bar1_54", align 8
  store i64 0, ptr %"%pci_bar1_55", align 8
  store i64 0, ptr %"%pci_bar1_56", align 8
  store i64 0, ptr %"%pci_bar1_57", align 8
  store i64 0, ptr %"%pci_bar1_58", align 8
  store i64 0, ptr %"%pci_bar1_59", align 8
  store i64 0, ptr %"%pci_bar1_60", align 8
  store i64 0, ptr %"%pci_bar1_61", align 8
  store i64 0, ptr %"%pci_bar1_62", align 8
  store i64 0, ptr %"%pci_bar1_63", align 8
  store i64 0, ptr %"%pci_irq_0", align 8
  store i64 0, ptr %"%pci_irq_1", align 8
  store i64 0, ptr %"%pci_irq_2", align 8
  store i64 0, ptr %"%pci_irq_3", align 8
  store i64 0, ptr %"%pci_irq_4", align 8
  store i64 0, ptr %"%pci_irq_5", align 8
  store i64 0, ptr %"%pci_irq_6", align 8
  store i64 0, ptr %"%pci_irq_7", align 8
  store i64 0, ptr %"%pci_irq_8", align 8
  store i64 0, ptr %"%pci_irq_9", align 8
  store i64 0, ptr %"%pci_irq_10", align 8
  store i64 0, ptr %"%pci_irq_11", align 8
  store i64 0, ptr %"%pci_irq_12", align 8
  store i64 0, ptr %"%pci_irq_13", align 8
  store i64 0, ptr %"%pci_irq_14", align 8
  store i64 0, ptr %"%pci_irq_15", align 8
  store i64 0, ptr %"%pci_irq_16", align 8
  store i64 0, ptr %"%pci_irq_17", align 8
  store i64 0, ptr %"%pci_irq_18", align 8
  store i64 0, ptr %"%pci_irq_19", align 8
  store i64 0, ptr %"%pci_irq_20", align 8
  store i64 0, ptr %"%pci_irq_21", align 8
  store i64 0, ptr %"%pci_irq_22", align 8
  store i64 0, ptr %"%pci_irq_23", align 8
  store i64 0, ptr %"%pci_irq_24", align 8
  store i64 0, ptr %"%pci_irq_25", align 8
  store i64 0, ptr %"%pci_irq_26", align 8
  store i64 0, ptr %"%pci_irq_27", align 8
  store i64 0, ptr %"%pci_irq_28", align 8
  store i64 0, ptr %"%pci_irq_29", align 8
  store i64 0, ptr %"%pci_irq_30", align 8
  store i64 0, ptr %"%pci_irq_31", align 8
  store i64 0, ptr %"%pci_irq_32", align 8
  store i64 0, ptr %"%pci_irq_33", align 8
  store i64 0, ptr %"%pci_irq_34", align 8
  store i64 0, ptr %"%pci_irq_35", align 8
  store i64 0, ptr %"%pci_irq_36", align 8
  store i64 0, ptr %"%pci_irq_37", align 8
  store i64 0, ptr %"%pci_irq_38", align 8
  store i64 0, ptr %"%pci_irq_39", align 8
  store i64 0, ptr %"%pci_irq_40", align 8
  store i64 0, ptr %"%pci_irq_41", align 8
  store i64 0, ptr %"%pci_irq_42", align 8
  store i64 0, ptr %"%pci_irq_43", align 8
  store i64 0, ptr %"%pci_irq_44", align 8
  store i64 0, ptr %"%pci_irq_45", align 8
  store i64 0, ptr %"%pci_irq_46", align 8
  store i64 0, ptr %"%pci_irq_47", align 8
  store i64 0, ptr %"%pci_irq_48", align 8
  store i64 0, ptr %"%pci_irq_49", align 8
  store i64 0, ptr %"%pci_irq_50", align 8
  store i64 0, ptr %"%pci_irq_51", align 8
  store i64 0, ptr %"%pci_irq_52", align 8
  store i64 0, ptr %"%pci_irq_53", align 8
  store i64 0, ptr %"%pci_irq_54", align 8
  store i64 0, ptr %"%pci_irq_55", align 8
  store i64 0, ptr %"%pci_irq_56", align 8
  store i64 0, ptr %"%pci_irq_57", align 8
  store i64 0, ptr %"%pci_irq_58", align 8
  store i64 0, ptr %"%pci_irq_59", align 8
  store i64 0, ptr %"%pci_irq_60", align 8
  store i64 0, ptr %"%pci_irq_61", align 8
  store i64 0, ptr %"%pci_irq_62", align 8
  store i64 0, ptr %"%pci_irq_63", align 8
  store i64 0, ptr %"%pci_usb_count", align 8
  store i64 0, ptr %"%pci_net_count", align 8
  store i64 0, ptr %"%pci_disk_count", align 8
  store i64 0, ptr %"%pci_display_count", align 8
  store i64 0, ptr %"%pci_audio_count", align 8
  store i64 0, ptr %"%pci_bridge_count", align 8
  store i64 0, ptr %"%pci_other_count", align 8
  store i64 5, ptr %"%PCI_CAP_MSI", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
