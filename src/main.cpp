
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>


template <typename T>
using Vector = std::vector<T>;
using String = std::string;
using Token = String;
using SToken = const char*;

// Tokens clasifier automat
class TCA {
public:
    enum States {
        initial, integer, ID, symbol, end
    };

    enum Alphabet {
        letters, numbers, symbols, unknown
    };

    States apply(char c) {
        if (c == ' ' || c == '\t' || c == '\n') {
            prevState = state;
            state = end;
            return state;
        }
        // obtiene a que alfabeto pertence el caracter c
        Alphabet alp = getAlphabet(c);
        assert(alp != unknown);
        // cambia el estado
        prevState = state;
        assert(state != end);
        state = transition[state][alp];
        return state;
    }

    void reset() {
        state = initial;
    }

    States getState() {
        return state;
    }

    States getPrevState() {
        return prevState;
    }

private:
    static constexpr States transition[4][3]{
        //  letter  numbers   symbols
            {ID,    integer,  symbol},    // initial
            {end,   integer,  end},       // integer
            {ID,    ID,       end},       // ID
            {end,   end,      symbol}     // symbol
    };

    States state = initial;
    States prevState;

private:

    Alphabet getAlphabet(char c) {
        if (isLetter(c)) return letters;
        if (isNumber(c)) return numbers;
        if (isSymbol(c)) return symbols;
        else return unknown;
    }

    bool isLetter(char c) {
        return ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == '_');
    }

    bool isNumber(char c) {
        return ('0' <= c && c <= '9');
    }

    bool isSymbol(char c) {
        std::string_view symbols = "<>()[]{},;=!+-*/|&";
        return std::find(symbols.begin(), symbols.end(), c) != symbols.end();
    }
};


class IdentifierTokens {
public:
    Vector<Token> identifyTokens(String cCode) {
        Vector<Token> identifiedTokens{};
        String word{};
        TCA tokensClasifier;

        for (int i = 0; i < cCode.length();) {
            char letter = cCode[i];
            auto state = tokensClasifier.apply(letter);

            if (state == TCA::States::end) {
                auto prevState = tokensClasifier.getPrevState();

                if (prevState == TCA::States::ID) {
                    int rwi = getReservedWordIndex(word);
                    if (rwi != -1) {
                        identifiedTokens.push_back(reservedWords[rwi]);
                    }
                }

                word.clear();
                tokensClasifier.reset();
            }
            else {
                word += letter;
            }

            if (letter != ' ' || letter != '\t' || letter != '\n') {
                i++;
            }

            
        }

        return identifiedTokens;
    }

private:
    static constexpr SToken reservedWords[11]{
        "int",      "main",     "void",
        "break",    "do",       "else",
        "if",       "while",    "return",
        "scanf",    "printf"
    };

    static constexpr SToken specialSymbols[26]{
        "{", "}", "[", "]", "(", ")",
        ";", "+", "-", "*", "/", "&",
        "|", "!", "=", "<", ">", "<<",
        ">>", "==", "!=", "<=", ">=",
        "&&", "||", ","
    };

    int getReservedWordIndex(String word) {
        for (int i = 0; i < 11; ++i) {
            if (word == reservedWords[i]) {
                return i;
            }
        }
        return -1;
    }
};





int main() {
    IdentifierTokens idt;

    auto tokens = idt.identifyTokens("int main() { int a; int b; a = b + 1; return 0; }");

    for (auto token: tokens) {
        std::cout << token << '\n';
    }


    return 0;
}
