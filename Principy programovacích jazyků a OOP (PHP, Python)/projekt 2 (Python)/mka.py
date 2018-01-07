#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
IPP - úloha MKA
Název úlohy: Minimalizace konečného automatu
Autor:       Marek Šipoš
Datum:       17.4 2017
Popis:       Skript načte reprezentaci konečného automatu, zkontroluje jeho platnost
             a na základě požadavků předaných parametry automat minimalizuje nebo najde
             jeho neukončující stavy.
'''
from __future__ import print_function # for eprint
from enum import Enum # for Enum
import argparse
import sys

current_row = 1 # current read line (for errors)

# ----------------- ERROR HANDLING -----------------

'''
print message to stderr
needs import: from __future__ import print_function
source: https://goo.gl/LVS0mi
'''
def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

'''
print error message and abort program with given error code
'''
def make_error( message, code ):
    if code == 60 or code == 61:
        eprint('Error #' + str(code) + ': ' + message + ' [line: ' + str(current_row) + ']')
    else:
        eprint('Error #' + str(code) + ': ' + message)
    sys.exit(code)

'''
custom error handling for argparse
source: https://goo.gl/aLgqmZ
'''
class ThrowingArgumentParser(argparse.ArgumentParser):
    def error(self, message):
        make_error(message, 1)

# ----------------- FSM RELATED OBJECTS -----------------

'''
Represents any state
'''
class State:
    def __init__(self, name):
        self.name = name
    def __repr__(self):
        return self.name
    def __eq__(self, other):
        return isinstance(other, State) and self.name == other.name
    def __ne__(self, other):
        return (not self.__eq__(other))
    def __hash__(self):
        return hash(self.__repr__())

'''
Represents any input symbol
'''
class Symbol:
    def __init__(self, symbol):
        self.symbol = symbol
    def __repr__(self):
        if self.symbol == '\'': # apostrophe doubled
            return '\'' + self.symbol + self.symbol + '\''
        else:
            return '\'' + self.symbol + '\''
    def __eq__(self, other):
        return isinstance(other, Symbol) and self.symbol == other.symbol
    def __ne__(self, other):
        return (not self.__eq__(other))
    def __hash__(self):
        return hash(self.__repr__())

'''
Represents one rule (with two states and one symbol)
'''
class Rule:
    def __init__(self, istate, isymbol, ostate):
        self.istate = istate
        self.isymbol = isymbol
        self.ostate = ostate
    def __repr__(self):
        return str(self.istate) + ' ' + str(self.isymbol) + ' -> ' + str(self.ostate)
    def __eq__(self, other):
        return isinstance(other, Rule) and str(self.istate) == str(other.istate) and \
               str(self.isymbol) == str(other.isymbol) and str(self.ostate) == str(other.ostate)
    def __ne__(self, other):
        return (not self.__eq__(other))
    def __hash__(self):
        return hash(self.__repr__())

'''
Represents finite state machine
'''
class FSM:
    def __init__(self):
        self.states = set()
        self.symbols = set()
        self.rules = set()
        self.start = None
        self.ends = set()
    def __repr__(self):
        # normalize
        s = '(\n{'
        s += ', '.join(str(e) for e in sorted(list(self.states), key=lambda x: x.name))
        s += '},\n{'
        s += ', '.join(str(e) for e in sorted(list(self.symbols), key=lambda x: x.symbol))
        s += '},\n{\n'
        s += ',\n'.join(str(e) for e in sorted(list(self.rules), key=lambda x: (x.istate.name, x.isymbol.symbol)))
        s += '\n},\n' + self.start.name + ',\n{'
        s += ', '.join(str(e) for e in sorted(list(self.ends), key=lambda x: x.name))
        s += '}\n)\n'
        return s

# ----------------- PARSE ARGUMENTS -----------------

# test params with help combination
if len(sys.argv) > 1 and sys.argv[1] == '--help' and len(sys.argv) > 2:
    make_error('Cannot use --help with other parameters', 1)

# test multiple params
used_params = set()
for p in sys.argv[1:]:
    if p == '--find-non-finishing':
        p = '-f'
    elif p == '--minimize':
        p = '-m'
    elif p == '--case-insensitive':
        p = '-i'
    if p in used_params:
        make_error('Parameter ' + p + ' was used multiple times', 1)
    else:
        used_params.add(p)

parser = ThrowingArgumentParser(description='IPP úloha MKA')
parser.add_argument('--input', help='Vstupní textový soubor', required=False, default=None)
parser.add_argument('--output', help='Výsledný textový soubor', required=False, default=None)
parser.add_argument('-f', '--find-non-finishing', help='Najít neukončující stav automatu', \
                    required=False, action='store_true', default=None)
parser.add_argument('-m', '--minimize', help='Provést minimalizaci automatu', required=False, \
                    action='store_true', default=None)
parser.add_argument('-i', '--case-insensitive', help='Při porovnávání nebude záležet na velikosti písmen', \
                    required=False, action='store_true', default=None)
args = parser.parse_args()

if args.find_non_finishing is not None and args.minimize is not None:
    make_error('Combination of --find-non-finishing (-f) and --minimize (-m) arguments is not allowed', 1)

# ----------------- LOAD INPUT -----------------

if args.input is not None:
    try:
        readfile = open(args.input, 'r');
    except OSError as e:
        make_error(e.strerror + ': ' + e.filename, 2)
else:
    readfile = sys.stdin;

read = readfile.read()
readfile.close()

# ----------------- LEXICAL ANALYSIS -----------------

'''
All possible lexems (lexical units)
'''
class Lexem(Enum):
    LPAR = 1    # (
    RPAR = 2    # )
    LSET = 3    # {
    RSET = 4    # }
    DASH = 5    # ,
    ARROW = 6   # ->
    STATE = 7   # c-like state id
    INPUT = 8   # 'X' where X is symbol
    EOF = 9     # end of file reached

'''
Represents token with Lexem type and bundled data
'''
class Token:
    def __init__(self, lexem, data):
        self.lexem = lexem
        self.data = data
    def __repr__(self):
        return 'Token: ' + str(self.lexem) + ' (data: \'' + self.data + '\')'
    def __eq__(self, other):
        return isinstance(other, Token) and self.__dict__ == other.__dict__
    def __ne__(self, other):
        return (not self.__eq__(other))
    def __hash__(self):
        return hash(self.__repr__())

'''
Extracts valid state identifier from input
'''
def read_state_identifier():
    global read
    identifier = ''
    
    # first char (must be alphabetical)
    if len(read) == 0 or not read[0].isalpha():
        return ''
    else:
        identifier = identifier + read[0]
        read = read[1:]
    
    # another char
    while True:
        if len(read) == 0 or not (read[0] == '_' or read[0].isdigit() or read[0].isalpha()):
            if identifier[-1:] == '_':
                return ''
            return identifier
        identifier = identifier + read[0]
        read = read[1:]

'''
Returns next valid token from input
It will return EOF if there is nothing else to return
'''
def get_next_token():
    global read
    global current_row
    state = 0
    lexem = Lexem.EOF
    data = ''
    while True:
        if state == 0:
            # end of file reached
            if len(read) == 0:
                break
            # white spaces
            if read[0] == '\n':
                current_row += 1
            if read[0] in {' ', '\t', '\n'}:
                read = read[1:]
                continue
            # ( ) { } ,
            if read[0] == '(':
                lexem = Lexem.LPAR
            elif read[0] == ')':
                lexem = Lexem.RPAR
            elif read[0] == '{':
                lexem = Lexem.LSET
            elif read[0] == '}':
                lexem = Lexem.RSET
            elif read[0] == ',':
                lexem = Lexem.DASH
            if read[0] in {'(', ')', '{', '}', ','}:
                read = read[1:]
                break
            # comments
            if read[0] == '#':
                state = 1
                read = read[1:]
                continue
            # ->
            if read[0] == '-':
                state = 2
                lexem = Lexem.ARROW
                read = read[1:]
                continue
            # c-like state id
            if read[0] == '_' or read[0].isdigit() or read[0].isalpha():
                state = 3
                lexem = Lexem.STATE
                continue
            # 'X' where X is symbol
            if read[0] == '\'':
                state = 4
                lexem = Lexem.INPUT
                read = read[1:]
                continue
        elif state == 1:
            if len(read) == 0:
                break
            if read[0] == '\n':
                current_row += 1
                state = 0
            read = read[1:]
        elif state == 2:
            if len(read) == 0 or read[0] != '>':
                make_error('Lexical error, expected \'>\' for rule arrow', 60)
            read = read[1:]
            break
        elif state == 3:
            data = read_state_identifier()
            if len(data) == 0:
                make_error('Lexical error in state identifier', 60)
            break
        elif state == 4:
            # '''' (symbol is escaped apostrophe)
            if len(read) >= 3 and read[:3] == '\'\'\'':
                data = '\''
                read = read[3:]
            # 'x' (normal symbol)
            elif len(read) >= 2 and read[1] == '\'':
                # ''' (invalid)
                if read[0] == '\'':
                    make_error('Lexical error in input symbol, apostrophe must be doubled', 60)
                else:
                    if read[0] == '\n':
                        current_row += 1
                    data = read[0]
                    read = read[2:]
            # '' (symbol is eps)
            elif len(read) >= 1 and read[0] == '\'':
                read = read[1:]
            else:
                make_error('Lexical error in input symbol', 60)
            break
    return Token(lexem, data)

