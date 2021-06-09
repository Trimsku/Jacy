# *Jacy*

### The idea

*Jacy* is an experimental project that is aimed to have powerful
features with ease of use but without an implicity.

Some main features *Jacy* would have one day:
- Base of functional programming (first-class monads, pattern matching,
  etc.)
- Rustish ownership system and safety
- `trait` system
- Comprehensive type system (generics, type bounds, `where` clause,
  etc.)
- In addition to generics -- type parameters inference

All of these features are not a firmly established, that's just a
starting point from which the way of research will be done.

What is the muse of *Jacy*? -- Do as much as we can at compilation-time.
At first, I was inspired by modern languages that became fairly popular
-- Kotlin/Swift, etc. Anyway, the reason of their popularity is that
their purpose is to modernise old technologies, Kotlin for Java, Swift
for Objective-C. I felt into stupor, I didn't want to solve problems of
old languages, I wanted to make something new and solid.

Also, I want to describe some kind of Zen, I need a list of common
values that *Jacy* should have in my opinion:
1. Convenience is more important than anything else, except the cases
   when convenience violates any other rule.
2. Everything that can be explicit -- must be explicit.
3. Safety, safety and safety, but it is important as far as there's no
   lack of convenience.
4. Sometimes we just prototype something and want to write code with
   speed of speaking. So every feature in *Jacy* should be review from
   side of coding speed and ease.
5. If we can help user with avoiding mistake -- we help him.

### Docs

That's not the real documentation especially a tutorial, just some
thoughts about the view on this project. I mostly will explain why I've
chosen certain solutions and not how to work with them. Some thoughts
may seem simple and meticulous, but I want to cover even simplest
aspects of PL.

#### Basics

##### Variables

The first idea was to use `var` and `val`, it's pretty nice, we don't
have weird `let` and `let mut` like Rust, but then I thought "`var` and
`val` are pretty confusing, they only differ in `r` and `l`, not really
easy to read". So, I replaced `val` with `let` and it looked like the
solution. Now, we have `let` and `let mut` 😊... Why? I forgot about
pattern-matching, Rust's solution is right because `let` is not a
constant var declaration, it is just a declaration of variable, and
variable name is a pattern in which we can set if it is a `mut`able or
not. I've already reserved `mut` keyword, so now we have only one
keyword for variable declaration (run-time!).

The syntax:

```g4
'let' pattern (':' type)? ('=' expr)?
```

Anyway, I'm able to add `var` keyword and just use it as alias for `let
mut`. In this way we are not able to use pattern, just only an
identifier, so we also lose ability of destructuring. I think it does
not worth it, let's stay with `let` and `let mut`.

#### Blocks

Before control-flow chapter I have to establish rules about blocks,
which are different from Rust's. All blocks (in control-flow) which
enclosed into `{}` are last-statement typed (it means that the value of
block is the last expression of this block is the value and type of the
whole block).

While Rust has rules about absence of `;`, *Jacy* does not have required
`;`, so this rule cannot be applied in the same way. Let's look at some
examples:
- This block is type of `bool` and has result value `true`, even though
  we don't use this value

```clike
{true}
```

- This block will produce a type error, because block either has result
  of type of `myval` or `()` (unit type)

```clike
let a = {if myval => myval}
```

- This block won't produce a type error, because we don't use result
  value

```clike
{if myval => myval}
```

So, we already can establish some requirements about type analysis -- we
need union types which are impossible to be declared in language, but
may exist in type system.

##### One-line blocks

In this thing *Jacy* blocks differ from Rust's. I really appreciate
opportunity to declare one-line blocks without `{}`. As far as I wanna
*Jacy* to be consistent, and I established that syntax of `match`
expression arms uses `=>`, for one-line blocks we use the same syntax.
Let's look at the syntax:

```g4
while true => print('kek')
```

After `=>` we can only place one expression, and if we put `{}` compiler
will give a warning because there's no need to put `{}` after `=>`. So,
the syntax looks kind of like that:

```g4
block: `=>` expr | blockExpression | ';';
```

`{}` blocks in control-flow behaves absolutely the same way as
block-expressions.

One important thing is that function declaration has different syntax
and rules about blocks, more about that soon below.

##### Ignoring blocks

This is a feature that satisfies one definite rule from Zen --
prototyping ease. It is a pretty simple thing -- we can ignore any block
(including control-structures, `mod`s, `func`s, etc.) with `;`.

Examples:

```clike
if myval;
else doSomething()
```

Of course, I couldn't leave this thing without covering the Zen rule
about helping user with mistakes, so there will be a warning if you're
writing code like that.

> Don't confuse block-ignorance with trait method signatures, in case of
> traits it is not ignorance.

#### Control-flow

Control flow of *Jacy* is mostly inspired by Rust.

We've got `if`/`if let` as an expression, `loop` as an expression,
`while`/`while let` and `for`.

`while`/`while let` and `for` are statements, because:
- Why we need to use them as expressions if they return `()` (unit)
- I'm trying to solve the problem above, and it will be solved they'll
  become expression which returns any-type value
- If I made them expressions then it would break backwards
  compatibility:
- - You could put them in expression place, but they returned `()`, and
    in the new version they started returning some non-`()` value

#### `if`/`if let`

`if` is an expression, works same as in other languages, here's nothing
to say about except that I need to note that *Jacy* does not support
implicit `bool` conversion even through operator overloading like C++
does.

##### `if let`

`if let` is a way to check if some value matches a specific pattern.
Also, as this is a pattern matching we able to destruct our value.

Syntax is following:

```g4
ifLetExpression: 'if let' pattern '=' expr block
```

#### `while`/`while let`

`while` is a statement which works absolutely the same as `while` in
other c-like languages

`while let` is same as `while` except that its condition behaves like
`if let`.


##### `while`/`while let` as expressions

Here are some thoughts about possible solutions.

```clike
while myval {
    // Do something if `myval` is true
} else {
    // Do something if `myval` is false (at first)
}
```

It is an obvious solution, but has some problems:
- As far as `while` can return some value it must explicitly `break`
  with value. We cannot just use last statement of `while` block as
  result value, because `while` is possibly multiple-times iterable.
- If we don't `break` with value, then what would be the result? - It
  cannot be simply written in asm-like code with jumps, because we don't
  know when our `while` "does not break".

Problem example:

```clike
let a = while myval {
    if somethingElse => break true
} else {
    false
}
```

- What is the type of this `while` expression? - `bool | ()`, but we
  don't support inferred union types.

For now, I cannot come up with any good solution, so `while` is a
statement. Anyway, let's try something:

**IDEA #1** This one requires static-analysis (maybe complex):

```clike
let a = while myval {
    if somethingElse => break true
    break false
} else {
    false
}
```

We can analyse this code and say that each `break`-value is `bool`, so
we allow this.

What about this?:

```clike
let a = while myval {
    if somethingElse => break true
} else {
    false
}
```

Each `break`-value is of type `bool`, so we allow it because alternative
workflow is an infinite loop.

We required some static-analysis on `while`, which is, as I see, is not
really complex and not differs much from `if` expression value
inference. The only problem is that the use cases of `while-else` are
not really common, especially when we cover only this use case:

```clike
let a = if myval {
    let mut result = false
    while myval {
        // ...
        if somethingElse {
            result = true
            break
        }
    }
    return result
} else {
    false
}
```

#### `for`

`for`-loop is statement, not an expression, here, problems with making
it an expression are same as for `while` (read above) but even more
complex. `for`-loop in *Jacy* has only one syntax (`for ... in ...`)
same as Rust, which covers all usages (almost) of `for`-loop from C++.

Syntax is the following:

```g4
forLoop: 'for' pattern 'in' expression block
```

Examples:

```clike
// In C++ we write
for (int i = 0; i < something; i++) {
    // ...
}

