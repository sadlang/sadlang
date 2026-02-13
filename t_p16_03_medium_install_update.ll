; ModuleID = '03_medium_install_update'
source_filename = "03_medium_install_update"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [151 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [151 x i8] c"\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\00", align 1
@str.const.7 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.11 = private unnamed_addr constant [8 x i8] c"  \F0\9F\93\A6 \00", align 1
@str.const.16 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.20 = private unnamed_addr constant [7 x i8] c"  \E2\96\B8 \00", align 1
@str.const.25 = private unnamed_addr constant [5 x i8] c"    \00", align 1
@str.const.28 = private unnamed_addr constant [3 x i8] c": \00", align 1
@fmt.d = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1
@str.const.40 = private unnamed_addr constant [3 x i8] c"  \00", align 1
@str.const.41 = private unnamed_addr constant [11 x i8] c"\E2\94\9C\E2\94\80\E2\94\80 \00", align 1
@str.const.42 = private unnamed_addr constant [17 x i8] c"\E2\94\82   \E2\94\9C\E2\94\80\E2\94\80 \00", align 1
@str.const.43 = private unnamed_addr constant [23 x i8] c"\E2\94\82   \E2\94\82   \E2\94\94\E2\94\80\E2\94\80 \00", align 1
@str.const.48 = private unnamed_addr constant [2 x i8] c"@\00", align 1
@str.const.53 = private unnamed_addr constant [31 x i8] c"\D8\AD\D9\84 \D8\B4\D8\AC\D8\B1\D8\A9 \D8\A7\D9\84\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.54 = private unnamed_addr constant [23 x i8] c"    \D9\85\D8\B4\D8\B1\D9\88\D8\B9\D9\8A@1.0.0\00", align 1
@str.const.58 = private unnamed_addr constant [15 x i8] c"\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.59 = private unnamed_addr constant [6 x i8] c"2.1.0\00", align 1
@str.const.60 = private unnamed_addr constant [15 x i8] c"\D8\A7\D8\B3\D8\A7\D8\B3\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.61 = private unnamed_addr constant [6 x i8] c"1.0.0\00", align 1
@str.const.62 = private unnamed_addr constant [11 x i8] c"\D8\AB\D9\88\D8\A7\D8\A8\D8\AA\00", align 1
@str.const.64 = private unnamed_addr constant [9 x i8] c"\D8\B4\D8\A8\D9\83\D8\A9\00", align 1
@str.const.65 = private unnamed_addr constant [6 x i8] c"0.9.2\00", align 1
@str.const.66 = private unnamed_addr constant [17 x i8] c"\D8\A8\D8\B1\D9\88\D8\AA\D9\88\D9\83\D9\88\D9\84\00", align 1
@str.const.67 = private unnamed_addr constant [6 x i8] c"1.2.0\00", align 1
@str.const.68 = private unnamed_addr constant [11 x i8] c"\D8\AA\D8\B1\D9\85\D9\8A\D8\B2\00", align 1
@str.const.69 = private unnamed_addr constant [6 x i8] c"3.0.0\00", align 1
@str.const.70 = private unnamed_addr constant [11 x i8] c"\D9\85\D9\84\D9\81\D8\A7\D8\AA\00", align 1
@str.const.72 = private unnamed_addr constant [11 x i8] c"\D9\88\D8\A7\D8\AC\D9\87\D8\A9\00", align 1
@str.const.73 = private unnamed_addr constant [6 x i8] c"3.5.1\00", align 1
@str.const.74 = private unnamed_addr constant [13 x i8] c"\D8\B1\D8\B3\D9\88\D9\85\D8\A7\D8\AA\00", align 1
@str.const.75 = private unnamed_addr constant [6 x i8] c"2.0.0\00", align 1
@str.const.76 = private unnamed_addr constant [11 x i8] c"\D8\A7\D9\84\D9\88\D8\A7\D9\86\00", align 1
@str.const.77 = private unnamed_addr constant [6 x i8] c"1.1.0\00", align 1
@str.const.78 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.82 = private unnamed_addr constant [74 x i8] c"    \F0\9F\93\8A \D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A: 10 \D8\AD\D8\B2\D9\85 (3 \D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9 + 7 \D8\BA\D9\8A\D8\B1 \D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9)\00", align 1
@str.const.86 = private unnamed_addr constant [41 x i8] c"\D9\83\D8\B4\D9\81 \D8\AA\D8\B9\D8\A7\D8\B1\D8\B6\D8\A7\D8\AA \D8\A7\D9\84\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1\D8\A7\D8\AA\00", align 1
@str.const.87 = private unnamed_addr constant [71 x i8] c"    \D8\AA\D8\B1\D9\85\D9\8A\D8\B2: \D8\B4\D8\A8\D9\83\D8\A9 \D8\AA\D8\B7\D9\84\D8\A8 3.0.0\D8\8C \D9\86\D8\B5\D9\88\D8\B5 \D8\AA\D8\B7\D9\84\D8\A8 3.2.0 \E2\86\92 \00", align 1
@str.const.89 = private unnamed_addr constant [16 x i8] c"\E2\9D\8C \D8\AA\D8\B9\D8\A7\D8\B1\D8\B6!\00", align 1
@str.const.93 = private unnamed_addr constant [56 x i8] c"\E2\9C\85 \D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82 (\D9\86\D9\81\D8\B3 \D8\A7\D9\84\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1 \D8\A7\D9\84\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A)\00", align 1
@str.const.97 = private unnamed_addr constant [83 x i8] c"    \D8\A7\D8\B3\D8\A7\D8\B3\D9\8A\D8\A7\D8\AA: \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA \D8\AA\D8\B7\D9\84\D8\A8 1.0.0\D8\8C \D9\88\D8\A7\D8\AC\D9\87\D8\A9 \D8\AA\D8\B7\D9\84\D8\A8 2.0.0 \E2\86\92 \00", align 1
@str.const.99 = private unnamed_addr constant [59 x i8] c"\E2\9D\8C \D8\AA\D8\B9\D8\A7\D8\B1\D8\B6! (\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1\D8\A7\D8\AA \D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9 \D9\85\D8\AE\D8\AA\D9\84\D9\81\D8\A9)\00", align 1
@str.const.103 = private unnamed_addr constant [17 x i8] c"\E2\9C\85 \D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82\00", align 1
@str.const.107 = private unnamed_addr constant [73 x i8] c"    \D9\85\D9\84\D9\81\D8\A7\D8\AA: \D8\B4\D8\A8\D9\83\D8\A9 \D8\AA\D8\B7\D9\84\D8\A8 1.0.0\D8\8C \D9\88\D8\A7\D8\AC\D9\87\D8\A9 \D8\AA\D8\B7\D9\84\D8\A8 1.2.0 \E2\86\92 \00", align 1
@str.const.117 = private unnamed_addr constant [2 x i8] c"[\00", align 1
@str.const.118 = private unnamed_addr constant [3 x i8] c"] \00", align 1
@str.const.119 = private unnamed_addr constant [4 x i8] c"\E2\96\88\00", align 1
@str.const.120 = private unnamed_addr constant [4 x i8] c"\E2\96\91\00", align 1
@str.const.122 = private unnamed_addr constant [20 x i8] c"    \E2\AC\87 \D8\AA\D9\86\D8\B2\D9\8A\D9\84 \00", align 1
@str.const.128 = private unnamed_addr constant [3 x i8] c" (\00", align 1
@str.const.131 = private unnamed_addr constant [19 x i8] c" \D9\83\D9\8A\D9\84\D9\88\D8\A8\D8\A7\D9\8A\D8\AA)\00", align 1
@str.const.135 = private unnamed_addr constant [8 x i8] c"      [\00", align 1
@str.const.139 = private unnamed_addr constant [7 x i8] c"] 100%\00", align 1
@str.const.143 = private unnamed_addr constant [25 x i8] c"    \E2\9C\85 \D8\AA\D9\85 \D8\AA\D8\AB\D8\A8\D9\8A\D8\AA \00", align 1
@str.const.148 = private unnamed_addr constant [28 x i8] c"\D8\AA\D8\AB\D8\A8\D9\8A\D8\AA \D8\A7\D9\84\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.149 = private unnamed_addr constant [42 x i8] c"    \E2\8F\B3 \D8\AC\D8\A7\D8\B1\D9\8A \D8\AD\D9\84 \D8\A7\D9\84\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA...\00", align 1
@str.const.153 = private unnamed_addr constant [47 x i8] c"    \E2\9C\85 \D8\AA\D9\85 \D8\AD\D9\84 \D8\A7\D9\84\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA (10 \D8\AD\D8\B2\D9\85)\00", align 1
@str.const.157 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.163 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.169 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.175 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.180 = private unnamed_addr constant [6 x i8] c"3.2.0\00", align 1
@str.const.181 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.187 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.193 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.199 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.205 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.211 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.217 = private unnamed_addr constant [24 x i8] c"\D9\85\D9\84\D8\AE\D8\B5 \D8\A7\D9\84\D8\AA\D8\AB\D8\A8\D9\8A\D8\AA\00", align 1
@str.const.218 = private unnamed_addr constant [18 x i8] c"\D8\AD\D8\B2\D9\85 \D9\85\D8\AB\D8\A8\D8\AA\D8\A9\00", align 1
@str.const.219 = private unnamed_addr constant [39 x i8] c"\D8\AD\D8\AC\D9\85 \D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A (\D9\83\D9\8A\D9\84\D9\88\D8\A8\D8\A7\D9\8A\D8\AA)\00", align 1
@str.const.220 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D8\A9\00", align 1
@str.const.221 = private unnamed_addr constant [26 x i8] c"\E2\9C\85 \D8\A7\D9\83\D8\AA\D9\85\D9\84 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD\00", align 1
@str.const.222 = private unnamed_addr constant [16 x i8] c"\D9\84\D8\A7 \D8\AA\D8\AD\D8\AF\D9\8A\D8\AB\00", align 1
@str.const.223 = private unnamed_addr constant [26 x i8] c"\E2\AC\86 \D8\AA\D8\AD\D8\AF\D9\8A\D8\AB \D9\85\D8\AA\D9\88\D9\81\D8\B1\00", align 1
@str.const.224 = private unnamed_addr constant [15 x i8] c"\E2\9C\85 \D9\85\D8\AD\D8\AF\D8\AB\D8\A9\00", align 1
@str.const.225 = private unnamed_addr constant [43 x i8] c"\D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\AA\D9\88\D9\81\D8\B1\D8\A9\00", align 1
@str.const.226 = private unnamed_addr constant [53 x i8] c"    \E2\8F\B3 \D8\AC\D8\A7\D8\B1\D9\8A \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\B3\D8\AC\D9\84 \D8\A7\D9\84\D9\85\D8\B1\D9\83\D8\B2\D9\8A...\00", align 1
@str.const.230 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.234 = private unnamed_addr constant [21 x i8] c"\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA 2.1.0\00", align 1
@str.const.235 = private unnamed_addr constant [15 x i8] c"\D8\B4\D8\A8\D9\83\D8\A9 0.9.2\00", align 1
@str.const.236 = private unnamed_addr constant [17 x i8] c"\D9\88\D8\A7\D8\AC\D9\87\D8\A9 3.5.1\00", align 1
@str.const.237 = private unnamed_addr constant [17 x i8] c"\D8\AA\D8\B1\D9\85\D9\8A\D8\B2 3.2.0\00", align 1
@str.const.238 = private unnamed_addr constant [19 x i8] c"\D8\B1\D8\B3\D9\88\D9\85\D8\A7\D8\AA 2.0.0\00", align 1
@str.const.239 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.243 = private unnamed_addr constant [28 x i8] c"\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB\D8\A7\D8\AA \D9\85\D8\AA\D9\88\D9\81\D8\B1\D8\A9\00", align 1
@str.const.244 = private unnamed_addr constant [44 x i8] c"    \F0\9F\92\A1 \D8\B4\D8\BA\D9\91\D9\84: \D8\B5 \D8\AA\D8\AD\D8\AF\D9\8A\D8\AB --\D8\A7\D9\84\D9\83\D9\84\00", align 1
@str.const.248 = private unnamed_addr constant [157 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.252 = private unnamed_addr constant [77 x i8] c"\E2\95\91  \F0\9F\93\A6 \D9\85\D8\AF\D9\8A\D8\B1 \D8\AD\D8\B2\D9\85 \D8\B5 - \D8\AA\D8\AB\D8\A8\D9\8A\D8\AA \D9\88\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB                   \E2\95\91\00", align 1
@str.const.256 = private unnamed_addr constant [157 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.260 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.264 = private unnamed_addr constant [48 x i8] c"  \E2\9C\85 \D8\A7\D9\83\D8\AA\D9\85\D9\84 \D9\85\D8\AF\D9\8A\D8\B1 \D8\A7\D9\84\D8\AD\D8\B2\D9\85 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD\00", align 1

define i64 @"\D8\AE\D8\B7_\D9\85\D8\B2\D8\AF\D9\88\D8\AC"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\AE\D8\B7_\D8\A8\D8\B3\D9\8A\D8\B7"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\82\D8\B3\D9\85"(ptr %"\D8\B9") {
entry:
  %"\D8\B91" = alloca ptr, align 8
  store ptr %"\D8\B9", ptr %"\D8\B91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\AE\D8\B7_\D9\85\D8\B2\D8\AF\D9\88\D8\AC"()
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.11)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\B91")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp2 = call i64 @"\D8\AE\D8\B7_\D9\85\D8\B2\D8\AF\D9\88\D8\AC"()
  ret i64 0
}

define i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D8\B1\D8\B9\D9\8A"(ptr %"\D8\B9") {
entry:
  %"\D8\B91" = alloca ptr, align 8
  store ptr %"\D8\B9", ptr %"\D8\B91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.16)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\AE\D8\B7_\D8\A8\D8\B3\D9\8A\D8\B7"()
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.20)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\B91")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp2 = call i64 @"\D8\AE\D8\B7_\D8\A8\D8\B3\D9\8A\D8\B7"()
  ret i64 0
}

