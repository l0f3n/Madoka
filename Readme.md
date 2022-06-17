
# Readme


## Supported math functions

```
sin, cos, tan, log, abs, sqrt, ceil, floor, round
```

## Grammar

```
Start -> Expression

Expression -> MinusTerm ExpressionTail

ExpressionTail ->   '+' Expression
                  | '-' Expression
                  | '*' Expression
                  | '/' Expression
                  | '^' Expression
                  | e

MinusTerm ->   '-' Term
             | Term

Term ->   NUMBER
        | IDENTIFIER TermTail
        | '(' Expression ')'

TermTail ->   '(' Expression ')'
            | e
```