; ModuleID = '../examples/test.vsop'
source_filename = "../examples/test.vsop"

%struct.ChildVtable = type { %Object* (%Child*, i8*)*, %Object* (%Child*, i1)*, %Object* (%Child*, i32)*, i8* (%Child*)*, i1 (%Child*)*, i32 (%Child*)*, i32 (%Child*)*, i8* (%Child*)*, i32 (%Child*)* }
%Object = type { %struct.ObjectVtable* }
%struct.ObjectVtable = type { %Object* (%Object*, i8*)*, %Object* (%Object*, i1)*, %Object* (%Object*, i32)*, i8* (%Object*)*, i1 (%Object*)*, i32 (%Object*)* }
%Child = type { %struct.ChildVtable*, i32, i32 }
%struct.MainVtable = type { %Object* (%Main*, i8*)*, %Object* (%Main*, i1)*, %Object* (%Main*, i32)*, i8* (%Main*)*, i1 (%Main*)*, i32 (%Main*)*, i32 (%Main*)*, i8* (%Main*)*, i32 (%Main*)* }
%Main = type { %struct.MainVtable*, i32 }
%struct.ParentVtable = type { %Object* (%Parent*, i8*)*, %Object* (%Parent*, i1)*, %Object* (%Parent*, i32)*, i8* (%Parent*)*, i1 (%Parent*)*, i32 (%Parent*)*, i32 (%Parent*)*, i8* (%Parent*)* }
%Parent = type { %struct.ParentVtable*, i32 }

@Child_mtable = internal constant %struct.ChildVtable { %Object* (%Child*, i8*)* @print_Object.1, %Object* (%Child*, i1)* @printBool_Object.2, %Object* (%Child*, i32)* @printInt32_Object.3, i8* (%Child*)* @inputLine_Object.4, i1 (%Child*)* @inputBool_Object.5, i32 (%Child*)* @inputInt32_Object.6, i32 (%Child*)* @p_fun_Child, i8* (%Child*)* @ho_Parent.7, i32 (%Child*)* @c_fun_Child }
@Main_mtable = internal constant %struct.MainVtable { %Object* (%Main*, i8*)* @print_Object.8, %Object* (%Main*, i1)* @printBool_Object.9, %Object* (%Main*, i32)* @printInt32_Object.10, i8* (%Main*)* @inputLine_Object.11, i1 (%Main*)* @inputBool_Object.12, i32 (%Main*)* @inputInt32_Object.13, i32 (%Main*)* @foo_Main, i8* (%Main*)* @test_Main, i32 (%Main*)* @main_Main }
@Object_mtable = internal constant %struct.ObjectVtable { %Object* (%Object*, i8*)* @print_Object, %Object* (%Object*, i1)* @printBool_Object, %Object* (%Object*, i32)* @printInt32_Object, i8* (%Object*)* @inputLine_Object, i1 (%Object*)* @inputBool_Object, i32 (%Object*)* @inputInt32_Object }
@Parent_mtable = internal constant %struct.ParentVtable { %Object* (%Parent*, i8*)* @print_Object.14, %Object* (%Parent*, i1)* @printBool_Object.15, %Object* (%Parent*, i32)* @printInt32_Object.16, i8* (%Parent*)* @inputLine_Object.17, i1 (%Parent*)* @inputBool_Object.18, i32 (%Parent*)* @inputInt32_Object.19, i32 (%Parent*)* @p_fun_Parent, i8* (%Parent*)* @ho_Parent }
@0 = private unnamed_addr constant [4 x i8] c"str\00", align 1
@1 = private unnamed_addr constant [6 x i8] c"rgfdg\00", align 1

declare i8* @malloc(i64)

declare i32 @pow(i32, i32)

define i32 @c_fun_Child(%Child* %self) {
  ret i32 0
}

define i32 @p_fun_Child(%Child* %self) {
  ret i32 1
}

define i32 @foo_Main(%Main* %self) {
  %1 = getelementptr %Main, %Main* %self, i32 0, i32 1
  %2 = load i32, i32* %1, align 4
  ret i32 %2
}

define i8* @test_Main(%Main* %self) {
  ret i8* getelementptr inbounds ([6 x i8], [6 x i8]* @1, i32 0, i32 0)
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

define i32 @p_fun_Parent(%Parent* %self) {
  ret i32 0
}

define i8* @ho_Parent(%Parent* %self) {
  ret i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0)
}

declare %Object* @print_Object.1(%Child*, i8*)

declare %Object* @printBool_Object.2(%Child*, i1)

declare %Object* @printInt32_Object.3(%Child*, i32)

declare i8* @inputLine_Object.4(%Child*)

declare i1 @inputBool_Object.5(%Child*)

declare i32 @inputInt32_Object.6(%Child*)

declare i8* @ho_Parent.7(%Child*)

define %Child* @Child..new() {
entry:
  %0 = call i8* @malloc(i64 ptrtoint (%Child* getelementptr (%Child, %Child* null, i32 1) to i64))
  %1 = bitcast i8* %0 to %Child*
  %2 = call %Child* @Child..init(%Child* %1)
  ret %Child* %2
}

define %Child* @Child..init(%Child* %self) {
entry:
  %0 = getelementptr %Child, %Child* %self, i32 0, i32 0
  store %struct.ChildVtable* @Child_mtable, %struct.ChildVtable** %0, align 8
  ret %Child* %self
}

declare %Object* @print_Object.8(%Main*, i8*)

declare %Object* @printBool_Object.9(%Main*, i1)

declare %Object* @printInt32_Object.10(%Main*, i32)

declare i8* @inputLine_Object.11(%Main*)

declare i1 @inputBool_Object.12(%Main*)

declare i32 @inputInt32_Object.13(%Main*)

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

declare %Object* @print_Object.14(%Parent*, i8*)

declare %Object* @printBool_Object.15(%Parent*, i1)

declare %Object* @printInt32_Object.16(%Parent*, i32)

declare i8* @inputLine_Object.17(%Parent*)

declare i1 @inputBool_Object.18(%Parent*)

declare i32 @inputInt32_Object.19(%Parent*)

define %Parent* @Parent..new() {
entry:
  %0 = call i8* @malloc(i64 ptrtoint (%Parent* getelementptr (%Parent, %Parent* null, i32 1) to i64))
  %1 = bitcast i8* %0 to %Parent*
  %2 = call %Parent* @Parent..init(%Parent* %1)
  ret %Parent* %2
}

define %Parent* @Parent..init(%Parent* %self) {
entry:
  %0 = getelementptr %Parent, %Parent* %self, i32 0, i32 0
  store %struct.ParentVtable* @Parent_mtable, %struct.ParentVtable** %0, align 8
  ret %Parent* %self
}
