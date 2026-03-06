; ModuleID = 'usb_نحلة'
source_filename = "usb_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@COM1 = constant i64 1016
@USB_REQ_HOST_TO_DEV = constant i64 0
@USB_REQ_DEV_TO_HOST = constant i64 128
@USB_REQ_TYPE_STD = constant i64 0
@USB_REQ_TYPE_CLASS = constant i64 32
@USB_REQ_TYPE_VENDOR = constant i64 64
@USB_REQ_RECIP_DEV = constant i64 0
@USB_REQ_RECIP_IFACE = constant i64 1
@USB_REQ_RECIP_EP = constant i64 2
@USB_REQ_GET_STATUS = constant i64 0
@USB_REQ_CLEAR_FEATURE = constant i64 1
@USB_REQ_SET_FEATURE = constant i64 3
@USB_REQ_SET_ADDRESS = constant i64 5
@USB_REQ_GET_DESCRIPTOR = constant i64 6
@USB_REQ_SET_DESCRIPTOR = constant i64 7
@USB_REQ_GET_CONFIG = constant i64 8
@USB_REQ_SET_CONFIG = constant i64 9
@USB_REQ_GET_INTERFACE = constant i64 10
@USB_REQ_SET_INTERFACE = constant i64 11
@USB_DESC_DEVICE = constant i64 1
@USB_DESC_CONFIG = constant i64 2
@USB_DESC_STRING = constant i64 3
@USB_DESC_INTERFACE = constant i64 4
@USB_DESC_ENDPOINT = constant i64 5
@USB_DESC_HID = constant i64 33
@USB_DESC_REPORT = constant i64 34
@USB_DESC_HUB = constant i64 41
@USB_CLASS_HUB = constant i64 9
@USB_CLASS_HID = constant i64 3
@USB_CLASS_STORAGE = constant i64 8
@USB_CLASS_AUDIO = constant i64 1
@USB_CLASS_VIDEO = constant i64 14
@USB_CLASS_CDC = constant i64 2
@USB_CLASS_PRINTER = constant i64 7
@USB_CLASS_WIRELESS = constant i64 224
@USB_CLASS_VENDOR = constant i64 255
@USB_SUBCLASS_BOOT = constant i64 1
@USB_PROTOCOL_KEYBOARD = constant i64 1
@USB_PROTOCOL_MOUSE = constant i64 2
@USB_SUBCLASS_SCSI = constant i64 6
@USB_PROTOCOL_BBB = constant i64 80
@USB_EP_CONTROL = constant i64 0
@USB_EP_ISOCHRONOUS = constant i64 1
@USB_EP_BULK = constant i64 2
@USB_EP_INTERRUPT = constant i64 3
@USB_EP_DIR_OUT = constant i64 0
@USB_EP_DIR_IN = constant i64 128
@USB_SPEED_LOW = constant i64 0
@USB_SPEED_FULL = constant i64 1
@USB_SPEED_HIGH = constant i64 2
@USB_DEV_DISCONNECTED = constant i64 0
@USB_DEV_ATTACHED = constant i64 1
@USB_DEV_POWERED = constant i64 2
@USB_DEV_DEFAULT = constant i64 3
@USB_DEV_ADDRESSED = constant i64 4
@USB_DEV_CONFIGURED = constant i64 5
@USB_HC_UHCI = constant i64 0
@USB_HC_OHCI = constant i64 16
@USB_HC_EHCI = constant i64 32
@USB_HC_XHCI = constant i64 48
@USB_MAX_DEVICES = constant i64 32
@USB_MAX_ENDPOINTS = constant i64 8
@USB_MAX_CONTROLLERS = constant i64 4
@usb_dev_count = global i64 0
@usb_addr_0 = global i64 0
@usb_addr_1 = global i64 0
@usb_addr_2 = global i64 0
@usb_addr_3 = global i64 0
@usb_addr_4 = global i64 0
@usb_addr_5 = global i64 0
@usb_addr_6 = global i64 0
@usb_addr_7 = global i64 0
@usb_addr_8 = global i64 0
@usb_addr_9 = global i64 0
@usb_addr_10 = global i64 0
@usb_addr_11 = global i64 0
@usb_addr_12 = global i64 0
@usb_addr_13 = global i64 0
@usb_addr_14 = global i64 0
@usb_addr_15 = global i64 0
@usb_addr_16 = global i64 0
@usb_addr_17 = global i64 0
@usb_addr_18 = global i64 0
@usb_addr_19 = global i64 0
@usb_addr_20 = global i64 0
@usb_addr_21 = global i64 0
@usb_addr_22 = global i64 0
@usb_addr_23 = global i64 0
@usb_addr_24 = global i64 0
@usb_addr_25 = global i64 0
@usb_addr_26 = global i64 0
@usb_addr_27 = global i64 0
@usb_addr_28 = global i64 0
@usb_addr_29 = global i64 0
@usb_addr_30 = global i64 0
@usb_addr_31 = global i64 0
@usb_state_0 = global i64 0
@usb_state_1 = global i64 0
@usb_state_2 = global i64 0
@usb_state_3 = global i64 0
@usb_state_4 = global i64 0
@usb_state_5 = global i64 0
@usb_state_6 = global i64 0
@usb_state_7 = global i64 0
@usb_state_8 = global i64 0
@usb_state_9 = global i64 0
@usb_state_10 = global i64 0
@usb_state_11 = global i64 0
@usb_state_12 = global i64 0
@usb_state_13 = global i64 0
@usb_state_14 = global i64 0
@usb_state_15 = global i64 0
@usb_state_16 = global i64 0
@usb_state_17 = global i64 0
@usb_state_18 = global i64 0
@usb_state_19 = global i64 0
@usb_state_20 = global i64 0
@usb_state_21 = global i64 0
@usb_state_22 = global i64 0
@usb_state_23 = global i64 0
@usb_state_24 = global i64 0
@usb_state_25 = global i64 0
@usb_state_26 = global i64 0
@usb_state_27 = global i64 0
@usb_state_28 = global i64 0
@usb_state_29 = global i64 0
@usb_state_30 = global i64 0
@usb_state_31 = global i64 0
@usb_speed_0 = global i64 0
@usb_speed_1 = global i64 0
@usb_speed_2 = global i64 0
@usb_speed_3 = global i64 0
@usb_speed_4 = global i64 0
@usb_speed_5 = global i64 0
@usb_speed_6 = global i64 0
@usb_speed_7 = global i64 0
@usb_speed_8 = global i64 0
@usb_speed_9 = global i64 0
@usb_speed_10 = global i64 0
@usb_speed_11 = global i64 0
@usb_speed_12 = global i64 0
@usb_speed_13 = global i64 0
@usb_speed_14 = global i64 0
@usb_speed_15 = global i64 0
@usb_speed_16 = global i64 0
@usb_speed_17 = global i64 0
@usb_speed_18 = global i64 0
@usb_speed_19 = global i64 0
@usb_speed_20 = global i64 0
@usb_speed_21 = global i64 0
@usb_speed_22 = global i64 0
@usb_speed_23 = global i64 0
@usb_speed_24 = global i64 0
@usb_speed_25 = global i64 0
@usb_speed_26 = global i64 0
@usb_speed_27 = global i64 0
@usb_speed_28 = global i64 0
@usb_speed_29 = global i64 0
@usb_speed_30 = global i64 0
@usb_speed_31 = global i64 0
@usb_class_0 = global i64 0
@usb_class_1 = global i64 0
@usb_class_2 = global i64 0
@usb_class_3 = global i64 0
@usb_class_4 = global i64 0
@usb_class_5 = global i64 0
@usb_class_6 = global i64 0
@usb_class_7 = global i64 0
@usb_class_8 = global i64 0
@usb_class_9 = global i64 0
@usb_class_10 = global i64 0
@usb_class_11 = global i64 0
@usb_class_12 = global i64 0
@usb_class_13 = global i64 0
@usb_class_14 = global i64 0
@usb_class_15 = global i64 0
@usb_class_16 = global i64 0
@usb_class_17 = global i64 0
@usb_class_18 = global i64 0
@usb_class_19 = global i64 0
@usb_class_20 = global i64 0
@usb_class_21 = global i64 0
@usb_class_22 = global i64 0
@usb_class_23 = global i64 0
@usb_class_24 = global i64 0
@usb_class_25 = global i64 0
@usb_class_26 = global i64 0
@usb_class_27 = global i64 0
@usb_class_28 = global i64 0
@usb_class_29 = global i64 0
@usb_class_30 = global i64 0
@usb_class_31 = global i64 0
@usb_proto_0 = global i64 0
@usb_proto_1 = global i64 0
@usb_proto_2 = global i64 0
@usb_proto_3 = global i64 0
@usb_proto_4 = global i64 0
@usb_proto_5 = global i64 0
@usb_proto_6 = global i64 0
@usb_proto_7 = global i64 0
@usb_proto_8 = global i64 0
@usb_proto_9 = global i64 0
@usb_proto_10 = global i64 0
@usb_proto_11 = global i64 0
@usb_proto_12 = global i64 0
@usb_proto_13 = global i64 0
@usb_proto_14 = global i64 0
@usb_proto_15 = global i64 0
@usb_proto_16 = global i64 0
@usb_proto_17 = global i64 0
@usb_proto_18 = global i64 0
@usb_proto_19 = global i64 0
@usb_proto_20 = global i64 0
@usb_proto_21 = global i64 0
@usb_proto_22 = global i64 0
@usb_proto_23 = global i64 0
@usb_proto_24 = global i64 0
@usb_proto_25 = global i64 0
@usb_proto_26 = global i64 0
@usb_proto_27 = global i64 0
@usb_proto_28 = global i64 0
@usb_proto_29 = global i64 0
@usb_proto_30 = global i64 0
@usb_proto_31 = global i64 0
@usb_vid_0 = global i64 0
@usb_vid_1 = global i64 0
@usb_vid_2 = global i64 0
@usb_vid_3 = global i64 0
@usb_vid_4 = global i64 0
@usb_vid_5 = global i64 0
@usb_vid_6 = global i64 0
@usb_vid_7 = global i64 0
@usb_vid_8 = global i64 0
@usb_vid_9 = global i64 0
@usb_vid_10 = global i64 0
@usb_vid_11 = global i64 0
@usb_vid_12 = global i64 0
@usb_vid_13 = global i64 0
@usb_vid_14 = global i64 0
@usb_vid_15 = global i64 0
@usb_pid_0 = global i64 0
@usb_pid_1 = global i64 0
@usb_pid_2 = global i64 0
@usb_pid_3 = global i64 0
@usb_pid_4 = global i64 0
@usb_pid_5 = global i64 0
@usb_pid_6 = global i64 0
@usb_pid_7 = global i64 0
@usb_pid_8 = global i64 0
@usb_pid_9 = global i64 0
@usb_pid_10 = global i64 0
@usb_pid_11 = global i64 0
@usb_pid_12 = global i64 0
@usb_pid_13 = global i64 0
@usb_pid_14 = global i64 0
@usb_pid_15 = global i64 0
@usb_maxpkt_0 = global i64 8
@usb_maxpkt_1 = global i64 8
@usb_maxpkt_2 = global i64 8
@usb_maxpkt_3 = global i64 8
@usb_maxpkt_4 = global i64 8
@usb_maxpkt_5 = global i64 8
@usb_maxpkt_6 = global i64 8
@usb_maxpkt_7 = global i64 8
@usb_maxpkt_8 = global i64 8
@usb_maxpkt_9 = global i64 8
@usb_maxpkt_10 = global i64 8
@usb_maxpkt_11 = global i64 8
@usb_maxpkt_12 = global i64 8
@usb_maxpkt_13 = global i64 8
@usb_maxpkt_14 = global i64 8
@usb_maxpkt_15 = global i64 8
@usb_hc_0 = global i64 0
@usb_hc_1 = global i64 0
@usb_hc_2 = global i64 0
@usb_hc_3 = global i64 0
@usb_hc_4 = global i64 0
@usb_hc_5 = global i64 0
@usb_hc_6 = global i64 0
@usb_hc_7 = global i64 0
@usb_hc_8 = global i64 0
@usb_hc_9 = global i64 0
@usb_hc_10 = global i64 0
@usb_hc_11 = global i64 0
@usb_hc_12 = global i64 0
@usb_hc_13 = global i64 0
@usb_hc_14 = global i64 0
@usb_hc_15 = global i64 0
@usb_port_0 = global i64 0
@usb_port_1 = global i64 0
@usb_port_2 = global i64 0
@usb_port_3 = global i64 0
@usb_port_4 = global i64 0
@usb_port_5 = global i64 0
@usb_port_6 = global i64 0
@usb_port_7 = global i64 0
@usb_port_8 = global i64 0
@usb_port_9 = global i64 0
@usb_port_10 = global i64 0
@usb_port_11 = global i64 0
@usb_port_12 = global i64 0
@usb_port_13 = global i64 0
@usb_port_14 = global i64 0
@usb_port_15 = global i64 0
@usb_num_ep_0 = global i64 0
@usb_num_ep_1 = global i64 0
@usb_num_ep_2 = global i64 0
@usb_num_ep_3 = global i64 0
@usb_num_ep_4 = global i64 0
@usb_num_ep_5 = global i64 0
@usb_num_ep_6 = global i64 0
@usb_num_ep_7 = global i64 0
@usb_num_ep_8 = global i64 0
@usb_num_ep_9 = global i64 0
@usb_num_ep_10 = global i64 0
@usb_num_ep_11 = global i64 0
@usb_num_ep_12 = global i64 0
@usb_num_ep_13 = global i64 0
@usb_num_ep_14 = global i64 0
@usb_num_ep_15 = global i64 0
@usb_hc_count = global i64 0
@hc_type_0 = global i64 0
@hc_type_1 = global i64 0
@hc_type_2 = global i64 0
@hc_type_3 = global i64 0
@hc_base_0 = global i64 0
@hc_base_1 = global i64 0
@hc_base_2 = global i64 0
@hc_base_3 = global i64 0
@hc_irq_0 = global i64 0
@hc_irq_1 = global i64 0
@hc_irq_2 = global i64 0
@hc_irq_3 = global i64 0
@hc_pci_idx_0 = global i64 0
@hc_pci_idx_1 = global i64 0
@hc_pci_idx_2 = global i64 0
@hc_pci_idx_3 = global i64 0
@usb_next_address = global i64 1
@usb_total_transfers = global i64 0
@usb_transfer_errors = global i64 0
@usb_hid_devices = global i64 0
@usb_storage_devices = global i64 0
@usb_hub_devices = global i64 0
@usb_other_devices = global i64 0
@str.const = private unnamed_addr constant [26 x i8] c"[USB] Device Desc: VID=0x\00", align 1
@str.const.1 = private unnamed_addr constant [8 x i8] c" PID=0x\00", align 1
@str.const.2 = private unnamed_addr constant [8 x i8] c" Class=\00", align 1
@str.const.3 = private unnamed_addr constant [9 x i8] c" MaxPkt=\00", align 1
@str.const.4 = private unnamed_addr constant [26 x i8] c"[USB]   Interface: class=\00", align 1
@str.const.5 = private unnamed_addr constant [14 x i8] c"[USB]   EP 0x\00", align 1
@str.const.6 = private unnamed_addr constant [7 x i8] c" type=\00", align 1
@str.const.7 = private unnamed_addr constant [5 x i8] c"CTRL\00", align 1
@str.const.8 = private unnamed_addr constant [5 x i8] c"BULK\00", align 1
@str.const.9 = private unnamed_addr constant [5 x i8] c"INTR\00", align 1
@str.const.10 = private unnamed_addr constant [5 x i8] c"ISOC\00", align 1
@str.const.11 = private unnamed_addr constant [9 x i8] c" maxpkt=\00", align 1
@str.const.12 = private unnamed_addr constant [20 x i8] c"[USB]   -> Keyboard\00", align 1
@str.const.13 = private unnamed_addr constant [17 x i8] c"[USB]   -> Mouse\00", align 1
@str.const.14 = private unnamed_addr constant [22 x i8] c"[USB]   -> HID device\00", align 1
@str.const.15 = private unnamed_addr constant [24 x i8] c"[USB]   -> Mass Storage\00", align 1
@str.const.16 = private unnamed_addr constant [15 x i8] c"[USB]   -> Hub\00", align 1
@str.const.17 = private unnamed_addr constant [17 x i8] c"[USB]   -> Audio\00", align 1
@str.const.18 = private unnamed_addr constant [23 x i8] c"[USB]   -> CDC/Network\00", align 1
@str.const.19 = private unnamed_addr constant [19 x i8] c"[USB]   -> Printer\00", align 1
@str.const.20 = private unnamed_addr constant [27 x i8] c"[USB] Max devices reached!\00", align 1
@str.const.21 = private unnamed_addr constant [19 x i8] c"[USB] New device #\00", align 1
@str.const.22 = private unnamed_addr constant [7 x i8] c" addr=\00", align 1
@str.const.23 = private unnamed_addr constant [7 x i8] c" port=\00", align 1
@str.const.24 = private unnamed_addr constant [8 x i8] c" speed=\00", align 1
@str.const.25 = private unnamed_addr constant [4 x i8] c"Low\00", align 1
@str.const.26 = private unnamed_addr constant [5 x i8] c"Full\00", align 1
@str.const.27 = private unnamed_addr constant [5 x i8] c"High\00", align 1
@str.const.28 = private unnamed_addr constant [35 x i8] c"[USB] Detecting USB controllers...\00", align 1
@str.const.29 = private unnamed_addr constant [13 x i8] c"[USB] Found \00", align 1
@str.const.30 = private unnamed_addr constant [19 x i8] c" USB controller(s)\00", align 1
@str.const.31 = private unnamed_addr constant [11 x i8] c"[USB] HC #\00", align 1
@str.const.33 = private unnamed_addr constant [5 x i8] c"UHCI\00", align 1
@str.const.34 = private unnamed_addr constant [9 x i8] c" base=0x\00", align 1
@str.const.35 = private unnamed_addr constant [6 x i8] c" IRQ=\00", align 1
@str.const.36 = private unnamed_addr constant [5 x i8] c"OHCI\00", align 1
@str.const.37 = private unnamed_addr constant [5 x i8] c"EHCI\00", align 1
@str.const.38 = private unnamed_addr constant [5 x i8] c"XHCI\00", align 1
@str.const.39 = private unnamed_addr constant [8 x i8] c"Unknown\00", align 1
@str.const.40 = private unnamed_addr constant [37 x i8] c"\E2\95\90\E2\95\90\E2\95\90 [USB] \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.41 = private unnamed_addr constant [19 x i8] c"  \D9\85\D8\AA\D8\AD\D9\83\D9\85\D8\A7\D8\AA: \00", align 1
@str.const.42 = private unnamed_addr constant [15 x i8] c"  \D8\A3\D8\AC\D9\87\D8\B2\D8\A9: \00", align 1
@str.const.43 = private unnamed_addr constant [8 x i8] c"  HID: \00", align 1
@str.const.44 = private unnamed_addr constant [12 x i8] c"  Storage: \00", align 1
@str.const.45 = private unnamed_addr constant [9 x i8] c"  Hubs: \00", align 1
@str.const.46 = private unnamed_addr constant [10 x i8] c"  Other: \00", align 1
@str.const.47 = private unnamed_addr constant [20 x i8] c"  Total transfers: \00", align 1
@str.const.48 = private unnamed_addr constant [20 x i8] c"  Transfer errors: \00", align 1
@str.const.49 = private unnamed_addr constant [4 x i8] c"  [\00", align 1
@str.const.50 = private unnamed_addr constant [8 x i8] c"] addr=\00", align 1
@str.const.51 = private unnamed_addr constant [8 x i8] c" state=\00", align 1
@str.const.52 = private unnamed_addr constant [10 x i8] c" class=0x\00", align 1
@str.const.53 = private unnamed_addr constant [106 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.54 = private unnamed_addr constant [33 x i8] c"[USB] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D9\86\D8\B8\D8\A7\D9\85 USB...\00", align 1
@str.const.55 = private unnamed_addr constant [15 x i8] c"[USB] \D8\AC\D8\A7\D9\87\D8\B2\00", align 1

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

declare void @pci_find_usb_controller(i64)

declare void @pci_get_usb_type(i64)

declare void @pci_get_bar0(i64)

declare void @pci_get_bar1(i64)

declare void @pci_get_irq(i64)

declare void @pci_loc_bus(i64)

declare void @pci_loc_dev(i64)

declare void @pci_loc_func(i64)

declare void @pci_enable_bus_master(i64, i64, i64)

declare void @pci_bar_is_io(i64)

declare void @pci_bar_address(i64)

define i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
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

define i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
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

define i64 @usb_serial_hex8(i64 %val) {
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

define i64 @usb_serial_hex16(i64 %val) {
entry:
  %val1 = alloca i64, align 8
  store i64 %val, ptr %val1, align 8
  %"%val.load" = load i64, ptr %val1, align 8
  %shrtmp = lshr i64 %"%val.load", 8
  %calltmp = call i64 @usb_serial_hex8(i64 %shrtmp)
  %"%val.load2" = load i64, ptr %val1, align 8
  %andtmp = and i64 %"%val.load2", 255
  %calltmp3 = call i64 @usb_serial_hex8(i64 %andtmp)
  ret i64 0
}

define i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"() {
entry:
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 13)
  %"%COM1.load1" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load1", i64 10)
  ret i64 0
}

define i64 @usb_get_addr(i64 %idx) {
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
  ret i64 0

then_30:                                          ; preds = %entry
  %loadtmp = load i64, ptr @usb_addr_0, align 8
  ret i64 %loadtmp

then_32:                                          ; preds = %merge_31
  %loadtmp4 = load i64, ptr @usb_addr_1, align 8
  ret i64 %loadtmp4

then_34:                                          ; preds = %merge_33
  %loadtmp7 = load i64, ptr @usb_addr_2, align 8
  ret i64 %loadtmp7

then_36:                                          ; preds = %merge_35
  %loadtmp10 = load i64, ptr @usb_addr_3, align 8
  ret i64 %loadtmp10

then_38:                                          ; preds = %merge_37
  %loadtmp13 = load i64, ptr @usb_addr_4, align 8
  ret i64 %loadtmp13

then_40:                                          ; preds = %merge_39
  %loadtmp16 = load i64, ptr @usb_addr_5, align 8
  ret i64 %loadtmp16

then_42:                                          ; preds = %merge_41
  %loadtmp19 = load i64, ptr @usb_addr_6, align 8
  ret i64 %loadtmp19

then_44:                                          ; preds = %merge_43
  %loadtmp22 = load i64, ptr @usb_addr_7, align 8
  ret i64 %loadtmp22

then_46:                                          ; preds = %merge_45
  %loadtmp25 = load i64, ptr @usb_addr_8, align 8
  ret i64 %loadtmp25

then_48:                                          ; preds = %merge_47
  %loadtmp28 = load i64, ptr @usb_addr_9, align 8
  ret i64 %loadtmp28

then_50:                                          ; preds = %merge_49
  %loadtmp31 = load i64, ptr @usb_addr_10, align 8
  ret i64 %loadtmp31

then_52:                                          ; preds = %merge_51
  %loadtmp34 = load i64, ptr @usb_addr_11, align 8
  ret i64 %loadtmp34

then_54:                                          ; preds = %merge_53
  %loadtmp37 = load i64, ptr @usb_addr_12, align 8
  ret i64 %loadtmp37

then_56:                                          ; preds = %merge_55
  %loadtmp40 = load i64, ptr @usb_addr_13, align 8
  ret i64 %loadtmp40

then_58:                                          ; preds = %merge_57
  %loadtmp43 = load i64, ptr @usb_addr_14, align 8
  ret i64 %loadtmp43

then_60:                                          ; preds = %merge_59
  %loadtmp46 = load i64, ptr @usb_addr_15, align 8
  ret i64 %loadtmp46
}

