; ModuleID = '02_simple_borrowing_rules'
source_filename = "02_simple_borrowing_rules"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [14 x i8] c"  [\D9\85\D8\A7\D9\84\D9\83] \00", align 1
@str.const.1 = private unnamed_addr constant [4 x i8] c" = \00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.2 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.4 = private unnamed_addr constant [16 x i8] c"  [\D9\82\D8\B1\D8\A7\D8\A1\D8\A9] \00", align 1
@str.const.5 = private unnamed_addr constant [16 x i8] c" \D9\8A\D9\82\D8\B1\D8\A3 \D9\85\D9\86 \00", align 1
@str.const.9 = private unnamed_addr constant [16 x i8] c"  [\D8\AA\D8\B9\D8\AF\D9\8A\D9\84] \00", align 1
@str.const.10 = private unnamed_addr constant [13 x i8] c" \D9\8A\D8\B9\D8\AF\D9\91\D9\84 \00", align 1
@str.const.14 = private unnamed_addr constant [12 x i8] c"  [\D8\AD\D8\B1\D8\B1] \00", align 1
@str.const.15 = private unnamed_addr constant [19 x i8] c" \D8\AA\D9\85 \D8\AA\D8\AD\D8\B1\D9\8A\D8\B1\D9\87\00", align 1
@str.const.19 = private unnamed_addr constant [13 x i8] c"  [\D8\AE\D8\B7\D8\A3!] \00", align 1
@str.const.23 = private unnamed_addr constant [14 x i8] c"  [\D9\86\D8\AC\D8\A7\D8\AD] \00", align 1
@str.const.27 = private unnamed_addr constant [80 x i8] c"\D9\84\D8\A7 \D9\8A\D9\85\D9\83\D9\86 \D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D9\85\D8\B9 \D9\88\D8\AC\D9\88\D8\AF \D8\AA\D8\B9\D8\AF\D9\8A\D9\84 \D9\86\D8\B4\D8\B7\00", align 1
@str.const.28 = private unnamed_addr constant [28 x i8] c"\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D9\82\D8\B1\D8\A7\D8\A1\D8\A9 #\00", align 1
@str.const.29 = private unnamed_addr constant [14 x i8] c" \D9\85\D8\B3\D9\85\D9\88\D8\AD\D8\A9\00", align 1
@str.const.30 = private unnamed_addr constant [3 x i8] c"  \00", align 1
@str.const.34 = private unnamed_addr constant [63 x i8] c"\D9\84\D8\A7 \D9\8A\D9\85\D9\83\D9\86 \D8\A7\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84 \D9\85\D8\B9 \D9\88\D8\AC\D9\88\D8\AF \D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D9\86\D8\B4\D8\B7\D8\A9\00", align 1
@str.const.35 = private unnamed_addr constant [57 x i8] c"\D9\84\D8\A7 \D9\8A\D9\85\D9\83\D9\86 \D9\88\D8\AC\D9\88\D8\AF \D8\A3\D9\83\D8\AB\D8\B1 \D9\85\D9\86 \D8\AA\D8\B9\D8\AF\D9\8A\D9\84 \D9\88\D8\A7\D8\AD\D8\AF\00", align 1
@str.const.36 = private unnamed_addr constant [41 x i8] c"  \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D8\AA\D8\B9\D8\AF\D9\8A\D9\84 \D9\85\D8\B3\D9\85\D9\88\D8\AD\D8\A9\00", align 1
@str.const.40 = private unnamed_addr constant [57 x i8] c"=== \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D8\A7\D9\84\D9\85\D8\AA\D8\B9\D8\AF\D8\AF\D8\A9 ===\00", align 1
@str.const.44 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.48 = private unnamed_addr constant [13 x i8] c"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA\00", align 1
@str.const.49 = private unnamed_addr constant [15 x i8] c"[10\D8\8C 20\D8\8C 30]\00", align 1
@str.const.50 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.54 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\B1\D8\AC\D8\B9_1\00", align 1
@str.const.56 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\B1\D8\AC\D8\B9_2\00", align 1
@str.const.58 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\B1\D8\AC\D8\B9_3\00", align 1
@str.const.60 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.64 = private unnamed_addr constant [49 x i8] c"  \D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9: \00", align 1
@str.const.70 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.74 = private unnamed_addr constant [51 x i8] c"=== \D8\AA\D8\B9\D8\A7\D8\B1\D8\B6 \D8\A7\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84 \D9\88\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9 ===\00", align 1
@str.const.78 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.82 = private unnamed_addr constant [11 x i8] c"\D9\82\D8\A7\D8\A6\D9\85\D8\A9\00", align 1
@str.const.83 = private unnamed_addr constant [15 x i8] c"[\D8\A3\D8\8C \D8\A8\D8\8C \D8\AC]\00", align 1
@str.const.84 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.88 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\B9\D8\AF\D9\91\D9\84\00", align 1
@str.const.90 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.94 = private unnamed_addr constant [53 x i8] c"  \D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9 \D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D8\A3\D8\AB\D9\86\D8\A7\D8\A1 \D8\A7\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84:\00", align 1
@str.const.98 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.102 = private unnamed_addr constant [31 x i8] c"  \D8\AA\D8\AD\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84...\00", align 1
@str.const.107 = private unnamed_addr constant [49 x i8] c"  \D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9 \D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D8\A8\D8\B9\D8\AF \D8\A7\D9\84\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1:\00", align 1
@str.const.111 = private unnamed_addr constant [9 x i8] c"\D9\82\D8\A7\D8\B1\D8\A6\00", align 1
@str.const.114 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.118 = private unnamed_addr constant [30 x i8] c"=== \D9\86\D9\82\D9\84 \D8\A7\D9\84\D9\85\D9\84\D9\83\D9\8A\D8\A9 ===\00", align 1
@str.const.122 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.126 = private unnamed_addr constant [38 x i8] c"  \D8\A7\D9\84\D8\AE\D8\B7\D9\88\D8\A9 1: \D8\A5\D9\86\D8\B4\D8\A7\D8\A1 \D9\85\D9\88\D8\B1\D8\AF\00", align 1
@str.const.130 = private unnamed_addr constant [12 x i8] c"\D9\85\D9\88\D8\B1\D8\AF_\D8\A3\00", align 1
@str.const.131 = private unnamed_addr constant [24 x i8] c"\D9\85\D9\84\D9\81_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.txt\00", align 1
@str.const.132 = private unnamed_addr constant [50 x i8] c"  \D8\A7\D9\84\D8\AE\D8\B7\D9\88\D8\A9 2: \D9\86\D9\82\D9\84 \D8\A5\D9\84\D9\89 \D9\85\D8\A7\D9\84\D9\83 \D8\AC\D8\AF\D9\8A\D8\AF\00", align 1
@str.const.136 = private unnamed_addr constant [33 x i8] c"     \D9\85\D9\88\D8\B1\D8\AF_\D8\A3 --> \D9\85\D9\88\D8\B1\D8\AF_\D8\A8\00", align 1
@str.const.140 = private unnamed_addr constant [12 x i8] c"\D9\85\D9\88\D8\B1\D8\AF_\D8\A8\00", align 1
@str.const.142 = private unnamed_addr constant [43 x i8] c"     \D9\85\D9\88\D8\B1\D8\AF_\D8\A3 \D8\A3\D8\B5\D8\A8\D8\AD \D8\BA\D9\8A\D8\B1 \D8\B5\D8\A7\D9\84\D8\AD!\00", align 1
@str.const.146 = private unnamed_addr constant [64 x i8] c"  \D8\A7\D9\84\D8\AE\D8\B7\D9\88\D8\A9 3: \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D9\85\D9\86 \D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83 \D8\A7\D9\84\D8\AC\D8\AF\D9\8A\D8\AF\00", align 1
@str.const.152 = private unnamed_addr constant [38 x i8] c"  \D8\A7\D9\84\D8\AE\D8\B7\D9\88\D8\A9 4: \D8\AA\D8\AD\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D9\83\D9\84\00", align 1
@str.const.158 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.162 = private unnamed_addr constant [39 x i8] c"=== \D8\AF\D9\88\D8\B1\D8\A9 \D8\AD\D9\8A\D8\A7\D8\A9 \D8\A7\D9\84\D9\85\D8\B1\D8\AC\D8\B9 ===\00", align 1
@str.const.166 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.171 = private unnamed_addr constant [26 x i8] c". \D8\A5\D9\86\D8\B4\D8\A7\D8\A1 \D8\A7\D9\84\D9\83\D8\A7\D8\A6\D9\86\00", align 1
@str.const.175 = private unnamed_addr constant [9 x i8] c"\D9\83\D8\A7\D8\A6\D9\86\00", align 1
@str.const.176 = private unnamed_addr constant [20 x i8] c"{\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA: 100}\00", align 1
@str.const.178 = private unnamed_addr constant [37 x i8] c". \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D8\A3\D9\88\D9\84\D9\89\00", align 1
@str.const.182 = private unnamed_addr constant [4 x i8] c"\D9\821\00", align 1
@str.const.185 = private unnamed_addr constant [39 x i8] c". \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D8\AB\D8\A7\D9\86\D9\8A\D8\A9\00", align 1
@str.const.189 = private unnamed_addr constant [4 x i8] c"\D9\822\00", align 1
@str.const.192 = private unnamed_addr constant [30 x i8] c". \D8\AA\D8\AD\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A7\D8\AA\00", align 1
@str.const.199 = private unnamed_addr constant [28 x i8] c". \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D8\AA\D8\B9\D8\AF\D9\8A\D9\84\00", align 1
@str.const.206 = private unnamed_addr constant [28 x i8] c". \D8\AA\D8\AD\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84\00", align 1
@str.const.212 = private unnamed_addr constant [26 x i8] c". \D8\AA\D8\AD\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83\00", align 1
@str.const.217 = private unnamed_addr constant [46 x i8] c"\D8\AF\D9\88\D8\B1\D8\A9 \D8\A7\D9\84\D8\AD\D9\8A\D8\A7\D8\A9 \D8\A7\D9\83\D8\AA\D9\85\D9\84\D8\AA \D8\A8\D9\86\D8\AC\D8\A7\D8\AD\00", align 1
@str.const.218 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.222 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\93] \00", align 1
@str.const.226 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\97] \00", align 1
@str.const.230 = private unnamed_addr constant [47 x i8] c"=== \D9\85\D9\84\D8\AE\D8\B5 \D9\82\D9\88\D8\A7\D8\B9\D8\AF \D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 ===\00", align 1
@str.const.234 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.238 = private unnamed_addr constant [35 x i8] c"\D9\85\D8\A7\D9\84\D9\83 \D9\88\D8\A7\D8\AD\D8\AF \D9\81\D9\8A \D9\83\D9\84 \D9\88\D9\82\D8\AA\00", align 1
@str.const.239 = private unnamed_addr constant [45 x i8] c"\D8\B9\D8\AF\D8\A9 \D9\82\D8\B1\D8\A7\D8\A1\D8\A7\D8\AA \D8\A3\D9\88 \D8\AA\D8\B9\D8\AF\D9\8A\D9\84 \D9\88\D8\A7\D8\AD\D8\AF\00", align 1
@str.const.240 = private unnamed_addr constant [49 x i8] c"\D8\A7\D9\84\D9\85\D8\B1\D8\AC\D8\B9 \D8\A3\D9\82\D8\B5\D8\B1 \D8\B9\D9\85\D8\B1\D8\A7 \D9\85\D9\86 \D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83\00", align 1
@str.const.241 = private unnamed_addr constant [46 x i8] c"\D8\A7\D9\84\D9\86\D9\82\D9\84 \D9\8A\D8\A8\D8\B7\D9\84 \D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83 \D8\A7\D9\84\D8\A3\D8\B5\D9\84\D9\8A\00", align 1
@str.const.242 = private unnamed_addr constant [63 x i8] c"\D8\A7\D9\84\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D8\AA\D9\84\D9\82\D8\A7\D8\A6\D9\8A \D8\B9\D9\86\D8\AF \D9\86\D9\87\D8\A7\D9\8A\D8\A9 \D8\A7\D9\84\D9\86\D8\B7\D8\A7\D9\82\00", align 1
@str.const.243 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.247 = private unnamed_addr constant [19 x i8] c"  \D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9: \00", align 1
@str.const.248 = private unnamed_addr constant [14 x i8] c"/5 \D9\82\D9\88\D8\A7\D8\B9\D8\AF\00", align 1
@str.const.251 = private unnamed_addr constant [109 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.255 = private unnamed_addr constant [63 x i8] c"\E2\95\91  \D9\82\D9\88\D8\A7\D8\B9\D8\AF \D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D9\88\D8\A7\D9\84\D9\85\D9\84\D9\83\D9\8A\D8\A9        \E2\95\91\00", align 1
@str.const.259 = private unnamed_addr constant [109 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.263 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.267 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.271 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.275 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.279 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.283 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.287 = private unnamed_addr constant [19 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 ===\00", align 1

define i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A7\D9\84\D9\83"(ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"\D9\82\D9\8A\D9\85\D8\A92" = alloca ptr, align 8
  store ptr %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %"\D8\A7\D8\B3\D9\851" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\851", align 8
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\851")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D8\B3\D9\851", i64 %copylen2)
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
  %len211 = call i64 @strlen(ptr %"\D9\82\D9\8A\D9\85\D8\A92")
  %totallen12 = add i64 %len110, %len211
  %bufsize13 = add i64 %totallen12, 1
  %concat.buf14 = alloca i8, i64 %bufsize13, align 1
  %4 = call ptr @memcpy(ptr %concat.buf14, ptr %concat.buf7, i64 %len110)
  %dest215 = getelementptr i8, ptr %concat.buf14, i64 %len110
  %copylen216 = add i64 %len211, 1
  %5 = call ptr @memcpy(ptr %dest215, ptr %"\D9\82\D9\8A\D9\85\D8\A92", i64 %copylen216)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf14)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"(ptr %"\D9\85\D8\B1\D8\AC\D8\B9", ptr %"\D9\85\D8\A7\D9\84\D9\83") {
entry:
  %"\D9\85\D8\A7\D9\84\D9\832" = alloca ptr, align 8
  store ptr %"\D9\85\D8\A7\D9\84\D9\83", ptr %"\D9\85\D8\A7\D9\84\D9\832", align 8
  %"\D9\85\D8\B1\D8\AC\D8\B91" = alloca ptr, align 8
  store ptr %"\D9\85\D8\B1\D8\AC\D8\B9", ptr %"\D9\85\D8\B1\D8\AC\D8\B91", align 8
  %len1 = call i64 @strlen(ptr @str.const.4)
  %len2 = call i64 @strlen(ptr %"\D9\85\D8\B1\D8\AC\D8\B91")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.4, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\85\D8\B1\D8\AC\D8\B91", i64 %copylen2)
  %len13 = call i64 @strlen(ptr %concat.buf)
  %len24 = call i64 @strlen(ptr @str.const.5)
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %2 = call ptr @memcpy(ptr %concat.buf7, ptr %concat.buf, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %3 = call ptr @memcpy(ptr %dest28, ptr @str.const.5, i64 %copylen29)
  %len110 = call i64 @strlen(ptr %concat.buf7)
  %len211 = call i64 @strlen(ptr %"\D9\85\D8\A7\D9\84\D9\832")
  %totallen12 = add i64 %len110, %len211
  %bufsize13 = add i64 %totallen12, 1
  %concat.buf14 = alloca i8, i64 %bufsize13, align 1
  %4 = call ptr @memcpy(ptr %concat.buf14, ptr %concat.buf7, i64 %len110)
  %dest215 = getelementptr i8, ptr %concat.buf14, i64 %len110
  %copylen216 = add i64 %len211, 1
  %5 = call ptr @memcpy(ptr %dest215, ptr %"\D9\85\D8\A7\D9\84\D9\832", i64 %copylen216)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf14)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84"(ptr %"\D9\85\D8\B1\D8\AC\D8\B9", ptr %"\D9\85\D8\A7\D9\84\D9\83") {
entry:
  %"\D9\85\D8\A7\D9\84\D9\832" = alloca ptr, align 8
  store ptr %"\D9\85\D8\A7\D9\84\D9\83", ptr %"\D9\85\D8\A7\D9\84\D9\832", align 8
  %"\D9\85\D8\B1\D8\AC\D8\B91" = alloca ptr, align 8
  store ptr %"\D9\85\D8\B1\D8\AC\D8\B9", ptr %"\D9\85\D8\B1\D8\AC\D8\B91", align 8
  %len1 = call i64 @strlen(ptr @str.const.9)
  %len2 = call i64 @strlen(ptr %"\D9\85\D8\B1\D8\AC\D8\B91")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.9, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\85\D8\B1\D8\AC\D8\B91", i64 %copylen2)
  %len13 = call i64 @strlen(ptr %concat.buf)
  %len24 = call i64 @strlen(ptr @str.const.10)
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %2 = call ptr @memcpy(ptr %concat.buf7, ptr %concat.buf, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %3 = call ptr @memcpy(ptr %dest28, ptr @str.const.10, i64 %copylen29)
  %len110 = call i64 @strlen(ptr %concat.buf7)
  %len211 = call i64 @strlen(ptr %"\D9\85\D8\A7\D9\84\D9\832")
  %totallen12 = add i64 %len110, %len211
  %bufsize13 = add i64 %totallen12, 1
  %concat.buf14 = alloca i8, i64 %bufsize13, align 1
  %4 = call ptr @memcpy(ptr %concat.buf14, ptr %concat.buf7, i64 %len110)
  %dest215 = getelementptr i8, ptr %concat.buf14, i64 %len110
  %copylen216 = add i64 %len211, 1
  %5 = call ptr @memcpy(ptr %dest215, ptr %"\D9\85\D8\A7\D9\84\D9\832", i64 %copylen216)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf14)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr %"\D8\A7\D8\B3\D9\85") {
entry:
  %"\D8\A7\D8\B3\D9\851" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\851", align 8
  %len1 = call i64 @strlen(ptr @str.const.14)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\851")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.14, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D8\B3\D9\851", i64 %copylen2)
  %len12 = call i64 @strlen(ptr %concat.buf)
  %len23 = call i64 @strlen(ptr @str.const.15)
  %totallen4 = add i64 %len12, %len23
  %bufsize5 = add i64 %totallen4, 1
  %concat.buf6 = alloca i8, i64 %bufsize5, align 1
  %2 = call ptr @memcpy(ptr %concat.buf6, ptr %concat.buf, i64 %len12)
  %dest27 = getelementptr i8, ptr %concat.buf6, i64 %len12
  %copylen28 = add i64 %len23, 1
  %3 = call ptr @memcpy(ptr %dest27, ptr @str.const.15, i64 %copylen28)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf6)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D8\AE\D8\B7\D8\A7"(ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  %len1 = call i64 @strlen(ptr @str.const.19)
  %len2 = call i64 @strlen(ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.19, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AC\D8\A7\D8\AD"(ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  %len1 = call i64 @strlen(ptr @str.const.23)
  %len2 = call i64 @strlen(ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.23, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"(i64 %"\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", i64 %"\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84") {
entry:
  %"\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\842" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84", ptr %"\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\842", align 8
  %"\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", ptr %"\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A91", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\842", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84.load", 0
  br i1 %cmpgttmp, label %then_0, label %merge_1

then_0:                                           ; preds = %entry
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AE\D8\B7\D8\A7"(ptr @str.const.27)
  ret i64 0

merge_1:                                          ; preds = %entry
  %"%\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A91", align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load", 1
  %alloca = alloca ptr, align 8
  %len1 = call i64 @strlen(ptr @str.const.30)
  %len2 = call i64 @strlen(ptr %alloca)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.30, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %alloca, i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 1
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84"(i64 %"\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", i64 %"\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84") {
entry:
  %"\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\842" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84", ptr %"\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\842", align 8
  %"\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", ptr %"\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A91", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A91", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load", 0
  br i1 %cmpgttmp, label %then_2, label %merge_3

then_2:                                           ; preds = %entry
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AE\D8\B7\D8\A7"(ptr @str.const.34)
  ret i64 0

merge_3:                                          ; preds = %entry
  %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\842", align 8
  %cmpgttmp3 = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84.load", 0
  br i1 %cmpgttmp3, label %then_4, label %merge_5

then_4:                                           ; preds = %merge_3
  %calltmp4 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AE\D8\B7\D8\A7"(ptr @str.const.35)
  ret i64 0

merge_5:                                          ; preds = %merge_3
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.36)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 1
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9_\D9\85\D8\AA\D8\B9\D8\AF\D8\AF\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.40)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.44)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A7\D9\84\D9\83"(ptr @str.const.48, ptr @str.const.49)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.50)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\82.load" = load i64, ptr %alloca, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\AA.load" = load i64, ptr %alloca1, align 8
  %calltmp2 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"(i64 %"%\D8\B9\D8\AF\D8\AF_\D9\82.load", i64 %"%\D8\B9\D8\AF\D8\AF_\D8\AA.load")
  %alloca3 = alloca i64, align 8
  store i64 %calltmp2, ptr %alloca3, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91.load" = load i64, ptr %alloca3, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91.load", 1
  br i1 %cmpeqtmp, label %then_6, label %merge_7

