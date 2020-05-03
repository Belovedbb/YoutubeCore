#include "parse_url.h"

namespace url_space {

    

    string& replace_all(string& str, char replacee, char replacer) {
        size_t pos = 0;

        for (;; ) {
            pos = str.find(replacee, pos);
            if (pos == string::npos )
                break;
            str.replace(pos,  1, 1, replacer);
        }
        return str;
    }

    bool ParseUrl::is_literal() {
        return (*internal_iter).get_code_rep() == URLCODE::LETTER ||
               (*internal_iter).get_code_rep() == URLCODE::DIGIT ||
               (*internal_iter).get_code_rep() == URLCODE::PERIOD || (*internal_iter).get_code_rep() == URLCODE::PLUS ||
               (*internal_iter).get_code_rep() == URLCODE::HYPHEN;
    }

    bool ParseUrl::is_end()
    {
        return url_tokens.end() == internal_iter;
    }

    bool is_literal_iter(list<Token>::iterator internal_iter) {
        return (*internal_iter).get_code_rep() == URLCODE::LETTER ||
               (*internal_iter).get_code_rep() == URLCODE::DIGIT ||
               (*internal_iter).get_code_rep() == URLCODE::PERIOD || (*internal_iter).get_code_rep() == URLCODE::PLUS ||
               (*internal_iter).get_code_rep() == URLCODE::HYPHEN;
    }

    bool ParseUrl::match(URLCODE code)
    {
        return (*internal_iter).get_code_rep() == code;
    }

    ParseUrl::ParseUrl(const list<Token>& tokenized_url) : url_tokens(tokenized_url){}

    tuple<string, string, string, string, string> ParseUrl::parse_url()
    {
        scheme = authority = path = query = fragment = "";

        for (internal_iter = url_tokens.begin(); internal_iter != url_tokens.end();) {
            parse();
        }
        return make_tuple(scheme, authority, path, query, fragment);
    }

    void ParseUrl::parse()
    {
        auto local_iter = internal_iter;

        while (!is_end() && is_literal()) {
            advance(internal_iter, 1);
        }
        //scheme -> EMPTY | literal ":"; || begin 'literal' (scheme) <-> terminate ':'(scheme_end)
        if (!is_end() &&  ((*internal_iter).get_code_rep() == URLCODE::COLON && peek_next(1).get_code_rep() != URLCODE::LETTER) ) {
            list<Token> temp{};
            copy(local_iter, internal_iter, std::back_inserter(temp));
            for (Token token: temp) {
                scheme.append( token.get_literal());
            }
        }
        //authority "//" literal  (":"literal)? "@" "[" literal"]" (":"literal)? ; || begin '//'(host) <-> terminate '/'(path)
        if (!is_end() && (*++internal_iter).get_code_rep() == URLCODE::SLASH_SLASH) {
            advance(internal_iter, 1);
            local_iter = internal_iter;
            while (!is_end() && (*internal_iter).get_code_rep() != URLCODE::SLASH ) {
                advance(internal_iter, 1);
            }
            list<Token> temp{};
            copy(local_iter, internal_iter, std::back_inserter(temp));
            for (Token token : temp) {
                authority.append(token.get_literal());
            }
        }
        //path -> EMPTY | ("/" (EMPTY|literal)?)* ; || begin '/'(path) <-> terminate 'EOF | # | ?'(optionals)
        if (!is_end() && (*internal_iter).get_code_rep() == URLCODE::SLASH ) {
            local_iter = internal_iter;
            if(!is_end())
                advance(internal_iter, 1);
            auto begin_iter = internal_iter;
            while (!is_end()) {
                if ((*internal_iter).get_code_rep() == URLCODE::Q_MARK)
                    break;
                if ((*internal_iter).get_code_rep() == URLCODE::HASH)
                    break;

                advance(internal_iter, 1);
            }
            list<Token> temp{};
            copy(local_iter, internal_iter, std::back_inserter(temp));
            for (Token token : temp) {
                path.append(token.get_literal());
            }
        }
        
        // OPTIONALS: query -> "?" (LETTER | OTHER |DIGIT)*; || begin '?'(query) <-> terminate 'EOF | # '(optionals)
        if (!is_end() && (*internal_iter).get_code_rep() == URLCODE::Q_MARK) {
            local_iter = internal_iter;
            if (!is_end())
                advance(internal_iter, 1);
            auto begin_iter = internal_iter;
            while (!is_end()) {
                if ((*internal_iter).get_code_rep() == URLCODE::HASH)
                    break;

                advance(internal_iter, 1);
            }
            list<Token> temp{};
            copy(local_iter, internal_iter, std::back_inserter(temp));
            for (Token token : temp) {
                query.append(token.get_literal());
            }
        }

        //OPTIONALS: fragment -> "#" (LETTER | OTHER | DIGIT)*; || begin '#'(frag) <-> terminate 'EOF'(optionals)
        if (!is_end() && (*internal_iter).get_code_rep() == URLCODE::HASH) {
            local_iter = internal_iter;
            if (!is_end())
                advance(internal_iter, 1);
            auto begin_iter = internal_iter;
            while (!is_end()) {
                advance(internal_iter, 1);
            }
            list<Token> temp{};
            copy(local_iter, internal_iter, std::back_inserter(temp));
            for (Token token : temp) {
                fragment.append(token.get_literal());
            }
        }

    }

