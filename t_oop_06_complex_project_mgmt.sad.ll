; ModuleID = '06_complex_project_mgmt'
source_filename = "06_complex_project_mgmt"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.1 = private unnamed_addr constant [22 x i8] c"    \F0\9F\93\8B \D8\AA\D8\B9\D9\8A\D9\8A\D9\86 '\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.3 = private unnamed_addr constant [10 x i8] c"' \D8\A5\D9\84\D9\89 \00", align 1
@str.const.6 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.8 = private unnamed_addr constant [23 x i8] c"    \E2\9C\85 \D8\A7\D9\83\D8\AA\D9\85\D9\84\D8\AA: \00", align 1
@str.ret = private unnamed_addr constant [14 x i8] c"\F0\9F\94\B4 \D8\B9\D8\A7\D8\AC\D9\84\00", align 1
@str.ret.13 = private unnamed_addr constant [16 x i8] c"\F0\9F\9F\A1 \D9\85\D8\AA\D9\88\D8\B3\D8\B7\00", align 1
@str.ret.14 = private unnamed_addr constant [14 x i8] c"\F0\9F\9F\A2 \D8\B9\D8\A7\D8\AF\D9\8A\00", align 1
@str.const.15 = private unnamed_addr constant [5 x i8] c"    \00", align 1
@str.const.17 = private unnamed_addr constant [5 x i8] c"\F0\9F\94\B4\00", align 1
@str.const.19 = private unnamed_addr constant [2 x i8] c" \00", align 1
@str.const.22 = private unnamed_addr constant [5 x i8] c"\F0\9F\9F\A1\00", align 1
@str.const.24 = private unnamed_addr constant [5 x i8] c"\F0\9F\9F\A2\00", align 1
@str.const.26 = private unnamed_addr constant [20 x i8] c" [\E2\9C\85 \D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9]\00", align 1
@str.const.28 = private unnamed_addr constant [29 x i8] c" [\E2\8F\B3 \D9\82\D9\8A\D8\AF \D8\A7\D9\84\D8\AA\D9\86\D9\81\D9\8A\D8\B0]\00", align 1
@str.const.30 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.31 = private unnamed_addr constant [6 x i8] c" \E2\86\92 \00", align 1
@str.const.34 = private unnamed_addr constant [3 x i8] c" (\00", align 1
@fmt.d = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1
@str.const.36 = private unnamed_addr constant [2 x i8] c"/\00", align 1
@str.const.39 = private unnamed_addr constant [8 x i8] c" \D9\8A\D9\88\D9\85\00", align 1
@str.const.42 = private unnamed_addr constant [17 x i8] c" \D9\8A\D9\88\D9\85 \D9\85\D9\82\D8\AF\D8\B1\00", align 1
@str.const.44 = private unnamed_addr constant [2 x i8] c")\00", align 1
@str.ret.48 = private unnamed_addr constant [21 x i8] c"\E2\AD\90\E2\AD\90\E2\AD\90 \D9\85\D9\85\D8\AA\D8\A7\D8\B2\00", align 1
@str.ret.49 = private unnamed_addr constant [14 x i8] c"\E2\AD\90\E2\AD\90 \D8\AC\D9\8A\D8\AF\00", align 1
@str.ret.50 = private unnamed_addr constant [15 x i8] c"\E2\AD\90 \D9\85\D9\82\D8\A8\D9\88\D9\84\00", align 1
@str.ret.51 = private unnamed_addr constant [16 x i8] c"\E2\9A\A0\EF\B8\8F \D8\B6\D8\B9\D9\8A\D9\81\00", align 1
@str.const.52 = private unnamed_addr constant [101 x i8] c"    \E2\94\8C\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\90\00", align 1
@str.const.56 = private unnamed_addr constant [21 x i8] c"    \E2\94\82 \F0\9F\91\A8\E2\80\8D\F0\9F\92\BC \00", align 1
@str.const.59 = private unnamed_addr constant [4 x i8] c" - \00", align 1
@str.const.64 = private unnamed_addr constant [23 x i8] c"    \E2\94\82 \D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85: \00", align 1
@str.const.67 = private unnamed_addr constant [16 x i8] c" \D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9 (\00", align 1
@str.const.70 = private unnamed_addr constant [15 x i8] c" \D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9)\00", align 1
@str.const.74 = private unnamed_addr constant [25 x i8] c"    \E2\94\82 \D8\A7\D9\84\D8\B3\D8\A7\D8\B9\D8\A7\D8\AA: \00", align 1
@str.const.79 = private unnamed_addr constant [25 x i8] c"    \E2\94\82 \D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85: \00", align 1
@str.const.82 = private unnamed_addr constant [8 x i8] c"/100 - \00", align 1
@str.const.84 = private unnamed_addr constant [21 x i8] c"\D9\85\D9\85\D8\AA\D8\A7\D8\B2 \E2\AD\90\E2\AD\90\E2\AD\90\00", align 1
@str.const.88 = private unnamed_addr constant [101 x i8] c"    \E2\94\94\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\98\00", align 1
@str.const.92 = private unnamed_addr constant [14 x i8] c"\D8\AC\D9\8A\D8\AF \E2\AD\90\E2\AD\90\00", align 1
@str.const.96 = private unnamed_addr constant [15 x i8] c"\D9\85\D9\82\D8\A8\D9\88\D9\84 \E2\AD\90\00", align 1
@str.const.100 = private unnamed_addr constant [16 x i8] c"\D8\B6\D8\B9\D9\8A\D9\81 \E2\9A\A0\EF\B8\8F\00", align 1
@str.const.104 = private unnamed_addr constant [113 x i8] c"    \E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.108 = private unnamed_addr constant [36 x i8] c"    \E2\95\91  \F0\9F\93\8A \D9\85\D9\84\D8\AE\D8\B5 \D9\85\D8\B4\D8\B1\D9\88\D8\B9: \00", align 1
@str.const.113 = private unnamed_addr constant [113 x i8] c"    \E2\95\A0\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\A3\00", align 1
@str.const.117 = private unnamed_addr constant [41 x i8] c"    \E2\95\91  \D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A \D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85:     \00", align 1
@str.const.122 = private unnamed_addr constant [38 x i8] c"    \E2\95\91  \D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9:           \00", align 1
@str.const.127 = private unnamed_addr constant [38 x i8] c"    \E2\95\91  \D8\A7\D9\84\D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9:           \00", align 1
@str.const.132 = private unnamed_addr constant [37 x i8] c"    \E2\95\91  \D8\A7\D9\84\D8\B9\D8\A7\D8\AC\D9\84\D8\A9:            \00", align 1
@str.const.137 = private unnamed_addr constant [41 x i8] c"    \E2\95\91  \D9\86\D8\B3\D8\A8\D8\A9 \D8\A7\D9\84\D8\A5\D9\86\D8\AC\D8\A7\D8\B2:       \00", align 1
@str.const.141 = private unnamed_addr constant [2 x i8] c"%\00", align 1
@str.const.145 = private unnamed_addr constant [42 x i8] c"    \E2\95\91  \D8\A7\D9\84\D9\85\D8\AF\D8\A9 \D8\A7\D9\84\D9\85\D9\82\D8\AF\D8\B1\D8\A9:      \00", align 1
@str.const.152 = private unnamed_addr constant [42 x i8] c"    \E2\95\91  \D8\A7\D9\84\D9\85\D8\AF\D8\A9 \D8\A7\D9\84\D9\81\D8\B9\D9\84\D9\8A\D8\A9:      \00", align 1
@str.const.159 = private unnamed_addr constant [113 x i8] c"    \E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.163 = private unnamed_addr constant [121 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.167 = private unnamed_addr constant [63 x i8] c"\E2\95\91  \F0\9F\8F\97\EF\B8\8F Phase 17: \D9\86\D8\B8\D8\A7\D9\85 \D8\A5\D8\AF\D8\A7\D8\B1\D8\A9 \D9\85\D8\B4\D8\B1\D9\88\D8\B9      \E2\95\91\00", align 1
@str.const.171 = private unnamed_addr constant [121 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.175 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.179 = private unnamed_addr constant [40 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D9\81\D8\B1\D9\8A\D9\82 \D8\A7\D9\84\D8\B9\D9\85\D9\84 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.183 = private unnamed_addr constant [9 x i8] c"\D8\B3\D8\A7\D8\B1\D8\A9\00", align 1
@str.const.184 = private unnamed_addr constant [22 x i8] c"\D9\85\D8\AF\D9\8A\D8\B1\D8\A9 \D9\85\D8\B4\D8\B1\D9\88\D8\B9\00", align 1
@str.const.185 = private unnamed_addr constant [9 x i8] c"\D8\A3\D8\AD\D9\85\D8\AF\00", align 1
@str.const.186 = private unnamed_addr constant [16 x i8] c"\D9\85\D8\B7\D9\88\D8\B1 \D8\A3\D9\88\D9\84\00", align 1
@str.const.187 = private unnamed_addr constant [11 x i8] c"\D9\81\D8\A7\D8\B7\D9\85\D8\A9\00", align 1
@str.const.188 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\B5\D9\85\D9\85\D8\A9\00", align 1
@str.const.189 = private unnamed_addr constant [9 x i8] c"\D8\AE\D8\A7\D9\84\D8\AF\00", align 1
@str.const.190 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\AE\D8\AA\D8\A8\D8\B1\00", align 1
@str.const.191 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.195 = private unnamed_addr constant [44 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\A5\D9\86\D8\B4\D8\A7\D8\A1 \D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.199 = private unnamed_addr constant [39 x i8] c"\D8\AA\D8\B5\D9\85\D9\8A\D9\85 \D9\82\D8\A7\D8\B9\D8\AF\D8\A9 \D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA\00", align 1
@str.const.200 = private unnamed_addr constant [39 x i8] c"\D8\AA\D8\B7\D9\88\D9\8A\D8\B1 \D9\88\D8\A7\D8\AC\D9\87\D8\A9 \D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\00", align 1
@str.const.201 = private unnamed_addr constant [26 x i8] c"\D9\83\D8\AA\D8\A7\D8\A8\D8\A9 \D8\A7\D9\84\D8\AA\D9\88\D8\AB\D9\8A\D9\82\00", align 1
@str.const.202 = private unnamed_addr constant [26 x i8] c"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\A7\D9\84\D8\A3\D9\85\D8\A7\D9\86\00", align 1
@str.const.203 = private unnamed_addr constant [24 x i8] c"\D8\AA\D8\AD\D8\B3\D9\8A\D9\86 \D8\A7\D9\84\D8\A3\D8\AF\D8\A7\D8\A1\00", align 1
@str.const.204 = private unnamed_addr constant [26 x i8] c"\D8\A5\D8\B5\D9\84\D8\A7\D8\AD \D8\A7\D9\84\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1\00", align 1
@str.const.205 = private unnamed_addr constant [24 x i8] c"\D9\85\D8\B1\D8\A7\D8\AC\D8\B9\D8\A9 \D8\A7\D9\84\D9\83\D9\88\D8\AF\00", align 1
@str.const.206 = private unnamed_addr constant [22 x i8] c"\D9\86\D8\B4\D8\B1 \D8\A7\D9\84\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB\00", align 1
@str.const.207 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.211 = private unnamed_addr constant [44 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\AA\D8\B9\D9\8A\D9\8A\D9\86 \D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.223 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.227 = private unnamed_addr constant [44 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\A5\D9\86\D8\AC\D8\A7\D8\B2 \D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.231 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.235 = private unnamed_addr constant [42 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\AD\D8\A7\D9\84\D8\A9 \D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.239 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.243 = private unnamed_addr constant [50 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\A8\D8\B7\D8\A7\D9\82\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D9\88\D8\B8\D9\81\D9\8A\D9\86 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.247 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.251 = private unnamed_addr constant [44 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D9\85\D9\84\D8\AE\D8\B5 \D8\A7\D9\84\D9\85\D8\B4\D8\B1\D9\88\D8\B9 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.255 = private unnamed_addr constant [26 x i8] c"\D8\A8\D9\88\D8\A7\D8\A8\D8\A9 \D8\A7\D9\84\D9\85\D8\B9\D8\B1\D9\81\D8\A9\00", align 1
@str.const.256 = private unnamed_addr constant [46 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1\D8\A7\D8\AA \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.260 = private unnamed_addr constant [64 x i8] c"    \E2\9C\85 \D8\AA\D8\B5\D9\85\D9\8A\D9\85 \D9\82\D8\A7\D8\B9\D8\AF\D8\A9 \D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA: \D9\81\D9\8A \D8\A7\D9\84\D9\88\D9\82\D8\AA\00", align 1
@str.const.264 = private unnamed_addr constant [61 x i8] c"    \E2\9D\8C \D8\AA\D8\B5\D9\85\D9\8A\D9\85 \D9\82\D8\A7\D8\B9\D8\AF\D8\A9 \D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA: \D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9\00", align 1
@str.const.268 = private unnamed_addr constant [48 x i8] c"    \E2\9D\8C \D9\83\D8\AA\D8\A7\D8\A8\D8\A9 \D8\A7\D9\84\D8\AA\D9\88\D8\AB\D9\8A\D9\82: \D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9\00", align 1
@str.const.272 = private unnamed_addr constant [51 x i8] c"    \E2\9C\85 \D9\83\D8\AA\D8\A7\D8\A8\D8\A9 \D8\A7\D9\84\D8\AA\D9\88\D8\AB\D9\8A\D9\82: \D9\81\D9\8A \D8\A7\D9\84\D9\88\D9\82\D8\AA\00", align 1
@str.const.276 = private unnamed_addr constant [46 x i8] c"    \E2\9D\8C \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\A7\D9\84\D8\A3\D9\85\D8\A7\D9\86: \D9\85\D8\AA\D8\A3\D8\AE\D8\B1\00", align 1
@str.const.280 = private unnamed_addr constant [51 x i8] c"    \E2\9C\85 \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\A7\D9\84\D8\A3\D9\85\D8\A7\D9\86: \D9\81\D9\8A \D8\A7\D9\84\D9\88\D9\82\D8\AA\00", align 1
@str.const.284 = private unnamed_addr constant [46 x i8] c"    \E2\9D\8C \D8\A5\D8\B5\D9\84\D8\A7\D8\AD \D8\A7\D9\84\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1: \D9\85\D8\AA\D8\A3\D8\AE\D8\B1\00", align 1
@str.const.288 = private unnamed_addr constant [51 x i8] c"    \E2\9C\85 \D8\A5\D8\B5\D9\84\D8\A7\D8\AD \D8\A7\D9\84\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1: \D9\81\D9\8A \D8\A7\D9\84\D9\88\D9\82\D8\AA\00", align 1
@str.const.292 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.296 = private unnamed_addr constant [68 x i8] c"\E2\9C\85 \D8\AA\D9\85 \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D9\86\D8\B8\D8\A7\D9\85 \D8\A5\D8\AF\D8\A7\D8\B1\D8\A9 \D8\A7\D9\84\D9\85\D8\B4\D8\B1\D9\88\D8\B9 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD\00", align 1

define void @"\D9\85\D9\87\D9\85\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", i64 %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9", i64 %"\D9\85\D8\AF\D8\A9") {
entry:
  %"\D9\85\D8\AF\D8\A94" = alloca i64, align 8
  store i64 %"\D9\85\D8\AF\D8\A9", ptr %"\D9\85\D8\AF\D8\A94", align 8
  %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A93" = alloca i64, align 8
  store i64 %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9", ptr %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A93", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\862" = alloca ptr, align 8
  store ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\862", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca5 = alloca ptr, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %alloca8 = alloca ptr, align 8
  %alloca9 = alloca i64, align 8
  store ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\862", ptr %alloca5, align 8
  %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load" = load i64, ptr %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A93", align 8
  store i64 %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load", ptr %alloca7, align 8
  %"%\D9\85\D8\AF\D8\A9.load" = load i64, ptr %"\D9\85\D8\AF\D8\A94", align 8
  store i64 %"%\D9\85\D8\AF\D8\A9.load", ptr %alloca9, align 8
  store i64 0, ptr %alloca, align 8
  store i64 0, ptr %alloca6, align 8
  store ptr @str.const, ptr %alloca8, align 8
  ret void
}

define void @"\D9\85\D9\87\D9\85\D8\A9.\D8\AA\D8\B9\D9\8A\D9\8A\D9\86"(i64 %self, ptr %"\D8\A7\D8\B3\D9\85") {
entry:
  %"\D8\A7\D8\B3\D9\852" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca i64, align 8
  store ptr %"\D8\A7\D8\B3\D9\852", ptr %alloca6, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca3)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\A7\D8\B3\D9\852")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  ret void
}

define void @"\D9\85\D9\87\D9\85\D8\A9.\D8\A5\D9\83\D9\85\D8\A7\D9\84"(i64 %self, i64 %"\D9\85\D8\AF\D8\A9_\D9\81\D8\B9\D9\84\D9\8A\D8\A9") {
entry:
  %"\D9\85\D8\AF\D8\A9_\D9\81\D8\B9\D9\84\D9\8A\D8\A92" = alloca i64, align 8
  store i64 %"\D9\85\D8\AF\D8\A9_\D9\81\D8\B9\D9\84\D9\8A\D8\A9", ptr %"\D9\85\D8\AF\D8\A9_\D9\81\D8\B9\D9\84\D9\8A\D8\A92", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca i64, align 8
  store i64 1, ptr %alloca4, align 8
  %"%\D9\85\D8\AF\D8\A9_\D9\81\D8\B9\D9\84\D9\8A\D8\A9.load" = load i64, ptr %"\D9\85\D8\AF\D8\A9_\D9\81\D8\B9\D9\84\D9\8A\D8\A92", align 8
  store i64 %"%\D9\85\D8\AF\D8\A9_\D9\81\D8\B9\D9\84\D9\8A\D8\A9.load", ptr %alloca, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.8)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca3)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  ret void
}

define i64 @"\D9\85\D9\87\D9\85\D8\A9.\D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca ptr, align 8
  %alloca6 = alloca i64, align 8
  %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\81\D8\B9\D9\84\D9\8A\D8\A9.load" = load i64, ptr %alloca, align 8
  %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D9\82\D8\AF\D8\B1\D8\A9.load" = load i64, ptr %alloca6, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\81\D8\B9\D9\84\D9\8A\D8\A9.load", %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D9\82\D8\AF\D8\B1\D8\A9.load"
  br i1 %cmpgttmp, label %then_0, label %merge_1

then_0:                                           ; preds = %entry
  ret i64 1

merge_1:                                          ; preds = %entry
  ret i64 0
}

define ptr @"\D9\85\D9\87\D9\85\D8\A9.\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca ptr, align 8
  %alloca6 = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load" = load i64, ptr %alloca4, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load", 1
  br i1 %cmpeqtmp, label %then_2, label %merge_3

then_2:                                           ; preds = %entry
  ret ptr @str.ret

merge_3:                                          ; preds = %entry
  %"%\D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load7" = load i64, ptr %alloca4, align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load7", 2
  br i1 %cmpeqtmp8, label %then_4, label %merge_5

then_4:                                           ; preds = %merge_3
  ret ptr @str.ret.13

merge_5:                                          ; preds = %merge_3
  ret ptr @str.ret.14
}

define void @"\D9\85\D9\87\D9\85\D8\A9.\D8\B9\D8\B1\D8\B6"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca ptr, align 8
  %alloca6 = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.15)
  %"%\D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load" = load i64, ptr %alloca4, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load", 1
  br i1 %cmpeqtmp, label %then_6, label %else_8

then_6:                                           ; preds = %entry
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.17)
  br label %merge_7

else_8:                                           ; preds = %entry
  %"%\D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load7" = load i64, ptr %alloca4, align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load7", 2
  br i1 %cmpeqtmp8, label %then_9, label %else_11

merge_7:                                          ; preds = %merge_10, %then_6
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.19)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca2)
  %"%\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load" = load i64, ptr %alloca3, align 8
  %cmpeqtmp9 = icmp eq i64 %"%\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load", 1
  br i1 %cmpeqtmp9, label %then_12, label %else_14

