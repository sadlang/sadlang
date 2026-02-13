; ModuleID = '06_complex_interactive_docs'
source_filename = "06_complex_interactive_docs"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [9 x i8] c"  [\E2\9C\93] \00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [9 x i8] c"  [\E2\9C\97] \00", align 1
@str.const.7 = private unnamed_addr constant [2 x i8] c"<\00", align 1
@str.const.8 = private unnamed_addr constant [2 x i8] c">\00", align 1
@str.const.9 = private unnamed_addr constant [3 x i8] c"</\00", align 1
@str.const.11 = private unnamed_addr constant [19 x i8] c"      \D8\A7\D9\84\D9\83\D9\88\D8\AF: \00", align 1
@str.const.15 = private unnamed_addr constant [23 x i8] c"      \D8\A7\D9\84\D9\85\D8\AA\D9\88\D9\82\D8\B9: \00", align 1
@str.const.16 = private unnamed_addr constant [18 x i8] c" | \D8\A7\D9\84\D9\81\D8\B9\D9\84\D9\8A: \00", align 1
@str.const.17 = private unnamed_addr constant [5 x i8] c" \E2\9C\93\00", align 1
@str.const.27 = private unnamed_addr constant [5 x i8] c" \E2\9C\97\00", align 1
@str.const.31 = private unnamed_addr constant [9 x i8] c"        \00", align 1
@str.const.34 = private unnamed_addr constant [2 x i8] c"[\00", align 1
@str.const.35 = private unnamed_addr constant [2 x i8] c"]\00", align 1
@str.const.36 = private unnamed_addr constant [5 x i8] c"span\00", align 1
@str.const.41 = private unnamed_addr constant [4 x i8] c"pre\00", align 1
@str.const.46 = private unnamed_addr constant [5 x i8] c"mark\00", align 1
@str.const.47 = private unnamed_addr constant [11 x i8] c"\D9\86\D8\AC\D8\AD \E2\9C\93\00", align 1
@str.const.53 = private unnamed_addr constant [11 x i8] c"\D9\81\D8\B4\D9\84 \E2\9C\97\00", align 1
@str.ret = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.58 = private unnamed_addr constant [43 x i8] c"=== \D8\A3\D9\85\D8\AB\D9\84\D8\A9 \D8\A7\D9\84\D8\AC\D9\85\D8\B9 \D9\88\D8\A7\D9\84\D8\B7\D8\B1\D8\AD ===\00", align 1
@str.const.62 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.66 = private unnamed_addr constant [13 x i8] c"\D8\AC\D9\85\D8\B9 5 + 3\00", align 1
@str.const.67 = private unnamed_addr constant [18 x i8] c"\D8\B1\D9\82\D9\85 \D9\86 = 5 + 3\00", align 1
@str.const.68 = private unnamed_addr constant [17 x i8] c"\D8\AC\D9\85\D8\B9 100 + 200\00", align 1
@str.const.69 = private unnamed_addr constant [22 x i8] c"\D8\B1\D9\82\D9\85 \D9\86 = 100 + 200\00", align 1
@str.const.70 = private unnamed_addr constant [13 x i8] c"\D8\AC\D9\85\D8\B9 0 + 0\00", align 1
@str.const.71 = private unnamed_addr constant [18 x i8] c"\D8\B1\D9\82\D9\85 \D9\86 = 0 + 0\00", align 1
@str.const.72 = private unnamed_addr constant [14 x i8] c"\D8\B7\D8\B1\D8\AD 10 - 4\00", align 1
@str.const.73 = private unnamed_addr constant [19 x i8] c"\D8\B1\D9\82\D9\85 \D9\86 = 10 - 4\00", align 1
@str.const.74 = private unnamed_addr constant [15 x i8] c"\D8\B7\D8\B1\D8\AD 50 - 50\00", align 1
@str.const.75 = private unnamed_addr constant [20 x i8] c"\D8\B1\D9\82\D9\85 \D9\86 = 50 - 50\00", align 1
@str.const.76 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.80 = private unnamed_addr constant [39 x i8] c"  \D9\86\D8\AA\D8\A7\D8\A6\D8\AC \D8\A7\D9\84\D8\AC\D9\85\D8\B9 \D9\88\D8\A7\D9\84\D8\B7\D8\B1\D8\AD: \00", align 1
@str.const.81 = private unnamed_addr constant [3 x i8] c"/5\00", align 1
@str.const.85 = private unnamed_addr constant [40 x i8] c"\D9\83\D9\84 \D8\A3\D9\85\D8\AB\D9\84\D8\A9 \D8\A7\D9\84\D8\AC\D9\85\D8\B9 \D9\88\D8\A7\D9\84\D8\B7\D8\B1\D8\AD\00", align 1
@str.const.87 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.91 = private unnamed_addr constant [45 x i8] c"=== \D8\A3\D9\85\D8\AB\D9\84\D8\A9 \D8\A7\D9\84\D8\B6\D8\B1\D8\A8 \D9\88\D8\A7\D9\84\D9\82\D8\B3\D9\85\D8\A9 ===\00", align 1
@str.const.95 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.99 = private unnamed_addr constant [13 x i8] c"\D8\B6\D8\B1\D8\A8 4 * 7\00", align 1
@str.const.100 = private unnamed_addr constant [18 x i8] c"\D8\B1\D9\82\D9\85 \D9\86 = 4 * 7\00", align 1
@str.const.101 = private unnamed_addr constant [15 x i8] c"\D8\B6\D8\B1\D8\A8 0 * 999\00", align 1
@str.const.102 = private unnamed_addr constant [20 x i8] c"\D8\B1\D9\82\D9\85 \D9\86 = 0 * 999\00", align 1
@str.const.103 = private unnamed_addr constant [16 x i8] c"\D9\82\D8\B3\D9\85\D8\A9 20 / 4\00", align 1
@str.const.104 = private unnamed_addr constant [19 x i8] c"\D8\B1\D9\82\D9\85 \D9\86 = 20 / 4\00", align 1
@str.const.105 = private unnamed_addr constant [18 x i8] c"\D9\82\D8\B3\D9\85\D8\A9 100 / 10\00", align 1
@str.const.106 = private unnamed_addr constant [21 x i8] c"\D8\B1\D9\82\D9\85 \D9\86 = 100 / 10\00", align 1
@str.const.107 = private unnamed_addr constant [16 x i8] c"\D8\A8\D8\A7\D9\82\D9\8A 17 % 5\00", align 1
@str.const.108 = private unnamed_addr constant [19 x i8] c"\D8\B1\D9\82\D9\85 \D9\86 = 17 % 5\00", align 1
@str.const.109 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.113 = private unnamed_addr constant [41 x i8] c"  \D9\86\D8\AA\D8\A7\D8\A6\D8\AC \D8\A7\D9\84\D8\B6\D8\B1\D8\A8 \D9\88\D8\A7\D9\84\D9\82\D8\B3\D9\85\D8\A9: \00", align 1
@str.const.118 = private unnamed_addr constant [42 x i8] c"\D9\83\D9\84 \D8\A3\D9\85\D8\AB\D9\84\D8\A9 \D8\A7\D9\84\D8\B6\D8\B1\D8\A8 \D9\88\D8\A7\D9\84\D9\82\D8\B3\D9\85\D8\A9\00", align 1
@str.const.120 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.124 = private unnamed_addr constant [32 x i8] c"=== \D8\A3\D9\85\D8\AB\D9\84\D8\A9 \D8\A7\D9\84\D9\86\D8\B5\D9\88\D8\B5 ===\00", align 1
@str.const.128 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.132 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7\00", align 1
@str.const.133 = private unnamed_addr constant [18 x i8] c"\D8\B7\D9\88\D9\84 \D9\85\D8\B1\D8\AD\D8\A8\D8\A7\00", align 1
@str.const.134 = private unnamed_addr constant [24 x i8] c"\D8\B7\D9\88\D9\84_\D9\86\D8\B5(\D9\85\D8\B1\D8\AD\D8\A8\D8\A7)\00", align 1
@str.const.135 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.136 = private unnamed_addr constant [21 x i8] c"\D8\B7\D9\88\D9\84 \D9\86\D8\B5 \D9\81\D8\A7\D8\B1\D8\BA\00", align 1
@str.const.137 = private unnamed_addr constant [22 x i8] c"\D8\B7\D9\88\D9\84_\D9\86\D8\B5(\D9\81\D8\A7\D8\B1\D8\BA)\00", align 1
@str.const.138 = private unnamed_addr constant [40 x i8] c"\D8\B3\D9\84\D8\A7\D9\85 \D8\B9\D9\84\D9\8A\D9\83\D9\85 \D9\88\D8\B1\D8\AD\D9\85\D8\A9 \D8\A7\D9\84\D9\84\D9\87\00", align 1
@str.const.139 = private unnamed_addr constant [27 x i8] c"\D8\B7\D9\88\D9\84 \D8\AC\D9\85\D9\84\D8\A9 \D8\B7\D9\88\D9\8A\D9\84\D8\A9\00", align 1
@str.const.141 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.145 = private unnamed_addr constant [28 x i8] c"  \D9\86\D8\AA\D8\A7\D8\A6\D8\AC \D8\A7\D9\84\D9\86\D8\B5\D9\88\D8\B5: \00", align 1
@str.const.146 = private unnamed_addr constant [3 x i8] c"/3\00", align 1
@str.const.150 = private unnamed_addr constant [29 x i8] c"\D9\83\D9\84 \D8\A3\D9\85\D8\AB\D9\84\D8\A9 \D8\A7\D9\84\D9\86\D8\B5\D9\88\D8\B5\00", align 1
@str.const.152 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.156 = private unnamed_addr constant [44 x i8] c"=== \D8\AA\D9\88\D9\84\D9\8A\D8\AF \D8\B5\D9\81\D8\AD\D8\A9 \D8\A3\D9\85\D8\AB\D9\84\D8\A9 HTML ===\00", align 1
@str.const.160 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.164 = private unnamed_addr constant [18 x i8] c"  <!DOCTYPE html>\00", align 1
@str.const.168 = private unnamed_addr constant [29 x i8] c"  <html lang=\22ar\22 dir=\22rtl\22>\00", align 1
@str.const.172 = private unnamed_addr constant [9 x i8] c"  <head>\00", align 1
@str.const.176 = private unnamed_addr constant [6 x i8] c"title\00", align 1
@str.const.177 = private unnamed_addr constant [41 x i8] c"\D8\A3\D9\85\D8\AB\D9\84\D8\A9 \D8\AA\D9\88\D8\AB\D9\8A\D9\82\D9\8A\D8\A9 \D8\AA\D9\81\D8\A7\D8\B9\D9\84\D9\8A\D8\A9\00", align 1
@str.const.178 = private unnamed_addr constant [5 x i8] c"    \00", align 1
@str.const.182 = private unnamed_addr constant [10 x i8] c"  </head>\00", align 1
@str.const.186 = private unnamed_addr constant [9 x i8] c"  <body>\00", align 1
@str.const.190 = private unnamed_addr constant [3 x i8] c"h1\00", align 1
@str.const.191 = private unnamed_addr constant [45 x i8] c"\D9\85\D9\83\D8\AA\D8\A8\D8\A9 \D8\A7\D9\84\D8\A3\D9\85\D8\AB\D9\84\D8\A9 \D8\A7\D9\84\D8\AA\D9\81\D8\A7\D8\B9\D9\84\D9\8A\D8\A9\00", align 1
@str.const.196 = private unnamed_addr constant [3 x i8] c"h2\00", align 1
@str.const.197 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D9\81\D8\A6\D8\A7\D8\AA\00", align 1
@str.const.202 = private unnamed_addr constant [9 x i8] c"    <ul>\00", align 1
@str.const.206 = private unnamed_addr constant [3 x i8] c"li\00", align 1
@str.const.207 = private unnamed_addr constant [15 x i8] c"\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.209 = private unnamed_addr constant [9 x i8] c"\D9\86\D8\B5\D9\88\D8\B5\00", align 1
@str.const.211 = private unnamed_addr constant [9 x i8] c"\D9\85\D9\86\D8\B7\D9\82\00", align 1
@str.const.212 = private unnamed_addr constant [7 x i8] c"      \00", align 1
@str.const.224 = private unnamed_addr constant [10 x i8] c"    </ul>\00", align 1
@str.const.229 = private unnamed_addr constant [30 x i8] c"\D8\A3\D9\85\D8\AB\D9\84\D8\A9 \D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.234 = private unnamed_addr constant [14 x i8] c"    <section>\00", align 1
@str.const.238 = private unnamed_addr constant [18 x i8] c"\D8\AC\D9\85\D8\B9 \D8\B9\D8\AF\D8\AF\D9\8A\D9\86\00", align 1
@str.const.241 = private unnamed_addr constant [18 x i8] c"\D8\B6\D8\B1\D8\A8 \D8\B9\D8\AF\D8\AF\D9\8A\D9\86\00", align 1
@str.const.244 = private unnamed_addr constant [9 x i8] c"\D9\82\D8\B3\D9\85\D8\A9\00", align 1
@str.const.247 = private unnamed_addr constant [22 x i8] c"\D8\A8\D8\A7\D9\82\D9\8A \D8\A7\D9\84\D9\82\D8\B3\D9\85\D8\A9\00", align 1
@str.const.250 = private unnamed_addr constant [15 x i8] c"    </section>\00", align 1
@str.const.255 = private unnamed_addr constant [24 x i8] c"\D8\A3\D9\85\D8\AB\D9\84\D8\A9 \D8\A7\D9\84\D9\86\D8\B5\D9\88\D8\B5\00", align 1
@str.const.264 = private unnamed_addr constant [12 x i8] c"\D8\B7\D9\88\D9\84 \D9\86\D8\B5\00", align 1
@str.const.267 = private unnamed_addr constant [14 x i8] c"\D9\86\D8\B5 \D9\81\D8\A7\D8\B1\D8\BA\00", align 1
@str.const.274 = private unnamed_addr constant [10 x i8] c"  </body>\00", align 1
@str.const.278 = private unnamed_addr constant [10 x i8] c"  </html>\00", align 1
@str.const.282 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.286 = private unnamed_addr constant [25 x i8] c"\D8\AA\D9\88\D9\84\D9\8A\D8\AF \D8\B5\D9\81\D8\AD\D8\A9 HTML\00", align 1
@str.const.287 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.291 = private unnamed_addr constant [34 x i8] c"=== \D8\A7\D9\84\D9\85\D9\84\D8\AE\D8\B5 \D8\A7\D9\84\D8\B4\D8\A7\D9\85\D9\84 ===\00", align 1
@str.const.295 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.299 = private unnamed_addr constant [114 x i8] c"  \E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.303 = private unnamed_addr constant [63 x i8] c"  \E2\95\91  \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D8\A3\D9\85\D8\AB\D9\84\D8\A9 \D8\A7\D9\84\D8\AA\D9\88\D8\AB\D9\8A\D9\82\D9\8A\D8\A9        \E2\95\91\00", align 1
@str.const.307 = private unnamed_addr constant [114 x i8] c"  \E2\95\A0\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\A3\00", align 1
@str.const.311 = private unnamed_addr constant [37 x i8] c"  \E2\95\91  \D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A \D8\A7\D9\84\D8\A3\D9\85\D8\AB\D9\84\D8\A9: \00", align 1
@str.const.312 = private unnamed_addr constant [16 x i8] c"            \E2\95\91\00", align 1
@str.const.316 = private unnamed_addr constant [20 x i8] c"  \E2\95\91  \D9\86\D8\A7\D8\AC\D8\AD\D8\A9: \00", align 1
@str.const.317 = private unnamed_addr constant [24 x i8] c"                    \E2\95\91\00", align 1
@str.const.321 = private unnamed_addr constant [20 x i8] c"  \E2\95\91  \D9\81\D8\A7\D8\B4\D9\84\D8\A9: \00", align 1
@str.const.322 = private unnamed_addr constant [25 x i8] c"                     \E2\95\91\00", align 1
@str.const.326 = private unnamed_addr constant [31 x i8] c"  \E2\95\91  \D9\86\D8\B3\D8\A8\D8\A9 \D8\A7\D9\84\D9\86\D8\AC\D8\A7\D8\AD: \00", align 1
@str.const.327 = private unnamed_addr constant [15 x i8] c"%          \E2\95\91\00", align 1
@str.const.335 = private unnamed_addr constant [24 x i8] c"  \E2\95\91  \D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA: \00", align 1
@str.const.336 = private unnamed_addr constant [24 x i8] c" \D9\85\D8\AB\D8\A7\D9\84           \E2\95\91\00", align 1
@str.const.340 = private unnamed_addr constant [18 x i8] c"  \E2\95\91  \D9\86\D8\B5\D9\88\D8\B5: \00", align 1
@str.const.341 = private unnamed_addr constant [29 x i8] c" \D9\85\D8\AB\D8\A7\D9\84                \E2\95\91\00", align 1
@str.const.345 = private unnamed_addr constant [51 x i8] c"  \E2\95\91  \D8\B9\D8\AF\D8\AF \D8\A7\D9\84\D9\81\D8\A6\D8\A7\D8\AA: 3                  \E2\95\91\00", align 1
@str.const.349 = private unnamed_addr constant [114 x i8] c"  \E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.353 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.357 = private unnamed_addr constant [41 x i8] c"\D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA \D9\86\D8\A7\D8\AC\D8\AD\D8\A9\00", align 1
@str.const.359 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.363 = private unnamed_addr constant [157 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.367 = private unnamed_addr constant [88 x i8] c"\E2\95\91  \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D8\AA\D9\88\D8\AB\D9\8A\D9\82 \D8\A7\D9\84\D8\AA\D9\81\D8\A7\D8\B9\D9\84\D9\8A \D9\85\D8\B9 \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA \D9\85\D8\AF\D9\85\D8\AC\D8\A9      \E2\95\91\00", align 1
@str.const.371 = private unnamed_addr constant [157 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.375 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.379 = private unnamed_addr constant [71 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D8\AA\D9\88\D8\AB\D9\8A\D9\82 \D8\A7\D9\84\D8\AA\D9\81\D8\A7\D8\B9\D9\84\D9\8A \D8\A8\D9\86\D8\AC\D8\A7\D8\AD ===\00", align 1

define i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr %"\D9\88\D8\B5\D9\81", i64 %"\D9\86\D8\A7\D8\AC\D8\AD") {
entry:
  %"\D9\86\D8\A7\D8\AC\D8\AD2" = alloca i64, align 8
  store i64 %"\D9\86\D8\A7\D8\AC\D8\AD", ptr %"\D9\86\D8\A7\D8\AC\D8\AD2", align 8
  %"\D9\88\D8\B5\D9\811" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81", ptr %"\D9\88\D8\B5\D9\811", align 8
  %"%\D9\86\D8\A7\D8\AC\D8\AD.load" = load i64, ptr %"\D9\86\D8\A7\D8\AC\D8\AD2", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\A7\D8\AC\D8\AD.load", 1
  br i1 %cmpeqtmp, label %then_0, label %else_2

then_0:                                           ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\811")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\88\D8\B5\D9\811", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_1

else_2:                                           ; preds = %entry
  %len13 = call i64 @strlen(ptr @str.const.3)
  %len24 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\811")
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %4 = call ptr @memcpy(ptr %concat.buf7, ptr @str.const.3, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %5 = call ptr @memcpy(ptr %dest28, ptr %"\D9\88\D8\B5\D9\811", i64 %copylen29)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf7)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_1

merge_1:                                          ; preds = %else_2, %then_0
  ret i64 0
}

define ptr @"\D9\88\D8\B3\D9\85"(ptr %"\D8\A7\D8\B3\D9\85_\D9\88", ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\89") {
entry:
  %"\D9\85\D8\AD\D8\AA\D9\88\D9\892" = alloca ptr, align 8
  store ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\892", align 8
  %"\D8\A7\D8\B3\D9\85_\D9\881" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85_\D9\88", ptr %"\D8\A7\D8\B3\D9\85_\D9\881", align 8
  %len1 = call i64 @strlen(ptr @str.const.7)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\85_\D9\881")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.7, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D8\B3\D9\85_\D9\881", i64 %copylen2)
  %len13 = call i64 @strlen(ptr %concat.buf)
  %len24 = call i64 @strlen(ptr @str.const.8)
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %2 = call ptr @memcpy(ptr %concat.buf7, ptr %concat.buf, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %3 = call ptr @memcpy(ptr %dest28, ptr @str.const.8, i64 %copylen29)
  %alloca = alloca ptr, align 8
  store ptr %concat.buf7, ptr %alloca, align 8
  %len110 = call i64 @strlen(ptr @str.const.9)
  %len211 = call i64 @strlen(ptr %"\D8\A7\D8\B3\D9\85_\D9\881")
  %totallen12 = add i64 %len110, %len211
  %bufsize13 = add i64 %totallen12, 1
  %concat.buf14 = alloca i8, i64 %bufsize13, align 1
  %4 = call ptr @memcpy(ptr %concat.buf14, ptr @str.const.9, i64 %len110)
  %dest215 = getelementptr i8, ptr %concat.buf14, i64 %len110
  %copylen216 = add i64 %len211, 1
  %5 = call ptr @memcpy(ptr %dest215, ptr %"\D8\A7\D8\B3\D9\85_\D9\881", i64 %copylen216)
  %len117 = call i64 @strlen(ptr %concat.buf14)
  %len218 = call i64 @strlen(ptr @str.const.8)
  %totallen19 = add i64 %len117, %len218
  %bufsize20 = add i64 %totallen19, 1
  %concat.buf21 = alloca i8, i64 %bufsize20, align 1
  %6 = call ptr @memcpy(ptr %concat.buf21, ptr %concat.buf14, i64 %len117)
  %dest222 = getelementptr i8, ptr %concat.buf21, i64 %len117
  %copylen223 = add i64 %len218, 1
  %7 = call ptr @memcpy(ptr %dest222, ptr @str.const.8, i64 %copylen223)
  %alloca24 = alloca ptr, align 8
  store ptr %concat.buf21, ptr %alloca24, align 8
  %len125 = call i64 @strlen(ptr %alloca)
  %len226 = call i64 @strlen(ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\892")
  %totallen27 = add i64 %len125, %len226
  %bufsize28 = add i64 %totallen27, 1
  %concat.buf29 = alloca i8, i64 %bufsize28, align 1
  %8 = call ptr @memcpy(ptr %concat.buf29, ptr %alloca, i64 %len125)
  %dest230 = getelementptr i8, ptr %concat.buf29, i64 %len125
  %copylen231 = add i64 %len226, 1
  %9 = call ptr @memcpy(ptr %dest230, ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\892", i64 %copylen231)
  %len132 = call i64 @strlen(ptr %concat.buf29)
  %len233 = call i64 @strlen(ptr %alloca24)
  %totallen34 = add i64 %len132, %len233
  %bufsize35 = add i64 %totallen34, 1
  %concat.buf36 = alloca i8, i64 %bufsize35, align 1
  %10 = call ptr @memcpy(ptr %concat.buf36, ptr %concat.buf29, i64 %len132)
  %dest237 = getelementptr i8, ptr %concat.buf36, i64 %len132
  %copylen238 = add i64 %len233, 1
  %11 = call ptr @memcpy(ptr %dest237, ptr %alloca24, i64 %copylen238)
  %loadtmp = load ptr, ptr %concat.buf36, align 8
  ret ptr %loadtmp
}

define i64 @"\D9\86\D9\81\D8\B0_\D8\AC\D9\85\D8\B9"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %addtmp = add i64 %"%\D8\A3.load", %"%\D8\A8.load"
  ret i64 %addtmp
}

define i64 @"\D9\86\D9\81\D8\B0_\D8\B6\D8\B1\D8\A8"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %multmp = mul i64 %"%\D8\A3.load", %"%\D8\A8.load"
  ret i64 %multmp
}

define i64 @"\D9\86\D9\81\D8\B0_\D8\B7\D8\B1\D8\AD"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %subtmp = sub i64 %"%\D8\A3.load", %"%\D8\A8.load"
  ret i64 %subtmp
}

define i64 @"\D9\86\D9\81\D8\B0_\D9\82\D8\B3\D9\85\D8\A9"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A8.load", 0
  br i1 %cmpeqtmp, label %then_3, label %merge_4

then_3:                                           ; preds = %entry
  ret i64 0

merge_4:                                          ; preds = %entry
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load3" = load i64, ptr %"\D8\A82", align 8
  %divtmp = sdiv i64 %"%\D8\A3.load", %"%\D8\A8.load3"
  ret i64 %divtmp
}

