; ModuleID = '../examples/test.vsop'
source_filename = "../examples/test.vsop"

%struct.MainVtable = type { %Object* (%Object*, i8*)*, %Object* (%Object*, i1)*, %Object* (%Object*, i32)*, i8* (%Object*)*, i1 (%Object*)*, i32 (%Object*)*, i32 (%Main*)* }
%Object = type { %struct.ObjectVtable* }
%struct.ObjectVtable = type { %Object* (%Object*, i8*)*, %Object* (%Object*, i1)*, %Object* (%Object*, i32)*, i8* (%Object*)*, i1 (%Object*)*, i32 (%Object*)* }
%Main = type { %struct.MainVtable* }

@Main_mtable = internal constant %struct.MainVtable { %Object* (%Object*, i8*)* @Object__print, %Object* (%Object*, i1)* @Object__printBool, %Object* (%Object*, i32)* @Object__printInt32, i8* (%Object*)* @Object__inputLine, i1 (%Object*)* @Object__inputBool, i32 (%Object*)* @Object__inputInt32, i32 (%Main*)* @Main__main }
@0 = private unnamed_addr constant [5 x i8] c" -> \00", align 1
@1 = private unnamed_addr constant [7 x i8] c"\22\0A\0A\22\0A\\\00", align 1

declare i8* @malloc(i64)

declare i32 @pow(i32, i32)

define i32 @Main__main(%Main* %self) {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 2, i32* %1, align 4
  store i32 0, i32* %2, align 4
  br label %3

3:                                                ; preds = %6, %0
  %4 = load i32, i32* %2, align 4
  %5 = icmp slt i32 %4, 10
  br i1 %5, label %6, label %24

6:                                                ; preds = %3
  %7 = load i32, i32* %1, align 4
  %8 = load i32, i32* %2, align 4
  %9 = call i32 @pow(i32 %7, i32 %8)
  %10 = getelementptr %Main, %Main* %self, i32 0, i32 0
  %11 = load %struct.MainVtable*, %struct.MainVtable** %10, align 8
  %12 = getelementptr %struct.MainVtable, %struct.MainVtable* %11, i32 0, i32 2
  %13 = load %Object* (%Object*, i32)*, %Object* (%Object*, i32)** %12, align 8
  %14 = bitcast %Main* %self to %Object*
  %15 = call %Object* %13(%Object* %14, i32 %9)
  %16 = getelementptr %Main, %Main* %self, i32 0, i32 0
  %17 = load %struct.MainVtable*, %struct.MainVtable** %16, align 8
  %18 = getelementptr %struct.MainVtable, %struct.MainVtable* %17, i32 0, i32 0
  %19 = load %Object* (%Object*, i8*)*, %Object* (%Object*, i8*)** %18, align 8
  %20 = bitcast %Main* %self to %Object*
  %21 = call %Object* %19(%Object* %20, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @0, i32 0, i32 0))
  %22 = load i32, i32* %2, align 4
  %23 = add i32 %22, 1
  store i32 %23, i32* %2, align 4
  br label %3

24:                                               ; preds = %3
  %25 = getelementptr %Main, %Main* %self, i32 0, i32 0
  %26 = load %struct.MainVtable*, %struct.MainVtable** %25, align 8
  %27 = getelementptr %struct.MainVtable, %struct.MainVtable* %26, i32 0, i32 0
  %28 = load %Object* (%Object*, i8*)*, %Object* (%Object*, i8*)** %27, align 8
  %29 = bitcast %Main* %self to %Object*
  %30 = call %Object* %28(%Object* %29, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @1, i32 0, i32 0))
  ret i32 0
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
