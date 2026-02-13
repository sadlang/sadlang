; ModuleID = '05_complex_school'
source_filename = "05_complex_school"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.ret = private unnamed_addr constant [16 x i8] c"\D9\85\D9\85\D8\AA\D8\A7\D8\B2 \F0\9F\8C\9F\00", align 1
@str.ret.1 = private unnamed_addr constant [20 x i8] c"\D8\AC\D9\8A\D8\AF \D8\AC\D8\AF\D8\A7\D9\8B \E2\9C\A8\00", align 1
@str.ret.2 = private unnamed_addr constant [12 x i8] c"\D8\AC\D9\8A\D8\AF \F0\9F\91\8D\00", align 1
@str.ret.3 = private unnamed_addr constant [18 x i8] c"\D9\85\D9\82\D8\A8\D9\88\D9\84 \E2\9A\A0\EF\B8\8F\00", align 1
@str.ret.4 = private unnamed_addr constant [13 x i8] c"\D8\B1\D8\A7\D8\B3\D8\A8 \E2\9D\8C\00", align 1
@str.const = private unnamed_addr constant [101 x i8] c"    \E2\94\8C\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\90\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.5 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.7 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.9 = private unnamed_addr constant [14 x i8] c"    \E2\94\82 \F0\9F\8E\93 \00", align 1
@str.const.12 = private unnamed_addr constant [4 x i8] c" (#\00", align 1
@fmt.d = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1
@str.const.14 = private unnamed_addr constant [2 x i8] c")\00", align 1
@str.const.18 = private unnamed_addr constant [25 x i8] c"    \E2\94\82 \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA: \00", align 1
@str.const.21 = private unnamed_addr constant [14 x i8] c" | \D8\B9\D9\84\D9\88\D9\85: \00", align 1
@str.const.24 = private unnamed_addr constant [12 x i8] c" | \D9\84\D8\BA\D8\A9: \00", align 1
@str.const.27 = private unnamed_addr constant [16 x i8] c" | \D8\AA\D8\A7\D8\B1\D9\8A\D8\AE: \00", align 1
@str.const.32 = private unnamed_addr constant [23 x i8] c"    \E2\94\82 \D8\A7\D9\84\D9\85\D8\B9\D8\AF\D9\84: \00", align 1
@str.const.35 = private unnamed_addr constant [20 x i8] c" | \D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\8A\D8\B1: \00", align 1
@str.const.37 = private unnamed_addr constant [11 x i8] c"\D9\85\D9\85\D8\AA\D8\A7\D8\B2\00", align 1
@str.const.39 = private unnamed_addr constant [14 x i8] c" | \D8\BA\D9\8A\D8\A7\D8\A8: \00", align 1
@str.const.44 = private unnamed_addr constant [101 x i8] c"    \E2\94\94\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\98\00", align 1
@str.const.48 = private unnamed_addr constant [16 x i8] c"\D8\AC\D9\8A\D8\AF \D8\AC\D8\AF\D8\A7\D9\8B\00", align 1
@str.const.50 = private unnamed_addr constant [7 x i8] c"\D8\AC\D9\8A\D8\AF\00", align 1
@str.const.52 = private unnamed_addr constant [11 x i8] c"\D9\85\D9\82\D8\A8\D9\88\D9\84\00", align 1
@str.const.54 = private unnamed_addr constant [9 x i8] c"\D8\B1\D8\A7\D8\B3\D8\A8\00", align 1
@str.const.56 = private unnamed_addr constant [10 x i8] c"    \F0\9F\93\8A \00", align 1
@str.const.59 = private unnamed_addr constant [14 x i8] c": \D9\85\D8\AA\D9\88\D8\B3\D8\B7=\00", align 1
@str.const.63 = private unnamed_addr constant [11 x i8] c" \D8\A3\D8\B9\D9\84\D9\89=\00", align 1
@str.const.66 = private unnamed_addr constant [11 x i8] c" \D8\A3\D8\AF\D9\86\D9\89=\00", align 1
@str.const.69 = private unnamed_addr constant [3 x i8] c" (\00", align 1
@str.const.72 = private unnamed_addr constant [11 x i8] c" \D8\B7\D9\84\D8\A7\D8\A8)\00", align 1
@str.const.76 = private unnamed_addr constant [101 x i8] c"    \E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.80 = private unnamed_addr constant [53 x i8] c"    \E2\95\91     \F0\9F\93\8B \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D9\81\D8\B5\D9\84           \E2\95\91\00", align 1
@str.const.84 = private unnamed_addr constant [101 x i8] c"    \E2\95\A0\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\A3\00", align 1
@str.const.88 = private unnamed_addr constant [39 x i8] c"    \E2\95\91  \D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A \D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8:   \00", align 1
@str.const.93 = private unnamed_addr constant [36 x i8] c"    \E2\95\91  \D8\A7\D9\84\D9\86\D8\A7\D8\AC\D8\AD\D9\88\D9\86:         \00", align 1
@str.const.98 = private unnamed_addr constant [36 x i8] c"    \E2\95\91  \D8\A7\D9\84\D8\B1\D8\A7\D8\B3\D8\A8\D9\88\D9\86:         \00", align 1
@str.const.103 = private unnamed_addr constant [37 x i8] c"    \E2\95\91  \D8\A7\D9\84\D9\85\D8\AA\D9\81\D9\88\D9\82\D9\88\D9\86 (90+):  \00", align 1
@str.const.108 = private unnamed_addr constant [40 x i8] c"    \E2\95\91  \D8\A7\D9\84\D9\85\D9\86\D8\B0\D8\B1\D9\88\D9\86 (\D8\BA\D9\8A\D8\A7\D8\A8):  \00", align 1
@str.const.113 = private unnamed_addr constant [37 x i8] c"    \E2\95\91  \D9\86\D8\B3\D8\A8\D8\A9 \D8\A7\D9\84\D9\86\D8\AC\D8\A7\D8\AD:     \00", align 1
@str.const.116 = private unnamed_addr constant [2 x i8] c"%\00", align 1
@str.const.120 = private unnamed_addr constant [101 x i8] c"    \E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.124 = private unnamed_addr constant [121 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.128 = private unnamed_addr constant [61 x i8] c"\E2\95\91  \F0\9F\8F\AB Phase 17: \D9\86\D8\B8\D8\A7\D9\85 \D8\A5\D8\AF\D8\A7\D8\B1\D8\A9 \D9\85\D8\AF\D8\B1\D8\B3\D8\A9       \E2\95\91\00", align 1
@str.const.132 = private unnamed_addr constant [121 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.136 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.140 = private unnamed_addr constant [61 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\AA\D8\B3\D8\AC\D9\8A\D9\84 \D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8 \D9\88\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.144 = private unnamed_addr constant [9 x i8] c"\D8\A3\D8\AD\D9\85\D8\AF\00", align 1
@str.const.145 = private unnamed_addr constant [11 x i8] c"\D9\81\D8\A7\D8\B7\D9\85\D8\A9\00", align 1
@str.const.146 = private unnamed_addr constant [9 x i8] c"\D8\AE\D8\A7\D9\84\D8\AF\00", align 1
@str.const.147 = private unnamed_addr constant [9 x i8] c"\D9\85\D8\B1\D9\8A\D9\85\00", align 1
@str.const.148 = private unnamed_addr constant [7 x i8] c"\D8\B9\D9\85\D8\B1\00", align 1
@str.const.149 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.153 = private unnamed_addr constant [46 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\A8\D8\B7\D8\A7\D9\82\D8\A7\D8\AA \D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.157 = private unnamed_addr constant [50 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D9\88\D8\A7\D8\AF \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.161 = private unnamed_addr constant [19 x i8] c"\D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.162 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85\00", align 1
@str.const.163 = private unnamed_addr constant [26 x i8] c"\D8\A7\D9\84\D9\84\D8\BA\D8\A9 \D8\A7\D9\84\D8\B9\D8\B1\D8\A8\D9\8A\D8\A9\00", align 1
@str.const.164 = private unnamed_addr constant [15 x i8] c"\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE\00", align 1
@str.const.165 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.169 = private unnamed_addr constant [42 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D9\81\D8\B5\D9\84 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.173 = private unnamed_addr constant [51 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\A3\D8\B9\D9\84\D9\89 \D9\88\D8\A3\D8\AF\D9\86\D9\89 \D8\AF\D8\B1\D8\AC\D8\A7\D8\AA \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.177 = private unnamed_addr constant [24 x i8] c"    \D8\A3\D8\AD\D9\85\D8\AF: \D8\A3\D8\B9\D9\84\D9\89=\00", align 1
@str.const.185 = private unnamed_addr constant [24 x i8] c"    \D9\85\D8\B1\D9\8A\D9\85: \D8\A3\D8\B9\D9\84\D9\89=\00", align 1
@str.const.193 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.197 = private unnamed_addr constant [57 x i8] c"\E2\9C\85 \D8\AA\D9\85 \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D9\85\D8\AF\D8\B1\D8\B3\D8\A9 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD\00", align 1

define void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, ptr %"\D8\A7\D8\B3\D9\85", i64 %"\D8\B1\D9\82\D9\85_\D8\B7") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\B73" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\B7", ptr %"\D8\B1\D9\82\D9\85_\D8\B73", align 8
  %"\D8\A7\D8\B3\D9\852" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %alloca8 = alloca i64, align 8
  %alloca9 = alloca i64, align 8
  store ptr %"\D8\A7\D8\B3\D9\852", ptr %alloca4, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\B7.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\B73", align 8
  store i64 %"%\D8\B1\D9\82\D9\85_\D8\B7.load", ptr %alloca6, align 8
  store i64 0, ptr %alloca7, align 8
  store i64 0, ptr %alloca8, align 8
  store i64 0, ptr %alloca5, align 8
  store i64 0, ptr %alloca9, align 8
  store i64 0, ptr %alloca, align 8
  ret void
}

define void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA"(i64 %self, i64 %"\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA", i64 %"\D8\B9\D9\84\D9\88\D9\85", i64 %"\D9\84\D8\BA\D8\A9", i64 %"\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE") {
entry:
  %"\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE5" = alloca i64, align 8
  store i64 %"\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE", ptr %"\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE5", align 8
  %"\D9\84\D8\BA\D8\A94" = alloca i64, align 8
  store i64 %"\D9\84\D8\BA\D8\A9", ptr %"\D9\84\D8\BA\D8\A94", align 8
  %"\D8\B9\D9\84\D9\88\D9\853" = alloca i64, align 8
  store i64 %"\D8\B9\D9\84\D9\88\D9\85", ptr %"\D8\B9\D9\84\D9\88\D9\853", align 8
  %"\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA2" = alloca i64, align 8
  store i64 %"\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA", ptr %"\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA2", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca i64, align 8
  %alloca8 = alloca i64, align 8
  %alloca9 = alloca i64, align 8
  %alloca10 = alloca i64, align 8
  %alloca11 = alloca i64, align 8
  %"%\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA2", align 8
  store i64 %"%\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load", ptr %alloca9, align 8
  %"%\D8\B9\D9\84\D9\88\D9\85.load" = load i64, ptr %"\D8\B9\D9\84\D9\88\D9\853", align 8
  store i64 %"%\D8\B9\D9\84\D9\88\D9\85.load", ptr %alloca10, align 8
  %"%\D9\84\D8\BA\D8\A9.load" = load i64, ptr %"\D9\84\D8\BA\D8\A94", align 8
  store i64 %"%\D9\84\D8\BA\D8\A9.load", ptr %alloca7, align 8
  %"%\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load" = load i64, ptr %"\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE5", align 8
  store i64 %"%\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load", ptr %alloca11, align 8
  ret void
}

define i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A7\D9\84\D9\85\D8\B9\D8\AF\D9\84"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %alloca5, align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load" = load i64, ptr %alloca6, align 8
  %addtmp = add i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load", %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load"
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load" = load i64, ptr %alloca3, align 8
  %addtmp8 = add i64 %addtmp, %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load"
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load" = load i64, ptr %alloca7, align 8
  %addtmp9 = add i64 %addtmp8, %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load"
  %divtmp = sdiv i64 %addtmp9, 4
  ret i64 %divtmp
}

define ptr @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\8A\D8\B1"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %alloca5, align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load" = load i64, ptr %alloca6, align 8
  %addtmp = add i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load", %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load"
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load" = load i64, ptr %alloca3, align 8
  %addtmp8 = add i64 %addtmp, %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load"
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load" = load i64, ptr %alloca7, align 8
  %addtmp9 = add i64 %addtmp8, %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load"
  %divtmp = sdiv i64 %addtmp9, 4
  %alloca10 = alloca i64, align 8
  store i64 %divtmp, ptr %alloca10, align 8
  %"%\D9\85\D8\B9\D8\AF\D9\84.load" = load i64, ptr %alloca10, align 8
  %cmpgetmp = icmp sge i64 %"%\D9\85\D8\B9\D8\AF\D9\84.load", 90
  br i1 %cmpgetmp, label %then_0, label %merge_1

then_0:                                           ; preds = %entry
  ret ptr @str.ret

merge_1:                                          ; preds = %entry
  %"%\D9\85\D8\B9\D8\AF\D9\84.load11" = load i64, ptr %alloca10, align 8
  %cmpgetmp12 = icmp sge i64 %"%\D9\85\D8\B9\D8\AF\D9\84.load11", 80
  br i1 %cmpgetmp12, label %then_2, label %merge_3

then_2:                                           ; preds = %merge_1
  ret ptr @str.ret.1

merge_3:                                          ; preds = %merge_1
  %"%\D9\85\D8\B9\D8\AF\D9\84.load13" = load i64, ptr %alloca10, align 8
  %cmpgetmp14 = icmp sge i64 %"%\D9\85\D8\B9\D8\AF\D9\84.load13", 70
  br i1 %cmpgetmp14, label %then_4, label %merge_5

then_4:                                           ; preds = %merge_3
  ret ptr @str.ret.2

merge_5:                                          ; preds = %merge_3
  %"%\D9\85\D8\B9\D8\AF\D9\84.load15" = load i64, ptr %alloca10, align 8
  %cmpgetmp16 = icmp sge i64 %"%\D9\85\D8\B9\D8\AF\D9\84.load15", 60
  br i1 %cmpgetmp16, label %then_6, label %merge_7

then_6:                                           ; preds = %merge_5
  ret ptr @str.ret.3

merge_7:                                          ; preds = %merge_5
  ret ptr @str.ret.4
}

define i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D9\86\D8\A7\D8\AC\D8\AD"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %alloca5, align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load" = load i64, ptr %alloca6, align 8
  %addtmp = add i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load", %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load"
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load" = load i64, ptr %alloca3, align 8
  %addtmp8 = add i64 %addtmp, %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load"
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load" = load i64, ptr %alloca7, align 8
  %addtmp9 = add i64 %addtmp8, %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load"
  %divtmp = sdiv i64 %addtmp9, 4
  %alloca10 = alloca i64, align 8
  store i64 %divtmp, ptr %alloca10, align 8
  %"%\D9\85\D8\B9\D8\AF\D9\84.load" = load i64, ptr %alloca10, align 8
  %cmpgetmp = icmp sge i64 %"%\D9\85\D8\B9\D8\AF\D9\84.load", 60
  br i1 %cmpgetmp, label %then_8, label %merge_9

then_8:                                           ; preds = %entry
  ret i64 1

merge_9:                                          ; preds = %entry
  ret i64 0
}

