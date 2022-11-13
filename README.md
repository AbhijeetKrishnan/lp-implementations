This repository has been archived.

# lp-implementations

Implementations of two of the algorithms described in the Language Processors course at Visvesvaraya National Institute of Technology -

## 1. Macro processor

Understands the C pre-processor directive `#define` and makes appropriate symbol replacements in code. 

Call using 

```bash
./macro_processor.py test[n].c test[n].i
```

## 2. Regex to NFA Converter

Parses a simple input regex and generates the code for an NFA simulator which accepts the language of the regex.

Call using

```bash
g++ regex_to_nfa.c -o regex_to_nfa.o
./regex_to_nfa.o <<< "ab*|ba*|(ab)+"
g++ decider.cpp -o decider.o
./decider.o <<< "abbb"
```
