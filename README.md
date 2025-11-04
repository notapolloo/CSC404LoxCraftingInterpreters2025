
Added the token types to the scanner    
I asked Coplt what files would need to be changed and said only the compilers, scanners, chunk h, and vm

First I went to the scanner and added the tokentypes
scanner.h
```TOKEN_QUESTION, TOKEN_COLON,
static const char* TokenTypeNames[] = {
    "TOKEN_LEFT_PAREN", "TOKEN_RIGHT_PAREN",
    "TOKEN_LEFT_BRACE", "TOKEN_RIGHT_BRACE",
    "TOKEN_COMMA", "TOKEN_DOT", "TOKEN_MINUS", "TOKEN_PLUS",
    "TOKEN_SEMICOLON", "TOKEN_SLASH", "TOKEN_STAR",
    "TOKEN_QUESTION", "TOKEN_COLON",```
scanner.c
   ``` case '?': return makeToken(TOKEN_QUESTION);
    case ':': return makeToken(TOKEN_COLON);```

Added ternary to the compiler
compiler.c
 ``` PREC_CONDITIONAL, // ?:```

 Added the operation to chunk.h
```  OP_CONDITIONAL,
```

Added function for compiling conditionals in front of binary to avoid a condition being sent to the 
wrong funtion

```static void conditional() {
  // Compile the middle expression (between ? and :)
  parsePrecedence(PREC_OR);
  
  consume(TOKEN_COLON, "Expect ':' after middle expression in conditional.");
  
  // Compile the right expression (after :)
  parsePrecedence(PREC_ASSIGNMENT);
  
  // Emit conditional instruction
  emitByte(OP_CONDITIONAL);
}```

Update rules table
```  [TOKEN_QUESTION]      = {NULL,     conditional, PREC_CONDITIONAL},
  [TOKEN_COLON]        = {NULL,     NULL,   PREC_NONE},```

Add the conditionals to
vm.c 
```      case OP_CONDITIONAL: {
        Value falseValue = pop();  //else
        Value trueValue = pop();   //then
        Value condition = pop();   //condition
        push(isFalsey(condition) ? falseValue : trueValue);
        break;
      }```