; ModuleID = 'test_gen_sadc'
source_filename = "test_gen_sadc"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%"\D8\A8\D8\B3\D9\8A\D8\B7.Frame" = type { ptr, ptr, i64, i2 }
%"\D8\B6\D8\B9\D9\81.Frame" = type { ptr, ptr, i64, i64, i2 }
%"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame" = type { ptr, ptr, i64, i3 }

@fmt.d = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@"\D8\A8\D8\B3\D9\8A\D8\B7.resumers" = private constant [3 x ptr] [ptr @"\D8\A8\D8\B3\D9\8A\D8\B7.resume", ptr @"\D8\A8\D8\B3\D9\8A\D8\B7.destroy", ptr @"\D8\A8\D8\B3\D9\8A\D8\B7.cleanup"]
@"\D8\B6\D8\B9\D9\81.resumers" = private constant [3 x ptr] [ptr @"\D8\B6\D8\B9\D9\81.resume", ptr @"\D8\B6\D8\B9\D9\81.destroy", ptr @"\D8\B6\D8\B9\D9\81.cleanup"]
@"\D9\85\D8\AC\D9\85\D9\88\D8\B9.resumers" = private constant [3 x ptr] [ptr @"\D9\85\D8\AC\D9\85\D9\88\D8\B9.resume", ptr @"\D9\85\D8\AC\D9\85\D9\88\D8\B9.destroy", ptr @"\D9\85\D8\AC\D9\85\D9\88\D8\B9.cleanup"]

define ptr @"\D8\A8\D8\B3\D9\8A\D8\B7"() {
entry:
  %coro.promise = alloca i64, align 8
  store i64 0, ptr %coro.promise, align 8
  %coro.alloc = call ptr @malloc(i64 32)
  %resume.addr = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.alloc, i32 0, i32 0
  store ptr @"\D8\A8\D8\B3\D9\8A\D8\B7.resume", ptr %resume.addr, align 8
  %destroy.addr = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.alloc, i32 0, i32 1
  store ptr @"\D8\A8\D8\B3\D9\8A\D8\B7.destroy", ptr %destroy.addr, align 8
  %0 = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.alloc, i32 0, i32 2
  %1 = load i64, ptr %coro.promise, align 8
  store i64 %1, ptr %0, align 8
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.alloc, i32 0, i32 2
  %index.addr7 = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.alloc, i32 0, i32 3
  store i2 0, ptr %index.addr7, align 1
  ret ptr %coro.alloc
}

define ptr @"\D8\B6\D8\B9\D9\81"(i64 %"\D9\86") {
entry:
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  %coro.promise = alloca i64, align 8
  store i64 0, ptr %coro.promise, align 8
  %coro.alloc = call ptr @malloc(i64 40)
  %resume.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.alloc, i32 0, i32 0
  store ptr @"\D8\B6\D8\B9\D9\81.resume", ptr %resume.addr, align 8
  %destroy.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.alloc, i32 0, i32 1
  store ptr @"\D8\B6\D8\B9\D9\81.destroy", ptr %destroy.addr, align 8
  %0 = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.alloc, i32 0, i32 2
  %1 = load i64, ptr %coro.promise, align 8
  store i64 %1, ptr %0, align 8
  %2 = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.alloc, i32 0, i32 3
  %3 = load i64, ptr %"\D9\861", align 8
  store i64 %3, ptr %2, align 8
  %"\D9\861.reload.addr" = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.alloc, i32 0, i32 3
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.alloc, i32 0, i32 2
  %index.addr8 = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.alloc, i32 0, i32 4
  store i2 0, ptr %index.addr8, align 1
  ret ptr %coro.alloc
}

define ptr @"\D9\85\D8\AC\D9\85\D9\88\D8\B9"() {
entry:
  %coro.promise = alloca i64, align 8
  store i64 0, ptr %coro.promise, align 8
  %coro.alloc = call ptr @malloc(i64 32)
  %resume.addr = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.alloc, i32 0, i32 0
  store ptr @"\D9\85\D8\AC\D9\85\D9\88\D8\B9.resume", ptr %resume.addr, align 8
  %destroy.addr = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.alloc, i32 0, i32 1
  store ptr @"\D9\85\D8\AC\D9\85\D9\88\D8\B9.destroy", ptr %destroy.addr, align 8
  %0 = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.alloc, i32 0, i32 2
  %1 = load i64, ptr %coro.promise, align 8
  store i64 %1, ptr %0, align 8
  %coro.promise.reload.addr = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.alloc, i32 0, i32 2
  %index.addr13 = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.alloc, i32 0, i32 3
  store i3 0, ptr %index.addr13, align 1
  ret ptr %coro.alloc
}

