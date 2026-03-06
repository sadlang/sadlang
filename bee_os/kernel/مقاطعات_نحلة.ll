; ModuleID = 'مقاطعات_نحلة'
source_filename = "\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%SadArray = type { i64, i64, ptr }

@str.const = private unnamed_addr constant [17 x i8] c"0123456789ABCDEF\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.2 = private unnamed_addr constant [149 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\0A\00", align 1
@str.const.3 = private unnamed_addr constant [75 x i8] c"\E2\95\91  \E2\9B\94 \D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1: \D8\A7\D9\84\D9\82\D8\B3\D9\85\D8\A9 \D8\B9\D9\84\D9\89 \D8\B5\D9\81\D8\B1 (Exception #0)    \E2\95\91\0A\00", align 1
@str.const.4 = private unnamed_addr constant [149 x i8] c"\E2\95\A0\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\A3\0A\00", align 1
@str.const.5 = private unnamed_addr constant [89 x i8] c"\E2\95\91  \D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A9 \D8\A7\D9\84\D8\AA\D9\8A \D8\AD\D8\A7\D9\88\D9\84\D8\AA \D8\A7\D9\84\D9\82\D8\B3\D9\85\D8\A9 \D8\B9\D9\84\D9\89 \D8\B5\D9\81\D8\B1 \D8\B3\D8\AA\D9\8F\D9\86\D9\87\D9\89.    \E2\95\91\0A\00", align 1
@str.const.6 = private unnamed_addr constant [72 x i8] c"\E2\95\91  \D9\87\D8\B0\D8\A7 \D8\A7\D9\84\D8\AE\D8\B7\D8\A3 \D9\8A\D8\AD\D8\AF\D8\AB \D8\B9\D9\86\D8\AF\D9\85\D8\A7:                        \E2\95\91\0A\00", align 1
@str.const.7 = private unnamed_addr constant [70 x i8] c"\E2\95\91   - \D9\8A\D9\83\D9\88\D9\86 \D8\A7\D9\84\D9\85\D9\82\D8\B3\D9\88\D9\85 \D8\B9\D9\84\D9\8A\D9\87 = 0                     \E2\95\91\0A\00", align 1
@str.const.8 = private unnamed_addr constant [84 x i8] c"\E2\95\91   - \D9\86\D8\A7\D8\AA\D8\AC \D8\A7\D9\84\D9\82\D8\B3\D9\85\D8\A9 \D8\A3\D9\83\D8\A8\D8\B1 \D9\85\D9\86 \D8\A7\D9\84\D8\B3\D8\AC\D9\84 \D8\A7\D9\84\D9\85\D8\B3\D8\AA\D9\87\D8\AF\D9\81        \E2\95\91\0A\00", align 1
@str.const.9 = private unnamed_addr constant [149 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\0A\00", align 1
@str.const.10 = private unnamed_addr constant [40 x i8] c"[INT] \D8\AA\D9\86\D9\82\D9\8A\D8\AD: Exception #1 (Debug)\0A\00", align 1
@str.const.11 = private unnamed_addr constant [40 x i8] c"[INT] \D9\86\D9\82\D8\B7\D8\A9 \D8\AA\D9\88\D9\82\D9\81: Breakpoint #3\0A\00", align 1
@str.const.12 = private unnamed_addr constant [59 x i8] c"  \E2\86\92 \D8\A7\D9\84\D9\85\D9\8F\D9\86\D9\82\D9\91\D8\AD: \D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A9 INT 3 \D9\85\D9\8F\D9\83\D8\AA\D8\B4\D9\81\D8\A9\0A\00", align 1
@str.const.15 = private unnamed_addr constant [77 x i8] c"\E2\95\91  \E2\9B\94 \D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1: \D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A9 \D8\BA\D9\8A\D8\B1 \D8\B5\D8\A7\D9\84\D8\AD\D8\A9 (Exception #6)  \E2\95\91\0A\00", align 1
@str.const.17 = private unnamed_addr constant [84 x i8] c"\E2\95\91  \D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC \D9\88\D8\AC\D8\AF \D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A9 \D9\84\D8\A7 \D9\8A\D9\85\D9\83\D9\86\D9\87 \D8\AA\D9\86\D9\81\D9\8A\D8\B0\D9\87\D8\A7.        \E2\95\91\0A\00", align 1
@str.const.18 = private unnamed_addr constant [66 x i8] c"\E2\95\91  \D8\A3\D8\B3\D8\A8\D8\A7\D8\A8 \D9\85\D8\AD\D8\AA\D9\85\D9\84\D8\A9:                                \E2\95\91\0A\00", align 1
@str.const.19 = private unnamed_addr constant [67 x i8] c"\E2\95\91   - \D9\83\D9\88\D8\AF \D8\B0\D8\A7\D9\83\D8\B1\D8\A9 \D9\81\D8\A7\D8\B3\D8\AF                            \E2\95\91\0A\00", align 1
@str.const.20 = private unnamed_addr constant [72 x i8] c"\E2\95\91   - \D9\82\D9\81\D8\B2 \D8\A5\D9\84\D9\89 \D8\B9\D9\86\D9\88\D8\A7\D9\86 \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA                      \E2\95\91\0A\00", align 1
@str.const.21 = private unnamed_addr constant [79 x i8] c"\E2\95\91   - \D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A9 \D8\AA\D8\AD\D8\AA\D8\A7\D8\AC \D9\85\D9\8A\D8\B2\D8\A9 CPU \D8\BA\D9\8A\D8\B1 \D9\85\D8\AA\D9\88\D9\81\D8\B1\D8\A9          \E2\95\91\0A\00", align 1
@str.const.24 = private unnamed_addr constant [173 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\0A\00", align 1
@str.const.25 = private unnamed_addr constant [73 x i8] c"\E2\95\91  \F0\9F\9A\A8 \D8\AE\D8\B7\D8\A3 \D9\85\D8\B2\D8\AF\D9\88\D8\AC!! (Double Fault \E2\80\94 Exception #8)       \E2\95\91\0A\00", align 1
@str.const.26 = private unnamed_addr constant [173 x i8] c"\E2\95\A0\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\A3\0A\00", align 1
@str.const.27 = private unnamed_addr constant [93 x i8] c"\E2\95\91  \D8\AD\D8\AF\D8\AB \D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1 \D8\A3\D8\AB\D9\86\D8\A7\D8\A1 \D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9 \D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1 \D8\A2\D8\AE\D8\B1!               \E2\95\91\0A\00", align 1
@str.const.28 = private unnamed_addr constant [83 x i8] c"\E2\95\91  \D8\A7\D9\84\D9\86\D8\B8\D8\A7\D9\85 \D9\81\D9\8A \D8\AD\D8\A7\D9\84\D8\A9 \D8\BA\D9\8A\D8\B1 \D9\85\D8\B3\D8\AA\D9\82\D8\B1\D8\A9.                          \E2\95\91\0A\00", align 1
@str.const.29 = private unnamed_addr constant [63 x i8] c"\E2\95\91                                                       \E2\95\91\0A\00", align 1
@str.const.30 = private unnamed_addr constant [74 x i8] c"\E2\95\91  \D9\87\D8\B0\D8\A7 \D9\8A\D8\B9\D9\86\D9\8A \D8\B9\D8\A7\D8\AF\D8\A9\D9\8B:                                     \E2\95\91\0A\00", align 1
@str.const.31 = private unnamed_addr constant [73 x i8] c"\E2\95\91   - \D8\AA\D9\84\D9\81 \D9\81\D9\8A \D8\A7\D9\84\D9\85\D9\83\D8\AF\D8\B3 (Stack corruption)                 \E2\95\91\0A\00", align 1
@str.const.32 = private unnamed_addr constant [71 x i8] c"\E2\95\91   - GDT \D8\A3\D9\88 IDT \D8\BA\D9\8A\D8\B1 \D8\B5\D8\A7\D9\84\D8\AD                              \E2\95\91\0A\00", align 1
@str.const.33 = private unnamed_addr constant [82 x i8] c"\E2\95\91   - \D8\AE\D8\B7\D8\A3 \D9\81\D9\8A \D9\85\D8\B9\D8\A7\D9\84\D8\AC \D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1 \D8\A2\D8\AE\D8\B1                         \E2\95\91\0A\00", align 1
@str.const.35 = private unnamed_addr constant [93 x i8] c"\E2\95\91  \E2\9A\A0 \D8\A5\D8\B0\D8\A7 \D9\81\D8\B4\D9\84 \D9\87\D8\B0\D8\A7 \D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC \E2\86\92 Triple Fault \E2\86\92 \D8\A5\D8\B9\D8\A7\D8\AF\D8\A9 \D8\AA\D8\B4\D8\BA\D9\8A\D9\84 \E2\95\91\0A\00", align 1
@str.const.36 = private unnamed_addr constant [173 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\0A\00", align 1
@str.const.38 = private unnamed_addr constant [161 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\0A\00", align 1
@str.const.39 = private unnamed_addr constant [73 x i8] c"\E2\95\91  \F0\9F\9B\A1 \D8\AE\D8\B7\D8\A3 \D8\AD\D9\85\D8\A7\D9\8A\D8\A9 \D8\B9\D8\A7\D9\85 (GPF \E2\80\94 Exception #13)          \E2\95\91\0A\00", align 1
@str.const.40 = private unnamed_addr constant [161 x i8] c"\E2\95\A0\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\A3\0A\00", align 1
@str.const.41 = private unnamed_addr constant [93 x i8] c"\E2\95\91  \D8\A7\D9\86\D8\AA\D9\87\D8\A7\D9\83 \D8\B5\D9\84\D8\A7\D8\AD\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D9\88\D8\B5\D9\88\D9\84 \D9\84\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9 \D8\A3\D9\88 \D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B7\D8\B9.       \E2\95\91\0A\00", align 1
@str.const.42 = private unnamed_addr constant [60 x i8] c"\E2\95\91                                                    \E2\95\91\0A\00", align 1
@str.const.43 = private unnamed_addr constant [73 x i8] c"\E2\95\91  \D8\A7\D9\84\D8\A3\D8\B3\D8\A8\D8\A7\D8\A8 \D8\A7\D9\84\D8\B4\D8\A7\D8\A6\D8\B9\D8\A9:                                 \E2\95\91\0A\00", align 1
@str.const.44 = private unnamed_addr constant [87 x i8] c"\E2\95\91   - \D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9 \D8\A7\D9\84\D9\88\D8\B5\D9\88\D9\84 \D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9 \D8\AD\D9\84\D9\82\D8\A9 0 \D9\85\D9\86 \D8\AD\D9\84\D9\82\D8\A9 3         \E2\95\91\0A\00", align 1
@str.const.45 = private unnamed_addr constant [78 x i8] c"\E2\95\91   - \D8\AA\D8\AD\D9\85\D9\8A\D9\84 \D9\88\D8\A7\D8\B5\D9\81 \D9\85\D9\82\D8\B7\D8\B9 \D8\BA\D9\8A\D8\B1 \D8\B5\D8\A7\D9\84\D8\AD                     \E2\95\91\0A\00", align 1
@str.const.46 = private unnamed_addr constant [86 x i8] c"\E2\95\91   - \D8\AA\D9\86\D9\81\D9\8A\D8\B0 \D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A9 \D9\85\D9\85\D9\8A\D9\91\D8\B2\D8\A9 (\D9\85\D8\AB\D9\84 hlt) \D9\85\D9\86 \D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85      \E2\95\91\0A\00", align 1
@str.const.47 = private unnamed_addr constant [161 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\0A\00", align 1
@str.const.48 = private unnamed_addr constant [37 x i8] c"  \D8\B9\D8\AF\D8\AF \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1 \D8\A7\D9\84\D8\AD\D9\85\D8\A7\D9\8A\D8\A9: \00", align 1
@str.const.52 = private unnamed_addr constant [69 x i8] c"\E2\95\91  \F0\9F\93\84 \D8\AE\D8\B7\D8\A3 \D8\B5\D9\81\D8\AD\D8\A9 (Page Fault \E2\80\94 Exception #14)        \E2\95\91\0A\00", align 1
@str.const.54 = private unnamed_addr constant [96 x i8] c"\E2\95\91  \D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9 \D8\A7\D9\84\D9\88\D8\B5\D9\88\D9\84 \D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\86 \D8\B0\D8\A7\D9\83\D8\B1\D8\A9 \D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A\D8\A9 \D8\BA\D9\8A\D8\B1 \D8\B5\D8\A7\D9\84\D8\AD.   \E2\95\91\0A\00", align 1
@str.const.56 = private unnamed_addr constant [87 x i8] c"\E2\95\91  \D9\81\D9\8A \D9\86\D8\B8\D8\A7\D9\85 \D9\85\D9\83\D8\AA\D9\85\D9\84\D8\8C \D9\87\D8\B0\D8\A7 \D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC \D8\B3\D9\8A\D9\82\D9\88\D9\85 \D8\A8\D9\80:            \E2\95\91\0A\00", align 1
@str.const.57 = private unnamed_addr constant [80 x i8] c"\E2\95\91   - \D9\81\D8\AD\D8\B5 CR2 \D9\84\D9\85\D8\B9\D8\B1\D9\81\D8\A9 \D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\86 \D8\A7\D9\84\D9\85\D8\B3\D8\A8\D8\A8                \E2\95\91\0A\00", align 1
@str.const.58 = private unnamed_addr constant [84 x i8] c"\E2\95\91   - \D9\81\D8\AD\D8\B5 \D9\83\D9\88\D8\AF \D8\A7\D9\84\D8\AE\D8\B7\D8\A3 \D9\84\D9\85\D8\B9\D8\B1\D9\81\D8\A9 \D9\86\D9\88\D8\B9 \D8\A7\D9\84\D9\88\D8\B5\D9\88\D9\84              \E2\95\91\0A\00", align 1
@str.const.59 = private unnamed_addr constant [79 x i8] c"\E2\95\91   - \D8\AA\D8\AD\D9\85\D9\8A\D9\84 \D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9 \D9\85\D9\86 \D8\A7\D9\84\D9\82\D8\B1\D8\B5 (swap) \D8\A3\D9\88               \E2\95\91\0A\00", align 1
@str.const.60 = private unnamed_addr constant [74 x i8] c"\E2\95\91   - \D8\AA\D8\B9\D9\8A\D9\8A\D9\86 \D8\B5\D9\81\D8\AD\D8\A9 \D8\AC\D8\AF\D9\8A\D8\AF\D8\A9 \D8\A3\D9\88                          \E2\95\91\0A\00", align 1
@str.const.61 = private unnamed_addr constant [79 x i8] c"\E2\95\91   - \D8\A5\D9\86\D9\87\D8\A7\D8\A1 \D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A9 \D8\A7\D9\84\D9\85\D8\AE\D8\A7\D9\84\D9\81\D8\A9 (SIGSEGV)              \E2\95\91\0A\00", align 1
@str.const.63 = private unnamed_addr constant [37 x i8] c"  \D8\B9\D8\AF\D8\AF \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1 \D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA: \00", align 1
@str.const.65 = private unnamed_addr constant [141 x i8] c"\0A\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\0A\00", align 1
@str.const.66 = private unnamed_addr constant [45 x i8] c"\E2\95\91  \E2\9A\A0 \D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1 \D8\BA\D9\8A\D8\B1 \D9\85\D8\B9\D8\A7\D9\84\D8\AC: #\00", align 1
@str.const.67 = private unnamed_addr constant [25 x i8] c"                    \E2\95\91\0A\00", align 1
@str.const.68 = private unnamed_addr constant [140 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\0A\00", align 1
@str.const.69 = private unnamed_addr constant [21 x i8] c"[TIMER] \D8\AB\D8\A7\D9\86\D9\8A\D8\A9: \00", align 1
@str.const.71 = private unnamed_addr constant [16 x i8] c"[KB] \D8\B6\D8\BA\D8\B7\D8\A9 #\00", align 1
@str.const.72 = private unnamed_addr constant [16 x i8] c" \E2\86\92 scancode: \00", align 1
@str.const.73 = private unnamed_addr constant [5 x i8] c" = '\00", align 1
@str.const.74 = private unnamed_addr constant [2 x i8] c"'\00", align 1
@str.const.76 = private unnamed_addr constant [62 x i8] c"  [!] \D9\85\D9\81\D8\AA\D8\A7\D8\AD Escape \E2\80\94 \D8\B3\D9\8A\D8\AA\D9\85 \D8\A5\D9\8A\D9\82\D8\A7\D9\81 \D8\A7\D9\84\D9\86\D8\B8\D8\A7\D9\85\0A\00", align 1
@str.const.77 = private unnamed_addr constant [29 x i8] c"[SERIAL] \D8\A8\D9\8A\D8\A7\D9\86 \D9\88\D8\A7\D8\B1\D8\AF: \00", align 1
@str.const.79 = private unnamed_addr constant [29 x i8] c"[MOUSE] IRQ12: \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA=\00", align 1
@str.const.81 = private unnamed_addr constant [37 x i8] c"[SPURIOUS] \D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A9 \D8\B2\D8\A7\D8\A6\D9\81\D8\A9 #\00", align 1
@str.const.83 = private unnamed_addr constant [44 x i8] c"[SYSCALL] \D8\A7\D8\B3\D8\AA\D8\AF\D8\B9\D8\A7\D8\A1 \D9\86\D8\B8\D8\A7\D9\85 INT 0x80\0A\00", align 1
@str.const.85 = private unnamed_addr constant [155 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\0A\00", align 1
@str.const.86 = private unnamed_addr constant [68 x i8] c"  \F0\9F\90\9D \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA \E2\80\94 BeeOS Interrupts\0A\00", align 1
@str.const.88 = private unnamed_addr constant [48 x i8] c"  [1/8] \D8\AA\D8\B9\D8\B7\D9\8A\D9\84 \D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA (cli)...\0A\00", align 1
@str.const.89 = private unnamed_addr constant [47 x i8] c"        \E2\9C\93 \D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA \D9\85\D9\8F\D8\B9\D8\B7\D9\91\D9\84\D8\A9\0A\00", align 1
@str.const.90 = private unnamed_addr constant [44 x i8] c"  [2/8] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 IDT (256 \D9\85\D8\AF\D8\AE\D9\84\D8\A9)...\0A\00", align 1
@str.const.91 = private unnamed_addr constant [30 x i8] c"        \E2\9C\93 IDT \D9\85\D9\8F\D9\87\D9\8A\D9\91\D8\A3\0A\00", align 1
@str.const.92 = private unnamed_addr constant [54 x i8] c"  [3/8] \D8\A5\D8\B9\D8\A7\D8\AF\D8\A9 \D8\AA\D8\B9\D9\8A\D9\8A\D9\86 PIC (IRQ \E2\86\92 32-47)...\0A\00", align 1
@str.const.93 = private unnamed_addr constant [42 x i8] c"        \E2\9C\93 PIC: Master\E2\86\9232, Slave\E2\86\9240\0A\00", align 1
@str.const.94 = private unnamed_addr constant [61 x i8] c"  [4/8] \D8\AA\D8\B3\D8\AC\D9\8A\D9\84 \D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA \D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1\D8\A7\D8\AA...\0A\00", align 1
@str.const.95 = private unnamed_addr constant [44 x i8] c"        \E2\9C\93 20 \D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1 \D9\85\D9\8F\D8\B3\D8\AC\D9\91\D9\84\0A\00", align 1
@str.const.96 = private unnamed_addr constant [42 x i8] c"  [5/8] \D8\AA\D8\B3\D8\AC\D9\8A\D9\84 \D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA IRQ...\0A\00", align 1
@str.const.97 = private unnamed_addr constant [49 x i8] c"        \E2\9C\93 IRQ: Timer, Keyboard, Serial, Mouse\0A\00", align 1
@str.const.98 = private unnamed_addr constant [73 x i8] c"  [6/8] \D8\AA\D8\B3\D8\AC\D9\8A\D9\84 \D9\85\D8\B9\D8\A7\D9\84\D8\AC \D8\A7\D8\B3\D8\AA\D8\AF\D8\B9\D8\A7\D8\A1 \D8\A7\D9\84\D9\86\D8\B8\D8\A7\D9\85 (INT 0x80)...\0A\00", align 1
@str.const.99 = private unnamed_addr constant [44 x i8] c"        \E2\9C\93 INT 0x80 \D9\85\D9\8F\D8\B3\D8\AC\D9\91\D9\84 (ring 3)\0A\00", align 1
@str.const.100 = private unnamed_addr constant [34 x i8] c"  [7/8] \D8\AA\D8\AD\D9\85\D9\8A\D9\84 IDT (lidt)...\0A\00", align 1
@str.const.101 = private unnamed_addr constant [40 x i8] c"        \E2\9C\93 IDT \D9\85\D9\8F\D8\AD\D9\85\D9\91\D9\84 \D9\81\D9\8A IDTR\0A\00", align 1
@str.const.102 = private unnamed_addr constant [48 x i8] c"  [8/8] \D8\AA\D9\85\D9\83\D9\8A\D9\86 \D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA (sti)...\0A\00", align 1
@str.const.103 = private unnamed_addr constant [53 x i8] c"        \E2\9C\93 \D8\A3\D9\82\D9\86\D8\B9\D8\A9 PIC: Master=0xE8, Slave=0xEF\0A\00", align 1
@str.const.104 = private unnamed_addr constant [53 x i8] c"        \E2\9C\93 \D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA \D9\85\D9\8F\D9\81\D8\B9\D9\91\D9\84\D8\A9 (sti)\0A\00", align 1
@str.const.107 = private unnamed_addr constant [78 x i8] c"  \E2\9C\93 \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA \D8\AC\D8\A7\D9\87\D8\B2 \E2\80\94 \F0\9F\90\9D \D8\A7\D9\84\D8\AE\D9\84\D9\8A\D8\A9 \D8\AA\D8\B3\D8\AA\D9\85\D8\B9!\0A\00", align 1
@str.const.110 = private unnamed_addr constant [61 x i8] c"    \E2\86\92 \D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1\D8\A7\D8\AA: 0(\C3\B70) 1(Debug) 3(BP) 6(UD)\0A\00", align 1
@str.const.111 = private unnamed_addr constant [43 x i8] c"    \E2\86\92              8(DF) 13(GPF) 14(PF)\0A\00", align 1
@str.const.112 = private unnamed_addr constant [29 x i8] c"    \E2\86\92 IRQ0: Timer (100Hz)\0A\00", align 1
@str.const.113 = private unnamed_addr constant [31 x i8] c"    \E2\86\92 IRQ1: Keyboard (PS/2)\0A\00", align 1
@str.const.114 = private unnamed_addr constant [29 x i8] c"    \E2\86\92 IRQ4: Serial (COM1)\0A\00", align 1
@str.const.115 = private unnamed_addr constant [29 x i8] c"    \E2\86\92 IRQ12: Mouse (PS/2)\0A\00", align 1
@str.const.117 = private unnamed_addr constant [152 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\0A\00", align 1
@str.const.118 = private unnamed_addr constant [75 x i8] c"\E2\95\91  \F0\9F\93\8A \D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA \E2\80\94 BeeOS               \E2\95\91\0A\00", align 1
@str.const.119 = private unnamed_addr constant [152 x i8] c"\E2\95\A0\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\A3\0A\00", align 1
@str.const.120 = private unnamed_addr constant [38 x i8] c"\E2\95\91  \D9\86\D8\A8\D8\B6\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\A4\D9\82\D8\AA:        \00", align 1
@str.const.122 = private unnamed_addr constant [34 x i8] c"\E2\95\91  \D8\A7\D9\84\D8\AB\D9\88\D8\A7\D9\86\D9\8A:             \00", align 1
@str.const.124 = private unnamed_addr constant [41 x i8] c"\E2\95\91  \D8\B6\D8\BA\D8\B7\D8\A7\D8\AA \D9\84\D9\88\D8\AD\D8\A9 \D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD:  \00", align 1
@str.const.126 = private unnamed_addr constant [36 x i8] c"\E2\95\91  \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1 \D8\B5\D9\81\D8\AD\D8\A7\D8\AA:        \00", align 1
@str.const.128 = private unnamed_addr constant [36 x i8] c"\E2\95\91  \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1 \D8\AD\D9\85\D8\A7\D9\8A\D8\A9:        \00", align 1
@str.const.130 = private unnamed_addr constant [38 x i8] c"\E2\95\91  \D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA \D8\B2\D8\A7\D8\A6\D9\81\D8\A9:      \00", align 1
@str.const.132 = private unnamed_addr constant [152 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\0A\00", align 1

declare void @sad_ll_idt_init()

declare void @sad_ll_idt_load()

declare void @sad_ll_idt_set_gate(i64, i64, i64)

declare void @sad_ll_interrupt_enable()

declare void @sad_ll_interrupt_disable()

declare void @sad_ll_interrupt_eoi(i64)

declare void @sad_ll_pic_remap(i64, i64)

declare void @sad_ll_pic_set_mask(i64, i64)

declare void @sad_ll_port_inb(i64)

declare void @sad_ll_port_outb(i64, i64)

declare void @sad_ll_serial_init(i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_timer_init(i64)

declare void @sad_ll_timer_get_ticks()

define void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_0

while_body_1:                                     ; preds = %while_cond_0
  %"%\D8\B7.load2" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load2", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_0

while_cond_0:                                     ; preds = %while_body_1, %entry
  %arr.ptr.int = load i64, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 0
  %arr.len = load i64, ptr %arr.len.gep, align 8
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", %arr.len
  br i1 %cmplttmp, label %while_body_1, label %while_exit_2

while_exit_2:                                     ; preds = %while_cond_0
  ret void
}

define void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D8\B1\D9\82\D8\A7\D9\85" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85" = alloca i64, align 8
  %"\D9\82\D9\8A\D9\85\D8\A91" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_3, label %merge_4

merge_4:                                          ; preds = %entry
  %"%\D9\82\D9\8A\D9\85\D8\A9.load2" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load2", 0
  br i1 %cmplttmp, label %then_5, label %merge_6

merge_6:                                          ; preds = %then_5, %merge_4
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 20, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 20, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 20))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep3 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data4 = load ptr, ptr %arr.data.gep3, align 8
  %arr.elem = getelementptr i64, ptr %arr.data4, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data.gep5 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data6 = load ptr, ptr %arr.data.gep5, align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 1
  store i64 0, ptr %arr.elem7, align 8
  %arr.data.gep8 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data9 = load ptr, ptr %arr.data.gep8, align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 2
  store i64 0, ptr %arr.elem10, align 8
  %arr.data.gep11 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data12 = load ptr, ptr %arr.data.gep11, align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 3
  store i64 0, ptr %arr.elem13, align 8
  %arr.data.gep14 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data15 = load ptr, ptr %arr.data.gep14, align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 4
  store i64 0, ptr %arr.elem16, align 8
  %arr.data.gep17 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data18 = load ptr, ptr %arr.data.gep17, align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 5
  store i64 0, ptr %arr.elem19, align 8
  %arr.data.gep20 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data21 = load ptr, ptr %arr.data.gep20, align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 6
  store i64 0, ptr %arr.elem22, align 8
  %arr.data.gep23 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data24 = load ptr, ptr %arr.data.gep23, align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 7
  store i64 0, ptr %arr.elem25, align 8
  %arr.data.gep26 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data27 = load ptr, ptr %arr.data.gep26, align 8
  %arr.elem28 = getelementptr i64, ptr %arr.data27, i64 8
  store i64 0, ptr %arr.elem28, align 8
  %arr.data.gep29 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data30 = load ptr, ptr %arr.data.gep29, align 8
  %arr.elem31 = getelementptr i64, ptr %arr.data30, i64 9
  store i64 0, ptr %arr.elem31, align 8
  %arr.data.gep32 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data33 = load ptr, ptr %arr.data.gep32, align 8
  %arr.elem34 = getelementptr i64, ptr %arr.data33, i64 10
  store i64 0, ptr %arr.elem34, align 8
  %arr.data.gep35 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data36 = load ptr, ptr %arr.data.gep35, align 8
  %arr.elem37 = getelementptr i64, ptr %arr.data36, i64 11
  store i64 0, ptr %arr.elem37, align 8
  %arr.data.gep38 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data39 = load ptr, ptr %arr.data.gep38, align 8
  %arr.elem40 = getelementptr i64, ptr %arr.data39, i64 12
  store i64 0, ptr %arr.elem40, align 8
  %arr.data.gep41 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data42 = load ptr, ptr %arr.data.gep41, align 8
  %arr.elem43 = getelementptr i64, ptr %arr.data42, i64 13
  store i64 0, ptr %arr.elem43, align 8
  %arr.data.gep44 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data45 = load ptr, ptr %arr.data.gep44, align 8
  %arr.elem46 = getelementptr i64, ptr %arr.data45, i64 14
  store i64 0, ptr %arr.elem46, align 8
  %arr.data.gep47 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data48 = load ptr, ptr %arr.data.gep47, align 8
  %arr.elem49 = getelementptr i64, ptr %arr.data48, i64 15
  store i64 0, ptr %arr.elem49, align 8
  %arr.data.gep50 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data51 = load ptr, ptr %arr.data.gep50, align 8
  %arr.elem52 = getelementptr i64, ptr %arr.data51, i64 16
  store i64 0, ptr %arr.elem52, align 8
  %arr.data.gep53 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data54 = load ptr, ptr %arr.data.gep53, align 8
  %arr.elem55 = getelementptr i64, ptr %arr.data54, i64 17
  store i64 0, ptr %arr.elem55, align 8
  %arr.data.gep56 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data57 = load ptr, ptr %arr.data.gep56, align 8
  %arr.elem58 = getelementptr i64, ptr %arr.data57, i64 18
  store i64 0, ptr %arr.elem58, align 8
  %arr.data.gep59 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data60 = load ptr, ptr %arr.data.gep59, align 8
  %arr.elem61 = getelementptr i64, ptr %arr.data60, i64 19
  store i64 0, ptr %arr.elem61, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  br label %while_cond_7

then_3:                                           ; preds = %entry
  ret void

then_5:                                           ; preds = %merge_4
  br label %merge_6

while_body_11:                                    ; preds = %while_cond_10
  %"%\D8\B7.load70" = load i64, ptr %"%\D8\B7", align 8
  %subtmp71 = sub i64 %"%\D8\B7.load70", 1
  store i64 %subtmp71, ptr %"%\D8\B7", align 8
  br label %while_cond_10

while_body_8:                                     ; preds = %while_cond_7
  %"%\D9\82\D9\8A\D9\85\D8\A9.load63" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %modtmp = srem i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load63", 10
  %addtmp = add i64 %modtmp, 48
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D8\B1\D9\82\D8\A7\D9\85.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %arr.ptr.int = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.data.gep64 = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 2
  %arr.data65 = load ptr, ptr %arr.data.gep64, align 8
  %arr.elem66 = getelementptr i64, ptr %arr.data65, i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D8\B1\D9\82\D8\A7\D9\85.load"
  store i64 %addtmp, ptr %arr.elem66, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D8\B1\D9\82\D8\A7\D9\85.load67" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %addtmp68 = add i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D8\B1\D9\82\D8\A7\D9\85.load67", 1
  store i64 %addtmp68, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  br label %while_cond_7

while_cond_10:                                    ; preds = %while_body_11, %while_exit_9
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B7.load", 0
  br i1 %cmpgetmp, label %while_body_11, label %while_exit_12

while_cond_7:                                     ; preds = %while_body_8, %merge_6
  %"%\D9\82\D9\8A\D9\85\D8\A9.load62" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load62", 0
  br i1 %cmpgttmp, label %while_body_8, label %while_exit_9

while_exit_12:                                    ; preds = %while_cond_10
  ret void

while_exit_9:                                     ; preds = %while_cond_7
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D8\B1\D9\82\D8\A7\D9\85.load69" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %subtmp = sub i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D8\B1\D9\82\D8\A7\D9\85.load69", 1
  store i64 %subtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_10
}

define void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B3\D8\AA_\D8\B9\D8\B4\D8\B1\D9\8A"(i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"%\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"%\D8\B7" = alloca i64, align 8
  %"%\D8\A8\D8\A7\D9\82\D9\8A" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85" = alloca i64, align 8
  %"%\D8\A3\D8\AD\D8\B1\D9\81_\D8\B3\D8\AA_\D8\B9\D8\B4\D8\B1\D9\8A\D8\A9" = alloca ptr, align 8
  %"\D9\82\D9\8A\D9\85\D8\A91" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  store ptr @str.const, ptr %"%\D8\A3\D8\AD\D8\B1\D9\81_\D8\B3\D8\AA_\D8\B9\D8\B4\D8\B1\D9\8A\D8\A9", align 8
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 16, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep2 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data3 = load ptr, ptr %arr.data.gep2, align 8
  %arr.elem = getelementptr i64, ptr %arr.data3, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data.gep4 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data5 = load ptr, ptr %arr.data.gep4, align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 1
  store i64 0, ptr %arr.elem6, align 8
  %arr.data.gep7 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data8 = load ptr, ptr %arr.data.gep7, align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 2
  store i64 0, ptr %arr.elem9, align 8
  %arr.data.gep10 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data11 = load ptr, ptr %arr.data.gep10, align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 3
  store i64 0, ptr %arr.elem12, align 8
  %arr.data.gep13 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data14 = load ptr, ptr %arr.data.gep13, align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 4
  store i64 0, ptr %arr.elem15, align 8
  %arr.data.gep16 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data17 = load ptr, ptr %arr.data.gep16, align 8
  %arr.elem18 = getelementptr i64, ptr %arr.data17, i64 5
  store i64 0, ptr %arr.elem18, align 8
  %arr.data.gep19 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data20 = load ptr, ptr %arr.data.gep19, align 8
  %arr.elem21 = getelementptr i64, ptr %arr.data20, i64 6
  store i64 0, ptr %arr.elem21, align 8
  %arr.data.gep22 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data23 = load ptr, ptr %arr.data.gep22, align 8
  %arr.elem24 = getelementptr i64, ptr %arr.data23, i64 7
  store i64 0, ptr %arr.elem24, align 8
  %arr.data.gep25 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data26 = load ptr, ptr %arr.data.gep25, align 8
  %arr.elem27 = getelementptr i64, ptr %arr.data26, i64 8
  store i64 0, ptr %arr.elem27, align 8
  %arr.data.gep28 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data29 = load ptr, ptr %arr.data.gep28, align 8
  %arr.elem30 = getelementptr i64, ptr %arr.data29, i64 9
  store i64 0, ptr %arr.elem30, align 8
  %arr.data.gep31 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data32 = load ptr, ptr %arr.data.gep31, align 8
  %arr.elem33 = getelementptr i64, ptr %arr.data32, i64 10
  store i64 0, ptr %arr.elem33, align 8
  %arr.data.gep34 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data35 = load ptr, ptr %arr.data.gep34, align 8
  %arr.elem36 = getelementptr i64, ptr %arr.data35, i64 11
  store i64 0, ptr %arr.elem36, align 8
  %arr.data.gep37 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data38 = load ptr, ptr %arr.data.gep37, align 8
  %arr.elem39 = getelementptr i64, ptr %arr.data38, i64 12
  store i64 0, ptr %arr.elem39, align 8
  %arr.data.gep40 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data41 = load ptr, ptr %arr.data.gep40, align 8
  %arr.elem42 = getelementptr i64, ptr %arr.data41, i64 13
  store i64 0, ptr %arr.elem42, align 8
  %arr.data.gep43 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data44 = load ptr, ptr %arr.data.gep43, align 8
  %arr.elem45 = getelementptr i64, ptr %arr.data44, i64 14
  store i64 0, ptr %arr.elem45, align 8
  %arr.data.gep46 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data47 = load ptr, ptr %arr.data.gep46, align 8
  %arr.elem48 = getelementptr i64, ptr %arr.data47, i64 15
  store i64 0, ptr %arr.elem48, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_13, label %merge_14

else_23:                                          ; preds = %while_body_19
  br label %merge_22

merge_14:                                         ; preds = %entry
  br label %while_cond_15

merge_22:                                         ; preds = %else_23, %then_21
  %"%\D8\B7.load62" = load i64, ptr %"%\D8\B7", align 8
  %subtmp63 = sub i64 %"%\D8\B7.load62", 1
  store i64 %subtmp63, ptr %"%\D8\B7", align 8
  br label %while_cond_18

then_13:                                          ; preds = %entry
  ret void

then_21:                                          ; preds = %while_body_19
  br label %merge_22

while_body_16:                                    ; preds = %while_cond_15
  %"%\D9\82\D9\8A\D9\85\D8\A9.load50" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %andtmp = and i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load50", 15
  store i64 %andtmp, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A", align 8
  %"%\D8\B9\D8\AF\D8\AF.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %"%\D8\A8\D8\A7\D9\82\D9\8A.load" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A", align 8
  %arr.ptr.int = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.data.gep51 = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 2
  %arr.data52 = load ptr, ptr %arr.data.gep51, align 8
  %arr.elem53 = getelementptr i64, ptr %arr.data52, i64 %"%\D8\B9\D8\AF\D8\AF.load"
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A.load", ptr %arr.elem53, align 8
  %"%\D8\B9\D8\AF\D8\AF.load54" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\AF.load54", 1
  store i64 %addtmp, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  br label %while_cond_15

while_body_19:                                    ; preds = %while_cond_18
  %"%\D8\B7.load56" = load i64, ptr %"%\D8\B7", align 8
  %arr.ptr.int57 = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %arr.ptr58 = inttoptr i64 %arr.ptr.int57 to ptr
  %arr.data.gep59 = getelementptr inbounds %SadArray, ptr %arr.ptr58, i32 0, i32 2
  %arr.data60 = load ptr, ptr %arr.data.gep59, align 8
  %arr.elem61 = getelementptr i64, ptr %arr.data60, i64 %"%\D8\B7.load56"
  %arr.get = load i64, ptr %arr.elem61, align 8
  store i64 %arr.get, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 10
  br i1 %cmplttmp, label %then_21, label %else_23

while_cond_15:                                    ; preds = %while_body_16, %merge_14
  %"%\D9\82\D9\8A\D9\85\D8\A9.load49" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load49", 0
  br i1 %cmpgttmp, label %while_body_16, label %while_exit_17

while_cond_18:                                    ; preds = %merge_22, %while_exit_17
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B7.load", 0
  br i1 %cmpgetmp, label %while_body_19, label %while_exit_20

while_exit_17:                                    ; preds = %while_cond_15
  %"%\D8\B9\D8\AF\D8\AF.load55" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %subtmp = sub i64 %"%\D8\B9\D8\AF\D8\AF.load55", 1
  store i64 %subtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_18

while_exit_20:                                    ; preds = %while_cond_18
  ret void
}

define void @"\D9\85\D8\B9\D8\A7\D9\84\D8\AC_\D9\82\D8\B3\D9\85\D8\A9_\D8\B9\D9\84\D9\89_\D8\B5\D9\81\D8\B1"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  br label %while_cond_24

while_body_25:                                    ; preds = %while_cond_24
  br label %while_cond_24

while_cond_24:                                    ; preds = %while_body_25, %entry
  br i1 true, label %while_body_25, label %while_exit_26

while_exit_26:                                    ; preds = %while_cond_24
  ret void
}

define void @"\D9\85\D8\B9\D8\A7\D9\84\D8\AC_\D8\AA\D9\86\D9\82\D9\8A\D8\AD"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  ret void
}

define void @"\D9\85\D8\B9\D8\A7\D9\84\D8\AC_\D9\86\D9\82\D8\B7\D8\A9_\D8\AA\D9\88\D9\82\D9\81"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  ret void
}

define void @"\D9\85\D8\B9\D8\A7\D9\84\D8\AC_\D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A9_\D8\BA\D9\8A\D8\B1_\D8\B5\D8\A7\D9\84\D8\AD\D8\A9"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  br label %while_cond_27

while_body_28:                                    ; preds = %while_cond_27
  br label %while_cond_27

while_cond_27:                                    ; preds = %while_body_28, %entry
  br i1 true, label %while_body_28, label %while_exit_29

while_exit_29:                                    ; preds = %while_cond_27
  ret void
}

define void @"\D9\85\D8\B9\D8\A7\D9\84\D8\AC_\D8\AE\D8\B7\D8\A3_\D9\85\D8\B2\D8\AF\D9\88\D8\AC"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.30 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.32 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.35 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.36 to i64))
  br label %while_cond_30