define i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr %"\D9\88\D8\B5\D9\81", ptr %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"\D9\82\D9\8A\D9\85\D8\A92" = alloca ptr, align 8
  store ptr %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %"\D9\88\D8\B5\D9\811" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81", ptr %"\D9\88\D8\B5\D9\811", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.25)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\88\D8\B5\D9\811")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.28)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\82\D9\8A\D9\85\D8\A92")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A8\D9\8A\D8\A7\D9\86_\D8\B1\D9\82\D9\85\D9\8A"(ptr %"\D9\88\D8\B5\D9\81", i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"\D9\82\D9\8A\D9\85\D8\A92" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %"\D9\88\D8\B5\D9\811" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81", ptr %"\D9\88\D8\B5\D9\811", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.25)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\88\D8\B5\D9\811")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.28)
  %"%\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define ptr @"\D9\85\D8\B3\D8\AA\D9\88\D9\89_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89") {
entry:
  %"\D9\85\D8\B3\D8\AA\D9\88\D9\891" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %alloca = alloca ptr, align 8
  store ptr @str.const.40, ptr %alloca, align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load", 0
  br i1 %cmpeqtmp, label %then_0, label %else_2

then_0:                                           ; preds = %entry
  store ptr @str.const.41, ptr %alloca, align 8
  br label %merge_1