// In Jacy:
for i in 0..=something {
    // ...
}

// In C++
for (const auto & x : vec) {
    // ...
}

// In Jacy
for x in &vec {
    // ...
}
```

### Compile-time evaluation

One thing I appreciate much is an ability of compile-time evaluation
(CTE - compile-time evaluat(-ion)/(-ed) further) Unlike Zig there's no
hardly separate syntax like `comptime`, etc., as far as we don't base
something else (like type parameters in Zig) on CTE, we only use for
computations.

There are some terms we need to establish:
- `const` keyword in context of CTE
- CTE context
- CTE functions
- CTE expressions

#### `const` keyword

In CTE `const` used to declare, obviously, a constant which will be
evaluated at compile-time and which usages will be inlined.

I wanna note that `const` is a synonym for compile-time evaluable
expression, so further I'll use it in this context.

`const` must be immediately assigned when declared. Syntax:

```g4
'const' IDENT '=' expr
```

After `'='` goes an expression which MUST also be CTE, but not exactly
another `const`.

The difference between `let` and `const` is that `const` is an item,
whereas `let` is a statement. As being an item `const` can be placed
mostly on any level, including top level:

```clike
const a = 10

trait MyTrait {
    const traitConst = 1010
}

func main() {
    const b = 123
}
```

#### CTE Context

In some places we cannot put run-time computed expressions, e.g. when we
declare fixed-sized array `[T; getSize()]`, `getSize()` function must be
CTE.

Here's the list of all (I hope) this kind of places (`N` is `const` and
`ctFunc()` is CTE function):
- Array types `[T; N]`
- Fill-array generator `[0; N]` (generate an array of `N` zeros)
- When we set default value for `const` parameter like `<const N: usize
  = ctFunc()>`
- Enum discriminant `enum MyEnum { Kind = N }`
- `static` items initializers

#### CTE functions

Function is CTE if:
- It is marked with `const` modifier
- It is possible to infer that function can be CTE

We mark function as CTE so:

```clike
const func foo() {}
```

Then compiler will check that all computations performed inside this
function `'foo'` are CTE, if not, it gives an error.

In other way the result of the function won't be inlined in usage
places, but it is possible to use function that wasn't qualified as
`const` in CTE context. More about that below.

##### `const` inference

Another approach is more complex for the compiler but simple for user:
If we declare a function and use it in CTE context when compiler goes to
this function and checks that it's CTE function. Anyway, if we use this
function in run-time context it won't be inlined and evaluated at
compile-time. Example:

```
// Just a simple function that returns `1`
func foo = 1

const func myConstFunc {
    const a = foo()
}

func myRawFunc {
    let a = foo()
}
```

After `const` expansion this code will look (structurally) like that:

```
func foo() = 1

const func myConstFunc() {
    const a = 1
}

func myRawFunc() {
    let a = foo()
}
```

As you can see in `myRawFunc` `foo` is still a function call, because
`foo` used in non-`const` context. Whereas in `myConstFunc` value returned by
`foo` was inlined as we declared `a` as `const`.
`const` qualifier does not mean that everything inside it will be inlined, you still can declare `let` or use `if` inside of it.
`const` just mean compiler will check function for constness and tell you if it's not.