while_body_31:                                    ; preds = %while_cond_30
  br label %while_cond_30

while_cond_30:                                    ; preds = %while_body_31, %entry
  br i1 true, label %while_body_31, label %while_exit_32

while_exit_32:                                    ; preds = %while_cond_30
  ret void
}

define void @"\D9\85\D8\B9\D8\A7\D9\84\D8\AC_\D8\AE\D8\B7\D8\A3_\D8\AD\D9\85\D8\A7\D9\8A\D8\A9_\D8\B9\D8\A7\D9\85"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.38 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.39 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.40 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.41 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.42 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.43 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.44 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.45 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.46 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.47 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.48 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  br label %while_cond_33

while_body_34:                                    ; preds = %while_cond_33
  br label %while_cond_33

while_cond_33:                                    ; preds = %while_body_34, %entry
  br i1 true, label %while_body_34, label %while_exit_35

while_exit_35:                                    ; preds = %while_cond_33
  ret void
}

define void @"\D9\85\D8\B9\D8\A7\D9\84\D8\AC_\D8\AE\D8\B7\D8\A3_\D8\B5\D9\81\D8\AD\D8\A9"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.38 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.52 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.40 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.54 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.42 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.56 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.57 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.58 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.59 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.60 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.61 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.47 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.63 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  br label %while_cond_36

