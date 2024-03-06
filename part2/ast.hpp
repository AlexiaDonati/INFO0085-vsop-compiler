#ifndef _AST_HPP
#define _AST_HPP

#include <string>
#include <vector>

namespace AST
{
    enum KEYWORD {AND, BOOL, CLASS, DO, ELSE, EXTENDS, FALSE, IF, IN, INT32, ISNULL, LET, NEW, NOT, SELF, STRING, THEN, TRUE, UNIT, WHILE};

    class Integer_literal;
    class Type_identifier;
    class Keyword;
    class Object_identifier;
    class String_literal;
    class Lbrace;
    class Rbrace;
    class Lpar;
    class Rpar;
    class Colon;
    class Semicolon;
    class Comma;
    class Plus;
    class Minus;
    class Times;
    class Div;
    class Pow;
    class Dot;
    class Equal;
    class Lower_equal;
    class Assign;
    class Lower;

    // Visitor classes

    class Visitor{
        public:
            virtual void* visit(Integer_literal integer_literal) = 0;
            virtual void* visit(Type_identifier type_identifier) = 0;
            virtual void* visit(Keyword keyword) = 0;
            virtual void* visit(Object_identifier object_identifier) = 0;
            virtual void* visit(String_literal string_literal) = 0;

            virtual void* visit(Lbrace lbrace) = 0;
            virtual void* visit(Rbrace rbrace) = 0;
            virtual void* visit(Lpar lpar) = 0;
            virtual void* visit(Rpar rpar) = 0;
            virtual void* visit(Colon colon) = 0;
            virtual void* visit(Semicolon semicolon) = 0;
            virtual void* visit(Comma comma) = 0;
            virtual void* visit(Plus plus) = 0;
            virtual void* visit(Minus minus) = 0;
            virtual void* visit(Times times) = 0;
            virtual void* visit(Div div) = 0;
            virtual void* visit(Pow pow) = 0;
            virtual void* visit(Dot dot) = 0;
            virtual void* visit(Equal equal) = 0;
            virtual void* visit(Lower_equal lower_equal) = 0;
            virtual void* visit(Assign assign) = 0;
            virtual void* visit(Lower lower) = 0;
    };

    class Print_visitor : public Visitor {
        public:
            void* visit(Integer_literal integer_literal);
            void* visit(Type_identifier type_identifier);
            void* visit(Keyword keyword);
            void* visit(Object_identifier object_identifier);
            void* visit(String_literal string_literal);

            void* visit(Lbrace lbrace);
            void* visit(Rbrace rbrace);
            void* visit(Lpar lpar);
            void* visit(Rpar rpar);
            void* visit(Colon colon);
            void* visit(Semicolon semicolon);
            void* visit(Comma comma);
            void* visit(Plus plus);
            void* visit(Minus minus);
            void* visit(Times times);
            void* visit(Div div);
            void* visit(Pow pow);
            void* visit(Dot dot);
            void* visit(Equal equal);
            void* visit(Lower_equal lower_equal);
            void* visit(Assign assign);
            void* visit(Lower lower);
    };

    // Data classes

    class Expr {
        public:
            Expr(std::string file_name, int line, int column) 
                : file_name(file_name), line(line), column(column) {}
            virtual ~Expr(){
                for (auto it = this->childrens.begin(); it != this->childrens.end(); ++it){
                    delete *it;
                }
            };
            virtual void* accept(Visitor* visitor) = 0;
            std::string get_file_name() { return file_name; }
            int get_line() { return line; }
            int get_column() { return column; }
            void add_child(Expr* child) { childrens.push_back(child); }
            Expr* get_child(unsigned long index) { 
                if (index >= childrens.size()) 
                    return nullptr;
                return childrens[index]; 
            }

        private:
            std::vector<Expr*> childrens;
            std::string file_name;
            int line;
            int column;
    };

    class Integer_literal : public Expr {
        public:
            Integer_literal(int value, std::string file_name, int line, int column)
                : Expr(file_name, line, column), value(value) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
            int get_value() { return value; }

        private:
            int value;
    };

    class Type_identifier : public Expr {
        public:
            Type_identifier(std::string value, std::string file_name, int line, int column)
                : Expr(file_name, line, column), value(value) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
            std::string get_value() { return value; }

        private:
            std::string value;
    };

    class Keyword : public Expr {
        public:
            Keyword(KEYWORD value, std::string file_name, int line, int column)
                : Expr(file_name, line, column), value(value) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
            KEYWORD get_value() { return value; }

        private:
            KEYWORD value;
    };

    class Object_identifier : public Expr {
        public:
            Object_identifier(std::string value, std::string file_name, int line, int column)
                : Expr(file_name, line, column), value(value) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
            std::string get_value() { return value; }

        private:
            std::string value;
    };

    class String_literal : public Expr {
        public:
            String_literal(std::string value, std::string file_name, int line, int column)
                : Expr(file_name, line, column), value(value) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
            std::string get_value() { return value; }

        private:
            std::string value;
    };

    class Lbrace : public Expr {
        public:
            Lbrace(std::string file_name, int line, int column)
                : Expr(file_name, line, column) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
    };

    class Rbrace : public Expr {
        public:
            Rbrace(std::string file_name, int line, int column)
                : Expr(file_name, line, column) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
    };

    class Lpar : public Expr {
        public:
            Lpar(std::string file_name, int line, int column)
                : Expr(file_name, line, column) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
    };

    class Rpar : public Expr {
        public:
            Rpar(std::string file_name, int line, int column)
                : Expr(file_name, line, column) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
    };

    class Colon : public Expr {
        public:
            Colon(std::string file_name, int line, int column)
                : Expr(file_name, line, column) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
    };

    class Semicolon : public Expr {
        public:
            Semicolon(std::string file_name, int line, int column)
                : Expr(file_name, line, column) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
    };

    class Comma : public Expr {
        public:
            Comma(std::string file_name, int line, int column)
                : Expr(file_name, line, column) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
    };

    class Plus : public Expr {
        public:
            Plus(std::string file_name, int line, int column)
                : Expr(file_name, line, column) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
    };

    class Minus : public Expr {
        public:
            Minus(std::string file_name, int line, int column)
                : Expr(file_name, line, column) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
    };

    class Times : public Expr {
        public:
            Times(std::string file_name, int line, int column)
                : Expr(file_name, line, column) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
    };

    class Div : public Expr {
        public:
            Div(std::string file_name, int line, int column)
                : Expr(file_name, line, column) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
    };

    class Pow : public Expr {
        public:
            Pow(std::string file_name, int line, int column)
                : Expr(file_name, line, column) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
    };

    class Dot : public Expr {
        public:
            Dot(std::string file_name, int line, int column)
                : Expr(file_name, line, column) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
    };

    class Equal : public Expr {
        public:
            Equal(std::string file_name, int line, int column)
                : Expr(file_name, line, column) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
    };

    class Lower_equal : public Expr {
        public:
            Lower_equal(std::string file_name, int line, int column)
                : Expr(file_name, line, column) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
    };

    class Assign : public Expr {
        public:
            Assign(std::string file_name, int line, int column)
                : Expr(file_name, line, column) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
    };

    class Lower : public Expr {
        public:
            Lower(std::string file_name, int line, int column)
                : Expr(file_name, line, column) {}
            void* accept(Visitor* visitor) { return visitor->visit(*this); }
    };

}

#endif
