; ModuleID = '05_complex_dependency_resolver'
source_filename = "05_complex_dependency_resolver"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [163 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [163 x i8] c"\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\00", align 1
@str.const.7 = private unnamed_addr constant [81 x i8] c"\C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7 \C2\B7\00", align 1
@str.const.11 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.15 = private unnamed_addr constant [8 x i8] c"  \F0\9F\93\A6 \00", align 1
@str.const.20 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.24 = private unnamed_addr constant [7 x i8] c"  \E2\96\B8 \00", align 1
@str.const.29 = private unnamed_addr constant [5 x i8] c"    \00", align 1
@str.const.32 = private unnamed_addr constant [3 x i8] c": \00", align 1
@fmt.d = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1
@str.const.44 = private unnamed_addr constant [9 x i8] c"    \E2\9C\85 \00", align 1
@str.const.49 = private unnamed_addr constant [9 x i8] c"    \E2\9D\8C \00", align 1
@str.const.54 = private unnamed_addr constant [9 x i8] c"    \E2\9A\A0 \00", align 1
@str.const.59 = private unnamed_addr constant [9 x i8] c"    \E2\84\B9 \00", align 1
@str.const.67 = private unnamed_addr constant [7 x i8] c" \E2\86\92 v\00", align 1
@str.const.69 = private unnamed_addr constant [2 x i8] c".\00", align 1
@str.const.75 = private unnamed_addr constant [14 x i8] c" (\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A=\00", align 1
@str.const.77 = private unnamed_addr constant [11 x i8] c" \D9\81\D8\B1\D8\B9\D9\8A=\00", align 1
@str.const.80 = private unnamed_addr constant [13 x i8] c" \D8\AA\D8\B5\D8\AD\D9\8A\D8\AD=\00", align 1
@str.const.83 = private unnamed_addr constant [2 x i8] c")\00", align 1
@str.const.87 = private unnamed_addr constant [43 x i8] c"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\AA\D9\88\D8\A7\D9\81\D9\82 \D8\A7\D9\84\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1\D8\A7\D8\AA\00", align 1
@str.const.88 = private unnamed_addr constant [30 x i8] c"\D8\A7\D9\84\D8\AA\D9\88\D8\A7\D9\81\D9\82 \D8\A7\D9\84\D9\85\D8\B1\D9\86 (^)\00", align 1
@str.const.89 = private unnamed_addr constant [24 x i8] c"    ^2.1.0 \D9\85\D8\B9 2.3.5: \00", align 1
@str.const.91 = private unnamed_addr constant [17 x i8] c"\E2\9C\85 \D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82\00", align 1
@str.const.95 = private unnamed_addr constant [24 x i8] c"\E2\9D\8C \D8\BA\D9\8A\D8\B1 \D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82\00", align 1
@str.const.99 = private unnamed_addr constant [24 x i8] c"    ^2.1.0 \D9\85\D8\B9 3.0.0: \00", align 1
@str.const.105 = private unnamed_addr constant [59 x i8] c"\E2\9D\8C \D8\BA\D9\8A\D8\B1 \D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82 (\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1 \D8\B1\D8\A6\D9\8A\D8\B3\D9\8A \D9\85\D8\AE\D8\AA\D9\84\D9\81)\00", align 1
@str.const.109 = private unnamed_addr constant [24 x i8] c"    ^2.1.0 \D9\85\D8\B9 2.0.9: \00", align 1
@str.const.115 = private unnamed_addr constant [53 x i8] c"\E2\9D\8C \D8\BA\D9\8A\D8\B1 \D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82 (\D8\A3\D9\82\D9\84 \D9\85\D9\86 \D8\A7\D9\84\D9\85\D8\B7\D9\84\D9\88\D8\A8)\00", align 1
@str.const.119 = private unnamed_addr constant [32 x i8] c"\D8\A7\D9\84\D8\AA\D9\88\D8\A7\D9\81\D9\82 \D8\A7\D9\84\D8\B5\D8\A7\D8\B1\D9\85 (~)\00", align 1
@str.const.120 = private unnamed_addr constant [24 x i8] c"    ~1.3.5 \D9\85\D8\B9 1.3.8: \00", align 1
@str.const.130 = private unnamed_addr constant [24 x i8] c"    ~1.3.5 \D9\85\D8\B9 1.4.0: \00", align 1
@str.const.136 = private unnamed_addr constant [46 x i8] c"\E2\9D\8C \D8\BA\D9\8A\D8\B1 \D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82 (\D9\81\D8\B1\D8\B9\D9\8A \D9\85\D8\AE\D8\AA\D9\84\D9\81)\00", align 1
@str.const.140 = private unnamed_addr constant [13 x i8] c"\D9\85\D8\AC\D9\87\D9\88\D9\84\D8\A9\00", align 1
@str.const.141 = private unnamed_addr constant [15 x i8] c"\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.142 = private unnamed_addr constant [11 x i8] c"\D9\87\D9\86\D8\AF\D8\B3\D8\A9\00", align 1
@str.const.143 = private unnamed_addr constant [13 x i8] c"\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A1\00", align 1
@str.const.144 = private unnamed_addr constant [11 x i8] c"\D8\A5\D8\AD\D8\B5\D8\A7\D8\A1\00", align 1
@str.const.145 = private unnamed_addr constant [7 x i8] c"\D8\B1\D8\B3\D9\85\00", align 1
@str.const.146 = private unnamed_addr constant [41 x i8] c"\D9\83\D8\B4\D9\81 \D8\A7\D9\84\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D8\AF\D8\A7\D8\A6\D8\B1\D9\8A\D8\A9\00", align 1
@str.const.147 = private unnamed_addr constant [28 x i8] c"\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9 \D8\A7\D9\84\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.148 = private unnamed_addr constant [40 x i8] c"    \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA \E2\94\80\E2\94\80\E2\86\92 \D9\87\D9\86\D8\AF\D8\B3\D8\A9\00", align 1
@str.const.152 = private unnamed_addr constant [38 x i8] c"    \D9\87\D9\86\D8\AF\D8\B3\D8\A9 \E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\86\92 \D8\B1\D8\B3\D9\85\00", align 1
@str.const.156 = private unnamed_addr constant [64 x i8] c"    \D8\B1\D8\B3\D9\85 \E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\86\92 \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA \E2\9F\B2 (\D8\AD\D9\84\D9\82\D8\A9!)\00", align 1
@str.const.160 = private unnamed_addr constant [45 x i8] c"    \D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A1 \E2\94\80\E2\94\80\E2\94\80\E2\86\92 \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.164 = private unnamed_addr constant [43 x i8] c"    \D8\A5\D8\AD\D8\B5\D8\A7\D8\A1 \E2\94\80\E2\94\80\E2\94\80\E2\86\92 \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.168 = private unnamed_addr constant [28 x i8] c"\D8\AA\D8\AA\D8\A8\D8\B9 \D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1 (DFS)\00", align 1
@str.const.169 = private unnamed_addr constant [37 x i8] c"    \F0\9F\94\8D \D8\A8\D8\AF\D8\A1 \D9\85\D9\86: \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.173 = private unnamed_addr constant [61 x i8] c"    \E2\94\82 \D8\B2\D9\8A\D8\A7\D8\B1\D8\A9: \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA [\D9\82\D9\8A\D8\AF \D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9]\00", align 1
@str.const.177 = private unnamed_addr constant [12 x i8] c"    \E2\94\82 \E2\86\93\00", align 1
@str.const.181 = private unnamed_addr constant [57 x i8] c"    \E2\94\82 \D8\B2\D9\8A\D8\A7\D8\B1\D8\A9: \D9\87\D9\86\D8\AF\D8\B3\D8\A9 [\D9\82\D9\8A\D8\AF \D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9]\00", align 1
@str.const.189 = private unnamed_addr constant [53 x i8] c"    \E2\94\82 \D8\B2\D9\8A\D8\A7\D8\B1\D8\A9: \D8\B1\D8\B3\D9\85 [\D9\82\D9\8A\D8\AF \D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9]\00", align 1
@str.const.197 = private unnamed_addr constant [82 x i8] c"    \E2\94\82 \E2\9F\B2 \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA [\D9\82\D9\8A\D8\AF \D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9] \E2\86\90 \D8\A7\D9\83\D8\AA\D9\8F\D8\B4\D9\81\D8\AA \D8\AD\D9\84\D9\82\D8\A9!\00", align 1
@str.const.201 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.205 = private unnamed_addr constant [100 x i8] c"\D8\A7\D9\83\D8\AA\D9\8F\D8\B4\D9\81\D8\AA \D8\AA\D8\A8\D8\B9\D9\8A\D8\A9 \D8\AF\D8\A7\D8\A6\D8\B1\D9\8A\D8\A9: \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA \E2\86\92 \D9\87\D9\86\D8\AF\D8\B3\D8\A9 \E2\86\92 \D8\B1\D8\B3\D9\85 \E2\86\92 \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.206 = private unnamed_addr constant [55 x i8] c"\D9\8A\D8\AC\D8\A8 \D9\83\D8\B3\D8\B1 \D8\A7\D9\84\D8\AD\D9\84\D9\82\D8\A9 \D9\84\D8\A5\D9\83\D9\85\D8\A7\D9\84 \D8\A7\D9\84\D8\AA\D8\AB\D8\A8\D9\8A\D8\AA\00", align 1
@str.const.207 = private unnamed_addr constant [30 x i8] c"\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D8\A2\D9\85\D9\86\D8\A9\00", align 1
@str.const.208 = private unnamed_addr constant [57 x i8] c"    \F0\9F\94\8D \D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A1 \E2\86\92 \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA (\D9\84\D8\A7 \D8\AD\D9\84\D9\82\D8\A9)\00", align 1
@str.const.212 = private unnamed_addr constant [47 x i8] c"\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A1: \D8\B3\D9\84\D8\B3\D9\84\D8\A9 \D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA \D8\A2\D9\85\D9\86\D8\A9\00", align 1
@str.const.213 = private unnamed_addr constant [55 x i8] c"    \F0\9F\94\8D \D8\A5\D8\AD\D8\B5\D8\A7\D8\A1 \E2\86\92 \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA (\D9\84\D8\A7 \D8\AD\D9\84\D9\82\D8\A9)\00", align 1
@str.const.217 = private unnamed_addr constant [45 x i8] c"\D8\A5\D8\AD\D8\B5\D8\A7\D8\A1: \D8\B3\D9\84\D8\B3\D9\84\D8\A9 \D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA \D8\A2\D9\85\D9\86\D8\A9\00", align 1
@str.const.218 = private unnamed_addr constant [45 x i8] c"\D8\AA\D8\B1\D8\AA\D9\8A\D8\A8 \D8\A7\D9\84\D8\AA\D8\AB\D8\A8\D9\8A\D8\AA (Topological Sort)\00", align 1
@str.const.219 = private unnamed_addr constant [41 x i8] c"    \D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8\D8\A9:\00", align 1
@str.const.223 = private unnamed_addr constant [23 x i8] c"      \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA: \00", align 1
@str.const.228 = private unnamed_addr constant [19 x i8] c"      \D9\87\D9\86\D8\AF\D8\B3\D8\A9: \00", align 1
@str.const.233 = private unnamed_addr constant [21 x i8] c"      \D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A1: \00", align 1
@str.const.238 = private unnamed_addr constant [19 x i8] c"      \D8\A5\D8\AD\D8\B5\D8\A7\D8\A1: \00", align 1
@str.const.243 = private unnamed_addr constant [15 x i8] c"      \D8\B1\D8\B3\D9\85: \00", align 1
@str.const.248 = private unnamed_addr constant [47 x i8] c"\D8\AA\D8\B1\D8\AA\D9\8A\D8\A8 \D8\A7\D9\84\D8\AA\D8\AB\D8\A8\D9\8A\D8\AA \D8\A7\D9\84\D8\B7\D9\88\D8\A8\D9\88\D9\84\D9\88\D8\AC\D9\8A\00", align 1
@str.const.249 = private unnamed_addr constant [72 x i8] c"    \E2\93\B5 \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA (\D9\84\D8\A7 \D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA \D8\AF\D8\A7\D8\A6\D8\B1\D9\8A\D8\A9 \D8\A8\D8\B9\D8\AF \D8\A7\D9\84\D8\AD\D9\84)\00", align 1
@str.const.253 = private unnamed_addr constant [51 x i8] c"    \E2\93\B6 \D8\B1\D8\B3\D9\85 (\D9\8A\D8\B9\D8\AA\D9\85\D8\AF \D8\B9\D9\84\D9\89: \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA)\00", align 1
@str.const.257 = private unnamed_addr constant [47 x i8] c"    \E2\93\B7 \D9\87\D9\86\D8\AF\D8\B3\D8\A9 (\D9\8A\D8\B9\D8\AA\D9\85\D8\AF \D8\B9\D9\84\D9\89: \D8\B1\D8\B3\D9\85)\00", align 1
@str.const.261 = private unnamed_addr constant [57 x i8] c"    \E2\93\B8 \D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A1 (\D9\8A\D8\B9\D8\AA\D9\85\D8\AF \D8\B9\D9\84\D9\89: \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA)\00", align 1
@str.const.265 = private unnamed_addr constant [55 x i8] c"    \E2\93\B9 \D8\A5\D8\AD\D8\B5\D8\A7\D8\A1 (\D9\8A\D8\B9\D8\AA\D9\85\D8\AF \D8\B9\D9\84\D9\89: \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA)\00", align 1
@str.const.269 = private unnamed_addr constant [39 x i8] c"\D8\AA\D9\88\D9\84\D9\8A\D8\AF \D9\85\D9\84\D9\81 \D8\A7\D9\84\D9\82\D9\81\D9\84 \D8\B5.\D9\82\D9\81\D9\84\00", align 1
@str.const.270 = private unnamed_addr constant [48 x i8] c"  # \D8\B5.\D9\82\D9\81\D9\84 - \D9\85\D9\8F\D9\88\D9\84\D9\8E\D9\91\D8\AF \D8\AA\D9\84\D9\82\D8\A7\D8\A6\D9\8A\D8\A7\D9\8B\00", align 1
@str.const.274 = private unnamed_addr constant [49 x i8] c"  # \D9\84\D8\A7 \D8\AA\D8\B9\D8\AF\D9\84 \D9\87\D8\B0\D8\A7 \D8\A7\D9\84\D9\85\D9\84\D9\81 \D9\8A\D8\AF\D9\88\D9\8A\D8\A7\D9\8B\00", align 1
@str.const.278 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.282 = private unnamed_addr constant [15 x i8] c"  [[\D8\AD\D8\B2\D9\85\D8\A9]]\00", align 1
@str.const.286 = private unnamed_addr constant [11 x i8] c"\D8\A7\D9\84\D8\A7\D8\B3\D9\85\00", align 1
@str.const.288 = private unnamed_addr constant [15 x i8] c"\D8\A7\D9\84\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1\00", align 1
@str.const.289 = private unnamed_addr constant [6 x i8] c"2.1.0\00", align 1
@str.const.290 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1\00", align 1
@str.const.291 = private unnamed_addr constant [10 x i8] c"\D8\B3\D8\AC\D9\84:\D8\B5\00", align 1
@str.const.292 = private unnamed_addr constant [19 x i8] c"    \D8\A7\D9\84\D8\A8\D8\B5\D9\85\D8\A9: \00", align 1
@str.const.297 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.308 = private unnamed_addr constant [6 x i8] c"1.5.3\00", align 1
@str.const.316 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.327 = private unnamed_addr constant [6 x i8] c"3.0.1\00", align 1
@str.const.335 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.346 = private unnamed_addr constant [6 x i8] c"1.2.0\00", align 1
@str.const.348 = private unnamed_addr constant [9 x i8] c"\D9\85\D8\AD\D9\84\D9\8A\00", align 1
@str.const.354 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.365 = private unnamed_addr constant [6 x i8] c"0.8.7\00", align 1
@str.const.367 = private unnamed_addr constant [4 x i8] c"git\00", align 1
@str.const.373 = private unnamed_addr constant [32 x i8] c"\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\B4\D8\B1\D9\88\D8\B9\00", align 1
@str.const.374 = private unnamed_addr constant [24 x i8] c"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A \D8\A7\D9\84\D8\AD\D8\B2\D9\85\00", align 1
@str.const.375 = private unnamed_addr constant [20 x i8] c"\D8\AD\D8\B2\D9\85 \D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9\00", align 1
@str.const.376 = private unnamed_addr constant [27 x i8] c"\D8\AD\D8\B2\D9\85 \D8\BA\D9\8A\D8\B1 \D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9\00", align 1
@str.const.377 = private unnamed_addr constant [41 x i8] c"\D8\A7\D9\84\D8\AD\D8\AC\D9\85 \D8\A7\D9\84\D9\83\D9\84\D9\8A (\D9\83\D9\8A\D9\84\D9\88\D8\A8\D8\A7\D9\8A\D8\AA)\00", align 1
@str.const.378 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.382 = private unnamed_addr constant [27 x i8] c"    \D8\AA\D9\88\D8\B2\D9\8A\D8\B9 \D8\A7\D9\84\D8\AD\D8\AC\D9\85:\00", align 1
@str.const.389 = private unnamed_addr constant [8 x i8] c" \D9\83\D8\A8 (\00", align 1
@str.const.392 = private unnamed_addr constant [3 x i8] c"%)\00", align 1
@str.const.436 = private unnamed_addr constant [169 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.440 = private unnamed_addr constant [96 x i8] c"\E2\95\91  \F0\9F\93\A6 \D9\85\D8\AF\D9\8A\D8\B1 \D8\AD\D8\B2\D9\85 \D8\B5 \D8\A7\D9\84\D8\B4\D8\A7\D9\85\D9\84 - \D8\AD\D9\84 \D8\A7\D9\84\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA \D9\88\D9\83\D8\B4\D9\81 \D8\A7\D9\84\D8\AD\D9\84\D9\82\D8\A7\D8\AA   \E2\95\91\00", align 1
@str.const.444 = private unnamed_addr constant [169 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.448 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.452 = private unnamed_addr constant [61 x i8] c"  \E2\9C\85 \D8\A7\D9\83\D8\AA\D9\85\D9\84 \D9\85\D8\AF\D9\8A\D8\B1 \D8\A7\D9\84\D8\AD\D8\B2\D9\85 \D8\A7\D9\84\D8\B4\D8\A7\D9\85\D9\84 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD\00", align 1

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

define i64 @"\D8\AE\D8\B7_\D9\86\D9\82\D8\A7\D8\B7"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A"(ptr %"\D8\B9") {
entry:
  %"\D8\B91" = alloca ptr, align 8
  store ptr %"\D8\B9", ptr %"\D8\B91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.11)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\AE\D8\B7_\D9\85\D8\B2\D8\AF\D9\88\D8\AC"()
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.15)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\B91")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp2 = call i64 @"\D8\AE\D8\B7_\D9\85\D8\B2\D8\AF\D9\88\D8\AC"()
  ret i64 0
}

define i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D8\B1\D8\B9\D9\8A"(ptr %"\D8\B9") {
entry:
  %"\D8\B91" = alloca ptr, align 8
  store ptr %"\D8\B9", ptr %"\D8\B91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.20)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\AE\D8\B7_\D8\A8\D8\B3\D9\8A\D8\B7"()
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.24)
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
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.29)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\88\D8\B5\D9\811")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.32)
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
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.29)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\88\D8\B5\D9\811")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.32)
  %"%\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\86\D8\AC\D8\A7\D8\AD"(ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.44)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\B4\D9\84"(ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.49)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1"(ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.54)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A9"(ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.59)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A"(i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1") {
entry:
  %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %divtmp = sdiv i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load", 10000
  ret i64 %divtmp
}

