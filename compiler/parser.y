/* L# Parser Grammar - Bison/Yacc Format */
/* Syntax analyzer for L# programming language */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylex();
extern void yyerror(const char *s);
extern int line_num;
extern int col_num;

ASTNode *root = NULL;
%}

%union {
    int int_val;
    double float_val;
    char *str_val;
    int bool_val;
    struct ASTNode *node;
}

/* Tokens */
%token <int_val> INTEGER
%token <float_val> FLOAT
%token <str_val> STRING TEMPLATE_STRING IDENTIFIER TYPE_IDENTIFIER
%token <bool_val> TRUE FALSE

/* Keywords */
%token IF ELSE ELIF MATCH CASE FOR WHILE LOOP BREAK CONTINUE RETURN
%token LET CONST VAR FN FUNC CLASS STRUCT ENUM INTERFACE TRAIT IMPL TYPE ALIAS
%token PUB PRIV PROTECTED STATIC FINAL ABSTRACT VIRTUAL OVERRIDE MUT REF
%token USE IMPORT EXPORT FROM AS IN IS NEW DELETE SIZEOF TYPEOF INSTANCEOF
%token ASYNC AWAIT YIELD
%token SELF THIS SUPER BASE WHERE MOVE COPY UNSAFE EXTERN DYN MACRO DERIVE

/* Types */
%token INT_TYPE FLOAT_TYPE DOUBLE_TYPE BOOL_TYPE STRING_TYPE CHAR_TYPE BYTE_TYPE
%token VOID_TYPE ANY_TYPE NEVER_TYPE

/* Literals */
%token NULL_LIT NIL UNDEFINED

/* Operators */
%token AND OR NOT XOR
%token PLUS MINUS STAR SLASH PERCENT POWER
%token EQ NE LT GT LE GE STRICT_EQ STRICT_NE
%token LOGICAL_AND LOGICAL_OR LOGICAL_NOT
%token AMPERSAND PIPE CARET TILDE LSHIFT RSHIFT
%token ASSIGN PLUS_ASSIGN MINUS_ASSIGN STAR_ASSIGN SLASH_ASSIGN PERCENT_ASSIGN
%token AND_ASSIGN OR_ASSIGN XOR_ASSIGN LSHIFT_ASSIGN RSHIFT_ASSIGN
%token ARROW FAT_ARROW RANGE RANGE_INCLUSIVE OPTIONAL_CHAIN NULL_COALESCE
%token QUESTION SPREAD AT HASH

/* Delimiters */
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token SEMICOLON COLON COMMA DOT

%token ERROR

/* Non-terminals */
%type <node> program statement_list statement
%type <node> expression primary_expression
%type <node> variable_declaration function_declaration class_declaration
%type <node> struct_declaration enum_declaration trait_declaration
%type <node> if_statement while_statement for_statement match_statement
%type <node> block parameter_list parameter type_annotation
%type <node> expression_list argument_list
%type <node> binary_expression unary_expression call_expression
%type <node> member_expression index_expression
%type <node> literal array_literal struct_literal
%type <node> pattern match_arm_list match_arm

/* Operator Precedence (lowest to highest) */
%right ASSIGN PLUS_ASSIGN MINUS_ASSIGN STAR_ASSIGN SLASH_ASSIGN
%right QUESTION
%left LOGICAL_OR OR
%left LOGICAL_AND AND
%left PIPE
%left CARET
%left AMPERSAND
%left EQ NE STRICT_EQ STRICT_NE
%left LT GT LE GE
%left RANGE RANGE_INCLUSIVE
%left LSHIFT RSHIFT
%left PLUS MINUS
%left STAR SLASH PERCENT
%right POWER
%right LOGICAL_NOT NOT TILDE
%left DOT OPTIONAL_CHAIN
%left LPAREN LBRACKET

%start program

%%

/* Program Structure */
program
    : statement_list                { root = $1; $$ = $1; }
    | /* empty */                   { root = NULL; $$ = NULL; }
    ;

statement_list
    : statement                     { $$ = create_statement_list($1); }
    | statement_list statement      { $$ = append_statement($1, $2); }
    ;

statement
    : variable_declaration          { $$ = $1; }
    | function_declaration          { $$ = $1; }
    | class_declaration             { $$ = $1; }
    | struct_declaration            { $$ = $1; }
    | enum_declaration              { $$ = $1; }
    | trait_declaration             { $$ = $1; }
    | if_statement                  { $$ = $1; }
    | while_statement               { $$ = $1; }
    | for_statement                 { $$ = $1; }
    | match_statement               { $$ = $1; }
    | RETURN expression SEMICOLON   { $$ = create_return_stmt($2); }
    | RETURN SEMICOLON              { $$ = create_return_stmt(NULL); }
    | BREAK SEMICOLON               { $$ = create_break_stmt(); }
    | CONTINUE SEMICOLON            { $$ = create_continue_stmt(); }
    | expression SEMICOLON          { $$ = create_expr_stmt($1); }
    | block                         { $$ = $1; }
    | SEMICOLON                     { $$ = NULL; }
    ;

/* Variable Declaration */
variable_declaration
    : LET IDENTIFIER type_annotation ASSIGN expression SEMICOLON
        { $$ = create_var_decl($2, $3, $5, 0); }
    | LET MUT IDENTIFIER type_annotation ASSIGN expression SEMICOLON
        { $$ = create_var_decl($3, $4, $6, 1); }
    | LET IDENTIFIER ASSIGN expression SEMICOLON
        { $$ = create_var_decl($2, NULL, $4, 0); }
    | CONST IDENTIFIER ASSIGN expression SEMICOLON
        { $$ = create_const_decl($2, $4); }
    ;

type_annotation
    : COLON type_annotation         { $$ = $2; }
    | TYPE_IDENTIFIER               { $$ = create_type_node($1); }
    | INT_TYPE                      { $$ = create_primitive_type("int"); }
    | FLOAT_TYPE                    { $$ = create_primitive_type("float"); }
    | BOOL_TYPE                     { $$ = create_primitive_type("bool"); }
    | STRING_TYPE                   { $$ = create_primitive_type("string"); }
    | LBRACKET type_annotation RBRACKET
        { $$ = create_array_type($2); }
    | TYPE_IDENTIFIER LT type_annotation GT
        { $$ = create_generic_type($1, $3); }
    | /* empty */                   { $$ = NULL; }
    ;

/* Function Declaration */
function_declaration
    : FN IDENTIFIER LPAREN parameter_list RPAREN type_annotation block
        { $$ = create_func_decl($2, $4, $6, $7, 0); }
    | ASYNC FN IDENTIFIER LPAREN parameter_list RPAREN type_annotation block
        { $$ = create_func_decl($3, $5, $7, $8, 1); }
    ;

parameter_list
    : parameter                     { $$ = create_param_list($1); }
    | parameter_list COMMA parameter { $$ = append_param($1, $3); }
    | /* empty */                   { $$ = NULL; }
    ;

parameter
    : IDENTIFIER type_annotation    { $$ = create_param($1, $2, NULL); }
    | IDENTIFIER type_annotation ASSIGN expression
        { $$ = create_param($1, $2, $4); }
    ;

/* Class Declaration */
class_declaration
    : CLASS TYPE_IDENTIFIER LBRACE statement_list RBRACE
        { $$ = create_class_decl($2, NULL, $4); }
    ;

/* Struct Declaration */
struct_declaration
    : STRUCT TYPE_IDENTIFIER LBRACE parameter_list RBRACE
        { $$ = create_struct_decl($2, $4); }
    ;

/* Enum Declaration */
enum_declaration
    : ENUM TYPE_IDENTIFIER LBRACE expression_list RBRACE
        { $$ = create_enum_decl($2, $4); }
    ;

