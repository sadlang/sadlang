; ModuleID = '05_complex_memory_system'
source_filename = "05_complex_memory_system"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [99 x i8] c"  \E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [26 x i8] c"  [\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5] \D9\83\D8\AA\D9\84\D8\A9 #\00", align 1
@str.const.4 = private unnamed_addr constant [3 x i8] c" (\00", align 1
@str.const.5 = private unnamed_addr constant [15 x i8] c" \D8\A8\D8\A7\D9\8A\D8\AA) -> \00", align 1
@str.ret = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.8 = private unnamed_addr constant [26 x i8] c"  [\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1] \D9\83\D8\AA\D9\84\D8\A9 #\00", align 1
@str.const.10 = private unnamed_addr constant [11 x i8] c" \D8\A8\D8\A7\D9\8A\D8\AA)\00", align 1
@str.ret.13 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.14 = private unnamed_addr constant [22 x i8] c"  [\D9\86\D9\82\D9\84] \D9\83\D8\AA\D9\84\D8\A9 #\00", align 1
@str.const.15 = private unnamed_addr constant [3 x i8] c": \00", align 1
@str.const.16 = private unnamed_addr constant [5 x i8] c" -> \00", align 1
@str.ret.19 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.20 = private unnamed_addr constant [30 x i8] c"  [\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9] \D9\83\D8\AA\D9\84\D8\A9 #\00", align 1
@str.const.21 = private unnamed_addr constant [5 x i8] c" <- \00", align 1
@str.const.22 = private unnamed_addr constant [14 x i8] c" (\D8\AA\D8\B9\D8\AF\D9\8A\D9\84)\00", align 1
@str.const.27 = private unnamed_addr constant [14 x i8] c" (\D9\82\D8\B1\D8\A7\D8\A1\D8\A9)\00", align 1
@str.ret.30 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.31 = private unnamed_addr constant [66 x i8] c"  \E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90 \D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9 \E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.35 = private unnamed_addr constant [26 x i8] c"  \E2\95\91 \D8\A7\D9\84\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A:  \00", align 1
@str.const.36 = private unnamed_addr constant [10 x i8] c" \D8\A8\D8\A7\D9\8A\D8\AA\00", align 1
@str.const.39 = private unnamed_addr constant [26 x i8] c"  \E2\95\91 \D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85:  \00", align 1
@str.const.43 = private unnamed_addr constant [24 x i8] c"  \E2\95\91 \D8\A7\D9\84\D9\85\D8\AA\D8\A7\D8\AD:    \00", align 1
@str.const.47 = private unnamed_addr constant [23 x i8] c"  \E2\95\91 \D8\A7\D9\84\D9\83\D8\AA\D9\84:     \00", align 1
@str.const.50 = private unnamed_addr constant [26 x i8] c"  \E2\95\91 \D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA:  \00", align 1
@str.const.53 = private unnamed_addr constant [90 x i8] c"  \E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.57 = private unnamed_addr constant [47 x i8] c"=== \D8\AA\D8\AE\D8\B5\D9\8A\D8\B5 \D9\88\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9 ===\00", align 1
@str.const.61 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.65 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.69 = private unnamed_addr constant [16 x i8] c"\D9\85\D8\B5\D9\81\D9\88\D9\81\D8\A9_\D8\A3\00", align 1
@str.const.70 = private unnamed_addr constant [18 x i8] c"\D9\85\D8\AE\D8\B2\D9\86_\D9\85\D8\A4\D9\82\D8\AA\00", align 1
@str.const.71 = private unnamed_addr constant [22 x i8] c"\D8\B5\D9\88\D8\B1\D8\A9_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA\00", align 1
@str.const.72 = private unnamed_addr constant [22 x i8] c"\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA_\D8\B1\D8\A3\D8\B3\00", align 1
@str.const.73 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.77 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.81 = private unnamed_addr constant [25 x i8] c"  \D8\A8\D8\AF\D8\A1 \D8\A7\D9\84\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1:\00", align 1
@str.const.85 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.89 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.93 = private unnamed_addr constant [41 x i8] c"=== \D9\86\D9\82\D9\84 \D9\85\D9\84\D9\83\D9\8A\D8\A9 \D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9 ===\00", align 1
@str.const.97 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.101 = private unnamed_addr constant [20 x i8] c"\D9\82\D8\A7\D8\B1\D8\A6_\D9\85\D9\84\D9\81\D8\A7\D8\AA\00", align 1
@str.const.103 = private unnamed_addr constant [24 x i8] c"\D9\85\D8\B9\D8\A7\D9\84\D8\AC_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA\00", align 1
@str.const.104 = private unnamed_addr constant [59 x i8] c"     \D9\82\D8\A7\D8\B1\D8\A6_\D9\85\D9\84\D9\81\D8\A7\D8\AA \D9\84\D9\85 \D9\8A\D8\B9\D8\AF \D9\8A\D9\85\D9\84\D9\83 \D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9\00", align 1
@str.const.108 = private unnamed_addr constant [13 x i8] c"\D9\85\D8\B1\D8\A7\D9\82\D8\A8_1\00", align 1
@str.const.109 = private unnamed_addr constant [13 x i8] c"\D9\85\D8\B1\D8\A7\D9\82\D8\A8_2\00", align 1
@str.const.110 = private unnamed_addr constant [46 x i8] c"     \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\AA\D8\A7 \D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D9\86\D8\B4\D8\B7\D8\AA\D8\A7\D9\86\00", align 1
@str.const.114 = private unnamed_addr constant [43 x i8] c"  [\D8\A5\D9\86\D9\87\D8\A7\D8\A1] \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D9\85\D8\B1\D8\A7\D9\82\D8\A8_2\00", align 1
@str.const.118 = private unnamed_addr constant [43 x i8] c"  [\D8\A5\D9\86\D9\87\D8\A7\D8\A1] \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D9\85\D8\B1\D8\A7\D9\82\D8\A8_1\00", align 1
@str.const.122 = private unnamed_addr constant [9 x i8] c"\D9\85\D8\AD\D8\B1\D8\B1\00", align 1
@str.const.123 = private unnamed_addr constant [77 x i8] c"     \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D8\AA\D8\B9\D8\AF\D9\8A\D9\84 \D9\85\D9\85\D9\83\D9\86\D8\A9 \D8\A8\D8\B9\D8\AF \D8\A5\D9\86\D9\87\D8\A7\D8\A1 \D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A7\D8\AA\00", align 1
@str.const.127 = private unnamed_addr constant [39 x i8] c"  [\D8\A5\D9\86\D9\87\D8\A7\D8\A1] \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D9\85\D8\AD\D8\B1\D8\B1\00", align 1
@str.const.132 = private unnamed_addr constant [20 x i8] c"\D9\83\D8\A7\D8\AA\D8\A8_\D9\85\D9\84\D9\81\D8\A7\D8\AA\00", align 1
@str.const.133 = private unnamed_addr constant [53 x i8] c"     \D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83 \D8\A7\D9\84\D8\A3\D8\AE\D9\8A\D8\B1 \D8\AD\D8\B1\D8\B1 \D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9\00", align 1
@str.const.137 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.141 = private unnamed_addr constant [34 x i8] c"  \D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A \D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA: \00", align 1
@str.const.144 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.148 = private unnamed_addr constant [39 x i8] c"=== \D9\83\D8\B4\D9\81 \D8\AA\D8\B3\D8\B1\D8\A8 \D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9 ===\00", align 1
@str.const.152 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.156 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\A4\D9\82\D8\AA_1\00", align 1
@str.const.157 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\A4\D9\82\D8\AA_2\00", align 1
@str.const.158 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\A4\D9\82\D8\AA_3\00", align 1
@str.const.159 = private unnamed_addr constant [11 x i8] c"\D8\AF\D8\A7\D8\A6\D9\85_1\00", align 1
@str.const.160 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.164 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.168 = private unnamed_addr constant [27 x i8] c"  \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D8\AA\D8\B3\D8\B1\D8\A8:\00", align 1
@str.const.172 = private unnamed_addr constant [21 x i8] c"    \D8\AA\D8\AE\D8\B5\D9\8A\D8\B5\D8\A7\D8\AA: \00", align 1
@str.const.175 = private unnamed_addr constant [21 x i8] c"    \D8\AA\D8\AD\D8\B1\D9\8A\D8\B1\D8\A7\D8\AA: \00", align 1
@str.const.178 = private unnamed_addr constant [18 x i8] c"    [\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1] \00", align 1
@str.const.179 = private unnamed_addr constant [23 x i8] c" \D9\83\D8\AA\D9\84 \D9\84\D9\85 \D8\AA\D8\AD\D8\B1\D8\B1!\00", align 1
@str.const.182 = private unnamed_addr constant [33 x i8] c"    \D8\A7\D9\84\D9\83\D8\AA\D9\84 \D8\A7\D9\84\D9\85\D8\AA\D8\B3\D8\B1\D8\A8\D8\A9:\00", align 1
@str.const.186 = private unnamed_addr constant [49 x i8] c"      - \D9\83\D8\AA\D9\84\D8\A9 #21 (200 \D8\A8\D8\A7\D9\8A\D8\AA) - \D9\85\D8\A4\D9\82\D8\AA_2\00", align 1
@str.const.190 = private unnamed_addr constant [49 x i8] c"      - \D9\83\D8\AA\D9\84\D8\A9 #23 (400 \D8\A8\D8\A7\D9\8A\D8\AA) - \D8\AF\D8\A7\D8\A6\D9\85_1\00", align 1
@str.const.194 = private unnamed_addr constant [44 x i8] c"    \D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A \D8\A7\D9\84\D8\AA\D8\B3\D8\B1\D8\A8: 600 \D8\A8\D8\A7\D9\8A\D8\AA\00", align 1
@str.const.198 = private unnamed_addr constant [40 x i8] c"    [\D9\86\D8\AC\D8\A7\D8\AD] \D9\84\D8\A7 \D8\AA\D8\B3\D8\B1\D8\A8 \D8\B0\D8\A7\D9\83\D8\B1\D8\A9\00", align 1
@str.const.202 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.206 = private unnamed_addr constant [32 x i8] c"=== \D9\85\D9\83\D8\AF\D8\B3 \D8\A7\D9\84\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5 ===\00", align 1
@str.const.210 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.214 = private unnamed_addr constant [41 x i8] c"  \D8\AF\D8\AE\D9\88\D9\84 \D8\A7\D9\84\D8\AF\D8\A7\D9\84\D8\A9 \D8\A7\D9\84\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9\00", align 1
@str.const.218 = private unnamed_addr constant [20 x i8] c"\D8\A5\D8\B7\D8\A7\D8\B1_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\00", align 1
@str.const.219 = private unnamed_addr constant [17 x i8] c"    \D9\85\D8\B3\D8\AA\D9\88\D9\89: \00", align 1
@str.const.220 = private unnamed_addr constant [16 x i8] c" | \D8\B0\D8\A7\D9\83\D8\B1\D8\A9: \00", align 1
@str.const.223 = private unnamed_addr constant [33 x i8] c"  \D8\AF\D8\AE\D9\88\D9\84 \D8\AF\D8\A7\D9\84\D8\A9 \D9\81\D8\B1\D8\B9\D9\8A\D8\A9 1\00", align 1
@str.const.227 = private unnamed_addr constant [20 x i8] c"\D8\A5\D8\B7\D8\A7\D8\B1_\D9\81\D8\B1\D8\B9\D9\8A_1\00", align 1
@str.const.232 = private unnamed_addr constant [33 x i8] c"  \D8\AF\D8\AE\D9\88\D9\84 \D8\AF\D8\A7\D9\84\D8\A9 \D9\81\D8\B1\D8\B9\D9\8A\D8\A9 2\00", align 1
@str.const.236 = private unnamed_addr constant [20 x i8] c"\D8\A5\D8\B7\D8\A7\D8\B1_\D9\81\D8\B1\D8\B9\D9\8A_2\00", align 1
@str.const.241 = private unnamed_addr constant [33 x i8] c"  \D8\AF\D8\AE\D9\88\D9\84 \D8\AF\D8\A7\D9\84\D8\A9 \D9\81\D8\B1\D8\B9\D9\8A\D8\A9 3\00", align 1
@str.const.245 = private unnamed_addr constant [20 x i8] c"\D8\A5\D8\B7\D8\A7\D8\B1_\D9\81\D8\B1\D8\B9\D9\8A_3\00", align 1
@str.const.250 = private unnamed_addr constant [38 x i8] c"  \D8\A3\D9\82\D8\B5\D9\89 \D8\B9\D9\85\D9\82 \D9\88\D8\B5\D9\84\D9\86\D8\A7 \D8\A5\D9\84\D9\8A\D9\87\00", align 1
@str.const.254 = private unnamed_addr constant [38 x i8] c"  \D8\AE\D8\B1\D9\88\D8\AC \D9\85\D9\86 \D8\AF\D8\A7\D9\84\D8\A9 \D9\81\D8\B1\D8\B9\D9\8A\D8\A9 3\00", align 1
@str.const.262 = private unnamed_addr constant [38 x i8] c"  \D8\AE\D8\B1\D9\88\D8\AC \D9\85\D9\86 \D8\AF\D8\A7\D9\84\D8\A9 \D9\81\D8\B1\D8\B9\D9\8A\D8\A9 2\00", align 1
@str.const.270 = private unnamed_addr constant [38 x i8] c"  \D8\AE\D8\B1\D9\88\D8\AC \D9\85\D9\86 \D8\AF\D8\A7\D9\84\D8\A9 \D9\81\D8\B1\D8\B9\D9\8A\D8\A9 1\00", align 1
@str.const.278 = private unnamed_addr constant [46 x i8] c"  \D8\AE\D8\B1\D9\88\D8\AC \D9\85\D9\86 \D8\A7\D9\84\D8\AF\D8\A7\D9\84\D8\A9 \D8\A7\D9\84\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9\00", align 1
@str.const.286 = private unnamed_addr constant [37 x i8] c"  \D8\A7\D9\84\D9\85\D9\83\D8\AF\D8\B3 \D9\81\D8\A7\D8\B1\D8\BA \D8\AA\D9\85\D8\A7\D9\85\D8\A7\D9\8B\00", align 1
@str.const.290 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.294 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\93] \00", align 1
@str.const.298 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\97] \00", align 1
@str.const.302 = private unnamed_addr constant [41 x i8] c"=== \D9\85\D9\84\D8\AE\D8\B5 \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9 ===\00", align 1
@str.const.306 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.310 = private unnamed_addr constant [37 x i8] c"\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5 \D9\88\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1 \D9\85\D8\AA\D9\88\D8\A7\D8\B2\D9\86\00", align 1
@str.const.311 = private unnamed_addr constant [57 x i8] c"\D9\86\D9\82\D9\84 \D8\A7\D9\84\D9\85\D9\84\D9\83\D9\8A\D8\A9 \D9\8A\D8\A8\D8\B7\D9\84 \D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83 \D8\A7\D9\84\D9\82\D8\AF\D9\8A\D9\85\00", align 1
@str.const.312 = private unnamed_addr constant [49 x i8] c"\D9\83\D8\B4\D9\81 \D8\A7\D9\84\D8\AA\D8\B3\D8\B1\D8\A8 \D8\B9\D9\86\D8\AF \D8\B9\D8\AF\D9\85 \D8\A7\D9\84\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1\00", align 1
@str.const.313 = private unnamed_addr constant [29 x i8] c"\D9\85\D9\83\D8\AF\D8\B3 \D8\A7\D9\84\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5 LIFO\00", align 1
@str.const.314 = private unnamed_addr constant [48 x i8] c"\D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D9\84\D8\A7 \D8\AA\D9\86\D9\82\D9\84 \D8\A7\D9\84\D9\85\D9\84\D9\83\D9\8A\D8\A9\00", align 1
@str.const.315 = private unnamed_addr constant [57 x i8] c"\D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83 \D8\A7\D9\84\D8\A3\D8\AE\D9\8A\D8\B1 \D9\85\D8\B3\D8\A4\D9\88\D9\84 \D8\B9\D9\86 \D8\A7\D9\84\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1\00", align 1
@str.const.316 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.320 = private unnamed_addr constant [19 x i8] c"  \D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9: \00", align 1
@str.const.321 = private unnamed_addr constant [25 x i8] c"/6 \D9\85\D8\A8\D8\A7\D8\AF\D8\A6 \D9\85\D8\B7\D8\A8\D9\82\D8\A9\00", align 1
@str.const.324 = private unnamed_addr constant [121 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.328 = private unnamed_addr constant [67 x i8] c"\E2\95\91  \D9\86\D8\B8\D8\A7\D9\85 \D8\A5\D8\AF\D8\A7\D8\B1\D8\A9 \D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9 \D8\A7\D9\84\D9\85\D8\AA\D9\82\D8\AF\D9\85         \E2\95\91\00", align 1
@str.const.332 = private unnamed_addr constant [121 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.336 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.340 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.344 = private unnamed_addr constant [43 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9 ===\00", align 1

define i64 @"\D8\B9\D8\B1\D8\B6_\D9\81\D8\A7\D8\B5\D9\84"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define ptr @"\D8\B3\D8\AC_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5"(i64 %"\D9\85\D8\B9\D8\B1\D9\81", i64 %"\D8\A8\D8\AD\D8\AC\D9\85", ptr %"\D9\85\D8\A7\D9\84\D9\83\D9\87") {
entry:
  %"\D9\85\D8\A7\D9\84\D9\83\D9\873" = alloca ptr, align 8
  store ptr %"\D9\85\D8\A7\D9\84\D9\83\D9\87", ptr %"\D9\85\D8\A7\D9\84\D9\83\D9\873", align 8
  %"\D8\A8\D8\AD\D8\AC\D9\852" = alloca i64, align 8
  store i64 %"\D8\A8\D8\AD\D8\AC\D9\85", ptr %"\D8\A8\D8\AD\D8\AC\D9\852", align 8
  %"\D9\85\D8\B9\D8\B1\D9\811" = alloca i64, align 8
  store i64 %"\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %alloca = alloca ptr, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret ptr @str.ret
}

define ptr @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1_\D8\B0"(i64 %"\D9\85\D8\B9\D8\B1\D9\81", i64 %"\D8\A8\D8\AD\D8\AC\D9\85") {
entry:
  %"\D8\A8\D8\AD\D8\AC\D9\852" = alloca i64, align 8
  store i64 %"\D8\A8\D8\AD\D8\AC\D9\85", ptr %"\D8\A8\D8\AD\D8\AC\D9\852", align 8
  %"\D9\85\D8\B9\D8\B1\D9\811" = alloca i64, align 8
  store i64 %"\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %alloca = alloca ptr, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret ptr @str.ret.13
}

define ptr @"\D8\B3\D8\AC_\D9\86\D9\82\D9\84_\D9\85"(i64 %"\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D9\85\D9\86_\D9\85\D8\A7\D9\84\D9\83", ptr %"\D8\A5\D9\84\D9\89_\D9\85\D8\A7\D9\84\D9\83") {
entry:
  %"\D8\A5\D9\84\D9\89_\D9\85\D8\A7\D9\84\D9\833" = alloca ptr, align 8
  store ptr %"\D8\A5\D9\84\D9\89_\D9\85\D8\A7\D9\84\D9\83", ptr %"\D8\A5\D9\84\D9\89_\D9\85\D8\A7\D9\84\D9\833", align 8
  %"\D9\85\D9\86_\D9\85\D8\A7\D9\84\D9\832" = alloca ptr, align 8
  store ptr %"\D9\85\D9\86_\D9\85\D8\A7\D9\84\D9\83", ptr %"\D9\85\D9\86_\D9\85\D8\A7\D9\84\D9\832", align 8
  %"\D9\85\D8\B9\D8\B1\D9\811" = alloca i64, align 8
  store i64 %"\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret ptr @str.ret.19
}

define ptr @"\D8\B3\D8\AC_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\85"(i64 %"\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D9\85\D8\B3\D8\AA\D8\B9\D9\8A\D8\B1", i64 %"\D9\84\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84") {
entry:
  %"\D9\84\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\843" = alloca i64, align 8
  store i64 %"\D9\84\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84", ptr %"\D9\84\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\843", align 8
  %"\D9\85\D8\B3\D8\AA\D8\B9\D9\8A\D8\B12" = alloca ptr, align 8
  store ptr %"\D9\85\D8\B3\D8\AA\D8\B9\D9\8A\D8\B1", ptr %"\D9\85\D8\B3\D8\AA\D8\B9\D9\8A\D8\B12", align 8
  %"\D9\85\D8\B9\D8\B1\D9\811" = alloca i64, align 8
  store i64 %"\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %"%\D9\84\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84.load" = load i64, ptr %"\D9\84\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\843", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\84\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84.load", 1
  br i1 %cmpeqtmp, label %then_0, label %else_2

then_0:                                           ; preds = %entry
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_1

else_2:                                           ; preds = %entry
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_1

merge_1:                                          ; preds = %else_2, %then_0
  ret ptr @str.ret.30
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA"(i64 %"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A", i64 %"\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85", i64 %"\D9\83\D8\AA\D9\84_\D9\86\D8\B4\D8\B7\D8\A9", i64 %"\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA") {
entry:
  %"\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA4" = alloca i64, align 8
  store i64 %"\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA", ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA4", align 8
  %"\D9\83\D8\AA\D9\84_\D9\86\D8\B4\D8\B7\D8\A93" = alloca i64, align 8
  store i64 %"\D9\83\D8\AA\D9\84_\D9\86\D8\B4\D8\B7\D8\A9", ptr %"\D9\83\D8\AA\D9\84_\D9\86\D8\B4\D8\B7\D8\A93", align 8
  %"\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\852" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85", ptr %"\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\852", align 8
  %"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A1" = alloca i64, align 8
  store i64 %"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A", ptr %"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A1", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.31)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load" = load i64, ptr %"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A1", align 8
  %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\852", align 8
  %subtmp = sub i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load", %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load"
  %alloca = alloca i64, align 8
  store i64 %subtmp, ptr %alloca, align 8
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.53)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5_\D8\A3\D8\B3\D8\A7\D8\B3\D9\8A"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.57)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.61)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 4096, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 0, ptr %alloca2, align 8
  %alloca3 = alloca i64, align 8
  store i64 0, ptr %alloca3, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load" = load i64, ptr %alloca, align 8
  %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load" = load i64, ptr %alloca1, align 8
  %"%\D9\83\D8\AA\D9\84.load" = load i64, ptr %alloca2, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %alloca3, align 8
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA"(i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load", i64 %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load", i64 %"%\D9\83\D8\AA\D9\84.load", i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.65)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp4 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5"(i64 1, i64 256, ptr @str.const.69)
  %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load5" = load i64, ptr %alloca1, align 8
  %addtmp = add i64 %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load5", 256
  store i64 %addtmp, ptr %alloca1, align 8
  %"%\D9\83\D8\AA\D9\84.load6" = load i64, ptr %alloca2, align 8
  %addtmp7 = add i64 %"%\D9\83\D8\AA\D9\84.load6", 1
  store i64 %addtmp7, ptr %alloca2, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load8" = load i64, ptr %alloca3, align 8
  %addtmp9 = add i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load8", 1
  store i64 %addtmp9, ptr %alloca3, align 8
  %calltmp10 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5"(i64 2, i64 512, ptr @str.const.70)
  %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load11" = load i64, ptr %alloca1, align 8
  %addtmp12 = add i64 %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load11", 512
  store i64 %addtmp12, ptr %alloca1, align 8
  %"%\D9\83\D8\AA\D9\84.load13" = load i64, ptr %alloca2, align 8
  %addtmp14 = add i64 %"%\D9\83\D8\AA\D9\84.load13", 1
  store i64 %addtmp14, ptr %alloca2, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load15" = load i64, ptr %alloca3, align 8
  %addtmp16 = add i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load15", 1
  store i64 %addtmp16, ptr %alloca3, align 8
  %calltmp17 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5"(i64 3, i64 1024, ptr @str.const.71)
  %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load18" = load i64, ptr %alloca1, align 8
  %addtmp19 = add i64 %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load18", 1024
  store i64 %addtmp19, ptr %alloca1, align 8
  %"%\D9\83\D8\AA\D9\84.load20" = load i64, ptr %alloca2, align 8
  %addtmp21 = add i64 %"%\D9\83\D8\AA\D9\84.load20", 1
  store i64 %addtmp21, ptr %alloca2, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load22" = load i64, ptr %alloca3, align 8
  %addtmp23 = add i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load22", 1
  store i64 %addtmp23, ptr %alloca3, align 8
  %calltmp24 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5"(i64 4, i64 128, ptr @str.const.72)
  %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load25" = load i64, ptr %alloca1, align 8
  %addtmp26 = add i64 %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load25", 128
  store i64 %addtmp26, ptr %alloca1, align 8
  %"%\D9\83\D8\AA\D9\84.load27" = load i64, ptr %alloca2, align 8
  %addtmp28 = add i64 %"%\D9\83\D8\AA\D9\84.load27", 1
  store i64 %addtmp28, ptr %alloca2, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load29" = load i64, ptr %alloca3, align 8
  %addtmp30 = add i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load29", 1
  store i64 %addtmp30, ptr %alloca3, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.73)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load31" = load i64, ptr %alloca, align 8
  %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load32" = load i64, ptr %alloca1, align 8
  %"%\D9\83\D8\AA\D9\84.load33" = load i64, ptr %alloca2, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load34" = load i64, ptr %alloca3, align 8
  %calltmp35 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA"(i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load31", i64 %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load32", i64 %"%\D9\83\D8\AA\D9\84.load33", i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load34")
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.77)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.81)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp36 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1_\D8\B0"(i64 4, i64 128)
  %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load37" = load i64, ptr %alloca1, align 8
  %subtmp = sub i64 %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load37", 128
  store i64 %subtmp, ptr %alloca1, align 8
  %"%\D9\83\D8\AA\D9\84.load38" = load i64, ptr %alloca2, align 8
  %subtmp39 = sub i64 %"%\D9\83\D8\AA\D9\84.load38", 1
  store i64 %subtmp39, ptr %alloca2, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load40" = load i64, ptr %alloca3, align 8
  %addtmp41 = add i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load40", 1
  store i64 %addtmp41, ptr %alloca3, align 8
  %calltmp42 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1_\D8\B0"(i64 3, i64 1024)
  %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load43" = load i64, ptr %alloca1, align 8
  %subtmp44 = sub i64 %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load43", 1024
  store i64 %subtmp44, ptr %alloca1, align 8
  %"%\D9\83\D8\AA\D9\84.load45" = load i64, ptr %alloca2, align 8
  %subtmp46 = sub i64 %"%\D9\83\D8\AA\D9\84.load45", 1
  store i64 %subtmp46, ptr %alloca2, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load47" = load i64, ptr %alloca3, align 8
  %addtmp48 = add i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load47", 1
  store i64 %addtmp48, ptr %alloca3, align 8
  %calltmp49 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1_\D8\B0"(i64 2, i64 512)
  %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load50" = load i64, ptr %alloca1, align 8
  %subtmp51 = sub i64 %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load50", 512
  store i64 %subtmp51, ptr %alloca1, align 8
  %"%\D9\83\D8\AA\D9\84.load52" = load i64, ptr %alloca2, align 8
  %subtmp53 = sub i64 %"%\D9\83\D8\AA\D9\84.load52", 1
  store i64 %subtmp53, ptr %alloca2, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load54" = load i64, ptr %alloca3, align 8
  %addtmp55 = add i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load54", 1
  store i64 %addtmp55, ptr %alloca3, align 8
  %calltmp56 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1_\D8\B0"(i64 1, i64 256)
  %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load57" = load i64, ptr %alloca1, align 8
  %subtmp58 = sub i64 %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load57", 256
  store i64 %subtmp58, ptr %alloca1, align 8
  %"%\D9\83\D8\AA\D9\84.load59" = load i64, ptr %alloca2, align 8
  %subtmp60 = sub i64 %"%\D9\83\D8\AA\D9\84.load59", 1
  store i64 %subtmp60, ptr %alloca2, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load61" = load i64, ptr %alloca3, align 8
  %addtmp62 = add i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load61", 1
  store i64 %addtmp62, ptr %alloca3, align 8
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.85)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load63" = load i64, ptr %alloca, align 8
  %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load64" = load i64, ptr %alloca1, align 8
  %"%\D9\83\D8\AA\D9\84.load65" = load i64, ptr %alloca2, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load66" = load i64, ptr %alloca3, align 8
  %calltmp67 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA"(i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load63", i64 %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85.load64", i64 %"%\D9\83\D8\AA\D9\84.load65", i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load66")
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.89)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\86\D9\82\D9\84_\D9\85\D9\84\D9\83\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.93)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.97)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %calltmp = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5"(i64 10, i64 512, ptr @str.const.101)
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp1 = call ptr @"\D8\B3\D8\AC_\D9\86\D9\82\D9\84_\D9\85"(i64 10, ptr @str.const.101, ptr @str.const.103)
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load2" = load i64, ptr %alloca, align 8
  %addtmp3 = add i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load2", 1
  store i64 %addtmp3, ptr %alloca, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.104)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp4 = call ptr @"\D8\B3\D8\AC_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\85"(i64 10, ptr @str.const.108, i64 0)
  %calltmp5 = call ptr @"\D8\B3\D8\AC_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\85"(i64 10, ptr @str.const.109, i64 0)
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load6" = load i64, ptr %alloca, align 8
  %addtmp7 = add i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load6", 2
  store i64 %addtmp7, ptr %alloca, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.110)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.114)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.118)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load8" = load i64, ptr %alloca, align 8
  %addtmp9 = add i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load8", 2
  store i64 %addtmp9, ptr %alloca, align 8
  %calltmp10 = call ptr @"\D8\B3\D8\AC_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\85"(i64 10, ptr @str.const.122, i64 1)
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load11" = load i64, ptr %alloca, align 8
  %addtmp12 = add i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load11", 1
  store i64 %addtmp12, ptr %alloca, align 8
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.123)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.127)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load13" = load i64, ptr %alloca, align 8
  %addtmp14 = add i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load13", 1
  store i64 %addtmp14, ptr %alloca, align 8
  %calltmp15 = call ptr @"\D8\B3\D8\AC_\D9\86\D9\82\D9\84_\D9\85"(i64 10, ptr @str.const.103, ptr @str.const.132)
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load16" = load i64, ptr %alloca, align 8
  %addtmp17 = add i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load16", 1
  store i64 %addtmp17, ptr %alloca, align 8
  %calltmp18 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1_\D8\B0"(i64 10, i64 512)
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load19" = load i64, ptr %alloca, align 8
  %addtmp20 = add i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA.load19", 1
  store i64 %addtmp20, ptr %alloca, align 8
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.133)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.137)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.144)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\83\D8\B4\D9\81_\D8\AA\D8\B3\D8\B1\D8\A8"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.148)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.152)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %calltmp = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5"(i64 20, i64 100, ptr @str.const.156)
  %"%\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5\D8\A7\D8\AA.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp2 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5"(i64 21, i64 200, ptr @str.const.157)
  %"%\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5\D8\A7\D8\AA.load3" = load i64, ptr %alloca, align 8
  %addtmp4 = add i64 %"%\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5\D8\A7\D8\AA.load3", 1
  store i64 %addtmp4, ptr %alloca, align 8
  %calltmp5 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5"(i64 22, i64 300, ptr @str.const.158)
  %"%\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5\D8\A7\D8\AA.load6" = load i64, ptr %alloca, align 8
  %addtmp7 = add i64 %"%\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5\D8\A7\D8\AA.load6", 1
  store i64 %addtmp7, ptr %alloca, align 8
  %calltmp8 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5"(i64 23, i64 400, ptr @str.const.159)
  %"%\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5\D8\A7\D8\AA.load9" = load i64, ptr %alloca, align 8
  %addtmp10 = add i64 %"%\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5\D8\A7\D8\AA.load9", 1
  store i64 %addtmp10, ptr %alloca, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.160)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp11 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1_\D8\B0"(i64 20, i64 100)
  %"%\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1\D8\A7\D8\AA.load" = load i64, ptr %alloca1, align 8
  %addtmp12 = add i64 %"%\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1\D8\A7\D8\AA.load", 1
  store i64 %addtmp12, ptr %alloca1, align 8
  %calltmp13 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1_\D8\B0"(i64 22, i64 300)
  %"%\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1\D8\A7\D8\AA.load14" = load i64, ptr %alloca1, align 8
  %addtmp15 = add i64 %"%\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1\D8\A7\D8\AA.load14", 1
  store i64 %addtmp15, ptr %alloca1, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.164)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5\D8\A7\D8\AA.load16" = load i64, ptr %alloca, align 8
  %"%\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1\D8\A7\D8\AA.load17" = load i64, ptr %alloca1, align 8
  %subtmp = sub i64 %"%\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5\D8\A7\D8\AA.load16", %"%\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1\D8\A7\D8\AA.load17"
  %alloca18 = alloca i64, align 8
  store i64 %subtmp, ptr %alloca18, align 8
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.168)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\81\D8\B1\D9\82.load" = load i64, ptr %alloca18, align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\81\D8\B1\D9\82.load", 0
  br i1 %cmpgttmp, label %then_3, label %else_5

