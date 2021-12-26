; ModuleID = 'byref'
source_filename = "byref"

@str = private unnamed_addr constant [4 x i8] c"AFA\00", align 1
@str.1 = private unnamed_addr constant [5 x i8] c"exit\00", align 1

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
  %hidden_struct = alloca {}
  %i = alloca i32
  store i32 0, i32* %i
  br label %a1

a:                                                ; No predecessors!
  call void @writeString(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str, i32 0, i32 0))
  br label %a1

a1:                                               ; preds = %endif, %entry, %a
  %call = load i32, i32* %i
  call void @writeInteger(i32 %call)
  %binop_l = load i32, i32* %i
  %addtmp = add i32 %binop_l, 1
  store i32 %addtmp, i32* %i
  %binop_l2 = load i32, i32* %i
  %eqtmp = icmp eq i32 %binop_l2, 5
  %if_cond = icmp ne i1 %eqtmp, false
  br i1 %if_cond, label %then, label %else

then:                                             ; preds = %a1
  br label %b4

endif:                                            ; preds = %else, %b
  br label %a1

else:                                             ; preds = %a1
  br label %endif

b:                                                ; No predecessors!
  br label %endif

a3:                                               ; No predecessors!
  br label %b4

b4:                                               ; preds = %then, %a3
  call void @writeString(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @str.1, i32 0, i32 0))
  ret void
}
