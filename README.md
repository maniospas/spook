# spook

Welcome to the spook language! A very unserious interpreted language with -you guessed it-
*spoo~oo~ky* syntax.

# Setup

Download *main.cpp* and compile it with your c++ compiler of choice. For example:

```bash
g++ -o spook main.cpp -O3
```

You will also need the standard library file *spook.oo*
to get access to the spooky syntax. You can define your
own syntax, but this is a self-highlighting emoji theme
that looks good in any text editor with unicode support.
Anyway, here is the mandatory badly-implemented recursive 
example.

```text
# main.oo
\dep "spook.oo"
🪦 ALWAYS = 1          # symbol definition
👻 fib                 # function
    🕸️ n               # argument
    | n.nle 1 → n      # return n if condition is true
    | ALWAYS  → nadd(  # proper function call
        fib n.nsub 1,  # call one return without parentheses, dot pipes a first argument
        fib n.nsub 2
    )
🏚️ main
    | nprint fib(30)   # just run an always-zero outcome
```

Run this program per `./spook main.oo`.

The syntax can be reconfigured. Here's a version with boring syntax. I hope,
given the explanation for `|` above, that it does not need any further explaining.
Right? Here we do need some highlighting, but more on this in the future - if at all.

```cpp
@dep "boring.oo"

symbol ALWAYS = 1
fn fib
    arg n
    | n.nle 1 then n
    | ALWAYS  then nadd(
        fib n.nsub 1,
        fib n.nsub 2
    )

namespace main
    let result = fib 30
    | nprint result

```


# Syntax

Syntax is basically a list of declarations organized
into namespaces and functions. Any variable can be interpreted 
as any other type using its byte representation into `u64`, 
where things like strings and arrays are tracked via their 
addresses into safely accessible buffer; you may get errors but not segfaults. 

**declarations**

- **\dep "spook.oo"** brings in the spook dialect. You can create your own using different symbols.
- **👻 function_name** starts a new function (there are no nested functions). Functions are like ghosts in that they are immaterial, get it?
- **🕸️ argument_name** indicates the next function argument. Think of a web tangling the arguments as the stack tape goes through them.
- **🎃 variable = value** sets a local variable name to the value computed by the right-hand-side expression. Pumpkins!
- **| condition_value → return_value** checks the condition by parsing its expression. If the condition evaluates to 0, the rest of the declaration is skipped. If it evaluates to non-zero, it parses the
returned value expression and returns its outcome (prevents further statements in the function from running). Who doesn't like early returns, right Can use this for sequential code when using functions that always return zero and hence need not have the arrow and return branch (conditions skip to the next declaration). See examples.
- **🏚️ namespace_name** This is our haunted house. It starts a new namespace for variables (there are no nested functions or namespaces). It runs upon declaration, and becomes a system exit code if it returns a non-zero value. If the program is still alive and kickin, its variables can be accessed within expressions.
- **🪦 variable = value** defines a global symbol. Use with care, though symbols cannot be over-written.

**expressions**
- Constants and local variables are expressions. Constants can be strings, nats, or floats.
- Access namespace variables via `namespace::varname`. 
- Call functions with comma-separated arguments in parentheses like `nadd(x,y)`. Arguments are parsed expressions themselves (),
- optionally, call functions with one argument using syntax like `cos|3.141592`. This makes for handy chaining.

**builtin functions**

Two or more arguments (prefix is the data type being operated among nats,floats,strings,lists):
*nadd,nmul,fadd,fmul,sadd,ladd*

Exactly two arguments: 
*nsub,ndiv,fsub,fdiv,pow* (pow for floats only), *nlt,ngt,nle,nge,neq,nneq,not*, *flt,fgt,fle,fge,feq,fneq,seq,sneq*.

Unary operators (unary arithmetics apply only to floats): 
*cos,sin,tan,acos,asin,atan,cosh,sinh,tanh,exp*

Others:
- *fprint,nprint,sprint* print respective types
- *ncast,fcast* cast nats to floats and conversely (the first letter is the return type)
- *nconvert,fconvert* convert strings to nats or floats
- *nstring,fstring* convert nats or floats to strings
- *lget* get a list element
- *llen* list length
- *slen* string length
- *lslice(from,to)* get a sublist (zero-copy)
- *sslice(from,to)* get a substring (zero-copy)
- *sfind(string,substring)* find a substring in a string (return the string length if not found)
- *exec(command)* run a command in the system console and return its string output
- *read()* read a user input from the console
- *read(path)* read a path string (file or dir) as a string, where dir paths have each entry in a new line
- *write(path,string)* write a string onto a file


## FAQ

*Q: I didn't get spooked.*

Can happen to the best of us. Dare I introduce you... to superspook?
Enable it per `\dep "superspook.oo"` and have fun. Like the example below.
Read *syperspook.oo* to make sense of available symbol substitutions.

```text
\dep "superspook.oo"

🏚️ globs
    🎃 always
        🐾 1
👻 fib
    🕸️ n
    🍭 n.nle 1 
        🩸 n
    🍭 globs::always  
        🩸 nadd
            🌔 fib n.nsub 1
            🦴 fib n.nsub 2
            🌒

🏚️ main
    🎃 result 
        🐾 fib 30
    🍭 nprint result
```


*Q: MOARE SPOOK!*

Nobody stops you from using emojis as names. Like this:

```
\dep "superspook.oo"

🏚️ ⭐
    🎃 ⭐ 🐾 1
👻 🐰
    🕸️ 🧺
    🍭 🧺.nle 1 
        🩸 🧺
    🍭 ⭐::⭐
        🩸 nadd
            🌔 🐰 🧺.nsub 1
            🦴 🐰 🧺.nsub 2
            🌒

🏚️ main
    🍭 nprint 🐰 30
```


*Q: Can I use this?*

Sure, if you want some simple file processing
or system scripting. Comparably fast to
Python and its executable is <200KB. Can 
call your system's curl and stuff via 
the `exec` command.

*Q: Is there a type system?*

Type system? What type system? Didn't you get
the memo that computers run on only 0 and 1?

There's not even a guarantee on whether
a value represents a primitive or an array!
You will make do with some begrudgingly added
errors for out-of-memory or use-after-free.

Things might change. Or might not.

*Q: I see file operations. Are spook programs gonna wreck my PC?*

Part of the spookiness, right? For now, the language just 
refuses to overwrite files.

Some advanced safety features may be added in the future.

*Q: It's cumbersome to use emojis.*

Right? But fun! You can use the textual versions of symbols if you must...
Or define your own dialect! The spook.oo dialect definition is copied below:

```
# spook.oo
# Enables semi-spooky, semi-usable syntax.
\sym 🪦 = \sym
🪦 👻 = \fn
🪦 🎃 = \let
🪦 🕸️ = \arg
🪦 🏚️ = \sp
🪦 |  = \do
🪦 →  = \then
🪦 🧹 = \del
🪦 🕯️ = \dep
```

*Q. Nice slop, bro.*

First of all, everyone categorically dismissing AI is missing
out on its responsible uses and needs to chill out a bit. Second, 
*this project did not use ANY AI whatsoever. Not even for searching up functions or commands.*
It was a while since I tried the good-old "actually go through search results"
(from duck-duck-go search) and was pleasantly surprised that the internet 
still contains useful information. Who knew?
