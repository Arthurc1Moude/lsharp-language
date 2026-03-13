/* Form and Validation System for L# Language
 * Comprehensive form handling, validation rules, error messages, field types
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include "ui_renderer.h"

/* Validation Rule Types */
typedef enum {
    RULE_REQUIRED,
    RULE_MIN_LENGTH,
    RULE_MAX_LENGTH,
    RULE_MIN_VALUE,
    RULE_MAX_VALUE,
    RULE_PATTERN,
    RULE_EMAIL,
    RULE_URL,
    RULE_PHONE,
    RULE_ZIP_CODE,
    RULE_CREDIT_CARD,
    RULE_DATE,
    RULE_TIME,
    RULE_DATETIME,
    RULE_ALPHA,
    RULE_ALPHANUMERIC,
    RULE_NUMERIC,
    RULE_INTEGER,
    RULE_DECIMAL,
    RULE_MATCH_FIELD,
    RULE_CUSTOM
} ValidationRuleType;

typedef struct ValidationRule {
    ValidationRuleType type;
    union {
        int int_value;
        double double_value;
        char *string_value;
        regex_t *regex_value;
    } param;
    char *error_message;
    struct ValidationRule *next;
} ValidationRule;

/* Field Types */
typedef enum {
    FIELD_TEXT,
    FIELD_EMAIL,
    FIELD_PASSWORD,
    FIELD_NUMBER,
    FIELD_TEL,
    FIELD_URL,
    FIELD_SEARCH,
    FIELD_DATE,
    FIELD_TIME,
    FIELD_DATETIME,
    FIELD_MONTH,
    FIELD_WEEK,
    FIELD_COLOR,
    FIELD_RANGE,
    FIELD_FILE,
    FIELD_TEXTAREA,
    FIELD_SELECT,
    FIELD_CHECKBOX,
    FIELD_RADIO,
    FIELD_SWITCH
} FieldType;

typedef struct FormField {
    char *name;
    char *label;
    FieldType type;
    char *placeholder;
    char *default_value;
    int required;
    int disabled;
    int readonly;
    ValidationRule *rules;
    char **options;
    int option_count;
    char *help_text;
    struct FormField *next;
} FormField;

/* Form Builder */
typedef struct {
    char *form_id;
    char *action;
    char *method;
    FormField *fields;
    int field_count;
    char *submit_label;
    char *cancel_label;
    int inline;
    int show_labels;
} FormBuilder;

FormBuilder* create_form_builder(const char *form_id) {
    FormBuilder *fb = malloc(sizeof(FormBuilder));
    fb->form_id = strdup(form_id);
    fb->action = NULL;
    fb->method = strdup("POST");
    fb->fields = NULL;
    fb->field_count = 0;
    fb->submit_label = strdup("Submit");
    fb->cancel_label = strdup("Cancel");
    fb->inline = 0;
    fb->show_labels = 1;
    return fb;
}

FormField* create_form_field(const char *name, const char *label, FieldType type) {
    FormField *field = malloc(sizeof(FormField));
    field->name = strdup(name);
    field->label = strdup(label);
    field->type = type;
    field->placeholder = NULL;
    field->default_value = NULL;
    field->required = 0;
    field->disabled = 0;
    field->readonly = 0;
    field->rules = NULL;
    field->options = NULL;
    field->option_count = 0;
    field->help_text = NULL;
    field->next = NULL;
    return field;
}

void add_field_to_form(FormBuilder *fb, FormField *field) {
    field->next = fb->fields;
    fb->fields = field;
    fb->field_count++;
}

/* Validation Rules */
ValidationRule* create_validation_rule(ValidationRuleType type, const char *error_message) {
    ValidationRule *rule = malloc(sizeof(ValidationRule));
    rule->type = type;
    rule->error_message = strdup(error_message);
    rule->next = NULL;
    return rule;
}

void add_validation_rule(FormField *field, ValidationRule *rule) {
    rule->next = field->rules;
    field->rules = rule;
}

ValidationRule* create_required_rule() {
    return create_validation_rule(RULE_REQUIRED, "This field is required");
}

