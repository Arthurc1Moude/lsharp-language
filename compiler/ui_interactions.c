/* UI Interactions System for L# Language
 * Handles user interactions, events, gestures, drag-and-drop, touch, keyboard
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ui_renderer.h"

/* Event System */
typedef enum {
    EVENT_CLICK,
    EVENT_DOUBLE_CLICK,
    EVENT_RIGHT_CLICK,
    EVENT_MOUSE_DOWN,
    EVENT_MOUSE_UP,
    EVENT_MOUSE_MOVE,
    EVENT_MOUSE_ENTER,
    EVENT_MOUSE_LEAVE,
    EVENT_MOUSE_OVER,
    EVENT_MOUSE_OUT,
    EVENT_KEY_DOWN,
    EVENT_KEY_UP,
    EVENT_KEY_PRESS,
    EVENT_FOCUS,
    EVENT_BLUR,
    EVENT_CHANGE,
    EVENT_INPUT,
    EVENT_SUBMIT,
    EVENT_SCROLL,
    EVENT_RESIZE,
    EVENT_DRAG_START,
    EVENT_DRAG,
    EVENT_DRAG_END,
    EVENT_DROP,
    EVENT_TOUCH_START,
    EVENT_TOUCH_MOVE,
    EVENT_TOUCH_END,
    EVENT_SWIPE_LEFT,
    EVENT_SWIPE_RIGHT,
    EVENT_SWIPE_UP,
    EVENT_SWIPE_DOWN,
    EVENT_PINCH,
    EVENT_ROTATE,
    EVENT_LONG_PRESS
} EventType;

typedef struct EventHandler {
    EventType type;
    char *callback;
    int prevent_default;
    int stop_propagation;
    struct EventHandler *next;
} EventHandler;

typedef struct {
    char *element_id;
    EventHandler *handlers;
    int capture_phase;
} EventBinding;

EventHandler* create_event_handler(EventType type, const char *callback) {
    EventHandler *handler = malloc(sizeof(EventHandler));
    handler->type = type;
    handler->callback = strdup(callback);
    handler->prevent_default = 0;
    handler->stop_propagation = 0;
    handler->next = NULL;
    return handler;
}

void add_event_listener(UIElement *element, EventType type, const char *callback) {
    EventHandler *handler = create_event_handler(type, callback);
    
    char event_attr[256];
    switch(type) {
        case EVENT_CLICK:
            snprintf(event_attr, sizeof(event_attr), "onClick={%s}", callback);
            break;
        case EVENT_DOUBLE_CLICK:
            snprintf(event_attr, sizeof(event_attr), "onDoubleClick={%s}", callback);
            break;
        case EVENT_MOUSE_ENTER:
            snprintf(event_attr, sizeof(event_attr), "onMouseEnter={%s}", callback);
            break;
        case EVENT_MOUSE_LEAVE:
            snprintf(event_attr, sizeof(event_attr), "onMouseLeave={%s}", callback);
            break;
        case EVENT_KEY_DOWN:
            snprintf(event_attr, sizeof(event_attr), "onKeyDown={%s}", callback);
            break;
        case EVENT_CHANGE:
            snprintf(event_attr, sizeof(event_attr), "onChange={%s}", callback);
            break;
        case EVENT_SUBMIT:
            snprintf(event_attr, sizeof(event_attr), "onSubmit={%s}", callback);
            break;
        case EVENT_SCROLL:
            snprintf(event_attr, sizeof(event_attr), "onScroll={%s}", callback);
            break;
        case EVENT_DRAG_START:
            snprintf(event_attr, sizeof(event_attr), "onDragStart={%s}", callback);
            break;
        case EVENT_DROP:
            snprintf(event_attr, sizeof(event_attr), "onDrop={%s}", callback);
            break;
        case EVENT_TOUCH_START:
            snprintf(event_attr, sizeof(event_attr), "onTouchStart={%s}", callback);
            break;
        default:
            snprintf(event_attr, sizeof(event_attr), "onClick={%s}", callback);
    }
    
    add_attribute(element, event_attr);
}

/* Gesture Recognition */
typedef struct {
    float start_x;
    float start_y;
    float current_x;
    float current_y;
    long start_time;
    long current_time;
    int touch_count;
} GestureState;

