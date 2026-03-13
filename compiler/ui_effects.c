/* L# UI Effects System - Advanced Visual Effects */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_renderer.h"

/* Shadow Types */
typedef struct {
    int offset_x;
    int offset_y;
    int blur_radius;
    int spread_radius;
    char *color;
    int inset;
} BoxShadow;

/* Gradient Types */
typedef enum {
    GRADIENT_LINEAR,
    GRADIENT_RADIAL,
    GRADIENT_CONIC
} GradientType;

typedef struct {
    char *color;
    int position;
} ColorStop;

typedef struct {
    GradientType type;
    int angle;
    ColorStop *stops;
    int stop_count;
    char *shape;
    char *size;
} Gradient;

/* Filter Effects */
typedef struct {
    int blur;
    int brightness;
    int contrast;
    int grayscale;
    int hue_rotate;
    int invert;
    int opacity;
    int saturate;
    int sepia;
} FilterEffects;

/* Transform Properties */
typedef struct {
    int translate_x;
    int translate_y;
    int translate_z;
    float scale_x;
    float scale_y;
    float scale_z;
    int rotate_x;
    int rotate_y;
    int rotate_z;
    int skew_x;
    int skew_y;
    int perspective;
} Transform;

/* Apply Box Shadow */
void ui_apply_box_shadow(UIElement *elem, BoxShadow *shadow) {
    char shadow_str[256];
    snprintf(shadow_str, 256, "%s%dpx %dpx %dpx %dpx %s",
             shadow->inset ? "inset " : "",
             shadow->offset_x,
             shadow->offset_y,
             shadow->blur_radius,
             shadow->spread_radius,
             shadow->color);
    
    ui_set_style(elem, "box-shadow", shadow_str);
}

/* Apply Multiple Shadows */
void ui_apply_multiple_shadows(UIElement *elem, BoxShadow **shadows, int count) {
    char shadow_str[1024] = "";
    char temp[256];
    
    for (int i = 0; i < count; i++) {
        snprintf(temp, 256, "%s%dpx %dpx %dpx %dpx %s",
                 shadows[i]->inset ? "inset " : "",
                 shadows[i]->offset_x,
                 shadows[i]->offset_y,
                 shadows[i]->blur_radius,
                 shadows[i]->spread_radius,
                 shadows[i]->color);
        
        strcat(shadow_str, temp);
        if (i < count - 1) strcat(shadow_str, ", ");
    }
    
    ui_set_style(elem, "box-shadow", shadow_str);
}

/* Apply Gradient Background */
void ui_apply_gradient(UIElement *elem, Gradient *gradient) {
    char gradient_str[512];
    
    if (gradient->type == GRADIENT_LINEAR) {
        snprintf(gradient_str, 512, "linear-gradient(%ddeg", gradient->angle);
    } else if (gradient->type == GRADIENT_RADIAL) {
        snprintf(gradient_str, 512, "radial-gradient(%s %s",
                 gradient->shape ? gradient->shape : "circle",
                 gradient->size ? gradient->size : "farthest-corner");
    } else {
        snprintf(gradient_str, 512, "conic-gradient(from %ddeg", gradient->angle);
    }
    
    for (int i = 0; i < gradient->stop_count; i++) {
        char stop[64];
        snprintf(stop, 64, ", %s %d%%",
                 gradient->stops[i].color,
                 gradient->stops[i].position);
        strcat(gradient_str, stop);
    }
    
    strcat(gradient_str, ")");
    ui_set_style(elem, "background", gradient_str);
}

/* Apply Filter Effects */
void ui_apply_filters(UIElement *elem, FilterEffects *filters) {
    char filter_str[512] = "";
    char temp[64];
    
    if (filters->blur > 0) {
        snprintf(temp, 64, "blur(%dpx) ", filters->blur);
        strcat(filter_str, temp);
    }
    
    if (filters->brightness != 100) {
        snprintf(temp, 64, "brightness(%d%%) ", filters->brightness);
        strcat(filter_str, temp);
    }
    
    if (filters->contrast != 100) {
        snprintf(temp, 64, "contrast(%d%%) ", filters->contrast);
        strcat(filter_str, temp);
    }
    
    if (filters->grayscale > 0) {
        snprintf(temp, 64, "grayscale(%d%%) ", filters->grayscale);
        strcat(filter_str, temp);
    }
    
    if (filters->hue_rotate != 0) {
        snprintf(temp, 64, "hue-rotate(%ddeg) ", filters->hue_rotate);
        strcat(filter_str, temp);
    }
    
    if (filters->invert > 0) {
        snprintf(temp, 64, "invert(%d%%) ", filters->invert);
        strcat(filter_str, temp);
    }
    
    if (filters->opacity != 100) {
        snprintf(temp, 64, "opacity(%d%%) ", filters->opacity);
        strcat(filter_str, temp);
    }
    
    if (filters->saturate != 100) {
        snprintf(temp, 64, "saturate(%d%%) ", filters->saturate);
        strcat(filter_str, temp);
    }
    
    if (filters->sepia > 0) {
        snprintf(temp, 64, "sepia(%d%%) ", filters->sepia);
        strcat(filter_str, temp);
    }
    
    if (strlen(filter_str) > 0) {
        ui_set_style(elem, "filter", filter_str);
    }
}

