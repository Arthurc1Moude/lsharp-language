/* Accessibility System for L# Language
 * ARIA attributes, keyboard navigation, screen reader support, focus management
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_renderer.h"

/* ARIA Roles */
typedef enum {
    ARIA_ALERT,
    ARIA_ALERTDIALOG,
    ARIA_APPLICATION,
    ARIA_ARTICLE,
    ARIA_BANNER,
    ARIA_BUTTON,
    ARIA_CHECKBOX,
    ARIA_COLUMNHEADER,
    ARIA_COMBOBOX,
    ARIA_COMPLEMENTARY,
    ARIA_CONTENTINFO,
    ARIA_DEFINITION,
    ARIA_DIALOG,
    ARIA_DIRECTORY,
    ARIA_DOCUMENT,
    ARIA_FEED,
    ARIA_FIGURE,
    ARIA_FORM,
    ARIA_GRID,
    ARIA_GRIDCELL,
    ARIA_GROUP,
    ARIA_HEADING,
    ARIA_IMG,
    ARIA_LINK,
    ARIA_LIST,
    ARIA_LISTBOX,
    ARIA_LISTITEM,
    ARIA_LOG,
    ARIA_MAIN,
    ARIA_MARQUEE,
    ARIA_MATH,
    ARIA_MENU,
    ARIA_MENUBAR,
    ARIA_MENUITEM,
    ARIA_MENUITEMCHECKBOX,
    ARIA_MENUITEMRADIO,
    ARIA_NAVIGATION,
    ARIA_NONE,
    ARIA_NOTE,
    ARIA_OPTION,
    ARIA_PRESENTATION,
    ARIA_PROGRESSBAR,
    ARIA_RADIO,
    ARIA_RADIOGROUP,
    ARIA_REGION,
    ARIA_ROW,
    ARIA_ROWGROUP,
    ARIA_ROWHEADER,
    ARIA_SCROLLBAR,
    ARIA_SEARCH,
    ARIA_SEARCHBOX,
    ARIA_SEPARATOR,
    ARIA_SLIDER,
    ARIA_SPINBUTTON,
    ARIA_STATUS,
    ARIA_SWITCH,
    ARIA_TAB,
    ARIA_TABLE,
    ARIA_TABLIST,
    ARIA_TABPANEL,
    ARIA_TERM,
    ARIA_TEXTBOX,
    ARIA_TIMER,
    ARIA_TOOLBAR,
    ARIA_TOOLTIP,
    ARIA_TREE,
    ARIA_TREEGRID,
    ARIA_TREEITEM
} AriaRole;

