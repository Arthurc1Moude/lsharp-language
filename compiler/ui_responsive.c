/* Responsive Design System for L# Language
 * Handles breakpoints, media queries, adaptive layouts, device detection
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_renderer.h"

/* Breakpoint System */
typedef struct Breakpoint {
    char *name;
    int min_width;
    int max_width;
    struct Breakpoint *next;
} Breakpoint;

typedef struct {
    Breakpoint *breakpoints;
    int count;
} BreakpointSystem;

BreakpointSystem* create_breakpoint_system() {
    BreakpointSystem *bs = malloc(sizeof(BreakpointSystem));
    bs->breakpoints = NULL;
    bs->count = 0;
    
    // Add default breakpoints
    Breakpoint *xs = malloc(sizeof(Breakpoint));
    xs->name = strdup("xs");
    xs->min_width = 0;
    xs->max_width = 575;
    xs->next = NULL;
    
    Breakpoint *sm = malloc(sizeof(Breakpoint));
    sm->name = strdup("sm");
    sm->min_width = 576;
    sm->max_width = 767;
    sm->next = xs;
    
    Breakpoint *md = malloc(sizeof(Breakpoint));
    md->name = strdup("md");
    md->min_width = 768;
    md->max_width = 991;
    md->next = sm;
    
    Breakpoint *lg = malloc(sizeof(Breakpoint));
    lg->name = strdup("lg");
    lg->min_width = 992;
    lg->max_width = 1199;
    lg->next = md;
    
    Breakpoint *xl = malloc(sizeof(Breakpoint));
    xl->name = strdup("xl");
    xl->min_width = 1200;
    xl->max_width = 1399;
    xl->next = lg;
    
    Breakpoint *xxl = malloc(sizeof(Breakpoint));
    xxl->name = strdup("xxl");
    xxl->min_width = 1400;
    xxl->max_width = 9999;
    xxl->next = xl;
    
    bs->breakpoints = xxl;
    bs->count = 6;
    
    return bs;
}

void add_breakpoint(BreakpointSystem *bs, const char *name, int min_width, int max_width) {
    Breakpoint *bp = malloc(sizeof(Breakpoint));
    bp->name = strdup(name);
    bp->min_width = min_width;
    bp->max_width = max_width;
    bp->next = bs->breakpoints;
    bs->breakpoints = bp;
    bs->count++;
}

char* generate_media_query(const char *breakpoint, const char *styles) {
    char *query = malloc(2048);
    
    if (strcmp(breakpoint, "xs") == 0) {
        snprintf(query, 2048, "@media (max-width: 575px) { %s }", styles);
    } else if (strcmp(breakpoint, "sm") == 0) {
        snprintf(query, 2048, "@media (min-width: 576px) and (max-width: 767px) { %s }", styles);
    } else if (strcmp(breakpoint, "md") == 0) {
        snprintf(query, 2048, "@media (min-width: 768px) and (max-width: 991px) { %s }", styles);
    } else if (strcmp(breakpoint, "lg") == 0) {
        snprintf(query, 2048, "@media (min-width: 992px) and (max-width: 1199px) { %s }", styles);
    } else if (strcmp(breakpoint, "xl") == 0) {
        snprintf(query, 2048, "@media (min-width: 1200px) and (max-width: 1399px) { %s }", styles);
    } else if (strcmp(breakpoint, "xxl") == 0) {
        snprintf(query, 2048, "@media (min-width: 1400px) { %s }", styles);
    } else {
        snprintf(query, 2048, "%s", styles);
    }
    
    return query;
}

/* Responsive Container */
UIElement* create_responsive_container(const char *id) {
    UIElement *container = create_element("div");
    add_attribute(container, "className=\"container\"");
    
    char style[1024];
    snprintf(style, sizeof(style),
        "width: 100%%; "
        "padding-right: 15px; "
        "padding-left: 15px; "
        "margin-right: auto; "
        "margin-left: auto; "
        "@media (min-width: 576px) { max-width: 540px; } "
        "@media (min-width: 768px) { max-width: 720px; } "
        "@media (min-width: 992px) { max-width: 960px; } "
        "@media (min-width: 1200px) { max-width: 1140px; } "
        "@media (min-width: 1400px) { max-width: 1320px; }");
    
    char attr[2048];
    snprintf(attr, sizeof(attr), "style=\"%s\"", style);
    add_attribute(container, attr);
    
    return container;
}

/* Responsive Grid */
typedef struct GridColumn {
    int xs;
    int sm;
    int md;
    int lg;
    int xl;
    int xxl;
    char *content;
    struct GridColumn *next;
} GridColumn;