then_9:                                           ; preds = %else_8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.22)
  br label %merge_10

else_11:                                          ; preds = %else_8
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.24)
  br label %merge_10

merge_10:                                         ; preds = %else_11, %then_9
  br label %merge_7

then_12:                                          ; preds = %merge_7
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.26)
  br label %merge_13

else_14:                                          ; preds = %merge_7
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.28)
  br label %merge_13

merge_13:                                         ; preds = %else_14, %then_12
  %strcmp.ret = call i32 @strcmp(ptr %alloca5, ptr @str.const.30)
  %streq = icmp eq i32 %strcmp.ret, 0
  br i1 %streq, label %then_15, label %merge_16

then_15:                                          ; preds = %merge_13
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.31)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca5)
  br label %merge_16

merge_16:                                         ; preds = %then_15, %merge_13
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.34)
  %"%\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load10" = load i64, ptr %alloca3, align 8
  %cmpeqtmp11 = icmp eq i64 %"%\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load10", 1
  br i1 %cmpeqtmp11, label %then_17, label %else_19

then_17:                                          ; preds = %merge_16
  %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\81\D8\B9\D9\84\D9\8A\D8\A9.load" = load i64, ptr %alloca, align 8
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\81\D8\B9\D9\84\D9\8A\D8\A9.load")
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.36)
  %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D9\82\D8\AF\D8\B1\D8\A9.load" = load i64, ptr %alloca6, align 8
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D9\82\D8\AF\D8\B1\D8\A9.load")
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.39)
  br label %merge_18

