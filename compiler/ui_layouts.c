/* L# UI Layout System - Advanced Responsive Layouts */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_renderer.h"

/* Layout Types */
typedef enum {
    LAYOUT_FLEX,
    LAYOUT_GRID,
    LAYOUT_ABSOLUTE,
    LAYOUT_RELATIVE,
    LAYOUT_FIXED,
    LAYOUT_STICKY,
    LAYOUT_FLOAT,
    LAYOUT_INLINE,
    LAYOUT_INLINE_BLOCK,
    LAYOUT_TABLE,
    LAYOUT_MASONRY,
    LAYOUT_COLUMNS,
    LAYOUT_STACK
} LayoutType;

/* Flexbox Properties */
typedef struct {
    char *direction;
    char *wrap;
    char *justify_content;
    char *align_items;
    char *align_content;
    int gap;
} FlexLayout;

/* Grid Properties */
typedef struct {
    char *template_columns;
    char *template_rows;
    char *template_areas;
    int column_gap;
    int row_gap;
    char *auto_flow;
    char *auto_columns;
    char *auto_rows;
    char *justify_items;
    char *align_items;
    char *justify_content;
    char *align_content;
} GridLayout;

/* Responsive Breakpoints */
typedef struct {
    int xs;
    int sm;
    int md;
    int lg;
    int xl;
    int xxl;
} Breakpoints;

/* Create Flexbox Layout */
UIElement* ui_create_flex_layout(UIElement **children, int child_count, FlexLayout *props) {
    UIElement *container = ui_create_element(UI_FLEX, "flex-container");
    container->className = strdup("flex-layout");
    
    ui_set_style(container, "display", "flex");
    
    if (props) {
        if (props->direction) {
            ui_set_style(container, "flex-direction", props->direction);
        }
        if (props->wrap) {
            ui_set_style(container, "flex-wrap", props->wrap);
        }
        if (props->justify_content) {
            ui_set_style(container, "justify-content", props->justify_content);
        }
        if (props->align_items) {
            ui_set_style(container, "align-items", props->align_items);
        }
        if (props->align_content) {
            ui_set_style(container, "align-content", props->align_content);
        }
        if (props->gap > 0) {
            ui_set_style_int(container, "gap", props->gap);
        }
    }
    
    for (int i = 0; i < child_count; i++) {
        ui_add_child(container, children[i]);
    }
    
    return container;
}

/* Create Grid Layout */
UIElement* ui_create_grid_layout(UIElement **children, int child_count, GridLayout *props) {
    UIElement *container = ui_create_element(UI_GRID, "grid-container");
    container->className = strdup("grid-layout");
    
    ui_set_style(container, "display", "grid");
    
    if (props) {
        if (props->template_columns) {
            ui_set_style(container, "grid-template-columns", props->template_columns);
        }
        if (props->template_rows) {
            ui_set_style(container, "grid-template-rows", props->template_rows);
        }
        if (props->template_areas) {
            ui_set_style(container, "grid-template-areas", props->template_areas);
        }
        if (props->column_gap > 0) {
            ui_set_style_int(container, "column-gap", props->column_gap);
        }
        if (props->row_gap > 0) {
            ui_set_style_int(container, "row-gap", props->row_gap);
        }
        if (props->auto_flow) {
            ui_set_style(container, "grid-auto-flow", props->auto_flow);
        }
        if (props->justify_items) {
            ui_set_style(container, "justify-items", props->justify_items);
        }
        if (props->align_items) {
            ui_set_style(container, "align-items", props->align_items);
        }
    }
    
    for (int i = 0; i < child_count; i++) {
        ui_add_child(container, children[i]);
    }
    
    return container;
}

/* Create Masonry Layout */
UIElement* ui_create_masonry_layout(UIElement **children, int child_count, int columns) {
    UIElement *container = ui_create_element(UI_CONTAINER, "masonry-container");
    container->className = strdup("masonry-layout");
    
    char columns_str[64];
    snprintf(columns_str, 64, "%d", columns);
    ui_set_style(container, "column-count", columns_str);
    ui_set_style(container, "column-gap", "16px");
    
    for (int i = 0; i < child_count; i++) {
        UIElement *item = ui_create_element(UI_CONTAINER, NULL);
        item->className = strdup("masonry-item");
        ui_set_style(item, "break-inside", "avoid");
        ui_set_style(item, "margin-bottom", "16px");
        ui_add_child(item, children[i]);
        ui_add_child(container, item);
    }
    
    return container;
}

/* Create Responsive Container */
UIElement* ui_create_responsive_container(UIElement *content, Breakpoints *breakpoints) {
    UIElement *container = ui_create_element(UI_CONTAINER, "responsive-container");
    container->className = strdup("container");
    
    ui_set_style(container, "width", "100%");
    ui_set_style(container, "margin-left", "auto");
    ui_set_style(container, "margin-right", "auto");
    ui_set_style(container, "padding-left", "15px");
    ui_set_style(container, "padding-right", "15px");
    
    if (content) {
        ui_add_child(container, content);
    }
    
    return container;
}

