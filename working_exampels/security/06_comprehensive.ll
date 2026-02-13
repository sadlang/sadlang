; ModuleID = '06_comprehensive'
source_filename = "06_comprehensive"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [94 x i8] c"\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.7 = private unnamed_addr constant [3 x i8] c"  \00", align 1
@str.const.15 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\93] \00", align 1
@str.const.19 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\97] \00", align 1
@str.const.23 = private unnamed_addr constant [56 x i8] c"\D8\A7\D9\84\D9\82\D8\B3\D9\85 1: \D8\A7\D9\84\D8\AA\D8\AD\D9\82\D9\82 \D9\85\D9\86 \D9\83\D9\84\D9\85\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\B1\D9\88\D8\B1\00", align 1
@str.const.24 = private unnamed_addr constant [32 x i8] c"\D9\83\D9\84\D9\85\D8\A9_\D8\B3\D8\B1_\D9\82\D9\88\D9\8A\D8\A9_\D8\AC\D8\AF\D8\A7\D9\8B\00", align 1
@str.const.25 = private unnamed_addr constant [32 x i8] c"\D9\83\D9\84\D9\85\D8\A9 \D9\82\D9\88\D9\8A\D8\A9 (\D8\B7\D9\88\D9\84 >= 8)\00", align 1
@str.const.26 = private unnamed_addr constant [9 x i8] c"\D9\82\D8\B5\D9\8A\D8\B1\00", align 1
@str.const.27 = private unnamed_addr constant [42 x i8] c"\D9\83\D9\84\D9\85\D8\A9 \D9\82\D8\B5\D9\8A\D8\B1\D8\A9 \D8\B1\D9\8F\D9\81\D8\B6\D8\AA (\D8\B5\D8\AD\D9\8A\D8\AD)\00", align 1
@str.const.28 = private unnamed_addr constant [41 x i8] c"\D9\83\D9\84\D9\85\D8\A9 \D9\82\D8\B5\D9\8A\D8\B1\D8\A9 \D9\82\D9\8F\D8\A8\D9\84\D8\AA (\D8\AE\D8\B7\D8\A3!)\00", align 1
@str.const.29 = private unnamed_addr constant [9 x i8] c"12345678\00", align 1
@str.const.30 = private unnamed_addr constant [44 x i8] c"\D9\83\D9\84\D9\85\D8\A9 8 \D8\A3\D8\AD\D8\B1\D9\81 (\D8\A7\D9\84\D8\AD\D8\AF \D8\A7\D9\84\D8\A3\D8\AF\D9\86\D9\89)\00", align 1
@str.const.31 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.35 = private unnamed_addr constant [41 x i8] c"  \D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA \D9\83\D9\84\D9\85\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\B1\D9\88\D8\B1: \00", align 1
@str.const.36 = private unnamed_addr constant [3 x i8] c"/3\00", align 1
@str.const.39 = private unnamed_addr constant [51 x i8] c"\D8\A7\D9\84\D9\82\D8\B3\D9\85 2: \D8\A7\D9\84\D8\AA\D8\B4\D9\81\D9\8A\D8\B1 \D9\88\D9\81\D9\83 \D8\A7\D9\84\D8\AA\D8\B4\D9\81\D9\8A\D8\B1\00", align 1
@str.const.40 = private unnamed_addr constant [24 x i8] c"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA \D8\AD\D8\B3\D8\A7\D8\B3\D8\A9\00", align 1
@str.const.41 = private unnamed_addr constant [18 x i8] c"\D9\85\D9\81\D8\AA\D8\A7\D8\AD_\D8\B3\D8\B1\D9\8A\00", align 1
@str.const.42 = private unnamed_addr constant [26 x i8] c"  \D8\A7\D9\84\D9\86\D8\B5 \D8\A7\D9\84\D8\A3\D8\B5\D9\84\D9\8A: \00", align 1
@str.const.46 = private unnamed_addr constant [17 x i8] c"  \D8\A7\D9\84\D9\85\D8\B4\D9\81\D8\B1: \00", align 1
@str.const.50 = private unnamed_addr constant [19 x i8] c"  \D8\A7\D9\84\D9\85\D9\81\D9\83\D9\88\D9\83: \00", align 1
@str.const.54 = private unnamed_addr constant [42 x i8] c"\D8\AA\D8\B4\D9\81\D9\8A\D8\B1 + \D9\81\D9\83 = \D8\A7\D9\84\D9\86\D8\B5 \D8\A7\D9\84\D8\A3\D8\B5\D9\84\D9\8A\00", align 1
@str.const.55 = private unnamed_addr constant [29 x i8] c"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9 \D8\B3\D8\B1\D9\8A\D8\A9 \D8\AC\D8\AF\D8\A7\D9\8B\00", align 1
@str.const.56 = private unnamed_addr constant [27 x i8] c"\D9\85\D9\81\D8\AA\D8\A7\D8\AD_\D8\A2\D8\AE\D8\B1_\D8\B7\D9\88\D9\8A\D9\84\00", align 1
@str.const.57 = private unnamed_addr constant [42 x i8] c"\D9\85\D9\81\D8\AA\D8\A7\D8\AD \D9\85\D8\AE\D8\AA\D9\84\D9\81 \D9\8A\D8\B9\D9\85\D9\84 \D8\A3\D9\8A\D8\B6\D8\A7\D9\8B\00", align 1
@str.const.58 = private unnamed_addr constant [16 x i8] c"Hello World 123\00", align 1
@str.const.59 = private unnamed_addr constant [7 x i8] c"key123\00", align 1
@str.const.60 = private unnamed_addr constant [29 x i8] c"\D9\86\D8\B5 \D8\A5\D9\86\D8\AC\D9\84\D9\8A\D8\B2\D9\8A \D9\8A\D8\B9\D9\85\D9\84\00", align 1
@str.const.61 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.65 = private unnamed_addr constant [32 x i8] c"  \D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA \D8\A7\D9\84\D8\AA\D8\B4\D9\81\D9\8A\D8\B1: \00", align 1
@str.const.69 = private unnamed_addr constant [41 x i8] c"\D8\A7\D9\84\D9\82\D8\B3\D9\85 3: \D8\AD\D9\85\D8\A7\D9\8A\D8\A9 \D9\85\D9\86 \D8\A7\D9\84\D8\AD\D9\82\D9\86\00", align 1
@str.const.70 = private unnamed_addr constant [26 x i8] c"<script>alert(1)</script>\00", align 1
@str.const.71 = private unnamed_addr constant [11 x i8] c"  \D9\82\D8\A8\D9\84: \00", align 1
@str.const.75 = private unnamed_addr constant [11 x i8] c"  \D8\A8\D8\B9\D8\AF: \00", align 1
@str.const.79 = private unnamed_addr constant [39 x i8] c"HTML entities \D8\A3\D8\B7\D9\88\D9\84 \D9\85\D9\86 \D8\A7\D9\84\D8\A3\D8\B5\D9\84\00", align 1
@str.const.80 = private unnamed_addr constant [33 x i8] c"\D8\A7\D8\B3\D9\85 \D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85: \22admin\22\00", align 1
@str.const.89 = private unnamed_addr constant [50 x i8] c"\D8\B9\D9\84\D8\A7\D9\85\D8\A7\D8\AA \D8\A7\D9\84\D8\A7\D9\82\D8\AA\D8\A8\D8\A7\D8\B3 \D8\AA\D9\85 \D8\AA\D9\86\D8\B8\D9\8A\D9\81\D9\87\D8\A7\00", align 1
@str.const.90 = private unnamed_addr constant [32 x i8] c"\D9\86\D8\B5 \D8\B9\D8\A7\D8\AF\D9\8A \D8\A8\D8\AF\D9\88\D9\86 \D8\B1\D9\85\D9\88\D8\B2\00", align 1
@str.const.91 = private unnamed_addr constant [30 x i8] c"\D9\86\D8\B5 \D8\B9\D8\A7\D8\AF\D9\8A \D9\84\D8\A7 \D9\8A\D8\AA\D8\BA\D9\8A\D8\B1\00", align 1
@str.const.92 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.96 = private unnamed_addr constant [32 x i8] c"  \D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA \D8\A7\D9\84\D8\AA\D9\86\D8\B8\D9\8A\D9\81: \00", align 1
@str.const.100 = private unnamed_addr constant [45 x i8] c"\D8\A7\D9\84\D9\82\D8\B3\D9\85 4: \D9\87\D8\A7\D8\B4 \D9\83\D9\84\D9\85\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\B1\D9\88\D8\B1\00", align 1
@str.const.101 = private unnamed_addr constant [22 x i8] c"\D9\83\D9\84\D9\85\D8\A9_\D9\85\D8\B1\D9\88\D8\B1_123\00", align 1
@str.const.103 = private unnamed_addr constant [22 x i8] c"\D9\83\D9\84\D9\85\D8\A9_\D9\85\D8\B1\D9\88\D8\B1_456\00", align 1
@str.const.104 = private unnamed_addr constant [22 x i8] c"  \D9\87\D8\A7\D8\B4 \D9\83\D9\84\D9\85\D8\A91 = \00", align 1
@str.const.108 = private unnamed_addr constant [22 x i8] c"  \D9\87\D8\A7\D8\B4 \D9\83\D9\84\D9\85\D8\A92 = \00", align 1
@str.const.112 = private unnamed_addr constant [22 x i8] c"  \D9\87\D8\A7\D8\B4 \D9\83\D9\84\D9\85\D8\A93 = \00", align 1
@str.const.116 = private unnamed_addr constant [40 x i8] c"\D9\86\D9\81\D8\B3 \D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9 = \D9\86\D9\81\D8\B3 \D8\A7\D9\84\D9\87\D8\A7\D8\B4\00", align 1
@str.const.117 = private unnamed_addr constant [44 x i8] c"\D9\83\D9\84\D9\85\D8\A7\D8\AA \D9\85\D8\AE\D8\AA\D9\84\D9\81\D8\A9 = \D9\87\D8\A7\D8\B4 \D9\85\D8\AE\D8\AA\D9\84\D9\81\00", align 1
@str.const.119 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.120 = private unnamed_addr constant [21 x i8] c"  \D9\87\D8\A7\D8\B4 \D9\81\D8\A7\D8\B1\D8\BA = \00", align 1
@str.const.124 = private unnamed_addr constant [30 x i8] c"\D9\87\D8\A7\D8\B4 \D9\86\D8\B5 \D9\81\D8\A7\D8\B1\D8\BA \D9\8A\D8\B9\D9\85\D9\84\00", align 1
@str.const.125 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.129 = private unnamed_addr constant [28 x i8] c"  \D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA \D8\A7\D9\84\D9\87\D8\A7\D8\B4: \00", align 1
@str.const.133 = private unnamed_addr constant [45 x i8] c"\D8\A7\D9\84\D9\82\D8\B3\D9\85 5: \D8\AA\D8\B1\D9\85\D9\8A\D8\B2 Base64 \D9\88\D8\A7\D9\84\D9\88\D9\82\D8\AA\00", align 1
@str.const.134 = private unnamed_addr constant [12 x i8] c"Hello World\00", align 1
@str.const.135 = private unnamed_addr constant [9 x i8] c"  \D9\86\D8\B5: \00", align 1
@str.const.139 = private unnamed_addr constant [11 x i8] c"  Base64: \00", align 1
@str.const.143 = private unnamed_addr constant [27 x i8] c"\D8\AA\D8\B1\D9\85\D9\8A\D8\B2 Base64 \D9\8A\D8\B9\D9\85\D9\84\00", align 1
@str.const.144 = private unnamed_addr constant [26 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7 \D8\A8\D8\A7\D9\84\D8\B9\D8\A7\D9\84\D9\85\00", align 1
@str.const.153 = private unnamed_addr constant [34 x i8] c"\D8\AA\D8\B1\D9\85\D9\8A\D8\B2 \D9\86\D8\B5 \D8\B9\D8\B1\D8\A8\D9\8A \D9\8A\D8\B9\D9\85\D9\84\00", align 1
@str.const.154 = private unnamed_addr constant [16 x i8] c"user:admin:2024\00", align 1
@str.const.155 = private unnamed_addr constant [11 x i8] c"  \D8\B1\D9\85\D8\B2: \00", align 1
@str.const.163 = private unnamed_addr constant [27 x i8] c"\D8\AA\D8\B1\D9\85\D9\8A\D8\B2 \D8\B1\D9\85\D8\B2 \D9\88\D8\B5\D9\88\D9\84\00", align 1
@str.const.164 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.168 = private unnamed_addr constant [30 x i8] c"  \D8\A7\D9\84\D8\B7\D8\A7\D8\A8\D8\B9 \D8\A7\D9\84\D8\B2\D9\85\D9\86\D9\8A: \00", align 1
@str.const.172 = private unnamed_addr constant [27 x i8] c"\D8\B7\D8\A7\D8\A8\D8\B9 \D8\B2\D9\85\D9\86\D9\8A \D9\85\D9\88\D8\AC\D8\A8\00", align 1
@str.const.173 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.177 = private unnamed_addr constant [32 x i8] c"  \D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA \D8\A7\D9\84\D8\AA\D8\B1\D9\85\D9\8A\D8\B2: \00", align 1
@str.const.178 = private unnamed_addr constant [3 x i8] c"/4\00", align 1
@str.const.181 = private unnamed_addr constant [50 x i8] c"\D8\A7\D9\84\D9\82\D8\B3\D9\85 6: \D8\A7\D9\84\D8\AA\D8\A3\D9\83\D9\8A\D8\AF\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\AA\D9\82\D8\AF\D9\85\D8\A9\00", align 1
@str.const.182 = private unnamed_addr constant [7 x i8] c"1 == 1\00", align 1
@str.const.183 = private unnamed_addr constant [12 x i8] c"7 * 8 == 56\00", align 1
@str.const.184 = private unnamed_addr constant [11 x i8] c"1000 > 100\00", align 1
@str.const.185 = private unnamed_addr constant [13 x i8] c"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\00", align 1
@str.const.187 = private unnamed_addr constant [27 x i8] c"\D9\86\D8\B5 \D9\83\D9\84\D8\A7 == \D9\86\D8\B5 \D9\83\D9\84\D8\A8\00", align 1
@str.const.188 = private unnamed_addr constant [22 x i8] c"42 \D9\85\D9\86 \D9\86\D9\88\D8\B9 \D8\B1\D9\82\D9\85\00", align 1
@str.const.189 = private unnamed_addr constant [28 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7 \D9\85\D9\86 \D9\86\D9\88\D8\B9 \D9\86\D8\B5\00", align 1
@str.const.190 = private unnamed_addr constant [37 x i8] c"\D8\B3\D9\84\D8\B3\D9\84\D8\A9 5 \D8\AA\D8\A3\D9\83\D9\8A\D8\AF\D8\A7\D8\AA \D8\A3\D9\83\D8\A8\D8\B1\00", align 1
@str.const.191 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.195 = private unnamed_addr constant [36 x i8] c"  \D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA \D8\A7\D9\84\D8\AA\D8\A3\D9\83\D9\8A\D8\AF\D8\A7\D8\AA: \00", align 1
@str.const.196 = private unnamed_addr constant [3 x i8] c"/7\00", align 1
@str.const.199 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.203 = private unnamed_addr constant [121 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.207 = private unnamed_addr constant [64 x i8] c"\E2\95\91       \D9\85\D9\84\D8\AE\D8\B5 \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D8\A3\D9\85\D8\A7\D9\86 \D8\A7\D9\84\D8\B4\D8\A7\D9\85\D9\84       \E2\95\91\00", align 1
@str.const.211 = private unnamed_addr constant [121 x i8] c"\E2\95\A0\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\A3\00", align 1
@str.const.215 = private unnamed_addr constant [57 x i8] c"\E2\95\91  1. \D9\83\D9\84\D9\85\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\B1\D9\88\D8\B1    : 3/3  \E2\9C\93        \E2\95\91\00", align 1
@str.const.219 = private unnamed_addr constant [53 x i8] c"\E2\95\91  2. \D8\A7\D9\84\D8\AA\D8\B4\D9\81\D9\8A\D8\B1         : 3/3  \E2\9C\93        \E2\95\91\00", align 1
@str.const.223 = private unnamed_addr constant [53 x i8] c"\E2\95\91  3. \D8\A7\D9\84\D8\AA\D9\86\D8\B8\D9\8A\D9\81         : 3/3  \E2\9C\93        \E2\95\91\00", align 1
@str.const.227 = private unnamed_addr constant [51 x i8] c"\E2\95\91  4. \D8\A7\D9\84\D9\87\D8\A7\D8\B4           : 3/3  \E2\9C\93        \E2\95\91\00", align 1
@str.const.231 = private unnamed_addr constant [59 x i8] c"\E2\95\91  5. \D8\A7\D9\84\D8\AA\D8\B1\D9\85\D9\8A\D8\B2 \D9\88\D8\A7\D9\84\D9\88\D9\82\D8\AA  : 4/4  \E2\9C\93        \E2\95\91\00", align 1
@str.const.235 = private unnamed_addr constant [55 x i8] c"\E2\95\91  6. \D8\A7\D9\84\D8\AA\D8\A3\D9\83\D9\8A\D8\AF\D8\A7\D8\AA       : 7/7  \E2\9C\93        \E2\95\91\00", align 1
@str.const.243 = private unnamed_addr constant [55 x i8] c"\E2\95\91  \D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9: 23/23 \D9\81\D8\AD\D8\B5                  \E2\95\91\00", align 1
@str.const.247 = private unnamed_addr constant [61 x i8] c"\E2\95\91  \D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9: \D9\86\D8\AC\D8\A7\D8\AD \D9\83\D8\A7\D9\85\D9\84 \E2\9C\93               \E2\95\91\00", align 1
@str.const.251 = private unnamed_addr constant [121 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.259 = private unnamed_addr constant [60 x i8] c"\E2\95\91      \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D8\A3\D9\85\D8\A7\D9\86 \D8\A7\D9\84\D8\B4\D8\A7\D9\85\D9\84             \E2\95\91\00", align 1

define i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\82\D8\B3\D9\85"(ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86") {
entry:
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\861" = alloca ptr, align 8
  store ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len1 = call i64 @strlen(ptr @str.const.7)
  %len2 = call i64 @strlen(ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %4 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.7, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %5 = call ptr @memcpy(ptr %dest2, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", i64 %copylen2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr %"\D9\88\D8\B5\D9\81", i64 %"\D8\AD\D8\A7\D9\84\D8\AA\D9\87") {
entry:
  %"\D8\AD\D8\A7\D9\84\D8\AA\D9\872" = alloca i64, align 8
  store i64 %"\D8\AD\D8\A7\D9\84\D8\AA\D9\87", ptr %"\D8\AD\D8\A7\D9\84\D8\AA\D9\872", align 8
  %"\D9\88\D8\B5\D9\811" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81", ptr %"\D9\88\D8\B5\D9\811", align 8
  %"%\D8\AD\D8\A7\D9\84\D8\AA\D9\87.load" = load i64, ptr %"\D8\AD\D8\A7\D9\84\D8\AA\D9\872", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AD\D8\A7\D9\84\D8\AA\D9\87.load", 1
  br i1 %cmpeqtmp, label %then_0, label %else_2

then_0:                                           ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const.15)
  %len2 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\811")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.15, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\88\D8\B5\D9\811", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_1

else_2:                                           ; preds = %entry
  %len13 = call i64 @strlen(ptr @str.const.19)
  %len24 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\811")
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %4 = call ptr @memcpy(ptr %concat.buf7, ptr @str.const.19, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %5 = call ptr @memcpy(ptr %dest28, ptr %"\D9\88\D8\B5\D9\811", i64 %copylen29)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf7)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_1

merge_1:                                          ; preds = %else_2, %then_0
  ret i64 0
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\B7\D9\88\D9\84_\D9\83\D9\84\D9\85\D8\A9"(ptr %"\D9\83\D9\84\D9\85\D9\87", i64 %"\D8\AD\D8\AF_\D8\A3\D8\AF\D9\86\D9\89") {
entry:
  %"\D8\AD\D8\AF_\D8\A3\D8\AF\D9\86\D9\892" = alloca i64, align 8
  store i64 %"\D8\AD\D8\AF_\D8\A3\D8\AF\D9\86\D9\89", ptr %"\D8\AD\D8\AF_\D8\A3\D8\AF\D9\86\D9\892", align 8
  %"\D9\83\D9\84\D9\85\D9\871" = alloca ptr, align 8
  store ptr %"\D9\83\D9\84\D9\85\D9\87", ptr %"\D9\83\D9\84\D9\85\D9\871", align 8
  %strlen.ret = call i64 @strlen(ptr %"\D9\83\D9\84\D9\85\D9\871")
  %alloca = alloca i64, align 8
  store i64 %strlen.ret, ptr %alloca, align 8
  %"%\D8\B7\D9\88\D9\84.load" = load i64, ptr %alloca, align 8
  %"%\D8\AD\D8\AF_\D8\A3\D8\AF\D9\86\D9\89.load" = load i64, ptr %"\D8\AD\D8\AF_\D8\A3\D8\AF\D9\86\D9\892", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B7\D9\88\D9\84.load", %"%\D8\AD\D8\AF_\D8\A3\D8\AF\D9\86\D9\89.load"
  br i1 %cmpgetmp, label %then_3, label %merge_4

then_3:                                           ; preds = %entry
  ret i64 1

merge_4:                                          ; preds = %entry
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\83\D9\84\D9\85\D8\A7\D8\AA_\D9\85\D8\B1\D9\88\D8\B1"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\82\D8\B3\D9\85"(ptr @str.const.23)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca ptr, align 8
  store ptr @str.const.24, ptr %alloca1, align 8
  %calltmp2 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\B7\D9\88\D9\84_\D9\83\D9\84\D9\85\D8\A9"(ptr %alloca1, i64 8)
  %alloca3 = alloca i64, align 8
  store i64 %calltmp2, ptr %alloca3, align 8
  %"%\D9\861.load" = load i64, ptr %alloca3, align 8
  %calltmp4 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.25, i64 %"%\D9\861.load")
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %"%\D9\861.load5" = load i64, ptr %alloca3, align 8
  %addtmp = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load", %"%\D9\861.load5"
  store i64 %addtmp, ptr %alloca, align 8
  %alloca6 = alloca ptr, align 8
  store ptr @str.const.26, ptr %alloca6, align 8
  %calltmp7 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\B7\D9\88\D9\84_\D9\83\D9\84\D9\85\D8\A9"(ptr %alloca6, i64 8)
  %alloca8 = alloca i64, align 8
  store i64 %calltmp7, ptr %alloca8, align 8
  %"%\D9\862.load" = load i64, ptr %alloca8, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\862.load", 0
  br i1 %cmpeqtmp, label %then_5, label %else_7

then_5:                                           ; preds = %entry
  %calltmp9 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.27, i64 1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load10" = load i64, ptr %alloca, align 8
  %addtmp11 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load10", 1
  store i64 %addtmp11, ptr %alloca, align 8
  br label %merge_6

else_7:                                           ; preds = %entry
  %calltmp12 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.28, i64 0)
  br label %merge_6

merge_6:                                          ; preds = %else_7, %then_5
  %alloca13 = alloca ptr, align 8
  store ptr @str.const.29, ptr %alloca13, align 8
  %calltmp14 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\B7\D9\88\D9\84_\D9\83\D9\84\D9\85\D8\A9"(ptr %alloca13, i64 8)
  %alloca15 = alloca i64, align 8
  store i64 %calltmp14, ptr %alloca15, align 8
  %"%\D9\863.load" = load i64, ptr %alloca15, align 8
  %calltmp16 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.30, i64 %"%\D9\863.load")
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load17" = load i64, ptr %alloca, align 8
  %"%\D9\863.load18" = load i64, ptr %alloca15, align 8
  %addtmp19 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load17", %"%\D9\863.load18"
  store i64 %addtmp19, ptr %alloca, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.31)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AA\D8\B4\D9\81\D9\8A\D8\B1"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\82\D8\B3\D9\85"(ptr @str.const.39)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca ptr, align 8
  store ptr @str.const.40, ptr %alloca1, align 8
  %alloca2 = alloca ptr, align 8
  store ptr @str.const.41, ptr %alloca2, align 8
  %encrypt.ret = call ptr @sad_security_encrypt(ptr %alloca1, ptr %alloca2)
  %alloca3 = alloca ptr, align 8
  store ptr %encrypt.ret, ptr %alloca3, align 8
  %len1 = call i64 @strlen(ptr @str.const.42)
  %len2 = call i64 @strlen(ptr %alloca1)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.42, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %alloca1, i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len14 = call i64 @strlen(ptr @str.const.46)
  %len25 = call i64 @strlen(ptr %alloca3)
  %totallen6 = add i64 %len14, %len25
  %bufsize7 = add i64 %totallen6, 1
  %concat.buf8 = alloca i8, i64 %bufsize7, align 1
  %4 = call ptr @memcpy(ptr %concat.buf8, ptr @str.const.46, i64 %len14)
  %dest29 = getelementptr i8, ptr %concat.buf8, i64 %len14
  %copylen210 = add i64 %len25, 1
  %5 = call ptr @memcpy(ptr %dest29, ptr %alloca3, i64 %copylen210)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf8)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %decrypt.ret = call ptr @sad_security_decrypt(ptr %alloca3, ptr %alloca2)
  %alloca11 = alloca ptr, align 8
  store ptr %decrypt.ret, ptr %alloca11, align 8
  %len112 = call i64 @strlen(ptr @str.const.50)
  %len213 = call i64 @strlen(ptr %alloca11)
  %totallen14 = add i64 %len112, %len213
  %bufsize15 = add i64 %totallen14, 1
  %concat.buf16 = alloca i8, i64 %bufsize15, align 1
  %8 = call ptr @memcpy(ptr %concat.buf16, ptr @str.const.50, i64 %len112)
  %dest217 = getelementptr i8, ptr %concat.buf16, i64 %len112
  %copylen218 = add i64 %len213, 1
  %9 = call ptr @memcpy(ptr %dest217, ptr %alloca11, i64 %copylen218)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf16)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  call void @sad_security_assert_equal_str(ptr %alloca11, ptr %alloca1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp19 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.54, i64 1)
  %alloca20 = alloca ptr, align 8
  store ptr @str.const.55, ptr %alloca20, align 8
  %alloca21 = alloca ptr, align 8
  store ptr @str.const.56, ptr %alloca21, align 8
  %encrypt.ret22 = call ptr @sad_security_encrypt(ptr %alloca20, ptr %alloca21)
  %alloca23 = alloca ptr, align 8
  store ptr %encrypt.ret22, ptr %alloca23, align 8
  %decrypt.ret24 = call ptr @sad_security_decrypt(ptr %alloca23, ptr %alloca21)
  %alloca25 = alloca ptr, align 8
  store ptr %decrypt.ret24, ptr %alloca25, align 8
  call void @sad_security_assert_equal_str(ptr %alloca25, ptr %alloca20)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load26" = load i64, ptr %alloca, align 8
  %addtmp27 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load26", 1
  store i64 %addtmp27, ptr %alloca, align 8
  %calltmp28 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.57, i64 1)
  %alloca29 = alloca ptr, align 8
  store ptr @str.const.58, ptr %alloca29, align 8
  %alloca30 = alloca ptr, align 8
  store ptr @str.const.59, ptr %alloca30, align 8
  %encrypt.ret31 = call ptr @sad_security_encrypt(ptr %alloca29, ptr %alloca30)
  %alloca32 = alloca ptr, align 8
  store ptr %encrypt.ret31, ptr %alloca32, align 8
  %decrypt.ret33 = call ptr @sad_security_decrypt(ptr %alloca32, ptr %alloca30)
  %alloca34 = alloca ptr, align 8
  store ptr %decrypt.ret33, ptr %alloca34, align 8
  call void @sad_security_assert_equal_str(ptr %alloca34, ptr %alloca29)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load35" = load i64, ptr %alloca, align 8
  %addtmp36 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load35", 1
  store i64 %addtmp36, ptr %alloca, align 8
  %calltmp37 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.60, i64 1)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.61)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AA\D9\86\D8\B8\D9\8A\D9\81"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\82\D8\B3\D9\85"(ptr @str.const.69)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca ptr, align 8
  store ptr @str.const.70, ptr %alloca1, align 8
  %sanitize.ret = call ptr @sad_security_sanitize(ptr %alloca1)
  %alloca2 = alloca ptr, align 8
  store ptr %sanitize.ret, ptr %alloca2, align 8
  %len1 = call i64 @strlen(ptr @str.const.71)
  %len2 = call i64 @strlen(ptr %alloca1)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.71, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %alloca1, i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len13 = call i64 @strlen(ptr @str.const.75)
  %len24 = call i64 @strlen(ptr %alloca2)
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %4 = call ptr @memcpy(ptr %concat.buf7, ptr @str.const.75, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %5 = call ptr @memcpy(ptr %dest28, ptr %alloca2, i64 %copylen29)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf7)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %strlen.ret = call i64 @strlen(ptr %alloca1)
  %alloca10 = alloca i64, align 8
  store i64 %strlen.ret, ptr %alloca10, align 8
  %strlen.ret11 = call i64 @strlen(ptr %alloca2)
  %alloca12 = alloca i64, align 8
  store i64 %strlen.ret11, ptr %alloca12, align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A8\D8\B9\D8\AF.load" = load i64, ptr %alloca12, align 8
  %"%\D8\B7\D9\88\D9\84_\D9\82\D8\A8\D9\84.load" = load i64, ptr %alloca10, align 8
  %assert.gt = icmp sgt i64 %"%\D8\B7\D9\88\D9\84_\D8\A8\D8\B9\D8\AF.load", %"%\D8\B7\D9\88\D9\84_\D9\82\D8\A8\D9\84.load"
  br i1 %assert.gt, label %agt.pass, label %agt.fail

