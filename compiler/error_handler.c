/* ============================================================================
 * L# ERROR HANDLER IMPLEMENTATION
 * Standard compiler error detection and reporting
 * ============================================================================ */

#include "error_handler.h"
#include <ctype.h>

/* Global error handler instance */
ErrorHandler *global_error_handler = NULL;

/* External variables from lexer/parser */
extern int yylineno;
extern int yycolumn;
extern char *yytext;
extern FILE *yyin;

/* ============================================================================
 * ERROR HANDLER CORE FUNCTIONS
 * ============================================================================ */

ErrorHandler* error_handler_init(void) {
    ErrorHandler *handler = (ErrorHandler*)malloc(sizeof(ErrorHandler));
    if (!handler) {
        fprintf(stderr, "Fatal: Cannot allocate error handler\n");
        exit(1);
    }
    
    handler->errors = NULL;
    handler->last_error = NULL;
    handler->error_count = 0;
    handler->warning_count = 0;
    handler->show_colors = 1;
    handler->output = stderr;
    
    global_error_handler = handler;
    return handler;
}

void error_handler_destroy(ErrorHandler *handler) {
    if (!handler) return;
    
    LSharpError *current = handler->errors;
    while (current) {
        LSharpError *next = current->next;
        free(current->message);
        free(current->suggestion);
        free(current->code_snippet);
        free(current);
        current = next;
    }
    
    free(handler);
    if (global_error_handler == handler) {
        global_error_handler = NULL;
    }
}

void report_error(ErrorHandler *handler, ErrorType type, ErrorSeverity severity,
                 const char *filename, int line, int column,
                 const char *message, const char *suggestion) {
    if (!handler) handler = global_error_handler;
    if (!handler) return;
    
    LSharpError *error = (LSharpError*)malloc(sizeof(LSharpError));
    if (!error) return;
    
    error->type = type;
    error->severity = severity;
    error->message = strdup(message);
    error->location.filename = filename;
    error->location.line = line;
    error->location.column = column;
    error->location.length = 1;
    error->suggestion = suggestion ? strdup(suggestion) : NULL;
    error->code_snippet = NULL;
    error->next = NULL;
    
    if (handler->last_error) {
        handler->last_error->next = error;
    } else {
        handler->errors = error;
    }
    handler->last_error = error;
    
    if (severity == SEVERITY_ERROR) {
        handler->error_count++;
    } else if (severity == SEVERITY_WARNING) {
        handler->warning_count++;
    }
}

void yyerror(const char *msg) {
    if (!global_error_handler) {
        fprintf(stderr, "Parse error: %s\n", msg);
        return;
    }
    
    report_error(global_error_handler, ERROR_SYNTAX, SEVERITY_ERROR,
                "input", yylineno, yycolumn, msg, NULL);
}

void error_printf(ErrorType type, ErrorSeverity severity,
                 const char *filename, int line, int column,
                 const char *format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    report_error(global_error_handler, type, severity,
                filename, line, column, buffer, NULL);
}

void print_error(LSharpError *error, int show_colors) {
    if (!error) return;
    
    const char *severity_str;
    const char *color = "";
    const char *reset = show_colors ? COLOR_RESET : "";
    
    switch (error->severity) {
        case SEVERITY_ERROR:
            severity_str = "ERROR";
            color = show_colors ? COLOR_RED : "";
            break;
        case SEVERITY_WARNING:
            severity_str = "WARNING";
            color = show_colors ? COLOR_YELLOW : "";
            break;
        case SEVERITY_INFO:
            severity_str = "INFO";
            color = show_colors ? COLOR_CYAN : "";
            break;
        case SEVERITY_HINT:
            severity_str = "HINT";
            color = show_colors ? COLOR_GREEN : "";
            break;
        default:
            severity_str = "UNKNOWN";
    }
    
    const char *type_str;
    switch (error->type) {
        case ERROR_SYNTAX: type_str = "SyntaxError"; break;
        case ERROR_SEMANTIC: type_str = "SemanticError"; break;
        case ERROR_TYPE: type_str = "TypeError"; break;
        case ERROR_RUNTIME: type_str = "RuntimeError"; break;
        case ERROR_LOGIC: type_str = "LogicError"; break;
        case ERROR_IMPORT: type_str = "ImportError"; break;
        case ERROR_COMPONENT: type_str = "ComponentError"; break;
        case ERROR_STATE: type_str = "StateError"; break;
        case ERROR_LIFECYCLE: type_str = "LifecycleError"; break;
        default: type_str = "Error";
    }
    
    fprintf(stderr, "%s%s%s: %s\n", color, severity_str, reset, type_str);
    fprintf(stderr, "  %s\n", error->message);
    fprintf(stderr, "  at %s:%d:%d\n", 
            error->location.filename, 
            error->location.line, 
            error->location.column);
    
    if (error->code_snippet) {
        fprintf(stderr, "\n  %d | %s\n", error->location.line, error->code_snippet);
        fprintf(stderr, "  %*s | %*s^\n", 
                (int)strlen(error->code_snippet), "",
                error->location.column, "");
    }
    
    if (error->suggestion) {
        fprintf(stderr, "\n  💡 Suggestion: %s\n", error->suggestion);
    }
    
    fprintf(stderr, "\n");
}