then_6:                                           ; preds = %entry
  %"%\D8\B9\D8\AF\D8\AF_\D9\82.load4" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\AF_\D9\82.load4", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp5 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"(ptr @str.const.54, ptr @str.const.48)
  br label %merge_7

merge_7:                                          ; preds = %then_6, %entry
  %"%\D8\B9\D8\AF\D8\AF_\D9\82.load6" = load i64, ptr %alloca, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\AA.load7" = load i64, ptr %alloca1, align 8
  %calltmp8 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"(i64 %"%\D8\B9\D8\AF\D8\AF_\D9\82.load6", i64 %"%\D8\B9\D8\AF\D8\AF_\D8\AA.load7")
  %alloca9 = alloca i64, align 8
  store i64 %calltmp8, ptr %alloca9, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A92.load" = load i64, ptr %alloca9, align 8
  %cmpeqtmp10 = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A92.load", 1
  br i1 %cmpeqtmp10, label %then_8, label %merge_9

then_8:                                           ; preds = %merge_7
  %"%\D8\B9\D8\AF\D8\AF_\D9\82.load11" = load i64, ptr %alloca, align 8
  %addtmp12 = add i64 %"%\D8\B9\D8\AF\D8\AF_\D9\82.load11", 1
  store i64 %addtmp12, ptr %alloca, align 8
  %calltmp13 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"(ptr @str.const.56, ptr @str.const.48)
  br label %merge_9