const char* get_aria_role_string(AriaRole role) {
    switch(role) {
        case ARIA_ALERT: return "alert";
        case ARIA_ALERTDIALOG: return "alertdialog";
        case ARIA_APPLICATION: return "application";
        case ARIA_ARTICLE: return "article";
        case ARIA_BANNER: return "banner";
        case ARIA_BUTTON: return "button";
        case ARIA_CHECKBOX: return "checkbox";
        case ARIA_COLUMNHEADER: return "columnheader";
        case ARIA_COMBOBOX: return "combobox";
        case ARIA_COMPLEMENTARY: return "complementary";
        case ARIA_CONTENTINFO: return "contentinfo";
        case ARIA_DEFINITION: return "definition";
        case ARIA_DIALOG: return "dialog";
        case ARIA_DIRECTORY: return "directory";
        case ARIA_DOCUMENT: return "document";
        case ARIA_FEED: return "feed";
        case ARIA_FIGURE: return "figure";
        case ARIA_FORM: return "form";
        case ARIA_GRID: return "grid";
        case ARIA_GRIDCELL: return "gridcell";
        case ARIA_GROUP: return "group";
        case ARIA_HEADING: return "heading";
        case ARIA_IMG: return "img";
        case ARIA_LINK: return "link";
        case ARIA_LIST: return "list";
        case ARIA_LISTBOX: return "listbox";
        case ARIA_LISTITEM: return "listitem";
        case ARIA_LOG: return "log";
        case ARIA_MAIN: return "main";
        case ARIA_MARQUEE: return "marquee";
        case ARIA_MATH: return "math";
        case ARIA_MENU: return "menu";
        case ARIA_MENUBAR: return "menubar";
        case ARIA_MENUITEM: return "menuitem";
        case ARIA_MENUITEMCHECKBOX: return "menuitemcheckbox";
        case ARIA_MENUITEMRADIO: return "menuitemradio";
        case ARIA_NAVIGATION: return "navigation";
        case ARIA_NONE: return "none";
        case ARIA_NOTE: return "note";
        case ARIA_OPTION: return "option";
        case ARIA_PRESENTATION: return "presentation";
        case ARIA_PROGRESSBAR: return "progressbar";
        case ARIA_RADIO: return "radio";
        case ARIA_RADIOGROUP: return "radiogroup";
        case ARIA_REGION: return "region";
        case ARIA_ROW: return "row";
        case ARIA_ROWGROUP: return "rowgroup";
        case ARIA_ROWHEADER: return "rowheader";
        case ARIA_SCROLLBAR: return "scrollbar";
        case ARIA_SEARCH: return "search";
        case ARIA_SEARCHBOX: return "searchbox";
        case ARIA_SEPARATOR: return "separator";
        case ARIA_SLIDER: return "slider";
        case ARIA_SPINBUTTON: return "spinbutton";
        case ARIA_STATUS: return "status";
        case ARIA_SWITCH: return "switch";
        case ARIA_TAB: return "tab";
        case ARIA_TABLE: return "table";
        case ARIA_TABLIST: return "tablist";
        case ARIA_TABPANEL: return "tabpanel";
        case ARIA_TERM: return "term";
        case ARIA_TEXTBOX: return "textbox";
        case ARIA_TIMER: return "timer";
        case ARIA_TOOLBAR: return "toolbar";
        case ARIA_TOOLTIP: return "tooltip";
        case ARIA_TREE: return "tree";
        case ARIA_TREEGRID: return "treegrid";
        case ARIA_TREEITEM: return "treeitem";
        default: return "none";
    }
}

/* ARIA Attributes */
void add_aria_role(UIElement *element, AriaRole role) {
    char attr[128];
    snprintf(attr, sizeof(attr), "role=\"%s\"", get_aria_role_string(role));
    add_attribute(element, attr);
}

void add_aria_label(UIElement *element, const char *label) {
    char attr[512];
    snprintf(attr, sizeof(attr), "aria-label=\"%s\"", label);
    add_attribute(element, attr);
}

void add_aria_labelledby(UIElement *element, const char *id) {
    char attr[256];
    snprintf(attr, sizeof(attr), "aria-labelledby=\"%s\"", id);
    add_attribute(element, attr);
}

void add_aria_describedby(UIElement *element, const char *id) {
    char attr[256];
    snprintf(attr, sizeof(attr), "aria-describedby=\"%s\"", id);
    add_attribute(element, attr);
}

void add_aria_expanded(UIElement *element, int expanded) {
    char attr[64];
    snprintf(attr, sizeof(attr), "aria-expanded=\"%s\"", expanded ? "true" : "false");
    add_attribute(element, attr);
}

void add_aria_hidden(UIElement *element, int hidden) {
    char attr[64];
    snprintf(attr, sizeof(attr), "aria-hidden=\"%s\"", hidden ? "true" : "false");
    add_attribute(element, attr);
}

void add_aria_selected(UIElement *element, int selected) {
    char attr[64];
    snprintf(attr, sizeof(attr), "aria-selected=\"%s\"", selected ? "true" : "false");
    add_attribute(element, attr);
}

void add_aria_checked(UIElement *element, int checked) {
    char attr[64];
    snprintf(attr, sizeof(attr), "aria-checked=\"%s\"", checked ? "true" : "false");
    add_attribute(element, attr);
}

void add_aria_disabled(UIElement *element, int disabled) {
    char attr[64];
    snprintf(attr, sizeof(attr), "aria-disabled=\"%s\"", disabled ? "true" : "false");
    add_attribute(element, attr);
}

void add_aria_required(UIElement *element, int required) {
    char attr[64];
    snprintf(attr, sizeof(attr), "aria-required=\"%s\"", required ? "true" : "false");
    add_attribute(element, attr);
}

