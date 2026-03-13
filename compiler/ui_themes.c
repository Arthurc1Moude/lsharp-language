/* L# UI Theme System - Complete Design System */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_renderer.h"

/* Color Palette */
typedef struct {
    char *primary;
    char *secondary;
    char *success;
    char *danger;
    char *warning;
    char *info;
    char *light;
    char *dark;
    char *background;
    char *surface;
    char *text_primary;
    char *text_secondary;
    char *border;
} ColorPalette;

/* Typography */
typedef struct {
    char *font_family_primary;
    char *font_family_secondary;
    char *font_family_mono;
    int font_size_xs;
    int font_size_sm;
    int font_size_md;
    int font_size_lg;
    int font_size_xl;
    int font_size_xxl;
    int line_height;
    int letter_spacing;
} Typography;

/* Spacing */
typedef struct {
    int xs;
    int sm;
    int md;
    int lg;
    int xl;
    int xxl;
} Spacing;

/* Border Radius */
typedef struct {
    int none;
    int sm;
    int md;
    int lg;
    int xl;
    int full;
} BorderRadius;

/* Theme */
typedef struct {
    char *name;
    ColorPalette *colors;
    Typography *typography;
    Spacing *spacing;
    BorderRadius *radius;
    int shadow_enabled;
    int animation_enabled;
} Theme;

/* Create Default Light Theme */
Theme* create_light_theme() {
    Theme *theme = malloc(sizeof(Theme));
    theme->name = strdup("light");
    
    theme->colors = malloc(sizeof(ColorPalette));
    theme->colors->primary = strdup("#007bff");
    theme->colors->secondary = strdup("#6c757d");
    theme->colors->success = strdup("#28a745");
    theme->colors->danger = strdup("#dc3545");
    theme->colors->warning = strdup("#ffc107");
    theme->colors->info = strdup("#17a2b8");
    theme->colors->light = strdup("#f8f9fa");
    theme->colors->dark = strdup("#343a40");
    theme->colors->background = strdup("#ffffff");
    theme->colors->surface = strdup("#f5f5f5");
    theme->colors->text_primary = strdup("#212529");
    theme->colors->text_secondary = strdup("#6c757d");
    theme->colors->border = strdup("#dee2e6");
    
    theme->typography = malloc(sizeof(Typography));
    theme->typography->font_family_primary = strdup("-apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, sans-serif");
    theme->typography->font_family_secondary = strdup("Georgia, 'Times New Roman', serif");
    theme->typography->font_family_mono = strdup("'Courier New', Courier, monospace");
    theme->typography->font_size_xs = 12;
    theme->typography->font_size_sm = 14;
    theme->typography->font_size_md = 16;
    theme->typography->font_size_lg = 18;
    theme->typography->font_size_xl = 24;
    theme->typography->font_size_xxl = 32;
    theme->typography->line_height = 150;
    theme->typography->letter_spacing = 0;
    
    theme->spacing = malloc(sizeof(Spacing));
    theme->spacing->xs = 4;
    theme->spacing->sm = 8;
    theme->spacing->md = 16;
    theme->spacing->lg = 24;
    theme->spacing->xl = 32;
    theme->spacing->xxl = 48;
    
    theme->radius = malloc(sizeof(BorderRadius));
    theme->radius->none = 0;
    theme->radius->sm = 4;
    theme->radius->md = 8;
    theme->radius->lg = 12;
    theme->radius->xl = 16;
    theme->radius->full = 9999;
    
    theme->shadow_enabled = 1;
    theme->animation_enabled = 1;
    
    return theme;
}

/* Create Dark Theme */
Theme* create_dark_theme() {
    Theme *theme = malloc(sizeof(Theme));
    theme->name = strdup("dark");
    
    theme->colors = malloc(sizeof(ColorPalette));
    theme->colors->primary = strdup("#0d6efd");
    theme->colors->secondary = strdup("#6c757d");
    theme->colors->success = strdup("#198754");
    theme->colors->danger = strdup("#dc3545");
    theme->colors->warning = strdup("#ffc107");
    theme->colors->info = strdup("#0dcaf0");
    theme->colors->light = strdup("#f8f9fa");
    theme->colors->dark = strdup("#212529");
    theme->colors->background = strdup("#1a1a1a");
    theme->colors->surface = strdup("#2d2d2d");
    theme->colors->text_primary = strdup("#ffffff");
    theme->colors->text_secondary = strdup("#adb5bd");
    theme->colors->border = strdup("#495057");
    
    theme->typography = malloc(sizeof(Typography));
    theme->typography->font_family_primary = strdup("-apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, sans-serif");
    theme->typography->font_family_secondary = strdup("Georgia, 'Times New Roman', serif");
    theme->typography->font_family_mono = strdup("'Courier New', Courier, monospace");
    theme->typography->font_size_xs = 12;
    theme->typography->font_size_sm = 14;
    theme->typography->font_size_md = 16;
    theme->typography->font_size_lg = 18;
    theme->typography->font_size_xl = 24;
    theme->typography->font_size_xxl = 32;
    theme->typography->line_height = 150;
    theme->typography->letter_spacing = 0;
    
    theme->spacing = malloc(sizeof(Spacing));
    theme->spacing->xs = 4;
    theme->spacing->sm = 8;
    theme->spacing->md = 16;
    theme->spacing->lg = 24;
    theme->spacing->xl = 32;
    theme->spacing->xxl = 48;
    
    theme->radius = malloc(sizeof(BorderRadius));
    theme->radius->none = 0;
    theme->radius->sm = 4;
    theme->radius->md = 8;
    theme->radius->lg = 12;
    theme->radius->xl = 16;
    theme->radius->full = 9999;
    
    theme->shadow_enabled = 1;
    theme->animation_enabled = 1;
    
    return theme;
}

