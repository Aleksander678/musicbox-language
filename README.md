# Music Box Programming Language

Music Box is a small tree-walking interpreter of my own design, built in C++.
It looks like an ordinary scripting language — until it starts rejecting your variable names for reasons it won't explain.

The puzzle is to figure out why, without looking into the source code. The preferable way is to figure things out on your own, but as a hint you can look into examples provided in ./examples.

## Building 

Requires a C++17-capable compiler.

```
g++ -std=c++17 -o musicbox main.cpp Lexer.cpp Parser.cpp Environment.cpp interpreter.cpp Token.cpp
```

## Running

```
./musicbox script.mbx
```


**Interactive REPL:**
```
./musicbox
```
Type your code, press enter on a blank line to run what you've written, and `exit` to quit.
 
## Example
 
```
function factorial(limit) {
    condition(limit <= 1) {
        return 1
    }
    return limit * factorial(limit - 1)
}
 
declare result = factorial(5)
display(result)
```
 
## Language basics
 
- `declare name = value` — declare a variable
- `function name(params) { ... }` — declare a function
- `condition(expr) { ... }` — run a block if true
- `repeat(n) { ... }` — run a block n times
- `return value` — return from a function
- `display(...)` — print output
Arithmetic (`+ - * /`), comparisons (`< > <= >=`), and equality (`== !=`) all work as you'd expect.
 
## License
 
MIT — see [LICENSE](LICENSE).
 