while_body_37:                                    ; preds = %while_cond_36
  br label %while_cond_36

while_cond_36:                                    ; preds = %while_body_37, %entry
  br i1 true, label %while_body_37, label %while_exit_38

while_exit_38:                                    ; preds = %while_cond_36
  ret void
}

define void @"\D9\85\D8\B9\D8\A7\D9\84\D8\AC_\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\B9\D8\A7\D9\85"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A11" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A11", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.65 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.66 to i64))
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A11", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.67 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.68 to i64))
  br label %while_cond_39

while_body_40:                                    ; preds = %while_cond_39
  br label %while_cond_39

while_cond_39:                                    ; preds = %while_body_40, %entry
  br i1 true, label %while_body_40, label %while_exit_41

while_exit_41:                                    ; preds = %while_cond_39
  ret void
}

define void @"\D9\85\D8\B9\D8\A7\D9\84\D8\AC_\D8\A7\D9\84\D9\85\D8\A4\D9\82\D8\AA"() {
entry:
  br i1 true, label %then_42, label %merge_43

merge_43:                                         ; preds = %then_42, %entry
  ret void

then_42:                                          ; preds = %entry
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.69 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  br label %merge_43
}

define void @"\D9\85\D8\B9\D8\A7\D9\84\D8\AC_\D9\84\D9\88\D8\AD\D8\A9_\D8\A7\D9\84\D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD"() {
entry:
  %"%\D8\AD\D8\B1\D9\81" = alloca i64, align 8
  %"%scancode" = alloca i64, align 8
  %"%scancode.load" = load i64, ptr %"%scancode", align 8
  %cmplttmp = icmp slt i64 %"%scancode.load", 128
  br i1 %cmplttmp, label %then_44, label %merge_45

merge_45:                                         ; preds = %merge_49, %entry
  ret void

merge_47:                                         ; preds = %then_46, %then_44
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %"%scancode.load3" = load i64, ptr %"%scancode", align 8
  %cmpeqtmp = icmp eq i64 %"%scancode.load3", 1
  br i1 %cmpeqtmp, label %then_48, label %merge_49

merge_49:                                         ; preds = %then_48, %merge_47
  br label %merge_45

then_44:                                          ; preds = %entry
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.71 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.72 to i64))
  %"%scancode.load1" = load i64, ptr %"%scancode", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B3\D8\AA_\D8\B9\D8\B4\D8\B1\D9\8A"(i64 %"%scancode.load1")
  %"%scancode.load2" = load i64, ptr %"%scancode", align 8
  %calltmp = call i64 @"\D8\AD\D9\88\D9\84_scancode"(i64 %"%scancode.load2")
  store i64 %calltmp, ptr %"%\D8\AD\D8\B1\D9\81", align 8
  %"%\D8\AD\D8\B1\D9\81.load" = load i64, ptr %"%\D8\AD\D8\B1\D9\81", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\AD\D8\B1\D9\81.load", 0
  br i1 %cmpgttmp, label %then_46, label %merge_47

