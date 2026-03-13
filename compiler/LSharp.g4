// ANTLR4 Grammar for L# Programming Language
grammar LSharp;

// Parser Rules

program
    : statement* EOF
    ;

statement
    : variableDeclaration
    | constantDeclaration
    | functionDeclaration
    | classDeclaration
    | structDeclaration
    | enumDeclaration
    | traitDeclaration
    | implDeclaration
    | useStatement
    | exportStatement
    | expressionStatement
    | ifStatement
    | whileStatement
    | forStatement
    | loopStatement
    | matchStatement
    | returnStatement
    | breakStatement
    | continueStatement
    | block
    ;

// Declarations

variableDeclaration
    : LET MUT? IDENTIFIER typeAnnotation? '=' expression ';'
    ;

constantDeclaration
    : CONST IDENTIFIER '=' expression ';'
    ;

functionDeclaration
    : ASYNC? FN IDENTIFIER genericParams? '(' parameterList? ')' returnType? block
    ;

classDeclaration
    : CLASS TYPE_IDENTIFIER genericParams? inheritance? '{' classMember* '}'
    ;

structDeclaration
    : STRUCT TYPE_IDENTIFIER genericParams? '{' structField* '}'
    ;

enumDeclaration
    : ENUM TYPE_IDENTIFIER genericParams? '{' enumVariant (',' enumVariant)* ','? '}'
    ;

traitDeclaration
    : TRAIT TYPE_IDENTIFIER genericParams? traitBounds? '{' traitMember* '}'
    ;

implDeclaration
    : IMPL genericParams? TYPE_IDENTIFIER FOR TYPE_IDENTIFIER whereClause? '{' implMember* '}'
    ;

useStatement
    : USE modulePath ('as' IDENTIFIER)? ';'
    | USE modulePath '::' '{' importList '}' ';'
    | USE modulePath '::' '*' ';'
    ;

exportStatement
    : EXPORT statement
    | EXPORT '{' IDENTIFIER (',' IDENTIFIER)* '}'
    ;

// Class Members

classMember
    : fieldDeclaration
    | methodDeclaration
    | constructorDeclaration
    ;

fieldDeclaration
    : modifier* IDENTIFIER ':' typeExpression ';'
    ;

methodDeclaration
    : modifier* ASYNC? FN IDENTIFIER genericParams? '(' parameterList? ')' returnType? block
    ;

constructorDeclaration
    : INIT '(' parameterList? ')' block
    ;

// Struct Fields

structField
    : IDENTIFIER ':' typeExpression ','?
    ;

// Enum Variants

enumVariant
    : IDENTIFIER ('(' typeList ')')?  ('=' expression)?
    ;

// Trait Members

traitMember
    : traitMethod
    | associatedType
    | associatedConst
    ;

traitMethod
    : FN IDENTIFIER genericParams? '(' parameterList? ')' returnType? (block | ';')
    ;

associatedType
    : TYPE IDENTIFIER traitBounds? ';'
    ;

associatedConst
    : CONST IDENTIFIER ':' typeExpression ';'
    ;

// Impl Members

implMember
    : methodDeclaration
    | associatedTypeImpl
    | associatedConstImpl
    ;

associatedTypeImpl
    : TYPE IDENTIFIER '=' typeExpression ';'
    ;

associatedConstImpl
    : CONST IDENTIFIER ':' typeExpression '=' expression ';'
    ;

// Parameters

parameterList
    : parameter (',' parameter)*
    ;

parameter
    : IDENTIFIER ':' typeExpression ('=' expression)?
    ;

// Type System

typeAnnotation
    : ':' typeExpression
    ;

returnType
    : '->' typeExpression
    ;

typeExpression
    : primitiveType
    | TYPE_IDENTIFIER genericArgs?
    | '[' typeExpression ']'                    // Array type
    | '(' typeList? ')'                         // Tuple type
    | '&' MUT? typeExpression                   // Reference type
    | typeExpression '?'                        // Optional type
    | FN '(' typeList? ')' '->' typeExpression  // Function type
    | DYN traitBounds                           // Trait object
    ;

primitiveType
    : INT_TYPE | FLOAT_TYPE | DOUBLE_TYPE | BOOL_TYPE
    | STRING_TYPE | CHAR_TYPE | BYTE_TYPE | VOID_TYPE
    | ANY_TYPE | NEVER_TYPE
    ;

typeList
    : typeExpression (',' typeExpression)*
    ;

genericParams
    : '<' genericParam (',' genericParam)* '>'
    ;

genericParam
    : IDENTIFIER traitBounds?
    ;

