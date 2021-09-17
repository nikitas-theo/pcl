; ModuleID = 'primes'
source_filename = "primes"

@str = private unnamed_addr constant [32 x i8] c"Please, give the upper limit : \00", align 1
@str.1 = private unnamed_addr constant [29 x i8] c"Prime numbers between 0 and \00", align 1
@str.2 = private unnamed_addr constant [3 x i8] c"\0A\0A\00", align 1
@str.3 = private unnamed_addr constant [3 x i8] c"2\0A\00", align 1
@str.4 = private unnamed_addr constant [3 x i8] c"3\0A\00", align 1
@str.5 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.6 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.7 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.8 = private unnamed_addr constant [30 x i8] c" prime number(s) were found.\0A\00", align 1

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
  %limit = alloca i32
  %number = alloca i32
  %counter = alloca i32
  call void @writeString(i8* getelementptr inbounds ([32 x i8], [32 x i8]* @str, i32 0, i32 0))
  %0 = call i32 @readInteger()
  store i32 %0, i32* %limit
  call void @writeString(i8* getelementptr inbounds ([29 x i8], [29 x i8]* @str.1, i32 0, i32 0))
  %1 = load i32, i32* %limit
  call void @writeInteger(i32 %1)
  call void @writeString(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @str.2, i32 0, i32 0))
  store i32 0, i32* %counter
  %2 = load i32, i32* %limit
  %3 = icmp sge i32 %2, 2
  %if_cond = icmp ne i1 %3, false
  br i1 %if_cond, label %then, label %else

then:                                             ; preds = %entry
  %4 = load i32, i32* %counter
  %addtmp = add i32 %4, 1
  store i32 %addtmp, i32* %counter
  call void @writeString(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @str.3, i32 0, i32 0))
  br label %endif

endif:                                            ; preds = %else, %then
  %5 = load i32, i32* %limit
  %6 = icmp sge i32 %5, 3
  %if_cond1 = icmp ne i1 %6, false
  br i1 %if_cond1, label %then2, label %else4

else:                                             ; preds = %entry
  br label %endif

then2:                                            ; preds = %endif
  %7 = load i32, i32* %counter
  %addtmp5 = add i32 %7, 1
  store i32 %addtmp5, i32* %counter
  call void @writeString(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @str.4, i32 0, i32 0))
  br label %endif3

endif3:                                           ; preds = %else4, %then2
  store i32 6, i32* %number
  br label %loop

else4:                                            ; preds = %endif
  br label %endif3

loop:                                             ; preds = %endif16, %endif3
  %8 = load i32, i32* %number
  %9 = load i32, i32* %limit
  %10 = icmp sle i32 %8, %9
  br i1 %10, label %body, label %endloop

endloop:                                          ; preds = %loop
  call void @writeString(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @str.7, i32 0, i32 0))
  %11 = load i32, i32* %counter
  call void @writeInteger(i32 %11)
  call void @writeString(i8* getelementptr inbounds ([30 x i8], [30 x i8]* @str.8, i32 0, i32 0))
  ret i32 0

body:                                             ; preds = %loop
  %12 = load i32, i32* %number
  %addtmp6 = sub i32 %12, 1
  %13 = call i1 @prime(i32 %addtmp6)
  %if_cond7 = icmp ne i1 %13, false
  br i1 %if_cond7, label %then8, label %else10

then8:                                            ; preds = %body
  %14 = load i32, i32* %counter
  %addtmp11 = add i32 %14, 1
  store i32 %addtmp11, i32* %counter
  %15 = load i32, i32* %number
  %addtmp12 = sub i32 %15, 1
  call void @writeInteger(i32 %addtmp12)
  call void @writeString(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @str.5, i32 0, i32 0))
  br label %endif9

endif9:                                           ; preds = %else10, %then8
  %16 = load i32, i32* %number
  %17 = load i32, i32* %limit
  %neqtmp = icmp ne i32 %16, %17
  %18 = icmp eq i1 %neqtmp, false
  br i1 %18, label %short_and, label %full_and

else10:                                           ; preds = %body
  br label %endif9

short_and:                                        ; preds = %endif9
  br label %end_and