merge_9:                                          ; preds = %then_8, %merge_7
  %"%\D8\B9\D8\AF\D8\AF_\D9\82.load14" = load i64, ptr %alloca, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\AA.load15" = load i64, ptr %alloca1, align 8
  %calltmp16 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"(i64 %"%\D8\B9\D8\AF\D8\AF_\D9\82.load14", i64 %"%\D8\B9\D8\AF\D8\AF_\D8\AA.load15")
  %alloca17 = alloca i64, align 8
  store i64 %calltmp16, ptr %alloca17, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A93.load" = load i64, ptr %alloca17, align 8
  %cmpeqtmp18 = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A93.load", 1
  br i1 %cmpeqtmp18, label %then_10, label %merge_11

then_10:                                          ; preds = %merge_9
  %"%\D8\B9\D8\AF\D8\AF_\D9\82.load19" = load i64, ptr %alloca, align 8
  %addtmp20 = add i64 %"%\D8\B9\D8\AF\D8\AF_\D9\82.load19", 1
  store i64 %addtmp20, ptr %alloca, align 8
  %calltmp21 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"(ptr @str.const.58, ptr @str.const.48)
  br label %merge_11

merge_11:                                         ; preds = %then_10, %merge_9
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.60)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp22 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.58)
  %calltmp23 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.56)
  %calltmp24 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.54)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.70)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D8\AA\D8\B9\D8\A7\D8\B1\D8\B6_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.74)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.78)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A7\D9\84\D9\83"(ptr @str.const.82, ptr @str.const.83)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.84)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %"%\D9\82.load" = load i64, ptr %alloca, align 8
  %"%\D8\AA.load" = load i64, ptr %alloca1, align 8
  %calltmp2 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84"(i64 %"%\D9\82.load", i64 %"%\D8\AA.load")
  %alloca3 = alloca i64, align 8
  store i64 %calltmp2, ptr %alloca3, align 8
  %"%\D9\85.load" = load i64, ptr %alloca3, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85.load", 1
  br i1 %cmpeqtmp, label %then_12, label %merge_13