define i64 @"\D9\86\D9\81\D8\B0_\D8\A8\D8\A7\D9\82\D9\8A"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A8.load", 0
  br i1 %cmpeqtmp, label %then_5, label %merge_6

then_5:                                           ; preds = %entry
  ret i64 0

merge_6:                                          ; preds = %entry
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load3" = load i64, ptr %"\D8\A82", align 8
  %modtmp = srem i64 %"%\D8\A3.load", %"%\D8\A8.load3"
  ret i64 %modtmp
}

define i64 @"\D9\86\D9\81\D8\B0_\D8\B7\D9\88\D9\84"(ptr %"\D9\82\D9\8A\D9\85\D8\A9_\D9\86") {
entry:
  %"\D9\82\D9\8A\D9\85\D8\A9_\D9\861" = alloca ptr, align 8
  store ptr %"\D9\82\D9\8A\D9\85\D8\A9_\D9\86", ptr %"\D9\82\D9\8A\D9\85\D8\A9_\D9\861", align 8
  %strlen.ret = call i64 @strlen(ptr %"\D9\82\D9\8A\D9\85\D8\A9_\D9\861")
  ret i64 %strlen.ret
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A"(ptr %"\D8\A7\D8\B3\D9\85_\D9\85", ptr %"\D9\83\D9\88\D8\AF_\D9\85", i64 %"\D9\81\D8\B9\D9\84\D9\8A", i64 %"\D9\85\D8\AA\D9\88\D9\82\D8\B9") {
entry:
  %"\D9\85\D8\AA\D9\88\D9\82\D8\B94" = alloca i64, align 8
  store i64 %"\D9\85\D8\AA\D9\88\D9\82\D8\B9", ptr %"\D9\85\D8\AA\D9\88\D9\82\D8\B94", align 8
  %"\D9\81\D8\B9\D9\84\D9\8A3" = alloca i64, align 8
  store i64 %"\D9\81\D8\B9\D9\84\D9\8A", ptr %"\D9\81\D8\B9\D9\84\D9\8A3", align 8
  %"\D9\83\D9\88\D8\AF_\D9\852" = alloca ptr, align 8
  store ptr %"\D9\83\D9\88\D8\AF_\D9\85", ptr %"\D9\83\D9\88\D8\AF_\D9\852", align 8
  %"\D8\A7\D8\B3\D9\85_\D9\851" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85_\D9\85", ptr %"\D8\A7\D8\B3\D9\85_\D9\851", align 8
  %alloca = alloca ptr, align 8
  %alloca5 = alloca ptr, align 8
  %"%\D9\81\D8\B9\D9\84\D9\8A.load" = load i64, ptr %"\D9\81\D8\B9\D9\84\D9\8A3", align 8
  %"%\D9\85\D8\AA\D9\88\D9\82\D8\B9.load" = load i64, ptr %"\D9\85\D8\AA\D9\88\D9\82\D8\B94", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\81\D8\B9\D9\84\D9\8A.load", %"%\D9\85\D8\AA\D9\88\D9\82\D8\B9.load"
  br i1 %cmpeqtmp, label %then_7, label %else_9

then_7:                                           ; preds = %entry
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr %"\D8\A7\D8\B3\D9\85_\D9\851", i64 1)
  %len1 = call i64 @strlen(ptr @str.const.11)
  %len2 = call i64 @strlen(ptr %"\D9\83\D9\88\D8\AF_\D9\852")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.11, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\83\D9\88\D8\AF_\D9\852", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len16 = call i64 @strlen(ptr @str.const.15)
  %len27 = call i64 @strlen(ptr %alloca)
  %totallen8 = add i64 %len16, %len27
  %bufsize9 = add i64 %totallen8, 1
  %concat.buf10 = alloca i8, i64 %bufsize9, align 1
  %4 = call ptr @memcpy(ptr %concat.buf10, ptr @str.const.15, i64 %len16)
  %dest211 = getelementptr i8, ptr %concat.buf10, i64 %len16
  %copylen212 = add i64 %len27, 1
  %5 = call ptr @memcpy(ptr %dest211, ptr %alloca, i64 %copylen212)
  %len113 = call i64 @strlen(ptr %concat.buf10)
  %len214 = call i64 @strlen(ptr @str.const.16)
  %totallen15 = add i64 %len113, %len214
  %bufsize16 = add i64 %totallen15, 1
  %concat.buf17 = alloca i8, i64 %bufsize16, align 1
  %6 = call ptr @memcpy(ptr %concat.buf17, ptr %concat.buf10, i64 %len113)
  %dest218 = getelementptr i8, ptr %concat.buf17, i64 %len113
  %copylen219 = add i64 %len214, 1
  %7 = call ptr @memcpy(ptr %dest218, ptr @str.const.16, i64 %copylen219)
  %len120 = call i64 @strlen(ptr %concat.buf17)
  %len221 = call i64 @strlen(ptr %alloca5)
  %totallen22 = add i64 %len120, %len221
  %bufsize23 = add i64 %totallen22, 1
  %concat.buf24 = alloca i8, i64 %bufsize23, align 1
  %8 = call ptr @memcpy(ptr %concat.buf24, ptr %concat.buf17, i64 %len120)
  %dest225 = getelementptr i8, ptr %concat.buf24, i64 %len120
  %copylen226 = add i64 %len221, 1
  %9 = call ptr @memcpy(ptr %dest225, ptr %alloca5, i64 %copylen226)
  %len127 = call i64 @strlen(ptr %concat.buf24)
  %len228 = call i64 @strlen(ptr @str.const.17)
  %totallen29 = add i64 %len127, %len228
  %bufsize30 = add i64 %totallen29, 1
  %concat.buf31 = alloca i8, i64 %bufsize30, align 1
  %10 = call ptr @memcpy(ptr %concat.buf31, ptr %concat.buf24, i64 %len127)
  %dest232 = getelementptr i8, ptr %concat.buf31, i64 %len127
  %copylen233 = add i64 %len228, 1
  %11 = call ptr @memcpy(ptr %dest232, ptr @str.const.17, i64 %copylen233)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf31)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 1

