; ModuleID = 'while'
source_filename = "while"

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
  %i = alloca i32
  %seed = alloca i32
  store i32 0, i32* %i
  store i32 0, i32* %seed
  br label %loop

loop:                                             ; preds = %body, %entry
  %0 = load i32, i32* %i
  %1 = icmp slt i32 %0, 16
  br i1 %1, label %body, label %endloop

endloop:                                          ; preds = %loop
  %2 = load i32, i32* %seed
  call void @writeInteger(i32 %2)
  ret i32 0

body:                                             ; preds = %loop
  %3 = load i32, i32* %seed
  %4 = load i32, i32* %i
  %addtmp = add i32 %3, %4
  store i32 %addtmp, i32* %seed
  %5 = load i32, i32* %i
  %addtmp1 = add i32 %5, 1
  store i32 %addtmp1, i32* %i
  br label %loop
}
