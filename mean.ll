; ModuleID = 'mean'
source_filename = "mean"

@str = private unnamed_addr constant [9 x i8] c"Give n: \00", align 1
@str.1 = private unnamed_addr constant [9 x i8] c"Give k: \00", align 1
@str.2 = private unnamed_addr constant [7 x i8] c"Mean: \00", align 1
@str.3 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

declare i8* @GC_malloc(i64)

declare void @GC_init()

declare void @GC_free(i8*)

declare void @writeInteger(i32)

declare void @writeBoolean(i1)

declare void @writeChar(i8)

declare void @writeReal(double)

declare void @writeString(i8*)

declare i32 @readInteger()

declare i1 @readBoolean()

declare i8 @readChar()

declare double @readReal()

declare i8* @readString()

declare i32 @abs(i32)

declare double @fabs(double)

declare double @sqrt(double)

declare double @sin(double)

declare double @cos(double)

declare double @tan(double)

declare double @arctan(double)

declare double @exp(double)

declare double @ln(double)

declare double @pi()

declare i8 @ord(i32)

declare i32 @chr(i8)

declare i32 @truncFunc(double)

declare i32 @roundFunc(double)

define i32 @main() {
entry:
  call void @GC_init()
  %sum = alloca double
  %n = alloca i32
  %k = alloca i32
  %i = alloca i32
  %seed = alloca i32
  call void @writeString(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @str, i32 0, i32 0))
  %0 = call i32 @readInteger()
  store i32 %0, i32* %n
  call void @writeString(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @str.1, i32 0, i32 0))
  %1 = call i32 @readInteger()
  store i32 %1, i32* %k
  store i32 0, i32* %i
  store double 0.000000e+00, double* %sum
  store i32 65, i32* %seed
  br label %loop

loop:                                             ; preds = %body, %entry
  %2 = load i32, i32* %i
  %3 = load i32, i32* %k
  %4 = icmp slt i32 %2, %3
  br i1 %4, label %body, label %endloop

endloop:                                          ; preds = %loop
  %5 = load i32, i32* %k
  %6 = icmp sgt i32 %5, 0
  %if_cond = icmp ne i1 %6, false
  br i1 %if_cond, label %then, label %else

body:                                             ; preds = %loop
  %7 = load i32, i32* %seed
  %addtmp = mul i32 %7, 137
  %addtmp1 = add i32 %addtmp, 221
  %8 = load i32, i32* %i
  %addtmp2 = add i32 %addtmp1, %8
  %9 = load i32, i32* %n
  %modtmp = srem i32 %addtmp2, %9
  store i32 %modtmp, i32* %seed
  %10 = load double, double* %sum
  %11 = load i32, i32* %seed
  %12 = sitofp i32 %11 to double
  %addtmp3 = fadd double %10, %12
  store double %addtmp3, double* %sum
  %13 = load i32, i32* %i
  %addtmp4 = add i32 %13, 1
  store i32 %addtmp4, i32* %i
  br label %loop

then:                                             ; preds = %endloop
  call void @writeString(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @str.2, i32 0, i32 0))
  %14 = load double, double* %sum
  %15 = load i32, i32* %k
  %16 = sitofp i32 %15 to double
  %addtmp5 = fdiv double %14, %16
  call void @writeReal(double %addtmp5)
  call void @writeString(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @str.3, i32 0, i32 0))
  br label %endif

endif:                                            ; preds = %else, %then
  ret i32 0

else:                                             ; preds = %endloop
  br label %endif
}
