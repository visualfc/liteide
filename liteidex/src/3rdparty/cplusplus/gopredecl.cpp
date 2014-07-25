
#include "Lexer.h"
#include "Token.h"


/*
  T_GO_NIL,
  T_GO_TRUE,
  T_GO_IOTA,
  T_GO_FALSE,
*/

/*
("nil"),("true"),("iota"),("false"),
*/

namespace CPlusPlus {

static inline int golangPredecl3(const char *s) {
  if (s[0] == 'n') {
    if (s[1] == 'i') {
      if (s[2] == 'l') {
        return T_GO_NIL;
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int golangPredecl4(const char *s) {
  if (s[0] == 'i') {
    if (s[1] == 'o') {
      if (s[2] == 't') {
        if (s[3] == 'a') {
          return T_GO_IOTA;
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
  return T_IDENTIFIER;
}

static inline int golangPredecl5(const char *s) {
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
  return T_IDENTIFIER;
}

int Lexer::golangPredecl(const char *s, int n) {
  switch (n) {
    case 3: return golangPredecl3(s);
    case 4: return golangPredecl4(s);
    case 5: return golangPredecl5(s);
    default: return T_IDENTIFIER;
  } // switch
}


} //namespace CPlusPlus
