   #include <Arduino.h>
   #include <SymbolMap.h>
   #include <Symbols.h>

   SymbolMap::SymbolMap() {
    addSymbol({'A', _A});
    addSymbol({'B', _B});
    addSymbol({'C', _C});
    addSymbol({'D', _D});
    addSymbol({'E', _E});
    addSymbol({'F', _F});
    addSymbol({'G', _G});
    addSymbol({'H', _H});
    addSymbol({'I', _I});
    addSymbol({'J', _J});
    addSymbol({'K', _K});
    addSymbol({'L', _L});
    addSymbol({'M', _M});
    addSymbol({'N', _N});
    addSymbol({'O', _O});
    addSymbol({'P', _P});
    addSymbol({'Q', _Q});
    addSymbol({'R', _R});
    addSymbol({'S', _S});
    addSymbol({'T', _T});
    addSymbol({'U', _U});
    addSymbol({'V', _V});
    addSymbol({'X', _X});
   }
   
   bool SymbolMap::addSymbol(const struct symbol newSymbol) {
      if (this->symbolsAdded <= MAX_SYMBOLS) {
        symbols[this->symbolsAdded++] = newSymbol;
        return true;
      }
      return false;
    }

    char *SymbolMap::getSymbol(char key) {
      for (uint8_t i = 0; i < this->symbolsAdded; i++) {
        if (this->symbols[i].key == key) {
          return this->symbols[i].arr;
        }
      }
      return NULL;
    }