void add_aria_invalid(UIElement *element, int invalid) {
    char attr[64];
    snprintf(attr, sizeof(attr), "aria-invalid=\"%s\"", invalid ? "true" : "false");
    add_attribute(element, attr);
}

void add_aria_live(UIElement *element, const char *politeness) {
    char attr[128];
    snprintf(attr, sizeof(attr), "aria-live=\"%s\"", politeness);
    add_attribute(element, attr);
}

void add_aria_atomic(UIElement *element, int atomic) {
    char attr[64];
    snprintf(attr, sizeof(attr), "aria-atomic=\"%s\"", atomic ? "true" : "false");
    add_attribute(element, attr);
}

void add_aria_busy(UIElement *element, int busy) {
    char attr[64];
    snprintf(attr, sizeof(attr), "aria-busy=\"%s\"", busy ? "true" : "false");
    add_attribute(element, attr);
}

void add_aria_controls(UIElement *element, const char *id) {
    char attr[256];
    snprintf(attr, sizeof(attr), "aria-controls=\"%s\"", id);
    add_attribute(element, attr);
}

void add_aria_owns(UIElement *element, const char *id) {
    char attr[256];
    snprintf(attr, sizeof(attr), "aria-owns=\"%s\"", id);
    add_attribute(element, attr);
}

void add_aria_haspopup(UIElement *element, const char *popup_type) {
    char attr[128];
    snprintf(attr, sizeof(attr), "aria-haspopup=\"%s\"", popup_type);
    add_attribute(element, attr);
}

void add_aria_level(UIElement *element, int level) {
    char attr[64];
    snprintf(attr, sizeof(attr), "aria-level=\"%d\"", level);
    add_attribute(element, attr);
}

void add_aria_valuemin(UIElement *element, double min) {
    char attr[64];
    snprintf(attr, sizeof(attr), "aria-valuemin=\"%.2f\"", min);
    add_attribute(element, attr);
}

void add_aria_valuemax(UIElement *element, double max) {
    char attr[64];
    snprintf(attr, sizeof(attr), "aria-valuemax=\"%.2f\"", max);
    add_attribute(element, attr);
}

void add_aria_valuenow(UIElement *element, double now) {
    char attr[64];
    snprintf(attr, sizeof(attr), "aria-valuenow=\"%.2f\"", now);
    add_attribute(element, attr);
}

void add_aria_valuetext(UIElement *element, const char *text) {
    char attr[256];
    snprintf(attr, sizeof(attr), "aria-valuetext=\"%s\"", text);
    add_attribute(element, attr);
}

/* Accessible Button */
UIElement* create_accessible_button(const char *text, const char *aria_label) {
    UIElement *button = create_element("button");
    add_attribute(button, "type=\"button\"");
    add_aria_role(button, ARIA_BUTTON);
    
    if (aria_label) {
        add_aria_label(button, aria_label);
    }
    
    add_attribute(button, "tabIndex=\"0\"");
    add_attribute(button, "onKeyDown={(e) => { if (e.key === 'Enter' || e.key === ' ') { e.preventDefault(); e.target.click(); } }}");
    
    add_child(button, create_text_node(text));
    return button;
}

/* Accessible Link */
UIElement* create_accessible_link(const char *href, const char *text, const char *aria_label) {
    UIElement *link = create_element("a");
    
    char attr[512];
    snprintf(attr, sizeof(attr), "href=\"%s\"", href);
    add_attribute(link, attr);
    
    add_aria_role(link, ARIA_LINK);
    
    if (aria_label) {
        add_aria_label(link, aria_label);
    }
    
    add_attribute(link, "tabIndex=\"0\"");
    add_child(link, create_text_node(text));
    
    return link;
}