/* Trait Declaration */
trait_declaration
    : TRAIT TYPE_IDENTIFIER LBRACE statement_list RBRACE
        { $$ = create_trait_decl($2, $4); }
    ;

/* Control Flow */
if_statement
    : IF expression block
        { $$ = create_if_stmt($2, $3, NULL); }
    | IF expression block ELSE block
        { $$ = create_if_stmt($2, $3, $5); }
    | IF expression block ELSE if_statement
        { $$ = create_if_stmt($2, $3, $5); }
    ;

while_statement
    : WHILE expression block
        { $$ = create_while_stmt($2, $3); }
    ;

for_statement
    : FOR IDENTIFIER IN expression block
        { $$ = create_for_stmt($2, $4, $5); }
    ;

match_statement
    : MATCH expression LBRACE match_arm_list RBRACE
        { $$ = create_match_stmt($2, $4); }
    ;

match_arm_list
    : match_arm                     { $$ = create_match_arm_list($1); }
    | match_arm_list match_arm      { $$ = append_match_arm($1, $2); }
    ;

match_arm
    : pattern FAT_ARROW expression COMMA
        { $$ = create_match_arm($1, $3); }
    | pattern FAT_ARROW block
        { $$ = create_match_arm($1, $3); }
    ;

pattern
    : literal                       { $$ = $1; }
    | IDENTIFIER                    { $$ = create_identifier($1); }
    | expression RANGE expression   { $$ = create_range_pattern($1, $3, 0); }
    | expression RANGE_INCLUSIVE expression
        { $$ = create_range_pattern($1, $3, 1); }
    ;

/* Block */
block
    : LBRACE statement_list RBRACE  { $$ = create_block($2); }
    | LBRACE RBRACE                 { $$ = create_block(NULL); }
    ;

/* Expressions */
expression
    : binary_expression             { $$ = $1; }
    | unary_expression              { $$ = $1; }
    | call_expression               { $$ = $1; }
    | member_expression             { $$ = $1; }
    | index_expression              { $$ = $1; }
    | primary_expression            { $$ = $1; }
    ;

binary_expression
    : expression PLUS expression    { $$ = create_binary_expr("+", $1, $3); }
    | expression MINUS expression   { $$ = create_binary_expr("-", $1, $3); }
    | expression STAR expression    { $$ = create_binary_expr("*", $1, $3); }
    | expression SLASH expression   { $$ = create_binary_expr("/", $1, $3); }
    | expression PERCENT expression { $$ = create_binary_expr("%", $1, $3); }
    | expression POWER expression   { $$ = create_binary_expr("**", $1, $3); }
    | expression EQ expression      { $$ = create_binary_expr("==", $1, $3); }
    | expression NE expression      { $$ = create_binary_expr("!=", $1, $3); }
    | expression LT expression      { $$ = create_binary_expr("<", $1, $3); }
    | expression GT expression      { $$ = create_binary_expr(">", $1, $3); }
    | expression LE expression      { $$ = create_binary_expr("<=", $1, $3); }
    | expression GE expression      { $$ = create_binary_expr(">=", $1, $3); }
    | expression LOGICAL_AND expression { $$ = create_binary_expr("&&", $1, $3); }
    | expression LOGICAL_OR expression  { $$ = create_binary_expr("||", $1, $3); }
    | expression AMPERSAND expression   { $$ = create_binary_expr("&", $1, $3); }
    | expression PIPE expression        { $$ = create_binary_expr("|", $1, $3); }
    | expression ASSIGN expression      { $$ = create_binary_expr("=", $1, $3); }
    ;

unary_expression
    : MINUS expression              { $$ = create_unary_expr("-", $2); }
    | LOGICAL_NOT expression        { $$ = create_unary_expr("!", $2); }
    | TILDE expression              { $$ = create_unary_expr("~", $2); }
    | AMPERSAND expression          { $$ = create_unary_expr("&", $2); }
    | STAR expression               { $$ = create_unary_expr("*", $2); }
    ;

call_expression
    : expression LPAREN argument_list RPAREN
        { $$ = create_call_expr($1, $3); }
    ;

member_expression
    : expression DOT IDENTIFIER     { $$ = create_member_expr($1, $3); }
    ;

index_expression
    : expression LBRACKET expression RBRACKET
        { $$ = create_index_expr($1, $3); }
    ;

primary_expression
    : literal                       { $$ = $1; }
    | IDENTIFIER                    { $$ = create_identifier($1); }
    | LPAREN expression RPAREN      { $$ = $2; }
    | array_literal                 { $$ = $1; }
    | struct_literal                { $$ = $1; }
    ;

literal
    : INTEGER                       { $$ = create_int_literal($1); }
    | FLOAT                         { $$ = create_float_literal($1); }
    | STRING                        { $$ = create_string_literal($1); }
    | TRUE                          { $$ = create_bool_literal(1); }
    | FALSE                         { $$ = create_bool_literal(0); }
    | NULL_LIT                      { $$ = create_null_literal(); }
    ;

array_literal
    : LBRACKET expression_list RBRACKET
        { $$ = create_array_literal($2); }
    | LBRACKET RBRACKET             { $$ = create_array_literal(NULL); }
    ;

struct_literal
    : TYPE_IDENTIFIER LBRACE expression_list RBRACE
        { $$ = create_struct_literal($1, $3); }
    ;

expression_list
    : expression                    { $$ = create_expr_list($1); }
    | expression_list COMMA expression
        { $$ = append_expr($1, $3); }
    ;

argument_list
    : expression                    { $$ = create_arg_list($1); }
    | argument_list COMMA expression { $$ = append_arg($1, $3); }
    | /* empty */                   { $$ = NULL; }
    ;

%%

/* Additional C code */
void yyerror(const char *s) {
    fprintf(stderr, "Parse error at line %d: %s\n", line_num, s);
}


/* ============================================================================
 * EXTENDED GRAMMAR RULES - ADVANCED FEATURES
 * ============================================================================
 */

/* Lambda Expressions with Multiple Syntaxes */
lambda_expression
    : PIPE parameter_list PIPE expression
        { $$ = create_lambda_expr($2, $4, 0); }
    | PIPE parameter_list PIPE block
        { $$ = create_lambda_expr($2, $4, 1); }
    | PIPE PIPE expression
        { $$ = create_lambda_expr(NULL, $3, 0); }
    | PIPE PIPE block
        { $$ = create_lambda_expr(NULL, $3, 1); }
    ;

/* Async Blocks */
async_block
    : ASYNC block
        { $$ = create_async_block($2); }
    ;

/* Try Expression */
try_expression
    : TRY block catch_clauses finally_clause
        { $$ = create_try_expr($2, $3, $4); }
    | TRY block catch_clauses
        { $$ = create_try_expr($2, $3, NULL); }
    ;

catch_clauses
    : catch_clause
        { $$ = create_catch_list($1); }
    | catch_clauses catch_clause
        { $$ = append_catch($1, $2); }
    ;

catch_clause
    : CATCH LPAREN parameter RPAREN block
        { $$ = create_catch_clause($3, $5); }
    ;

finally_clause
    : FINALLY block
        { $$ = $2; }
    ;

/* Unsafe Blocks */
unsafe_block
    : UNSAFE block
        { $$ = create_unsafe_block($2); }
    ;

/* Const Blocks */
const_block
    : CONST block
        { $$ = create_const_block($2); }
    ;

/* Type Aliases */
type_alias_declaration
    : TYPE TYPE_IDENTIFIER generic_params ASSIGN type_annotation SEMICOLON
        { $$ = create_type_alias($2, $3, $5); }
    ;

/* Trait Bounds */
trait_bounds
    : COLON trait_bound_list
        { $$ = $2; }
    ;