else_2:                                           ; preds = %entry
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load2" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load2", 1
  br i1 %cmpeqtmp3, label %then_3, label %else_5

merge_1:                                          ; preds = %merge_4, %then_0
  %loadtmp = load ptr, ptr %alloca, align 8
  ret ptr %loadtmp

then_3:                                           ; preds = %else_2
  store ptr @str.const.42, ptr %alloca, align 8
  br label %merge_4

else_5:                                           ; preds = %else_2
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load4" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load4", 2
  br i1 %cmpeqtmp5, label %then_6, label %merge_7

merge_4:                                          ; preds = %merge_7, %then_3
  br label %merge_1

then_6:                                           ; preds = %else_5
  store ptr @str.const.43, ptr %alloca, align 8
  br label %merge_7

merge_7:                                          ; preds = %then_6, %else_5
  br label %merge_4
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(ptr %"\D8\AD\D8\B2\D9\85\D8\A9", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89") {
entry:
  %"\D9\85\D8\B3\D8\AA\D9\88\D9\893" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\893", align 8
  %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12", align 8
  %"\D8\AD\D8\B2\D9\85\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\AD\D8\B2\D9\85\D8\A9", ptr %"\D8\AD\D8\B2\D9\85\D8\A91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.25)
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\893", align 8
  %calltmp = call ptr @"\D9\85\D8\B3\D8\AA\D9\88\D9\89_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load")
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\AD\D8\B2\D9\85\D8\A91")
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.48)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12")
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\AD\D9\84_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\82\D8\B3\D9\85"(ptr @str.const.53)
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.54)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp1 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(ptr @str.const.58, ptr @str.const.59, i64 0)
  %calltmp2 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(ptr @str.const.60, ptr @str.const.61, i64 1)
  %calltmp3 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(ptr @str.const.62, ptr @str.const.61, i64 2)
  %calltmp4 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(ptr @str.const.64, ptr @str.const.65, i64 0)
  %calltmp5 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(ptr @str.const.66, ptr @str.const.67, i64 1)
  %calltmp6 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(ptr @str.const.68, ptr @str.const.69, i64 2)
  %calltmp7 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(ptr @str.const.70, ptr @str.const.61, i64 1)
  %calltmp8 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(ptr @str.const.72, ptr @str.const.73, i64 0)
  %calltmp9 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(ptr @str.const.74, ptr @str.const.75, i64 1)
  %calltmp10 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(ptr @str.const.76, ptr @str.const.77, i64 2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.78)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.82)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\83\D8\B4\D9\81_\D8\AA\D8\B9\D8\A7\D8\B1\D8\B6"(i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12") {
entry:
  %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B122" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B122", align 8
  %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B111" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B111", align 8
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11.load" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B111", align 8
  %divtmp = sdiv i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11.load", 10000
  %alloca = alloca i64, align 8
  store i64 %divtmp, ptr %alloca, align 8
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12.load" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B122", align 8
  %divtmp3 = sdiv i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12.load", 10000
  %alloca4 = alloca i64, align 8
  store i64 %divtmp3, ptr %alloca4, align 8
  %alloca5 = alloca i64, align 8
  store i64 0, ptr %alloca5, align 8
  %"%\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A1.load" = load i64, ptr %alloca, align 8
  %"%\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A2.load" = load i64, ptr %alloca4, align 8
  %cmpnetmp = icmp ne i64 %"%\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A1.load", %"%\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A2.load"
  br i1 %cmpnetmp, label %then_8, label %merge_9

then_8:                                           ; preds = %entry
  store i64 1, ptr %alloca5, align 8
  br label %merge_9

merge_9:                                          ; preds = %then_8, %entry
  %loadtmp = load i64, ptr %alloca5, align 8
  ret i64 %loadtmp
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\AA\D8\B9\D8\A7\D8\B1\D8\B6\D8\A7\D8\AA"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D8\B1\D8\B9\D9\8A"(ptr @str.const.86)
  %alloca = alloca i64, align 8
  store i64 30000, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 30200, ptr %alloca1, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.87)
  %"%\D8\AA\D8\B1\D9\85\D9\8A\D8\B2_\D9\85\D9\86_\D8\B4\D8\A8\D9\83\D8\A9.load" = load i64, ptr %alloca, align 8
  %"%\D8\AA\D8\B1\D9\85\D9\8A\D8\B2_\D9\85\D9\86_\D9\86\D8\B5\D9\88\D8\B5.load" = load i64, ptr %alloca1, align 8
  %calltmp2 = call i64 @"\D9\83\D8\B4\D9\81_\D8\AA\D8\B9\D8\A7\D8\B1\D8\B6"(i64 %"%\D8\AA\D8\B1\D9\85\D9\8A\D8\B2_\D9\85\D9\86_\D8\B4\D8\A8\D9\83\D8\A9.load", i64 %"%\D8\AA\D8\B1\D9\85\D9\8A\D8\B2_\D9\85\D9\86_\D9\86\D8\B5\D9\88\D8\B5.load")
  %cmpeqtmp = icmp eq i64 %calltmp2, 1
  br i1 %cmpeqtmp, label %then_10, label %else_12

