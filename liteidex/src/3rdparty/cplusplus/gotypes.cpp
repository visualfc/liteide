
#include "Lexer.h"
#include "Token.h"


/*
  T_GO_INT,
  T_GO_NIL,
  T_GO_BOOL,
  T_GO_INT8,
  T_GO_BYTE,
  T_GO_RUNE,
  T_GO_UINT,
  T_GO_TRUE,
  T_GO_IOTA,
  T_GO_UINT8,
  T_GO_INT16,
  T_GO_INT32,
  T_GO_INT64,
  T_GO_FALSE,
  T_GO_UINT16,
  T_GO_UINT32,
  T_GO_UINT64,
  T_GO_FLOAT32,
  T_GO_FLOAT64,
  T_GO_UINTPTR,
  T_GO_COMPLEX64,
  T_GO_COMPLEX128,
*/

/*
("int"),("nil"),("bool"),("int8"),("byte"),("rune"),
("uint"),("true"),("iota"),("uint8"),("int16"),("int32"),
("int64"),("false"),("uint16"),("uint32"),("uint64"),("float32"),
("float64"),("uintptr"),("complex64"),("complex128"),
*/

namespace CPlusPlus {

static inline int golangTypes3(const char *s) {
  if (s[0] == 'i') {
    if (s[1] == 'n') {
      if (s[2] == 't') {
        return T_GO_INT;
      }
    }
  }
  else if (s[0] == 'n') {
    if (s[1] == 'i') {
      if (s[2] == 'l') {
        return T_GO_NIL;
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int golangTypes4(const char *s) {
  if (s[0] == 'b') {
    if (s[1] == 'o') {
      if (s[2] == 'o') {
        if (s[3] == 'l') {
          return T_GO_BOOL;
        }
      }
    }
    else if (s[1] == 'y') {
      if (s[2] == 't') {
        if (s[3] == 'e') {
          return T_GO_BYTE;
        }
      }
    }
  }
  else if (s[0] == 'i') {
    if (s[1] == 'n') {
      if (s[2] == 't') {
        if (s[3] == '8') {
          return T_GO_INT8;
        }
      }
    }
    else if (s[1] == 'o') {
      if (s[2] == 't') {
        if (s[3] == 'a') {
          return T_GO_IOTA;
        }
      }
    }
  }
  else if (s[0] == 'r') {
    if (s[1] == 'u') {
      if (s[2] == 'n') {
        if (s[3] == 'e') {
          return T_GO_RUNE;
        }
      }
    }
  }
  else if (s[0] == 't') {
    if (s[1] == 'r') {
      if (s[2] == 'u') {
        if (s[3] == 'e') {
          return T_GO_TRUE;
        }
      }
    }
  }
  else if (s[0] == 'u') {
    if (s[1] == 'i') {
      if (s[2] == 'n') {
        if (s[3] == 't') {
          return T_GO_UINT;
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int golangTypes5(const char *s) {
  if (s[0] == 'f') {
    if (s[1] == 'a') {
      if (s[2] == 'l') {
        if (s[3] == 's') {
          if (s[4] == 'e') {
            return T_GO_FALSE;
          }
        }
      }
    }
  }
  else if (s[0] == 'i') {
    if (s[1] == 'n') {
      if (s[2] == 't') {
        if (s[3] == '1') {
          if (s[4] == '6') {
            return T_GO_INT16;
          }
        }
        else if (s[3] == '3') {
          if (s[4] == '2') {
            return T_GO_INT32;
          }
        }
        else if (s[3] == '6') {
          if (s[4] == '4') {
            return T_GO_INT64;
          }
        }
      }
    }
  }
  else if (s[0] == 'u') {
    if (s[1] == 'i') {
      if (s[2] == 'n') {
        if (s[3] == 't') {
          if (s[4] == '8') {
            return T_GO_UINT8;
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int golangTypes6(const char *s) {
  if (s[0] == 'u') {
    if (s[1] == 'i') {
      if (s[2] == 'n') {
        if (s[3] == 't') {
          if (s[4] == '1') {
            if (s[5] == '6') {
              return T_GO_UINT16;
            }
          }
          else if (s[4] == '3') {
            if (s[5] == '2') {
              return T_GO_UINT32;
            }
          }
          else if (s[4] == '6') {
            if (s[5] == '4') {
              return T_GO_UINT64;
            }
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int golangTypes7(const char *s) {
  if (s[0] == 'f') {
    if (s[1] == 'l') {
      if (s[2] == 'o') {
        if (s[3] == 'a') {
          if (s[4] == 't') {
            if (s[5] == '3') {
              if (s[6] == '2') {
                return T_GO_FLOAT32;
              }
            }
            else if (s[5] == '6') {
              if (s[6] == '4') {
                return T_GO_FLOAT64;
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'u') {
    if (s[1] == 'i') {
      if (s[2] == 'n') {
        if (s[3] == 't') {
          if (s[4] == 'p') {
            if (s[5] == 't') {
              if (s[6] == 'r') {
                return T_GO_UINTPTR;
              }
            }
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int golangTypes9(const char *s) {
  if (s[0] == 'c') {
    if (s[1] == 'o') {
      if (s[2] == 'm') {
        if (s[3] == 'p') {
          if (s[4] == 'l') {
            if (s[5] == 'e') {
              if (s[6] == 'x') {
                if (s[7] == '6') {
                  if (s[8] == '4') {
                    return T_GO_COMPLEX64;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int golangTypes10(const char *s) {
  if (s[0] == 'c') {
    if (s[1] == 'o') {
      if (s[2] == 'm') {
        if (s[3] == 'p') {
          if (s[4] == 'l') {
            if (s[5] == 'e') {
              if (s[6] == 'x') {
                if (s[7] == '1') {
                  if (s[8] == '2') {
                    if (s[9] == '8') {
                      return T_GO_COMPLEX128;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

int Lexer::golangTypes(const char *s, int n) {
  switch (n) {
    case 3: return golangTypes3(s);
    case 4: return golangTypes4(s);
    case 5: return golangTypes5(s);
    case 6: return golangTypes6(s);
    case 7: return golangTypes7(s);
    case 9: return golangTypes9(s);
    case 10: return golangTypes10(s);
    default: return T_IDENTIFIER;
  } // switch
}


} //namespace CPlusPlus