define i64 @usb_set_addr(i64 %idx, i64 %val) {
entry:
  %"%usb_addr_15" = alloca i64, align 8
  %"%usb_addr_14" = alloca i64, align 8
  %"%usb_addr_13" = alloca i64, align 8
  %"%usb_addr_12" = alloca i64, align 8
  %"%usb_addr_11" = alloca i64, align 8
  %"%usb_addr_10" = alloca i64, align 8
  %"%usb_addr_9" = alloca i64, align 8
  %"%usb_addr_8" = alloca i64, align 8
  %"%usb_addr_7" = alloca i64, align 8
  %"%usb_addr_6" = alloca i64, align 8
  %"%usb_addr_5" = alloca i64, align 8
  %"%usb_addr_4" = alloca i64, align 8
  %"%usb_addr_3" = alloca i64, align 8
  %"%usb_addr_2" = alloca i64, align 8
  %"%usb_addr_1" = alloca i64, align 8
  %"%usb_addr_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_62, label %merge_63

merge_63:                                         ; preds = %then_62, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_64, label %merge_65

merge_65:                                         ; preds = %then_64, %merge_63
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_66, label %merge_67

merge_67:                                         ; preds = %then_66, %merge_65
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_68, label %merge_69

merge_69:                                         ; preds = %then_68, %merge_67
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_70, label %merge_71

merge_71:                                         ; preds = %then_70, %merge_69
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_72, label %merge_73

merge_73:                                         ; preds = %then_72, %merge_71
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_74, label %merge_75

merge_75:                                         ; preds = %then_74, %merge_73
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_76, label %merge_77

merge_77:                                         ; preds = %then_76, %merge_75
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_78, label %merge_79

merge_79:                                         ; preds = %then_78, %merge_77
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_80, label %merge_81

merge_81:                                         ; preds = %then_80, %merge_79
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_82, label %merge_83

merge_83:                                         ; preds = %then_82, %merge_81
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_84, label %merge_85

merge_85:                                         ; preds = %then_84, %merge_83
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_86, label %merge_87

merge_87:                                         ; preds = %then_86, %merge_85
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_88, label %merge_89

merge_89:                                         ; preds = %then_88, %merge_87
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_90, label %merge_91

merge_91:                                         ; preds = %then_90, %merge_89
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_92, label %merge_93

merge_93:                                         ; preds = %then_92, %merge_91
  ret i64 0

then_62:                                          ; preds = %entry
  %"%199.load" = load i64, ptr %val2, align 8
  store i64 %"%199.load", ptr %"%usb_addr_0", align 8
  br label %merge_63

then_64:                                          ; preds = %merge_63
  %"%203.load" = load i64, ptr %val2, align 8
  store i64 %"%203.load", ptr %"%usb_addr_1", align 8
  br label %merge_65

then_66:                                          ; preds = %merge_65
  %"%207.load" = load i64, ptr %val2, align 8
  store i64 %"%207.load", ptr %"%usb_addr_2", align 8
  br label %merge_67

then_68:                                          ; preds = %merge_67
  %"%211.load" = load i64, ptr %val2, align 8
  store i64 %"%211.load", ptr %"%usb_addr_3", align 8
  br label %merge_69

then_70:                                          ; preds = %merge_69
  %"%215.load" = load i64, ptr %val2, align 8
  store i64 %"%215.load", ptr %"%usb_addr_4", align 8
  br label %merge_71

then_72:                                          ; preds = %merge_71
  %"%219.load" = load i64, ptr %val2, align 8
  store i64 %"%219.load", ptr %"%usb_addr_5", align 8
  br label %merge_73

then_74:                                          ; preds = %merge_73
  %"%223.load" = load i64, ptr %val2, align 8
  store i64 %"%223.load", ptr %"%usb_addr_6", align 8
  br label %merge_75

then_76:                                          ; preds = %merge_75
  %"%227.load" = load i64, ptr %val2, align 8
  store i64 %"%227.load", ptr %"%usb_addr_7", align 8
  br label %merge_77

then_78:                                          ; preds = %merge_77
  %"%231.load" = load i64, ptr %val2, align 8
  store i64 %"%231.load", ptr %"%usb_addr_8", align 8
  br label %merge_79

then_80:                                          ; preds = %merge_79
  %"%235.load" = load i64, ptr %val2, align 8
  store i64 %"%235.load", ptr %"%usb_addr_9", align 8
  br label %merge_81

then_82:                                          ; preds = %merge_81
  %"%239.load" = load i64, ptr %val2, align 8
  store i64 %"%239.load", ptr %"%usb_addr_10", align 8
  br label %merge_83

then_84:                                          ; preds = %merge_83
  %"%243.load" = load i64, ptr %val2, align 8
  store i64 %"%243.load", ptr %"%usb_addr_11", align 8
  br label %merge_85

then_86:                                          ; preds = %merge_85
  %"%247.load" = load i64, ptr %val2, align 8
  store i64 %"%247.load", ptr %"%usb_addr_12", align 8
  br label %merge_87

then_88:                                          ; preds = %merge_87
  %"%251.load" = load i64, ptr %val2, align 8
  store i64 %"%251.load", ptr %"%usb_addr_13", align 8
  br label %merge_89

then_90:                                          ; preds = %merge_89
  %"%255.load" = load i64, ptr %val2, align 8
  store i64 %"%255.load", ptr %"%usb_addr_14", align 8
  br label %merge_91

then_92:                                          ; preds = %merge_91
  %"%259.load" = load i64, ptr %val2, align 8
  store i64 %"%259.load", ptr %"%usb_addr_15", align 8
  br label %merge_93
}

define i64 @usb_get_state(i64 %idx) {
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
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 8
  br i1 %cmpeqtmp24, label %then_110, label %merge_111

merge_111:                                        ; preds = %merge_109
  %"%idx.load26" = load i64, ptr %idx1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%idx.load26", 9
  br i1 %cmpeqtmp27, label %then_112, label %merge_113

merge_113:                                        ; preds = %merge_111
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 10
  br i1 %cmpeqtmp30, label %then_114, label %merge_115

merge_115:                                        ; preds = %merge_113
  %"%idx.load32" = load i64, ptr %idx1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%idx.load32", 11
  br i1 %cmpeqtmp33, label %then_116, label %merge_117

merge_117:                                        ; preds = %merge_115
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 12
  br i1 %cmpeqtmp36, label %then_118, label %merge_119

merge_119:                                        ; preds = %merge_117
  %"%idx.load38" = load i64, ptr %idx1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%idx.load38", 13
  br i1 %cmpeqtmp39, label %then_120, label %merge_121

merge_121:                                        ; preds = %merge_119
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 14
  br i1 %cmpeqtmp42, label %then_122, label %merge_123

merge_123:                                        ; preds = %merge_121
  %"%idx.load44" = load i64, ptr %idx1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%idx.load44", 15
  br i1 %cmpeqtmp45, label %then_124, label %merge_125

merge_125:                                        ; preds = %merge_123
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
  %loadtmp10 = load i64, ptr @usb_state_3, align 8
  ret i64 %loadtmp10

then_102:                                         ; preds = %merge_101
  %loadtmp13 = load i64, ptr @usb_state_4, align 8
  ret i64 %loadtmp13

then_104:                                         ; preds = %merge_103
  %loadtmp16 = load i64, ptr @usb_state_5, align 8
  ret i64 %loadtmp16

then_106:                                         ; preds = %merge_105
  %loadtmp19 = load i64, ptr @usb_state_6, align 8
  ret i64 %loadtmp19

then_108:                                         ; preds = %merge_107
  %loadtmp22 = load i64, ptr @usb_state_7, align 8
  ret i64 %loadtmp22

then_110:                                         ; preds = %merge_109
  %loadtmp25 = load i64, ptr @usb_state_8, align 8
  ret i64 %loadtmp25

then_112:                                         ; preds = %merge_111
  %loadtmp28 = load i64, ptr @usb_state_9, align 8
  ret i64 %loadtmp28

then_114:                                         ; preds = %merge_113
  %loadtmp31 = load i64, ptr @usb_state_10, align 8
  ret i64 %loadtmp31

then_116:                                         ; preds = %merge_115
  %loadtmp34 = load i64, ptr @usb_state_11, align 8
  ret i64 %loadtmp34

then_118:                                         ; preds = %merge_117
  %loadtmp37 = load i64, ptr @usb_state_12, align 8
  ret i64 %loadtmp37

then_120:                                         ; preds = %merge_119
  %loadtmp40 = load i64, ptr @usb_state_13, align 8
  ret i64 %loadtmp40

then_122:                                         ; preds = %merge_121
  %loadtmp43 = load i64, ptr @usb_state_14, align 8
  ret i64 %loadtmp43

then_124:                                         ; preds = %merge_123
  %loadtmp46 = load i64, ptr @usb_state_15, align 8
  ret i64 %loadtmp46

then_94:                                          ; preds = %entry
  %loadtmp = load i64, ptr @usb_state_0, align 8
  ret i64 %loadtmp

then_96:                                          ; preds = %merge_95
  %loadtmp4 = load i64, ptr @usb_state_1, align 8
  ret i64 %loadtmp4

then_98:                                          ; preds = %merge_97
  %loadtmp7 = load i64, ptr @usb_state_2, align 8
  ret i64 %loadtmp7
}

define i64 @usb_set_state(i64 %idx, i64 %val) {
entry:
  %"%usb_state_15" = alloca i64, align 8
  %"%usb_state_14" = alloca i64, align 8
  %"%usb_state_13" = alloca i64, align 8
  %"%usb_state_12" = alloca i64, align 8
  %"%usb_state_11" = alloca i64, align 8
  %"%usb_state_10" = alloca i64, align 8
  %"%usb_state_9" = alloca i64, align 8
  %"%usb_state_8" = alloca i64, align 8
  %"%usb_state_7" = alloca i64, align 8
  %"%usb_state_6" = alloca i64, align 8
  %"%usb_state_5" = alloca i64, align 8
  %"%usb_state_4" = alloca i64, align 8
  %"%usb_state_3" = alloca i64, align 8
  %"%usb_state_2" = alloca i64, align 8
  %"%usb_state_1" = alloca i64, align 8
  %"%usb_state_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_126, label %merge_127

merge_127:                                        ; preds = %then_126, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_128, label %merge_129

merge_129:                                        ; preds = %then_128, %merge_127
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_130, label %merge_131

merge_131:                                        ; preds = %then_130, %merge_129
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_132, label %merge_133

merge_133:                                        ; preds = %then_132, %merge_131
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_134, label %merge_135

merge_135:                                        ; preds = %then_134, %merge_133
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_136, label %merge_137

merge_137:                                        ; preds = %then_136, %merge_135
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_138, label %merge_139

merge_139:                                        ; preds = %then_138, %merge_137
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_140, label %merge_141

merge_141:                                        ; preds = %then_140, %merge_139
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_142, label %merge_143

merge_143:                                        ; preds = %then_142, %merge_141
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_144, label %merge_145

merge_145:                                        ; preds = %then_144, %merge_143
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_146, label %merge_147

merge_147:                                        ; preds = %then_146, %merge_145
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_148, label %merge_149

merge_149:                                        ; preds = %then_148, %merge_147
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_150, label %merge_151

merge_151:                                        ; preds = %then_150, %merge_149
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_152, label %merge_153

merge_153:                                        ; preds = %then_152, %merge_151
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_154, label %merge_155

merge_155:                                        ; preds = %then_154, %merge_153
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_156, label %merge_157

merge_157:                                        ; preds = %then_156, %merge_155
  ret i64 0

then_126:                                         ; preds = %entry
  %"%345.load" = load i64, ptr %val2, align 8
  store i64 %"%345.load", ptr %"%usb_state_0", align 8
  br label %merge_127

then_128:                                         ; preds = %merge_127
  %"%349.load" = load i64, ptr %val2, align 8
  store i64 %"%349.load", ptr %"%usb_state_1", align 8
  br label %merge_129

then_130:                                         ; preds = %merge_129
  %"%353.load" = load i64, ptr %val2, align 8
  store i64 %"%353.load", ptr %"%usb_state_2", align 8
  br label %merge_131

then_132:                                         ; preds = %merge_131
  %"%357.load" = load i64, ptr %val2, align 8
  store i64 %"%357.load", ptr %"%usb_state_3", align 8
  br label %merge_133

then_134:                                         ; preds = %merge_133
  %"%361.load" = load i64, ptr %val2, align 8
  store i64 %"%361.load", ptr %"%usb_state_4", align 8
  br label %merge_135

then_136:                                         ; preds = %merge_135
  %"%365.load" = load i64, ptr %val2, align 8
  store i64 %"%365.load", ptr %"%usb_state_5", align 8
  br label %merge_137

then_138:                                         ; preds = %merge_137
  %"%369.load" = load i64, ptr %val2, align 8
  store i64 %"%369.load", ptr %"%usb_state_6", align 8
  br label %merge_139

then_140:                                         ; preds = %merge_139
  %"%373.load" = load i64, ptr %val2, align 8
  store i64 %"%373.load", ptr %"%usb_state_7", align 8
  br label %merge_141

then_142:                                         ; preds = %merge_141
  %"%377.load" = load i64, ptr %val2, align 8
  store i64 %"%377.load", ptr %"%usb_state_8", align 8
  br label %merge_143

then_144:                                         ; preds = %merge_143
  %"%381.load" = load i64, ptr %val2, align 8
  store i64 %"%381.load", ptr %"%usb_state_9", align 8
  br label %merge_145

then_146:                                         ; preds = %merge_145
  %"%385.load" = load i64, ptr %val2, align 8
  store i64 %"%385.load", ptr %"%usb_state_10", align 8
  br label %merge_147

then_148:                                         ; preds = %merge_147
  %"%389.load" = load i64, ptr %val2, align 8
  store i64 %"%389.load", ptr %"%usb_state_11", align 8
  br label %merge_149

then_150:                                         ; preds = %merge_149
  %"%393.load" = load i64, ptr %val2, align 8
  store i64 %"%393.load", ptr %"%usb_state_12", align 8
  br label %merge_151

then_152:                                         ; preds = %merge_151
  %"%397.load" = load i64, ptr %val2, align 8
  store i64 %"%397.load", ptr %"%usb_state_13", align 8
  br label %merge_153

then_154:                                         ; preds = %merge_153
  %"%401.load" = load i64, ptr %val2, align 8
  store i64 %"%401.load", ptr %"%usb_state_14", align 8
  br label %merge_155

then_156:                                         ; preds = %merge_155
  %"%405.load" = load i64, ptr %val2, align 8
  store i64 %"%405.load", ptr %"%usb_state_15", align 8
  br label %merge_157
}

define i64 @usb_get_speed(i64 %idx) {
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
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 8
  br i1 %cmpeqtmp24, label %then_174, label %merge_175

merge_175:                                        ; preds = %merge_173
  %"%idx.load26" = load i64, ptr %idx1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%idx.load26", 9
  br i1 %cmpeqtmp27, label %then_176, label %merge_177

merge_177:                                        ; preds = %merge_175
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 10
  br i1 %cmpeqtmp30, label %then_178, label %merge_179

merge_179:                                        ; preds = %merge_177
  %"%idx.load32" = load i64, ptr %idx1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%idx.load32", 11
  br i1 %cmpeqtmp33, label %then_180, label %merge_181

merge_181:                                        ; preds = %merge_179
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 12
  br i1 %cmpeqtmp36, label %then_182, label %merge_183

merge_183:                                        ; preds = %merge_181
  %"%idx.load38" = load i64, ptr %idx1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%idx.load38", 13
  br i1 %cmpeqtmp39, label %then_184, label %merge_185

merge_185:                                        ; preds = %merge_183
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 14
  br i1 %cmpeqtmp42, label %then_186, label %merge_187

merge_187:                                        ; preds = %merge_185
  %"%idx.load44" = load i64, ptr %idx1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%idx.load44", 15
  br i1 %cmpeqtmp45, label %then_188, label %merge_189

merge_189:                                        ; preds = %merge_187
  ret i64 0

then_158:                                         ; preds = %entry
  %loadtmp = load i64, ptr @usb_speed_0, align 8
  ret i64 %loadtmp

then_160:                                         ; preds = %merge_159
  %loadtmp4 = load i64, ptr @usb_speed_1, align 8
  ret i64 %loadtmp4

then_162:                                         ; preds = %merge_161
  %loadtmp7 = load i64, ptr @usb_speed_2, align 8
  ret i64 %loadtmp7

then_164:                                         ; preds = %merge_163
  %loadtmp10 = load i64, ptr @usb_speed_3, align 8
  ret i64 %loadtmp10

then_166:                                         ; preds = %merge_165
  %loadtmp13 = load i64, ptr @usb_speed_4, align 8
  ret i64 %loadtmp13

then_168:                                         ; preds = %merge_167
  %loadtmp16 = load i64, ptr @usb_speed_5, align 8
  ret i64 %loadtmp16

then_170:                                         ; preds = %merge_169
  %loadtmp19 = load i64, ptr @usb_speed_6, align 8
  ret i64 %loadtmp19

then_172:                                         ; preds = %merge_171
  %loadtmp22 = load i64, ptr @usb_speed_7, align 8
  ret i64 %loadtmp22

then_174:                                         ; preds = %merge_173
  %loadtmp25 = load i64, ptr @usb_speed_8, align 8
  ret i64 %loadtmp25

then_176:                                         ; preds = %merge_175
  %loadtmp28 = load i64, ptr @usb_speed_9, align 8
  ret i64 %loadtmp28

then_178:                                         ; preds = %merge_177
  %loadtmp31 = load i64, ptr @usb_speed_10, align 8
  ret i64 %loadtmp31

then_180:                                         ; preds = %merge_179
  %loadtmp34 = load i64, ptr @usb_speed_11, align 8
  ret i64 %loadtmp34

then_182:                                         ; preds = %merge_181
  %loadtmp37 = load i64, ptr @usb_speed_12, align 8
  ret i64 %loadtmp37

then_184:                                         ; preds = %merge_183
  %loadtmp40 = load i64, ptr @usb_speed_13, align 8
  ret i64 %loadtmp40

then_186:                                         ; preds = %merge_185
  %loadtmp43 = load i64, ptr @usb_speed_14, align 8
  ret i64 %loadtmp43

then_188:                                         ; preds = %merge_187
  %loadtmp46 = load i64, ptr @usb_speed_15, align 8
  ret i64 %loadtmp46
}

define i64 @usb_set_speed(i64 %idx, i64 %val) {
entry:
  %"%usb_speed_15" = alloca i64, align 8
  %"%usb_speed_14" = alloca i64, align 8
  %"%usb_speed_13" = alloca i64, align 8
  %"%usb_speed_12" = alloca i64, align 8
  %"%usb_speed_11" = alloca i64, align 8
  %"%usb_speed_10" = alloca i64, align 8
  %"%usb_speed_9" = alloca i64, align 8
  %"%usb_speed_8" = alloca i64, align 8
  %"%usb_speed_7" = alloca i64, align 8
  %"%usb_speed_6" = alloca i64, align 8
  %"%usb_speed_5" = alloca i64, align 8
  %"%usb_speed_4" = alloca i64, align 8
  %"%usb_speed_3" = alloca i64, align 8
  %"%usb_speed_2" = alloca i64, align 8
  %"%usb_speed_1" = alloca i64, align 8
  %"%usb_speed_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_190, label %merge_191

merge_191:                                        ; preds = %then_190, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_192, label %merge_193

merge_193:                                        ; preds = %then_192, %merge_191
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_194, label %merge_195

merge_195:                                        ; preds = %then_194, %merge_193
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_196, label %merge_197

merge_197:                                        ; preds = %then_196, %merge_195
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_198, label %merge_199

merge_199:                                        ; preds = %then_198, %merge_197
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_200, label %merge_201

merge_201:                                        ; preds = %then_200, %merge_199
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_202, label %merge_203

merge_203:                                        ; preds = %then_202, %merge_201
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_204, label %merge_205

merge_205:                                        ; preds = %then_204, %merge_203
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_206, label %merge_207

merge_207:                                        ; preds = %then_206, %merge_205
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_208, label %merge_209

merge_209:                                        ; preds = %then_208, %merge_207
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_210, label %merge_211

merge_211:                                        ; preds = %then_210, %merge_209
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_212, label %merge_213

merge_213:                                        ; preds = %then_212, %merge_211
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_214, label %merge_215

merge_215:                                        ; preds = %then_214, %merge_213
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_216, label %merge_217

merge_217:                                        ; preds = %then_216, %merge_215
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_218, label %merge_219

merge_219:                                        ; preds = %then_218, %merge_217
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_220, label %merge_221

merge_221:                                        ; preds = %then_220, %merge_219
  ret i64 0

then_190:                                         ; preds = %entry
  %"%491.load" = load i64, ptr %val2, align 8
  store i64 %"%491.load", ptr %"%usb_speed_0", align 8
  br label %merge_191

then_192:                                         ; preds = %merge_191
  %"%495.load" = load i64, ptr %val2, align 8
  store i64 %"%495.load", ptr %"%usb_speed_1", align 8
  br label %merge_193

then_194:                                         ; preds = %merge_193
  %"%499.load" = load i64, ptr %val2, align 8
  store i64 %"%499.load", ptr %"%usb_speed_2", align 8
  br label %merge_195

then_196:                                         ; preds = %merge_195
  %"%503.load" = load i64, ptr %val2, align 8
  store i64 %"%503.load", ptr %"%usb_speed_3", align 8
  br label %merge_197

then_198:                                         ; preds = %merge_197
  %"%507.load" = load i64, ptr %val2, align 8
  store i64 %"%507.load", ptr %"%usb_speed_4", align 8
  br label %merge_199

then_200:                                         ; preds = %merge_199
  %"%511.load" = load i64, ptr %val2, align 8
  store i64 %"%511.load", ptr %"%usb_speed_5", align 8
  br label %merge_201

then_202:                                         ; preds = %merge_201
  %"%515.load" = load i64, ptr %val2, align 8
  store i64 %"%515.load", ptr %"%usb_speed_6", align 8
  br label %merge_203

then_204:                                         ; preds = %merge_203
  %"%519.load" = load i64, ptr %val2, align 8
  store i64 %"%519.load", ptr %"%usb_speed_7", align 8
  br label %merge_205

then_206:                                         ; preds = %merge_205
  %"%523.load" = load i64, ptr %val2, align 8
  store i64 %"%523.load", ptr %"%usb_speed_8", align 8
  br label %merge_207

then_208:                                         ; preds = %merge_207
  %"%527.load" = load i64, ptr %val2, align 8
  store i64 %"%527.load", ptr %"%usb_speed_9", align 8
  br label %merge_209

then_210:                                         ; preds = %merge_209
  %"%531.load" = load i64, ptr %val2, align 8
  store i64 %"%531.load", ptr %"%usb_speed_10", align 8
  br label %merge_211

then_212:                                         ; preds = %merge_211
  %"%535.load" = load i64, ptr %val2, align 8
  store i64 %"%535.load", ptr %"%usb_speed_11", align 8
  br label %merge_213

then_214:                                         ; preds = %merge_213
  %"%539.load" = load i64, ptr %val2, align 8
  store i64 %"%539.load", ptr %"%usb_speed_12", align 8
  br label %merge_215

then_216:                                         ; preds = %merge_215
  %"%543.load" = load i64, ptr %val2, align 8
  store i64 %"%543.load", ptr %"%usb_speed_13", align 8
  br label %merge_217

then_218:                                         ; preds = %merge_217
  %"%547.load" = load i64, ptr %val2, align 8
  store i64 %"%547.load", ptr %"%usb_speed_14", align 8
  br label %merge_219

then_220:                                         ; preds = %merge_219
  %"%551.load" = load i64, ptr %val2, align 8
  store i64 %"%551.load", ptr %"%usb_speed_15", align 8
  br label %merge_221
}

define i64 @usb_get_class(i64 %idx) {
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
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_230, label %merge_231

merge_231:                                        ; preds = %merge_229
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_232, label %merge_233

merge_233:                                        ; preds = %merge_231
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_234, label %merge_235

merge_235:                                        ; preds = %merge_233
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_236, label %merge_237

merge_237:                                        ; preds = %merge_235
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 8
  br i1 %cmpeqtmp24, label %then_238, label %merge_239

merge_239:                                        ; preds = %merge_237
  %"%idx.load26" = load i64, ptr %idx1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%idx.load26", 9
  br i1 %cmpeqtmp27, label %then_240, label %merge_241

merge_241:                                        ; preds = %merge_239
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 10
  br i1 %cmpeqtmp30, label %then_242, label %merge_243

merge_243:                                        ; preds = %merge_241
  %"%idx.load32" = load i64, ptr %idx1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%idx.load32", 11
  br i1 %cmpeqtmp33, label %then_244, label %merge_245

merge_245:                                        ; preds = %merge_243
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 12
  br i1 %cmpeqtmp36, label %then_246, label %merge_247

merge_247:                                        ; preds = %merge_245
  %"%idx.load38" = load i64, ptr %idx1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%idx.load38", 13
  br i1 %cmpeqtmp39, label %then_248, label %merge_249

merge_249:                                        ; preds = %merge_247
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 14
  br i1 %cmpeqtmp42, label %then_250, label %merge_251

merge_251:                                        ; preds = %merge_249
  %"%idx.load44" = load i64, ptr %idx1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%idx.load44", 15
  br i1 %cmpeqtmp45, label %then_252, label %merge_253

merge_253:                                        ; preds = %merge_251
  ret i64 0

then_222:                                         ; preds = %entry
  %loadtmp = load i64, ptr @usb_class_0, align 8
  ret i64 %loadtmp

then_224:                                         ; preds = %merge_223
  %loadtmp4 = load i64, ptr @usb_class_1, align 8
  ret i64 %loadtmp4

then_226:                                         ; preds = %merge_225
  %loadtmp7 = load i64, ptr @usb_class_2, align 8
  ret i64 %loadtmp7

then_228:                                         ; preds = %merge_227
  %loadtmp10 = load i64, ptr @usb_class_3, align 8
  ret i64 %loadtmp10

then_230:                                         ; preds = %merge_229
  %loadtmp13 = load i64, ptr @usb_class_4, align 8
  ret i64 %loadtmp13

then_232:                                         ; preds = %merge_231
  %loadtmp16 = load i64, ptr @usb_class_5, align 8
  ret i64 %loadtmp16

then_234:                                         ; preds = %merge_233
  %loadtmp19 = load i64, ptr @usb_class_6, align 8
  ret i64 %loadtmp19

then_236:                                         ; preds = %merge_235
  %loadtmp22 = load i64, ptr @usb_class_7, align 8
  ret i64 %loadtmp22

then_238:                                         ; preds = %merge_237
  %loadtmp25 = load i64, ptr @usb_class_8, align 8
  ret i64 %loadtmp25

then_240:                                         ; preds = %merge_239
  %loadtmp28 = load i64, ptr @usb_class_9, align 8
  ret i64 %loadtmp28

then_242:                                         ; preds = %merge_241
  %loadtmp31 = load i64, ptr @usb_class_10, align 8
  ret i64 %loadtmp31

then_244:                                         ; preds = %merge_243
  %loadtmp34 = load i64, ptr @usb_class_11, align 8
  ret i64 %loadtmp34

then_246:                                         ; preds = %merge_245
  %loadtmp37 = load i64, ptr @usb_class_12, align 8
  ret i64 %loadtmp37

then_248:                                         ; preds = %merge_247
  %loadtmp40 = load i64, ptr @usb_class_13, align 8
  ret i64 %loadtmp40

then_250:                                         ; preds = %merge_249
  %loadtmp43 = load i64, ptr @usb_class_14, align 8
  ret i64 %loadtmp43

then_252:                                         ; preds = %merge_251
  %loadtmp46 = load i64, ptr @usb_class_15, align 8
  ret i64 %loadtmp46
}

