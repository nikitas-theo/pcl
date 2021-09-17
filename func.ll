; ModuleID = 'func'
source_filename = "func"

@str = private unnamed_addr constant [11 x i8] c"Move from \00", align 1
@str.1 = private unnamed_addr constant [5 x i8] c" to \00", align 1
@str.2 = private unnamed_addr constant [3 x i8] c".\0A\00", align 1
@str.3 = private unnamed_addr constant [36 x i8] c"Please, give the number of rings : \00", align 1
@str.4 = private unnamed_addr constant [26 x i8] c"\0AHere is the solution :\0A\0A\00", align 1
@str.5 = private unnamed_addr constant [5 x i8] c"left\00", align 1
@str.6 = private unnamed_addr constant [6 x i8] c"right\00", align 1
@str.7 = private unnamed_addr constant [7 x i8] c"middle\00", align 1

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
  %numberOfRings = alloca i32
  call void @writeString(i8* getelementptr inbounds ([36 x i8], [36 x i8]* @str.3, i32 0, i32 0))
  %0 = call i32 @readInteger()
  store i32 %0, i32* %numberOfRings
  call void @writeString(i8* getelementptr inbounds ([26 x i8], [26 x i8]* @str.4, i32 0, i32 0))
  %1 = load i32, i32* %numberOfRings
  call void @hanoi(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @str.5, i32 0, i32 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @str.6, i32 0, i32 0), i8* getelementptr inbounds ([7 x i8], [7 x i8]* @str.7, i32 0, i32 0), i32 %1)
  ret i32 0
}

define void @hanoi(i8* %0, i8* %1, i8* %2, i32 %3) {
entry:
  %source = alloca i8*
  store i8* %0, i8** %source
  %target = alloca i8*
  store i8* %1, i8** %target
  %auxiliary = alloca i8*
  store i8* %2, i8** %auxiliary
  %rings = alloca i32
  store i32 %3, i32* %rings
  %4 = load i32, i32* %rings
  %5 = icmp sge i32 %4, 1
  %if_cond = icmp ne i1 %5, false
  br i1 %if_cond, label %then, label %else

then:                                             ; preds = %entry
  %6 = load i8*, i8** %source
  %7 = load i8*, i8** %auxiliary
  %8 = load i8*, i8** %target
  %9 = load i32, i32* %rings
  %addtmp = sub i32 %9, 1
  call void @hanoi(i8* %6, i8* %7, i8* %8, i32 %addtmp)
  %10 = load i8*, i8** %source
  %11 = load i8*, i8** %target
  call void @move(i8* %10, i8* %11)
  %12 = load i8*, i8** %auxiliary
  %13 = load i8*, i8** %target
  %14 = load i8*, i8** %source
  %15 = load i32, i32* %rings
  %addtmp1 = sub i32 %15, 1
  call void @hanoi(i8* %12, i8* %13, i8* %14, i32 %addtmp1)
  br label %endif

endif:                                            ; preds = %else, %then
  ret void

else:                                             ; preds = %entry
  br label %endif
}

define void @move(i8* %0, i8* %1) {
entry:
  %source = alloca i8*
  store i8* %0, i8** %source
  %target = alloca i8*
  store i8* %1, i8** %target
  call void @writeString(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @str, i32 0, i32 0))
  %2 = load i8*, i8** %source
  call void @writeString(i8* %2)
  call void @writeString(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @str.1, i32 0, i32 0))
  %3 = load i8*, i8** %target
  call void @writeString(i8* %3)
  call void @writeString(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @str.2, i32 0, i32 0))
  ret void
}
