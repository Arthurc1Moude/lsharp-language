/* ============================================================================
 * L# Programming Language - Lexical Analyzer
 * ============================================================================
 * File: lexer.l
 * Format: Flex/Lex
 * Purpose: Tokenize L# source code into lexical tokens
 * 
 * Copyright (c) 2026 Moude AI Inc.
 * Licensed under MIT License
 * ============================================================================
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"
#include "ast.h"

/* Global variables for position tracking */
int line_num = 1;
int col_num = 1;
int prev_col_num = 1;

/* String buffer for string literals */
#define MAX_STR_CONST 4096
char string_buf[MAX_STR_CONST];
char *string_buf_ptr;

/* Error reporting */
void lexical_error(const char *msg);
void warning(const char *msg);

/* Position tracking macro */
#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = line_num; \
    yylloc.first_column = col_num; \
    yylloc.last_column = col_num + yyleng - 1; \
    prev_col_num = col_num; \
    col_num += yyleng;

/* Token counting for statistics */
static unsigned long token_count = 0;
static unsigned long line_count = 0;

#define COUNT_TOKEN() token_count++
%}

/* Flex options */
%option noyywrap
%option yylineno
%option case-sensitive
%option never-interactive
%option nounput
%option noinput

/* Start conditions for string literals and comments */
%x STRING_STATE
%x TEMPLATE_STRING_STATE
%x BLOCK_COMMENT_STATE
%x DOC_COMMENT_STATE

/* ============================================================================
 * PATTERN DEFINITIONS
 * ============================================================================
 */

/* Character classes */
DIGIT           [0-9]
DIGIT_SEQ       {DIGIT}(_?{DIGIT})*
HEX_DIGIT       [0-9A-Fa-f]
HEX_SEQ         {HEX_DIGIT}(_?{HEX_DIGIT})*
BINARY_DIGIT    [01]
BIN_SEQ         {BINARY_DIGIT}(_?{BINARY_DIGIT})*
OCTAL_DIGIT     [0-7]
OCT_SEQ         {OCTAL_DIGIT}(_?{OCTAL_DIGIT})*

/* Letters and identifiers */
LETTER          [a-zA-Z]
UNDERSCORE      _
ID_START        ({LETTER}|{UNDERSCORE})
ID_CONTINUE     ({LETTER}|{DIGIT}|{UNDERSCORE})
IDENTIFIER      {ID_START}{ID_CONTINUE}*
TYPE_ID         [A-Z]{ID_CONTINUE}*
CONST_ID        [A-Z][A-Z0-9_]*

/* Unicode support */
UNICODE_ESCAPE  \\u{HEX_DIGIT}{4}
UNICODE_LONG    \\U{HEX_DIGIT}{8}

/* Whitespace */
WHITESPACE      [ \t\v\f]
NEWLINE         (\r\n|\r|\n)

/* Integer literals */
DEC_INTEGER     {DIGIT_SEQ}
HEX_INTEGER     0[xX]{HEX_SEQ}
BIN_INTEGER     0[bB]{BIN_SEQ}
OCT_INTEGER     0[oO]{OCT_SEQ}

/* Floating point literals */
EXPONENT        [eE][+-]?{DIGIT_SEQ}
FLOAT_SUFFIX    [fF]
DOUBLE_SUFFIX   [dD]
DECIMAL_FLOAT   {DIGIT_SEQ}\.{DIGIT_SEQ}{EXPONENT}?{FLOAT_SUFFIX}?
EXPONENT_FLOAT  {DIGIT_SEQ}{EXPONENT}{FLOAT_SUFFIX}?
HEX_FLOAT       0[xX]{HEX_SEQ}\.{HEX_SEQ}[pP][+-]?{DIGIT_SEQ}

