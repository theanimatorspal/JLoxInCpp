<h1 align="center">🦎 lox-cpp</h1>
<p align="center"><i>LOX language interpreter – rewritten from Crafting Interpreters (Java ➡️ C++)</i></p>
<p align="center"><b>Built with:</b> C++17 🚀 | Pure STL 📚 | No garbage collection (manual suffering)</p>
<p align="center">
  <img src="https://img.shields.io/badge/build-mostly%20working-green?style=flat-square&logo=github" />
  <img src="https://img.shields.io/badge/brain%20cells-lost-yellow?style=flat-square&logo=probot" />
  <img src="https://img.shields.io/badge/errors-daily-red?style=flat-square&logo=bugatti" />
  <img src="https://img.shields.io/badge/specification-followed-lightgrey?style=flat-square&logo=openaccess" />
  <img src="https://img.shields.io/badge/bugs-survived-critical?style=flat-square&logo=ghostery" />
  <img src="https://img.shields.io/badge/docs-your%20problem-blue?style=flat-square&logo=readthedocs" />
</p>

---

## 📜 About

This is a C++ implementation of the LOX programming language, based on the book **Crafting Interpreters** by Bob Nystrom.  
Not a line of Java was spared in the making of this mess — everything is lovingly re-implemented in modern C++17.

> 💡 Written mainly as a learning project to suffer through the pains of building an interpreter by hand.  
> 🏗️ Modular structure: Scanner, Parser, Interpreter, Resolver, Environment, Token, Expression.  
> 🔥 Total: **~8K+ lines** of pure C++ pain.

---

## 🛠️ What's (Supposedly) Working

| Component         | Status         | Notes                             |
|-------------------|-----------------|-----------------------------------|
| Scanner           | 🧹 Complete      | Tokenizes input text properly     |
| Parser            | 📖 Functional    | AST generation mostly solid       |
| Resolver          | 🕵️ Semi-working  | Variable scope resolution         |
| Interpreter       | 🧠 Runs           | Executes without immediate death |
| Error Handling    | 😵‍💫 Basic         | Could be prettier (but it works)  |
| Memory Management | 🤲 Manual         | No GC yet (hand-managed objects)  |

---

## 🦎 Sample Lox Program

```lox
fun fib(n) {
  if (n <= 1) return n;
  return fib(n - 2) + fib(n - 1);
}

print fib(10); // Output: 55
```

> 📝 Yes, recursion works. No, it's not fast. Welcome to interpreters.

---


## 🪳 Known Issues

- No Garbage Collection yet (memory leaks if you stare too hard).
- No native support for class inheritance or advanced features (yet).
- Error reporting could be more user-friendly (or at least less sarcastic).
- Multithreading? Parallelism? 🧵 Nope.
- If it breaks, you get to keep both pieces.

---

## 🎯 Future Improvements

- Implement a basic garbage collector (manual allocation tracking first).
- Add support for classes, inheritance, and method binding.
- Improve error recovery and reporting.
- Optimize interpreter for speed (currently travels through molasses).

---

> 🧠 This project is 90% for educational purposes, 10% for screaming at your monitor.
>  
> If it runs, it counts. 🏆
```
