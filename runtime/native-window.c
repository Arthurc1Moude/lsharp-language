/* L# Native Window System - Independent UI Framework
 * No HTML, No JavaScript, No Web Dependencies
 * Pure native rendering like Flutter
 * 
 * This is the COMPLETE native UI system for L# language
 * Renders directly to native windows using SDL2 + Cairo
 * No web technologies involved - pure native performance
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cairo/cairo.h>
#include <pango/pangocairo.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    cairo_surface_t *surface;
    cairo_t *cr;
    TTF_Font *font;
    int width;
    int height;
    int running;
} LSharpWindow;

typedef struct {
    float x, y, width, height;
    float r, g, b, a;
    float border_radius;
    char *text;
    int font_size;
    int is_button;
    void (*on_click)(void);
} LSharpWidget;

typedef struct {
    LSharpWidget *widgets;
    int widget_count;
    int capacity;
} LSharpUI;

/* Initialize L# Native Window */
LSharpWindow* lsharp_window_create(const char *title, int width, int height) {
    LSharpWindow *win = malloc(sizeof(LSharpWindow));
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        free(win);
        return NULL;
    }
    
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_Quit();
        free(win);
        return NULL;
    }
    
    win->window = SDL_CreateWindow(title,
                                   SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED,
                                   width, height,
                                   SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    
    if (win->window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        free(win);
        return NULL;
    }
    
    win->renderer = SDL_CreateRenderer(win->window, -1, SDL_RENDERER_ACCELERATED);
    if (win->renderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(win->window);
        TTF_Quit();
        SDL_Quit();
        free(win);
        return NULL;
    }
    
    win->width = width;
    win->height = height;
    win->running = 1;
    
    // Create Cairo surface for advanced rendering
    win->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    win->cr = cairo_create(win->surface);
    
    // Load font
    win->font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16);
    if (win->font == NULL) {
        // Try alternative font paths
        win->font = TTF_OpenFont("/System/Library/Fonts/Arial.ttf", 16);
        if (win->font == NULL) {
            printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        }
    }
    
    printf("✅ L# Native Window Created: %dx%d\n", width, height);
    return win;
}

/* Create L# UI System */
LSharpUI* lsharp_ui_create() {
    LSharpUI *ui = malloc(sizeof(LSharpUI));
    ui->capacity = 100;
    ui->widgets = malloc(sizeof(LSharpWidget) * ui->capacity);
    ui->widget_count = 0;
    return ui;
}

/* Add widget to UI */
void lsharp_ui_add_widget(LSharpUI *ui, float x, float y, float width, float height,
                          float r, float g, float b, float a, const char *text) {
    if (ui->widget_count >= ui->capacity) {
        ui->capacity *= 2;
        ui->widgets = realloc(ui->widgets, sizeof(LSharpWidget) * ui->capacity);
    }
    
    LSharpWidget *widget = &ui->widgets[ui->widget_count];
    widget->x = x;
    widget->y = y;
    widget->width = width;
    widget->height = height;
    widget->r = r;
    widget->g = g;
    widget->b = b;
    widget->a = a;
    widget->border_radius = 8.0f;
    widget->text = text ? strdup(text) : NULL;
    widget->font_size = 14;
    widget->is_button = 0;
    widget->on_click = NULL;
    
    ui->widget_count++;
}

/* Add button widget */
void lsharp_ui_add_button(LSharpUI *ui, float x, float y, float width, float height,
                          const char *text, void (*on_click)(void)) {
    lsharp_ui_add_widget(ui, x, y, width, height, 0.4f, 0.5f, 0.9f, 1.0f, text);
    LSharpWidget *widget = &ui->widgets[ui->widget_count - 1];
    widget->is_button = 1;
    widget->on_click = on_click;
    widget->border_radius = 12.0f;
}

/* Render rounded rectangle with Cairo */
void draw_rounded_rect(cairo_t *cr, float x, float y, float width, float height, float radius) {
    cairo_new_sub_path(cr);
    cairo_arc(cr, x + radius, y + radius, radius, M_PI, 3 * M_PI / 2);
    cairo_arc(cr, x + width - radius, y + radius, radius, 3 * M_PI / 2, 0);
    cairo_arc(cr, x + width - radius, y + height - radius, radius, 0, M_PI / 2);
    cairo_arc(cr, x + radius, y + height - radius, radius, M_PI / 2, M_PI);
    cairo_close_path(cr);
}

/* Render gradient */
void draw_gradient(cairo_t *cr, float x, float y, float width, float height) {
    cairo_pattern_t *gradient = cairo_pattern_create_linear(x, y, x + width, y);
    cairo_pattern_add_color_stop_rgb(gradient, 0, 0.4, 0.5, 0.9);  // #667eea
    cairo_pattern_add_color_stop_rgb(gradient, 1, 0.46, 0.29, 0.64); // #764ba2
    cairo_set_source(cr, gradient);
    cairo_pattern_destroy(gradient);
}

