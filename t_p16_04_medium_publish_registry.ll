; ModuleID = '04_medium_publish_registry'
source_filename = "04_medium_publish_registry"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [139 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [139 x i8] c"\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\00", align 1
@str.const.7 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.11 = private unnamed_addr constant [8 x i8] c"  \F0\9F\9A\80 \00", align 1
@str.const.16 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.20 = private unnamed_addr constant [7 x i8] c"  \E2\96\B8 \00", align 1
@str.const.25 = private unnamed_addr constant [5 x i8] c"    \00", align 1
@str.const.28 = private unnamed_addr constant [3 x i8] c": \00", align 1
@fmt.d = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1
@str.const.40 = private unnamed_addr constant [9 x i8] c"    \E2\9C\93 \00", align 1
@str.const.45 = private unnamed_addr constant [9 x i8] c"    \E2\9A\A0 \00", align 1
@str.const.50 = private unnamed_addr constant [9 x i8] c"    \E2\9D\8C \00", align 1
@str.const.55 = private unnamed_addr constant [30 x i8] c"\D9\82\D8\B1\D8\A7\D8\A1\D8\A9 \D8\B5.toml \D9\84\D9\84\D9\86\D8\B4\D8\B1\00", align 1
@str.const.56 = private unnamed_addr constant [13 x i8] c"  [\D8\AD\D8\B2\D9\85\D8\A9]\00", align 1
@str.const.60 = private unnamed_addr constant [11 x i8] c"\D8\A7\D9\84\D8\A7\D8\B3\D9\85\00", align 1
@str.const.61 = private unnamed_addr constant [26 x i8] c"\D9\85\D9\83\D8\AA\D8\A8\D8\A9_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.62 = private unnamed_addr constant [15 x i8] c"\D8\A7\D9\84\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1\00", align 1
@str.const.63 = private unnamed_addr constant [6 x i8] c"2.1.0\00", align 1
@str.const.64 = private unnamed_addr constant [11 x i8] c"\D8\A7\D9\84\D9\88\D8\B5\D9\81\00", align 1
@str.const.65 = private unnamed_addr constant [48 x i8] c"\D9\85\D9\83\D8\AA\D8\A8\D8\A9 \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA \D8\B9\D8\B1\D8\A8\D9\8A\D8\A9 \D8\B4\D8\A7\D9\85\D9\84\D8\A9\00", align 1
@str.const.66 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D9\85\D8\A4\D9\84\D9\81\00", align 1
@str.const.67 = private unnamed_addr constant [12 x i8] c"\D9\81\D8\B1\D9\8A\D9\82 \D8\B5\00", align 1
@str.const.68 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D8\B1\D8\AE\D8\B5\D8\A9\00", align 1
@str.const.69 = private unnamed_addr constant [4 x i8] c"MIT\00", align 1
@str.const.70 = private unnamed_addr constant [17 x i8] c"\D8\A7\D9\84\D9\85\D8\B3\D8\AA\D9\88\D8\AF\D8\B9\00", align 1
@str.const.71 = private unnamed_addr constant [33 x i8] c"https://github.com/sad-lang/math\00", align 1
@str.const.72 = private unnamed_addr constant [34 x i8] c"\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD\D9\8A\D8\A9\00", align 1
@str.const.73 = private unnamed_addr constant [35 x i8] c"\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA\D8\8C \D8\AD\D8\B3\D8\A7\D8\A8\D8\8C \D8\AC\D8\A8\D8\B1\00", align 1
@str.const.74 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.78 = private unnamed_addr constant [17 x i8] c"  [\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA]\00", align 1
@str.const.82 = private unnamed_addr constant [15 x i8] c"\D8\A7\D8\B3\D8\A7\D8\B3\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.83 = private unnamed_addr constant [8 x i8] c">=1.0.0\00", align 1
@str.const.84 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.88 = private unnamed_addr constant [28 x i8] c"  [\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA_\D8\AA\D8\B7\D9\88\D9\8A\D8\B1]\00", align 1
@str.const.92 = private unnamed_addr constant [13 x i8] c"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\00", align 1
@str.const.93 = private unnamed_addr constant [8 x i8] c">=2.0.0\00", align 1
@str.const.94 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.98 = private unnamed_addr constant [19 x i8] c"  [\D8\B3\D9\83\D8\B1\D8\A8\D8\AA\D8\A7\D8\AA]\00", align 1
@str.const.102 = private unnamed_addr constant [9 x i8] c"\D8\A8\D9\86\D8\A7\D8\A1\00", align 1
@str.const.103 = private unnamed_addr constant [25 x i8] c"\D8\B5 \D8\A8\D9\86\D8\A7\D8\A1 --\D8\A5\D8\B7\D9\84\D8\A7\D9\82\00", align 1
@str.const.105 = private unnamed_addr constant [27 x i8] c"\D8\B5 \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 --\D8\A7\D9\84\D9\83\D9\84\00", align 1
@str.const.106 = private unnamed_addr constant [11 x i8] c"\D8\AA\D9\88\D8\AB\D9\8A\D9\82\00", align 1
@str.const.107 = private unnamed_addr constant [19 x i8] c"\D8\B5 \D9\88\D8\AB\D9\91\D9\82 --html\00", align 1
@str.const.108 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@0 = private unnamed_addr constant [11 x i8] c"Apache-2.0\00", align 1
@1 = private unnamed_addr constant [8 x i8] c"GPL-3.0\00", align 1
@2 = private unnamed_addr constant [6 x i8] c"BSD-3\00", align 1
@str.const.109 = private unnamed_addr constant [36 x i8] c"\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA \D9\85\D8\A7 \D9\82\D8\A8\D9\84 \D8\A7\D9\84\D9\86\D8\B4\D8\B1\00", align 1
@str.const.110 = private unnamed_addr constant [41 x i8] c"    [1/7] \D9\81\D8\AD\D8\B5 \D8\A7\D8\B3\D9\85 \D8\A7\D9\84\D8\AD\D8\B2\D9\85\D8\A9... \00", align 1
@str.const.113 = private unnamed_addr constant [13 x i8] c"\E2\9C\85 \D8\B5\D8\A7\D9\84\D8\AD\00", align 1
@str.const.117 = private unnamed_addr constant [34 x i8] c"\E2\9D\8C \D9\81\D8\A7\D8\B1\D8\BA \D8\A3\D9\88 \D8\BA\D9\8A\D8\B1 \D8\B5\D8\A7\D9\84\D8\AD\00", align 1
@str.const.121 = private unnamed_addr constant [51 x i8] c"    [2/7] \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1 \D8\A7\D9\84\D8\AF\D9\84\D8\A7\D9\84\D9\8A... \00", align 1
@str.const.123 = private unnamed_addr constant [19 x i8] c"\E2\9C\85 2.1.0 \D8\B5\D8\A7\D9\84\D8\AD\00", align 1
@str.const.127 = private unnamed_addr constant [31 x i8] c"\E2\9D\8C \D8\A5\D8\B5\D8\AF\D8\A7\D8\B1 \D8\BA\D9\8A\D8\B1 \D8\B5\D8\A7\D9\84\D8\AD\00", align 1
@str.const.131 = private unnamed_addr constant [34 x i8] c"    [3/7] \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\B1\D8\AE\D8\B5\D8\A9... \00", align 1
@str.const.134 = private unnamed_addr constant [21 x i8] c"\E2\9C\85 MIT \D9\85\D8\B9\D8\AA\D9\85\D8\AF\D8\A9\00", align 1
@str.const.138 = private unnamed_addr constant [33 x i8] c"\E2\9D\8C \D8\B1\D8\AE\D8\B5\D8\A9 \D8\BA\D9\8A\D8\B1 \D9\85\D8\B9\D8\AA\D9\85\D8\AF\D8\A9\00", align 1
@str.const.142 = private unnamed_addr constant [32 x i8] c"    [4/7] \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D9\88\D8\B5\D9\81... \00", align 1
@str.const.144 = private unnamed_addr constant [27 x i8] c"\E2\9C\85 \D9\85\D9\88\D8\AC\D9\88\D8\AF (30 \D8\AD\D8\B1\D9\81)\00", align 1
@str.const.148 = private unnamed_addr constant [46 x i8] c"    [5/7] \D8\AA\D8\B4\D8\BA\D9\8A\D9\84 \D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA... \00", align 1
@str.const.150 = private unnamed_addr constant [19 x i8] c"\E2\9C\85 24/24 \D9\86\D8\AC\D8\AD\D8\AA\00", align 1
@str.const.154 = private unnamed_addr constant [36 x i8] c"    [6/7] \D8\A8\D9\86\D8\A7\D8\A1 \D8\A7\D9\84\D8\AD\D8\B2\D9\85\D8\A9... \00", align 1
@str.const.156 = private unnamed_addr constant [22 x i8] c"\E2\9C\85 \D8\A8\D9\86\D8\A7\D8\A1 \D9\86\D8\A7\D8\AC\D8\AD\00", align 1
@str.const.160 = private unnamed_addr constant [36 x i8] c"    [7/7] \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\AA\D9\88\D8\AB\D9\8A\D9\82... \00", align 1
@str.const.162 = private unnamed_addr constant [27 x i8] c"\E2\9C\85 12 \D8\AF\D8\A7\D9\84\D8\A9 \D9\85\D9\88\D8\AB\D9\82\D8\A9\00", align 1
@str.const.166 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.170 = private unnamed_addr constant [21 x i8] c"    \D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9: \00", align 1
@str.const.173 = private unnamed_addr constant [2 x i8] c"/\00", align 1
@str.const.176 = private unnamed_addr constant [31 x i8] c" - \D8\AC\D8\A7\D9\87\D8\B2\D8\A9 \D9\84\D9\84\D9\86\D8\B4\D8\B1! \F0\9F\8E\89\00", align 1
@str.const.180 = private unnamed_addr constant [38 x i8] c" - \D9\8A\D9\84\D8\B2\D9\85 \D8\A5\D8\B5\D9\84\D8\A7\D8\AD \D8\A7\D9\84\D9\85\D8\B4\D8\A7\D9\83\D9\84\00", align 1
@str.const.184 = private unnamed_addr constant [10 x i8] c"    \F0\9F\93\9D \00", align 1
@str.const.189 = private unnamed_addr constant [20 x i8] c"       \D8\A7\D9\84\D9\88\D8\B5\D9\81: \00", align 1
@str.const.194 = private unnamed_addr constant [26 x i8] c"       \D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA: \00", align 1
@str.const.199 = private unnamed_addr constant [26 x i8] c"       \D8\A7\D9\84\D9\85\D8\AE\D8\B1\D8\AC\D8\A7\D8\AA: \00", align 1
@str.const.204 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.208 = private unnamed_addr constant [32 x i8] c"\D8\A7\D9\84\D8\AA\D9\88\D8\AB\D9\8A\D9\82 \D8\A7\D9\84\D8\AA\D9\84\D9\82\D8\A7\D8\A6\D9\8A\00", align 1
@str.const.209 = private unnamed_addr constant [60 x i8] c"    \F0\9F\93\9A \D8\AA\D9\88\D9\84\D9\8A\D8\AF \D8\AA\D9\88\D8\AB\D9\8A\D9\82 \D9\85\D9\83\D8\AA\D8\A8\D8\A9_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA...\00", align 1
@str.const.213 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.217 = private unnamed_addr constant [16 x i8] c"\D8\AC\D9\85\D8\B9(\D8\A3\D8\8C \D8\A8)\00", align 1
@str.const.218 = private unnamed_addr constant [18 x i8] c"\D8\AC\D9\85\D8\B9 \D8\B9\D8\AF\D8\AF\D9\8A\D9\86\00", align 1
@str.const.219 = private unnamed_addr constant [22 x i8] c"\D8\B1\D9\82\D9\85 \D8\A3\D8\8C \D8\B1\D9\82\D9\85 \D8\A8\00", align 1
@str.const.220 = private unnamed_addr constant [7 x i8] c"\D8\B1\D9\82\D9\85\00", align 1
@str.const.221 = private unnamed_addr constant [16 x i8] c"\D8\B7\D8\B1\D8\AD(\D8\A3\D8\8C \D8\A8)\00", align 1
@str.const.222 = private unnamed_addr constant [18 x i8] c"\D8\B7\D8\B1\D8\AD \D8\B9\D8\AF\D8\AF\D9\8A\D9\86\00", align 1
@str.const.225 = private unnamed_addr constant [16 x i8] c"\D8\B6\D8\B1\D8\A8(\D8\A3\D8\8C \D8\A8)\00", align 1
@str.const.226 = private unnamed_addr constant [18 x i8] c"\D8\B6\D8\B1\D8\A8 \D8\B9\D8\AF\D8\AF\D9\8A\D9\86\00", align 1
@str.const.229 = private unnamed_addr constant [27 x i8] c"\D9\82\D8\B3\D9\85\D8\A9_\D8\A2\D9\85\D9\86\D8\A9(\D8\A3\D8\8C \D8\A8)\00", align 1
@str.const.230 = private unnamed_addr constant [41 x i8] c"\D9\82\D8\B3\D9\85\D8\A9 \D9\85\D8\B9 \D8\AD\D9\85\D8\A7\D9\8A\D8\A9 \D9\85\D9\86 \D8\A7\D9\84\D8\B5\D9\81\D8\B1\00", align 1
@str.const.233 = private unnamed_addr constant [16 x i8] c"\D9\82\D9\88\D8\A9(\D8\A3\D8\8C \D9\86)\00", align 1
@str.const.234 = private unnamed_addr constant [23 x i8] c"\D8\B1\D9\81\D8\B9 \D8\B9\D8\AF\D8\AF \D9\84\D9\82\D9\88\D8\A9\00", align 1
@str.const.235 = private unnamed_addr constant [22 x i8] c"\D8\B1\D9\82\D9\85 \D8\A3\D8\8C \D8\B1\D9\82\D9\85 \D9\86\00", align 1
@str.const.237 = private unnamed_addr constant [15 x i8] c"\D9\85\D8\B6\D8\B1\D9\88\D8\A8(\D9\86)\00", align 1
@str.const.238 = private unnamed_addr constant [24 x i8] c"\D8\AD\D8\B3\D8\A7\D8\A8 \D8\A7\D9\84\D9\85\D8\B6\D8\B1\D9\88\D8\A8\00", align 1
@str.const.239 = private unnamed_addr constant [10 x i8] c"\D8\B1\D9\82\D9\85 \D9\86\00", align 1
@str.const.241 = private unnamed_addr constant [20 x i8] c"\D8\AF\D9\88\D8\A7\D9\84 \D9\85\D9\88\D8\AB\D9\82\D8\A9\00", align 1
@str.const.242 = private unnamed_addr constant [26 x i8] c"\D8\AA\D9\86\D8\B3\D9\8A\D9\82 \D8\A7\D9\84\D8\A5\D8\AE\D8\B1\D8\A7\D8\AC\00", align 1
@str.const.243 = private unnamed_addr constant [16 x i8] c"HTML + Markdown\00", align 1
@str.const.244 = private unnamed_addr constant [31 x i8] c"\D9\86\D8\B4\D8\B1 \D8\A7\D9\84\D8\AD\D8\B2\D9\85\D8\A9: \D8\B5 \D9\86\D8\B4\D8\B1\00", align 1
@str.const.245 = private unnamed_addr constant [40 x i8] c"    \E2\8F\B3 \D8\AC\D8\A7\D8\B1\D9\8A \D8\B6\D8\BA\D8\B7 \D8\A7\D9\84\D8\AD\D8\B2\D9\85\D8\A9...\00", align 1
@str.const.249 = private unnamed_addr constant [62 x i8] c"    \F0\9F\93\A6 \D8\A7\D9\84\D8\AD\D8\B2\D9\85\D8\A9: \D9\85\D9\83\D8\AA\D8\A8\D8\A9_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA-2.1.0.tar.gz\00", align 1
@str.const.253 = private unnamed_addr constant [39 x i8] c"\D8\AD\D8\AC\D9\85 \D8\A7\D9\84\D8\AD\D8\B2\D9\85\D8\A9 (\D9\83\D9\8A\D9\84\D9\88\D8\A8\D8\A7\D9\8A\D8\AA)\00", align 1
@str.const.254 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.258 = private unnamed_addr constant [63 x i8] c"    \E2\8F\B3 \D8\AC\D8\A7\D8\B1\D9\8A \D8\A7\D9\84\D8\B1\D9\81\D8\B9 \D8\A5\D9\84\D9\89 \D8\B3\D8\AC\D9\84 \D8\B5 \D8\A7\D9\84\D9\85\D8\B1\D9\83\D8\B2\D9\8A...\00", align 1
@str.const.262 = private unnamed_addr constant [6 x i8] c"    [\00", align 1
@str.const.264 = private unnamed_addr constant [4 x i8] c"\E2\96\88\00", align 1
@str.const.266 = private unnamed_addr constant [7 x i8] c"] 100%\00", align 1
@str.const.270 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.274 = private unnamed_addr constant [65 x i8] c"    \F0\9F\8E\89 \D8\AA\D9\85 \D9\86\D8\B4\D8\B1 \D9\85\D9\83\D8\AA\D8\A8\D8\A9_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA@2.1.0 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD!\00", align 1
@str.const.278 = private unnamed_addr constant [60 x i8] c"    \F0\9F\94\97 https://pkg.sad-lang.org/\D9\85\D9\83\D8\AA\D8\A8\D8\A9_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.282 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.286 = private unnamed_addr constant [40 x i8] c"    \F0\9F\93\8A \D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D8\AD\D8\B2\D9\85\D8\A9:\00", align 1
@str.const.290 = private unnamed_addr constant [32 x i8] c"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A \D8\A7\D9\84\D8\AA\D9\86\D8\B2\D9\8A\D9\84\D8\A7\D8\AA\00", align 1
@str.const.291 = private unnamed_addr constant [26 x i8] c"\D8\B9\D8\AF\D8\AF \D8\A7\D9\84\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1\D8\A7\D8\AA\00", align 1
@str.const.292 = private unnamed_addr constant [45 x i8] c"\D8\A7\D9\84\D9\85\D8\B4\D8\A7\D8\B1\D9\8A\D8\B9 \D8\A7\D9\84\D9\85\D8\B9\D8\AA\D9\85\D8\AF\D8\A9 \D8\B9\D9\84\D9\8A\D9\87\D8\A7\00", align 1
@str.const.293 = private unnamed_addr constant [145 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.297 = private unnamed_addr constant [70 x i8] c"\E2\95\91  \F0\9F\9A\80 \D8\B5 \D9\86\D8\B4\D8\B1 - \D9\86\D8\B8\D8\A7\D9\85 \D9\86\D8\B4\D8\B1 \D8\A7\D9\84\D8\AD\D8\B2\D9\85                  \E2\95\91\00", align 1
@str.const.301 = private unnamed_addr constant [145 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.305 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.309 = private unnamed_addr constant [48 x i8] c"  \E2\9C\85 \D8\A7\D9\83\D8\AA\D9\85\D9\84 \D9\86\D8\B4\D8\B1 \D8\A7\D9\84\D8\AD\D8\B2\D9\85\D8\A9 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD\00", align 1

define i64 @"\D8\AE\D8\B7_\D8\B9\D8\B1\D9\8A\D8\B6"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\AE\D8\B7"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr %"\D8\B9") {
entry:
  %"\D8\B91" = alloca ptr, align 8
  store ptr %"\D8\B9", ptr %"\D8\B91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\AE\D8\B7_\D8\B9\D8\B1\D9\8A\D8\B6"()
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.11)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\B91")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp2 = call i64 @"\D8\AE\D8\B7_\D8\B9\D8\B1\D9\8A\D8\B6"()
  ret i64 0
}

define i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\82\D8\B3\D9\85"(ptr %"\D8\B9") {
entry:
  %"\D8\B91" = alloca ptr, align 8
  store ptr %"\D8\B9", ptr %"\D8\B91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.16)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\AE\D8\B7"()
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.20)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\B91")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp2 = call i64 @"\D8\AE\D8\B7"()
  ret i64 0
}