then_10:                                          ; preds = %entry
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.89)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_11

else_12:                                          ; preds = %entry
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.93)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_11

merge_11:                                         ; preds = %else_12, %then_10
  %alloca3 = alloca i64, align 8
  store i64 10000, ptr %alloca3, align 8
  %alloca4 = alloca i64, align 8
  store i64 20000, ptr %alloca4, align 8
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.97)
  %"%\D8\A7\D8\B3\D8\A7\D8\B3\D9\8A\D8\A7\D8\AA_\D9\85\D9\86_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %alloca3, align 8
  %"%\D8\A7\D8\B3\D8\A7\D8\B3\D9\8A\D8\A7\D8\AA_\D9\85\D9\86_\D9\88\D8\A7\D8\AC\D9\87\D8\A9.load" = load i64, ptr %alloca4, align 8
  %calltmp5 = call i64 @"\D9\83\D8\B4\D9\81_\D8\AA\D8\B9\D8\A7\D8\B1\D8\B6"(i64 %"%\D8\A7\D8\B3\D8\A7\D8\B3\D9\8A\D8\A7\D8\AA_\D9\85\D9\86_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load", i64 %"%\D8\A7\D8\B3\D8\A7\D8\B3\D9\8A\D8\A7\D8\AA_\D9\85\D9\86_\D9\88\D8\A7\D8\AC\D9\87\D8\A9.load")
  %cmpeqtmp6 = icmp eq i64 %calltmp5, 1
  br i1 %cmpeqtmp6, label %then_13, label %else_15

