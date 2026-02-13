; ModuleID = '06_complex_multi_target'
source_filename = "06_complex_multi_target"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [4 x i8] c"  [\00", align 1
@str.const.1 = private unnamed_addr constant [3 x i8] c"] \00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.2 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.ret = private unnamed_addr constant [13 x i8] c"x86_64-linux\00", align 1
@str.ret.4 = private unnamed_addr constant [14 x i8] c"aarch64-linux\00", align 1
@str.ret.5 = private unnamed_addr constant [7 x i8] c"wasm32\00", align 1
@str.ret.6 = private unnamed_addr constant [8 x i8] c"riscv64\00", align 1
@str.ret.7 = private unnamed_addr constant [8 x i8] c"unknown\00", align 1
@str.const.8 = private unnamed_addr constant [66 x i8] c"=== \D8\A7\D9\84\D8\AC\D8\B2\D8\A1 1: \D8\AA\D8\AD\D9\84\D9\8A\D9\84 \D8\A7\D9\84\D8\AA\D9\85\D8\AB\D9\8A\D9\84 \D8\A7\D9\84\D9\88\D8\B3\D9\8A\D8\B7 (IR) ===\00", align 1
@str.const.12 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.16 = private unnamed_addr constant [33 x i8] c"  \D8\A7\D9\84\D8\A8\D8\B1\D9\86\D8\A7\D9\85\D8\AC \D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1:\00", align 1
@str.const.20 = private unnamed_addr constant [55 x i8] c"    \D8\AF\D8\A7\D9\84\D8\A9 \D8\AD\D8\B3\D8\A7\D8\A8(\D8\B1\D9\82\D9\85 \D8\A3\D8\8C \D8\B1\D9\82\D9\85 \D8\A8) -> \D8\B1\D9\82\D9\85\00", align 1
@str.const.24 = private unnamed_addr constant [40 x i8] c"      \D8\B1\D9\82\D9\85 \D9\86\D8\AA\D9\8A\D8\AC\D8\A9 = (\D8\A3 + \D8\A8) * 2\00", align 1
@str.const.28 = private unnamed_addr constant [26 x i8] c"      \D8\A7\D8\B1\D8\AC\D8\B9 \D9\86\D8\AA\D9\8A\D8\AC\D8\A9\00", align 1
@str.const.32 = private unnamed_addr constant [15 x i8] c"    \D9\86\D9\87\D8\A7\D9\8A\D8\A9\00", align 1
@str.const.36 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.40 = private unnamed_addr constant [36 x i8] c"  \D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A7\D8\AA IR \D8\A7\D9\84\D9\85\D9\88\D9\84\D8\AF\D8\A9:\00", align 1
@str.const.44 = private unnamed_addr constant [24 x i8] c"    IR.00: PARAM r0, \D8\A3\00", align 1
@str.const.48 = private unnamed_addr constant [24 x i8] c"    IR.01: PARAM r1, \D8\A8\00", align 1
@str.const.52 = private unnamed_addr constant [28 x i8] c"    IR.02: ADD   r2, r0, r1\00", align 1
@str.const.56 = private unnamed_addr constant [23 x i8] c"    IR.03: CONST r3, 2\00", align 1
@str.const.60 = private unnamed_addr constant [28 x i8] c"    IR.04: MUL   r4, r2, r3\00", align 1
@str.const.64 = private unnamed_addr constant [20 x i8] c"    IR.05: RET   r4\00", align 1
@str.const.68 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.72 = private unnamed_addr constant [29 x i8] c"  \D8\B9\D8\AF\D8\AF \D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A7\D8\AA IR: \00", align 1
@str.const.75 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.79 = private unnamed_addr constant [34 x i8] c"  \E2\94\80\E2\94\80 \D8\AA\D9\88\D9\84\D9\8A\D8\AF \D9\83\D9\88\D8\AF \D9\84\D9\80: \00", align 1
@str.const.80 = private unnamed_addr constant [8 x i8] c" \E2\94\80\E2\94\80\00", align 1
@str.const.84 = private unnamed_addr constant [32 x i8] c"    \D9\85\D9\82\D8\A7\D8\B3 \D8\A7\D9\84\D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A9: \00", align 1
@str.const.85 = private unnamed_addr constant [10 x i8] c" \D8\A8\D8\A7\D9\8A\D8\AA\00", align 1
@str.const.88 = private unnamed_addr constant [30 x i8] c"    \D8\B9\D8\AF\D8\AF \D8\A7\D9\84\D9\85\D8\B3\D8\AC\D9\84\D8\A7\D8\AA: \00", align 1
@str.const.91 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.95 = private unnamed_addr constant [38 x i8] c"    \D8\A7\D9\84\D9\83\D9\88\D8\AF \D8\A7\D9\84\D9\85\D9\88\D9\84\D8\AF (x86_64):\00", align 1
@str.const.99 = private unnamed_addr constant [32 x i8] c"      mov  rax, rdi        ; \D8\A3\00", align 1
@str.const.103 = private unnamed_addr constant [37 x i8] c"      add  rax, rsi        ; \D8\A3 + \D8\A8\00", align 1
@str.const.107 = private unnamed_addr constant [33 x i8] c"      shl  rax, 1          ; * 2\00", align 1
@str.const.111 = private unnamed_addr constant [10 x i8] c"      ret\00", align 1
@str.const.115 = private unnamed_addr constant [26 x i8] c"    \D9\85\D9\82\D8\A7\D8\B3 \D8\A7\D9\84\D9\83\D9\88\D8\AF: \00", align 1
@str.const.119 = private unnamed_addr constant [39 x i8] c"    \D8\A7\D9\84\D9\83\D9\88\D8\AF \D8\A7\D9\84\D9\85\D9\88\D9\84\D8\AF (AArch64):\00", align 1
@str.const.123 = private unnamed_addr constant [37 x i8] c"      add  x2, x0, x1      ; \D8\A3 + \D8\A8\00", align 1
@str.const.127 = private unnamed_addr constant [33 x i8] c"      lsl  x0, x2, #1      ; * 2\00", align 1
@str.const.139 = private unnamed_addr constant [36 x i8] c"    \D8\A7\D9\84\D9\83\D9\88\D8\AF \D8\A7\D9\84\D9\85\D9\88\D9\84\D8\AF (WASM):\00", align 1
@str.const.143 = private unnamed_addr constant [33 x i8] c"      local.get 0           ; \D8\A3\00", align 1
@str.const.147 = private unnamed_addr constant [33 x i8] c"      local.get 1           ; \D8\A8\00", align 1
@str.const.151 = private unnamed_addr constant [32 x i8] c"      i64.add               ; +\00", align 1
@str.const.155 = private unnamed_addr constant [18 x i8] c"      i64.const 2\00", align 1
@str.const.159 = private unnamed_addr constant [34 x i8] c"      i64.mul               ; * 2\00", align 1
@str.const.167 = private unnamed_addr constant [38 x i8] c"    \D8\A7\D9\84\D9\83\D9\88\D8\AF \D8\A7\D9\84\D9\85\D9\88\D9\84\D8\AF (RISC-V):\00", align 1
@str.const.171 = private unnamed_addr constant [38 x i8] c"      add  a2, a0, a1       ; \D8\A3 + \D8\A8\00", align 1
@str.const.175 = private unnamed_addr constant [34 x i8] c"      slli a0, a2, 1        ; * 2\00", align 1
@str.const.187 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.191 = private unnamed_addr constant [66 x i8] c"=== \D8\A7\D9\84\D8\AC\D8\B2\D8\A1 2: \D8\AA\D9\88\D9\84\D9\8A\D8\AF \D8\A7\D9\84\D9\83\D9\88\D8\AF \D9\84\D9\83\D9\84 \D8\A7\D9\84\D9\85\D9\86\D8\B5\D8\A7\D8\AA ===\00", align 1
@str.const.195 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.199 = private unnamed_addr constant [50 x i8] c"=== \D8\A7\D9\84\D8\AC\D8\B2\D8\A1 3: \D8\AA\D8\AE\D8\B5\D9\8A\D8\B5 \D8\A7\D9\84\D9\85\D8\B3\D8\AC\D9\84\D8\A7\D8\AA ===\00", align 1
@str.const.203 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.207 = private unnamed_addr constant [34 x i8] c"  \D9\85\D8\AA\D8\BA\D9\8A\D8\B1\D8\A7\D8\AA IR \D8\A7\D9\84\D9\86\D8\B4\D8\B7\D8\A9:\00", align 1
@str.const.211 = private unnamed_addr constant [57 x i8] c"    r0 (\D8\A3):     [0-2]  \E2\86\90 \D8\AD\D9\8A \D9\81\D9\8A \D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A7\D8\AA 0-2\00", align 1
@str.const.215 = private unnamed_addr constant [57 x i8] c"    r1 (\D8\A8):     [1-2]  \E2\86\90 \D8\AD\D9\8A \D9\81\D9\8A \D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A7\D8\AA 1-2\00", align 1
@str.const.219 = private unnamed_addr constant [61 x i8] c"    r2 (\D9\85\D8\AC\D9\85\D9\88\D8\B9): [2-4]  \E2\86\90 \D8\AD\D9\8A \D9\81\D9\8A \D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A7\D8\AA 2-4\00", align 1
@str.const.223 = private unnamed_addr constant [60 x i8] c"    r3 (\D8\AB\D8\A7\D8\A8\D8\AA):  [3-4]  \E2\86\90 \D8\AD\D9\8A \D9\81\D9\8A \D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A7\D8\AA 3-4\00", align 1
@str.const.227 = private unnamed_addr constant [61 x i8] c"    r4 (\D9\86\D8\AA\D9\8A\D8\AC\D8\A9): [4-5]  \E2\86\90 \D8\AD\D9\8A \D9\81\D9\8A \D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A7\D8\AA 4-5\00", align 1
@str.const.231 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.235 = private unnamed_addr constant [29 x i8] c"  \D8\AA\D8\AD\D9\84\D9\8A\D9\84 \D8\A7\D9\84\D8\AA\D8\AF\D8\A7\D8\AE\D9\84:\00", align 1
@str.const.239 = private unnamed_addr constant [37 x i8] c"    \D8\A3\D9\82\D8\B5\D9\89 \D9\85\D8\AA\D8\BA\D9\8A\D8\B1\D8\A7\D8\AA \D8\AD\D9\8A\D8\A9: \00", align 1
@str.const.240 = private unnamed_addr constant [27 x i8] c" (\D8\B9\D9\86\D8\AF \D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A9 2-4)\00", align 1
@str.const.243 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.247 = private unnamed_addr constant [44 x i8] c"  \D8\AA\D8\AE\D8\B5\D9\8A\D8\B5 x86_64 (16 \D9\85\D8\B3\D8\AC\D9\84 \D9\85\D8\AA\D8\A7\D8\AD):\00", align 1
@str.const.251 = private unnamed_addr constant [26 x i8] c"    r0 \E2\86\92 rdi (\D9\85\D8\AF\D8\AE\D9\84)\00", align 1
@str.const.255 = private unnamed_addr constant [26 x i8] c"    r1 \E2\86\92 rsi (\D9\85\D8\AF\D8\AE\D9\84)\00", align 1
@str.const.259 = private unnamed_addr constant [15 x i8] c"    r2 \E2\86\92 rax\00", align 1
@str.const.263 = private unnamed_addr constant [42 x i8] c"    r3 \E2\86\92 (\D9\85\D9\8F\D8\B7\D9\88\D9\89 \D9\81\D9\8A \D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A9)\00", align 1
@str.const.267 = private unnamed_addr constant [43 x i8] c"    r4 \E2\86\92 rax (\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9 \D8\A7\D8\B3\D8\AA\D8\B9\D9\85\D8\A7\D9\84)\00", align 1
@str.const.271 = private unnamed_addr constant [42 x i8] c"    \D8\A7\D9\86\D8\B3\D9\83\D8\A7\D8\A8 \D8\A5\D9\84\D9\89 \D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9: 0\00", align 1
@str.const.275 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.279 = private unnamed_addr constant [30 x i8] c"  \D8\AA\D8\AE\D8\B5\D9\8A\D8\B5 WASM (\D9\85\D9\83\D8\AF\D8\B3):\00", align 1
@str.const.283 = private unnamed_addr constant [52 x i8] c"    \D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D9\84\D9\85\D8\AA\D8\BA\D9\8A\D8\B1\D8\A7\D8\AA \D8\B9\D9\84\D9\89 \D8\A7\D9\84\D9\85\D9\83\D8\AF\D8\B3\00", align 1
@str.const.287 = private unnamed_addr constant [44 x i8] c"    \D9\84\D8\A7 \D8\AD\D8\A7\D8\AC\D8\A9 \D9\84\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5 \D9\85\D8\B3\D8\AC\D9\84\D8\A7\D8\AA\00", align 1
@str.const.291 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.295 = private unnamed_addr constant [59 x i8] c"=== \D8\A7\D9\84\D8\AC\D8\B2\D8\A1 4: \D8\B0\D8\A7\D9\83\D8\B1\D8\A9 \D8\A7\D9\84\D9\83\D9\88\D8\AF \D8\A7\D9\84\D9\85\D8\A4\D9\82\D8\AA\D8\A9 ===\00", align 1
@str.const.299 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.303 = private unnamed_addr constant [33 x i8] c"  \D8\B3\D8\B9\D8\A9 \D8\B0\D8\A7\D9\83\D8\B1\D8\A9 \D8\A7\D9\84\D9\83\D9\88\D8\AF: \00", align 1
@str.const.307 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.311 = private unnamed_addr constant [18 x i8] c"  \D8\A5\D8\AF\D8\AE\D8\A7\D9\84\D8\A7\D8\AA:\00", align 1
@str.const.315 = private unnamed_addr constant [6 x i8] c"CACHE\00", align 1
@str.const.316 = private unnamed_addr constant [33 x i8] c"\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9_\D8\B7\D9\84\D8\A8: 64 \D8\A8\D8\A7\D9\8A\D8\AA\00", align 1
@str.const.318 = private unnamed_addr constant [30 x i8] c"\D8\AA\D8\AD\D9\84\D9\8A\D9\84_HTTP: 128 \D8\A8\D8\A7\D9\8A\D8\AA\00", align 1
@str.const.320 = private unnamed_addr constant [39 x i8] c"\D8\AA\D8\B1\D9\85\D9\8A\D8\B2_\D8\A7\D8\B3\D8\AA\D8\AC\D8\A7\D8\A8\D8\A9: 96 \D8\A8\D8\A7\D9\8A\D8\AA\00", align 1
@str.const.322 = private unnamed_addr constant [40 x i8] c"\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA: 256 \D8\A8\D8\A7\D9\8A\D8\AA\00", align 1
@str.const.324 = private unnamed_addr constant [31 x i8] c"\D8\AA\D8\AD\D9\82\D9\82_\D8\A3\D9\85\D8\A7\D9\86: 48 \D8\A8\D8\A7\D9\8A\D8\AA\00", align 1
@str.const.325 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.329 = private unnamed_addr constant [14 x i8] c"  \D8\A7\D9\84\D9\88\D8\B6\D8\B9:\00", align 1
@str.const.333 = private unnamed_addr constant [19 x i8] c"    \D9\85\D8\B3\D8\AA\D8\B9\D9\85\D9\84: \00", align 1
@str.const.337 = private unnamed_addr constant [17 x i8] c"    \D9\85\D8\AA\D8\A8\D9\82\D9\8A: \00", align 1
@str.const.341 = private unnamed_addr constant [25 x i8] c"    \D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\B9\D9\85\D8\A7\D9\84: \00", align 1
@str.const.342 = private unnamed_addr constant [2 x i8] c"%\00", align 1
@str.const.345 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.350 = private unnamed_addr constant [51 x i8] c"\D9\85\D8\B3\D8\A7\D8\AD\D8\A9 \D9\83\D8\A7\D9\81\D9\8A\D8\A9 - \D9\84\D8\A7 \D8\AD\D8\A7\D8\AC\D8\A9 \D9\84\D8\A5\D8\AE\D9\84\D8\A7\D8\A1\00", align 1
@str.const.352 = private unnamed_addr constant [41 x i8] c"\D9\85\D8\B3\D8\A7\D8\AD\D8\A9 \D9\85\D9\86\D8\AE\D9\81\D8\B6\D8\A9 - \D8\AA\D9\81\D8\B9\D9\8A\D9\84 LRU\00", align 1
@str.const.353 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.357 = private unnamed_addr constant [65 x i8] c"=== \D8\A7\D9\84\D8\AC\D8\B2\D8\A1 5: \D8\A5\D9\84\D8\BA\D8\A7\D8\A1 \D8\A7\D9\84\D8\AA\D8\AD\D8\B3\D9\8A\D9\86 (Deoptimization) ===\00", align 1
@str.const.361 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.365 = private unnamed_addr constant [55 x i8] c"  \D8\A7\D9\84\D8\B3\D9\8A\D9\86\D8\A7\D8\B1\D9\8A\D9\88: \D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6 \D8\A3\D9\86\D9\88\D8\A7\D8\B9 \D9\83\D9\8F\D8\B3\D8\B1\00", align 1
@str.const.369 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.373 = private unnamed_addr constant [4 x i8] c"OPT\00", align 1
@str.const.374 = private unnamed_addr constant [37 x i8] c"\D8\A7\D9\84\D8\AF\D8\A7\D9\84\D8\A9: \D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9(\D9\82\D9\8A\D9\85\D8\A9)\00", align 1
@str.const.376 = private unnamed_addr constant [47 x i8] c"\D8\A7\D9\84\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6: \D9\82\D9\8A\D9\85\D8\A9 \D8\AF\D8\A7\D8\A6\D9\85\D8\A7\D9\8B \D8\B9\D8\AF\D8\AF\00", align 1
@str.const.378 = private unnamed_addr constant [52 x i8] c"\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5: \D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA \D8\B9\D8\AF\D8\AF \D8\B5\D8\AD\D9\8A\D8\AD \D8\B3\D8\B1\D9\8A\D8\B9\D8\A9\00", align 1
@str.const.379 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.383 = private unnamed_addr constant [20 x i8] c"    \D8\A7\D9\84\D8\AA\D9\83\D8\B1\D8\A7\D8\B1 \00", align 1
@str.const.384 = private unnamed_addr constant [51 x i8] c": \D9\82\D9\8A\D9\85\D8\A9 = \D8\B9\D8\AF\D8\AF \E2\86\92 [JIT: \D8\AA\D9\86\D9\81\D9\8A\D8\B0 \D8\B3\D8\B1\D9\8A\D8\B9]\00", align 1
@str.const.387 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.391 = private unnamed_addr constant [6 x i8] c"DEOPT\00", align 1
@str.const.392 = private unnamed_addr constant [66 x i8] c"\D8\A7\D9\84\D8\AA\D9\83\D8\B1\D8\A7\D8\B1 6: \D9\82\D9\8A\D9\85\D8\A9 = \D9\83\D9\84\D9\85\D8\A9! \D8\A7\D9\84\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6 \D9\83\D9\8F\D8\B3\D8\B1!\00", align 1
@str.const.393 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.397 = private unnamed_addr constant [40 x i8] c"  \D8\AE\D8\B7\D9\88\D8\A7\D8\AA \D8\A5\D9\84\D8\BA\D8\A7\D8\A1 \D8\A7\D9\84\D8\AA\D8\AD\D8\B3\D9\8A\D9\86:\00", align 1
@str.const.401 = private unnamed_addr constant [33 x i8] c"    1. \D8\A5\D9\8A\D9\82\D8\A7\D9\81 \D8\AA\D9\86\D9\81\D9\8A\D8\B0 JIT\00", align 1
@str.const.405 = private unnamed_addr constant [31 x i8] c"    2. \D8\AD\D9\81\D8\B8 \D8\A7\D9\84\D9\85\D8\B3\D8\AC\D9\84\D8\A7\D8\AA\00", align 1
@str.const.409 = private unnamed_addr constant [38 x i8] c"    3. \D8\A8\D9\86\D8\A7\D8\A1 \D8\A5\D8\B7\D8\A7\D8\B1 \D8\A7\D9\84\D9\85\D9\81\D8\B3\D8\B1\00", align 1
@str.const.413 = private unnamed_addr constant [33 x i8] c"    4. \D8\A7\D9\84\D8\B9\D9\88\D8\AF\D8\A9 \D9\84\D9\84\D9\85\D9\81\D8\B3\D8\B1\00", align 1
@str.const.417 = private unnamed_addr constant [44 x i8] c"    5. \D8\AA\D8\AD\D8\AF\D9\8A\D8\AB \D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\AD\D9\84\D9\84\00", align 1
@str.const.421 = private unnamed_addr constant [53 x i8] c"    6. \D8\A5\D8\B9\D8\A7\D8\AF\D8\A9 \D8\A8\D9\86\D8\A7\D8\A1 \D8\A8\D8\AF\D9\88\D9\86 \D8\A7\D9\84\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\00", align 1
@str.const.425 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.429 = private unnamed_addr constant [6 x i8] c"REBLD\00", align 1
@str.const.430 = private unnamed_addr constant [56 x i8] c"\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9 \D8\A8\D9\86\D8\A7\D8\A1 \D8\A8\D8\A3\D9\86\D9\88\D8\A7\D8\B9 \D8\B9\D8\A7\D9\85\D8\A9 (polymorphic)\00", align 1
@str.const.431 = private unnamed_addr constant [3 x i8] c"GO\00", align 1
@str.const.432 = private unnamed_addr constant [60 x i8] c"\D8\A7\D8\B3\D8\AA\D8\A6\D9\86\D8\A7\D9\81 \D8\A7\D9\84\D8\AA\D9\86\D9\81\D9\8A\D8\B0 \D8\A8\D8\A7\D9\84\D9\86\D8\B3\D8\AE\D8\A9 \D8\A7\D9\84\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9\00", align 1
@str.const.433 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.437 = private unnamed_addr constant [38 x i8] c"=== \D8\A7\D9\84\D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D9\86\D9\87\D8\A7\D8\A6\D9\8A ===\00", align 1
@str.const.441 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.445 = private unnamed_addr constant [129 x i8] c"  \E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.449 = private unnamed_addr constant [70 x i8] c"  \E2\95\91  \D9\85\D8\AD\D8\B1\D9\83 \D8\A7\D9\84\D8\AA\D8\B1\D8\AC\D9\85\D8\A9 \D9\85\D8\AA\D8\B9\D8\AF\D8\AF \D8\A7\D9\84\D8\A3\D9\87\D8\AF\D8\A7\D9\81          \E2\95\91\00", align 1
@str.const.453 = private unnamed_addr constant [129 x i8] c"  \E2\95\A0\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\A3\00", align 1
@str.const.457 = private unnamed_addr constant [63 x i8] c"  \E2\95\91  \D8\A7\D9\84\D9\85\D9\86\D8\B5\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\AF\D8\B9\D9\88\D9\85\D8\A9:                    \E2\95\91\00", align 1
@str.const.461 = private unnamed_addr constant [49 x i8] c"  \E2\95\91    - x86_64-linux (Intel/AMD)          \E2\95\91\00", align 1
@str.const.465 = private unnamed_addr constant [49 x i8] c"  \E2\95\91    - aarch64-linux (ARM)               \E2\95\91\00", align 1
@str.const.469 = private unnamed_addr constant [49 x i8] c"  \E2\95\91    - wasm32 (WebAssembly)              \E2\95\91\00", align 1
@str.const.473 = private unnamed_addr constant [49 x i8] c"  \E2\95\91    - riscv64 (RISC-V)                  \E2\95\91\00", align 1
@str.const.481 = private unnamed_addr constant [55 x i8] c"  \E2\95\91  \D8\A7\D9\84\D8\A3\D8\AC\D8\B2\D8\A7\D8\A1:                             \E2\95\91\00", align 1
@str.const.485 = private unnamed_addr constant [53 x i8] c"  \E2\95\91    1. \D8\AA\D8\AD\D9\84\D9\8A\D9\84 IR                        \E2\95\91\00", align 1
@str.const.489 = private unnamed_addr constant [68 x i8] c"  \E2\95\91    2. \D8\AA\D9\88\D9\84\D9\8A\D8\AF \D9\83\D9\88\D8\AF \D9\85\D8\AA\D8\B9\D8\AF\D8\AF \D8\A7\D9\84\D9\85\D9\86\D8\B5\D8\A7\D8\AA         \E2\95\91\00", align 1
@str.const.493 = private unnamed_addr constant [61 x i8] c"  \E2\95\91    3. \D8\AA\D8\AE\D8\B5\D9\8A\D8\B5 \D8\A7\D9\84\D9\85\D8\B3\D8\AC\D9\84\D8\A7\D8\AA                  \E2\95\91\00", align 1
@str.const.497 = private unnamed_addr constant [65 x i8] c"  \E2\95\91    4. \D8\B0\D8\A7\D9\83\D8\B1\D8\A9 \D8\A7\D9\84\D9\83\D9\88\D8\AF \D8\A7\D9\84\D9\85\D8\A4\D9\82\D8\AA\D8\A9             \E2\95\91\00", align 1
@str.const.501 = private unnamed_addr constant [60 x i8] c"  \E2\95\91    5. \D8\A5\D9\84\D8\BA\D8\A7\D8\A1 \D8\A7\D9\84\D8\AA\D8\AD\D8\B3\D9\8A\D9\86                   \E2\95\91\00", align 1
@str.const.509 = private unnamed_addr constant [73 x i8] c"  \E2\95\91  \D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9: \D9\86\D8\B8\D8\A7\D9\85 \D8\AA\D8\B1\D8\AC\D9\85\D8\A9 \D8\B4\D8\A7\D9\85\D9\84 \D9\8A\D8\B9\D9\85\D9\84 \E2\9C\93     \E2\95\91\00", align 1
@str.const.513 = private unnamed_addr constant [129 x i8] c"  \E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.517 = private unnamed_addr constant [121 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.521 = private unnamed_addr constant [65 x i8] c"\E2\95\91  \D9\85\D8\AD\D8\B1\D9\83 \D8\AA\D8\B1\D8\AC\D9\85\D8\A9 \D9\85\D8\AA\D8\B9\D8\AF\D8\AF \D8\A7\D9\84\D8\A3\D9\87\D8\AF\D8\A7\D9\81           \E2\95\91\00", align 1
@str.const.525 = private unnamed_addr constant [121 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.529 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.533 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.537 = private unnamed_addr constant [32 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 \D8\A7\D9\84\D9\85\D8\AD\D8\B1\D9\83 ===\00", align 1

define i64 @"\D8\B3\D8\AC_\D9\85\D8\AD\D8\B1\D9\83"(ptr %"\D9\85\D9\83\D9\88\D9\86", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92" = alloca ptr, align 8
  store ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92", align 8
  %"\D9\85\D9\83\D9\88\D9\861" = alloca ptr, align 8
  store ptr %"\D9\85\D9\83\D9\88\D9\86", ptr %"\D9\85\D9\83\D9\88\D9\861", align 8
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %"\D9\85\D9\83\D9\88\D9\861")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\85\D9\83\D9\88\D9\861", i64 %copylen2)
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

define ptr @"\D8\A7\D8\B3\D9\85_\D9\85\D9\86\D8\B5\D8\A9"(i64 %"\D9\85\D8\B9\D8\B1\D9\81") {
entry:
  %"\D9\85\D8\B9\D8\B1\D9\811" = alloca i64, align 8
  store i64 %"\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %"%\D9\85\D8\B9\D8\B1\D9\81.load" = load i64, ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D8\B9\D8\B1\D9\81.load", 1
  br i1 %cmpeqtmp, label %then_0, label %merge_1

then_0:                                           ; preds = %entry
  ret ptr @str.ret

merge_1:                                          ; preds = %entry
  %"%\D9\85\D8\B9\D8\B1\D9\81.load2" = load i64, ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D9\85\D8\B9\D8\B1\D9\81.load2", 2
  br i1 %cmpeqtmp3, label %then_2, label %merge_3

then_2:                                           ; preds = %merge_1
  ret ptr @str.ret.4

merge_3:                                          ; preds = %merge_1
  %"%\D9\85\D8\B9\D8\B1\D9\81.load4" = load i64, ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D9\85\D8\B9\D8\B1\D9\81.load4", 3
  br i1 %cmpeqtmp5, label %then_4, label %merge_5

then_4:                                           ; preds = %merge_3
  ret ptr @str.ret.5

merge_5:                                          ; preds = %merge_3
  %"%\D9\85\D8\B9\D8\B1\D9\81.load6" = load i64, ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D9\85\D8\B9\D8\B1\D9\81.load6", 4
  br i1 %cmpeqtmp7, label %then_6, label %merge_7

then_6:                                           ; preds = %merge_5
  ret ptr @str.ret.6

merge_7:                                          ; preds = %merge_5
  ret ptr @str.ret.7
}

define i64 @"\D9\85\D9\82\D8\A7\D8\B3_\D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A9"(i64 %"\D9\85\D9\86\D8\B5\D8\A9") {
entry:
  %"\D9\85\D9\86\D8\B5\D8\A91" = alloca i64, align 8
  store i64 %"\D9\85\D9\86\D8\B5\D8\A9", ptr %"\D9\85\D9\86\D8\B5\D8\A91", align 8
  %"%\D9\85\D9\86\D8\B5\D8\A9.load" = load i64, ptr %"\D9\85\D9\86\D8\B5\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D9\86\D8\B5\D8\A9.load", 1
  br i1 %cmpeqtmp, label %then_8, label %merge_9

then_8:                                           ; preds = %entry
  ret i64 4

merge_9:                                          ; preds = %entry
  %"%\D9\85\D9\86\D8\B5\D8\A9.load2" = load i64, ptr %"\D9\85\D9\86\D8\B5\D8\A91", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D9\85\D9\86\D8\B5\D8\A9.load2", 2
  br i1 %cmpeqtmp3, label %then_10, label %merge_11

then_10:                                          ; preds = %merge_9
  ret i64 4

merge_11:                                         ; preds = %merge_9
  %"%\D9\85\D9\86\D8\B5\D8\A9.load4" = load i64, ptr %"\D9\85\D9\86\D8\B5\D8\A91", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D9\85\D9\86\D8\B5\D8\A9.load4", 3
  br i1 %cmpeqtmp5, label %then_12, label %merge_13

then_12:                                          ; preds = %merge_11
  ret i64 1

merge_13:                                         ; preds = %merge_11
  %"%\D9\85\D9\86\D8\B5\D8\A9.load6" = load i64, ptr %"\D9\85\D9\86\D8\B5\D8\A91", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D9\85\D9\86\D8\B5\D8\A9.load6", 4
  br i1 %cmpeqtmp7, label %then_14, label %merge_15

then_14:                                          ; preds = %merge_13
  ret i64 4

merge_15:                                         ; preds = %merge_13
  ret i64 4
}

define i64 @"\D8\B9\D8\AF\D8\AF_\D9\85\D8\B3\D8\AC\D9\84\D8\A7\D8\AA"(i64 %"\D9\85\D9\86\D8\B5\D8\A9") {
entry:
  %"\D9\85\D9\86\D8\B5\D8\A91" = alloca i64, align 8
  store i64 %"\D9\85\D9\86\D8\B5\D8\A9", ptr %"\D9\85\D9\86\D8\B5\D8\A91", align 8
  %"%\D9\85\D9\86\D8\B5\D8\A9.load" = load i64, ptr %"\D9\85\D9\86\D8\B5\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D9\86\D8\B5\D8\A9.load", 1
  br i1 %cmpeqtmp, label %then_16, label %merge_17

then_16:                                          ; preds = %entry
  ret i64 16

merge_17:                                         ; preds = %entry
  %"%\D9\85\D9\86\D8\B5\D8\A9.load2" = load i64, ptr %"\D9\85\D9\86\D8\B5\D8\A91", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D9\85\D9\86\D8\B5\D8\A9.load2", 2
  br i1 %cmpeqtmp3, label %then_18, label %merge_19

then_18:                                          ; preds = %merge_17
  ret i64 31

merge_19:                                         ; preds = %merge_17
  %"%\D9\85\D9\86\D8\B5\D8\A9.load4" = load i64, ptr %"\D9\85\D9\86\D8\B5\D8\A91", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D9\85\D9\86\D8\B5\D8\A9.load4", 3
  br i1 %cmpeqtmp5, label %then_20, label %merge_21

then_20:                                          ; preds = %merge_19
  ret i64 0

merge_21:                                         ; preds = %merge_19
  %"%\D9\85\D9\86\D8\B5\D8\A9.load6" = load i64, ptr %"\D9\85\D9\86\D8\B5\D8\A91", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D9\85\D9\86\D8\B5\D8\A9.load6", 4
  br i1 %cmpeqtmp7, label %then_22, label %merge_23

then_22:                                          ; preds = %merge_21
  ret i64 32

merge_23:                                         ; preds = %merge_21
  ret i64 0
}

define i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\AA\D8\AD\D9\84\D9\8A\D9\84_IR"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.8)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.16)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.20)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.24)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.28)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.32)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.36)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.40)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.44)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.48)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.52)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.56)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.60)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.64)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.68)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 6, ptr %alloca, align 8
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.75)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\AA\D9\88\D9\84\D9\8A\D8\AF_\D9\83\D9\88\D8\AF_\D9\85\D9\86\D8\B5\D8\A9"(i64 %"\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\85") {
entry:
  %"\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\851" = alloca i64, align 8
  store i64 %"\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\85", ptr %"\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\851", align 8
  %"%\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\85.load" = load i64, ptr %"\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\851", align 8
  %calltmp = call ptr @"\D8\A7\D8\B3\D9\85_\D9\85\D9\86\D8\B5\D8\A9"(i64 %"%\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\85.load")
  %alloca = alloca ptr, align 8
  store ptr %calltmp, ptr %alloca, align 8
  %"%\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\85.load2" = load i64, ptr %"\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\851", align 8
  %calltmp3 = call i64 @"\D9\85\D9\82\D8\A7\D8\B3_\D8\AA\D8\B9\D9\84\D9\8A\D9\85\D8\A9"(i64 %"%\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\85.load2")
  %alloca4 = alloca i64, align 8
  store i64 %calltmp3, ptr %alloca4, align 8
  %"%\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\85.load5" = load i64, ptr %"\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\851", align 8
  %calltmp6 = call i64 @"\D8\B9\D8\AF\D8\AF_\D9\85\D8\B3\D8\AC\D9\84\D8\A7\D8\AA"(i64 %"%\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\85.load5")
  %alloca7 = alloca i64, align 8
  store i64 %calltmp6, ptr %alloca7, align 8
  %len1 = call i64 @strlen(ptr @str.const.79)
  %len2 = call i64 @strlen(ptr %alloca)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.79, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %alloca, i64 %copylen2)
  %len18 = call i64 @strlen(ptr %concat.buf)
  %len29 = call i64 @strlen(ptr @str.const.80)
  %totallen10 = add i64 %len18, %len29
  %bufsize11 = add i64 %totallen10, 1
  %concat.buf12 = alloca i8, i64 %bufsize11, align 1
  %2 = call ptr @memcpy(ptr %concat.buf12, ptr %concat.buf, i64 %len18)
  %dest213 = getelementptr i8, ptr %concat.buf12, i64 %len18
  %copylen214 = add i64 %len29, 1
  %3 = call ptr @memcpy(ptr %dest213, ptr @str.const.80, i64 %copylen214)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf12)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.91)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\85.load15" = load i64, ptr %"\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\851", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\85.load15", 1
  br i1 %cmpeqtmp, label %then_24, label %merge_25

