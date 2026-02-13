; ModuleID = '06_complex_package_registry'
source_filename = "06_complex_package_registry"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [175 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [175 x i8] c"\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\00", align 1
@str.const.7 = private unnamed_addr constant [100 x i8] c"\E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85 \E2\98\85\00", align 1
@str.const.11 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.15 = private unnamed_addr constant [8 x i8] c"  \F0\9F\8F\AA \00", align 1
@str.const.20 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.24 = private unnamed_addr constant [7 x i8] c"  \E2\96\B8 \00", align 1
@str.const.29 = private unnamed_addr constant [5 x i8] c"    \00", align 1
@str.const.32 = private unnamed_addr constant [3 x i8] c": \00", align 1
@fmt.d = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1
@str.const.44 = private unnamed_addr constant [16 x i8] c"\E2\98\86\E2\98\86\E2\98\86\E2\98\86\E2\98\86\00", align 1
@str.const.45 = private unnamed_addr constant [16 x i8] c"\E2\98\85\E2\98\86\E2\98\86\E2\98\86\E2\98\86\00", align 1
@str.const.46 = private unnamed_addr constant [16 x i8] c"\E2\98\85\E2\98\85\E2\98\86\E2\98\86\E2\98\86\00", align 1
@str.const.47 = private unnamed_addr constant [16 x i8] c"\E2\98\85\E2\98\85\E2\98\85\E2\98\86\E2\98\86\00", align 1
@str.const.48 = private unnamed_addr constant [16 x i8] c"\E2\98\85\E2\98\85\E2\98\85\E2\98\85\E2\98\86\00", align 1
@str.const.49 = private unnamed_addr constant [16 x i8] c"\E2\98\85\E2\98\85\E2\98\85\E2\98\85\E2\98\85\00", align 1
@str.const.50 = private unnamed_addr constant [20 x i8] c"\D8\BA\D9\8A\D8\B1 \D9\85\D8\B9\D8\B1\D9\88\D9\81\D8\A9\00", align 1
@str.const.51 = private unnamed_addr constant [25 x i8] c"\F0\9F\94\A5 \D8\B4\D8\A7\D8\A6\D8\B9\D8\A9 \D8\AC\D8\AF\D8\A7\D9\8B\00", align 1
@str.const.52 = private unnamed_addr constant [15 x i8] c"\E2\AD\90 \D8\B4\D8\A7\D8\A6\D8\B9\D8\A9\00", align 1
@str.const.53 = private unnamed_addr constant [20 x i8] c"\F0\9F\93\88 \D9\85\D8\AA\D9\86\D8\A7\D9\85\D9\8A\D8\A9\00", align 1
@str.const.54 = private unnamed_addr constant [16 x i8] c"\F0\9F\8C\B1 \D8\AC\D8\AF\D9\8A\D8\AF\D8\A9\00", align 1
@str.const.55 = private unnamed_addr constant [20 x i8] c"\F0\9F\94\AC \D8\AA\D8\AC\D8\B1\D9\8A\D8\A8\D9\8A\D8\A9\00", align 1
@str.const.56 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.60 = private unnamed_addr constant [134 x i8] c"    \E2\94\8C\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\90\00", align 1
@str.const.62 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.66 = private unnamed_addr constant [14 x i8] c"    \E2\94\82 \F0\9F\93\A6 \00", align 1
@str.const.69 = private unnamed_addr constant [2 x i8] c"@\00", align 1
@str.const.74 = private unnamed_addr constant [9 x i8] c"    \E2\94\82 \00", align 1
@str.const.79 = private unnamed_addr constant [23 x i8] c"    \E2\94\82 \D8\A7\D9\84\D9\85\D8\A4\D9\84\D9\81: \00", align 1
@str.const.84 = private unnamed_addr constant [25 x i8] c"    \E2\94\82 \D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85: \00", align 1
@str.const.87 = private unnamed_addr constant [3 x i8] c" (\00", align 1
@str.const.90 = private unnamed_addr constant [4 x i8] c"/5)\00", align 1
@str.const.94 = private unnamed_addr constant [29 x i8] c"    \E2\94\82 \D8\A7\D9\84\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA: \00", align 1
@str.const.97 = private unnamed_addr constant [2 x i8] c" \00", align 1
@str.const.102 = private unnamed_addr constant [23 x i8] c"    \E2\94\82 \D8\A7\D9\84\D8\B1\D8\AE\D8\B5\D8\A9: \00", align 1
@str.const.107 = private unnamed_addr constant [134 x i8] c"    \E2\94\94\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\98\00", align 1
@str.const.109 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.113 = private unnamed_addr constant [32 x i8] c"\D8\B3\D8\AC\D9\84 \D8\AD\D8\B2\D9\85 \D8\B5 \D8\A7\D9\84\D9\85\D8\B1\D9\83\D8\B2\D9\8A\00", align 1
@str.const.114 = private unnamed_addr constant [15 x i8] c"\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.115 = private unnamed_addr constant [6 x i8] c"2.3.0\00", align 1
@str.const.116 = private unnamed_addr constant [48 x i8] c"\D9\85\D9\83\D8\AA\D8\A8\D8\A9 \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA \D8\B9\D8\B1\D8\A8\D9\8A\D8\A9 \D8\B4\D8\A7\D9\85\D9\84\D8\A9\00", align 1
@str.const.117 = private unnamed_addr constant [12 x i8] c"\D9\81\D8\B1\D9\8A\D9\82 \D8\B5\00", align 1
@str.const.118 = private unnamed_addr constant [4 x i8] c"MIT\00", align 1
@str.const.119 = private unnamed_addr constant [9 x i8] c"\D8\B4\D8\A8\D9\83\D8\A9\00", align 1
@str.const.120 = private unnamed_addr constant [6 x i8] c"1.0.0\00", align 1
@str.const.121 = private unnamed_addr constant [45 x i8] c"\D9\85\D9\83\D8\AA\D8\A8\D8\A9 \D8\B4\D8\A8\D9\83\D8\A7\D8\AA \D9\88\D8\A8\D8\B1\D9\88\D8\AA\D9\88\D9\83\D9\88\D9\84\D8\A7\D8\AA\00", align 1
@str.const.122 = private unnamed_addr constant [18 x i8] c"\D8\A3\D8\AD\D9\85\D8\AF \D9\85\D8\AD\D9\85\D8\AF\00", align 1
@str.const.123 = private unnamed_addr constant [11 x i8] c"Apache-2.0\00", align 1
@str.const.124 = private unnamed_addr constant [11 x i8] c"\D9\88\D8\A7\D8\AC\D9\87\D8\A9\00", align 1
@str.const.125 = private unnamed_addr constant [6 x i8] c"3.5.1\00", align 1
@str.const.126 = private unnamed_addr constant [44 x i8] c"\D8\A5\D8\B7\D8\A7\D8\B1 \D9\88\D8\A7\D8\AC\D9\87\D8\A7\D8\AA \D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85 \D8\B9\D8\B1\D8\A8\D9\8A\00", align 1
@str.const.127 = private unnamed_addr constant [16 x i8] c"\D8\B3\D8\A7\D8\B1\D8\A9 \D8\B9\D9\84\D9\8A\00", align 1
@str.const.129 = private unnamed_addr constant [24 x i8] c"\D9\82\D8\A7\D8\B9\D8\AF\D8\A9_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA\00", align 1
@str.const.130 = private unnamed_addr constant [6 x i8] c"2.0.0\00", align 1
@str.const.131 = private unnamed_addr constant [46 x i8] c"\D9\88\D8\A7\D8\AC\D9\87\D8\A9 \D9\82\D9\88\D8\A7\D8\B9\D8\AF \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA \D9\85\D9\88\D8\AD\D8\AF\D8\A9\00", align 1
@str.const.132 = private unnamed_addr constant [16 x i8] c"\D8\AE\D8\A7\D9\84\D8\AF \D8\AD\D8\B3\D9\86\00", align 1
@str.const.133 = private unnamed_addr constant [8 x i8] c"GPL-3.0\00", align 1
@str.const.134 = private unnamed_addr constant [11 x i8] c"\D8\AA\D8\B4\D9\81\D9\8A\D8\B1\00", align 1
@str.const.135 = private unnamed_addr constant [6 x i8] c"1.2.5\00", align 1
@str.const.136 = private unnamed_addr constant [39 x i8] c"\D8\AE\D9\88\D8\A7\D8\B1\D8\B2\D9\85\D9\8A\D8\A7\D8\AA \D8\AA\D8\B4\D9\81\D9\8A\D8\B1 \D8\A2\D9\85\D9\86\D8\A9\00", align 1
@str.const.137 = private unnamed_addr constant [18 x i8] c"\D9\86\D9\88\D8\B1\D8\A9 \D8\A3\D8\AD\D9\85\D8\AF\00", align 1
@str.const.138 = private unnamed_addr constant [6 x i8] c"BSD-3\00", align 1
@str.const.139 = private unnamed_addr constant [40 x i8] c"\D8\A7\D9\84\D8\A8\D8\AD\D8\AB: \22\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA \D8\B9\D8\B1\D8\A8\D9\8A\D8\A9\22\00", align 1
@str.const.140 = private unnamed_addr constant [47 x i8] c"    \E2\8F\B3 \D8\AC\D8\A7\D8\B1\D9\8A \D8\A7\D9\84\D8\A8\D8\AD\D8\AB \D9\81\D9\8A \D8\A7\D9\84\D8\B3\D8\AC\D9\84...\00", align 1
@str.const.144 = private unnamed_addr constant [48 x i8] c"    \F0\9F\93\8A \D8\AA\D9\85 \D8\A7\D9\84\D8\B9\D8\AB\D9\88\D8\B1 \D8\B9\D9\84\D9\89 5 \D9\86\D8\AA\D8\A7\D8\A6\D8\AC:\00", align 1
@str.const.148 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.152 = private unnamed_addr constant [38 x i8] c"    1. \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA (\D9\85\D9\84\D8\A7\D8\A1\D9\85\D8\A9: \00", align 1
@str.const.155 = private unnamed_addr constant [2 x i8] c")\00", align 1
@str.const.159 = private unnamed_addr constant [3 x i8] c"  \00", align 1
@str.const.160 = private unnamed_addr constant [83 x i8] c"\D9\85\D9\83\D8\AA\D8\A8\D8\A9 \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA \D8\B9\D8\B1\D8\A8\D9\8A\D8\A9 \D8\B4\D8\A7\D9\85\D9\84\D8\A9 - \D8\AC\D8\A8\D8\B1 \D9\88\D9\87\D9\86\D8\AF\D8\B3\D8\A9 \D9\88\D8\A5\D8\AD\D8\B5\D8\A7\D8\A1\00", align 1
@str.const.161 = private unnamed_addr constant [43 x i8] c"    2. \D8\AD\D8\B3\D8\A7\D8\A8_\D9\85\D8\AA\D9\82\D8\AF\D9\85 (\D9\85\D9\84\D8\A7\D8\A1\D9\85\D8\A9: \00", align 1
@str.const.169 = private unnamed_addr constant [58 x i8] c"\D8\AD\D8\B3\D8\A7\D8\A8\D8\A7\D8\AA \D9\85\D8\AA\D9\82\D8\AF\D9\85\D8\A9 \D9\88\D9\85\D8\B9\D8\A7\D8\AF\D9\84\D8\A7\D8\AA \D8\AA\D9\81\D8\A7\D8\B6\D9\84\D9\8A\D8\A9\00", align 1
@str.const.170 = private unnamed_addr constant [34 x i8] c"    3. \D9\87\D9\86\D8\AF\D8\B3\D8\A9 (\D9\85\D9\84\D8\A7\D8\A1\D9\85\D8\A9: \00", align 1
@str.const.178 = private unnamed_addr constant [46 x i8] c"\D9\87\D9\86\D8\AF\D8\B3\D8\A9 \D8\AA\D8\AD\D9\84\D9\8A\D9\84\D9\8A\D8\A9 \D9\88\D8\B1\D8\B3\D9\85 \D8\A8\D9\8A\D8\A7\D9\86\D9\8A\00", align 1
@str.const.179 = private unnamed_addr constant [43 x i8] c"    4. \D8\A5\D8\AD\D8\B5\D8\A7\D8\A1_\D8\B9\D8\B1\D8\A8\D9\8A (\D9\85\D9\84\D8\A7\D8\A1\D9\85\D8\A9: \00", align 1
@str.const.187 = private unnamed_addr constant [52 x i8] c"\D8\AA\D8\AD\D9\84\D9\8A\D9\84 \D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A \D8\A8\D8\A7\D9\84\D9\84\D8\BA\D8\A9 \D8\A7\D9\84\D8\B9\D8\B1\D8\A8\D9\8A\D8\A9\00", align 1
@str.const.188 = private unnamed_addr constant [34 x i8] c"    5. \D8\A3\D8\B1\D9\82\D8\A7\D9\85 (\D9\85\D9\84\D8\A7\D8\A1\D9\85\D8\A9: \00", align 1
@str.const.196 = private unnamed_addr constant [58 x i8] c"\D8\AA\D8\AD\D9\88\D9\8A\D9\84 \D8\A7\D9\84\D8\A3\D8\B1\D9\82\D8\A7\D9\85 \D8\A7\D9\84\D8\B9\D8\B1\D8\A8\D9\8A\D8\A9 \D9\88\D8\A7\D9\84\D9\87\D9\86\D8\AF\D9\8A\D8\A9\00", align 1
@str.const.197 = private unnamed_addr constant [15 x i8] c"\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA\00", align 1
@str.const.198 = private unnamed_addr constant [19 x i8] c"\E2\84\B9 \D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA\00", align 1
@str.const.199 = private unnamed_addr constant [15 x i8] c"\E2\9A\A0 \D9\85\D9\86\D8\AE\D9\81\D8\B6\00", align 1
@str.const.200 = private unnamed_addr constant [15 x i8] c"\E2\9A\A0 \D9\85\D8\AA\D9\88\D8\B3\D8\B7\00", align 1
@str.const.201 = private unnamed_addr constant [14 x i8] c"\F0\9F\94\B4 \D8\B9\D8\A7\D9\84\D9\8A\00", align 1
@str.const.202 = private unnamed_addr constant [12 x i8] c"\F0\9F\9A\A8 \D8\AD\D8\B1\D8\AC\00", align 1
@str.const.203 = private unnamed_addr constant [33 x i8] c"\D9\81\D8\AD\D8\B5 \D8\A3\D9\85\D8\A7\D9\86 \D8\A7\D9\84\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.204 = private unnamed_addr constant [59 x i8] c"    \E2\8F\B3 \D8\AC\D8\A7\D8\B1\D9\8A \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\AB\D8\BA\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\B9\D8\B1\D9\88\D9\81\D8\A9...\00", align 1
@str.const.208 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.212 = private unnamed_addr constant [32 x i8] c"    \F0\9F\93\8B \D9\86\D8\AA\D8\A7\D8\A6\D8\AC \D8\A7\D9\84\D9\81\D8\AD\D8\B5:\00", align 1
@str.const.216 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.220 = private unnamed_addr constant [28 x i8] c"    [1] \D8\B4\D8\A8\D9\83\D8\A9@0.9.2 \E2\86\92 \00", align 1
@str.const.225 = private unnamed_addr constant [14 x i8] c"   \D8\A7\D9\84\D9\88\D8\B5\D9\81\00", align 1
@str.const.226 = private unnamed_addr constant [40 x i8] c"\D8\AB\D8\BA\D8\B1\D8\A9 \D9\81\D9\8A \D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9 HTTP headers\00", align 1
@str.const.227 = private unnamed_addr constant [12 x i8] c"   \D8\A7\D9\84\D8\AD\D9\84\00", align 1
@str.const.228 = private unnamed_addr constant [25 x i8] c"\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB \D8\A5\D9\84\D9\89 1.0.0+\00", align 1
@str.const.229 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.233 = private unnamed_addr constant [30 x i8] c"    [2] \D8\AA\D8\B4\D9\81\D9\8A\D8\B1@1.2.5 \E2\86\92 \00", align 1
@str.const.239 = private unnamed_addr constant [37 x i8] c"\D8\AE\D9\88\D8\A7\D8\B1\D8\B2\D9\85\D9\8A\D8\A9 MD5 \D8\BA\D9\8A\D8\B1 \D8\A2\D9\85\D9\86\D8\A9\00", align 1
@str.const.241 = private unnamed_addr constant [34 x i8] c"\D8\A7\D8\B3\D8\AA\D8\AE\D8\AF\D8\A7\D9\85 SHA-256 \D8\A8\D8\AF\D9\84\D8\A7\D9\8B\00", align 1
@str.const.242 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.246 = private unnamed_addr constant [43 x i8] c"    [3] \D9\82\D8\A7\D8\B9\D8\AF\D8\A9_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA@2.0.0 \E2\86\92 \00", align 1
@str.const.252 = private unnamed_addr constant [56 x i8] c"SQL injection \D9\85\D8\AD\D8\AA\D9\85\D9\84 \D9\81\D9\8A \D8\A7\D8\B3\D8\AA\D8\B9\D9\84\D8\A7\D9\85\D8\A7\D8\AA \D8\AE\D8\A7\D9\85\00", align 1
@str.const.254 = private unnamed_addr constant [45 x i8] c"\D8\A7\D8\B3\D8\AA\D8\AE\D8\AF\D8\A7\D9\85 \D8\A7\D8\B3\D8\AA\D8\B9\D9\84\D8\A7\D9\85\D8\A7\D8\AA \D9\85\D8\AD\D8\B6\D8\B1\D8\A9\00", align 1
@str.const.255 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.259 = private unnamed_addr constant [33 x i8] c"\D9\85\D9\84\D8\AE\D8\B5 \D8\A7\D9\84\D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\A3\D9\85\D9\86\D9\8A\00", align 1
@str.const.260 = private unnamed_addr constant [18 x i8] c"\D8\AD\D8\B2\D9\85 \D9\81\D9\8F\D8\AD\D8\B5\D8\AA\00", align 1
@str.const.261 = private unnamed_addr constant [20 x i8] c"\D8\AB\D8\BA\D8\B1\D8\A7\D8\AA \D8\AD\D8\B1\D8\AC\D8\A9\00", align 1
@str.const.262 = private unnamed_addr constant [22 x i8] c"\D8\AB\D8\BA\D8\B1\D8\A7\D8\AA \D8\B9\D8\A7\D9\84\D9\8A\D8\A9\00", align 1
@str.const.263 = private unnamed_addr constant [24 x i8] c"\D8\AB\D8\BA\D8\B1\D8\A7\D8\AA \D9\85\D8\AA\D9\88\D8\B3\D8\B7\D8\A9\00", align 1
@str.const.264 = private unnamed_addr constant [24 x i8] c"\D8\AB\D8\BA\D8\B1\D8\A7\D8\AA \D9\85\D9\86\D8\AE\D9\81\D8\B6\D8\A9\00", align 1
@str.const.265 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.269 = private unnamed_addr constant [57 x i8] c"    \F0\9F\92\A1 \D8\B4\D8\BA\D9\91\D9\84: \D8\B5 \D8\AD\D8\A7\D8\B1\D8\B3 --\D8\A5\D8\B5\D9\84\D8\A7\D8\AD-\D8\AA\D9\84\D9\82\D8\A7\D8\A6\D9\8A\00", align 1
@str.const.273 = private unnamed_addr constant [42 x i8] c"\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA \D8\B3\D8\AC\D9\84 \D8\B5 \D8\A7\D9\84\D9\85\D8\B1\D9\83\D8\B2\D9\8A\00", align 1
@str.const.274 = private unnamed_addr constant [41 x i8] c"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A \D8\A7\D9\84\D8\AD\D8\B2\D9\85 \D8\A7\D9\84\D9\85\D9\86\D8\B4\D9\88\D8\B1\D8\A9\00", align 1
@str.const.275 = private unnamed_addr constant [50 x i8] c"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A \D8\A7\D9\84\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA \D9\87\D8\B0\D8\A7 \D8\A7\D9\84\D8\B4\D9\87\D8\B1\00", align 1
@str.const.276 = private unnamed_addr constant [24 x i8] c"\D9\85\D8\B7\D9\88\D8\B1\D9\88\D9\86 \D9\86\D8\B4\D8\B7\D9\88\D9\86\00", align 1
@str.const.277 = private unnamed_addr constant [40 x i8] c"\D8\AD\D8\B2\D9\85 \D8\AC\D8\AF\D9\8A\D8\AF\D8\A9 \D9\87\D8\B0\D8\A7 \D8\A7\D9\84\D8\A3\D8\B3\D8\A8\D9\88\D8\B9\00", align 1
@str.const.278 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.282 = private unnamed_addr constant [35 x i8] c"\D8\A3\D9\83\D8\AB\D8\B1 \D8\A7\D9\84\D8\AD\D8\B2\D9\85 \D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D9\8B\00", align 1
@str.const.283 = private unnamed_addr constant [48 x i8] c"    \F0\9F\A5\87 \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA     - 15,420 \D8\AA\D9\86\D8\B2\D9\8A\D9\84\00", align 1
@str.const.287 = private unnamed_addr constant [46 x i8] c"    \F0\9F\A5\88 \D9\88\D8\A7\D8\AC\D9\87\D8\A9       - 12,300 \D8\AA\D9\86\D8\B2\D9\8A\D9\84\00", align 1
@str.const.291 = private unnamed_addr constant [44 x i8] c"    \F0\9F\A5\89 \D8\B4\D8\A8\D9\83\D8\A9        - 8,750 \D8\AA\D9\86\D8\B2\D9\8A\D9\84\00", align 1
@str.const.295 = private unnamed_addr constant [43 x i8] c"    4. \D8\AA\D8\B4\D9\81\D9\8A\D8\B1       - 6,800 \D8\AA\D9\86\D8\B2\D9\8A\D9\84\00", align 1
@str.const.299 = private unnamed_addr constant [50 x i8] c"    5. \D9\82\D8\A7\D8\B9\D8\AF\D8\A9_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA - 4,200 \D8\AA\D9\86\D8\B2\D9\8A\D9\84\00", align 1
@str.const.303 = private unnamed_addr constant [34 x i8] c"\D8\A7\D9\84\D8\AA\D8\B5\D9\86\D9\8A\D9\81\D8\A7\D8\AA \D8\A7\D9\84\D8\B4\D8\A7\D8\A6\D8\B9\D8\A9\00", align 1
@str.const.304 = private unnamed_addr constant [30 x i8] c"\D8\A7\D9\84\D8\B1\D8\AE\D8\B5 \D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9\00", align 1
@str.const.305 = private unnamed_addr constant [82 x i8] c"    MIT          \E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\91\E2\96\91\E2\96\91\E2\96\91 62%\00", align 1
@str.const.309 = private unnamed_addr constant [82 x i8] c"    Apache-2.0   \E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91 21%\00", align 1
@str.const.313 = private unnamed_addr constant [82 x i8] c"    GPL-3.0      \E2\96\88\E2\96\88\E2\96\88\E2\96\88\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91 11%\00", align 1
@str.const.317 = private unnamed_addr constant [82 x i8] c"    BSD-3        \E2\96\88\E2\96\88\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91\E2\96\91  6%\00", align 1
@str.const.321 = private unnamed_addr constant [50 x i8] c"    \F0\9F\93\8A \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA \D9\88\D8\AD\D8\B3\D8\A7\D8\A8 (187 \D8\AD\D8\B2\D9\85\D8\A9)\00", align 1
@str.const.325 = private unnamed_addr constant [44 x i8] c"    \F0\9F\8C\90 \D8\B4\D8\A8\D9\83\D8\A7\D8\AA \D9\88\D9\88\D9\8A\D8\A8 (145 \D8\AD\D8\B2\D9\85\D8\A9)\00", align 1
@str.const.329 = private unnamed_addr constant [50 x i8] c"    \F0\9F\8E\A8 \D9\88\D8\A7\D8\AC\D9\87\D8\A7\D8\AA \D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85 (132 \D8\AD\D8\B2\D9\85\D8\A9)\00", align 1
@str.const.333 = private unnamed_addr constant [47 x i8] c"    \F0\9F\97\83 \D9\82\D9\88\D8\A7\D8\B9\D8\AF \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA (98 \D8\AD\D8\B2\D9\85\D8\A9)\00", align 1
@str.const.337 = private unnamed_addr constant [45 x i8] c"    \F0\9F\94\90 \D8\A3\D9\85\D8\A7\D9\86 \D9\88\D8\AA\D8\B4\D9\81\D9\8A\D8\B1 (87 \D8\AD\D8\B2\D9\85\D8\A9)\00", align 1
@str.const.341 = private unnamed_addr constant [47 x i8] c"    \F0\9F\93\9D \D9\86\D8\B5\D9\88\D8\B5 \D9\88\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9 (76 \D8\AD\D8\B2\D9\85\D8\A9)\00", align 1
@str.const.345 = private unnamed_addr constant [40 x i8] c"    \F0\9F\A7\AA \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA (65 \D8\AD\D8\B2\D9\85\D8\A9)\00", align 1
@str.const.349 = private unnamed_addr constant [43 x i8] c"    \F0\9F\93\A6 \D8\A3\D8\AF\D9\88\D8\A7\D8\AA \D8\A8\D9\86\D8\A7\D8\A1 (54 \D8\AD\D8\B2\D9\85\D8\A9)\00", align 1
@str.const.353 = private unnamed_addr constant [13 x i8] c"\E2\9D\8C \D8\B6\D8\B9\D9\8A\D9\81\00", align 1
@str.const.354 = private unnamed_addr constant [16 x i8] c"\F0\9F\9F\A2 \D9\85\D9\85\D8\AA\D8\A7\D8\B2\00", align 1
@str.const.355 = private unnamed_addr constant [12 x i8] c"\F0\9F\9F\A1 \D8\AC\D9\8A\D8\AF\00", align 1
@str.const.356 = private unnamed_addr constant [16 x i8] c"\F0\9F\9F\A0 \D9\85\D8\AA\D9\88\D8\B3\D8\B7\00", align 1
@str.const.357 = private unnamed_addr constant [14 x i8] c"\F0\9F\94\B4 \D8\B6\D8\B9\D9\8A\D9\81\00", align 1
@str.const.358 = private unnamed_addr constant [33 x i8] c"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\B5\D8\AD\D8\A9 \D8\A7\D9\84\D9\85\D8\B4\D8\B1\D9\88\D8\B9\00", align 1
@str.const.359 = private unnamed_addr constant [40 x i8] c"    \D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9 \D8\A7\D9\84\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A\D8\A9: \00", align 1
@str.const.362 = private unnamed_addr constant [6 x i8] c"/100 \00", align 1
@str.const.367 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.371 = private unnamed_addr constant [22 x i8] c"    \D8\A7\D9\84\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84:\00", align 1
@str.const.375 = private unnamed_addr constant [53 x i8] c"    \E2\94\9C\E2\94\80\E2\94\80 \D8\A7\D9\84\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA: 4/5 \D9\85\D8\AD\D8\AF\D8\AB\D8\A9 (80%)\00", align 1
@str.const.379 = private unnamed_addr constant [50 x i8] c"    \E2\94\9C\E2\94\80\E2\94\80 \D8\A7\D9\84\D8\A3\D9\85\D8\A7\D9\86: 2 \D8\AB\D8\BA\D8\B1\D8\A7\D8\AA (70/100)\00", align 1
@str.const.383 = private unnamed_addr constant [59 x i8] c"    \E2\94\9C\E2\94\80\E2\94\80 \D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA: 22/24 \D9\86\D8\A7\D8\AC\D8\AD\D8\A9 (91%)\00", align 1
@str.const.387 = private unnamed_addr constant [62 x i8] c"    \E2\94\94\E2\94\80\E2\94\80 \D8\A7\D9\84\D8\AA\D9\88\D8\AB\D9\8A\D9\82: 12/15 \D8\AF\D9\88\D8\A7\D9\84 \D9\85\D9\88\D8\AB\D9\82\D8\A9 (80%)\00", align 1
@str.const.391 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.395 = private unnamed_addr constant [23 x i8] c"    \F0\9F\92\A1 \D8\AA\D9\88\D8\B5\D9\8A\D8\A7\D8\AA:\00", align 1
@str.const.399 = private unnamed_addr constant [79 x i8] c"    1. \D8\AD\D8\AF\D9\91\D8\AB \D8\B4\D8\A8\D9\83\D8\A9 \D8\A5\D9\84\D9\89 1.0.0 \D9\84\D8\A5\D8\B5\D9\84\D8\A7\D8\AD \D8\A7\D9\84\D8\AB\D8\BA\D8\B1\D8\A9 \D8\A7\D9\84\D8\A3\D9\85\D9\86\D9\8A\D8\A9\00", align 1
@str.const.403 = private unnamed_addr constant [77 x i8] c"    2. \D8\A3\D8\B5\D9\84\D8\AD \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D9\8E\D9\8A\D9\86 \D9\81\D8\A7\D8\B4\D9\84\D9\8A\D9\86 \D9\81\D9\8A \D9\88\D8\AD\D8\AF\D8\A9 \D8\A7\D9\84\D8\AA\D8\AD\D9\88\D9\8A\D9\84\00", align 1
@str.const.407 = private unnamed_addr constant [40 x i8] c"    3. \D9\88\D8\AB\D9\91\D9\82 3 \D8\AF\D9\88\D8\A7\D9\84 \D8\A5\D8\B6\D8\A7\D9\81\D9\8A\D8\A9\00", align 1
@str.const.411 = private unnamed_addr constant [181 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.415 = private unnamed_addr constant [92 x i8] c"\E2\95\91  \F0\9F\8F\AA \D8\B3\D8\AC\D9\84 \D8\AD\D8\B2\D9\85 \D8\B5 \D8\A7\D9\84\D9\85\D8\B1\D9\83\D8\B2\D9\8A - \D8\A8\D8\AD\D8\AB \D9\88\D8\AA\D9\82\D9\8A\D9\8A\D9\85 \D9\88\D8\A3\D9\85\D8\A7\D9\86              \E2\95\91\00", align 1
@str.const.419 = private unnamed_addr constant [181 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.423 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.427 = private unnamed_addr constant [57 x i8] c"  \E2\9C\85 \D8\A7\D9\83\D8\AA\D9\85\D9\84 \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\B3\D8\AC\D9\84 \D8\A7\D9\84\D8\AD\D8\B2\D9\85 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD\00", align 1

define i64 @"\D8\AE\D8\B7_\D8\B9\D8\B1\D9\8A\D8\B6"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\AE\D8\B7_\D8\B1\D9\81\D9\8A\D8\B9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\AE\D8\B7_\D9\86\D8\AC\D9\88\D9\85"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr %"\D8\B9") {
entry:
  %"\D8\B91" = alloca ptr, align 8
  store ptr %"\D8\B9", ptr %"\D8\B91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.11)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\AE\D8\B7_\D8\B9\D8\B1\D9\8A\D8\B6"()
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.15)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\B91")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp2 = call i64 @"\D8\AE\D8\B7_\D8\B9\D8\B1\D9\8A\D8\B6"()
  ret i64 0
}

define i64 @"\D9\82\D8\B3\D9\85_\D9\81\D8\B1\D8\B9\D9\8A"(ptr %"\D8\B9") {
entry:
  %"\D8\B91" = alloca ptr, align 8
  store ptr %"\D8\B9", ptr %"\D8\B91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.20)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\AE\D8\B7_\D8\B1\D9\81\D9\8A\D8\B9"()
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.24)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\B91")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp2 = call i64 @"\D8\AE\D8\B7_\D8\B1\D9\81\D9\8A\D8\B9"()
  ret i64 0
}