trait_bound_list
    : trait_bound
        { $$ = create_trait_bound_list($1); }
    | trait_bound_list PLUS trait_bound
        { $$ = append_trait_bound($1, $3); }
    ;

trait_bound
    : TYPE_IDENTIFIER
        { $$ = create_trait_bound($1, NULL); }
    | TYPE_IDENTIFIER LT type_annotation GT
        { $$ = create_trait_bound($1, $3); }
    ;

/* Where Clauses */
where_clause
    : WHERE where_predicates
        { $$ = $2; }
    ;

where_predicates
    : where_predicate
        { $$ = create_where_predicate_list($1); }
    | where_predicates COMMA where_predicate
        { $$ = append_where_predicate($1, $3); }
    ;

where_predicate
    : IDENTIFIER trait_bounds
        { $$ = create_where_predicate($1, $2); }
    | IDENTIFIER COLON type_annotation
        { $$ = create_where_predicate_type($1, $3); }
    ;

/* Generic Parameters */
generic_params
    : LT generic_param_list GT
        { $$ = $2; }
    ;

generic_param_list
    : generic_param
        { $$ = create_generic_param_list($1); }
    | generic_param_list COMMA generic_param
        { $$ = append_generic_param($1, $3); }
    ;

generic_param
    : IDENTIFIER
        { $$ = create_generic_param($1, NULL, NULL); }
    | IDENTIFIER trait_bounds
        { $$ = create_generic_param($1, $2, NULL); }
    | IDENTIFIER ASSIGN type_annotation
        { $$ = create_generic_param($1, NULL, $3); }
    ;

/* Lifetime Annotations */
lifetime_annotation
    : APOSTROPHE IDENTIFIER
        { $$ = create_lifetime($2); }
    ;

lifetime_params
    : LT lifetime_param_list GT
        { $$ = $2; }
    ;

lifetime_param_list
    : lifetime_annotation
        { $$ = create_lifetime_list($1); }
    | lifetime_param_list COMMA lifetime_annotation
        { $$ = append_lifetime($1, $3); }
    ;

/* Reference Types */
reference_type
    : AMPERSAND type_annotation
        { $$ = create_reference_type($2, 0); }
    | AMPERSAND MUT type_annotation
        { $$ = create_reference_type($3, 1); }
    | AMPERSAND lifetime_annotation type_annotation
        { $$ = create_reference_type_with_lifetime($2, $3, 0); }
    | AMPERSAND lifetime_annotation MUT type_annotation
        { $$ = create_reference_type_with_lifetime($2, $4, 1); }
    ;

/* Pointer Types */
pointer_type
    : STAR CONST type_annotation
        { $$ = create_pointer_type($3, 1); }
    | STAR MUT type_annotation
        { $$ = create_pointer_type($3, 0); }
    ;

/* Array Types */
array_type
    : LBRACKET type_annotation SEMICOLON expression RBRACKET
        { $$ = create_array_type_sized($2, $4); }
    | LBRACKET type_annotation RBRACKET
        { $$ = create_array_type($2); }
    ;

/* Tuple Types */
tuple_type
    : LPAREN type_annotation_list RPAREN
        { $$ = create_tuple_type($2); }
    ;

type_annotation_list
    : type_annotation
        { $$ = create_type_list($1); }
    | type_annotation_list COMMA type_annotation
        { $$ = append_type($1, $3); }
    ;

/* Function Types */
function_type
    : FN LPAREN type_annotation_list RPAREN ARROW type_annotation
        { $$ = create_function_type($3, $6); }
    | FN LPAREN RPAREN ARROW type_annotation
        { $$ = create_function_type(NULL, $5); }
    ;

/* Trait Object Types */
trait_object_type
    : DYN trait_bounds
        { $$ = create_trait_object_type($2); }
    ;

/* Impl Trait Types */
impl_trait_type
    : IMPL trait_bounds
        { $$ = create_impl_trait_type($2); }
    ;

/* Never Type */
never_type
    : LOGICAL_NOT
        { $$ = create_never_type(); }
    ;

/* Slice Patterns */
slice_pattern
    : LBRACKET slice_pattern_items RBRACKET
        { $$ = create_slice_pattern($2); }
    ;

slice_pattern_items
    : pattern
        { $$ = create_pattern_list($1); }
    | slice_pattern_items COMMA pattern
        { $$ = append_pattern($1, $3); }
    | pattern COMMA RANGE COMMA pattern
        { $$ = create_slice_pattern_with_rest($1, $5); }
    ;

/* Struct Pattern */
struct_pattern
    : TYPE_IDENTIFIER LBRACE struct_pattern_fields RBRACE
        { $$ = create_struct_pattern($1, $3); }
    ;

struct_pattern_fields
    : struct_pattern_field
        { $$ = create_struct_field_pattern_list($1); }
    | struct_pattern_fields COMMA struct_pattern_field
        { $$ = append_struct_field_pattern($1, $3); }
    ;

struct_pattern_field
    : IDENTIFIER
        { $$ = create_struct_field_pattern($1, NULL); }
    | IDENTIFIER COLON pattern
        { $$ = create_struct_field_pattern($1, $3); }
    | RANGE
        { $$ = create_struct_field_rest_pattern(); }
    ;

/* Enum Pattern */
enum_pattern
    : TYPE_IDENTIFIER DOUBLE_COLON IDENTIFIER
        { $$ = create_enum_pattern($1, $3, NULL); }
    | TYPE_IDENTIFIER DOUBLE_COLON IDENTIFIER LPAREN pattern_list RPAREN
        { $$ = create_enum_pattern($1, $3, $5); }
    ;

pattern_list
    : pattern
        { $$ = create_pattern_list($1); }
    | pattern_list COMMA pattern
        { $$ = append_pattern($1, $3); }
    ;

/* Or Pattern */
or_pattern
    : pattern PIPE pattern
        { $$ = create_or_pattern($1, $3); }
    | or_pattern PIPE pattern
        { $$ = append_or_pattern($1, $3); }
    ;

/* Binding Pattern */
binding_pattern
    : IDENTIFIER AT pattern
        { $$ = create_binding_pattern($1, $3); }
    ;

/* Macro Invocation */
macro_invocation
    : IDENTIFIER LOGICAL_NOT LPAREN token_tree RPAREN
        { $$ = create_macro_invocation($1, $4, MACRO_PAREN); }
    | IDENTIFIER LOGICAL_NOT LBRACKET token_tree RBRACKET
        { $$ = create_macro_invocation($1, $4, MACRO_BRACKET); }
    | IDENTIFIER LOGICAL_NOT LBRACE token_tree RBRACE
        { $$ = create_macro_invocation($1, $4, MACRO_BRACE); }
    ;

token_tree
    : /* empty */
        { $$ = NULL; }
    | token_tree_list
        { $$ = $1; }
    ;

token_tree_list
    : token_tree_item
        { $$ = create_token_tree_list($1); }
    | token_tree_list token_tree_item
        { $$ = append_token_tree($1, $2); }
    ;

token_tree_item
    : IDENTIFIER
        { $$ = create_token_tree_identifier($1); }
    | literal
        { $$ = create_token_tree_literal($1); }
    | LPAREN token_tree RPAREN
        { $$ = create_token_tree_group($2, MACRO_PAREN); }
    | LBRACKET token_tree RBRACKET
        { $$ = create_token_tree_group($2, MACRO_BRACKET); }
    | LBRACE token_tree RBRACE
        { $$ = create_token_tree_group($2, MACRO_BRACE); }
    ;

/* Attribute Syntax */
attribute
    : HASH LBRACKET attribute_content RBRACKET
        { $$ = create_attribute($3, 0); }
    | HASH LOGICAL_NOT LBRACKET attribute_content RBRACKET
        { $$ = create_attribute($4, 1); }
    ;