else_9:                                           ; preds = %entry
  %calltmp34 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr %"\D8\A7\D8\B3\D9\85_\D9\851", i64 0)
  %len135 = call i64 @strlen(ptr @str.const.11)
  %len236 = call i64 @strlen(ptr %"\D9\83\D9\88\D8\AF_\D9\852")
  %totallen37 = add i64 %len135, %len236
  %bufsize38 = add i64 %totallen37, 1
  %concat.buf39 = alloca i8, i64 %bufsize38, align 1
  %14 = call ptr @memcpy(ptr %concat.buf39, ptr @str.const.11, i64 %len135)
  %dest240 = getelementptr i8, ptr %concat.buf39, i64 %len135
  %copylen241 = add i64 %len236, 1
  %15 = call ptr @memcpy(ptr %dest240, ptr %"\D9\83\D9\88\D8\AF_\D9\852", i64 %copylen241)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf39)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len142 = call i64 @strlen(ptr @str.const.15)
  %len243 = call i64 @strlen(ptr %alloca)
  %totallen44 = add i64 %len142, %len243
  %bufsize45 = add i64 %totallen44, 1
  %concat.buf46 = alloca i8, i64 %bufsize45, align 1
  %18 = call ptr @memcpy(ptr %concat.buf46, ptr @str.const.15, i64 %len142)
  %dest247 = getelementptr i8, ptr %concat.buf46, i64 %len142
  %copylen248 = add i64 %len243, 1
  %19 = call ptr @memcpy(ptr %dest247, ptr %alloca, i64 %copylen248)
  %len149 = call i64 @strlen(ptr %concat.buf46)
  %len250 = call i64 @strlen(ptr @str.const.16)
  %totallen51 = add i64 %len149, %len250
  %bufsize52 = add i64 %totallen51, 1
  %concat.buf53 = alloca i8, i64 %bufsize52, align 1
  %20 = call ptr @memcpy(ptr %concat.buf53, ptr %concat.buf46, i64 %len149)
  %dest254 = getelementptr i8, ptr %concat.buf53, i64 %len149
  %copylen255 = add i64 %len250, 1
  %21 = call ptr @memcpy(ptr %dest254, ptr @str.const.16, i64 %copylen255)
  %len156 = call i64 @strlen(ptr %concat.buf53)
  %len257 = call i64 @strlen(ptr %alloca5)
  %totallen58 = add i64 %len156, %len257
  %bufsize59 = add i64 %totallen58, 1
  %concat.buf60 = alloca i8, i64 %bufsize59, align 1
  %22 = call ptr @memcpy(ptr %concat.buf60, ptr %concat.buf53, i64 %len156)
  %dest261 = getelementptr i8, ptr %concat.buf60, i64 %len156
  %copylen262 = add i64 %len257, 1
  %23 = call ptr @memcpy(ptr %dest261, ptr %alloca5, i64 %copylen262)
  %len163 = call i64 @strlen(ptr %concat.buf60)
  %len264 = call i64 @strlen(ptr @str.const.27)
  %totallen65 = add i64 %len163, %len264
  %bufsize66 = add i64 %totallen65, 1
  %concat.buf67 = alloca i8, i64 %bufsize66, align 1
  %24 = call ptr @memcpy(ptr %concat.buf67, ptr %concat.buf60, i64 %len163)
  %dest268 = getelementptr i8, ptr %concat.buf67, i64 %len163
  %copylen269 = add i64 %len264, 1
  %25 = call ptr @memcpy(ptr %dest268, ptr @str.const.27, i64 %copylen269)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf67)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0