genericArgs
    : '<' typeExpression (',' typeExpression)* '>'
    ;

traitBounds
    : ':' TYPE_IDENTIFIER ('+' TYPE_IDENTIFIER)*
    ;

whereClause
    : WHERE whereClauseItem (',' whereClauseItem)*
    ;

whereClauseItem
    : IDENTIFIER ':' TYPE_IDENTIFIER ('+' TYPE_IDENTIFIER)*
    ;

inheritance
    : ':' TYPE_IDENTIFIER (',' TYPE_IDENTIFIER)*
    ;

// Statements

expressionStatement
    : expression ';'
    ;

ifStatement
    : IF expression block (ELIF expression block)* (ELSE block)?
    ;

whileStatement
    : WHILE expression block
    ;

forStatement
    : FOR pattern IN expression block
    ;

loopStatement
    : LOOP block
    ;

matchStatement
    : MATCH expression '{' matchArm+ '}'
    ;

matchArm
    : pattern guard? '=>' (expression ','? | block)
    ;

guard
    : IF expression
    ;

returnStatement
    : RETURN expression? ';'
    ;

breakStatement
    : BREAK ';'
    ;

continueStatement
    : CONTINUE ';'
    ;

block
    : '{' statement* '}'
    ;

// Expressions

expression
    : assignmentExpression
    ;

assignmentExpression
    : conditionalExpression
    | conditionalExpression assignmentOperator assignmentExpression
    ;

conditionalExpression
    : logicalOrExpression
    | logicalOrExpression '?' expression ':' conditionalExpression
    ;

logicalOrExpression
    : logicalAndExpression
    | logicalOrExpression ('||' | OR) logicalAndExpression
    ;

logicalAndExpression
    : bitwiseOrExpression
    | logicalAndExpression ('&&' | AND) bitwiseOrExpression
    ;

bitwiseOrExpression
    : bitwiseXorExpression
    | bitwiseOrExpression '|' bitwiseXorExpression
    ;

bitwiseXorExpression
    : bitwiseAndExpression
    | bitwiseXorExpression '^' bitwiseAndExpression
    ;

bitwiseAndExpression
    : equalityExpression
    | bitwiseAndExpression '&' equalityExpression
    ;

equalityExpression
    : relationalExpression
    | equalityExpression ('==' | '!=' | '===' | '!==') relationalExpression
    ;

relationalExpression
    : rangeExpression
    | relationalExpression ('<' | '>' | '<=' | '>=') rangeExpression
    ;

rangeExpression
    : shiftExpression
    | shiftExpression ('..' | '..=') shiftExpression
    ;

shiftExpression
    : additiveExpression
    | shiftExpression ('<<' | '>>') additiveExpression
    ;

additiveExpression
    : multiplicativeExpression
    | additiveExpression ('+' | '-') multiplicativeExpression
    ;

multiplicativeExpression
    : powerExpression
    | multiplicativeExpression ('*' | '/' | '%') powerExpression
    ;

powerExpression
    : unaryExpression
    | unaryExpression '**' powerExpression
    ;

unaryExpression
    : postfixExpression
    | ('-' | '!' | NOT | '~' | '&' | '*') unaryExpression
    | AWAIT unaryExpression
    ;

postfixExpression
    : primaryExpression
    | postfixExpression '(' argumentList? ')'           // Call
    | postfixExpression '[' expression ']'              // Index
    | postfixExpression '.' IDENTIFIER                  // Member access
    | postfixExpression '?.' IDENTIFIER                 // Optional chaining
    | postfixExpression '?'                             // Try operator
    ;

primaryExpression
    : literal
    | IDENTIFIER
    | SELF
    | THIS
    | SUPER
    | '(' expression ')'
    | arrayLiteral
    | tupleLiteral
    | structLiteral
    | lambdaExpression
    | ifExpression
    | matchExpression
    | blockExpression
    ;

// Literals

literal
    : INTEGER
    | FLOAT
    | STRING
    | TEMPLATE_STRING
    | CHAR
    | TRUE
    | FALSE
    | NULL_LIT
    | NIL
    ;

arrayLiteral
    : '[' (expression (',' expression)* ','?)? ']'
    ;

tupleLiteral
    : '(' expression (',' expression)+ ','? ')'
    ;

structLiteral
    : TYPE_IDENTIFIER '{' structFieldInit (',' structFieldInit)* ','? '}'
    ;

structFieldInit
    : IDENTIFIER ':' expression
    | IDENTIFIER  // Shorthand
    ;

