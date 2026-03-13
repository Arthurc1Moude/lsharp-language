/* ============================================================================
 * L# UI GRAMMAR RULES - PREMIUM UI SYSTEM SUPPORT
 * Advanced grammar rules for native UI development
 * ============================================================================
 */

%{
#include "ast.h"
#include "ui_renderer.h"

/* UI-specific AST node creation */
extern ASTNode* create_ui_component_node(const char *name, ASTNode *props, ASTNode *children);
extern ASTNode* create_style_node(ASTNode *properties);
extern ASTNode* create_animation_node(const char *name, ASTNode *keyframes);
extern ASTNode* create_layout_node(const char *type, ASTNode *config);
%}

/* ============================================================================
 * UI COMPONENT GRAMMAR
 * ============================================================================ */

/* Component Declaration with Props and State */
ui_component_declaration
    : COMPONENT TYPE_IDENTIFIER component_generics LBRACE component_body RBRACE
        { $ = create_ui_component($2, $3, $5); }
    | COMPONENT TYPE_IDENTIFIER LPAREN component_props RPAREN LBRACE component_body RBRACE
        { $ = create_ui_component_with_props($2, $4, $7); }
    ;

component_generics
    : LT generic_param_list GT
        { $ = $2; }
    | /* empty */
        { $ = NULL; }
    ;

component_props
    : prop_definition
        { $ = create_prop_list($1); }
    | component_props COMMA prop_definition
        { $ = append_prop($1, $3); }
    | /* empty */
        { $ = NULL; }
    ;

prop_definition
    : IDENTIFIER COLON type_annotation
        { $ = create_prop_def($1, $3, NULL, 0); }
    | IDENTIFIER COLON type_annotation ASSIGN expression
        { $ = create_prop_def($1, $3, $5, 0); }
    | IDENTIFIER QUESTION COLON type_annotation
        { $ = create_prop_def($1, $4, NULL, 1); }
    ;

component_body
    : component_member_list
        { $ = $1; }
    ;

component_member_list
    : component_member
        { $ = create_component_member_list($1); }
    | component_member_list component_member
        { $ = append_component_member($1, $2); }
    ;

component_member
    : state_declaration
        { $ = $1; }
    | effect_declaration
        { $ = $1; }
    | computed_property
        { $ = $1; }
    | event_handler
        { $ = $1; }
    | lifecycle_method
        { $ = $1; }
    | render_method
        { $ = $1; }
    | function_declaration
        { $ = $1; }
    ;

/* ============================================================================
 * STATE MANAGEMENT GRAMMAR
 * ============================================================================ */

state_declaration
    : STATE IDENTIFIER COLON type_annotation ASSIGN expression SEMICOLON
        { $ = create_state_decl($2, $4, $6); }
    | STATE IDENTIFIER ASSIGN expression SEMICOLON
        { $ = create_state_decl($2, NULL, $4); }
    | CONST LBRACKET IDENTIFIER COMMA IDENTIFIER RBRACKET ASSIGN USE_STATE LPAREN expression RPAREN SEMICOLON
        { $ = create_use_state($3, $5, $10); }
    ;

effect_declaration
    : USE_EFFECT LPAREN lambda_expression COMMA dependency_array RPAREN SEMICOLON
        { $ = create_use_effect($3, $5); }
    | USE_EFFECT LPAREN lambda_expression RPAREN SEMICOLON
        { $ = create_use_effect($3, NULL); }
    ;

dependency_array
    : LBRACKET expression_list RBRACKET
        { $ = $2; }
    | LBRACKET RBRACKET
        { $ = NULL; }
    ;

computed_property
    : COMPUTED IDENTIFIER COLON type_annotation LBRACE expression RBRACE
        { $ = create_computed_prop($2, $4, $6); }
    | COMPUTED IDENTIFIER LBRACE expression RBRACE
        { $ = create_computed_prop($2, NULL, $4); }
    ;

