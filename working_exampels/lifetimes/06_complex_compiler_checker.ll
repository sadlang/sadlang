; ModuleID = '06_complex_compiler_checker'
source_filename = "06_complex_compiler_checker"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [12 x i8] c"  [\D9\81\D8\AD\D8\B5] \00", align 1
@str.const.1 = private unnamed_addr constant [3 x i8] c": \00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.2 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.4 = private unnamed_addr constant [16 x i8] c"  [\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1] \00", align 1
@str.const.8 = private unnamed_addr constant [13 x i8] c"  [\D8\AE\D8\B7\D8\A3!] \00", align 1
@str.const.12 = private unnamed_addr constant [14 x i8] c"  [\D9\86\D8\AC\D8\A7\D8\AD] \00", align 1
@str.const.16 = private unnamed_addr constant [19 x i8] c"  \D8\A7\D9\84\D9\85\D8\AA\D8\BA\D9\8A\D8\B1: \00", align 1
@str.const.20 = private unnamed_addr constant [17 x i8] c"    \D8\A7\D9\84\D9\86\D9\88\D8\B9: \00", align 1
@str.const.24 = private unnamed_addr constant [28 x i8] c"    \D8\A7\D9\84\D8\A5\D9\86\D8\B4\D8\A7\D8\A1: \D8\B3\D8\B7\D8\B1 \00", align 1
@str.const.27 = private unnamed_addr constant [35 x i8] c"    \D8\A2\D8\AE\D8\B1 \D8\A7\D8\B3\D8\AA\D8\AE\D8\AF\D8\A7\D9\85: \D8\B3\D8\B7\D8\B1 \00", align 1
@str.const.30 = private unnamed_addr constant [24 x i8] c"    \D9\85\D8\AF\D9\89 \D8\A7\D9\84\D8\B9\D9\85\D8\B1: \00", align 1
@str.const.31 = private unnamed_addr constant [10 x i8] c" \D8\A3\D8\B3\D8\B7\D8\B1\00", align 1
@str.const.34 = private unnamed_addr constant [43 x i8] c"\D8\A7\D9\84\D9\85\D8\B1\D8\AC\D8\B9 \D9\8A\D8\A8\D8\AF\D8\A3 \D9\82\D8\A8\D9\84 \D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83!\00", align 1
@str.const.35 = private unnamed_addr constant [50 x i8] c"\D8\A7\D9\84\D9\85\D8\B1\D8\AC\D8\B9 \D9\8A\D8\B9\D9\8A\D8\B4 \D8\A3\D8\B7\D9\88\D9\84 \D9\85\D9\86 \D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83!\00", align 1
@str.const.36 = private unnamed_addr constant [47 x i8] c"\D8\B9\D9\85\D8\B1 \D8\A7\D9\84\D9\85\D8\B1\D8\AC\D8\B9 \D8\B6\D9\85\D9\86 \D8\B9\D9\85\D8\B1 \D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83\00", align 1
@str.const.37 = private unnamed_addr constant [37 x i8] c"=== \D9\81\D8\AD\D8\B5 \D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC \D8\B5\D8\AD\D9\8A\D8\AD ===\00", align 1
@str.const.41 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.45 = private unnamed_addr constant [40 x i8] c"  --- \D8\AA\D8\AD\D9\84\D9\8A\D9\84 \D8\A7\D9\84\D9\85\D8\AA\D8\BA\D9\8A\D8\B1\D8\A7\D8\AA ---\00", align 1
@str.const.49 = private unnamed_addr constant [13 x i8] c"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA\00", align 1
@str.const.50 = private unnamed_addr constant [13 x i8] c"\D9\85\D8\B5\D9\81\D9\88\D9\81\D8\A9\00", align 1
@str.const.51 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.55 = private unnamed_addr constant [12 x i8] c"\D9\85\D8\B1\D8\AC\D8\B9_\D8\A3\00", align 1
@str.const.56 = private unnamed_addr constant [20 x i8] c"\D9\85\D8\B1\D8\AC\D8\B9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9\00", align 1
@str.const.57 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.61 = private unnamed_addr constant [12 x i8] c"\D9\85\D8\B1\D8\AC\D8\B9_\D8\A8\00", align 1
@str.const.63 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.67 = private unnamed_addr constant [32 x i8] c"  --- \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\A3\D8\B9\D9\85\D8\A7\D8\B1 ---\00", align 1
@str.const.72 = private unnamed_addr constant [43 x i8] c"\D9\87\D9\84 \D9\8A\D8\B9\D9\8A\D8\B4 \D8\B6\D9\85\D9\86 \D8\B9\D9\85\D8\B1 \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA\D8\9F\00", align 1
@str.const.75 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.79 = private unnamed_addr constant [36 x i8] c"  --- \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 ---\00", align 1
@str.const.83 = private unnamed_addr constant [11 x i8] c"\D8\AA\D8\B9\D8\A7\D8\B1\D8\B6\00", align 1
@str.const.84 = private unnamed_addr constant [51 x i8] c"\D9\85\D8\B1\D8\AC\D8\B9_\D8\A3 \D9\88 \D9\85\D8\B1\D8\AC\D8\B9_\D8\A8 \D9\83\D9\84\D8\A7\D9\87\D9\85\D8\A7 \D9\82\D8\B1\D8\A7\D8\A1\D8\A9\00", align 1
@str.const.85 = private unnamed_addr constant [48 x i8] c"\D8\B9\D8\AF\D8\A9 \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A7\D8\AA \D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D9\85\D8\B3\D9\85\D9\88\D8\AD\D8\A9\00", align 1
@str.const.86 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.90 = private unnamed_addr constant [25 x i8] c"  --- \D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9 ---\00", align 1
@str.const.94 = private unnamed_addr constant [17 x i8] c"  \D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA: \00", align 1
@str.const.97 = private unnamed_addr constant [15 x i8] c"  \D9\85\D8\B4\D8\A7\D9\83\D9\84: \00", align 1
@str.const.100 = private unnamed_addr constant [50 x i8] c"\D8\A7\D9\84\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC \D9\8A\D8\AC\D8\AA\D8\A7\D8\B2 \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\A3\D8\B9\D9\85\D8\A7\D8\B1\00", align 1
@str.const.101 = private unnamed_addr constant [50 x i8] c"\D8\A7\D9\84\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC \D9\8A\D8\AD\D8\AA\D9\88\D9\8A \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1 \D8\A3\D8\B9\D9\85\D8\A7\D8\B1\00", align 1
@str.const.102 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.106 = private unnamed_addr constant [54 x i8] c"=== \D9\81\D8\AD\D8\B5 \D9\85\D8\B1\D8\AC\D8\B9 \D9\85\D8\B9\D9\84\D9\82 (Dangling Reference) ===\00", align 1
@str.const.110 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.118 = private unnamed_addr constant [9 x i8] c"\D9\85\D8\AD\D9\84\D9\8A\00", align 1
@str.const.119 = private unnamed_addr constant [5 x i8] c"\D9\86\D8\B5\00", align 1
@str.const.120 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.124 = private unnamed_addr constant [20 x i8] c"\D9\85\D8\B1\D8\AC\D8\B9_\D8\AE\D8\A7\D8\B1\D8\AC\D9\8A\00", align 1
@str.const.125 = private unnamed_addr constant [9 x i8] c"\D9\85\D8\B1\D8\AC\D8\B9\00", align 1
@str.const.126 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.135 = private unnamed_addr constant [39 x i8] c"\D9\87\D9\84 \D9\8A\D8\B9\D9\8A\D8\B4 \D8\B6\D9\85\D9\86 \D8\B9\D9\85\D8\B1 \D9\85\D8\AD\D9\84\D9\8A\D8\9F\00", align 1
@str.const.136 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.140 = private unnamed_addr constant [18 x i8] c"  \D8\A7\D9\84\D8\AA\D9\88\D8\B6\D9\8A\D8\AD:\00", align 1
@str.const.144 = private unnamed_addr constant [52 x i8] c"    \D9\85\D8\AD\D9\84\D9\8A:        [\D8\B3\D8\B7\D8\B1 10 -------- \D8\B3\D8\B7\D8\B1 15]\00", align 1
@str.const.148 = private unnamed_addr constant [96 x i8] c"    \D9\85\D8\B1\D8\AC\D8\B9_\D8\AE\D8\A7\D8\B1\D8\AC\D9\8A: [\D8\B3\D8\B7\D8\B1 12 \E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80 \D8\B3\D8\B7\D8\B1 25]\00", align 1
@str.const.152 = private unnamed_addr constant [43 x i8] c"                               ^ \D8\AE\D8\B7\D8\B1! ^\00", align 1
@str.const.156 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.160 = private unnamed_addr constant [28 x i8] c"  \D9\85\D8\B4\D8\A7\D9\83\D9\84 \D9\85\D9\83\D8\AA\D8\B4\D9\81\D8\A9: \00", align 1
@str.const.163 = private unnamed_addr constant [65 x i8] c"\D9\85\D8\B1\D8\AC\D8\B9 \D9\85\D8\B9\D9\84\D9\82! \D8\A7\D9\84\D9\85\D8\B1\D8\AC\D8\B9 \D9\8A\D8\B4\D9\8A\D8\B1 \D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9 \D9\85\D8\AD\D8\B1\D8\B1\D8\A9\00", align 1
@str.const.164 = private unnamed_addr constant [79 x i8] c"  \D8\A7\D9\84\D8\AD\D9\84: \D8\AA\D9\82\D8\B5\D9\8A\D8\B1 \D8\B9\D9\85\D8\B1 \D8\A7\D9\84\D9\85\D8\B1\D8\AC\D8\B9 \D8\A3\D9\88 \D8\AA\D9\85\D8\AF\D9\8A\D8\AF \D8\B9\D9\85\D8\B1 \D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83\00", align 1
@str.const.168 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.172 = private unnamed_addr constant [45 x i8] c"=== \D9\81\D8\AD\D8\B5 \D8\AA\D8\B9\D8\A7\D8\B1\D8\B6 \D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 ===\00", align 1
@str.const.176 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.180 = private unnamed_addr constant [29 x i8] c"  --- \D8\A7\D9\84\D9\85\D8\AA\D8\BA\D9\8A\D8\B1\D8\A7\D8\AA ---\00", align 1
@str.const.185 = private unnamed_addr constant [9 x i8] c"\D9\85\D8\A7\D9\84\D9\83\00", align 1
@str.const.186 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.190 = private unnamed_addr constant [11 x i8] c"\D9\82\D8\A7\D8\B1\D8\A6_1\00", align 1
@str.const.191 = private unnamed_addr constant [26 x i8] c"\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9\00", align 1
@str.const.192 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.196 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\B9\D8\AF\D9\84_1\00", align 1
@str.const.197 = private unnamed_addr constant [26 x i8] c"\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84\00", align 1
@str.const.198 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.207 = private unnamed_addr constant [29 x i8] c"\D8\B6\D9\85\D9\86 \D8\B9\D9\85\D8\B1 \D9\85\D8\B5\D9\81\D9\88\D9\81\D8\A9\D8\9F\00", align 1
@str.const.210 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.214 = private unnamed_addr constant [32 x i8] c"  --- \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\AA\D8\B9\D8\A7\D8\B1\D8\B6 ---\00", align 1
@str.const.218 = private unnamed_addr constant [53 x i8] c"\D8\AA\D8\B9\D8\A7\D8\B1\D8\B6! \D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D9\88\D8\AA\D8\B9\D8\AF\D9\8A\D9\84 \D9\85\D8\AA\D8\AF\D8\A7\D8\AE\D9\84\D8\A7\D9\86\00", align 1
@str.const.219 = private unnamed_addr constant [37 x i8] c"    \D9\81\D8\AA\D8\B1\D8\A9 \D8\A7\D9\84\D8\AA\D8\AF\D8\A7\D8\AE\D9\84: \D8\B3\D8\B7\D8\B1 \00", align 1
@str.const.220 = private unnamed_addr constant [4 x i8] c" - \00", align 1
@str.const.223 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.231 = private unnamed_addr constant [95 x i8] c"    \D9\85\D8\B5\D9\81\D9\88\D9\81\D8\A9: [1 \E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80 30]\00", align 1
@str.const.235 = private unnamed_addr constant [66 x i8] c"    \D9\82\D8\A7\D8\B1\D8\A6_1: [5 \E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80 20]\00", align 1
@str.const.239 = private unnamed_addr constant [42 x i8] c"    \D9\85\D8\B9\D8\AF\D9\84_1:      [10 \E2\94\80\E2\94\80\E2\94\80\E2\94\80 18]\00", align 1
@str.const.243 = private unnamed_addr constant [40 x i8] c"                  ^^^^^^^^^ \D8\AA\D8\B9\D8\A7\D8\B1\D8\B6!\00", align 1
@str.const.247 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.254 = private unnamed_addr constant [62 x i8] c"  \D8\A7\D9\84\D8\AD\D9\84: \D9\81\D8\B5\D9\84 \D9\81\D8\AA\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D9\88\D8\A7\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84\00", align 1
@str.const.258 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.262 = private unnamed_addr constant [39 x i8] c"=== \D8\AA\D8\AD\D9\84\D9\8A\D9\84 \D8\AF\D8\A7\D9\84\D8\A9 \D9\83\D8\A7\D9\85\D9\84\D8\A9 ===\00", align 1
@str.const.266 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.270 = private unnamed_addr constant [44 x i8] c"  \D8\A7\D9\84\D8\AF\D8\A7\D9\84\D8\A9: \D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA()\00", align 1
@str.const.274 = private unnamed_addr constant [30 x i8] c"  \D8\A7\D9\84\D9\86\D8\B7\D8\A7\D9\82: \D8\B3\D8\B7\D8\B1 1 - 50\00", align 1
@str.const.278 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.282 = private unnamed_addr constant [44 x i8] c"  \D8\A7\D9\84\D8\AE\D8\B7\D9\88\D8\A9 1: \D8\AC\D9\85\D8\B9 \D8\A7\D9\84\D9\85\D8\AA\D8\BA\D9\8A\D8\B1\D8\A7\D8\AA\00", align 1
@str.const.286 = private unnamed_addr constant [54 x i8] c"    \D9\85\D8\AA\D8\BA\D9\8A\D8\B1: \D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA (\D9\85\D8\A7\D9\84\D9\83\D8\8C \D8\B3\D8\B7\D8\B1 2-45)\00", align 1
@str.const.290 = private unnamed_addr constant [52 x i8] c"    \D9\85\D8\AA\D8\BA\D9\8A\D8\B1: \D9\86\D8\AA\D8\A7\D8\A6\D8\AC (\D9\85\D8\A7\D9\84\D9\83\D8\8C \D8\B3\D8\B7\D8\B1 3-48)\00", align 1
@str.const.294 = private unnamed_addr constant [63 x i8] c"    \D9\85\D8\AA\D8\BA\D9\8A\D8\B1: \D9\85\D8\A4\D8\B4\D8\B1 (\D9\85\D8\B1\D8\AC\D8\B9 \D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA\D8\8C \D8\B3\D8\B7\D8\B1 5-30)\00", align 1
@str.const.298 = private unnamed_addr constant [73 x i8] c"    \D9\85\D8\AA\D8\BA\D9\8A\D8\B1: \D9\85\D8\AD\D8\B1\D8\B1 (\D9\85\D8\B1\D8\AC\D8\B9_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84 \D9\86\D8\AA\D8\A7\D8\A6\D8\AC\D8\8C \D8\B3\D8\B7\D8\B1 10-40)\00", align 1
@str.const.302 = private unnamed_addr constant [62 x i8] c"    \D9\85\D8\AA\D8\BA\D9\8A\D8\B1: \D9\82\D8\A7\D8\B1\D8\A6 (\D9\85\D8\B1\D8\AC\D8\B9 \D9\86\D8\AA\D8\A7\D8\A6\D8\AC\D8\8C \D8\B3\D8\B7\D8\B1 35-42)\00", align 1
@str.const.306 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.310 = private unnamed_addr constant [40 x i8] c"  \D8\A7\D9\84\D8\AE\D8\B7\D9\88\D8\A9 2: \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\A3\D8\B9\D9\85\D8\A7\D8\B1\00", align 1
@str.const.314 = private unnamed_addr constant [26 x i8] c"\D9\85\D8\A4\D8\B4\D8\B1 \E2\86\90 \D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA\00", align 1
@str.const.315 = private unnamed_addr constant [14 x i8] c"5-30 \E2\8A\82 2-45\00", align 1
@str.const.316 = private unnamed_addr constant [24 x i8] c"\D9\85\D8\AD\D8\B1\D8\B1 \E2\86\90 \D9\86\D8\AA\D8\A7\D8\A6\D8\AC\00", align 1
@str.const.317 = private unnamed_addr constant [15 x i8] c"10-40 \E2\8A\82 3-48\00", align 1
@str.const.318 = private unnamed_addr constant [24 x i8] c"\D9\82\D8\A7\D8\B1\D8\A6 \E2\86\90 \D9\86\D8\AA\D8\A7\D8\A6\D8\AC\00", align 1
@str.const.319 = private unnamed_addr constant [15 x i8] c"35-42 \E2\8A\82 3-48\00", align 1
@str.const.320 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.324 = private unnamed_addr constant [55 x i8] c"  \D8\A7\D9\84\D8\AE\D8\B7\D9\88\D8\A9 3: \D9\81\D8\AD\D8\B5 \D8\AA\D8\B9\D8\A7\D8\B1\D8\B6 \D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9\00", align 1
@str.const.328 = private unnamed_addr constant [71 x i8] c"\D8\AA\D8\B9\D8\A7\D8\B1\D8\B6: \D9\85\D8\AD\D8\B1\D8\B1 (\D8\AA\D8\B9\D8\AF\D9\8A\D9\84 10-40) \D9\88 \D9\82\D8\A7\D8\B1\D8\A6 (\D9\82\D8\B1\D8\A7\D8\A1\D8\A9 35-42)\00", align 1
@str.const.329 = private unnamed_addr constant [44 x i8] c"    \D9\81\D8\AA\D8\B1\D8\A9 \D8\A7\D9\84\D8\AA\D8\B9\D8\A7\D8\B1\D8\B6: \D8\B3\D8\B7\D8\B1 35 - 40\00", align 1
@str.const.333 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.337 = private unnamed_addr constant [33 x i8] c"  \D8\A7\D9\84\D8\AE\D8\B7\D9\88\D8\A9 4: \D8\AA\D8\AD\D8\B0\D9\8A\D8\B1\D8\A7\D8\AA\00", align 1
@str.const.341 = private unnamed_addr constant [85 x i8] c"\D9\85\D8\AA\D8\BA\D9\8A\D8\B1 '\D9\85\D8\A4\D8\B4\D8\B1' \D9\8A\D9\85\D9\83\D9\86 \D8\AA\D9\82\D8\B5\D9\8A\D8\B1 \D8\B9\D9\85\D8\B1\D9\87 (\D8\A2\D8\AE\D8\B1 \D8\A7\D8\B3\D8\AA\D8\AE\D8\AF\D8\A7\D9\85 \D8\B3\D8\B7\D8\B1 25)\00", align 1
@str.const.342 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.346 = private unnamed_addr constant [72 x i8] c"  \E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90 \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D8\AA\D8\AD\D9\84\D9\8A\D9\84 \E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.350 = private unnamed_addr constant [21 x i8] c"  \E2\95\91 \D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA: \00", align 1
@str.const.353 = private unnamed_addr constant [20 x i8] c"  \E2\95\91 \D9\85\D8\B4\D8\A7\D9\83\D9\84:  \00", align 1
@str.const.356 = private unnamed_addr constant [23 x i8] c"  \E2\95\91 \D8\AA\D8\AD\D8\B0\D9\8A\D8\B1\D8\A7\D8\AA: \00", align 1
@str.const.359 = private unnamed_addr constant [33 x i8] c"  \E2\95\91 \D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9: \D9\81\D8\B4\D9\84 \E2\9C\97\00", align 1
@str.const.363 = private unnamed_addr constant [35 x i8] c"  \E2\95\91 \D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9: \D9\86\D8\AC\D8\A7\D8\AD \E2\9C\93\00", align 1
@str.const.367 = private unnamed_addr constant [90 x i8] c"  \E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.371 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.375 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\93] \00", align 1
@str.const.379 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\97] \00", align 1
@str.const.383 = private unnamed_addr constant [52 x i8] c"=== \D9\85\D9\84\D8\AE\D8\B5 \D9\85\D8\A8\D8\A7\D8\AF\D8\A6 \D9\81\D8\A7\D8\AD\D8\B5 \D8\A7\D9\84\D8\A3\D8\B9\D9\85\D8\A7\D8\B1 ===\00", align 1
@str.const.387 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.391 = private unnamed_addr constant [36 x i8] c"\D8\AA\D8\AD\D9\84\D9\8A\D9\84 \D9\86\D8\B7\D8\A7\D9\82 \D9\83\D9\84 \D9\85\D8\AA\D8\BA\D9\8A\D8\B1\00", align 1
@str.const.392 = private unnamed_addr constant [40 x i8] c"\D8\A7\D9\84\D8\AA\D8\AD\D9\82\D9\82 \D9\85\D9\86 \D8\B9\D9\85\D8\B1 \D8\A7\D9\84\D9\85\D8\B1\D8\A7\D8\AC\D8\B9\00", align 1
@str.const.393 = private unnamed_addr constant [37 x i8] c"\D9\83\D8\B4\D9\81 \D8\A7\D9\84\D9\85\D8\B1\D8\A7\D8\AC\D8\B9 \D8\A7\D9\84\D9\85\D8\B9\D9\84\D9\82\D8\A9\00", align 1
@str.const.394 = private unnamed_addr constant [37 x i8] c"\D9\83\D8\B4\D9\81 \D8\AA\D8\B9\D8\A7\D8\B1\D8\B6 \D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9\00", align 1
@str.const.395 = private unnamed_addr constant [41 x i8] c"\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1\D8\A7\D8\AA \D8\AA\D8\AD\D8\B3\D9\8A\D9\86 \D8\A7\D9\84\D8\A3\D8\B9\D9\85\D8\A7\D8\B1\00", align 1
@str.const.396 = private unnamed_addr constant [35 x i8] c"\D8\AA\D9\82\D8\A7\D8\B1\D9\8A\D8\B1 \D8\AA\D8\AD\D9\84\D9\8A\D9\84 \D8\B4\D8\A7\D9\85\D9\84\D8\A9\00", align 1
@str.const.397 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.401 = private unnamed_addr constant [19 x i8] c"  \D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9: \00", align 1
@str.const.402 = private unnamed_addr constant [14 x i8] c"/6 \D9\85\D8\A8\D8\A7\D8\AF\D8\A6\00", align 1
@str.const.405 = private unnamed_addr constant [121 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.409 = private unnamed_addr constant [64 x i8] c"\E2\95\91  \D9\81\D8\A7\D8\AD\D8\B5 \D8\A7\D9\84\D8\A3\D8\B9\D9\85\D8\A7\D8\B1 \D9\81\D9\8A \D8\A7\D9\84\D9\85\D8\AA\D8\B1\D8\AC\D9\85            \E2\95\91\00", align 1
@str.const.413 = private unnamed_addr constant [121 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.417 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.421 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.425 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.429 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.433 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.437 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.441 = private unnamed_addr constant [32 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 \D8\A7\D9\84\D9\81\D8\A7\D8\AD\D8\B5 ===\00", align 1

define i64 @"\D8\B3\D8\AC_\D9\81\D8\AD\D8\B5"(ptr %"\D9\85\D8\B1\D8\AD\D9\84\D8\A9", ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84") {
entry:
  %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\842" = alloca ptr, align 8
  store ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84", ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\842", align 8
  %"\D9\85\D8\B1\D8\AD\D9\84\D8\A91" = alloca ptr, align 8
  store ptr %"\D9\85\D8\B1\D8\AD\D9\84\D8\A9", ptr %"\D9\85\D8\B1\D8\AD\D9\84\D8\A91", align 8
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %"\D9\85\D8\B1\D8\AD\D9\84\D8\A91")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\85\D8\B1\D8\AD\D9\84\D8\A91", i64 %copylen2)
  %len13 = call i64 @strlen(ptr %concat.buf)
  %len24 = call i64 @strlen(ptr @str.const.1)
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %2 = call ptr @memcpy(ptr %concat.buf7, ptr %concat.buf, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %3 = call ptr @memcpy(ptr %dest28, ptr @str.const.1, i64 %copylen29)
  %len110 = call i64 @strlen(ptr %concat.buf7)
  %len211 = call i64 @strlen(ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\842")
  %totallen12 = add i64 %len110, %len211
  %bufsize13 = add i64 %totallen12, 1
  %concat.buf14 = alloca i8, i64 %bufsize13, align 1
  %4 = call ptr @memcpy(ptr %concat.buf14, ptr %concat.buf7, i64 %len110)
  %dest215 = getelementptr i8, ptr %concat.buf14, i64 %len110
  %copylen216 = add i64 %len211, 1
  %5 = call ptr @memcpy(ptr %dest215, ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\842", i64 %copylen216)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf14)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1"(ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84") {
entry:
  %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\841" = alloca ptr, align 8
  store ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84", ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\841", align 8
  %len1 = call i64 @strlen(ptr @str.const.4)
  %len2 = call i64 @strlen(ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\841")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.4, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\841", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B3\D8\AC_\D9\85\D8\B4\D9\83\D9\84\D8\A9"(ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84") {
entry:
  %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\841" = alloca ptr, align 8
  store ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84", ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\841", align 8
  %len1 = call i64 @strlen(ptr @str.const.8)
  %len2 = call i64 @strlen(ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\841")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.8, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\841", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B3\D8\AC_\D8\B3\D9\84\D9\8A\D9\85"(ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84") {
entry:
  %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\841" = alloca ptr, align 8
  store ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84", ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\841", align 8
  %len1 = call i64 @strlen(ptr @str.const.12)
  %len2 = call i64 @strlen(ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\841")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.12, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\841", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D9\85\D8\AA\D8\BA\D9\8A\D8\B1"(ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D8\B5\D9\86\D9\81\D9\87", i64 %"\D8\B3\D8\B7\D8\B1_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1", i64 %"\D8\B3\D8\B7\D8\B1_\D8\A2\D8\AE\D8\B1") {
entry:
  %"\D8\B3\D8\B7\D8\B1_\D8\A2\D8\AE\D8\B14" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\B1_\D8\A2\D8\AE\D8\B1", ptr %"\D8\B3\D8\B7\D8\B1_\D8\A2\D8\AE\D8\B14", align 8
  %"\D8\B3\D8\B7\D8\B1_\D8\A5\D9\86\D8\B4\D8\A7\D8\A13" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\B1_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1", ptr %"\D8\B3\D8\B7\D8\B1_\D8\A5\D9\86\D8\B4\D8\A7\D8\A13", align 8
  %"\D8\B5\D9\86\D9\81\D9\872" = alloca ptr, align 8
  store ptr %"\D8\B5\D9\86\D9\81\D9\87", ptr %"\D8\B5\D9\86\D9\81\D9\872", align 8
  %"\D8\A7\D8\B3\D9\851" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\851", align 8
  %len1 = call i64 @strlen(ptr @str.const.16)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\851")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.16, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D8\B3\D9\851", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %len15 = call i64 @strlen(ptr @str.const.20)
  %len26 = call i64 @strlen(ptr %"\D8\B5\D9\86\D9\81\D9\872")
  %totallen7 = add i64 %len15, %len26
  %bufsize8 = add i64 %totallen7, 1
  %concat.buf9 = alloca i8, i64 %bufsize8, align 1
  %4 = call ptr @memcpy(ptr %concat.buf9, ptr @str.const.20, i64 %len15)
  %dest210 = getelementptr i8, ptr %concat.buf9, i64 %len15
  %copylen211 = add i64 %len26, 1
  %5 = call ptr @memcpy(ptr %dest210, ptr %"\D8\B5\D9\86\D9\81\D9\872", i64 %copylen211)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf9)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\B3\D8\B7\D8\B1_\D8\A2\D8\AE\D8\B1.load" = load i64, ptr %"\D8\B3\D8\B7\D8\B1_\D8\A2\D8\AE\D8\B14", align 8
  %"%\D8\B3\D8\B7\D8\B1_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1.load" = load i64, ptr %"\D8\B3\D8\B7\D8\B1_\D8\A5\D9\86\D8\B4\D8\A7\D8\A13", align 8
  %subtmp = sub i64 %"%\D8\B3\D8\B7\D8\B1_\D8\A2\D8\AE\D8\B1.load", %"%\D8\B3\D8\B7\D8\B1_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1.load"
  %alloca = alloca i64, align 8
  store i64 %subtmp, ptr %alloca, align 8
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\B9\D9\85\D8\B1_\D9\85\D8\B1\D8\AC\D8\B9"(i64 %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B9", i64 %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B9", i64 %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\83", i64 %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\83") {
entry:
  %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\834" = alloca i64, align 8
  store i64 %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\83", ptr %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\834", align 8
  %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\833" = alloca i64, align 8
  store i64 %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\83", ptr %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\833", align 8
  %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B92" = alloca i64, align 8
  store i64 %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B9", ptr %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B92", align 8
  %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B91" = alloca i64, align 8
  store i64 %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B9", ptr %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B91", align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B9.load" = load i64, ptr %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B91", align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\83.load" = load i64, ptr %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\833", align 8
  %cmplttmp = icmp slt i64 %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B9.load", %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\83.load"
  br i1 %cmplttmp, label %then_0, label %merge_1

then_0:                                           ; preds = %entry
  %calltmp = call i64 @"\D8\B3\D8\AC_\D9\85\D8\B4\D9\83\D9\84\D8\A9"(ptr @str.const.34)
  ret i64 0

merge_1:                                          ; preds = %entry
  %"%\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B9.load" = load i64, ptr %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B92", align 8
  %"%\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\83.load" = load i64, ptr %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\834", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\B1\D8\AC\D8\B9.load", %"%\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D9\85\D8\A7\D9\84\D9\83.load"
  br i1 %cmpgttmp, label %then_2, label %merge_3

then_2:                                           ; preds = %merge_1
  %calltmp5 = call i64 @"\D8\B3\D8\AC_\D9\85\D8\B4\D9\83\D9\84\D8\A9"(ptr @str.const.35)
  ret i64 0

merge_3:                                          ; preds = %merge_1
  %calltmp6 = call i64 @"\D8\B3\D8\AC_\D8\B3\D9\84\D9\8A\D9\85"(ptr @str.const.36)
  ret i64 1
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC_\D8\B5\D8\AD\D9\8A\D8\AD"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.37)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.41)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.45)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D9\85\D8\AA\D8\BA\D9\8A\D8\B1"(ptr @str.const.49, ptr @str.const.50, i64 5, i64 25)
  %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load" = load i64, ptr %alloca1, align 8
  %addtmp = add i64 %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load", 1
  store i64 %addtmp, ptr %alloca1, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.51)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp2 = call i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D9\85\D8\AA\D8\BA\D9\8A\D8\B1"(ptr @str.const.55, ptr @str.const.56, i64 8, i64 15)
  %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load3" = load i64, ptr %alloca1, align 8
  %addtmp4 = add i64 %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load3", 1
  store i64 %addtmp4, ptr %alloca1, align 8
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.57)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp5 = call i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D9\85\D8\AA\D8\BA\D9\8A\D8\B1"(ptr @str.const.61, ptr @str.const.56, i64 10, i64 14)
  %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load6" = load i64, ptr %alloca1, align 8
  %addtmp7 = add i64 %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load6", 1
  store i64 %addtmp7, ptr %alloca1, align 8
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.63)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.67)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp8 = call i64 @"\D8\B3\D8\AC_\D9\81\D8\AD\D8\B5"(ptr @str.const.55, ptr @str.const.72)
  %calltmp9 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\B9\D9\85\D8\B1_\D9\85\D8\B1\D8\AC\D8\B9"(i64 8, i64 15, i64 5, i64 25)
  %alloca10 = alloca i64, align 8
  store i64 %calltmp9, ptr %alloca10, align 8
  %"%\D9\861.load" = load i64, ptr %alloca10, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\861.load", 1
  br i1 %cmpeqtmp, label %then_4, label %else_6

then_4:                                           ; preds = %entry
  %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load11" = load i64, ptr %alloca1, align 8
  %addtmp12 = add i64 %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load11", 1
  store i64 %addtmp12, ptr %alloca1, align 8
  br label %merge_5

else_6:                                           ; preds = %entry
  %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load" = load i64, ptr %alloca, align 8
  %addtmp13 = add i64 %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load", 1
  store i64 %addtmp13, ptr %alloca, align 8
  br label %merge_5

merge_5:                                          ; preds = %else_6, %then_4
  %calltmp14 = call i64 @"\D8\B3\D8\AC_\D9\81\D8\AD\D8\B5"(ptr @str.const.61, ptr @str.const.72)
  %calltmp15 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\B9\D9\85\D8\B1_\D9\85\D8\B1\D8\AC\D8\B9"(i64 10, i64 14, i64 5, i64 25)
  %alloca16 = alloca i64, align 8
  store i64 %calltmp15, ptr %alloca16, align 8
  %"%\D9\862.load" = load i64, ptr %alloca16, align 8
  %cmpeqtmp17 = icmp eq i64 %"%\D9\862.load", 1
  br i1 %cmpeqtmp17, label %then_7, label %else_9

then_7:                                           ; preds = %merge_5
  %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load18" = load i64, ptr %alloca1, align 8
  %addtmp19 = add i64 %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load18", 1
  store i64 %addtmp19, ptr %alloca1, align 8
  br label %merge_8

else_9:                                           ; preds = %merge_5
  %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load20" = load i64, ptr %alloca, align 8
  %addtmp21 = add i64 %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load20", 1
  store i64 %addtmp21, ptr %alloca, align 8
  br label %merge_8

merge_8:                                          ; preds = %else_9, %then_7
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.75)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.79)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp22 = call i64 @"\D8\B3\D8\AC_\D9\81\D8\AD\D8\B5"(ptr @str.const.83, ptr @str.const.84)
  %calltmp23 = call i64 @"\D8\B3\D8\AC_\D8\B3\D9\84\D9\8A\D9\85"(ptr @str.const.85)
  %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load24" = load i64, ptr %alloca1, align 8
  %addtmp25 = add i64 %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load24", 1
  store i64 %addtmp25, ptr %alloca1, align 8
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.86)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.90)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load26" = load i64, ptr %alloca, align 8
  %cmpeqtmp27 = icmp eq i64 %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load26", 0
  br i1 %cmpeqtmp27, label %then_10, label %else_12

then_10:                                          ; preds = %merge_8
  %calltmp28 = call i64 @"\D8\B3\D8\AC_\D8\B3\D9\84\D9\8A\D9\85"(ptr @str.const.100)
  br label %merge_11

else_12:                                          ; preds = %merge_8
  %calltmp29 = call i64 @"\D8\B3\D8\AC_\D9\85\D8\B4\D9\83\D9\84\D8\A9"(ptr @str.const.101)
  br label %merge_11

merge_11:                                         ; preds = %else_12, %then_10
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.102)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\85\D8\B1\D8\AC\D8\B9_\D9\85\D8\B9\D9\84\D9\82"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.106)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.110)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.45)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D9\85\D8\AA\D8\BA\D9\8A\D8\B1"(ptr @str.const.118, ptr @str.const.119, i64 10, i64 15)
  %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load" = load i64, ptr %alloca1, align 8
  %addtmp = add i64 %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load", 1
  store i64 %addtmp, ptr %alloca1, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.120)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp2 = call i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D9\85\D8\AA\D8\BA\D9\8A\D8\B1"(ptr @str.const.124, ptr @str.const.125, i64 12, i64 25)
  %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load3" = load i64, ptr %alloca1, align 8
  %addtmp4 = add i64 %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load3", 1
  store i64 %addtmp4, ptr %alloca1, align 8
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.126)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.67)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp5 = call i64 @"\D8\B3\D8\AC_\D9\81\D8\AD\D8\B5"(ptr @str.const.124, ptr @str.const.135)
  %calltmp6 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\B9\D9\85\D8\B1_\D9\85\D8\B1\D8\AC\D8\B9"(i64 12, i64 25, i64 10, i64 15)
  %alloca7 = alloca i64, align 8
  store i64 %calltmp6, ptr %alloca7, align 8
  %"%\D9\863.load" = load i64, ptr %alloca7, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\863.load", 1
  br i1 %cmpeqtmp, label %then_13, label %else_15