define i64 @usb_set_class(i64 %idx, i64 %val) {
entry:
  %"%usb_class_15" = alloca i64, align 8
  %"%usb_class_14" = alloca i64, align 8
  %"%usb_class_13" = alloca i64, align 8
  %"%usb_class_12" = alloca i64, align 8
  %"%usb_class_11" = alloca i64, align 8
  %"%usb_class_10" = alloca i64, align 8
  %"%usb_class_9" = alloca i64, align 8
  %"%usb_class_8" = alloca i64, align 8
  %"%usb_class_7" = alloca i64, align 8
  %"%usb_class_6" = alloca i64, align 8
  %"%usb_class_5" = alloca i64, align 8
  %"%usb_class_4" = alloca i64, align 8
  %"%usb_class_3" = alloca i64, align 8
  %"%usb_class_2" = alloca i64, align 8
  %"%usb_class_1" = alloca i64, align 8
  %"%usb_class_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_254, label %merge_255

merge_255:                                        ; preds = %then_254, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_256, label %merge_257

merge_257:                                        ; preds = %then_256, %merge_255
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_258, label %merge_259

merge_259:                                        ; preds = %then_258, %merge_257
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_260, label %merge_261

merge_261:                                        ; preds = %then_260, %merge_259
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_262, label %merge_263

merge_263:                                        ; preds = %then_262, %merge_261
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_264, label %merge_265

merge_265:                                        ; preds = %then_264, %merge_263
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_266, label %merge_267

merge_267:                                        ; preds = %then_266, %merge_265
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_268, label %merge_269

merge_269:                                        ; preds = %then_268, %merge_267
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_270, label %merge_271

merge_271:                                        ; preds = %then_270, %merge_269
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_272, label %merge_273

merge_273:                                        ; preds = %then_272, %merge_271
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_274, label %merge_275

merge_275:                                        ; preds = %then_274, %merge_273
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_276, label %merge_277

merge_277:                                        ; preds = %then_276, %merge_275
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_278, label %merge_279

merge_279:                                        ; preds = %then_278, %merge_277
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_280, label %merge_281

merge_281:                                        ; preds = %then_280, %merge_279
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_282, label %merge_283

merge_283:                                        ; preds = %then_282, %merge_281
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_284, label %merge_285

merge_285:                                        ; preds = %then_284, %merge_283
  ret i64 0

then_254:                                         ; preds = %entry
  %"%637.load" = load i64, ptr %val2, align 8
  store i64 %"%637.load", ptr %"%usb_class_0", align 8
  br label %merge_255

then_256:                                         ; preds = %merge_255
  %"%641.load" = load i64, ptr %val2, align 8
  store i64 %"%641.load", ptr %"%usb_class_1", align 8
  br label %merge_257

then_258:                                         ; preds = %merge_257
  %"%645.load" = load i64, ptr %val2, align 8
  store i64 %"%645.load", ptr %"%usb_class_2", align 8
  br label %merge_259

then_260:                                         ; preds = %merge_259
  %"%649.load" = load i64, ptr %val2, align 8
  store i64 %"%649.load", ptr %"%usb_class_3", align 8
  br label %merge_261

then_262:                                         ; preds = %merge_261
  %"%653.load" = load i64, ptr %val2, align 8
  store i64 %"%653.load", ptr %"%usb_class_4", align 8
  br label %merge_263

then_264:                                         ; preds = %merge_263
  %"%657.load" = load i64, ptr %val2, align 8
  store i64 %"%657.load", ptr %"%usb_class_5", align 8
  br label %merge_265

then_266:                                         ; preds = %merge_265
  %"%661.load" = load i64, ptr %val2, align 8
  store i64 %"%661.load", ptr %"%usb_class_6", align 8
  br label %merge_267

then_268:                                         ; preds = %merge_267
  %"%665.load" = load i64, ptr %val2, align 8
  store i64 %"%665.load", ptr %"%usb_class_7", align 8
  br label %merge_269

then_270:                                         ; preds = %merge_269
  %"%669.load" = load i64, ptr %val2, align 8
  store i64 %"%669.load", ptr %"%usb_class_8", align 8
  br label %merge_271

then_272:                                         ; preds = %merge_271
  %"%673.load" = load i64, ptr %val2, align 8
  store i64 %"%673.load", ptr %"%usb_class_9", align 8
  br label %merge_273

then_274:                                         ; preds = %merge_273
  %"%677.load" = load i64, ptr %val2, align 8
  store i64 %"%677.load", ptr %"%usb_class_10", align 8
  br label %merge_275

then_276:                                         ; preds = %merge_275
  %"%681.load" = load i64, ptr %val2, align 8
  store i64 %"%681.load", ptr %"%usb_class_11", align 8
  br label %merge_277

then_278:                                         ; preds = %merge_277
  %"%685.load" = load i64, ptr %val2, align 8
  store i64 %"%685.load", ptr %"%usb_class_12", align 8
  br label %merge_279

then_280:                                         ; preds = %merge_279
  %"%689.load" = load i64, ptr %val2, align 8
  store i64 %"%689.load", ptr %"%usb_class_13", align 8
  br label %merge_281

then_282:                                         ; preds = %merge_281
  %"%693.load" = load i64, ptr %val2, align 8
  store i64 %"%693.load", ptr %"%usb_class_14", align 8
  br label %merge_283

then_284:                                         ; preds = %merge_283
  %"%697.load" = load i64, ptr %val2, align 8
  store i64 %"%697.load", ptr %"%usb_class_15", align 8
  br label %merge_285
}

define i64 @usb_get_proto(i64 %idx) {
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
  ret i64 0

then_286:                                         ; preds = %entry
  %loadtmp = load i64, ptr @usb_proto_0, align 8
  ret i64 %loadtmp

then_288:                                         ; preds = %merge_287
  %loadtmp4 = load i64, ptr @usb_proto_1, align 8
  ret i64 %loadtmp4

then_290:                                         ; preds = %merge_289
  %loadtmp7 = load i64, ptr @usb_proto_2, align 8
  ret i64 %loadtmp7

then_292:                                         ; preds = %merge_291
  %loadtmp10 = load i64, ptr @usb_proto_3, align 8
  ret i64 %loadtmp10

then_294:                                         ; preds = %merge_293
  %loadtmp13 = load i64, ptr @usb_proto_4, align 8
  ret i64 %loadtmp13

then_296:                                         ; preds = %merge_295
  %loadtmp16 = load i64, ptr @usb_proto_5, align 8
  ret i64 %loadtmp16

then_298:                                         ; preds = %merge_297
  %loadtmp19 = load i64, ptr @usb_proto_6, align 8
  ret i64 %loadtmp19

then_300:                                         ; preds = %merge_299
  %loadtmp22 = load i64, ptr @usb_proto_7, align 8
  ret i64 %loadtmp22

then_302:                                         ; preds = %merge_301
  %loadtmp25 = load i64, ptr @usb_proto_8, align 8
  ret i64 %loadtmp25

then_304:                                         ; preds = %merge_303
  %loadtmp28 = load i64, ptr @usb_proto_9, align 8
  ret i64 %loadtmp28

then_306:                                         ; preds = %merge_305
  %loadtmp31 = load i64, ptr @usb_proto_10, align 8
  ret i64 %loadtmp31

then_308:                                         ; preds = %merge_307
  %loadtmp34 = load i64, ptr @usb_proto_11, align 8
  ret i64 %loadtmp34

then_310:                                         ; preds = %merge_309
  %loadtmp37 = load i64, ptr @usb_proto_12, align 8
  ret i64 %loadtmp37

then_312:                                         ; preds = %merge_311
  %loadtmp40 = load i64, ptr @usb_proto_13, align 8
  ret i64 %loadtmp40

then_314:                                         ; preds = %merge_313
  %loadtmp43 = load i64, ptr @usb_proto_14, align 8
  ret i64 %loadtmp43

then_316:                                         ; preds = %merge_315
  %loadtmp46 = load i64, ptr @usb_proto_15, align 8
  ret i64 %loadtmp46
}

define i64 @usb_set_proto(i64 %idx, i64 %val) {
entry:
  %"%usb_proto_15" = alloca i64, align 8
  %"%usb_proto_14" = alloca i64, align 8
  %"%usb_proto_13" = alloca i64, align 8
  %"%usb_proto_12" = alloca i64, align 8
  %"%usb_proto_11" = alloca i64, align 8
  %"%usb_proto_10" = alloca i64, align 8
  %"%usb_proto_9" = alloca i64, align 8
  %"%usb_proto_8" = alloca i64, align 8
  %"%usb_proto_7" = alloca i64, align 8
  %"%usb_proto_6" = alloca i64, align 8
  %"%usb_proto_5" = alloca i64, align 8
  %"%usb_proto_4" = alloca i64, align 8
  %"%usb_proto_3" = alloca i64, align 8
  %"%usb_proto_2" = alloca i64, align 8
  %"%usb_proto_1" = alloca i64, align 8
  %"%usb_proto_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_318, label %merge_319

merge_319:                                        ; preds = %then_318, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_320, label %merge_321

merge_321:                                        ; preds = %then_320, %merge_319
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_322, label %merge_323

merge_323:                                        ; preds = %then_322, %merge_321
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_324, label %merge_325

merge_325:                                        ; preds = %then_324, %merge_323
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_326, label %merge_327

merge_327:                                        ; preds = %then_326, %merge_325
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_328, label %merge_329

merge_329:                                        ; preds = %then_328, %merge_327
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_330, label %merge_331

merge_331:                                        ; preds = %then_330, %merge_329
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_332, label %merge_333

merge_333:                                        ; preds = %then_332, %merge_331
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_334, label %merge_335

merge_335:                                        ; preds = %then_334, %merge_333
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_336, label %merge_337

merge_337:                                        ; preds = %then_336, %merge_335
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_338, label %merge_339

merge_339:                                        ; preds = %then_338, %merge_337
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_340, label %merge_341

merge_341:                                        ; preds = %then_340, %merge_339
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_342, label %merge_343

merge_343:                                        ; preds = %then_342, %merge_341
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_344, label %merge_345

merge_345:                                        ; preds = %then_344, %merge_343
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_346, label %merge_347

merge_347:                                        ; preds = %then_346, %merge_345
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_348, label %merge_349

merge_349:                                        ; preds = %then_348, %merge_347
  ret i64 0

then_318:                                         ; preds = %entry
  %"%783.load" = load i64, ptr %val2, align 8
  store i64 %"%783.load", ptr %"%usb_proto_0", align 8
  br label %merge_319

then_320:                                         ; preds = %merge_319
  %"%787.load" = load i64, ptr %val2, align 8
  store i64 %"%787.load", ptr %"%usb_proto_1", align 8
  br label %merge_321

then_322:                                         ; preds = %merge_321
  %"%791.load" = load i64, ptr %val2, align 8
  store i64 %"%791.load", ptr %"%usb_proto_2", align 8
  br label %merge_323

then_324:                                         ; preds = %merge_323
  %"%795.load" = load i64, ptr %val2, align 8
  store i64 %"%795.load", ptr %"%usb_proto_3", align 8
  br label %merge_325

then_326:                                         ; preds = %merge_325
  %"%799.load" = load i64, ptr %val2, align 8
  store i64 %"%799.load", ptr %"%usb_proto_4", align 8
  br label %merge_327

then_328:                                         ; preds = %merge_327
  %"%803.load" = load i64, ptr %val2, align 8
  store i64 %"%803.load", ptr %"%usb_proto_5", align 8
  br label %merge_329

then_330:                                         ; preds = %merge_329
  %"%807.load" = load i64, ptr %val2, align 8
  store i64 %"%807.load", ptr %"%usb_proto_6", align 8
  br label %merge_331

then_332:                                         ; preds = %merge_331
  %"%811.load" = load i64, ptr %val2, align 8
  store i64 %"%811.load", ptr %"%usb_proto_7", align 8
  br label %merge_333

then_334:                                         ; preds = %merge_333
  %"%815.load" = load i64, ptr %val2, align 8
  store i64 %"%815.load", ptr %"%usb_proto_8", align 8
  br label %merge_335

then_336:                                         ; preds = %merge_335
  %"%819.load" = load i64, ptr %val2, align 8
  store i64 %"%819.load", ptr %"%usb_proto_9", align 8
  br label %merge_337

then_338:                                         ; preds = %merge_337
  %"%823.load" = load i64, ptr %val2, align 8
  store i64 %"%823.load", ptr %"%usb_proto_10", align 8
  br label %merge_339

then_340:                                         ; preds = %merge_339
  %"%827.load" = load i64, ptr %val2, align 8
  store i64 %"%827.load", ptr %"%usb_proto_11", align 8
  br label %merge_341

then_342:                                         ; preds = %merge_341
  %"%831.load" = load i64, ptr %val2, align 8
  store i64 %"%831.load", ptr %"%usb_proto_12", align 8
  br label %merge_343

then_344:                                         ; preds = %merge_343
  %"%835.load" = load i64, ptr %val2, align 8
  store i64 %"%835.load", ptr %"%usb_proto_13", align 8
  br label %merge_345

then_346:                                         ; preds = %merge_345
  %"%839.load" = load i64, ptr %val2, align 8
  store i64 %"%839.load", ptr %"%usb_proto_14", align 8
  br label %merge_347

then_348:                                         ; preds = %merge_347
  %"%843.load" = load i64, ptr %val2, align 8
  store i64 %"%843.load", ptr %"%usb_proto_15", align 8
  br label %merge_349
}

define i64 @usb_get_vid(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_350, label %merge_351

merge_351:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_352, label %merge_353

merge_353:                                        ; preds = %merge_351
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_354, label %merge_355

merge_355:                                        ; preds = %merge_353
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_356, label %merge_357

merge_357:                                        ; preds = %merge_355
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 4
  br i1 %cmpeqtmp12, label %then_358, label %merge_359

merge_359:                                        ; preds = %merge_357
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_360, label %merge_361

merge_361:                                        ; preds = %merge_359
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 6
  br i1 %cmpeqtmp18, label %then_362, label %merge_363

merge_363:                                        ; preds = %merge_361
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 7
  br i1 %cmpeqtmp21, label %then_364, label %merge_365

merge_365:                                        ; preds = %merge_363
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 8
  br i1 %cmpeqtmp24, label %then_366, label %merge_367

merge_367:                                        ; preds = %merge_365
  %"%idx.load26" = load i64, ptr %idx1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%idx.load26", 9
  br i1 %cmpeqtmp27, label %then_368, label %merge_369

merge_369:                                        ; preds = %merge_367
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 10
  br i1 %cmpeqtmp30, label %then_370, label %merge_371

merge_371:                                        ; preds = %merge_369
  %"%idx.load32" = load i64, ptr %idx1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%idx.load32", 11
  br i1 %cmpeqtmp33, label %then_372, label %merge_373

merge_373:                                        ; preds = %merge_371
  %"%idx.load35" = load i64, ptr %idx1, align 8
  %cmpeqtmp36 = icmp eq i64 %"%idx.load35", 12
  br i1 %cmpeqtmp36, label %then_374, label %merge_375

merge_375:                                        ; preds = %merge_373
  %"%idx.load38" = load i64, ptr %idx1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%idx.load38", 13
  br i1 %cmpeqtmp39, label %then_376, label %merge_377

merge_377:                                        ; preds = %merge_375
  %"%idx.load41" = load i64, ptr %idx1, align 8
  %cmpeqtmp42 = icmp eq i64 %"%idx.load41", 14
  br i1 %cmpeqtmp42, label %then_378, label %merge_379

merge_379:                                        ; preds = %merge_377
  %"%idx.load44" = load i64, ptr %idx1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%idx.load44", 15
  br i1 %cmpeqtmp45, label %then_380, label %merge_381

merge_381:                                        ; preds = %merge_379
  ret i64 0

then_350:                                         ; preds = %entry
  %loadtmp = load i64, ptr @usb_vid_0, align 8
  ret i64 %loadtmp

then_352:                                         ; preds = %merge_351
  %loadtmp4 = load i64, ptr @usb_vid_1, align 8
  ret i64 %loadtmp4

then_354:                                         ; preds = %merge_353
  %loadtmp7 = load i64, ptr @usb_vid_2, align 8
  ret i64 %loadtmp7

then_356:                                         ; preds = %merge_355
  %loadtmp10 = load i64, ptr @usb_vid_3, align 8
  ret i64 %loadtmp10

then_358:                                         ; preds = %merge_357
  %loadtmp13 = load i64, ptr @usb_vid_4, align 8
  ret i64 %loadtmp13

then_360:                                         ; preds = %merge_359
  %loadtmp16 = load i64, ptr @usb_vid_5, align 8
  ret i64 %loadtmp16

then_362:                                         ; preds = %merge_361
  %loadtmp19 = load i64, ptr @usb_vid_6, align 8
  ret i64 %loadtmp19

then_364:                                         ; preds = %merge_363
  %loadtmp22 = load i64, ptr @usb_vid_7, align 8
  ret i64 %loadtmp22

then_366:                                         ; preds = %merge_365
  %loadtmp25 = load i64, ptr @usb_vid_8, align 8
  ret i64 %loadtmp25

then_368:                                         ; preds = %merge_367
  %loadtmp28 = load i64, ptr @usb_vid_9, align 8
  ret i64 %loadtmp28

then_370:                                         ; preds = %merge_369
  %loadtmp31 = load i64, ptr @usb_vid_10, align 8
  ret i64 %loadtmp31

then_372:                                         ; preds = %merge_371
  %loadtmp34 = load i64, ptr @usb_vid_11, align 8
  ret i64 %loadtmp34

then_374:                                         ; preds = %merge_373
  %loadtmp37 = load i64, ptr @usb_vid_12, align 8
  ret i64 %loadtmp37

then_376:                                         ; preds = %merge_375
  %loadtmp40 = load i64, ptr @usb_vid_13, align 8
  ret i64 %loadtmp40

then_378:                                         ; preds = %merge_377
  %loadtmp43 = load i64, ptr @usb_vid_14, align 8
  ret i64 %loadtmp43

then_380:                                         ; preds = %merge_379
  %loadtmp46 = load i64, ptr @usb_vid_15, align 8
  ret i64 %loadtmp46
}

define i64 @usb_set_vid(i64 %idx, i64 %val) {
entry:
  %"%usb_vid_15" = alloca i64, align 8
  %"%usb_vid_14" = alloca i64, align 8
  %"%usb_vid_13" = alloca i64, align 8
  %"%usb_vid_12" = alloca i64, align 8
  %"%usb_vid_11" = alloca i64, align 8
  %"%usb_vid_10" = alloca i64, align 8
  %"%usb_vid_9" = alloca i64, align 8
  %"%usb_vid_8" = alloca i64, align 8
  %"%usb_vid_7" = alloca i64, align 8
  %"%usb_vid_6" = alloca i64, align 8
  %"%usb_vid_5" = alloca i64, align 8
  %"%usb_vid_4" = alloca i64, align 8
  %"%usb_vid_3" = alloca i64, align 8
  %"%usb_vid_2" = alloca i64, align 8
  %"%usb_vid_1" = alloca i64, align 8
  %"%usb_vid_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_382, label %merge_383

merge_383:                                        ; preds = %then_382, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_384, label %merge_385

merge_385:                                        ; preds = %then_384, %merge_383
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_386, label %merge_387

merge_387:                                        ; preds = %then_386, %merge_385
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_388, label %merge_389

merge_389:                                        ; preds = %then_388, %merge_387
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_390, label %merge_391

merge_391:                                        ; preds = %then_390, %merge_389
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_392, label %merge_393

merge_393:                                        ; preds = %then_392, %merge_391
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_394, label %merge_395

merge_395:                                        ; preds = %then_394, %merge_393
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_396, label %merge_397

merge_397:                                        ; preds = %then_396, %merge_395
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_398, label %merge_399

merge_399:                                        ; preds = %then_398, %merge_397
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_400, label %merge_401

merge_401:                                        ; preds = %then_400, %merge_399
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_402, label %merge_403

merge_403:                                        ; preds = %then_402, %merge_401
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_404, label %merge_405

merge_405:                                        ; preds = %then_404, %merge_403
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_406, label %merge_407

merge_407:                                        ; preds = %then_406, %merge_405
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_408, label %merge_409

merge_409:                                        ; preds = %then_408, %merge_407
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_410, label %merge_411

merge_411:                                        ; preds = %then_410, %merge_409
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_412, label %merge_413

merge_413:                                        ; preds = %then_412, %merge_411
  ret i64 0

then_382:                                         ; preds = %entry
  %"%929.load" = load i64, ptr %val2, align 8
  store i64 %"%929.load", ptr %"%usb_vid_0", align 8
  br label %merge_383

then_384:                                         ; preds = %merge_383
  %"%933.load" = load i64, ptr %val2, align 8
  store i64 %"%933.load", ptr %"%usb_vid_1", align 8
  br label %merge_385

then_386:                                         ; preds = %merge_385
  %"%937.load" = load i64, ptr %val2, align 8
  store i64 %"%937.load", ptr %"%usb_vid_2", align 8
  br label %merge_387

then_388:                                         ; preds = %merge_387
  %"%941.load" = load i64, ptr %val2, align 8
  store i64 %"%941.load", ptr %"%usb_vid_3", align 8
  br label %merge_389

then_390:                                         ; preds = %merge_389
  %"%945.load" = load i64, ptr %val2, align 8
  store i64 %"%945.load", ptr %"%usb_vid_4", align 8
  br label %merge_391

then_392:                                         ; preds = %merge_391
  %"%949.load" = load i64, ptr %val2, align 8
  store i64 %"%949.load", ptr %"%usb_vid_5", align 8
  br label %merge_393

then_394:                                         ; preds = %merge_393
  %"%953.load" = load i64, ptr %val2, align 8
  store i64 %"%953.load", ptr %"%usb_vid_6", align 8
  br label %merge_395

then_396:                                         ; preds = %merge_395
  %"%957.load" = load i64, ptr %val2, align 8
  store i64 %"%957.load", ptr %"%usb_vid_7", align 8
  br label %merge_397

then_398:                                         ; preds = %merge_397
  %"%961.load" = load i64, ptr %val2, align 8
  store i64 %"%961.load", ptr %"%usb_vid_8", align 8
  br label %merge_399

then_400:                                         ; preds = %merge_399
  %"%965.load" = load i64, ptr %val2, align 8
  store i64 %"%965.load", ptr %"%usb_vid_9", align 8
  br label %merge_401

then_402:                                         ; preds = %merge_401
  %"%969.load" = load i64, ptr %val2, align 8
  store i64 %"%969.load", ptr %"%usb_vid_10", align 8
  br label %merge_403

then_404:                                         ; preds = %merge_403
  %"%973.load" = load i64, ptr %val2, align 8
  store i64 %"%973.load", ptr %"%usb_vid_11", align 8
  br label %merge_405

then_406:                                         ; preds = %merge_405
  %"%977.load" = load i64, ptr %val2, align 8
  store i64 %"%977.load", ptr %"%usb_vid_12", align 8
  br label %merge_407

then_408:                                         ; preds = %merge_407
  %"%981.load" = load i64, ptr %val2, align 8
  store i64 %"%981.load", ptr %"%usb_vid_13", align 8
  br label %merge_409

then_410:                                         ; preds = %merge_409
  %"%985.load" = load i64, ptr %val2, align 8
  store i64 %"%985.load", ptr %"%usb_vid_14", align 8
  br label %merge_411

then_412:                                         ; preds = %merge_411
  %"%989.load" = load i64, ptr %val2, align 8
  store i64 %"%989.load", ptr %"%usb_vid_15", align 8
  br label %merge_413
}

define i64 @usb_get_maxpkt(i64 %idx) {
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
  ret i64 8

then_414:                                         ; preds = %entry
  %loadtmp = load i64, ptr @usb_maxpkt_0, align 8
  ret i64 %loadtmp

then_416:                                         ; preds = %merge_415
  %loadtmp4 = load i64, ptr @usb_maxpkt_1, align 8
  ret i64 %loadtmp4

then_418:                                         ; preds = %merge_417
  %loadtmp7 = load i64, ptr @usb_maxpkt_2, align 8
  ret i64 %loadtmp7

then_420:                                         ; preds = %merge_419
  %loadtmp10 = load i64, ptr @usb_maxpkt_3, align 8
  ret i64 %loadtmp10

then_422:                                         ; preds = %merge_421
  %loadtmp13 = load i64, ptr @usb_maxpkt_4, align 8
  ret i64 %loadtmp13

then_424:                                         ; preds = %merge_423
  %loadtmp16 = load i64, ptr @usb_maxpkt_5, align 8
  ret i64 %loadtmp16

then_426:                                         ; preds = %merge_425
  %loadtmp19 = load i64, ptr @usb_maxpkt_6, align 8
  ret i64 %loadtmp19

then_428:                                         ; preds = %merge_427
  %loadtmp22 = load i64, ptr @usb_maxpkt_7, align 8
  ret i64 %loadtmp22

then_430:                                         ; preds = %merge_429
  %loadtmp25 = load i64, ptr @usb_maxpkt_8, align 8
  ret i64 %loadtmp25

then_432:                                         ; preds = %merge_431
  %loadtmp28 = load i64, ptr @usb_maxpkt_9, align 8
  ret i64 %loadtmp28

then_434:                                         ; preds = %merge_433
  %loadtmp31 = load i64, ptr @usb_maxpkt_10, align 8
  ret i64 %loadtmp31

then_436:                                         ; preds = %merge_435
  %loadtmp34 = load i64, ptr @usb_maxpkt_11, align 8
  ret i64 %loadtmp34

then_438:                                         ; preds = %merge_437
  %loadtmp37 = load i64, ptr @usb_maxpkt_12, align 8
  ret i64 %loadtmp37

then_440:                                         ; preds = %merge_439
  %loadtmp40 = load i64, ptr @usb_maxpkt_13, align 8
  ret i64 %loadtmp40

then_442:                                         ; preds = %merge_441
  %loadtmp43 = load i64, ptr @usb_maxpkt_14, align 8
  ret i64 %loadtmp43

then_444:                                         ; preds = %merge_443
  %loadtmp46 = load i64, ptr @usb_maxpkt_15, align 8
  ret i64 %loadtmp46
}

