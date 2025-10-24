Code I changed.
made T conditionals for each OP minus swap to avoid redundancy 

//-------------
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

//-------------
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
      //-------------