/* String escape sequences */
SIMPLE_ESCAPE   \\[nrtfvab\\\'\"]
HEX_ESCAPE      \\x{HEX_DIGIT}{2}
UNICODE_ESC     {UNICODE_ESCAPE}|{UNICODE_LONG}
ESCAPE_SEQ      {SIMPLE_ESCAPE}|{HEX_ESCAPE}|{UNICODE_ESC}

/* Character literals */
CHAR_CONTENT    [^\\\'\n]|{ESCAPE_SEQ}

%%

/* ============================================================================
 * COMMENT RULES
 * ============================================================================
 */

"//".*                          { COUNT_TOKEN(); /* Line comment */ }

"/*"                            { 
                                    COUNT_TOKEN();
                                    BEGIN(BLOCK_COMMENT_STATE); 
                                }

<BLOCK_COMMENT_STATE>{
    "*/"                        { BEGIN(INITIAL); }
    [^*\n]+                     { /* Eat comment */ }
    "*"                         { /* Eat lone star */ }
    {NEWLINE}                   { line_num++; col_num = 1; }
    <<EOF>>                     { 
                                    lexical_error("Unterminated block comment");
                                    return ERROR;
                                }
}

"/**"                           {
                                    COUNT_TOKEN();
                                    BEGIN(DOC_COMMENT_STATE);
                                }

<DOC_COMMENT_STATE>{
    "*/"                        { BEGIN(INITIAL); }
    "@param"                    { /* Documentation tag */ }
    "@return"                   { /* Documentation tag */ }
    "@throws"                   { /* Documentation tag */ }
    "@example"                  { /* Documentation tag */ }
    "@author"                   { /* Documentation tag */ }
    "@version"                  { /* Documentation tag */ }
    "@since"                    { /* Documentation tag */ }
    "@deprecated"               { /* Documentation tag */ }
    "@see"                      { /* Documentation tag */ }
    [^*@\n]+                    { /* Eat comment */ }
    "*"                         { /* Eat lone star */ }
    {NEWLINE}                   { line_num++; col_num = 1; }
    <<EOF>>                     { 
                                    lexical_error("Unterminated doc comment");
                                    return ERROR;
                                }
}

/* ============================================================================
 * KEYWORD RULES - CONTROL FLOW
 * ============================================================================
 */

"if"                            { COUNT_TOKEN(); return IF; }
"else"                          { COUNT_TOKEN(); return ELSE; }
"elif"                          { COUNT_TOKEN(); return ELIF; }
"match"                         { COUNT_TOKEN(); return MATCH; }
"case"                          { COUNT_TOKEN(); return CASE; }
"for"                           { COUNT_TOKEN(); return FOR; }
"while"                         { COUNT_TOKEN(); return WHILE; }
"loop"                          { COUNT_TOKEN(); return LOOP; }
"do"                            { COUNT_TOKEN(); return DO; }
"break"                         { COUNT_TOKEN(); return BREAK; }
"continue"                      { COUNT_TOKEN(); return CONTINUE; }
"return"                        { COUNT_TOKEN(); return RETURN; }
"goto"                          { COUNT_TOKEN(); return GOTO; }

/* ============================================================================
 * KEYWORD RULES - DECLARATIONS
 * ============================================================================
 */

"let"                           { COUNT_TOKEN(); return LET; }
"const"                         { COUNT_TOKEN(); return CONST; }
"var"                           { COUNT_TOKEN(); return VAR; }
"fn"                            { COUNT_TOKEN(); return FN; }
"func"                          { COUNT_TOKEN(); return FUNC; }
"function"                      { COUNT_TOKEN(); return FUNCTION; }
"class"                         { COUNT_TOKEN(); return CLASS; }
"struct"                        { COUNT_TOKEN(); return STRUCT; }
"enum"                          { COUNT_TOKEN(); return ENUM; }
"union"                         { COUNT_TOKEN(); return UNION; }
"interface"                     { COUNT_TOKEN(); return INTERFACE; }
"trait"                         { COUNT_TOKEN(); return TRAIT; }
"impl"                          { COUNT_TOKEN(); return IMPL; }
"type"                          { COUNT_TOKEN(); return TYPE; }
"alias"                         { COUNT_TOKEN(); return ALIAS; }
"typedef"                       { COUNT_TOKEN(); return TYPEDEF; }
"namespace"                     { COUNT_TOKEN(); return NAMESPACE; }
"module"                        { COUNT_TOKEN(); return MODULE; }
"package"                       { COUNT_TOKEN(); return PACKAGE; }

/* ============================================================================
 * KEYWORD RULES - MODIFIERS
 * ============================================================================
 */

"pub"                           { COUNT_TOKEN(); return PUB; }
"public"                        { COUNT_TOKEN(); return PUBLIC; }
"priv"                          { COUNT_TOKEN(); return PRIV; }
"private"                       { COUNT_TOKEN(); return PRIVATE; }
"protected"                     { COUNT_TOKEN(); return PROTECTED; }
"internal"                      { COUNT_TOKEN(); return INTERNAL; }
"static"                        { COUNT_TOKEN(); return STATIC; }
"final"                         { COUNT_TOKEN(); return FINAL; }
"abstract"                      { COUNT_TOKEN(); return ABSTRACT; }
"virtual"                       { COUNT_TOKEN(); return VIRTUAL; }
"override"                      { COUNT_TOKEN(); return OVERRIDE; }
"sealed"                        { COUNT_TOKEN(); return SEALED; }
"mut"                           { COUNT_TOKEN(); return MUT; }
"mutable"                       { COUNT_TOKEN(); return MUTABLE; }
"ref"                           { COUNT_TOKEN(); return REF; }
"const"                         { COUNT_TOKEN(); return CONST_MOD; }
"volatile"                      { COUNT_TOKEN(); return VOLATILE; }
"inline"                        { COUNT_TOKEN(); return INLINE; }
"extern"                        { COUNT_TOKEN(); return EXTERN; }
"export"                        { COUNT_TOKEN(); return EXPORT; }

/* ============================================================================
 * KEYWORD RULES - MODULE SYSTEM
 * ============================================================================
 */

"use"                           { COUNT_TOKEN(); return USE; }
"import"                        { COUNT_TOKEN(); return IMPORT; }
"from"                          { COUNT_TOKEN(); return FROM; }
"as"                            { COUNT_TOKEN(); return AS; }
"with"                          { COUNT_TOKEN(); return WITH; }

/* ============================================================================
 * KEYWORD RULES - OPERATORS (WORD FORM)
 * ============================================================================
 */

"and"                           { COUNT_TOKEN(); return AND; }
"or"                            { COUNT_TOKEN(); return OR; }
"not"                           { COUNT_TOKEN(); return NOT; }
"xor"                           { COUNT_TOKEN(); return XOR; }
"in"                            { COUNT_TOKEN(); return IN; }
"is"                            { COUNT_TOKEN(); return IS; }
"as"                            { COUNT_TOKEN(); return AS_OP; }

/* ============================================================================
 * KEYWORD RULES - MEMORY & POINTERS
 * ============================================================================
 */

"new"                           { COUNT_TOKEN(); return NEW; }
"delete"                        { COUNT_TOKEN(); return DELETE; }
"sizeof"                        { COUNT_TOKEN(); return SIZEOF; }
"typeof"                        { COUNT_TOKEN(); return TYPEOF; }
"instanceof"                    { COUNT_TOKEN(); return INSTANCEOF; }
"alignof"                       { COUNT_TOKEN(); return ALIGNOF; }
"offsetof"                      { COUNT_TOKEN(); return OFFSETOF; }

/* ============================================================================
 * KEYWORD RULES - ASYNC/CONCURRENCY
 * ============================================================================
 */

"async"                         { COUNT_TOKEN(); return ASYNC; }
"await"                         { COUNT_TOKEN(); return AWAIT; }
"yield"                         { COUNT_TOKEN(); return YIELD; }
"spawn"                         { COUNT_TOKEN(); return SPAWN; }
"thread"                        { COUNT_TOKEN(); return THREAD; }
"sync"                          { COUNT_TOKEN(); return SYNC; }
"atomic"                        { COUNT_TOKEN(); return ATOMIC; }
"lock"                          { COUNT_TOKEN(); return LOCK; }
"unlock"                        { COUNT_TOKEN(); return UNLOCK; }

/* ============================================================================
 * KEYWORD RULES - SPECIAL
 * ============================================================================
 */

"self"                          { COUNT_TOKEN(); return SELF; }
"Self"                          { COUNT_TOKEN(); return SELF_TYPE; }
"this"                          { COUNT_TOKEN(); return THIS; }
"super"                         { COUNT_TOKEN(); return SUPER; }
"base"                          { COUNT_TOKEN(); return BASE; }
"where"                         { COUNT_TOKEN(); return WHERE; }
"move"                          { COUNT_TOKEN(); return MOVE; }
"copy"                          { COUNT_TOKEN(); return COPY; }
"clone"                         { COUNT_TOKEN(); return CLONE; }
"drop"                          { COUNT_TOKEN(); return DROP; }
"unsafe"                        { COUNT_TOKEN(); return UNSAFE; }
"dyn"                           { COUNT_TOKEN(); return DYN; }
"box"                           { COUNT_TOKEN(); return BOX; }
"pin"                           { COUNT_TOKEN(); return PIN; }

/* ============================================================================
 * KEYWORD RULES - MACROS & ATTRIBUTES
 * ============================================================================
 */

"macro"                         { COUNT_TOKEN(); return MACRO; }
"macro_rules"                   { COUNT_TOKEN(); return MACRO_RULES; }
"derive"                        { COUNT_TOKEN(); return DERIVE; }
"cfg"                           { COUNT_TOKEN(); return CFG; }
"test"                          { COUNT_TOKEN(); return TEST; }
"bench"                         { COUNT_TOKEN(); return BENCH; }
"deprecated"                    { COUNT_TOKEN(); return DEPRECATED; }

/* ============================================================================
 * KEYWORD RULES - ERROR HANDLING
 * ============================================================================
 */

"try"                           { COUNT_TOKEN(); return TRY; }
"catch"                         { COUNT_TOKEN(); return CATCH; }
"finally"                       { COUNT_TOKEN(); return FINALLY; }
"throw"                         { COUNT_TOKEN(); return THROW; }
"panic"                         { COUNT_TOKEN(); return PANIC; }
"assert"                        { COUNT_TOKEN(); return ASSERT; }

/* ============================================================================
 * PRIMITIVE TYPE KEYWORDS
 * ============================================================================
 */

"int"                           { COUNT_TOKEN(); return INT_TYPE; }
"i8"                            { COUNT_TOKEN(); return I8_TYPE; }
"i16"                           { COUNT_TOKEN(); return I16_TYPE; }
"i32"                           { COUNT_TOKEN(); return I32_TYPE; }
"i64"                           { COUNT_TOKEN(); return I64_TYPE; }
"i128"                          { COUNT_TOKEN(); return I128_TYPE; }
"uint"                          { COUNT_TOKEN(); return UINT_TYPE; }
"u8"                            { COUNT_TOKEN(); return U8_TYPE; }
"u16"                           { COUNT_TOKEN(); return U16_TYPE; }
"u32"                           { COUNT_TOKEN(); return U32_TYPE; }
"u64"                           { COUNT_TOKEN(); return U64_TYPE; }
"u128"                          { COUNT_TOKEN(); return U128_TYPE; }
"float"                         { COUNT_TOKEN(); return FLOAT_TYPE; }
"f32"                           { COUNT_TOKEN(); return F32_TYPE; }
"f64"                           { COUNT_TOKEN(); return F64_TYPE; }
"double"                        { COUNT_TOKEN(); return DOUBLE_TYPE; }
"bool"                          { COUNT_TOKEN(); return BOOL_TYPE; }
"string"                        { COUNT_TOKEN(); return STRING_TYPE; }
"str"                           { COUNT_TOKEN(); return STR_TYPE; }
"char"                          { COUNT_TOKEN(); return CHAR_TYPE; }
"byte"                          { COUNT_TOKEN(); return BYTE_TYPE; }
"void"                          { COUNT_TOKEN(); return VOID_TYPE; }
"any"                           { COUNT_TOKEN(); return ANY_TYPE; }
"never"                         { COUNT_TOKEN(); return NEVER_TYPE; }
"unit"                          { COUNT_TOKEN(); return UNIT_TYPE; }
"isize"                         { COUNT_TOKEN(); return ISIZE_TYPE; }
"usize"                         { COUNT_TOKEN(); return USIZE_TYPE; }

/* ============================================================================
 * BOOLEAN & NULL LITERALS
 * ============================================================================
 */

"true"                          { COUNT_TOKEN(); yylval.bool_val = 1; return TRUE; }
"false"                         { COUNT_TOKEN(); yylval.bool_val = 0; return FALSE; }
"null"                          { COUNT_TOKEN(); return NULL_LIT; }
"nil"                           { COUNT_TOKEN(); return NIL; }
"none"                          { COUNT_TOKEN(); return NONE; }
"undefined"                     { COUNT_TOKEN(); return UNDEFINED; }

/* ============================================================================
 * OPERATOR RULES - ARITHMETIC
 * ============================================================================
 */

"+"                             { COUNT_TOKEN(); return PLUS; }
"-"                             { COUNT_TOKEN(); return MINUS; }
"*"                             { COUNT_TOKEN(); return STAR; }
"/"                             { COUNT_TOKEN(); return SLASH; }
"%"                             { COUNT_TOKEN(); return PERCENT; }
"**"                            { COUNT_TOKEN(); return POWER; }
"++"                            { COUNT_TOKEN(); return INCREMENT; }
"--"                            { COUNT_TOKEN(); return DECREMENT; }

/* ============================================================================
 * OPERATOR RULES - COMPARISON
 * ============================================================================
 */

"=="                            { COUNT_TOKEN(); return EQ; }
"!="                            { COUNT_TOKEN(); return NE; }
"<"                             { COUNT_TOKEN(); return LT; }
">"                             { COUNT_TOKEN(); return GT; }
"<="                            { COUNT_TOKEN(); return LE; }
">="                            { COUNT_TOKEN(); return GE; }
"==="                           { COUNT_TOKEN(); return STRICT_EQ; }
"!=="                           { COUNT_TOKEN(); return STRICT_NE; }
"<=>"                           { COUNT_TOKEN(); return SPACESHIP; }

/* ============================================================================
 * OPERATOR RULES - LOGICAL
 * ============================================================================
 */

"&&"                            { COUNT_TOKEN(); return LOGICAL_AND; }
"||"                            { COUNT_TOKEN(); return LOGICAL_OR; }
"!"                             { COUNT_TOKEN(); return LOGICAL_NOT; }

/* ============================================================================
 * OPERATOR RULES - BITWISE
 * ============================================================================
 */

"&"                             { COUNT_TOKEN(); return AMPERSAND; }
"|"                             { COUNT_TOKEN(); return PIPE; }
"^"                             { COUNT_TOKEN(); return CARET; }
"~"                             { COUNT_TOKEN(); return TILDE; }
"<<"                            { COUNT_TOKEN(); return LSHIFT; }
">>"                            { COUNT_TOKEN(); return RSHIFT; }
">>>"                           { COUNT_TOKEN(); return UNSIGNED_RSHIFT; }

/* ============================================================================
 * OPERATOR RULES - ASSIGNMENT
 * ============================================================================
 */

"="                             { COUNT_TOKEN(); return ASSIGN; }
"+="                            { COUNT_TOKEN(); return PLUS_ASSIGN; }
"-="                            { COUNT_TOKEN(); return MINUS_ASSIGN; }
"*="                            { COUNT_TOKEN(); return STAR_ASSIGN; }
"/="                            { COUNT_TOKEN(); return SLASH_ASSIGN; }
"%="                            { COUNT_TOKEN(); return PERCENT_ASSIGN; }
"**="                           { COUNT_TOKEN(); return POWER_ASSIGN; }
"&="                            { COUNT_TOKEN(); return AND_ASSIGN; }
"|="                            { COUNT_TOKEN(); return OR_ASSIGN; }
"^="                            { COUNT_TOKEN(); return XOR_ASSIGN; }
"<<="                           { COUNT_TOKEN(); return LSHIFT_ASSIGN; }
">>="                           { COUNT_TOKEN(); return RSHIFT_ASSIGN; }
">>>="                          { COUNT_TOKEN(); return UNSIGNED_RSHIFT_ASSIGN; }
"&&="                           { COUNT_TOKEN(); return LOGICAL_AND_ASSIGN; }
"||="                           { COUNT_TOKEN(); return LOGICAL_OR_ASSIGN; }
"??="                           { COUNT_TOKEN(); return NULL_COALESCE_ASSIGN; }

/* ============================================================================
 * OPERATOR RULES - SPECIAL
 * ============================================================================
 */

"->"                            { COUNT_TOKEN(); return ARROW; }
"=>"                            { COUNT_TOKEN(); return FAT_ARROW; }
".."                            { COUNT_TOKEN(); return RANGE; }
"..="                           { COUNT_TOKEN(); return RANGE_INCLUSIVE; }
"..."                           { COUNT_TOKEN(); return SPREAD; }
"?."                            { COUNT_TOKEN(); return OPTIONAL_CHAIN; }
"??"                            { COUNT_TOKEN(); return NULL_COALESCE; }
"?"                             { COUNT_TOKEN(); return QUESTION; }
"::"                            { COUNT_TOKEN(); return DOUBLE_COLON; }
"@"                             { COUNT_TOKEN(); return AT; }
"#"                             { COUNT_TOKEN(); return HASH; }
"$"                             { COUNT_TOKEN(); return DOLLAR; }

/* ============================================================================
 * DELIMITER RULES
 * ============================================================================
 */

"("                             { COUNT_TOKEN(); return LPAREN; }
")"                             { COUNT_TOKEN(); return RPAREN; }
"{"                             { COUNT_TOKEN(); return LBRACE; }
"}"                             { COUNT_TOKEN(); return RBRACE; }
"["                             { COUNT_TOKEN(); return LBRACKET; }
"]"                             { COUNT_TOKEN(); return RBRACKET; }
";"                             { COUNT_TOKEN(); return SEMICOLON; }
":"                             { COUNT_TOKEN(); return COLON; }
","                             { COUNT_TOKEN(); return COMMA; }
"."                             { COUNT_TOKEN(); return DOT; }

/* ============================================================================
 * NUMBER LITERAL RULES
 * ============================================================================
 */

{HEX_INTEGER}                   { 
                                    COUNT_TOKEN();
                                    yylval.int_val = strtoll(yytext, NULL, 16); 
                                    return INTEGER; 
                                }

{BIN_INTEGER}                   { 
                                    COUNT_TOKEN();
                                    yylval.int_val = strtoll(yytext + 2, NULL, 2); 
                                    return INTEGER; 
                                }

{OCT_INTEGER}                   { 
                                    COUNT_TOKEN();
                                    yylval.int_val = strtoll(yytext + 2, NULL, 8); 
                                    return INTEGER; 
                                }

{DEC_INTEGER}                   { 
                                    COUNT_TOKEN();
                                    yylval.int_val = atoll(yytext); 
                                    return INTEGER; 
                                }

{DECIMAL_FLOAT}                 { 
                                    COUNT_TOKEN();
                                    yylval.float_val = atof(yytext); 
                                    return FLOAT; 
                                }

{EXPONENT_FLOAT}                { 
                                    COUNT_TOKEN();
                                    yylval.float_val = atof(yytext); 
                                    return FLOAT; 
                                }

{HEX_FLOAT}                     { 
                                    COUNT_TOKEN();
                                    yylval.float_val = strtod(yytext, NULL); 
                                    return FLOAT; 
                                }

/* ============================================================================
 * STRING LITERAL RULES
 * ============================================================================
 */

\"                              { 
                                    COUNT_TOKEN();
                                    string_buf_ptr = string_buf; 
                                    BEGIN(STRING_STATE); 
                                }

<STRING_STATE>{
    \"                          { 
                                    BEGIN(INITIAL);
                                    *string_buf_ptr = '\0';
                                    yylval.str_val = strdup(string_buf);
                                    return STRING;
                                }
    
    {ESCAPE_SEQ}                { 
                                    /* Handle escape sequences */
                                    if (string_buf_ptr - string_buf >= MAX_STR_CONST - 1) {
                                        lexical_error("String constant too long");
                                        return ERROR;
                                    }
                                    switch (yytext[1]) {
                                        case 'n': *string_buf_ptr++ = '\n'; break;
                                        case 'r': *string_buf_ptr++ = '\r'; break;
                                        case 't': *string_buf_ptr++ = '\t'; break;
                                        case 'f': *string_buf_ptr++ = '\f'; break;
                                        case 'v': *string_buf_ptr++ = '\v'; break;
                                        case 'a': *string_buf_ptr++ = '\a'; break;
                                        case 'b': *string_buf_ptr++ = '\b'; break;
                                        case '\\': *string_buf_ptr++ = '\\'; break;
                                        case '\'': *string_buf_ptr++ = '\''; break;
                                        case '\"': *string_buf_ptr++ = '\"'; break;
                                        case '0': *string_buf_ptr++ = '\0'; break;
                                        default: *string_buf_ptr++ = yytext[1]; break;
                                    }
                                }
    
    [^\\\"\n]+                  { 
                                    char *yptr = yytext;
                                    while (*yptr) {
                                        if (string_buf_ptr - string_buf >= MAX_STR_CONST - 1) {
                                            lexical_error("String constant too long");
                                            return ERROR;
                                        }
                                        *string_buf_ptr++ = *yptr++;
                                    }
                                }
    
    {NEWLINE}                   { 
                                    lexical_error("Unterminated string literal");
                                    BEGIN(INITIAL);
                                    return ERROR;
                                }
    
    <<EOF>>                     { 
                                    lexical_error("Unterminated string literal at EOF");
                                    return ERROR;
                                }
}