then_13:                                          ; preds = %entry
  %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load8" = load i64, ptr %alloca1, align 8
  %addtmp9 = add i64 %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load8", 1
  store i64 %addtmp9, ptr %alloca1, align 8
  br label %merge_14

else_15:                                          ; preds = %entry
  %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load" = load i64, ptr %alloca, align 8
  %addtmp10 = add i64 %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load", 1
  store i64 %addtmp10, ptr %alloca, align 8
  br label %merge_14

merge_14:                                         ; preds = %else_15, %then_13
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.136)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.140)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.144)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.148)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.152)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.156)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load11" = load i64, ptr %alloca, align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load11", 0
  br i1 %cmpgttmp, label %then_16, label %merge_17

then_16:                                          ; preds = %merge_14
  %calltmp12 = call i64 @"\D8\B3\D8\AC_\D9\85\D8\B4\D9\83\D9\84\D8\A9"(ptr @str.const.163)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.164)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_17

merge_17:                                         ; preds = %then_16, %merge_14
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.168)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D8\AA\D8\B9\D8\A7\D8\B1\D8\B6_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.172)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.176)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.180)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D9\85\D8\AA\D8\BA\D9\8A\D8\B1"(ptr @str.const.50, ptr @str.const.185, i64 1, i64 30)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.186)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp2 = call i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D9\85\D8\AA\D8\BA\D9\8A\D8\B1"(ptr @str.const.190, ptr @str.const.191, i64 5, i64 20)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.192)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp3 = call i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D9\85\D8\AA\D8\BA\D9\8A\D8\B1"(ptr @str.const.196, ptr @str.const.197, i64 10, i64 18)
  %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load" = load i64, ptr %alloca1, align 8
  %addtmp = add i64 %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load", 3
  store i64 %addtmp, ptr %alloca1, align 8
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.198)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.67)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp4 = call i64 @"\D8\B3\D8\AC_\D9\81\D8\AD\D8\B5"(ptr @str.const.190, ptr @str.const.207)
  %calltmp5 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\B9\D9\85\D8\B1_\D9\85\D8\B1\D8\AC\D8\B9"(i64 5, i64 20, i64 1, i64 30)
  %alloca6 = alloca i64, align 8
  store i64 %calltmp5, ptr %alloca6, align 8
  %"%\D9\864.load" = load i64, ptr %alloca6, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\864.load", 1
  br i1 %cmpeqtmp, label %then_18, label %else_20

