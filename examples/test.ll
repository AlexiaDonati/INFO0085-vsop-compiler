; ModuleID = '../examples/test.vsop'
source_filename = "../examples/test.vsop"

%struct.ChildVtable = type { %Object* (%Object*, i8*)*, %Object* (%Object*, i1)*, %Object* (%Object*, i32)*, i8* (%Object*)*, i1 (%Object*)*, i32 (%Object*)*, i32 (%Parent*)*, i8* (%Parent*)*, i32 (%Child*)* }
%Object = type { %struct.ObjectVtable* }
%struct.ObjectVtable = type { %Object* (%Object*, i8*)*, %Object* (%Object*, i1)*, %Object* (%Object*, i32)*, i8* (%Object*)*, i1 (%Object*)*, i32 (%Object*)* }
%Parent = type { %struct.ParentVtable*, i32 }
%struct.ParentVtable = type { %Object* (%Object*, i8*)*, %Object* (%Object*, i1)*, %Object* (%Object*, i32)*, i8* (%Object*)*, i1 (%Object*)*, i32 (%Object*)*, i32 (%Parent*)*, i8* (%Parent*)* }
%Child = type { %struct.ChildVtable*, i32, i32 }
%struct.MainVtable = type { %Object* (%Object*, i8*)*, %Object* (%Object*, i1)*, %Object* (%Object*, i32)*, i8* (%Object*)*, i1 (%Object*)*, i32 (%Object*)*, i32 (%Main*)*, i8* (%Main*)*, i32 (%Main*, i32, i32)*, i32 (%Main*)* }
%Main = type { %struct.MainVtable*, i32 }

@Child_mtable = internal constant %struct.ChildVtable { %Object* (%Object*, i8*)* @Object__print, %Object* (%Object*, i1)* @Object__printBool, %Object* (%Object*, i32)* @Object__printInt32, i8* (%Object*)* @Object__inputLine, i1 (%Object*)* @Object__inputBool, i32 (%Object*)* @Object__inputInt32, i32 (%Parent*)* @Parent__p_fun, i8* (%Parent*)* @Parent__ho, i32 (%Child*)* @Child__c_fun }
@Main_mtable = internal constant %struct.MainVtable { %Object* (%Object*, i8*)* @Object__print, %Object* (%Object*, i1)* @Object__printBool, %Object* (%Object*, i32)* @Object__printInt32, i8* (%Object*)* @Object__inputLine, i1 (%Object*)* @Object__inputBool, i32 (%Object*)* @Object__inputInt32, i32 (%Main*)* @Main__foo, i8* (%Main*)* @Main__test, i32 (%Main*, i32, i32)* @Main__pow, i32 (%Main*)* @Main__main }
@Parent_mtable = internal constant %struct.ParentVtable { %Object* (%Object*, i8*)* @Object__print, %Object* (%Object*, i1)* @Object__printBool, %Object* (%Object*, i32)* @Object__printInt32, i8* (%Object*)* @Object__inputLine, i1 (%Object*)* @Object__inputBool, i32 (%Object*)* @Object__inputInt32, i32 (%Parent*)* @Parent__p_fun, i8* (%Parent*)* @Parent__ho }
@0 = private unnamed_addr constant [4 x i8] c"str\00", align 1
@1 = private unnamed_addr constant [6 x i8] c"rgfdg\00", align 1

declare i8* @malloc(i64)

declare i32 @pow(i32, i32)

define i32 @Child__c_fun(%Child* %self) {
  ret i32 0
}

define i32 @Child__p_fun(%Child* %self) {
  ret i32 1
}

define i32 @Main__foo(%Main* %self) {
  %1 = getelementptr %Main, %Main* %self, i32 0, i32 1
  %2 = load i32, i32* %1, align 4
  ret i32 %2
}

define i8* @Main__test(%Main* %self) {
  ret i8* getelementptr inbounds ([6 x i8], [6 x i8]* @1, i32 0, i32 0)
}

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
  %18 = phi i32 [ 1, %19 ], [ 0, %21 ]
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

declare %Object* @Object__print(%Object*, i8*)

declare %Object* @Object__printBool(%Object*, i1)

declare %Object* @Object__printInt32(%Object*, i32)

declare i8* @Object__inputLine(%Object*)

declare i1 @Object__inputBool(%Object*)

declare i32 @Object__inputInt32(%Object*)

define i32 @Parent__p_fun(%Parent* %self) {
  ret i32 0
}

define i8* @Parent__ho(%Parent* %self) {
  ret i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0)
}

define %Child* @Child___new() {
entry:
  %0 = call i8* @malloc(i64 ptrtoint (%Child* getelementptr (%Child, %Child* null, i32 1) to i64))
  %1 = bitcast i8* %0 to %Child*
  %2 = call %Child* @Child___init(%Child* %1)
  ret %Child* %2
}

define %Child* @Child___init(%Child* %self) {
entry:
  %0 = getelementptr %Child, %Child* %self, i32 0, i32 0
  store %struct.ChildVtable* @Child_mtable, %struct.ChildVtable** %0, align 8
  ret %Child* %self
}

define %Main* @Main___new() {
entry:
  %0 = call i8* @malloc(i64 ptrtoint (%Main* getelementptr (%Main, %Main* null, i32 1) to i64))
  %1 = bitcast i8* %0 to %Main*
  %2 = call %Main* @Main___init(%Main* %1)
  ret %Main* %2
}

define %Main* @Main___init(%Main* %self) {
entry:
  %0 = getelementptr %Main, %Main* %self, i32 0, i32 0
  store %struct.MainVtable* @Main_mtable, %struct.MainVtable** %0, align 8
  %1 = getelementptr %Main, %Main* %self, i32 0, i32 1
  store i32 2, i32* %1, align 4
  ret %Main* %self
}

declare %Object* @Object___new()

declare %Object* @Object___init(%Object*)

define %Parent* @Parent___new() {
entry:
  %0 = call i8* @malloc(i64 ptrtoint (%Parent* getelementptr (%Parent, %Parent* null, i32 1) to i64))
  %1 = bitcast i8* %0 to %Parent*
  %2 = call %Parent* @Parent___init(%Parent* %1)
  ret %Parent* %2
}

define %Parent* @Parent___init(%Parent* %self) {
entry:
  %0 = getelementptr %Parent, %Parent* %self, i32 0, i32 0
  store %struct.ParentVtable* @Parent_mtable, %struct.ParentVtable** %0, align 8
  ret %Parent* %self
}

define i32 @main() {
entry:
  %0 = call %Main* @Main___new()
  %1 = call i32 @Main__main(%Main* %0)
  ret i32 %1
}