attribute_content
    : IDENTIFIER
        { $$ = create_attribute_content($1, NULL); }
    | IDENTIFIER LPAREN attribute_args RPAREN
        { $$ = create_attribute_content($1, $3); }
    ;

attribute_args
    : expression
        { $$ = create_attr_arg_list($1); }
    | attribute_args COMMA expression
        { $$ = append_attr_arg($1, $3); }
    ;

/* Visibility Modifiers */
visibility
    : PUB
        { $$ = create_visibility(VIS_PUBLIC, NULL); }
    | PUB LPAREN CRATE RPAREN
        { $$ = create_visibility(VIS_CRATE, NULL); }
    | PUB LPAREN SUPER RPAREN
        { $$ = create_visibility(VIS_SUPER, NULL); }
    | PUB LPAREN IN module_path RPAREN
        { $$ = create_visibility(VIS_IN, $4); }
    | PRIV
        { $$ = create_visibility(VIS_PRIVATE, NULL); }
    ;

module_path
    : IDENTIFIER
        { $$ = create_module_path($1); }
    | module_path DOUBLE_COLON IDENTIFIER
        { $$ = append_module_path($1, $3); }
    ;

/* Use Declarations */
use_declaration
    : USE use_tree SEMICOLON
        { $$ = create_use_decl($2); }
    ;

use_tree
    : module_path
        { $$ = create_use_tree_path($1); }
    | module_path AS IDENTIFIER
        { $$ = create_use_tree_alias($1, $3); }
    | module_path DOUBLE_COLON STAR
        { $$ = create_use_tree_glob($1); }
    | module_path DOUBLE_COLON LBRACE use_tree_list RBRACE
        { $$ = create_use_tree_nested($1, $4); }
    | LBRACE use_tree_list RBRACE
        { $$ = create_use_tree_nested(NULL, $2); }
    ;

use_tree_list
    : use_tree
        { $$ = create_use_tree_list($1); }
    | use_tree_list COMMA use_tree
        { $$ = append_use_tree($1, $3); }
    ;

/* Extern Blocks */
extern_block
    : EXTERN STRING LBRACE extern_items RBRACE
        { $$ = create_extern_block($2, $4); }
    | EXTERN LBRACE extern_items RBRACE
        { $$ = create_extern_block(NULL, $3); }
    ;

extern_items
    : extern_item
        { $$ = create_extern_item_list($1); }
    | extern_items extern_item
        { $$ = append_extern_item($1, $2); }
    ;

extern_item
    : extern_function
        { $$ = $1; }
    | extern_static
        { $$ = $1; }
    ;

extern_function
    : FN IDENTIFIER LPAREN parameter_list RPAREN return_type SEMICOLON
        { $$ = create_extern_function($2, $4, $6); }
    ;

extern_static
    : STATIC MUT IDENTIFIER COLON type_annotation SEMICOLON
        { $$ = create_extern_static($3, $5, 1); }
    | STATIC IDENTIFIER COLON type_annotation SEMICOLON
        { $$ = create_extern_static($2, $4, 0); }
    ;

/* Associated Items */
associated_type
    : TYPE IDENTIFIER trait_bounds SEMICOLON
        { $$ = create_associated_type($2, $3); }
    | TYPE IDENTIFIER SEMICOLON
        { $$ = create_associated_type($2, NULL); }
    ;

associated_const
    : CONST IDENTIFIER COLON type_annotation SEMICOLON
        { $$ = create_associated_const($2, $4, NULL); }
    | CONST IDENTIFIER COLON type_annotation ASSIGN expression SEMICOLON
        { $$ = create_associated_const($2, $4, $6); }
    ;

/* Closure Expressions */
closure_expression
    : PIPE closure_params PIPE closure_body
        { $$ = create_closure($2, $4); }
    | PIPE PIPE closure_body
        { $$ = create_closure(NULL, $3); }
    ;

closure_params
    : closure_param
        { $$ = create_closure_param_list($1); }
    | closure_params COMMA closure_param
        { $$ = append_closure_param($1, $3); }
    ;

closure_param
    : IDENTIFIER
        { $$ = create_closure_param($1, NULL); }
    | IDENTIFIER COLON type_annotation
        { $$ = create_closure_param($1, $3); }
    ;

closure_body
    : expression
        { $$ = $1; }
    | block
        { $$ = $1; }
    ;

/* Range Expressions */
range_expression
    : expression RANGE expression
        { $$ = create_range_expr($1, $3, 0); }
    | expression RANGE_INCLUSIVE expression
        { $$ = create_range_expr($1, $3, 1); }
    | RANGE expression
        { $$ = create_range_expr(NULL, $2, 0); }
    | expression RANGE
        { $$ = create_range_expr($1, NULL, 0); }
    | RANGE
        { $$ = create_range_expr(NULL, NULL, 0); }
    ;

/* Field Expressions */
field_expression
    : expression DOT IDENTIFIER
        { $$ = create_field_expr($1, $3); }
    | expression DOT INTEGER
        { $$ = create_tuple_index_expr($1, $3); }
    ;

/* Method Call */
method_call_expression
    : expression DOT IDENTIFIER LPAREN argument_list RPAREN
        { $$ = create_method_call($1, $3, $5); }
    | expression DOT IDENTIFIER LPAREN RPAREN
        { $$ = create_method_call($1, $3, NULL); }
    ;

/* Turbofish (Generic Arguments in Calls) */
turbofish
    : DOUBLE_COLON LT type_annotation_list GT
        { $$ = $3; }
    ;

/* Await Expression */
await_expression
    : expression DOT AWAIT
        { $$ = create_await_expr($1); }
    ;

/* Question Mark Operator */
try_operator_expression
    : expression QUESTION
        { $$ = create_try_operator_expr($1); }
    ;

/* Cast Expression */
cast_expression
    : expression AS type_annotation
        { $$ = create_cast_expr($1, $3); }
    ;

/* Type Ascription */
type_ascription
    : expression COLON type_annotation
        { $$ = create_type_ascription($1, $3); }
    ;

/* Dereference */
dereference_expression
    : STAR expression
        { $$ = create_deref_expr($2); }
    ;

/* Address-of */
address_of_expression
    : AMPERSAND expression
        { $$ = create_address_of_expr($2, 0); }
    | AMPERSAND MUT expression
        { $$ = create_address_of_expr($3, 1); }
    ;

/* Box Expression */
box_expression
    : BOX expression
        { $$ = create_box_expr($2); }
    ;

/* Compound Assignment */
compound_assignment
    : expression PLUS_ASSIGN expression
        { $$ = create_compound_assign("+", $1, $3); }
    | expression MINUS_ASSIGN expression
        { $$ = create_compound_assign("-", $1, $3); }
    | expression STAR_ASSIGN expression
        { $$ = create_compound_assign("*", $1, $3); }
    | expression SLASH_ASSIGN expression
        { $$ = create_compound_assign("/", $1, $3); }
    | expression PERCENT_ASSIGN expression
        { $$ = create_compound_assign("%", $1, $3); }
    | expression AND_ASSIGN expression
        { $$ = create_compound_assign("&", $1, $3); }
    | expression OR_ASSIGN expression
        { $$ = create_compound_assign("|", $1, $3); }
    | expression XOR_ASSIGN expression
        { $$ = create_compound_assign("^", $1, $3); }
    | expression LSHIFT_ASSIGN expression
        { $$ = create_compound_assign("<<", $1, $3); }
    | expression RSHIFT_ASSIGN expression
        { $$ = create_compound_assign(">>", $1, $3); }
    ;

/* Labeled Blocks */
labeled_block
    : lifetime_annotation COLON block
        { $$ = create_labeled_block($1, $3); }
    ;