define i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D9\81\D8\B1\D8\B9\D9\8A"(i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1") {
entry:
  %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %divtmp = sdiv i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load", 100
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load2" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %calltmp = call i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A"(i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load2")
  %multmp = mul i64 %calltmp, 100
  %subtmp = sub i64 %divtmp, %multmp
  ret i64 %subtmp
}

define i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD"(i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1") {
entry:
  %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %divtmp = sdiv i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load", 100
  %multmp = mul i64 %divtmp, 100
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load2" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %subtmp = sub i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load2", %multmp
  ret i64 %subtmp
}

define i64 @"\D8\AA\D9\83\D9\88\D9\8A\D9\86_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(i64 %"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A", i64 %"\D9\81\D8\B1\D8\B9\D9\8A", i64 %"\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD") {
entry:
  %"\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD3" = alloca i64, align 8
  store i64 %"\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD", ptr %"\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD3", align 8
  %"\D9\81\D8\B1\D8\B9\D9\8A2" = alloca i64, align 8
  store i64 %"\D9\81\D8\B1\D8\B9\D9\8A", ptr %"\D9\81\D8\B1\D8\B9\D9\8A2", align 8
  %"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A1" = alloca i64, align 8
  store i64 %"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A", ptr %"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A1", align 8
  %"%\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A.load" = load i64, ptr %"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A1", align 8
  %multmp = mul i64 %"%\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A.load", 10000
  %"%\D9\81\D8\B1\D8\B9\D9\8A.load" = load i64, ptr %"\D9\81\D8\B1\D8\B9\D9\8A2", align 8
  %multmp4 = mul i64 %"%\D9\81\D8\B1\D8\B9\D9\8A.load", 100
  %addtmp = add i64 %multmp, %multmp4
  %"%\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD.load" = load i64, ptr %"\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD3", align 8
  %addtmp5 = add i64 %addtmp, %"%\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD.load"
  ret i64 %addtmp5
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D9\85\D9\81\D8\B5\D9\84"(ptr %"\D8\AD\D8\B2\D9\85\D8\A9", i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1") {
entry:
  %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12", align 8
  %"\D8\AD\D8\B2\D9\85\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\AD\D8\B2\D9\85\D8\A9", ptr %"\D8\AD\D8\B2\D9\85\D8\A91", align 8
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12", align 8
  %calltmp = call i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D9\81\D8\B1\D8\B9\D9\8A"(i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load")
  %alloca = alloca i64, align 8
  store i64 %calltmp, ptr %alloca, align 8
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load3" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12", align 8
  %calltmp4 = call i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD"(i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load3")
  %alloca5 = alloca i64, align 8
  store i64 %calltmp4, ptr %alloca5, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.29)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\AD\D8\B2\D9\85\D8\A91")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.67)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.69)
  %"%\D9\81.load" = load i64, ptr %alloca, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\81.load")
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.69)
  %"%\D8\AA.load" = load i64, ptr %alloca5, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AA.load")
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.75)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.77)
  %"%\D9\81.load6" = load i64, ptr %alloca, align 8
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\81.load6")
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.80)
  %"%\D8\AA.load7" = load i64, ptr %alloca5, align 8
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AA.load7")
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.83)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\A7\D9\83\D8\A8\D8\B1"(i64 %"\D8\A7", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A71" = alloca i64, align 8
  store i64 %"\D8\A7", ptr %"\D8\A71", align 8
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %"%\D8\A7.load" = load i64, ptr %"\D8\A71", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A7.load", %"%\D8\A8.load"
  br i1 %cmpgttmp, label %then_0, label %merge_1

