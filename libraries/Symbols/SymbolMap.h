#define MAX_SYMBOLS    28

class SymbolMap {
    struct symbol {
      char key;
      char *arr;
    } symbols[MAX_SYMBOLS];

    uint8_t symbolsAdded = 0;

  public:
    bool addSymbol(const struct symbol newSymbol);
    char *getSymbol(char key);
    SymbolMap();
};