define i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A3\D8\B9\D9\84\D9\89_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %alloca8 = alloca i64, align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %alloca5, align 8
  store i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load", ptr %alloca8, align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load" = load i64, ptr %alloca6, align 8
  %"%\D8\A3\D8\B9\D9\84\D9\89.load" = load i64, ptr %alloca8, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load", %"%\D8\A3\D8\B9\D9\84\D9\89.load"
  br i1 %cmpgttmp, label %then_10, label %merge_11

then_10:                                          ; preds = %entry
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load9" = load i64, ptr %alloca6, align 8
  store i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load9", ptr %alloca8, align 8
  br label %merge_11

merge_11:                                         ; preds = %then_10, %entry
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load" = load i64, ptr %alloca3, align 8
  %"%\D8\A3\D8\B9\D9\84\D9\89.load10" = load i64, ptr %alloca8, align 8
  %cmpgttmp11 = icmp sgt i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load", %"%\D8\A3\D8\B9\D9\84\D9\89.load10"
  br i1 %cmpgttmp11, label %then_12, label %merge_13

then_12:                                          ; preds = %merge_11
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load12" = load i64, ptr %alloca3, align 8
  store i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load12", ptr %alloca8, align 8
  br label %merge_13

merge_13:                                         ; preds = %then_12, %merge_11
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load" = load i64, ptr %alloca7, align 8
  %"%\D8\A3\D8\B9\D9\84\D9\89.load13" = load i64, ptr %alloca8, align 8
  %cmpgttmp14 = icmp sgt i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load", %"%\D8\A3\D8\B9\D9\84\D9\89.load13"
  br i1 %cmpgttmp14, label %then_14, label %merge_15

