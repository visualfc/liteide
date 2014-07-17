// Copyright (c) 2008 Roberto Raggi <roberto.raggi@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "Lexer.h"
#include "Token.h"

namespace CPlusPlus {

static inline int classify2(const char *s, LanguageFeatures)
{
  if (s[0] == 'd') {
    if (s[1] == 'o') {
      return T_DO;
    }
  }
  else if (s[0] == 'i') {
    if (s[1] == 'f') {
      return T_IF;
    }
  }
  return T_IDENTIFIER;
}

static inline int classify3(const char *s, LanguageFeatures features)
{
  if (s[0] == 'a') {
    if (s[1] == 's') {
      if (s[2] == 'm') {
        return T_ASM;
      }
    }
  }
  else if (s[0] == 'f') {
    if (s[1] == 'o') {
      if (s[2] == 'r') {
        return T_FOR;
      }
    }
  }
  else if (s[0] == 'i') {
    if (s[1] == 'n') {
      if (s[2] == 't') {
        return T_INT;
      }
    }
  }
  else if (s[0] == 'n') {
    if (s[1] == 'e') {
      if (s[2] == 'w') {
        return T_NEW;
      }
    }
  }
  else if (s[0] == 't') {
    if (s[1] == 'r') {
      if (s[2] == 'y') {
        return T_TRY;
      }
    }
  }
  else if (features.qtMocRunEnabled && s[0] == 'Q') {
    if (s[1] == '_') {
      if (s[2] == 'D') {
        return T_Q_D;
      }
      else if (s[2] == 'Q') {
        return T_Q_Q;
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int classify4(const char *s, LanguageFeatures features)
{
  if (s[0] == 'a') {
    if (s[1] == 'u') {
      if (s[2] == 't') {
        if (s[3] == 'o') {
          return T_AUTO;
        }
      }
    }
  }
  else if (s[0] == 'b') {
    if (s[1] == 'o') {
      if (s[2] == 'o') {
        if (s[3] == 'l') {
          return T_BOOL;
        }
      }
    }
  }
  else if (s[0] == 'c') {
    if (s[1] == 'a') {
      if (s[2] == 's') {
        if (s[3] == 'e') {
          return T_CASE;
        }
      }
    }
    else if (s[1] == 'h') {
      if (s[2] == 'a') {
        if (s[3] == 'r') {
          return T_CHAR;
        }
      }
    }
  }
  else if (s[0] == 'e') {
    if (s[1] == 'l') {
      if (s[2] == 's') {
        if (s[3] == 'e') {
          return T_ELSE;
        }
      }
    }
    else if (s[1] == 'n') {
      if (s[2] == 'u') {
        if (s[3] == 'm') {
          return T_ENUM;
        }
      }
    }
    else if (features.qtKeywordsEnabled && s[1] == 'm') {
      if (s[2] == 'i') {
        if (s[3] == 't') {
          return T_EMIT;
        }
      }
    }
  }
  else if (s[0] == 'g') {
    if (s[1] == 'o') {
      if (s[2] == 't') {
        if (s[3] == 'o') {
          return T_GOTO;
        }
      }
    }
  }
  else if (s[0] == 'l') {
    if (s[1] == 'o') {
      if (s[2] == 'n') {
        if (s[3] == 'g') {
          return T_LONG;
        }
      }
    }
  }
  else if (s[0] == 't') {
    if (s[1] == 'h') {
      if (s[2] == 'i') {
        if (s[3] == 's') {
          return T_THIS;
        }
      }
    }
    else if (s[1] == 'r') {
      if (s[2] == 'u') {
        if (s[3] == 'e') {
          return T_TRUE;
        }
      }
    }
  }
  else if (s[0] == 'v') {
    if (s[1] == 'o') {
      if (s[2] == 'i') {
        if (s[3] == 'd') {
          return T_VOID;
        }
      }
    }
  }
  else if (features.qtEnabled && s[0] == 'S') {
    if (s[1] == 'L') {
      if (s[2] == 'O') {
        if (s[3] == 'T') {
          return T_SLOT;
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int classify5(const char *s, LanguageFeatures features)
{
  if (s[0] == '_') {
    if (s[1] == '_') {
      if (s[2] == 'a') {
        if (s[3] == 's') {
          if (s[4] == 'm') {
            return T___ASM;
          }
        }
      }
    }
  }
  else if (s[0] == 'b') {
    if (s[1] == 'r') {
      if (s[2] == 'e') {
        if (s[3] == 'a') {
          if (s[4] == 'k') {
            return T_BREAK;
          }
        }
      }
    }
  }
  else if (s[0] == 'c') {
    if (s[1] == 'a') {
      if (s[2] == 't') {
        if (s[3] == 'c') {
          if (s[4] == 'h') {
            return T_CATCH;
          }
        }
      }
    }
    else if (s[1] == 'l') {
      if (s[2] == 'a') {
        if (s[3] == 's') {
          if (s[4] == 's') {
            return T_CLASS;
          }
        }
      }
    }
    else if (s[1] == 'o') {
      if (s[2] == 'n') {
        if (s[3] == 's') {
          if (s[4] == 't') {
            return T_CONST;
          }
        }
      }
    }
  }
  else if (s[0] == 'f') {
    if (s[1] == 'a') {
      if (s[2] == 'l') {
        if (s[3] == 's') {
          if (s[4] == 'e') {
            return T_FALSE;
          }
        }
      }
    }
    else if (s[1] == 'l') {
      if (s[2] == 'o') {
        if (s[3] == 'a') {
          if (s[4] == 't') {
            return T_FLOAT;
          }
        }
      }
    }
  }
  else if (s[0] == 's') {
    if (s[1] == 'h') {
      if (s[2] == 'o') {
        if (s[3] == 'r') {
          if (s[4] == 't') {
            return T_SHORT;
          }
        }
      }
    }
    else if (features.qtKeywordsEnabled) {
      if (s[1] == 'l') {
        if (s[2] == 'o') {
          if (s[3] == 't') {
            if (s[4] == 's') {
              return T_Q_SLOTS;
            }
          }
        }
      }
    }
  }
  else if (s[0] == 't') {
    if (s[1] == 'h') {
      if (s[2] == 'r') {
        if (s[3] == 'o') {
          if (s[4] == 'w') {
            return T_THROW;
          }
        }
      }
    }
  }
  else if (s[0] == 'u') {
    if (s[1] == 'n') {
      if (s[2] == 'i') {
        if (s[3] == 'o') {
          if (s[4] == 'n') {
            return T_UNION;
          }
        }
      }
    }
    else if (s[1] == 's') {
      if (s[2] == 'i') {
        if (s[3] == 'n') {
          if (s[4] == 'g') {
            return T_USING;
          }
        }
      }
    }
  }
  else if (s[0] == 'w') {
    if (s[1] == 'h') {
      if (s[2] == 'i') {
        if (s[3] == 'l') {
          if (s[4] == 'e') {
            return T_WHILE;
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int classify6(const char *s, LanguageFeatures features)
{
  if (s[0] == 'd') {
    if (s[1] == 'e') {
      if (s[2] == 'l') {
        if (s[3] == 'e') {
          if (s[4] == 't') {
            if (s[5] == 'e') {
              return T_DELETE;
            }
          }
        }
      }
    }
    else if (s[1] == 'o') {
      if (s[2] == 'u') {
        if (s[3] == 'b') {
          if (s[4] == 'l') {
            if (s[5] == 'e') {
              return T_DOUBLE;
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'e') {
    if (s[1] == 'x') {
      if (s[2] == 'p') {
        if (s[3] == 'o') {
          if (s[4] == 'r') {
            if (s[5] == 't') {
              return T_EXPORT;
            }
          }
        }
      }
      else if (s[2] == 't') {
        if (s[3] == 'e') {
          if (s[4] == 'r') {
            if (s[5] == 'n') {
              return T_EXTERN;
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'f') {
    if (s[1] == 'r') {
      if (s[2] == 'i') {
        if (s[3] == 'e') {
          if (s[4] == 'n') {
            if (s[5] == 'd') {
              return T_FRIEND;
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'i') {
    if (s[1] == 'n') {
      if (s[2] == 'l') {
        if (s[3] == 'i') {
          if (s[4] == 'n') {
            if (s[5] == 'e') {
              return T_INLINE;
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'p') {
    if (s[1] == 'u') {
      if (s[2] == 'b') {
        if (s[3] == 'l') {
          if (s[4] == 'i') {
            if (s[5] == 'c') {
              return T_PUBLIC;
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
              return T_RETURN;
            }
          }
        }
      }
    }
  }
  else if (s[0] == 's') {
    if (s[1] == 'i') {
      if (s[2] == 'g') {
        if (s[3] == 'n') {
          if (s[4] == 'e') {
            if (s[5] == 'd') {
              return T_SIGNED;
            }
          }
        }
      }
      else if (s[2] == 'z') {
        if (s[3] == 'e') {
          if (s[4] == 'o') {
            if (s[5] == 'f') {
              return T_SIZEOF;
            }
          }
        }
      }
    }
    else if (s[1] == 't') {
      if (s[2] == 'a') {
        if (s[3] == 't') {
          if (s[4] == 'i') {
            if (s[5] == 'c') {
              return T_STATIC;
            }
          }
        }
      }
      else if (s[2] == 'r') {
        if (s[3] == 'u') {
          if (s[4] == 'c') {
            if (s[5] == 't') {
              return T_STRUCT;
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
              return T_SWITCH;
            }
          }
        }
      }
    }
  }
  else if (s[0] == 't') {
    if (s[1] == 'y') {
      if (s[2] == 'p') {
        if (s[3] == 'e') {
          if (s[4] == 'i') {
            if (s[5] == 'd') {
              return T_TYPEID;
            }
          }
          else if (s[4] == 'o') {
            if (s[5] == 'f') {
              return T_TYPEOF;
            }
          }
        }
      }
    }
  }
  else if (features.qtKeywordsEnabled && s[0] == 'S') {
    if (s[1] == 'I') {
      if (s[2] == 'G') {
        if (s[3] == 'N') {
          if (s[4] == 'A') {
            if (s[5] == 'L') {
              return T_SIGNAL;
            }
          }
        }
      }
    }
  }
  else if (features.qtKeywordsEnabled && s[0] == 'Q') {
    if (s[1] == '_') {
      if (s[2] == 'S') {
        if (s[3] == 'L') {
          if (s[4] == 'O') {
            if (s[5] == 'T') {
              return T_Q_SLOT;
            }
          }
        }
      }
      else if (s[2] == 'E') {
        if (s[3] == 'M') {
          if (s[4] == 'I') {
            if (s[5] == 'T') {
              return T_Q_EMIT;
            }
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int classify7(const char *s, LanguageFeatures features)
{
  if (s[0] == '_') {
    if (s[1] == '_') {
      if (s[2] == 'a') {
        if (s[3] == 's') {
          if (s[4] == 'm') {
            if (s[5] == '_') {
              if (s[6] == '_') {
                return T___ASM__;
              }
            }
          }
        }
      }
      else if (s[2] == 'c') {
        if (s[3] == 'o') {
          if (s[4] == 'n') {
            if (s[5] == 's') {
              if (s[6] == 't') {
                return T___CONST;
              }
            }
          }
        }
      }
    }
  }
  else if (features.cxx11Enabled && s[0] == 'a') {
    if (s[1] == 'l') {
      if (s[2] == 'i') {
        if (s[3] == 'g') {
          if (s[4] == 'n') {
            if (s[5] == 'a') {
              if (s[6] == 's') {
                return T_ALIGNAS;
              }
            }
            else if (s[5] == 'o') {
              if (s[6] == 'f') {
                return T_ALIGNOF;
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'd') {
    if (s[1] == 'e') {
      if (s[2] == 'f') {
        if (s[3] == 'a') {
          if (s[4] == 'u') {
            if (s[5] == 'l') {
              if (s[6] == 't') {
                return T_DEFAULT;
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'm') {
    if (s[1] == 'u') {
      if (s[2] == 't') {
        if (s[3] == 'a') {
          if (s[4] == 'b') {
            if (s[5] == 'l') {
              if (s[6] == 'e') {
                return T_MUTABLE;
              }
            }
          }
        }
      }
    }
  }
  else if (features.cxx11Enabled && s[0] == 'n') {
    if (s[1] == 'u') {
      if (s[2] == 'l') {
        if (s[3] == 'l') {
          if (s[4] == 'p') {
            if (s[5] == 't') {
              if (s[6] == 'r') {
                return T_NULLPTR;
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
        if (s[3] == 'v') {
          if (s[4] == 'a') {
            if (s[5] == 't') {
              if (s[6] == 'e') {
                return T_PRIVATE;
              }
            }
          }
        }
      }
    }
  }
  else if (features.qtKeywordsEnabled && s[0] == 'f') {
    if (s[1] == 'o') {
      if (s[2] == 'r') {
        if (s[3] == 'e') {
          if (s[4] == 'a') {
            if (s[5] == 'c') {
              if (s[6] == 'h') {
                return T_Q_FOREACH;
              }
            }
          }
        }
      }
    }
  }
  else if (features.qtEnabled && s[0] == 's') {
    if (s[1] == 'i') {
      if (s[2] == 'g') {
        if (s[3] == 'n') {
          if (s[4] == 'a') {
            if (s[5] == 'l') {
              if (s[6] == 's') {
                return T_Q_SIGNALS;
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 't') {
    if (s[1] == 'y') {
      if (s[2] == 'p') {
        if (s[3] == 'e') {
          if (s[4] == 'd') {
            if (s[5] == 'e') {
              if (s[6] == 'f') {
                return T_TYPEDEF;
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'v') {
    if (s[1] == 'i') {
      if (s[2] == 'r') {
        if (s[3] == 't') {
          if (s[4] == 'u') {
            if (s[5] == 'a') {
              if (s[6] == 'l') {
                return T_VIRTUAL;
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'w') {
    if (s[1] == 'c') {
      if (s[2] == 'h') {
        if (s[3] == 'a') {
          if (s[4] == 'r') {
            if (s[5] == '_') {
              if (s[6] == 't') {
                return T_WCHAR_T;
              }
            }
          }
        }
      }
    }
  }
  else if (features.qtEnabled && s[0] == 'Q') {
    if (s[1] == '_') {
      if (s[2] == 'S') {
        if (s[3] == 'L') {
          if (s[4] == 'O') {
            if (s[5] == 'T') {
              if (s[6] == 'S') {
                return T_Q_SLOTS;
              }
            }
          }
        }
      }
      else if (s[2] == 'E') {
        if (s[3] == 'N') {
          if (s[4] == 'U') {
            if (s[5] == 'M') {
              if (s[6] == 'S') {
                return T_Q_ENUMS;
              }
            }
          }
        }
      }
      else if (s[2] == 'F') {
        if (s[3] == 'L') {
          if (s[4] == 'A') {
            if (s[5] == 'G') {
              if (s[6] == 'S') {
                return T_Q_FLAGS;
              }
            }
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int classify8(const char *s, LanguageFeatures features)
{
  if (s[0] == '_') {
    if (s[1] == '_') {
      if (s[2] == 'i') {
        if (s[3] == 'n') {
          if (s[4] == 'l') {
            if (s[5] == 'i') {
              if (s[6] == 'n') {
                if (s[7] == 'e') {
                  return T___INLINE;
                }
              }
            }
          }
        }
      }
      else if (s[2] == 't') {
        if (s[3] == 'y') {
          if (s[4] == 'p') {
            if (s[5] == 'e') {
              if (s[6] == 'o') {
                if (s[7] == 'f') {
                  return T___TYPEOF;
                }
              }
            }
          }
        }
        else if (s[3] == 'h') {
          if (s[4] == 'r') {
            if (s[5] == 'e') {
              if (s[6] == 'a') {
                if (s[7] == 'd') {
                  return T___THREAD;
                }
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'c') {
    if (s[1] == 'o') {
      if (s[2] == 'n') {
        if (s[3] == 't') {
          if (s[4] == 'i') {
            if (s[5] == 'n') {
              if (s[6] == 'u') {
                if (s[7] == 'e') {
                  return T_CONTINUE;
                }
              }
            }
          }
        }
      }
    } else if (features.cxx11Enabled && s[1] == 'h') {
        if (s[2] == 'a') {
            if (s[3] == 'r') {
                if (s[4] == '1') {
                    if (s[5] == '6') {
                        if (s[6] == '_') {
                            if (s[7] == 't') {
                                return T_CHAR16_T;
                            }
                        }
                    }
                } else if (s[4] == '3') {
                    if (s[5] == '2') {
                        if (s[6] == '_') {
                            if (s[7] == 't') {
                                return T_CHAR32_T;
                            }
                        }
                    }
                }
            }
        }
    }
  }
  else if (features.cxx11Enabled && s[0] == 'd') {
    if (s[1] == 'e') {
      if (s[2] == 'c') {
        if (s[3] == 'l') {
          if (s[4] == 't') {
            if (s[5] == 'y') {
              if (s[6] == 'p') {
                if (s[7] == 'e') {
                  return T_DECLTYPE;
                }
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'e') {
    if (s[1] == 'x') {
      if (s[2] == 'p') {
        if (s[3] == 'l') {
          if (s[4] == 'i') {
            if (s[5] == 'c') {
              if (s[6] == 'i') {
                if (s[7] == 't') {
                  return T_EXPLICIT;
                }
              }
            }
          }
        }
      }
    }
  }
  else if (features.cxx11Enabled && s[0] == 'n') {
    if (s[1] == 'o') {
      if (s[2] == 'e') {
        if (s[3] == 'x') {
          if (s[4] == 'c') {
            if (s[5] == 'e') {
              if (s[6] == 'p') {
                if (s[7] == 't') {
                  return T_NOEXCEPT;
                }
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'o') {
    if (s[1] == 'p') {
      if (s[2] == 'e') {
        if (s[3] == 'r') {
          if (s[4] == 'a') {
            if (s[5] == 't') {
              if (s[6] == 'o') {
                if (s[7] == 'r') {
                  return T_OPERATOR;
                }
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'r') {
    if (s[1] == 'e') {
      if (s[2] == 'g') {
        if (s[3] == 'i') {
          if (s[4] == 's') {
            if (s[5] == 't') {
              if (s[6] == 'e') {
                if (s[7] == 'r') {
                  return T_REGISTER;
                }
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 't') {
    if (s[1] == 'e') {
      if (s[2] == 'm') {
        if (s[3] == 'p') {
          if (s[4] == 'l') {
            if (s[5] == 'a') {
              if (s[6] == 't') {
                if (s[7] == 'e') {
                  return T_TEMPLATE;
                }
              }
            }
          }
        }
      }
    }
    else if (s[1] == 'y') {
      if (s[2] == 'p') {
        if (s[3] == 'e') {
          if (s[4] == 'n') {
            if (s[5] == 'a') {
              if (s[6] == 'm') {
                if (s[7] == 'e') {
                  return T_TYPENAME;
                }
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'u') {
    if (s[1] == 'n') {
      if (s[2] == 's') {
        if (s[3] == 'i') {
          if (s[4] == 'g') {
            if (s[5] == 'n') {
              if (s[6] == 'e') {
                if (s[7] == 'd') {
                  return T_UNSIGNED;
                }
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'v') {
    if (s[1] == 'o') {
      if (s[2] == 'l') {
        if (s[3] == 'a') {
          if (s[4] == 't') {
            if (s[5] == 'i') {
              if (s[6] == 'l') {
                if (s[7] == 'e') {
                  return T_VOLATILE;
                }
              }
            }
          }
        }
      }
    }
  }
  else if (features.qtEnabled && s[0] == 'Q') {
    if (s[1] == '_') {
      if (s[2] == 'G') {
        if (s[3] == 'A') {
          if (s[4] == 'D') {
            if (s[5] == 'G') {
              if (s[6] == 'E') {
                if (s[7] == 'T') {
                  return T_Q_GADGET;
                }
              }
            }
          }
        }
      }
      else if (s[2] == 'O') {
        if (s[3] == 'B') {
          if (s[4] == 'J') {
            if (s[5] == 'E') {
              if (s[6] == 'C') {
                if (s[7] == 'T') {
                  return T_Q_OBJECT;
                }
              }
            }
          }
        }
      }
      else if (s[2] == 'S') {
        if (s[3] == 'I') {
          if (s[4] == 'G') {
            if (s[5] == 'N') {
              if (s[6] == 'A') {
                if (s[7] == 'L') {
                  return T_Q_SIGNAL;
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

static inline int classify9(const char *s, LanguageFeatures features)
{
  if (s[0] == '_') {
    if (s[1] == '_') {
      if (s[2] == 'c') {
        if (s[3] == 'o') {
          if (s[4] == 'n') {
            if (s[5] == 's') {
              if (s[6] == 't') {
                if (s[7] == '_') {
                  if (s[8] == '_') {
                    return T___CONST__;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  else if (features.cxx11Enabled && s[0] == 'c') {
    if (s[1] == 'o') {
      if (s[2] == 'n') {
        if (s[3] == 's') {
          if (s[4] == 't') {
            if (s[5] == 'e') {
              if (s[6] == 'x') {
                if (s[7] == 'p') {
                  if (s[8] == 'r') {
                    return T_CONSTEXPR;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'n') {
    if (s[1] == 'a') {
      if (s[2] == 'm') {
        if (s[3] == 'e') {
          if (s[4] == 's') {
            if (s[5] == 'p') {
              if (s[6] == 'a') {
                if (s[7] == 'c') {
                  if (s[8] == 'e') {
                    return T_NAMESPACE;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'p') {
    if (s[1] == 'r') {
      if (s[2] == 'o') {
        if (s[3] == 't') {
          if (s[4] == 'e') {
            if (s[5] == 'c') {
              if (s[6] == 't') {
                if (s[7] == 'e') {
                  if (s[8] == 'd') {
                    return T_PROTECTED;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  else if (features.qtEnabled && s[0] == 'Q') {
    if (s[1] == '_') {
      if (s[2] == 'S') {
        if (s[3] == 'I') {
          if (s[4] == 'G') {
            if (s[5] == 'N') {
              if (s[6] == 'A') {
                if (s[7] == 'L') {
                  if (s[8] == 'S') {
                    return T_Q_SIGNALS;
                  }
                }
              }
            }
          }
        }
      } else if (s[2] == 'F') {
        if (s[3] == 'O') {
          if (s[4] == 'R') {
            if (s[5] == 'E') {
              if (s[6] == 'A') {
                if (s[7] == 'C') {
                  if (s[8] == 'H') {
                    return T_Q_FOREACH;
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

static inline int classify10(const char *s, LanguageFeatures features)
{
  if (s[0] == '_') {
    if (s[1] == '_') {
      if (s[2] == 'i') {
        if (s[3] == 'n') {
          if (s[4] == 'l') {
            if (s[5] == 'i') {
              if (s[6] == 'n') {
                if (s[7] == 'e') {
                  if (s[8] == '_') {
                    if (s[9] == '_') {
                      return T___INLINE__;
                    }
                  }
                }
              }
            }
          }
        }
      }
      else if (s[2] == 'd') {
        if (s[3] == 'e') {
          if (s[4] == 'c') {
            if (s[5] == 'l') {
              if (s[6] == 't') {
                if (s[7] == 'y') {
                  if (s[8] == 'p') {
                    if (s[9] == 'e') {
                      return T___DECLTYPE;
                    }
                  }
                }
              }
            }
          }
        }
      }
      else if (s[2] == 't') {
        if (s[3] == 'y') {
          if (s[4] == 'p') {
            if (s[5] == 'e') {
              if (s[6] == 'o') {
                if (s[7] == 'f') {
                  if (s[8] == '_') {
                    if (s[9] == '_') {
                      return T___TYPEOF__;
                    }
                  }
                }
              }
            }
          }
        }
      }
      else if (s[2] == 'v') {
        if (s[3] == 'o') {
          if (s[4] == 'l') {
            if (s[5] == 'a') {
              if (s[6] == 't') {
                if (s[7] == 'i') {
                  if (s[8] == 'l') {
                    if (s[9] == 'e') {
                      return T___VOLATILE;
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
  else if (s[0] == 'c') {
    if (s[1] == 'o') {
      if (s[2] == 'n') {
        if (s[3] == 's') {
          if (s[4] == 't') {
            if (s[5] == '_') {
              if (s[6] == 'c') {
                if (s[7] == 'a') {
                  if (s[8] == 's') {
                    if (s[9] == 't') {
                      return T_CONST_CAST;
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
  else if (features.qtEnabled && s[0] == 'Q') {
    if (s[1] == '_') {
      if (s[2] == 'O') {
        if (s[3] == 'V') {
          if (s[4] == 'E') {
            if (s[5] == 'R') {
              if (s[6] == 'R') {
                if (s[7] == 'I') {
                  if (s[8] == 'D') {
                    if (s[9] == 'E') {
                      return T_Q_PROPERTY; // Q_OVERRIDE is just an alias for Q_PROPERTY
                    }
                  }
                }
              }
            }
          }
        }
      }
      else if (s[2] == 'P') {
        if (s[3] == 'R') {
          if (s[4] == 'O') {
            if (s[5] == 'P') {
              if (s[6] == 'E') {
                if (s[7] == 'R') {
                  if (s[8] == 'T') {
                    if (s[9] == 'Y') {
                      return T_Q_PROPERTY;
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

static inline int classify11(const char *s, LanguageFeatures features)
{
  if (s[0] == '_') {
    if (s[1] == '_') {
      if (s[2] == 'a') {
        if (s[3] == 't') {
          if (s[4] == 't') {
            if (s[5] == 'r') {
              if (s[6] == 'i') {
                if (s[7] == 'b') {
                  if (s[8] == 'u') {
                    if (s[9] == 't') {
                      if (s[10] == 'e') {
                        return T___ATTRIBUTE;
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
  else if (s[0] == 's') {
    if (s[1] == 't') {
      if (s[2] == 'a') {
        if (s[3] == 't') {
          if (s[4] == 'i') {
            if (s[5] == 'c') {
              if (s[6] == '_') {
                if (s[7] == 'c') {
                  if (s[8] == 'a') {
                    if (s[9] == 's') {
                      if (s[10] == 't') {
                        return T_STATIC_CAST;
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
  else if (features.qtEnabled && s[0] == 'Q') {
    if (s[1] == '_') {
      if (s[2] == 'I') {
        if (s[3] == 'N') {
          if (s[4] == 'V') {
            if (s[5] == 'O') {
              if (s[6] == 'K') {
                if (s[7] == 'A') {
                  if (s[8] == 'B') {
                    if (s[9] == 'L') {
                      if (s[10] == 'E') {
                        return T_Q_INVOKABLE;
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

static inline int classify12(const char *s, LanguageFeatures features)
{
  if (s[0] == '_') {
    if (s[1] == '_') {
      if (s[2] == 'v') {
        if (s[3] == 'o') {
          if (s[4] == 'l') {
            if (s[5] == 'a') {
              if (s[6] == 't') {
                if (s[7] == 'i') {
                  if (s[8] == 'l') {
                    if (s[9] == 'e') {
                      if (s[10] == '_') {
                        if (s[11] == '_') {
                          return T___VOLATILE__;
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
  }
  else if (features.qtEnabled && s[0] == 'Q') {
    if (s[1] == '_') {
      if (s[2] == 'I') {
        if (s[3] == 'N') {
          if (s[4] == 'T') {
            if (s[5] == 'E') {
              if (s[6] == 'R') {
                if (s[7] == 'F') {
                  if (s[8] == 'A') {
                    if (s[9] == 'C') {
                      if (s[10] == 'E') {
                        if (s[11] == 'S') {
                          return T_Q_INTERFACES;
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
  }
  else if (s[0] == 'd') {
    if (s[1] == 'y') {
      if (s[2] == 'n') {
        if (s[3] == 'a') {
          if (s[4] == 'm') {
            if (s[5] == 'i') {
              if (s[6] == 'c') {
                if (s[7] == '_') {
                  if (s[8] == 'c') {
                    if (s[9] == 'a') {
                      if (s[10] == 's') {
                        if (s[11] == 't') {
                          return T_DYNAMIC_CAST;
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
  }
  else if (features.cxx11Enabled && s[0] == 't') {
    if (s[1] == 'h') {
      if (s[2] == 'r') {
        if (s[3] == 'e') {
          if (s[4] == 'a') {
            if (s[5] == 'd') {
              if (s[6] == '_') {
                if (s[7] == 'l') {
                  if (s[8] == 'o') {
                    if (s[9] == 'c') {
                      if (s[10] == 'a') {
                        if (s[11] == 'l') {
                          return T_THREAD_LOCAL;
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
  }
  return T_IDENTIFIER;
}

static inline int classify13(const char *s, LanguageFeatures features)
{
  if (s[0] == '_') {
    if (s[1] == '_') {
      if (s[2] == 'a') {
        if (s[3] == 't') {
          if (s[4] == 't') {
            if (s[5] == 'r') {
              if (s[6] == 'i') {
                if (s[7] == 'b') {
                  if (s[8] == 'u') {
                    if (s[9] == 't') {
                      if (s[10] == 'e') {
                        if (s[11] == '_') {
                          if (s[12] == '_') {
                            return T___ATTRIBUTE__;
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
    }
  } else if (features.cxx11Enabled && s[0] == 's') {
    if (s[1] == 't') {
      if (s[2] == 'a') {
        if (s[3] == 't') {
          if (s[4] == 'i') {
            if (s[5] == 'c') {
              if (s[6] == '_') {
                if (s[7] == 'a') {
                  if (s[8] == 's') {
                    if (s[9] == 's') {
                      if (s[10] == 'e') {
                        if (s[11] == 'r') {
                          if (s[12] == 't') {
                            return T_STATIC_ASSERT;
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
    }
  }
  return T_IDENTIFIER;
}

static inline int classify16(const char *s, LanguageFeatures)
{
  if (s[0] == 'r') {
    if (s[1] == 'e') {
      if (s[2] == 'i') {
        if (s[3] == 'n') {
          if (s[4] == 't') {
            if (s[5] == 'e') {
              if (s[6] == 'r') {
                if (s[7] == 'p') {
                  if (s[8] == 'r') {
                    if (s[9] == 'e') {
                      if (s[10] == 't') {
                        if (s[11] == '_') {
                          if (s[12] == 'c') {
                            if (s[13] == 'a') {
                              if (s[14] == 's') {
                                if (s[15] == 't') {
                                  return T_REINTERPRET_CAST;
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
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int classify14(const char *s, LanguageFeatures features)
{
  if (features.qtEnabled && s[0] == 'Q') {
    if (s[1] == '_') {
      if (s[2] == 'P') {
        if (s[3] == 'R') {
          if (s[4] == 'I') {
            if (s[5] == 'V') {
              if (s[6] == 'A') {
                if (s[7] == 'T') {
                  if (s[8] == 'E') {
                    if (s[9] == '_') {
                      if (s[10] == 'S') {
                        if (s[11] == 'L') {
                          if (s[12] == 'O') {
                            if (s[13] == 'T') {
                              return T_Q_PRIVATE_SLOT;
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
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int classify18(const char *s, LanguageFeatures features)
{
  if (features.qtEnabled && s[0] == 'Q') {
    if (s[1] == '_') {
      if (s[2] == 'P') {
        if (s[3] == 'R') {
          if (s[4] == 'I') {
            if (s[5] == 'V') {
              if (s[6] == 'A') {
                if (s[7] == 'T') {
                  if (s[8] == 'E') {
                    if (s[9] == '_') {
                      if (s[10] == 'P') {
                        if (s[11] == 'R') {
                          if (s[12] == 'O') {
                            if (s[13] == 'P') {
                              if (s[14] == 'E') {
                                if (s[15] == 'R') {
                                  if (s[16] == 'T') {
                                    if (s[17] == 'Y') {
                                      return T_Q_PRIVATE_PROPERTY;
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
              }
            }
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int classify19(const char *s, LanguageFeatures features)
{
  if (features.qtEnabled && s[0] == 'Q') {
    if (s[1] == '_') {
      if (s[2] == 'D') {
        if (s[3] == 'E') {
          if (s[4] == 'C') {
            if (s[5] == 'L') {
              if (s[6] == 'A') {
                if (s[7] == 'R') {
                  if (s[8] == 'E') {
                    if (s[9] == '_') {
                      if (s[10] == 'I') {
                        if (s[11] == 'N') {
                          if (s[12] == 'T') {
                            if (s[13] == 'E') {
                              if (s[14] == 'R') {
                                if (s[15] == 'F') {
                                  if (s[16] == 'A') {
                                    if (s[17] == 'C') {
                                      if (s[18] == 'E') {
                                        return T_Q_DECLARE_INTERFACE;
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


int Lexer::classify(const char *s, int n, LanguageFeatures features) {
  switch (n) {
    case 2: return classify2(s, features);
    case 3: return classify3(s, features);
    case 4: return classify4(s, features);
    case 5: return classify5(s, features);
    case 6: return classify6(s, features);
    case 7: return classify7(s, features);
    case 8: return classify8(s, features);
    case 9: return classify9(s, features);
    case 10: return classify10(s, features);
    case 11: return classify11(s, features);
    case 12: return classify12(s, features);
    case 13: return classify13(s, features);
    case 14: return classify14(s, features);
    case 16: return classify16(s, features);
    case 18: return classify18(s, features);
    case 19: return classify19(s, features);
    default: return T_IDENTIFIER;
  } // switch
}

static inline int classifyOperator2(const char *s) {
  if (s[0] == 'o') {
    if (s[1] == 'r') {
      return T_OR;
    }
  }
  return T_IDENTIFIER;
}

static inline int classifyOperator3(const char *s) {
  if (s[0] == 'a') {
    if (s[1] == 'n') {
      if (s[2] == 'd') {
        return T_AND;
      }
    }
  }
  else if (s[0] == 'n') {
    if (s[1] == 'o') {
      if (s[2] == 't') {
        return T_NOT;
      }
    }
  }
  else if (s[0] == 'x') {
    if (s[1] == 'o') {
      if (s[2] == 'r') {
        return T_XOR;
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int classifyOperator5(const char *s) {
  if (s[0] == 'b') {
    if (s[1] == 'i') {
      if (s[2] == 't') {
        if (s[3] == 'o') {
          if (s[4] == 'r') {
            return T_BITOR;
          }
        }
      }
    }
  }
  else if (s[0] == 'c') {
    if (s[1] == 'o') {
      if (s[2] == 'm') {
        if (s[3] == 'p') {
          if (s[4] == 'l') {
            return T_COMPL;
          }
        }
      }
    }
  }
  else if (s[0] == 'o') {
    if (s[1] == 'r') {
      if (s[2] == '_') {
        if (s[3] == 'e') {
          if (s[4] == 'q') {
            return T_OR_EQ;
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

static inline int classifyOperator6(const char *s) {
  if (s[0] == 'a') {
    if (s[1] == 'n') {
      if (s[2] == 'd') {
        if (s[3] == '_') {
          if (s[4] == 'e') {
            if (s[5] == 'q') {
              return T_AND_EQ;
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'b') {
    if (s[1] == 'i') {
      if (s[2] == 't') {
        if (s[3] == 'a') {
          if (s[4] == 'n') {
            if (s[5] == 'd') {
              return T_BITAND;
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'n') {
    if (s[1] == 'o') {
      if (s[2] == 't') {
        if (s[3] == '_') {
          if (s[4] == 'e') {
            if (s[5] == 'q') {
              return T_NOT_EQ;
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'x') {
    if (s[1] == 'o') {
      if (s[2] == 'r') {
        if (s[3] == '_') {
          if (s[4] == 'e') {
            if (s[5] == 'q') {
              return T_XOR_EQ;
            }
          }
        }
      }
    }
  }
  return T_IDENTIFIER;
}

int Lexer::classifyOperator(const char *s, int n) {
  switch (n) {
    case 2: return classifyOperator2(s);
    case 3: return classifyOperator3(s);
    case 5: return classifyOperator5(s);
    case 6: return classifyOperator6(s);
    default: return T_IDENTIFIER;
  } // switch
}


} // namespace CPlusPlus
