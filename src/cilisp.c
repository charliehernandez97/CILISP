#include "cilisp.h"
#include "math.h"

#define RED             "\033[31m"
#define RESET_COLOR     "\033[0m"

/**
* Name: Charlie Hernandez
* Lab: CILISP Project
* Date: 11/9/2020
**/
// yyerror:
// Something went so wrong that the whole program should crash.
// You should basically never call this unless an allocation fails.
// (see the "yyerror("Memory allocation failed!")" calls and do the same.
// This is basically printf, but red, with "\nERROR: " prepended, "\n" appended,
// and an "exit(1);" at the end to crash the program.
// It's called "yyerror" instead of "error" so the parser will use it for errors too.
void yyerror(char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start (args, format);
    vsnprintf (buffer, 255, format, args);

    printf(RED "\nERROR: %s\nExiting...\n" RESET_COLOR, buffer);
    fflush(stdout);

    va_end (args);
    exit(1);
}

// warning:
// Something went mildly wrong (on the user-input level, probably)
// Let the user know what happened and what you're doing about it.
// Then, move on. No big deal, they can enter more inputs. ¯\_(ツ)_/¯
// You should use this pretty often:
//      too many arguments, let them know and ignore the extra
//      too few arguments, let them know and return NAN
//      invalid arguments, let them know and return NAN
//      many more uses to be added as we progress...
// This is basically printf, but red, and with "\nWARNING: " prepended and "\n" appended.
void warning(char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start (args, format);
    vsnprintf (buffer, 255, format, args);

    printf(RED "WARNING: %s\n" RESET_COLOR, buffer);
    fflush(stdout);

    va_end (args);
}

FUNC_TYPE resolveFunc(char *funcName)
{
    // Array of string values for function names.
    // Must be in sync with members of the FUNC_TYPE enum in order for resolveFunc to work.
    // For example, funcNames[NEG_FUNC] should be "neg"
    char *funcNames[] = {
            "neg",
            "abs",
            "add",
            "sub",
            "mult",
            "div",
            "remainder",
            "exp",
            "exp2",
            "pow",
            "log",
            "sqrt",
            "cbrt",
            "hypot",
            "max",
            "min",
            "custom",

            // TODO complete the array
            // the empty string below must remain the last element
            ""
    };
    int i = 0;
    while (funcNames[i][0] != '\0')
    {
        if (strcmp(funcNames[i], funcName) == 0)
        {
            return i;
        }
        i++;
    }
    return CUSTOM_FUNC;
}

AST_NODE *createNumberNode(double value, NUM_TYPE type)
{
    AST_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
    {
        yyerror("Memory allocation failed!");
        exit(1);
    }

    // TODO complete the function - done
    // Populate "node", the AST_NODE * created above with the argument data.
    // node is a generic AST_NODE, don't forget to specify it is of type NUMBER_NODE
    node->data.number.value = value;
    node->data.number.type = type;
    node->type = NUM_NODE_TYPE;
    //printf("%f\n", node->data.number.value);

    return node;
}


AST_NODE *createFunctionNode(FUNC_TYPE func, AST_NODE *opList)
{
    AST_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
    {
        yyerror("Memory allocation failed!");
        exit(1);
    }

    // TODO complete the function - done
    // Populate the allocated AST_NODE *node's data
    node->type = FUNC_NODE_TYPE;
    node->data.function.func = func;
    node->data.function.opList = opList;

    return node;
}

AST_NODE *addExpressionToList(AST_NODE *newExpr, AST_NODE *exprList)
{

    newExpr->next = exprList;
    return newExpr;
}

RET_VAL *evalNeg(AST_NODE *node)
{
    if(!node)
    {
        warning("No operands in Neg function");
        return &NAN_RET_VAL;
    }
    else if(node->next)
    {
        warning("neg called with extra (ignored) operands!");
    }

    RET_VAL *result = malloc(sizeof(RET_VAL));
    result->value = -node->data.number.value;
    result->type = node->data.number.type;
    return result;

}

