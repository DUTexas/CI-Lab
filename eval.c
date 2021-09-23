/**************************************************************************
 * C S 429 EEL interpreter
 * 
 * eval.c - This file contains the skeleton of functions to be implemented by
 * you. When completed, it will contain the code used to evaluate an expression
 * based on its AST.
 * 
 * Copyright (c) 2021. S. Chatterjee, X. Shen, T. Byrd. All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#include "ci.h"

extern bool is_binop(token_t);
extern bool is_unop(token_t);
char *strrev(char *str);

/* infer_type() - set the type of a non-root node based on the types of children
 * Parameter: A node pointer, possibly NULL.
 * Return value: None.
 * Side effect: The type field of the node is updated.
 * (STUDENT TODO)
 */

static void infer_type(node_t *nptr) {
    if (nptr == NULL) return;
    // check running status
    if (terminate || ignore_input) return;
    
    if (nptr->type == ID_TYPE) {
        infer_type(nptr->children[1]);
    } 
    // check for assignment
    if (nptr->node_type == NT_INTERNAL){

        //If no child node node_type should not be NT_Interal
        if (nptr->children[0] == NULL) {
            logging(LOG_ERROR, "failed to find child node, possibly misslabled as internal node");
            return;
        }
        
        if (is_unop(nptr->tok)){
            switch (nptr->tok){
            case TOK_UMINUS:
                if (nptr->children[0]->type == BOOL_TYPE) nptr->type = BOOL_TYPE;
                if (nptr->children[0]->type == STRING_TYPE) nptr->type = STRING_TYPE;
                if (nptr->children[0]->type == INT_TYPE) nptr->type = INT_TYPE;
                break;
            case TOK_NOT:
                if (nptr->children[0]->type == INT_TYPE) nptr->type = INT_TYPE;
                if (nptr->children[0]->type == BOOL_TYPE) nptr->type = BOOL_TYPE;
                break;
            default:
                handle_error(ERR_TYPE);
                break;
            }
        }
        
        if (is_binop(nptr->tok)){
            //get child types
            infer_type(nptr->children[0]);
            infer_type(nptr->children[1]);
            bool is2Boo = (nptr->children[0]->type == BOOL_TYPE  ) && (nptr->children[1]->type == BOOL_TYPE  );
            bool is2Int = (nptr->children[0]->type == INT_TYPE   ) && (nptr->children[1]->type == INT_TYPE   );
            bool is2Str = (nptr->children[0]->type == STRING_TYPE) && (nptr->children[1]->type == STRING_TYPE);
            bool isSInt = (nptr->children[0]->type == STRING_TYPE) && (nptr->children[1]->type == INT_TYPE   );

            switch (nptr->tok){ 
            case TOK_PLUS:
                if (is2Str) nptr->type = STRING_TYPE;
                if (is2Int) nptr->type = INT_TYPE;
                break;
            case TOK_BMINUS:
                if (is2Int) nptr->type = INT_TYPE;
                break;
            case TOK_TIMES:
                if (is2Int) nptr->type = INT_TYPE;
                if (isSInt) nptr->type = STRING_TYPE;
                break;
            case TOK_DIV:
                if (is2Int) nptr->type = INT_TYPE;
                break;
            case TOK_MOD:
                if (is2Int) nptr->type = INT_TYPE;
                break;
            case TOK_AND:
                if (is2Boo) nptr->type = BOOL_TYPE;
                break;
            case TOK_OR:
                if (is2Boo) nptr->type = BOOL_TYPE;
                break;
            case TOK_LT:
                if (is2Int) nptr->type = BOOL_TYPE;
                if (is2Str) nptr->type = STRING_TYPE;
                break;
            case TOK_GT:
                if (is2Int) nptr->type = BOOL_TYPE;
                if (is2Str) nptr->type = STRING_TYPE;
                break;
            case TOK_EQ:
                if (is2Str) nptr->type = STRING_TYPE;
                if (is2Int) nptr->type = BOOL_TYPE;
                if (is2Boo) nptr->type = BOOL_TYPE;
                break;
            default:
                //type error
                handle_error(ERR_TYPE);
                break;
            } 
        }
        
        // Ternary Operator

        //evaluate ternary expression

        //ternary type is the type of the child that is selected

    } else if (nptr -> node_type == NT_LEAF){
        switch (nptr -> tok)
        {
        case TOK_NUM:
            nptr->type = INT_TYPE;
            break;
        case TOK_FALSE: case TOK_TRUE:
            nptr->type = BOOL_TYPE;
            break;
        case TOK_STR:
            nptr->type = STRING_TYPE;
            break;
        case TOK_FMT_SPEC:
            nptr->type = FMT_TYPE;
            break;
        case TOK_ID:
            nptr->type = ID_TYPE;
            break;
        default:
            handle_error(ERR_TYPE);
            break;
        }
    } else logging(LOG_ERROR, "Node type not set at infrance");
    return;
}