/* ============================================================================
 * JSX/UI ELEMENT GRAMMAR
 * ============================================================================ */

jsx_element
    : jsx_self_closing_element
        { $ = $1; }
    | jsx_opening_closing_element
        { $ = $1; }
    | jsx_fragment
        { $ = $1; }
    ;

jsx_self_closing_element
    : LT jsx_element_name jsx_attributes SLASH GT
        { $ = create_jsx_element($2, $3, NULL, 1); }
    ;

jsx_opening_closing_element
    : LT jsx_element_name jsx_attributes GT jsx_children LT SLASH jsx_element_name GT
        { $ = create_jsx_element($2, $3, $5, 0); }
    ;

jsx_fragment
    : LT GT jsx_children LT SLASH GT
        { $ = create_jsx_fragment($3); }
    ;

jsx_element_name
    : IDENTIFIER
        { $ = create_jsx_name($1, NULL); }
    | TYPE_IDENTIFIER
        { $ = create_jsx_name($1, NULL); }
    | jsx_element_name DOT IDENTIFIER
        { $ = create_jsx_namespaced_name($1, $3); }
    ;

jsx_attributes
    : jsx_attribute
        { $ = create_jsx_attr_list($1); }
    | jsx_attributes jsx_attribute
        { $ = append_jsx_attr($1, $2); }
    | /* empty */
        { $ = NULL; }
    ;

jsx_attribute
    : IDENTIFIER ASSIGN jsx_attribute_value
        { $ = create_jsx_attr($1, $3); }
    | IDENTIFIER
        { $ = create_jsx_attr($1, create_bool_literal(1)); }
    | LBRACE SPREAD expression RBRACE
        { $ = create_jsx_spread_attr($3); }
    ;

jsx_attribute_value
    : STRING
        { $ = create_string_literal($1); }
    | LBRACE expression RBRACE
        { $ = $2; }
    | jsx_element
        { $ = $1; }
    ;

jsx_children
    : jsx_child
        { $ = create_jsx_child_list($1); }
    | jsx_children jsx_child
        { $ = append_jsx_child($1, $2); }
    | /* empty */
        { $ = NULL; }
    ;

jsx_child
    : jsx_element
        { $ = $1; }
    | jsx_text
        { $ = $1; }
    | LBRACE expression RBRACE
        { $ = create_jsx_expression_child($2); }
    | LBRACE jsx_spread_child RBRACE
        { $ = $2; }
    ;

jsx_text
    : JSX_TEXT
        { $ = create_jsx_text($1); }
    ;

jsx_spread_child
    : SPREAD expression
        { $ = create_jsx_spread_child($2); }
    ;

/* ============================================================================
 * STYLE GRAMMAR
 * ============================================================================ */

style_object
    : LBRACE style_properties RBRACE
        { $ = create_style_object($2); }
    | LBRACE RBRACE
        { $ = create_style_object(NULL); }
    ;

style_properties
    : style_property
        { $ = create_style_prop_list($1); }
    | style_properties COMMA style_property
        { $ = append_style_prop($1, $3); }
    ;

style_property
    : IDENTIFIER COLON style_value
        { $ = create_style_prop($1, $3); }
    | STRING COLON style_value
        { $ = create_style_prop($1, $3); }
    ;

style_value
    : expression
        { $ = $1; }
    | gradient_expression
        { $ = $1; }
    | transform_expression
        { $ = $1; }
    | filter_expression
        { $ = $1; }
    ;

/* Gradient Expressions */
gradient_expression
    : LINEAR_GRADIENT LPAREN gradient_args RPAREN
        { $ = create_gradient("linear", $3); }
    | RADIAL_GRADIENT LPAREN gradient_args RPAREN
        { $ = create_gradient("radial", $3); }
    | CONIC_GRADIENT LPAREN gradient_args RPAREN
        { $ = create_gradient("conic", $3); }
    ;