/* Labeled Loops */
labeled_loop
    : lifetime_annotation COLON LOOP block
        { $$ = create_labeled_loop($1, $4); }
    | lifetime_annotation COLON WHILE expression block
        { $$ = create_labeled_while($1, $4, $5); }
    | lifetime_annotation COLON FOR IDENTIFIER IN expression block
        { $$ = create_labeled_for($1, $4, $6, $7); }
    ;

/* Break with Label */
break_with_label
    : BREAK lifetime_annotation
        { $$ = create_break_with_label($2, NULL); }
    | BREAK lifetime_annotation expression
        { $$ = create_break_with_label($2, $3); }
    ;

/* Continue with Label */
continue_with_label
    : CONTINUE lifetime_annotation
        { $$ = create_continue_with_label($2); }
    ;

/* If Let Expression */
if_let_expression
    : IF LET pattern ASSIGN expression block
        { $$ = create_if_let($3, $5, $6, NULL); }
    | IF LET pattern ASSIGN expression block ELSE block
        { $$ = create_if_let($3, $5, $6, $8); }
    ;

/* While Let Loop */
while_let_loop
    : WHILE LET pattern ASSIGN expression block
        { $$ = create_while_let($3, $5, $6); }
    ;

%%

/* ============================================================================
 * ADDITIONAL HELPER FUNCTIONS
 * ============================================================================
 */

void print_parse_error(const char *msg, int line, int col) {
    fprintf(stderr, "\033[1;31mParse Error\033[0m at line %d, column %d: %s\n", 
            line, col, msg);
}

void print_syntax_error(const char *expected, const char *got, int line, int col) {
    fprintf(stderr, "\033[1;31mSyntax Error\033[0m at line %d, column %d:\n", 
            line, col);
    fprintf(stderr, "  Expected: %s\n", expected);
    fprintf(stderr, "  Got: %s\n", got);
}

void print_warning(const char *msg, int line, int col) {
    fprintf(stderr, "\033[1;33mWarning\033[0m at line %d, column %d: %s\n", 
            line, col, msg);
}

/* Error recovery */
void recover_from_error() {
    /* Skip tokens until we find a synchronization point */
    int token;
    while ((token = yylex()) != 0) {
        if (token == SEMICOLON || token == RBRACE || token == END_OF_FILE) {
            break;
        }
    }
}

/* Semantic validation helpers */
int validate_pattern(ASTNode *pattern) {
    if (!pattern) return 0;
    /* Add pattern validation logic */
    return 1;
}

int validate_type(ASTNode *type) {
    if (!type) return 0;
    /* Add type validation logic */
    return 1;
}

/* AST optimization hints */
void mark_tail_recursive(ASTNode *func) {
    if (func && func->type == AST_FUNC_DECL) {
        /* Mark function as tail recursive for optimization */
    }
}

void mark_pure_function(ASTNode *func) {
    if (func && func->type == AST_FUNC_DECL) {
        /* Mark function as pure (no side effects) */
    }
}

/* Debug helpers */
void dump_ast_json(ASTNode *node, FILE *output) {
    /* Output AST in JSON format for debugging */
    if (!node || !output) return;
    fprintf(output, "{\n");
    fprintf(output, "  \"type\": \"%d\",\n", node->type);
    fprintf(output, "  \"line\": %d,\n", node->line);
    fprintf(output, "  \"column\": %d\n", node->column);
    fprintf(output, "}\n");
}

void dump_ast_xml(ASTNode *node, FILE *output) {
    /* Output AST in XML format for debugging */
    if (!node || !output) return;
    fprintf(output, "<node type=\"%d\" line=\"%d\" column=\"%d\" />\n",
            node->type, node->line, node->column);
}

/* Statistics */
static struct {
    unsigned long total_nodes;
    unsigned long function_count;
    unsigned long class_count;
    unsigned long expression_count;
} parse_stats = {0, 0, 0, 0};

void increment_node_count() {
    parse_stats.total_nodes++;
}

void print_parse_statistics() {
    printf("\n=== Parse Statistics ===\n");
    printf("Total AST nodes: %lu\n", parse_stats.total_nodes);
    printf("Functions: %lu\n", parse_stats.function_count);
    printf("Classes: %lu\n", parse_stats.class_count);
    printf("Expressions: %lu\n", parse_stats.expression_count);
    printf("========================\n");
}

/* ============================================================================
 * ADDITIONAL EXPRESSION RULES
 * ============================================================================
 */

/* Spread operator in expressions */
spread_expression
    : SPREAD expression
        { $ = create_spread_expr($2); }
    ;

/* Conditional expression (ternary) */
ternary_expression
    : expression QUESTION expression COLON expression
        { $ = create_ternary_expr($1, $3, $5); }
    ;

/* Null coalescing */
null_coalesce_expression
    : expression NULL_COALESCE expression
        { $ = create_null_coalesce_expr($1, $3); }
    ;

/* Optional chaining */
optional_chain_expression
    : expression OPTIONAL_CHAIN IDENTIFIER
        { $ = create_optional_chain_expr($1, $3); }
    | expression OPTIONAL_CHAIN IDENTIFIER LPAREN argument_list RPAREN
        { $ = create_optional_chain_call($1, $3, $5); }
    ;

/* Increment/Decrement */
increment_expression
    : INCREMENT expression
        { $ = create_pre_increment($2); }
    | expression INCREMENT
        { $ = create_post_increment($1); }
    | DECREMENT expression
        { $ = create_pre_decrement($2); }
    | expression DECREMENT
        { $ = create_post_decrement($1); }
    ;

/* ============================================================================
 * ADVANCED TYPE SYSTEM RULES
 * ============================================================================
 */

/* Union types */
union_type
    : type_annotation PIPE type_annotation
        { $ = create_union_type($1, $3); }
    | union_type PIPE type_annotation
        { $ = append_union_type($1, $3); }
    ;

/* Intersection types */
intersection_type
    : type_annotation AMPERSAND type_annotation
        { $ = create_intersection_type($1, $3); }
    | intersection_type AMPERSAND type_annotation
        { $ = append_intersection_type($1, $3); }
    ;

/* Conditional types */
conditional_type
    : type_annotation EXTENDS type_annotation QUESTION type_annotation COLON type_annotation
        { $ = create_conditional_type($1, $3, $5, $7); }
    ;

/* Mapped types */
mapped_type
    : LBRACE IDENTIFIER IN type_annotation COLON type_annotation RBRACE
        { $ = create_mapped_type($2, $4, $6); }
    ;

/* Template literal types */
template_literal_type
    : TEMPLATE_STRING
        { $ = create_template_literal_type($1); }
    ;

/* ============================================================================
 * PATTERN MATCHING EXTENSIONS
 * ============================================================================
 */

/* Array pattern with rest */
array_pattern
    : LBRACKET array_pattern_elements RBRACKET
        { $ = create_array_pattern($2); }
    ;

array_pattern_elements
    : pattern
        { $ = create_pattern_list($1); }
    | array_pattern_elements COMMA pattern
        { $ = append_pattern($1, $3); }
    | pattern COMMA SPREAD IDENTIFIER
        { $ = create_array_pattern_with_rest($1, $4); }
    ;

/* Object destructuring pattern */
object_pattern
    : LBRACE object_pattern_properties RBRACE
        { $ = create_object_pattern($2); }
    ;

object_pattern_properties
    : object_pattern_property
        { $ = create_object_pattern_property_list($1); }
    | object_pattern_properties COMMA object_pattern_property
        { $ = append_object_pattern_property($1, $3); }
    ;

object_pattern_property
    : IDENTIFIER
        { $ = create_object_pattern_property_shorthand($1); }
    | IDENTIFIER COLON pattern
        { $ = create_object_pattern_property($1, $3); }
    | SPREAD IDENTIFIER
        { $ = create_object_pattern_rest($2); }
    ;

