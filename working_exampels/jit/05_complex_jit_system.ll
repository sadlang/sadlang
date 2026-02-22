; ModuleID = '05_complex_jit_system'
source_filename = "05_complex_jit_system"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [4 x i8] c"  [\00", align 1
@str.const.1 = private unnamed_addr constant [3 x i8] c"] \00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.2 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.ret = private unnamed_addr constant [15 x i8] c"\D9\85\D9\8F\D9\81\D8\B3\D9\8E\D9\91\D8\B1\00", align 1
@str.ret.4 = private unnamed_addr constant [15 x i8] c"JIT-\D8\A3\D8\B3\D8\A7\D8\B3\D9\8A\00", align 1
@str.ret.5 = private unnamed_addr constant [15 x i8] c"JIT-\D9\85\D8\AD\D8\B3\D9\91\D9\86\00", align 1
@str.ret.6 = private unnamed_addr constant [4 x i8] c"AOT\00", align 1
@str.const.7 = private unnamed_addr constant [5 x i8] c"    \00", align 1
@str.const.8 = private unnamed_addr constant [3 x i8] c": \00", align 1
@str.const.9 = private unnamed_addr constant [5 x i8] c" \C3\97 \00", align 1
@str.const.10 = private unnamed_addr constant [4 x i8] c" = \00", align 1
@str.const.13 = private unnamed_addr constant [60 x i8] c"=== \D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9: \D8\AE\D8\A7\D8\AF\D9\85 \D9\88\D9\8A\D8\A8 \D8\B9\D8\A7\D9\84\D9\8A \D8\A7\D9\84\D8\A3\D8\AF\D8\A7\D8\A1 ===\00", align 1
@str.const.17 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.21 = private unnamed_addr constant [64 x i8] c"  \E2\95\94\E2\95\90\E2\95\90 \D8\A7\D9\84\D8\AC\D8\B2\D8\A1 1: \D8\A7\D9\84\D8\AA\D8\B4\D8\BA\D9\8A\D9\84 \D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\8A \E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.25 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.29 = private unnamed_addr constant [5 x i8] c"INIT\00", align 1
@str.const.30 = private unnamed_addr constant [28 x i8] c"\D8\AA\D8\AD\D9\85\D9\8A\D9\84 \D8\A7\D9\84\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC\00", align 1
@str.const.32 = private unnamed_addr constant [47 x i8] c"\D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D9\84\D8\AF\D9\88\D8\A7\D9\84 \D9\81\D9\8A \D9\88\D8\B6\D8\B9 \D8\A7\D9\84\D9\85\D9\81\D8\B3\D8\B1\00", align 1
@str.const.33 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.37 = private unnamed_addr constant [33 x i8] c"    \D8\A7\D9\84\D8\AF\D9\88\D8\A7\D9\84 \D8\A7\D9\84\D9\85\D8\AD\D9\85\D9\84\D8\A9:\00", align 1
@str.const.41 = private unnamed_addr constant [53 x i8] c"      1. \D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9_\D8\B7\D9\84\D8\A8()      [\D9\85\D9\8F\D9\81\D8\B3\D9\8E\D9\91\D8\B1]\00", align 1
@str.const.45 = private unnamed_addr constant [50 x i8] c"      2. \D8\AA\D8\AD\D9\84\D9\8A\D9\84_HTTP()       [\D9\85\D9\8F\D9\81\D8\B3\D9\8E\D9\91\D8\B1]\00", align 1
@str.const.49 = private unnamed_addr constant [57 x i8] c"      3. \D8\AA\D8\B1\D9\85\D9\8A\D8\B2_\D8\A7\D8\B3\D8\AA\D8\AC\D8\A7\D8\A8\D8\A9()    [\D9\85\D9\8F\D9\81\D8\B3\D9\8E\D9\91\D8\B1]\00", align 1
@str.const.53 = private unnamed_addr constant [53 x i8] c"      4. \D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AF\D8\AE\D9\88\D9\84()      [\D9\85\D9\8F\D9\81\D8\B3\D9\8E\D9\91\D8\B1]\00", align 1
@str.const.57 = private unnamed_addr constant [54 x i8] c"      5. \D8\A5\D8\B9\D8\AF\D8\A7\D8\AF_\D8\AE\D8\A7\D8\AF\D9\85()       [\D9\85\D9\8F\D9\81\D8\B3\D9\8E\D9\91\D8\B1]\00", align 1
@str.const.61 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.65 = private unnamed_addr constant [71 x i8] c"  \E2\95\94\E2\95\90\E2\95\90 \D8\A7\D9\84\D8\AC\D8\B2\D8\A1 2: \D8\AC\D9\85\D8\B9 \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA \D8\A7\D9\84\D8\AA\D9\86\D9\81\D9\8A\D8\B0 \E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.69 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.73 = private unnamed_addr constant [38 x i8] c"    \D8\A8\D8\B9\D8\AF 5 \D8\AC\D9\88\D9\84\D8\A7\D8\AA \D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9:\00", align 1
@str.const.77 = private unnamed_addr constant [31 x i8] c"      \D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9_\D8\B7\D9\84\D8\A8:    \00", align 1
@str.const.78 = private unnamed_addr constant [14 x i8] c" \D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A9\00", align 1
@str.const.81 = private unnamed_addr constant [28 x i8] c"      \D8\AA\D8\AD\D9\84\D9\8A\D9\84_HTTP:     \00", align 1
@str.const.85 = private unnamed_addr constant [35 x i8] c"      \D8\AA\D8\B1\D9\85\D9\8A\D8\B2_\D8\A7\D8\B3\D8\AA\D8\AC\D8\A7\D8\A8\D8\A9:  \00", align 1
@str.const.89 = private unnamed_addr constant [31 x i8] c"      \D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AF\D8\AE\D9\88\D9\84:    \00", align 1
@str.const.93 = private unnamed_addr constant [32 x i8] c"      \D8\A5\D8\B9\D8\AF\D8\A7\D8\AF_\D8\AE\D8\A7\D8\AF\D9\85:     \00", align 1
@str.const.97 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.101 = private unnamed_addr constant [64 x i8] c"  \E2\95\94\E2\95\90\E2\95\90 \D8\A7\D9\84\D8\AC\D8\B2\D8\A1 3: \D9\82\D8\B1\D8\A7\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D8\AA\D8\B1\D8\AC\D9\85\D8\A9 \E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.105 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.109 = private unnamed_addr constant [4 x i8] c"JIT\00", align 1
@str.const.110 = private unnamed_addr constant [59 x i8] c"\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9_\D8\B7\D9\84\D8\A8 \E2\86\92 JIT \D8\A3\D8\B3\D8\A7\D8\B3\D9\8A (1000 \D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A9)\00", align 1
@str.const.112 = private unnamed_addr constant [54 x i8] c"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_HTTP \E2\86\92 JIT \D8\A3\D8\B3\D8\A7\D8\B3\D9\8A (900 \D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A9)\00", align 1
@str.const.114 = private unnamed_addr constant [64 x i8] c"\D8\AA\D8\B1\D9\85\D9\8A\D8\B2_\D8\A7\D8\B3\D8\AA\D8\AC\D8\A7\D8\A8\D8\A9 \E2\86\92 JIT \D8\A3\D8\B3\D8\A7\D8\B3\D9\8A (975 \D8\AA\D9\86\D9\81\D9\8A\D8\B0\D8\A9)\00", align 1
@str.const.115 = private unnamed_addr constant [3 x i8] c"VM\00", align 1
@str.const.116 = private unnamed_addr constant [63 x i8] c"\D8\AA\D8\B3\D8\AC\D9\8A\D9\84_\D8\AF\D8\AE\D9\88\D9\84 \E2\86\92 \D9\8A\D8\A8\D9\82\D9\89 \D9\81\D9\8A \D8\A7\D9\84\D9\85\D9\81\D8\B3\D8\B1 (50 \D9\81\D9\82\D8\B7)\00", align 1
@str.const.118 = private unnamed_addr constant [62 x i8] c"\D8\A5\D8\B9\D8\AF\D8\A7\D8\AF_\D8\AE\D8\A7\D8\AF\D9\85 \E2\86\92 \D9\8A\D8\A8\D9\82\D9\89 \D9\81\D9\8A \D8\A7\D9\84\D9\85\D9\81\D8\B3\D8\B1 (5 \D9\81\D9\82\D8\B7)\00", align 1
@str.const.119 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.123 = private unnamed_addr constant [51 x i8] c"  \E2\95\94\E2\95\90\E2\95\90 \D8\A7\D9\84\D8\AC\D8\B2\D8\A1 4: \D8\AA\D8\B1\D8\AC\D9\85\D8\A9 JIT \E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.127 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.131 = private unnamed_addr constant [6 x i8] c"BUILD\00", align 1
@str.const.132 = private unnamed_addr constant [78 x i8] c"\D8\A8\D9\86\D8\A7\D8\A1 \D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9_\D8\B7\D9\84\D8\A8: \D8\AA\D8\AD\D9\84\D9\8A\D9\84 \E2\86\92 \D8\AA\D8\AD\D8\B3\D9\8A\D9\86 \E2\86\92 \D8\AA\D9\88\D9\84\D9\8A\D8\AF \D9\83\D9\88\D8\AF\00", align 1
@str.const.133 = private unnamed_addr constant [71 x i8] c"    \D8\A7\D9\84\D8\AA\D8\AD\D8\B3\D9\8A\D9\86\D8\A7\D8\AA: \D8\AA\D8\B6\D9\85\D9\8A\D9\86 + \D8\B7\D9\8A \D8\AB\D9\88\D8\A7\D8\A8\D8\AA + \D9\81\D9\83 \D8\AD\D9\84\D9\82\D8\A7\D8\AA\00", align 1
@str.const.138 = private unnamed_addr constant [74 x i8] c"\D8\A8\D9\86\D8\A7\D8\A1 \D8\AA\D8\AD\D9\84\D9\8A\D9\84_HTTP: \D8\AA\D8\AD\D9\84\D9\8A\D9\84 \E2\86\92 \D8\AA\D8\AD\D8\B3\D9\8A\D9\86 \E2\86\92 \D8\AA\D9\88\D9\84\D9\8A\D8\AF \D9\83\D9\88\D8\AF\00", align 1
@str.const.139 = private unnamed_addr constant [62 x i8] c"    \D8\A7\D9\84\D8\AA\D8\AD\D8\B3\D9\8A\D9\86\D8\A7\D8\AA: \D8\AA\D8\B6\D9\85\D9\8A\D9\86 + \D8\AD\D8\B0\D9\81 \D9\81\D8\B1\D9\88\D8\B9 \D9\85\D9\8A\D8\AA\D8\A9\00", align 1
@str.const.144 = private unnamed_addr constant [84 x i8] c"\D8\A8\D9\86\D8\A7\D8\A1 \D8\AA\D8\B1\D9\85\D9\8A\D8\B2_\D8\A7\D8\B3\D8\AA\D8\AC\D8\A7\D8\A8\D8\A9: \D8\AA\D8\AD\D9\84\D9\8A\D9\84 \E2\86\92 \D8\AA\D8\AD\D8\B3\D9\8A\D9\86 \E2\86\92 \D8\AA\D9\88\D9\84\D9\8A\D8\AF \D9\83\D9\88\D8\AF\00", align 1
@str.const.145 = private unnamed_addr constant [68 x i8] c"    \D8\A7\D9\84\D8\AA\D8\AD\D8\B3\D9\8A\D9\86\D8\A7\D8\AA: \D8\AA\D8\AE\D8\B5\D9\8A\D8\B5 \D8\A7\D9\84\D8\A3\D9\86\D9\88\D8\A7\D8\B9 + \D9\81\D9\83 \D8\AD\D9\84\D9\82\D8\A7\D8\AA\00", align 1
@str.const.149 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.153 = private unnamed_addr constant [26 x i8] c"    \D8\AF\D9\88\D8\A7\D9\84 \D9\85\D8\A8\D9\86\D9\8A\D8\A9: \00", align 1
@str.const.154 = private unnamed_addr constant [5 x i8] c" / 5\00", align 1
@str.const.157 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.161 = private unnamed_addr constant [58 x i8] c"  \E2\95\94\E2\95\90\E2\95\90 \D8\A7\D9\84\D8\AC\D8\B2\D8\A1 5: \D9\82\D9\8A\D8\A7\D8\B3 \D8\A7\D9\84\D8\A3\D8\AF\D8\A7\D8\A1 \E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.165 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.169 = private unnamed_addr constant [27 x i8] c"    \D9\82\D8\A8\D9\84 JIT (\D9\85\D9\81\D8\B3\D8\B1):\00", align 1
@str.const.182 = private unnamed_addr constant [23 x i8] c"      \D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9: \00", align 1
@str.const.185 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.189 = private unnamed_addr constant [16 x i8] c"    \D8\A8\D8\B9\D8\AF JIT:\00", align 1
@str.const.205 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.209 = private unnamed_addr constant [19 x i8] c"    \D8\A7\D9\84\D8\AA\D8\AD\D8\B3\D9\86: \00", align 1
@str.const.210 = private unnamed_addr constant [3 x i8] c" (\00", align 1
@str.const.211 = private unnamed_addr constant [3 x i8] c"%)\00", align 1
@str.const.214 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.218 = private unnamed_addr constant [62 x i8] c"  \E2\95\94\E2\95\90\E2\95\90 \D8\A7\D9\84\D8\AC\D8\B2\D8\A1 6: \D8\AA\D8\B1\D9\82\D9\8A\D8\A9 JIT \D9\85\D8\AD\D8\B3\D9\91\D9\86 \E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.222 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.226 = private unnamed_addr constant [6 x i8] c"TIER2\00", align 1
@str.const.227 = private unnamed_addr constant [65 x i8] c"\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9_\D8\B7\D9\84\D8\A8 \E2\86\92 JIT \D9\85\D8\AD\D8\B3\D9\91\D9\86 (\D8\A7\D9\84\D8\A3\D9\83\D8\AB\D8\B1 \D8\B3\D8\AE\D9\88\D9\86\D8\A9)\00", align 1
@str.const.228 = private unnamed_addr constant [41 x i8] c"    \D8\A7\D9\84\D8\AA\D8\AD\D8\B3\D9\8A\D9\86\D8\A7\D8\AA \D8\A7\D9\84\D8\A5\D8\B6\D8\A7\D9\81\D9\8A\D8\A9:\00", align 1
@str.const.232 = private unnamed_addr constant [56 x i8] c"      - \D8\AA\D8\AE\D8\B5\D9\8A\D8\B5 \D8\B9\D9\84\D9\89 \D8\A7\D9\84\D8\A3\D9\86\D9\88\D8\A7\D8\B9 \D8\A7\D9\84\D9\81\D8\B9\D9\84\D9\8A\D8\A9\00", align 1
@str.const.236 = private unnamed_addr constant [35 x i8] c"      - \D9\86\D8\B4\D8\B1 \D9\86\D8\B7\D8\A7\D9\82 \D8\A7\D9\84\D9\82\D9\8A\D9\85\00", align 1
@str.const.240 = private unnamed_addr constant [38 x i8] c"      - \D8\AC\D8\AF\D9\88\D9\84\D8\A9 \D8\A7\D9\84\D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A7\D8\AA\00", align 1
@str.const.244 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.248 = private unnamed_addr constant [48 x i8] c"    \D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9_\D8\B7\D9\84\D8\A8 \D8\A8\D8\B9\D8\AF JIT \D9\85\D8\AD\D8\B3\D9\91\D9\86: \00", align 1
@str.const.251 = private unnamed_addr constant [26 x i8] c"    \D8\AA\D8\AD\D8\B3\D9\86 \D8\A5\D8\B6\D8\A7\D9\81\D9\8A: \00", align 1
@str.const.254 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.258 = private unnamed_addr constant [48 x i8] c"=== \D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9 OSR (On-Stack Replacement) ===\00", align 1
@str.const.262 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.266 = private unnamed_addr constant [83 x i8] c"  \D8\A7\D9\84\D8\B3\D9\8A\D9\86\D8\A7\D8\B1\D9\8A\D9\88: \D8\AD\D9\84\D9\82\D8\A9 \D8\B7\D9\88\D9\8A\D9\84\D8\A9 \D8\A7\D9\83\D8\AA\D9\8F\D8\B4\D9\81\D8\AA \D8\A3\D8\AB\D9\86\D8\A7\D8\A1 \D8\A7\D9\84\D8\AA\D9\86\D9\81\D9\8A\D8\B0\00", align 1
@str.const.270 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.274 = private unnamed_addr constant [32 x i8] c"  \D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9: \D9\85\D8\AC\D9\85\D9\88\D8\B9 = \00", align 1
@str.const.277 = private unnamed_addr constant [41 x i8] c"  \D8\A7\D9\84\D8\AA\D9\83\D8\B1\D8\A7\D8\B1\D8\A7\D8\AA \D9\81\D9\8A \D8\A7\D9\84\D9\85\D9\81\D8\B3\D8\B1: \00", align 1
@str.const.280 = private unnamed_addr constant [32 x i8] c"  \D8\A7\D9\84\D8\AA\D9\83\D8\B1\D8\A7\D8\B1\D8\A7\D8\AA \D9\81\D9\8A JIT: \00", align 1
@str.const.283 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.287 = private unnamed_addr constant [24 x i8] c"  [OSR] \D8\A7\D9\84\D8\AA\D9\83\D8\B1\D8\A7\D8\B1 \00", align 1
@str.const.288 = private unnamed_addr constant [30 x i8] c": \D9\83\D8\B4\D9\81 \D8\AD\D9\84\D9\82\D8\A9 \D8\B3\D8\A7\D8\AE\D9\86\D8\A9!\00", align 1
@str.const.291 = private unnamed_addr constant [40 x i8] c"  [OSR] \D8\AD\D9\81\D8\B8 \D8\AD\D8\A7\D9\84\D8\A9 \D8\A7\D9\84\D9\85\D9\83\D8\AF\D8\B3...\00", align 1
@str.const.295 = private unnamed_addr constant [39 x i8] c"  [OSR] \D8\AA\D8\B1\D8\AC\D9\85\D8\A9 JIT \D9\84\D9\84\D8\AD\D9\84\D9\82\D8\A9...\00", align 1
@str.const.299 = private unnamed_addr constant [60 x i8] c"  [OSR] \D8\A7\D8\B3\D8\AA\D8\A8\D8\AF\D8\A7\D9\84 \D8\A5\D8\B7\D8\A7\D8\B1 \D8\A7\D9\84\D9\85\D9\81\D8\B3\D8\B1 \D8\A8\D8\A5\D8\B7\D8\A7\D8\B1 JIT\00", align 1
@str.const.303 = private unnamed_addr constant [47 x i8] c"  [OSR] \D8\A7\D8\B3\D8\AA\D8\A6\D9\86\D8\A7\D9\81 \D8\A7\D9\84\D8\AA\D9\86\D9\81\D9\8A\D8\B0 \D8\A8\D9\80 JIT\00", align 1
@str.const.307 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.311 = private unnamed_addr constant [36 x i8] c"=== \D8\A7\D9\84\D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D8\B4\D8\A7\D9\85\D9\84 ===\00", align 1
@str.const.315 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.319 = private unnamed_addr constant [29 x i8] c"  \D9\85\D9\83\D9\88\D9\86\D8\A7\D8\AA \D9\86\D8\B8\D8\A7\D9\85 JIT:\00", align 1
@str.const.323 = private unnamed_addr constant [31 x i8] c"    1. \D8\A7\D9\84\D9\85\D8\AD\D9\84\D9\84 (Profiler)\00", align 1
@str.const.327 = private unnamed_addr constant [45 x i8] c"       - \D8\B9\D8\AF\D8\A7\D8\AF \D8\AA\D9\86\D9\81\D9\8A\D8\B0 \D9\84\D9\83\D9\84 \D8\AF\D8\A7\D9\84\D8\A9\00", align 1
@str.const.331 = private unnamed_addr constant [46 x i8] c"       - \D9\83\D8\B4\D9\81 \D8\A7\D9\84\D8\AD\D9\84\D9\82\D8\A7\D8\AA \D8\A7\D9\84\D8\B3\D8\A7\D8\AE\D9\86\D8\A9\00", align 1
@str.const.335 = private unnamed_addr constant [44 x i8] c"       - \D8\AA\D8\AA\D8\A8\D8\B9 \D8\A3\D9\86\D9\88\D8\A7\D8\B9 \D8\A7\D9\84\D8\AA\D8\B4\D8\BA\D9\8A\D9\84\00", align 1
@str.const.339 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.343 = private unnamed_addr constant [52 x i8] c"    2. \D8\A7\D9\84\D9\85\D8\A8\D9\86\D9\8A \D8\A7\D9\84\D9\85\D8\AA\D8\AF\D8\B1\D8\AC (Tiered Builder)\00", align 1
@str.const.347 = private unnamed_addr constant [43 x i8] c"       - \D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9 0: \D9\85\D9\81\D8\B3\D8\B1 bytecode\00", align 1
@str.const.351 = private unnamed_addr constant [60 x i8] c"       - \D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9 1: JIT \D8\A3\D8\B3\D8\A7\D8\B3\D9\8A (\D8\A8\D9\86\D8\A7\D8\A1 \D8\B3\D8\B1\D9\8A\D8\B9)\00", align 1
@str.const.355 = private unnamed_addr constant [68 x i8] c"       - \D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A9 2: JIT \D9\85\D8\AD\D8\B3\D9\91\D9\86 (\D8\AA\D8\AD\D8\B3\D9\8A\D9\86\D8\A7\D8\AA \D9\83\D8\A7\D9\85\D9\84\D8\A9)\00", align 1
@str.const.359 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.363 = private unnamed_addr constant [41 x i8] c"    3. OSR (\D8\A7\D8\B3\D8\AA\D8\A8\D8\AF\D8\A7\D9\84 \D8\A7\D9\84\D9\85\D9\83\D8\AF\D8\B3)\00", align 1
@str.const.367 = private unnamed_addr constant [70 x i8] c"       - \D8\AA\D8\A8\D8\AF\D9\8A\D9\84 \D8\A7\D9\84\D9\85\D9\81\D8\B3\D8\B1 \D8\A5\D9\84\D9\89 JIT \D8\A3\D8\AB\D9\86\D8\A7\D8\A1 \D8\A7\D9\84\D8\AA\D9\86\D9\81\D9\8A\D8\B0\00", align 1
@str.const.371 = private unnamed_addr constant [52 x i8] c"       - \D8\AD\D9\81\D8\B8 \D9\88\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\AF\D8\A9 \D8\A7\D9\84\D9\85\D8\AA\D8\BA\D9\8A\D8\B1\D8\A7\D8\AA\00", align 1
@str.const.375 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.379 = private unnamed_addr constant [34 x i8] c"    4. \D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\91\D9\86 (Optimizer)\00", align 1
@str.const.383 = private unnamed_addr constant [29 x i8] c"       - \D8\B7\D9\8A \D8\A7\D9\84\D8\AB\D9\88\D8\A7\D8\A8\D8\AA\00", align 1
@str.const.387 = private unnamed_addr constant [33 x i8] c"       - \D8\AA\D8\B6\D9\85\D9\8A\D9\86 \D8\A7\D9\84\D8\AF\D9\88\D8\A7\D9\84\00", align 1
@str.const.391 = private unnamed_addr constant [38 x i8] c"       - \D8\AD\D8\B0\D9\81 \D8\A7\D9\84\D9\83\D9\88\D8\AF \D8\A7\D9\84\D9\85\D9\8A\D8\AA\00", align 1
@str.const.395 = private unnamed_addr constant [35 x i8] c"       - \D8\AA\D8\AE\D8\B5\D9\8A\D8\B5 \D8\A7\D9\84\D8\A3\D9\86\D9\88\D8\A7\D8\B9\00", align 1
@str.const.399 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.403 = private unnamed_addr constant [64 x i8] c"  \D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9: \D9\86\D8\B8\D8\A7\D9\85 JIT \D9\85\D8\AA\D9\83\D8\A7\D9\85\D9\84 \D9\8A\D8\B9\D9\85\D9\84 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD\00", align 1
@str.const.407 = private unnamed_addr constant [121 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.411 = private unnamed_addr constant [59 x i8] c"\E2\95\91  \D9\86\D8\B8\D8\A7\D9\85 \D8\AA\D8\B1\D8\AC\D9\85\D8\A9 JIT \D8\A7\D9\84\D9\83\D8\A7\D9\85\D9\84              \E2\95\91\00", align 1
@str.const.415 = private unnamed_addr constant [121 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.419 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.423 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.427 = private unnamed_addr constant [32 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 \D8\A7\D9\84\D9\86\D8\B8\D8\A7\D9\85 ===\00", align 1

define i64 @"\D8\B3\D8\AC_\D9\86\D8\B8\D8\A7\D9\85"(ptr %"\D9\82\D8\B7\D8\A7\D8\B9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92" = alloca ptr, align 8
  store ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92", align 8
  %"\D9\82\D8\B7\D8\A7\D8\B91" = alloca ptr, align 8
  store ptr %"\D9\82\D8\B7\D8\A7\D8\B9", ptr %"\D9\82\D8\B7\D8\A7\D8\B91", align 8
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %"\D9\82\D8\B7\D8\A7\D8\B91")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\82\D8\B7\D8\A7\D8\B91", i64 %copylen2)
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
  %len211 = call i64 @strlen(ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92")
  %totallen12 = add i64 %len110, %len211
  %bufsize13 = add i64 %totallen12, 1
  %concat.buf14 = alloca i8, i64 %bufsize13, align 1
  %4 = call ptr @memcpy(ptr %concat.buf14, ptr %concat.buf7, i64 %len110)
  %dest215 = getelementptr i8, ptr %concat.buf14, i64 %len110
  %copylen216 = add i64 %len211, 1
  %5 = call ptr @memcpy(ptr %dest215, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92", i64 %copylen216)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf14)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define ptr @"\D9\88\D8\B6\D8\B9_\D8\AF\D8\A7\D9\84\D8\A9"(i64 %"\D8\AF\D8\B1\D8\AC\D8\A9_\D8\AA\D8\B1\D8\AC\D9\85\D8\A9") {
entry:
  %"\D8\AF\D8\B1\D8\AC\D8\A9_\D8\AA\D8\B1\D8\AC\D9\85\D8\A91" = alloca i64, align 8
  store i64 %"\D8\AF\D8\B1\D8\AC\D8\A9_\D8\AA\D8\B1\D8\AC\D9\85\D8\A9", ptr %"\D8\AF\D8\B1\D8\AC\D8\A9_\D8\AA\D8\B1\D8\AC\D9\85\D8\A91", align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\AA\D8\B1\D8\AC\D9\85\D8\A9.load" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A9_\D8\AA\D8\B1\D8\AC\D9\85\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\AA\D8\B1\D8\AC\D9\85\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_0, label %merge_1

then_0:                                           ; preds = %entry
  ret ptr @str.ret

merge_1:                                          ; preds = %entry
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\AA\D8\B1\D8\AC\D9\85\D8\A9.load2" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A9_\D8\AA\D8\B1\D8\AC\D9\85\D8\A91", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\AA\D8\B1\D8\AC\D9\85\D8\A9.load2", 1
  br i1 %cmpeqtmp3, label %then_2, label %merge_3

then_2:                                           ; preds = %merge_1
  ret ptr @str.ret.4

merge_3:                                          ; preds = %merge_1
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\AA\D8\B1\D8\AC\D9\85\D8\A9.load4" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A9_\D8\AA\D8\B1\D8\AC\D9\85\D8\A91", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_\D8\AA\D8\B1\D8\AC\D9\85\D8\A9.load4", 2
  br i1 %cmpeqtmp5, label %then_4, label %merge_5

then_4:                                           ; preds = %merge_3
  ret ptr @str.ret.5

merge_5:                                          ; preds = %merge_3
  ret ptr @str.ret.6
}

define i64 @"\D8\AD\D8\B3\D8\A7\D8\A8_\D8\A3\D8\AF\D8\A7\D8\A1"(i64 %"\D8\AF\D8\B1\D8\AC\D8\A9") {
entry:
  %"\D8\AF\D8\B1\D8\AC\D8\A91" = alloca i64, align 8
  store i64 %"\D8\AF\D8\B1\D8\AC\D8\A9", ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_6, label %merge_7

then_6:                                           ; preds = %entry
  ret i64 100

merge_7:                                          ; preds = %entry
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load2" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load2", 1
  br i1 %cmpeqtmp3, label %then_8, label %merge_9

then_8:                                           ; preds = %merge_7
  ret i64 20

merge_9:                                          ; preds = %merge_7
  %"%\D8\AF\D8\B1\D8\AC\D8\A9.load4" = load i64, ptr %"\D8\AF\D8\B1\D8\AC\D8\A91", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9.load4", 2
  br i1 %cmpeqtmp5, label %then_10, label %merge_11

then_10:                                          ; preds = %merge_9
  ret i64 5

merge_11:                                         ; preds = %merge_9
  ret i64 2
}

define i64 @"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_\D8\AA\D9\86\D9\81\D9\8A\D8\B0"(ptr %"\D8\A7\D8\B3\D9\85", i64 %"\D8\AA\D9\83\D8\B1\D8\A7\D8\B1", i64 %"\D8\B2\D9\85\D9\86_\D9\83\D9\84") {
entry:
  %"\D8\B2\D9\85\D9\86_\D9\83\D9\843" = alloca i64, align 8
  store i64 %"\D8\B2\D9\85\D9\86_\D9\83\D9\84", ptr %"\D8\B2\D9\85\D9\86_\D9\83\D9\843", align 8
  %"\D8\AA\D9\83\D8\B1\D8\A7\D8\B12" = alloca i64, align 8
  store i64 %"\D8\AA\D9\83\D8\B1\D8\A7\D8\B1", ptr %"\D8\AA\D9\83\D8\B1\D8\A7\D8\B12", align 8
  %"\D8\A7\D8\B3\D9\851" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\851", align 8
  %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1.load" = load i64, ptr %"\D8\AA\D9\83\D8\B1\D8\A7\D8\B12", align 8
  %"%\D8\B2\D9\85\D9\86_\D9\83\D9\84.load" = load i64, ptr %"\D8\B2\D9\85\D9\86_\D9\83\D9\843", align 8
  %multmp = mul i64 %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1.load", %"%\D8\B2\D9\85\D9\86_\D9\83\D9\84.load"
  %alloca = alloca i64, align 8
  store i64 %multmp, ptr %alloca, align 8
  %len1 = call i64 @strlen(ptr @str.const.7)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\851")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.7, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D8\B3\D9\851", i64 %copylen2)
  %len14 = call i64 @strlen(ptr %concat.buf)
  %len25 = call i64 @strlen(ptr @str.const.8)
  %totallen6 = add i64 %len14, %len25
  %bufsize7 = add i64 %totallen6, 1
  %concat.buf8 = alloca i8, i64 %bufsize7, align 1
  %2 = call ptr @memcpy(ptr %concat.buf8, ptr %concat.buf, i64 %len14)
  %dest29 = getelementptr i8, ptr %concat.buf8, i64 %len14
  %copylen210 = add i64 %len25, 1
  %3 = call ptr @memcpy(ptr %dest29, ptr @str.const.8, i64 %copylen210)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %loadtmp = load i64, ptr %alloca, align 8
  ret i64 %loadtmp
}

define i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\AE\D8\A7\D8\AF\D9\85_\D9\88\D9\8A\D8\A8"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.13)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.17)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.21)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.25)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D8\B3\D8\AC_\D9\86\D8\B8\D8\A7\D9\85"(ptr @str.const.29, ptr @str.const.30)
  %calltmp1 = call i64 @"\D8\B3\D8\AC_\D9\86\D8\B8\D8\A7\D9\85"(ptr @str.const.29, ptr @str.const.32)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.33)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.37)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.41)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.45)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.49)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.53)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.57)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.61)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.65)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.69)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca2 = alloca i64, align 8
  store i64 0, ptr %alloca2, align 8
  %alloca3 = alloca i64, align 8
  store i64 0, ptr %alloca3, align 8
  %alloca4 = alloca i64, align 8
  store i64 0, ptr %alloca4, align 8
  %alloca5 = alloca i64, align 8
  store i64 0, ptr %alloca5, align 8
  %alloca6 = alloca i64, align 8
  store i64 0, ptr %alloca6, align 8
  br label %while_cond_12

