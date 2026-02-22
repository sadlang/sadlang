; ModuleID = '03_medium_resource_management'
source_filename = "03_medium_resource_management"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [28 x i8] c"  [+] \D8\A5\D9\86\D8\B4\D8\A7\D8\A1 \D9\85\D9\88\D8\B1\D8\AF #\00", align 1
@str.const.1 = private unnamed_addr constant [3 x i8] c": \00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.2 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.4 = private unnamed_addr constant [28 x i8] c"  [-] \D8\AA\D8\AD\D8\B1\D9\8A\D8\B1 \D9\85\D9\88\D8\B1\D8\AF #\00", align 1
@str.const.9 = private unnamed_addr constant [32 x i8] c"  [~] \D8\A7\D8\B3\D8\AA\D8\AE\D8\AF\D8\A7\D9\85 \D9\85\D9\88\D8\B1\D8\AF #\00", align 1
@str.const.14 = private unnamed_addr constant [27 x i8] c"  [!!] \D8\AE\D8\B7\D8\A3 \D8\A5\D8\AF\D8\A7\D8\B1\D8\A9: \00", align 1
@str.const.18 = private unnamed_addr constant [45 x i8] c"=== \D8\A5\D8\AF\D8\A7\D8\B1\D8\A9 \D9\85\D9\88\D8\A7\D8\B1\D8\AF \D8\A7\D9\84\D9\85\D9\84\D9\81\D8\A7\D8\AA ===\00", align 1
@str.const.22 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.26 = private unnamed_addr constant [2 x i8] c"1\00", align 1
@str.const.27 = private unnamed_addr constant [17 x i8] c"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.txt\00", align 1
@str.const.28 = private unnamed_addr constant [9 x i8] c"\D9\85\D9\84\D9\81: \00", align 1
@str.const.29 = private unnamed_addr constant [22 x i8] c"\D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D9\85\D8\AD\D8\AA\D9\88\D9\89\00", align 1
@str.const.30 = private unnamed_addr constant [27 x i8] c"\D9\83\D8\AA\D8\A7\D8\A8\D8\A9 \D8\B3\D8\B7\D8\B1 \D8\AC\D8\AF\D9\8A\D8\AF\00", align 1
@str.const.31 = private unnamed_addr constant [26 x i8] c"\D8\AD\D9\81\D8\B8 \D8\A7\D9\84\D8\AA\D8\BA\D9\8A\D9\8A\D8\B1\D8\A7\D8\AA\00", align 1
@str.const.33 = private unnamed_addr constant [46 x i8] c"  \D8\A7\D9\84\D9\85\D9\84\D9\81 \D9\85\D8\BA\D9\84\D9\82 \D8\AA\D9\84\D9\82\D8\A7\D8\A6\D9\8A\D8\A7\D9\8B (RAII)\00", align 1
@str.const.37 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.41 = private unnamed_addr constant [19 x i8] c"  \D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9: \00", align 1
@str.const.42 = private unnamed_addr constant [2 x i8] c"/\00", align 1
@str.const.43 = private unnamed_addr constant [13 x i8] c" (\D9\85\D8\AA\D8\A7\D8\AD: \00", align 1
@str.const.44 = private unnamed_addr constant [2 x i8] c")\00", align 1
@str.ret = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.47 = private unnamed_addr constant [45 x i8] c"=== \D8\A5\D8\AF\D8\A7\D8\B1\D8\A9 \D9\85\D9\88\D8\A7\D8\B1\D8\AF \D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9 ===\00", align 1
@str.const.51 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.55 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.59 = private unnamed_addr constant [2 x i8] c"2\00", align 1
@str.const.60 = private unnamed_addr constant [33 x i8] c"\D9\83\D8\AA\D9\84\D8\A9 \D8\B0\D8\A7\D9\83\D8\B1\D8\A9 256 \D8\A8\D8\A7\D9\8A\D8\AA\00", align 1
@str.const.61 = private unnamed_addr constant [2 x i8] c"3\00", align 1
@str.const.62 = private unnamed_addr constant [33 x i8] c"\D9\83\D8\AA\D9\84\D8\A9 \D8\B0\D8\A7\D9\83\D8\B1\D8\A9 512 \D8\A8\D8\A7\D9\8A\D8\AA\00", align 1
@str.const.63 = private unnamed_addr constant [2 x i8] c"4\00", align 1
@str.const.64 = private unnamed_addr constant [33 x i8] c"\D9\83\D8\AA\D9\84\D8\A9 \D8\B0\D8\A7\D9\83\D8\B1\D8\A9 128 \D8\A8\D8\A7\D9\8A\D8\AA\00", align 1
@str.const.65 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.69 = private unnamed_addr constant [32 x i8] c"  \D8\AA\D8\AD\D8\B1\D9\8A\D8\B1 \D8\A8\D8\AA\D8\B1\D8\AA\D9\8A\D8\A8 LIFO:\00", align 1
@str.const.74 = private unnamed_addr constant [22 x i8] c"\D9\83\D8\AA\D9\84\D8\A9 128 \D8\A8\D8\A7\D9\8A\D8\AA\00", align 1
@str.const.76 = private unnamed_addr constant [22 x i8] c"\D9\83\D8\AA\D9\84\D8\A9 512 \D8\A8\D8\A7\D9\8A\D8\AA\00", align 1
@str.const.78 = private unnamed_addr constant [22 x i8] c"\D9\83\D8\AA\D9\84\D8\A9 256 \D8\A8\D8\A7\D9\8A\D8\AA\00", align 1
@str.const.79 = private unnamed_addr constant [37 x i8] c"  \D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D9\84\D9\85\D9\88\D8\A7\D8\B1\D8\AF \D9\85\D8\AD\D8\B1\D8\B1\D8\A9\00", align 1
@str.const.83 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.87 = private unnamed_addr constant [54 x i8] c"=== \D9\85\D8\AC\D9\85\D8\B9 \D8\A7\D9\84\D8\A7\D8\AA\D8\B5\D8\A7\D9\84\D8\A7\D8\AA (Connection Pool) ===\00", align 1
@str.const.91 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.95 = private unnamed_addr constant [24 x i8] c"  \D8\B3\D8\B9\D8\A9 \D8\A7\D9\84\D9\85\D8\AC\D9\85\D8\B9: \00", align 1
@str.const.96 = private unnamed_addr constant [16 x i8] c" \D8\A7\D8\AA\D8\B5\D8\A7\D9\84\D8\A7\D8\AA\00", align 1
@str.const.99 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.103 = private unnamed_addr constant [2 x i8] c"5\00", align 1
@str.const.104 = private unnamed_addr constant [38 x i8] c"\D8\A7\D8\AA\D8\B5\D8\A7\D9\84 \D9\82\D8\A7\D8\B9\D8\AF\D8\A9 \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA #1\00", align 1
@str.const.105 = private unnamed_addr constant [28 x i8] c"  \D8\A7\D8\AA\D8\B5\D8\A7\D9\84\D8\A7\D8\AA \D9\86\D8\B4\D8\B7\D8\A9: \00", align 1
@str.const.108 = private unnamed_addr constant [2 x i8] c"6\00", align 1
@str.const.109 = private unnamed_addr constant [38 x i8] c"\D8\A7\D8\AA\D8\B5\D8\A7\D9\84 \D9\82\D8\A7\D8\B9\D8\AF\D8\A9 \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA #2\00", align 1
@str.const.113 = private unnamed_addr constant [2 x i8] c"7\00", align 1
@str.const.114 = private unnamed_addr constant [38 x i8] c"\D8\A7\D8\AA\D8\B5\D8\A7\D9\84 \D9\82\D8\A7\D8\B9\D8\AF\D8\A9 \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA #3\00", align 1
@str.const.118 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.122 = private unnamed_addr constant [2 x i8] c"8\00", align 1
@str.const.123 = private unnamed_addr constant [14 x i8] c"\D8\A7\D8\AA\D8\B5\D8\A7\D9\84 #4\00", align 1
@str.const.124 = private unnamed_addr constant [60 x i8] c"\D8\A7\D9\84\D9\85\D8\AC\D9\85\D8\B9 \D9\85\D9\85\D8\AA\D9\84\D8\A6! \D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1 \D8\AA\D8\AD\D8\B1\D9\8A\D8\B1 \D8\A7\D8\AA\D8\B5\D8\A7\D9\84\00", align 1
@str.const.125 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.130 = private unnamed_addr constant [14 x i8] c"\D8\A7\D8\AA\D8\B5\D8\A7\D9\84 #1\00", align 1
@str.const.135 = private unnamed_addr constant [38 x i8] c"\D8\A7\D8\AA\D8\B5\D8\A7\D9\84 #4 (\D8\A8\D8\B9\D8\AF \D8\A7\D9\84\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1)\00", align 1
@str.const.139 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.143 = private unnamed_addr constant [42 x i8] c"  \D8\AA\D8\AD\D8\B1\D9\8A\D8\B1 \D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D9\84\D8\A7\D8\AA\D8\B5\D8\A7\D9\84\D8\A7\D8\AA:\00", align 1
@str.const.150 = private unnamed_addr constant [14 x i8] c"\D8\A7\D8\AA\D8\B5\D8\A7\D9\84 #3\00", align 1
@str.const.152 = private unnamed_addr constant [14 x i8] c"\D8\A7\D8\AA\D8\B5\D8\A7\D9\84 #2\00", align 1
@str.const.156 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.160 = private unnamed_addr constant [50 x i8] c"=== \D8\AD\D8\B1\D8\A7\D8\B3\D8\A9 \D8\A7\D9\84\D9\85\D9\88\D8\A7\D8\B1\D8\AF (Guard Pattern) ===\00", align 1
@str.const.164 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.168 = private unnamed_addr constant [33 x i8] c"  \D8\A5\D9\86\D8\B4\D8\A7\D8\A1 \D8\AD\D8\A7\D8\B1\D8\B3 \D8\A7\D9\84\D9\82\D9\81\D9\84\00", align 1
@str.const.172 = private unnamed_addr constant [2 x i8] c"9\00", align 1
@str.const.173 = private unnamed_addr constant [13 x i8] c"\D9\82\D9\81\D9\84 mutex\00", align 1
@str.const.174 = private unnamed_addr constant [35 x i8] c"  \D8\A7\D9\84\D9\82\D8\B3\D9\85 \D8\A7\D9\84\D8\AD\D8\B1\D8\AC \D9\85\D8\A4\D9\85\D9\91\D9\86\00", align 1
@str.const.179 = private unnamed_addr constant [37 x i8] c"\D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA \D9\85\D8\B4\D8\AA\D8\B1\D9\83\D8\A9\00", align 1
@str.const.181 = private unnamed_addr constant [22 x i8] c"\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB \D8\B9\D8\AF\D9\91\D8\A7\D8\AF\00", align 1
@str.const.183 = private unnamed_addr constant [22 x i8] c"\D9\83\D8\AA\D8\A7\D8\A8\D8\A9 \D9\86\D8\AA\D9\8A\D8\AC\D8\A9\00", align 1
@str.const.186 = private unnamed_addr constant [50 x i8] c"  \D8\A7\D9\84\D9\82\D8\B3\D9\85 \D8\A7\D9\84\D8\AD\D8\B1\D8\AC \D9\85\D8\AD\D8\B1\D8\B1 \D8\AA\D9\84\D9\82\D8\A7\D8\A6\D9\8A\D8\A7\D9\8B\00", align 1
@str.const.190 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.194 = private unnamed_addr constant [38 x i8] c"  \D8\A5\D9\86\D8\B4\D8\A7\D8\A1 \D8\AD\D8\A7\D8\B1\D8\B3 \D9\85\D9\84\D9\81 \D9\85\D8\A4\D9\82\D8\AA\00", align 1
@str.const.198 = private unnamed_addr constant [3 x i8] c"10\00", align 1
@str.const.199 = private unnamed_addr constant [31 x i8] c"\D9\85\D9\84\D9\81 \D9\85\D8\A4\D9\82\D8\AA: /tmp/data.tmp\00", align 1
@str.const.201 = private unnamed_addr constant [35 x i8] c"\D9\83\D8\AA\D8\A7\D8\A8\D8\A9 \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA \D9\85\D8\A4\D9\82\D8\AA\D8\A9\00", align 1
@str.const.203 = private unnamed_addr constant [26 x i8] c"\D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D8\A7\D9\84\D9\86\D8\AA\D8\A7\D8\A6\D8\AC\00", align 1
@str.const.205 = private unnamed_addr constant [38 x i8] c"\D9\85\D9\84\D9\81 \D9\85\D8\A4\D9\82\D8\AA (\D8\AD\D8\B0\D9\81 \D8\AA\D9\84\D9\82\D8\A7\D8\A6\D9\8A)\00", align 1
@str.const.206 = private unnamed_addr constant [64 x i8] c"  \D8\A7\D9\84\D9\85\D9\84\D9\81 \D8\A7\D9\84\D9\85\D8\A4\D9\82\D8\AA \D8\AD\D9\8F\D8\B0\D9\81 \D8\B9\D9\86\D8\AF \D8\AE\D8\B1\D9\88\D8\AC \D8\A7\D9\84\D9\86\D8\B7\D8\A7\D9\82\00", align 1
@str.const.210 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.214 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\93] \00", align 1
@str.const.218 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\97] \00", align 1
@str.const.222 = private unnamed_addr constant [43 x i8] c"=== \D9\85\D9\84\D8\AE\D8\B5 \D8\A5\D8\AF\D8\A7\D8\B1\D8\A9 \D8\A7\D9\84\D9\85\D9\88\D8\A7\D8\B1\D8\AF ===\00", align 1
@str.const.226 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.230 = private unnamed_addr constant [69 x i8] c"RAII: \D8\A7\D9\84\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D8\AA\D9\84\D9\82\D8\A7\D8\A6\D9\8A \D8\B9\D9\86\D8\AF \D9\86\D9\87\D8\A7\D9\8A\D8\A9 \D8\A7\D9\84\D9\86\D8\B7\D8\A7\D9\82\00", align 1
@str.const.231 = private unnamed_addr constant [43 x i8] c"LIFO: \D8\A7\D9\84\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1 \D8\A8\D8\AA\D8\B1\D8\AA\D9\8A\D8\A8 \D8\B9\D9\83\D8\B3\D9\8A\00", align 1
@str.const.232 = private unnamed_addr constant [55 x i8] c"\D8\A7\D9\84\D9\85\D8\AC\D9\85\D8\B9: \D8\A5\D8\AF\D8\A7\D8\B1\D8\A9 \D8\A7\D9\84\D8\B3\D8\B9\D8\A9 \D9\88\D8\A7\D9\84\D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1\00", align 1
@str.const.233 = private unnamed_addr constant [47 x i8] c"\D8\A7\D9\84\D8\AD\D8\A7\D8\B1\D8\B3: \D8\AD\D9\85\D8\A7\D9\8A\D8\A9 \D8\A7\D9\84\D9\82\D8\B3\D9\85 \D8\A7\D9\84\D8\AD\D8\B1\D8\AC\00", align 1
@str.const.234 = private unnamed_addr constant [53 x i8] c"\D8\A7\D9\84\D8\AA\D9\86\D8\B8\D9\8A\D9\81: \D8\AD\D8\B0\D9\81 \D8\A7\D9\84\D9\85\D9\88\D8\A7\D8\B1\D8\AF \D8\A7\D9\84\D9\85\D8\A4\D9\82\D8\AA\D8\A9\00", align 1
@str.const.235 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.239 = private unnamed_addr constant [19 x i8] c"  \D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9: \00", align 1
@str.const.240 = private unnamed_addr constant [14 x i8] c"/5 \D9\85\D8\A8\D8\A7\D8\AF\D8\A6\00", align 1
@str.const.243 = private unnamed_addr constant [109 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.247 = private unnamed_addr constant [56 x i8] c"\E2\95\91  \D8\A5\D8\AF\D8\A7\D8\B1\D8\A9 \D8\A7\D9\84\D9\85\D9\88\D8\A7\D8\B1\D8\AF - \D9\86\D9\85\D8\B7 RAII        \E2\95\91\00", align 1
@str.const.251 = private unnamed_addr constant [109 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.255 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.259 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.263 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.267 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.271 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.275 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.279 = private unnamed_addr constant [19 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 ===\00", align 1

define i64 @"\D8\B3\D8\AC_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1"(ptr %"\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D9\88\D8\B5\D9\81") {
entry:
  %"\D9\88\D8\B5\D9\812" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81", ptr %"\D9\88\D8\B5\D9\812", align 8
  %"\D9\85\D8\B9\D8\B1\D9\811" = alloca ptr, align 8
  store ptr %"\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %"\D9\85\D8\B9\D8\B1\D9\811")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\85\D8\B9\D8\B1\D9\811", i64 %copylen2)
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
  %len211 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\812")
  %totallen12 = add i64 %len110, %len211
  %bufsize13 = add i64 %totallen12, 1
  %concat.buf14 = alloca i8, i64 %bufsize13, align 1
  %4 = call ptr @memcpy(ptr %concat.buf14, ptr %concat.buf7, i64 %len110)
  %dest215 = getelementptr i8, ptr %concat.buf14, i64 %len110
  %copylen216 = add i64 %len211, 1
  %5 = call ptr @memcpy(ptr %dest215, ptr %"\D9\88\D8\B5\D9\812", i64 %copylen216)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf14)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr %"\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D9\88\D8\B5\D9\81") {
entry:
  %"\D9\88\D8\B5\D9\812" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81", ptr %"\D9\88\D8\B5\D9\812", align 8
  %"\D9\85\D8\B9\D8\B1\D9\811" = alloca ptr, align 8
  store ptr %"\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %len1 = call i64 @strlen(ptr @str.const.4)
  %len2 = call i64 @strlen(ptr %"\D9\85\D8\B9\D8\B1\D9\811")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.4, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\85\D8\B9\D8\B1\D9\811", i64 %copylen2)
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
  %len211 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\812")
  %totallen12 = add i64 %len110, %len211
  %bufsize13 = add i64 %totallen12, 1
  %concat.buf14 = alloca i8, i64 %bufsize13, align 1
  %4 = call ptr @memcpy(ptr %concat.buf14, ptr %concat.buf7, i64 %len110)
  %dest215 = getelementptr i8, ptr %concat.buf14, i64 %len110
  %copylen216 = add i64 %len211, 1
  %5 = call ptr @memcpy(ptr %dest215, ptr %"\D9\88\D8\B5\D9\812", i64 %copylen216)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf14)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr %"\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A9") {
entry:
  %"\D8\B9\D9\85\D9\84\D9\8A\D8\A92" = alloca ptr, align 8
  store ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A9", ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A92", align 8
  %"\D9\85\D8\B9\D8\B1\D9\811" = alloca ptr, align 8
  store ptr %"\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %len1 = call i64 @strlen(ptr @str.const.9)
  %len2 = call i64 @strlen(ptr %"\D9\85\D8\B9\D8\B1\D9\811")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.9, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\85\D8\B9\D8\B1\D9\811", i64 %copylen2)
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
  %len211 = call i64 @strlen(ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A92")
  %totallen12 = add i64 %len110, %len211
  %bufsize13 = add i64 %totallen12, 1
  %concat.buf14 = alloca i8, i64 %bufsize13, align 1
  %4 = call ptr @memcpy(ptr %concat.buf14, ptr %concat.buf7, i64 %len110)
  %dest215 = getelementptr i8, ptr %concat.buf14, i64 %len110
  %copylen216 = add i64 %len211, 1
  %5 = call ptr @memcpy(ptr %dest215, ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A92", i64 %copylen216)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf14)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B3\D8\AC_\D9\85\D8\B4\D9\83\D9\84\D8\A9"(ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  %len1 = call i64 @strlen(ptr @str.const.14)
  %len2 = call i64 @strlen(ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.14, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\85\D9\84\D9\81"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.22)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca ptr, align 8
  store ptr @str.const.26, ptr %alloca, align 8
  %alloca1 = alloca ptr, align 8
  store ptr @str.const.27, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 1, ptr %alloca2, align 8
  %len1 = call i64 @strlen(ptr @str.const.28)
  %len2 = call i64 @strlen(ptr %alloca1)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %4 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.28, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %5 = call ptr @memcpy(ptr %dest2, ptr %alloca1, i64 %copylen2)
  %calltmp = call i64 @"\D8\B3\D8\AC_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1"(ptr %alloca, ptr %concat.buf)
  %"%\D8\AD\D8\A7\D9\84_\D9\85\D9\81\D8\AA\D9\88\D8\AD.load" = load i64, ptr %alloca2, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AD\D8\A7\D9\84_\D9\85\D9\81\D8\AA\D9\88\D8\AD.load", 1
  br i1 %cmpeqtmp, label %then_0, label %merge_1

then_0:                                           ; preds = %entry
  %calltmp3 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr %alloca, ptr @str.const.29)
  %calltmp4 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr %alloca, ptr @str.const.30)
  %calltmp5 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr %alloca, ptr @str.const.31)
  br label %merge_1

merge_1:                                          ; preds = %then_0, %entry
  %len16 = call i64 @strlen(ptr @str.const.28)
  %len27 = call i64 @strlen(ptr %alloca1)
  %totallen8 = add i64 %len16, %len27
  %bufsize9 = add i64 %totallen8, 1
  %concat.buf10 = alloca i8, i64 %bufsize9, align 1
  %6 = call ptr @memcpy(ptr %concat.buf10, ptr @str.const.28, i64 %len16)
  %dest211 = getelementptr i8, ptr %concat.buf10, i64 %len16
  %copylen212 = add i64 %len27, 1
  %7 = call ptr @memcpy(ptr %dest211, ptr %alloca1, i64 %copylen212)
  %calltmp13 = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr %alloca, ptr %concat.buf10)
  store i64 0, ptr %alloca2, align 8
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.33)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.37)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define ptr @"\D8\B9\D8\B1\D8\B6_\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"(i64 %"\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9", i64 %"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A") {
entry:
  %"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A2" = alloca i64, align 8
  store i64 %"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A", ptr %"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A2", align 8
  %"\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A91" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9", ptr %"\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A91", align 8
  %alloca = alloca ptr, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load" = load i64, ptr %"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A2", align 8
  %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A91", align 8
  %subtmp = sub i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load", %"%\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load"
  %alloca3 = alloca i64, align 8
  store i64 %subtmp, ptr %alloca3, align 8
  %alloca4 = alloca ptr, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret ptr @str.ret
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.47)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.51)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 1024, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load" = load i64, ptr %alloca1, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\83\D9\84\D9\8A\D8\A9.load" = load i64, ptr %alloca, align 8
  %calltmp = call ptr @"\D8\B9\D8\B1\D8\B6_\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"(i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load", i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\83\D9\84\D9\8A\D8\A9.load")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.55)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca2 = alloca i64, align 8
  store i64 256, ptr %alloca2, align 8
  %calltmp3 = call i64 @"\D8\B3\D8\AC_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1"(ptr @str.const.59, ptr @str.const.60)
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load4" = load i64, ptr %alloca1, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A91.load" = load i64, ptr %alloca2, align 8
  %addtmp = add i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load4", %"%\D9\83\D8\AA\D9\84\D8\A91.load"
  store i64 %addtmp, ptr %alloca1, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load5" = load i64, ptr %alloca1, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\83\D9\84\D9\8A\D8\A9.load6" = load i64, ptr %alloca, align 8
  %calltmp7 = call ptr @"\D8\B9\D8\B1\D8\B6_\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"(i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load5", i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\83\D9\84\D9\8A\D8\A9.load6")
  %alloca8 = alloca i64, align 8
  store i64 512, ptr %alloca8, align 8
  %calltmp9 = call i64 @"\D8\B3\D8\AC_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1"(ptr @str.const.61, ptr @str.const.62)
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load10" = load i64, ptr %alloca1, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A92.load" = load i64, ptr %alloca8, align 8
  %addtmp11 = add i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load10", %"%\D9\83\D8\AA\D9\84\D8\A92.load"
  store i64 %addtmp11, ptr %alloca1, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load12" = load i64, ptr %alloca1, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\83\D9\84\D9\8A\D8\A9.load13" = load i64, ptr %alloca, align 8
  %calltmp14 = call ptr @"\D8\B9\D8\B1\D8\B6_\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"(i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load12", i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\83\D9\84\D9\8A\D8\A9.load13")
  %alloca15 = alloca i64, align 8
  store i64 128, ptr %alloca15, align 8
  %calltmp16 = call i64 @"\D8\B3\D8\AC_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1"(ptr @str.const.63, ptr @str.const.64)
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load17" = load i64, ptr %alloca1, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A93.load" = load i64, ptr %alloca15, align 8
  %addtmp18 = add i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load17", %"%\D9\83\D8\AA\D9\84\D8\A93.load"
  store i64 %addtmp18, ptr %alloca1, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load19" = load i64, ptr %alloca1, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\83\D9\84\D9\8A\D8\A9.load20" = load i64, ptr %alloca, align 8
  %calltmp21 = call ptr @"\D8\B9\D8\B1\D8\B6_\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"(i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load19", i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\83\D9\84\D9\8A\D8\A9.load20")
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.65)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.69)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp22 = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.63, ptr @str.const.74)
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load23" = load i64, ptr %alloca1, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A93.load24" = load i64, ptr %alloca15, align 8
  %subtmp = sub i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load23", %"%\D9\83\D8\AA\D9\84\D8\A93.load24"
  store i64 %subtmp, ptr %alloca1, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load25" = load i64, ptr %alloca1, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\83\D9\84\D9\8A\D8\A9.load26" = load i64, ptr %alloca, align 8
  %calltmp27 = call ptr @"\D8\B9\D8\B1\D8\B6_\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"(i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load25", i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\83\D9\84\D9\8A\D8\A9.load26")
  %calltmp28 = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.61, ptr @str.const.76)
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load29" = load i64, ptr %alloca1, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A92.load30" = load i64, ptr %alloca8, align 8
  %subtmp31 = sub i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load29", %"%\D9\83\D8\AA\D9\84\D8\A92.load30"
  store i64 %subtmp31, ptr %alloca1, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load32" = load i64, ptr %alloca1, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\83\D9\84\D9\8A\D8\A9.load33" = load i64, ptr %alloca, align 8
  %calltmp34 = call ptr @"\D8\B9\D8\B1\D8\B6_\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"(i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load32", i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\83\D9\84\D9\8A\D8\A9.load33")
  %calltmp35 = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.59, ptr @str.const.78)
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load36" = load i64, ptr %alloca1, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A91.load37" = load i64, ptr %alloca2, align 8
  %subtmp38 = sub i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load36", %"%\D9\83\D8\AA\D9\84\D8\A91.load37"
  store i64 %subtmp38, ptr %alloca1, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load39" = load i64, ptr %alloca1, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\83\D9\84\D9\8A\D8\A9.load40" = load i64, ptr %alloca, align 8
  %calltmp41 = call ptr @"\D8\B9\D8\B1\D8\B6_\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"(i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\D8\A9.load39", i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\83\D9\84\D9\8A\D8\A9.load40")
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.79)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.83)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\85\D8\AC\D9\85\D8\B9_\D8\A7\D8\AA\D8\B5\D8\A7\D9\84\D8\A7\D8\AA"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.87)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.91)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 3, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.99)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D9\86\D8\B4\D8\B7\D8\A9.load" = load i64, ptr %alloca1, align 8
  %"%\D8\B3\D8\B9\D8\A9.load" = load i64, ptr %alloca, align 8
  %cmplttmp = icmp slt i64 %"%\D9\86\D8\B4\D8\B7\D8\A9.load", %"%\D8\B3\D8\B9\D8\A9.load"
  br i1 %cmplttmp, label %then_2, label %merge_3

