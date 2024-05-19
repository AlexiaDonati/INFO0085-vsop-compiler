; ModuleID = '../examples/linked-list.vsop'
source_filename = "../examples/linked-list.vsop"

%struct.ConsVtable = type { %Object* (%Object*, i8*)*, %Object* (%Object*, i1)*, %Object* (%Object*, i32)*, i8* (%Object*)*, i1 (%Object*)*, i32 (%Object*)*, i32 (%Cons*, %Cons*)* }
%Object = type { %struct.ObjectVtable* }
%struct.ObjectVtable = type { %Object* (%Object*, i8*)*, %Object* (%Object*, i1)*, %Object* (%Object*, i32)*, i8* (%Object*)*, i1 (%Object*)*, i32 (%Object*)* }
%Cons = type { %struct.ConsVtable* }
%struct.ListVtable = type { %Object* (%Object*, i8*)*, %Object* (%Object*, i1)*, %Object* (%Object*, i32)*, i8* (%Object*)*, i1 (%Object*)*, i32 (%Object*)* }
%struct.MainVtable = type { %Object* (%Object*, i8*)*, %Object* (%Object*, i1)*, %Object* (%Object*, i32)*, i8* (%Object*)*, i1 (%Object*)*, i32 (%Object*)*, i32 (%Main*)* }
%Main = type { %struct.MainVtable* }
%List = type { %struct.ListVtable* }

@Cons_mtable = internal constant %struct.ConsVtable { %Object* (%Object*, i8*)* @Object__print, %Object* (%Object*, i1)* @Object__printBool, %Object* (%Object*, i32)* @Object__printInt32, i8* (%Object*)* @Object__inputLine, i1 (%Object*)* @Object__inputBool, i32 (%Object*)* @Object__inputInt32, i32 (%Cons*, %Cons*)* @Cons__test }
@List_mtable = internal constant %struct.ListVtable { %Object* (%Object*, i8*)* @Object__print, %Object* (%Object*, i1)* @Object__printBool, %Object* (%Object*, i32)* @Object__printInt32, i8* (%Object*)* @Object__inputLine, i1 (%Object*)* @Object__inputBool, i32 (%Object*)* @Object__inputInt32 }
@Main_mtable = internal constant %struct.MainVtable { %Object* (%Object*, i8*)* @Object__print, %Object* (%Object*, i1)* @Object__printBool, %Object* (%Object*, i32)* @Object__printInt32, i8* (%Object*)* @Object__inputLine, i1 (%Object*)* @Object__inputBool, i32 (%Object*)* @Object__inputInt32, i32 (%Main*)* @Main__main }

declare i8* @malloc(i64)

declare i32 @pow(i32, i32)

define i32 @Cons__test(%Cons* %self, %Cons* %tl) {
  ret i32 0
}

define i32 @Main__main(%Main* %self) {
  ret i32 0
}

declare %Object* @Object__print(%Object*, i8*)

declare %Object* @Object__printBool(%Object*, i1)

declare %Object* @Object__printInt32(%Object*, i32)

declare i8* @Object__inputLine(%Object*)

declare i1 @Object__inputBool(%Object*)

declare i32 @Object__inputInt32(%Object*)

define %Cons* @Cons___new() {
entry:
  %0 = call i8* @malloc(i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64))
  %1 = bitcast i8* %0 to %Cons*
  %2 = call %Cons* @Cons___init(%Cons* %1)
  ret %Cons* %2
}

define %Cons* @Cons___init(%Cons* %self) {
entry:
  %0 = getelementptr %Cons, %Cons* %self, i32 0, i32 0
  store %struct.ConsVtable* @Cons_mtable, %struct.ConsVtable** %0, align 8
  ret %Cons* %self
}

define %List* @List___new() {
entry:
  %0 = call i8* @malloc(i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64))
  %1 = bitcast i8* %0 to %List*
  %2 = call %List* @List___init(%List* %1)
  ret %List* %2
}

define %List* @List___init(%List* %self) {
entry:
  %0 = getelementptr %List, %List* %self, i32 0, i32 0
  store %struct.ListVtable* @List_mtable, %struct.ListVtable** %0, align 8
  ret %List* %self
}

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