then_12:                                          ; preds = %entry
  %"%\D8\AA.load4" = load i64, ptr %alloca1, align 8
  %addtmp = add i64 %"%\D8\AA.load4", 1
  store i64 %addtmp, ptr %alloca1, align 8
  %calltmp5 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84"(ptr @str.const.88, ptr @str.const.82)
  br label %merge_13

merge_13:                                         ; preds = %then_12, %entry
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.90)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.94)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D9\82.load6" = load i64, ptr %alloca, align 8
  %"%\D8\AA.load7" = load i64, ptr %alloca1, align 8
  %calltmp8 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"(i64 %"%\D9\82.load6", i64 %"%\D8\AA.load7")
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.98)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.102)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp9 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.88)
  %"%\D8\AA.load10" = load i64, ptr %alloca1, align 8
  %subtmp = sub i64 %"%\D8\AA.load10", 1
  store i64 %subtmp, ptr %alloca1, align 8
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.107)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D9\82.load11" = load i64, ptr %alloca, align 8
  %"%\D8\AA.load12" = load i64, ptr %alloca1, align 8
  %calltmp13 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"(i64 %"%\D9\82.load11", i64 %"%\D8\AA.load12")
  %alloca14 = alloca i64, align 8
  store i64 %calltmp13, ptr %alloca14, align 8
  %"%\D9\822.load" = load i64, ptr %alloca14, align 8
  %cmpeqtmp15 = icmp eq i64 %"%\D9\822.load", 1
  br i1 %cmpeqtmp15, label %then_14, label %merge_15