then_3:                                           ; preds = %entry
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.182)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.186)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.190)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.194)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_4

else_5:                                           ; preds = %entry
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.198)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_4

merge_4:                                          ; preds = %else_5, %then_3
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.202)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\85\D9\83\D8\AF\D8\B3"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.206)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.210)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.214)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5"(i64 30, i64 64, ptr @str.const.218)
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load" = load i64, ptr %alloca1, align 8
  %addtmp = add i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load", 64
  store i64 %addtmp, ptr %alloca1, align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load" = load i64, ptr %alloca, align 8
  %addtmp2 = add i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load", 1
  store i64 %addtmp2, ptr %alloca, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.223)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp3 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5"(i64 31, i64 128, ptr @str.const.227)
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load4" = load i64, ptr %alloca1, align 8
  %addtmp5 = add i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load4", 128
  store i64 %addtmp5, ptr %alloca1, align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load6" = load i64, ptr %alloca, align 8
  %addtmp7 = add i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load6", 1
  store i64 %addtmp7, ptr %alloca, align 8
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.232)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp8 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5"(i64 32, i64 256, ptr @str.const.236)
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load9" = load i64, ptr %alloca1, align 8
  %addtmp10 = add i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load9", 256
  store i64 %addtmp10, ptr %alloca1, align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load11" = load i64, ptr %alloca, align 8
  %addtmp12 = add i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load11", 1
  store i64 %addtmp12, ptr %alloca, align 8
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.241)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp13 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5"(i64 33, i64 512, ptr @str.const.245)
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load14" = load i64, ptr %alloca1, align 8
  %addtmp15 = add i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load14", 512
  store i64 %addtmp15, ptr %alloca1, align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load16" = load i64, ptr %alloca, align 8
  %addtmp17 = add i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load16", 1
  store i64 %addtmp17, ptr %alloca, align 8
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp18 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\81\D8\A7\D8\B5\D9\84"()
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.250)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp19 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\81\D8\A7\D8\B5\D9\84"()
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.254)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp20 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1_\D8\B0"(i64 33, i64 512)
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load21" = load i64, ptr %alloca1, align 8
  %subtmp = sub i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load21", 512
  store i64 %subtmp, ptr %alloca1, align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load22" = load i64, ptr %alloca, align 8
  %subtmp23 = sub i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load22", 1
  store i64 %subtmp23, ptr %alloca, align 8
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.262)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp24 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1_\D8\B0"(i64 32, i64 256)
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load25" = load i64, ptr %alloca1, align 8
  %subtmp26 = sub i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load25", 256
  store i64 %subtmp26, ptr %alloca1, align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load27" = load i64, ptr %alloca, align 8
  %subtmp28 = sub i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load27", 1
  store i64 %subtmp28, ptr %alloca, align 8
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.270)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp29 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1_\D8\B0"(i64 31, i64 128)
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load30" = load i64, ptr %alloca1, align 8
  %subtmp31 = sub i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load30", 128
  store i64 %subtmp31, ptr %alloca1, align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load32" = load i64, ptr %alloca, align 8
  %subtmp33 = sub i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load32", 1
  store i64 %subtmp33, ptr %alloca, align 8
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.278)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp34 = call ptr @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1_\D8\B0"(i64 30, i64 64)
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load35" = load i64, ptr %alloca1, align 8
  %subtmp36 = sub i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9.load35", 64
  store i64 %subtmp36, ptr %alloca1, align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load37" = load i64, ptr %alloca, align 8
  %subtmp38 = sub i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load37", 1
  store i64 %subtmp38, ptr %alloca, align 8
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.286)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.290)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3_\D8\B0"(ptr %"\D9\85\D8\A8\D8\AF\D8\A3", i64 %"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82") {
entry:
  %"\D8\AA\D8\B7\D8\A8\D9\8A\D9\822" = alloca i64, align 8
  store i64 %"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82", ptr %"\D8\AA\D8\B7\D8\A8\D9\8A\D9\822", align 8
  %"\D9\85\D8\A8\D8\AF\D8\A31" = alloca ptr, align 8
  store ptr %"\D9\85\D8\A8\D8\AF\D8\A3", ptr %"\D9\85\D8\A8\D8\AF\D8\A31", align 8
  %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82.load" = load i64, ptr %"\D8\AA\D8\B7\D8\A8\D9\8A\D9\822", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82.load", 1
  br i1 %cmpeqtmp, label %then_6, label %else_8

