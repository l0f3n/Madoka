
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
             | 'return' OptionalExpression
             | 'function' IDENTIFIER '(' OptionalParameterList ')' OptionalReturn '{' StatementList '}'
             | 'if' '(' Expression ')' '{' StatementList '}'
             | 'while' '(' Expression ')' '{' StatementList '}'

OptionalReturn ->  '->' IDENTIFIER
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

OptionalExpression ->   Expression
                      | e

MinusTerm ->   '-' Term
             | Term

Term ->   INTEGER
        | REAL
        | IDENTIFIER
        | IDENTIFIER '(' OptionalArgumentList ')'
        | '(' Expression ')'
```
