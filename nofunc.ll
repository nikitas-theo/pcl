; ModuleID = 'nofunc'
source_filename = "nofunc"

@str = private unnamed_addr constant [8 x i8] c"ant gam\00", align 1

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
  %x = alloca i32
  %n = alloca double
  store double 4.000000e+02, double* %n
  br label %loop

loop:                                             ; preds = %body, %entry
  %0 = load double, double* %n
  %1 = fcmp ogt double %0, 1.000000e+01
  br i1 %1, label %body, label %endloop

endloop:                                          ; preds = %loop
  ret i32 0

body:                                             ; preds = %loop
  call void @writeString(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @str, i32 0, i32 0))
  %2 = load double, double* %n
  %addtmp = fsub double %2, 1.000000e+01
  store double %addtmp, double* %n
  br label %loop
}