then_46:                                          ; preds = %then_44
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.73 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.74 to i64))
  br label %merge_47

then_48:                                          ; preds = %merge_47
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.76 to i64))
  br label %merge_49
}

define i64 @"\D8\AD\D9\88\D9\84_scancode"(i64 %scancode) {
entry:
  %scancode1 = alloca i64, align 8
  store i64 %scancode, ptr %scancode1, align 8
  %"%scancode.load" = load i64, ptr %scancode1, align 8
  %cmpeqtmp = icmp eq i64 %"%scancode.load", 2
  br i1 %cmpeqtmp, label %then_50, label %merge_51

merge_101:                                        ; preds = %merge_99
  %"%scancode.load52" = load i64, ptr %scancode1, align 8
  %cmpeqtmp53 = icmp eq i64 %"%scancode.load52", 36
  br i1 %cmpeqtmp53, label %then_102, label %merge_103

merge_103:                                        ; preds = %merge_101
  %"%scancode.load54" = load i64, ptr %scancode1, align 8
  %cmpeqtmp55 = icmp eq i64 %"%scancode.load54", 37
  br i1 %cmpeqtmp55, label %then_104, label %merge_105

merge_105:                                        ; preds = %merge_103
  %"%scancode.load56" = load i64, ptr %scancode1, align 8
  %cmpeqtmp57 = icmp eq i64 %"%scancode.load56", 38
  br i1 %cmpeqtmp57, label %then_106, label %merge_107

merge_107:                                        ; preds = %merge_105
  %"%scancode.load58" = load i64, ptr %scancode1, align 8
  %cmpeqtmp59 = icmp eq i64 %"%scancode.load58", 44
  br i1 %cmpeqtmp59, label %then_108, label %merge_109

merge_109:                                        ; preds = %merge_107
  %"%scancode.load60" = load i64, ptr %scancode1, align 8
  %cmpeqtmp61 = icmp eq i64 %"%scancode.load60", 45
  br i1 %cmpeqtmp61, label %then_110, label %merge_111

merge_111:                                        ; preds = %merge_109
  %"%scancode.load62" = load i64, ptr %scancode1, align 8
  %cmpeqtmp63 = icmp eq i64 %"%scancode.load62", 46
  br i1 %cmpeqtmp63, label %then_112, label %merge_113

merge_113:                                        ; preds = %merge_111
  %"%scancode.load64" = load i64, ptr %scancode1, align 8
  %cmpeqtmp65 = icmp eq i64 %"%scancode.load64", 47
  br i1 %cmpeqtmp65, label %then_114, label %merge_115

merge_115:                                        ; preds = %merge_113
  %"%scancode.load66" = load i64, ptr %scancode1, align 8
  %cmpeqtmp67 = icmp eq i64 %"%scancode.load66", 48
  br i1 %cmpeqtmp67, label %then_116, label %merge_117

merge_117:                                        ; preds = %merge_115
  %"%scancode.load68" = load i64, ptr %scancode1, align 8
  %cmpeqtmp69 = icmp eq i64 %"%scancode.load68", 49
  br i1 %cmpeqtmp69, label %then_118, label %merge_119

merge_119:                                        ; preds = %merge_117
  %"%scancode.load70" = load i64, ptr %scancode1, align 8
  %cmpeqtmp71 = icmp eq i64 %"%scancode.load70", 50
  br i1 %cmpeqtmp71, label %then_120, label %merge_121

merge_121:                                        ; preds = %merge_119
  %"%scancode.load72" = load i64, ptr %scancode1, align 8
  %cmpeqtmp73 = icmp eq i64 %"%scancode.load72", 28
  br i1 %cmpeqtmp73, label %then_122, label %merge_123

merge_123:                                        ; preds = %merge_121
  %"%scancode.load74" = load i64, ptr %scancode1, align 8
  %cmpeqtmp75 = icmp eq i64 %"%scancode.load74", 57
  br i1 %cmpeqtmp75, label %then_124, label %merge_125

merge_125:                                        ; preds = %merge_123
  %"%scancode.load76" = load i64, ptr %scancode1, align 8
  %cmpeqtmp77 = icmp eq i64 %"%scancode.load76", 14
  br i1 %cmpeqtmp77, label %then_126, label %merge_127

merge_127:                                        ; preds = %merge_125
  %"%scancode.load78" = load i64, ptr %scancode1, align 8
  %cmpeqtmp79 = icmp eq i64 %"%scancode.load78", 15
  br i1 %cmpeqtmp79, label %then_128, label %merge_129

merge_129:                                        ; preds = %merge_127
  ret i64 0

merge_51:                                         ; preds = %entry
  %"%scancode.load2" = load i64, ptr %scancode1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%scancode.load2", 3
  br i1 %cmpeqtmp3, label %then_52, label %merge_53

merge_53:                                         ; preds = %merge_51
  %"%scancode.load4" = load i64, ptr %scancode1, align 8
  %cmpeqtmp5 = icmp eq i64 %"%scancode.load4", 4
  br i1 %cmpeqtmp5, label %then_54, label %merge_55

merge_55:                                         ; preds = %merge_53
  %"%scancode.load6" = load i64, ptr %scancode1, align 8
  %cmpeqtmp7 = icmp eq i64 %"%scancode.load6", 5
  br i1 %cmpeqtmp7, label %then_56, label %merge_57

merge_57:                                         ; preds = %merge_55
  %"%scancode.load8" = load i64, ptr %scancode1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%scancode.load8", 6
  br i1 %cmpeqtmp9, label %then_58, label %merge_59

merge_59:                                         ; preds = %merge_57
  %"%scancode.load10" = load i64, ptr %scancode1, align 8
  %cmpeqtmp11 = icmp eq i64 %"%scancode.load10", 7
  br i1 %cmpeqtmp11, label %then_60, label %merge_61

merge_61:                                         ; preds = %merge_59
  %"%scancode.load12" = load i64, ptr %scancode1, align 8
  %cmpeqtmp13 = icmp eq i64 %"%scancode.load12", 8
  br i1 %cmpeqtmp13, label %then_62, label %merge_63

merge_63:                                         ; preds = %merge_61
  %"%scancode.load14" = load i64, ptr %scancode1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%scancode.load14", 9
  br i1 %cmpeqtmp15, label %then_64, label %merge_65

merge_65:                                         ; preds = %merge_63
  %"%scancode.load16" = load i64, ptr %scancode1, align 8
  %cmpeqtmp17 = icmp eq i64 %"%scancode.load16", 10
  br i1 %cmpeqtmp17, label %then_66, label %merge_67

merge_67:                                         ; preds = %merge_65
  %"%scancode.load18" = load i64, ptr %scancode1, align 8
  %cmpeqtmp19 = icmp eq i64 %"%scancode.load18", 11
  br i1 %cmpeqtmp19, label %then_68, label %merge_69

merge_69:                                         ; preds = %merge_67
  %"%scancode.load20" = load i64, ptr %scancode1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%scancode.load20", 16
  br i1 %cmpeqtmp21, label %then_70, label %merge_71

merge_71:                                         ; preds = %merge_69
  %"%scancode.load22" = load i64, ptr %scancode1, align 8
  %cmpeqtmp23 = icmp eq i64 %"%scancode.load22", 17
  br i1 %cmpeqtmp23, label %then_72, label %merge_73

merge_73:                                         ; preds = %merge_71
  %"%scancode.load24" = load i64, ptr %scancode1, align 8
  %cmpeqtmp25 = icmp eq i64 %"%scancode.load24", 18
  br i1 %cmpeqtmp25, label %then_74, label %merge_75

merge_75:                                         ; preds = %merge_73
  %"%scancode.load26" = load i64, ptr %scancode1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%scancode.load26", 19
  br i1 %cmpeqtmp27, label %then_76, label %merge_77

merge_77:                                         ; preds = %merge_75
  %"%scancode.load28" = load i64, ptr %scancode1, align 8
  %cmpeqtmp29 = icmp eq i64 %"%scancode.load28", 20
  br i1 %cmpeqtmp29, label %then_78, label %merge_79

merge_79:                                         ; preds = %merge_77
  %"%scancode.load30" = load i64, ptr %scancode1, align 8
  %cmpeqtmp31 = icmp eq i64 %"%scancode.load30", 21
  br i1 %cmpeqtmp31, label %then_80, label %merge_81

merge_81:                                         ; preds = %merge_79
  %"%scancode.load32" = load i64, ptr %scancode1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%scancode.load32", 22
  br i1 %cmpeqtmp33, label %then_82, label %merge_83

merge_83:                                         ; preds = %merge_81
  %"%scancode.load34" = load i64, ptr %scancode1, align 8
  %cmpeqtmp35 = icmp eq i64 %"%scancode.load34", 23
  br i1 %cmpeqtmp35, label %then_84, label %merge_85

merge_85:                                         ; preds = %merge_83
  %"%scancode.load36" = load i64, ptr %scancode1, align 8
  %cmpeqtmp37 = icmp eq i64 %"%scancode.load36", 24
  br i1 %cmpeqtmp37, label %then_86, label %merge_87

merge_87:                                         ; preds = %merge_85
  %"%scancode.load38" = load i64, ptr %scancode1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%scancode.load38", 25
  br i1 %cmpeqtmp39, label %then_88, label %merge_89

merge_89:                                         ; preds = %merge_87
  %"%scancode.load40" = load i64, ptr %scancode1, align 8
  %cmpeqtmp41 = icmp eq i64 %"%scancode.load40", 30
  br i1 %cmpeqtmp41, label %then_90, label %merge_91

merge_91:                                         ; preds = %merge_89
  %"%scancode.load42" = load i64, ptr %scancode1, align 8
  %cmpeqtmp43 = icmp eq i64 %"%scancode.load42", 31
  br i1 %cmpeqtmp43, label %then_92, label %merge_93

merge_93:                                         ; preds = %merge_91
  %"%scancode.load44" = load i64, ptr %scancode1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%scancode.load44", 32
  br i1 %cmpeqtmp45, label %then_94, label %merge_95

merge_95:                                         ; preds = %merge_93
  %"%scancode.load46" = load i64, ptr %scancode1, align 8
  %cmpeqtmp47 = icmp eq i64 %"%scancode.load46", 33
  br i1 %cmpeqtmp47, label %then_96, label %merge_97

merge_97:                                         ; preds = %merge_95
  %"%scancode.load48" = load i64, ptr %scancode1, align 8
  %cmpeqtmp49 = icmp eq i64 %"%scancode.load48", 34
  br i1 %cmpeqtmp49, label %then_98, label %merge_99

merge_99:                                         ; preds = %merge_97
  %"%scancode.load50" = load i64, ptr %scancode1, align 8
  %cmpeqtmp51 = icmp eq i64 %"%scancode.load50", 35
  br i1 %cmpeqtmp51, label %then_100, label %merge_101

then_100:                                         ; preds = %merge_99
  ret i64 104

then_102:                                         ; preds = %merge_101
  ret i64 106

then_104:                                         ; preds = %merge_103
  ret i64 107

then_106:                                         ; preds = %merge_105
  ret i64 108

then_108:                                         ; preds = %merge_107
  ret i64 122

then_110:                                         ; preds = %merge_109
  ret i64 120

then_112:                                         ; preds = %merge_111
  ret i64 99

then_114:                                         ; preds = %merge_113
  ret i64 118

then_116:                                         ; preds = %merge_115
  ret i64 98

then_118:                                         ; preds = %merge_117
  ret i64 110

then_120:                                         ; preds = %merge_119
  ret i64 109

then_122:                                         ; preds = %merge_121
  ret i64 10

then_124:                                         ; preds = %merge_123
  ret i64 32

then_126:                                         ; preds = %merge_125
  ret i64 8

then_128:                                         ; preds = %merge_127
  ret i64 9

then_50:                                          ; preds = %entry
  ret i64 49

then_52:                                          ; preds = %merge_51
  ret i64 50

then_54:                                          ; preds = %merge_53
  ret i64 51

then_56:                                          ; preds = %merge_55
  ret i64 52

then_58:                                          ; preds = %merge_57
  ret i64 53

then_60:                                          ; preds = %merge_59
  ret i64 54

then_62:                                          ; preds = %merge_61
  ret i64 55

then_64:                                          ; preds = %merge_63
  ret i64 56

then_66:                                          ; preds = %merge_65
  ret i64 57

then_68:                                          ; preds = %merge_67
  ret i64 48

then_70:                                          ; preds = %merge_69
  ret i64 113

then_72:                                          ; preds = %merge_71
  ret i64 119

then_74:                                          ; preds = %merge_73
  ret i64 101

then_76:                                          ; preds = %merge_75
  ret i64 114

then_78:                                          ; preds = %merge_77
  ret i64 116

then_80:                                          ; preds = %merge_79
  ret i64 121

then_82:                                          ; preds = %merge_81
  ret i64 117

then_84:                                          ; preds = %merge_83
  ret i64 105

then_86:                                          ; preds = %merge_85
  ret i64 111

then_88:                                          ; preds = %merge_87
  ret i64 112

then_90:                                          ; preds = %merge_89
  ret i64 97

then_92:                                          ; preds = %merge_91
  ret i64 115

then_94:                                          ; preds = %merge_93
  ret i64 100

then_96:                                          ; preds = %merge_95
  ret i64 102

then_98:                                          ; preds = %merge_97
  ret i64 103
}