void print_errors(ErrorHandler *handler) {
    if (!handler) handler = global_error_handler;
    if (!handler) return;
    
    LSharpError *current = handler->errors;
    while (current) {
        print_error(current, handler->show_colors);
        current = current->next;
    }
    
    if (handler->error_count > 0 || handler->warning_count > 0) {
        fprintf(stderr, "Found %d error(s), %d warning(s)\n",
                handler->error_count, handler->warning_count);
    }
}

int has_errors(ErrorHandler *handler) {
    if (!handler) handler = global_error_handler;
    return handler ? handler->error_count > 0 : 0;
}

int get_error_count(ErrorHandler *handler) {
    if (!handler) handler = global_error_handler;
    return handler ? handler->error_count : 0;
}

int get_warning_count(ErrorHandler *handler) {
    if (!handler) handler = global_error_handler;
    return handler ? handler->warning_count : 0;
}

void clear_errors(ErrorHandler *handler) {
    if (!handler) handler = global_error_handler;
    if (!handler) return;
    
    LSharpError *current = handler->errors;
    while (current) {
        LSharpError *next = current->next;
        free(current->message);
        free(current->suggestion);
        free(current->code_snippet);
        free(current);
        current = next;
    }
    
    handler->errors = NULL;
    handler->last_error = NULL;
    handler->error_count = 0;
    handler->warning_count = 0;
}

/* ============================================================================
 * SYNTAX ERROR DETECTION
 * ============================================================================ */

void check_balanced_braces(const char *source, const char *filename) {
    typedef struct {
        char brace;
        int line;
        int column;
    } BraceInfo;
    
    BraceInfo stack[1000];
    int stack_top = -1;
    
    int line = 1, column = 0;
    const char *p = source;
    
    while (*p) {
        column++;
        
        if (*p == '\n') {
            line++;
            column = 0;
        }
        
        // Skip comments
        if (*p == '/' && *(p+1) == '/') {
            while (*p && *p != '\n') p++;
            continue;
        }
        
        // Skip strings
        if (*p == '"' || *p == '\'') {
            char quote = *p++;
            while (*p && *p != quote) {
                if (*p == '\\') p++;
                p++;
            }
            if (*p) p++;
            continue;
        }
        
        // Check opening braces
        if (*p == '(' || *p == '[' || *p == '{') {
            if (stack_top < 999) {
                stack[++stack_top].brace = *p;
                stack[stack_top].line = line;
                stack[stack_top].column = column;
            }
        }
        // Check closing braces
        else if (*p == ')' || *p == ']' || *p == '}') {
            char expected;
            if (*p == ')') expected = '(';
            else if (*p == ']') expected = '[';
            else expected = '{';
            
            if (stack_top < 0) {
                char msg[256];
                snprintf(msg, sizeof(msg), "Unexpected closing '%c'", *p);
                report_error(global_error_handler, ERROR_SYNTAX, SEVERITY_ERROR,
                           filename, line, column, msg, "Remove this or add matching opening brace");
            } else if (stack[stack_top].brace != expected) {
                char msg[256];
                char closing = (expected == '(') ? ')' : (expected == '[') ? ']' : '}';
                snprintf(msg, sizeof(msg), 
                        "Mismatched braces: '%c' opened at line %d but '%c' found",
                        stack[stack_top].brace, stack[stack_top].line, *p);
                report_error(global_error_handler, ERROR_SYNTAX, SEVERITY_ERROR,
                           filename, line, column, msg, NULL);
                stack_top--;
            } else {
                stack_top--;
            }
        }
        
        p++;
    }
    
    // Check for unclosed braces
    while (stack_top >= 0) {
        char msg[256];
        char closing = (stack[stack_top].brace == '(') ? ')' : 
                      (stack[stack_top].brace == '[') ? ']' : '}';
        snprintf(msg, sizeof(msg), "Unclosed '%c'", stack[stack_top].brace);
        
        char sugg[128];
        snprintf(sugg, sizeof(sugg), "Add closing '%c'", closing);
        
        report_error(global_error_handler, ERROR_SYNTAX, SEVERITY_ERROR,
                   filename, stack[stack_top].line, stack[stack_top].column,
                   msg, sugg);
        stack_top--;
    }
}

