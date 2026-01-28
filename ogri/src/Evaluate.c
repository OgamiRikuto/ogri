#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "Ast.h"
#include "built_in_functions.h"
#include "Environment.h"
#include "Evaluate.h"
#include "Iterator.h"
#include "List.h"
#include "Object.h"

/**
 * エラー文を出力します。
 */
static void runtime_error(int line, const char* format, ...)
{
    va_list args;
    fprintf(stderr, "Runtime Error at line %d: ", line);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

/**
 * 単一のオブジェクトをリストにラップします。
 */
static Object* wrap_list(Object* value)
{
    List* list = newList(Object*);
    add(list, &value);
    return new_array(list);
}


/**
 * 実行します。
 */
int evaluate(Ast* node)
{
    if(node == NULL) {
        fprintf(stderr, "Runtime Error: no statments...\n");
        return EXIT_FAILURE;
    }
    Environment* env = newEnv(NULL);
    Interpreter* interpreter = malloc(sizeof(Interpreter));
    if(interpreter == NULL) {
        fprintf(stderr, "Runtime Error: Cannot ready for evaluate...\n");
        return EXIT_FAILURE;
    }

    interpreter->loop_level = 0;
    interpreter->call_stack_depth = 0;

    set_builtins(env);

    eval(node, env, interpreter);

    free(interpreter);
    
    return EXIT_SUCCESS;
}

/**
 * 抽象木を再帰的に探索して実行します。
 */
Object* eval(Ast* node, Environment* env, Interpreter* interpreter)
{
    if(node == NULL) return NULL;
    switch(node->kind) {
        case AST_STATEMENTS:
            return eval_statements(node, env, interpreter);
        case AST_WHEN:
            return eval_when(node, env, interpreter);
        case AST_OTHERWHEN:
            return eval_otherwhen(node, env, interpreter);
        case AST_REPEAT:
            return eval_repeat(node, env, interpreter);
        case AST_REPEAT_UNTIL:
            return eval_repeat_until(node, env, interpreter);
        case AST_FUNC_DEF:
            return eval_func_def(node, env, interpreter);
        case AST_ASSIGN: {
                Object* value = eval(node->assign.right, env, interpreter);
                if(node->assign.is_are) {
                    if(value->type != LIST)
                        value = wrap_list(value);
                } else {
                    bool is_single = (node->assign.left->kind == AST_IDENTIFIER ||
                                        node->assign.left->kind == AST_ARRAY_ACCESS);
                    if(is_single && value->type == LIST && getSize(value->list) == 1) {
                        Object* content;
                        getAt(value->list, 0, Object*, &content);
                        dList(value->list);
                        value = content;
                    }
                }
                Object* result = eval_assign(node->assign.left, value, env, interpreter);
                return result;
        }
        case AST_RETURN: {
            Object* value = NULL;
            if(node->return_stmt.expr != NULL)
                value = eval(node->return_stmt.expr, env, interpreter);
            else 
                value = new_bool(true);
            return new_result(value);
        }
        case AST_BREAK:
            return new_break();
        case AST_CONTINUE:
            return new_continue();
        case AST_FUNC_CALL:
            return eval_func_call(node, env, interpreter);
        case AST_BLOCK:
            return eval_block(node, env, interpreter);
        case AST_BINOP: {
            const char* op = node->binop.op;
            if(strcmp(op, "and") == 0 || strcmp(op, "or") == 0) 
                return eval_logical(node, env, interpreter);
            return eval_binop(node, env, interpreter);
        }
        case AST_UNARY:
            return eval_unary(node, env, interpreter);
        case AST_IDENTIFIER: {
            Object* value = env_get(env, node->identifier.name, node->line);
            if(value == NULL) 
                runtime_error(node->line, "undefined variable '%s'\n", node->identifier.name);
            return value;
        }
        case AST_INTEGER:
            return new_int(node->integer.value);
        case AST_FLOAT:
            return new_float(node->real.value);
        case AST_STRING:
            return new_string(node->string.value);
        case AST_FSTRING:
            return eval_fstring(node, env, interpreter);
        case AST_VALUE_LIST:
            return eval_value_list(node, env, interpreter);
        case AST_ARRAY_ACCESS: 
            return eval_array_access(node, env, interpreter);
        case AST_SLICE:
            return eval_slice(node, env, interpreter);
        default: return NULL;
    }
}

/**
 * 与えられたオブジェクトからその真偽値を返します。
 */
static bool is_true(Object* object)
{
    if(object == NULL) return false;
    switch(object->type) {
        case BOOL:      return object->boolean;
        case INTEGER:   return object->integer != 0;
        case FLOAT:     return object->real != 0.0;
        case LIST:      return getSize(object->list) > 0;
        default:    return true;
    }
}

/**
 * 文を実行します。
 */
Object* eval_statements(Ast* node, Environment* env, Interpreter* interpreter)
{
    Object* result = NULL;

    if(node->list.first != NULL) {
        result = eval(node->list.first, env, interpreter);
        if(result != NULL && (result->type == RETURN || result->type == BREAK || result->type == CONTINUE)) 
            return result;
    }
    if(node->list.next != NULL) {
        result = eval(node->list.next, env, interpreter);
        if(result != NULL && (result->type == RETURN || result->type == BREAK || result->type == CONTINUE)) 
            return result;
    }
    
    return result;
}

/**
 * repeat文を実行します。
 */
Object* eval_repeat(Ast* node, Environment* env, Interpreter* interpreter)
{
    Object* collection = eval(node->repeat_stmt.collection, env, interpreter);

    if(collection == NULL)
        runtime_error(node->line, "repeat..foreach requires a list.\n");

    List* target = NULL;
    bool is_temporary_list = false;

    if (collection->type == LIST) {
        target = collection->list;
    } else {
        target = newList(Object*);
        add(target, &collection);
        is_temporary_list = true;
    }

    const char* variable_name = node->repeat_stmt.identifier->identifier.name;
    Object* result = NULL;

    Iterator* iterator = newIterator(new_array(target));
    interpreter->loop_level++;

    while(has_next(iterator)) {
        Object* item = next(iterator);
        env_set(env, variable_name, item);

        result = eval(node->repeat_stmt.block, env, interpreter);

        if(result != NULL) {
            if(result->type == BREAK) {
                result = NULL;
                break;
            }
            if (result->type == CONTINUE) {
                result = NULL;
                continue;
            }
            if (result->type == RETURN) {
                break;
            }
        }
    }
    free(iterator);
    if(is_temporary_list)
        dList(target);
    interpreter->loop_level--;

    return result;
}

/**
 * repeat_until文を実行します。
 */
Object* eval_repeat_until(Ast* node, Environment* env, Interpreter* interpreter)
{
    Object* result = NULL;
    interpreter->loop_level++;
    while(1) {
        Object* condition = eval(node->repeat_until_stmt.cond, env, interpreter);

        if(is_true(condition)) break;

        result = eval(node->repeat_until_stmt.block, env, interpreter);

        if(result != NULL) {
            if(result->type == BREAK) {
                result = NULL;
                break;
            }
            if (result->type == CONTINUE) {
                result = NULL;
                continue;
            }
            if (result->type == RETURN) {
                break;
            }
        }
    }
    return result;
}

/**
 * ブロックを実行します。
 */
Object* eval_block(Ast* node, Environment* env, Interpreter* interpreter)
{
    if(node == NULL || node->block.statements == NULL) return NULL;
    Environment* newScope = newEnv(env);
    return eval(node->block.statements, newScope, interpreter);
}

/**
 * when文を実行します。
 */
Object* eval_when(Ast* node, Environment* env, Interpreter* interpreter)
{
    Object* conditon = eval(node->when_stmt.cond, env, interpreter);

    if(is_true(conditon)) 
        return eval(node->when_stmt.then_block, env, interpreter);

    if(node->when_stmt.otherwhen_list != NULL) {
        Object* result = eval_otherwhen(node->when_stmt.otherwhen_list, env, interpreter);
        if(result != NULL) return result;
    }
    if(node->when_stmt.other_block != NULL) {
        return eval(node->when_stmt.other_block, env, interpreter);
    }
    return NULL;

}

/**
 * otherwise_when文を実行します。
 */
Object* eval_otherwhen(Ast* node, Environment* env, Interpreter* interpreter)
{
    if(node == NULL) return NULL;

    Object* condition = eval(node->otherwhen.cond, env, interpreter);

    if(is_true(condition)) {
        return eval(node->otherwhen.block, env, interpreter);
    } else if(node->otherwhen.next != NULL) 
        return eval_otherwhen(node->otherwhen.next, env, interpreter);

    return NULL;
}

/**
 * 論理式を実行します。
 */
Object* eval_logical(Ast* node, Environment* env, Interpreter* interpreter)
{
    const char* op = node->binop.op;
    Object* left = eval(node->binop.left, env, interpreter);
    if(strcmp(op, "and") == 0) {
        if(!is_true(left)) return left;
        return eval(node->binop.right, env, interpreter);
    }

    if(strcmp(op, "or") == 0) {
        if(is_true(left)) return left;
        return eval(node->binop.right, env, interpreter);
    }
    return new_bool(false);
}

/**
 * 二項演算を実行します。
 */
Object* eval_binop(Ast* node, Environment* env, Interpreter* interpreter)
{
    Object* left = eval(node->binop.left, env, interpreter);
    Object* right = eval(node->binop.right, env, interpreter);

    const char* op = node->binop.op;

    if (left->type == STRING && right->type == STRING) {
        if (strcmp(op, "==") == 0) return new_bool(strcmp(left->string, right->string) == 0);
        if (strcmp(op, "!=") == 0) return new_bool(strcmp(left->string, right->string) != 0);
        
        if (strcmp(op, "+") == 0) {
            char buf[1<<16];
            snprintf(buf, sizeof(buf), "%s%s", left->string, right->string);
            return new_string(buf);
        }
    
        runtime_error(node->line, "Operator '%s' is not supported for strings.\n", op);
    }

    if(left->type == INTEGER && right->type == INTEGER) {
        if(strcmp(op, "+") == 0) return new_int(left->integer + right->integer);
        if(strcmp(op, "-") == 0) return new_int(left->integer - right->integer);
        if(strcmp(op, "*") == 0) return new_int(left->integer * right->integer);
        if(strcmp(op, "/") == 0) {
            if(right->integer == 0)
                runtime_error(node->line, "Divide by zero.\n");
            return new_int(left->integer / right->integer);
        }
        if(strcmp(op, "%") == 0) return new_int(left->integer % right->integer);
        if(strcmp(op, "==") == 0) return new_bool(left->integer == right->integer);
        if(strcmp(op, "!=") == 0) return new_bool(left->integer != right->integer);
        if(strcmp(op, "<") == 0) return new_bool(left->integer < right->integer);
        if(strcmp(op, ">") == 0) return new_bool(left->integer > right->integer);
        if(strcmp(op, "<=") == 0) return new_bool(left->integer <= right->integer);
        if(strcmp(op, ">=") == 0) return new_bool(left->integer >= right->integer);
        runtime_error(node->line, "Operator '%s' is not supported for strings.\n", op);
    }
    if(left->type == FLOAT && right->type == FLOAT) {
        if(strcmp(op, "+") == 0) return new_float(left->real + right->real);
        if(strcmp(op, "-") == 0) return new_float(left->real - right->real);
        if(strcmp(op, "*") == 0) return new_float(left->real * right->real);
        if(strcmp(op, "/") == 0) {
            if(right->real == 0) 
                runtime_error(node->line, "Divide by zero.\n");
            return new_float(left->real / right->real);
        }
        if(strcmp(op, "%") == 0) 
            runtime_error(node->line, "'%%' is only for integer.\n");
        if(strcmp(op, "==") == 0) return new_bool(left->real == right->real);
        if(strcmp(op, "!=") == 0) return new_bool(left->real != right->real);
        if(strcmp(op, "<") == 0) return new_bool(left->real < right->real);
        if(strcmp(op, ">") == 0) return new_bool(left->real > right->real);
        if(strcmp(op, "<=") == 0) return new_bool(left->real <= right->real);
        if(strcmp(op, ">=") == 0) return new_bool(left->real >= right->real);
        runtime_error(node->line, "Operator '%s' is not supported for strings.\n", op);
    }
    if((left->type == FLOAT && right->type == INTEGER) ||
        (left->type == INTEGER && right->type == FLOAT)) {
            double left_value = (left->type == FLOAT) ? left->real : (double) left->integer;
            double right_value = (right->type == FLOAT) ? right->real : (double) right->integer;
            if(strcmp(op, "+") == 0) return new_float(left_value + right_value);
            if(strcmp(op, "-") == 0) return new_float(left_value - right_value);
            if(strcmp(op, "*") == 0) return new_float(left_value * right_value);
            if(strcmp(op, "/") == 0) {
                if(right_value == 0) 
                    runtime_error(node->line, "Divide by zero.\n");
                return new_float(left_value / right_value);
            }
            if(strcmp(op, "%") == 0) 
                runtime_error(node->line, "'%%' is only for integer.\n");

            if(strcmp(op, "==") == 0) return new_bool(left_value == right_value);
            if(strcmp(op, "!=") == 0) return new_bool(left_value != right_value);
            if(strcmp(op, "<") == 0) return new_bool(left_value < right_value);
            if(strcmp(op, ">") == 0) return new_bool(left_value > right_value);
            if(strcmp(op, "<=") == 0) return new_bool(left_value <= right_value);
            if(strcmp(op, ">=") == 0) return new_bool(left_value >= right_value);
            runtime_error(node->line, "Operator '%s' is not supported for strings.\n", op);
    }
    runtime_error(node->line, "Invalid operands for operator '%s'\n", op);
    exit(EXIT_FAILURE);
}

/**
 * リストより再帰的に代入します。
 */
static void assign_recursive(Ast* node, Object* list, int* index, Environment* env)
{
    if(node == NULL || list->type != LIST) return;

    if(node->kind == AST_IDENTIFIER_LIST) {
        assign_recursive(node->identifier_list.first, list, index, env);
        assign_recursive(node->identifier_list.next, list, index, env);
    } else if(node->kind == AST_IDENTIFIER) {
        Object* value;
        LIST_ERROR getErr = getAt(list->list, *index, Object*, &value);
        if(getErr != LIST_OK)
            runtime_error(node->line, "Failed to assign to '%s'\n", node->identifier.name);
        env_set(env, node->identifier.name, value);
        (*index)++;
    }
}

/**
 * 代入文を実行します。
 */
Object* eval_assign(Ast* node, Object* obj, Environment* env, Interpreter* interpreter)
{
    if(node == NULL) return obj;
    switch(node->kind) {
        case AST_IDENTIFIER:
            env_set(env, node->identifier.name, obj);
            return obj;
        case AST_ARRAY_ACCESS: {
            Object* list = env_get(env, node->array_access.identifier->identifier.name, node->line);
            Object* index = eval(node->array_access.index, env, interpreter);
            if(list->type == LIST && index->type == INTEGER) {
                LIST_ERROR setErr = setAt(list->list, (int)index->integer, Object*, &obj);
                if(setErr != LIST_OK) 
                    runtime_error(node->line, "Index out of range.\n");

                return obj;
            }
            break;
        }

        case AST_IDENTIFIER_LIST: {
            int index = 0;
            assign_recursive(node, obj, &index, env);
            return obj;
        }
        default: return obj;
    }
    return obj;
}

/**
 * 関数定義を実行します。
 */
Object* eval_func_def(Ast* node, Environment* env, Interpreter* interpreter)
{
    Object* function = new_func(node->func_def.params, node->func_def.body, env);
    env_set(env, node->func_def.name->identifier.name, function);
    return function;
}

/**
 * 関数呼び出しを実行します。
 */
Object* eval_func_call(Ast* node, Environment* env, Interpreter* interpreter)
{
    Object* function = env_get(env, node->func_call.name->identifier.name, node->line);
    if(function == NULL || (function->type != FUNCTION && function->type != BUILT_IN_FUNCTION)) 
        runtime_error(node->line, "'%s' is not a function.\n", node->func_call.name->identifier.name);

    Object* arguments = NULL;
    if(node->func_call.args != NULL) 
        arguments = eval(node->func_call.args, env, interpreter);

    if(function->type == BUILT_IN_FUNCTION) {
        List* args = NULL;
        bool tmp_list = false;
        if(arguments == NULL) {
            return function->b_func(NULL);
        }
        if(node->func_call.args->kind == AST_VALUE_LIST && node->func_call.args->value_list.next != NULL) {
                args = arguments->list;
        } else {
            args = newList(Object*);
            add(args, &arguments);
            tmp_list = true;
        }
        Object* result = function->b_func(args);
        if (tmp_list) dList(args);
        return result;
        
    }
        
    Environment* local = newEnv(function->func->env);

    Ast* params = function->func->params;

    if(arguments != NULL) {
        if(arguments->type == LIST) {
            int argc = getSize(arguments->list);
            for(int index = 0; index < argc; index++) {
                if(params == NULL) break;
                Ast* current = (params->kind == AST_IDENTIFIER_LIST) ? params->identifier_list.first : params;
                
                Object* value;
                LIST_ERROR getErr = getAt(arguments->list, index, Object*, &value);
                if(getErr != LIST_OK) 
                    runtime_error(node->line, "Failed to function '%s' call.\n", node->func_call.name->identifier.name);
            
                env_define(local, current->identifier.name, value);
                if(params->kind == AST_IDENTIFIER_LIST)
                    params = params->identifier_list.next;
                else 
                    params = NULL;

            }
        } else {
            if(params != NULL) {
                Ast* current = (params->kind == AST_IDENTIFIER_LIST) ? params->identifier_list.first : params;
                env_define(local, current->identifier.name, arguments);
            }
        }
    }

    interpreter->call_stack_depth++;
    Object* result = eval(function->func->block, local, interpreter);
    interpreter->call_stack_depth--;

    if(result != NULL && result->type == RETURN) {
        Object* result_value = result->result;
        return result_value;
    }
    return result;
}

/**
 * 単項を実行します。
 */
Object* eval_unary(Ast* node, Environment* env, Interpreter* interpreter)
{
    Object* right = eval(node->unary.expr, env, interpreter);
    const char* op = node->unary.op;

    if(strcmp(op, "+") == 0) {
        if(right->type == INTEGER || right->type == FLOAT)
            return right;
        else 
            runtime_error(node->line, "'+' operator requires a numeric type.\n");
    }

    if(strcmp(op, "-") == 0) {
        if(right->type == INTEGER)
            return new_int(-(right->integer));
        if(right->type == FLOAT)
            return new_float(-(right->real));
        else 
            runtime_error(node->line, "'+' operator requires a numeric type.\n");
    }

    if(strcmp(op, "not") == 0)
        return new_bool(!is_true(right));

    return right;
}

/**
 * リストを再帰的に探索して作成します。
 */
static void make_list(Ast* node, List* list, Environment* env, Interpreter* interpreter)
{
    if(node == NULL) return;

    if(node->kind == AST_VALUE_LIST) {
        make_list(node->value_list.first, list, env, interpreter);
        Object* value = eval(node->value_list.next, env, interpreter);
        if(value != NULL) add(list, &value);   
    } else {
        Object* value = eval(node, env, interpreter);
        if(value != NULL) add(list, &value);
    }
}

/**
 * 値リストを作成します。
 */
Object* eval_value_list(Ast* node, Environment* env, Interpreter* interpreter)
{
    List* list = newList(Object*);
    make_list(node, list, env, interpreter);
    if(getSize(list) == 1) {
        Object* single;
        getAt(list, 0, Object*, &single);
        dList(list);
        return single;
    }
    return new_array(list);
}

/**
 * 配列アクセスを実行します。
 */
Object* eval_array_access(Ast* node, Environment* env, Interpreter* interpreter)
{
    Object* index = eval(node->array_access.index, env, interpreter);

    Object* list = env_get(env, node->array_access.identifier->identifier.name, node->line);

    if(list->type != LIST || index->type != INTEGER) 
        runtime_error(node->line, "Invalid array access to '%s'.\n", node->array_access.identifier->identifier.name);


    Object* result = NULL;
    LIST_ERROR getErr = getAt(list->list, (int)index->integer, Object*, &result);
    if(getErr != LIST_OK) 
        runtime_error(node->line, "Index out of range of '%s'.\n", node->array_access.identifier->identifier.name);

    return result;
}

/**
 * スライスを実行します。
 */
Object* eval_slice(Ast* node, Environment* env, Interpreter* interpreter)
{
    Object* list = env_get(env, node->slice.identifier->identifier.name, node->line);
    if(list->type != LIST)
        runtime_error(node->line, "Slice requires a list.\n");
    List* source = list->list;
    int source_length = getSize(source);

    int start = 0;
    int end = source_length;

    if(node->slice.index->kind == AST_RANGE) {
        if(node->slice.index->range.from != NULL) {
            Object* from = eval(node->slice.index->range.from, env, interpreter);
            start = (int)from->integer;
        }

        if(node->slice.index->range.end != NULL) {
            Object* to = eval(node->slice.index->range.end, env, interpreter);
            end = (int)to->integer;
        }
    } else {
        Object* index = eval(node->slice.index, env, interpreter);
        start = (int)index->integer;
        end = start + 1;
    }

    if(start < 0) start = 0;
    if(end > source_length) end = source_length;
    if(start > end) start = end;

    List* result = newList(Object*);
    for(int index = start; index < end; index++) {
        Object* item;
        getAt(source, index, Object*, &item);
        add(result, &item);
    }
    return new_array(result);
}

/**
 * f文字列を再帰的に探索して作成します。
 */
static void collect_fstring_content(Ast* node, char* buffer, size_t buf_size, Environment* env, Interpreter* interpreter) {
    if (node == NULL) return;

    if (node->kind == AST_FSTRING_PARTS) {
        collect_fstring_content(node->fstring_parts.first, buffer, buf_size, env, interpreter);
        collect_fstring_content(node->fstring_parts.next, buffer, buf_size, env, interpreter);
    } else if (node->kind == AST_FSTRING_TEXT) {
        strncat(buffer, node->fstring_text.text, buf_size - strlen(buffer) - 1);
    } else {
        Object* val = eval(node, env, interpreter);
        char* str = obj_toString(val);
        strncat(buffer, str, buf_size - strlen(buffer) - 1);
        free(str);
    }
}

/**
 * f文字列を実行します。
 */
Object* eval_fstring(Ast* node, Environment* env, Interpreter* interpreter)
{
    char buffer[1 << 16] = "";
    collect_fstring_content(node->fstring.parts, buffer, sizeof(buffer), env, interpreter);
    return new_string(buffer);
}