while_cond_12:                                    ; preds = %while_body_13, %entry
  %"%\D8\AC\D9\88\D9\84\D8\A9.load" = load i64, ptr %alloca6, align 8
  %cmplttmp = icmp slt i64 %"%\D8\AC\D9\88\D9\84\D8\A9.load", 5
  br i1 %cmplttmp, label %while_body_13, label %while_exit_14

while_body_13:                                    ; preds = %while_cond_12
  %"%\D8\B91.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\B91.load", 200
  store i64 %addtmp, ptr %alloca, align 8
  %"%\D8\B92.load" = load i64, ptr %alloca2, align 8
  %addtmp7 = add i64 %"%\D8\B92.load", 180
  store i64 %addtmp7, ptr %alloca2, align 8
  %"%\D8\B93.load" = load i64, ptr %alloca3, align 8
  %addtmp8 = add i64 %"%\D8\B93.load", 195
  store i64 %addtmp8, ptr %alloca3, align 8
  %"%\D8\B94.load" = load i64, ptr %alloca4, align 8
  %addtmp9 = add i64 %"%\D8\B94.load", 10
  store i64 %addtmp9, ptr %alloca4, align 8
  %"%\D8\B95.load" = load i64, ptr %alloca5, align 8
  %addtmp10 = add i64 %"%\D8\B95.load", 1
  store i64 %addtmp10, ptr %alloca5, align 8
  %"%\D8\AC\D9\88\D9\84\D8\A9.load11" = load i64, ptr %alloca6, align 8
  %addtmp12 = add i64 %"%\D8\AC\D9\88\D9\84\D8\A9.load11", 1
  store i64 %addtmp12, ptr %alloca6, align 8
  br label %while_cond_12

