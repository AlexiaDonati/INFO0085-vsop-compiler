; ModuleID = '../examples/factorial.vsop'
source_filename = "../examples/factorial.vsop"

%struct.MainVtable = type { %Object* (%Object*, i8*)*, %Object* (%Object*, i1)*, %Object* (%Object*, i32)*, i8* (%Object*)*, i1 (%Object*)*, i32 (%Object*)*, i32 (%Main*, i32)*, i32 (%Main*)* }
%Object = type { %struct.ObjectVtable* }
%struct.ObjectVtable = type { %Object* (%Object*, i8*)*, %Object* (%Object*, i1)*, %Object* (%Object*, i32)*, i8* (%Object*)*, i1 (%Object*)*, i32 (%Object*)* }
%Main = type { %struct.MainVtable* }

@Main_mtable = internal constant %struct.MainVtable { %Object* (%Object*, i8*)* @Object__print, %Object* (%Object*, i1)* @Object__printBool, %Object* (%Object*, i32)* @Object__printInt32, i8* (%Object*)* @Object__inputLine, i1 (%Object*)* @Object__inputBool, i32 (%Object*)* @Object__inputInt32, i32 (%Main*, i32)* @Main__factorial, i32 (%Main*)* @Main__main }
@0 = private unnamed_addr constant [46 x i8] c"Enter an integer greater-than or equal to 0: \00", align 1
@1 = private unnamed_addr constant [54 x i8] c"Error: number must be greater-than or equal to 0.\\x0a\00", align 1
@2 = private unnamed_addr constant [18 x i8] c"The factorial of \00", align 1
@3 = private unnamed_addr constant [5 x i8] c" is \00", align 1
@4 = private unnamed_addr constant [5 x i8] c"\\x0a\00", align 1

declare i8* @malloc(i64)

declare i32 @pow(i32, i32)

define i32 @Main__factorial(%Main* %self, i32 %n) {
  %1 = alloca i32, align 4
  store i32 %n, i32* %1, align 4
  br label %2

2:                                                ; preds = %0
  %3 = load i32, i32* %1, align 4
  %4 = icmp slt i32 %3, 2
  br i1 %4, label %7, label %8

5:                                                ; preds = %8, %7
  %6 = phi i32 [ 1, %7 ], [ %17, %8 ]
  ret i32 %6

7:                                                ; preds = %2
  br label %5

8:                                                ; preds = %2
  %9 = load i32, i32* %1, align 4
  %10 = load i32, i32* %1, align 4
  %11 = sub i32 %10, 1
  %12 = getelementptr %Main, %Main* %self, i32 0, i32 0
  %13 = load %struct.MainVtable*, %struct.MainVtable** %12, align 8
  %14 = getelementptr %struct.MainVtable, %struct.MainVtable* %13, i32 0, i32 6
  %15 = load i32 (%Main*, i32)*, i32 (%Main*, i32)** %14, align 8
  %16 = call i32 %15(%Main* %self, i32 %11)
  %17 = mul i32 %9, %16
  br label %5
}

