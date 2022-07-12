
# Readme

## Grammar

```
StatementList ->   Statement StatementList
                 | Statement 
 
Statement ->   IDENTIFIER ':' IDENTIFIER '=' Expression
             | IDENTIFIER '=' Expression
             | FunctionCall
             | 'return' OptionalExpression
             | 'function' IDENTIFIER '(' OptionalParameterList ')' OptionalReturn '{' StatementList '}'
             | 'if' '(' Expression ')' '{' StatementList '}'

OptionalReturn ->  '->' IDENTIFIER
                  | e

OptionalArgumentList ->   ArgumentList
                        | e

ArgumentList ->   Expression ',' ArgumentList 
                | Expression 

OptionalParameterList ->   ParameterList
                         | e

ParameterList ->   Parameter ',' ParameterList 
                 | Parameter

Parameter -> IDENTIFIER ':' IDENTIFIER

Expression ->   MinusTerm '+' Expression
              | MinusTerm '-' Expression
              | MinusTerm '*' Expression
              | MinusTerm '/' Expression
              | MinusTerm '==' Expression
              | MinusTerm '<' Expression
              | MinusTerm '<=' Expression
              | MinusTerm '>' Expression
              | MinusTerm '>=' Expression
              | MinusTerm

OptionalExpression ->   Expression
                      | e

MinusTerm ->   '-' Term
             | Term

Term ->   INTEGER
        | REAL
        | IDENTIFIER
        | FunctionCall
        | '(' Expression ')'

FunctionCall -> IDENTIFIER '(' OptionalArgumentList ')'
```