/* Accessible Modal Dialog */
UIElement* create_accessible_modal(const char *id, const char *title, const char *description_id) {
    UIElement *overlay = create_element("div");
    add_attribute(overlay, "className=\"modal-overlay\"");
    add_attribute(overlay, "style=\"position: fixed; top: 0; left: 0; right: 0; bottom: 0; background: rgba(0,0,0,0.5); display: flex; align-items: center; justify-content: center; z-index: 1000;\"");
    add_attribute(overlay, "onClick={(e) => { if (e.target === e.currentTarget) closeModal(); }}");
    
    UIElement *modal = create_element("div");
    
    char attr[256];
    snprintf(attr, sizeof(attr), "id=\"%s\"", id);
    add_attribute(modal, attr);
    
    add_aria_role(modal, ARIA_DIALOG);
    add_aria_label(modal, title);
    
    if (description_id) {
        add_aria_describedby(modal, description_id);
    }
    
    add_attribute(modal, "aria-modal=\"true\"");
    add_attribute(modal, "tabIndex=\"-1\"");
    add_attribute(modal, "style=\"background: white; padding: 24px; border-radius: 8px; max-width: 500px; width: 90%; box-shadow: 0 4px 20px rgba(0,0,0,0.15);\"");
    
    UIElement *title_elem = create_element("h2");
    snprintf(attr, sizeof(attr), "id=\"%s-title\"", id);
    add_attribute(title_elem, attr);
    add_attribute(title_elem, "style=\"margin: 0 0 16px 0; font-size: 24px;\"");
    add_child(title_elem, create_text_node(title));
    add_child(modal, title_elem);
    
    UIElement *close_btn = create_element("button");
    add_attribute(close_btn, "type=\"button\"");
    add_attribute(close_btn, "onClick={() => closeModal()}");
    add_aria_label(close_btn, "Close dialog");
    add_attribute(close_btn, "style=\"position: absolute; top: 16px; right: 16px; background: none; border: none; font-size: 24px; cursor: pointer; padding: 4px 8px;\"");
    add_child(close_btn, create_text_node("×"));
    add_child(modal, close_btn);
    
    add_child(overlay, modal);
    return overlay;
}

/* Accessible Tabs */
typedef struct {
    char *id;
    char *label;
    char *panel_id;
} TabItem;

UIElement* create_accessible_tabs(TabItem *tabs, int tab_count) {
    UIElement *container = create_element("div");
    add_attribute(container, "className=\"tabs-container\"");
    
    UIElement *tablist = create_element("div");
    add_aria_role(tablist, ARIA_TABLIST);
    add_attribute(tablist, "style=\"display: flex; border-bottom: 2px solid #ddd; margin-bottom: 16px;\"");
    
    for (int i = 0; i < tab_count; i++) {
        UIElement *tab = create_element("button");
        
        char attr[256];
        snprintf(attr, sizeof(attr), "id=\"%s\"", tabs[i].id);
        add_attribute(tab, attr);
        
        add_aria_role(tab, ARIA_TAB);
        add_aria_selected(tab, i == 0);
        add_aria_controls(tab, tabs[i].panel_id);
        
        add_attribute(tab, "tabIndex=\"0\"");
        snprintf(attr, sizeof(attr), "onClick={() => selectTab(%d)}", i);
        add_attribute(tab, attr);
        
        add_attribute(tab, "onKeyDown={(e) => { handleTabKeyDown(e, " + i + "); }}");
        add_attribute(tab, "style=\"padding: 12px 24px; background: none; border: none; border-bottom: 2px solid transparent; cursor: pointer; font-size: 14px;\"");
        
        add_child(tab, create_text_node(tabs[i].label));
        add_child(tablist, tab);
    }
    
    add_child(container, tablist);
    
    for (int i = 0; i < tab_count; i++) {
        UIElement *panel = create_element("div");
        
        char attr[256];
        snprintf(attr, sizeof(attr), "id=\"%s\"", tabs[i].panel_id);
        add_attribute(panel, attr);
        
        add_aria_role(panel, ARIA_TABPANEL);
        add_aria_labelledby(panel, tabs[i].id);
        
        add_attribute(panel, "tabIndex=\"0\"");
        
        if (i != 0) {
            add_aria_hidden(panel, 1);
            add_attribute(panel, "style=\"display: none;\"");
        }
        
        add_child(container, panel);
    }
    
    return container;
}