define i64 @"\D8\AD\D9\82\D9\84"(ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD", ptr %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"\D9\82\D9\8A\D9\85\D8\A92" = alloca ptr, align 8
  store ptr %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1" = alloca ptr, align 8
  store ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD", ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.25)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.28)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\82\D9\8A\D9\85\D8\A92")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\AD\D9\82\D9\84_\D8\B1\D9\82\D9\85\D9\8A"(ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD", i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"\D9\82\D9\8A\D9\85\D8\A92" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1" = alloca ptr, align 8
  store ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD", ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.25)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\85\D9\81\D8\AA\D8\A7\D8\AD1")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.28)
  %"%\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D9\86\D8\AC\D9\85\D8\A9"(ptr %"\D9\86\D8\B5_\D8\B9\D9\86\D8\B5\D8\B1") {
entry:
  %"\D9\86\D8\B5_\D8\B9\D9\86\D8\B5\D8\B11" = alloca ptr, align 8
  store ptr %"\D9\86\D8\B5_\D8\B9\D9\86\D8\B5\D8\B1", ptr %"\D9\86\D8\B5_\D8\B9\D9\86\D8\B5\D8\B11", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.40)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\86\D8\B5_\D8\B9\D9\86\D8\B5\D8\B11")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1"(ptr %"\D9\86\D8\B5_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1") {
entry:
  %"\D9\86\D8\B5_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B11" = alloca ptr, align 8
  store ptr %"\D9\86\D8\B5_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1", ptr %"\D9\86\D8\B5_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B11", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.45)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\86\D8\B5_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B11")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\AE\D8\B7\D8\A3_\D8\B9\D8\B1\D8\B6"(ptr %"\D9\86\D8\B5_\D8\AE\D8\B7\D8\A3") {
entry:
  %"\D9\86\D8\B5_\D8\AE\D8\B7\D8\A31" = alloca ptr, align 8
  store ptr %"\D9\86\D8\B5_\D8\AE\D8\B7\D8\A3", ptr %"\D9\86\D8\B5_\D8\AE\D8\B7\D8\A31", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.50)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\86\D8\B5_\D8\AE\D8\B7\D8\A31")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D9\84\D9\81_\D8\AD\D8\B2\D9\85\D8\A9"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr @str.const.55)
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.56)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp1 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.60, ptr @str.const.61)
  %calltmp2 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.62, ptr @str.const.63)
  %calltmp3 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.64, ptr @str.const.65)
  %calltmp4 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.66, ptr @str.const.67)
  %calltmp5 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.68, ptr @str.const.69)
  %calltmp6 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.70, ptr @str.const.71)
  %calltmp7 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.72, ptr @str.const.73)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.74)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.78)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp8 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.82, ptr @str.const.83)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.84)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.88)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp9 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.92, ptr @str.const.93)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.94)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.98)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp10 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.102, ptr @str.const.103)
  %calltmp11 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.92, ptr @str.const.105)
  %calltmp12 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.106, ptr @str.const.107)
  ret i64 0
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\A7\D8\B3\D9\85"(ptr %"\D8\AD\D8\B2\D9\85\D8\A9") {
entry:
  %"\D8\AD\D8\B2\D9\85\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\AD\D8\B2\D9\85\D8\A9", ptr %"\D8\AD\D8\B2\D9\85\D8\A91", align 8
  %alloca = alloca i64, align 8
  store i64 1, ptr %alloca, align 8
  %strcmp.ret = call i32 @strcmp(ptr %"\D8\AD\D8\B2\D9\85\D8\A91", ptr @str.const.108)
  %streq = icmp eq i32 %strcmp.ret, 0
  br i1 %streq, label %then_0, label %merge_1

