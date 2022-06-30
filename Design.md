# Design document 

This is the first programming languange I've ever made. 
I don't actually intend to use it for anything, but it's fun to make!

Some of the mottos I try to follow when designing and implementing this languange are:
 - Explicit is better than implicit.
 - Simple is better than complex.
 - Readability counts.
 - If the implementation is easy to explain, it may be a good idea.

For those of you familiar with [The Zen of Python](https://peps.python.org/pep-0020/), these might seem familiar...

This languange is inspired, at least in spirit, by Python.
And by Jai in its syntax.

## Variables 

```
a: int = 3   ; Definition of integer
a: int = 3.0 ; Invalid: Type mismatch

b: [int] = [1, 2, 3] ; List of ints
```

## Functions

```
; A function taking an int and a real as arguments and returing an int
function main(a: int, b: real) -> int
{
    return a*b 
}

result := main(1, 2) ; Function call

; A function with no arguments and no return value
function main()
{

}

```

## Branching 

```
if a < 2
{
    return 2
}
```

## Loops

```
; Loop over list a
for (i : a) 
{
    print(i)
}

; Loop until break
for
{
    if a > 2
    {
        break
    }
}
```