then_18:                                          ; preds = %entry
  %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load7" = load i64, ptr %alloca1, align 8
  %addtmp8 = add i64 %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load7", 1
  store i64 %addtmp8, ptr %alloca1, align 8
  br label %merge_19

else_20:                                          ; preds = %entry
  %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load" = load i64, ptr %alloca, align 8
  %addtmp9 = add i64 %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load", 1
  store i64 %addtmp9, ptr %alloca, align 8
  br label %merge_19

merge_19:                                         ; preds = %else_20, %then_18
  %calltmp10 = call i64 @"\D8\B3\D8\AC_\D9\81\D8\AD\D8\B5"(ptr @str.const.196, ptr @str.const.207)
  %calltmp11 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\B9\D9\85\D8\B1_\D9\85\D8\B1\D8\AC\D8\B9"(i64 10, i64 18, i64 1, i64 30)
  %alloca12 = alloca i64, align 8
  store i64 %calltmp11, ptr %alloca12, align 8
  %"%\D9\865.load" = load i64, ptr %alloca12, align 8
  %cmpeqtmp13 = icmp eq i64 %"%\D9\865.load", 1
  br i1 %cmpeqtmp13, label %then_21, label %else_23

then_21:                                          ; preds = %merge_19
  %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load14" = load i64, ptr %alloca1, align 8
  %addtmp15 = add i64 %"%\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA.load14", 1
  store i64 %addtmp15, ptr %alloca1, align 8
  br label %merge_22