while_exit_14:                                    ; preds = %while_cond_12
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.73)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.97)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.101)
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.105)
  %40 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca13 = alloca i64, align 8
  store i64 0, ptr %alloca13, align 8
  %alloca14 = alloca i64, align 8
  store i64 0, ptr %alloca14, align 8
  %alloca15 = alloca i64, align 8
  store i64 0, ptr %alloca15, align 8
  %"%\D8\B91.load16" = load i64, ptr %alloca, align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B91.load16", 100
  br i1 %cmpgetmp, label %then_15, label %merge_16

then_15:                                          ; preds = %while_exit_14
  store i64 1, ptr %alloca13, align 8
  %calltmp17 = call i64 @"\D8\B3\D8\AC_\D9\86\D8\B8\D8\A7\D9\85"(ptr @str.const.109, ptr @str.const.110)
  br label %merge_16

merge_16:                                         ; preds = %then_15, %while_exit_14
  %"%\D8\B92.load18" = load i64, ptr %alloca2, align 8
  %cmpgetmp19 = icmp sge i64 %"%\D8\B92.load18", 100
  br i1 %cmpgetmp19, label %then_17, label %merge_18

then_17:                                          ; preds = %merge_16
  store i64 1, ptr %alloca14, align 8
  %calltmp20 = call i64 @"\D8\B3\D8\AC_\D9\86\D8\B8\D8\A7\D9\85"(ptr @str.const.109, ptr @str.const.112)
  br label %merge_18