agt.fail:                                         ; preds = %entry
  call void @abort()
  unreachable

agt.pass:                                         ; preds = %entry
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp13 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.79, i64 1)
  %alloca14 = alloca ptr, align 8
  store ptr @str.const.80, ptr %alloca14, align 8
  %sanitize.ret15 = call ptr @sad_security_sanitize(ptr %alloca14)
  %alloca16 = alloca ptr, align 8
  store ptr %sanitize.ret15, ptr %alloca16, align 8
  %len117 = call i64 @strlen(ptr @str.const.71)
  %len218 = call i64 @strlen(ptr %alloca14)
  %totallen19 = add i64 %len117, %len218
  %bufsize20 = add i64 %totallen19, 1
  %concat.buf21 = alloca i8, i64 %bufsize20, align 1
  %8 = call ptr @memcpy(ptr %concat.buf21, ptr @str.const.71, i64 %len117)
  %dest222 = getelementptr i8, ptr %concat.buf21, i64 %len117
  %copylen223 = add i64 %len218, 1
  %9 = call ptr @memcpy(ptr %dest222, ptr %alloca14, i64 %copylen223)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf21)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len124 = call i64 @strlen(ptr @str.const.75)
  %len225 = call i64 @strlen(ptr %alloca16)
  %totallen26 = add i64 %len124, %len225
  %bufsize27 = add i64 %totallen26, 1
  %concat.buf28 = alloca i8, i64 %bufsize27, align 1
  %12 = call ptr @memcpy(ptr %concat.buf28, ptr @str.const.75, i64 %len124)
  %dest229 = getelementptr i8, ptr %concat.buf28, i64 %len124
  %copylen230 = add i64 %len225, 1
  %13 = call ptr @memcpy(ptr %dest229, ptr %alloca16, i64 %copylen230)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf28)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load31" = load i64, ptr %alloca, align 8
  %addtmp32 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load31", 1
  store i64 %addtmp32, ptr %alloca, align 8
  %calltmp33 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.89, i64 1)
  %alloca34 = alloca ptr, align 8
  store ptr @str.const.90, ptr %alloca34, align 8
  %sanitize.ret35 = call ptr @sad_security_sanitize(ptr %alloca34)
  %alloca36 = alloca ptr, align 8
  store ptr %sanitize.ret35, ptr %alloca36, align 8
  call void @sad_security_assert_equal_str(ptr %alloca36, ptr %alloca34)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load37" = load i64, ptr %alloca, align 8
  %addtmp38 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load37", 1
  store i64 %addtmp38, ptr %alloca, align 8
  %calltmp39 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.91, i64 1)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.92)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\87\D8\A7\D8\B4"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\82\D8\B3\D9\85"(ptr @str.const.100)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca ptr, align 8
  store ptr @str.const.101, ptr %alloca1, align 8
  %alloca2 = alloca ptr, align 8
  store ptr @str.const.101, ptr %alloca2, align 8
  %alloca3 = alloca ptr, align 8
  store ptr @str.const.103, ptr %alloca3, align 8
  %hash.ret = call i64 @sad_security_hash(ptr %alloca1)
  %alloca4 = alloca i64, align 8
  store i64 %hash.ret, ptr %alloca4, align 8
  %hash.ret5 = call i64 @sad_security_hash(ptr %alloca2)
  %alloca6 = alloca i64, align 8
  store i64 %hash.ret5, ptr %alloca6, align 8
  %hash.ret7 = call i64 @sad_security_hash(ptr %alloca3)
  %alloca8 = alloca i64, align 8
  store i64 %hash.ret7, ptr %alloca8, align 8
  %alloca9 = alloca ptr, align 8
  %alloca10 = alloca ptr, align 8
  %alloca11 = alloca ptr, align 8
  %len1 = call i64 @strlen(ptr @str.const.104)
  %len2 = call i64 @strlen(ptr %alloca9)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.104, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %alloca9, i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len112 = call i64 @strlen(ptr @str.const.108)
  %len213 = call i64 @strlen(ptr %alloca10)
  %totallen14 = add i64 %len112, %len213
  %bufsize15 = add i64 %totallen14, 1
  %concat.buf16 = alloca i8, i64 %bufsize15, align 1
  %4 = call ptr @memcpy(ptr %concat.buf16, ptr @str.const.108, i64 %len112)
  %dest217 = getelementptr i8, ptr %concat.buf16, i64 %len112
  %copylen218 = add i64 %len213, 1
  %5 = call ptr @memcpy(ptr %dest217, ptr %alloca10, i64 %copylen218)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf16)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len119 = call i64 @strlen(ptr @str.const.112)
  %len220 = call i64 @strlen(ptr %alloca11)
  %totallen21 = add i64 %len119, %len220
  %bufsize22 = add i64 %totallen21, 1
  %concat.buf23 = alloca i8, i64 %bufsize22, align 1
  %8 = call ptr @memcpy(ptr %concat.buf23, ptr @str.const.112, i64 %len119)
  %dest224 = getelementptr i8, ptr %concat.buf23, i64 %len119
  %copylen225 = add i64 %len220, 1
  %9 = call ptr @memcpy(ptr %dest224, ptr %alloca11, i64 %copylen225)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf23)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\871.load" = load i64, ptr %alloca4, align 8
  %"%\D9\872.load" = load i64, ptr %alloca6, align 8
  %assert.eq = icmp eq i64 %"%\D9\871.load", %"%\D9\872.load"
  br i1 %assert.eq, label %aeq.pass, label %aeq.fail