define void @"\D9\85\D8\B9\D8\A7\D9\84\D8\AC_\D8\AA\D8\B3\D9\84\D8\B3\D9\84\D9\8A"() {
entry:
  %"%\D8\A8\D9\8A\D8\A7\D9\86" = alloca i64, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.77 to i64))
  %"%\D8\A8\D9\8A\D8\A7\D9\86.load" = load i64, ptr %"%\D8\A8\D9\8A\D8\A7\D9\86", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B3\D8\AA_\D8\B9\D8\B4\D8\B1\D9\8A"(i64 %"%\D8\A8\D9\8A\D8\A7\D9\86.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  ret void
}

define void @"\D9\85\D8\B9\D8\A7\D9\84\D8\AC_\D8\A7\D9\84\D9\81\D8\A3\D8\B1\D8\A9"() {
entry:
  %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.79 to i64))
  %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load" = load i64, ptr %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B3\D8\AA_\D8\B9\D8\B4\D8\B1\D9\8A"(i64 %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  ret void
}

define void @"\D9\85\D8\B9\D8\A7\D9\84\D8\AC_\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A9_\D8\B2\D8\A7\D8\A6\D9\81\D8\A9"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.81 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  ret void
}

define void @"\D9\85\D8\B9\D8\A7\D9\84\D8\AC_\D8\A7\D8\B3\D8\AA\D8\AF\D8\B9\D8\A7\D8\A1_\D9\86\D8\B8\D8\A7\D9\85"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.83 to i64))
  ret void
}