/* Apply Transform */
void ui_apply_transform(UIElement *elem, Transform *transform) {
    char transform_str[512] = "";
    char temp[128];
    
    if (transform->translate_x != 0 || transform->translate_y != 0) {
        snprintf(temp, 128, "translate(%dpx, %dpx) ",
                 transform->translate_x, transform->translate_y);
        strcat(transform_str, temp);
    }
    
    if (transform->translate_z != 0) {
        snprintf(temp, 128, "translateZ(%dpx) ", transform->translate_z);
        strcat(transform_str, temp);
    }
    
    if (transform->scale_x != 1.0 || transform->scale_y != 1.0) {
        snprintf(temp, 128, "scale(%.2f, %.2f) ",
                 transform->scale_x, transform->scale_y);
        strcat(transform_str, temp);
    }
    
    if (transform->rotate_z != 0) {
        snprintf(temp, 128, "rotate(%ddeg) ", transform->rotate_z);
        strcat(transform_str, temp);
    }
    
    if (transform->rotate_x != 0) {
        snprintf(temp, 128, "rotateX(%ddeg) ", transform->rotate_x);
        strcat(transform_str, temp);
    }
    
    if (transform->rotate_y != 0) {
        snprintf(temp, 128, "rotateY(%ddeg) ", transform->rotate_y);
        strcat(transform_str, temp);
    }
    
    if (transform->skew_x != 0 || transform->skew_y != 0) {
        snprintf(temp, 128, "skew(%ddeg, %ddeg) ",
                 transform->skew_x, transform->skew_y);
        strcat(transform_str, temp);
    }
    
    if (strlen(transform_str) > 0) {
        ui_set_style(elem, "transform", transform_str);
    }
    
    if (transform->perspective > 0) {
        snprintf(temp, 128, "%dpx", transform->perspective);
        ui_set_style(elem, "perspective", temp);
    }
}

/* Create Glassmorphism Effect */
void ui_apply_glassmorphism(UIElement *elem, int blur, float opacity) {
    char rgba[64];
    snprintf(rgba, 64, "rgba(255, 255, 255, %.2f)", opacity);
    ui_set_style(elem, "background", rgba);
    
    char filter[64];
    snprintf(filter, 64, "blur(%dpx)", blur);
    ui_set_style(elem, "backdrop-filter", filter);
    
    ui_set_style(elem, "border", "1px solid rgba(255, 255, 255, 0.18)");
    ui_set_style(elem, "box-shadow", "0 8px 32px 0 rgba(31, 38, 135, 0.37)");
}

/* Create Neumorphism Effect */
void ui_apply_neumorphism(UIElement *elem, const char *bg_color, int raised) {
    ui_set_style(elem, "background", bg_color);
    ui_set_style(elem, "border-radius", "20px");
    
    if (raised) {
        ui_set_style(elem, "box-shadow",
                    "20px 20px 60px #bebebe, -20px -20px 60px #ffffff");
    } else {
        ui_set_style(elem, "box-shadow",
                    "inset 20px 20px 60px #bebebe, inset -20px -20px 60px #ffffff");
    }
}

/* Create Neon Glow Effect */
void ui_apply_neon_glow(UIElement *elem, const char *color) {
    char shadow[256];
    snprintf(shadow, 256,
             "0 0 5px %s, 0 0 10px %s, 0 0 20px %s, 0 0 40px %s",
             color, color, color, color);
    ui_set_style(elem, "text-shadow", shadow);
    ui_set_style(elem, "color", color);
}

/* Create Parallax Effect */
void ui_apply_parallax(UIElement *elem, const char *image_url, float speed) {
    ui_set_style(elem, "background-image", image_url);
    ui_set_style(elem, "background-attachment", "fixed");
    ui_set_style(elem, "background-position", "center");
    ui_set_style(elem, "background-repeat", "no-repeat");
    ui_set_style(elem, "background-size", "cover");
}

/* Create Hover Effects */
void ui_apply_hover_lift(UIElement *elem) {
    ui_set_style(elem, "transition", "transform 0.3s ease, box-shadow 0.3s ease");
    ui_set_style(elem, "cursor", "pointer");
}

/* Create Ripple Effect */
void ui_apply_ripple_effect(UIElement *elem, const char *color) {
    ui_set_style(elem, "position", "relative");
    ui_set_style(elem, "overflow", "hidden");
}

