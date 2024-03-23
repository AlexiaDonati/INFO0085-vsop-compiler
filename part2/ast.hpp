#ifndef _AST_HPP
#define _AST_HPP

#include <string>
#include <vector>
#include <algorithm>

#define TO_VALUE(void_pointer) this->get_value_from_void(void_pointer)
#define TO_VOID(value) this->get_void_from_value(value)
#define ACCEPT(expr) TO_VALUE(expr->accept(this))
#define ACCEPT_LIST(list) this->accept_list(list)

namespace AST
{
    class Expr;
    template <typename T>
    class List;
    class Program;
    class Class;
    class Field;
    class Method;
    class Formal;
    class Block;
    class If;
    class While;
    class Let;
    class Assign;
    class Self;
    class Unop;
    class Binop;
    class Call;
    class New;
    class String;
    class Integer;
    class Boolean;
    class Unit;
    class Object;

    // Visitor

    class Visitor {
        public:
            virtual void* visit(Program* program) = 0;
            virtual void* visit(Class* class_) = 0;
            virtual void* visit(Field* field) = 0;
            virtual void* visit(Method* method) = 0;
            virtual void* visit(Formal* formal) = 0;
            virtual void* visit(Block* block) = 0;
            virtual void* visit(If* if_) = 0;
            virtual void* visit(While* while_) = 0;
            virtual void* visit(Let* let) = 0;
            virtual void* visit(Assign* assign) = 0;
            virtual void* visit(Self* self) = 0;
            virtual void* visit(Unop* unop) = 0;
            virtual void* visit(Binop* binop) = 0;
            virtual void* visit(Call* call) = 0;
            virtual void* visit(New* new_) = 0;
            virtual void* visit(String* string_) = 0;
            virtual void* visit(Integer* integer) = 0;
            virtual void* visit(Boolean* boolean) = 0;
            virtual void* visit(Unit* unit) = 0;
            virtual void* visit(Object* object) = 0;

            /*==================================\
            | Cannot make virtual template.     |
            | So, these are not specified here, |
            | but must be done in children!     |
            \==================================*/
            // virtual T get_value_from_void(void* void_value) = 0
            // virtual void* get_void_from_value(T value) = 0
            // virtual E accept_list(List<T>* list) = 0
    };

    class Print_visitor : public Visitor {
        public:
            void* visit(Program* program);
            void* visit(Class* class_);
            void* visit(Field* field);
            void* visit(Method* method);
            void* visit(Formal* formal);
            void* visit(Block* block);
            void* visit(If* if_);
            void* visit(While* while_);
            void* visit(Let* let);
            void* visit(Assign* assign);
            void* visit(Self* self);
            void* visit(Unop* unop);
            void* visit(Binop* binop);
            void* visit(Call* call);
            void* visit(New* new_);
            void* visit(String* string_);
            void* visit(Integer* integer);
            void* visit(Boolean* boolean);
            void* visit(Unit* unit);
            void* visit(Object* object);

            std::string get_value_from_void(void* void_value){
                // Specifiate the type of the value
                std::string* value = (std::string*) void_value;

                // Give this value to a non pointer variable
                std::string return_value = *value;

                // Free the pointer
                delete value;

                // Return the value as non pointer
                return return_value;
            }   

            void* get_void_from_value(std::string value){
                return new std::string(value);
            }

            template <typename T>
            std::string accept_list(List<T>* list){
                size_t size = list->get_size();
                std::string result = "[";

                for (size_t i = 0; i < size; i++){
                    std::string expr_result = this->get_value_from_void(list->accept_one(this, i));
                    result += expr_result;
                    result += (i+1 == size) ? "]" : ", ";
                }

                if(size == 0){

                    result += "]";
                }

                return result;
            }    
    };

    // Data classes

    class Expr {
        public:
            Expr(int line, int column, std::string file_name) : 
                line(line), column(column), file_name(file_name) {};
            virtual ~Expr() = default;

            virtual void* accept(Visitor* visitor) = 0;

            int get_line() { return line; }
            int get_column() { return column; }
            std::string get_file_name() { return file_name; }
        private:
            int line;
            int column;
            std::string file_name;
    };

    template <typename T>
    class List {
        public:
            List() = default;
            ~List() {
                for (auto expr : list)
                    delete expr;
            };

            void* accept_one(Visitor* visitor, size_t index) {
                return list[index]->accept(visitor);
            }

            size_t get_size() { return list.size(); }

            void add(T* expr) { list.push_back(expr);};
            void reverse() { std::reverse(list.begin(), list.end()); };