/* Create Split Layout */
UIElement* ui_create_split_layout(UIElement *left, UIElement *right, int left_width_percent) {
    UIElement *container = ui_create_element(UI_FLEX, "split-layout");
    container->className = strdup("split-layout");
    
    ui_set_style(container, "display", "flex");
    ui_set_style(container, "width", "100%");
    ui_set_style(container, "height", "100%");
    
    UIElement *leftPane = ui_create_element(UI_CONTAINER, "left-pane");
    leftPane->className = strdup("split-pane left");
    char width_str[32];
    snprintf(width_str, 32, "%d%%", left_width_percent);
    ui_set_style(leftPane, "width", width_str);
    ui_set_style(leftPane, "overflow", "auto");
    if (left) ui_add_child(leftPane, left);
    
    UIElement *rightPane = ui_create_element(UI_CONTAINER, "right-pane");
    rightPane->className = strdup("split-pane right");
    snprintf(width_str, 32, "%d%%", 100 - left_width_percent);
    ui_set_style(rightPane, "width", width_str);
    ui_set_style(rightPane, "overflow", "auto");
    if (right) ui_add_child(rightPane, right);
    
    ui_add_child(container, leftPane);
    ui_add_child(container, rightPane);
    
    return container;
}

/* Create Holy Grail Layout */
UIElement* ui_create_holy_grail_layout(UIElement *header, UIElement *left, UIElement *main, UIElement *right, UIElement *footer) {
    UIElement *container = ui_create_element(UI_CONTAINER, "holy-grail");
    container->className = strdup("holy-grail-layout");
    
    ui_set_style(container, "display", "grid");
    ui_set_style(container, "grid-template-rows", "auto 1fr auto");
    ui_set_style(container, "grid-template-columns", "200px 1fr 200px");
    ui_set_style(container, "min-height", "100vh");
    
    if (header) {
        UIElement *headerWrapper = ui_create_element(UI_HEADER, "header");
        headerWrapper->className = strdup("holy-grail-header");
        ui_set_style(headerWrapper, "grid-column", "1 / -1");
        ui_add_child(headerWrapper, header);
        ui_add_child(container, headerWrapper);
    }
    
    if (left) {
        UIElement *leftWrapper = ui_create_element(UI_SIDEBAR, "left-sidebar");
        leftWrapper->className = strdup("holy-grail-left");
        ui_add_child(leftWrapper, left);
        ui_add_child(container, leftWrapper);
    }
    
    if (main) {
        UIElement *mainWrapper = ui_create_element(UI_CONTAINER, "main-content");
        mainWrapper->className = strdup("holy-grail-main");
        ui_add_child(mainWrapper, main);
        ui_add_child(container, mainWrapper);
    }
    
    if (right) {
        UIElement *rightWrapper = ui_create_element(UI_SIDEBAR, "right-sidebar");
        rightWrapper->className = strdup("holy-grail-right");
        ui_add_child(rightWrapper, right);
        ui_add_child(container, rightWrapper);
    }
    
    if (footer) {
        UIElement *footerWrapper = ui_create_element(UI_FOOTER, "footer");
        footerWrapper->className = strdup("holy-grail-footer");
        ui_set_style(footerWrapper, "grid-column", "1 / -1");
        ui_add_child(footerWrapper, footer);
        ui_add_child(container, footerWrapper);
    }
    
    return container;
}

/* Create Dashboard Layout */
UIElement* ui_create_dashboard_layout(UIElement **widgets, int widget_count, int columns) {
    UIElement *container = ui_create_element(UI_GRID, "dashboard");
    container->className = strdup("dashboard-layout");
    
    char grid_template[128];
    snprintf(grid_template, 128, "repeat(auto-fit, minmax(300px, 1fr))");
    ui_set_style(container, "display", "grid");
    ui_set_style(container, "grid-template-columns", grid_template);
    ui_set_style(container, "gap", "20px");
    ui_set_style(container, "padding", "20px");
    
    for (int i = 0; i < widget_count; i++) {
        UIElement *widget = ui_create_element(UI_CARD, NULL);
        widget->className = strdup("dashboard-widget");
        ui_set_style(widget, "background", "#fff");
        ui_set_style(widget, "border-radius", "8px");
        ui_set_style(widget, "box-shadow", "0 2px 8px rgba(0,0,0,0.1)");
        ui_set_style(widget, "padding", "20px");
        ui_add_child(widget, widgets[i]);
        ui_add_child(container, widget);
    }
    
    return container;
}

