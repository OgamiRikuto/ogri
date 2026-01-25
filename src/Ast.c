
#include "defs.h"

/**
 * 抽象木のメモリを確保し、種類を設定します。
 */
Ast* new_ast(AstKind kind) 
{
    Ast* node = malloc(sizeof(Ast));
    node->kind = kind;
    return node;
}
/**
 * 文の抽象木を作成します。
 */
Ast* ast_stmts(Ast* first, Ast* next, int line)
{
    Ast* node = new_ast(AST_STATEMENTS);
    node->line = line;
    node->list.first = first;
    node->list.next = next;
    return node;
}

/**
 * when文の抽象木を作成します。
 */
Ast* ast_when(Ast* cond, Ast* block, Ast* otherwhen, Ast* other, int line)
{
    Ast* node = new_ast(AST_WHEN);
    node->line = line;
    node->when_stmt.cond = cond;
    node->when_stmt.then_block = block;
    node->when_stmt.otherwhen_list = otherwhen;
    node->when_stmt.other_block = other;
    return node;
}

/**
 * otherwise_when文の抽象木を作成します。
 */
Ast* ast_otherwhen(Ast* cond, Ast* block, Ast* next, int line)
{
    Ast* node = new_ast(AST_OTHERWHEN);
    node->line = line;
    node->otherwhen.cond = cond;
    node->otherwhen.block = block;
    node->otherwhen.next = next;
    return node;
} 

/**
 * repeat_until文の抽象木を作成します。
 */
Ast* ast_repeat_until(Ast* cond, Ast* block, int line)
{
    Ast* node = new_ast(AST_REPEAT_UNTIL);
    node->line = line;
    node->repeat_until_stmt.cond = cond;
    node->repeat_until_stmt.block = block;
    return node;
}

/**
 * repeat文の抽象木を作成します。
 */
Ast* ast_repeat(Ast* identifier, Ast* collection, Ast* block, int line)
{
    Ast* node = new_ast(AST_REPEAT);
    node->line = line;
    node->repeat_stmt.identifier = identifier;
    node->repeat_stmt.collection = collection;
    node->repeat_stmt.block = block;
    return node;
}

/**
 * 関数定義の抽象木を作成します。
 */
Ast* ast_func_def(Ast* name, Ast* params, Ast* body, int line)
{
    Ast* node = new_ast(AST_FUNC_DEF);
    node->line = line;
    node->func_def.name = name;
    node->func_def.params = params;
    node->func_def.body = body;
    return node;
}

/**
 * 代入文の抽象木を作成します。
 */
Ast* ast_assign(Ast* left, Ast* right, bool is_are, int line)
{
    Ast* node = new_ast(AST_ASSIGN);
    node->line = line;
    node->assign.left = left;
    node->assign.right = right;
    node->assign.is_are = is_are;
    return node;
}

/**
 * 配列代入文の抽象木を作成します。
 */
Ast* ast_array_assign(Ast* left, Ast* right, int line)
{
    Ast* node = new_ast(AST_ARRAY_ASSIGN);
    node->line = line;
    node->assign_array.left = left;
    node->assign_array.right = right;
    return node;
}

/**
 * ブロックの抽象木を作成します。
 */
Ast* ast_block(Ast* statements, int line)
{
    Ast* node = new_ast(AST_BLOCK);
    node->line = line;
    node->block.statements = statements;
    return node;
}

/**
 * retuen文の抽象木を作成します。
 */
Ast* ast_return(Ast* expr, int line)
{
    Ast* node = new_ast(AST_RETURN);
    node->line = line;
    node->return_stmt.expr = expr;
    return node;
}

/**
 * break文の抽象木を作成します。
 */
Ast* ast_break(int line)
{
    Ast* node = new_ast(AST_BREAK);
    node->line = line;
    return node;
}

/**
 * continue文の抽象木を作成します。
 */
Ast* ast_continue(int line)
{
    Ast* node = new_ast(AST_CONTINUE);
    node->line = line;
    return node;
}

/**
 * 関数呼び出し文の抽象木を作成します。
 */
Ast* ast_func_call(Ast* name, Ast* args, int line)
{
    Ast* node = new_ast(AST_FUNC_CALL);
    node->line = line;
    node->func_call.name = name;
    node->func_call.args = args;
    return node;
}

/**
 * 二項演算の抽象木を作成します。
 */