gradient_args
    : gradient_direction COMMA color_stop_list
        { $ = create_gradient_args($1, $3); }
    | color_stop_list
        { $ = create_gradient_args(NULL, $1); }
    ;

gradient_direction
    : INTEGER IDENTIFIER
        { $ = create_gradient_direction($1, $2); }
    | TO IDENTIFIER
        { $ = create_gradient_direction_to($2); }
    ;

color_stop_list
    : color_stop
        { $ = create_color_stop_list($1); }
    | color_stop_list COMMA color_stop
        { $ = append_color_stop($1, $3); }
    ;

color_stop
    : expression
        { $ = create_color_stop($1, NULL); }
    | expression expression
        { $ = create_color_stop($1, $2); }
    ;

/* Transform Expressions */
transform_expression
    : TRANSFORM LPAREN transform_function_list RPAREN
        { $ = create_transform($3); }
    ;

transform_function_list
    : transform_function
        { $ = create_transform_list($1); }
    | transform_function_list transform_function
        { $ = append_transform($1, $2); }
    ;

transform_function
    : TRANSLATE LPAREN expression COMMA expression RPAREN
        { $ = create_transform_func("translate", $3, $5, NULL); }
    | SCALE LPAREN expression RPAREN
        { $ = create_transform_func("scale", $3, NULL, NULL); }
    | ROTATE LPAREN expression RPAREN
        { $ = create_transform_func("rotate", $3, NULL, NULL); }
    | SKEW LPAREN expression COMMA expression RPAREN
        { $ = create_transform_func("skew", $3, $5, NULL); }
    | MATRIX LPAREN expression_list RPAREN
        { $ = create_transform_func("matrix", $3, NULL, NULL); }
    ;

/* Filter Expressions */
filter_expression
    : FILTER LPAREN filter_function_list RPAREN
        { $ = create_filter($3); }
    ;

filter_function_list
    : filter_function
        { $ = create_filter_list($1); }
    | filter_function_list filter_function
        { $ = append_filter($1, $2); }
    ;

filter_function
    : BLUR LPAREN expression RPAREN
        { $ = create_filter_func("blur", $3); }
    | BRIGHTNESS LPAREN expression RPAREN
        { $ = create_filter_func("brightness", $3); }
    | CONTRAST LPAREN expression RPAREN
        { $ = create_filter_func("contrast", $3); }
    | SATURATE LPAREN expression RPAREN
        { $ = create_filter_func("saturate", $3); }
    | HUE_ROTATE LPAREN expression RPAREN
        { $ = create_filter_func("hue-rotate", $3); }
    ;

/* ============================================================================
 * ANIMATION GRAMMAR
 * ============================================================================ */

animation_declaration
    : ANIMATION IDENTIFIER LBRACE keyframe_list RBRACE
        { $ = create_animation_decl($2, $4); }
    ;

keyframe_list
    : keyframe
        { $ = create_keyframe_list($1); }
    | keyframe_list keyframe
        { $ = append_keyframe($1, $2); }
    ;

keyframe
    : keyframe_selector LBRACE style_properties RBRACE
        { $ = create_keyframe($1, $3); }
    ;

keyframe_selector
    : INTEGER PERCENT
        { $ = create_keyframe_selector($1); }
    | FROM
        { $ = create_keyframe_selector(0); }
    | TO
        { $ = create_keyframe_selector(100); }
    ;

/* Animation Properties */
animation_property
    : ANIMATION COLON animation_value_list SEMICOLON
        { $ = create_animation_prop($3); }
    ;

animation_value_list
    : animation_value
        { $ = create_animation_value_list($1); }
    | animation_value_list COMMA animation_value
        { $ = append_animation_value($1, $3); }
    ;

animation_value
    : IDENTIFIER expression expression IDENTIFIER
        { $ = create_animation_value($1, $2, $3, $4); }
    ;

