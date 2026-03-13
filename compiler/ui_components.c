/* L# Built-in UI Components Library */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_renderer.h"

/* Create Button Component */
UIElement* ui_create_button(const char *text, const char *className) {
    UIElement *button = ui_create_element(UI_BUTTON, NULL);
    button->className = className ? strdup(className) : strdup("btn");
    button->content = strdup(text);
    return button;
}

/* Create Input Component */
UIElement* ui_create_input(const char *placeholder, const char *type) {
    UIElement *input = ui_create_element(UI_INPUT, NULL);
    input->className = strdup("input-field");
    
    char *content = malloc(256);
    snprintf(content, 256, "type=\"%s\" placeholder=\"%s\"", 
             type ? type : "text", placeholder ? placeholder : "");
    input->content = content;
    
    return input;
}

/* Create Card Component */
UIElement* ui_create_card(const char *title, const char *content) {
    UIElement *card = ui_create_element(UI_CARD, NULL);
    card->className = strdup("card");
    
    if (title) {
        UIElement *header = ui_create_element(UI_HEADER, NULL);
        header->className = strdup("card-header");
        
        UIElement *titleElem = ui_create_element(UI_TEXT, NULL);
        titleElem->className = strdup("card-title");
        titleElem->content = strdup(title);
        
        ui_add_child(header, titleElem);
        ui_add_child(card, header);
    }
    
    UIElement *body = ui_create_element(UI_CONTAINER, NULL);
    body->className = strdup("card-body");
    
    if (content) {
        UIElement *contentElem = ui_create_element(UI_TEXT, NULL);
        contentElem->content = strdup(content);
        ui_add_child(body, contentElem);
    }
    
    ui_add_child(card, body);
    
    return card;
}

/* Create Modal Component */
UIElement* ui_create_modal(const char *title, UIElement *content) {
    UIElement *overlay = ui_create_element(UI_MODAL, "modal-overlay");
    overlay->className = strdup("modal-overlay");
    
    UIElement *modal = ui_create_element(UI_CONTAINER, "modal-content");
    modal->className = strdup("modal-content");
    
    UIElement *header = ui_create_element(UI_HEADER, NULL);
    header->className = strdup("modal-header");
    
    UIElement *titleElem = ui_create_element(UI_TEXT, NULL);
    titleElem->className = strdup("modal-title");
    titleElem->content = strdup(title);
    
    UIElement *closeBtn = ui_create_element(UI_BUTTON, "modal-close");
    closeBtn->className = strdup("modal-close-btn");
    closeBtn->content = strdup("×");
    
    ui_add_child(header, titleElem);
    ui_add_child(header, closeBtn);
    ui_add_child(modal, header);
    
    if (content) {
        UIElement *body = ui_create_element(UI_CONTAINER, NULL);
        body->className = strdup("modal-body");
        ui_add_child(body, content);
        ui_add_child(modal, body);
    }
    
    ui_add_child(overlay, modal);
    
    return overlay;
}

/* Create Navbar Component */
UIElement* ui_create_navbar(const char *brand, UIElement **items, int item_count) {
    UIElement *navbar = ui_create_element(UI_NAVBAR, "navbar");
    navbar->className = strdup("navbar");
    
    if (brand) {
        UIElement *brandElem = ui_create_element(UI_TEXT, "navbar-brand");
        brandElem->className = strdup("navbar-brand");
        brandElem->content = strdup(brand);
        ui_add_child(navbar, brandElem);
    }
    
    UIElement *navItems = ui_create_element(UI_CONTAINER, "navbar-items");
    navItems->className = strdup("navbar-items");
    
    for (int i = 0; i < item_count; i++) {
        ui_add_child(navItems, items[i]);
    }
    
    ui_add_child(navbar, navItems);
    
    return navbar;
}

/* Create Sidebar Component */
UIElement* ui_create_sidebar(UIElement **items, int item_count) {
    UIElement *sidebar = ui_create_element(UI_SIDEBAR, "sidebar");
    sidebar->className = strdup("sidebar");
    
    UIElement *sidebarContent = ui_create_element(UI_CONTAINER, "sidebar-content");
    sidebarContent->className = strdup("sidebar-content");
    
    for (int i = 0; i < item_count; i++) {
        UIElement *itemWrapper = ui_create_element(UI_CONTAINER, NULL);
        itemWrapper->className = strdup("sidebar-item");
        ui_add_child(itemWrapper, items[i]);
        ui_add_child(sidebarContent, itemWrapper);
    }
    
    ui_add_child(sidebar, sidebarContent);
    
    return sidebar;
}