merge_8:                                          ; No predecessors!
  ret i64 0
}

define ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\85\D8\AB\D8\A7\D9\84_html"(ptr %"\D8\A7\D8\B3\D9\85_\D9\85", ptr %"\D9\83\D9\88\D8\AF_\D9\85", ptr %"\D9\81\D8\A6\D8\A9_\D9\85", i64 %"\D9\86\D8\A7\D8\AC\D8\AD") {
entry:
  %"\D9\86\D8\A7\D8\AC\D8\AD4" = alloca i64, align 8
  store i64 %"\D9\86\D8\A7\D8\AC\D8\AD", ptr %"\D9\86\D8\A7\D8\AC\D8\AD4", align 8
  %"\D9\81\D8\A6\D8\A9_\D9\853" = alloca ptr, align 8
  store ptr %"\D9\81\D8\A6\D8\A9_\D9\85", ptr %"\D9\81\D8\A6\D8\A9_\D9\853", align 8
  %"\D9\83\D9\88\D8\AF_\D9\852" = alloca ptr, align 8
  store ptr %"\D9\83\D9\88\D8\AF_\D9\85", ptr %"\D9\83\D9\88\D8\AF_\D9\852", align 8
  %"\D8\A7\D8\B3\D9\85_\D9\851" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85_\D9\85", ptr %"\D8\A7\D8\B3\D9\85_\D9\851", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len1 = call i64 @strlen(ptr @str.const.34)
  %len2 = call i64 @strlen(ptr %"\D9\81\D8\A6\D8\A9_\D9\853")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %1 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.34, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %2 = call ptr @memcpy(ptr %dest2, ptr %"\D9\81\D8\A6\D8\A9_\D9\853", i64 %copylen2)
  %len15 = call i64 @strlen(ptr %concat.buf)
  %len26 = call i64 @strlen(ptr @str.const.35)
  %totallen7 = add i64 %len15, %len26
  %bufsize8 = add i64 %totallen7, 1
  %concat.buf9 = alloca i8, i64 %bufsize8, align 1
  %3 = call ptr @memcpy(ptr %concat.buf9, ptr %concat.buf, i64 %len15)
  %dest210 = getelementptr i8, ptr %concat.buf9, i64 %len15
  %copylen211 = add i64 %len26, 1
  %4 = call ptr @memcpy(ptr %dest210, ptr @str.const.35, i64 %copylen211)
  %calltmp = call ptr @"\D9\88\D8\B3\D9\85"(ptr @str.const.36, ptr %concat.buf9)
  %alloca = alloca ptr, align 8
  store ptr %calltmp, ptr %alloca, align 8
  %len112 = call i64 @strlen(ptr @str.const.31)
  %len213 = call i64 @strlen(ptr %alloca)
  %totallen14 = add i64 %len112, %len213
  %bufsize15 = add i64 %totallen14, 1
  %concat.buf16 = alloca i8, i64 %bufsize15, align 1
  %5 = call ptr @memcpy(ptr %concat.buf16, ptr @str.const.31, i64 %len112)
  %dest217 = getelementptr i8, ptr %concat.buf16, i64 %len112
  %copylen218 = add i64 %len213, 1
  %6 = call ptr @memcpy(ptr %dest217, ptr %alloca, i64 %copylen218)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf16)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp19 = call ptr @"\D9\88\D8\B3\D9\85"(ptr @str.const.41, ptr %"\D9\83\D9\88\D8\AF_\D9\852")
  %alloca20 = alloca ptr, align 8
  store ptr %calltmp19, ptr %alloca20, align 8
  %len121 = call i64 @strlen(ptr @str.const.31)
  %len222 = call i64 @strlen(ptr %alloca20)
  %totallen23 = add i64 %len121, %len222
  %bufsize24 = add i64 %totallen23, 1
  %concat.buf25 = alloca i8, i64 %bufsize24, align 1
  %9 = call ptr @memcpy(ptr %concat.buf25, ptr @str.const.31, i64 %len121)
  %dest226 = getelementptr i8, ptr %concat.buf25, i64 %len121
  %copylen227 = add i64 %len222, 1
  %10 = call ptr @memcpy(ptr %dest226, ptr %alloca20, i64 %copylen227)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf25)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\86\D8\A7\D8\AC\D8\AD.load" = load i64, ptr %"\D9\86\D8\A7\D8\AC\D8\AD4", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\A7\D8\AC\D8\AD.load", 1
  br i1 %cmpeqtmp, label %then_10, label %else_12

