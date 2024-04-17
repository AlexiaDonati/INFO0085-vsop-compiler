#include "ast.hpp"
#include <iostream>
#include <map>

using namespace AST;
using namespace std;

void* Check_classes::visit(Method* method){

    map<string, Formal*> formals_map; // Check formal redefinition in current method
    List<Formal>* formals = method->get_formal_list();

    if(!formals){
        return method;
    }

    size_t formals_size = formals->get_size();
    for (size_t i = 0; i < formals_size; i++){
        Formal* formal = (Formal *) formals->accept_one(this, i);
        string name = formal->get_name();
        if (formals_map.find(name) != formals_map.end()){
            cout << formal->get_file_name() << ":" << formal->get_line() << ":" << formal->get_column() << ": semantic error: Formal with name " << name << " already exists." << endl;
            return nullptr;
        }
        formals_map[name] = formal;
    }

    return method;
}

bool Check_classes::check_class_body_redefinition(Class* class_){
    List<Field>* field_list = class_->get_field_list();
    size_t field_size = field_list->get_size();
    for (size_t i = 0; i < field_size; i++){
        Field* field = (Field *) field_list->accept_one(this, i);

        string name = field->get_name();
        if (class_->field_map.find(name) != class_->field_map.end()){
            cout << field->get_file_name() << ":" << field->get_line() << ":" << field->get_column() << ": semantic error: Field with name " << name << " already exists." << endl;
            return false;
        }
        class_->field_map[name] = field;
    }


    List<Method>* method_list = class_->get_method_list();
    size_t method_size = method_list->get_size();
    for (size_t i = 0; i < method_size; i++){
        Method* method = (Method *) method_list->accept_one(this, i);
        if(!method){
            return false;
        }

        string name = method->get_name();
        if (class_->method_map.find(name) != class_->method_map.end()){
            cout << method->get_file_name() << ":" << method->get_line() << ":" << method->get_column() << ": semantic error: Method with name " << name << " already exists." << endl;
            return false;
        }
        class_->method_map[name] = method;
    }

    return true;
}

void* Check_classes::visit(Class* class_){
    string name = class_->get_name();

    if (class_map.find(name) != class_map.end()) {
        cout << class_->get_file_name() << ":" << class_->get_line() << ":" << class_->get_column() << ": semantic error: Class with name " << name << " already exists." << endl;
        return TO_VOID(false);
    }

    class_map[name] = class_;

    if(!check_class_body_redefinition(class_)){
        return TO_VOID(false);
    }

    if (name == "Main") { // Check for Main class existence
        main_class_exists = true;
        
        if(class_->method_map.find("main") == class_->method_map.end()){
            cout << class_->get_file_name() << ":" << class_->get_line() << ":" << class_->get_column() << ": semantic error: main method not found in Main class." << endl;
            return TO_VOID(false);
        } else{
            Method* method = class_->method_map["main"];
            if(method->get_return_type() != "int32"){
                cout << method->get_file_name() << ":" << method->get_line() << ":" << method->get_column() << ": semantic error: main method should return int32." << endl;
                return TO_VOID(false);
            }
            if(method->get_formal_list()->get_size() != 0){
                cout << method->get_file_name() << ":" << method->get_line() << ":" << method->get_column() << ": semantic error: main method should not have any formals." << endl;
                return TO_VOID(false);
            }
        }
    }

    return TO_VOID(true);
}

bool Check_classes::fill_class_map(List<Class>* class_list){
    size_t size = class_list->get_size();
    for (size_t i = 0; i < size; i++){
        if (!TO_VALUE(class_list->accept_one(this, i))){
            return false;
        }
    }

    return true;
}

bool Check_classes::check_override_field(Class* class_) {
    for (auto it = class_->field_map.begin(); it != class_->field_map.end(); it++){
        string name = it->first;
        Field* field = it->second;

        Class* parent = class_map[class_->get_parent()];
        while (parent->get_name() != "Object"){
            if (parent->field_map.find(name) != parent->field_map.end()){
                cout << field->get_file_name() << ":" << field->get_line() << ":" << field->get_column() << ": semantic error: Field with name " << name << " already exists in parent class." << endl;
                return false;
            }

            parent = class_map[parent->get_parent()];
        }
    }

    return true;      
}