then_24:                                          ; preds = %entry
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.95)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.99)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.103)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.107)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.111)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D9\85\D9\82_\D8\AA\D8\B9\D9\84.load" = load i64, ptr %alloca4, align 8
  %multmp = mul i64 4, %"%\D9\85\D9\82_\D8\AA\D8\B9\D9\84.load"
  %alloca16 = alloca i64, align 8
  store i64 %multmp, ptr %alloca16, align 8
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_25

merge_25:                                         ; preds = %then_24, %entry
  %"%\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\85.load17" = load i64, ptr %"\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\851", align 8
  %cmpeqtmp18 = icmp eq i64 %"%\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\85.load17", 2
  br i1 %cmpeqtmp18, label %then_26, label %merge_27

then_26:                                          ; preds = %merge_25
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.119)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.123)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.127)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.111)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D9\85\D9\82_\D8\AA\D8\B9\D9\84.load19" = load i64, ptr %alloca4, align 8
  %multmp20 = mul i64 3, %"%\D9\85\D9\82_\D8\AA\D8\B9\D9\84.load19"
  %alloca21 = alloca i64, align 8
  store i64 %multmp20, ptr %alloca21, align 8
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_27

merge_27:                                         ; preds = %then_26, %merge_25
  %"%\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\85.load22" = load i64, ptr %"\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\851", align 8
  %cmpeqtmp23 = icmp eq i64 %"%\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\85.load22", 3
  br i1 %cmpeqtmp23, label %then_28, label %merge_29