then_14:                                          ; preds = %merge_13
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load15" = load i64, ptr %alloca7, align 8
  store i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load15", ptr %alloca8, align 8
  br label %merge_15

merge_15:                                         ; preds = %then_14, %merge_13
  %loadtmp = load i64, ptr %alloca8, align 8
  ret i64 %loadtmp
}

define i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A3\D8\AF\D9\86\D9\89_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %alloca8 = alloca i64, align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %alloca5, align 8
  store i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load", ptr %alloca8, align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load" = load i64, ptr %alloca6, align 8
  %"%\D8\A3\D8\AF\D9\86\D9\89.load" = load i64, ptr %alloca8, align 8
  %cmplttmp = icmp slt i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load", %"%\D8\A3\D8\AF\D9\86\D9\89.load"
  br i1 %cmplttmp, label %then_16, label %merge_17

then_16:                                          ; preds = %entry
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load9" = load i64, ptr %alloca6, align 8
  store i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load9", ptr %alloca8, align 8
  br label %merge_17

merge_17:                                         ; preds = %then_16, %entry
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load" = load i64, ptr %alloca3, align 8
  %"%\D8\A3\D8\AF\D9\86\D9\89.load10" = load i64, ptr %alloca8, align 8
  %cmplttmp11 = icmp slt i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load", %"%\D8\A3\D8\AF\D9\86\D9\89.load10"
  br i1 %cmplttmp11, label %then_18, label %merge_19

then_18:                                          ; preds = %merge_17
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load12" = load i64, ptr %alloca3, align 8
  store i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load12", ptr %alloca8, align 8
  br label %merge_19

merge_19:                                         ; preds = %then_18, %merge_17
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load" = load i64, ptr %alloca7, align 8
  %"%\D8\A3\D8\AF\D9\86\D9\89.load13" = load i64, ptr %alloca8, align 8
  %cmplttmp14 = icmp slt i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load", %"%\D8\A3\D8\AF\D9\86\D9\89.load13"
  br i1 %cmplttmp14, label %then_20, label %merge_21

then_20:                                          ; preds = %merge_19
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load15" = load i64, ptr %alloca7, align 8
  store i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load15", ptr %alloca8, align 8
  br label %merge_21

merge_21:                                         ; preds = %then_20, %merge_19
  %loadtmp = load i64, ptr %alloca8, align 8
  ret i64 %loadtmp
}