then_10:                                          ; preds = %entry
  %calltmp28 = call ptr @"\D9\88\D8\B3\D9\85"(ptr @str.const.46, ptr @str.const.47)
  %alloca29 = alloca ptr, align 8
  store ptr %calltmp28, ptr %alloca29, align 8
  %len130 = call i64 @strlen(ptr @str.const.31)
  %len231 = call i64 @strlen(ptr %alloca29)
  %totallen32 = add i64 %len130, %len231
  %bufsize33 = add i64 %totallen32, 1
  %concat.buf34 = alloca i8, i64 %bufsize33, align 1
  %13 = call ptr @memcpy(ptr %concat.buf34, ptr @str.const.31, i64 %len130)
  %dest235 = getelementptr i8, ptr %concat.buf34, i64 %len130
  %copylen236 = add i64 %len231, 1
  %14 = call ptr @memcpy(ptr %dest235, ptr %alloca29, i64 %copylen236)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf34)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_11

else_12:                                          ; preds = %entry
  %calltmp37 = call ptr @"\D9\88\D8\B3\D9\85"(ptr @str.const.46, ptr @str.const.53)
  %alloca38 = alloca ptr, align 8
  store ptr %calltmp37, ptr %alloca38, align 8
  %len139 = call i64 @strlen(ptr @str.const.31)
  %len240 = call i64 @strlen(ptr %alloca38)
  %totallen41 = add i64 %len139, %len240
  %bufsize42 = add i64 %totallen41, 1
  %concat.buf43 = alloca i8, i64 %bufsize42, align 1
  %17 = call ptr @memcpy(ptr %concat.buf43, ptr @str.const.31, i64 %len139)
  %dest244 = getelementptr i8, ptr %concat.buf43, i64 %len139
  %copylen245 = add i64 %len240, 1
  %18 = call ptr @memcpy(ptr %dest244, ptr %alloca38, i64 %copylen245)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf43)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_11

merge_11:                                         ; preds = %else_12, %then_10
  ret ptr @str.ret
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AC\D9\85\D8\B9_\D9\88\D8\B7\D8\B1\D8\AD"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.58)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.62)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %calltmp = call i64 @"\D9\86\D9\81\D8\B0_\D8\AC\D9\85\D8\B9"(i64 5, i64 3)
  %alloca1 = alloca i64, align 8
  store i64 %calltmp, ptr %alloca1, align 8
  %"%\D9\861.load" = load i64, ptr %alloca1, align 8
  %calltmp2 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A"(ptr @str.const.66, ptr @str.const.67, i64 %"%\D9\861.load", i64 8)
  %alloca3 = alloca i64, align 8
  store i64 %calltmp2, ptr %alloca3, align 8
  %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load" = load i64, ptr %alloca, align 8
  %"%\D8\B11.load" = load i64, ptr %alloca3, align 8
  %addtmp = add i64 %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load", %"%\D8\B11.load"
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp4 = call i64 @"\D9\86\D9\81\D8\B0_\D8\AC\D9\85\D8\B9"(i64 100, i64 200)
  %alloca5 = alloca i64, align 8
  store i64 %calltmp4, ptr %alloca5, align 8
  %"%\D9\862.load" = load i64, ptr %alloca5, align 8
  %calltmp6 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A"(ptr @str.const.68, ptr @str.const.69, i64 %"%\D9\862.load", i64 300)
  %alloca7 = alloca i64, align 8
  store i64 %calltmp6, ptr %alloca7, align 8
  %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load8" = load i64, ptr %alloca, align 8
  %"%\D8\B12.load" = load i64, ptr %alloca7, align 8
  %addtmp9 = add i64 %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load8", %"%\D8\B12.load"
  store i64 %addtmp9, ptr %alloca, align 8
  %calltmp10 = call i64 @"\D9\86\D9\81\D8\B0_\D8\AC\D9\85\D8\B9"(i64 0, i64 0)
  %alloca11 = alloca i64, align 8
  store i64 %calltmp10, ptr %alloca11, align 8
  %"%\D9\863.load" = load i64, ptr %alloca11, align 8
  %calltmp12 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A"(ptr @str.const.70, ptr @str.const.71, i64 %"%\D9\863.load", i64 0)
  %alloca13 = alloca i64, align 8
  store i64 %calltmp12, ptr %alloca13, align 8
  %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load14" = load i64, ptr %alloca, align 8
  %"%\D8\B13.load" = load i64, ptr %alloca13, align 8
  %addtmp15 = add i64 %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load14", %"%\D8\B13.load"
  store i64 %addtmp15, ptr %alloca, align 8
  %calltmp16 = call i64 @"\D9\86\D9\81\D8\B0_\D8\B7\D8\B1\D8\AD"(i64 10, i64 4)
  %alloca17 = alloca i64, align 8
  store i64 %calltmp16, ptr %alloca17, align 8
  %"%\D9\864.load" = load i64, ptr %alloca17, align 8
  %calltmp18 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A"(ptr @str.const.72, ptr @str.const.73, i64 %"%\D9\864.load", i64 6)
  %alloca19 = alloca i64, align 8
  store i64 %calltmp18, ptr %alloca19, align 8
  %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load20" = load i64, ptr %alloca, align 8
  %"%\D8\B14.load" = load i64, ptr %alloca19, align 8
  %addtmp21 = add i64 %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load20", %"%\D8\B14.load"
  store i64 %addtmp21, ptr %alloca, align 8
  %calltmp22 = call i64 @"\D9\86\D9\81\D8\B0_\D8\B7\D8\B1\D8\AD"(i64 50, i64 50)
  %alloca23 = alloca i64, align 8
  store i64 %calltmp22, ptr %alloca23, align 8
  %"%\D9\865.load" = load i64, ptr %alloca23, align 8
  %calltmp24 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A"(ptr @str.const.74, ptr @str.const.75, i64 %"%\D9\865.load", i64 0)
  %alloca25 = alloca i64, align 8
  store i64 %calltmp24, ptr %alloca25, align 8
  %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load26" = load i64, ptr %alloca, align 8
  %"%\D8\B15.load" = load i64, ptr %alloca25, align 8
  %addtmp27 = add i64 %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load26", %"%\D8\B15.load"
  store i64 %addtmp27, ptr %alloca, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.76)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca28 = alloca ptr, align 8
  %len1 = call i64 @strlen(ptr @str.const.80)
  %len2 = call i64 @strlen(ptr %alloca28)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %6 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.80, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %7 = call ptr @memcpy(ptr %dest2, ptr %alloca28, i64 %copylen2)
  %len129 = call i64 @strlen(ptr %concat.buf)
  %len230 = call i64 @strlen(ptr @str.const.81)
  %totallen31 = add i64 %len129, %len230
  %bufsize32 = add i64 %totallen31, 1
  %concat.buf33 = alloca i8, i64 %bufsize32, align 1
  %8 = call ptr @memcpy(ptr %concat.buf33, ptr %concat.buf, i64 %len129)
  %dest234 = getelementptr i8, ptr %concat.buf33, i64 %len129
  %copylen235 = add i64 %len230, 1
  %9 = call ptr @memcpy(ptr %dest234, ptr @str.const.81, i64 %copylen235)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf33)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load36" = load i64, ptr %alloca, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load36", 5
  br i1 %cmpeqtmp, label %then_13, label %else_15

then_13:                                          ; preds = %entry
  %calltmp37 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.85, i64 1)
  br label %merge_14

else_15:                                          ; preds = %entry
  %calltmp38 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.85, i64 0)
  br label %merge_14

