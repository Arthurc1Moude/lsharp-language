/* ============================================================================
 * L# ERROR HANDLER - Syntax, Runtime, and Logic Error Detection
 * Official error checking system using standard compiler techniques
 * ============================================================================ */

#ifndef LSHARP_ERROR_HANDLER_H
#define LSHARP_ERROR_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* Error Types */
typedef enum {
    ERROR_SYNTAX,
    ERROR_SEMANTIC,
    ERROR_TYPE,
    ERROR_RUNTIME,
    ERROR_LOGIC,
    ERROR_IMPORT,
    ERROR_COMPONENT,
    ERROR_STATE,
    ERROR_LIFECYCLE,
    ERROR_WARNING,
    ERROR_INFO
} ErrorType;

/* Error Severity */
typedef enum {
    SEVERITY_ERROR,
    SEVERITY_WARNING,
    SEVERITY_INFO,
    SEVERITY_HINT
} ErrorSeverity;

/* Error Location */
typedef struct {
    const char *filename;
    int line;
    int column;
    int length;
} ErrorLocation;

/* Error Structure */
typedef struct LSharpError {
    ErrorType type;
    ErrorSeverity severity;
    char *message;
    ErrorLocation location;
    char *suggestion;
    char *code_snippet;
    struct LSharpError *next;
} LSharpError;

/* Error Handler Context */
typedef struct {
    LSharpError *errors;
    LSharpError *last_error;
    int error_count;
    int warning_count;
    int show_colors;
    FILE *output;
} ErrorHandler;

/* Global error handler */
extern ErrorHandler *global_error_handler;

/* ============================================================================
 * ERROR HANDLER FUNCTIONS
 * ============================================================================ */

/* Initialize error handler */
ErrorHandler* error_handler_init(void);

/* Destroy error handler */
void error_handler_destroy(ErrorHandler *handler);

/* Report error */
void report_error(ErrorHandler *handler, ErrorType type, ErrorSeverity severity,
                 const char *filename, int line, int column,
                 const char *message, const char *suggestion);

/* Report syntax error (called by parser) */
void yyerror(const char *msg);

/* Report error with format string */
void error_printf(ErrorType type, ErrorSeverity severity,
                 const char *filename, int line, int column,
                 const char *format, ...);

/* Print all errors */
void print_errors(ErrorHandler *handler);

/* Print single error */
void print_error(LSharpError *error, int show_colors);

/* Check if has errors */
int has_errors(ErrorHandler *handler);

/* Get error count */
int get_error_count(ErrorHandler *handler);

/* Get warning count */
int get_warning_count(ErrorHandler *handler);

/* Clear all errors */
void clear_errors(ErrorHandler *handler);

/* ============================================================================
 * SYNTAX ERROR DETECTION
 * ============================================================================ */

/* Check balanced braces */
void check_balanced_braces(const char *source, const char *filename);

/* Check unclosed strings */
void check_unclosed_strings(const char *source, const char *filename);

/* Check missing semicolons */
void check_missing_semicolons(const char *source, const char *filename);

/* Check JSX syntax */
void check_jsx_syntax(const char *source, const char *filename);

/* ============================================================================
 * SEMANTIC ERROR DETECTION
 * ============================================================================ */

/* Check undefined variables */
void check_undefined_variables(void *ast, const char *filename);

/* Check type mismatches */
void check_type_mismatches(void *ast, const char *filename);

/* Check duplicate declarations */
void check_duplicate_declarations(void *ast, const char *filename);

/* ============================================================================
 * COMPONENT ERROR DETECTION
 * ============================================================================ */

/* Check component naming */
void check_component_naming(const char *name, const char *filename, int line);

/* Check missing render method */
void check_missing_render(void *component, const char *filename);

/* Check component lifecycle */
void check_component_lifecycle(void *component, const char *filename);

/* ============================================================================
 * STATE ERROR DETECTION
 * ============================================================================ */

/* Check useState syntax */
void check_usestate_syntax(const char *line, const char *filename, int line_num);

/* Check state mutations */
void check_state_mutations(void *ast, const char *filename);

/* ============================================================================
 * IMPORT ERROR DETECTION
 * ============================================================================ */

/* Check import syntax */
void check_import_syntax(const char *line, const char *filename, int line_num);

/* Check import conflicts */
void check_import_conflicts(const char *line, const char *filename, int line_num);

/* Check module not found */
void check_module_exists(const char *module_path, const char *filename, int line_num);

/* ============================================================================
 * LOGIC ERROR DETECTION
 * ============================================================================ */