then_0:                                           ; preds = %entry
  store i64 0, ptr %alloca, align 8
  br label %merge_1

merge_1:                                          ; preds = %then_0, %entry
  %loadtmp = load i64, ptr %alloca, align 8
  ret i64 %loadtmp
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(i64 %"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A", i64 %"\D9\81\D8\B1\D8\B9\D9\8A", i64 %"\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD") {
entry:
  %"\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD3" = alloca i64, align 8
  store i64 %"\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD", ptr %"\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD3", align 8
  %"\D9\81\D8\B1\D8\B9\D9\8A2" = alloca i64, align 8
  store i64 %"\D9\81\D8\B1\D8\B9\D9\8A", ptr %"\D9\81\D8\B1\D8\B9\D9\8A2", align 8
  %"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A1" = alloca i64, align 8
  store i64 %"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A", ptr %"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A1", align 8
  %alloca = alloca i64, align 8
  store i64 1, ptr %alloca, align 8
  %"%\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A.load" = load i64, ptr %"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A1", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A.load", 0
  br i1 %cmplttmp, label %then_2, label %merge_3

then_2:                                           ; preds = %entry
  store i64 0, ptr %alloca, align 8
  br label %merge_3

merge_3:                                          ; preds = %then_2, %entry
  %"%\D9\81\D8\B1\D8\B9\D9\8A.load" = load i64, ptr %"\D9\81\D8\B1\D8\B9\D9\8A2", align 8
  %cmplttmp4 = icmp slt i64 %"%\D9\81\D8\B1\D8\B9\D9\8A.load", 0
  br i1 %cmplttmp4, label %then_4, label %merge_5

then_4:                                           ; preds = %merge_3
  store i64 0, ptr %alloca, align 8
  br label %merge_5

merge_5:                                          ; preds = %then_4, %merge_3
  %"%\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD.load" = load i64, ptr %"\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD3", align 8
  %cmplttmp5 = icmp slt i64 %"%\D8\AA\D8\B5\D8\AD\D9\8A\D8\AD.load", 0
  br i1 %cmplttmp5, label %then_6, label %merge_7

then_6:                                           ; preds = %merge_5
  store i64 0, ptr %alloca, align 8
  br label %merge_7

merge_7:                                          ; preds = %then_6, %merge_5
  %loadtmp = load i64, ptr %alloca, align 8
  ret i64 %loadtmp
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\B1\D8\AE\D8\B5\D8\A9"(ptr %"\D8\B1\D8\AE\D8\B5\D8\A9") {
entry:
  %"\D8\B1\D8\AE\D8\B5\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\B1\D8\AE\D8\B5\D8\A9", ptr %"\D8\B1\D8\AE\D8\B5\D8\A91", align 8
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %loadtmp = load ptr, ptr %"\D8\B1\D8\AE\D8\B5\D8\A91", align 8
  br label %match.case0.test_9

match.end_8:                                      ; preds = %match.case4.body_18, %match.case4.test_17, %match.case3.body_16, %match.case2.body_14, %match.case1.body_12, %match.case0.body_10
  %loadtmp2 = load i64, ptr %alloca, align 8
  ret i64 %loadtmp2

match.case0.test_9:                               ; preds = %entry
  %strcmp.ret = call i32 @strcmp(ptr %loadtmp, ptr @str.const.69)
  %streq = icmp eq i32 %strcmp.ret, 0
  br i1 %streq, label %match.case0.body_10, label %match.case1.test_11

match.case0.body_10:                              ; preds = %match.case0.test_9
  store i64 1, ptr %alloca, align 8
  br label %match.end_8

match.case1.test_11:                              ; preds = %match.case0.test_9
  %strcmp.ret3 = call i32 @strcmp(ptr %loadtmp, ptr @0)
  %streq4 = icmp eq i32 %strcmp.ret3, 0
  br i1 %streq4, label %match.case1.body_12, label %match.case2.test_13

match.case1.body_12:                              ; preds = %match.case1.test_11
  store i64 1, ptr %alloca, align 8
  br label %match.end_8

match.case2.test_13:                              ; preds = %match.case1.test_11
  %strcmp.ret5 = call i32 @strcmp(ptr %loadtmp, ptr @1)
  %streq6 = icmp eq i32 %strcmp.ret5, 0
  br i1 %streq6, label %match.case2.body_14, label %match.case3.test_15

match.case2.body_14:                              ; preds = %match.case2.test_13
  store i64 1, ptr %alloca, align 8
  br label %match.end_8

match.case3.test_15:                              ; preds = %match.case2.test_13
  %strcmp.ret7 = call i32 @strcmp(ptr %loadtmp, ptr @2)
  %streq8 = icmp eq i32 %strcmp.ret7, 0
  br i1 %streq8, label %match.case3.body_16, label %match.case4.test_17

match.case3.body_16:                              ; preds = %match.case3.test_15
  store i64 1, ptr %alloca, align 8
  br label %match.end_8

match.case4.test_17:                              ; preds = %match.case3.test_15
  br i1 true, label %match.case4.body_18, label %match.end_8

match.case4.body_18:                              ; preds = %match.case4.test_17
  store i64 0, ptr %alloca, align 8
  br label %match.end_8
}