then_2:                                           ; preds = %entry
  %"%\D9\86\D8\B4\D8\B7\D8\A9.load2" = load i64, ptr %alloca1, align 8
  %addtmp = add i64 %"%\D9\86\D8\B4\D8\B7\D8\A9.load2", 1
  store i64 %addtmp, ptr %alloca1, align 8
  %calltmp = call i64 @"\D8\B3\D8\AC_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1"(ptr @str.const.103, ptr @str.const.104)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_3

merge_3:                                          ; preds = %then_2, %entry
  %"%\D9\86\D8\B4\D8\B7\D8\A9.load3" = load i64, ptr %alloca1, align 8
  %"%\D8\B3\D8\B9\D8\A9.load4" = load i64, ptr %alloca, align 8
  %cmplttmp5 = icmp slt i64 %"%\D9\86\D8\B4\D8\B7\D8\A9.load3", %"%\D8\B3\D8\B9\D8\A9.load4"
  br i1 %cmplttmp5, label %then_4, label %merge_5

then_4:                                           ; preds = %merge_3
  %"%\D9\86\D8\B4\D8\B7\D8\A9.load6" = load i64, ptr %alloca1, align 8
  %addtmp7 = add i64 %"%\D9\86\D8\B4\D8\B7\D8\A9.load6", 1
  store i64 %addtmp7, ptr %alloca1, align 8
  %calltmp8 = call i64 @"\D8\B3\D8\AC_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1"(ptr @str.const.108, ptr @str.const.109)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_5