/* Create Modern Theme */
Theme* create_modern_theme() {
    Theme *theme = malloc(sizeof(Theme));
    theme->name = strdup("modern");
    
    theme->colors = malloc(sizeof(ColorPalette));
    theme->colors->primary = strdup("#6366f1");
    theme->colors->secondary = strdup("#8b5cf6");
    theme->colors->success = strdup("#10b981");
    theme->colors->danger = strdup("#ef4444");
    theme->colors->warning = strdup("#f59e0b");
    theme->colors->info = strdup("#3b82f6");
    theme->colors->light = strdup("#f9fafb");
    theme->colors->dark = strdup("#111827");
    theme->colors->background = strdup("#ffffff");
    theme->colors->surface = strdup("#f3f4f6");
    theme->colors->text_primary = strdup("#1f2937");
    theme->colors->text_secondary = strdup("#6b7280");
    theme->colors->border = strdup("#e5e7eb");
    
    theme->typography = malloc(sizeof(Typography));
    theme->typography->font_family_primary = strdup("'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif");
    theme->typography->font_family_secondary = strdup("'Playfair Display', Georgia, serif");
    theme->typography->font_family_mono = strdup("'Fira Code', 'Courier New', monospace");
    theme->typography->font_size_xs = 12;
    theme->typography->font_size_sm = 14;
    theme->typography->font_size_md = 16;
    theme->typography->font_size_lg = 20;
    theme->typography->font_size_xl = 28;
    theme->typography->font_size_xxl = 36;
    theme->typography->line_height = 160;
    theme->typography->letter_spacing = -1;
    
    theme->spacing = malloc(sizeof(Spacing));
    theme->spacing->xs = 4;
    theme->spacing->sm = 8;
    theme->spacing->md = 16;
    theme->spacing->lg = 24;
    theme->spacing->xl = 32;
    theme->spacing->xxl = 48;
    
    theme->radius = malloc(sizeof(BorderRadius));
    theme->radius->none = 0;
    theme->radius->sm = 6;
    theme->radius->md = 12;
    theme->radius->lg = 16;
    theme->radius->xl = 24;
    theme->radius->full = 9999;
    
    theme->shadow_enabled = 1;
    theme->animation_enabled = 1;
    
    return theme;
}

/* Apply Theme to Element */
void ui_apply_theme(UIElement *elem, Theme *theme, const char *variant) {
    if (strcmp(variant, "primary") == 0) {
        ui_set_style(elem, "background-color", theme->colors->primary);
        ui_set_style(elem, "color", "#ffffff");
    } else if (strcmp(variant, "secondary") == 0) {
        ui_set_style(elem, "background-color", theme->colors->secondary);
        ui_set_style(elem, "color", "#ffffff");
    } else if (strcmp(variant, "success") == 0) {
        ui_set_style(elem, "background-color", theme->colors->success);
        ui_set_style(elem, "color", "#ffffff");
    } else if (strcmp(variant, "danger") == 0) {
        ui_set_style(elem, "background-color", theme->colors->danger);
        ui_set_style(elem, "color", "#ffffff");
    } else if (strcmp(variant, "warning") == 0) {
        ui_set_style(elem, "background-color", theme->colors->warning);
        ui_set_style(elem, "color", theme->colors->dark);
    } else if (strcmp(variant, "info") == 0) {
        ui_set_style(elem, "background-color", theme->colors->info);
        ui_set_style(elem, "color", "#ffffff");
    }
    
    ui_set_style(elem, "font-family", theme->typography->font_family_primary);
    ui_set_style_int(elem, "border-radius", theme->radius->md);
    
    if (theme->shadow_enabled) {
        ui_set_style(elem, "box-shadow", "0 2px 8px rgba(0, 0, 0, 0.1)");
    }
    
    if (theme->animation_enabled) {
        ui_set_style(elem, "transition", "all 0.3s ease");
    }
}

