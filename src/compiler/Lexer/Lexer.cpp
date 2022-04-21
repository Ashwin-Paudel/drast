//
// Created by Ashwin Paudel on 2022-03-20.
//

#include "Lexer.h"

#define ALPHA                                                                  \
    case 'a':                                                                  \
    case 'b':                                                                  \
    case 'c':                                                                  \
    case 'd':                                                                  \
    case 'e':                                                                  \
    case 'f':                                                                  \
    case 'g':                                                                  \
    case 'h':                                                                  \
    case 'i':                                                                  \
    case 'j':                                                                  \
    case 'k':                                                                  \
    case 'l':                                                                  \
    case 'm':                                                                  \
    case 'n':                                                                  \
    case 'o':                                                                  \
    case 'p':                                                                  \
    case 'q':                                                                  \
    case 'r':                                                                  \
    case 's':                                                                  \
    case 't':                                                                  \
    case 'u':                                                                  \
    case 'v':                                                                  \
    case 'w':                                                                  \
    case 'x':                                                                  \
    case 'y':                                                                  \
    case 'z':                                                                  \
    case 'A':                                                                  \
    case 'B':                                                                  \
    case 'C':                                                                  \
    case 'D':                                                                  \
    case 'E':                                                                  \
    case 'F':                                                                  \
    case 'G':                                                                  \
    case 'H':                                                                  \
    case 'I':                                                                  \
    case 'J':                                                                  \
    case 'K':                                                                  \
    case 'L':                                                                  \
    case 'M':                                                                  \
    case 'N':                                                                  \
    case 'O':                                                                  \
    case 'P':                                                                  \
    case 'Q':                                                                  \
    case 'R':                                                                  \
    case 'S':                                                                  \
    case 'T':                                                                  \
    case 'U':                                                                  \
    case 'V':                                                                  \
    case 'W':                                                                  \
    case 'X':                                                                  \
    case 'Y':                                                                  \
    case 'Z'
#define DIGIT                                                                  \
    case '0':                                                                  \
    case '1':                                                                  \
    case '2':                                                                  \
    case '3':                                                                  \
    case '4':                                                                  \
    case '5':                                                                  \
    case '6':                                                                  \
    case '7':                                                                  \
    case '8':                                                                  \
    case '9'