ValidationRule* create_min_length_rule(int min_length) {
    ValidationRule *rule = create_validation_rule(RULE_MIN_LENGTH, "Value is too short");
    rule->param.int_value = min_length;
    return rule;
}

ValidationRule* create_max_length_rule(int max_length) {
    ValidationRule *rule = create_validation_rule(RULE_MAX_LENGTH, "Value is too long");
    rule->param.int_value = max_length;
    return rule;
}

ValidationRule* create_min_value_rule(double min_value) {
    ValidationRule *rule = create_validation_rule(RULE_MIN_VALUE, "Value is too small");
    rule->param.double_value = min_value;
    return rule;
}

ValidationRule* create_max_value_rule(double max_value) {
    ValidationRule *rule = create_validation_rule(RULE_MAX_VALUE, "Value is too large");
    rule->param.double_value = max_value;
    return rule;
}

ValidationRule* create_email_rule() {
    return create_validation_rule(RULE_EMAIL, "Invalid email address");
}

ValidationRule* create_url_rule() {
    return create_validation_rule(RULE_URL, "Invalid URL");
}

ValidationRule* create_phone_rule() {
    return create_validation_rule(RULE_PHONE, "Invalid phone number");
}

ValidationRule* create_pattern_rule(const char *pattern) {
    ValidationRule *rule = create_validation_rule(RULE_PATTERN, "Invalid format");
    rule->param.string_value = strdup(pattern);
    return rule;
}

/* Validation Functions */
int validate_required(const char *value) {
    return value != NULL && strlen(value) > 0;
}

int validate_min_length(const char *value, int min_length) {
    return value != NULL && strlen(value) >= min_length;
}

int validate_max_length(const char *value, int max_length) {
    return value != NULL && strlen(value) <= max_length;
}

int validate_email(const char *email) {
    if (!email) return 0;
    
    const char *pattern = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$";
    regex_t regex;
    int result;
    
    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) return 0;
    result = regexec(&regex, email, 0, NULL, 0);
    regfree(&regex);
    
    return result == 0;
}

int validate_url(const char *url) {
    if (!url) return 0;
    
    const char *pattern = "^(https?://)?([a-zA-Z0-9-]+\\.)+[a-zA-Z]{2,}(/.*)?$";
    regex_t regex;
    int result;
    
    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) return 0;
    result = regexec(&regex, url, 0, NULL, 0);
    regfree(&regex);
    
    return result == 0;
}

int validate_phone(const char *phone) {
    if (!phone) return 0;
    
    const char *pattern = "^[+]?[(]?[0-9]{1,4}[)]?[-\\s\\.]?[(]?[0-9]{1,4}[)]?[-\\s\\.]?[0-9]{1,9}$";
    regex_t regex;
    int result;
    
    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) return 0;
    result = regexec(&regex, phone, 0, NULL, 0);
    regfree(&regex);
    
    return result == 0;
}

int validate_numeric(const char *value) {
    if (!value || strlen(value) == 0) return 0;
    
    for (int i = 0; value[i]; i++) {
        if (!isdigit(value[i]) && value[i] != '.' && value[i] != '-') {
            return 0;
        }
    }
    return 1;
}

int validate_alpha(const char *value) {
    if (!value || strlen(value) == 0) return 0;
    
    for (int i = 0; value[i]; i++) {
        if (!isalpha(value[i]) && value[i] != ' ') {
            return 0;
        }
    }
    return 1;
}

int validate_alphanumeric(const char *value) {
    if (!value || strlen(value) == 0) return 0;
    
    for (int i = 0; value[i]; i++) {
        if (!isalnum(value[i]) && value[i] != ' ') {
            return 0;
        }
    }
    return 1;
}