/* Accessible Accordion */
UIElement* create_accessible_accordion_item(const char *id, const char *title, int expanded) {
    UIElement *item = create_element("div");
    add_attribute(item, "className=\"accordion-item\"");
    add_attribute(item, "style=\"border: 1px solid #ddd; margin-bottom: 8px; border-radius: 4px;\"");
    
    UIElement *header = create_element("button");
    
    char attr[256];
    snprintf(attr, sizeof(attr), "id=\"%s-header\"", id);
    add_attribute(header, attr);
    
    add_attribute(header, "type=\"button\"");
    add_aria_expanded(header, expanded);
    add_aria_controls(header, id);
    
    snprintf(attr, sizeof(attr), "onClick={() => toggleAccordion('%s')}", id);
    add_attribute(header, attr);
    
    add_attribute(header, "style=\"width: 100%; padding: 16px; background: #f5f5f5; border: none; text-align: left; cursor: pointer; font-size: 16px; font-weight: 500;\"");
    
    add_child(header, create_text_node(title));
    add_child(item, header);
    
    UIElement *panel = create_element("div");
    snprintf(attr, sizeof(attr), "id=\"%s\"", id);
    add_attribute(panel, attr);
    
    add_aria_role(panel, ARIA_REGION);
    add_aria_labelledby(panel, attr);
    
    if (!expanded) {
        add_aria_hidden(panel, 1);
        add_attribute(panel, "style=\"display: none; padding: 16px;\"");
    } else {
        add_attribute(panel, "style=\"padding: 16px;\"");
    }
    
    add_child(item, panel);
    return item;
}

/* Skip Navigation Link */
UIElement* create_skip_link(const char *target_id, const char *text) {
    UIElement *link = create_element("a");
    
    char attr[256];
    snprintf(attr, sizeof(attr), "href=\"#%s\"", target_id);
    add_attribute(link, attr);
    
    add_attribute(link, "className=\"skip-link\"");
    add_attribute(link, "style=\"position: absolute; top: -40px; left: 0; background: #000; color: #fff; padding: 8px; z-index: 100; :focus { top: 0; }\"");
    
    add_child(link, create_text_node(text));
    return link;
}

/* Live Region for Announcements */
UIElement* create_live_region(const char *id, const char *politeness) {
    UIElement *region = create_element("div");
    
    char attr[256];
    snprintf(attr, sizeof(attr), "id=\"%s\"", id);
    add_attribute(region, attr);
    
    add_aria_role(region, ARIA_STATUS);
    add_aria_live(region, politeness);
    add_aria_atomic(region, 1);
    
    add_attribute(region, "style=\"position: absolute; left: -10000px; width: 1px; height: 1px; overflow: hidden;\"");
    
    return region;
}

/* Accessible Form Input */
UIElement* create_accessible_input(const char *id, const char *label, const char *type, int required) {
    UIElement *container = create_element("div");
    add_attribute(container, "className=\"form-field\"");
    add_attribute(container, "style=\"margin-bottom: 16px;\"");
    
    UIElement *label_elem = create_element("label");
    char attr[256];
    snprintf(attr, sizeof(attr), "htmlFor=\"%s\"", id);
    add_attribute(label_elem, attr);
    add_attribute(label_elem, "style=\"display: block; margin-bottom: 4px; font-weight: 500;\"");
    
    char label_text[256];
    snprintf(label_text, sizeof(label_text), "%s%s", label, required ? " *" : "");
    add_child(label_elem, create_text_node(label_text));
    add_child(container, label_elem);
    
    UIElement *input = create_element("input");
    snprintf(attr, sizeof(attr), "id=\"%s\"", id);
    add_attribute(input, attr);
    
    snprintf(attr, sizeof(attr), "type=\"%s\"", type);
    add_attribute(input, attr);
    
    snprintf(attr, sizeof(attr), "name=\"%s\"", id);
    add_attribute(input, attr);
    
    if (required) {
        add_attribute(input, "required");
        add_aria_required(input, 1);
    }
    
    add_attribute(input, "style=\"width: 100%; padding: 8px; border: 1px solid #ddd; border-radius: 4px;\"");
    
    UIElement *error = create_element("div");
    snprintf(attr, sizeof(attr), "id=\"%s-error\"", id);
    add_attribute(error, attr);
    add_aria_role(error, ARIA_ALERT);
    add_attribute(error, "style=\"color: #dc3545; font-size: 14px; margin-top: 4px; display: none;\"");
    add_child(container, error);
    
    add_aria_describedby(input, attr);
    add_child(container, input);
    add_child(container, error);
    
    return container;
}