/* Render L# UI */
void lsharp_ui_render(LSharpWindow *win, LSharpUI *ui) {
    // Clear Cairo surface
    cairo_set_source_rgba(win->cr, 0.4, 0.5, 0.9, 1.0);
    cairo_paint(win->cr);
    
    // Draw background gradient
    cairo_pattern_t *bg_gradient = cairo_pattern_create_linear(0, 0, win->width, win->height);
    cairo_pattern_add_color_stop_rgb(bg_gradient, 0, 0.4, 0.5, 0.9);
    cairo_pattern_add_color_stop_rgb(bg_gradient, 1, 0.46, 0.29, 0.64);
    cairo_set_source(win->cr, bg_gradient);
    cairo_paint(win->cr);
    cairo_pattern_destroy(bg_gradient);
    
    // Render all widgets
    for (int i = 0; i < ui->widget_count; i++) {
        LSharpWidget *widget = &ui->widgets[i];
        
        // Draw widget background
        if (widget->is_button) {
            // Button gradient
            cairo_pattern_t *btn_gradient = cairo_pattern_create_linear(
                widget->x, widget->y, widget->x + widget->width, widget->y);
            cairo_pattern_add_color_stop_rgb(btn_gradient, 0, 0.4, 0.5, 0.9);
            cairo_pattern_add_color_stop_rgb(btn_gradient, 1, 0.46, 0.29, 0.64);
            cairo_set_source(win->cr, btn_gradient);
            cairo_pattern_destroy(btn_gradient);
        } else {
            cairo_set_source_rgba(win->cr, widget->r, widget->g, widget->b, widget->a);
        }
        
        draw_rounded_rect(win->cr, widget->x, widget->y, widget->width, widget->height, widget->border_radius);
        cairo_fill(win->cr);
        
        // Draw widget border/shadow
        cairo_set_source_rgba(win->cr, 0, 0, 0, 0.1);
        draw_rounded_rect(win->cr, widget->x + 2, widget->y + 2, widget->width, widget->height, widget->border_radius);
        cairo_fill(win->cr);
        
        // Draw text if present
        if (widget->text) {
            PangoLayout *layout;
            PangoFontDescription *desc;
            
            layout = pango_cairo_create_layout(win->cr);
            pango_layout_set_text(layout, widget->text, -1);
            
            desc = pango_font_description_from_string("Sans");
            pango_font_description_set_size(desc, widget->font_size * PANGO_SCALE);
            pango_layout_set_font_description(layout, desc);
            
            int text_width, text_height;
            pango_layout_get_size(layout, &text_width, &text_height);
            text_width /= PANGO_SCALE;
            text_height /= PANGO_SCALE;
            
            // Center text in widget
            float text_x = widget->x + (widget->width - text_width) / 2;
            float text_y = widget->y + (widget->height - text_height) / 2;
            
            cairo_move_to(win->cr, text_x, text_y);
            cairo_set_source_rgba(win->cr, widget->is_button ? 1.0 : 0.2, 
                                           widget->is_button ? 1.0 : 0.2, 
                                           widget->is_button ? 1.0 : 0.2, 1.0);
            pango_cairo_show_layout(win->cr, layout);
            
            g_object_unref(layout);
            pango_font_description_free(desc);
        }
    }
    
    // Copy Cairo surface to SDL
    unsigned char *data = cairo_image_surface_get_data(win->surface);
    int stride = cairo_image_surface_get_stride(win->surface);
    
    SDL_Surface *sdl_surface = SDL_CreateRGBSurfaceFrom(data, win->width, win->height, 32, stride,
                                                        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(win->renderer, sdl_surface);
    SDL_RenderClear(win->renderer);
    SDL_RenderCopy(win->renderer, texture, NULL, NULL);
    SDL_RenderPresent(win->renderer);
    
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(sdl_surface);
}

/* Handle events */
void lsharp_window_handle_events(LSharpWindow *win, LSharpUI *ui) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            win->running = 0;
        } else if (e.type == SDL_MOUSEBUTTONDOWN) {
            int mouse_x = e.button.x;
            int mouse_y = e.button.y;
            
            // Check widget clicks
            for (int i = 0; i < ui->widget_count; i++) {
                LSharpWidget *widget = &ui->widgets[i];
                if (widget->is_button && widget->on_click &&
                    mouse_x >= widget->x && mouse_x <= widget->x + widget->width &&
                    mouse_y >= widget->y && mouse_y <= widget->y + widget->height) {
                    widget->on_click();
                }
            }
        } else if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                win->width = e.window.data1;
                win->height = e.window.data2;
                
                // Recreate Cairo surface
                cairo_destroy(win->cr);
                cairo_surface_destroy(win->surface);
                win->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, win->width, win->height);
                win->cr = cairo_create(win->surface);
            }
        }
    }
}