/* Generate Validation JavaScript */
char* generate_field_validation_js(FormField *field) {
    char *js = malloc(8192);
    snprintf(js, 256, "const validate%s = (value) => {\n  const errors = [];\n", field->name);
    
    ValidationRule *rule = field->rules;
    while (rule) {
        char rule_code[1024];
        
        switch(rule->type) {
            case RULE_REQUIRED:
                snprintf(rule_code, sizeof(rule_code),
                    "  if (!value || value.trim() === '') {\n"
                    "    errors.push('%s');\n"
                    "  }\n",
                    rule->error_message);
                break;
                
            case RULE_MIN_LENGTH:
                snprintf(rule_code, sizeof(rule_code),
                    "  if (value && value.length < %d) {\n"
                    "    errors.push('%s (minimum %d characters)');\n"
                    "  }\n",
                    rule->param.int_value,
                    rule->error_message,
                    rule->param.int_value);
                break;
                
            case RULE_MAX_LENGTH:
                snprintf(rule_code, sizeof(rule_code),
                    "  if (value && value.length > %d) {\n"
                    "    errors.push('%s (maximum %d characters)');\n"
                    "  }\n",
                    rule->param.int_value,
                    rule->error_message,
                    rule->param.int_value);
                break;
                
            case RULE_EMAIL:
                snprintf(rule_code, sizeof(rule_code),
                    "  const emailRegex = /^[^\\s@]+@[^\\s@]+\\.[^\\s@]+$/;\n"
                    "  if (value && !emailRegex.test(value)) {\n"
                    "    errors.push('%s');\n"
                    "  }\n",
                    rule->error_message);
                break;
                
            case RULE_URL:
                snprintf(rule_code, sizeof(rule_code),
                    "  const urlRegex = /^(https?:\\/\\/)?(www\\.)?[-a-zA-Z0-9@:%._\\+~#=]{1,256}\\.[a-zA-Z0-9()]{1,6}\\b([-a-zA-Z0-9()@:%_\\+.~#?&//=]*)$/;\n"
                    "  if (value && !urlRegex.test(value)) {\n"
                    "    errors.push('%s');\n"
                    "  }\n",
                    rule->error_message);
                break;
                
            case RULE_PHONE:
                snprintf(rule_code, sizeof(rule_code),
                    "  const phoneRegex = /^[+]?[(]?[0-9]{1,4}[)]?[-\\s\\.]?[(]?[0-9]{1,4}[)]?[-\\s\\.]?[0-9]{1,9}$/;\n"
                    "  if (value && !phoneRegex.test(value)) {\n"
                    "    errors.push('%s');\n"
                    "  }\n",
                    rule->error_message);
                break;
                
            case RULE_NUMERIC:
                snprintf(rule_code, sizeof(rule_code),
                    "  if (value && isNaN(value)) {\n"
                    "    errors.push('%s');\n"
                    "  }\n",
                    rule->error_message);
                break;
                
            case RULE_MIN_VALUE:
                snprintf(rule_code, sizeof(rule_code),
                    "  if (value && parseFloat(value) < %.2f) {\n"
                    "    errors.push('%s (minimum %.2f)');\n"
                    "  }\n",
                    rule->param.double_value,
                    rule->error_message,
                    rule->param.double_value);
                break;
                
            case RULE_MAX_VALUE:
                snprintf(rule_code, sizeof(rule_code),
                    "  if (value && parseFloat(value) > %.2f) {\n"
                    "    errors.push('%s (maximum %.2f)');\n"
                    "  }\n",
                    rule->param.double_value,
                    rule->error_message,
                    rule->param.double_value);
                break;
                
            default:
                rule_code[0] = '\0';
        }
        
        strcat(js, rule_code);
        rule = rule->next;
    }
    
    strcat(js, "  return errors;\n};\n");
    return js;
}

/* Render Form Field */
const char* get_field_type_string(FieldType type) {
    switch(type) {
        case FIELD_TEXT: return "text";
        case FIELD_EMAIL: return "email";
        case FIELD_PASSWORD: return "password";
        case FIELD_NUMBER: return "number";
        case FIELD_TEL: return "tel";
        case FIELD_URL: return "url";
        case FIELD_SEARCH: return "search";
        case FIELD_DATE: return "date";
        case FIELD_TIME: return "time";
        case FIELD_DATETIME: return "datetime-local";
        case FIELD_MONTH: return "month";
        case FIELD_WEEK: return "week";
        case FIELD_COLOR: return "color";
        case FIELD_RANGE: return "range";
        case FIELD_FILE: return "file";
        default: return "text";
    }
}

