; ModuleID = 'param_pass2'
source_filename = "param_pass2"

%anon = type { <{}>*, i32*, [3 x i8]* }
%anon.0 = type { %anon* }

@str = private unnamed_addr constant [3 x i8] c"99\00", align 1

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
  %hidden_struct = alloca %anon
  %z = alloca i32
  %a = alloca [3 x i8]
  store i32 1, i32* %z
  %assign = load [3 x i8], [3 x i8]* @str
  store [3 x i8] %assign, [3 x i8]* %a
  %0 = getelementptr %anon, %anon* %hidden_struct, i32 0, i32 2
  store [3 x i8]* %a, [3 x i8]** %0
  %1 = getelementptr %anon, %anon* %hidden_struct, i32 0, i32 1
  store i32* %z, i32** %1
  call void @x(%anon* %hidden_struct)
  %bitcast_special_ref = bitcast [3 x i8]* %a to i8*
  call void @writeString(i8* %bitcast_special_ref)
  ret void
}

define void @x(%anon* %0) {
entry:
  %hidden_struct = alloca %anon.0
  %1 = getelementptr %anon.0, %anon.0* %hidden_struct, i32 0, i32 0
  store %anon* %0, %anon** %1
  %e = alloca i32
  %2 = getelementptr %anon.0, %anon.0* %hidden_struct, i32 0, i32 0
  %3 = load %anon*, %anon** %2
  %4 = getelementptr %anon, %anon* %3, i32 0, i32 1
  %5 = load i32*, i32** %4
  %binop_l = load i32, i32* %5
  %addtmp = add i32 %binop_l, 1
  store i32 %addtmp, i32* %e
  %6 = getelementptr %anon.0, %anon.0* %hidden_struct, i32 0, i32 0
  %7 = load %anon*, %anon** %6
  %8 = getelementptr %anon, %anon* %7, i32 0, i32 2
  %9 = load [3 x i8]*, [3 x i8]** %8
  %bitcast_special_ref = bitcast [3 x i8]* %9 to i8*
  call void @writeString(i8* %bitcast_special_ref)
  %call = load i32, i32* %e
  call void @writeInteger(i32 %call)
  ret void
}