else_19:                                          ; preds = %merge_16
  %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D9\82\D8\AF\D8\B1\D8\A9.load12" = load i64, ptr %alloca6, align 8
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D9\82\D8\AF\D8\B1\D8\A9.load12")
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.42)
  br label %merge_18

merge_18:                                         ; preds = %else_19, %then_17
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.44)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  ret void
}

define void @"\D9\85\D9\88\D8\B8\D9\81.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D9\85\D9\86\D8\B5\D8\A8") {
entry:
  %"\D9\85\D9\86\D8\B5\D8\A83" = alloca ptr, align 8
  store ptr %"\D9\85\D9\86\D8\B5\D8\A8", ptr %"\D9\85\D9\86\D8\B5\D8\A83", align 8
  %"\D8\A7\D8\B3\D9\852" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %alloca8 = alloca i64, align 8
  store ptr %"\D8\A7\D8\B3\D9\852", ptr %alloca4, align 8
  store ptr %"\D9\85\D9\86\D8\B5\D8\A83", ptr %alloca, align 8
  store i64 0, ptr %alloca5, align 8
  store i64 0, ptr %alloca7, align 8
  store i64 0, ptr %alloca8, align 8
  store i64 50, ptr %alloca6, align 8
  ret void
}

define void @"\D9\85\D9\88\D8\B8\D9\81.\D8\A5\D9\86\D8\AC\D8\A7\D8\B2_\D9\85\D9\87\D9\85\D8\A9"(i64 %self, i64 %"\D8\B3\D8\A7\D8\B9\D8\A7\D8\AA", i64 %"\D9\81\D9\8A_\D8\A7\D9\84\D9\88\D9\82\D8\AA") {
entry:
  %"\D9\81\D9\8A_\D8\A7\D9\84\D9\88\D9\82\D8\AA3" = alloca i64, align 8
  store i64 %"\D9\81\D9\8A_\D8\A7\D9\84\D9\88\D9\82\D8\AA", ptr %"\D9\81\D9\8A_\D8\A7\D9\84\D9\88\D9\82\D8\AA3", align 8
  %"\D8\B3\D8\A7\D8\B9\D8\A7\D8\AA2" = alloca i64, align 8
  store i64 %"\D8\B3\D8\A7\D8\B9\D8\A7\D8\AA", ptr %"\D8\B3\D8\A7\D8\B9\D8\A7\D8\AA2", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %alloca8 = alloca i64, align 8
  %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load" = load i64, ptr %alloca5, align 8
  %addtmp = add i64 %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load", 1
  store i64 %addtmp, ptr %alloca5, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B3\D8\A7\D8\B9\D8\A7\D8\AA.load" = load i64, ptr %alloca8, align 8
  %"%\D8\B3\D8\A7\D8\B9\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B3\D8\A7\D8\B9\D8\A7\D8\AA2", align 8
  %addtmp9 = add i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B3\D8\A7\D8\B9\D8\A7\D8\AA.load", %"%\D8\B3\D8\A7\D8\B9\D8\A7\D8\AA.load"
  store i64 %addtmp9, ptr %alloca8, align 8
  %"%\D9\81\D9\8A_\D8\A7\D9\84\D9\88\D9\82\D8\AA.load" = load i64, ptr %"\D9\81\D9\8A_\D8\A7\D9\84\D9\88\D9\82\D8\AA3", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\81\D9\8A_\D8\A7\D9\84\D9\88\D9\82\D8\AA.load", 1
  br i1 %cmpeqtmp, label %then_20, label %else_22

then_20:                                          ; preds = %entry
  %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load" = load i64, ptr %alloca6, align 8
  %addtmp10 = add i64 %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load", 5
  store i64 %addtmp10, ptr %alloca6, align 8
  %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load11" = load i64, ptr %alloca6, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load11", 100
  br i1 %cmpgttmp, label %then_23, label %merge_24

else_22:                                          ; preds = %entry
  %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9.load" = load i64, ptr %alloca7, align 8
  %addtmp12 = add i64 %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9.load", 1
  store i64 %addtmp12, ptr %alloca7, align 8
  %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load13" = load i64, ptr %alloca6, align 8
  %subtmp = sub i64 %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load13", 3
  store i64 %subtmp, ptr %alloca6, align 8
  %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load14" = load i64, ptr %alloca6, align 8
  %cmplttmp = icmp slt i64 %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load14", 0
  br i1 %cmplttmp, label %then_25, label %merge_26

merge_21:                                         ; preds = %merge_26, %merge_24
  ret void

then_23:                                          ; preds = %then_20
  store i64 100, ptr %alloca6, align 8
  br label %merge_24

merge_24:                                         ; preds = %then_23, %then_20
  br label %merge_21

then_25:                                          ; preds = %else_22
  store i64 0, ptr %alloca6, align 8
  br label %merge_26

merge_26:                                         ; preds = %then_25, %else_22
  br label %merge_21
}