# ----------------- SEMANTIC ANALYSIS PREPARATION -----------------

'''
Check state for existence in set of states in FSM (Semantic check)
'''
def check_state(fsm, state):
    for i in fsm.states:
        if i == state:
            return
    make_error('State \'' + state.name + '\' does not exist', 61)

'''
Check symbol for existence in set of symbols in FSM (Semantic check)
'''
def check_symbol(fsm, symbol):
    if len(symbol.symbol) == 0: # eps
        return
    for i in fsm.symbols:
        if i == symbol:
            return
    make_error('Symbol \'' + symbol.symbol + '\' is not in alphabet', 61)

'''
Add state to FSM with respect to case_insensitive parameter
'''
def states_add(fsm, state):
    if args.case_insensitive is not None:
        state.name = state.name.lower()
    fsm.states.add(state)

'''
Add symbol to FSM with respect to case_insensitive parameter
'''
def symbols_add(fsm, symbol):
    if args.case_insensitive is not None:
        symbol.symbol = symbol.symbol.lower()
    fsm.symbols.add(symbol)

'''
Add rule to FSM with respect to case_insensitive parameter
This function has to be called 3 times to add one rule
1. With left state 2. With left symbol 3. With right state
That allows the "build" of rule
'''
def rules_build(fsm, obj):
    if rules_build.rule.istate is None:
        if args.case_insensitive is not None:
            obj.name = obj.name.lower()
        check_state(fsm, obj)
        rules_build.rule.istate = obj
    elif rules_build.rule.isymbol is None:
        if args.case_insensitive is not None:
            obj.symbol = obj.symbol.lower()
        check_symbol(fsm, obj)
        rules_build.rule.isymbol = obj
    else:
        if args.case_insensitive is not None:
            obj.name = obj.name.lower()
        check_state(fsm, obj)
        rules_build.rule.ostate = obj
        fsm.rules.add(rules_build.rule)
        rules_build.rule = Rule(None, None, None)