bool Check_classes::check_override_method(Class* class_) {
    for (auto it = class_->method_map.begin(); it != class_->method_map.end(); it++){
        string name = it->first;
        Method* method = it->second;

        Class* parent = class_map[class_->get_parent()];
        while (parent->get_name() != "Object"){
            if (parent->method_map.find(name) != parent->method_map.end()){

                // Check return type
                Method* parent_method = parent->method_map[name];
                if (method->get_return_type() != parent_method->get_return_type()){
                    cout << method->get_file_name() << ":" << method->get_line() << ":" << method->get_column() << ": semantic error: Method with name " << name << " has different return type in parent class." << endl;
                    return false;
                }

                // Check formals
                List<Formal>* formals = method->get_formal_list();
                List<Formal>* parent_formals = parent_method->get_formal_list();
                if (formals->get_size() != parent_formals->get_size()){
                    cout << method->get_file_name() << ":" << method->get_line() << ":" << method->get_column() << ": semantic error: Method with name " << name << " has different number of formals in parent class." << endl;
                    return false;
                }

                size_t formals_size = formals->get_size();
                for (size_t i = 0; i < formals_size; i++){
                    Formal* formal = (Formal *) formals->accept_one(this, i);
                    Formal* parent_formal = (Formal *) parent_formals->accept_one(this, i);
                    if (formal->get_type() != parent_formal->get_type()){
                        cout << method->get_file_name() << ":" << method->get_line() << ":" << method->get_column() << ": semantic error: Method with name " << name << " has different type of formal in parent class." << endl;
                        return false;
                    }
                }
            }

            parent = class_map[parent->get_parent()];
        }
    }  

    return true;    
}

bool Check_classes::check_extend(){
    map<string, Class*> extend_path;
    for (auto it = class_map.begin(); it != class_map.end(); it++){
        string name = it->first;
        extend_path[name] = it->second;

        string parent_name = it->second->get_parent();

        if (parent_name == "Object" || class_map.find(parent_name) == class_map.end()){
            continue;
        }

        Class* parent = class_map[parent_name];
        extend_path[parent_name] = parent;

        while (parent->get_name() != "Object"){
            string next_parent_name = parent->get_parent();

            if (extend_path.find(next_parent_name) != extend_path.end()){
                cout << it->second->get_file_name() << ":" << it->second->get_line() << ":" << it->second->get_column() << ": semantic error: Inheritance cycle detected." << endl;
                return false;
            }

            parent = class_map[next_parent_name];
            extend_path[next_parent_name] = parent;
        }
    
        if(!check_override_field(it->second)){
            return false;
        }
        if(!check_override_method(it->second)){
            return false;
        }
    }

    return true;
}

void Check_classes::add_object_class(Program* program){
    int line = program->get_line();
    int column = program->get_column();
    string file_name = program->get_file_name();

    List<Field>* field_list = new List<Field>();
    List<Method>* method_list = new List<Method>();
 
    List<Formal>* print_formals = new List<Formal>();
    print_formals->add(new Formal(line, column, file_name, "s", "string"));
    Method *print = new Method(line, column, file_name, "print", print_formals, "Object", NULL);
    method_list->add(print);

    List<Formal>* printBool_formals = new List<Formal>();
    printBool_formals->add(new Formal(line, column, file_name, "b", "bool"));
    Method *printBool = new Method(line, column, file_name, "printBool", printBool_formals, "Object", NULL);
    method_list->add(printBool);
    
    List<Formal>* printInt_formals = new List<Formal>();
    printInt_formals->add(new Formal(line, column, file_name, "i", "int32"));
    Method *printInt32 = new Method(line, column, file_name, "printInt32", printInt_formals, "Object", NULL);
    method_list->add(printInt32);
    
    Method *inputLine = new Method(line, column, file_name, "inputLine", new List<Formal>(), "string", NULL);
    method_list->add(inputLine);

    Method *inputBool = new Method(line, column, file_name, "inputBool", new List<Formal>(), "bool", NULL);
    method_list->add(inputBool);

    Method *inputInt = new Method(line, column, file_name, "inputInt32", new List<Formal>(), "int32", NULL);
    method_list->add(inputInt);

    Class* object_class = new Class(line, column, file_name, "Object", "", field_list, method_list);
    class_map["Object"] = object_class;
}

void* Check_classes::visit(Program* program){
    add_object_class(program);

    List<Class>* class_list = program->get_class_list();
    if(!fill_class_map(class_list)){
        return TO_VOID(false); // Error in class definition
    }

    if (!main_class_exists) {
        cout << program->get_file_name() << program->get_line() << program->get_column() << ": semantic error: Main class not found." << endl;
        return TO_VOID(false); // Error in main class definition
    }

    if(!check_extend()){
        return TO_VOID(false); // Error in class inheritance
    }

    return TO_VOID(true);
}