UIElement* render_form_field(FormField *field) {
    UIElement *field_group = create_element("div");
    add_attribute(field_group, "className=\"form-group\"");
    add_attribute(field_group, "style=\"margin-bottom: 20px;\"");
    
    if (field->label) {
        UIElement *label = create_element("label");
        char attr[512];
        snprintf(attr, sizeof(attr), "htmlFor=\"%s\"", field->name);
        add_attribute(label, attr);
        add_attribute(label, "style=\"display: block; margin-bottom: 8px; font-weight: 500;\"");
        
        char label_text[256];
        snprintf(label_text, sizeof(label_text), "%s%s", 
                 field->label, 
                 field->required ? " *" : "");
        add_child(label, create_text_node(label_text));
        add_child(field_group, label);
    }
    
    UIElement *input = NULL;
    
    if (field->type == FIELD_TEXTAREA) {
        input = create_element("textarea");
    } else if (field->type == FIELD_SELECT) {
        input = create_element("select");
        for (int i = 0; i < field->option_count; i++) {
            UIElement *option = create_element("option");
            char attr[256];
            snprintf(attr, sizeof(attr), "value=\"%s\"", field->options[i]);
            add_attribute(option, attr);
            add_child(option, create_text_node(field->options[i]));
            add_child(input, option);
        }
    } else if (field->type == FIELD_CHECKBOX || field->type == FIELD_RADIO) {
        input = create_element("input");
        char attr[256];
        snprintf(attr, sizeof(attr), "type=\"%s\"", 
                 field->type == FIELD_CHECKBOX ? "checkbox" : "radio");
        add_attribute(input, attr);
    } else {
        input = create_element("input");
        char attr[256];
        snprintf(attr, sizeof(attr), "type=\"%s\"", get_field_type_string(field->type));
        add_attribute(input, attr);
    }
    
    char attr[512];
    snprintf(attr, sizeof(attr), "name=\"%s\"", field->name);
    add_attribute(input, attr);
    
    snprintf(attr, sizeof(attr), "id=\"%s\"", field->name);
    add_attribute(input, attr);
    
    if (field->placeholder) {
        snprintf(attr, sizeof(attr), "placeholder=\"%s\"", field->placeholder);
        add_attribute(input, attr);
    }
    
    if (field->default_value) {
        snprintf(attr, sizeof(attr), "defaultValue=\"%s\"", field->default_value);
        add_attribute(input, attr);
    }
    
    if (field->required) {
        add_attribute(input, "required");
    }
    
    if (field->disabled) {
        add_attribute(input, "disabled");
    }
    
    if (field->readonly) {
        add_attribute(input, "readOnly");
    }
    
    add_attribute(input, "className=\"form-control\"");
    add_attribute(input, "style=\"width: 100%; padding: 10px; border: 1px solid #ddd; border-radius: 4px; font-size: 14px;\"");
    
    add_child(field_group, input);
    
    if (field->help_text) {
        UIElement *help = create_element("small");
        add_attribute(help, "className=\"form-text\"");
        add_attribute(help, "style=\"display: block; margin-top: 4px; color: #666; font-size: 12px;\"");
        add_child(help, create_text_node(field->help_text));
        add_child(field_group, help);
    }
    
    UIElement *error = create_element("div");
    add_attribute(error, "className=\"field-error\"");
    add_attribute(error, "style=\"color: #dc3545; font-size: 12px; margin-top: 4px; display: none;\"");
    add_child(field_group, error);
    
    return field_group;
}