define i64 @"\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA_\D9\86\D8\B4\D8\B1"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\82\D8\B3\D9\85"(ptr @str.const.109)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 7, ptr %alloca1, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.110)
  %calltmp2 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\A7\D8\B3\D9\85"(ptr @str.const.61)
  %cmpeqtmp = icmp eq i64 %calltmp2, 1
  br i1 %cmpeqtmp, label %then_19, label %else_21

then_19:                                          ; preds = %entry
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.113)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\86\D9\82\D8\A7\D8\B7.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\86\D9\82\D8\A7\D8\B7.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  br label %merge_20

else_21:                                          ; preds = %entry
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.117)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_20

merge_20:                                         ; preds = %else_21, %then_19
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.121)
  %calltmp3 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1"(i64 2, i64 1, i64 0)
  %cmpeqtmp4 = icmp eq i64 %calltmp3, 1
  br i1 %cmpeqtmp4, label %then_22, label %else_24

then_22:                                          ; preds = %merge_20
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.123)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\86\D9\82\D8\A7\D8\B7.load5" = load i64, ptr %alloca, align 8
  %addtmp6 = add i64 %"%\D9\86\D9\82\D8\A7\D8\B7.load5", 1
  store i64 %addtmp6, ptr %alloca, align 8
  br label %merge_23

