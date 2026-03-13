/* L# UI Animation System - Complete Animation Engine */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ui_renderer.h"

/* Animation Types */
typedef enum {
    ANIM_FADE_IN,
    ANIM_FADE_OUT,
    ANIM_SLIDE_IN_LEFT,
    ANIM_SLIDE_IN_RIGHT,
    ANIM_SLIDE_IN_TOP,
    ANIM_SLIDE_IN_BOTTOM,
    ANIM_SLIDE_OUT_LEFT,
    ANIM_SLIDE_OUT_RIGHT,
    ANIM_SLIDE_OUT_TOP,
    ANIM_SLIDE_OUT_BOTTOM,
    ANIM_SCALE_IN,
    ANIM_SCALE_OUT,
    ANIM_ROTATE_IN,
    ANIM_ROTATE_OUT,
    ANIM_BOUNCE,
    ANIM_SHAKE,
    ANIM_PULSE,
    ANIM_FLIP,
    ANIM_ZOOM_IN,
    ANIM_ZOOM_OUT,
    ANIM_SWING,
    ANIM_WOBBLE,
    ANIM_JELLO,
    ANIM_HEARTBEAT,
    ANIM_FLASH,
    ANIM_RUBBER_BAND,
    ANIM_TADA,
    ANIM_ROLL_IN,
    ANIM_ROLL_OUT,
    ANIM_LIGHT_SPEED_IN,
    ANIM_LIGHT_SPEED_OUT,
    ANIM_HINGE,
    ANIM_JACK_IN_THE_BOX,
    ANIM_BACK_IN_DOWN,
    ANIM_BACK_IN_UP,
    ANIM_BACK_OUT_DOWN,
    ANIM_BACK_OUT_UP,
    ANIM_FLIP_IN_X,
    ANIM_FLIP_IN_Y,
    ANIM_FLIP_OUT_X,
    ANIM_FLIP_OUT_Y,
    ANIM_ROTATE_IN_DOWN_LEFT,
    ANIM_ROTATE_IN_DOWN_RIGHT,
    ANIM_ROTATE_IN_UP_LEFT,
    ANIM_ROTATE_IN_UP_RIGHT,
    ANIM_ROTATE_OUT_DOWN_LEFT,
    ANIM_ROTATE_OUT_DOWN_RIGHT,
    ANIM_ROTATE_OUT_UP_LEFT,
    ANIM_ROTATE_OUT_UP_RIGHT
} AnimationType;

/* Easing Functions */
typedef enum {
    EASE_LINEAR,
    EASE_IN_QUAD,
    EASE_OUT_QUAD,
    EASE_IN_OUT_QUAD,
    EASE_IN_CUBIC,
    EASE_OUT_CUBIC,
    EASE_IN_OUT_CUBIC,
    EASE_IN_QUART,
    EASE_OUT_QUART,
    EASE_IN_OUT_QUART,
    EASE_IN_QUINT,
    EASE_OUT_QUINT,
    EASE_IN_OUT_QUINT,
    EASE_IN_SINE,
    EASE_OUT_SINE,
    EASE_IN_OUT_SINE,
    EASE_IN_EXPO,
    EASE_OUT_EXPO,
    EASE_IN_OUT_EXPO,
    EASE_IN_CIRC,
    EASE_OUT_CIRC,
    EASE_IN_OUT_CIRC,
    EASE_IN_BACK,
    EASE_OUT_BACK,
    EASE_IN_OUT_BACK,
    EASE_IN_ELASTIC,
    EASE_OUT_ELASTIC,
    EASE_IN_OUT_ELASTIC,
    EASE_IN_BOUNCE,
    EASE_OUT_BOUNCE,
    EASE_IN_OUT_BOUNCE
} EasingType;

/* Animation Structure */
typedef struct {
    AnimationType type;
    EasingType easing;
    float duration;
    float delay;
    int iteration_count;
    int alternate;
    float fill_mode;
} Animation;