/* Generate Theme CSS */
void generate_theme_css(FILE *output, Theme *theme) {
    fprintf(output, "/* %s Theme */\n", theme->name);
    fprintf(output, ":root {\n");
    fprintf(output, "  --color-primary: %s;\n", theme->colors->primary);
    fprintf(output, "  --color-secondary: %s;\n", theme->colors->secondary);
    fprintf(output, "  --color-success: %s;\n", theme->colors->success);
    fprintf(output, "  --color-danger: %s;\n", theme->colors->danger);
    fprintf(output, "  --color-warning: %s;\n", theme->colors->warning);
    fprintf(output, "  --color-info: %s;\n", theme->colors->info);
    fprintf(output, "  --color-light: %s;\n", theme->colors->light);
    fprintf(output, "  --color-dark: %s;\n", theme->colors->dark);
    fprintf(output, "  --color-background: %s;\n", theme->colors->background);
    fprintf(output, "  --color-surface: %s;\n", theme->colors->surface);
    fprintf(output, "  --color-text-primary: %s;\n", theme->colors->text_primary);
    fprintf(output, "  --color-text-secondary: %s;\n", theme->colors->text_secondary);
    fprintf(output, "  --color-border: %s;\n", theme->colors->border);
    fprintf(output, "\n");
    fprintf(output, "  --font-family-primary: %s;\n", theme->typography->font_family_primary);
    fprintf(output, "  --font-family-secondary: %s;\n", theme->typography->font_family_secondary);
    fprintf(output, "  --font-family-mono: %s;\n", theme->typography->font_family_mono);
    fprintf(output, "  --font-size-xs: %dpx;\n", theme->typography->font_size_xs);
    fprintf(output, "  --font-size-sm: %dpx;\n", theme->typography->font_size_sm);
    fprintf(output, "  --font-size-md: %dpx;\n", theme->typography->font_size_md);
    fprintf(output, "  --font-size-lg: %dpx;\n", theme->typography->font_size_lg);
    fprintf(output, "  --font-size-xl: %dpx;\n", theme->typography->font_size_xl);
    fprintf(output, "  --font-size-xxl: %dpx;\n", theme->typography->font_size_xxl);
    fprintf(output, "\n");
    fprintf(output, "  --spacing-xs: %dpx;\n", theme->spacing->xs);
    fprintf(output, "  --spacing-sm: %dpx;\n", theme->spacing->sm);
    fprintf(output, "  --spacing-md: %dpx;\n", theme->spacing->md);
    fprintf(output, "  --spacing-lg: %dpx;\n", theme->spacing->lg);
    fprintf(output, "  --spacing-xl: %dpx;\n", theme->spacing->xl);
    fprintf(output, "  --spacing-xxl: %dpx;\n", theme->spacing->xxl);
    fprintf(output, "\n");
    fprintf(output, "  --radius-none: %dpx;\n", theme->radius->none);
    fprintf(output, "  --radius-sm: %dpx;\n", theme->radius->sm);
    fprintf(output, "  --radius-md: %dpx;\n", theme->radius->md);
    fprintf(output, "  --radius-lg: %dpx;\n", theme->radius->lg);
    fprintf(output, "  --radius-xl: %dpx;\n", theme->radius->xl);
    fprintf(output, "  --radius-full: %dpx;\n", theme->radius->full);
    fprintf(output, "}\n\n");
    
    fprintf(output, "body {\n");
    fprintf(output, "  font-family: var(--font-family-primary);\n");
    fprintf(output, "  font-size: var(--font-size-md);\n");
    fprintf(output, "  color: var(--color-text-primary);\n");
    fprintf(output, "  background-color: var(--color-background);\n");
    fprintf(output, "  line-height: %.2f;\n", theme->typography->line_height / 100.0);
    fprintf(output, "}\n\n");
}

/* Free Theme */
void free_theme(Theme *theme) {
    if (!theme) return;
    
    free(theme->name);
    
    if (theme->colors) {
        free(theme->colors->primary);
        free(theme->colors->secondary);
        free(theme->colors->success);
        free(theme->colors->danger);
        free(theme->colors->warning);
        free(theme->colors->info);
        free(theme->colors->light);
        free(theme->colors->dark);
        free(theme->colors->background);
        free(theme->colors->surface);
        free(theme->colors->text_primary);
        free(theme->colors->text_secondary);
        free(theme->colors->border);
        free(theme->colors);
    }
    
    if (theme->typography) {
        free(theme->typography->font_family_primary);
        free(theme->typography->font_family_secondary);
        free(theme->typography->font_family_mono);
        free(theme->typography);
    }
    
    free(theme->spacing);
    free(theme->radius);
    free(theme);
}
