/* L# Complete UI/Design System
 * OpenGL-like rendering with beautiful design
 * Full native UI framework
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* L# UI System Structure */
typedef struct {
    SDL_Window *window;
    SDL_GLContext gl_context;
    int width;
    int height;
    int running;
    float time;
} LSharpUI;

/* Color structure */
typedef struct {
    float r, g, b, a;
} LSColor;

/* Vector2 */
typedef struct {
    float x, y;
} LSVec2;

/* Vector3 */
typedef struct {
    float x, y, z;
} LSVec3;

/* Rectangle */
typedef struct {
    float x, y, width, height;
} LSRect;

/* Initialize L# UI System with OpenGL */
LSharpUI* ls_ui_init(const char *title, int width, int height) {
    LSharpUI *ui = malloc(sizeof(LSharpUI));
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("❌ SDL Init failed: %s\n", SDL_GetError());
        free(ui);
        return NULL;
    }
    
    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    ui->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
    
    if (!ui->window) {
        printf("❌ Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        free(ui);
        return NULL;
    }
    
    ui->gl_context = SDL_GL_CreateContext(ui->window);
    if (!ui->gl_context) {
        printf("❌ OpenGL context creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(ui->window);
        SDL_Quit();
        free(ui);
        return NULL;
    }
    
    ui->width = width;
    ui->height = height;
    ui->running = 1;
    ui->time = 0.0f;
    
    // Initialize OpenGL
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Enable features
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    printf("✅ L# UI System initialized with OpenGL\n");
    printf("📊 Resolution: %dx%d\n", width, height);
    printf("🎨 OpenGL Version: %s\n", glGetString(GL_VERSION));
    
    return ui;
}

/* Set color */
void ls_color(LSColor color) {
    glColor4f(color.r, color.g, color.b, color.a);
}

/* Draw rectangle */
void ls_rect(LSRect rect, LSColor color) {
    ls_color(color);
    glBegin(GL_QUADS);
    glVertex2f(rect.x, rect.y);
    glVertex2f(rect.x + rect.width, rect.y);
    glVertex2f(rect.x + rect.width, rect.y + rect.height);
    glVertex2f(rect.x, rect.y + rect.height);
    glEnd();
}

/* Draw rounded rectangle */
void ls_rounded_rect(LSRect rect, float radius, LSColor color) {
    ls_color(color);
    
    int segments = 16;
    float angle_step = (M_PI / 2.0f) / segments;
    
    glBegin(GL_TRIANGLE_FAN);
    
    // Center
    glVertex2f(rect.x + rect.width / 2, rect.y + rect.height / 2);
    
    // Top-left corner
    for (int i = 0; i <= segments; i++) {
        float angle = M_PI + i * angle_step;
        glVertex2f(
            rect.x + radius + cos(angle) * radius,
            rect.y + radius + sin(angle) * radius
        );
    }
    
    // Top-right corner
    for (int i = 0; i <= segments; i++) {
        float angle = 3 * M_PI / 2 + i * angle_step;
        glVertex2f(
            rect.x + rect.width - radius + cos(angle) * radius,
            rect.y + radius + sin(angle) * radius
        );
    }
    
    // Bottom-right corner
    for (int i = 0; i <= segments; i++) {
        float angle = i * angle_step;
        glVertex2f(
            rect.x + rect.width - radius + cos(angle) * radius,
            rect.y + rect.height - radius + sin(angle) * radius
        );
    }
    
    // Bottom-left corner
    for (int i = 0; i <= segments; i++) {
        float angle = M_PI / 2 + i * angle_step;
        glVertex2f(
            rect.x + radius + cos(angle) * radius,
            rect.y + rect.height - radius + sin(angle) * radius
        );
    }
    
    glEnd();
}

/* Draw gradient rectangle */
void ls_gradient_rect(LSRect rect, LSColor color1, LSColor color2, int vertical) {
    glBegin(GL_QUADS);
    
    if (vertical) {
        glColor4f(color1.r, color1.g, color1.b, color1.a);
        glVertex2f(rect.x, rect.y);
        glVertex2f(rect.x + rect.width, rect.y);
        
        glColor4f(color2.r, color2.g, color2.b, color2.a);
        glVertex2f(rect.x + rect.width, rect.y + rect.height);
        glVertex2f(rect.x, rect.y + rect.height);
    } else {
        glColor4f(color1.r, color1.g, color1.b, color1.a);
        glVertex2f(rect.x, rect.y);
        glVertex2f(rect.x, rect.y + rect.height);
        
        glColor4f(color2.r, color2.g, color2.b, color2.a);
        glVertex2f(rect.x + rect.width, rect.y + rect.height);
        glVertex2f(rect.x + rect.width, rect.y);
    }
    
    glEnd();
}

/* Draw circle */
void ls_circle(LSVec2 center, float radius, LSColor color) {
    ls_color(color);
    
    int segments = 32;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(center.x, center.y);
    
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        glVertex2f(
            center.x + cos(angle) * radius,
            center.y + sin(angle) * radius
        );
    }
    
    glEnd();
}