rules_build.rule = Rule(None, None, None) # static

'''
Add end state to FSM with respect to case_insensitive parameter
'''
def ends_add(fsm, state):
    if args.case_insensitive is not None:
        state.name = state.name.lower()
    check_state(fsm, state)
    fsm.ends.add(state)

fsm = FSM()
sem_inside = False
sem_state = 1

# ----------------- PREDICTIVE SYNTAX + SEMANTIC ANALYSIS -----------------

'''
---- PRODUCTION RULES ----
FSM -> ( STATES , INPUTS , RULES , state , STATES )
STATES -> { FIRST_STATE }
FIRST_STATE -> state ANOTHER_STATE
FIRST_STATE -> ԑ
ANOTHER_STATE -> , state ANOTHER_STATE
ANOTHER_STATE -> ԑ
INPUTS -> { FIRST_INPUT }
FIRST_INPUT -> input ANOTHER_INPUT
FIRST_INPUT -> ԑ
ANOTHER_INPUT -> , input ANOTHER_INPUT
ANOTHER_INPUT -> ԑ
RULES -> { FIRST_RULE }
FIRST_RULE -> state input → state ANOTHER_RULE
FIRST_RULE -> ԑ
ANOTHER_RULE -> , state input → state ANOTHER_RULE
ANOTHER_RULE -> ԑ
'''