/* ============================================================================
 * LAYOUT GRAMMAR
 * ============================================================================ */

layout_declaration
    : LAYOUT layout_type LBRACE layout_config RBRACE
        { $ = create_layout_decl($2, $4); }
    ;

layout_type
    : FLEX
        { $ = create_layout_type("flex"); }
    | GRID
        { $ = create_layout_type("grid"); }
    | STACK
        { $ = create_layout_type("stack"); }
    | ABSOLUTE
        { $ = create_layout_type("absolute"); }
    ;

layout_config
    : layout_property_list
        { $ = $1; }
    ;

layout_property_list
    : layout_property
        { $ = create_layout_prop_list($1); }
    | layout_property_list layout_property
        { $ = append_layout_prop($1, $2); }
    ;

layout_property
    : IDENTIFIER COLON expression SEMICOLON
        { $ = create_layout_prop($1, $3); }
    ;

/* Flexbox Specific */
flex_direction
    : FLEX_DIRECTION COLON flex_direction_value SEMICOLON
        { $ = create_flex_direction($3); }
    ;

flex_direction_value
    : ROW
        { $ = create_flex_dir_value("row"); }
    | COLUMN
        { $ = create_flex_dir_value("column"); }
    | ROW_REVERSE
        { $ = create_flex_dir_value("row-reverse"); }
    | COLUMN_REVERSE
        { $ = create_flex_dir_value("column-reverse"); }
    ;

/* Grid Specific */
grid_template
    : GRID_TEMPLATE_COLUMNS COLON grid_track_list SEMICOLON
        { $ = create_grid_template("columns", $3); }
    | GRID_TEMPLATE_ROWS COLON grid_track_list SEMICOLON
        { $ = create_grid_template("rows", $3); }
    ;

grid_track_list
    : grid_track
        { $ = create_grid_track_list($1); }
    | grid_track_list grid_track
        { $ = append_grid_track($1, $2); }
    ;

grid_track
    : expression
        { $ = create_grid_track($1); }
    | REPEAT LPAREN expression COMMA expression RPAREN
        { $ = create_grid_repeat($3, $5); }
    | MINMAX LPAREN expression COMMA expression RPAREN
        { $ = create_grid_minmax($3, $5); }
    | AUTO
        { $ = create_grid_auto(); }
    | FR
        { $ = create_grid_fr(); }
    ;

/* ============================================================================
 * EVENT HANDLER GRAMMAR
 * ============================================================================ */

event_handler
    : ON event_name ASSIGN event_handler_body
        { $ = create_event_handler($2, $4); }
    ;

event_name
    : IDENTIFIER
        { $ = $1; }
    | CLICK
        { $ = "click"; }
    | CHANGE
        { $ = "change"; }
    | SUBMIT
        { $ = "submit"; }
    | KEYDOWN
        { $ = "keydown"; }
    | KEYUP
        { $ = "keyup"; }
    | MOUSEENTER
        { $ = "mouseenter"; }
    | MOUSELEAVE
        { $ = "mouseleave"; }
    ;

event_handler_body
    : lambda_expression
        { $ = $1; }
    | IDENTIFIER
        { $ = create_identifier($1); }
    ;

/* ============================================================================
 * LIFECYCLE METHODS
 * ============================================================================ */

lifecycle_method
    : ON_MOUNT block
        { $ = create_lifecycle_method("mount", $2); }
    | ON_UNMOUNT block
        { $ = create_lifecycle_method("unmount", $2); }
    | ON_UPDATE block
        { $ = create_lifecycle_method("update", $2); }
    | BEFORE_RENDER block
        { $ = create_lifecycle_method("before_render", $2); }
    | AFTER_RENDER block
        { $ = create_lifecycle_method("after_render", $2); }
    ;

/* ============================================================================
 * RENDER METHOD
 * ============================================================================ */