merge_5:                                          ; preds = %then_4, %merge_3
  %"%\D9\86\D8\B4\D8\B7\D8\A9.load9" = load i64, ptr %alloca1, align 8
  %"%\D8\B3\D8\B9\D8\A9.load10" = load i64, ptr %alloca, align 8
  %cmplttmp11 = icmp slt i64 %"%\D9\86\D8\B4\D8\B7\D8\A9.load9", %"%\D8\B3\D8\B9\D8\A9.load10"
  br i1 %cmplttmp11, label %then_6, label %merge_7

then_6:                                           ; preds = %merge_5
  %"%\D9\86\D8\B4\D8\B7\D8\A9.load12" = load i64, ptr %alloca1, align 8
  %addtmp13 = add i64 %"%\D9\86\D8\B4\D8\B7\D8\A9.load12", 1
  store i64 %addtmp13, ptr %alloca1, align 8
  %calltmp14 = call i64 @"\D8\B3\D8\AC_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1"(ptr @str.const.113, ptr @str.const.114)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_7

merge_7:                                          ; preds = %then_6, %merge_5
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.118)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D9\86\D8\B4\D8\B7\D8\A9.load15" = load i64, ptr %alloca1, align 8
  %"%\D8\B3\D8\B9\D8\A9.load16" = load i64, ptr %alloca, align 8
  %cmplttmp17 = icmp slt i64 %"%\D9\86\D8\B4\D8\B7\D8\A9.load15", %"%\D8\B3\D8\B9\D8\A9.load16"
  br i1 %cmplttmp17, label %then_8, label %else_10

