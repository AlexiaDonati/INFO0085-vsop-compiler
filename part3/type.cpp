#include "type.hpp"
#include "ast.hpp"

using namespace AST;
using namespace AST::type;

void Table::throw_error(Expr *expr, std::string message) { 
    Error *new_error = new Error(
        expr->get_line(),
        expr->get_column(),
        expr->get_file_name(),
        message
    );
    error_list.push_back(new_error); 
}