'''
Non-terminals (terminals are tokens)
'''
class Nonterm(Enum):
    FSM = 1
    STATES = 2
    FIRST_STATE = 3
    ANOTHER_STATE = 4
    INPUTS = 5
    FIRST_INPUT = 6
    ANOTHER_INPUT = 7
    RULES = 8
    FIRST_RULE = 9
    ANOTHER_RULE = 10
    MOSTLEFT = 11 # dollar symbol, equivalent to EOF token

stack = [Nonterm.MOSTLEFT, Nonterm.FSM]
token = get_next_token()

while True:
    nt = stack[-1]
    if not isinstance(nt, Token) and nt == Nonterm.MOSTLEFT:
        if token.lexem == Lexem.EOF:
            break
        else:
            make_error('Syntax error', 60)
    elif isinstance(nt, Token):
        if nt.lexem == token.lexem:
            stack.pop()
            # --- SEMANTIC START ---
            if token.lexem == Lexem.LSET:
                sem_inside = True
            elif token.lexem == Lexem.RSET:
                sem_inside = False
            elif token.lexem == Lexem.DASH and sem_inside == False:
                sem_state += 1
            elif token.lexem == Lexem.STATE:
                if sem_state == 1:
                    states_add(fsm, State(token.data))
                elif sem_state == 3:
                    rules_build(fsm, State(token.data))
                elif sem_state == 4:
                    s = State(token.data)
                    check_state(fsm, s)
                    fsm.start = s
                elif sem_state == 5:
                    ends_add(fsm, State(token.data))
            elif token.lexem == Lexem.INPUT:
                if sem_state == 2:
                    if token.data == '':
                        make_error('Lexical error, empty char (\'\') inside an alphabet', 60)
                    symbols_add(fsm, Symbol(token.data))
                elif sem_state == 3:
                    rules_build(fsm, Symbol(token.data))
            # --- SEMANTIC END ---
            token = get_next_token()
        else:
            make_error('Syntax error', 60)
    else:
        stack.pop()
        if nt == Nonterm.FSM and token.lexem == Lexem.LPAR:
            stack.append(Token(Lexem.RPAR, ''))
            stack.append(Nonterm.STATES)
            stack.append(Token(Lexem.DASH, ''))
            stack.append(Token(Lexem.STATE, ''))
            stack.append(Token(Lexem.DASH, ''))
            stack.append(Nonterm.RULES)
            stack.append(Token(Lexem.DASH, ''))
            stack.append(Nonterm.INPUTS)
            stack.append(Token(Lexem.DASH, ''))
            stack.append(Nonterm.STATES)
            stack.append(Token(Lexem.LPAR, ''))
        elif nt == Nonterm.STATES and token.lexem == Lexem.LSET:
            stack.append(Token(Lexem.RSET, ''))
            stack.append(Nonterm.FIRST_STATE)
            stack.append(Token(Lexem.LSET, ''))
        elif nt == Nonterm.FIRST_STATE and token.lexem == Lexem.STATE:
            stack.append(Nonterm.ANOTHER_STATE)
            stack.append(Token(Lexem.STATE, ''))
        elif nt == Nonterm.FIRST_STATE and token.lexem == Lexem.RSET:
            pass
        elif nt == Nonterm.ANOTHER_STATE and token.lexem == Lexem.DASH:
            stack.append(Nonterm.ANOTHER_STATE)
            stack.append(Token(Lexem.STATE, ''))
            stack.append(Token(Lexem.DASH, ''))
        elif nt == Nonterm.ANOTHER_STATE and token.lexem == Lexem.RSET:
            pass
        elif nt == Nonterm.INPUTS and token.lexem == Lexem.LSET:
            stack.append(Token(Lexem.RSET, ''))
            stack.append(Nonterm.FIRST_INPUT)
            stack.append(Token(Lexem.LSET, ''))
        elif nt == Nonterm.FIRST_INPUT and token.lexem == Lexem.INPUT:
            stack.append(Nonterm.ANOTHER_INPUT)
            stack.append(Token(Lexem.INPUT, ''))
        elif nt == Nonterm.FIRST_INPUT and token.lexem == Lexem.RSET:
            pass
        elif nt == Nonterm.ANOTHER_INPUT and token.lexem == Lexem.DASH:
            stack.append(Nonterm.ANOTHER_INPUT)
            stack.append(Token(Lexem.INPUT, ''))
            stack.append(Token(Lexem.DASH, ''))
        elif nt == Nonterm.ANOTHER_INPUT and token.lexem == Lexem.RSET:
            pass
        elif nt == Nonterm.RULES and token.lexem == Lexem.LSET:
            stack.append(Token(Lexem.RSET, ''))
            stack.append(Nonterm.FIRST_RULE)
            stack.append(Token(Lexem.LSET, ''))
        elif nt == Nonterm.FIRST_RULE and token.lexem == Lexem.STATE:
            stack.append(Nonterm.ANOTHER_RULE)
            stack.append(Token(Lexem.STATE, ''))
            stack.append(Token(Lexem.ARROW, ''))
            stack.append(Token(Lexem.INPUT, ''))
            stack.append(Token(Lexem.STATE, ''))
        elif nt == Nonterm.FIRST_RULE and token.lexem == Lexem.RSET:
            pass
        elif nt == Nonterm.ANOTHER_RULE and token.lexem == Lexem.DASH:
            stack.append(Nonterm.ANOTHER_RULE)
            stack.append(Token(Lexem.STATE, ''))
            stack.append(Token(Lexem.ARROW, ''))
            stack.append(Token(Lexem.INPUT, ''))
            stack.append(Token(Lexem.STATE, ''))
            stack.append(Token(Lexem.DASH, ''))
        elif nt == Nonterm.ANOTHER_RULE and token.lexem == Lexem.RSET:
            pass
        else:
            make_error('Syntax error', 60)

