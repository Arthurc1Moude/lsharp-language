/* State Management System for L# Language
 * Handles component state, global state, reducers, context, and reactive updates
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_renderer.h"

/* State Types */
typedef enum {
    STATE_STRING,
    STATE_NUMBER,
    STATE_BOOLEAN,
    STATE_OBJECT,
    STATE_ARRAY,
    STATE_FUNCTION
} StateType;

typedef struct StateValue {
    StateType type;
    union {
        char *string_val;
        double number_val;
        int boolean_val;
        void *object_val;
        void *array_val;
    } data;
} StateValue;

/* Component State */
typedef struct StateEntry {
    char *key;
    StateValue *value;
    char *setter_name;
    struct StateEntry *next;
} StateEntry;

typedef struct {
    StateEntry *entries;
    int count;
} ComponentState;

ComponentState* create_component_state() {
    ComponentState *state = malloc(sizeof(ComponentState));
    state->entries = NULL;
    state->count = 0;
    return state;
}

void add_state_variable(ComponentState *state, const char *key, StateType type, const char *initial_value) {
    StateEntry *entry = malloc(sizeof(StateEntry));
    entry->key = strdup(key);
    entry->value = malloc(sizeof(StateValue));
    entry->value->type = type;
    
    switch(type) {
        case STATE_STRING:
            entry->value->data.string_val = strdup(initial_value);
            break;
        case STATE_NUMBER:
            entry->value->data.number_val = atof(initial_value);
            break;
        case STATE_BOOLEAN:
            entry->value->data.boolean_val = (strcmp(initial_value, "true") == 0);
            break;
        default:
            entry->value->data.string_val = strdup(initial_value);
    }
    
    char setter[256];
    snprintf(setter, sizeof(setter), "set%c%s", 
             toupper(key[0]), key + 1);
    entry->setter_name = strdup(setter);
    
    entry->next = state->entries;
    state->entries = entry;
    state->count++;
}

char* generate_use_state(const char *var_name, const char *initial_value) {
    char *code = malloc(512);
    snprintf(code, 512,
        "const [%s, set%c%s] = useState(%s);\n",
        var_name,
        toupper(var_name[0]),
        var_name + 1,
        initial_value);
    return code;
}

/* Effect Hook */
typedef struct Effect {
    char *callback;
    char **dependencies;
    int dep_count;
    int cleanup;
    struct Effect *next;
} Effect;

typedef struct {
    Effect *effects;
    int count;
} EffectManager;

EffectManager* create_effect_manager() {
    EffectManager *em = malloc(sizeof(EffectManager));
    em->effects = NULL;
    em->count = 0;
    return em;
}

void add_effect(EffectManager *em, const char *callback, char **dependencies, int dep_count) {
    Effect *effect = malloc(sizeof(Effect));
    effect->callback = strdup(callback);
    effect->dependencies = malloc(dep_count * sizeof(char*));
    for (int i = 0; i < dep_count; i++) {
        effect->dependencies[i] = strdup(dependencies[i]);
    }
    effect->dep_count = dep_count;
    effect->cleanup = 0;
    effect->next = em->effects;
    em->effects = effect;
    em->count++;
}

char* generate_use_effect(const char *callback, char **dependencies, int dep_count) {
    char *code = malloc(2048);
    strcpy(code, "useEffect(() => {\n");
    strcat(code, "  ");
    strcat(code, callback);
    strcat(code, "\n");
    
    if (dep_count > 0) {
        strcat(code, "}, [");
        for (int i = 0; i < dep_count; i++) {
            strcat(code, dependencies[i]);
            if (i < dep_count - 1) strcat(code, ", ");
        }
        strcat(code, "]);\n");
    } else {
        strcat(code, "}, []);\n");
    }
    
    return code;
}

/* Reducer Pattern */
typedef struct Action {
    char *type;
    void *payload;
} Action;

typedef struct ReducerCase {
    char *action_type;
    char *handler;
    struct ReducerCase *next;
} ReducerCase;

typedef struct {
    char *name;
    char *initial_state;
    ReducerCase *cases;
    int case_count;
} Reducer;

Reducer* create_reducer(const char *name, const char *initial_state) {
    Reducer *reducer = malloc(sizeof(Reducer));
    reducer->name = strdup(name);
    reducer->initial_state = strdup(initial_state);
    reducer->cases = NULL;
    reducer->case_count = 0;
    return reducer;
}