then_28:                                          ; preds = %merge_27
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.139)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.143)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.147)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.151)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.155)
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %40 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.159)
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D9\85\D9\82_\D8\AA\D8\B9\D9\84.load24" = load i64, ptr %alloca4, align 8
  %multmp25 = mul i64 5, %"%\D9\85\D9\82_\D8\AA\D8\B9\D9\84.load24"
  %alloca26 = alloca i64, align 8
  store i64 %multmp25, ptr %alloca26, align 8
  %42 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_29

merge_29:                                         ; preds = %then_28, %merge_27
  %"%\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\85.load27" = load i64, ptr %"\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\851", align 8
  %cmpeqtmp28 = icmp eq i64 %"%\D9\85\D9\86\D8\B5\D8\A9_\D8\B1\D9\82\D9\85.load27", 4
  br i1 %cmpeqtmp28, label %then_30, label %merge_31

then_30:                                          ; preds = %merge_29
  %43 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.167)
  %44 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %45 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.171)
  %46 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %47 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.175)
  %48 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %49 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.111)
  %50 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D9\85\D9\82_\D8\AA\D8\B9\D9\84.load29" = load i64, ptr %alloca4, align 8
  %multmp30 = mul i64 3, %"%\D9\85\D9\82_\D8\AA\D8\B9\D9\84.load29"
  %alloca31 = alloca i64, align 8
  store i64 %multmp30, ptr %alloca31, align 8
  %51 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %merge_31