typedef struct {
    float velocity_x;
    float velocity_y;
    float distance;
    float angle;
    long duration;
} GestureMetrics;

GestureMetrics* calculate_swipe_metrics(GestureState *state) {
    GestureMetrics *metrics = malloc(sizeof(GestureMetrics));
    
    float dx = state->current_x - state->start_x;
    float dy = state->current_y - state->start_y;
    long dt = state->current_time - state->start_time;
    
    metrics->distance = sqrt(dx * dx + dy * dy);
    metrics->angle = atan2(dy, dx) * 180.0 / M_PI;
    metrics->duration = dt;
    metrics->velocity_x = (dt > 0) ? (dx / dt * 1000.0) : 0;
    metrics->velocity_y = (dt > 0) ? (dy / dt * 1000.0) : 0;
    
    return metrics;
}

int detect_swipe_direction(GestureMetrics *metrics) {
    if (metrics->distance < 50) return -1;
    
    float angle = metrics->angle;
    if (angle >= -45 && angle < 45) return EVENT_SWIPE_RIGHT;
    if (angle >= 45 && angle < 135) return EVENT_SWIPE_DOWN;
    if (angle >= 135 || angle < -135) return EVENT_SWIPE_LEFT;
    return EVENT_SWIPE_UP;
}

/* Drag and Drop System */
typedef struct {
    char *drag_id;
    char *drop_zone_id;
    void *drag_data;
    int is_dragging;
    float offset_x;
    float offset_y;
} DragDropState;

UIElement* create_draggable(const char *id, const char *content) {
    UIElement *elem = create_element("div");
    add_attribute(elem, "draggable=\"true\"");
    
    char attr[512];
    snprintf(attr, sizeof(attr), 
        "onDragStart={(e) => { e.dataTransfer.setData('text/plain', '%s'); }}",
        id);
    add_attribute(elem, attr);
    
    snprintf(attr, sizeof(attr),
        "onDrag={(e) => { console.log('Dragging %s'); }}",
        id);
    add_attribute(elem, attr);
    
    add_child(elem, create_text_node(content));
    return elem;
}

UIElement* create_drop_zone(const char *id, const char *on_drop_callback) {
    UIElement *elem = create_element("div");
    
    char attr[512];
    snprintf(attr, sizeof(attr),
        "onDragOver={(e) => { e.preventDefault(); e.currentTarget.style.background = '#e0e0e0'; }}");
    add_attribute(elem, attr);
    
    snprintf(attr, sizeof(attr),
        "onDragLeave={(e) => { e.currentTarget.style.background = 'transparent'; }}");
    add_attribute(elem, attr);
    
    snprintf(attr, sizeof(attr),
        "onDrop={(e) => { e.preventDefault(); const data = e.dataTransfer.getData('text/plain'); %s(data); }}",
        on_drop_callback);
    add_attribute(elem, attr);
    
    return elem;
}

/* Keyboard Shortcuts */
typedef struct KeyBinding {
    char *key;
    int ctrl;
    int shift;
    int alt;
    char *callback;
    struct KeyBinding *next;
} KeyBinding;

typedef struct {
    KeyBinding *bindings;
    int num_bindings;
} KeyboardManager;

KeyboardManager* create_keyboard_manager() {
    KeyboardManager *km = malloc(sizeof(KeyboardManager));
    km->bindings = NULL;
    km->num_bindings = 0;
    return km;
}

void add_keyboard_shortcut(KeyboardManager *km, const char *key, int ctrl, int shift, int alt, const char *callback) {
    KeyBinding *binding = malloc(sizeof(KeyBinding));
    binding->key = strdup(key);
    binding->ctrl = ctrl;
    binding->shift = shift;
    binding->alt = alt;
    binding->callback = strdup(callback);
    binding->next = km->bindings;
    km->bindings = binding;
    km->num_bindings++;
}