else_24:                                          ; preds = %merge_20
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.127)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_23

merge_23:                                         ; preds = %else_24, %then_22
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.131)
  %calltmp7 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\B1\D8\AE\D8\B5\D8\A9"(ptr @str.const.69)
  %cmpeqtmp8 = icmp eq i64 %calltmp7, 1
  br i1 %cmpeqtmp8, label %then_25, label %else_27

then_25:                                          ; preds = %merge_23
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.134)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\86\D9\82\D8\A7\D8\B7.load9" = load i64, ptr %alloca, align 8
  %addtmp10 = add i64 %"%\D9\86\D9\82\D8\A7\D8\B7.load9", 1
  store i64 %addtmp10, ptr %alloca, align 8
  br label %merge_26

else_27:                                          ; preds = %merge_23
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.138)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_26

merge_26:                                         ; preds = %else_27, %then_25
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.142)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.144)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\86\D9\82\D8\A7\D8\B7.load11" = load i64, ptr %alloca, align 8
  %addtmp12 = add i64 %"%\D9\86\D9\82\D8\A7\D8\B7.load11", 1
  store i64 %addtmp12, ptr %alloca, align 8
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.148)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.150)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\86\D9\82\D8\A7\D8\B7.load13" = load i64, ptr %alloca, align 8
  %addtmp14 = add i64 %"%\D9\86\D9\82\D8\A7\D8\B7.load13", 1
  store i64 %addtmp14, ptr %alloca, align 8
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.154)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.156)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\86\D9\82\D8\A7\D8\B7.load15" = load i64, ptr %alloca, align 8
  %addtmp16 = add i64 %"%\D9\86\D9\82\D8\A7\D8\B7.load15", 1
  store i64 %addtmp16, ptr %alloca, align 8
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.160)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.162)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\86\D9\82\D8\A7\D8\B7.load17" = load i64, ptr %alloca, align 8
  %addtmp18 = add i64 %"%\D9\86\D9\82\D8\A7\D8\B7.load17", 1
  store i64 %addtmp18, ptr %alloca, align 8
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.166)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.170)
  %"%\D9\86\D9\82\D8\A7\D8\B7.load19" = load i64, ptr %alloca, align 8
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\86\D9\82\D8\A7\D8\B7.load19")
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.173)
  %"%\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load" = load i64, ptr %alloca1, align 8
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load")
  %"%\D9\86\D9\82\D8\A7\D8\B7.load20" = load i64, ptr %alloca, align 8
  %"%\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load21" = load i64, ptr %alloca1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%\D9\86\D9\82\D8\A7\D8\B7.load20", %"%\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load21"
  br i1 %cmpeqtmp22, label %then_28, label %else_30

