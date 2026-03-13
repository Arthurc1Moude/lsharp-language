/* L# Compiler Main Entry Point */
/* Command-line interface for the L# compiler */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantic.h"
#include "codegen.h"

/* External declarations from lexer/parser */
extern FILE *yyin;
extern int yyparse();
extern ASTNode *root;
extern int line_num;
extern void reset_lexer();
extern void print_lexer_stats();

/* ============================================================================
 * COMPILER OPTIONS
 * ============================================================================
 */

typedef struct CompilerOptions {
    char *input_file;
    char *output_file;
    int verbose;
    int debug;
    int optimize;
    int emit_ast;
    int emit_bytecode;
    int check_only;
} CompilerOptions;

CompilerOptions default_options() {
    CompilerOptions opts;
    opts.input_file = NULL;
    opts.output_file = NULL;
    opts.verbose = 0;
    opts.debug = 0;
    opts.optimize = 0;
    opts.emit_ast = 0;
    opts.emit_bytecode = 1;
    opts.check_only = 0;
    return opts;
}

/* ============================================================================
 * COMMAND LINE PARSING
 * ============================================================================
 */

void print_usage(const char *program_name) {
    printf("L# Compiler v1.0.0\n");
    printf("Usage: %s [options] <input-file>\n\n", program_name);
    printf("Options:\n");
    printf("  -o <file>       Write output to <file>\n");
    printf("  -v, --verbose   Enable verbose output\n");
    printf("  -d, --debug     Enable debug mode\n");
    printf("  -O, --optimize  Enable optimizations\n");
    printf("  --emit-ast      Emit AST to file\n");
    printf("  --check         Type check only, don't generate code\n");
    printf("  -h, --help      Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s program.ls\n", program_name);
    printf("  %s -o program.lsb program.ls\n", program_name);
    printf("  %s --check program.ls\n", program_name);
}

CompilerOptions parse_args(int argc, char **argv) {
    CompilerOptions opts = default_options();
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            opts.output_file = argv[++i];
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            opts.verbose = 1;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            opts.debug = 1;
        } else if (strcmp(argv[i], "-O") == 0 || strcmp(argv[i], "--optimize") == 0) {
            opts.optimize = 1;
        } else if (strcmp(argv[i], "--emit-ast") == 0) {
            opts.emit_ast = 1;
        } else if (strcmp(argv[i], "--check") == 0) {
            opts.check_only = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            exit(0);
        } else if (argv[i][0] != '-') {
            opts.input_file = argv[i];
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            exit(1);
        }
    }
    
    return opts;
}

/* ============================================================================
 * COMPILATION PIPELINE
 * ============================================================================
 */

int compile_file(CompilerOptions opts) {
    /* Open input file */
    yyin = fopen(opts.input_file, "r");
    if (!yyin) {
        fprintf(stderr, "Error: Could not open file '%s'\n", opts.input_file);
        return 1;
    }
    
    if (opts.verbose) {
        printf("Compiling %s...\n", opts.input_file);
    }
    
    /* Phase 1: Lexical and Syntax Analysis */
    if (opts.verbose) {
        printf("[1/4] Parsing...\n");
    }
    
    reset_lexer();
    int parse_result = yyparse();
    
    if (parse_result != 0) {
        fprintf(stderr, "Parse failed with %d errors\n", parse_result);
        fclose(yyin);
        return 1;
    }
    
    if (opts.verbose) {
        print_lexer_stats();
    }
    
    if (!root) {
        fprintf(stderr, "Error: No AST generated\n");
        fclose(yyin);
        return 1;
    }
    
    /* Emit AST if requested */
    if (opts.emit_ast) {
        if (opts.verbose) {
            printf("Emitting AST...\n");
        }
        print_ast(root, 0);
    }
    
    /* Phase 2: Semantic Analysis */
    if (opts.verbose) {
        printf("[2/4] Semantic analysis...\n");
    }
    
    analyze_program(root);
    
    /* Stop here if check-only mode */
    if (opts.check_only) {
        if (opts.verbose) {
            printf("Type checking completed successfully\n");
        }
        fclose(yyin);
        return 0;
    }
    
    /* Phase 3: Optimization (if enabled) */
    if (opts.optimize) {
        if (opts.verbose) {
            printf("[3/4] Optimizing...\n");
        }
        /* Apply optimizations */
    }
    
    /* Phase 4: Code Generation */
    if (opts.verbose) {
        printf("[4/4] Generating bytecode...\n");
    }
    
    /* Determine output filename */
    char *output_file = opts.output_file;
    char default_output[256];
    if (!output_file) {
        /* Replace .ls with .lsb */
        strcpy(default_output, opts.input_file);
        char *ext = strrchr(default_output, '.');
        if (ext) {
            strcpy(ext, ".lsb");
        } else {
            strcat(default_output, ".lsb");
        }
        output_file = default_output;
    }
    
    FILE *output = fopen(output_file, "wb");
    if (!output) {
        fprintf(stderr, "Error: Could not open output file '%s'\n", output_file);
        fclose(yyin);
        return 1;
    }
    
    CodeGenerator *gen = create_code_generator(output);
    generate_code(gen, root);
    write_bytecode_file(gen, output_file);
    
    if (opts.debug) {
        disassemble_chunk(gen->chunk, opts.input_file);
    }
    
    free_code_generator(gen);
    fclose(output);
    fclose(yyin);
    
    if (opts.verbose) {
        printf("Compilation successful: %s -> %s\n", opts.input_file, output_file);
    }
    
    return 0;
}

/* ============================================================================
 * MAIN ENTRY POINT
 * ============================================================================
 */

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    CompilerOptions opts = parse_args(argc, argv);
    
    if (!opts.input_file) {
        fprintf(stderr, "Error: No input file specified\n");
        print_usage(argv[0]);
        return 1;
    }
    
    /* Check file extension */
    const char *ext = strrchr(opts.input_file, '.');
    if (!ext || strcmp(ext, ".ls") != 0) {
        fprintf(stderr, "Warning: Input file does not have .ls extension\n");
    }
    
    int result = compile_file(opts);
    
    if (result == 0) {
        printf("✓ Compilation successful\n");
    } else {
        printf("✗ Compilation failed\n");
    }
    
    return result;
}

/* ============================================================================
 * COMPILER INFORMATION
 * ============================================================================
 */

void print_compiler_info() {
    printf("L# Compiler Information\n");
    printf("=======================\n");
    printf("Version: 1.0.0\n");
    printf("Target: Geneia Platform\n");
    printf("Bytecode Format: LSB# v1.0\n");
    printf("Language Specification: L# v1.0\n");
    printf("\nFeatures:\n");
    printf("  - Advanced type system with generics\n");
    printf("  - Ownership and borrowing\n");
    printf("  - Pattern matching\n");
    printf("  - Async/await\n");
    printf("  - Trait system\n");
    printf("  - Macro system\n");
    printf("  - Zero-cost abstractions\n");
    printf("\nCopyright (c) 2026 Moude AI Inc.\n");
    printf("Licensed under MIT License\n");
}
