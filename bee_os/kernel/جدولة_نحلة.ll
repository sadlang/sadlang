; ModuleID = 'جدولة_نحلة'
source_filename = "\D8\AC\D8\AF\D9\88\D9\84\D8\A9_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%SadArray = type { i64, i64, ptr }

@str.const = private unnamed_addr constant [55 x i8] c"[SCHED] \D8\AE\D8\B7\D8\A3: \D8\AC\D8\AF\D9\88\D9\84 \D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA \D9\85\D9\85\D8\AA\D9\84\D8\A6!\0A\00", align 1
@str.const.1 = private unnamed_addr constant [41 x i8] c"[SCHED] \F0\9F\90\9D \D8\B9\D9\85\D9\84\D9\8A\D8\A9 \D8\AC\D8\AF\D9\8A\D8\AF\D8\A9: PID=\00", align 1
@str.const.2 = private unnamed_addr constant [15 x i8] c" \D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9=\00", align 1
@str.const.3 = private unnamed_addr constant [11 x i8] c" \D9\81\D9\87\D8\B1\D8\B3=\00", align 1
@str.const.4 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.5 = private unnamed_addr constant [40 x i8] c"[SCHED] \E2\9B\94 \D8\A5\D9\86\D9\87\D8\A7\D8\A1 \D8\B9\D9\85\D9\84\D9\8A\D8\A9: PID=\00", align 1
@str.const.6 = private unnamed_addr constant [22 x i8] c" \D9\83\D9\88\D8\AF_\D8\A7\D9\84\D8\AE\D8\B1\D9\88\D8\AC=\00", align 1
@str.const.8 = private unnamed_addr constant [36 x i8] c"[SCHED] \E2\9B\94 \D9\82\D8\AA\D9\84 \D8\B9\D9\85\D9\84\D9\8A\D8\A9: PID=\00", align 1
@str.const.10 = private unnamed_addr constant [35 x i8] c"[SCHED] \F0\9F\A7\B9 \D8\AA\D9\86\D8\B8\D9\8A\D9\81 \D8\AE\D8\A7\D9\86\D8\A9: \00", align 1
@str.const.12 = private unnamed_addr constant [33 x i8] c"[SCHED] \F0\9F\98\B4 \D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A9 PID=\00", align 1
@str.const.13 = private unnamed_addr constant [22 x i8] c" \D9\86\D8\A7\D8\A6\D9\85\D8\A9 \D9\84\D9\85\D8\AF\D8\A9 \00", align 1
@str.const.14 = private unnamed_addr constant [11 x i8] c" \D9\86\D8\A8\D8\B6\D8\A9\0A\00", align 1
@str.const.15 = private unnamed_addr constant [33 x i8] c"[SCHED] \F0\9F\92\A4 \D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A9 PID=\00", align 1
@str.const.16 = private unnamed_addr constant [17 x i8] c" \D8\AA\D9\86\D8\AA\D8\B8\D8\B1 I/O\0A\00", align 1
@str.const.17 = private unnamed_addr constant [29 x i8] c"[SCHED] \E2\8F\B0 \D8\A5\D9\8A\D9\82\D8\A7\D8\B8: PID=\00", align 1
@str.const.19 = private unnamed_addr constant [46 x i8] c"[SCHED] \E2\8F\B0 \D8\A7\D8\B3\D8\AA\D9\8A\D9\82\D8\A7\D8\B8 \D8\AA\D9\84\D9\82\D8\A7\D8\A6\D9\8A: PID=\00", align 1
@str.const.21 = private unnamed_addr constant [29 x i8] c"[SCHED] \E2\9D\84 \D8\AA\D8\AC\D9\85\D9\8A\D8\AF: PID=\00", align 1
@str.const.23 = private unnamed_addr constant [49 x i8] c"[SCHED] \E2\9D\84 \D8\AA\D8\AC\D9\85\D9\8A\D8\AF (\D9\83\D8\A7\D9\86\D8\AA \D8\AA\D8\B9\D9\85\D9\84): PID=\00", align 1
@str.const.25 = private unnamed_addr constant [33 x i8] c"[SCHED] \E2\96\B6 \D8\A7\D8\B3\D8\AA\D8\A6\D9\86\D8\A7\D9\81: PID=\00", align 1
@str.const.27 = private unnamed_addr constant [21 x i8] c"[SCHED] \D8\AA\D8\A8\D8\AF\D9\8A\D9\84 #\00", align 1
@str.const.28 = private unnamed_addr constant [7 x i8] c": PID \00", align 1
@str.const.29 = private unnamed_addr constant [10 x i8] c" \E2\86\92 PID \00", align 1
@str.const.31 = private unnamed_addr constant [59 x i8] c"[SCHED] \F0\9F\90\9D \D8\B9\D9\85\D9\84\D9\8A\D8\A9 \D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84 (PID 0) \D9\85\D9\8F\D9\86\D8\B4\D8\A3\D8\A9\0A\00", align 1
@str.const.33 = private unnamed_addr constant [155 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\0A\00", align 1
@str.const.34 = private unnamed_addr constant [67 x i8] c"  \F0\9F\90\9D \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D9\85\D9\8F\D8\AC\D8\AF\D9\88\D9\90\D9\84 \D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85 \E2\80\94 BeeOS Scheduler\0A\00", align 1
@str.const.36 = private unnamed_addr constant [49 x i8] c"  [1/4] \D8\AA\D9\86\D8\B8\D9\8A\D9\81 \D8\AC\D8\AF\D9\88\D9\84 \D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA...\0A\00", align 1
@str.const.37 = private unnamed_addr constant [13 x i8] c"        \E2\9C\93 \00", align 1
@str.const.38 = private unnamed_addr constant [26 x i8] c" \D8\AE\D8\A7\D9\86\D8\A9 \D9\85\D9\8F\D9\86\D8\B8\D9\91\D9\81\D8\A9\0A\00", align 1
@str.const.39 = private unnamed_addr constant [55 x i8] c"  [2/4] \D8\A5\D9\86\D8\B4\D8\A7\D8\A1 \D8\B9\D9\85\D9\84\D9\8A\D8\A9 \D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84 (PID 0)...\0A\00", align 1
@str.const.40 = private unnamed_addr constant [48 x i8] c"        \E2\9C\93 \D8\B9\D9\85\D9\84\D9\8A\D8\A9 \D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84 \D8\AC\D8\A7\D9\87\D8\B2\D8\A9\0A\00", align 1
@str.const.41 = private unnamed_addr constant [57 x i8] c"  [3/4] \D8\AA\D8\B9\D9\8A\D9\8A\D9\86 \D9\85\D8\AA\D8\BA\D9\8A\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D9\8F\D8\AC\D8\AF\D9\88\D9\90\D9\84...\0A\00", align 1
@str.const.42 = private unnamed_addr constant [50 x i8] c"        \E2\9C\93 \D8\A7\D9\84\D8\B4\D8\B1\D9\8A\D8\AD\D8\A9 \D8\A7\D9\84\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A\D8\A9: \00", align 1
@str.const.44 = private unnamed_addr constant [42 x i8] c"  [4/4] \D8\AA\D9\81\D8\B9\D9\8A\D9\84 \D8\A7\D9\84\D9\85\D9\8F\D8\AC\D8\AF\D9\88\D9\90\D9\84...\0A\00", align 1
@str.const.45 = private unnamed_addr constant [40 x i8] c"        \E2\9C\93 \D8\A7\D9\84\D9\85\D9\8F\D8\AC\D8\AF\D9\88\D9\90\D9\84 \D9\86\D8\B4\D8\B7!\0A\00", align 1
@str.const.48 = private unnamed_addr constant [52 x i8] c"  \E2\9C\93 \D8\A7\D9\84\D9\85\D9\8F\D8\AC\D8\AF\D9\88\D9\90\D9\84 \D8\AC\D8\A7\D9\87\D8\B2 \E2\80\94 Round Robin, \00", align 1
@str.const.49 = private unnamed_addr constant [11 x i8] c" \D8\AE\D8\A7\D9\86\D8\A9\0A\00", align 1
@str.const.52 = private unnamed_addr constant [12 x i8] c"\D9\81\D8\A7\D8\B1\D8\BA\D8\A9 \00", align 1
@str.const.53 = private unnamed_addr constant [12 x i8] c"\D8\AC\D8\A7\D9\87\D8\B2\D8\A9 \00", align 1
@str.const.54 = private unnamed_addr constant [11 x i8] c"\D8\AA\D8\B9\D9\85\D9\84  \00", align 1
@str.const.55 = private unnamed_addr constant [12 x i8] c"\D9\86\D8\A7\D8\A6\D9\85\D8\A9 \00", align 1
@str.const.56 = private unnamed_addr constant [13 x i8] c"\D9\85\D9\8F\D8\AC\D9\85\D9\91\D8\AF\00", align 1
@str.const.57 = private unnamed_addr constant [13 x i8] c"\D9\85\D9\86\D8\AA\D9\87\D9\8A\D8\A9\00", align 1
@str.const.58 = private unnamed_addr constant [12 x i8] c"\D8\B2\D9\88\D9\85\D8\A8\D9\8A \00", align 1
@str.const.60 = private unnamed_addr constant [197 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\0A\00", align 1
@str.const.61 = private unnamed_addr constant [86 x i8] c"\E2\95\91  \F0\9F\93\8B \D8\AC\D8\AF\D9\88\D9\84 \D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA \E2\80\94 BeeOS Process Table                      \E2\95\91\0A\00", align 1
@str.const.62 = private unnamed_addr constant [197 x i8] c"\E2\95\A0\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\A3\0A\00", align 1
@str.const.63 = private unnamed_addr constant [109 x i8] c"\E2\95\91  PID  \E2\94\82 \D8\A7\D9\84\D8\AD\D8\A7\D9\84\D8\A9  \E2\94\82 \D8\A7\D9\84\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9 \E2\94\82 CPU (\D9\86\D8\A8\D8\B6\D8\A9) \E2\94\82 \D8\AA\D8\A8\D8\AF\D9\8A\D9\84\D8\A7\D8\AA \E2\94\82 \D8\A7\D9\84\D9\86\D9\88\D9\85   \E2\95\91\0A\00", align 1
@str.const.66 = private unnamed_addr constant [22 x i8] c"\E2\95\91  \D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9: \00", align 1
@str.const.67 = private unnamed_addr constant [31 x i8] c" \D8\B9\D9\85\D9\84\D9\8A\D8\A9 | \D8\AA\D8\A8\D8\AF\D9\8A\D9\84\D8\A7\D8\AA: \00", align 1
@str.const.68 = private unnamed_addr constant [24 x i8] c" | \D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9: PID \00", align 1
@str.const.69 = private unnamed_addr constant [10 x i8] c"     \E2\95\91\0A\00", align 1
@str.const.70 = private unnamed_addr constant [197 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\0A\00", align 1
@str.const.72 = private unnamed_addr constant [6 x i8] c"\E2\95\91  \00", align 1
@str.const.73 = private unnamed_addr constant [10 x i8] c"     \E2\94\82 \00", align 1
@str.const.74 = private unnamed_addr constant [6 x i8] c" \E2\94\82 \00", align 1
@str.const.75 = private unnamed_addr constant [13 x i8] c"        \E2\94\82 \00", align 1
@str.const.76 = private unnamed_addr constant [15 x i8] c"          \E2\94\82 \00", align 1
@str.const.77 = private unnamed_addr constant [12 x i8] c"       \E2\94\82 \00", align 1
@str.const.78 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\A4\D9\82\D8\AA  \00", align 1
@str.const.79 = private unnamed_addr constant [7 x i8] c"I/O   \00", align 1
@str.const.80 = private unnamed_addr constant [10 x i8] c"\D9\82\D9\81\D9\84   \00", align 1
@str.const.81 = private unnamed_addr constant [9 x i8] c"\E2\80\94     \00", align 1
@str.const.82 = private unnamed_addr constant [6 x i8] c" \E2\95\91\0A\00", align 1
@str.const.84 = private unnamed_addr constant [152 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\0A\00", align 1
@str.const.85 = private unnamed_addr constant [75 x i8] c"\E2\95\91  \F0\9F\93\8A \D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D9\8F\D8\AC\D8\AF\D9\88\D9\90\D9\84 \E2\80\94 BeeOS Scheduler     \E2\95\91\0A\00", align 1
@str.const.86 = private unnamed_addr constant [152 x i8] c"\E2\95\A0\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\A3\0A\00", align 1
@str.const.87 = private unnamed_addr constant [65 x i8] c"\E2\95\91  \D8\A7\D9\84\D8\AE\D9\88\D8\A7\D8\B1\D8\B2\D9\85\D9\8A\D8\A9:        Round Robin               \E2\95\91\0A\00", align 1
@str.const.88 = private unnamed_addr constant [39 x i8] c"\E2\95\91  \D8\A7\D9\84\D8\B4\D8\B1\D9\8A\D8\AD\D8\A9 \D8\A7\D9\84\D8\B2\D9\85\D9\86\D9\8A\D8\A9:   \00", align 1
@str.const.89 = private unnamed_addr constant [10 x i8] c" \D9\86\D8\A8\D8\B6\D8\A9\00", align 1
@str.const.90 = private unnamed_addr constant [27 x i8] c"                      \E2\95\91\0A\00", align 1
@str.const.91 = private unnamed_addr constant [36 x i8] c"\E2\95\91  \D8\B9\D8\AF\D8\AF \D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA:      \00", align 1
@str.const.92 = private unnamed_addr constant [34 x i8] c"                             \E2\95\91\0A\00", align 1
@str.const.93 = private unnamed_addr constant [38 x i8] c"\E2\95\91  \D8\AA\D8\A8\D8\AF\D9\8A\D9\84\D8\A7\D8\AA \D8\A7\D9\84\D8\B3\D9\8A\D8\A7\D9\82:    \00", align 1
@str.const.95 = private unnamed_addr constant [43 x i8] c"\E2\95\91  \D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A9 \D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9:   PID \00", align 1
@str.const.96 = private unnamed_addr constant [31 x i8] c"                          \E2\95\91\0A\00", align 1
@str.const.97 = private unnamed_addr constant [41 x i8] c"\E2\95\91  \D8\A2\D8\AE\D8\B1 \D8\AC\D8\AF\D9\88\D9\84\D8\A9:        \D9\86\D8\A8\D8\B6\D8\A9 \00", align 1
@str.const.98 = private unnamed_addr constant [30 x i8] c"                         \E2\95\91\0A\00", align 1
@str.const.99 = private unnamed_addr constant [18 x i8] c"\E2\95\91  \D8\AC\D8\A7\D9\87\D8\B2\D8\A9: \00", align 1
@str.const.100 = private unnamed_addr constant [16 x i8] c" | \D9\86\D8\A7\D8\A6\D9\85\D8\A9: \00", align 1
@str.const.101 = private unnamed_addr constant [20 x i8] c" | \D9\85\D9\8F\D8\AC\D9\85\D9\91\D8\AF\D8\A9: \00", align 1
@str.const.102 = private unnamed_addr constant [15 x i8] c"          \E2\95\91\0A\00", align 1
@str.const.103 = private unnamed_addr constant [152 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\0A\00", align 1
@str.const.107 = private unnamed_addr constant [55 x i8] c"  \F0\9F\A7\AA \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\A7\D9\84\D9\85\D9\8F\D8\AC\D8\AF\D9\88\D9\90\D9\84 \E2\80\94 Test Suite\0A\00", align 1
@str.const.109 = private unnamed_addr constant [47 x i8] c"\0A\F0\9F\93\9D \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 1: \D8\A5\D9\86\D8\B4\D8\A7\D8\A1 \D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA\0A\00", align 1
@str.const.110 = private unnamed_addr constant [40 x i8] c"  \E2\9C\93 \D8\A3\D9\8F\D9\86\D8\B4\D8\A6\D8\AA 3 \D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA: PID \00", align 1
@str.const.111 = private unnamed_addr constant [3 x i8] c", \00", align 1
@str.const.114 = private unnamed_addr constant [56 x i8] c"\0A\F0\9F\93\9D \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 2: \D8\B9\D8\B1\D8\B6 \D8\AC\D8\AF\D9\88\D9\84 \D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA\0A\00", align 1
@str.const.115 = private unnamed_addr constant [51 x i8] c"\0A\F0\9F\93\9D \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 3: \D8\AA\D8\AC\D9\85\D9\8A\D8\AF \D9\88\D8\A7\D8\B3\D8\AA\D8\A6\D9\86\D8\A7\D9\81\0A\00", align 1
@str.const.116 = private unnamed_addr constant [20 x i8] c"  \E2\86\92 \D8\AD\D8\A7\D9\84\D8\A9 PID \00", align 1
@str.const.117 = private unnamed_addr constant [25 x i8] c" \D8\A8\D8\B9\D8\AF \D8\A7\D9\84\D8\AA\D8\AC\D9\85\D9\8A\D8\AF: \00", align 1
@str.const.120 = private unnamed_addr constant [29 x i8] c" \D8\A8\D8\B9\D8\AF \D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\A6\D9\86\D8\A7\D9\81: \00", align 1
@str.const.122 = private unnamed_addr constant [45 x i8] c"\0A\F0\9F\93\9D \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 4: \D8\A5\D9\86\D9\87\D8\A7\D8\A1 \D8\B9\D9\85\D9\84\D9\8A\D8\A9\0A\00", align 1
@str.const.123 = private unnamed_addr constant [26 x i8] c"  \E2\9C\93 \D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A9 PID \00", align 1
@str.const.124 = private unnamed_addr constant [13 x i8] c" \D8\A7\D9\86\D8\AA\D9\87\D8\AA\0A\00", align 1
@str.const.125 = private unnamed_addr constant [58 x i8] c"\0A\F0\9F\93\9D \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 5: \D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\84 \D8\A8\D8\B9\D8\AF \D8\A7\D9\84\D8\AA\D9\86\D8\B8\D9\8A\D9\81\0A\00", align 1
@str.const.126 = private unnamed_addr constant [59 x i8] c"\0A\F0\9F\93\9D \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 6: \D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D9\8F\D8\AC\D8\AF\D9\88\D9\90\D9\84\0A\00", align 1
@str.const.128 = private unnamed_addr constant [60 x i8] c"  \E2\9C\93 \D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA \D8\A7\D9\86\D8\AA\D9\87\D8\AA \D8\A8\D9\86\D8\AC\D8\A7\D8\AD!\0A\00", align 1
@str.const.129 = private unnamed_addr constant [156 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\0A\0A\00", align 1

declare void @sad_ll_interrupt_disable()

declare void @sad_ll_interrupt_enable()

declare void @sad_ll_timer_get_ticks()

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_context_switch(i64, i64)

declare void @sad_ll_stack_init(i64, i64)

define void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_0

while_body_1:                                     ; preds = %while_cond_0
  %"%\D8\B7.load2" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load2", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_0

while_cond_0:                                     ; preds = %while_body_1, %entry
  %arr.ptr.int = load i64, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 0
  %arr.len = load i64, ptr %arr.len.gep, align 8
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", %arr.len
  br i1 %cmplttmp, label %while_body_1, label %while_exit_2

while_exit_2:                                     ; preds = %while_cond_0
  ret void
}

define void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85" = alloca i64, align 8
  %"\D9\82\D9\8A\D9\85\D8\A91" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_3, label %merge_4

merge_4:                                          ; preds = %entry
  %"%\D9\82\D9\8A\D9\85\D8\A9.load2" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load2", 0
  br i1 %cmplttmp, label %then_5, label %merge_6

merge_6:                                          ; preds = %then_5, %merge_4
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 20, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 20, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 20))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep3 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data4 = load ptr, ptr %arr.data.gep3, align 8
  %arr.elem = getelementptr i64, ptr %arr.data4, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data.gep5 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data6 = load ptr, ptr %arr.data.gep5, align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 1
  store i64 0, ptr %arr.elem7, align 8
  %arr.data.gep8 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data9 = load ptr, ptr %arr.data.gep8, align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 2
  store i64 0, ptr %arr.elem10, align 8
  %arr.data.gep11 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data12 = load ptr, ptr %arr.data.gep11, align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 3
  store i64 0, ptr %arr.elem13, align 8
  %arr.data.gep14 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data15 = load ptr, ptr %arr.data.gep14, align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 4
  store i64 0, ptr %arr.elem16, align 8
  %arr.data.gep17 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data18 = load ptr, ptr %arr.data.gep17, align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 5
  store i64 0, ptr %arr.elem19, align 8
  %arr.data.gep20 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data21 = load ptr, ptr %arr.data.gep20, align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 6
  store i64 0, ptr %arr.elem22, align 8
  %arr.data.gep23 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data24 = load ptr, ptr %arr.data.gep23, align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 7
  store i64 0, ptr %arr.elem25, align 8
  %arr.data.gep26 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data27 = load ptr, ptr %arr.data.gep26, align 8
  %arr.elem28 = getelementptr i64, ptr %arr.data27, i64 8
  store i64 0, ptr %arr.elem28, align 8
  %arr.data.gep29 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data30 = load ptr, ptr %arr.data.gep29, align 8
  %arr.elem31 = getelementptr i64, ptr %arr.data30, i64 9
  store i64 0, ptr %arr.elem31, align 8
  %arr.data.gep32 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data33 = load ptr, ptr %arr.data.gep32, align 8
  %arr.elem34 = getelementptr i64, ptr %arr.data33, i64 10
  store i64 0, ptr %arr.elem34, align 8
  %arr.data.gep35 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data36 = load ptr, ptr %arr.data.gep35, align 8
  %arr.elem37 = getelementptr i64, ptr %arr.data36, i64 11
  store i64 0, ptr %arr.elem37, align 8
  %arr.data.gep38 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data39 = load ptr, ptr %arr.data.gep38, align 8
  %arr.elem40 = getelementptr i64, ptr %arr.data39, i64 12
  store i64 0, ptr %arr.elem40, align 8
  %arr.data.gep41 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data42 = load ptr, ptr %arr.data.gep41, align 8
  %arr.elem43 = getelementptr i64, ptr %arr.data42, i64 13
  store i64 0, ptr %arr.elem43, align 8
  %arr.data.gep44 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data45 = load ptr, ptr %arr.data.gep44, align 8
  %arr.elem46 = getelementptr i64, ptr %arr.data45, i64 14
  store i64 0, ptr %arr.elem46, align 8
  %arr.data.gep47 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data48 = load ptr, ptr %arr.data.gep47, align 8
  %arr.elem49 = getelementptr i64, ptr %arr.data48, i64 15
  store i64 0, ptr %arr.elem49, align 8
  %arr.data.gep50 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data51 = load ptr, ptr %arr.data.gep50, align 8
  %arr.elem52 = getelementptr i64, ptr %arr.data51, i64 16
  store i64 0, ptr %arr.elem52, align 8
  %arr.data.gep53 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data54 = load ptr, ptr %arr.data.gep53, align 8
  %arr.elem55 = getelementptr i64, ptr %arr.data54, i64 17
  store i64 0, ptr %arr.elem55, align 8
  %arr.data.gep56 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data57 = load ptr, ptr %arr.data.gep56, align 8
  %arr.elem58 = getelementptr i64, ptr %arr.data57, i64 18
  store i64 0, ptr %arr.elem58, align 8
  %arr.data.gep59 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data60 = load ptr, ptr %arr.data.gep59, align 8
  %arr.elem61 = getelementptr i64, ptr %arr.data60, i64 19
  store i64 0, ptr %arr.elem61, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  br label %while_cond_7

then_3:                                           ; preds = %entry
  ret void

then_5:                                           ; preds = %merge_4
  br label %merge_6

while_body_11:                                    ; preds = %while_cond_10
  %"%\D8\B7.load70" = load i64, ptr %"%\D8\B7", align 8
  %subtmp71 = sub i64 %"%\D8\B7.load70", 1
  store i64 %subtmp71, ptr %"%\D8\B7", align 8
  br label %while_cond_10

while_body_8:                                     ; preds = %while_cond_7
  %"%\D9\82\D9\8A\D9\85\D8\A9.load63" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %modtmp = srem i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load63", 10
  %addtmp = add i64 %modtmp, 48
  %"%\D8\B9\D8\AF\D8\AF.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %arr.ptr.int = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.data.gep64 = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 2
  %arr.data65 = load ptr, ptr %arr.data.gep64, align 8
  %arr.elem66 = getelementptr i64, ptr %arr.data65, i64 %"%\D8\B9\D8\AF\D8\AF.load"
  store i64 %addtmp, ptr %arr.elem66, align 8
  %"%\D8\B9\D8\AF\D8\AF.load67" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %addtmp68 = add i64 %"%\D8\B9\D8\AF\D8\AF.load67", 1
  store i64 %addtmp68, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  br label %while_cond_7

while_cond_10:                                    ; preds = %while_body_11, %while_exit_9
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B7.load", 0
  br i1 %cmpgetmp, label %while_body_11, label %while_exit_12

while_cond_7:                                     ; preds = %while_body_8, %merge_6
  %"%\D9\82\D9\8A\D9\85\D8\A9.load62" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load62", 0
  br i1 %cmpgttmp, label %while_body_8, label %while_exit_9

while_exit_12:                                    ; preds = %while_cond_10
  ret void

while_exit_9:                                     ; preds = %while_cond_7
  %"%\D8\B9\D8\AF\D8\AF.load69" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %subtmp = sub i64 %"%\D8\B9\D8\AF\D8\AF.load69", 1
  store i64 %subtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_10
}

define i64 @"\D8\A7\D8\A8\D8\AD\D8\AB_\D8\B9\D9\86_\D8\AE\D8\A7\D9\86\D8\A9_\D9\81\D8\A7\D8\B1\D8\BA\D8\A9"() {
entry:
  %"%\D8\B7" = alloca i64, align 8
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_13

merge_17:                                         ; preds = %while_body_14
  %"%\D8\B7.load2" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load2", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_13

then_16:                                          ; preds = %while_body_14
  %loadtmp = load i64, ptr %"%\D8\B7", align 8
  ret i64 %loadtmp

while_body_14:                                    ; preds = %while_cond_13
  %"%\D8\B7.load1" = load i64, ptr %"%\D8\B7", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B7.load1"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_16, label %merge_17

while_cond_13:                                    ; preds = %merge_17, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", 0
  br i1 %cmplttmp, label %while_body_14, label %while_exit_15

while_exit_15:                                    ; preds = %while_cond_13
  ret i64 -1
}

define i64 @"\D9\88\D9\84\D8\AF_PID"() {
entry:
  ret i64 0
}

define i64 @"\D8\A3\D9\86\D8\B4\D8\A6_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(i64 %"\D9\86\D9\82\D8\B7\D8\A9_\D8\A7\D9\84\D8\AF\D8\AE\D9\88\D9\84", i64 %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9") {
entry:
  %"%pid" = alloca i64, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A92" = alloca i64, align 8
  store i64 %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9", ptr %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A92", align 8
  %"\D9\86\D9\82\D8\B7\D8\A9_\D8\A7\D9\84\D8\AF\D8\AE\D9\88\D9\841" = alloca i64, align 8
  store i64 %"\D9\86\D9\82\D8\B7\D8\A9_\D8\A7\D9\84\D8\AF\D8\AE\D9\88\D9\84", ptr %"\D9\86\D9\82\D8\B7\D8\A9_\D8\A7\D9\84\D8\AF\D8\AE\D9\88\D9\841", align 8
  call void @sad_ll_interrupt_disable()
  %calltmp = call i64 @"\D8\A7\D8\A8\D8\AD\D8\AB_\D8\B9\D9\86_\D8\AE\D8\A7\D9\86\D8\A9_\D9\81\D8\A7\D8\B1\D8\BA\D8\A9"()
  store i64 %calltmp, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 0
  br i1 %cmplttmp, label %then_18, label %merge_19

merge_19:                                         ; preds = %entry
  %calltmp3 = call i64 @"\D9\88\D9\84\D8\AF_PID"()
  store i64 %calltmp3, ptr %"%pid", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load4" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%pid.load" = load i64, ptr %"%pid", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load4"
  store i64 %"%pid.load", ptr %arr.elem, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load5" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load5"
  store i64 0, ptr %arr.elem7, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load8" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load" = load i64, ptr %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A92", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load8"
  store i64 %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load", ptr %arr.elem10, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load11" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load11"
  store i64 0, ptr %arr.elem13, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load14" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load14"
  store i64 0, ptr %arr.elem16, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load17" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load17"
  store i64 0, ptr %arr.elem19, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load20" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data21 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load20"
  store i64 0, ptr %arr.elem22, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load23" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data24 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load23"
  store i64 0, ptr %arr.elem25, align 8
  %arr.data26 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem27 = getelementptr i64, ptr %arr.data26, i64 0
  %arr.get = load i64, ptr %arr.elem27, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load28" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data29 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem30 = getelementptr i64, ptr %arr.data29, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load28"
  store i64 %arr.get, ptr %arr.elem30, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %"%pid.load31" = load i64, ptr %"%pid", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%pid.load31")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load32" = load i64, ptr %"\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A92", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9.load32")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %"%\D9\81\D9\87\D8\B1\D8\B3.load33" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load33")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  call void @sad_ll_interrupt_enable()
  %loadtmp = load i64, ptr %"%pid", align 8
  ret i64 %loadtmp

then_18:                                          ; preds = %entry
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  call void @sad_ll_interrupt_enable()
  ret i64 -1
}

define void @"\D8\A3\D9\86\D9\87_\D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9"(i64 %"\D9\83\D9\88\D8\AF_\D8\A7\D9\84\D8\AE\D8\B1\D9\88\D8\AC") {
entry:
  %"\D9\83\D9\88\D8\AF_\D8\A7\D9\84\D8\AE\D8\B1\D9\88\D8\AC1" = alloca i64, align 8
  store i64 %"\D9\83\D9\88\D8\AF_\D8\A7\D9\84\D8\AE\D8\B1\D9\88\D8\AC", ptr %"\D9\83\D9\88\D8\AF_\D8\A7\D9\84\D8\AE\D8\B1\D9\88\D8\AC1", align 8
  call void @sad_ll_interrupt_disable()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  %"%\D9\83\D9\88\D8\AF_\D8\A7\D9\84\D8\AE\D8\B1\D9\88\D8\AC.load" = load i64, ptr %"\D9\83\D9\88\D8\AF_\D8\A7\D9\84\D8\AE\D8\B1\D9\88\D8\AC1", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\83\D9\88\D8\AF_\D8\A7\D9\84\D8\AE\D8\B1\D9\88\D8\AC.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %arr.data2 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem3 = getelementptr i64, ptr %arr.data2, i64 0
  store i64 0, ptr %arr.elem3, align 8
  call void @sad_ll_interrupt_enable()
  call void @"\D8\AC\D8\AF\D9\88\D9\84"()
  ret void
}

define i64 @"\D8\A3\D9\86\D9\87_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(i64 %pid) {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %pid1 = alloca i64, align 8
  store i64 %pid, ptr %pid1, align 8
  call void @sad_ll_interrupt_disable()
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_20

merge_24:                                         ; preds = %merge_26, %while_body_21
  %"%\D8\B7.load7" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load7", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_20

merge_26:                                         ; preds = %then_23
  br label %merge_24

