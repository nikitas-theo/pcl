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
  %x = alloca [2 x i8]
  %y = alloca i8*
  %z = alloca [2 x i8]**
  %bitcast = bitcast [2 x i8]* %x to i8*
  %arrayIdx = getelementptr i8, i8* %bitcast, i64 0
  store i8 49, i8* %arrayIdx
  %bitcast1 = bitcast [2 x i8]* %x to i8*
  %arrayIdx2 = getelementptr i8, i8* %bitcast1, i64 1
  store i8 92, i8* %arrayIdx2
  %deref = load [2 x i8]**, [2 x i8]*** %z
  store [2 x i8]* %x, [2 x i8]** %deref
  %deref3 = load [2 x i8]**, [2 x i8]*** %z
  %deref4 = load [2 x i8]*, [2 x i8]** %deref3
  %passArr = getelementptr [2 x i8], [2 x i8]* %deref4, i64 0, i64 0
  call void @zz(i8* %passArr)
  ret i32 0
}

define void @zz(i8* %0) {
entry:
  %y = alloca i8*
  store i8* %0, i8** %y
  %load_id = load i8*, i8** %y
  call void @writeString(i8* %load_id)
  ret void
}