then_0:                                           ; preds = %entry
  store i64 1, ptr %alloca, align 8
  br label %merge_1

merge_1:                                          ; preds = %then_0, %entry
  %loadtmp = load i64, ptr %alloca, align 8
  ret i64 %loadtmp
}

define i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82"(i64 %"\D9\85\D8\B7\D9\84\D9\88\D8\A8", i64 %"\D9\85\D8\AA\D9\88\D9\81\D8\B1") {
entry:
  %"\D9\85\D8\AA\D9\88\D9\81\D8\B12" = alloca i64, align 8
  store i64 %"\D9\85\D8\AA\D9\88\D9\81\D8\B1", ptr %"\D9\85\D8\AA\D9\88\D9\81\D8\B12", align 8
  %"\D9\85\D8\B7\D9\84\D9\88\D8\A81" = alloca i64, align 8
  store i64 %"\D9\85\D8\B7\D9\84\D9\88\D8\A8", ptr %"\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  %"%\D9\85\D8\B7\D9\84\D9\88\D8\A8.load" = load i64, ptr %"\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  %calltmp = call i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A"(i64 %"%\D9\85\D8\B7\D9\84\D9\88\D8\A8.load")
  %alloca = alloca i64, align 8
  store i64 %calltmp, ptr %alloca, align 8
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load" = load i64, ptr %"\D9\85\D8\AA\D9\88\D9\81\D8\B12", align 8
  %calltmp3 = call i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A"(i64 %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load")
  %alloca4 = alloca i64, align 8
  store i64 %calltmp3, ptr %alloca4, align 8
  %alloca5 = alloca i64, align 8
  store i64 0, ptr %alloca5, align 8
  %"%\D8\B1_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load" = load i64, ptr %alloca, align 8
  %"%\D8\B1_\D9\85\D8\AA\D9\88\D9\81\D8\B1.load" = load i64, ptr %alloca4, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B1_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load", %"%\D8\B1_\D9\85\D8\AA\D9\88\D9\81\D8\B1.load"
  br i1 %cmpeqtmp, label %then_2, label %merge_3