/* Generate CSS Animation */
void generate_css_animation(FILE *output, AnimationType type, const char *name) {
    fprintf(output, "@keyframes %s {\n", name);
    
    switch (type) {
        case ANIM_FADE_IN:
            fprintf(output, "  from { opacity: 0; }\n");
            fprintf(output, "  to { opacity: 1; }\n");
            break;
            
        case ANIM_FADE_OUT:
            fprintf(output, "  from { opacity: 1; }\n");
            fprintf(output, "  to { opacity: 0; }\n");
            break;
            
        case ANIM_SLIDE_IN_LEFT:
            fprintf(output, "  from { transform: translateX(-100%%); opacity: 0; }\n");
            fprintf(output, "  to { transform: translateX(0); opacity: 1; }\n");
            break;
            
        case ANIM_SLIDE_IN_RIGHT:
            fprintf(output, "  from { transform: translateX(100%%); opacity: 0; }\n");
            fprintf(output, "  to { transform: translateX(0); opacity: 1; }\n");
            break;
            
        case ANIM_SLIDE_IN_TOP:
            fprintf(output, "  from { transform: translateY(-100%%); opacity: 0; }\n");
            fprintf(output, "  to { transform: translateY(0); opacity: 1; }\n");
            break;
            
        case ANIM_SLIDE_IN_BOTTOM:
            fprintf(output, "  from { transform: translateY(100%%); opacity: 0; }\n");
            fprintf(output, "  to { transform: translateY(0); opacity: 1; }\n");
            break;
            
        case ANIM_SCALE_IN:
            fprintf(output, "  from { transform: scale(0); opacity: 0; }\n");
            fprintf(output, "  to { transform: scale(1); opacity: 1; }\n");
            break;
            
        case ANIM_SCALE_OUT:
            fprintf(output, "  from { transform: scale(1); opacity: 1; }\n");
            fprintf(output, "  to { transform: scale(0); opacity: 0; }\n");
            break;
            
        case ANIM_ROTATE_IN:
            fprintf(output, "  from { transform: rotate(-200deg); opacity: 0; }\n");
            fprintf(output, "  to { transform: rotate(0); opacity: 1; }\n");
            break;
            
        case ANIM_ROTATE_OUT:
            fprintf(output, "  from { transform: rotate(0); opacity: 1; }\n");
            fprintf(output, "  to { transform: rotate(200deg); opacity: 0; }\n");
            break;
            
        case ANIM_BOUNCE:
            fprintf(output, "  0%%, 20%%, 53%%, 80%%, 100%% { transform: translateY(0); }\n");
            fprintf(output, "  40%%, 43%% { transform: translateY(-30px); }\n");
            fprintf(output, "  70%% { transform: translateY(-15px); }\n");
            fprintf(output, "  90%% { transform: translateY(-4px); }\n");
            break;
            
        case ANIM_SHAKE:
            fprintf(output, "  0%%, 100%% { transform: translateX(0); }\n");
            fprintf(output, "  10%%, 30%%, 50%%, 70%%, 90%% { transform: translateX(-10px); }\n");
            fprintf(output, "  20%%, 40%%, 60%%, 80%% { transform: translateX(10px); }\n");
            break;
            
        case ANIM_PULSE:
            fprintf(output, "  0%% { transform: scale(1); }\n");
            fprintf(output, "  50%% { transform: scale(1.05); }\n");
            fprintf(output, "  100%% { transform: scale(1); }\n");
            break;
            
        case ANIM_FLIP:
            fprintf(output, "  from { transform: perspective(400px) rotateY(0); }\n");
            fprintf(output, "  to { transform: perspective(400px) rotateY(360deg); }\n");
            break;
            
        case ANIM_ZOOM_IN:
            fprintf(output, "  from { transform: scale(0.3); opacity: 0; }\n");
            fprintf(output, "  50%% { opacity: 1; }\n");
            fprintf(output, "  to { transform: scale(1); }\n");
            break;
            
        case ANIM_ZOOM_OUT:
            fprintf(output, "  from { transform: scale(1); opacity: 1; }\n");
            fprintf(output, "  50%% { opacity: 0; }\n");
            fprintf(output, "  to { transform: scale(0.3); opacity: 0; }\n");
            break;
            
        case ANIM_SWING:
            fprintf(output, "  20%% { transform: rotate(15deg); }\n");
            fprintf(output, "  40%% { transform: rotate(-10deg); }\n");
            fprintf(output, "  60%% { transform: rotate(5deg); }\n");
            fprintf(output, "  80%% { transform: rotate(-5deg); }\n");
            fprintf(output, "  100%% { transform: rotate(0deg); }\n");
            break;
            
        case ANIM_WOBBLE:
            fprintf(output, "  0%% { transform: translateX(0%%); }\n");
            fprintf(output, "  15%% { transform: translateX(-25%%) rotate(-5deg); }\n");
            fprintf(output, "  30%% { transform: translateX(20%%) rotate(3deg); }\n");
            fprintf(output, "  45%% { transform: translateX(-15%%) rotate(-3deg); }\n");
            fprintf(output, "  60%% { transform: translateX(10%%) rotate(2deg); }\n");
            fprintf(output, "  75%% { transform: translateX(-5%%) rotate(-1deg); }\n");
            fprintf(output, "  100%% { transform: translateX(0%%); }\n");
            break;
            
        case ANIM_JELLO:
            fprintf(output, "  0%%, 11.1%%, 100%% { transform: skewX(0deg) skewY(0deg); }\n");
            fprintf(output, "  22.2%% { transform: skewX(-12.5deg) skewY(-12.5deg); }\n");
            fprintf(output, "  33.3%% { transform: skewX(6.25deg) skewY(6.25deg); }\n");
            fprintf(output, "  44.4%% { transform: skewX(-3.125deg) skewY(-3.125deg); }\n");
            fprintf(output, "  55.5%% { transform: skewX(1.5625deg) skewY(1.5625deg); }\n");
            fprintf(output, "  66.6%% { transform: skewX(-0.78125deg) skewY(-0.78125deg); }\n");
            fprintf(output, "  77.7%% { transform: skewX(0.390625deg) skewY(0.390625deg); }\n");
            fprintf(output, "  88.8%% { transform: skewX(-0.1953125deg) skewY(-0.1953125deg); }\n");
            break;
            
        case ANIM_HEARTBEAT:
            fprintf(output, "  0%% { transform: scale(1); }\n");
            fprintf(output, "  14%% { transform: scale(1.3); }\n");
            fprintf(output, "  28%% { transform: scale(1); }\n");
            fprintf(output, "  42%% { transform: scale(1.3); }\n");
            fprintf(output, "  70%% { transform: scale(1); }\n");
            break;
            
        case ANIM_FLASH:
            fprintf(output, "  0%%, 50%%, 100%% { opacity: 1; }\n");
            fprintf(output, "  25%%, 75%% { opacity: 0; }\n");
            break;
            
        case ANIM_RUBBER_BAND:
            fprintf(output, "  0%% { transform: scale(1); }\n");
            fprintf(output, "  30%% { transform: scaleX(1.25) scaleY(0.75); }\n");
            fprintf(output, "  40%% { transform: scaleX(0.75) scaleY(1.25); }\n");
            fprintf(output, "  50%% { transform: scaleX(1.15) scaleY(0.85); }\n");
            fprintf(output, "  65%% { transform: scaleX(0.95) scaleY(1.05); }\n");
            fprintf(output, "  75%% { transform: scaleX(1.05) scaleY(0.95); }\n");
            fprintf(output, "  100%% { transform: scale(1); }\n");
            break;
            
        case ANIM_TADA:
            fprintf(output, "  0%% { transform: scale(1) rotate(0deg); }\n");
            fprintf(output, "  10%%, 20%% { transform: scale(0.9) rotate(-3deg); }\n");
            fprintf(output, "  30%%, 50%%, 70%%, 90%% { transform: scale(1.1) rotate(3deg); }\n");
            fprintf(output, "  40%%, 60%%, 80%% { transform: scale(1.1) rotate(-3deg); }\n");
            fprintf(output, "  100%% { transform: scale(1) rotate(0deg); }\n");
            break;
            
        case ANIM_ROLL_IN:
            fprintf(output, "  from { opacity: 0; transform: translateX(-100%%) rotate(-120deg); }\n");
            fprintf(output, "  to { opacity: 1; transform: translateX(0) rotate(0deg); }\n");
            break;
            
        case ANIM_ROLL_OUT:
            fprintf(output, "  from { opacity: 1; transform: translateX(0) rotate(0deg); }\n");
            fprintf(output, "  to { opacity: 0; transform: translateX(100%%) rotate(120deg); }\n");
            break;
            
        case ANIM_LIGHT_SPEED_IN:
            fprintf(output, "  from { transform: translateX(100%%) skewX(-30deg); opacity: 0; }\n");
            fprintf(output, "  60%% { transform: translateX(-20%%) skewX(20deg); opacity: 1; }\n");
            fprintf(output, "  80%% { transform: translateX(0) skewX(-5deg); }\n");
            fprintf(output, "  to { transform: translateX(0) skewX(0deg); }\n");
            break;
            
        case ANIM_LIGHT_SPEED_OUT:
            fprintf(output, "  from { opacity: 1; }\n");
            fprintf(output, "  to { transform: translateX(100%%) skewX(30deg); opacity: 0; }\n");
            break;
            
        case ANIM_HINGE:
            fprintf(output, "  0%% { transform-origin: top left; }\n");
            fprintf(output, "  20%%, 60%% { transform: rotate(80deg); }\n");
            fprintf(output, "  40%%, 80%% { transform: rotate(60deg); opacity: 1; }\n");
            fprintf(output, "  100%% { transform: translateY(700px); opacity: 0; }\n");
            break;
            
        case ANIM_JACK_IN_THE_BOX:
            fprintf(output, "  from { opacity: 0; transform: scale(0.1) rotate(30deg); }\n");
            fprintf(output, "  50%% { transform: rotate(-10deg); }\n");
            fprintf(output, "  70%% { transform: rotate(3deg); }\n");
            fprintf(output, "  to { opacity: 1; transform: scale(1); }\n");
            break;
    }
    
    fprintf(output, "}\n\n");
}