then_8:                                           ; preds = %aeq.pass
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load28" = load i64, ptr %alloca, align 8
  %addtmp29 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load28", 1
  store i64 %addtmp29, ptr %alloca, align 8
  %calltmp30 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.117, i64 1)
  br label %merge_9

else_10:                                          ; preds = %aeq.pass
  %calltmp31 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.117, i64 0)
  br label %merge_9

merge_9:                                          ; preds = %else_10, %then_8
  %hash.ret32 = call i64 @sad_security_hash(ptr @str.const.119)
  %alloca33 = alloca i64, align 8
  store i64 %hash.ret32, ptr %alloca33, align 8
  %alloca34 = alloca ptr, align 8
  %len135 = call i64 @strlen(ptr @str.const.120)
  %len236 = call i64 @strlen(ptr %alloca34)
  %totallen37 = add i64 %len135, %len236
  %bufsize38 = add i64 %totallen37, 1
  %concat.buf39 = alloca i8, i64 %bufsize38, align 1
  %12 = call ptr @memcpy(ptr %concat.buf39, ptr @str.const.120, i64 %len135)
  %dest240 = getelementptr i8, ptr %concat.buf39, i64 %len135
  %copylen241 = add i64 %len236, 1
  %13 = call ptr @memcpy(ptr %dest240, ptr %alloca34, i64 %copylen241)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf39)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load42" = load i64, ptr %alloca, align 8
  %addtmp43 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load42", 1
  store i64 %addtmp43, ptr %alloca, align 8
  %calltmp44 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.124, i64 1)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.125)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0