merge_18:                                         ; preds = %then_17, %merge_16
  %"%\D8\B93.load21" = load i64, ptr %alloca3, align 8
  %cmpgetmp22 = icmp sge i64 %"%\D8\B93.load21", 100
  br i1 %cmpgetmp22, label %then_19, label %merge_20

then_19:                                          ; preds = %merge_18
  store i64 1, ptr %alloca15, align 8
  %calltmp23 = call i64 @"\D8\B3\D8\AC_\D9\86\D8\B8\D8\A7\D9\85"(ptr @str.const.109, ptr @str.const.114)
  br label %merge_20

merge_20:                                         ; preds = %then_19, %merge_18
  %"%\D8\B94.load24" = load i64, ptr %alloca4, align 8
  %cmplttmp25 = icmp slt i64 %"%\D8\B94.load24", 100
  br i1 %cmplttmp25, label %then_21, label %merge_22

then_21:                                          ; preds = %merge_20
  %calltmp26 = call i64 @"\D8\B3\D8\AC_\D9\86\D8\B8\D8\A7\D9\85"(ptr @str.const.115, ptr @str.const.116)
  br label %merge_22

merge_22:                                         ; preds = %then_21, %merge_20
  %"%\D8\B95.load27" = load i64, ptr %alloca5, align 8
  %cmplttmp28 = icmp slt i64 %"%\D8\B95.load27", 100
  br i1 %cmplttmp28, label %then_23, label %merge_24