/* Generate Easing Function CSS */
const char* get_easing_function(EasingType easing) {
    switch (easing) {
        case EASE_LINEAR: return "linear";
        case EASE_IN_QUAD: return "cubic-bezier(0.55, 0.085, 0.68, 0.53)";
        case EASE_OUT_QUAD: return "cubic-bezier(0.25, 0.46, 0.45, 0.94)";
        case EASE_IN_OUT_QUAD: return "cubic-bezier(0.455, 0.03, 0.515, 0.955)";
        case EASE_IN_CUBIC: return "cubic-bezier(0.55, 0.055, 0.675, 0.19)";
        case EASE_OUT_CUBIC: return "cubic-bezier(0.215, 0.61, 0.355, 1)";
        case EASE_IN_OUT_CUBIC: return "cubic-bezier(0.645, 0.045, 0.355, 1)";
        case EASE_IN_QUART: return "cubic-bezier(0.895, 0.03, 0.685, 0.22)";
        case EASE_OUT_QUART: return "cubic-bezier(0.165, 0.84, 0.44, 1)";
        case EASE_IN_OUT_QUART: return "cubic-bezier(0.77, 0, 0.175, 1)";
        case EASE_IN_QUINT: return "cubic-bezier(0.755, 0.05, 0.855, 0.06)";
        case EASE_OUT_QUINT: return "cubic-bezier(0.23, 1, 0.32, 1)";
        case EASE_IN_OUT_QUINT: return "cubic-bezier(0.86, 0, 0.07, 1)";
        case EASE_IN_SINE: return "cubic-bezier(0.47, 0, 0.745, 0.715)";
        case EASE_OUT_SINE: return "cubic-bezier(0.39, 0.575, 0.565, 1)";
        case EASE_IN_OUT_SINE: return "cubic-bezier(0.445, 0.05, 0.55, 0.95)";
        case EASE_IN_EXPO: return "cubic-bezier(0.95, 0.05, 0.795, 0.035)";
        case EASE_OUT_EXPO: return "cubic-bezier(0.19, 1, 0.22, 1)";
        case EASE_IN_OUT_EXPO: return "cubic-bezier(1, 0, 0, 1)";
        case EASE_IN_CIRC: return "cubic-bezier(0.6, 0.04, 0.98, 0.335)";
        case EASE_OUT_CIRC: return "cubic-bezier(0.075, 0.82, 0.165, 1)";
        case EASE_IN_OUT_CIRC: return "cubic-bezier(0.785, 0.135, 0.15, 0.86)";
        case EASE_IN_BACK: return "cubic-bezier(0.6, -0.28, 0.735, 0.045)";
        case EASE_OUT_BACK: return "cubic-bezier(0.175, 0.885, 0.32, 1.275)";
        case EASE_IN_OUT_BACK: return "cubic-bezier(0.68, -0.55, 0.265, 1.55)";
        default: return "ease";
    }
}

/* Apply Animation to Element */
void ui_apply_animation(UIElement *elem, Animation *anim, const char *anim_name) {
    char animation_str[512];
    snprintf(animation_str, 512, "%s %.2fs %s %.2fs %s %s",
             anim_name,
             anim->duration,
             get_easing_function(anim->easing),
             anim->delay,
             anim->iteration_count < 0 ? "infinite" : "",
             anim->alternate ? "alternate" : "normal");
    
    ui_set_style(elem, "animation", animation_str);
}
