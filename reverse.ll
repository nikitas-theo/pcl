; ModuleID = 'reverse'
source_filename = "reverse"

%anon = type { <{}>*, [32 x i8] }
%anon.0 = type { %anon* }
%anon.1 = type { %anon* }

@str = private unnamed_addr constant [14 x i8] c"\0A!dlrow olleH\00", align 1
@str.1 = private unnamed_addr constant [3 x i8] c"AA\00", align 1

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
  %r = alloca [32 x i8]
  %0 = getelementptr %anon, %anon* %hidden_struct, i32 0, i32 1
  %1 = load [32 x i8], [32 x i8]* %r
  store [32 x i8] %1, [32 x i8]* %0
  call void @reverse(%anon* %hidden_struct, i8* getelementptr inbounds ([14 x i8], [14 x i8]* @str, i32 0, i32 0))
  %bitcast_special_ref = bitcast [32 x i8]* %r to i8*
  call void @writeString(i8* %bitcast_special_ref)
  call void @writeString(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @str.1, i32 0, i32 0))
  ret void
}

define i32 @strlen(%anon* %0, i8* %1) {
entry:
  %hidden_struct = alloca %anon.0
  %2 = getelementptr %anon.0, %anon.0* %hidden_struct, i32 0, i32 0
  store %anon* %0, %anon** %2
  %result = alloca i32
  store i32 0, i32* %result
  br label %loop

loop:                                             ; preds = %body, %entry
  %arrAcc = load i32, i32* %result
  %3 = sext i32 %arrAcc to i64
  %arrayIdx = getelementptr i8, i8* %1, i64 %3
  %binop_l = load i8, i8* %arrayIdx
  %neqtmp = icmp ne i8 %binop_l, 0
  br i1 %neqtmp, label %body, label %endloop

endloop:                                          ; preds = %loop
  %fdef = load i32, i32* %result
  ret i32 %fdef

body:                                             ; preds = %loop
  %binop_l1 = load i32, i32* %result
  %addtmp = add i32 %binop_l1, 1
  store i32 %addtmp, i32* %result
  br label %loop
}

define void @reverse(%anon* %0, i8* %1) {
entry:
  %hidden_struct = alloca %anon.1
  %2 = getelementptr %anon.1, %anon.1* %hidden_struct, i32 0, i32 0
  store %anon* %0, %anon** %2
  %i = alloca i32
  %l = alloca i32
  %3 = getelementptr %anon.1, %anon.1* %hidden_struct, i32 0, i32 0
  %4 = load %anon*, %anon** %3
  %5 = call i32 @strlen(%anon* %4, i8* %1)
  store i32 %5, i32* %l
  store i32 0, i32* %i
  br label %loop

loop:                                             ; preds = %body, %entry
  %binop_l = load i32, i32* %i
  %binop_r = load i32, i32* %l
  %6 = icmp slt i32 %binop_l, %binop_r
  br i1 %6, label %body, label %endloop

endloop:                                          ; preds = %loop
  %7 = getelementptr %anon.1, %anon.1* %hidden_struct, i32 0, i32 0
  %8 = load %anon*, %anon** %7
  %9 = getelementptr %anon, %anon* %8, i32 0, i32 1
  %arrAcc7 = load i32, i32* %i
  %10 = sext i32 %arrAcc7 to i64
  %arrayIdx8 = getelementptr [32 x i8], [32 x i8]* %9, i64 0, i64 %10
  store i8 0, i8* %arrayIdx8
  ret void

body:                                             ; preds = %loop
  %11 = getelementptr %anon.1, %anon.1* %hidden_struct, i32 0, i32 0
  %12 = load %anon*, %anon** %11
  %13 = getelementptr %anon, %anon* %12, i32 0, i32 1
  %arrAcc = load i32, i32* %i
  %14 = sext i32 %arrAcc to i64
  %arrayIdx = getelementptr [32 x i8], [32 x i8]* %13, i64 0, i64 %14
  %binop_l1 = load i32, i32* %l
  %binop_r2 = load i32, i32* %i
  %addtmp = sub i32 %binop_l1, %binop_r2
  %addtmp3 = sub i32 %addtmp, 1
  %15 = sext i32 %addtmp3 to i64
  %arrayIdx4 = getelementptr i8, i8* %1, i64 %15
  %assign = load i8, i8* %arrayIdx4
  store i8 %assign, i8* %arrayIdx
  %binop_l5 = load i32, i32* %i
  %addtmp6 = add i32 %binop_l5, 1
  store i32 %addtmp6, i32* %i
  br label %loop
}
