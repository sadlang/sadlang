; ModuleID = '04_medium_code_optimizer'
source_filename = "04_medium_code_optimizer"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [9 x i8] c"  [OPT] \00", align 1
@str.const.1 = private unnamed_addr constant [3 x i8] c": \00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.2 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.4 = private unnamed_addr constant [17 x i8] c"  [DEL] \D8\AD\D8\B0\D9\81: \00", align 1
@str.const.8 = private unnamed_addr constant [9 x i8] c"  [REP] \00", align 1
@str.const.9 = private unnamed_addr constant [6 x i8] c" \E2\86\92 \00", align 1
@str.const.13 = private unnamed_addr constant [42 x i8] c"=== \D8\AA\D8\AD\D8\B3\D9\8A\D9\86 1: \D8\B7\D9\8A \D8\A7\D9\84\D8\AB\D9\88\D8\A7\D8\A8\D8\AA ===\00", align 1
@str.const.17 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.21 = private unnamed_addr constant [27 x i8] c"  \D8\A7\D9\84\D9\83\D9\88\D8\AF \D8\A7\D9\84\D8\A3\D8\B5\D9\84\D9\8A:\00", align 1
@str.const.25 = private unnamed_addr constant [14 x i8] c"    x = 3 + 4\00", align 1
@str.const.29 = private unnamed_addr constant [14 x i8] c"    y = x * 2\00", align 1
@str.const.33 = private unnamed_addr constant [14 x i8] c"    z = y + 1\00", align 1
@str.const.37 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.41 = private unnamed_addr constant [5 x i8] c"\D8\B7\D9\8A\00", align 1
@str.const.42 = private unnamed_addr constant [12 x i8] c"3 + 4 \E2\86\92 7\00", align 1
@str.const.43 = private unnamed_addr constant [7 x i8] c"\D9\86\D8\B4\D8\B1\00", align 1
@str.const.44 = private unnamed_addr constant [32 x i8] c"x = 7\D8\8C \D9\84\D8\B0\D8\A7 y = 7 * 2 \E2\86\92 14\00", align 1
@str.const.46 = private unnamed_addr constant [34 x i8] c"y = 14\D8\8C \D9\84\D8\B0\D8\A7 z = 14 + 1 \E2\86\92 15\00", align 1
@str.const.47 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.51 = private unnamed_addr constant [29 x i8] c"  \D8\A7\D9\84\D9\83\D9\88\D8\AF \D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\91\D9\86:\00", align 1
@str.const.55 = private unnamed_addr constant [10 x i8] c"    x = 7\00", align 1
@str.const.59 = private unnamed_addr constant [11 x i8] c"    y = 14\00", align 1
@str.const.63 = private unnamed_addr constant [11 x i8] c"    z = 15\00", align 1
@str.const.67 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.71 = private unnamed_addr constant [36 x i8] c"  \D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D8\A3\D8\B5\D9\84\D9\8A\D8\A9: \00", align 1
@str.const.74 = private unnamed_addr constant [38 x i8] c"  \D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\91\D9\86\D8\A9: \00", align 1
@str.const.77 = private unnamed_addr constant [19 x i8] c"  \D8\A7\D9\84\D8\AA\D9\88\D9\81\D9\8A\D8\B1: \00", align 1
@str.const.78 = private unnamed_addr constant [14 x i8] c" \D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.81 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.85 = private unnamed_addr constant [51 x i8] c"=== \D8\AA\D8\AD\D8\B3\D9\8A\D9\86 2: \D8\AD\D8\B0\D9\81 \D8\A7\D9\84\D9\83\D9\88\D8\AF \D8\A7\D9\84\D9\85\D9\8A\D8\AA ===\00", align 1
@str.const.89 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.97 = private unnamed_addr constant [39 x i8] c"    a = 10              # \D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\00", align 1
@str.const.101 = private unnamed_addr constant [47 x i8] c"    b = 20              # \D8\BA\D9\8A\D8\B1 \D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85!\00", align 1
@str.const.105 = private unnamed_addr constant [39 x i8] c"    c = a + 5           # \D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85\00", align 1
@str.const.109 = private unnamed_addr constant [52 x i8] c"    d = b * 3           # \D9\8A\D8\B9\D8\AA\D9\85\D8\AF \D8\B9\D9\84\D9\89 \D9\85\D9\8A\D8\AA!\00", align 1
@str.const.113 = private unnamed_addr constant [16 x i8] c"    \D8\A7\D8\B7\D8\A8\D8\B9(c)\00", align 1
@str.const.117 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.121 = private unnamed_addr constant [49 x i8] c"b = 20 (\D8\BA\D9\8A\D8\B1 \D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85 \D9\81\D9\8A \D8\A7\D9\84\D8\A5\D8\AE\D8\B1\D8\A7\D8\AC)\00", align 1
@str.const.122 = private unnamed_addr constant [48 x i8] c"d = b * 3 (\D9\8A\D8\B9\D8\AA\D9\85\D8\AF \D8\B9\D9\84\D9\89 \D9\85\D8\AA\D8\BA\D9\8A\D8\B1 \D9\85\D9\8A\D8\AA)\00", align 1
@str.const.123 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.131 = private unnamed_addr constant [11 x i8] c"    a = 10\00", align 1
@str.const.135 = private unnamed_addr constant [14 x i8] c"    c = a + 5\00", align 1
@str.const.143 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.147 = private unnamed_addr constant [17 x i8] c"  \D8\A7\D9\84\D8\A3\D8\B3\D8\B7\D8\B1: \00", align 1
@str.const.151 = private unnamed_addr constant [15 x i8] c"  \D9\85\D8\AD\D8\B0\D9\88\D9\81: \00", align 1
@str.const.152 = private unnamed_addr constant [10 x i8] c" \D8\A3\D8\B3\D8\B7\D8\B1\00", align 1
@str.const.155 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.159 = private unnamed_addr constant [46 x i8] c"=== \D8\AA\D8\AD\D8\B3\D9\8A\D9\86 3: \D8\AA\D8\B6\D9\85\D9\8A\D9\86 \D8\A7\D9\84\D8\AF\D9\88\D8\A7\D9\84 ===\00", align 1
@str.const.163 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.171 = private unnamed_addr constant [44 x i8] c"    \D8\AF\D8\A7\D9\84\D8\A9 \D9\85\D8\B1\D8\A8\D8\B9(x) { \D8\A7\D8\B1\D8\AC\D8\B9 x * x }\00", align 1
@str.const.175 = private unnamed_addr constant [48 x i8] c"    \D8\AF\D8\A7\D9\84\D8\A9 \D9\85\D9\83\D8\B9\D8\A8(x) { \D8\A7\D8\B1\D8\AC\D8\B9 x * x * x }\00", align 1
@str.const.179 = private unnamed_addr constant [20 x i8] c"    a = \D9\85\D8\B1\D8\A8\D8\B9(5)\00", align 1
@str.const.183 = private unnamed_addr constant [20 x i8] c"    b = \D9\85\D9\83\D8\B9\D8\A8(3)\00", align 1
@str.const.187 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.191 = private unnamed_addr constant [11 x i8] c"\D8\AA\D8\B6\D9\85\D9\8A\D9\86\00", align 1
@str.const.192 = private unnamed_addr constant [22 x i8] c"\D9\85\D8\B1\D8\A8\D8\B9(5) \E2\86\92 5 * 5\00", align 1
@str.const.194 = private unnamed_addr constant [13 x i8] c"5 * 5 \E2\86\92 25\00", align 1
@str.const.196 = private unnamed_addr constant [26 x i8] c"\D9\85\D9\83\D8\B9\D8\A8(3) \E2\86\92 3 * 3 * 3\00", align 1
@str.const.198 = private unnamed_addr constant [17 x i8] c"3 * 3 * 3 \E2\86\92 27\00", align 1
@str.const.199 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.207 = private unnamed_addr constant [11 x i8] c"    a = 25\00", align 1
@str.const.211 = private unnamed_addr constant [11 x i8] c"    b = 27\00", align 1
@str.const.215 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.219 = private unnamed_addr constant [63 x i8] c"  \D8\AA\D9\88\D9\81\D9\8A\D8\B1: \D8\A7\D8\B3\D8\AA\D8\AF\D8\B9\D8\A7\D8\A1\D9\8A\D9\86 + 3 \D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA \D8\AD\D8\B3\D8\A7\D8\A8\D9\8A\D8\A9\00", align 1
@str.const.223 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.227 = private unnamed_addr constant [42 x i8] c"=== \D8\AA\D8\AD\D8\B3\D9\8A\D9\86 4: \D9\81\D9\83 \D8\A7\D9\84\D8\AD\D9\84\D9\82\D8\A7\D8\AA ===\00", align 1
@str.const.231 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.239 = private unnamed_addr constant [19 x i8] c"    \D9\85\D8\AC\D9\85\D9\88\D8\B9 = 0\00", align 1
@str.const.243 = private unnamed_addr constant [30 x i8] c"    \D9\84\D9\83\D9\84 i \D9\85\D9\86 0 \D8\A5\D9\84\D9\89 3:\00", align 1
@str.const.247 = private unnamed_addr constant [36 x i8] c"      \D9\85\D8\AC\D9\85\D9\88\D8\B9 += \D9\85\D8\B5\D9\81\D9\88\D9\81\D8\A9[i]\00", align 1
@str.const.251 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.255 = private unnamed_addr constant [5 x i8] c"\D9\81\D9\83\00", align 1
@str.const.256 = private unnamed_addr constant [56 x i8] c"\D8\A7\D9\84\D8\AD\D9\84\D9\82\D8\A9 \D9\82\D8\B5\D9\8A\D8\B1\D8\A9 (4 \D8\AA\D9\83\D8\B1\D8\A7\D8\B1\D8\A7\D8\AA) - \D9\81\D9\83\D9\91\D9\87\D8\A7\00", align 1
@str.const.257 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.265 = private unnamed_addr constant [33 x i8] c"    \D9\85\D8\AC\D9\85\D9\88\D8\B9 = \D9\85\D8\B5\D9\81\D9\88\D9\81\D8\A9[0]\00", align 1
@str.const.269 = private unnamed_addr constant [34 x i8] c"    \D9\85\D8\AC\D9\85\D9\88\D8\B9 += \D9\85\D8\B5\D9\81\D9\88\D9\81\D8\A9[1]\00", align 1
@str.const.273 = private unnamed_addr constant [34 x i8] c"    \D9\85\D8\AC\D9\85\D9\88\D8\B9 += \D9\85\D8\B5\D9\81\D9\88\D9\81\D8\A9[2]\00", align 1
@str.const.277 = private unnamed_addr constant [34 x i8] c"    \D9\85\D8\AC\D9\85\D9\88\D8\B9 += \D9\85\D8\B5\D9\81\D9\88\D9\81\D8\A9[3]\00", align 1
@str.const.281 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.285 = private unnamed_addr constant [18 x i8] c"  \D8\A7\D9\84\D9\81\D9\88\D8\A7\D8\A6\D8\AF:\00", align 1
@str.const.289 = private unnamed_addr constant [35 x i8] c"    - \D8\A5\D8\B2\D8\A7\D9\84\D8\A9 \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\B4\D8\B1\D8\B7\00", align 1
@str.const.293 = private unnamed_addr constant [41 x i8] c"    - \D8\A5\D8\B2\D8\A7\D9\84\D8\A9 \D8\B2\D9\8A\D8\A7\D8\AF\D8\A9 \D8\A7\D9\84\D8\B9\D8\AF\D8\A7\D8\AF\00", align 1
@str.const.297 = private unnamed_addr constant [45 x i8] c"    - \D8\A5\D9\85\D9\83\D8\A7\D9\86\D9\8A\D8\A9 \D8\AA\D9\86\D9\81\D9\8A\D8\B0 \D9\85\D8\AA\D9\88\D8\A7\D8\B2\D9\8A\00", align 1
@str.const.301 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.305 = private unnamed_addr constant [70 x i8] c"=== \D8\AA\D8\AD\D8\B3\D9\8A\D9\86 5: \D8\AA\D8\AD\D8\B1\D9\8A\D9\83 \D8\A7\D9\84\D8\AB\D9\88\D8\A7\D8\A8\D8\AA \D8\AE\D8\A7\D8\B1\D8\AC \D8\A7\D9\84\D8\AD\D9\84\D9\82\D8\A9 ===\00", align 1
@str.const.309 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.317 = private unnamed_addr constant [33 x i8] c"    \D9\84\D9\83\D9\84 i \D9\85\D9\86 0 \D8\A5\D9\84\D9\89 1000:\00", align 1
@str.const.321 = private unnamed_addr constant [51 x i8] c"      x = \D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\85\D8\B5\D9\81\D9\88\D9\81\D8\A9 * 4  # \D8\AB\D8\A7\D8\A8\D8\AA!\00", align 1
@str.const.325 = private unnamed_addr constant [42 x i8] c"      \D9\86\D8\AA\D9\8A\D8\AC\D8\A9[i] = \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA[i] + x\00", align 1
@str.const.329 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.333 = private unnamed_addr constant [11 x i8] c"\D8\AA\D8\AD\D8\B1\D9\8A\D9\83\00", align 1
@str.const.334 = private unnamed_addr constant [67 x i8] c"x = \D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\85\D8\B5\D9\81\D9\88\D9\81\D8\A9 * 4 \D8\AB\D8\A7\D8\A8\D8\AA \E2\86\90 \D8\AE\D8\A7\D8\B1\D8\AC \D8\A7\D9\84\D8\AD\D9\84\D9\82\D8\A9\00", align 1
@str.const.335 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.343 = private unnamed_addr constant [59 x i8] c"    x = \D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\85\D8\B5\D9\81\D9\88\D9\81\D8\A9 * 4    # \D9\85\D8\B1\D8\A9 \D9\88\D8\A7\D8\AD\D8\AF\D8\A9\00", align 1
@str.const.355 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.359 = private unnamed_addr constant [60 x i8] c"  \D8\A7\D9\84\D8\AA\D9\88\D9\81\D9\8A\D8\B1: 999 \D8\B9\D9\85\D9\84\D9\8A\D8\A9 \D8\B6\D8\B1\D8\A8 \D8\BA\D9\8A\D8\B1 \D8\B6\D8\B1\D9\88\D8\B1\D9\8A\D8\A9\00", align 1
@str.const.363 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.367 = private unnamed_addr constant [38 x i8] c"=== \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D8\AA\D8\AD\D8\B3\D9\8A\D9\86\D8\A7\D8\AA ===\00", align 1
@str.const.371 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.375 = private unnamed_addr constant [120 x i8] c"  \E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.379 = private unnamed_addr constant [63 x i8] c"  \E2\95\91 \D8\A7\D9\84\D8\AA\D8\AD\D8\B3\D9\8A\D9\86             \E2\94\82 \D8\A7\D9\84\D8\AA\D9\88\D9\81\D9\8A\D8\B1        \E2\95\91\00", align 1
@str.const.383 = private unnamed_addr constant [120 x i8] c"  \E2\95\A0\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\A3\00", align 1
@str.const.387 = private unnamed_addr constant [64 x i8] c"  \E2\95\91 \D8\B7\D9\8A \D8\A7\D9\84\D8\AB\D9\88\D8\A7\D8\A8\D8\AA          \E2\94\82 3 \D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA       \E2\95\91\00", align 1
@str.const.391 = private unnamed_addr constant [65 x i8] c"  \E2\95\91 \D8\AD\D8\B0\D9\81 \D8\A7\D9\84\D9\83\D9\88\D8\AF \D8\A7\D9\84\D9\85\D9\8A\D8\AA     \E2\94\82 2 \D8\A3\D8\B3\D8\B7\D8\B1        \E2\95\91\00", align 1
@str.const.395 = private unnamed_addr constant [67 x i8] c"  \E2\95\91 \D8\AA\D8\B6\D9\85\D9\8A\D9\86 \D8\A7\D9\84\D8\AF\D9\88\D8\A7\D9\84        \E2\94\82 2 \D8\A7\D8\B3\D8\AA\D8\AF\D8\B9\D8\A7\D8\A1      \E2\95\91\00", align 1
@str.const.399 = private unnamed_addr constant [69 x i8] c"  \E2\95\91 \D9\81\D9\83 \D8\A7\D9\84\D8\AD\D9\84\D9\82\D8\A7\D8\AA          \E2\94\82 \D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA \D8\A7\D9\84\D8\B4\D8\B1\D8\B7   \E2\95\91\00", align 1
@str.const.403 = private unnamed_addr constant [66 x i8] c"  \E2\95\91 \D8\AA\D8\AD\D8\B1\D9\8A\D9\83 \D8\A7\D9\84\D8\AB\D9\88\D8\A7\D8\A8\D8\AA       \E2\94\82 999 \D8\B9\D9\85\D9\84\D9\8A\D8\A9      \E2\95\91\00", align 1
@str.const.407 = private unnamed_addr constant [120 x i8] c"  \E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.411 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.415 = private unnamed_addr constant [70 x i8] c"  \D9\85\D9\84\D8\A7\D8\AD\D8\B8\D8\A9: \D9\87\D8\B0\D9\87 \D8\A7\D9\84\D8\AA\D8\AD\D8\B3\D9\8A\D9\86\D8\A7\D8\AA \D8\AA\D9\8F\D8\B7\D8\A8\D9\82 \D8\AA\D9\84\D9\82\D8\A7\D8\A6\D9\8A\D8\A7\D9\8B\00", align 1
@str.const.419 = private unnamed_addr constant [60 x i8] c"  \D8\A3\D8\AB\D9\86\D8\A7\D8\A1 \D8\AA\D8\B1\D8\AC\D9\85\D8\A9 JIT \D9\84\D9\84\D9\85\D8\B3\D8\A7\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D8\B3\D8\A7\D8\AE\D9\86\D8\A9\00", align 1
@str.const.423 = private unnamed_addr constant [121 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.427 = private unnamed_addr constant [53 x i8] c"\E2\95\91  \D9\85\D8\AD\D8\B3\D9\91\D9\86 \D9\83\D9\88\D8\AF JIT                       \E2\95\91\00", align 1
@str.const.431 = private unnamed_addr constant [121 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.435 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.439 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.443 = private unnamed_addr constant [34 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 \D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\91\D9\86 ===\00", align 1

define i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B3\D9\8A\D9\86"(ptr %"\D8\AA\D8\AD\D8\B3\D9\8A\D9\86", ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84") {
entry:
  %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\842" = alloca ptr, align 8
  store ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\84", ptr %"\D8\AA\D9\81\D8\A7\D8\B5\D9\8A\D9\842", align 8
  %"\D8\AA\D8\AD\D8\B3\D9\8A\D9\861" = alloca ptr, align 8
  store ptr %"\D8\AA\D8\AD\D8\B3\D9\8A\D9\86", ptr %"\D8\AA\D8\AD\D8\B3\D9\8A\D9\861", align 8
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %"\D8\AA\D8\AD\D8\B3\D9\8A\D9\861")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\AA\D8\AD\D8\B3\D9\8A\D9\861", i64 %copylen2)
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

define i64 @"\D8\B3\D8\AC_\D8\AD\D8\B0\D9\81"(ptr %"\D8\B9\D8\A8\D8\A7\D8\B1\D8\A9") {
entry:
  %"\D8\B9\D8\A8\D8\A7\D8\B1\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\B9\D8\A8\D8\A7\D8\B1\D8\A9", ptr %"\D8\B9\D8\A8\D8\A7\D8\B1\D8\A91", align 8
  %len1 = call i64 @strlen(ptr @str.const.4)
  %len2 = call i64 @strlen(ptr %"\D8\B9\D8\A8\D8\A7\D8\B1\D8\A91")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.4, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\B9\D8\A8\D8\A7\D8\B1\D8\A91", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B3\D8\AC_\D8\A7\D8\B3\D8\AA\D8\A8\D8\AF\D8\A7\D9\84"(ptr %"\D8\A3\D8\B5\D9\84\D9\8A", ptr %"\D8\A8\D8\AF\D9\8A\D9\84") {
entry:
  %"\D8\A8\D8\AF\D9\8A\D9\842" = alloca ptr, align 8
  store ptr %"\D8\A8\D8\AF\D9\8A\D9\84", ptr %"\D8\A8\D8\AF\D9\8A\D9\842", align 8
  %"\D8\A3\D8\B5\D9\84\D9\8A1" = alloca ptr, align 8
  store ptr %"\D8\A3\D8\B5\D9\84\D9\8A", ptr %"\D8\A3\D8\B5\D9\84\D9\8A1", align 8
  %len1 = call i64 @strlen(ptr @str.const.8)
  %len2 = call i64 @strlen(ptr %"\D8\A3\D8\B5\D9\84\D9\8A1")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.8, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A3\D8\B5\D9\84\D9\8A1", i64 %copylen2)
  %len13 = call i64 @strlen(ptr %concat.buf)
  %len24 = call i64 @strlen(ptr @str.const.9)
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %2 = call ptr @memcpy(ptr %concat.buf7, ptr %concat.buf, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %3 = call ptr @memcpy(ptr %dest28, ptr @str.const.9, i64 %copylen29)
  %len110 = call i64 @strlen(ptr %concat.buf7)
  %len211 = call i64 @strlen(ptr %"\D8\A8\D8\AF\D9\8A\D9\842")
  %totallen12 = add i64 %len110, %len211
  %bufsize13 = add i64 %totallen12, 1
  %concat.buf14 = alloca i8, i64 %bufsize13, align 1
  %4 = call ptr @memcpy(ptr %concat.buf14, ptr %concat.buf7, i64 %len110)
  %dest215 = getelementptr i8, ptr %concat.buf14, i64 %len110
  %copylen216 = add i64 %len211, 1
  %5 = call ptr @memcpy(ptr %dest215, ptr %"\D8\A8\D8\AF\D9\8A\D9\842", i64 %copylen216)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf14)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\AA\D8\AD\D8\B3\D9\8A\D9\86_\D8\B7\D9\8A_\D8\AB\D9\88\D8\A7\D8\A8\D8\AA"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.13)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.17)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.21)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.25)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.29)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.33)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.37)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 3, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %calltmp = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B3\D9\8A\D9\86"(ptr @str.const.41, ptr @str.const.42)
  %calltmp2 = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B3\D9\8A\D9\86"(ptr @str.const.43, ptr @str.const.44)
  %calltmp3 = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B3\D9\8A\D9\86"(ptr @str.const.43, ptr @str.const.46)
  store i64 0, ptr %alloca1, align 8
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.47)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.51)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.55)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.59)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.63)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.67)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\A3\D8\B5\D9\84\D9\8A\D8\A9.load" = load i64, ptr %alloca, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\A8\D8\B9\D8\AF.load" = load i64, ptr %alloca1, align 8
  %subtmp = sub i64 %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\A3\D8\B5\D9\84\D9\8A\D8\A9.load", %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\A8\D8\B9\D8\AF.load"
  %alloca4 = alloca i64, align 8
  store i64 %subtmp, ptr %alloca4, align 8
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.81)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\AA\D8\AD\D8\B3\D9\8A\D9\86_\D9\83\D9\88\D8\AF_\D9\85\D9\8A\D8\AA"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.85)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.89)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.21)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.97)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.101)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.105)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.109)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.113)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.117)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 5, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %calltmp = call i64 @"\D8\B3\D8\AC_\D8\AD\D8\B0\D9\81"(ptr @str.const.121)
  %"%\D8\A3\D8\B3\D8\B7\D8\B1_\D9\85\D8\AD\D8\B0\D9\88\D9\81\D8\A9.load" = load i64, ptr %alloca1, align 8
  %addtmp = add i64 %"%\D8\A3\D8\B3\D8\B7\D8\B1_\D9\85\D8\AD\D8\B0\D9\88\D9\81\D8\A9.load", 1
  store i64 %addtmp, ptr %alloca1, align 8
  %calltmp2 = call i64 @"\D8\B3\D8\AC_\D8\AD\D8\B0\D9\81"(ptr @str.const.122)
  %"%\D8\A3\D8\B3\D8\B7\D8\B1_\D9\85\D8\AD\D8\B0\D9\88\D9\81\D8\A9.load3" = load i64, ptr %alloca1, align 8
  %addtmp4 = add i64 %"%\D8\A3\D8\B3\D8\B7\D8\B1_\D9\85\D8\AD\D8\B0\D9\88\D9\81\D8\A9.load3", 1
  store i64 %addtmp4, ptr %alloca1, align 8
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.123)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.51)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.131)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.135)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.113)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.143)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\A3\D8\B3\D8\B7\D8\B1_\D8\A3\D8\B5\D9\84\D9\8A\D8\A9.load" = load i64, ptr %alloca, align 8
  %"%\D8\A3\D8\B3\D8\B7\D8\B1_\D9\85\D8\AD\D8\B0\D9\88\D9\81\D8\A9.load5" = load i64, ptr %alloca1, align 8
  %subtmp = sub i64 %"%\D8\A3\D8\B3\D8\B7\D8\B1_\D8\A3\D8\B5\D9\84\D9\8A\D8\A9.load", %"%\D8\A3\D8\B3\D8\B7\D8\B1_\D9\85\D8\AD\D8\B0\D9\88\D9\81\D8\A9.load5"
  %alloca6 = alloca i64, align 8
  store i64 %subtmp, ptr %alloca6, align 8
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.155)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\AA\D8\AD\D8\B3\D9\8A\D9\86_\D8\AA\D8\B6\D9\85\D9\8A\D9\86"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.159)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.163)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.21)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.171)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.175)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.179)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.183)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.187)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B3\D9\8A\D9\86"(ptr @str.const.191, ptr @str.const.192)
  %calltmp1 = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B3\D9\8A\D9\86"(ptr @str.const.41, ptr @str.const.194)
  %calltmp2 = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B3\D9\8A\D9\86"(ptr @str.const.191, ptr @str.const.196)
  %calltmp3 = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B3\D9\8A\D9\86"(ptr @str.const.41, ptr @str.const.198)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.199)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.51)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.207)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.211)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.215)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.219)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.223)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\AA\D8\AD\D8\B3\D9\8A\D9\86_\D9\81\D9\83_\D8\AD\D9\84\D9\82\D8\A7\D8\AA"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.227)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.231)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.21)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.239)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.243)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.247)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.251)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B3\D9\8A\D9\86"(ptr @str.const.255, ptr @str.const.256)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.257)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.51)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.265)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.269)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.273)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.277)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.281)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.285)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.289)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.293)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.297)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.301)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\AA\D8\AD\D8\B3\D9\8A\D9\86_\D8\AA\D8\AD\D8\B1\D9\8A\D9\83"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.305)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.309)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.21)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.317)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.321)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.325)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.329)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D8\B3\D8\AC_\D8\AA\D8\AD\D8\B3\D9\8A\D9\86"(ptr @str.const.333, ptr @str.const.334)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.335)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.51)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.343)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.317)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.325)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.355)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.359)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.363)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D8\AA\D8\AD\D8\B3\D9\8A\D9\86\D8\A7\D8\AA"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.367)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.371)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.375)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.379)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.383)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.387)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.391)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.395)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.399)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.403)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.407)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.411)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.415)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.419)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.423)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.427)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.431)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.435)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D8\AA\D8\AD\D8\B3\D9\8A\D9\86_\D8\B7\D9\8A_\D8\AB\D9\88\D8\A7\D8\A8\D8\AA"()
  %calltmp1 = call i64 @"\D8\AA\D8\AD\D8\B3\D9\8A\D9\86_\D9\83\D9\88\D8\AF_\D9\85\D9\8A\D8\AA"()
  %calltmp2 = call i64 @"\D8\AA\D8\AD\D8\B3\D9\8A\D9\86_\D8\AA\D8\B6\D9\85\D9\8A\D9\86"()
  %calltmp3 = call i64 @"\D8\AA\D8\AD\D8\B3\D9\8A\D9\86_\D9\81\D9\83_\D8\AD\D9\84\D9\82\D8\A7\D8\AA"()
  %calltmp4 = call i64 @"\D8\AA\D8\AD\D8\B3\D9\8A\D9\86_\D8\AA\D8\AD\D8\B1\D9\8A\D9\83"()
  %calltmp5 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D8\AA\D8\AD\D8\B3\D9\8A\D9\86\D8\A7\D8\AA"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.439)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.443)
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