else_23:                                          ; preds = %merge_19
  %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load16" = load i64, ptr %alloca, align 8
  %addtmp17 = add i64 %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load16", 1
  store i64 %addtmp17, ptr %alloca, align 8
  br label %merge_22

merge_22:                                         ; preds = %else_23, %then_21
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.210)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.214)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca18 = alloca i64, align 8
  store i64 10, ptr %alloca18, align 8
  %alloca19 = alloca i64, align 8
  store i64 18, ptr %alloca19, align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\AA\D8\AF\D8\A7\D8\AE\D9\84.load" = load i64, ptr %alloca18, align 8
  %cmplttmp = icmp slt i64 %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\AA\D8\AF\D8\A7\D8\AE\D9\84.load", 20
  br i1 %cmplttmp, label %then_24, label %merge_25

then_24:                                          ; preds = %merge_22
  %calltmp20 = call i64 @"\D8\B3\D8\AC_\D9\85\D8\B4\D9\83\D9\84\D8\A9"(ptr @str.const.218)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load21" = load i64, ptr %alloca, align 8
  %addtmp22 = add i64 %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load21", 1
  store i64 %addtmp22, ptr %alloca, align 8
  br label %merge_25

merge_25:                                         ; preds = %then_24, %merge_22
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.223)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.140)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.231)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.235)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.239)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.243)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.247)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.254)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.258)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D8\AF\D8\A7\D9\84\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.262)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.266)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 0, ptr %alloca2, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.270)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.274)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.278)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.282)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.286)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.290)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.294)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.298)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.302)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\81\D8\AD\D8\B5.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\81\D8\AD\D8\B5.load", 5
  store i64 %addtmp, ptr %alloca, align 8
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.306)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.310)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D8\B3\D8\AC_\D9\81\D8\AD\D8\B5"(ptr @str.const.314, ptr @str.const.315)
  %calltmp3 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\B9\D9\85\D8\B1_\D9\85\D8\B1\D8\AC\D8\B9"(i64 5, i64 30, i64 2, i64 45)
  %alloca4 = alloca i64, align 8
  store i64 %calltmp3, ptr %alloca4, align 8
  %"%\D8\B11.load" = load i64, ptr %alloca4, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B11.load", 1
  br i1 %cmpeqtmp, label %then_26, label %else_28