aeq.fail:                                         ; preds = %entry
  call void @abort()
  unreachable

aeq.pass:                                         ; preds = %entry
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp26 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.116, i64 1)
  %"%\D9\871.load27" = load i64, ptr %alloca4, align 8
  %"%\D9\873.load" = load i64, ptr %alloca8, align 8
  %cmpnetmp = icmp ne i64 %"%\D9\871.load27", %"%\D9\873.load"
  br i1 %cmpnetmp, label %then_8, label %else_10
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AA\D8\B1\D9\85\D9\8A\D8\B2_\D9\88\D9\88\D9\82\D8\AA"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\82\D8\B3\D9\85"(ptr @str.const.133)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca ptr, align 8
  store ptr @str.const.134, ptr %alloca1, align 8
  %base64.ret = call ptr @sad_security_base64_encode(ptr %alloca1)
  %alloca2 = alloca ptr, align 8
  store ptr %base64.ret, ptr %alloca2, align 8
  %len1 = call i64 @strlen(ptr @str.const.135)
  %len2 = call i64 @strlen(ptr %alloca1)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.135, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %alloca1, i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len13 = call i64 @strlen(ptr @str.const.139)
  %len24 = call i64 @strlen(ptr %alloca2)
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %4 = call ptr @memcpy(ptr %concat.buf7, ptr @str.const.139, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %5 = call ptr @memcpy(ptr %dest28, ptr %alloca2, i64 %copylen29)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf7)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp10 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.143, i64 1)
  %alloca11 = alloca ptr, align 8
  store ptr @str.const.144, ptr %alloca11, align 8
  %base64.ret12 = call ptr @sad_security_base64_encode(ptr %alloca11)
  %alloca13 = alloca ptr, align 8
  store ptr %base64.ret12, ptr %alloca13, align 8
  %len114 = call i64 @strlen(ptr @str.const.135)
  %len215 = call i64 @strlen(ptr %alloca11)
  %totallen16 = add i64 %len114, %len215
  %bufsize17 = add i64 %totallen16, 1
  %concat.buf18 = alloca i8, i64 %bufsize17, align 1
  %8 = call ptr @memcpy(ptr %concat.buf18, ptr @str.const.135, i64 %len114)
  %dest219 = getelementptr i8, ptr %concat.buf18, i64 %len114
  %copylen220 = add i64 %len215, 1
  %9 = call ptr @memcpy(ptr %dest219, ptr %alloca11, i64 %copylen220)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf18)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len121 = call i64 @strlen(ptr @str.const.139)
  %len222 = call i64 @strlen(ptr %alloca13)
  %totallen23 = add i64 %len121, %len222
  %bufsize24 = add i64 %totallen23, 1
  %concat.buf25 = alloca i8, i64 %bufsize24, align 1
  %12 = call ptr @memcpy(ptr %concat.buf25, ptr @str.const.139, i64 %len121)
  %dest226 = getelementptr i8, ptr %concat.buf25, i64 %len121
  %copylen227 = add i64 %len222, 1
  %13 = call ptr @memcpy(ptr %dest226, ptr %alloca13, i64 %copylen227)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf25)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load28" = load i64, ptr %alloca, align 8
  %addtmp29 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load28", 1
  store i64 %addtmp29, ptr %alloca, align 8
  %calltmp30 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.153, i64 1)
  %alloca31 = alloca ptr, align 8
  store ptr @str.const.154, ptr %alloca31, align 8
  %base64.ret32 = call ptr @sad_security_base64_encode(ptr %alloca31)
  %alloca33 = alloca ptr, align 8
  store ptr %base64.ret32, ptr %alloca33, align 8
  %len134 = call i64 @strlen(ptr @str.const.155)
  %len235 = call i64 @strlen(ptr %alloca31)
  %totallen36 = add i64 %len134, %len235
  %bufsize37 = add i64 %totallen36, 1
  %concat.buf38 = alloca i8, i64 %bufsize37, align 1
  %16 = call ptr @memcpy(ptr %concat.buf38, ptr @str.const.155, i64 %len134)
  %dest239 = getelementptr i8, ptr %concat.buf38, i64 %len134
  %copylen240 = add i64 %len235, 1
  %17 = call ptr @memcpy(ptr %dest239, ptr %alloca31, i64 %copylen240)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf38)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len141 = call i64 @strlen(ptr @str.const.139)
  %len242 = call i64 @strlen(ptr %alloca33)
  %totallen43 = add i64 %len141, %len242
  %bufsize44 = add i64 %totallen43, 1
  %concat.buf45 = alloca i8, i64 %bufsize44, align 1
  %20 = call ptr @memcpy(ptr %concat.buf45, ptr @str.const.139, i64 %len141)
  %dest246 = getelementptr i8, ptr %concat.buf45, i64 %len141
  %copylen247 = add i64 %len242, 1
  %21 = call ptr @memcpy(ptr %dest246, ptr %alloca33, i64 %copylen247)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf45)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load48" = load i64, ptr %alloca, align 8
  %addtmp49 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load48", 1
  store i64 %addtmp49, ptr %alloca, align 8
  %calltmp50 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.163, i64 1)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.164)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %time.ret = call i64 @time(ptr null)
  %alloca51 = alloca i64, align 8
  store i64 %time.ret, ptr %alloca51, align 8
  %alloca52 = alloca ptr, align 8
  %len153 = call i64 @strlen(ptr @str.const.168)
  %len254 = call i64 @strlen(ptr %alloca52)
  %totallen55 = add i64 %len153, %len254
  %bufsize56 = add i64 %totallen55, 1
  %concat.buf57 = alloca i8, i64 %bufsize56, align 1
  %26 = call ptr @memcpy(ptr %concat.buf57, ptr @str.const.168, i64 %len153)
  %dest258 = getelementptr i8, ptr %concat.buf57, i64 %len153
  %copylen259 = add i64 %len254, 1
  %27 = call ptr @memcpy(ptr %dest258, ptr %alloca52, i64 %copylen259)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf57)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\A7\D9\84\D8\A2\D9\86.load" = load i64, ptr %alloca51, align 8
  %assert.gt = icmp sgt i64 %"%\D8\A7\D9\84\D8\A2\D9\86.load", 0
  br i1 %assert.gt, label %agt.pass, label %agt.fail