then_13:                                          ; preds = %merge_11
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.99)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_14

else_15:                                          ; preds = %merge_11
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.103)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_14

merge_14:                                         ; preds = %else_15, %then_13
  %alloca7 = alloca i64, align 8
  store i64 10000, ptr %alloca7, align 8
  %alloca8 = alloca i64, align 8
  store i64 10200, ptr %alloca8, align 8
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.107)
  %"%\D9\85\D9\84\D9\81\D8\A7\D8\AA_\D9\85\D9\86_\D8\B4\D8\A8\D9\83\D8\A9.load" = load i64, ptr %alloca7, align 8
  %"%\D9\85\D9\84\D9\81\D8\A7\D8\AA_\D9\85\D9\86_\D9\88\D8\A7\D8\AC\D9\87\D8\A9.load" = load i64, ptr %alloca8, align 8
  %calltmp9 = call i64 @"\D9\83\D8\B4\D9\81_\D8\AA\D8\B9\D8\A7\D8\B1\D8\B6"(i64 %"%\D9\85\D9\84\D9\81\D8\A7\D8\AA_\D9\85\D9\86_\D8\B4\D8\A8\D9\83\D8\A9.load", i64 %"%\D9\85\D9\84\D9\81\D8\A7\D8\AA_\D9\85\D9\86_\D9\88\D8\A7\D8\AC\D9\87\D8\A9.load")
  %cmpeqtmp10 = icmp eq i64 %calltmp9, 1
  br i1 %cmpeqtmp10, label %then_16, label %else_18

