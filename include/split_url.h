#ifndef SPLIT_URL_H
#define SPLIT_URL_H

#include <string>
#include <list>
#include <cctype>

namespace url_space {

    using std::string;
    using std::list;

    enum class URLCODE{
        EMPTY,LETTER,COLON,
        DIGIT,PLUS,PERIOD,
        HYPHEN, SLASH_SLASH,
        AT, SLASH,
        HASH, Q_MARK,
        S_BRACKET_O, S_BRACKET_C,
        OTHER
    };

    class Token {
    private:
        string literal;
        URLCODE url_code_rep;
    public:
        Token(const string& literal, URLCODE url_code_rep);
        string get_literal();
        URLCODE get_code_rep();

    };

    class TokenizeUrl {
    private:
        string url;
        Token scanner(const char& current_char) ;
        int index = 0;
    public:
        TokenizeUrl(const string& url): url(url){}
        list<Token> scan_url();

    };
}
#endif