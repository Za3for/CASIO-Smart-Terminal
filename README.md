# 🧮 CASIO Smart Terminal

I built this calculator from scratch in C++ as one of my first real projects — and honestly, it taught me more than any tutorial ever could.

It started as a simple switch-case calculator. Then I kept asking myself *"why does `3 + 5 * 2` give the wrong answer?"* — and that question led me down the rabbit hole of parsers, tokenizers, and operator precedence. What came out the other side is this.

---

## ✨ What it can do

- Handles full expressions like `3 + 5 * (2 ^ 3)` correctly
- Respects operator precedence — no more wrong answers
- Right-associative power — `2^3^2 = 512` just like real math
- Trig functions: `S`, `C`, `T` (sin, cos, tan — in degrees)
- Square root with `R`, remainder with `r`, Pi with `P`
- Implicit multiplication — `2(3+4)` just works
- Unary minus — `-5 + 3` works fine
- Answer memory — `M` recalls your last result
- Actual error messages, not just "something went wrong"
- Colorful terminal UI because why not 😄

---

## 🚀 How to run it

You'll need a C++17 compiler. Then:

```bash
g++ -o calculator main.cpp -std=c++17
./calculator
```

---

## 🎮 Try it out

```
=========================================
         CASIO SMART TERMINAL
=========================================
[Ans = EMPTY] | Equation: 3 + 5 * 2
 Result => 13.0000

[Ans = 13.0000] | Equation: M * 2
 Result => 26.0000

[Ans = 26.0000] | Equation: S90
 Result => 1.0000
```

### Quick reference

| Symbol | What it does | Example |
|--------|-------------|---------|
| `+` `-` `*` `/` | Basic math | `10 / 2` |
| `^` | Power | `2^8` |
| `r` | Remainder | `10r3` → `1` |
| `R` | Square root | `R16` → `4` |
| `S` `C` `T` | sin, cos, tan | `S90` → `1` |
| `P` | Pi | `P * 2` |
| `M` | Last answer | `M + 5` |
| `q` | Quit | — |

---

## 🏗️ How it works under the hood

The part I'm most proud of — instead of a giant switch-case, I built a **Recursive Descent Parser**:

```
Your input
    ↓
Scanner — breaks "3+5*2" into tokens: [3] [+] [5] [*] [2]
    ↓
Parser
├── expression()  — handles + and -
├── term()        — handles * / r
├── power()       — handles ^
└── factor()      — handles numbers, functions, parentheses
    ↓
Correct answer ✅
```

This is actually how real compilers and interpreters work. Building it made operator precedence finally *click* for me.

---

## ⚠️ Error messages that actually help

```
[Calculation Error]: Math Error: Division by zero
[Calculation Error]: Syntax Error: Missing ')'
[Calculation Error]: Memory Error: 'Ans' is empty
[Calculation Error]: Unknown character detected: '@' at position 2
```

---

## 🔮 What's next

- [ ] Equation history
- [ ] Scientific notation support (`1.5e10`)
- [ ] Split into multiple files
- [ ] Variable storage (`x = 5`)

---

## 👨‍💻 About

First-year Cyber Security student, building projects to actually learn C++ — not just read about it.

This calculator was my first real dive into how programming languages work internally. It won't be the last. 🚀