then_16:                                          ; preds = %merge_14
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.89)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_17

else_18:                                          ; preds = %merge_14
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.93)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_17

merge_17:                                         ; preds = %else_18, %then_16
  ret i64 0
}

define ptr @"\D8\B4\D8\B1\D9\8A\D8\B7_\D8\AA\D9\82\D8\AF\D9\85"(i64 %"\D9\86\D8\B3\D8\A8\D8\A9") {
entry:
  %"\D9\86\D8\B3\D8\A8\D8\A91" = alloca i64, align 8
  store i64 %"\D9\86\D8\B3\D8\A8\D8\A9", ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %alloca = alloca ptr, align 8
  store ptr @str.const.117, ptr %alloca, align 8
  %alloca2 = alloca i64, align 8
  store i64 0, ptr %alloca2, align 8
  br label %while_cond_19

while_cond_19:                                    ; preds = %merge_23, %entry
  %"%\D8\B9\D8\AF.load" = load i64, ptr %alloca2, align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9\D8\AF.load", 20
  br i1 %cmplttmp, label %while_body_20, label %while_exit_21

while_body_20:                                    ; preds = %while_cond_19
  %"%\D8\B9\D8\AF.load3" = load i64, ptr %alloca2, align 8
  %multmp = mul i64 %"%\D8\B9\D8\AF.load3", 5
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmplttmp4 = icmp slt i64 %multmp, %"%\D9\86\D8\B3\D8\A8\D8\A9.load"
  br i1 %cmplttmp4, label %then_22, label %else_24

while_exit_21:                                    ; preds = %while_cond_19
  %alloca5 = alloca ptr, align 8
  store ptr @str.const.118, ptr %alloca5, align 8
  %loadtmp = load ptr, ptr %alloca5, align 8
  ret ptr %loadtmp

then_22:                                          ; preds = %while_body_20
  store ptr @str.const.119, ptr %alloca, align 8
  br label %merge_23

else_24:                                          ; preds = %while_body_20
  store ptr @str.const.120, ptr %alloca, align 8
  br label %merge_23

merge_23:                                         ; preds = %else_24, %then_22
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca)
  %"%\D8\B9\D8\AF.load6" = load i64, ptr %alloca2, align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF.load6", 1
  store i64 %addtmp, ptr %alloca2, align 8
  br label %while_cond_19
}