then_28:                                          ; preds = %merge_26
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.176)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_29

else_30:                                          ; preds = %merge_26
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.180)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_29

merge_29:                                         ; preds = %else_30, %then_28
  ret i64 0
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D9\88\D8\AB\D9\8A\D9\82_\D8\AF\D8\A7\D9\84\D8\A9"(ptr %"\D8\AF\D8\A7\D9\84\D8\A9_\D8\A7\D8\B3\D9\85", ptr %"\D9\88\D8\B5\D9\81_\D8\AF\D8\A7\D9\84\D8\A9", ptr %"\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA", ptr %"\D9\85\D8\AE\D8\B1\D8\AC\D8\A7\D8\AA") {
entry:
  %"\D9\85\D8\AE\D8\B1\D8\AC\D8\A7\D8\AA4" = alloca ptr, align 8
  store ptr %"\D9\85\D8\AE\D8\B1\D8\AC\D8\A7\D8\AA", ptr %"\D9\85\D8\AE\D8\B1\D8\AC\D8\A7\D8\AA4", align 8
  %"\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA3" = alloca ptr, align 8
  store ptr %"\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA", ptr %"\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA3", align 8
  %"\D9\88\D8\B5\D9\81_\D8\AF\D8\A7\D9\84\D8\A92" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81_\D8\AF\D8\A7\D9\84\D8\A9", ptr %"\D9\88\D8\B5\D9\81_\D8\AF\D8\A7\D9\84\D8\A92", align 8
  %"\D8\AF\D8\A7\D9\84\D8\A9_\D8\A7\D8\B3\D9\851" = alloca ptr, align 8
  store ptr %"\D8\AF\D8\A7\D9\84\D8\A9_\D8\A7\D8\B3\D9\85", ptr %"\D8\AF\D8\A7\D9\84\D8\A9_\D8\A7\D8\B3\D9\851", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.184)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\AF\D8\A7\D9\84\D8\A9_\D8\A7\D8\B3\D9\851")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.189)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\88\D8\B5\D9\81_\D8\AF\D8\A7\D9\84\D8\A92")
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.194)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA3")
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.199)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\85\D8\AE\D8\B1\D8\AC\D8\A7\D8\AA4")
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.204)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\AA\D9\88\D9\84\D9\8A\D8\AF_\D8\AA\D9\88\D8\AB\D9\8A\D9\82"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\82\D8\B3\D9\85"(ptr @str.const.208)
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.209)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.213)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp1 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D9\88\D8\AB\D9\8A\D9\82_\D8\AF\D8\A7\D9\84\D8\A9"(ptr @str.const.217, ptr @str.const.218, ptr @str.const.219, ptr @str.const.220)
  %calltmp2 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D9\88\D8\AB\D9\8A\D9\82_\D8\AF\D8\A7\D9\84\D8\A9"(ptr @str.const.221, ptr @str.const.222, ptr @str.const.219, ptr @str.const.220)
  %calltmp3 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D9\88\D8\AB\D9\8A\D9\82_\D8\AF\D8\A7\D9\84\D8\A9"(ptr @str.const.225, ptr @str.const.226, ptr @str.const.219, ptr @str.const.220)
  %calltmp4 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D9\88\D8\AB\D9\8A\D9\82_\D8\AF\D8\A7\D9\84\D8\A9"(ptr @str.const.229, ptr @str.const.230, ptr @str.const.219, ptr @str.const.220)
  %calltmp5 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D9\88\D8\AB\D9\8A\D9\82_\D8\AF\D8\A7\D9\84\D8\A9"(ptr @str.const.233, ptr @str.const.234, ptr @str.const.235, ptr @str.const.220)
  %calltmp6 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AA\D9\88\D8\AB\D9\8A\D9\82_\D8\AF\D8\A7\D9\84\D8\A9"(ptr @str.const.237, ptr @str.const.238, ptr @str.const.239, ptr @str.const.220)
  %calltmp7 = call i64 @"\D8\AD\D9\82\D9\84_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.241, i64 6)
  %calltmp8 = call i64 @"\D8\AD\D9\82\D9\84"(ptr @str.const.242, ptr @str.const.243)
  ret i64 0
}

