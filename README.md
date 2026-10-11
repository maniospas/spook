# spook

Welcome to the spook language! A very unserious interpreted language with -you guessed it-
*spoo~oo~ky* syntax.

# Setup

Download *main.cpp* and compile it with your c++ compiler of choice. For example:

```bash
g++ -o spook main.cpp -O3
```

## Example

Our syntax is self-highlighting thanks to emoji colors! Just write spook in any text editor. Anyway, here is the mandatory badly-implemented recursive example. Functions are interpreted on-demand only.

```text
# main.oo
🏚️ globals  
    🎃 always = 1
    🎃 pi     = 3.141592
    
👻 fib
    🕸️ n
    🍭 nle(n,1) → n
    🍭 globals::always → nadd(
        fib|nsub(n,1),
        fib|nsub(n,2)
    )

🏚️ main
    # easy piping of one return
    🍭 sprint|sadd("hello ", "world!")

    # up to you to ensure correctness
    🎃 mylist = lost(1.0,2,3)
    🍭 fprint|get(mylist,0)
    🍭 nprint|get(mylist,1)

    # some math utilities
    🍭 fprint|sin|fmul(globals::pi,0.5)

    # custom functions
    🍭 nprint|fib(30)
```

Run this program per `./spook main.oo`.



# Syntax

Syntax is ridiculously easy. Any variable can be interpreted as any other type using its byte representation into `u64`, where things like strings and arrays are tracked via their addressese into safely accessible buffer; you may get errors but not segfaults. 

**declarations**

- **👻 function_name** starts a new function (there are no nested functions). Functions are like ghosts in that they are immaterial, get it?
- **🕸️ argument_name** indicates the next function argument. Think of a web tangling the arguments as the stack tape goes through them.
- **🎃 variable = value** sets a local variable name to the value computed by the right-hand-side expression. Pumpkins!
- **🍭 condition_value → return_value** checks the condition by parsing its expression. If the condition evaluates to 0, the rest of the declaration is skipped. If it evaluates to non-zero, it parses the
returned value expression and returns its outcome (prevents further statements in the function from running). Who doesn't like early returns, right? "Lick" the condition like a candy and check if it's in our taste. Can use this for sequential code when using functions like printing ones, which always return zero and hence need not have the arrow and return branch.
- **🏚️ namespace_name** starts a new namespace for variables (there are no nested functions or namespaces). It runs upon declaration, becomes a system exit code if it returns a non-zero value, and
it variables can be accessed within expressions. This is our haunted house. Fitting, I hope.

**expressions**
- Constants and local variables are expressions.
- Access namespace variables via `namespace::varname`. 
- Call functions with comma-separated arguments in parentheses like `nadd(x,y)`. Arguments are parsed expressions themselves (),
- optionally, call functions with one argument using

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

Can happen to the best of us.

*Q: Can I use this?*

Sure, if you want some simple file processing
or system scripting. Nearly as fast as 
python and its executable is <128KB. Can 
you your system's curl and stuff via 
the `exec` command.

*Q: Is there a type system?*

Type system? What type system? Didn't you get
the memo that computers run on only 0 and 1?

There's not even a guarantee on whether
a value represents a primitive or an array!
You will make do with some begrudgingly added
errors for out-of-memory or use-after-after.

Things might change. Or might not.

*Q: I see file operations. Are spook programs gonna wreck my PC?*

Part of the spookiness, right? For now, the language just 
refuses to overwrite files.

Some advanced safety features may be added in the future.

*Q: It's cumbersome to use emojis.*

Right? But fun! In future versions
you will be able to use the following symbol
names instead, and there will be an auto-formatter
for maximum spookiness:

```
\fn 👻
\let 🎃
\arg 🕸️
\sp 🏚️
\do 🍭
\permit 🕯️
\del 🪦  (current unused)
\then →
```

*Q. Nice slop, bro.*

First of all, everyone categorically dismissing AI is missing
out on its responsible uses and needs to chill out a bit. Second, 
*this project did not use ANY AI whatsoever. Not even for searching up functions or commands.*
It was a while since I tried the good-old "actually go through search results"
(from duck-duck-go search) and was pleasantly surprised that the internet 
still contains useful information. Who knew?