define void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\BA\D9\8A\D8\A7\D8\A8"(i64 %self, i64 %"\D8\A3\D9\8A\D8\A7\D9\85") {
entry:
  %"\D8\A3\D9\8A\D8\A7\D9\852" = alloca i64, align 8
  store i64 %"\D8\A3\D9\8A\D8\A7\D9\85", ptr %"\D8\A3\D9\8A\D8\A7\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %alloca8 = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\BA\D9\8A\D8\A7\D8\A8.load" = load i64, ptr %alloca, align 8
  %"%\D8\A3\D9\8A\D8\A7\D9\85.load" = load i64, ptr %"\D8\A3\D9\8A\D8\A7\D9\852", align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\BA\D9\8A\D8\A7\D8\A8.load", %"%\D8\A3\D9\8A\D8\A7\D9\85.load"
  store i64 %addtmp, ptr %alloca, align 8
  ret void
}

define i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D9\85\D9\86\D8\B0\D8\B1"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\BA\D9\8A\D8\A7\D8\A8.load" = load i64, ptr %alloca, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\BA\D9\8A\D8\A7\D8\A8.load", 10
  br i1 %cmpgttmp, label %then_22, label %merge_23

then_22:                                          ; preds = %entry
  ret i64 1

merge_23:                                         ; preds = %entry
  ret i64 0
}

define void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\B9\D8\B1\D8\B6_\D8\A8\D8\B7\D8\A7\D9\82\D8\A9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.9)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca2)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B7\D8\A7\D9\84\D8\A8.load" = load i64, ptr %alloca4, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B7\D8\A7\D9\84\D8\A8.load")
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.14)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %alloca5, align 8
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load")
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.21)
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load" = load i64, ptr %alloca6, align 8
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load")
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.24)
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load" = load i64, ptr %alloca3, align 8
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load")
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.27)
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load" = load i64, ptr %alloca7, align 8
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load")
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.32)
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load8" = load i64, ptr %alloca5, align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load9" = load i64, ptr %alloca6, align 8
  %addtmp = add i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load8", %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85.load9"
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load10" = load i64, ptr %alloca3, align 8
  %addtmp11 = add i64 %addtmp, %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D9\84\D8\BA\D8\A9.load10"
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load12" = load i64, ptr %alloca7, align 8
  %addtmp13 = add i64 %addtmp11, %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load12"
  %divtmp = sdiv i64 %addtmp13, 4
  %alloca14 = alloca i64, align 8
  store i64 %divtmp, ptr %alloca14, align 8
  %"%\D9\85.load" = load i64, ptr %alloca14, align 8
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\85.load")
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.35)
  %"%\D9\85.load15" = load i64, ptr %alloca14, align 8
  %cmpgetmp = icmp sge i64 %"%\D9\85.load15", 90
  br i1 %cmpgetmp, label %then_24, label %else_26

then_24:                                          ; preds = %entry
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.37)
  br label %merge_25

else_26:                                          ; preds = %entry
  %"%\D9\85.load16" = load i64, ptr %alloca14, align 8
  %cmpgetmp17 = icmp sge i64 %"%\D9\85.load16", 80
  br i1 %cmpgetmp17, label %then_27, label %else_29

merge_25:                                         ; preds = %merge_28, %then_24
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.39)
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\BA\D9\8A\D8\A7\D8\A8.load" = load i64, ptr %alloca, align 8
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\BA\D9\8A\D8\A7\D8\A8.load")
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.44)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  ret void

then_27:                                          ; preds = %else_26
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.48)
  br label %merge_28

else_29:                                          ; preds = %else_26
  %"%\D9\85.load18" = load i64, ptr %alloca14, align 8
  %cmpgetmp19 = icmp sge i64 %"%\D9\85.load18", 70
  br i1 %cmpgetmp19, label %then_30, label %else_32

merge_28:                                         ; preds = %merge_31, %then_27
  br label %merge_25

then_30:                                          ; preds = %else_29
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.50)
  br label %merge_31

else_32:                                          ; preds = %else_29
  %"%\D9\85.load20" = load i64, ptr %alloca14, align 8
  %cmpgetmp21 = icmp sge i64 %"%\D9\85.load20", 60
  br i1 %cmpgetmp21, label %then_33, label %else_35

merge_31:                                         ; preds = %merge_34, %then_30
  br label %merge_28

then_33:                                          ; preds = %else_32
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.52)
  br label %merge_34

else_35:                                          ; preds = %else_32
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.54)
  br label %merge_34

merge_34:                                         ; preds = %else_35, %then_33
  br label %merge_31
}

define void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, ptr %"\D8\A7\D8\B3\D9\85") {
entry:
  %"\D8\A7\D8\B3\D9\852" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  store ptr %"\D8\A7\D8\B3\D9\852", ptr %alloca3, align 8
  store i64 0, ptr %alloca, align 8
  store i64 0, ptr %alloca4, align 8
  store i64 0, ptr %alloca6, align 8
  store i64 100, ptr %alloca5, align 8
  ret void
}

define void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %self, i64 %"\D8\AF\D8\B1\D8\AC\D8\A9") {
entry:
  %"\D8\AF\D8\B1\D8\AC\D8\A92" = alloca i64, align 8
  store i64 %"\D8\AF\D8\B1\D8\AC\D8\A9", ptr %"\D8\AF\D8\B1\D8\AC\D8\A92", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A92", align 8
  %addtmp = add i64 %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA.load", %"%\D8\AF\D8\B1\D8\AC\D8\A9.load"
  store i64 %addtmp, ptr %alloca, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load" = load i64, ptr %alloca4, align 8
  %addtmp7 = add i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load", 1
  store i64 %addtmp7, ptr %alloca4, align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load8" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A92", align 8
  %"%\D8\A3\D8\B9\D9\84\D9\89_\D8\AF\D8\B1\D8\AC\D8\A9.load" = load i64, ptr %alloca6, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load8", %"%\D8\A3\D8\B9\D9\84\D9\89_\D8\AF\D8\B1\D8\AC\D8\A9.load"
  br i1 %cmpgttmp, label %then_36, label %merge_37

then_36:                                          ; preds = %entry
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load9" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A92", align 8
  store i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load9", ptr %alloca6, align 8
  br label %merge_37

merge_37:                                         ; preds = %then_36, %entry
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load10" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A92", align 8
  %"%\D8\A3\D8\AF\D9\86\D9\89_\D8\AF\D8\B1\D8\AC\D8\A9.load" = load i64, ptr %alloca5, align 8
  %cmplttmp = icmp slt i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load10", %"%\D8\A3\D8\AF\D9\86\D9\89_\D8\AF\D8\B1\D8\AC\D8\A9.load"
  br i1 %cmplttmp, label %then_38, label %merge_39

then_38:                                          ; preds = %merge_37
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load11" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A92", align 8
  store i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load11", ptr %alloca5, align 8
  br label %merge_39

merge_39:                                         ; preds = %then_38, %merge_37
  ret void
}