define void @"\D9\87\D9\8A\D8\A6_\D9\86\D8\B8\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA"() {
entry:
  %"%\D9\82\D9\86\D8\A7\D8\B9_\D9\81\D8\B1\D8\B9\D9\8A" = alloca i64, align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A" = alloca i64, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.85 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.86 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.85 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.88 to i64))
  call void @sad_ll_interrupt_disable()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.89 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.90 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.91 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.92 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.93 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.94 to i64))
  call void @"\D8\B3\D8\AC\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1\D8\A7\D8\AA"()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.95 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.96 to i64))
  call void @"\D8\B3\D8\AC\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA_IRQ"()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.97 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.98 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.99 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.100 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.101 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.102 to i64))
  store i64 232, ptr %"%\D9\82\D9\86\D8\A7\D8\B9_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A", align 8
  store i64 239, ptr %"%\D9\82\D9\86\D8\A7\D8\B9_\D9\81\D8\B1\D8\B9\D9\8A", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A.load" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9_\D9\81\D8\B1\D8\B9\D9\8A.load" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9_\D9\81\D8\B1\D8\B9\D9\8A", align 8
  call void @sad_ll_pic_set_mask(i64 %"%\D9\82\D9\86\D8\A7\D8\B9_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A.load", i64 %"%\D9\82\D9\86\D8\A7\D8\B9_\D9\81\D8\B1\D8\B9\D9\8A.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.103 to i64))
  call void @sad_ll_interrupt_enable()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.104 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.85 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.107 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.85 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  ret void
}