then_6:                                           ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const.294)
  %len2 = call i64 @strlen(ptr %"\D9\85\D8\A8\D8\AF\D8\A31")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.294, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\85\D8\A8\D8\AF\D8\A31", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_7

else_8:                                           ; preds = %entry
  %len13 = call i64 @strlen(ptr @str.const.298)
  %len24 = call i64 @strlen(ptr %"\D9\85\D8\A8\D8\AF\D8\A31")
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %4 = call ptr @memcpy(ptr %concat.buf7, ptr @str.const.298, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %5 = call ptr @memcpy(ptr %dest28, ptr %"\D9\85\D8\A8\D8\AF\D8\A31", i64 %copylen29)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf7)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_7

merge_7:                                          ; preds = %else_8, %then_6
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D9\84\D9\85\D9\84\D8\AE\D8\B5"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.302)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.306)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3_\D8\B0"(ptr @str.const.310, i64 1)
  %"%\D8\B9.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\B9.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp1 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3_\D8\B0"(ptr @str.const.311, i64 1)
  %"%\D8\B9.load2" = load i64, ptr %alloca, align 8
  %addtmp3 = add i64 %"%\D8\B9.load2", 1
  store i64 %addtmp3, ptr %alloca, align 8
  %calltmp4 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3_\D8\B0"(ptr @str.const.312, i64 1)
  %"%\D8\B9.load5" = load i64, ptr %alloca, align 8
  %addtmp6 = add i64 %"%\D8\B9.load5", 1
  store i64 %addtmp6, ptr %alloca, align 8
  %calltmp7 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3_\D8\B0"(ptr @str.const.313, i64 1)
  %"%\D8\B9.load8" = load i64, ptr %alloca, align 8
  %addtmp9 = add i64 %"%\D8\B9.load8", 1
  store i64 %addtmp9, ptr %alloca, align 8
  %calltmp10 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3_\D8\B0"(ptr @str.const.314, i64 1)
  %"%\D8\B9.load11" = load i64, ptr %alloca, align 8
  %addtmp12 = add i64 %"%\D8\B9.load11", 1
  store i64 %addtmp12, ptr %alloca, align 8
  %calltmp13 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3_\D8\B0"(ptr @str.const.315, i64 1)
  %"%\D8\B9.load14" = load i64, ptr %alloca, align 8
  %addtmp15 = add i64 %"%\D8\B9.load14", 1
  store i64 %addtmp15, ptr %alloca, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.316)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.324)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.328)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.332)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.336)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5_\D8\A3\D8\B3\D8\A7\D8\B3\D9\8A"()
  %calltmp1 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\81\D8\A7\D8\B5\D9\84"()
  %calltmp2 = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\86\D9\82\D9\84_\D9\85\D9\84\D9\83\D9\8A\D8\A9"()
  %calltmp3 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\81\D8\A7\D8\B5\D9\84"()
  %calltmp4 = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\83\D8\B4\D9\81_\D8\AA\D8\B3\D8\B1\D8\A8"()
  %calltmp5 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\81\D8\A7\D8\B5\D9\84"()
  %calltmp6 = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\85\D9\83\D8\AF\D8\B3"()
  %calltmp7 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\81\D8\A7\D8\B5\D9\84"()
  %calltmp8 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D9\84\D9\85\D9\84\D8\AE\D8\B5"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.340)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.344)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

declare i32 @printf(ptr, ...)

declare i64 @strlen(ptr)

declare ptr @memcpy(ptr, ptr, i64)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
