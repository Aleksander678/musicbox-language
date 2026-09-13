# Music Box Programming Language

Music Box is a small tree-walking interpreter built in C++.
It looks like an ordinary scripting language, until you try to name something. Every single word in a working Music Box program follows one rule — your variable names, your function names, even the language's own keywords.

The puzzle is to figure out the rule and learn to write code that works, without looking into the source code. The preferable way is to work it out on your own, but as a hint you can look into examples provided in ./examples.

## Building 

Requires a C++17-capable compiler.

```
g++ -std=c++17 -o musicbox src/main.cpp src/Lexer.cpp src/Parser.cpp src/Environment.cpp src/Interpreter.cpp src/Token.cpp
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
 