RET_VAL *evalAbs(AST_NODE *node)
{

    if(!node)
    {
        warning("No operands in Neg function");
        return &NAN_RET_VAL;
    }
    else if(node->next)
    {
        warning("Too many operands in Abs function");

    }
    RET_VAL *result = malloc(sizeof(RET_VAL));
    result->value = fabs(node->data.number.value);
    result->type = node->data.number.type;
    return result;
}

RET_VAL *evalAdd(AST_NODE *node)
{
    RET_VAL *result = malloc(sizeof(RET_VAL));
    RET_VAL *result2 = malloc(sizeof(RET_VAL));
    result->value = 0;

    if(node == NULL)
    {
        warning("WARNING: No operands detected");
        //result->value = 0;
        result->type = INT_TYPE;
        return result;
    }


    while(node != NULL)
    {
        *result2 = eval(node);

        result->value += result2->value;
        result->type = result2->type || result->type;
        //printf("add %f", node->data.number.value);
        node = node->next;
    }

    return result;
}


RET_VAL *evalSub(AST_NODE *node)
{
    RET_VAL *result = malloc(sizeof(RET_VAL));
    RET_VAL *result2= malloc(sizeof(RET_VAL));
    if(!node)
    {
        warning("No operands in Sub function");
        return &NAN_RET_VAL;
    }
    else if(!node->next)
    {
        warning("WARNING: Sub called with only one arg!");
        return &NAN_RET_VAL;
    }
    else if(node->next->next)
    {
        warning("Sub called with extra (ignored) operands!");
    }

    *result = eval(node);
    *result2 = eval(node->next);
    result->value -= result2->value;
    result->type = result->type || result2->type;
    return result;



}

// TODO - DEBUGGING REQUIRED - DOES NOT PRINT RIGHT NUM_TYPE IN SPECIFIC CIRCUMSTANCES
RET_VAL *evalMult(AST_NODE *node)
{
    RET_VAL *result = malloc(sizeof(RET_VAL));
    RET_VAL *result2 = malloc(sizeof(RET_VAL));

    result->value = 1;

    if(node == NULL)
    {
        warning("WARNING: No operands detected");
        result->type = INT_TYPE;
        return result;
    }

    //result->type = node->data.number.type || node->next->data.number.type;
    while(node != NULL)
    {
        *result2 = eval(node);

        result->value *= result2->value;
        result->type = result2->type || result->type;
        //printf("add %f", node->data.number.value);
        node = node->next;

        //printf("add %f", node->data.number.value);
    }
    return result;
}

RET_VAL *evalDiv(AST_NODE *node)
{
    RET_VAL *result = malloc(sizeof(RET_VAL));
    RET_VAL *result2 = malloc(sizeof(RET_VAL));

    if(!node)
    {
        warning("No operands in div function");
        return &NAN_RET_VAL;
    }
    else if(!node->next)
    {
        warning("WARNING: div called with only one arg!");
        return &NAN_RET_VAL;
    }
    else if(node->next->next)
    {
        warning("Sub called with extra (ignored) operands!");
    }

    if(node->next->data.number.value == 0)
    {
        warning("You cannot divide by zero!");
        return &NAN_RET_VAL;
    }
    *result = eval(node);
    *result2 = eval(node->next);
    result->value /= result2->value;
    result->type = result->type || result2->type;
    return result;

    return result;
}

// TODO - DEBUGGING
RET_VAL *evalRemainder(AST_NODE *node)
{
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if(!node)
    {
        warning("No operands in remainder function");
        return &NAN_RET_VAL;
    }
    else if(!node->next)
    {
        warning("WARNING: Remainder called with only one arg!");
        return &NAN_RET_VAL;
    }
    else if(node->next->next)
    {
        warning("Remainder called with extra (ignored) operands!");
    }
    result->value = fmod(node->data.number.value, node->next->data.number.value);
    result->type = node->data.number.type || node->next->data.number.type;
    return result;
}