char* generate_keyboard_handler(KeyboardManager *km) {
    char *handler = malloc(8192);
    strcpy(handler, "const handleKeyDown = (e) => {\n");
    
    KeyBinding *current = km->bindings;
    while (current) {
        char condition[512];
        snprintf(condition, sizeof(condition),
            "  if (e.key === '%s'%s%s%s) {\n    e.preventDefault();\n    %s();\n  }\n",
            current->key,
            current->ctrl ? " && e.ctrlKey" : "",
            current->shift ? " && e.shiftKey" : "",
            current->alt ? " && e.altKey" : "",
            current->callback);
        strcat(handler, condition);
        current = current->next;
    }
    
    strcat(handler, "};\n");
    return handler;
}

/* Focus Management */
typedef struct FocusNode {
    char *element_id;
    int tab_index;
    struct FocusNode *next;
    struct FocusNode *prev;
} FocusNode;

typedef struct {
    FocusNode *head;
    FocusNode *current;
    int trap_focus;
} FocusManager;

FocusManager* create_focus_manager() {
    FocusManager *fm = malloc(sizeof(FocusManager));
    fm->head = NULL;
    fm->current = NULL;
    fm->trap_focus = 0;
    return fm;
}

void add_focusable_element(FocusManager *fm, const char *element_id, int tab_index) {
    FocusNode *node = malloc(sizeof(FocusNode));
    node->element_id = strdup(element_id);
    node->tab_index = tab_index;
    node->next = NULL;
    node->prev = NULL;
    
    if (!fm->head) {
        fm->head = node;
        fm->current = node;
    } else {
        FocusNode *last = fm->head;
        while (last->next) last = last->next;
        last->next = node;
        node->prev = last;
    }
}

char* generate_focus_trap(FocusManager *fm) {
    char *trap = malloc(4096);
    strcpy(trap, "const focusTrap = (e) => {\n");
    strcat(trap, "  if (e.key === 'Tab') {\n");
    strcat(trap, "    const focusableElements = document.querySelectorAll('[tabindex]:not([tabindex=\"-1\"])');\n");
    strcat(trap, "    const first = focusableElements[0];\n");
    strcat(trap, "    const last = focusableElements[focusableElements.length - 1];\n");
    strcat(trap, "    if (e.shiftKey && document.activeElement === first) {\n");
    strcat(trap, "      e.preventDefault();\n");
    strcat(trap, "      last.focus();\n");
    strcat(trap, "    } else if (!e.shiftKey && document.activeElement === last) {\n");
    strcat(trap, "      e.preventDefault();\n");
    strcat(trap, "      first.focus();\n");
    strcat(trap, "    }\n");
    strcat(trap, "  }\n");
    strcat(trap, "};\n");
    return trap;
}

/* Touch Gestures */
typedef struct {
    float scale;
    float rotation;
    float center_x;
    float center_y;
} MultiTouchState;

MultiTouchState* calculate_pinch_zoom(float x1, float y1, float x2, float y2, float prev_x1, float prev_y1, float prev_x2, float prev_y2) {
    MultiTouchState *state = malloc(sizeof(MultiTouchState));
    
    float current_distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    float prev_distance = sqrt(pow(prev_x2 - prev_x1, 2) + pow(prev_y2 - prev_y1, 2));
    
    state->scale = (prev_distance > 0) ? (current_distance / prev_distance) : 1.0;
    state->center_x = (x1 + x2) / 2.0;
    state->center_y = (y1 + y2) / 2.0;
    
    float current_angle = atan2(y2 - y1, x2 - x1);
    float prev_angle = atan2(prev_y2 - prev_y1, prev_x2 - prev_x1);
    state->rotation = (current_angle - prev_angle) * 180.0 / M_PI;
    
    return state;
}

/* Scroll Handling */
typedef struct {
    float scroll_top;
    float scroll_left;
    float scroll_height;
    float client_height;
    int is_at_top;
    int is_at_bottom;
    int is_scrolling_up;
    int is_scrolling_down;
} ScrollState;

ScrollState* create_scroll_state() {
    ScrollState *state = malloc(sizeof(ScrollState));
    state->scroll_top = 0;
    state->scroll_left = 0;
    state->scroll_height = 0;
    state->client_height = 0;
    state->is_at_top = 1;
    state->is_at_bottom = 0;
    state->is_scrolling_up = 0;
    state->is_scrolling_down = 0;
    return state;
}