define i64 @"\D8\AD\D9\82\D9\84"(ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD", ptr %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"\D9\82\D9\8A\D9\85\D8\A92" = alloca ptr, align 8
  store ptr %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1" = alloca ptr, align 8
  store ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD", ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.29)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.32)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\82\D9\8A\D9\85\D8\A92")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\AD\D9\82\D9\84_\D8\B1\D9\82\D9\85\D9\8A"(ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD", i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"\D9\82\D9\8A\D9\85\D8\A92" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1" = alloca ptr, align 8
  store ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD", ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.29)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.32)
  %"%\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\85\D8\B7\D9\84\D9\82\D8\A9"(i64 %"\D9\82") {
entry:
  %"\D9\821" = alloca i64, align 8
  store i64 %"\D9\82", ptr %"\D9\821", align 8
  %alloca = alloca i64, align 8
  %"%\D9\82.load" = load i64, ptr %"\D9\821", align 8
  store i64 %"%\D9\82.load", ptr %alloca, align 8
  %"%\D9\82.load2" = load i64, ptr %"\D9\821", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82.load2", 0
  br i1 %cmplttmp, label %then_0, label %merge_1

then_0:                                           ; preds = %entry
  %"%\D9\82.load3" = load i64, ptr %"\D9\821", align 8
  %subtmp = sub i64 0, %"%\D9\82.load3"
  store i64 %subtmp, ptr %alloca, align 8
  br label %merge_1

merge_1:                                          ; preds = %then_0, %entry
  %loadtmp = load i64, ptr %alloca, align 8
  ret i64 %loadtmp
}