agt.fail:                                         ; preds = %entry
  call void @abort()
  unreachable

agt.pass:                                         ; preds = %entry
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load60" = load i64, ptr %alloca, align 8
  %addtmp61 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load60", 1
  store i64 %addtmp61, ptr %alloca, align 8
  %calltmp62 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.172, i64 1)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.173)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AA\D8\A3\D9\83\D9\8A\D8\AF\D8\A7\D8\AA"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\82\D8\B3\D9\85"(ptr @str.const.181)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  br i1 true, label %sec.pass, label %sec.fail

sec.fail:                                         ; preds = %entry
  call void @abort()
  unreachable

sec.pass:                                         ; preds = %entry
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp1 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.182, i64 1)
  %alloca2 = alloca i64, align 8
  store i64 56, ptr %alloca2, align 8
  %"%\D8\AD\D8\B3\D8\A7\D8\A8.load" = load i64, ptr %alloca2, align 8
  %assert.eq = icmp eq i64 %"%\D8\AD\D8\B3\D8\A7\D8\A8.load", 56
  br i1 %assert.eq, label %aeq.pass, label %aeq.fail

aeq.fail:                                         ; preds = %sec.pass
  call void @abort()
  unreachable

aeq.pass:                                         ; preds = %sec.pass
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load3" = load i64, ptr %alloca, align 8
  %addtmp4 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load3", 1
  store i64 %addtmp4, ptr %alloca, align 8
  %calltmp5 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.183, i64 1)
  br i1 true, label %agt.pass, label %agt.fail