/* Create Tabs Component */
UIElement* ui_create_tabs(const char **labels, UIElement **contents, int tab_count) {
    UIElement *tabs = ui_create_element(UI_TABS, "tabs");
    tabs->className = strdup("tabs-component");
    
    UIElement *tabHeaders = ui_create_element(UI_CONTAINER, "tab-headers");
    tabHeaders->className = strdup("tab-headers");
    
    for (int i = 0; i < tab_count; i++) {
        UIElement *tabBtn = ui_create_element(UI_BUTTON, NULL);
        tabBtn->className = strdup(i == 0 ? "tab-button active" : "tab-button");
        tabBtn->content = strdup(labels[i]);
        ui_add_child(tabHeaders, tabBtn);
    }
    
    ui_add_child(tabs, tabHeaders);
    
    UIElement *tabContent = ui_create_element(UI_CONTAINER, "tab-content");
    tabContent->className = strdup("tab-content");
    
    for (int i = 0; i < tab_count; i++) {
        UIElement *pane = ui_create_element(UI_CONTAINER, NULL);
        pane->className = strdup(i == 0 ? "tab-pane active" : "tab-pane");
        ui_add_child(pane, contents[i]);
        ui_add_child(tabContent, pane);
    }
    
    ui_add_child(tabs, tabContent);
    
    return tabs;
}

/* Create Dropdown Component */
UIElement* ui_create_dropdown(const char **options, int option_count) {
    UIElement *dropdown = ui_create_element(UI_DROPDOWN, "dropdown");
    dropdown->className = strdup("dropdown");
    
    UIElement *trigger = ui_create_element(UI_BUTTON, "dropdown-trigger");
    trigger->className = strdup("dropdown-trigger");
    trigger->content = strdup("Select...");
    ui_add_child(dropdown, trigger);
    
    UIElement *menu = ui_create_element(UI_MENU, "dropdown-menu");
    menu->className = strdup("dropdown-menu");
    
    for (int i = 0; i < option_count; i++) {
        UIElement *item = ui_create_element(UI_CONTAINER, NULL);
        item->className = strdup("dropdown-item");
        item->content = strdup(options[i]);
        ui_add_child(menu, item);
    }
    
    ui_add_child(dropdown, menu);
    
    return dropdown;
}

/* Create Table Component */
UIElement* ui_create_table(const char **headers, char ***data, int rows, int cols) {
    UIElement *table = ui_create_element(UI_CONTAINER, "data-table");
    table->className = strdup("data-table");
    
    UIElement *thead = ui_create_element(UI_CONTAINER, NULL);
    thead->className = strdup("table-header");
    
    UIElement *headerRow = ui_create_element(UI_CONTAINER, NULL);
    headerRow->className = strdup("table-row");
    
    for (int i = 0; i < cols; i++) {
        UIElement *th = ui_create_element(UI_TEXT, NULL);
        th->className = strdup("table-header-cell");
        th->content = strdup(headers[i]);
        ui_add_child(headerRow, th);
    }
    
    ui_add_child(thead, headerRow);
    ui_add_child(table, thead);
    
    UIElement *tbody = ui_create_element(UI_CONTAINER, NULL);
    tbody->className = strdup("table-body");
    
    for (int r = 0; r < rows; r++) {
        UIElement *row = ui_create_element(UI_CONTAINER, NULL);
        row->className = strdup("table-row");
        
        for (int c = 0; c < cols; c++) {
            UIElement *td = ui_create_element(UI_TEXT, NULL);
            td->className = strdup("table-cell");
            td->content = strdup(data[r][c]);
            ui_add_child(row, td);
        }
        
        ui_add_child(tbody, row);
    }
    
    ui_add_child(table, tbody);
    
    return table;
}

/* Create Form Component */
UIElement* ui_create_form(UIElement **fields, int field_count) {
    UIElement *form = ui_create_element(UI_CONTAINER, "form");
    form->className = strdup("form-component");
    
    for (int i = 0; i < field_count; i++) {
        UIElement *fieldGroup = ui_create_element(UI_CONTAINER, NULL);
        fieldGroup->className = strdup("form-group");
        ui_add_child(fieldGroup, fields[i]);
        ui_add_child(form, fieldGroup);
    }
    
    UIElement *submitBtn = ui_create_element(UI_BUTTON, "form-submit");
    submitBtn->className = strdup("btn btn-primary");
    submitBtn->content = strdup("Submit");
    ui_add_child(form, submitBtn);
    
    return form;
}