Ast* ast_binop(const char* op, Ast* left, Ast* right, int line)
{
    Ast* node = new_ast(AST_BINOP);
    node->line = line;
    node->binop.op = strdup(op);
    node->binop.left = left;
    node->binop.right = right;
    return node;
}

/**
 * 単項の抽象木を作成します。
 */
Ast* ast_unary(const char* op, Ast* expr, int line)
{
    Ast* node = new_ast(AST_UNARY);
    node->line = line;
    node->unary.op = strdup(op);
    node->unary.expr = expr;
    return node;
}

/**
 * 識別子の抽象木を作成します。
 */
Ast* ast_identifier(const char* name, int line)
{
    Ast* node = new_ast(AST_IDENTIFIER);
    node->line = line;
    node->identifier.name = strdup(name);
    return node;
}

/**
 * 整数の抽象木を作成します。
 */
Ast* ast_integer(const char* value, int line)
{
    Ast* node = new_ast(AST_INTEGER);
    node->line = line;
    node->integer.value = atol(value);
    return node;
}

/**
 * 実数の抽象木を作成します。
 */
Ast* ast_real(const char* value, int line)
{
    Ast* node = new_ast(AST_FLOAT);
    node->line = line;
    node->real.value = strtod(value, NULL);
    return node;
}

/**
 * 文字列の抽象木を作成します。
 */
Ast* ast_string(const char* string, int line)
{
    Ast* node = new_ast(AST_STRING);
    node->line = line;
    node->string.value = strdup(string);
    return node;
}

/**
 * f文字列の抽象木を作成します。
 */
Ast* ast_fstring(Ast* parts, int line)
{
    Ast* node = new_ast(AST_FSTRING);
    node->line = line;
    node->fstring.parts = parts;
    return node;
}

/**
 * f文字列部分
 */
Ast* ast_fstring_parts(Ast* first, Ast* next, int line)
{
    Ast* node = new_ast(AST_FSTRING_PARTS);
    node->line = line;
    node->fstring_parts.first = first;
    node->fstring_parts.next = next;
    return node;
}

/**
 * f文字列テキストの抽象木を作成します。
 */
Ast* ast_fstring_text(const char* text, int line)
{
    Ast* node = new_ast(AST_FSTRING_TEXT);
    node->line = line;
    node->fstring_text.text = strdup(text);
    return node;
}

/**
 * 値リストの抽象木を作成します。
 */
Ast* ast_value_list(Ast* first, Ast* next, int line)
{
    Ast* node = new_ast(AST_VALUE_LIST);
    node->line = line;
    node->value_list.first = first;
    node->value_list.next = next;
    return node;
}
/**
 * 識別子リストの抽象木を作成します。
 */
Ast* ast_identifier_list(Ast* first, Ast* next, int line)
{
    Ast* node = new_ast(AST_IDENTIFIER_LIST);
    node->line = line;
    node->identifier_list.first = first;
    node->identifier_list.next = next;
    return node;
}

/**
 * 配列アクセスの抽象木を作成します。
 */
Ast* ast_array_access(Ast* index, Ast* identifier, int line)
{
    Ast* node = new_ast(AST_ARRAY_ACCESS);
    node->line = line;
    node->array_access.index = index;
    node->array_access.identifier =identifier;
    return node;
}

/**
 * スライスの抽象木を作成します.
 */
Ast* ast_slice(Ast* index, Ast* identifier, int line)
{
    Ast* node = new_ast(AST_SLICE);
    node->line = line;
    node->slice.index = index;
    node->slice.identifier = identifier;
    return node;
}

/**
 * スライス範囲の抽象木を作成します.
 */
Ast* ast_range(Ast* from, Ast* end, int line)
{
    Ast* node = new_ast(AST_RANGE);
    node->line = line;
    node->range.from = from;
    node->range.end = end;
    return node;
}

/**
 * インデントを出力します。
 */
static void indent(int num)
{
    for(int index = 0; index < num; index++) printf("    ");
}

/**
 * 抽象木を出力します。
 */
void print(Ast* node)
{
    ast_dump(node, 1);
    printf("\n");
}

/**
 * 抽象木を再帰的に探索しながら出力します。
 */