agt.fail:                                         ; preds = %aeq.pass
  call void @abort()
  unreachable

agt.pass:                                         ; preds = %aeq.pass
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load6" = load i64, ptr %alloca, align 8
  %addtmp7 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load6", 1
  store i64 %addtmp7, ptr %alloca, align 8
  %calltmp8 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.184, i64 1)
  %alloca9 = alloca ptr, align 8
  store ptr @str.const.185, ptr %alloca9, align 8
  %alloca10 = alloca ptr, align 8
  store ptr @str.const.185, ptr %alloca10, align 8
  call void @sad_security_assert_equal_str(ptr %alloca9, ptr %alloca10)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load11" = load i64, ptr %alloca, align 8
  %addtmp12 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load11", 1
  store i64 %addtmp12, ptr %alloca, align 8
  %calltmp13 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.187, i64 1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load14" = load i64, ptr %alloca, align 8
  %addtmp15 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load14", 1
  store i64 %addtmp15, ptr %alloca, align 8
  %calltmp16 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.188, i64 1)
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load17" = load i64, ptr %alloca, align 8
  %addtmp18 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load17", 1
  store i64 %addtmp18, ptr %alloca, align 8
  %calltmp19 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.189, i64 1)
  br i1 true, label %agt.pass21, label %agt.fail20

