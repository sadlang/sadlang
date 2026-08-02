// ======================================================================
// sir_types.cpp - تنفيذ أنواع SIR
//                SIR Types Implementation
// ======================================================================
// الوصف بالعربية:
//   تنفيذ الدوال المساعدة لأنواع SIR
//
// English Description:
//   Implementation of SIR types helper functions
//
// الإصدار / Version: 1.0
// التاريخ / Date: December 2, 2025
// ======================================================================

#include "frontend/sir_types.h"
#include <sstream>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ======================================================================
            // تحويل SIROpcode إلى نص / Convert SIROpcode to string
            // ======================================================================

            const char *sirOpcodeToString(SIROpcode opcode)
            {
                switch (opcode)
                {
                // Arithmetic
                case SIROpcode::ADD_I64:
                    return "add.i64";
                case SIROpcode::ADD_F64:
                    return "add.f64";
                case SIROpcode::SUB_I64:
                    return "sub.i64";
                case SIROpcode::SUB_F64:
                    return "sub.f64";
                case SIROpcode::MUL_I64:
                    return "mul.i64";
                case SIROpcode::MUL_F64:
                    return "mul.f64";
                case SIROpcode::DIV_I64:
                    return "div.i64";
                case SIROpcode::DIV_F64:
                    return "div.f64";
                case SIROpcode::FLOOR_DIV_I64:
                    return "floordiv.i64";
                case SIROpcode::MOD_I64:
                    return "mod.i64";
                case SIROpcode::NEG:
                    return "neg";
                case SIROpcode::TRUNCATE_U8:
                    return "trunc.u8";
                case SIROpcode::BOX_DYN:
                    return "box.dyn";
                case SIROpcode::NULL_ASSERT:
                    return "null.assert";

                // Bitwise
                case SIROpcode::AND:
                    return "and";
                case SIROpcode::OR:
                    return "or";
                case SIROpcode::XOR:
                    return "xor";
                case SIROpcode::NOT:
                    return "not";
                case SIROpcode::SHL:
                    return "shl";
                case SIROpcode::SHR:
                    return "shr";
                case SIROpcode::SAR:
                    return "sar";
                case SIROpcode::ROL:
                    return "rol";

                // Comparison
                case SIROpcode::EQ:
                    return "eq";
                case SIROpcode::NE:
                    return "ne";
                case SIROpcode::LT:
                    return "lt";
                case SIROpcode::LE:
                    return "le";
                case SIROpcode::GT:
                    return "gt";
                case SIROpcode::GE:
                    return "ge";

                // Control Flow
                case SIROpcode::BR:
                    return "br";
                case SIROpcode::BR_COND:
                    return "br.cond";
                case SIROpcode::RET:
                    return "ret";
                case SIROpcode::RET_VOID:
                    return "ret.void";
                case SIROpcode::CALL:
                    return "call";
                case SIROpcode::CALL_INDIRECT:
                    return "call.indirect";
                case SIROpcode::SWITCH:
                    return "switch";
                case SIROpcode::PHI:
                    return "phi";

                // Memory
                case SIROpcode::ALLOC:
                    return "alloc";
                case SIROpcode::ALLOC_HEAP:
                    return "alloc.heap";
                case SIROpcode::FREE:
                    return "free";
                case SIROpcode::LOAD:
                    return "load";
                case SIROpcode::STORE:
                    return "store";
                case SIROpcode::MOVE:
                    return "move";
                case SIROpcode::ADDR:
                    return "addr";
                case SIROpcode::PTR_ADD:
                    return "ptr.add";
                case SIROpcode::PTR_CAST:
                    return "ptr.cast";
                case SIROpcode::MEMCPY:
                    return "memcpy";
                case SIROpcode::MEMSET:
                    return "memset";

                // Array
                case SIROpcode::ARRAY_NEW:
                    return "array.new";
                case SIROpcode::ARRAY_GET:
                    return "array.get";
                case SIROpcode::ARRAY_SET:
                    return "array.set";
                case SIROpcode::ARRAY_LEN:
                    return "array.len";
                case SIROpcode::ARRAY_APPEND:
                    return "array.append";
                case SIROpcode::ARRAY_REMOVE:
                    return "array.remove";
                case SIROpcode::ARRAY_CONCAT:
                    return "array.concat";
                case SIROpcode::ARRAY_ZIP:
                    return "array.zip";

                // Tuple
                case SIROpcode::TUPLE_NEW:
                    return "tuple.new";
                case SIROpcode::TUPLE_GET:
                    return "tuple.get";
                case SIROpcode::TUPLE_LEN:
                    return "tuple.len";

                // String
                case SIROpcode::STRING_NEW:
                    return "string.new";
                case SIROpcode::STRING_LEN:
                    return "string.len";
                case SIROpcode::STRING_CONCAT:
                    return "string.concat";
                case SIROpcode::STRING_CMP:
                    return "string.cmp";
                case SIROpcode::STRING_ORD_CMP:
                    return "string.ord_cmp";
                case SIROpcode::STRING_SUBSTR:
                    return "string.substr";
                case SIROpcode::STRING_FIND:
                    return "string.find";
                case SIROpcode::STRING_REPLACE:
                    return "string.replace";
                case SIROpcode::STRING_TO_I64:
                    return "string.to_i64";

                // OOP
                case SIROpcode::OBJECT_NEW:
                    return "object.new";
                case SIROpcode::OBJECT_GET:
                    return "object.get";
                case SIROpcode::OBJECT_SET:
                    return "object.set";
                case SIROpcode::OBJECT_CALL:
                    return "object.call";
                case SIROpcode::INSTANCEOF:
                    return "instanceof";
                case SIROpcode::OBJECT_CAST:
                    return "object.cast";
                case SIROpcode::CLASS_DEF:
                    return "class.def";
                case SIROpcode::METHOD_DEF:
                    return "method.def";
                case SIROpcode::FIELD_DEF:
                    return "field.def";
                case SIROpcode::CONSTRUCTOR_CALL:
                    return "constructor.call";

                // Type Conversion
                case SIROpcode::I64_TO_F64:
                    return "i64.to.f64";
                case SIROpcode::F64_TO_I64:
                    return "f64.to.i64";
                case SIROpcode::F64_TO_I64_SAT:
                    return "f64.to.i64.sat";
                case SIROpcode::I64_TO_BOOL:
                    return "i64.to.bool";
                case SIROpcode::BOOL_TO_I64:
                    return "bool.to.i64";
                case SIROpcode::I64_TO_STRING:
                    return "i64.to.string";
                case SIROpcode::F64_TO_STRING:
                    return "f64.to.string";
                case SIROpcode::BOOL_TO_STRING:
                    return "bool.to.string";
                case SIROpcode::ARRAY_TO_STRING:
                    return "array.to.string";
                case SIROpcode::TUPLE_TO_STRING:
                    return "tuple.to.string";
                case SIROpcode::CAST:
                    return "cast";

                // Builtin
                case SIROpcode::BUILTIN_PRINT:
                    return "builtin.print";
                case SIROpcode::BUILTIN_READ:
                    return "builtin.read";
                case SIROpcode::BUILTIN_SQRT:
                    return "builtin.sqrt";
                case SIROpcode::BUILTIN_LOG:
                    return "builtin.log";
                case SIROpcode::BUILTIN_POW:
                    return "builtin.pow";
                case SIROpcode::BUILTIN_SIN:
                    return "builtin.sin";
                case SIROpcode::BUILTIN_COS:
                    return "builtin.cos";
                case SIROpcode::BUILTIN_TAN:
                    return "builtin.tan";
                case SIROpcode::BUILTIN_ABS:
                    return "builtin.abs";
                case SIROpcode::BUILTIN_FLOOR:
                    return "builtin.floor";
                case SIROpcode::BUILTIN_CEIL:
                    return "builtin.ceil";
                case SIROpcode::BUILTIN_ROUND:
                    return "builtin.round";
                case SIROpcode::BUILTIN_MIN:
                    return "builtin.min";
                case SIROpcode::BUILTIN_MAX:
                    return "builtin.max";
                case SIROpcode::BUILTIN_ASSERT:
                    return "builtin.assert";
                case SIROpcode::BUILTIN_DEBUG:
                    return "builtin.debug";
                case SIROpcode::BUILTIN_LOG10:
                    return "builtin.log10";
                case SIROpcode::BUILTIN_LOG2:
                    return "builtin.log2";
                case SIROpcode::BUILTIN_ASIN:
                    return "builtin.asin";
                case SIROpcode::BUILTIN_ACOS:
                    return "builtin.acos";
                case SIROpcode::BUILTIN_TRUNC:
                    return "builtin.trunc";
                case SIROpcode::BUILTIN_FMOD:
                    return "builtin.fmod";
                case SIROpcode::BUILTIN_CLAMP:
                    return "builtin.clamp";
                case SIROpcode::BUILTIN_ATAN:
                    return "builtin.atan";

                // Hardware/OS Builtins
                case SIROpcode::BUILTIN_PORT_WRITE:
                    return "builtin.port_write";
                case SIROpcode::BUILTIN_PORT_READ:
                    return "builtin.port_read";
                case SIROpcode::BUILTIN_PORT_WRITE_16:
                    return "builtin.port_write16";
                case SIROpcode::BUILTIN_PORT_READ_16:
                    return "builtin.port_read16";
                case SIROpcode::BUILTIN_PORT_WRITE_32:
                    return "builtin.port_write32";
                case SIROpcode::BUILTIN_PORT_READ_32:
                    return "builtin.port_read32";
                case SIROpcode::BUILTIN_MEM_WRITE_8:
                    return "builtin.mem_write8";
                case SIROpcode::BUILTIN_MEM_READ_8:
                    return "builtin.mem_read8";
                case SIROpcode::BUILTIN_MEM_WRITE_16:
                    return "builtin.mem_write16";
                case SIROpcode::BUILTIN_MEM_READ_16:
                    return "builtin.mem_read16";
                case SIROpcode::BUILTIN_MEM_WRITE_32:
                    return "builtin.mem_write32";
                case SIROpcode::BUILTIN_MEM_READ_32:
                    return "builtin.mem_read32";
                case SIROpcode::BUILTIN_MEM_WRITE_64:
                    return "builtin.mem_write64";
                case SIROpcode::BUILTIN_MEM_READ_64:
                    return "builtin.mem_read64";
                case SIROpcode::BUILTIN_VGA_WRITE:
                    return "builtin.vga_write";
                case SIROpcode::BUILTIN_VGA_CLEAR:
                    return "builtin.vga_clear";
                case SIROpcode::BUILTIN_INTERRUPT:
                    return "builtin.interrupt";
                case SIROpcode::BUILTIN_HALT:
                    return "builtin.halt";
                case SIROpcode::BUILTIN_CLI:
                    return "builtin.cli";
                case SIROpcode::BUILTIN_STI:
                    return "builtin.sti";
                case SIROpcode::BUILTIN_MEM_COPY:
                    return "builtin.mem_copy";
                case SIROpcode::BUILTIN_MEM_SET:
                    return "builtin.mem_set";

                // Android Operations (48 opcodes)
                // 19a: Memory
                case SIROpcode::ANDROID_ALLOC:
                    return "android.alloc";
                case SIROpcode::ANDROID_FREE:
                    return "android.free";
                // 19b: Strings
                case SIROpcode::ANDROID_STRING_CREATE:
                    return "android.string.create";
                case SIROpcode::ANDROID_STRING_CONCAT:
                    return "android.string.concat";
                case SIROpcode::ANDROID_STRING_LENGTH:
                    return "android.string.length";
                case SIROpcode::ANDROID_STRING_SUBSTR:
                    return "android.string.substr";
                case SIROpcode::ANDROID_STRING_COMPARE:
                    return "android.string.compare";
                case SIROpcode::ANDROID_STRING_FREE:
                    return "android.string.free";
                // 19c: Arrays
                case SIROpcode::ANDROID_ARRAY_CREATE:
                    return "android.array.create";
                case SIROpcode::ANDROID_ARRAY_GET:
                    return "android.array.get";
                case SIROpcode::ANDROID_ARRAY_SET:
                    return "android.array.set";
                case SIROpcode::ANDROID_ARRAY_LENGTH:
                    return "android.array.length";
                case SIROpcode::ANDROID_ARRAY_PUSH:
                    return "android.array.push";
                case SIROpcode::ANDROID_ARRAY_POP:
                    return "android.array.pop";
                case SIROpcode::ANDROID_ARRAY_FREE:
                    return "android.array.free";
                // 19d: Maps
                case SIROpcode::ANDROID_MAP_CREATE:
                    return "android.map.create";
                case SIROpcode::ANDROID_MAP_GET:
                    return "android.map.get";
                case SIROpcode::ANDROID_MAP_SET:
                    return "android.map.set";
                case SIROpcode::ANDROID_MAP_HAS:
                    return "android.map.has";
                case SIROpcode::ANDROID_MAP_DELETE:
                    return "android.map.delete";
                case SIROpcode::ANDROID_MAP_SIZE:
                    return "android.map.size";
                case SIROpcode::ANDROID_MAP_FREE:
                    return "android.map.free";
                // 19e: Network
                case SIROpcode::ANDROID_NET_CONNECT:
                    return "android.net.connect";
                case SIROpcode::ANDROID_NET_SEND:
                    return "android.net.send";
                case SIROpcode::ANDROID_NET_RECV:
                    return "android.net.recv";
                case SIROpcode::ANDROID_NET_CLOSE:
                    return "android.net.close";
                case SIROpcode::ANDROID_HTTP_REQUEST:
                    return "android.http.request";
                case SIROpcode::ANDROID_WS_CONNECT:
                    return "android.ws.connect";
                case SIROpcode::ANDROID_WS_SEND:
                    return "android.ws.send";
                case SIROpcode::ANDROID_WS_RECV:
                    return "android.ws.recv";
                // 19f: Threads
                case SIROpcode::ANDROID_THREAD_CREATE:
                    return "android.thread.create";
                case SIROpcode::ANDROID_THREAD_JOIN:
                    return "android.thread.join";
                case SIROpcode::ANDROID_MUTEX_CREATE:
                    return "android.mutex.create";
                case SIROpcode::ANDROID_MUTEX_LOCK:
                    return "android.mutex.lock";
                case SIROpcode::ANDROID_MUTEX_UNLOCK:
                    return "android.mutex.unlock";
                case SIROpcode::ANDROID_CHANNEL_CREATE:
                    return "android.channel.create";
                case SIROpcode::ANDROID_CHANNEL_SEND:
                    return "android.channel.send";
                case SIROpcode::ANDROID_CHANNEL_RECV:
                    return "android.channel.recv";
                // 19g: UI
                case SIROpcode::ANDROID_UI_INIT:
                    return "android.ui.init";
                case SIROpcode::ANDROID_UI_CREATE_WIDGET:
                    return "android.ui.create_widget";
                case SIROpcode::ANDROID_UI_SET_TEXT:
                    return "android.ui.set_text";
                case SIROpcode::ANDROID_UI_SET_CALLBACK:
                    return "android.ui.set_callback";
                case SIROpcode::ANDROID_UI_SHOW:
                    return "android.ui.show";
                case SIROpcode::ANDROID_UI_HIDE:
                    return "android.ui.hide";
                case SIROpcode::ANDROID_UI_UPDATE:
                    return "android.ui.update";
                case SIROpcode::ANDROID_UI_RUN:
                    return "android.ui.run";
                // 19h: Logging
                case SIROpcode::ANDROID_LOG:
                    return "android.log";
                case SIROpcode::ANDROID_PRINT:
                    return "android.print";

                // 20: Unified UI System
                case SIROpcode::BUILTIN_UI_COLUMN:
                    return "ui.column";
                case SIROpcode::BUILTIN_UI_ROW:
                    return "ui.row";
                case SIROpcode::BUILTIN_UI_STACK:
                    return "ui.stack";
                case SIROpcode::BUILTIN_UI_CONTAINER:
                    return "ui.container";
                case SIROpcode::BUILTIN_UI_TEXT:
                    return "ui.text";
                case SIROpcode::BUILTIN_UI_TEXT_STYLED:
                    return "ui.text_styled";
                case SIROpcode::BUILTIN_UI_BUTTON:
                    return "ui.button";
                case SIROpcode::BUILTIN_UI_BUTTON_VARIANT:
                    return "ui.button_variant";
                case SIROpcode::BUILTIN_UI_ICON_BUTTON:
                    return "ui.icon_button";
                case SIROpcode::BUILTIN_UI_FAB:
                    return "ui.fab";
                case SIROpcode::BUILTIN_UI_TEXT_FIELD:
                    return "ui.text_field";
                case SIROpcode::BUILTIN_UI_CHECKBOX:
                    return "ui.checkbox";
                case SIROpcode::BUILTIN_UI_SWITCH:
                    return "ui.switch";
                case SIROpcode::BUILTIN_UI_SLIDER:
                    return "ui.slider";
                case SIROpcode::BUILTIN_UI_CARD:
                    return "ui.card";
                case SIROpcode::BUILTIN_UI_SCAFFOLD:
                    return "ui.scaffold";
                case SIROpcode::BUILTIN_UI_APP_BAR:
                    return "ui.app_bar";
                case SIROpcode::BUILTIN_UI_SPACER:
                    return "ui.spacer";
                case SIROpcode::BUILTIN_UI_DIVIDER:
                    return "ui.divider";
                case SIROpcode::BUILTIN_UI_DIALOG:
                    return "ui.dialog";
                // مصانع إضافيّة (م-مصانع) / Extra factories
                case SIROpcode::BUILTIN_UI_IMAGE:
                    return "ui.image";
                case SIROpcode::BUILTIN_UI_ICON:
                    return "ui.icon";
                case SIROpcode::BUILTIN_UI_TEXT_BUTTON:
                    return "ui.text_button";
                case SIROpcode::BUILTIN_UI_GRID:
                    return "ui.grid";
                case SIROpcode::BUILTIN_UI_CENTER:
                    return "ui.center";
                case SIROpcode::BUILTIN_UI_PADDING:
                    return "ui.padding";
                case SIROpcode::BUILTIN_UI_ALIGN:
                    return "ui.align";
                case SIROpcode::BUILTIN_UI_EXPANDED:
                    return "ui.expanded";
                case SIROpcode::BUILTIN_UI_FLEXIBLE:
                    return "ui.flexible";
                case SIROpcode::BUILTIN_UI_SIZED_BOX:
                    return "ui.sized_box";
                case SIROpcode::BUILTIN_UI_WRAP:
                    return "ui.wrap";
                case SIROpcode::BUILTIN_UI_BOX:
                    return "ui.box";
                case SIROpcode::BUILTIN_UI_SCROLL_VIEW:
                    return "ui.scroll_view";
                case SIROpcode::BUILTIN_UI_BOTTOM_NAV:
                    return "ui.bottom_nav";
                case SIROpcode::BUILTIN_UI_SNACKBAR:
                    return "ui.snackbar";
                case SIROpcode::BUILTIN_UI_TOOLTIP:
                    return "ui.tooltip";
                case SIROpcode::BUILTIN_UI_PROGRESS:
                    return "ui.progress";
                case SIROpcode::BUILTIN_UI_LAZY_COLUMN:
                    return "ui.lazy_column";
                case SIROpcode::BUILTIN_UI_LAZY_ROW:
                    return "ui.lazy_row";
                case SIROpcode::BUILTIN_UI_LIST_VIEW:
                    return "ui.list_view";
                case SIROpcode::BUILTIN_UI_TEXT_AREA:
                    return "ui.text_area";
                case SIROpcode::BUILTIN_UI_DRAWER:
                    return "ui.drawer";
                case SIROpcode::BUILTIN_UI_SPIN_BOX:
                    return "ui.spin_box";
                case SIROpcode::BUILTIN_UI_GROUP_BOX:
                    return "ui.group_box";
                case SIROpcode::BUILTIN_UI_SPINNER:
                    return "ui.spinner";
                case SIROpcode::BUILTIN_UI_STATUS_BAR:
                    return "ui.status_bar";
                case SIROpcode::BUILTIN_UI_WINDOW:
                    return "ui.window";
                case SIROpcode::BUILTIN_UI_TITLE_BAR:
                    return "ui.title_bar";
                case SIROpcode::BUILTIN_UI_SCROLL_BAR:
                    return "ui.scroll_bar";
                case SIROpcode::BUILTIN_UI_TASKBAR:
                    return "ui.taskbar";
                case SIROpcode::BUILTIN_UI_START_MENU:
                    return "ui.start_menu";
                case SIROpcode::BUILTIN_UI_SYSTEM_TRAY:
                    return "ui.system_tray";
                case SIROpcode::BUILTIN_UI_MENU:
                    return "ui.menu";
                case SIROpcode::BUILTIN_UI_TOOLBAR:
                    return "ui.toolbar";
                case SIROpcode::BUILTIN_UI_SEGMENTED_CONTROL:
                    return "ui.segmented_control";
                case SIROpcode::BUILTIN_UI_TREE_VIEW:
                    return "ui.tree_view";
                case SIROpcode::BUILTIN_UI_SPLIT_VIEW:
                    return "ui.split_view";
                case SIROpcode::BUILTIN_UI_DATA_TABLE:
                    return "ui.data_table";
                case SIROpcode::BUILTIN_UI_BREADCRUMB:
                    return "ui.breadcrumb";
                case SIROpcode::BUILTIN_UI_PICKER:
                    return "ui.picker";
                case SIROpcode::BUILTIN_UI_DATE_PICKER:
                    return "ui.date_picker";
                case SIROpcode::BUILTIN_UI_STEPPER:
                    return "ui.stepper";
                case SIROpcode::BUILTIN_UI_RATING_BAR:
                    return "ui.rating_bar";
                case SIROpcode::BUILTIN_UI_ALERT:
                    return "ui.alert";
                case SIROpcode::BUILTIN_UI_SAFE_AREA:
                    return "ui.safe_area";
                case SIROpcode::BUILTIN_UI_SURFACE:
                    return "ui.surface";
                case SIROpcode::BUILTIN_UI_SET_PROP_STR:
                    return "ui.set_prop_str";
                case SIROpcode::BUILTIN_UI_SET_PROP_INT:
                    return "ui.set_prop_int";
                case SIROpcode::BUILTIN_UI_SET_PROP_NUM:
                    return "ui.set_prop_num";
                case SIROpcode::BUILTIN_UI_SET_PROP_BOOL:
                    return "ui.set_prop_bool";
                case SIROpcode::BUILTIN_UI_ADD_EVENT:
                    return "ui.add_event";
                case SIROpcode::BUILTIN_UI_ANIM_BEGIN:
                    return "ui.anim_begin";
                case SIROpcode::BUILTIN_UI_ANIM_DURATION:
                    return "ui.anim_duration";
                case SIROpcode::BUILTIN_UI_ANIM_EASING:
                    return "ui.anim_easing";
                case SIROpcode::BUILTIN_UI_SET_EVENT_PHASE:
                    return "ui.set_event_phase";
                case SIROpcode::BUILTIN_UI_SET_EVENT_DATA:
                    return "ui.set_event_data";
                case SIROpcode::BUILTIN_UI_ANIM_DELAY:
                    return "ui.anim_delay";
                case SIROpcode::BUILTIN_UI_ANIM_REPEAT:
                    return "ui.anim_repeat";
                case SIROpcode::BUILTIN_UI_ANIM_AUTO_REVERSE:
                    return "ui.anim_auto_reverse";
                case SIROpcode::BUILTIN_UI_PROP_JOIN_ADD:
                    return "ui.prop_join_add";
                case SIROpcode::BUILTIN_UI_PROP_JOIN_COMMIT:
                    return "ui.prop_join_commit";
                case SIROpcode::BUILTIN_UI_ADD_CHILD:
                    return "ui.add_child";
                case SIROpcode::BUILTIN_UI_REMOVE_CHILD:
                    return "ui.remove_child";
                case SIROpcode::BUILTIN_UI_CLEAR_CHILDREN:
                    return "ui.clear_children";
                case SIROpcode::BUILTIN_UI_SET_TEXT:
                    return "ui.set_text";
                case SIROpcode::BUILTIN_UI_SET_SIZE:
                    return "ui.set_size";
                case SIROpcode::BUILTIN_UI_SET_FLEX:
                    return "ui.set_flex";
                case SIROpcode::BUILTIN_UI_SET_BACKGROUND:
                    return "ui.set_background";
                case SIROpcode::BUILTIN_UI_SET_FOREGROUND:
                    return "ui.set_foreground";
                case SIROpcode::BUILTIN_UI_SET_SPACING:
                    return "ui.set_spacing";
                case SIROpcode::BUILTIN_UI_SET_PADDING:
                    return "ui.set_padding";
                case SIROpcode::BUILTIN_UI_SET_ALIGNMENT:
                    return "ui.set_alignment";
                case SIROpcode::BUILTIN_UI_SET_BORDER:
                    return "ui.set_border";
                case SIROpcode::BUILTIN_UI_SET_ELEVATION:
                    return "ui.set_elevation";
                case SIROpcode::BUILTIN_UI_SET_OPACITY:
                    return "ui.set_opacity";
                case SIROpcode::BUILTIN_UI_SET_VISIBILITY:
                    return "ui.set_visibility";
                case SIROpcode::BUILTIN_UI_APP_CREATE:
                    return "ui.app_create";
                case SIROpcode::BUILTIN_UI_APP_SET_ROOT:
                    return "ui.app_set_root";
                case SIROpcode::BUILTIN_UI_APP_LAYOUT:
                    return "ui.app_layout";
                case SIROpcode::BUILTIN_UI_APP_RENDER:
                    return "ui.app_render";
                case SIROpcode::BUILTIN_UI_APP_RUN:
                    return "ui.app_run";
                case SIROpcode::BUILTIN_UI_PRINT_TREE:
                    return "ui.print_tree";
                case SIROpcode::BUILTIN_UI_APP_DESTROY:
                    return "ui.app_destroy";
                case SIROpcode::BUILTIN_UI_WIDGET_DESTROY:
                    return "ui.widget_destroy";
                // (AR) دوال الثيم — ترتيب الحالات مطابق لترتيب التعداد في sir_types.h
                case SIROpcode::BUILTIN_UI_TOGGLE_THEME:
                    return "ui.toggle_theme";
                case SIROpcode::BUILTIN_UI_DARK_MODE:
                    return "ui.dark_mode";
                case SIROpcode::BUILTIN_UI_LIGHT_MODE:
                    return "ui.light_mode";
                case SIROpcode::BUILTIN_UI_IS_DARK:
                    return "ui.is_dark";
                case SIROpcode::BUILTIN_UI_NEXT_KEY:
                    return "ui.next_key";
                case SIROpcode::BUILTIN_UI_NAVIGATE:
                    return "ui.navigate";
                case SIROpcode::BUILTIN_UI_NAV_BACK:
                    return "ui.nav_back";
                case SIROpcode::BUILTIN_UI_NAV_ROOT:
                    return "ui.nav_root";
                case SIROpcode::BUILTIN_UI_REPLACE_PAGE:
                    return "ui.replace_page";
                case SIROpcode::BUILTIN_UI_PAGE_COUNT:
                    return "ui.page_count";
                case SIROpcode::BUILTIN_UI_CURRENT_PAGE:
                    return "ui.current_page";
                case SIROpcode::BUILTIN_UI_NAVIGATE_TRANSITION:
                    return "ui.navigate_transition";
                case SIROpcode::BUILTIN_UI_BACK_TRANSITION:
                    return "ui.back_transition";
                // (AR) إكمال corui: انتقال كامل + الحالة + النافذة + توليد ويب
                case SIROpcode::BUILTIN_UI_NAVIGATE_EXIT_TRANSITION:
                    return "ui.navigate_exit_transition";
                case SIROpcode::BUILTIN_UI_STOP_PROPAGATION:
                    return "ui.stop_propagation";
                case SIROpcode::BUILTIN_UI_UPDATE_STATE:
                    return "ui.update_state";
                case SIROpcode::BUILTIN_UI_SET_STATE:
                    return "ui.set_state";
                case SIROpcode::BUILTIN_UI_SET_TITLE:
                    return "ui.set_title";
                case SIROpcode::BUILTIN_UI_CLOSE_WINDOW:
                    return "ui.close_window";
                case SIROpcode::BUILTIN_UI_GEN_WEB:
                    return "ui.gen_web";

                // Module System
                case SIROpcode::MODULE_LOAD:
                    return "module.load";
                case SIROpcode::MODULE_SYMBOL:
                    return "module.symbol";
                case SIROpcode::MODULE_EXPORT:
                    return "module.export";
                case SIROpcode::MODULE_INIT:
                    return "module.init";

                // Closures
                case SIROpcode::CLOSURE_CREATE:
                    return "closure.create";
                case SIROpcode::CLOSURE_CALL:
                    return "closure.call";
                case SIROpcode::ENV_LOAD:
                    return "env.load";
                case SIROpcode::ENV_STORE:
                    return "env.store";

                // (AR) عمليات القنوات / (EN) Channel operations
                case SIROpcode::ASYNC_CREATE_CHANNEL:
                    return "async.channel.create";
                case SIROpcode::ASYNC_CHANNEL_SEND:
                    return "async.channel.send";
                case SIROpcode::ASYNC_CHANNEL_RECV:
                    return "async.channel.recv";
                case SIROpcode::ASYNC_CHANNEL_CLOSE:
                    return "async.channel.close";
                case SIROpcode::ASYNC_CHANNEL_IS_CLOSED:
                    return "async.channel.is_closed";
                case SIROpcode::ASYNC_CHANNEL_HAS_DATA:
                    return "async.channel.has_data";
                case SIROpcode::ASYNC_CHANNEL_SIZE:
                    return "async.channel.size";
                case SIROpcode::ASYNC_CHANNEL_CAPACITY:
                    return "async.channel.capacity";
                case SIROpcode::ASYNC_CHANNEL_TRY_SEND:
                    return "async.channel.try_send";
                case SIROpcode::ASYNC_CHANNEL_TRY_RECV:
                    return "async.channel.try_recv";
                case SIROpcode::ASYNC_CHANNEL_SEND_TIMEOUT:
                    return "async.channel.send_timeout";
                case SIROpcode::ASYNC_CHANNEL_RECV_TIMEOUT:
                    return "async.channel.recv_timeout";
                case SIROpcode::ASYNC_MUTEX_TRY_LOCK:
                    return "async.mutex.try_lock";
                case SIROpcode::ASYNC_MUTEX_IS_LOCKED:
                    return "async.mutex.is_locked";
                case SIROpcode::ASYNC_FUTURE_IS_READY:
                    return "async.future.is_ready";
                case SIROpcode::ASYNC_WG_CREATE:
                    return "async.wg.create";
                case SIROpcode::ASYNC_WG_ADD:
                    return "async.wg.add";
                case SIROpcode::ASYNC_WG_DONE:
                    return "async.wg.done";
                case SIROpcode::ASYNC_WG_WAIT:
                    return "async.wg.wait";
                case SIROpcode::ASYNC_WG_COUNT:
                    return "async.wg.count";

                default:
                    return "unknown";
                }
            }

            // ======================================================================
            // عدد المعاملات / Operand count
            // ======================================================================

            int sirOpcodeOperandCount(SIROpcode opcode)
            {
                switch (opcode)
                {
                // Binary operations (2 operands)
                case SIROpcode::ADD_I64:
                case SIROpcode::ADD_F64:
                case SIROpcode::SUB_I64:
                case SIROpcode::SUB_F64:
                case SIROpcode::MUL_I64:
                case SIROpcode::MUL_F64:
                case SIROpcode::DIV_I64:
                case SIROpcode::DIV_F64:
                case SIROpcode::FLOOR_DIV_I64:
                case SIROpcode::MOD_I64:
                case SIROpcode::AND:
                case SIROpcode::OR:
                case SIROpcode::XOR:
                case SIROpcode::SHL:
                case SIROpcode::SHR:
                case SIROpcode::SAR:
                case SIROpcode::ROL:
                case SIROpcode::EQ:
                case SIROpcode::NE:
                case SIROpcode::LT:
                case SIROpcode::LE:
                case SIROpcode::GT:
                case SIROpcode::GE:
                case SIROpcode::PTR_ADD:
                case SIROpcode::ARRAY_GET:
                case SIROpcode::TUPLE_GET:
                case SIROpcode::STRING_CONCAT:
                case SIROpcode::STRING_CMP:
                case SIROpcode::STRING_ORD_CMP:
                case SIROpcode::OBJECT_GET:
                case SIROpcode::BUILTIN_POW:
                case SIROpcode::BUILTIN_MIN:
                case SIROpcode::BUILTIN_MAX:
                case SIROpcode::BUILTIN_FMOD:
                    return 2;

                // Unary operations (1 operand)
                case SIROpcode::NEG:
                case SIROpcode::NULL_ASSERT:
                case SIROpcode::NOT:
                case SIROpcode::RET:
                case SIROpcode::BR:
                case SIROpcode::LOAD:
                case SIROpcode::MOVE:
                case SIROpcode::ADDR:
                case SIROpcode::FREE:
                case SIROpcode::ARRAY_LEN:
                case SIROpcode::TUPLE_LEN:
                case SIROpcode::STRING_LEN:
                case SIROpcode::OBJECT_NEW:
                case SIROpcode::I64_TO_F64:
                case SIROpcode::F64_TO_I64:
                case SIROpcode::F64_TO_I64_SAT:
                case SIROpcode::I64_TO_BOOL:
                case SIROpcode::BOOL_TO_I64:
                case SIROpcode::I64_TO_STRING:
                case SIROpcode::F64_TO_STRING:
                case SIROpcode::BOOL_TO_STRING:
                case SIROpcode::ARRAY_TO_STRING:
                case SIROpcode::TUPLE_TO_STRING:
                case SIROpcode::BUILTIN_PRINT:
                case SIROpcode::BUILTIN_SQRT:
                case SIROpcode::BUILTIN_LOG:
                case SIROpcode::BUILTIN_SIN:
                case SIROpcode::BUILTIN_COS:
                case SIROpcode::BUILTIN_TAN:
                case SIROpcode::BUILTIN_ABS:
                case SIROpcode::BUILTIN_FLOOR:
                case SIROpcode::BUILTIN_CEIL:
                case SIROpcode::BUILTIN_ROUND:
                case SIROpcode::BUILTIN_LOG10:
                case SIROpcode::BUILTIN_LOG2:
                case SIROpcode::BUILTIN_ASIN:
                case SIROpcode::BUILTIN_ACOS:
                case SIROpcode::BUILTIN_TRUNC:
                case SIROpcode::BUILTIN_ATAN:
                    return 1;

                // No operands
                case SIROpcode::RET_VOID:
                case SIROpcode::BUILTIN_READ:
                    return 0;

                // Variable operands (return -1)
                case SIROpcode::BR_COND:
                case SIROpcode::CALL:
                case SIROpcode::CALL_INDIRECT:
                case SIROpcode::SWITCH:
                case SIROpcode::PHI:
                case SIROpcode::ALLOC:
                case SIROpcode::ALLOC_HEAP:
                case SIROpcode::STORE:
                case SIROpcode::PTR_CAST:
                case SIROpcode::MEMCPY:
                case SIROpcode::MEMSET:
                case SIROpcode::ARRAY_NEW:
                case SIROpcode::ARRAY_SET:
                case SIROpcode::ARRAY_APPEND:
                case SIROpcode::ARRAY_REMOVE:
                case SIROpcode::ARRAY_CONCAT:
                case SIROpcode::ARRAY_ZIP:
                case SIROpcode::TUPLE_NEW:
                case SIROpcode::STRING_NEW:
                case SIROpcode::STRING_SUBSTR:
                case SIROpcode::STRING_FIND:
                case SIROpcode::STRING_REPLACE:
                case SIROpcode::STRING_TO_I64:
                case SIROpcode::OBJECT_SET:
                case SIROpcode::OBJECT_CALL:
                case SIROpcode::INSTANCEOF:
                case SIROpcode::OBJECT_CAST:
                case SIROpcode::CLASS_DEF:
                case SIROpcode::METHOD_DEF:
                case SIROpcode::FIELD_DEF:
                case SIROpcode::CONSTRUCTOR_CALL:
                case SIROpcode::CAST:
                case SIROpcode::BUILTIN_ASSERT:
                case SIROpcode::BUILTIN_DEBUG:
                    return -1; // Variable number of operands

                default:
                    return 0;
                }
            }

            // ======================================================================
            // تحويل SIROperand إلى نص / Convert SIROperand to string
            // ======================================================================

            std::string SIROperand::toString() const
            {
                std::ostringstream oss;

                switch (type)
                {
                case SIROperandType::REGISTER:
                    oss << "%" << name;
                    break;

                case SIROperandType::CONSTANT:
                    switch (dataType)
                    {
                    case SadTypeKind::Integer:
                        oss << intValue;
                        break;
                    case SadTypeKind::Float:
                        oss << floatValue;
                        break;
                    case SadTypeKind::Boolean:
                        oss << (boolValue ? "true" : "false");
                        break;
                    case SadTypeKind::String:
                        oss << "\"" << name << "\"";
                        break;
                    default:
                        oss << "const";
                        break;
                    }
                    break;

                case SIROperandType::LABEL:
                    oss << "label %" << name;
                    break;

                case SIROperandType::GLOBAL:
                    oss << "$" << name;
                    break;

                case SIROperandType::FUNCTION:
                    oss << "@" << name;
                    break;
                }

                return oss.str();
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