typedef struct {
    GridColumn *columns;
    int column_count;
    int gutter;
} ResponsiveGrid;

ResponsiveGrid* create_responsive_grid(int gutter) {
    ResponsiveGrid *grid = malloc(sizeof(ResponsiveGrid));
    grid->columns = NULL;
    grid->column_count = 0;
    grid->gutter = gutter;
    return grid;
}

void add_grid_column(ResponsiveGrid *grid, int xs, int sm, int md, int lg, int xl, int xxl) {
    GridColumn *col = malloc(sizeof(GridColumn));
    col->xs = xs;
    col->sm = sm;
    col->md = md;
    col->lg = lg;
    col->xl = xl;
    col->xxl = xxl;
    col->content = NULL;
    col->next = grid->columns;
    grid->columns = col;
    grid->column_count++;
}

UIElement* render_responsive_grid(ResponsiveGrid *grid) {
    UIElement *row = create_element("div");
    add_attribute(row, "className=\"row\"");
    
    char row_style[512];
    snprintf(row_style, sizeof(row_style),
        "display: flex; flex-wrap: wrap; margin-left: -%dpx; margin-right: -%dpx;",
        grid->gutter / 2, grid->gutter / 2);
    
    char attr[1024];
    snprintf(attr, sizeof(attr), "style=\"%s\"", row_style);
    add_attribute(row, attr);
    
    GridColumn *current = grid->columns;
    while (current) {
        UIElement *col = create_element("div");
        
        char col_style[2048];
        snprintf(col_style, sizeof(col_style),
            "flex: 0 0 auto; "
            "padding-left: %dpx; padding-right: %dpx; "
            "width: %.2f%%; "
            "@media (min-width: 576px) { width: %.2f%%; } "
            "@media (min-width: 768px) { width: %.2f%%; } "
            "@media (min-width: 992px) { width: %.2f%%; } "
            "@media (min-width: 1200px) { width: %.2f%%; } "
            "@media (min-width: 1400px) { width: %.2f%%; }",
            grid->gutter / 2, grid->gutter / 2,
            (current->xs / 12.0) * 100,
            (current->sm / 12.0) * 100,
            (current->md / 12.0) * 100,
            (current->lg / 12.0) * 100,
            (current->xl / 12.0) * 100,
            (current->xxl / 12.0) * 100);
        
        snprintf(attr, sizeof(attr), "style=\"%s\"", col_style);
        add_attribute(col, attr);
        
        add_child(row, col);
        current = current->next;
    }
    
    return row;
}

/* Responsive Typography */
typedef struct {
    float base_size;
    float scale_ratio;
    int min_width;
    int max_width;
} FluidTypography;

FluidTypography* create_fluid_typography(float base_size, float scale_ratio) {
    FluidTypography *ft = malloc(sizeof(FluidTypography));
    ft->base_size = base_size;
    ft->scale_ratio = scale_ratio;
    ft->min_width = 320;
    ft->max_width = 1200;
    return ft;
}

char* generate_fluid_font_size(FluidTypography *ft, int level) {
    char *css = malloc(512);
    
    float min_size = ft->base_size * pow(ft->scale_ratio, level);
    float max_size = min_size * 1.5;
    
    snprintf(css, 512,
        "font-size: clamp(%.2frem, %.2fvw + %.2frem, %.2frem);",
        min_size, (max_size - min_size) / (ft->max_width - ft->min_width) * 100,
        min_size, max_size);
    
    return css;
}

/* Responsive Images */
UIElement* create_responsive_image(const char *src, const char *alt) {
    UIElement *img = create_element("img");
    
    char attr[512];
    snprintf(attr, sizeof(attr), "src=\"%s\"", src);
    add_attribute(img, attr);
    
    snprintf(attr, sizeof(attr), "alt=\"%s\"", alt);
    add_attribute(img, attr);
    
    add_attribute(img, "style=\"max-width: 100%; height: auto; display: block;\"");
    
    return img;
}

UIElement* create_picture_element(const char *src_mobile, const char *src_tablet, const char *src_desktop, const char *alt) {
    UIElement *picture = create_element("picture");
    
    UIElement *source_desktop = create_element("source");
    char attr[512];
    snprintf(attr, sizeof(attr), "media=\"(min-width: 1024px)\"");
    add_attribute(source_desktop, attr);
    snprintf(attr, sizeof(attr), "srcSet=\"%s\"", src_desktop);
    add_attribute(source_desktop, attr);
    add_child(picture, source_desktop);
    
    UIElement *source_tablet = create_element("source");
    snprintf(attr, sizeof(attr), "media=\"(min-width: 768px)\"");
    add_attribute(source_tablet, attr);
    snprintf(attr, sizeof(attr), "srcSet=\"%s\"", src_tablet);
    add_attribute(source_tablet, attr);
    add_child(picture, source_tablet);
    
    UIElement *img = create_element("img");
    snprintf(attr, sizeof(attr), "src=\"%s\"", src_mobile);
    add_attribute(img, attr);
    snprintf(attr, sizeof(attr), "alt=\"%s\"", alt);
    add_attribute(img, attr);
    add_attribute(img, "style=\"max-width: 100%; height: auto;\"");
    add_child(picture, img);
    
    return picture;
}

