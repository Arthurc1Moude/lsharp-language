/* L# UI Rendering Engine - Native UI Generation */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_renderer.h"

/* Create UI Element */
UIElement* ui_create_element(UIElementType type, const char *id) {
    UIElement *elem = malloc(sizeof(UIElement));
    elem->type = type;
    elem->id = id ? strdup(id) : NULL;
    elem->className = NULL;
    elem->content = NULL;
    elem->style = calloc(1, sizeof(UIStyle));
    elem->events = calloc(1, sizeof(UIEventHandlers));
    elem->children = NULL;
    elem->child_count = 0;
    elem->data = NULL;
    elem->visible = 1;
    elem->enabled = 1;
    return elem;
}

/* Add Child Element */
void ui_add_child(UIElement *parent, UIElement *child) {
    parent->child_count++;
    parent->children = realloc(parent->children, parent->child_count * sizeof(UIElement*));
    parent->children[parent->child_count - 1] = child;
}

/* Set Style Property */
void ui_set_style(UIElement *elem, const char *property, const char *value) {
    if (strcmp(property, "background-color") == 0) {
        elem->style->background_color = strdup(value);
    } else if (strcmp(property, "color") == 0) {
        elem->style->text_color = strdup(value);
    } else if (strcmp(property, "border-color") == 0) {
        elem->style->border_color = strdup(value);
    } else if (strcmp(property, "font-family") == 0) {
        elem->style->font_family = strdup(value);
    } else if (strcmp(property, "display") == 0) {
        elem->style->display = strdup(value);
    } else if (strcmp(property, "flex-direction") == 0) {
        elem->style->flex_direction = strdup(value);
    } else if (strcmp(property, "justify-content") == 0) {
        elem->style->justify_content = strdup(value);
    } else if (strcmp(property, "align-items") == 0) {
        elem->style->align_items = strdup(value);
    }
}

void ui_set_style_int(UIElement *elem, const char *property, int value) {
    if (strcmp(property, "width") == 0) {
        elem->style->width = value;
    } else if (strcmp(property, "height") == 0) {
        elem->style->height = value;
    } else if (strcmp(property, "padding") == 0) {
        elem->style->padding_top = elem->style->padding_right = 
        elem->style->padding_bottom = elem->style->padding_left = value;
    } else if (strcmp(property, "margin") == 0) {
        elem->style->margin_top = elem->style->margin_right = 
        elem->style->margin_bottom = elem->style->margin_left = value;
    } else if (strcmp(property, "border-width") == 0) {
        elem->style->border_width = value;
    } else if (strcmp(property, "border-radius") == 0) {
        elem->style->border_radius = value;
    } else if (strcmp(property, "font-size") == 0) {
        elem->style->font_size = value;
    } else if (strcmp(property, "z-index") == 0) {
        elem->style->z_index = value;
    }
}

/* Render to HTML */
void ui_render_html(UIElement *elem, FILE *output, int indent) {
    const char *tag_name = NULL;
    
    switch (elem->type) {
        case UI_CONTAINER: tag_name = "div"; break;
        case UI_TEXT: tag_name = "span"; break;
        case UI_INPUT: tag_name = "input"; break;
        case UI_TEXTAREA: tag_name = "textarea"; break;
        case UI_BUTTON: tag_name = "button"; break;
        case UI_IMAGE: tag_name = "img"; break;
        case UI_HEADER: tag_name = "header"; break;
        case UI_FOOTER: tag_name = "footer"; break;
        case UI_NAVBAR: tag_name = "nav"; break;
        case UI_SIDEBAR: tag_name = "aside"; break;
        default: tag_name = "div"; break;
    }
    
    for (int i = 0; i < indent; i++) fprintf(output, "  ");
    fprintf(output, "<%s", tag_name);
    
    if (elem->id) {
        fprintf(output, " id=\"%s\"", elem->id);
    }
    
    if (elem->className) {
        fprintf(output, " class=\"%s\"", elem->className);
    }
    
    if (elem->style) {
        fprintf(output, " style=\"");
        if (elem->style->background_color) {
            fprintf(output, "background-color:%s;", elem->style->background_color);
        }
        if (elem->style->text_color) {
            fprintf(output, "color:%s;", elem->style->text_color);
        }
        if (elem->style->width > 0) {
            fprintf(output, "width:%dpx;", elem->style->width);
        }
        if (elem->style->height > 0) {
            fprintf(output, "height:%dpx;", elem->style->height);
        }
        if (elem->style->padding_top > 0) {
            fprintf(output, "padding:%dpx %dpx %dpx %dpx;", 
                   elem->style->padding_top, elem->style->padding_right,
                   elem->style->padding_bottom, elem->style->padding_left);
        }
        if (elem->style->display) {
            fprintf(output, "display:%s;", elem->style->display);
        }
        if (elem->style->flex_direction) {
            fprintf(output, "flex-direction:%s;", elem->style->flex_direction);
        }
        fprintf(output, "\"");
    }
    
    fprintf(output, ">");
    
    if (elem->content) {
        fprintf(output, "%s", elem->content);
    }
    
    if (elem->child_count > 0) {
        fprintf(output, "\n");
        for (int i = 0; i < elem->child_count; i++) {
            ui_render_html(elem->children[i], output, indent + 1);
        }
        for (int i = 0; i < indent; i++) fprintf(output, "  ");
    }
    
    fprintf(output, "</%s>\n", tag_name);
}