then_23:                                          ; preds = %merge_22
  %calltmp29 = call i64 @"\D8\B3\D8\AC_\D9\86\D8\B8\D8\A7\D9\85"(ptr @str.const.115, ptr @str.const.118)
  br label %merge_24

merge_24:                                         ; preds = %then_23, %merge_22
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.119)
  %42 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %43 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.123)
  %44 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %45 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.127)
  %46 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca30 = alloca i64, align 8
  store i64 0, ptr %alloca30, align 8
  %"%\D9\851.load" = load i64, ptr %alloca13, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\851.load", 1
  br i1 %cmpeqtmp, label %then_25, label %merge_26

then_25:                                          ; preds = %merge_24
  %calltmp31 = call i64 @"\D8\B3\D8\AC_\D9\86\D8\B8\D8\A7\D9\85"(ptr @str.const.131, ptr @str.const.132)
  %47 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.133)
  %48 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\AF\D9\88\D8\A7\D9\84_\D9\85\D8\A8\D9\86\D9\8A\D8\A9.load" = load i64, ptr %alloca30, align 8
  %addtmp32 = add i64 %"%\D8\AF\D9\88\D8\A7\D9\84_\D9\85\D8\A8\D9\86\D9\8A\D8\A9.load", 1
  store i64 %addtmp32, ptr %alloca30, align 8
  br label %merge_26

