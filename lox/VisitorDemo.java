package lox;

import lox.Expr.Binary;
import lox.Expr.Grouping;
import lox.Expr.Literal;
import lox.Expr.Unary;

public class VisitorDemo {




    public static void main(String[] args) {
        // 1 - (2 * - 3) < 4 == false
        Expr expr2 = new Expr.Binary(new Expr.Binary(new Expr.Binary(new Expr.Literal(1), 
                                                                new Token(TokenType.MINUS, "-", null, 1),
                                                                new Expr.Binary(new Expr.Literal(2),
                                                                                new Token(TokenType.STAR, "*", null, 1),
                                                                                new Expr.Unary(new Token(TokenType.MINUS, "-", null, 1),
                                                                                               new Expr.Literal(3)))), 
                                                    new Token(TokenType.LESS, "<", null, 1), 
                                                    new Expr.Literal(4)), 
                                    new Token(TokenType.EQUAL_EQUAL, "==", null, 1),
                                    new Expr.Literal(false));     

        System.out.println(expr2.accept(new CountMinus()));
    }
}

/* Task : Write an algorithm that counts the number of occurrences of a - (unary and binary) in an expression.  */
/*

expr = (.......+....)   +   (...-.......* ......-...)


    int countMinus( expr ) {
        if (expr is Literal) { return 0; }
        else if (expr is Grouping) { return countMinus(expr.expression); }
        else if (expr is Unary) { if ( expr.op == '-') { return 1 + countMinus(expr.right); } 
                                  else return countMinus(expr.right); }
        else if (expr is Binary) {
            if (expr.op == '-') {
                return 1 + countMinus(expr.left) + countMinus(expr.right);
            } else {
                return countMinus(expr.left) + countMinus(expr.right);
            }
        }
    }
 */

class CountMinus implements Expr.Visitor<Integer> {

    @Override
    public Integer visitBinaryExpr(Binary expr) {
        if (expr.operator.type == TokenType.MINUS) {
            return 1 + expr.left.accept(this) + expr.right.accept(this);
        } else {
            return expr.left.accept(this) + expr.right.accept(this);
        }
    }

    @Override
    public Integer visitGroupingExpr(Grouping expr) {
        // Conceptually a recursive call: return countMinus(expr.expression);

        return expr.expression.accept(this);
    }

    @Override
    public Integer visitLiteralExpr(Literal expr) {
        return 0;
    }

    @Override
    public Integer visitUnaryExpr(Unary expr) {
        if (expr.operator.type == TokenType.MINUS) {
            return 1 + expr.right.accept(this);
        } else {
            return expr.right.accept(this);
        }
    }

}