merge_14:                                         ; preds = %else_15, %then_13
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.87)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\B6\D8\B1\D8\A8_\D9\88\D9\82\D8\B3\D9\85\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.91)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.95)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %calltmp = call i64 @"\D9\86\D9\81\D8\B0_\D8\B6\D8\B1\D8\A8"(i64 4, i64 7)
  %alloca1 = alloca i64, align 8
  store i64 %calltmp, ptr %alloca1, align 8
  %"%\D9\861.load" = load i64, ptr %alloca1, align 8
  %calltmp2 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A"(ptr @str.const.99, ptr @str.const.100, i64 %"%\D9\861.load", i64 28)
  %alloca3 = alloca i64, align 8
  store i64 %calltmp2, ptr %alloca3, align 8
  %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load" = load i64, ptr %alloca, align 8
  %"%\D8\B11.load" = load i64, ptr %alloca3, align 8
  %addtmp = add i64 %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load", %"%\D8\B11.load"
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp4 = call i64 @"\D9\86\D9\81\D8\B0_\D8\B6\D8\B1\D8\A8"(i64 0, i64 999)
  %alloca5 = alloca i64, align 8
  store i64 %calltmp4, ptr %alloca5, align 8
  %"%\D9\862.load" = load i64, ptr %alloca5, align 8
  %calltmp6 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A"(ptr @str.const.101, ptr @str.const.102, i64 %"%\D9\862.load", i64 0)
  %alloca7 = alloca i64, align 8
  store i64 %calltmp6, ptr %alloca7, align 8
  %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load8" = load i64, ptr %alloca, align 8
  %"%\D8\B12.load" = load i64, ptr %alloca7, align 8
  %addtmp9 = add i64 %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load8", %"%\D8\B12.load"
  store i64 %addtmp9, ptr %alloca, align 8
  %calltmp10 = call i64 @"\D9\86\D9\81\D8\B0_\D9\82\D8\B3\D9\85\D8\A9"(i64 20, i64 4)
  %alloca11 = alloca i64, align 8
  store i64 %calltmp10, ptr %alloca11, align 8
  %"%\D9\863.load" = load i64, ptr %alloca11, align 8
  %calltmp12 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A"(ptr @str.const.103, ptr @str.const.104, i64 %"%\D9\863.load", i64 5)
  %alloca13 = alloca i64, align 8
  store i64 %calltmp12, ptr %alloca13, align 8
  %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load14" = load i64, ptr %alloca, align 8
  %"%\D8\B13.load" = load i64, ptr %alloca13, align 8
  %addtmp15 = add i64 %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load14", %"%\D8\B13.load"
  store i64 %addtmp15, ptr %alloca, align 8
  %calltmp16 = call i64 @"\D9\86\D9\81\D8\B0_\D9\82\D8\B3\D9\85\D8\A9"(i64 100, i64 10)
  %alloca17 = alloca i64, align 8
  store i64 %calltmp16, ptr %alloca17, align 8
  %"%\D9\864.load" = load i64, ptr %alloca17, align 8
  %calltmp18 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A"(ptr @str.const.105, ptr @str.const.106, i64 %"%\D9\864.load", i64 10)
  %alloca19 = alloca i64, align 8
  store i64 %calltmp18, ptr %alloca19, align 8
  %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load20" = load i64, ptr %alloca, align 8
  %"%\D8\B14.load" = load i64, ptr %alloca19, align 8
  %addtmp21 = add i64 %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load20", %"%\D8\B14.load"
  store i64 %addtmp21, ptr %alloca, align 8
  %calltmp22 = call i64 @"\D9\86\D9\81\D8\B0_\D8\A8\D8\A7\D9\82\D9\8A"(i64 17, i64 5)
  %alloca23 = alloca i64, align 8
  store i64 %calltmp22, ptr %alloca23, align 8
  %"%\D9\865.load" = load i64, ptr %alloca23, align 8
  %calltmp24 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A"(ptr @str.const.107, ptr @str.const.108, i64 %"%\D9\865.load", i64 2)
  %alloca25 = alloca i64, align 8
  store i64 %calltmp24, ptr %alloca25, align 8
  %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load26" = load i64, ptr %alloca, align 8
  %"%\D8\B15.load" = load i64, ptr %alloca25, align 8
  %addtmp27 = add i64 %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load26", %"%\D8\B15.load"
  store i64 %addtmp27, ptr %alloca, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.109)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca28 = alloca ptr, align 8
  %len1 = call i64 @strlen(ptr @str.const.113)
  %len2 = call i64 @strlen(ptr %alloca28)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %6 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.113, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %7 = call ptr @memcpy(ptr %dest2, ptr %alloca28, i64 %copylen2)
  %len129 = call i64 @strlen(ptr %concat.buf)
  %len230 = call i64 @strlen(ptr @str.const.81)
  %totallen31 = add i64 %len129, %len230
  %bufsize32 = add i64 %totallen31, 1
  %concat.buf33 = alloca i8, i64 %bufsize32, align 1
  %8 = call ptr @memcpy(ptr %concat.buf33, ptr %concat.buf, i64 %len129)
  %dest234 = getelementptr i8, ptr %concat.buf33, i64 %len129
  %copylen235 = add i64 %len230, 1
  %9 = call ptr @memcpy(ptr %dest234, ptr @str.const.81, i64 %copylen235)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf33)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load36" = load i64, ptr %alloca, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load36", 5
  br i1 %cmpeqtmp, label %then_16, label %else_18

then_16:                                          ; preds = %entry
  %calltmp37 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.118, i64 1)
  br label %merge_17

else_18:                                          ; preds = %entry
  %calltmp38 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.118, i64 0)
  br label %merge_17

merge_17:                                         ; preds = %else_18, %then_16
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.120)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\86\D8\B5\D9\88\D8\B5"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.124)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.128)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %calltmp = call i64 @"\D9\86\D9\81\D8\B0_\D8\B7\D9\88\D9\84"(ptr @str.const.132)
  %alloca1 = alloca i64, align 8
  store i64 %calltmp, ptr %alloca1, align 8
  %"%\D8\B71.load" = load i64, ptr %alloca1, align 8
  %calltmp2 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A"(ptr @str.const.133, ptr @str.const.134, i64 %"%\D8\B71.load", i64 5)
  %alloca3 = alloca i64, align 8
  store i64 %calltmp2, ptr %alloca3, align 8
  %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load" = load i64, ptr %alloca, align 8
  %"%\D8\B11.load" = load i64, ptr %alloca3, align 8
  %addtmp = add i64 %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load", %"%\D8\B11.load"
  store i64 %addtmp, ptr %alloca, align 8
  %calltmp4 = call i64 @"\D9\86\D9\81\D8\B0_\D8\B7\D9\88\D9\84"(ptr @str.const.135)
  %alloca5 = alloca i64, align 8
  store i64 %calltmp4, ptr %alloca5, align 8
  %"%\D8\B72.load" = load i64, ptr %alloca5, align 8
  %calltmp6 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A"(ptr @str.const.136, ptr @str.const.137, i64 %"%\D8\B72.load", i64 0)
  %alloca7 = alloca i64, align 8
  store i64 %calltmp6, ptr %alloca7, align 8
  %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load8" = load i64, ptr %alloca, align 8
  %"%\D8\B12.load" = load i64, ptr %alloca7, align 8
  %addtmp9 = add i64 %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load8", %"%\D8\B12.load"
  store i64 %addtmp9, ptr %alloca, align 8
  %calltmp10 = call i64 @"\D9\86\D9\81\D8\B0_\D8\B7\D9\88\D9\84"(ptr @str.const.138)
  %alloca11 = alloca i64, align 8
  store i64 %calltmp10, ptr %alloca11, align 8
  %"%\D8\B73.load" = load i64, ptr %alloca11, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B73.load", 10
  br i1 %cmpgttmp, label %then_19, label %else_21

then_19:                                          ; preds = %entry
  %calltmp12 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.139, i64 1)
  %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load13" = load i64, ptr %alloca, align 8
  %addtmp14 = add i64 %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load13", 1
  store i64 %addtmp14, ptr %alloca, align 8
  br label %merge_20

else_21:                                          ; preds = %entry
  %calltmp15 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.139, i64 0)
  br label %merge_20

merge_20:                                         ; preds = %else_21, %then_19
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.141)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca16 = alloca ptr, align 8
  %len1 = call i64 @strlen(ptr @str.const.145)
  %len2 = call i64 @strlen(ptr %alloca16)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %6 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.145, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %7 = call ptr @memcpy(ptr %dest2, ptr %alloca16, i64 %copylen2)
  %len117 = call i64 @strlen(ptr %concat.buf)
  %len218 = call i64 @strlen(ptr @str.const.146)
  %totallen19 = add i64 %len117, %len218
  %bufsize20 = add i64 %totallen19, 1
  %concat.buf21 = alloca i8, i64 %bufsize20, align 1
  %8 = call ptr @memcpy(ptr %concat.buf21, ptr %concat.buf, i64 %len117)
  %dest222 = getelementptr i8, ptr %concat.buf21, i64 %len117
  %copylen223 = add i64 %len218, 1
  %9 = call ptr @memcpy(ptr %dest222, ptr @str.const.146, i64 %copylen223)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf21)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load24" = load i64, ptr %alloca, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load24", 3
  br i1 %cmpeqtmp, label %then_22, label %else_24

then_22:                                          ; preds = %merge_20
  %calltmp25 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.150, i64 1)
  br label %merge_23

else_24:                                          ; preds = %merge_20
  %calltmp26 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.150, i64 0)
  br label %merge_23