then_2:                                           ; preds = %entry
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load6" = load i64, ptr %"\D9\85\D8\AA\D9\88\D9\81\D8\B12", align 8
  %"%\D9\85\D8\B7\D9\84\D9\88\D8\A8.load7" = load i64, ptr %"\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load6", %"%\D9\85\D8\B7\D9\84\D9\88\D8\A8.load7"
  br i1 %cmpgetmp, label %then_4, label %merge_5

merge_3:                                          ; preds = %merge_5, %entry
  %loadtmp = load i64, ptr %alloca5, align 8
  ret i64 %loadtmp

then_4:                                           ; preds = %then_2
  store i64 1, ptr %alloca5, align 8
  br label %merge_5

merge_5:                                          ; preds = %then_4, %then_2
  br label %merge_3
}

define i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82_\D8\B5\D8\A7\D8\B1\D9\85"(i64 %"\D9\85\D8\B7\D9\84\D9\88\D8\A8", i64 %"\D9\85\D8\AA\D9\88\D9\81\D8\B1") {
entry:
  %"\D9\85\D8\AA\D9\88\D9\81\D8\B12" = alloca i64, align 8
  store i64 %"\D9\85\D8\AA\D9\88\D9\81\D8\B1", ptr %"\D9\85\D8\AA\D9\88\D9\81\D8\B12", align 8
  %"\D9\85\D8\B7\D9\84\D9\88\D8\A81" = alloca i64, align 8
  store i64 %"\D9\85\D8\B7\D9\84\D9\88\D8\A8", ptr %"\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  %"%\D9\85\D8\B7\D9\84\D9\88\D8\A8.load" = load i64, ptr %"\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  %calltmp = call i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A"(i64 %"%\D9\85\D8\B7\D9\84\D9\88\D8\A8.load")
  %alloca = alloca i64, align 8
  store i64 %calltmp, ptr %alloca, align 8
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load" = load i64, ptr %"\D9\85\D8\AA\D9\88\D9\81\D8\B12", align 8
  %calltmp3 = call i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A"(i64 %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load")
  %alloca4 = alloca i64, align 8
  store i64 %calltmp3, ptr %alloca4, align 8
  %"%\D9\85\D8\B7\D9\84\D9\88\D8\A8.load5" = load i64, ptr %"\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  %calltmp6 = call i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D9\81\D8\B1\D8\B9\D9\8A"(i64 %"%\D9\85\D8\B7\D9\84\D9\88\D8\A8.load5")
  %alloca7 = alloca i64, align 8
  store i64 %calltmp6, ptr %alloca7, align 8
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load8" = load i64, ptr %"\D9\85\D8\AA\D9\88\D9\81\D8\B12", align 8
  %calltmp9 = call i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D9\81\D8\B1\D8\B9\D9\8A"(i64 %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load8")
  %alloca10 = alloca i64, align 8
  store i64 %calltmp9, ptr %alloca10, align 8
  %alloca11 = alloca i64, align 8
  store i64 0, ptr %alloca11, align 8
  %"%\D8\B1_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load" = load i64, ptr %alloca, align 8
  %"%\D8\B1_\D9\85\D8\AA\D9\88\D9\81\D8\B1.load" = load i64, ptr %alloca4, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B1_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load", %"%\D8\B1_\D9\85\D8\AA\D9\88\D9\81\D8\B1.load"
  br i1 %cmpeqtmp, label %then_6, label %merge_7

then_6:                                           ; preds = %entry
  %"%\D9\81_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load" = load i64, ptr %alloca7, align 8
  %"%\D9\81_\D9\85\D8\AA\D9\88\D9\81\D8\B1.load" = load i64, ptr %alloca10, align 8
  %cmpeqtmp12 = icmp eq i64 %"%\D9\81_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load", %"%\D9\81_\D9\85\D8\AA\D9\88\D9\81\D8\B1.load"
  br i1 %cmpeqtmp12, label %then_8, label %merge_9

merge_7:                                          ; preds = %merge_9, %entry
  %loadtmp = load i64, ptr %alloca11, align 8
  ret i64 %loadtmp

then_8:                                           ; preds = %then_6
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load13" = load i64, ptr %"\D9\85\D8\AA\D9\88\D9\81\D8\B12", align 8
  %"%\D9\85\D8\B7\D9\84\D9\88\D8\A8.load14" = load i64, ptr %"\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load13", %"%\D9\85\D8\B7\D9\84\D9\88\D8\A8.load14"
  br i1 %cmpgetmp, label %then_10, label %merge_11

merge_9:                                          ; preds = %merge_11, %then_6
  br label %merge_7

then_10:                                          ; preds = %then_8
  store i64 1, ptr %alloca11, align 8
  br label %merge_11