/* ============================================================================
 * GENERATOR AND ASYNC GENERATOR FUNCTIONS
 * ============================================================================
 */

/* Generator function */
generator_function
    : FN STAR IDENTIFIER LPAREN parameter_list RPAREN return_type block
        { $ = create_generator_function($3, $5, $7, $8); }
    ;

/* Async generator function */
async_generator_function
    : ASYNC FN STAR IDENTIFIER LPAREN parameter_list RPAREN return_type block
        { $ = create_async_generator_function($4, $6, $8, $9); }
    ;

/* Yield expression */
yield_expression
    : YIELD
        { $ = create_yield_expr(NULL); }
    | YIELD expression
        { $ = create_yield_expr($2); }
    | YIELD STAR expression
        { $ = create_yield_from_expr($3); }
    ;

/* ============================================================================
 * DECORATOR/ANNOTATION SYNTAX
 * ============================================================================
 */

/* Decorator list */
decorator_list
    : decorator
        { $ = create_decorator_list($1); }
    | decorator_list decorator
        { $ = append_decorator($1, $2); }
    ;

/* Single decorator */
decorator
    : AT IDENTIFIER
        { $ = create_decorator($2, NULL); }
    | AT IDENTIFIER LPAREN argument_list RPAREN
        { $ = create_decorator($2, $4); }
    | AT module_path
        { $ = create_decorator_path($2, NULL); }
    | AT module_path LPAREN argument_list RPAREN
        { $ = create_decorator_path($2, $4); }
    ;

/* Decorated function */
decorated_function
    : decorator_list function_declaration
        { $ = create_decorated_function($1, $2); }
    ;

/* Decorated class */
decorated_class
    : decorator_list class_declaration
        { $ = create_decorated_class($1, $2); }
    ;

/* ============================================================================
 * COMPREHENSION EXPRESSIONS
 * ============================================================================
 */

/* List comprehension */
list_comprehension
    : LBRACKET expression FOR IDENTIFIER IN expression comprehension_clauses RBRACKET
        { $ = create_list_comprehension($2, $4, $6, $7); }
    ;

/* Set comprehension */
set_comprehension
    : LBRACE expression FOR IDENTIFIER IN expression comprehension_clauses RBRACE
        { $ = create_set_comprehension($2, $4, $6, $7); }
    ;

/* Dict comprehension */
dict_comprehension
    : LBRACE expression COLON expression FOR IDENTIFIER IN expression comprehension_clauses RBRACE
        { $ = create_dict_comprehension($2, $4, $6, $8, $9); }
    ;

/* Comprehension clauses */
comprehension_clauses
    : /* empty */
        { $ = NULL; }
    | comprehension_clause_list
        { $ = $1; }
    ;

comprehension_clause_list
    : comprehension_clause
        { $ = create_comprehension_clause_list($1); }
    | comprehension_clause_list comprehension_clause
        { $ = append_comprehension_clause($1, $2); }
    ;

comprehension_clause
    : FOR IDENTIFIER IN expression
        { $ = create_comprehension_for($2, $4); }
    | IF expression
        { $ = create_comprehension_if($2); }
    ;

/* ============================================================================
 * WITH STATEMENT (CONTEXT MANAGERS)
 * ============================================================================
 */

/* With statement */
with_statement
    : WITH with_items block
        { $ = create_with_stmt($2, $3); }
    ;

with_items
    : with_item
        { $ = create_with_item_list($1); }
    | with_items COMMA with_item
        { $ = append_with_item($1, $3); }
    ;

with_item
    : expression
        { $ = create_with_item($1, NULL); }
    | expression AS IDENTIFIER
        { $ = create_with_item($1, $3); }
    ;


/* ============================================================================
 * EXTENDED EXPRESSION GRAMMAR - COMPREHENSIVE OPERATORS
 * ============================================================================
 */

/* Bitwise operations extended */
bitwise_expression
    : expression AMPERSAND expression
        { $ = create_bitwise_expr("&", $1, $3); }
    | expression PIPE expression
        { $ = create_bitwise_expr("|", $1, $3); }
    | expression CARET expression
        { $ = create_bitwise_expr("^", $1, $3); }
    | expression LSHIFT expression
        { $ = create_bitwise_expr("<<", $1, $3); }
    | expression RSHIFT expression
        { $ = create_bitwise_expr(">>", $1, $3); }
    | expression UNSIGNED_RSHIFT expression
        { $ = create_bitwise_expr(">>>", $1, $3); }
    | TILDE expression
        { $ = create_bitwise_not($2); }
    ;

/* String interpolation */
string_interpolation
    : STRING_START interpolation_parts STRING_END
        { $ = create_string_interpolation($2); }
    ;

interpolation_parts
    : interpolation_part
        { $ = create_interpolation_list($1); }
    | interpolation_parts interpolation_part
        { $ = append_interpolation($1, $2); }
    ;

interpolation_part
    : STRING_LITERAL
        { $ = create_string_part($1); }
    | INTERPOLATION_START expression INTERPOLATION_END
        { $ = create_expr_part($2); }
    ;

/* Collection literals extended */
set_literal
    : LBRACE expression_list RBRACE
        { $ = create_set_literal($2); }
    | LBRACE RBRACE
        { $ = create_set_literal(NULL); }
    ;

map_literal
    : LBRACE map_entries RBRACE
        { $ = create_map_literal($2); }
    | LBRACE RBRACE
        { $ = create_map_literal(NULL); }
    ;

map_entries
    : map_entry
        { $ = create_map_entry_list($1); }
    | map_entries COMMA map_entry
        { $ = append_map_entry($1, $3); }
    ;

map_entry
    : expression COLON expression
        { $ = create_map_entry($1, $3); }
    ;

/* Slice expressions */
slice_expression
    : expression LBRACKET slice_range RBRACKET
        { $ = create_slice_expr($1, $3); }
    ;

slice_range
    : expression RANGE expression
        { $ = create_slice_range($1, $3, 0); }
    | expression RANGE_INCLUSIVE expression
        { $ = create_slice_range($1, $3, 1); }
    | RANGE expression
        { $ = create_slice_range(NULL, $2, 0); }
    | expression RANGE
        { $ = create_slice_range($1, NULL, 0); }
    | RANGE
        { $ = create_slice_range(NULL, NULL, 0); }
    ;

/* Chained comparisons */
chained_comparison
    : expression comparison_op expression comparison_op expression
        { $ = create_chained_comparison($1, $2, $3, $4, $5); }
    ;

comparison_op
    : LT | GT | LE | GE | EQ | NE
    ;

/* Elvis operator */
elvis_expression
    : expression QUESTION COLON expression
        { $ = create_elvis_expr($1, $4); }
    ;

/* Safe navigation */
safe_navigation
    : expression QUESTION DOT IDENTIFIER
        { $ = create_safe_nav($1, $4); }
    | expression QUESTION LBRACKET expression RBRACKET
        { $ = create_safe_index($1, $4); }
    ;

/* Type checking expressions */
type_check_expression
    : expression IS type_annotation
        { $ = create_type_check($1, $3); }
    | expression IS NOT type_annotation
        { $ = create_type_check_not($1, $4); }
    ;

/* Pattern guards */
pattern_guard
    : pattern IF expression
        { $ = create_pattern_guard($1, $3); }
    ;

/* Destructuring in function parameters */
destructuring_parameter
    : LBRACKET identifier_list RBRACKET
        { $ = create_array_destructure_param($2); }
    | LBRACE field_destructure_list RBRACE
        { $ = create_object_destructure_param($2); }
    ;

identifier_list
    : IDENTIFIER
        { $ = create_identifier_list($1); }
    | identifier_list COMMA IDENTIFIER
        { $ = append_identifier($1, $3); }
    ;

