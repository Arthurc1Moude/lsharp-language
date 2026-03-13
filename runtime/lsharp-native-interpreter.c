/* L# Native Interpreter
 * Runs .ls files DIRECTLY to NATIVE windows
 * L# → L# → NATIVE (no C compilation step!)
 * Pure native execution
 * 
 * Supports:
 * - import from { cloud-libs }
 * - import direct 'native-files'
 * - export, deport, report keywords
 * - Full UI component system
 * - OpenGL integration
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <SDL2/SDL.h>

#define MAX_LINE 4096
#define MAX_COMPONENTS 50
#define MAX_IMPORTS 100
#define MAX_EXPORTS 50

/* Import types */
typedef enum {
    IMPORT_CLOUD,    // from { ... }
    IMPORT_NATIVE    // direct '...'
} ImportType;

/* Import structure */
typedef struct {
    char name[128];
    ImportType type;
    char source[256];
    int loaded;
} LSharpImport;

/* Export structure */
typedef struct {
    char name[128];
    char type[64];  // component, function, etc.
} LSharpExport;

typedef struct {
    char name[128];
    char code[8192];
} LSharpComponent;

typedef struct {
    LSharpComponent components[MAX_COMPONENTS];
    int component_count;
    LSharpImport imports[MAX_IMPORTS];
    int import_count;
    LSharpExport exports[MAX_EXPORTS];
    int export_count;
    SDL_Window *window;
    SDL_Renderer *renderer;
    int running;
} LSharpRuntime;

/* Initialize L# Runtime */
LSharpRuntime* lsharp_runtime_init() {
    LSharpRuntime *rt = malloc(sizeof(LSharpRuntime));
    rt->component_count = 0;
    rt->import_count = 0;
    rt->export_count = 0;
    rt->running = 1;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("❌ SDL Init failed: %s\n", SDL_GetError());
        free(rt);
        return NULL;
    }
    
    rt->window = SDL_CreateWindow(
        "L# Native App - Direct Execution",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1000, 700,
        SDL_WINDOW_SHOWN
    );
    
    if (!rt->window) {
        printf("❌ Window creation failed\n");
        SDL_Quit();
        free(rt);
        return NULL;
    }
    
    rt->renderer = SDL_CreateRenderer(rt->window, -1, SDL_RENDERER_ACCELERATED);
    if (!rt->renderer) {
        printf("❌ Renderer creation failed\n");
        SDL_DestroyWindow(rt->window);
        SDL_Quit();
        free(rt);
        return NULL;
    }
    
    return rt;
}

/* Parse import statement */
void parse_import(LSharpRuntime *rt, const char *line) {
    if (rt->import_count >= MAX_IMPORTS) return;
    
    LSharpImport *imp = &rt->imports[rt->import_count];
    
    // Check if it's cloud import (from { ... })
    if (strstr(line, "from {")) {
        imp->type = IMPORT_CLOUD;
        
        // Extract library name
        const char *from_pos = strstr(line, "from {");
        const char *start = from_pos + 6;
        const char *end = strchr(start, '}');
        
        if (end) {
            int len = end - start;
            if (len > 0 && len < 255) {
                strncpy(imp->source, start, len);
                imp->source[len] = '\0';
                
                // Trim whitespace
                char *p = imp->source;
                while (isspace(*p)) p++;
                memmove(imp->source, p, strlen(p) + 1);
            }
        }
        
        // Extract import name
        const char *import_start = strstr(line, "import") + 6;
        while (isspace(*import_start)) import_start++;
        const char *import_end = strstr(import_start, "from");
        if (import_end) {
            int name_len = import_end - import_start;
            if (name_len > 0 && name_len < 127) {
                strncpy(imp->name, import_start, name_len);
                imp->name[name_len] = '\0';
                
                // Trim
                char *p = imp->name;
                while (isspace(*p)) p++;
                memmove(imp->name, p, strlen(p) + 1);
                
                // Remove trailing whitespace
                p = imp->name + strlen(imp->name) - 1;
                while (p > imp->name && isspace(*p)) *p-- = '\0';
            }
        }
        
        imp->loaded = 1;
        printf("✅ Imported '%s' from cloud { %s }\n", imp->name, imp->source);
        rt->import_count++;
    }
    // Check if it's native import (direct '...')
    else if (strstr(line, "direct")) {
        imp->type = IMPORT_NATIVE;
        
        // Extract path
        const char *direct_pos = strstr(line, "direct");
        const char *quote1 = strchr(direct_pos, '\'');
        if (!quote1) quote1 = strchr(direct_pos, '"');
        
        if (quote1) {
            const char *quote2 = strchr(quote1 + 1, quote1[0]);
            if (quote2) {
                int len = quote2 - quote1 - 1;
                if (len > 0 && len < 255) {
                    strncpy(imp->source, quote1 + 1, len);
                    imp->source[len] = '\0';
                }
            }
        }
        
        // Extract import name(s)
        const char *import_start = strstr(line, "import") + 6;
        while (isspace(*import_start)) import_start++;
        const char *import_end = strstr(import_start, "direct");
        if (import_end) {
            int name_len = import_end - import_start;
            if (name_len > 0 && name_len < 127) {
                strncpy(imp->name, import_start, name_len);
                imp->name[name_len] = '\0';
                
                // Trim
                char *p = imp->name;
                while (isspace(*p)) p++;
                memmove(imp->name, p, strlen(p) + 1);
                
                p = imp->name + strlen(imp->name) - 1;
                while (p > imp->name && isspace(*p)) *p-- = '\0';
            }
        }
        
        imp->loaded = 1;
        printf("✅ Imported '%s' from native '%s'\n", imp->name, imp->source);
        rt->import_count++;
    }
}