/* Device Detection */
typedef enum {
    DEVICE_MOBILE,
    DEVICE_TABLET,
    DEVICE_DESKTOP,
    DEVICE_TV
} DeviceType;

typedef struct {
    DeviceType type;
    int width;
    int height;
    float pixel_ratio;
    int is_touch;
    int is_portrait;
} DeviceInfo;

char* generate_device_detection() {
    char *code = malloc(2048);
    strcpy(code,
        "const detectDevice = () => {\n"
        "  const width = window.innerWidth;\n"
        "  const height = window.innerHeight;\n"
        "  const pixelRatio = window.devicePixelRatio || 1;\n"
        "  const isTouch = 'ontouchstart' in window || navigator.maxTouchPoints > 0;\n"
        "  const isPortrait = height > width;\n"
        "  \n"
        "  let deviceType = 'desktop';\n"
        "  if (width < 768) deviceType = 'mobile';\n"
        "  else if (width < 1024) deviceType = 'tablet';\n"
        "  \n"
        "  return { deviceType, width, height, pixelRatio, isTouch, isPortrait };\n"
        "};\n");
    return code;
}

/* Responsive Navigation */
UIElement* create_responsive_nav(const char **items, int item_count) {
    UIElement *nav = create_element("nav");
    add_attribute(nav, "className=\"responsive-nav\"");
    
    char style[1024];
    snprintf(style, sizeof(style),
        "display: flex; "
        "flex-direction: column; "
        "@media (min-width: 768px) { flex-direction: row; justify-content: space-between; }");
    
    char attr[2048];
    snprintf(attr, sizeof(attr), "style=\"%s\"", style);
    add_attribute(nav, attr);
    
    UIElement *menu_toggle = create_element("button");
    add_attribute(menu_toggle, "className=\"menu-toggle\"");
    add_attribute(menu_toggle, "style=\"display: block; @media (min-width: 768px) { display: none; }\"");
    add_child(menu_toggle, create_text_node("☰"));
    add_child(nav, menu_toggle);
    
    UIElement *menu = create_element("ul");
    add_attribute(menu, "className=\"nav-menu\"");
    snprintf(style, sizeof(style),
        "display: none; "
        "flex-direction: column; "
        "@media (min-width: 768px) { display: flex; flex-direction: row; }");
    snprintf(attr, sizeof(attr), "style=\"%s\"", style);
    add_attribute(menu, attr);
    
    for (int i = 0; i < item_count; i++) {
        UIElement *item = create_element("li");
        add_child(item, create_text_node(items[i]));
        add_child(menu, item);
    }
    
    add_child(nav, menu);
    return nav;
}

/* Responsive Spacing */
char* generate_responsive_spacing(const char *property, int xs, int sm, int md, int lg, int xl) {
    char *css = malloc(1024);
    snprintf(css, 1024,
        "%s: %dpx; "
        "@media (min-width: 576px) { %s: %dpx; } "
        "@media (min-width: 768px) { %s: %dpx; } "
        "@media (min-width: 992px) { %s: %dpx; } "
        "@media (min-width: 1200px) { %s: %dpx; }",
        property, xs,
        property, sm,
        property, md,
        property, lg,
        property, xl);
    return css;
}

/* Aspect Ratio Container */
UIElement* create_aspect_ratio_container(float ratio, const char *content_type) {
    UIElement *container = create_element("div");
    
    char style[512];
    snprintf(style, sizeof(style),
        "position: relative; width: 100%%; padding-bottom: %.2f%%;",
        (1.0 / ratio) * 100);
    
    char attr[1024];
    snprintf(attr, sizeof(attr), "style=\"%s\"", style);
    add_attribute(container, attr);
    
    UIElement *content = create_element("div");
    add_attribute(content, "style=\"position: absolute; top: 0; left: 0; width: 100%; height: 100%;\"");
    add_child(container, content);
    
    return container;
}