define i64 @"\D8\A7\D9\83\D8\A8\D8\B1_\D8\B9\D8\AF\D8\AF\D9\8A\D9\86"(i64 %"\D8\A7", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A71" = alloca i64, align 8
  store i64 %"\D8\A7", ptr %"\D8\A71", align 8
  %alloca = alloca i64, align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  store i64 %"%\D8\A8.load", ptr %alloca, align 8
  %"%\D8\A7.load" = load i64, ptr %"\D8\A71", align 8
  %"%\D8\A8.load3" = load i64, ptr %"\D8\A82", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A7.load", %"%\D8\A8.load3"
  br i1 %cmpgttmp, label %then_2, label %merge_3

then_2:                                           ; preds = %entry
  %"%\D8\A7.load4" = load i64, ptr %"\D8\A71", align 8
  store i64 %"%\D8\A7.load4", ptr %alloca, align 8
  br label %merge_3

merge_3:                                          ; preds = %then_2, %entry
  %loadtmp = load i64, ptr %alloca, align 8
  ret i64 %loadtmp
}

define i64 @"\D8\A7\D8\B5\D8\BA\D8\B1_\D8\B9\D8\AF\D8\AF\D9\8A\D9\86"(i64 %"\D8\A7", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A71" = alloca i64, align 8
  store i64 %"\D8\A7", ptr %"\D8\A71", align 8
  %alloca = alloca i64, align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  store i64 %"%\D8\A8.load", ptr %alloca, align 8
  %"%\D8\A7.load" = load i64, ptr %"\D8\A71", align 8
  %"%\D8\A8.load3" = load i64, ptr %"\D8\A82", align 8
  %cmplttmp = icmp slt i64 %"%\D8\A7.load", %"%\D8\A8.load3"
  br i1 %cmplttmp, label %then_4, label %merge_5

then_4:                                           ; preds = %entry
  %"%\D8\A7.load4" = load i64, ptr %"\D8\A71", align 8
  store i64 %"%\D8\A7.load4", ptr %alloca, align 8
  br label %merge_5

merge_5:                                          ; preds = %then_4, %entry
  %loadtmp = load i64, ptr %alloca, align 8
  ret i64 %loadtmp
}