render_method
    : RENDER block
        { $ = create_render_method($2); }
    | RENDER LBRACE jsx_element RBRACE
        { $ = create_render_method_jsx($3); }
    | RENDER LBRACE RETURN jsx_element SEMICOLON RBRACE
        { $ = create_render_method_jsx($4); }
    ;

/* ============================================================================
 * THEME GRAMMAR
 * ============================================================================ */

theme_declaration
    : THEME IDENTIFIER LBRACE theme_properties RBRACE
        { $ = create_theme_decl($2, $4); }
    ;

theme_properties
    : theme_property
        { $ = create_theme_prop_list($1); }
    | theme_properties theme_property
        { $ = append_theme_prop($1, $2); }
    ;

theme_property
    : IDENTIFIER COLON theme_value SEMICOLON
        { $ = create_theme_prop($1, $3); }
    | IDENTIFIER COLON LBRACE theme_properties RBRACE
        { $ = create_theme_nested_prop($1, $4); }
    ;

theme_value
    : expression
        { $ = $1; }
    | color_palette
        { $ = $1; }
    ;

color_palette
    : LBRACE color_palette_items RBRACE
        { $ = create_color_palette($2); }
    ;

color_palette_items
    : color_palette_item
        { $ = create_color_palette_list($1); }
    | color_palette_items COMMA color_palette_item
        { $ = append_color_palette_item($1, $3); }
    ;

color_palette_item
    : INTEGER COLON expression
        { $ = create_color_palette_item($1, $3); }
    ;

/* ============================================================================
 * RESPONSIVE DESIGN GRAMMAR
 * ============================================================================ */

media_query
    : AT_MEDIA media_query_list LBRACE style_properties RBRACE
        { $ = create_media_query($2, $4); }
    ;

media_query_list
    : media_query_item
        { $ = create_media_query_list($1); }
    | media_query_list AND media_query_item
        { $ = append_media_query($1, $3); }
    ;

media_query_item
    : LPAREN media_feature COLON expression RPAREN
        { $ = create_media_query_item($2, $4); }
    ;

media_feature
    : MIN_WIDTH
        { $ = "min-width"; }
    | MAX_WIDTH
        { $ = "max-width"; }
    | MIN_HEIGHT
        { $ = "min-height"; }
    | MAX_HEIGHT
        { $ = "max-height"; }
    ;

/* Breakpoint System */
breakpoint_declaration
    : BREAKPOINT IDENTIFIER COLON expression SEMICOLON
        { $ = create_breakpoint($2, $4); }
    ;

responsive_value
    : LBRACE responsive_value_items RBRACE
        { $ = create_responsive_value($2); }
    ;

responsive_value_items
    : responsive_value_item
        { $ = create_responsive_value_list($1); }
    | responsive_value_items COMMA responsive_value_item
        { $ = append_responsive_value($1, $3); }
    ;

responsive_value_item
    : IDENTIFIER COLON expression
        { $ = create_responsive_value_item($1, $3); }
    ;

/* ============================================================================
 * TRANSITION GRAMMAR
 * ============================================================================ */

transition_declaration
    : TRANSITION COLON transition_list SEMICOLON
        { $ = create_transition_decl($3); }
    ;

transition_list
    : transition_item
        { $ = create_transition_list($1); }
    | transition_list COMMA transition_item
        { $ = append_transition($1, $3); }
    ;

transition_item
    : IDENTIFIER expression expression IDENTIFIER
        { $ = create_transition_item($1, $2, $3, $4); }
    | IDENTIFIER expression
        { $ = create_transition_item($1, $2, NULL, NULL); }
    ;

/* ============================================================================
 * SHADOW GRAMMAR
 * ============================================================================ */

shadow_expression
    : BOX_SHADOW LPAREN shadow_list RPAREN
        { $ = create_shadow("box", $3); }
    | TEXT_SHADOW LPAREN shadow_list RPAREN
        { $ = create_shadow("text", $3); }
    ;