lambdaExpression
    : '|' parameterList? '|' (expression | block)
    | '||' (expression | block)
    ;

ifExpression
    : IF expression block (ELSE block)?
    ;

matchExpression
    : MATCH expression '{' matchArm+ '}'
    ;

blockExpression
    : block
    ;

// Patterns

pattern
    : literalPattern
    | identifierPattern
    | wildcardPattern
    | tuplePattern
    | structPattern
    | enumPattern
    | rangePattern
    | orPattern
    | bindingPattern
    ;

literalPattern
    : literal
    ;

identifierPattern
    : IDENTIFIER
    ;

wildcardPattern
    : '_'
    ;

tuplePattern
    : '(' pattern (',' pattern)* ')'
    ;

structPattern
    : TYPE_IDENTIFIER '{' structPatternField (',' structPatternField)* '}'
    ;

structPatternField
    : IDENTIFIER ':' pattern
    | IDENTIFIER
    | '..'
    ;

enumPattern
    : TYPE_IDENTIFIER '::' IDENTIFIER ('(' pattern (',' pattern)* ')')?
    ;

rangePattern
    : expression ('..' | '..=') expression
    ;

orPattern
    : pattern ('|' pattern)+
    ;

bindingPattern
    : IDENTIFIER '@' pattern
    ;

// Helpers

argumentList
    : expression (',' expression)*
    ;

modulePath
    : IDENTIFIER ('::' IDENTIFIER)*
    ;

importList
    : IDENTIFIER (',' IDENTIFIER)*
    ;

assignmentOperator
    : '=' | '+=' | '-=' | '*=' | '/=' | '%='
    | '&=' | '|=' | '^=' | '<<=' | '>>='
    ;

modifier
    : PUB | PRIV | PROTECTED | STATIC | FINAL
    | ABSTRACT | VIRTUAL | OVERRIDE | MUT | REF
    ;

// Lexer Rules

// Keywords
IF          : 'if';
ELSE        : 'else';
ELIF        : 'elif';
MATCH       : 'match';
CASE        : 'case';
FOR         : 'for';
WHILE       : 'while';
LOOP        : 'loop';
BREAK       : 'break';
CONTINUE    : 'continue';
RETURN      : 'return';

LET         : 'let';
CONST       : 'const';
VAR         : 'var';
FN          : 'fn';
FUNC        : 'func';
CLASS       : 'class';
STRUCT      : 'struct';
ENUM        : 'enum';
INTERFACE   : 'interface';
TRAIT       : 'trait';
IMPL        : 'impl';
TYPE        : 'type';
ALIAS       : 'alias';

PUB         : 'pub';
PRIV        : 'priv';
PROTECTED   : 'protected';
STATIC      : 'static';
FINAL       : 'final';
ABSTRACT    : 'abstract';
VIRTUAL     : 'virtual';
OVERRIDE    : 'override';
MUT         : 'mut';
REF         : 'ref';

USE         : 'use';
IMPORT      : 'import';
EXPORT      : 'export';
FROM        : 'from';
AS          : 'as';
IN          : 'in';
IS          : 'is';
NEW         : 'new';
DELETE      : 'delete';
SIZEOF      : 'sizeof';
TYPEOF      : 'typeof';
INSTANCEOF  : 'instanceof';

ASYNC       : 'async';
AWAIT       : 'await';
YIELD       : 'yield';

SELF        : 'self';
THIS        : 'this';
SUPER       : 'super';
BASE        : 'base';
WHERE       : 'where';
MOVE        : 'move';
COPY        : 'copy';
UNSAFE      : 'unsafe';
EXTERN      : 'extern';
DYN         : 'dyn';
MACRO       : 'macro';
DERIVE      : 'derive';
INIT        : 'init';

// Types
INT_TYPE    : 'int';
FLOAT_TYPE  : 'float';
DOUBLE_TYPE : 'double';
BOOL_TYPE   : 'bool';
STRING_TYPE : 'string';
CHAR_TYPE   : 'char';
BYTE_TYPE   : 'byte';
VOID_TYPE   : 'void';
ANY_TYPE    : 'any';
NEVER_TYPE  : 'never';

// Literals
TRUE        : 'true';
FALSE       : 'false';
NULL_LIT    : 'null';
NIL         : 'nil';

// Operators
AND         : 'and';
OR          : 'or';
NOT         : 'not';
XOR         : 'xor';

// Identifiers
TYPE_IDENTIFIER : [A-Z][a-zA-Z0-9_]*;
IDENTIFIER      : [a-z_][a-zA-Z0-9_]*;