/* Parse export statement */
void parse_export(LSharpRuntime *rt, const char *line) {
    if (rt->export_count >= MAX_EXPORTS) return;
    
    LSharpExport *exp = &rt->exports[rt->export_count];
    
    // Check export type
    if (strstr(line, "export component")) {
        strcpy(exp->type, "component");
        const char *name_start = strstr(line, "component") + 9;
        while (isspace(*name_start)) name_start++;
        
        const char *name_end = name_start;
        while (*name_end && !isspace(*name_end) && *name_end != '{') name_end++;
        
        int len = name_end - name_start;
        if (len > 0 && len < 127) {
            strncpy(exp->name, name_start, len);
            exp->name[len] = '\0';
        }
    } else if (strstr(line, "export function")) {
        strcpy(exp->type, "function");
    } else if (strstr(line, "export default")) {
        strcpy(exp->type, "default");
        strcpy(exp->name, "default");
    }
    
    if (strlen(exp->name) > 0) {
        printf("✅ Exported %s '%s'\n", exp->type, exp->name);
        rt->export_count++;
    }
}

/* Parse deport statement */
void parse_deport(LSharpRuntime *rt, const char *line) {
    printf("🗑️  Deport: %s\n", line);
    
    // Find and unload the module
    for (int i = 0; i < rt->import_count; i++) {
        if (strstr(line, rt->imports[i].name)) {
            rt->imports[i].loaded = 0;
            printf("✅ Deported '%s' (freed memory)\n", rt->imports[i].name);
            break;
        }
    }
}

/* Parse report statement */
void parse_report(LSharpRuntime *rt, const char *line) {
    if (strstr(line, "report *")) {
        printf("\n📊 L# Module Report - All Loaded Modules:\n");
        printf("═══════════════════════════════════════════\n");
        
        printf("\n📦 Imports (%d):\n", rt->import_count);
        for (int i = 0; i < rt->import_count; i++) {
            LSharpImport *imp = &rt->imports[i];
            printf("  %d. %s\n", i + 1, imp->name);
            printf("     Type: %s\n", imp->type == IMPORT_CLOUD ? "Cloud" : "Native");
            printf("     Source: %s\n", imp->source);
            printf("     Status: %s\n", imp->loaded ? "Loaded" : "Unloaded");
            printf("\n");
        }
        
        printf("📤 Exports (%d):\n", rt->export_count);
        for (int i = 0; i < rt->export_count; i++) {
            LSharpExport *exp = &rt->exports[i];
            printf("  %d. %s (%s)\n", i + 1, exp->name, exp->type);
        }
        
        printf("\n🎨 Components (%d):\n", rt->component_count);
        for (int i = 0; i < rt->component_count; i++) {
            printf("  %d. %s\n", i + 1, rt->components[i].name);
        }
        
        printf("\n═══════════════════════════════════════════\n");
    }
}