define ptr @"\D9\85\D9\88\D8\B8\D9\81.\D9\85\D8\B3\D8\AA\D9\88\D9\89_\D8\A7\D9\84\D8\A3\D8\AF\D8\A7\D8\A1"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load" = load i64, ptr %alloca4, align 8
  %cmpgetmp = icmp sge i64 %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load", 80
  br i1 %cmpgetmp, label %then_27, label %merge_28

then_27:                                          ; preds = %entry
  ret ptr @str.ret.48

merge_28:                                         ; preds = %entry
  %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load7" = load i64, ptr %alloca4, align 8
  %cmpgetmp8 = icmp sge i64 %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load7", 60
  br i1 %cmpgetmp8, label %then_29, label %merge_30

then_29:                                          ; preds = %merge_28
  ret ptr @str.ret.49

merge_30:                                         ; preds = %merge_28
  %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load9" = load i64, ptr %alloca4, align 8
  %cmpgetmp10 = icmp sge i64 %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load9", 40
  br i1 %cmpgetmp10, label %then_31, label %merge_32

then_31:                                          ; preds = %merge_30
  ret ptr @str.ret.50

merge_32:                                         ; preds = %merge_30
  ret ptr @str.ret.51
}

define i64 @"\D9\85\D9\88\D8\B8\D9\81.\D9\86\D8\B3\D8\A8\D8\A9_\D8\A7\D9\84\D8\A5\D9\86\D8\AC\D8\A7\D8\B2_\D9\81\D9\8A_\D8\A7\D9\84\D9\88\D9\82\D8\AA"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load" = load i64, ptr %alloca3, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_33, label %merge_34

then_33:                                          ; preds = %entry
  ret i64 0

merge_34:                                         ; preds = %entry
  %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load7" = load i64, ptr %alloca3, align 8
  %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9.load" = load i64, ptr %alloca5, align 8
  %subtmp = sub i64 %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load7", %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9.load"
  %multmp = mul i64 %subtmp, 100
  %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load8" = load i64, ptr %alloca3, align 8
  %divtmp = sdiv i64 %multmp, %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load8"
  ret i64 %divtmp
}