/* Main L# Application Loop */
void lsharp_app_run(LSharpWindow *win, LSharpUI *ui) {
    printf("🚀 L# Native App Running - No HTML/JS Dependencies!\n");
    
    while (win->running) {
        lsharp_window_handle_events(win, ui);
        lsharp_ui_render(win, ui);
        SDL_Delay(16); // ~60 FPS
    }
}

/* Cleanup */
void lsharp_window_destroy(LSharpWindow *win) {
    if (win) {
        cairo_destroy(win->cr);
        cairo_surface_destroy(win->surface);
        
        if (win->font) TTF_CloseFont(win->font);
        if (win->renderer) SDL_DestroyRenderer(win->renderer);
        if (win->window) SDL_DestroyWindow(win->window);
        
        TTF_Quit();
        SDL_Quit();
        free(win);
    }
}

void lsharp_ui_destroy(LSharpUI *ui) {
    if (ui) {
        for (int i = 0; i < ui->widget_count; i++) {
            if (ui->widgets[i].text) {
                free(ui->widgets[i].text);
            }
        }
        free(ui->widgets);
        free(ui);
    }
}
/* MoudeStyle Chat Input Recreation - Pure L# Native */
void create_moude_style_chat_input(LSharpWindow *win, LSharpUI *ui) {
    const float window_width = win->width;
    const float window_height = win->height;
    
    // Main chat container
    const float chat_width = 800;
    const float chat_height = 600;
    const float chat_x = (window_width - chat_width) / 2;
    const float chat_y = (window_height - chat_height) / 2;
    
    // Chat window background
    lsharp_ui_add_widget(ui, chat_x, chat_y, chat_width, chat_height, 1.0f, 1.0f, 1.0f, 1.0f, NULL);
    
    // Chat header
    lsharp_ui_add_widget(ui, chat_x, chat_y, chat_width, 60, 0.4f, 0.5f, 0.9f, 1.0f, "L# MoudeStyle Chat");
    
    // Messages area background
    lsharp_ui_add_widget(ui, chat_x + 20, chat_y + 80, chat_width - 40, 400, 0.97f, 0.98f, 0.99f, 1.0f, NULL);
    
    // Input section background
    const float input_section_y = chat_y + chat_height - 120;
    lsharp_ui_add_widget(ui, chat_x + 20, input_section_y, chat_width - 40, 100, 1.0f, 1.0f, 1.0f, 1.0f, NULL);
    
    // Main input area
    const float input_x = chat_x + 40;
    const float input_y = input_section_y + 20;
    const float input_width = chat_width - 120;
    const float input_height = 40;
    
    lsharp_ui_add_widget(ui, input_x, input_y, input_width, input_height, 0.97f, 0.98f, 0.99f, 1.0f, "Ask L# AI to help edit files...");
    
    // Send button
    const float send_btn_x = input_x + input_width - 40;
    const float send_btn_y = input_y + 4;
    lsharp_ui_add_button(ui, send_btn_x, send_btn_y, 32, 32, "↑", NULL);
    
    // Bottom action buttons
    const float bottom_y = input_y + 50;
    
    // Add attachment button
    lsharp_ui_add_button(ui, input_x, bottom_y, 28, 28, "+", NULL);
    
    // Context button
    lsharp_ui_add_button(ui, input_x + 36, bottom_y, 28, 28, "<>", NULL);
    
    // Model selector
    const float model_btn_x = input_x + input_width - 80;
    lsharp_ui_add_button(ui, model_btn_x, bottom_y, 80, 28, "L# ▼", NULL);
}

/* L# Native Application Entry Point - Can be called from generated code */
int lsharp_native_main(int argc, char* argv[]) {
    (void)argc;  // Unused
    (void)argv;  // Unused
    
    printf("🚀 Starting L# Native Application - No Web Dependencies!\n");
    printf("📊 Pure native rendering with SDL2 + Cairo\n");
    
    // Create native window
    LSharpWindow *window = lsharp_window_create("L# MoudeStyle Chat - Native", 1000, 700);
    if (!window) {
        printf("❌ Failed to create L# native window\n");
        return -1;
    }
    
    // Create UI system
    LSharpUI *ui = lsharp_ui_create();
    
    // Build MoudeStyle chat interface
    create_moude_style_chat_input(window, ui);
    
    printf("✅ L# Native UI Created - MoudeStyle Design\n");
    printf("🎨 Rendering with native graphics - No HTML/CSS/JS\n");
    
    // Run the application
    lsharp_app_run(window, ui);
    
    // Cleanup
    lsharp_ui_destroy(ui);
    lsharp_window_destroy(window);
    
    printf("👋 L# Native App Closed\n");
    return 0;
}