/* Generated C code from L# source
 * L# Compiler v1.0 - Pure Native Code Generation
 * No HTML, JavaScript, React, or web dependencies
 */

#include "lsharp-native-window.c"

/* Application State */
typedef struct {
    char input[256];
    char loading[256];
    char selectedModel[256];
    char showModelMenu[256];
    int loading;
    int show_menu;
} AppState;

static AppState app_state = {
    .loading = 0,
    .show_menu = 0
};

/* Event Handlers */
void handle_send_message() {
    printf("📤 Message sent\n");
}

void handle_button_click() {
    printf("🖱️ Button clicked\n");
}

/* UI Builder - Generated from L# Components */
void build_ui(LSharpWindow *win, LSharpUI *ui) {
    const float w = win->width;
    const float h = win->height;

    // Main container
    lsharp_ui_add_widget(ui, 0, 0, w, h, 0.4f, 0.5f, 0.9f, 1.0f, NULL);

    // Chat window
    const float chat_w = 800;
    const float chat_h = 600;
    const float chat_x = (w - chat_w) / 2;
    const float chat_y = (h - chat_h) / 2;

    lsharp_ui_add_widget(ui, chat_x, chat_y, chat_w, chat_h, 1.0f, 1.0f, 1.0f, 1.0f, NULL);
    lsharp_ui_add_widget(ui, chat_x, chat_y, chat_w, 60, 0.4f, 0.5f, 0.9f, 1.0f, "L# Native App");

    // Input area
    const float input_y = chat_y + chat_h - 100;
    lsharp_ui_add_widget(ui, chat_x + 20, input_y, chat_w - 80, 40, 0.97f, 0.98f, 0.99f, 1.0f, "Type here...");
    lsharp_ui_add_button(ui, chat_x + chat_w - 80, input_y + 4, 32, 32, "↑", handle_send_message);
}

/* Main Entry Point */
int main(int argc, char *argv[]) {
    printf("🚀 L# Native Application Starting...\n");
    printf("⚡ Pure C • No HTML/JS/React • Independent\n\n");

    LSharpWindow *window = lsharp_window_create("L# Native App", 1000, 700);
    if (!window) {
        printf("❌ Failed to create window\n");
        return -1;
    }

    LSharpUI *ui = lsharp_ui_create();
    build_ui(window, ui);

    printf("✅ UI Created - Running application...\n");
    lsharp_app_run(window, ui);

    lsharp_ui_destroy(ui);
    lsharp_window_destroy(window);

    printf("👋 Application closed\n");
    return 0;
}