define i32 @Main__main(%Main* %self) {
  %1 = getelementptr %Main, %Main* %self, i32 0, i32 0
  %2 = load %struct.MainVtable*, %struct.MainVtable** %1, align 8
  %3 = getelementptr %struct.MainVtable, %struct.MainVtable* %2, i32 0, i32 0
  %4 = load %Object* (%Object*, i8*)*, %Object* (%Object*, i8*)** %3, align 8
  %5 = bitcast %Main* %self to %Object*
  %6 = call %Object* %4(%Object* %5, i8* getelementptr inbounds ([46 x i8], [46 x i8]* @0, i32 0, i32 0))
  %7 = alloca i32, align 4
  %8 = getelementptr %Main, %Main* %self, i32 0, i32 0
  %9 = load %struct.MainVtable*, %struct.MainVtable** %8, align 8
  %10 = getelementptr %struct.MainVtable, %struct.MainVtable* %9, i32 0, i32 5
  %11 = load i32 (%Object*)*, i32 (%Object*)** %10, align 8
  %12 = bitcast %Main* %self to %Object*
  %13 = call i32 %11(%Object* %12)
  store i32 %13, i32* %7, align 4
  br label %14

14:                                               ; preds = %0
  %15 = load i32, i32* %7, align 4
  %16 = icmp slt i32 %15, 0
  br i1 %16, label %19, label %26

17:                                               ; preds = %26, %19
  %18 = phi i32 [ -1, %19 ], [ 0, %26 ]
  ret i32 %18

19:                                               ; preds = %14
  %20 = getelementptr %Main, %Main* %self, i32 0, i32 0
  %21 = load %struct.MainVtable*, %struct.MainVtable** %20, align 8
  %22 = getelementptr %struct.MainVtable, %struct.MainVtable* %21, i32 0, i32 0
  %23 = load %Object* (%Object*, i8*)*, %Object* (%Object*, i8*)** %22, align 8
  %24 = bitcast %Main* %self to %Object*
  %25 = call %Object* %23(%Object* %24, i8* getelementptr inbounds ([54 x i8], [54 x i8]* @1, i32 0, i32 0))
  br label %17

26:                                               ; preds = %14
  %27 = getelementptr %Main, %Main* %self, i32 0, i32 0
  %28 = load %struct.MainVtable*, %struct.MainVtable** %27, align 8
  %29 = getelementptr %struct.MainVtable, %struct.MainVtable* %28, i32 0, i32 0
  %30 = load %Object* (%Object*, i8*)*, %Object* (%Object*, i8*)** %29, align 8
  %31 = bitcast %Main* %self to %Object*
  %32 = call %Object* %30(%Object* %31, i8* getelementptr inbounds ([18 x i8], [18 x i8]* @2, i32 0, i32 0))
  %33 = load i32, i32* %7, align 4
  %34 = getelementptr %Object, %Object* %32, i32 0, i32 0
  %35 = load %struct.ObjectVtable*, %struct.ObjectVtable** %34, align 8
  %36 = getelementptr %struct.ObjectVtable, %struct.ObjectVtable* %35, i32 0, i32 2
  %37 = load %Object* (%Object*, i32)*, %Object* (%Object*, i32)** %36, align 8
  %38 = call %Object* %37(%Object* %32, i32 %33)
  %39 = getelementptr %Object, %Object* %38, i32 0, i32 0
  %40 = load %struct.ObjectVtable*, %struct.ObjectVtable** %39, align 8
  %41 = getelementptr %struct.ObjectVtable, %struct.ObjectVtable* %40, i32 0, i32 0
  %42 = load %Object* (%Object*, i8*)*, %Object* (%Object*, i8*)** %41, align 8
  %43 = call %Object* %42(%Object* %38, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @3, i32 0, i32 0))
  %44 = load i32, i32* %7, align 4
  %45 = getelementptr %Main, %Main* %self, i32 0, i32 0
  %46 = load %struct.MainVtable*, %struct.MainVtable** %45, align 8
  %47 = getelementptr %struct.MainVtable, %struct.MainVtable* %46, i32 0, i32 6
  %48 = load i32 (%Main*, i32)*, i32 (%Main*, i32)** %47, align 8
  %49 = call i32 %48(%Main* %self, i32 %44)
  %50 = getelementptr %Main, %Main* %self, i32 0, i32 0
  %51 = load %struct.MainVtable*, %struct.MainVtable** %50, align 8
  %52 = getelementptr %struct.MainVtable, %struct.MainVtable* %51, i32 0, i32 2
  %53 = load %Object* (%Object*, i32)*, %Object* (%Object*, i32)** %52, align 8
  %54 = bitcast %Main* %self to %Object*
  %55 = call %Object* %53(%Object* %54, i32 %49)
  %56 = getelementptr %Object, %Object* %55, i32 0, i32 0
  %57 = load %struct.ObjectVtable*, %struct.ObjectVtable** %56, align 8
  %58 = getelementptr %struct.ObjectVtable, %struct.ObjectVtable* %57, i32 0, i32 0
  %59 = load %Object* (%Object*, i8*)*, %Object* (%Object*, i8*)** %58, align 8
  %60 = call %Object* %59(%Object* %55, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @4, i32 0, i32 0))
  br label %17
}

declare %Object* @Object__print(%Object*, i8*)

declare %Object* @Object__printBool(%Object*, i1)

declare %Object* @Object__printInt32(%Object*, i32)

declare i8* @Object__inputLine(%Object*)

declare i1 @Object__inputBool(%Object*)

declare i32 @Object__inputInt32(%Object*)

define %Main* @Main___new() {
entry:
  %0 = call i8* @malloc(i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64))
  %1 = bitcast i8* %0 to %Main*
  %2 = call %Main* @Main___init(%Main* %1)
  ret %Main* %2
}

define %Main* @Main___init(%Main* %self) {
entry:
  %0 = getelementptr %Main, %Main* %self, i32 0, i32 0
  store %struct.MainVtable* @Main_mtable, %struct.MainVtable** %0, align 8
  ret %Main* %self
}

declare %Object* @Object___new()

declare %Object* @Object___init(%Object*)

define i32 @main() {
entry:
  %0 = call %Main* @Main___new()
  %1 = call i32 @Main__main(%Main* %0)
  ret i32 %1
}