char* generate_scroll_handler(const char *on_scroll_callback) {
    char *handler = malloc(2048);
    snprintf(handler, 2048,
        "const handleScroll = (e) => {\n"
        "  const scrollTop = e.target.scrollTop;\n"
        "  const scrollHeight = e.target.scrollHeight;\n"
        "  const clientHeight = e.target.clientHeight;\n"
        "  const isAtTop = scrollTop === 0;\n"
        "  const isAtBottom = scrollTop + clientHeight >= scrollHeight - 1;\n"
        "  %s({ scrollTop, scrollHeight, clientHeight, isAtTop, isAtBottom });\n"
        "};\n",
        on_scroll_callback);
    return handler;
}

/* Infinite Scroll */
UIElement* create_infinite_scroll_container(const char *load_more_callback) {
    UIElement *container = create_element("div");
    
    char attr[1024];
    snprintf(attr, sizeof(attr),
        "onScroll={(e) => {"
        "  const bottom = e.target.scrollHeight - e.target.scrollTop === e.target.clientHeight;"
        "  if (bottom) { %s(); }"
        "}}",
        load_more_callback);
    add_attribute(container, attr);
    
    add_attribute(container, "style=\"overflow-y: auto; height: 100%;\"");
    return container;
}

/* Virtual Scrolling */
typedef struct {
    int total_items;
    int visible_items;
    int item_height;
    int scroll_top;
    int start_index;
    int end_index;
} VirtualScrollState;

VirtualScrollState* calculate_virtual_scroll(int total_items, int item_height, int container_height, int scroll_top) {
    VirtualScrollState *state = malloc(sizeof(VirtualScrollState));
    state->total_items = total_items;
    state->item_height = item_height;
    state->scroll_top = scroll_top;
    state->visible_items = (container_height / item_height) + 2;
    state->start_index = scroll_top / item_height;
    state->end_index = state->start_index + state->visible_items;
    
    if (state->end_index > total_items) {
        state->end_index = total_items;
    }
    
    return state;
}

/* Hover Effects */
UIElement* create_hover_element(const char *tag, const char *normal_style, const char *hover_style) {
    UIElement *elem = create_element(tag);
    
    char attr[2048];
    snprintf(attr, sizeof(attr), "style=\"%s\"", normal_style);
    add_attribute(elem, attr);
    
    snprintf(attr, sizeof(attr),
        "onMouseEnter={(e) => { e.target.style.cssText = '%s'; }}",
        hover_style);
    add_attribute(elem, attr);
    
    snprintf(attr, sizeof(attr),
        "onMouseLeave={(e) => { e.target.style.cssText = '%s'; }}",
        normal_style);
    add_attribute(elem, attr);
    
    return elem;
}

/* Click Outside Detection */
char* generate_click_outside_handler(const char *element_ref, const char *callback) {
    char *handler = malloc(1024);
    snprintf(handler, 1024,
        "useEffect(() => {\n"
        "  const handleClickOutside = (e) => {\n"
        "    if (%s.current && !%s.current.contains(e.target)) {\n"
        "      %s();\n"
        "    }\n"
        "  };\n"
        "  document.addEventListener('mousedown', handleClickOutside);\n"
        "  return () => document.removeEventListener('mousedown', handleClickOutside);\n"
        "}, []);\n",
        element_ref, element_ref, callback);
    return handler;
}

/* Debounce and Throttle */
char* generate_debounce_function(const char *func_name, int delay_ms) {
    char *code = malloc(1024);
    snprintf(code, 1024,
        "const %s = debounce((value) => {\n"
        "  // Debounced function logic\n"
        "  console.log('Debounced:', value);\n"
        "}, %d);\n",
        func_name, delay_ms);
    return code;
}

char* generate_throttle_function(const char *func_name, int delay_ms) {
    char *code = malloc(1024);
    snprintf(code, 1024,
        "const %s = throttle((value) => {\n"
        "  // Throttled function logic\n"
        "  console.log('Throttled:', value);\n"
        "}, %d);\n",
        func_name, delay_ms);
    return code;
}

