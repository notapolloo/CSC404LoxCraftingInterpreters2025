
## Steps to adding a new instruction

- Add an constant to the `OpCode` enum in `chunk.h`.
- Update the parser table in `compiler.c`. 
  (May need to introduce a new row if it's a new token type.)
- Update the parsing function (maybe reuse `unary` or `binary`) 
  in `compiler.c`.
- Add a case to `run()` in `vm.c` to implement the op code.
- Update `disassembleInstruction()` in `debug.c`.

> For new types of values, need to update `printValue()` in `value.c`.
