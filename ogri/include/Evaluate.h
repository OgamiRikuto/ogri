/**
 * 実行します。
 * 抽象木を再帰的に探索して実行します。
 */
#ifndef __EVALUATE_H__
#define __EVALUATE_H__

typedef struct Ast Ast;
typedef struct environment Environment;
typedef struct object Object;

typedef struct interpreter Interpreter;

struct interpreter {
    int loop_level;
    int call_stack_depth;
};

int evaluate(Ast*);
Object* eval(Ast*, Environment*, Interpreter*);
Object* eval_statements(Ast*, Environment*, Interpreter*);
Object* eval_block(Ast*, Environment*, Interpreter*);
Object* eval_repeat(Ast*, Environment*, Interpreter*);
Object* eval_repeat_until(Ast*, Environment*, Interpreter*);
Object* eval_when(Ast*, Environment*, Interpreter*);
Object* eval_otherwhen(Ast*, Environment*, Interpreter*);
Object* eval_logical(Ast*, Environment*, Interpreter*);
Object* eval_binop(Ast*, Environment*, Interpreter*);
Object* eval_assign(Ast*, Object*, Environment*, Interpreter*);
Object* eval_func_def(Ast*, Environment*, Interpreter*);
Object* eval_func_call(Ast*, Environment*, Interpreter*);
Object* eval_unary(Ast*, Environment*, Interpreter*);
Object* eval_value_list(Ast*, Environment*, Interpreter*);
Object* eval_array_access(Ast*, Environment*, Interpreter*);
Object* eval_slice(Ast*, Environment*, Interpreter*);
Object* eval_fstring(Ast*, Environment*, Interpreter*);

#endif /* __EVALUATE_H__ */