then_14:                                          ; preds = %merge_13
  %"%\D9\82.load16" = load i64, ptr %alloca, align 8
  %addtmp17 = add i64 %"%\D9\82.load16", 1
  store i64 %addtmp17, ptr %alloca, align 8
  %calltmp18 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"(ptr @str.const.111, ptr @str.const.82)
  br label %merge_15

merge_15:                                         ; preds = %then_14, %merge_13
  %calltmp19 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.111)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.114)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\86\D9\82\D9\84_\D9\85\D9\84\D9\83\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.118)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.122)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.126)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A7\D9\84\D9\83"(ptr @str.const.130, ptr @str.const.131)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.132)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.136)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp1 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A7\D9\84\D9\83"(ptr @str.const.140, ptr @str.const.131)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.142)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.146)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp2 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"(ptr @str.const.111, ptr @str.const.140)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.152)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp3 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.111)
  %calltmp4 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.140)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.158)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D8\AF\D9\88\D8\B1\D8\A9_\D8\AD\D9\8A\D8\A7\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.162)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.166)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 1, ptr %alloca, align 8
  %alloca1 = alloca ptr, align 8
  %len1 = call i64 @strlen(ptr @str.const.30)
  %len2 = call i64 @strlen(ptr %alloca1)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %4 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.30, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %5 = call ptr @memcpy(ptr %dest2, ptr %alloca1, i64 %copylen2)
  %len12 = call i64 @strlen(ptr %concat.buf)
  %len23 = call i64 @strlen(ptr @str.const.171)
  %totallen4 = add i64 %len12, %len23
  %bufsize5 = add i64 %totallen4, 1
  %concat.buf6 = alloca i8, i64 %bufsize5, align 1
  %6 = call ptr @memcpy(ptr %concat.buf6, ptr %concat.buf, i64 %len12)
  %dest27 = getelementptr i8, ptr %concat.buf6, i64 %len12
  %copylen28 = add i64 %len23, 1
  %7 = call ptr @memcpy(ptr %dest27, ptr @str.const.171, i64 %copylen28)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf6)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D8\A7\D9\84\D9\83"(ptr @str.const.175, ptr @str.const.176)
  %"%\D8\AE\D8\B7\D9\88\D8\A9.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\AE\D8\B7\D9\88\D8\A9.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %len19 = call i64 @strlen(ptr @str.const.30)
  %len210 = call i64 @strlen(ptr %alloca1)
  %totallen11 = add i64 %len19, %len210
  %bufsize12 = add i64 %totallen11, 1
  %concat.buf13 = alloca i8, i64 %bufsize12, align 1
  %10 = call ptr @memcpy(ptr %concat.buf13, ptr @str.const.30, i64 %len19)
  %dest214 = getelementptr i8, ptr %concat.buf13, i64 %len19
  %copylen215 = add i64 %len210, 1
  %11 = call ptr @memcpy(ptr %dest214, ptr %alloca1, i64 %copylen215)
  %len116 = call i64 @strlen(ptr %concat.buf13)
  %len217 = call i64 @strlen(ptr @str.const.178)
  %totallen18 = add i64 %len116, %len217
  %bufsize19 = add i64 %totallen18, 1
  %concat.buf20 = alloca i8, i64 %bufsize19, align 1
  %12 = call ptr @memcpy(ptr %concat.buf20, ptr %concat.buf13, i64 %len116)
  %dest221 = getelementptr i8, ptr %concat.buf20, i64 %len116
  %copylen222 = add i64 %len217, 1
  %13 = call ptr @memcpy(ptr %dest221, ptr @str.const.178, i64 %copylen222)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf20)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp23 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"(ptr @str.const.182, ptr @str.const.175)
  %"%\D8\AE\D8\B7\D9\88\D8\A9.load24" = load i64, ptr %alloca, align 8
  %addtmp25 = add i64 %"%\D8\AE\D8\B7\D9\88\D8\A9.load24", 1
  store i64 %addtmp25, ptr %alloca, align 8
  %len126 = call i64 @strlen(ptr @str.const.30)
  %len227 = call i64 @strlen(ptr %alloca1)
  %totallen28 = add i64 %len126, %len227
  %bufsize29 = add i64 %totallen28, 1
  %concat.buf30 = alloca i8, i64 %bufsize29, align 1
  %16 = call ptr @memcpy(ptr %concat.buf30, ptr @str.const.30, i64 %len126)
  %dest231 = getelementptr i8, ptr %concat.buf30, i64 %len126
  %copylen232 = add i64 %len227, 1
  %17 = call ptr @memcpy(ptr %dest231, ptr %alloca1, i64 %copylen232)
  %len133 = call i64 @strlen(ptr %concat.buf30)
  %len234 = call i64 @strlen(ptr @str.const.185)
  %totallen35 = add i64 %len133, %len234
  %bufsize36 = add i64 %totallen35, 1
  %concat.buf37 = alloca i8, i64 %bufsize36, align 1
  %18 = call ptr @memcpy(ptr %concat.buf37, ptr %concat.buf30, i64 %len133)
  %dest238 = getelementptr i8, ptr %concat.buf37, i64 %len133
  %copylen239 = add i64 %len234, 1
  %19 = call ptr @memcpy(ptr %dest238, ptr @str.const.185, i64 %copylen239)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf37)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp40 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"(ptr @str.const.189, ptr @str.const.175)
  %"%\D8\AE\D8\B7\D9\88\D8\A9.load41" = load i64, ptr %alloca, align 8
  %addtmp42 = add i64 %"%\D8\AE\D8\B7\D9\88\D8\A9.load41", 1
  store i64 %addtmp42, ptr %alloca, align 8
  %len143 = call i64 @strlen(ptr @str.const.30)
  %len244 = call i64 @strlen(ptr %alloca1)
  %totallen45 = add i64 %len143, %len244
  %bufsize46 = add i64 %totallen45, 1
  %concat.buf47 = alloca i8, i64 %bufsize46, align 1
  %22 = call ptr @memcpy(ptr %concat.buf47, ptr @str.const.30, i64 %len143)
  %dest248 = getelementptr i8, ptr %concat.buf47, i64 %len143
  %copylen249 = add i64 %len244, 1
  %23 = call ptr @memcpy(ptr %dest248, ptr %alloca1, i64 %copylen249)
  %len150 = call i64 @strlen(ptr %concat.buf47)
  %len251 = call i64 @strlen(ptr @str.const.192)
  %totallen52 = add i64 %len150, %len251
  %bufsize53 = add i64 %totallen52, 1
  %concat.buf54 = alloca i8, i64 %bufsize53, align 1
  %24 = call ptr @memcpy(ptr %concat.buf54, ptr %concat.buf47, i64 %len150)
  %dest255 = getelementptr i8, ptr %concat.buf54, i64 %len150
  %copylen256 = add i64 %len251, 1
  %25 = call ptr @memcpy(ptr %dest255, ptr @str.const.192, i64 %copylen256)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf54)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp57 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.189)
  %calltmp58 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.182)
  %"%\D8\AE\D8\B7\D9\88\D8\A9.load59" = load i64, ptr %alloca, align 8
  %addtmp60 = add i64 %"%\D8\AE\D8\B7\D9\88\D8\A9.load59", 1
  store i64 %addtmp60, ptr %alloca, align 8
  %len161 = call i64 @strlen(ptr @str.const.30)
  %len262 = call i64 @strlen(ptr %alloca1)
  %totallen63 = add i64 %len161, %len262
  %bufsize64 = add i64 %totallen63, 1
  %concat.buf65 = alloca i8, i64 %bufsize64, align 1
  %28 = call ptr @memcpy(ptr %concat.buf65, ptr @str.const.30, i64 %len161)
  %dest266 = getelementptr i8, ptr %concat.buf65, i64 %len161
  %copylen267 = add i64 %len262, 1
  %29 = call ptr @memcpy(ptr %dest266, ptr %alloca1, i64 %copylen267)
  %len168 = call i64 @strlen(ptr %concat.buf65)
  %len269 = call i64 @strlen(ptr @str.const.199)
  %totallen70 = add i64 %len168, %len269
  %bufsize71 = add i64 %totallen70, 1
  %concat.buf72 = alloca i8, i64 %bufsize71, align 1
  %30 = call ptr @memcpy(ptr %concat.buf72, ptr %concat.buf65, i64 %len168)
  %dest273 = getelementptr i8, ptr %concat.buf72, i64 %len168
  %copylen274 = add i64 %len269, 1
  %31 = call ptr @memcpy(ptr %dest273, ptr @str.const.199, i64 %copylen274)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf72)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp75 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84"(ptr @str.const.88, ptr @str.const.175)
  %"%\D8\AE\D8\B7\D9\88\D8\A9.load76" = load i64, ptr %alloca, align 8
  %addtmp77 = add i64 %"%\D8\AE\D8\B7\D9\88\D8\A9.load76", 1
  store i64 %addtmp77, ptr %alloca, align 8
  %len178 = call i64 @strlen(ptr @str.const.30)
  %len279 = call i64 @strlen(ptr %alloca1)
  %totallen80 = add i64 %len178, %len279
  %bufsize81 = add i64 %totallen80, 1
  %concat.buf82 = alloca i8, i64 %bufsize81, align 1
  %34 = call ptr @memcpy(ptr %concat.buf82, ptr @str.const.30, i64 %len178)
  %dest283 = getelementptr i8, ptr %concat.buf82, i64 %len178
  %copylen284 = add i64 %len279, 1
  %35 = call ptr @memcpy(ptr %dest283, ptr %alloca1, i64 %copylen284)
  %len185 = call i64 @strlen(ptr %concat.buf82)
  %len286 = call i64 @strlen(ptr @str.const.206)
  %totallen87 = add i64 %len185, %len286
  %bufsize88 = add i64 %totallen87, 1
  %concat.buf89 = alloca i8, i64 %bufsize88, align 1
  %36 = call ptr @memcpy(ptr %concat.buf89, ptr %concat.buf82, i64 %len185)
  %dest290 = getelementptr i8, ptr %concat.buf89, i64 %len185
  %copylen291 = add i64 %len286, 1
  %37 = call ptr @memcpy(ptr %dest290, ptr @str.const.206, i64 %copylen291)
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf89)
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp92 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.88)
  %"%\D8\AE\D8\B7\D9\88\D8\A9.load93" = load i64, ptr %alloca, align 8
  %addtmp94 = add i64 %"%\D8\AE\D8\B7\D9\88\D8\A9.load93", 1
  store i64 %addtmp94, ptr %alloca, align 8
  %len195 = call i64 @strlen(ptr @str.const.30)
  %len296 = call i64 @strlen(ptr %alloca1)
  %totallen97 = add i64 %len195, %len296
  %bufsize98 = add i64 %totallen97, 1
  %concat.buf99 = alloca i8, i64 %bufsize98, align 1
  %40 = call ptr @memcpy(ptr %concat.buf99, ptr @str.const.30, i64 %len195)
  %dest2100 = getelementptr i8, ptr %concat.buf99, i64 %len195
  %copylen2101 = add i64 %len296, 1
  %41 = call ptr @memcpy(ptr %dest2100, ptr %alloca1, i64 %copylen2101)
  %len1102 = call i64 @strlen(ptr %concat.buf99)
  %len2103 = call i64 @strlen(ptr @str.const.212)
  %totallen104 = add i64 %len1102, %len2103
  %bufsize105 = add i64 %totallen104, 1
  %concat.buf106 = alloca i8, i64 %bufsize105, align 1
  %42 = call ptr @memcpy(ptr %concat.buf106, ptr %concat.buf99, i64 %len1102)
  %dest2107 = getelementptr i8, ptr %concat.buf106, i64 %len1102
  %copylen2108 = add i64 %len2103, 1
  %43 = call ptr @memcpy(ptr %dest2107, ptr @str.const.212, i64 %copylen2108)
  %44 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf106)
  %45 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp109 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"(ptr @str.const.175)
  %calltmp110 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AC\D8\A7\D8\AD"(ptr @str.const.217)
  %46 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.218)
  %47 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9"(ptr %"\D9\88\D8\B5\D9\81", i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\81\D9\89") {
entry:
  %"\D9\85\D8\B3\D8\AA\D9\88\D9\81\D9\892" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\81\D9\89", ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\81\D9\892", align 8
  %"\D9\88\D8\B5\D9\811" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81", ptr %"\D9\88\D8\B5\D9\811", align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\81\D9\89.load" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\81\D9\892", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\81\D9\89.load", 1
  br i1 %cmpeqtmp, label %then_16, label %else_18

then_16:                                          ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const.222)
  %len2 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\811")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.222, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\88\D8\B5\D9\811", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_17