namespace drast::lexer {

Lexer::Lexer(std::string &file_name, Error &error)
    : error(error), location(1, 1) {

    utils::readFile(file_name, file_buffer);
    error.buffer = file_buffer;
}

void Lexer::lex() {
    for (auto token = getToken(); token.type != TokenType::T_EOF;
         token = getToken()) {
        tokens.push_back(token);
    }

    if (did_encounter_error) {
        error.displayMessages();
    }

    for (auto &token : tokens) {
        if (token.type != TokenType::NEW_LINE) {
            std::cout << token.toString(this->file_buffer) << '\n';
        }
    }
    this->tokens.push_back(returnToken(TokenType::T_EOF));
}

Token Lexer::getToken() {
    this->advanceWhitespace();
    this->start = this->buffer_index;

    switch (this->current()) {
    ALPHA:
        return this->identifier();
    DIGIT:
        return this->number();
    case '"':
        return this->string();
    case '\'':
        return this->character();
    case '\0':
        break;
    default:
        return lexOperator();
    }

    return returnToken(TokenType::T_EOF, true);
}

Token Lexer::lexOperator() {
    TokenType type;
    auto next = peek();

    switch (this->current()) {
    case '?': {
        type = TokenType::QUESTION;
        break;
    }
    case '<': {
        if (next == '<') {
            this->advance();
            type = equalAndAdvance() ? TokenType::BITWISE_SHIFT_LEFT_EQUAL
                                     : TokenType::BITWISE_SHIFT_LEFT;
        } else {
            type = equalAndAdvance() ? TokenType::LESS_THAN_EQUAL
                                     : TokenType::LESS_THAN;
        }
        break;
    }
    case '>': {
        if (next == '>') {
            this->advance();
            type = equalAndAdvance() ? TokenType::BITWISE_SHIFT_RIGHT_EQUAL
                                     : TokenType::BITWISE_SHIFT_RIGHT;
        } else {
            type = equalAndAdvance() ? TokenType::GREATER_THAN_EQUAL
                                     : TokenType::GREATER_THAN;
        }
        break;
    }
    case '=': {
        type = equalAndAdvance() ? TokenType::EQUAL_EQUAL : TokenType::EQUAL;
        break;
    }
    case '!': {
        type = equalAndAdvance() ? TokenType::NOT_EQUAL : TokenType::NOT;
        break;
    }
    case '+': {
        type = equalAndAdvance() ? TokenType::OPERATOR_ADD_EQUAL
                                 : TokenType::OPERATOR_ADD;
        break;
    }
    case '-': {
        type = equalAndAdvance() ? TokenType::OPERATOR_SUB_EQUAL
                                 : TokenType::OPERATOR_SUB;
        break;
    }
    case '*': {
        type = equalAndAdvance() ? TokenType::OPERATOR_MUL_EQUAL
                                 : TokenType::OPERATOR_MUL;
        break;
    }
    case '/': {
        switch (next) {
        case '/':
            this->advanceLineComment();
            return getToken();
        case '*':
            this->advanceMultilineComment();
            return getToken();
        default:
            type = equalAndAdvance() ? TokenType::OPERATOR_DIV_EQUAL
                                     : TokenType::OPERATOR_DIV;
        }
        break;
    }
    case '%': {
        type = equalAndAdvance() ? TokenType::OPERATOR_MOD_EQUAL
                                 : TokenType::OPERATOR_MOD;
        break;
    }

    case '&': {
        if (next == '&') {
            this->advance();
            type = equalAndAdvance() ? TokenType::BITWISE_AND_AND_EQUAL
                                     : TokenType::BITWISE_AND_AND;
        } else {
            type = equalAndAdvance() ? TokenType::BITWISE_AND_EQUAL
                                     : TokenType::BITWISE_AND;
        }
        break;
    }
    case '|': {
        if (next == '|') {
            this->advance();
            type = equalAndAdvance() ? TokenType::BITWISE_PIPE_PIPE_EQUAL
                                     : TokenType::BITWISE_PIPE_PIPE;
        } else {
            type = equalAndAdvance() ? TokenType::BITWISE_PIPE_EQUAL
                                     : TokenType::BITWISE_PIPE;
        }
        break;
    }
    case '^': {
        type = equalAndAdvance() ? TokenType::BITWISE_POWER_EQUAL
                                 : TokenType::BITWISE_POWER;
        break;
    }
    case '~': {
        type = TokenType::BITWISE_NOT;
        break;
    }
    case ':': {
        if (next == ':') {
            this->advance();
            type = TokenType::DOUBLE_COLON;
        } else {
            type = TokenType::COLON;
        }
        break;
    }
    case ';': {
        type = TokenType::SEMICOLON;
        break;
    }
    case '(': {
        type = TokenType::PARENS_OPEN;
        break;
    }
    case ')': {
        type = TokenType::PARENS_CLOSE;
        break;
    }
    case '[': {
        type = TokenType::SQUARE_OPEN;
        break;
    }
    case ']': {
        type = TokenType::SQUARE_CLOSE;
        break;
    }
    case '{': {
        type = TokenType::BRACE_OPEN;
        break;
    }
    case '}': {
        type = TokenType::BRACE_CLOSE;
        break;
    }
    case ',': {
        type = TokenType::COMMA;
        break;
    }
    case '.': {
        type = TokenType::PERIOD;
        break;
    }
    case '$': {
        type = TokenType::DOLLAR;
        break;
    }
    case '#': {
        type = TokenType::HASHTAG;
        break;
    }
    case '@': {
        type = TokenType::AT;
        break;
    }
    case '\\': {
        type = TokenType::BACKSLASH;
        break;
    }
    default:
        return this->throwError("Unexpected Character `" +
                                std::string({this->current()}) + "`");
    }

    return returnToken(type);
}

Token Lexer::identifier() {
    this->start = this->buffer_index;

    while (utils::isAlphaNumeric(this->current())) {
        this->advance();
    }

    uint32_t length = this->buffer_index - this->start;

    std::string_view identifier(this->file_buffer.data() + this->start, length);

    TokenType type1 = Token::isKeyword(identifier);
    return this->returnToken(type1, true);
}

Token Lexer::hexadecimal() {
    advance();

    return lexWhile(TokenType::V_HEX,
                    [this]() { return utils::isHexadecimal(this->current()); });
}

Token Lexer::octal() {
    advance();

    return lexWhile(TokenType::V_OCTAL,
                    [this]() { return utils::isOctal(this->current()); });
}

Token Lexer::binary() {
    advance();

    return lexWhile(TokenType::V_BINARY,
                    [this]() { return utils::isBinary(this->current()); });
}

Token Lexer::number() {
    this->start = this->buffer_index;

    // Check for other numbers
    if (this->current() == '0') {
        advance();
        switch (this->current()) {
        case 'x':
        case 'X':
            return hexadecimal();
        case 'b':
        case 'B':
            return binary();
        case 'o':
        case 'O':
            return octal();
        }
    }

    bool is_float = false;

loop:
    while (utils::isNumber(this->current())) {
        if (this->current() == '\n') {
            break;
        }
        this->advance();
    }

    if (this->current() == '.') {
        if (is_float) {
            return this->throwError("Invalid number");
        }
        is_float = true;

        this->advance();
        if (!utils::isNumber(this->current())) {
            return this->throwError("Invalid number");
        }

        goto loop;
    }

    return this->returnToken(is_float ? TokenType::V_FLOAT : TokenType::V_INT,
                             true);
}

Token Lexer::string() {
    this->advance();

    if (this->current() == '"' && this->peek() == '"') {
        return multilineString();
    }

    while (this->current() != '"') {
        switch (this->current()) {
        case '\n':
        case '\0':
            return this->throwError("Unterminated string literal");
        case '\\':
            this->advance();
            evaluateEscapeSequence();
            continue;
        default:
            break;
        }
        this->advance();
    }

end:
    return returnToken(TokenType::V_STRING);
}

Token Lexer::character() {
    this->advance();

    switch (this->current()) {
    case '\\':
        this->advance();
        evaluateEscapeSequence();
        break;
    case '\'':
        return this->throwError("Empty character");
    case '\0':
        return this->throwError("Unterminated character");
    }
    this->advance();

    return returnToken(TokenType::V_CHAR);
}

Token Lexer::multilineString() {
    advance(2);

loop:
    // Run a loop until we find three of the string character
    while (this->current() != '"') {
        switch (this->current()) {
        case '\0':
            return this->throwError("Unterminated string literal");
        case '\n':
            advanceLine();
            break;
        case '\\':
            advance();
            evaluateEscapeSequence();
        default:
            break;
        }
        this->advance();
    }

    if (this->peek() == '"' && this->peek(2) == '"') {
        advance(3);
        return returnToken(TokenType::V_MULTILINE_STRING, true);
    } else {
        advance();
        goto loop;
    }
}

Token Lexer::returnToken(TokenType type, bool without_advance) {
    auto return_token = Token(type, this->start,
                              this->buffer_index - this->start, this->location);
    if (!without_advance) {
        this->advance();
        return_token.location.column += 1;
        return_token.length += 1;
    }
    return return_token;
}

constexpr bool Lexer::equalAndAdvance() {
    if (this->peek() == '=') {
        advance();
        return true;
    }
    return false;
}

void Lexer::advanceWhitespace() {
    while (isspace(this->current())) {
        if (this->current() == '\n') {
            this->tokens.push_back(returnToken(TokenType::NEW_LINE, true));
            advanceLine();
        }
        if (this->current() == '\0') {
            break;
        }
        this->advance();
    }
}

void Lexer::advanceLineComment() {
    while (this->current() != '\n') {
        if (this->current() == '\0') {
            break;
        }
        this->advance();
    }
}

void Lexer::advanceMultilineComment() {
    advance(2);
    auto comment_start = this->location;
    while (this->current() != '*' || this->peek() != '/') {
        switch (this->current()) {
        case '\0':
            this->throwError("Unterminated block comment.", comment_start);
            return;
        case '/':
            if (peek() == '*')
                advanceMultilineComment();
        case '\n':
            advanceLine();
            break;
        default:
            break;
        }
        this->advance();
    }
    advance(2);
}

constexpr void Lexer::advanceLine() {
    this->location.line += 1;
    this->location.column = 0;
}

void Lexer::advance() {
    this->location.column += 1;
    this->buffer_index += 1;
    this->current() = this->file_buffer[this->buffer_index];
}

void Lexer::advance(size_t offset) {
    this->location.column += offset;
    this->buffer_index += offset;
    this->current() = this->file_buffer[this->buffer_index];
}

void Lexer::evaluateEscapeSequence() {
    switch (this->current()) {
    case '0':
    case 'a':
    case 'b':
    case 'f':
    case 'n':
    case 'r':
    case 't':
    case 'v':
    case 'x':
    case '\'':
        advance();
        return;
    default:
        this->throwError("Invalid escape sequence.");
        break;
    }
}

constexpr char Lexer::peek(size_t offset) {
    return this->file_buffer[this->buffer_index + offset];
}

template <typename predicate>
Token Lexer::lexWhile(TokenType type, predicate &&pred) {
    while (pred()) {
        switch (this->current()) {
        case '\0':
        case '\n':
            goto end;
        }
        advance();
    }
end:
    return this->returnToken(type, true);
}

Token Lexer::throwError(const std::string &message) {
    error.addError(message, this->location);
    did_encounter_error = true;
    advanceLineComment();
    return this->getToken();
}

Token Lexer::throwError(const std::string &message, Location &loc) {
    error.addError(message, loc);
    did_encounter_error = true;
    advanceLineComment();
    return this->getToken();
}

} // namespace drast::lexer