/* Responsive Table */
UIElement* create_responsive_table(char **headers, int header_count, char ***data, int row_count) {
    UIElement *wrapper = create_element("div");
    add_attribute(wrapper, "className=\"table-responsive\"");
    add_attribute(wrapper, "style=\"overflow-x: auto; -webkit-overflow-scrolling: touch;\"");
    
    UIElement *table = create_element("table");
    add_attribute(table, "style=\"width: 100%; min-width: 600px; border-collapse: collapse;\"");
    
    UIElement *thead = create_element("thead");
    UIElement *header_row = create_element("tr");
    
    for (int i = 0; i < header_count; i++) {
        UIElement *th = create_element("th");
        add_attribute(th, "style=\"padding: 12px; text-align: left; border-bottom: 2px solid #ddd;\"");
        add_child(th, create_text_node(headers[i]));
        add_child(header_row, th);
    }
    
    add_child(thead, header_row);
    add_child(table, thead);
    
    UIElement *tbody = create_element("tbody");
    for (int i = 0; i < row_count; i++) {
        UIElement *row = create_element("tr");
        for (int j = 0; j < header_count; j++) {
            UIElement *td = create_element("td");
            add_attribute(td, "style=\"padding: 12px; border-bottom: 1px solid #ddd;\"");
            add_child(td, create_text_node(data[i][j]));
            add_child(row, td);
        }
        add_child(tbody, row);
    }
    
    add_child(table, tbody);
    add_child(wrapper, table);
    
    return wrapper;
}

/* Mobile-First Utilities */
char* generate_mobile_first_css() {
    char *css = malloc(4096);
    strcpy(css,
        "/* Mobile First Base Styles */\n"
        ".container { width: 100%; padding: 0 15px; }\n"
        ".row { display: flex; flex-wrap: wrap; margin: 0 -15px; }\n"
        ".col { flex: 1; padding: 0 15px; }\n"
        "\n"
        "/* Hide/Show Utilities */\n"
        ".hide-mobile { display: none; }\n"
        ".show-mobile { display: block; }\n"
        "\n"
        "@media (min-width: 768px) {\n"
        "  .hide-mobile { display: block; }\n"
        "  .show-mobile { display: none; }\n"
        "  .hide-tablet { display: none; }\n"
        "  .show-tablet { display: block; }\n"
        "}\n"
        "\n"
        "@media (min-width: 1024px) {\n"
        "  .hide-desktop { display: none; }\n"
        "  .show-desktop { display: block; }\n"
        "}\n");
    return css;
}

/* Orientation Detection */
char* generate_orientation_styles(const char *portrait_styles, const char *landscape_styles) {
    char *css = malloc(2048);
    snprintf(css, 2048,
        "@media (orientation: portrait) { %s }\n"
        "@media (orientation: landscape) { %s }\n",
        portrait_styles, landscape_styles);
    return css;
}

/* Print Styles */
char* generate_print_styles() {
    char *css = malloc(2048);
    strcpy(css,
        "@media print {\n"
        "  * { background: transparent !important; color: black !important; }\n"
        "  .no-print { display: none !important; }\n"
        "  a[href]:after { content: ' (' attr(href) ')'; }\n"
        "  img { max-width: 100% !important; page-break-inside: avoid; }\n"
        "  h1, h2, h3 { page-break-after: avoid; }\n"
        "}\n");
    return css;
}

/* Dark Mode Support */
char* generate_dark_mode_styles(const char *light_styles, const char *dark_styles) {
    char *css = malloc(2048);
    snprintf(css, 2048,
        "@media (prefers-color-scheme: light) { %s }\n"
        "@media (prefers-color-scheme: dark) { %s }\n",
        light_styles, dark_styles);
    return css;
}

/* Reduced Motion Support */
char* generate_reduced_motion_styles() {
    char *css = malloc(1024);
    strcpy(css,
        "@media (prefers-reduced-motion: reduce) {\n"
        "  *, *::before, *::after {\n"
        "    animation-duration: 0.01ms !important;\n"
        "    animation-iteration-count: 1 !important;\n"
        "    transition-duration: 0.01ms !important;\n"
        "  }\n"
        "}\n");
    return css;
}

/* High Contrast Mode */
char* generate_high_contrast_styles() {
    char *css = malloc(1024);
    strcpy(css,
        "@media (prefers-contrast: high) {\n"
        "  * {\n"
        "    border-color: currentColor !important;\n"
        "    outline-color: currentColor !important;\n"
        "  }\n"
        "  button, a {\n"
        "    text-decoration: underline;\n"
        "  }\n"
        "}\n");
    return css;
}

/* Container Queries */
char* generate_container_query(const char *container_name, int min_width, const char *styles) {
    char *css = malloc(1024);
    snprintf(css, 1024,
        "@container %s (min-width: %dpx) { %s }",
        container_name, min_width, styles);
    return css;
}