define i64 @"\D8\A8\D8\A7\D9\82\D9\8A"(i64 %"\D8\A7", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A71" = alloca i64, align 8
  store i64 %"\D8\A7", ptr %"\D8\A71", align 8
  %"%\D8\A7.load" = load i64, ptr %"\D8\A71", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %divtmp = sdiv i64 %"%\D8\A7.load", %"%\D8\A8.load"
  %"%\D8\A8.load3" = load i64, ptr %"\D8\A82", align 8
  %multmp = mul i64 %divtmp, %"%\D8\A8.load3"
  %"%\D8\A7.load4" = load i64, ptr %"\D8\A71", align 8
  %subtmp = sub i64 %"%\D8\A7.load4", %multmp
  ret i64 %subtmp
}

define i64 @"\D9\82\D9\88\D8\A9"(i64 %"\D8\A7\D8\B3\D8\A7\D8\B3", i64 %"\D8\A7\D8\B3") {
entry:
  %"\D8\A7\D8\B32" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B3", ptr %"\D8\A7\D8\B32", align 8
  %"\D8\A7\D8\B3\D8\A7\D8\B31" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B3\D8\A7\D8\B3", ptr %"\D8\A7\D8\B3\D8\A7\D8\B31", align 8
  %alloca = alloca i64, align 8
  store i64 1, ptr %alloca, align 8
  %alloca3 = alloca i64, align 8
  store i64 0, ptr %alloca3, align 8
  br label %while_cond_6

while_cond_6:                                     ; preds = %while_body_7, %entry
  %"%\D8\B9\D8\AF.load" = load i64, ptr %alloca3, align 8
  %"%\D8\A7\D8\B3.load" = load i64, ptr %"\D8\A7\D8\B32", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9\D8\AF.load", %"%\D8\A7\D8\B3.load"
  br i1 %cmplttmp, label %while_body_7, label %while_exit_8

while_body_7:                                     ; preds = %while_cond_6
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %alloca, align 8
  %"%\D8\A7\D8\B3\D8\A7\D8\B3.load" = load i64, ptr %"\D8\A7\D8\B3\D8\A7\D8\B31", align 8
  %multmp = mul i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", %"%\D8\A7\D8\B3\D8\A7\D8\B3.load"
  store i64 %multmp, ptr %alloca, align 8
  %"%\D8\B9\D8\AF.load4" = load i64, ptr %alloca3, align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF.load4", 1
  store i64 %addtmp, ptr %alloca3, align 8
  br label %while_cond_6

while_exit_8:                                     ; preds = %while_cond_6
  %loadtmp = load i64, ptr %alloca, align 8
  ret i64 %loadtmp
}

