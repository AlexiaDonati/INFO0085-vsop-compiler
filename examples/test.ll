; ModuleID = '../examples/test.vsop'
source_filename = "../examples/test.vsop"

%struct.MainVtable = type { i32 (%Main*)*, i8* (%Main*)*, i32 (%Main*)*, %Object* (%Object*, i8*)*, %Object* (%Object*, i1)*, %Object* (%Object*, i32)*, i8* (%Object*)*, i1 (%Object*)*, i32 (%Object*)* }
%Main = type { %struct.MainVtable*, i32 }
%Object = type { %struct.ObjectVtable* }
%struct.ObjectVtable = type { %Object* (%Object*, i8*)*, %Object* (%Object*, i1)*, %Object* (%Object*, i32)*, i8* (%Object*)*, i1 (%Object*)*, i32 (%Object*)* }

@Main_mtable = internal constant %struct.MainVtable { i32 (%Main*)* @foo_Main, i8* (%Main*)* @test_Main, i32 (%Main*)* @main_Main, %Object* (%Object*, i8*)* @print_Object, %Object* (%Object*, i1)* @printBool_Object, %Object* (%Object*, i32)* @printInt32_Object, i8* (%Object*)* @inputLine_Object, i1 (%Object*)* @inputBool_Object, i32 (%Object*)* @inputInt32_Object }
@Object_mtable = internal constant %struct.ObjectVtable { %Object* (%Object*, i8*)* @print_Object, %Object* (%Object*, i1)* @printBool_Object, %Object* (%Object*, i32)* @printInt32_Object, i8* (%Object*)* @inputLine_Object, i1 (%Object*)* @inputBool_Object, i32 (%Object*)* @inputInt32_Object }
@0 = private unnamed_addr constant [6 x i8] c"rgfdg\00", align 1

declare i8* @malloc(i64)

declare i32 @pow(i32, i32)

define i32 @foo_Main(%Main* %self) {
  %1 = getelementptr %Main, %Main* %self, i32 0, i32 1
  %2 = load i32, i32* %1, align 4
  ret i32 %2
}

define i8* @test_Main(%Main* %self) {
  ret i8* getelementptr inbounds ([6 x i8], [6 x i8]* @0, i32 0, i32 0)
}

define i32 @main_Main(%Main* %self) {
  %1 = getelementptr %Main, %Main* %self, i32 0, i32 1
  store i32 2, i32* %1, align 4
  br label %2

2:                                                ; preds = %6, %0
  %3 = getelementptr %Main, %Main* %self, i32 0, i32 1
  %4 = load i32, i32* %3, align 4
  %5 = icmp eq i32 %4, 2
  br i1 %5, label %6, label %11

6:                                                ; preds = %2
  %7 = getelementptr %Main, %Main* %self, i32 0, i32 1
  %8 = load i32, i32* %7, align 4
  %9 = add i32 %8, 1
  %10 = getelementptr %Main, %Main* %self, i32 0, i32 1
  store i32 %9, i32* %10, align 4
  br label %2

11:                                               ; preds = %2
  br label %12

12:                                               ; preds = %11
  %13 = getelementptr %Main, %Main* %self, i32 0, i32 1
  %14 = load i32, i32* %13, align 4
  %15 = icmp eq i32 %14, 2
  %16 = and i1 true, %15
  br i1 %16, label %19, label %21

17:                                               ; preds = %21, %19
  %18 = phi i32* [ %20, %19 ], [ %22, %21 ]
  ret i32 0

19:                                               ; preds = %12
  %20 = getelementptr %Main, %Main* %self, i32 0, i32 1
  store i32 1, i32* %20, align 4
  br label %17

21:                                               ; preds = %12
  %22 = getelementptr %Main, %Main* %self, i32 0, i32 1
  store i32 0, i32* %22, align 4
  br label %17
}

declare %Object* @print_Object(%Object*, i8*)

declare %Object* @printBool_Object(%Object*, i1)

declare %Object* @printInt32_Object(%Object*, i32)

declare i8* @inputLine_Object(%Object*)

declare i1 @inputBool_Object(%Object*)

declare i32 @inputInt32_Object(%Object*)

define %Main* @Main..new() {
entry:
  %0 = call i8* @malloc(i64 ptrtoint (%Main* getelementptr (%Main, %Main* null, i32 1) to i64))
  %1 = bitcast i8* %0 to %Main*
  %2 = call %Main* @Main..init(%Main* %1)
  ret %Main* %2
}

define %Main* @Main..init(%Main* %self) {
entry:
  %0 = getelementptr %Main, %Main* %self, i32 0, i32 0
  store %struct.MainVtable* @Main_mtable, %struct.MainVtable** %0, align 8
  %1 = getelementptr %Main, %Main* %self, i32 0, i32 1
  store i32 2, i32* %1, align 4
  ret %Main* %self
}

define %Object* @Object..new() {
entry:
  %0 = call i8* @malloc(i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64))
  %1 = bitcast i8* %0 to %Object*
  %2 = call %Object* @Object..init(%Object* %1)
  ret %Object* %2
}

define %Object* @Object..init(%Object* %self) {
entry:
  %0 = getelementptr %Object, %Object* %self, i32 0, i32 0
  store %struct.ObjectVtable* @Object_mtable, %struct.ObjectVtable** %0, align 8
  ret %Object* %self
}