/* Create Skeleton Loading Effect */
UIElement* ui_create_skeleton_loader(int width, int height) {
    UIElement *skeleton = ui_create_element(UI_CONTAINER, "skeleton");
    skeleton->className = strdup("skeleton-loader");
    
    char width_str[32], height_str[32];
    snprintf(width_str, 32, "%dpx", width);
    snprintf(height_str, 32, "%dpx", height);
    
    ui_set_style(skeleton, "width", width_str);
    ui_set_style(skeleton, "height", height_str);
    ui_set_style(skeleton, "background", "linear-gradient(90deg, #f0f0f0 25%, #e0e0e0 50%, #f0f0f0 75%)");
    ui_set_style(skeleton, "background-size", "200% 100%");
    ui_set_style(skeleton, "animation", "skeleton-loading 1.5s ease-in-out infinite");
    ui_set_style(skeleton, "border-radius", "4px");
    
    return skeleton;
}

/* Create Shimmer Effect */
void ui_apply_shimmer(UIElement *elem) {
    ui_set_style(elem, "background",
                "linear-gradient(90deg, transparent, rgba(255,255,255,0.5), transparent)");
    ui_set_style(elem, "background-size", "200% 100%");
    ui_set_style(elem, "animation", "shimmer 2s infinite");
}

/* Create Blur Background */
void ui_apply_blur_background(UIElement *elem, int blur_amount) {
    char filter[64];
    snprintf(filter, 64, "blur(%dpx)", blur_amount);
    ui_set_style(elem, "backdrop-filter", filter);
    ui_set_style(elem, "background", "rgba(255, 255, 255, 0.1)");
}

/* Create Gradient Border */
void ui_apply_gradient_border(UIElement *elem, Gradient *gradient, int border_width) {
    char gradient_str[512];
    
    snprintf(gradient_str, 512, "linear-gradient(%ddeg", gradient->angle);
    for (int i = 0; i < gradient->stop_count; i++) {
        char stop[64];
        snprintf(stop, 64, ", %s %d%%",
                 gradient->stops[i].color,
                 gradient->stops[i].position);
        strcat(gradient_str, stop);
    }
    strcat(gradient_str, ")");
    
    ui_set_style(elem, "border", "none");
    ui_set_style(elem, "background", gradient_str);
    ui_set_style(elem, "background-clip", "padding-box");
    
    char padding[32];
    snprintf(padding, 32, "%dpx", border_width);
    ui_set_style(elem, "padding", padding);
}

/* Create Text Gradient */
void ui_apply_text_gradient(UIElement *elem, Gradient *gradient) {
    char gradient_str[512];
    
    snprintf(gradient_str, 512, "linear-gradient(%ddeg", gradient->angle);
    for (int i = 0; i < gradient->stop_count; i++) {
        char stop[64];
        snprintf(stop, 64, ", %s %d%%",
                 gradient->stops[i].color,
                 gradient->stops[i].position);
        strcat(gradient_str, stop);
    }
    strcat(gradient_str, ")");
    
    ui_set_style(elem, "background", gradient_str);
    ui_set_style(elem, "-webkit-background-clip", "text");
    ui_set_style(elem, "background-clip", "text");
    ui_set_style(elem, "-webkit-text-fill-color", "transparent");
}

/* Create Frosted Glass Effect */
void ui_apply_frosted_glass(UIElement *elem) {
    ui_set_style(elem, "background", "rgba(255, 255, 255, 0.25)");
    ui_set_style(elem, "backdrop-filter", "blur(10px) saturate(180%)");
    ui_set_style(elem, "border", "1px solid rgba(255, 255, 255, 0.3)");
    ui_set_style(elem, "box-shadow", "0 8px 32px 0 rgba(31, 38, 135, 0.37)");
}

/* Create Particle Effect Background */
UIElement* ui_create_particle_background(int particle_count) {
    UIElement *container = ui_create_element(UI_CONTAINER, "particle-bg");
    container->className = strdup("particle-background");
    
    ui_set_style(container, "position", "fixed");
    ui_set_style(container, "top", "0");
    ui_set_style(container, "left", "0");
    ui_set_style(container, "width", "100%");
    ui_set_style(container, "height", "100%");
    ui_set_style(container, "pointer-events", "none");
    ui_set_style(container, "z-index", "-1");
    
    for (int i = 0; i < particle_count; i++) {
        UIElement *particle = ui_create_element(UI_CONTAINER, NULL);
        particle->className = strdup("particle");
        
        ui_set_style(particle, "position", "absolute");
        ui_set_style(particle, "width", "4px");
        ui_set_style(particle, "height", "4px");
        ui_set_style(particle, "background", "#fff");
        ui_set_style(particle, "border-radius", "50%");
        ui_set_style(particle, "opacity", "0.6");
        
        char anim[128];
        snprintf(anim, 128, "float %ds infinite ease-in-out", 3 + (i % 5));
        ui_set_style(particle, "animation", anim);
        
        ui_add_child(container, particle);
    }
    
    return container;
}