else_18:                                          ; preds = %entry
  %len13 = call i64 @strlen(ptr @str.const.226)
  %len24 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\811")
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %4 = call ptr @memcpy(ptr %concat.buf7, ptr @str.const.226, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %5 = call ptr @memcpy(ptr %dest28, ptr %"\D9\88\D8\B5\D9\811", i64 %copylen29)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf7)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_17

merge_17:                                         ; preds = %else_18, %then_16
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D9\84\D8\AE\D8\B5"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.230)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.234)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9"(ptr @str.const.238, i64 1)
  %"%\D9\86.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\86.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp1 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9"(ptr @str.const.239, i64 1)
  %"%\D9\86.load2" = load i64, ptr %alloca, align 8
  %addtmp3 = add i64 %"%\D9\86.load2", 1
  store i64 %addtmp3, ptr %alloca, align 8
  %calltmp4 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9"(ptr @str.const.240, i64 1)
  %"%\D9\86.load5" = load i64, ptr %alloca, align 8
  %addtmp6 = add i64 %"%\D9\86.load5", 1
  store i64 %addtmp6, ptr %alloca, align 8
  %calltmp7 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9"(ptr @str.const.241, i64 1)
  %"%\D9\86.load8" = load i64, ptr %alloca, align 8
  %addtmp9 = add i64 %"%\D9\86.load8", 1
  store i64 %addtmp9, ptr %alloca, align 8
  %calltmp10 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9"(ptr @str.const.242, i64 1)
  %"%\D9\86.load11" = load i64, ptr %alloca, align 8
  %addtmp12 = add i64 %"%\D9\86.load11", 1
  store i64 %addtmp12, ptr %alloca, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.243)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.251)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.255)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.259)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.263)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9_\D9\85\D8\AA\D8\B9\D8\AF\D8\AF\D8\A9"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.267)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp1 = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D8\AA\D8\B9\D8\A7\D8\B1\D8\B6_\D8\AA\D8\B9\D8\AF\D9\8A\D9\84"()
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.271)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp2 = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D9\86\D9\82\D9\84_\D9\85\D9\84\D9\83\D9\8A\D8\A9"()
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.275)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp3 = call i64 @"\D9\85\D8\AB\D8\A7\D9\84_\D8\AF\D9\88\D8\B1\D8\A9_\D8\AD\D9\8A\D8\A7\D8\A9"()
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.279)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp4 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D9\84\D8\AE\D8\B5"()
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.283)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.287)
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