    Token ParseUrl::peek_next(int offset){
        auto local = internal_iter;
        advance(local, offset);
        return *local;
    }

    Token ParseUrl::peek_prev(int offset)
    {
        auto local = internal_iter;
        advance(local, offset * -1);
        return *local;
    }

    /* Converts a hex character to its integer value */
    char from_hex(char ch) {
        return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
    }
    
    string ParseUrl::decode_url(const string& str) {
        string decoded_str;
        for (string::const_iterator ch_p = str.begin(); ch_p != str.end(); ch_p++) {
            if (*ch_p == '%') {
                advance(ch_p, 1);
                char a = *ch_p;
                advance(ch_p, 1);
                char b = *ch_p;
                if (a && b) {
                    decoded_str.push_back(from_hex(a) << 4 | from_hex(b));
                }
            }
            else if (*ch_p == '+') {
                decoded_str.push_back(' ');
            }
            else {
                decoded_str.push_back(*ch_p);
            }
        }
        return decoded_str;
    }

    map<string, string> ParseUrl::parse_query(string &query_part) {
        map<string, string> parsed_query = {};
        if (query_part.at(0) != '?')
            return parsed_query;
        else {
            query_part = query_part.substr(1, query_part.size());
            for (string part : split(query_part, '&')) {
                replace_all(part, '+', ' ');
                int eq = part.find('=');
                string key = eq > -1 ? part.substr(0, eq) : part;
                string val = eq > -1 ? this->decode_url(part.substr(eq + 1)) : "";
                int from = key.find('[');
                if (from == -1) {
                    parsed_query[this->decode_url(key)] = val;
                }
                else {
                    int to = key.find(']', from);
                    string index = this->decode_url(key.substr(from + 1, to));
                    key = this->decode_url(key.substr(0, from));
                    if (parsed_query.at(key) != "")
                        parsed_query.at(key) = "";
                    if (index != "")
                        parsed_query.at(key) = val;
                    //else result[key][index] = val;
                }
            }
            return parsed_query;
        }
    }

};


namespace url_construct{
    pair<tuple<string, string, string, string, string>, map<string, string>> parse_url(const std::string &url, bool parse_query){
        map<string, string> parsed_query_;
        TokenizeUrl scanned_url = TokenizeUrl(url);
        auto list_ = scanned_url.scan_url();
        ParseUrl parse_url = ParseUrl(list_);
        auto parsed_url_tuple =  parse_url.parse_url();
        if(parse_query)
            parsed_query_ = map<string,string>(parse_url.parse_query(get<3>(parsed_url_tuple)));
        return make_pair(parsed_url_tuple, parsed_query_);
    }
};