merge_11:                                         ; preds = %then_10, %then_8
  br label %merge_9
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\AA\D9\88\D8\A7\D9\81\D9\82\D8\A7\D8\AA"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A"(ptr @str.const.87)
  %calltmp1 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D8\B1\D8\B9\D9\8A"(ptr @str.const.88)
  %calltmp2 = call i64 @"\D8\AA\D9\83\D9\88\D9\8A\D9\86_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(i64 2, i64 1, i64 0)
  %alloca = alloca i64, align 8
  store i64 %calltmp2, ptr %alloca, align 8
  %calltmp3 = call i64 @"\D8\AA\D9\83\D9\88\D9\8A\D9\86_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(i64 2, i64 3, i64 5)
  %alloca4 = alloca i64, align 8
  store i64 %calltmp3, ptr %alloca4, align 8
  %calltmp5 = call i64 @"\D8\AA\D9\83\D9\88\D9\8A\D9\86_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(i64 3, i64 0, i64 0)
  %alloca6 = alloca i64, align 8
  store i64 %calltmp5, ptr %alloca6, align 8
  %calltmp7 = call i64 @"\D8\AA\D9\83\D9\88\D9\8A\D9\86_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(i64 2, i64 0, i64 9)
  %alloca8 = alloca i64, align 8
  store i64 %calltmp7, ptr %alloca8, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.89)
  %"%\D9\85\D8\B7\D9\84\D9\88\D8\A81.load" = load i64, ptr %alloca, align 8
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B11_\D8\A3.load" = load i64, ptr %alloca4, align 8
  %calltmp9 = call i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82"(i64 %"%\D9\85\D8\B7\D9\84\D9\88\D8\A81.load", i64 %"%\D9\85\D8\AA\D9\88\D9\81\D8\B11_\D8\A3.load")
  %cmpeqtmp = icmp eq i64 %calltmp9, 1
  br i1 %cmpeqtmp, label %then_12, label %else_14

then_12:                                          ; preds = %entry
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.91)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_13

else_14:                                          ; preds = %entry
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.95)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_13

merge_13:                                         ; preds = %else_14, %then_12
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.99)
  %"%\D9\85\D8\B7\D9\84\D9\88\D8\A81.load10" = load i64, ptr %alloca, align 8
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B11_\D8\A8.load" = load i64, ptr %alloca6, align 8
  %calltmp11 = call i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82"(i64 %"%\D9\85\D8\B7\D9\84\D9\88\D8\A81.load10", i64 %"%\D9\85\D8\AA\D9\88\D9\81\D8\B11_\D8\A8.load")
  %cmpeqtmp12 = icmp eq i64 %calltmp11, 1
  br i1 %cmpeqtmp12, label %then_15, label %else_17

then_15:                                          ; preds = %merge_13
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.91)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_16

else_17:                                          ; preds = %merge_13
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.105)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_16

merge_16:                                         ; preds = %else_17, %then_15
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.109)
  %"%\D9\85\D8\B7\D9\84\D9\88\D8\A81.load13" = load i64, ptr %alloca, align 8
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B11_\D8\AC.load" = load i64, ptr %alloca8, align 8
  %calltmp14 = call i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82"(i64 %"%\D9\85\D8\B7\D9\84\D9\88\D8\A81.load13", i64 %"%\D9\85\D8\AA\D9\88\D9\81\D8\B11_\D8\AC.load")
  %cmpeqtmp15 = icmp eq i64 %calltmp14, 1
  br i1 %cmpeqtmp15, label %then_18, label %else_20

then_18:                                          ; preds = %merge_16
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.91)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_19

else_20:                                          ; preds = %merge_16
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.115)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_19

merge_19:                                         ; preds = %else_20, %then_18
  %calltmp16 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D8\B1\D8\B9\D9\8A"(ptr @str.const.119)
  %calltmp17 = call i64 @"\D8\AA\D9\83\D9\88\D9\8A\D9\86_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(i64 1, i64 3, i64 5)
  %alloca18 = alloca i64, align 8
  store i64 %calltmp17, ptr %alloca18, align 8
  %calltmp19 = call i64 @"\D8\AA\D9\83\D9\88\D9\8A\D9\86_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(i64 1, i64 3, i64 8)
  %alloca20 = alloca i64, align 8
  store i64 %calltmp19, ptr %alloca20, align 8
  %calltmp21 = call i64 @"\D8\AA\D9\83\D9\88\D9\8A\D9\86_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(i64 1, i64 4, i64 0)
  %alloca22 = alloca i64, align 8
  store i64 %calltmp21, ptr %alloca22, align 8
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.120)
  %"%\D9\85\D8\B7\D9\84\D9\88\D8\A82.load" = load i64, ptr %alloca18, align 8
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B12_\D8\A3.load" = load i64, ptr %alloca20, align 8
  %calltmp23 = call i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82_\D8\B5\D8\A7\D8\B1\D9\85"(i64 %"%\D9\85\D8\B7\D9\84\D9\88\D8\A82.load", i64 %"%\D9\85\D8\AA\D9\88\D9\81\D8\B12_\D8\A3.load")
  %cmpeqtmp24 = icmp eq i64 %calltmp23, 1
  br i1 %cmpeqtmp24, label %then_21, label %else_23

then_21:                                          ; preds = %merge_19
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.91)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_22

else_23:                                          ; preds = %merge_19
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.95)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_22

merge_22:                                         ; preds = %else_23, %then_21
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.130)
  %"%\D9\85\D8\B7\D9\84\D9\88\D8\A82.load25" = load i64, ptr %alloca18, align 8
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B12_\D8\A8.load" = load i64, ptr %alloca22, align 8
  %calltmp26 = call i64 @"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1_\D9\85\D8\AA\D9\88\D8\A7\D9\81\D9\82_\D8\B5\D8\A7\D8\B1\D9\85"(i64 %"%\D9\85\D8\B7\D9\84\D9\88\D8\A82.load25", i64 %"%\D9\85\D8\AA\D9\88\D9\81\D8\B12_\D8\A8.load")
  %cmpeqtmp27 = icmp eq i64 %calltmp26, 1
  br i1 %cmpeqtmp27, label %then_24, label %else_26

then_24:                                          ; preds = %merge_22
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.91)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_25

else_26:                                          ; preds = %merge_22
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.136)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_25

merge_25:                                         ; preds = %else_26, %then_24
  ret i64 0
}

define ptr @"\D8\A7\D8\B3\D9\85_\D8\AD\D8\B2\D9\85\D8\A9_\D8\B1\D9\82\D9\85"(i64 %"\D8\B1\D9\85\D8\B2") {
entry:
  %"\D8\B1\D9\85\D8\B21" = alloca i64, align 8
  store i64 %"\D8\B1\D9\85\D8\B2", ptr %"\D8\B1\D9\85\D8\B21", align 8
  %alloca = alloca ptr, align 8
  store ptr @str.const.140, ptr %alloca, align 8
  %loadtmp = load i64, ptr %"\D8\B1\D9\85\D8\B21", align 8
  br label %match.case0.test_28

match.end_27:                                     ; preds = %match.case4.body_37, %match.case4.test_36, %match.case3.body_35, %match.case2.body_33, %match.case1.body_31, %match.case0.body_29
  %loadtmp2 = load ptr, ptr %alloca, align 8
  ret ptr %loadtmp2

match.case0.test_28:                              ; preds = %entry
  %cmpeqtmp = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp, label %match.case0.body_29, label %match.case1.test_30

match.case0.body_29:                              ; preds = %match.case0.test_28
  store ptr @str.const.141, ptr %alloca, align 8
  br label %match.end_27

match.case1.test_30:                              ; preds = %match.case0.test_28
  %cmpeqtmp3 = icmp eq i64 %loadtmp, 2
  br i1 %cmpeqtmp3, label %match.case1.body_31, label %match.case2.test_32

match.case1.body_31:                              ; preds = %match.case1.test_30
  store ptr @str.const.142, ptr %alloca, align 8
  br label %match.end_27

match.case2.test_32:                              ; preds = %match.case1.test_30
  %cmpeqtmp4 = icmp eq i64 %loadtmp, 3
  br i1 %cmpeqtmp4, label %match.case2.body_33, label %match.case3.test_34

match.case2.body_33:                              ; preds = %match.case2.test_32
  store ptr @str.const.143, ptr %alloca, align 8
  br label %match.end_27

match.case3.test_34:                              ; preds = %match.case2.test_32
  %cmpeqtmp5 = icmp eq i64 %loadtmp, 4
  br i1 %cmpeqtmp5, label %match.case3.body_35, label %match.case4.test_36

match.case3.body_35:                              ; preds = %match.case3.test_34
  store ptr @str.const.144, ptr %alloca, align 8
  br label %match.end_27

match.case4.test_36:                              ; preds = %match.case3.test_34
  %cmpeqtmp6 = icmp eq i64 %loadtmp, 5
  br i1 %cmpeqtmp6, label %match.case4.body_37, label %match.end_27

match.case4.body_37:                              ; preds = %match.case4.test_36
  store ptr @str.const.145, ptr %alloca, align 8
  br label %match.end_27
}