/* Render Complete Form */
UIElement* render_form(FormBuilder *fb) {
    UIElement *form = create_element("form");
    
    char attr[512];
    snprintf(attr, sizeof(attr), "id=\"%s\"", fb->form_id);
    add_attribute(form, attr);
    
    if (fb->action) {
        snprintf(attr, sizeof(attr), "action=\"%s\"", fb->action);
        add_attribute(form, attr);
    }
    
    snprintf(attr, sizeof(attr), "method=\"%s\"", fb->method);
    add_attribute(form, attr);
    
    add_attribute(form, "onSubmit={(e) => { e.preventDefault(); handleSubmit(e); }}");
    
    FormField *current = fb->fields;
    while (current) {
        UIElement *field_elem = render_form_field(current);
        add_child(form, field_elem);
        current = current->next;
    }
    
    UIElement *button_group = create_element("div");
    add_attribute(button_group, "className=\"form-buttons\"");
    add_attribute(button_group, "style=\"margin-top: 24px; display: flex; gap: 12px;\"");
    
    UIElement *submit_btn = create_element("button");
    add_attribute(submit_btn, "type=\"submit\"");
    add_attribute(submit_btn, "className=\"btn btn-primary\"");
    add_attribute(submit_btn, "style=\"padding: 10px 24px; background: #007bff; color: white; border: none; border-radius: 4px; cursor: pointer; font-size: 14px;\"");
    add_child(submit_btn, create_text_node(fb->submit_label));
    add_child(button_group, submit_btn);
    
    UIElement *cancel_btn = create_element("button");
    add_attribute(cancel_btn, "type=\"button\"");
    add_attribute(cancel_btn, "className=\"btn btn-secondary\"");
    add_attribute(cancel_btn, "style=\"padding: 10px 24px; background: #6c757d; color: white; border: none; border-radius: 4px; cursor: pointer; font-size: 14px;\"");
    add_child(cancel_btn, create_text_node(fb->cancel_label));
    add_child(button_group, cancel_btn);
    
    add_child(form, button_group);
    
    return form;
}

/* Multi-Step Form */
typedef struct FormStep {
    char *title;
    char *description;
    FormField *fields;
    int field_count;
    struct FormStep *next;
} FormStep;

typedef struct {
    char *form_id;
    FormStep *steps;
    int step_count;
    int current_step;
    int show_progress;
} MultiStepForm;

MultiStepForm* create_multi_step_form(const char *form_id) {
    MultiStepForm *msf = malloc(sizeof(MultiStepForm));
    msf->form_id = strdup(form_id);
    msf->steps = NULL;
    msf->step_count = 0;
    msf->current_step = 0;
    msf->show_progress = 1;
    return msf;
}

FormStep* create_form_step(const char *title, const char *description) {
    FormStep *step = malloc(sizeof(FormStep));
    step->title = strdup(title);
    step->description = description ? strdup(description) : NULL;
    step->fields = NULL;
    step->field_count = 0;
    step->next = NULL;
    return step;
}

void add_step_to_form(MultiStepForm *msf, FormStep *step) {
    step->next = msf->steps;
    msf->steps = step;
    msf->step_count++;
}