merge_31:                                         ; preds = %then_30, %merge_29
  %52 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.187)
  %53 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9_\D9\85\D9\86\D8\B5\D8\A7\D8\AA"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.191)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.195)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 1, ptr %alloca, align 8
  br label %while_cond_32

while_cond_32:                                    ; preds = %while_body_33, %entry
  %"%\D9\85.load" = load i64, ptr %alloca, align 8
  %cmpletmp = icmp sle i64 %"%\D9\85.load", 4
  br i1 %cmpletmp, label %while_body_33, label %while_exit_34

while_body_33:                                    ; preds = %while_cond_32
  %"%\D9\85.load1" = load i64, ptr %alloca, align 8
  %calltmp = call i64 @"\D8\AA\D9\88\D9\84\D9\8A\D8\AF_\D9\83\D9\88\D8\AF_\D9\85\D9\86\D8\B5\D8\A9"(i64 %"%\D9\85.load1")
  %"%\D9\85.load2" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\85.load2", 1
  store i64 %addtmp, ptr %alloca, align 8
  br label %while_cond_32

while_exit_34:                                    ; preds = %while_cond_32
  ret i64 0
}

define i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.199)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.203)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.207)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.211)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.215)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.219)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.223)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.227)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.231)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.235)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 3, ptr %alloca, align 8
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.243)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.247)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.251)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.255)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.259)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.263)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.267)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.271)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.275)
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.279)
  %40 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.283)
  %42 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %43 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.287)
  %44 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %45 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.291)
  %46 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\83\D9\88\D8\AF"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.295)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.299)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 1024, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.307)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.311)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D8\B3\D8\AC_\D9\85\D8\AD\D8\B1\D9\83"(ptr @str.const.315, ptr @str.const.316)
  %"%\D9\85\D8\B3\D8\AA\D8\B9\D9\85\D9\84.load" = load i64, ptr %alloca1, align 8
  %addtmp = add i64 %"%\D9\85\D8\B3\D8\AA\D8\B9\D9\85\D9\84.load", 64
  store i64 %addtmp, ptr %alloca1, align 8
  %calltmp2 = call i64 @"\D8\B3\D8\AC_\D9\85\D8\AD\D8\B1\D9\83"(ptr @str.const.315, ptr @str.const.318)
  %"%\D9\85\D8\B3\D8\AA\D8\B9\D9\85\D9\84.load3" = load i64, ptr %alloca1, align 8
  %addtmp4 = add i64 %"%\D9\85\D8\B3\D8\AA\D8\B9\D9\85\D9\84.load3", 128
  store i64 %addtmp4, ptr %alloca1, align 8
  %calltmp5 = call i64 @"\D8\B3\D8\AC_\D9\85\D8\AD\D8\B1\D9\83"(ptr @str.const.315, ptr @str.const.320)
  %"%\D9\85\D8\B3\D8\AA\D8\B9\D9\85\D9\84.load6" = load i64, ptr %alloca1, align 8
  %addtmp7 = add i64 %"%\D9\85\D8\B3\D8\AA\D8\B9\D9\85\D9\84.load6", 96
  store i64 %addtmp7, ptr %alloca1, align 8
  %calltmp8 = call i64 @"\D8\B3\D8\AC_\D9\85\D8\AD\D8\B1\D9\83"(ptr @str.const.315, ptr @str.const.322)
  %"%\D9\85\D8\B3\D8\AA\D8\B9\D9\85\D9\84.load9" = load i64, ptr %alloca1, align 8
  %addtmp10 = add i64 %"%\D9\85\D8\B3\D8\AA\D8\B9\D9\85\D9\84.load9", 256
  store i64 %addtmp10, ptr %alloca1, align 8
  %calltmp11 = call i64 @"\D8\B3\D8\AC_\D9\85\D8\AD\D8\B1\D9\83"(ptr @str.const.315, ptr @str.const.324)
  %"%\D9\85\D8\B3\D8\AA\D8\B9\D9\85\D9\84.load12" = load i64, ptr %alloca1, align 8
  %addtmp13 = add i64 %"%\D9\85\D8\B3\D8\AA\D8\B9\D9\85\D9\84.load12", 48
  store i64 %addtmp13, ptr %alloca1, align 8
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.325)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\B3\D8\B9\D8\A9_\D9\83\D8\A7\D8\B4.load" = load i64, ptr %alloca, align 8
  %"%\D9\85\D8\B3\D8\AA\D8\B9\D9\85\D9\84.load14" = load i64, ptr %alloca1, align 8
  %subtmp = sub i64 %"%\D8\B3\D8\B9\D8\A9_\D9\83\D8\A7\D8\B4.load", %"%\D9\85\D8\B3\D8\AA\D8\B9\D9\85\D9\84.load14"
  %alloca15 = alloca i64, align 8
  store i64 %subtmp, ptr %alloca15, align 8
  %"%\D9\85\D8\B3\D8\AA\D8\B9\D9\85\D9\84.load16" = load i64, ptr %alloca1, align 8
  %multmp = mul i64 %"%\D9\85\D8\B3\D8\AA\D8\B9\D9\85\D9\84.load16", 100
  %"%\D8\B3\D8\B9\D8\A9_\D9\83\D8\A7\D8\B4.load17" = load i64, ptr %alloca, align 8
  %divtmp = sdiv i64 %multmp, %"%\D8\B3\D8\B9\D8\A9_\D9\83\D8\A7\D8\B4.load17"
  %alloca18 = alloca i64, align 8
  store i64 %divtmp, ptr %alloca18, align 8
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.329)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.345)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\B4.load" = load i64, ptr %alloca18, align 8
  %cmplttmp = icmp slt i64 %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\B4.load", 80
  br i1 %cmplttmp, label %then_35, label %else_37