define i64 @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A7\D9\84\D9\85\D8\AA\D9\88\D8\B3\D8\B7"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load" = load i64, ptr %alloca3, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load", 0
  br i1 %cmpeqtmp, label %then_40, label %merge_41

then_40:                                          ; preds = %entry
  ret i64 0

merge_41:                                         ; preds = %entry
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load6" = load i64, ptr %alloca3, align 8
  %divtmp = sdiv i64 %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA.load", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load6"
  ret i64 %divtmp
}

define void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\B9\D8\B1\D8\B6_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.56)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.59)
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load" = load i64, ptr %alloca3, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load", 0
  br i1 %cmpgttmp, label %then_42, label %else_44

then_42:                                          ; preds = %entry
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load6" = load i64, ptr %alloca3, align 8
  %divtmp = sdiv i64 %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA.load", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load6"
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %divtmp)
  br label %merge_43

else_44:                                          ; preds = %entry
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 0)
  br label %merge_43

merge_43:                                         ; preds = %else_44, %then_42
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.63)
  %"%\D8\A3\D8\B9\D9\84\D9\89_\D8\AF\D8\B1\D8\AC\D8\A9.load" = load i64, ptr %alloca5, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A3\D8\B9\D9\84\D9\89_\D8\AF\D8\B1\D8\AC\D8\A9.load")
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.66)
  %"%\D8\A3\D8\AF\D9\86\D9\89_\D8\AF\D8\B1\D8\AC\D8\A9.load" = load i64, ptr %alloca4, align 8
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A3\D8\AF\D9\86\D9\89_\D8\AF\D8\B1\D8\AC\D8\A9.load")
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.69)
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load7" = load i64, ptr %alloca3, align 8
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load7")
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.72)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  ret void
}

define void @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\B5\D9\84.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  store i64 0, ptr %alloca2, align 8
  store i64 0, ptr %alloca3, align 8
  store i64 0, ptr %alloca5, align 8
  store i64 0, ptr %alloca4, align 8
  ret void
}

define void @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\B5\D9\84.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\B7\D8\A7\D9\84\D8\A8"(i64 %self, i64 %"\D9\86\D8\A7\D8\AC\D8\AD", i64 %"\D9\85\D8\AA\D9\81\D9\88\D9\82", i64 %"\D9\85\D9\86\D8\B0\D8\B1") {
entry:
  %"\D9\85\D9\86\D8\B0\D8\B14" = alloca i64, align 8
  store i64 %"\D9\85\D9\86\D8\B0\D8\B1", ptr %"\D9\85\D9\86\D8\B0\D8\B14", align 8
  %"\D9\85\D8\AA\D9\81\D9\88\D9\823" = alloca i64, align 8
  store i64 %"\D9\85\D8\AA\D9\81\D9\88\D9\82", ptr %"\D9\85\D8\AA\D9\81\D9\88\D9\823", align 8
  %"\D9\86\D8\A7\D8\AC\D8\AD2" = alloca i64, align 8
  store i64 %"\D9\86\D8\A7\D8\AC\D8\AD", ptr %"\D9\86\D8\A7\D8\AC\D8\AD2", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %alloca7 = alloca i64, align 8
  %alloca8 = alloca i64, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load" = load i64, ptr %alloca7, align 8
  %addtmp = add i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load", 1
  store i64 %addtmp, ptr %alloca7, align 8
  %"%\D9\86\D8\A7\D8\AC\D8\AD.load" = load i64, ptr %"\D9\86\D8\A7\D8\AC\D8\AD2", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\A7\D8\AC\D8\AD.load", 1
  br i1 %cmpeqtmp, label %then_45, label %else_47

then_45:                                          ; preds = %entry
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\86\D8\A7\D8\AC\D8\AD\D9\8A\D9\86.load" = load i64, ptr %alloca, align 8
  %addtmp9 = add i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\86\D8\A7\D8\AC\D8\AD\D9\8A\D9\86.load", 1
  store i64 %addtmp9, ptr %alloca, align 8
  br label %merge_46

else_47:                                          ; preds = %entry
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B1\D8\A7\D8\B3\D8\A8\D9\8A\D9\86.load" = load i64, ptr %alloca5, align 8
  %addtmp10 = add i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B1\D8\A7\D8\B3\D8\A8\D9\8A\D9\86.load", 1
  store i64 %addtmp10, ptr %alloca5, align 8
  br label %merge_46

merge_46:                                         ; preds = %else_47, %then_45
  %"%\D9\85\D8\AA\D9\81\D9\88\D9\82.load" = load i64, ptr %"\D9\85\D8\AA\D9\81\D9\88\D9\823", align 8
  %cmpeqtmp11 = icmp eq i64 %"%\D9\85\D8\AA\D9\81\D9\88\D9\82.load", 1
  br i1 %cmpeqtmp11, label %then_48, label %merge_49

then_48:                                          ; preds = %merge_46
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AA\D9\81\D9\88\D9\82\D9\8A\D9\86.load" = load i64, ptr %alloca6, align 8
  %addtmp12 = add i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AA\D9\81\D9\88\D9\82\D9\8A\D9\86.load", 1
  store i64 %addtmp12, ptr %alloca6, align 8
  br label %merge_49

merge_49:                                         ; preds = %then_48, %merge_46
  %"%\D9\85\D9\86\D8\B0\D8\B1.load" = load i64, ptr %"\D9\85\D9\86\D8\B0\D8\B14", align 8
  %cmpeqtmp13 = icmp eq i64 %"%\D9\85\D9\86\D8\B0\D8\B1.load", 1
  br i1 %cmpeqtmp13, label %then_50, label %merge_51

then_50:                                          ; preds = %merge_49
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\86\D8\B0\D8\B1\D9\8A\D9\86.load" = load i64, ptr %alloca8, align 8
  %addtmp14 = add i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\86\D8\B0\D8\B1\D9\8A\D9\86.load", 1
  store i64 %addtmp14, ptr %alloca8, align 8
  br label %merge_51

merge_51:                                         ; preds = %then_50, %merge_49
  ret void
}