define i64 @"\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9"(i64 %"\D8\AD\D8\B2\D9\85\D8\A9") {
entry:
  %"\D8\AD\D8\B2\D9\85\D8\A91" = alloca i64, align 8
  store i64 %"\D8\AD\D8\B2\D9\85\D8\A9", ptr %"\D8\AD\D8\B2\D9\85\D8\A91", align 8
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %loadtmp = load i64, ptr %"\D8\AD\D8\B2\D9\85\D8\A91", align 8
  br label %match.case0.test_39

match.end_38:                                     ; preds = %match.case4.body_48, %match.case4.test_47, %match.case3.body_46, %match.case2.body_44, %match.case1.body_42, %match.case0.body_40
  %loadtmp2 = load i64, ptr %alloca, align 8
  ret i64 %loadtmp2

match.case0.test_39:                              ; preds = %entry
  %cmpeqtmp = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp, label %match.case0.body_40, label %match.case1.test_41

match.case0.body_40:                              ; preds = %match.case0.test_39
  store i64 2, ptr %alloca, align 8
  br label %match.end_38

match.case1.test_41:                              ; preds = %match.case0.test_39
  %cmpeqtmp3 = icmp eq i64 %loadtmp, 2
  br i1 %cmpeqtmp3, label %match.case1.body_42, label %match.case2.test_43

match.case1.body_42:                              ; preds = %match.case1.test_41
  store i64 5, ptr %alloca, align 8
  br label %match.end_38

match.case2.test_43:                              ; preds = %match.case1.test_41
  %cmpeqtmp4 = icmp eq i64 %loadtmp, 3
  br i1 %cmpeqtmp4, label %match.case2.body_44, label %match.case3.test_45

match.case2.body_44:                              ; preds = %match.case2.test_43
  store i64 1, ptr %alloca, align 8
  br label %match.end_38

match.case3.test_45:                              ; preds = %match.case2.test_43
  %cmpeqtmp5 = icmp eq i64 %loadtmp, 4
  br i1 %cmpeqtmp5, label %match.case3.body_46, label %match.case4.test_47

match.case3.body_46:                              ; preds = %match.case3.test_45
  store i64 1, ptr %alloca, align 8
  br label %match.end_38

match.case4.test_47:                              ; preds = %match.case3.test_45
  %cmpeqtmp6 = icmp eq i64 %loadtmp, 5
  br i1 %cmpeqtmp6, label %match.case4.body_48, label %match.end_38

match.case4.body_48:                              ; preds = %match.case4.test_47
  store i64 1, ptr %alloca, align 8
  br label %match.end_38
}

define i64 @"\D9\83\D8\B4\D9\81_\D8\AD\D9\84\D9\82\D8\A7\D8\AA"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A"(ptr @str.const.146)
  %calltmp1 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D8\B1\D8\B9\D9\8A"(ptr @str.const.147)
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.148)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.152)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.156)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.160)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.164)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp2 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D8\B1\D8\B9\D9\8A"(ptr @str.const.168)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.169)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.173)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.177)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.181)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.177)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.189)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.177)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.197)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.201)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp3 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\81\D8\B4\D9\84"(ptr @str.const.205)
  %calltmp4 = call i64 @"\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1"(ptr @str.const.206)
  %calltmp5 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D8\B1\D8\B9\D9\8A"(ptr @str.const.207)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.208)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp6 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\86\D8\AC\D8\A7\D8\AD"(ptr @str.const.212)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.213)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp7 = call i64 @"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D9\86\D8\AC\D8\A7\D8\AD"(ptr @str.const.217)
  ret i64 0
}

define i64 @"\D8\A7\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D8\AD\D8\B2\D9\85\D8\A9"(i64 %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA", i64 %"\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D8\B9\D9\83\D8\B3\D9\8A\D8\A9") {
entry:
  %"\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D8\B9\D9\83\D8\B3\D9\8A\D8\A92" = alloca i64, align 8
  store i64 %"\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D8\B9\D9\83\D8\B3\D9\8A\D8\A9", ptr %"\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D8\B9\D9\83\D8\B3\D9\8A\D8\A92", align 8
  %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA1" = alloca i64, align 8
  store i64 %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA", ptr %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA1", align 8
  %"%\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA.load" = load i64, ptr %"\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA1", align 8
  %divtmp = sdiv i64 %"%\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA.load", 100
  %"%\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D8\B9\D9\83\D8\B3\D9\8A\D8\A9.load" = load i64, ptr %"\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D8\B9\D9\83\D8\B3\D9\8A\D8\A92", align 8
  %multmp = mul i64 %"%\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D8\B9\D9\83\D8\B3\D9\8A\D8\A9.load", 10
  %addtmp = add i64 %divtmp, %multmp
  ret i64 %addtmp
}