then_35:                                          ; preds = %entry
  %calltmp19 = call i64 @"\D8\B3\D8\AC_\D9\85\D8\AD\D8\B1\D9\83"(ptr @str.const.315, ptr @str.const.350)
  br label %merge_36

else_37:                                          ; preds = %entry
  %calltmp20 = call i64 @"\D8\B3\D8\AC_\D9\85\D8\AD\D8\B1\D9\83"(ptr @str.const.315, ptr @str.const.352)
  br label %merge_36

merge_36:                                         ; preds = %else_37, %then_35
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.353)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\A5\D9\84\D8\BA\D8\A7\D8\A1_\D8\AA\D8\AD\D8\B3\D9\8A\D9\86"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.357)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.361)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.365)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.369)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D8\B3\D8\AC_\D9\85\D8\AD\D8\B1\D9\83"(ptr @str.const.373, ptr @str.const.374)
  %calltmp1 = call i64 @"\D8\B3\D8\AC_\D9\85\D8\AD\D8\B1\D9\83"(ptr @str.const.373, ptr @str.const.376)
  %calltmp2 = call i64 @"\D8\B3\D8\AC_\D9\85\D8\AD\D8\B1\D9\83"(ptr @str.const.373, ptr @str.const.378)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.379)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  br label %while_cond_38