then_26:                                          ; preds = %entry
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\81\D8\AD\D8\B5.load5" = load i64, ptr %alloca, align 8
  %addtmp6 = add i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\81\D8\AD\D8\B5.load5", 1
  store i64 %addtmp6, ptr %alloca, align 8
  br label %merge_27

else_28:                                          ; preds = %entry
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\85\D8\B4\D8\A7\D9\83\D9\84.load" = load i64, ptr %alloca1, align 8
  %addtmp7 = add i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\85\D8\B4\D8\A7\D9\83\D9\84.load", 1
  store i64 %addtmp7, ptr %alloca1, align 8
  br label %merge_27

merge_27:                                         ; preds = %else_28, %then_26
  %calltmp8 = call i64 @"\D8\B3\D8\AC_\D9\81\D8\AD\D8\B5"(ptr @str.const.316, ptr @str.const.317)
  %calltmp9 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\B9\D9\85\D8\B1_\D9\85\D8\B1\D8\AC\D8\B9"(i64 10, i64 40, i64 3, i64 48)
  %alloca10 = alloca i64, align 8
  store i64 %calltmp9, ptr %alloca10, align 8
  %"%\D8\B12.load" = load i64, ptr %alloca10, align 8
  %cmpeqtmp11 = icmp eq i64 %"%\D8\B12.load", 1
  br i1 %cmpeqtmp11, label %then_29, label %else_31

