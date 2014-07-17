
#include "Lexer.h"
#include "Token.h"


/*
  T_GO_GO,
  T_GO_IF,
  T_GO_MAP,
  T_GO_FOR,
  T_GO_VAR,
  T_GO_FUNC,
  T_GO_CASE,
  T_GO_CHAN,
  T_GO_ELSE,
  T_GO_GOTO,
  T_GO_TYPE,
  T_GO_BREAK,
  T_GO_DEFER,
  T_GO_CONST,
  T_GO_RANGE,
  T_GO_SELECT,
  T_GO_STRUCT,
  T_GO_SWITCH,
  T_GO_IMPORT,
  T_GO_RETURN,
  T_GO_DEFAULT,
  T_GO_PACKAGE,
  T_GO_CONTINUE,
  T_GO_INTERFACE,
  T_GO_FALLTHROUGH,
*/

/*
("go"),("if"),("map"),("for"),("var"),("func"),
("case"),("chan"),("else"),("goto"),("type"),("break"),
("defer"),("const"),("range"),("select"),("struct"),("switch"),
("import"),("return"),("default"),("package"),("continue"),("interface"),
("fallthrough"),
*/

namespace CPlusPlus {

static inline int golangKeywords2(const char *s) {
  if (s[0] == 'g') {
    if (s[1] == 'o') {
      return T_GO_GO;
    }
  }
  else if (s[0] == 'i') {
    if (s[1] == 'f') {
      return T_GO_IF;
    }
  }
  return T_IDENTIFIER;
}

static inline int golangKeywords3(const char *s) {
  if (s[0] == 'f') {
    if (s[1] == 'o') {
      if (s[2] == 'r') {
        return T_GO_FOR;
      }
    }
  }
  else if (s[0] == 'm') {
    if (s[1] == 'a') {
      if (s[2] == 'p') {
        return T_GO_MAP;
      }
    }
  }
  else if (s[0] == 'v') {
    if (s[1] == 'a') {
      if (s[2] == 'r') {
        return T_GO_VAR;
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int golangKeywords4(const char *s) {
  if (s[0] == 'c') {
    if (s[1] == 'a') {
      if (s[2] == 's') {
        if (s[3] == 'e') {
          return T_GO_CASE;
        }
      }
    }
    else if (s[1] == 'h') {
      if (s[2] == 'a') {
        if (s[3] == 'n') {
          return T_GO_CHAN;
        }
      }
    }
  }
  else if (s[0] == 'e') {
    if (s[1] == 'l') {
      if (s[2] == 's') {
        if (s[3] == 'e') {
          return T_GO_ELSE;
        }
      }
    }
  }
  else if (s[0] == 'f') {
    if (s[1] == 'u') {
      if (s[2] == 'n') {
        if (s[3] == 'c') {
          return T_GO_FUNC;
        }
      }
    }
  }
  else if (s[0] == 'g') {
    if (s[1] == 'o') {
      if (s[2] == 't') {
        if (s[3] == 'o') {
          return T_GO_GOTO;
        }
      }
    }
  }
  else if (s[0] == 't') {
    if (s[1] == 'y') {
      if (s[2] == 'p') {
        if (s[3] == 'e') {
          return T_GO_TYPE;
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int golangKeywords5(const char *s) {
  if (s[0] == 'b') {
    if (s[1] == 'r') {
      if (s[2] == 'e') {
        if (s[3] == 'a') {
          if (s[4] == 'k') {
            return T_GO_BREAK;
          }
        }
      }
    }
  }
  else if (s[0] == 'c') {
    if (s[1] == 'o') {
      if (s[2] == 'n') {
        if (s[3] == 's') {
          if (s[4] == 't') {
            return T_GO_CONST;
          }
        }
      }
    }
  }
  else if (s[0] == 'd') {
    if (s[1] == 'e') {
      if (s[2] == 'f') {
        if (s[3] == 'e') {
          if (s[4] == 'r') {
            return T_GO_DEFER;
          }
        }
      }
    }
  }
  else if (s[0] == 'r') {
    if (s[1] == 'a') {
      if (s[2] == 'n') {
        if (s[3] == 'g') {
          if (s[4] == 'e') {
            return T_GO_RANGE;
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int golangKeywords6(const char *s) {
  if (s[0] == 'i') {
    if (s[1] == 'm') {
      if (s[2] == 'p') {
        if (s[3] == 'o') {
          if (s[4] == 'r') {
            if (s[5] == 't') {
              return T_GO_IMPORT;
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'r') {
    if (s[1] == 'e') {
      if (s[2] == 't') {
        if (s[3] == 'u') {
          if (s[4] == 'r') {
            if (s[5] == 'n') {
              return T_GO_RETURN;
            }
          }
        }
      }
    }
  }
  else if (s[0] == 's') {
    if (s[1] == 'e') {
      if (s[2] == 'l') {
        if (s[3] == 'e') {
          if (s[4] == 'c') {
            if (s[5] == 't') {
              return T_GO_SELECT;
            }
          }
        }
      }
    }
    else if (s[1] == 't') {
      if (s[2] == 'r') {
        if (s[3] == 'u') {
          if (s[4] == 'c') {
            if (s[5] == 't') {
              return T_GO_STRUCT;
            }
          }
        }
      }
    }
    else if (s[1] == 'w') {
      if (s[2] == 'i') {
        if (s[3] == 't') {
          if (s[4] == 'c') {
            if (s[5] == 'h') {
              return T_GO_SWITCH;
            }
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int golangKeywords7(const char *s) {
  if (s[0] == 'd') {
    if (s[1] == 'e') {
      if (s[2] == 'f') {
        if (s[3] == 'a') {
          if (s[4] == 'u') {
            if (s[5] == 'l') {
              if (s[6] == 't') {
                return T_GO_DEFAULT;
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'p') {
    if (s[1] == 'a') {
      if (s[2] == 'c') {
        if (s[3] == 'k') {
          if (s[4] == 'a') {
            if (s[5] == 'g') {
              if (s[6] == 'e') {
                return T_GO_PACKAGE;
              }
            }
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int golangKeywords8(const char *s) {
  if (s[0] == 'c') {
    if (s[1] == 'o') {
      if (s[2] == 'n') {
        if (s[3] == 't') {
          if (s[4] == 'i') {
            if (s[5] == 'n') {
              if (s[6] == 'u') {
                if (s[7] == 'e') {
                  return T_GO_CONTINUE;
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

static inline int golangKeywords9(const char *s) {
  if (s[0] == 'i') {
    if (s[1] == 'n') {
      if (s[2] == 't') {
        if (s[3] == 'e') {
          if (s[4] == 'r') {
            if (s[5] == 'f') {
              if (s[6] == 'a') {
                if (s[7] == 'c') {
                  if (s[8] == 'e') {
                    return T_GO_INTERFACE;
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

static inline int golangKeywords11(const char *s) {
  if (s[0] == 'f') {
    if (s[1] == 'a') {
      if (s[2] == 'l') {
        if (s[3] == 'l') {
          if (s[4] == 't') {
            if (s[5] == 'h') {
              if (s[6] == 'r') {
                if (s[7] == 'o') {
                  if (s[8] == 'u') {
                    if (s[9] == 'g') {
                      if (s[10] == 'h') {
                        return T_GO_FALLTHROUGH;
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
  }
  return T_IDENTIFIER;
}

int Lexer::golangKeywords(const char *s, int n) {
  switch (n) {
    case 2: return golangKeywords2(s);
    case 3: return golangKeywords3(s);
    case 4: return golangKeywords4(s);
    case 5: return golangKeywords5(s);
    case 6: return golangKeywords6(s);
    case 7: return golangKeywords7(s);
    case 8: return golangKeywords8(s);
    case 9: return golangKeywords9(s);
    case 11: return golangKeywords11(s);
    default: return T_IDENTIFIER;
  } // switch
}


} //namespace CPlusPlus
