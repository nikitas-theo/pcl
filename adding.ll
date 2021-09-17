; ModuleID = 'adding'
source_filename = "adding"

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
  %x = alloca [10 x i32]
  %z = alloca i32
  %bitcast = bitcast [10 x i32]* %x to i32*
  %arrayIdx = getelementptr i32, i32* %bitcast, i64 1
  store i32 2, i32* %arrayIdx
  store i32 4, i32* %z
  %0 = load i32, i32* %z
  call void @writeInteger(i32 %0)
  %bitcast1 = bitcast [10 x i32]* %x to i32*
  %arrayIdx2 = getelementptr i32, i32* %bitcast1, i64 1
  %1 = load i32, i32* %arrayIdx2
  call void @writeInteger(i32 %1)
  ret i32 0
}