define i64 @usb_set_maxpkt(i64 %idx, i64 %val) {
entry:
  %"%usb_maxpkt_15" = alloca i64, align 8
  %"%usb_maxpkt_14" = alloca i64, align 8
  %"%usb_maxpkt_13" = alloca i64, align 8
  %"%usb_maxpkt_12" = alloca i64, align 8
  %"%usb_maxpkt_11" = alloca i64, align 8
  %"%usb_maxpkt_10" = alloca i64, align 8
  %"%usb_maxpkt_9" = alloca i64, align 8
  %"%usb_maxpkt_8" = alloca i64, align 8
  %"%usb_maxpkt_7" = alloca i64, align 8
  %"%usb_maxpkt_6" = alloca i64, align 8
  %"%usb_maxpkt_5" = alloca i64, align 8
  %"%usb_maxpkt_4" = alloca i64, align 8
  %"%usb_maxpkt_3" = alloca i64, align 8
  %"%usb_maxpkt_2" = alloca i64, align 8
  %"%usb_maxpkt_1" = alloca i64, align 8
  %"%usb_maxpkt_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_446, label %merge_447

merge_447:                                        ; preds = %then_446, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_448, label %merge_449

merge_449:                                        ; preds = %then_448, %merge_447
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_450, label %merge_451

merge_451:                                        ; preds = %then_450, %merge_449
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_452, label %merge_453

merge_453:                                        ; preds = %then_452, %merge_451
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_454, label %merge_455

merge_455:                                        ; preds = %then_454, %merge_453
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_456, label %merge_457

merge_457:                                        ; preds = %then_456, %merge_455
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_458, label %merge_459

merge_459:                                        ; preds = %then_458, %merge_457
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_460, label %merge_461

merge_461:                                        ; preds = %then_460, %merge_459
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_462, label %merge_463

merge_463:                                        ; preds = %then_462, %merge_461
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_464, label %merge_465

merge_465:                                        ; preds = %then_464, %merge_463
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_466, label %merge_467

merge_467:                                        ; preds = %then_466, %merge_465
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_468, label %merge_469

merge_469:                                        ; preds = %then_468, %merge_467
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_470, label %merge_471

merge_471:                                        ; preds = %then_470, %merge_469
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_472, label %merge_473

merge_473:                                        ; preds = %then_472, %merge_471
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_474, label %merge_475

merge_475:                                        ; preds = %then_474, %merge_473
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_476, label %merge_477

merge_477:                                        ; preds = %then_476, %merge_475
  ret i64 0

then_446:                                         ; preds = %entry
  %"%1075.load" = load i64, ptr %val2, align 8
  store i64 %"%1075.load", ptr %"%usb_maxpkt_0", align 8
  br label %merge_447

then_448:                                         ; preds = %merge_447
  %"%1079.load" = load i64, ptr %val2, align 8
  store i64 %"%1079.load", ptr %"%usb_maxpkt_1", align 8
  br label %merge_449

then_450:                                         ; preds = %merge_449
  %"%1083.load" = load i64, ptr %val2, align 8
  store i64 %"%1083.load", ptr %"%usb_maxpkt_2", align 8
  br label %merge_451

then_452:                                         ; preds = %merge_451
  %"%1087.load" = load i64, ptr %val2, align 8
  store i64 %"%1087.load", ptr %"%usb_maxpkt_3", align 8
  br label %merge_453

then_454:                                         ; preds = %merge_453
  %"%1091.load" = load i64, ptr %val2, align 8
  store i64 %"%1091.load", ptr %"%usb_maxpkt_4", align 8
  br label %merge_455

then_456:                                         ; preds = %merge_455
  %"%1095.load" = load i64, ptr %val2, align 8
  store i64 %"%1095.load", ptr %"%usb_maxpkt_5", align 8
  br label %merge_457

then_458:                                         ; preds = %merge_457
  %"%1099.load" = load i64, ptr %val2, align 8
  store i64 %"%1099.load", ptr %"%usb_maxpkt_6", align 8
  br label %merge_459

then_460:                                         ; preds = %merge_459
  %"%1103.load" = load i64, ptr %val2, align 8
  store i64 %"%1103.load", ptr %"%usb_maxpkt_7", align 8
  br label %merge_461

then_462:                                         ; preds = %merge_461
  %"%1107.load" = load i64, ptr %val2, align 8
  store i64 %"%1107.load", ptr %"%usb_maxpkt_8", align 8
  br label %merge_463

then_464:                                         ; preds = %merge_463
  %"%1111.load" = load i64, ptr %val2, align 8
  store i64 %"%1111.load", ptr %"%usb_maxpkt_9", align 8
  br label %merge_465

then_466:                                         ; preds = %merge_465
  %"%1115.load" = load i64, ptr %val2, align 8
  store i64 %"%1115.load", ptr %"%usb_maxpkt_10", align 8
  br label %merge_467

then_468:                                         ; preds = %merge_467
  %"%1119.load" = load i64, ptr %val2, align 8
  store i64 %"%1119.load", ptr %"%usb_maxpkt_11", align 8
  br label %merge_469

then_470:                                         ; preds = %merge_469
  %"%1123.load" = load i64, ptr %val2, align 8
  store i64 %"%1123.load", ptr %"%usb_maxpkt_12", align 8
  br label %merge_471

then_472:                                         ; preds = %merge_471
  %"%1127.load" = load i64, ptr %val2, align 8
  store i64 %"%1127.load", ptr %"%usb_maxpkt_13", align 8
  br label %merge_473

then_474:                                         ; preds = %merge_473
  %"%1131.load" = load i64, ptr %val2, align 8
  store i64 %"%1131.load", ptr %"%usb_maxpkt_14", align 8
  br label %merge_475

then_476:                                         ; preds = %merge_475
  %"%1135.load" = load i64, ptr %val2, align 8
  store i64 %"%1135.load", ptr %"%usb_maxpkt_15", align 8
  br label %merge_477
}

define i64 @usb_get_hc_type(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_478, label %merge_479

merge_479:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_480, label %merge_481

merge_481:                                        ; preds = %merge_479
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_482, label %merge_483

merge_483:                                        ; preds = %merge_481
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_484, label %merge_485

merge_485:                                        ; preds = %merge_483
  ret i64 0

then_478:                                         ; preds = %entry
  %loadtmp = load i64, ptr @hc_type_0, align 8
  ret i64 %loadtmp

then_480:                                         ; preds = %merge_479
  %loadtmp4 = load i64, ptr @hc_type_1, align 8
  ret i64 %loadtmp4

then_482:                                         ; preds = %merge_481
  %loadtmp7 = load i64, ptr @hc_type_2, align 8
  ret i64 %loadtmp7

then_484:                                         ; preds = %merge_483
  %loadtmp10 = load i64, ptr @hc_type_3, align 8
  ret i64 %loadtmp10
}

define i64 @usb_set_hc_type(i64 %idx, i64 %val) {
entry:
  %"%hc_type_3" = alloca i64, align 8
  %"%hc_type_2" = alloca i64, align 8
  %"%hc_type_1" = alloca i64, align 8
  %"%hc_type_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_486, label %merge_487

merge_487:                                        ; preds = %then_486, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_488, label %merge_489

merge_489:                                        ; preds = %then_488, %merge_487
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_490, label %merge_491

merge_491:                                        ; preds = %then_490, %merge_489
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_492, label %merge_493

merge_493:                                        ; preds = %then_492, %merge_491
  ret i64 0

then_486:                                         ; preds = %entry
  %"%1161.load" = load i64, ptr %val2, align 8
  store i64 %"%1161.load", ptr %"%hc_type_0", align 8
  br label %merge_487

then_488:                                         ; preds = %merge_487
  %"%1165.load" = load i64, ptr %val2, align 8
  store i64 %"%1165.load", ptr %"%hc_type_1", align 8
  br label %merge_489

then_490:                                         ; preds = %merge_489
  %"%1169.load" = load i64, ptr %val2, align 8
  store i64 %"%1169.load", ptr %"%hc_type_2", align 8
  br label %merge_491

then_492:                                         ; preds = %merge_491
  %"%1173.load" = load i64, ptr %val2, align 8
  store i64 %"%1173.load", ptr %"%hc_type_3", align 8
  br label %merge_493
}

define i64 @usb_get_hc_base(i64 %idx) {
entry:
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_494, label %merge_495

merge_495:                                        ; preds = %entry
  %"%idx.load2" = load i64, ptr %idx1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%idx.load2", 1
  br i1 %cmpeqtmp3, label %then_496, label %merge_497

merge_497:                                        ; preds = %merge_495
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_498, label %merge_499

merge_499:                                        ; preds = %merge_497
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_500, label %merge_501

merge_501:                                        ; preds = %merge_499
  ret i64 0

then_494:                                         ; preds = %entry
  %loadtmp = load i64, ptr @hc_base_0, align 8
  ret i64 %loadtmp

then_496:                                         ; preds = %merge_495
  %loadtmp4 = load i64, ptr @hc_base_1, align 8
  ret i64 %loadtmp4

then_498:                                         ; preds = %merge_497
  %loadtmp7 = load i64, ptr @hc_base_2, align 8
  ret i64 %loadtmp7

then_500:                                         ; preds = %merge_499
  %loadtmp10 = load i64, ptr @hc_base_3, align 8
  ret i64 %loadtmp10
}

define i64 @usb_set_hc_base(i64 %idx, i64 %val) {
entry:
  %"%hc_base_3" = alloca i64, align 8
  %"%hc_base_2" = alloca i64, align 8
  %"%hc_base_1" = alloca i64, align 8
  %"%hc_base_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_502, label %merge_503

merge_503:                                        ; preds = %then_502, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_504, label %merge_505

merge_505:                                        ; preds = %then_504, %merge_503
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_506, label %merge_507

merge_507:                                        ; preds = %then_506, %merge_505
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_508, label %merge_509

merge_509:                                        ; preds = %then_508, %merge_507
  ret i64 0

then_502:                                         ; preds = %entry
  %"%1199.load" = load i64, ptr %val2, align 8
  store i64 %"%1199.load", ptr %"%hc_base_0", align 8
  br label %merge_503

then_504:                                         ; preds = %merge_503
  %"%1203.load" = load i64, ptr %val2, align 8
  store i64 %"%1203.load", ptr %"%hc_base_1", align 8
  br label %merge_505

then_506:                                         ; preds = %merge_505
  %"%1207.load" = load i64, ptr %val2, align 8
  store i64 %"%1207.load", ptr %"%hc_base_2", align 8
  br label %merge_507

then_508:                                         ; preds = %merge_507
  %"%1211.load" = load i64, ptr %val2, align 8
  store i64 %"%1211.load", ptr %"%hc_base_3", align 8
  br label %merge_509
}

define i64 @usb_build_setup(i64 %buf, i64 %req_type, i64 %request, i64 %w_value, i64 %w_index, i64 %w_length) {
entry:
  %w_length6 = alloca i64, align 8
  store i64 %w_length, ptr %w_length6, align 8
  %w_index5 = alloca i64, align 8
  store i64 %w_index, ptr %w_index5, align 8
  %w_value4 = alloca i64, align 8
  store i64 %w_value, ptr %w_value4, align 8
  %request3 = alloca i64, align 8
  store i64 %request, ptr %request3, align 8
  %req_type2 = alloca i64, align 8
  store i64 %req_type, ptr %req_type2, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  %"%1214.load" = load i64, ptr %req_type2, align 8
  call void @sad_ll_mem_write8(i64 %"%buf.load", i64 %"%1214.load")
  %"%buf.load7" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load7", 1
  %"%1219.load" = load i64, ptr %request3, align 8
  call void @sad_ll_mem_write8(i64 %addtmp, i64 %"%1219.load")
  %"%buf.load8" = load i64, ptr %buf1, align 8
  %addtmp9 = add i64 %"%buf.load8", 2
  %"%w_value.load" = load i64, ptr %w_value4, align 8
  %andtmp = and i64 %"%w_value.load", 255
  call void @sad_ll_mem_write8(i64 %addtmp9, i64 %andtmp)
  %"%buf.load10" = load i64, ptr %buf1, align 8
  %addtmp11 = add i64 %"%buf.load10", 3
  %"%w_value.load12" = load i64, ptr %w_value4, align 8
  %shrtmp = lshr i64 %"%w_value.load12", 8
  call void @sad_ll_mem_write8(i64 %addtmp11, i64 %shrtmp)
  %"%buf.load13" = load i64, ptr %buf1, align 8
  %addtmp14 = add i64 %"%buf.load13", 4
  %"%w_index.load" = load i64, ptr %w_index5, align 8
  %andtmp15 = and i64 %"%w_index.load", 255
  call void @sad_ll_mem_write8(i64 %addtmp14, i64 %andtmp15)
  %"%buf.load16" = load i64, ptr %buf1, align 8
  %addtmp17 = add i64 %"%buf.load16", 5
  %"%w_index.load18" = load i64, ptr %w_index5, align 8
  %shrtmp19 = lshr i64 %"%w_index.load18", 8
  call void @sad_ll_mem_write8(i64 %addtmp17, i64 %shrtmp19)
  %"%buf.load20" = load i64, ptr %buf1, align 8
  %addtmp21 = add i64 %"%buf.load20", 6
  %"%w_length.load" = load i64, ptr %w_length6, align 8
  %andtmp22 = and i64 %"%w_length.load", 255
  call void @sad_ll_mem_write8(i64 %addtmp21, i64 %andtmp22)
  %"%buf.load23" = load i64, ptr %buf1, align 8
  %addtmp24 = add i64 %"%buf.load23", 7
  %"%w_length.load25" = load i64, ptr %w_length6, align 8
  %shrtmp26 = lshr i64 %"%w_length.load25", 8
  call void @sad_ll_mem_write8(i64 %addtmp24, i64 %shrtmp26)
  ret i64 8
}

define i64 @usb_parse_device_descriptor(i64 %buf, i64 %dev_idx) {
entry:
  %"%num_configs" = alloca i64, align 8
  %"%pid" = alloca i64, align 8
  %"%pid_hi" = alloca i64, align 8
  %"%pid_lo" = alloca i64, align 8
  %"%vid" = alloca i64, align 8
  %"%vid_hi" = alloca i64, align 8
  %"%vid_lo" = alloca i64, align 8
  %"%max_pkt" = alloca i64, align 8
  %"%dev_protocol" = alloca i64, align 8
  %"%dev_subclass" = alloca i64, align 8
  %"%dev_class" = alloca i64, align 8
  %"%desc_type" = alloca i64, align 8
  %dev_idx2 = alloca i64, align 8
  store i64 %dev_idx, ptr %dev_idx2, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load", 1
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%desc_type", align 8
  %"%desc_type.load" = load i64, ptr %"%desc_type", align 8
  %"%USB_DESC_DEVICE.load" = load i64, ptr @USB_DESC_DEVICE, align 8
  %cmpnetmp = icmp ne i64 %"%desc_type.load", %"%USB_DESC_DEVICE.load"
  br i1 %cmpnetmp, label %then_510, label %merge_511

merge_511:                                        ; preds = %entry
  %"%buf.load3" = load i64, ptr %buf1, align 8
  %addtmp4 = add i64 %"%buf.load3", 4
  call void @sad_ll_mem_read8(i64 %addtmp4)
  store i64 0, ptr %"%dev_class", align 8
  %"%buf.load5" = load i64, ptr %buf1, align 8
  %addtmp6 = add i64 %"%buf.load5", 5
  call void @sad_ll_mem_read8(i64 %addtmp6)
  store i64 0, ptr %"%dev_subclass", align 8
  %"%buf.load7" = load i64, ptr %buf1, align 8
  %addtmp8 = add i64 %"%buf.load7", 6
  call void @sad_ll_mem_read8(i64 %addtmp8)
  store i64 0, ptr %"%dev_protocol", align 8
  %"%buf.load9" = load i64, ptr %buf1, align 8
  %addtmp10 = add i64 %"%buf.load9", 7
  call void @sad_ll_mem_read8(i64 %addtmp10)
  store i64 0, ptr %"%max_pkt", align 8
  %"%buf.load11" = load i64, ptr %buf1, align 8
  %addtmp12 = add i64 %"%buf.load11", 8
  call void @sad_ll_mem_read8(i64 %addtmp12)
  store i64 0, ptr %"%vid_lo", align 8
  %"%buf.load13" = load i64, ptr %buf1, align 8
  %addtmp14 = add i64 %"%buf.load13", 9
  call void @sad_ll_mem_read8(i64 %addtmp14)
  store i64 0, ptr %"%vid_hi", align 8
  %"%vid_hi.load" = load i64, ptr %"%vid_hi", align 8
  %shltmp = shl i64 %"%vid_hi.load", 8
  %"%vid_lo.load" = load i64, ptr %"%vid_lo", align 8
  %addtmp15 = add i64 %shltmp, %"%vid_lo.load"
  store i64 %addtmp15, ptr %"%vid", align 8
  %"%buf.load16" = load i64, ptr %buf1, align 8
  %addtmp17 = add i64 %"%buf.load16", 10
  call void @sad_ll_mem_read8(i64 %addtmp17)
  store i64 0, ptr %"%pid_lo", align 8
  %"%buf.load18" = load i64, ptr %buf1, align 8
  %addtmp19 = add i64 %"%buf.load18", 11
  call void @sad_ll_mem_read8(i64 %addtmp19)
  store i64 0, ptr %"%pid_hi", align 8
  %"%pid_hi.load" = load i64, ptr %"%pid_hi", align 8
  %shltmp20 = shl i64 %"%pid_hi.load", 8
  %"%pid_lo.load" = load i64, ptr %"%pid_lo", align 8
  %addtmp21 = add i64 %shltmp20, %"%pid_lo.load"
  store i64 %addtmp21, ptr %"%pid", align 8
  %"%buf.load22" = load i64, ptr %buf1, align 8
  %addtmp23 = add i64 %"%buf.load22", 17
  call void @sad_ll_mem_read8(i64 %addtmp23)
  store i64 0, ptr %"%num_configs", align 8
  %"%dev_class.load" = load i64, ptr %"%dev_class", align 8
  %shltmp24 = shl i64 %"%dev_class.load", 8
  %"%dev_subclass.load" = load i64, ptr %"%dev_subclass", align 8
  %addtmp25 = add i64 %shltmp24, %"%dev_subclass.load"
  %"%dev_idx.load" = load i64, ptr %dev_idx2, align 8
  %calltmp = call i64 @usb_set_class(i64 %"%dev_idx.load", i64 %addtmp25)
  %"%dev_idx.load26" = load i64, ptr %dev_idx2, align 8
  %"%dev_protocol.load" = load i64, ptr %"%dev_protocol", align 8
  %calltmp27 = call i64 @usb_set_proto(i64 %"%dev_idx.load26", i64 %"%dev_protocol.load")
  %"%dev_idx.load28" = load i64, ptr %dev_idx2, align 8
  %"%max_pkt.load" = load i64, ptr %"%max_pkt", align 8
  %calltmp29 = call i64 @usb_set_maxpkt(i64 %"%dev_idx.load28", i64 %"%max_pkt.load")
  %"%dev_idx.load30" = load i64, ptr %dev_idx2, align 8
  %"%vid.load" = load i64, ptr %"%vid", align 8
  %calltmp31 = call i64 @usb_set_vid(i64 %"%dev_idx.load30", i64 %"%vid.load")
  %calltmp32 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %"%1320.load" = load i64, ptr %"%vid", align 8
  %calltmp33 = call i64 @usb_serial_hex16(i64 %"%1320.load")
  %calltmp34 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %"%1325.load" = load i64, ptr %"%pid", align 8
  %calltmp35 = call i64 @usb_serial_hex16(i64 %"%1325.load")
  %calltmp36 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %"%1330.load" = load i64, ptr %"%dev_class", align 8
  %calltmp37 = call i64 @usb_serial_hex8(i64 %"%1330.load")
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 58)
  %"%1335.load" = load i64, ptr %"%dev_subclass", align 8
  %calltmp38 = call i64 @usb_serial_hex8(i64 %"%1335.load")
  %calltmp39 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %"%1340.load" = load i64, ptr %"%max_pkt", align 8
  %calltmp40 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%1340.load")
  %calltmp41 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 1

then_510:                                         ; preds = %entry
  ret i64 0
}

define i64 @usb_parse_config_descriptor(i64 %buf, i64 %total_len, i64 %dev_idx) {
entry:
  %"%ep_type" = alloca i64, align 8
  %"%ep_interval" = alloca i64, align 8
  %"%ep_maxpkt_hi" = alloca i64, align 8
  %"%ep_maxpkt_lo" = alloca i64, align 8
  %"%ep_attr" = alloca i64, align 8
  %"%ep_addr" = alloca i64, align 8
  %"%cur_class" = alloca i64, align 8
  %"%bDescType" = alloca i64, align 8
  %"%bLength" = alloca i64, align 8
  %"%ep_count" = alloca i64, align 8
  %"%iface_proto" = alloca i64, align 8
  %"%iface_sub" = alloca i64, align 8
  %"%iface_class" = alloca i64, align 8
  %"%pos" = alloca i64, align 8
  %dev_idx3 = alloca i64, align 8
  store i64 %dev_idx, ptr %dev_idx3, align 8
  %total_len2 = alloca i64, align 8
  store i64 %total_len, ptr %total_len2, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  store i64 0, ptr %"%pos", align 8
  store i64 0, ptr %"%iface_class", align 8
  store i64 0, ptr %"%iface_sub", align 8
  store i64 0, ptr %"%iface_proto", align 8
  store i64 0, ptr %"%ep_count", align 8
  br label %while_cond_512

merge_516:                                        ; preds = %while_body_513
  %"%buf.load6" = load i64, ptr %buf1, align 8
  %"%pos.load7" = load i64, ptr %"%pos", align 8
  %addtmp8 = add i64 %"%buf.load6", %"%pos.load7"
  %addtmp9 = add i64 %addtmp8, 1
  call void @sad_ll_mem_read8(i64 %addtmp9)
  store i64 0, ptr %"%bDescType", align 8
  %"%bDescType.load" = load i64, ptr %"%bDescType", align 8
  %"%USB_DESC_INTERFACE.load" = load i64, ptr @USB_DESC_INTERFACE, align 8
  %cmpeqtmp10 = icmp eq i64 %"%bDescType.load", %"%USB_DESC_INTERFACE.load"
  br i1 %cmpeqtmp10, label %then_517, label %merge_518

merge_518:                                        ; preds = %merge_520, %merge_516
  %"%bDescType.load18" = load i64, ptr %"%bDescType", align 8
  %"%USB_DESC_ENDPOINT.load" = load i64, ptr @USB_DESC_ENDPOINT, align 8
  %cmpeqtmp19 = icmp eq i64 %"%bDescType.load18", %"%USB_DESC_ENDPOINT.load"
  br i1 %cmpeqtmp19, label %then_521, label %merge_522

merge_520:                                        ; preds = %then_519, %then_517
  %calltmp25 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%1407.load" = load i64, ptr %"%iface_class", align 8
  %calltmp26 = call i64 @usb_serial_hex8(i64 %"%1407.load")
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 58)
  %"%1412.load" = load i64, ptr %"%iface_sub", align 8
  %calltmp27 = call i64 @usb_serial_hex8(i64 %"%1412.load")
  %"%COM1.load28" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load28", i64 58)
  %"%1417.load" = load i64, ptr %"%iface_proto", align 8
  %calltmp29 = call i64 @usb_serial_hex8(i64 %"%1417.load")
  %calltmp30 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_518

merge_522:                                        ; preds = %merge_530, %merge_518
  %"%pos.load43" = load i64, ptr %"%pos", align 8
  %"%bLength.load44" = load i64, ptr %"%bLength", align 8
  %addtmp45 = add i64 %"%pos.load43", %"%bLength.load44"
  store i64 %addtmp45, ptr %"%pos", align 8
  br label %while_cond_512

merge_524:                                        ; preds = %then_523, %then_521
  %"%ep_type.load47" = load i64, ptr %"%ep_type", align 8
  %"%USB_EP_BULK.load" = load i64, ptr @USB_EP_BULK, align 8
  %cmpeqtmp48 = icmp eq i64 %"%ep_type.load47", %"%USB_EP_BULK.load"
  br i1 %cmpeqtmp48, label %then_525, label %merge_526

merge_526:                                        ; preds = %then_525, %merge_524
  %"%ep_type.load50" = load i64, ptr %"%ep_type", align 8
  %"%USB_EP_INTERRUPT.load" = load i64, ptr @USB_EP_INTERRUPT, align 8
  %cmpeqtmp51 = icmp eq i64 %"%ep_type.load50", %"%USB_EP_INTERRUPT.load"
  br i1 %cmpeqtmp51, label %then_527, label %merge_528