void ast_dump(Ast* node, int depth)
{
    const char* kind[] = {  
    "STATEMENTS",
    "WHEN",
    "OTHERWHEN",
    "REPEAT",
    "REPEAT_UNTIL",
    "FUNC_DEF",
    "ASSIGN",
    "ARRAY_ASSIGN",
    "RETURN",
    "BREAK",
    "CONTINUE",
    "FUNC_CALL",
    "BLOCK",
    "BINOP",
    "UNARY",
    "IDENTIFIER",
    "INTEGER",
    "FLOAT",
    "STRING",
    "FSTRING",
    "FSTRING_PARTS",
    "FSTRING_TEXT",
    "VALUE_LIST",
    "IDENTIFIER_LIST",
    "ARRAY_ACCESS",
    "SLICE",
    "RANGE"
    };

    printf("\n");
    if(node == NULL) {
        indent(depth);
        printf("(null)");
        return;
    }

    indent(depth);
    printf("%s ", kind[node->kind]);

    switch(node->kind) {
        case AST_STATEMENTS:
            ast_dump(node->list.first, depth+1);
            ast_dump(node->list.next, depth+1);
            break;
        case AST_WHEN:
            ast_dump(node->when_stmt.cond, depth+1);
            ast_dump(node->when_stmt.then_block, depth+1);
            ast_dump(node->when_stmt.otherwhen_list, depth+1);
            ast_dump(node->when_stmt.other_block, depth+1);
            break;
        case AST_OTHERWHEN:
            ast_dump(node->otherwhen.cond, depth+1);
            ast_dump(node->otherwhen.block, depth+1);
            ast_dump(node->otherwhen.next, depth+1);
            break;
        case AST_REPEAT_UNTIL:
            ast_dump(node->repeat_until_stmt.cond, depth+1);
            ast_dump(node->repeat_until_stmt.block, depth+1);
            break;
        case AST_REPEAT:
            ast_dump(node->repeat_stmt.identifier, depth+1);
            ast_dump(node->repeat_stmt.collection, depth+1);
            ast_dump(node->repeat_stmt.block, depth+1);
            break;
        case AST_FUNC_DEF:
            ast_dump(node->func_def.name, depth+1);
            ast_dump(node->func_def.params, depth+1);
            ast_dump(node->func_def.body, depth+1);
            break;
        case AST_ASSIGN:
            ast_dump(node->assign.left, depth+1);
            ast_dump(node->assign.right, depth+1);
            break;
        case AST_ARRAY_ASSIGN:
            ast_dump(node->assign_array.left, depth+1);
            ast_dump(node->assign_array.right, depth+1);
            break;
        case AST_RETURN:
            ast_dump(node->return_stmt.expr, depth+1);
            break;
        case AST_BREAK:
        case AST_CONTINUE:
            break;
        case AST_BLOCK:
            ast_dump(node->block.statements, depth+1);
            break;
        case AST_IDENTIFIER:
            printf("%s", node->identifier.name);
            break;
        case AST_INTEGER:
            printf("%ld", node->integer.value);
            break;
        case AST_FLOAT:
            printf("%f", node->real.value);
            break;
        case AST_STRING:
            printf("%s", node->string.value);
            break;
        case AST_FSTRING:
            ast_dump(node->fstring.parts, depth+1);
            break;
        case AST_FSTRING_PARTS:
            ast_dump(node->fstring_parts.first, depth+1);
            ast_dump(node->fstring_parts.next, depth+1);
            break;
        case AST_FSTRING_TEXT:
            printf("%s", node->fstring_text.text);
            break;
        case AST_BINOP:
            printf("%s", node->binop.op);
            ast_dump(node->binop.left, depth+1);
            ast_dump(node->binop.right, depth+1);
            break;
        case AST_UNARY:
            printf("%s", node->unary.op);
            ast_dump(node->unary.expr, depth+1);
            break;
        case AST_FUNC_CALL:
            ast_dump(node->func_call.name, depth+1);
            ast_dump(node->func_call.args, depth+1);
            break;
        case AST_ARRAY_ACCESS:
            ast_dump(node->array_access.index, depth+1);
            ast_dump(node->array_access.identifier, depth+1);
            break;
        case AST_VALUE_LIST:
            ast_dump(node->value_list.first, depth+1);
            ast_dump(node->value_list.next, depth+1);
            break;
        case AST_IDENTIFIER_LIST:
            ast_dump(node->identifier_list.first, depth+1);
            ast_dump(node->identifier_list.next, depth+1);
            break;
        case AST_SLICE:
            ast_dump(node->slice.index, depth+1);
            ast_dump(node->slice.identifier, depth+1);
            break;
        case AST_RANGE:
            ast_dump(node->range.from, depth+1);
            ast_dump(node->range.end, depth+1);
            break;
    }
}