define void @"\D8\B3\D8\AC\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1\D8\A7\D8\AA"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.110 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.111 to i64))
  ret void
}

define void @"\D8\B3\D8\AC\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA_IRQ"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.112 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.113 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.114 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.115 to i64))
  ret void
}

define i64 @"\D8\A3\D8\B9\D8\B7\D9\86\D9\8A_\D9\86\D8\A8\D8\B6\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\A4\D9\82\D8\AA"() {
entry:
  ret i64 0
}

define i64 @"\D8\A3\D8\B9\D8\B7\D9\86\D9\8A_\D8\A7\D9\84\D8\AB\D9\88\D8\A7\D9\86\D9\8A"() {
entry:
  ret i64 0
}

define i64 @"\D8\A3\D8\B9\D8\B7\D9\86\D9\8A_\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B6\D8\BA\D8\B7\D8\A7\D8\AA"() {
entry:
  ret i64 0
}

define i64 @"\D8\A3\D8\B9\D8\B7\D9\86\D9\8A_\D8\A2\D8\AE\D8\B1_\D9\85\D9\81\D8\AA\D8\A7\D8\AD"() {
entry:
  ret i64 0
}

define i64 @"\D9\87\D9\84_\D8\A7\D9\84\D9\86\D8\B8\D8\A7\D9\85_\D8\AC\D8\A7\D9\87\D8\B2"() {
entry:
  ret i64 0
}

define void @"\D8\A7\D8\B7\D8\A8\D8\B9_\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.117 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.118 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.119 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.120 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.122 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.124 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.126 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.128 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.130 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.132 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  ret void
}