then_8:                                           ; preds = %merge_7
  %"%\D9\86\D8\B4\D8\B7\D8\A9.load18" = load i64, ptr %alloca1, align 8
  %addtmp19 = add i64 %"%\D9\86\D8\B4\D8\B7\D8\A9.load18", 1
  store i64 %addtmp19, ptr %alloca1, align 8
  %calltmp20 = call i64 @"\D8\B3\D8\AC_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1"(ptr @str.const.122, ptr @str.const.123)
  br label %merge_9

else_10:                                          ; preds = %merge_7
  %calltmp21 = call i64 @"\D8\B3\D8\AC_\D9\85\D8\B4\D9\83\D9\84\D8\A9"(ptr @str.const.124)
  br label %merge_9

merge_9:                                          ; preds = %else_10, %then_8
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.125)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp22 = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.103, ptr @str.const.130)
  %"%\D9\86\D8\B4\D8\B7\D8\A9.load23" = load i64, ptr %alloca1, align 8
  %subtmp = sub i64 %"%\D9\86\D8\B4\D8\B7\D8\A9.load23", 1
  store i64 %subtmp, ptr %alloca1, align 8
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D9\86\D8\B4\D8\B7\D8\A9.load24" = load i64, ptr %alloca1, align 8
  %"%\D8\B3\D8\B9\D8\A9.load25" = load i64, ptr %alloca, align 8
  %cmplttmp26 = icmp slt i64 %"%\D9\86\D8\B4\D8\B7\D8\A9.load24", %"%\D8\B3\D8\B9\D8\A9.load25"
  br i1 %cmplttmp26, label %then_11, label %merge_12