define i64 @"\D8\AA\D8\B1\D8\AA\D9\8A\D8\A8_\D8\AA\D8\AB\D8\A8\D9\8A\D8\AA"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A"(ptr @str.const.218)
  %calltmp1 = call i64 @"\D8\A7\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D8\AD\D8\B2\D9\85\D8\A9"(i64 5000, i64 4)
  %alloca = alloca i64, align 8
  store i64 %calltmp1, ptr %alloca, align 8
  %calltmp2 = call i64 @"\D8\A7\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D8\AD\D8\B2\D9\85\D8\A9"(i64 2000, i64 1)
  %alloca3 = alloca i64, align 8
  store i64 %calltmp2, ptr %alloca3, align 8
  %calltmp4 = call i64 @"\D8\A7\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D8\AD\D8\B2\D9\85\D8\A9"(i64 3000, i64 0)
  %alloca5 = alloca i64, align 8
  store i64 %calltmp4, ptr %alloca5, align 8
  %calltmp6 = call i64 @"\D8\A7\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D8\AD\D8\B2\D9\85\D8\A9"(i64 1500, i64 0)
  %alloca7 = alloca i64, align 8
  store i64 %calltmp6, ptr %alloca7, align 8
  %calltmp8 = call i64 @"\D8\A7\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D8\AD\D8\B2\D9\85\D8\A9"(i64 800, i64 1)
  %alloca9 = alloca i64, align 8
  store i64 %calltmp8, ptr %alloca9, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.219)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.223)
  %"%\D8\A7\D9\88\D9\84_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\88\D9\84_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.228)
  %"%\D8\A7\D9\88\D9\84_\D9\87\D9\86\D8\AF\D8\B3\D8\A9.load" = load i64, ptr %alloca3, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\88\D9\84_\D9\87\D9\86\D8\AF\D8\B3\D8\A9.load")
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.233)
  %"%\D8\A7\D9\88\D9\84_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A1.load" = load i64, ptr %alloca5, align 8
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\88\D9\84_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A1.load")
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.238)
  %"%\D8\A7\D9\88\D9\84_\D8\A7\D8\AD\D8\B5\D8\A7\D8\A1.load" = load i64, ptr %alloca7, align 8
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\88\D9\84_\D8\A7\D8\AD\D8\B5\D8\A7\D8\A1.load")
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.243)
  %"%\D8\A7\D9\88\D9\84_\D8\B1\D8\B3\D9\85.load" = load i64, ptr %alloca9, align 8
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\88\D9\84_\D8\B1\D8\B3\D9\85.load")
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp10 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D8\B1\D8\B9\D9\8A"(ptr @str.const.248)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.249)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.253)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.257)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.261)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.265)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A8\D8\B5\D9\85\D8\A9_\D8\A8\D8\B3\D9\8A\D8\B7\D8\A9"(i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", i64 %"\D8\AD\D8\AC\D9\85_\D9\85\D9\84\D9\81") {
entry:
  %"\D8\AD\D8\AC\D9\85_\D9\85\D9\84\D9\812" = alloca i64, align 8
  store i64 %"\D8\AD\D8\AC\D9\85_\D9\85\D9\84\D9\81", ptr %"\D8\AD\D8\AC\D9\85_\D9\85\D9\84\D9\812", align 8
  %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %multmp = mul i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load", 31
  %"%\D8\AD\D8\AC\D9\85_\D9\85\D9\84\D9\81.load" = load i64, ptr %"\D8\AD\D8\AC\D9\85_\D9\85\D9\84\D9\812", align 8
  %multmp3 = mul i64 %"%\D8\AD\D8\AC\D9\85_\D9\85\D9\84\D9\81.load", 17
  %addtmp = add i64 %multmp, %multmp3
  %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load4" = load i64, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B11", align 8
  %multmp5 = mul i64 %"%\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1.load4", 31
  %"%\D8\AD\D8\AC\D9\85_\D9\85\D9\84\D9\81.load6" = load i64, ptr %"\D8\AD\D8\AC\D9\85_\D9\85\D9\84\D9\812", align 8
  %multmp7 = mul i64 %"%\D8\AD\D8\AC\D9\85_\D9\85\D9\84\D9\81.load6", 17
  %addtmp8 = add i64 %multmp5, %multmp7
  %divtmp = sdiv i64 %addtmp8, 10000
  %multmp9 = mul i64 %divtmp, 10000
  %subtmp = sub i64 %addtmp, %multmp9
  ret i64 %subtmp
}

define i64 @"\D8\AA\D9\88\D9\84\D9\8A\D8\AF_\D9\82\D9\81\D9\84"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A"(ptr @str.const.269)
  %calltmp1 = call i64 @"\D8\AA\D9\83\D9\88\D9\8A\D9\86_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(i64 2, i64 1, i64 0)
  %calltmp2 = call i64 @"\D8\A8\D8\B5\D9\85\D8\A9_\D8\A8\D8\B3\D9\8A\D8\B7\D8\A9"(i64 %calltmp1, i64 128)
  %alloca = alloca i64, align 8
  store i64 %calltmp2, ptr %alloca, align 8
  %calltmp3 = call i64 @"\D8\AA\D9\83\D9\88\D9\8A\D9\86_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(i64 1, i64 5, i64 3)
  %calltmp4 = call i64 @"\D8\A8\D8\B5\D9\85\D8\A9_\D8\A8\D8\B3\D9\8A\D8\B7\D8\A9"(i64 %calltmp3, i64 67)
  %alloca5 = alloca i64, align 8
  store i64 %calltmp4, ptr %alloca5, align 8
  %calltmp6 = call i64 @"\D8\AA\D9\83\D9\88\D9\8A\D9\86_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(i64 3, i64 0, i64 1)
  %calltmp7 = call i64 @"\D8\A8\D8\B5\D9\85\D8\A9_\D8\A8\D8\B3\D9\8A\D8\B7\D8\A9"(i64 %calltmp6, i64 89)
  %alloca8 = alloca i64, align 8
  store i64 %calltmp7, ptr %alloca8, align 8
  %calltmp9 = call i64 @"\D8\AA\D9\83\D9\88\D9\8A\D9\86_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(i64 1, i64 2, i64 0)
  %calltmp10 = call i64 @"\D8\A8\D8\B5\D9\85\D8\A9_\D8\A8\D8\B3\D9\8A\D8\B7\D8\A9"(i64 %calltmp9, i64 34)
  %alloca11 = alloca i64, align 8
  store i64 %calltmp10, ptr %alloca11, align 8
  %calltmp12 = call i64 @"\D8\AA\D9\83\D9\88\D9\8A\D9\86_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(i64 0, i64 8, i64 7)
  %calltmp13 = call i64 @"\D8\A8\D8\B5\D9\85\D8\A9_\D8\A8\D8\B3\D9\8A\D8\B7\D8\A9"(i64 %calltmp12, i64 23)
  %alloca14 = alloca i64, align 8
  store i64 %calltmp13, ptr %alloca14, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.270)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.274)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.278)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.282)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp15 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.286, ptr @str.const.141)
  %calltmp16 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.288, ptr @str.const.289)
  %calltmp17 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.290, ptr @str.const.291)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.292)
  %"%\D8\A81.load" = load i64, ptr %alloca, align 8
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A81.load")
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.297)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.282)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp18 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.286, ptr @str.const.142)
  %calltmp19 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.288, ptr @str.const.308)
  %calltmp20 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.290, ptr @str.const.291)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.292)
  %"%\D8\A82.load" = load i64, ptr %alloca5, align 8
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A82.load")
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.316)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.282)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp21 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.286, ptr @str.const.143)
  %calltmp22 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.288, ptr @str.const.327)
  %calltmp23 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.290, ptr @str.const.291)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.292)
  %"%\D8\A83.load" = load i64, ptr %alloca8, align 8
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A83.load")
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.335)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.282)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp24 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.286, ptr @str.const.144)
  %calltmp25 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.288, ptr @str.const.346)
  %calltmp26 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.290, ptr @str.const.348)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.292)
  %"%\D8\A84.load" = load i64, ptr %alloca11, align 8
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A84.load")
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.354)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.282)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp27 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.286, ptr @str.const.145)
  %calltmp28 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.288, ptr @str.const.365)
  %calltmp29 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86"(ptr @str.const.290, ptr @str.const.367)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.292)
  %"%\D8\A85.load" = load i64, ptr %alloca14, align 8
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A85.load")
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\AE\D9\85\D8\B3\D8\A9"(i64 %"\D8\A7", i64 %"\D8\A8", i64 %"\D8\AC", i64 %"\D8\AF", i64 %"\D9\87\D9\80") {
entry:
  %"\D9\87\D9\805" = alloca i64, align 8
  store i64 %"\D9\87\D9\80", ptr %"\D9\87\D9\805", align 8
  %"\D8\AF4" = alloca i64, align 8
  store i64 %"\D8\AF", ptr %"\D8\AF4", align 8
  %"\D8\AC3" = alloca i64, align 8
  store i64 %"\D8\AC", ptr %"\D8\AC3", align 8
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A71" = alloca i64, align 8
  store i64 %"\D8\A7", ptr %"\D8\A71", align 8
  %"%\D8\A7.load" = load i64, ptr %"\D8\A71", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %addtmp = add i64 %"%\D8\A7.load", %"%\D8\A8.load"
  %"%\D8\AC.load" = load i64, ptr %"\D8\AC3", align 8
  %addtmp6 = add i64 %addtmp, %"%\D8\AC.load"
  %"%\D8\AF.load" = load i64, ptr %"\D8\AF4", align 8
  %addtmp7 = add i64 %addtmp6, %"%\D8\AF.load"
  %"%\D9\87\D9\80.load" = load i64, ptr %"\D9\87\D9\805", align 8
  %addtmp8 = add i64 %addtmp7, %"%\D9\87\D9\80.load"
  ret i64 %addtmp8
}