void check_unclosed_strings(const char *source, const char *filename) {
    int line = 1, column = 0;
    const char *p = source;
    
    while (*p) {
        column++;
        
        if (*p == '\n') {
            line++;
            column = 0;
            p++;
            continue;
        }
        
        if (*p == '"' || *p == '\'') {
            char quote = *p;
            int start_col = column;
            p++;
            column++;
            
            int closed = 0;
            while (*p && *p != '\n') {
                if (*p == '\\') {
                    p++;
                    column++;
                    if (*p) {
                        p++;
                        column++;
                    }
                    continue;
                }
                if (*p == quote) {
                    closed = 1;
                    break;
                }
                p++;
                column++;
            }
            
            if (!closed) {
                char msg[256];
                snprintf(msg, sizeof(msg), "Unclosed string literal");
                char sugg[128];
                snprintf(sugg, sizeof(sugg), "Add closing %c to complete the string", quote);
                report_error(global_error_handler, ERROR_SYNTAX, SEVERITY_ERROR,
                           filename, line, start_col, msg, sugg);
            }
        }
        
        p++;
    }
}

void check_missing_semicolons(const char *source, const char *filename) {
    int line = 1;
    const char *line_start = source;
    const char *p = source;
    
    while (*p) {
        if (*p == '\n') {
            // Check previous line
            const char *line_end = p;
            while (line_end > line_start && isspace(*(line_end-1))) {
                line_end--;
            }
            
            if (line_end > line_start) {
                char last_char = *(line_end - 1);
                
                // Check if line should end with semicolon
                const char *trimmed = line_start;
                while (trimmed < line_end && isspace(*trimmed)) trimmed++;
                
                if (strncmp(trimmed, "const ", 6) == 0 ||
                    strncmp(trimmed, "let ", 4) == 0 ||
                    strncmp(trimmed, "var ", 4) == 0 ||
                    strncmp(trimmed, "return ", 7) == 0 ||
                    strncmp(trimmed, "import ", 7) == 0 ||
                    strncmp(trimmed, "export ", 7) == 0) {
                    
                    if (last_char != ';' && last_char != '{' && last_char != '}') {
                        report_error(global_error_handler, ERROR_SYNTAX, SEVERITY_WARNING,
                                   filename, line, (int)(line_end - line_start),
                                   ERR_SYNTAX_MISSING_SEMICOLON, SUGG_ADD_SEMICOLON);
                    }
                }
            }
            
            line++;
            line_start = p + 1;
        }
        p++;
    }
}

void check_jsx_syntax(const char *source, const char *filename) {
    int line = 1;
    const char *p = source;
    
    while (*p) {
        if (*p == '<' && isupper(*(p+1))) {
            // Found JSX opening tag
            const char *tag_start = p + 1;
            const char *tag_end = tag_start;
            
            while (*tag_end && (isalnum(*tag_end) || *tag_end == '_')) {
                tag_end++;
            }
            
            char tag_name[128];
            int tag_len = tag_end - tag_start;
            if (tag_len < sizeof(tag_name)) {
                strncpy(tag_name, tag_start, tag_len);
                tag_name[tag_len] = '\0';
                
                // Check for self-closing or closing tag
                const char *rest = tag_end;
                while (*rest && *rest != '>' && *rest != '\n') rest++;
                
                if (*rest == '>' && *(rest-1) != '/') {
                    // Not self-closing, look for closing tag
                    char closing_tag[140];
                    snprintf(closing_tag, sizeof(closing_tag), "</%s>", tag_name);
                    
                    if (!strstr(rest, closing_tag)) {
                        char msg[256];
                        snprintf(msg, sizeof(msg), "JSX tag <%s> is not closed", tag_name);
                        char sugg[256];
                        snprintf(sugg, sizeof(sugg), "Add </%s> or make it self-closing with />", tag_name);
                        report_error(global_error_handler, ERROR_SYNTAX, SEVERITY_ERROR,
                                   filename, line, (int)(p - source), msg, sugg);
                    }
                }
            }
        }
        
        if (*p == '\n') line++;
        p++;
    }
}