then_11:                                          ; preds = %merge_9
  %"%\D9\86\D8\B4\D8\B7\D8\A9.load27" = load i64, ptr %alloca1, align 8
  %addtmp28 = add i64 %"%\D9\86\D8\B4\D8\B7\D8\A9.load27", 1
  store i64 %addtmp28, ptr %alloca1, align 8
  %calltmp29 = call i64 @"\D8\B3\D8\AC_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1"(ptr @str.const.122, ptr @str.const.135)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_12

merge_12:                                         ; preds = %then_11, %merge_9
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.139)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.143)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp30 = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.122, ptr @str.const.123)
  %calltmp31 = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.113, ptr @str.const.150)
  %calltmp32 = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.108, ptr @str.const.152)
  store i64 0, ptr %alloca1, align 8
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.156)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D8\AD\D8\B1\D8\A7\D8\B3\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.160)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.164)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.168)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  store i64 1, ptr %alloca, align 8
  %calltmp = call i64 @"\D8\B3\D8\AC_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1"(ptr @str.const.172, ptr @str.const.173)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.174)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp1 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.172, ptr @str.const.179)
  %calltmp2 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.172, ptr @str.const.181)
  %calltmp3 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.172, ptr @str.const.183)
  %calltmp4 = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.172, ptr @str.const.173)
  store i64 0, ptr %alloca, align 8
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.186)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.190)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.194)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp5 = call i64 @"\D8\B3\D8\AC_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1"(ptr @str.const.198, ptr @str.const.199)
  %calltmp6 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.198, ptr @str.const.201)
  %calltmp7 = call i64 @"\D8\B3\D8\AC_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(ptr @str.const.198, ptr @str.const.203)
  %calltmp8 = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.198, ptr @str.const.205)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.206)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.210)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3"(ptr %"\D9\82\D8\A7\D8\B9\D8\AF\D8\A9", i64 %"\D9\85\D8\B1") {