merge_528:                                        ; preds = %then_527, %merge_526
  %"%ep_type.load53" = load i64, ptr %"%ep_type", align 8
  %"%USB_EP_ISOCHRONOUS.load" = load i64, ptr @USB_EP_ISOCHRONOUS, align 8
  %cmpeqtmp54 = icmp eq i64 %"%ep_type.load53", %"%USB_EP_ISOCHRONOUS.load"
  br i1 %cmpeqtmp54, label %then_529, label %merge_530

merge_530:                                        ; preds = %then_529, %merge_528
  %calltmp56 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %"%ep_maxpkt_hi.load" = load i64, ptr %"%ep_maxpkt_hi", align 8
  %shltmp57 = shl i64 %"%ep_maxpkt_hi.load", 8
  %"%ep_maxpkt_lo.load" = load i64, ptr %"%ep_maxpkt_lo", align 8
  %addtmp58 = add i64 %shltmp57, %"%ep_maxpkt_lo.load"
  %calltmp59 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %addtmp58)
  %calltmp60 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  %"%ep_count.load" = load i64, ptr %"%ep_count", align 8
  %addtmp61 = add i64 %"%ep_count.load", 1
  store i64 %addtmp61, ptr %"%ep_count", align 8
  br label %merge_522

then_515:                                         ; preds = %while_body_513
  %loadtmp5 = load i64, ptr %"%ep_count", align 8
  ret i64 %loadtmp5

then_517:                                         ; preds = %merge_516
  %"%buf.load11" = load i64, ptr %buf1, align 8
  %"%pos.load12" = load i64, ptr %"%pos", align 8
  %addtmp13 = add i64 %"%buf.load11", %"%pos.load12"
  %addtmp14 = add i64 %addtmp13, 5
  call void @sad_ll_mem_read8(i64 %addtmp14)
  store i64 0, ptr %"%iface_class", align 8
  %addtmp15 = add i64 %addtmp13, 6
  call void @sad_ll_mem_read8(i64 %addtmp15)
  store i64 0, ptr %"%iface_sub", align 8
  %addtmp16 = add i64 %addtmp13, 7
  call void @sad_ll_mem_read8(i64 %addtmp16)
  store i64 0, ptr %"%iface_proto", align 8
  %"%dev_idx.load" = load i64, ptr %dev_idx3, align 8
  %calltmp = call i64 @usb_get_class(i64 %"%dev_idx.load")
  store i64 %calltmp, ptr %"%cur_class", align 8
  %"%cur_class.load" = load i64, ptr %"%cur_class", align 8
  %cmpeqtmp17 = icmp eq i64 %"%cur_class.load", 0
  br i1 %cmpeqtmp17, label %then_519, label %merge_520

then_519:                                         ; preds = %then_517
  %"%iface_class.load" = load i64, ptr %"%iface_class", align 8
  %shltmp = shl i64 %"%iface_class.load", 8
  %"%iface_sub.load" = load i64, ptr %"%iface_sub", align 8
  %addtmp20 = add i64 %shltmp, %"%iface_sub.load"
  %"%dev_idx.load21" = load i64, ptr %dev_idx3, align 8
  %calltmp22 = call i64 @usb_set_class(i64 %"%dev_idx.load21", i64 %addtmp20)
  %"%dev_idx.load23" = load i64, ptr %dev_idx3, align 8
  %"%iface_proto.load" = load i64, ptr %"%iface_proto", align 8
  %calltmp24 = call i64 @usb_set_proto(i64 %"%dev_idx.load23", i64 %"%iface_proto.load")
  br label %merge_520

then_521:                                         ; preds = %merge_518
  %"%buf.load31" = load i64, ptr %buf1, align 8
  %"%pos.load32" = load i64, ptr %"%pos", align 8
  %addtmp33 = add i64 %"%buf.load31", %"%pos.load32"
  %addtmp34 = add i64 %addtmp33, 2
  call void @sad_ll_mem_read8(i64 %addtmp34)
  store i64 0, ptr %"%ep_addr", align 8
  %addtmp35 = add i64 %addtmp33, 3
  call void @sad_ll_mem_read8(i64 %addtmp35)
  store i64 0, ptr %"%ep_attr", align 8
  %addtmp36 = add i64 %addtmp33, 4
  call void @sad_ll_mem_read8(i64 %addtmp36)
  store i64 0, ptr %"%ep_maxpkt_lo", align 8
  %addtmp37 = add i64 %addtmp33, 5
  call void @sad_ll_mem_read8(i64 %addtmp37)
  store i64 0, ptr %"%ep_maxpkt_hi", align 8
  %addtmp38 = add i64 %addtmp33, 6
  call void @sad_ll_mem_read8(i64 %addtmp38)
  store i64 0, ptr %"%ep_interval", align 8
  %"%ep_attr.load" = load i64, ptr %"%ep_attr", align 8
  %andtmp = and i64 %"%ep_attr.load", 3
  store i64 %andtmp, ptr %"%ep_type", align 8
  %calltmp39 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %"%1456.load" = load i64, ptr %"%ep_addr", align 8
  %calltmp40 = call i64 @usb_serial_hex8(i64 %"%1456.load")
  %calltmp41 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  %"%ep_type.load" = load i64, ptr %"%ep_type", align 8
  %"%USB_EP_CONTROL.load" = load i64, ptr @USB_EP_CONTROL, align 8
  %cmpeqtmp42 = icmp eq i64 %"%ep_type.load", %"%USB_EP_CONTROL.load"
  br i1 %cmpeqtmp42, label %then_523, label %merge_524

then_523:                                         ; preds = %then_521
  %calltmp46 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  br label %merge_524

then_525:                                         ; preds = %merge_524
  %calltmp49 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  br label %merge_526

then_527:                                         ; preds = %merge_526
  %calltmp52 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  br label %merge_528

then_529:                                         ; preds = %merge_528
  %calltmp55 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  br label %merge_530

while_body_513:                                   ; preds = %while_cond_512
  %"%buf.load" = load i64, ptr %buf1, align 8
  %"%pos.load4" = load i64, ptr %"%pos", align 8
  %addtmp = add i64 %"%buf.load", %"%pos.load4"
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%bLength", align 8
  %"%bLength.load" = load i64, ptr %"%bLength", align 8
  %cmpeqtmp = icmp eq i64 %"%bLength.load", 0
  br i1 %cmpeqtmp, label %then_515, label %merge_516

while_cond_512:                                   ; preds = %merge_522, %entry
  %"%pos.load" = load i64, ptr %"%pos", align 8
  %"%total_len.load" = load i64, ptr %total_len2, align 8
  %cmplttmp = icmp slt i64 %"%pos.load", %"%total_len.load"
  br i1 %cmplttmp, label %while_body_513, label %while_exit_514

while_exit_514:                                   ; preds = %while_cond_512
  %loadtmp = load i64, ptr %"%ep_count", align 8
  ret i64 %loadtmp
}

define i64 @usb_classify_device(i64 %dev_idx) {
entry:
  %"%usb_other_devices" = alloca i64, align 8
  %"%usb_hub_devices" = alloca i64, align 8
  %"%usb_storage_devices" = alloca i64, align 8
  %"%usb_hid_devices" = alloca i64, align 8
  %"%proto" = alloca i64, align 8
  %"%sub" = alloca i64, align 8
  %"%cls" = alloca i64, align 8
  %"%cls_full" = alloca i64, align 8
  %dev_idx1 = alloca i64, align 8
  store i64 %dev_idx, ptr %dev_idx1, align 8
  %"%dev_idx.load" = load i64, ptr %dev_idx1, align 8
  %calltmp = call i64 @usb_get_class(i64 %"%dev_idx.load")
  store i64 %calltmp, ptr %"%cls_full", align 8
  %"%cls_full.load" = load i64, ptr %"%cls_full", align 8
  %shrtmp = lshr i64 %"%cls_full.load", 8
  store i64 %shrtmp, ptr %"%cls", align 8
  %"%cls_full.load2" = load i64, ptr %"%cls_full", align 8
  %andtmp = and i64 %"%cls_full.load2", 255
  store i64 %andtmp, ptr %"%sub", align 8
  %"%dev_idx.load3" = load i64, ptr %dev_idx1, align 8
  %calltmp4 = call i64 @usb_get_proto(i64 %"%dev_idx.load3")
  store i64 %calltmp4, ptr %"%proto", align 8
  %"%cls.load" = load i64, ptr %"%cls", align 8
  %"%USB_CLASS_HID.load" = load i64, ptr @USB_CLASS_HID, align 8
  %cmpeqtmp = icmp eq i64 %"%cls.load", %"%USB_CLASS_HID.load"
  br i1 %cmpeqtmp, label %then_531, label %merge_532

else_535:                                         ; preds = %then_531
  %"%proto.load10" = load i64, ptr %"%proto", align 8
  %"%USB_PROTOCOL_MOUSE.load" = load i64, ptr @USB_PROTOCOL_MOUSE, align 8
  %cmpeqtmp11 = icmp eq i64 %"%proto.load10", %"%USB_PROTOCOL_MOUSE.load"
  br i1 %cmpeqtmp11, label %then_536, label %else_538

else_538:                                         ; preds = %else_535
  %calltmp14 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  %calltmp15 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_537

merge_532:                                        ; preds = %entry
  %"%cls.load6" = load i64, ptr %"%cls", align 8
  %"%USB_CLASS_STORAGE.load" = load i64, ptr @USB_CLASS_STORAGE, align 8
  %cmpeqtmp7 = icmp eq i64 %"%cls.load6", %"%USB_CLASS_STORAGE.load"
  br i1 %cmpeqtmp7, label %then_539, label %merge_540

merge_534:                                        ; preds = %merge_537, %then_533
  %loadtmp = load i64, ptr @USB_CLASS_HID, align 8
  ret i64 %loadtmp

merge_537:                                        ; preds = %else_538, %then_536
  br label %merge_534

merge_540:                                        ; preds = %merge_532
  %"%cls.load20" = load i64, ptr %"%cls", align 8
  %"%USB_CLASS_HUB.load" = load i64, ptr @USB_CLASS_HUB, align 8
  %cmpeqtmp21 = icmp eq i64 %"%cls.load20", %"%USB_CLASS_HUB.load"
  br i1 %cmpeqtmp21, label %then_541, label %merge_542

merge_542:                                        ; preds = %merge_540
  %"%cls.load26" = load i64, ptr %"%cls", align 8
  %"%USB_CLASS_AUDIO.load" = load i64, ptr @USB_CLASS_AUDIO, align 8
  %cmpeqtmp27 = icmp eq i64 %"%cls.load26", %"%USB_CLASS_AUDIO.load"
  br i1 %cmpeqtmp27, label %then_543, label %merge_544

merge_544:                                        ; preds = %then_543, %merge_542
  %"%cls.load30" = load i64, ptr %"%cls", align 8
  %"%USB_CLASS_CDC.load" = load i64, ptr @USB_CLASS_CDC, align 8
  %cmpeqtmp31 = icmp eq i64 %"%cls.load30", %"%USB_CLASS_CDC.load"
  br i1 %cmpeqtmp31, label %then_545, label %merge_546

merge_546:                                        ; preds = %then_545, %merge_544
  %"%cls.load34" = load i64, ptr %"%cls", align 8
  %"%USB_CLASS_PRINTER.load" = load i64, ptr @USB_CLASS_PRINTER, align 8
  %cmpeqtmp35 = icmp eq i64 %"%cls.load34", %"%USB_CLASS_PRINTER.load"
  br i1 %cmpeqtmp35, label %then_547, label %merge_548

merge_548:                                        ; preds = %then_547, %merge_546
  %"%usb_other_devices.load" = load i64, ptr @usb_other_devices, align 8
  %addtmp38 = add i64 %"%usb_other_devices.load", 1
  store i64 %addtmp38, ptr %"%usb_other_devices", align 8
  %loadtmp39 = load i64, ptr %"%cls", align 8
  ret i64 %loadtmp39

then_531:                                         ; preds = %entry
  %"%usb_hid_devices.load" = load i64, ptr @usb_hid_devices, align 8
  %addtmp = add i64 %"%usb_hid_devices.load", 1
  store i64 %addtmp, ptr %"%usb_hid_devices", align 8
  %"%proto.load" = load i64, ptr %"%proto", align 8
  %"%USB_PROTOCOL_KEYBOARD.load" = load i64, ptr @USB_PROTOCOL_KEYBOARD, align 8
  %cmpeqtmp5 = icmp eq i64 %"%proto.load", %"%USB_PROTOCOL_KEYBOARD.load"
  br i1 %cmpeqtmp5, label %then_533, label %else_535

then_533:                                         ; preds = %then_531
  %calltmp8 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %calltmp9 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_534

then_536:                                         ; preds = %else_535
  %calltmp12 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %calltmp13 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_537

then_539:                                         ; preds = %merge_532
  %"%usb_storage_devices.load" = load i64, ptr @usb_storage_devices, align 8
  %addtmp16 = add i64 %"%usb_storage_devices.load", 1
  store i64 %addtmp16, ptr %"%usb_storage_devices", align 8
  %calltmp17 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %calltmp18 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp19 = load i64, ptr @USB_CLASS_STORAGE, align 8
  ret i64 %loadtmp19

then_541:                                         ; preds = %merge_540
  %"%usb_hub_devices.load" = load i64, ptr @usb_hub_devices, align 8
  %addtmp22 = add i64 %"%usb_hub_devices.load", 1
  store i64 %addtmp22, ptr %"%usb_hub_devices", align 8
  %calltmp23 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  %calltmp24 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp25 = load i64, ptr @USB_CLASS_HUB, align 8
  ret i64 %loadtmp25

then_543:                                         ; preds = %merge_542
  %calltmp28 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %calltmp29 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_544

then_545:                                         ; preds = %merge_544
  %calltmp32 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %calltmp33 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_546

then_547:                                         ; preds = %merge_546
  %calltmp36 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  %calltmp37 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_548
}

define i64 @usb_register_device(i64 %hc_idx, i64 %port_num, i64 %speed) {
entry:
  %"%usb_port_15" = alloca i64, align 8
  %"%usb_port_14" = alloca i64, align 8
  %"%usb_port_13" = alloca i64, align 8
  %"%usb_port_12" = alloca i64, align 8
  %"%usb_port_11" = alloca i64, align 8
  %"%usb_port_10" = alloca i64, align 8
  %"%usb_port_9" = alloca i64, align 8
  %"%usb_port_8" = alloca i64, align 8
  %"%usb_port_7" = alloca i64, align 8
  %"%usb_port_6" = alloca i64, align 8
  %"%usb_port_5" = alloca i64, align 8
  %"%usb_port_4" = alloca i64, align 8
  %"%usb_port_3" = alloca i64, align 8
  %"%usb_port_2" = alloca i64, align 8
  %"%usb_port_1" = alloca i64, align 8
  %"%usb_port_0" = alloca i64, align 8
  %"%usb_hc_15" = alloca i64, align 8
  %"%usb_hc_14" = alloca i64, align 8
  %"%usb_hc_13" = alloca i64, align 8
  %"%usb_hc_12" = alloca i64, align 8
  %"%usb_hc_11" = alloca i64, align 8
  %"%usb_hc_10" = alloca i64, align 8
  %"%usb_hc_9" = alloca i64, align 8
  %"%usb_hc_8" = alloca i64, align 8
  %"%usb_hc_7" = alloca i64, align 8
  %"%usb_hc_6" = alloca i64, align 8
  %"%usb_hc_5" = alloca i64, align 8
  %"%usb_hc_4" = alloca i64, align 8
  %"%usb_hc_3" = alloca i64, align 8
  %"%usb_hc_2" = alloca i64, align 8
  %"%usb_hc_1" = alloca i64, align 8
  %"%usb_hc_0" = alloca i64, align 8
  %"%usb_next_address" = alloca i64, align 8
  %"%new_addr" = alloca i64, align 8
  %"%usb_dev_count" = alloca i64, align 8
  %"%dev_idx" = alloca i64, align 8
  %speed3 = alloca i64, align 8
  store i64 %speed, ptr %speed3, align 8
  %port_num2 = alloca i64, align 8
  store i64 %port_num, ptr %port_num2, align 8
  %hc_idx1 = alloca i64, align 8
  store i64 %hc_idx, ptr %hc_idx1, align 8
  %"%usb_dev_count.load" = load i64, ptr @usb_dev_count, align 8
  %"%USB_MAX_DEVICES.load" = load i64, ptr @USB_MAX_DEVICES, align 8
  %cmpgetmp = icmp sge i64 %"%usb_dev_count.load", %"%USB_MAX_DEVICES.load"
  br i1 %cmpgetmp, label %then_549, label %merge_550

else_621:                                         ; preds = %merge_586
  %"%speed.load88" = load i64, ptr %speed3, align 8
  %"%USB_SPEED_FULL.load" = load i64, ptr @USB_SPEED_FULL, align 8
  %cmpeqtmp89 = icmp eq i64 %"%speed.load88", %"%USB_SPEED_FULL.load"
  br i1 %cmpeqtmp89, label %then_622, label %else_624

else_624:                                         ; preds = %else_621
  %calltmp92 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  br label %merge_623

merge_550:                                        ; preds = %entry
  %"%1556.load" = load i64, ptr @usb_dev_count, align 8
  store i64 %"%1556.load", ptr %"%dev_idx", align 8
  %"%usb_dev_count.load5" = load i64, ptr @usb_dev_count, align 8
  %addtmp = add i64 %"%usb_dev_count.load5", 1
  store i64 %addtmp, ptr %"%usb_dev_count", align 8
  %"%1560.load" = load i64, ptr @usb_next_address, align 8
  store i64 %"%1560.load", ptr %"%new_addr", align 8
  %"%usb_next_address.load" = load i64, ptr @usb_next_address, align 8
  %addtmp6 = add i64 %"%usb_next_address.load", 1
  store i64 %addtmp6, ptr %"%usb_next_address", align 8
  %"%dev_idx.load" = load i64, ptr %"%dev_idx", align 8
  %"%new_addr.load" = load i64, ptr %"%new_addr", align 8
  %calltmp7 = call i64 @usb_set_addr(i64 %"%dev_idx.load", i64 %"%new_addr.load")
  %"%dev_idx.load8" = load i64, ptr %"%dev_idx", align 8
  %"%USB_DEV_ATTACHED.load" = load i64, ptr @USB_DEV_ATTACHED, align 8
  %calltmp9 = call i64 @usb_set_state(i64 %"%dev_idx.load8", i64 %"%USB_DEV_ATTACHED.load")
  %"%dev_idx.load10" = load i64, ptr %"%dev_idx", align 8
  %"%speed.load" = load i64, ptr %speed3, align 8
  %calltmp11 = call i64 @usb_set_speed(i64 %"%dev_idx.load10", i64 %"%speed.load")
  %"%dev_idx.load12" = load i64, ptr %"%dev_idx", align 8
  %cmplttmp = icmp slt i64 %"%dev_idx.load12", 16
  br i1 %cmplttmp, label %then_551, label %merge_552

merge_552:                                        ; preds = %merge_584, %merge_550
  %"%dev_idx.load14" = load i64, ptr %"%dev_idx", align 8
  %cmplttmp15 = icmp slt i64 %"%dev_idx.load14", 16
  br i1 %cmplttmp15, label %then_585, label %merge_586

merge_554:                                        ; preds = %then_553, %then_551
  %"%dev_idx.load16" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp17 = icmp eq i64 %"%dev_idx.load16", 1
  br i1 %cmpeqtmp17, label %then_555, label %merge_556

merge_556:                                        ; preds = %then_555, %merge_554
  %"%dev_idx.load18" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp19 = icmp eq i64 %"%dev_idx.load18", 2
  br i1 %cmpeqtmp19, label %then_557, label %merge_558

merge_558:                                        ; preds = %then_557, %merge_556
  %"%dev_idx.load20" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp21 = icmp eq i64 %"%dev_idx.load20", 3
  br i1 %cmpeqtmp21, label %then_559, label %merge_560

merge_560:                                        ; preds = %then_559, %merge_558
  %"%dev_idx.load22" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp23 = icmp eq i64 %"%dev_idx.load22", 4
  br i1 %cmpeqtmp23, label %then_561, label %merge_562

merge_562:                                        ; preds = %then_561, %merge_560
  %"%dev_idx.load24" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp25 = icmp eq i64 %"%dev_idx.load24", 5
  br i1 %cmpeqtmp25, label %then_563, label %merge_564

merge_564:                                        ; preds = %then_563, %merge_562
  %"%dev_idx.load26" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp27 = icmp eq i64 %"%dev_idx.load26", 6
  br i1 %cmpeqtmp27, label %then_565, label %merge_566

merge_566:                                        ; preds = %then_565, %merge_564
  %"%dev_idx.load28" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp29 = icmp eq i64 %"%dev_idx.load28", 7
  br i1 %cmpeqtmp29, label %then_567, label %merge_568

merge_568:                                        ; preds = %then_567, %merge_566
  %"%dev_idx.load30" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp31 = icmp eq i64 %"%dev_idx.load30", 8
  br i1 %cmpeqtmp31, label %then_569, label %merge_570

merge_570:                                        ; preds = %then_569, %merge_568
  %"%dev_idx.load32" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp33 = icmp eq i64 %"%dev_idx.load32", 9
  br i1 %cmpeqtmp33, label %then_571, label %merge_572

merge_572:                                        ; preds = %then_571, %merge_570
  %"%dev_idx.load34" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp35 = icmp eq i64 %"%dev_idx.load34", 10
  br i1 %cmpeqtmp35, label %then_573, label %merge_574

merge_574:                                        ; preds = %then_573, %merge_572
  %"%dev_idx.load36" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp37 = icmp eq i64 %"%dev_idx.load36", 11
  br i1 %cmpeqtmp37, label %then_575, label %merge_576

merge_576:                                        ; preds = %then_575, %merge_574
  %"%dev_idx.load38" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp39 = icmp eq i64 %"%dev_idx.load38", 12
  br i1 %cmpeqtmp39, label %then_577, label %merge_578

merge_578:                                        ; preds = %then_577, %merge_576
  %"%dev_idx.load40" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp41 = icmp eq i64 %"%dev_idx.load40", 13
  br i1 %cmpeqtmp41, label %then_579, label %merge_580

merge_580:                                        ; preds = %then_579, %merge_578
  %"%dev_idx.load42" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp43 = icmp eq i64 %"%dev_idx.load42", 14
  br i1 %cmpeqtmp43, label %then_581, label %merge_582

merge_582:                                        ; preds = %then_581, %merge_580
  %"%dev_idx.load44" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp45 = icmp eq i64 %"%dev_idx.load44", 15
  br i1 %cmpeqtmp45, label %then_583, label %merge_584

merge_584:                                        ; preds = %then_583, %merge_582
  br label %merge_552

merge_586:                                        ; preds = %merge_618, %merge_552
  %calltmp48 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  %"%1701.load" = load i64, ptr %"%dev_idx", align 8
  %calltmp49 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%1701.load")
  %calltmp50 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  %"%1706.load" = load i64, ptr %"%new_addr", align 8
  %calltmp51 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%1706.load")
  %calltmp52 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  %"%1711.load" = load i64, ptr %port_num2, align 8
  %calltmp53 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%1711.load")
  %calltmp54 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %"%speed.load55" = load i64, ptr %speed3, align 8
  %"%USB_SPEED_LOW.load" = load i64, ptr @USB_SPEED_LOW, align 8
  %cmpeqtmp56 = icmp eq i64 %"%speed.load55", %"%USB_SPEED_LOW.load"
  br i1 %cmpeqtmp56, label %then_619, label %else_621

merge_588:                                        ; preds = %then_587, %then_585
  %"%dev_idx.load57" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp58 = icmp eq i64 %"%dev_idx.load57", 1
  br i1 %cmpeqtmp58, label %then_589, label %merge_590

merge_590:                                        ; preds = %then_589, %merge_588
  %"%dev_idx.load59" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp60 = icmp eq i64 %"%dev_idx.load59", 2
  br i1 %cmpeqtmp60, label %then_591, label %merge_592

merge_592:                                        ; preds = %then_591, %merge_590
  %"%dev_idx.load61" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp62 = icmp eq i64 %"%dev_idx.load61", 3
  br i1 %cmpeqtmp62, label %then_593, label %merge_594

merge_594:                                        ; preds = %then_593, %merge_592
  %"%dev_idx.load63" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp64 = icmp eq i64 %"%dev_idx.load63", 4
  br i1 %cmpeqtmp64, label %then_595, label %merge_596

merge_596:                                        ; preds = %then_595, %merge_594
  %"%dev_idx.load65" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp66 = icmp eq i64 %"%dev_idx.load65", 5
  br i1 %cmpeqtmp66, label %then_597, label %merge_598

merge_598:                                        ; preds = %then_597, %merge_596
  %"%dev_idx.load67" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp68 = icmp eq i64 %"%dev_idx.load67", 6
  br i1 %cmpeqtmp68, label %then_599, label %merge_600

merge_600:                                        ; preds = %then_599, %merge_598
  %"%dev_idx.load69" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp70 = icmp eq i64 %"%dev_idx.load69", 7
  br i1 %cmpeqtmp70, label %then_601, label %merge_602

merge_602:                                        ; preds = %then_601, %merge_600
  %"%dev_idx.load71" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp72 = icmp eq i64 %"%dev_idx.load71", 8
  br i1 %cmpeqtmp72, label %then_603, label %merge_604

merge_604:                                        ; preds = %then_603, %merge_602
  %"%dev_idx.load73" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp74 = icmp eq i64 %"%dev_idx.load73", 9
  br i1 %cmpeqtmp74, label %then_605, label %merge_606

merge_606:                                        ; preds = %then_605, %merge_604
  %"%dev_idx.load75" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp76 = icmp eq i64 %"%dev_idx.load75", 10
  br i1 %cmpeqtmp76, label %then_607, label %merge_608

merge_608:                                        ; preds = %then_607, %merge_606
  %"%dev_idx.load77" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp78 = icmp eq i64 %"%dev_idx.load77", 11
  br i1 %cmpeqtmp78, label %then_609, label %merge_610

merge_610:                                        ; preds = %then_609, %merge_608
  %"%dev_idx.load79" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp80 = icmp eq i64 %"%dev_idx.load79", 12
  br i1 %cmpeqtmp80, label %then_611, label %merge_612

merge_612:                                        ; preds = %then_611, %merge_610
  %"%dev_idx.load81" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp82 = icmp eq i64 %"%dev_idx.load81", 13
  br i1 %cmpeqtmp82, label %then_613, label %merge_614

merge_614:                                        ; preds = %then_613, %merge_612
  %"%dev_idx.load83" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp84 = icmp eq i64 %"%dev_idx.load83", 14
  br i1 %cmpeqtmp84, label %then_615, label %merge_616

merge_616:                                        ; preds = %then_615, %merge_614
  %"%dev_idx.load85" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp86 = icmp eq i64 %"%dev_idx.load85", 15
  br i1 %cmpeqtmp86, label %then_617, label %merge_618

merge_618:                                        ; preds = %then_617, %merge_616
  br label %merge_586

merge_620:                                        ; preds = %merge_623, %then_619
  %calltmp90 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr %"%dev_idx", align 8
  ret i64 %loadtmp

merge_623:                                        ; preds = %else_624, %then_622
  br label %merge_620

then_549:                                         ; preds = %entry
  %calltmp = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  %calltmp4 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -1

then_551:                                         ; preds = %merge_550
  %"%dev_idx.load13" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp = icmp eq i64 %"%dev_idx.load13", 0
  br i1 %cmpeqtmp, label %then_553, label %merge_554

then_553:                                         ; preds = %then_551
  %"%1571.load" = load i64, ptr %hc_idx1, align 8
  store i64 %"%1571.load", ptr %"%usb_hc_0", align 8
  br label %merge_554

then_555:                                         ; preds = %merge_554
  %"%1575.load" = load i64, ptr %hc_idx1, align 8
  store i64 %"%1575.load", ptr %"%usb_hc_1", align 8
  br label %merge_556

then_557:                                         ; preds = %merge_556
  %"%1579.load" = load i64, ptr %hc_idx1, align 8
  store i64 %"%1579.load", ptr %"%usb_hc_2", align 8
  br label %merge_558

then_559:                                         ; preds = %merge_558
  %"%1583.load" = load i64, ptr %hc_idx1, align 8
  store i64 %"%1583.load", ptr %"%usb_hc_3", align 8
  br label %merge_560

then_561:                                         ; preds = %merge_560
  %"%1587.load" = load i64, ptr %hc_idx1, align 8
  store i64 %"%1587.load", ptr %"%usb_hc_4", align 8
  br label %merge_562

then_563:                                         ; preds = %merge_562
  %"%1591.load" = load i64, ptr %hc_idx1, align 8
  store i64 %"%1591.load", ptr %"%usb_hc_5", align 8
  br label %merge_564

then_565:                                         ; preds = %merge_564
  %"%1595.load" = load i64, ptr %hc_idx1, align 8
  store i64 %"%1595.load", ptr %"%usb_hc_6", align 8
  br label %merge_566

then_567:                                         ; preds = %merge_566
  %"%1599.load" = load i64, ptr %hc_idx1, align 8
  store i64 %"%1599.load", ptr %"%usb_hc_7", align 8
  br label %merge_568

then_569:                                         ; preds = %merge_568
  %"%1603.load" = load i64, ptr %hc_idx1, align 8
  store i64 %"%1603.load", ptr %"%usb_hc_8", align 8
  br label %merge_570

then_571:                                         ; preds = %merge_570
  %"%1607.load" = load i64, ptr %hc_idx1, align 8
  store i64 %"%1607.load", ptr %"%usb_hc_9", align 8
  br label %merge_572

then_573:                                         ; preds = %merge_572
  %"%1611.load" = load i64, ptr %hc_idx1, align 8
  store i64 %"%1611.load", ptr %"%usb_hc_10", align 8
  br label %merge_574

then_575:                                         ; preds = %merge_574
  %"%1615.load" = load i64, ptr %hc_idx1, align 8
  store i64 %"%1615.load", ptr %"%usb_hc_11", align 8
  br label %merge_576

then_577:                                         ; preds = %merge_576
  %"%1619.load" = load i64, ptr %hc_idx1, align 8
  store i64 %"%1619.load", ptr %"%usb_hc_12", align 8
  br label %merge_578

then_579:                                         ; preds = %merge_578
  %"%1623.load" = load i64, ptr %hc_idx1, align 8
  store i64 %"%1623.load", ptr %"%usb_hc_13", align 8
  br label %merge_580

then_581:                                         ; preds = %merge_580
  %"%1627.load" = load i64, ptr %hc_idx1, align 8
  store i64 %"%1627.load", ptr %"%usb_hc_14", align 8
  br label %merge_582

then_583:                                         ; preds = %merge_582
  %"%1631.load" = load i64, ptr %hc_idx1, align 8
  store i64 %"%1631.load", ptr %"%usb_hc_15", align 8
  br label %merge_584

then_585:                                         ; preds = %merge_552
  %"%dev_idx.load46" = load i64, ptr %"%dev_idx", align 8
  %cmpeqtmp47 = icmp eq i64 %"%dev_idx.load46", 0
  br i1 %cmpeqtmp47, label %then_587, label %merge_588

then_587:                                         ; preds = %then_585
  %"%1637.load" = load i64, ptr %port_num2, align 8
  store i64 %"%1637.load", ptr %"%usb_port_0", align 8
  br label %merge_588

then_589:                                         ; preds = %merge_588
  %"%1641.load" = load i64, ptr %port_num2, align 8
  store i64 %"%1641.load", ptr %"%usb_port_1", align 8
  br label %merge_590

then_591:                                         ; preds = %merge_590
  %"%1645.load" = load i64, ptr %port_num2, align 8
  store i64 %"%1645.load", ptr %"%usb_port_2", align 8
  br label %merge_592

then_593:                                         ; preds = %merge_592
  %"%1649.load" = load i64, ptr %port_num2, align 8
  store i64 %"%1649.load", ptr %"%usb_port_3", align 8
  br label %merge_594

then_595:                                         ; preds = %merge_594
  %"%1653.load" = load i64, ptr %port_num2, align 8
  store i64 %"%1653.load", ptr %"%usb_port_4", align 8
  br label %merge_596

then_597:                                         ; preds = %merge_596
  %"%1657.load" = load i64, ptr %port_num2, align 8
  store i64 %"%1657.load", ptr %"%usb_port_5", align 8
  br label %merge_598

then_599:                                         ; preds = %merge_598
  %"%1661.load" = load i64, ptr %port_num2, align 8
  store i64 %"%1661.load", ptr %"%usb_port_6", align 8
  br label %merge_600

then_601:                                         ; preds = %merge_600
  %"%1665.load" = load i64, ptr %port_num2, align 8
  store i64 %"%1665.load", ptr %"%usb_port_7", align 8
  br label %merge_602

then_603:                                         ; preds = %merge_602
  %"%1669.load" = load i64, ptr %port_num2, align 8
  store i64 %"%1669.load", ptr %"%usb_port_8", align 8
  br label %merge_604

then_605:                                         ; preds = %merge_604
  %"%1673.load" = load i64, ptr %port_num2, align 8
  store i64 %"%1673.load", ptr %"%usb_port_9", align 8
  br label %merge_606

then_607:                                         ; preds = %merge_606
  %"%1677.load" = load i64, ptr %port_num2, align 8
  store i64 %"%1677.load", ptr %"%usb_port_10", align 8
  br label %merge_608

then_609:                                         ; preds = %merge_608
  %"%1681.load" = load i64, ptr %port_num2, align 8
  store i64 %"%1681.load", ptr %"%usb_port_11", align 8
  br label %merge_610

then_611:                                         ; preds = %merge_610
  %"%1685.load" = load i64, ptr %port_num2, align 8
  store i64 %"%1685.load", ptr %"%usb_port_12", align 8
  br label %merge_612

then_613:                                         ; preds = %merge_612
  %"%1689.load" = load i64, ptr %port_num2, align 8
  store i64 %"%1689.load", ptr %"%usb_port_13", align 8
  br label %merge_614

then_615:                                         ; preds = %merge_614
  %"%1693.load" = load i64, ptr %port_num2, align 8
  store i64 %"%1693.load", ptr %"%usb_port_14", align 8
  br label %merge_616

then_617:                                         ; preds = %merge_616
  %"%1697.load" = load i64, ptr %port_num2, align 8
  store i64 %"%1697.load", ptr %"%usb_port_15", align 8
  br label %merge_618

then_619:                                         ; preds = %merge_586
  %calltmp87 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  br label %merge_620

then_622:                                         ; preds = %else_621
  %calltmp91 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  br label %merge_623
}