then_29:                                          ; preds = %merge_27
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\81\D8\AD\D8\B5.load12" = load i64, ptr %alloca, align 8
  %addtmp13 = add i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\81\D8\AD\D8\B5.load12", 1
  store i64 %addtmp13, ptr %alloca, align 8
  br label %merge_30

else_31:                                          ; preds = %merge_27
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\85\D8\B4\D8\A7\D9\83\D9\84.load14" = load i64, ptr %alloca1, align 8
  %addtmp15 = add i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\85\D8\B4\D8\A7\D9\83\D9\84.load14", 1
  store i64 %addtmp15, ptr %alloca1, align 8
  br label %merge_30

merge_30:                                         ; preds = %else_31, %then_29
  %calltmp16 = call i64 @"\D8\B3\D8\AC_\D9\81\D8\AD\D8\B5"(ptr @str.const.318, ptr @str.const.319)
  %calltmp17 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\B9\D9\85\D8\B1_\D9\85\D8\B1\D8\AC\D8\B9"(i64 35, i64 42, i64 3, i64 48)
  %alloca18 = alloca i64, align 8
  store i64 %calltmp17, ptr %alloca18, align 8
  %"%\D8\B13.load" = load i64, ptr %alloca18, align 8
  %cmpeqtmp19 = icmp eq i64 %"%\D8\B13.load", 1
  br i1 %cmpeqtmp19, label %then_32, label %else_34