define void @"\D9\85\D9\88\D8\B8\D9\81.\D8\B9\D8\B1\D8\B6_\D8\A8\D8\B7\D8\A7\D9\82\D8\A9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.52)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.56)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.59)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.64)
  %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load" = load i64, ptr %alloca3, align 8
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load")
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.67)
  %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9.load" = load i64, ptr %alloca5, align 8
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9.load")
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.70)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.74)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B3\D8\A7\D8\B9\D8\A7\D8\AA.load" = load i64, ptr %alloca6, align 8
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B3\D8\A7\D8\B9\D8\A7\D8\AA.load")
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.79)
  %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load" = load i64, ptr %alloca4, align 8
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load")
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.82)
  %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load7" = load i64, ptr %alloca4, align 8
  %cmpgetmp = icmp sge i64 %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load7", 80
  br i1 %cmpgetmp, label %then_35, label %else_37

then_35:                                          ; preds = %entry
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.84)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  br label %merge_36

else_37:                                          ; preds = %entry
  %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load8" = load i64, ptr %alloca4, align 8
  %cmpgetmp9 = icmp sge i64 %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load8", 60
  br i1 %cmpgetmp9, label %then_38, label %else_40

merge_36:                                         ; preds = %merge_39, %then_35
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.88)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  ret void

then_38:                                          ; preds = %else_37
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.92)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  br label %merge_39

else_40:                                          ; preds = %else_37
  %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load10" = load i64, ptr %alloca4, align 8
  %cmpgetmp11 = icmp sge i64 %"%\D8\A7\D9\84\D8\AA\D9\82\D9\8A\D9\8A\D9\85.load10", 40
  br i1 %cmpgetmp11, label %then_41, label %else_43

merge_39:                                         ; preds = %merge_42, %then_38
  br label %merge_36

then_41:                                          ; preds = %else_40
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.96)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  br label %merge_42

else_43:                                          ; preds = %else_40
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.100)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  br label %merge_42

merge_42:                                         ; preds = %else_43, %then_41
  br label %merge_39
}

define void @"\D9\85\D9\84\D8\AE\D8\B5_\D9\85\D8\B4\D8\B1\D9\88\D8\B9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, ptr %"\D8\A7\D8\B3\D9\85") {
entry:
  %"\D8\A7\D8\B3\D9\852" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %alloca8 = alloca i64, align 8
  store ptr %"\D8\A7\D8\B3\D9\852", ptr %alloca, align 8
  store i64 0, ptr %alloca4, align 8
  store i64 0, ptr %alloca5, align 8
  store i64 0, ptr %alloca6, align 8
  store i64 0, ptr %alloca3, align 8
  store i64 0, ptr %alloca8, align 8
  store i64 0, ptr %alloca7, align 8
  ret void
}

define void @"\D9\85\D9\84\D8\AE\D8\B5_\D9\85\D8\B4\D8\B1\D9\88\D8\B9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D9\85\D9\87\D9\85\D8\A9"(i64 %self, i64 %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9", i64 %"\D9\85\D8\AF\D8\A9_\D9\85\D9\82\D8\AF\D8\B1\D8\A9", i64 %"\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9", i64 %"\D9\85\D8\AF\D8\A9_\D9\81\D8\B9\D9\84\D9\8A\D8\A9") {
entry:
  %"\D9\85\D8\AF\D8\A9_\D9\81\D8\B9\D9\84\D9\8A\D8\A95" = alloca i64, align 8
  store i64 %"\D9\85\D8\AF\D8\A9_\D9\81\D8\B9\D9\84\D9\8A\D8\A9", ptr %"\D9\85\D8\AF\D8\A9_\D9\81\D8\B9\D9\84\D9\8A\D8\A95", align 8
  %"\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A94" = alloca i64, align 8
  store i64 %"\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9", ptr %"\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A94", align 8
  %"\D9\85\D8\AF\D8\A9_\D9\85\D9\82\D8\AF\D8\B1\D8\A93" = alloca i64, align 8
  store i64 %"\D9\85\D8\AF\D8\A9_\D9\85\D9\82\D8\AF\D8\B1\D8\A9", ptr %"\D9\85\D8\AF\D8\A9_\D9\85\D9\82\D8\AF\D8\B1\D8\A93", align 8
  %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A92" = alloca i64, align 8
  store i64 %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9", ptr %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A92", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %alloca8 = alloca i64, align 8
  %alloca9 = alloca i64, align 8
  %alloca10 = alloca i64, align 8
  %alloca11 = alloca i64, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85.load" = load i64, ptr %alloca7, align 8
  %addtmp = add i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85.load", 1
  store i64 %addtmp, ptr %alloca7, align 8
  %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\8A\D8\A9_\D8\A7\D9\84\D9\85\D9\82\D8\AF\D8\B1\D8\A9.load" = load i64, ptr %alloca11, align 8
  %"%\D9\85\D8\AF\D8\A9_\D9\85\D9\82\D8\AF\D8\B1\D8\A9.load" = load i64, ptr %"\D9\85\D8\AF\D8\A9_\D9\85\D9\82\D8\AF\D8\B1\D8\A93", align 8
  %addtmp12 = add i64 %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\8A\D8\A9_\D8\A7\D9\84\D9\85\D9\82\D8\AF\D8\B1\D8\A9.load", %"%\D9\85\D8\AF\D8\A9_\D9\85\D9\82\D8\AF\D8\B1\D8\A9.load"
  store i64 %addtmp12, ptr %alloca11, align 8
  %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load" = load i64, ptr %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A92", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load", 1
  br i1 %cmpeqtmp, label %then_44, label %merge_45

then_44:                                          ; preds = %entry
  %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D8\B9\D8\A7\D8\AC\D9\84\D8\A9.load" = load i64, ptr %alloca6, align 8
  %addtmp13 = add i64 %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D8\B9\D8\A7\D8\AC\D9\84\D8\A9.load", 1
  store i64 %addtmp13, ptr %alloca6, align 8
  br label %merge_45

merge_45:                                         ; preds = %then_44, %entry
  %"%\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load" = load i64, ptr %"\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A94", align 8
  %cmpeqtmp14 = icmp eq i64 %"%\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load", 1
  br i1 %cmpeqtmp14, label %then_46, label %merge_47

then_46:                                          ; preds = %merge_45
  %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load" = load i64, ptr %alloca8, align 8
  %addtmp15 = add i64 %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load", 1
  store i64 %addtmp15, ptr %alloca8, align 8
  %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\8A\D8\A9_\D8\A7\D9\84\D9\81\D8\B9\D9\84\D9\8A\D8\A9.load" = load i64, ptr %alloca10, align 8
  %"%\D9\85\D8\AF\D8\A9_\D9\81\D8\B9\D9\84\D9\8A\D8\A9.load" = load i64, ptr %"\D9\85\D8\AF\D8\A9_\D9\81\D8\B9\D9\84\D9\8A\D8\A95", align 8
  %addtmp16 = add i64 %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\8A\D8\A9_\D8\A7\D9\84\D9\81\D8\B9\D9\84\D9\8A\D8\A9.load", %"%\D9\85\D8\AF\D8\A9_\D9\81\D8\B9\D9\84\D9\8A\D8\A9.load"
  store i64 %addtmp16, ptr %alloca10, align 8
  %"%\D9\85\D8\AF\D8\A9_\D9\81\D8\B9\D9\84\D9\8A\D8\A9.load17" = load i64, ptr %"\D9\85\D8\AF\D8\A9_\D9\81\D8\B9\D9\84\D9\8A\D8\A95", align 8
  %"%\D9\85\D8\AF\D8\A9_\D9\85\D9\82\D8\AF\D8\B1\D8\A9.load18" = load i64, ptr %"\D9\85\D8\AF\D8\A9_\D9\85\D9\82\D8\AF\D8\B1\D8\A93", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\85\D8\AF\D8\A9_\D9\81\D8\B9\D9\84\D9\8A\D8\A9.load17", %"%\D9\85\D8\AF\D8\A9_\D9\85\D9\82\D8\AF\D8\B1\D8\A9.load18"
  br i1 %cmpgttmp, label %then_48, label %merge_49

merge_47:                                         ; preds = %merge_49, %merge_45
  ret void

then_48:                                          ; preds = %then_46
  %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9.load" = load i64, ptr %alloca9, align 8
  %addtmp19 = add i64 %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9.load", 1
  store i64 %addtmp19, ptr %alloca9, align 8
  br label %merge_49

merge_49:                                         ; preds = %then_48, %then_46
  br label %merge_47
}

