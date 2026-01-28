/**
 * 抽象木を作成します。
 */
#ifndef __AST_H__
#define __AST_H__

#include <stdbool.h>

typedef enum {
    AST_STATEMENTS,
    // compound_stmt
    AST_WHEN,
    AST_OTHERWHEN,
    AST_REPEAT,
    AST_REPEAT_UNTIL,
    AST_FUNC_DEF,
    // simple_stmt
    AST_ASSIGN,
    AST_ARRAY_ASSIGN,
    AST_RETURN,
    AST_BREAK,
    AST_CONTINUE,
    AST_FUNC_CALL,

    AST_BLOCK,
    // expressions
    AST_BINOP,
    AST_UNARY,
    AST_IDENTIFIER,
    AST_INTEGER,
    AST_FLOAT,
    AST_STRING,
    AST_FSTRING,
    AST_FSTRING_PARTS,
    AST_FSTRING_TEXT,
    AST_VALUE_LIST,
    AST_IDENTIFIER_LIST,
    AST_ARRAY_ACCESS,
    AST_SLICE,
    AST_RANGE
} AstKind;

typedef struct Ast Ast;

struct Ast {
    AstKind kind;
    int line;
    union {
        // statements
        struct {
            Ast* first;
            Ast* next;
        } list;

        // when
        struct {
            Ast* cond;
            Ast* then_block;
            Ast* otherwhen_list;
            Ast* other_block;
        } when_stmt;

        struct {
            Ast* cond;
            Ast* block;
            Ast* next;
        } otherwhen;

        // repeat
        struct {
            Ast* cond;
            Ast* block;
        } repeat_until_stmt;

        struct {
            Ast* identifier;
            Ast* collection;
            Ast* block;
        } repeat_stmt;

        // define
        struct {
            Ast* name;
            Ast* params;
            Ast* body;
        } func_def;

        struct {
            Ast* statements;
        } block;

         // assign
        struct {
            Ast* left;
            Ast* right;
            bool is_are;
        } assign;

        struct {
            Ast* left;
            Ast* right;
        } assign_array;

        struct {
            Ast* expr;
        } return_stmt;

        struct {
            Ast* name;
            Ast* args;
        } func_call;

        // expressions
        struct {
            char* op;
            Ast* left;
            Ast* right;
        } binop;

        struct {
            char* op;
            Ast* expr;
        } unary;

        // literals
        struct {
            const char* name;
        } identifier;

        struct {
            long value;
        } integer;

        struct {
            double value;
        } real;

        struct {
            char* value;
        } string;

        struct {
            Ast* parts;
        } fstring;

        struct {
            Ast* first;
            Ast* next;
        } fstring_parts;

        struct {
            const char* text;
        } fstring_text;

        struct {
            Ast* first;
            Ast* next;
        } value_list;

        struct {
            Ast* first;
            Ast* next;
        } identifier_list;

        // array
        struct {
            Ast* index;
            Ast* identifier;
        } array_access;

        struct {
            Ast* index;
            Ast* identifier;
        } slice;

        struct {
            Ast* from;
            Ast* end;
        } range;
    };
};


Ast* new_ast(AstKind);
Ast* ast_stmts(Ast*, Ast*, int);
Ast* ast_when(Ast*, Ast*, Ast*, Ast*, int);
Ast* ast_otherwhen(Ast*, Ast*, Ast*, int);
Ast* ast_repeat_until(Ast*, Ast*, int);
Ast* ast_repeat(Ast*, Ast*, Ast*, int);
Ast* ast_func_def(Ast*, Ast*, Ast*, int);
Ast* ast_assign(Ast*, Ast*, bool, int);
Ast* ast_array_assign(Ast*, Ast*, int);
Ast* ast_block(Ast*, int);
Ast* ast_return(Ast*, int);
Ast* ast_break(int);
Ast* ast_continue(int);
Ast* ast_func_call(Ast*, Ast*, int);
Ast* ast_binop(const char*, Ast*, Ast*, int);
Ast* ast_unary(const char*, Ast*, int);
Ast* ast_identifier(const char*, int);
Ast* ast_integer(const char*, int);
Ast* ast_real(const char*, int);
Ast* ast_string(const char*, int);
Ast* ast_fstring(Ast*, int);
Ast* ast_fstring_parts(Ast*, Ast*, int);
Ast* ast_fstring_text(const char*, int);
Ast* ast_value_list(Ast*, Ast*, int);
Ast* ast_identifier_list(Ast*, Ast*, int);
Ast* ast_array_access(Ast*, Ast*, int);
Ast* ast_slice(Ast*, Ast*, int);
Ast* ast_range(Ast*, Ast*, int);
void print(Ast*);
void ast_dump(Ast*, int);

#endif /* __AST_H__ */