define i64 @usb_detect_controllers() {
entry:
  %"%usb_hc_count" = alloca i64, align 8
  %"%func" = alloca i64, align 8
  %"%dev" = alloca i64, align 8
  %"%bus" = alloca i64, align 8
  %"%hc_pci_idx_3" = alloca i64, align 8
  %"%hc_pci_idx_2" = alloca i64, align 8
  %"%hc_pci_idx_1" = alloca i64, align 8
  %"%hc_pci_idx_0" = alloca i64, align 8
  %"%hc_irq_3" = alloca i64, align 8
  %"%hc_irq_2" = alloca i64, align 8
  %"%hc_irq_1" = alloca i64, align 8
  %"%hc_irq_0" = alloca i64, align 8
  %"%base_addr" = alloca i64, align 8
  %"%irq" = alloca i64, align 8
  %"%bar0" = alloca i64, align 8
  %"%usb_type" = alloca i64, align 8
  %"%hc_idx" = alloca i64, align 8
  %"%pci_idx" = alloca i64, align 8
  %"%search" = alloca i64, align 8
  %calltmp = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  %calltmp1 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%search", align 8
  br label %while_cond_625

else_630:                                         ; preds = %while_body_626
  %"%1742.load" = load i64, ptr @usb_hc_count, align 8
  store i64 %"%1742.load", ptr %"%hc_idx", align 8
  %"%hc_idx.load" = load i64, ptr %"%hc_idx", align 8
  %"%USB_MAX_CONTROLLERS.load8" = load i64, ptr @USB_MAX_CONTROLLERS, align 8
  %cmplttmp9 = icmp slt i64 %"%hc_idx.load", %"%USB_MAX_CONTROLLERS.load8"
  br i1 %cmplttmp9, label %then_631, label %merge_632

else_651:                                         ; preds = %merge_648
  %"%usb_type.load41" = load i64, ptr %"%usb_type", align 8
  %"%USB_HC_OHCI.load" = load i64, ptr @USB_HC_OHCI, align 8
  %cmpeqtmp42 = icmp eq i64 %"%usb_type.load41", %"%USB_HC_OHCI.load"
  br i1 %cmpeqtmp42, label %then_652, label %else_654

else_654:                                         ; preds = %else_651
  %"%usb_type.load52" = load i64, ptr %"%usb_type", align 8
  %"%USB_HC_EHCI.load" = load i64, ptr @USB_HC_EHCI, align 8
  %cmpeqtmp53 = icmp eq i64 %"%usb_type.load52", %"%USB_HC_EHCI.load"
  br i1 %cmpeqtmp53, label %then_655, label %else_657

else_657:                                         ; preds = %else_654
  %"%usb_type.load55" = load i64, ptr %"%usb_type", align 8
  %"%USB_HC_XHCI.load" = load i64, ptr @USB_HC_XHCI, align 8
  %cmpeqtmp56 = icmp eq i64 %"%usb_type.load55", %"%USB_HC_XHCI.load"
  br i1 %cmpeqtmp56, label %then_658, label %else_660

else_660:                                         ; preds = %else_657
  %calltmp58 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.39 to i64))
  br label %merge_659

merge_629:                                        ; preds = %merge_632, %then_628
  br label %while_cond_625

merge_632:                                        ; preds = %merge_650, %else_630
  %"%search.load17" = load i64, ptr %"%search", align 8
  %addtmp = add i64 %"%search.load17", 1
  store i64 %addtmp, ptr %"%search", align 8
  br label %merge_629

merge_634:                                        ; preds = %then_633, %then_631
  %"%hc_idx.load18" = load i64, ptr %"%hc_idx", align 8
  %cmpeqtmp19 = icmp eq i64 %"%hc_idx.load18", 1
  br i1 %cmpeqtmp19, label %then_635, label %merge_636

merge_636:                                        ; preds = %then_635, %merge_634
  %"%hc_idx.load20" = load i64, ptr %"%hc_idx", align 8
  %cmpeqtmp21 = icmp eq i64 %"%hc_idx.load20", 2
  br i1 %cmpeqtmp21, label %then_637, label %merge_638

merge_638:                                        ; preds = %then_637, %merge_636
  %"%hc_idx.load22" = load i64, ptr %"%hc_idx", align 8
  %cmpeqtmp23 = icmp eq i64 %"%hc_idx.load22", 3
  br i1 %cmpeqtmp23, label %then_639, label %merge_640

merge_640:                                        ; preds = %then_639, %merge_638
  %"%hc_idx.load24" = load i64, ptr %"%hc_idx", align 8
  %cmpeqtmp25 = icmp eq i64 %"%hc_idx.load24", 0
  br i1 %cmpeqtmp25, label %then_641, label %merge_642

merge_642:                                        ; preds = %then_641, %merge_640
  %"%hc_idx.load26" = load i64, ptr %"%hc_idx", align 8
  %cmpeqtmp27 = icmp eq i64 %"%hc_idx.load26", 1
  br i1 %cmpeqtmp27, label %then_643, label %merge_644

merge_644:                                        ; preds = %then_643, %merge_642
  %"%hc_idx.load28" = load i64, ptr %"%hc_idx", align 8
  %cmpeqtmp29 = icmp eq i64 %"%hc_idx.load28", 2
  br i1 %cmpeqtmp29, label %then_645, label %merge_646

merge_646:                                        ; preds = %then_645, %merge_644
  %"%hc_idx.load30" = load i64, ptr %"%hc_idx", align 8
  %cmpeqtmp31 = icmp eq i64 %"%hc_idx.load30", 3
  br i1 %cmpeqtmp31, label %then_647, label %merge_648

merge_648:                                        ; preds = %then_647, %merge_646
  %"%1799.load" = load i64, ptr %"%pci_idx", align 8
  call void @pci_loc_bus(i64 %"%1799.load")
  store i64 0, ptr %"%bus", align 8
  %"%1799.load32" = load i64, ptr %"%pci_idx", align 8
  call void @pci_loc_dev(i64 %"%1799.load32")
  store i64 0, ptr %"%dev", align 8
  %"%1799.load33" = load i64, ptr %"%pci_idx", align 8
  call void @pci_loc_func(i64 %"%1799.load33")
  store i64 0, ptr %"%func", align 8
  %"%1814.load" = load i64, ptr %"%bus", align 8
  %"%1816.load" = load i64, ptr %"%dev", align 8
  %"%1818.load" = load i64, ptr %"%func", align 8
  call void @pci_enable_bus_master(i64 %"%1814.load", i64 %"%1816.load", i64 %"%1818.load")
  %"%usb_hc_count.load" = load i64, ptr @usb_hc_count, align 8
  %addtmp34 = add i64 %"%usb_hc_count.load", 1
  store i64 %addtmp34, ptr %"%usb_hc_count", align 8
  %calltmp35 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  %"%1825.load" = load i64, ptr %"%hc_idx", align 8
  %calltmp36 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%1825.load")
  %calltmp37 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  %"%usb_type.load38" = load i64, ptr %"%usb_type", align 8
  %"%USB_HC_UHCI.load" = load i64, ptr @USB_HC_UHCI, align 8
  %cmpeqtmp39 = icmp eq i64 %"%usb_type.load38", %"%USB_HC_UHCI.load"
  br i1 %cmpeqtmp39, label %then_649, label %else_651

merge_650:                                        ; preds = %merge_653, %then_649
  %calltmp43 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.34 to i64))
  %"%base_addr.load44" = load i64, ptr %"%base_addr", align 8
  %shrtmp = lshr i64 %"%base_addr.load44", 16
  %calltmp45 = call i64 @usb_serial_hex16(i64 %shrtmp)
  %"%base_addr.load46" = load i64, ptr %"%base_addr", align 8
  %andtmp = and i64 %"%base_addr.load46", 65535
  %calltmp47 = call i64 @usb_serial_hex16(i64 %andtmp)
  %calltmp48 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.35 to i64))
  %"%1854.load" = load i64, ptr %"%irq", align 8
  %calltmp49 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%1854.load")
  %calltmp50 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_632

merge_653:                                        ; preds = %merge_656, %then_652
  br label %merge_650

merge_656:                                        ; preds = %merge_659, %then_655
  br label %merge_653

merge_659:                                        ; preds = %else_660, %then_658
  br label %merge_656

then_628:                                         ; preds = %while_body_626
  %"%USB_MAX_CONTROLLERS.load7" = load i64, ptr @USB_MAX_CONTROLLERS, align 8
  store i64 %"%USB_MAX_CONTROLLERS.load7", ptr %"%search", align 8
  br label %merge_629

then_631:                                         ; preds = %else_630
  %"%1745.load" = load i64, ptr %"%pci_idx", align 8
  call void @pci_get_usb_type(i64 %"%1745.load")
  store i64 0, ptr %"%usb_type", align 8
  %"%1745.load10" = load i64, ptr %"%pci_idx", align 8
  call void @pci_get_bar0(i64 %"%1745.load10")
  store i64 0, ptr %"%bar0", align 8
  %"%1745.load11" = load i64, ptr %"%pci_idx", align 8
  call void @pci_get_irq(i64 %"%1745.load11")
  store i64 0, ptr %"%irq", align 8
  %"%1760.load" = load i64, ptr %"%bar0", align 8
  call void @pci_bar_address(i64 %"%1760.load")
  store i64 0, ptr %"%base_addr", align 8
  %"%hc_idx.load12" = load i64, ptr %"%hc_idx", align 8
  %"%usb_type.load" = load i64, ptr %"%usb_type", align 8
  %calltmp13 = call i64 @usb_set_hc_type(i64 %"%hc_idx.load12", i64 %"%usb_type.load")
  %"%hc_idx.load14" = load i64, ptr %"%hc_idx", align 8
  %"%base_addr.load" = load i64, ptr %"%base_addr", align 8
  %calltmp15 = call i64 @usb_set_hc_base(i64 %"%hc_idx.load14", i64 %"%base_addr.load")
  %"%hc_idx.load16" = load i64, ptr %"%hc_idx", align 8
  %cmpeqtmp = icmp eq i64 %"%hc_idx.load16", 0
  br i1 %cmpeqtmp, label %then_633, label %merge_634

then_633:                                         ; preds = %then_631
  %"%1769.load" = load i64, ptr %"%irq", align 8
  store i64 %"%1769.load", ptr %"%hc_irq_0", align 8
  br label %merge_634

then_635:                                         ; preds = %merge_634
  %"%1773.load" = load i64, ptr %"%irq", align 8
  store i64 %"%1773.load", ptr %"%hc_irq_1", align 8
  br label %merge_636

then_637:                                         ; preds = %merge_636
  %"%1777.load" = load i64, ptr %"%irq", align 8
  store i64 %"%1777.load", ptr %"%hc_irq_2", align 8
  br label %merge_638

then_639:                                         ; preds = %merge_638
  %"%1781.load" = load i64, ptr %"%irq", align 8
  store i64 %"%1781.load", ptr %"%hc_irq_3", align 8
  br label %merge_640

then_641:                                         ; preds = %merge_640
  %"%1785.load" = load i64, ptr %"%pci_idx", align 8
  store i64 %"%1785.load", ptr %"%hc_pci_idx_0", align 8
  br label %merge_642

then_643:                                         ; preds = %merge_642
  %"%1789.load" = load i64, ptr %"%pci_idx", align 8
  store i64 %"%1789.load", ptr %"%hc_pci_idx_1", align 8
  br label %merge_644

then_645:                                         ; preds = %merge_644
  %"%1793.load" = load i64, ptr %"%pci_idx", align 8
  store i64 %"%1793.load", ptr %"%hc_pci_idx_2", align 8
  br label %merge_646

then_647:                                         ; preds = %merge_646
  %"%1797.load" = load i64, ptr %"%pci_idx", align 8
  store i64 %"%1797.load", ptr %"%hc_pci_idx_3", align 8
  br label %merge_648

then_649:                                         ; preds = %merge_648
  %calltmp40 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  br label %merge_650

then_652:                                         ; preds = %else_651
  %calltmp51 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.36 to i64))
  br label %merge_653

then_655:                                         ; preds = %else_654
  %calltmp54 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.37 to i64))
  br label %merge_656

then_658:                                         ; preds = %else_657
  %calltmp57 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.38 to i64))
  br label %merge_659

while_body_626:                                   ; preds = %while_cond_625
  %"%1735.load" = load i64, ptr %"%search", align 8
  call void @pci_find_usb_controller(i64 %"%1735.load")
  store i64 0, ptr %"%pci_idx", align 8
  %"%pci_idx.load" = load i64, ptr %"%pci_idx", align 8
  %cmplttmp2 = icmp slt i64 %"%pci_idx.load", 0
  br i1 %cmplttmp2, label %then_628, label %else_630

while_cond_625:                                   ; preds = %merge_629, %entry
  %"%search.load" = load i64, ptr %"%search", align 8
  %"%USB_MAX_CONTROLLERS.load" = load i64, ptr @USB_MAX_CONTROLLERS, align 8
  %cmplttmp = icmp slt i64 %"%search.load", %"%USB_MAX_CONTROLLERS.load"
  br i1 %cmplttmp, label %while_body_626, label %while_exit_627

while_exit_627:                                   ; preds = %while_cond_625
  %calltmp3 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  %"%1862.load" = load i64, ptr @usb_hc_count, align 8
  %calltmp4 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%1862.load")
  %calltmp5 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.30 to i64))
  %calltmp6 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr @usb_hc_count, align 8
  ret i64 %loadtmp
}

define i64 @usb_find_device_by_class(i64 %class_code, i64 %subclass) {
entry:
  %"%dev_sub" = alloca i64, align 8
  %"%dev_cls" = alloca i64, align 8
  %"%cls" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %subclass2 = alloca i64, align 8
  store i64 %subclass, ptr %subclass2, align 8
  %class_code1 = alloca i64, align 8
  store i64 %class_code, ptr %class_code1, align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_661

else_668:                                         ; preds = %then_664
  %"%dev_sub.load" = load i64, ptr %"%dev_sub", align 8
  %"%subclass.load7" = load i64, ptr %subclass2, align 8
  %cmpeqtmp8 = icmp eq i64 %"%dev_sub.load", %"%subclass.load7"
  br i1 %cmpeqtmp8, label %then_669, label %merge_670

merge_665:                                        ; preds = %merge_667, %while_body_662
  %"%i.load6" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load6", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_661

merge_667:                                        ; preds = %merge_670
  br label %merge_665

merge_670:                                        ; preds = %else_668
  br label %merge_667

then_664:                                         ; preds = %while_body_662
  %"%subclass.load" = load i64, ptr %subclass2, align 8
  %cmpeqtmp5 = icmp eq i64 %"%subclass.load", 0
  br i1 %cmpeqtmp5, label %then_666, label %else_668

then_666:                                         ; preds = %then_664
  %loadtmp = load i64, ptr %"%i", align 8
  ret i64 %loadtmp

then_669:                                         ; preds = %else_668
  %loadtmp9 = load i64, ptr %"%i", align 8
  ret i64 %loadtmp9

while_body_662:                                   ; preds = %while_cond_661
  %"%i.load3" = load i64, ptr %"%i", align 8
  %calltmp = call i64 @usb_get_class(i64 %"%i.load3")
  store i64 %calltmp, ptr %"%cls", align 8
  %"%cls.load" = load i64, ptr %"%cls", align 8
  %shrtmp = lshr i64 %"%cls.load", 8
  store i64 %shrtmp, ptr %"%dev_cls", align 8
  %"%cls.load4" = load i64, ptr %"%cls", align 8
  %andtmp = and i64 %"%cls.load4", 255
  store i64 %andtmp, ptr %"%dev_sub", align 8
  %"%dev_cls.load" = load i64, ptr %"%dev_cls", align 8
  %"%class_code.load" = load i64, ptr %class_code1, align 8
  %cmpeqtmp = icmp eq i64 %"%dev_cls.load", %"%class_code.load"
  br i1 %cmpeqtmp, label %then_664, label %merge_665

while_cond_661:                                   ; preds = %merge_665, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%usb_dev_count.load" = load i64, ptr @usb_dev_count, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%usb_dev_count.load"
  br i1 %cmplttmp, label %while_body_662, label %while_exit_663

while_exit_663:                                   ; preds = %while_cond_661
  ret i64 -1
}