define ptr @"\D9\86\D8\AC\D9\88\D9\85_\D8\AA\D9\82\D9\8A\D9\8A\D9\85"(i64 %"\D8\AA\D9\82\D9\8A\D9\8A\D9\85") {
entry:
  %"\D8\AA\D9\82\D9\8A\D9\8A\D9\851" = alloca i64, align 8
  store i64 %"\D8\AA\D9\82\D9\8A\D9\8A\D9\85", ptr %"\D8\AA\D9\82\D9\8A\D9\8A\D9\851", align 8
  %alloca = alloca ptr, align 8
  store ptr @str.const.44, ptr %alloca, align 8
  %loadtmp = load i64, ptr %"\D8\AA\D9\82\D9\8A\D9\8A\D9\851", align 8
  br label %match.case0.test_10

match.end_9:                                      ; preds = %match.case4.body_19, %match.case4.test_18, %match.case3.body_17, %match.case2.body_15, %match.case1.body_13, %match.case0.body_11
  %loadtmp2 = load ptr, ptr %alloca, align 8
  ret ptr %loadtmp2

match.case0.test_10:                              ; preds = %entry
  %cmpeqtmp = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp, label %match.case0.body_11, label %match.case1.test_12

match.case0.body_11:                              ; preds = %match.case0.test_10
  store ptr @str.const.45, ptr %alloca, align 8
  br label %match.end_9

match.case1.test_12:                              ; preds = %match.case0.test_10
  %cmpeqtmp3 = icmp eq i64 %loadtmp, 2
  br i1 %cmpeqtmp3, label %match.case1.body_13, label %match.case2.test_14

match.case1.body_13:                              ; preds = %match.case1.test_12
  store ptr @str.const.46, ptr %alloca, align 8
  br label %match.end_9

match.case2.test_14:                              ; preds = %match.case1.test_12
  %cmpeqtmp4 = icmp eq i64 %loadtmp, 3
  br i1 %cmpeqtmp4, label %match.case2.body_15, label %match.case3.test_16

match.case2.body_15:                              ; preds = %match.case2.test_14
  store ptr @str.const.47, ptr %alloca, align 8
  br label %match.end_9

match.case3.test_16:                              ; preds = %match.case2.test_14
  %cmpeqtmp5 = icmp eq i64 %loadtmp, 4
  br i1 %cmpeqtmp5, label %match.case3.body_17, label %match.case4.test_18

match.case3.body_17:                              ; preds = %match.case3.test_16
  store ptr @str.const.48, ptr %alloca, align 8
  br label %match.end_9

match.case4.test_18:                              ; preds = %match.case3.test_16
  %cmpeqtmp6 = icmp eq i64 %loadtmp, 5
  br i1 %cmpeqtmp6, label %match.case4.body_19, label %match.end_9

match.case4.body_19:                              ; preds = %match.case4.test_18
  store ptr @str.const.49, ptr %alloca, align 8
  br label %match.end_9
}

define ptr @"\D9\85\D8\B3\D8\AA\D9\88\D9\89_\D8\B4\D8\B9\D8\A8\D9\8A\D8\A9"(i64 %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA") {
entry:
  %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA1" = alloca i64, align 8
  store i64 %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA", ptr %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA1", align 8
  %alloca = alloca ptr, align 8
  store ptr @str.const.50, ptr %alloca, align 8
  %"%\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA.load" = load i64, ptr %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA1", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA.load", 10000
  br i1 %cmpgetmp, label %then_20, label %else_22

then_20:                                          ; preds = %entry
  store ptr @str.const.51, ptr %alloca, align 8
  br label %merge_21

else_22:                                          ; preds = %entry
  %"%\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA.load2" = load i64, ptr %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA1", align 8
  %cmpgetmp3 = icmp sge i64 %"%\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA.load2", 5000
  br i1 %cmpgetmp3, label %then_23, label %else_25

merge_21:                                         ; preds = %merge_24, %then_20
  %loadtmp = load ptr, ptr %alloca, align 8
  ret ptr %loadtmp

then_23:                                          ; preds = %else_22
  store ptr @str.const.52, ptr %alloca, align 8
  br label %merge_24

else_25:                                          ; preds = %else_22
  %"%\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA.load4" = load i64, ptr %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA1", align 8
  %cmpgetmp5 = icmp sge i64 %"%\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA.load4", 1000
  br i1 %cmpgetmp5, label %then_26, label %else_28

merge_24:                                         ; preds = %merge_27, %then_23
  br label %merge_21

then_26:                                          ; preds = %else_25
  store ptr @str.const.53, ptr %alloca, align 8
  br label %merge_27

else_28:                                          ; preds = %else_25
  %"%\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA.load6" = load i64, ptr %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA1", align 8
  %cmpgetmp7 = icmp sge i64 %"%\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA.load6", 100
  br i1 %cmpgetmp7, label %then_29, label %else_31

merge_27:                                         ; preds = %merge_30, %then_26
  br label %merge_24

then_29:                                          ; preds = %else_28
  store ptr @str.const.54, ptr %alloca, align 8
  br label %merge_30

else_31:                                          ; preds = %else_28
  store ptr @str.const.55, ptr %alloca, align 8
  br label %merge_30

merge_30:                                         ; preds = %else_31, %then_29
  br label %merge_27
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D8\AD\D8\B2\D9\85\D8\A9"(ptr %"\D8\AD\D8\B2\D9\85\D8\A9", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", ptr %"\D9\88\D8\B5\D9\81", ptr %"\D9\85\D8\A4\D9\84\D9\81", i64 %"\D8\AA\D9\82\D9\8A\D9\8A\D9\85", i64 %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA", ptr %"\D8\B1\D8\AE\D8\B5\D8\A9") {
entry:
  %"\D8\B1\D8\AE\D8\B5\D8\A97" = alloca ptr, align 8
  store ptr %"\D8\B1\D8\AE\D8\B5\D8\A9", ptr %"\D8\B1\D8\AE\D8\B5\D8\A97", align 8
  %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA6" = alloca i64, align 8
  store i64 %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA", ptr %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA6", align 8
  %"\D8\AA\D9\82\D9\8A\D9\8A\D9\855" = alloca i64, align 8
  store i64 %"\D8\AA\D9\82\D9\8A\D9\8A\D9\85", ptr %"\D8\AA\D9\82\D9\8A\D9\8A\D9\855", align 8
  %"\D9\85\D8\A4\D9\84\D9\814" = alloca ptr, align 8
  store ptr %"\D9\85\D8\A4\D9\84\D9\81", ptr %"\D9\85\D8\A4\D9\84\D9\814", align 8
  %"\D9\88\D8\B5\D9\813" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81", ptr %"\D9\88\D8\B5\D9\813", align 8
  %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12", align 8
  %"\D8\AD\D8\B2\D9\85\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\AD\D8\B2\D9\85\D8\A9", ptr %"\D8\AD\D8\B2\D9\85\D8\A91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.56)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.60)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.62)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.66)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\AD\D8\B2\D9\85\D8\A91")
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.69)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12")
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.74)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\88\D8\B5\D9\813")
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.79)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\85\D8\A4\D9\84\D9\814")
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.84)
  %"%\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load" = load i64, ptr %"\D8\AA\D9\82\D9\8A\D9\8A\D9\855", align 8
  %calltmp = call ptr @"\D9\86\D8\AC\D9\88\D9\85_\D8\AA\D9\82\D9\8A\D9\8A\D9\85"(i64 %"%\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load")
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.87)
  %"%\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load8" = load i64, ptr %"\D8\AA\D9\82\D9\8A\D9\8A\D9\855", align 8
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load8")
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.90)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.94)
  %"%\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA.load" = load i64, ptr %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA6", align 8
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA.load")
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.97)
  %"%\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA.load9" = load i64, ptr %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA6", align 8
  %calltmp10 = call ptr @"\D9\85\D8\B3\D8\AA\D9\88\D9\89_\D8\B4\D8\B9\D8\A8\D9\8A\D8\A9"(i64 %"%\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA.load9")
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp10)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.102)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\B1\D8\AE\D8\B5\D8\A97")
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.107)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.109)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D8\B3\D8\AC\D9\84_\D8\AD\D8\B2\D9\85"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr @str.const.113)
  %calltmp1 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AD\D8\B2\D9\85\D8\A9"(ptr @str.const.114, ptr @str.const.115, ptr @str.const.116, ptr @str.const.117, i64 5, i64 15420, ptr @str.const.118)
  %calltmp2 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AD\D8\B2\D9\85\D8\A9"(ptr @str.const.119, ptr @str.const.120, ptr @str.const.121, ptr @str.const.122, i64 4, i64 8750, ptr @str.const.123)
  %calltmp3 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AD\D8\B2\D9\85\D8\A9"(ptr @str.const.124, ptr @str.const.125, ptr @str.const.126, ptr @str.const.127, i64 5, i64 12300, ptr @str.const.118)
  %calltmp4 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AD\D8\B2\D9\85\D8\A9"(ptr @str.const.129, ptr @str.const.130, ptr @str.const.131, ptr @str.const.132, i64 3, i64 4200, ptr @str.const.133)
  %calltmp5 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AD\D8\B2\D9\85\D8\A9"(ptr @str.const.134, ptr @str.const.135, ptr @str.const.136, ptr @str.const.137, i64 4, i64 6800, ptr @str.const.138)
  ret i64 0
}