define void @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\B5\D9\84.\D8\B9\D8\B1\D8\B6"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.76)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.80)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.84)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.88)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load" = load i64, ptr %alloca4, align 8
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load")
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.93)
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\86\D8\A7\D8\AC\D8\AD\D9\8A\D9\86.load" = load i64, ptr %alloca, align 8
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\86\D8\A7\D8\AC\D8\AD\D9\8A\D9\86.load")
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.98)
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B1\D8\A7\D8\B3\D8\A8\D9\8A\D9\86.load" = load i64, ptr %alloca2, align 8
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B1\D8\A7\D8\B3\D8\A8\D9\8A\D9\86.load")
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.103)
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AA\D9\81\D9\88\D9\82\D9\8A\D9\86.load" = load i64, ptr %alloca3, align 8
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AA\D9\81\D9\88\D9\82\D9\8A\D9\86.load")
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.108)
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\86\D8\B0\D8\B1\D9\8A\D9\86.load" = load i64, ptr %alloca5, align 8
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\86\D8\B0\D8\B1\D9\8A\D9\86.load")
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load6" = load i64, ptr %alloca4, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load6", 0
  br i1 %cmpgttmp, label %then_52, label %merge_53

then_52:                                          ; preds = %entry
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.113)
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\86\D8\A7\D8\AC\D8\AD\D9\8A\D9\86.load7" = load i64, ptr %alloca, align 8
  %multmp = mul i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\86\D8\A7\D8\AC\D8\AD\D9\8A\D9\86.load7", 100
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load8" = load i64, ptr %alloca4, align 8
  %divtmp = sdiv i64 %multmp, %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B7\D9\84\D8\A7\D8\A8.load8"
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %divtmp)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.116)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  br label %merge_53