merge_23:                                         ; preds = %else_24, %then_22
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.152)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AA\D9\88\D9\84\D9\8A\D8\AF_html"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.156)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.160)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.164)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.168)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.172)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call ptr @"\D9\88\D8\B3\D9\85"(ptr @str.const.176, ptr @str.const.177)
  %alloca = alloca ptr, align 8
  store ptr %calltmp, ptr %alloca, align 8
  %len1 = call i64 @strlen(ptr @str.const.178)
  %len2 = call i64 @strlen(ptr %alloca)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %10 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.178, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %11 = call ptr @memcpy(ptr %dest2, ptr %alloca, i64 %copylen2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.182)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.186)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp1 = call ptr @"\D9\88\D8\B3\D9\85"(ptr @str.const.190, ptr @str.const.191)
  %alloca2 = alloca ptr, align 8
  store ptr %calltmp1, ptr %alloca2, align 8
  %len13 = call i64 @strlen(ptr @str.const.178)
  %len24 = call i64 @strlen(ptr %alloca2)
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %18 = call ptr @memcpy(ptr %concat.buf7, ptr @str.const.178, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %19 = call ptr @memcpy(ptr %dest28, ptr %alloca2, i64 %copylen29)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf7)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp10 = call ptr @"\D9\88\D8\B3\D9\85"(ptr @str.const.196, ptr @str.const.197)
  %alloca11 = alloca ptr, align 8
  store ptr %calltmp10, ptr %alloca11, align 8
  %len112 = call i64 @strlen(ptr @str.const.178)
  %len213 = call i64 @strlen(ptr %alloca11)
  %totallen14 = add i64 %len112, %len213
  %bufsize15 = add i64 %totallen14, 1
  %concat.buf16 = alloca i8, i64 %bufsize15, align 1
  %22 = call ptr @memcpy(ptr %concat.buf16, ptr @str.const.178, i64 %len112)
  %dest217 = getelementptr i8, ptr %concat.buf16, i64 %len112
  %copylen218 = add i64 %len213, 1
  %23 = call ptr @memcpy(ptr %dest217, ptr %alloca11, i64 %copylen218)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf16)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.202)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp19 = call ptr @"\D9\88\D8\B3\D9\85"(ptr @str.const.206, ptr @str.const.207)
  %alloca20 = alloca ptr, align 8
  store ptr %calltmp19, ptr %alloca20, align 8
  %calltmp21 = call ptr @"\D9\88\D8\B3\D9\85"(ptr @str.const.206, ptr @str.const.209)
  %alloca22 = alloca ptr, align 8
  store ptr %calltmp21, ptr %alloca22, align 8
  %calltmp23 = call ptr @"\D9\88\D8\B3\D9\85"(ptr @str.const.206, ptr @str.const.211)
  %alloca24 = alloca ptr, align 8
  store ptr %calltmp23, ptr %alloca24, align 8
  %len125 = call i64 @strlen(ptr @str.const.212)
  %len226 = call i64 @strlen(ptr %alloca20)
  %totallen27 = add i64 %len125, %len226
  %bufsize28 = add i64 %totallen27, 1
  %concat.buf29 = alloca i8, i64 %bufsize28, align 1
  %28 = call ptr @memcpy(ptr %concat.buf29, ptr @str.const.212, i64 %len125)
  %dest230 = getelementptr i8, ptr %concat.buf29, i64 %len125
  %copylen231 = add i64 %len226, 1
  %29 = call ptr @memcpy(ptr %dest230, ptr %alloca20, i64 %copylen231)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf29)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len132 = call i64 @strlen(ptr @str.const.212)
  %len233 = call i64 @strlen(ptr %alloca22)
  %totallen34 = add i64 %len132, %len233
  %bufsize35 = add i64 %totallen34, 1
  %concat.buf36 = alloca i8, i64 %bufsize35, align 1
  %32 = call ptr @memcpy(ptr %concat.buf36, ptr @str.const.212, i64 %len132)
  %dest237 = getelementptr i8, ptr %concat.buf36, i64 %len132
  %copylen238 = add i64 %len233, 1
  %33 = call ptr @memcpy(ptr %dest237, ptr %alloca22, i64 %copylen238)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf36)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len139 = call i64 @strlen(ptr @str.const.212)
  %len240 = call i64 @strlen(ptr %alloca24)
  %totallen41 = add i64 %len139, %len240
  %bufsize42 = add i64 %totallen41, 1
  %concat.buf43 = alloca i8, i64 %bufsize42, align 1
  %36 = call ptr @memcpy(ptr %concat.buf43, ptr @str.const.212, i64 %len139)
  %dest244 = getelementptr i8, ptr %concat.buf43, i64 %len139
  %copylen245 = add i64 %len240, 1
  %37 = call ptr @memcpy(ptr %dest244, ptr %alloca24, i64 %copylen245)
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf43)
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %40 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.224)
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp46 = call ptr @"\D9\88\D8\B3\D9\85"(ptr @str.const.196, ptr @str.const.229)
  %alloca47 = alloca ptr, align 8
  store ptr %calltmp46, ptr %alloca47, align 8
  %len148 = call i64 @strlen(ptr @str.const.178)
  %len249 = call i64 @strlen(ptr %alloca47)
  %totallen50 = add i64 %len148, %len249
  %bufsize51 = add i64 %totallen50, 1
  %concat.buf52 = alloca i8, i64 %bufsize51, align 1
  %42 = call ptr @memcpy(ptr %concat.buf52, ptr @str.const.178, i64 %len148)
  %dest253 = getelementptr i8, ptr %concat.buf52, i64 %len148
  %copylen254 = add i64 %len249, 1
  %43 = call ptr @memcpy(ptr %dest253, ptr %alloca47, i64 %copylen254)
  %44 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf52)
  %45 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %46 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.234)
  %47 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp55 = call ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\85\D8\AB\D8\A7\D9\84_html"(ptr @str.const.238, ptr @str.const.67, ptr @str.const.207, i64 1)
  %calltmp56 = call ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\85\D8\AB\D8\A7\D9\84_html"(ptr @str.const.241, ptr @str.const.100, ptr @str.const.207, i64 1)
  %calltmp57 = call ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\85\D8\AB\D8\A7\D9\84_html"(ptr @str.const.244, ptr @str.const.104, ptr @str.const.207, i64 1)
  %calltmp58 = call ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\85\D8\AB\D8\A7\D9\84_html"(ptr @str.const.247, ptr @str.const.108, ptr @str.const.207, i64 1)
  %48 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.250)
  %49 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp59 = call ptr @"\D9\88\D8\B3\D9\85"(ptr @str.const.196, ptr @str.const.255)
  %alloca60 = alloca ptr, align 8
  store ptr %calltmp59, ptr %alloca60, align 8
  %len161 = call i64 @strlen(ptr @str.const.178)
  %len262 = call i64 @strlen(ptr %alloca60)
  %totallen63 = add i64 %len161, %len262
  %bufsize64 = add i64 %totallen63, 1
  %concat.buf65 = alloca i8, i64 %bufsize64, align 1
  %50 = call ptr @memcpy(ptr %concat.buf65, ptr @str.const.178, i64 %len161)
  %dest266 = getelementptr i8, ptr %concat.buf65, i64 %len161
  %copylen267 = add i64 %len262, 1
  %51 = call ptr @memcpy(ptr %dest266, ptr %alloca60, i64 %copylen267)
  %52 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf65)
  %53 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %54 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.234)
  %55 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp68 = call ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\85\D8\AB\D8\A7\D9\84_html"(ptr @str.const.264, ptr @str.const.134, ptr @str.const.209, i64 1)
  %calltmp69 = call ptr @"\D8\B7\D8\A8\D8\A7\D8\B9\D8\A9_\D9\85\D8\AB\D8\A7\D9\84_html"(ptr @str.const.267, ptr @str.const.137, ptr @str.const.209, i64 1)
  %56 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.250)
  %57 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %58 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.274)
  %59 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %60 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.278)
  %61 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %62 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.282)
  %63 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp70 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.286, i64 1)
  %64 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.287)
  %65 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\85\D9\84\D8\AE\D8\B5"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.291)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.295)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 10, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 3, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 0, ptr %alloca2, align 8
  %"%\D8\A3\D9\85\D8\AB\D9\84\D8\A9_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %"%\D8\A3\D9\85\D8\AB\D9\84\D8\A9_\D9\86\D8\B5\D9\88\D8\B5.load" = load i64, ptr %alloca1, align 8
  %addtmp = add i64 %"%\D8\A3\D9\85\D8\AB\D9\84\D8\A9_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load", %"%\D8\A3\D9\85\D8\AB\D9\84\D8\A9_\D9\86\D8\B5\D9\88\D8\B5.load"
  %"%\D8\A3\D9\85\D8\AB\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82.load" = load i64, ptr %alloca2, align 8
  %addtmp3 = add i64 %addtmp, %"%\D8\A3\D9\85\D8\AB\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82.load"
  %alloca4 = alloca i64, align 8
  store i64 %addtmp3, ptr %alloca4, align 8
  %alloca5 = alloca i64, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load" = load i64, ptr %alloca4, align 8
  store i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load", ptr %alloca5, align 8
  %alloca6 = alloca i64, align 8
  store i64 0, ptr %alloca6, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca ptr, align 8
  %alloca9 = alloca ptr, align 8
  %alloca10 = alloca ptr, align 8
  %alloca11 = alloca ptr, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.299)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.303)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.307)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len1 = call i64 @strlen(ptr @str.const.311)
  %len2 = call i64 @strlen(ptr %alloca7)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %10 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.311, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %11 = call ptr @memcpy(ptr %dest2, ptr %alloca7, i64 %copylen2)
  %len112 = call i64 @strlen(ptr %concat.buf)
  %len213 = call i64 @strlen(ptr @str.const.312)
  %totallen14 = add i64 %len112, %len213
  %bufsize15 = add i64 %totallen14, 1
  %concat.buf16 = alloca i8, i64 %bufsize15, align 1
  %12 = call ptr @memcpy(ptr %concat.buf16, ptr %concat.buf, i64 %len112)
  %dest217 = getelementptr i8, ptr %concat.buf16, i64 %len112
  %copylen218 = add i64 %len213, 1
  %13 = call ptr @memcpy(ptr %dest217, ptr @str.const.312, i64 %copylen218)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf16)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len119 = call i64 @strlen(ptr @str.const.316)
  %len220 = call i64 @strlen(ptr %alloca8)
  %totallen21 = add i64 %len119, %len220
  %bufsize22 = add i64 %totallen21, 1
  %concat.buf23 = alloca i8, i64 %bufsize22, align 1
  %16 = call ptr @memcpy(ptr %concat.buf23, ptr @str.const.316, i64 %len119)
  %dest224 = getelementptr i8, ptr %concat.buf23, i64 %len119
  %copylen225 = add i64 %len220, 1
  %17 = call ptr @memcpy(ptr %dest224, ptr %alloca8, i64 %copylen225)
  %len126 = call i64 @strlen(ptr %concat.buf23)
  %len227 = call i64 @strlen(ptr @str.const.317)
  %totallen28 = add i64 %len126, %len227
  %bufsize29 = add i64 %totallen28, 1
  %concat.buf30 = alloca i8, i64 %bufsize29, align 1
  %18 = call ptr @memcpy(ptr %concat.buf30, ptr %concat.buf23, i64 %len126)
  %dest231 = getelementptr i8, ptr %concat.buf30, i64 %len126
  %copylen232 = add i64 %len227, 1
  %19 = call ptr @memcpy(ptr %dest231, ptr @str.const.317, i64 %copylen232)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf30)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len133 = call i64 @strlen(ptr @str.const.321)
  %len234 = call i64 @strlen(ptr %alloca9)
  %totallen35 = add i64 %len133, %len234
  %bufsize36 = add i64 %totallen35, 1
  %concat.buf37 = alloca i8, i64 %bufsize36, align 1
  %22 = call ptr @memcpy(ptr %concat.buf37, ptr @str.const.321, i64 %len133)
  %dest238 = getelementptr i8, ptr %concat.buf37, i64 %len133
  %copylen239 = add i64 %len234, 1
  %23 = call ptr @memcpy(ptr %dest238, ptr %alloca9, i64 %copylen239)
  %len140 = call i64 @strlen(ptr %concat.buf37)
  %len241 = call i64 @strlen(ptr @str.const.322)
  %totallen42 = add i64 %len140, %len241
  %bufsize43 = add i64 %totallen42, 1
  %concat.buf44 = alloca i8, i64 %bufsize43, align 1
  %24 = call ptr @memcpy(ptr %concat.buf44, ptr %concat.buf37, i64 %len140)
  %dest245 = getelementptr i8, ptr %concat.buf44, i64 %len140
  %copylen246 = add i64 %len241, 1
  %25 = call ptr @memcpy(ptr %dest245, ptr @str.const.322, i64 %copylen246)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf44)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load47" = load i64, ptr %alloca4, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load47", 0
  br i1 %cmpgttmp, label %then_25, label %merge_26