/* Context Menu */
UIElement* create_context_menu(const char *id, const char **items, int num_items) {
    UIElement *menu = create_element("div");
    add_attribute(menu, "className=\"context-menu\"");
    add_attribute(menu, "style=\"position: absolute; display: none; background: white; border: 1px solid #ccc; box-shadow: 0 2px 10px rgba(0,0,0,0.1); z-index: 1000;\"");
    
    for (int i = 0; i < num_items; i++) {
        UIElement *item = create_element("div");
        add_attribute(item, "className=\"context-menu-item\"");
        add_attribute(item, "style=\"padding: 8px 16px; cursor: pointer; hover:background-color: #f0f0f0;\"");
        add_child(item, create_text_node(items[i]));
        add_child(menu, item);
    }
    
    return menu;
}

/* Tooltip System */
UIElement* create_tooltip(const char *content, const char *position) {
    UIElement *tooltip = create_element("div");
    add_attribute(tooltip, "className=\"tooltip\"");
    
    char style[512];
    snprintf(style, sizeof(style),
        "position: absolute; background: #333; color: white; padding: 8px 12px; "
        "border-radius: 4px; font-size: 14px; white-space: nowrap; z-index: 9999; "
        "pointer-events: none; opacity: 0; transition: opacity 0.2s;");
    
    char attr[1024];
    snprintf(attr, sizeof(attr), "style=\"%s\"", style);
    add_attribute(tooltip, attr);
    
    add_child(tooltip, create_text_node(content));
    return tooltip;
}

UIElement* create_element_with_tooltip(const char *tag, const char *content, const char *tooltip_text) {
    UIElement *wrapper = create_element("div");
    add_attribute(wrapper, "style=\"position: relative; display: inline-block;\"");
    
    UIElement *elem = create_element(tag);
    add_child(elem, create_text_node(content));
    
    UIElement *tooltip = create_tooltip(tooltip_text, "top");
    
    add_attribute(wrapper,
        "onMouseEnter={(e) => { e.currentTarget.querySelector('.tooltip').style.opacity = '1'; }}");
    add_attribute(wrapper,
        "onMouseLeave={(e) => { e.currentTarget.querySelector('.tooltip').style.opacity = '0'; }}");
    
    add_child(wrapper, elem);
    add_child(wrapper, tooltip);
    
    return wrapper;
}

/* Resize Observer */
char* generate_resize_observer(const char *element_ref, const char *callback) {
    char *code = malloc(1024);
    snprintf(code, 1024,
        "useEffect(() => {\n"
        "  const resizeObserver = new ResizeObserver((entries) => {\n"
        "    for (let entry of entries) {\n"
        "      const { width, height } = entry.contentRect;\n"
        "      %s({ width, height });\n"
        "    }\n"
        "  });\n"
        "  if (%s.current) resizeObserver.observe(%s.current);\n"
        "  return () => resizeObserver.disconnect();\n"
        "}, []);\n",
        callback, element_ref, element_ref);
    return code;
}

/* Intersection Observer for Lazy Loading */
char* generate_intersection_observer(const char *element_ref, const char *callback) {
    char *code = malloc(1024);
    snprintf(code, 1024,
        "useEffect(() => {\n"
        "  const observer = new IntersectionObserver((entries) => {\n"
        "    entries.forEach(entry => {\n"
        "      if (entry.isIntersecting) {\n"
        "        %s(entry);\n"
        "      }\n"
        "    });\n"
        "  }, { threshold: 0.1 });\n"
        "  if (%s.current) observer.observe(%s.current);\n"
        "  return () => observer.disconnect();\n"
        "}, []);\n",
        callback, element_ref, element_ref);
    return code;
}

/* Long Press Detection */
char* generate_long_press_handler(const char *callback, int duration_ms) {
    char *handler = malloc(2048);
    snprintf(handler, 2048,
        "const [pressTimer, setPressTimer] = useState(null);\n"
        "const handleMouseDown = () => {\n"
        "  const timer = setTimeout(() => {\n"
        "    %s();\n"
        "  }, %d);\n"
        "  setPressTimer(timer);\n"
        "};\n"
        "const handleMouseUp = () => {\n"
        "  if (pressTimer) clearTimeout(pressTimer);\n"
        "};\n",
        callback, duration_ms);
    return handler;
}