void add_reducer_case(Reducer *reducer, const char *action_type, const char *handler) {
    ReducerCase *rc = malloc(sizeof(ReducerCase));
    rc->action_type = strdup(action_type);
    rc->handler = strdup(handler);
    rc->next = reducer->cases;
    reducer->cases = rc;
    reducer->case_count++;
}

char* generate_reducer(Reducer *reducer) {
    char *code = malloc(8192);
    snprintf(code, 512, "const %sReducer = (state, action) => {\n  switch(action.type) {\n", reducer->name);
    
    ReducerCase *current = reducer->cases;
    while (current) {
        char case_code[1024];
        snprintf(case_code, sizeof(case_code),
            "    case '%s':\n      return %s;\n",
            current->action_type,
            current->handler);
        strcat(code, case_code);
        current = current->next;
    }
    
    strcat(code, "    default:\n      return state;\n  }\n};\n");
    return code;
}

char* generate_use_reducer(const char *reducer_name, const char *initial_state) {
    char *code = malloc(512);
    snprintf(code, 512,
        "const [state, dispatch] = useReducer(%sReducer, %s);\n",
        reducer_name, initial_state);
    return code;
}

/* Context API */
typedef struct ContextProvider {
    char *name;
    char *value;
    struct ContextProvider *parent;
} ContextProvider;

ContextProvider* create_context(const char *name, const char *default_value) {
    ContextProvider *ctx = malloc(sizeof(ContextProvider));
    ctx->name = strdup(name);
    ctx->value = strdup(default_value);
    ctx->parent = NULL;
    return ctx;
}

char* generate_context_creation(const char *context_name) {
    char *code = malloc(512);
    snprintf(code, 512,
        "const %sContext = createContext();\n"
        "export const use%s = () => useContext(%sContext);\n",
        context_name, context_name, context_name);
    return code;
}

UIElement* create_context_provider(const char *context_name, const char *value) {
    UIElement *provider = create_element("ContextProvider");
    
    char attr[512];
    snprintf(attr, sizeof(attr), "context={%sContext}", context_name);
    add_attribute(provider, attr);
    
    snprintf(attr, sizeof(attr), "value={%s}", value);
    add_attribute(provider, attr);
    
    return provider;
}

/* Memo Hook */
char* generate_use_memo(const char *computation, char **dependencies, int dep_count) {
    char *code = malloc(1024);
    strcpy(code, "const memoizedValue = useMemo(() => {\n  return ");
    strcat(code, computation);
    strcat(code, ";\n}, [");
    
    for (int i = 0; i < dep_count; i++) {
        strcat(code, dependencies[i]);
        if (i < dep_count - 1) strcat(code, ", ");
    }
    strcat(code, "]);\n");
    
    return code;
}

/* Callback Hook */
char* generate_use_callback(const char *callback, char **dependencies, int dep_count) {
    char *code = malloc(1024);
    strcpy(code, "const memoizedCallback = useCallback(");
    strcat(code, callback);
    strcat(code, ", [");
    
    for (int i = 0; i < dep_count; i++) {
        strcat(code, dependencies[i]);
        if (i < dep_count - 1) strcat(code, ", ");
    }
    strcat(code, "]);\n");
    
    return code;
}

/* Ref Hook */
char* generate_use_ref(const char *ref_name, const char *initial_value) {
    char *code = malloc(256);
    snprintf(code, 256, "const %s = useRef(%s);\n", ref_name, initial_value);
    return code;
}

/* Custom Hooks */
typedef struct CustomHook {
    char *name;
    char **parameters;
    int param_count;
    char *body;
    char **return_values;
    int return_count;
} CustomHook;

CustomHook* create_custom_hook(const char *name) {
    CustomHook *hook = malloc(sizeof(CustomHook));
    hook->name = strdup(name);
    hook->parameters = NULL;
    hook->param_count = 0;
    hook->body = NULL;
    hook->return_values = NULL;
    hook->return_count = 0;
    return hook;
}

void add_hook_parameter(CustomHook *hook, const char *param) {
    hook->param_count++;
    hook->parameters = realloc(hook->parameters, hook->param_count * sizeof(char*));
    hook->parameters[hook->param_count - 1] = strdup(param);
}