/* Accessible Progress Bar */
UIElement* create_accessible_progress(const char *id, double value, double max, const char *label) {
    UIElement *container = create_element("div");
    add_attribute(container, "className=\"progress-container\"");
    
    if (label) {
        UIElement *label_elem = create_element("div");
        char attr[256];
        snprintf(attr, sizeof(attr), "id=\"%s-label\"", id);
        add_attribute(label_elem, attr);
        add_attribute(label_elem, "style=\"margin-bottom: 8px; font-weight: 500;\"");
        add_child(label_elem, create_text_node(label));
        add_child(container, label_elem);
    }
    
    UIElement *progress = create_element("div");
    
    char attr[256];
    snprintf(attr, sizeof(attr), "id=\"%s\"", id);
    add_attribute(progress, attr);
    
    add_aria_role(progress, ARIA_PROGRESSBAR);
    add_aria_valuemin(progress, 0);
    add_aria_valuemax(progress, max);
    add_aria_valuenow(progress, value);
    
    if (label) {
        snprintf(attr, sizeof(attr), "%s-label", id);
        add_aria_labelledby(progress, attr);
    }
    
    char value_text[64];
    snprintf(value_text, sizeof(value_text), "%.0f%% complete", (value / max) * 100);
    add_aria_valuetext(progress, value_text);
    
    add_attribute(progress, "style=\"width: 100%; height: 24px; background: #e0e0e0; border-radius: 4px; overflow: hidden;\"");
    
    UIElement *bar = create_element("div");
    char style[256];
    snprintf(style, sizeof(style), "height: 100%%; width: %.1f%%; background: #007bff; transition: width 0.3s ease;", (value / max) * 100);
    snprintf(attr, sizeof(attr), "style=\"%s\"", style);
    add_attribute(bar, attr);
    
    add_child(progress, bar);
    add_child(container, progress);
    
    return container;
}

/* Keyboard Navigation Helper */
char* generate_keyboard_navigation() {
    char *code = malloc(4096);
    strcpy(code,
        "const handleKeyboardNavigation = (e) => {\n"
        "  const focusableElements = document.querySelectorAll(\n"
        "    'a[href], button:not([disabled]), textarea:not([disabled]), ' +\n"
        "    'input:not([disabled]), select:not([disabled]), [tabindex]:not([tabindex=\"-1\"])'\n"
        "  );\n"
        "  const focusableArray = Array.from(focusableElements);\n"
        "  const currentIndex = focusableArray.indexOf(document.activeElement);\n"
        "  \n"
        "  if (e.key === 'Tab') {\n"
        "    if (e.shiftKey) {\n"
        "      if (currentIndex > 0) {\n"
        "        e.preventDefault();\n"
        "        focusableArray[currentIndex - 1].focus();\n"
        "      }\n"
        "    } else {\n"
        "      if (currentIndex < focusableArray.length - 1) {\n"
        "        e.preventDefault();\n"
        "        focusableArray[currentIndex + 1].focus();\n"
        "      }\n"
        "    }\n"
        "  }\n"
        "};\n"
        "\n"
        "document.addEventListener('keydown', handleKeyboardNavigation);\n");
    return code;
}

/* Screen Reader Only Text */
UIElement* create_sr_only_text(const char *text) {
    UIElement *span = create_element("span");
    add_attribute(span, "className=\"sr-only\"");
    add_attribute(span, "style=\"position: absolute; width: 1px; height: 1px; padding: 0; margin: -1px; overflow: hidden; clip: rect(0,0,0,0); white-space: nowrap; border: 0;\"");
    add_child(span, create_text_node(text));
    return span;
}

/* Focus Visible Styles */
char* generate_focus_visible_styles() {
    char *css = malloc(1024);
    strcpy(css,
        "*:focus-visible {\n"
        "  outline: 2px solid #007bff;\n"
        "  outline-offset: 2px;\n"
        "}\n"
        "\n"
        "button:focus-visible,\n"
        "a:focus-visible,\n"
        "input:focus-visible,\n"
        "select:focus-visible,\n"
        "textarea:focus-visible {\n"
        "  outline: 2px solid #007bff;\n"
        "  outline-offset: 2px;\n"
        "  box-shadow: 0 0 0 3px rgba(0, 123, 255, 0.25);\n"
        "}\n");
    return css;
}