RET_VAL *evalExp(AST_NODE *node)
{
    if(!node)
    {
        warning("No operands in Exp function");
        return &NAN_RET_VAL;
    }
    else if(node->next)
    {
        warning("Exp called with extra (ignored) operands!");

    }

    RET_VAL *result = malloc(sizeof(RET_VAL));
    *result = eval(node);
    result->value = exp(result->value);
    result->type = DOUBLE_TYPE;
    return result;
}

RET_VAL *evalExp2(AST_NODE *node)
{
    if(!node)
    {
        warning("No operands in Exp2 function");
        return &NAN_RET_VAL;
    }
    else if(node->next)
    {
        warning("Exp2 called with extra (ignored) operands!");

    }

    RET_VAL *result = malloc(sizeof(RET_VAL));
    *result = eval(node);
    result->value = exp2(result->value);
    if (node->data.number.value < 0)
        result->type = DOUBLE_TYPE;
//    else
//        result->type = result->type;

    return result;
}

RET_VAL *evalPow(AST_NODE *node)
{
    RET_VAL *result = malloc(sizeof(RET_VAL));
    RET_VAL *result2 = malloc(sizeof(RET_VAL));


    if(!node)
    {
        warning("No operands in Pow function");
        return &NAN_RET_VAL;
    }
    else if(!node->next)
    {
        warning("Pow called with no second operand!");
        return &NAN_RET_VAL;
    }
    else if(node->next->next)
    {
        warning(" Pow called with extra (ignored) operands!");
    }

    *result = eval(node);
    *result2 = eval(node->next);
    result->type = result->type || result2->type;
    result->value = pow(result->value, result2->value);

    return result;
}

RET_VAL *evalLog(AST_NODE *node)
{
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if(!node)
    {
        warning("No operands in Log function");
        return &NAN_RET_VAL;
    }
    else if(node->next)
    {
        warning("Log called with extra (ignored) operands!");

    }
    *result = eval(node);
    result->value = log(result->value);
    result->type = DOUBLE_TYPE;
    return result;
}

RET_VAL *evalSqrt(AST_NODE *node)
{
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if(!node)
    {
        warning("No operands in Sqrt function");
        return &NAN_RET_VAL;
    }
    else if(node->next)
    {
        warning("Sqrt called with extra (ignored) operands!");

    }

    *result = eval(node);
    result->value = sqrt(result->value);
    result->type = DOUBLE_TYPE;
    return result;
}

RET_VAL *evalCbrt(AST_NODE *node)
{
    RET_VAL *result = malloc(sizeof(RET_VAL));
    if(!node)
    {
        warning("No operands in Cbrt function");
        return &NAN_RET_VAL;
    }
    else if(node->next)
    {
        warning("Cbrt called with extra (ignored) operands!");

    }

    *result = eval(node);
    result->value = cbrt(result->value);
    result->type = DOUBLE_TYPE;
    return result;
}

RET_VAL *evalHypot(AST_NODE *node)
{
    RET_VAL *result = malloc(sizeof(RET_VAL));
    RET_VAL *result2 = malloc(sizeof(RET_VAL));
    result->value = 0;

    if(node == NULL)
    {
        warning("WARNING: No operands detected!");
        return &NAN_RET_VAL;
    }


    while(node != NULL)
    {
        *result2 = eval(node);
        result2->value = pow(result2->value, 2);
        result->value += result2->value;

        //printf("add %f", node->data.number.value);

        node = node->next;
    }
    result->type = DOUBLE_TYPE;
    result->value = sqrt(result->value);

    return result;
}

RET_VAL *evalMax(AST_NODE *node)
{
    RET_VAL *result = malloc(sizeof(RET_VAL));
    RET_VAL *result2 = malloc(sizeof(RET_VAL));
    result->value = 0;

    if(!node)
    {
        warning("No operands detected!");
        return &NAN_RET_VAL;
    }
    while(node != NULL)
    {
        *result2 = eval(node);
        if(result->value < result2->value)
        {
            result->value = result2->value;
            result->type = result2->type;
        }

        node = node->next;
    }
    return result;
}