void add_hook_return_value(CustomHook *hook, const char *value) {
    hook->return_count++;
    hook->return_values = realloc(hook->return_values, hook->return_count * sizeof(char*));
    hook->return_values[hook->return_count - 1] = strdup(value);
}

char* generate_custom_hook(CustomHook *hook) {
    char *code = malloc(4096);
    snprintf(code, 256, "const %s = (", hook->name);
    
    for (int i = 0; i < hook->param_count; i++) {
        strcat(code, hook->parameters[i]);
        if (i < hook->param_count - 1) strcat(code, ", ");
    }
    strcat(code, ") => {\n");
    
    if (hook->body) {
        strcat(code, "  ");
        strcat(code, hook->body);
        strcat(code, "\n");
    }
    
    strcat(code, "  return ");
    if (hook->return_count > 1) {
        strcat(code, "[");
        for (int i = 0; i < hook->return_count; i++) {
            strcat(code, hook->return_values[i]);
            if (i < hook->return_count - 1) strcat(code, ", ");
        }
        strcat(code, "]");
    } else if (hook->return_count == 1) {
        strcat(code, hook->return_values[0]);
    }
    strcat(code, ";\n};\n");
    
    return code;
}

/* Global State Store */
typedef struct StoreSlice {
    char *name;
    char *initial_state;
    char *reducers;
    char *actions;
    struct StoreSlice *next;
} StoreSlice;

typedef struct {
    StoreSlice *slices;
    int slice_count;
    char *middleware;
} GlobalStore;

GlobalStore* create_global_store() {
    GlobalStore *store = malloc(sizeof(GlobalStore));
    store->slices = NULL;
    store->slice_count = 0;
    store->middleware = NULL;
    return store;
}

void add_store_slice(GlobalStore *store, const char *name, const char *initial_state) {
    StoreSlice *slice = malloc(sizeof(StoreSlice));
    slice->name = strdup(name);
    slice->initial_state = strdup(initial_state);
    slice->reducers = NULL;
    slice->actions = NULL;
    slice->next = store->slices;
    store->slices = slice;
    store->slice_count++;
}

char* generate_redux_store(GlobalStore *store) {
    char *code = malloc(8192);
    strcpy(code, "import { configureStore } from '@reduxjs/toolkit';\n\n");
    
    StoreSlice *current = store->slices;
    while (current) {
        char slice_code[1024];
        snprintf(slice_code, sizeof(slice_code),
            "const %sSlice = createSlice({\n"
            "  name: '%s',\n"
            "  initialState: %s,\n"
            "  reducers: {}\n"
            "});\n\n",
            current->name, current->name, current->initial_state);
        strcat(code, slice_code);
        current = current->next;
    }
    
    strcat(code, "export const store = configureStore({\n  reducer: {\n");
    current = store->slices;
    while (current) {
        char reducer_line[256];
        snprintf(reducer_line, sizeof(reducer_line),
            "    %s: %sSlice.reducer,\n",
            current->name, current->name);
        strcat(code, reducer_line);
        current = current->next;
    }
    strcat(code, "  }\n});\n");
    
    return code;
}

/* Reactive State */
typedef struct Observer {
    char *callback;
    struct Observer *next;
} Observer;

typedef struct ReactiveState {
    char *key;
    StateValue *value;
    Observer *observers;
    int observer_count;
    struct ReactiveState *next;
} ReactiveState;

typedef struct {
    ReactiveState *states;
    int count;
} ReactiveStore;

ReactiveStore* create_reactive_store() {
    ReactiveStore *store = malloc(sizeof(ReactiveStore));
    store->states = NULL;
    store->count = 0;
    return store;
}

void add_reactive_state(ReactiveStore *store, const char *key, StateValue *initial_value) {
    ReactiveState *state = malloc(sizeof(ReactiveState));
    state->key = strdup(key);
    state->value = initial_value;
    state->observers = NULL;
    state->observer_count = 0;
    state->next = store->states;
    store->states = state;
    store->count++;
}

void subscribe_to_state(ReactiveState *state, const char *callback) {
    Observer *observer = malloc(sizeof(Observer));
    observer->callback = strdup(callback);
    observer->next = state->observers;
    state->observers = observer;
    state->observer_count++;
}