merge_53:                                         ; preds = %then_52, %entry
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.120)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  ret void
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.124)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.128)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.132)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.136)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.140)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %alloca = alloca i64, align 8
  %"%148.load" = load i64, ptr %alloca, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%148.load", ptr @str.const.144, i64 101)
  %alloca1 = alloca i64, align 8
  %"%148.load2" = load i64, ptr %alloca, align 8
  store i64 %"%148.load2", ptr %alloca1, align 8
  %alloca3 = alloca i64, align 8
  %"%152.load" = load i64, ptr %alloca3, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%152.load", ptr @str.const.145, i64 102)
  %alloca4 = alloca i64, align 8
  %"%152.load5" = load i64, ptr %alloca3, align 8
  store i64 %"%152.load5", ptr %alloca4, align 8
  %alloca6 = alloca i64, align 8
  %"%156.load" = load i64, ptr %alloca6, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%156.load", ptr @str.const.146, i64 103)
  %alloca7 = alloca i64, align 8
  %"%156.load8" = load i64, ptr %alloca6, align 8
  store i64 %"%156.load8", ptr %alloca7, align 8
  %alloca9 = alloca i64, align 8
  %"%160.load" = load i64, ptr %alloca9, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%160.load", ptr @str.const.147, i64 104)
  %alloca10 = alloca i64, align 8
  %"%160.load11" = load i64, ptr %alloca9, align 8
  store i64 %"%160.load11", ptr %alloca10, align 8
  %alloca12 = alloca i64, align 8
  %"%164.load" = load i64, ptr %alloca12, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%164.load", ptr @str.const.148, i64 105)
  %alloca13 = alloca i64, align 8
  %"%164.load14" = load i64, ptr %alloca12, align 8
  store i64 %"%164.load14", ptr %alloca13, align 8
  %"%\D8\B71.load" = load i64, ptr %alloca1, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA"(i64 %"%\D8\B71.load", i64 95, i64 88, i64 92, i64 85)
  %"%\D8\B72.load" = load i64, ptr %alloca4, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA"(i64 %"%\D8\B72.load", i64 78, i64 91, i64 85, i64 88)
  %"%\D8\B73.load" = load i64, ptr %alloca7, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA"(i64 %"%\D8\B73.load", i64 55, i64 62, i64 48, i64 70)
  %"%\D8\B74.load" = load i64, ptr %alloca10, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA"(i64 %"%\D8\B74.load", i64 92, i64 95, i64 98, i64 94)
  %"%\D8\B75.load" = load i64, ptr %alloca13, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA"(i64 %"%\D8\B75.load", i64 40, i64 35, i64 55, i64 42)
  %"%\D8\B71.load15" = load i64, ptr %alloca1, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\BA\D9\8A\D8\A7\D8\A8"(i64 %"%\D8\B71.load15", i64 3)
  %"%\D8\B72.load16" = load i64, ptr %alloca4, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\BA\D9\8A\D8\A7\D8\A8"(i64 %"%\D8\B72.load16", i64 5)
  %"%\D8\B73.load17" = load i64, ptr %alloca7, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\BA\D9\8A\D8\A7\D8\A8"(i64 %"%\D8\B73.load17", i64 15)
  %"%\D8\B74.load18" = load i64, ptr %alloca10, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\BA\D9\8A\D8\A7\D8\A8"(i64 %"%\D8\B74.load18", i64 1)
  %"%\D8\B75.load19" = load i64, ptr %alloca13, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\BA\D9\8A\D8\A7\D8\A8"(i64 %"%\D8\B75.load19", i64 20)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.149)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.153)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %"%\D8\B71.load20" = load i64, ptr %alloca1, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\B9\D8\B1\D8\B6_\D8\A8\D8\B7\D8\A7\D9\82\D8\A9"(i64 %"%\D8\B71.load20")
  %"%\D8\B72.load21" = load i64, ptr %alloca4, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\B9\D8\B1\D8\B6_\D8\A8\D8\B7\D8\A7\D9\82\D8\A9"(i64 %"%\D8\B72.load21")
  %"%\D8\B73.load22" = load i64, ptr %alloca7, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\B9\D8\B1\D8\B6_\D8\A8\D8\B7\D8\A7\D9\82\D8\A9"(i64 %"%\D8\B73.load22")
  %"%\D8\B74.load23" = load i64, ptr %alloca10, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\B9\D8\B1\D8\B6_\D8\A8\D8\B7\D8\A7\D9\82\D8\A9"(i64 %"%\D8\B74.load23")
  %"%\D8\B75.load24" = load i64, ptr %alloca13, align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\B9\D8\B1\D8\B6_\D8\A8\D8\B7\D8\A7\D9\82\D8\A9"(i64 %"%\D8\B75.load24")
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.157)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %alloca25 = alloca i64, align 8
  %"%211.load" = load i64, ptr %alloca25, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%211.load", ptr @str.const.161)
  %alloca26 = alloca i64, align 8
  %"%211.load27" = load i64, ptr %alloca25, align 8
  store i64 %"%211.load27", ptr %alloca26, align 8
  %alloca28 = alloca i64, align 8
  %"%214.load" = load i64, ptr %alloca28, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%214.load", ptr @str.const.162)
  %alloca29 = alloca i64, align 8
  %"%214.load30" = load i64, ptr %alloca28, align 8
  store i64 %"%214.load30", ptr %alloca29, align 8
  %alloca31 = alloca i64, align 8
  %"%217.load" = load i64, ptr %alloca31, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%217.load", ptr @str.const.163)
  %alloca32 = alloca i64, align 8
  %"%217.load33" = load i64, ptr %alloca31, align 8
  store i64 %"%217.load33", ptr %alloca32, align 8
  %alloca34 = alloca i64, align 8
  %"%220.load" = load i64, ptr %alloca34, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%220.load", ptr @str.const.164)
  %alloca35 = alloca i64, align 8
  %"%220.load36" = load i64, ptr %alloca34, align 8
  store i64 %"%220.load36", ptr %alloca35, align 8
  %"%\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %alloca26, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load", i64 95)
  %"%\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load37" = load i64, ptr %alloca26, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load37", i64 78)
  %"%\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load38" = load i64, ptr %alloca26, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load38", i64 55)
  %"%\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load39" = load i64, ptr %alloca26, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load39", i64 92)
  %"%\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load40" = load i64, ptr %alloca26, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load40", i64 40)
  %"%\D9\85_\D8\B9\D9\84\D9\88\D9\85.load" = load i64, ptr %alloca29, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D8\B9\D9\84\D9\88\D9\85.load", i64 88)
  %"%\D9\85_\D8\B9\D9\84\D9\88\D9\85.load41" = load i64, ptr %alloca29, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D8\B9\D9\84\D9\88\D9\85.load41", i64 91)
  %"%\D9\85_\D8\B9\D9\84\D9\88\D9\85.load42" = load i64, ptr %alloca29, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D8\B9\D9\84\D9\88\D9\85.load42", i64 62)
  %"%\D9\85_\D8\B9\D9\84\D9\88\D9\85.load43" = load i64, ptr %alloca29, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D8\B9\D9\84\D9\88\D9\85.load43", i64 95)
  %"%\D9\85_\D8\B9\D9\84\D9\88\D9\85.load44" = load i64, ptr %alloca29, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D8\B9\D9\84\D9\88\D9\85.load44", i64 35)
  %"%\D9\85_\D9\84\D8\BA\D8\A9.load" = load i64, ptr %alloca32, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D9\84\D8\BA\D8\A9.load", i64 92)
  %"%\D9\85_\D9\84\D8\BA\D8\A9.load45" = load i64, ptr %alloca32, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D9\84\D8\BA\D8\A9.load45", i64 85)
  %"%\D9\85_\D9\84\D8\BA\D8\A9.load46" = load i64, ptr %alloca32, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D9\84\D8\BA\D8\A9.load46", i64 48)
  %"%\D9\85_\D9\84\D8\BA\D8\A9.load47" = load i64, ptr %alloca32, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D9\84\D8\BA\D8\A9.load47", i64 98)
  %"%\D9\85_\D9\84\D8\BA\D8\A9.load48" = load i64, ptr %alloca32, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D9\84\D8\BA\D8\A9.load48", i64 55)
  %"%\D9\85_\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load" = load i64, ptr %alloca35, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load", i64 85)
  %"%\D9\85_\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load49" = load i64, ptr %alloca35, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load49", i64 88)
  %"%\D9\85_\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load50" = load i64, ptr %alloca35, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load50", i64 70)
  %"%\D9\85_\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load51" = load i64, ptr %alloca35, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load51", i64 94)
  %"%\D9\85_\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load52" = load i64, ptr %alloca35, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D9\85_\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load52", i64 42)
  %"%\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load53" = load i64, ptr %alloca26, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\B9\D8\B1\D8\B6_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"(i64 %"%\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load53")
  %"%\D9\85_\D8\B9\D9\84\D9\88\D9\85.load54" = load i64, ptr %alloca29, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\B9\D8\B1\D8\B6_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"(i64 %"%\D9\85_\D8\B9\D9\84\D9\88\D9\85.load54")
  %"%\D9\85_\D9\84\D8\BA\D8\A9.load55" = load i64, ptr %alloca32, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\B9\D8\B1\D8\B6_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"(i64 %"%\D9\85_\D9\84\D8\BA\D8\A9.load55")
  %"%\D9\85_\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load56" = load i64, ptr %alloca35, align 8
  call void @"\D9\85\D8\A7\D8\AF\D8\A9.\D8\B9\D8\B1\D8\B6_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"(i64 %"%\D9\85_\D8\AA\D8\A7\D8\B1\D9\8A\D8\AE.load56")
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.165)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.169)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %alloca57 = alloca i64, align 8
  %alloca58 = alloca i64, align 8
  %"%269.load" = load i64, ptr %alloca57, align 8
  store i64 %"%269.load", ptr %alloca58, align 8
  %alloca59 = alloca i64, align 8
  store i64 0, ptr %alloca59, align 8
  %"%\D8\B71.load60" = load i64, ptr %alloca1, align 8
  %calltmp = call i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A7\D9\84\D9\85\D8\B9\D8\AF\D9\84"(i64 %"%\D8\B71.load60")
  %cmpgetmp = icmp sge i64 %calltmp, 90
  br i1 %cmpgetmp, label %then_54, label %merge_55

then_54:                                          ; preds = %entry
  store i64 1, ptr %alloca59, align 8
  br label %merge_55

merge_55:                                         ; preds = %then_54, %entry
  %"%\D8\B71.load61" = load i64, ptr %alloca1, align 8
  %calltmp62 = call i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D9\86\D8\A7\D8\AC\D8\AD"(i64 %"%\D8\B71.load61")
  %"%\D8\B71.load63" = load i64, ptr %alloca1, align 8
  %calltmp64 = call i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D9\85\D9\86\D8\B0\D8\B1"(i64 %"%\D8\B71.load63")
  %"%\D8\AA\D9\82\D8\B1\D9\8A\D8\B1.load" = load i64, ptr %alloca58, align 8
  %"%\D9\85\D8\AA\D9\81\D9\88\D9\821.load" = load i64, ptr %alloca59, align 8
  call void @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\B5\D9\84.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\B7\D8\A7\D9\84\D8\A8"(i64 %"%\D8\AA\D9\82\D8\B1\D9\8A\D8\B1.load", i64 %calltmp62, i64 %"%\D9\85\D8\AA\D9\81\D9\88\D9\821.load", i64 %calltmp64)
  %alloca65 = alloca i64, align 8
  store i64 0, ptr %alloca65, align 8
  %"%\D8\B72.load66" = load i64, ptr %alloca4, align 8
  %calltmp67 = call i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A7\D9\84\D9\85\D8\B9\D8\AF\D9\84"(i64 %"%\D8\B72.load66")
  %cmpgetmp68 = icmp sge i64 %calltmp67, 90
  br i1 %cmpgetmp68, label %then_56, label %merge_57