// Literals
INTEGER         : [0-9]+ | '0x'[0-9A-Fa-f]+ | '0b'[01]+ | '0o'[0-7]+;
FLOAT           : [0-9]+ '.' [0-9]+ ([eE][+-]?[0-9]+)?;
STRING          : '"' (~["\\\r\n] | EscapeSequence)* '"';
TEMPLATE_STRING : '`' (~[`\\] | EscapeSequence)* '`';
CHAR            : '\'' (~['\\\r\n] | EscapeSequence) '\'';

fragment EscapeSequence
    : '\\' [nrt\\"'0]
    | '\\x' [0-9A-Fa-f][0-9A-Fa-f]
    | '\\u' [0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f]
    ;

// Comments
LINE_COMMENT    : '//' ~[\r\n]* -> skip;
BLOCK_COMMENT   : '/*' .*? '*/' -> skip;

// Whitespace
WS              : [ \t\r\n]+ -> skip;


/* ============================================================================
 * EXTENDED GRAMMAR RULES - ADVANCED FEATURES
 * ============================================================================
 */

// Async/Await Extensions
asyncExpression
    : ASYNC block
    | ASYNC lambdaExpression
    ;

awaitExpression
    : AWAIT expression
    ;

// Generator Expressions
generatorExpression
    : YIELD expression
    | YIELD FROM expression
    ;

// Decorator Syntax
decoratorList
    : decorator+
    ;

decorator
    : '@' IDENTIFIER ('(' argumentList? ')')?
    | '@' modulePath ('(' argumentList? ')')?
    ;

decoratedDeclaration
    : decoratorList (functionDeclaration | classDeclaration | methodDeclaration)
    ;

// With Statement (Context Managers)
withStatement
    : WITH withItem (',' withItem)* block
    ;

withItem
    : expression (AS IDENTIFIER)?
    ;

// Comprehensions
listComprehension
    : '[' expression comprehensionFor comprehensionClause* ']'
    ;

setComprehension
    : '{' expression comprehensionFor comprehensionClause* '}'
    ;

dictComprehension
    : '{' expression ':' expression comprehensionFor comprehensionClause* '}'
    ;

comprehensionFor
    : FOR IDENTIFIER IN expression
    ;

comprehensionClause
    : comprehensionFor
    | IF expression
    ;

// Advanced Pattern Matching
advancedPattern
    : arrayPattern
    | objectPattern
    | regexPattern
    | typePattern
    | guardPattern
    ;

arrayPattern
    : '[' pattern (',' pattern)* (',' '...' IDENTIFIER)? ']'
    ;

objectPattern
    : '{' objectPatternProperty (',' objectPatternProperty)* '}'
    ;

objectPatternProperty
    : IDENTIFIER (':' pattern)?
    | '...' IDENTIFIER
    ;

regexPattern
    : '/' regexBody '/' regexFlags?
    ;

typePattern
    : pattern ':' typeExpression
    ;

guardPattern
    : pattern IF expression
    ;

// Type System Extensions
advancedType
    : unionType
    | intersectionType
    | conditionalType
    | mappedType
    | templateLiteralType
    | indexedAccessType
    | keyofType
    ;

unionType
    : typeExpression ('|' typeExpression)+
    ;

intersectionType
    : typeExpression ('&' typeExpression)+
    ;

conditionalType
    : typeExpression EXTENDS typeExpression '?' typeExpression ':' typeExpression
    ;

mappedType
    : '{' '[' IDENTIFIER IN typeExpression ']' ':' typeExpression '}'
    ;

templateLiteralType
    : '`' templateLiteralTypeSpan* '`'
    ;

templateLiteralTypeSpan
    : TEMPLATE_STRING_PART
    | '${' typeExpression '}'
    ;

indexedAccessType
    : typeExpression '[' typeExpression ']'
    ;

keyofType
    : KEYOF typeExpression
    ;

// Operator Overloading
operatorOverload
    : OPERATOR overloadableOperator '(' parameterList ')' returnType? block
    ;

overloadableOperator
    : '+' | '-' | '*' | '/' | '%' | '**'
    | '==' | '!=' | '<' | '>' | '<=' | '>='
    | '&' | '|' | '^' | '~' | '<<' | '>>'
    | '[]' | '()'
    ;

// Extension Methods
extensionDeclaration
    : EXTENSION TYPE_IDENTIFIER '{' extensionMember* '}'
    ;

extensionMember
    : methodDeclaration
    | propertyDeclaration
    ;

propertyDeclaration
    : modifier* IDENTIFIER ':' typeExpression propertyAccessors
    ;

propertyAccessors
    : '{' getter setter? '}'
    | '{' setter getter? '}'
    ;

getter
    : GET block
    | GET '=>' expression ';'
    ;

setter
    : SET block
    | SET '(' IDENTIFIER ')' block
    ;

// Partial Classes
partialClassDeclaration
    : PARTIAL CLASS TYPE_IDENTIFIER genericParams? inheritance? '{' classMember* '}'
    ;

// Record Types
recordDeclaration
    : RECORD TYPE_IDENTIFIER genericParams? '(' parameterList ')' inheritance? '{' recordMember* '}'
    ;

recordMember
    : methodDeclaration
    | propertyDeclaration
    ;

// Namespace Declaration
namespaceDeclaration
    : NAMESPACE modulePath '{' statement* '}'
    ;

// Alias Declarations
aliasDeclaration
    : ALIAS IDENTIFIER '=' typeExpression ';'
    | ALIAS IDENTIFIER '=' modulePath ';'
    ;

// Inline Assembly
inlineAssembly
    : ASM '(' STRING (',' asmOperand)* ')' ';'
    ;

asmOperand
    : STRING ':' expression
    ;

// Compile-Time Evaluation
compileTimeBlock
    : COMPTIME block
    ;

compileTimeExpression
    : COMPTIME expression
    ;

// Reflection and Metaprogramming
reflectionExpression
    : TYPEOF expression
    | NAMEOF expression
    | SIZEOF typeExpression
    | ALIGNOF typeExpression
    | OFFSETOF '(' typeExpression ',' IDENTIFIER ')'
    ;

// Contract Programming
contractDeclaration
    : requiresClause
    | ensuresClause
    | invariantClause
    ;

requiresClause
    : REQUIRES expression ';'
    ;

ensuresClause
    : ENSURES expression ';'
    ;

invariantClause
    : INVARIANT expression ';'
    ;

// Advanced Loop Constructs
doWhileStatement
    : DO block WHILE expression ';'
    ;

forEachStatement
    : FOREACH '(' typeExpression? IDENTIFIER IN expression ')' block
    ;

rangeForStatement
    : FOR '(' IDENTIFIER IN expression '..' expression ')' block
    ;

// Switch Expression (as expression, not statement)
switchExpression
    : SWITCH expression '{' switchExpressionArm (',' switchExpressionArm)* ','? '}'
    ;

switchExpressionArm
    : pattern '=>' expression
    ;

// Null Safety Operators
nullCoalesceExpression
    : expression '??' expression
    ;

nullConditionalExpression
    : expression '?.' IDENTIFIER
    | expression '?[' expression ']'
    ;

// Spread and Rest Operators
spreadExpression
    : '...' expression
    ;

restParameter
    : '...' IDENTIFIER
    ;

// Destructuring Assignment
destructuringAssignment
    : arrayDestructuring '=' expression ';'
    | objectDestructuring '=' expression ';'
    ;

arrayDestructuring
    : '[' destructuringElement (',' destructuringElement)* ']'
    ;

objectDestructuring
    : '{' destructuringProperty (',' destructuringProperty)* '}'
    ;

destructuringElement
    : IDENTIFIER
    | arrayDestructuring
    | objectDestructuring
    | '...' IDENTIFIER
    ;

destructuringProperty
    : IDENTIFIER (':' (IDENTIFIER | destructuringPattern))?
    | '...' IDENTIFIER
    ;

destructuringPattern
    : arrayDestructuring
    | objectDestructuring
    ;

// Pipeline Operator
pipelineExpression
    : expression '|>' expression
    ;

// Composition Operator
compositionExpression
    : expression '<<' expression
    | expression '>>' expression
    ;

// Additional Keywords
KEYOF       : 'keyof';
OPERATOR    : 'operator';
EXTENSION   : 'extension';
PARTIAL     : 'partial';
RECORD      : 'record';
NAMESPACE   : 'namespace';
ASM         : 'asm';
COMPTIME    : 'comptime';
NAMEOF      : 'nameof';
ALIGNOF     : 'alignof';
OFFSETOF    : 'offsetof';
REQUIRES    : 'requires';
ENSURES     : 'ensures';
INVARIANT   : 'invariant';
FOREACH     : 'foreach';
GET         : 'get';
SET         : 'set';
EXTENDS     : 'extends';

// Additional Tokens
TEMPLATE_STRING_PART : '`' (~['`$\\] | '\\' .)* ('${' | '`');

// Error Recovery
errorRecovery
    : ERROR_TOKEN
    ;

ERROR_TOKEN : . ;