define i64 @"\D8\AF\D8\B1\D8\AC\D8\A9_\D9\85\D9\84\D8\A7\D8\A1\D9\85\D8\A9"(i64 %"\D8\AA\D9\82\D9\8A\D9\8A\D9\85", i64 %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA", i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1\D8\A7\D8\AA") {
entry:
  %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1\D8\A7\D8\AA3" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1\D8\A7\D8\AA", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1\D8\A7\D8\AA3", align 8
  %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA2" = alloca i64, align 8
  store i64 %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA", ptr %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA2", align 8
  %"\D8\AA\D9\82\D9\8A\D9\8A\D9\851" = alloca i64, align 8
  store i64 %"\D8\AA\D9\82\D9\8A\D9\8A\D9\85", ptr %"\D8\AA\D9\82\D9\8A\D9\8A\D9\851", align 8
  %"%\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load" = load i64, ptr %"\D8\AA\D9\82\D9\8A\D9\8A\D9\851", align 8
  %multmp = mul i64 %"%\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load", 200
  %alloca = alloca i64, align 8
  store i64 %multmp, ptr %alloca, align 8
  %"%\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA.load" = load i64, ptr %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA2", align 8
  %divtmp = sdiv i64 %"%\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA.load", 10
  %alloca4 = alloca i64, align 8
  store i64 %divtmp, ptr %alloca4, align 8
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1\D8\A7\D8\AA.load" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1\D8\A7\D8\AA3", align 8
  %multmp5 = mul i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1\D8\A7\D8\AA.load", 50
  %alloca6 = alloca i64, align 8
  store i64 %multmp5, ptr %alloca6, align 8
  %"%\D9\86\D9\82\D8\A7\D8\B7_\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load" = load i64, ptr %alloca, align 8
  %"%\D9\86\D9\82\D8\A7\D8\B7_\D8\B4\D8\B9\D8\A8\D9\8A\D8\A9.load" = load i64, ptr %alloca4, align 8
  %addtmp = add i64 %"%\D9\86\D9\82\D8\A7\D8\B7_\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load", %"%\D9\86\D9\82\D8\A7\D8\B7_\D8\B4\D8\B9\D8\A8\D9\8A\D8\A9.load"
  %"%\D9\86\D9\82\D8\A7\D8\B7_\D9\86\D8\B4\D8\A7\D8\B7.load" = load i64, ptr %alloca6, align 8
  %addtmp7 = add i64 %addtmp, %"%\D9\86\D9\82\D8\A7\D8\B7_\D9\86\D8\B4\D8\A7\D8\B7.load"
  ret i64 %addtmp7
}

define i64 @"\D8\A8\D8\AD\D8\AB_\D8\AD\D8\B2\D9\85"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr @str.const.139)
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.140)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.144)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.148)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp1 = call i64 @"\D8\AF\D8\B1\D8\AC\D8\A9_\D9\85\D9\84\D8\A7\D8\A1\D9\85\D8\A9"(i64 5, i64 15420, i64 12)
  %alloca = alloca i64, align 8
  store i64 %calltmp1, ptr %alloca, align 8
  %calltmp2 = call i64 @"\D8\AF\D8\B1\D8\AC\D8\A9_\D9\85\D9\84\D8\A7\D8\A1\D9\85\D8\A9"(i64 4, i64 8750, i64 8)
  %alloca3 = alloca i64, align 8
  store i64 %calltmp2, ptr %alloca3, align 8
  %calltmp4 = call i64 @"\D8\AF\D8\B1\D8\AC\D8\A9_\D9\85\D9\84\D8\A7\D8\A1\D9\85\D8\A9"(i64 3, i64 4200, i64 5)
  %alloca5 = alloca i64, align 8
  store i64 %calltmp4, ptr %alloca5, align 8
  %calltmp6 = call i64 @"\D8\AF\D8\B1\D8\AC\D8\A9_\D9\85\D9\84\D8\A7\D8\A1\D9\85\D8\A9"(i64 4, i64 2100, i64 3)
  %alloca7 = alloca i64, align 8
  store i64 %calltmp6, ptr %alloca7, align 8
  %calltmp8 = call i64 @"\D8\AF\D8\B1\D8\AC\D8\A9_\D9\85\D9\84\D8\A7\D8\A1\D9\85\D8\A9"(i64 2, i64 500, i64 2)
  %alloca9 = alloca i64, align 8
  store i64 %calltmp8, ptr %alloca9, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.152)
  %"%\D8\AF\D8\B1\D8\AC\D8\A91.load" = load i64, ptr %alloca, align 8
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AF\D8\B1\D8\AC\D8\A91.load")
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.155)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp10 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.159, ptr @str.const.160)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.161)
  %"%\D8\AF\D8\B1\D8\AC\D8\A92.load" = load i64, ptr %alloca3, align 8
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AF\D8\B1\D8\AC\D8\A92.load")
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.155)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp11 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.159, ptr @str.const.169)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.170)
  %"%\D8\AF\D8\B1\D8\AC\D8\A93.load" = load i64, ptr %alloca5, align 8
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AF\D8\B1\D8\AC\D8\A93.load")
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.155)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp12 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.159, ptr @str.const.178)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.179)
  %"%\D8\AF\D8\B1\D8\AC\D8\A94.load" = load i64, ptr %alloca7, align 8
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AF\D8\B1\D8\AC\D8\A94.load")
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.155)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp13 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.159, ptr @str.const.187)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.188)
  %"%\D8\AF\D8\B1\D8\AC\D8\A95.load" = load i64, ptr %alloca9, align 8
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AF\D8\B1\D8\AC\D8\A95.load")
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.155)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp14 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.159, ptr @str.const.196)
  ret i64 0
}