then_23:                                          ; preds = %while_body_21
  %"%\D8\B7.load3" = load i64, ptr %"%\D8\B7", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D8\B7.load3"
  %arr.get6 = load i64, ptr %arr.elem5, align 8
  %cmpnetmp = icmp ne i64 %arr.get6, 0
  br i1 %cmpnetmp, label %then_25, label %merge_26

then_25:                                          ; preds = %then_23
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %"%pid.load8" = load i64, ptr %pid1, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%pid.load8")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%\D8\B7.load9" = load i64, ptr %"%\D8\B7", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D8\B7.load9"
  store i64 0, ptr %arr.elem11, align 8
  call void @sad_ll_interrupt_enable()
  ret i64 1

while_body_21:                                    ; preds = %while_cond_20
  %"%\D8\B7.load2" = load i64, ptr %"%\D8\B7", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B7.load2"
  %arr.get = load i64, ptr %arr.elem, align 8
  %"%pid.load" = load i64, ptr %pid1, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, %"%pid.load"
  br i1 %cmpeqtmp, label %then_23, label %merge_24

while_cond_20:                                    ; preds = %merge_24, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", 0
  br i1 %cmplttmp, label %while_body_21, label %while_exit_22

while_exit_22:                                    ; preds = %while_cond_20
  call void @sad_ll_interrupt_enable()
  ret i64 0
}

define void @"\D9\86\D8\B8\D9\81_\D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D9\86\D8\AA\D9\87\D9\8A\D8\A9"() {
entry:
  %"%\D8\B7" = alloca i64, align 8
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_27

merge_31:                                         ; preds = %then_30, %while_body_28
  %"%\D8\B7.load18" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load18", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_27

then_30:                                          ; preds = %while_body_28
  %"%\D8\B7.load2" = load i64, ptr %"%\D8\B7", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D8\B7.load2"
  store i64 0, ptr %arr.elem4, align 8
  %"%\D8\B7.load5" = load i64, ptr %"%\D8\B7", align 8
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 %"%\D8\B7.load5"
  store i64 0, ptr %arr.elem7, align 8
  %"%\D8\B7.load8" = load i64, ptr %"%\D8\B7", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D8\B7.load8"
  store i64 0, ptr %arr.elem10, align 8
  %"%\D8\B7.load11" = load i64, ptr %"%\D8\B7", align 8
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 %"%\D8\B7.load11"
  store i64 0, ptr %arr.elem13, align 8
  %"%\D8\B7.load14" = load i64, ptr %"%\D8\B7", align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 %"%\D8\B7.load14"
  store i64 0, ptr %arr.elem16, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %"%\D8\B7.load17" = load i64, ptr %"%\D8\B7", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D8\B7.load17")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  br label %merge_31

while_body_28:                                    ; preds = %while_cond_27
  %"%\D8\B7.load1" = load i64, ptr %"%\D8\B7", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B7.load1"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_30, label %merge_31

while_cond_27:                                    ; preds = %merge_31, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", 0
  br i1 %cmplttmp, label %while_body_28, label %while_exit_29

while_exit_29:                                    ; preds = %while_cond_27
  ret void
}

define void @"\D9\86\D9\85"(i64 %"\D9\85\D8\AF\D8\A9") {
entry:
  %"%\D8\A7\D9\84\D9\86\D8\A8\D8\B6\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"\D9\85\D8\AF\D8\A91" = alloca i64, align 8
  store i64 %"\D9\85\D8\AF\D8\A9", ptr %"\D9\85\D8\AF\D8\A91", align 8
  call void @sad_ll_interrupt_disable()
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%\D8\A7\D9\84\D9\86\D8\A8\D8\B6\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data2 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem3 = getelementptr i64, ptr %arr.data2, i64 0
  store i64 0, ptr %arr.elem3, align 8
  %"%\D8\A7\D9\84\D9\86\D8\A8\D8\B6\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load" = load i64, ptr %"%\D8\A7\D9\84\D9\86\D8\A8\D8\B6\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %"%\D9\85\D8\AF\D8\A9.load" = load i64, ptr %"\D9\85\D8\AF\D8\A91", align 8
  %addtmp = add i64 %"%\D8\A7\D9\84\D9\86\D8\A8\D8\B6\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load", %"%\D9\85\D8\AF\D8\A9.load"
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 0
  store i64 %addtmp, ptr %arr.elem5, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 0
  %arr.get = load i64, ptr %arr.elem7, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %"%\D9\85\D8\AF\D8\A9.load8" = load i64, ptr %"\D9\85\D8\AF\D8\A91", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\85\D8\AF\D8\A9.load8")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  call void @sad_ll_interrupt_enable()
  call void @"\D8\AC\D8\AF\D9\88\D9\84"()
  ret void
}

define void @"\D9\86\D9\85_\D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1_IO"() {
entry:
  call void @sad_ll_interrupt_disable()
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data1 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem2 = getelementptr i64, ptr %arr.data1, i64 0
  store i64 0, ptr %arr.elem2, align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 0
  store i64 0, ptr %arr.elem4, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 0
  %arr.get = load i64, ptr %arr.elem6, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  call void @sad_ll_interrupt_enable()
  call void @"\D8\AC\D8\AF\D9\88\D9\84"()
  ret void
}

define i64 @"\D8\A3\D9\8A\D9\82\D8\B8_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(i64 %pid) {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %pid1 = alloca i64, align 8
  store i64 %pid, ptr %pid1, align 8
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_32

merge_36:                                         ; preds = %merge_38, %while_body_33
  %"%\D8\B7.load8" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load8", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_32

merge_38:                                         ; preds = %then_35
  br label %merge_36

then_35:                                          ; preds = %while_body_33
  %"%\D8\B7.load3" = load i64, ptr %"%\D8\B7", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D8\B7.load3"
  %arr.get6 = load i64, ptr %arr.elem5, align 8
  %cmpeqtmp7 = icmp eq i64 %arr.get6, 0
  br i1 %cmpeqtmp7, label %then_37, label %merge_38

then_37:                                          ; preds = %then_35
  %"%\D8\B7.load9" = load i64, ptr %"%\D8\B7", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D8\B7.load9"
  store i64 0, ptr %arr.elem11, align 8
  %"%\D8\B7.load12" = load i64, ptr %"%\D8\B7", align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 %"%\D8\B7.load12"
  store i64 0, ptr %arr.elem14, align 8
  %"%\D8\B7.load15" = load i64, ptr %"%\D8\B7", align 8
  %arr.data16 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 %"%\D8\B7.load15"
  store i64 0, ptr %arr.elem17, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %"%pid.load18" = load i64, ptr %pid1, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%pid.load18")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  ret i64 1

while_body_33:                                    ; preds = %while_cond_32
  %"%\D8\B7.load2" = load i64, ptr %"%\D8\B7", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B7.load2"
  %arr.get = load i64, ptr %arr.elem, align 8
  %"%pid.load" = load i64, ptr %pid1, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, %"%pid.load"
  br i1 %cmpeqtmp, label %then_35, label %merge_36

while_cond_32:                                    ; preds = %merge_36, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", 0
  br i1 %cmplttmp, label %while_body_33, label %while_exit_34

while_exit_34:                                    ; preds = %while_cond_32
  ret i64 0
}

define void @"\D9\81\D8\AD\D8\B5_\D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\A7\D9\84\D9\86\D8\A7\D8\A6\D9\85\D8\A9"() {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %"%\D8\A7\D9\84\D9\86\D8\A8\D8\B6\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%\D8\A7\D9\84\D9\86\D8\A8\D8\B6\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_39

merge_43:                                         ; preds = %merge_45, %while_body_40
  %"%\D8\B7.load7" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load7", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_39

merge_45:                                         ; preds = %merge_47, %then_42
  br label %merge_43

merge_47:                                         ; preds = %then_46, %then_44
  br label %merge_45

then_42:                                          ; preds = %while_body_40
  %"%\D8\B7.load2" = load i64, ptr %"%\D8\B7", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D8\B7.load2"
  %arr.get5 = load i64, ptr %arr.elem4, align 8
  %cmpeqtmp6 = icmp eq i64 %arr.get5, 0
  br i1 %cmpeqtmp6, label %then_44, label %merge_45

then_44:                                          ; preds = %then_42
  %"%\D8\B7.load8" = load i64, ptr %"%\D8\B7", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D8\B7.load8"
  %arr.get11 = load i64, ptr %arr.elem10, align 8
  %"%\D8\A7\D9\84\D9\86\D8\A8\D8\B6\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load" = load i64, ptr %"%\D8\A7\D9\84\D9\86\D8\A8\D8\B6\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\A7\D9\84\D9\86\D8\A8\D8\B6\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load", %arr.get11
  br i1 %cmpgetmp, label %then_46, label %merge_47

then_46:                                          ; preds = %then_44
  %"%\D8\B7.load12" = load i64, ptr %"%\D8\B7", align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 %"%\D8\B7.load12"
  store i64 0, ptr %arr.elem14, align 8
  %"%\D8\B7.load15" = load i64, ptr %"%\D8\B7", align 8
  %arr.data16 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 %"%\D8\B7.load15"
  store i64 0, ptr %arr.elem17, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  %"%\D8\B7.load18" = load i64, ptr %"%\D8\B7", align 8
  %arr.data19 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 %"%\D8\B7.load18"
  %arr.get21 = load i64, ptr %arr.elem20, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get21)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  br label %merge_47

while_body_40:                                    ; preds = %while_cond_39
  %"%\D8\B7.load1" = load i64, ptr %"%\D8\B7", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B7.load1"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_42, label %merge_43

while_cond_39:                                    ; preds = %merge_43, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", 0
  br i1 %cmplttmp, label %while_body_40, label %while_exit_41

while_exit_41:                                    ; preds = %while_cond_39
  ret void
}

define i64 @"\D8\AC\D9\85\D8\AF_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(i64 %pid) {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %pid1 = alloca i64, align 8
  store i64 %pid, ptr %pid1, align 8
  call void @sad_ll_interrupt_disable()
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_48

merge_52:                                         ; preds = %merge_56, %while_body_49
  %"%\D8\B7.load8" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load8", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_48

merge_54:                                         ; preds = %then_51
  %"%\D8\B7.load13" = load i64, ptr %"%\D8\B7", align 8
  %arr.data14 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 %"%\D8\B7.load13"
  %arr.get16 = load i64, ptr %arr.elem15, align 8
  %cmpeqtmp17 = icmp eq i64 %arr.get16, 0
  br i1 %cmpeqtmp17, label %then_55, label %merge_56

merge_56:                                         ; preds = %merge_54
  br label %merge_52

merge_58:                                         ; preds = %then_57, %then_55
  ret i64 1

then_51:                                          ; preds = %while_body_49
  %"%\D8\B7.load3" = load i64, ptr %"%\D8\B7", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D8\B7.load3"
  %arr.get6 = load i64, ptr %arr.elem5, align 8
  %cmpeqtmp7 = icmp eq i64 %arr.get6, 0
  br i1 %cmpeqtmp7, label %then_53, label %merge_54

then_53:                                          ; preds = %then_51
  %"%\D8\B7.load9" = load i64, ptr %"%\D8\B7", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D8\B7.load9"
  store i64 0, ptr %arr.elem11, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  %"%pid.load12" = load i64, ptr %pid1, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%pid.load12")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  call void @sad_ll_interrupt_enable()
  ret i64 1

then_55:                                          ; preds = %merge_54
  %"%\D8\B7.load18" = load i64, ptr %"%\D8\B7", align 8
  %arr.data19 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 %"%\D8\B7.load18"
  store i64 0, ptr %arr.elem20, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  %"%pid.load21" = load i64, ptr %pid1, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%pid.load21")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  call void @sad_ll_interrupt_enable()
  %"%\D8\B7.load22" = load i64, ptr %"%\D8\B7", align 8
  %cmpeqtmp23 = icmp eq i64 %"%\D8\B7.load22", 0
  br i1 %cmpeqtmp23, label %then_57, label %merge_58

then_57:                                          ; preds = %then_55
  call void @"\D8\AC\D8\AF\D9\88\D9\84"()
  br label %merge_58

while_body_49:                                    ; preds = %while_cond_48
  %"%\D8\B7.load2" = load i64, ptr %"%\D8\B7", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B7.load2"
  %arr.get = load i64, ptr %arr.elem, align 8
  %"%pid.load" = load i64, ptr %pid1, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, %"%pid.load"
  br i1 %cmpeqtmp, label %then_51, label %merge_52

while_cond_48:                                    ; preds = %merge_52, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", 0
  br i1 %cmplttmp, label %while_body_49, label %while_exit_50

while_exit_50:                                    ; preds = %while_cond_48
  call void @sad_ll_interrupt_enable()
  ret i64 0
}

define i64 @"\D8\A7\D8\B3\D8\AA\D8\A3\D9\86\D9\81_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(i64 %pid) {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %pid1 = alloca i64, align 8
  store i64 %pid, ptr %pid1, align 8
  call void @sad_ll_interrupt_disable()
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_59

merge_63:                                         ; preds = %merge_65, %while_body_60
  %"%\D8\B7.load8" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load8", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_59

merge_65:                                         ; preds = %then_62
  br label %merge_63

then_62:                                          ; preds = %while_body_60
  %"%\D8\B7.load3" = load i64, ptr %"%\D8\B7", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D8\B7.load3"
  %arr.get6 = load i64, ptr %arr.elem5, align 8
  %cmpeqtmp7 = icmp eq i64 %arr.get6, 0
  br i1 %cmpeqtmp7, label %then_64, label %merge_65

then_64:                                          ; preds = %then_62
  %"%\D8\B7.load9" = load i64, ptr %"%\D8\B7", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D8\B7.load9"
  store i64 0, ptr %arr.elem11, align 8
  %"%\D8\B7.load12" = load i64, ptr %"%\D8\B7", align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 %"%\D8\B7.load12"
  store i64 0, ptr %arr.elem14, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  %"%pid.load15" = load i64, ptr %pid1, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%pid.load15")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  call void @sad_ll_interrupt_enable()
  ret i64 1

while_body_60:                                    ; preds = %while_cond_59
  %"%\D8\B7.load2" = load i64, ptr %"%\D8\B7", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B7.load2"
  %arr.get = load i64, ptr %arr.elem, align 8
  %"%pid.load" = load i64, ptr %pid1, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, %"%pid.load"
  br i1 %cmpeqtmp, label %then_62, label %merge_63

while_cond_59:                                    ; preds = %merge_63, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", 0
  br i1 %cmplttmp, label %while_body_60, label %while_exit_61

while_exit_61:                                    ; preds = %while_cond_59
  call void @sad_ll_interrupt_enable()
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\B1_\D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9"() {
entry:
  %"%\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"%\D8\B7" = alloca i64, align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9" = alloca i64, align 8
  store i64 1, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", align 8
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_66

merge_70:                                         ; preds = %while_body_67
  %"%\D8\B7.load6" = load i64, ptr %"%\D8\B7", align 8
  %addtmp7 = add i64 %"%\D8\B7.load6", 1
  store i64 %addtmp7, ptr %"%\D8\B7", align 8
  br label %while_cond_66

merge_72:                                         ; preds = %while_exit_68
  ret i64 0

then_69:                                          ; preds = %while_body_67
  %loadtmp = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  ret i64 %loadtmp

then_71:                                          ; preds = %while_exit_68
  ret i64 0

while_body_67:                                    ; preds = %while_cond_66
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load" = load i64, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", align 8
  %"%\D8\B7.load1" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load", %"%\D8\B7.load1"
  %modtmp = srem i64 %addtmp, 0
  store i64 %modtmp, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_69, label %merge_70

while_cond_66:                                    ; preds = %merge_70, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", 0
  br i1 %cmplttmp, label %while_body_67, label %while_exit_68

while_exit_68:                                    ; preds = %while_cond_66
  %arr.data2 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem3 = getelementptr i64, ptr %arr.data2, i64 0
  %arr.get4 = load i64, ptr %arr.elem3, align 8
  %cmpeqtmp5 = icmp eq i64 %arr.get4, 0
  br i1 %cmpeqtmp5, label %then_71, label %merge_72
}

define void @"\D9\86\D8\A8\D8\B6\D8\A9_\D8\A7\D9\84\D9\85\D8\AC\D8\AF\D9\88\D9\84"() {
entry:
  br i1 true, label %then_73, label %merge_74

merge_74:                                         ; preds = %entry
  call void @"\D9\81\D8\AD\D8\B5_\D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\A7\D9\84\D9\86\D8\A7\D8\A6\D9\85\D8\A9"()
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_75, label %merge_76

merge_76:                                         ; preds = %then_75, %merge_74
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 0
  %arr.get8 = load i64, ptr %arr.elem7, align 8
  %subtmp = sub i64 %arr.get8, 1
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 0
  store i64 %subtmp, ptr %arr.elem10, align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 0
  %arr.get13 = load i64, ptr %arr.elem12, align 8
  %cmpletmp = icmp sle i64 %arr.get13, 0
  br i1 %cmpletmp, label %then_77, label %merge_78

merge_78:                                         ; preds = %merge_80, %merge_76
  ret void

merge_80:                                         ; preds = %then_79, %then_77
  call void @"\D8\AC\D8\AF\D9\88\D9\84"()
  br label %merge_78

then_73:                                          ; preds = %entry
  ret void

then_75:                                          ; preds = %merge_74
  %arr.data1 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem2 = getelementptr i64, ptr %arr.data1, i64 0
  %arr.get3 = load i64, ptr %arr.elem2, align 8
  %addtmp = add i64 %arr.get3, 1
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 0
  store i64 %addtmp, ptr %arr.elem5, align 8
  br label %merge_76

then_77:                                          ; preds = %merge_76
  %arr.data14 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 0
  %arr.get16 = load i64, ptr %arr.elem15, align 8
  %cmpeqtmp17 = icmp eq i64 %arr.get16, 0
  br i1 %cmpeqtmp17, label %then_79, label %merge_80

then_79:                                          ; preds = %then_77
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 0
  store i64 0, ptr %arr.elem19, align 8
  %arr.data20 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem21 = getelementptr i64, ptr %arr.data20, i64 0
  store i64 0, ptr %arr.elem21, align 8
  br label %merge_80
}

define void @"\D8\AC\D8\AF\D9\88\D9\84"() {
entry:
  %"%\D8\A7\D9\84\D8\B3\D8\A7\D8\A8\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %calltmp = call i64 @"\D8\A7\D8\AE\D8\AA\D8\B1_\D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9"()
  store i64 %calltmp, ptr %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load" = load i64, ptr %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_81, label %merge_82

merge_82:                                         ; preds = %entry
  %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load2" = load i64, ptr %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load2"
  %arr.get5 = load i64, ptr %arr.elem4, align 8
  %addtmp = add i64 %arr.get5, 1
  %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load6" = load i64, ptr %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load6"
  store i64 %addtmp, ptr %arr.elem8, align 8
  %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load9" = load i64, ptr %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load9"
  store i64 0, ptr %arr.elem11, align 8
  %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load12" = load i64, ptr %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load12"
  store i64 0, ptr %arr.elem14, align 8
  br i1 true, label %then_85, label %merge_86

merge_84:                                         ; preds = %then_83, %then_81
  ret void

merge_86:                                         ; preds = %then_85, %merge_82
  ret void

then_81:                                          ; preds = %entry
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp1 = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp1, label %then_83, label %merge_84

then_83:                                          ; preds = %then_81
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 0
  store i64 0, ptr %arr.elem16, align 8
  br label %merge_84

then_85:                                          ; preds = %merge_82
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  %"%\D8\A7\D9\84\D8\B3\D8\A7\D8\A8\D9\82\D8\A9.load" = load i64, ptr %"%\D8\A7\D9\84\D8\B3\D8\A7\D8\A8\D9\82\D8\A9", align 8
  %arr.data17 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem18 = getelementptr i64, ptr %arr.data17, i64 %"%\D8\A7\D9\84\D8\B3\D8\A7\D8\A8\D9\82\D8\A9.load"
  %arr.get19 = load i64, ptr %arr.elem18, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get19)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load20" = load i64, ptr %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %arr.data21 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 %"%\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load20"
  %arr.get23 = load i64, ptr %arr.elem22, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get23)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  br label %merge_86
}

define void @"\D8\A3\D9\86\D8\B4\D8\A6_\D8\B9\D9\85\D9\84\D9\8A\D8\A9_\D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84"() {
entry:
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data1 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem2 = getelementptr i64, ptr %arr.data1, i64 0
  store i64 0, ptr %arr.elem2, align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 0
  store i64 0, ptr %arr.elem4, align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 0
  store i64 0, ptr %arr.elem6, align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 0
  store i64 0, ptr %arr.elem8, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  ret void
}

define void @"\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D9\85\D8\AC\D8\AF\D9\88\D9\84"() {
entry:
  %"%\D8\B7" = alloca i64, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.34 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.36 to i64))
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_87

while_body_88:                                    ; preds = %while_cond_87
  %"%\D8\B7.load1" = load i64, ptr %"%\D8\B7", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B7.load1"
  store i64 0, ptr %arr.elem, align 8
  %"%\D8\B7.load2" = load i64, ptr %"%\D8\B7", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D8\B7.load2"
  store i64 0, ptr %arr.elem4, align 8
  %"%\D8\B7.load5" = load i64, ptr %"%\D8\B7", align 8
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 %"%\D8\B7.load5"
  store i64 0, ptr %arr.elem7, align 8
  %"%\D8\B7.load8" = load i64, ptr %"%\D8\B7", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D8\B7.load8"
  store i64 0, ptr %arr.elem10, align 8
  %"%\D8\B7.load11" = load i64, ptr %"%\D8\B7", align 8
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 %"%\D8\B7.load11"
  store i64 0, ptr %arr.elem13, align 8
  %"%\D8\B7.load14" = load i64, ptr %"%\D8\B7", align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 %"%\D8\B7.load14"
  store i64 0, ptr %arr.elem16, align 8
  %"%\D8\B7.load17" = load i64, ptr %"%\D8\B7", align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 %"%\D8\B7.load17"
  store i64 0, ptr %arr.elem19, align 8
  %"%\D8\B7.load20" = load i64, ptr %"%\D8\B7", align 8
  %arr.data21 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 %"%\D8\B7.load20"
  store i64 0, ptr %arr.elem22, align 8
  %"%\D8\B7.load23" = load i64, ptr %"%\D8\B7", align 8
  %arr.data24 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 %"%\D8\B7.load23"
  store i64 0, ptr %arr.elem25, align 8
  %"%\D8\B7.load26" = load i64, ptr %"%\D8\B7", align 8
  %arr.data27 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem28 = getelementptr i64, ptr %arr.data27, i64 %"%\D8\B7.load26"
  store i64 0, ptr %arr.elem28, align 8
  %"%\D8\B7.load29" = load i64, ptr %"%\D8\B7", align 8
  %arr.data30 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem31 = getelementptr i64, ptr %arr.data30, i64 %"%\D8\B7.load29"
  store i64 0, ptr %arr.elem31, align 8
  %"%\D8\B7.load32" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load32", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_87

while_cond_87:                                    ; preds = %while_body_88, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", 0
  br i1 %cmplttmp, label %while_body_88, label %while_exit_89

while_exit_89:                                    ; preds = %while_cond_87
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.37 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.38 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.39 to i64))
  call void @"\D8\A3\D9\86\D8\B4\D8\A6_\D8\B9\D9\85\D9\84\D9\8A\D8\A9_\D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84"()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.40 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.41 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.42 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.44 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.45 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.48 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.49 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  ret void
}

define i64 @"\D8\A3\D8\B9\D8\B7\D9\86\D9\8A_PID_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A"() {
entry:
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  ret i64 %arr.get
}

define i64 @"\D8\A3\D8\B9\D8\B7\D9\86\D9\8A_\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA"() {
entry:
  ret i64 0
}

define i64 @"\D8\A3\D8\B9\D8\B7\D9\86\D9\8A_\D8\AA\D8\A8\D8\AF\D9\8A\D9\84\D8\A7\D8\AA_\D8\A7\D9\84\D8\B3\D9\8A\D8\A7\D9\82"() {
entry:
  ret i64 0
}

define i64 @"\D8\A3\D8\B9\D8\B7\D9\86\D9\8A_\D8\AD\D8\A7\D9\84\D8\A9_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(i64 %pid) {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %pid1 = alloca i64, align 8
  store i64 %pid, ptr %pid1, align 8
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_90

merge_94:                                         ; preds = %while_body_91
  %"%\D8\B7.load7" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load7", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_90

then_93:                                          ; preds = %while_body_91
  %"%\D8\B7.load3" = load i64, ptr %"%\D8\B7", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D8\B7.load3"
  %arr.get6 = load i64, ptr %arr.elem5, align 8
  ret i64 %arr.get6

while_body_91:                                    ; preds = %while_cond_90
  %"%\D8\B7.load2" = load i64, ptr %"%\D8\B7", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B7.load2"
  %arr.get = load i64, ptr %arr.elem, align 8
  %"%pid.load" = load i64, ptr %pid1, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, %"%pid.load"
  br i1 %cmpeqtmp, label %then_93, label %merge_94

while_cond_90:                                    ; preds = %merge_94, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", 0
  br i1 %cmplttmp, label %while_body_91, label %while_exit_92

while_exit_92:                                    ; preds = %while_cond_90
  ret i64 -1
}

define void @"\D8\A7\D8\B3\D9\85_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D8\A9"(i64 %"\D8\AD\D8\A7\D9\84\D8\A9") {
entry:
  %"\D8\AD\D8\A7\D9\84\D8\A91" = alloca i64, align 8
  store i64 %"\D8\AD\D8\A7\D9\84\D8\A9", ptr %"\D8\AD\D8\A7\D9\84\D8\A91", align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9.load" = load i64, ptr %"\D8\AD\D8\A7\D9\84\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AD\D8\A7\D9\84\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_95, label %merge_96

merge_100:                                        ; preds = %then_99, %merge_98
  %"%\D8\AD\D8\A7\D9\84\D8\A9.load6" = load i64, ptr %"\D8\AD\D8\A7\D9\84\D8\A91", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D8\AD\D8\A7\D9\84\D8\A9.load6", 0
  br i1 %cmpeqtmp7, label %then_101, label %merge_102

merge_102:                                        ; preds = %then_101, %merge_100
  %"%\D8\AD\D8\A7\D9\84\D8\A9.load8" = load i64, ptr %"\D8\AD\D8\A7\D9\84\D8\A91", align 8
  %cmpeqtmp9 = icmp eq i64 %"%\D8\AD\D8\A7\D9\84\D8\A9.load8", 0
  br i1 %cmpeqtmp9, label %then_103, label %merge_104

merge_104:                                        ; preds = %then_103, %merge_102
  %"%\D8\AD\D8\A7\D9\84\D8\A9.load10" = load i64, ptr %"\D8\AD\D8\A7\D9\84\D8\A91", align 8
  %cmpeqtmp11 = icmp eq i64 %"%\D8\AD\D8\A7\D9\84\D8\A9.load10", 0
  br i1 %cmpeqtmp11, label %then_105, label %merge_106

merge_106:                                        ; preds = %then_105, %merge_104
  %"%\D8\AD\D8\A7\D9\84\D8\A9.load12" = load i64, ptr %"\D8\AD\D8\A7\D9\84\D8\A91", align 8
  %cmpeqtmp13 = icmp eq i64 %"%\D8\AD\D8\A7\D9\84\D8\A9.load12", 0
  br i1 %cmpeqtmp13, label %then_107, label %merge_108

merge_108:                                        ; preds = %then_107, %merge_106
  ret void

merge_96:                                         ; preds = %then_95, %entry
  %"%\D8\AD\D8\A7\D9\84\D8\A9.load2" = load i64, ptr %"\D8\AD\D8\A7\D9\84\D8\A91", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D8\AD\D8\A7\D9\84\D8\A9.load2", 0
  br i1 %cmpeqtmp3, label %then_97, label %merge_98

merge_98:                                         ; preds = %then_97, %merge_96
  %"%\D8\AD\D8\A7\D9\84\D8\A9.load4" = load i64, ptr %"\D8\AD\D8\A7\D9\84\D8\A91", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D8\AD\D8\A7\D9\84\D8\A9.load4", 0
  br i1 %cmpeqtmp5, label %then_99, label %merge_100

then_101:                                         ; preds = %merge_100
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.55 to i64))
  br label %merge_102

then_103:                                         ; preds = %merge_102
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.56 to i64))
  br label %merge_104

then_105:                                         ; preds = %merge_104
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.57 to i64))
  br label %merge_106

then_107:                                         ; preds = %merge_106
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.58 to i64))
  br label %merge_108

then_95:                                          ; preds = %entry
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.52 to i64))
  br label %merge_96

then_97:                                          ; preds = %merge_96
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.53 to i64))
  br label %merge_98

then_99:                                          ; preds = %merge_98
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.54 to i64))
  br label %merge_100
}

define void @"\D8\A7\D8\B9\D8\B1\D8\B6_\D8\AC\D8\AF\D9\88\D9\84_\D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA"() {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %"%\D8\B9\D8\B1\D8\B6\D8\AA" = alloca i64, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.60 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.61 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.62 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.63 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.62 to i64))
  store i64 0, ptr %"%\D8\B9\D8\B1\D8\B6\D8\AA", align 8
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_109

merge_113:                                        ; preds = %merge_121, %while_body_110
  %"%\D8\B7.load29" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load29", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_109

merge_115:                                        ; preds = %then_114, %then_112
  %"%\D8\B7.load30" = load i64, ptr %"%\D8\B7", align 8
  %arr.data31 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem32 = getelementptr i64, ptr %arr.data31, i64 %"%\D8\B7.load30"
  %arr.get33 = load i64, ptr %arr.elem32, align 8
  %cmpeqtmp34 = icmp eq i64 %arr.get33, 0
  br i1 %cmpeqtmp34, label %then_116, label %merge_117

merge_117:                                        ; preds = %then_116, %merge_115
  %"%\D8\B7.load35" = load i64, ptr %"%\D8\B7", align 8
  %arr.data36 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem37 = getelementptr i64, ptr %arr.data36, i64 %"%\D8\B7.load35"
  %arr.get38 = load i64, ptr %arr.elem37, align 8
  %cmpeqtmp39 = icmp eq i64 %arr.get38, 0
  br i1 %cmpeqtmp39, label %then_118, label %merge_119

merge_119:                                        ; preds = %then_118, %merge_117
  %"%\D8\B7.load40" = load i64, ptr %"%\D8\B7", align 8
  %arr.data41 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem42 = getelementptr i64, ptr %arr.data41, i64 %"%\D8\B7.load40"
  %arr.get43 = load i64, ptr %arr.elem42, align 8
  %cmpeqtmp44 = icmp eq i64 %arr.get43, 0
  br i1 %cmpeqtmp44, label %then_120, label %merge_121

merge_121:                                        ; preds = %then_120, %merge_119
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.82 to i64))
  %"%\D8\B9\D8\B1\D8\B6\D8\AA.load45" = load i64, ptr %"%\D8\B9\D8\B1\D8\B6\D8\AA", align 8
  %addtmp46 = add i64 %"%\D8\B9\D8\B1\D8\B6\D8\AA.load45", 1
  store i64 %addtmp46, ptr %"%\D8\B9\D8\B1\D8\B6\D8\AA", align 8
  br label %merge_113