/* Check assignment in condition */
void check_assignment_in_condition(const char *line, const char *filename, int line_num);

/* Check infinite loops */
void check_infinite_loops(void *ast, const char *filename);

/* Check unused variables */
void check_unused_variables(void *ast, const char *filename);

/* Check missing return */
void check_missing_return(void *function, const char *filename);

/* ============================================================================
 * RUNTIME ERROR DETECTION
 * ============================================================================ */

/* Check null pointer dereference */
void check_null_dereference(void *ast, const char *filename);

/* Check array bounds */
void check_array_bounds(void *ast, const char *filename);

/* Check division by zero */
void check_division_by_zero(void *ast, const char *filename);

/* ============================================================================
 * ERROR MESSAGES
 * ============================================================================ */

/* Common error messages */
#define ERR_SYNTAX_UNCLOSED_BRACE "Unclosed '%c' - expected closing '%c'"
#define ERR_SYNTAX_UNEXPECTED_TOKEN "Unexpected token '%s'"
#define ERR_SYNTAX_MISSING_SEMICOLON "Missing semicolon at end of statement"
#define ERR_SYNTAX_UNCLOSED_STRING "Unclosed string literal"
#define ERR_SYNTAX_INVALID_KEYWORD "Invalid keyword '%s'"

#define ERR_SEMANTIC_UNDEFINED_VAR "Variable '%s' is not defined"
#define ERR_SEMANTIC_DUPLICATE_DECL "Duplicate declaration of '%s'"
#define ERR_SEMANTIC_TYPE_MISMATCH "Type mismatch: expected '%s', got '%s'"

#define ERR_COMPONENT_LOWERCASE "Component name '%s' must start with uppercase letter"
#define ERR_COMPONENT_NO_RENDER "Component '%s' is missing render method"
#define ERR_COMPONENT_INVALID_LIFECYCLE "Invalid lifecycle method '%s'"

#define ERR_STATE_INVALID_USESTATE "useState must use array destructuring: const [value, setValue] = useState(initial)"
#define ERR_STATE_MUTATION "Direct state mutation detected - use setter function instead"

#define ERR_IMPORT_INVALID_SYNTAX "Invalid import syntax - use 'from { }' for cloud or 'direct' for native"
#define ERR_IMPORT_CONFLICT "Cannot use both 'from { }' and 'direct' in same import"
#define ERR_IMPORT_NOT_FOUND "Module '%s' not found"

#define ERR_LOGIC_ASSIGNMENT_IN_CONDITION "Assignment in condition - use '==' or '===' for comparison"
#define ERR_LOGIC_INFINITE_LOOP "Potential infinite loop without break statement"
#define ERR_LOGIC_UNUSED_VAR "Variable '%s' is declared but never used"
#define ERR_LOGIC_MISSING_RETURN "Function may be missing return statement"

#define ERR_RUNTIME_NULL_DEREF "Potential null pointer dereference"
#define ERR_RUNTIME_ARRAY_BOUNDS "Array index out of bounds"
#define ERR_RUNTIME_DIV_BY_ZERO "Division by zero"

/* Suggestions */
#define SUGG_ADD_CLOSING_BRACE "Add closing '%c' to match opening '%c'"
#define SUGG_ADD_SEMICOLON "Add ';' at the end of the statement"
#define SUGG_CLOSE_STRING "Add closing quote to complete the string"
#define SUGG_FIX_KEYWORD "Did you mean '%s'?"
#define SUGG_DECLARE_VAR "Declare variable before using it"
#define SUGG_RENAME_COMPONENT "Rename to '%s' (capitalize first letter)"
#define SUGG_ADD_RENDER "Add: render { <Container>...</Container> }"
#define SUGG_FIX_USESTATE "Use: const [value, setValue] = useState(initial)"
#define SUGG_USE_SETTER "Use setState() or setter function to update state"
#define SUGG_FIX_IMPORT "Use 'from { library }' for cloud OR 'direct \"./path\"' for native"
#define SUGG_USE_COMPARISON "Change '=' to '==' or '==='"
#define SUGG_ADD_BREAK "Add a break condition or use different loop structure"
#define SUGG_REMOVE_UNUSED "Remove unused variable or use it in your code"
#define SUGG_ADD_RETURN "Add return statement if function should return a value"

/* ============================================================================
 * COLOR CODES FOR TERMINAL OUTPUT
 * ============================================================================ */

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_BOLD    "\033[1m"

#endif /* LSHARP_ERROR_HANDLER_H */