define i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D9\86\D8\B4\D8\B1"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\82\D8\B3\D9\85"(ptr @str.const.244)
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.245)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.249)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp1 = call i64 @"\D8\AD\D9\82\D9\84_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.253, i64 156)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.254)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.258)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.262)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  br label %while_cond_31

while_cond_31:                                    ; preds = %while_body_32, %entry
  %"%\D8\AE\D8\B7\D9\88\D8\A9.load" = load i64, ptr %alloca, align 8
  %cmplttmp = icmp slt i64 %"%\D8\AE\D8\B7\D9\88\D8\A9.load", 30
  br i1 %cmplttmp, label %while_body_32, label %while_exit_33

while_body_32:                                    ; preds = %while_cond_31
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.264)
  %"%\D8\AE\D8\B7\D9\88\D8\A9.load2" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\AE\D8\B7\D9\88\D8\A9.load2", 1
  store i64 %addtmp, ptr %alloca, align 8
  br label %while_cond_31

while_exit_33:                                    ; preds = %while_cond_31
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.266)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.270)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.274)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.278)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.282)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.286)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp3 = call i64 @"\D8\AD\D9\82\D9\84_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.290, i64 1247)
  %calltmp4 = call i64 @"\D8\AD\D9\82\D9\84_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.291, i64 8)
  %calltmp5 = call i64 @"\D8\AD\D9\82\D9\84_\D8\B1\D9\82\D9\85\D9\8A"(ptr @str.const.292, i64 34)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.293)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.297)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.301)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D9\84\D9\81_\D8\AD\D8\B2\D9\85\D8\A9"()
  %calltmp1 = call i64 @"\D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA_\D9\86\D8\B4\D8\B1"()
  %calltmp2 = call i64 @"\D8\AA\D9\88\D9\84\D9\8A\D8\AF_\D8\AA\D9\88\D8\AB\D9\8A\D9\82"()
  %calltmp3 = call i64 @"\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9_\D9\86\D8\B4\D8\B1"()
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.305)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp4 = call i64 @"\D8\AE\D8\B7_\D8\B9\D8\B1\D9\8A\D8\B6"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.309)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp5 = call i64 @"\D8\AE\D8\B7_\D8\B9\D8\B1\D9\8A\D8\B6"()
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
