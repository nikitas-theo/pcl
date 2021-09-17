; ModuleID = 'array_simple'
source_filename = "array_simple"

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
  %z = alloca i8
  store i8 50, i8* %z
  %0 = load i8, i8* %z
  call void @writeChar(i8 %0)
  %1 = call i32* @zz(i8* %z)
  ret i32 0
}

define i32* @zz(i8* %0) {
entry:
  %y = alloca i8*
  store i8* %0, i8** %y
  %result = alloca i32*
  %1 = load i8*, i8** %y
  store i8 51, i8* %1
  %2 = load i32*, i32** %result
  ret i32* %2
}
