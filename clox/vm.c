#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "object.h"
#include "memory.h"
#include "vm.h"
#include <time.h> //p7

VM vm; 
static clock_t start, end; //p7

static void resetStack() {
  vm.stackTop = vm.stack;
}

static void runtimeError(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);
  
  size_t instruction = vm.ip - vm.chunk->code - 1;
  int line = vm.chunk->lines[instruction];
  fprintf(stderr, "[line %d] in script\n", line);
  resetStack();
}

void initVM() {
  resetStack();
  vm.objects = NULL;
  
  initTable(&vm.globals);
  initTable(&vm.strings);
}

void freeVM() {
  freeTable(&vm.globals);
  freeTable(&vm.strings);
  freeObjects();
}

void push(Value value) {
  *vm.stackTop = value;
  vm.stackTop++;
}

Value pop() {
  vm.stackTop--;
  return *vm.stackTop;
}

static Value peek(int distance) {
  return vm.stackTop[-1 - distance];
}

static bool isFalsey(Value value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate() {
  ObjString* b = AS_STRING(pop());
  ObjString* a = AS_STRING(pop());
  
  int length = a->length + b->length;
  char* chars = ALLOCATE(char, length + 1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';
  
  ObjString* result = takeString(chars, length);
  push(OBJ_VAL(result));
}

static void printGlobalsTable() { //p7
  printf("\n=== Globals Table ===\n");
  start = clock(); //p7
  for (int i = 0; i < vm.globals.capacity; i++) {
    Entry* entry = &vm.globals.entries[i];
    if (entry->key != NULL) {
      printf("%s = ", entry->key->chars);
      printValue(entry->value);
      printf("\n");
    }
  }
  end = clock(); //p7
  double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; //p7
  printf("Globals Table print time: %f seconds\n", cpu_time_used); //p
  printf("===================\n");
}

static void printConstantsArray(){ //p7
  printf("\n=== Constants Array ===\n");
  start = clock(); //p7
  for (int i = 0; i < vm.chunk->constants.count; i++) {
    printf("%3d | ", i); //3 represents max ammt of space i can take when in the dump  __3
    printValue(vm.chunk->constants.values[i]);
    printf("\n");
  }
  end = clock(); //p7
  double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; //p7
  printf("Constants Array print time: %f seconds\n", cpu_time_used); //p
  printf("===================\n");
  
}

static void internedStringsTable(){ //p7
  printf("\n=== Interned Strings Table ===\n");
  start = clock(); //p7 
  for (int i = 0; i < vm.strings.capacity; i++) {
    Entry* entry = &vm.strings.entries[i];
    if (entry->key != NULL) {
      printf("'%s' = ", entry->key->chars);
      printValue(entry->value);
      printf("\n");
    }
  }
  end = clock(); //p7
  double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; //p7
  printf("Interned Strings Table print time: %f seconds\n", cpu_time_used); //p
  printf("===================\n");
}

static void objectHeapLinkedList(){ //p7
  printf("\n=== Object Heap Linked List ===\n");
  start = clock(); //p7
  Obj* current = vm.objects;
  while (current != NULL) {
    ObjString* stringObj = (ObjString*)current;
    printf("String: '%s' (Length: %d, Hash: %u)\n", stringObj->chars, stringObj->length, stringObj->hash);
    
    // Add handling for other object types here if needed
    current = current->next;
  }
  end = clock(); //p7
  double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; //p7
  printf("Object Heap Linked List print time: %f seconds\n", cpu_time_used); //p
  printf("===================\n");
}

static InterpretResult run() {
  #define READ_BYTE() (*vm.ip++)
  #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
  #define READ_STRING() AS_STRING(READ_CONSTANT())
  #define BINARY_OP(valueType, op) \
  do { \
    if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
      runtimeError("Operands must be numbers."); \
      return INTERPRET_RUNTIME_ERROR; \
    } \
    double b = AS_NUMBER(pop()); \
    double a = AS_NUMBER(pop()); \
    push(valueType(a op b)); \
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
        case OP_NIL: push(NIL_VAL); break;
        case OP_TRUE: push(BOOL_VAL(true)); break;
        case OP_FALSE: push(BOOL_VAL(false)); break;
        case OP_POP: pop(); break;
        case OP_GET_GLOBAL: {
          ObjString* name = READ_STRING();
          Value value;
          if (!tableGet(&vm.globals, name, &value)) {
            runtimeError("Undefined variable '%s'.", name->chars);
            return INTERPRET_RUNTIME_ERROR;
          }
          push(value);
          break;
        }
        case OP_DEFINE_GLOBAL: {
          ObjString* name = READ_STRING();
          tableSet(&vm.globals, name, peek(0));
          pop();
          break;
        }
        case OP_GET_LOCAL: {
          uint8_t slot = READ_BYTE();
          push(vm.stack[slot]); 
          break;
        }
        case OP_SET_LOCAL: {
          uint8_t slot = READ_BYTE();
          vm.stack[slot] = peek(0);
          break;
        }
        case OP_SET_GLOBAL: {
          ObjString* name = READ_STRING();
          if (tableSet(&vm.globals, name, peek(0))) {
            tableDelete(&vm.globals, name); 
            runtimeError("Undefined variable '%s'.", name->chars);
            return INTERPRET_RUNTIME_ERROR;
          }
          break;
        }
        case OP_EQUAL: {
          Value b = pop();
          Value a = pop();
          push(BOOL_VAL(valuesEqual(a, b)));
          break;
        }
        case OP_GREATER:  BINARY_OP(BOOL_VAL, >); break;
        case OP_LESS:     BINARY_OP(BOOL_VAL, <); break;
        case OP_ADD: {
          if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
            concatenate();
          } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
            double b = AS_NUMBER(pop());
            double a = AS_NUMBER(pop());
            push(NUMBER_VAL(a + b));
          } else {
            runtimeError(
              "Operands must be two numbers or two strings.");
              return INTERPRET_RUNTIME_ERROR;
            }
            break;
          }
          case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
          case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
          case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /); break;
          case OP_NOT:
          push(BOOL_VAL(isFalsey(pop())));
          break;
          case OP_NEGATE:
          if (!IS_NUMBER(peek(0))) {
            runtimeError("Operand must be a number.");
            return INTERPRET_RUNTIME_ERROR;
          }
          push(NUMBER_VAL(-AS_NUMBER(pop())));
          break;
          case OP_PRINT: {
            printValue(pop());
            printf("\n");
            break;
          }
          case OP_RETURN: {
            // Exit interpreter.
            
            return INTERPRET_OK;
          }
        }
        
        
      }
      
      #undef READ_BYTE
      #undef READ_CONSTANT
      #undef READ_STRING
      #undef BINARY_OP
    }
    
    InterpretResult interpret(const char* source) {
      printf("--1\n");
      Chunk chunk;
      printf("--2\n");
      initChunk(&chunk);
      printf("--3\n");
      //debugging
      if (!compile(source, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
      }
      //debugging
      printf("--4\n");
      vm.chunk = &chunk;
      printf("--5\n");
      vm.ip = vm.chunk->code;
      
      printf("--6\n");
      InterpretResult result = run();
      printf("--7\n");
      freeChunk(&chunk);
      printf("--8\n");
      printf("InterpretResult: %d\n", result);
      printf("--9\n");
      
      printGlobalsTable();  // Display globals before returning
      printConstantsArray(); //p7
      internedStringsTable(); //p7
      objectHeapLinkedList(); //p7
      return result;
      
      
    }
    