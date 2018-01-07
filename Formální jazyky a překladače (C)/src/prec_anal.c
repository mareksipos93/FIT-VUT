/*
*	Projekt:	IFJ16
*	Vedouci:	xztef02
*	Autori:		xsztef02 xsipos03 xszucs01 xstolj00
*				Patrik Sztefek, Marek Sipos, Tomas Szucz, Jakub Stol
*/
#include "synt_anal.h"

Ttoken *token_param = NULL; // Token got from prec_analysis() call

char prec_rules[17][17] = {
//     i    ESYS   (     )     +     -     *     /     <     >     <=    >=    ==    !=   LSYS  RSYS   $
    { 'X' , 'X' , 'X' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , 'X' , 'X' , '>' } , // PS_VALUE
    { 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' } , // PS_ESYS
    { '<' , 'X' , '<' , '=' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , 'X' , 'X' , 'X' } , // PS_LRB
    { 'X' , 'X' , 'X' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , 'X' , 'X' , '>' } , // PS_RRB
    { '<' , 'X' , '<' , '>' , '>' , '>' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , 'X' , 'X' , '>' } , // PS_PLUS
    { '<' , 'X' , '<' , '>' , '>' , '>' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , 'X' , 'X' , '>' } , // PS_MINUS
    { '<' , 'X' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , 'X' , 'X' , '>' } , // PS_STAR
    { '<' , 'X' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , 'X' , 'X' , '>' } , // PS_SLASH
    { '<' , 'X' , '<' , '>' , '<' , '<' , '<' , '<' , 'X' , 'X' , 'X' , 'X' , '>' , '>' , 'X' , 'X' , '>' } , // PS_LTHAN
    { '<' , 'X' , '<' , '>' , '<' , '<' , '<' , '<' , 'X' , 'X' , 'X' , 'X' , '>' , '>' , 'X' , 'X' , '>' } , // PS_RTHAN
    { '<' , 'X' , '<' , '>' , '<' , '<' , '<' , '<' , 'X' , 'X' , 'X' , 'X' , '>' , '>' , 'X' , 'X' , '>' } , // PS_LTHANEQ
    { '<' , 'X' , '<' , '>' , '<' , '<' , '<' , '<' , 'X' , 'X' , 'X' , 'X' , '>' , '>' , 'X' , 'X' , '>' } , // PS_RTHANEQ
    { '<' , 'X' , '<' , '>' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , 'X' , 'X' , 'X' , 'X' , '>' } , // PS_EQ
    { '<' , 'X' , '<' , '>' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , 'X' , 'X' , 'X' , 'X' , '>' } , // PS_NEQ
    { 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' } , // PS_LSYS
    { 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' , 'X' } , // PS_RSYS
    { '<' , 'X' , '<' , 'X' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , 'X' , 'X' , 'E' }   // PS_DOLLAR
};

char getPrecedenceOperation(PType top, PType input) {

    #if PREC_DEBUG == 1
        fprintf(stdout, "  [PREC_DEBUG] getPrecedenceOperation(%s, %s)\n", PType_string[top], PType_string[input]);
    #endif

    return prec_rules[top][input];
}

Psymbol getNextPrecSymbol() {
    static int par_level = 0; // Level of parenthesis

    Ttoken *token = NULL; // Token to work with
    Psymbol symbol; // Symbol to return

    // We got token prec_analysis() call
    if (token_param != NULL) {
        token = token_param;
        token_param = NULL;
    }
    // Something is left in token archive, we must use that
    else if (!cQueue_isempty(&token_archive)) {
        token = cQueue_first(&token_archive).content.token;
        cQueue_pop(&token_archive);
    }
    // Read token from input
    else {
        token = load_next_token();
    }

    symbol.token = token;
    symbol.type = PS_DOLLAR;
    symbol.data = '-';
    symbol.temp_var = NULL;

    if (token->type == IDENTIFIKATOR ||
        token->type == PLNE_KVALIFIKOVANY_IDENTIFIKATOR) {

        if ((first_analysis && strlen(current_func) == 0) || (!first_analysis && strlen(current_func) > 0)) {
            /* @SEM12 - Check for variables declaration (in expr) */
            if (get_declared_variable(token->attr->str, current_class, current_func) == NULL) {
                #if SEM_DEBUG == 1
                    fprintf(stdout, "\t@ User tried to use undeclared variable %s in expression!\n", token->attr->str);
                #endif
                error(ERR_SEM_DEF);
            }
            /* @SEM12 - Get data type from identificator */
            symbol.data = get_declared_variable(token->attr->str, current_class, current_func)->type[1];
        }


    }

    /* @SEM12 - Get data type from value */
    if (token->type == RETEZEC)
        symbol.data = 'S';
    else if (token->type == CELOCISELNY_LITERAL || token->type == CELOCISELNY_LITERAL_EXPONENT)
        symbol.data = 'I';
    else if (token->type == DESETINNY_LITERAL || token->type == DESETINNY_LITERAL_EXPONENT)
        symbol.data = 'D';

    if (token->type == IDENTIFIKATOR ||
        token->type == PLNE_KVALIFIKOVANY_IDENTIFIKATOR ||
        token->type == RETEZEC ||
        token->type == CELOCISELNY_LITERAL ||
        token->type == CELOCISELNY_LITERAL_EXPONENT ||
        token->type == DESETINNY_LITERAL ||
        token->type == DESETINNY_LITERAL_EXPONENT) {
        symbol.type = PS_VALUE;
    }
    else if (token->type == LEVA_KULATA_ZAVORKA) {
        symbol.type = PS_LRB;
        par_level++;
    }
    else if (token->type == PRAVA_KULATA_ZAVORKA) {
        if (par_level > 0) { // Only if not in inner parenthesis level
            symbol.type = PS_RRB;
            par_level--;
        }
    }
    else if (token->type == SCITANI) {
        symbol.type = PS_PLUS;
    }
    else if (token->type == ODECITANI) {
        symbol.type = PS_MINUS;
    }
    else if (token->type == NASOBENI) {
        symbol.type = PS_STAR;
    }
    else if (token->type == DELENI) {
        symbol.type = PS_SLASH;
    }
    else if (token->type == MENSI) {
        symbol.type = PS_LTHAN;
    }
    else if (token->type == VETSI) {
        symbol.type = PS_RTHAN;
    }
    else if (token->type == MENSI_NEBO_ROVNO) {
        symbol.type = PS_LTHANEQ;
    }
    else if (token->type == VETSI_NEBO_ROVNO) {
        symbol.type = PS_RTHANEQ;
    }
    else if (token->type == ROVNO) {
        symbol.type = PS_EQ;
    }
    else if (token->type == NEROVNO) {
        symbol.type = PS_NEQ;
    }

    // We are ending precedence analysis - put unused token back
    if (symbol.type == PS_DOLLAR) {
        insert_cqueue_token(token, &token_archive);
    }

    #if PREC_DEBUG == 1
        fprintf(stdout, "  [PREC_DEBUG]   ## getNextPrecSymbol %s -> Token: '%s' (data: '%s'), Result type: %c\n", PType_string[symbol.type], getTokenName(token->type), token->attr->str, symbol.data);
    #endif
    return symbol;
}

void push_cstack_psymbol(PType type, cStack *stack_p, char data, char *temp_var, Ttoken *token) {
    cItem item;
    Psymbol symbol;
    symbol.token = NULL; // We dont know, from what its made.. could be E op E -> E
    symbol.type = type;
    symbol.data = data;
    symbol.temp_var = temp_var;
    symbol.token = token;
    item.type = IT_PSYMBOL;
    item.content.psymbol = symbol;
    if (!cStack_push(stack_p, item)) {
        error(ERR_INTER);
    }
}

void insert_cqueue_token(Ttoken *token, cQueue *queue) {
    cItem to_insert;
    to_insert.content.token = token;
    to_insert.type = IT_TOKEN;
    cQueue_insert(queue, to_insert);
}

char prec_analysis(Ttoken *token) {
    #if PREC_DEBUG == 1
        fprintf(stdout, "  [PREC_DEBUG]   === PRECEDENCE ANALYSIS STARTED ===\n");
    #endif

    token_param = token; // We need to process this first

    // First step - there is T_EXPRESSION on top, pop it
    cStack_pop(&stack);
    // Second step - push $ on top
    push_cstack_psymbol(PS_DOLLAR, &stack, '-', NULL, NULL);

    char operation;
    Psymbol input = getNextPrecSymbol();
    cItem top;
    cStack temporary; // Because we need sometimes to read prec.symbol more deeply from stack
    cStack_init(&temporary, 5);
    do {
        #if PREC_DEBUG == 1
            fprintf(stdout, "    - - -\n");
        #endif

        top = cStack_top(&stack);
        if (top.type == IT_ERROR) {
            error(ERR_INTER);
        }

        // We HAVE TO have prec. symbol on the top
        if (top.type != IT_PSYMBOL) {
            error(ERR_SYNT);
        }

        // There could be PS_ESYS, PS_LSYS or PS_RSYS on top, in that case we are popping until we find regular prec. symbol to get prec. operation
        while (top.content.psymbol.type == PS_ESYS || top.content.psymbol.type == PS_LSYS || top.content.psymbol.type == PS_RSYS) {
            push_cstack_psymbol(top.content.psymbol.type, &temporary, top.content.psymbol.data, top.content.psymbol.temp_var, top.content.psymbol.token);
            cStack_pop(&stack);
            top = cStack_top(&stack);
        }
        // Finally get precedence operation
        operation = getPrecedenceOperation(top.content.psymbol.type, input.type);
        if (operation != '<') {
            // We have to push everything "borrowed" back (the check for < is there because operation < does it by itself)
            while (!cStack_isempty(&temporary)) {
                push_cstack_psymbol(cStack_top(&temporary).content.psymbol.type, &stack, cStack_top(&temporary).content.psymbol.data, cStack_top(&temporary).content.psymbol.temp_var, cStack_top(&temporary).content.psymbol.token);
                cStack_pop(&temporary);
                top = cStack_top(&stack);
            }
        }

        #if PREC_DEBUG == 1
            fprintf(stdout, "  [PREC_DEBUG]  Operation = '%c'\n", operation);
        #endif

        /* Apply operation */
        if (operation == '<') {
            push_cstack_psymbol(PS_LSYS, &stack, '-', NULL, NULL); // This is pushed just after non-SYS prec. symbol
            // We have to push everything "borrowed" back
            while (!cStack_isempty(&temporary)) {
                push_cstack_psymbol(cStack_top(&temporary).content.psymbol.type, &stack, cStack_top(&temporary).content.psymbol.data, cStack_top(&temporary).content.psymbol.temp_var, cStack_top(&temporary).content.psymbol.token);
                cStack_pop(&temporary);
                top = cStack_top(&stack);
            }
            push_cstack_psymbol(input.type, &stack, input.data, input.temp_var, input.token);
            input = getNextPrecSymbol();
        }
        else if (operation == '>') {

            // (E) -> E
            if (top.content.psymbol.type == PS_RRB) { // )>
                cStack_pop(&stack);
                top = cStack_top(&stack);
                if (top.content.psymbol.type == PS_ESYS) { // E)>
                    Psymbol top_symbol = top.content.psymbol; // Save E
                    #if PREC_DEBUG == 1
                        fprintf(stdout, "  [PREC_DEBUG] [ (ESYS) -> ESYS ] %c -> %c\n", top.content.psymbol.data, top_symbol.data);
                    #endif
                    cStack_pop(&stack);
                    top = cStack_top(&stack);
                    if (top.content.psymbol.type == PS_LRB) { // (E)>
                        cStack_pop(&stack);
                        top = cStack_top(&stack);
                        if (top.content.psymbol.type == PS_LSYS) { // <(E)>
                            cStack_pop(&stack);
                            top = cStack_top(&stack);
                            push_cstack_psymbol(PS_ESYS, &stack, top_symbol.type, top_symbol.temp_var, top_symbol.token); // <(E)> -> E
                        }
                        else error(ERR_SYNT);
                    }
                    else error(ERR_SYNT);
                }
                else error(ERR_SYNT);
            }
            // i -> E
            else if (top.content.psymbol.type == PS_VALUE) { // i>
                Psymbol s_top = top.content.psymbol; // Save i

                #if PREC_DEBUG == 1
                    fprintf(stdout, "  [PREC_DEBUG] [ PSYM -> ESYS ] %c -> %c\n", top.content.psymbol.data, s_top.data);
                #endif
                cStack_pop(&stack);
                top = cStack_top(&stack);
                if (top.content.psymbol.type == PS_LSYS) { // <i>
                    /* GENERATOR */
                    char *temp_var = manage_temp_var(s_top.data, NULL);
                    if ((first_analysis && strlen(current_func) == 0) || (!first_analysis && strlen(current_func) > 0)) {
                        char c = (s_top.token->type == IDENTIFIKATOR || s_top.token->type == PLNE_KVALIFIKOVANY_IDENTIFIKATOR) ? 'N' : s_top.data;
                        add_instruction(I_MOV, 'N', temp_var, c, s_top.token->attr->str, '-', NULL);
                        expr_temp_last = makeString(temp_var);
                    }

                    cStack_pop(&stack);
                    top = cStack_top(&stack);
                    push_cstack_psymbol(PS_ESYS, &stack, s_top.data, temp_var, s_top.token); // <i> -> E
                }
                else error(ERR_SYNT);
            }
            // E op E -> E
            else if (top.content.psymbol.type == PS_ESYS) { // E>
                Psymbol first_s = top.content.psymbol; // Save first E
                cStack_pop(&stack);
                top = cStack_top(&stack);
                if (top.content.psymbol.type == PS_PLUS || 
                    top.content.psymbol.type == PS_MINUS || 
                    top.content.psymbol.type == PS_STAR || 
                    top.content.psymbol.type == PS_SLASH || 
                    top.content.psymbol.type == PS_LTHAN || 
                    top.content.psymbol.type == PS_RTHAN || 
                    top.content.psymbol.type == PS_LTHANEQ || 
                    top.content.psymbol.type == PS_RTHANEQ || 
                    top.content.psymbol.type == PS_EQ || 
                    top.content.psymbol.type == PS_NEQ) { // op E>
                    PType op = top.content.psymbol.type; // Save operator
                    cStack_pop(&stack);
                    top = cStack_top(&stack);
                    if (top.content.psymbol.type == PS_ESYS) { // E op E>
                        Psymbol second_s = top.content.psymbol; // Save second E
                        cStack_pop(&stack);
                        top = cStack_top(&stack);
                        if (top.content.psymbol.type == PS_LSYS) { // <E op E>
                            cStack_pop(&stack);
                            top = cStack_top(&stack);
                            char result_type = get_result_type(first_s.data, second_s.data, op);
                            #if PREC_DEBUG == 1
                                fprintf(stdout, "  [PREC_DEBUG] [ ESYS op ESYS -> ESYS ] %c %s %c -> %c\n", first_s.data, PType_string[op], second_s.data, result_type);
                            #endif

                            /* @SEM12 - Check for operands compatibility */
                            if (result_type == 'E' && (((first_analysis && strlen(current_func) == 0) || (!first_analysis && strlen(current_func) > 0)))) {
                                #if SEM_DEBUG == 1
                                    fprintf(stdout, "\t@ Error in expression! Operands %c and %c are not compatible with operator %s\n", first_s.data, second_s.data, PType_string[op]);
                                #endif
                                error(ERR_SEM_TYPE);
                            }

                            push_cstack_psymbol(PS_ESYS, &stack, result_type, first_s.temp_var, NULL); // <E op E> -> E
                            expr_temp_last = first_s.temp_var;
                            /* GENERATOR */
                            if ((first_analysis && strlen(current_func) == 0) || (!first_analysis && strlen(current_func) > 0)) {
                                if (op == PS_PLUS || op == PS_MINUS || op == PS_STAR || op == PS_SLASH) {
                                    Instructions ins = I_ADD;
                                    if (op == PS_PLUS) ins = I_ADD;
                                    if (op == PS_MINUS) ins = I_SUB;
                                    if (op == PS_SLASH) ins = I_DIV;
                                    if (op == PS_STAR) ins = I_MUL;
                                    add_instruction(ins, 'N', first_s.temp_var, 'N', second_s.temp_var, '-', NULL);
                                }
                            }

                            // Save last used logical operator + operands (for later IF and WHILE jumps)
                            if (op == PS_LTHAN || op == PS_RTHAN || op == PS_LTHANEQ || op == PS_RTHANEQ || op == PS_EQ || op == PS_NEQ) {
                                expr_left_bool = first_s.temp_var;
                                expr_right_bool = second_s.temp_var;
                                if (op == PS_LTHAN) { expr_last_bool = I_JMPL; expr_last_bool_neg = I_JMPGE; }
                                if (op == PS_RTHAN) { expr_last_bool = I_JMPG; expr_last_bool_neg = I_JMPLE; }
                                if (op == PS_LTHANEQ) { expr_last_bool = I_JMPLE; expr_last_bool_neg = I_JMPG; }
                                if (op == PS_RTHANEQ) { expr_last_bool = I_JMPGE; expr_last_bool_neg = I_JMPL; }
                                if (op == PS_EQ) { expr_last_bool = I_JMPE; expr_last_bool_neg = I_JMPNE; }
                                if (op == PS_NEQ) { expr_last_bool = I_JMPNE; expr_last_bool_neg = I_JMPE; }
                            
                            }

                            // Free temp var for another usage
                            manage_temp_var('-', second_s.temp_var);
                        }
                        else error(ERR_SYNT);
                    }
                    else error(ERR_SYNT);
                }
                else error(ERR_SYNT);
            }
            else error(ERR_SYNT);

        }
        else if (operation == '=') {
            push_cstack_psymbol(input.type, &stack, input.data, NULL, input.token);
            input = getNextPrecSymbol();
        }
        else if (operation != 'E') {
            // Unknown operation? X = error
            error(ERR_SYNT);
        }

    } while (operation != 'E');

    // Now, there should be $E on the top
    if (top.type != IT_PSYMBOL || top.content.psymbol.type != PS_ESYS) {
        error(ERR_SYNT);
    }
    char result = top.content.psymbol.data;
    manage_temp_var('-', top.content.psymbol.temp_var); // Free last used temp var

    cStack_pop(&stack);
    top = cStack_top(&stack);
    // Now, there should be $ on the top
    if (top.type != IT_PSYMBOL || top.content.psymbol.type != PS_DOLLAR) {
        error(ERR_SYNT);
    }
    cStack_pop(&stack);

    cStack_free(&temporary);

    #if PREC_DEBUG == 1
        fprintf(stdout, "  [PREC_DEBUG]   === PRECEDENCE ANALYSIS FINISHED ===\n");
    #endif

    return result;
}