then_32:                                          ; preds = %merge_30
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\81\D8\AD\D8\B5.load20" = load i64, ptr %alloca, align 8
  %addtmp21 = add i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\81\D8\AD\D8\B5.load20", 1
  store i64 %addtmp21, ptr %alloca, align 8
  br label %merge_33

else_34:                                          ; preds = %merge_30
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\85\D8\B4\D8\A7\D9\83\D9\84.load22" = load i64, ptr %alloca1, align 8
  %addtmp23 = add i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\85\D8\B4\D8\A7\D9\83\D9\84.load22", 1
  store i64 %addtmp23, ptr %alloca1, align 8
  br label %merge_33

merge_33:                                         ; preds = %else_34, %then_32
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.320)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.324)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br i1 true, label %then_35, label %merge_36

then_35:                                          ; preds = %merge_33
  %calltmp24 = call i64 @"\D8\B3\D8\AC_\D9\85\D8\B4\D9\83\D9\84\D8\A9"(ptr @str.const.328)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.329)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\85\D8\B4\D8\A7\D9\83\D9\84.load25" = load i64, ptr %alloca1, align 8
  %addtmp26 = add i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\85\D8\B4\D8\A7\D9\83\D9\84.load25", 1
  store i64 %addtmp26, ptr %alloca1, align 8
  br label %merge_36

