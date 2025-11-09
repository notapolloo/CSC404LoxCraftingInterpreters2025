Project 8

Adding instrumentation 

Added a timer to check elapsed time per function 
I looked up how elapsed time is usually measured in C for functions and used a version of what I saw. 
```https://www.geeksforgeeks.org/cpp/how-to-measure-time-taken-by-a-program-in-c/```

I used Copilot to make a more visually appealing table for each part.
First with the constants array 
``` for (int i = 0; i < vm.chunk->constants.count; i++) {
    printf("%3d | ", i); //3 represents max ammt of space i can take when in the dump  __3
    printValue(vm.chunk->constants.values[i]);
    printf("\n");
  }```
I used the constants.values at i to print each constant that's interpreted.
I use this same idea for the other functions 
```
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
```
A the end of the result we see all accumulated tables.
I tested with the file with test.lox
and sectioned off the resulting print with "--n" tags for debugging purposes
```
--1
--2
--3
== Timer Start ==\
\== code ==\
0000    1 OP_CONSTANT         1 '5'
0002    | OP_DEFINE_GLOBAL    0 'G'
0004    3 OP_CONSTANT         2 '7'
0006    4 OP_GET_LOCAL        0
0008    | OP_CONSTANT         3 '2'
0010    | OP_ADD
0011    | OP_PRINT
0012    5 OP_POP
0013    7 OP_GET_GLOBAL       4 'G'
0015    | OP_CONSTANT         5 '4'
0017    | OP_MULTIPLY
0018    | OP_PRINT
0019    8 OP_RETURN
-------------------------------
  0 | G
  1 | 5
  2 | 7
  3 | 2
  4 | G
  5 | 4
\== code ==/
Disassembly time: 0.001000 seconds

--4
--5
--6
          ><top
0000    1 OP_CONSTANT         1 '5'
          >[ 5 ]<top
0002    | OP_DEFINE_GLOBAL    0 'G'
          ><top
0004    3 OP_CONSTANT         2 '7'
          >[ 7 ]<top
0006    4 OP_GET_LOCAL        0
          >[ 7 ][ 7 ]<top
0008    | OP_CONSTANT         3 '2'
          >[ 7 ][ 7 ][ 2 ]<top
0010    | OP_ADD
          >[ 7 ][ 9 ]<top
0011    | OP_PRINT
9
          >[ 7 ]<top
0012    5 OP_POP
          ><top
0013    7 OP_GET_GLOBAL       4 'G'
          >[ 5 ]<top
0015    | OP_CONSTANT         5 '4'
          >[ 5 ][ 4 ]<top
0017    | OP_MULTIPLY
          >[ 20 ]<top
0018    | OP_PRINT
20
          ><top
0019    8 OP_RETURN
--7
--8
InterpretResult: 0
--9

=== Globals Table ===
G = 5
Globals Table print time: 0.000000 seconds
===================

=== Constants Array ===
Constants Array print time: 0.000000 seconds
===================

=== Interned Strings Table ===
'G' = nil
Interned Strings Table print time: 0.000000 seconds
===================

=== Object Heap Linked List ===
String: 'G' (Length: 1, Hash: 3255563174)
Object Heap Linked List print time: 0.000000 seconds
===================
```