then_112:                                         ; preds = %while_body_110
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.72 to i64))
  %"%\D8\B7.load5" = load i64, ptr %"%\D8\B7", align 8
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 %"%\D8\B7.load5"
  %arr.get8 = load i64, ptr %arr.elem7, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get8)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.73 to i64))
  %"%\D8\B7.load9" = load i64, ptr %"%\D8\B7", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D8\B7.load9"
  %arr.get12 = load i64, ptr %arr.elem11, align 8
  call void @"\D8\A7\D8\B3\D9\85_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D8\A9"(i64 %arr.get12)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.74 to i64))
  %"%\D8\B7.load13" = load i64, ptr %"%\D8\B7", align 8
  %arr.data14 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 %"%\D8\B7.load13"
  %arr.get16 = load i64, ptr %arr.elem15, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get16)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.75 to i64))
  %"%\D8\B7.load17" = load i64, ptr %"%\D8\B7", align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 %"%\D8\B7.load17"
  %arr.get20 = load i64, ptr %arr.elem19, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get20)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.76 to i64))
  %"%\D8\B7.load21" = load i64, ptr %"%\D8\B7", align 8
  %arr.data22 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 %"%\D8\B7.load21"
  %arr.get24 = load i64, ptr %arr.elem23, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get24)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.77 to i64))
  %"%\D8\B7.load25" = load i64, ptr %"%\D8\B7", align 8
  %arr.data26 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem27 = getelementptr i64, ptr %arr.data26, i64 %"%\D8\B7.load25"
  %arr.get28 = load i64, ptr %arr.elem27, align 8
  %cmpeqtmp = icmp eq i64 %arr.get28, 0
  br i1 %cmpeqtmp, label %then_114, label %merge_115

then_114:                                         ; preds = %then_112
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.78 to i64))
  br label %merge_115

then_116:                                         ; preds = %merge_115
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.79 to i64))
  br label %merge_117

then_118:                                         ; preds = %merge_117
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.80 to i64))
  br label %merge_119

then_120:                                         ; preds = %merge_119
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.81 to i64))
  br label %merge_121

while_body_110:                                   ; preds = %while_cond_109
  %"%\D8\B7.load1" = load i64, ptr %"%\D8\B7", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B7.load1"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp, label %then_112, label %merge_113

while_cond_109:                                   ; preds = %merge_113, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", 0
  br i1 %cmplttmp, label %while_body_110, label %while_exit_111

while_exit_111:                                   ; preds = %while_cond_109
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.62 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.66 to i64))
  %"%\D8\B9\D8\B1\D8\B6\D8\AA.load" = load i64, ptr %"%\D8\B9\D8\B1\D8\B6\D8\AA", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D8\B9\D8\B1\D8\B6\D8\AA.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.67 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.68 to i64))
  %arr.data2 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem3 = getelementptr i64, ptr %arr.data2, i64 0
  %arr.get4 = load i64, ptr %arr.elem3, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get4)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.69 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.70 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  ret void
}

define void @"\D8\A7\D8\B9\D8\B1\D8\B6_\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AC\D8\AF\D9\88\D9\84"() {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %"%\D9\85\D8\AC\D9\85\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D9\86\D8\A7\D8\A6\D9\85\D8\A9" = alloca i64, align 8
  %"%\D8\AC\D8\A7\D9\87\D8\B2\D8\A9" = alloca i64, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.84 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.85 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.86 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.87 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.88 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.89 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.90 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.91 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.92 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.93 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.92 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.95 to i64))
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.96 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.97 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.98 to i64))
  store i64 0, ptr %"%\D8\AC\D8\A7\D9\87\D8\B2\D8\A9", align 8
  store i64 0, ptr %"%\D9\86\D8\A7\D8\A6\D9\85\D8\A9", align 8
  store i64 0, ptr %"%\D9\85\D8\AC\D9\85\D8\AF\D8\A9", align 8
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_122

merge_126:                                        ; preds = %then_125, %while_body_123
  %"%\D8\B7.load6" = load i64, ptr %"%\D8\B7", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D8\B7.load6"
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  %cmpeqtmp10 = icmp eq i64 %arr.get9, 0
  br i1 %cmpeqtmp10, label %then_127, label %merge_128

merge_128:                                        ; preds = %then_127, %merge_126
  %"%\D8\B7.load13" = load i64, ptr %"%\D8\B7", align 8
  %arr.data14 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 %"%\D8\B7.load13"
  %arr.get16 = load i64, ptr %arr.elem15, align 8
  %cmpeqtmp17 = icmp eq i64 %arr.get16, 0
  br i1 %cmpeqtmp17, label %then_129, label %merge_130

merge_130:                                        ; preds = %then_129, %merge_128
  %"%\D8\B7.load20" = load i64, ptr %"%\D8\B7", align 8
  %addtmp21 = add i64 %"%\D8\B7.load20", 1
  store i64 %addtmp21, ptr %"%\D8\B7", align 8
  br label %while_cond_122

then_125:                                         ; preds = %while_body_123
  %"%\D8\AC\D8\A7\D9\87\D8\B2\D8\A9.load5" = load i64, ptr %"%\D8\AC\D8\A7\D9\87\D8\B2\D8\A9", align 8
  %addtmp = add i64 %"%\D8\AC\D8\A7\D9\87\D8\B2\D8\A9.load5", 1
  store i64 %addtmp, ptr %"%\D8\AC\D8\A7\D9\87\D8\B2\D8\A9", align 8
  br label %merge_126

then_127:                                         ; preds = %merge_126
  %"%\D9\86\D8\A7\D8\A6\D9\85\D8\A9.load11" = load i64, ptr %"%\D9\86\D8\A7\D8\A6\D9\85\D8\A9", align 8
  %addtmp12 = add i64 %"%\D9\86\D8\A7\D8\A6\D9\85\D8\A9.load11", 1
  store i64 %addtmp12, ptr %"%\D9\86\D8\A7\D8\A6\D9\85\D8\A9", align 8
  br label %merge_128

then_129:                                         ; preds = %merge_128
  %"%\D9\85\D8\AC\D9\85\D8\AF\D8\A9.load18" = load i64, ptr %"%\D9\85\D8\AC\D9\85\D8\AF\D8\A9", align 8
  %addtmp19 = add i64 %"%\D9\85\D8\AC\D9\85\D8\AF\D8\A9.load18", 1
  store i64 %addtmp19, ptr %"%\D9\85\D8\AC\D9\85\D8\AF\D8\A9", align 8
  br label %merge_130

while_body_123:                                   ; preds = %while_cond_122
  %"%\D8\B7.load1" = load i64, ptr %"%\D8\B7", align 8
  %arr.data2 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem3 = getelementptr i64, ptr %arr.data2, i64 %"%\D8\B7.load1"
  %arr.get4 = load i64, ptr %arr.elem3, align 8
  %cmpeqtmp = icmp eq i64 %arr.get4, 0
  br i1 %cmpeqtmp, label %then_125, label %merge_126

while_cond_122:                                   ; preds = %merge_130, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", 0
  br i1 %cmplttmp, label %while_body_123, label %while_exit_124

while_exit_124:                                   ; preds = %while_cond_122
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.99 to i64))
  %"%\D8\AC\D8\A7\D9\87\D8\B2\D8\A9.load" = load i64, ptr %"%\D8\AC\D8\A7\D9\87\D8\B2\D8\A9", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D8\AC\D8\A7\D9\87\D8\B2\D8\A9.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.100 to i64))
  %"%\D9\86\D8\A7\D8\A6\D9\85\D8\A9.load" = load i64, ptr %"%\D9\86\D8\A7\D8\A6\D9\85\D8\A9", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\86\D8\A7\D8\A6\D9\85\D8\A9.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.101 to i64))
  %"%\D9\85\D8\AC\D9\85\D8\AF\D8\A9.load" = load i64, ptr %"%\D9\85\D8\AC\D9\85\D8\AF\D8\A9", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\85\D8\AC\D9\85\D8\AF\D8\A9.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.102 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.103 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  ret void
}

define void @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\A7\D9\84\D9\85\D8\AC\D8\AF\D9\88\D9\84"() {
entry:
  %"%\D8\AD" = alloca i64, align 8
  %"%pid3" = alloca i64, align 8
  %"%pid2" = alloca i64, align 8
  %"%pid1" = alloca i64, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.107 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.109 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.110 to i64))
  %"%pid1.load" = load i64, ptr %"%pid1", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%pid1.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.111 to i64))
  %"%pid2.load" = load i64, ptr %"%pid2", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%pid2.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.111 to i64))
  %"%pid3.load" = load i64, ptr %"%pid3", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%pid3.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.114 to i64))
  call void @"\D8\A7\D8\B9\D8\B1\D8\B6_\D8\AC\D8\AF\D9\88\D9\84_\D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA"()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.115 to i64))
  %"%pid2.load1" = load i64, ptr %"%pid2", align 8
  %calltmp = call i64 @"\D8\AC\D9\85\D8\AF_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(i64 %"%pid2.load1")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.116 to i64))
  %"%pid2.load2" = load i64, ptr %"%pid2", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%pid2.load2")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.117 to i64))
  %"%pid2.load3" = load i64, ptr %"%pid2", align 8
  %calltmp4 = call i64 @"\D8\A3\D8\B9\D8\B7\D9\86\D9\8A_\D8\AD\D8\A7\D9\84\D8\A9_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(i64 %"%pid2.load3")
  store i64 %calltmp4, ptr %"%\D8\AD", align 8
  %"%\D8\AD.load" = load i64, ptr %"%\D8\AD", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D8\AD.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%pid2.load5" = load i64, ptr %"%pid2", align 8
  %calltmp6 = call i64 @"\D8\A7\D8\B3\D8\AA\D8\A3\D9\86\D9\81_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(i64 %"%pid2.load5")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.116 to i64))
  %"%pid2.load7" = load i64, ptr %"%pid2", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%pid2.load7")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.120 to i64))
  %"%pid2.load8" = load i64, ptr %"%pid2", align 8
  %calltmp9 = call i64 @"\D8\A3\D8\B9\D8\B7\D9\86\D9\8A_\D8\AD\D8\A7\D9\84\D8\A9_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(i64 %"%pid2.load8")
  store i64 %calltmp9, ptr %"%\D8\AD", align 8
  %"%\D8\AD.load10" = load i64, ptr %"%\D8\AD", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D8\AD.load10")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.122 to i64))
  %"%pid3.load11" = load i64, ptr %"%pid3", align 8
  %calltmp12 = call i64 @"\D8\A3\D9\86\D9\87_\D8\B9\D9\85\D9\84\D9\8A\D8\A9"(i64 %"%pid3.load11")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.123 to i64))
  %"%pid3.load13" = load i64, ptr %"%pid3", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%pid3.load13")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.124 to i64))
  call void @"\D9\86\D8\B8\D9\81_\D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D9\86\D8\AA\D9\87\D9\8A\D8\A9"()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.125 to i64))
  call void @"\D8\A7\D8\B9\D8\B1\D8\B6_\D8\AC\D8\AF\D9\88\D9\84_\D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA"()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.126 to i64))
  call void @"\D8\A7\D8\B9\D8\B1\D8\B6_\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AC\D8\AF\D9\88\D9\84"()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.128 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.129 to i64))
  ret void
}

