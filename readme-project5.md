I did the last option the extention.
I've created a native function that shows the amout of classes that exist in lox per session.
I created a database for all created classes and made it so it is impossible to create a duplicate class.
First, I created a list in the interpreter. They hold the names of each class. 
Then in the globals section. I created 2 native functions
ClassNum and ClassDisplay which show the number of classes and the class list respectively.
This can be tested in the terminial.

Welcome to jlox!
> classNum();   
0
> class Bagel{} 
> classNum();  
1
> classDisplay();
[Bagel]
> class Pizza{}
> classNum();    
2
> class Bagel{}  
Runtime error at IDENTIFIER Bagel null: This class already exists.