merge_26:                                         ; preds = %then_25, %merge_24
  %"%\D9\852.load" = load i64, ptr %alloca14, align 8
  %cmpeqtmp33 = icmp eq i64 %"%\D9\852.load", 1
  br i1 %cmpeqtmp33, label %then_27, label %merge_28

then_27:                                          ; preds = %merge_26
  %calltmp34 = call i64 @"\D8\B3\D8\AC_\D9\86\D8\B8\D8\A7\D9\85"(ptr @str.const.131, ptr @str.const.138)
  %49 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.139)
  %50 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\AF\D9\88\D8\A7\D9\84_\D9\85\D8\A8\D9\86\D9\8A\D8\A9.load35" = load i64, ptr %alloca30, align 8
  %addtmp36 = add i64 %"%\D8\AF\D9\88\D8\A7\D9\84_\D9\85\D8\A8\D9\86\D9\8A\D8\A9.load35", 1
  store i64 %addtmp36, ptr %alloca30, align 8
  br label %merge_28

merge_28:                                         ; preds = %then_27, %merge_26
  %"%\D9\853.load" = load i64, ptr %alloca15, align 8
  %cmpeqtmp37 = icmp eq i64 %"%\D9\853.load", 1
  br i1 %cmpeqtmp37, label %then_29, label %merge_30