define void @__sad_main() {
entry:
  %calltmp = call ptr @"\D8\A8\D8\B3\D9\8A\D8\B7"()
  %gen.sum = alloca i64, align 8
  store i64 0, ptr %gen.sum, align 8
  br label %gen.loop

gen.loop:                                         ; preds = %gen.yield, %entry
  %0 = getelementptr inbounds { ptr, ptr }, ptr %calltmp, i32 0, i32 0
  %1 = load ptr, ptr %0, align 8
  call fastcc void %1(ptr %calltmp)
  %2 = load ptr, ptr %calltmp, align 8
  %3 = icmp eq ptr %2, null
  br i1 %3, label %gen.end, label %gen.yield

gen.yield:                                        ; preds = %gen.loop
  %4 = getelementptr inbounds i8, ptr %calltmp, i32 16
  %gen.val = load i64, ptr %4, align 8
  %gen.cursum = load i64, ptr %gen.sum, align 8
  %gen.newsum = add i64 %gen.cursum, %gen.val
  store i64 %gen.newsum, ptr %gen.sum, align 8
  br label %gen.loop

gen.end:                                          ; preds = %gen.loop
  %5 = getelementptr inbounds { ptr, ptr }, ptr %calltmp, i32 0, i32 1
  %6 = load ptr, ptr %5, align 8
  call fastcc void %6(ptr %calltmp)
  %gen.result = load i64, ptr %gen.sum, align 8
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %gen.result)
  %calltmp1 = call ptr @"\D8\B6\D8\B9\D9\81"(i64 21)
  %gen.sum2 = alloca i64, align 8
  store i64 0, ptr %gen.sum2, align 8
  br label %gen.loop3

gen.loop3:                                        ; preds = %gen.yield4, %gen.end
  %8 = getelementptr inbounds { ptr, ptr }, ptr %calltmp1, i32 0, i32 0
  %9 = load ptr, ptr %8, align 8
  call fastcc void %9(ptr %calltmp1)
  %10 = load ptr, ptr %calltmp1, align 8
  %11 = icmp eq ptr %10, null
  br i1 %11, label %gen.end5, label %gen.yield4

gen.yield4:                                       ; preds = %gen.loop3
  %12 = getelementptr inbounds i8, ptr %calltmp1, i32 16
  %gen.val8 = load i64, ptr %12, align 8
  %gen.cursum9 = load i64, ptr %gen.sum2, align 8
  %gen.newsum10 = add i64 %gen.cursum9, %gen.val8
  store i64 %gen.newsum10, ptr %gen.sum2, align 8
  br label %gen.loop3

gen.end5:                                         ; preds = %gen.loop3
  %13 = getelementptr inbounds { ptr, ptr }, ptr %calltmp1, i32 0, i32 1
  %14 = load ptr, ptr %13, align 8
  call fastcc void %14(ptr %calltmp1)
  %gen.result11 = load i64, ptr %gen.sum2, align 8
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %gen.result11)
  %calltmp12 = call ptr @"\D9\85\D8\AC\D9\85\D9\88\D8\B9"()
  %gen.sum13 = alloca i64, align 8
  store i64 0, ptr %gen.sum13, align 8
  br label %gen.loop14

gen.loop14:                                       ; preds = %gen.yield15, %gen.end5
  %16 = getelementptr inbounds { ptr, ptr }, ptr %calltmp12, i32 0, i32 0
  %17 = load ptr, ptr %16, align 8
  call fastcc void %17(ptr %calltmp12)
  %18 = load ptr, ptr %calltmp12, align 8
  %19 = icmp eq ptr %18, null
  br i1 %19, label %gen.end16, label %gen.yield15

gen.yield15:                                      ; preds = %gen.loop14
  %20 = getelementptr inbounds i8, ptr %calltmp12, i32 16
  %gen.val19 = load i64, ptr %20, align 8
  %gen.cursum20 = load i64, ptr %gen.sum13, align 8
  %gen.newsum21 = add i64 %gen.cursum20, %gen.val19
  store i64 %gen.newsum21, ptr %gen.sum13, align 8
  br label %gen.loop14

gen.end16:                                        ; preds = %gen.loop14
  %21 = getelementptr inbounds { ptr, ptr }, ptr %calltmp12, i32 0, i32 1
  %22 = load ptr, ptr %21, align 8
  call fastcc void %22(ptr %calltmp12)
  %gen.result22 = load i64, ptr %gen.sum13, align 8
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %gen.result22)
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare token @llvm.coro.id(i32, ptr readnone, ptr nocapture readonly, ptr) #0

; Function Attrs: nounwind memory(none)
declare i64 @llvm.coro.size.i64() #1

declare ptr @malloc(i64)

; Function Attrs: nounwind
declare ptr @llvm.coro.begin(token, ptr writeonly) #2

; Function Attrs: nomerge nounwind
declare token @llvm.coro.save(ptr) #3

; Function Attrs: nounwind
declare i8 @llvm.coro.suspend(token, i1) #2

; Function Attrs: nounwind memory(argmem: read)
declare ptr @llvm.coro.free(token, ptr nocapture readonly) #4

declare void @free(ptr)

; Function Attrs: nounwind
declare i1 @llvm.coro.end(ptr, i1, token) #2

declare void @llvm.coro.resume(ptr)