define i64 @"\D8\AA\D9\86\D8\B2\D9\8A\D9\84_\D8\AD\D8\B2\D9\85\D8\A9"(ptr %"\D8\AD\D8\B2\D9\85\D8\A9", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", i64 %"\D8\AD\D8\AC\D9\85_\D9\85\D9\84\D9\81") {
entry:
  %"\D8\AD\D8\AC\D9\85_\D9\85\D9\84\D9\813" = alloca i64, align 8
  store i64 %"\D8\AD\D8\AC\D9\85_\D9\85\D9\84\D9\81", ptr %"\D8\AD\D8\AC\D9\85_\D9\85\D9\84\D9\813", align 8
  %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12", align 8
  %"\D8\AD\D8\B2\D9\85\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\AD\D8\B2\D9\85\D8\A9", ptr %"\D8\AD\D8\B2\D9\85\D8\A91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.122)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\AD\D8\B2\D9\85\D8\A91")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.48)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.128)
  %"%\D8\AD\D8\AC\D9\85_\D9\85\D9\84\D9\81.load" = load i64, ptr %"\D8\AD\D8\AC\D9\85_\D9\85\D9\84\D9\813", align 8
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AD\D8\AC\D9\85_\D9\85\D9\84\D9\81.load")
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.131)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.135)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  br label %while_cond_25

while_cond_25:                                    ; preds = %while_body_26, %entry
  %"%\D8\AE\D8\B7\D9\88\D8\A9.load" = load i64, ptr %alloca, align 8
  %cmplttmp = icmp slt i64 %"%\D8\AE\D8\B7\D9\88\D8\A9.load", 20
  br i1 %cmplttmp, label %while_body_26, label %while_exit_27

while_body_26:                                    ; preds = %while_cond_25
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.119)
  %"%\D8\AE\D8\B7\D9\88\D8\A9.load4" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\AE\D8\B7\D9\88\D8\A9.load4", 1
  store i64 %addtmp, ptr %alloca, align 8
  br label %while_cond_25

while_exit_27:                                    ; preds = %while_cond_25
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.139)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.143)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\AD\D8\B2\D9\85\D8\A91")
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\AA\D8\AB\D8\A8\D9\8A\D8\AA"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\82\D8\B3\D9\85"(ptr @str.const.148)
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.149)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.153)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.157)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp1 = call i64 @"\D8\AA\D9\86\D8\B2\D9\8A\D9\84_\D8\AD\D8\B2\D9\85\D8\A9"(ptr @str.const.62, ptr @str.const.61, i64 12)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.163)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp2 = call i64 @"\D8\AA\D9\86\D8\B2\D9\8A\D9\84_\D8\AD\D8\B2\D9\85\D8\A9"(ptr @str.const.60, ptr @str.const.61, i64 45)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.169)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp3 = call i64 @"\D8\AA\D9\86\D8\B2\D9\8A\D9\84_\D8\AD\D8\B2\D9\85\D8\A9"(ptr @str.const.58, ptr @str.const.59, i64 128)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.175)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp4 = call i64 @"\D8\AA\D9\86\D8\B2\D9\8A\D9\84_\D8\AD\D8\B2\D9\85\D8\A9"(ptr @str.const.68, ptr @str.const.180, i64 67)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.181)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp5 = call i64 @"\D8\AA\D9\86\D8\B2\D9\8A\D9\84_\D8\AD\D8\B2\D9\85\D8\A9"(ptr @str.const.66, ptr @str.const.67, i64 89)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.187)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp6 = call i64 @"\D8\AA\D9\86\D8\B2\D9\8A\D9\84_\D8\AD\D8\B2\D9\85\D8\A9"(ptr @str.const.70, ptr @str.const.67, i64 34)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.193)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp7 = call i64 @"\D8\AA\D9\86\D8\B2\D9\8A\D9\84_\D8\AD\D8\B2\D9\85\D8\A9"(ptr @str.const.64, ptr @str.const.65, i64 256)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.199)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp8 = call i64 @"\D8\AA\D9\86\D8\B2\D9\8A\D9\84_\D8\AD\D8\B2\D9\85\D8\A9"(ptr @str.const.76, ptr @str.const.77, i64 23)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.205)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp9 = call i64 @"\D8\AA\D9\86\D8\B2\D9\8A\D9\84_\D8\AD\D8\B2\D9\85\D8\A9"(ptr @str.const.74, ptr @str.const.75, i64 178)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.211)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp10 = call i64 @"\D8\AA\D9\86\D8\B2\D9\8A\D9\84_\D8\AD\D8\B2\D9\85\D8\A9"(ptr @str.const.72, ptr @str.const.73, i64 345)
  %calltmp11 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D8\B1\D8\B9\D9\8A"(ptr @str.const.217)
  %calltmp12 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.218, i64 10)
  %calltmp13 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.219, i64 1177)
  %calltmp14 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.220, ptr @str.const.221)
  ret i64 0
}