define void @__sad_main() {
entry:
  %"%\D9\88\D9\82\D8\AA_\D8\A2\D8\AE\D8\B1_\D8\AC\D8\AF\D9\88\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\A7\D9\84\D9\85\D8\AC\D8\AF\D9\88\D9\84_\D9\86\D8\B4\D8\B7" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D8\A8\D8\AF\D9\8A\D9\84\D8\A7\D8\AA_\D8\A7\D9\84\D8\B3\D9\8A\D8\A7\D9\82" = alloca i64, align 8
  %"%\D8\A2\D8\AE\D8\B1_PID" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\A3\D8\A8" = alloca i64, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\B3\D9\8A\D8\A7\D9\82" = alloca i64, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D9\85\D9\83\D8\AF\D8\B3" = alloca i64, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\AA\D8\A8\D8\AF\D9\8A\D9\84\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D9\88\D9\82\D8\AA_CPU" = alloca i64, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\B3\D8\A8\D8\A8_\D9\86\D9\88\D9\85" = alloca i64, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\A7\D8\B3\D8\AA\D9\8A\D9\82\D8\A7\D8\B8" = alloca i64, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\B4\D8\B1\D8\A7\D8\A6\D8\AD" = alloca i64, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_PID" = alloca i64, align 8
  %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\AD\D8\A7\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\B4\D8\A7\D8\B1\D8\A9_\D8\A7\D8\B3\D8\AA\D8\A3\D9\86\D9\81" = alloca i64, align 8
  %"%\D8\A5\D8\B4\D8\A7\D8\B1\D8\A9_\D8\AA\D9\88\D9\82\D9\81" = alloca i64, align 8
  %"%\D8\A5\D8\B4\D8\A7\D8\B1\D8\A9_\D9\82\D8\AA\D9\84" = alloca i64, align 8
  %"%\D8\A5\D8\B4\D8\A7\D8\B1\D8\A9_\D8\A5\D9\86\D9\87\D8\A7\D8\A1" = alloca i64, align 8
  %"%\D9\86\D9\88\D9\85_\D8\AD\D8\AF\D8\AB" = alloca i64, align 8
  %"%\D9\86\D9\88\D9\85_\D9\82\D9\81\D9\84" = alloca i64, align 8
  %"%\D9\86\D9\88\D9\85_IO" = alloca i64, align 8
  %"%\D9\86\D9\88\D9\85_\D9\85\D8\A4\D9\82\D8\AA" = alloca i64, align 8
  %"%\D9\86\D9\88\D9\85_\D9\84\D8\A7_\D8\B4\D9\8A\D8\A1" = alloca i64, align 8
  %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D8\AE\D9\85\D9\88\D9\84" = alloca i64, align 8
  %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D9\85\D9\86\D8\AE\D9\81\D8\B6\D8\A9" = alloca i64, align 8
  %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D8\AA\D8\AD\D8\AA_\D8\B9\D8\A7\D8\AF\D9\8A" = alloca i64, align 8
  %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D8\B9\D8\A7\D8\AF\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D9\81\D9\88\D9\82_\D8\B9\D8\A7\D8\AF\D9\8A" = alloca i64, align 8
  %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D8\B9\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D8\AD\D8\B1\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\B2\D9\88\D9\85\D8\A8\D9\8A" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D9\85\D9\86\D8\AA\D9\87\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D9\85\D8\AC\D9\85\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D9\86\D8\A7\D8\A6\D9\85\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\AA\D8\B9\D9\85\D9\84" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\AC\D8\A7\D9\87\D8\B2\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D9\81\D8\A7\D8\B1\D8\BA\D8\A9" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\B4\D8\B1\D9\8A\D8\AD\D8\A9_\D8\A7\D9\84\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AF\D8\B3" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\B9\D8\AF\D8\AF_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 64, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\B9\D8\AF\D8\AF_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA", align 8
  store i64 16384, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\85\D9\83\D8\AF\D8\B3", align 8
  store i64 5, ptr %"%\D8\A7\D9\84\D8\B4\D8\B1\D9\8A\D8\AD\D8\A9_\D8\A7\D9\84\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A\D8\A9", align 8
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D9\81\D8\A7\D8\B1\D8\BA\D8\A9", align 8
  store i64 1, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\AC\D8\A7\D9\87\D8\B2\D8\A9", align 8
  store i64 2, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\AA\D8\B9\D9\85\D9\84", align 8
  store i64 3, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D9\86\D8\A7\D8\A6\D9\85\D8\A9", align 8
  store i64 4, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D9\85\D8\AC\D9\85\D8\AF\D8\A9", align 8
  store i64 5, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D9\85\D9\86\D8\AA\D9\87\D9\8A\D8\A9", align 8
  store i64 6, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\B2\D9\88\D9\85\D8\A8\D9\8A", align 8
  store i64 0, ptr %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D8\AD\D8\B1\D8\AC\D8\A9", align 8
  store i64 1, ptr %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D8\B9\D8\A7\D9\84\D9\8A\D8\A9", align 8
  store i64 2, ptr %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D9\81\D9\88\D9\82_\D8\B9\D8\A7\D8\AF\D9\8A", align 8
  store i64 3, ptr %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D8\B9\D8\A7\D8\AF\D9\8A\D8\A9", align 8
  store i64 4, ptr %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D8\AA\D8\AD\D8\AA_\D8\B9\D8\A7\D8\AF\D9\8A", align 8
  store i64 5, ptr %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D9\85\D9\86\D8\AE\D9\81\D8\B6\D8\A9", align 8
  store i64 7, ptr %"%\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9_\D8\AE\D9\85\D9\88\D9\84", align 8
  store i64 0, ptr %"%\D9\86\D9\88\D9\85_\D9\84\D8\A7_\D8\B4\D9\8A\D8\A1", align 8
  store i64 1, ptr %"%\D9\86\D9\88\D9\85_\D9\85\D8\A4\D9\82\D8\AA", align 8
  store i64 2, ptr %"%\D9\86\D9\88\D9\85_IO", align 8
  store i64 3, ptr %"%\D9\86\D9\88\D9\85_\D9\82\D9\81\D9\84", align 8
  store i64 4, ptr %"%\D9\86\D9\88\D9\85_\D8\AD\D8\AF\D8\AB", align 8
  store i64 1, ptr %"%\D8\A5\D8\B4\D8\A7\D8\B1\D8\A9_\D8\A5\D9\86\D9\87\D8\A7\D8\A1", align 8
  store i64 9, ptr %"%\D8\A5\D8\B4\D8\A7\D8\B1\D8\A9_\D9\82\D8\AA\D9\84", align 8
  store i64 19, ptr %"%\D8\A5\D8\B4\D8\A7\D8\B1\D8\A9_\D8\AA\D9\88\D9\82\D9\81", align 8
  store i64 18, ptr %"%\D8\A5\D8\B4\D8\A7\D8\B1\D8\A9_\D8\A7\D8\B3\D8\AA\D8\A3\D9\86\D9\81", align 8
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 64, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep1 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data2 = load ptr, ptr %arr.data.gep1, align 8
  %arr.elem = getelementptr i64, ptr %arr.data2, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data.gep3 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data4 = load ptr, ptr %arr.data.gep3, align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 1
  store i64 0, ptr %arr.elem5, align 8
  %arr.data.gep6 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data7 = load ptr, ptr %arr.data.gep6, align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 2
  store i64 0, ptr %arr.elem8, align 8
  %arr.data.gep9 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data10 = load ptr, ptr %arr.data.gep9, align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 3
  store i64 0, ptr %arr.elem11, align 8
  %arr.data.gep12 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data13 = load ptr, ptr %arr.data.gep12, align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 4
  store i64 0, ptr %arr.elem14, align 8
  %arr.data.gep15 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data16 = load ptr, ptr %arr.data.gep15, align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 5
  store i64 0, ptr %arr.elem17, align 8
  %arr.data.gep18 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data19 = load ptr, ptr %arr.data.gep18, align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 6
  store i64 0, ptr %arr.elem20, align 8
  %arr.data.gep21 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data22 = load ptr, ptr %arr.data.gep21, align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 7
  store i64 0, ptr %arr.elem23, align 8
  %arr.data.gep24 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data25 = load ptr, ptr %arr.data.gep24, align 8
  %arr.elem26 = getelementptr i64, ptr %arr.data25, i64 8
  store i64 0, ptr %arr.elem26, align 8
  %arr.data.gep27 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data28 = load ptr, ptr %arr.data.gep27, align 8
  %arr.elem29 = getelementptr i64, ptr %arr.data28, i64 9
  store i64 0, ptr %arr.elem29, align 8
  %arr.data.gep30 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data31 = load ptr, ptr %arr.data.gep30, align 8
  %arr.elem32 = getelementptr i64, ptr %arr.data31, i64 10
  store i64 0, ptr %arr.elem32, align 8
  %arr.data.gep33 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data34 = load ptr, ptr %arr.data.gep33, align 8
  %arr.elem35 = getelementptr i64, ptr %arr.data34, i64 11
  store i64 0, ptr %arr.elem35, align 8
  %arr.data.gep36 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data37 = load ptr, ptr %arr.data.gep36, align 8
  %arr.elem38 = getelementptr i64, ptr %arr.data37, i64 12
  store i64 0, ptr %arr.elem38, align 8
  %arr.data.gep39 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data40 = load ptr, ptr %arr.data.gep39, align 8
  %arr.elem41 = getelementptr i64, ptr %arr.data40, i64 13
  store i64 0, ptr %arr.elem41, align 8
  %arr.data.gep42 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data43 = load ptr, ptr %arr.data.gep42, align 8
  %arr.elem44 = getelementptr i64, ptr %arr.data43, i64 14
  store i64 0, ptr %arr.elem44, align 8
  %arr.data.gep45 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data46 = load ptr, ptr %arr.data.gep45, align 8
  %arr.elem47 = getelementptr i64, ptr %arr.data46, i64 15
  store i64 0, ptr %arr.elem47, align 8
  %arr.data.gep48 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data49 = load ptr, ptr %arr.data.gep48, align 8
  %arr.elem50 = getelementptr i64, ptr %arr.data49, i64 16
  store i64 0, ptr %arr.elem50, align 8
  %arr.data.gep51 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data52 = load ptr, ptr %arr.data.gep51, align 8
  %arr.elem53 = getelementptr i64, ptr %arr.data52, i64 17
  store i64 0, ptr %arr.elem53, align 8
  %arr.data.gep54 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data55 = load ptr, ptr %arr.data.gep54, align 8
  %arr.elem56 = getelementptr i64, ptr %arr.data55, i64 18
  store i64 0, ptr %arr.elem56, align 8
  %arr.data.gep57 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data58 = load ptr, ptr %arr.data.gep57, align 8
  %arr.elem59 = getelementptr i64, ptr %arr.data58, i64 19
  store i64 0, ptr %arr.elem59, align 8
  %arr.data.gep60 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data61 = load ptr, ptr %arr.data.gep60, align 8
  %arr.elem62 = getelementptr i64, ptr %arr.data61, i64 20
  store i64 0, ptr %arr.elem62, align 8
  %arr.data.gep63 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data64 = load ptr, ptr %arr.data.gep63, align 8
  %arr.elem65 = getelementptr i64, ptr %arr.data64, i64 21
  store i64 0, ptr %arr.elem65, align 8
  %arr.data.gep66 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data67 = load ptr, ptr %arr.data.gep66, align 8
  %arr.elem68 = getelementptr i64, ptr %arr.data67, i64 22
  store i64 0, ptr %arr.elem68, align 8
  %arr.data.gep69 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data70 = load ptr, ptr %arr.data.gep69, align 8
  %arr.elem71 = getelementptr i64, ptr %arr.data70, i64 23
  store i64 0, ptr %arr.elem71, align 8
  %arr.data.gep72 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data73 = load ptr, ptr %arr.data.gep72, align 8
  %arr.elem74 = getelementptr i64, ptr %arr.data73, i64 24
  store i64 0, ptr %arr.elem74, align 8
  %arr.data.gep75 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data76 = load ptr, ptr %arr.data.gep75, align 8
  %arr.elem77 = getelementptr i64, ptr %arr.data76, i64 25
  store i64 0, ptr %arr.elem77, align 8
  %arr.data.gep78 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data79 = load ptr, ptr %arr.data.gep78, align 8
  %arr.elem80 = getelementptr i64, ptr %arr.data79, i64 26
  store i64 0, ptr %arr.elem80, align 8
  %arr.data.gep81 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data82 = load ptr, ptr %arr.data.gep81, align 8
  %arr.elem83 = getelementptr i64, ptr %arr.data82, i64 27
  store i64 0, ptr %arr.elem83, align 8
  %arr.data.gep84 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data85 = load ptr, ptr %arr.data.gep84, align 8
  %arr.elem86 = getelementptr i64, ptr %arr.data85, i64 28
  store i64 0, ptr %arr.elem86, align 8
  %arr.data.gep87 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data88 = load ptr, ptr %arr.data.gep87, align 8
  %arr.elem89 = getelementptr i64, ptr %arr.data88, i64 29
  store i64 0, ptr %arr.elem89, align 8
  %arr.data.gep90 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data91 = load ptr, ptr %arr.data.gep90, align 8
  %arr.elem92 = getelementptr i64, ptr %arr.data91, i64 30
  store i64 0, ptr %arr.elem92, align 8
  %arr.data.gep93 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data94 = load ptr, ptr %arr.data.gep93, align 8
  %arr.elem95 = getelementptr i64, ptr %arr.data94, i64 31
  store i64 0, ptr %arr.elem95, align 8
  %arr.data.gep96 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data97 = load ptr, ptr %arr.data.gep96, align 8
  %arr.elem98 = getelementptr i64, ptr %arr.data97, i64 32
  store i64 0, ptr %arr.elem98, align 8
  %arr.data.gep99 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data100 = load ptr, ptr %arr.data.gep99, align 8
  %arr.elem101 = getelementptr i64, ptr %arr.data100, i64 33
  store i64 0, ptr %arr.elem101, align 8
  %arr.data.gep102 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data103 = load ptr, ptr %arr.data.gep102, align 8
  %arr.elem104 = getelementptr i64, ptr %arr.data103, i64 34
  store i64 0, ptr %arr.elem104, align 8
  %arr.data.gep105 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data106 = load ptr, ptr %arr.data.gep105, align 8
  %arr.elem107 = getelementptr i64, ptr %arr.data106, i64 35
  store i64 0, ptr %arr.elem107, align 8
  %arr.data.gep108 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data109 = load ptr, ptr %arr.data.gep108, align 8
  %arr.elem110 = getelementptr i64, ptr %arr.data109, i64 36
  store i64 0, ptr %arr.elem110, align 8
  %arr.data.gep111 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data112 = load ptr, ptr %arr.data.gep111, align 8
  %arr.elem113 = getelementptr i64, ptr %arr.data112, i64 37
  store i64 0, ptr %arr.elem113, align 8
  %arr.data.gep114 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data115 = load ptr, ptr %arr.data.gep114, align 8
  %arr.elem116 = getelementptr i64, ptr %arr.data115, i64 38
  store i64 0, ptr %arr.elem116, align 8
  %arr.data.gep117 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data118 = load ptr, ptr %arr.data.gep117, align 8
  %arr.elem119 = getelementptr i64, ptr %arr.data118, i64 39
  store i64 0, ptr %arr.elem119, align 8
  %arr.data.gep120 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data121 = load ptr, ptr %arr.data.gep120, align 8
  %arr.elem122 = getelementptr i64, ptr %arr.data121, i64 40
  store i64 0, ptr %arr.elem122, align 8
  %arr.data.gep123 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data124 = load ptr, ptr %arr.data.gep123, align 8
  %arr.elem125 = getelementptr i64, ptr %arr.data124, i64 41
  store i64 0, ptr %arr.elem125, align 8
  %arr.data.gep126 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data127 = load ptr, ptr %arr.data.gep126, align 8
  %arr.elem128 = getelementptr i64, ptr %arr.data127, i64 42
  store i64 0, ptr %arr.elem128, align 8
  %arr.data.gep129 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data130 = load ptr, ptr %arr.data.gep129, align 8
  %arr.elem131 = getelementptr i64, ptr %arr.data130, i64 43
  store i64 0, ptr %arr.elem131, align 8
  %arr.data.gep132 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data133 = load ptr, ptr %arr.data.gep132, align 8
  %arr.elem134 = getelementptr i64, ptr %arr.data133, i64 44
  store i64 0, ptr %arr.elem134, align 8
  %arr.data.gep135 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data136 = load ptr, ptr %arr.data.gep135, align 8
  %arr.elem137 = getelementptr i64, ptr %arr.data136, i64 45
  store i64 0, ptr %arr.elem137, align 8
  %arr.data.gep138 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data139 = load ptr, ptr %arr.data.gep138, align 8
  %arr.elem140 = getelementptr i64, ptr %arr.data139, i64 46
  store i64 0, ptr %arr.elem140, align 8
  %arr.data.gep141 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data142 = load ptr, ptr %arr.data.gep141, align 8
  %arr.elem143 = getelementptr i64, ptr %arr.data142, i64 47
  store i64 0, ptr %arr.elem143, align 8
  %arr.data.gep144 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data145 = load ptr, ptr %arr.data.gep144, align 8
  %arr.elem146 = getelementptr i64, ptr %arr.data145, i64 48
  store i64 0, ptr %arr.elem146, align 8
  %arr.data.gep147 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data148 = load ptr, ptr %arr.data.gep147, align 8
  %arr.elem149 = getelementptr i64, ptr %arr.data148, i64 49
  store i64 0, ptr %arr.elem149, align 8
  %arr.data.gep150 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data151 = load ptr, ptr %arr.data.gep150, align 8
  %arr.elem152 = getelementptr i64, ptr %arr.data151, i64 50
  store i64 0, ptr %arr.elem152, align 8
  %arr.data.gep153 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data154 = load ptr, ptr %arr.data.gep153, align 8
  %arr.elem155 = getelementptr i64, ptr %arr.data154, i64 51
  store i64 0, ptr %arr.elem155, align 8
  %arr.data.gep156 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data157 = load ptr, ptr %arr.data.gep156, align 8
  %arr.elem158 = getelementptr i64, ptr %arr.data157, i64 52
  store i64 0, ptr %arr.elem158, align 8
  %arr.data.gep159 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data160 = load ptr, ptr %arr.data.gep159, align 8
  %arr.elem161 = getelementptr i64, ptr %arr.data160, i64 53
  store i64 0, ptr %arr.elem161, align 8
  %arr.data.gep162 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data163 = load ptr, ptr %arr.data.gep162, align 8
  %arr.elem164 = getelementptr i64, ptr %arr.data163, i64 54
  store i64 0, ptr %arr.elem164, align 8
  %arr.data.gep165 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data166 = load ptr, ptr %arr.data.gep165, align 8
  %arr.elem167 = getelementptr i64, ptr %arr.data166, i64 55
  store i64 0, ptr %arr.elem167, align 8
  %arr.data.gep168 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data169 = load ptr, ptr %arr.data.gep168, align 8
  %arr.elem170 = getelementptr i64, ptr %arr.data169, i64 56
  store i64 0, ptr %arr.elem170, align 8
  %arr.data.gep171 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data172 = load ptr, ptr %arr.data.gep171, align 8
  %arr.elem173 = getelementptr i64, ptr %arr.data172, i64 57
  store i64 0, ptr %arr.elem173, align 8
  %arr.data.gep174 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data175 = load ptr, ptr %arr.data.gep174, align 8
  %arr.elem176 = getelementptr i64, ptr %arr.data175, i64 58
  store i64 0, ptr %arr.elem176, align 8
  %arr.data.gep177 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data178 = load ptr, ptr %arr.data.gep177, align 8
  %arr.elem179 = getelementptr i64, ptr %arr.data178, i64 59
  store i64 0, ptr %arr.elem179, align 8
  %arr.data.gep180 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data181 = load ptr, ptr %arr.data.gep180, align 8
  %arr.elem182 = getelementptr i64, ptr %arr.data181, i64 60
  store i64 0, ptr %arr.elem182, align 8
  %arr.data.gep183 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data184 = load ptr, ptr %arr.data.gep183, align 8
  %arr.elem185 = getelementptr i64, ptr %arr.data184, i64 61
  store i64 0, ptr %arr.elem185, align 8
  %arr.data.gep186 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data187 = load ptr, ptr %arr.data.gep186, align 8
  %arr.elem188 = getelementptr i64, ptr %arr.data187, i64 62
  store i64 0, ptr %arr.elem188, align 8
  %arr.data.gep189 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data190 = load ptr, ptr %arr.data.gep189, align 8
  %arr.elem191 = getelementptr i64, ptr %arr.data190, i64 63
  store i64 0, ptr %arr.elem191, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\AD\D8\A7\D9\84\D8\A9", align 8
  %arr_new192 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep193 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 0
  store i64 64, ptr %arr.len.gep193, align 8
  %arr.cap.gep194 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep194, align 8
  %arr.data195 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep196 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  store ptr %arr.data195, ptr %arr.data.gep196, align 8
  %arr.data.gep197 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data198 = load ptr, ptr %arr.data.gep197, align 8
  %arr.elem199 = getelementptr i64, ptr %arr.data198, i64 0
  store i64 0, ptr %arr.elem199, align 8
  %arr.data.gep200 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data201 = load ptr, ptr %arr.data.gep200, align 8
  %arr.elem202 = getelementptr i64, ptr %arr.data201, i64 1
  store i64 0, ptr %arr.elem202, align 8
  %arr.data.gep203 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data204 = load ptr, ptr %arr.data.gep203, align 8
  %arr.elem205 = getelementptr i64, ptr %arr.data204, i64 2
  store i64 0, ptr %arr.elem205, align 8
  %arr.data.gep206 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data207 = load ptr, ptr %arr.data.gep206, align 8
  %arr.elem208 = getelementptr i64, ptr %arr.data207, i64 3
  store i64 0, ptr %arr.elem208, align 8
  %arr.data.gep209 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data210 = load ptr, ptr %arr.data.gep209, align 8
  %arr.elem211 = getelementptr i64, ptr %arr.data210, i64 4
  store i64 0, ptr %arr.elem211, align 8
  %arr.data.gep212 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data213 = load ptr, ptr %arr.data.gep212, align 8
  %arr.elem214 = getelementptr i64, ptr %arr.data213, i64 5
  store i64 0, ptr %arr.elem214, align 8
  %arr.data.gep215 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data216 = load ptr, ptr %arr.data.gep215, align 8
  %arr.elem217 = getelementptr i64, ptr %arr.data216, i64 6
  store i64 0, ptr %arr.elem217, align 8
  %arr.data.gep218 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data219 = load ptr, ptr %arr.data.gep218, align 8
  %arr.elem220 = getelementptr i64, ptr %arr.data219, i64 7
  store i64 0, ptr %arr.elem220, align 8
  %arr.data.gep221 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data222 = load ptr, ptr %arr.data.gep221, align 8
  %arr.elem223 = getelementptr i64, ptr %arr.data222, i64 8
  store i64 0, ptr %arr.elem223, align 8
  %arr.data.gep224 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data225 = load ptr, ptr %arr.data.gep224, align 8
  %arr.elem226 = getelementptr i64, ptr %arr.data225, i64 9
  store i64 0, ptr %arr.elem226, align 8
  %arr.data.gep227 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data228 = load ptr, ptr %arr.data.gep227, align 8
  %arr.elem229 = getelementptr i64, ptr %arr.data228, i64 10
  store i64 0, ptr %arr.elem229, align 8
  %arr.data.gep230 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data231 = load ptr, ptr %arr.data.gep230, align 8
  %arr.elem232 = getelementptr i64, ptr %arr.data231, i64 11
  store i64 0, ptr %arr.elem232, align 8
  %arr.data.gep233 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data234 = load ptr, ptr %arr.data.gep233, align 8
  %arr.elem235 = getelementptr i64, ptr %arr.data234, i64 12
  store i64 0, ptr %arr.elem235, align 8
  %arr.data.gep236 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data237 = load ptr, ptr %arr.data.gep236, align 8
  %arr.elem238 = getelementptr i64, ptr %arr.data237, i64 13
  store i64 0, ptr %arr.elem238, align 8
  %arr.data.gep239 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data240 = load ptr, ptr %arr.data.gep239, align 8
  %arr.elem241 = getelementptr i64, ptr %arr.data240, i64 14
  store i64 0, ptr %arr.elem241, align 8
  %arr.data.gep242 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data243 = load ptr, ptr %arr.data.gep242, align 8
  %arr.elem244 = getelementptr i64, ptr %arr.data243, i64 15
  store i64 0, ptr %arr.elem244, align 8
  %arr.data.gep245 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data246 = load ptr, ptr %arr.data.gep245, align 8
  %arr.elem247 = getelementptr i64, ptr %arr.data246, i64 16
  store i64 0, ptr %arr.elem247, align 8
  %arr.data.gep248 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data249 = load ptr, ptr %arr.data.gep248, align 8
  %arr.elem250 = getelementptr i64, ptr %arr.data249, i64 17
  store i64 0, ptr %arr.elem250, align 8
  %arr.data.gep251 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data252 = load ptr, ptr %arr.data.gep251, align 8
  %arr.elem253 = getelementptr i64, ptr %arr.data252, i64 18
  store i64 0, ptr %arr.elem253, align 8
  %arr.data.gep254 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data255 = load ptr, ptr %arr.data.gep254, align 8
  %arr.elem256 = getelementptr i64, ptr %arr.data255, i64 19
  store i64 0, ptr %arr.elem256, align 8
  %arr.data.gep257 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data258 = load ptr, ptr %arr.data.gep257, align 8
  %arr.elem259 = getelementptr i64, ptr %arr.data258, i64 20
  store i64 0, ptr %arr.elem259, align 8
  %arr.data.gep260 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data261 = load ptr, ptr %arr.data.gep260, align 8
  %arr.elem262 = getelementptr i64, ptr %arr.data261, i64 21
  store i64 0, ptr %arr.elem262, align 8
  %arr.data.gep263 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data264 = load ptr, ptr %arr.data.gep263, align 8
  %arr.elem265 = getelementptr i64, ptr %arr.data264, i64 22
  store i64 0, ptr %arr.elem265, align 8
  %arr.data.gep266 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data267 = load ptr, ptr %arr.data.gep266, align 8
  %arr.elem268 = getelementptr i64, ptr %arr.data267, i64 23
  store i64 0, ptr %arr.elem268, align 8
  %arr.data.gep269 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data270 = load ptr, ptr %arr.data.gep269, align 8
  %arr.elem271 = getelementptr i64, ptr %arr.data270, i64 24
  store i64 0, ptr %arr.elem271, align 8
  %arr.data.gep272 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data273 = load ptr, ptr %arr.data.gep272, align 8
  %arr.elem274 = getelementptr i64, ptr %arr.data273, i64 25
  store i64 0, ptr %arr.elem274, align 8
  %arr.data.gep275 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data276 = load ptr, ptr %arr.data.gep275, align 8
  %arr.elem277 = getelementptr i64, ptr %arr.data276, i64 26
  store i64 0, ptr %arr.elem277, align 8
  %arr.data.gep278 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data279 = load ptr, ptr %arr.data.gep278, align 8
  %arr.elem280 = getelementptr i64, ptr %arr.data279, i64 27
  store i64 0, ptr %arr.elem280, align 8
  %arr.data.gep281 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data282 = load ptr, ptr %arr.data.gep281, align 8
  %arr.elem283 = getelementptr i64, ptr %arr.data282, i64 28
  store i64 0, ptr %arr.elem283, align 8
  %arr.data.gep284 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data285 = load ptr, ptr %arr.data.gep284, align 8
  %arr.elem286 = getelementptr i64, ptr %arr.data285, i64 29
  store i64 0, ptr %arr.elem286, align 8
  %arr.data.gep287 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data288 = load ptr, ptr %arr.data.gep287, align 8
  %arr.elem289 = getelementptr i64, ptr %arr.data288, i64 30
  store i64 0, ptr %arr.elem289, align 8
  %arr.data.gep290 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data291 = load ptr, ptr %arr.data.gep290, align 8
  %arr.elem292 = getelementptr i64, ptr %arr.data291, i64 31
  store i64 0, ptr %arr.elem292, align 8
  %arr.data.gep293 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data294 = load ptr, ptr %arr.data.gep293, align 8
  %arr.elem295 = getelementptr i64, ptr %arr.data294, i64 32
  store i64 0, ptr %arr.elem295, align 8
  %arr.data.gep296 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data297 = load ptr, ptr %arr.data.gep296, align 8
  %arr.elem298 = getelementptr i64, ptr %arr.data297, i64 33
  store i64 0, ptr %arr.elem298, align 8
  %arr.data.gep299 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data300 = load ptr, ptr %arr.data.gep299, align 8
  %arr.elem301 = getelementptr i64, ptr %arr.data300, i64 34
  store i64 0, ptr %arr.elem301, align 8
  %arr.data.gep302 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data303 = load ptr, ptr %arr.data.gep302, align 8
  %arr.elem304 = getelementptr i64, ptr %arr.data303, i64 35
  store i64 0, ptr %arr.elem304, align 8
  %arr.data.gep305 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data306 = load ptr, ptr %arr.data.gep305, align 8
  %arr.elem307 = getelementptr i64, ptr %arr.data306, i64 36
  store i64 0, ptr %arr.elem307, align 8
  %arr.data.gep308 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data309 = load ptr, ptr %arr.data.gep308, align 8
  %arr.elem310 = getelementptr i64, ptr %arr.data309, i64 37
  store i64 0, ptr %arr.elem310, align 8
  %arr.data.gep311 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data312 = load ptr, ptr %arr.data.gep311, align 8
  %arr.elem313 = getelementptr i64, ptr %arr.data312, i64 38
  store i64 0, ptr %arr.elem313, align 8
  %arr.data.gep314 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data315 = load ptr, ptr %arr.data.gep314, align 8
  %arr.elem316 = getelementptr i64, ptr %arr.data315, i64 39
  store i64 0, ptr %arr.elem316, align 8
  %arr.data.gep317 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data318 = load ptr, ptr %arr.data.gep317, align 8
  %arr.elem319 = getelementptr i64, ptr %arr.data318, i64 40
  store i64 0, ptr %arr.elem319, align 8
  %arr.data.gep320 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data321 = load ptr, ptr %arr.data.gep320, align 8
  %arr.elem322 = getelementptr i64, ptr %arr.data321, i64 41
  store i64 0, ptr %arr.elem322, align 8
  %arr.data.gep323 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data324 = load ptr, ptr %arr.data.gep323, align 8
  %arr.elem325 = getelementptr i64, ptr %arr.data324, i64 42
  store i64 0, ptr %arr.elem325, align 8
  %arr.data.gep326 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data327 = load ptr, ptr %arr.data.gep326, align 8
  %arr.elem328 = getelementptr i64, ptr %arr.data327, i64 43
  store i64 0, ptr %arr.elem328, align 8
  %arr.data.gep329 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data330 = load ptr, ptr %arr.data.gep329, align 8
  %arr.elem331 = getelementptr i64, ptr %arr.data330, i64 44
  store i64 0, ptr %arr.elem331, align 8
  %arr.data.gep332 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data333 = load ptr, ptr %arr.data.gep332, align 8
  %arr.elem334 = getelementptr i64, ptr %arr.data333, i64 45
  store i64 0, ptr %arr.elem334, align 8
  %arr.data.gep335 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data336 = load ptr, ptr %arr.data.gep335, align 8
  %arr.elem337 = getelementptr i64, ptr %arr.data336, i64 46
  store i64 0, ptr %arr.elem337, align 8
  %arr.data.gep338 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data339 = load ptr, ptr %arr.data.gep338, align 8
  %arr.elem340 = getelementptr i64, ptr %arr.data339, i64 47
  store i64 0, ptr %arr.elem340, align 8
  %arr.data.gep341 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data342 = load ptr, ptr %arr.data.gep341, align 8
  %arr.elem343 = getelementptr i64, ptr %arr.data342, i64 48
  store i64 0, ptr %arr.elem343, align 8
  %arr.data.gep344 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data345 = load ptr, ptr %arr.data.gep344, align 8
  %arr.elem346 = getelementptr i64, ptr %arr.data345, i64 49
  store i64 0, ptr %arr.elem346, align 8
  %arr.data.gep347 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data348 = load ptr, ptr %arr.data.gep347, align 8
  %arr.elem349 = getelementptr i64, ptr %arr.data348, i64 50
  store i64 0, ptr %arr.elem349, align 8
  %arr.data.gep350 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data351 = load ptr, ptr %arr.data.gep350, align 8
  %arr.elem352 = getelementptr i64, ptr %arr.data351, i64 51
  store i64 0, ptr %arr.elem352, align 8
  %arr.data.gep353 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data354 = load ptr, ptr %arr.data.gep353, align 8
  %arr.elem355 = getelementptr i64, ptr %arr.data354, i64 52
  store i64 0, ptr %arr.elem355, align 8
  %arr.data.gep356 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data357 = load ptr, ptr %arr.data.gep356, align 8
  %arr.elem358 = getelementptr i64, ptr %arr.data357, i64 53
  store i64 0, ptr %arr.elem358, align 8
  %arr.data.gep359 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data360 = load ptr, ptr %arr.data.gep359, align 8
  %arr.elem361 = getelementptr i64, ptr %arr.data360, i64 54
  store i64 0, ptr %arr.elem361, align 8
  %arr.data.gep362 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data363 = load ptr, ptr %arr.data.gep362, align 8
  %arr.elem364 = getelementptr i64, ptr %arr.data363, i64 55
  store i64 0, ptr %arr.elem364, align 8
  %arr.data.gep365 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data366 = load ptr, ptr %arr.data.gep365, align 8
  %arr.elem367 = getelementptr i64, ptr %arr.data366, i64 56
  store i64 0, ptr %arr.elem367, align 8
  %arr.data.gep368 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data369 = load ptr, ptr %arr.data.gep368, align 8
  %arr.elem370 = getelementptr i64, ptr %arr.data369, i64 57
  store i64 0, ptr %arr.elem370, align 8
  %arr.data.gep371 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data372 = load ptr, ptr %arr.data.gep371, align 8
  %arr.elem373 = getelementptr i64, ptr %arr.data372, i64 58
  store i64 0, ptr %arr.elem373, align 8
  %arr.data.gep374 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data375 = load ptr, ptr %arr.data.gep374, align 8
  %arr.elem376 = getelementptr i64, ptr %arr.data375, i64 59
  store i64 0, ptr %arr.elem376, align 8
  %arr.data.gep377 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data378 = load ptr, ptr %arr.data.gep377, align 8
  %arr.elem379 = getelementptr i64, ptr %arr.data378, i64 60
  store i64 0, ptr %arr.elem379, align 8
  %arr.data.gep380 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data381 = load ptr, ptr %arr.data.gep380, align 8
  %arr.elem382 = getelementptr i64, ptr %arr.data381, i64 61
  store i64 0, ptr %arr.elem382, align 8
  %arr.data.gep383 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data384 = load ptr, ptr %arr.data.gep383, align 8
  %arr.elem385 = getelementptr i64, ptr %arr.data384, i64 62
  store i64 0, ptr %arr.elem385, align 8
  %arr.data.gep386 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data387 = load ptr, ptr %arr.data.gep386, align 8
  %arr.elem388 = getelementptr i64, ptr %arr.data387, i64 63
  store i64 0, ptr %arr.elem388, align 8
  %obj.ptrtoint389 = ptrtoint ptr %arr_new192 to i64
  store i64 %obj.ptrtoint389, ptr %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_PID", align 8
  %arr_new390 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep391 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 0
  store i64 64, ptr %arr.len.gep391, align 8
  %arr.cap.gep392 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep392, align 8
  %arr.data393 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep394 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  store ptr %arr.data393, ptr %arr.data.gep394, align 8
  %arr.data.gep395 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data396 = load ptr, ptr %arr.data.gep395, align 8
  %arr.elem397 = getelementptr i64, ptr %arr.data396, i64 0
  store i64 0, ptr %arr.elem397, align 8
  %arr.data.gep398 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data399 = load ptr, ptr %arr.data.gep398, align 8
  %arr.elem400 = getelementptr i64, ptr %arr.data399, i64 1
  store i64 0, ptr %arr.elem400, align 8
  %arr.data.gep401 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data402 = load ptr, ptr %arr.data.gep401, align 8
  %arr.elem403 = getelementptr i64, ptr %arr.data402, i64 2
  store i64 0, ptr %arr.elem403, align 8
  %arr.data.gep404 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data405 = load ptr, ptr %arr.data.gep404, align 8
  %arr.elem406 = getelementptr i64, ptr %arr.data405, i64 3
  store i64 0, ptr %arr.elem406, align 8
  %arr.data.gep407 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data408 = load ptr, ptr %arr.data.gep407, align 8
  %arr.elem409 = getelementptr i64, ptr %arr.data408, i64 4
  store i64 0, ptr %arr.elem409, align 8
  %arr.data.gep410 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data411 = load ptr, ptr %arr.data.gep410, align 8
  %arr.elem412 = getelementptr i64, ptr %arr.data411, i64 5
  store i64 0, ptr %arr.elem412, align 8
  %arr.data.gep413 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data414 = load ptr, ptr %arr.data.gep413, align 8
  %arr.elem415 = getelementptr i64, ptr %arr.data414, i64 6
  store i64 0, ptr %arr.elem415, align 8
  %arr.data.gep416 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data417 = load ptr, ptr %arr.data.gep416, align 8
  %arr.elem418 = getelementptr i64, ptr %arr.data417, i64 7
  store i64 0, ptr %arr.elem418, align 8
  %arr.data.gep419 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data420 = load ptr, ptr %arr.data.gep419, align 8
  %arr.elem421 = getelementptr i64, ptr %arr.data420, i64 8
  store i64 0, ptr %arr.elem421, align 8
  %arr.data.gep422 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data423 = load ptr, ptr %arr.data.gep422, align 8
  %arr.elem424 = getelementptr i64, ptr %arr.data423, i64 9
  store i64 0, ptr %arr.elem424, align 8
  %arr.data.gep425 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data426 = load ptr, ptr %arr.data.gep425, align 8
  %arr.elem427 = getelementptr i64, ptr %arr.data426, i64 10
  store i64 0, ptr %arr.elem427, align 8
  %arr.data.gep428 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data429 = load ptr, ptr %arr.data.gep428, align 8
  %arr.elem430 = getelementptr i64, ptr %arr.data429, i64 11
  store i64 0, ptr %arr.elem430, align 8
  %arr.data.gep431 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data432 = load ptr, ptr %arr.data.gep431, align 8
  %arr.elem433 = getelementptr i64, ptr %arr.data432, i64 12
  store i64 0, ptr %arr.elem433, align 8
  %arr.data.gep434 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data435 = load ptr, ptr %arr.data.gep434, align 8
  %arr.elem436 = getelementptr i64, ptr %arr.data435, i64 13
  store i64 0, ptr %arr.elem436, align 8
  %arr.data.gep437 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data438 = load ptr, ptr %arr.data.gep437, align 8
  %arr.elem439 = getelementptr i64, ptr %arr.data438, i64 14
  store i64 0, ptr %arr.elem439, align 8
  %arr.data.gep440 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data441 = load ptr, ptr %arr.data.gep440, align 8
  %arr.elem442 = getelementptr i64, ptr %arr.data441, i64 15
  store i64 0, ptr %arr.elem442, align 8
  %arr.data.gep443 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data444 = load ptr, ptr %arr.data.gep443, align 8
  %arr.elem445 = getelementptr i64, ptr %arr.data444, i64 16
  store i64 0, ptr %arr.elem445, align 8
  %arr.data.gep446 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data447 = load ptr, ptr %arr.data.gep446, align 8
  %arr.elem448 = getelementptr i64, ptr %arr.data447, i64 17
  store i64 0, ptr %arr.elem448, align 8
  %arr.data.gep449 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data450 = load ptr, ptr %arr.data.gep449, align 8
  %arr.elem451 = getelementptr i64, ptr %arr.data450, i64 18
  store i64 0, ptr %arr.elem451, align 8
  %arr.data.gep452 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data453 = load ptr, ptr %arr.data.gep452, align 8
  %arr.elem454 = getelementptr i64, ptr %arr.data453, i64 19
  store i64 0, ptr %arr.elem454, align 8
  %arr.data.gep455 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data456 = load ptr, ptr %arr.data.gep455, align 8
  %arr.elem457 = getelementptr i64, ptr %arr.data456, i64 20
  store i64 0, ptr %arr.elem457, align 8
  %arr.data.gep458 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data459 = load ptr, ptr %arr.data.gep458, align 8
  %arr.elem460 = getelementptr i64, ptr %arr.data459, i64 21
  store i64 0, ptr %arr.elem460, align 8
  %arr.data.gep461 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data462 = load ptr, ptr %arr.data.gep461, align 8
  %arr.elem463 = getelementptr i64, ptr %arr.data462, i64 22
  store i64 0, ptr %arr.elem463, align 8
  %arr.data.gep464 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data465 = load ptr, ptr %arr.data.gep464, align 8
  %arr.elem466 = getelementptr i64, ptr %arr.data465, i64 23
  store i64 0, ptr %arr.elem466, align 8
  %arr.data.gep467 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data468 = load ptr, ptr %arr.data.gep467, align 8
  %arr.elem469 = getelementptr i64, ptr %arr.data468, i64 24
  store i64 0, ptr %arr.elem469, align 8
  %arr.data.gep470 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data471 = load ptr, ptr %arr.data.gep470, align 8
  %arr.elem472 = getelementptr i64, ptr %arr.data471, i64 25
  store i64 0, ptr %arr.elem472, align 8
  %arr.data.gep473 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data474 = load ptr, ptr %arr.data.gep473, align 8
  %arr.elem475 = getelementptr i64, ptr %arr.data474, i64 26
  store i64 0, ptr %arr.elem475, align 8
  %arr.data.gep476 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data477 = load ptr, ptr %arr.data.gep476, align 8
  %arr.elem478 = getelementptr i64, ptr %arr.data477, i64 27
  store i64 0, ptr %arr.elem478, align 8
  %arr.data.gep479 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data480 = load ptr, ptr %arr.data.gep479, align 8
  %arr.elem481 = getelementptr i64, ptr %arr.data480, i64 28
  store i64 0, ptr %arr.elem481, align 8
  %arr.data.gep482 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data483 = load ptr, ptr %arr.data.gep482, align 8
  %arr.elem484 = getelementptr i64, ptr %arr.data483, i64 29
  store i64 0, ptr %arr.elem484, align 8
  %arr.data.gep485 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data486 = load ptr, ptr %arr.data.gep485, align 8
  %arr.elem487 = getelementptr i64, ptr %arr.data486, i64 30
  store i64 0, ptr %arr.elem487, align 8
  %arr.data.gep488 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data489 = load ptr, ptr %arr.data.gep488, align 8
  %arr.elem490 = getelementptr i64, ptr %arr.data489, i64 31
  store i64 0, ptr %arr.elem490, align 8
  %arr.data.gep491 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data492 = load ptr, ptr %arr.data.gep491, align 8
  %arr.elem493 = getelementptr i64, ptr %arr.data492, i64 32
  store i64 0, ptr %arr.elem493, align 8
  %arr.data.gep494 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data495 = load ptr, ptr %arr.data.gep494, align 8
  %arr.elem496 = getelementptr i64, ptr %arr.data495, i64 33
  store i64 0, ptr %arr.elem496, align 8
  %arr.data.gep497 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data498 = load ptr, ptr %arr.data.gep497, align 8
  %arr.elem499 = getelementptr i64, ptr %arr.data498, i64 34
  store i64 0, ptr %arr.elem499, align 8
  %arr.data.gep500 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data501 = load ptr, ptr %arr.data.gep500, align 8
  %arr.elem502 = getelementptr i64, ptr %arr.data501, i64 35
  store i64 0, ptr %arr.elem502, align 8
  %arr.data.gep503 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data504 = load ptr, ptr %arr.data.gep503, align 8
  %arr.elem505 = getelementptr i64, ptr %arr.data504, i64 36
  store i64 0, ptr %arr.elem505, align 8
  %arr.data.gep506 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data507 = load ptr, ptr %arr.data.gep506, align 8
  %arr.elem508 = getelementptr i64, ptr %arr.data507, i64 37
  store i64 0, ptr %arr.elem508, align 8
  %arr.data.gep509 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data510 = load ptr, ptr %arr.data.gep509, align 8
  %arr.elem511 = getelementptr i64, ptr %arr.data510, i64 38
  store i64 0, ptr %arr.elem511, align 8
  %arr.data.gep512 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data513 = load ptr, ptr %arr.data.gep512, align 8
  %arr.elem514 = getelementptr i64, ptr %arr.data513, i64 39
  store i64 0, ptr %arr.elem514, align 8
  %arr.data.gep515 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data516 = load ptr, ptr %arr.data.gep515, align 8
  %arr.elem517 = getelementptr i64, ptr %arr.data516, i64 40
  store i64 0, ptr %arr.elem517, align 8
  %arr.data.gep518 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data519 = load ptr, ptr %arr.data.gep518, align 8
  %arr.elem520 = getelementptr i64, ptr %arr.data519, i64 41
  store i64 0, ptr %arr.elem520, align 8
  %arr.data.gep521 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data522 = load ptr, ptr %arr.data.gep521, align 8
  %arr.elem523 = getelementptr i64, ptr %arr.data522, i64 42
  store i64 0, ptr %arr.elem523, align 8
  %arr.data.gep524 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data525 = load ptr, ptr %arr.data.gep524, align 8
  %arr.elem526 = getelementptr i64, ptr %arr.data525, i64 43
  store i64 0, ptr %arr.elem526, align 8
  %arr.data.gep527 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data528 = load ptr, ptr %arr.data.gep527, align 8
  %arr.elem529 = getelementptr i64, ptr %arr.data528, i64 44
  store i64 0, ptr %arr.elem529, align 8
  %arr.data.gep530 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data531 = load ptr, ptr %arr.data.gep530, align 8
  %arr.elem532 = getelementptr i64, ptr %arr.data531, i64 45
  store i64 0, ptr %arr.elem532, align 8
  %arr.data.gep533 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data534 = load ptr, ptr %arr.data.gep533, align 8
  %arr.elem535 = getelementptr i64, ptr %arr.data534, i64 46
  store i64 0, ptr %arr.elem535, align 8
  %arr.data.gep536 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data537 = load ptr, ptr %arr.data.gep536, align 8
  %arr.elem538 = getelementptr i64, ptr %arr.data537, i64 47
  store i64 0, ptr %arr.elem538, align 8
  %arr.data.gep539 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data540 = load ptr, ptr %arr.data.gep539, align 8
  %arr.elem541 = getelementptr i64, ptr %arr.data540, i64 48
  store i64 0, ptr %arr.elem541, align 8
  %arr.data.gep542 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data543 = load ptr, ptr %arr.data.gep542, align 8
  %arr.elem544 = getelementptr i64, ptr %arr.data543, i64 49
  store i64 0, ptr %arr.elem544, align 8
  %arr.data.gep545 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data546 = load ptr, ptr %arr.data.gep545, align 8
  %arr.elem547 = getelementptr i64, ptr %arr.data546, i64 50
  store i64 0, ptr %arr.elem547, align 8
  %arr.data.gep548 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data549 = load ptr, ptr %arr.data.gep548, align 8
  %arr.elem550 = getelementptr i64, ptr %arr.data549, i64 51
  store i64 0, ptr %arr.elem550, align 8
  %arr.data.gep551 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data552 = load ptr, ptr %arr.data.gep551, align 8
  %arr.elem553 = getelementptr i64, ptr %arr.data552, i64 52
  store i64 0, ptr %arr.elem553, align 8
  %arr.data.gep554 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data555 = load ptr, ptr %arr.data.gep554, align 8
  %arr.elem556 = getelementptr i64, ptr %arr.data555, i64 53
  store i64 0, ptr %arr.elem556, align 8
  %arr.data.gep557 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data558 = load ptr, ptr %arr.data.gep557, align 8
  %arr.elem559 = getelementptr i64, ptr %arr.data558, i64 54
  store i64 0, ptr %arr.elem559, align 8
  %arr.data.gep560 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data561 = load ptr, ptr %arr.data.gep560, align 8
  %arr.elem562 = getelementptr i64, ptr %arr.data561, i64 55
  store i64 0, ptr %arr.elem562, align 8
  %arr.data.gep563 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data564 = load ptr, ptr %arr.data.gep563, align 8
  %arr.elem565 = getelementptr i64, ptr %arr.data564, i64 56
  store i64 0, ptr %arr.elem565, align 8
  %arr.data.gep566 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data567 = load ptr, ptr %arr.data.gep566, align 8
  %arr.elem568 = getelementptr i64, ptr %arr.data567, i64 57
  store i64 0, ptr %arr.elem568, align 8
  %arr.data.gep569 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data570 = load ptr, ptr %arr.data.gep569, align 8
  %arr.elem571 = getelementptr i64, ptr %arr.data570, i64 58
  store i64 0, ptr %arr.elem571, align 8
  %arr.data.gep572 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data573 = load ptr, ptr %arr.data.gep572, align 8
  %arr.elem574 = getelementptr i64, ptr %arr.data573, i64 59
  store i64 0, ptr %arr.elem574, align 8
  %arr.data.gep575 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data576 = load ptr, ptr %arr.data.gep575, align 8
  %arr.elem577 = getelementptr i64, ptr %arr.data576, i64 60
  store i64 0, ptr %arr.elem577, align 8
  %arr.data.gep578 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data579 = load ptr, ptr %arr.data.gep578, align 8
  %arr.elem580 = getelementptr i64, ptr %arr.data579, i64 61
  store i64 0, ptr %arr.elem580, align 8
  %arr.data.gep581 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data582 = load ptr, ptr %arr.data.gep581, align 8
  %arr.elem583 = getelementptr i64, ptr %arr.data582, i64 62
  store i64 0, ptr %arr.elem583, align 8
  %arr.data.gep584 = getelementptr inbounds %SadArray, ptr %arr_new390, i32 0, i32 2
  %arr.data585 = load ptr, ptr %arr.data.gep584, align 8
  %arr.elem586 = getelementptr i64, ptr %arr.data585, i64 63
  store i64 0, ptr %arr.elem586, align 8
  %obj.ptrtoint587 = ptrtoint ptr %arr_new390 to i64
  store i64 %obj.ptrtoint587, ptr %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\A3\D9\88\D9\84\D9\88\D9\8A\D8\A9", align 8
  %arr_new588 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep589 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 0
  store i64 64, ptr %arr.len.gep589, align 8
  %arr.cap.gep590 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep590, align 8
  %arr.data591 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep592 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  store ptr %arr.data591, ptr %arr.data.gep592, align 8
  %arr.data.gep593 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data594 = load ptr, ptr %arr.data.gep593, align 8
  %arr.elem595 = getelementptr i64, ptr %arr.data594, i64 0
  store i64 0, ptr %arr.elem595, align 8
  %arr.data.gep596 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data597 = load ptr, ptr %arr.data.gep596, align 8
  %arr.elem598 = getelementptr i64, ptr %arr.data597, i64 1
  store i64 0, ptr %arr.elem598, align 8
  %arr.data.gep599 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data600 = load ptr, ptr %arr.data.gep599, align 8
  %arr.elem601 = getelementptr i64, ptr %arr.data600, i64 2
  store i64 0, ptr %arr.elem601, align 8
  %arr.data.gep602 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data603 = load ptr, ptr %arr.data.gep602, align 8
  %arr.elem604 = getelementptr i64, ptr %arr.data603, i64 3
  store i64 0, ptr %arr.elem604, align 8
  %arr.data.gep605 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data606 = load ptr, ptr %arr.data.gep605, align 8
  %arr.elem607 = getelementptr i64, ptr %arr.data606, i64 4
  store i64 0, ptr %arr.elem607, align 8
  %arr.data.gep608 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data609 = load ptr, ptr %arr.data.gep608, align 8
  %arr.elem610 = getelementptr i64, ptr %arr.data609, i64 5
  store i64 0, ptr %arr.elem610, align 8
  %arr.data.gep611 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data612 = load ptr, ptr %arr.data.gep611, align 8
  %arr.elem613 = getelementptr i64, ptr %arr.data612, i64 6
  store i64 0, ptr %arr.elem613, align 8
  %arr.data.gep614 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data615 = load ptr, ptr %arr.data.gep614, align 8
  %arr.elem616 = getelementptr i64, ptr %arr.data615, i64 7
  store i64 0, ptr %arr.elem616, align 8
  %arr.data.gep617 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data618 = load ptr, ptr %arr.data.gep617, align 8
  %arr.elem619 = getelementptr i64, ptr %arr.data618, i64 8
  store i64 0, ptr %arr.elem619, align 8
  %arr.data.gep620 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data621 = load ptr, ptr %arr.data.gep620, align 8
  %arr.elem622 = getelementptr i64, ptr %arr.data621, i64 9
  store i64 0, ptr %arr.elem622, align 8
  %arr.data.gep623 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data624 = load ptr, ptr %arr.data.gep623, align 8
  %arr.elem625 = getelementptr i64, ptr %arr.data624, i64 10
  store i64 0, ptr %arr.elem625, align 8
  %arr.data.gep626 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data627 = load ptr, ptr %arr.data.gep626, align 8
  %arr.elem628 = getelementptr i64, ptr %arr.data627, i64 11
  store i64 0, ptr %arr.elem628, align 8
  %arr.data.gep629 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data630 = load ptr, ptr %arr.data.gep629, align 8
  %arr.elem631 = getelementptr i64, ptr %arr.data630, i64 12
  store i64 0, ptr %arr.elem631, align 8
  %arr.data.gep632 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data633 = load ptr, ptr %arr.data.gep632, align 8
  %arr.elem634 = getelementptr i64, ptr %arr.data633, i64 13
  store i64 0, ptr %arr.elem634, align 8
  %arr.data.gep635 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data636 = load ptr, ptr %arr.data.gep635, align 8
  %arr.elem637 = getelementptr i64, ptr %arr.data636, i64 14
  store i64 0, ptr %arr.elem637, align 8
  %arr.data.gep638 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data639 = load ptr, ptr %arr.data.gep638, align 8
  %arr.elem640 = getelementptr i64, ptr %arr.data639, i64 15
  store i64 0, ptr %arr.elem640, align 8
  %arr.data.gep641 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data642 = load ptr, ptr %arr.data.gep641, align 8
  %arr.elem643 = getelementptr i64, ptr %arr.data642, i64 16
  store i64 0, ptr %arr.elem643, align 8
  %arr.data.gep644 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data645 = load ptr, ptr %arr.data.gep644, align 8
  %arr.elem646 = getelementptr i64, ptr %arr.data645, i64 17
  store i64 0, ptr %arr.elem646, align 8
  %arr.data.gep647 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data648 = load ptr, ptr %arr.data.gep647, align 8
  %arr.elem649 = getelementptr i64, ptr %arr.data648, i64 18
  store i64 0, ptr %arr.elem649, align 8
  %arr.data.gep650 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data651 = load ptr, ptr %arr.data.gep650, align 8
  %arr.elem652 = getelementptr i64, ptr %arr.data651, i64 19
  store i64 0, ptr %arr.elem652, align 8
  %arr.data.gep653 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data654 = load ptr, ptr %arr.data.gep653, align 8
  %arr.elem655 = getelementptr i64, ptr %arr.data654, i64 20
  store i64 0, ptr %arr.elem655, align 8
  %arr.data.gep656 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data657 = load ptr, ptr %arr.data.gep656, align 8
  %arr.elem658 = getelementptr i64, ptr %arr.data657, i64 21
  store i64 0, ptr %arr.elem658, align 8
  %arr.data.gep659 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data660 = load ptr, ptr %arr.data.gep659, align 8
  %arr.elem661 = getelementptr i64, ptr %arr.data660, i64 22
  store i64 0, ptr %arr.elem661, align 8
  %arr.data.gep662 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data663 = load ptr, ptr %arr.data.gep662, align 8
  %arr.elem664 = getelementptr i64, ptr %arr.data663, i64 23
  store i64 0, ptr %arr.elem664, align 8
  %arr.data.gep665 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data666 = load ptr, ptr %arr.data.gep665, align 8
  %arr.elem667 = getelementptr i64, ptr %arr.data666, i64 24
  store i64 0, ptr %arr.elem667, align 8
  %arr.data.gep668 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data669 = load ptr, ptr %arr.data.gep668, align 8
  %arr.elem670 = getelementptr i64, ptr %arr.data669, i64 25
  store i64 0, ptr %arr.elem670, align 8
  %arr.data.gep671 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data672 = load ptr, ptr %arr.data.gep671, align 8
  %arr.elem673 = getelementptr i64, ptr %arr.data672, i64 26
  store i64 0, ptr %arr.elem673, align 8
  %arr.data.gep674 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data675 = load ptr, ptr %arr.data.gep674, align 8
  %arr.elem676 = getelementptr i64, ptr %arr.data675, i64 27
  store i64 0, ptr %arr.elem676, align 8
  %arr.data.gep677 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data678 = load ptr, ptr %arr.data.gep677, align 8
  %arr.elem679 = getelementptr i64, ptr %arr.data678, i64 28
  store i64 0, ptr %arr.elem679, align 8
  %arr.data.gep680 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data681 = load ptr, ptr %arr.data.gep680, align 8
  %arr.elem682 = getelementptr i64, ptr %arr.data681, i64 29
  store i64 0, ptr %arr.elem682, align 8
  %arr.data.gep683 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data684 = load ptr, ptr %arr.data.gep683, align 8
  %arr.elem685 = getelementptr i64, ptr %arr.data684, i64 30
  store i64 0, ptr %arr.elem685, align 8
  %arr.data.gep686 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data687 = load ptr, ptr %arr.data.gep686, align 8
  %arr.elem688 = getelementptr i64, ptr %arr.data687, i64 31
  store i64 0, ptr %arr.elem688, align 8
  %arr.data.gep689 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data690 = load ptr, ptr %arr.data.gep689, align 8
  %arr.elem691 = getelementptr i64, ptr %arr.data690, i64 32
  store i64 0, ptr %arr.elem691, align 8
  %arr.data.gep692 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data693 = load ptr, ptr %arr.data.gep692, align 8
  %arr.elem694 = getelementptr i64, ptr %arr.data693, i64 33
  store i64 0, ptr %arr.elem694, align 8
  %arr.data.gep695 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data696 = load ptr, ptr %arr.data.gep695, align 8
  %arr.elem697 = getelementptr i64, ptr %arr.data696, i64 34
  store i64 0, ptr %arr.elem697, align 8
  %arr.data.gep698 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data699 = load ptr, ptr %arr.data.gep698, align 8
  %arr.elem700 = getelementptr i64, ptr %arr.data699, i64 35
  store i64 0, ptr %arr.elem700, align 8
  %arr.data.gep701 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data702 = load ptr, ptr %arr.data.gep701, align 8
  %arr.elem703 = getelementptr i64, ptr %arr.data702, i64 36
  store i64 0, ptr %arr.elem703, align 8
  %arr.data.gep704 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data705 = load ptr, ptr %arr.data.gep704, align 8
  %arr.elem706 = getelementptr i64, ptr %arr.data705, i64 37
  store i64 0, ptr %arr.elem706, align 8
  %arr.data.gep707 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data708 = load ptr, ptr %arr.data.gep707, align 8
  %arr.elem709 = getelementptr i64, ptr %arr.data708, i64 38
  store i64 0, ptr %arr.elem709, align 8
  %arr.data.gep710 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data711 = load ptr, ptr %arr.data.gep710, align 8
  %arr.elem712 = getelementptr i64, ptr %arr.data711, i64 39
  store i64 0, ptr %arr.elem712, align 8
  %arr.data.gep713 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data714 = load ptr, ptr %arr.data.gep713, align 8
  %arr.elem715 = getelementptr i64, ptr %arr.data714, i64 40
  store i64 0, ptr %arr.elem715, align 8
  %arr.data.gep716 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data717 = load ptr, ptr %arr.data.gep716, align 8
  %arr.elem718 = getelementptr i64, ptr %arr.data717, i64 41
  store i64 0, ptr %arr.elem718, align 8
  %arr.data.gep719 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data720 = load ptr, ptr %arr.data.gep719, align 8
  %arr.elem721 = getelementptr i64, ptr %arr.data720, i64 42
  store i64 0, ptr %arr.elem721, align 8
  %arr.data.gep722 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data723 = load ptr, ptr %arr.data.gep722, align 8
  %arr.elem724 = getelementptr i64, ptr %arr.data723, i64 43
  store i64 0, ptr %arr.elem724, align 8
  %arr.data.gep725 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data726 = load ptr, ptr %arr.data.gep725, align 8
  %arr.elem727 = getelementptr i64, ptr %arr.data726, i64 44
  store i64 0, ptr %arr.elem727, align 8
  %arr.data.gep728 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data729 = load ptr, ptr %arr.data.gep728, align 8
  %arr.elem730 = getelementptr i64, ptr %arr.data729, i64 45
  store i64 0, ptr %arr.elem730, align 8
  %arr.data.gep731 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data732 = load ptr, ptr %arr.data.gep731, align 8
  %arr.elem733 = getelementptr i64, ptr %arr.data732, i64 46
  store i64 0, ptr %arr.elem733, align 8
  %arr.data.gep734 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data735 = load ptr, ptr %arr.data.gep734, align 8
  %arr.elem736 = getelementptr i64, ptr %arr.data735, i64 47
  store i64 0, ptr %arr.elem736, align 8
  %arr.data.gep737 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data738 = load ptr, ptr %arr.data.gep737, align 8
  %arr.elem739 = getelementptr i64, ptr %arr.data738, i64 48
  store i64 0, ptr %arr.elem739, align 8
  %arr.data.gep740 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data741 = load ptr, ptr %arr.data.gep740, align 8
  %arr.elem742 = getelementptr i64, ptr %arr.data741, i64 49
  store i64 0, ptr %arr.elem742, align 8
  %arr.data.gep743 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data744 = load ptr, ptr %arr.data.gep743, align 8
  %arr.elem745 = getelementptr i64, ptr %arr.data744, i64 50
  store i64 0, ptr %arr.elem745, align 8
  %arr.data.gep746 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data747 = load ptr, ptr %arr.data.gep746, align 8
  %arr.elem748 = getelementptr i64, ptr %arr.data747, i64 51
  store i64 0, ptr %arr.elem748, align 8
  %arr.data.gep749 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data750 = load ptr, ptr %arr.data.gep749, align 8
  %arr.elem751 = getelementptr i64, ptr %arr.data750, i64 52
  store i64 0, ptr %arr.elem751, align 8
  %arr.data.gep752 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data753 = load ptr, ptr %arr.data.gep752, align 8
  %arr.elem754 = getelementptr i64, ptr %arr.data753, i64 53
  store i64 0, ptr %arr.elem754, align 8
  %arr.data.gep755 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data756 = load ptr, ptr %arr.data.gep755, align 8
  %arr.elem757 = getelementptr i64, ptr %arr.data756, i64 54
  store i64 0, ptr %arr.elem757, align 8
  %arr.data.gep758 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data759 = load ptr, ptr %arr.data.gep758, align 8
  %arr.elem760 = getelementptr i64, ptr %arr.data759, i64 55
  store i64 0, ptr %arr.elem760, align 8
  %arr.data.gep761 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data762 = load ptr, ptr %arr.data.gep761, align 8
  %arr.elem763 = getelementptr i64, ptr %arr.data762, i64 56
  store i64 0, ptr %arr.elem763, align 8
  %arr.data.gep764 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data765 = load ptr, ptr %arr.data.gep764, align 8
  %arr.elem766 = getelementptr i64, ptr %arr.data765, i64 57
  store i64 0, ptr %arr.elem766, align 8
  %arr.data.gep767 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data768 = load ptr, ptr %arr.data.gep767, align 8
  %arr.elem769 = getelementptr i64, ptr %arr.data768, i64 58
  store i64 0, ptr %arr.elem769, align 8
  %arr.data.gep770 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data771 = load ptr, ptr %arr.data.gep770, align 8
  %arr.elem772 = getelementptr i64, ptr %arr.data771, i64 59
  store i64 0, ptr %arr.elem772, align 8
  %arr.data.gep773 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data774 = load ptr, ptr %arr.data.gep773, align 8
  %arr.elem775 = getelementptr i64, ptr %arr.data774, i64 60
  store i64 0, ptr %arr.elem775, align 8
  %arr.data.gep776 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data777 = load ptr, ptr %arr.data.gep776, align 8
  %arr.elem778 = getelementptr i64, ptr %arr.data777, i64 61
  store i64 0, ptr %arr.elem778, align 8
  %arr.data.gep779 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data780 = load ptr, ptr %arr.data.gep779, align 8
  %arr.elem781 = getelementptr i64, ptr %arr.data780, i64 62
  store i64 0, ptr %arr.elem781, align 8
  %arr.data.gep782 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data783 = load ptr, ptr %arr.data.gep782, align 8
  %arr.elem784 = getelementptr i64, ptr %arr.data783, i64 63
  store i64 0, ptr %arr.elem784, align 8
  %obj.ptrtoint785 = ptrtoint ptr %arr_new588 to i64
  store i64 %obj.ptrtoint785, ptr %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\B4\D8\B1\D8\A7\D8\A6\D8\AD", align 8
  %arr_new786 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep787 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 0
  store i64 64, ptr %arr.len.gep787, align 8
  %arr.cap.gep788 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep788, align 8
  %arr.data789 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep790 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  store ptr %arr.data789, ptr %arr.data.gep790, align 8
  %arr.data.gep791 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data792 = load ptr, ptr %arr.data.gep791, align 8
  %arr.elem793 = getelementptr i64, ptr %arr.data792, i64 0
  store i64 0, ptr %arr.elem793, align 8
  %arr.data.gep794 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data795 = load ptr, ptr %arr.data.gep794, align 8
  %arr.elem796 = getelementptr i64, ptr %arr.data795, i64 1
  store i64 0, ptr %arr.elem796, align 8
  %arr.data.gep797 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data798 = load ptr, ptr %arr.data.gep797, align 8
  %arr.elem799 = getelementptr i64, ptr %arr.data798, i64 2
  store i64 0, ptr %arr.elem799, align 8
  %arr.data.gep800 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data801 = load ptr, ptr %arr.data.gep800, align 8
  %arr.elem802 = getelementptr i64, ptr %arr.data801, i64 3
  store i64 0, ptr %arr.elem802, align 8
  %arr.data.gep803 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data804 = load ptr, ptr %arr.data.gep803, align 8
  %arr.elem805 = getelementptr i64, ptr %arr.data804, i64 4
  store i64 0, ptr %arr.elem805, align 8
  %arr.data.gep806 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data807 = load ptr, ptr %arr.data.gep806, align 8
  %arr.elem808 = getelementptr i64, ptr %arr.data807, i64 5
  store i64 0, ptr %arr.elem808, align 8
  %arr.data.gep809 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data810 = load ptr, ptr %arr.data.gep809, align 8
  %arr.elem811 = getelementptr i64, ptr %arr.data810, i64 6
  store i64 0, ptr %arr.elem811, align 8
  %arr.data.gep812 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data813 = load ptr, ptr %arr.data.gep812, align 8
  %arr.elem814 = getelementptr i64, ptr %arr.data813, i64 7
  store i64 0, ptr %arr.elem814, align 8
  %arr.data.gep815 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data816 = load ptr, ptr %arr.data.gep815, align 8
  %arr.elem817 = getelementptr i64, ptr %arr.data816, i64 8
  store i64 0, ptr %arr.elem817, align 8
  %arr.data.gep818 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data819 = load ptr, ptr %arr.data.gep818, align 8
  %arr.elem820 = getelementptr i64, ptr %arr.data819, i64 9
  store i64 0, ptr %arr.elem820, align 8
  %arr.data.gep821 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data822 = load ptr, ptr %arr.data.gep821, align 8
  %arr.elem823 = getelementptr i64, ptr %arr.data822, i64 10
  store i64 0, ptr %arr.elem823, align 8
  %arr.data.gep824 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data825 = load ptr, ptr %arr.data.gep824, align 8
  %arr.elem826 = getelementptr i64, ptr %arr.data825, i64 11
  store i64 0, ptr %arr.elem826, align 8
  %arr.data.gep827 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data828 = load ptr, ptr %arr.data.gep827, align 8
  %arr.elem829 = getelementptr i64, ptr %arr.data828, i64 12
  store i64 0, ptr %arr.elem829, align 8
  %arr.data.gep830 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data831 = load ptr, ptr %arr.data.gep830, align 8
  %arr.elem832 = getelementptr i64, ptr %arr.data831, i64 13
  store i64 0, ptr %arr.elem832, align 8
  %arr.data.gep833 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data834 = load ptr, ptr %arr.data.gep833, align 8
  %arr.elem835 = getelementptr i64, ptr %arr.data834, i64 14
  store i64 0, ptr %arr.elem835, align 8
  %arr.data.gep836 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data837 = load ptr, ptr %arr.data.gep836, align 8
  %arr.elem838 = getelementptr i64, ptr %arr.data837, i64 15
  store i64 0, ptr %arr.elem838, align 8
  %arr.data.gep839 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data840 = load ptr, ptr %arr.data.gep839, align 8
  %arr.elem841 = getelementptr i64, ptr %arr.data840, i64 16
  store i64 0, ptr %arr.elem841, align 8
  %arr.data.gep842 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data843 = load ptr, ptr %arr.data.gep842, align 8
  %arr.elem844 = getelementptr i64, ptr %arr.data843, i64 17
  store i64 0, ptr %arr.elem844, align 8
  %arr.data.gep845 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data846 = load ptr, ptr %arr.data.gep845, align 8
  %arr.elem847 = getelementptr i64, ptr %arr.data846, i64 18
  store i64 0, ptr %arr.elem847, align 8
  %arr.data.gep848 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data849 = load ptr, ptr %arr.data.gep848, align 8
  %arr.elem850 = getelementptr i64, ptr %arr.data849, i64 19
  store i64 0, ptr %arr.elem850, align 8
  %arr.data.gep851 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data852 = load ptr, ptr %arr.data.gep851, align 8
  %arr.elem853 = getelementptr i64, ptr %arr.data852, i64 20
  store i64 0, ptr %arr.elem853, align 8
  %arr.data.gep854 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data855 = load ptr, ptr %arr.data.gep854, align 8
  %arr.elem856 = getelementptr i64, ptr %arr.data855, i64 21
  store i64 0, ptr %arr.elem856, align 8
  %arr.data.gep857 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data858 = load ptr, ptr %arr.data.gep857, align 8
  %arr.elem859 = getelementptr i64, ptr %arr.data858, i64 22
  store i64 0, ptr %arr.elem859, align 8
  %arr.data.gep860 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data861 = load ptr, ptr %arr.data.gep860, align 8
  %arr.elem862 = getelementptr i64, ptr %arr.data861, i64 23
  store i64 0, ptr %arr.elem862, align 8
  %arr.data.gep863 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data864 = load ptr, ptr %arr.data.gep863, align 8
  %arr.elem865 = getelementptr i64, ptr %arr.data864, i64 24
  store i64 0, ptr %arr.elem865, align 8
  %arr.data.gep866 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data867 = load ptr, ptr %arr.data.gep866, align 8
  %arr.elem868 = getelementptr i64, ptr %arr.data867, i64 25
  store i64 0, ptr %arr.elem868, align 8
  %arr.data.gep869 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data870 = load ptr, ptr %arr.data.gep869, align 8
  %arr.elem871 = getelementptr i64, ptr %arr.data870, i64 26
  store i64 0, ptr %arr.elem871, align 8
  %arr.data.gep872 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data873 = load ptr, ptr %arr.data.gep872, align 8
  %arr.elem874 = getelementptr i64, ptr %arr.data873, i64 27
  store i64 0, ptr %arr.elem874, align 8
  %arr.data.gep875 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data876 = load ptr, ptr %arr.data.gep875, align 8
  %arr.elem877 = getelementptr i64, ptr %arr.data876, i64 28
  store i64 0, ptr %arr.elem877, align 8
  %arr.data.gep878 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data879 = load ptr, ptr %arr.data.gep878, align 8
  %arr.elem880 = getelementptr i64, ptr %arr.data879, i64 29
  store i64 0, ptr %arr.elem880, align 8
  %arr.data.gep881 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data882 = load ptr, ptr %arr.data.gep881, align 8
  %arr.elem883 = getelementptr i64, ptr %arr.data882, i64 30
  store i64 0, ptr %arr.elem883, align 8
  %arr.data.gep884 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data885 = load ptr, ptr %arr.data.gep884, align 8
  %arr.elem886 = getelementptr i64, ptr %arr.data885, i64 31
  store i64 0, ptr %arr.elem886, align 8
  %arr.data.gep887 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data888 = load ptr, ptr %arr.data.gep887, align 8
  %arr.elem889 = getelementptr i64, ptr %arr.data888, i64 32
  store i64 0, ptr %arr.elem889, align 8
  %arr.data.gep890 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data891 = load ptr, ptr %arr.data.gep890, align 8
  %arr.elem892 = getelementptr i64, ptr %arr.data891, i64 33
  store i64 0, ptr %arr.elem892, align 8
  %arr.data.gep893 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data894 = load ptr, ptr %arr.data.gep893, align 8
  %arr.elem895 = getelementptr i64, ptr %arr.data894, i64 34
  store i64 0, ptr %arr.elem895, align 8
  %arr.data.gep896 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data897 = load ptr, ptr %arr.data.gep896, align 8
  %arr.elem898 = getelementptr i64, ptr %arr.data897, i64 35
  store i64 0, ptr %arr.elem898, align 8
  %arr.data.gep899 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data900 = load ptr, ptr %arr.data.gep899, align 8
  %arr.elem901 = getelementptr i64, ptr %arr.data900, i64 36
  store i64 0, ptr %arr.elem901, align 8
  %arr.data.gep902 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data903 = load ptr, ptr %arr.data.gep902, align 8
  %arr.elem904 = getelementptr i64, ptr %arr.data903, i64 37
  store i64 0, ptr %arr.elem904, align 8
  %arr.data.gep905 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data906 = load ptr, ptr %arr.data.gep905, align 8
  %arr.elem907 = getelementptr i64, ptr %arr.data906, i64 38
  store i64 0, ptr %arr.elem907, align 8
  %arr.data.gep908 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data909 = load ptr, ptr %arr.data.gep908, align 8
  %arr.elem910 = getelementptr i64, ptr %arr.data909, i64 39
  store i64 0, ptr %arr.elem910, align 8
  %arr.data.gep911 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data912 = load ptr, ptr %arr.data.gep911, align 8
  %arr.elem913 = getelementptr i64, ptr %arr.data912, i64 40
  store i64 0, ptr %arr.elem913, align 8
  %arr.data.gep914 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data915 = load ptr, ptr %arr.data.gep914, align 8
  %arr.elem916 = getelementptr i64, ptr %arr.data915, i64 41
  store i64 0, ptr %arr.elem916, align 8
  %arr.data.gep917 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data918 = load ptr, ptr %arr.data.gep917, align 8
  %arr.elem919 = getelementptr i64, ptr %arr.data918, i64 42
  store i64 0, ptr %arr.elem919, align 8
  %arr.data.gep920 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data921 = load ptr, ptr %arr.data.gep920, align 8
  %arr.elem922 = getelementptr i64, ptr %arr.data921, i64 43
  store i64 0, ptr %arr.elem922, align 8
  %arr.data.gep923 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data924 = load ptr, ptr %arr.data.gep923, align 8
  %arr.elem925 = getelementptr i64, ptr %arr.data924, i64 44
  store i64 0, ptr %arr.elem925, align 8
  %arr.data.gep926 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data927 = load ptr, ptr %arr.data.gep926, align 8
  %arr.elem928 = getelementptr i64, ptr %arr.data927, i64 45
  store i64 0, ptr %arr.elem928, align 8
  %arr.data.gep929 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data930 = load ptr, ptr %arr.data.gep929, align 8
  %arr.elem931 = getelementptr i64, ptr %arr.data930, i64 46
  store i64 0, ptr %arr.elem931, align 8
  %arr.data.gep932 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data933 = load ptr, ptr %arr.data.gep932, align 8
  %arr.elem934 = getelementptr i64, ptr %arr.data933, i64 47
  store i64 0, ptr %arr.elem934, align 8
  %arr.data.gep935 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data936 = load ptr, ptr %arr.data.gep935, align 8
  %arr.elem937 = getelementptr i64, ptr %arr.data936, i64 48
  store i64 0, ptr %arr.elem937, align 8
  %arr.data.gep938 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data939 = load ptr, ptr %arr.data.gep938, align 8
  %arr.elem940 = getelementptr i64, ptr %arr.data939, i64 49
  store i64 0, ptr %arr.elem940, align 8
  %arr.data.gep941 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data942 = load ptr, ptr %arr.data.gep941, align 8
  %arr.elem943 = getelementptr i64, ptr %arr.data942, i64 50
  store i64 0, ptr %arr.elem943, align 8
  %arr.data.gep944 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data945 = load ptr, ptr %arr.data.gep944, align 8
  %arr.elem946 = getelementptr i64, ptr %arr.data945, i64 51
  store i64 0, ptr %arr.elem946, align 8
  %arr.data.gep947 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data948 = load ptr, ptr %arr.data.gep947, align 8
  %arr.elem949 = getelementptr i64, ptr %arr.data948, i64 52
  store i64 0, ptr %arr.elem949, align 8
  %arr.data.gep950 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data951 = load ptr, ptr %arr.data.gep950, align 8
  %arr.elem952 = getelementptr i64, ptr %arr.data951, i64 53
  store i64 0, ptr %arr.elem952, align 8
  %arr.data.gep953 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data954 = load ptr, ptr %arr.data.gep953, align 8
  %arr.elem955 = getelementptr i64, ptr %arr.data954, i64 54
  store i64 0, ptr %arr.elem955, align 8
  %arr.data.gep956 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data957 = load ptr, ptr %arr.data.gep956, align 8
  %arr.elem958 = getelementptr i64, ptr %arr.data957, i64 55
  store i64 0, ptr %arr.elem958, align 8
  %arr.data.gep959 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data960 = load ptr, ptr %arr.data.gep959, align 8
  %arr.elem961 = getelementptr i64, ptr %arr.data960, i64 56
  store i64 0, ptr %arr.elem961, align 8
  %arr.data.gep962 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data963 = load ptr, ptr %arr.data.gep962, align 8
  %arr.elem964 = getelementptr i64, ptr %arr.data963, i64 57
  store i64 0, ptr %arr.elem964, align 8
  %arr.data.gep965 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data966 = load ptr, ptr %arr.data.gep965, align 8
  %arr.elem967 = getelementptr i64, ptr %arr.data966, i64 58
  store i64 0, ptr %arr.elem967, align 8
  %arr.data.gep968 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data969 = load ptr, ptr %arr.data.gep968, align 8
  %arr.elem970 = getelementptr i64, ptr %arr.data969, i64 59
  store i64 0, ptr %arr.elem970, align 8
  %arr.data.gep971 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data972 = load ptr, ptr %arr.data.gep971, align 8
  %arr.elem973 = getelementptr i64, ptr %arr.data972, i64 60
  store i64 0, ptr %arr.elem973, align 8
  %arr.data.gep974 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data975 = load ptr, ptr %arr.data.gep974, align 8
  %arr.elem976 = getelementptr i64, ptr %arr.data975, i64 61
  store i64 0, ptr %arr.elem976, align 8
  %arr.data.gep977 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data978 = load ptr, ptr %arr.data.gep977, align 8
  %arr.elem979 = getelementptr i64, ptr %arr.data978, i64 62
  store i64 0, ptr %arr.elem979, align 8
  %arr.data.gep980 = getelementptr inbounds %SadArray, ptr %arr_new786, i32 0, i32 2
  %arr.data981 = load ptr, ptr %arr.data.gep980, align 8
  %arr.elem982 = getelementptr i64, ptr %arr.data981, i64 63
  store i64 0, ptr %arr.elem982, align 8
  %obj.ptrtoint983 = ptrtoint ptr %arr_new786 to i64
  store i64 %obj.ptrtoint983, ptr %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\A7\D8\B3\D8\AA\D9\8A\D9\82\D8\A7\D8\B8", align 8
  %arr_new984 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep985 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 0
  store i64 64, ptr %arr.len.gep985, align 8
  %arr.cap.gep986 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep986, align 8
  %arr.data987 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep988 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  store ptr %arr.data987, ptr %arr.data.gep988, align 8
  %arr.data.gep989 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data990 = load ptr, ptr %arr.data.gep989, align 8
  %arr.elem991 = getelementptr i64, ptr %arr.data990, i64 0
  store i64 0, ptr %arr.elem991, align 8
  %arr.data.gep992 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data993 = load ptr, ptr %arr.data.gep992, align 8
  %arr.elem994 = getelementptr i64, ptr %arr.data993, i64 1
  store i64 0, ptr %arr.elem994, align 8
  %arr.data.gep995 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data996 = load ptr, ptr %arr.data.gep995, align 8
  %arr.elem997 = getelementptr i64, ptr %arr.data996, i64 2
  store i64 0, ptr %arr.elem997, align 8
  %arr.data.gep998 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data999 = load ptr, ptr %arr.data.gep998, align 8
  %arr.elem1000 = getelementptr i64, ptr %arr.data999, i64 3
  store i64 0, ptr %arr.elem1000, align 8
  %arr.data.gep1001 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1002 = load ptr, ptr %arr.data.gep1001, align 8
  %arr.elem1003 = getelementptr i64, ptr %arr.data1002, i64 4
  store i64 0, ptr %arr.elem1003, align 8
  %arr.data.gep1004 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1005 = load ptr, ptr %arr.data.gep1004, align 8
  %arr.elem1006 = getelementptr i64, ptr %arr.data1005, i64 5
  store i64 0, ptr %arr.elem1006, align 8
  %arr.data.gep1007 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1008 = load ptr, ptr %arr.data.gep1007, align 8
  %arr.elem1009 = getelementptr i64, ptr %arr.data1008, i64 6
  store i64 0, ptr %arr.elem1009, align 8
  %arr.data.gep1010 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1011 = load ptr, ptr %arr.data.gep1010, align 8
  %arr.elem1012 = getelementptr i64, ptr %arr.data1011, i64 7
  store i64 0, ptr %arr.elem1012, align 8
  %arr.data.gep1013 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1014 = load ptr, ptr %arr.data.gep1013, align 8
  %arr.elem1015 = getelementptr i64, ptr %arr.data1014, i64 8
  store i64 0, ptr %arr.elem1015, align 8
  %arr.data.gep1016 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1017 = load ptr, ptr %arr.data.gep1016, align 8
  %arr.elem1018 = getelementptr i64, ptr %arr.data1017, i64 9
  store i64 0, ptr %arr.elem1018, align 8
  %arr.data.gep1019 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1020 = load ptr, ptr %arr.data.gep1019, align 8
  %arr.elem1021 = getelementptr i64, ptr %arr.data1020, i64 10
  store i64 0, ptr %arr.elem1021, align 8
  %arr.data.gep1022 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1023 = load ptr, ptr %arr.data.gep1022, align 8
  %arr.elem1024 = getelementptr i64, ptr %arr.data1023, i64 11
  store i64 0, ptr %arr.elem1024, align 8
  %arr.data.gep1025 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1026 = load ptr, ptr %arr.data.gep1025, align 8
  %arr.elem1027 = getelementptr i64, ptr %arr.data1026, i64 12
  store i64 0, ptr %arr.elem1027, align 8
  %arr.data.gep1028 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1029 = load ptr, ptr %arr.data.gep1028, align 8
  %arr.elem1030 = getelementptr i64, ptr %arr.data1029, i64 13
  store i64 0, ptr %arr.elem1030, align 8
  %arr.data.gep1031 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1032 = load ptr, ptr %arr.data.gep1031, align 8
  %arr.elem1033 = getelementptr i64, ptr %arr.data1032, i64 14
  store i64 0, ptr %arr.elem1033, align 8
  %arr.data.gep1034 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1035 = load ptr, ptr %arr.data.gep1034, align 8
  %arr.elem1036 = getelementptr i64, ptr %arr.data1035, i64 15
  store i64 0, ptr %arr.elem1036, align 8
  %arr.data.gep1037 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1038 = load ptr, ptr %arr.data.gep1037, align 8
  %arr.elem1039 = getelementptr i64, ptr %arr.data1038, i64 16
  store i64 0, ptr %arr.elem1039, align 8
  %arr.data.gep1040 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1041 = load ptr, ptr %arr.data.gep1040, align 8
  %arr.elem1042 = getelementptr i64, ptr %arr.data1041, i64 17
  store i64 0, ptr %arr.elem1042, align 8
  %arr.data.gep1043 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1044 = load ptr, ptr %arr.data.gep1043, align 8
  %arr.elem1045 = getelementptr i64, ptr %arr.data1044, i64 18
  store i64 0, ptr %arr.elem1045, align 8
  %arr.data.gep1046 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1047 = load ptr, ptr %arr.data.gep1046, align 8
  %arr.elem1048 = getelementptr i64, ptr %arr.data1047, i64 19
  store i64 0, ptr %arr.elem1048, align 8
  %arr.data.gep1049 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1050 = load ptr, ptr %arr.data.gep1049, align 8
  %arr.elem1051 = getelementptr i64, ptr %arr.data1050, i64 20
  store i64 0, ptr %arr.elem1051, align 8
  %arr.data.gep1052 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1053 = load ptr, ptr %arr.data.gep1052, align 8
  %arr.elem1054 = getelementptr i64, ptr %arr.data1053, i64 21
  store i64 0, ptr %arr.elem1054, align 8
  %arr.data.gep1055 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1056 = load ptr, ptr %arr.data.gep1055, align 8
  %arr.elem1057 = getelementptr i64, ptr %arr.data1056, i64 22
  store i64 0, ptr %arr.elem1057, align 8
  %arr.data.gep1058 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1059 = load ptr, ptr %arr.data.gep1058, align 8
  %arr.elem1060 = getelementptr i64, ptr %arr.data1059, i64 23
  store i64 0, ptr %arr.elem1060, align 8
  %arr.data.gep1061 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1062 = load ptr, ptr %arr.data.gep1061, align 8
  %arr.elem1063 = getelementptr i64, ptr %arr.data1062, i64 24
  store i64 0, ptr %arr.elem1063, align 8
  %arr.data.gep1064 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1065 = load ptr, ptr %arr.data.gep1064, align 8
  %arr.elem1066 = getelementptr i64, ptr %arr.data1065, i64 25
  store i64 0, ptr %arr.elem1066, align 8
  %arr.data.gep1067 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1068 = load ptr, ptr %arr.data.gep1067, align 8
  %arr.elem1069 = getelementptr i64, ptr %arr.data1068, i64 26
  store i64 0, ptr %arr.elem1069, align 8
  %arr.data.gep1070 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1071 = load ptr, ptr %arr.data.gep1070, align 8
  %arr.elem1072 = getelementptr i64, ptr %arr.data1071, i64 27
  store i64 0, ptr %arr.elem1072, align 8
  %arr.data.gep1073 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1074 = load ptr, ptr %arr.data.gep1073, align 8
  %arr.elem1075 = getelementptr i64, ptr %arr.data1074, i64 28
  store i64 0, ptr %arr.elem1075, align 8
  %arr.data.gep1076 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1077 = load ptr, ptr %arr.data.gep1076, align 8
  %arr.elem1078 = getelementptr i64, ptr %arr.data1077, i64 29
  store i64 0, ptr %arr.elem1078, align 8
  %arr.data.gep1079 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1080 = load ptr, ptr %arr.data.gep1079, align 8
  %arr.elem1081 = getelementptr i64, ptr %arr.data1080, i64 30
  store i64 0, ptr %arr.elem1081, align 8
  %arr.data.gep1082 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1083 = load ptr, ptr %arr.data.gep1082, align 8
  %arr.elem1084 = getelementptr i64, ptr %arr.data1083, i64 31
  store i64 0, ptr %arr.elem1084, align 8
  %arr.data.gep1085 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1086 = load ptr, ptr %arr.data.gep1085, align 8
  %arr.elem1087 = getelementptr i64, ptr %arr.data1086, i64 32
  store i64 0, ptr %arr.elem1087, align 8
  %arr.data.gep1088 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1089 = load ptr, ptr %arr.data.gep1088, align 8
  %arr.elem1090 = getelementptr i64, ptr %arr.data1089, i64 33
  store i64 0, ptr %arr.elem1090, align 8
  %arr.data.gep1091 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1092 = load ptr, ptr %arr.data.gep1091, align 8
  %arr.elem1093 = getelementptr i64, ptr %arr.data1092, i64 34
  store i64 0, ptr %arr.elem1093, align 8
  %arr.data.gep1094 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1095 = load ptr, ptr %arr.data.gep1094, align 8
  %arr.elem1096 = getelementptr i64, ptr %arr.data1095, i64 35
  store i64 0, ptr %arr.elem1096, align 8
  %arr.data.gep1097 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1098 = load ptr, ptr %arr.data.gep1097, align 8
  %arr.elem1099 = getelementptr i64, ptr %arr.data1098, i64 36
  store i64 0, ptr %arr.elem1099, align 8
  %arr.data.gep1100 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1101 = load ptr, ptr %arr.data.gep1100, align 8
  %arr.elem1102 = getelementptr i64, ptr %arr.data1101, i64 37
  store i64 0, ptr %arr.elem1102, align 8
  %arr.data.gep1103 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1104 = load ptr, ptr %arr.data.gep1103, align 8
  %arr.elem1105 = getelementptr i64, ptr %arr.data1104, i64 38
  store i64 0, ptr %arr.elem1105, align 8
  %arr.data.gep1106 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1107 = load ptr, ptr %arr.data.gep1106, align 8
  %arr.elem1108 = getelementptr i64, ptr %arr.data1107, i64 39
  store i64 0, ptr %arr.elem1108, align 8
  %arr.data.gep1109 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1110 = load ptr, ptr %arr.data.gep1109, align 8
  %arr.elem1111 = getelementptr i64, ptr %arr.data1110, i64 40
  store i64 0, ptr %arr.elem1111, align 8
  %arr.data.gep1112 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1113 = load ptr, ptr %arr.data.gep1112, align 8
  %arr.elem1114 = getelementptr i64, ptr %arr.data1113, i64 41
  store i64 0, ptr %arr.elem1114, align 8
  %arr.data.gep1115 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1116 = load ptr, ptr %arr.data.gep1115, align 8
  %arr.elem1117 = getelementptr i64, ptr %arr.data1116, i64 42
  store i64 0, ptr %arr.elem1117, align 8
  %arr.data.gep1118 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1119 = load ptr, ptr %arr.data.gep1118, align 8
  %arr.elem1120 = getelementptr i64, ptr %arr.data1119, i64 43
  store i64 0, ptr %arr.elem1120, align 8
  %arr.data.gep1121 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1122 = load ptr, ptr %arr.data.gep1121, align 8
  %arr.elem1123 = getelementptr i64, ptr %arr.data1122, i64 44
  store i64 0, ptr %arr.elem1123, align 8
  %arr.data.gep1124 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1125 = load ptr, ptr %arr.data.gep1124, align 8
  %arr.elem1126 = getelementptr i64, ptr %arr.data1125, i64 45
  store i64 0, ptr %arr.elem1126, align 8
  %arr.data.gep1127 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1128 = load ptr, ptr %arr.data.gep1127, align 8
  %arr.elem1129 = getelementptr i64, ptr %arr.data1128, i64 46
  store i64 0, ptr %arr.elem1129, align 8
  %arr.data.gep1130 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1131 = load ptr, ptr %arr.data.gep1130, align 8
  %arr.elem1132 = getelementptr i64, ptr %arr.data1131, i64 47
  store i64 0, ptr %arr.elem1132, align 8
  %arr.data.gep1133 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1134 = load ptr, ptr %arr.data.gep1133, align 8
  %arr.elem1135 = getelementptr i64, ptr %arr.data1134, i64 48
  store i64 0, ptr %arr.elem1135, align 8
  %arr.data.gep1136 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1137 = load ptr, ptr %arr.data.gep1136, align 8
  %arr.elem1138 = getelementptr i64, ptr %arr.data1137, i64 49
  store i64 0, ptr %arr.elem1138, align 8
  %arr.data.gep1139 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1140 = load ptr, ptr %arr.data.gep1139, align 8
  %arr.elem1141 = getelementptr i64, ptr %arr.data1140, i64 50
  store i64 0, ptr %arr.elem1141, align 8
  %arr.data.gep1142 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1143 = load ptr, ptr %arr.data.gep1142, align 8
  %arr.elem1144 = getelementptr i64, ptr %arr.data1143, i64 51
  store i64 0, ptr %arr.elem1144, align 8
  %arr.data.gep1145 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1146 = load ptr, ptr %arr.data.gep1145, align 8
  %arr.elem1147 = getelementptr i64, ptr %arr.data1146, i64 52
  store i64 0, ptr %arr.elem1147, align 8
  %arr.data.gep1148 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1149 = load ptr, ptr %arr.data.gep1148, align 8
  %arr.elem1150 = getelementptr i64, ptr %arr.data1149, i64 53
  store i64 0, ptr %arr.elem1150, align 8
  %arr.data.gep1151 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1152 = load ptr, ptr %arr.data.gep1151, align 8
  %arr.elem1153 = getelementptr i64, ptr %arr.data1152, i64 54
  store i64 0, ptr %arr.elem1153, align 8
  %arr.data.gep1154 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1155 = load ptr, ptr %arr.data.gep1154, align 8
  %arr.elem1156 = getelementptr i64, ptr %arr.data1155, i64 55
  store i64 0, ptr %arr.elem1156, align 8
  %arr.data.gep1157 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1158 = load ptr, ptr %arr.data.gep1157, align 8
  %arr.elem1159 = getelementptr i64, ptr %arr.data1158, i64 56
  store i64 0, ptr %arr.elem1159, align 8
  %arr.data.gep1160 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1161 = load ptr, ptr %arr.data.gep1160, align 8
  %arr.elem1162 = getelementptr i64, ptr %arr.data1161, i64 57
  store i64 0, ptr %arr.elem1162, align 8
  %arr.data.gep1163 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1164 = load ptr, ptr %arr.data.gep1163, align 8
  %arr.elem1165 = getelementptr i64, ptr %arr.data1164, i64 58
  store i64 0, ptr %arr.elem1165, align 8
  %arr.data.gep1166 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1167 = load ptr, ptr %arr.data.gep1166, align 8
  %arr.elem1168 = getelementptr i64, ptr %arr.data1167, i64 59
  store i64 0, ptr %arr.elem1168, align 8
  %arr.data.gep1169 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1170 = load ptr, ptr %arr.data.gep1169, align 8
  %arr.elem1171 = getelementptr i64, ptr %arr.data1170, i64 60
  store i64 0, ptr %arr.elem1171, align 8
  %arr.data.gep1172 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1173 = load ptr, ptr %arr.data.gep1172, align 8
  %arr.elem1174 = getelementptr i64, ptr %arr.data1173, i64 61
  store i64 0, ptr %arr.elem1174, align 8
  %arr.data.gep1175 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1176 = load ptr, ptr %arr.data.gep1175, align 8
  %arr.elem1177 = getelementptr i64, ptr %arr.data1176, i64 62
  store i64 0, ptr %arr.elem1177, align 8
  %arr.data.gep1178 = getelementptr inbounds %SadArray, ptr %arr_new984, i32 0, i32 2
  %arr.data1179 = load ptr, ptr %arr.data.gep1178, align 8
  %arr.elem1180 = getelementptr i64, ptr %arr.data1179, i64 63
  store i64 0, ptr %arr.elem1180, align 8
  %obj.ptrtoint1181 = ptrtoint ptr %arr_new984 to i64
  store i64 %obj.ptrtoint1181, ptr %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\B3\D8\A8\D8\A8_\D9\86\D9\88\D9\85", align 8
  %arr_new1182 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep1183 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 0
  store i64 64, ptr %arr.len.gep1183, align 8
  %arr.cap.gep1184 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep1184, align 8
  %arr.data1185 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep1186 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  store ptr %arr.data1185, ptr %arr.data.gep1186, align 8
  %arr.data.gep1187 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1188 = load ptr, ptr %arr.data.gep1187, align 8
  %arr.elem1189 = getelementptr i64, ptr %arr.data1188, i64 0
  store i64 0, ptr %arr.elem1189, align 8
  %arr.data.gep1190 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1191 = load ptr, ptr %arr.data.gep1190, align 8
  %arr.elem1192 = getelementptr i64, ptr %arr.data1191, i64 1
  store i64 0, ptr %arr.elem1192, align 8
  %arr.data.gep1193 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1194 = load ptr, ptr %arr.data.gep1193, align 8
  %arr.elem1195 = getelementptr i64, ptr %arr.data1194, i64 2
  store i64 0, ptr %arr.elem1195, align 8
  %arr.data.gep1196 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1197 = load ptr, ptr %arr.data.gep1196, align 8
  %arr.elem1198 = getelementptr i64, ptr %arr.data1197, i64 3
  store i64 0, ptr %arr.elem1198, align 8
  %arr.data.gep1199 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1200 = load ptr, ptr %arr.data.gep1199, align 8
  %arr.elem1201 = getelementptr i64, ptr %arr.data1200, i64 4
  store i64 0, ptr %arr.elem1201, align 8
  %arr.data.gep1202 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1203 = load ptr, ptr %arr.data.gep1202, align 8
  %arr.elem1204 = getelementptr i64, ptr %arr.data1203, i64 5
  store i64 0, ptr %arr.elem1204, align 8
  %arr.data.gep1205 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1206 = load ptr, ptr %arr.data.gep1205, align 8
  %arr.elem1207 = getelementptr i64, ptr %arr.data1206, i64 6
  store i64 0, ptr %arr.elem1207, align 8
  %arr.data.gep1208 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1209 = load ptr, ptr %arr.data.gep1208, align 8
  %arr.elem1210 = getelementptr i64, ptr %arr.data1209, i64 7
  store i64 0, ptr %arr.elem1210, align 8
  %arr.data.gep1211 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1212 = load ptr, ptr %arr.data.gep1211, align 8
  %arr.elem1213 = getelementptr i64, ptr %arr.data1212, i64 8
  store i64 0, ptr %arr.elem1213, align 8
  %arr.data.gep1214 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1215 = load ptr, ptr %arr.data.gep1214, align 8
  %arr.elem1216 = getelementptr i64, ptr %arr.data1215, i64 9
  store i64 0, ptr %arr.elem1216, align 8
  %arr.data.gep1217 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1218 = load ptr, ptr %arr.data.gep1217, align 8
  %arr.elem1219 = getelementptr i64, ptr %arr.data1218, i64 10
  store i64 0, ptr %arr.elem1219, align 8
  %arr.data.gep1220 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1221 = load ptr, ptr %arr.data.gep1220, align 8
  %arr.elem1222 = getelementptr i64, ptr %arr.data1221, i64 11
  store i64 0, ptr %arr.elem1222, align 8
  %arr.data.gep1223 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1224 = load ptr, ptr %arr.data.gep1223, align 8
  %arr.elem1225 = getelementptr i64, ptr %arr.data1224, i64 12
  store i64 0, ptr %arr.elem1225, align 8
  %arr.data.gep1226 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1227 = load ptr, ptr %arr.data.gep1226, align 8
  %arr.elem1228 = getelementptr i64, ptr %arr.data1227, i64 13
  store i64 0, ptr %arr.elem1228, align 8
  %arr.data.gep1229 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1230 = load ptr, ptr %arr.data.gep1229, align 8
  %arr.elem1231 = getelementptr i64, ptr %arr.data1230, i64 14
  store i64 0, ptr %arr.elem1231, align 8
  %arr.data.gep1232 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1233 = load ptr, ptr %arr.data.gep1232, align 8
  %arr.elem1234 = getelementptr i64, ptr %arr.data1233, i64 15
  store i64 0, ptr %arr.elem1234, align 8
  %arr.data.gep1235 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1236 = load ptr, ptr %arr.data.gep1235, align 8
  %arr.elem1237 = getelementptr i64, ptr %arr.data1236, i64 16
  store i64 0, ptr %arr.elem1237, align 8
  %arr.data.gep1238 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1239 = load ptr, ptr %arr.data.gep1238, align 8
  %arr.elem1240 = getelementptr i64, ptr %arr.data1239, i64 17
  store i64 0, ptr %arr.elem1240, align 8
  %arr.data.gep1241 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1242 = load ptr, ptr %arr.data.gep1241, align 8
  %arr.elem1243 = getelementptr i64, ptr %arr.data1242, i64 18
  store i64 0, ptr %arr.elem1243, align 8
  %arr.data.gep1244 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1245 = load ptr, ptr %arr.data.gep1244, align 8
  %arr.elem1246 = getelementptr i64, ptr %arr.data1245, i64 19
  store i64 0, ptr %arr.elem1246, align 8
  %arr.data.gep1247 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1248 = load ptr, ptr %arr.data.gep1247, align 8
  %arr.elem1249 = getelementptr i64, ptr %arr.data1248, i64 20
  store i64 0, ptr %arr.elem1249, align 8
  %arr.data.gep1250 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1251 = load ptr, ptr %arr.data.gep1250, align 8
  %arr.elem1252 = getelementptr i64, ptr %arr.data1251, i64 21
  store i64 0, ptr %arr.elem1252, align 8
  %arr.data.gep1253 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1254 = load ptr, ptr %arr.data.gep1253, align 8
  %arr.elem1255 = getelementptr i64, ptr %arr.data1254, i64 22
  store i64 0, ptr %arr.elem1255, align 8
  %arr.data.gep1256 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1257 = load ptr, ptr %arr.data.gep1256, align 8
  %arr.elem1258 = getelementptr i64, ptr %arr.data1257, i64 23
  store i64 0, ptr %arr.elem1258, align 8
  %arr.data.gep1259 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1260 = load ptr, ptr %arr.data.gep1259, align 8
  %arr.elem1261 = getelementptr i64, ptr %arr.data1260, i64 24
  store i64 0, ptr %arr.elem1261, align 8
  %arr.data.gep1262 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1263 = load ptr, ptr %arr.data.gep1262, align 8
  %arr.elem1264 = getelementptr i64, ptr %arr.data1263, i64 25
  store i64 0, ptr %arr.elem1264, align 8
  %arr.data.gep1265 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1266 = load ptr, ptr %arr.data.gep1265, align 8
  %arr.elem1267 = getelementptr i64, ptr %arr.data1266, i64 26
  store i64 0, ptr %arr.elem1267, align 8
  %arr.data.gep1268 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1269 = load ptr, ptr %arr.data.gep1268, align 8
  %arr.elem1270 = getelementptr i64, ptr %arr.data1269, i64 27
  store i64 0, ptr %arr.elem1270, align 8
  %arr.data.gep1271 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1272 = load ptr, ptr %arr.data.gep1271, align 8
  %arr.elem1273 = getelementptr i64, ptr %arr.data1272, i64 28
  store i64 0, ptr %arr.elem1273, align 8
  %arr.data.gep1274 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1275 = load ptr, ptr %arr.data.gep1274, align 8
  %arr.elem1276 = getelementptr i64, ptr %arr.data1275, i64 29
  store i64 0, ptr %arr.elem1276, align 8
  %arr.data.gep1277 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1278 = load ptr, ptr %arr.data.gep1277, align 8
  %arr.elem1279 = getelementptr i64, ptr %arr.data1278, i64 30
  store i64 0, ptr %arr.elem1279, align 8
  %arr.data.gep1280 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1281 = load ptr, ptr %arr.data.gep1280, align 8
  %arr.elem1282 = getelementptr i64, ptr %arr.data1281, i64 31
  store i64 0, ptr %arr.elem1282, align 8
  %arr.data.gep1283 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1284 = load ptr, ptr %arr.data.gep1283, align 8
  %arr.elem1285 = getelementptr i64, ptr %arr.data1284, i64 32
  store i64 0, ptr %arr.elem1285, align 8
  %arr.data.gep1286 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1287 = load ptr, ptr %arr.data.gep1286, align 8
  %arr.elem1288 = getelementptr i64, ptr %arr.data1287, i64 33
  store i64 0, ptr %arr.elem1288, align 8
  %arr.data.gep1289 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1290 = load ptr, ptr %arr.data.gep1289, align 8
  %arr.elem1291 = getelementptr i64, ptr %arr.data1290, i64 34
  store i64 0, ptr %arr.elem1291, align 8
  %arr.data.gep1292 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1293 = load ptr, ptr %arr.data.gep1292, align 8
  %arr.elem1294 = getelementptr i64, ptr %arr.data1293, i64 35
  store i64 0, ptr %arr.elem1294, align 8
  %arr.data.gep1295 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1296 = load ptr, ptr %arr.data.gep1295, align 8
  %arr.elem1297 = getelementptr i64, ptr %arr.data1296, i64 36
  store i64 0, ptr %arr.elem1297, align 8
  %arr.data.gep1298 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1299 = load ptr, ptr %arr.data.gep1298, align 8
  %arr.elem1300 = getelementptr i64, ptr %arr.data1299, i64 37
  store i64 0, ptr %arr.elem1300, align 8
  %arr.data.gep1301 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1302 = load ptr, ptr %arr.data.gep1301, align 8
  %arr.elem1303 = getelementptr i64, ptr %arr.data1302, i64 38
  store i64 0, ptr %arr.elem1303, align 8
  %arr.data.gep1304 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1305 = load ptr, ptr %arr.data.gep1304, align 8
  %arr.elem1306 = getelementptr i64, ptr %arr.data1305, i64 39
  store i64 0, ptr %arr.elem1306, align 8
  %arr.data.gep1307 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1308 = load ptr, ptr %arr.data.gep1307, align 8
  %arr.elem1309 = getelementptr i64, ptr %arr.data1308, i64 40
  store i64 0, ptr %arr.elem1309, align 8
  %arr.data.gep1310 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1311 = load ptr, ptr %arr.data.gep1310, align 8
  %arr.elem1312 = getelementptr i64, ptr %arr.data1311, i64 41
  store i64 0, ptr %arr.elem1312, align 8
  %arr.data.gep1313 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1314 = load ptr, ptr %arr.data.gep1313, align 8
  %arr.elem1315 = getelementptr i64, ptr %arr.data1314, i64 42
  store i64 0, ptr %arr.elem1315, align 8
  %arr.data.gep1316 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1317 = load ptr, ptr %arr.data.gep1316, align 8
  %arr.elem1318 = getelementptr i64, ptr %arr.data1317, i64 43
  store i64 0, ptr %arr.elem1318, align 8
  %arr.data.gep1319 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1320 = load ptr, ptr %arr.data.gep1319, align 8
  %arr.elem1321 = getelementptr i64, ptr %arr.data1320, i64 44
  store i64 0, ptr %arr.elem1321, align 8
  %arr.data.gep1322 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1323 = load ptr, ptr %arr.data.gep1322, align 8
  %arr.elem1324 = getelementptr i64, ptr %arr.data1323, i64 45
  store i64 0, ptr %arr.elem1324, align 8
  %arr.data.gep1325 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1326 = load ptr, ptr %arr.data.gep1325, align 8
  %arr.elem1327 = getelementptr i64, ptr %arr.data1326, i64 46
  store i64 0, ptr %arr.elem1327, align 8
  %arr.data.gep1328 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1329 = load ptr, ptr %arr.data.gep1328, align 8
  %arr.elem1330 = getelementptr i64, ptr %arr.data1329, i64 47
  store i64 0, ptr %arr.elem1330, align 8
  %arr.data.gep1331 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1332 = load ptr, ptr %arr.data.gep1331, align 8
  %arr.elem1333 = getelementptr i64, ptr %arr.data1332, i64 48
  store i64 0, ptr %arr.elem1333, align 8
  %arr.data.gep1334 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1335 = load ptr, ptr %arr.data.gep1334, align 8
  %arr.elem1336 = getelementptr i64, ptr %arr.data1335, i64 49
  store i64 0, ptr %arr.elem1336, align 8
  %arr.data.gep1337 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1338 = load ptr, ptr %arr.data.gep1337, align 8
  %arr.elem1339 = getelementptr i64, ptr %arr.data1338, i64 50
  store i64 0, ptr %arr.elem1339, align 8
  %arr.data.gep1340 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1341 = load ptr, ptr %arr.data.gep1340, align 8
  %arr.elem1342 = getelementptr i64, ptr %arr.data1341, i64 51
  store i64 0, ptr %arr.elem1342, align 8
  %arr.data.gep1343 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1344 = load ptr, ptr %arr.data.gep1343, align 8
  %arr.elem1345 = getelementptr i64, ptr %arr.data1344, i64 52
  store i64 0, ptr %arr.elem1345, align 8
  %arr.data.gep1346 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1347 = load ptr, ptr %arr.data.gep1346, align 8
  %arr.elem1348 = getelementptr i64, ptr %arr.data1347, i64 53
  store i64 0, ptr %arr.elem1348, align 8
  %arr.data.gep1349 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1350 = load ptr, ptr %arr.data.gep1349, align 8
  %arr.elem1351 = getelementptr i64, ptr %arr.data1350, i64 54
  store i64 0, ptr %arr.elem1351, align 8
  %arr.data.gep1352 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1353 = load ptr, ptr %arr.data.gep1352, align 8
  %arr.elem1354 = getelementptr i64, ptr %arr.data1353, i64 55
  store i64 0, ptr %arr.elem1354, align 8
  %arr.data.gep1355 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1356 = load ptr, ptr %arr.data.gep1355, align 8
  %arr.elem1357 = getelementptr i64, ptr %arr.data1356, i64 56
  store i64 0, ptr %arr.elem1357, align 8
  %arr.data.gep1358 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1359 = load ptr, ptr %arr.data.gep1358, align 8
  %arr.elem1360 = getelementptr i64, ptr %arr.data1359, i64 57
  store i64 0, ptr %arr.elem1360, align 8
  %arr.data.gep1361 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1362 = load ptr, ptr %arr.data.gep1361, align 8
  %arr.elem1363 = getelementptr i64, ptr %arr.data1362, i64 58
  store i64 0, ptr %arr.elem1363, align 8
  %arr.data.gep1364 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1365 = load ptr, ptr %arr.data.gep1364, align 8
  %arr.elem1366 = getelementptr i64, ptr %arr.data1365, i64 59
  store i64 0, ptr %arr.elem1366, align 8
  %arr.data.gep1367 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1368 = load ptr, ptr %arr.data.gep1367, align 8
  %arr.elem1369 = getelementptr i64, ptr %arr.data1368, i64 60
  store i64 0, ptr %arr.elem1369, align 8
  %arr.data.gep1370 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1371 = load ptr, ptr %arr.data.gep1370, align 8
  %arr.elem1372 = getelementptr i64, ptr %arr.data1371, i64 61
  store i64 0, ptr %arr.elem1372, align 8
  %arr.data.gep1373 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1374 = load ptr, ptr %arr.data.gep1373, align 8
  %arr.elem1375 = getelementptr i64, ptr %arr.data1374, i64 62
  store i64 0, ptr %arr.elem1375, align 8
  %arr.data.gep1376 = getelementptr inbounds %SadArray, ptr %arr_new1182, i32 0, i32 2
  %arr.data1377 = load ptr, ptr %arr.data.gep1376, align 8
  %arr.elem1378 = getelementptr i64, ptr %arr.data1377, i64 63
  store i64 0, ptr %arr.elem1378, align 8
  %obj.ptrtoint1379 = ptrtoint ptr %arr_new1182 to i64
  store i64 %obj.ptrtoint1379, ptr %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D9\88\D9\82\D8\AA_CPU", align 8
  %arr_new1380 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep1381 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 0
  store i64 64, ptr %arr.len.gep1381, align 8
  %arr.cap.gep1382 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep1382, align 8
  %arr.data1383 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep1384 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  store ptr %arr.data1383, ptr %arr.data.gep1384, align 8
  %arr.data.gep1385 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1386 = load ptr, ptr %arr.data.gep1385, align 8
  %arr.elem1387 = getelementptr i64, ptr %arr.data1386, i64 0
  store i64 0, ptr %arr.elem1387, align 8
  %arr.data.gep1388 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1389 = load ptr, ptr %arr.data.gep1388, align 8
  %arr.elem1390 = getelementptr i64, ptr %arr.data1389, i64 1
  store i64 0, ptr %arr.elem1390, align 8
  %arr.data.gep1391 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1392 = load ptr, ptr %arr.data.gep1391, align 8
  %arr.elem1393 = getelementptr i64, ptr %arr.data1392, i64 2
  store i64 0, ptr %arr.elem1393, align 8
  %arr.data.gep1394 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1395 = load ptr, ptr %arr.data.gep1394, align 8
  %arr.elem1396 = getelementptr i64, ptr %arr.data1395, i64 3
  store i64 0, ptr %arr.elem1396, align 8
  %arr.data.gep1397 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1398 = load ptr, ptr %arr.data.gep1397, align 8
  %arr.elem1399 = getelementptr i64, ptr %arr.data1398, i64 4
  store i64 0, ptr %arr.elem1399, align 8
  %arr.data.gep1400 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1401 = load ptr, ptr %arr.data.gep1400, align 8
  %arr.elem1402 = getelementptr i64, ptr %arr.data1401, i64 5
  store i64 0, ptr %arr.elem1402, align 8
  %arr.data.gep1403 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1404 = load ptr, ptr %arr.data.gep1403, align 8
  %arr.elem1405 = getelementptr i64, ptr %arr.data1404, i64 6
  store i64 0, ptr %arr.elem1405, align 8
  %arr.data.gep1406 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1407 = load ptr, ptr %arr.data.gep1406, align 8
  %arr.elem1408 = getelementptr i64, ptr %arr.data1407, i64 7
  store i64 0, ptr %arr.elem1408, align 8
  %arr.data.gep1409 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1410 = load ptr, ptr %arr.data.gep1409, align 8
  %arr.elem1411 = getelementptr i64, ptr %arr.data1410, i64 8
  store i64 0, ptr %arr.elem1411, align 8
  %arr.data.gep1412 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1413 = load ptr, ptr %arr.data.gep1412, align 8
  %arr.elem1414 = getelementptr i64, ptr %arr.data1413, i64 9
  store i64 0, ptr %arr.elem1414, align 8
  %arr.data.gep1415 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1416 = load ptr, ptr %arr.data.gep1415, align 8
  %arr.elem1417 = getelementptr i64, ptr %arr.data1416, i64 10
  store i64 0, ptr %arr.elem1417, align 8
  %arr.data.gep1418 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1419 = load ptr, ptr %arr.data.gep1418, align 8
  %arr.elem1420 = getelementptr i64, ptr %arr.data1419, i64 11
  store i64 0, ptr %arr.elem1420, align 8
  %arr.data.gep1421 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1422 = load ptr, ptr %arr.data.gep1421, align 8
  %arr.elem1423 = getelementptr i64, ptr %arr.data1422, i64 12
  store i64 0, ptr %arr.elem1423, align 8
  %arr.data.gep1424 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1425 = load ptr, ptr %arr.data.gep1424, align 8
  %arr.elem1426 = getelementptr i64, ptr %arr.data1425, i64 13
  store i64 0, ptr %arr.elem1426, align 8
  %arr.data.gep1427 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1428 = load ptr, ptr %arr.data.gep1427, align 8
  %arr.elem1429 = getelementptr i64, ptr %arr.data1428, i64 14
  store i64 0, ptr %arr.elem1429, align 8
  %arr.data.gep1430 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1431 = load ptr, ptr %arr.data.gep1430, align 8
  %arr.elem1432 = getelementptr i64, ptr %arr.data1431, i64 15
  store i64 0, ptr %arr.elem1432, align 8
  %arr.data.gep1433 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1434 = load ptr, ptr %arr.data.gep1433, align 8
  %arr.elem1435 = getelementptr i64, ptr %arr.data1434, i64 16
  store i64 0, ptr %arr.elem1435, align 8
  %arr.data.gep1436 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1437 = load ptr, ptr %arr.data.gep1436, align 8
  %arr.elem1438 = getelementptr i64, ptr %arr.data1437, i64 17
  store i64 0, ptr %arr.elem1438, align 8
  %arr.data.gep1439 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1440 = load ptr, ptr %arr.data.gep1439, align 8
  %arr.elem1441 = getelementptr i64, ptr %arr.data1440, i64 18
  store i64 0, ptr %arr.elem1441, align 8
  %arr.data.gep1442 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1443 = load ptr, ptr %arr.data.gep1442, align 8
  %arr.elem1444 = getelementptr i64, ptr %arr.data1443, i64 19
  store i64 0, ptr %arr.elem1444, align 8
  %arr.data.gep1445 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1446 = load ptr, ptr %arr.data.gep1445, align 8
  %arr.elem1447 = getelementptr i64, ptr %arr.data1446, i64 20
  store i64 0, ptr %arr.elem1447, align 8
  %arr.data.gep1448 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1449 = load ptr, ptr %arr.data.gep1448, align 8
  %arr.elem1450 = getelementptr i64, ptr %arr.data1449, i64 21
  store i64 0, ptr %arr.elem1450, align 8
  %arr.data.gep1451 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1452 = load ptr, ptr %arr.data.gep1451, align 8
  %arr.elem1453 = getelementptr i64, ptr %arr.data1452, i64 22
  store i64 0, ptr %arr.elem1453, align 8
  %arr.data.gep1454 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1455 = load ptr, ptr %arr.data.gep1454, align 8
  %arr.elem1456 = getelementptr i64, ptr %arr.data1455, i64 23
  store i64 0, ptr %arr.elem1456, align 8
  %arr.data.gep1457 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1458 = load ptr, ptr %arr.data.gep1457, align 8
  %arr.elem1459 = getelementptr i64, ptr %arr.data1458, i64 24
  store i64 0, ptr %arr.elem1459, align 8
  %arr.data.gep1460 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1461 = load ptr, ptr %arr.data.gep1460, align 8
  %arr.elem1462 = getelementptr i64, ptr %arr.data1461, i64 25
  store i64 0, ptr %arr.elem1462, align 8
  %arr.data.gep1463 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1464 = load ptr, ptr %arr.data.gep1463, align 8
  %arr.elem1465 = getelementptr i64, ptr %arr.data1464, i64 26
  store i64 0, ptr %arr.elem1465, align 8
  %arr.data.gep1466 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1467 = load ptr, ptr %arr.data.gep1466, align 8
  %arr.elem1468 = getelementptr i64, ptr %arr.data1467, i64 27
  store i64 0, ptr %arr.elem1468, align 8
  %arr.data.gep1469 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1470 = load ptr, ptr %arr.data.gep1469, align 8
  %arr.elem1471 = getelementptr i64, ptr %arr.data1470, i64 28
  store i64 0, ptr %arr.elem1471, align 8
  %arr.data.gep1472 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1473 = load ptr, ptr %arr.data.gep1472, align 8
  %arr.elem1474 = getelementptr i64, ptr %arr.data1473, i64 29
  store i64 0, ptr %arr.elem1474, align 8
  %arr.data.gep1475 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1476 = load ptr, ptr %arr.data.gep1475, align 8
  %arr.elem1477 = getelementptr i64, ptr %arr.data1476, i64 30
  store i64 0, ptr %arr.elem1477, align 8
  %arr.data.gep1478 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1479 = load ptr, ptr %arr.data.gep1478, align 8
  %arr.elem1480 = getelementptr i64, ptr %arr.data1479, i64 31
  store i64 0, ptr %arr.elem1480, align 8
  %arr.data.gep1481 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1482 = load ptr, ptr %arr.data.gep1481, align 8
  %arr.elem1483 = getelementptr i64, ptr %arr.data1482, i64 32
  store i64 0, ptr %arr.elem1483, align 8
  %arr.data.gep1484 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1485 = load ptr, ptr %arr.data.gep1484, align 8
  %arr.elem1486 = getelementptr i64, ptr %arr.data1485, i64 33
  store i64 0, ptr %arr.elem1486, align 8
  %arr.data.gep1487 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1488 = load ptr, ptr %arr.data.gep1487, align 8
  %arr.elem1489 = getelementptr i64, ptr %arr.data1488, i64 34
  store i64 0, ptr %arr.elem1489, align 8
  %arr.data.gep1490 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1491 = load ptr, ptr %arr.data.gep1490, align 8
  %arr.elem1492 = getelementptr i64, ptr %arr.data1491, i64 35
  store i64 0, ptr %arr.elem1492, align 8
  %arr.data.gep1493 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1494 = load ptr, ptr %arr.data.gep1493, align 8
  %arr.elem1495 = getelementptr i64, ptr %arr.data1494, i64 36
  store i64 0, ptr %arr.elem1495, align 8
  %arr.data.gep1496 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1497 = load ptr, ptr %arr.data.gep1496, align 8
  %arr.elem1498 = getelementptr i64, ptr %arr.data1497, i64 37
  store i64 0, ptr %arr.elem1498, align 8
  %arr.data.gep1499 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1500 = load ptr, ptr %arr.data.gep1499, align 8
  %arr.elem1501 = getelementptr i64, ptr %arr.data1500, i64 38
  store i64 0, ptr %arr.elem1501, align 8
  %arr.data.gep1502 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1503 = load ptr, ptr %arr.data.gep1502, align 8
  %arr.elem1504 = getelementptr i64, ptr %arr.data1503, i64 39
  store i64 0, ptr %arr.elem1504, align 8
  %arr.data.gep1505 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1506 = load ptr, ptr %arr.data.gep1505, align 8
  %arr.elem1507 = getelementptr i64, ptr %arr.data1506, i64 40
  store i64 0, ptr %arr.elem1507, align 8
  %arr.data.gep1508 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1509 = load ptr, ptr %arr.data.gep1508, align 8
  %arr.elem1510 = getelementptr i64, ptr %arr.data1509, i64 41
  store i64 0, ptr %arr.elem1510, align 8
  %arr.data.gep1511 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1512 = load ptr, ptr %arr.data.gep1511, align 8
  %arr.elem1513 = getelementptr i64, ptr %arr.data1512, i64 42
  store i64 0, ptr %arr.elem1513, align 8
  %arr.data.gep1514 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1515 = load ptr, ptr %arr.data.gep1514, align 8
  %arr.elem1516 = getelementptr i64, ptr %arr.data1515, i64 43
  store i64 0, ptr %arr.elem1516, align 8
  %arr.data.gep1517 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1518 = load ptr, ptr %arr.data.gep1517, align 8
  %arr.elem1519 = getelementptr i64, ptr %arr.data1518, i64 44
  store i64 0, ptr %arr.elem1519, align 8
  %arr.data.gep1520 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1521 = load ptr, ptr %arr.data.gep1520, align 8
  %arr.elem1522 = getelementptr i64, ptr %arr.data1521, i64 45
  store i64 0, ptr %arr.elem1522, align 8
  %arr.data.gep1523 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1524 = load ptr, ptr %arr.data.gep1523, align 8
  %arr.elem1525 = getelementptr i64, ptr %arr.data1524, i64 46
  store i64 0, ptr %arr.elem1525, align 8
  %arr.data.gep1526 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1527 = load ptr, ptr %arr.data.gep1526, align 8
  %arr.elem1528 = getelementptr i64, ptr %arr.data1527, i64 47
  store i64 0, ptr %arr.elem1528, align 8
  %arr.data.gep1529 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1530 = load ptr, ptr %arr.data.gep1529, align 8
  %arr.elem1531 = getelementptr i64, ptr %arr.data1530, i64 48
  store i64 0, ptr %arr.elem1531, align 8
  %arr.data.gep1532 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1533 = load ptr, ptr %arr.data.gep1532, align 8
  %arr.elem1534 = getelementptr i64, ptr %arr.data1533, i64 49
  store i64 0, ptr %arr.elem1534, align 8
  %arr.data.gep1535 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1536 = load ptr, ptr %arr.data.gep1535, align 8
  %arr.elem1537 = getelementptr i64, ptr %arr.data1536, i64 50
  store i64 0, ptr %arr.elem1537, align 8
  %arr.data.gep1538 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1539 = load ptr, ptr %arr.data.gep1538, align 8
  %arr.elem1540 = getelementptr i64, ptr %arr.data1539, i64 51
  store i64 0, ptr %arr.elem1540, align 8
  %arr.data.gep1541 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1542 = load ptr, ptr %arr.data.gep1541, align 8
  %arr.elem1543 = getelementptr i64, ptr %arr.data1542, i64 52
  store i64 0, ptr %arr.elem1543, align 8
  %arr.data.gep1544 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1545 = load ptr, ptr %arr.data.gep1544, align 8
  %arr.elem1546 = getelementptr i64, ptr %arr.data1545, i64 53
  store i64 0, ptr %arr.elem1546, align 8
  %arr.data.gep1547 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1548 = load ptr, ptr %arr.data.gep1547, align 8
  %arr.elem1549 = getelementptr i64, ptr %arr.data1548, i64 54
  store i64 0, ptr %arr.elem1549, align 8
  %arr.data.gep1550 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1551 = load ptr, ptr %arr.data.gep1550, align 8
  %arr.elem1552 = getelementptr i64, ptr %arr.data1551, i64 55
  store i64 0, ptr %arr.elem1552, align 8
  %arr.data.gep1553 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1554 = load ptr, ptr %arr.data.gep1553, align 8
  %arr.elem1555 = getelementptr i64, ptr %arr.data1554, i64 56
  store i64 0, ptr %arr.elem1555, align 8
  %arr.data.gep1556 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1557 = load ptr, ptr %arr.data.gep1556, align 8
  %arr.elem1558 = getelementptr i64, ptr %arr.data1557, i64 57
  store i64 0, ptr %arr.elem1558, align 8
  %arr.data.gep1559 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1560 = load ptr, ptr %arr.data.gep1559, align 8
  %arr.elem1561 = getelementptr i64, ptr %arr.data1560, i64 58
  store i64 0, ptr %arr.elem1561, align 8
  %arr.data.gep1562 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1563 = load ptr, ptr %arr.data.gep1562, align 8
  %arr.elem1564 = getelementptr i64, ptr %arr.data1563, i64 59
  store i64 0, ptr %arr.elem1564, align 8
  %arr.data.gep1565 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1566 = load ptr, ptr %arr.data.gep1565, align 8
  %arr.elem1567 = getelementptr i64, ptr %arr.data1566, i64 60
  store i64 0, ptr %arr.elem1567, align 8
  %arr.data.gep1568 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1569 = load ptr, ptr %arr.data.gep1568, align 8
  %arr.elem1570 = getelementptr i64, ptr %arr.data1569, i64 61
  store i64 0, ptr %arr.elem1570, align 8
  %arr.data.gep1571 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1572 = load ptr, ptr %arr.data.gep1571, align 8
  %arr.elem1573 = getelementptr i64, ptr %arr.data1572, i64 62
  store i64 0, ptr %arr.elem1573, align 8
  %arr.data.gep1574 = getelementptr inbounds %SadArray, ptr %arr_new1380, i32 0, i32 2
  %arr.data1575 = load ptr, ptr %arr.data.gep1574, align 8
  %arr.elem1576 = getelementptr i64, ptr %arr.data1575, i64 63
  store i64 0, ptr %arr.elem1576, align 8
  %obj.ptrtoint1577 = ptrtoint ptr %arr_new1380 to i64
  store i64 %obj.ptrtoint1577, ptr %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\AA\D8\A8\D8\AF\D9\8A\D9\84\D8\A7\D8\AA", align 8
  %arr_new1578 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep1579 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 0
  store i64 64, ptr %arr.len.gep1579, align 8
  %arr.cap.gep1580 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep1580, align 8
  %arr.data1581 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep1582 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  store ptr %arr.data1581, ptr %arr.data.gep1582, align 8
  %arr.data.gep1583 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1584 = load ptr, ptr %arr.data.gep1583, align 8
  %arr.elem1585 = getelementptr i64, ptr %arr.data1584, i64 0
  store i64 0, ptr %arr.elem1585, align 8
  %arr.data.gep1586 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1587 = load ptr, ptr %arr.data.gep1586, align 8
  %arr.elem1588 = getelementptr i64, ptr %arr.data1587, i64 1
  store i64 0, ptr %arr.elem1588, align 8
  %arr.data.gep1589 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1590 = load ptr, ptr %arr.data.gep1589, align 8
  %arr.elem1591 = getelementptr i64, ptr %arr.data1590, i64 2
  store i64 0, ptr %arr.elem1591, align 8
  %arr.data.gep1592 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1593 = load ptr, ptr %arr.data.gep1592, align 8
  %arr.elem1594 = getelementptr i64, ptr %arr.data1593, i64 3
  store i64 0, ptr %arr.elem1594, align 8
  %arr.data.gep1595 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1596 = load ptr, ptr %arr.data.gep1595, align 8
  %arr.elem1597 = getelementptr i64, ptr %arr.data1596, i64 4
  store i64 0, ptr %arr.elem1597, align 8
  %arr.data.gep1598 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1599 = load ptr, ptr %arr.data.gep1598, align 8
  %arr.elem1600 = getelementptr i64, ptr %arr.data1599, i64 5
  store i64 0, ptr %arr.elem1600, align 8
  %arr.data.gep1601 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1602 = load ptr, ptr %arr.data.gep1601, align 8
  %arr.elem1603 = getelementptr i64, ptr %arr.data1602, i64 6
  store i64 0, ptr %arr.elem1603, align 8
  %arr.data.gep1604 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1605 = load ptr, ptr %arr.data.gep1604, align 8
  %arr.elem1606 = getelementptr i64, ptr %arr.data1605, i64 7
  store i64 0, ptr %arr.elem1606, align 8
  %arr.data.gep1607 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1608 = load ptr, ptr %arr.data.gep1607, align 8
  %arr.elem1609 = getelementptr i64, ptr %arr.data1608, i64 8
  store i64 0, ptr %arr.elem1609, align 8
  %arr.data.gep1610 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1611 = load ptr, ptr %arr.data.gep1610, align 8
  %arr.elem1612 = getelementptr i64, ptr %arr.data1611, i64 9
  store i64 0, ptr %arr.elem1612, align 8
  %arr.data.gep1613 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1614 = load ptr, ptr %arr.data.gep1613, align 8
  %arr.elem1615 = getelementptr i64, ptr %arr.data1614, i64 10
  store i64 0, ptr %arr.elem1615, align 8
  %arr.data.gep1616 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1617 = load ptr, ptr %arr.data.gep1616, align 8
  %arr.elem1618 = getelementptr i64, ptr %arr.data1617, i64 11
  store i64 0, ptr %arr.elem1618, align 8
  %arr.data.gep1619 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1620 = load ptr, ptr %arr.data.gep1619, align 8
  %arr.elem1621 = getelementptr i64, ptr %arr.data1620, i64 12
  store i64 0, ptr %arr.elem1621, align 8
  %arr.data.gep1622 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1623 = load ptr, ptr %arr.data.gep1622, align 8
  %arr.elem1624 = getelementptr i64, ptr %arr.data1623, i64 13
  store i64 0, ptr %arr.elem1624, align 8
  %arr.data.gep1625 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1626 = load ptr, ptr %arr.data.gep1625, align 8
  %arr.elem1627 = getelementptr i64, ptr %arr.data1626, i64 14
  store i64 0, ptr %arr.elem1627, align 8
  %arr.data.gep1628 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1629 = load ptr, ptr %arr.data.gep1628, align 8
  %arr.elem1630 = getelementptr i64, ptr %arr.data1629, i64 15
  store i64 0, ptr %arr.elem1630, align 8
  %arr.data.gep1631 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1632 = load ptr, ptr %arr.data.gep1631, align 8
  %arr.elem1633 = getelementptr i64, ptr %arr.data1632, i64 16
  store i64 0, ptr %arr.elem1633, align 8
  %arr.data.gep1634 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1635 = load ptr, ptr %arr.data.gep1634, align 8
  %arr.elem1636 = getelementptr i64, ptr %arr.data1635, i64 17
  store i64 0, ptr %arr.elem1636, align 8
  %arr.data.gep1637 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1638 = load ptr, ptr %arr.data.gep1637, align 8
  %arr.elem1639 = getelementptr i64, ptr %arr.data1638, i64 18
  store i64 0, ptr %arr.elem1639, align 8
  %arr.data.gep1640 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1641 = load ptr, ptr %arr.data.gep1640, align 8
  %arr.elem1642 = getelementptr i64, ptr %arr.data1641, i64 19
  store i64 0, ptr %arr.elem1642, align 8
  %arr.data.gep1643 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1644 = load ptr, ptr %arr.data.gep1643, align 8
  %arr.elem1645 = getelementptr i64, ptr %arr.data1644, i64 20
  store i64 0, ptr %arr.elem1645, align 8
  %arr.data.gep1646 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1647 = load ptr, ptr %arr.data.gep1646, align 8
  %arr.elem1648 = getelementptr i64, ptr %arr.data1647, i64 21
  store i64 0, ptr %arr.elem1648, align 8
  %arr.data.gep1649 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1650 = load ptr, ptr %arr.data.gep1649, align 8
  %arr.elem1651 = getelementptr i64, ptr %arr.data1650, i64 22
  store i64 0, ptr %arr.elem1651, align 8
  %arr.data.gep1652 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1653 = load ptr, ptr %arr.data.gep1652, align 8
  %arr.elem1654 = getelementptr i64, ptr %arr.data1653, i64 23
  store i64 0, ptr %arr.elem1654, align 8
  %arr.data.gep1655 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1656 = load ptr, ptr %arr.data.gep1655, align 8
  %arr.elem1657 = getelementptr i64, ptr %arr.data1656, i64 24
  store i64 0, ptr %arr.elem1657, align 8
  %arr.data.gep1658 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1659 = load ptr, ptr %arr.data.gep1658, align 8
  %arr.elem1660 = getelementptr i64, ptr %arr.data1659, i64 25
  store i64 0, ptr %arr.elem1660, align 8
  %arr.data.gep1661 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1662 = load ptr, ptr %arr.data.gep1661, align 8
  %arr.elem1663 = getelementptr i64, ptr %arr.data1662, i64 26
  store i64 0, ptr %arr.elem1663, align 8
  %arr.data.gep1664 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1665 = load ptr, ptr %arr.data.gep1664, align 8
  %arr.elem1666 = getelementptr i64, ptr %arr.data1665, i64 27
  store i64 0, ptr %arr.elem1666, align 8
  %arr.data.gep1667 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1668 = load ptr, ptr %arr.data.gep1667, align 8
  %arr.elem1669 = getelementptr i64, ptr %arr.data1668, i64 28
  store i64 0, ptr %arr.elem1669, align 8
  %arr.data.gep1670 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1671 = load ptr, ptr %arr.data.gep1670, align 8
  %arr.elem1672 = getelementptr i64, ptr %arr.data1671, i64 29
  store i64 0, ptr %arr.elem1672, align 8
  %arr.data.gep1673 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1674 = load ptr, ptr %arr.data.gep1673, align 8
  %arr.elem1675 = getelementptr i64, ptr %arr.data1674, i64 30
  store i64 0, ptr %arr.elem1675, align 8
  %arr.data.gep1676 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1677 = load ptr, ptr %arr.data.gep1676, align 8
  %arr.elem1678 = getelementptr i64, ptr %arr.data1677, i64 31
  store i64 0, ptr %arr.elem1678, align 8
  %arr.data.gep1679 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1680 = load ptr, ptr %arr.data.gep1679, align 8
  %arr.elem1681 = getelementptr i64, ptr %arr.data1680, i64 32
  store i64 0, ptr %arr.elem1681, align 8
  %arr.data.gep1682 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1683 = load ptr, ptr %arr.data.gep1682, align 8
  %arr.elem1684 = getelementptr i64, ptr %arr.data1683, i64 33
  store i64 0, ptr %arr.elem1684, align 8
  %arr.data.gep1685 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1686 = load ptr, ptr %arr.data.gep1685, align 8
  %arr.elem1687 = getelementptr i64, ptr %arr.data1686, i64 34
  store i64 0, ptr %arr.elem1687, align 8
  %arr.data.gep1688 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1689 = load ptr, ptr %arr.data.gep1688, align 8
  %arr.elem1690 = getelementptr i64, ptr %arr.data1689, i64 35
  store i64 0, ptr %arr.elem1690, align 8
  %arr.data.gep1691 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1692 = load ptr, ptr %arr.data.gep1691, align 8
  %arr.elem1693 = getelementptr i64, ptr %arr.data1692, i64 36
  store i64 0, ptr %arr.elem1693, align 8
  %arr.data.gep1694 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1695 = load ptr, ptr %arr.data.gep1694, align 8
  %arr.elem1696 = getelementptr i64, ptr %arr.data1695, i64 37
  store i64 0, ptr %arr.elem1696, align 8
  %arr.data.gep1697 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1698 = load ptr, ptr %arr.data.gep1697, align 8
  %arr.elem1699 = getelementptr i64, ptr %arr.data1698, i64 38
  store i64 0, ptr %arr.elem1699, align 8
  %arr.data.gep1700 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1701 = load ptr, ptr %arr.data.gep1700, align 8
  %arr.elem1702 = getelementptr i64, ptr %arr.data1701, i64 39
  store i64 0, ptr %arr.elem1702, align 8
  %arr.data.gep1703 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1704 = load ptr, ptr %arr.data.gep1703, align 8
  %arr.elem1705 = getelementptr i64, ptr %arr.data1704, i64 40
  store i64 0, ptr %arr.elem1705, align 8
  %arr.data.gep1706 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1707 = load ptr, ptr %arr.data.gep1706, align 8
  %arr.elem1708 = getelementptr i64, ptr %arr.data1707, i64 41
  store i64 0, ptr %arr.elem1708, align 8
  %arr.data.gep1709 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1710 = load ptr, ptr %arr.data.gep1709, align 8
  %arr.elem1711 = getelementptr i64, ptr %arr.data1710, i64 42
  store i64 0, ptr %arr.elem1711, align 8
  %arr.data.gep1712 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1713 = load ptr, ptr %arr.data.gep1712, align 8
  %arr.elem1714 = getelementptr i64, ptr %arr.data1713, i64 43
  store i64 0, ptr %arr.elem1714, align 8
  %arr.data.gep1715 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1716 = load ptr, ptr %arr.data.gep1715, align 8
  %arr.elem1717 = getelementptr i64, ptr %arr.data1716, i64 44
  store i64 0, ptr %arr.elem1717, align 8
  %arr.data.gep1718 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1719 = load ptr, ptr %arr.data.gep1718, align 8
  %arr.elem1720 = getelementptr i64, ptr %arr.data1719, i64 45
  store i64 0, ptr %arr.elem1720, align 8
  %arr.data.gep1721 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1722 = load ptr, ptr %arr.data.gep1721, align 8
  %arr.elem1723 = getelementptr i64, ptr %arr.data1722, i64 46
  store i64 0, ptr %arr.elem1723, align 8
  %arr.data.gep1724 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1725 = load ptr, ptr %arr.data.gep1724, align 8
  %arr.elem1726 = getelementptr i64, ptr %arr.data1725, i64 47
  store i64 0, ptr %arr.elem1726, align 8
  %arr.data.gep1727 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1728 = load ptr, ptr %arr.data.gep1727, align 8
  %arr.elem1729 = getelementptr i64, ptr %arr.data1728, i64 48
  store i64 0, ptr %arr.elem1729, align 8
  %arr.data.gep1730 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1731 = load ptr, ptr %arr.data.gep1730, align 8
  %arr.elem1732 = getelementptr i64, ptr %arr.data1731, i64 49
  store i64 0, ptr %arr.elem1732, align 8
  %arr.data.gep1733 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1734 = load ptr, ptr %arr.data.gep1733, align 8
  %arr.elem1735 = getelementptr i64, ptr %arr.data1734, i64 50
  store i64 0, ptr %arr.elem1735, align 8
  %arr.data.gep1736 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1737 = load ptr, ptr %arr.data.gep1736, align 8
  %arr.elem1738 = getelementptr i64, ptr %arr.data1737, i64 51
  store i64 0, ptr %arr.elem1738, align 8
  %arr.data.gep1739 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1740 = load ptr, ptr %arr.data.gep1739, align 8
  %arr.elem1741 = getelementptr i64, ptr %arr.data1740, i64 52
  store i64 0, ptr %arr.elem1741, align 8
  %arr.data.gep1742 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1743 = load ptr, ptr %arr.data.gep1742, align 8
  %arr.elem1744 = getelementptr i64, ptr %arr.data1743, i64 53
  store i64 0, ptr %arr.elem1744, align 8
  %arr.data.gep1745 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1746 = load ptr, ptr %arr.data.gep1745, align 8
  %arr.elem1747 = getelementptr i64, ptr %arr.data1746, i64 54
  store i64 0, ptr %arr.elem1747, align 8
  %arr.data.gep1748 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1749 = load ptr, ptr %arr.data.gep1748, align 8
  %arr.elem1750 = getelementptr i64, ptr %arr.data1749, i64 55
  store i64 0, ptr %arr.elem1750, align 8
  %arr.data.gep1751 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1752 = load ptr, ptr %arr.data.gep1751, align 8
  %arr.elem1753 = getelementptr i64, ptr %arr.data1752, i64 56
  store i64 0, ptr %arr.elem1753, align 8
  %arr.data.gep1754 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1755 = load ptr, ptr %arr.data.gep1754, align 8
  %arr.elem1756 = getelementptr i64, ptr %arr.data1755, i64 57
  store i64 0, ptr %arr.elem1756, align 8
  %arr.data.gep1757 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1758 = load ptr, ptr %arr.data.gep1757, align 8
  %arr.elem1759 = getelementptr i64, ptr %arr.data1758, i64 58
  store i64 0, ptr %arr.elem1759, align 8
  %arr.data.gep1760 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1761 = load ptr, ptr %arr.data.gep1760, align 8
  %arr.elem1762 = getelementptr i64, ptr %arr.data1761, i64 59
  store i64 0, ptr %arr.elem1762, align 8
  %arr.data.gep1763 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1764 = load ptr, ptr %arr.data.gep1763, align 8
  %arr.elem1765 = getelementptr i64, ptr %arr.data1764, i64 60
  store i64 0, ptr %arr.elem1765, align 8
  %arr.data.gep1766 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1767 = load ptr, ptr %arr.data.gep1766, align 8
  %arr.elem1768 = getelementptr i64, ptr %arr.data1767, i64 61
  store i64 0, ptr %arr.elem1768, align 8
  %arr.data.gep1769 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1770 = load ptr, ptr %arr.data.gep1769, align 8
  %arr.elem1771 = getelementptr i64, ptr %arr.data1770, i64 62
  store i64 0, ptr %arr.elem1771, align 8
  %arr.data.gep1772 = getelementptr inbounds %SadArray, ptr %arr_new1578, i32 0, i32 2
  %arr.data1773 = load ptr, ptr %arr.data.gep1772, align 8
  %arr.elem1774 = getelementptr i64, ptr %arr.data1773, i64 63
  store i64 0, ptr %arr.elem1774, align 8
  %obj.ptrtoint1775 = ptrtoint ptr %arr_new1578 to i64
  store i64 %obj.ptrtoint1775, ptr %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D9\85\D9\83\D8\AF\D8\B3", align 8
  %arr_new1776 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep1777 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 0
  store i64 64, ptr %arr.len.gep1777, align 8
  %arr.cap.gep1778 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep1778, align 8
  %arr.data1779 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep1780 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  store ptr %arr.data1779, ptr %arr.data.gep1780, align 8
  %arr.data.gep1781 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1782 = load ptr, ptr %arr.data.gep1781, align 8
  %arr.elem1783 = getelementptr i64, ptr %arr.data1782, i64 0
  store i64 0, ptr %arr.elem1783, align 8
  %arr.data.gep1784 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1785 = load ptr, ptr %arr.data.gep1784, align 8
  %arr.elem1786 = getelementptr i64, ptr %arr.data1785, i64 1
  store i64 0, ptr %arr.elem1786, align 8
  %arr.data.gep1787 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1788 = load ptr, ptr %arr.data.gep1787, align 8
  %arr.elem1789 = getelementptr i64, ptr %arr.data1788, i64 2
  store i64 0, ptr %arr.elem1789, align 8
  %arr.data.gep1790 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1791 = load ptr, ptr %arr.data.gep1790, align 8
  %arr.elem1792 = getelementptr i64, ptr %arr.data1791, i64 3
  store i64 0, ptr %arr.elem1792, align 8
  %arr.data.gep1793 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1794 = load ptr, ptr %arr.data.gep1793, align 8
  %arr.elem1795 = getelementptr i64, ptr %arr.data1794, i64 4
  store i64 0, ptr %arr.elem1795, align 8
  %arr.data.gep1796 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1797 = load ptr, ptr %arr.data.gep1796, align 8
  %arr.elem1798 = getelementptr i64, ptr %arr.data1797, i64 5
  store i64 0, ptr %arr.elem1798, align 8
  %arr.data.gep1799 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1800 = load ptr, ptr %arr.data.gep1799, align 8
  %arr.elem1801 = getelementptr i64, ptr %arr.data1800, i64 6
  store i64 0, ptr %arr.elem1801, align 8
  %arr.data.gep1802 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1803 = load ptr, ptr %arr.data.gep1802, align 8
  %arr.elem1804 = getelementptr i64, ptr %arr.data1803, i64 7
  store i64 0, ptr %arr.elem1804, align 8
  %arr.data.gep1805 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1806 = load ptr, ptr %arr.data.gep1805, align 8
  %arr.elem1807 = getelementptr i64, ptr %arr.data1806, i64 8
  store i64 0, ptr %arr.elem1807, align 8
  %arr.data.gep1808 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1809 = load ptr, ptr %arr.data.gep1808, align 8
  %arr.elem1810 = getelementptr i64, ptr %arr.data1809, i64 9
  store i64 0, ptr %arr.elem1810, align 8
  %arr.data.gep1811 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1812 = load ptr, ptr %arr.data.gep1811, align 8
  %arr.elem1813 = getelementptr i64, ptr %arr.data1812, i64 10
  store i64 0, ptr %arr.elem1813, align 8
  %arr.data.gep1814 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1815 = load ptr, ptr %arr.data.gep1814, align 8
  %arr.elem1816 = getelementptr i64, ptr %arr.data1815, i64 11
  store i64 0, ptr %arr.elem1816, align 8
  %arr.data.gep1817 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1818 = load ptr, ptr %arr.data.gep1817, align 8
  %arr.elem1819 = getelementptr i64, ptr %arr.data1818, i64 12
  store i64 0, ptr %arr.elem1819, align 8
  %arr.data.gep1820 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1821 = load ptr, ptr %arr.data.gep1820, align 8
  %arr.elem1822 = getelementptr i64, ptr %arr.data1821, i64 13
  store i64 0, ptr %arr.elem1822, align 8
  %arr.data.gep1823 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1824 = load ptr, ptr %arr.data.gep1823, align 8
  %arr.elem1825 = getelementptr i64, ptr %arr.data1824, i64 14
  store i64 0, ptr %arr.elem1825, align 8
  %arr.data.gep1826 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1827 = load ptr, ptr %arr.data.gep1826, align 8
  %arr.elem1828 = getelementptr i64, ptr %arr.data1827, i64 15
  store i64 0, ptr %arr.elem1828, align 8
  %arr.data.gep1829 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1830 = load ptr, ptr %arr.data.gep1829, align 8
  %arr.elem1831 = getelementptr i64, ptr %arr.data1830, i64 16
  store i64 0, ptr %arr.elem1831, align 8
  %arr.data.gep1832 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1833 = load ptr, ptr %arr.data.gep1832, align 8
  %arr.elem1834 = getelementptr i64, ptr %arr.data1833, i64 17
  store i64 0, ptr %arr.elem1834, align 8
  %arr.data.gep1835 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1836 = load ptr, ptr %arr.data.gep1835, align 8
  %arr.elem1837 = getelementptr i64, ptr %arr.data1836, i64 18
  store i64 0, ptr %arr.elem1837, align 8
  %arr.data.gep1838 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1839 = load ptr, ptr %arr.data.gep1838, align 8
  %arr.elem1840 = getelementptr i64, ptr %arr.data1839, i64 19
  store i64 0, ptr %arr.elem1840, align 8
  %arr.data.gep1841 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1842 = load ptr, ptr %arr.data.gep1841, align 8
  %arr.elem1843 = getelementptr i64, ptr %arr.data1842, i64 20
  store i64 0, ptr %arr.elem1843, align 8
  %arr.data.gep1844 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1845 = load ptr, ptr %arr.data.gep1844, align 8
  %arr.elem1846 = getelementptr i64, ptr %arr.data1845, i64 21
  store i64 0, ptr %arr.elem1846, align 8
  %arr.data.gep1847 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1848 = load ptr, ptr %arr.data.gep1847, align 8
  %arr.elem1849 = getelementptr i64, ptr %arr.data1848, i64 22
  store i64 0, ptr %arr.elem1849, align 8
  %arr.data.gep1850 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1851 = load ptr, ptr %arr.data.gep1850, align 8
  %arr.elem1852 = getelementptr i64, ptr %arr.data1851, i64 23
  store i64 0, ptr %arr.elem1852, align 8
  %arr.data.gep1853 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1854 = load ptr, ptr %arr.data.gep1853, align 8
  %arr.elem1855 = getelementptr i64, ptr %arr.data1854, i64 24
  store i64 0, ptr %arr.elem1855, align 8
  %arr.data.gep1856 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1857 = load ptr, ptr %arr.data.gep1856, align 8
  %arr.elem1858 = getelementptr i64, ptr %arr.data1857, i64 25
  store i64 0, ptr %arr.elem1858, align 8
  %arr.data.gep1859 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1860 = load ptr, ptr %arr.data.gep1859, align 8
  %arr.elem1861 = getelementptr i64, ptr %arr.data1860, i64 26
  store i64 0, ptr %arr.elem1861, align 8
  %arr.data.gep1862 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1863 = load ptr, ptr %arr.data.gep1862, align 8
  %arr.elem1864 = getelementptr i64, ptr %arr.data1863, i64 27
  store i64 0, ptr %arr.elem1864, align 8
  %arr.data.gep1865 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1866 = load ptr, ptr %arr.data.gep1865, align 8
  %arr.elem1867 = getelementptr i64, ptr %arr.data1866, i64 28
  store i64 0, ptr %arr.elem1867, align 8
  %arr.data.gep1868 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1869 = load ptr, ptr %arr.data.gep1868, align 8
  %arr.elem1870 = getelementptr i64, ptr %arr.data1869, i64 29
  store i64 0, ptr %arr.elem1870, align 8
  %arr.data.gep1871 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1872 = load ptr, ptr %arr.data.gep1871, align 8
  %arr.elem1873 = getelementptr i64, ptr %arr.data1872, i64 30
  store i64 0, ptr %arr.elem1873, align 8
  %arr.data.gep1874 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1875 = load ptr, ptr %arr.data.gep1874, align 8
  %arr.elem1876 = getelementptr i64, ptr %arr.data1875, i64 31
  store i64 0, ptr %arr.elem1876, align 8
  %arr.data.gep1877 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1878 = load ptr, ptr %arr.data.gep1877, align 8
  %arr.elem1879 = getelementptr i64, ptr %arr.data1878, i64 32
  store i64 0, ptr %arr.elem1879, align 8
  %arr.data.gep1880 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1881 = load ptr, ptr %arr.data.gep1880, align 8
  %arr.elem1882 = getelementptr i64, ptr %arr.data1881, i64 33
  store i64 0, ptr %arr.elem1882, align 8
  %arr.data.gep1883 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1884 = load ptr, ptr %arr.data.gep1883, align 8
  %arr.elem1885 = getelementptr i64, ptr %arr.data1884, i64 34
  store i64 0, ptr %arr.elem1885, align 8
  %arr.data.gep1886 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1887 = load ptr, ptr %arr.data.gep1886, align 8
  %arr.elem1888 = getelementptr i64, ptr %arr.data1887, i64 35
  store i64 0, ptr %arr.elem1888, align 8
  %arr.data.gep1889 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1890 = load ptr, ptr %arr.data.gep1889, align 8
  %arr.elem1891 = getelementptr i64, ptr %arr.data1890, i64 36
  store i64 0, ptr %arr.elem1891, align 8
  %arr.data.gep1892 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1893 = load ptr, ptr %arr.data.gep1892, align 8
  %arr.elem1894 = getelementptr i64, ptr %arr.data1893, i64 37
  store i64 0, ptr %arr.elem1894, align 8
  %arr.data.gep1895 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1896 = load ptr, ptr %arr.data.gep1895, align 8
  %arr.elem1897 = getelementptr i64, ptr %arr.data1896, i64 38
  store i64 0, ptr %arr.elem1897, align 8
  %arr.data.gep1898 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1899 = load ptr, ptr %arr.data.gep1898, align 8
  %arr.elem1900 = getelementptr i64, ptr %arr.data1899, i64 39
  store i64 0, ptr %arr.elem1900, align 8
  %arr.data.gep1901 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1902 = load ptr, ptr %arr.data.gep1901, align 8
  %arr.elem1903 = getelementptr i64, ptr %arr.data1902, i64 40
  store i64 0, ptr %arr.elem1903, align 8
  %arr.data.gep1904 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1905 = load ptr, ptr %arr.data.gep1904, align 8
  %arr.elem1906 = getelementptr i64, ptr %arr.data1905, i64 41
  store i64 0, ptr %arr.elem1906, align 8
  %arr.data.gep1907 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1908 = load ptr, ptr %arr.data.gep1907, align 8
  %arr.elem1909 = getelementptr i64, ptr %arr.data1908, i64 42
  store i64 0, ptr %arr.elem1909, align 8
  %arr.data.gep1910 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1911 = load ptr, ptr %arr.data.gep1910, align 8
  %arr.elem1912 = getelementptr i64, ptr %arr.data1911, i64 43
  store i64 0, ptr %arr.elem1912, align 8
  %arr.data.gep1913 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1914 = load ptr, ptr %arr.data.gep1913, align 8
  %arr.elem1915 = getelementptr i64, ptr %arr.data1914, i64 44
  store i64 0, ptr %arr.elem1915, align 8
  %arr.data.gep1916 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1917 = load ptr, ptr %arr.data.gep1916, align 8
  %arr.elem1918 = getelementptr i64, ptr %arr.data1917, i64 45
  store i64 0, ptr %arr.elem1918, align 8
  %arr.data.gep1919 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1920 = load ptr, ptr %arr.data.gep1919, align 8
  %arr.elem1921 = getelementptr i64, ptr %arr.data1920, i64 46
  store i64 0, ptr %arr.elem1921, align 8
  %arr.data.gep1922 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1923 = load ptr, ptr %arr.data.gep1922, align 8
  %arr.elem1924 = getelementptr i64, ptr %arr.data1923, i64 47
  store i64 0, ptr %arr.elem1924, align 8
  %arr.data.gep1925 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1926 = load ptr, ptr %arr.data.gep1925, align 8
  %arr.elem1927 = getelementptr i64, ptr %arr.data1926, i64 48
  store i64 0, ptr %arr.elem1927, align 8
  %arr.data.gep1928 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1929 = load ptr, ptr %arr.data.gep1928, align 8
  %arr.elem1930 = getelementptr i64, ptr %arr.data1929, i64 49
  store i64 0, ptr %arr.elem1930, align 8
  %arr.data.gep1931 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1932 = load ptr, ptr %arr.data.gep1931, align 8
  %arr.elem1933 = getelementptr i64, ptr %arr.data1932, i64 50
  store i64 0, ptr %arr.elem1933, align 8
  %arr.data.gep1934 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1935 = load ptr, ptr %arr.data.gep1934, align 8
  %arr.elem1936 = getelementptr i64, ptr %arr.data1935, i64 51
  store i64 0, ptr %arr.elem1936, align 8
  %arr.data.gep1937 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1938 = load ptr, ptr %arr.data.gep1937, align 8
  %arr.elem1939 = getelementptr i64, ptr %arr.data1938, i64 52
  store i64 0, ptr %arr.elem1939, align 8
  %arr.data.gep1940 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1941 = load ptr, ptr %arr.data.gep1940, align 8
  %arr.elem1942 = getelementptr i64, ptr %arr.data1941, i64 53
  store i64 0, ptr %arr.elem1942, align 8
  %arr.data.gep1943 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1944 = load ptr, ptr %arr.data.gep1943, align 8
  %arr.elem1945 = getelementptr i64, ptr %arr.data1944, i64 54
  store i64 0, ptr %arr.elem1945, align 8
  %arr.data.gep1946 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1947 = load ptr, ptr %arr.data.gep1946, align 8
  %arr.elem1948 = getelementptr i64, ptr %arr.data1947, i64 55
  store i64 0, ptr %arr.elem1948, align 8
  %arr.data.gep1949 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1950 = load ptr, ptr %arr.data.gep1949, align 8
  %arr.elem1951 = getelementptr i64, ptr %arr.data1950, i64 56
  store i64 0, ptr %arr.elem1951, align 8
  %arr.data.gep1952 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1953 = load ptr, ptr %arr.data.gep1952, align 8
  %arr.elem1954 = getelementptr i64, ptr %arr.data1953, i64 57
  store i64 0, ptr %arr.elem1954, align 8
  %arr.data.gep1955 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1956 = load ptr, ptr %arr.data.gep1955, align 8
  %arr.elem1957 = getelementptr i64, ptr %arr.data1956, i64 58
  store i64 0, ptr %arr.elem1957, align 8
  %arr.data.gep1958 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1959 = load ptr, ptr %arr.data.gep1958, align 8
  %arr.elem1960 = getelementptr i64, ptr %arr.data1959, i64 59
  store i64 0, ptr %arr.elem1960, align 8
  %arr.data.gep1961 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1962 = load ptr, ptr %arr.data.gep1961, align 8
  %arr.elem1963 = getelementptr i64, ptr %arr.data1962, i64 60
  store i64 0, ptr %arr.elem1963, align 8
  %arr.data.gep1964 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1965 = load ptr, ptr %arr.data.gep1964, align 8
  %arr.elem1966 = getelementptr i64, ptr %arr.data1965, i64 61
  store i64 0, ptr %arr.elem1966, align 8
  %arr.data.gep1967 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1968 = load ptr, ptr %arr.data.gep1967, align 8
  %arr.elem1969 = getelementptr i64, ptr %arr.data1968, i64 62
  store i64 0, ptr %arr.elem1969, align 8
  %arr.data.gep1970 = getelementptr inbounds %SadArray, ptr %arr_new1776, i32 0, i32 2
  %arr.data1971 = load ptr, ptr %arr.data.gep1970, align 8
  %arr.elem1972 = getelementptr i64, ptr %arr.data1971, i64 63
  store i64 0, ptr %arr.elem1972, align 8
  %obj.ptrtoint1973 = ptrtoint ptr %arr_new1776 to i64
  store i64 %obj.ptrtoint1973, ptr %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\B3\D9\8A\D8\A7\D9\82", align 8
  %arr_new1974 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep1975 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 0
  store i64 64, ptr %arr.len.gep1975, align 8
  %arr.cap.gep1976 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep1976, align 8
  %arr.data1977 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep1978 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  store ptr %arr.data1977, ptr %arr.data.gep1978, align 8
  %arr.data.gep1979 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data1980 = load ptr, ptr %arr.data.gep1979, align 8
  %arr.elem1981 = getelementptr i64, ptr %arr.data1980, i64 0
  store i64 0, ptr %arr.elem1981, align 8
  %arr.data.gep1982 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data1983 = load ptr, ptr %arr.data.gep1982, align 8
  %arr.elem1984 = getelementptr i64, ptr %arr.data1983, i64 1
  store i64 0, ptr %arr.elem1984, align 8
  %arr.data.gep1985 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data1986 = load ptr, ptr %arr.data.gep1985, align 8
  %arr.elem1987 = getelementptr i64, ptr %arr.data1986, i64 2
  store i64 0, ptr %arr.elem1987, align 8
  %arr.data.gep1988 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data1989 = load ptr, ptr %arr.data.gep1988, align 8
  %arr.elem1990 = getelementptr i64, ptr %arr.data1989, i64 3
  store i64 0, ptr %arr.elem1990, align 8
  %arr.data.gep1991 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data1992 = load ptr, ptr %arr.data.gep1991, align 8
  %arr.elem1993 = getelementptr i64, ptr %arr.data1992, i64 4
  store i64 0, ptr %arr.elem1993, align 8
  %arr.data.gep1994 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data1995 = load ptr, ptr %arr.data.gep1994, align 8
  %arr.elem1996 = getelementptr i64, ptr %arr.data1995, i64 5
  store i64 0, ptr %arr.elem1996, align 8
  %arr.data.gep1997 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data1998 = load ptr, ptr %arr.data.gep1997, align 8
  %arr.elem1999 = getelementptr i64, ptr %arr.data1998, i64 6
  store i64 0, ptr %arr.elem1999, align 8
  %arr.data.gep2000 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2001 = load ptr, ptr %arr.data.gep2000, align 8
  %arr.elem2002 = getelementptr i64, ptr %arr.data2001, i64 7
  store i64 0, ptr %arr.elem2002, align 8
  %arr.data.gep2003 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2004 = load ptr, ptr %arr.data.gep2003, align 8
  %arr.elem2005 = getelementptr i64, ptr %arr.data2004, i64 8
  store i64 0, ptr %arr.elem2005, align 8
  %arr.data.gep2006 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2007 = load ptr, ptr %arr.data.gep2006, align 8
  %arr.elem2008 = getelementptr i64, ptr %arr.data2007, i64 9
  store i64 0, ptr %arr.elem2008, align 8
  %arr.data.gep2009 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2010 = load ptr, ptr %arr.data.gep2009, align 8
  %arr.elem2011 = getelementptr i64, ptr %arr.data2010, i64 10
  store i64 0, ptr %arr.elem2011, align 8
  %arr.data.gep2012 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2013 = load ptr, ptr %arr.data.gep2012, align 8
  %arr.elem2014 = getelementptr i64, ptr %arr.data2013, i64 11
  store i64 0, ptr %arr.elem2014, align 8
  %arr.data.gep2015 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2016 = load ptr, ptr %arr.data.gep2015, align 8
  %arr.elem2017 = getelementptr i64, ptr %arr.data2016, i64 12
  store i64 0, ptr %arr.elem2017, align 8
  %arr.data.gep2018 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2019 = load ptr, ptr %arr.data.gep2018, align 8
  %arr.elem2020 = getelementptr i64, ptr %arr.data2019, i64 13
  store i64 0, ptr %arr.elem2020, align 8
  %arr.data.gep2021 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2022 = load ptr, ptr %arr.data.gep2021, align 8
  %arr.elem2023 = getelementptr i64, ptr %arr.data2022, i64 14
  store i64 0, ptr %arr.elem2023, align 8
  %arr.data.gep2024 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2025 = load ptr, ptr %arr.data.gep2024, align 8
  %arr.elem2026 = getelementptr i64, ptr %arr.data2025, i64 15
  store i64 0, ptr %arr.elem2026, align 8
  %arr.data.gep2027 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2028 = load ptr, ptr %arr.data.gep2027, align 8
  %arr.elem2029 = getelementptr i64, ptr %arr.data2028, i64 16
  store i64 0, ptr %arr.elem2029, align 8
  %arr.data.gep2030 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2031 = load ptr, ptr %arr.data.gep2030, align 8
  %arr.elem2032 = getelementptr i64, ptr %arr.data2031, i64 17
  store i64 0, ptr %arr.elem2032, align 8
  %arr.data.gep2033 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2034 = load ptr, ptr %arr.data.gep2033, align 8
  %arr.elem2035 = getelementptr i64, ptr %arr.data2034, i64 18
  store i64 0, ptr %arr.elem2035, align 8
  %arr.data.gep2036 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2037 = load ptr, ptr %arr.data.gep2036, align 8
  %arr.elem2038 = getelementptr i64, ptr %arr.data2037, i64 19
  store i64 0, ptr %arr.elem2038, align 8
  %arr.data.gep2039 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2040 = load ptr, ptr %arr.data.gep2039, align 8
  %arr.elem2041 = getelementptr i64, ptr %arr.data2040, i64 20
  store i64 0, ptr %arr.elem2041, align 8
  %arr.data.gep2042 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2043 = load ptr, ptr %arr.data.gep2042, align 8
  %arr.elem2044 = getelementptr i64, ptr %arr.data2043, i64 21
  store i64 0, ptr %arr.elem2044, align 8
  %arr.data.gep2045 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2046 = load ptr, ptr %arr.data.gep2045, align 8
  %arr.elem2047 = getelementptr i64, ptr %arr.data2046, i64 22
  store i64 0, ptr %arr.elem2047, align 8
  %arr.data.gep2048 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2049 = load ptr, ptr %arr.data.gep2048, align 8
  %arr.elem2050 = getelementptr i64, ptr %arr.data2049, i64 23
  store i64 0, ptr %arr.elem2050, align 8
  %arr.data.gep2051 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2052 = load ptr, ptr %arr.data.gep2051, align 8
  %arr.elem2053 = getelementptr i64, ptr %arr.data2052, i64 24
  store i64 0, ptr %arr.elem2053, align 8
  %arr.data.gep2054 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2055 = load ptr, ptr %arr.data.gep2054, align 8
  %arr.elem2056 = getelementptr i64, ptr %arr.data2055, i64 25
  store i64 0, ptr %arr.elem2056, align 8
  %arr.data.gep2057 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2058 = load ptr, ptr %arr.data.gep2057, align 8
  %arr.elem2059 = getelementptr i64, ptr %arr.data2058, i64 26
  store i64 0, ptr %arr.elem2059, align 8
  %arr.data.gep2060 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2061 = load ptr, ptr %arr.data.gep2060, align 8
  %arr.elem2062 = getelementptr i64, ptr %arr.data2061, i64 27
  store i64 0, ptr %arr.elem2062, align 8
  %arr.data.gep2063 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2064 = load ptr, ptr %arr.data.gep2063, align 8
  %arr.elem2065 = getelementptr i64, ptr %arr.data2064, i64 28
  store i64 0, ptr %arr.elem2065, align 8
  %arr.data.gep2066 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2067 = load ptr, ptr %arr.data.gep2066, align 8
  %arr.elem2068 = getelementptr i64, ptr %arr.data2067, i64 29
  store i64 0, ptr %arr.elem2068, align 8
  %arr.data.gep2069 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2070 = load ptr, ptr %arr.data.gep2069, align 8
  %arr.elem2071 = getelementptr i64, ptr %arr.data2070, i64 30
  store i64 0, ptr %arr.elem2071, align 8
  %arr.data.gep2072 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2073 = load ptr, ptr %arr.data.gep2072, align 8
  %arr.elem2074 = getelementptr i64, ptr %arr.data2073, i64 31
  store i64 0, ptr %arr.elem2074, align 8
  %arr.data.gep2075 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2076 = load ptr, ptr %arr.data.gep2075, align 8
  %arr.elem2077 = getelementptr i64, ptr %arr.data2076, i64 32
  store i64 0, ptr %arr.elem2077, align 8
  %arr.data.gep2078 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2079 = load ptr, ptr %arr.data.gep2078, align 8
  %arr.elem2080 = getelementptr i64, ptr %arr.data2079, i64 33
  store i64 0, ptr %arr.elem2080, align 8
  %arr.data.gep2081 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2082 = load ptr, ptr %arr.data.gep2081, align 8
  %arr.elem2083 = getelementptr i64, ptr %arr.data2082, i64 34
  store i64 0, ptr %arr.elem2083, align 8
  %arr.data.gep2084 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2085 = load ptr, ptr %arr.data.gep2084, align 8
  %arr.elem2086 = getelementptr i64, ptr %arr.data2085, i64 35
  store i64 0, ptr %arr.elem2086, align 8
  %arr.data.gep2087 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2088 = load ptr, ptr %arr.data.gep2087, align 8
  %arr.elem2089 = getelementptr i64, ptr %arr.data2088, i64 36
  store i64 0, ptr %arr.elem2089, align 8
  %arr.data.gep2090 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2091 = load ptr, ptr %arr.data.gep2090, align 8
  %arr.elem2092 = getelementptr i64, ptr %arr.data2091, i64 37
  store i64 0, ptr %arr.elem2092, align 8
  %arr.data.gep2093 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2094 = load ptr, ptr %arr.data.gep2093, align 8
  %arr.elem2095 = getelementptr i64, ptr %arr.data2094, i64 38
  store i64 0, ptr %arr.elem2095, align 8
  %arr.data.gep2096 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2097 = load ptr, ptr %arr.data.gep2096, align 8
  %arr.elem2098 = getelementptr i64, ptr %arr.data2097, i64 39
  store i64 0, ptr %arr.elem2098, align 8
  %arr.data.gep2099 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2100 = load ptr, ptr %arr.data.gep2099, align 8
  %arr.elem2101 = getelementptr i64, ptr %arr.data2100, i64 40
  store i64 0, ptr %arr.elem2101, align 8
  %arr.data.gep2102 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2103 = load ptr, ptr %arr.data.gep2102, align 8
  %arr.elem2104 = getelementptr i64, ptr %arr.data2103, i64 41
  store i64 0, ptr %arr.elem2104, align 8
  %arr.data.gep2105 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2106 = load ptr, ptr %arr.data.gep2105, align 8
  %arr.elem2107 = getelementptr i64, ptr %arr.data2106, i64 42
  store i64 0, ptr %arr.elem2107, align 8
  %arr.data.gep2108 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2109 = load ptr, ptr %arr.data.gep2108, align 8
  %arr.elem2110 = getelementptr i64, ptr %arr.data2109, i64 43
  store i64 0, ptr %arr.elem2110, align 8
  %arr.data.gep2111 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2112 = load ptr, ptr %arr.data.gep2111, align 8
  %arr.elem2113 = getelementptr i64, ptr %arr.data2112, i64 44
  store i64 0, ptr %arr.elem2113, align 8
  %arr.data.gep2114 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2115 = load ptr, ptr %arr.data.gep2114, align 8
  %arr.elem2116 = getelementptr i64, ptr %arr.data2115, i64 45
  store i64 0, ptr %arr.elem2116, align 8
  %arr.data.gep2117 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2118 = load ptr, ptr %arr.data.gep2117, align 8
  %arr.elem2119 = getelementptr i64, ptr %arr.data2118, i64 46
  store i64 0, ptr %arr.elem2119, align 8
  %arr.data.gep2120 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2121 = load ptr, ptr %arr.data.gep2120, align 8
  %arr.elem2122 = getelementptr i64, ptr %arr.data2121, i64 47
  store i64 0, ptr %arr.elem2122, align 8
  %arr.data.gep2123 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2124 = load ptr, ptr %arr.data.gep2123, align 8
  %arr.elem2125 = getelementptr i64, ptr %arr.data2124, i64 48
  store i64 0, ptr %arr.elem2125, align 8
  %arr.data.gep2126 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2127 = load ptr, ptr %arr.data.gep2126, align 8
  %arr.elem2128 = getelementptr i64, ptr %arr.data2127, i64 49
  store i64 0, ptr %arr.elem2128, align 8
  %arr.data.gep2129 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2130 = load ptr, ptr %arr.data.gep2129, align 8
  %arr.elem2131 = getelementptr i64, ptr %arr.data2130, i64 50
  store i64 0, ptr %arr.elem2131, align 8
  %arr.data.gep2132 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2133 = load ptr, ptr %arr.data.gep2132, align 8
  %arr.elem2134 = getelementptr i64, ptr %arr.data2133, i64 51
  store i64 0, ptr %arr.elem2134, align 8
  %arr.data.gep2135 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2136 = load ptr, ptr %arr.data.gep2135, align 8
  %arr.elem2137 = getelementptr i64, ptr %arr.data2136, i64 52
  store i64 0, ptr %arr.elem2137, align 8
  %arr.data.gep2138 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2139 = load ptr, ptr %arr.data.gep2138, align 8
  %arr.elem2140 = getelementptr i64, ptr %arr.data2139, i64 53
  store i64 0, ptr %arr.elem2140, align 8
  %arr.data.gep2141 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2142 = load ptr, ptr %arr.data.gep2141, align 8
  %arr.elem2143 = getelementptr i64, ptr %arr.data2142, i64 54
  store i64 0, ptr %arr.elem2143, align 8
  %arr.data.gep2144 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2145 = load ptr, ptr %arr.data.gep2144, align 8
  %arr.elem2146 = getelementptr i64, ptr %arr.data2145, i64 55
  store i64 0, ptr %arr.elem2146, align 8
  %arr.data.gep2147 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2148 = load ptr, ptr %arr.data.gep2147, align 8
  %arr.elem2149 = getelementptr i64, ptr %arr.data2148, i64 56
  store i64 0, ptr %arr.elem2149, align 8
  %arr.data.gep2150 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2151 = load ptr, ptr %arr.data.gep2150, align 8
  %arr.elem2152 = getelementptr i64, ptr %arr.data2151, i64 57
  store i64 0, ptr %arr.elem2152, align 8
  %arr.data.gep2153 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2154 = load ptr, ptr %arr.data.gep2153, align 8
  %arr.elem2155 = getelementptr i64, ptr %arr.data2154, i64 58
  store i64 0, ptr %arr.elem2155, align 8
  %arr.data.gep2156 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2157 = load ptr, ptr %arr.data.gep2156, align 8
  %arr.elem2158 = getelementptr i64, ptr %arr.data2157, i64 59
  store i64 0, ptr %arr.elem2158, align 8
  %arr.data.gep2159 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2160 = load ptr, ptr %arr.data.gep2159, align 8
  %arr.elem2161 = getelementptr i64, ptr %arr.data2160, i64 60
  store i64 0, ptr %arr.elem2161, align 8
  %arr.data.gep2162 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2163 = load ptr, ptr %arr.data.gep2162, align 8
  %arr.elem2164 = getelementptr i64, ptr %arr.data2163, i64 61
  store i64 0, ptr %arr.elem2164, align 8
  %arr.data.gep2165 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2166 = load ptr, ptr %arr.data.gep2165, align 8
  %arr.elem2167 = getelementptr i64, ptr %arr.data2166, i64 62
  store i64 0, ptr %arr.elem2167, align 8
  %arr.data.gep2168 = getelementptr inbounds %SadArray, ptr %arr_new1974, i32 0, i32 2
  %arr.data2169 = load ptr, ptr %arr.data.gep2168, align 8
  %arr.elem2170 = getelementptr i64, ptr %arr.data2169, i64 63
  store i64 0, ptr %arr.elem2170, align 8
  %obj.ptrtoint2171 = ptrtoint ptr %arr_new1974 to i64
  store i64 %obj.ptrtoint2171, ptr %"%\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\A3\D8\A8", align 8
  store i64 0, ptr %"%\D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%\D8\A2\D8\AE\D8\B1_PID", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D8\A8\D8\AF\D9\8A\D9\84\D8\A7\D8\AA_\D8\A7\D9\84\D8\B3\D9\8A\D8\A7\D9\82", align 8
  store i64 0, ptr %"%\D8\A7\D9\84\D9\85\D8\AC\D8\AF\D9\88\D9\84_\D9\86\D8\B4\D8\B7", align 8
  store i64 0, ptr %"%\D9\88\D9\82\D8\AA_\D8\A2\D8\AE\D8\B1_\D8\AC\D8\AF\D9\88\D9\84\D8\A9", align 8
  ret void
}

declare ptr @malloc(i64)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