RET_VAL *evalMin(AST_NODE *node)
{
    RET_VAL *result = malloc(sizeof(RET_VAL));
    RET_VAL *result2 = malloc(sizeof(RET_VAL));
    result->value = 0;

    if(!node)
    {
        warning("No operands detected!");
        return &NAN_RET_VAL;
    }
    while(node != NULL)
    {
        *result2 = eval(node);
        if(result->value > result2->value)
        {
            result->value = result2->value;
            result->type = result2->type;
        }

        node = node->next;
    }
    return result;
}


RET_VAL evalFuncNode(AST_NODE *node)
{
    if (!node)
    {
        yyerror("NULL ast node passed into evalFuncNode!");
        return NAN_RET_VAL; // unreachable but kills a clang-tidy warning
    }

    // TODO complete the function - done
    // HINT:
    // the helper functions that it calls will need to be defined above it
    // because they are not declared in the .h file (and should not be)
    RET_VAL *result;
    switch(node->data.function.func)
    {
        case NEG_FUNC:
            result = evalNeg(node->data.function.opList);
            break;
        case ADD_FUNC:
            result = evalAdd(node->data.function.opList);
            break;
        case ABS_FUNC:
            result = evalAbs(node->data.function.opList);
            break;
        case SUB_FUNC:
            result = evalSub(node->data.function.opList);
            break;
        case MULT_FUNC:
            result = evalMult(node->data.function.opList);
            break;
        case DIV_FUNC:
            result = evalDiv(node->data.function.opList);
            break;
        case REM_FUNC:
            result = evalRemainder(node->data.function.opList);
            break;
        case EXP_FUNC:
            result = evalExp(node->data.function.opList);
            break;
        case EXP2_FUNC:
            result = evalExp2(node->data.function.opList);
            break;
        case POW_FUNC:
            result = evalPow(node->data.function.opList);
            break;
        case LOG_FUNC:
            result = evalLog(node->data.function.opList);
            break;
        case SQRT_FUNC:
            result = evalSqrt(node->data.function.opList);
            break;
        case CBRT_FUNC:
            result = evalCbrt(node->data.function.opList);
            break;
        case HYPOT_FUNC:
            result = evalHypot(node->data.function.opList);
            break;
        case MAX_FUNC:
            result = evalMax(node->data.function.opList);
            break;
        case MIN_FUNC:
            result = evalMin(node->data.function.opList);
            break;
        default:
            warning("WARNING: Function not recognized!");


    }

    return *result;
}

RET_VAL evalNumNode(AST_NODE *node)
{
    if (!node)
    {
        yyerror("NULL ast node passed into evalNumNode!");
        return NAN_RET_VAL;
    }

    // TODO complete the function - done

    return node->data.number;
}

RET_VAL eval(AST_NODE *node)
{
    if (!node)
    {
        yyerror("NULL ast node passed into eval!");
        return NAN_RET_VAL;
    }

    // TODO complete the function - done
    switch (node->type) {
        case NUM_NODE_TYPE:
            return evalNumNode(node);
        case FUNC_NODE_TYPE:
            return evalFuncNode(node);
        default:
            yyerror("TYPE not recognized!");
            return NAN_RET_VAL;

    }


}

// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val)
{
    switch (val.type)
    {
        case INT_TYPE:
            printf("Integer : %.lf\n", val.value);
            break;
        case DOUBLE_TYPE:
            printf("Double : %lf\n", val.value);
            break;
        default:
            printf("No Type : %lf\n", val.value);
            break;
    }
}



void freeNode(AST_NODE *node)
{
    if (!node)
    {
        return;
    }

    // TODO complete the function

    // look through the AST_NODE struct, decide what
    // referenced data should have freeNode called on it
    // (hint: it might be part of an s_expr_list, with more
    // nodes following it in the list)

    // if this node is FUNC_TYPE, it might have some operands
    // to free as well (but this should probably be done in
    // a call to another function, named something like
    // freeFunctionNode)

    // and, finally,
    free(node);
}