then_25:                                          ; preds = %entry
  %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load" = load i64, ptr %alloca5, align 8
  %multmp = mul i64 %"%\D9\86\D8\A7\D8\AC\D8\AD\D8\A9.load", 100
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load48" = load i64, ptr %alloca4, align 8
  %divtmp = sdiv i64 %multmp, %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load48"
  %alloca49 = alloca i64, align 8
  store i64 %divtmp, ptr %alloca49, align 8
  %alloca50 = alloca ptr, align 8
  %len151 = call i64 @strlen(ptr @str.const.326)
  %len252 = call i64 @strlen(ptr %alloca50)
  %totallen53 = add i64 %len151, %len252
  %bufsize54 = add i64 %totallen53, 1
  %concat.buf55 = alloca i8, i64 %bufsize54, align 1
  %28 = call ptr @memcpy(ptr %concat.buf55, ptr @str.const.326, i64 %len151)
  %dest256 = getelementptr i8, ptr %concat.buf55, i64 %len151
  %copylen257 = add i64 %len252, 1
  %29 = call ptr @memcpy(ptr %dest256, ptr %alloca50, i64 %copylen257)
  %len158 = call i64 @strlen(ptr %concat.buf55)
  %len259 = call i64 @strlen(ptr @str.const.327)
  %totallen60 = add i64 %len158, %len259
  %bufsize61 = add i64 %totallen60, 1
  %concat.buf62 = alloca i8, i64 %bufsize61, align 1
  %30 = call ptr @memcpy(ptr %concat.buf62, ptr %concat.buf55, i64 %len158)
  %dest263 = getelementptr i8, ptr %concat.buf62, i64 %len158
  %copylen264 = add i64 %len259, 1
  %31 = call ptr @memcpy(ptr %dest263, ptr @str.const.327, i64 %copylen264)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf62)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_26

merge_26:                                         ; preds = %then_25, %entry
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.307)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len165 = call i64 @strlen(ptr @str.const.335)
  %len266 = call i64 @strlen(ptr %alloca10)
  %totallen67 = add i64 %len165, %len266
  %bufsize68 = add i64 %totallen67, 1
  %concat.buf69 = alloca i8, i64 %bufsize68, align 1
  %36 = call ptr @memcpy(ptr %concat.buf69, ptr @str.const.335, i64 %len165)
  %dest270 = getelementptr i8, ptr %concat.buf69, i64 %len165
  %copylen271 = add i64 %len266, 1
  %37 = call ptr @memcpy(ptr %dest270, ptr %alloca10, i64 %copylen271)
  %len172 = call i64 @strlen(ptr %concat.buf69)
  %len273 = call i64 @strlen(ptr @str.const.336)
  %totallen74 = add i64 %len172, %len273
  %bufsize75 = add i64 %totallen74, 1
  %concat.buf76 = alloca i8, i64 %bufsize75, align 1
  %38 = call ptr @memcpy(ptr %concat.buf76, ptr %concat.buf69, i64 %len172)
  %dest277 = getelementptr i8, ptr %concat.buf76, i64 %len172
  %copylen278 = add i64 %len273, 1
  %39 = call ptr @memcpy(ptr %dest277, ptr @str.const.336, i64 %copylen278)
  %40 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf76)
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len179 = call i64 @strlen(ptr @str.const.340)
  %len280 = call i64 @strlen(ptr %alloca11)
  %totallen81 = add i64 %len179, %len280
  %bufsize82 = add i64 %totallen81, 1
  %concat.buf83 = alloca i8, i64 %bufsize82, align 1
  %42 = call ptr @memcpy(ptr %concat.buf83, ptr @str.const.340, i64 %len179)
  %dest284 = getelementptr i8, ptr %concat.buf83, i64 %len179
  %copylen285 = add i64 %len280, 1
  %43 = call ptr @memcpy(ptr %dest284, ptr %alloca11, i64 %copylen285)
  %len186 = call i64 @strlen(ptr %concat.buf83)
  %len287 = call i64 @strlen(ptr @str.const.341)
  %totallen88 = add i64 %len186, %len287
  %bufsize89 = add i64 %totallen88, 1
  %concat.buf90 = alloca i8, i64 %bufsize89, align 1
  %44 = call ptr @memcpy(ptr %concat.buf90, ptr %concat.buf83, i64 %len186)
  %dest291 = getelementptr i8, ptr %concat.buf90, i64 %len186
  %copylen292 = add i64 %len287, 1
  %45 = call ptr @memcpy(ptr %dest291, ptr @str.const.341, i64 %copylen292)
  %46 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf90)
  %47 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %48 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.345)
  %49 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %50 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.349)
  %51 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %52 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.353)
  %53 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\81\D8\A7\D8\B4\D9\84\D8\A9.load" = load i64, ptr %alloca6, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\81\D8\A7\D8\B4\D9\84\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_27, label %else_29

then_27:                                          ; preds = %merge_26
  %calltmp = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.357, i64 1)
  br label %merge_28

else_29:                                          ; preds = %merge_26
  %calltmp93 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9"(ptr @str.const.357, i64 0)
  br label %merge_28

merge_28:                                         ; preds = %else_29, %then_27
  %54 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.359)
  %55 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.363)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.367)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.371)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.375)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AC\D9\85\D8\B9_\D9\88\D8\B7\D8\B1\D8\AD"()
  %calltmp1 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\B6\D8\B1\D8\A8_\D9\88\D9\82\D8\B3\D9\85\D8\A9"()
  %calltmp2 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\86\D8\B5\D9\88\D8\B5"()
  %calltmp3 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D8\AA\D9\88\D9\84\D9\8A\D8\AF_html"()
  %calltmp4 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1_\D9\85\D9\84\D8\AE\D8\B5"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.379)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
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
