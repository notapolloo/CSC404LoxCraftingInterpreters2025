#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "vm.h"

VM vm; 

static void resetStack() {
  vm.stackTop = vm.stack;
}

void initVM() {
  resetStack();
}

void freeVM() {
}

void push(Value value) {
  *vm.stackTop = value;
  vm.stackTop++;
}

Value pop() {
  vm.stackTop--;
  return *vm.stackTop;
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
    do { \
      double b = pop(); \
      double a = pop(); \
      push(a op b); \
    } while (false)

#define BINARY_OP_MINMAX(op) \
    do { \
      double b = pop(); \
      double a = pop(); \
       push(a op b ? a : b); \
    } while (false)

#define BINARY_OP_INCREM(op) \
    do { \
      double a = pop(); \
      push(a op 1); \
    } while (false)
        
  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("          >");
    for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
    printf("<top\n");
    disassembleInstruction(vm.chunk,
                           (int)(vm.ip - vm.chunk->code));
#endif

    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
      case OP_CONSTANT: {
        Value constant = READ_CONSTANT();
        push(constant);
        break;
      }
      case OP_ADD:      BINARY_OP(+); break;
      case OP_SUBTRACT: BINARY_OP(-); break;
      case OP_MULTIPLY: BINARY_OP(*); break;
      case OP_DIVIDE:   BINARY_OP(/); break;
      case OP_NEGATE: 
        Value value = pop();
        push(-value);
        break;
      
      case OP_MIN: BINARY_OP_MINMAX(<); break;
      case OP_MAX:BINARY_OP_MINMAX(>);break;
      
      case OP_INCREMENT: BINARY_OP_INCREM(+); break;
      case OP_DECREMENT: BINARY_OP_INCREM(-); break;
      case OP_SWAP: 
        Value b = pop();
        Value a = pop();
        push(b);
        push(a);
        break;
      
      case OP_RETURN: 
        printf("Terminating.\n");
        printValue(pop());
        printf("\n");
        return INTERPRET_OK;
      
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(const char* source) {
  compile(source);
  return INTERPRET_OK;
}