define i64 @"\D9\85\D9\84\D8\AE\D8\B5_\D9\85\D8\B4\D8\B1\D9\88\D8\B9.\D9\86\D8\B3\D8\A8\D8\A9_\D8\A7\D9\84\D8\A5\D9\86\D8\AC\D8\A7\D8\B2"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85.load" = load i64, ptr %alloca3, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85.load", 0
  br i1 %cmpeqtmp, label %then_50, label %merge_51

then_50:                                          ; preds = %entry
  ret i64 0

merge_51:                                         ; preds = %entry
  %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load" = load i64, ptr %alloca4, align 8
  %multmp = mul i64 %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load", 100
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85.load8" = load i64, ptr %alloca3, align 8
  %divtmp = sdiv i64 %multmp, %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85.load8"
  ret i64 %divtmp
}

define void @"\D9\85\D9\84\D8\AE\D8\B5_\D9\85\D8\B4\D8\B1\D9\88\D8\B9.\D8\B9\D8\B1\D8\B6"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca ptr, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.104)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.108)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.113)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.117)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85.load" = load i64, ptr %alloca3, align 8
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85.load")
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.122)
  %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load" = load i64, ptr %alloca4, align 8
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load")
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.127)
  %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9.load" = load i64, ptr %alloca5, align 8
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9.load")
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.132)
  %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D8\B9\D8\A7\D8\AC\D9\84\D8\A9.load" = load i64, ptr %alloca2, align 8
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D8\B9\D8\A7\D8\AC\D9\84\D8\A9.load")
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.137)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85.load8" = load i64, ptr %alloca3, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85.load8", 0
  br i1 %cmpgttmp, label %then_52, label %else_54

then_52:                                          ; preds = %entry
  %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load9" = load i64, ptr %alloca4, align 8
  %multmp = mul i64 %"%\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D9\85\D9\84\D8\A9.load9", 100
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85.load10" = load i64, ptr %alloca3, align 8
  %divtmp = sdiv i64 %multmp, %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85.load10"
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %divtmp)
  br label %merge_53

else_54:                                          ; preds = %entry
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 0)
  br label %merge_53