shadow_list
    : shadow_item
        { $ = create_shadow_list($1); }
    | shadow_list COMMA shadow_item
        { $ = append_shadow($1, $3); }
    ;

shadow_item
    : expression expression expression expression expression
        { $ = create_shadow_item($1, $2, $3, $4, $5); }
    | expression expression expression expression
        { $ = create_shadow_item($1, $2, $3, $4, NULL); }
    ;

/* ============================================================================
 * ACCESSIBILITY GRAMMAR
 * ============================================================================ */

aria_attribute
    : ARIA_LABEL ASSIGN expression
        { $ = create_aria_attr("label", $3); }
    | ARIA_DESCRIBEDBY ASSIGN expression
        { $ = create_aria_attr("describedby", $3); }
    | ARIA_HIDDEN ASSIGN expression
        { $ = create_aria_attr("hidden", $3); }
    | ARIA_LIVE ASSIGN expression
        { $ = create_aria_attr("live", $3); }
    ;

role_attribute
    : ROLE ASSIGN expression
        { $ = create_role_attr($3); }
    ;

tabindex_attribute
    : TABINDEX ASSIGN expression
        { $ = create_tabindex_attr($3); }
    ;

/* ============================================================================
 * CUSTOM HOOKS GRAMMAR
 * ============================================================================ */

custom_hook_declaration
    : USE IDENTIFIER LPAREN parameter_list RPAREN type_annotation block
        { $ = create_custom_hook($2, $4, $6, $7); }
    ;

hook_call
    : USE IDENTIFIER LPAREN argument_list RPAREN
        { $ = create_hook_call($2, $4); }
    ;

/* ============================================================================
 * CONTEXT API GRAMMAR
 * ============================================================================ */

context_declaration
    : CONTEXT IDENTIFIER ASSIGN CREATE_CONTEXT LPAREN expression RPAREN SEMICOLON
        { $ = create_context_decl($2, $6); }
    ;

context_provider
    : LT IDENTIFIER DOT PROVIDER jsx_attributes GT jsx_children LT SLASH IDENTIFIER DOT PROVIDER GT
        { $ = create_context_provider($2, $4, $6); }
    ;

use_context_call
    : USE_CONTEXT LPAREN IDENTIFIER RPAREN
        { $ = create_use_context($3); }
    ;

/* ============================================================================
 * PORTAL GRAMMAR
 * ============================================================================ */

portal_expression
    : CREATE_PORTAL LPAREN jsx_element COMMA expression RPAREN
        { $ = create_portal($3, $5); }
    ;

/* ============================================================================
 * REF GRAMMAR
 * ============================================================================ */

ref_declaration
    : CONST IDENTIFIER ASSIGN USE_REF LPAREN expression RPAREN SEMICOLON
        { $ = create_ref_decl($2, $6); }
    | CONST IDENTIFIER ASSIGN USE_REF LPAREN RPAREN SEMICOLON
        { $ = create_ref_decl($2, NULL); }
    ;

ref_attribute
    : REF ASSIGN IDENTIFIER
        { $ = create_ref_attr($3); }
    | REF ASSIGN LBRACE expression RBRACE
        { $ = create_ref_attr_expr($4); }
    ;

/* ============================================================================
 * MEMO GRAMMAR
 * ============================================================================ */

memo_declaration
    : CONST IDENTIFIER ASSIGN USE_MEMO LPAREN lambda_expression COMMA dependency_array RPAREN SEMICOLON
        { $ = create_memo_decl($2, $6, $8); }
    ;

callback_declaration
    : CONST IDENTIFIER ASSIGN USE_CALLBACK LPAREN lambda_expression COMMA dependency_array RPAREN SEMICOLON
        { $ = create_callback_decl($2, $6, $8); }
    ;

%%

/* End of UI Grammar Rules */
