; ModuleID = 'param_pass'
source_filename = "param_pass"

%anon = type { <{}>*, i32 }
%anon.0 = type { %anon* }

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
  store i32 1, i32* %z
  %0 = getelementptr %anon, %anon* %hidden_struct, i32 0, i32 1
  %1 = load i32, i32* %z
  store i32 %1, i32* %0
  call void @x(%anon* %hidden_struct)
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
  %binop_l = load i32, i32* %4
  %addtmp = add i32 %binop_l, 1
  store i32 %addtmp, i32* %e
  %call = load i32, i32* %e
  call void @writeInteger(i32 %call)
  ret void
}