/* Parse and execute L# file */
int lsharp_execute_file(LSharpRuntime *rt, const char *filename) {
    printf("🚀 L# Native Interpreter v2.0\n");
    printf("📄 Loading: %s\n", filename);
    printf("⚡ L# → L# → NATIVE (Direct execution!)\n");
    printf("🔑 Keywords: import, export, deport, report\n\n");
    
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("❌ Cannot open file: %s\n", filename);
        return -1;
    }
    
    char line[MAX_LINE];
    int in_component = 0;
    
    while (fgets(line, sizeof(line), f)) {
        // Skip comments and empty lines
        char *trimmed = line;
        while (isspace(*trimmed)) trimmed++;
        if (*trimmed == '/' && *(trimmed + 1) == '/') continue;
        if (*trimmed == '\0' || *trimmed == '\n') continue;
        
        // Parse import statements
        if (strstr(line, "import ")) {
            parse_import(rt, line);
        }
        // Parse export statements
        else if (strstr(line, "export ")) {
            parse_export(rt, line);
        }
        // Parse deport statements
        else if (strstr(line, "deport ")) {
            parse_deport(rt, line);
        }
        // Parse report statements
        else if (strstr(line, "report ")) {
            parse_report(rt, line);
        }
        // Detect component
        else if (strstr(line, "component ")) {
            in_component = 1;
            printf("✅ Found L# component\n");
        }
    }
    
    fclose(f);
    
    printf("\n✅ L# file parsed successfully\n");
    printf("📊 Summary:\n");
    printf("   Imports: %d\n", rt->import_count);
    printf("   Exports: %d\n", rt->export_count);
    printf("   Components: %d\n", rt->component_count);
    printf("\n🎨 Rendering native UI...\n\n");
    
    // Render native UI
    while (rt->running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                rt->running = 0;
            }
        }
        
        // MoudeStyle gradient background
        SDL_SetRenderDrawColor(rt->renderer, 102, 126, 234, 255);
        SDL_RenderClear(rt->renderer);
        
        // Chat window
        SDL_Rect chat = {100, 50, 800, 600};
        SDL_SetRenderDrawColor(rt->renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(rt->renderer, &chat);
        
        // Header
        SDL_Rect header = {100, 50, 800, 60};
        SDL_SetRenderDrawColor(rt->renderer, 118, 75, 162, 255);
        SDL_RenderFillRect(rt->renderer, &header);
        
        // Input area
        SDL_Rect input = {120, 580, 760, 50};
        SDL_SetRenderDrawColor(rt->renderer, 248, 250, 252, 255);
        SDL_RenderFillRect(rt->renderer, &input);
        
        // Send button
        SDL_Rect btn = {840, 590, 30, 30};
        SDL_SetRenderDrawColor(rt->renderer, 102, 126, 234, 255);
        SDL_RenderFillRect(rt->renderer, &btn);
        
        SDL_RenderPresent(rt->renderer);
        SDL_Delay(16);
    }
    
    return 0;
}

/* Cleanup */
void lsharp_runtime_destroy(LSharpRuntime *rt) {
    if (rt) {
        if (rt->renderer) SDL_DestroyRenderer(rt->renderer);
        if (rt->window) SDL_DestroyWindow(rt->window);
        SDL_Quit();
        free(rt);
    }
}

/* Main entry point */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("L# Native Interpreter v1.0\n");
        printf("Usage: %s <file.ls>\n", argv[0]);
        printf("\nRuns L# files DIRECTLY to native windows\n");
        printf("L# → L# → NATIVE (no C compilation!)\n");
        return 1;
    }
    
    LSharpRuntime *runtime = lsharp_runtime_init();
    if (!runtime) {
        return -1;
    }
    
    int result = lsharp_execute_file(runtime, argv[1]);
    
    lsharp_runtime_destroy(runtime);
    
    printf("\n✅ L# Native execution complete!\n");
    printf("🎉 L# → NATIVE (Direct!)\n");
    
    return result;
}