field_destructure_list
    : field_destructure
        { $ = create_field_destructure_list($1); }
    | field_destructure_list COMMA field_destructure
        { $ = append_field_destructure($1, $3); }
    ;

field_destructure
    : IDENTIFIER
        { $ = create_field_destructure($1, NULL); }
    | IDENTIFIER COLON IDENTIFIER
        { $ = create_field_destructure($1, $3); }
    ;

/* ============================================================================
 * ADVANCED CONTROL FLOW
 * ============================================================================
 */

/* Do-while loop */
do_while_statement
    : DO block WHILE expression SEMICOLON
        { $ = create_do_while($2, $4); }
    ;

/* Foreach loop with index */
foreach_indexed
    : FOR LPAREN IDENTIFIER COMMA IDENTIFIER IN expression RPAREN block
        { $ = create_foreach_indexed($3, $5, $7, $9); }
    ;

/* Loop with label and break value */
labeled_loop_with_value
    : IDENTIFIER COLON LOOP block
        { $ = create_labeled_loop($1, $4); }
    ;

break_with_value
    : BREAK expression SEMICOLON
        { $ = create_break_with_value($2); }
    | BREAK IDENTIFIER expression SEMICOLON
        { $ = create_break_labeled_value($2, $3); }
    ;

/* Switch statement (C-style) */
switch_statement
    : SWITCH expression LBRACE switch_cases RBRACE
        { $ = create_switch_stmt($2, $4); }
    ;

switch_cases
    : switch_case
        { $ = create_switch_case_list($1); }
    | switch_cases switch_case
        { $ = append_switch_case($1, $2); }
    ;

switch_case
    : CASE expression COLON statement_list
        { $ = create_switch_case($2, $4); }
    | DEFAULT COLON statement_list
        { $ = create_switch_default($3); }
    ;

/* Guard clauses */
guard_statement
    : GUARD expression ELSE block
        { $ = create_guard_stmt($2, $4); }
    ;

/* Unless statement */
unless_statement
    : UNLESS expression block
        { $ = create_unless_stmt($2, $3); }
    ;

/* ============================================================================
 * TYPE SYSTEM EXTENSIONS
 * ============================================================================
 */

/* Variadic types */
variadic_type
    : type_annotation SPREAD
        { $ = create_variadic_type($1); }
    ;

/* Const generics */
const_generic_param
    : CONST IDENTIFIER COLON type_annotation
        { $ = create_const_generic_param($2, $4); }
    ;

/* Higher-kinded types */
higher_kinded_type
    : TYPE_IDENTIFIER LT TYPE_IDENTIFIER LT type_annotation GT GT
        { $ = create_higher_kinded_type($1, $3, $5); }
    ;

/* Existential types */
existential_type
    : EXISTS TYPE_IDENTIFIER DOT type_annotation
        { $ = create_existential_type($2, $4); }
    ;

/* Refinement types */
refinement_type
    : LBRACE IDENTIFIER COLON type_annotation PIPE expression RBRACE
        { $ = create_refinement_type($2, $4, $6); }
    ;

/* Type bounds with multiple traits */
multi_trait_bound
    : type_annotation COLON trait_list
        { $ = create_multi_trait_bound($1, $3); }
    ;

trait_list
    : TYPE_IDENTIFIER
        { $ = create_trait_list($1); }
    | trait_list PLUS TYPE_IDENTIFIER
        { $ = append_trait($1, $3); }
    ;

/* Associated type projection */
associated_type_projection
    : TYPE_IDENTIFIER DOUBLE_COLON TYPE_IDENTIFIER
        { $ = create_assoc_type_projection($1, $3); }
    ;

/* ============================================================================
 * ADVANCED PATTERN MATCHING
 * ============================================================================
 */

/* Nested patterns */
nested_pattern
    : pattern AT pattern
        { $ = create_nested_pattern($1, $3); }
    ;

/* Range patterns with step */
range_pattern_stepped
    : expression RANGE expression COLON expression
        { $ = create_range_pattern_step($1, $3, $5); }
    ;

/* Regex patterns */
regex_pattern
    : REGEX_LITERAL
        { $ = create_regex_pattern($1); }
    ;

/* Type patterns */
type_pattern
    : TYPE_IDENTIFIER LPAREN pattern_list RPAREN
        { $ = create_type_pattern($1, $3); }
    ;

pattern_list
    : pattern
        { $ = create_pattern_list($1); }
    | pattern_list COMMA pattern
        { $ = append_pattern($1, $3); }
    ;

/* Constant patterns */
const_pattern
    : CONST_IDENTIFIER
        { $ = create_const_pattern($1); }
    ;

/* ============================================================================
 * MEMORY MANAGEMENT CONSTRUCTS
 * ============================================================================
 */

/* Explicit lifetime bounds */
lifetime_bound
    : lifetime_annotation COLON lifetime_annotation
        { $ = create_lifetime_bound($1, $3); }
    ;

/* Lifetime elision rules */
lifetime_elision
    : AMPERSAND type_annotation
        { $ = create_lifetime_elision($2); }
    ;

/* Move semantics */
move_expression
    : MOVE expression
        { $ = create_move_expr($2); }
    ;

/* Borrow expressions */
borrow_expression
    : AMPERSAND expression
        { $ = create_borrow_expr($2, 0); }
    | AMPERSAND MUT expression
        { $ = create_borrow_expr($3, 1); }
    ;

/* Pin expressions */
pin_expression
    : PIN expression
        { $ = create_pin_expr($2); }
    ;

/* ============================================================================
 * ASYNC/CONCURRENT CONSTRUCTS
 * ============================================================================
 */

/* Async blocks with move */
async_move_block
    : ASYNC MOVE block
        { $ = create_async_move_block($3); }
    ;

/* Select expression */
select_expression
    : SELECT LBRACE select_arms RBRACE
        { $ = create_select_expr($3); }
    ;

select_arms
    : select_arm
        { $ = create_select_arm_list($1); }
    | select_arms select_arm
        { $ = append_select_arm($1, $2); }
    ;

select_arm
    : pattern ASSIGN AWAIT expression FAT_ARROW expression COMMA
        { $ = create_select_arm($1, $4, $6); }
    ;

/* Spawn expression */
spawn_expression
    : SPAWN expression
        { $ = create_spawn_expr($2); }
    | SPAWN MOVE expression
        { $ = create_spawn_move_expr($3); }
    ;

/* Channel operations */
channel_send
    : expression SEND_OP expression
        { $ = create_channel_send($1, $3); }
    ;

channel_receive
    : RECEIVE_OP expression
        { $ = create_channel_receive($2); }
    ;

/* ============================================================================
 * METAPROGRAMMING CONSTRUCTS
 * ============================================================================
 */

/* Compile-time if */
comptime_if
    : HASH IF expression block
        { $ = create_comptime_if($3, $4, NULL); }
    | HASH IF expression block ELSE block
        { $ = create_comptime_if($3, $4, $6); }
    ;

/* Static assertions */
static_assert
    : STATIC_ASSERT LPAREN expression COMMA STRING RPAREN SEMICOLON
        { $ = create_static_assert($3, $5); }
    ;

/* Type introspection */
type_introspection
    : TYPEOF LPAREN expression RPAREN
        { $ = create_typeof_expr($3); }
    | SIZEOF LPAREN type_annotation RPAREN
        { $ = create_sizeof_type($3); }
    | ALIGNOF LPAREN type_annotation RPAREN
        { $ = create_alignof_type($3); }
    ;

/* Macro expansion */
macro_expansion
    : IDENTIFIER LOGICAL_NOT macro_args
        { $ = create_macro_expansion($1, $3); }
    ;

