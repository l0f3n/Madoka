
# Readme


## Supported math functions

```
sin, cos, tan, log, abs, sqrt, ceil, floor, round
```

## Grammar

```
Start -> StatementList

StatementList -> Statement StatementListTail

StatementListTail ->   StatementList
                     | e
 
Statement ->   IDENTIFIER StatementTail
             | 'return' OptionalArgumentList
             | 'function' IDENTIFIER '(' OptionalParameterList ')' OptionalReturn '{' StatementList '}'
             | 'if' '(' Expression ')' '{' StatementList '}'
             | 'while' '(' Expression ')' '{' StatementList '}'

StatementTail ->   ':' IDENTIFIER '=' Expression
                 | '=' Expression
                 | '(' OptionalArgumentList ')'

OptionalReturn ->  '->' '(' ParameterList ')'
                  | e

OptionalArgumentList ->   ArgumentList
                        | e

ArgumentList -> Argument ArgumentListTail

ArgumentListTail ->   ',' ArgumentList
                    | e

Argument -> Expression

OptionalParameterList ->   ParameterList
                         | e

ParameterList ->  Parameter ParameterListTail

ParameterListTail ->   ',' ParameterList
                     | e

Parameter -> IDENTIFIER ':' IDENTIFIER

Expression -> MinusTerm ExpressionTail

ExpressionTail ->   '+' Expression
                  | '-' Expression
                  | '*' Expression
                  | '/' Expression
                  | '^' Expression
                  | '>' Expression
                  | '<' Expression
                  | '=' Expression
                  | e

MinusTerm ->   '-' Term
             | Term

Term ->   NUMBER
        | IDENTIFIER TermTail
        | '(' Expression ')'

TermTail ->   '(' OptionalArgumentList ')'
            | e
```