define void @__sad_main() {
entry:
  %"%\D8\A7\D9\84\D9\86\D8\B8\D8\A7\D9\85_\D8\AC\D8\A7\D9\87\D8\B2" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D8\B2\D8\A7\D8\A6\D9\81\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1_\D8\A7\D9\84\D8\AD\D9\85\D8\A7\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD_\D8\A7\D8\AE\D9\8A\D8\B1_\D8\B6\D8\BA\D8\B7" = alloca i64, align 8
  %"%\D8\B6\D8\BA\D8\B7\D8\A7\D8\AA_\D9\84\D9\88\D8\AD\D8\A9_\D8\B9\D8\AF\D8\AF" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\86\D8\A8\D8\B6\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\A4\D9\82\D8\AA" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\AD\D8\A7\D9\84\D8\A9_\D9\84\D9\88\D8\AD\D8\A9" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D9\84\D9\88\D8\AD\D8\A9" = alloca i64, align 8
  %"%PIC_EOI" = alloca i64, align 8
  %"%PIC2_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"%PIC2_\D8\A3\D9\88\D8\A7\D9\85\D8\B1" = alloca i64, align 8
  %"%PIC1_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"%PIC1_\D8\A3\D9\88\D8\A7\D9\85\D8\B1" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D8\B3\D8\AA\D8\AF\D8\B9\D8\A7\D8\A1_\D9\86\D8\B8\D8\A7\D9\85" = alloca i64, align 8
  %"%IRQ_\D9\82\D8\B1\D8\B5_\D8\AB\D8\A7\D9\86\D9\88\D9\8A" = alloca i64, align 8
  %"%IRQ_\D9\82\D8\B1\D8\B5_\D8\A3\D9\88\D9\84\D9\8A" = alloca i64, align 8
  %"%IRQ_\D9\81\D8\A3\D8\B1\D8\A9" = alloca i64, align 8
  %"%IRQ_COM1" = alloca i64, align 8
  %"%IRQ_COM2" = alloca i64, align 8
  %"%IRQ_cascade" = alloca i64, align 8
  %"%IRQ_\D9\84\D9\88\D8\AD\D8\A9_\D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD" = alloca i64, align 8
  %"%IRQ_\D9\85\D8\A4\D9\82\D8\AA" = alloca i64, align 8
  %"%IRQ_\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D8\B1\D8\B9\D9\8A" = alloca i64, align 8
  %"%IRQ_\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AE\D8\B7\D8\A3_SIMD" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D9\81\D8\AD\D8\B5_\D8\A2\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D9\81\D8\AD\D8\B5_\D9\85\D8\AD\D8\A7\D8\B0\D8\A7\D8\A9" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AE\D8\B7\D8\A3_\D8\AD\D8\B3\D8\A7\D8\A8\D9\8A" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AE\D8\B7\D8\A3_\D8\B5\D9\81\D8\AD\D8\A9" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AD\D9\85\D8\A7\D9\8A\D8\A9_\D8\B9\D8\A7\D9\85" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AE\D8\B7\D8\A3_\D9\85\D9\83\D8\AF\D8\B3" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D9\85\D9\82\D8\B7\D8\B9_\D8\BA\D9\8A\D8\B1_\D9\85\D9\88\D8\AC\D9\88\D8\AF" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_TSS_\D8\BA\D9\8A\D8\B1_\D8\B5\D8\A7\D9\84\D8\AD" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AE\D8\B7\D8\A3_\D9\85\D8\B2\D8\AF\D9\88\D8\AC" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AC\D9\87\D8\A7\D8\B2_\D8\BA\D9\8A\D8\B1_\D9\85\D8\AA\D9\88\D9\81\D8\B1" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A9_\D8\BA\D9\8A\D8\B1_\D8\B5\D8\A7\D9\84\D8\AD\D8\A9" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AA\D8\AC\D8\A7\D9\88\D8\B2_\D8\AD\D8\AF\D9\88\D8\AF" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AA\D8\AC\D8\A7\D9\88\D8\B2" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D9\86\D9\82\D8\B7\D8\A9_\D8\AA\D9\88\D9\82\D9\81" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_NMI" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AA\D9\86\D9\82\D9\8A\D8\AD" = alloca i64, align 8
  %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D9\82\D8\B3\D9\85\D8\A9_\D8\B5\D9\81\D8\B1" = alloca i64, align 8
  store i64 0, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D9\82\D8\B3\D9\85\D8\A9_\D8\B5\D9\81\D8\B1", align 8
  store i64 1, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AA\D9\86\D9\82\D9\8A\D8\AD", align 8
  store i64 2, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_NMI", align 8
  store i64 3, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D9\86\D9\82\D8\B7\D8\A9_\D8\AA\D9\88\D9\82\D9\81", align 8
  store i64 4, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AA\D8\AC\D8\A7\D9\88\D8\B2", align 8
  store i64 5, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AA\D8\AC\D8\A7\D9\88\D8\B2_\D8\AD\D8\AF\D9\88\D8\AF", align 8
  store i64 6, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A9_\D8\BA\D9\8A\D8\B1_\D8\B5\D8\A7\D9\84\D8\AD\D8\A9", align 8
  store i64 7, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AC\D9\87\D8\A7\D8\B2_\D8\BA\D9\8A\D8\B1_\D9\85\D8\AA\D9\88\D9\81\D8\B1", align 8
  store i64 8, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AE\D8\B7\D8\A3_\D9\85\D8\B2\D8\AF\D9\88\D8\AC", align 8
  store i64 10, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_TSS_\D8\BA\D9\8A\D8\B1_\D8\B5\D8\A7\D9\84\D8\AD", align 8
  store i64 11, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D9\85\D9\82\D8\B7\D8\B9_\D8\BA\D9\8A\D8\B1_\D9\85\D9\88\D8\AC\D9\88\D8\AF", align 8
  store i64 12, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AE\D8\B7\D8\A3_\D9\85\D9\83\D8\AF\D8\B3", align 8
  store i64 13, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AD\D9\85\D8\A7\D9\8A\D8\A9_\D8\B9\D8\A7\D9\85", align 8
  store i64 14, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AE\D8\B7\D8\A3_\D8\B5\D9\81\D8\AD\D8\A9", align 8
  store i64 16, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AE\D8\B7\D8\A3_\D8\AD\D8\B3\D8\A7\D8\A8\D9\8A", align 8
  store i64 17, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D9\81\D8\AD\D8\B5_\D9\85\D8\AD\D8\A7\D8\B0\D8\A7\D8\A9", align 8
  store i64 18, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D9\81\D8\AD\D8\B5_\D8\A2\D9\84\D8\A9", align 8
  store i64 19, ptr %"%\D8\A7\D8\B3\D8\AA\D8\AB\D9\86\D8\A7\D8\A1_\D8\AE\D8\B7\D8\A3_SIMD", align 8
  store i64 32, ptr %"%IRQ_\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A", align 8
  store i64 40, ptr %"%IRQ_\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D8\B1\D8\B9\D9\8A", align 8
  store i64 32, ptr %"%IRQ_\D9\85\D8\A4\D9\82\D8\AA", align 8
  store i64 33, ptr %"%IRQ_\D9\84\D9\88\D8\AD\D8\A9_\D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD", align 8
  store i64 34, ptr %"%IRQ_cascade", align 8
  store i64 35, ptr %"%IRQ_COM2", align 8
  store i64 36, ptr %"%IRQ_COM1", align 8
  store i64 44, ptr %"%IRQ_\D9\81\D8\A3\D8\B1\D8\A9", align 8
  store i64 46, ptr %"%IRQ_\D9\82\D8\B1\D8\B5_\D8\A3\D9\88\D9\84\D9\8A", align 8
  store i64 47, ptr %"%IRQ_\D9\82\D8\B1\D8\B5_\D8\AB\D8\A7\D9\86\D9\88\D9\8A", align 8
  store i64 128, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D8\B3\D8\AA\D8\AF\D8\B9\D8\A7\D8\A1_\D9\86\D8\B8\D8\A7\D9\85", align 8
  store i64 32, ptr %"%PIC1_\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  store i64 33, ptr %"%PIC1_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  store i64 160, ptr %"%PIC2_\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  store i64 161, ptr %"%PIC2_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  store i64 32, ptr %"%PIC_EOI", align 8
  store i64 96, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D9\84\D9\88\D8\AD\D8\A9", align 8
  store i64 100, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\AD\D8\A7\D9\84\D8\A9_\D9\84\D9\88\D8\AD\D8\A9", align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\86\D8\A8\D8\B6\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\A4\D9\82\D8\AA", align 8
  store i64 0, ptr %"%\D8\B6\D8\BA\D8\B7\D8\A7\D8\AA_\D9\84\D9\88\D8\AD\D8\A9_\D8\B9\D8\AF\D8\AF", align 8
  store i64 0, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD_\D8\A7\D8\AE\D9\8A\D8\B1_\D8\B6\D8\BA\D8\B7", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1_\D8\A7\D9\84\D8\AD\D9\85\D8\A7\D9\8A\D8\A9", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D8\B2\D8\A7\D8\A6\D9\81\D8\A9", align 8
  store i64 0, ptr %"%\D8\A7\D9\84\D9\86\D8\B8\D8\A7\D9\85_\D8\AC\D8\A7\D9\87\D8\B2", align 8
  ret void
}

declare ptr @malloc(i64)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