            unsigned long size() { return list.size();};
        private:
            std::vector<T*> list;
    };

    class String : public Expr {
        public:
            String(int line, int column, std::string file_name, std::string value) : 
                Expr(line, column, file_name), value(value) {};
            ~String() = default;

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            std::string get_value() { return value; }
        private:
            std::string value;
    };

    class Integer : public Expr {
        public:
            Integer(int line, int column, std::string file_name, int value) : 
                Expr(line, column, file_name), value(value) {};
            ~Integer() = default;

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            int get_value() { return value; }
        private:
            int value;
    };

    class Boolean : public Expr {
        public:
            Boolean(int line, int column, std::string file_name, bool value) : 
                Expr(line, column, file_name), value(value) {};
            ~Boolean() = default;

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            bool get_value() { return value; }
        private:
            bool value;
    };


    class Unit : public Expr {
        public:
            Unit(int line, int column, std::string file_name) : 
                Expr(line, column, file_name) {};
            ~Unit() = default;

            void* accept(Visitor* visitor) { return visitor->visit(this); }
    };

    class Object : public Expr {
        public:
            Object(int line, int column, std::string file_name, std::string name) : 
                Expr(line, column, file_name), name(name) {};
            ~Object() = default;

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            std::string get_name() { return name; }
        private:
            std::string name;
    };

    class New : public Expr {
        public:
            New(int line, int column, std::string file_name, std::string type) : 
                Expr(line, column, file_name), type(type) {};
            ~New() = default;

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            std::string get_type() { return type; }
        private:
            std::string type;
    };

    class Self : public Expr {
        public:
            Self(int line, int column, std::string file_name) : 
                Expr(line, column, file_name) {};
            ~Self() = default;

            void* accept(Visitor* visitor) { return visitor->visit(this); }
    };

    class Binop : public Expr {
        public:
            Binop(int line, int column, std::string file_name, std::string op, Expr* left_expr, Expr* right_expr) : 
                Expr(line, column, file_name), op(op), left_expr(left_expr), right_expr(right_expr) {};
            ~Binop() {
                delete left_expr;
                delete right_expr;
            };

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            std::string get_op() { return op; }
            Expr* get_left_expr() { return left_expr; }
            Expr* get_right_expr() { return right_expr; }
        private:
            std::string op;
            Expr* left_expr;
            Expr* right_expr;
    };

    class Unop : public Expr {
        public:
            Unop(int line, int column, std::string file_name, std::string op, Expr* expr) : 
                Expr(line, column, file_name), op(op), expr(expr) {};
            ~Unop() {
                delete expr;
            };

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            std::string get_op() { return op; }
            Expr* get_expr() { return expr; }
        private:
            std::string op;
            Expr* expr;
    };

    class Assign : public Expr {
        public:
            Assign(int line, int column, std::string file_name, std::string name, Expr* expr) : 
                Expr(line, column, file_name), name(name), expr(expr) {};
            ~Assign() {
                delete expr;
            };

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            std::string get_name() { return name; }
            Expr* get_expr() { return expr; }
        private:
            std::string name;
            Expr* expr;
    };

    class Let : public Expr {
        public:
            Let(int line, int column, std::string file_name, std::string name, std::string type, Expr* init_expr, Expr* scope_expr) : 
                Expr(line, column, file_name), name(name), type(type), init_expr(init_expr), scope_expr(scope_expr) {};
            Let(int line, int column, std::string file_name, std::string name, std::string type, Expr* scope_expr) : 
                Expr(line, column, file_name), name(name), type(type), init_expr(nullptr), scope_expr(scope_expr) {};
            ~Let() {
                if (has_init_expr())
                    delete init_expr;
                delete scope_expr;
            };

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            bool has_init_expr() { return init_expr != nullptr; }
            std::string get_name() { return name; }
            std::string get_type() { return type; }
            Expr* get_init_expr() { return init_expr; }
            Expr* get_scope_expr() { return scope_expr; }
        private:
            std::string name;
            std::string type;
            Expr* init_expr; // not mandatory
            Expr* scope_expr;
    };

    class While : public Expr {
        public:
            While(int line, int column, std::string file_name, Expr* cond_expr, Expr* body_expr) : 
                Expr(line, column, file_name), cond_expr(cond_expr), body_expr(body_expr) {};
            ~While() {
                delete cond_expr;
                delete body_expr;
            };

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            Expr* get_cond_expr() { return cond_expr; }
            Expr* get_body_expr() { return body_expr; }
        private:
            Expr* cond_expr;
            Expr* body_expr;
    };