define i64 @usb_find_keyboard() {
entry:
  %"%proto" = alloca i64, align 8
  %"%dev_cls" = alloca i64, align 8
  %"%cls" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_671

merge_675:                                        ; preds = %merge_677, %while_body_672
  %"%i.load5" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load5", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_671

merge_677:                                        ; preds = %then_674
  br label %merge_675

then_674:                                         ; preds = %while_body_672
  %"%proto.load" = load i64, ptr %"%proto", align 8
  %"%USB_PROTOCOL_KEYBOARD.load" = load i64, ptr @USB_PROTOCOL_KEYBOARD, align 8
  %cmpeqtmp4 = icmp eq i64 %"%proto.load", %"%USB_PROTOCOL_KEYBOARD.load"
  br i1 %cmpeqtmp4, label %then_676, label %merge_677

then_676:                                         ; preds = %then_674
  %loadtmp = load i64, ptr %"%i", align 8
  ret i64 %loadtmp

while_body_672:                                   ; preds = %while_cond_671
  %"%i.load1" = load i64, ptr %"%i", align 8
  %calltmp = call i64 @usb_get_class(i64 %"%i.load1")
  store i64 %calltmp, ptr %"%cls", align 8
  %"%cls.load" = load i64, ptr %"%cls", align 8
  %shrtmp = lshr i64 %"%cls.load", 8
  store i64 %shrtmp, ptr %"%dev_cls", align 8
  %"%i.load2" = load i64, ptr %"%i", align 8
  %calltmp3 = call i64 @usb_get_proto(i64 %"%i.load2")
  store i64 %calltmp3, ptr %"%proto", align 8
  %"%dev_cls.load" = load i64, ptr %"%dev_cls", align 8
  %"%USB_CLASS_HID.load" = load i64, ptr @USB_CLASS_HID, align 8
  %cmpeqtmp = icmp eq i64 %"%dev_cls.load", %"%USB_CLASS_HID.load"
  br i1 %cmpeqtmp, label %then_674, label %merge_675

while_cond_671:                                   ; preds = %merge_675, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%usb_dev_count.load" = load i64, ptr @usb_dev_count, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%usb_dev_count.load"
  br i1 %cmplttmp, label %while_body_672, label %while_exit_673

while_exit_673:                                   ; preds = %while_cond_671
  ret i64 -1
}

define i64 @usb_find_mouse() {
entry:
  %"%proto" = alloca i64, align 8
  %"%dev_cls" = alloca i64, align 8
  %"%cls" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_678

merge_682:                                        ; preds = %merge_684, %while_body_679
  %"%i.load5" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load5", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_678

merge_684:                                        ; preds = %then_681
  br label %merge_682

then_681:                                         ; preds = %while_body_679
  %"%proto.load" = load i64, ptr %"%proto", align 8
  %"%USB_PROTOCOL_MOUSE.load" = load i64, ptr @USB_PROTOCOL_MOUSE, align 8
  %cmpeqtmp4 = icmp eq i64 %"%proto.load", %"%USB_PROTOCOL_MOUSE.load"
  br i1 %cmpeqtmp4, label %then_683, label %merge_684

then_683:                                         ; preds = %then_681
  %loadtmp = load i64, ptr %"%i", align 8
  ret i64 %loadtmp

while_body_679:                                   ; preds = %while_cond_678
  %"%i.load1" = load i64, ptr %"%i", align 8
  %calltmp = call i64 @usb_get_class(i64 %"%i.load1")
  store i64 %calltmp, ptr %"%cls", align 8
  %"%cls.load" = load i64, ptr %"%cls", align 8
  %shrtmp = lshr i64 %"%cls.load", 8
  store i64 %shrtmp, ptr %"%dev_cls", align 8
  %"%i.load2" = load i64, ptr %"%i", align 8
  %calltmp3 = call i64 @usb_get_proto(i64 %"%i.load2")
  store i64 %calltmp3, ptr %"%proto", align 8
  %"%dev_cls.load" = load i64, ptr %"%dev_cls", align 8
  %"%USB_CLASS_HID.load" = load i64, ptr @USB_CLASS_HID, align 8
  %cmpeqtmp = icmp eq i64 %"%dev_cls.load", %"%USB_CLASS_HID.load"
  br i1 %cmpeqtmp, label %then_681, label %merge_682

while_cond_678:                                   ; preds = %merge_682, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%usb_dev_count.load" = load i64, ptr @usb_dev_count, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%usb_dev_count.load"
  br i1 %cmplttmp, label %while_body_679, label %while_exit_680

while_exit_680:                                   ; preds = %while_cond_678
  ret i64 -1
}

define i64 @usb_find_storage() {
entry:
  %"%USB_CLASS_STORAGE.load" = load i64, ptr @USB_CLASS_STORAGE, align 8
  %calltmp = call i64 @usb_find_device_by_class(i64 %"%USB_CLASS_STORAGE.load", i64 0)
  ret i64 %calltmp
}

define i64 @"usb_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %"%cls" = alloca i64, align 8
  %"%speed" = alloca i64, align 8
  %"%state" = alloca i64, align 8
  %"%addr" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %calltmp = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.40 to i64))
  %calltmp1 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.41 to i64))
  %"%1954.load" = load i64, ptr @usb_hc_count, align 8
  %calltmp3 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%1954.load")
  %calltmp4 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp5 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.42 to i64))
  %"%1960.load" = load i64, ptr @usb_dev_count, align 8
  %calltmp6 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%1960.load")
  %calltmp7 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp8 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.43 to i64))
  %"%1966.load" = load i64, ptr @usb_hid_devices, align 8
  %calltmp9 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%1966.load")
  %calltmp10 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp11 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.44 to i64))
  %"%1972.load" = load i64, ptr @usb_storage_devices, align 8
  %calltmp12 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%1972.load")
  %calltmp13 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp14 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.45 to i64))
  %"%1978.load" = load i64, ptr @usb_hub_devices, align 8
  %calltmp15 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%1978.load")
  %calltmp16 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp17 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.46 to i64))
  %"%1984.load" = load i64, ptr @usb_other_devices, align 8
  %calltmp18 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%1984.load")
  %calltmp19 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp20 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.47 to i64))
  %"%1990.load" = load i64, ptr @usb_total_transfers, align 8
  %calltmp21 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%1990.load")
  %calltmp22 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp23 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.48 to i64))
  %"%1996.load" = load i64, ptr @usb_transfer_errors, align 8
  %calltmp24 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%1996.load")
  %calltmp25 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%i", align 8
  br label %while_cond_685

while_body_686:                                   ; preds = %while_cond_685
  %"%i.load26" = load i64, ptr %"%i", align 8
  %calltmp27 = call i64 @usb_get_addr(i64 %"%i.load26")
  store i64 %calltmp27, ptr %"%addr", align 8
  %"%i.load28" = load i64, ptr %"%i", align 8
  %calltmp29 = call i64 @usb_get_state(i64 %"%i.load28")
  store i64 %calltmp29, ptr %"%state", align 8
  %"%i.load30" = load i64, ptr %"%i", align 8
  %calltmp31 = call i64 @usb_get_speed(i64 %"%i.load30")
  store i64 %calltmp31, ptr %"%speed", align 8
  %"%i.load32" = load i64, ptr %"%i", align 8
  %calltmp33 = call i64 @usb_get_class(i64 %"%i.load32")
  store i64 %calltmp33, ptr %"%cls", align 8
  %calltmp34 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.49 to i64))
  %"%2018.load" = load i64, ptr %"%i", align 8
  %calltmp35 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%2018.load")
  %calltmp36 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.50 to i64))
  %"%2023.load" = load i64, ptr %"%addr", align 8
  %calltmp37 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%2023.load")
  %calltmp38 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.51 to i64))
  %"%2028.load" = load i64, ptr %"%state", align 8
  %calltmp39 = call i64 @"usb_serial_\D8\B1\D9\82\D9\85"(i64 %"%2028.load")
  %calltmp40 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.52 to i64))
  %"%2033.load" = load i64, ptr %"%cls", align 8
  %calltmp41 = call i64 @usb_serial_hex16(i64 %"%2033.load")
  %calltmp42 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  %"%i.load43" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%i.load43", 1
  store i64 %addtmp, ptr %"%i", align 8
  br label %while_cond_685

while_cond_685:                                   ; preds = %while_body_686, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%usb_dev_count.load" = load i64, ptr @usb_dev_count, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%usb_dev_count.load"
  br i1 %cmplttmp, label %while_body_686, label %while_exit_687

while_exit_687:                                   ; preds = %while_cond_685
  %calltmp44 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.53 to i64))
  %calltmp45 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @"usb_\D9\87\D9\8A\D8\A6"() {
entry:
  %"%usb_other_devices" = alloca i64, align 8
  %"%usb_hub_devices" = alloca i64, align 8
  %"%usb_storage_devices" = alloca i64, align 8
  %"%usb_hid_devices" = alloca i64, align 8
  %"%usb_transfer_errors" = alloca i64, align 8
  %"%usb_total_transfers" = alloca i64, align 8
  %"%usb_next_address" = alloca i64, align 8
  %"%usb_hc_count" = alloca i64, align 8
  %"%usb_dev_count" = alloca i64, align 8
  %calltmp = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.54 to i64))
  %calltmp1 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%usb_dev_count", align 8
  store i64 0, ptr %"%usb_hc_count", align 8
  store i64 1, ptr %"%usb_next_address", align 8
  store i64 0, ptr %"%usb_total_transfers", align 8
  store i64 0, ptr %"%usb_transfer_errors", align 8
  store i64 0, ptr %"%usb_hid_devices", align 8
  store i64 0, ptr %"%usb_storage_devices", align 8
  store i64 0, ptr %"%usb_hub_devices", align 8
  store i64 0, ptr %"%usb_other_devices", align 8
  %calltmp2 = call i64 @usb_detect_controllers()
  %calltmp3 = call i64 @"usb_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.55 to i64))
  %calltmp4 = call i64 @"usb_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr %"%usb_hc_count", align 8
  ret i64 %loadtmp
}

define i64 @"usb_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  ret i64 0
}

