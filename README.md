calc
====

Reads a mathematical expression from stdin, outputs the rezult to stdout

Supported operators / functions
====

|                        |
|------------------------|
|  + - * / ^ %           |
|  sqrt  ln  lb  lg      |
|  cos  sin  tan         |
|  acos  asin  atan      |
|  abs  floor  ceil      |
|  e  pi  tau            |

Examples
====

```mathematica
    10^3 + 9^3
    > 1729
```

```mathematica
    (1 + sqrt(5)) / 2
    > 1.61803
```

```mathematica
    (4 * atan(1/5) - atan(1/239)) * 8 / tau()
    > 1
```

```mathematica
    sqrt(ceil((3 - sqrt(5)) * 1000))
    > 27.6405
```