/* Draw line */
void ls_line(LSVec2 start, LSVec2 end, float width, LSColor color) {
    ls_color(color);
    glLineWidth(width);
    glBegin(GL_LINES);
    glVertex2f(start.x, start.y);
    glVertex2f(end.x, end.y);
    glEnd();
}

/* Draw shadow */
void ls_shadow(LSRect rect, float blur, LSColor color) {
    for (int i = 0; i < blur; i++) {
        LSColor shadow_color = {
            color.r, color.g, color.b,
            color.a * (1.0f - (float)i / blur)
        };
        
        LSRect shadow_rect = {
            rect.x - i, rect.y - i,
            rect.width + i * 2, rect.height + i * 2
        };
        
        ls_rect(shadow_rect, shadow_color);
    }
}

/* MoudeStyle gradient colors */
LSColor ls_moude_color1() {
    return (LSColor){0.4f, 0.5f, 0.9f, 1.0f}; // #667eea
}

LSColor ls_moude_color2() {
    return (LSColor){0.46f, 0.29f, 0.64f, 1.0f}; // #764ba2
}

/* Draw MoudeStyle button */
void ls_moude_button(LSRect rect, const char *text, int hovered) {
    // Shadow
    LSColor shadow = {0.0f, 0.0f, 0.0f, 0.15f};
    LSRect shadow_rect = {rect.x + 2, rect.y + 2, rect.width, rect.height};
    ls_rounded_rect(shadow_rect, 8, shadow);
    
    // Button gradient
    LSColor color1 = ls_moude_color1();
    LSColor color2 = ls_moude_color2();
    
    if (hovered) {
        color1.r *= 1.1f; color1.g *= 1.1f; color1.b *= 1.1f;
        color2.r *= 1.1f; color2.g *= 1.1f; color2.b *= 1.1f;
    }
    
    ls_gradient_rect(rect, color1, color2, 0);
    
    // Border
    LSColor border = {1.0f, 1.0f, 1.0f, 0.3f};
    glLineWidth(1.0f);
    ls_color(border);
    glBegin(GL_LINE_LOOP);
    glVertex2f(rect.x, rect.y);
    glVertex2f(rect.x + rect.width, rect.y);
    glVertex2f(rect.x + rect.width, rect.y + rect.height);
    glVertex2f(rect.x, rect.y + rect.height);
    glEnd();
}

/* Draw MoudeStyle card */
void ls_moude_card(LSRect rect) {
    // Shadow
    LSColor shadow = {0.0f, 0.0f, 0.0f, 0.1f};
    for (int i = 0; i < 20; i++) {
        LSRect shadow_rect = {
            rect.x + i, rect.y + i,
            rect.width, rect.height
        };
        shadow.a = 0.1f * (1.0f - (float)i / 20.0f);
        ls_rounded_rect(shadow_rect, 16, shadow);
    }
    
    // Card background
    LSColor bg = {1.0f, 1.0f, 1.0f, 1.0f};
    ls_rounded_rect(rect, 16, bg);
}

/* Clear screen */
void ls_clear(LSColor color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/* Present frame */
void ls_present(LSharpUI *ui) {
    SDL_GL_SwapWindow(ui->window);
}

/* Handle events */
int ls_handle_events(LSharpUI *ui) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            ui->running = 0;
            return 0;
        }
    }
    return 1;
}

/* Cleanup */
void ls_ui_destroy(LSharpUI *ui) {
    if (ui) {
        if (ui->gl_context) SDL_GL_DeleteContext(ui->gl_context);
        if (ui->window) SDL_DestroyWindow(ui->window);
        SDL_Quit();
        free(ui);
    }
}