entry:
  %"\D9\85\D8\B12" = alloca i64, align 8
  store i64 %"\D9\85\D8\B1", ptr %"\D9\85\D8\B12", align 8
  %"\D9\82\D8\A7\D8\B9\D8\AF\D8\A91" = alloca ptr, align 8
  store ptr %"\D9\82\D8\A7\D8\B9\D8\AF\D8\A9", ptr %"\D9\82\D8\A7\D8\B9\D8\AF\D8\A91", align 8
  %"%\D9\85\D8\B1.load" = load i64, ptr %"\D9\85\D8\B12", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D8\B1.load", 1
  br i1 %cmpeqtmp, label %then_13, label %else_15

then_13:                                          ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const.214)
  %len2 = call i64 @strlen(ptr %"\D9\82\D8\A7\D8\B9\D8\AF\D8\A91")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.214, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\82\D8\A7\D8\B9\D8\AF\D8\A91", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_14

else_15:                                          ; preds = %entry
  %len13 = call i64 @strlen(ptr @str.const.218)
  %len24 = call i64 @strlen(ptr %"\D9\82\D8\A7\D8\B9\D8\AF\D8\A91")
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %4 = call ptr @memcpy(ptr %concat.buf7, ptr @str.const.218, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %5 = call ptr @memcpy(ptr %dest28, ptr %"\D9\82\D8\A7\D8\B9\D8\AF\D8\A91", i64 %copylen29)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf7)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_14