merge_53:                                         ; preds = %else_54, %then_52
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.141)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.145)
  %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\8A\D8\A9_\D8\A7\D9\84\D9\85\D9\82\D8\AF\D8\B1\D8\A9.load" = load i64, ptr %alloca7, align 8
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\8A\D8\A9_\D8\A7\D9\84\D9\85\D9\82\D8\AF\D8\B1\D8\A9.load")
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.39)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.152)
  %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\8A\D8\A9_\D8\A7\D9\84\D9\81\D8\B9\D9\84\D9\8A\D8\A9.load" = load i64, ptr %alloca6, align 8
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\8A\D8\A9_\D8\A7\D9\84\D9\81\D8\B9\D9\84\D9\8A\D8\A9.load")
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.39)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.159)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  ret void
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.163)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.167)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.171)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.175)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.179)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %alloca = alloca i64, align 8
  %"%152.load" = load i64, ptr %alloca, align 8
  call void @"\D9\85\D9\88\D8\B8\D9\81.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%152.load", ptr @str.const.183, ptr @str.const.184)
  %alloca1 = alloca i64, align 8
  %"%152.load2" = load i64, ptr %alloca, align 8
  store i64 %"%152.load2", ptr %alloca1, align 8
  %alloca3 = alloca i64, align 8
  %"%156.load" = load i64, ptr %alloca3, align 8
  call void @"\D9\85\D9\88\D8\B8\D9\81.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%156.load", ptr @str.const.185, ptr @str.const.186)
  %alloca4 = alloca i64, align 8
  %"%156.load5" = load i64, ptr %alloca3, align 8
  store i64 %"%156.load5", ptr %alloca4, align 8
  %alloca6 = alloca i64, align 8
  %"%160.load" = load i64, ptr %alloca6, align 8
  call void @"\D9\85\D9\88\D8\B8\D9\81.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%160.load", ptr @str.const.187, ptr @str.const.188)
  %alloca7 = alloca i64, align 8
  %"%160.load8" = load i64, ptr %alloca6, align 8
  store i64 %"%160.load8", ptr %alloca7, align 8
  %alloca9 = alloca i64, align 8
  %"%164.load" = load i64, ptr %alloca9, align 8
  call void @"\D9\85\D9\88\D8\B8\D9\81.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%164.load", ptr @str.const.189, ptr @str.const.190)
  %alloca10 = alloca i64, align 8
  %"%164.load11" = load i64, ptr %alloca9, align 8
  store i64 %"%164.load11", ptr %alloca10, align 8
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.191)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.195)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %alloca12 = alloca i64, align 8
  %"%170.load" = load i64, ptr %alloca12, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%170.load", ptr @str.const.199, i64 1, i64 5)
  %alloca13 = alloca i64, align 8
  %"%170.load14" = load i64, ptr %alloca12, align 8
  store i64 %"%170.load14", ptr %alloca13, align 8
  %alloca15 = alloca i64, align 8
  %"%175.load" = load i64, ptr %alloca15, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%175.load", ptr @str.const.200, i64 2, i64 10)
  %alloca16 = alloca i64, align 8
  %"%175.load17" = load i64, ptr %alloca15, align 8
  store i64 %"%175.load17", ptr %alloca16, align 8
  %alloca18 = alloca i64, align 8
  %"%180.load" = load i64, ptr %alloca18, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%180.load", ptr @str.const.201, i64 3, i64 3)
  %alloca19 = alloca i64, align 8
  %"%180.load20" = load i64, ptr %alloca18, align 8
  store i64 %"%180.load20", ptr %alloca19, align 8
  %alloca21 = alloca i64, align 8
  %"%185.load" = load i64, ptr %alloca21, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%185.load", ptr @str.const.202, i64 1, i64 7)
  %alloca22 = alloca i64, align 8
  %"%185.load23" = load i64, ptr %alloca21, align 8
  store i64 %"%185.load23", ptr %alloca22, align 8
  %alloca24 = alloca i64, align 8
  %"%190.load" = load i64, ptr %alloca24, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%190.load", ptr @str.const.203, i64 2, i64 4)
  %alloca25 = alloca i64, align 8
  %"%190.load26" = load i64, ptr %alloca24, align 8
  store i64 %"%190.load26", ptr %alloca25, align 8
  %alloca27 = alloca i64, align 8
  %"%195.load" = load i64, ptr %alloca27, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%195.load", ptr @str.const.204, i64 1, i64 2)
  %alloca28 = alloca i64, align 8
  %"%195.load29" = load i64, ptr %alloca27, align 8
  store i64 %"%195.load29", ptr %alloca28, align 8
  %alloca30 = alloca i64, align 8
  %"%200.load" = load i64, ptr %alloca30, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%200.load", ptr @str.const.205, i64 3, i64 3)
  %alloca31 = alloca i64, align 8
  %"%200.load32" = load i64, ptr %alloca30, align 8
  store i64 %"%200.load32", ptr %alloca31, align 8
  %alloca33 = alloca i64, align 8
  %"%205.load" = load i64, ptr %alloca33, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%205.load", ptr @str.const.206, i64 2, i64 1)
  %alloca34 = alloca i64, align 8
  %"%205.load35" = load i64, ptr %alloca33, align 8
  store i64 %"%205.load35", ptr %alloca34, align 8
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.207)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.211)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %"%\D8\AA1.load" = load i64, ptr %alloca13, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\AA\D8\B9\D9\8A\D9\8A\D9\86"(i64 %"%\D8\AA1.load", ptr @str.const.185)
  %"%\D8\AA2.load" = load i64, ptr %alloca16, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\AA\D8\B9\D9\8A\D9\8A\D9\86"(i64 %"%\D8\AA2.load", ptr @str.const.187)
  %"%\D8\AA3.load" = load i64, ptr %alloca19, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\AA\D8\B9\D9\8A\D9\8A\D9\86"(i64 %"%\D8\AA3.load", ptr @str.const.183)
  %"%\D8\AA4.load" = load i64, ptr %alloca22, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\AA\D8\B9\D9\8A\D9\8A\D9\86"(i64 %"%\D8\AA4.load", ptr @str.const.189)
  %"%\D8\AA5.load" = load i64, ptr %alloca25, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\AA\D8\B9\D9\8A\D9\8A\D9\86"(i64 %"%\D8\AA5.load", ptr @str.const.185)
  %"%\D8\AA6.load" = load i64, ptr %alloca28, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\AA\D8\B9\D9\8A\D9\8A\D9\86"(i64 %"%\D8\AA6.load", ptr @str.const.185)
  %"%\D8\AA7.load" = load i64, ptr %alloca31, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\AA\D8\B9\D9\8A\D9\8A\D9\86"(i64 %"%\D8\AA7.load", ptr @str.const.183)
  %"%\D8\AA8.load" = load i64, ptr %alloca34, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\AA\D8\B9\D9\8A\D9\8A\D9\86"(i64 %"%\D8\AA8.load", ptr @str.const.189)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.223)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.227)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %"%\D8\AA1.load36" = load i64, ptr %alloca13, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\A5\D9\83\D9\85\D8\A7\D9\84"(i64 %"%\D8\AA1.load36", i64 4)
  %"%\D8\AA3.load37" = load i64, ptr %alloca19, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\A5\D9\83\D9\85\D8\A7\D9\84"(i64 %"%\D8\AA3.load37", i64 5)
  %"%\D8\AA4.load38" = load i64, ptr %alloca22, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\A5\D9\83\D9\85\D8\A7\D9\84"(i64 %"%\D8\AA4.load38", i64 8)
  %"%\D8\AA5.load39" = load i64, ptr %alloca25, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\A5\D9\83\D9\85\D8\A7\D9\84"(i64 %"%\D8\AA5.load39", i64 3)
  %"%\D8\AA6.load40" = load i64, ptr %alloca28, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\A5\D9\83\D9\85\D8\A7\D9\84"(i64 %"%\D8\AA6.load40", i64 3)
  %"%\D9\851.load" = load i64, ptr %alloca1, align 8
  call void @"\D9\85\D9\88\D8\B8\D9\81.\D8\A5\D9\86\D8\AC\D8\A7\D8\B2_\D9\85\D9\87\D9\85\D8\A9"(i64 %"%\D9\851.load", i64 5, i64 0)
  %"%\D9\852.load" = load i64, ptr %alloca4, align 8
  call void @"\D9\85\D9\88\D8\B8\D9\81.\D8\A5\D9\86\D8\AC\D8\A7\D8\B2_\D9\85\D9\87\D9\85\D8\A9"(i64 %"%\D9\852.load", i64 4, i64 1)
  %"%\D9\852.load41" = load i64, ptr %alloca4, align 8
  call void @"\D9\85\D9\88\D8\B8\D9\81.\D8\A5\D9\86\D8\AC\D8\A7\D8\B2_\D9\85\D9\87\D9\85\D8\A9"(i64 %"%\D9\852.load41", i64 3, i64 1)
  %"%\D9\852.load42" = load i64, ptr %alloca4, align 8
  call void @"\D9\85\D9\88\D8\B8\D9\81.\D8\A5\D9\86\D8\AC\D8\A7\D8\B2_\D9\85\D9\87\D9\85\D8\A9"(i64 %"%\D9\852.load42", i64 3, i64 0)
  %"%\D9\854.load" = load i64, ptr %alloca10, align 8
  call void @"\D9\85\D9\88\D8\B8\D9\81.\D8\A5\D9\86\D8\AC\D8\A7\D8\B2_\D9\85\D9\87\D9\85\D8\A9"(i64 %"%\D9\854.load", i64 8, i64 0)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.231)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.235)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %"%\D8\AA1.load43" = load i64, ptr %alloca13, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\AA1.load43")
  %"%\D8\AA2.load44" = load i64, ptr %alloca16, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\AA2.load44")
  %"%\D8\AA3.load45" = load i64, ptr %alloca19, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\AA3.load45")
  %"%\D8\AA4.load46" = load i64, ptr %alloca22, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\AA4.load46")
  %"%\D8\AA5.load47" = load i64, ptr %alloca25, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\AA5.load47")
  %"%\D8\AA6.load48" = load i64, ptr %alloca28, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\AA6.load48")
  %"%\D8\AA7.load49" = load i64, ptr %alloca31, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\AA7.load49")
  %"%\D8\AA8.load50" = load i64, ptr %alloca34, align 8
  call void @"\D9\85\D9\87\D9\85\D8\A9.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\AA8.load50")
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.239)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.243)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %"%\D9\851.load51" = load i64, ptr %alloca1, align 8
  call void @"\D9\85\D9\88\D8\B8\D9\81.\D8\B9\D8\B1\D8\B6_\D8\A8\D8\B7\D8\A7\D9\82\D8\A9"(i64 %"%\D9\851.load51")
  %"%\D9\852.load52" = load i64, ptr %alloca4, align 8
  call void @"\D9\85\D9\88\D8\B8\D9\81.\D8\B9\D8\B1\D8\B6_\D8\A8\D8\B7\D8\A7\D9\82\D8\A9"(i64 %"%\D9\852.load52")
  %"%\D9\853.load" = load i64, ptr %alloca7, align 8
  call void @"\D9\85\D9\88\D8\B8\D9\81.\D8\B9\D8\B1\D8\B6_\D8\A8\D8\B7\D8\A7\D9\82\D8\A9"(i64 %"%\D9\853.load")
  %"%\D9\854.load53" = load i64, ptr %alloca10, align 8
  call void @"\D9\85\D9\88\D8\B8\D9\81.\D8\B9\D8\B1\D8\B6_\D8\A8\D8\B7\D8\A7\D9\82\D8\A9"(i64 %"%\D9\854.load53")
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.247)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.251)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %alloca54 = alloca i64, align 8
  %"%273.load" = load i64, ptr %alloca54, align 8
  call void @"\D9\85\D9\84\D8\AE\D8\B5_\D9\85\D8\B4\D8\B1\D9\88\D8\B9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%273.load", ptr @str.const.255)
  %alloca55 = alloca i64, align 8
  %"%273.load56" = load i64, ptr %alloca54, align 8
  store i64 %"%273.load56", ptr %alloca55, align 8
  %"%\D9\85\D9\84\D8\AE\D8\B5.load" = load i64, ptr %alloca55, align 8
  call void @"\D9\85\D9\84\D8\AE\D8\B5_\D9\85\D8\B4\D8\B1\D9\88\D8\B9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D9\85\D9\87\D9\85\D8\A9"(i64 %"%\D9\85\D9\84\D8\AE\D8\B5.load", i64 1, i64 5, i64 1, i64 4)
  %"%\D9\85\D9\84\D8\AE\D8\B5.load57" = load i64, ptr %alloca55, align 8
  call void @"\D9\85\D9\84\D8\AE\D8\B5_\D9\85\D8\B4\D8\B1\D9\88\D8\B9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D9\85\D9\87\D9\85\D8\A9"(i64 %"%\D9\85\D9\84\D8\AE\D8\B5.load57", i64 2, i64 10, i64 0, i64 0)
  %"%\D9\85\D9\84\D8\AE\D8\B5.load58" = load i64, ptr %alloca55, align 8
  call void @"\D9\85\D9\84\D8\AE\D8\B5_\D9\85\D8\B4\D8\B1\D9\88\D8\B9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D9\85\D9\87\D9\85\D8\A9"(i64 %"%\D9\85\D9\84\D8\AE\D8\B5.load58", i64 3, i64 3, i64 1, i64 5)
  %"%\D9\85\D9\84\D8\AE\D8\B5.load59" = load i64, ptr %alloca55, align 8
  call void @"\D9\85\D9\84\D8\AE\D8\B5_\D9\85\D8\B4\D8\B1\D9\88\D8\B9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D9\85\D9\87\D9\85\D8\A9"(i64 %"%\D9\85\D9\84\D8\AE\D8\B5.load59", i64 1, i64 7, i64 1, i64 8)
  %"%\D9\85\D9\84\D8\AE\D8\B5.load60" = load i64, ptr %alloca55, align 8
  call void @"\D9\85\D9\84\D8\AE\D8\B5_\D9\85\D8\B4\D8\B1\D9\88\D8\B9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D9\85\D9\87\D9\85\D8\A9"(i64 %"%\D9\85\D9\84\D8\AE\D8\B5.load60", i64 2, i64 4, i64 1, i64 3)
  %"%\D9\85\D9\84\D8\AE\D8\B5.load61" = load i64, ptr %alloca55, align 8
  call void @"\D9\85\D9\84\D8\AE\D8\B5_\D9\85\D8\B4\D8\B1\D9\88\D8\B9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D9\85\D9\87\D9\85\D8\A9"(i64 %"%\D9\85\D9\84\D8\AE\D8\B5.load61", i64 1, i64 2, i64 1, i64 3)
  %"%\D9\85\D9\84\D8\AE\D8\B5.load62" = load i64, ptr %alloca55, align 8
  call void @"\D9\85\D9\84\D8\AE\D8\B5_\D9\85\D8\B4\D8\B1\D9\88\D8\B9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D9\85\D9\87\D9\85\D8\A9"(i64 %"%\D9\85\D9\84\D8\AE\D8\B5.load62", i64 3, i64 3, i64 0, i64 0)
  %"%\D9\85\D9\84\D8\AE\D8\B5.load63" = load i64, ptr %alloca55, align 8
  call void @"\D9\85\D9\84\D8\AE\D8\B5_\D9\85\D8\B4\D8\B1\D9\88\D8\B9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D9\85\D9\87\D9\85\D8\A9"(i64 %"%\D9\85\D9\84\D8\AE\D8\B5.load63", i64 2, i64 1, i64 0, i64 0)
  %"%\D9\85\D9\84\D8\AE\D8\B5.load64" = load i64, ptr %alloca55, align 8
  call void @"\D9\85\D9\84\D8\AE\D8\B5_\D9\85\D8\B4\D8\B1\D9\88\D8\B9.\D8\B9\D8\B1\D8\B6"(i64 %"%\D9\85\D9\84\D8\AE\D8\B5.load64")
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.256)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %"%\D8\AA1.load65" = load i64, ptr %alloca13, align 8
  %calltmp = call i64 @"\D9\85\D9\87\D9\85\D8\A9.\D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9"(i64 %"%\D8\AA1.load65")
  %cmpeqtmp = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp, label %then_55, label %else_57