define i64 @"\D8\A7\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A"(ptr @str.const.373)
  %alloca = alloca i64, align 8
  store i64 128, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 67, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 89, ptr %alloca2, align 8
  %alloca3 = alloca i64, align 8
  store i64 34, ptr %alloca3, align 8
  %alloca4 = alloca i64, align 8
  store i64 23, ptr %alloca4, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %"%\D8\AD\D8\AC\D9\85_\D9\87\D9\86\D8\AF\D8\B3\D8\A9.load" = load i64, ptr %alloca1, align 8
  %"%\D8\AD\D8\AC\D9\85_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A1.load" = load i64, ptr %alloca2, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D8\AD\D8\B5\D8\A7\D8\A1.load" = load i64, ptr %alloca3, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\B1\D8\B3\D9\85.load" = load i64, ptr %alloca4, align 8
  %calltmp5 = call i64 @"\D9\85\D8\AC\D9\85\D9\88\D8\B9_\D8\AE\D9\85\D8\B3\D8\A9"(i64 %"%\D8\AD\D8\AC\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load", i64 %"%\D8\AD\D8\AC\D9\85_\D9\87\D9\86\D8\AF\D8\B3\D8\A9.load", i64 %"%\D8\AD\D8\AC\D9\85_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A1.load", i64 %"%\D8\AD\D8\AC\D9\85_\D8\A7\D8\AD\D8\B5\D8\A7\D8\A1.load", i64 %"%\D8\AD\D8\AC\D9\85_\D8\B1\D8\B3\D9\85.load")
  %alloca6 = alloca i64, align 8
  store i64 %calltmp5, ptr %alloca6, align 8
  %calltmp7 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.374, i64 5)
  %calltmp8 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.375, i64 3)
  %calltmp9 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.376, i64 2)
  %"%\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\AD\D8\AC\D9\85.load" = load i64, ptr %alloca6, align 8
  %calltmp10 = call i64 @"\D8\A8\D9\8A\D8\A7\D9\86_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.377, i64 %"%\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\AD\D8\AC\D9\85.load")
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.378)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.382)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.223)
  %"%\D8\AD\D8\AC\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load11" = load i64, ptr %alloca, align 8
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AD\D8\AC\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load11")
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.389)
  %"%\D8\AD\D8\AC\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load12" = load i64, ptr %alloca, align 8
  %multmp = mul i64 %"%\D8\AD\D8\AC\D9\85_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load12", 100
  %"%\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\AD\D8\AC\D9\85.load13" = load i64, ptr %alloca6, align 8
  %divtmp = sdiv i64 %multmp, %"%\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\AD\D8\AC\D9\85.load13"
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %divtmp)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.392)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.228)
  %"%\D8\AD\D8\AC\D9\85_\D9\87\D9\86\D8\AF\D8\B3\D8\A9.load14" = load i64, ptr %alloca1, align 8
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AD\D8\AC\D9\85_\D9\87\D9\86\D8\AF\D8\B3\D8\A9.load14")
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.389)
  %"%\D8\AD\D8\AC\D9\85_\D9\87\D9\86\D8\AF\D8\B3\D8\A9.load15" = load i64, ptr %alloca1, align 8
  %multmp16 = mul i64 %"%\D8\AD\D8\AC\D9\85_\D9\87\D9\86\D8\AF\D8\B3\D8\A9.load15", 100
  %"%\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\AD\D8\AC\D9\85.load17" = load i64, ptr %alloca6, align 8
  %divtmp18 = sdiv i64 %multmp16, %"%\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\AD\D8\AC\D9\85.load17"
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %divtmp18)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.392)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.233)
  %"%\D8\AD\D8\AC\D9\85_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A1.load19" = load i64, ptr %alloca2, align 8
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AD\D8\AC\D9\85_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A1.load19")
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.389)
  %"%\D8\AD\D8\AC\D9\85_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A1.load20" = load i64, ptr %alloca2, align 8
  %multmp21 = mul i64 %"%\D8\AD\D8\AC\D9\85_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A1.load20", 100
  %"%\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\AD\D8\AC\D9\85.load22" = load i64, ptr %alloca6, align 8
  %divtmp23 = sdiv i64 %multmp21, %"%\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\AD\D8\AC\D9\85.load22"
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %divtmp23)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.392)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.238)
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D8\AD\D8\B5\D8\A7\D8\A1.load24" = load i64, ptr %alloca3, align 8
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AD\D8\AC\D9\85_\D8\A7\D8\AD\D8\B5\D8\A7\D8\A1.load24")
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.389)
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D8\AD\D8\B5\D8\A7\D8\A1.load25" = load i64, ptr %alloca3, align 8
  %multmp26 = mul i64 %"%\D8\AD\D8\AC\D9\85_\D8\A7\D8\AD\D8\B5\D8\A7\D8\A1.load25", 100
  %"%\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\AD\D8\AC\D9\85.load27" = load i64, ptr %alloca6, align 8
  %divtmp28 = sdiv i64 %multmp26, %"%\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\AD\D8\AC\D9\85.load27"
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %divtmp28)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.392)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.243)
  %"%\D8\AD\D8\AC\D9\85_\D8\B1\D8\B3\D9\85.load29" = load i64, ptr %alloca4, align 8
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\AD\D8\AC\D9\85_\D8\B1\D8\B3\D9\85.load29")
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.389)
  %"%\D8\AD\D8\AC\D9\85_\D8\B1\D8\B3\D9\85.load30" = load i64, ptr %alloca4, align 8
  %multmp31 = mul i64 %"%\D8\AD\D8\AC\D9\85_\D8\B1\D8\B3\D9\85.load30", 100
  %"%\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\AD\D8\AC\D9\85.load32" = load i64, ptr %alloca6, align 8
  %divtmp33 = sdiv i64 %multmp31, %"%\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\AD\D8\AC\D9\85.load32"
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %divtmp33)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.392)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.436)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.440)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.444)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D9\81\D8\AD\D8\B5_\D8\AA\D9\88\D8\A7\D9\81\D9\82\D8\A7\D8\AA"()
  %calltmp1 = call i64 @"\D9\83\D8\B4\D9\81_\D8\AD\D9\84\D9\82\D8\A7\D8\AA"()
  %calltmp2 = call i64 @"\D8\AA\D8\B1\D8\AA\D9\8A\D8\A8_\D8\AA\D8\AB\D8\A8\D9\8A\D8\AA"()
  %calltmp3 = call i64 @"\D8\AA\D9\88\D9\84\D9\8A\D8\AF_\D9\82\D9\81\D9\84"()
  %calltmp4 = call i64 @"\D8\A7\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA"()
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.448)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp5 = call i64 @"\D8\AE\D8\B7_\D9\85\D8\B2\D8\AF\D9\88\D8\AC"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.452)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp6 = call i64 @"\D8\AE\D8\B7_\D9\85\D8\B2\D8\AF\D9\88\D8\AC"()
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