merge_14:                                         ; preds = %else_15, %then_13
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D9\84\D8\AE\D8\B5_\D8\A5\D8\AF\D8\A7\D8\B1\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.222)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.226)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3"(ptr @str.const.230, i64 1)
  %"%\D8\B9.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\B9.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp1 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3"(ptr @str.const.231, i64 1)
  %"%\D8\B9.load2" = load i64, ptr %alloca, align 8
  %addtmp3 = add i64 %"%\D8\B9.load2", 1
  store i64 %addtmp3, ptr %alloca, align 8
  %calltmp4 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3"(ptr @str.const.232, i64 1)
  %"%\D8\B9.load5" = load i64, ptr %alloca, align 8
  %addtmp6 = add i64 %"%\D8\B9.load5", 1
  store i64 %addtmp6, ptr %alloca, align 8
  %calltmp7 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3"(ptr @str.const.233, i64 1)
  %"%\D8\B9.load8" = load i64, ptr %alloca, align 8
  %addtmp9 = add i64 %"%\D8\B9.load8", 1
  store i64 %addtmp9, ptr %alloca, align 8
  %calltmp10 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A8\D8\AF\D8\A3"(ptr @str.const.234, i64 1)
  %"%\D8\B9.load11" = load i64, ptr %alloca, align 8
  %addtmp12 = add i64 %"%\D8\B9.load11", 1
  store i64 %addtmp12, ptr %alloca, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.235)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.243)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.247)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.251)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.255)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\85\D9\84\D9\81"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.259)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp1 = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"()
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.263)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp2 = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\85\D8\AC\D9\85\D8\B9_\D8\A7\D8\AA\D8\B5\D8\A7\D9\84\D8\A7\D8\AA"()
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.267)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp3 = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D8\AD\D8\B1\D8\A7\D8\B3\D8\A9"()
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.271)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp4 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D9\84\D8\AE\D8\B5_\D8\A5\D8\AF\D8\A7\D8\B1\D8\A9"()
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.275)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.279)
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