then_56:                                          ; preds = %merge_55
  store i64 1, ptr %alloca65, align 8
  br label %merge_57

merge_57:                                         ; preds = %then_56, %merge_55
  %"%\D8\B72.load69" = load i64, ptr %alloca4, align 8
  %calltmp70 = call i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D9\86\D8\A7\D8\AC\D8\AD"(i64 %"%\D8\B72.load69")
  %"%\D8\B72.load71" = load i64, ptr %alloca4, align 8
  %calltmp72 = call i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D9\85\D9\86\D8\B0\D8\B1"(i64 %"%\D8\B72.load71")
  %"%\D8\AA\D9\82\D8\B1\D9\8A\D8\B1.load73" = load i64, ptr %alloca58, align 8
  %"%\D9\85\D8\AA\D9\81\D9\88\D9\822.load" = load i64, ptr %alloca65, align 8
  call void @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\B5\D9\84.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\B7\D8\A7\D9\84\D8\A8"(i64 %"%\D8\AA\D9\82\D8\B1\D9\8A\D8\B1.load73", i64 %calltmp70, i64 %"%\D9\85\D8\AA\D9\81\D9\88\D9\822.load", i64 %calltmp72)
  %alloca74 = alloca i64, align 8
  store i64 0, ptr %alloca74, align 8
  %"%\D8\B73.load75" = load i64, ptr %alloca7, align 8
  %calltmp76 = call i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D9\86\D8\A7\D8\AC\D8\AD"(i64 %"%\D8\B73.load75")
  %"%\D8\B73.load77" = load i64, ptr %alloca7, align 8
  %calltmp78 = call i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D9\85\D9\86\D8\B0\D8\B1"(i64 %"%\D8\B73.load77")
  %"%\D8\AA\D9\82\D8\B1\D9\8A\D8\B1.load79" = load i64, ptr %alloca58, align 8
  %"%\D9\85\D8\AA\D9\81\D9\88\D9\823.load" = load i64, ptr %alloca74, align 8
  call void @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\B5\D9\84.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\B7\D8\A7\D9\84\D8\A8"(i64 %"%\D8\AA\D9\82\D8\B1\D9\8A\D8\B1.load79", i64 %calltmp76, i64 %"%\D9\85\D8\AA\D9\81\D9\88\D9\823.load", i64 %calltmp78)
  %alloca80 = alloca i64, align 8
  store i64 0, ptr %alloca80, align 8
  %"%\D8\B74.load81" = load i64, ptr %alloca10, align 8
  %calltmp82 = call i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A7\D9\84\D9\85\D8\B9\D8\AF\D9\84"(i64 %"%\D8\B74.load81")
  %cmpgetmp83 = icmp sge i64 %calltmp82, 90
  br i1 %cmpgetmp83, label %then_58, label %merge_59

then_58:                                          ; preds = %merge_57
  store i64 1, ptr %alloca80, align 8
  br label %merge_59

merge_59:                                         ; preds = %then_58, %merge_57
  %"%\D8\B74.load84" = load i64, ptr %alloca10, align 8
  %calltmp85 = call i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D9\86\D8\A7\D8\AC\D8\AD"(i64 %"%\D8\B74.load84")
  %"%\D8\B74.load86" = load i64, ptr %alloca10, align 8
  %calltmp87 = call i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D9\85\D9\86\D8\B0\D8\B1"(i64 %"%\D8\B74.load86")
  %"%\D8\AA\D9\82\D8\B1\D9\8A\D8\B1.load88" = load i64, ptr %alloca58, align 8
  %"%\D9\85\D8\AA\D9\81\D9\88\D9\824.load" = load i64, ptr %alloca80, align 8
  call void @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\B5\D9\84.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\B7\D8\A7\D9\84\D8\A8"(i64 %"%\D8\AA\D9\82\D8\B1\D9\8A\D8\B1.load88", i64 %calltmp85, i64 %"%\D9\85\D8\AA\D9\81\D9\88\D9\824.load", i64 %calltmp87)
  %alloca89 = alloca i64, align 8
  store i64 0, ptr %alloca89, align 8
  %"%\D8\B75.load90" = load i64, ptr %alloca13, align 8
  %calltmp91 = call i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D9\86\D8\A7\D8\AC\D8\AD"(i64 %"%\D8\B75.load90")
  %"%\D8\B75.load92" = load i64, ptr %alloca13, align 8
  %calltmp93 = call i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D9\85\D9\86\D8\B0\D8\B1"(i64 %"%\D8\B75.load92")
  %"%\D8\AA\D9\82\D8\B1\D9\8A\D8\B1.load94" = load i64, ptr %alloca58, align 8
  %"%\D9\85\D8\AA\D9\81\D9\88\D9\825.load" = load i64, ptr %alloca89, align 8
  call void @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\B5\D9\84.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D8\B7\D8\A7\D9\84\D8\A8"(i64 %"%\D8\AA\D9\82\D8\B1\D9\8A\D8\B1.load94", i64 %calltmp91, i64 %"%\D9\85\D8\AA\D9\81\D9\88\D9\825.load", i64 %calltmp93)
  %"%\D8\AA\D9\82\D8\B1\D9\8A\D8\B1.load95" = load i64, ptr %alloca58, align 8
  call void @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\81\D8\B5\D9\84.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\AA\D9\82\D8\B1\D9\8A\D8\B1.load95")
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.173)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.177)
  %"%\D8\B71.load96" = load i64, ptr %alloca1, align 8
  %calltmp97 = call i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A3\D8\B9\D9\84\D9\89_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D8\B71.load96")
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %calltmp97)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.66)
  %"%\D8\B71.load98" = load i64, ptr %alloca1, align 8
  %calltmp99 = call i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A3\D8\AF\D9\86\D9\89_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D8\B71.load98")
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %calltmp99)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.185)
  %"%\D8\B74.load100" = load i64, ptr %alloca10, align 8
  %calltmp101 = call i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A3\D8\B9\D9\84\D9\89_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D8\B74.load100")
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %calltmp101)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.66)
  %"%\D8\B74.load102" = load i64, ptr %alloca10, align 8
  %calltmp103 = call i64 @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A3\D8\AF\D9\86\D9\89_\D8\AF\D8\B1\D8\AC\D8\A9"(i64 %"%\D8\B74.load102")
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %calltmp103)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.193)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.197)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