merge_36:                                         ; preds = %then_35, %merge_33
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.333)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.337)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp27 = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1"(ptr @str.const.341)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1.load" = load i64, ptr %alloca2, align 8
  %addtmp28 = add i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1.load", 1
  store i64 %addtmp28, ptr %alloca2, align 8
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.342)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.346)
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %40 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %42 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\85\D8\B4\D8\A7\D9\83\D9\84.load29" = load i64, ptr %alloca1, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\85\D8\B4\D8\A7\D9\83\D9\84.load29", 0
  br i1 %cmpgttmp, label %then_37, label %else_39

then_37:                                          ; preds = %merge_36
  %43 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.359)
  %44 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_38

else_39:                                          ; preds = %merge_36
  %45 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.363)
  %46 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_38

merge_38:                                         ; preds = %else_39, %then_37
  %47 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.367)
  %48 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %49 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.371)
  %50 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3_\D9\81"(ptr %"\D9\85\D8\A8\D8\AF\D8\A3", i64 %"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82") {
entry:
  %"\D8\AA\D8\B7\D8\A8\D9\8A\D9\822" = alloca i64, align 8
  store i64 %"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82", ptr %"\D8\AA\D8\B7\D8\A8\D9\8A\D9\822", align 8
  %"\D9\85\D8\A8\D8\AF\D8\A31" = alloca ptr, align 8
  store ptr %"\D9\85\D8\A8\D8\AF\D8\A3", ptr %"\D9\85\D8\A8\D8\AF\D8\A31", align 8
  %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82.load" = load i64, ptr %"\D8\AA\D8\B7\D8\A8\D9\8A\D9\822", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82.load", 1
  br i1 %cmpeqtmp, label %then_40, label %else_42

then_40:                                          ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const.375)
  %len2 = call i64 @strlen(ptr %"\D9\85\D8\A8\D8\AF\D8\A31")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.375, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\85\D8\A8\D8\AF\D8\A31", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_41

else_42:                                          ; preds = %entry
  %len13 = call i64 @strlen(ptr @str.const.379)
  %len24 = call i64 @strlen(ptr %"\D9\85\D8\A8\D8\AF\D8\A31")
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %4 = call ptr @memcpy(ptr %concat.buf7, ptr @str.const.379, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %5 = call ptr @memcpy(ptr %dest28, ptr %"\D9\85\D8\A8\D8\AF\D8\A31", i64 %copylen29)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf7)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_41

merge_41:                                         ; preds = %else_42, %then_40
  ret i64 0
}

define i64 @"\D9\85\D9\84\D8\AE\D8\B5_\D9\81\D8\A7\D8\AD\D8\B5"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.383)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.387)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3_\D9\81"(ptr @str.const.391, i64 1)
  %"%\D8\B9.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\B9.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp1 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3_\D9\81"(ptr @str.const.392, i64 1)
  %"%\D8\B9.load2" = load i64, ptr %alloca, align 8
  %addtmp3 = add i64 %"%\D8\B9.load2", 1
  store i64 %addtmp3, ptr %alloca, align 8
  %calltmp4 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3_\D9\81"(ptr @str.const.393, i64 1)
  %"%\D8\B9.load5" = load i64, ptr %alloca, align 8
  %addtmp6 = add i64 %"%\D8\B9.load5", 1
  store i64 %addtmp6, ptr %alloca, align 8
  %calltmp7 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3_\D9\81"(ptr @str.const.394, i64 1)
  %"%\D8\B9.load8" = load i64, ptr %alloca, align 8
  %addtmp9 = add i64 %"%\D8\B9.load8", 1
  store i64 %addtmp9, ptr %alloca, align 8
  %calltmp10 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3_\D9\81"(ptr @str.const.395, i64 1)
  %"%\D8\B9.load11" = load i64, ptr %alloca, align 8
  %addtmp12 = add i64 %"%\D8\B9.load11", 1
  store i64 %addtmp12, ptr %alloca, align 8
  %calltmp13 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3_\D9\81"(ptr @str.const.396, i64 1)
  %"%\D8\B9.load14" = load i64, ptr %alloca, align 8
  %addtmp15 = add i64 %"%\D8\B9.load14", 1
  store i64 %addtmp15, ptr %alloca, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.397)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.405)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.409)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.413)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.417)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC_\D8\B5\D8\AD\D9\8A\D8\AD"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.421)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp1 = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\85\D8\B1\D8\AC\D8\B9_\D9\85\D8\B9\D9\84\D9\82"()
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.425)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp2 = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D8\AA\D8\B9\D8\A7\D8\B1\D8\B6_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9"()
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.429)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp3 = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D8\AF\D8\A7\D9\84\D8\A9"()
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.433)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp4 = call i64 @"\D9\85\D9\84\D8\AE\D8\B5_\D9\81\D8\A7\D8\AD\D8\B5"()
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.437)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.441)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

declare i64 @strlen(ptr)

declare ptr @memcpy(ptr, ptr, i64)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