agt.fail20:                                       ; preds = %agt.pass
  call void @abort()
  unreachable

agt.pass21:                                       ; preds = %agt.pass
  br i1 true, label %agt.pass23, label %agt.fail22

agt.fail22:                                       ; preds = %agt.pass21
  call void @abort()
  unreachable

agt.pass23:                                       ; preds = %agt.pass21
  br i1 true, label %agt.pass25, label %agt.fail24

agt.fail24:                                       ; preds = %agt.pass23
  call void @abort()
  unreachable

agt.pass25:                                       ; preds = %agt.pass23
  br i1 true, label %agt.pass27, label %agt.fail26

agt.fail26:                                       ; preds = %agt.pass25
  call void @abort()
  unreachable

agt.pass27:                                       ; preds = %agt.pass25
  br i1 true, label %agt.pass29, label %agt.fail28

agt.fail28:                                       ; preds = %agt.pass27
  call void @abort()
  unreachable

agt.pass29:                                       ; preds = %agt.pass27
  %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load30" = load i64, ptr %alloca, align 8
  %addtmp31 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD\D8\A7\D8\AA.load30", 1
  store i64 %addtmp31, ptr %alloca, align 8
  %calltmp32 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\AD\D8\B5"(ptr @str.const.190, i64 1)
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.191)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\85\D9\84\D8\AE\D8\B5_\D8\B4\D8\A7\D9\85\D9\84"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.199)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.203)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.207)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.211)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.215)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.219)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.223)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.227)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.231)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.235)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.211)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.243)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.247)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.251)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.203)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.259)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.251)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\83\D9\84\D9\85\D8\A7\D8\AA_\D9\85\D8\B1\D9\88\D8\B1"()
  %calltmp1 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AA\D8\B4\D9\81\D9\8A\D8\B1"()
  %calltmp2 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AA\D9\86\D8\B8\D9\8A\D9\81"()
  %calltmp3 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\87\D8\A7\D8\B4"()
  %calltmp4 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AA\D8\B1\D9\85\D9\8A\D8\B2_\D9\88\D9\88\D9\82\D8\AA"()
  %calltmp5 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AA\D8\A3\D9\83\D9\8A\D8\AF\D8\A7\D8\AA"()
  %calltmp6 = call i64 @"\D9\85\D9\84\D8\AE\D8\B5_\D8\B4\D8\A7\D9\85\D9\84"()
  ret i64 0
}

declare i32 @printf(ptr, ...)

declare i64 @strlen(ptr)

declare ptr @memcpy(ptr, ptr, i64)

declare ptr @sad_security_encrypt(ptr, ptr)

declare ptr @sad_security_decrypt(ptr, ptr)

declare void @sad_security_assert_equal_str(ptr, ptr)

declare ptr @sad_security_sanitize(ptr)

declare void @abort()

declare i64 @sad_security_hash(ptr)

declare ptr @sad_security_base64_encode(ptr)

declare i64 @time(ptr)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