then_29:                                          ; preds = %merge_28
  %calltmp38 = call i64 @"\D8\B3\D8\AC_\D9\86\D8\B8\D8\A7\D9\85"(ptr @str.const.131, ptr @str.const.144)
  %51 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.145)
  %52 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\AF\D9\88\D8\A7\D9\84_\D9\85\D8\A8\D9\86\D9\8A\D8\A9.load39" = load i64, ptr %alloca30, align 8
  %addtmp40 = add i64 %"%\D8\AF\D9\88\D8\A7\D9\84_\D9\85\D8\A8\D9\86\D9\8A\D8\A9.load39", 1
  store i64 %addtmp40, ptr %alloca30, align 8
  br label %merge_30

merge_30:                                         ; preds = %then_29, %merge_28
  %53 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.149)
  %54 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %55 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %56 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.157)
  %57 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %58 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.161)
  %59 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %60 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.165)
  %61 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %62 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.169)
  %63 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca41 = alloca i64, align 8
  store i64 100000, ptr %alloca41, align 8
  %alloca42 = alloca i64, align 8
  store i64 90000, ptr %alloca42, align 8
  %alloca43 = alloca i64, align 8
  store i64 97500, ptr %alloca43, align 8
  %64 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %65 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %66 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\B2_\D9\82\D8\A8\D9\84_1.load" = load i64, ptr %alloca41, align 8
  %"%\D8\B2_\D9\82\D8\A8\D9\84_2.load" = load i64, ptr %alloca42, align 8
  %addtmp44 = add i64 %"%\D8\B2_\D9\82\D8\A8\D9\84_1.load", %"%\D8\B2_\D9\82\D8\A8\D9\84_2.load"
  %"%\D8\B2_\D9\82\D8\A8\D9\84_3.load" = load i64, ptr %alloca43, align 8
  %addtmp45 = add i64 %addtmp44, %"%\D8\B2_\D9\82\D8\A8\D9\84_3.load"
  %alloca46 = alloca i64, align 8
  store i64 %addtmp45, ptr %alloca46, align 8
  %67 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %68 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.185)
  %69 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %70 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.189)
  %71 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca47 = alloca i64, align 8
  store i64 20000, ptr %alloca47, align 8
  %alloca48 = alloca i64, align 8
  store i64 18000, ptr %alloca48, align 8
  %alloca49 = alloca i64, align 8
  store i64 19500, ptr %alloca49, align 8
  %72 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %73 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %74 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\B2_\D8\A8\D8\B9\D8\AF_1.load" = load i64, ptr %alloca47, align 8
  %"%\D8\B2_\D8\A8\D8\B9\D8\AF_2.load" = load i64, ptr %alloca48, align 8
  %addtmp50 = add i64 %"%\D8\B2_\D8\A8\D8\B9\D8\AF_1.load", %"%\D8\B2_\D8\A8\D8\B9\D8\AF_2.load"
  %"%\D8\B2_\D8\A8\D8\B9\D8\AF_3.load" = load i64, ptr %alloca49, align 8
  %addtmp51 = add i64 %addtmp50, %"%\D8\B2_\D8\A8\D8\B9\D8\AF_3.load"
  %alloca52 = alloca i64, align 8
  store i64 %addtmp51, ptr %alloca52, align 8
  %75 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %76 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.205)
  %77 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D9\82\D8\A8\D9\84.load" = load i64, ptr %alloca46, align 8
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\A8\D8\B9\D8\AF.load" = load i64, ptr %alloca52, align 8
  %subtmp = sub i64 %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D9\82\D8\A8\D9\84.load", %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\A8\D8\B9\D8\AF.load"
  %alloca53 = alloca i64, align 8
  store i64 %subtmp, ptr %alloca53, align 8
  %"%\D8\AA\D8\AD\D8\B3\D9\86.load" = load i64, ptr %alloca53, align 8
  %multmp = mul i64 %"%\D8\AA\D8\AD\D8\B3\D9\86.load", 100
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D9\82\D8\A8\D9\84.load54" = load i64, ptr %alloca46, align 8
  %divtmp = sdiv i64 %multmp, %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D9\82\D8\A8\D9\84.load54"
  %alloca55 = alloca i64, align 8
  store i64 %divtmp, ptr %alloca55, align 8
  %78 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %79 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.214)
  %80 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %81 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.218)
  %82 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %83 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.222)
  %84 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\B91.load56" = load i64, ptr %alloca, align 8
  %cmpgetmp57 = icmp sge i64 %"%\D8\B91.load56", 500
  br i1 %cmpgetmp57, label %then_31, label %merge_32

