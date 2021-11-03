; ModuleID = 'byref'
source_filename = "byref"

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
  %b = alloca i8*
  %0 = call i8* @GC_malloc(i64 mul (i64 ptrtoint (i8* getelementptr (i8, i8* null, i64 1) to i64), i64 10))
  store i8* %0, i8** %b
  %1 = load i8*, i8** %b
  %arrayIdx = getelementptr i8, i8* %1, i64 0
  store i8 65, i8* %arrayIdx
  %2 = load i8*, i8** %b
  %arrayIdx1 = getelementptr i8, i8* %2, i64 1
  store i8 66, i8* %arrayIdx1
  %3 = load i8*, i8** %b
  call void @writeString(i8* %3)
  %4 = load i8*, i8** %b
  call void @GC_free(i8* %4)
  %5 = load i8*, i8** %b
  call void @writeString(i8* %5)
  ret i32 0
}