while_cond_38:                                    ; preds = %while_body_39, %entry
  %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_\D9\86\D8\A7\D8\AC\D8\AD.load" = load i64, ptr %alloca, align 8
  %cmplttmp = icmp slt i64 %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_\D9\86\D8\A7\D8\AC\D8\AD.load", 5
  br i1 %cmplttmp, label %while_body_39, label %while_exit_40

while_body_39:                                    ; preds = %while_cond_38
  %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_\D9\86\D8\A7\D8\AC\D8\AD.load3" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_\D9\86\D8\A7\D8\AC\D8\AD.load3", 1
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_\D9\86\D8\A7\D8\AC\D8\AD.load4" = load i64, ptr %alloca, align 8
  %addtmp5 = add i64 %"%\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_\D9\86\D8\A7\D8\AC\D8\AD.load4", 1
  store i64 %addtmp5, ptr %alloca, align 8
  br label %while_cond_38

while_exit_40:                                    ; preds = %while_cond_38
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.387)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp6 = call i64 @"\D8\B3\D8\AC_\D9\85\D8\AD\D8\B1\D9\83"(ptr @str.const.391, ptr @str.const.392)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.393)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.397)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.401)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.405)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.409)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.413)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.417)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.421)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.425)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp7 = call i64 @"\D8\B3\D8\AC_\D9\85\D8\AD\D8\B1\D9\83"(ptr @str.const.429, ptr @str.const.430)
  %calltmp8 = call i64 @"\D8\B3\D8\AC_\D9\85\D8\AD\D8\B1\D9\83"(ptr @str.const.431, ptr @str.const.432)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.433)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\86\D9\87\D8\A7\D8\A6\D9\8A"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.437)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.441)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.445)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.449)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.453)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.457)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.461)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.465)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.469)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.473)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.453)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.481)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.485)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.489)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.493)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.497)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.501)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.453)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.509)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.513)
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.517)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.521)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.525)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.529)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %calltmp = call i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\AA\D8\AD\D9\84\D9\8A\D9\84_IR"()
  %calltmp1 = call i64 @"\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9_\D9\85\D9\86\D8\B5\D8\A7\D8\AA"()
  %calltmp2 = call i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5"()
  %calltmp3 = call i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\83\D9\88\D8\AF"()
  %calltmp4 = call i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D8\A5\D9\84\D8\BA\D8\A7\D8\A1_\D8\AA\D8\AD\D8\B3\D9\8A\D9\86"()
  %calltmp5 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D9\86\D9\87\D8\A7\D8\A6\D9\8A"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.533)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.537)
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