; Function Attrs: nounwind memory(argmem: readwrite)
declare i1 @llvm.coro.done(ptr nocapture readonly) #5

; Function Attrs: nounwind memory(none)
declare ptr @llvm.coro.promise(ptr nocapture, i32, i1) #1

declare void @llvm.coro.destroy(ptr)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: read)
declare ptr @llvm.coro.subfn.addr(ptr nocapture readonly, i8) #0

define internal fastcc void @"\D8\A8\D8\B3\D9\8A\D8\B7.resume"(ptr noundef nonnull align 8 dereferenceable(32) %coro.hdl) {
entry.resume:
  %coro.promise = alloca i64, align 8
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %resume.entry
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 0, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave4:                                        ; preds = %gen.yield.0.resume
  %ResumeFn.addr = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend5

CoroSuspend5:                                     ; preds = %CoroSave4
  br label %resume.2.landing

resume.2.landing:                                 ; preds = %CoroSuspend5
  br label %AfterCoroSuspend6

AfterCoroSuspend6:                                ; preds = %resume.2.landing
  %cond = icmp eq i8 -1, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend3, %AfterCoroSuspend6, %AfterCoroSuspend
  call void @free(ptr %coro.hdl)
  br label %coro.suspend

coro.suspend:                                     ; preds = %AfterCoroSuspend3, %coro.cleanup, %AfterCoroSuspend6, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  store i64 42, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave1

CoroSave1:                                        ; preds = %coro.init.resume
  %index.addr8 = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 3
  store i2 1, ptr %index.addr8, align 1
  br label %CoroSuspend2

CoroSuspend2:                                     ; preds = %CoroSave1
  br label %resume.1.landing

resume.1:                                         ; preds = %resume.entry
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %resume.1, %CoroSuspend2
  %0 = phi i8 [ -1, %CoroSuspend2 ], [ 0, %resume.1 ]
  br label %AfterCoroSuspend3

AfterCoroSuspend3:                                ; preds = %resume.1.landing
  switch i8 %0, label %coro.suspend [
    i8 0, label %gen.yield.0.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.0.resume:                               ; preds = %AfterCoroSuspend3
  br label %CoroSave4

resume.entry:                                     ; preds = %entry.resume
  %index.addr = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 3
  %index = load i2, ptr %index.addr, align 1
  switch i2 %index, label %unreachable [
    i2 0, label %resume.0
    i2 1, label %resume.1
  ]

unreachable:                                      ; preds = %resume.entry
  unreachable
}

define internal fastcc void @"\D8\A8\D8\B3\D9\8A\D8\B7.destroy"(ptr noundef nonnull align 8 dereferenceable(32) %coro.hdl) {
entry.destroy:
  %coro.promise = alloca i64, align 8
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %Switch
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 1, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave4:                                        ; preds = %gen.yield.0.resume
  %ResumeFn.addr = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend5

CoroSuspend5:                                     ; preds = %CoroSave4
  br label %resume.2.landing

resume.2:                                         ; preds = %resume.entry
  br label %resume.2.landing

resume.2.landing:                                 ; preds = %resume.2, %CoroSuspend5
  %0 = phi i8 [ -1, %CoroSuspend5 ], [ 1, %resume.2 ]
  br label %AfterCoroSuspend6

AfterCoroSuspend6:                                ; preds = %resume.2.landing
  %cond = icmp eq i8 %0, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend3, %AfterCoroSuspend6, %AfterCoroSuspend
  call void @free(ptr %coro.hdl)
  br label %coro.suspend

coro.suspend:                                     ; preds = %AfterCoroSuspend3, %coro.cleanup, %AfterCoroSuspend6, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  store i64 42, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave1

CoroSave1:                                        ; preds = %coro.init.resume
  %index.addr8 = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 3
  store i2 1, ptr %index.addr8, align 1
  br label %CoroSuspend2

CoroSuspend2:                                     ; preds = %CoroSave1
  br label %resume.1.landing

resume.1:                                         ; preds = %Switch
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %resume.1, %CoroSuspend2
  %1 = phi i8 [ -1, %CoroSuspend2 ], [ 1, %resume.1 ]
  br label %AfterCoroSuspend3

AfterCoroSuspend3:                                ; preds = %resume.1.landing
  switch i8 %1, label %coro.suspend [
    i8 0, label %gen.yield.0.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.0.resume:                               ; preds = %AfterCoroSuspend3
  br label %CoroSave4

resume.entry:                                     ; preds = %entry.destroy
  %index.addr = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 3
  %index = load i2, ptr %index.addr, align 1
  %ResumeFn.addr1 = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 0
  %2 = load ptr, ptr %ResumeFn.addr1, align 8
  %3 = icmp eq ptr %2, null
  br i1 %3, label %resume.2, label %Switch

Switch:                                           ; preds = %resume.entry
  switch i2 %index, label %unreachable [
    i2 0, label %resume.0
    i2 1, label %resume.1
  ]

unreachable:                                      ; preds = %Switch
  unreachable
}

define internal fastcc void @"\D8\A8\D8\B3\D9\8A\D8\B7.cleanup"(ptr noundef nonnull align 8 dereferenceable(32) %coro.hdl) {
entry.cleanup:
  %coro.promise = alloca i64, align 8
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %Switch
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 1, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave4:                                        ; preds = %gen.yield.0.resume
  %ResumeFn.addr = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend5

CoroSuspend5:                                     ; preds = %CoroSave4
  br label %resume.2.landing

resume.2:                                         ; preds = %resume.entry
  br label %resume.2.landing

resume.2.landing:                                 ; preds = %resume.2, %CoroSuspend5
  %0 = phi i8 [ -1, %CoroSuspend5 ], [ 1, %resume.2 ]
  br label %AfterCoroSuspend6

AfterCoroSuspend6:                                ; preds = %resume.2.landing
  %cond = icmp eq i8 %0, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend3, %AfterCoroSuspend6, %AfterCoroSuspend
  call void @free(ptr null)
  br label %coro.suspend

coro.suspend:                                     ; preds = %AfterCoroSuspend3, %coro.cleanup, %AfterCoroSuspend6, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  store i64 42, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave1

CoroSave1:                                        ; preds = %coro.init.resume
  %index.addr8 = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 3
  store i2 1, ptr %index.addr8, align 1
  br label %CoroSuspend2

CoroSuspend2:                                     ; preds = %CoroSave1
  br label %resume.1.landing

resume.1:                                         ; preds = %Switch
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %resume.1, %CoroSuspend2
  %1 = phi i8 [ -1, %CoroSuspend2 ], [ 1, %resume.1 ]
  br label %AfterCoroSuspend3

AfterCoroSuspend3:                                ; preds = %resume.1.landing
  switch i8 %1, label %coro.suspend [
    i8 0, label %gen.yield.0.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.0.resume:                               ; preds = %AfterCoroSuspend3
  br label %CoroSave4

resume.entry:                                     ; preds = %entry.cleanup
  %index.addr = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 3
  %index = load i2, ptr %index.addr, align 1
  %ResumeFn.addr1 = getelementptr inbounds %"\D8\A8\D8\B3\D9\8A\D8\B7.Frame", ptr %coro.hdl, i32 0, i32 0
  %2 = load ptr, ptr %ResumeFn.addr1, align 8
  %3 = icmp eq ptr %2, null
  br i1 %3, label %resume.2, label %Switch

Switch:                                           ; preds = %resume.entry
  switch i2 %index, label %unreachable [
    i2 0, label %resume.0
    i2 1, label %resume.1
  ]

unreachable:                                      ; preds = %Switch
  unreachable
}

define internal fastcc void @"\D8\B6\D8\B9\D9\81.resume"(ptr noundef nonnull align 8 dereferenceable(40) %coro.hdl) {
entry.resume:
  %coro.promise = alloca i64, align 8
  %"\D9\861" = alloca i64, align 8
  %"\D9\861.reload.addr" = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 3
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %resume.entry
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 0, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave5:                                        ; preds = %gen.yield.0.resume
  %ResumeFn.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend6

CoroSuspend6:                                     ; preds = %CoroSave5
  br label %resume.2.landing

resume.2.landing:                                 ; preds = %CoroSuspend6
  br label %AfterCoroSuspend7

AfterCoroSuspend7:                                ; preds = %resume.2.landing
  %cond = icmp eq i8 -1, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend4, %AfterCoroSuspend7, %AfterCoroSuspend
  call void @free(ptr %coro.hdl)
  br label %coro.suspend

coro.suspend:                                     ; preds = %AfterCoroSuspend4, %coro.cleanup, %AfterCoroSuspend7, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  %"%\D9\86.load" = load i64, ptr %"\D9\861.reload.addr", align 8
  %shltmp = shl i64 %"%\D9\86.load", 1
  store i64 %shltmp, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave2

CoroSave2:                                        ; preds = %coro.init.resume
  %index.addr9 = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 4
  store i2 1, ptr %index.addr9, align 1
  br label %CoroSuspend3

CoroSuspend3:                                     ; preds = %CoroSave2
  br label %resume.1.landing

resume.1:                                         ; preds = %resume.entry
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %resume.1, %CoroSuspend3
  %0 = phi i8 [ -1, %CoroSuspend3 ], [ 0, %resume.1 ]
  br label %AfterCoroSuspend4

AfterCoroSuspend4:                                ; preds = %resume.1.landing
  switch i8 %0, label %coro.suspend [
    i8 0, label %gen.yield.0.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.0.resume:                               ; preds = %AfterCoroSuspend4
  br label %CoroSave5

resume.entry:                                     ; preds = %entry.resume
  %index.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 4
  %index = load i2, ptr %index.addr, align 1
  switch i2 %index, label %unreachable [
    i2 0, label %resume.0
    i2 1, label %resume.1
  ]

unreachable:                                      ; preds = %resume.entry
  unreachable
}

define internal fastcc void @"\D8\B6\D8\B9\D9\81.destroy"(ptr noundef nonnull align 8 dereferenceable(40) %coro.hdl) {
entry.destroy:
  %coro.promise = alloca i64, align 8
  %"\D9\861" = alloca i64, align 8
  %"\D9\861.reload.addr" = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 3
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %Switch
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 1, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave5:                                        ; preds = %gen.yield.0.resume
  %ResumeFn.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend6

CoroSuspend6:                                     ; preds = %CoroSave5
  br label %resume.2.landing

resume.2:                                         ; preds = %resume.entry
  br label %resume.2.landing

resume.2.landing:                                 ; preds = %resume.2, %CoroSuspend6
  %0 = phi i8 [ -1, %CoroSuspend6 ], [ 1, %resume.2 ]
  br label %AfterCoroSuspend7

AfterCoroSuspend7:                                ; preds = %resume.2.landing
  %cond = icmp eq i8 %0, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend4, %AfterCoroSuspend7, %AfterCoroSuspend
  call void @free(ptr %coro.hdl)
  br label %coro.suspend

coro.suspend:                                     ; preds = %AfterCoroSuspend4, %coro.cleanup, %AfterCoroSuspend7, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  %"%\D9\86.load" = load i64, ptr %"\D9\861.reload.addr", align 8
  %shltmp = shl i64 %"%\D9\86.load", 1
  store i64 %shltmp, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave2

CoroSave2:                                        ; preds = %coro.init.resume
  %index.addr9 = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 4
  store i2 1, ptr %index.addr9, align 1
  br label %CoroSuspend3

CoroSuspend3:                                     ; preds = %CoroSave2
  br label %resume.1.landing

resume.1:                                         ; preds = %Switch
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %resume.1, %CoroSuspend3
  %1 = phi i8 [ -1, %CoroSuspend3 ], [ 1, %resume.1 ]
  br label %AfterCoroSuspend4

AfterCoroSuspend4:                                ; preds = %resume.1.landing
  switch i8 %1, label %coro.suspend [
    i8 0, label %gen.yield.0.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.0.resume:                               ; preds = %AfterCoroSuspend4
  br label %CoroSave5

resume.entry:                                     ; preds = %entry.destroy
  %index.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 4
  %index = load i2, ptr %index.addr, align 1
  %ResumeFn.addr1 = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 0
  %2 = load ptr, ptr %ResumeFn.addr1, align 8
  %3 = icmp eq ptr %2, null
  br i1 %3, label %resume.2, label %Switch

Switch:                                           ; preds = %resume.entry
  switch i2 %index, label %unreachable [
    i2 0, label %resume.0
    i2 1, label %resume.1
  ]

unreachable:                                      ; preds = %Switch
  unreachable
}

define internal fastcc void @"\D8\B6\D8\B9\D9\81.cleanup"(ptr noundef nonnull align 8 dereferenceable(40) %coro.hdl) {
entry.cleanup:
  %coro.promise = alloca i64, align 8
  %"\D9\861" = alloca i64, align 8
  %"\D9\861.reload.addr" = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 3
  %coro.promise.reload.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %Switch
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 1, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave5:                                        ; preds = %gen.yield.0.resume
  %ResumeFn.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend6

CoroSuspend6:                                     ; preds = %CoroSave5
  br label %resume.2.landing

resume.2:                                         ; preds = %resume.entry
  br label %resume.2.landing

resume.2.landing:                                 ; preds = %resume.2, %CoroSuspend6
  %0 = phi i8 [ -1, %CoroSuspend6 ], [ 1, %resume.2 ]
  br label %AfterCoroSuspend7

AfterCoroSuspend7:                                ; preds = %resume.2.landing
  %cond = icmp eq i8 %0, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend4, %AfterCoroSuspend7, %AfterCoroSuspend
  call void @free(ptr null)
  br label %coro.suspend

coro.suspend:                                     ; preds = %AfterCoroSuspend4, %coro.cleanup, %AfterCoroSuspend7, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  %"%\D9\86.load" = load i64, ptr %"\D9\861.reload.addr", align 8
  %shltmp = shl i64 %"%\D9\86.load", 1
  store i64 %shltmp, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave2

CoroSave2:                                        ; preds = %coro.init.resume
  %index.addr9 = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 4
  store i2 1, ptr %index.addr9, align 1
  br label %CoroSuspend3

CoroSuspend3:                                     ; preds = %CoroSave2
  br label %resume.1.landing

resume.1:                                         ; preds = %Switch
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %resume.1, %CoroSuspend3
  %1 = phi i8 [ -1, %CoroSuspend3 ], [ 1, %resume.1 ]
  br label %AfterCoroSuspend4

AfterCoroSuspend4:                                ; preds = %resume.1.landing
  switch i8 %1, label %coro.suspend [
    i8 0, label %gen.yield.0.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.0.resume:                               ; preds = %AfterCoroSuspend4
  br label %CoroSave5

resume.entry:                                     ; preds = %entry.cleanup
  %index.addr = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 4
  %index = load i2, ptr %index.addr, align 1
  %ResumeFn.addr1 = getelementptr inbounds %"\D8\B6\D8\B9\D9\81.Frame", ptr %coro.hdl, i32 0, i32 0
  %2 = load ptr, ptr %ResumeFn.addr1, align 8
  %3 = icmp eq ptr %2, null
  br i1 %3, label %resume.2, label %Switch

Switch:                                           ; preds = %resume.entry
  switch i2 %index, label %unreachable [
    i2 0, label %resume.0
    i2 1, label %resume.1
  ]

unreachable:                                      ; preds = %Switch
  unreachable
}

define internal fastcc void @"\D9\85\D8\AC\D9\85\D9\88\D8\B9.resume"(ptr noundef nonnull align 8 dereferenceable(32) %coro.hdl) {
entry.resume:
  %coro.promise = alloca i64, align 8
  %coro.promise.reload.addr = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %resume.entry
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 0, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave10:                                       ; preds = %gen.yield.2.resume
  %ResumeFn.addr = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend11

CoroSuspend11:                                    ; preds = %CoroSave10
  br label %resume.4.landing

resume.4.landing:                                 ; preds = %CoroSuspend11
  br label %AfterCoroSuspend12

AfterCoroSuspend12:                               ; preds = %resume.4.landing
  %cond = icmp eq i8 -1, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend3, %AfterCoroSuspend9, %AfterCoroSuspend6, %AfterCoroSuspend12, %AfterCoroSuspend
  call void @free(ptr %coro.hdl)
  br label %coro.suspend

coro.suspend:                                     ; preds = %AfterCoroSuspend3, %AfterCoroSuspend9, %AfterCoroSuspend6, %coro.cleanup, %AfterCoroSuspend12, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  store i64 10, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave4

CoroSave4:                                        ; preds = %coro.init.resume
  %index.addr15 = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 3
  store i3 2, ptr %index.addr15, align 1
  br label %CoroSuspend5

CoroSuspend5:                                     ; preds = %CoroSave4
  br label %resume.2.landing

resume.2:                                         ; preds = %resume.entry
  br label %resume.2.landing

resume.2.landing:                                 ; preds = %resume.2, %CoroSuspend5
  %0 = phi i8 [ -1, %CoroSuspend5 ], [ 0, %resume.2 ]
  br label %AfterCoroSuspend6

AfterCoroSuspend6:                                ; preds = %resume.2.landing
  switch i8 %0, label %coro.suspend [
    i8 0, label %gen.yield.0.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.0.resume:                               ; preds = %AfterCoroSuspend6
  store i64 20, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave7

CoroSave7:                                        ; preds = %gen.yield.0.resume
  %index.addr16 = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 3
  store i3 3, ptr %index.addr16, align 1
  br label %CoroSuspend8

CoroSuspend8:                                     ; preds = %CoroSave7
  br label %resume.3.landing

resume.3:                                         ; preds = %resume.entry
  br label %resume.3.landing

resume.3.landing:                                 ; preds = %resume.3, %CoroSuspend8
  %1 = phi i8 [ -1, %CoroSuspend8 ], [ 0, %resume.3 ]
  br label %AfterCoroSuspend9

AfterCoroSuspend9:                                ; preds = %resume.3.landing
  switch i8 %1, label %coro.suspend [
    i8 0, label %gen.yield.1.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.1.resume:                               ; preds = %AfterCoroSuspend9
  store i64 30, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave1

CoroSave1:                                        ; preds = %gen.yield.1.resume
  %index.addr14 = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 3
  store i3 1, ptr %index.addr14, align 1
  br label %CoroSuspend2

CoroSuspend2:                                     ; preds = %CoroSave1
  br label %resume.1.landing

resume.1:                                         ; preds = %resume.entry
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %resume.1, %CoroSuspend2
  %2 = phi i8 [ -1, %CoroSuspend2 ], [ 0, %resume.1 ]
  br label %AfterCoroSuspend3

AfterCoroSuspend3:                                ; preds = %resume.1.landing
  switch i8 %2, label %coro.suspend [
    i8 0, label %gen.yield.2.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.2.resume:                               ; preds = %AfterCoroSuspend3
  br label %CoroSave10

resume.entry:                                     ; preds = %entry.resume
  %index.addr = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 3
  %index = load i3, ptr %index.addr, align 1
  switch i3 %index, label %unreachable [
    i3 0, label %resume.0
    i3 1, label %resume.1
    i3 2, label %resume.2
    i3 3, label %resume.3
  ]

unreachable:                                      ; preds = %resume.entry
  unreachable
}

define internal fastcc void @"\D9\85\D8\AC\D9\85\D9\88\D8\B9.destroy"(ptr noundef nonnull align 8 dereferenceable(32) %coro.hdl) {
entry.destroy:
  %coro.promise = alloca i64, align 8
  %coro.promise.reload.addr = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %Switch
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 1, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave10:                                       ; preds = %gen.yield.2.resume
  %ResumeFn.addr = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend11

CoroSuspend11:                                    ; preds = %CoroSave10
  br label %resume.4.landing

resume.4:                                         ; preds = %resume.entry
  br label %resume.4.landing

resume.4.landing:                                 ; preds = %resume.4, %CoroSuspend11
  %0 = phi i8 [ -1, %CoroSuspend11 ], [ 1, %resume.4 ]
  br label %AfterCoroSuspend12

AfterCoroSuspend12:                               ; preds = %resume.4.landing
  %cond = icmp eq i8 %0, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend3, %AfterCoroSuspend9, %AfterCoroSuspend6, %AfterCoroSuspend12, %AfterCoroSuspend
  call void @free(ptr %coro.hdl)
  br label %coro.suspend

coro.suspend:                                     ; preds = %AfterCoroSuspend3, %AfterCoroSuspend9, %AfterCoroSuspend6, %coro.cleanup, %AfterCoroSuspend12, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  store i64 10, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave4

CoroSave4:                                        ; preds = %coro.init.resume
  %index.addr15 = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 3
  store i3 2, ptr %index.addr15, align 1
  br label %CoroSuspend5

CoroSuspend5:                                     ; preds = %CoroSave4
  br label %resume.2.landing

resume.2:                                         ; preds = %Switch
  br label %resume.2.landing

resume.2.landing:                                 ; preds = %resume.2, %CoroSuspend5
  %1 = phi i8 [ -1, %CoroSuspend5 ], [ 1, %resume.2 ]
  br label %AfterCoroSuspend6

AfterCoroSuspend6:                                ; preds = %resume.2.landing
  switch i8 %1, label %coro.suspend [
    i8 0, label %gen.yield.0.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.0.resume:                               ; preds = %AfterCoroSuspend6
  store i64 20, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave7

CoroSave7:                                        ; preds = %gen.yield.0.resume
  %index.addr16 = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 3
  store i3 3, ptr %index.addr16, align 1
  br label %CoroSuspend8

CoroSuspend8:                                     ; preds = %CoroSave7
  br label %resume.3.landing

resume.3:                                         ; preds = %Switch
  br label %resume.3.landing

resume.3.landing:                                 ; preds = %resume.3, %CoroSuspend8
  %2 = phi i8 [ -1, %CoroSuspend8 ], [ 1, %resume.3 ]
  br label %AfterCoroSuspend9

AfterCoroSuspend9:                                ; preds = %resume.3.landing
  switch i8 %2, label %coro.suspend [
    i8 0, label %gen.yield.1.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.1.resume:                               ; preds = %AfterCoroSuspend9
  store i64 30, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave1

CoroSave1:                                        ; preds = %gen.yield.1.resume
  %index.addr14 = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 3
  store i3 1, ptr %index.addr14, align 1
  br label %CoroSuspend2

CoroSuspend2:                                     ; preds = %CoroSave1
  br label %resume.1.landing

resume.1:                                         ; preds = %Switch
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %resume.1, %CoroSuspend2
  %3 = phi i8 [ -1, %CoroSuspend2 ], [ 1, %resume.1 ]
  br label %AfterCoroSuspend3

AfterCoroSuspend3:                                ; preds = %resume.1.landing
  switch i8 %3, label %coro.suspend [
    i8 0, label %gen.yield.2.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.2.resume:                               ; preds = %AfterCoroSuspend3
  br label %CoroSave10

resume.entry:                                     ; preds = %entry.destroy
  %index.addr = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 3
  %index = load i3, ptr %index.addr, align 1
  %ResumeFn.addr1 = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 0
  %4 = load ptr, ptr %ResumeFn.addr1, align 8
  %5 = icmp eq ptr %4, null
  br i1 %5, label %resume.4, label %Switch

Switch:                                           ; preds = %resume.entry
  switch i3 %index, label %unreachable [
    i3 0, label %resume.0
    i3 1, label %resume.1
    i3 2, label %resume.2
    i3 3, label %resume.3
  ]

unreachable:                                      ; preds = %Switch
  unreachable
}

define internal fastcc void @"\D9\85\D8\AC\D9\85\D9\88\D8\B9.cleanup"(ptr noundef nonnull align 8 dereferenceable(32) %coro.hdl) {
entry.cleanup:
  %coro.promise = alloca i64, align 8
  %coro.promise.reload.addr = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 2
  br label %resume.entry

resume.0:                                         ; preds = %Switch
  br label %resume.0.landing

resume.0.landing:                                 ; preds = %resume.0
  br label %AfterCoroSuspend

AfterCoroSuspend:                                 ; preds = %resume.0.landing
  switch i8 1, label %coro.suspend [
    i8 0, label %coro.init.resume
    i8 1, label %coro.cleanup
  ]

CoroSave10:                                       ; preds = %gen.yield.2.resume
  %ResumeFn.addr = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 0
  store ptr null, ptr %ResumeFn.addr, align 8
  br label %CoroSuspend11

CoroSuspend11:                                    ; preds = %CoroSave10
  br label %resume.4.landing

resume.4:                                         ; preds = %resume.entry
  br label %resume.4.landing

resume.4.landing:                                 ; preds = %resume.4, %CoroSuspend11
  %0 = phi i8 [ -1, %CoroSuspend11 ], [ 1, %resume.4 ]
  br label %AfterCoroSuspend12

AfterCoroSuspend12:                               ; preds = %resume.4.landing
  %cond = icmp eq i8 %0, 1
  br i1 %cond, label %coro.cleanup, label %coro.suspend

coro.cleanup:                                     ; preds = %AfterCoroSuspend3, %AfterCoroSuspend9, %AfterCoroSuspend6, %AfterCoroSuspend12, %AfterCoroSuspend
  call void @free(ptr null)
  br label %coro.suspend

coro.suspend:                                     ; preds = %AfterCoroSuspend3, %AfterCoroSuspend9, %AfterCoroSuspend6, %coro.cleanup, %AfterCoroSuspend12, %AfterCoroSuspend
  br label %CoroEnd

CoroEnd:                                          ; preds = %coro.suspend
  ret void

coro.init.resume:                                 ; preds = %AfterCoroSuspend
  store i64 10, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave4

CoroSave4:                                        ; preds = %coro.init.resume
  %index.addr15 = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 3
  store i3 2, ptr %index.addr15, align 1
  br label %CoroSuspend5

CoroSuspend5:                                     ; preds = %CoroSave4
  br label %resume.2.landing

resume.2:                                         ; preds = %Switch
  br label %resume.2.landing

resume.2.landing:                                 ; preds = %resume.2, %CoroSuspend5
  %1 = phi i8 [ -1, %CoroSuspend5 ], [ 1, %resume.2 ]
  br label %AfterCoroSuspend6

AfterCoroSuspend6:                                ; preds = %resume.2.landing
  switch i8 %1, label %coro.suspend [
    i8 0, label %gen.yield.0.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.0.resume:                               ; preds = %AfterCoroSuspend6
  store i64 20, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave7

CoroSave7:                                        ; preds = %gen.yield.0.resume
  %index.addr16 = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 3
  store i3 3, ptr %index.addr16, align 1
  br label %CoroSuspend8

CoroSuspend8:                                     ; preds = %CoroSave7
  br label %resume.3.landing

resume.3:                                         ; preds = %Switch
  br label %resume.3.landing

resume.3.landing:                                 ; preds = %resume.3, %CoroSuspend8
  %2 = phi i8 [ -1, %CoroSuspend8 ], [ 1, %resume.3 ]
  br label %AfterCoroSuspend9

AfterCoroSuspend9:                                ; preds = %resume.3.landing
  switch i8 %2, label %coro.suspend [
    i8 0, label %gen.yield.1.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.1.resume:                               ; preds = %AfterCoroSuspend9
  store i64 30, ptr %coro.promise.reload.addr, align 8
  br label %CoroSave1

CoroSave1:                                        ; preds = %gen.yield.1.resume
  %index.addr14 = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 3
  store i3 1, ptr %index.addr14, align 1
  br label %CoroSuspend2

CoroSuspend2:                                     ; preds = %CoroSave1
  br label %resume.1.landing

resume.1:                                         ; preds = %Switch
  br label %resume.1.landing

resume.1.landing:                                 ; preds = %resume.1, %CoroSuspend2
  %3 = phi i8 [ -1, %CoroSuspend2 ], [ 1, %resume.1 ]
  br label %AfterCoroSuspend3

AfterCoroSuspend3:                                ; preds = %resume.1.landing
  switch i8 %3, label %coro.suspend [
    i8 0, label %gen.yield.2.resume
    i8 1, label %coro.cleanup
  ]

gen.yield.2.resume:                               ; preds = %AfterCoroSuspend3
  br label %CoroSave10

resume.entry:                                     ; preds = %entry.cleanup
  %index.addr = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 3
  %index = load i3, ptr %index.addr, align 1
  %ResumeFn.addr1 = getelementptr inbounds %"\D9\85\D8\AC\D9\85\D9\88\D8\B9.Frame", ptr %coro.hdl, i32 0, i32 0
  %4 = load ptr, ptr %ResumeFn.addr1, align 8
  %5 = icmp eq ptr %4, null
  br i1 %5, label %resume.4, label %Switch

Switch:                                           ; preds = %resume.entry
  switch i3 %index, label %unreachable [
    i3 0, label %resume.0
    i3 1, label %resume.1
    i3 2, label %resume.2
    i3 3, label %resume.3
  ]

unreachable:                                      ; preds = %Switch
  unreachable
}

attributes #0 = { nocallback nofree nosync nounwind willreturn memory(argmem: read) }
attributes #1 = { nounwind memory(none) }
attributes #2 = { nounwind }
attributes #3 = { nomerge nounwind }
attributes #4 = { nounwind memory(argmem: read) }
attributes #5 = { nounwind memory(argmem: readwrite) }