if len(fsm.symbols) == 0:
    make_error('Alphabet is empty', 61)

# ----------------- VALIDATE FSM -----------------

# something is empty
if len(fsm.states) == 0:
    make_error('FSM is not complete, states are empty', 62)
if len(fsm.symbols) == 0:
    make_error('FSM is not complete, alphabet is empty', 62)
if len(fsm.rules) == 0:
    make_error('FSM is not complete, rules are empty', 62)
if fsm.start is None:
    make_error('FSM is not complete, start state is empty', 62)
if len(fsm.ends) == 0:
    make_error('FSM is not complete, ending states are empty', 62)

# check determinism
for i in fsm.rules:
    if i.isymbol.symbol == '':
        make_error('FSM is not deterministic, epsilon transition found in rule with state ' + str(i.istate), 62)
    for j in fsm.rules:
        if i is j:
            continue
        if i.istate == j.istate and i.isymbol == j.isymbol:
            make_error('FSM is not deterministic, cannot decide resulting state when state ' \
                       + str(i.istate) + ' gets symbol ' + str(i.isymbol), 62)

'''
Recursive function for finding reachable set of states
First param should be set used in all get_reachable calls (passed by reference)
Second param is state from where to begin with checking
Third param determines, whether rule expansions should be reversed (S1 <- s S2)
'''
def get_reachable(reachable, state, reverse):
    reachable.add(state)
    for r in fsm.rules:
        if reverse is True and r.ostate == state and r.istate not in reachable:
            get_reachable(reachable, r.istate, True)
        if reverse is False and r.istate == state and r.ostate not in reachable:
            get_reachable(reachable, r.ostate, False)

# find non-reachable states
non_reachable = set()
reachable = set()
get_reachable(reachable, fsm.start, False)
for s in fsm.states:
    if s not in reachable:
        non_reachable.add(s)