define void @__sad_main() {
entry:
  %"%usb_other_devices" = alloca i64, align 8
  %"%usb_hub_devices" = alloca i64, align 8
  %"%usb_storage_devices" = alloca i64, align 8
  %"%usb_hid_devices" = alloca i64, align 8
  %"%usb_transfer_errors" = alloca i64, align 8
  %"%usb_total_transfers" = alloca i64, align 8
  %"%usb_next_address" = alloca i64, align 8
  %"%hc_pci_idx_3" = alloca i64, align 8
  %"%hc_pci_idx_2" = alloca i64, align 8
  %"%hc_pci_idx_1" = alloca i64, align 8
  %"%hc_pci_idx_0" = alloca i64, align 8
  %"%hc_irq_3" = alloca i64, align 8
  %"%hc_irq_2" = alloca i64, align 8
  %"%hc_irq_1" = alloca i64, align 8
  %"%hc_irq_0" = alloca i64, align 8
  %"%hc_base_3" = alloca i64, align 8
  %"%hc_base_2" = alloca i64, align 8
  %"%hc_base_1" = alloca i64, align 8
  %"%hc_base_0" = alloca i64, align 8
  %"%hc_type_3" = alloca i64, align 8
  %"%hc_type_2" = alloca i64, align 8
  %"%hc_type_1" = alloca i64, align 8
  %"%hc_type_0" = alloca i64, align 8
  %"%usb_hc_count" = alloca i64, align 8
  %"%usb_num_ep_15" = alloca i64, align 8
  %"%usb_num_ep_14" = alloca i64, align 8
  %"%usb_num_ep_13" = alloca i64, align 8
  %"%usb_num_ep_12" = alloca i64, align 8
  %"%usb_num_ep_11" = alloca i64, align 8
  %"%usb_num_ep_10" = alloca i64, align 8
  %"%usb_num_ep_9" = alloca i64, align 8
  %"%usb_num_ep_8" = alloca i64, align 8
  %"%usb_num_ep_7" = alloca i64, align 8
  %"%usb_num_ep_6" = alloca i64, align 8
  %"%usb_num_ep_5" = alloca i64, align 8
  %"%usb_num_ep_4" = alloca i64, align 8
  %"%usb_num_ep_3" = alloca i64, align 8
  %"%usb_num_ep_2" = alloca i64, align 8
  %"%usb_num_ep_1" = alloca i64, align 8
  %"%usb_num_ep_0" = alloca i64, align 8
  %"%usb_port_15" = alloca i64, align 8
  %"%usb_port_14" = alloca i64, align 8
  %"%usb_port_13" = alloca i64, align 8
  %"%usb_port_12" = alloca i64, align 8
  %"%usb_port_11" = alloca i64, align 8
  %"%usb_port_10" = alloca i64, align 8
  %"%usb_port_9" = alloca i64, align 8
  %"%usb_port_8" = alloca i64, align 8
  %"%usb_port_7" = alloca i64, align 8
  %"%usb_port_6" = alloca i64, align 8
  %"%usb_port_5" = alloca i64, align 8
  %"%usb_port_4" = alloca i64, align 8
  %"%usb_port_3" = alloca i64, align 8
  %"%usb_port_2" = alloca i64, align 8
  %"%usb_port_1" = alloca i64, align 8
  %"%usb_port_0" = alloca i64, align 8
  %"%usb_hc_15" = alloca i64, align 8
  %"%usb_hc_14" = alloca i64, align 8
  %"%usb_hc_13" = alloca i64, align 8
  %"%usb_hc_12" = alloca i64, align 8
  %"%usb_hc_11" = alloca i64, align 8
  %"%usb_hc_10" = alloca i64, align 8
  %"%usb_hc_9" = alloca i64, align 8
  %"%usb_hc_8" = alloca i64, align 8
  %"%usb_hc_7" = alloca i64, align 8
  %"%usb_hc_6" = alloca i64, align 8
  %"%usb_hc_5" = alloca i64, align 8
  %"%usb_hc_4" = alloca i64, align 8
  %"%usb_hc_3" = alloca i64, align 8
  %"%usb_hc_2" = alloca i64, align 8
  %"%usb_hc_1" = alloca i64, align 8
  %"%usb_hc_0" = alloca i64, align 8
  %"%usb_maxpkt_15" = alloca i64, align 8
  %"%usb_maxpkt_14" = alloca i64, align 8
  %"%usb_maxpkt_13" = alloca i64, align 8
  %"%usb_maxpkt_12" = alloca i64, align 8
  %"%usb_maxpkt_11" = alloca i64, align 8
  %"%usb_maxpkt_10" = alloca i64, align 8
  %"%usb_maxpkt_9" = alloca i64, align 8
  %"%usb_maxpkt_8" = alloca i64, align 8
  %"%usb_maxpkt_7" = alloca i64, align 8
  %"%usb_maxpkt_6" = alloca i64, align 8
  %"%usb_maxpkt_5" = alloca i64, align 8
  %"%usb_maxpkt_4" = alloca i64, align 8
  %"%usb_maxpkt_3" = alloca i64, align 8
  %"%usb_maxpkt_2" = alloca i64, align 8
  %"%usb_maxpkt_1" = alloca i64, align 8
  %"%usb_maxpkt_0" = alloca i64, align 8
  %"%usb_pid_15" = alloca i64, align 8
  %"%usb_pid_14" = alloca i64, align 8
  %"%usb_pid_13" = alloca i64, align 8
  %"%usb_pid_12" = alloca i64, align 8
  %"%usb_pid_11" = alloca i64, align 8
  %"%usb_pid_10" = alloca i64, align 8
  %"%usb_pid_9" = alloca i64, align 8
  %"%usb_pid_8" = alloca i64, align 8
  %"%usb_pid_7" = alloca i64, align 8
  %"%usb_pid_6" = alloca i64, align 8
  %"%usb_pid_5" = alloca i64, align 8
  %"%usb_pid_4" = alloca i64, align 8
  %"%usb_pid_3" = alloca i64, align 8
  %"%usb_pid_2" = alloca i64, align 8
  %"%usb_pid_1" = alloca i64, align 8
  %"%usb_pid_0" = alloca i64, align 8
  %"%usb_vid_15" = alloca i64, align 8
  %"%usb_vid_14" = alloca i64, align 8
  %"%usb_vid_13" = alloca i64, align 8
  %"%usb_vid_12" = alloca i64, align 8
  %"%usb_vid_11" = alloca i64, align 8
  %"%usb_vid_10" = alloca i64, align 8
  %"%usb_vid_9" = alloca i64, align 8
  %"%usb_vid_8" = alloca i64, align 8
  %"%usb_vid_7" = alloca i64, align 8
  %"%usb_vid_6" = alloca i64, align 8
  %"%usb_vid_5" = alloca i64, align 8
  %"%usb_vid_4" = alloca i64, align 8
  %"%usb_vid_3" = alloca i64, align 8
  %"%usb_vid_2" = alloca i64, align 8
  %"%usb_vid_1" = alloca i64, align 8
  %"%usb_vid_0" = alloca i64, align 8
  %"%usb_proto_31" = alloca i64, align 8
  %"%usb_proto_30" = alloca i64, align 8
  %"%usb_proto_29" = alloca i64, align 8
  %"%usb_proto_28" = alloca i64, align 8
  %"%usb_proto_27" = alloca i64, align 8
  %"%usb_proto_26" = alloca i64, align 8
  %"%usb_proto_25" = alloca i64, align 8
  %"%usb_proto_24" = alloca i64, align 8
  %"%usb_proto_23" = alloca i64, align 8
  %"%usb_proto_22" = alloca i64, align 8
  %"%usb_proto_21" = alloca i64, align 8
  %"%usb_proto_20" = alloca i64, align 8
  %"%usb_proto_19" = alloca i64, align 8
  %"%usb_proto_18" = alloca i64, align 8
  %"%usb_proto_17" = alloca i64, align 8
  %"%usb_proto_16" = alloca i64, align 8
  %"%usb_proto_15" = alloca i64, align 8
  %"%usb_proto_14" = alloca i64, align 8
  %"%usb_proto_13" = alloca i64, align 8
  %"%usb_proto_12" = alloca i64, align 8
  %"%usb_proto_11" = alloca i64, align 8
  %"%usb_proto_10" = alloca i64, align 8
  %"%usb_proto_9" = alloca i64, align 8
  %"%usb_proto_8" = alloca i64, align 8
  %"%usb_proto_7" = alloca i64, align 8
  %"%usb_proto_6" = alloca i64, align 8
  %"%usb_proto_5" = alloca i64, align 8
  %"%usb_proto_4" = alloca i64, align 8
  %"%usb_proto_3" = alloca i64, align 8
  %"%usb_proto_2" = alloca i64, align 8
  %"%usb_proto_1" = alloca i64, align 8
  %"%usb_proto_0" = alloca i64, align 8
  %"%usb_class_31" = alloca i64, align 8
  %"%usb_class_30" = alloca i64, align 8
  %"%usb_class_29" = alloca i64, align 8
  %"%usb_class_28" = alloca i64, align 8
  %"%usb_class_27" = alloca i64, align 8
  %"%usb_class_26" = alloca i64, align 8
  %"%usb_class_25" = alloca i64, align 8
  %"%usb_class_24" = alloca i64, align 8
  %"%usb_class_23" = alloca i64, align 8
  %"%usb_class_22" = alloca i64, align 8
  %"%usb_class_21" = alloca i64, align 8
  %"%usb_class_20" = alloca i64, align 8
  %"%usb_class_19" = alloca i64, align 8
  %"%usb_class_18" = alloca i64, align 8
  %"%usb_class_17" = alloca i64, align 8
  %"%usb_class_16" = alloca i64, align 8
  %"%usb_class_15" = alloca i64, align 8
  %"%usb_class_14" = alloca i64, align 8
  %"%usb_class_13" = alloca i64, align 8
  %"%usb_class_12" = alloca i64, align 8
  %"%usb_class_11" = alloca i64, align 8
  %"%usb_class_10" = alloca i64, align 8
  %"%usb_class_9" = alloca i64, align 8
  %"%usb_class_8" = alloca i64, align 8
  %"%usb_class_7" = alloca i64, align 8
  %"%usb_class_6" = alloca i64, align 8
  %"%usb_class_5" = alloca i64, align 8
  %"%usb_class_4" = alloca i64, align 8
  %"%usb_class_3" = alloca i64, align 8
  %"%usb_class_2" = alloca i64, align 8
  %"%usb_class_1" = alloca i64, align 8
  %"%usb_class_0" = alloca i64, align 8
  %"%usb_speed_31" = alloca i64, align 8
  %"%usb_speed_30" = alloca i64, align 8
  %"%usb_speed_29" = alloca i64, align 8
  %"%usb_speed_28" = alloca i64, align 8
  %"%usb_speed_27" = alloca i64, align 8
  %"%usb_speed_26" = alloca i64, align 8
  %"%usb_speed_25" = alloca i64, align 8
  %"%usb_speed_24" = alloca i64, align 8
  %"%usb_speed_23" = alloca i64, align 8
  %"%usb_speed_22" = alloca i64, align 8
  %"%usb_speed_21" = alloca i64, align 8
  %"%usb_speed_20" = alloca i64, align 8
  %"%usb_speed_19" = alloca i64, align 8
  %"%usb_speed_18" = alloca i64, align 8
  %"%usb_speed_17" = alloca i64, align 8
  %"%usb_speed_16" = alloca i64, align 8
  %"%usb_speed_15" = alloca i64, align 8
  %"%usb_speed_14" = alloca i64, align 8
  %"%usb_speed_13" = alloca i64, align 8
  %"%usb_speed_12" = alloca i64, align 8
  %"%usb_speed_11" = alloca i64, align 8
  %"%usb_speed_10" = alloca i64, align 8
  %"%usb_speed_9" = alloca i64, align 8
  %"%usb_speed_8" = alloca i64, align 8
  %"%usb_speed_7" = alloca i64, align 8
  %"%usb_speed_6" = alloca i64, align 8
  %"%usb_speed_5" = alloca i64, align 8
  %"%usb_speed_4" = alloca i64, align 8
  %"%usb_speed_3" = alloca i64, align 8
  %"%usb_speed_2" = alloca i64, align 8
  %"%usb_speed_1" = alloca i64, align 8
  %"%usb_speed_0" = alloca i64, align 8
  %"%usb_state_31" = alloca i64, align 8
  %"%usb_state_30" = alloca i64, align 8
  %"%usb_state_29" = alloca i64, align 8
  %"%usb_state_28" = alloca i64, align 8
  %"%usb_state_27" = alloca i64, align 8
  %"%usb_state_26" = alloca i64, align 8
  %"%usb_state_25" = alloca i64, align 8
  %"%usb_state_24" = alloca i64, align 8
  %"%usb_state_23" = alloca i64, align 8
  %"%usb_state_22" = alloca i64, align 8
  %"%usb_state_21" = alloca i64, align 8
  %"%usb_state_20" = alloca i64, align 8
  %"%usb_state_19" = alloca i64, align 8
  %"%usb_state_18" = alloca i64, align 8
  %"%usb_state_17" = alloca i64, align 8
  %"%usb_state_16" = alloca i64, align 8
  %"%usb_state_15" = alloca i64, align 8
  %"%usb_state_14" = alloca i64, align 8
  %"%usb_state_13" = alloca i64, align 8
  %"%usb_state_12" = alloca i64, align 8
  %"%usb_state_11" = alloca i64, align 8
  %"%usb_state_10" = alloca i64, align 8
  %"%usb_state_9" = alloca i64, align 8
  %"%usb_state_8" = alloca i64, align 8
  %"%usb_state_7" = alloca i64, align 8
  %"%usb_state_6" = alloca i64, align 8
  %"%usb_state_5" = alloca i64, align 8
  %"%usb_state_4" = alloca i64, align 8
  %"%usb_state_3" = alloca i64, align 8
  %"%usb_state_2" = alloca i64, align 8
  %"%usb_state_1" = alloca i64, align 8
  %"%usb_state_0" = alloca i64, align 8
  %"%usb_addr_31" = alloca i64, align 8
  %"%usb_addr_30" = alloca i64, align 8
  %"%usb_addr_29" = alloca i64, align 8
  %"%usb_addr_28" = alloca i64, align 8
  %"%usb_addr_27" = alloca i64, align 8
  %"%usb_addr_26" = alloca i64, align 8
  %"%usb_addr_25" = alloca i64, align 8
  %"%usb_addr_24" = alloca i64, align 8
  %"%usb_addr_23" = alloca i64, align 8
  %"%usb_addr_22" = alloca i64, align 8
  %"%usb_addr_21" = alloca i64, align 8
  %"%usb_addr_20" = alloca i64, align 8
  %"%usb_addr_19" = alloca i64, align 8
  %"%usb_addr_18" = alloca i64, align 8
  %"%usb_addr_17" = alloca i64, align 8
  %"%usb_addr_16" = alloca i64, align 8
  %"%usb_addr_15" = alloca i64, align 8
  %"%usb_addr_14" = alloca i64, align 8
  %"%usb_addr_13" = alloca i64, align 8
  %"%usb_addr_12" = alloca i64, align 8
  %"%usb_addr_11" = alloca i64, align 8
  %"%usb_addr_10" = alloca i64, align 8
  %"%usb_addr_9" = alloca i64, align 8
  %"%usb_addr_8" = alloca i64, align 8
  %"%usb_addr_7" = alloca i64, align 8
  %"%usb_addr_6" = alloca i64, align 8
  %"%usb_addr_5" = alloca i64, align 8
  %"%usb_addr_4" = alloca i64, align 8
  %"%usb_addr_3" = alloca i64, align 8
  %"%usb_addr_2" = alloca i64, align 8
  %"%usb_addr_1" = alloca i64, align 8
  %"%usb_addr_0" = alloca i64, align 8
  %"%usb_dev_count" = alloca i64, align 8
  %"%USB_MAX_CONTROLLERS" = alloca i64, align 8
  %"%USB_MAX_ENDPOINTS" = alloca i64, align 8
  %"%USB_MAX_DEVICES" = alloca i64, align 8
  %"%USB_HC_XHCI" = alloca i64, align 8
  %"%USB_HC_EHCI" = alloca i64, align 8
  %"%USB_HC_OHCI" = alloca i64, align 8
  %"%USB_HC_UHCI" = alloca i64, align 8
  %"%USB_DEV_CONFIGURED" = alloca i64, align 8
  %"%USB_DEV_ADDRESSED" = alloca i64, align 8
  %"%USB_DEV_DEFAULT" = alloca i64, align 8
  %"%USB_DEV_POWERED" = alloca i64, align 8
  %"%USB_DEV_ATTACHED" = alloca i64, align 8
  %"%USB_DEV_DISCONNECTED" = alloca i64, align 8
  %"%USB_SPEED_HIGH" = alloca i64, align 8
  %"%USB_SPEED_FULL" = alloca i64, align 8
  %"%USB_SPEED_LOW" = alloca i64, align 8
  %"%USB_EP_DIR_IN" = alloca i64, align 8
  %"%USB_EP_DIR_OUT" = alloca i64, align 8
  %"%USB_EP_INTERRUPT" = alloca i64, align 8
  %"%USB_EP_BULK" = alloca i64, align 8
  %"%USB_EP_ISOCHRONOUS" = alloca i64, align 8
  %"%USB_EP_CONTROL" = alloca i64, align 8
  %"%USB_PROTOCOL_BBB" = alloca i64, align 8
  %"%USB_SUBCLASS_SCSI" = alloca i64, align 8
  %"%USB_PROTOCOL_MOUSE" = alloca i64, align 8
  %"%USB_PROTOCOL_KEYBOARD" = alloca i64, align 8
  %"%USB_SUBCLASS_BOOT" = alloca i64, align 8
  %"%USB_CLASS_VENDOR" = alloca i64, align 8
  %"%USB_CLASS_WIRELESS" = alloca i64, align 8
  %"%USB_CLASS_PRINTER" = alloca i64, align 8
  %"%USB_CLASS_CDC" = alloca i64, align 8
  %"%USB_CLASS_VIDEO" = alloca i64, align 8
  %"%USB_CLASS_AUDIO" = alloca i64, align 8
  %"%USB_CLASS_STORAGE" = alloca i64, align 8
  %"%USB_CLASS_HID" = alloca i64, align 8
  %"%USB_CLASS_HUB" = alloca i64, align 8
  %"%USB_DESC_HUB" = alloca i64, align 8
  %"%USB_DESC_REPORT" = alloca i64, align 8
  %"%USB_DESC_HID" = alloca i64, align 8
  %"%USB_DESC_ENDPOINT" = alloca i64, align 8
  %"%USB_DESC_INTERFACE" = alloca i64, align 8
  %"%USB_DESC_STRING" = alloca i64, align 8
  %"%USB_DESC_CONFIG" = alloca i64, align 8
  %"%USB_DESC_DEVICE" = alloca i64, align 8
  %"%USB_REQ_SET_INTERFACE" = alloca i64, align 8
  %"%USB_REQ_GET_INTERFACE" = alloca i64, align 8
  %"%USB_REQ_SET_CONFIG" = alloca i64, align 8
  %"%USB_REQ_GET_CONFIG" = alloca i64, align 8
  %"%USB_REQ_SET_DESCRIPTOR" = alloca i64, align 8
  %"%USB_REQ_GET_DESCRIPTOR" = alloca i64, align 8
  %"%USB_REQ_SET_ADDRESS" = alloca i64, align 8
  %"%USB_REQ_SET_FEATURE" = alloca i64, align 8
  %"%USB_REQ_CLEAR_FEATURE" = alloca i64, align 8
  %"%USB_REQ_GET_STATUS" = alloca i64, align 8
  %"%USB_REQ_RECIP_EP" = alloca i64, align 8
  %"%USB_REQ_RECIP_IFACE" = alloca i64, align 8
  %"%USB_REQ_RECIP_DEV" = alloca i64, align 8
  %"%USB_REQ_TYPE_VENDOR" = alloca i64, align 8
  %"%USB_REQ_TYPE_CLASS" = alloca i64, align 8
  %"%USB_REQ_TYPE_STD" = alloca i64, align 8
  %"%USB_REQ_DEV_TO_HOST" = alloca i64, align 8
  %"%USB_REQ_HOST_TO_DEV" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 0, ptr %"%USB_REQ_HOST_TO_DEV", align 8
  store i64 128, ptr %"%USB_REQ_DEV_TO_HOST", align 8
  store i64 0, ptr %"%USB_REQ_TYPE_STD", align 8
  store i64 32, ptr %"%USB_REQ_TYPE_CLASS", align 8
  store i64 64, ptr %"%USB_REQ_TYPE_VENDOR", align 8
  store i64 0, ptr %"%USB_REQ_RECIP_DEV", align 8
  store i64 1, ptr %"%USB_REQ_RECIP_IFACE", align 8
  store i64 2, ptr %"%USB_REQ_RECIP_EP", align 8
  store i64 0, ptr %"%USB_REQ_GET_STATUS", align 8
  store i64 1, ptr %"%USB_REQ_CLEAR_FEATURE", align 8
  store i64 3, ptr %"%USB_REQ_SET_FEATURE", align 8
  store i64 5, ptr %"%USB_REQ_SET_ADDRESS", align 8
  store i64 6, ptr %"%USB_REQ_GET_DESCRIPTOR", align 8
  store i64 7, ptr %"%USB_REQ_SET_DESCRIPTOR", align 8
  store i64 8, ptr %"%USB_REQ_GET_CONFIG", align 8
  store i64 9, ptr %"%USB_REQ_SET_CONFIG", align 8
  store i64 10, ptr %"%USB_REQ_GET_INTERFACE", align 8
  store i64 11, ptr %"%USB_REQ_SET_INTERFACE", align 8
  store i64 1, ptr %"%USB_DESC_DEVICE", align 8
  store i64 2, ptr %"%USB_DESC_CONFIG", align 8
  store i64 3, ptr %"%USB_DESC_STRING", align 8
  store i64 4, ptr %"%USB_DESC_INTERFACE", align 8
  store i64 5, ptr %"%USB_DESC_ENDPOINT", align 8
  store i64 33, ptr %"%USB_DESC_HID", align 8
  store i64 34, ptr %"%USB_DESC_REPORT", align 8
  store i64 41, ptr %"%USB_DESC_HUB", align 8
  store i64 9, ptr %"%USB_CLASS_HUB", align 8
  store i64 3, ptr %"%USB_CLASS_HID", align 8
  store i64 8, ptr %"%USB_CLASS_STORAGE", align 8
  store i64 1, ptr %"%USB_CLASS_AUDIO", align 8
  store i64 14, ptr %"%USB_CLASS_VIDEO", align 8
  store i64 2, ptr %"%USB_CLASS_CDC", align 8
  store i64 7, ptr %"%USB_CLASS_PRINTER", align 8
  store i64 224, ptr %"%USB_CLASS_WIRELESS", align 8
  store i64 255, ptr %"%USB_CLASS_VENDOR", align 8
  store i64 1, ptr %"%USB_SUBCLASS_BOOT", align 8
  store i64 1, ptr %"%USB_PROTOCOL_KEYBOARD", align 8
  store i64 2, ptr %"%USB_PROTOCOL_MOUSE", align 8
  store i64 6, ptr %"%USB_SUBCLASS_SCSI", align 8
  store i64 80, ptr %"%USB_PROTOCOL_BBB", align 8
  store i64 0, ptr %"%USB_EP_CONTROL", align 8
  store i64 1, ptr %"%USB_EP_ISOCHRONOUS", align 8
  store i64 2, ptr %"%USB_EP_BULK", align 8
  store i64 3, ptr %"%USB_EP_INTERRUPT", align 8
  store i64 0, ptr %"%USB_EP_DIR_OUT", align 8
  store i64 128, ptr %"%USB_EP_DIR_IN", align 8
  store i64 0, ptr %"%USB_SPEED_LOW", align 8
  store i64 1, ptr %"%USB_SPEED_FULL", align 8
  store i64 2, ptr %"%USB_SPEED_HIGH", align 8
  store i64 0, ptr %"%USB_DEV_DISCONNECTED", align 8
  store i64 1, ptr %"%USB_DEV_ATTACHED", align 8
  store i64 2, ptr %"%USB_DEV_POWERED", align 8
  store i64 3, ptr %"%USB_DEV_DEFAULT", align 8
  store i64 4, ptr %"%USB_DEV_ADDRESSED", align 8
  store i64 5, ptr %"%USB_DEV_CONFIGURED", align 8
  store i64 0, ptr %"%USB_HC_UHCI", align 8
  store i64 16, ptr %"%USB_HC_OHCI", align 8
  store i64 32, ptr %"%USB_HC_EHCI", align 8
  store i64 48, ptr %"%USB_HC_XHCI", align 8
  store i64 32, ptr %"%USB_MAX_DEVICES", align 8
  store i64 8, ptr %"%USB_MAX_ENDPOINTS", align 8
  store i64 4, ptr %"%USB_MAX_CONTROLLERS", align 8
  store i64 0, ptr %"%usb_dev_count", align 8
  store i64 0, ptr %"%usb_addr_0", align 8
  store i64 0, ptr %"%usb_addr_1", align 8
  store i64 0, ptr %"%usb_addr_2", align 8
  store i64 0, ptr %"%usb_addr_3", align 8
  store i64 0, ptr %"%usb_addr_4", align 8
  store i64 0, ptr %"%usb_addr_5", align 8
  store i64 0, ptr %"%usb_addr_6", align 8
  store i64 0, ptr %"%usb_addr_7", align 8
  store i64 0, ptr %"%usb_addr_8", align 8
  store i64 0, ptr %"%usb_addr_9", align 8
  store i64 0, ptr %"%usb_addr_10", align 8
  store i64 0, ptr %"%usb_addr_11", align 8
  store i64 0, ptr %"%usb_addr_12", align 8
  store i64 0, ptr %"%usb_addr_13", align 8
  store i64 0, ptr %"%usb_addr_14", align 8
  store i64 0, ptr %"%usb_addr_15", align 8
  store i64 0, ptr %"%usb_addr_16", align 8
  store i64 0, ptr %"%usb_addr_17", align 8
  store i64 0, ptr %"%usb_addr_18", align 8
  store i64 0, ptr %"%usb_addr_19", align 8
  store i64 0, ptr %"%usb_addr_20", align 8
  store i64 0, ptr %"%usb_addr_21", align 8
  store i64 0, ptr %"%usb_addr_22", align 8
  store i64 0, ptr %"%usb_addr_23", align 8
  store i64 0, ptr %"%usb_addr_24", align 8
  store i64 0, ptr %"%usb_addr_25", align 8
  store i64 0, ptr %"%usb_addr_26", align 8
  store i64 0, ptr %"%usb_addr_27", align 8
  store i64 0, ptr %"%usb_addr_28", align 8
  store i64 0, ptr %"%usb_addr_29", align 8
  store i64 0, ptr %"%usb_addr_30", align 8
  store i64 0, ptr %"%usb_addr_31", align 8
  store i64 0, ptr %"%usb_state_0", align 8
  store i64 0, ptr %"%usb_state_1", align 8
  store i64 0, ptr %"%usb_state_2", align 8
  store i64 0, ptr %"%usb_state_3", align 8
  store i64 0, ptr %"%usb_state_4", align 8
  store i64 0, ptr %"%usb_state_5", align 8
  store i64 0, ptr %"%usb_state_6", align 8
  store i64 0, ptr %"%usb_state_7", align 8
  store i64 0, ptr %"%usb_state_8", align 8
  store i64 0, ptr %"%usb_state_9", align 8
  store i64 0, ptr %"%usb_state_10", align 8
  store i64 0, ptr %"%usb_state_11", align 8
  store i64 0, ptr %"%usb_state_12", align 8
  store i64 0, ptr %"%usb_state_13", align 8
  store i64 0, ptr %"%usb_state_14", align 8
  store i64 0, ptr %"%usb_state_15", align 8
  store i64 0, ptr %"%usb_state_16", align 8
  store i64 0, ptr %"%usb_state_17", align 8
  store i64 0, ptr %"%usb_state_18", align 8
  store i64 0, ptr %"%usb_state_19", align 8
  store i64 0, ptr %"%usb_state_20", align 8
  store i64 0, ptr %"%usb_state_21", align 8
  store i64 0, ptr %"%usb_state_22", align 8
  store i64 0, ptr %"%usb_state_23", align 8
  store i64 0, ptr %"%usb_state_24", align 8
  store i64 0, ptr %"%usb_state_25", align 8
  store i64 0, ptr %"%usb_state_26", align 8
  store i64 0, ptr %"%usb_state_27", align 8
  store i64 0, ptr %"%usb_state_28", align 8
  store i64 0, ptr %"%usb_state_29", align 8
  store i64 0, ptr %"%usb_state_30", align 8
  store i64 0, ptr %"%usb_state_31", align 8
  store i64 0, ptr %"%usb_speed_0", align 8
  store i64 0, ptr %"%usb_speed_1", align 8
  store i64 0, ptr %"%usb_speed_2", align 8
  store i64 0, ptr %"%usb_speed_3", align 8
  store i64 0, ptr %"%usb_speed_4", align 8
  store i64 0, ptr %"%usb_speed_5", align 8
  store i64 0, ptr %"%usb_speed_6", align 8
  store i64 0, ptr %"%usb_speed_7", align 8
  store i64 0, ptr %"%usb_speed_8", align 8
  store i64 0, ptr %"%usb_speed_9", align 8
  store i64 0, ptr %"%usb_speed_10", align 8
  store i64 0, ptr %"%usb_speed_11", align 8
  store i64 0, ptr %"%usb_speed_12", align 8
  store i64 0, ptr %"%usb_speed_13", align 8
  store i64 0, ptr %"%usb_speed_14", align 8
  store i64 0, ptr %"%usb_speed_15", align 8
  store i64 0, ptr %"%usb_speed_16", align 8
  store i64 0, ptr %"%usb_speed_17", align 8
  store i64 0, ptr %"%usb_speed_18", align 8
  store i64 0, ptr %"%usb_speed_19", align 8
  store i64 0, ptr %"%usb_speed_20", align 8
  store i64 0, ptr %"%usb_speed_21", align 8
  store i64 0, ptr %"%usb_speed_22", align 8
  store i64 0, ptr %"%usb_speed_23", align 8
  store i64 0, ptr %"%usb_speed_24", align 8
  store i64 0, ptr %"%usb_speed_25", align 8
  store i64 0, ptr %"%usb_speed_26", align 8
  store i64 0, ptr %"%usb_speed_27", align 8
  store i64 0, ptr %"%usb_speed_28", align 8
  store i64 0, ptr %"%usb_speed_29", align 8
  store i64 0, ptr %"%usb_speed_30", align 8
  store i64 0, ptr %"%usb_speed_31", align 8
  store i64 0, ptr %"%usb_class_0", align 8
  store i64 0, ptr %"%usb_class_1", align 8
  store i64 0, ptr %"%usb_class_2", align 8
  store i64 0, ptr %"%usb_class_3", align 8
  store i64 0, ptr %"%usb_class_4", align 8
  store i64 0, ptr %"%usb_class_5", align 8
  store i64 0, ptr %"%usb_class_6", align 8
  store i64 0, ptr %"%usb_class_7", align 8
  store i64 0, ptr %"%usb_class_8", align 8
  store i64 0, ptr %"%usb_class_9", align 8
  store i64 0, ptr %"%usb_class_10", align 8
  store i64 0, ptr %"%usb_class_11", align 8
  store i64 0, ptr %"%usb_class_12", align 8
  store i64 0, ptr %"%usb_class_13", align 8
  store i64 0, ptr %"%usb_class_14", align 8
  store i64 0, ptr %"%usb_class_15", align 8
  store i64 0, ptr %"%usb_class_16", align 8
  store i64 0, ptr %"%usb_class_17", align 8
  store i64 0, ptr %"%usb_class_18", align 8
  store i64 0, ptr %"%usb_class_19", align 8
  store i64 0, ptr %"%usb_class_20", align 8
  store i64 0, ptr %"%usb_class_21", align 8
  store i64 0, ptr %"%usb_class_22", align 8
  store i64 0, ptr %"%usb_class_23", align 8
  store i64 0, ptr %"%usb_class_24", align 8
  store i64 0, ptr %"%usb_class_25", align 8
  store i64 0, ptr %"%usb_class_26", align 8
  store i64 0, ptr %"%usb_class_27", align 8
  store i64 0, ptr %"%usb_class_28", align 8
  store i64 0, ptr %"%usb_class_29", align 8
  store i64 0, ptr %"%usb_class_30", align 8
  store i64 0, ptr %"%usb_class_31", align 8
  store i64 0, ptr %"%usb_proto_0", align 8
  store i64 0, ptr %"%usb_proto_1", align 8
  store i64 0, ptr %"%usb_proto_2", align 8
  store i64 0, ptr %"%usb_proto_3", align 8
  store i64 0, ptr %"%usb_proto_4", align 8
  store i64 0, ptr %"%usb_proto_5", align 8
  store i64 0, ptr %"%usb_proto_6", align 8
  store i64 0, ptr %"%usb_proto_7", align 8
  store i64 0, ptr %"%usb_proto_8", align 8
  store i64 0, ptr %"%usb_proto_9", align 8
  store i64 0, ptr %"%usb_proto_10", align 8
  store i64 0, ptr %"%usb_proto_11", align 8
  store i64 0, ptr %"%usb_proto_12", align 8
  store i64 0, ptr %"%usb_proto_13", align 8
  store i64 0, ptr %"%usb_proto_14", align 8
  store i64 0, ptr %"%usb_proto_15", align 8
  store i64 0, ptr %"%usb_proto_16", align 8
  store i64 0, ptr %"%usb_proto_17", align 8
  store i64 0, ptr %"%usb_proto_18", align 8
  store i64 0, ptr %"%usb_proto_19", align 8
  store i64 0, ptr %"%usb_proto_20", align 8
  store i64 0, ptr %"%usb_proto_21", align 8
  store i64 0, ptr %"%usb_proto_22", align 8
  store i64 0, ptr %"%usb_proto_23", align 8
  store i64 0, ptr %"%usb_proto_24", align 8
  store i64 0, ptr %"%usb_proto_25", align 8
  store i64 0, ptr %"%usb_proto_26", align 8
  store i64 0, ptr %"%usb_proto_27", align 8
  store i64 0, ptr %"%usb_proto_28", align 8
  store i64 0, ptr %"%usb_proto_29", align 8
  store i64 0, ptr %"%usb_proto_30", align 8
  store i64 0, ptr %"%usb_proto_31", align 8
  store i64 0, ptr %"%usb_vid_0", align 8
  store i64 0, ptr %"%usb_vid_1", align 8
  store i64 0, ptr %"%usb_vid_2", align 8
  store i64 0, ptr %"%usb_vid_3", align 8
  store i64 0, ptr %"%usb_vid_4", align 8
  store i64 0, ptr %"%usb_vid_5", align 8
  store i64 0, ptr %"%usb_vid_6", align 8
  store i64 0, ptr %"%usb_vid_7", align 8
  store i64 0, ptr %"%usb_vid_8", align 8
  store i64 0, ptr %"%usb_vid_9", align 8
  store i64 0, ptr %"%usb_vid_10", align 8
  store i64 0, ptr %"%usb_vid_11", align 8
  store i64 0, ptr %"%usb_vid_12", align 8
  store i64 0, ptr %"%usb_vid_13", align 8
  store i64 0, ptr %"%usb_vid_14", align 8
  store i64 0, ptr %"%usb_vid_15", align 8
  store i64 0, ptr %"%usb_pid_0", align 8
  store i64 0, ptr %"%usb_pid_1", align 8
  store i64 0, ptr %"%usb_pid_2", align 8
  store i64 0, ptr %"%usb_pid_3", align 8
  store i64 0, ptr %"%usb_pid_4", align 8
  store i64 0, ptr %"%usb_pid_5", align 8
  store i64 0, ptr %"%usb_pid_6", align 8
  store i64 0, ptr %"%usb_pid_7", align 8
  store i64 0, ptr %"%usb_pid_8", align 8
  store i64 0, ptr %"%usb_pid_9", align 8
  store i64 0, ptr %"%usb_pid_10", align 8
  store i64 0, ptr %"%usb_pid_11", align 8
  store i64 0, ptr %"%usb_pid_12", align 8
  store i64 0, ptr %"%usb_pid_13", align 8
  store i64 0, ptr %"%usb_pid_14", align 8
  store i64 0, ptr %"%usb_pid_15", align 8
  store i64 8, ptr %"%usb_maxpkt_0", align 8
  store i64 8, ptr %"%usb_maxpkt_1", align 8
  store i64 8, ptr %"%usb_maxpkt_2", align 8
  store i64 8, ptr %"%usb_maxpkt_3", align 8
  store i64 8, ptr %"%usb_maxpkt_4", align 8
  store i64 8, ptr %"%usb_maxpkt_5", align 8
  store i64 8, ptr %"%usb_maxpkt_6", align 8
  store i64 8, ptr %"%usb_maxpkt_7", align 8
  store i64 8, ptr %"%usb_maxpkt_8", align 8
  store i64 8, ptr %"%usb_maxpkt_9", align 8
  store i64 8, ptr %"%usb_maxpkt_10", align 8
  store i64 8, ptr %"%usb_maxpkt_11", align 8
  store i64 8, ptr %"%usb_maxpkt_12", align 8
  store i64 8, ptr %"%usb_maxpkt_13", align 8
  store i64 8, ptr %"%usb_maxpkt_14", align 8
  store i64 8, ptr %"%usb_maxpkt_15", align 8
  store i64 0, ptr %"%usb_hc_0", align 8
  store i64 0, ptr %"%usb_hc_1", align 8
  store i64 0, ptr %"%usb_hc_2", align 8
  store i64 0, ptr %"%usb_hc_3", align 8
  store i64 0, ptr %"%usb_hc_4", align 8
  store i64 0, ptr %"%usb_hc_5", align 8
  store i64 0, ptr %"%usb_hc_6", align 8
  store i64 0, ptr %"%usb_hc_7", align 8
  store i64 0, ptr %"%usb_hc_8", align 8
  store i64 0, ptr %"%usb_hc_9", align 8
  store i64 0, ptr %"%usb_hc_10", align 8
  store i64 0, ptr %"%usb_hc_11", align 8
  store i64 0, ptr %"%usb_hc_12", align 8
  store i64 0, ptr %"%usb_hc_13", align 8
  store i64 0, ptr %"%usb_hc_14", align 8
  store i64 0, ptr %"%usb_hc_15", align 8
  store i64 0, ptr %"%usb_port_0", align 8
  store i64 0, ptr %"%usb_port_1", align 8
  store i64 0, ptr %"%usb_port_2", align 8
  store i64 0, ptr %"%usb_port_3", align 8
  store i64 0, ptr %"%usb_port_4", align 8
  store i64 0, ptr %"%usb_port_5", align 8
  store i64 0, ptr %"%usb_port_6", align 8
  store i64 0, ptr %"%usb_port_7", align 8
  store i64 0, ptr %"%usb_port_8", align 8
  store i64 0, ptr %"%usb_port_9", align 8
  store i64 0, ptr %"%usb_port_10", align 8
  store i64 0, ptr %"%usb_port_11", align 8
  store i64 0, ptr %"%usb_port_12", align 8
  store i64 0, ptr %"%usb_port_13", align 8
  store i64 0, ptr %"%usb_port_14", align 8
  store i64 0, ptr %"%usb_port_15", align 8
  store i64 0, ptr %"%usb_num_ep_0", align 8
  store i64 0, ptr %"%usb_num_ep_1", align 8
  store i64 0, ptr %"%usb_num_ep_2", align 8
  store i64 0, ptr %"%usb_num_ep_3", align 8
  store i64 0, ptr %"%usb_num_ep_4", align 8
  store i64 0, ptr %"%usb_num_ep_5", align 8
  store i64 0, ptr %"%usb_num_ep_6", align 8
  store i64 0, ptr %"%usb_num_ep_7", align 8
  store i64 0, ptr %"%usb_num_ep_8", align 8
  store i64 0, ptr %"%usb_num_ep_9", align 8
  store i64 0, ptr %"%usb_num_ep_10", align 8
  store i64 0, ptr %"%usb_num_ep_11", align 8
  store i64 0, ptr %"%usb_num_ep_12", align 8
  store i64 0, ptr %"%usb_num_ep_13", align 8
  store i64 0, ptr %"%usb_num_ep_14", align 8
  store i64 0, ptr %"%usb_num_ep_15", align 8
  store i64 0, ptr %"%usb_hc_count", align 8
  store i64 0, ptr %"%hc_type_0", align 8
  store i64 0, ptr %"%hc_type_1", align 8
  store i64 0, ptr %"%hc_type_2", align 8
  store i64 0, ptr %"%hc_type_3", align 8
  store i64 0, ptr %"%hc_base_0", align 8
  store i64 0, ptr %"%hc_base_1", align 8
  store i64 0, ptr %"%hc_base_2", align 8
  store i64 0, ptr %"%hc_base_3", align 8
  store i64 0, ptr %"%hc_irq_0", align 8
  store i64 0, ptr %"%hc_irq_1", align 8
  store i64 0, ptr %"%hc_irq_2", align 8
  store i64 0, ptr %"%hc_irq_3", align 8
  store i64 0, ptr %"%hc_pci_idx_0", align 8
  store i64 0, ptr %"%hc_pci_idx_1", align 8
  store i64 0, ptr %"%hc_pci_idx_2", align 8
  store i64 0, ptr %"%hc_pci_idx_3", align 8
  store i64 1, ptr %"%usb_next_address", align 8
  store i64 0, ptr %"%usb_total_transfers", align 8
  store i64 0, ptr %"%usb_transfer_errors", align 8
  store i64 0, ptr %"%usb_hid_devices", align 8
  store i64 0, ptr %"%usb_storage_devices", align 8
  store i64 0, ptr %"%usb_hub_devices", align 8
  store i64 0, ptr %"%usb_other_devices", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