then_31:                                          ; preds = %merge_30
  %calltmp58 = call i64 @"\D8\B3\D8\AC_\D9\86\D8\B8\D8\A7\D9\85"(ptr @str.const.226, ptr @str.const.227)
  %85 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.228)
  %86 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %87 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.232)
  %88 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %89 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.236)
  %90 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %91 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.240)
  %92 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_32

merge_32:                                         ; preds = %then_31, %merge_30
  %93 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.244)
  %94 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca59 = alloca i64, align 8
  store i64 5000, ptr %alloca59, align 8
  %95 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\B2_\D8\A8\D8\B9\D8\AF_1.load60" = load i64, ptr %alloca47, align 8
  %"%\D8\B2_\D9\85\D8\AD\D8\B3\D9\86.load" = load i64, ptr %alloca59, align 8
  %subtmp61 = sub i64 %"%\D8\B2_\D8\A8\D8\B9\D8\AF_1.load60", %"%\D8\B2_\D9\85\D8\AD\D8\B3\D9\86.load"
  %alloca62 = alloca i64, align 8
  store i64 %subtmp61, ptr %alloca62, align 8
  %96 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %97 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.254)
  %98 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\A7\D8\B3\D8\AA\D8\A8\D8\AF\D8\A7\D9\84_\D8\A7\D9\84\D9\85\D9\83\D8\AF\D8\B3"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.258)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.262)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.266)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.270)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 1, ptr %alloca2, align 8
  %alloca3 = alloca i64, align 8
  store i64 100, ptr %alloca3, align 8
  br label %while_cond_33

while_cond_33:                                    ; preds = %merge_37, %entry
  %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1.load" = load i64, ptr %alloca, align 8
  %cmplttmp = icmp slt i64 %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1.load", 200
  br i1 %cmplttmp, label %while_body_34, label %while_exit_35

while_body_34:                                    ; preds = %while_cond_33
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load" = load i64, ptr %alloca1, align 8
  %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1.load4" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load", %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1.load4"
  store i64 %addtmp, ptr %alloca1, align 8
  %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1.load5" = load i64, ptr %alloca, align 8
  %"%\D9\86\D9\82\D8\B7\D8\A9_\D8\AA\D8\AD\D9\88\D9\84.load" = load i64, ptr %alloca3, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1.load5", %"%\D9\86\D9\82\D8\B7\D8\A9_\D8\AA\D8\AD\D9\88\D9\84.load"
  br i1 %cmpeqtmp, label %then_36, label %merge_37

while_exit_35:                                    ; preds = %while_cond_33
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D9\86\D9\82\D8\B7\D8\A9_\D8\AA\D8\AD\D9\88\D9\84.load6" = load i64, ptr %alloca3, align 8
  %subtmp = sub i64 200, %"%\D9\86\D9\82\D8\B7\D8\A9_\D8\AA\D8\AD\D9\88\D9\84.load6"
  %alloca7 = alloca i64, align 8
  store i64 %subtmp, ptr %alloca7, align 8
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.283)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0

then_36:                                          ; preds = %while_body_34
  %"%\D9\81\D9\8A_\D9\85\D9\81\D8\B3\D8\B1.load" = load i64, ptr %alloca2, align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D9\81\D9\8A_\D9\85\D9\81\D8\B3\D8\B1.load", 1
  br i1 %cmpeqtmp8, label %then_38, label %merge_39

merge_37:                                         ; preds = %merge_39, %while_body_34
  %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1.load9" = load i64, ptr %alloca, align 8
  %addtmp10 = add i64 %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1.load9", 1
  store i64 %addtmp10, ptr %alloca, align 8
  br label %while_cond_33

then_38:                                          ; preds = %then_36
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.291)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.295)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.299)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.303)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.307)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  store i64 0, ptr %alloca2, align 8
  br label %merge_39

merge_39:                                         ; preds = %then_38, %then_36
  br label %merge_37
}

define i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D8\B4\D8\A7\D9\85\D9\84"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.311)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.315)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.319)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.323)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.327)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.331)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.335)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.339)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.343)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.347)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.351)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.355)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.359)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.363)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.367)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.371)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.375)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.379)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.383)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.387)
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %40 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.391)
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %42 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.395)
  %43 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %44 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.399)
  %45 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %46 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.403)
  %47 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.407)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.411)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.415)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.419)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\AE\D8\A7\D8\AF\D9\85_\D9\88\D9\8A\D8\A8"()
  %calltmp1 = call i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\A7\D8\B3\D8\AA\D8\A8\D8\AF\D8\A7\D9\84_\D8\A7\D9\84\D9\85\D9\83\D8\AF\D8\B3"()
  %calltmp2 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D8\B4\D8\A7\D9\85\D9\84"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.423)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.427)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
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