/* Create Sidebar Layout */
UIElement* ui_create_sidebar_layout(UIElement *sidebar, UIElement *main, int sidebar_width, int collapsed) {
    UIElement *container = ui_create_element(UI_FLEX, "sidebar-layout");
    container->className = strdup("sidebar-layout");
    
    ui_set_style(container, "display", "flex");
    ui_set_style(container, "height", "100vh");
    
    UIElement *sidebarWrapper = ui_create_element(UI_SIDEBAR, "sidebar");
    sidebarWrapper->className = strdup(collapsed ? "sidebar collapsed" : "sidebar");
    
    char width_str[32];
    snprintf(width_str, 32, "%dpx", collapsed ? 60 : sidebar_width);
    ui_set_style(sidebarWrapper, "width", width_str);
    ui_set_style(sidebarWrapper, "transition", "width 0.3s ease");
    ui_set_style(sidebarWrapper, "overflow", "hidden");
    
    if (sidebar) ui_add_child(sidebarWrapper, sidebar);
    
    UIElement *mainWrapper = ui_create_element(UI_CONTAINER, "main");
    mainWrapper->className = strdup("main-content");
    ui_set_style(mainWrapper, "flex", "1");
    ui_set_style(mainWrapper, "overflow", "auto");
    
    if (main) ui_add_child(mainWrapper, main);
    
    ui_add_child(container, sidebarWrapper);
    ui_add_child(container, mainWrapper);
    
    return container;
}

/* Create Card Grid Layout */
UIElement* ui_create_card_grid(UIElement **cards, int card_count, int min_width, int max_width) {
    UIElement *container = ui_create_element(UI_GRID, "card-grid");
    container->className = strdup("card-grid");
    
    char grid_template[128];
    snprintf(grid_template, 128, "repeat(auto-fill, minmax(%dpx, %dpx))", min_width, max_width);
    ui_set_style(container, "display", "grid");
    ui_set_style(container, "grid-template-columns", grid_template);
    ui_set_style(container, "gap", "24px");
    ui_set_style(container, "padding", "24px");
    
    for (int i = 0; i < card_count; i++) {
        ui_add_child(container, cards[i]);
    }
    
    return container;
}

/* Create Sticky Header Layout */
UIElement* ui_create_sticky_header_layout(UIElement *header, UIElement *content) {
    UIElement *container = ui_create_element(UI_CONTAINER, "sticky-header-layout");
    container->className = strdup("sticky-header-layout");
    
    if (header) {
        UIElement *headerWrapper = ui_create_element(UI_HEADER, "sticky-header");
        headerWrapper->className = strdup("sticky-header");
        ui_set_style(headerWrapper, "position", "sticky");
        ui_set_style(headerWrapper, "top", "0");
        ui_set_style(headerWrapper, "z-index", "100");
        ui_set_style(headerWrapper, "background", "#fff");
        ui_set_style(headerWrapper, "box-shadow", "0 2px 4px rgba(0,0,0,0.1)");
        ui_add_child(headerWrapper, header);
        ui_add_child(container, headerWrapper);
    }
    
    if (content) {
        UIElement *contentWrapper = ui_create_element(UI_CONTAINER, "content");
        contentWrapper->className = strdup("content");
        ui_add_child(contentWrapper, content);
        ui_add_child(container, contentWrapper);
    }
    
    return container;
}

/* Create Centered Layout */
UIElement* ui_create_centered_layout(UIElement *content, int max_width) {
    UIElement *container = ui_create_element(UI_FLEX, "centered-layout");
    container->className = strdup("centered-layout");
    
    ui_set_style(container, "display", "flex");
    ui_set_style(container, "justify-content", "center");
    ui_set_style(container, "align-items", "center");
    ui_set_style(container, "min-height", "100vh");
    ui_set_style(container, "padding", "20px");
    
    UIElement *wrapper = ui_create_element(UI_CONTAINER, "centered-content");
    wrapper->className = strdup("centered-content");
    
    char width_str[32];
    snprintf(width_str, 32, "%dpx", max_width);
    ui_set_style(wrapper, "max-width", width_str);
    ui_set_style(wrapper, "width", "100%");
    
    if (content) ui_add_child(wrapper, content);
    ui_add_child(container, wrapper);
    
    return container;
}

/* Create Multi-Column Layout */
UIElement* ui_create_multi_column_layout(UIElement *content, int columns, int gap) {
    UIElement *container = ui_create_element(UI_CONTAINER, "multi-column");
    container->className = strdup("multi-column-layout");
    
    char columns_str[32];
    snprintf(columns_str, 32, "%d", columns);
    ui_set_style(container, "column-count", columns_str);
    
    char gap_str[32];
    snprintf(gap_str, 32, "%dpx", gap);
    ui_set_style(container, "column-gap", gap_str);
    ui_set_style(container, "column-rule", "1px solid #ddd");
    
    if (content) ui_add_child(container, content);
    
    return container;
}