/* Create Grid Component */
UIElement* ui_create_grid(UIElement **items, int item_count, int columns) {
    UIElement *grid = ui_create_element(UI_GRID, "grid");
    grid->className = strdup("grid-component");
    
    char style[64];
    snprintf(style, 64, "grid-template-columns: repeat(%d, 1fr)", columns);
    ui_set_style(grid, "display", "grid");
    
    for (int i = 0; i < item_count; i++) {
        UIElement *gridItem = ui_create_element(UI_CONTAINER, NULL);
        gridItem->className = strdup("grid-item");
        ui_add_child(gridItem, items[i]);
        ui_add_child(grid, gridItem);
    }
    
    return grid;
}

/* Create Accordion Component */
UIElement* ui_create_accordion(const char **titles, UIElement **contents, int item_count) {
    UIElement *accordion = ui_create_element(UI_ACCORDION, "accordion");
    accordion->className = strdup("accordion-component");
    
    for (int i = 0; i < item_count; i++) {
        UIElement *item = ui_create_element(UI_CONTAINER, NULL);
        item->className = strdup("accordion-item");
        
        UIElement *header = ui_create_element(UI_BUTTON, NULL);
        header->className = strdup("accordion-header");
        header->content = strdup(titles[i]);
        ui_add_child(item, header);
        
        UIElement *content = ui_create_element(UI_CONTAINER, NULL);
        content->className = strdup("accordion-content");
        ui_add_child(content, contents[i]);
        ui_add_child(item, content);
        
        ui_add_child(accordion, item);
    }
    
    return accordion;
}

/* Create Progress Bar Component */
UIElement* ui_create_progress(int value, int max, const char *label) {
    UIElement *container = ui_create_element(UI_PROGRESS, "progress");
    container->className = strdup("progress-component");
    
    if (label) {
        UIElement *labelElem = ui_create_element(UI_TEXT, NULL);
        labelElem->className = strdup("progress-label");
        labelElem->content = strdup(label);
        ui_add_child(container, labelElem);
    }
    
    UIElement *bar = ui_create_element(UI_CONTAINER, NULL);
    bar->className = strdup("progress-bar");
    
    int percentage = (value * 100) / max;
    ui_set_style_int(bar, "width", percentage);
    
    char percentText[16];
    snprintf(percentText, 16, "%d%%", percentage);
    bar->content = strdup(percentText);
    
    ui_add_child(container, bar);
    
    return container;
}

/* Create Tooltip Component */
UIElement* ui_create_tooltip(UIElement *target, const char *text) {
    UIElement *wrapper = ui_create_element(UI_TOOLTIP, "tooltip-wrapper");
    wrapper->className = strdup("tooltip-wrapper");
    
    ui_add_child(wrapper, target);
    
    UIElement *tooltip = ui_create_element(UI_CONTAINER, NULL);
    tooltip->className = strdup("tooltip-content");
    tooltip->content = strdup(text);
    ui_add_child(wrapper, tooltip);
    
    return wrapper;
}

/* Create Badge Component */
UIElement* ui_create_badge(const char *text, const char *variant) {
    UIElement *badge = ui_create_element(UI_TEXT, NULL);
    
    char className[64];
    snprintf(className, 64, "badge badge-%s", variant ? variant : "default");
    badge->className = strdup(className);
    badge->content = strdup(text);
    
    return badge;
}

/* Create Alert Component */
UIElement* ui_create_alert(const char *message, const char *type) {
    UIElement *alert = ui_create_element(UI_CONTAINER, "alert");
    
    char className[64];
    snprintf(className, 64, "alert alert-%s", type ? type : "info");
    alert->className = strdup(className);
    
    UIElement *icon = ui_create_element(UI_TEXT, NULL);
    icon->className = strdup("alert-icon");
    icon->content = strdup("ℹ");
    ui_add_child(alert, icon);
    
    UIElement *text = ui_create_element(UI_TEXT, NULL);
    text->className = strdup("alert-message");
    text->content = strdup(message);
    ui_add_child(alert, text);
    
    UIElement *closeBtn = ui_create_element(UI_BUTTON, NULL);
    closeBtn->className = strdup("alert-close");
    closeBtn->content = strdup("×");
    ui_add_child(alert, closeBtn);
    
    return alert;
}
