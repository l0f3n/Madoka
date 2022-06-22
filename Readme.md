
# Readme

## Grammar

```
Start -> StatementList

StatementList -> Statement StatementListTail

StatementListTail ->   StatementList
                     | e
 
Statement ->   IDENTIFIER ':' IDENTIFIER '=' Expression
             | IDENTIFIER '=' Expression
             | IDENTIFIER '(' OptionalArgumentList ')'
             | 'return' OptionalArgumentList
             | 'function' IDENTIFIER '(' OptionalParameterList ')' OptionalReturn '{' StatementList '}'
             | 'if' '(' Expression ')' '{' StatementList '}'
             | 'while' '(' Expression ')' '{' StatementList '}'

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

Expression ->   MinusTerm '+' Expression
              | MinusTerm '-' Expression
              | MinusTerm '*' Expression
              | MinusTerm '/' Expression
              | MinusTerm

MinusTerm ->   '-' Term
             | Term

Term ->   NUMBER
        | IDENTIFIER
        | IDENTIFIER '(' OptionalArgumentList ')'
        | '(' Expression ')'
```