/* Template strings */
`                               {
                                    COUNT_TOKEN();
                                    string_buf_ptr = string_buf;
                                    BEGIN(TEMPLATE_STRING_STATE);
                                }

<TEMPLATE_STRING_STATE>{
    `                           {
                                    BEGIN(INITIAL);
                                    *string_buf_ptr = '\0';
                                    yylval.str_val = strdup(string_buf);
                                    return TEMPLATE_STRING;
                                }
    
    [^`\\]+                     {
                                    char *yptr = yytext;
                                    while (*yptr) {
                                        if (string_buf_ptr - string_buf >= MAX_STR_CONST - 1) {
                                            lexical_error("Template string too long");
                                            return ERROR;
                                        }
                                        *string_buf_ptr++ = *yptr++;
                                    }
                                }
    
    {ESCAPE_SEQ}                {
                                    /* Handle escape sequences in template strings */
                                    *string_buf_ptr++ = yytext[1];
                                }
    
    <<EOF>>                     {
                                    lexical_error("Unterminated template string");
                                    return ERROR;
                                }
}

/* Character literals */
'{CHAR_CONTENT}'                { 
                                    COUNT_TOKEN();
                                    yylval.char_val = yytext[1]; 
                                    return CHAR; 
                                }

/* ============================================================================
 * IDENTIFIER RULES
 * ============================================================================
 */

{CONST_ID}                      { 
                                    COUNT_TOKEN();
                                    yylval.str_val = strdup(yytext); 
                                    return CONST_IDENTIFIER; 
                                }

{TYPE_ID}                       { 
                                    COUNT_TOKEN();
                                    yylval.str_val = strdup(yytext); 
                                    return TYPE_IDENTIFIER; 
                                }

{IDENTIFIER}                    { 
                                    COUNT_TOKEN();
                                    yylval.str_val = strdup(yytext); 
                                    return IDENTIFIER; 
                                }

/* ============================================================================
 * WHITESPACE & NEWLINE RULES
 * ============================================================================
 */

{WHITESPACE}+                   { /* Ignore whitespace */ }

{NEWLINE}                       { 
                                    line_num++; 
                                    line_count++;
                                    col_num = 1; 
                                }

/* ============================================================================
 * ERROR HANDLING
 * ============================================================================
 */

.                               { 
                                    char msg[256];
                                    snprintf(msg, sizeof(msg), 
                                            "Unexpected character '%s' (0x%02X)", 
                                            yytext, (unsigned char)yytext[0]);
                                    lexical_error(msg);
                                    return ERROR;
                                }

<<EOF>>                         { 
                                    return END_OF_FILE; 
                                }

%%

/* ============================================================================
 * ADDITIONAL C CODE
 * ============================================================================
 */

void lexical_error(const char *msg) {
    fprintf(stderr, "\033[1;31mLexical Error\033[0m at line %d, column %d: %s\n", 
            line_num, prev_col_num, msg);
}

void warning(const char *msg) {
    fprintf(stderr, "\033[1;33mWarning\033[0m at line %d, column %d: %s\n", 
            line_num, prev_col_num, msg);
}

void yyerror(const char *s) {
    fprintf(stderr, "\033[1;31mParse Error\033[0m at line %d, column %d: %s\n", 
            line_num, col_num, s);
}

/* Get lexer statistics */
void print_lexer_stats() {
    printf("\n=== Lexer Statistics ===\n");
    printf("Total tokens: %lu\n", token_count);
    printf("Total lines: %lu\n", line_count);
    printf("========================\n");
}

/* Reset lexer state */
void reset_lexer() {
    line_num = 1;
    col_num = 1;
    prev_col_num = 1;
    token_count = 0;
    line_count = 0;
}

/* ============================================================================
 * EXTENDED LEXER FEATURES - ADVANCED TOKEN HANDLING
 * ============================================================================
 */

/* Lifetime annotations */
'[a-z][a-zA-Z0-9_]*             {
                                    COUNT_TOKEN();
                                    yylval.str_val = strdup(yytext + 1); // Skip apostrophe
                                    return LIFETIME;
                                }

/* Raw string literals */
r\"[^"]*\"                      {
                                    COUNT_TOKEN();
                                    yylval.str_val = strndup(yytext + 2, yyleng - 3);
                                    return RAW_STRING;
                                }

r#\".*?\"#                      {
                                    COUNT_TOKEN();
                                    yylval.str_val = strndup(yytext + 3, yyleng - 5);
                                    return RAW_STRING;
                                }

/* Byte string literals */
b\"[^"]*\"                      {
                                    COUNT_TOKEN();
                                    yylval.str_val = strndup(yytext + 2, yyleng - 3);
                                    return BYTE_STRING;
                                }

/* Byte literals */
b'{CHAR_CONTENT}'               {
                                    COUNT_TOKEN();
                                    yylval.char_val = yytext[2];
                                    return BYTE_LITERAL;
                                }

/* Integer suffixes */
{DEC_INTEGER}[iu](8|16|32|64|128|size) {
                                    COUNT_TOKEN();
                                    char *suffix_start = yytext;
                                    while (*suffix_start && isdigit(*suffix_start)) suffix_start++;
                                    yylval.int_val = atoll(yytext);
                                    return INTEGER_SUFFIXED;
                                }

/* Float suffixes */
{DECIMAL_FLOAT}[fd]             {
                                    COUNT_TOKEN();
                                    yylval.float_val = atof(yytext);
                                    return FLOAT_SUFFIXED;
                                }

/* ============================================================================
 * PREPROCESSOR DIRECTIVES
 * ============================================================================
 */

^[ \t]*#[ \t]*include[ \t]+\"[^"]+\" {
                                    COUNT_TOKEN();
                                    /* Extract filename from #include "file" */
                                    char *start = strchr(yytext, '"') + 1;
                                    char *end = strrchr(yytext, '"');
                                    yylval.str_val = strndup(start, end - start);
                                    return INCLUDE_DIRECTIVE;
                                }

^[ \t]*#[ \t]*define[ \t]+{IDENTIFIER} {
                                    COUNT_TOKEN();
                                    char *id_start = yytext;
                                    while (*id_start && !isalpha(*id_start) && *id_start != '_') id_start++;
                                    yylval.str_val = strdup(id_start);
                                    return DEFINE_DIRECTIVE;
                                }

^[ \t]*#[ \t]*ifdef[ \t]+{IDENTIFIER} {
                                    COUNT_TOKEN();
                                    return IFDEF_DIRECTIVE;
                                }

^[ \t]*#[ \t]*ifndef[ \t]+{IDENTIFIER} {
                                    COUNT_TOKEN();
                                    return IFNDEF_DIRECTIVE;
                                }

^[ \t]*#[ \t]*endif             {
                                    COUNT_TOKEN();
                                    return ENDIF_DIRECTIVE;
                                }

^[ \t]*#[ \t]*pragma[ \t]+.*    {
                                    COUNT_TOKEN();
                                    return PRAGMA_DIRECTIVE;
                                }

/* ============================================================================
 * ADDITIONAL OPERATORS AND SYMBOLS
 * ============================================================================
 */

"<=>"                           { COUNT_TOKEN(); return SPACESHIP; }
"?:"                            { COUNT_TOKEN(); return ELVIS; }
"!!"                            { COUNT_TOKEN(); return DOUBLE_BANG; }
"..<"                           { COUNT_TOKEN(); return RANGE_EXCLUSIVE; }
"..>"                           { COUNT_TOKEN(); return RANGE_OPEN_END; }
"<.."                           { COUNT_TOKEN(); return RANGE_OPEN_START; }
"~="                            { COUNT_TOKEN(); return APPROX_EQ; }
"=~"                            { COUNT_TOKEN(); return REGEX_MATCH; }
"!~"                            { COUNT_TOKEN(); return REGEX_NOT_MATCH; }
"|>"                            { COUNT_TOKEN(); return PIPE_FORWARD; }
"<|"                            { COUNT_TOKEN(); return PIPE_BACKWARD; }
"<>"                            { COUNT_TOKEN(); return DIAMOND; }
"<+>"                           { COUNT_TOKEN(); return APPEND_OP; }
"<*>"                           { COUNT_TOKEN(); return APPLY_OP; }
"<$>"                           { COUNT_TOKEN(); return FMAP_OP; }
">>="                           { COUNT_TOKEN(); return BIND_OP; }
"=<<"                           { COUNT_TOKEN(); return REVERSE_BIND; }
"<&>"                           { COUNT_TOKEN(); return PARALLEL_OP; }
"<|>"                           { COUNT_TOKEN(); return ALTERNATIVE_OP; }

/* ============================================================================
 * UNICODE OPERATORS (MATHEMATICAL SYMBOLS)
 * ============================================================================
 */

"∀"                             { COUNT_TOKEN(); return FORALL; }
"∃"                             { COUNT_TOKEN(); return EXISTS; }
"∈"                             { COUNT_TOKEN(); return ELEMENT_OF; }
"∉"                             { COUNT_TOKEN(); return NOT_ELEMENT_OF; }
"⊆"                             { COUNT_TOKEN(); return SUBSET; }
"⊇"                             { COUNT_TOKEN(); return SUPERSET; }
"∪"                             { COUNT_TOKEN(); return UNION; }
"∩"                             { COUNT_TOKEN(); return INTERSECTION; }
"∅"                             { COUNT_TOKEN(); return EMPTY_SET; }
"∞"                             { COUNT_TOKEN(); return INFINITY; }
"≤"                             { COUNT_TOKEN(); return LE; }
"≥"                             { COUNT_TOKEN(); return GE; }
"≠"                             { COUNT_TOKEN(); return NE; }
"≈"                             { COUNT_TOKEN(); return APPROX; }
"≡"                             { COUNT_TOKEN(); return EQUIV; }
"→"                             { COUNT_TOKEN(); return ARROW; }
"⇒"                             { COUNT_TOKEN(); return IMPLIES; }
"⇔"                             { COUNT_TOKEN(); return IFF; }
"∧"                             { COUNT_TOKEN(); return LOGICAL_AND; }
"∨"                             { COUNT_TOKEN(); return LOGICAL_OR; }
"¬"                             { COUNT_TOKEN(); return LOGICAL_NOT; }
"⊕"                             { COUNT_TOKEN(); return XOR; }
"λ"                             { COUNT_TOKEN(); return LAMBDA; }
"π"                             { COUNT_TOKEN(); return PI; }
"τ"                             { COUNT_TOKEN(); return TAU; }
"Σ"                             { COUNT_TOKEN(); return SIGMA; }
"Π"                             { COUNT_TOKEN(); return PRODUCT; }
"∫"                             { COUNT_TOKEN(); return INTEGRAL; }
"∂"                             { COUNT_TOKEN(); return PARTIAL; }
"∇"                             { COUNT_TOKEN(); return NABLA; }
"√"                             { COUNT_TOKEN(); return SQRT; }
"∛"                             { COUNT_TOKEN(); return CBRT; }
"∜"                             { COUNT_TOKEN(); return FOURTHRT; }
"°"                             { COUNT_TOKEN(); return DEGREE; }
"±"                             { COUNT_TOKEN(); return PLUS_MINUS; }
"∓"                             { COUNT_TOKEN(); return MINUS_PLUS; }
"×"                             { COUNT_TOKEN(); return TIMES; }
"÷"                             { COUNT_TOKEN(); return DIVIDE; }
"⋅"                             { COUNT_TOKEN(); return DOT_PRODUCT; }
"⊗"                             { COUNT_TOKEN(); return TENSOR_PRODUCT; }
"⊙"                             { COUNT_TOKEN(); return HADAMARD_PRODUCT; }

/* ============================================================================
 * ADDITIONAL KEYWORDS - ADVANCED FEATURES
 * ============================================================================
 */

"crate"                         { COUNT_TOKEN(); return CRATE; }
"super"                         { COUNT_TOKEN(); return SUPER; }
"mod"                           { COUNT_TOKEN(); return MOD; }
"priv"                          { COUNT_TOKEN(); return PRIV; }
"pub"                           { COUNT_TOKEN(); return PUB; }
"use"                           { COUNT_TOKEN(); return USE; }
"as"                            { COUNT_TOKEN(); return AS; }
"break"                         { COUNT_TOKEN(); return BREAK; }
"const"                         { COUNT_TOKEN(); return CONST; }
"continue"                      { COUNT_TOKEN(); return CONTINUE; }
"crate"                         { COUNT_TOKEN(); return CRATE; }
"else"                          { COUNT_TOKEN(); return ELSE; }
"enum"                          { COUNT_TOKEN(); return ENUM; }
"extern"                        { COUNT_TOKEN(); return EXTERN; }
"false"                         { COUNT_TOKEN(); return FALSE; }
"fn"                            { COUNT_TOKEN(); return FN; }
"for"                           { COUNT_TOKEN(); return FOR; }
"if"                            { COUNT_TOKEN(); return IF; }
"impl"                          { COUNT_TOKEN(); return IMPL; }
"in"                            { COUNT_TOKEN(); return IN; }
"let"                           { COUNT_TOKEN(); return LET; }
"loop"                          { COUNT_TOKEN(); return LOOP; }
"match"                         { COUNT_TOKEN(); return MATCH; }
"mod"                           { COUNT_TOKEN(); return MOD; }
"move"                          { COUNT_TOKEN(); return MOVE; }
"mut"                           { COUNT_TOKEN(); return MUT; }
"pub"                           { COUNT_TOKEN(); return PUB; }
"ref"                           { COUNT_TOKEN(); return REF; }
"return"                        { COUNT_TOKEN(); return RETURN; }
"self"                          { COUNT_TOKEN(); return SELF; }
"Self"                          { COUNT_TOKEN(); return SELF_TYPE; }
"static"                        { COUNT_TOKEN(); return STATIC; }
"struct"                        { COUNT_TOKEN(); return STRUCT; }
"super"                         { COUNT_TOKEN(); return SUPER; }
"trait"                         { COUNT_TOKEN(); return TRAIT; }
"true"                          { COUNT_TOKEN(); return TRUE; }
"type"                          { COUNT_TOKEN(); return TYPE; }
"unsafe"                        { COUNT_TOKEN(); return UNSAFE; }
"use"                           { COUNT_TOKEN(); return USE; }
"where"                         { COUNT_TOKEN(); return WHERE; }
"while"                         { COUNT_TOKEN(); return WHILE; }
"abstract"                      { COUNT_TOKEN(); return ABSTRACT; }
"become"                        { COUNT_TOKEN(); return BECOME; }
"box"                           { COUNT_TOKEN(); return BOX; }
"do"                            { COUNT_TOKEN(); return DO; }
"final"                         { COUNT_TOKEN(); return FINAL; }
"macro"                         { COUNT_TOKEN(); return MACRO; }
"override"                      { COUNT_TOKEN(); return OVERRIDE; }
"priv"                          { COUNT_TOKEN(); return PRIV; }
"typeof"                        { COUNT_TOKEN(); return TYPEOF; }
"unsized"                       { COUNT_TOKEN(); return UNSIZED; }
"virtual"                       { COUNT_TOKEN(); return VIRTUAL; }
"yield"                         { COUNT_TOKEN(); return YIELD; }
"async"                         { COUNT_TOKEN(); return ASYNC; }
"await"                         { COUNT_TOKEN(); return AWAIT; }
"dyn"                           { COUNT_TOKEN(); return DYN; }
"union"                         { COUNT_TOKEN(); return UNION; }

/* ============================================================================
 * CONTEXTUAL KEYWORDS
 * ============================================================================
 */

"auto"                          { COUNT_TOKEN(); return AUTO; }
"default"                       { COUNT_TOKEN(); return DEFAULT; }
"existential"                   { COUNT_TOKEN(); return EXISTENTIAL; }
"macro_rules"                   { COUNT_TOKEN(); return MACRO_RULES; }
"raw"                           { COUNT_TOKEN(); return RAW; }
"union"                         { COUNT_TOKEN(); return UNION; }
"'static"                       { COUNT_TOKEN(); return STATIC_LIFETIME; }

/* ============================================================================
 * NUMERIC LITERAL ENHANCEMENTS
 * ============================================================================
 */

/* Complex numbers */
{DECIMAL_FLOAT}[ij]             {
                                    COUNT_TOKEN();
                                    yylval.float_val = atof(yytext);
                                    return COMPLEX_NUMBER;
                                }

/* Scientific notation with underscores */
{DIGIT_SEQ}\.{DIGIT_SEQ}[eE][+-]?{DIGIT_SEQ} {
                                    COUNT_TOKEN();
                                    yylval.float_val = atof(yytext);
                                    return FLOAT;
                                }

/* Rational numbers */
{DEC_INTEGER}\/{DEC_INTEGER}    {
                                    COUNT_TOKEN();
                                    /* Parse as rational number */
                                    char *slash = strchr(yytext, '/');
                                    *slash = '\0';
                                    yylval.rational.numerator = atoll(yytext);
                                    yylval.rational.denominator = atoll(slash + 1);
                                    return RATIONAL_NUMBER;
                                }

/* ============================================================================
 * ADVANCED STRING FEATURES
 * ============================================================================
 */

/* Format strings */
f\"([^"\\]|\\.)*\"              {
                                    COUNT_TOKEN();
                                    yylval.str_val = strndup(yytext + 2, yyleng - 3);
                                    return FORMAT_STRING;
                                }

/* Multiline strings */
\"\"\"                          {
                                    COUNT_TOKEN();
                                    string_buf_ptr = string_buf;
                                    BEGIN(MULTILINE_STRING_STATE);
                                }

<MULTILINE_STRING_STATE>{
    \"\"\"                      {
                                    BEGIN(INITIAL);
                                    *string_buf_ptr = '\0';
                                    yylval.str_val = strdup(string_buf);
                                    return MULTILINE_STRING;
                                }
    [^"]+                       {
                                    char *yptr = yytext;
                                    while (*yptr) {
                                        if (string_buf_ptr - string_buf >= MAX_STR_CONST - 1) {
                                            lexical_error("Multiline string too long");
                                            return ERROR;
                                        }
                                        *string_buf_ptr++ = *yptr++;
                                    }
                                }
    \"                          {
                                    if (string_buf_ptr - string_buf >= MAX_STR_CONST - 1) {
                                        lexical_error("Multiline string too long");
                                        return ERROR;
                                    }
                                    *string_buf_ptr++ = '"';
                                }
    <<EOF>>                     {
                                    lexical_error("Unterminated multiline string");
                                    return ERROR;
                                }
}

/* ============================================================================
 * REGEX LITERALS
 * ============================================================================
 */

\/([^\/\\\n]|\\.)+\/[gimsuvy]*  {
                                    COUNT_TOKEN();
                                    /* Extract regex pattern and flags */
                                    char *pattern_start = yytext + 1;
                                    char *pattern_end = strrchr(yytext, '/');
                                    yylval.regex.pattern = strndup(pattern_start, pattern_end - pattern_start);
                                    yylval.regex.flags = strdup(pattern_end + 1);
                                    return REGEX_LITERAL;
                                }

/* ============================================================================
 * ATTRIBUTE SYNTAX
 * ============================================================================
 */

#\[                             { COUNT_TOKEN(); return ATTR_START; }
#!\[                            { COUNT_TOKEN(); return INNER_ATTR_START; }

/* ============================================================================
 * MACRO INVOCATION SYNTAX
 * ============================================================================
 */

{IDENTIFIER}!                   {
                                    COUNT_TOKEN();
                                    yylval.str_val = strndup(yytext, yyleng - 1);
                                    return MACRO_INVOCATION;
                                }

/* ============================================================================
 * LABEL SYNTAX
 * ============================================================================
 */

'{IDENTIFIER}:                  {
                                    COUNT_TOKEN();
                                    yylval.str_val = strndup(yytext + 1, yyleng - 2);
                                    return LABEL;
                                }

/* ============================================================================
 * ADVANCED COMMENT FEATURES
 * ============================================================================
 */

/* Nested block comments */
"/*"                            {
                                    COUNT_TOKEN();
                                    int depth = 1;
                                    int c;
                                    while (depth > 0 && (c = input()) != EOF) {
                                        if (c == '/' && (c = input()) == '*') {
                                            depth++;
                                        } else if (c == '*' && (c = input()) == '/') {
                                            depth--;
                                        } else if (c == '\n') {
                                            line_num++;
                                            col_num = 1;
                                        }
                                    }
                                    if (depth > 0) {
                                        lexical_error("Unterminated nested block comment");
                                        return ERROR;
                                    }
                                }

/* Module-level documentation comments */
//!.*                            {
                                    COUNT_TOKEN();
                                    yylval.str_val = strdup(yytext + 3);
                                    return MODULE_DOC_COMMENT;
                                }

/*!.*?\*/                        {
                                    COUNT_TOKEN();
                                    yylval.str_val = strndup(yytext + 3, yyleng - 5);
                                    return MODULE_DOC_BLOCK_COMMENT;
                                }

/* ============================================================================
 * TOKEN BUFFER MANAGEMENT
 * ============================================================================
 */

/* Token buffer for lookahead */
#define MAX_TOKEN_BUFFER 16
static struct {
    int type;
    YYSTYPE value;
    YYLTYPE location;
} token_buffer[MAX_TOKEN_BUFFER];
static int token_buffer_head = 0;
static int token_buffer_tail = 0;

int push_token(int type, YYSTYPE value, YYLTYPE location) {
    if ((token_buffer_tail + 1) % MAX_TOKEN_BUFFER == token_buffer_head) {
        return -1; /* Buffer full */
    }
    token_buffer[token_buffer_tail].type = type;
    token_buffer[token_buffer_tail].value = value;
    token_buffer[token_buffer_tail].location = location;
    token_buffer_tail = (token_buffer_tail + 1) % MAX_TOKEN_BUFFER;
    return 0;
}

int pop_token(int *type, YYSTYPE *value, YYLTYPE *location) {
    if (token_buffer_head == token_buffer_tail) {
        return -1; /* Buffer empty */
    }
    *type = token_buffer[token_buffer_head].type;
    *value = token_buffer[token_buffer_head].value;
    *location = token_buffer[token_buffer_head].location;
    token_buffer_head = (token_buffer_head + 1) % MAX_TOKEN_BUFFER;
    return 0;
}

/* ============================================================================
 * LEXER STATE MANAGEMENT
 * ============================================================================
 */

typedef struct LexerState {
    int line;
    int column;
    int prev_column;
    unsigned long tokens;
    unsigned long lines;
    YY_BUFFER_STATE buffer;
} LexerState;

LexerState* save_lexer_state() {
    LexerState *state = (LexerState*)malloc(sizeof(LexerState));
    state->line = line_num;
    state->column = col_num;
    state->prev_column = prev_col_num;
    state->tokens = token_count;
    state->lines = line_count;
    state->buffer = YY_CURRENT_BUFFER;
    return state;
}

void restore_lexer_state(LexerState *state) {
    if (!state) return;
    line_num = state->line;
    col_num = state->column;
    prev_col_num = state->prev_column;
    token_count = state->tokens;
    line_count = state->lines;
    yy_switch_to_buffer(state->buffer);
    free(state);
}

/* ============================================================================
 * TOKEN CLASSIFICATION HELPERS
 * ============================================================================
 */

int is_keyword(const char *str) {
    static const char *keywords[] = {
        "if", "else", "elif", "match", "case", "for", "while", "loop",
        "break", "continue", "return", "let", "const", "var", "fn",
        "class", "struct", "enum", "trait", "impl", "type", "pub",
        "priv", "static", "mut", "ref", "async", "await", "yield",
        "self", "Self", "super", "where", "move", "copy", "unsafe",
        "extern", "dyn", "macro", "use", "as", "in", "is", NULL
    };
    
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int is_type_keyword(const char *str) {
    static const char *type_keywords[] = {
        "int", "i8", "i16", "i32", "i64", "i128",
        "uint", "u8", "u16", "u32", "u64", "u128",
        "float", "f32", "f64", "double", "bool",
        "string", "str", "char", "byte", "void",
        "any", "never", "isize", "usize", NULL
    };
    
    for (int i = 0; type_keywords[i] != NULL; i++) {
        if (strcmp(str, type_keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int is_operator(const char *str) {
    static const char *operators[] = {
        "+", "-", "*", "/", "%", "**", "++", "--",
        "==", "!=", "<", ">", "<=", ">=", "===", "!==",
        "&&", "||", "!", "&", "|", "^", "~", "<<", ">>",
        "=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=",
        "->", "=>", "..", "..=", "...", "?.", "??", "?",
        "::", "@", "#", "$", NULL
    };
    
    for (int i = 0; operators[i] != NULL; i++) {
        if (strcmp(str, operators[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/* ============================================================================
 * DIAGNOSTIC HELPERS
 * ============================================================================
 */

typedef struct Diagnostic {
    enum { DIAG_ERROR, DIAG_WARNING, DIAG_NOTE } severity;
    int line;
    int column;
    char *message;
    struct Diagnostic *next;
} Diagnostic;

static Diagnostic *diagnostic_list = NULL;

void add_diagnostic(int severity, int line, int col, const char *msg) {
    Diagnostic *diag = (Diagnostic*)malloc(sizeof(Diagnostic));
    diag->severity = severity;
    diag->line = line;
    diag->column = col;
    diag->message = strdup(msg);
    diag->next = diagnostic_list;
    diagnostic_list = diag;
}

void print_diagnostics() {
    Diagnostic *diag = diagnostic_list;
    while (diag) {
        const char *severity_str;
        const char *color;
        
        switch (diag->severity) {
            case DIAG_ERROR:
                severity_str = "Error";
                color = "\033[1;31m";
                break;
            case DIAG_WARNING:
                severity_str = "Warning";
                color = "\033[1;33m";
                break;
            case DIAG_NOTE:
                severity_str = "Note";
                color = "\033[1;36m";
                break;
            default:
                severity_str = "Unknown";
                color = "\033[0m";
        }
        
        fprintf(stderr, "%s%s\033[0m at line %d, column %d: %s\n",
                color, severity_str, diag->line, diag->column, diag->message);
        
        Diagnostic *next = diag->next;
        free(diag->message);
        free(diag);
        diag = next;
    }
    diagnostic_list = NULL;
}

/* ============================================================================
 * SOURCE LOCATION TRACKING
 * ============================================================================
 */

typedef struct SourceLocation {
    const char *filename;
    int line;
    int column;
    int offset;
} SourceLocation;

static SourceLocation current_location = {NULL, 1, 1, 0};

void set_source_file(const char *filename) {
    current_location.filename = filename;
    current_location.line = 1;
    current_location.column = 1;
    current_location.offset = 0;
}

SourceLocation get_current_location() {
    current_location.line = line_num;
    current_location.column = col_num;
    return current_location;
}

/* ============================================================================
 * LEXER PERFORMANCE PROFILING
 * ============================================================================
 */

#include <time.h>

static struct {
    clock_t start_time;
    clock_t end_time;
    unsigned long tokens_per_second;
} lexer_profile;

void start_lexer_profiling() {
    lexer_profile.start_time = clock();
}

void end_lexer_profiling() {
    lexer_profile.end_time = clock();
    double elapsed = (double)(lexer_profile.end_time - lexer_profile.start_time) / CLOCKS_PER_SEC;
    if (elapsed > 0) {
        lexer_profile.tokens_per_second = (unsigned long)(token_count / elapsed);
    }
}

void print_lexer_profile() {
    double elapsed = (double)(lexer_profile.end_time - lexer_profile.start_time) / CLOCKS_PER_SEC;
    printf("\n=== Lexer Performance Profile ===\n");
    printf("Elapsed time: %.4f seconds\n", elapsed);
    printf("Tokens processed: %lu\n", token_count);
    printf("Tokens per second: %lu\n", lexer_profile.tokens_per_second);
    printf("Lines processed: %lu\n", line_count);
    printf("=================================\n");
}

/* ============================================================================
 * MEMORY MANAGEMENT FOR LEXER
 * ============================================================================
 */

typedef struct StringPool {
    char **strings;
    size_t count;
    size_t capacity;
} StringPool;

static StringPool string_pool = {NULL, 0, 0};

void init_string_pool() {
    string_pool.capacity = 1024;
    string_pool.strings = (char**)malloc(sizeof(char*) * string_pool.capacity);
    string_pool.count = 0;
}

char* pool_string(const char *str) {
    if (string_pool.count >= string_pool.capacity) {
        string_pool.capacity *= 2;
        string_pool.strings = (char**)realloc(string_pool.strings, 
                                               sizeof(char*) * string_pool.capacity);
    }
    char *pooled = strdup(str);
    string_pool.strings[string_pool.count++] = pooled;
    return pooled;
}

void free_string_pool() {
    for (size_t i = 0; i < string_pool.count; i++) {
        free(string_pool.strings[i]);
    }
    free(string_pool.strings);
    string_pool.strings = NULL;
    string_pool.count = 0;
    string_pool.capacity = 0;
}

/* ============================================================================
 * END OF LEXER
 * ============================================================================
 */


/* ============================================================================
 * CUSTOM SHORT-FORM KEYWORDS - L# CREATIVE SYNTAX
 * ============================================================================
 */

/* Type shortcuts */
"type"                          { COUNT_TOKEN(); return TYPE_KW; }
"tp"                            { COUNT_TOKEN(); return TYPE_KW; }
"t"                             { COUNT_TOKEN(); return TYPE_KW; }

/* Link/Import shortcuts */
"link"                          { COUNT_TOKEN(); return LINK; }
"lnk"                           { COUNT_TOKEN(); return LINK; }
"ln"                            { COUNT_TOKEN(); return LINK; }

/* Find/Search shortcuts */
"find"                          { COUNT_TOKEN(); return FIND; }
"fnd"                           { COUNT_TOKEN(); return FIND; }
"fd"                            { COUNT_TOKEN(); return FIND; }
"search"                        { COUNT_TOKEN(); return SEARCH; }
"src"                           { COUNT_TOKEN(); return SEARCH; }

/* Definition shortcuts */
"def"                           { COUNT_TOKEN(); return DEF; }
"define"                        { COUNT_TOKEN(); return DEF; }
"df"                            { COUNT_TOKEN(); return DEF; }

/* Information shortcuts */
"info"                          { COUNT_TOKEN(); return INFO; }
"inf"                           { COUNT_TOKEN(); return INFO; }
"i"                             { COUNT_TOKEN(); return INFO; }

/* Print/Output shortcuts */
"print"                         { COUNT_TOKEN(); return PRINT; }
"prnt"                          { COUNT_TOKEN(); return PRINT; }
"pr"                            { COUNT_TOKEN(); return PRINT; }
"out"                           { COUNT_TOKEN(); return OUT; }
"output"                        { COUNT_TOKEN(); return OUT; }
"echo"                          { COUNT_TOKEN(); return ECHO; }
"say"                           { COUNT_TOKEN(); return SAY; }

/* String shortcuts */
"str"                           { COUNT_TOKEN(); return STR_KW; }
"string"                        { COUNT_TOKEN(); return STR_KW; }
"s"                             { COUNT_TOKEN(); return STR_KW; }
"text"                          { COUNT_TOKEN(); return TEXT; }
"txt"                           { COUNT_TOKEN(); return TEXT; }

/* Variable shortcuts */
"var"                           { COUNT_TOKEN(); return VAR; }
"v"                             { COUNT_TOKEN(); return VAR; }
"variable"                      { COUNT_TOKEN(); return VAR; }

/* List shortcuts */
"ls"                            { COUNT_TOKEN(); return LIST; }
"list"                          { COUNT_TOKEN(); return LIST; }
"lst"                           { COUNT_TOKEN(); return LIST; }
"arr"                           { COUNT_TOKEN(); return ARRAY_KW; }
"array"                         { COUNT_TOKEN(); return ARRAY_KW; }

/* Map/Dictionary shortcuts */
"mt"                            { COUNT_TOKEN(); return MAP; }
"map"                           { COUNT_TOKEN(); return MAP; }
"dict"                          { COUNT_TOKEN(); return DICT; }
"dct"                           { COUNT_TOKEN(); return DICT; }
"hash"                          { COUNT_TOKEN(); return HASH_MAP; }

/* Function shortcuts */
"func"                          { COUNT_TOKEN(); return FUNC; }
"fn"                            { COUNT_TOKEN(); return FN; }
"f"                             { COUNT_TOKEN(); return FN; }
"lambda"                        { COUNT_TOKEN(); return LAMBDA_KW; }
"lam"                           { COUNT_TOKEN(); return LAMBDA_KW; }
"λ"                             { COUNT_TOKEN(); return LAMBDA_KW; }

/* Class shortcuts */
"cls"                           { COUNT_TOKEN(); return CLASS; }
"class"                         { COUNT_TOKEN(); return CLASS; }
"cl"                            { COUNT_TOKEN(); return CLASS; }

/* Object shortcuts */
"obj"                           { COUNT_TOKEN(); return OBJECT; }
"object"                        { COUNT_TOKEN(); return OBJECT; }
"ob"                            { COUNT_TOKEN(); return OBJECT; }

/* Module shortcuts */
"mod"                           { COUNT_TOKEN(); return MODULE; }
"module"                        { COUNT_TOKEN(); return MODULE; }
"md"                            { COUNT_TOKEN(); return MODULE; }

/* Namespace shortcuts */
"ns"                            { COUNT_TOKEN(); return NAMESPACE; }
"namespace"                     { COUNT_TOKEN(); return NAMESPACE; }

/* Interface shortcuts */
"iface"                         { COUNT_TOKEN(); return INTERFACE; }
"interface"                     { COUNT_TOKEN(); return INTERFACE; }
"if"                            { COUNT_TOKEN(); return INTERFACE; }

/* Trait shortcuts */
"tr"                            { COUNT_TOKEN(); return TRAIT; }
"trait"                         { COUNT_TOKEN(); return TRAIT; }

/* Struct shortcuts */
"st"                            { COUNT_TOKEN(); return STRUCT; }
"struct"                        { COUNT_TOKEN(); return STRUCT; }
"strc"                          { COUNT_TOKEN(); return STRUCT; }

/* Enum shortcuts */
"en"                            { COUNT_TOKEN(); return ENUM; }
"enum"                          { COUNT_TOKEN(); return ENUM; }

/* Return shortcuts */
"ret"                           { COUNT_TOKEN(); return RETURN; }
"return"                        { COUNT_TOKEN(); return RETURN; }
"r"                             { COUNT_TOKEN(); return RETURN; }

/* Break shortcuts */
"brk"                           { COUNT_TOKEN(); return BREAK; }
"break"                         { COUNT_TOKEN(); return BREAK; }
"br"                            { COUNT_TOKEN(); return BREAK; }

/* Continue shortcuts */
"cont"                          { COUNT_TOKEN(); return CONTINUE; }
"continue"                      { COUNT_TOKEN(); return CONTINUE; }
"cn"                            { COUNT_TOKEN(); return CONTINUE; }

/* Loop shortcuts */
"lp"                            { COUNT_TOKEN(); return LOOP; }
"loop"                          { COUNT_TOKEN(); return LOOP; }

/* While shortcuts */
"wh"                            { COUNT_TOKEN(); return WHILE; }
"while"                         { COUNT_TOKEN(); return WHILE; }

/* For shortcuts */
"fr"                            { COUNT_TOKEN(); return FOR; }
"for"                           { COUNT_TOKEN(); return FOR; }

/* Match shortcuts */
"mtch"                          { COUNT_TOKEN(); return MATCH; }
"match"                         { COUNT_TOKEN(); return MATCH; }
"mc"                            { COUNT_TOKEN(); return MATCH; }

/* Case shortcuts */
"cs"                            { COUNT_TOKEN(); return CASE; }
"case"                          { COUNT_TOKEN(); return CASE; }

/* If shortcuts */
"if"                            { COUNT_TOKEN(); return IF; }

/* Else shortcuts */
"el"                            { COUNT_TOKEN(); return ELSE; }
"else"                          { COUNT_TOKEN(); return ELSE; }

/* Try shortcuts */
"try"                           { COUNT_TOKEN(); return TRY; }
"tr"                            { COUNT_TOKEN(); return TRY; }

/* Catch shortcuts */
"catch"                         { COUNT_TOKEN(); return CATCH; }
"ctch"                          { COUNT_TOKEN(); return CATCH; }
"ct"                            { COUNT_TOKEN(); return CATCH; }

/* Throw shortcuts */
"throw"                         { COUNT_TOKEN(); return THROW; }
"thrw"                          { COUNT_TOKEN(); return THROW; }
"th"                            { COUNT_TOKEN(); return THROW; }

/* Import shortcuts */
"imp"                           { COUNT_TOKEN(); return IMPORT; }
"import"                        { COUNT_TOKEN(); return IMPORT; }
"im"                            { COUNT_TOKEN(); return IMPORT; }

/* Export shortcuts */
"exp"                           { COUNT_TOKEN(); return EXPORT; }
"export"                        { COUNT_TOKEN(); return EXPORT; }
"ex"                            { COUNT_TOKEN(); return EXPORT; }

/* Public shortcuts */
"pub"                           { COUNT_TOKEN(); return PUBLIC; }
"public"                        { COUNT_TOKEN(); return PUBLIC; }
"pb"                            { COUNT_TOKEN(); return PUBLIC; }

/* Private shortcuts */
"priv"                          { COUNT_TOKEN(); return PRIVATE; }
"private"                       { COUNT_TOKEN(); return PRIVATE; }
"pv"                            { COUNT_TOKEN(); return PRIVATE; }

/* Protected shortcuts */
"prot"                          { COUNT_TOKEN(); return PROTECTED; }
"protected"                     { COUNT_TOKEN(); return PROTECTED; }
"pt"                            { COUNT_TOKEN(); return PROTECTED; }

/* Static shortcuts */
"stat"                          { COUNT_TOKEN(); return STATIC; }
"static"                        { COUNT_TOKEN(); return STATIC; }
"stc"                           { COUNT_TOKEN(); return STATIC; }

/* Const shortcuts */
"const"                         { COUNT_TOKEN(); return CONST; }
"cnst"                          { COUNT_TOKEN(); return CONST; }
"c"                             { COUNT_TOKEN(); return CONST; }

/* Mutable shortcuts */
"mut"                           { COUNT_TOKEN(); return MUT; }
"mutable"                       { COUNT_TOKEN(); return MUT; }
"m"                             { COUNT_TOKEN(); return MUT; }

/* Reference shortcuts */
"ref"                           { COUNT_TOKEN(); return REF; }
"reference"                     { COUNT_TOKEN(); return REF; }
"rf"                            { COUNT_TOKEN(); return REF; }

/* Pointer shortcuts */
"ptr"                           { COUNT_TOKEN(); return PTR; }
"pointer"                       { COUNT_TOKEN(); return PTR; }
"p"                             { COUNT_TOKEN(); return PTR; }

/* Async shortcuts */
"async"                         { COUNT_TOKEN(); return ASYNC; }
"asn"                           { COUNT_TOKEN(); return ASYNC; }
"as"                            { COUNT_TOKEN(); return ASYNC; }

/* Await shortcuts */
"await"                         { COUNT_TOKEN(); return AWAIT; }
"awt"                           { COUNT_TOKEN(); return AWAIT; }
"aw"                            { COUNT_TOKEN(); return AWAIT; }

/* Yield shortcuts */
"yield"                         { COUNT_TOKEN(); return YIELD; }
"yld"                           { COUNT_TOKEN(); return YIELD; }
"y"                             { COUNT_TOKEN(); return YIELD; }

/* New shortcuts */
"new"                           { COUNT_TOKEN(); return NEW; }
"nw"                            { COUNT_TOKEN(); return NEW; }

/* Delete shortcuts */
"del"                           { COUNT_TOKEN(); return DELETE; }
"delete"                        { COUNT_TOKEN(); return DELETE; }
"dl"                            { COUNT_TOKEN(); return DELETE; }

/* This shortcuts */
"this"                          { COUNT_TOKEN(); return THIS; }
"ths"                           { COUNT_TOKEN(); return THIS; }

/* Self shortcuts */
"self"                          { COUNT_TOKEN(); return SELF; }
"slf"                           { COUNT_TOKEN(); return SELF; }

/* Super shortcuts */
"super"                         { COUNT_TOKEN(); return SUPER; }
"spr"                           { COUNT_TOKEN(); return SUPER; }
"sp"                            { COUNT_TOKEN(); return SUPER; }

/* True shortcuts */
"true"                          { COUNT_TOKEN(); return TRUE; }
"tr"                            { COUNT_TOKEN(); return TRUE; }
"t"                             { COUNT_TOKEN(); return TRUE; }

/* False shortcuts */
"false"                         { COUNT_TOKEN(); return FALSE; }
"fls"                           { COUNT_TOKEN(); return FALSE; }
"f"                             { COUNT_TOKEN(); return FALSE; }

/* Null shortcuts */
"null"                          { COUNT_TOKEN(); return NULL_LIT; }
"nil"                           { COUNT_TOKEN(); return NULL_LIT; }
"none"                          { COUNT_TOKEN(); return NULL_LIT; }
"n"                             { COUNT_TOKEN(); return NULL_LIT; }

/* Boolean shortcuts */
"bool"                          { COUNT_TOKEN(); return BOOL_TYPE; }
"boolean"                       { COUNT_TOKEN(); return BOOL_TYPE; }
"bl"                            { COUNT_TOKEN(); return BOOL_TYPE; }
"b"                             { COUNT_TOKEN(); return BOOL_TYPE; }

/* Integer shortcuts */
"int"                           { COUNT_TOKEN(); return INT_TYPE; }
"integer"                       { COUNT_TOKEN(); return INT_TYPE; }
"i"                             { COUNT_TOKEN(); return INT_TYPE; }
"num"                           { COUNT_TOKEN(); return INT_TYPE; }

/* Float shortcuts */
"float"                         { COUNT_TOKEN(); return FLOAT_TYPE; }
"flt"                           { COUNT_TOKEN(); return FLOAT_TYPE; }
"fl"                            { COUNT_TOKEN(); return FLOAT_TYPE; }
"real"                          { COUNT_TOKEN(); return FLOAT_TYPE; }

/* Double shortcuts */
"double"                        { COUNT_TOKEN(); return DOUBLE_TYPE; }
"dbl"                           { COUNT_TOKEN(); return DOUBLE_TYPE; }
"db"                            { COUNT_TOKEN(); return DOUBLE_TYPE; }

/* Char shortcuts */
"char"                          { COUNT_TOKEN(); return CHAR_TYPE; }
"chr"                           { COUNT_TOKEN(); return CHAR_TYPE; }
"ch"                            { COUNT_TOKEN(); return CHAR_TYPE; }

/* Byte shortcuts */
"byte"                          { COUNT_TOKEN(); return BYTE_TYPE; }
"byt"                           { COUNT_TOKEN(); return BYTE_TYPE; }
"by"                            { COUNT_TOKEN(); return BYTE_TYPE; }

/* Void shortcuts */
"void"                          { COUNT_TOKEN(); return VOID_TYPE; }
"vd"                            { COUNT_TOKEN(); return VOID_TYPE; }

/* Any shortcuts */
"any"                           { COUNT_TOKEN(); return ANY_TYPE; }
"an"                            { COUNT_TOKEN(); return ANY_TYPE; }

/* Where shortcuts */
"where"                         { COUNT_TOKEN(); return WHERE; }
"whr"                           { COUNT_TOKEN(); return WHERE; }
"wh"                            { COUNT_TOKEN(); return WHERE; }

/* Impl shortcuts */
"impl"                          { COUNT_TOKEN(); return IMPL; }
"implement"                     { COUNT_TOKEN(); return IMPL; }
"imp"                           { COUNT_TOKEN(); return IMPL; }

/* Extern shortcuts */
"extern"                        { COUNT_TOKEN(); return EXTERN; }
"ext"                           { COUNT_TOKEN(); return EXTERN; }
"ex"                            { COUNT_TOKEN(); return EXTERN; }

/* Unsafe shortcuts */
"unsafe"                        { COUNT_TOKEN(); return UNSAFE; }
"unsf"                          { COUNT_TOKEN(); return UNSAFE; }
"us"                            { COUNT_TOKEN(); return UNSAFE; }

/* Move shortcuts */
"move"                          { COUNT_TOKEN(); return MOVE; }
"mv"                            { COUNT_TOKEN(); return MOVE; }

/* Copy shortcuts */
"copy"                          { COUNT_TOKEN(); return COPY; }
"cp"                            { COUNT_TOKEN(); return COPY; }

/* Clone shortcuts */
"clone"                         { COUNT_TOKEN(); return CLONE; }
"cln"                           { COUNT_TOKEN(); return CLONE; }
"cl"                            { COUNT_TOKEN(); return CLONE; }

/* Drop shortcuts */
"drop"                          { COUNT_TOKEN(); return DROP; }
"drp"                           { COUNT_TOKEN(); return DROP; }
"dp"                            { COUNT_TOKEN(); return DROP; }

/* Box shortcuts */
"box"                           { COUNT_TOKEN(); return BOX; }
"bx"                            { COUNT_TOKEN(); return BOX; }

/* Macro shortcuts */
"macro"                         { COUNT_TOKEN(); return MACRO; }
"mcr"                           { COUNT_TOKEN(); return MACRO; }
"mc"                            { COUNT_TOKEN(); return MACRO; }

/* Derive shortcuts */
"derive"                        { COUNT_TOKEN(); return DERIVE; }
"drv"                           { COUNT_TOKEN(); return DERIVE; }
"dv"                            { COUNT_TOKEN(); return DERIVE; }

/* Assert shortcuts */
"assert"                        { COUNT_TOKEN(); return ASSERT; }
"asrt"                          { COUNT_TOKEN(); return ASSERT; }
"ast"                           { COUNT_TOKEN(); return ASSERT; }

/* Panic shortcuts */
"panic"                         { COUNT_TOKEN(); return PANIC; }
"pnc"                           { COUNT_TOKEN(); return PANIC; }
"pn"                            { COUNT_TOKEN(); return PANIC; }

/* Test shortcuts */
"test"                          { COUNT_TOKEN(); return TEST; }
"tst"                           { COUNT_TOKEN(); return TEST; }
"ts"                            { COUNT_TOKEN(); return TEST; }

/* Benchmark shortcuts */
"bench"                         { COUNT_TOKEN(); return BENCH; }
"benchmark"                     { COUNT_TOKEN(); return BENCH; }
"bnch"                          { COUNT_TOKEN(); return BENCH; }
"bm"                            { COUNT_TOKEN(); return BENCH; }

/* Debug shortcuts */
"debug"                         { COUNT_TOKEN(); return DEBUG; }
"dbg"                           { COUNT_TOKEN(); return DEBUG; }
"dg"                            { COUNT_TOKEN(); return DEBUG; }

/* Trace shortcuts */
"trace"                         { COUNT_TOKEN(); return TRACE; }
"trc"                           { COUNT_TOKEN(); return TRACE; }
"tc"                            { COUNT_TOKEN(); return TRACE; }

/* Log shortcuts */
"log"                           { COUNT_TOKEN(); return LOG; }
"lg"                            { COUNT_TOKEN(); return LOG; }

/* Error shortcuts */
"error"                         { COUNT_TOKEN(); return ERROR_KW; }
"err"                           { COUNT_TOKEN(); return ERROR_KW; }
"er"                            { COUNT_TOKEN(); return ERROR_KW; }

/* Warning shortcuts */
"warn"                          { COUNT_TOKEN(); return WARN; }
"warning"                       { COUNT_TOKEN(); return WARN; }
"wrn"                           { COUNT_TOKEN(); return WARN; }

/* Success shortcuts */
"success"                       { COUNT_TOKEN(); return SUCCESS; }
"succ"                          { COUNT_TOKEN(); return SUCCESS; }
"ok"                            { COUNT_TOKEN(); return SUCCESS; }

/* Fail shortcuts */
"fail"                          { COUNT_TOKEN(); return FAIL; }
"failure"                       { COUNT_TOKEN(); return FAIL; }
"fl"                            { COUNT_TOKEN(); return FAIL; }

/* Result shortcuts */
"result"                        { COUNT_TOKEN(); return RESULT; }
"res"                           { COUNT_TOKEN(); return RESULT; }
"rs"                            { COUNT_TOKEN(); return RESULT; }

/* Option shortcuts */
"option"                        { COUNT_TOKEN(); return OPTION; }
"opt"                           { COUNT_TOKEN(); return OPTION; }
"op"                            { COUNT_TOKEN(); return OPTION; }

/* Some shortcuts */
"some"                          { COUNT_TOKEN(); return SOME; }
"sm"                            { COUNT_TOKEN(); return SOME; }

/* Range shortcuts */
"range"                         { COUNT_TOKEN(); return RANGE_KW; }
"rng"                           { COUNT_TOKEN(); return RANGE_KW; }
"rg"                            { COUNT_TOKEN(); return RANGE_KW; }

/* Iterator shortcuts */
"iter"                          { COUNT_TOKEN(); return ITER; }
"iterator"                      { COUNT_TOKEN(); return ITER; }
"it"                            { COUNT_TOKEN(); return ITER; }

/* Collection shortcuts */
"collect"                       { COUNT_TOKEN(); return COLLECT; }
"coll"                          { COUNT_TOKEN(); return COLLECT; }
"col"                           { COUNT_TOKEN(); return COLLECT; }

/* Filter shortcuts */
"filter"                        { COUNT_TOKEN(); return FILTER; }
"flt"                           { COUNT_TOKEN(); return FILTER; }
"ftr"                           { COUNT_TOKEN(); return FILTER; }

/* Reduce shortcuts */
"reduce"                        { COUNT_TOKEN(); return REDUCE; }
"red"                           { COUNT_TOKEN(); return REDUCE; }
"rd"                            { COUNT_TOKEN(); return REDUCE; }

/* Fold shortcuts */
"fold"                          { COUNT_TOKEN(); return FOLD; }
"fld"                           { COUNT_TOKEN(); return FOLD; }

/* Zip shortcuts */
"zip"                           { COUNT_TOKEN(); return ZIP; }
"zp"                            { COUNT_TOKEN(); return ZIP; }

/* Chain shortcuts */
"chain"                         { COUNT_TOKEN(); return CHAIN; }
"chn"                           { COUNT_TOKEN(); return CHAIN; }
"ch"                            { COUNT_TOKEN(); return CHAIN; }

/* Take shortcuts */
"take"                          { COUNT_TOKEN(); return TAKE; }
"tk"                            { COUNT_TOKEN(); return TAKE; }

/* Skip shortcuts */
"skip"                          { COUNT_TOKEN(); return SKIP; }
"skp"                           { COUNT_TOKEN(); return SKIP; }
"sk"                            { COUNT_TOKEN(); return SKIP; }

/* Count shortcuts */
"count"                         { COUNT_TOKEN(); return COUNT; }
"cnt"                           { COUNT_TOKEN(); return COUNT; }
"ct"                            { COUNT_TOKEN(); return COUNT; }

/* Length shortcuts */
"length"                        { COUNT_TOKEN(); return LENGTH; }
"len"                           { COUNT_TOKEN(); return LENGTH; }
"ln"                            { COUNT_TOKEN(); return LENGTH; }

/* Size shortcuts */
"size"                          { COUNT_TOKEN(); return SIZE; }
"sz"                            { COUNT_TOKEN(); return SIZE; }

/* Empty shortcuts */
"empty"                         { COUNT_TOKEN(); return EMPTY; }
"empt"                          { COUNT_TOKEN(); return EMPTY; }
"emp"                           { COUNT_TOKEN(); return EMPTY; }
"e"                             { COUNT_TOKEN(); return EMPTY; }

/* Push shortcuts */
"push"                          { COUNT_TOKEN(); return PUSH; }
"psh"                           { COUNT_TOKEN(); return PUSH; }
"ps"                            { COUNT_TOKEN(); return PUSH; }

/* Pop shortcuts */
"pop"                           { COUNT_TOKEN(); return POP; }
"pp"                            { COUNT_TOKEN(); return POP; }

/* Insert shortcuts */
"insert"                        { COUNT_TOKEN(); return INSERT; }
"ins"                           { COUNT_TOKEN(); return INSERT; }
"in"                            { COUNT_TOKEN(); return INSERT; }

/* Remove shortcuts */
"remove"                        { COUNT_TOKEN(); return REMOVE; }
"rem"                           { COUNT_TOKEN(); return REMOVE; }
"rm"                            { COUNT_TOKEN(); return REMOVE; }

/* Clear shortcuts */
"clear"                         { COUNT_TOKEN(); return CLEAR; }
"clr"                           { COUNT_TOKEN(); return CLEAR; }
"cl"                            { COUNT_TOKEN(); return CLEAR; }

/* Get shortcuts */
"get"                           { COUNT_TOKEN(); return GET; }
"gt"                            { COUNT_TOKEN(); return GET; }

/* Set shortcuts */
"set"                           { COUNT_TOKEN(); return SET; }
"st"                            { COUNT_TOKEN(); return SET; }

/* Has shortcuts */
"has"                           { COUNT_TOKEN(); return HAS; }
"hs"                            { COUNT_TOKEN(); return HAS; }

/* Contains shortcuts */
"contains"                      { COUNT_TOKEN(); return CONTAINS; }
"cont"                          { COUNT_TOKEN(); return CONTAINS; }
"ctn"                           { COUNT_TOKEN(); return CONTAINS; }

/* Join shortcuts */
"join"                          { COUNT_TOKEN(); return JOIN; }
"jn"                            { COUNT_TOKEN(); return JOIN; }

/* Split shortcuts */
"split"                         { COUNT_TOKEN(); return SPLIT; }
"splt"                          { COUNT_TOKEN(); return SPLIT; }
"spl"                           { COUNT_TOKEN(); return SPLIT; }

/* Trim shortcuts */
"trim"                          { COUNT_TOKEN(); return TRIM; }
"trm"                           { COUNT_TOKEN(); return TRIM; }
"tm"                            { COUNT_TOKEN(); return TRIM; }

/* Replace shortcuts */
"replace"                       { COUNT_TOKEN(); return REPLACE; }
"repl"                          { COUNT_TOKEN(); return REPLACE; }
"rpl"                           { COUNT_TOKEN(); return REPLACE; }

/* Format shortcuts */
"format"                        { COUNT_TOKEN(); return FORMAT; }
"fmt"                           { COUNT_TOKEN(); return FORMAT; }
"fm"                            { COUNT_TOKEN(); return FORMAT; }

/* Parse shortcuts */
"parse"                         { COUNT_TOKEN(); return PARSE; }
"prs"                           { COUNT_TOKEN(); return PARSE; }
"ps"                            { COUNT_TOKEN(); return PARSE; }

/* Convert shortcuts */
"convert"                       { COUNT_TOKEN(); return CONVERT; }
"conv"                          { COUNT_TOKEN(); return CONVERT; }
"cvt"                           { COUNT_TOKEN(); return CONVERT; }

/* Cast shortcuts */
"cast"                          { COUNT_TOKEN(); return CAST; }
"cst"                           { COUNT_TOKEN(); return CAST; }
"cs"                            { COUNT_TOKEN(); return CAST; }

/* ============================================================================
 * END OF CUSTOM SHORT-FORM KEYWORDS
 * ============================================================================
 */
