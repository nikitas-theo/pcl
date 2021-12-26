; ModuleID = 'input.1'
source_filename = "input.1"

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

define void @main() {
entry:
  call void @GC_init()
  %z = alloca i32
  store i32 4, i32* %z
  %call = load i32, i32* %z
  call void @writeInteger(i32 %call)
  ret void
}