UIElement* render_multi_step_form(MultiStepForm *msf) {
    UIElement *container = create_element("div");
    add_attribute(container, "className=\"multi-step-form\"");
    
    if (msf->show_progress) {
        UIElement *progress = create_element("div");
        add_attribute(progress, "className=\"form-progress\"");
        add_attribute(progress, "style=\"margin-bottom: 32px;\"");
        
        UIElement *progress_bar = create_element("div");
        add_attribute(progress_bar, "style=\"height: 4px; background: #e0e0e0; border-radius: 2px; overflow: hidden;\"");
        
        UIElement *progress_fill = create_element("div");
        char style[256];
        snprintf(style, sizeof(style), 
                 "height: 100%%; width: %.1f%%; background: #007bff; transition: width 0.3s ease;",
                 ((float)(msf->current_step + 1) / msf->step_count) * 100);
        char attr[512];
        snprintf(attr, sizeof(attr), "style=\"%s\"", style);
        add_attribute(progress_fill, attr);
        
        add_child(progress_bar, progress_fill);
        add_child(progress, progress_bar);
        
        UIElement *step_indicator = create_element("div");
        add_attribute(step_indicator, "style=\"margin-top: 8px; text-align: center; color: #666; font-size: 14px;\"");
        char step_text[128];
        snprintf(step_text, sizeof(step_text), "Step %d of %d", 
                 msf->current_step + 1, msf->step_count);
        add_child(step_indicator, create_text_node(step_text));
        add_child(progress, step_indicator);
        
        add_child(container, progress);
    }
    
    FormStep *current_step = msf->steps;
    for (int i = 0; i < msf->current_step && current_step; i++) {
        current_step = current_step->next;
    }
    
    if (current_step) {
        UIElement *step_container = create_element("div");
        add_attribute(step_container, "className=\"form-step\"");
        
        UIElement *step_title = create_element("h2");
        add_attribute(step_title, "style=\"margin-bottom: 8px; font-size: 24px;\"");
        add_child(step_title, create_text_node(current_step->title));
        add_child(step_container, step_title);
        
        if (current_step->description) {
            UIElement *step_desc = create_element("p");
            add_attribute(step_desc, "style=\"margin-bottom: 24px; color: #666;\"");
            add_child(step_desc, create_text_node(current_step->description));
            add_child(step_container, step_desc);
        }
        
        FormField *field = current_step->fields;
        while (field) {
            UIElement *field_elem = render_form_field(field);
            add_child(step_container, field_elem);
            field = field->next;
        }
        
        add_child(container, step_container);
    }
    
    UIElement *nav_buttons = create_element("div");
    add_attribute(nav_buttons, "className=\"step-navigation\"");
    add_attribute(nav_buttons, "style=\"margin-top: 32px; display: flex; justify-content: space-between;\"");
    
    if (msf->current_step > 0) {
        UIElement *prev_btn = create_element("button");
        add_attribute(prev_btn, "type=\"button\"");
        add_attribute(prev_btn, "onClick={() => previousStep()}");
        add_attribute(prev_btn, "style=\"padding: 10px 24px; background: #6c757d; color: white; border: none; border-radius: 4px; cursor: pointer;\"");
        add_child(prev_btn, create_text_node("Previous"));
        add_child(nav_buttons, prev_btn);
    }
    
    UIElement *next_btn = create_element("button");
    add_attribute(next_btn, "type=\"button\"");
    
    if (msf->current_step < msf->step_count - 1) {
        add_attribute(next_btn, "onClick={() => nextStep()}");
        add_child(next_btn, create_text_node("Next"));
    } else {
        add_attribute(next_btn, "onClick={() => submitForm()}");
        add_child(next_btn, create_text_node("Submit"));
    }
    
    add_attribute(next_btn, "style=\"padding: 10px 24px; background: #007bff; color: white; border: none; border-radius: 4px; cursor: pointer;\"");
    add_child(nav_buttons, next_btn);
    
    add_child(container, nav_buttons);
    
    return container;
}

/* Form Serialization */
char* generate_form_serialization(const char *form_id) {
    char *code = malloc(2048);
    snprintf(code, 2048,
        "const serializeForm = (formId) => {\n"
        "  const form = document.getElementById(formId);\n"
        "  const formData = new FormData(form);\n"
        "  const data = {};\n"
        "  for (let [key, value] of formData.entries()) {\n"
        "    data[key] = value;\n"
        "  }\n"
        "  return data;\n"
        "};\n"
        "\n"
        "const formData = serializeForm('%s');\n",
        form_id);
    return code;
}

/* Auto-save Form */
char* generate_autosave_form(const char *form_id, int interval_ms) {
    char *code = malloc(2048);
    snprintf(code, 2048,
        "useEffect(() => {\n"
        "  const autoSave = setInterval(() => {\n"
        "    const formData = serializeForm('%s');\n"
        "    localStorage.setItem('form_%s_autosave', JSON.stringify(formData));\n"
        "    console.log('Form auto-saved');\n"
        "  }, %d);\n"
        "  return () => clearInterval(autoSave);\n"
        "}, []);\n",
        form_id, form_id, interval_ms);
    return code;
}
