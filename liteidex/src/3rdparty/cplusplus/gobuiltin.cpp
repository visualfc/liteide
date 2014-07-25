
#include "Lexer.h"
#include "Token.h"


/*
  T_GO_CAP,
  T_GO_LEN,
  T_GO_NEW,
  T_GO_COPY,
  T_GO_IMAG,
  T_GO_MAKE,
  T_GO_REAL,
  T_GO_CLOSE,
  T_GO_PANIC,
  T_GO_PRINT,
  T_GO_APPEND,
  T_GO_DELETE,
  T_GO_COMPLEX,
  T_GO_PRINTLN,
  T_GO_RECOVER,
*/

/*
("cap"),("len"),("new"),("copy"),("imag"),("make"),
("real"),("close"),("panic"),("print"),("append"),("delete"),
("complex"),("println"),("recover"),
*/

namespace CPlusPlus {

static inline int golangBuiltinFunc3(const char *s) {
  if (s[0] == 'c') {
    if (s[1] == 'a') {
      if (s[2] == 'p') {
        return T_GO_CAP;
      }
    }
  }
  else if (s[0] == 'l') {
    if (s[1] == 'e') {
      if (s[2] == 'n') {
        return T_GO_LEN;
      }
    }
  }
  else if (s[0] == 'n') {
    if (s[1] == 'e') {
      if (s[2] == 'w') {
        return T_GO_NEW;
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int golangBuiltinFunc4(const char *s) {
  if (s[0] == 'c') {
    if (s[1] == 'o') {
      if (s[2] == 'p') {
        if (s[3] == 'y') {
          return T_GO_COPY;
        }
      }
    }
  }
  else if (s[0] == 'i') {
    if (s[1] == 'm') {
      if (s[2] == 'a') {
        if (s[3] == 'g') {
          return T_GO_IMAG;
        }
      }
    }
  }
  else if (s[0] == 'm') {
    if (s[1] == 'a') {
      if (s[2] == 'k') {
        if (s[3] == 'e') {
          return T_GO_MAKE;
        }
      }
    }
  }
  else if (s[0] == 'r') {
    if (s[1] == 'e') {
      if (s[2] == 'a') {
        if (s[3] == 'l') {
          return T_GO_REAL;
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int golangBuiltinFunc5(const char *s) {
  if (s[0] == 'c') {
    if (s[1] == 'l') {
      if (s[2] == 'o') {
        if (s[3] == 's') {
          if (s[4] == 'e') {
            return T_GO_CLOSE;
          }
        }
      }
    }
  }
  else if (s[0] == 'p') {
    if (s[1] == 'a') {
      if (s[2] == 'n') {
        if (s[3] == 'i') {
          if (s[4] == 'c') {
            return T_GO_PANIC;
          }
        }
      }
    }
    else if (s[1] == 'r') {
      if (s[2] == 'i') {
        if (s[3] == 'n') {
          if (s[4] == 't') {
            return T_GO_PRINT;
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int golangBuiltinFunc6(const char *s) {
  if (s[0] == 'a') {
    if (s[1] == 'p') {
      if (s[2] == 'p') {
        if (s[3] == 'e') {
          if (s[4] == 'n') {
            if (s[5] == 'd') {
              return T_GO_APPEND;
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'd') {
    if (s[1] == 'e') {
      if (s[2] == 'l') {
        if (s[3] == 'e') {
          if (s[4] == 't') {
            if (s[5] == 'e') {
              return T_GO_DELETE;
            }
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int golangBuiltinFunc7(const char *s) {
  if (s[0] == 'c') {
    if (s[1] == 'o') {
      if (s[2] == 'm') {
        if (s[3] == 'p') {
          if (s[4] == 'l') {
            if (s[5] == 'e') {
              if (s[6] == 'x') {
                return T_GO_COMPLEX;
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'p') {
    if (s[1] == 'r') {
      if (s[2] == 'i') {
        if (s[3] == 'n') {
          if (s[4] == 't') {
            if (s[5] == 'l') {
              if (s[6] == 'n') {
                return T_GO_PRINTLN;
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'r') {
    if (s[1] == 'e') {
      if (s[2] == 'c') {
        if (s[3] == 'o') {
          if (s[4] == 'v') {
            if (s[5] == 'e') {
              if (s[6] == 'r') {
                return T_GO_RECOVER;
              }
            }
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

int Lexer::golangBuiltinFunc(const char *s, int n) {
  switch (n) {
    case 3: return golangBuiltinFunc3(s);
    case 4: return golangBuiltinFunc4(s);
    case 5: return golangBuiltinFunc5(s);
    case 6: return golangBuiltinFunc6(s);
    case 7: return golangBuiltinFunc7(s);
    default: return T_IDENTIFIER;
  } // switch
}


} //namespace CPlusPlus