define ptr @"\D9\85\D8\B3\D8\AA\D9\88\D9\89_\D8\AE\D8\B7\D9\88\D8\B1\D8\A9"(i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89") {
entry:
  %"\D9\85\D8\B3\D8\AA\D9\88\D9\891" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %alloca = alloca ptr, align 8
  store ptr @str.const.197, ptr %alloca, align 8
  %loadtmp = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  br label %match.case0.test_33

match.end_32:                                     ; preds = %match.case4.body_42, %match.case4.test_41, %match.case3.body_40, %match.case2.body_38, %match.case1.body_36, %match.case0.body_34
  %loadtmp2 = load ptr, ptr %alloca, align 8
  ret ptr %loadtmp2

match.case0.test_33:                              ; preds = %entry
  %cmpeqtmp = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp, label %match.case0.body_34, label %match.case1.test_35

match.case0.body_34:                              ; preds = %match.case0.test_33
  store ptr @str.const.198, ptr %alloca, align 8
  br label %match.end_32

match.case1.test_35:                              ; preds = %match.case0.test_33
  %cmpeqtmp3 = icmp eq i64 %loadtmp, 2
  br i1 %cmpeqtmp3, label %match.case1.body_36, label %match.case2.test_37

match.case1.body_36:                              ; preds = %match.case1.test_35
  store ptr @str.const.199, ptr %alloca, align 8
  br label %match.end_32

match.case2.test_37:                              ; preds = %match.case1.test_35
  %cmpeqtmp4 = icmp eq i64 %loadtmp, 3
  br i1 %cmpeqtmp4, label %match.case2.body_38, label %match.case3.test_39

match.case2.body_38:                              ; preds = %match.case2.test_37
  store ptr @str.const.200, ptr %alloca, align 8
  br label %match.end_32

match.case3.test_39:                              ; preds = %match.case2.test_37
  %cmpeqtmp5 = icmp eq i64 %loadtmp, 4
  br i1 %cmpeqtmp5, label %match.case3.body_40, label %match.case4.test_41

match.case3.body_40:                              ; preds = %match.case3.test_39
  store ptr @str.const.201, ptr %alloca, align 8
  br label %match.end_32

match.case4.test_41:                              ; preds = %match.case3.test_39
  %cmpeqtmp6 = icmp eq i64 %loadtmp, 5
  br i1 %cmpeqtmp6, label %match.case4.body_42, label %match.end_32

match.case4.body_42:                              ; preds = %match.case4.test_41
  store ptr @str.const.202, ptr %alloca, align 8
  br label %match.end_32
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\A7\D9\85\D8\A7\D9\86"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr @str.const.203)
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.204)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.208)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.212)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.216)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.220)
  %calltmp1 = call ptr @"\D9\85\D8\B3\D8\AA\D9\88\D9\89_\D8\AE\D8\B7\D9\88\D8\B1\D8\A9"(i64 4)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp2 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.225, ptr @str.const.226)
  %calltmp3 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.227, ptr @str.const.228)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.229)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.233)
  %calltmp4 = call ptr @"\D9\85\D8\B3\D8\AA\D9\88\D9\89_\D8\AE\D8\B7\D9\88\D8\B1\D8\A9"(i64 3)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp4)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp5 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.225, ptr @str.const.239)
  %calltmp6 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.227, ptr @str.const.241)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.242)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.246)
  %calltmp7 = call ptr @"\D9\85\D8\B3\D8\AA\D9\88\D9\89_\D8\AE\D8\B7\D9\88\D8\B1\D8\A9"(i64 2)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp7)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp8 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.225, ptr @str.const.252)
  %calltmp9 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.227, ptr @str.const.254)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.255)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp10 = call i64 @"\D9\82\D8\B3\D9\85_\D9\81\D8\B1\D8\B9\D9\8A"(ptr @str.const.259)
  %calltmp11 = call i64 @"\D8\AD\D9\82\D9\84_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.260, i64 5)
  %calltmp12 = call i64 @"\D8\AD\D9\82\D9\84_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.261, i64 0)
  %calltmp13 = call i64 @"\D8\AD\D9\82\D9\84_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.262, i64 1)
  %calltmp14 = call i64 @"\D8\AD\D9\82\D9\84_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.263, i64 1)
  %calltmp15 = call i64 @"\D8\AD\D9\82\D9\84_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.264, i64 1)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.265)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.269)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA_\D8\B3\D8\AC\D9\84"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr @str.const.273)
  %calltmp1 = call i64 @"\D8\AD\D9\82\D9\84_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.274, i64 1247)
  %calltmp2 = call i64 @"\D8\AD\D9\82\D9\84_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.275, i64 89500)
  %calltmp3 = call i64 @"\D8\AD\D9\82\D9\84_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.276, i64 342)
  %calltmp4 = call i64 @"\D8\AD\D9\82\D9\84_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.277, i64 23)
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.278)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp5 = call i64 @"\D9\82\D8\B3\D9\85_\D9\81\D8\B1\D8\B9\D9\8A"(ptr @str.const.282)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.283)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.287)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.291)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.295)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.299)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp6 = call i64 @"\D9\82\D8\B3\D9\85_\D9\81\D8\B1\D8\B9\D9\8A"(ptr @str.const.303)
  %alloca = alloca i64, align 8
  store i64 1, ptr %alloca, align 8
  br label %while_cond_43

while_cond_43:                                    ; preds = %match.end_46, %entry
  %"%\D8\B9\D8\AF.load" = load i64, ptr %alloca, align 8
  %cmpletmp = icmp sle i64 %"%\D8\B9\D8\AF.load", 8
  br i1 %cmpletmp, label %while_body_44, label %while_exit_45

while_body_44:                                    ; preds = %while_cond_43
  %loadtmp = load i64, ptr %alloca, align 8
  br label %match.case0.test_47

while_exit_45:                                    ; preds = %while_cond_43
  %calltmp7 = call i64 @"\D9\82\D8\B3\D9\85_\D9\81\D8\B1\D8\B9\D9\8A"(ptr @str.const.304)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.305)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.309)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.313)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.317)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0

match.end_46:                                     ; preds = %match.case7.body_62, %match.case7.test_61, %match.case6.body_60, %match.case5.body_58, %match.case4.body_56, %match.case3.body_54, %match.case2.body_52, %match.case1.body_50, %match.case0.body_48
  %"%\D8\B9\D8\AF.load8" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF.load8", 1
  store i64 %addtmp, ptr %alloca, align 8
  br label %while_cond_43

match.case0.test_47:                              ; preds = %while_body_44
  %cmpeqtmp = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp, label %match.case0.body_48, label %match.case1.test_49

match.case0.body_48:                              ; preds = %match.case0.test_47
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.321)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_46

match.case1.test_49:                              ; preds = %match.case0.test_47
  %cmpeqtmp9 = icmp eq i64 %loadtmp, 2
  br i1 %cmpeqtmp9, label %match.case1.body_50, label %match.case2.test_51

match.case1.body_50:                              ; preds = %match.case1.test_49
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.325)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_46

match.case2.test_51:                              ; preds = %match.case1.test_49
  %cmpeqtmp10 = icmp eq i64 %loadtmp, 3
  br i1 %cmpeqtmp10, label %match.case2.body_52, label %match.case3.test_53

match.case2.body_52:                              ; preds = %match.case2.test_51
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.329)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_46

match.case3.test_53:                              ; preds = %match.case2.test_51
  %cmpeqtmp11 = icmp eq i64 %loadtmp, 4
  br i1 %cmpeqtmp11, label %match.case3.body_54, label %match.case4.test_55

match.case3.body_54:                              ; preds = %match.case3.test_53
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.333)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_46

match.case4.test_55:                              ; preds = %match.case3.test_53
  %cmpeqtmp12 = icmp eq i64 %loadtmp, 5
  br i1 %cmpeqtmp12, label %match.case4.body_56, label %match.case5.test_57

match.case4.body_56:                              ; preds = %match.case4.test_55
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.337)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_46

match.case5.test_57:                              ; preds = %match.case4.test_55
  %cmpeqtmp13 = icmp eq i64 %loadtmp, 6
  br i1 %cmpeqtmp13, label %match.case5.body_58, label %match.case6.test_59

match.case5.body_58:                              ; preds = %match.case5.test_57
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.341)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_46

match.case6.test_59:                              ; preds = %match.case5.test_57
  %cmpeqtmp14 = icmp eq i64 %loadtmp, 7
  br i1 %cmpeqtmp14, label %match.case6.body_60, label %match.case7.test_61

match.case6.body_60:                              ; preds = %match.case6.test_59
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.345)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_46

match.case7.test_61:                              ; preds = %match.case6.test_59
  %cmpeqtmp15 = icmp eq i64 %loadtmp, 8
  br i1 %cmpeqtmp15, label %match.case7.body_62, label %match.end_46

match.case7.body_62:                              ; preds = %match.case7.test_61
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.349)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_46
}

