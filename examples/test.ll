; ModuleID = '../examples/test.vsop'
source_filename = "../examples/test.vsop"

%struct.MainVtable = type { %Object* (%Object*, i8*)*, %Object* (%Object*, i1)*, %Object* (%Object*, i32)*, i8* (%Object*)*, i1 (%Object*)*, i32 (%Object*)*, i32 (%Main*, i32, i32)*, i32 (%Main*)* }
%Object = type { %struct.ObjectVtable* }
%struct.ObjectVtable = type { %Object* (%Object*, i8*)*, %Object* (%Object*, i1)*, %Object* (%Object*, i32)*, i8* (%Object*)*, i1 (%Object*)*, i32 (%Object*)* }
%Main = type { %struct.MainVtable* }

@Main_mtable = internal constant %struct.MainVtable { %Object* (%Object*, i8*)* @Object__print, %Object* (%Object*, i1)* @Object__printBool, %Object* (%Object*, i32)* @Object__printInt32, i8* (%Object*)* @Object__inputLine, i1 (%Object*)* @Object__inputBool, i32 (%Object*)* @Object__inputInt32, i32 (%Main*, i32, i32)* @Main__pow, i32 (%Main*)* @Main__main }
@0 = private unnamed_addr constant [5 x i8] c"test\00", align 1

declare i8* @malloc(i64)

declare i32 @pow(i32, i32)

define i32 @Main__pow(%Main* %self, i32 %x, i32 %n) {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 1, i32* %3, align 4
  %4 = alloca i32, align 4
  store i32 0, i32* %4, align 4
  br label %5

5:                                                ; preds = %9, %0
  %6 = load i32, i32* %4, align 4
  %7 = load i32, i32* %2, align 4
  %8 = icmp slt i32 %6, %7
  br i1 %8, label %9, label %15

9:                                                ; preds = %5
  %10 = load i32, i32* %3, align 4
  %11 = load i32, i32* %1, align 4
  %12 = add i32 %10, %11
  store i32 %12, i32* %3, align 4
  %13 = load i32, i32* %4, align 4
  %14 = add i32 %13, 1
  store i32 %14, i32* %4, align 4
  br label %5

15:                                               ; preds = %5
  %16 = load i32, i32* %3, align 4
  ret i32 %16
}

define i32 @Main__main(%Main* %self) {
  %1 = getelementptr %Main, %Main* %self, i32 0, i32 0
  %2 = load %struct.MainVtable*, %struct.MainVtable** %1, align 8
  %3 = getelementptr %struct.MainVtable, %struct.MainVtable* %2, i32 0, i32 0
  %4 = load %Object* (%Object*, i8*)*, %Object* (%Object*, i8*)** %3, align 8
  %5 = bitcast %Main* %self to %Object*
  %6 = call %Object* %4(%Object* %5, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @0, i32 0, i32 0))
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