if len(non_reachable) > 0:
    make_error('FSM is not correct, state ' + str(non_reachable.pop()) + ' is not reachable', 62)

# find non-finishing states
non_finishing = set()
finishing = set()
for e in fsm.ends:
    get_reachable(finishing, e, True)
for s in fsm.states:
    if s not in finishing:
        non_finishing.add(s)
if len(non_finishing) > 1:
    make_error('FSM is not correct, found more than one non-finishing state', 62)

# ----------------- MINIMIZE -----------------

'''
Represents one group of states
'''
class Group:
    i = 0
    def __init__(self, stamp):
        Group.i += 1
        self.number = Group.i
        self.content = set()
        self.stamp = stamp
    def __repr__(self):
        # name created by all states inside (normalized)
        return '_'.join(str(e) for e in sorted(list(self.content), key=lambda x: x.name))
    def __eq__(self, other):
        return isinstance(other, Group) and self.number == other.number
    def __ne__(self, other):
        return (not self.__eq__(other))
    def __hash__(self):
        return hash(self.__repr__())

'''
Auxiliary class to hold group of groups
'''
class GroupManager:
    def __init__(self):
        self.groups = set() # All groups made by this manager
    # Returns group instance where given state belongs to
    def get_group_of_state(self, state):
        for g in self.groups:
            if state in g.content:
                return g
        return None
    # Returns group instance by text stamp (it is created if does not exists)
    def get_group_by_stamp(self, stamp):
        for g in self.groups:
            if g.stamp == stamp:
                return g
        newg = Group(stamp)
        self.groups.add(newg)
        return newg

if args.minimize is not None:
    newfsm = FSM()
    manager = GroupManager()
    # create two basic groups
    ng = manager.get_group_by_stamp('ng')
    g = manager.get_group_by_stamp('g')
    for s in fsm.states:
        if s in fsm.ends:
            g.content.add(s)
        else:
            ng.content.add(s)
    # sort alphabet (we will need it later and sorted)
    sorted_alphabet = sorted(list(fsm.symbols), key=lambda x: x.symbol)
    ''''''''''''''''''''''''''''''''''''''''''''''''''
    while True:
        newmanager = GroupManager() # fresh groups for next iteration
        # divide states into groups
        for s in fsm.states:
            # create stamp representing rule table of this state
            stamp = str(manager.get_group_of_state(s).number)
            for a in sorted_alphabet:
                for r in fsm.rules:
                    if r.istate == s and r.isymbol == a:
                        stamp += '|' + str(a.symbol) + '>' + str(manager.get_group_of_state(r.ostate).number)
                        break
            # put state into new group by its stamp
            newmanager.get_group_by_stamp(stamp).content.add(s)
        # check if any new groups were created
        if len(manager.groups) == len(newmanager.groups):
            break
        # next iteration
        else:
            manager = newmanager
    ''''''''''''''''''''''''''''''''''''''''''''''''''
    # create new states based on created groups
    for g in manager.groups:
        newfsm.states.add(State(str(g)))
    # change rules appropriately
    for r in fsm.rules:
        newrule = Rule(None, r.isymbol, None)
        for g in manager.groups:
            if r.istate in g.content:
                newrule.istate = State(str(g))
            if r.ostate in g.content:
                newrule.ostate = State(str(g))
        newfsm.rules.add(newrule)
    # change start state appropriately
    for g in manager.groups:
        if fsm.start in g.content:
            newfsm.start = State(str(g))
            break
    # change end states appropriately
    for s in fsm.ends:
        for g in manager.groups:
            if s in g.content:
                newfsm.ends.add(State(str(g)))
                break
    # finally - apply changes
    newfsm.symbols = fsm.symbols
    fsm = newfsm

# ----------------- OUTPUT AND ENDING -----------------

if args.output is not None:
    try:
        writefile = open(args.output, 'w+');
    except OSError as e:
        make_error(e.strerror + ': ' + e.filename, 3)
else:
    writefile = sys.stdout;

if args.find_non_finishing is not None:
    if len(non_finishing) == 0:
        writefile.write('0')
    else:
        writefile.write(str(non_finishing.pop()))
else:
    writefile.write(str(fsm))

writefile.close()