define i64 @"\D9\86\D9\82\D8\A7\D8\B7_\D8\B5\D8\AD\D8\A9"(i64 %"\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D9\85\D8\AD\D8\AF\D8\AB\D8\A9", i64 %"\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D9\83\D9\84\D9\8A\D8\A9", i64 %"\D8\AB\D8\BA\D8\B1\D8\A7\D8\AA", i64 %"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA_\D9\86\D8\A7\D8\AC\D8\AD\D8\A9", i64 %"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA_\D9\83\D9\84\D9\8A\D8\A9") {
entry:
  %"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA_\D9\83\D9\84\D9\8A\D8\A95" = alloca i64, align 8
  store i64 %"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA_\D9\83\D9\84\D9\8A\D8\A9", ptr %"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA_\D9\83\D9\84\D9\8A\D8\A95", align 8
  %"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA_\D9\86\D8\A7\D8\AC\D8\AD\D8\A94" = alloca i64, align 8
  store i64 %"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA_\D9\86\D8\A7\D8\AC\D8\AD\D8\A9", ptr %"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA_\D9\86\D8\A7\D8\AC\D8\AD\D8\A94", align 8
  %"\D8\AB\D8\BA\D8\B1\D8\A7\D8\AA3" = alloca i64, align 8
  store i64 %"\D8\AB\D8\BA\D8\B1\D8\A7\D8\AA", ptr %"\D8\AB\D8\BA\D8\B1\D8\A7\D8\AA3", align 8
  %"\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D9\83\D9\84\D9\8A\D8\A92" = alloca i64, align 8
  store i64 %"\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D9\83\D9\84\D9\8A\D8\A9", ptr %"\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D9\83\D9\84\D9\8A\D8\A92", align 8
  %"\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D9\85\D8\AD\D8\AF\D8\AB\D8\A91" = alloca i64, align 8
  store i64 %"\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D9\85\D8\AD\D8\AF\D8\AB\D8\A9", ptr %"\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D9\85\D8\AD\D8\AF\D8\AB\D8\A91", align 8
  %"%\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D9\85\D8\AD\D8\AF\D8\AB\D8\A9.load" = load i64, ptr %"\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D9\85\D8\AD\D8\AF\D8\AB\D8\A91", align 8
  %multmp = mul i64 %"%\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D9\85\D8\AD\D8\AF\D8\AB\D8\A9.load", 100
  %"%\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D9\83\D9\84\D9\8A\D8\A9.load" = load i64, ptr %"\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D9\83\D9\84\D9\8A\D8\A92", align 8
  %divtmp = sdiv i64 %multmp, %"%\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D9\83\D9\84\D9\8A\D8\A9.load"
  %alloca = alloca i64, align 8
  store i64 %divtmp, ptr %alloca, align 8
  %"%\D8\AB\D8\BA\D8\B1\D8\A7\D8\AA.load" = load i64, ptr %"\D8\AB\D8\BA\D8\B1\D8\A7\D8\AA3", align 8
  %multmp6 = mul i64 %"%\D8\AB\D8\BA\D8\B1\D8\A7\D8\AA.load", 15
  %subtmp = sub i64 100, %multmp6
  %alloca7 = alloca i64, align 8
  store i64 %subtmp, ptr %alloca7, align 8
  %"%\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA_\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load" = load i64, ptr %"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA_\D9\86\D8\A7\D8\AC\D8\AD\D8\A94", align 8
  %multmp8 = mul i64 %"%\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA_\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load", 100
  %"%\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA_\D9\83\D9\84\D9\8A\D8\A9.load" = load i64, ptr %"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA_\D9\83\D9\84\D9\8A\D8\A95", align 8
  %divtmp9 = sdiv i64 %multmp8, %"%\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA_\D9\83\D9\84\D9\8A\D8\A9.load"
  %alloca10 = alloca i64, align 8
  store i64 %divtmp9, ptr %alloca10, align 8
  %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB.load" = load i64, ptr %alloca, align 8
  %"%\D9\86\D9\82\D8\A7\D8\B7_\D8\A7\D9\85\D8\A7\D9\86.load" = load i64, ptr %alloca7, align 8
  %addtmp = add i64 %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB.load", %"%\D9\86\D9\82\D8\A7\D8\B7_\D8\A7\D9\85\D8\A7\D9\86.load"
  %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1.load" = load i64, ptr %alloca10, align 8
  %addtmp11 = add i64 %addtmp, %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1.load"
  %divtmp12 = sdiv i64 %addtmp11, 3
  ret i64 %divtmp12
}

define ptr @"\D8\AA\D8\B5\D9\86\D9\8A\D9\81_\D8\B5\D8\AD\D8\A9"(i64 %"\D9\86\D9\82\D8\A7\D8\B7") {
entry:
  %"\D9\86\D9\82\D8\A7\D8\B71" = alloca i64, align 8
  store i64 %"\D9\86\D9\82\D8\A7\D8\B7", ptr %"\D9\86\D9\82\D8\A7\D8\B71", align 8
  %alloca = alloca ptr, align 8
  store ptr @str.const.353, ptr %alloca, align 8
  %"%\D9\86\D9\82\D8\A7\D8\B7.load" = load i64, ptr %"\D9\86\D9\82\D8\A7\D8\B71", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\86\D9\82\D8\A7\D8\B7.load", 90
  br i1 %cmpgetmp, label %then_63, label %else_65

then_63:                                          ; preds = %entry
  store ptr @str.const.354, ptr %alloca, align 8
  br label %merge_64

else_65:                                          ; preds = %entry
  %"%\D9\86\D9\82\D8\A7\D8\B7.load2" = load i64, ptr %"\D9\86\D9\82\D8\A7\D8\B71", align 8
  %cmpgetmp3 = icmp sge i64 %"%\D9\86\D9\82\D8\A7\D8\B7.load2", 75
  br i1 %cmpgetmp3, label %then_66, label %else_68

merge_64:                                         ; preds = %merge_67, %then_63
  %loadtmp = load ptr, ptr %alloca, align 8
  ret ptr %loadtmp

then_66:                                          ; preds = %else_65
  store ptr @str.const.355, ptr %alloca, align 8
  br label %merge_67

else_68:                                          ; preds = %else_65
  %"%\D9\86\D9\82\D8\A7\D8\B7.load4" = load i64, ptr %"\D9\86\D9\82\D8\A7\D8\B71", align 8
  %cmpgetmp5 = icmp sge i64 %"%\D9\86\D9\82\D8\A7\D8\B7.load4", 60
  br i1 %cmpgetmp5, label %then_69, label %else_71

merge_67:                                         ; preds = %merge_70, %then_66
  br label %merge_64

then_69:                                          ; preds = %else_68
  store ptr @str.const.356, ptr %alloca, align 8
  br label %merge_70

else_71:                                          ; preds = %else_68
  %"%\D9\86\D9\82\D8\A7\D8\B7.load6" = load i64, ptr %"\D9\86\D9\82\D8\A7\D8\B71", align 8
  %cmpgetmp7 = icmp sge i64 %"%\D9\86\D9\82\D8\A7\D8\B7.load6", 40
  br i1 %cmpgetmp7, label %then_72, label %merge_73

merge_70:                                         ; preds = %merge_73, %then_69
  br label %merge_67

then_72:                                          ; preds = %else_71
  store ptr @str.const.357, ptr %alloca, align 8
  br label %merge_73

merge_73:                                         ; preds = %then_72, %else_71
  br label %merge_70
}

define i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D8\B5\D8\AD\D8\A9"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr @str.const.358)
  %calltmp1 = call i64 @"\D9\86\D9\82\D8\A7\D8\B7_\D8\B5\D8\AD\D8\A9"(i64 4, i64 5, i64 2, i64 22, i64 24)
  %alloca = alloca i64, align 8
  store i64 %calltmp1, ptr %alloca, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.359)
  %"%\D9\86\D9\82\D8\A7\D8\B7.load" = load i64, ptr %alloca, align 8
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\86\D9\82\D8\A7\D8\B7.load")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.362)
  %"%\D9\86\D9\82\D8\A7\D8\B7.load2" = load i64, ptr %alloca, align 8
  %calltmp3 = call ptr @"\D8\AA\D8\B5\D9\86\D9\8A\D9\81_\D8\B5\D8\AD\D8\A9"(i64 %"%\D9\86\D9\82\D8\A7\D8\B7.load2")
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp3)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.367)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.371)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.375)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.379)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.383)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.387)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.391)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.395)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.399)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.403)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.407)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.411)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.415)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.419)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\B3\D8\AC\D9\84_\D8\AD\D8\B2\D9\85"()
  %calltmp1 = call i64 @"\D8\A8\D8\AD\D8\AB_\D8\AD\D8\B2\D9\85"()
  %calltmp2 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\A7\D9\85\D8\A7\D9\86"()
  %calltmp3 = call i64 @"\D8\A7\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA_\D8\B3\D8\AC\D9\84"()
  %calltmp4 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D8\B5\D8\AD\D8\A9"()
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.423)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp5 = call i64 @"\D8\AE\D8\B7_\D8\B9\D8\B1\D9\8A\D8\B6"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.427)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp6 = call i64 @"\D8\AE\D8\B7_\D8\B9\D8\B1\D9\8A\D8\B6"()
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