/* ============================================================================
 * COMPONENT ERROR DETECTION
 * ============================================================================ */

void check_component_naming(const char *name, const char *filename, int line) {
    if (!name || !*name) return;
    
    if (islower(name[0])) {
        char msg[256];
        snprintf(msg, sizeof(msg), ERR_COMPONENT_LOWERCASE, name);
        
        char suggestion[256];
        char capitalized[128];
        strncpy(capitalized, name, sizeof(capitalized) - 1);
        capitalized[0] = toupper(capitalized[0]);
        snprintf(suggestion, sizeof(suggestion), SUGG_RENAME_COMPONENT, capitalized);
        
        report_error(global_error_handler, ERROR_COMPONENT, SEVERITY_ERROR,
                   filename, line, 0, msg, suggestion);
    }
}

/* ============================================================================
 * STATE ERROR DETECTION
 * ============================================================================ */

void check_usestate_syntax(const char *line, const char *filename, int line_num) {
    if (!strstr(line, "useState")) return;
    
    // Check for proper destructuring pattern
    if (!strstr(line, "const [") || !strstr(line, "] = useState")) {
        report_error(global_error_handler, ERROR_STATE, SEVERITY_WARNING,
                   filename, line_num, 0,
                   ERR_STATE_INVALID_USESTATE, SUGG_FIX_USESTATE);
    }
}

/* ============================================================================
 * IMPORT ERROR DETECTION
 * ============================================================================ */

void check_import_syntax(const char *line, const char *filename, int line_num) {
    if (!strstr(line, "import")) return;
    
    int has_from = strstr(line, "from") != NULL;
    int has_direct = strstr(line, "direct") != NULL;
    
    if (has_from && has_direct) {
        report_error(global_error_handler, ERROR_IMPORT, SEVERITY_ERROR,
                   filename, line_num, 0,
                   ERR_IMPORT_CONFLICT, SUGG_FIX_IMPORT);
    }
}

/* ============================================================================
 * LOGIC ERROR DETECTION
 * ============================================================================ */

void check_assignment_in_condition(const char *line, const char *filename, int line_num) {
    const char *if_pos = strstr(line, "if");
    if (!if_pos) return;
    
    const char *paren_start = strchr(if_pos, '(');
    if (!paren_start) return;
    
    const char *paren_end = strchr(paren_start, ')');
    if (!paren_end) return;
    
    // Check for single '=' between parentheses
    const char *p = paren_start + 1;
    while (p < paren_end) {
        if (*p == '=' && *(p-1) != '=' && *(p-1) != '!' && 
            *(p-1) != '<' && *(p-1) != '>' && *(p+1) != '=') {
            report_error(global_error_handler, ERROR_LOGIC, SEVERITY_WARNING,
                       filename, line_num, (int)(p - line),
                       ERR_LOGIC_ASSIGNMENT_IN_CONDITION, SUGG_USE_COMPARISON);
            break;
        }
        p++;
    }
}

/* ============================================================================
 * UTILITY FUNCTIONS
 * ============================================================================ */

void run_all_syntax_checks(const char *source, const char *filename) {
    check_balanced_braces(source, filename);
    check_unclosed_strings(source, filename);
    check_missing_semicolons(source, filename);
    check_jsx_syntax(source, filename);
    
    // Line-by-line checks
    int line_num = 1;
    const char *line_start = source;
    const char *p = source;
    
    while (*p) {
        if (*p == '\n') {
            char line_buffer[1024];
            int line_len = p - line_start;
            if (line_len < sizeof(line_buffer)) {
                strncpy(line_buffer, line_start, line_len);
                line_buffer[line_len] = '\0';
                
                check_usestate_syntax(line_buffer, filename, line_num);
                check_import_syntax(line_buffer, filename, line_num);
                check_assignment_in_condition(line_buffer, filename, line_num);
            }
            
            line_num++;
            line_start = p + 1;
        }
        p++;
    }
}