char* generate_reactive_state(const char *var_name, const char *initial_value) {
    char *code = malloc(1024);
    snprintf(code, 1024,
        "const %s = reactive(%s);\n"
        "watch(() => %s, (newVal, oldVal) => {\n"
        "  console.log('%s changed from', oldVal, 'to', newVal);\n"
        "});\n",
        var_name, initial_value, var_name, var_name);
    return code;
}

/* Computed Properties */
typedef struct ComputedProperty {
    char *name;
    char *computation;
    char **dependencies;
    int dep_count;
    struct ComputedProperty *next;
} ComputedProperty;

ComputedProperty* create_computed_property(const char *name, const char *computation) {
    ComputedProperty *prop = malloc(sizeof(ComputedProperty));
    prop->name = strdup(name);
    prop->computation = strdup(computation);
    prop->dependencies = NULL;
    prop->dep_count = 0;
    prop->next = NULL;
    return prop;
}

char* generate_computed(ComputedProperty *prop) {
    char *code = malloc(1024);
    snprintf(code, 1024,
        "const %s = computed(() => %s);\n",
        prop->name, prop->computation);
    return code;
}

/* State Persistence */
char* generate_local_storage_state(const char *key, const char *initial_value) {
    char *code = malloc(1024);
    snprintf(code, 1024,
        "const [%s, set%c%s] = useState(() => {\n"
        "  const saved = localStorage.getItem('%s');\n"
        "  return saved ? JSON.parse(saved) : %s;\n"
        "});\n"
        "useEffect(() => {\n"
        "  localStorage.setItem('%s', JSON.stringify(%s));\n"
        "}, [%s]);\n",
        key, toupper(key[0]), key + 1, key, initial_value,
        key, key, key);
    return code;
}

/* State Synchronization */
char* generate_sync_state(const char *local_state, const char *remote_endpoint) {
    char *code = malloc(2048);
    snprintf(code, 2048,
        "useEffect(() => {\n"
        "  const syncState = async () => {\n"
        "    try {\n"
        "      const response = await fetch('%s', {\n"
        "        method: 'POST',\n"
        "        headers: { 'Content-Type': 'application/json' },\n"
        "        body: JSON.stringify(%s)\n"
        "      });\n"
        "      const data = await response.json();\n"
        "      console.log('State synced:', data);\n"
        "    } catch (error) {\n"
        "      console.error('Sync failed:', error);\n"
        "    }\n"
        "  };\n"
        "  syncState();\n"
        "}, [%s]);\n",
        remote_endpoint, local_state, local_state);
    return code;
}

/* Form State Management */
typedef struct FormField {
    char *name;
    char *type;
    char *validation;
    char *error_message;
    struct FormField *next;
} FormField;

typedef struct {
    FormField *fields;
    int field_count;
    char *on_submit;
} FormState;

FormState* create_form_state() {
    FormState *form = malloc(sizeof(FormState));
    form->fields = NULL;
    form->field_count = 0;
    form->on_submit = NULL;
    return form;
}

void add_form_field(FormState *form, const char *name, const char *type, const char *validation) {
    FormField *field = malloc(sizeof(FormField));
    field->name = strdup(name);
    field->type = strdup(type);
    field->validation = validation ? strdup(validation) : NULL;
    field->error_message = NULL;
    field->next = form->fields;
    form->fields = field;
    form->field_count++;
}

char* generate_form_state(FormState *form) {
    char *code = malloc(4096);
    strcpy(code, "const [formData, setFormData] = useState({\n");
    
    FormField *current = form->fields;
    while (current) {
        char field_line[256];
        snprintf(field_line, sizeof(field_line), "  %s: '',\n", current->name);
        strcat(code, field_line);
        current = current->next;
    }
    strcat(code, "});\n\n");
    
    strcat(code, "const [errors, setErrors] = useState({});\n\n");
    
    strcat(code,
        "const handleChange = (e) => {\n"
        "  const { name, value } = e.target;\n"
        "  setFormData(prev => ({ ...prev, [name]: value }));\n"
        "};\n\n");
    
    strcat(code,
        "const handleSubmit = (e) => {\n"
        "  e.preventDefault();\n"
        "  // Validation logic\n"
        "  console.log('Form submitted:', formData);\n"
        "};\n");
    
    return code;
}