/* infer_root() - set the type of the root node based on the types of children
 * Parameter: A pointer to a root node, possibly NULL.
 * Return value: None.
 * Side effect: The type field of the node is updated. 
 */

static void infer_root(node_t *nptr) {
    if (nptr == NULL) return;
    // check running status
    if (terminate || ignore_input) return;

    // check for assignment
    if (nptr->type == ID_TYPE) {
        infer_type(nptr->children[1]);
    } else {
        for (int i = 0; i < 3; ++i) {
            infer_type(nptr->children[i]);
        }
        if (nptr->children[0] == NULL) {
            logging(LOG_ERROR, "failed to find child node");
            return;
        }
        nptr->type = nptr->children[0]->type;
    }
    return;
}

/* eval_node() - set the value of a non-root node based on the values of children
 * Parameter: A node pointer, possibly NULL.
 * Return value: None.
 * Side effect: The val field of the node is updated.
 * (STUDENT TODO) 
 */

static void eval_node(node_t *nptr) {
    if (nptr == NULL || nptr->node_type == NT_LEAF) return;
    // check running status
    if (terminate || ignore_input) return;


    // check for assignment
    // TODO handle ID case properly
    /*
    if (nptr->type == ID_TYPE) {
        eval_node(nptr->children[1]);
        if (terminate || ignore_input) return;
        
        if (nptr->children[0] == NULL) {
            logging(LOG_ERROR, "failed to find child node");
            return;
        }
        put(nptr->children[0]->val.sval, nptr->children[1]);
        return;
    }
    
    for (int i = 0; i < 2; ++i) {
        eval_node(nptr->children[i]);
    }
    if (terminate || ignore_input) return;
    */
   /*
    if (nptr->type == STRING_TYPE) {
        (nptr->val).sval = (char *) malloc(strlen(nptr->children[0]->val.sval) + 1);
        if (! nptr->val.sval) {
            logging(LOG_FATAL, "failed to allocate string");
            return;
        }
        strcpy(nptr->val.sval, nptr->children[0]->val.sval);
    } else {
        nptr->val.ival = nptr->children[0]->val.ival;
    }
    */

    switch (nptr->tok)
    {
    case TOK_PLUS:
        for (int i = 0; i < 2; ++i) {
            eval_node(nptr->children[i]);
        }

        nptr -> val.ival = nptr->children[0]->val.ival + nptr->children[1]->val.ival;
        break;
    
    default:
        break;
    }

    return;
}

/* eval_root() - set the value of the root node based on the values of children 
 * Parameter: A pointer to a root node, possibly NULL.
 * Return value: None.
 * Side effect: The val dield of the node is updated. 
 */

void eval_root(node_t *nptr) {
    if (nptr == NULL) return;
    // check running status
    if (terminate || ignore_input) return;

    // check for assignment
    if (nptr->type == ID_TYPE) {
        eval_node(nptr->children[1]);
        if (terminate || ignore_input) return;
        
        if (nptr->children[0] == NULL) {
            logging(LOG_ERROR, "failed to find child node");
            return;
        }
        put(nptr->children[0]->val.sval, nptr->children[1]);
        return;
    }

    for (int i = 0; i < 2; ++i) {
        eval_node(nptr->children[i]);
    }
    if (terminate || ignore_input) return;
    
    if (nptr->type == STRING_TYPE) {
        (nptr->val).sval = (char *) malloc(strlen(nptr->children[0]->val.sval) + 1);
        if (! nptr->val.sval) {
            logging(LOG_FATAL, "failed to allocate string");
            return;
        }
        strcpy(nptr->val.sval, nptr->children[0]->val.sval);
    } else {
        nptr->val.ival = nptr->children[0]->val.ival;
    }
    return;
}

/* infer_and_eval() - wrapper for calling infer() and eval() 
 * Parameter: A pointer to a root node.
 * Return value: none.
 * Side effect: The type and val fields of the node are updated. 
 */

void infer_and_eval(node_t *nptr) {
    infer_root(nptr);
    eval_root(nptr);
    return;
}

/* strrev() - helper function to reverse a given string 
 * Parameter: The string to reverse.
 * Return value: The reversed string. The input string is not modified.
 * (STUDENT TODO)
 */

char *strrev(char *str) {
    return NULL;
}