/* Render to React JSX */
void ui_render_jsx(UIElement *elem, FILE *output, int indent) {
    for (int i = 0; i < indent; i++) fprintf(output, "  ");
    
    fprintf(output, "<div");
    
    if (elem->id) {
        fprintf(output, " id=\"%s\"", elem->id);
    }
    
    if (elem->className) {
        fprintf(output, " className=\"%s\"", elem->className);
    }
    
    if (elem->style) {
        fprintf(output, " style={{");
        int first = 1;
        
        if (elem->style->background_color) {
            if (!first) fprintf(output, ", ");
            fprintf(output, "backgroundColor: '%s'", elem->style->background_color);
            first = 0;
        }
        if (elem->style->text_color) {
            if (!first) fprintf(output, ", ");
            fprintf(output, "color: '%s'", elem->style->text_color);
            first = 0;
        }
        if (elem->style->width > 0) {
            if (!first) fprintf(output, ", ");
            fprintf(output, "width: %d", elem->style->width);
            first = 0;
        }
        if (elem->style->height > 0) {
            if (!first) fprintf(output, ", ");
            fprintf(output, "height: %d", elem->style->height);
            first = 0;
        }
        if (elem->style->display) {
            if (!first) fprintf(output, ", ");
            fprintf(output, "display: '%s'", elem->style->display);
            first = 0;
        }
        
        fprintf(output, "}}");
    }
    
    if (elem->events && elem->events->onClick) {
        fprintf(output, " onClick={handleClick}");
    }
    
    fprintf(output, ">");
    
    if (elem->content) {
        fprintf(output, "\n");
        for (int i = 0; i <= indent; i++) fprintf(output, "  ");
        fprintf(output, "{%s}", elem->content);
    }
    
    if (elem->child_count > 0) {
        fprintf(output, "\n");
        for (int i = 0; i < elem->child_count; i++) {
            ui_render_jsx(elem->children[i], output, indent + 1);
        }
        for (int i = 0; i < indent; i++) fprintf(output, "  ");
    }
    
    fprintf(output, "</div>\n");
}

/* Create Chat Input UI (Example) */
UIElement* ui_create_chat_input() {
    UIElement *container = ui_create_element(UI_CONTAINER, "chat-input-section");
    container->className = "chat-input-section";
    
    UIElement *wrapper = ui_create_element(UI_CONTAINER, "chat-input-wrapper");
    wrapper->className = "chat-input-wrapper";
    ui_add_child(container, wrapper);
    
    UIElement *mainArea = ui_create_element(UI_CONTAINER, "input-main-area");
    mainArea->className = "input-main-area";
    ui_add_child(wrapper, mainArea);
    
    UIElement *textarea = ui_create_element(UI_TEXTAREA, "chat-input");
    textarea->className = "chat-input";
    textarea->content = "placeholder=\"Ask AI to help edit files...\"";
    ui_set_style_int(textarea, "rows", 2);
    ui_add_child(mainArea, textarea);
    
    UIElement *sendBtn = ui_create_element(UI_BUTTON, "send-btn");
    sendBtn->className = "send-btn";
    sendBtn->content = "<Icon name=\"arrowUp\" size={18} />";
    ui_add_child(mainArea, sendBtn);
    
    UIElement *bottomActions = ui_create_element(UI_CONTAINER, "input-bottom-actions");
    bottomActions->className = "input-bottom-actions";
    ui_add_child(wrapper, bottomActions);
    
    UIElement *attachBtn = ui_create_element(UI_BUTTON, "attach-btn");
    attachBtn->className = "icon-btn-small";
    attachBtn->content = "<Icon name=\"add\" size={14} />";
    ui_add_child(bottomActions, attachBtn);
    
    UIElement *contextBtn = ui_create_element(UI_BUTTON, "context-btn");
    contextBtn->className = "icon-btn-small";
    contextBtn->content = "<Icon name=\"code\" size={14} />";
    ui_add_child(bottomActions, contextBtn);
    
    UIElement *spacer = ui_create_element(UI_CONTAINER, NULL);
    spacer->className = "spacer";
    ui_add_child(bottomActions, spacer);
    
    UIElement *modelSelector = ui_create_element(UI_CONTAINER, "model-selector-wrapper");
    modelSelector->className = "model-selector-wrapper";
    ui_add_child(bottomActions, modelSelector);
    
    UIElement *modelBtn = ui_create_element(UI_BUTTON, "model-selector-btn");
    modelBtn->className = "model-selector-btn-small";
    modelBtn->content = "{selectedModel}<Icon name=\"chevronDown\" size={12} />";
    ui_add_child(modelSelector, modelBtn);
    
    return container;
}

/* Free UI Element */
void ui_free_element(UIElement *elem) {
    if (!elem) return;
    
    free(elem->id);
    free(elem->className);
    free(elem->content);
    
    if (elem->style) {
        free(elem->style->background_color);
        free(elem->style->text_color);
        free(elem->style->border_color);
        free(elem->style->font_family);
        free(elem->style->display);
        free(elem->style->flex_direction);
        free(elem->style->justify_content);
        free(elem->style->align_items);
        free(elem->style);
    }
    
    free(elem->events);
    
    for (int i = 0; i < elem->child_count; i++) {
        ui_free_element(elem->children[i]);
    }
    free(elem->children);
    
    free(elem);
}