    class If : public Expr {
        public:
            If(int line, int column, std::string file_name, Expr* cond_expr, Expr* then_expr, Expr* else_expr) : 
                Expr(line, column, file_name), cond_expr(cond_expr), then_expr(then_expr), else_expr(else_expr) {};
            If(int line, int column, std::string file_name, Expr* cond_expr, Expr* then_expr) :
                Expr(line, column, file_name), cond_expr(cond_expr), then_expr(then_expr), else_expr(nullptr) {};
            ~If() {
                delete cond_expr;
                delete then_expr;
                if (has_else_expr())
                    delete else_expr;
            };

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            bool has_else_expr() { return else_expr != nullptr; }
            Expr* get_cond_expr() { return cond_expr; }
            Expr* get_then_expr() { return then_expr; }
            Expr* get_else_expr() { return else_expr; }
        private:
            Expr* cond_expr;
            Expr* then_expr;
            Expr* else_expr; // not mandatory
    };

    class Block : public Expr {
        public:
            Block(int line, int column, std::string file_name, List<Expr>* expr_list) : 
                Expr(line, column, file_name), expr_list(expr_list) {};
            ~Block() {
                delete expr_list;
            };

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            List<Expr>* get_expr_list() { return expr_list; }
        private:
            List<Expr>* expr_list;
    };

    class Formal : public Expr {
        public:
            Formal(int line, int column, std::string file_name, std::string name, std::string type) : 
                Expr(line, column, file_name), name(name), type(type) {};
            ~Formal() = default;

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            std::string get_name() { return name; }
            std::string get_type() { return type; }
        private:
            std::string name;
            std::string type;
    };

    class Method : public Expr {
        public:
            Method(int line, int column, std::string file_name, std::string name, List<Formal>* formal_list, std::string return_type, Block* body_block) : 
                Expr(line, column, file_name), name(name), formal_list(formal_list), return_type(return_type), body_block(body_block) {};
            ~Method() {
                delete formal_list;
                delete body_block;
            };

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            std::string get_name() { return name; }
            List<Formal>* get_formal_list() { return formal_list; }
            std::string get_return_type() { return return_type; }
            Expr* get_body_block() { return body_block; }
        private:
            std::string name;
            List<Formal>* formal_list;
            std::string return_type;
            Block* body_block;
    };

    class Field : public Expr {
        public:
            Field(int line, int column, std::string file_name, std::string name, std::string type, Expr* init_expr) : 
                Expr(line, column, file_name), name(name), type(type), init_expr(init_expr) {};
            ~Field() {
                if (has_init_expr())
                    delete init_expr;
            };

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            bool has_init_expr() { return init_expr != nullptr; }
            std::string get_name() { return name; }
            std::string get_type() { return type; }
            Expr* get_init_expr() { return init_expr; }
        private:
            std::string name;
            std::string type;
            Expr* init_expr; // not mandatory
    };

    class Class : public Expr {
        public:
            Class(int line, int column, std::string file_name, std::string name, std::string parent, List<Field>* field_list, List<Method>* method_list) : 
                Expr(line, column, file_name), name(name), parent(parent), field_list(field_list), method_list(method_list) {};
            ~Class() {
                delete field_list;
                delete method_list;
            };

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            std::string get_name() { return name; }
            std::string get_parent() { return parent; }
            List<Field>* get_field_list() { return field_list; }
            List<Method>* get_method_list() { return method_list; }
        private:
            std::string name;
            std::string parent;
            List<Field>* field_list;
            List<Method>* method_list;
    };

    class Program : public Expr {
        public:
            Program(int line, int column, std::string file_name, List<Class>* class_list) : 
                Expr(line, column, file_name), class_list(class_list) {};
            ~Program() {
                delete class_list;
            };

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            List<Class>* get_class_list() { return class_list; }
        private:
            List<Class>* class_list;
    };

    class Call : public Expr {
        public:
            Call(int line, int column, std::string file_name, Expr* object, std::string method, List<Expr>* arg_expr_list) : 
                Expr(line, column, file_name), object(object), method(method), arg_expr_list(arg_expr_list) {};
            ~Call() {
                delete object;
                delete arg_expr_list;
            };

            void* accept(Visitor* visitor) { return visitor->visit(this); }

            Expr* get_object() { return object; }
            std::string get_method() { return method; }
            List<Expr>* get_arg_expr_list() { return arg_expr_list; }
        private:
            Expr* object;
            std::string method;
            List<Expr>* arg_expr_list;
    };
}

#endif