full_and:                                         ; preds = %endif9
  %19 = load i32, i32* %number
  %addtmp13 = add i32 %19, 1
  %20 = call i1 @prime(i32 %addtmp13)
  %andtmp = and i1 %neqtmp, %20
  br label %end_and

end_and:                                          ; preds = %full_and, %short_and
  %phi_and = phi i1 [ false, %short_and ], [ %andtmp, %full_and ]
  %if_cond14 = icmp ne i1 %phi_and, false
  br i1 %if_cond14, label %then15, label %else17

then15:                                           ; preds = %end_and
  %21 = load i32, i32* %counter
  %addtmp18 = add i32 %21, 1
  store i32 %addtmp18, i32* %counter
  %22 = load i32, i32* %number
  %addtmp19 = add i32 %22, 1
  call void @writeInteger(i32 %addtmp19)
  call void @writeString(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @str.6, i32 0, i32 0))
  br label %endif16

endif16:                                          ; preds = %else17, %then15
  %23 = load i32, i32* %number
  %addtmp20 = add i32 %23, 6
  store i32 %addtmp20, i32* %number
  br label %loop

else17:                                           ; preds = %end_and
  br label %endif16
}

define i1 @prime(i32 %0) {
entry:
  %n = alloca i32
  store i32 %0, i32* %n
  %result = alloca i1
  %i = alloca i32
  %1 = load i32, i32* %n
  %2 = icmp slt i32 %1, 0
  %if_cond = icmp ne i1 %2, false
  br i1 %if_cond, label %then, label %else

then:                                             ; preds = %entry
  %3 = load i32, i32* %n
  %4 = sub i32 0, %3
  %5 = call i1 @prime(i32 %4)
  store i1 %5, i1* %result
  br label %endif

endif:                                            ; preds = %endif3, %then
  store i1 true, i1* %result
  %6 = load i1, i1* %result
  ret i1 %6

else:                                             ; preds = %entry
  %7 = load i32, i32* %n
  %8 = icmp slt i32 %7, 2
  %if_cond1 = icmp ne i1 %8, false
  br i1 %if_cond1, label %then2, label %else4

then2:                                            ; preds = %else
  store i1 false, i1* %result
  br label %endif3

endif3:                                           ; preds = %endif7, %then2
  br label %endif

else4:                                            ; preds = %else
  %9 = load i32, i32* %n
  %eqtmp = icmp eq i32 %9, 2
  %if_cond5 = icmp ne i1 %eqtmp, false
  br i1 %if_cond5, label %then6, label %else8

then6:                                            ; preds = %else4
  store i1 true, i1* %result
  br label %endif7

endif7:                                           ; preds = %endif12, %then6
  br label %endif3

else8:                                            ; preds = %else4
  %10 = load i32, i32* %n
  %modtmp = srem i32 %10, 2
  %eqtmp9 = icmp eq i32 %modtmp, 0
  %if_cond10 = icmp ne i1 %eqtmp9, false
  br i1 %if_cond10, label %then11, label %else13

then11:                                           ; preds = %else8
  store i1 false, i1* %result
  br label %endif12

endif12:                                          ; preds = %endloop, %then11
  br label %endif7

else13:                                           ; preds = %else8
  store i32 3, i32* %i
  br label %loop

loop:                                             ; preds = %endif18, %else13
  %11 = load i32, i32* %n
  %divtmp = sdiv i32 %11, 2
  %12 = load i32, i32* %i
  %13 = icmp sle i32 %12, %divtmp
  br i1 %13, label %body, label %endloop

endloop:                                          ; preds = %loop
  br label %endif12

body:                                             ; preds = %loop
  %14 = load i32, i32* %n
  %15 = load i32, i32* %i
  %modtmp14 = srem i32 %14, %15
  %eqtmp15 = icmp eq i32 %modtmp14, 0
  %if_cond16 = icmp ne i1 %eqtmp15, false
  br i1 %if_cond16, label %then17, label %else19

then17:                                           ; preds = %body
  store i1 false, i1* %result
  %16 = load i1, i1* %result
  ret i1 %16

endif18:                                          ; preds = %else19
  %17 = load i32, i32* %i
  %addtmp = add i32 %17, 2
  store i32 %addtmp, i32* %i
  br label %loop

else19:                                           ; preds = %body
  br label %endif18
}
