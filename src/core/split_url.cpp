#include "split_url.h"

namespace url_space {

    Token::Token(const string& literal, URLCODE url_code_rep) {
        this->literal = literal;
        this->url_code_rep = url_code_rep;
    }

    string Token::get_literal() {
        return this->literal;
    }

    URLCODE Token::get_code_rep() {
        return this->url_code_rep;
    }


    Token TokenizeUrl::scanner(const char& current_char)
    {
        Token token = Token("", URLCODE::EMPTY);
        switch (current_char) {
            case ':':
                token =  Token(":", URLCODE::COLON);
                break;
            case '+':
                token = Token("+", URLCODE::PLUS);
                break;
            case '.':
                token = Token(".", URLCODE::PERIOD);
                break;
            case '-':
                token = Token("-", URLCODE::HYPHEN);
                break;
            case '/': {
                if (index + 1 < url.size() && url.at(index) == url.at(index + 1)) {
                    token = Token("//", URLCODE::SLASH_SLASH);
                    index++;
                }
                else
                    token = Token("/", URLCODE::SLASH);
            }
                break;
            case '@':
                token = Token("@", URLCODE::AT);
                break;
            case '?':
                token = Token("?", URLCODE::Q_MARK);
                break;
            case '#':
                token = Token("#", URLCODE::HASH);
                break;
            case '[':
                token = Token("[", URLCODE::S_BRACKET_O);
                break;
            case ']':
                token = Token("]", URLCODE::S_BRACKET_C);
                break;


            default:
            {
                if (isalpha(current_char)) {
                    string str = string(1,current_char);
                    token = Token(str, URLCODE::LETTER);
                }
                else if (isdigit(current_char)) {
                    string str = string(1, current_char);
                    token = Token(str, URLCODE::DIGIT);
                }
                else {
                    string str = string(1, current_char);
                    token = Token(str, URLCODE::OTHER);
                }
                break;
            }

        }
        return token;
    }

    list<Token> TokenizeUrl::scan_url()
    {
        char current_rep = '\0';
        list<Token> url_token_container{};

        for (index; index < url.size(); index++) {
            current_rep = url.at(index);
            url_token_container.push_back(scanner(current_rep));
        }
        return url_token_container;
    }

    URLCODE get_code_rep()
    {
        return URLCODE();
    }

};