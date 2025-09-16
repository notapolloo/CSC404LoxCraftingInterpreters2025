My AST is not generating so everything I'm writing is in theory
```
 public Void visitVarStmt(Stmt.Var stmt) {
    Object value = null;
    if (stmt.initializer = null) {
     throw new RuntimeError(stmt.name, "Please Initialize this Variable");
    }else

    if (stmt.initializer != null) {
      value = evaluate(stmt.initializer);
    }
    
    environment.define(stmt.name.lexeme, value);
    return null;
  }
```
I've added a simple else if to check if the given variable has been initialized, and if not, 