define ptr @"\D8\AD\D8\A7\D9\84\D8\A9_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB"(i64 %"\D9\82\D8\AF\D9\8A\D9\85", i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\AC\D8\AF\D9\8A\D8\AF") {
entry:
  %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\AC\D8\AF\D9\8A\D8\AF2" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\AC\D8\AF\D9\8A\D8\AF", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\AC\D8\AF\D9\8A\D8\AF2", align 8
  %"\D9\82\D8\AF\D9\8A\D9\851" = alloca i64, align 8
  store i64 %"\D9\82\D8\AF\D9\8A\D9\85", ptr %"\D9\82\D8\AF\D9\8A\D9\851", align 8
  %alloca = alloca ptr, align 8
  store ptr @str.const.222, ptr %alloca, align 8
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\AC\D8\AF\D9\8A\D8\AF.load" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\AC\D8\AF\D9\8A\D8\AF2", align 8
  %"%\D9\82\D8\AF\D9\8A\D9\85.load" = load i64, ptr %"\D9\82\D8\AF\D9\8A\D9\851", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\AC\D8\AF\D9\8A\D8\AF.load", %"%\D9\82\D8\AF\D9\8A\D9\85.load"
  br i1 %cmpgttmp, label %then_28, label %else_30

then_28:                                          ; preds = %entry
  store ptr @str.const.223, ptr %alloca, align 8
  br label %merge_29

else_30:                                          ; preds = %entry
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\AC\D8\AF\D9\8A\D8\AF.load3" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\AC\D8\AF\D9\8A\D8\AF2", align 8
  %"%\D9\82\D8\AF\D9\8A\D9\85.load4" = load i64, ptr %"\D9\82\D8\AF\D9\8A\D9\851", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\AC\D8\AF\D9\8A\D8\AF.load3", %"%\D9\82\D8\AF\D9\8A\D9\85.load4"
  br i1 %cmpeqtmp, label %then_31, label %merge_32

merge_29:                                         ; preds = %merge_32, %then_28
  %loadtmp = load ptr, ptr %alloca, align 8
  ret ptr %loadtmp

then_31:                                          ; preds = %else_30
  store ptr @str.const.224, ptr %alloca, align 8
  br label %merge_32

merge_32:                                         ; preds = %then_31, %else_30
  br label %merge_29
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB\D8\A7\D8\AA"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\82\D8\B3\D9\85"(ptr @str.const.225)
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.226)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.230)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp1 = call ptr @"\D8\AD\D8\A7\D9\84\D8\A9_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB"(i64 20100, i64 20300)
  %calltmp2 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.234, ptr %calltmp1)
  %calltmp3 = call ptr @"\D8\AD\D8\A7\D9\84\D8\A9_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB"(i64 902, i64 1000)
  %calltmp4 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.235, ptr %calltmp3)
  %calltmp5 = call ptr @"\D8\AD\D8\A7\D9\84\D8\A9_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB"(i64 30501, i64 30501)
  %calltmp6 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.236, ptr %calltmp5)
  %calltmp7 = call ptr @"\D8\AD\D8\A7\D9\84\D8\A9_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB"(i64 30200, i64 30200)
  %calltmp8 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.237, ptr %calltmp7)
  %calltmp9 = call ptr @"\D8\AD\D8\A7\D9\84\D8\A9_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB"(i64 20000, i64 20100)
  %calltmp10 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.238, ptr %calltmp9)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.239)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp11 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.243, i64 3)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.244)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.248)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.252)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.256)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\AD\D9\84_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA"()
  %calltmp1 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\AA\D8\B9\D8\A7\D8\B1\D8\B6\D8\A7\D8\AA"()
  %calltmp2 = call i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\AA\D8\AB\D8\A8\D9\8A\D8\AA"()
  %calltmp3 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB\D8\A7\D8\AA"()
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.260)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp4 = call i64 @"\D8\AE\D8\B7_\D9\85\D8\B2\D8\AF\D9\88\D8\AC"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.264)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp5 = call i64 @"\D8\AE\D8\B7_\D9\85\D8\B2\D8\AF\D9\88\D8\AC"()
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
