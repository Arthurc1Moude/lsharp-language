/* L# UI Rendering Engine - Header */
#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include <stdio.h>

typedef enum {
    UI_CONTAINER,
    UI_TEXT,
    UI_INPUT,
    UI_TEXTAREA,
    UI_BUTTON,
    UI_IMAGE,
    UI_LIST,
    UI_GRID,
    UI_FLEX,
    UI_CARD,
    UI_MODAL,
    UI_DROPDOWN,
    UI_CHECKBOX,
    UI_RADIO,
    UI_SLIDER,
    UI_PROGRESS,
    UI_TABS,
    UI_ACCORDION,
    UI_TOOLTIP,
    UI_MENU,
    UI_NAVBAR,
    UI_SIDEBAR,
    UI_FOOTER,
    UI_HEADER
} UIElementType;

/* Style Properties */
typedef struct {
    char *background_color;
    char *text_color;
    char *border_color;
    int border_width;
    int border_radius;
    int padding_top;
    int padding_right;
    int padding_bottom;
    int padding_left;
    int margin_top;
    int margin_right;
    int margin_bottom;
    int margin_left;
    int width;
    int height;
    char *font_family;
    int font_size;
    int font_weight;
    char *display;
    char *flex_direction;
    char *justify_content;
    char *align_items;
    float opacity;
    char *position;
    int z_index;
    char *overflow;
    char *cursor;
    char *box_shadow;
    char *transition;
} UIStyle;

/* Event Handlers */
typedef struct {
    void (*onClick)(void *data);
    void (*onChange)(void *data, char *value);
    void (*onInput)(void *data, char *value);
    void (*onFocus)(void *data);
    void (*onBlur)(void *data);
    void (*onMouseEnter)(void *data);
    void (*onMouseLeave)(void *data);
    void (*onKeyDown)(void *data, int keyCode);
    void (*onKeyUp)(void *data, int keyCode);
    void (*onSubmit)(void *data);
} UIEventHandlers;

/* UI Element Structure */
typedef struct UIElement {
    UIElementType type;
    char *id;
    char *className;
    char *content;
    UIStyle *style;
    UIEventHandlers *events;
    struct UIElement **children;
    int child_count;
    void *data;
    int visible;
    int enabled;
} UIElement;

/* Core Functions */
UIElement* ui_create_element(UIElementType type, const char *id);
void ui_add_child(UIElement *parent, UIElement *child);
void ui_set_style(UIElement *elem, const char *property, const char *value);
void ui_set_style_int(UIElement *elem, const char *property, int value);
void ui_render_html(UIElement *elem, FILE *output, int indent);
void ui_render_jsx(UIElement *elem, FILE *output, int indent);
void ui_free_element(UIElement *elem);

/* Predefined UI Components */
UIElement* ui_create_chat_input();
UIElement* ui_create_button(const char *text, const char *className);
UIElement* ui_create_input(const char *placeholder, const char *type);
UIElement* ui_create_card(const char *title, const char *content);
UIElement* ui_create_modal(const char *title, UIElement *content);
UIElement* ui_create_navbar(const char *brand, UIElement **items, int item_count);
UIElement* ui_create_sidebar(UIElement **items, int item_count);
UIElement* ui_create_tabs(const char **labels, UIElement **contents, int tab_count);
UIElement* ui_create_dropdown(const char **options, int option_count);
UIElement* ui_create_table(const char **headers, char ***data, int rows, int cols);
UIElement* ui_create_form(UIElement **fields, int field_count);
UIElement* ui_create_grid(UIElement **items, int item_count, int columns);

#endif /* UI_RENDERER_H */
