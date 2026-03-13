/* L# Lexer Grammar - Flex/Lex Format */
/* Lexical analyzer for L# programming language */

%{
#include <stdio.h>
#include <string.h>
#include "parser.tab.h"

int line_num = 1;
int col_num = 1;

#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = line_num; \
    yylloc.first_column = col_num; \
    yylloc.last_column = col_num + yyleng - 1; \
    col_num += yyleng;
%}

%option noyywrap
%option yylineno

/* Definitions */
DIGIT           [0-9]
LETTER          [a-zA-Z_]
HEX             [0-9A-Fa-f]
BINARY          [01]
OCTAL           [0-7]
WHITESPACE      [ \t\r]
NEWLINE         \n

/* Identifiers */
IDENTIFIER      {LETTER}({LETTER}|{DIGIT})*
TYPE_ID         [A-Z]({LETTER}|{DIGIT})*

/* Numbers */
INTEGER         {DIGIT}+
HEX_INT         0[xX]{HEX}+
BIN_INT         0[bB]{BINARY}+
OCT_INT         0[oO]{OCTAL}+
FLOAT           {DIGIT}+\.{DIGIT}+([eE][+-]?{DIGIT}+)?

/* String literals */
STRING_CHAR     [^\\\"\n]
ESCAPE_SEQ      \\[nrt\\\"\']|\\x{HEX}{2}|\\u{HEX}{4}

%%

/* Comments */
"//".*                  { /* Line comment - ignore */ }
"/*"([^*]|\*+[^*/])*\*+"/"  { /* Block comment - ignore */ }

/* Keywords - Control Flow */
"if"                    { return IF; }
"else"                  { return ELSE; }
"elif"                  { return ELIF; }
"match"                 { return MATCH; }
"case"                  { return CASE; }
"for"                   { return FOR; }
"while"                 { return WHILE; }
"loop"                  { return LOOP; }
"break"                 { return BREAK; }
"continue"              { return CONTINUE; }
"return"                { return RETURN; }

/* Keywords - Declarations */
"let"                   { return LET; }
"const"                 { return CONST; }
"var"                   { return VAR; }
"fn"                    { return FN; }
"func"                  { return FUNC; }
"class"                 { return CLASS; }
"struct"                { return STRUCT; }
"enum"                  { return ENUM; }
"interface"             { return INTERFACE; }
"trait"                 { return TRAIT; }
"impl"                  { return IMPL; }
"type"                  { return TYPE; }
"alias"                 { return ALIAS; }

/* Keywords - Modifiers */
"pub"                   { return PUB; }
"priv"                  { return PRIV; }
"protected"             { return PROTECTED; }
"static"                { return STATIC; }
"final"                 { return FINAL; }
"abstract"              { return ABSTRACT; }
"virtual"               { return VIRTUAL; }
"override"              { return OVERRIDE; }
"mut"                   { return MUT; }
"ref"                   { return REF; }

/* Keywords - Other */
"use"                   { return USE; }
"import"                { return IMPORT; }
"export"                { return EXPORT; }
"from"                  { return FROM; }
"as"                    { return AS; }
"in"                    { return IN; }
"is"                    { return IS; }
"new"                   { return NEW; }
"delete"                { return DELETE; }
"sizeof"                { return SIZEOF; }
"typeof"                { return TYPEOF; }
"instanceof"            { return INSTANCEOF; }

/* Keywords - Async */
"async"                 { return ASYNC; }
"await"                 { return AWAIT; }
"yield"                 { return YIELD; }

/* Keywords - Special */
"self"                  { return SELF; }
"this"                  { return THIS; }
"super"                 { return SUPER; }
"base"                  { return BASE; }
"where"                 { return WHERE; }
"move"                  { return MOVE; }
"copy"                  { return COPY; }
"unsafe"                { return UNSAFE; }
"extern"                { return EXTERN; }
"dyn"                   { return DYN; }
"macro"                 { return MACRO; }
"derive"                { return DERIVE; }

/* Primitive Types */
"int"                   { return INT_TYPE; }
"float"                 { return FLOAT_TYPE; }
"double"                { return DOUBLE_TYPE; }
"bool"                  { return BOOL_TYPE; }
"string"                { return STRING_TYPE; }
"char"                  { return CHAR_TYPE; }
"byte"                  { return BYTE_TYPE; }
"void"                  { return VOID_TYPE; }
"any"                   { return ANY_TYPE; }
"never"                 { return NEVER_TYPE; }

/* Boolean Literals */
"true"                  { yylval.bool_val = 1; return TRUE; }
"false"                 { yylval.bool_val = 0; return FALSE; }

/* Null Literals */
"null"                  { return NULL_LIT; }
"nil"                   { return NIL; }
"undefined"             { return UNDEFINED; }

/* Logical Operators (word form) */
"and"                   { return AND; }
"or"                    { return OR; }
"not"                   { return NOT; }
"xor"                   { return XOR; }

/* Operators - Arithmetic */
"+"                     { return PLUS; }
"-"                     { return MINUS; }
"*"                     { return STAR; }
"/"                     { return SLASH; }
"%"                     { return PERCENT; }
"**"                    { return POWER; }

/* Operators - Comparison */
"=="                    { return EQ; }
"!="                    { return NE; }
"<"                     { return LT; }
">"                     { return GT; }
"<="                    { return LE; }
">="                    { return GE; }
"==="                   { return STRICT_EQ; }
"!=="                   { return STRICT_NE; }

/* Operators - Logical */
"&&"                    { return LOGICAL_AND; }
"||"                    { return LOGICAL_OR; }
"!"                     { return LOGICAL_NOT; }

/* Operators - Bitwise */
"&"                     { return AMPERSAND; }
"|"                     { return PIPE; }
"^"                     { return CARET; }
"~"                     { return TILDE; }
"<<"                    { return LSHIFT; }
">>"                    { return RSHIFT; }

/* Operators - Assignment */
"="                     { return ASSIGN; }
"+="                    { return PLUS_ASSIGN; }
"-="                    { return MINUS_ASSIGN; }
"*="                    { return STAR_ASSIGN; }
"/="                    { return SLASH_ASSIGN; }
"%="                    { return PERCENT_ASSIGN; }
"&="                    { return AND_ASSIGN; }
"|="                    { return OR_ASSIGN; }
"^="                    { return XOR_ASSIGN; }
"<<="                   { return LSHIFT_ASSIGN; }
">>="                   { return RSHIFT_ASSIGN; }

/* Operators - Special */
"->"                    { return ARROW; }
"=>"                    { return FAT_ARROW; }
".."                    { return RANGE; }
"..="                   { return RANGE_INCLUSIVE; }
"?."                    { return OPTIONAL_CHAIN; }
"??"                    { return NULL_COALESCE; }
"?"                     { return QUESTION; }
"..."                   { return SPREAD; }
"@"                     { return AT; }
"#"                     { return HASH; }

/* Delimiters */
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"{"                     { return LBRACE; }
"}"                     { return RBRACE; }
"["                     { return LBRACKET; }
"]"                     { return RBRACKET; }
";"                     { return SEMICOLON; }
":"                     { return COLON; }
","                     { return COMMA; }
"."                     { return DOT; }

/* Numbers */
{HEX_INT}               { yylval.int_val = strtol(yytext, NULL, 16); return INTEGER; }
{BIN_INT}               { yylval.int_val = strtol(yytext + 2, NULL, 2); return INTEGER; }
{OCT_INT}               { yylval.int_val = strtol(yytext + 2, NULL, 8); return INTEGER; }
{INTEGER}               { yylval.int_val = atoi(yytext); return INTEGER; }
{FLOAT}                 { yylval.float_val = atof(yytext); return FLOAT; }

/* String Literals */
\"({STRING_CHAR}|{ESCAPE_SEQ})*\"  { 
    yylval.str_val = strndup(yytext + 1, yyleng - 2); 
    return STRING; 
}

'({STRING_CHAR}|{ESCAPE_SEQ})*'    { 
    yylval.str_val = strndup(yytext + 1, yyleng - 2); 
    return STRING; 
}

`([^`]|\\\`)*`          { 
    yylval.str_val = strndup(yytext + 1, yyleng - 2); 
    return TEMPLATE_STRING; 
}

/* Identifiers */
{TYPE_ID}               { yylval.str_val = strdup(yytext); return TYPE_IDENTIFIER; }
{IDENTIFIER}            { yylval.str_val = strdup(yytext); return IDENTIFIER; }

/* Whitespace */
{WHITESPACE}+           { /* Ignore whitespace */ }
{NEWLINE}               { line_num++; col_num = 1; }

/* Error */
.                       { 
    fprintf(stderr, "Lexical error at line %d, column %d: unexpected character '%s'\n", 
            line_num, col_num, yytext); 
    return ERROR;
}

%%

/* Additional C code */
void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d, column %d: %s\n", line_num, col_num, s);
}