macro_args
    : LPAREN token_stream RPAREN
        { $ = create_macro_args($2, MACRO_PAREN); }
    | LBRACKET token_stream RBRACKET
        { $ = create_macro_args($2, MACRO_BRACKET); }
    | LBRACE token_stream RBRACE
        { $ = create_macro_args($2, MACRO_BRACE); }
    ;

token_stream
    : /* empty */
        { $ = NULL; }
    | token_stream_items
        { $ = $1; }
    ;

token_stream_items
    : token_stream_item
        { $ = create_token_stream($1); }
    | token_stream_items token_stream_item
        { $ = append_token_stream($1, $2); }
    ;

token_stream_item
    : IDENTIFIER | literal | operator_token
    ;

/* ============================================================================
 * ADVANCED FUNCTION FEATURES
 * ============================================================================
 */

/* Function with constraints */
constrained_function
    : FN IDENTIFIER generic_params LPAREN parameter_list RPAREN return_type where_clause block
        { $ = create_constrained_func($2, $3, $5, $7, $8, $9); }
    ;

/* Const functions */
const_function
    : CONST FN IDENTIFIER LPAREN parameter_list RPAREN return_type block
        { $ = create_const_func($3, $5, $7, $8); }
    ;

/* Inline functions */
inline_function
    : INLINE FN IDENTIFIER LPAREN parameter_list RPAREN return_type block
        { $ = create_inline_func($3, $5, $7, $8); }
    ;

/* Variadic functions */
variadic_function
    : FN IDENTIFIER LPAREN parameter_list COMMA SPREAD RPAREN return_type block
        { $ = create_variadic_func($2, $4, $8, $9); }
    ;

/* Default parameters */
default_parameter
    : IDENTIFIER COLON type_annotation ASSIGN expression
        { $ = create_default_param($1, $3, $5); }
    ;

/* Named arguments */
named_argument
    : IDENTIFIER ASSIGN expression
        { $ = create_named_arg($1, $3); }
    ;

/* ============================================================================
 * ERROR HANDLING EXTENSIONS
 * ============================================================================
 */

/* Try expression with multiple catch */
try_multi_catch
    : TRY block catch_list finally_clause
        { $ = create_try_multi_catch($2, $3, $4); }
    ;

catch_list
    : catch_clause
        { $ = create_catch_list($1); }
    | catch_list catch_clause
        { $ = append_catch($1, $2); }
    ;

/* Error propagation with context */
error_propagation_context
    : expression QUESTION DOT CONTEXT LPAREN STRING RPAREN
        { $ = create_error_context($1, $6); }
    ;

/* Custom error types */
error_type_declaration
    : ERROR TYPE_IDENTIFIER LBRACE error_variants RBRACE
        { $ = create_error_type($2, $4); }
    ;

error_variants
    : error_variant
        { $ = create_error_variant_list($1); }
    | error_variants COMMA error_variant
        { $ = append_error_variant($1, $3); }
    ;

error_variant
    : IDENTIFIER LPAREN type_annotation_list RPAREN
        { $ = create_error_variant($1, $3); }
    ;

/* ============================================================================
 * MODULE SYSTEM EXTENSIONS
 * ============================================================================
 */

/* Conditional compilation */
cfg_attribute
    : HASH LBRACKET CFG LPAREN cfg_predicate RPAREN RBRACKET
        { $ = create_cfg_attr($5); }
    ;

cfg_predicate
    : IDENTIFIER
        { $ = create_cfg_predicate($1, NULL); }
    | IDENTIFIER ASSIGN STRING
        { $ = create_cfg_predicate($1, $3); }
    | cfg_predicate COMMA cfg_predicate
        { $ = create_cfg_and($1, $3); }
    ;

/* Re-exports */
reexport_declaration
    : PUB USE use_tree AS IDENTIFIER SEMICOLON
        { $ = create_reexport($3, $5); }
    ;

/* Glob imports with exclusions */
glob_import_exclude
    : USE module_path DOUBLE_COLON LBRACE STAR COMMA exclude_list RBRACE SEMICOLON
        { $ = create_glob_exclude($2, $7); }
    ;

exclude_list
    : IDENTIFIER
        { $ = create_exclude_list($1); }
    | exclude_list COMMA IDENTIFIER
        { $ = append_exclude($1, $3); }
    ;

/* ============================================================================
 * OPERATOR OVERLOADING
 * ============================================================================
 */

operator_impl
    : IMPL operator_trait FOR TYPE_IDENTIFIER LBRACE operator_methods RBRACE
        { $ = create_operator_impl($2, $4, $6); }
    ;

operator_trait
    : ADD_TRAIT | SUB_TRAIT | MUL_TRAIT | DIV_TRAIT
    | INDEX_TRAIT | DEREF_TRAIT | CALL_TRAIT
    ;

operator_methods
    : operator_method
        { $ = create_operator_method_list($1); }
    | operator_methods operator_method
        { $ = append_operator_method($1, $2); }
    ;

operator_method
    : FN operator_name LPAREN parameter_list RPAREN return_type block
        { $ = create_operator_method($2, $4, $6, $7); }
    ;

operator_name
    : ADD | SUB | MUL | DIV | INDEX | DEREF | CALL
    ;

/* ============================================================================
 * ADDITIONAL HELPER RULES
 * ============================================================================
 */

/* Documentation comments */
doc_comment
    : DOC_COMMENT_LINE
        { $ = create_doc_comment($1); }
    | doc_comment DOC_COMMENT_LINE
        { $ = append_doc_comment($1, $2); }
    ;

/* Attributes */
attribute_list
    : attribute
        { $ = create_attribute_list($1); }
    | attribute_list attribute
        { $ = append_attribute($1, $2); }
    ;

/* Visibility with path */
visibility_path
    : PUB LPAREN IN module_path RPAREN
        { $ = create_visibility_in($4); }
    ;

%%

/* ============================================================================
 * ADDITIONAL SEMANTIC ACTIONS
 * ============================================================================
 */

void validate_lifetime_bounds(ASTNode *node) {
    /* Validate lifetime relationships */
}

void check_trait_coherence(ASTNode *impl_node) {
    /* Ensure trait implementations don't conflict */
}

void verify_const_evaluation(ASTNode *expr) {
    /* Verify expression can be evaluated at compile time */
}

int calculate_type_size(ASTNode *type) {
    /* Calculate size of type for layout */
    return 0;
}

void optimize_tail_recursion(ASTNode *func) {
    /* Convert tail recursive calls to loops */
}

void inline_small_functions(ASTNode *program) {
    /* Inline functions below threshold */
}

void eliminate_dead_branches(ASTNode *node) {
    /* Remove unreachable code */
}

void propagate_constants(ASTNode *node) {
    /* Constant folding and propagation */
}

void check_exhaustiveness(ASTNode *match_expr) {
    /* Verify all patterns are covered */
}

void infer_lifetimes(ASTNode *func) {
    /* Apply lifetime elision rules */
}

void resolve_trait_methods(ASTNode *call) {
    /* Resolve which trait method to call */
}

void check_move_semantics(ASTNode *expr) {
    /* Verify value isn't used after move */
}

void validate_borrow_rules(ASTNode *node) {
    /* Check borrowing rules are followed */
}

void generate_drop_code(ASTNode *scope) {
    /* Insert destructor calls */
}

void check_send_sync_bounds(ASTNode *async_block) {
    /* Verify types are Send/Sync for async */
}

void resolve_macro_hygiene(ASTNode *macro_expansion) {
    /* Handle macro variable scoping */
}

void expand_derive_macros(ASTNode *struct_decl) {
    /* Generate trait implementations */
}

void validate_const_generics(ASTNode *generic_params) {
    /* Check const generic parameters */
}

void check_variance(ASTNode *type_param) {
    /* Determine variance of type parameters */
}

void resolve_associated_types(ASTNode *trait_bound) {
    /* Resolve associated type projections */
}

/* End of extended parser */