then_55:                                          ; preds = %entry
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.260)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  br label %merge_56

else_57:                                          ; preds = %entry
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.264)
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  br label %merge_56

merge_56:                                         ; preds = %else_57, %then_55
  %"%\D8\AA3.load66" = load i64, ptr %alloca19, align 8
  %calltmp67 = call i64 @"\D9\85\D9\87\D9\85\D8\A9.\D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9"(i64 %"%\D8\AA3.load66")
  %cmpeqtmp68 = icmp eq i64 %calltmp67, 1
  br i1 %cmpeqtmp68, label %then_58, label %else_60

then_58:                                          ; preds = %merge_56
  %40 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.268)
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  br label %merge_59

else_60:                                          ; preds = %merge_56
  %42 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.272)
  %43 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  br label %merge_59

merge_59:                                         ; preds = %else_60, %then_58
  %"%\D8\AA4.load69" = load i64, ptr %alloca22, align 8
  %calltmp70 = call i64 @"\D9\85\D9\87\D9\85\D8\A9.\D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9"(i64 %"%\D8\AA4.load69")
  %cmpeqtmp71 = icmp eq i64 %calltmp70, 1
  br i1 %cmpeqtmp71, label %then_61, label %else_63

then_61:                                          ; preds = %merge_59
  %44 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.276)
  %45 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  br label %merge_62

else_63:                                          ; preds = %merge_59
  %46 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.280)
  %47 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  br label %merge_62

merge_62:                                         ; preds = %else_63, %then_61
  %"%\D8\AA6.load72" = load i64, ptr %alloca28, align 8
  %calltmp73 = call i64 @"\D9\85\D9\87\D9\85\D8\A9.\D9\85\D8\AA\D8\A3\D8\AE\D8\B1\D8\A9"(i64 %"%\D8\AA6.load72")
  %cmpeqtmp74 = icmp eq i64 %calltmp73, 1
  br i1 %cmpeqtmp74, label %then_64, label %else_66

then_64:                                          ; preds = %merge_62
  %48 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.284)
  %49 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  br label %merge_65

else_66:                                          ; preds = %merge_62
  %50 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.288)
  %51 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  br label %merge_65

merge_65:                                         ; preds = %else_66, %then_64
  %52 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.292)
  %53 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %54 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.296)
  %55 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  ret i64 0
}

declare i32 @printf(ptr, ...)

declare i32 @strcmp(ptr, ptr)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
