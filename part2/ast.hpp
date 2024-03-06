#ifndef _AST_HPP
#define _AST_HPP

#include <string>
#include <vector>

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
    class Unop;
    class Binop;
    class Call;
    class New;
    class String;
    class Integer;
    class Boolean;
    class Unit;
    class Object;

    // Data classes

    class Expr {
        public:
            Expr(int line, int column, std::string file_name) : 
                line(line), column(column), file_name(file_name) {};
            ~Expr() = default;

            int get_line() { return line; }
            int get_column() { return column; }
            std::string get_file_name() { return file_name; }
        private:
            int line;
            int column;
            std::string file_name;
    };

    template <typename T>
    class List : public Expr {
        public:
            List(int line, int column, std::string file_name) : 
                Expr(line, column, file_name) {};
            ~List() {
                for (auto expr : list)
                    delete expr;
            };

            void add(T* expr) { list.push_back(expr);};
            unsigned long size() { return list.size();};
        private:
            std::vector<T*> list;
    };

    class String : public Expr {
        public:
            String(int line, int column, std::string file_name, std::string value) : 
                Expr(line, column, file_name), value(value) {};
            ~String() = default;

            std::string get_value() { return value; }
        private:
            std::string value;
    };

    class Integer : public Expr {
        public:
            Integer(int line, int column, std::string file_name, int value) : 
                Expr(line, column, file_name), value(value) {};
            ~Integer() = default;

            int get_value() { return value; }
        private:
            int value;
    };

    class Boolean : public Expr {
        public:
            Boolean(int line, int column, std::string file_name, bool value) : 
                Expr(line, column, file_name), value(value) {};
            ~Boolean() = default;

            bool get_value() { return value; }
        private:
            bool value;
    };


    class Unit : public Expr {
        public:
            Unit(int line, int column, std::string file_name) : 
                Expr(line, column, file_name) {};
            ~Unit() = default;
    };

    class Object : public Expr {
        public:
            Object(int line, int column, std::string file_name, std::string name) : 
                Expr(line, column, file_name), name(name) {};
            ~Object() = default;

            std::string get_name() { return name; }
        private:
            std::string name;
    };

    class New : public Expr {
        public:
            New(int line, int column, std::string file_name, std::string type) : 
                Expr(line, column, file_name), type(type) {};
            ~New() = default;

            std::string get_type() { return type; }
        private:
            std::string type;
    };

    class Binop : public Expr {
        public:
            Binop(int line, int column, std::string file_name, std::string op, Expr* left_expr, Expr* right_expr) : 
                Expr(line, column, file_name), op(op), left_expr(left_expr), right_expr(right_expr) {};
            ~Binop() {
                delete left_expr;
                delete right_expr;
            };

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
            Block(int line, int column, std::string file_name) : 
                Expr(line, column, file_name), expr_list(new List<Expr>(line, column, file_name)) {};
            ~Block() {
                delete expr_list;
            };

            List<Expr>* get_expr_list() { return expr_list; }
        private:
            List<Expr>* expr_list;
    };

    class Formal : public Expr {
        public:
            Formal(int line, int column, std::string file_name, std::string name, std::string type) : 
                Expr(line, column, file_name), name(name), type(type) {};
            ~Formal() = default;

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

            List<Class>* get_class_list() { return class_list; }
        private:
            List<Class>* class_list;
    };

    class Call : public Expr {
        public:
            Call(int line, int column, std::string file_name, Object* object, std::string method, List<Expr>* arg_expr_list) : 
                Expr(line, column, file_name), object(object), method(method), arg_expr_list(arg_expr_list) {};
            ~Call() {
                delete object;
                delete arg_expr_list;
            };

            Expr* get_object() { return object; }
            std::string get_method() { return method; }
            List<Expr>* get_arg_expr_list() { return arg_expr_list; }
        private:
            Object* object;
            std::string method;
            List<Expr>* arg_expr_list;
    };
}

#endif
